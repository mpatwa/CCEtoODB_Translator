// $Header: /CAMCAD/4.3/Bom.h 7     8/12/03 9:05p Kurt Van Ness $

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// BOMReader dialog
class BOMReader : public CDialog
{
// Construction
public:
   BOMReader(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(BOMReader)
   enum { IDD = IDD_BOM_READER };
   CString  m_logfile;
   CString  m_macfile;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(BOMReader)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(BOMReader)
   afx_msg void OnChangeMacFile();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
