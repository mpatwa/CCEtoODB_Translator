// $Header: /CAMCAD/4.6/DataDoctorPagePackages.h 11    4/06/07 4:46p Rick Faltersack $

#if ! defined (__DataDoctorPagePackages_h__)
#define __DataDoctorPagePackages_h__

#pragma once

#include "DataDoctorDialog.h"

#include "Edit.h"

class CDataDoctorPackagesPage;

enum EDDPkgGridViewMode
{
   PkgGridViewGeometries,
   PkgGridViewPartNumbers
};

//----------------------------------------------------------------------------

class CDDOutlineEditGrid : public CDDBaseGrid
{
private:
   CDataDoctorPackagesPage *m_packagesPage;
   CPoly *m_activePoly;  // Set when grid is filled

public:
    CDDOutlineEditGrid(CDataDoctorPackagesPage *page);
    ~CDDOutlineEditGrid() { }

   CUGEdit m_myCUGEdit;

   virtual void OnSetup();

   // Override to do nothing, CDDBaseGrid will sort the grid, don't want sorting in this grid
   virtual void OnTH_LClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed=0) {}; 

   virtual void OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed);

   void Fill(CDataDoctorPackagesPage *pkgpage, int markRow = -1);  // Fill whole grid with outline from package
   void Fill(int rowIndx, CPnt *pnt);                              // Fill one row with point data

   CPoly *GetActivePoly()     { return m_activePoly; }

   void Highlight(CDataDoctorPackagesPage *pkgpage, int row);  // Experimental
   void Highlight(CDataDoctorPackagesPage *pkgpage);  // Experimental

	//menu notifications
	virtual void OnMenuCommand(int col,long row,int section,int item);
	virtual int  OnMenuStart(int col,long row,int section);

	//editing
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual int OnEditVerify(int col,long row,CWnd *edit,UINT *vcKey);
	virtual int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);

   // to support enable/disable, display or hide column titles
   void SetColumnTitles(bool show);

};

//----------------------------------------------------------------------------

class CDDPkgsByGeometryGrid : public CDDBaseGrid
{
private:
   CDataDoctorPackagesPage *m_packagesPage;

 public:
    CDDPkgsByGeometryGrid(CDataDoctorPackagesPage *page);
    ~CDDPkgsByGeometryGrid() { }

   virtual void OnSetup();

	//cell type notifications
	virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);

	//menu notifications
	virtual void OnMenuCommand(int col,long row,int section,int item);
	virtual int  OnMenuStart(int col,long row,int section);

	//editing
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual int OnEditVerify(int col,long row,CWnd *edit,UINT *vcKey);
	virtual int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);

   //column swapping
	virtual void OnColSwapped(int fromCol,int toCol);

   // Data Doctor
   ////void FillGridRow(int row, CDataDoctorPackage *ddpkg);
private:
   void SetRowStatusColor(int row, DataDoctorStatusTag status);
public:
   void SetBackgroundColors(COLORREF red, COLORREF green, COLORREF gray) {m_redBackgroundColor = red; m_greenBackgroundColor = green; m_lightGrayBackgroundColor = gray; }
   void UpdateStatus(int row);
};

//----------------------------------------------------------------------------

class CDDPkgsByPartNumberGrid : public CDDBaseGrid
{
private:
   CDataDoctorPackagesPage *m_packagesPage;

 public:
    CDDPkgsByPartNumberGrid(CDataDoctorPackagesPage *page);
    ~CDDPkgsByPartNumberGrid() { }

   virtual void OnSetup();

	//cell type notifications
	virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);

	//menu notifications
	virtual void OnMenuCommand(int col,long row,int section,int item);
	virtual int  OnMenuStart(int col,long row,int section);

	//editing
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual int OnEditVerify(int col,long row,CWnd *edit,UINT *vcKey);
	virtual int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);

   //column swapping
	virtual void OnColSwapped(int fromCol,int toCol);

   // Special row color setter
   void SetRowBackColor(int rowIndx, COLORREF rowColor, COLORREF pkgSrcColColor);

   // Data Doctor
   ////void FillGridRow(int row, CDataDoctorPackage *ddpkg);
private:
   void SetRowStatusColor(int row, DataDoctorStatusTag status);
public:
   void SetBackgroundColors(COLORREF red, COLORREF green, COLORREF gray) {m_redBackgroundColor = red; m_greenBackgroundColor = green; m_lightGrayBackgroundColor = gray; }  
   void UpdateStatus(int row);
};

//----------------------------------------------------------------------------

class CDataDoctorPackagesPage : public CDataDoctorPropertyPageType
{
	DECLARE_DYNAMIC(CDataDoctorPackagesPage)

private:
   BOOL m_overwriteValues;
	BOOL m_storeOption;
   int m_viewModePartNum;
   static int m_outlineFill;     // Special handling, this value needs visibility outside of this tab
   int m_chkHideSinglePinComps;
   int m_outlineChamferRound;
   bool m_updatePending;   // for "global" update from other tabs
   bool m_updateOtherGrid; // for updating other grid when active grid changes

   bool m_dialogIsInitialized;

   CViewLiteToolbar m_toolbar;

   CDataDoctorPropertySheet& m_parent;

   CDDPkgsByGeometryGrid   m_pkgGeomGrid;
   CDDPkgsByPartNumberGrid m_pkgPartGrid;

   CDDOutlineEditGrid m_outlineEditGrid;
   DataStruct *m_highlightPntInsertData;

   CCEtoODBDoc *m_doc;
	FileStruct *m_activeFile;

   EDDPkgGridViewMode GetGridMode()             { return (m_viewModePartNum==0?PkgGridViewGeometries:PkgGridViewPartNumbers); }
   ///void SetGridMode(EDDPkgGridViewMode mode)    { m_gridViewMode = mode; }

   bool DialogIsInitialized()    { return m_dialogIsInitialized; }
   

   void AddFieldControls();
   void DefineGridColumnMaps();
   void FillGrid();
   void FillGeomGrid();
   void FillPartGrid();
   void FillGeomGridRow(int row, CDataDoctorPackage *ddpkg);
   void FillPartGridRow(int row, CDataDoctorPart *ddpart);

   CDDSpecialColumnMap m_customColumnMap_GeomGrid;
   CDDSpecialColumnMap m_customColumnMap_PartGrid;


   int m_radioApplyAll;
   CComboBox m_cboDefault;

   void EnableCustomOutlineEditing(bool enable);
   CComboBox m_cboTemplate;
   void SetTemplate(CString labelA1, CString labelA2, CString labelA3, bool showRoundCheckbox);
   void SetTemplate(CString a1, CString a2, CString a3, CString a4, CString b1, CString b2, CString b3, CString b4, CString b5);
   void ClearTemplate() { SetTemplate("", "", "", "", "", "", "", "", ""); }
   CDataDoctorRealPart *BeginOutline(CPoint2d &origin);
   void EndOutline(CDataDoctorRealPart *realpart);
   void CreateCustomRectangularOutline(double length, double width, double chamfer, bool roundChamfer);
   void CreateCustomCircularOutline(double diameter);
   void CreateCustomQFPOutline(double bodylen, double bodywid, double pinpitch, double pinwid, int pincountL, int pincountW, double pintoe, double overalllen, double overallwid);
   void CreateCustomSOICOutline(double bodylen, double bodywid, double pinpitch, double pinwid, int pincount, double pintoe, double overallwid);
   void CreateCustomSOT23Outline(double bodylen, double bodywid, double pinpitch, double pinwid, double pintoe, double overallwid);
   double GetDouble(int dlgItem);
   int GetInteger(int dlgItem);
   void InsertNewVertexAfter(int row); // Acts on current outline grid content
   void gatherFamily(CCamCadDatabase& camCadDatabase, CDataDoctorPart* part, CDataDoctorPackages &pkages);   
   
   void EnableGridView(); // show/hide geometry and parts grids according to switch setting

   void ResetRPOutlines();

   void SaveStickySettings();
   void LoadStickySettings();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

   CEditNumber m_templateEditA1;
   CEditNumber m_templateEditA2;
   CEditNumber m_templateEditA3;
   CEditNumber m_templateEditA4;
   CEditNumber m_templateEditB1;
   CEditNumber m_templateEditB2;
   CEditNumber m_templateEditB3;
   CEditNumber m_templateEditB4;
   CEditNumber m_templateEditB5;

   CEditNumber m_editOffsetX;
   CEditNumber m_editOffsetY;

public:
   CDataDoctorRealPart*  GetActiveRealPart();
   CDataDoctorComponent* GetActiveSampleComponent();

// Column index vars
	int m_colGeometry_GeomGrid;
   int m_colOutlineMethod_GeomGrid;
   int m_colPkgSource_GeomGrid;
   int m_colPkgAlias_GeomGrid;
   int m_colDPMO_GeomGrid;
   int m_colPkgHeight_GeomGrid;
   int m_colRefnames_GeomGrid;
   int m_colMessage_GeomGrid;
   int m_colFamily_GeomGrid;
   int m_colComments_GeomGrid;

   CColumnDefMap m_columnDefMap_GeomGrid;

   int m_colPartNumber_PartGrid;
	int m_colGeometry_PartGrid;
   int m_colOutlineMethod_PartGrid;
   int m_colPkgSource_PartGrid;
   int m_colPkgAlias_PartGrid;
   int m_colDPMO_PartGrid;
   int m_colPkgHeight_PartGrid;
   int m_colRefnames_PartGrid;
   int m_colMessage_PartGrid;
   int m_colFamily_PartGrid;
   int m_colComments_PartGrid;

   CColumnDefMap m_columnDefMap_PartGrid;

   bool IsCustomGeomGridColumn(int colIndx)  { return m_customColumnMap_GeomGrid.GetSpecialColumn(colIndx) != NULL; }
   bool IsCustomPartGridColumn(int colIndx)  { return m_customColumnMap_PartGrid.GetSpecialColumn(colIndx) != NULL; }

	CDataDoctorPackagesPage(CDataDoctorPropertySheet& parent);
	virtual ~CDataDoctorPackagesPage();

// Dialog Data
   enum { IDD = IDD_DataDoctorPackagesPage };

   static bool GetOutlineFillMode()                    { return (m_outlineFill==0?false:true); }

public:
   virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();
   virtual BOOL OnKillActive();

   virtual CString GetDialogProfileEntry() { return CString("DataDoctorPackagesPage"); }

   void QueueUpdate()      { m_updatePending = true; }
   bool IsUpdatePending()  { return m_updatePending; }

   void EnableDatabaseControls(bool flag);

   void Update();  // refills the grid from CC data
   void Update(int row); // refills the grid row from already attached package data
   ///void UpdateStatus(int row); // checks data in row, updates row color and message
   void UpdateStatusActiveRow(); // Figures out which grid and updates currently selected row

   void DoEditGeom(); // zoom to individual geom in selected row, does not really edit anything
   //void DoEditGeom(CDataDoctorComponent *ddcomp); // zoom to this comp geom
   void DoEditGeom(CDataDoctorComponent *ddcomp, BlockStruct *realpartBlk, bool enableCustom, int highlightPtIndx); // zoom to this comp geom

   void SetHighlight(double x, double y, bool show);


   CDataDoctorPropertySheet& getParent()  { return m_parent; }
   CCamCadDatabase& getCamCadDatabase();

   CDataDoctorPackages& getPackages();

   DECLARE_MESSAGE_MAP()
   virtual void OnCancel();
   afx_msg void OnBnClickedLoadFromLibrary();
   afx_msg void OnBnClickedStoreToLibrary();
   afx_msg void OnLoadPackage();           // one package from DB
   afx_msg void OnLoadGeometryPackage();   // one package from DB
   afx_msg void OnLoadPartNumberPackage(); // one package from DB
   afx_msg void OnSavePackage();           // one package to DB
   afx_msg void OnSaveGeometryPackage();   // one package to DB
   afx_msg void OnSavePartNumberPackage(); // one package to DB
   afx_msg void OnTbBnClickedZoomIn()           { m_toolbar.OnBnClickedZoomInOrigin(); }
   afx_msg void OnTbBnClickedColorsetsCustom()  { m_toolbar.OnBnClickedColorsetsCustom(); }
   afx_msg void OnTbBnClickedLayerTypes()       { m_toolbar.OnBnClickedLayerTypes(); }
   afx_msg void OnTbBnClickedShowPolyFill()     { m_toolbar.OnBnClickedShowPolyFills(); }
   afx_msg void OnTbBnClickedShowApertureFill() { m_toolbar.OnBnClickedShowApertureFills(); }
   afx_msg LRESULT OnRowEditFinished(WPARAM wParamROW, LPARAM lParamCOL);
   afx_msg void OnBnClickedChkHideSinglePinComps();
   afx_msg void OnBnClickedBtnGenOutlines();
   afx_msg void OnBnClickedGridViewMode();
   afx_msg void OnBnClickedFillOutlines();
   afx_msg void OnCboSelchangeTemplate();
   afx_msg void OnBnClickedCreateCustomTemplateOutline();
   afx_msg void OnBnClickedInsertVertexAfter();
   afx_msg void OnBnClickedInsertVertexBefore();
   afx_msg void OnBnClickedDeleteVertex();
   afx_msg void OnBnClickedCurveToNext();
   afx_msg void OnBnClickedApplyOffset();
   afx_msg void OnBnClickedRotate90();
   afx_msg void OnOutlineTemplateValueChanged();

   bool ValidateTemplateValue(int controlID, bool integer = false);

};


#endif