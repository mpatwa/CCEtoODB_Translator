
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-97. All Rights Reserved.
*/           

#include "stdafx.h"
#include <float.h>
#include <math.h>                                      
#include "ccdoc.h"
#include "CCEtoODB.h"
#include "API.h"
#include "graph.h"
#include "ccview.h"
#include "attrib.h"
#include "pan.h"
#include "drc.h"
#include "draw.h"
#include "cedit.h"
#include "mainfrm.h"
#include "pcbutil.h"
#include "lic.h"
#include "extents.h"
#include "crypt.h"
#include "hilite.h"
#include "dft_func.h"
#include "xml_read.h"
#include "lyrmanip.h"
#include "EntityNumber.h"
#include "panellib.h"
#include "DcaEnumIterator.h"
#include "DcaFileType.h"
#include "MGCCCZDecryptionLIcs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CView *activeView; // from CCVIEW.CPP
extern BOOL LoadingDataFile; // CCDOC.CPP
extern CString CAMCAD_File; // from CAMCAD.CPP
extern char *lics[]; // from LIC.CPP
extern CMultiDocTemplate* pDocTemplate; // from CAMCAD.CPP
extern BOOL LockLicenses;
extern License *licenses;
extern int NextFileNum; // from GRAPH.CPP

CCEtoODBView *apiView; 
BOOL LockApplication = FALSE;
BOOL LockDocuments = FALSE;
BOOL LockLicenses = FALSE;
BOOL API_AutoRedraw = TRUE;
BOOL HideDrawings = FALSE;
unsigned long apiSerialNum;

void HighlightPins(CDC *pDC, CCEtoODBView *view);
int PanReference(CCEtoODBView *view, const char *ref);
void LoadLayerTypeInfo(CCEtoODBDoc *doc, const char *filename);
void CloseDRCList();
void EditGeometry(CCEtoODBDoc *doc, BlockStruct *block);

static int APIOverrideLevel = 0;

#define LOCK_APPLICATION      1
#define LOCK_DOCUMENTS        2
#define LOCK_LICENSES         4

/******************************************************************************
* APILevelAllowed
*/
BOOL APILevelAllowed(int level)
{
   if (level <= getApp().getCamcadLicense().getLicensedApiLevel() || level <= getApp().getCamcadLicense().getLicensedApiLevel())
      return TRUE;

   ErrorMessage("You do not have a license to use API Functions at this level.", "No License for this API Level");
   return FALSE;
}

/******************************************************************************
* ListConstants
*/
short API::ListConstants(short list, VARIANT FAR* array) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   VariantInit(array);
   array->vt = VT_ARRAY | VT_BSTR;
   SAFEARRAY FAR* psa;
   CString buf;
   long i;
   BSTR bstr;

   switch (list)
   {
   case 0: // constants
      psa = SafeArrayCreateVector(VT_BSTR, 0, 10); 

      i = 0;
      buf = "Constants";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);
      
      i = 1;
      buf = "Return Codes";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);
      
      i = 2;
      buf = "Graphic Classes";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);
      
      i = 3;
      buf = "Insert Types";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);
      
      i = 4;
      buf = "Block Types";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);
      
      i = 5;
      buf = "Shapes";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);
      
      i = 6;
      buf = "Layer Types";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);
      
      i = 7;
      buf = "CAD Systems";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);
      
      i = 8;
      buf = "Units";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);

      i = 9;
      buf = "Deprecated";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);

      i = 10;
      buf = "Keyword Groups";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);

      i = 11;
      buf = "Keyword Value Types";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);

      i = 11;
      buf = "Data Types";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);

      i = 13;
      buf = "Licenses";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);

      break;

   case 1: // return codes
      psa = SafeArrayCreateVector(VT_BSTR, 0, 19); 

      i = 0;
      buf = "0 = RC_SUCCESS";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);
      
      i = 1;
      buf = "-1 = RC_GENERAL_ERROR";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);
      
      i = 2;
      buf = "-2 = RC_NO_ACTIVE_DOC";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);
      
      i = 3;
      buf = "-3 = RC_ITEM_NOT_FOUND";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);
      
      i = 4;
      buf = "-4 = RC_ILLEGAL_POSITION";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);
      
      i = 5;
      buf = "-5 = RC_NO_ATTRIBS";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);
      
      i = 6;
      buf = "-6 = RC_WRONG_TYPE";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);
      
      i = 7;
      buf = "-7 = RC_NOTHING_SELECTED";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);
      
      i = 8;
      buf = "-8 = RC_INDEX_OUT_OF_RANGE";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);
      
      i = 9;
      buf = "-9 = RC_EMPTY_ARRAY";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);
      
      i = 10;
      buf = "-10 = RC_NO_LICENSE";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);

      i = 11;
      buf = "-11 = RC_UNINITIALIZED_VARIABLES";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);

      i = 12;
      buf = "-12 = RC_NOT_ALLOWED";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);

      i = 13;
      buf = "-13 = RC_VALUE_OUT_OF_RANGE";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);

      i = 14;
      buf = "-14 = RC_HOLE_IN_ARRAY";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);

      i = 15;
      buf = "-15 = RC_DFM_TEST_FAILED";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);

      i = 16;
      buf = "-16 = RC_MISSING_DLL";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);

      i = 17;
      buf = "-17 = RC_FILE_NOT_FOUND";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);

      i = 18;
      buf = "-18 = RC_UNKNOWN_FORMAT";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);

      i = 19;
      buf = "-19 = RC_INVALID_FORMAT";
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);
		break;
   
   case 2: // graphic classes
      psa = SafeArrayCreateVector(VT_BSTR, 0, graphicClassUpperBound - graphicClassLowerBound + 1);
      for (EnumIterator(GraphicClassTag, grclassIterator); grclassIterator.hasNext();)
      {
         GraphicClassTag grclass = grclassIterator.getNext();
         buf = graphicClassTagToDisplayString(grclass);
         bstr = buf.AllocSysString();
         SafeArrayPutElement(psa, &i, bstr);
         SysFreeString(bstr);
      }
      break;

   case 3: // inserttypes
      psa = SafeArrayCreateVector(VT_BSTR, 0, MAX_INSERTTYPE);

      for (EnumIterator(InsertTypeTag,insertTypeIterator);insertTypeIterator.hasNext();)
      {
         InsertTypeTag insertType = insertTypeIterator.getNext();

         buf = insertTypeToDisplayString(insertType);
         bstr = buf.AllocSysString();
         SafeArrayPutElement(psa, &i, bstr);
         SysFreeString(bstr);
      }

      break;

   case 4: // blocktypes
      psa = SafeArrayCreateVector(VT_BSTR, 0, MAX_BLOCKTYPE); 

      for (EnumIterator(BlockTypeTag,blockTypeTagIterator);blockTypeTagIterator.hasNext();)
      {
         BlockTypeTag blockType = blockTypeTagIterator.getNext();

         buf = blockTypeToDisplayString(blockType);
         bstr = buf.AllocSysString();

         long blockTypeLong = blockType;
         SafeArrayPutElement(psa, &blockTypeLong, bstr);
         SysFreeString(bstr);
      }

      break;

   case 5: // shapes
      psa = SafeArrayCreateVector(VT_BSTR, 0, MAX_SHAPES); 
      for (i=0; i<MAX_SHAPES; i++)
      {
         buf = shapes[i];
         bstr = buf.AllocSysString();
         SafeArrayPutElement(psa, &i, bstr);
         SysFreeString(bstr);
      }
      break;

   case 6: // layertypes
      psa = SafeArrayCreateVector(VT_BSTR, 0, MAX_LAYTYPE); 
      for (i=0; i<MAX_LAYTYPE; i++)
      {
         buf = layerTypeToString(i);
         bstr = buf.AllocSysString();
         SafeArrayPutElement(psa, &i, bstr);
         SysFreeString(bstr);
      }
      break;

   case 7: // cadsystems
      psa = SafeArrayCreateVector(VT_BSTR, fileTypeLowerBound, (fileTypeUpperBound-fileTypeLowerBound+1)); 
      for (i=fileTypeLowerBound; i<=fileTypeUpperBound; i++)
      {
         buf = fileTypeTagToPrettyString(intToFileTypeTag(i));
         bstr = buf.AllocSysString();
         SafeArrayPutElement(psa, &i, bstr);
         SysFreeString(bstr);
      }
      break;

   case 8: // units
      psa = SafeArrayCreateVector(VT_BSTR,PageUnitsTagMin,PageUnitsTagMax - PageUnitsTagMin + 1); 
      for (EnumIterator(PageUnitsTag,unitsIterator);unitsIterator.hasNext();)
      {
         PageUnitsTag units = unitsIterator.getNext();
         i = units;
         buf = pageUnitsTagToString(units);
         bstr = buf.AllocSysString();
         SafeArrayPutElement(psa, &i, bstr);
         SysFreeString(bstr);
      }
      break;

   case 10: // keyword groups
      psa = SafeArrayCreateVector(VT_BSTR,AttribGroupTagMin,AttribGroupTagMax - AttribGroupTagMin + 1); 
      for (EnumIterator(AttribGroupTag,groupIterator);groupIterator.hasNext();)
      {
         AttribGroupTag group = groupIterator.getNext();
         i = group;
         buf = attribGroupTagToString(group);
         bstr = buf.AllocSysString();
         SafeArrayPutElement(psa, &i, bstr);
         SysFreeString(bstr);
      }
      break;

   case 11: // keyword value types
      psa = SafeArrayCreateVector(VT_BSTR,ValueTypeTagMin,ValueTypeTagMax - ValueTypeTagMin + 1); 
      for (EnumIterator(ValueTypeTag,valueTypeIterator);valueTypeIterator.hasNext();)
      {
         ValueTypeTag valueType = valueTypeIterator.getNext();
         i = valueType;
         buf = valueTypeTagToString(valueType);
         bstr = buf.AllocSysString();
         SafeArrayPutElement(psa, &i, bstr);
         SysFreeString(bstr);
      }
      break;

   case 12: // data types
      psa = SafeArrayCreateVector(VT_BSTR,DataTypeTagMin,DataTypeTagMax - DataTypeTagMin + 1); 
      for (EnumIterator(DataTypeTag,dataTypeIterator);dataTypeIterator.hasNext();)
      {
         DataTypeTag dataType = dataTypeIterator.getNext();
         i = dataType;
         buf = dataTypeTagToString(dataType);
         bstr = buf.AllocSysString();
         SafeArrayPutElement(psa, &i, bstr);
         SysFreeString(bstr);
      }
      break;

   case 9:
   case 13: // licenses
      psa = SafeArrayCreateVector(VT_BSTR, 0, LIC_MAXENTRY); 
      for (i=0; i<LIC_MAXENTRY; i++)
      {
         buf = lics[i];
         bstr = buf.AllocSysString();
         SafeArrayPutElement(psa, &i, bstr);
         SysFreeString(bstr);
      }
      break;

   default:
      return RC_INDEX_OUT_OF_RANGE;
   }

   array->parray = psa;

   return RC_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
// API
IMPLEMENT_DYNCREATE(API, CCmdTarget)

API::API()
{
   EnableAutomation();
   
   // To keep the application running as long as an OLE automation 
   // object is active, the constructor calls AfxOleLockApp.
   
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
   pApp->UsingAutomation = TRUE;

   //AfxOleLockApp();
}

API::~API()
{
   // To terminate the application when all objects created with
   //    with OLE automation, the destructor calls AfxOleUnlockApp.
   
   //AfxOleUnlockApp();
}

void API::OnFinalRelease()
{
   // When the last reference for an automation object is released
   // OnFinalRelease is called.  The base class will automatically
   // deletes the object.  Add additional cleanup required for your
   // object before calling the base class.

   CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(API, CCmdTarget)
   //{{AFX_MSG_MAP(API)
      // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(API, CCmdTarget)
   //{{AFX_DISPATCH_MAP(API)
   DISP_FUNCTION(API, "GraphAperture", GraphAperture, VT_I2, VTS_BSTR VTS_I2 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I2)
   DISP_FUNCTION(API, "OverrideColorByRefDes", OverrideColorByRefDes, VT_I2, VTS_BSTR VTS_BOOL VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "OverrideColorRemoveAll", OverrideColorRemoveAll, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "OverrideColorByAttribute", OverrideColorByAttribute, VT_I2, VTS_BSTR VTS_BSTR VTS_BOOL VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "Exit", Exit, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "ZoomLevel", ZoomLevel, VT_I2, VTS_R8 VTS_I2)
   DISP_FUNCTION(API, "ZoomWindow", ZoomWindow, VT_I2, VTS_R8 VTS_R8 VTS_R8 VTS_R8)
   DISP_FUNCTION(API, "PanRef", PanRef, VT_I2, VTS_BSTR)
   DISP_FUNCTION(API, "PanCoord", PanCoord, VT_I2, VTS_R8 VTS_R8)
   DISP_FUNCTION(API, "Redraw", Redraw, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "LayerColor", LayerColor, VT_I2, VTS_BSTR VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "LayerShow", LayerShow, VT_I2, VTS_BSTR VTS_BOOL)
   DISP_FUNCTION(API, "LayerShowAll", LayerShowAll, VT_I2, VTS_BOOL)
   DISP_FUNCTION(API, "TopView", TopView, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "BottomView", BottomView, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "ColorSet", ColorSet, VT_I2, VTS_I2)
   DISP_FUNCTION(API, "ClearSelected", ClearSelected, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "ZoomExtentsOfComponents", ZoomExtentsOfComponents, VT_I2, VTS_BSTR)
   DISP_FUNCTION(API, "CloseActiveDocument", CloseActiveDocument, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "SetAttribByRefDes", SetAttribByRefDes, VT_I2, VTS_BSTR VTS_BSTR VTS_I2 VTS_BSTR)
   DISP_FUNCTION(API, "ShadeByRefDes", ShadeByRefDes, VT_I2, VTS_BSTR VTS_BOOL VTS_I2 VTS_I2 VTS_I2 VTS_BOOL VTS_BOOL VTS_BOOL)
   DISP_FUNCTION(API, "RemoveShading", RemoveShading, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "ShowActiveDocument", ShowActiveDocument, VT_I2, VTS_I2)
   DISP_FUNCTION(API, "GraphTool", GraphTool, VT_I2, VTS_BSTR VTS_R4 VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "ZoomExtentsOfComponentsMargin", ZoomExtentsOfComponentsMargin, VT_I2, VTS_BSTR VTS_I2)
   DISP_FUNCTION(API, "TurnOnResponse", TurnOnResponse, VT_I2, VTS_BSTR)
   DISP_FUNCTION(API, "TurnOffResponse", TurnOffResponse, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "HighlightNet", HighlightNet, VT_I2, VTS_BSTR VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "HighlightNetClear", HighlightNetClear, VT_I2, VTS_BSTR)
   DISP_FUNCTION(API, "HighlightNetClearAll", HighlightNetClearAll, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "ZoomExtentsOfNets", ZoomExtentsOfNets, VT_I2, VTS_BSTR)
   DISP_FUNCTION(API, "ZoomExtentsOfNetsMargin", ZoomExtentsOfNetsMargin, VT_I2, VTS_BSTR VTS_I2)
   DISP_FUNCTION(API, "HighlightPin", HighlightPin, VT_I2, VTS_BSTR VTS_BSTR VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "ShowMainWindow", ShowMainWindow, VT_I2, VTS_I2)
   DISP_FUNCTION(API, "SizeMainWindow", SizeMainWindow, VT_I2, VTS_I2 VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "GetFileArray", GetFileArray, VT_I2, VTS_PVARIANT)
   DISP_FUNCTION(API, "GetNetArray", GetNetArray, VT_I2, VTS_I4 VTS_PVARIANT)
   DISP_FUNCTION(API, "GetNet", GetNet, VT_I2, VTS_I4 VTS_I4 VTS_PBSTR)
   DISP_FUNCTION(API, "GetCompPinArray", GetCompPinArray, VT_I2, VTS_I4 VTS_I4 VTS_PVARIANT)
   DISP_FUNCTION(API, "GetCompPin", GetCompPin, VT_I2, VTS_I4 VTS_I4 VTS_I4 VTS_PBSTR VTS_PBSTR VTS_PR4 VTS_PR4)
   DISP_FUNCTION(API, "GeneratePinLocs", GeneratePinLocs, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "GetKeyword", GetKeyword, VT_I2, VTS_I2 VTS_PBSTR VTS_PBSTR VTS_PBSTR VTS_PI2 VTS_PBOOL)
   DISP_FUNCTION(API, "GetAttribKeywordArray", GetAttribKeywordArray, VT_I2, VTS_PVARIANT)
   DISP_FUNCTION(API, "GetAttribValue", GetAttribValue, VT_I2, VTS_I2 VTS_PBSTR)
   DISP_FUNCTION(API, "SetNetCurrentAttrib", SetNetCurrentAttrib, VT_I2, VTS_I4 VTS_I4)
   DISP_FUNCTION(API, "SetCompPinCurrentAttrib", SetCompPinCurrentAttrib, VT_I2, VTS_I4 VTS_I4 VTS_I4)
   DISP_FUNCTION(API, "GetGeometry", GetGeometry, VT_I2, VTS_I2 VTS_PBSTR VTS_PI2 VTS_PI2)
   DISP_FUNCTION(API, "GetDataArray", GetDataArray, VT_I2, VTS_I2 VTS_PVARIANT)
   DISP_FUNCTION(API, "GetData", GetData, VT_I2, VTS_I2 VTS_I4 VTS_PI2 VTS_PI2 VTS_PI2)
   DISP_FUNCTION(API, "GetDataInsert", GetDataInsert, VT_I2, VTS_I2 VTS_I4 VTS_PI2 VTS_PR4 VTS_PR4 VTS_PR4 VTS_PR4 VTS_PI2 VTS_PI2 VTS_PBSTR)
   DISP_FUNCTION(API, "SetDataCurrentAttrib", SetDataCurrentAttrib, VT_I2, VTS_I2 VTS_I4)
   DISP_FUNCTION(API, "SetGeometryCurrentAttrib", SetGeometryCurrentAttrib, VT_I2, VTS_I2)
   DISP_FUNCTION(API, "GetDataArrayByType", GetDataArrayByType, VT_I2, VTS_I2 VTS_I2 VTS_PVARIANT)
   DISP_FUNCTION(API, "GetDataArrayByGraphicClass", GetDataArrayByGraphicClass, VT_I2, VTS_I2 VTS_I2 VTS_PVARIANT)
   DISP_FUNCTION(API, "GetDataArrayByInsertType", GetDataArrayByInsertType, VT_I2, VTS_I2 VTS_I2 VTS_PVARIANT)
   DISP_FUNCTION(API, "GetDataPolyStruct", GetDataPolyStruct, VT_I2, VTS_I2 VTS_I4 VTS_PVARIANT)
   DISP_FUNCTION(API, "GetDataPoly", GetDataPoly, VT_I2, VTS_I2 VTS_I4 VTS_I4 VTS_PI2 VTS_PBOOL VTS_PBOOL VTS_PBOOL VTS_PVARIANT)
   DISP_FUNCTION(API, "GetDataPolyPoint", GetDataPolyPoint, VT_I2, VTS_I2 VTS_I4 VTS_I4 VTS_I4 VTS_PR4 VTS_PR4 VTS_PR4)
   //DISP_FUNCTION(API, "DrawCross", DrawCross, VT_I2, VTS_R8 VTS_R8)
   DISP_FUNCTION(API, "GetKeywordIndex", GetKeywordIndex, VT_I2, VTS_BSTR VTS_PI2)
   DISP_FUNCTION(API, "SetSelectedDataCurrentAttrib", SetSelectedDataCurrentAttrib, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "SetAttribValue", SetAttribValue, VT_I2, VTS_I2 VTS_BSTR VTS_I2)
   DISP_FUNCTION(API, "SetAttribValueOnSelected", SetAttribValueOnSelected, VT_I2, VTS_I2 VTS_BSTR VTS_I2)
   DISP_FUNCTION(API, "GetGeometryArrayByType", GetGeometryArrayByType, VT_I2, VTS_I2 VTS_PVARIANT)
   DISP_FUNCTION(API, "GetDocumentArray", GetDocumentArray, VT_I2, VTS_PVARIANT)
   DISP_FUNCTION(API, "GetDocument", GetDocument, VT_I2, VTS_I4 VTS_PBSTR)
   DISP_FUNCTION(API, "ActivateDocument", ActivateDocument, VT_I2, VTS_I4)
   DISP_FUNCTION(API, "GetDRCArray", GetDRCArray, VT_I2, VTS_I4 VTS_PVARIANT)
   DISP_FUNCTION(API, "GetDRC", GetDRC, VT_I2, VTS_I4 VTS_I4 VTS_PBSTR VTS_PR8 VTS_PR8 VTS_PI2 VTS_PI2 VTS_PI2 VTS_PBSTR VTS_PI2)
   DISP_FUNCTION(API, "DeleteDRC", DeleteDRC, VT_I2, VTS_I4)
   DISP_FUNCTION(API, "GetActiveFile", GetActiveFile, VT_I2, VTS_PI4)
   DISP_FUNCTION(API, "AddDrcMarker", AddDrcMarker, VT_I2, VTS_I4 VTS_R4 VTS_R4 VTS_BSTR VTS_BSTR VTS_PI4)
   DISP_FUNCTION(API, "LockCAMCAD", LockCAMCAD, VT_I2, VTS_I2)
   DISP_FUNCTION(API, "LoadColorSet", LoadColorSet, VT_I2, VTS_BSTR)
   DISP_FUNCTION(API, "CurrentSettings", CurrentSettings, VT_I2, VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "GetLayerArrayByType", GetLayerArrayByType, VT_I2, VTS_I2 VTS_PVARIANT)
   DISP_FUNCTION(API, "SetLayerCurrentAttrib", SetLayerCurrentAttrib, VT_I2, VTS_I2)
   DISP_FUNCTION(API, "GetViewSurface", GetViewSurface, VT_I2, VTS_PI2)
   DISP_FUNCTION(API, "GetViewportCoords", GetViewportCoords, VT_I2, VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8)
   DISP_FUNCTION(API, "GetFile", GetFile, VT_I2, VTS_I4 VTS_PBSTR VTS_PI2 VTS_PR4 VTS_PR4 VTS_PR4 VTS_PR4 VTS_PI2 VTS_PI2 VTS_PI2)
   DISP_FUNCTION(API, "GetNetCompPinData", GetNetCompPinData, VT_I2, VTS_I4 VTS_PVARIANT)
   DISP_FUNCTION(API, "GetNetViaData", GetNetViaData, VT_I2, VTS_I4 VTS_PVARIANT)
   DISP_FUNCTION(API, "GetComponentData", GetComponentData, VT_I2, VTS_I4 VTS_PVARIANT)
   DISP_FUNCTION(API, "GetNetTraceData", GetNetTraceData, VT_I2, VTS_I4 VTS_PVARIANT)
   DISP_FUNCTION(API, "LoadDataFile", LoadDataFile, VT_I2, VTS_BSTR)
   DISP_FUNCTION(API, "FindVia", FindVia, VT_I2, VTS_BSTR VTS_R8 VTS_R8 VTS_R8 VTS_PI2 VTS_PI4)
   DISP_FUNCTION(API, "IssueCommand", IssueCommand, VT_I2, VTS_I4)
   DISP_FUNCTION(API, "ShowInsertType", ShowInsertType, VT_I2, VTS_I2 VTS_BOOL)
   DISP_FUNCTION(API, "ShowGraphicClass", ShowGraphicClass, VT_I2, VTS_I2 VTS_BOOL)
   DISP_FUNCTION(API, "HighlightPinClear", HighlightPinClear, VT_I2, VTS_BSTR VTS_BSTR)
   DISP_FUNCTION(API, "HighlightPinClearAll", HighlightPinClearAll, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "GetDocumentExtents", GetDocumentExtents, VT_I2, VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8)
   DISP_FUNCTION(API, "OverrideColorDRC", OverrideColorDRC, VT_I2, VTS_I4 VTS_BOOL VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "FindDRC", FindDRC, VT_I2, VTS_I4 VTS_PI4 VTS_PI4)
   DISP_FUNCTION(API, "GetNetTraceDataByNetname", GetNetTraceDataByNetname, VT_I2, VTS_I4 VTS_BSTR VTS_PVARIANT)
   DISP_FUNCTION(API, "GetAPIVersion", GetAPIVersion, VT_I2, VTS_PI2 VTS_PI2)
   DISP_FUNCTION(API, "OverrideColorData", OverrideColorData, VT_I2, VTS_I2 VTS_I4 VTS_BOOL VTS_I2 VTS_I2 VTS_I2)
   //DISP_FUNCTION(API, "Import", Import, VT_I2, VTS_I2 VTS_BSTR VTS_BSTR)
   DISP_FUNCTION(API, "Blank1", Blank1, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "ListConstants", ListConstants, VT_I2, VTS_I2 VTS_PVARIANT)
   DISP_FUNCTION(API, "AutoRedraw", AutoRedraw, VT_I2, VTS_BOOL)
   DISP_FUNCTION(API, "GetZoomLevel", GetZoomLevel, VT_I2, VTS_PR8)
   DISP_FUNCTION(API, "SetZoomLevel", SetZoomLevel, VT_I2, VTS_R8)
   DISP_FUNCTION(API, "SetLayerType", SetLayerType, VT_I2, VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "SetLayerShow", SetLayerShow, VT_I2, VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "SetLayerMirror", SetLayerMirror, VT_I2, VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "SetLayerColor", SetLayerColor, VT_I2, VTS_I2 VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "FindLayer", FindLayer, VT_I2, VTS_BSTR VTS_PI2)
   DISP_FUNCTION(API, "SetFileShow", SetFileShow, VT_I2, VTS_I4 VTS_BOOL)
   DISP_FUNCTION(API, "GetPageUnits", GetPageUnits, VT_I2, VTS_PBSTR)
   DISP_FUNCTION(API, "SetDataInsertInserttype", SetDataInsertInserttype, VT_I2, VTS_I2 VTS_I4 VTS_I2)
   DISP_FUNCTION(API, "FindNet", FindNet, VT_I2, VTS_I4 VTS_BSTR VTS_PI4)
   DISP_FUNCTION(API, "GetPlacementData", GetPlacementData, VT_I2, VTS_I4 VTS_I2 VTS_PVARIANT)
   DISP_FUNCTION(API, "GetActiveDocument", GetActiveDocument, VT_I2, VTS_PI4)
   DISP_FUNCTION(API, "GetFile2", GetFile2, VT_I2, VTS_I4 VTS_PBSTR VTS_PI2 VTS_PBOOL VTS_PR4 VTS_PR4 VTS_PR4 VTS_PR4 VTS_PI2 VTS_PI2 VTS_PI2)
   DISP_FUNCTION(API, "GetLayer2", GetLayer2, VT_I2, VTS_I2 VTS_PBSTR VTS_PI2 VTS_PI2 VTS_PI2 VTS_PBSTR VTS_PI2 VTS_PI2 VTS_PI2)
   DISP_FUNCTION(API, "GetLayer", GetLayer, VT_I2, VTS_I2 VTS_PBSTR VTS_PI2 VTS_PI2 VTS_PBSTR)
   DISP_FUNCTION(API, "SetLayerWorldView", SetLayerWorldView, VT_I2, VTS_I2 VTS_BOOL)
   DISP_FUNCTION(API, "GetLayer3", GetLayer3, VT_I2, VTS_I2 VTS_PBSTR VTS_PI2 VTS_PI2 VTS_PI2 VTS_PBSTR VTS_PI2 VTS_PI2 VTS_PI2 VTS_PI2)
   DISP_FUNCTION(API, "HideData", HideData, VT_I2, VTS_I2 VTS_I4 VTS_BOOL)
   DISP_FUNCTION(API, "HideAll", HideAll, VT_I2, VTS_BOOL)
   DISP_FUNCTION(API, "FindDataByRefname", FindDataByRefname, VT_I2, VTS_BSTR VTS_PI2 VTS_PI4)
   DISP_FUNCTION(API, "HideInserts", HideInserts, VT_I2, VTS_I2 VTS_BOOL)
   DISP_FUNCTION(API, "HideNetsAll", HideNetsAll, VT_I2, VTS_BOOL)
   DISP_FUNCTION(API, "HideNet", HideNet, VT_I2, VTS_BSTR VTS_BOOL)
   DISP_FUNCTION(API, "SetPageSize", SetPageSize, VT_I2, VTS_R8 VTS_R8 VTS_R8 VTS_R8)
   //DISP_FUNCTION(API, "SaveDataFileAs", SaveDataFileAs, VT_I2, VTS_BSTR)
   DISP_FUNCTION(API, "SetAPILevel", SetAPILevel, VT_I2, VTS_I2)
   DISP_FUNCTION(API, "GetGeometryData", GetGeometryData, VT_I2, VTS_I2 VTS_PVARIANT)
   DISP_FUNCTION(API, "SetFileRotation", SetFileRotation, VT_I2, VTS_I4 VTS_R8)
   DISP_FUNCTION(API, "UserCoord", UserCoord, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "TurnOffResponseByID", TurnOffResponseByID, VT_I2, VTS_BSTR)
   DISP_FUNCTION(API, "GenerateMarker", GenerateMarker, VT_I2, VTS_I2 VTS_R8 VTS_R8 VTS_R8)
   DISP_FUNCTION(API, "APIDocument", APIDocument, VT_I2, VTS_I4)
   DISP_FUNCTION(API, "NetLengthPoly", NetLengthPoly, VT_I2, VTS_I2 VTS_I4 VTS_PR8)
   DISP_FUNCTION(API, "NetLengthSegment", NetLengthSegment, VT_I2, VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_PR8)
   DISP_FUNCTION(API, "NetLengthNet", NetLengthNet, VT_I2, VTS_I4 VTS_BSTR VTS_PR8)
   DISP_FUNCTION(API, "NetLengthPins", NetLengthPins, VT_I2, VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_PR8)
   DISP_FUNCTION(API, "LayersUsed", LayersUsed, VT_I2, VTS_PVARIANT)
   DISP_FUNCTION(API, "GetSelectedArray", GetSelectedArray, VT_I2, VTS_PVARIANT)
   DISP_FUNCTION(API, "GetSelected", GetSelected, VT_I2, VTS_I4 VTS_PI2 VTS_PI4)
   DISP_FUNCTION(API, "GetSubSelectLevel", GetSubSelectLevel, VT_I2, VTS_PI2)
   DISP_FUNCTION(API, "GetSubSelect", GetSubSelect, VT_I2, VTS_I2 VTS_PI2 VTS_PI4 VTS_PR4 VTS_PR4 VTS_PR4 VTS_PR4 VTS_PI2 VTS_PI2)
   DISP_FUNCTION(API, "NetLengthNetManhattan", NetLengthNetManhattan, VT_I2, VTS_I4 VTS_BSTR VTS_PR8)
   DISP_FUNCTION(API, "NetLengthPinsManhattan", NetLengthPinsManhattan, VT_I2, VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_PR8)
   DISP_FUNCTION(API, "GetNetData", GetNetData, VT_I2, VTS_I4 VTS_PVARIANT)
   DISP_FUNCTION(API, "HideDrawings", HideDrawings, VT_I2, VTS_BOOL)
   DISP_FUNCTION(API, "GetFilePlaced", GetFilePlaced, VT_I2, VTS_I4 VTS_PBOOL)
   DISP_FUNCTION(API, "HighlightPinsOfANet", HighlightPinsOfANet, VT_I2, VTS_BSTR VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "HighlightPinsOfANetClear", HighlightPinsOfANetClear, VT_I2, VTS_BSTR)
   DISP_FUNCTION(API, "OverrideColorByInsertType", OverrideColorByInsertType, VT_I2, VTS_I2 VTS_BOOL VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "LayersUsedByNet", LayersUsedByNet, VT_I2, VTS_I4 VTS_BSTR VTS_PVARIANT)
   DISP_FUNCTION(API, "Export", Export, VT_I2, VTS_I2 VTS_BSTR)
   DISP_FUNCTION(API, "OverrideColorByGeometry", OverrideColorByGeometry, VT_I2, VTS_I2 VTS_BOOL VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "TempAddInsert", TempAddInsert, VT_I2, VTS_I4 VTS_I2 VTS_BSTR VTS_I2 VTS_R8 VTS_R8 VTS_R8 VTS_I2 VTS_I2 VTS_PI4)
   DISP_FUNCTION(API, "SetGeometryName", SetGeometryName, VT_I2, VTS_I2 VTS_BSTR)
   DISP_FUNCTION(API, "dfmFlatten", dfmFlatten, VT_I2, VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "SetFileGeometry", SetFileGeometry, VT_I2, VTS_I4 VTS_I2)
   DISP_FUNCTION(API, "dfmDistanceCheck", dfmDistanceCheck, VT_I2, VTS_I2 VTS_R8 VTS_R8 VTS_R8 VTS_BSTR VTS_I2 VTS_BSTR VTS_BOOL VTS_BOOL)
   DISP_FUNCTION(API, "License", License, VT_I2, VTS_I2 VTS_I4)
   DISP_FUNCTION(API, "dfmDistanceCheckDiffNets", dfmDistanceCheckDiffNets, VT_I2, VTS_I2 VTS_R8 VTS_R8 VTS_R8 VTS_BSTR VTS_I2 VTS_BSTR VTS_BOOL VTS_BOOL)
   DISP_FUNCTION(API, "dfmDistanceCheckByClass", dfmDistanceCheckByClass, VT_I2, VTS_I2 VTS_I2 VTS_I2 VTS_R8 VTS_R8 VTS_R8 VTS_BSTR VTS_I2 VTS_BSTR VTS_BOOL VTS_BOOL)
   DISP_FUNCTION(API, "CreateAperture", CreateAperture, VT_I2, VTS_BSTR VTS_I2 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I2 VTS_PI2 VTS_PI2)
   DISP_FUNCTION(API, "CreateLayer", CreateLayer, VT_I2, VTS_BSTR VTS_PI2)
   DISP_FUNCTION(API, "UserCoord2", UserCoord2, VT_I2, VTS_BOOL)
   DISP_FUNCTION(API, "GetLayerStackup", GetLayerStackup, VT_I2, VTS_I2 VTS_PI2 VTS_PI2 VTS_PI2)
   DISP_FUNCTION(API, "dfmCreateDFxOutline", dfmCreateDFxOutline, VT_I2, VTS_I2 VTS_I2 VTS_BOOL)
   DISP_FUNCTION(API, "SetLayerVisible", SetLayerVisible, VT_I2, VTS_I2 VTS_BOOL)
   DISP_FUNCTION(API, "SetLayerEditable", SetLayerEditable, VT_I2, VTS_I2 VTS_BOOL)
   DISP_FUNCTION(API, "GetLayerVisible", GetLayerVisible, VT_I2, VTS_I2 VTS_PBOOL)
   DISP_FUNCTION(API, "GetLayerEditable", GetLayerEditable, VT_I2, VTS_I2 VTS_PBOOL)
   DISP_FUNCTION(API, "GetCentroidData", GetCentroidData, VT_I2, VTS_I4 VTS_I2 VTS_PVARIANT)
   DISP_FUNCTION(API, "GetGeometryExtents", GetGeometryExtents, VT_I2, VTS_I2 VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8)
   DISP_FUNCTION(API, "HideCAMCAD", HideCAMCAD, VT_I2, VTS_BOOL)
   DISP_FUNCTION(API, "SplashMessage", SplashMessage, VT_I2, VTS_BSTR)
   DISP_FUNCTION(API, "SplashProgressRange", SplashProgressRange, VT_I2, VTS_I2)
   DISP_FUNCTION(API, "SplashProgressPosition", SplashProgressPosition, VT_I2, VTS_I2)
   DISP_FUNCTION(API, "DeleteGeometry", DeleteGeometry, VT_I2, VTS_I2)
   DISP_FUNCTION(API, "dfmFlattenByInserttypes", dfmFlattenByInserttypes, VT_I2, VTS_I2 VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "dfmComponentShadow", dfmComponentShadow, VT_I2, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8)
   DISP_FUNCTION(API, "dfmDistanceCheckByInserttype", dfmDistanceCheckByInserttype, VT_I2, VTS_I2 VTS_I2 VTS_I2 VTS_R8 VTS_R8 VTS_R8 VTS_BSTR VTS_I2 VTS_BSTR VTS_BOOL VTS_BOOL)
   DISP_FUNCTION(API, "dfmDistanceCheckClassInserttype", dfmDistanceCheckClassInserttype, VT_I2, VTS_I2 VTS_I2 VTS_I2 VTS_R8 VTS_R8 VTS_R8 VTS_BSTR VTS_I2 VTS_BSTR VTS_BOOL VTS_BOOL)
   DISP_FUNCTION(API, "MakeMaufacturingRefDes", MakeMaufacturingRefDes, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "ExportDXF", ExportDXF, VT_I2, VTS_BSTR VTS_R8 VTS_BOOL VTS_BOOL VTS_BOOL)
   DISP_FUNCTION(API, "CheckLicense", CheckLicense, VT_I2, VTS_I2 VTS_PBOOL)
   DISP_FUNCTION(API, "CheckImportFormatLicense", CheckImportFormatLicense, VT_I2, VTS_I2 VTS_PBOOL)
   DISP_FUNCTION(API, "dfmCreateFlattenGeometry", dfmCreateFlattenGeometry, VT_I2, VTS_I4 VTS_PI2)
   DISP_FUNCTION(API, "CreateKeyword", CreateKeyword, VT_I2, VTS_BSTR VTS_I2 VTS_I2 VTS_PI2)
   DISP_FUNCTION(API, "RemoveAttrib", RemoveAttrib, VT_I2, VTS_I2)
   DISP_FUNCTION(API, "GetDRCAlgorithm", GetDRCAlgorithm, VT_I2, VTS_I2 VTS_PBSTR)
   DISP_FUNCTION(API, "FindData", FindData, VT_I2, VTS_I4 VTS_PI2 VTS_PI4)
   DISP_FUNCTION(API, "GetDRCMeasure", GetDRCMeasure, VT_I2, VTS_I4 VTS_I4 VTS_PI4 VTS_PI2 VTS_PR8 VTS_PR8 VTS_PI4 VTS_PI2 VTS_PR8 VTS_PR8)
   DISP_FUNCTION(API, "InitRealPartInfo", InitRealPartInfo, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "dfmDistanceCheckTwoNets", dfmDistanceCheckTwoNets, VT_I2, VTS_I2 VTS_BSTR VTS_BSTR VTS_R8 VTS_R8 VTS_R8 VTS_BSTR VTS_I2 VTS_BSTR VTS_BOOL VTS_BOOL)
   DISP_FUNCTION(API, "dfmDistanceCheckByPackage", dfmDistanceCheckByPackage, VT_I2, VTS_I2 VTS_BSTR VTS_BSTR VTS_BSTR VTS_R8 VTS_R8 VTS_R8 VTS_BSTR VTS_I2)
   DISP_FUNCTION(API, "dfmSetMaxErrorsForTest", dfmSetMaxErrorsForTest, VT_I2, VTS_I2)
   DISP_FUNCTION(API, "dfmHaltTest", dfmHaltTest, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "GetTypeArray", GetTypeArray, VT_I2, VTS_I4 VTS_PVARIANT)
   DISP_FUNCTION(API, "GetType", GetType, VT_I2, VTS_I4 VTS_I4 VTS_PBSTR VTS_PI2)
   DISP_FUNCTION(API, "SetTypeCurrentAttrib", SetTypeCurrentAttrib, VT_I2, VTS_I4 VTS_I4)
   DISP_FUNCTION(API, "dfmAcidTraps", dfmAcidTraps, VT_I2, VTS_I2 VTS_R8 VTS_R8 VTS_R8 VTS_BSTR)
   DISP_FUNCTION(API, "AddInsert", AddInsert, VT_I2, VTS_I2 VTS_I2 VTS_BSTR VTS_I2 VTS_R8 VTS_R8 VTS_R8 VTS_I2 VTS_I2 VTS_PI4)
   DISP_FUNCTION(API, "AddPolyEntity", AddPolyEntity, VT_I2, VTS_I2 VTS_I2 VTS_PI4)
   DISP_FUNCTION(API, "AddPoly", AddPoly, VT_I2, VTS_I2 VTS_I4 VTS_I2 VTS_PI4)
   DISP_FUNCTION(API, "AddPolyPnt", AddPolyPnt, VT_I2, VTS_I2 VTS_I4 VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_PI4)
   DISP_FUNCTION(API, "SetAttribVisible", SetAttribVisible, VT_I2, VTS_I2 VTS_BOOL VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I2 VTS_I2 VTS_BOOL VTS_BOOL)
   DISP_FUNCTION(API, "FindCompPin", FindCompPin, VT_I2, VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_PI4)
   DISP_FUNCTION(API, "GetNetCompPinData2", GetNetCompPinData2, VT_I2, VTS_I4 VTS_PVARIANT)
   DISP_FUNCTION(API, "CopyFile", CopyFile, VT_I2, VTS_I4 VTS_BSTR VTS_PI4)
   DISP_FUNCTION(API, "SetFileMirror", SetFileMirror, VT_I2, VTS_I4 VTS_BOOL)
   DISP_FUNCTION(API, "DeleteEntity", DeleteEntity, VT_I2, VTS_I2 VTS_I4)
   DISP_FUNCTION(API, "RestructureFileAroundOrigin", RestructureFileAroundOrigin, VT_I2, VTS_I4)
   DISP_FUNCTION(API, "GetPlacementData2", GetPlacementData2, VT_I2, VTS_I4 VTS_I2 VTS_PVARIANT)
   DISP_FUNCTION(API, "GetDRC2", GetDRC2, VT_I2, VTS_I4 VTS_I4 VTS_PBSTR VTS_PR8 VTS_PR8 VTS_PI2 VTS_PI2 VTS_PI2 VTS_PI4 VTS_PBSTR VTS_PI2)
   DISP_FUNCTION(API, "GetDRCNets", GetDRCNets, VT_I2, VTS_I4 VTS_I4 VTS_PVARIANT)
   DISP_FUNCTION(API, "DeleteByLayer", DeleteByLayer, VT_I2, VTS_I2 VTS_BOOL)
   DISP_FUNCTION(API, "DeleteByInserttype", DeleteByInserttype, VT_I2, VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "DeleteByGraphicClass", DeleteByGraphicClass, VT_I2, VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "SplashMin", SplashMin, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "SetFileLocation", SetFileLocation, VT_I2, VTS_I4 VTS_R8 VTS_R8)
   DISP_FUNCTION(API, "AutoBoardOrigin", AutoBoardOrigin, VT_I2, VTS_I4 VTS_BOOL VTS_BOOL)
   DISP_FUNCTION(API, "ClearAllDRCs", ClearAllDRCs, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "OEMLicense", OEMLicense, VT_I2, VTS_BSTR VTS_I2 VTS_I4)
   DISP_FUNCTION(API, "SetCentroid", SetCentroid, VT_I2, VTS_I2 VTS_R4 VTS_R4 VTS_R4 VTS_I2)
   DISP_FUNCTION(API, "AutoGenerateCentroid", AutoGenerateCentroid, VT_I2, VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "GetCentroid", GetCentroid, VT_I2, VTS_I2 VTS_PR4 VTS_PR4 VTS_PR4)
   DISP_FUNCTION(API, "ExportGENCAD", ExportGENCAD, VT_I2, VTS_BSTR)
   DISP_FUNCTION(API, "GetSchematicViewArray", GetSchematicViewArray, VT_I2, VTS_PVARIANT)
   DISP_FUNCTION(API, "GetSchematicSheetArray", GetSchematicSheetArray, VT_I2, VTS_I4 VTS_PVARIANT)
   DISP_FUNCTION(API, "GetLogicSymbolPlacementData", GetLogicSymbolPlacementData, VT_I2, VTS_I2 VTS_PVARIANT)
   DISP_FUNCTION(API, "GetSchematicNetData", GetSchematicNetData, VT_I2, VTS_I4 VTS_PVARIANT)
   DISP_FUNCTION(API, "SetDataPolyFilled", SetDataPolyFilled, VT_I2, VTS_I2 VTS_I4 VTS_I4 VTS_I2)
   DISP_FUNCTION(API, "SetDataPolyClosed", SetDataPolyClosed, VT_I2, VTS_I2 VTS_I4 VTS_I4 VTS_I2)
   DISP_FUNCTION(API, "SetDataPolyVoid", SetDataPolyVoid, VT_I2, VTS_I2 VTS_I4 VTS_I4 VTS_I2)
   DISP_FUNCTION(API, "SetDataPolyWidthIndex", SetDataPolyWidthIndex, VT_I2, VTS_I2 VTS_I4 VTS_I4 VTS_I2)
   DISP_FUNCTION(API, "SetDataGraphicClass", SetDataGraphicClass, VT_I2, VTS_I2 VTS_I4 VTS_I2)
   DISP_FUNCTION(API, "EditGeometry", EditGeometry, VT_I2, VTS_I2)
   DISP_FUNCTION(API, "OpenCadFile", OpenCadFile, VT_I2, VTS_BSTR)
   DISP_FUNCTION(API, "SetSchematicNetCrossProbe", SetSchematicNetCrossProbe, VT_I2, VTS_BOOL)
   DISP_FUNCTION(API, "SetSchematicComponentCrossProbe", SetSchematicComponentCrossProbe, VT_I2, VTS_BOOL)
   DISP_FUNCTION(API, "SetSchematicSheet", SetSchematicSheet, VT_I2, VTS_I4 VTS_I2)
   DISP_FUNCTION(API, "LoadSchematicNetCrossReference", LoadSchematicNetCrossReference, VT_I2, VTS_BSTR)
   DISP_FUNCTION(API, "LoadSchematicComponentCrossReference", LoadSchematicComponentCrossReference, VT_I2, VTS_BSTR)
   DISP_FUNCTION(API, "CrossProbeSchematicComponent", CrossProbeSchematicComponent, VT_I2, VTS_BSTR)
   DISP_FUNCTION(API, "CrossProbeSchematicNet", CrossProbeSchematicNet, VT_I2, VTS_BSTR)
   DISP_FUNCTION(API, "GetCAMCADVersion", GetCAMCADVersion, VT_I2, VTS_PBSTR)
   DISP_FUNCTION(API, "DeleteGenericGraphic", DeleteGenericGraphic, VT_I2, VTS_BOOL VTS_BOOL)
   DISP_FUNCTION(API, "DeleteTracesVias", DeleteTracesVias, VT_I2, VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "OpenNewDocument", OpenNewDocument, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "LoadSTDLicenses", LoadSTDLicenses, VT_I2, VTS_NONE)
   DISP_FUNCTION(API, "SetWindowName", SetWindowName, VT_I2, VTS_BSTR)
   //DISP_FUNCTION(API, "SaveCompressedDataFileAs", SaveCompressedDataFileAs, VT_I2, VTS_BSTR)
   DISP_FUNCTION(API, "SendLayerToBack", SendLayerToBack, VT_I2, VTS_I2)
   DISP_FUNCTION(API, "BringLayerToFront", BringLayerToFront, VT_I2, VTS_I2)
   DISP_FUNCTION(API, "SetWindowIcon", SetWindowIcon, VT_I2, VTS_BSTR VTS_I2)
   DISP_FUNCTION(API, "GetRealPartData", GetRealPartData, VT_I2, VTS_PBSTR)
   DISP_FUNCTION(API, "SetRealPartData", SetRealPartData, VT_I2, VTS_BSTR)
   DISP_FUNCTION(API, "GenerateCompCentroid", GenerateCompCentroid, VT_I2, VTS_I4 VTS_I2)
   DISP_FUNCTION(API, "GetNetCompPinData3", GetNetCompPinData3, VT_I2, VTS_I4 VTS_PVARIANT)
   DISP_FUNCTION(API, "ExportFormat", ExportFormat, VT_I2, VTS_I2 VTS_BSTR VTS_BSTR)
   DISP_FUNCTION(API, "GetProbeData", GetProbeData, VT_I2, VTS_I4 VTS_PVARIANT)
   DISP_FUNCTION(API, "SortPanelBoards", SortPanelBoards, VT_I2, VTS_I4 VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION(API, "LockDocument", LockDocument, VT_I2, VTS_I4 VTS_I2 VTS_BSTR)
   DISP_FUNCTION(API, "GetDataExtents", GetDataExtents, VT_I2, VTS_I2 VTS_I4 VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8)
   DISP_FUNCTION(API, "AddRealPartDrcMarker", AddRealPartDrcMarker, VT_I2, VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR VTS_R4 VTS_R4 VTS_I2 VTS_I2 VTS_BSTR VTS_PI4)
   DISP_FUNCTION(API, "RemoveRealPartDrcMarker", RemoveRealPartDrcMarker, VT_I2, VTS_I4 VTS_BSTR)
   DISP_FUNCTION(API, "DeleteOccuranceOfKeyword", DeleteOccuranceOfKeyword, VT_I2, VTS_I2)
   DISP_FUNCTION(API, "GetProjectPathAPI", GetProjectPathAPI, VT_I2, VTS_PBSTR)
   DISP_FUNCTION(API, "Validate", Validate, VT_I2, VTS_I4 VTS_PI4)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()
 

// Note: we add support for IID_IAPI to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {C3DB3A2F-0B71-11D2-BA40-0080ADB36DBB}
static const IID IID_IAPI =
{ 0xc3db3a2f, 0xb71, 0x11d2, { 0xba, 0x40, 0x0, 0x80, 0xad, 0xb3, 0x6d, 0xbb } };

BEGIN_INTERFACE_MAP(API, CCmdTarget)
   INTERFACE_PART(API, IID_IAPI, Dispatch)
END_INTERFACE_MAP()

// {C3DB3A2D-0B71-11D2-BA40-0080ADB36DBB}
IMPLEMENT_OLECREATE(API, "CAMCAD.Application", 0xc3db3a2d, 0xb71, 0x11d2, 0xba, 0x40, 0x0, 0x80, 0xad, 0xb3, 0x6d, 0xbb)


/******************************************************************************
* SetAPILevel
*/
short API::SetAPILevel(short code) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   switch (code)
   {
   case 1:
      APIOverrideLevel = 1;
      return RC_SUCCESS;
      
   case 2:
      APIOverrideLevel = 2;
      return RC_SUCCESS;
      
   case 3:
      APIOverrideLevel = 3;
      return RC_SUCCESS;
      
   case 4:
      APIOverrideLevel = 4;
      return RC_SUCCESS;
      
   default:
      APIOverrideLevel = 0;   
      return RC_GENERAL_ERROR;
   }
}

/******************************************************************************
* GetAPIVersion
*/
short API::GetAPIVersion(short FAR* majorVersionNumber, short FAR* minorVersionNumber) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif
	// please keep a history of changes as we do in CAMCAD version camcad.cpp
	*majorVersionNumber = 1;
	//*minorVersionNumber = 12;   // added license
	//*minorVersionNumber = 13;   // DFM distance check
	//*minorVersionNumber = 14;   // Layer Visible/Editable/Stackup
	//*minorVersionNumber = 15;   // GetGeometryExtents & GetCentroidData
	//*minorVersionNumber = 16;   // HideCAMCAD, SplashMessage, SplashProgressRange, SplashProgressPosition
	//*minorVersionNumber = 17;   // ManRefDes, ExportDXF
	//*minorVersionNumber = 18;   // FindCompPin, GetCompPinData2
	//*minorVersionNumber = 19;   // CopyFile
	//*minorVersionNumber = 20;   // SetFileMirror, RestructureFileAroundOrigin, DeleteEntity, GetPlacementData2
	//*minorVersionNumber = 21;   // GetDRC2
	//*minorVersionNumber = 22;   // GetDRCNets, DeleteByLayer, DeleteByInserttype, DeleteByGraphicClass, SplashMin, SetFileOffset
	//*minorVersionNumber = 23;   // AutoBoardOrigin
	//*minorVersionNumber = 24;   // DFM Interface
	//*minorVersionNumber = 25;   // DFM Overhall / Reorder
	//*minorVersionNumber = 26;   // DFM API Changes
	//*minorVersionNumber = 27;   // SetCentroid, AutoGenerateCentroid, GetCentroid
	//*minorVersionNumber = 28;   // SetCentroid & GetCentroid now have rotation also
	//*minorVersionNumber = 29;   // GetSchematicViewArray(), GetSchematicSheetArray(), GetLogicSymbolPlacementData(), GetSchematicNetData()
	//*minorVersionNumber = 30;   // SetDataPolyFilled(), SetDataPolyClosed(), SetDataPolyVoid(), SetDataPolyWidthIndex()
	//*minorVersionNumber = 31;   // SetDataGraphicClass()
	//*minorVersionNumber = 32;   // EditGeometry()
	//*minorVersionNumber = 33;   // OpenCadFile()
	//*minorVersionNumber = 34;   // GetCAMCADVersion()
	//*minorVersionNumber = 35;   // OpenNewDocument, DeleteTracesVias, DeleteGenericGraphics
	//*minorVersionNumber = 36;   // DFM GerberThermalBarrelPlugCheck
	//*minorVersionNumber = 37;   // LoadSTDLicenses
	//*minorVersionNumber = 38;   // DFM Pin1Orientation, TestPadDensityCheck
	//*minorVersionNumber = 39;   // SetWindowName
	//*minorVersionNumber = 40;   // DistanceCheck2 and DFM ComponentNotAllowedPerSurface and ComponentHeight
	//*minorVersionNumber = 41;   // SaveCompressedDataFileAs
	//*minorVersionNumber = 42;   // RestructureCompData and GetRealPartData
	//*minorVersionNumber = 43;   // SendLayerToBack and BringLayerToFront
	//*minorVersionNumber = 44;   // DFM CombinePolies
	//*minorVersionNumber = 45;   // SetRealPartData
	//*minorVersionNumber = 46;   // SetWindowIcon
	//*minorVersionNumber = 47;  // removed RestructureCompData
	//*minorVersionNumber = 48;  // added surface parameter to DFM FlattenByInsertType and FlattenByAttrib
	//*minorVersionNumber = 49;  // added GenerateCompCentroid
	//*minorVersionNumber = 50;  // added SortPanelBoards
	//*minorVersionNumber = 51;  // modified SortPanelBoards (added new parameter)
	//*minorVersionNumber = 52;  // Added LockDocument
	//*minorVersionNumber = 53;  // added GetDataExtents function
	*minorVersionNumber = 54;  // Added AddRealPartDrcMarker, RemoveRealPartDrcMarker


   return RC_SUCCESS;
}

/******************************************************************************
* GetCAMCADVersion
*/
short API::GetCAMCADVersion(BSTR FAR* ccVersion) 
{
   *ccVersion = getApp().getVersionString().AllocSysString();

   return RC_SUCCESS;
}

/******************************************************************************
* IssueCommand
*/
short API::IssueCommand(long command) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

/*
   if (!APILevelAllowed(4))   // this is the API level check !
      return RC_NO_LICENSE;
*/
   SendMessage(AfxGetMainWnd()->m_hWnd, WM_COMMAND, command, 0L);

   return RC_SUCCESS;
}


/******************************************************************************
* GraphAperture
*/
short API::GraphAperture(LPCTSTR name, short shape, double sizeA, double sizeB, double xOffset, double yOffset, 
      double rotation, short dcode) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   doc->StoreDocForImporting();

   Graph_Aperture(name, shape, sizeA, sizeB, xOffset, yOffset, DegToRad(rotation), dcode, 0L, FALSE, NULL);

   return RC_SUCCESS;
}

/******************************************************************************
* OverrideColorData
*/
short API::OverrideColorData(short geometryNumber, long dataPosition, BOOL On, short red, short green, short blue) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (geometryNumber < 0 || geometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *block = doc->getBlockAt(geometryNumber);

   if (!block)
      return RC_ITEM_NOT_FOUND;

   // find DATA
   POSITION pos = (POSITION)dataPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   DataStruct *data = block->getDataList().GetAt(pos);

   if (On)
   {
      data->setOverrideColor(RGB(red, green, blue));
      data->setColorOverride(true);
   }
   else
   {
      data->setColorOverride(false);
   }

   if (API_AutoRedraw)
      doc->UpdateAllViews(NULL);

   return RC_SUCCESS;
}

/******************************************************************************
* OverrideColors
*/
short API::OverrideColorByRefDes(LPCTSTR RefDes, BOOL On, short red, short green, short blue) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden() || file->notPlacedYet() || file->getBlock() == NULL)
         continue;

      POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

         if (data->getDataType() == T_INSERT)
         {
            if (data->getInsert()->getRefname() && !STRICMP(RefDes, data->getInsert()->getRefname()))
            {
               if (On)
               {
                  data->setOverrideColor(RGB(red, green, blue));
                  data->setColorOverride(true);
               }
               else
               {
                  data->setColorOverride(false);
               }
               
               if (API_AutoRedraw)
                  doc->UpdateAllViews(NULL);

               return RC_SUCCESS;
            }
         }
      }
   }

   return RC_ITEM_NOT_FOUND;
}

/******************************************************************************
* OverrideColorByGeometry
*/
short API::OverrideColorByGeometry(short geometryNumber, BOOL On, short red, short green, short blue) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   FileStruct *file;
   DataStruct *data;

   POSITION filePos, dataPos;
   filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      file = doc->getFileList().GetNext(filePos);
      if (!file->isShown() || file->isHidden() || file->notPlacedYet() || file->getBlock() == NULL)
         continue;

      dataPos = file->getBlock()->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         data = file->getBlock()->getDataList().GetNext(dataPos);

         if (data->getDataType() == T_INSERT)
         {
            if (data->getInsert()->getBlockNumber() == geometryNumber)
            {
               if (On)
               {
                  data->setOverrideColor(RGB(red, green, blue));
                  data->setColorOverride(true);
               }
               else
               {
                  data->setColorOverride(false);
               }
               
               if (API_AutoRedraw)
                  doc->UpdateAllViews(NULL);

               return RC_SUCCESS;
            }
         }
      }
   }

   return RC_ITEM_NOT_FOUND;
}

/******************************************************************************
* OverrideColorByInsertType
*/
short API::OverrideColorByInsertType(short insertType, BOOL On, short red, short green, short blue) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   FileStruct *file;
   DataStruct *data;

   POSITION filePos, dataPos;
   filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      file = doc->getFileList().GetNext(filePos);
      if (!file->isShown() || file->isHidden() || file->notPlacedYet() || file->getBlock() == NULL)
         continue;

      dataPos = file->getBlock()->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         data = file->getBlock()->getDataList().GetNext(dataPos);

         if (data->getDataType() == T_INSERT)
         {
            if (data->getInsert()->getInsertType() == insertType)
            {
               if (On)
               {
                  data->setOverrideColor(RGB(red, green, blue));
                  data->setColorOverride(true);
               }
               else
               {
                  data->setColorOverride(false);
               }
            }
         }
      }
   }

   if (API_AutoRedraw)
      doc->UpdateAllViews(NULL);

   return RC_SUCCESS;
}

/******************************************************************************
* OverrideColorByAttribute
*/
short API::OverrideColorByAttribute(LPCTSTR keyword, LPCTSTR value, BOOL On, short red, short green, short blue) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   int iKeyword = doc->IsKeyWord(keyword, 0);
   if (iKeyword == -1)
      return RC_GENERAL_ERROR;
   int iValue = 0;
   if (strlen(value))
      iValue = doc->RegisterValue(value);

   BOOL Found = FALSE;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);
         Attrib* attrib;

         if (data->getAttributesRef() && data->getAttributesRef()->Lookup(iKeyword, attrib))
         {
            if (!strlen(value) || (attrib->getValueType() == VT_STRING && attrib->getStringValueIndex() == iValue))
            {
               if (On)
               {
                  data->setOverrideColor(RGB(red, green, blue));
                  data->setColorOverride(true);
               }
               else
               {
                  data->setColorOverride(false);
               }

               Found = TRUE;
            }
         }     
      }
   }

   if (Found)
   {
      if (API_AutoRedraw)
         doc->UpdateAllViews(NULL);

      return RC_SUCCESS;
   }
   else
      return RC_ITEM_NOT_FOUND;
}

short API::OverrideColorDRC(long entityNumber, BOOL On, short red, short green, short blue) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   FileStruct *file;
   DataStruct *data;

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      file = doc->getFileList().GetNext(filePos);
      if (!file->isShown() || file->isHidden() || file->notPlacedYet() || file->getBlock() == NULL)
         continue;

      POSITION drcPos = file->getDRCList().GetHeadPosition();
      while (drcPos != NULL)
      {
         DRCStruct *drc = file->getDRCList().GetNext(drcPos);

         if (drc->getEntityNumber() == entityNumber)
         {
            POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
            while (dataPos != NULL)
            {
               data = file->getBlock()->getDataList().GetNext(dataPos);

               if (data->getEntityNumber() == drc->getInsertEntityNumber())
               {
                  if (On)
                  {
                     data->setOverrideColor(RGB(red, green, blue));
                     data->setColorOverride(true);
                  }
                  else
                  {
                     data->setColorOverride(false);
                  }
                  
                  if (API_AutoRedraw)
                     doc->UpdateAllViews(NULL);

                  return RC_SUCCESS;
               }
            }
         }
      }
   }

   return RC_ITEM_NOT_FOUND;
}

short API::OverrideColorRemoveAll() 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = ((CCEtoODBView*)apiView)->GetDocument();

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);
         data->setColorOverride(false);
      }
   }

   if (API_AutoRedraw)
      doc->UpdateAllViews(NULL);

   return RC_SUCCESS;
}


/******************************************************************************
* Exit
*/
short API::Exit() 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   PostQuitMessage(0);
   return RC_SUCCESS;
}


/******************************************************************************
* LoadDataFile
*/
short API::LoadDataFile(LPCTSTR filename) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   FILE *stream = fopen(filename, "r"); // text mode by default
   if (stream == NULL)
   {
      return RC_ITEM_NOT_FOUND; 
   }
   fclose(stream);

   CAMCAD_File = filename;

   LoadingDataFile = TRUE;
               
   ((CCEtoODBApp*)(AfxGetApp()))->OnFileNew();
               
   LoadingDataFile = FALSE;

   CCEtoODBView *view = (CCEtoODBView*)apiView;
   if (view)
   {
      CCEtoODBDoc *doc = view->GetDocument();
      doc->SetTitle(CAMCAD_File);
      doc->docTitle = CAMCAD_File;
      doc->GenerateSmdComponentTechnologyAttribs(NULL, false);
   }

   return RC_SUCCESS;
}

/******************************************************************************
* SaveDataFileAs
*/
short API::SaveDataFileAs(LPCTSTR filename) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   //doc->SaveDataFileAs(filename);

   return RC_SUCCESS;
}

/******************************************************************************
* SaveCompressedDataFileAs
*/
short API::SaveCompressedDataFileAs(LPCTSTR filename) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   //doc->SaveCompressedDataFileAs(filename, ccFileTypeXmlPkZip);

   return RC_SUCCESS;
}

/******************************************************************************
* Zoom
*/
short API::ZoomLevel(double scaleDenom, short scaleNum) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBView *view = (CCEtoODBView*)apiView;

   if (scaleDenom < 1.01 && scaleNum <= 1)
   {
      view->OnZoom1To1();
      return RC_SUCCESS;
   }

   if (scaleDenom > 1.01)
   {
      view->ScaleDenom = scaleDenom;
      view->ScaleNum = 1;
   }
   else
   {
      view->ScaleDenom = 1.0;
      if (scaleNum < 1)
         view->ScaleNum = 1;
      else
         view->ScaleNum = scaleNum;
   }

   view->UpdateScale();
   view->Invalidate();

   return RC_SUCCESS;
}

short API::ZoomWindow(double x1, double y1, double x2, double y2) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBView *view = (CCEtoODBView*)apiView;
   view->ZoomBox(min(x1, x2), max(x1, x2), min(y1, y2), max(y1, y2));

   return RC_SUCCESS;
}

short API::PanRef(LPCTSTR refdes) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBView *view = (CCEtoODBView*)apiView;
   
   return PanReference(view, refdes);
}

short API::PanCoord(double x, double y) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBView *view = (CCEtoODBView*)apiView;

   view->PanCoordinate(x, y, TRUE);

   return RC_SUCCESS;
}

short API::Redraw() 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   doc->UpdateAllViews(NULL);

   return RC_SUCCESS;
}

/******************************************************************************
* Layers
*/
short API::LayerColor(LPCTSTR name, short red, short green, short blue) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   for (int i=0; i < doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];
      if (layer == NULL)   continue;
      if (!layer->getName().CompareNoCase(name))
      {
         COLORREF color = RGB(red, green, blue);
         layer->setColor(color);
         if (API_AutoRedraw)
            doc->UpdateAllViews(NULL);
         return RC_SUCCESS;
      }
   }

   return RC_ITEM_NOT_FOUND;
}

short API::LayerShow(LPCTSTR name, BOOL show) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   for (int i=0; i < doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];
      if (layer == NULL)   continue;
      if (!layer->getName().CompareNoCase(name))
      {
         layer->setVisible(show);
         if (API_AutoRedraw)
            doc->UpdateAllViews(NULL);
         return RC_SUCCESS;
      }
   }

   return RC_ITEM_NOT_FOUND;
}

short API::LayerShowAll(BOOL show) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   for (int i=0; i < doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];
      if (layer == NULL)   continue;
      layer->setVisible(show);
   }

   if (API_AutoRedraw)
      doc->UpdateAllViews(NULL);

   return RC_SUCCESS;
}

short API::TopView() 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   doc->OnTopview();

   return RC_SUCCESS;
}

short API::BottomView() 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   doc->OnBottomview();

   return RC_SUCCESS;
}

short API::ColorSet(short set) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   doc->UseColorSet(set, doc->getBottomView());

   if (API_AutoRedraw)
      doc->UpdateAllViews(NULL);

   return RC_SUCCESS;
}

short API::ClearSelected() 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();
   //doc->OnClearSelected();

   return RC_SUCCESS;
}

/******************************************************************************
* API::ZoomExtentsOfNets
*/
short API::ZoomExtentsOfNets(LPCTSTR netNames) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   double xmin, xmax, ymin, ymax;
   xmin = ymin = DBL_MAX;
   xmax = ymax = -DBL_MAX;

   char *buf = STRDUP(netNames);
   char *tok = strtok(buf, ", \t\n");
   
      while (tok)
      {
         double left, right, top, bottom;

         NetExtents(doc, tok, &left, &right, &bottom, &top);

         if (left < xmin) xmin = left;
         if (right > xmax) xmax = right;
         if (bottom < ymin) ymin = bottom;
         if (top > ymax) ymax = top;

         tok = strtok(NULL, ", \t\n");
      }
      if (buf != NULL)
         free (buf);

      if (xmin > xmax)
      {
         ErrorMessage("None Found", "ZoomExtentsOfNets");
         return RC_ITEM_NOT_FOUND;
      }

      double margin = (xmax - xmin) * doc->getSettings().ExtentMargin / 200;
      xmin -= margin;
      xmax += margin;
      
      margin = (ymax - ymin) * doc->getSettings().ExtentMargin / 200;
      ymin -= margin;
      ymax += margin;

      ZoomWindow(xmin, ymin, xmax, ymax);
      
      return RC_SUCCESS;      
   
}

/******************************************************************************
* API::ZoomExtentsOfNetsMargin
*/
short API::ZoomExtentsOfNetsMargin(LPCTSTR netNames, short margin) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   double xmin, xmax, ymin, ymax;
   xmin = ymin = DBL_MAX;
   xmax = ymax = -DBL_MAX;

   char *buf = STRDUP(netNames);
   char *tok = strtok(buf, ", \t\n");
   while (tok)
   {
      double left, right, top, bottom;

      NetExtents(doc, tok, &left, &right, &bottom, &top);

      if (left < xmin) xmin = left;
      if (right > xmax) xmax = right;
      if (bottom < ymin) ymin = bottom;
      if (top > ymax) ymax = top;

      tok = strtok(NULL, ", \t\n");
   }

   free(buf);

   if (xmin > xmax)
   {
      ErrorMessage("None Found", "ZoomExtentsOfNets");
      return RC_ITEM_NOT_FOUND;
   }

   double pageWidth = doc->getSettings().getXmax() - doc->getSettings().getXmin();
   double pageHeight = doc->getSettings().getYmax() - doc->getSettings().getYmin();
   double pageSize = max(pageWidth, pageWidth);

   double marginSize = pageSize * margin / 100;

   xmin -= marginSize;
   xmax += marginSize;
   
   ymin -= marginSize;
   ymax += marginSize;

   ZoomWindow(xmin, ymin, xmax, ymax);

   return RC_SUCCESS;
}

short API::CloseActiveDocument() 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   doc->OnCloseDocument();

   return RC_SUCCESS;
}

/******************************************************************************
* ShadeByRefDes
*/
short API::ShadeByRefDes(LPCTSTR refDes, BOOL On, short red, short green, short blue, BOOL Before, BOOL Transparent, BOOL Hatched) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();
   
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden() || file->notPlacedYet() || file->getBlock() == NULL)
         continue;

      POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

         if (data->getDataType() == T_INSERT && data->getInsert()->getRefname() && !STRICMP(data->getInsert()->getRefname(), refDes))
         {
            data->getInsert()->getShadingRef().On          = On;
            data->getInsert()->getShadingRef().red         = (unsigned char)red;
            data->getInsert()->getShadingRef().green       = (unsigned char)green;
            data->getInsert()->getShadingRef().blue        = (unsigned char)blue;
            data->getInsert()->getShadingRef().Before      = Before; // draw shading rectangle before drawing the block
            data->getInsert()->getShadingRef().Transparent = Transparent;
            data->getInsert()->getShadingRef().Hatched     = Hatched;

            if (API_AutoRedraw)
               doc->UpdateAllViews(NULL);

            return RC_SUCCESS;
         }
      }
   }

   return RC_ITEM_NOT_FOUND;
}

short API::RemoveShading() 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      POSITION pos = block->getDataList().GetHeadPosition();
      while (pos != NULL)
      {
         DataStruct *data = block->getDataList().GetNext(pos);
         if (data->getDataType() == T_INSERT)
            data->getInsert()->getShadingRef().On = FALSE;
      }
   }

   if (API_AutoRedraw)
      doc->UpdateAllViews(NULL);
   
   return RC_SUCCESS;
}

/******************************************************************************
* API::ShowMainWindow()
*
*  - See Below for values
*/
short API::ShowMainWindow(short mode) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (mode < 0 || mode > SW_MAX)
      return RC_VALUE_OUT_OF_RANGE;

   //switch (mode)
   //{
   //case 0: // SW_HIDE
   //case 2: // SW_SHOWMINIMIZED
   //case 6: // SW_MINIMIZE
   //case 7: // SW_SHOWMINNOACTIVE
   //case 11: // SW_SHOWMINIMIZED
   // return RC_NOT_ALLOWED;
   //}

   AfxGetMainWnd()->ShowWindow(mode);
   return RC_SUCCESS;
}

/******************************************************************************
* API::ShowActiveDocument()
*
*  - See Below for values
*/
short API::ShowActiveDocument(short mode) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBView *view = (CCEtoODBView*)apiView;

   if (mode < 0 || mode > 11)
      return RC_VALUE_OUT_OF_RANGE;

   view->GetParentFrame()->ShowWindow(mode);

   return RC_SUCCESS;
}

/*
SW_HIDE             0
SW_SHOWNORMAL       1
SW_NORMAL           1
SW_SHOWMINIMIZED    2
SW_SHOWMAXIMIZED    3
SW_MAXIMIZE         3
SW_SHOWNOACTIVATE   4
SW_SHOW             5
SW_MINIMIZE         6
SW_SHOWMINNOACTIVE  7
SW_SHOWNA           8
SW_RESTORE          9
SW_SHOWDEFAULT      10
SW_FORCEMINIMIZE    11
SW_MAX              11

SW_HIDE   Hides this window and passes activation to another window.
SW_MINIMIZE   Minimizes the window and activates the top-level window in the system’s list.
SW_RESTORE   Activates and displays the window. If the window is minimized or maximized, Windows restores it to its original size and position.
SW_SHOW   Activates the window and displays it in its current size and position.
SW_SHOWMAXIMIZED   Activates the window and displays it as a maximized window.
SW_SHOWMINIMIZED   Activates the window and displays it as an icon.
SW_SHOWMINNOACTIVE   Displays the window as an icon. The window that is currently active remains active.
SW_SHOWNA   Displays the window in its current state. The window that is currently active remains active.
SW_SHOWNOACTIVATE   Displays the window in its most recent size and position. The window that is currently active remains active.
SW_SHOWNORMAL   Activates and displays the window. If the window  is minimized or maximized, Windows restores it to its original size and position. 
SW_FORCEMINIMIZE Windows 2000: Minimizes a window, even if the thread that owns the window is hung. This flag should only be used when minimizing windows from a different thread.
*/


short API::SizeMainWindow(short x, short y, short width, short height) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   AfxGetMainWnd()->ShowWindow(SW_RESTORE);
   AfxGetMainWnd()->SetWindowPos(NULL, x, y, width, height, 0);

   return RC_SUCCESS;
}

short API::GraphTool(LPCTSTR name, float size, short tcode, short type) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   ((CCEtoODBDoc*)apiView->GetDocument())->StoreDocForImporting();
   Graph_Tool(name, tcode, size, type, 0, FALSE, 0L);
   return RC_SUCCESS;
}

/******************************************************************************
* HighlightNet
*/
short API::HighlightNet(LPCTSTR netName, short red, short green, short blue) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown())
         continue;

      POSITION netPos = file->getNetList().GetHeadPosition();
      while (netPos != NULL)
      {
         NetStruct *net = file->getNetList().GetNext(netPos);

         if (!net->getNetName().Compare(netName))
         {
            net->setHighlighted(true);
            int value = doc->RegisterValue(netName);
            doc->HighlightedNetsMap.SetAt(value, RGB(red, green, blue));

            doc->HighlightByAttrib(FALSE, doc->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, value);

            CClientDC dc(apiView);
            //apiView->OnPrepareDC(&dc);
            dc.SetTextAlign(TA_CENTER);
            dc.SetBkColor(RGB(255, 255, 255));
            dc.SetBkMode(TRANSPARENT);
            HighlightPins(&dc, (CCEtoODBView*)apiView);

            return RC_SUCCESS;
         }
      }
   }

   return RC_ITEM_NOT_FOUND;
}

/******************************************************************************
* HighlightNetClear
*/
short API::HighlightNetClear(LPCTSTR netName) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   int value = doc->RegisterValue(netName);
   doc->HighlightedNetsMap.RemoveKey(value);

   if (API_AutoRedraw)
      doc->UpdateAllViews(NULL);

   return RC_SUCCESS;
}

/******************************************************************************
* HighlightNetClearAll
*/
short API::HighlightNetClearAll() 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   doc->HighlightedNetsMap.RemoveAll();

   if (API_AutoRedraw)
      doc->UpdateAllViews(NULL);

   return RC_SUCCESS;
}

/******************************************************************************
* HighlightPin
*/
short API::HighlightPin(LPCTSTR comp, LPCTSTR pin, short red, short green, short blue) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBView *view = (CCEtoODBView*)apiView;
   CCEtoODBDoc *doc = (CCEtoODBDoc*)view->GetDocument();

   HighlightAPin(doc, comp, pin, RGB(red, green, blue));

   if (API_AutoRedraw)
   {
      CClientDC dc(apiView);
      apiView->OnPrepareDC(&dc);
      dc.SetTextAlign(TA_CENTER);
      dc.SetBkColor(RGB(255, 255, 255));
      dc.SetBkMode(TRANSPARENT);
      InitPens(&dc, doc->getSettings().FillStyle);
      HighlightPins(&dc, (CCEtoODBView*)apiView);
   }

   return RC_SUCCESS;
}

/******************************************************************************
* HighlightPinsOfANet
*/
short API::HighlightPinsOfANet(LPCTSTR netName, short red, short green, short blue) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBView *view = (CCEtoODBView*)apiView;
   CCEtoODBDoc *doc = apiView->GetDocument();

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown())
         continue;

      POSITION netPos = file->getNetList().GetHeadPosition();
      while (netPos)
      {
         NetStruct *net = file->getNetList().GetNext(netPos);

         if (!net->getNetName().Compare(netName))
         {
            POSITION cpPos = net->getHeadCompPinPosition();
            while (cpPos)
            {
               CompPinStruct *cp = net->getNextCompPin(cpPos);

               HighlightAPin(doc, cp->getRefDes(), cp->getPinName(), RGB(red, green, blue));
            }

            if (API_AutoRedraw)
            {
               CClientDC dc(apiView);
               //apiView->OnPrepareDC(&dc);
               dc.SetTextAlign(TA_CENTER);
               dc.SetBkColor(RGB(255, 255, 255));
               dc.SetBkMode(TRANSPARENT);
               HighlightPins(&dc, (CCEtoODBView*)apiView);
            }

            return RC_SUCCESS;
         }
      }
   }

   return RC_ITEM_NOT_FOUND;
}

/******************************************************************************
* HighlightPinsOfANetClear
*/
short API::HighlightPinsOfANetClear(LPCTSTR netName) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown())
         continue;

      POSITION netPos = file->getNetList().GetHeadPosition();
      while (netPos)
      {
         NetStruct *net = file->getNetList().GetNext(netPos);

         if (!net->getNetName().Compare(netName))
         {
            POSITION cpPos = net->getHeadCompPinPosition();
            while (cpPos)
            {
               CompPinStruct *cp = net->getNextCompPin(cpPos);

               POSITION hpPos = doc->HighlightedPinsList.GetHeadPosition();
               while (hpPos)
               {
                  HighlightedPinStruct *hp = doc->HighlightedPinsList.GetAt(hpPos);

                  if (!hp->comp.CompareNoCase(cp->getRefDes()) && !hp->pin.CompareNoCase(cp->getPinName()))
                  {
                     delete hp;
                     doc->HighlightedPinsList.RemoveAt(hpPos);
                     break;
                  }
                  doc->HighlightedPinsList.GetNext(hpPos);
               }
            }

            if (API_AutoRedraw)
               doc->UpdateAllViews(NULL);

            return RC_SUCCESS;
         }
      }
   }

   return RC_ITEM_NOT_FOUND;
}

/******************************************************************************
* HighlightPinClear
*/
short API::HighlightPinClear(LPCTSTR comp, LPCTSTR pin) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (HighlightAPinClear(doc, comp, pin))
   {
      if (API_AutoRedraw)
         doc->UpdateAllViews(NULL);
      return RC_SUCCESS;
   }
   else
      return RC_ITEM_NOT_FOUND;
}

/******************************************************************************
* HighlightPinClearAll
*/
short API::HighlightPinClearAll() 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   HighlightAPinClearAll(doc);

   if (API_AutoRedraw)
      doc->UpdateAllViews(NULL);

   return RC_SUCCESS;
}

/******************************************************************************
* GeneratePinLocs
*/
short API::GeneratePinLocs() 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();
   doc->OnGeneratePinloc();

   return RC_SUCCESS;
}

short API::DrawCross(double x, double y) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBView *view = (CCEtoODBView*)apiView;

   CClientDC dc(view);
   //view->OnPrepareDC(&dc);

   //view->DrawCross(&dc, round(x * view->getScaleFactor()), round(y * view->getScaleFactor()), 2);

   return 0;
}

short API::DeleteDRC(long entityNumber) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      POSITION drcPos = file->getDRCList().GetHeadPosition();
      while (drcPos != NULL)
      {
         DRCStruct *drc = file->getDRCList().GetNext(drcPos);

         if (drc->getEntityNumber() == entityNumber)
         {
            if (!RemoveOneDRC(doc, drc, file))
               return RC_SUCCESS;
            else
               return RC_GENERAL_ERROR;
         }
      }
   }

   return RC_ITEM_NOT_FOUND;
}


short API::GetActiveFile(long FAR* filePosition) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   *filePosition = NULL;

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = doc->getFileList().GetAt(pos);
      if (!file->isHidden() && file->isShown())
      {
         *filePosition = (long)pos;
         return RC_SUCCESS;
      }
      
      doc->getFileList().GetNext(pos);
   }

   return RC_ITEM_NOT_FOUND;
}


/******************************************************************************
* API::LockCAMCAD
*/
short API::LockCAMCAD(short lockCodes) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (!lockCodes)
   {
      LockApplication = FALSE;
      LockDocuments = FALSE;
      LockLicenses = FALSE;
   }
   else
   {
      if (lockCodes & LOCK_APPLICATION)
         LockApplication = TRUE;

      if (lockCodes & LOCK_DOCUMENTS)
         LockDocuments = TRUE;

      if (lockCodes & LOCK_LICENSES)
         LockLicenses = TRUE;
   }

   return RC_SUCCESS;
}

/******************************************************************************
* API::LockCAMCAD
*/
short API::LockDocument(long documentPosition, short lockDoc, LPCTSTR userName) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   POSITION pos = pDocTemplate->GetFirstDocPosition();
   while (pos != NULL)
   {
      if (pos == (POSITION)documentPosition)
      {
			CCEtoODBDoc *pDoc = (CCEtoODBDoc*)pDocTemplate->GetNextDoc(pos);
			if (pDoc == NULL)
				return RC_ILLEGAL_POSITION;

			if (lockDoc)
			{
				// lock if not already locked
				if (!pDoc->Lock(userName))
					return RC_NOT_ALLOWED;
			}
			else
			{
				// unlock if locked and the user name matches
				if (!pDoc->Unlock(userName))
					return RC_NOT_ALLOWED;
			}

			return RC_SUCCESS;
      }
      pDocTemplate->GetNextDoc(pos);
   }

   return RC_ITEM_NOT_FOUND;
}

/******************************************************************************
* API::LoadColorSet
*/
short API::LoadColorSet(LPCTSTR filename) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   LoadLayerTypeInfo(doc, filename);

   return RC_SUCCESS;
}

/******************************************************************************
* API::GetViewSurface
*/
short API::GetViewSurface(short FAR* surface) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   *surface = doc->getBottomView();

   return RC_SUCCESS;
}

/******************************************************************************
* API::GetViewportCoords
*/
short API::GetViewportCoords(double FAR* left, double FAR* right, double FAR* top, double FAR* bottom) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBView *view = (CCEtoODBView*)apiView;

   if (!view->DrawVariablesInitialized)
      return RC_UNINITIALIZED_VARIABLES;

   *left = view->pageRect.left;
   *right = view->pageRect.right;
   *top = view->pageRect.top;
   *bottom = view->pageRect.bottom;

   return RC_SUCCESS;
}

/******************************************************************************
* GetDocumentExtents
*/
short API::GetDocumentExtents(double FAR* left, double FAR* right, double FAR* top, double FAR* bottom) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   *left = doc->getSettings().getXmin();
   *right = doc->getSettings().getXmax();
   *top = doc->getSettings().getYmax();
   *bottom = doc->getSettings().getYmin();

   return RC_SUCCESS;
}

/******************************************************************************
* ShowInsertType
*/
short API::ShowInsertType(short insertType, BOOL On) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   doc->getInsertTypeFilter().set(insertType,On != NULL);

   if (API_AutoRedraw)
      doc->UpdateAllViews(NULL);

   return RC_SUCCESS;
}

/******************************************************************************
* ShowGraphicClass
*/
short API::ShowGraphicClass(short graphicClass, BOOL On) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   doc->getGraphicsClassFilter().set(graphicClass,On != 0);
   //doc->showClasses[graphicClass] = On;
   
   if (API_AutoRedraw)
      doc->UpdateAllViews(NULL);

   return RC_SUCCESS;
}

/******************************************************************************
* CurrentSettings
*/
short API::CurrentSettings(short fileNumber, short layerNumber) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   //SetCurrentFile(fileNumber);
   //SetCurrentLayer(layerNumber);

   return RC_SUCCESS;
}

/******************************************************************************
* API::AutoRedraw
*/
short API::AutoRedraw(BOOL On) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   API_AutoRedraw = On;

   return RC_SUCCESS;
}

/******************************************************************************
* API::GetZoomLevel
*/
short API::GetZoomLevel(double FAR* zoomLevel) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBView *view = (CCEtoODBView*)apiView;

   *zoomLevel = view->ScaleDenom / view->ScaleNum;

   return RC_SUCCESS;
}

/******************************************************************************
* API::SetZoomLevel
*/
short API::SetZoomLevel(double zoomLevel) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();
   CCEtoODBView *view = (CCEtoODBView*)apiView;

   if (fabs(zoomLevel - 1) < SMALLNUMBER)
   {
      view->OnZoom1To1();
      return RC_SUCCESS;
   }

   if (zoomLevel > 1)
   {
      view->ScaleDenom = zoomLevel;
      view->ScaleNum = 1;
   }
   else
   {
      view->ScaleDenom = 1.0;
      view->ScaleNum = (int)ceil(1.0 / zoomLevel);
   }

   view->UpdateScale();
   view->Invalidate();

   return RC_SUCCESS;
}

/******************************************************************************
* API::GetPageUnits()
*/
short API::GetPageUnits(BSTR FAR* units) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   CString buf = GetUnitName(doc->getSettings().getPageUnits());
   *units = buf.AllocSysString();

   return RC_SUCCESS;
}

/******************************************************************************
* ActivateDocument
*/
short API::ActivateDocument(long documentPosition) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   POSITION pos = pDocTemplate->GetFirstDocPosition();
   while (pos != NULL)
   {
      if (pos == (POSITION)documentPosition)
      {
         CDocument *doc = pDocTemplate->GetNextDoc(pos);

         POSITION viewPos = doc->GetFirstViewPosition();
         if (viewPos == NULL)
            return RC_GENERAL_ERROR;
         CView *view = doc->GetNextView(viewPos);
         view->GetParentFrame()->ActivateFrame(SW_SHOW);
         view->SetFocus();
//       view->GetParentFrame()->ActivateFrame(SW_SHOW);
//       ((CMainFrame*)AfxGetMainWnd())->SetActiveView(NULL);
//       ((CMainFrame*)AfxGetMainWnd())->SetActiveView(view);
         return RC_SUCCESS;
      }

      pDocTemplate->GetNextDoc(pos);
   }

   return RC_ITEM_NOT_FOUND;
}

/******************************************************************************
* APIDocument
*/
short API::APIDocument(long documentPosition) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   POSITION pos = pDocTemplate->GetFirstDocPosition();
   while (pos != NULL)
   {
      if (pos == (POSITION)documentPosition)
      {
         CDocument *doc = pDocTemplate->GetNextDoc(pos);
         POSITION viewPos = doc->GetFirstViewPosition();
         if (viewPos == NULL)
            return RC_GENERAL_ERROR;
         CView *view = doc->GetNextView(viewPos);
         apiView = (CCEtoODBView*)view;
         return RC_SUCCESS;
      }

      pDocTemplate->GetNextDoc(pos);
   }

   return RC_ITEM_NOT_FOUND;
}

/******************************************************************************
* HideData
*/
short API::HideData(short geometryNumber, long dataPosition, BOOL Hide) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (geometryNumber < 0 || geometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *block = doc->getBlockAt(geometryNumber);

   if (!block)
      return RC_ITEM_NOT_FOUND;

   // find DATA
   POSITION pos = (POSITION)dataPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   DataStruct *data = block->getDataList().GetAt(pos);

   data->setHidden(Hide);

   return RC_SUCCESS;
}

/******************************************************************************
* HideAll
*/
short API::HideAll(BOOL Hide) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   doc->OnHideRemoveAll();

   return RC_SUCCESS;
}

void CCEtoODBDoc::OnHideRemoveAll() 
{
   for (int i=0; i < getMaxBlockIndex(); i++)
   {
      BlockStruct *block = getBlockAt(i);

      if (!block)
         continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);
         data->setHidden(false);

         if (data->getDataType() == T_POLY)
         {
            POSITION pos = data->getPolyList()->GetHeadPosition();
            while (pos)
            {
               CPoly *poly = data->getPolyList()->GetNext(pos);
               poly->setHidden(false);
            }
         }
      }
   }

   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnHideTopLevel() 
{
   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = getFileList().GetNext(filePos);
      if (!file->isShown())
         continue;

      POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

         if (data->getDataType() != T_INSERT)
            data->setHidden(true);
      }
   }

   UpdateAllViews(NULL);
}

/******************************************************************************
* HideInserts
*/
short API::HideInserts(short inserttype, BOOL Hide) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   for (int i=0; i < doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);
         if (data->getDataType() == T_INSERT && (inserttype == -1 || data->getInsert()->getInsertType() == inserttype))
            data->setHidden(Hide);
      }
   }

   return RC_SUCCESS;
}

/******************************************************************************
* HideNetsAll
*/
short API::HideNetsAll(BOOL Hide) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   Hide_Nets_All(doc, Hide);

   return RC_SUCCESS;
}

/******************************************************************************
* HideNet
*/
short API::HideNet(LPCTSTR netname, BOOL Hide) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();
   
   int netValueIndex = doc->RegisterValue(netname);
   Hide_Net(doc, netValueIndex, Hide);

   return RC_SUCCESS;
}

void SetPageSize(CCEtoODBDoc *doc, double xmin, double xmax, double ymin, double ymax);
short API::SetPageSize(double xmin, double xmax, double ymin, double ymax) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   ::SetPageSize(doc, xmin, xmax, ymin, ymax);

   return RC_SUCCESS;
}


/******************************************************************************
* API::UserCoord()
*/
short API::UserCoord() 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBView *view = (CCEtoODBView*)apiView;

   //view->HideSearchCursor();
   view->cursorMode = GetCoord;
//   view->ShowSearchCursor();

   return RC_SUCCESS;
}

short API::UserCoord2(BOOL On) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBView *view = (CCEtoODBView*)apiView;

   //view->HideSearchCursor();
   if (On)
      view->cursorMode = GetCoord;
   else
      view->cursorMode = Search;
//   view->ShowSearchCursor();

   return RC_SUCCESS;
}

/******************************************************************************
* API::GenerateMarker()
*
* PARAMETER    : short shape // 0=circle, 1=rectangle, -1=default
*/
short API::GenerateMarker(short shape, double sizeA, double sizeB, double penWidth) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (shape == -1)
   {
      doc->currentMarker = NULL;
      return RC_SUCCESS;
   }

   CString name;
   if (!shape)
      name.Format("%s-Circ%.2lf", DRCMARKERGEOM, sizeA);
   else
      name.Format("%s-Rect%.2lf,%.2lf", DRCMARKERGEOM, sizeA, sizeB);

   doc->currentMarker = Graph_Block_Exists(doc, name, -1);

   if (doc->currentMarker)
      return RC_SUCCESS;

   doc->currentMarker = Graph_Block_On(GBO_APPEND, name, -1, 0);
   doc->currentMarker->setBlockType(BLOCKTYPE_DRCMARKER);
   doc->currentMarker->setFlagBits(BL_SPECIAL);
   int layer = Graph_Level("0", "", 1);
   int widthIndex = Graph_Aperture("", T_ROUND, penWidth, 0.0, 0.0, 0.0, 0.0, 0, 0L, FALSE, NULL);

   if (!shape)
   {
      Graph_Circle(layer, 0, 0, sizeA, 0L, widthIndex, FALSE, FALSE);
   }
   else
   {
      Graph_PolyStruct(layer, 0, FALSE);
      Graph_Poly(NULL, widthIndex, 0, 0, 0);
      Graph_Vertex(-sizeA, -sizeB, 0.0);
      Graph_Vertex(-sizeA, sizeB, 0.0);
      Graph_Vertex(sizeA, sizeB, 0.0);
      Graph_Vertex(sizeA, -sizeB, 0.0);
      Graph_Vertex(-sizeA, -sizeB, 0.0);
   }
   Graph_Block_Off();

   return RC_SUCCESS;
}

/******************************************************************************
* API::AddDrcMarker()
*/
short API::AddDrcMarker(long filePosition, float x, float y, LPCTSTR layer, LPCTSTR comment, long FAR* entityNumber) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);

   int algIndex = GetAlgorithmNameIndex(doc, "Externally Added DRCs");

	DataStruct *data;
   DRCStruct *drc = AddDRCAndMarker(doc, file, x, y, comment, DRC_CLASS_SIMPLE, 1, algIndex, DRC_ALG_GENERIC, layer, &data);

   *entityNumber = drc->getEntityNumber();

   if (API_AutoRedraw)
      doc->UpdateAllViews(NULL);

   return RC_SUCCESS;
}

/******************************************************************************
* LayersUsed
*/
short API::LayersUsed(VARIANT FAR* array) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (!doc->getMaxLayerIndex())
      return RC_EMPTY_ARRAY;

   VariantInit(array);
   array->vt = VT_ARRAY | VT_I4;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I4, 0, doc->getMaxLayerIndex()); 

   int Used = 0;
	long i=0;
   for (i=0; i<doc->getMaxLayerIndex(); i++)
      SafeArrayPutElement(psa, &i, &Used);

   // loop Blocklist, loop Datas, mark layers as used 
   // protect mirror layers.
   Used = 1;

   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      for (POSITION dataPos = block->getDataList().GetHeadPosition();dataPos != NULL;)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getAttributesRef())
         {
            for (POSITION attribPos = data->getAttributesRef()->GetStartPosition();attribPos != NULL;)
            {
               Attrib* attrib;
               WORD keyword;

               data->getAttributesRef()->GetNextAssoc(attribPos, keyword, attrib);

               if (attrib->isVisible())
               {
                  long layerNum = attrib->getLayerIndex();
                  SafeArrayPutElement(psa, &layerNum, &Used);

                  if (layerNum >= 0)
                  {
                     LayerStruct *layer = doc->getLayerArray()[attrib->getLayerIndex()];

                     if (layer->getMirroredLayerIndex() != layerNum) // protect mirror layers
                     {
                        LayerStruct *mirrorLayer = doc->getLayerArray()[layer->getMirroredLayerIndex()];
                        long mirrorLayerNum = mirrorLayer->getLayerIndex();
                        SafeArrayPutElement(psa, &mirrorLayerNum, &Used);
                     }
                  }
               }
            }
         }

         if (data->getLayerIndex() < 0) 
            continue;

         LayerStruct *layer = doc->getLayerArray()[data->getLayerIndex()];

         long layerNum = layer->getLayerIndex();
         SafeArrayPutElement(psa, &layerNum, &Used);

         if (layer->getMirroredLayerIndex() != layerNum) // protect mirror layers
         {
            LayerStruct *mirrorLayer = doc->getLayerArray()[layer->getMirroredLayerIndex()];
            long mirrorLayerNum = mirrorLayer->getLayerIndex();
            SafeArrayPutElement(psa, &mirrorLayerNum, &Used);
         }
      }
   }

   array->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* LayersUsedByNet
*/
short API::LayersUsedByNet(long filePosition, LPCTSTR netName, VARIANT FAR* array) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (!doc->getMaxLayerIndex())
      return RC_EMPTY_ARRAY;

   VariantInit(array);
   array->vt = VT_ARRAY | VT_I4;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I4, 0, doc->getMaxLayerIndex()); 

   int Used = 0;
	long i = 0; 
   for (i = 0; i<doc->getMaxLayerIndex(); i++)
      SafeArrayPutElement(psa, &i, &Used);

   // loop Blocklist, loop Datas, mark layers as used 
   // protect mirror layers.
   Used = 1;
   WORD keyword = doc->IsKeyWord(ATT_NETNAME, 0);
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block)
         continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getLayerIndex() < 0) 
            continue;

         if (data->getAttributesRef())
         {
            Attrib* attrib;

            if (data->getAttributesRef()->Lookup(keyword, attrib))
            {
               if (!attrib->getStringValue().Compare(netName))
               {
                  long layerNum = data->getLayerIndex();
                  SafeArrayPutElement(psa, &layerNum, &Used);
               }
            }
         }
      }
   }

   array->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* HideDrawings
*/
short API::HideDrawings(BOOL Hide) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   ::HideDrawings = Hide;

/* if (API_AutoRedraw)
      doc->UpdateAllViews(NULL);
*/
   return RC_SUCCESS;
}

/******************************************************************************
* GetGeometryExtents
*/
short API::GetGeometryExtents(short geometryNumber, double FAR* left, double FAR* right, double FAR* top, double FAR* bottom) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (geometryNumber < 0 || geometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *block = doc->getBlockAt(geometryNumber);

   if (!block)
      return RC_ITEM_NOT_FOUND;

   block_extents(doc, left, right, bottom, top, &(block->getDataList()), 0, 0,  0, 0, 1, -1, FALSE);

   return RC_SUCCESS;
}

/******************************************************************************
* MakeMaufacturingRefDes
*/
short API::MakeMaufacturingRefDes() 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   DoMakeManufacturingRefDes(doc, FALSE);

   return RC_SUCCESS;
}

/******************************************************************************
* License
*/
short API::License(short licenseNumber, long accessCode) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   LockLicenses = TRUE;
   short returnCode = 0;//getApp().getCamcadLicense().setLicenseByApi((CamcadProductIdTag)licenseNumber, accessCode);

   getApp().setTitleSuffix(" - Licensed through API");
   getMainFrame()->SetFrameTitle(getApp().getCamCadTitle());

   if (activeView)
   {
      CDocument *doc = activeView->GetDocument();
      getMainFrame()->UpdateFrameTitleForDocument(doc->GetTitle());
   }
   else
   {
      getMainFrame()->UpdateFrameTitleForDocument(NULL);
   }


   return returnCode;
}

/******************************************************************************
* API::OEMLicense
*/
short API::OEMLicense(LPCTSTR OEMName, short licenseNumber, long accessCode) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   bool tmpLockLicense = LockLicenses==TRUE?true:false;
   short returnCode = 0;//getApp().getCamcadLicense().setOEMLicenseByApi(OEMName, (CamcadProductIdTag)licenseNumber, accessCode, tmpLockLicense);
   LockLicenses = tmpLockLicense?TRUE:FALSE;

   getApp().setTitleSuffix(" - Licensed through API");
   getMainFrame()->SetFrameTitle(getApp().getCamCadTitle());

   if (activeView)
   {
      CDocument *doc = activeView->GetDocument();
      getMainFrame()->UpdateFrameTitleForDocument(doc->GetTitle());
   }
   else
   {
      getMainFrame()->UpdateFrameTitleForDocument(NULL);
   }


   return returnCode;
}

/******************************************************************************
* CheckLicense
*/
short API::CheckLicense(short licenseNumber, BOOL FAR* allowed) 
{
   *allowed = FALSE;

   //if (getApp().getCamcadLicense().isLicensed((CamcadProductIdTag)licenseNumber))
      *allowed = TRUE;

   return RC_SUCCESS;
}

/******************************************************************************
* CheckLicense
*/
short API::LoadSTDLicenses() 
{
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
   pApp->SetCAMCADTitle(); 
   pApp->getCamcadLicense().initializeLicenses();
   pApp->getCamcadLicense().checkOutLicenses();
   
   return RC_SUCCESS;
}

/******************************************************************************
* CheckImportFormatLicense
*/
short API::CheckImportFormatLicense(short format, BOOL FAR* allowed) 
{
   short retval = RC_ITEM_NOT_FOUND;   
   *allowed = FALSE;

   /*if (getApp().getCamcadLicense().isFormatLicensedForImport(format))
   {
      retval = RC_SUCCESS;
      *allowed = TRUE;
   }*/

   return retval;
}

/******************************************************************************
* InitRealPartInfo
*/
short API::InitRealPartInfo() 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   // TODO 
   init_realpart_info(doc);

   return RC_SUCCESS;
}

/******************************************************************************
* Blank1
*/
short API::Blank1() 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   ErrorMessage("Test");

   return RC_SUCCESS;
}

/******************************************************************************
* CopyFile
*/
short API::CopyFile(long filePosition, LPCTSTR newFileName, long FAR* newFilePosition) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();
   CCamCadData& camCadData = doc->getCamCadData();

   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);
   BlockStruct *block = file->getBlock();

   // graph new block
   BlockStruct *newBlock;
   newBlock = Graph_Block_On(GBO_OVERWRITE, newFileName, NextFileNum, 0);
   newBlock->setToolType(block->getToolType());
   newBlock->setFileNumber(NextFileNum);
   newBlock->setFlags(block->getFlags());
   newBlock->setBlockType(block->getBlockType());
   Graph_Block_Copy(block, 0, 0, 0, 0, 1, 0, TRUE);
   Graph_Block_Off();

   // create new file
   //FileStruct *newFile = new FileStruct;
   FileStruct* newFile = camCadData.getFileList().addNewFile(newFileName,file->getCadSourceFormat());
   *newFilePosition    = (long)(camCadData.getFileList().Find(newFile));

   //newFile->setFileNumber(NextFileNum++);
   //newFile->setName(newFileName);
   newFile->setBlock(newBlock);
   newFile->setOriginalBlock(newBlock);
   newFile->setInsertX(file->getInsertX());
   newFile->setInsertY(file->getInsertY());
   newFile->setScale(file->getScale());
   newFile->setRotation(file->getRotation());
   newFile->setMirrored(file->isMirrored());
   newFile->setShow(file->isShown());
   newFile->setHidden(file->isHidden());
   newFile->setNotPlacedYet(false);
   newFile->setBlockType(file->getBlockType());
   //newFile->setCadSourceFormat(file->getCadSourceFormat());
   //*newFilePosition = (long)doc->getFileList().AddTail(newFile);

   // copy netlist, etc   
   for (POSITION netPos = file->getNetList().GetHeadPosition();netPos != NULL;)
   {
      NetStruct* net = file->getNetList().GetNext(netPos);

      NetStruct* newNet = newFile->getNetList().addNet(net->getNetName());

      //NetStruct* newNet = new NetStruct;
      //newFile->getNetList().AddTail(newNet);
      //newNet->setNetName( net->getNetName());
      newNet->setFlags( net->getFlags());
      //newNet->setEntityNumber(CEntityNumber::allocate());
      //newNet->getAttributesRef() = NULL;
      doc->CopyAttribs(&newNet->getAttributesRef(), net->getAttributesRef());

      for (POSITION cpPos = net->getHeadCompPinPosition();cpPos != NULL;)
      {
         CompPinStruct* cp = net->getNextCompPin(cpPos);

         CompPinStruct* newCP = newNet->addCompPin(cp->getRefDes(),cp->getPinName(),cp->getOriginX(),cp->getOriginY(),
            cp->getRotationDegrees(),cp->getMirror(),cp->getPadstackBlockNumber(),cp->getVisible());
         if (newCP != NULL)
         {
            //CompPinStruct* newCP = new CompPinStruct;
            //newNet->getCompPinList().AddTail(newCP);
            //newCP->setRefDes(cp->getRefDes());
            //newCP->setPinName( cp->getPinName());
            //newCP->setOrigin(cp->getOrigin());
            //newCP->setRotationRadians( cp->getRotationRadians());
            //newCP->setMirror(cp->getMirror());            
            newCP->setPinCoordinatesComplete( cp->getPinCoordinatesComplete());
            //newCP->setVisible(cp->getVisible());
            //newCP->setPadstackBlockNumber( cp->getPadstackBlockNumber());
            //newCP->setEntityNumber(CEntityNumber::allocate());
            //newCP->getAttributesRef() = NULL;
            doc->CopyAttribs(&newCP->getAttributesRef(), cp->getAttributesRef());
         }
      }
   }

   POSITION typePos = file->getTypeList().GetHeadPosition();
   while (typePos)
   {
      TypeStruct* type = file->getTypeList().GetNext(typePos);
      TypeStruct* newType = newFile->getTypeList().addType(type->getName());

      //TypeStruct *newType = new TypeStruct;
      //newFile->getTypeList().AddTail(newType);
      //newType->setName( type->getName());
      newType->setBlockNumber( type->getBlockNumber());
      //newType->setEntityNumber(CEntityNumber::allocate());
      //newType->getAttributesRef() = NULL;

      doc->CopyAttribs(&newType->getAttributesRef(), type->getAttributesRef());

      for (POSITION tpPos = type->getTypePinList().GetHeadPosition();tpPos != NULL;)
      {
         TypePinStruct* tp = type->getTypePinList().GetNext(tpPos);

         TypePinStruct* newTP = newType->getTypePinList().addTypePin();

         *newTP = *tp;

         //TypePinStruct *newTP = new TypePinStruct;
         //newType->getTypePinList().AddTail(newTP);
         //newTP->setPhysicalPinName( tp->getPhysicalPinName());
         //newTP->setLogicalPinName( tp->getLogicalPinName());
         //newTP->setGateNumber( tp->getGateNumber());
         //newTP->setSwap( tp->getSwap());
         //newTP->setPinType( tp->getPinType());
         //newTP->setFlags( tp->getFlags());
         //newTP->setEntityNumber(CEntityNumber::allocate());
         //newTP->getAttributesRef() = NULL;

         //doc->CopyAttribs(&newTP->getAttributesRef(), tp->getAttributesRef());
      }
   }

   POSITION drcPos = file->getDRCList().GetHeadPosition();
   while (drcPos)
   {
      DRCStruct* drc = file->getDRCList().GetNext(drcPos);

      DRCStruct* newDRC = newFile->getDRCList().addDrc("");

      //newFile->getDRCList().AddTail(newDRC);
      newDRC->setString(drc->getString());
      newDRC->setOrigin(drc->getOrigin());
      newDRC->setDrcClass(drc->getDrcClass());
      newDRC->setPriority(drc->getPriority());
      newDRC->setReviewed(drc->getReviewed());
      newDRC->setFailureRange(drc->getFailureRange());
      newDRC->setComment(drc->getComment());
      newDRC->setAlgorithmIndex(drc->getAlgorithmIndex());
      newDRC->setAlgorithmType(drc->getAlgorithmType());
      newDRC->setInsertEntityNumber(drc->getInsertEntityNumber());
      doc->CopyAttribs(&newDRC->getAttributesRef(), drc->getAttributesRef());

      switch (drc->getDrcClass())
      {
      case DRC_CLASS_SIMPLE:
      case DRC_CLASS_NO_MARKER:
         break;
      case DRC_CLASS_MEASURE:
         if (drc->getVoidPtr())
         {
            newDRC->setVoidPtr(new DRC_MeasureStruct);
            memcpy(newDRC->getVoidPtr(), drc->getVoidPtr(), sizeof(DRC_MeasureStruct));
         }
         break;
      case DRC_CLASS_NETS:
         if (drc->getVoidPtr())
         {
            CStringList *list = (CStringList*)drc->getVoidPtr();
            CStringList *newList = new CStringList;
            newDRC->setVoidPtr(newList);

            POSITION pos = list->GetHeadPosition();
            while (pos)
               newList->AddTail(list->GetNext(pos));
         }
         break;
      };
   }

   return RC_SUCCESS;
}

/******************************************************************************
* RestructureFileAroundOrigin 
*/
short API::RestructureFileAroundOrigin(long filePosition) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   // find FILE
   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);

   RestructureFile(doc, file);

   return RC_SUCCESS;
}


/******************************************************************************
* API::AutoBoardOrigin
*/
short API::AutoBoardOrigin(long filePosition, BOOL Left, BOOL Bottom) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   // find FILE
   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);

   ::AutoBoardOrigin(doc, file, Left, Bottom);

   return RC_SUCCESS;
}

/******************************************************************************
* dfmCreateDFxOutline  
*   algorithm 0 = classic
*             1 = inside pins
*             2 = cover pins
*/
short API::dfmCreateDFxOutline(short geometryNumber, short algorithm, BOOL overwrite) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (geometryNumber < 0 || geometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *block = doc->getBlockAt(geometryNumber);

   if (!block)
      return RC_HOLE_IN_ARRAY;

   CWaitCursor wait;

	int err = DFT_CreateGeomDFxOutline(doc, block, algorithm, overwrite);
   if (err != 0 || err != 2)
      return RC_GENERAL_ERROR;

   return RC_SUCCESS;
}


/******************************************************************************
* API::ClearAllDRCs
*/
short API::ClearAllDRCs() 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   CloseDRCList();

   ClearDRCList(doc);

   return RC_SUCCESS;
}

/******************************************************************************
* EditGeometry
*/
short API::EditGeometry(short geometryNumber) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (geometryNumber < 0 || geometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *block = doc->getBlockAt(geometryNumber);

   if (!block)
      return RC_GENERAL_ERROR;

   ::EditGeometry(doc, block);

   return RC_SUCCESS;
}

/******************************************************************************
* OpenCadFile
*/
short API::OpenCadFile(LPCTSTR filename) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
   
   bool lastUseDialogs = pApp->getUseDialogsFlag();
   pApp->setUseDialogsFlag(false);

   short retVal = pApp->OpenCadFile(filename);

   pApp->setUseDialogsFlag(lastUseDialogs);

   return retVal;
}

/******************************************************************************
* OpenNewDoc
*/
short API::OpenNewDocument()
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif
   
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
   pApp->OnFileNew();

   return RC_SUCCESS;
}

/******************************************************************************
* SetWindowName
*/
short API::SetWindowName(LPCTSTR name)
{
   getMainFrame()->SetFrameTitle(name);
   getMainFrame()->SetWindowText(name);

   return RC_SUCCESS;
}

/******************************************************************************
* SetWindowIcon
*/
short API::SetWindowIcon(LPCTSTR iconFilename, short icon)
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   HICON hIcon = NULL;
   if (strlen(iconFilename))
      hIcon = ExtractIcon(NULL, iconFilename, 0);

   if (!hIcon)
   {
      CWinApp *pApp = AfxGetApp();
      hIcon = pApp->LoadIcon(IDR_MAINFRAME);
   }

   getMainFrame()->SetIcon(hIcon, TRUE);
   getMainFrame()->SetIcon(hIcon, FALSE);

   return RC_SUCCESS;
}

/******************************************************************************
* SendLayerToBack
*/
short API::SendLayerToBack(short layerNumber)
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   ::SendLayerToBack(doc, layerNumber);

   return RC_SUCCESS;
}

/******************************************************************************
* BringLayerToFront
*/
short API::BringLayerToFront(short layerNumber)
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   ::BringLayerToFront(doc, layerNumber);

   return RC_SUCCESS;
}

/******************************************************************************
* SortPanelBoards
*		startPoint:
*			0 = Bottom Left (default)
*			1 = Top Left
*			2 = Bottom Right
*			3 = Top Right
*
*		sortStyle:
*			0 = Serpentine Like
*			1 = Progressive Raster
*/
short API::SortPanelBoards(long filePosition, short startPoint, short sortStyle, short tolerancePercentage)
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   // find FILE
   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);

	if (file->getBlockType() != blockTypePanel)
		return RC_WRONG_TYPE;

	CSortPanel pnlSort(*doc, file, tolerancePercentage);
	pnlSort.SetSortAlg((EPanelSortAlg)sortStyle);
	pnlSort.SetStartSortLocation((EPanelSortStartLocation)startPoint);
	pnlSort.Sort();
	pnlSort.ApplyToPanelFile();

   return RC_SUCCESS;
}


short API::GetDataExtents(short geometryNumber, long dataPosition, double FAR* xMin, double FAR* xMax, double FAR* yMin, double FAR* yMax)
{
	#ifdef SHAREWARE
	   return RC_NO_LICENSE;
	#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

	if (geometryNumber < 0 || geometryNumber >= doc->getMaxBlockIndex())
		return RC_INDEX_OUT_OF_RANGE;

	BlockStruct *block = doc->getBlockAt(geometryNumber);

	if (block==NULL)
		return RC_ITEM_NOT_FOUND;

	// find DATA
	POSITION pos = (POSITION)dataPosition;
	if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
		return RC_ILLEGAL_POSITION;
	DataStruct* data = block->getDataList().GetAt(pos);
	if (data==NULL)
		return RC_ILLEGAL_POSITION;

	CExtent extent;
	if (data->getDataType() == dataTypeInsert)
	{
		BlockStruct* subBlock = doc->getBlockAt(data->getInsert()->getBlockNumber());
      CTMatrix matrix = data->getInsert()->getTMatrix();
      if (subBlock != NULL)
      {
		   extent = subBlock->getExtent();
		   extent.transform(matrix);
      }
	}
	else if (data->getDataType() == dataTypePoly)
	{
		extent =  data->getPolyList()->getExtent();
	}
	else if (data->getDataType() == dataTypeText)
	{
		extent = data->getText()->getExtent(0);
	}

	if (extent.getXmin() > extent.getXmax())
		return RC_ITEM_NOT_FOUND;

	*xMin = extent.getXmin();
	*xMax = extent.getXmax();
	*yMin = extent.getYmin();
	*yMax = extent.getYmax();

	return RC_SUCCESS;
}

//---------------------------------------------------------------------------------------
// For licensing support.

////#include "crypto.h"
////extern bool CCZDataEncryptionEnabled;  // in CCZData.cpp

enum eValidationState { eUnvalidated, eKeyRetrieved, eExternalClient, eInternalClient };
// It would be nicer if these were owned by some class instead of global. Maybe later.
static int m_nKeySeed = 0;
static eValidationState m_eState = eUnvalidated;
eValidationState GetValidationState() { return m_eState; }
void SetValidationState(eValidationState state) { m_eState = state; }
//
//---------------------------------------------------------------------------------------

short API::Validate(long nLicenseToken, long FAR* pKey)
{
   // This is patterned after Borg Automation licensing, from code found
   // in Boarg Automation Document.cpp.
   // STDMETHODIMP XDocument::Validate(long nLicenseToken, long *pKey)

   // Currently we always return success, even when it fails. This is so someone
   // can't build a small loop and pound on the Validate until they get success.
   // But if we decide to return success or fail, this is the var it would be.
   short potentialRetcode = RC_SUCCESS;

   // Clear return key seed
   *pKey = 0;

   if(nLicenseToken == 0) 
   {
      // Seed the random number generator
      srand( (unsigned)time( NULL ) );
      // Cache the random number
      m_nKeySeed = rand();
      // Return the random number to be used as a seed.
      *pKey = m_nKeySeed;
      // Move the state to key retrieved if we are unititialized
      if( GetValidationState() == eUnvalidated) {
         SetValidationState(eKeyRetrieved);
      }
   } 
   else 
   {
      // Make sure the user got a key from us.  If not fail
      // they may be trying to get around licensing.
      if(GetValidationState() == eUnvalidated) 
      {
         //hr = epcbErrCodeValidationUninitialized;
         potentialRetcode = RC_NO_LICENSE;
         // See comment below by other call to this
         getApp().SetDecryptionAllowed(false);
      }
      // Test the license token against our transformed seed.  
      // First test the internal algorithm
      else if(nLicenseToken == GetMGCCCZDecryptionKey(m_nKeySeed)) 
      {
         SetValidationState(eInternalClient);

         // Probably better to just set the validation state, then have the
         // reader check the validation state and set/clear decryption accordingly.
         // But for ease of quick implementation for now, just directly set
         // the decryptor state now.
         getApp().SetDecryptionAllowed(true);
      }
      //--------------------------------
#ifdef FOR_FUTURE_USE
      // If that failed see if this is an external client
      else if(nLicenseToken == GetExternalToken(m_nKeySeed)) 
      {
         // Check license for external client
//#if !defined NDEBUG || defined RATQUANTIFY || defined DISABLE_LICENSING
         if(XApplication::IgnoreLicensing()) 
         {
            SetValidationState(eExternalClient);
            return S_OK;
         }
//#endif
         if(!gGuiLicense->IsBasicAutomationEnabled())
         {
            hr = epcbErrCodeInvalidAutomationLicense;
         }
         else
         {
            SetValidationState(eExternalClient);
         }
      }
#endif // future use
      //--------------------------------

      // Otherwise, incorrect token, no license granted
      else 
      {
         // hr = epcbErrCodeValidationFailed;
         potentialRetcode = RC_NO_LICENSE;
         SetValidationState(eUnvalidated);
         // See comment above by other call to this
         getApp().SetDecryptionAllowed(false);
      }
   }

   // Always return SUCCESS, even if validation actually failed.
   // We do not want to give away a simple entry point that might allow some
   // snooper to experiment until they get a successful return from this
   // validator. The only way one will be able to tell if it worked is to
   // call validate and then try an export, and that is on purpose.
   return RC_SUCCESS;
}

#ifdef FOR_FUTURE_USE
// This may turn out useful for 3rd party licensing, is also from
// Borg Automation Document.cpp.
///////////////////////////////////////////////////////////////////////
//                 DO NOT CHANGE                                     //
///////////////////////////////////////////////////////////////////////
// This function is duplicated in MGCPCBAutomationLicensing.dll in the
// AutLicensing project.  Do not change unless the change reflected
// there also.

int GetExternalToken(int key)
{
   long token;

   token = (long)( ( (key * 1.25) + 432 ) / 11 );

   // swap words
   long high = token << 16;
   long low = token >> 16;
   token = high | low;

   return token;
}
#endif

//-----------------------------------------------------------------------------------
