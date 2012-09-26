// $Header: /CAMCAD/4.3/MergeFiles.h 7     8/12/03 9:06p Kurt Van Ness $

#if !defined(AFX_MERGEFILES_H__73AAFBB1_96FE_11D1_BA40_444553540000__INCLUDED_)
#define AFX_MERGEFILES_H__73AAFBB1_96FE_11D1_BA40_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CCEtoODBDoc;

void CopyFileContents(FileStruct *from, FileStruct *to, CCEtoODBDoc *doc);

/////////////////////////////////////////////////////////////////////////////
// MergeFiles dialog
class MergeFiles : public CDialog
{
// Construction
public:
   MergeFiles(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(MergeFiles)
   enum { IDD = IDD_MERGE_FILES };
   CListBox m_files;
   CComboBox   m_mergeTo;
   //}}AFX_DATA
   CCEtoODBDoc *doc;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(MergeFiles)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(MergeFiles)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGEFILES_H__73AAFBB1_96FE_11D1_BA40_444553540000__INCLUDED_)
