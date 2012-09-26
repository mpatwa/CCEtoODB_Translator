// $Header: /CAMCAD/4.6/MainFrm.h 52    5/16/07 3:10p Rick Faltersack $

// mainfrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "cedit.h"
#include "measure.h"
#include "ToolBarC.h"
#include "CAMCADNavigator.h"

/******************************************************************************
* CToolBarVisibilityState
*/
class CToolBarVisibilityState
{
public:
   CToolBarVisibilityState();
   CToolBarVisibilityState(CToolBarVisibilityState &toolBarVisibility);
   ~CToolBarVisibilityState();
   CToolBarVisibilityState operator=(CToolBarVisibilityState &toolBarVisibility);

   HMENU m_hMenu;
   bool m_bCurrentSettings;
   bool m_bWorldView;
   bool m_bNavigator;
   bool m_bGerberEducatorDialogBar;
   bool m_bGerberEducatorToolBar;
   bool m_bToolBar;
   bool m_bAddToolBar;
   bool m_bSnapToolBar;
   bool m_bEditToolBar;
   bool m_bQueryToolBar;
   bool m_bGeomToolBar;
   bool m_bPolyToolBar;
   bool m_bGC_ToolBar;
   bool m_bIT_ToolBar;
   bool m_bPcbToolBar;
   bool m_bOGP_ToolBar;
   bool m_bRedlineToolBar;
   bool m_bJoinToolBar;
   bool m_bSequenceToolBar;
   bool m_bViewToolBar;
   bool m_bFilterToolBar;
   bool m_bTestAccessToolBar;
   bool m_bSchematicToolBar;
   bool m_bDFTToolBar;
   bool m_bToolsToolBar;
   bool m_bCustomToolBar;
   bool m_bCustom2ToolBar;
};

class CGerberEducatorToolbar;

/******************************************************************************
* CMainFrame 
*/
class CMainFrame : public CMDIFrameWnd
{
   DECLARE_DYNAMIC(CMainFrame)

public:
   CMainFrame();

// Attributes
public:

private:
   WINDOWPLACEMENT lastWindowPlacement;
   CAMCADNavigator m_navigator;
   CDockState* m_dockState;
   bool m_menuItemsEnabled;
   bool m_exitWithoutSave;

// Operations
public:
   void LoadMenuText(CString file);
   void SetFrameTitle(const char *title) { m_strTitle = title; };
   void LoadToolbars(const char *filename);
   void SaveToolbars(const char *filename);
   void OnToolbarDropDown(NMHDR* pnmhdr, LRESULT *plr);

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CMainFrame)
   protected:
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CMainFrame();
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif


public:  // control bar embedded members
   BOOL        m_Activated;
   CStatusBar  m_wndStatusBar;
   CSDialogBar m_wndCurrentSettings;

   CMyToolBar m_wndToolBar;
   CMyToolBar m_AddToolBar;
   CMyToolBar m_SnapToolBar;
   CMyToolBar m_EditToolBar;
   CMyToolBar m_QueryToolBar;
   CMyToolBar m_GerberEducatorToolBar;
   CMyToolBar m_GeomToolBar;
   CMyToolBar m_PolyToolBar;
   CMyToolBar m_GC_ToolBar;
   CMyToolBar m_IT_ToolBar;
   CMyToolBar m_PcbToolBar;
   CMyToolBar m_OGP_ToolBar;
   CMyToolBar m_RedlineToolBar;
   CMyToolBar m_JoinToolBar;
   CMyToolBar m_SequenceToolBar;
   CMyToolBar m_ViewToolBar;
   CMyToolBar m_FilterToolBar;
   CMyToolBar m_TestAccessToolBar;
   CMyToolBar m_SchematicToolBar;
   CMyToolBar m_DFTToolBar;
   CMyToolBar m_ToolsToolBar;
   CCustomToolBar m_CustomToolBar;
   CCustomToolBar m_Custom2ToolBar;

// Generated message map functions
protected:
   //{{AFX_MSG(CMainFrame)
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnClose();
   afx_msg void OnSaveMenuText();
   afx_msg void OnLoadMenuText();
   afx_msg void OnSaveToolbarFile();
   afx_msg void OnLoadToolbarFile();
   afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
   afx_msg void OnTaPlaceOffsets();
   afx_msg void OnInitMenu(CMenu* pMenu);
   afx_msg void OnHelp();
   afx_msg void OnContextHelp();
   afx_msg void OnDropFiles(HDROP hDropInfo);
   //}}AFX_MSG

   afx_msg LRESULT OnDDEExecute(WPARAM wParam, LPARAM lParam);  // for MFC bug fix

   DECLARE_MESSAGE_MAP()

public:
   void setDefaultWindowSize();
   CRect getSavedMonitorRect();
   void saveWindowState();
   void restoreWindowState();
   void NavCallClear(){m_navigator.OnBnClickedButtonClear();};
   void NavCallFind(){m_navigator.OnBnClickedButtonFind();};
   void NavCallOptions(){m_navigator.OnBnClickedBtnOptions();};
   void PCBNavCallVariantChange(){m_navigator.OnBnClickedButtonVariantChange();}
   void PCBNavCallMachineChange(){m_navigator.OnBnClickedButtonMachineChange();}

   CAMCADNavigator& getNavigator();
   CStatusBar& getStatusBar() { return m_wndStatusBar; }

   //void PCBNavCallVarApply(){m_navigator.OnBnClickedButtonVarApply();}
   //void PCBNavCallMachineApply(){m_navigator.OnBnClickedButtonMachineApply();}

   void OnUpdateNavCallFind(CCmdUI *pCmdUI);
   void OnUpdateNavCallClear(CCmdUI *pCmdUI);
   void OnUpdateNavCallOptions(CCmdUI *pCmdUI);
   void OnUpdatePCBNavVariantButtons(CCmdUI *pCmdUI);
   void OnUpdatePCBNavMachineButtons(CCmdUI *pCmdUI);

   void frmOnCmdDeleteProbe()  { m_navigator.OnCmdDeleteProbe();  }
   void frmOnCmdUnplaceProbe() { m_navigator.OnCmdUnplaceProbe(); }
   //void frmOnCreateProbe()  { m_navigator.OnCreateProbe();  }
   void frmOnEditProbe()    { m_navigator.OnEditProbe();    }
   void frmOnHighlightNet() { m_navigator.OnHighlightNet(); }
   
   CToolBarVisibilityState GetToolBarVisiblity();
   void SetToolBarVisiblity(CToolBarVisibilityState toolBarState);
   void enableToolBars(bool enableFlag = true);
   void disableToolBars();
   void enableMenuItems(bool enableFlag = true);
   void disableMenuItems();
   void displayToolBars(bool displayFlag = true);
   void maximizeActiveChildFrame();
   void normalizeToMaximumActiveChildFrame();
   void updateMenuState();
   void enableExitWithoutSave(const bool enable) { m_exitWithoutSave = enable; }

   // Overrides
   virtual HMENU GetWindowMenuPopup(HMENU hMenuBar);

   afx_msg void OnUpdateToolsSchematiclink(CCmdUI *pCmdUI);
   afx_msg void OnUpdateGerberEducator(CCmdUI *pCmdUI);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnMove(int x, int y);
};

CMainFrame* getMainFrame();

/////////////////////////////////////////////////////////////////////////////
#pragma once


// YesNoNoToAllCancelDialog dialog

class CYesNoNoToAllCancelDialog : public CDialog
{
	DECLARE_DYNAMIC(CYesNoNoToAllCancelDialog)

public:
	CYesNoNoToAllCancelDialog(CString message, CWnd* pParent = NULL);   // standard constructor
	virtual ~CYesNoNoToAllCancelDialog();

// Dialog Data
	enum { IDD = IDD_Yes_No_NoToAll_Cancel };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
   CString m_message;
public:
   afx_msg void OnBnClickedYes();
   afx_msg void OnBnClickedNo();
   afx_msg void OnBnClickedNotoall();
   afx_msg void OnStnClickedPicture();
};
