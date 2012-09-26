// $Header: /CAMCAD/4.6/DataDoctorDialog.h 61    4/11/07 1:50p Rick Faltersack $

#if ! defined (__DataDoctorDialog_h__)
#define __DataDoctorDialog_h__

#pragma once

//.............................................................................

#define DataDoctorResizingDialogFlag

#if defined (DataDoctorResizingDialogFlag)
   #define CDataDoctorPropertySheetType CResizingPropertySheet
   #define CDataDoctorPropertyPageType  CResizingPropertyPage
#else
   #define CDataDoctorPropertySheetType CPropertySheet
   #define CDataDoctorPropertyPageType  CPropertyPage
#endif

//.............................................................................

#define ID_GRIDROWEDITFINISHED         (WM_APP + 100)
#define ID_OUTLINE_GRIDROWEDITFINISHED (WM_APP + 101)

class CDataDoctorPropertySheet;

#include "DataDoctor.h"
#include "GerberThermalDfm.h"
#include "ResizingPropertySheet.h"
#include "RegularExpression.h"
#include "afxwin.h"
#include "DialogToolbar.h"

// Previous includes cause this to be defined but out Ultimate Grid is not built with OLE enabled
// so having this symbol defined before this include causes three undefined references at link time.
// It is not previous includes right here necessarily, it can be previous includes where this .h gets included.
#undef __AFXOLE_H__
/////#include "UGCtrl.h"
#include "UltimateGrid.h"

// Please help keep these in alphabetical order, just for our own ease

#define QBlockType         "Block Type"
#define QCapacitiveOpens   "Cap. Opens"
#define QCentroidRot       "Cent Rot"
#define QCentroidX         "Cent X"
#define QCentroidY         "Cent Y"
#define QDescription			"Description"
#define QDeviceType        "Device Type"
#define QDiodeOpens        "Diode Opens"
#define QDPMO              "DPMO"
#define QElements          "Elements"
#define QErrors            "Errors"
#define QGeometryName      "Geometry Name"
#define QHeight            "Height"
#define QInsertType        "Insert Type"
#define QFromLibrary       "Library"
#define QLoaded            "Loaded"
#define QMapping           "Mapping"
#define QMessage           "Message"
#define QMethod            "Method"
#define QName              "Name"
#define QNet               "Net"
#define QNone              "None"
#define QSearchObjects     "Objects"
#define QOutlineMethod     "Outline Method"
#define QPackageAlias      "Package Alias"
#define QPackageSource     "Package Source"
#define QPin               "Pin"
#define QPins              "Pins"
#define QPinCount          "Pin Count"
#define QPinFunction       "Pin Function"
#define QPinMap				"Pin Mapping"
#define QPinName           "Pin Name"
#define QRefDes            "Ref Des"
#define QRegularExpression "Reg. Exp."
#define QSortableRefDes    "Sortable Ref Des"
#define QStatus            "Status"
#define QSubclass          "Subclass"
#define QValue             "Value"
#define QPTolerance        "+Tolerance"
#define QNTolerance        "-Tolerance"
#define QComments          "Comment"
#define QFamily            "Family"

enum FilterSchemeTag
{
   filterSchemeFiducial,
   filterSchemeTooling,
   filterSchemeFiducialDefault,
   filterSchemeToolingDefault,
   filterSchemeUndefined
};

//_____________________________________________________________________________
class CDataDoctorFilterSchemeTerm
{
private:
   CRegularExpression m_regularExpression;
   CString m_searchFlagsString;
   bool    m_searchRefDesFlag;
   bool    m_searchGeometryNameFlag;
   bool    m_searchPadstackNameFlag;

public:
   CDataDoctorFilterSchemeTerm(const CString& regularExpression,const CString& searchFlags);
   
   CString getRegularExpressionString() const { return m_regularExpression.getRegularExpression(); }
   void setRegularExpression(const CString& regularExpression) { m_regularExpression.setRegularExpression(regularExpression); }

   CString getSearchFlagsString() const { return m_searchFlagsString; }
   void setSearchFlags(const CString& searchFlags);
   bool matches(CCamCadDatabase& camCadDatabase,DataStruct& data);
};

//_____________________________________________________________________________
class CDataDoctorFilterScheme
{
private:
   FilterSchemeTag m_filterSchemeTag;
   CTypedPtrArrayContainer<CDataDoctorFilterSchemeTerm*> m_terms;

public:
   CDataDoctorFilterScheme(FilterSchemeTag filterSchemeTag);
   CDataDoctorFilterScheme& operator=(const CDataDoctorFilterScheme& other);
   void empty();

   void addTerm(const CString& regularExpression,const CString& searchFlags);
   void loadFromRegistry();
   int getTermCount() const { return m_terms.GetCount(); }
   CDataDoctorFilterSchemeTerm* getTermAt(int index) { return m_terms.GetAt(index); }
   bool matches(CCamCadDatabase& camCadDatabase,DataStruct& data);
};

//_____________________________________________________________________________
class CDataDoctorFilterSchemes
{
private:
   CTypedPtrArrayContainer<CDataDoctorFilterScheme*> m_schemes;

public:
   CDataDoctorFilterSchemes();

   CDataDoctorFilterScheme& getScheme(FilterSchemeTag);
};

//__________________________________________________________________________

//class CViewLiteToolbar;

class CViewLiteToolbarHandler
{
private:
   CCEtoODBDoc *m_doc;
   CToolBar *m_myBoss;

public:
   CViewLiteToolbarHandler(CCEtoODBDoc *doc, CToolBar *myBoss) { m_doc = doc; m_myBoss = myBoss; }

   afx_msg void OnBnClickedZoomIn();
   afx_msg void OnBnClickedZoomInOrigin();
   afx_msg void OnBnClickedColorsetsCustom();
   afx_msg void OnBnClickedLayerTypes();
   afx_msg void OnBnClickedShowPolyFills();
   afx_msg void OnBnClickedShowApertureFills();
};

//__________________________________________________________________________

class CViewLiteToolbar : public CResizingPropertyPageToolBar
{
private:
   CCEtoODBDoc *m_doc;
   CViewLiteToolbarHandler m_tbhandler;

public:
   CViewLiteToolbar(CResizingPropertyPage& parentPropPage);

   afx_msg void OnBnClickedZoomIn()             { m_tbhandler.OnBnClickedZoomIn(); }
   afx_msg void OnBnClickedZoomInOrigin()       { m_tbhandler.OnBnClickedZoomInOrigin(); }
   afx_msg void OnBnClickedColorsetsCustom()    { m_tbhandler.OnBnClickedColorsetsCustom(); }
   afx_msg void OnBnClickedLayerTypes()         { m_tbhandler.OnBnClickedLayerTypes(); }
   afx_msg void OnBnClickedShowPolyFills()      { m_tbhandler.OnBnClickedShowPolyFills(); }
   afx_msg void OnBnClickedShowApertureFills()  { m_tbhandler.OnBnClickedShowApertureFills(); }
};


//__________________________________________________________________________

class CViewLiteToolbarAA : public CToolBar
{
private:
   CCEtoODBDoc *m_doc;
   CViewLiteToolbarHandler m_tbhandler;

public:
   CViewLiteToolbarAA();

   afx_msg void OnBnClickedZoomIn()             { m_tbhandler.OnBnClickedZoomIn(); }
   afx_msg void OnBnClickedZoomInOrigin()       { m_tbhandler.OnBnClickedZoomInOrigin(); }
   afx_msg void OnBnClickedColorsetsCustom()    { m_tbhandler.OnBnClickedColorsetsCustom(); }
   afx_msg void OnBnClickedLayerTypes()         { m_tbhandler.OnBnClickedLayerTypes(); }
   afx_msg void OnBnClickedShowPolyFills()      { m_tbhandler.OnBnClickedShowPolyFills(); }
   afx_msg void OnBnClickedShowApertureFills()  { m_tbhandler.OnBnClickedShowApertureFills(); }
};

//__________________________________________________________________________
//
//////////////////////////////////////////////////////////////////////////////
// Filtered CEdit

enum DDNumberType  // Can add more when the need arises
{
   ddUnsignedInteger,
   ddUnsignedFloat,
   ddFloat
};

class CEditNumber : public CEdit
{
private:
   DDNumberType m_numberType;

protected:
  // Generated message map functions
  //{{AFX_MSG(CSomeEdit)
        afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

public:
   CEditNumber()  { m_numberType = ddUnsignedFloat; }

   void SetNumberType(DDNumberType numType)  { m_numberType = numType; }
};

//_____________________________________________________________________________

// These have to be after the base grid class is defined
#include "DataDoctorPageCentroids.h"
#include "DataDoctorPageComps.h"
#include "DataDoctorPageParts.h"
#include "DataDoctorPagePackages.h"
#include "DataDoctorPagePAnalysis.h"
#include "DataDoctorPageSubclasses.h"
#include "DataDoctorPageLibrary.h"



//_____________________________________________________________________________
class CDataDoctorPropertySheet : public CDataDoctorPropertySheetType
{
	DECLARE_DYNAMIC(CDataDoctorPropertySheet)

private:
   CDataDoctor& m_dataDoctor;
   CDataDoctorComponentsPage m_componentsPage;
   CDataDoctorPartsPage m_partsPage;
   CDataDoctorCentroidsPage m_centroidsPage;
   CDataDoctorPackagesPage m_packagesPage;
   CDataDoctorSubclassesPage m_subclassesPage;
   CDataDoctorLibraryPage m_libraryPage;

   DdPageTag m_initialPage;

   CDataDoctorParallelAnalysisPropertyPage m_parallelAnalysisPage;

public:
	CDataDoctorPropertySheet(CDataDoctor& dataDoctor);
	//virtual ~CDataDoctorPropertySheet();

   CDataDoctor& getDataDoctor() { return m_dataDoctor; }
   CCamCadDatabase& getCamCadDatabase() { return getDataDoctor().getCamCadDatabase(); }

   BlockStruct* getPcbBlock() { return getDataDoctor().getPcbBlock(); }
   void updatePropertyPages(CPropertyPage* sendingPage);

   void SetInitialPage(DdPageTag page) { m_initialPage = page; }
   void SaveAsInitialPage(DdPageTag page); // Writes to registry
   DdPageTag LookupInitialPage();  // Lookup in registry
   DdPageTag StringToDdPageTag(CString str);
   CString DdPageTagToString(DdPageTag tag);
   CDataDoctorPropertyPageType *GetInitialPagePtr();

   void EnableDisableDatabaseControls();  // Gets on/off setting from DB itself, updates controls in GUI

   virtual CString GetDialogProfileEntry() { return CString("DataDoctorPropertySheet"); }

private:
   void init();

protected:
	DECLARE_MESSAGE_MAP()

public:
   virtual BOOL OnInitDialog();
   afx_msg void OnClose();
};

#endif
#pragma once


//_____________________________________________________________________________

class CDataDoctorPinMappingDlg;

class CDDPinMapGrid : public CDDBaseGrid
{
private:
   CDataDoctorPinMappingDlg *m_parentDlg;

 public:
    CDDPinMapGrid(CDataDoctorPinMappingDlg *parentDlg)  { m_parentDlg = parentDlg; }
   ~CDDPinMapGrid() { }

   virtual void OnSetup();

	//cell type notifications
	virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);

   //movement
   //virtual void OnCellChange(int oldcol,int newcol,long oldrow,long newrow);

	//editing
	virtual int OnEditStart(int col, long row,CWnd **edit);
	//virtual int OnEditVerify(int col,long row,CWnd *edit,UINT *vcKey);
	//virtual int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);

   // mouse
   //virtual void OnRClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed);

   //column swapping
	virtual void OnColSwapped(int fromCol,int toCol);

   // pin mapping menu options
   void SetMappingOptions(CString options);

};

//
enum AddCompOperationTag
{
   AddCompOperation_OverWrite,
   AddCompOperation_Append,
};
//_____________________________________________________________________________
// CDataDoctorPinMappingDlg dialog
class CDataDoctorPinMappingDlg : public CResizingDialog
{
	DECLARE_DYNAMIC(CDataDoctorPinMappingDlg)

private:
   CCamCadDatabase& m_camCadDatabase;
   CDDPinMapGrid m_pinMappingGrid;
	CString m_pinRefsToPinNamesMapString;
   CString m_unmapped_pin_option_name;
   CComboBox m_ComponentCombox;
   FileStruct *m_pFile;

   CDataDoctorPart *m_ddpart;

	void fillGrid();
   void augmentGrid(bool resizeAndRedraw = true);

   CString getNetCompPinNames(NetStruct *net); // Comma separated list of comppin refs on net

public:
   CDataDoctorPinMappingDlg(CCamCadDatabase& camCadDatabase, CDataDoctorPart *ddpart, CWnd* pParent = NULL);
	//CDataDoctorPinMappingDlg(FileStruct *file, CString sampleRefname, CString pinRefsToPinNamesMapString, CString pinNamesString, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDataDoctorPinMappingDlg();

	enum { IDD = IDD_DataDoctorPinMapDlg };
   
   virtual CString GetDialogProfileEntry() { return CString("DataDoctorPinMappingDialog"); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
   

public:
   int m_colPinRefName;
   int m_colPinMappedName;
   int m_colPinSampleNetName;
   int m_colPinsDescriptor;
   CColumnDefMap m_columnDefMap;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
   afx_msg void OnCbnSelchangeRefName();

   void AfterEditGrid(long row, long col);
   void AddComponents(CComboBox &destItem, CString srcCompName, CString delimiter, AddCompOperationTag operation);

	CString getPinRefsToPinNamesMapString() const { return m_pinRefsToPinNamesMapString; }
};
