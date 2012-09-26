// $Header: /CAMCAD/4.5/StaticLabel.h 5     12/19/05 3:04a Kurt Van Ness $

#if !defined(__StaticLabel_h__)
#define __StaticLabel_h__

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CStaticLabel window
enum FlashType {None, Text, Background };

class CStaticLabel : public CStatic
{
private:
   COLORREF m_bkColor;

// Construction
public:
	CStaticLabel();

	CStaticLabel& SetBkColor(COLORREF crBkgnd);
	CStaticLabel& SetTextColor(COLORREF crText);
	CStaticLabel& SetText(const CString& strText);
	CStaticLabel& SetFontBold(BOOL bBold);
	CStaticLabel& SetFontName(const CString& strFont);
	CStaticLabel& SetFontUnderline(BOOL bSet);
	CStaticLabel& SetFontItalic(BOOL bSet);
	CStaticLabel& SetFontSize(int nSize);
	CStaticLabel& SetSunken(BOOL bSet);
	CStaticLabel& SetBorder(BOOL bSet);
	CStaticLabel& FlashText(BOOL bActivate);
	CStaticLabel& FlashBackground(BOOL bActivate);
	CStaticLabel& SetLink(BOOL bLink);
	CStaticLabel& SetLinkCursor(HCURSOR hCursor);

   COLORREF getBkColor() const { return m_bkColor; }

// Attributes
public:
protected:
	void ReconstructFont();
	COLORREF	m_crText;
	HBRUSH		m_hBrush;
	HBRUSH		m_hwndBrush;
	LOGFONT		m_lf;
	CFont		m_font;
	CString		m_strText;
	BOOL		m_bState;
	BOOL		m_bTimer;
	BOOL		m_bLink;
	FlashType	m_Type;
	HCURSOR		m_hCursor;
			// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticLabel)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStaticLabel();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStaticLabel)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//_____________________________________________________________________________
class CStaticCounter : public CStaticLabel
{
private:
   int m_count;

public:
   CStaticCounter();
   ~CStaticCounter();

   int getCount() const;
   void setCount(int count);
   int incrementCount(int delta=1);
   int decrementCount(int delta=1);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(__StaticLabel_h__)
