// $Header: /CAMCAD/4.3/ApList.h 8     8/12/03 9:05p Kurt Van Ness $

#include "ccdoc.h"
#include "ResizingDialog.h"

/////////////////////////////////////////////////////////////////////////////
// DListCtrl window

class DListCtrl : public CListCtrl
{
public:  
   DListCtrl();
   virtual ~DListCtrl();

   int decimals;
   CCEtoODBDoc *doc;

protected:
   //{{AFX_MSG(DListCtrl)
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CApertureListDialog dialog

class CApertureListDialog : public CResizingDialog
{
// Construction
public:
   CApertureListDialog(CCEtoODBDoc *Doc, CWnd* pParent = NULL);   // standard constructor
   ~CApertureListDialog();

// Dialog Data
   //{{AFX_DATA(CApertureListDialog)
   enum { IDD = IDD_LIST_APERTURE };
   DListCtrl   m_listCtrl;
   CString  m_units;
   //}}AFX_DATA

   CCEtoODBDoc *doc;
   int decimals;

   CImageList *m_imageList;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CApertureListDialog)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("ApertureListDialog"); }

// Implementation
protected:
   void FillListCtrl();

   // Generated message map functions
   //{{AFX_MSG(CApertureListDialog)
   virtual BOOL OnInitDialog();
   afx_msg void OnSetDcodes();
   afx_msg void OnEdit();
   afx_msg void OnHelp();
   afx_msg void OnPrint();
   afx_msg void OnAdd();
   afx_msg void OnSquaresToRects();
   afx_msg void OnRectsToSquares();
   afx_msg void OnUndefine();
   afx_msg void OnClearDcodes();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CEditAperture dialog

class CEditAperture : public CDialog
{
// Construction
public:
   CEditAperture(CWnd *pParent = NULL);   // standard constructor
   ~CEditAperture();

// Dialog Data
   //{{AFX_DATA(CEditAperture)
   enum { IDD = IDD_EDIT_APERTURE };
   CComboBox   m_blockCB;
   CComboBox   m_ShapeCB;
   int      m_DCode;
   CString  m_Name;
   CString  m_sizeA;
   CString  m_sizeB;
   CString  m_rotation;
   CString  m_xoffset;
   CString  m_yoffset;
   CString  m_sizeC;
   CString  m_sizeD;
   CString  m_spokes;
   //}}AFX_DATA
   int m_shape;
   int m_blockNum;
   CCEtoODBDoc *doc;
   CToolTipCtrl tooltip;
   CString hint;
   

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CEditAperture)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CEditAperture)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnSelchangeShapeCb();
   afx_msg void OnHelp();
   afx_msg BOOL UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
