// $Header: /CAMCAD/4.5/ToolbarC.h 10    11/30/05 8:47p Kurt Van Ness $

#if !defined(AFX_TOOLBARC_H__C5D7E190_1540_11D3_863E_004005408E44__INCLUDED_)
#define AFX_TOOLBARC_H__C5D7E190_1540_11D3_863E_004005408E44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolBarC.h : header file
//

#include "ToolBarM.h"

/////////////////////////////////////////////////////////////////////////////
// CCustomToolbar window

class CCustomToolBar : public CMyToolBar
{
// Construction
public:
   CCustomToolBar(CFrameWnd& parentFrame);

// Attributes
public:

// Operations
public:
   BOOL Create(CWnd *pParentWnd, DWORD dwStyle, UINT nID);
   void RemeberButtons();

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CCustomToolBar)
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CCustomToolBar();
   void SaveState();
   void RestoreState();
   TBBUTTON *buttonArray;
   int maxButtonArray;
   int tbNum;

   // Generated message map functions
protected:
   //{{AFX_MSG(CCustomToolBar)
   afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
   afx_msg void OnCustomize();
   afx_msg void OnDestroy();
   //}}AFX_MSG

   afx_msg void OnToolBarQueryInsert(NMHDR *notify, LRESULT *result);
   afx_msg void OnToolBarQueryDelete(NMHDR *notify, LRESULT *result);
   afx_msg void OnToolBarChange(NMHDR *notify, LRESULT *result);
   afx_msg void OnToolBarBeginDrag(NMHDR *notify, LRESULT *result);
   afx_msg void OnToolBarEndDrag(NMHDR *notify, LRESULT *result);
   afx_msg void OnToolBarBeginAdjust(NMHDR *notify, LRESULT *result);
   afx_msg void OnToolBarCustomHelp(NMHDR *notify, LRESULT *result);
   afx_msg void OnToolBarEndAdjust(NMHDR *notify, LRESULT *result);
   afx_msg void OnToolBarGetButtonInfo(NMHDR *notify, LRESULT *result);
   afx_msg void OnToolBarReset(NMHDR *notify, LRESULT *result);

   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLBARC_H__C5D7E190_1540_11D3_863E_004005408E44__INCLUDED_)
