// $Header: /CAMCAD/4.5/TreeListFrame.h 7     2/15/05 8:27p Kurt Van Ness $

#if ! defined (__TreeListFrame_h__)
#define __TreeListFrame_h__

#pragma once

#include "TreeListCtrl.h"

#define ID_EDIT_FIELD   333
#define ID_TREE_LIST_HEADER 337
#define ID_TREE_LIST_CTRL   373
#define ID_TREE_LIST_SCROLLBAR   377

/////////////////////////////////////////////////////////////////////////////
// CTreeListFrame window

class CTreeListFrame : public CWnd
{
// Construction
public:
	CTreeListFrame();

// Attributes
public:
	BOOL m_RTL;

	CFont m_headerFont;
	CScrollBar m_horScrollBar;

protected:
	CTreeListCtrl* m_tree;

// Operations
private:
   bool m_sortingEnabled;
	static LONG FAR PASCAL DummyWndProc(HWND, UINT, WPARAM, LPARAM);
	void Initialize();

public:
	static void RegisterClass();
	BOOL SubclassDlgItem(UINT nID, CWnd* parent); // use in CDialog/CFormView

	void SortTree(int nCol, BOOL bAscending, HTREEITEM hParent);
	void ResetScrollBar();

	BOOL VerticalScrollVisible();
	BOOL HorizontalScrollVisible();

	int StretchWidth(int m_nWidth, int m_nMeasure);
   void setSortingEnabled(bool enableFlag) { m_sortingEnabled = enableFlag; }
   virtual CTreeListCtrl& getTree();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeListFrame)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnChildNotify( UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult );
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeListFrame();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeListFrame)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TLFRAME_H__99EB040C_4FA1_11D1_980A_004095E0DEFA__INCLUDED_)
