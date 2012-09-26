// $Header: /CAMCAD/4.3/ColorEditBox.cpp 1     12/03/03 8:00a Kurt Van Ness $

/*
History: ColorEditBox.cpp $
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

#include "StdAfx.h"
#include "ColorEditBox.h"
#include "Colors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//_____________________________________________________________________________
CColorEditBox::CColorEditBox()
{
   m_brush.CreateSolidBrush(colorWhite);
   m_backGroundColor = colorWhite;
   m_textColor       = colorBlack;
}

CColorEditBox::~CColorEditBox()
{
}

void CColorEditBox::setTextColor(COLORREF color) 
{ 
   m_textColor = color; 

   Invalidate();
}

void CColorEditBox::setBackGroundColor(COLORREF color) 
{ 
   m_backGroundColor = color; 

   m_brush.DeleteObject();
   m_brush.CreateSolidBrush(m_backGroundColor);

   Invalidate();
}

BEGIN_MESSAGE_MAP(CColorEditBox, CEdit)
	//{{AFX_MSG_MAP(CColorEditBox)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorEditBox message handlers

HBRUSH CColorEditBox::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	pDC->SetTextColor(m_textColor);
   pDC->SetBkColor(m_backGroundColor);
   pDC->SetBkMode(OPAQUE);

	// TODO: Return a non-NULL brush if the parent's handler should not be called
	//return NULL;

   return (HBRUSH)(m_brush.m_hObject);
}
