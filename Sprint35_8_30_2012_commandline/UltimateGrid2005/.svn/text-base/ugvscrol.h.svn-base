// $Header: /CAMCAD/4.6/UltimateGrid2005/ugvscrol.h 1     5/08/06 8:28p Kurt Van Ness $

/*************************************************************************
				Class Declaration : CUGVScroll
**************************************************************************
	Source file : ugvscrol.cpp
	Header file : ugvscrol.h
	Copyright © The Code Project 1994 - 2002, All Rights Reserved

	This class is grid's vertical scrollbar.

*************************************************************************/
#ifndef _ugvscrol_H_
#define _ugvscrol_H_

class UG_CLASS_DECL CUGVScroll : public CScrollBar
{
// Construction
public:
	CUGVScroll();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUGVScroll)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUGVScroll();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUGVScroll)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	
	friend CUGCtrl;
	CUGCtrl *		m_ctrl;
	CUGGridInfo *	m_GI;			//pointer to the grid information


	double			m_multiRange;	//scroll bar multiplication factor
									//for setting the scroll range
	double			m_multiPos;		//multiplication factor for setting the 
									//top row during a thumb track
	long			m_lastMaxTopRow;//last max top row

	int				m_lastScrollMode;

	int				m_lastNumLockRow;

	long			m_trackRowPos;

public:

	//internal functions
	void Update();
	void Moved();
	void VScroll(UINT nSBCode, UINT nPos);

};

#endif // _ugvscrol_H_