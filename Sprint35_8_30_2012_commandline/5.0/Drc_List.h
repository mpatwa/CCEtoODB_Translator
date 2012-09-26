// $Header: /CAMCAD/4.5/Drc_List.h 16    3/30/05 11:38a Devin Dow $

#if !defined(AFX_DRC_LIST_H__3A2411C1_078A_11D3_9D65_00E09802C1AC__INCLUDED_)
#define AFX_DRC_LIST_H__3A2411C1_078A_11D3_9D65_00E09802C1AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ccdoc.h"
#include "afxwin.h"
#include "ResizingDialog.h"

/////////////////////////////////////////////////////////////////////////////
// DRC_List dialog

class DRC_List : public CResizingDialog
{
// Construction
public:
   DRC_List(CWnd* pParent = NULL);   // standard constructor
   ~DRC_List();

// Dialog Data
   //{{AFX_DATA(DRC_List)
   enum { IDD = IDD_LIST_DRC };
   CComboBox   m_algorithmCB;
   CTreeCtrl   m_tree;
   CString  m_tAlg;
   CString  m_tString;
   CString  m_tText;
   CString  m_tXY;
   CString  m_comment;
   CString  m_count;
   BOOL  m_hideDRCs;
   BOOL  m_hideTests;
   //}}AFX_DATA

   CCEtoODBDoc *doc;
   FileStruct *curFile;
   CImageList *m_imageList;
   BOOL Expanded;
   int width;
   CToolTipCtrl tooltip;
   CString hint;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(DRC_List)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual void PostNcDestroy();
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("DrcListDialog"); }

// Implementation
public:
   void SelectAlgorithm(int algIndex);
   void SelectDRC(FileStruct *file, DRCStruct *drc);

   DRCStruct* GetSelectedDRC();
   void DrawDRCArrow(DRCStruct *drc);
   virtual void OnCancel();


protected:
   // Generated message map functions
   //{{AFX_MSG(DRC_List)
   virtual BOOL OnInitDialog();
   afx_msg void OnAcknowledge();
   afx_msg void OnPanTo();
   afx_msg void OnUnacknowledge();
   afx_msg void OnClear();
   afx_msg void OnSelchangedDrcTree(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnClearAll();
   afx_msg void OnDblclkDrcTree(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnCommentChanged();
   afx_msg void OnHideDrcs();
   afx_msg void OnHideTests();
   afx_msg BOOL UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result);
   //}}AFX_MSG
   void FillTree();
   void FillAlgorithms();
	void panToSelectedDRC();
   DECLARE_MESSAGE_MAP()

public:
   CComboBox m_fileCB;
   int m_status;

	void ChangeFile(void);
   void SelectVisibleFile(void);

   afx_msg void OnCbnSelchangeFilecb();
   afx_msg void OnStatusChanged();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRC_LIST_H__3A2411C1_078A_11D3_9D65_00E09802C1AC__INCLUDED_)
