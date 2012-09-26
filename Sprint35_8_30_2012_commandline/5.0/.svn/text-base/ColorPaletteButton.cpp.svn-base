// $Header: /CAMCAD/4.5/ColorPaletteButton.cpp 2     10/18/05 8:13p Kurt Van Ness $

/*
History: ColorPaletteButton.cpp $
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

#include "StdAfx.h"
//#include "PadsDRC.h"
#include "ColorPaletteButton.h"
//#include "Globals.h"
//#include "AppPalette.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//_____________________________________________________________________________
CColorPaletteButton::CColorPaletteButton(CAppPalette& appPalette)
: m_appPalette(appPalette)
{
}

CColorPaletteButton::~CColorPaletteButton()
{
}

bool CColorPaletteButton::verifyColor()
{
   CDC* dc = GetDC();

   ASSERT(dc != NULL);
   ASSERT(dc->GetDeviceCaps(RASTERCAPS) & RC_BITBLT);

   CRect clientRect;
   GetClientRect(&clientRect);
   CPoint center((clientRect.left + clientRect.right)/2,(clientRect.top + clientRect.bottom)/2);

   COLORREF colorButtonColor = dc->GetPixel(center);

   m_appPalette.pushPalette(dc);

   PALETTEENTRY paletteEntry;
   CPalette* palette = dc->GetCurrentPalette();
   ASSERT(palette != NULL);
   int numEntries = palette->GetEntryCount();

   VERIFY(palette->GetPaletteEntries(m_appPalette.getEditIndex(),1,&paletteEntry) > 0);

   COLORREF paletteColor = RGB(paletteEntry.peRed,paletteEntry.peGreen,paletteEntry.peBlue);

   bool retval = (colorButtonColor == paletteColor);

   m_appPalette.popPalette(dc);

   if (!retval)
   {
      TRACE("CColorPaletteButton::verifyColor() - %s, buttonColor=0x%08x, paletteColor=0x%08x, \n",
         retval ? "success" : "failure",colorButtonColor,paletteColor);
   }
   else
   {
      TRACE("CColorPaletteButton::verifyColor() - %s, buttonColor=0x%08x, paletteColor=0x%08x, \n",
         retval ? "success" : "failure",colorButtonColor,paletteColor);
   }

   return retval;
}

BEGIN_MESSAGE_MAP(CColorPaletteButton, CButton)
	//{{AFX_MSG_MAP(CColorPaletteButton)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorPaletteButton message handlers

BOOL CColorPaletteButton::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	//return CButton::OnEraseBkgnd(pDC);
   TRACE("CColorPaletteButton::OnEraseBkgnd()\n");
   return TRUE;
}

void CColorPaletteButton::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CButton::OnPaint() for painting messages
   COLORREF buttonColor = (m_appPalette.isPaletteDevice() ? 
      PALETTEINDEX(m_appPalette.getEditIndex()) : 
      m_appPalette.getEditColor() );

   TRACE("CColorPaletteButton::OnPaint() - buttonColor = %08x\n",buttonColor);

   CRect rect;
   GetClientRect(&rect);
   CPen penHighlight(PS_SOLID,1,GetSysColor(COLOR_BTNHIGHLIGHT));
   CPen* oldPen = dc.SelectObject(&penHighlight);
   dc.MoveTo(rect.left,rect.bottom);
   dc.LineTo(rect.right,rect.bottom);
   dc.LineTo(rect.right,rect.top - 1);

   CPen penShadow(PS_SOLID,1,GetSysColor(COLOR_BTNSHADOW));
   dc.SelectObject(&penShadow);
   dc.MoveTo(rect.left,rect.bottom - 1);
   dc.LineTo(rect.left,rect.top);
   dc.LineTo(rect.right,rect.top);

   CPen penFace(PS_SOLID,1,GetSysColor(COLOR_BTNFACE));
   dc.SelectObject(&penFace);
   dc.MoveTo(rect.left + 1,rect.bottom - 1);
   dc.LineTo(rect.right - 1,rect.bottom - 1);
   dc.LineTo(rect.right - 1,rect.top);

   CPen penDarkShadow(PS_SOLID,1,GetSysColor(COLOR_3DDKSHADOW));
   dc.SelectObject(&penDarkShadow);
   dc.MoveTo(rect.left + 1,rect.bottom - 2);
   dc.LineTo(rect.left + 1,rect.top + 1);
   dc.LineTo(rect.right - 1,rect.top + 1);

   dc.SelectObject(oldPen);

   m_appPalette.pushPalette(&dc);

   CBrush brush;
   brush.CreateSolidBrush(buttonColor);
   CBrush* oldBrush = dc.SelectObject(&brush);
   rect.top += 2;
   rect.left += 2;
   rect.bottom -= 1;
   rect.right -= 1;
   dc.SelectObject(&brush);
   dc.FillRect(&rect,&brush);

   dc.SelectObject(oldBrush);

   m_appPalette.popPalette(&dc);
}
