// $Header: /CAMCAD/4.5/PcbLayer.h 12    4/11/06 6:08p Rick Faltersack $

#if !defined(AFX_MENLAYER_H__D7F00E41_BC00_11D0_BA3E_444553540000__INCLUDED_)
#define AFX_MENLAYER_H__D7F00E41_BC00_11D0_BA3E_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>

struct MentorLayerStruct
{
   int stackNum;        // XRF electrical Number 1..n including power layers
   int layerindex;      // layerindex for CAMCAD.
   int xrf;             // mentor PHYSICAL_ layer
   BOOL on;
   char type;           // S = Signal, D = Document, P = Power
   CString oldName;     // CAMCAD name
   CString newName;     // MENTOR name
};
typedef CTypedPtrArray<CPtrArray, MentorLayerStruct*> CMentorLayerArray;

struct AllegroLayerStruct
{
   int stackNum;        // XRF electrical Number 1..n including power layers
   int layerindex;      // layerindex for CAMCAD.
   BOOL on;
   char type;           // S = Signal, D = Document, P = Power
   CString oldName;     // CAMCAD name
   CString newName;     // new lookup name
};
typedef CTypedPtrArray<CPtrArray, AllegroLayerStruct*> CAllegroLayerArray;

struct CADIFLayerStruct
{
   int stackNum;        // XRF electrical Number 1..n including power layers
   int layerindex;      // layerindex for CAMCAD.
   BOOL on;
   char type;           // S = Signal, D = Document, P = Power
   CString oldName;     // CAMCAD name
   CString newName;     // new lookup name
};
typedef CTypedPtrArray<CPtrArray, CADIFLayerStruct*> CCADIFLayerArray;

struct VB99LayerStruct
{
   int stackNum;        // XRF electrical Number 1..n including power layers
   int layerindex;      // layerindex for CAMCAD.
   LayerStruct* layer;
   BOOL on;
   char type;           // S = Signal, D = Document, P = Power
   CString oldName;     // CAMCAD name
   CString newName;     // new lookup name
};
typedef CTypedPtrArray<CPtrArray, VB99LayerStruct*> CVB99LayerArray;

struct ACCELLayerStruct
{
   int stackNum;        // artworkstackup
   int signr;           // signal number, same for PAD_2 and SIGNAL_2
   int layerindex;      // layerindex for CAMCAD.
   BOOL on;
   char type;           // S = Signal, D = Document, P = Power
   CString oldName;     // CAMCAD name
   CString newName;     // new lookup name
};
typedef CTypedPtrArray<CPtrArray, ACCELLayerStruct*> CACCELLayerArray;

struct ORCADLayerStruct
{
   int stackNum;        // artworkstackup
   int signr;           // signal number, same for PAD_2 and SIGNAL_2
   int layerindex;      // layerindex for CAMCAD.
   BOOL on;
   char type;           // S = Signal, D = Document, P = Power
   CString oldName;     // CAMCAD name
   CString newName;     // new lookup name
   CString nickName;    // needed layernames in orcad 
   CString libName;     // needed layernames
};
typedef CTypedPtrArray<CPtrArray, ORCADLayerStruct*> CORCADLayerArray;

struct DDELayerStruct
{
   int      stackNum;         // dde layer number
   int      signr;            // signal number, same for PAD_2 and SIGNAL_2
   int      layerindex;       // layerindex for CAMCAD.
   BOOL     on;
   char     type;             // S = Signal, D = Document, P = Power
   int      layertype;        // Layer->getLayerType()
   CString  oldName;          // CAMCAD name
   CString  newName;          // new lookup name

   LayerTypeTag getLayerType() const         { return (LayerTypeTag)layertype; }
   void setLayerType(LayerTypeTag layerType) { layertype = layerType; }
   void setLayerType(int layerType)          { layertype = ((layertype >= layerTypeLowerBound && layertype <= layerTypeUpperBound) ? layerType : layerTypeUndefined);; }
};
typedef CTypedPtrArray<CPtrArray, DDELayerStruct*> CDDELayerArray;

struct GenCADLayerStruct
{
   int stackNum;        // XRF electrical Number 1..n including power layers
   int layerindex;      // layerindex for CAMCAD.
   BOOL on;
   char type;           // S = Signal, D = Document, P = Power
   CString oldName;     // CAMCAD name
   CString newName;     // new lookup name
};
typedef CTypedPtrArray<CPtrArray, GenCADLayerStruct*> CGenCADLayerArray;

struct GenCAMLayerStruct
{
   int stackNum;        // XRF electrical Number 1..n including power layers
   int layerindex;      // layerindex for CAMCAD.
   BOOL on;
   char type;           // S = Signal, D = Document, P = Power
   CString oldName;     // CAMCAD name
   CString newName;     // new lookup name
};
typedef CTypedPtrArray<CPtrArray, GenCAMLayerStruct*> CGenCAMLayerArray;

struct FabMasterLayerStruct
{
   int stackNum;        // XRF electrical Number 1..n including power layers
   int layerindex;      // layerindex for CAMCAD.
   BOOL on;
   char type;           // S = Signal, D = Document, P = Power
   CString oldName;     // CAMCAD name
   CString newName;     // new lookup name
};
typedef CTypedPtrArray<CPtrArray, FabMasterLayerStruct*> CFabMasterLayerArray;

struct ProtelLayerStruct
{
   int stackNum;        // artworkstackup
   int signr;           // signal number, same for PAD_2 and SIGNAL_2
   int layerindex;      // layerindex for CAMCAD.
   BOOL on;
   char type;           // S = Signal, D = Document, P = Power
   CString oldName;     // CAMCAD name
   CString newName;     // new PADS stacknumber 1..30 lookup name
};
typedef CTypedPtrArray<CPtrArray, ProtelLayerStruct*> CProtelLayerArray;

struct PADSLayerStruct
{
   int stackNum;        // artworkstackup
   int signr;           // signal number, same for PAD_2 and SIGNAL_2
   int layerindex;      // layerindex for CAMCAD.
   BOOL on;
   char type;           // S = Signal, D = Document, P = Power
   CString oldName;     // CAMCAD name
   CString newName;     // new lookup name
};
typedef CTypedPtrArray<CPtrArray, PADSLayerStruct*> CPADSLayerArray;

/////////////////////////////////////////////////////////////////////////////
// MentorLayerDlg dialog
class MentorLayerDlg : public CDialog
{
// Construction
public:
   char cwd[_MAX_PATH];
   MentorLayerDlg(CWnd* pParent = NULL);   // standard constructor
   ~MentorLayerDlg();

   CMentorLayerArray *arr;
   int maxArr;
   int selItem;
   int sortBy;
   BOOL reverse;

// Dialog Data
   //{{AFX_DATA(MentorLayerDlg)
   enum { IDD = IDD_MENTOR_LAYER };
   CListCtrl   m_list;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(MentorLayerDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
   void FillListCtrl();

protected:

   // Generated message map functions
   //{{AFX_MSG(MentorLayerDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnEdit();
   afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnSave();
   afx_msg void OnLoad();
   afx_msg void OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// MentorLayerEdit dialog
class MentorLayerEdit : public CDialog
{
// Construction
public:
   MentorLayerEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(MentorLayerEdit)
   enum { IDD = IDD_MENTOR_LAYER_EDIT };
   CComboBox   m_stacknumCB;
   BOOL  m_check;
   CString  m_new;
   CString  m_old;
   CString  m_type;
   CString  m_stacknum;
   int      m_xrf;
   //}}AFX_DATA
   int stacknum;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(MentorLayerEdit)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(MentorLayerEdit)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// AllegroLayer dialog
class AllegroLayer : public CDialog
{
// Construction
public:
   AllegroLayer(CWnd* pParent = NULL);   // standard constructor
   ~AllegroLayer();

   char cwd[_MAX_PATH];
   CAllegroLayerArray *arr;
   int maxArr;
   int selItem;

// Dialog Data
   //{{AFX_DATA(AllegroLayer)
   enum { IDD = IDD_ALLEGRO_LAYER };
   CListCtrl   m_list;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(AllegroLayer)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
   void FillListCtrl();

protected:

   // Generated message map functions
   //{{AFX_MSG(AllegroLayer)
   virtual BOOL OnInitDialog();
   afx_msg void OnEdit();
   afx_msg void OnLoad1();
   afx_msg void OnSave1();
   afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// AllegroLayerEdit dialog
class AllegroLayerEdit : public CDialog
{
// Construction
public:
   AllegroLayerEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(AllegroLayerEdit)
   enum { IDD = IDD_ALLEGRO_EDIT_DLG };
   CComboBox   m_stacknumCB;
   BOOL  m_check;
   CString  m_new;
   CString  m_old;
   CString  m_stacknum;
   CString  m_type;
   //}}AFX_DATA
   int stacknum;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(AllegroLayerEdit)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(AllegroLayerEdit)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// GenCADLayerDlg dialog
class GenCADLayerDlg : public CDialog
{
// Construction
public:
   char cwd[_MAX_PATH];
   GenCADLayerDlg(CWnd* pParent = NULL);   // standard constructor
   ~GenCADLayerDlg();

   CGenCADLayerArray *arr;
   int maxArr;
   int selItem;

// Dialog Data
   //{{AFX_DATA(GenCADLayerDlg)
   enum { IDD = IDD_GENCAD_LAYER };
   CListCtrl   m_list;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(GenCADLayerDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
   void FillListCtrl();

protected:

   // Generated message map functions
   //{{AFX_MSG(GenCADLayerDlg)
   afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnEdit();
   afx_msg void OnLoad();
   afx_msg void OnSave();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// GenCADLayerEdit dialog
class GenCADLayerEdit : public CDialog
{
// Construction
public:
   GenCADLayerEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(GenCADLayerEdit)
   enum { IDD = IDD_GENCAD_LAYER_EDIT };
   CComboBox   m_stacknumCB;
   BOOL  m_check;
   CString  m_new;
   CString  m_old;
   CString  m_stacknum;
   CString  m_type;
   //}}AFX_DATA
   int stacknum;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(GenCADLayerEdit)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(GenCADLayerEdit)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// GenCAMLayerDlg dialog
class GenCAMLayerDlg : public CDialog
{
// Construction
public:
   char cwd[_MAX_PATH];
   GenCAMLayerDlg(CWnd* pParent = NULL);   // standard constructor
   ~GenCAMLayerDlg();

   CGenCAMLayerArray *arr;
   int maxArr;
   int selItem;

// Dialog Data
   //{{AFX_DATA(GenCAMLayerDlg)
   enum { IDD = IDD_GENCAM_LAYER };
   CListCtrl   m_list;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(GenCAMLayerDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
   void FillListCtrl();

protected:

   // Generated message map functions
   //{{AFX_MSG(GenCAMLayerDlg)
   afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnEdit();
   afx_msg void OnLoad();
   afx_msg void OnSave();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// GenCAMLayerEdit dialog
class GenCAMLayerEdit : public CDialog
{
// Construction
public:
   GenCAMLayerEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(GenCAMLayerEdit)
   enum { IDD = IDD_GENCAM_LAYER_EDIT };
   CComboBox   m_stacknumCB;
   BOOL  m_check;
   CString  m_new;
   CString  m_old;
   CString  m_stacknum;
   CString  m_type;
   //}}AFX_DATA
   int stacknum;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(GenCAMLayerEdit)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(GenCAMLayerEdit)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// FabMasterLayerDlg dialog
class FabMasterLayerDlg : public CDialog
{
// Construction
public:
   char cwd[_MAX_PATH];
   FabMasterLayerDlg(CWnd* pParent = NULL);   // standard constructor
   ~FabMasterLayerDlg();

   CFabMasterLayerArray *arr;
   int maxArr;
   int selItem;

// Dialog Data
   //{{AFX_DATA(FabMasterLayerDlg)
   enum { IDD = IDD_FABMASTER_LAYER };
   CListCtrl   m_list;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FabMasterLayerDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
   void FillListCtrl();

protected:

   // Generated message map functions
   //{{AFX_MSG(FabMasterLayerDlg)
   afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnEdit();
   afx_msg void OnLoad();
   afx_msg void OnSave();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// FabMasterLayerEdit dialog
class FabMasterLayerEdit : public CDialog
{
// Construction
public:
   FabMasterLayerEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FabMasterLayerEdit)
   enum { IDD = IDD_FABMASTER_LAYER_EDIT };
   CComboBox   m_stacknumCB;
   BOOL  m_check;
   CString  m_new;
   CString  m_old;
   CString  m_stacknum;
   CString  m_type;
   //}}AFX_DATA
   int stacknum;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FabMasterLayerEdit)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FabMasterLayerEdit)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// VB99Layer dialog
class VB99Layer : public CDialog
{
// Construction
public:
   VB99Layer(CWnd* pParent = NULL);   // standard constructor
   ~VB99Layer();

   char cwd[_MAX_PATH];
   CVB99LayerArray *arr;
   int maxArr;
   int selItem;

// Dialog Data
   //{{AFX_DATA(VB99Layer)
   enum { IDD = IDD_VB99_LAYER };
   CListCtrl   m_list;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(VB99Layer)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
   void FillListCtrl();

protected:

   // Generated message map functions
   //{{AFX_MSG(VB99Layer)
   virtual BOOL OnInitDialog();
   afx_msg void OnEdit();
   afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnSave();
   afx_msg void OnLoad();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CADIFLayer dialog
class CADIFLayer : public CDialog
{
// Construction
public:
   CADIFLayer(CWnd* pParent = NULL);   // standard constructor
   ~CADIFLayer();

// Dialog Data
   //{{AFX_DATA(CADIFLayer)
   enum { IDD = IDD_CADIF_LAYER };
   CListCtrl   m_list;
   CString  m_technologyName;
   //}}AFX_DATA
   char cwd[_MAX_PATH];
   CCADIFLayerArray *arr;
   int maxArr;
   int selItem;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CADIFLayer)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
   void FillListCtrl();

protected:

   // Generated message map functions
   //{{AFX_MSG(CADIFLayer)
   afx_msg void OnLoadTechnology();
   afx_msg void OnEdit();
   afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnLoad1();
   afx_msg void OnSave1();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CADIFLayerEditDlg dialog
class CADIFLayerEditDlg : public CDialog
{
// Construction
public:
   CADIFLayerEditDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(CADIFLayerEditDlg)
   enum { IDD = IDD_CADIF_LAYER_EDIT };
   CComboBox   m_stacknumCB;
   int      m_xrf;
   CString  m_type;
   CString  m_old;
   CString  m_new;
   BOOL  m_check;
   CString  m_stacknum;
   //}}AFX_DATA
   int stacknum;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CADIFLayerEditDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CADIFLayerEditDlg)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// ProtelLayerDlg dialog
class ProtelLayerDlg : public CDialog
{
// Construction
public:
   ProtelLayerDlg(CWnd* pParent = NULL);   // standard constructor
   ~ProtelLayerDlg();

   char cwd[_MAX_PATH];
   CProtelLayerArray *arr;
   int maxArr;
   int selItem;

// Dialog Data
   //{{AFX_DATA(ProtelLayerDlg)
   enum { IDD = IDD_PROTEL_LAYER };
   CListCtrl   m_list;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ProtelLayerDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
   void FillListCtrl();
protected:

   // Generated message map functions
   //{{AFX_MSG(ProtelLayerDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnEdit();
   afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnLoad1();
   afx_msg void OnSave1();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// ProtelLayerEdit dialog
class ProtelLayerEdit : public CDialog
{
// Construction
public:
   ProtelLayerEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(ProtelLayerEdit)
   enum { IDD = IDD_PROTEL_LAYER_EDIT };
   CComboBox   m_newCB;
   CComboBox   m_stacknumCB;
   BOOL  m_check;
   CString  m_old;
   CString  m_type;
   CString  m_stacknum;
   //}}AFX_DATA
   int stacknum;
   CString newname;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ProtelLayerEdit)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(ProtelLayerEdit)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// PadsLayerDlg dialog
class PadsLayerDlg : public CDialog
{
// Construction
public:
   PadsLayerDlg(CWnd* pParent = NULL);   // standard constructor
   ~PadsLayerDlg();

   char cwd[_MAX_PATH];
   CPADSLayerArray *arr;
   int maxArr;
   int selItem;

// Dialog Data
   //{{AFX_DATA(PadsLayerDlg)
   enum { IDD = IDD_PADS_LAYER };
   CListCtrl   m_list;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(PadsLayerDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
   void FillListCtrl();
protected:

   // Generated message map functions
   //{{AFX_MSG(PadsLayerDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnEdit();
   afx_msg void OnLoad();
   afx_msg void OnSave();
   afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// PADSLayerEdit dialog
class PADSLayerEdit : public CDialog
{
// Construction
public:
   PADSLayerEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(PADSLayerEdit)
   enum { IDD = IDD_PADS_LAYER_EDIT };
   CComboBox   m_stacknumCB;
   CComboBox   m_newCB;
   BOOL  m_check;
   CString  m_old;
   CString  m_stacknum;
   CString  m_type;
   //}}AFX_DATA
   int stacknum;
   CString newname;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(PADSLayerEdit)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(PADSLayerEdit)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// DDELayer dialog
class DDELayer : public CDialog
{
// Construction
public:
   DDELayer(CWnd* pParent = NULL);   // standard constructor
   ~DDELayer();

   char cwd[_MAX_PATH];
   CDDELayerArray *arr;
   int maxArr;
   int selItem;

// Dialog Data
   //{{AFX_DATA(DDELayer)
   enum { IDD = IDD_DDE_LAYER };
   CListCtrl   m_list;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(DDELayer)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
   void FillListCtrl();

protected:

   // Generated message map functions
   //{{AFX_MSG(DDELayer)
   afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnEdit();
   afx_msg void OnLoad1();
   afx_msg void OnSave1();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// DDELayerEdit dialog
class DDELayerEdit : public CDialog
{
// Construction
public:
   DDELayerEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(DDELayerEdit)
   enum { IDD = IDD_DDE_EDIT_DLG };
   CComboBox   m_stacknumCB;
   BOOL  m_check;
   CString  m_new;
   CString  m_old;
   CString  m_type;
   CString  m_stacknum;
   //}}AFX_DATA
   int stacknum;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(DDELayerEdit)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(DDELayerEdit)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

/////////////////////////////////////////////////////////////////////////////
// VB99LayerEdit dialog
class VB99LayerEdit : public CDialog
{
// Construction
public:
   VB99LayerEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(VB99LayerEdit)
   enum { IDD = IDD_VB99_EDIT_DLG };
   CComboBox   m_stacknumCB;
   BOOL  m_check;
   CString  m_new;
   CString  m_old;
   CString  m_stacknum;
   CString  m_type;
   //}}AFX_DATA
   int stacknum;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(VB99LayerEdit)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(VB99LayerEdit)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// ACCELLayer dialog
class ACCELLayer : public CDialog
{
// Construction
public:
   ACCELLayer(CWnd* pParent = NULL);   // standard constructor
   ~ACCELLayer();

   char cwd[_MAX_PATH];
   CACCELLayerArray *arr;
   int maxArr;
   int selItem;

// Dialog Data
   //{{AFX_DATA(ACCELLayer)
   enum { IDD = IDD_ACCEL_LAYER };
   CListCtrl   m_list;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ACCELLayer)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
   void FillListCtrl();

protected:

   // Generated message map functions
   //{{AFX_MSG(ACCELLayer)
   virtual BOOL OnInitDialog();
   afx_msg void OnEdit();
   afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnSave();
   afx_msg void OnLoad();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// ACCELLayerEdit dialog
class ACCELLayerEdit : public CDialog
{
// Construction
public:
   ACCELLayerEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(ACCELLayerEdit)
   enum { IDD = IDD_ACCEL_EDIT_DLG };
   CComboBox   m_stacknumCB;
   BOOL  m_check;
   CString  m_new;
   CString  m_old;
   CString  m_stacknum;
   CString  m_type;
   //}}AFX_DATA
   int stacknum;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ACCELLayerEdit)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(ACCELLayerEdit)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// ORCADLayer dialog
class ORCADLayer : public CDialog
{
// Construction
public:
   ORCADLayer(CWnd* pParent = NULL);   // standard constructor
   ~ORCADLayer();

   char cwd[_MAX_PATH];
   CORCADLayerArray *arr;
   int maxArr;
   int selItem;

// Dialog Data
   //{{AFX_DATA(ORCADLayer)
   enum { IDD = IDD_ORCAD_LAYER };
   CListCtrl   m_list;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ORCADLayer)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
   void FillListCtrl();

protected:

   // Generated message map functions
   //{{AFX_MSG(ORCADLayer)
   virtual BOOL OnInitDialog();
   afx_msg void OnEdit();
   afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnSave();
   afx_msg void OnLoad();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

/////////////////////////////////////////////////////////////////////////////
// ACCELLayerEdit dialog
class ORCADLayerEdit : public CDialog
{
// Construction
public:
   ORCADLayerEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(ORCADLayerEdit)
   enum { IDD = IDD_ORCAD_EDIT_DLG };
   CComboBox   m_stacknumCB;
   BOOL  m_check;
   CString  m_new;
   CString  m_old;
   CString  m_stacknum;
   CString  m_type;
   CString  m_nickname;
   CString  m_libname;
   //}}AFX_DATA
   int stacknum;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ORCADLayerEdit)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(ORCADLayerEdit)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//--------------------------------------------------------------------
// This is a utility for loading .LAYERATTR and .MIRRORLAYER settings from
// any settings file and applying them. The settings file name is passed in to
// the constructor. The app using this then calls the Apply() method.
// E.g. from GenCamIn.cpp
//   assign_layer(); // something local in reader
//   CLayerSettings layerSettings(s_genCamReader->getSettingsFilename());
//   layerSettings.Apply(doc);
//   doc->RemoveUnusedLayers(); // something else local in the reader

class CLayerSettings
{
private:
	CString m_settingsFileName;
	CMapStringToString m_layerTypeMap;
	CMapStringToString m_mirrorMap;

	void LoadSettings();

public:
	CLayerSettings(CString settingsFileName);
	void Apply(CCEtoODBDoc *doc);

};

//--------------------------------------------------------------------

#endif // !defined(AFX_MENLAYER_H__D7F00E41_BC00_11D0_BA3E_444553540000__INCLUDED_)
