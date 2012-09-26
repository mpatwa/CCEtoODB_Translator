// $Header: /CAMCAD/4.6/DataDoctorPageSubclasses.h 11    4/06/07 4:46p Rick Faltersack $

#if ! defined (__DataDoctorPageSubclasses_h__)
#define __DataDoctorPageSubclasses_h__

#pragma once

#include "DataDoctorDialog.h"
#include "Element.h"

class CDataDoctorSubclassesPage;



//----------------------------------------------------------------------------

class CDDSubclassesGrid : public CDDBaseGrid
{
private:
   CDataDoctorSubclassesPage *m_subclassesPage;
   CPoly *m_activePoly;  // Set when grid is filled

   // Background colors - Based on Error Statuses
   COLORREF m_backgroundColorOK;
   COLORREF m_backgroundColorERROR;
   COLORREF m_backgroundColorIGNORE;

public:
    CDDSubclassesGrid(CDataDoctorSubclassesPage *page);
    ~CDDSubclassesGrid() { }

   CUGEdit m_myCUGEdit;

   virtual void OnSetup();

   // Override to do nothing, CDDBaseGrid will sort the grid, don't want sorting in this grid
   virtual void OnTH_LClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed=0) {}; 

   virtual void OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed);

   void Fill(CSubclassList &subclassList);
   void AddRow(CSubclass *sc);

   CPoly *GetActivePoly()     { return m_activePoly; }

	//menu notifications
	virtual void OnMenuCommand(int col,long row,int section,int item);
	virtual int  OnMenuStart(int col,long row,int section);

	//editing
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual int OnEditVerify(int col,long row,CWnd *edit,UINT *vcKey);
	virtual int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);

   // colors
   void SetBackgroundColors(COLORREF ok, COLORREF err, COLORREF ignore) {m_backgroundColorOK = ok; m_backgroundColorERROR = err; m_backgroundColorIGNORE = ignore; }
   COLORREF GetBackgroundColor(DataDoctorStatusTag stat) { switch(stat) { case dataDoctorStatusOk: return m_backgroundColorOK; case dataDoctorStatusError: return m_backgroundColorERROR; case dataDoctorStatusIgnore: return m_backgroundColorIGNORE; } return m_backgroundColorOK; }
   

};

//----------------------------------------------------------------------------

class CDDElementsGrid : public CDDBaseGrid
{
private:
   CDataDoctorSubclassesPage *m_subclassesPage;
   CPoly *m_activePoly;  // Set when grid is filled

   // Background colors - Based on Error Statuses
   COLORREF m_backgroundColorOK;
   COLORREF m_backgroundColorERROR;
   COLORREF m_backgroundColorIGNORE;

public:
    CDDElementsGrid(CDataDoctorSubclassesPage *page);
    ~CDDElementsGrid() { }

   CUGEdit m_myCUGEdit;

   virtual void OnSetup();

	//cell type notifications
	virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);

   // Override to do nothing, CDDBaseGrid will sort the grid, don't want sorting in this grid
   virtual void OnTH_LClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed=0) {}; 

   virtual void OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed);

   void Fill(CSubclass *sc);
   void AddRow(CSubclassElement *el);
   void SetRow(int row, CSubclassElement *el);

   CPoly *GetActivePoly()     { return m_activePoly; }

	//menu notifications
	virtual void OnMenuCommand(int col,long row,int section,int item);
	virtual int  OnMenuStart(int col,long row,int section);

	//editing
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual int OnEditVerify(int col,long row,CWnd *edit,UINT *vcKey);
	virtual int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);

   //column swapping
	virtual void OnColSwapped(int fromCol,int toCol);

   // colors
   void SetBackgroundColors(COLORREF ok, COLORREF err, COLORREF ignore) {m_backgroundColorOK = ok; m_backgroundColorERROR = err; m_backgroundColorIGNORE = ignore; }
   COLORREF GetBackgroundColor(DataDoctorStatusTag stat) { switch(stat) { case dataDoctorStatusOk: return m_backgroundColorOK; case dataDoctorStatusError: return m_backgroundColorERROR; case dataDoctorStatusIgnore: return m_backgroundColorIGNORE; } return m_backgroundColorOK; }
   

};

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

class CDataDoctorSubclassesPage : public CDataDoctorPropertyPageType
{
	DECLARE_DYNAMIC(CDataDoctorSubclassesPage)

private:
   BOOL m_overwriteValues;
	BOOL m_storeOption;
   bool m_updatePending;   // for "global" update from other tabs
   bool m_dialogIsInitialized;

   CDataDoctorPropertySheet& m_parent;
   //CString m_deviceTypeComboList;

   CDDSubclassesGrid m_subclassesGrid;

   CDDElementsGrid m_elementsGrid;

   CCEtoODBDoc *m_doc;
	FileStruct *m_activeFile;

   void AddFieldControls();
   void DefineGridColumnMaps();
   void FillGrids();

   double GetDouble(int dlgItem);
   
   void EnableGridView(); // show/hide geometry and parts grids according to switch setting

   void SaveStickySettings();
   void LoadStickySettings();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
   CDataDoctorRealPart*  GetActiveRealPart();
   CDataDoctorComponent* GetActiveSampleComponent();

   // Column index vars
	int m_colSubclass_SubclassesGrid;
   int m_colElements_SubclassesGrid;
   //int m_colMessage_SubclassesGrid;

   int m_colElementName_ElementsGrid;
   int m_colPin1Function_ElementsGrid;
   int m_colPin1Name_ElementsGrid;
   int m_colPin2Function_ElementsGrid;
   int m_colPin2Name_ElementsGrid;
   int m_colPin3Function_ElementsGrid;
   int m_colPin3Name_ElementsGrid;
   int m_colPin4Function_ElementsGrid;
   int m_colPin4Name_ElementsGrid;
   int m_colValue_ElementsGrid;
   int m_colPlusTol_ElementsGrid;
   int m_colMinusTol_ElementsGrid;
   int m_colDevType_ElementsGrid;

   CColumnDefMap m_columnDefMap_SubclassesGrid;
   CColumnDefMap m_columnDefMap_ElementsGrid;




	CDataDoctorSubclassesPage(CDataDoctorPropertySheet& parent);
	virtual ~CDataDoctorSubclassesPage();

   // Dialog Data
   enum { IDD = IDD_DataDoctorSubclassesPage };

public:
   virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();
   virtual BOOL OnKillActive();

   virtual CString GetDialogProfileEntry() { return CString("DataDoctorSubclassesPage"); }

   void QueueUpdate()      { m_updatePending = true; }
   bool IsUpdatePending()  { return m_updatePending; }

   bool DialogIsInitialized()    { return m_dialogIsInitialized; }

   void EnableDatabaseControls(bool flag);

   void Update();  // refills the grid from CC data
   void UpdateStatus(int row); // checks data in row, updates row color and message

   CString getNormalizedTolerance(CString randomTolerance);

   void DisplayElements(CSubclass *sc);

   bool isSupportedElementDevType(DeviceTypeTag devtype);

   CDataDoctorPropertySheet& getParent()  { return m_parent; }
   CCamCadDatabase& getCamCadDatabase();

   CDataDoctorPackages& getPackages();

   CSubclassList &getSubclassList()      ;// { return getParent().getDataDoctor().getSubclassList(); }
   //CSubclass *findSubclass(CString name) ;// { return this->getSubclassList().findSubclass(name); }

   void enableAddSubclassButton();

   CEdit m_newSubclassEditBox;
   CButton m_addSubclassButton;

   DECLARE_MESSAGE_MAP()
   virtual void OnCancel();
   afx_msg void OnBnClickedLoadFromLibrary();
   afx_msg void OnBnClickedStoreToLibrary();
   afx_msg void OnLoadSubclass();           // one subclass from DB
   afx_msg void OnSaveSubclass();           // one subclass to DB

   afx_msg LRESULT OnSubclassGridRowEditFinished(WPARAM wParamROW, LPARAM lParamCOL);
   afx_msg LRESULT OnElementGridRowEditFinished(WPARAM wParamROW, LPARAM lParamCOL);
   
   afx_msg void OnBnClickedAddSubclass();
   afx_msg void OnNewSubclassNameChanged();

   afx_msg void OnBnClickedSubclassCopy();
   afx_msg void OnBnClickedSubclassDelete();
   afx_msg void OnBnClickedSubclassPaste();

};

/////////////////////////////////////////////////////////////////////////////
// Delete Subclass Confirmation
class DeleteSubclassConfirmDlg : public CDialog
{
private:
   CString  m_subclassName;
   BOOL     m_justDoIt;

public:
   DeleteSubclassConfirmDlg(CString subclassName, CWnd* pParent = NULL);   // standard constructor
   ~DeleteSubclassConfirmDlg();

// Dialog Data
   //{{AFX_DATA(AddLayer)
   enum { IDD = IDD_DataDoctorSubclassDeleteConfirm };
   //}}AFX_DATA

   virtual CString GetDialogProfileEntry() { return "DeleteSubclassConfirmDlg"; }

   bool GetJustDoIt()     { return m_justDoIt?true:false; }

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(AddLayer)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(AddLayer)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   afx_msg void OnBnClickedJustDoIt();
   afx_msg void OnBnClickedOk();
};



#endif