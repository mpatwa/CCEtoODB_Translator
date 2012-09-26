// $Header: /CAMCAD/5.0/MainFrm.cpp 77    5/23/07 12:07p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "mainfrm.h"
#include "splash.h"
#include "license.h"
#include "measure.h"
#include <afxadv.h>
#include <afxpriv.h>
#include "crypt.h"
#include "coords.h"
#include "api_hide.h"
#include "response.h"
#include "RwLib.h"
#include "CAMCADNavigator.h"
#include "Variant.h"
#include "MultipleMachine.h"
#include <dde.h>
#include "GerberEducatorToolbar.h"
#ifdef COMPILE_MULTIMON_STUBS
// XXXXXXXXX
#else
// XXXXXXXXX
#endif
//*rcf BUG MAYBE?  This was not defined before, but now we get unresolved references from MultiMon.h if we do not define it.
#define COMPILE_MULTIMON_STUBS
#include <MultiMon.h>
#include "olhapi.h"


// 20040706 - knv
#if !defined(_countof)
#define _countof(array) \
   (sizeof(array)/sizeof(array[0]))
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CView *activeView; // from CCVIEW.CPP
extern BOOL NoUI;
extern BOOL LockApplication; // from API.CPP
extern HideCAMCADDlg *hideCAMCADDlg; // from API_HIDE.CPP

BOOL ClosingApp = FALSE;
CStatusBar *StatusBar = NULL;
CDialogBar *CurrentSettings;
CString toolbarFile, worldViewFile;

void InitCurrentSettings();
void InitMeasureDlg(BOOL UpdateRadioButtons);

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
   ON_COMMAND_EX(CG_ID_VIEW_MEASURE, OnBarCheck)
   ON_UPDATE_COMMAND_UI(CG_ID_VIEW_MEASURE, OnUpdateControlBarMenu)
   ON_WM_INITMENU()
   ON_COMMAND_EX(CG_ID_VIEW_CURRENTSETTINGS, OnBarCheck)
   ON_UPDATE_COMMAND_UI(CG_ID_VIEW_CURRENTSETTINGS, OnUpdateControlBarMenu)

	ON_COMMAND_EX(ID_PCB_CAMCADNAVIGATOR, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_PCB_CAMCADNAVIGATOR, OnUpdateControlBarMenu)

	ON_COMMAND_EX(ID_GerberEducatorToolbar, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_GerberEducatorToolbar, OnUpdateControlBarMenu)
	//ON_UPDATE_COMMAND_UI(ID_SetupDataType, OnUpdateGerberEducator)
	//ON_UPDATE_COMMAND_UI(ID_CreateGeometries, OnUpdateGerberEducator)
	//ON_UPDATE_COMMAND_UI(ID_ReviewGeometries, OnUpdateGerberEducator)
	//ON_UPDATE_COMMAND_UI(ID_ReviewComponents, OnUpdateGerberEducator)

	ON_COMMAND_EX(ID_VIEW_TOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_ADDTOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_ADDTOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_SNAPTOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_SNAPTOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_EDITTOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_EDITTOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_QUERYTOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_QUERYTOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_GEOMTOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_GEOMTOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_POLYTOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_POLYTOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_GC_TOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_GC_TOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_IT_TOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_IT_TOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_PCBTOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_PCBTOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_OGPTOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_OGPTOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_REDLINETOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_REDLINETOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_JOINTOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_JOINTOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_SEQUENCETOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_SEQUENCETOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_SCHEMATICTOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_SCHEMATICTOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_FILTERTOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_FILTERTOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_DFTTOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_DFTTOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_TOOLSTOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLSTOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_VIEWTOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_VIEWTOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_TESTACCESSOFFSETTOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_TESTACCESSOFFSETTOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_CUSTOMTOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_CUSTOMTOOLBAR, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_CUSTOM2TOOLBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_CUSTOM2TOOLBAR, OnUpdateControlBarMenu)
   ON_BN_CLICKED(IDC_BUTTON_NAV_FIND,  NavCallFind)
   ON_UPDATE_COMMAND_UI(IDC_BUTTON_NAV_FIND, OnUpdateNavCallFind)
   ON_BN_CLICKED(IDC_BUTTON_NAV_CLEAR, NavCallClear)
   ON_UPDATE_COMMAND_UI(IDC_BUTTON_NAV_CLEAR, OnUpdateNavCallClear)
	ON_BN_CLICKED(IDC_BTN_OPTIONS, NavCallOptions)
	ON_UPDATE_COMMAND_UI(IDC_BTN_OPTIONS, OnUpdateNavCallOptions)
	ON_BN_CLICKED(IDC_BUTTON_VARIANT_CHANGE, PCBNavCallVariantChange)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_VARIANT_CHANGE, OnUpdatePCBNavVariantButtons)
	ON_BN_CLICKED(IDC_BUTTON_MACHINE_CHANGE, PCBNavCallMachineChange)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_MACHINE_CHANGE, OnUpdatePCBNavMachineButtons)

	ON_COMMAND(ID__DELETEPROBE, frmOnCmdDeleteProbe)
	ON_COMMAND(ID__UNPLACEPROBE, frmOnCmdUnplaceProbe)
	//ON_COMMAND(ID__CREATEPROBE33634, frmOnCreateProbe)
	ON_COMMAND(ID__EDITPROBE33636, frmOnEditProbe)
	ON_COMMAND(ID_HIGHLIGHT_NET, frmOnHighlightNet)

   //{{AFX_MSG_MAP(CMainFrame)
   ON_WM_CREATE()
   ON_WM_CLOSE()
   ON_COMMAND(ID_SAVE_MENU_TEXT, OnSaveMenuText)
   ON_COMMAND(ID_LOAD_MENU_TEXT, OnLoadMenuText)
   ON_COMMAND(ID_SAVE_TOOLBAR_FILE, OnSaveToolbarFile)
   ON_WM_INITMENUPOPUP()
   ON_WM_TIMER()
   ON_COMMAND(ID_TA_PLACE_OFFSETS, OnTaPlaceOffsets)
   ON_NOTIFY(TBN_DROPDOWN, ID_VIEW_VIEWTOOLBAR, OnToolbarDropDown)
   ON_WM_INITMENU()
   //}}AFX_MSG_MAP
   // Global help commands
   //ON_COMMAND(ID_HELP_INDEX, CMDIFrameWnd::OnHelpIndex)
   ON_COMMAND(ID_HELP_USING, CMDIFrameWnd::OnHelpUsing)
   ON_COMMAND(ID_HELP, OnHelp)
   ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
   //ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpIndex)
   ON_UPDATE_COMMAND_UI(ID_SCHEMATICLINK_SCHEMATICLINK, OnUpdateToolsSchematiclink)
   ON_WM_SIZE()
   ON_WM_MOVE()

   ON_MESSAGE(WM_DDE_EXECUTE, OnDDEExecute) // for MFC bug fix

	ON_WM_DROPFILES()
END_MESSAGE_MAP()

static UINT BASED_CODE indicators[] =
{
   ID_SEPARATOR,           // status line indicator
   IDS_FILES,
   IDS_SCALE,
   IDS_POSITION_X,
   IDS_POSITION_Y,
   IDS_UNITS,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction
CMainFrame::CMainFrame()
: m_dockState(NULL)
, m_wndToolBar(*this)
, m_AddToolBar(*this)
, m_SnapToolBar(*this)
, m_EditToolBar(*this)
, m_QueryToolBar(*this)
, m_GerberEducatorToolBar(*this)
, m_GeomToolBar(*this)
, m_PolyToolBar(*this)
, m_GC_ToolBar(*this)
, m_IT_ToolBar(*this)
, m_PcbToolBar(*this)
, m_OGP_ToolBar(*this)
, m_RedlineToolBar(*this)
, m_JoinToolBar(*this)
, m_SequenceToolBar(*this)
, m_ViewToolBar(*this)
, m_FilterToolBar(*this)
, m_TestAccessToolBar(*this)
, m_SchematicToolBar(*this)
, m_DFTToolBar(*this)
, m_ToolsToolBar(*this)
, m_CustomToolBar(*this)
, m_Custom2ToolBar(*this)
, m_menuItemsEnabled(true)
, m_exitWithoutSave(false)
{
   m_Activated = FALSE;
}

CMainFrame::~CMainFrame() 
{
   delete m_dockState;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
      return -1;

   GetWindowPlacement(&lastWindowPlacement);

   if (!m_wndToolBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_SIZE_DYNAMIC, ID_VIEW_TOOLBAR) ||
      !m_wndToolBar.LoadToolBar(IDR_MAINFRAME) )
   {
      TRACE0("Failed to create Main Toolbar\n");
      return -1;      // fail to create
   }

   if (!m_wndStatusBar.Create(this) ||
      !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
   {
      TRACE0("Failed to create status bar\n");
      return -1;      // fail to create
   }
   else 
   {
      StatusBar = &m_wndStatusBar;
      StatusBar->SetPaneInfo(StatusBar->CommandToIndex(IDS_FILES), 
            IDS_FILES, SBPS_NORMAL, 200);
      StatusBar->SetPaneInfo(StatusBar->CommandToIndex(IDS_SCALE), 
            IDS_SCALE, SBPS_NORMAL, 75);
      StatusBar->SetPaneInfo(StatusBar->CommandToIndex(IDS_POSITION_X), 
            IDS_POSITION_X, SBPS_NORMAL, 60);
      StatusBar->SetPaneInfo(StatusBar->CommandToIndex(IDS_POSITION_Y), 
            IDS_POSITION_Y, SBPS_NORMAL, 60);
      StatusBar->SetPaneInfo(StatusBar->CommandToIndex(IDS_UNITS), 
            IDS_UNITS, SBPS_NORMAL, 70);
   }

   m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
   EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_wndToolBar);
   m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
      CBRS_TOOLTIPS | CBRS_FLYBY);

   // CG: The following line was added by the Splash Screen component.
   CRect monitorRect = getSavedMonitorRect();
   CSplashWnd::ShowSplashScreen(this,&monitorRect);

   // CG: The following block was inserted by the 'Dialog Bar' component
   {
      // Initialize dialog bar m_wndCurrentSettings                                    
      if (!m_wndCurrentSettings.Create(this, CG_IDD_CURRENTSETTINGS,
         CBRS_RIGHT | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_HIDE_INPLACE,
         CG_ID_VIEW_CURRENTSETTINGS))
      {
         TRACE0("Failed to create dialog bar m_wndCurrentSettings\n");
         return -1;     // fail to create
      }

      m_wndCurrentSettings.EnableDocking(CBRS_ALIGN_RIGHT | CBRS_ALIGN_LEFT);
      DockControlBar(&m_wndCurrentSettings);
   }

   CurrentSettings = &m_wndCurrentSettings;
   InitCurrentSettings();

   {
		// Initialize dialog bar m_wndCCNavigator                                    
		if (!m_navigator.Create(this, CG_IDD_NAVIGATOR,
			CBRS_RIGHT | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_HIDE_INPLACE,
			ID_PCB_CAMCADNAVIGATOR))
		{
			TRACE0("Failed to create dialog bar CCNavigator\n");
			return -1;     // fail to create
		}

		m_navigator.EnableDocking(CBRS_ALIGN_RIGHT | CBRS_ALIGN_LEFT);
		DockControlBar(&m_navigator);
		m_navigator.setDoc(NULL);
	}

   if (!m_AddToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_ADDTOOLBAR) ||
      !m_AddToolBar.LoadToolBar(IDR_ADD_TOOLBAR) )
   {                                               
      TRACE0("Failed to create Add Toolbar\n");
      return -1;      // fail to create
   }

   m_AddToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_AddToolBar);
   m_AddToolBar.SetWindowText("Add");

   if (!m_SnapToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_SNAPTOOLBAR) ||
      !m_SnapToolBar.LoadToolBar(IDR_SNAP_TOOLBAR) )
   {                                               
      TRACE0("Failed to create Snap Toolbar\n");
      return -1;      // fail to create
   }

   m_SnapToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_SnapToolBar);
   m_SnapToolBar.SetWindowText("Snap");

   if (!m_EditToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_EDITTOOLBAR) ||
      !m_EditToolBar.LoadToolBar(IDR_EDIT_TOOLBAR) )
   {                                               
      TRACE0("Failed to create Edit Toolbar\n");
      return -1;      // fail to create
   }

   m_EditToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_EditToolBar);
   m_EditToolBar.SetWindowText("Edit");

   if (!m_QueryToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_QUERYTOOLBAR) ||
      !m_QueryToolBar.LoadToolBar(IDR_QUERY_TOOLBAR) )
   {                                               
      TRACE0("Failed to create Query Toolbar\n");
      return -1;      // fail to create
   }

   m_QueryToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_QueryToolBar);
   m_QueryToolBar.SetWindowText("Query");

   if (!m_GeomToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_GEOMTOOLBAR) ||
      !m_GeomToolBar.LoadToolBar(IDR_GEOM_TOOLBAR) )
   {                                               
      TRACE0("Failed to create Geometry Toolbar\n");
      return -1;      // fail to create
   }

   m_GeomToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_GeomToolBar);
   m_GeomToolBar.SetWindowText("Geometry");

   if (!m_PolyToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_POLYTOOLBAR) ||
      !m_PolyToolBar.LoadToolBar(IDR_POLY_TOOLBAR) )
   {                                               
      TRACE0("Failed to create Polyline Toolbar\n");
      return -1;      // fail to create
   }

   m_PolyToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_PolyToolBar);
   m_PolyToolBar.SetWindowText("Poly");

	// Create Gerber Educator Toolbar
   if (!m_GerberEducatorToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_GerberEducatorToolbar) ||
      !m_GerberEducatorToolBar.LoadToolBar(IDR_GerberEducator_Toolbar) )
   {                                               
			TRACE0("Failed to create Gerber Educator Toolbar\n");
      return -1;      // fail to create
   }
   else
   {
      m_GerberEducatorToolBar.hideButton(ID_GerberEducatorCommand_ColorsDialog);
   }

   m_GerberEducatorToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_GerberEducatorToolBar);
   m_GerberEducatorToolBar.SetWindowText("Gerber Educator");

   if (!m_GC_ToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_GC_TOOLBAR) ||
      !m_GC_ToolBar.LoadToolBar(IDR_GC_TOOLBAR) )
   {                                               
      TRACE0("Failed to create Graphic Class Toolbar\n");
      return -1;      // fail to create
   }

   m_GC_ToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_GC_ToolBar);
   m_GC_ToolBar.SetWindowText("Show Graphic Classes");

   if (!m_IT_ToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_IT_TOOLBAR) ||
      !m_IT_ToolBar.LoadToolBar(IDR_IT_TOOLBAR) )
   {                                               
      TRACE0("Failed to create Insert Type Toolbar\n");
      return -1;      // fail to create
   }

   m_IT_ToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_IT_ToolBar);
   m_IT_ToolBar.SetWindowText("Show Insert Types");

   if (!m_PcbToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_PCBTOOLBAR) ||
      !m_PcbToolBar.LoadToolBar(IDR_PCB_TOOLBAR) )
   {                                               
      TRACE0("Failed to create PCB Toolbar\n");
      return -1;      // fail to create
   }

   m_PcbToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_PcbToolBar);
   m_PcbToolBar.SetWindowText("PCB");

   if (!m_OGP_ToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_OGPTOOLBAR) ||
      !m_OGP_ToolBar.LoadToolBar(IDR_OGP_TOOLBAR) )
   {                                               
      TRACE0("Failed to create OGP Toolbar\n");
      return -1;      // fail to create
   }

   m_OGP_ToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_OGP_ToolBar);
   m_OGP_ToolBar.SetWindowText("OGP");

   if (!m_RedlineToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_REDLINETOOLBAR) ||
      !m_RedlineToolBar.LoadToolBar(IDR_REDLINE_TOOLBAR) )
   {                                               
      TRACE0("Failed to create Redline Toolbar\n");
      return -1;      // fail to create
   }

   m_RedlineToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_RedlineToolBar);
   m_RedlineToolBar.SetWindowText("Redline");

   if (!m_JoinToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_JOINTOOLBAR) ||
      !m_JoinToolBar.LoadToolBar(IDR_JOIN_TOOLBAR) )
   {                                               
      TRACE0("Failed to create Join Toolbar\n");
      return -1;      // fail to create
   }

   m_JoinToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_JoinToolBar);
   m_JoinToolBar.SetWindowText("Join");

   if (!m_SequenceToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_SEQUENCETOOLBAR) ||
      !m_SequenceToolBar.LoadToolBar(IDR_SEQUENCE_TOOLBAR) )
   {                                               
      TRACE0("Failed to create Sequence Toolbar\n");
      return -1;      // fail to create
   }

   m_SequenceToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_SequenceToolBar);
   m_SequenceToolBar.SetWindowText("Sequence");

   if (!m_ViewToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_VIEWTOOLBAR) ||
      !m_ViewToolBar.LoadToolBar(IDR_VIEW_TOOLBAR) )
   {                                               
      TRACE0("Failed to create View Toolbar\n");
      return -1;      // fail to create
   }

   m_ViewToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_ViewToolBar);
   m_ViewToolBar.SetWindowText("View");

   if (!m_TestAccessToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_TESTACCESSOFFSETTOOLBAR) ||
      !m_TestAccessToolBar.LoadToolBar(IDR_TEST_TOOLBAR) )
   {                                               
      TRACE0("Failed to create Test Access Offset Toolbar\n");
      return -1;      // fail to create
   }

   m_TestAccessToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_TestAccessToolBar);
   m_TestAccessToolBar.SetWindowText("Test Access Offset");

   if (!m_SchematicToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_SCHEMATICTOOLBAR) ||
      !m_SchematicToolBar.LoadToolBar(IDR_SCHEMATIC_TOOLBAR) )
   {                                               
      TRACE0("Failed to create Schematic Toolbar\n");
      return -1;      // fail to create
   }

   m_SchematicToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_SchematicToolBar);
   m_SchematicToolBar.SetWindowText("Schematic");

   if (!m_DFTToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_DFTTOOLBAR) ||
      !m_DFTToolBar.LoadToolBar(IDR_DFT_TOOLBAR) )
   {                                               
      TRACE0("Failed to create DFT Toolbar\n");
      return -1;      // fail to create
   }

   m_DFTToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_DFTToolBar);
   m_DFTToolBar.SetWindowText("DFT");

   if (!m_ToolsToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_TOOLSTOOLBAR) ||
      !m_ToolsToolBar.LoadToolBar(IDR_TOOLS_TOOLBAR) )
   {                                               
      TRACE0("Failed to create DFT Toolbar\n");
      return -1;      // fail to create
   }

   m_ToolsToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_ToolsToolBar);
   m_ToolsToolBar.SetWindowText("Tools");
	

// Set the drop arrow.
// m_ViewToolBar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
   int index = m_ViewToolBar.CommandToIndex(ID_COLORSETS_CUSTOM);
   DWORD dwStyle = m_ViewToolBar.GetButtonStyle(index);
   dwStyle |= TBSTYLE_DROPDOWN;
   m_ViewToolBar.SetButtonStyle(index, dwStyle);

   if (!m_FilterToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_FILTERTOOLBAR) ||
      !m_FilterToolBar.LoadToolBar(IDR_FILTER_TOOLBAR) )
   {                                               
      TRACE0("Failed to create Filter Toolbar\n");
      return -1;      // fail to create
   }

   m_FilterToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_FilterToolBar);
   m_FilterToolBar.SetWindowText("Filter");

   if (!m_CustomToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_CUSTOMTOOLBAR) ||
      !m_CustomToolBar.LoadToolBar(IDR_CUSTOM_TOOLBAR) )
   {                                               
      TRACE0("Failed to create Custom Toolbar\n");
      return -1;      // fail to create
   }

   m_CustomToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_CustomToolBar);
   m_CustomToolBar.SetWindowText("Custom");
   m_CustomToolBar.RemeberButtons();
   m_CustomToolBar.tbNum = 1;

   if (!m_Custom2ToolBar.Create(this, 
            WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS | 
            CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_VIEW_CUSTOM2TOOLBAR) ||
      !m_Custom2ToolBar.LoadToolBar(IDR_CUSTOM2_TOOLBAR) )
   {                                               
      TRACE0("Failed to create Custom2 Toolbar\n");
      return -1;      // fail to create
   }

   m_Custom2ToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_Custom2ToolBar);
   m_Custom2ToolBar.SetWindowText("Custom2");
   m_Custom2ToolBar.RemeberButtons();
   m_Custom2ToolBar.tbNum = 2;

   if (NoUI)
   {
      m_wndToolBar.ShowWindow(SW_HIDE);
   }

   m_AddToolBar.ShowWindow(SW_HIDE);
   m_DFTToolBar.ShowWindow(SW_HIDE);
	m_ToolsToolBar.ShowWindow(SW_HIDE);
   m_SnapToolBar.ShowWindow(SW_HIDE);
   m_EditToolBar.ShowWindow(SW_HIDE);
   m_QueryToolBar.ShowWindow(SW_HIDE);
	m_GerberEducatorToolBar.ShowWindow(SW_HIDE);
   m_GeomToolBar.ShowWindow(SW_HIDE);
   m_PolyToolBar.ShowWindow(SW_HIDE);
   m_GC_ToolBar.ShowWindow(SW_HIDE);
   m_IT_ToolBar.ShowWindow(SW_HIDE);
   m_PcbToolBar.ShowWindow(SW_HIDE);
   m_OGP_ToolBar.ShowWindow(SW_HIDE);
   m_RedlineToolBar.ShowWindow(SW_HIDE);
   m_JoinToolBar.ShowWindow(SW_HIDE);
   m_SchematicToolBar.ShowWindow(SW_HIDE);
   m_SequenceToolBar.ShowWindow(SW_HIDE);
   m_ViewToolBar.ShowWindow(SW_HIDE);
   m_FilterToolBar.ShowWindow(SW_HIDE);
   m_TestAccessToolBar.ShowWindow(SW_HIDE);
   m_wndCurrentSettings.ShowWindow(SW_HIDE);
	m_navigator.ShowWindow(SW_HIDE);
   m_CustomToolBar.ShowWindow(SW_HIDE);
   m_Custom2ToolBar.ShowWindow(SW_HIDE);

   CString temp = getApp().getUserPath();

   switch (Product)
   {
      case PRODUCT_GRAPHIC:
         if (NoUI)
            temp += "graphi_n.";
         else
            temp += "graphic.";
      break;
      case PRODUCT_PROFESSIONAL:
         if (NoUI)
            temp += "pro_n.";
         else
            temp += "pro.";
      break;
      case PRODUCT_PCB_TRANSLATOR:
         if (NoUI)
            temp += "pcb_n.";
         else
            temp += "pcb.";
      break;
      case PRODUCT_VISION:
         if (NoUI)
            temp += "vision_n.";
         else
            temp += "vision.";
      break;
   }

   toolbarFile = temp;
   toolbarFile += "tb";

   worldViewFile = temp;
   worldViewFile += "wv";

   return 0;
}

CAMCADNavigator& CMainFrame::getNavigator()
{
   return m_navigator;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
   CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
   CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {return CMDIFrameWnd::PreCreateWindow(cs);}

/*void CMainFrame::OnViewCustomize() 
{
   m_AddToolBar.GetToolBarCtrl().Customize();
}*/

void CMainFrame::updateMenuState()
{
   bool modifiedFlag = false;
   
   CMenu* menu = GetMenu();
   if (menu != NULL)
   {
      int itemCount = menu->GetMenuItemCount();
      unsigned int enableFlags = (m_menuItemsEnabled ? MF_ENABLED : MF_GRAYED);
      MENUITEMINFO menuItemInfo;
      menuItemInfo.cbSize = sizeof(MENUITEMINFO);
      menuItemInfo.fMask  = MIIM_STATE;


      for (int itemIndex = 0;itemIndex < itemCount;itemIndex++)
      {
         menu->GetMenuItemInfo(itemIndex,&menuItemInfo,TRUE);

         if ((menuItemInfo.fState & enableFlags) == 0)
         {
            menu->EnableMenuItem(itemIndex,MF_BYPOSITION | enableFlags);
            modifiedFlag = true;
         }      
      }
   }

   if (modifiedFlag)
   {
      DrawMenuBar();
   }
}

void CMainFrame::OnInitMenu(CMenu* pMenu)
{
   CMDIFrameWnd::OnInitMenu(pMenu);

   if (hideCAMCADDlg)
   {
      CMenu *sysMenu = GetSystemMenu(FALSE);

      UINT res;
   
      res = sysMenu->EnableMenuItem(SC_MINIMIZE, MF_BYCOMMAND | MF_GRAYED);
      res = sysMenu->EnableMenuItem(SC_MAXIMIZE, MF_BYCOMMAND | MF_GRAYED);
      res = sysMenu->EnableMenuItem(SC_RESTORE , MF_BYCOMMAND | MF_GRAYED);
   }

   updateMenuState();

/*  
   // CG: This block added by 'Tip of the Day' component.
   {
      // TODO: This code adds the "Tip of the Day" menu item
      // on the fly.  It may be removed after adding the menu
      // item to all applicable menu items using the resource
      // editor.

      // Add Tip of the Day menu item on the fly!
      static CMenu* pSubMenu = NULL;

      CString strHelp; strHelp.LoadString(CG_IDS_TIPOFTHEDAYHELP);
      CString strMenu;
      int nMenuCount = pMenu->GetMenuItemCount();
      BOOL bFound = FALSE;
      for (int i=0; i < nMenuCount; i++) 
      {
         pMenu->GetMenuString(i, strMenu, MF_BYPOSITION);
         if (strMenu == strHelp)
         { 
            pSubMenu = pMenu->GetSubMenu(i);
            bFound = TRUE;
            ASSERT(pSubMenu != NULL);
         }
      }

      CString strTipMenu;
      strTipMenu.LoadString(CG_IDS_TIPOFTHEDAYMENU);
      if (!bFound)
      {
         // Help menu is not available. Please add it!
         if (pSubMenu == NULL) 
         {
            // The same pop-up menu is shared between mainfrm and frame 
            // with the doc.
            static CMenu popUpMenu;
            pSubMenu = &popUpMenu;
            pSubMenu->CreatePopupMenu();
            pSubMenu->InsertMenu(0, MF_STRING|MF_BYPOSITION, 
               CG_IDS_TIPOFTHEDAY, strTipMenu);
         } 
         pMenu->AppendMenu(MF_STRING|MF_BYPOSITION|MF_ENABLED|MF_POPUP, 
            (UINT)pSubMenu->m_hMenu, strHelp);
         DrawMenuBar();
      } 
      else
      {      
         // Check to see if the Tip of the Day menu has already been added.
         pSubMenu->GetMenuString(0, strMenu, MF_BYPOSITION);

         if (strMenu != strTipMenu) 
         {
            // Tip of the Day submenu has not been added to the 
            // first position, so add it.
            pSubMenu->InsertMenu(0, MF_BYPOSITION);  // Separator
            pSubMenu->InsertMenu(0, MF_STRING|MF_BYPOSITION, 
               CG_IDS_TIPOFTHEDAY, strTipMenu);
         }
      }
   }
*/
}

void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
   CMDIFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

   if (pPopupMenu->GetMenuItemID(0) == ID_COLORSETS_CUSTOM1)
   {
      for (int i = 0; i < MAX_CUSTOM_COLORSETS; i++)
      {
         pPopupMenu->RemoveMenu(0, MF_BYPOSITION);
      }

      CCEtoODBDoc *doc = (CCEtoODBDoc*)activeView->GetDocument();
      for (int i = 0; i < MAX_CUSTOM_COLORSETS; i++)
      {
         pPopupMenu->AppendMenu(MF_STRING, doc->CustomColorsetButtonIDs[i], doc->CustomLayersetNames.GetAt(i));
      }
   }
} 

void CMainFrame::OnClose() 
{
   if (!m_exitWithoutSave)
   {
      if (LockApplication)
      {
         ErrorMessage("CAMCAD Application Close is Locked", "Cannot close CAMCAD");
         return;
      }

	   // search through all the doc templates and the docs in each template to see if any one is locked
	   bool docLocked = false;
      CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
	   POSITION docTempPos = pApp->GetFirstDocTemplatePosition();
	   while (docTempPos)
	   {
		   CMultiDocTemplate* pDocTemplate = (CMultiDocTemplate*)pApp->GetNextDocTemplate(docTempPos);
		   POSITION pos = pDocTemplate->GetFirstDocPosition();
		   while (pos)
		   {
			   CCEtoODBDoc *pDoc = (CCEtoODBDoc*)pDocTemplate->GetNextDoc(pos);
			   if (pDoc->IsLocked())
			   {
				   docLocked = true;
				   CString buf;
				   buf.Format("CAMCAD Document %s is locked by %s.", pDoc->GetTitle(), pDoc->GetUserLock());
				   if (ErrorMessage("You should not close CAMCAD while another application has Locked it.\nAre you sure you want to override the Lock and Close CAMCAD?", buf, MB_ICONSTOP | MB_YESNO | MB_DEFBUTTON2) != IDYES)
					   return;
			   }
		   }
	   }

      saveWindowState();

      // Case dts0100409048 style exit
      if (!docLocked && pApp->getUseDialogsFlag())
      {
         bool noToAll = false;

         POSITION docTempPos = pApp->GetFirstDocTemplatePosition();
         while (docTempPos != NULL && !noToAll)
         {
            CMultiDocTemplate* pDocTemplate = (CMultiDocTemplate*)pApp->GetNextDocTemplate(docTempPos);
            POSITION pos = pDocTemplate->GetFirstDocPosition();
            while (pos != NULL && !noToAll)
            {
               CCEtoODBDoc *pDoc = (CCEtoODBDoc*)pDocTemplate->GetNextDoc(pos);

               CString name;

               if (pDoc->isFileLoaded())
               {
                  CFilePath filepath(pDoc->CCFileName);
                  name = filepath.getFileName();
               }
               else if (pDoc->DrawingImported)
               {
                  POSITION filepos = pDoc->getFileList().GetHeadPosition();
                  while (filepos != NULL)
                  {
                     FileStruct *file = pDoc->getFileList().GetNext(filepos);
                     if (!name.IsEmpty())
                        name += " | ";
                     name += file->getName();
                  }
               }
               
               // About deciding - User might select to save the file, then cancel out of file save
               // or something else goes wrong. So user is "deciding" until either Save is successful
               // or user says No to save.

               if (name.IsEmpty())
               {
                  // If name is empty then doc->isFileLoaded() and doc->DrawingImported are both false.
                  // So no name, no data, nothing to save, just nuke it.
                  pDoc->OnCloseDocument();
               }
               else
               {
                  // Has a name, and so presumably some data too. Query user if they want to save it.

                  bool deciding = true;
                  while (deciding)
                  {
                     CString msg;
                     msg.Format("Do you want to save %s?", name);
                     CYesNoNoToAllCancelDialog yesNoEtcDialog(msg);
                     int response = yesNoEtcDialog.DoModal();

                     if (response == IDCANCEL)
                     {
                        return;  // Cancelled exit, back to camcad
                     }
                     else if (response == IDYES)
                     {
                        //deciding = !pDoc->SaveDatafile();  // No longer deciding if Save worked, still deciding if it didn't
                     }
                     else if (response == IDNO)
                     {
                        deciding = false;  // User says toss this one
                     }
                     else if (response == IDNOTOALL)
                     {
                        deciding = false;
                        noToAll = true;    // User says toss them all
                     }

                     if (!deciding && !noToAll)
                     {
                        // Either doc was saved or user said No to save, in this case Mark wants
                        // the doc removed from teh camcad session. In multiple docs, even if user
                        // Cancels on a subsequent doc, this doc has been removed from the session.
                        // I'm not in favor of this, I think Cancel at any point should get you back
                        // to a session with all docs. But it is done as Mark asked for it to be.
                        pDoc->OnCloseDocument();
                     }
                  }
               }
            }
         }
      }


      //getApp().getCamcadLicense().warnExpiration();
   }

   ClosingApp = TRUE;

   CMDIFrameWnd::OnClose();

   SendResponse("CAMCAD Close", "");
}

// knv - 20051205
// Copied from "C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\atlmfc\src\mfc\winmdi.cpp"
// Fixed error when GetMenuItemCount(hMenuBar) returns -1.
HMENU CMainFrame::GetWindowMenuPopup(HMENU hMenuBar)
	// find which popup is the "Window" menu
{
	if (hMenuBar == NULL)
		return NULL;

	ASSERT(::IsMenu(hMenuBar));
	
	for (int iItem = ::GetMenuItemCount(hMenuBar) - 1;iItem >= 0;iItem--)
	{
		HMENU hMenuPop = ::GetSubMenu(hMenuBar, iItem);
		if (hMenuPop != NULL)
		{
			int iItemMax = ::GetMenuItemCount(hMenuPop);
			for (int iItemPop = 0; iItemPop < iItemMax; iItemPop++)
			{
				UINT nID = GetMenuItemID(hMenuPop, iItemPop);
				if (nID >= AFX_IDM_WINDOW_FIRST && nID <= AFX_IDM_WINDOW_LAST)
					return hMenuPop;
			}
		}
	}

	// no default menu found
	TRACE(traceAppMsg, 0, "Warning: GetWindowMenuPopup failed!\n");
	return NULL;
}

void CMainFrame::OnHelp()
{
   //getApp().OnHelpIndex();
}

void CMainFrame::OnContextHelp()
{
   this->OnHelp();
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default

	SetActiveWindow();      // activate us first !
	UINT nFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp != NULL);
	for (UINT iFile = 0; iFile < nFiles; iFile++)
	{
		TCHAR szFileName[_MAX_PATH];
		::DragQueryFile(hDropInfo, iFile, szFileName, _MAX_PATH);
		pApp->OpenDocumentFile(szFileName);
	}
	::DragFinish(hDropInfo);

//	CMDIFrameWnd::OnDropFiles(hDropInfo);
}

void CMainFrame::OnUpdateNavCallFind(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_navigator.getDocNotPCB());
}

void CMainFrame::OnUpdateNavCallClear(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_navigator.getDocNotPCB());
}

void CMainFrame::OnUpdateNavCallOptions(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_navigator.getDocNotPCB());
}

void CMainFrame::OnUpdatePCBNavVariantButtons(CCmdUI *pCmdUI)
{
	if (m_navigator.getCurFile() != NULL && m_navigator.getCurFile()->getVariantList().GetCount() > 0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnUpdatePCBNavMachineButtons(CCmdUI *pCmdUI)
{
   if (activeView != NULL)
   {
	   CCEtoODBDoc* doc = (CCEtoODBDoc*)activeView->GetDocument();

      /*if (doc != NULL && m_navigator.getCurFile() != NULL && doc->GetMachineCount(*(m_navigator.getCurFile())) > 0)
		   pCmdUI->Enable(TRUE);
	   else
		   pCmdUI->Enable(FALSE);*/
   }
   else
   {  
      pCmdUI->Enable(FALSE);
   }
}

void CMainFrame::OnUpdateToolsSchematiclink(CCmdUI *pCmdUI)
{
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();

   pCmdUI->SetCheck(pApp->getSchematicLinkController().isSchematicLinkInSession());
}

void CMainFrame::OnUpdateGerberEducator(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(TRUE);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
   CMDIFrameWnd::OnSize(nType, cx, cy);

   if (!LockApplication)
   {
      GetWindowPlacement(&lastWindowPlacement);
      //saveWindowState();
   }
   else
   {
      SetWindowPlacement(&lastWindowPlacement);
   }
}

void CMainFrame::OnMove(int x, int y)
{
   CMDIFrameWnd::OnMove(x, y);

   WINDOWPLACEMENT wndPlacement;
   GetWindowPlacement(&wndPlacement);

   int curWidth = wndPlacement.rcNormalPosition.right - wndPlacement.rcNormalPosition.left;
   int curHeight = wndPlacement.rcNormalPosition.bottom - wndPlacement.rcNormalPosition.top;
   int prevWidth = lastWindowPlacement.rcNormalPosition.right - lastWindowPlacement.rcNormalPosition.left;
   int prevHeight = lastWindowPlacement.rcNormalPosition.bottom - lastWindowPlacement.rcNormalPosition.top;

   if (LockApplication && curWidth == prevWidth && curHeight == prevHeight)
   {
      GetWindowPlacement(&lastWindowPlacement);
   }
   else if (!LockApplication)
   {
      GetWindowPlacement(&lastWindowPlacement);
      //saveWindowState();
   }
}

void CMainFrame::setDefaultWindowSize()
{
   WINDOWPLACEMENT windowPlacement;
   BOOL success = GetWindowPlacement(&windowPlacement);

   int xScreenPixels = GetSystemMetrics(SM_CXSCREEN);
   int yScreenPixels = GetSystemMetrics(SM_CYSCREEN);
   int xMargin = (int)(xScreenPixels * .01);
   int yMargin = (int)(yScreenPixels * .01);
   
   windowPlacement.rcNormalPosition.left   = xMargin;
   windowPlacement.rcNormalPosition.top    = yMargin;
   windowPlacement.rcNormalPosition.right  = xScreenPixels - 1 - xMargin;
   windowPlacement.rcNormalPosition.bottom = yScreenPixels - 1 - yMargin;

   windowPlacement.flags = 0;
   windowPlacement.showCmd = SW_SHOWNORMAL;

   success = SetWindowPlacement(&windowPlacement);
}

CRect CMainFrame::getSavedMonitorRect()
{
   CRect monitorRect;

   int xScreenPixels = GetSystemMetrics(SM_CXSCREEN);
   int yScreenPixels = GetSystemMetrics(SM_CYSCREEN);

   // default to primary monitor rect
   monitorRect.SetRect(0,0,xScreenPixels,yScreenPixels);

   CSupString windowParameters = AfxGetApp()->GetProfileString("MainFrame","Window Parameters","");

   if (!windowParameters.IsEmpty())
   {
      CStringArray param;
      windowParameters.Parse(param);

      if (param.GetSize() >= 4)
      {
         // Load the previous size of the dialog box from the INI/Registry
         int x  = atoi(param[0]);
         int y  = atoi(param[1]);
         int cx = atoi(param[2]);
         int cy = atoi(param[3]);

         CRect rect(x,y, x + cx,y + cy);

         // multiple monitor support
         POINT ul = rect.TopLeft();
         POINT ll = ul;
         ll.y     = rect.bottom;
         POINT lr = rect.BottomRight();
         POINT ur = lr;
         ur.y     = rect.top;

         HMONITOR ulMonitor = MonitorFromPoint(ul,MONITOR_DEFAULTTONULL);
         HMONITOR llMonitor = MonitorFromPoint(ll,MONITOR_DEFAULTTONULL);
         HMONITOR lrMonitor = MonitorFromPoint(lr,MONITOR_DEFAULTTONULL);
         HMONITOR urMonitor = MonitorFromPoint(ur,MONITOR_DEFAULTTONULL);

         if (ulMonitor != NULL      &&
             ulMonitor == llMonitor &&
             ulMonitor == lrMonitor &&
             ulMonitor == urMonitor)
         {
            // fully contained on a valid monitor
            MONITORINFO monitorInfo;
            monitorInfo.cbSize = sizeof(MONITORINFO);

            GetMonitorInfo(ulMonitor,&monitorInfo);

            monitorRect = monitorInfo.rcWork;
         }
      }
   }

   return monitorRect;
}

void CMainFrame::saveWindowState() 
{
   WINDOWPLACEMENT windowPlacement;
   BOOL success = GetWindowPlacement(&windowPlacement);

   CString windowParameters;
   CRect rect(windowPlacement.rcNormalPosition.left,
              windowPlacement.rcNormalPosition.top,
              windowPlacement.rcNormalPosition.right,
              windowPlacement.rcNormalPosition.bottom);

   windowParameters.Format("%d %d %d %d %d",
      rect.left,
      rect.top,
      rect.Width(),
      rect.Height(),
      windowPlacement.showCmd );

   AfxGetApp()->WriteProfileString("MainFrame","Window Parameters",windowParameters);  
}

void CMainFrame::restoreWindowState() 
{
   CSupString windowParameters = AfxGetApp()->GetProfileString("MainFrame","Window Parameters","");

   if (!windowParameters.IsEmpty())
   {
      CStringArray param;
      windowParameters.Parse(param);

      if (param.GetSize() >= 4)
      {
         // Load the previous size of the dialog box from the INI/Registry
         int x  = atoi(param[0]);
         int y  = atoi(param[1]);
         int cx = atoi(param[2]);
         int cy = atoi(param[3]);

         int dx,dy;

         int xScreenPixels = GetSystemMetrics(SM_CXSCREEN);
         int yScreenPixels = GetSystemMetrics(SM_CYSCREEN);

         CRect rect(x,y, x + cx,y + cy);

         CRect monitorRect = getSavedMonitorRect();

         dx = ((rect.right  >= monitorRect.right ) ? monitorRect.right  - rect.right  - 1  : 0);
         dy = ((rect.bottom >= monitorRect.bottom) ? monitorRect.bottom - rect.bottom - 1  : 0);
         rect.OffsetRect(dx,dy);

         dx = ((rect.left < monitorRect.left) ? monitorRect.left - rect.left : 0);
         dy = ((rect.top  < monitorRect.top ) ? monitorRect.top  - rect.top  : 0);
         rect.OffsetRect(dx,dy);

         if (rect.right  >= monitorRect.right ) rect.right  = monitorRect.right  - 1;
         if (rect.bottom >= monitorRect.bottom) rect.bottom = monitorRect.bottom - 1;

         WINDOWPLACEMENT windowPlacement;
         BOOL success = GetWindowPlacement(&windowPlacement);
   
         windowPlacement.rcNormalPosition.left   = rect.left;
         windowPlacement.rcNormalPosition.top    = rect.top;
         windowPlacement.rcNormalPosition.right  = rect.right;
         windowPlacement.rcNormalPosition.bottom = rect.bottom;

         unsigned int showCommand = SW_SHOWNORMAL;

			if (getApp().ViewMode > -1)
			{
				windowPlacement.showCmd = getApp().ViewMode;
			}
         else if (param.GetSize() >= 5)
         {
            showCommand = atoi(param[4]);

            if (showCommand != SW_SHOWMAXIMIZED)
            {
               showCommand = SW_SHOWNORMAL;
            }

            windowPlacement.showCmd = showCommand;
         }
         else if (windowPlacement.showCmd == SW_SHOWMAXIMIZED)
         {
            windowPlacement.showCmd = SW_SHOWNORMAL;
         }

         windowPlacement.flags &= ~WPF_RESTORETOMAXIMIZED;

         success = SetWindowPlacement(&windowPlacement);
      }
   }
   else 
   {
      setDefaultWindowSize();
   }

	/* Case 1027 - TLS
	// Moved LoadToolbars from the last line of OnCreate to here.
	// This location allows for the normal window size to be
	// set first, and then loads the toolbars to allow for
	// the proper placement of the toolbars even when maximized.
	*/
	LoadToolbars(toolbarFile);
}

// 20040706 - knv
// MFC code bug
// This code was copied from 
// c:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\atlmfc\src\mfc\winfrm.cpp
// Related article: http://support.microsoft.com/default.aspx?scid=kb;en-us;280467#appliesto
LRESULT CMainFrame::OnDDEExecute(WPARAM wParam, LPARAM lParam)
{
	// unpack the DDE message
   UINT_PTR unused;
	HGLOBAL hData;
   //IA64: Assume DDE LPARAMs are still 32-bit
	VERIFY(UnpackDDElParam(WM_DDE_EXECUTE, lParam, &unused, (UINT_PTR*)&hData));

	// get the command string
	TCHAR szCommand[_MAX_PATH * 2];
	LPCTSTR lpsz = (LPCTSTR)GlobalLock(hData);
	int commandLength = lstrlen(lpsz);
	if (commandLength >= _countof(szCommand))
	{
		// The command would be truncated. This could be a security problem
		TRACE0("Warning: Command was ignored because it was too long.\n");
		return 0;
	}

   // knv next line from
   // c:\Program Files\Microsoft Visual Studio .NET\Vc7\atlmfc\src\mfc\winfrm.cpp
	lstrcpyn(szCommand, lpsz, _countof(szCommand)); 

	GlobalUnlock(hData);

	// acknowledge now - before attempting to execute
	::PostMessage((HWND)wParam, WM_DDE_ACK, (WPARAM)m_hWnd,
	  //IA64: Assume DDE LPARAMs are still 32-bit
		ReuseDDElParam(lParam, WM_DDE_EXECUTE, WM_DDE_ACK,
		(UINT)0x8000, (UINT_PTR)hData));

	// don't execute the command when the window is disabled
	if (!IsWindowEnabled())
	{
		TRACE(traceAppMsg, 0, _T("Warning: DDE command '%s' ignored because window is disabled.\n"),
			szCommand);
		return 0;
	}

	// execute the command
	if (!AfxGetApp()->OnDDECommand(szCommand))
		TRACE(traceAppMsg, 0, _T("Error: failed to execute DDE command '%s'.\n"), szCommand);

	return 0L;
}

/******************************************************************************
* CToolBarVisibilityState
*/
CToolBarVisibilityState::CToolBarVisibilityState()
{
	m_hMenu = NULL;
   m_bCurrentSettings = false;
	m_bWorldView = false;
	m_bNavigator = false;
	m_bGerberEducatorDialogBar = false;
	m_bGerberEducatorToolBar = false;
   m_bToolBar = false;
   m_bAddToolBar = false;
   m_bSnapToolBar = false;
   m_bEditToolBar = false;
   m_bQueryToolBar = false;
   m_bGeomToolBar = false;
   m_bPolyToolBar = false;
   m_bGC_ToolBar = false;
   m_bIT_ToolBar = false;
   m_bPcbToolBar = false;
   m_bOGP_ToolBar = false;
   m_bRedlineToolBar = false;
   m_bJoinToolBar = false;
   m_bSequenceToolBar = false;
   m_bViewToolBar = false;
   m_bFilterToolBar = false;
   m_bTestAccessToolBar = false;
   m_bSchematicToolBar = false;
   m_bDFTToolBar = false;
	m_bToolsToolBar = false;
   m_bCustomToolBar = false;
   m_bCustom2ToolBar = false;
}

CToolBarVisibilityState::CToolBarVisibilityState(CToolBarVisibilityState &toolBarVisibility)
{
	if (&toolBarVisibility != this)
	{
		m_hMenu = toolBarVisibility.m_hMenu;
		m_bCurrentSettings = toolBarVisibility.m_bCurrentSettings;
		m_bWorldView = toolBarVisibility.m_bWorldView;
		m_bNavigator = toolBarVisibility.m_bNavigator;
		m_bGerberEducatorDialogBar = toolBarVisibility.m_bGerberEducatorDialogBar;
		m_bToolBar = toolBarVisibility.m_bToolBar;
		m_bAddToolBar = toolBarVisibility.m_bAddToolBar;
		m_bSnapToolBar = toolBarVisibility.m_bSnapToolBar;
		m_bEditToolBar = toolBarVisibility.m_bEditToolBar;
		m_bQueryToolBar = toolBarVisibility.m_bQueryToolBar;
		m_bGeomToolBar = toolBarVisibility.m_bGeomToolBar;
		m_bPolyToolBar = toolBarVisibility.m_bPolyToolBar;
		m_bGC_ToolBar = toolBarVisibility.m_bGC_ToolBar;
		m_bIT_ToolBar = toolBarVisibility.m_bIT_ToolBar;
		m_bPcbToolBar = toolBarVisibility.m_bPcbToolBar;
		m_bOGP_ToolBar = toolBarVisibility.m_bOGP_ToolBar;
		m_bRedlineToolBar = toolBarVisibility.m_bRedlineToolBar;
		m_bJoinToolBar = toolBarVisibility.m_bJoinToolBar;
		m_bSequenceToolBar = toolBarVisibility.m_bSequenceToolBar;
		m_bViewToolBar = toolBarVisibility.m_bViewToolBar;
		m_bFilterToolBar = toolBarVisibility.m_bFilterToolBar;
		m_bTestAccessToolBar = toolBarVisibility.m_bTestAccessToolBar;
		m_bSchematicToolBar = toolBarVisibility.m_bSchematicToolBar;
		m_bDFTToolBar = toolBarVisibility.m_bDFTToolBar;
		m_bToolsToolBar = toolBarVisibility.m_bToolsToolBar;
		m_bCustomToolBar = toolBarVisibility.m_bCustomToolBar;
		m_bCustom2ToolBar = toolBarVisibility.m_bCustom2ToolBar;
	}
}

CToolBarVisibilityState::~CToolBarVisibilityState()
{
	m_hMenu = NULL;
   m_bCurrentSettings = false;
	m_bWorldView = false;
	m_bNavigator = false;
	m_bGerberEducatorDialogBar = false;
	m_bGerberEducatorToolBar = false;
   m_bToolBar = false;
   m_bAddToolBar = false;
   m_bSnapToolBar = false;
   m_bEditToolBar = false;
   m_bQueryToolBar = false;
   m_bGeomToolBar = false;
   m_bPolyToolBar = false;
   m_bGC_ToolBar = false;
   m_bIT_ToolBar = false;
   m_bPcbToolBar = false;
   m_bOGP_ToolBar = false;
   m_bRedlineToolBar = false;
   m_bJoinToolBar = false;
   m_bSequenceToolBar = false;
   m_bViewToolBar = false;
   m_bFilterToolBar = false;
   m_bTestAccessToolBar = false;
   m_bSchematicToolBar = false;
   m_bDFTToolBar = false;
	m_bToolsToolBar = false;
   m_bCustomToolBar = false;
   m_bCustom2ToolBar = false;
}

CToolBarVisibilityState CToolBarVisibilityState::operator=(CToolBarVisibilityState &toolBarVisibility)
{
	if (&toolBarVisibility != this)
	{
		m_hMenu = toolBarVisibility.m_hMenu;
		m_bCurrentSettings = toolBarVisibility.m_bCurrentSettings;
		m_bWorldView = toolBarVisibility.m_bWorldView;
		m_bNavigator = toolBarVisibility.m_bNavigator;
		m_bGerberEducatorDialogBar = toolBarVisibility.m_bGerberEducatorDialogBar;
		m_bGerberEducatorToolBar = toolBarVisibility.m_bGerberEducatorToolBar;
		m_bToolBar = toolBarVisibility.m_bToolBar;
		m_bAddToolBar = toolBarVisibility.m_bAddToolBar;
		m_bSnapToolBar = toolBarVisibility.m_bSnapToolBar;
		m_bEditToolBar = toolBarVisibility.m_bEditToolBar;
		m_bQueryToolBar = toolBarVisibility.m_bQueryToolBar;
		m_bGeomToolBar = toolBarVisibility.m_bGeomToolBar;
		m_bPolyToolBar = toolBarVisibility.m_bPolyToolBar;
		m_bGC_ToolBar = toolBarVisibility.m_bGC_ToolBar;
		m_bIT_ToolBar = toolBarVisibility.m_bIT_ToolBar;
		m_bPcbToolBar = toolBarVisibility.m_bPcbToolBar;
		m_bOGP_ToolBar = toolBarVisibility.m_bOGP_ToolBar;
		m_bRedlineToolBar = toolBarVisibility.m_bRedlineToolBar;
		m_bJoinToolBar = toolBarVisibility.m_bJoinToolBar;
		m_bSequenceToolBar = toolBarVisibility.m_bSequenceToolBar;
		m_bViewToolBar = toolBarVisibility.m_bViewToolBar;
		m_bFilterToolBar = toolBarVisibility.m_bFilterToolBar;
		m_bTestAccessToolBar = toolBarVisibility.m_bTestAccessToolBar;
		m_bSchematicToolBar = toolBarVisibility.m_bSchematicToolBar;
		m_bDFTToolBar = toolBarVisibility.m_bDFTToolBar;
		m_bToolsToolBar = toolBarVisibility.m_bToolsToolBar;
		m_bCustomToolBar = toolBarVisibility.m_bCustomToolBar;
		m_bCustom2ToolBar = toolBarVisibility.m_bCustom2ToolBar;
	}

	return *this;
}



// YesNoNoToAllCancelDialog dialog

IMPLEMENT_DYNAMIC(CYesNoNoToAllCancelDialog, CDialog)
CYesNoNoToAllCancelDialog::CYesNoNoToAllCancelDialog(CString message, CWnd* pParent /*=NULL*/)
	: CDialog(CYesNoNoToAllCancelDialog::IDD, pParent)
   , m_message(_T(message))
{
}

CYesNoNoToAllCancelDialog::~CYesNoNoToAllCancelDialog()
{
}

void CYesNoNoToAllCancelDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_MESSAGE, m_message);
}


BEGIN_MESSAGE_MAP(CYesNoNoToAllCancelDialog, CDialog)
   ON_BN_CLICKED(IDYES, OnBnClickedYes)
   ON_BN_CLICKED(IDNO, OnBnClickedNo)
   ON_BN_CLICKED(IDNOTOALL, OnBnClickedNotoall)
   ON_STN_CLICKED(IDC_PICTURE, OnStnClickedPicture)
END_MESSAGE_MAP()


// YesNoNoToAllCancelDialog message handlers

void CYesNoNoToAllCancelDialog::OnBnClickedYes()
{
   EndDialog(IDYES);
}

void CYesNoNoToAllCancelDialog::OnBnClickedNo()
{
   EndDialog(IDNO);
}

void CYesNoNoToAllCancelDialog::OnBnClickedNotoall()
{
   EndDialog(IDNOTOALL);
}

void CYesNoNoToAllCancelDialog::OnStnClickedPicture()
{
   // TODO: Add your control notification handler code here
}
