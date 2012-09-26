// $Header: /CAMCAD/4.6/SAX.cpp 12    12/07/06 12:08p Kurt Van Ness $

#include "stdafx.h"
#include "CCEtoODB.h"
#include "SAX.h"
#include "saxbasic.h"
#include "api.h"
#include "ccdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CView *activeView; // from CCVIEW.CPP

SAX *SAXdlg;

HRESULT __CreateObject(LPOLESTR pszProgID, IDispatch FAR* FAR* ppdisp);

void CCEtoODBApp::OnRunMacro() 
{
   CString filename;
   if (activeView)
   {
      CCEtoODBDoc *doc = (CCEtoODBDoc*)activeView->GetDocument();
      filename = doc->getSettings().getDirectoryMacro();
   }
   filename += "*.BAS";
   CFileDialog FileDialog(TRUE, "BAS", filename,
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Macro File (*.BAS)|*.BAS|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;

   CWaitCursor wait;

   filename = "\"";
   filename += FileDialog.GetPathName();
   filename += "\""; 
// filename = FileDialog.GetPathName();

   SAX dlg;
   dlg.Create(IDD_SAX_BASIC);
   dlg.m_sbpro.RunFile(filename);
}

void CCEtoODBApp::OnMacros() 
{
   SAXdlg = new SAX;
   if (SAXdlg->Create(IDD_SAX_BASIC))
      SAXdlg->ShowWindow(SW_SHOW);
   else
   {
      ErrorMessage("Error creating window.");
   }
}

/////////////////////////////////////////////////////////////////////////////
// SAX dialog
SAX::SAX(CWnd* pParent /*=NULL*/)
   : CDialog(SAX::IDD, pParent)
{
   //{{AFX_DATA_INIT(SAX)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void SAX::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(SAX)
   DDX_Control(pDX, IDC_SAXBASICM42PRO1, m_sbpro);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SAX, CDialog)
   //{{AFX_MSG_MAP(SAX)
   ON_WM_SIZE()
   ON_WM_INITMENUPOPUP()
   ON_COMMAND(ID_CMD_EXIT, OnCmdExit)
   //}}AFX_MSG_MAP
   ON_COMMAND_RANGE(ID_CMD_FILENEW, ID_CMD_HELPABOUT, OnCommand)
   ON_UPDATE_COMMAND_UI_RANGE(ID_CMD_FILENEW, ID_CMD_HELPABOUT, OnUpdateMenu)
END_MESSAGE_MAP()

struct MenuId2CmdNum
{
   UINT id;
   UINT cmd;
};

static const MenuId2CmdNum g_MenuId2CmdNum[] =
{
   {ID_CMD_FILENEW, sbcFileNew},
   {ID_CMD_FILENEWCODEMODULE, sbcFileNewCodeModule},
   {ID_CMD_FILENEWOBJECTMODULE, sbcFileNewObjectModule},
   {ID_CMD_FILENEWCLASSMODULE, sbcFileNewClassModule},
   {ID_CMD_FILEOPEN, sbcFileOpen},
   {ID_CMD_FILECLOSE, sbcFileClose},
   {ID_CMD_FILESAVE, sbcFileSave},
   {ID_CMD_FILESAVEAS, sbcFileSaveAs},
   {ID_CMD_FILESAVEALL, sbcFileSaveAll},
   {ID_CMD_FILEPRINT, sbcFilePrint},
   {ID_CMD_FILEPRINTSETUP, sbcFilePrintSetup},
   {ID_CMD_EDITUNDO, sbcEditUndo},
   {ID_CMD_EDITREDO, sbcEditRedo},
   {ID_CMD_EDITCUT, sbcEditCut},
   {ID_CMD_EDITCOPY, sbcEditCopy},
   {ID_CMD_EDITPASTE, sbcEditPaste},
   {ID_CMD_EDITDELETE, sbcEditDelete},
   {ID_CMD_EDITSELECTALL, sbcEditSelectAll},
   {ID_CMD_EDITINDENT, sbcEditIndent},
   {ID_CMD_EDITOUTDENT, sbcEditOutdent},
   {ID_CMD_EDITTABASSPACES, sbcEditTabAsSpaces},
   {ID_CMD_EDITFIND, sbcEditFind},
   {ID_CMD_EDITREPLACE, sbcEditReplace},
   {ID_CMD_EDITAGAIN, sbcEditAgain},                
   {ID_CMD_EDITUSERDIALOG, sbcEditUserDialog},
   {ID_CMD_EDITREFERENCES, sbcEditReferences},
   {ID_CMD_EDITPROPERTIES, sbcEditProperties},
   {ID_CMD_VIEWMACRO, sbcViewMacro},
   {ID_CMD_VIEWIMMEDIATE, sbcViewImmediate},
   {ID_CMD_VIEWWATCH, sbcViewWatch},
   {ID_CMD_VIEWSTACK, sbcViewStack},
   {ID_CMD_VIEWLOADED, sbcViewLoaded},
   {ID_CMD_VIEWTOOLBAR, sbcViewToolbar},
   {ID_CMD_VIEWSTATUSBAR, sbcViewStatusBar},
   {ID_CMD_VIEWEDITBUTTONS, sbcViewEditButtons},
   {ID_CMD_VIEWALWAYSSPLIT, sbcViewAlwaysSplit},
   {ID_CMD_VIEWFONT, sbcViewFont},
   {ID_CMD_VIEWTABWIDTH1, sbcViewTabWidth1},
   {ID_CMD_VIEWTABWIDTH2, sbcViewTabWidth2},
   {ID_CMD_VIEWTABWIDTH3, sbcViewTabWidth3},
   {ID_CMD_VIEWTABWIDTH4, sbcViewTabWidth4},
   {ID_CMD_VIEWTABWIDTH5, sbcViewTabWidth5},
   {ID_CMD_VIEWTABWIDTH6, sbcViewTabWidth6},
   {ID_CMD_VIEWTABWIDTH7, sbcViewTabWidth7},
   {ID_CMD_VIEWTABWIDTH8, sbcViewTabWidth8},
   {ID_CMD_MACRORUN, sbcMacroRun},
   {ID_CMD_MACROPAUSE, sbcMacroPause},
   {ID_CMD_MACROEND, sbcMacroEnd},
   {ID_CMD_DEBUGSTEPINTO, sbcDebugStepInto},
   {ID_CMD_DEBUGSTEPOVER, sbcDebugStepOver},
   {ID_CMD_DEBUGSTEPOUT, sbcDebugStepOut},
   {ID_CMD_DEBUGSTEPTO, sbcDebugStepTo},
   {ID_CMD_DEBUGBREAK, sbcDebugToggleBreak},
   {ID_CMD_DEBUGCLEARALLBREAKS, sbcDebugClearAllBreaks},
   {ID_CMD_DEBUGQUICKWATCH, sbcDebugQuickWatch},
   {ID_CMD_DEBUGADDWATCH, sbcDebugAddWatch},
   {ID_CMD_DEBUGBROWSE, sbcDebugBrowse},
   {ID_CMD_DEBUGSETNEXT, sbcDebugSetNext},
   {ID_CMD_DEBUGSHOWNEXT, sbcDebugShowNext},
   {ID_CMD_SHEETOPENUSES, sbcSheetOpenUses},
   {ID_CMD_SHEETCLOSEALL, sbcSheetCloseAll},
   {ID_CMD_HELPAPP, sbcHelpApp},
   {ID_CMD_HELPLANGUAGE, sbcHelpLanguage},
   {ID_CMD_HELPTOPIC, sbcHelpTopic},
   {ID_CMD_HELPABOUT, sbcHelpAbout},
   {0, 0}, //sbcCount},
   // {ID_CMD_<menuitem>, sbc<MenuItem>},
};

static BOOL GetCmdNum(UINT id, UINT* pCmdNum)
{
   const MenuId2CmdNum* pmi2cn;
   for (pmi2cn = g_MenuId2CmdNum; pmi2cn->id; ++pmi2cn)
   {
      if (pmi2cn->id == id)
      {
         *pCmdNum = pmi2cn->cmd;
         return TRUE;
      }
   }

   return FALSE;
}

void SAX::OnUpdateMenu(CCmdUI* pCmdUI)
{
   CMenu *pMainMenu = GetMenu();
   if (pMainMenu)
   {
      const MenuId2CmdNum* pmi2cn;
      for (pmi2cn = g_MenuId2CmdNum; pmi2cn->id; ++pmi2cn)
      {
         // Update for menu items that are either checked or unchecked
         if (m_sbpro.IsMenuCommandChecked(pmi2cn->cmd))
            pMainMenu->CheckMenuItem(pmi2cn->id, MF_CHECKED);
         else
            pMainMenu->CheckMenuItem(pmi2cn->id, MF_UNCHECKED);

         // Update for menu items that are either enabled or disabled
         if (m_sbpro.IsMenuCommandEnabled(pmi2cn->cmd))
            pMainMenu->EnableMenuItem(pmi2cn->id, MF_ENABLED);
         else
            pMainMenu->EnableMenuItem(pmi2cn->id, MF_GRAYED);
      }
   }

   DrawMenuBar();
}

void SAX::OnCommand(UINT id)
{
   UINT cmd;
   if (GetCmdNum(id, &cmd))
      m_sbpro.ExecuteMenuCommand(cmd);
}

/////////////////////////////////////////////////////////////////////////////
// SAX message handlers
BOOL SAX::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   ITypeLib **pptLib = new (ITypeLib *);
   CString tlb, path;
   tlb = path = getApp().getCamcadExeFolderPath();
   tlb += "camcad.tlb";

   if (LoadTypeLib(tlb.AllocSysString(), pptLib) != S_OK || RegisterTypeLib(*pptLib, tlb.AllocSysString(), path.AllocSysString()) != S_OK)
      MessageBox("Type libraries not avalible for browsing.");

   delete (pptLib);

//#Reference {F8DE4B05-FBC2-11D1-BA40-0080ADB36DBB}#1.0#0#C:\camcad\WinDebug\Camcad.tlb#CAMCAD
   dispatch = NULL;
// HRESULT hr = __CreateObject(OLESTR("CAMCAD.Application"), &dispatch);
// if (FAILED(hr))
// {
      m_sbpro.SetCode("Sub Main\r\n\tDim objCAMCAD\r\n\tSet objCAMCAD = CreateObject(\"CAMCAD.Application\")\r\n\r\n\t'example\r\n\t'objCAMCAD.ZoomLevel 2, 1\r\n\r\nEnd Sub");
      m_sbpro.AddExtension("{F8DE4B05-FBC2-11D1-BA40-0080ADB36DBB}#1.0#0#", NULL);
// }
// else
//    m_sbpro.AddExtension("", dispatch);

   m_sbpro.SetDefaultDataType("CAMCAD.Application");
   m_sbpro.SetHighlightExtension(0x804000);
   
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void SAX::OnSize(UINT nType, int cx, int cy) 
{
   CDialog::OnSize(nType, cx, cy);

   CWnd *ctrl = GetDlgItem(IDC_SAXBASICM42PRO1);
   if (ctrl)
   {
      CRect rect(1, 1, cx-1, cy-1);
      ctrl->MoveWindow(rect);
   }
}

void SAX::OnCancel() 
{
   int Result = MessageBox("Would you like to save any last changes?", "Question", MB_YESNOCANCEL);

   if (IDYES == Result)
      m_sbpro.ExecuteMenuCommand(sbcFileSave);
   else if (IDCANCEL == Result)
      return;

   switch (m_sbpro.Shutdown())
   {
   case -1:
      break;
   case 0:
      m_sbpro.Disconnect();
      if (dispatch)
         dispatch->Release();
      DestroyWindow();
      break;
   case 1:
      break;
   }
}

void SAX::PostNcDestroy() 
{
   delete this;
   CDialog::PostNcDestroy();
}

void SAX::OnEditReferences() 
{
   m_sbpro.ExecuteMenuCommand(CSaxBasic::cmdEditReferences);
}

void SAX::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
   OnUpdateMenu();
}

void SAX::OnCmdExit() 
{
   OnCancel(); 
}

