// $Header: /CAMCAD/4.3/Print.h 7     8/12/03 9:06p Kurt Van Ness $

/////////////////////////////////////////////////////////////////////////////
// PrintDialog dialog
class PrintDialog : public CDialog
{
// Construction
public:
   PrintDialog(CPrintInfo *PInfo, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(PrintDialog)
   enum { IDD = IDD_PRINT };
   int      m_fit;
   int      m_extents;
   BOOL  m_banner;
   BOOL  m_print_by_layer;
   double   m_scale;
   int      m_portrait;
   int      m_Black;
   BOOL  m_grid;
   //}}AFX_DATA
   CPrintInfo *pInfo;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(PrintDialog)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(PrintDialog)
   virtual void OnOK();
   afx_msg void OnHelp();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
