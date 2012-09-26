// $Header: /CAMCAD/4.5/LayerLst.h 16    12/18/06 3:46p Rick Faltersack $

#pragma once

#include "ccdoc.h"
#include "ResizingDialog.h"

typedef struct
{
   int            num;
   int            mirror;
   CString        *name;
   CString        *comment;
   unsigned long color;
   int show;
   char           negative;
   char           floating;
   char           worldView;
   double         zheight;
   short          layertype;
   short          artworkstacknumber;
   short          electricalstacknumber;
   short          physicalstacknumber;
   unsigned long  attr;
}TempLayerArray;

/////////////////////////////////////////////////////////////////////////////
// LayerListCtrl window
class LayerListCtrl : public CListCtrl
{
// Construction
public:
   LayerListCtrl();

// Attributes
public:
   CCEtoODBDoc *doc;

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(LayerListCtrl)
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~LayerListCtrl();
   virtual void DrawItem(LPDRAWITEMSTRUCT);

   // Generated message map functions
protected:
   //{{AFX_MSG(LayerListCtrl)
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

enum LayerListColumnTag
{
   LayerListColumnName                  =  0,
   LayerListColumnNumber                =  1,
   LayerListColumnArtworkStackNumber    =  2,
   LayerListColumnElectricalStackNumber =  3,
   LayerListColumnPhysicalStackNumber   =  4,
   LayerListColumnType                  =  5,
   LayerListColumnMirroredLayer         =  6,
   LayerListColumnComment               =  7,
   LayerListColumnUndefined             = -1
};

CString layerListColumnTagToString(LayerListColumnTag tagValue);
LayerListColumnTag stringToLayerListColumnTag(const CString& columnName);
LayerListColumnTag intToLayerListColumnTag(int columnValue);

/////////////////////////////////////////////////////////////////////////////
// LayerListDlg dialog
class LayerListDlg : public CResizingDialog
{
// Construction
public:
   ~LayerListDlg();
   LayerListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(LayerListDlg)
   enum { IDD = IDD_LIST_LAYERS };
   LayerListCtrl  m_list;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   BOOL OnBox;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(LayerListDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("LayerListDialog"); }

public:
   LayerListColumnTag m_sortColumn;
   bool m_descendingSort;

	void ApplyExposedMetalChanges();
   virtual void saveWindowState();
   void restoreColumnWidths();
   void restoreSortOrder();

protected:
	bool m_bExposedMetalTopOn;
	bool m_bExposedMetalBotOn;
	LayerStruct *m_pExposedMetalTopLayer;
	LayerStruct *m_pExposedMetalBotLayer;

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(LayerListDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnEdit();
   afx_msg void OnApply();
   afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnUsedOn();
   afx_msg void OnAllOff();
   afx_msg void OnAllOn();
   afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnAllShow();
   afx_msg void OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
   afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
   afx_msg void OnBringLayerToFront();
   afx_msg void OnSendLayerToBack();
   afx_msg void OnReassignUniqueColors();
};


/////////////////////////////////////////////////////////////////////////////
// CLayerColorButton window
class CLayerColorButton : public CButton
{
// Construction
public:
   CLayerColorButton();

// Attributes
public:

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CLayerColorButton)
   public:
   virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
   //}}AFX_VIRTUAL
   COLORREF tempColor;

// Implementation
public:
   virtual ~CLayerColorButton();

   // Generated message map functions
protected:
   //{{AFX_MSG(CLayerColorButton)
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// MirrorWith dialog
class MirrorWith : public CDialog
{
// Construction
public:
   MirrorWith(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(MirrorWith)
   enum { IDD = IDD_MIRROR };
   CListBox m_listbox;
   //}}AFX_DATA
   int m_layer, m_mirror;
   CCEtoODBDoc *m_doc;

   static int AscendingNameSortFunc(const void *a, const void *b);

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(MirrorWith)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(MirrorWith)
   virtual void OnOK();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// EditLayer dialog
class EditLayer : public CDialog
{
// Construction
public:
   EditLayer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(EditLayer)
   enum { IDD = IDD_EDIT_LAYER };
   CLayerColorButton m_colorButton;
   CComboBox   m_layerTypeCB;
   BOOL  m_floating;
   BOOL  m_negative;
   int      m_artwork;
   int      m_electrical;
   int      m_physical;
   int      m_mirror;
   CString  m_zheight;
   CString  m_layername;
   BOOL  m_worldView;
   CString  m_mirrorLayer;
   BOOL  m_visible;
   BOOL  m_editable;
   CString  m_comment;
   //}}AFX_DATA
   short layertype;
   short layer, mirror;
   CCEtoODBDoc *doc;

public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditLayer)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(EditLayer)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnChangeMirrorLayer();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

