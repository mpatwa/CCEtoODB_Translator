// $Header: /CAMCAD/4.3/Explode.h 7     8/12/03 9:06p Kurt Van Ness $

/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-6. All Rights Reserved.
*/           

#include "ccdoc.h"

/////////////////////////////////////////////////////////////////////////////
// GenerateBlockDlg dialog
class GenerateBlockDlg : public CDialog
{
// Construction
public:
   GenerateBlockDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(GenerateBlockDlg)
   enum { IDD = IDD_GENERATE_BLOCK };
   CString  m_name;
   //}}AFX_DATA

   CCEtoODBDoc *doc;
   int filenum;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(GenerateBlockDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(GenerateBlockDlg)
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

