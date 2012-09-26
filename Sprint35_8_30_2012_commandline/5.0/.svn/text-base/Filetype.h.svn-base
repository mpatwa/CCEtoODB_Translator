// $Header: /CAMCAD/4.3/FileType.h 8     8/12/03 9:06p Kurt Van Ness $

#include "ResizingDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CFileTypeDialog dialog

class CFileTypeDialog : public CResizingDialog
{
// Construction
public:
   CFileTypeDialog(FileTypeTag *FileType, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(CFileTypeDialog)
   enum { IDD = IDD_FILE_TYPE_DIALOG };
   CListBox m_UnlicensedLB;
   CListBox m_LicensedLB;
   int      m_sel;
   //}}AFX_DATA
   FileTypeTag *m_fileType;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CFileTypeDialog)
   public:
   virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("FileTypeDialog"); }

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CFileTypeDialog)
   virtual void OnOK();
   virtual BOOL OnInitDialog();
   afx_msg void OnHelp();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// FileExportType dialog

class FileExportType : public CResizingDialog
{
// Construction
public:
   FileExportType(int *FileType, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FileExportType)
   enum { IDD = IDD_FILE_EXPORT_TYPE };
   CListBox m_UnlicensedLB;
   CListBox m_LicensedLB;
   int      m_sel;
   //}}AFX_DATA
   int *fileType;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FileExportType)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("FileExportTypeDialog"); }

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FileExportType)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// MentorFileType dialog

class MentorFileType : public CResizingDialog
{
// Construction
public:
   MentorFileType(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(MentorFileType)
   enum { IDD = IDD_MENTOR_FILE_TYPE };
   int      m_format;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(MentorFileType)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("MentorFileTypDialog"); }

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(MentorFileType)
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
