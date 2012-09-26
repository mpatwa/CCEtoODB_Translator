// $Header: /CAMCAD/4.6/UltimateGrid2005/UGEdit.h 1     5/08/06 8:28p Kurt Van Ness $

/*************************************************************************
				Class Declaration : CUGEdit
**************************************************************************
	Source file : UGEdit.cpp
	Header file : UGEdit.h
	Copyright © The Code Project 1994 - 2002, All Rights Reserved

	Purpose
		The Ultimate grid uses the CUGEdit class
		as the default edit control.  An instance
		of this class is created every time an instance
		of the Ultimate Grid is created.

		It is possible to specify new edit control
		for the Ultimate Grid to use, you can do this
		either with the use of the OnEditStart notification
		or SetNewEditClass.  Each of these methods has
		its benefits and please refer to the documentation
		for more information.

	Key features
		-This control automatically expands to the
		 right as text is entered, once the control
		 reaches the right side of the grid then it
		 expands downward until it reaches the bottom.
		 Once it reaches the bottom then it will start
		 scrolling text as it is entered.
		-When editing first starts the control also
		 automatically expands to fit the inital text.
*************************************************************************/
#ifndef _UGEdit_H_
#define _UGEdit_H_

class UG_CLASS_DECL CUGEdit : public CUGEditBase
{
// Construction
public:
	CUGEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUGEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUGEdit();


	// Generated message map functions
protected:
	//{{AFX_MSG(CUGEdit)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnUpdate();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	BOOL m_autoSize;
	BOOL m_tempAutoSize;

	int m_origHeight;
	int m_currentHeight;

	void CalcEditRect();

public:

	BOOL SetAutoSize(BOOL state);
};

#endif //#ifndef _UGEdit_H_
