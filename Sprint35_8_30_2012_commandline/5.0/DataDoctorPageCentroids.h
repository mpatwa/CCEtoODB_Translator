// $Header: /CAMCAD/4.6/DataDoctorPageCentroids.h 20    4/06/07 4:46p Rick Faltersack $

#if ! defined (__DataDoctorPageCentroids_h__)
#define __DataDoctorPageCentroids_h__

#pragma once

#include "DataDoctorDialog.h"

//----------------------------------------------------------------------------

#define DFT_OUTLINE_TOP						"DFT_OUTLINE_TOP"
#define DFT_OUTLINE_BOTTOM					"DFT_OUTLINE_BOTTOM"

#define ALG_BODY_OUTLINE					0
//#define ALG_PIN_CENTER						1			
#define ALG_BODY_PIN_EXTENTS				2
#define ALG_INSIDE_PADS						3


// Naming style in original Generate Component Centroids code
#define Q_PIN_CENTERS      "Pin-Centers"
#define Q_PIN_EXTENTS      "Pin-Extents"
#define Q_COMP_OUTLINE     "Component-Outline"
#define Q_PIN_BODY_EXTENTS "Pin-Body-Extents"
#define Q_XY_VALUE         "XY-Value"
#define Q_NONE             "None"

class CDataDoctorCentroidsPage;

//----------------------------------------------------------------------------

class CDDCentroidsGrid : public CDDBaseGrid
{
private:
   CDataDoctorCentroidsPage *m_centroidsPage;

 public:
    CDDCentroidsGrid(CDataDoctorCentroidsPage *centsPage)  { m_centroidsPage = centsPage; }
   ~CDDCentroidsGrid() { }

   virtual void OnSetup();

	//cell type notifications
	virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);

	//editing
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);

	//menu notifications
	virtual void OnMenuCommand(int col,long row,int section,int item);
	virtual int  OnMenuStart(int col,long row,int section);

   //column swapping
	virtual void OnColSwapped(int fromCol,int toCol);

};

//----------------------------------------------------------------------------

class CDataDoctorCentroidsPage : public CDataDoctorPropertyPageType
{
	DECLARE_DYNAMIC(CDataDoctorCentroidsPage)

private:
   CDataDoctorPropertySheet& m_parent;
   BOOL m_overwriteValues;
   BOOL m_storeOption;
   //CResizingPropertyPageToolBar m_toolbar;
   CViewLiteToolbar m_toolbar;

   bool m_updatePending;
   int m_CentroidCommentKeywordIndex;

   bool m_dialogIsInitialized;

public:
   // Dialog Data
	enum { IDD = IDD_DataDoctorCentroidsPage };

   CDataDoctorCentroidsPage(CDataDoctorPropertySheet& parent);
   virtual ~CDataDoctorCentroidsPage();

   CDataDoctorPropertySheet& getParent() { return m_parent; }
   CCamCadDatabase& getCamCadDatabase();

   void QueueUpdate()      { m_updatePending = true; }
   bool IsUpdatePending()  { return m_updatePending; }

   bool DialogIsInitialized()    { return m_dialogIsInitialized; }

   void EnableDatabaseControls(bool flag);

   int GetCentroidCommentKeywordIndex();
   void SetCentroidComment(int row, CString comment);
   CString GetCentroidComment(BlockStruct *block);

private:

// Construction
///public:
///   GenerateCentroidDlg();
///   GenerateCentroidDlg(CCEtoODBDoc *doc);
///   ~GenerateCentroidDlg();

// Dialog Data
   //{{AFX_DATA(GenerateCentroidDlg)
  // enum { IDD = IDD_GEN_CENTROID };
   int m_radioApplyAll;
   int m_radioShowPins;
   //}}AFX_DATA
   CCEtoODBDoc *m_doc;
	FileStruct *m_activeFile;


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(GenerateCentroidDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(GenerateCentroidDlg)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
   //afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
   afx_msg void OnBnClickedLoadFromLibrary();
   afx_msg void OnBnClickedStoreToLibrary();
   afx_msg void OnBnClickedPinsShowAll();
   afx_msg void OnBnClickedPinsShowNamed();
   afx_msg void OnBnClickedPinsShowNumbered();
   afx_msg void OnBnClickedClearCentroids();
   afx_msg void OnTbBnClickedZoomIn()           { m_toolbar.OnBnClickedZoomInOrigin(); }
   afx_msg void OnTbBnClickedColorsetsCustom()  { m_toolbar.OnBnClickedColorsetsCustom(); }
   afx_msg void OnTbBnClickedLayerTypes()       { m_toolbar.OnBnClickedLayerTypes(); }
   afx_msg void OnTbBnClickedShowPolyFill()     { m_toolbar.OnBnClickedShowPolyFills(); }
   afx_msg void OnTbBnClickedShowApertureFill() { m_toolbar.OnBnClickedShowApertureFills(); }
   afx_msg LRESULT OnRowEditFinished(WPARAM wParamROW, LPARAM lParamCOL);
   
public:
   
   // Class members for quick and easy access to column indices
   int m_colStatus;
	int m_colGeomName;
   int m_colRefnames;
   int m_colPinCnt;
	int m_colCentX;
	int m_colCentY;
	int m_colCentR;
	int m_colMethod;
   int m_colMessage;
   int m_colComments;

   CColumnDefMap m_columnDefMap;

   CDDSpecialColumnMap m_customColumnMap_CentroidGrid;

public:
   afx_msg void OnBnClickedBtnShowgeom();
   afx_msg void OnBnClickedBtnGenCentroids();
   virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();
   virtual BOOL OnKillActive();
   void update();
   virtual CString GetDialogProfileEntry() { return "DataDoctorCentroidsTab"; }

   bool IsCustomCentroidGridColumn(int colIndx)  { return m_customColumnMap_CentroidGrid.GetSpecialColumn(colIndx) != NULL; }

   void DefineGridColumnMap();

   void DoEditGeom();

   void AfterEditGrid(long Row, long Col);

   DECLARE_EVENTSINK_MAP()
   void StartEditGrid(long Row, long Col, BOOL* Cancel);
	void MouseDownGrid(short Button, short Shift, float X, float Y);
   void OnRButtonDown(CPoint windowRelativePoint);

   

private:
	int m_priority[3];
   CDDCentroidsGrid m_grid;
   int m_chkHideSinglePinComps;
   int m_chkZeroRotation;

   void SaveStickySettings();
   void LoadStickySettings();

   bool SaveCustomAttribsToDB(CDBInterface &db, CDataDoctorPackage *ddpkg);
   bool LoadCustomAttribsFromDB(CDBInterface &db, CDataDoctorPackage *ddpkg);

   void addFieldControls();
   void setItemToGrid(GRID_CNT_TYPE row, GRID_CNT_TYPE col, CString itemValue);
   void fillGrid();
   void setCentroidInRow(int rowIndex, int method, bool overwrite);
   void setByDefaultAllRows();
   void ApplyOneRow(int rowIndex);
   void ApplyXyValue(int rowIndex);
   void ApplyUserRotation(int rowIndex);
	double GenerateRotationOffset(BlockStruct* block, DataStruct* centroid);
   CDDCentroidsGrid &getGrid()   { return m_grid; }

	bool calculateCentroid(BlockStruct *block, int method, double *x, double *y, CString &curAlg, CString &message);

	void setGridRow(GRID_CNT_TYPE row, CString centXstr, CString centYstr, CString centRstr, CString algorithm, DataDoctorStatusTag status, CString message, CString comment);
	void setGridRow(GRID_CNT_TYPE row, double centX, double centY, double centR, CString algorithm, CString message, CString comment);
	void setGridRow(GRID_CNT_TYPE row, double centX, double centY, int centR, CString algorithm, CString message, CString comment);
	void setGridRow(GRID_CNT_TYPE row, DataStruct *centroidData, CString algorithm, CString message, CString comment);
   void setGridRow(GRID_CNT_TYPE row, CDataDoctorPackage *ddpkg);

   void resizeGrid();

   void setRowStyle(int rowIndex, DataDoctorStatusTag status);

   bool isRecognizedMethodName(CString name);

public:
   CComboBox m_cboDefault;
   afx_msg void OnBnClickedBtnClose();
   afx_msg void OnBnClickedChkHideSinglePinComps();
   afx_msg void OnBnClickedChkUseZeroRotation();
   afx_msg void ClearGridSelection();
   afx_msg void OnLoadCentroid(); // from DB library
   afx_msg void OnSaveCentroid(); // to DB library

};

//-----------------------------------------------------------------------------



#endif