
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-6. All Rights Reserved.
*/           

// THIS IS A THROW AWAY - EXPLORING USE OF BRANCHES IN CLEARCASE - RCF
// SECOND THROW AWAY

#include "ccview.h"

/////////////////////////////////////////////////////////////////////////////
// AddTextDlg dialog
class AddTextDlg : public CResizingDialog
{
// Construction
public:
   AddTextDlg(CWnd* pParent = NULL);   // standard constructor
   ~AddTextDlg();

// Dialog Data
   //{{AFX_DATA(AddTextDlg)
   enum { IDD = IDD_ADD_TEXT };
   CString  m_text;
   //}}AFX_DATA
   CCEtoODBView *view;

   virtual CString GetDialogProfileEntry() { return "AddTextDlg"; }

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(AddTextDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(AddTextDlg)
   virtual void OnOK();
   virtual void OnCancel();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

// AddInsertDlg dialog
class AddInsertDlg : public CResizingDialog
{
// Construction
public:
   AddInsertDlg(CWnd* pParent = NULL);   // standard constructor
   ~AddInsertDlg();

// Dialog Data
   //{{AFX_DATA(AddInsertDlg)
   enum { IDD = IDD_ADD_INSERT };
   CComboBox   m_fileCB;
   CTreeCtrl   m_tree;
   //}}AFX_DATA
   BlockStruct *block;
   CCEtoODBDoc *doc;
   int filenum;
   CImageList *m_imageList;

   virtual CString GetDialogProfileEntry() { return "AddInsertDlg"; }

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(AddInsertDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void FillTree();

   // Generated message map functions
   //{{AFX_MSG(AddInsertDlg)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnSelchangeFilecb();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// AddLayer dialog
class AddLayer : public CResizingDialog
{
// Construction
public:
   AddLayer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(AddLayer)
   enum { IDD = IDD_ADD_LAYER };
   CString  m_layer;
   //}}AFX_DATA

   virtual CString GetDialogProfileEntry() { return "AddLayer"; }

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
};

/////////////////////////////////////////////////////////////////////////////
// AddFile dialog
class AddFile : public CResizingDialog
{
// Construction
public:
   AddFile(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(AddFile)
   enum { IDD = IDD_ADD_FILE };
   CString  m_file;
   //}}AFX_DATA

   virtual CString GetDialogProfileEntry() { return "AddFile"; }

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(AddFile)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(AddFile)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

// end ADD.H/////////////////////////////////////////////////////////////////////////////
