

#if !defined(__CAMCADNavigator_h__)
#define __CAMCADNavigator_h__

#include "afxcmn.h"
#include "afxwin.h"
#include "Horz_lb.h"
#include "PrepDlg.h"

#define ICON_FOLDER          0
#define ICON_OPENFOLDER      1
#define ICON_FILE            2
#define ICON_NET             3
#define ICON_CP              4
#define ICON_CPA             5
#define ICON_COMP            6
#define ICON_VIA             7
#define ICON_PLCD				  8
#define ICON_UNPLCD          9
#define ICON_ACCESS          10
#define ICON_PIN             11
#define ICON_FID             12
#define ICON_TOOLING         13
#define ICON_TESTPOINT       14
#define ICON_DIE             15


extern SettingsStruct GlSettings;      // from CAMCAD.CPP

typedef LPARAM(*PFNMTICOPYDATA)(const CTreeCtrl&, HTREEITEM, LPARAM);

#define HighlightColorCount 5
#define QMACHINESIDE_TOP "_T"
#define QMACHINESIDE_BOT "_B"

class CCamCadPin;
class CCamCadPinMap;

//-----------------------------------------------------
// The order here is not arbitrary. These must be kept
// in sync with "tab order" of the nav mode radio buttons.
enum NavigatorModeTag
{
   navigatorModeComponents        = 0,
   navigatorModeManufacturing     = 1,
   navigatorModeNets              = 2,
   navigatorModeNetsWithAccess    = 3,
   navigatorModeNetsWithoutAccess = 4,
   navigatorModePlacedProbes      = 5,
   navigatorModeUnplacedProbes    = 6,
   navigatorModeBoards            = 7,

};
//-----------------------------------------------------

enum FlippingTag
{
   flipNot,
   flipToTop,
   flipToBottom
};

///////////////////////////////////////////////////////////////////////////
// CExtendComboBox
/////////////////////////////////////////////////////////////////////////////
class CExtendedComboBox : public CComboBox
{
public:
   void ClearAll()
   {
      for(int itemCnt = GetCount(); itemCnt > 0 ; itemCnt --)
      {
         DeleteString(0);  
         DeleteItem(0);
      }
      SetWindowText("");
      Clear();
   };
};

//_____________________________________________________________________________
class CAMCADNavigator : public CDialogBar
{
	DECLARE_DYNAMIC(CAMCADNavigator)

public:
	CAMCADNavigator(CWnd* pParent = NULL);   // standard constructor
	CAMCADNavigator(CCEtoODBDoc *document, CWnd* pParent = NULL);
	virtual ~CAMCADNavigator();
	
// Dialog Data
	enum { IDD = CG_IDD_NAVIGATOR };

protected:
	CImageList*     m_pDragImage;
	BOOL            m_bLDragging;
	HTREEITEM       m_hitemDrag,m_hitemDrop;
	HCURSOR         m_dropCursor,m_noDropCursor;
	BOOL            IsDropSource(HTREEITEM hItem);
	HTREEITEM	    GetDropTarget(HTREEITEM hItem);
	UINT    m_nTimerID;
	UINT    m_timerticks;
	HICON m_hIcon;
	HTREEITEM m_hItemDrag;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
   CCEtoODBDoc *m_doc;
	FileStruct* m_curFile;
	CEdit m_findText;
	CImageList *m_imageList;
	CTreeCtrl m_tree;
   CHorzListBox m_list;
   CSliderCtrl m_zoomSlider;
   int m_curZoomMargin;
   int m_maxZoomMarginSetting;
	CString m_sVariantName;
	CString m_sMachineName;
	CTypedMapStringToPtrContainer<NetStruct*> m_netLookupMap;
   CExtendedComboBox m_machineSide;

	CPtrArray netsWithAccess;

	//CMapStringToPtr compPinNetArray;
   CCamCadPinMap* m_camCadPinMap;
	BOOL docNotPCB;
	DataStruct* pastedProbe;
	CString GetNetNameByCompPin(const CString& pinRef);
   CCamCadPin* getCamCadPin(const CString& pinRef);
   CompPinStruct* getCompPin(const CString& pinRef);
	WORD PROBE_PLACEMENT;
	WORD DATALINK;
	WORD testResKW;

   int m_boardCount;
	int compCount;
   int m_manfCount;
   int netCount;
	int netsWithAccessCnt;
	int netsWithNoAccessCnt;
	int placedProbesCount;
	int unplacedProbesCount;

	int m_netMode;
	NavigatorModeTag m_compMode;
	int nIndexNet;
	int nIndexComp;
   HTREEITEM m_RightClickItem;

   CDataList m_components;
   CDataList m_placedProbes;
   CDataList m_unplacedProbes;
   CNetStructList m_nets;

	void OnInit();
   void Enable(bool flag);
	void SetCompPinNetArray();
	void FillNetsWithAccess();
	void FillNetsWithNoAccess();
	void DeleteTreeItemDatasMem();
	void enableAndFillVariantList();
	void enableAndFillMachineList();

   HTREEITEM FindTreeSiblingItem(HTREEITEM root, CString itemName);
   HTREEITEM FindTreeSiblingItem(HTREEITEM root, void *itemData);

	void Reset();
	BOOL IsNetAccessible(NetStruct *net);
	void UpdateNetMap(CCEtoODBDoc *newDoc);

	//gets and sets
	int  getNetCount()       { return netCount; };

	//functions
   void ResetCounts();
   void UpdateCountsDisplay();
   void FillBoardList(FileStruct *panelFile);
	void FillCompList(FileStruct *pcbFile);
   void FillManufactList(FileStruct *pcbFile);
   int  FillInsertList(FileStruct *pcbFile, CInsertTypeMask insertTypes);
	void FillNetList();
	void FillPlacedProbes(FileStruct *pcbFile);
	void FillUnplacedProbes(FileStruct *pcbFile);
   void ExpandTree();
   void LoadBasicNetTree(NetStruct *net);
	void LoadProbeTree(NetStruct *net);
	bool GetPlacedProbesZoomRegion(double *left, double *right, double *top, double *bottom);
	bool GetUnplacedProbesZoomRegion(double *left, double *right, double *top, double *bottom);
   void SelectBoard(DataStruct* data);
	void SelectComponent(DataStruct* data);
	void SelectNet(NetStruct* net);
	void UnHighLightNets();
	void Show_selected_only();
   void highlightNet(NetStruct& net,COLORREF color);
   void highlightNet(const CString& netName,COLORREF color);
   void clearHighlights();
	void OnEndDrag(UINT nFlags, CPoint point);
	void FinishDragging(BOOL bDraggingImageList);
	void OnCancelMode();
	void SortList(CMapStringToPtr& temp);
	int ShowSelectedOnly;
	void OnTimer(UINT nIDEvent);
	HCURSOR OnQueryDragIcon();
   bool isWindow() const { return IsWindow(m_hWnd) != 0; }
   bool MoveToBoardOrigin(ExportFileSettings& PCBLocation, bool bottomSide);
   void RemoveNonExistCustomMachines(CCEtoODBDoc *doc, FileStruct *pcbFile);

   void ApplyMargin(double margin, double *xmin, double *xmax, double *ymin, double *ymax);
   void ApplyMargin(int    margin, double *xmin, double *xmax, double *ymin, double *ymax);
   void ZoomPanTo(double ZmLeftX, double ZmRightX, double ZmBotY, double ZmTopY, double PnX, double PnY, FlippingTag flipToSide, bool ShowCross = false); 
   void ZoomPanTo(double ZmLeftX, double ZmRightX, double ZmBotY, double ZmTopY, InsertStruct *PnInsert = NULL, bool ShowCross = false);
   void ZoomPanTo(double ZmLeftX, double ZmRightX, double ZmBotY, double ZmTopY, CPoint2d *PanPt, bool mirrored, bool ShowCross = false, bool CancelFlip = false); // Zm..X and Zm..Y are non-standard extents
   
   void ZoomPanToComponent(DataStruct *data);
   void ZoomPanToPin(DataStruct *componentData, DataStruct *pinData);
   
   void GetDefaultZoomRegion(double x, double y, double *xmin, double *xmax, double *ymin, double *ymax);
   void GetZoomRegion(double x, double y, InsertStruct *insert, double *xmin, double *xmax, double *ymin, double *ymax); // e.g. for pin inserts in components
   void GetZoomRegion(InsertStruct *insert,  double *xmin, double *xmax, double *ymin, double *ymax); // e.g. for vias
   void GetZoomRegion(CompPinStruct *comppin,  double *xmin, double *xmax, double *ymin, double *ymax);

   DataStruct *FindComponentData(CString refname);
   DataStruct *FindComponentPinData(DataStruct *pcbComp, CString pinName);

   //message handlers	
	afx_msg void OnLbnSelchangeList();
	afx_msg void OnBnClickedShowSelected();
	afx_msg void OnBnClickedRadioUnplacedProbes();
	afx_msg void OnBnClickedRadioPlacedProbes();
	afx_msg void OnTvnBegindragTreeNavigator(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTvnSelchangedNavigatorTree(NMHDR *pNMHDR, LRESULT *pResult);
   afx_msg void OnBnClickedRadioBoardmode();
	afx_msg void OnBnClickedNetMode();
	afx_msg void OnBnClickedCompMode();
   afx_msg void OnBnClickedManufactMode();
	afx_msg void OnNMRclickTreeNavigator(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedNetsaccess();
	afx_msg void OnBnClickedNetsnoaccess();
   afx_msg void OnNMReleasedcaptureZoomslider(NMHDR *pNMHDR, LRESULT *pResult);

public:

	//gets and sets
	CTreeCtrl* getTree()         { return &m_tree; }
	CCEtoODBDoc* getDoc()         { return m_doc;}
   CCamCadData& getCamCadData() { return m_doc->getCamCadData(); }
	FileStruct* getCurFile()     { return m_curFile;}
	BOOL getDocNotPCB()          { return docNotPCB;}

   // Settings in Nav OPTIONS act globally, attempt to keep local settings in sync
   bool GetPanning()           { return !(m_doc->getSettings().navigatorZoomMode = GlSettings.navigatorZoomMode); }
	void SetPanning(bool pan)   {          m_doc->getSettings().navigatorZoomMode = GlSettings.navigatorZoomMode = !pan; } // ZoomMode aka Zooming
   bool GetFlip()              { return  (m_doc->getSettings().navigatorFlipView = GlSettings.navigatorFlipView); }
   void SetFlip(bool flip)     {          m_doc->getSettings().navigatorFlipView = GlSettings.navigatorFlipView = flip; }
   bool GetShowSide()          { return  (m_doc->getSettings().navigatorShowSide = GlSettings.navigatorShowSide);       }
   void SetShowSide(bool show) {          m_doc->getSettings().navigatorShowSide = GlSettings.navigatorShowSide = show; }
   bool GetShowType()          { return  (m_doc->getSettings().navigatorShowType = GlSettings.navigatorShowType);       }
   void SetShowType(bool show) {          m_doc->getSettings().navigatorShowType = GlSettings.navigatorShowType = show; }

	void setDoc(CCEtoODBDoc *document);
	COLORREF highlightColors[HighlightColorCount];

   CString GetWhoAndWhen();  // returns string with window's user name and date/time stamp, for annotating when a change is made, e.g. as an attribute value

	//functions
	void UpdateProbes(CCEtoODBDoc *document);
	void CountNetsWithAccess(CCEtoODBDoc* document);
	void SetTreeItemData(CTreeCtrl *treeview, HTREEITEM treeItem, CString itemText, int itemType, void *voidPtr);	
   void FillMachineList(CCEtoODBDoc *m_doc, FileStruct *pcbFile);
   bool ApplyPrepSettingToMachine();
   //bool ApplyDFTSolutionToMachine(CMachine *machine);

   //message handlers	
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonFind();
	afx_msg void OnBnClickedBtnOptions();
	afx_msg void OnBnClickedButtonVariantChange();
	afx_msg void OnBnClickedButtonMachineChange();
	afx_msg void OnCmdDeleteProbe();
	afx_msg void OnCmdUnplaceProbe();
	//afx_msg void OnCreateProbe();
	afx_msg void OnEditProbe();
   afx_msg void OnHighlightNet();
   afx_msg void OnCbnSelchangeMachineList();
};

////////////////////////////////////////////////////////////
// CEditProbe dialog

class CEditProbe : public CDialog
{
	DECLARE_DYNAMIC(CEditProbe)

public:
	CEditProbe(CWnd* pParent = NULL);   // standard constructor
	CEditProbe(CCEtoODBDoc *document, CWnd* pParent = NULL);
	virtual ~CEditProbe();

// Dialog Data
	enum { IDD = IDD_PROBE_EDIT };

public:
   CCEtoODBDoc*  getCamCadDoc()  { return m_doc; }
   CCamCadData& getCamCadData() { return m_doc->getCamCadData(); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CCEtoODBDoc *m_doc;

	DECLARE_MESSAGE_MAP()

public:
	const char* m_size;
	int m_side;
	int m_type;
	//virtual BOOL OnInitDialog();
	//void    UpdateSizeList();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	CListBox m_sizeList;
	afx_msg void OnBnClickedRadioSidetop();
	afx_msg void OnBnClickedRadioSidebot();
};


////////////////////////////////////////////////////////////
// CNavigatorOptions dialog

class CNavigatorOptions : public CDialog
{
	DECLARE_DYNAMIC(CNavigatorOptions)

public:
	CNavigatorOptions(CWnd* pParent = NULL);   // standard constructor
	CNavigatorOptions(CCEtoODBDoc *document, CWnd* pParent =NULL);
	virtual ~CNavigatorOptions();

// Dialog Data
	enum { IDD = IDD_NAVIGATOR_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CCEtoODBDoc *m_doc;
	int m_Pan;
   BOOL m_Flip;
   BOOL m_ShowSide;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
   
};
#endif