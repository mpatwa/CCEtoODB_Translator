// $Header: /CAMCAD/4.5/CompPin.h 12    10/18/05 3:51p Moustafa Noureddine $

#if !defined(AFX_LISTCOMPPIN_H__3A8F11E1_707E_11D1_BA40_444553540000__INCLUDED_)
#define AFX_LISTCOMPPIN_H__3A8F11E1_707E_11D1_BA40_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ccdoc.h"
#include "ResizingDialog.h"

typedef struct TempStruct
{
   BOOL cp;
   void *voidPtr;
} CPStruct;

/////////////////////////////////////////////////////////////////////////////
// ListCompPin dialog
class ListCompPin : public CResizingDialog
{
// Construction
public:
   ListCompPin(CWnd* pParent = NULL);   // standard constructor
   ~ListCompPin();

// Dialog Data
   //{{AFX_DATA(ListCompPin)
   enum { IDD = IDD_LIST_COMP_PIN };
   CButton  m_netButton;
   CComboBox   m_fileCB;
   CTreeCtrl   m_tree;
   CString  m_comp;
   CString  m_pin;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   CImageList *m_imageList;
   HTREEITEM root, item;
   CString compName, pinName;
   BOOL StartedSearch;
   BOOL Generated;
   BOOL FillingTree;
   NetStruct *lastNet;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ListCompPin)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual void PostNcDestroy();
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("ListCompPinDialog"); }

// Implementation
public:               
   void FillTree();
   void FreeItems();
   void FindFirstCompPin();
   void FindNextCompPin();
   void PanTo(CompPinStruct *cp);
   // Generated message map functions
   //{{AFX_MSG(ListCompPin)
   virtual void OnCancel();
   virtual BOOL OnInitDialog();
   afx_msg void OnEdit();
   afx_msg void OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnAttribs();
   afx_msg void OnSelchangeFile();
   afx_msg void OnFindCompPin();
   afx_msg void OnPan();
   afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnNet();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedClear();
};

/////////////////////////////////////////////////////////////////////////////
// EditCompPin dialog
class EditCompPin : public CDialog
{
// Construction
public:
   EditCompPin(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(EditCompPin)
   enum { IDD = IDD_EDIT_COMPPIN };
   CComboBox   m_blockCB;
   CString  m_x;
   CString  m_y;
   CString  m_angle;
   BOOL  m_visibleBottom;
   BOOL  m_visibleTop;
   BOOL  m_mirrorGraphics;
   BOOL  m_mirrorLayers;
   //}}AFX_DATA
   CompPinStruct *cp;
   int decimals;
   CCEtoODBDoc *doc;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditCompPin)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(EditCompPin)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_LISTCOMPPIN_H__3A8F11E1_707E_11D1_BA40_444553540000__INCLUDED_)
