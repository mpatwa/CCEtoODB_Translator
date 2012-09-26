// $Header: /CAMCAD/4.3/DFTFacade.cpp 14    8/16/03 1:34a Kurt Van Ness $

#include "stdafx.h"

//XCP_TRY_HR eMode is optional
#pragma warning (disable: 4003)  //Not enough actual parameters for macro

#include "resource.h"       // main symbols
#include "DFTFacade.h"
#include "DFTAttributeCalculator.h"

#if !defined(DisableDftModule)

// CDFTFacade

IMPLEMENT_DYNAMIC(CDFTFacade, CWnd)

CDFTFacade::CDFTFacade(CCEtoODBDoc *pCCDoc)
{
   EnableAutomation();

   pDFT = NULL;

   XCP_TRY{
      #ifdef _DEBUG
         pdlgDFTTester = new CDFTTester(this);
      #else
         pdlgDFTTester = NULL;
      #endif

      this->pCCDoc = pCCDoc;  //Keep the document reference
   }XCP_VOID_CATCH
}

CDFTFacade::~CDFTFacade()
{
   if (NULL != pDFT)
      pDFT->SetDFTFacade( NULL);  //closes connection
   if (NULL != pdlgDFTTester)
      delete pdlgDFTTester;
}

void CDFTFacade::OnFinalRelease()
{
   // When the last reference for an automation object is released
   // OnFinalRelease is called.  The base class will automatically
   // deletes the object.  Add additional cleanup required for your
   // object before calling the base class.

   CCmdTarget::OnFinalRelease();
}

///  IDFTFacade Methods

BSTR CDFTFacade::GetCCFilePath(void)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   calculatePcbValidationAttributes(false);
   //Save the CAMCAD document to the CC file
   ::MessageBox( NULL, 
      "You are about to be prompted to save the CC file. \
       \nThis is required at this time to complete PCB Validation.", "Validate PCB / DFT /CAMCAD", 
      MB_OK | MB_ICONINFORMATION);
   pCCDoc->OnSaveDatafile();

   return pCCDoc->CCFileName.AllocSysString();  //client must free it
}


///  DFT Workflow Menu Item Handlers

void CDFTFacade::OnAdvancedValidatepcb()
{
   XCP_TRY{
      if (pdlgDFTTester) 
      {
         pdlgDFTTester->DoModal();
      }
      else
      {
         //Delegate request to DFT
         XCP_TRY_HR( pDFT->OnValidatePCB() );
      }

   }XCP_VOID_CATCH
}

void CDFTFacade::OnUpdateAdvancedValidatepcb(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(TRUE);
}

void CDFTFacade::OnAdvancedDftoptions()
{
   // TODO: Add your command handler code here
}
void CDFTFacade::OnUpdateAdvancedDftoptions(CCmdUI *pCmdUI)
{
   // TODO: Add your command update UI handler code here
}

BEGIN_MESSAGE_MAP(CDFTFacade, CWnd)
   ON_COMMAND(ID_ADVANCED_DFTOPTIONS, OnAdvancedDftoptions)
   ON_COMMAND(ID_ADVANCED_VALIDATEPCB, OnAdvancedValidatepcb)
   ON_UPDATE_COMMAND_UI(ID_ADVANCED_VALIDATEPCB, OnUpdateAdvancedValidatepcb)
   ON_UPDATE_COMMAND_UI(ID_ADVANCED_DFTOPTIONS, OnUpdateAdvancedDftoptions)
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CDFTFacade, CWnd)
   DISP_FUNCTION_ID(CDFTFacade, "GetCCFilePath",    dispidGetCCFilePath,    GetCCFilePath,   VT_BSTR,    VTS_NONE)
   DISP_FUNCTION_ID(CDFTFacade, "RemoveToolstacks", dispidRemoveToolstacks, RemoveToolstacks,VT_HRESULT, VTS_NONE)
   DISP_FUNCTION_ID(CDFTFacade, "InsertToolstacks", dispidInsertToolstacks, InsertToolstacks,VT_HRESULT, VTS_BSTR)
   DISP_FUNCTION_ID(CDFTFacade, "EditToolstack",    dispidEditToolstack,    EditToolstack,   VT_HRESULT, VTS_BSTR)
   DISP_FUNCTION_ID(CDFTFacade, "DeleteToolstack",  dispidDeleteToolstack,  DeleteToolstack, VT_HRESULT, VTS_BSTR)
   DISP_FUNCTION_ID(CDFTFacade, "AddToolstack",     dispidAddToolstack,     AddToolstack,    VT_HRESULT, VTS_BSTR)
   DISP_FUNCTION_ID(CDFTFacade, "CalcBoard2Pin",    dispidCalcBoard2Pin,    CalcBoard2Pin,   VT_HRESULT, VTS_I4)
   DISP_FUNCTION_ID(CDFTFacade, "CalcComp2Pin",     dispidCalcComp2Pin,     CalcComp2Pin,    VT_HRESULT, VTS_I4)
   DISP_FUNCTION_ID(CDFTFacade, "CalcSMaskClear",   dispidCalcSMaskClear,   CalcSMaskClear,  VT_HRESULT, VTS_I4)
   DISP_FUNCTION_ID(CDFTFacade, "CalcPadSize",      dispidCalcPadSize,      CalcPadSize,     VT_HRESULT, VTS_I4)
   DISP_FUNCTION_ID(CDFTFacade, "CalcMinMetal",     dispidCalcMinMetal,     CalcMinMetal,    VT_HRESULT, VTS_I4)
   DISP_FUNCTION_ID(CDFTFacade, "SetPinTBX",        dispidSetPinTBX,        SetPinTBX,       VT_HRESULT, VTS_I4)
   DISP_FUNCTION_ID(CDFTFacade, "CalcPadLocation",  dispidCalcPadLocation,  CalcPadLocation, VT_HRESULT, VTS_I4)
   DISP_FUNCTION_ID(CDFTFacade, "CalcCompCenterXY", dispidCalcCompCenterXY, CalcCompCenterXY,VT_HRESULT, VTS_I4)
   DISP_FUNCTION_ID(CDFTFacade, "SetCompTB",        dispidSetCompTB,        SetCompTB,       VT_HRESULT, VTS_I4)
   DISP_FUNCTION_ID(CDFTFacade, "trash", dispidtrash, trash, VT_HRESULT, VTS_BSTR)
   DISP_FUNCTION_ID(CDFTFacade, "InsertToolstacks2", dispidInsertToolstacks2, InsertToolstacks2, VT_HRESULT, VTS_PBSTR)
END_DISPATCH_MAP()

// Note: we add support for IID_IDFTFacade to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .IDL file.

// {D3B6EDD0-EE9D-4978-9C20-E12D2BB762EB}
static const IID IID_IDFTFacade =
{ 0xD3B6EDD0, 0xEE9D, 0x4978, { 0x9C, 0x20, 0xE1, 0x2D, 0x2B, 0xB7, 0x62, 0xEB } };

BEGIN_INTERFACE_MAP(CDFTFacade, CWnd)
   INTERFACE_PART(CDFTFacade, IID_IDFTFacade, Dispatch)
END_INTERFACE_MAP()

// {6AE201D4-8B7D-49c4-955D-450A564B26BE}
IMPLEMENT_OLECREATE(CDFTFacade, "CAMCAD.DFTFacade", 
0x6ae201d4, 0x8b7d, 0x49c4, 0x95, 0x5d, 0x45, 0xa, 0x56, 0x4b, 0x26, 0xbe);

HRESULT CDFTFacade::RemoveToolstacks(void)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // TODO: Add your dispatch handler code here

   return S_OK;
}

HRESULT CDFTFacade::InsertToolstacks(LPCTSTR ToolstacksXML)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   CComBSTR bstrToolstacks;
   CString ToolStacks = ToolstacksXML;
   //bstrToolstacks.AppendBSTR( bstrToolstacksXML);
   //ToolStacks = bstrToolstacks;
   pdlgDFTTester->Output.AddString(ToolStacks);

   return S_OK;
}

HRESULT CDFTFacade::EditToolstack(LPCTSTR ToolstackXML)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // TODO: Add your dispatch handler code here

   return S_OK;
}

HRESULT CDFTFacade::DeleteToolstack(LPCTSTR ToolstackXML)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // TODO: Add your dispatch handler code here

   return S_OK;
}

HRESULT CDFTFacade::AddToolstack(LPCTSTR ToolstackXML)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // TODO: Add your dispatch handler code here

   return S_OK;
}

HRESULT CDFTFacade::CalcBoard2Pin(long bOverwrite)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // TODO: Add your dispatch handler code here

   return S_OK;
}

HRESULT CDFTFacade::CalcComp2Pin(long bOverwrite)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // TODO: Add your dispatch handler code here

   return S_OK;
}

HRESULT CDFTFacade::CalcSMaskClear(long bOverwrite)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // TODO: Add your dispatch handler code here

   return S_OK;
}

HRESULT CDFTFacade::CalcPadSize(long bOverwrite)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // TODO: Add your dispatch handler code here

   return S_OK;
}

HRESULT CDFTFacade::CalcMinMetal(long bOverwrite)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // TODO: Add your dispatch handler code here

   return S_OK;
}

HRESULT CDFTFacade::SetPinTBX(long bOverwrite)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // TODO: Add your dispatch handler code here

   return S_OK;
}

HRESULT CDFTFacade::CalcPadLocation(long bOverwrite)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // TODO: Add your dispatch handler code here

   return S_OK;
}

HRESULT CDFTFacade::CalcCompCenterXY(long bOverwrite)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // TODO: Add your dispatch handler code here

   return S_OK;
}

HRESULT CDFTFacade::SetCompTB(long bOverwrite)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // TODO: Add your dispatch handler code here

   return S_OK;
}


HRESULT CDFTFacade::trash(LPCTSTR test)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // TODO: Add your dispatch handler code here

   return S_OK;
}

HRESULT CDFTFacade::InsertToolstacks2(BSTR* pbstrToolstacks)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   CComBSTR bstrToolstacks;
   CString ToolStacks;

   bstrToolstacks.AppendBSTR( *pbstrToolstacks);
   ToolStacks = bstrToolstacks;
   pdlgDFTTester->Output.AddString(ToolStacks);

   return S_OK;
}

#endif
