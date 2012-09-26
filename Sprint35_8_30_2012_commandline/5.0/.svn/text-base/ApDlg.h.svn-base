// $Header: /CAMCAD/4.3/ApDlg.h 7     8/12/03 9:05p Kurt Van Ness $

#include "aperture.h"

/////////////////////////////////////////////////////////////////////////////
// SmartReader dialog

class SmartReader : public CDialog
{
// Construction
public:
   SmartReader(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(SmartReader)
   enum { IDD = IDD_APERTURE_READER };
   CString  m_ApPrefix;
   CString  m_Logfile;
   CString  m_MacFile;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(SmartReader)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(SmartReader)
   afx_msg void OnChangeMacFile();
   afx_msg void OnHelp();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// ApImportList dialog

class ApImportList : public CDialog
{
// Construction
public:
   ApImportList(AperStruct **Head, char *Prefix, char *Logfile, int apUnits,
      char *macName, CWnd* pParent = NULL);   // standard constructor
   ~ApImportList();

// Dialog Data
   //{{AFX_DATA(ApImportList)
   enum { IDD = IDD_IMPORTED_APERTURE_LIST };
   CListCtrl   m_listCtrl;
   CString  m_units;
   CString  m_macName;
   //}}AFX_DATA
   AperStruct **head;
   char *prefix;
   char *logfile;
   int decimals;
   CImageList *m_imageList;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ApImportList)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(ApImportList)
   afx_msg void OnLogfile();
   virtual BOOL OnInitDialog();
   afx_msg void OnHelp();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
