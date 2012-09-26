// $Header: /CAMCAD/4.3/Ado_Util.cpp 7     8/12/03 2:55p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/

#include "stdafx.h"
#include "ado_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

_COM_SMARTPTR_TYPEDEF(IADORecordBinding, __uuidof(IADORecordBinding));

inline void TESTHR(HRESULT _hr) { if FAILED(_hr) _com_issue_error(_hr); }

static CString dataSource;
static CString partDesc;
//static _RecordsetPtr pRs("ADODB.Recordset");

int executeSqlCommand(CString cmd, _RecordsetPtr pRs)
{
   CString connStr;
   int result = 0;

   connStr.Format("%s%s%s", "Data Source=", dataSource, ";Provider=Microsoft.Jet.OLEDB.4.0;");
   ::CoInitialize(NULL);
   try 
   {
//    _RecordsetPtr pRs("ADODB.Recordset");
//    IADORecordBindingPtr picRs(pRs);

      pRs->Open((LPCSTR)cmd, (LPCSTR)connStr, adOpenStatic, adLockOptimistic, adCmdText);

//    TESTHR(picRs->BindToRecordset(&rs));

//    while (!pRs->EndOfFile)
//    {
//       // Process data in the CCustomRs C++ instance variables.
//       resultStr.Format("%s\t%s\r\n", pRs->Fields->GetItem("PartName")->GetValue(),
//             pRs->Fields->GetItem("Decsription")->GetValue());
//
//       
//       // Move to the next row of the Recordset. 
//       // Fields in the new row will automatically be 
//       // placed in the CCustomRs C++ instance variables.
//       pRs->MoveNext();
//    }
//
//    pRs->Close();
   }
   catch (_com_error &e )
   {
      result = e.Error();
   }
   ::CoUninitialize();

   return result;
}

// this should be an mdb file.
void openFileConnection(CString filename)
{
   dataSource = filename;
}

void closeFileConnection()
{
   dataSource.Empty();
}

int addPart(CString partName, CString desc)
{
   _RecordsetPtr pRs("ADODB.Recordset");
   CString cmd;

   // INSERT INTO tableName (fieldName1, fieldName2, fieldName3, ... )
   //             VALUES (info1, info2, info3, ... )
   cmd.Format("INSERT INTO Parts(partName, desc) VALUES ('%s', '%s')", partName, desc);
   executeSqlCommand(cmd, pRs);

   return 0;
}

int getPart(CString tableName, CString partName, CString &desc)
{
   _RecordsetPtr pRs("ADODB.Recordset");
   CString cmd;
   int result;

   // SELECT fieldName1, fieldName2, fieldName3, ... FROM tableName WHERE fieldName = partName
   cmd.Format("SELECT * FROM %s WHERE PartName = '%s'", tableName, partName);
   result = executeSqlCommand(cmd, pRs);
   if (result != 0)
      return result;

// while (!pRs->EndOfFile)
// {
      // Process data in the CCustomRs C++ instance variables.
      desc = (char*)((_bstr_t)pRs->Fields->GetItem("Description")->GetValue());

      
      // Move to the next row of the Recordset. 
      // Fields in the new row will automatically be 
      // placed in the CCustomRs C++ instance variables.
//    pRs->MoveNext();
// }

   pRs->Close();

   return 0;
}
