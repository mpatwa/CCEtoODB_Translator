// $Header: /CAMCAD/4.3/TreeListHeaderCtrl.h 3     9/18/03 12:29p Kurt Van Ness $

#if ! defined (__TreeListHeaderCtrl_h__)
#define __TreeListHeaderCtrl_h__

#pragma once

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CTreeListHeaderCtrl window

class CTreeListHeaderCtrl : public CHeaderCtrl
{
// Construction
public:
	CTreeListHeaderCtrl();

// Attributes
protected:
	CImageList *m_pImageList;
	CMap< int, int, int, int> m_mapImageIndex;

private:
   bool m_sortingEnabled;
	BOOL m_bAutofit;
	void Autofit(int nOverrideItemData = -1, int nOverrideWidth = 0);

// Operations
public:
	BOOL m_RTL;

	void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	CImageList* SetImageList( CImageList* pImageList );
	int GetItemImage( int nItem );
	void SetItemImage( int nItem, int nImage );
	void SetAutofit(bool bAutofit = true) { m_bAutofit = bAutofit; Autofit(); }
   void setSortingEnabled(bool enableFlag) { m_sortingEnabled = enableFlag; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeListHeaderCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeListHeaderCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeListHeaderCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWHEADERCTRL_H__99EB0481_4FA1_11D1_980A_004095E0DEFA__INCLUDED_)
