// $Header: /CAMCAD/4.5/ToolbarM.cpp 37    1/18/07 10:34p Rick Faltersack $

#include "stdafx.h"
#include "CCEtoODB.h"
#include "ToolbarM.h"
#include "mainfrm.h"
#include <afxadv.h>
#include "wrldview.h"
#include ".\toolbarm.h"
#include "GerberEducatorToolbar.h"
//#include "MenuState.h"
#include <WinUser.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TB_VERSION   1

extern CView *activeView; // from CCVIEW.CPP
extern CString toolbarFile; // from MAINFRM.CPP
extern BOOL WorldViewOn;
extern WorldView *worldView;

/******************************************************************************
* OnLoadToolbarFile
*/
void CMainFrame::OnLoadToolbarFile() 
{
   CFileDialog FileDialog(TRUE, "TB", "*.TB",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "CAMCAD Toolbar Files (*.TB)|*.TB|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   LoadToolbars(FileDialog.GetPathName());
}

/******************************************************************************
* OnSaveToolbarFile
*/
void CMainFrame::OnSaveToolbarFile() 
{
   FILE *fp = fopen(toolbarFile,"w"); // check that file has write access.
   if (fp)
   {
      fclose(fp);
      SaveToolbars(toolbarFile);
      SaveWorldViewState();
   }
   else
   {
      ErrorMessage(toolbarFile, "Unable To Write File", MB_ICONEXCLAMATION | MB_OK);
   }
}

/******************************************************************************
* LoadToolbars
*/
void CMainFrame::LoadToolbars(const char *filename)
{
   m_CustomToolBar.RestoreState();
   m_Custom2ToolBar.RestoreState();

   //LoadBarState("ToolBars");
   CDockState state;

   CFile file;
   if (!file.Open(filename, CFile::modeRead))
      return;
   CArchive archive(&file, CArchive::load);

   WORD version;
   archive >> version;

   if (version == TB_VERSION)
   {
      state.Serialize(archive);
      SetDockState(state);
   }

   archive.Close();
   file.Close();
}
                                     
/******************************************************************************
* SaveToolbars
*/
void CMainFrame::SaveToolbars(const char *filename)
{
   //SaveBarState("ToolBars");
   CDockState state;
   GetDockState(state);

   CFile file;
   file.Open(filename, CFile::modeCreate | CFile::modeWrite);
   CArchive archive(&file, CArchive::store);

   WORD version = TB_VERSION;
   archive << version;

   state.Serialize(archive);

   archive.Close();
   file.Close();

   m_CustomToolBar.SaveState();
   m_Custom2ToolBar.SaveState();
}

CToolBarVisibilityState CMainFrame::GetToolBarVisiblity()
{
   CToolBarVisibilityState tbVisibility;

   tbVisibility.m_hMenu = GetMenu()->GetSafeHmenu();
   tbVisibility.m_bCurrentSettings = m_wndCurrentSettings.IsWindowVisible()?true:false;
   tbVisibility.m_bNavigator = m_navigator.IsWindowVisible()?true:false;
   tbVisibility.m_bGerberEducatorToolBar = m_GerberEducatorToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bToolBar = m_wndToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bAddToolBar = m_AddToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bSnapToolBar = m_SnapToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bEditToolBar = m_EditToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bQueryToolBar = m_QueryToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bGeomToolBar = m_GeomToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bPolyToolBar = m_PolyToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bGC_ToolBar = m_GC_ToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bIT_ToolBar = m_IT_ToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bPcbToolBar = m_PcbToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bOGP_ToolBar = m_OGP_ToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bRedlineToolBar = m_RedlineToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bJoinToolBar = m_JoinToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bSequenceToolBar = m_SequenceToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bViewToolBar = m_ViewToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bFilterToolBar = m_FilterToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bTestAccessToolBar = m_TestAccessToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bSchematicToolBar = m_SchematicToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bDFTToolBar = m_DFTToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bToolsToolBar = m_ToolsToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bCustomToolBar = m_CustomToolBar.IsWindowVisible()?true:false;
   tbVisibility.m_bCustom2ToolBar = m_Custom2ToolBar.IsWindowVisible()?true:false;

   if (worldView != NULL)
      tbVisibility.m_bWorldView = worldView->IsWindowVisible()?true:false;

   return tbVisibility;
}

void CMainFrame::SetToolBarVisiblity(CToolBarVisibilityState toolBarState)
{
   if (toolBarState.m_hMenu)
   {
      CMenu menu;
      menu.Attach(toolBarState.m_hMenu);
      SetMenu(&menu);
      menu.Detach();
   }
   else
      SetMenu(NULL);


   // TOM - Case 1187 - Removed event-driven toolbar hiding/showing
   m_wndToolBar.ShowWindow             (toolBarState.m_bToolBar               ? SW_SHOW : SW_HIDE);
   m_AddToolBar.ShowWindow             (toolBarState.m_bAddToolBar            ? SW_SHOW : SW_HIDE);
   m_DFTToolBar.ShowWindow             (toolBarState.m_bDFTToolBar            ? SW_SHOW : SW_HIDE);
   m_wndCurrentSettings.ShowWindow     (toolBarState.m_bCurrentSettings       ? SW_SHOW : SW_HIDE);
   m_navigator.ShowWindow              (toolBarState.m_bNavigator             ? SW_SHOW : SW_HIDE);
   m_GerberEducatorToolBar.ShowWindow  (toolBarState.m_bGerberEducatorToolBar ? SW_SHOW : SW_HIDE);
   m_CustomToolBar.ShowWindow          (toolBarState.m_bCustomToolBar         ? SW_SHOW : SW_HIDE);
   m_Custom2ToolBar.ShowWindow         (toolBarState.m_bCustom2ToolBar        ? SW_SHOW : SW_HIDE);
   m_EditToolBar.ShowWindow            (toolBarState.m_bEditToolBar           ? SW_SHOW : SW_HIDE);
   m_GeomToolBar.ShowWindow            (toolBarState.m_bGeomToolBar           ? SW_SHOW : SW_HIDE);
   m_GC_ToolBar.ShowWindow             (toolBarState.m_bGC_ToolBar            ? SW_SHOW : SW_HIDE);
   m_IT_ToolBar.ShowWindow             (toolBarState.m_bIT_ToolBar            ? SW_SHOW : SW_HIDE);
   m_JoinToolBar.ShowWindow            (toolBarState.m_bJoinToolBar           ? SW_SHOW : SW_HIDE);
   m_OGP_ToolBar.ShowWindow            (toolBarState.m_bOGP_ToolBar           ? SW_SHOW : SW_HIDE);
   m_PolyToolBar.ShowWindow            (toolBarState.m_bPolyToolBar           ? SW_SHOW : SW_HIDE);
   m_QueryToolBar.ShowWindow           (toolBarState.m_bQueryToolBar          ? SW_SHOW : SW_HIDE);
   m_SchematicToolBar.ShowWindow       (toolBarState.m_bSchematicToolBar      ? SW_SHOW : SW_HIDE);
   m_SequenceToolBar.ShowWindow        (toolBarState.m_bSequenceToolBar       ? SW_SHOW : SW_HIDE);
   m_SnapToolBar.ShowWindow            (toolBarState.m_bSnapToolBar           ? SW_SHOW : SW_HIDE);
   m_ViewToolBar.ShowWindow            (toolBarState.m_bViewToolBar           ? SW_SHOW : SW_HIDE);
   m_PcbToolBar.ShowWindow             (toolBarState.m_bPcbToolBar            ? SW_SHOW : SW_HIDE);
   m_FilterToolBar.ShowWindow          (toolBarState.m_bFilterToolBar         ? SW_SHOW : SW_HIDE);
   m_RedlineToolBar.ShowWindow         (toolBarState.m_bRedlineToolBar        ? SW_SHOW : SW_HIDE);

   if (worldView != NULL)
      worldView->ShowWindow(toolBarState.m_bWorldView?SW_SHOW:SW_HIDE);

   RecalcLayout();
   UpdateWindow();
}

void CMainFrame::enableToolBars(bool enableFlag)
{
   m_wndToolBar.enable(enableFlag);
   m_AddToolBar.enable(enableFlag);
   m_DFTToolBar.enable(enableFlag);
   //m_wndCurrentSettings.enable(enableFlag);
   //navigator.enable(enableFlag);
   m_GerberEducatorToolBar.enable(enableFlag);
   m_CustomToolBar.enable(enableFlag);
   m_Custom2ToolBar.enable(enableFlag);
   m_EditToolBar.enable(enableFlag);
   m_GeomToolBar.enable(enableFlag);
   m_GC_ToolBar.enable(enableFlag);
   m_IT_ToolBar.enable(enableFlag);
   m_JoinToolBar.enable(enableFlag);
   m_OGP_ToolBar.enable(enableFlag);
   m_PolyToolBar.enable(enableFlag);
   m_QueryToolBar.enable(enableFlag);
   m_SchematicToolBar.enable(enableFlag);
   m_SequenceToolBar.enable(enableFlag);
   m_SnapToolBar.enable(enableFlag);
   m_ViewToolBar.enable(enableFlag);
   m_PcbToolBar.enable(enableFlag);
   m_FilterToolBar.enable(enableFlag);
   m_RedlineToolBar.enable(enableFlag);
   m_ToolsToolBar.enable(enableFlag);
}

void CMainFrame::disableToolBars()
{
   enableToolBars(false);
}

void CMainFrame::displayToolBars(bool displayFlag)
{
   if (displayFlag)
   {
      m_wndToolBar.unhide();
      m_AddToolBar.unhide();
      m_DFTToolBar.unhide();
      //m_wndCurrentSettings.unhide();
      //navigator.unhide();
      m_GerberEducatorToolBar.unhide();
      m_CustomToolBar.unhide();
      m_Custom2ToolBar.unhide();
      m_EditToolBar.unhide();
      m_GeomToolBar.unhide();
      m_GC_ToolBar.unhide();
      m_IT_ToolBar.unhide();
      m_JoinToolBar.unhide();
      m_OGP_ToolBar.unhide();
      m_PolyToolBar.unhide();
      m_QueryToolBar.unhide();
      m_SchematicToolBar.unhide();
      m_SequenceToolBar.unhide();
      m_SnapToolBar.unhide();
      m_ViewToolBar.unhide();
      m_PcbToolBar.unhide();
      m_FilterToolBar.unhide();
      m_RedlineToolBar.unhide();
      m_ToolsToolBar.unhide();

      if (m_dockState != NULL)
      {
         SetDockState(*m_dockState);

         delete m_dockState;
         m_dockState = NULL;
      }
   }
   else
   {

      if (m_dockState == NULL)
      {
         m_dockState = new CDockState();

         GetDockState(*m_dockState);
      }

      m_wndToolBar.hide();
      m_AddToolBar.hide();
      m_DFTToolBar.hide();
      //m_wndCurrentSettings.hide();
      //navigator.hide();
      m_GerberEducatorToolBar.hide();
      m_CustomToolBar.hide();
      m_Custom2ToolBar.hide();
      m_EditToolBar.hide();
      m_GeomToolBar.hide();
      m_GC_ToolBar.hide();
      m_IT_ToolBar.hide();
      m_JoinToolBar.hide();
      m_OGP_ToolBar.hide();
      m_PolyToolBar.hide();
      m_QueryToolBar.hide();
      m_SchematicToolBar.hide();
      m_SequenceToolBar.hide();
      m_SnapToolBar.hide();
      m_ViewToolBar.hide();
      m_PcbToolBar.hide();
      m_FilterToolBar.hide();
      m_RedlineToolBar.hide();
      m_ToolsToolBar.hide();
   }

   RecalcLayout();
}

void CMainFrame::enableMenuItems(bool enableFlag)
{
   m_menuItemsEnabled = enableFlag;

   updateMenuState();

   //return;

   //static CMenuState* menuState = NULL;

   //bool useMenuState = false;

   //if (useMenuState)
   //{
   //   if (menuState != NULL)
   //   {
   //      if (enableFlag)
   //      {
   //         CMenu* currentMenu = GetMenu();
   //         CMenu* savedMenu = menuState->createMenu();

   //         CMenuState restoredMenuState(savedMenu);
   //         CDebugWriteFormat::setFilePath("C:\\MenuStateAfterCreate-Debug.txt");
   //         restoredMenuState.dump(CDebugWriteFormat::getWriteFormat());
   //         CDebugWriteFormat::close();

   //         SetMenu(savedMenu);
   //         savedMenu->Detach();
   //         delete savedMenu;

   //         delete menuState;
   //         menuState = NULL;

   //         if (currentMenu != NULL)
   //         {
   //            currentMenu->DestroyMenu();
   //         }

   //         DrawMenuBar();
   //      }
   //   }
   //   else
   //   {
   //      if (!enableFlag)
   //      {
   //         CMenu* currentMenu = GetMenu();
   //         menuState = new CMenuState(currentMenu);

   //         CDebugWriteFormat::setFilePath("C:\\MenuState-Debug.txt");
   //         menuState->dump(CDebugWriteFormat::getWriteFormat());
   //         CDebugWriteFormat::close();

   //         CMenu menu;
   //         menu.LoadMenu(IDR_CAMCADTYPE_Dummy);

   //         SetMenu(&menu);
   //         menu.Detach();

   //         if (currentMenu != NULL)
   //         {
   //            currentMenu->DestroyMenu();
   //         }

   //         DrawMenuBar();
   //      }
   //   }
   //}
   //else
   //{
   //   CMenu* oldMenu = GetMenu();

   //   CMenu menu;
   //   menu.LoadMenu(enableFlag ? IDR_CAMCADTYPE : IDR_CAMCADTYPE_Dummy);

   //   if (!enableFlag)
   //   {
   //      for (unsigned int itemIndex = 0;itemIndex < menu.GetMenuItemCount();itemIndex++)
   //      {
   //         menu.EnableMenuItem(itemIndex,enableFlag);
   //      }
   //   }

   //   SetMenu(&menu);
   //   menu.Detach();

   //   DrawMenuBar();

   //   if (oldMenu != NULL)
   //   {
   //      oldMenu->DestroyMenu();
   //   }
   //}

   //return;
}

void CMainFrame::disableMenuItems()
{
   enableMenuItems(false);
}

void CMainFrame::maximizeActiveChildFrame()
{
   CMDIChildWnd* childFrame = MDIGetActive(NULL);
   childFrame->MDIMaximize();
}

void updateClientRectFromBar(CRect& rect,CPoint& windowCenterPoint,CWnd& wnd)
{
   if (::IsWindow(wnd))
   {
      CRect windowRect;
      wnd.GetWindowRect(&windowRect);
      bool horizontalFlag = windowRect.Width() > windowRect.Height();

      if (horizontalFlag)
      {
         if (windowRect.bottom < windowCenterPoint.y)
         {
            if (windowRect.bottom > rect.top)
            {
               rect.top = windowRect.bottom;
            }
         }

         if (windowRect.top > windowCenterPoint.y)
         {
            if (windowRect.top < rect.bottom)
            {
               rect.bottom = windowRect.top;
            }
         }
      }
      else
      {
         if (windowRect.left > windowCenterPoint.y)
         {
            if (windowRect.left < rect.right)
            {
               rect.right = windowRect.left;
            }
         }

         if (windowRect.right < windowCenterPoint.y)
         {
            if (windowRect.right > rect.left)
            {
               rect.left = windowRect.right;
            }
         }
      }
   }
}

void CMainFrame::normalizeToMaximumActiveChildFrame()
{
   BOOL childFrameMaximizedFlag;
   CMDIChildWnd* childFrame = MDIGetActive(&childFrameMaximizedFlag);

   CRect windowRect;

   if (!childFrameMaximizedFlag)
   {
      childFrame->MDIMaximize();
   }

   childFrame->GetWindowRect(&windowRect);
   childFrame->MDIRestore();

   CRect clientRectWithoutBars = windowRect;
   CPoint windowCenterPoint = clientRectWithoutBars.CenterPoint();

   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_wndToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_AddToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_SnapToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_EditToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_QueryToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_GerberEducatorToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_GeomToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_PolyToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_GC_ToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_IT_ToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_PcbToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_OGP_ToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_RedlineToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_JoinToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_SequenceToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_ViewToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_FilterToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_TestAccessToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_SchematicToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_DFTToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_ToolsToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_CustomToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_Custom2ToolBar);
   //updateClientRectFromBar(clientRectWithoutBars,windowCenterPoint,m_navigator);

   int nonClientVerticalPadding   = 31;
   int nonClientHorizontalPadding = 12;

   childFrame->MoveWindow(0,0,clientRectWithoutBars.Width() - 1 - nonClientHorizontalPadding,
                           clientRectWithoutBars.Height() - nonClientVerticalPadding);
}

/////////////////////////////////////////////////////////////////////////////
// CMyToolbar

CMyToolBar::CMyToolBar(CFrameWnd& parentFrame)
: m_parentFrame(parentFrame)
, m_showState(false)
{
}

CMyToolBar::~CMyToolBar()
{
}

void CMyToolBar::saveCommandIds()
{
   if (m_commandIds.GetSize() == 0)
   {
      CToolBarCtrl& toolBarControl = GetToolBarCtrl();
      TBBUTTON buttonInfo;

      m_commandIds.SetSize(0,toolBarControl.GetButtonCount());

      for (int buttonIndex = 0;buttonIndex < toolBarControl.GetButtonCount();buttonIndex++)
      {
         toolBarControl.GetButton(buttonIndex,&buttonInfo);

         int commandId = buttonInfo.idCommand;

         m_commandIds.SetAtGrow(buttonIndex,commandId);
      }
   }
}

void CMyToolBar::restoreCommandIds()
{
   CToolBarCtrl& toolBarControl = GetToolBarCtrl();

   for (int buttonIndex = 0;
         buttonIndex < toolBarControl.GetButtonCount() && buttonIndex < m_commandIds.GetSize();buttonIndex++)
   {
      int commandId = m_commandIds.GetAt(buttonIndex);

      toolBarControl.SetCmdID(buttonIndex,commandId);
   }
}

void CMyToolBar::hide()
{
   WINDOWPLACEMENT windowPlacement;
   GetWindowPlacement(&windowPlacement);

   m_showState = (windowPlacement.showCmd != SW_HIDE);
   m_barStyle  = GetBarStyle();

   ShowWindow(SW_HIDE);

   m_parentFrame.FloatControlBar(this,CPoint(0,0));
}

void CMyToolBar::unhide()
{
   ShowWindow(m_showState ? SW_SHOW : SW_HIDE);
   SetBarStyle(m_barStyle);
}

void CMyToolBar::enable(bool enableFlag)
{
   if (enableFlag)
   {
      restoreCommandIds();
   }
   else
   {
      saveCommandIds();

      CToolBarCtrl& toolBarControl = GetToolBarCtrl();
      TBBUTTON buttonInfo;

      for (int buttonIndex = 0;buttonIndex < toolBarControl.GetButtonCount();buttonIndex++)
      {
         toolBarControl.GetButton(buttonIndex,&buttonInfo);
         int commandId = buttonInfo.idCommand;

         if (commandId != ID_SEPARATOR)
         {
            toolBarControl.SetCmdID(buttonIndex,ID_NoOperation);
         }
      }
   }
}

void CMyToolBar::disable()
{
   enable(false);
}

void CMyToolBar::hideButton(int id)
{
   int buttonIndex = CommandToIndex(id);

   if (buttonIndex != -1)
   {
      SetButtonStyle(buttonIndex,TBBS_SEPARATOR);
   }
}

BEGIN_MESSAGE_MAP(CMyToolBar, CToolBar)
   //{{AFX_MSG_MAP(CMyToolBar)
   ON_WM_CONTEXTMENU()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyToolBar message handlers

/******************************************************************************
* OnContextMenu
*/
void CMyToolBar::OnContextMenu(CWnd* pWnd, CPoint point) 
{
   CMenu     menu;      // toolbar right-click menu

   // load the menu from resources
   VERIFY(menu.LoadMenu(IDR_TOOLBAR_MENU));

   /// track the menu as a pop-up
   CMenu* pPopup = menu.GetSubMenu(0);
   ASSERT(pPopup != NULL);

   // force all message in this menu to be sent here
   pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}


/////////////////////////////////////////////////////////////////////////////
// ArrangeToolbars dialog
void CCEtoODBDoc::OnArrangeToolbars() 
{
   ArrangeToolbars dlg;
   CMainFrame* frame = (CMainFrame*)AfxGetMainWnd();

   dlg.m_add             = frame->m_AddToolBar.IsWindowVisible();
   dlg.m_currentSettings = frame->m_wndCurrentSettings.IsWindowVisible();
   dlg.m_navigator       = frame->getNavigator().IsWindowVisible();
   dlg.m_custom          = frame->m_CustomToolBar.IsWindowVisible();
   dlg.m_custom2         = frame->m_Custom2ToolBar.IsWindowVisible();
   dlg.m_dft             = frame->m_DFTToolBar.IsWindowVisible();
   dlg.m_tools           = frame->m_ToolsToolBar.IsWindowVisible();
   dlg.m_edit            = frame->m_EditToolBar.IsWindowVisible();
   dlg.m_filter          = frame->m_FilterToolBar.IsWindowVisible();
   dlg.m_gerberEducator  = frame->m_GerberEducatorToolBar.IsWindowVisible();
   dlg.m_geometry        = frame->m_GeomToolBar.IsWindowVisible();
   dlg.m_gc              = frame->m_GC_ToolBar.IsWindowVisible();
   dlg.m_it              = frame->m_IT_ToolBar.IsWindowVisible();
   dlg.m_join            = frame->m_JoinToolBar.IsWindowVisible();
   dlg.m_main            = frame->m_wndToolBar.IsWindowVisible();
   dlg.m_ogp             = frame->m_OGP_ToolBar.IsWindowVisible();
   dlg.m_pcb             = frame->m_PcbToolBar.IsWindowVisible();
   dlg.m_poly            = frame->m_PolyToolBar.IsWindowVisible();
   dlg.m_query           = frame->m_QueryToolBar.IsWindowVisible();
   dlg.m_schematic       = frame->m_SchematicToolBar.IsWindowVisible();
   dlg.m_sequence        = frame->m_SequenceToolBar.IsWindowVisible();
   dlg.m_snap            = frame->m_SnapToolBar.IsWindowVisible();
   dlg.m_view            = frame->m_ViewToolBar.IsWindowVisible();
   dlg.m_redline         = frame->m_RedlineToolBar.IsWindowVisible();
 //dlg.m_testAccess      = frame->m_TestAccessToolBar.IsWindowVisible();

   dlg.m_worldView = WorldViewOn;
   dlg.DoModal(); 
}

ArrangeToolbars::ArrangeToolbars(CWnd* pParent /*=NULL*/)
   : CDialog(ArrangeToolbars::IDD, pParent)
   , m_dft(FALSE)
   , m_tools(FALSE)
{
   //{{AFX_DATA_INIT(ArrangeToolbars)
   m_add = FALSE;
   m_currentSettings = FALSE;
   m_navigator = FALSE;
   m_custom = FALSE;
   m_custom2 = FALSE;
   m_edit = FALSE;
   m_filter = FALSE;
   m_gerberEducator = FALSE;
   m_geometry = FALSE;
   m_gc = FALSE;
   m_it = FALSE;
   m_join = FALSE;
   m_main = FALSE;
   m_ogp = FALSE;
   m_pcb = FALSE;
   m_poly = FALSE;
   m_query = FALSE;
   m_schematic = FALSE;
   m_sequence = FALSE;
   m_snap = FALSE;
   m_view = FALSE;
   m_worldView = FALSE;
   m_redline = FALSE;

   m_dft = FALSE;
   //}}AFX_DATA_INIT
}

void ArrangeToolbars::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ArrangeToolbars)
   DDX_Check(pDX, IDC_ADD, m_add);
   DDX_Check(pDX, IDC_CURRENT_SETTINGS, m_currentSettings);
   DDX_Check(pDX, IDC_PCBNAVIGATOR, m_navigator);
   DDX_Check(pDX, IDC_CUSTOM, m_custom);
   DDX_Check(pDX, IDC_CUSTOM2, m_custom2);
   DDX_Check(pDX, IDC_EDIT, m_edit);
   DDX_Check(pDX, IDC_FILTER, m_filter);
   DDX_Check(pDX, IDC_GerberEducator, m_gerberEducator);
   DDX_Check(pDX, IDC_GEOMETRY, m_geometry);
   DDX_Check(pDX, IDC_GRAPHIC_CLASS, m_gc);
   DDX_Check(pDX, IDC_INSERT_TYPES, m_it);
   DDX_Check(pDX, IDC_JOIN, m_join);
   DDX_Check(pDX, IDC_MAIN, m_main);
   DDX_Check(pDX, IDC_OGP, m_ogp);
   DDX_Check(pDX, IDC_PCB, m_pcb);
   DDX_Check(pDX, IDC_POLY, m_poly);
   DDX_Check(pDX, IDC_QUERY, m_query);
   DDX_Check(pDX, IDC_SCHEMATIC, m_schematic);
   DDX_Check(pDX, IDC_SEQUENCE, m_sequence);
   DDX_Check(pDX, IDC_SNAP, m_snap);
   DDX_Check(pDX, IDC_VIEW, m_view);
   DDX_Check(pDX, IDC_WORLD_VIEW, m_worldView);
   DDX_Check(pDX, IDC_REDLINE, m_redline);
   DDX_Check(pDX, IDC_DFT, m_dft);
   //}}AFX_DATA_MAP
   DDX_Check(pDX, IDC_TOOLS, m_tools);
}

BEGIN_MESSAGE_MAP(ArrangeToolbars, CDialog)
   //{{AFX_MSG_MAP(ArrangeToolbars)
   ON_BN_CLICKED(ID_APPLY, OnApply)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ArrangeToolbars message handlers
void ArrangeToolbars::OnApply() 
{
   UpdateData();

   CMainFrame* frame = (CMainFrame*)AfxGetMainWnd();

   if (m_main ^ frame->m_wndToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_TOOLBAR, 0L);      

   if (m_add ^ frame->m_AddToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_ADDTOOLBAR, 0L);      

   if (m_dft ^ frame->m_DFTToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_DFTTOOLBAR, 0L);      

   if (m_tools ^ frame->m_ToolsToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_TOOLSTOOLBAR, 0L);      

   if (m_currentSettings ^ frame->m_wndCurrentSettings.IsWindowVisible())
      SendMessage(WM_COMMAND, CG_ID_VIEW_CURRENTSETTINGS, 0L);    

   if (m_navigator ^ frame->getNavigator().IsWindowVisible())
      SendMessage(WM_COMMAND, ID_PCB_CAMCADNAVIGATOR, 0L);    

   if (m_custom ^ frame->m_CustomToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_CUSTOMTOOLBAR, 0L);      

   if (m_custom2 ^ frame->m_Custom2ToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_CUSTOM2TOOLBAR, 0L);     

   if (m_edit ^ frame->m_EditToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_EDITTOOLBAR, 0L);    

   if (m_gerberEducator ^ frame->m_GerberEducatorToolBar.IsWindowVisible())
		SendMessage(WM_COMMAND, ID_GerberEducatorToolbar, 0L);

   if (m_geometry ^ frame->m_GeomToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_GEOMTOOLBAR, 0L);     

   if (m_gc ^ frame->m_GC_ToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_GC_TOOLBAR, 0L);      

   if (m_it ^ frame->m_IT_ToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_IT_TOOLBAR, 0L);      

   if (m_join ^ frame->m_JoinToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_JOINTOOLBAR, 0L);     

   if (m_ogp ^ frame->m_OGP_ToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_OGPTOOLBAR, 0L);      

   if (m_poly ^ frame->m_PolyToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_POLYTOOLBAR, 0L);     

   if (m_query ^ frame->m_QueryToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_QUERYTOOLBAR, 0L);    

   if (m_schematic ^ frame->m_SchematicToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_SCHEMATICTOOLBAR, 0L);      

   if (m_sequence ^ frame->m_SequenceToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_SEQUENCETOOLBAR, 0L);    

   if (m_snap ^ frame->m_SnapToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_SNAPTOOLBAR, 0L);     

   if (m_view ^ frame->m_ViewToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_VIEWTOOLBAR, 0L);     

   if (m_pcb ^ frame->m_PcbToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_PCBTOOLBAR, 0L);      

   if (m_filter ^ frame->m_FilterToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_FILTERTOOLBAR, 0L);      

   if (m_redline ^ frame->m_RedlineToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_REDLINETOOLBAR, 0L);     

   /*if (m_testAccess ^ frame->m_TestAccessToolBar.IsWindowVisible())
      SendMessage(WM_COMMAND, ID_VIEW_TESTACCESSOFFSETTOOLBAR, 0L);     */
      
   WorldViewOn = m_worldView;
   worldView->ShowWindow(WorldViewOn);
}

void ArrangeToolbars::OnOK() 
{
   OnApply();  
   CDialog::OnOK();
}
       

/******************************************************************************
* OnToolbarDropDown
*/
void CMainFrame::OnToolbarDropDown(NMHDR* pnmhdr, LRESULT *plr)
{
   NMTOOLBAR* pnmtb = (NMTOOLBAR*)pnmhdr;

   CWnd *pWnd;
// UINT nID;

   // Switch on button command id's.
   switch (pnmtb->iItem)
   {
   case ID_COLORSETS_CUSTOM:
      pWnd = &m_ViewToolBar;
//    nID  = IDR_TOOLBAR_POPUP;
      break;
   default:
      return;
   }

   CCEtoODBDoc *doc = (CCEtoODBDoc*)activeView->GetDocument();
   
   // load and display popup menu
   CMenu menu;
   menu.CreatePopupMenu();

   for (int i = 0; i < MAX_CUSTOM_COLORSETS; i++)
   {
      menu.AppendMenu(MF_STRING, doc->CustomColorsetButtonIDs[i], doc->CustomLayersetNames.GetAt(i));
   }
   
   CRect rc;
   pWnd->SendMessage(TB_GETRECT, pnmtb->iItem, (LPARAM)&rc);
   pWnd->ClientToScreen(&rc);
   
   menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, rc.left, rc.bottom, this, &rc);
}

void CCEtoODBDoc::OnColorsetsCustom() 
{
   int i=0;
   
}