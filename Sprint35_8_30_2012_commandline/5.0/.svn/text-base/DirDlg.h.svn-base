// $Header: /CAMCAD/4.3/DirDlg.h 11    8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.
*/           

/***  INSTRUCTIONS  ****

DIR DIALOG (with Browse Capability)
----------

- Construct
CDirDlg dlg;

- Optionally Set Title
dlg.m_title    // Static text at top of dialog

- Execute
if (dlg.DoModal() == ID_OK)

- Get Result
CString path = dlg.m_dir;



BROWSE DIALOG
-------------

- Construct
CBrowse dlg;

- Optionally Set Variables
dlg.m_strInitDir = "C:\\";                               // Root to Browse From (if not set, then root is Desktop)
dlg.m_strSelDir = "C:\\temp";                            // Initially Selected Directory
dlg.m_strTitle = "Select Path for CAMCAD Output Files";  // Static text at top of dialog

- Execute
if (dlg.DoBrowse())

- Get Result
CString path = dlg.m_strPath;

*/ 

#if _MSC_VER >= 1000          
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#include "ResizingDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CDirDlg dialog
class CDirDlg : public CDialog
{
// Construction
public:
   CDirDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(CDirDlg)
   enum { IDD = IDD_DIRECTORY };
   CString  m_title;
   CString  m_dir;
   //}}AFX_DATA

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CDirDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CDirDlg)
   afx_msg void OnBrowse();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CBrowse
class CBrowse
{
  public:
   CBrowse();
   int DoBrowse();

   CString m_strPath;      // Result Path 
   CString m_strInitDir;   // Root to Browse From (if not set, then root is Desktop)
   CString m_strSelDir;    // Initially Selected Directory
   CString m_strTitle;     // Static text at top of dialog
   int  m_iImageIndex;     // index into system image list of selected item
};

/////////////////////////////////////////////////////////////////////////////
// ODBDirDlg dialog
class ODBDirDlg : public CResizingDialog
{
private:
   bool m_enableArchiveOpen;
   bool m_archiveFileOpened;
   CString m_archiveFilePath;

// Construction
public:
   ODBDirDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(ODBDirDlg)
   enum { IDD = IDD_ODB_DIRECTORY };
   CListBox m_filesLB;
   CString  m_dir;
   CString  m_filter;
   //}}AFX_DATA

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ODBDirDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("OdbDirDialog"); }

// Implementation
public:
   bool isArchiveFileOpened() { return m_archiveFileOpened; }
   CString getArchiveFilePath() { return m_archiveFilePath; }
   void setEnableArchiveOpen(bool enable) { m_enableArchiveOpen = enable; }

protected:
   void FillFileList();

   // Generated message map functions
   //{{AFX_MSG(ODBDirDlg)
   afx_msg void OnBrowse();
   afx_msg void OnUnzip();
   afx_msg void OnChangeDir();
   afx_msg void OnChangeFilter();
   virtual BOOL OnInitDialog();
   afx_msg void OnDblclkFiles();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

public:
   afx_msg void OnBnClickedOpenArchive();
};
