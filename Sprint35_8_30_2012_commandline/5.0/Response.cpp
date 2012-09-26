               
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-97. All Rights Reserved.
*/           
      
#include "stdafx.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "api.h"
#include "attrib.h"
#include "Response.h"
#include "drc.h"
#include "ODBC_Lib.h"



HRESULT __CreateObject(LPOLESTR pszProgID, IDispatch FAR* FAR* ppdisp);

static BOOL API_Responses = FALSE;
static CStringList programList;

/******************************************************************************
* TurnOnResponse()
*/
short API::TurnOnResponse(LPCTSTR programID) 
{
   if (programID)
   {
      API_Responses = TRUE;
      if (!programList.Find(programID))
         programList.AddTail(programID);
      return RC_SUCCESS;
   }
   else
      return RC_GENERAL_ERROR;
}

/******************************************************************************
* TurnOffResponse()
*/
short API::TurnOffResponse() 
{
   API_Responses = FALSE;
   programList.RemoveAll();

   return RC_SUCCESS;
}

/******************************************************************************
* API::TurnOffResponseByID()
*/
short API::TurnOffResponseByID(LPCTSTR programID) 
{
   POSITION pos = programList.Find(programID);
   if (!pos)
      return RC_ITEM_NOT_FOUND;

   programList.RemoveAt(pos);

   API_Responses = !programList.IsEmpty();

   return RC_SUCCESS;
}

#define DELIM (char)5
/******************************************************************************
* API_EntitySelected()
*/
void API_EntitySelected(CCEtoODBDoc *doc)
{
   if (!API_Responses)
      return;

   SelectStruct *s = doc->getSelectStack().getAtLevel();

   CString strKeyword;
   CString strValue;

   Attrib* attrib;
   WORD netNameKeyword = doc->IsKeyWord(ATT_NETNAME, TRUE);

   if (s->getData()->getDataType() == T_INSERT)
   {
      switch (s->getData()->getInsert()->getInsertType())
      {
         case INSERTTYPE_VIA:
            strKeyword = "Via";
            strValue = "";

            if (s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(netNameKeyword, attrib))
            {
               int decimals = GetDecimals(doc->getSettings().getPageUnits());

               strValue.Format("%s (%+.*lf,%+.*lf)", attrib->getStringValue(),
                  decimals, s->getData()->getInsert()->getOriginX(), decimals, s->getData()->getInsert()->getOriginY());
            }
         break;

         case INSERTTYPE_PIN:
            {
               strKeyword = "Comp-Pin";

               if (s->getData()->getInsert()->getRefname())
                  strValue = s->getData()->getInsert()->getRefname();
               else
                  strValue = "";

               //if (doc->getSelectStackLevelIndex() > 0)
               //{
               //   SelectStruct *comp = doc->getSelectionAt(doc->getSelectStackLevelIndex() - 1);
               if (doc->getSelectStack().isLevelParentValid())
               {
                  SelectStruct* comp = doc->getSelectStack().getAtLevelParent();
                  strValue = "";

                  if (comp->getData()->getInsert()->getRefname())
                     strValue = comp->getData()->getInsert()->getRefname();

                  strValue += (char)DELIM;

                  if (s->getData()->getInsert()->getRefname())
                     strValue += s->getData()->getInsert()->getRefname();
               }
            }
         break;

         case INSERTTYPE_DRCMARKER:
         {
            POSITION filePos = doc->getFileList().GetHeadPosition();
            while (filePos != NULL)
            {
               FileStruct *file = doc->getFileList().GetNext(filePos);

               POSITION drcPos = file->getDRCList().GetHeadPosition();
               while (drcPos != NULL)
               {
                  DRCStruct *drc = file->getDRCList().GetNext(drcPos);

                  if (drc->getInsertEntityNumber() == s->getData()->getEntityNumber())
                  {
                     strKeyword = "DRC";
                     strValue.Format("%ld", drc->getEntityNumber());
                     break;
                  }
               }
            }
         }
         break;

         default:
            strKeyword = "RefDes";
            strValue = "";
            if (s->getData()->getInsert()->getRefname())
               strValue = s->getData()->getInsert()->getRefname();
         break;
      }
   }
   else if (s->getData()->getDataType() == T_POLY && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(netNameKeyword, attrib))
   {
      strKeyword = "NetName";
      strValue = attrib->getStringValue();
   }
   else
   {
      strKeyword = "";
      strValue = "";
   }

   SendResponse(strKeyword, strValue);
}


/******************************************************************************
* SendResponse()
*/
void SendResponse(CString strKeyword, CString strValue)
{
   if (!API_Responses)
      return;

   BSTR keyword = strKeyword.AllocSysString();
   BSTR value = strValue.AllocSysString();

   POSITION pos = programList.GetHeadPosition();
   while (pos)
   {
      CString program = programList.GetNext(pos);

      LPDISPATCH pdispApp; 
      if (__CreateObject(program.AllocSysString(), &pdispApp))
         continue;

      OLECHAR FAR* szName = OLESTR("Response");
      DISPID dispid; 
      pdispApp->GetIDsOfNames(IID_NULL, &szName, 1, LOCALE_USER_DEFAULT, &dispid); 

      DISPPARAMS dispparams;
      dispparams.cArgs = 2;
      dispparams.cNamedArgs = 0;
      dispparams.rgdispidNamedArgs = NULL;
      dispparams.rgvarg = new VARIANTARG[2];
      memset(dispparams.rgvarg, 0, sizeof(VARIANTARG) * 2);

      dispparams.rgvarg[0].vt = VT_BSTR;
      dispparams.rgvarg[0].bstrVal = value;
      dispparams.rgvarg[1].vt = VT_BSTR;
      dispparams.rgvarg[1].bstrVal = keyword;

      pdispApp->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, 
            &dispparams, NULL, NULL, NULL);

      delete dispparams.rgvarg;
   }

   SysFreeString(keyword);
   SysFreeString(value);
}



/* 
 * This file contains helper functions that make it simple to create late-binding 
 * automation controllers. CreateObject() will create an automation object and 
 * Invoke() will call a property or method of the automation object.  
 * 
 * For example, the following VB code will control Microsoft Word: 
 *     
 *    Private Sub Form_Load() 
 *    Dim wb As Object 
 *    Set wb = CreateObject("Word.Basic") 
 *    wb.AppShow 
 *    wb.FileNewDefault 
 *    wb.Insert "This is a test" 
 *    wb.FileSaveAs "c:\sample.doc)" 
 *    End Sub 
 *    
 * A C++ automation controller that does the same can be written as follows using 
 * the helper functions: 
 *  
 *   LPDISPATCH pdispWord; 
 *   CreateObject(OLESTR("Word.Basic"), &pdispWord); 
 *   Invoke(pdispWord, DISPATCH_METHOD, NULL, NULL, NULL, 
 *              OLESTR("AppShow"), NULL);    
 *   Invoke(pdispWord, DISPATCH_METHOD, NULL, NULL, NULL, 
 *              OLESTR("FileNewDefault"), NULL);  
 *   Invoke(pdispWord, DISPATCH_METHOD, NULL, NULL, NULL,  
 *              OLESTR("Insert"), TEXT("s"), (LPOLESTR)OLESTR("This is a test")); 
 *   Invoke(pdispWord, DISPATCH_METHOD, NULL, NULL, NULL,  
 *              OLESTR("FileSaveAs"), TEXT("s"), OLESTR("c:\\sample.doc")); 
 *   pdispWord->Release(); 
 *   
 * See the header comments of CreateObject and Invoke for documentation to use 
 * these functions. 
 * 
 */ 

/* 
 * CreateObject 
 * 
 * Purpose: 
 *  Creates an instance of the Automation object and obtains it's IDispatch interface. 
 *  Uses Unicode with OLE. 
 * 
 * Parameters: 
 *  pszProgID         ProgID of Automation object 
 *  ppdisp             Returns IDispatch of Automation object 
 * 
 * Return Value: 
 *  HRESULT indicating success or failure  
 */ 
HRESULT __CreateObject(LPOLESTR pszProgID, IDispatch FAR* FAR* ppdisp) 
{ 
    CLSID clsid;                   // CLSID of automation object 
    HRESULT hr; 
    LPUNKNOWN punk = NULL;         // IUnknown of automation object 
    LPDISPATCH pdisp = NULL;       // IDispatch of automation object 
     
    *ppdisp = NULL; 
     
    // Retrieve CLSID from the progID that the user specified 
    hr = CLSIDFromProgID(pszProgID, &clsid); 
    if (FAILED(hr)) 
        goto error; 
     
    // Create an instance of the automation object and ask for the IDispatch interface 
    hr = CoCreateInstance(clsid, NULL, CLSCTX_SERVER,  
                          IID_IUnknown, (void FAR* FAR*)&punk); 
    if (FAILED(hr)) 
        goto error; 
                    
    hr = punk->QueryInterface(IID_IDispatch, (void FAR* FAR*)&pdisp); 
    if (FAILED(hr)) 
        goto error; 
 
    *ppdisp = pdisp; 
    punk->Release(); 
    return NOERROR; 
      
error: 
    if (punk) punk->Release(); 
    if (pdisp) pdisp->Release(); 
    return hr; 
}   
 
/*HRESULT CountArgsInFormat(LPCTSTR pszFmt, UINT FAR *pn); 
LPCTSTR GetNextVarType(LPCTSTR pszFmt, VARTYPE FAR* pvt); 

/* 
 * Invoke 
 * 
 * Purpose: 
 *  Invokes a property accessor function or method of an automation object. Uses Unicode with OLE. 
 * 
 * Parameters: 
 *  pdisp         IDispatch* of automation object. 
 *  wFlags        Specfies if property is to be accessed or method to be invoked. 
 *                Can hold DISPATCH_PROPERTYGET, DISPATCH_PROPERTYPUT, DISPATCH_METHOD, 
 *                DISPATCH_PROPERTYPUTREF or DISPATCH_PROPERTYGET|DISPATCH_METHOD.    
 *  pvRet         NULL if caller excepts no result. Otherwise returns result. 
 *  pexcepinfo    Returns exception info if DISP_E_EXCEPTION is returned. Can be NULL if 
 *                caller is not interested in exception information.  
 *  pnArgErr      If return is DISP_E_TYPEMISMATCH, this returns the index (in reverse 
 *                order) of argument with incorrect type. Can be NULL if caller is not interested 
 *                in this information.  
 *  pszName       Name of property or method. 
 *  pszFmt        Format string that describes the variable list of parameters that  
 *                follows. The format string can contain the follwoing characters. 
 *                & = mark the following format character as VT_BYREF  
 *                b = VT_BOOL 
 *                i = VT_I2 
 *                I = VT_I4 
 *                r = VT_R2 
 *                R = VT_R4 
 *                c = VT_CY  
 *                s = VT_BSTR (far string pointer can be passed, BSTR will be allocated by this function). 
 *                e = VT_ERROR 
 *                d = VT_DATE 
 *                v = VT_VARIANT. Use this to pass data types that are not described in  
 *                                the format string. (For example SafeArrays). 
 *                D = VT_DISPATCH 
 *                U = VT_UNKNOWN 
 *     
 *  ...           Arguments of the property or method. Arguments are described by pszFmt.   
 *                ****FAR POINTERS MUST BE PASSED FOR POINTER ARGUMENTS in Win16.**** 
 * Return Value: 
 *  HRESULT indicating success or failure         
 * 
 * Usage examples: 
 * 
 *  HRESULT hr;   
 *  LPDISPATCH pdisp;    
 *  BSTR bstr; 
 *  short i; 
 *  BOOL b;    
 *  VARIANT v, v2; 
 * 
 *1. bstr = SysAllocString(OLESTR("")); 
 *   hr = Invoke(pdisp, DISPATCH_METHOD, NULL, NULL, NULL, OLESTR("method1"),  
 *        TEXT("bis&b&i&s"), TRUE, 2, (LPOLESTR)OLESTR("param"), (BOOL FAR*)&b, (short FAR*)&i, (BSTR FAR*)&bstr);    
 * 
 *2. VariantInit(&v); 
 *   V_VT(&v) = VT_R8; 
 *   V_R8(&v) = 12345.6789;  
 *   VariantInit(&v2); 
 *   hr = Invoke(pdisp, DISPATCH_METHOD, NULL, NULL, NULL, OLESTR("method2"),  
 *         TEXT("v&v"), v, (VARIANT FAR*)&v2); 
 * 
HRESULT  
Invoke(LPDISPATCH pdisp,  
    WORD wFlags, 
    LPVARIANT pvRet, 
    EXCEPINFO FAR* pexcepinfo, 
    UINT FAR* pnArgErr,  
    LPOLESTR pszName, 
    LPCTSTR pszFmt,  
    ...) 
{ 
    va_list argList; 
    va_start(argList, pszFmt);   
    DISPID dispid; 
    HRESULT hr; 
    VARIANTARG* pvarg = NULL; 
   
    if (pdisp == NULL) 
        return E_INVALIDARG; 
     
    // Get DISPID of property/method 
    hr = pdisp->GetIDsOfNames(IID_NULL, &pszName, 1, LOCALE_USER_DEFAULT, &dispid); 
    if(FAILED(hr)) 
        return hr; 
                
    DISPPARAMS dispparams; 
    _fmemset(&dispparams, 0, sizeof dispparams); 
 
    // determine number of arguments 
    if (pszFmt != NULL) 
        CountArgsInFormat(pszFmt, &dispparams.cArgs); 
     
    // Property puts have a named argument that represents the value that the property is 
    // being assigned. 
    DISPID dispidNamed = DISPID_PROPERTYPUT; 
    if (wFlags & DISPATCH_PROPERTYPUT) 
    { 
        if (dispparams.cArgs == 0) 
            return E_INVALIDARG; 
        dispparams.cNamedArgs = 1; 
        dispparams.rgdispidNamedArgs = &dispidNamed; 
    } 
 
    if (dispparams.cArgs != 0) 
    { 
        // allocate memory for all VARIANTARG parameters 
        pvarg = new VARIANTARG[dispparams.cArgs]; 
        if(pvarg == NULL) 
            return E_OUTOFMEMORY;    
        dispparams.rgvarg = pvarg; 
        _fmemset(pvarg, 0, sizeof(VARIANTARG) * dispparams.cArgs); 
 
        // get ready to walk vararg list 
        LPCTSTR psz = pszFmt; 
        pvarg += dispparams.cArgs - 1;   // params go in opposite order 
         
        while (psz = GetNextVarType(psz, &pvarg->vt)) 
        { 
            if (pvarg < dispparams.rgvarg) 
            { 
                hr = E_INVALIDARG; 
                goto cleanup;   
            } 
            switch (pvarg->vt) 
            { 
            case VT_I2: 
                V_I2(pvarg) = va_arg(argList, short); 
                break; 
            case VT_I4: 
                V_I4(pvarg) = va_arg(argList, long); 
                break; 
            case VT_R4: 
                V_R4(pvarg) = va_arg(argList, float); 
                break;  
            case VT_DATE: 
            case VT_R8: 
                V_R8(pvarg) = va_arg(argList, double); 
                break; 
            case VT_CY: 
                V_CY(pvarg) = va_arg(argList, CY); 
                break; 
            case VT_BSTR: 
                V_BSTR(pvarg) = SysAllocString(va_arg(argList, OLECHAR FAR*)); 
                if (pvarg->bstrVal == NULL)  
                { 
                    hr = E_OUTOFMEMORY;   
                    pvarg->vt = VT_EMPTY; 
                    goto cleanup;   
                } 
                break; 
            case VT_DISPATCH: 
                V_DISPATCH(pvarg) = va_arg(argList, LPDISPATCH); 
                break; 
            case VT_ERROR: 
                V_ERROR(pvarg) = va_arg(argList, SCODE); 
                break; 
            case VT_BOOL: 
                V_BOOL(pvarg) = va_arg(argList, BOOL) ? -1 : 0; 
                break; 
            case VT_VARIANT: 
                *pvarg = va_arg(argList, VARIANTARG);  
                break; 
            case VT_UNKNOWN: 
                V_UNKNOWN(pvarg) = va_arg(argList, LPUNKNOWN); 
                break; 
 
            case VT_I2|VT_BYREF: 
                V_I2REF(pvarg) = va_arg(argList, short FAR*); 
                break; 
            case VT_I4|VT_BYREF: 
                V_I4REF(pvarg) = va_arg(argList, long FAR*); 
                break; 
            case VT_R4|VT_BYREF: 
                V_R4REF(pvarg) = va_arg(argList, float FAR*); 
                break; 
            case VT_R8|VT_BYREF: 
                V_R8REF(pvarg) = va_arg(argList, double FAR*); 
                break; 
            case VT_DATE|VT_BYREF: 
                V_DATEREF(pvarg) = va_arg(argList, DATE FAR*); 
                break; 
            case VT_CY|VT_BYREF: 
                V_CYREF(pvarg) = va_arg(argList, CY FAR*); 
                break; 
            case VT_BSTR|VT_BYREF: 
                V_BSTRREF(pvarg) = va_arg(argList, BSTR FAR*); 
                break; 
            case VT_DISPATCH|VT_BYREF: 
                V_DISPATCHREF(pvarg) = va_arg(argList, LPDISPATCH FAR*); 
                break; 
            case VT_ERROR|VT_BYREF: 
                V_ERRORREF(pvarg) = va_arg(argList, SCODE FAR*); 
                break; 
            case VT_BOOL|VT_BYREF:  
                { 
                    BOOL FAR* pbool = va_arg(argList, BOOL FAR*); 
                    *pbool = 0; 
                    V_BOOLREF(pvarg) = (VARIANT_BOOL FAR*)pbool; 
                }  
                break;               
            case VT_VARIANT|VT_BYREF:  
                V_VARIANTREF(pvarg) = va_arg(argList, VARIANTARG FAR*); 
                break; 
            case VT_UNKNOWN|VT_BYREF: 
                V_UNKNOWNREF(pvarg) = va_arg(argList, LPUNKNOWN FAR*); 
                break; 
 
            default: 
                { 
                    hr = E_INVALIDARG; 
                    goto cleanup;   
                } 
                break; 
            } 
 
            --pvarg; // get ready to fill next argument 
        } //while 
    } //if 
     
    // Initialize return variant, in case caller forgot. Caller can pass NULL if return 
    // value is not expected. 
    if (pvRet) 
        VariantInit(pvRet);  
    // make the call  
    hr = pdisp->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, wFlags, 
        &dispparams, pvRet, pexcepinfo, pnArgErr); 
 
cleanup: 
    // cleanup any arguments that need cleanup 
    if (dispparams.cArgs != 0) 
    { 
        VARIANTARG FAR* pvarg = dispparams.rgvarg; 
        UINT cArgs = dispparams.cArgs;    
         
        while (cArgs--) 
        { 
            switch (pvarg->vt) 
            { 
            case VT_BSTR: 
                VariantClear(pvarg); 
                break; 
            } 
            ++pvarg; 
        } 
    } 
    delete dispparams.rgvarg; 
    va_end(argList); 
    return hr;    
}    
 
HRESULT CountArgsInFormat(LPCTSTR pszFmt, UINT FAR *pn) 
{ 
    *pn = 0; 
 
    if(pszFmt == NULL) 
      return NOERROR; 
     
    while (*pszFmt)   
    { 
       if (*pszFmt == '&') 
           pszFmt++; 
 
       switch(*pszFmt) 
       { 
           case 'b': 
           case 'i':  
           case 'I': 
           case 'r':  
           case 'R': 
           case 'c': 
           case 's': 
           case 'e': 
           case 'd': 
           case 'v': 
           case 'D': 
           case 'U': 
               ++*pn;  
               pszFmt++; 
               break; 
           case '\0':   
           default: 
               return E_INVALIDARG;    
        } 
    } 
    return NOERROR; 
} 
 
 
LPCTSTR GetNextVarType(LPCTSTR pszFmt, VARTYPE FAR* pvt) 
{    
    *pvt = 0; 
    if (*pszFmt == '&')  
    { 
         *pvt = VT_BYREF;  
         pszFmt++;     
         if (!*pszFmt) 
             return NULL;     
    }  
    switch(*pszFmt) 
    { 
        case 'b': 
            *pvt |= VT_BOOL; 
            break; 
        case 'i':  
            *pvt |= VT_I2; 
            break; 
        case 'I':  
            *pvt |= VT_I4; 
            break; 
        case 'r':  
            *pvt |= VT_R4; 
            break; 
        case 'R':  
            *pvt |= VT_R8; 
            break; 
        case 'c': 
            *pvt |= VT_CY; 
            break; 
        case 's':  
            *pvt |= VT_BSTR; 
            break; 
        case 'e':  
            *pvt |= VT_ERROR; 
            break; 
        case 'd':  
            *pvt |= VT_DATE;  
            break; 
        case 'v':  
            *pvt |= VT_VARIANT; 
            break; 
        case 'U':  
            *pvt |= VT_UNKNOWN;  
            break; 
        case 'D':  
            *pvt |= VT_DISPATCH; 
            break;   
        case '\0': 
             return NULL;     // End of Format string 
        default: 
            return NULL; 
    }  
    return ++pszFmt;   
} 
*/

// sample code to call external applications.  Moustafa Noureddine, 2/17/2005
//#import "C:\\development\\Live Update\\RSI_LiveUpdate.exe"

#if defined(EnablePartLibrary)


bool PartLibTableExists(CString TableName) 
{
   bool tableExists = false;

   CDBInterface db;
   CString dbname( db.GetLibraryDatabaseName() );
   if (db.Connect(dbname, "", ""))
   {
      tableExists = db.TableExists(TableName);
   }

   // CBDInterface destructor will take care of disconnecting DB, we're done here.

   return tableExists;
}

bool PartLibCreateTable(CString TableName, CString ColumnNames) 
{
   bool tablecreated = false;

   if (!TableName.IsEmpty() && !ColumnNames.IsEmpty())
   {
      //CString recordDelimiter("\x05");
      CString fieldDelimiter("\x06");   

      CSupString fieldStr(ColumnNames);
      CStringArray columnNamesAry;
      fieldStr.ParseQuote(columnNamesAry, fieldDelimiter);
      
      if (columnNamesAry.GetCount() > 0)
      {
         CDBInterface db;
         CString dbname( db.GetLibraryDatabaseName() );
         if (db.Connect(dbname, "", ""))
         {
            if (db.CreateTable(TableName, columnNamesAry))
               tablecreated = true;
         }
      }
   }

   return tablecreated;
}

void SetStencilRules2Data(const CString& Entity,
   const CString& Designator1, const CString& Designator2, const CString& Designator3, const CString& Designator4, const CString& Designator5,
   const CString& RulesString)
{   
   // EXAMPLE SetStencilRulesData("Geom", "1234", "", "GeomRule|data|moreData|..")

   bool retval = true;

   CDBInterface db;
   CString dbname( db.GetLibraryDatabaseName() );

   if (db.Connect(dbname, "", ""))
   {
      CString UserName = db.GetCurrentUserName();
      CString DateTime = db.GetCurrentTimeStamp();

      CDBStencilRule2 rule(Entity, Designator1, Designator2, Designator3, Designator4, Designator5, RulesString, UserName, DateTime, ""/*comment*/, NULL);
      if (!db.SaveStencilRule(rule))
         retval = false;
   }

   //return retval;
}


void SetStencilRules2DataSet(const CString& RulesString)
{
   bool retval = true;

   CDBInterface db;
   CString dbname( db.GetLibraryDatabaseName() );

   if (db.Connect(dbname, "", ""))
   {
      CString recordDelimiter("\x05");
      CString fieldDelimiter("\x06");   

      CSupString valSetStr(RulesString);
      CStringArray recordAry;
     //alSetStr.ParseQuote(recordAry, recordDelimiter);
      valSetStr.Parse(recordAry, recordDelimiter);

      for (int i = 0; i < recordAry.GetCount(); i++)
      {
         CSupString record(recordAry.GetAt(i));
         CStringArray fieldAry;
         record.Parse(fieldAry, fieldDelimiter);

         CString entity, designator1, designator2, designator3, designator4, designator5, rulestr, comment;

         if (fieldAry.GetCount() > 0)
            entity = fieldAry.GetAt(0);

         if (!entity.IsEmpty())
         {
            if (fieldAry.GetCount() > 1)
               designator1 = fieldAry.GetAt(1);

            if (fieldAry.GetCount() > 2)
               designator2 = fieldAry.GetAt(2);

            if (fieldAry.GetCount() > 3)
               designator3 = fieldAry.GetAt(3);

            if (fieldAry.GetCount() > 4)
               designator4 = fieldAry.GetAt(4);

            if (fieldAry.GetCount() > 5)
               designator5 = fieldAry.GetAt(5);

            if (fieldAry.GetCount() > 6)
               rulestr = fieldAry.GetAt(6);

            if (fieldAry.GetCount() > 7)
               comment = fieldAry.GetAt(7);

            CString UserName = db.GetCurrentUserName();
            CString DateTime = db.GetCurrentTimeStamp();

            CDBStencilRule2 rule(entity, designator1, designator2, designator3, designator4, designator5, rulestr, UserName, DateTime, comment, NULL);

            if (!db.SaveStencilRule(rule))
               retval = false;
         }
      }
   }

   //return retval;

}

void GetStencilRules2Data(CString& out_RulesString, const CString& Entity, const CString& Designator1, const CString& Designator2)
{
   CString ruleSetReturnVal;

   CDBInterface db;
   CString dbname( db.GetLibraryDatabaseName() );

   if (db.Connect(dbname, "", ""))
   {
      // For this lookup we want all records of given entity/designator1/designator2, regardless of designator3, designator4, designator5
      CDBStencilRule2 *rule = db.LookupStencilRule(Entity, Designator1, Designator2);

      CString recordDelimiter("\x05");
      CString fieldDelimiter("\x06");   

      while (rule != NULL)
      {
         if (!ruleSetReturnVal.IsEmpty())
            ruleSetReturnVal += recordDelimiter;

         ruleSetReturnVal += rule->getEntity() + fieldDelimiter +
            rule->getDesignator1() + fieldDelimiter +
            rule->getDesignator2() + fieldDelimiter +
            rule->getDesignator3() + fieldDelimiter +
            rule->getDesignator4() + fieldDelimiter +
            rule->getDesignator5() + fieldDelimiter +
            rule->getRule() + fieldDelimiter +
            rule->getComment();
         
         CDBStencilRule2 *trash = rule;
         rule = rule->getNext();
         delete trash;
      }

   }

   out_RulesString = ruleSetReturnVal;

}

void GetStencilRules2DataSet(CString& out_RulesString,const CString& entityAndDesignatorsSet)
{
   /*
   * This is the query from ccpartslib.exe, it seems odd to me, the part about
   * the Designator Suffix being checked. I would expect that to be Prefix.
   * The ODBC version is being coded to mach what was in place in ccpartslib.
   * Kurt has been queried as to whether this really looks right to him.
   * If you end up with problems because of this query, well, it was always wrong.
   *
   cmd = "SELECT *  FROM STENCILRULES WHERE Entity = '" & rowSetArr(0) & "' "

   If rowSetArr(1) <> "" Then
   cmd = cmd & " AND DesignatorPrefix = '" & rowSetArr(1) & "'"
   Else
   cmd = cmd & " AND (DesignatorSuffix = '' or DesignatorSuffix is NULL)"
   End If
   */

   CString ruleSetReturnVal;

   CDBInterface db;
   CString dbname( db.GetLibraryDatabaseName() );

   if (db.Connect(dbname, "", ""))
   {
      CString recordDelimiter("\x05");
      CString fieldDelimiter("\x06");

      CSupString entityList(entityAndDesignatorsSet);
      CStringArray inputArgs;
      entityList.ParseQuote(inputArgs, recordDelimiter);

      for (int irow = 0; irow < inputArgs.GetCount(); irow++)
      {
         CSupString row(inputArgs.GetAt(irow));
         CStringArray argSet;
         row.ParseQuote(argSet, fieldDelimiter);

         if (argSet.GetCount() > 0)
         {
            CString entity = argSet.GetAt(0);

            CString designator1;

            if (argSet.GetCount() > 1)
               designator1 = argSet.GetAt(1);

            CString designator2;

            if (argSet.GetCount() > 2)
               designator2 = argSet.GetAt(2);

            // For this lookup we want all records of given entity/designator1/designator2, regardless of designator3, designator4, designator5
            CDBStencilRule2* rule = db.LookupStencilRule(entity, designator1, designator2);

            CString recordDelimiter("\x05");
            CString fieldDelimiter("\x06");   

            while (rule != NULL)
            {
               if (!ruleSetReturnVal.IsEmpty())
               {
                  ruleSetReturnVal += recordDelimiter;
               }

               CString ruleString = rule->getEntity()      + fieldDelimiter +
                                    rule->getDesignator1() + fieldDelimiter +
                                    rule->getDesignator2() + fieldDelimiter +
                                    rule->getDesignator3() + fieldDelimiter +
                                    rule->getDesignator4() + fieldDelimiter +
                                    rule->getDesignator5() + fieldDelimiter +
                                    rule->getRule()        + fieldDelimiter +
                                    rule->getComment();

               ruleSetReturnVal += ruleString;

               CDBStencilRule2* trash = rule;
               rule = rule->getNext();

               delete trash;
            }
         }
      }
   }

   out_RulesString = ruleSetReturnVal;
}

void migrateStencilData2()
{
   CDBInterface db;
   CString dbname( db.GetLibraryDatabaseName() );

   if (db.Connect(dbname, "", ""))
   {
      db.migrateStencilData2();
   }
}

void SetStencilSetting(const CString& settingName,const CString& settingValue)
{
   bool retval = true;

   CDBInterface db;
   CString dbname( db.GetLibraryDatabaseName() );
   if (db.Connect(dbname, "", ""))
   {
      if (!settingName.IsEmpty())
      {
         CDBStencilSetting setting(settingName, settingValue, NULL);
         if (!db.SaveStencilSetting(setting))
            retval = false;
      }
   }

   //return retval;
}

void GetStencilSetting(CString& settingValue,const CString& settingName)
{
   settingValue = "";

   CDBInterface db;
   CString dbname( db.GetLibraryDatabaseName() );
   if (db.Connect(dbname, "", ""))
   {
      if (!settingName.IsEmpty())
      {
         CDBStencilSetting *dbSetting = db.LookupStencilSetting(settingName);
         if (dbSetting != NULL)
         {
            settingValue = dbSetting->getValue();

            delete dbSetting;
            dbSetting = NULL;
         }
      }
   }

   //return partSetReturnVal;
}

void GetStencilSettings(CString& settingValues)
{
   settingValues = "";

   CString settingsSetReturnVal;

   CDBInterface db;
   CString dbname( db.GetLibraryDatabaseName() );
   if (db.Connect(dbname, "", ""))
   {
      CDBStencilSetting *setting = db.LookupStencilSetting(""); // all settings

      CString recordDelimiter("\x05");
      CString fieldDelimiter("\x06");   

      while (setting != NULL)
      {
         if (!settingsSetReturnVal.IsEmpty())
            settingsSetReturnVal += recordDelimiter;

         settingsSetReturnVal += setting->getName() + fieldDelimiter + setting->getValue();
         
         CDBStencilSetting *trash = setting;
         setting = setting->getNext();
         delete trash;
      }

   }

   settingValues = settingsSetReturnVal;
}

#endif  // defined(EnablePartLibrary)


#if defined(EnableSonyAoiLibrary)
#import "SonyAOI.exe"
//#import "c:\development\camcad\4.5\SonyAOI.exe"

void GetAllCatNames(CString DBName, CString DBPassword, CString& out_CatNames)
{
   BSTR lDBName = DBName.AllocSysString();
	BSTR lDBPassword = DBPassword.AllocSysString();
	BSTR l_out_CatNames = out_CatNames.AllocSysString();

	IClassFactory * Ic2 = NULL;
	SonyAOI::_CSonyDB  * m_COMobj;
	HRESULT hr1, hr2;
	hr1 = CoGetClassObject(	__uuidof(SonyAOI::CSonyDB), CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&Ic2);

	bool res1 = false;
	bool res2 = false;

	if (SUCCEEDED(hr1))
	{
		res1 = true;
				
		hr2 = Ic2->CreateInstance(NULL, __uuidof(SonyAOI::_CSonyDB), (void**)&m_COMobj);
		Ic2->Release();

		res2 = (SUCCEEDED(hr2));
		
		m_COMobj->GetAllCatNames (&lDBName, &lDBPassword, &l_out_CatNames);
		m_COMobj->Release();
		out_CatNames = l_out_CatNames;
	}
	
   ::SysFreeString(lDBName);
}
void CreateDB(CString DBName, CString DBPassword, CString DBDest)
{
	
   BSTR lDBName = DBName.AllocSysString();
	BSTR lDBPassword = DBPassword.AllocSysString();
	BSTR lDBDest = DBDest.AllocSysString();

	IClassFactory * Ic2 = NULL;
	SonyAOI::_CSonyDB  * m_COMobj;
	HRESULT hr1, hr2;
	hr1 = CoGetClassObject(	__uuidof(SonyAOI::CSonyDB), CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&Ic2);

	bool res1 = false;
	bool res2 = false;

	if (SUCCEEDED(hr1))
	{
		res1 = true;
				
		hr2 = Ic2->CreateInstance(NULL, __uuidof(SonyAOI::_CSonyDB), (void**)&m_COMobj);
		Ic2->Release();

		res2 = (SUCCEEDED(hr2));
		
		m_COMobj->CreateDB(&lDBName, &lDBPassword, &lDBDest) ;
		m_COMobj->Release();
	}

   ::SysFreeString(lDBName);
	::SysFreeString(lDBPassword);
	::SysFreeString(lDBDest);
}
void CreateAndCopyTableLibPro(CString DBName, CString DBPassword, CString SourceCat, CString SourceMod, CString DBDest, CString DestCat, CString DestMod)
{
	
   BSTR lDBName = DBName.AllocSysString();
	BSTR lDBPassword = DBPassword.AllocSysString();
	BSTR lDBDest = DBDest.AllocSysString();
	BSTR lSourceCat = SourceCat.AllocSysString();
	BSTR lSourceMod = SourceMod.AllocSysString();
	BSTR lDestCat = DestCat.AllocSysString();
	BSTR lDestMod = DestMod.AllocSysString();


	IClassFactory * Ic2 = NULL;
	SonyAOI::_CSonyDB  * m_COMobj;
	HRESULT hr1, hr2;
	hr1 = CoGetClassObject(	__uuidof(SonyAOI::CSonyDB), CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&Ic2);

	bool res1 = false;
	bool res2 = false;

	if (SUCCEEDED(hr1))
	{
		res1 = true;
				
		hr2 = Ic2->CreateInstance(NULL, __uuidof(SonyAOI::_CSonyDB), (void**)&m_COMobj);
		Ic2->Release();

		res2 = (SUCCEEDED(hr2));
		
		m_COMobj->CreateAndCopyTableLibPro (&lDBName, &lDBPassword, &lSourceCat, &lSourceMod, &lDBDest, &lDestCat, &lDestMod) ;
		m_COMobj->Release();
	}

   ::SysFreeString(lDBName);
	::SysFreeString(lDBPassword);
	::SysFreeString(lDBDest);
}

void CreateLibProTable(CString DBDest, CString RSIDBPassword, CString tableName)
{
	
   BSTR lDBDest = DBDest.AllocSysString();
	BSTR lTbName = tableName.AllocSysString();
	BSTR lRSIDBPassword = RSIDBPassword.AllocSysString();

	IClassFactory * Ic2 = NULL;
	SonyAOI::_CSonyDB  * m_COMobj;
	HRESULT hr1, hr2;
	hr1 = CoGetClassObject(	__uuidof(SonyAOI::CSonyDB), CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&Ic2);

	bool res1 = false;
	bool res2 = false;

	if (SUCCEEDED(hr1))
	{
		res1 = true;
				
		hr2 = Ic2->CreateInstance(NULL, __uuidof(SonyAOI::_CSonyDB), (void**)&m_COMobj);
		Ic2->Release();

		res2 = (SUCCEEDED(hr2));
		
		m_COMobj->CreateTableLibPro (&lDBDest, &lRSIDBPassword, &lTbName) ;
		m_COMobj->Release();
	}

   
	::SysFreeString(lTbName);
	::SysFreeString(lDBDest);
}

void CreateLibModTable(CString DBDest, CString RSIDBPassword, CString CatID)
{
	
   BSTR lDBDest = DBDest.AllocSysString();
	BSTR lCatID = CatID.AllocSysString();
	BSTR lRSIDBPassword = RSIDBPassword.AllocSysString();

	IClassFactory * Ic2 = NULL;
	SonyAOI::_CSonyDB  * m_COMobj;
	HRESULT hr1, hr2;
	hr1 = CoGetClassObject(	__uuidof(SonyAOI::CSonyDB), CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&Ic2);

	bool res1 = false;
	bool res2 = false;

	if (SUCCEEDED(hr1))
	{
		res1 = true;
				
		hr2 = Ic2->CreateInstance(NULL, __uuidof(SonyAOI::_CSonyDB), (void**)&m_COMobj);
		Ic2->Release();

		res2 = (SUCCEEDED(hr2));
		
		m_COMobj->CreateLibModTable (&lDBDest, &lRSIDBPassword, &lCatID) ;
		m_COMobj->Release();
	}

   
	::SysFreeString(lCatID);
	::SysFreeString(lDBDest);
}

void GetBitMapName(CString DBName, CString DBPassword, CString CatID, CString  ModelID, CString& out_BitMapName) 
{
	
   BSTR lDBName = DBName.AllocSysString();
	BSTR lDBPassword = DBPassword.AllocSysString();
	BSTR lCatID = CatID.AllocSysString();
	BSTR lModelID = ModelID.AllocSysString();
	BSTR l_out_BitMapName = out_BitMapName.AllocSysString();
	
	IClassFactory * Ic2 = NULL;
	SonyAOI::_CSonyDB  * m_COMobj;
	HRESULT hr1, hr2;
	hr1 = CoGetClassObject(	__uuidof(SonyAOI::CSonyDB), CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&Ic2);

	bool res1 = false;
	bool res2 = false;

	if (SUCCEEDED(hr1))
	{
		res1 = true;
				
		hr2 = Ic2->CreateInstance(NULL, __uuidof(SonyAOI::_CSonyDB), (void**)&m_COMobj);
		Ic2->Release();

		res2 = (SUCCEEDED(hr2));
		
		m_COMobj->GetBitMapName (&lDBName, &lDBPassword, &lCatID, &lModelID, &l_out_BitMapName) ;
		m_COMobj->Release();
		out_BitMapName = l_out_BitMapName;
	}

   
	::SysFreeString(lCatID);
	::SysFreeString(lDBName);
}
void GetCatName(CString DBName, CString DBPassword, CString CatID, CString out_CatName) 
{
	
   BSTR lDBName = DBName.AllocSysString();
	BSTR lDBPassword = DBPassword.AllocSysString();
	BSTR lCatID = CatID.AllocSysString();
	BSTR l_out_CatName = out_CatName.AllocSysString();
	
	IClassFactory * Ic2 = NULL;
	SonyAOI::_CSonyDB  * m_COMobj;
	HRESULT hr1, hr2;
	hr1 = CoGetClassObject(	__uuidof(SonyAOI::CSonyDB), CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&Ic2);

	bool res1 = false;
	bool res2 = false;

	if (SUCCEEDED(hr1))
	{
		res1 = true;
				
		hr2 = Ic2->CreateInstance(NULL, __uuidof(SonyAOI::_CSonyDB), (void**)&m_COMobj);
		Ic2->Release();

		res2 = (SUCCEEDED(hr2));
		
		m_COMobj->GetCatName (&lDBName, &lDBPassword, &lCatID, &l_out_CatName) ;
		m_COMobj->Release();
	}

   
	::SysFreeString(lCatID);
	::SysFreeString(lDBName);
}

void GetFrameSize(CString DBName, CString DBPassword, CString CatID, CString ModelID, CString& out_XSize, CString& out_YSize) 
{
	
   BSTR lDBName = DBName.AllocSysString();
	BSTR lDBPassword = DBPassword.AllocSysString();
	BSTR lCatID = CatID.AllocSysString();
	BSTR lModelID = ModelID.AllocSysString();
	BSTR l_out_XSize = out_XSize.AllocSysString();
	BSTR l_out_YSize = out_YSize.AllocSysString();
	
	IClassFactory * Ic2 = NULL;
	SonyAOI::_CSonyDB  * m_COMobj;
	HRESULT hr1, hr2;
	hr1 = CoGetClassObject(	__uuidof(SonyAOI::CSonyDB), CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&Ic2);

	bool res1 = false;
	bool res2 = false;

	if (SUCCEEDED(hr1))
	{
		res1 = true;
				
		hr2 = Ic2->CreateInstance(NULL, __uuidof(SonyAOI::_CSonyDB), (void**)&m_COMobj);
		Ic2->Release();

		res2 = (SUCCEEDED(hr2));
		
		m_COMobj->GetFramSize (&lDBName, &lDBPassword, &lCatID, &lModelID,  &l_out_XSize, &l_out_YSize) ;
		m_COMobj->Release();
		out_XSize = l_out_XSize;
		out_YSize = l_out_YSize;
	}

   
	::SysFreeString(lCatID);
	::SysFreeString(lDBName);
}

bool GetXREFCatModel(CString DBName, CString PartNumber, CString& out_Cat, CString& out_Mod, CString& out_Explode, double& out_Angle) 
{
	
   BSTR lDBName = DBName.AllocSysString();
	BSTR lPartNumber = PartNumber.AllocSysString();
	BSTR l_out_Cat = out_Cat.AllocSysString();
	BSTR l_out_Mod = out_Mod.AllocSysString();
	BSTR l_out_Explode = out_Explode.AllocSysString();
	double l_out_Angle = out_Angle;
	
	IClassFactory * Ic2 = NULL;
	SonyAOI::_CSonyDB  * m_COMobj;
	HRESULT hr1, hr2;
	hr1 = CoGetClassObject(	__uuidof(SonyAOI::CSonyDB), CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&Ic2);

	//bool res1 = false;
	//bool res2 = false;
	bool res3 = false;

	if (SUCCEEDED(hr1))
	{
		//res1 = true;
				
		hr2 = Ic2->CreateInstance(NULL, __uuidof(SonyAOI::_CSonyDB), (void**)&m_COMobj);
		Ic2->Release();

		//res2 = (SUCCEEDED(hr2));
		
		short getxref = m_COMobj->GetXREFCatModel(&lDBName, &lPartNumber, &l_out_Cat, &l_out_Mod,  &l_out_Explode, &l_out_Angle) ;
		m_COMobj->Release();
		res3 = (getxref == 0); // anything non-zero from GetXREFCatModel call is an error code
		out_Cat = l_out_Cat;
		out_Mod = l_out_Mod;
		out_Explode = l_out_Explode;
		out_Angle = l_out_Angle;
	}

   
	::SysFreeString(lPartNumber);
	::SysFreeString(lDBName);

	// res3 can't be true unless res1 and res2 were also true, so don't really
	// need res1 and res2
	return (/* res1 && res2 && */ res3);
}
void PutLibCatList(CString DBName, CString DBPassword, CString SourceCat, CString DestDB,  CString DestCatID) 
{
	
   BSTR lDBName = DBName.AllocSysString();
	BSTR lDBPassword = DBPassword.AllocSysString();
	BSTR lSourceCat = SourceCat.AllocSysString();
	BSTR lDestDB = DestDB.AllocSysString();
	BSTR lDestCatID = DestCatID.AllocSysString();
	
	IClassFactory * Ic2 = NULL;
	SonyAOI::_CSonyDB  * m_COMobj;
	HRESULT hr1, hr2;
	hr1 = CoGetClassObject(	__uuidof(SonyAOI::CSonyDB), CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&Ic2);

	bool res1 = false;
	bool res2 = false;

	if (SUCCEEDED(hr1))
	{
		res1 = true;
				
		hr2 = Ic2->CreateInstance(NULL, __uuidof(SonyAOI::_CSonyDB), (void**)&m_COMobj);
		Ic2->Release();

		res2 = (SUCCEEDED(hr2));
		
		m_COMobj->PutLibCatList(&lDBName, &lDBPassword, &lSourceCat, &lDestDB,  &lDestCatID) ;
		m_COMobj->Release();
	}

}

void PutLibModData(CString DBName, CString DBPassword, CString SourceCat, CString SourceMod, CString DestDB,  CString DestCatID, CString DestMod) 
{
	
   BSTR lDBName = DBName.AllocSysString();
	BSTR lDBPassword = DBPassword.AllocSysString();
	BSTR lSourceCat = SourceCat.AllocSysString();
	BSTR lSourceMod = SourceMod.AllocSysString();
	BSTR lDestDB = DestDB.AllocSysString();
	BSTR lDestCatID = DestCatID.AllocSysString();
	BSTR lDestMod = DestMod.AllocSysString();
	
	IClassFactory * Ic2 = NULL;
	SonyAOI::_CSonyDB  * m_COMobj;
	HRESULT hr1, hr2;
	hr1 = CoGetClassObject(	__uuidof(SonyAOI::CSonyDB), CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&Ic2);

	bool res1 = false;
	bool res2 = false;

	if (SUCCEEDED(hr1))
	{
		res1 = true;
				
		hr2 = Ic2->CreateInstance(NULL, __uuidof(SonyAOI::_CSonyDB), (void**)&m_COMobj);
		Ic2->Release();

		res2 = (SUCCEEDED(hr2));
		
		m_COMobj->PutLibModData (&lDBName, &lDBPassword, &lSourceCat, &lSourceMod, &lDestDB,  &lDestCatID, &lDestMod) ;
		m_COMobj->Release();
	}

}

void SaveXREFCatModel(CString DBName, CString PartNumber, CString Cat, CString Mod, CString Explode, double Angle) 
{
	
   BSTR lDBName = DBName.AllocSysString();
	BSTR lPartNumber = PartNumber.AllocSysString();
	BSTR lCat = Cat.AllocSysString();
	BSTR lMod = Mod.AllocSysString();
	BSTR lExplode = Explode.AllocSysString();
	double lAngle = Angle;
	
	IClassFactory * Ic2 = NULL;
	SonyAOI::_CSonyDB  * m_COMobj;
	HRESULT hr1, hr2;
	hr1 = CoGetClassObject(	__uuidof(SonyAOI::CSonyDB), CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&Ic2);

	bool res1 = false;
	bool res2 = false;

	if (SUCCEEDED(hr1))
	{
		res1 = true;
				
		hr2 = Ic2->CreateInstance(NULL, __uuidof(SonyAOI::_CSonyDB), (void**)&m_COMobj);
		Ic2->Release();

		res2 = (SUCCEEDED(hr2));
		
		m_COMobj->SaveXREFCatModel(&lDBName, &lPartNumber, &lCat, &lMod,  &lExplode, &lAngle) ;
		m_COMobj->Release();
	}


}
void GetModName(CString DBName, CString DBPassword, CString ModID,CString CatID, CString out_ModName) 
{
	BSTR lDBName = DBName.AllocSysString();
	BSTR lDBPassword = DBPassword.AllocSysString();
	BSTR lModID = ModID.AllocSysString();
	BSTR lCatID = CatID.AllocSysString();
	BSTR l_out_ModName = out_ModName.AllocSysString();
	
	IClassFactory * Ic2 = NULL;
	SonyAOI::_CSonyDB  * m_COMobj;
	HRESULT hr1, hr2;
	hr1 = CoGetClassObject(	__uuidof(SonyAOI::CSonyDB), CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&Ic2);

	bool res1 = false;
	bool res2 = false;

	if (SUCCEEDED(hr1))
	{
		res1 = true;
				
		hr2 = Ic2->CreateInstance(NULL, __uuidof(SonyAOI::_CSonyDB), (void**)&m_COMobj);
		Ic2->Release();

		res2 = (SUCCEEDED(hr2));
		
		m_COMobj->GetModName(&lDBName, &lDBPassword, &lModID, &lCatID, &l_out_ModName) ;
		m_COMobj->Release();
	}
}

void GetAllModNames(CString DBName, CString DBPassword, CString CatID, CString& out_ModNames)
{
	BSTR lDBName = DBName.AllocSysString();
	BSTR lDBPassword = DBPassword.AllocSysString();
	BSTR lCatID = CatID.AllocSysString();
	BSTR l_out_ModNames = out_ModNames.AllocSysString();

	IClassFactory * Ic2 = NULL;
	SonyAOI::_CSonyDB  * m_COMobj;
	HRESULT hr1, hr2;
	hr1 = CoGetClassObject(	__uuidof(SonyAOI::CSonyDB), CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&Ic2);

	bool res1 = false;
	bool res2 = false;

	if (SUCCEEDED(hr1))
	{
		res1 = true;
				
		hr2 = Ic2->CreateInstance(NULL, __uuidof(SonyAOI::_CSonyDB), (void**)&m_COMobj);
		Ic2->Release();

		res2 = (SUCCEEDED(hr2));
		
		m_COMobj->GetAllModNames (&lDBName, &lDBPassword, &lCatID, &l_out_ModNames);
		m_COMobj->Release();
		out_ModNames = l_out_ModNames;
	}

   ::SysFreeString(lDBName);
}

void AppendToLibCatList(CString DBName, CString CatID, CString OrderNum,  CString CatName, CString RSIDBPassword) 
{
   BSTR lDBName = DBName.AllocSysString();
	BSTR lRSIDBPassword = RSIDBPassword.AllocSysString();

	BSTR lCatID = CatID.AllocSysString();
	BSTR lOrderNum = OrderNum.AllocSysString();
	BSTR lCatName = CatName.AllocSysString();
	
	
	IClassFactory * Ic2 = NULL;
	SonyAOI::_CSonyDB  * m_COMobj;
	HRESULT hr1, hr2;
	hr1 = CoGetClassObject(	__uuidof(SonyAOI::CSonyDB), CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&Ic2);

	bool res1 = false;
	bool res2 = false;

	if (SUCCEEDED(hr1))
	{
		res1 = true;
				
		hr2 = Ic2->CreateInstance(NULL, __uuidof(SonyAOI::_CSonyDB), (void**)&m_COMobj);
		Ic2->Release();

		res2 = (SUCCEEDED(hr2));
		
		m_COMobj->AppendToLibCatList(&lDBName, &lCatID, &lOrderNum,  &lCatName, &lRSIDBPassword) ;
		m_COMobj->Release();
	}

}

void AppendToLibMod(CString DBName, CString RSIDBPassword, CString CatID, CString ModID, CString OrderNum, CString ModelName, CString UpdateFlag, CString ImageData) 
{
	BSTR lDBName = DBName.AllocSysString();
	BSTR lRSIDBPassword =RSIDBPassword.AllocSysString();

	BSTR lCatID = CatID.AllocSysString();
	BSTR lModID = ModID.AllocSysString();
	BSTR lOrderNum = OrderNum.AllocSysString();
	BSTR lModelName = ModelName.AllocSysString();
	BSTR lUpdateFlag = UpdateFlag.AllocSysString();
	BSTR lImageData = ImageData.AllocSysString();
	

	IClassFactory * Ic2 = NULL;
	SonyAOI::_CSonyDB  * m_COMobj;
	HRESULT hr1, hr2;
	hr1 = CoGetClassObject(	__uuidof(SonyAOI::CSonyDB), CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&Ic2);

	bool res1 = false;
	bool res2 = false;

	if (SUCCEEDED(hr1))
	{
		res1 = true;
				
		hr2 = Ic2->CreateInstance(NULL, __uuidof(SonyAOI::_CSonyDB), (void**)&m_COMobj);
		Ic2->Release();

		res2 = (SUCCEEDED(hr2));
		
		m_COMobj->AppendToLibMod(&lDBName, &lRSIDBPassword, &lCatID, &lModID, &lOrderNum, &lModelName, &lUpdateFlag, &lImageData);
		m_COMobj->Release();
	}

}

void AppendToLibPro(CString DBName, CString RSIDBPassword, CString CatID, CString ModID, CString ProcNo, CString ProcClass, CString ProcData) 
{
	BSTR lDBName = DBName.AllocSysString();
	BSTR lRSIDBPassword =RSIDBPassword.AllocSysString();

	BSTR lCatID = CatID.AllocSysString();
	BSTR lModID = ModID.AllocSysString();
	BSTR lProcNo = ProcNo.AllocSysString();
	BSTR lProcClass = ProcClass.AllocSysString();
	BSTR lProcData = ProcData.AllocSysString();
	

	IClassFactory * Ic2 = NULL;
	SonyAOI::_CSonyDB  * m_COMobj;
	HRESULT hr1, hr2;
	hr1 = CoGetClassObject(	__uuidof(SonyAOI::CSonyDB), CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&Ic2);

	bool res1 = false;
	bool res2 = false;

	if (SUCCEEDED(hr1))
	{
		res1 = true;
				
		hr2 = Ic2->CreateInstance(NULL, __uuidof(SonyAOI::_CSonyDB), (void**)&m_COMobj);
		Ic2->Release();

		res2 = (SUCCEEDED(hr2));
		
		m_COMobj->AppendToLibPro(&lDBName, &lRSIDBPassword, &lCatID, &lModID,  &lProcNo, & lProcClass, & lProcData) ;
		m_COMobj->Release();
	}

}

// only Sony AOI uses this one at the moment
int TableExists(CString DBName, CString DBPassword, CString TableName) 
{
	BSTR lDBName = DBName.AllocSysString();
	BSTR lDBPw = DBPassword.AllocSysString();
	BSTR lTbName = TableName.AllocSysString();
	int exists = 0;
	
	IClassFactory * Ic2 = NULL;
	SonyAOI::_CSonyDB  * m_COMobj;
	HRESULT hr1, hr2;
	hr1 = CoGetClassObject(	__uuidof(SonyAOI::CSonyDB), CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&Ic2);

	bool res1 = false;
	bool res2 = false;

	if (SUCCEEDED(hr1))
	{
		res1 = true;
				
		hr2 = Ic2->CreateInstance(NULL, __uuidof(SonyAOI::_CSonyDB), (void**)&m_COMobj);
		Ic2->Release();

		res2 = (SUCCEEDED(hr2));
		
		exists = m_COMobj->TableExist(&lDBName, &lDBPw, &lTbName) ;
		m_COMobj->Release();
	}

	// COMobj->TableExist() returns
	// -2  Database does not exist
	// -1  Table does not exist
	//  1  Table exists

	return (exists == 1);
}

//_____________________________________________________________________________

#endif  // defined(EnablePartLibrary)



