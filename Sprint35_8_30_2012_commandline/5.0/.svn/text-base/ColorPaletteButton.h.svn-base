// $Header: /CAMCAD/4.5/ColorPaletteButton.h 2     10/18/05 8:13p Kurt Van Ness $

/*
History: ColorPaletteButton.h $
 * 
 * *****************  Version 2  *****************
 * User: Kvanness     Date: 10/20/00   Time: 11:46p
 * Updated in $/PadsDRC
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 6/12/99    Time: 11:31a
 * Created in $/PadsDRC
 * Initial add.
*/

#if !defined(__ColorPaletteButton_H__)
#define __ColorPaletteButton_H__

#pragma once

#include "AppPalette.h"

//_____________________________________________________________________________
class CColorPaletteButton : public CButton
{
private:
   CAppPalette& m_appPalette;

// Construction
public:
	CColorPaletteButton(CAppPalette& m_appPalette);

// Attributes
public:

// Operations
public:
   bool verifyColor();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPaletteButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorPaletteButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorPaletteButton)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__ColorPaletteButton_H__)
