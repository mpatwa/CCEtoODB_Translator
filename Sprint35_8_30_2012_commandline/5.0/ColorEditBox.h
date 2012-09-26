// $Header: /CAMCAD/4.3/ColorEditBox.h 1     12/03/03 8:00a Kurt Van Ness $

/*
History: ColorEditBox.h $
 * 
 * *****************  Version 1  *****************
 * User: Knv          Date: 12/22/00   Time: 6:52p
 * Created in $/knv/BibTool
 * Initial add.
 * 
 * *****************  Version 1  *****************
 * User: Kvanness     Date: 1/03/00    Time: 12:37a
 * Created in $/CdControl
 * Initial add
*/

#if !defined(__ColorEditBox_H__)
#define __ColorEditBox_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorEditBox.h : header file
//

//_____________________________________________________________________________
class CColorEditBox : public CEdit
{
// Construction
public:
	CColorEditBox();
	CColorEditBox(UINT);

// Attributes
private:
   CBrush m_brush;
   COLORREF m_textColor;
   COLORREF m_backGroundColor;

// Operations
public:
   COLORREF getTextColor() { return m_textColor; }
   void setTextColor(COLORREF color);

   COLORREF getBackGroundColor() { return m_backGroundColor; }
   void setBackGroundColor(COLORREF color);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorEditBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorEditBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorEditBox)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__ColorEditBox_H__)
