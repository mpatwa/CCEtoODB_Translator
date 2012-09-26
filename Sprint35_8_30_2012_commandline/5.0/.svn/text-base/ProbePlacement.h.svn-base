// $Header: /CAMCAD/4.6/ProbePlacement.h 60    2/14/07 4:05p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#if !defined(__ProbePlacement_h__)
#define __ProbePlacement_h__

#pragma once

#include "afxwin.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "extents.h"
#include "dft.h"
#include "RwUiLib.h"
#include "UltimateGrid.h"

//////////////////////////////////////////////////////////////////////////////////
// Ultimate Grid based PP Results Grid
//

class CPPResultsGrid : public CDDBaseGrid
{
private:
   int m_rowIndxTop;
   int m_rowIndxBottom;
   int m_rowIndxTotal;

   int m_colIndxPlaced;
   int m_colIndxUnplaced;

public:
    CPPResultsGrid();
    ~CPPResultsGrid() { }

    virtual void OnSetup();
    virtual int  OnCanSizeCol(int col) { return true; }
    virtual int  OnCanSizeTopHdg()     { return true; }
    virtual int  OnCanSizeSideHdg()    { return true; }

    void SizeToFit(CRect &rect);

    int RowIndxTop()        { return m_rowIndxTop; }
    int RowIndxBot()        { return m_rowIndxBottom; }
    int RowIndxTot()        { return m_rowIndxTotal; }

    int ColIndxPlaced()     { return m_colIndxPlaced; }
    int ColIndxUnplaced()   { return m_colIndxUnplaced; }

    // Grid size is not constant, number of cols varies with number of probes
    // in the solution. So can't do the grid setup in OnSetup like usual.
    // At point of displayying the results, these methods are used to define
    // the grid row and column headings. Do fixed columns once, and ProbeSize
    // columns one set for each probe size. Return val is index of TR
    // column, the PIR column is that plus one.
    int  DefineProbeSizeColumns(CString probeSizeName);
    void DefineFixedColumns();
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// Ultimate Grid based Probes Grid
//

class CProbesGrid : public CDDBaseGrid
{
private:
   int m_mostRecentRow;

public:
    CProbesGrid();
    ~CProbesGrid() { }

    virtual void OnSetup();
    virtual int  OnCanSizeCol(int col) { return false; }
    virtual int  OnCanSizeTopHdg()     { return false; }
    virtual int  OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);

    void SizeToFit(CRect &rect);

    void AddProbe(CDFTProbeTemplate *probeTemplate, int decimals, PageUnitsTag pageUnits);
    void SetProbe(int rowIndx, CDFTProbeTemplate *probeTemplate, int decimals, PageUnitsTag pageUnits);
    void AddBlankProbe();

    virtual void OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed);
    virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);

    void DeleteMostRecentRow();
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// Ultimate Grid based Preconditions Grid
//

class CPreconditionsGrid : public CDDBaseGrid
{
private:
   int m_expandedRowHeight;

public:
    CPreconditionsGrid();
    ~CPreconditionsGrid();

    virtual void OnSetup();
    virtual int  OnCanSizeCol(int col) { return false; }
    virtual int  OnCanSizeTopHdg()     { return false; }
    virtual int  OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);
    virtual int  OnSortEvaluate(CUGCell *cell1,CUGCell *cell2,int flags);
    
    virtual int  OnEditStart(int col, long row,CWnd **edit)  { return false; } // No editing of any cells
    virtual void OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed);

    void SizeToFit(CRect &rect);

    int GetRowIndxContaining(CString name);
    void AddFeatureToGrid(CDFTFeature *feature);
    void AddFeatureToGrid(CString compRefDes, CString deviceType, CString pinName);
    void SortAscending();

    void ExpandOneComponent(int componentNameRowIndx);
    void CollapseOneComponent(int componentNameRowIndx);
    void ResetRowHeights(int startRow);

    void SetExpandedRowHeight(int h)      { m_expandedRowHeight = h; }
    int GetExpandedRowHeight()            { return m_expandedRowHeight; }
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// Ultimate Grid based Net Conditions Grid
//

class CNetConditionsGrid : public CDDBaseGrid
{
private:
   int m_expandedRowHeight;

public:
    CNetConditionsGrid();
    ~CNetConditionsGrid() { }

    void SetExpandedRowHeight(int h)      { m_expandedRowHeight = h; }
    int GetExpandedRowHeight()            { return m_expandedRowHeight; }

    void ExpandLayerGroup(int groupNameRowIndx);
    void CollapseLayerGroup(int groupNameRowIndx);

    void SetRowCellsToCheckBox(int row);

    void OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed);

    virtual void OnSetup();
    virtual int  OnCanSizeCol(int col) { return true; }
    virtual int  OnCanSizeTopHdg()     { return true; }

    void SizeToFit(CRect &rect);
};

//////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// CTargetPriorityPP dialog
////////////////////////////////////////////////////////////////////////////////

class CTargetPriorityGrid : public CUGCtrl
{
public:
   virtual void OnSetup();

   void AddRowData(bool checked, CString name);
   void SetRowData(int row, bool checked, CString name); 
   void SetCellReadOnly(int col, int row, bool readonly);
   void GetRowData(int row, bool &checked, CString &name);
};

//-----------------------------------------------------------------------------

class CTargetPriorityPP : public CPropertyPage
{
	DECLARE_DYNAMIC(CTargetPriorityPP)

public:
	CTargetPriorityPP();
	virtual ~CTargetPriorityPP();

// Dialog Data
	enum { IDD = IDD_PP_TARGETPRIORITY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CTestPlan *m_pTestPlan;
	CDFTTargetPriority *m_pTargetPriority;
	CWnd *m_pParent;

   CTargetPriorityGrid m_targetGridTop;
   CTargetPriorityGrid m_targetGridBot;

	void fillGrids();

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinTop(NMHDR *pNMHDR, LRESULT *pResult);
   afx_msg void OnDeltaposSpinBot(NMHDR *pNMHDR, LRESULT *pResult);
   void OnDeltaposSpinHandler(bool isTop, NMHDR *pNMHDR, LRESULT *pResult);
	
	DECLARE_EVENTSINK_MAP()

public:
	void SetCurrentParentWnd(CWnd *wnd)						{ m_pParent = wnd;																					};
	void SetTestPlanPointer(CTestPlan *testPlan)			{ m_pTestPlan = testPlan;	m_pTargetPriority = &testPlan->GetTargetPriority();	};
	bool UpdateTestPlan();
	void UpdateUI();
};


////////////////////////////////////////////////////////////////////////////////
// CProbesPP dialog
////////////////////////////////////////////////////////////////////////////////
class CProbesPP : public CPropertyPage
{
	DECLARE_DYNAMIC(CProbesPP)

public:
	CProbesPP();
	CProbesPP(CCEtoODBDoc *Doc);
	virtual ~CProbesPP();

// Dialog Data
	enum { IDD = IDD_PP_PROBES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CCEtoODBDoc *m_pDoc;
	CTestPlan *m_pTestPlan;
	CDFTProbeTemplates *m_pProbes;
   BOOL m_bNeverMirrorRefname;
	CWnd *m_pParent;

   CProbesGrid m_topProbesGrid;
   CProbesGrid m_botProbesGrid;

   void updateRowStates(CProbesGrid &probesGrid, int row = -1);

   void FillGrid(CProbesGrid &probesGrid, ETestSurface surface);


protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedAddTop();
	afx_msg void OnBnClickedRemoveTop();
	afx_msg void OnBnClickedAddBottom();
	afx_msg void OnBnClickedRemoveBottom();
	DECLARE_EVENTSINK_MAP()

public:
	void SetCurrentParentWnd(CWnd *wnd)						{ m_pParent = wnd;																};
	void SetTestPlanPointer(CTestPlan *testPlan)			{ m_pTestPlan = testPlan;	m_pProbes = &testPlan->GetProbes();		};
	bool UpdateTestPlan();
	void UpdateUI();
};


////////////////////////////////////////////////////////////////////////////////
// CKelvinPowerInjPP dialog
////////////////////////////////////////////////////////////////////////////////
class CKelvinPowerInjPP : public CPropertyPage
{
	DECLARE_DYNAMIC(CKelvinPowerInjPP)

public:
	CKelvinPowerInjPP();
	virtual ~CKelvinPowerInjPP();

// Dialog Data
	enum { IDD = IDD_PP_KELVIN_POWERINJ };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CTestPlan *m_pTestPlan;
	CWnd *m_pParent;

	BOOL m_useResistor;
	BOOL m_useCapacitor;
	BOOL m_useInductor;
	double m_resistorValue;
	double m_capacitorValue;
	int m_probesPerPowerRail;
	int m_probesPerConn;
	int m_probesPerNumNets;
	BOOL m_powerInjectionSetting;

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedResistorChk();
	afx_msg void OnBnClickedCapacitorChk();
	afx_msg void OnBnClickedInductorChk();
	afx_msg void OnBnClickedRadioPowerInj();
	afx_msg void OnEnChangeProbesPerPowerrail();
	afx_msg void OnEnChangeProbesPerConn();
	afx_msg void OnEnChangeProbesPerNumnet();
	afx_msg void OnEnChangeResistorVal();
	afx_msg void OnEnChangeCapacitorVal();

public:
	void SetCurrentParentWnd(CWnd *wnd)						{ m_pParent = wnd;				};
	void SetTestPlanPointer(CTestPlan *testPlan)			{ m_pTestPlan = testPlan;		};
	bool UpdateTestPlan();
};


////////////////////////////////////////////////////////////////////////////////
// CPreconditionsPP dialog
////////////////////////////////////////////////////////////////////////////////
class CPreconditionsPP : public CPropertyPage
{
	DECLARE_DYNAMIC(CPreconditionsPP)

public:
	CPreconditionsPP();
	CPreconditionsPP(CCEtoODBDoc *Doc);
	virtual ~CPreconditionsPP();

// Dialog Data
	enum { IDD = IDD_PP_PRECONDITIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	// temporary control variable
	bool zoomToSelected;

	CCEtoODBDoc *m_pDoc;
	CCEtoODBView *m_pView;
	CTestPlan *m_pTestPlan;
	CDFTPreconditions *m_pPreconditions;
	CWnd *m_pParent;

   CPreconditionsGrid m_ppAvailableFeaturesGrid;
   CPreconditionsGrid m_ppForcedProbesGrid;
   CPreconditionsGrid m_ppNoProbesGrid;

   void moveSelected(CPreconditionsGrid &sourceGrid, CPreconditionsGrid &destGrid);
	void zoomToComponent(CString compName);
	void getAndFillFeatureGrid();

//	CListBox m_netListBox;
	CString m_filter;

	////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////
	// for testing purposes
	CStringList netList;	// dummy data
	////////////////////////////////////////////////////////
protected:
	virtual BOOL OnInitDialog();

	DECLARE_EVENTSINK_MAP()

public:
	void SetCurrentParentWnd(CWnd *wnd)						{ m_pParent = wnd;																				};
	void SetTestPlanPointer(CTestPlan *testPlan)			{ m_pTestPlan = testPlan;	m_pPreconditions = &testPlan->GetPreconditions();	};
	bool UpdateTestPlan();
	afx_msg void OnBnClickedAvail2forced();
	afx_msg void OnBnClickedAvail2no();
	afx_msg void OnBnClickedForced2avail();
	afx_msg void OnBnClickedNo2avail();
};


////////////////////////////////////////////////////////////////////////////////
// CNetConditionsPP dialog
////////////////////////////////////////////////////////////////////////////////
class CNetConditionsPP : public CPropertyPage
{
	DECLARE_DYNAMIC(CNetConditionsPP)

public:
	CNetConditionsPP();
	CNetConditionsPP(CCEtoODBDoc *Doc);
	virtual ~CNetConditionsPP();

// Dialog Data
	enum { IDD = IDD_PP_NET_CONDITIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CCEtoODBDoc *m_pDoc;
	CCEtoODBView *m_pView;
	CTestPlan *m_pTestPlan;
	CDFTNetConditions *m_pNetConditions;
	CWnd *m_pParent;

	CString m_filter;
	BOOL m_caseSensitive;
	CString m_curFilter;

	CString m_netType;
	CComboBox m_netTypeCombo;
	BOOL m_noProbe;
	CString m_voltage;
	CString m_current;
	CString m_testResources;
	CString m_powerInjectionResorces;

   CNetConditionsGrid m_netConditionsGrid;

	void fillNetsGrid();

	void applyFilter(CString curFilter = "*", bool useCaseSensitivity = true);
   bool applyCurrentNetSettings(CNetConditionsGrid &netsGrid, int row);
	void zoomToNet(CString netName);

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedFilterNets();
	afx_msg void OnBnClickedUnfilterNets();
	afx_msg void OnBnClickedApplyToSelected();
	afx_msg void OnBnClickedApplyToVisible();
	afx_msg void OnBnClickedReset();

	DECLARE_EVENTSINK_MAP()

public:
	void SetCurrentParentWnd(CWnd *wnd)						{ m_pParent = wnd;																					};
	void SetTestPlanPointer(CTestPlan *testPlan)			{ m_pTestPlan = testPlan;	m_pNetConditions = &m_pTestPlan->GetNetConditions();	};
	void ReloadFromTestPlan();
	bool UpdateTestPlan();
};


////////////////////////////////////////////////////////////////////////////////
// COptionsPP dialog
////////////////////////////////////////////////////////////////////////////////
class COptionsPP : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionsPP)

public:
	COptionsPP();
	virtual ~COptionsPP();

// Dialog Data
	enum { IDD = IDD_PP_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CTestPlan *m_pTestPlan;
	CWnd *m_pParent;

	BOOL m_allowDoubleWiring;
	long m_startProbeNumber;
	BOOL m_useSurface;
	BOOL m_surfacePreference;
	BOOL m_useProbeOffsets;
	BOOL m_useSMDTop;
	BOOL m_useSMDBot;
	BOOL m_usePTHTop;
	BOOL m_usePTHBot;
	BOOL m_useViaTop;
	BOOL m_useViaBot;
	BOOL m_useCase;
	BOOL m_placeOnAllAccess;
	BOOL m_useExistingProbes;
	BOOL m_probeUse;
	BOOL m_retainProbeNames;
	BOOL m_retainProbePositions;
	BOOL m_retainProbeSizes;
	BOOL m_chkWriteIgnoreSurface;

	void updateUIProbeSide();
	void updateUIProbeLocking();

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioSideUse();
	afx_msg void OnBnClickedRadioSidePreference();
	afx_msg void OnEnChangeEditProbeStartNumber();
	afx_msg void OnBnClickedCheckDoubleWires();
	afx_msg void OnBnClickedLoadFeedback();
	afx_msg void OnBnClickedSaveCSV();
	afx_msg void OnBnClickedAllAccessible();
	afx_msg void OnBnClickedCheckUseExistingProbes();
	afx_msg void OnUpdateParentSheet();
	afx_msg void OnBnClickedCaseSensitive();

public:
	void SetCurrentParentWnd(CWnd *wnd)						{ m_pParent = wnd;				};
	void SetTestPlanPointer(CTestPlan *testPlan)			{ m_pTestPlan = testPlan;		};
	bool UpdateTestPlan();
	afx_msg void OnBnClickedCheckPpWriteIgnoredSurface();
};


////////////////////////////////////////////////////////////////////////////////
// CProbeLockingPP dialog
////////////////////////////////////////////////////////////////////////////////
class CProbeLockingPP : public CPropertyPage
{
	DECLARE_DYNAMIC(CProbeLockingPP)

public:
	CProbeLockingPP();
	virtual ~CProbeLockingPP();

// Dialog Data
	enum { IDD = IDD_PP_PROBE_LOCKING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CTestPlan *m_pTestPlan;
	CWnd *m_pParent;

public:
	void SetCurrentParentWnd(CWnd *wnd)						{ m_pParent = wnd;				};
	void SetTestPlanPointer(CTestPlan *testPlan)			{ m_pTestPlan = testPlan;		};
};

////////////////////////////////////////////////////////////////////////////////
// CProbePlacementExitDlg dialog
////////////////////////////////////////////////////////////////////////////////
class CProbePlacementExitDlg : public CDialog
{
	DECLARE_DYNAMIC(CProbePlacementExitDlg)
private:
   CString m_msg;

public:
   CProbePlacementExitDlg(CString message, CWnd* pParent = NULL);
	//virtual ~CProbePlacementExitDlg();
   virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_PP_EXIT };

   DECLARE_MESSAGE_MAP()
   afx_msg void OnBnClickedProbeStyles()  { EndDialog(IDC_PROBE_STYLES); }

};

////////////////////////////////////////////////////////////////////////////////
// CProbePlacementDlg dialog
////////////////////////////////////////////////////////////////////////////////
class CProbePlacementDlg : public CDialog
{
	DECLARE_DYNAMIC(CProbePlacementDlg)

public:
	CProbePlacementDlg(CCEtoODBDoc *Doc, CWnd* pParent = NULL);
	virtual ~CProbePlacementDlg();

// Dialog Data
	enum { IDD = IDD_PP_PROBE_PLACEMENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnUpdateFromTabDialog();
	afx_msg LRESULT OnSetPage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdatePages();
	afx_msg void OnSetModifiedPage();
	afx_msg void OnBnClickedProcess();
   afx_msg void OnBnClickedDisplayReport();
	afx_msg void OnBnClickedSaveTestplan();
	afx_msg void OnBnClickedSaveTestplanDebug();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

private:
	CCEtoODBDoc *m_pDoc;
	FileStruct *m_pFile;
	CDFTSolution m_DFTSolution;

	void createAndPlacePropertySheet();
	void populateDFTProbePlacementData();

   CPPResultsGrid m_ppResultsGrid;

	CPropertySheet m_propSheet;
	CTargetPriorityPP m_targetPriorityDlg;
	CProbesPP m_probesDlg;
	CKelvinPowerInjPP m_kelvinPowerInjectionDlg;
	CPreconditionsPP m_preconditionsDlg;
	CNetConditionsPP m_netConditionsDlg;
	COptionsPP m_optionsDlg;
	CProbeLockingPP m_probeLockingDlg;

	long lastTTTRow, lastTTTCol;
	void UpdateTestplan();

   CString GetReportFilename()   { return GetLogfilePath("ProbePlacementReport.txt"); }

protected:
	virtual void OnOK();

	DECLARE_EVENTSINK_MAP()

public:
	virtual BOOL OnInitDialog();
};

#endif
