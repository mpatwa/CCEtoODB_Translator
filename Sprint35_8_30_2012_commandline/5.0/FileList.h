// $Header: /CAMCAD/4.5/FileList.h 11    1/24/07 2:07p Rick Faltersack $

#include "ccdoc.h"

/////////////////////////////////////////////////////////////////////////////
// FileListCtrl window
class FileListCtrl : public CListCtrl
{
// Construction
public:
   FileListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FileListCtrl)
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~FileListCtrl();

   // Generated message map functions
protected:
   //{{AFX_MSG(FileListCtrl)
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CFileListDialog dialog
class CFileListDialog : public CResizingDialog
{
// Construction
public:
   CFileListDialog(CWnd* pParent = NULL);   // standard constructor
   virtual ~CFileListDialog();

   virtual CString GetDialogProfileEntry() { return "FileListDlg"; }

// Dialog Data
   //{{AFX_DATA(CFileListDialog)
   enum { IDD = IDD_LIST_FILE };
   FileListCtrl   m_fileLC;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   CImageList *m_imageList;
   int decimals;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CFileListDialog)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
   void WriteData();
   void ReadData();
   void FillListCtrl();
   void RestructureFiles();

public:
   // Generated message map functions
   //{{AFX_MSG(CFileListDialog)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   virtual void OnCancel();
   afx_msg void OnHelp();
   afx_msg void OnEdit();
   afx_msg void OnApply();
   afx_msg void OnAllOn();
   afx_msg void OnAllOff();
   afx_msg void OnRemove();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// EditFilePlacement dialog
class EditFilePlacement : public CDialog
{
// Construction
public:
   EditFilePlacement(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(EditFilePlacement)
   enum { IDD = IDD_LIST_FILE_EDIT };
   CComboBox   m_filetypeCB;
   BOOL  m_mirror;
   CString  m_angle;
   CString  m_scale;
   BOOL  m_show;
   CString  m_x;
   CString  m_y;
   CString  m_file;
   //}}AFX_DATA
   int filetype;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditFilePlacement)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(EditFilePlacement)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// SelectFileDlg dialog
class SelectFileDlg : public CDialog
{
// Construction
public:
   SelectFileDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(SelectFileDlg)
   enum { IDD = IDD_SELECT_FILE };
   CListBox m_list;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   FileStruct *file;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(SelectFileDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

private:
	BlockTypeTag m_eFileType;
	int m_iSourceCad;

public:
	BlockTypeTag GetFileType() const				{ return m_eFileType;			};
	int GetSourceCad() const						{ return m_iSourceCad;			};

	void SetFileType(BlockTypeTag fileType)	{ m_eFileType = fileType;		};
	void SetSourceCad(int sourceCad)				{ m_iSourceCad = sourceCad;	};

protected:

   // Generated message map functions
   //{{AFX_MSG(SelectFileDlg)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
