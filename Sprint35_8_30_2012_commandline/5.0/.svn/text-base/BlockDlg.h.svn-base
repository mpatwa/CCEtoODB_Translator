// $Header: /CAMCAD/4.5/BlockDlg.h 12    6/05/06 3:31p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-97. All Rights Reserved.
*/

#pragma once

#include "ccdoc.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// SelectGeometry dialog
class SelectGeometry : public CResizingDialog
{
// Construction
public:
   SelectGeometry(CWnd* pParent = NULL);   // standard constructor
   ~SelectGeometry();  

// Dialog Data
   //{{AFX_DATA(SelectGeometry)
   enum { IDD = IDD_GEOMETRY_SELECT };
   CTreeCtrl   m_tree;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   CImageList *m_imageList;
   HTREEITEM currentItem;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(SelectGeometry)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("SelectGeometryDialog"); }

// Implementation
public:
   void SelectBlock(BlockStruct *block);
protected:
   void FillTree();
   void AddItem(BlockStruct *block, HTREEITEM parent);
   void ExpandItem(HTREEITEM item);
   // Generated message map functions
   //{{AFX_MSG(SelectGeometry)
   virtual BOOL OnInitDialog();
   virtual void OnCancel();
   afx_msg void OnEdit();
   afx_msg void OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedDuplicateGeometry();
   afx_msg void OnBnClickedRenameGeometry();
};

//_____________________________________________________________________________
class CRenameGeometryDialog : public CDialog
{
	DECLARE_DYNAMIC(CRenameGeometryDialog)

private:
   CString m_oldGeometryName;
   CString m_newGeometryName;

public:
	CRenameGeometryDialog(const CString& oldGeometryName);   // standard constructor
	virtual ~CRenameGeometryDialog();

// Dialog Data
	enum { IDD = IDD_RenameGeometryDialog };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
   CString getNewGeometryName() const { return m_newGeometryName; }
   virtual BOOL OnInitDialog();
   CStatic m_oldGeometryNameStatic;
};
