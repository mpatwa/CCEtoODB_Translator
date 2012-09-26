// $Header: /CAMCAD/4.3/TypePin.h 7     8/12/03 9:06p Kurt Van Ness $
        
/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-98. All Rights Reserved.
*/           

#include "ccdoc.h"

/////////////////////////////////////////////////////////////////////////////
// ListTypePin dialog
class ListTypePin : public CDialog
{
// Construction
public:
   ListTypePin(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(ListTypePin)
   enum { IDD = IDD_LIST_TYPE_PIN };
   CTreeCtrl   m_tree;
   CComboBox   m_fileCB;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   CImageList *m_imageList;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ListTypePin)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void FillTree();
   // Generated message map functions
   //{{AFX_MSG(ListTypePin)
   virtual BOOL OnInitDialog();
   afx_msg void OnSelchangeFile();
   afx_msg void OnEdit();
   afx_msg void OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// EditTypePin dialog

class EditTypePin : public CDialog
{
// Construction
public:
   EditTypePin(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(EditTypePin)
   enum { IDD = IDD_EDIT_TYPE_PIN };
   CString  m_physpin;
   CString  m_logipin;
   int      m_gatenr;
   int      m_swap;
   int      m_pintype;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditTypePin)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(EditTypePin)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
