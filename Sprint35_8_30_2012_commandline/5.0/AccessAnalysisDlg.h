// $Header: /CAMCAD/4.5/AccessAnalysisDlg.h 61    1/12/07 3:51p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#if !defined(__AccessAnalysis_h__)
#define __AccessAnalysis_h__

#pragma once

#include "CCEtoODB.h"
#include "GeneralDialog.h"
#include "ccdoc.h"
#include "DFT.h"
#include "afxwin.h"
#include "RwUiLib.h"
#include "UltimateGrid.h"

// To get CViewLiteToolbar, the various vcproj files are not available at the moment.
// At some point, the CViewLiteToolbar ought to be moved to its own files.
// Not really a good fit for ToolbarC.cpp/h or ToolbarM.cpp/h.
#include "DataDoctorDialog.h"

#define ID_GRIDROWEDITFINISHED         (WM_APP + 100)


/* Define Section *********************************************************/

#define COLOR_BLACK							0x80000008
#define COLOR_WHITE							0x80000005
#define COLOR_GRAY							0x8000000F
#define COLOR_IVORY							0x80000018
#define COLOR_ORANGE							0x0080CDFF
#define COLOR_BLUE							0x00F5C080

// These macros are use for result display grid 
#define COL_NET_SURFACE				     -1
#define COL_ALL_TOT			            0
#define COL_ALL_ACC							1
#define COL_MUL_PIN_TOT						2
#define COL_MUL_PIN_ACC						3
#define COL_SNG_PIN_TOT						4
#define COL_SNG_PIN_ACC						5
#define COL_NC_PIN_TOT						6
#define COL_NC_PIN_ACC						7

#define ROW_NET_SURFACE_0					-1
#define ROW_NET_SURFACE_1					-1
#define ROW_HEADING_1                  -1
#define ROW_TOP								0
#define ROW_BOTTOM							1
#define ROW_BOTH								2
#define ROW_TOTAL 							3


// These macros are use for Height Analysis tab
#define COL_HEIGHT_NAME						(long)0		// Name of height
#define COL_HEIGHT_DISTANCE				(long)1		// Package Outline Distance
#define COL_HEIGHT_MIN						(long)2		// Minimum Height
#define COL_HEIGHT_MAX						(long)3		// Maximum Height

#define HAS_OUTLINE_YES						0
#define HAS_OUTLINE_NO						1
#define HAS_OUTLINE_BOTH					2

/* Class Section *********************************************************/
class CAccessAnalysisDlg;

//////////////////////////////////////////////////////////////////////////////////
// Ultimate Grid based AA Results Grid
//

class CAAResultsGrid : public CDDBaseGrid
{
private:
   COLORREF m_colorBlue;
   COLORREF m_colorOrange;
   COLORREF m_colorGray;

public:
    CAAResultsGrid();
    ~CAAResultsGrid() { }

    COLORREF GetColorBlue()         { return m_colorBlue; }
    COLORREF GetColorOrange()       { return m_colorOrange; }
    COLORREF GetColorGray()         { return m_colorGray; }

    virtual void OnSetup();
    virtual int  OnCanSizeCol(int col) { return false; }
    virtual int  OnCanSizeTopHdg()     { return false; }

    void SizeToFit(CRect &rect);
};

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// Ultimate Grid based AA Results Grid
//

class CHeightAnalysisGrid : public CDDBaseGrid
{
private:
   COLORREF m_colorBlue;
   COLORREF m_colorOrange;
   COLORREF m_colorGray;
   CHeightAnalysis* m_heightAnalysis;

public:
    CHeightAnalysisGrid();
    ~CHeightAnalysisGrid() { }

    void SetHeightAnalysis(CHeightAnalysis* ha) { m_heightAnalysis = ha; }

    COLORREF GetColorBlue()         { return m_colorBlue; }
    COLORREF GetColorOrange()       { return m_colorOrange; }
    COLORREF GetColorGray()         { return m_colorGray; }

    virtual void OnSetup();
    virtual int  OnCanSizeCol(int col) { return false; }
    virtual int  OnCanSizeTopHdg()     { return false; }

    virtual void OnMenuCommand(int col,long row,int section,int item);
    virtual int  OnMenuStart(int col,long row,int section);

    void SizeToFit(CRect &rect);

    int OnEditStart(int col, long row, CWnd **edit);
    int OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey);
    int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);

    void SetRow(int row, CString name, CString distance, CString min, CString max, CHeightRange* heightRange);
    void UpdateHeightRangeFromGridSettings(int row);

};

//////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// CHeightAnalysis dialog
////////////////////////////////////////////////////////////////////////////////
class CHeightAnalysisDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(CHeightAnalysisDlg)

public:
	CHeightAnalysisDlg(CCEtoODBDoc *doc);
	virtual ~CHeightAnalysisDlg();
	enum { IDD = IDD_AA_HEIGHT_ANALYSIS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

private:
	CWnd *m_pParent;
	CCEtoODBDoc *m_pDoc;
	CTestPlan *m_pTestPlan;
	bool m_bAlreadyOnInit;

   int m_decimals;

	CButton m_chkUseTopHeightAnalysis;
	CButton m_chkUseBotHeightAnalysis;

   CHeightAnalysisGrid m_topHeightGrid;
   CHeightAnalysisGrid m_botHeightGrid;

	CExtendedFlexGrid* m_pActiveGrid;
	CPoint m_mouseDownPoint;

	void loadGrid(CHeightAnalysisGrid *uggrid, CHeightAnalysis* heightAnalysis);
   void enableGrid(CHeightAnalysisGrid* grid, bool enable);
	void dblClickGrid(CExtendedFlexGrid* grid);
	void escapeKeyDown(long row, long col, CExtendedFlexGrid* grid);
	bool validateEdit(long row, long col, CExtendedFlexGrid* grid);

public:
	virtual BOOL OnInitDialog();
	void SetParent(CWnd* pParent);
	void SetTestPlanPointer(CTestPlan *testPlan);
	void LoadTestPlanSettings();

	afx_msg BOOL OnSetActive();
	afx_msg void OnBnClickedChkTopHeightAnalysis();
	afx_msg void OnBnClickedChkBotHeightAnalysis();
	afx_msg void OnDeleteSelected();
   afx_msg LRESULT OnRowEditFinished(WPARAM wParamROW, LPARAM lParamCOL);

	DECLARE_EVENTSINK_MAP()
private:
};


////////////////////////////////////////////////////////////////////////////////
// CPhysicalConstraints dialog
////////////////////////////////////////////////////////////////////////////////
class CPhysicalConstraints : public CPropertyPage
{
	DECLARE_DYNAMIC(CPhysicalConstraints)

public:
	CPhysicalConstraints(CCEtoODBDoc *doc);   // standard constructor
	virtual ~CPhysicalConstraints();
	enum { IDD = IDD_AA_PHYSICAL_CONSTRAINTS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

private:
	CWnd *m_pParent;
	CCEtoODBDoc *m_pDoc;
	CTestPlan *m_pTestPlan;
	CButton m_chkSoldermaskAnalysis;
	BOOL m_chkUseRealPartOutline;  // Not a separate user control anymore
	BOOL m_chkUseDFTOutline;       // Not a separate user control anymore
   BOOL m_chkUsePkgOutline;       // ** This controls both of the above **
	BOOL m_optOutlinePriority;     // Not controllable by user anymore
	CButton m_chkIgnore1PinCompOutline;
	CButton m_chkIgnoreUnloadedCompOutline;
	bool alreadyOnInit;
	bool m_bEnableSoldermaskCheckbox;
	bool m_bEnableRealPartCheckbox;

   int m_decimals;

	// Top constraints controls
	CStatic m_topConstraints;
	CButton m_chkTopBoardDist;
	CButton m_chkTopCompDist;
	CButton m_chkTopFeatureSize;
   CButton m_chkTopBeadProbeSize;
	CEdit m_txtTopBoardDist;
	CEdit m_txtTopCompDist;
	CEdit m_txtTopFeatureSize;
   CEdit m_txtTopBeadProbeSize;
	CString m_topBoardNetLost;
	CString m_topBoardFtrLost;
	CString m_topCompNetLost;
	CString m_topCompFtrLost;
	CString m_topSizeNetLost;
	CString m_topSizeFtrLost;
   CString m_topBeadProbeSizeNetLost;
	CString m_topBeadProbeSizeFtrLost;

	// Bottom constraints controls
	CStatic m_botConstraints;
	CButton m_chkBotBoardDist;
	CButton m_chkBotCompDist;
	CButton m_chkBotFeatureSize;
   CButton m_chkBotBeadProbeSize;
	CEdit m_txtBotBoardDist;
	CEdit m_txtBotCompDist;
	CEdit m_txtBotFeatureSize;
   CEdit m_txtBotBeadProbeSize;
	CString m_botBoardNetLost;
	CString m_botBoardFtrLost;
	CString m_botCompNetLost;
	CString m_botCompFtrLost;
	CString m_botSizeNetLost;
	CString m_botSizeFtrLost;
   CString m_botBeadProbeSizeNetLost;
	CString m_botBeadProbeSizeFtrLost;

	void LoadDefaultSettings();
	void EnableTopConstraints(BOOL enable);
	void EnableBottomConstraints(BOOL enable);
	void EnableOutlinePriority(bool enable);

public:
	virtual BOOL OnInitDialog();
	void SetParent(CWnd* pParent);
	void SetTestPlanPointer(CTestPlan *testPlan);
	void LoadTestPlanSettings();
	void UpdateFeatureLost(CAAFeatureLost topFeatureLost, CAAFeatureLost botFeatureLost);
	void EnableSoldermaskAnalysisCheckbox(bool enableCheckbox);
	void EnableRealPartCheckbox(bool enableCheckbox);

	afx_msg void OnBnClickedCheckSoldermaskAnalysis();
	afx_msg void OnBnClickedCheckUseOutline();
	afx_msg void OnBnClickedRadioUseRealpart();

	afx_msg void OnBnClickedCheckTopBoardDist();
	afx_msg void OnBnClickedCheckTopCompDist();
	afx_msg void OnBnClickedCheckTopFeatureSize();
   afx_msg void OnBnClickedCheckTopBeadProbeSize();
	afx_msg void OnBnClickedCheckBotBoardDist();
	afx_msg void OnBnClickedCheckBotCompDist();
	afx_msg void OnBnClickedCheckBotFeatureSize();
   afx_msg void OnBnClickedCheckBotBeadProbeSize();
	
	afx_msg void OnKillFocusEditTopBoardDist();
	afx_msg void OnKillFocusEditTopCompDist();
	afx_msg void OnKillFocusEditTopFeatureSize();
   afx_msg void OnKillFocusEditTopBeadProbeSize();
	afx_msg void OnKillFocusEditBotBoardDist();
	afx_msg void OnKillFocusEditBotCompDist();
	afx_msg void OnKillFocusEditBotFeatureSize();
   afx_msg void OnKillFocusEditBotBeadProbeSize();
	
	afx_msg void OnEnChangeEditTopBoardDist();
	afx_msg void OnEnChangeEditTopCompDist();
	afx_msg void OnEnChangeEditTopFeatureSize();
   afx_msg void OnEnChangeEditTopBeadProbeSize();
	afx_msg void OnEnChangeEditBotBoardDist();
	afx_msg void OnEnChangeEditBotCompDist();
	afx_msg void OnEnChangeEditBotFeatureSize();
   afx_msg void OnEnChangeEditBotBeadProbeSize();
	
	afx_msg BOOL OnSetActive();
	afx_msg void OnBnClickedCheckIgnore1pinCompoutline();
	afx_msg void OnBnClickedCheckIgnoreUnloadedCompoutline();
};


////////////////////////////////////////////////////////////////////////////////
// CTargetTypeDlg dialog
////////////////////////////////////////////////////////////////////////////////
class CTargetTypeDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(CTargetTypeDlg)

public:
	CTargetTypeDlg(CCEtoODBDoc *doc);   // standard constructor
	virtual ~CTargetTypeDlg();
	enum { IDD = IDD_AA_TARGET_TYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

private:
	CCEtoODBDoc *m_pDoc;
	CWnd *m_pParent;
	CTestPlan *m_pTestPlan;
	bool alreadyOnInit;

	// Top target type controls
	CStatic m_staticTopTargetTypes;
	CButton m_chkTopTestAttrib;
	CButton m_chkTopVia;
	CButton m_chkTopConnector;
	CButton m_chkTopSMD;
   CButton m_chkTopSinglePinSMD;
	CButton m_chkTopTHUR;
	CButton m_chkTopPadstack;
	CString m_topTestAttribNetLost;
	CString m_topTestAttribFtrLost;
	CString m_topViaNetLost;
	CString m_topViaFtrLost;
	CString m_topConNetLost;
	CString m_topConFtrLost;
	CString m_topSMDNetLost;
	CString m_topSMDFtrLost;
	CString m_topSinglePinSMDNetLost;
	CString m_topSinglePinSMDFtrLost;
	CString m_topTHURNetLost;
	CString m_topTHURFtrLost;
	CString m_topPadstackNetLost;
	CString m_topPadstackFtrLost;
	CListBox m_lstTopPadstack;
	CButton m_btnTopEditPadstack;

	// Bottom target type controls
	CStatic m_staticBotTargetTypes;
	CButton m_chkBotTestAttrib;
	CButton m_chkBotVia;
	CButton m_chkBotConnector;
	CButton m_chkBotSMD;
   CButton m_chkBotSinglePinSMD;
	CButton m_chkBotTHUR;
	CButton m_chkBotPadstack;
	CString m_botTestAttribNetLost;
	CString m_botTestAttribFtrLost;
	CString m_botViaNetLost;
	CString m_botViaFtrLost;
	CString m_botConNetLost;
	CString m_botConFtrLost;
	CString m_botSMDNetLost;
	CString m_botSMDFtrLost;
	CString m_botSinglePinSMDNetLost;
	CString m_botSinglePinSMDFtrLost;
	CString m_botTHURNetLost;
	CString m_botTHURFtrLost;
	CString m_botPadstackNetLost;
	CString m_botPadstackFtrLost;
	CListBox m_lstBotPadstack;
	CButton m_btnBotEditPadstack;

	void LoadDefaultSettings();
	void EnableTopTargetTypes(BOOL enable);
	void EnableBottomTargetTypes(BOOL enable);
	void EditPadstackList(CListBox *listbox, CString selectDialogCaption);

public:
	virtual BOOL OnInitDialog();
	void SetParent(CWnd* pParent);
	void SetTestPlanPointer(CTestPlan *testPlan);
	void LoadTestPlanSettings();
	void UpdateFeatureLost(CAAFeatureLost topFeatureLost, CAAFeatureLost botFeatureLost);

	afx_msg void OnBnClickedCheckTopTestattrib();
	afx_msg void OnBnClickedCheckTopVia();
	afx_msg void OnBnClickedCheckTopConnector();
	afx_msg void OnBnClickedCheckTopSmd();
   afx_msg void OnBnClickedCheckTopSinglePinSmd();
	afx_msg void OnBnClickedCheckTopThur();
	afx_msg void OnBnClickedCheckTopPadstack();
	afx_msg void OnBnClickedButtonTopEdit();
	afx_msg void OnBnClickedCheckBotTestattrib();
	afx_msg void OnBnClickedCheckBotVia();
	afx_msg void OnBnClickedCheckBotConnector();
	afx_msg void OnBnClickedCheckBotSmd();
   afx_msg void OnBnClickedCheckBotSinglePinSmd();
	afx_msg void OnBnClickedCheckBotThur();
	afx_msg void OnBnClickedCheckBotPadstack();
	afx_msg void OnBnClickedButtonBotEdit();
	afx_msg BOOL OnSetActive();
};


////////////////////////////////////////////////////////////////////////////////
// COptionDlg dialog
////////////////////////////////////////////////////////////////////////////////
class COptionDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionDlg)

public:
	COptionDlg(CAccessAnalysisDlg& parent,CCEtoODBDoc *doc);
	virtual ~COptionDlg();
	enum { IDD = IDD_AA_OPTION_NEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

private:
   CAccessAnalysisDlg& m_parent;
	//CWnd *m_pParent;
	CCEtoODBDoc *m_pDoc;
	CTestPlan *m_pTestPlan;
	BOOL m_optSurface;
	BOOL m_optTesterType;
	BOOL m_chkMultiplePinNet;
	BOOL m_chkSinglPinNet;
	BOOL m_chkUnconnectedNet;
   BOOL m_chkRetainExistingProbe;
   BOOL m_chkMultipleReasonCodes;
	CButton m_chkCreateUnconnectedNet;
	CButton m_chkAccessOffset;
	CString m_txtDirectory;
	CListBox m_lstTestPlans;
	bool alreadyOnInit;
	BOOL m_chkWriteIgnoreSurface;
   CAccessOffsetOptions m_accessOffsetOptions;

   CButton m_smdPinCheckBox;
   CButton m_thPinCheckBox;
   CEdit m_thPinEditBox;
   CButton m_smdViaCheckBox;
   CButton m_thViaCheckBox;
   CEdit m_thViaEditBox;
   CButton m_smdTpCheckBox;
   CButton m_thTpCheckBox;
   CEdit m_thTpEditBox;
   CComboBox m_smdPinComboBox;

	void LoadDefaultSettings();
	void FillTestPlanListbox(CString directoryPath);
	void WriteDirectoryToRegistry(CString directoryPath);
	CString GetDirectoryFromRegistry();

public:
   CAccessOffsetOptions& getAccessOffsetOptions();
	virtual BOOL OnInitDialog();
	//void SetParent(CWnd* pParent);
	void SetTestPlanPointer(CTestPlan *testPlan);
	void LoadTestPlanSettings();

	afx_msg void OnBnClickedRadioSurfaceTop();
	afx_msg void OnBnClickedRadioTesterFixture();
	afx_msg void OnBnClickedCheckMultiplePinNet();
	afx_msg void OnBnClickedCheckSinglePinNet();
	afx_msg void OnBnClickedCheckUnconnectedNet();
	afx_msg void OnBnClickedButtonBrowse();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonSaveas();
	afx_msg void OnBnClickedButtonSetasdefault();
	afx_msg void OnBnClickedButtonRestoredefault();
	afx_msg void OnBnClickedCheckAccessOffset();
	//afx_msg void OnBnClickedCheckViaOffset();
	//afx_msg void OnBnClickedCheckSmdOffset();
	//afx_msg void OnBnClickedCheckThruOffset();
	afx_msg void OnBnClickedCheckWriteIgnoredSurface();
	afx_msg void OnBnClickedCheckCreateUnconnectedNet();
	afx_msg void OnLbnSelchangeListTestPlans();
   afx_msg void OnBnClickedThPinCheckBox();
   afx_msg void OnBnClickedThViaCheckBox();
   afx_msg void OnBnClickedThTpCheckBox();
   afx_msg void OnBnClickedSmdPinCheckBox();
   afx_msg void OnBnClickedSmdViaCheckBox();
   afx_msg void OnBnClickedSmdTpCheckBox();
   afx_msg void OnSetFocusThPinEditBox();
   afx_msg void OnSetFocusThViaEditBox();
   afx_msg void OnSetFocusThTpEditBox();
   afx_msg void OnSelChangeSmdPinComboBox();
   afx_msg void OnBnClickedCheckRetainexistingProbes();
   afx_msg void OnBnClickedMultipleReasonCodes();
};


////////////////////////////////////////////////////////////////////////////////
// CAccessAnalysisDlg dialog
////////////////////////////////////////////////////////////////////////////////
class CAccessAnalysisDlg : public CDialog
{
	DECLARE_DYNAMIC(CAccessAnalysisDlg)

public:
	CAccessAnalysisDlg(CCEtoODBDoc& camCadDoc, FileStruct& pPCBFile, CWnd* pParent = NULL);   // standard constructor
	virtual ~CAccessAnalysisDlg();
	enum { IDD = IDD_AA_ACCESS_ANALYSIS_NEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	DECLARE_EVENTSINK_MAP()

private:
	CCEtoODBDoc& m_camCadDoc;
	FileStruct& m_pPCBFile;
	CDFTSolution m_DFTSolution;
   CAAResultsGrid m_aaResultsGrid;
	CExposedDataMap m_ExposedMetalMap;
   CMetalAnalysis m_metalAnalysis;
	bool m_createUnconnectedNet;
	bool m_explodedMetalLayer;
	bool m_processedSMAnalysis;
	int m_error;

	// Controls members
	CPropertySheet m_propSheet;
	CHeightAnalysisDlg m_heightAnalysisDlg;
	CPhysicalConstraints m_physicalConstraintsDlg;
	CTargetTypeDlg m_targetTypesDlg;
	COptionDlg m_optionDlg;
	CButton m_btnOK;
	CButton m_btnProcess;
   CButton m_btnReport;

	bool CheckSoldermask();
	void CreateTabDialog();
	void UpdateCellResult(int row, int totCol, int totValue,int accCol, int accValue);
	void UpdateGridResult();
	void UpdateGridColor();
	void CreateTestibilityReport();
   void RetainProbes(CTypedMapStringToPtrContainer<DataStruct*>& retainedProbesMap);

public:
   CCEtoODBDoc& getCamCadDoc() { return m_camCadDoc; }
	virtual BOOL OnInitDialog();
	void UpdateChange(BOOL updateGridColor);
	void LoadTestPlan();

	int GetError() { return m_error; };

   CString GetReportFilename()   { return GetLogfilePath("AccessReport.txt"); }

	afx_msg void OnBnClickedProcess();
   afx_msg void OnBnClickedDisplayReport();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};


////////////////////////////////////////////////////////////////////////////////
// CPadstackItem Class
////////////////////////////////////////////////////////////////////////////////
class CPadstackItem : public CSelItem
{
public:
	CPadstackItem(CString name = "", BlockStruct *pPadstack = NULL, BOOL selected = FALSE);
	~CPadstackItem();

	BlockStruct *GetPadstack();
	void SetName(CString name);
	void SetPadstack(BlockStruct *pPadstack);

private:
	BlockStruct *m_pPadstack;
};

//_____________________________________________________________________________
class CAccessOffsetDirectionPriorityDialog : public CDialog
{
	DECLARE_DYNAMIC(CAccessOffsetDirectionPriorityDialog)

private:
   //CAccessOffsetItem m_accessOffsetItem;
   CPrioritizedAccessOffsetDirectionsList m_allowedList;
   CPrioritizedAccessOffsetDirectionsList m_disallowedList;

   // controls
   CButton m_moveUpButton;
   CButton m_moveDownButton;
   CButton m_allowButton;
   CButton m_disallowButton;
   CListBox m_allowedListBox;
   CListBox m_disallowedListBox;
   CEdit m_directionPriorityEditBox;

public:
	CAccessOffsetDirectionPriorityDialog(const CPrioritizedAccessOffsetDirectionsList& allowedList,CWnd* pParent = NULL);
	virtual ~CAccessOffsetDirectionPriorityDialog();

   CPrioritizedAccessOffsetDirectionsList getAllowedList() const { return m_allowedList; }

// Dialog Data
	enum { IDD = IDD_AA_AccessOffsetDirections };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
   void fillDirectionList(CListBox& listBox,const CPrioritizedAccessOffsetDirectionsList& directionList);
   int getFirstSelected(CListBox& listBox);
   void getSelectedFromList(CPrioritizedAccessOffsetDirectionsList& directionList,CListBox& listBox);
   CString getDisplayString(AccessOffsetDirectionTag accessOffsetDirection);

public:
   virtual BOOL OnInitDialog();

   afx_msg void OnKillFocusDirectionPriority();
   afx_msg void OnSelChangeAllowedList();
   afx_msg void OnSelChangeDisallowedList();
   afx_msg void OnBnClickedMoveUp();
   afx_msg void OnBnClickedMoveDown();
   afx_msg void OnBnClickedAllow();
   afx_msg void OnBnClickedDisallow();
};

#endif

