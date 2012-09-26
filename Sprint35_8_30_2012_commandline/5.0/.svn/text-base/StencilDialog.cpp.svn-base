
#include "StdAfx.h"
#include "StencilDialog.h"
#include "StencilGenerator.h"
#include "CamCadDatabase.h"
#include "RwLib.h"
#include "PopupMenu.h"
#include "TreeListFrame.h"
#include ".\stencildialog.h"
#include "Attribute.h"
#include "Apertur2.h"
#include "ODBC_Lib.h"

#define newDataStruct getCamCadData()->getNewDataStruct
#define getCamCadDataDocP getCamCadData

#define ColumnStencilObject           0
#define ColumnRule                    1
#define ColumnRuleValue               2
#define ColumnRuleModifier            3
#define ColumnStencilDerivationMethod 4

#define ColumnStencilObjectWidth 200
#define ColumnRuleWidth           85
#define ColumnRuleValueWidth     200
#define ColumnRuleModifierWidth  340

#define QPCB                          "PCB"
#define QTopSurface                   "Top Surface"
#define QBottomSurface                "Bottom Surface"
#define QSmdGeometries                "SMD Geometries"
#define QThGeometries                 "Thru-hole Geometries"
#define QViaGeometries                "Via Geometries"
#define QFiducialGeometries           "Fiducial Geometries"
#define QPcbSurfaceTypeGeomPad        "PCB/Surface/Type/Geom/Pad"
#define QPcbSurfaceTypeCompPin        "PCB/Surface/Type/Comp/Pin"
#define QPcbSurfaceTypeGeomPadCompPin "PCB/Surface/Type/Geom/Pad/Comp/Pin"
#define QRule                         "Rule"
#define QRuleValue                    "Rule Value"
#define QRuleModifier                 "Rule Modifier"
#define QDerivationMethod             "Derivation Method"
#define QStencilRulesCaption          "Generate/Validate Stencils"
#define QStencilPcbCaption            "Global Configuration"
#define QStencilGeomPadCaption        "Component Geometry / Pad Geometry Rules"
#define QStencilCompPinCaption        "Component Insert / Component Pin Rules"
#define QSetRuleFirst                 "<Set Rule First>"
#define QSelectAperture               "<Select Aperture>"
#define QSelectApertureGeometry       "<Select Aperture Geometry>"
#define QRemoveRule                   "<Remove Rule>"

#define QFlipped                      "Flipped"

bool fileExists(const CString& path);
CWnd* getMainWnd();
BOOL ComponentsExtents(CCEtoODBDoc *doc, const char *string, double *xmin, double *xmax, double *ymin, double *ymax, int SELECT_COMP);
int PanReference(CCEtoODBView *view, const char *ref);
CCEtoODBView* getActiveView();

COLORREF getStatusColor(StencilStatusTag statusTag)
{
   COLORREF color = colorLtGray;

   switch (statusTag)
   {
   case StencilStatusPossiblyStale:   color = colorCyan;     break;
   case StencilStatusStale:           color = colorRed;      break;
   case StencilStatusUpToDate:        color = colorGreen;    break;
   case StencilStatusDoesNotExist:    color = colorMagenta;  break;
   case StencilStatusStaleUndefined:  color = colorLtGray;   break;
   }

   return color;
}

bool isStandardApertureShapeSupportedForStencils(StandardApertureTypeTag apertureType)
{
   bool supportedFlag = true;

   switch (apertureType)
   {
   case standardApertureHole:
   case standardApertureMoire:
   //case standardApertureNull:
   case standardApertureRectangularThermal:
   case standardApertureRectangularThermalOpenCorners:
   //case standardApertureRoundThermalRounded:
   case standardApertureRoundThermalSquare:
   case standardApertureSquareRoundThermal:
   case standardApertureSquareThermal:
   case standardApertureSquareThermalOpenCorners:
   //case standardApertureRoundDonut:
   case standardApertureSquareDonut:
      supportedFlag = false;
      break;
   }

   return supportedFlag;
}

//_____________________________________________________________________________
bool CClipboard::putText(const CString& text)
{
   bool retval = false;

   if (OpenClipboard(NULL))
	{
		EmptyClipboard();

		HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, text.GetLength()+1);
		char* buffer = (char*)GlobalLock(clipbuffer);

		strcpy(buffer, (const char*)text);

		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);

		CloseClipboard();

      retval = true;
	}

   return retval;
}

bool CClipboard::getText(CString& text)
{
   bool retval = false;

	if (OpenClipboard(NULL))
	{
		char* buffer = (char*)GetClipboardData(CF_TEXT);

      text = buffer;

      retval = true;
	}

	CloseClipboard();

   return retval;
}

//_____________________________________________________________________________
CString CEditDouble::getStringValue()
{
   CString stringValue;
   GetWindowText(stringValue);

   return stringValue;
}

CString CEditDouble::getDoubleStringValue(double defaultValue)
{
   CString doubleString = getStringValue().Trim();

   char* endChar;

   double doubleValue = strtod(doubleString,&endChar);

   if (doubleString.IsEmpty() || *endChar != '\0')
   {
      doubleString = fpfmt(defaultValue,3);
   }

   return doubleString;
}

double CEditDouble::getDoubleValue()
{
   double doubleValue = atof(getStringValue());

   return doubleValue;
}

double CEditDouble::getDoubleValue(double defaultValue)
{
   CString doubleString = getStringValue().Trim();

   char* endChar;

   double doubleValue = strtod(doubleString,&endChar);

   if (doubleString.IsEmpty() || *endChar != '\0')
   {
      doubleValue = defaultValue;
   }

   return doubleValue;
}

void CEditDouble::setDoubleValue(const CString& doubleString,double defaultValue)
{
   char* endChar;
   double value = strtod(doubleString,&endChar);

   if (doubleString.IsEmpty() || *endChar != '\0')
   {
      setDoubleValue(defaultValue);
   }
   else
   {
      SetWindowText(doubleString);
   }
}

void CEditDouble::setDoubleValue(double value)
{
   SetWindowText(fpfmt(value,3));
}

//_____________________________________________________________________________
void CStencilRuleStaticLabel::setStatus(StencilRuleStatusTag status)
{
   SetBorder(true);
   SetBkColor(colorBlack);
   SetFontBold(true);

   switch (status)
   {
   case ruleStatusOk:
      SetText("OK");
      SetTextColor(colorGreen);

      break;
   case ruleStatusNoSelectedPin:
      SetText("No selection");
      SetTextColor(colorYellow);

      break;
   case ruleStatusInsetError:
      SetText("Inset Error");
      SetTextColor(colorRed);

      break;
   case ruleStatusInvalid:
      SetText("Invalid rule");
      SetTextColor(colorRed);

      break;
   default:
      SetText("Unknown");
      SetTextColor(colorLtGray);

      break;
   }
}

//_____________________________________________________________________________
#define ButtonIn          0x01
#define ButtonOut         0x02
#define ButtonBlackBorder 0x04
//const int BUTTON_IN          =   0x01;
//const int BUTTON_OUT      =   0x02;
//const int BUTTON_BLACK_BORDER =  0x04;


CColoredButton::CColoredButton()
{
   text_colour                = GetSysColor(COLOR_BTNTEXT);
   background_colour          = GetSysColor(COLOR_BTNFACE); 
   disabled_background_colour = background_colour;
   light                      = GetSysColor(COLOR_3DLIGHT);
   highlight                  = GetSysColor(COLOR_BTNHIGHLIGHT);
   shadow                     = GetSysColor(COLOR_BTNSHADOW);
   dark_shadow                = GetSysColor(COLOR_3DDKSHADOW);
}

CColoredButton::~CColoredButton()
{
}

void CColoredButton::SetColor(COLORREF   new_text_colour)
{
   text_colour             = new_text_colour;

   Invalidate(FALSE);
}

void CColoredButton::SetColor(COLORREF   new_text_colour, COLORREF new_background_colour)
{
   text_colour             = new_text_colour;
   background_colour       = new_background_colour; 
   disabled_background_colour = GetSysColor(COLOR_BTNFACE); 

   Invalidate(FALSE);
}

void CColoredButton::setTextBackgroundColor(COLORREF new_background_colour)
{
   background_colour       = new_background_colour; 
   disabled_background_colour = GetSysColor(COLOR_BTNFACE); 

   Invalidate(FALSE);
}

void CColoredButton::SetColor(COLORREF   new_text_colour, COLORREF new_background_colour, COLORREF new_disabled_background_colour)
{
   text_colour             = new_text_colour;
   background_colour       = new_background_colour; 
   disabled_background_colour = new_disabled_background_colour; 

   Invalidate(FALSE);
}

void CColoredButton::ResetColor()
{
   text_colour             = GetSysColor(COLOR_BTNTEXT);
   background_colour       = GetSysColor(COLOR_BTNFACE); 
   disabled_background_colour = background_colour;
   light                   = GetSysColor(COLOR_3DLIGHT);
   highlight               = GetSysColor(COLOR_BTNHIGHLIGHT);
   shadow                  = GetSysColor(COLOR_BTNSHADOW);
   dark_shadow             = GetSysColor(COLOR_3DDKSHADOW);

   Invalidate(FALSE);
}

BEGIN_MESSAGE_MAP(CColoredButton, CButton)
   //{{AFX_MSG_MAP(CColoredButton)
      // NOTE  - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColoredButton message handlers

void CColoredButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   CDC      *dc;
   CRect focus_rect, button_rect, text_rect, offset_text_rect;
   UINT  state;
   
   dc   =   CDC::FromHandle(lpDrawItemStruct->hDC);
   state =  lpDrawItemStruct->itemState;

   focus_rect.CopyRect(&lpDrawItemStruct->rcItem); 
   button_rect.CopyRect(&lpDrawItemStruct->rcItem); 

   text_rect = button_rect;
   text_rect.OffsetRect(-1, -1); 
   offset_text_rect = text_rect;
   offset_text_rect.OffsetRect(1, 1);

   // Set the focus rectangle to just past   the   border decoration
   focus_rect.left     += 4;
   focus_rect.right  -= 4;
   focus_rect.top   += 4;
   focus_rect.bottom -= 4;
     
   // Retrieve the   button's caption
   const int bufSize =  512;
   TCHAR buffer[bufSize];
   GetWindowText(buffer, bufSize);

   if (state & ODS_DISABLED)
   {
      DrawFilledRect(dc, button_rect,  disabled_background_colour);
   }
   else
   {
      DrawFilledRect(dc, button_rect,  background_colour);
   }
      
   if (state & ODS_SELECTED)
   { 
      DrawFrame(dc, button_rect, ButtonIn);
   }
   else
   {
      if ((state & ODS_DEFAULT) || (state & ODS_FOCUS))
      {
         DrawFrame(dc, button_rect, ButtonOut |   ButtonBlackBorder);         
      }
      else
      {
         DrawFrame(dc, button_rect, ButtonOut);
      }
   }

   if (state & ODS_DISABLED)
   {
      DrawButtonText(dc, offset_text_rect, buffer,colorWhite);
      DrawButtonText(dc, text_rect,     buffer,colorDkGray);
   }
   else
   {
      if (state & ODS_SELECTED)
      {
         DrawButtonText(dc, offset_text_rect, buffer, text_colour);
      }
      else
      {
         DrawButtonText(dc, text_rect, buffer, text_colour);
      }
   }

   if (state & ODS_FOCUS)
   {
      DrawFocusRect(lpDrawItemStruct->hDC, (LPRECT)&focus_rect);
   }
} 

void CColoredButton::DrawFrame(CDC *dc,   CRect r, int state)
{ 
   COLORREF colour;

   if (state & ButtonBlackBorder)
   {
      colour = colorBlack;

      DrawLine(dc, r.left, r.top,   r.right, r.top,      colour); // Across top
      DrawLine(dc, r.left, r.top,   r.left,   r.bottom, colour);  // Down  left

      DrawLine(dc, r.left,   r.bottom - 1,   r.right,  r.bottom - 1, colour); // Across bottom
      DrawLine(dc, r.right - 1, r.top,    r.right  - 1, r.bottom,    colour); // Down  right

      r.InflateRect(-1, -1);
   }
   
   if (state & ButtonOut)
   {
      colour = highlight;

      DrawLine(dc, r.left, r.top,   r.right, r.top,      colour); // Across top
      DrawLine(dc, r.left, r.top,   r.left,   r.bottom, colour);  // Down  left

      colour = dark_shadow;
   
      DrawLine(dc, r.left,   r.bottom - 1,   r.right,  r.bottom - 1, colour); // Across bottom
      DrawLine(dc, r.right - 1, r.top,    r.right  - 1, r.bottom,    colour); // Down  right
   
      r.InflateRect(-1, -1);

      colour = light;
   
      DrawLine(dc, r.left, r.top,   r.right, r.top,      colour); // Across top
      DrawLine(dc, r.left, r.top,   r.left,   r.bottom, colour);  // Down  left

      colour = shadow;
   
      DrawLine(dc, r.left,   r.bottom - 1,   r.right,  r.bottom - 1, colour); // Across bottom
      DrawLine(dc, r.right - 1, r.top,    r.right  - 1, r.bottom,    colour); // Down  right
   }

   if (state & ButtonIn)
   {
      colour = dark_shadow;

      DrawLine(dc, r.left, r.top,   r.right, r.top,      colour); // Across top
      DrawLine(dc, r.left, r.top,   r.left,   r.bottom, colour);  // Down  left
      DrawLine(dc, r.left,   r.bottom - 1,   r.right,  r.bottom - 1, colour); // Across bottom
      DrawLine(dc, r.right - 1, r.top,    r.right  - 1, r.bottom,    colour); // Down  right
   
      r.InflateRect(-1, -1);

      colour = shadow;
   
      DrawLine(dc, r.left, r.top,   r.right, r.top,      colour); // Across top
      DrawLine(dc, r.left, r.top,   r.left,   r.bottom, colour);  // Down  left
      DrawLine(dc, r.left,   r.bottom - 1,   r.right,  r.bottom - 1, colour); // Across bottom
      DrawLine(dc, r.right - 1, r.top,    r.right  - 1, r.bottom,    colour); // Down  right
   }
}

void CColoredButton::DrawFilledRect(CDC   *dc, CRect r, COLORREF colour)
{ 
   CBrush B;

   B.CreateSolidBrush(colour);
   dc->FillRect(r,   &B);
}

void CColoredButton::DrawLine(CDC *dc, long  sx,   long sy, long ex, long ey, COLORREF colour)
{ 
   CPen new_pen;
   CPen *old_pen;

   new_pen.CreatePen(PS_SOLID,   1, colour);
   old_pen  = dc->SelectObject(&new_pen);
   dc->MoveTo(sx, sy);
   dc->LineTo(ex, ey);
   dc->SelectObject(old_pen);
   new_pen.DeleteObject();
}


void CColoredButton::DrawButtonText(CDC   *dc, CRect r, const  char *Buf, COLORREF  text_colour)
{
   COLORREF previous_colour;

   previous_colour   = dc->SetTextColor(text_colour);
   dc->SetBkMode(TRANSPARENT);
   dc->DrawText(Buf, strlen(Buf), r, DT_CENTER  | DT_VCENTER | DT_SINGLELINE);
   dc->SetTextColor(previous_colour);
}

//_____________________________________________________________________________
CGroupBox::CGroupBox()
{
   // set disabled box colors
   m_boxColor1_d = ::GetSysColor(COLOR_3DSHADOW);
   m_boxColor2_d = ::GetSysColor(COLOR_3DHILIGHT);

   // set default box colors
   m_boxColor1 = m_boxColor1_d;
   m_boxColor2 = m_boxColor2_d;

   // default text color is black
   m_txtColor = RGB(0,0,0);

   // default font
   CSxLogFont lf1;   
   m_txtFont = new CFont;
   m_txtFont->CreatePointFontIndirect(&lf1);

   // default text
   m_txtString.Empty();

   // default text alignment
   m_txtAlignment = BS_LEFT;

   // default text offset (pixels)
   m_txtHOffset = 8;
   m_txtVOffset = 0;

   // default 3D style
   m_LineStyle = BS_DEFAULT;

   // default bitmap style
   m_StyleBitmap = false;

   // default box line thickness
   m_boxThickness = 1;

}

CGroupBox::~CGroupBox()
{
    if(m_txtFont) delete m_txtFont;
}

BEGIN_MESSAGE_MAP(CGroupBox, CButton)
   //{{AFX_MSG_MAP(CGroupBox)
   ON_WM_PAINT()
   ON_WM_CTLCOLOR_REFLECT()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//
// store groupbox members after creation
//
void CGroupBox::PreSubclassWindow() 
{
   // store the current text (from resource)
   GetWindowText(m_txtString);

   // get current style parameters (from resource definition)
   DWORD dwStyle = GetStyle();
   DWORD dwExStyle = GetExStyle();

   // store the current text alignment
   if((dwStyle & BS_CENTER) == BS_RIGHT)
      m_txtAlignment = BS_RIGHT;
   else if((!(dwStyle & BS_CENTER)) || ((dwStyle & BS_CENTER) == BS_LEFT))
      m_txtAlignment = BS_LEFT;
   else if((dwStyle & BS_CENTER) == BS_CENTER)
      m_txtAlignment = BS_CENTER;

   // store the box rectangle
   GetWindowRect(&m_boxRect);
   ScreenToClient(&m_boxRect);

   // handle the extended window styles
   if(dwExStyle & WS_EX_DLGMODALFRAME)
   {
      m_boxRect.DeflateRect(2*GetSystemMetrics(SM_CXEDGE), 2*GetSystemMetrics(SM_CYEDGE));
      m_boxRect.left--;
   }
   if(dwExStyle & WS_EX_CLIENTEDGE)
   {
      m_boxRect.DeflateRect(GetSystemMetrics(SM_CXEDGE)+1, GetSystemMetrics(SM_CYEDGE)+1);
      m_boxRect.left--;
      if (dwExStyle & WS_EX_DLGMODALFRAME)
         m_boxRect.right++;
   }  
   if(dwExStyle & WS_EX_STATICEDGE)
   {
      m_boxRect.DeflateRect(2*GetSystemMetrics(SM_CXBORDER), 2*GetSystemMetrics(SM_CYBORDER));
      m_boxRect.left--;
   }

   // flat or 3D
   if(dwStyle & BS_FLAT)
   {
      m_LineStyle = 0;
   }
   else
   {
      m_LineStyle = 1;

      if (::IsWindow(m_hWnd))
      {
         WINDOWPLACEMENT windowPlacement;
         GetWindowPlacement(&windowPlacement);

         RECT& rect = windowPlacement.rcNormalPosition;

         rect.left   -= m_boxThickness;
         rect.right  += (m_boxThickness + 1)/2;
         rect.bottom += (m_boxThickness + 1)/2;

         SetWindowPlacement(&windowPlacement);
      }
   }

   // text or bitmap/icon
   if(dwExStyle & (BS_ICON | BS_BITMAP))
      m_StyleBitmap = true;
   else
      m_StyleBitmap = false;

   CButton::PreSubclassWindow();
}
//
// draw the control with custom colors and font
//
void CGroupBox::OnPaint() 
{
   CPoint ptStart, ptEnd;
   CPen pnFrmDark, pnFrmLight, *ppnOldPen;
   CBrush brFrmDark, brFrmLight;
   LOGBRUSH LbrFrmDark, LbrFrmLight;

   CPaintDC dc(this);
   int nSavedDC = dc.SaveDC();

   // add a blank on each side of text
   CString sText;
   sText.Format(" %s ", m_txtString);

   // get the text size used by resource editor
   CSize seText0 = dc.GetTextExtent(sText);

   // set the font to draw
   CFont *oldFont = dc.SelectObject(m_txtFont);

   // get the drawn text size
   CSize seText = dc.GetTextExtent(sText);
//
// handle text alignment (BS_CENTER == BS_LEFT|BS_RIGHT)
//
   // set the box top
   ptStart.y = ptEnd.y = m_boxRect.top + seText0.cy / 2 - 1;

   // set the horizontal points for the text gap
   if(m_txtAlignment == BS_RIGHT)
   {
      ptEnd.x = m_boxRect.right - m_txtHOffset;
      ptStart.x = ptEnd.x - seText.cx;
   }
   else if(m_txtAlignment == BS_LEFT)
   {
      ptStart.x = m_boxRect.left + m_txtHOffset;
      ptEnd.x = ptStart.x + seText.cx;
   }
   else if(m_txtAlignment == BS_CENTER)
   {
      ptStart.x = (m_boxRect.Width() - seText.cx) / 2;
      ptEnd.x = ptStart.x + seText.cx;
   }
//
// create pens to draw with
//
   int PenStyle = PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_SQUARE;
   if(IsWindowEnabled())
   {
      // create brushes
      brFrmDark.CreateSolidBrush(m_boxColor1);
      brFrmDark.GetLogBrush(&LbrFrmDark);
      brFrmLight.CreateSolidBrush(m_boxColor2);
      brFrmLight.GetLogBrush(&LbrFrmLight);
      // create pens
      pnFrmDark.CreatePen(PenStyle, m_boxThickness, &LbrFrmDark);
      pnFrmLight.CreatePen(PenStyle, m_boxThickness, &LbrFrmLight);
   }
   else
   {
      // create brushes
      brFrmDark.CreateSolidBrush(m_boxColor1_d);
      brFrmDark.GetLogBrush(&LbrFrmDark);
      brFrmLight.CreateSolidBrush(m_boxColor2_d);
      brFrmLight.GetLogBrush(&LbrFrmLight);
      // create pens
      pnFrmDark.CreatePen(PenStyle, m_boxThickness, &LbrFrmDark);
      pnFrmLight.CreatePen(PenStyle, m_boxThickness, &LbrFrmLight);
   }
//
// draw the box with text gap
//
   // flat frame style
   if(m_LineStyle == BS_FLAT)
   {
      // draw the box
      ppnOldPen = dc.SelectObject(&pnFrmDark);
      dc.MoveTo(ptStart);
      dc.LineTo(m_boxRect.left,   ptStart.y);
      dc.LineTo(m_boxRect.left,   m_boxRect.bottom);
      dc.LineTo(m_boxRect.right,  m_boxRect.bottom);
      dc.LineTo(m_boxRect.right,  ptEnd.y);
      dc.LineTo(ptEnd);
   }
   // inscribed rectangles
   else if(m_LineStyle == BS_RECT)
   {
      // draw the first box (inner)
      ppnOldPen = dc.SelectObject(&pnFrmDark);
      dc.MoveTo(ptStart);
      dc.LineTo(m_boxRect.left,   ptStart.y);
      dc.LineTo(m_boxRect.left,   m_boxRect.bottom);
      dc.LineTo(m_boxRect.right,  m_boxRect.bottom);
      dc.LineTo(m_boxRect.right,  ptEnd.y);
      dc.LineTo(ptEnd);

      // draw the second box (outer)
      dc.SelectObject(&pnFrmLight);
      dc.MoveTo(ptStart.x,                        ptStart.y - m_boxThickness);
      dc.LineTo(m_boxRect.left - m_boxThickness,  ptStart.y - m_boxThickness);
      dc.LineTo(m_boxRect.left - m_boxThickness,  m_boxRect.bottom + m_boxThickness);
      dc.LineTo(m_boxRect.right + m_boxThickness, m_boxRect.bottom + m_boxThickness);
      dc.LineTo(m_boxRect.right + m_boxThickness, ptEnd.y - m_boxThickness);
      dc.LineTo(ptEnd.x,                          ptEnd.y - m_boxThickness);  
   }
   // 3D hilite
   else
   {
      // draw the first box
      ppnOldPen = dc.SelectObject(&pnFrmDark);
      dc.MoveTo(ptStart);
      dc.LineTo(m_boxRect.left,                   ptStart.y);
      dc.LineTo(m_boxRect.left,                   m_boxRect.bottom - m_boxThickness);
      dc.LineTo(m_boxRect.right - m_boxThickness, m_boxRect.bottom - m_boxThickness);
      dc.LineTo(m_boxRect.right - m_boxThickness, ptEnd.y);
      dc.LineTo(ptEnd);

      // draw the second box (hilite)
      dc.SelectObject(&pnFrmLight);
      dc.MoveTo(ptStart.x,                        ptStart.y + m_boxThickness);
      dc.LineTo(m_boxRect.left + m_boxThickness,  ptStart.y + m_boxThickness);
      dc.LineTo(m_boxRect.left + m_boxThickness,  m_boxRect.bottom - 2*m_boxThickness);
      dc.MoveTo(m_boxRect.left + m_boxThickness,  m_boxRect.bottom);
      dc.LineTo(m_boxRect.right,                  m_boxRect.bottom);
      dc.LineTo(m_boxRect.right,                  ptEnd.y + m_boxThickness);
      dc.MoveTo(m_boxRect.right - 2*m_boxThickness, ptEnd.y + m_boxThickness);
      dc.LineTo(ptEnd.x,                          ptEnd.y + m_boxThickness);  
   }
//
// draw text (if any)
//
   // set color and drawing mode
   COLORREF oldColor = dc.GetTextColor();
   dc.SetTextColor(m_txtColor);
   dc.SetBkMode(TRANSPARENT);

   if(!sText.IsEmpty() && !m_StyleBitmap)
   {
      // draw disabled text
      if(!IsWindowEnabled())
      {
         ptStart.y -= seText.cy / 2;
         dc.DrawState(ptStart, seText, sText, DSS_DISABLED, TRUE, 0, (HBRUSH)NULL);
      }

      // draw enabled text
      else
      {
         // get top of text box
         ptStart.y -= m_txtVOffset;

         dc.DrawText(sText, CRect(ptStart, ptEnd), 
            DT_VCENTER | DT_LEFT | DT_SINGLELINE | DT_NOCLIP);
      }
   }

   // cleanup
   dc.SelectObject(oldFont);
   dc.SetTextColor(oldColor);
   dc.SelectObject(ppnOldPen);
   dc.RestoreDC(nSavedDC);
}

void CGroupBox::setBoxColor(COLORREF primaryColor)
{
   SetBoxColors(primaryColor,primaryColor,true);
   //SetTextColor(primaryColor,true);
}

//
// sets the box colors
//
void CGroupBox::SetBoxColors
(
   COLORREF color1,   // primary color
   COLORREF color2,   // highlight color
   bool redraw        // redraw flag
) 
{ 
   if(color1)
      m_boxColor1 = color1;
   else
      m_boxColor1 = m_boxColor1_d;

   if(color2)
      m_boxColor2 = color2;
   else
      m_boxColor2 = m_boxColor2_d;

   if(redraw) Invalidate(false);
}
//
// gets the box colors
//
void CGroupBox::GetBoxColors
(
   COLORREF& color1,
   COLORREF& color2
) 
{
   color1 = m_boxColor1;
   color2 = m_boxColor2;
}
//
// sets the text color
//
void CGroupBox::SetTextColor(COLORREF color, bool redraw) 
{ 
   m_txtColor = color;
   if(redraw)
      Invalidate();
}
//
// sets the text font
//
void CGroupBox::SetFont(const LOGFONT* lpLogFont)
{
   // create the font
   if(!lpLogFont)
   {
      CSxLogFont lf0;
      lpLogFont = &lf0;
   }

   // delete current font
   if(m_txtFont)
      delete m_txtFont;

   // create new font on heap
   m_txtFont = new CFont;
   m_txtFont->CreatePointFontIndirect(lpLogFont);
}
//
// sets the text
//
void CGroupBox::SetText(CString textStr, bool redraw)
{
   m_txtString = textStr;
   SetWindowText(m_txtString);   

   if(redraw)
   {
      Invalidate(true);
   }
}
//
// sets the text alignment
//
void CGroupBox::SetTextAlign(int TextAlign)
{
   switch(TextAlign)
   {
      case BS_LEFT:
      case BS_CENTER:
      case BS_RIGHT:
      {
         m_txtAlignment = TextAlign;
         break;
      }
      
      default:
         m_txtAlignment = BS_LEFT;
   }
}
//
// sets the horizontal and vertical text offsets
//
void CGroupBox::SetTextOffset(int Hoffset, int Voffset)
{
   m_txtHOffset = Hoffset;
   m_txtVOffset = Voffset;
}
//
// sets the box line style
//
void CGroupBox::SetLineStyle(int StyleLine)
{
   if( (StyleLine != BS_FLAT) && 
       (StyleLine != BS_3D) &&
       (StyleLine != BS_RECT))
   StyleLine = BS_DEFAULT;

   m_LineStyle = StyleLine;

}

void CGroupBox::SetLineThickness(int thickness, bool redraw)
{
   m_boxThickness = thickness;

   if (redraw)
   {
      Invalidate();
   }
};

//
// handle the tab key
//
HBRUSH CGroupBox::CtlColor(CDC* pDC, UINT nCtlColor) 
{
   GetParent()->Invalidate();
   return NULL;
}

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CStandardApertureGeometryViewStatic, CStandardApertureViewStatic)

CStandardApertureGeometryViewStatic::CStandardApertureGeometryViewStatic()
{
}

CStandardApertureGeometryViewStatic::~CStandardApertureGeometryViewStatic()
{
}

CExtent CStandardApertureGeometryViewStatic::getExtent()
{
   return m_extent;
}

void CStandardApertureGeometryViewStatic::setGeometryMicrocosm(CStencilGeometryMicrocosm& geometryMicrocosm,bool placedTopFlag)
{
   geometryMicrocosm.resetRuleStatus();

   if (m_camCadFrame != NULL)
   {
      CCEtoODBDoc* doc = getCamCadDoc();
      CCamCadDocGraph camCadDocGraph(doc);
      BlockStruct& pcbBlock = m_camCadFrame->getPcbBlock();
      int zeroWidthIndex = doc->getZeroWidthIndex();

      // delete everything on all layers
      CCamCadLayerMask layerMask(*m_camCadDatabase);
      layerMask.addAll();
      m_camCadDatabase->deleteEntitiesOnLayers(layerMask);

      if (geometryMicrocosm.getSourceStencilSubPin() != NULL)
      {
         CCamCadDatabase& sourceCamCadDatabase = geometryMicrocosm.getCamCadDatabase();
         //CStencilPin& sourceStencilPin = *(geometryMicrocosm.getSourceStencilPin());
         CStencilSubPin& sourceStencilSubPin = *(geometryMicrocosm.getSourceStencilSubPin());
         CStencilHole& sourceStencilHole = sourceStencilSubPin.getStencilHole(geometryMicrocosm.getSourceTopFlag());

         // knv - experimental fix
         //placedTopFlag = (placedTopFlag == geometryMicrocosm.getSourceTopFlag());

         CDataList& destinationDataList = m_camCadFrame->getPcbBlock().getDataList();

         CDataList stencilPads(true);
         geometryMicrocosm.getStencilPads(stencilPads);
         // knv - experimental fix
         //geometryMicrocosm.getStencilPads(stencilPads,placedTopFlag);

         CStencilPins& stencilPins = geometryMicrocosm.getStencilPins();
         m_extent.reset();

         // knv 20080115 - expetrimental fix
         InsertStruct* componentInsert = geometryMicrocosm.getComponent()->getInsert();
         CTMatrix componentMatrix = componentInsert->getTMatrix();

         BlockStruct* geometry = geometryMicrocosm.getGeometry();
         LayerStruct* axesLayer = m_camCadDatabase->getDefinedLayer(QAxes);

         CTMatrix axesMatrix;
         CPoint2d pinCentroid;

         if (geometry != NULL)
         {
            pinCentroid        = geometry->getPinCentroid();
            CBasesVector majorMinorAxis = geometry->getPinMajorMinorAxes();

            axesMatrix.translateCtm(pinCentroid);
            axesMatrix.rotateDegreesCtm(majorMinorAxis.getRotationDegrees());
            //axesMatrix.mirrorAboutYAxisCtm(componentMatrix.getMirror());
         }

         CTMatrix iAxesMatrix(axesMatrix);
         iAxesMatrix.invert();
         CExtent tAxesExtent;

         for (POSITION pos = stencilPins.GetHeadPosition();pos != NULL;)
         {
            CStencilPin* stencilPin           = stencilPins.GetNext(pos);

            for (int subPinIndex = 0;subPinIndex < stencilPin->getSubPinCount();subPinIndex++)
            {
               CStencilSubPin* subPin = stencilPin->getSubPin(subPinIndex);

               CStencilHole& stencilHole         = subPin->getStencilHole(placedTopFlag);
               CStencilHole& oppositeStencilHole = subPin->getStencilHole(!placedTopFlag);

               LayerStruct* oppositeCopperLayer = m_camCadDatabase->getDefinedLayer(QDimOppositeCopperPad);
               LayerStruct* copperLayer         = m_camCadDatabase->getDefinedLayer(QDimCopperPad);
               LayerStruct* stencilLayer        = m_camCadDatabase->getDefinedLayer(QDimStencilPad);
               LayerStruct* toolHoleLayer       = m_camCadDatabase->getDefinedLayer(QDimToolHole);

               switch (geometryMicrocosm.getAttributeSource())
               {
               case attributeSourceGeometry:
               case attributeSourceComponent:
                  copperLayer   = m_camCadDatabase->getDefinedLayer(QBrightCopperPad);
                  stencilLayer  = m_camCadDatabase->getDefinedLayer(QBrightStencilPad);
                  toolHoleLayer = m_camCadDatabase->getDefinedLayer(QBrightToolHole);

                  geometryMicrocosm.updateRuleStatus(stencilHole.getRuleStatus());
                  break;
               case attributeSourcePadstack:
                  if (sourceStencilSubPin.getPadStackGeometry() == subPin->getPadStackGeometry())
                  {
                     copperLayer   = m_camCadDatabase->getDefinedLayer(QBrightCopperPad);
                     stencilLayer  = m_camCadDatabase->getDefinedLayer(QBrightStencilPad);
                     toolHoleLayer = m_camCadDatabase->getDefinedLayer(QBrightToolHole);

                     geometryMicrocosm.updateRuleStatus(stencilHole.getRuleStatus());
                  }

                  break;
               case attributeSourceGeometryPin:
                  if (sourceStencilSubPin.getPadStackGeometry() == subPin->getPadStackGeometry() &&
                      sourceStencilHole.getSubPadstackName().CompareNoCase(stencilHole.getSubPadstackName()) == 0 &&
                      sourceStencilSubPin.getPinName().CompareNoCase(subPin->getPinName()) == 0)
                  {
                     copperLayer   = m_camCadDatabase->getDefinedLayer(QBrightCopperPad);
                     stencilLayer  = m_camCadDatabase->getDefinedLayer(QBrightStencilPad);
                     toolHoleLayer = m_camCadDatabase->getDefinedLayer(QBrightToolHole);

                     geometryMicrocosm.updateRuleStatus(stencilHole.getRuleStatus());
                  }

                  break;
               case attributeSourceComponentSubPin:
                  if (sourceStencilSubPin.getPinName().CompareNoCase(subPin->getPinName()) == 0 &&
                      sourceStencilSubPin.getSubPinIndex() == subPin->getSubPinIndex() )
                  {
                     copperLayer   = m_camCadDatabase->getDefinedLayer(QBrightCopperPad);
                     stencilLayer  = m_camCadDatabase->getDefinedLayer(QBrightStencilPad);
                     toolHoleLayer = m_camCadDatabase->getDefinedLayer(QBrightToolHole);

                     geometryMicrocosm.updateRuleStatus(stencilHole.getRuleStatus());
                  }

                  break;
               case attributeSourceComponentPin:
                  if (sourceStencilSubPin.getPinName().CompareNoCase(subPin->getPinName()) == 0)
                  {
                     copperLayer   = m_camCadDatabase->getDefinedLayer(QBrightCopperPad);
                     stencilLayer  = m_camCadDatabase->getDefinedLayer(QBrightStencilPad);
                     toolHoleLayer = m_camCadDatabase->getDefinedLayer(QBrightToolHole);

                     geometryMicrocosm.updateRuleStatus(stencilHole.getRuleStatus());
                  }

                  break;
               case attributeSourceSubPadstack:
                  if (sourceStencilSubPin.getPadStackGeometry() == subPin->getPadStackGeometry() &&
                      sourceStencilHole.getSubPadstackName().CompareNoCase(stencilHole.getSubPadstackName()) == 0)
                  {
                     copperLayer   = m_camCadDatabase->getDefinedLayer(QBrightCopperPad);
                     stencilLayer  = m_camCadDatabase->getDefinedLayer(QBrightStencilPad);
                     toolHoleLayer = m_camCadDatabase->getDefinedLayer(QBrightToolHole);

                     geometryMicrocosm.updateRuleStatus(stencilHole.getRuleStatus());
                  }

                  break;
               }

               DataStruct* copperPad  = stencilHole.getCopperPad();

               if (copperPad == NULL)
               {
                  copperPad   = oppositeStencilHole.getCopperPad();
                  copperLayer = oppositeCopperLayer;
               }

               // knv 20080115 - expetrimental fix
               //CTMatrix matrix = stencilPin->getPadStack().getInsert()->getTMatrix();

               DataStruct* stencilPad = stencilHole.getStencilHoleInstance();

               if (copperPad != NULL)
               {
                  CDataList copperDataList(true);

                  bool compmir = stencilPin->getComponent().getInsert()->getMirrorFlags() & MIRROR_FLIP;
                  bool padmir = copperPad->getInsert()->getMirrorFlags() & MIRROR_FLIP;


                  CTMatrix matrix = copperPad->getInsert()->getTMatrix(); // knv 20080115 - expetrimental fix
                  // dts0100506629 - incorrect pad position in preview graphic, when padstack has aperture insert with rotation and is mirrored to bottom
                  if (!placedTopFlag)
                     matrix.rotateDegrees( -2. * copperPad->getInsert()->getAngleDegrees() ); // back up and go the other way

                  // dts0100506629 - incorrect pad position in preview graphic, when padstack has aperture insert with rotation and is mirrored to bottom
                  // This alternative to rotate done above works nicely, but there is not always an associated stencilPad.
                  // Left here as future hint, in case we still get problems in this area, possibly use this if it 
                  // exists and if not then do rotate as above.
                  //if (stencilPad != NULL)
                  //   matrix = stencilPad->getInsert()->getTMatrix();

                  BlockStruct* copperGeometry = sourceCamCadDatabase.getBlock(copperPad->getInsert()->getBlockNumber());
                  flattenAndTransmute(copperDataList,*copperGeometry,sourceCamCadDatabase,copperLayer->getLayerIndex(),matrix);

                  m_extent.update(copperDataList.getExtent(getCamCadDataDocP()));
                  tAxesExtent.update(copperDataList.getExtent(iAxesMatrix));
                  pcbBlock.getDataList().takeData(copperDataList);
               }

               

               if (stencilPad != NULL)
               {
                  CDataList stencilDataList(true);
                  CTMatrix matrix = stencilPad->getInsert()->getTMatrix(); // knv 20080115 - expetrimental fix

                  BlockStruct* stencilGeometry = sourceCamCadDatabase.getBlock(stencilPad->getInsert()->getBlockNumber());
                  flattenAndTransmute(stencilDataList,*stencilGeometry,sourceCamCadDatabase,stencilLayer->getLayerIndex(),matrix);

                  m_extent.update(stencilDataList.getExtent(getCamCadDataDocP()));
                  tAxesExtent.update(stencilDataList.getExtent(iAxesMatrix));
                  pcbBlock.getDataList().takeData(stencilDataList);
               }

               DataStruct* drillTool = stencilPin->getDrillTool();

               if (drillTool != NULL)
               {
                  CDataList toolDataList(true);
                  //CTMatrix matrix = stencilPin->getPadStack().getInsert()->getTMatrix(); // knv 20080115 - expetrimental fix

                  // knv - experimental fix
                  InsertStruct* componentInsert = stencilPin->getComponent().getInsert();
                  CTMatrix componentMatrix = componentInsert->getTMatrix();
                  CTMatrix padstackMatrix = stencilPin->getPadStack().getInsert()->getTMatrix(); 
                  CTMatrix matrix = padstackMatrix * componentMatrix;

                  BlockStruct* toolGeometry = sourceCamCadDatabase.getBlock(drillTool->getInsert()->getBlockNumber());
                  flattenAndTransmute(toolDataList,*toolGeometry,sourceCamCadDatabase,toolHoleLayer->getLayerIndex(),matrix);

                  m_extent.update(toolDataList.getExtent(getCamCadDataDocP()));
                  tAxesExtent.update(toolDataList.getExtent(iAxesMatrix));
                  pcbBlock.getDataList().takeData(toolDataList);
               }
            }
         }

         if (geometry != NULL)
         {
            BlockStruct tempBlock;

            // Process "real" silkscreen
            CDataList& sourceSilkScreenDataList = tempBlock.getDataList();

            for (CDataListIterator silkScreenIterator(*geometry,dataTypePoly);silkScreenIterator.hasNext();)
            {
               DataStruct* silkScreenPolyData = silkScreenIterator.getNext();
               LayerStruct* layer = sourceCamCadDatabase.getLayerAt(silkScreenPolyData->getLayerIndex());

               if (layer->getLayerType() == layerTypeSilkTop)
               {
                  sourceSilkScreenDataList.AddTail(geometryMicrocosm.getCamCadData().getNewDataStruct(*silkScreenPolyData));
               }
            }

            // Fake out mechanical pins, make them like silkscreen circles at same position.
            for (CDataListIterator mechPinIterator(*geometry,insertTypeMechanicalPin);mechPinIterator.hasNext();)
            {
               DataStruct* pin = mechPinIterator.getNext();

               BlockStruct *mechPinBlock = geometryMicrocosm.getCamCadData().getBlockAt(pin->getInsert()->getBlockNumber());
               if (mechPinBlock != NULL)
               {
                  DataStruct *newData = geometryMicrocosm.getCamCadData().getNewDataStruct(*pin);
                  sourceSilkScreenDataList.AddTail(newData);
               }
            }

            CStencilRule stencilRule = sourceStencilHole.getStencilRuleForSource(geometryMicrocosm.getAttributeSource());

            // If stencil rule is PassthruStencilSourceLayer then add all comp geom level StencilSourceLayer polys.
            // This just adds them such that they can be seen in gui, it does not make them selectable
            // for adding stencil rules.
            if (stencilRule.getRule() == stencilRulePassthru)
            {
               CString topLyrName( geometryMicrocosm.getStencilGenerationParameters().getTopStencilSourceLayerName() );
               CString botLyrName( geometryMicrocosm.getStencilGenerationParameters().getBottomStencilSourceLayerName() );
               int topLyrIndx = geometryMicrocosm.getCamCadDatabase().getLayerIndex(topLyrName);
               int botLyrIndx = geometryMicrocosm.getCamCadDatabase().getLayerIndex(botLyrName);

               for (CDataListIterator polyIterator(*geometry,dataTypePoly);polyIterator.hasNext();)
               {
                  DataStruct* polydata = polyIterator.getNext();
                  bool topLyrMatch = (topLyrIndx != -1) ? (polydata->getLayerIndex() == topLyrIndx) : false;
                  bool botLyrMatch = (botLyrIndx != -1) ? (polydata->getLayerIndex() == botLyrIndx) : false;

                  if (topLyrMatch || botLyrMatch)
                  {
                     DataStruct *newData = geometryMicrocosm.getCamCadData().getNewDataStruct(*polydata);
                     sourceSilkScreenDataList.AddTail(newData);
                  }
               }
            }

            if (sourceSilkScreenDataList.GetCount() > 0)
            {
               CDataList targetSilkScreenDataList(true);
               LayerStruct* silkScreenLayer  = m_camCadDatabase->getDefinedLayer(QSilkScreen);
               //CTMatrix matrix; // knv 20080115 - expetrimental fix
               CTMatrix matrix(componentMatrix);

               flattenAndTransmute(targetSilkScreenDataList,tempBlock,sourceCamCadDatabase,
                  silkScreenLayer->getLayerIndex(),matrix);

               m_extent.update(targetSilkScreenDataList.getExtent(getCamCadDataDocP()));
               tAxesExtent.update(targetSilkScreenDataList.getExtent(iAxesMatrix));
               //m_extent.update(silkScreenPolyData->getPolyList()->getExtent());
               pcbBlock.getDataList().takeData(targetSilkScreenDataList);
            }




            // add axii
            //LayerStruct* axesLayer = m_camCadDatabase->getDefinedLayer(QAxes);
            //CPoint2d pinCentroid   = geometry->getPinCentroid();
            //CBasesVector majorMinorAxis = geometry->getPinMajorMinorAxes();

            //CTMatrix matrix;
            //matrix.translateCtm(pinCentroid);
            //matrix.rotateDegreesCtm(majorMinorAxis.getRotationDegrees());
            //CTMatrix iAxesMatrix(matrix);
            //iAxesMatrix.invert();
            //CExtent tAxesExtent;

            //for (CDataListIterator pinIterator(*geometry,insertTypePin);pinIterator.hasNext();)
            //{
            //   DataStruct* pin = pinIterator.getNext();
            //   CPoint2d pinOrigin = pin->getInsert()->getOrigin2d();
            //   iAxesMatrix.transform(pinOrigin);
            //   tAxesExtent.update(pinOrigin);
            //}

            //CStencilRule stencilRule = sourceStencilHole.getStencilRuleForSource(geometryMicrocosm.getAttributeSource());
            bool flipSymmetryAxisFlag = stencilRule.hasModifier(stencilRuleModifierFlipSymmetryAxis);

            const double spacingFactor = .05;
            tAxesExtent.expand(max(tAxesExtent.getXsize(),tAxesExtent.getYsize()) * spacingFactor * 3.);
            double arrowSize = max(tAxesExtent.getXsize(),tAxesExtent.getYsize()) * spacingFactor;

            CPoint2d origin(componentInsert->getGraphicMirrored() ? -pinCentroid.x : pinCentroid.x,pinCentroid.y);
            origin.transform(iAxesMatrix);

            DataStruct* axesPolyStruct = newDataStruct(dataTypePoly);
            axesPolyStruct->setLayerIndex(axesLayer->getLayerIndex());
            pcbBlock.getDataList().AddTail(axesPolyStruct);
            CPoly* poly;

            if (!flipSymmetryAxisFlag)
            {
               poly = new CPoly();
               poly->addVertex(tAxesExtent.getXmin()            ,origin.y);
               poly->addVertex(tAxesExtent.getXmin() - arrowSize,origin.y + arrowSize/2.);
               poly->addVertex(tAxesExtent.getXmin() - arrowSize,origin.y - arrowSize/2.);
               poly->close();
               poly->setFilled(true);
               poly->setWidthIndex(zeroWidthIndex);
               axesPolyStruct->getPolyList()->AddTail(poly);

               poly = new CPoly();
               poly->addVertex(tAxesExtent.getXmax()            ,origin.y);
               poly->addVertex(tAxesExtent.getXmax() + arrowSize,origin.y - arrowSize/2.);
               poly->addVertex(tAxesExtent.getXmax() + arrowSize,origin.y + arrowSize/2.);
               poly->close();
               poly->setFilled(true);
               poly->setWidthIndex(zeroWidthIndex);
               axesPolyStruct->getPolyList()->AddTail(poly);
            }

            poly = new CPoly();
            poly->addVertex(tAxesExtent.getXmin() - arrowSize,origin.y);
            poly->addVertex(tAxesExtent.getXmax() + arrowSize,origin.y);
            poly->setWidthIndex(zeroWidthIndex);
            axesPolyStruct->getPolyList()->AddTail(poly);

            if (flipSymmetryAxisFlag)
            {
               CPoly* poly = new CPoly();
               poly->addVertex(origin.x               ,tAxesExtent.getYmin());
               poly->addVertex(origin.x - arrowSize/2.,tAxesExtent.getYmin() - arrowSize);
               poly->addVertex(origin.x + arrowSize/2.,tAxesExtent.getYmin() - arrowSize);
               poly->close();
               poly->setFilled(true);
               poly->setWidthIndex(zeroWidthIndex);
               axesPolyStruct->getPolyList()->AddTail(poly);

               poly = new CPoly();
               poly->addVertex(origin.x               ,tAxesExtent.getYmax());
               poly->addVertex(origin.x + arrowSize/2.,tAxesExtent.getYmax() + arrowSize);
               poly->addVertex(origin.x - arrowSize/2.,tAxesExtent.getYmax() + arrowSize);
               poly->close();
               poly->setFilled(true);
               poly->setWidthIndex(zeroWidthIndex);
               axesPolyStruct->getPolyList()->AddTail(poly);
            }

            poly = new CPoly();
            poly->addVertex(origin.x,tAxesExtent.getYmin() - arrowSize);
            poly->addVertex(origin.x,tAxesExtent.getYmax() + arrowSize);
            poly->setWidthIndex(zeroWidthIndex);
            axesPolyStruct->getPolyList()->AddTail(poly);

            axesPolyStruct->transform(axesMatrix);

            if (getCamCadDoc() != NULL)
               m_extent.update(axesPolyStruct->getPolyList()->getExtent(*(getCamCadDataDocP())));
            else
               m_extent.update(axesPolyStruct->getPolyList()->getExtent());
         }

         //CStdioFileWriteFormat writeFormat("c:\\StencilGeneratorDebug.cc",2048);
         //m_camCadFrame->getCamCadDatabase().getCamCadDoc().WriteXML(writeFormat);

         calcScaling(1.2);
      }
   }
}

void CStandardApertureGeometryViewStatic::flattenAndTransmute(CDataList& targetDataList,BlockStruct& sourceGeometry,
   CCamCadDatabase& sourceCamCadDatabase,int targetLayerIndex,CTMatrix matrix)
{
   // Case dts0100393020
   // We have encountered sample data that has a pad block that is m_blocktype=drill hole and m_flags = aperture.
   // It is at once a drill hole and an aperture, happens to be a complex aperture at that.
   // Origninal code form of
   //
   // if (is drill hole)
   // ...
   // else if (is aperture)
   // ...
   // else
   // ...
   // 
   // failed to collect aperture geometry. It turns out the drill was size zero, so I am guessing the data
   // was wrong and it really wasn't a drill hole. It worked to just separate the isDrill and isAperture, so
   // it would process both. But since the drill size was zero, it really added nothing to the process.
   // So the final form is to process the drill hole only if it is not also an aperture.

   if (getCamCadData() == NULL)
   {
      return;
   }

   if (sourceGeometry.isDrillHole() && !sourceGeometry.isAperture())
   {
      DataStruct* newPolyStruct = m_camCadDatabase->newPolyStruct(targetLayerIndex,0,false,graphicClassNormal);
      CPoly* poly = m_camCadDatabase->addFilledPoly(newPolyStruct,m_camCadDatabase->getZeroWidthIndex());
      double drillRadius = sourceGeometry.getToolSize()/2.;
      poly->addVertex( drillRadius,0.,1.);
      poly->addVertex(-drillRadius,0.,1.);
      poly->addVertex( drillRadius,0.);
      poly->close();
      newPolyStruct->transform(matrix);

      targetDataList.AddTail(newPolyStruct);
   }
   else if (sourceGeometry.isAperture())
   {
      CUseVoidsInDonuts useVoidsInDonuts(false);

      CPolyList* sourcePolyList = sourceGeometry.getAperturePolys(sourceCamCadDatabase.getCamCadData());

      for (POSITION pos = sourcePolyList->GetHeadPosition();pos != NULL;)
      {
         CPoly* poly = sourcePolyList->GetNext(pos);

         int widthIndex = poly->getWidthIndex();

         if (widthIndex >= 0)
         {
            double width = sourceCamCadDatabase.getCamCadDoc().getWidth(widthIndex);
            int transmutedWidthIndex = m_camCadDatabase->getDefinedWidthIndex(width);
            poly->setWidthIndex(transmutedWidthIndex);
         }
         else
         {
            poly->setWidthIndex(m_camCadDatabase->getZeroWidthIndex());
         }
      }

      DataStruct* newPolyStruct = m_camCadDatabase->newPolyStruct(targetLayerIndex,0,false,graphicClassNormal);
      newPolyStruct->getPolyList()->takeData(*sourcePolyList);
      newPolyStruct->transform(matrix);
      targetDataList.AddTail(newPolyStruct);

      delete sourcePolyList;
   }
   else
   {
      CDataList& sourceDataList = sourceGeometry.getDataList();

      for (POSITION pos = sourceDataList.GetHeadPosition();pos != NULL;)
      {
         DataStruct* sourceData = sourceDataList.GetNext(pos);

         if (sourceData->getDataType() == dataTypeInsert)
         {
            InsertStruct* sourceInsert = sourceData->getInsert();
            BlockStruct* sourceSubBlock = sourceCamCadDatabase.getBlock(sourceInsert->getBlockNumber());

            CTMatrix insertMatrix(matrix);
            insertMatrix.translateCtm(sourceInsert->getOriginX(),sourceInsert->getOriginY());
            insertMatrix.rotateRadiansCtm(sourceInsert->getAngle());
            insertMatrix.scaleCtm(((sourceInsert->getMirrorFlags() & MIRROR_FLIP) != 0) ? -1 : 1,1);

            flattenAndTransmute(targetDataList,*sourceSubBlock,sourceCamCadDatabase,targetLayerIndex,insertMatrix);
         }
         else
         {
            DataStruct* targetData = newDataStruct(*sourceData,false);
            targetData->transform(matrix);
            targetDataList.AddTail(targetData);

            targetData->setLayerIndex(targetLayerIndex);

            if (sourceData->getDataType() == dataTypePoly)
            {
               CPolyList* polyList = targetData->getPolyList();

               for (POSITION pos = polyList->GetHeadPosition();pos != NULL;)
               {
                  CPoly* poly = polyList->GetNext(pos);
                  int widthIndex = poly->getWidthIndex();

                  if (widthIndex >= 0)
                  {
                     double width = sourceCamCadDatabase.getCamCadDoc().getWidth(widthIndex);
                     int transmutedWidthIndex = m_camCadDatabase->getDefinedWidthIndex(width);
                     poly->setWidthIndex(transmutedWidthIndex);
                  }
                  else
                  {
                     poly->setWidthIndex(m_camCadDatabase->getZeroWidthIndex());
                  }
               }
            }
            else if (sourceData->getDataType() == dataTypeText)
            {
               // pen width index

            }
         }
      }
   }
}
//
//void CStandardApertureGeometryViewStatic::setStandardApertureShape(CStandardApertureShape& standardApertureShape)
//{
//   //m_standardApertureShape = &standardApertureShape;
//   delete m_standardApertureShape;
//   m_standardApertureShape = CStandardApertureShape::create(standardApertureShape);
//
//   if (m_camCadFrame != NULL)
//   {
//      CCEtoODBDoc* doc = getCamCadDoc();
//      CCamCadDocGraph camCadDocGraph(doc);
//
//      CCamCadLayerMask layerMask(*m_camCadDatabase);
//      layerMask.add(m_camCadDatabase->getDefinedLayer(QBrightStencilPad)->getLayerIndex());
//      layerMask.add(m_camCadDatabase->getDefinedLayer(QDimStencilPad)->getLayerIndex());
//      m_camCadDatabase->deleteEntitiesOnLayers(layerMask);
//
//      LayerStruct* layer = m_camCadDatabase->getDefinedLayer(QBrightStencilPad);
//      int layerIndex = layer->getLayerIndex();
//
//      DataStruct* polyStruct = m_camCadDatabase->newPolyStruct(layerIndex,0,false,graphicClassNormal);
//      m_standardApertureShape->getAperturePoly(*m_camCadDatabase,NULL,polyStruct);
//
//      m_camCadFrame->getPcbBlock().getDataList().AddTail(polyStruct);
//
//      calcScaling();
//   }
//}

//void CStandardApertureGeometryViewStatic::Draw(CDC* dc)
//{
//   CExtent extent = getExtent();
//   extent.scale(1.6);
//
//   m_camCadFrame->getCamCadView().DrawFiles(dc,extent.getXsize(),extent.getXsize());
//}

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CStandardAperturePinViewStatic, CStandardApertureGeometryViewStatic)

CStandardAperturePinViewStatic::CStandardAperturePinViewStatic()
{
}

CStandardAperturePinViewStatic::~CStandardAperturePinViewStatic()
{
}

void CStandardAperturePinViewStatic::setGeometryMicrocosm(CStencilGeometryMicrocosm& geometryMicrocosm,bool placedTopFlag)
{
   if (m_camCadFrame != NULL)
   {
      CCEtoODBDoc* doc = getCamCadDoc();
      CCamCadDocGraph camCadDocGraph(doc);
      BlockStruct& pcbBlock = m_camCadFrame->getPcbBlock();

      // delete everything on all layers
      CCamCadLayerMask layerMask(*m_camCadDatabase);
      layerMask.addAll();
      m_camCadDatabase->deleteEntitiesOnLayers(layerMask);

      if (geometryMicrocosm.getSourceStencilSubPin() != NULL)
      {
         CCamCadDatabase& sourceCamCadDatabase = geometryMicrocosm.getCamCadDatabase();
         CStencilSubPin& sourceStencilSubPin = *(geometryMicrocosm.getSourceStencilSubPin());
         //int sourceHoleIndex = geometryMicrocosm.getSourceHoleIndex();
         //CStencilHole& sourceStencilHole = sourceStencilSubPin.getStencilHole(geometryMicrocosm.getSourceTopFlag());
         CStencilHole& sourceStencilHole = sourceStencilSubPin.getStencilHole(placedTopFlag); // knv 20080115 - expetrimental fix
         CString sourceStencilPinName = sourceStencilSubPin.getPinName();

         CDataList& destinationDataList = m_camCadFrame->getPcbBlock().getDataList();

         CDataList stencilPads(true);
         geometryMicrocosm.getStencilPads(stencilPads);
         CStencilPins& stencilPins = geometryMicrocosm.getStencilPins();
         m_extent.reset();

         LayerStruct* oppositeCopperLayer = m_camCadDatabase->getDefinedLayer(QDimOppositeCopperPad);
         LayerStruct* copperLayer         = m_camCadDatabase->getDefinedLayer(QBrightCopperPad);
         LayerStruct* stencilLayer        = m_camCadDatabase->getDefinedLayer(QBrightStencilPad);
         LayerStruct* toolHoleLayer       = m_camCadDatabase->getDefinedLayer(QBrightToolHole);

         for (POSITION pos = stencilPins.GetHeadPosition();pos != NULL;)
         {
            CStencilPin* stencilPin = stencilPins.GetNext(pos);

            for (int subPinIndex = 0;subPinIndex < stencilPin->getSubPinCount();subPinIndex++)
            {
               CStencilSubPin* stencilSubPin = stencilPin->getSubPin(subPinIndex);

               if (stencilSubPin->getPinName() == sourceStencilPinName)
               {
                  //// Case dts0100393141
                  //// This used to always only get the top stencil hole, which worked fine for regular top-built
                  //// geometry inserted top or bottom/mirrored. It does not work for bottom-only geometry inserted
                  //// bottom, like Zuken reader makes. Ultimately what we want is the copper pad, so just look
                  //// at top and bottom holes and take first non-null copper pad. If the top copper is not
                  //// null thenuse it, if it is use teh bottom. If bottom is null it doesn't matter, there are
                  //// no other alternatives.

                  //bool useTop = stencilSubPin->getStencilHole(true).getCopperPad() != NULL;
                  //DataStruct* copperPad  = stencilSubPin->getStencilHole(useTop).getCopperPad();
                  //DataStruct* stencilPad = stencilSubPin->getStencilHole(useTop).getStencilHoleInstance();

                  DataStruct* copperPad  = stencilSubPin->getStencilHole(placedTopFlag).getCopperPad();
                  DataStruct* stencilPad = stencilSubPin->getStencilHole(placedTopFlag).getStencilHoleInstance();

                  double padstackAngleRadians = stencilPin->getPadStack().getInsert()->getAngleRadians();
                  //double padstackAngleDegrees = stencilPin->getPadStack().getInsert()->getAngleDegrees();

                  //DataStruct* othercopperPad  = stencilSubPin->getStencilHole(!bogusplacedTopFlag).getCopperPad();
                  //DataStruct* otherstencilPad = stencilSubPin->getStencilHole(!bogusplacedTopFlag).getStencilHoleInstance();

                  // knv - experimental fix
                  //CTMatrix matrix = stencilPin->getPadStack().getInsert()->getTMatrix();
                  //matrix.rotateRadiansCtm(padstackAngleRadians);

                  if (copperPad == NULL)
                  {
                     copperPad   = stencilSubPin->getStencilHole(!placedTopFlag).getCopperPad();
                     copperLayer = oppositeCopperLayer;
                  }

                  CTMatrix stencilPadMatrix;
                  if (stencilPad != NULL)
                  {
                     stencilPadMatrix = stencilPad->getInsert()->getTMatrix(); // knv - experimental fix
                     stencilPadMatrix.rotateRadiansCtm(padstackAngleRadians);
                  }

                  if (copperPad != NULL)
                  {
                     CDataList copperDataList(true);

                     CTMatrix matrix = copperPad->getInsert()->getTMatrix(); // knv - experimental fix (rcf - that didn't quite do enough)
                     matrix.rotateRadiansCtm(padstackAngleRadians);

                     // dts0100506629 - incorrect pad position in preview graphic, when padstack has aperture insert with rotation and is mirrored to bottom
                     if (!placedTopFlag)
                        matrix.rotateDegrees( -2. * copperPad->getInsert()->getAngleDegrees() ); // back up and go the other way

                     BlockStruct* copperGeometry = sourceCamCadDatabase.getBlock(copperPad->getInsert()->getBlockNumber());
                     flattenAndTransmute(copperDataList,*copperGeometry,sourceCamCadDatabase,
                        copperLayer->getLayerIndex(), matrix);

                     m_copperExtent = copperDataList.getExtent( getCamCadDataDocP() );
                     m_extent.update(m_copperExtent);
                     pcbBlock.getDataList().takeData(copperDataList);
                  }

                  if (stencilPad != NULL)
                  {
                     CDataList stencilDataList(true);
                     CTMatrix matrix = stencilPad->getInsert()->getTMatrix(); // knv - experimental fix
                     matrix.rotateRadiansCtm(padstackAngleRadians);

                     BlockStruct* stencilGeometry = sourceCamCadDatabase.getBlock(stencilPad->getInsert()->getBlockNumber());
                     flattenAndTransmute(stencilDataList,*stencilGeometry,sourceCamCadDatabase,
                        stencilLayer->getLayerIndex(),matrix);

                     m_extent.update(stencilDataList.getExtent( getCamCadDataDocP() ));
                     pcbBlock.getDataList().takeData(stencilDataList);
                  }

                  DataStruct* drillTool = stencilPin->getDrillTool();

                  if (drillTool != NULL)
                  {
                     CDataList toolDataList(true);

                     //InsertStruct* componentInsert = stencilPin->getComponent().getInsert();
                     //CTMatrix componentMatrix = componentInsert->getTMatrix();

                     //CTMatrix padstackMatrix = stencilPin->getPadStack().getInsert()->getTMatrix(); // knv - experimental fix
                     //padstackMatrix.rotateRadiansCtm(padstackAngleRadians);

                     //CTMatrix matrix = padstackMatrix * componentMatrix;

                     //CTMatrix matrix = stencilPin->getPadStack().getInsert()->getTMatrix(); // knv - experimental fix
                     //matrix.rotateRadiansCtm(padstackAngleRadians);

                     // knv - experimental fix 2
                     InsertStruct* componentInsert = stencilPin->getComponent().getInsert();
                     CTMatrix componentMatrix = componentInsert->getTMatrix();
                     CTMatrix padstackMatrix = stencilPin->getPadStack().getInsert()->getTMatrix(); 
                     CTMatrix matrix = padstackMatrix * componentMatrix;

                     BlockStruct* toolGeometry = sourceCamCadDatabase.getBlock(drillTool->getInsert()->getBlockNumber());
                     flattenAndTransmute(toolDataList,*toolGeometry,sourceCamCadDatabase,toolHoleLayer->getLayerIndex(),matrix);

                     m_extent.update(toolDataList.getExtent( getCamCadDataDocP() ));
                     pcbBlock.getDataList().takeData(toolDataList);
                  }
               }
            }
         }

         calcScaling(1.2);
      }
   }
}

void CStandardAperturePinViewStatic::Draw(CDC* dc)
{
   //CStdioFileWriteFormat writeFormat("c:\\_StencilTest\\StencilGeneratorDebug.cc",2048);
   //getCamCadDoc()->WriteXML(writeFormat,false);

   CStandardApertureGeometryViewStatic::Draw(dc);

   //CStdioFileWriteFormat writeFormat2("c:\\_StencilTest\\StencilGeneratorDebug2.cc",2048);
   //getCamCadDoc()->WriteXML(writeFormat2,false);
}

void CStandardAperturePinViewStatic::setStandardApertureShape(CStandardApertureShape& standardApertureShape)
{
   m_extent = standardApertureShape.getExtent();
   CStandardApertureGeometryViewStatic::setStandardApertureShape(standardApertureShape);
}

//_____________________________________________________________________________
CStencilRuleComboBox::CStencilRuleComboBox()
{
}

CStencilRuleComboBox::~CStencilRuleComboBox()
{
}

void CStencilRuleComboBox::initialize()
{
   ResetContent();

   int itemIndex;

   itemIndex = AddString(stencilRuleTagToRuleString(stencilRuleNone));
   SetItemData(itemIndex,stencilRuleNone);

   itemIndex = AddString(stencilRuleTagToRuleString(stencilRuleArea));
   SetItemData(itemIndex,stencilRuleArea);

   itemIndex = AddString(stencilRuleTagToRuleString(stencilRuleInset));
   SetItemData(itemIndex,stencilRuleInset);

   itemIndex = AddString(stencilRuleTagToRuleString(stencilRuleAperture));
   SetItemData(itemIndex,stencilRuleAperture);

   itemIndex = AddString(stencilRuleTagToRuleString(stencilRuleApertureGeometry));
   SetItemData(itemIndex,stencilRuleApertureGeometry);

   itemIndex = AddString(stencilRuleTagToRuleString(stencilRuleStencilSourceLayer));
   SetItemData(itemIndex,stencilRuleStencilSourceLayer);

   itemIndex = AddString(stencilRuleTagToRuleString(stencilRulePassthru));
   SetItemData(itemIndex,stencilRulePassthru);

   itemIndex = AddString(stencilRuleTagToRuleString(stencilRuleCopperSourceLayer));
   SetItemData(itemIndex,stencilRuleCopperSourceLayer);

   itemIndex = AddString(stencilRuleTagToRuleString(stencilRuleInheritFromParent));
   SetItemData(itemIndex,stencilRuleInheritFromParent);

   //itemIndex = AddString("");
   //SetItemData(itemIndex,stencilRuleUndefined);
}

StencilRuleTag CStencilRuleComboBox::getSelected()
{
   StencilRuleTag selectedValue;
   int selectedIndex = GetCurSel();

   if (selectedIndex == CB_ERR)
   {
      selectedValue = stencilRuleUndefined;
   }
   else
   {
      selectedValue = intToStencilRuleTag(GetItemData(selectedIndex));
   }

   return selectedValue;
}

void CStencilRuleComboBox::setSelected(StencilRuleTag ruleTag)
{
   int selectedIndex = -1;

   for (int index = 0;index < GetCount();index++)
   {
      StencilRuleTag itemRuleTag = intToStencilRuleTag(GetItemData(index));

      if (itemRuleTag == ruleTag)
      {
         selectedIndex = index;
         break;
      }
   }

   SetCurSel(selectedIndex);
}

//_____________________________________________________________________________
CStencilRuleModifierComboBox::CStencilRuleModifierComboBox()
{
}

CStencilRuleModifierComboBox::~CStencilRuleModifierComboBox()
{
}

void CStencilRuleModifierComboBox::initialize(StencilRuleTag ruleTag)
{
   ResetContent();
   int index;

   switch (ruleTag)
   {
   case stencilRuleAperture:
      index = AddString(stencilRuleModifierToDescription(stencilRuleModifierAlignApertureMajorAxisTowardComponentCenterline));
      SetItemData(index,stencilRuleModifierAlignApertureMajorAxisTowardComponentCenterline);

      index = AddString(stencilRuleModifierToDescription(stencilRuleModifierAlignApertureMajorAxisAwayFromComponentCenterline));
      SetItemData(index,stencilRuleModifierAlignApertureMajorAxisAwayFromComponentCenterline);

      index = AddString(stencilRuleModifierToDescription(stencilRuleModifierOffsetRelativeToPadstack));
      SetItemData(index,stencilRuleModifierOffsetRelativeToPadstack);

      index = AddString("");
      SetItemData(index,stencilRuleModifierUndefined);

      break;
   case stencilRuleApertureGeometry:
      index = AddString(stencilRuleModifierToDescription(stencilRuleModifierOffsetRelativeToPadstack));
      SetItemData(index,stencilRuleModifierOffsetRelativeToPadstack);

      index = AddString("");
      SetItemData(index,stencilRuleModifierUndefined);

      break;
   case stencilRuleArea:
   case stencilRuleInset:
      index = AddString(stencilRuleModifierToDescription(stencilRuleModifierWebSettings));
      SetItemData(index,stencilRuleModifierWebSettings);

      index = AddString("");
      SetItemData(index,stencilRuleModifierUndefined);

      break;
   default:
      CString message;
      message.Format("<%s> has no modifiers",stencilRuleTagToRuleString(ruleTag));

      AddString(message);

      break;
   }
}

StencilRuleModifierTag CStencilRuleModifierComboBox::getSelected()
{
   StencilRuleModifierTag selectedValue;
   int selectedIndex = GetCurSel();

   if (selectedIndex == CB_ERR)
   {
      selectedValue = stencilRuleModifierUndefined;
   }
   else
   {
      selectedValue = intToStencilRuleModifierTag(GetItemData(selectedIndex));
   }

   return selectedValue;
}

void CStencilRuleModifierComboBox::setSelected(StencilRuleModifierTag modifierTag)
{
   int selectedIndex = -1;

   for (int index = 0;index < GetCount();index++)
   {
      StencilRuleModifierTag itemModifierTag = intToStencilRuleModifierTag(GetItemData(index));

      if (itemModifierTag == modifierTag)
      {
         selectedIndex = index;
         break;
      }
   }

   SetCurSel(selectedIndex);
}

//_____________________________________________________________________________
CStencilRuleValueComboBox::CStencilRuleValueComboBox()
{
   m_enableEditing = false;
   m_editFlag      = false;
}

CStencilRuleValueComboBox::~CStencilRuleValueComboBox()
{
}

CString CStencilRuleValueComboBox::getSelected()
{
   CString selectedValue;
   int selectionIndex = GetCurSel();

   if (selectionIndex == CB_ERR || getEditFlag())
   {
      selectedValue = m_currentEditBoxText;
   }
   else
   {
      GetLBText(selectionIndex,selectedValue);
   }

   selectedValue.Trim();

   return selectedValue;
}

void CStencilRuleValueComboBox::setSelected(const CString& text)
{
   int selectedIndex = FindStringExact(-1,text);

   if (selectedIndex != CB_ERR)
   {
      SetCurSel(selectedIndex);

      //CString text;
      //GetLBText(selectedIndex,text);

      //SetWindowText(text);
   }
   else
   {
      CString normalizedValue(text);
      normalizedValue.Trim();

      if (!normalizedValue.IsEmpty())
      {
         SetCurSel(AddString(normalizedValue));
      }
   }

   clearEditFlag();
}

void CStencilRuleValueComboBox::initialize(StencilRuleTag ruleTag)
{
   if (ruleTag != stencilRuleUndefined)
   {
      CMapSortStringToOb& valuesMap = CTreeListAttributeValueComboBox::getValues(ruleTag);
      CObject* object;
      CString key;
      int index;
      int currentSelection = GetCurSel();
      CString currentValue;

      if (currentSelection != CB_ERR)
      {
         GetLBText(currentSelection,currentValue);
      }

      ResetContent();

      for (valuesMap.rewind(index);valuesMap.next(object,key,index);)
      {
         AddString(key);
         //currentSelection = 0;
      }

      if (!currentValue.IsEmpty())
      {
         currentSelection = FindStringExact(-1,currentValue);
      }

      if (currentSelection < 0)
      {
         if (ruleTag == stencilRuleAperture)
         {
            currentSelection = FindStringExact(-1,"null0");

            if (currentSelection < 0)
            {
               currentSelection = AddString("null0");
            }
         }
         else
         {
            currentSelection = 0;
         }
      }

      SetCurSel(currentSelection);

      //if (attributeTag == stencilRuleAperture)
      //{
      //   //comboBox->AddString(QSelectAperture);
      //}
      //else if (attributeTag == stencilRuleApertureGeometry)
      //{
      //   AddString(QSelectApertureGeometry);
      //}
   }
   else
   {
      ResetContent();
   }

   clearEditFlag();
}

void CStencilRuleValueComboBox::clearEditFlag()
{
   m_editFlag = false;
}

void CStencilRuleValueComboBox::setEditFlag(bool flag)
{
   m_editFlag = flag;
}

void CStencilRuleValueComboBox::setEnableEditing(bool flag)
{
   m_enableEditing = flag;
}

BEGIN_MESSAGE_MAP(CStencilRuleValueComboBox, CComboBox)
   //{{AFX_MSG_MAP(CStencilRuleValueComboBox)
   ON_CONTROL_REFLECT(CBN_SELENDCANCEL, CStencilRuleValueComboBox::OnSelCancel)
   //ON_CONTROL_REFLECT(CBN_CLOSEUP, CStencilRuleValueComboBox::OnCloseUp)
   //ON_CONTROL_REFLECT(CBN_KILLFOCUS, CStencilRuleValueComboBox::OnKillFocus)
   ON_CONTROL_REFLECT(CBN_EDITUPDATE, CStencilRuleValueComboBox::OnEditUpdate)
   ON_CONTROL_REFLECT(CBN_SELCHANGE, CStencilRuleValueComboBox::OnSelChange)
   //}}AFX_MSG_MAP
   ON_CONTROL_REFLECT(CBN_SETFOCUS, OnCbnSetfocus)
END_MESSAGE_MAP()

void CStencilRuleValueComboBox::OnSelCancel()
{
   //CString strText;
   //CTreeListCtrl* pParent = (CTreeListCtrl*)GetParent();
   //ASSERT( pParent );
   //
   //pParent->SetFocus();
}

//void CStencilRuleValueComboBox::OnCloseUp()
//{
//   //CString strText;
//   //CTreeListCtrl* pParent = (CTreeListCtrl*)GetParent();
//   //ASSERT( pParent );
//   //
//   //pParent->SetFocus();
//}

//void CStencilRuleValueComboBox::OnKillFocus()
//{
//   //ShowWindow(SW_HIDE);
//}

void CStencilRuleValueComboBox::OnEditUpdate()
{
   // We need to hold last entered text to set it as an item description, 'cause
   // combo box sets matched text into it's edit part

   if (m_enableEditing)
   {
      GetWindowText(m_currentEditBoxText);
      m_editFlag = true;
   }
   else
   {
      SetWindowText(m_currentEditBoxText);
   }
}

void CStencilRuleValueComboBox::OnCbnSetfocus()
{
   GetWindowText(m_currentEditBoxText);
   m_editFlag = false;
}

void CStencilRuleValueComboBox::OnSelChange()
{
   GetWindowText(m_currentEditBoxText);
}

//_____________________________________________________________________________
CTreeListStencilRuleComboBox::CTreeListStencilRuleComboBox(int column)
: CTreeListComboBox(column)
{
   m_enableEditing = false;
}

CTreeListStencilRuleComboBox::~CTreeListStencilRuleComboBox()
{
}

CTreeListComboBox* CTreeListStencilRuleComboBox::create(CRect rect,CWnd* parentWnd)
{
   rect.bottom += 5*rect.Height();

   CTreeListComboBox* comboBox = new CTreeListStencilRuleComboBox(m_column);
   DWORD dwComboBoxStyle = WS_CHILD | WS_VSCROLL | CBS_DROPDOWNLIST;
   comboBox->Create(dwComboBoxStyle,rect,parentWnd,0);

   int index;

   index = comboBox->AddString(stencilRuleTagToRuleString(stencilRuleNone));
   comboBox->SetItemData(index,stencilRuleNone);

   index = comboBox->AddString(stencilRuleTagToRuleString(stencilRuleArea));
   comboBox->SetItemData(index,stencilRuleArea);

   index = comboBox->AddString(stencilRuleTagToRuleString(stencilRuleInset));
   comboBox->SetItemData(index,stencilRuleInset);

   index = comboBox->AddString(stencilRuleTagToRuleString(stencilRuleAperture));
   comboBox->SetItemData(index,stencilRuleAperture);

   index = comboBox->AddString(stencilRuleTagToRuleString(stencilRuleApertureGeometry));
   comboBox->SetItemData(index,stencilRuleApertureGeometry);

   index = comboBox->AddString(stencilRuleTagToRuleString(stencilRuleStencilSourceLayer));
   comboBox->SetItemData(index,stencilRuleStencilSourceLayer);

   index = comboBox->AddString(stencilRuleTagToRuleString(stencilRulePassthru));
   comboBox->SetItemData(index,stencilRulePassthru);

   index = comboBox->AddString(stencilRuleTagToRuleString(stencilRuleCopperSourceLayer));
   comboBox->SetItemData(index,stencilRuleCopperSourceLayer);

   index = comboBox->AddString(stencilRuleTagToRuleString(stencilRuleInheritFromParent));
   comboBox->SetItemData(index,stencilRuleInheritFromParent);

   //index = comboBox->AddString(stencilRuleToRuleString(stencilRuleGroup));
   //comboBox->SetItemData(index,stencilRuleGroup);

   index = comboBox->AddString(QRemoveRule);
   comboBox->SetItemData(index,stencilRuleUndefined);

   return comboBox;
}

void CTreeListStencilRuleComboBox::setSelectedText(const CString& text)
{
   SelectString(-1,text);
}

afx_msg void CTreeListStencilRuleComboBox::OnSelCancel()
{
	CStencilTreeListCtrl* treeListCtrl = (CStencilTreeListCtrl*)GetParent();
	if (treeListCtrl != NULL)
	{
		SetCurSel(-1);
		treeListCtrl->SelectItem(NULL);
	}
}

afx_msg void CTreeListStencilRuleComboBox::OnCloseUp()
{
   CString selectedText;
   CStencilTreeListCtrl* treeListCtrl = (CStencilTreeListCtrl*)GetParent();

   if (treeListCtrl != NULL)
   {
      int selectionIndex = GetCurSel();

      if (selectionIndex != CB_ERR)
      {
         StencilRuleTag ruleTag = intToStencilRuleTag(GetItemData(selectionIndex));

         HTREEITEM selectedItem = treeListCtrl->GetSelectedItem();

         if (selectedItem != NULL)
         {
            DWORD treeItemData = treeListCtrl->GetTreeItemData(selectedItem);
            //DWORD itemData = treeListCtrl->GetItemData(selectedItem);
            CStencilSubPin* subPin = treeListCtrl->getSubPinItemData(selectedItem);

            if (subPin != NULL)
            {
               CString attributeValue = treeListCtrl->GetItemText(selectedItem,ColumnRuleValue);

               CStencilTreeListItem* treeListItem = (CStencilTreeListItem*)treeItemData;
               AttributeSourceTag sourceTag = treeListItem->getAttributeSource();

               //CStencilPin* stencilPin = (CStencilPin*)itemData;
               //CStencilHole& stencilHole = stencilPin->getStencilHole(treeListItem->getSubPinIndex(),treeListItem->getTopFlag());
               CStencilHole& stencilHole = subPin->getStencilHole(treeListItem->getTopFlag());

               CStencilRule oldStencilRule;
               stencilHole.getStencilRuleForSourceLevel(sourceTag,oldStencilRule);

               CStencilRule stencilRule;
               stencilRule.setRule(ruleTag);

               if (oldStencilRule.isValid() &&
                  (oldStencilRule.getRule() == stencilRuleAperture         ||
                   oldStencilRule.getRule() == stencilRuleApertureGeometry ||
                   oldStencilRule.getRule() == stencilRuleArea             ||
                   oldStencilRule.getRule() == stencilRuleInset               )  )
               {
                  stencilRule.addModifiers(oldStencilRule);
               }

               //CStringArray parameters;
               ////CString stencilModifierAttributeValue = stencilPin->getStencilHole(treeListItem->getTopFlag()).getStencilModifierAttributeStringValue(sourceTag);
               ////StencilRuleModifierTag stencilRuleModifier = stencilRuleModifierDescriptionToStencilRuleModifierTag(stencilModifierAttributeValue);
               //StencilRuleModifierTag stencilRuleModifier = stencilHole.getStencilRuleModifierAttribute(sourceTag,parameters);

               //stencilPin->getStencilHole(treeListItem->getTopFlag()).setAttribute(sourceTag,attributeTag,attributeValue);

               //if (stencilRuleModifier != stencilRuleModifierUndefined &&
               //   (attributeTag == stencilAttributeAperture || 
               //    attributeTag == stencilAttributeApertureGeometry || 
               //    attributeTag == stencilAttributeArea || 
               //    attributeTag == stencilAttributeInset))
               //{
               //   stencilPin->getStencilHole(treeListItem->getTopFlag()).setModifierAttribute(sourceTag,stencilRuleModifier);
               //}
            }
         }
      }

      DestroyWindow();
      treeListCtrl->GetParent()->Invalidate(true);
      treeListCtrl->SetFocus();
   }
}

//_____________________________________________________________________________
CTreeListStencilRuleModifierComboBox::CTreeListStencilRuleModifierComboBox(int column)
: CTreeListStencilRuleComboBox(column)
{
   m_enableEditing = false;
}

CTreeListStencilRuleModifierComboBox::~CTreeListStencilRuleModifierComboBox()
{
}

CTreeListComboBox* CTreeListStencilRuleModifierComboBox::create(CRect rect,CWnd* parentWnd)
{
   rect.bottom += 5*rect.Height();

   CTreeListComboBox* comboBox = new CTreeListStencilRuleModifierComboBox(m_column);
   DWORD dwComboBoxStyle = WS_CHILD | WS_VSCROLL | CBS_DROPDOWNLIST;
   comboBox->Create(dwComboBoxStyle,rect,parentWnd,0);


   CStencilTreeListCtrl* treeListCtrl = (CStencilTreeListCtrl*)parentWnd;

   if (treeListCtrl != NULL)
   {
      HTREEITEM selectedItem = treeListCtrl->GetSelectedItem();

      if (selectedItem != NULL)
      {
         CString attributeIdentifier = treeListCtrl->GetItemText(selectedItem,ColumnRule);
         StencilRuleTag ruleTag = ruleStringToStencilRuleTag(attributeIdentifier);
         int index;

         switch (ruleTag)
         {
         case stencilRuleAperture:
            index = comboBox->AddString(stencilRuleModifierToDescription(stencilRuleModifierAlignApertureMajorAxisTowardComponentCenterline));
            comboBox->SetItemData(index,stencilRuleModifierAlignApertureMajorAxisTowardComponentCenterline);

            index = comboBox->AddString(stencilRuleModifierToDescription(stencilRuleModifierAlignApertureMajorAxisAwayFromComponentCenterline));
            comboBox->SetItemData(index,stencilRuleModifierAlignApertureMajorAxisAwayFromComponentCenterline);

            index = comboBox->AddString(stencilRuleModifierToDescription(stencilRuleModifierOffsetRelativeToPadstack));
            comboBox->SetItemData(index,stencilRuleModifierOffsetRelativeToPadstack);

            break;
         case stencilRuleApertureGeometry:
            index = comboBox->AddString(stencilRuleModifierToDescription(stencilRuleModifierOffsetRelativeToPadstack));
            comboBox->SetItemData(index,stencilRuleModifierOffsetRelativeToPadstack);

            break;
         case stencilRuleArea:
         case stencilRuleInset:
            index = comboBox->AddString(stencilRuleModifierToDescription(stencilRuleModifierWebSettings));
            comboBox->SetItemData(index,stencilRuleModifierWebSettings);

            break;
         default:
            {
               CString message;

               if (attributeIdentifier.IsEmpty())
               {
                  message = QSetRuleFirst;
               }
               else
               {
                  message.Format("<%s> has no modifiers",attributeIdentifier);
               }

               comboBox->AddString(message);
            }

            break;
         }
      }
   }

   return comboBox;
}

afx_msg void CTreeListStencilRuleModifierComboBox::OnCloseUp()
{
   CString selectedText;
   CStencilTreeListCtrl* treeListCtrl = (CStencilTreeListCtrl*)GetParent();

   if (treeListCtrl != NULL)
   {
      HTREEITEM selectedItem = treeListCtrl->GetSelectedItem();

      if (selectedItem != NULL)
      {
         CString attributeIdentifier = treeListCtrl->GetItemText(selectedItem,ColumnRule);
         StencilRuleTag ruleTag = ruleStringToStencilRuleTag(attributeIdentifier);
         DWORD treeItemData = treeListCtrl->GetTreeItemData(selectedItem);
         //DWORD itemData = treeListCtrl->GetItemData(selectedItem);
         CStencilSubPin* subPin = treeListCtrl->getSubPinItemData(selectedItem);

         if (ruleTag != stencilAttributeUndefined && subPin != NULL)
         {
            CString ruleModifierDescription;
            int selectionIndex = GetCurSel();

            if (selectionIndex == CB_ERR || getEditFlag())
            {
               ruleModifierDescription = m_currentEditBoxText;
            }
            else
            {
               GetLBText(selectionIndex,ruleModifierDescription);
            }

            if (ruleTag != stencilRuleAperture &&
                ruleTag != stencilRuleApertureGeometry &&
                ruleTag != stencilRuleArea &&
                ruleTag != stencilRuleInset)
            {
               ruleModifierDescription.Empty();
            }

            if (!ruleModifierDescription.IsEmpty())
            {
               StencilRuleModifierTag stencilRuleModifier = stencilRuleModifierDescriptionToStencilRuleModifierTag(ruleModifierDescription);
               //CString stencilRuleModifierAttributeValue  = stencilRuleModifierToString(stencilRuleModifier);

               CStencilTreeListItem* treeListItem = (CStencilTreeListItem*)treeItemData;
               AttributeSourceTag sourceTag = treeListItem->getAttributeSource();

               //CStencilPin* stencilPin = (CStencilPin*)itemData;
               //CStencilHole& stencilHole = stencilPin->getStencilHole(treeListItem->getSubPinIndex(),treeListItem->getTopFlag());
               CStencilHole& stencilHole = subPin->getStencilHole(treeListItem->getTopFlag());
               CStencilRule stencilRule = stencilHole.getStencilRuleForSource(sourceTag);
               CStencilRuleModifierValues modifierValues;

               stencilRule.addModifier(stencilRuleModifier,modifierValues);
               stencilHole.setStencilRuleForLevel(sourceTag,stencilRule);

               //Attrib* attribute = stencilPin->getStencilHole(treeListItem->getTopFlag()).setModifierAttribute(sourceTag,stencilRuleModifier);
            }
         }
      }

      DestroyWindow();
      treeListCtrl->GetParent()->Invalidate(true);
      treeListCtrl->SetFocus();
   }
}

//_____________________________________________________________________________
CTypedObArrayContainer<CMapSortStringToOb*> CTreeListAttributeValueComboBox::m_attributeValuesArray;

CTreeListAttributeValueComboBox::CTreeListAttributeValueComboBox(int column) : CTreeListComboBox(column)
{
}

CTreeListAttributeValueComboBox::~CTreeListAttributeValueComboBox()
{
}

void CTreeListAttributeValueComboBox::addAttributeValue(StencilRuleTag stencilRule,
   const CString& attributeValue)
{
   CMapSortStringToOb* map = NULL;

   CString normalizedValue(attributeValue);
   normalizedValue.Trim();

   if (!normalizedValue.IsEmpty())
   {
      if (stencilRule < m_attributeValuesArray.GetSize())
      {
         map = m_attributeValuesArray.GetAt(stencilRule);
      }

      if (map == NULL)
      {
         map = new CMapSortStringToOb(nextPrime2n(50));
         m_attributeValuesArray.SetAtGrow(stencilRule,map);
      }

      map->SetAt(normalizedValue,NULL);
   }
}

void CTreeListAttributeValueComboBox::setSelectedText(const CString& text)
{
   if (SelectString(-1,text) == CB_ERR)
   {
      SetCurSel(AddString(text));
   }
}

void CTreeListAttributeValueComboBox::clearAttributeValues()
{
   m_attributeValuesArray.empty();
}

CMapSortStringToOb& CTreeListAttributeValueComboBox::getValues(StencilRuleTag ruleTag)
{
   CMapSortStringToOb* map = NULL;

   if (ruleTag < m_attributeValuesArray.GetSize())
   {
      map = m_attributeValuesArray.GetAt(ruleTag);
   }

   if (map == NULL)
   {
      map = new CMapSortStringToOb(nextPrime2n(50));
      m_attributeValuesArray.SetAtGrow(ruleTag,map);
   }

   return *map;
}

//_____________________________________________________________________________
CTreeListStencilRuleValueComboBox::CTreeListStencilRuleValueComboBox(int column)
: CTreeListAttributeValueComboBox(column)
{
   m_enableEditing = true;
}

CTreeListStencilRuleValueComboBox::~CTreeListStencilRuleValueComboBox()
{
}

CTreeListComboBox* CTreeListStencilRuleValueComboBox::create(CRect rect,CWnd* parentWnd)
{
   rect.bottom += 5*rect.Height();

   CTreeListComboBox* comboBox = new CTreeListStencilRuleValueComboBox(m_column);
   DWORD dwComboBoxStyle = WS_CHILD | WS_VSCROLL | CBS_DROPDOWN;
   comboBox->Create(dwComboBoxStyle,rect,parentWnd,0);

   CTreeListCtrl* treeListCtrl = (CTreeListCtrl*)parentWnd;

   if (treeListCtrl != NULL)
   {
      HTREEITEM selectedItem = treeListCtrl->GetSelectedItem();
      CString attributeIdentifier = treeListCtrl->GetItemText(selectedItem,ColumnRule);
      StencilRuleTag ruleTag = ruleStringToStencilRuleTag(attributeIdentifier);

      if (ruleTag != stencilAttributeUndefined)
      {
         CMapSortStringToOb& valuesMap = getValues(ruleTag);
         CObject* object;
         CString key;
         int index;

         for (valuesMap.rewind(index);valuesMap.next(object,key,index);)
         {
            comboBox->AddString(key);
         }

         if (ruleTag == stencilRuleAperture)
         {
            comboBox->AddString(QSelectAperture);
         }
         //else if (ruleTag == stencilAttributeApertureGeometry)
         //{
         //   comboBox->AddString(QSelectApertureGeometry);
         //}
      }
      else
      {
         comboBox->AddString(QSetRuleFirst);
      }
   }

   return comboBox;
}

afx_msg void CTreeListStencilRuleValueComboBox::OnCloseUp()
{
   CString selectedText;
   CStencilTreeListCtrl* treeListCtrl = (CStencilTreeListCtrl*)GetParent();

   if (treeListCtrl != NULL)
   {
      HTREEITEM selectedItem = treeListCtrl->GetSelectedItem();

      if (selectedItem != NULL)
      {
         CString attributeIdentifier = treeListCtrl->GetItemText(selectedItem,ColumnRule);
         StencilRuleTag ruleTag = ruleStringToStencilRuleTag(attributeIdentifier);
         DWORD treeItemData = treeListCtrl->GetTreeItemData(selectedItem);
         //DWORD itemData = treeListCtrl->GetItemData(selectedItem);
         CStencilSubPin* subPin = treeListCtrl->getSubPinItemData(selectedItem);

         if (ruleTag != stencilAttributeUndefined && subPin != NULL)
         {
            CString attributeValue;
            int selectionIndex = GetCurSel();

            if (selectionIndex == CB_ERR || getEditFlag())
            {
               attributeValue = m_currentEditBoxText;
            }
            else
            {
               GetLBText(selectionIndex,attributeValue);
            }

            if (ruleTag == stencilRuleAperture)
            {
               if (attributeValue.CompareNoCase(QSelectAperture) == 0)
               {
                  CStandardApertureDialog standardApertureDialog(getActiveView()->GetDocument()->getPageUnits());
                  standardApertureDialog.setInitialDescriptor(m_currentEditBoxText);

                  if (standardApertureDialog.DoModal() == IDOK)
                  {
                     attributeValue = standardApertureDialog.getApertureDescriptor();
                  }
                  else
                  {
                     attributeValue.Empty();
                  }
               }
            }
            //else if (ruleTag == stencilAttributeApertureGeometry)
            //{
            //   if (attributeValue.CompareNoCase(QSelectApertureGeometry) == 0)
            //   {
            //      CApertureGeometryDialog apertureGeometryDialog;

            //      if (apertureGeometryDialog.DoModal() == IDOK)
            //      {
            //         attributeValue = apertureGeometryDialog.getApertureName();
            //      }
            //      else
            //      {
            //         attributeValue.Empty();
            //      }
            //   }
            //}

            if (!attributeValue.IsEmpty())
            {
               CStencilTreeListItem* treeListItem = (CStencilTreeListItem*)treeItemData;
               AttributeSourceTag sourceTag = treeListItem->getAttributeSource();

               //CStencilPin* stencilPin = (CStencilPin*)itemData;
               //CStencilHole& stencilHole = stencilPin->getStencilHole(treeListItem->getSubPinIndex(),treeListItem->getTopFlag());
               CStencilHole& stencilHole = subPin->getStencilHole(treeListItem->getTopFlag());

               CStencilRule oldStencilRule;
               stencilHole.getStencilRuleForSourceLevel(sourceTag,oldStencilRule);

               CStencilRule stencilRule;
               stencilRule.setRule(ruleTag);

               if (oldStencilRule.isValid() &&
                  (oldStencilRule.getRule() == stencilRuleAperture         ||
                   oldStencilRule.getRule() == stencilRuleApertureGeometry ||
                   oldStencilRule.getRule() == stencilRuleArea             ||
                   oldStencilRule.getRule() == stencilRuleInset               )  )
               {
                  stencilRule.addModifiers(oldStencilRule);
               }

               if (stencilRule.isValid())
               {
                  addAttributeValue(stencilRule.getRule(),stencilRule.getRuleValue());
               }

               //CStringArray modifierParameters;

               ////CString stencilModifierAttributeValue = stencilPin->getStencilHole(treeListItem->getTopFlag()).getStencilModifierAttributeStringValue(sourceTag);
               ////StencilRuleModifierTag stencilRuleModifier = stencilRuleModifierDescriptionToStencilRuleModifierTag(stencilModifierAttributeValue);
               //StencilRuleModifierTag stencilRuleModifier = stencilHole.getStencilRuleModifierAttribute(sourceTag,modifierParameters);

               //Attrib* attribute = stencilPin->getStencilHole(treeListItem->getTopFlag()).setAttribute(sourceTag,ruleTag,attributeValue);

               //if (stencilRuleModifier != stencilRuleModifierUndefined &&
               //   (attributeTag == stencilAttributeAperture || 
               //    attributeTag == stencilAttributeApertureGeometry || 
               //    attributeTag == stencilAttributeArea || 
               //    attributeTag == stencilAttributeInset))
               //{
               //   stencilPin->getStencilHole(treeListItem->getTopFlag()).setModifierAttribute(sourceTag,stencilRuleModifier);
               //}

               //if (attribute != NULL)
               //{
               //   int keywordIndex = CStencilPin::getKeywordIndex(attributeTag);

               //   CString attributeDisplayValue =
               //      CStencilPin::getStencilAttributeStringValue(stencilPin->getCamCadDatabase(),
               //         keywordIndex,attribute);

               //   addAttributeValue(attributeTag,attributeDisplayValue);
               //}
            }
         }
      }

      DestroyWindow();
      treeListCtrl->GetParent()->Invalidate(true);
      treeListCtrl->SetFocus();
   }
}

//_____________________________________________________________________________
CStencilTreeListItem::CStencilTreeListItem()
{
   m_sourceTag = attributeSourceUndefined;
   m_topFlag   = false;
   //m_subPinIndex = 0;
}

CStencilTreeListItem::CStencilTreeListItem(const CTreeListItem& other)
: CTreeListItem(other)
{
   m_sourceTag = attributeSourceUndefined;
   m_topFlag   = false;
   //m_subPinIndex = 0;
}

CStencilTreeListItem::CStencilTreeListItem(AttributeSourceTag sourceTag,bool topFlag)
{
   m_sourceTag = sourceTag;
   m_topFlag   = topFlag;
}

AttributeSourceTag CStencilTreeListItem::getAttributeSource() const
{
   return m_sourceTag;
}

bool CStencilTreeListItem::getTopFlag() const
{
   return m_topFlag;
}

//int CStencilTreeListItem::getSubPinIndex() const
//{
//   return m_subPinIndex;
//}

CString CStencilTreeListItem::GetSubstring(CTreeListCtrl& treeListCtrl,HTREEITEM item,int nSub) const
{
   CString retval;
   CString junk = this->GetItemText();

   if (itemData != NULL)
   {
      CStencilSubPin* stencilSubPin = getSubPinItemData();
      CStencilHole& stencilHole = stencilSubPin->getStencilHole(m_topFlag);
      CStencilRuleAttributes stencilRuleAttributes = stencilHole.getStencilRuleAttributes(m_sourceTag);

      switch (nSub)
      {
      case ColumnRule:
         retval = stencilRuleAttributes.getRule().getRuleString();
         //retval = stencilAttributeToRuleString(
         //            attributeNameToStencilAttributeTag(
         //               stencilHole.getStencilAttributeName(m_sourceTag) ));
         break;
      case ColumnRuleValue:
         retval = stencilRuleAttributes.getRule().getRuleValue();
         //retval = stencilHole.getStencilAttributeStringValue(m_sourceTag);
         break;
      case ColumnRuleModifier:
         {
            //CStringArray modifierParameters;

            //StencilRuleModifierTag stencilRuleModifier = stencilHole.getStencilRuleModifierAttribute(m_sourceTag,modifierParameters);
            StencilRuleModifierTag stencilRuleModifier = stencilRuleAttributes.getRule().getModifier();
            retval = stencilRuleModifierToDescription(stencilRuleModifier);

            //retval = stencilPin->getStencilHole(m_topFlag).getStencilModifierAttributeStringValue(m_sourceTag);
         }
         break;
      default:
         retval = CTreeListItem::GetSubstring(treeListCtrl,item,nSub);
         break;
      }
   }
   else
   {
      retval = CTreeListItem::GetSubstring(treeListCtrl,item,nSub);
   }

   return retval;
}

CStencilSubPin* CStencilTreeListItem::getSubPinItemData() const
{
   CStencilSubPin* stencilSubPin = (CStencilSubPin*)itemData;

   return stencilSubPin;
}

//_____________________________________________________________________________
CStencilTreeListCtrl::CStencilTreeListCtrl(CWnd& parent)
: m_parent(parent)
{
   setImageList();
}

CStencilTreeListCtrl::~CStencilTreeListCtrl()
{
   delete m_imageList;
}

CTreeListItem* CStencilTreeListCtrl::newTreeListItem()
{
   CTreeListItem* treeListItem = new CStencilTreeListItem();

   return treeListItem;
}

CTreeListItem* CStencilTreeListCtrl::newTreeListItem(const CTreeListItem& other)
{
   CTreeListItem* treeListItem = new CStencilTreeListItem(other);

   return treeListItem;
}

CTreeListItem* CStencilTreeListCtrl::newTreeListItem(AttributeSourceTag sourceTag,bool topFlag)
{
   CTreeListItem* treeListItem = new CStencilTreeListItem(sourceTag,topFlag);

   return treeListItem;
}

HTREEITEM CStencilTreeListCtrl::InsertItem(LPCTSTR lpszItem,AttributeSourceTag sourceTag,bool topFlag,
   int nImage,int nSelectedImage,HTREEITEM hParent,HTREEITEM hInsertAfter)
{
   CTreeListItem *pItem = newTreeListItem(sourceTag,topFlag);
   pItem->InsertItem(lpszItem);
   m_nItems++;
   ((CTreeListFrame*)GetParent())->ResetScrollBar();

   return CTreeCtrl::InsertItem(TVIF_PARAM|TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE, "", nImage, nSelectedImage, 0, 0, (LPARAM)pItem, hParent, hInsertAfter);
}

bool CStencilTreeListCtrl::setSubPinItemData(HTREEITEM item,CStencilSubPin* subPin)
{
   bool retval = (SetItemData(item,(DWORD)subPin) != 0);

   return retval;
}

CStencilSubPin* CStencilTreeListCtrl::getSubPinItemData(HTREEITEM item)
{
   CStencilSubPin* subPin = (CStencilSubPin*)GetItemData(item);

   return subPin;
}

void CStencilTreeListCtrl::setImageList()
{
   m_imageList = new CImageList();
   m_imageList->Create(16, 16,ILC_MASK, 50, 8);
   CWinApp* app = AfxGetApp();

   m_pcbIconIndex          = m_imageList->Add(app->LoadIcon(IDI_FILE     )); // pcb
   m_surfaceIconIndex      = m_imageList->Add(app->LoadIcon(IDI_LAYER    )); // surface
   m_geomIconIndex         = m_imageList->Add(app->LoadIcon(IDI_COMPBLK  )); // component
   m_padIconIndex          = m_imageList->Add(app->LoadIcon(IDI_PADSTK   )); // padstack
   m_compIconIndex         = m_imageList->Add(app->LoadIcon(IDI_CP       )); // refdes
   m_pinIconIndex          = m_imageList->Add(app->LoadIcon(IDI_PAD      )); // pin
   m_subPinIconIndex       = m_imageList->Add(app->LoadIcon(IDI_SubPad   )); // subpin
   m_viaIconIndex          = m_imageList->Add(app->LoadIcon(IDI_ViaStk   )); // via
   m_fiducialIconIndex     = m_imageList->Add(app->LoadIcon(IDI_Fiducial )); // fiducial
   m_pinInstanceIconIndex  = m_imageList->Add(app->LoadIcon(IDI_REF      )); // pin
   m_smdThIconIndex        = m_imageList->Add(app->LoadIcon(IDI_BLOCK    )); // smd/th

   m_pcbIconAIndex         = m_imageList->Add(app->LoadIcon(IDI_FILEA    )); // pcb
   m_surfaceIconAIndex     = m_imageList->Add(app->LoadIcon(IDI_LAYERA   )); // surface
   m_geomIconAIndex        = m_imageList->Add(app->LoadIcon(IDI_COMPBLKA )); // component
   m_padIconAIndex         = m_imageList->Add(app->LoadIcon(IDI_PADSTKA  )); // padstack
   m_compIconAIndex        = m_imageList->Add(app->LoadIcon(IDI_CPA      )); // refdes
   m_pinIconAIndex         = m_imageList->Add(app->LoadIcon(IDI_PADA     )); // pin
   m_subPinIconAIndex      = m_imageList->Add(app->LoadIcon(IDI_SubPadA  )); // subpin
   m_viaIconAIndex         = m_imageList->Add(app->LoadIcon(IDI_ViaStkA  )); // via
   m_fiducialIconAIndex    = m_imageList->Add(app->LoadIcon(IDI_FiducialA)); // fiducial
   m_pinInstanceIconAIndex = m_imageList->Add(app->LoadIcon(IDI_REFA     )); // pin
   m_smdThIconAIndex       = m_imageList->Add(app->LoadIcon(IDI_BLOCKA   )); // smd/th

   m_pcbIconBIndex         = m_imageList->Add(app->LoadIcon(IDI_FILEB    )); // pcb
   m_surfaceIconBIndex     = m_imageList->Add(app->LoadIcon(IDI_LAYERB   )); // surface
   m_geomIconBIndex        = m_imageList->Add(app->LoadIcon(IDI_COMPBLKB )); // component
   m_padIconBIndex         = m_imageList->Add(app->LoadIcon(IDI_PADSTKB  )); // padstack
   m_compIconBIndex        = m_imageList->Add(app->LoadIcon(IDI_CPB      )); // refdes
   m_pinIconBIndex         = m_imageList->Add(app->LoadIcon(IDI_PADB     )); // pin
   m_subPinIconBIndex      = m_imageList->Add(app->LoadIcon(IDI_SubPadB  )); // subpin
   m_viaIconBIndex         = m_imageList->Add(app->LoadIcon(IDI_ViaStkB  )); // via
   m_fiducialIconBIndex    = m_imageList->Add(app->LoadIcon(IDI_FiducialB)); // fiducial
   m_pinInstanceIconBIndex = m_imageList->Add(app->LoadIcon(IDI_REFB     )); // pin
   m_smdThIconBIndex       = m_imageList->Add(app->LoadIcon(IDI_BLOCKB   )); // smd/th

   SetImageList(m_imageList, TVSIL_NORMAL);
}

void CStencilTreeListCtrl::expandAll(HTREEITEM item)
{
   if (item != NULL)
   {
      if (ItemHasChildren(item))
      {
         Expand(item,TVE_EXPAND);

         HTREEITEM childItem = GetChildItem(item);

         while (childItem != NULL)
         {
            expandAll(childItem);
            childItem = GetNextSiblingItem(childItem);
         }
      }
   }
}

int CStencilTreeListCtrl::collapseLowerHierarchies(HTREEITEM item)
{
   int count = 0;

   if (item != NULL)
   {
      if (ItemHasChildren(item))
      {
         HTREEITEM childItem = GetChildItem(item);

         while (childItem != NULL)
         {
            count += collapseLowerHierarchies(childItem);
            childItem = GetNextSiblingItem(childItem);
         }

         Expand(item,((count > 0) ? TVE_EXPAND : TVE_COLLAPSE));
      }

      DWORD treeItemData = GetTreeItemData(item);
      //DWORD itemData     = GetItemData(item);
      CStencilSubPin* subPin = getSubPinItemData(item);

      if (subPin != NULL)
      {
         CStencilTreeListItem* treeListItem = (CStencilTreeListItem*)treeItemData;
         AttributeSourceTag sourceTag = treeListItem->getAttributeSource();

         if (sourceTag == attributeSourcePcb     ||
             sourceTag == attributeSourceSurface ||
             sourceTag == attributeSourceMount      )
         {
            count++;
         }
      }
   }

   return count;
}

int CStencilTreeListCtrl::expandToShowRules(HTREEITEM item)
{
   int count = 0;

   if (item != NULL)
   {
      if (ItemHasChildren(item))
      {
         HTREEITEM childItem = GetChildItem(item);

         while (childItem != NULL)
         {
            count += expandToShowRules(childItem);
            childItem = GetNextSiblingItem(childItem);
         }

         Expand(item,((count > 0) ? TVE_EXPAND : TVE_COLLAPSE));
      }

      //DWORD itemData = GetItemData(item);
      CStencilSubPin* subPin = getSubPinItemData(item);

      if (subPin != NULL)
      {
         CString attributeName = GetItemText(item,ColumnRule);

         if (! attributeName.IsEmpty())
         {
            count++;
         }
      }
   }

   return count;
}

void CStencilTreeListCtrl::expandAll()
{
   SetRedraw(false);

   expandAll(GetRootItem());

   SetRedraw(true);
   Invalidate();
}

void CStencilTreeListCtrl::collapseLowerHierarchies()
{
   SetRedraw(false);

   collapseLowerHierarchies(GetRootItem());

   SetRedraw(true);
   Invalidate();
}

void CStencilTreeListCtrl::expandToShowRules()
{
   SetRedraw(false);

   expandToShowRules(GetRootItem());

   SetRedraw(true);
   Invalidate();
}

void CStencilTreeListCtrl::cutRule()
{
   copyRule(true);
}

void CStencilTreeListCtrl::copyRule(bool removeFlag)
{
   HTREEITEM item = GetSelectedItem();

   if (item != NULL)
   {
      DWORD treeItemData = GetTreeItemData(item);
      //DWORD itemData     = GetItemData(item);
      CStencilSubPin* subPin = getSubPinItemData(item);

      if (subPin != NULL)
      {
         CStencilTreeListItem* treeListItem = (CStencilTreeListItem*)treeItemData;
         AttributeSourceTag sourceTag = treeListItem->getAttributeSource();
         bool topFlag  = treeListItem->getTopFlag();
         //int subPinIndex = treeListItem->getSubPinIndex();

         //CStencilPin* selectedStencilPin = (CStencilPin*)itemData;
         //CStencilHole& stencilHole = selectedStencilPin->getStencilHole(subPinIndex,topFlag);
         CStencilHole& stencilHole = subPin->getStencilHole(topFlag);

         CStencilRule stencilRule = stencilHole.getStencilRuleForSource(sourceTag);

         if (stencilRule.isValid())
         {
            CString ruleString = stencilRule.getRuleString();
            CClipboard::putText(ruleString);
         }

         if (removeFlag)
         {
            stencilHole.getStencilRuleAttributes(sourceTag).empty();
            Invalidate();
            getParent().SendMessage(WM_StencilPinChanged,0,0);
         }
      }
   }
}

void CStencilTreeListCtrl::pasteRule()
{
   HTREEITEM item = GetSelectedItem();

   if (item != NULL)
   {
      DWORD treeItemData = GetTreeItemData(item);
      //DWORD itemData     = GetItemData(item);
      CStencilSubPin* subPin = getSubPinItemData(item);

      if (subPin != NULL)
      {
         CStencilTreeListItem* treeListItem = (CStencilTreeListItem*)treeItemData;
         AttributeSourceTag sourceTag = treeListItem->getAttributeSource();
         bool topFlag = treeListItem->getTopFlag();
         //int subPinIndex = treeListItem->getSubPinIndex();

         //CStencilPin* selectedStencilPin = (CStencilPin*)itemData;
         //CStencilHole& stencilHole = selectedStencilPin->getStencilHole(subPinIndex,topFlag);
         CStencilHole& stencilHole = subPin->getStencilHole(topFlag);

         CString ruleString;

         if (CClipboard::getText(ruleString))
         {
            CStencilRule stencilRule;
            stencilRule.setRuleFromString(ruleString);

            if (stencilRule.isValid())
            {
               stencilHole.setStencilRuleForLevel(sourceTag,stencilRule);
               Invalidate();
               getParent().SendMessage(WM_StencilPinChanged,0,0);
            }
         }
      }
   }
}

BEGIN_MESSAGE_MAP(CStencilTreeListCtrl,CTreeListCtrl)
   //{{AFX_MSG_MAP(CStencilTreeListCtrl)
   ON_WM_RBUTTONDOWN()
   ON_COMMAND(ID_StencilCommand_ExpandAll,OnExpandAll)
   ON_COMMAND(ID_StencilCommand_CollapseAll,OnCollapseLowerHierarchies)
   ON_COMMAND(ID_StencilCommand_ExpandToShowRules,OnExpandToShowRules)
   ON_COMMAND(ID_StencilCommand_CutRule,OnCutRule)
   ON_COMMAND(ID_StencilCommand_CopyRule,OnCopyRule)
   ON_COMMAND(ID_StencilCommand_PasteRule,OnPasteRule)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CStencilTreeListCtrl::OnRButtonDown(UINT nFlags, CPoint windowRelativePoint)
{
   // TODO: Add your message handler code here and/or call default
   CPopupMenu popupMenu;
   CString prompt;
   prompt.Format("Action");

   popupMenu.addItem(0,prompt);
   popupMenu.addSeparator();
   popupMenu.addItem(ID_StencilCommand_ExpandAll        ,"   Expand all");
   popupMenu.addItem(ID_StencilCommand_CollapseAll      ,"   Collapse lower hierarchies");
   popupMenu.addItem(ID_StencilCommand_ExpandToShowRules,"   Expand to show rules");
   popupMenu.addSeparator();
   popupMenu.addItem(ID_StencilCommand_CutRule          ,"   Cut rule");
   popupMenu.addItem(ID_StencilCommand_CopyRule         ,"   Copy rule");
   popupMenu.addItem(ID_StencilCommand_PasteRule        ,"   Paste rule");

   popupMenu.trackMenu(TPM_LEFTALIGN,windowRelativePoint,this);
}

void CStencilTreeListCtrl::OnExpandAll()
{
   expandAll();
}

void CStencilTreeListCtrl::OnCollapseLowerHierarchies()
{
   collapseLowerHierarchies();
}

void CStencilTreeListCtrl::OnExpandToShowRules()
{
   expandToShowRules();
}

void CStencilTreeListCtrl::OnCutRule()
{
   cutRule();
}

void CStencilTreeListCtrl::OnCopyRule()
{
   copyRule();
}

void CStencilTreeListCtrl::OnPasteRule()
{
   pasteRule();
}

//_____________________________________________________________________________
CStencilTreeListFrame::CStencilTreeListFrame(CWnd& parent)
: m_parent(parent)
{
}

CTreeListCtrl& CStencilTreeListFrame::getTree()
{
   if (m_tree == NULL)
   {
      m_tree = new CStencilTreeListCtrl(getParent());
   }

   return *m_tree;
}

CWnd& CStencilTreeListFrame::getParent()
{
   return m_parent;
}

BOOL CStencilTreeListFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
   BOOL retval = CTreeListFrame::OnNotify(wParam,lParam,pResult);

   GetParent()->SendMessage(WM_NOTIFY, wParam, lParam);

   return retval;
}

//_____________________________________________________________________________
CLayerComboBox::CLayerComboBox(CCamCadDatabase& camCadDatabase)
: m_camCadDatabase(camCadDatabase)
{
}

void CLayerComboBox::init()
{
   ResetContent();

   int index = AddString("");
   SetItemData(index,-1);

   for (int layerIndex = 0;layerIndex < m_camCadDatabase.getNumLayers();layerIndex++)
   {
      LayerStruct* layer = m_camCadDatabase.getLayerAt(layerIndex);

      if (layer != NULL)
      {
         index = AddString(layer->getName());
         SetItemData(index,layer->getLayerIndex());
      }
   }
}

LayerStruct* CLayerComboBox::getSelectedLayer()
{
   LayerStruct* layer = NULL;

   int selectedIndex = GetCurSel();

   if (selectedIndex != CB_ERR)
   {
      int layerIndex = GetItemData(selectedIndex);

      layer = m_camCadDatabase.getLayerAt(layerIndex);
   }

   return layer;
}

bool CLayerComboBox::setSelectedLayer(const CString& layerName)
{
   bool retval = false;

   int index = FindStringExact(-1,layerName);

   if (index != CB_ERR)
   {
      retval = (SetCurSel(index) != CB_ERR);
   }
   else
   {
      SetCurSel(-1);
   }

   return retval;
}

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CStencilConfigurationPropertyPage, CResizingPropertyPage)

CStencilConfigurationPropertyPage::CStencilConfigurationPropertyPage(CStencilGenerationParameters& stencilGenerationParameters)
: CResizingPropertyPage(CStencilConfigurationPropertyPage::IDD)
//, m_stencilGenerationParameters(stencilGenerationParameters)
, m_topStencilSourceLayerComboBox(stencilGenerationParameters.getCamCadDatabase())
, m_bottomStencilSourceLayerComboBox(stencilGenerationParameters.getCamCadDatabase())
{
   m_psp.pszTitle = QStencilPcbCaption;
   m_psp.dwFlags |= PSP_USETITLE;
   m_chkUseDatabaseConnection = TRUE;
}

CStencilConfigurationPropertyPage::~CStencilConfigurationPropertyPage()
{
}

CStencilGenerationParameters& CStencilConfigurationPropertyPage::getStencilGenerationParameters()
{
   return getParent()->getStencilUi().getStencilGenerationParameters();
}

//void CStencilConfigurationPropertyPage::setEditBoxText(CEdit& editBox,
//   StencilAttributeTag attributeTag,double defaultValue)
//{
//   BlockStruct* pcbBlock = getParent()->getPcbBlock();
//   Attrib* attrib;
//   CString displayString;
//   double value;
//
//   if (pcbBlock->getAttributesRef()->Lookup(CStencilPin::getKeywordIndex(attributeTag),(void*&)attrib))
//   {
//      value = atof(getParent()->getCamCadDatabase().getAttributeStringValue(attrib));
//   }
//   else
//   {
//      value = defaultValue;
//   }
//
//   displayString.Format("%.3f",value);
//
//   editBox.SetWindowText(displayString);
//}

void CStencilConfigurationPropertyPage::DoDataExchange(CDataExchange* pDX)
{
   CCamCadDatabase& camCadDatabase = getParent()->getCamCadDatabase();

   if (! pDX->m_bSaveAndValidate && ::IsWindow(m_topStencilThicknessEditBox))
   {
      CString displayString;

      displayString.Format("%.3f",getStencilGenerationParameters().getTopStencilThickness());
      m_topStencilThicknessEditBox.SetWindowText(displayString);

      displayString.Format("%.3f",getStencilGenerationParameters().getBottomStencilThickness());
      m_bottomStencilThicknessEditBox.SetWindowText(displayString);

      displayString.Format("%.3f",getStencilGenerationParameters().getTopMaxFeatureSize());
      m_topMaxFeatureEditBox.SetWindowText(displayString);

      displayString.Format("%.3f",getStencilGenerationParameters().getBottomMaxFeatureSize());
      m_bottomMaxFeatureEditBox.SetWindowText(displayString);

      displayString.Format("%.3f",getStencilGenerationParameters().getTopWebSize());
      m_topWebWidthEditBox.SetWindowText(displayString);

      displayString.Format("%.3f",getStencilGenerationParameters().getBottomWebSize());
      m_bottomWebWidthEditBox.SetWindowText(displayString);

      displayString.Format("%.3f",getStencilGenerationParameters().getExteriorCornerRadius());
      m_exteriorCornerRadiusEditBox.SetWindowText(displayString);

      m_topPasteInHoleCheckBox.SetCheck(getStencilGenerationParameters().getTopPasteInHole());
      m_bottomPasteInHoleCheckBox.SetCheck(getStencilGenerationParameters().getBottomPasteInHole());

      m_topPasteInViaCheckBox.SetCheck(getStencilGenerationParameters().getTopPasteInVia());
      m_bottomPasteInViaCheckBox.SetCheck(getStencilGenerationParameters().getBottomPasteInVia());

      m_topStencilSourceLayerComboBox.setSelectedLayer(getStencilGenerationParameters().getTopStencilSourceLayerName());
      m_bottomStencilSourceLayerComboBox.setSelectedLayer(getStencilGenerationParameters().getBottomStencilSourceLayerName());

      m_useMountTechAttribCheckBox.SetCheck(getStencilGenerationParameters().getUseMountTechAttrib());
      m_mountTechAttribNameEditBox.SetWindowText(getStencilGenerationParameters().getMountTechAttribName());
      m_mountTechAttribSMDValueEditBox.SetWindowText(getStencilGenerationParameters().getMountTechAttribSMDValue());
      
   }

   CResizingPropertyPage::DoDataExchange(pDX);

   DDX_Control(pDX, IDC_TopStencilThicknessEditBox   , m_topStencilThicknessEditBox);
   DDX_Control(pDX, IDC_BottomStencilThicknessEditBox, m_bottomStencilThicknessEditBox);

   DDX_Control(pDX, IDC_TopMaxFeatureEditBox   , m_topMaxFeatureEditBox);
   DDX_Control(pDX, IDC_BottomMaxFeatureEditBox, m_bottomMaxFeatureEditBox);
   DDX_Control(pDX, IDC_TopWebWidthEditBox     , m_topWebWidthEditBox);
   DDX_Control(pDX, IDC_BottomWebWidthEditBox  , m_bottomWebWidthEditBox);

   DDX_Control(pDX, IDC_TopMinPinPitchEditBox   , m_topMinPinPitchEditBox);
   DDX_Control(pDX, IDC_BottomMinPinPitchEditBox, m_bottomMinPinPitchEditBox);

   DDX_Control(pDX, IDC_ExteriorCornerRadiusEditBox, m_exteriorCornerRadiusEditBox);

   DDX_Control(pDX, IDC_TopPasteInHoleCheckBox   , m_topPasteInHoleCheckBox);
   DDX_Control(pDX, IDC_BottomPasteInHoleCheckBox, m_bottomPasteInHoleCheckBox);
   DDX_Control(pDX, IDC_TopPasteInViaCheckBox    , m_topPasteInViaCheckBox);
   DDX_Control(pDX, IDC_BottomPasteInViaCheckBox , m_bottomPasteInViaCheckBox);

   DDX_Control(pDX, IDC_TopStencilSourceLayer    , m_topStencilSourceLayerComboBox);
   DDX_Control(pDX, IDC_BottomStencilSourceLayer2, m_bottomStencilSourceLayerComboBox);
   
   DDX_Control(pDX, IDC_UseMountTechAttribCheckBox    , m_useMountTechAttribCheckBox);
   DDX_Control(pDX, IDC_MountTechAttribNameEditBox    , m_mountTechAttribNameEditBox);
   DDX_Control(pDX, IDC_MountTechAttribSMDValueEditBox, m_mountTechAttribSMDValueEditBox);

   DDX_Check(pDX, IDC_ChkBxUseDatabaseConnection, m_chkUseDatabaseConnection);

   if (pDX->m_bSaveAndValidate)
   {
      CStencilGenerationParameters stencilGenerationParameters(getStencilGenerationParameters());
      CString stringValue;

      m_topStencilThicknessEditBox.GetWindowText(stringValue);
      stencilGenerationParameters.setTopStencilThickness(atof(stringValue));

      m_bottomStencilThicknessEditBox.GetWindowText(stringValue);
      stencilGenerationParameters.setBottomStencilThickness(atof(stringValue));

      m_topMaxFeatureEditBox.GetWindowText(stringValue);
      stencilGenerationParameters.setTopMaxFeatureSize(atof(stringValue));

      m_bottomMaxFeatureEditBox.GetWindowText(stringValue);
      stencilGenerationParameters.setBottomMaxFeatureSize(atof(stringValue));

      m_topWebWidthEditBox.GetWindowText(stringValue);
      stencilGenerationParameters.setTopWebSize(atof(stringValue));

      m_bottomWebWidthEditBox.GetWindowText(stringValue);
      stencilGenerationParameters.setBottomWebSize(atof(stringValue));

      m_exteriorCornerRadiusEditBox.GetWindowText(stringValue);
      stencilGenerationParameters.setExteriorCornerRadius(atof(stringValue));

      LayerStruct* layer = m_topStencilSourceLayerComboBox.getSelectedLayer();
      CString layerName;

      if (layer != NULL)
      {
         layerName = layer->getName();
      }

      stencilGenerationParameters.setTopStencilSourceLayerName(layerName);

      layer = m_bottomStencilSourceLayerComboBox.getSelectedLayer();
      layerName.Empty();

      if (layer != NULL)
      {
         layerName = layer->getName();
      }

      stencilGenerationParameters.setBottomStencilSourceLayerName(layerName);

      bool checkedFlag = (m_topPasteInHoleCheckBox.GetCheck() != 0);
      stencilGenerationParameters.setTopPasteInHole(checkedFlag);

      checkedFlag = (m_bottomPasteInHoleCheckBox.GetCheck() != 0);
      stencilGenerationParameters.setBottomPasteInHole(checkedFlag);

      checkedFlag = (m_topPasteInViaCheckBox.GetCheck() != 0);
      stencilGenerationParameters.setTopPasteInVia(checkedFlag);

      checkedFlag = (m_bottomPasteInViaCheckBox.GetCheck() != 0);
      stencilGenerationParameters.setBottomPasteInVia(checkedFlag);

      checkedFlag = (m_useMountTechAttribCheckBox.GetCheck() != 0);
      stencilGenerationParameters.setUseMountTechAttrib(checkedFlag);

      m_mountTechAttribNameEditBox.GetWindowText(stringValue);
      stencilGenerationParameters.setMountTechAttribName(stringValue);

      m_mountTechAttribSMDValueEditBox.GetWindowText(stringValue);
      stencilGenerationParameters.setMountTechAttribSMDValue(stringValue);


      if (! getStencilGenerationParameters().allParametersEqual(stencilGenerationParameters))
      {
         getStencilGenerationParameters() = stencilGenerationParameters;
         getStencilGenerationParameters().writeAttributes(getParent()->getStencilUi().getPcbFileNum());

         getParent()->getStencilUi().OnEventModifiedGenerationParameters();
      }
   }

   //if (pDX->m_bSaveAndValidate)
   //{
   //   BlockStruct* pcbBlock = getParent()->getPcbBlock();

   //   if (pcbBlock != NULL)
   //   {
   //      if (pcbBlock->getAttributesRef() == NULL)
   //      {
   //         pcbBlock->getAttributesRef() = new CAttributes();
   //      }

   //      CString stringValue;

   //      m_topStencilThicknessEditBox.GetWindowText(stringValue);
   //      camCadDatabase.addAttribute(getAttributeMap(pcbBlock),
   //         CStencilPin::getKeywordIndex(stencilAttributeTopThickness),stringValue);

   //      m_bottomStencilThicknessEditBox.GetWindowText(stringValue);
   //      camCadDatabase.addAttribute(getAttributeMap(pcbBlock),
   //         CStencilPin::getKeywordIndex(stencilAttributeBottomThickness),stringValue);

   //      m_topMaxFeatureEditBox.GetWindowText(stringValue);
   //      camCadDatabase.addAttribute(getAttributeMap(pcbBlock),
   //         CStencilPin::getKeywordIndex(stencilAttributeTopMaxFeature),stringValue);

   //      m_bottomMaxFeatureEditBox.GetWindowText(stringValue);
   //      camCadDatabase.addAttribute(getAttributeMap(pcbBlock),
   //         CStencilPin::getKeywordIndex(stencilAttributeBottomMaxFeature),stringValue);

   //      m_topWebWidthEditBox.GetWindowText(stringValue);
   //      camCadDatabase.addAttribute(getAttributeMap(pcbBlock),
   //         CStencilPin::getKeywordIndex(stencilAttributeTopWebWidth),stringValue);

   //      m_bottomWebWidthEditBox.GetWindowText(stringValue);
   //      camCadDatabase.addAttribute(getAttributeMap(pcbBlock),
   //         CStencilPin::getKeywordIndex(stencilAttributeBottomWebWidth),stringValue);

   //      stringValue.Format("%d",m_topPasteInHoleCheckBox.GetCheck());
   //      camCadDatabase.addAttribute(getAttributeMap(pcbBlock),
   //         CStencilPin::getKeywordIndex(stencilAttributeTopPasteInHole),stringValue);

   //      stringValue.Format("%d",m_bottomPasteInHoleCheckBox.GetCheck());
   //      camCadDatabase.addAttribute(getAttributeMap(pcbBlock),
   //         CStencilPin::getKeywordIndex(stencilAttributeBottomPasteInHole),stringValue);
   //   }
   //}

}

BEGIN_MESSAGE_MAP(CStencilConfigurationPropertyPage, CResizingPropertyPage)
   //ON_BN_CLICKED(IDC_GenerateStencils, OnBnClickedGenerateStencils)
   ON_EN_KILLFOCUS(IDC_TopStencilThicknessEditBox   , OnKillFocusTopStencilThicknessEditBox   )
   ON_EN_KILLFOCUS(IDC_BottomStencilThicknessEditBox, OnKillFocusBottomStencilThicknessEditBox)
   ON_BN_CLICKED(IDC_TopPasteInHoleCheckBox   , OnBnClickedTopPasteInHoleCheckBox   )
   ON_BN_CLICKED(IDC_BottomPasteInHoleCheckBox, OnBnClickedBottomPasteInHoleCheckBox)

   ON_BN_CLICKED(IDC_UseMountTechAttribCheckBox      , OnBnClickedUseMountTechAttribCheckBox )
   ON_EN_KILLFOCUS(IDC_MountTechAttribNameEditBox    , OnKillFocusMountTechAttribNameEditBox)
   ON_EN_KILLFOCUS(IDC_MountTechAttribSMDValueEditBox, OnKillFocusMountTechAttribSMDValueEditBox)

   ON_EN_KILLFOCUS(IDC_TopMaxFeatureEditBox, OnKillFocusTopMaxFeatureEditBox)
   ON_EN_KILLFOCUS(IDC_BottomMaxFeatureEditBox, OnKillFocusBottomMaxFeatureEditBox)
   ON_EN_KILLFOCUS(IDC_TopWebWidthEditBox, OnKillFocusTopWebWidthEditBox)
   ON_EN_KILLFOCUS(IDC_BottomWebWidthEditBox, OnKillFocusBottomWebWidthEditBox)
   ON_BN_CLICKED(IDC_SaveSettings, OnBnClickedSaveSettings)
   ON_BN_CLICKED(IDC_LoadSettings, OnBnClickedLoadSettings)
   ON_BN_CLICKED(IDC_MergeSettings, OnBnClickedMergeSettings)
   ON_BN_CLICKED(IDC_RemoveAllRules, OnBnClickedRemoveAllRules)
   ON_EN_KILLFOCUS(IDC_ExteriorCornerRadiusEditBox, OnKillFocusExteriorCornerRadiusEditBox)
   ON_CBN_SELCHANGE(IDC_TopStencilSourceLayer, OnSelChangeTopStencilSourceLayer)
   ON_CBN_SELCHANGE(IDC_BottomStencilSourceLayer2, OnSelChangeBottomStencilSourceLayer)
   ON_WM_ACTIVATE()
   ON_BN_CLICKED(IDC_TopPasteInViaCheckBox, OnBnClickedTopPasteInViaCheckBox)
   ON_BN_CLICKED(IDC_BottomPasteInViaCheckBox, OnBnClickedBottomPasteInViaCheckBox)
   ON_BN_CLICKED(IDC_SaveSettingsToDatabase, OnBnClickedSaveSettingsToDatabase)
   ON_BN_CLICKED(IDC_LoadSettingsFromDatabase, OnBnClickedLoadSettingsFromDatabase)

	ON_BN_CLICKED(IDC_BrowseButton, OnBnClickedBrowseButton)
   ON_BN_CLICKED(IDC_ChkBxUseDatabaseConnection, OnBnClickedChkUseDatabaseConnection)
END_MESSAGE_MAP()


/******************************************************************************
*/
void CStencilConfigurationPropertyPage::OnBnClickedBrowseButton()
{
   CDBInterface db;
   db.PresentDBFileChooser();
   GetDlgItem(IDC_LibraryDatabaseName)->SetWindowText( db.GetLibraryDatabaseName() );
}

/******************************************************************************
*/
void CStencilConfigurationPropertyPage::OnBnClickedChkUseDatabaseConnection()
{
   UpdateData(TRUE); 

   // Update sticky setting, the DB interface owns this setting
   CDBInterface db;
   db.SetConnectionAttemptEnabled(m_chkUseDatabaseConnection?true:false);

   // Enable/Disable DB related controls in Data Doc
   this->EnableDatabaseControls(db.GetConnectionAttemptEnabled(), db.GetConnectionAttemptAllowed());
}

/******************************************************************************
*/
void CStencilConfigurationPropertyPage::EnableDatabaseControls(bool attempt, bool allowed)
{
   if (GetDlgItem(IDC_LibraryDatabaseName))
   {
      // These are set according to whether or not the user wants to attempt connection and
      // such connection is allowed.
      GetDlgItem(IDC_LibraryDatabaseName)->EnableWindow( attempt && allowed );
      GetDlgItem(IDC_BrowseButton)->EnableWindow( attempt && allowed );

      GetDlgItem(IDC_SaveSettingsToDatabase)->EnableWindow( attempt && allowed );
      GetDlgItem(IDC_LoadSettingsFromDatabase)->EnableWindow( attempt && allowed );

      // These are set according to whether or not the system is going to allow the user to attempt connection.
      GetDlgItem(IDC_ChkBxUseDatabaseConnection)->EnableWindow( allowed );
      GetDlgItem(IDC_ActiveLibraryGroup)->EnableWindow( allowed );

      // This is set to show if system will allow connection and user wishes to attempt connection.
      m_chkUseDatabaseConnection = attempt && allowed;

      UpdateData(FALSE);
   }
}

/******************************************************************************
*/

BOOL CStencilConfigurationPropertyPage::OnInitDialog()
{
#ifdef STENCIL_TIMER
   GetStencilTimer().AddMessage("enter CStencilConfigurationPropertyPage::OnInitDialog()");
#endif

   CResizingPropertyPage::OnInitDialog();

   m_topStencilSourceLayerComboBox.init();
   m_bottomStencilSourceLayerComboBox.init();

   getStencilGenerationParameters().initUndefinedStencilSourceLayerNames();

   m_topStencilSourceLayerComboBox.setSelectedLayer(getStencilGenerationParameters().getTopStencilSourceLayerName());
   m_bottomStencilSourceLayerComboBox.setSelectedLayer(getStencilGenerationParameters().getBottomStencilSourceLayerName());

   // The scope here is to ensure full shutdown of CDBInterface.
   {
      // Within scope parens so DB will be closed/deleted immediately after use.
      CDBInterface db;
	   GetDlgItem(IDC_LibraryDatabaseName)->SetWindowText( db.GetLibraryDatabaseName() );
      this->EnableDatabaseControls(db.GetConnectionAttemptEnabled(), db.GetConnectionAttemptAllowed());
   }

   // The pin pitch edit boxes are read only for user, init them here, the can't change
   // during duration of SG so this is all there is to it.
   // The SG internal data is not set up until a geometry tab is opened, so we can't
   // access that stuff yet. What we want here are just attribs on pcb component inserts,
   // so just scan the data in the usual way.
   // We're looking for the smallest value that is greater than zero.
   // Assuming pitch is positive value, negative pin pitch is silly.
   // We're also taking pcb surface at face value from "placed top", not considering wild
   // combinations where placment is top but item ends up on bottom (there are many
   // ways to do that, all of them a nuisance.)
#ifdef STENCIL_TIMER
   GetStencilTimer().AddMessage("      CStencilConfigurationPropertyPage::OnInitDialog() PinPitch Start");
#endif
   double minTopPinPitch = -1;
   double minBotPinPitch = -1;
   BlockStruct *pcbBlock = this->getParent()->getStencilUi().getPcbBlock();
   if (pcbBlock != NULL)
   {
      int pinPitchKw = getParent()->getCamCadDatabase().getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributePinPitch);

      for (CDataListIterator pcbComponentIterator(*pcbBlock,insertTypePcbComponent);pcbComponentIterator.hasNext();)
      {
         DataStruct* pcbComponent = pcbComponentIterator.getNext();

         Attrib *attrib = NULL;
         if (pcbComponent->lookUpAttrib(pinPitchKw, attrib))
         {
            double pitch = attrib->getDoubleValue();
            if (pitch > 0.0)
            {
               if (pcbComponent->getInsert()->getPlacedTop())
               {
                  if (minTopPinPitch < 0.0 || pitch < minTopPinPitch)
                     minTopPinPitch = pitch;
               }
               else // Bottom
               {
                  if (minBotPinPitch < 0.0 || pitch < minBotPinPitch)
                     minBotPinPitch = pitch;
               }
            }
         }
      }
#ifdef STENCIL_TIMER
      GetStencilTimer().AddMessage("      CStencilConfigurationPropertyPage::OnInitDialog() PinPitch Loop End");
#endif

      CString minTopPitchStr;
      if (minTopPinPitch > 0.0)
         minTopPitchStr.Format("%f", minTopPinPitch);

      CString minBotPitchStr;
      if (minBotPinPitch > 0.0)
         minBotPitchStr.Format("%f", minBotPinPitch);

      m_topMinPinPitchEditBox.SetWindowText(minTopPitchStr);
      m_bottomMinPinPitchEditBox.SetWindowText(minBotPitchStr);
#ifdef STENCIL_TIMER
      GetStencilTimer().AddMessage("      CStencilConfigurationPropertyPage::OnInitDialog() PinPitch End");
#endif
      
   }

#ifdef STENCIL_TIMER
   GetStencilTimer().AddMessage("      CStencilConfigurationPropertyPage::OnInitDialog() 9");
#endif


   UpdateData(false);

   enableMountTechAttribControls( getStencilGenerationParameters().getUseMountTechAttrib() );
#ifdef STENCIL_TIMER
   GetStencilTimer().AddMessage("      CStencilConfigurationPropertyPage::OnInitDialog() 10");
#endif

   //if (m_pcbFileNum < 0)
   //{
   //   m_topStencilThicknessEditBox.EnableWindow(false);
   //   m_bottomStencilThicknessEditBox.EnableWindow(false);
   //   m_topPasteInHoleCheckBox.EnableWindow(false);
   //   m_bottomPasteInHoleCheckBox.EnableWindow(false);
   //}

   checkStencilSourceLayers();
#ifdef STENCIL_TIMER
   GetStencilTimer().AddMessage("exit  CStencilConfigurationPropertyPage::OnInitDialog()");
#endif

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CStencilConfigurationPropertyPage::enableMountTechAttribControls(bool flag)
{
//#define PROHIBIT_ACCESS
#ifdef  PROHIBIT_ACCESS
   m_mountTechAttribNameEditBox.EnableWindow(false);
   m_mountTechAttribSMDValueEditBox.EnableWindow(false);

   GetDlgItem(IDC_MountTechAttribNameLabel)->EnableWindow(false);
   GetDlgItem(IDC_MountTechAttribSMDValueLabel)->EnableWindow(false);
   GetDlgItem(IDC_MountTechAttribGroupBox)->EnableWindow(false);
   GetDlgItem(IDC_UseMountTechAttribCheckBox)->EnableWindow(false);

   return;
#endif

   m_mountTechAttribNameEditBox.EnableWindow(flag);
   m_mountTechAttribSMDValueEditBox.EnableWindow(flag);

   GetDlgItem(IDC_MountTechAttribNameLabel)->EnableWindow(flag);
   GetDlgItem(IDC_MountTechAttribSMDValueLabel)->EnableWindow(flag);
   ///GetDlgItem(IDC_MountTechAttribGroupBox)->EnableWindow(flag);  Don't do this, draws line through checkbox which is on groupbox boundary
}

bool CStencilConfigurationPropertyPage::checkStencilSourceLayers()
{
   bool retval = true;

   LayerStruct*    topStencilSourceLayer =    m_topStencilSourceLayerComboBox.getSelectedLayer();
   LayerStruct* bottomStencilSourceLayer = m_bottomStencilSourceLayerComboBox.getSelectedLayer();

   CString message;

   for (int pass = 0;pass < 2;pass++)
   {
      LayerStruct* layer = ((pass == 0) ? topStencilSourceLayer : bottomStencilSourceLayer);
      CString surface    = ((pass == 0) ? "Top" : "Bottom"); 

      if (layer != NULL)
      {
         LayerTypeTag   layerType = layer->getLayerType();
         LayerGroupTag layerGroup = getLayerGroupForLayerType(layerType);

         if ((layerGroup != layerGroupTop) && (layerGroup != layerGroupBottom) && (layerGroup != layerGroupAll) && (layerGroup != layerGroupOuter))
         {
            message.AppendFormat("Layer '%s' has an invalid layer type of '%s' - use a %s surface layer type e.g. Paste %s, Soldemask %s, etc.\n",
               layer->getName(),
               layerTypeToDisplayString(layerType),
               surface,surface,surface);
         }
      }
   }

   if (!message.IsEmpty())
   {
      formatMessageBox(message);
      retval = false;
   }

   return retval;
}

BOOL CStencilConfigurationPropertyPage::OnSetActive()
{
   return CResizingPropertyPage::OnSetActive();
}

BOOL CStencilConfigurationPropertyPage::OnKillActive()
{
   CStencilGenerationParameters oldParams = getStencilGenerationParameters();

   UpdateData(true);

   checkStencilSourceLayers();

   getStencilGenerationParameters().writeAttributes(getParent()->getStencilUi().getPcbFileNum());

   int pcbFileNum = getParent()->getStencilUi().getPcbFileNum();
   if (pcbFileNum >= 0 && !getStencilGenerationParameters().smdParametersEqual(oldParams))
   {
      COperationProgress progress;
      getParent()->getStencilGenerator().identifySMDEntities(pcbFileNum,&progress);
   }

   return CResizingPropertyPage::OnKillActive();
}

void CStencilConfigurationPropertyPage::OnKillFocusTopStencilThicknessEditBox()
{
   UpdateData(true);
   UpdateData(false);

   double newThickness = getStencilGenerationParameters().getTopStencilThickness();
   getParent()->getStencilGenerator().propagateStencilThickness(newThickness, true /*top*/);
}

void CStencilConfigurationPropertyPage::OnKillFocusBottomStencilThicknessEditBox()
{
   UpdateData(true);
   UpdateData(false);

   double newThickness = getStencilGenerationParameters().getBottomStencilThickness();
   getParent()->getStencilGenerator().propagateStencilThickness(newThickness, false /*bottom*/);
}

void CStencilConfigurationPropertyPage::OnKillFocusTopMaxFeatureEditBox()
{
   UpdateData(true);
   UpdateData(false);
}

void CStencilConfigurationPropertyPage::OnKillFocusBottomMaxFeatureEditBox()
{
   UpdateData(true);
   UpdateData(false);
}

void CStencilConfigurationPropertyPage::OnKillFocusTopWebWidthEditBox()
{
   UpdateData(true);
   UpdateData(false);
}

void CStencilConfigurationPropertyPage::OnKillFocusBottomWebWidthEditBox()
{
   UpdateData(true);
   UpdateData(false);
}

void CStencilConfigurationPropertyPage::OnKillFocusExteriorCornerRadiusEditBox()
{
   UpdateData(true);
   UpdateData(false);
}

void CStencilConfigurationPropertyPage::OnSelChangeTopStencilSourceLayer()
{
   UpdateData(true);
   UpdateData(false);
}

void CStencilConfigurationPropertyPage::OnSelChangeBottomStencilSourceLayer()
{
   UpdateData(true);
   UpdateData(false);
}

void CStencilConfigurationPropertyPage::OnBnClickedTopPasteInHoleCheckBox()
{
   UpdateData(true);
   UpdateData(false);
}

void CStencilConfigurationPropertyPage::OnBnClickedBottomPasteInHoleCheckBox()
{
   UpdateData(true);
   UpdateData(false);
}

void CStencilConfigurationPropertyPage::OnBnClickedUseMountTechAttribCheckBox()
{
   bool oldVal = this->getStencilGenerationParameters().getUseMountTechAttrib();

   UpdateData(true);
   UpdateData(false);

   enableMountTechAttribControls( getStencilGenerationParameters().getUseMountTechAttrib() );

   bool newVal = this->getStencilGenerationParameters().getUseMountTechAttrib();

   if (newVal != oldVal)
   {
      int pcbFileNum = getParent()->getStencilUi().getPcbFileNum();
      if (pcbFileNum >= 0 )
      {
         COperationProgress progress;
         getParent()->getStencilGenerator().identifySMDEntities(pcbFileNum, &progress);
      }
   }
}

void CStencilConfigurationPropertyPage::OnKillFocusMountTechAttribNameEditBox()
{
   CString oldVal = this->getStencilGenerationParameters().getMountTechAttribName();

   UpdateData(true);
   UpdateData(false);

   CString newVal = this->getStencilGenerationParameters().getMountTechAttribName();

   if (newVal.CompareNoCase(oldVal) != 0)
   {
      int pcbFileNum = getParent()->getStencilUi().getPcbFileNum();
      if (pcbFileNum >= 0 )
      {
         COperationProgress progress;
         getParent()->getStencilGenerator().identifySMDEntities(pcbFileNum, &progress);
      }
   }
}

void CStencilConfigurationPropertyPage::OnKillFocusMountTechAttribSMDValueEditBox()
{
   CString oldVal = this->getStencilGenerationParameters().getMountTechAttribSMDValue();

   UpdateData(true);
   UpdateData(false);

   CString newVal = this->getStencilGenerationParameters().getMountTechAttribSMDValue();

   if (newVal.CompareNoCase(oldVal) != 0)
   {
      int pcbFileNum = getParent()->getStencilUi().getPcbFileNum();
      if (pcbFileNum >= 0 )
      {
         COperationProgress progress;
         getParent()->getStencilGenerator().identifySMDEntities(pcbFileNum, &progress);
      }
   }
}

void CStencilConfigurationPropertyPage::OnBnClickedTopPasteInViaCheckBox()
{
   UpdateData(true);
   UpdateData(false);
}

void CStencilConfigurationPropertyPage::OnBnClickedBottomPasteInViaCheckBox()
{
   UpdateData(true);
   UpdateData(false);
}

#define QStencilConfiguration "StencilConfiguration"
#define QSaveSettingsFile     "SaveSettingsFile"
#define QLoadSettingsFile     "LoadSettingsFile"

void CStencilConfigurationPropertyPage::OnBnClickedSaveSettings()
{
   CString settingsFilePath =
      AfxGetApp()->GetProfileString(QStencilConfiguration,QSaveSettingsFile,"");

   CFileDialog fileDialog(FALSE,"txt",settingsFilePath,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
         "Stencil Settings File (*.txt)|*.txt|All Files (*.*)|*.*||", NULL);

   if (fileDialog.DoModal() == IDOK)
   {
      settingsFilePath = fileDialog.GetPathName();
      getParent()->getStencilUi().saveStencilSettings(settingsFilePath);
      AfxGetApp()->WriteProfileString(QStencilConfiguration,QSaveSettingsFile,settingsFilePath);
   }
}

void CStencilConfigurationPropertyPage::OnBnClickedLoadSettings()
{
   //if (formatMessageBox(MB_YESNO | MB_ICONQUESTION,
   //   "Loading stencil settings will discard all existing stencil generation rules, validation rules, and settings.  OK to proceed?") == IDYES)
   //{
      CString settingsFilePath =
         AfxGetApp()->GetProfileString(QStencilConfiguration,QLoadSettingsFile,"");

      if (settingsFilePath.IsEmpty())
      {
         settingsFilePath =
            AfxGetApp()->GetProfileString(QStencilConfiguration,QSaveSettingsFile,"");
      }

      if (settingsFilePath.IsEmpty())
      {
         settingsFilePath = "*.txt";
      }

      CFileDialog fileDialog(TRUE,"txt",settingsFilePath,OFN_FILEMUSTEXIST,
                     "Stencil Settings File (*.txt)|*.txt|All Files (*.*)|*.*||", NULL);

      if (fileDialog.DoModal() == IDOK)
      {
         settingsFilePath = fileDialog.GetPathName();
         getParent()->getStencilUi().loadStencilSettings(settingsFilePath,false);
         AfxGetApp()->WriteProfileString(QStencilConfiguration,QLoadSettingsFile,settingsFilePath);
      }
   //}
}

void CStencilConfigurationPropertyPage::OnBnClickedLoadSettingsFromDatabase()
{

   CStencilGenerationParameters oldParams = getStencilGenerationParameters();

   getParent()->getStencilUi().loadStencilSettingsFromDatabase();
   UpdateData(false);

   enableMountTechAttribControls( getStencilGenerationParameters().getUseMountTechAttrib() );

   int pcbFileNum = getParent()->getStencilUi().getPcbFileNum();
   if (pcbFileNum >= 0 && !getStencilGenerationParameters().smdParametersEqual(oldParams))
   {
      COperationProgress progress;
      getParent()->getStencilGenerator().identifySMDEntities(pcbFileNum,&progress);
   }
}

void CStencilConfigurationPropertyPage::OnBnClickedMergeSettings()
{
   CString settingsFilePath =
      AfxGetApp()->GetProfileString(QStencilConfiguration,QSaveSettingsFile,"");

   CFileDialog fileDialog(FALSE,"txt",settingsFilePath,OFN_HIDEREADONLY,
         "Stencil Settings File (*.txt)|*.txt|All Files (*.*)|*.*||", NULL);

   if (fileDialog.DoModal() == IDOK)
   {
      settingsFilePath = fileDialog.GetPathName();
      getParent()->getStencilUi().updateStencilSettings(settingsFilePath);
      AfxGetApp()->WriteProfileString(QStencilConfiguration,QSaveSettingsFile,settingsFilePath);
   }

   //if (formatMessageBox(MB_YESNO | MB_ICONQUESTION,
   //   "Merging stencil settings may overwrite existing stencil generation rules, validation rules, and settings.  OK to proceed?") == IDYES)
   //{
   //   CString settingsFilePath =
   //      AfxGetApp()->GetProfileString(QStencilConfiguration,QLoadSettingsFile,"");

   //   if (settingsFilePath.IsEmpty())
   //   {
   //      settingsFilePath =
   //         AfxGetApp()->GetProfileString(QStencilConfiguration,QSaveSettingsFile,"");
   //   }

   //   if (settingsFilePath.IsEmpty())
   //   {
   //      settingsFilePath = "*.txt";
   //   }

   //   CFileDialog fileDialog(TRUE,"txt",settingsFilePath,OFN_FILEMUSTEXIST,
   //                  "Stencil Settings File (*.txt)|*.txt|All Files (*.*)|*.*||", NULL);

   //   if (fileDialog.DoModal() == IDOK)
   //   {
   //      settingsFilePath = fileDialog.GetPathName();
   //      getParent()->getStencilUi().loadStencilSettings(settingsFilePath,false);
   //      AfxGetApp()->WriteProfileString(QStencilConfiguration,QLoadSettingsFile,settingsFilePath);
   //   }
   //}
}

void CStencilConfigurationPropertyPage::OnBnClickedSaveSettingsToDatabase()
{
   if (ErrorMessage("Are you sure you want to overwrite the database with these settings?", "", MB_ICONQUESTION | MB_YESNO) == IDYES)
   {
      getParent()->getStencilUi().updateStencilSettingsInDatabase();
   }
}

void CStencilConfigurationPropertyPage::OnBnClickedRemoveAllRules()
{
   getParent()->clearStencilSettings();
}

void CStencilConfigurationPropertyPage::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
   CResizingPropertyPage::OnActivate(nState, pWndOther, bMinimized);
   
   // TODO: Add your message handler code here
}

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CStencilGeomPadPropertyPage, CResizingPropertyPage)

CStencilGeomPadPropertyPage::CStencilGeomPadPropertyPage()
: CResizingPropertyPage(CStencilGeomPadPropertyPage::IDD)
, m_treeListFrame(*this)
{
   m_psp.pszTitle = QStencilGeomPadCaption;
   m_psp.dwFlags |= PSP_USETITLE;

   addFieldControl(IDC_TreeListCtrl,anchorLeft,growBoth);

   //m_treeInitialized = false;
   m_fillingTreeFlag = false;
}

CStencilGeomPadPropertyPage::~CStencilGeomPadPropertyPage()
{
}

CStencilGenerator& CStencilGeomPadPropertyPage::getStencilGenerator()
{
   return getParent()->getStencilGenerator();
}

void CStencilGeomPadPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CResizingPropertyPage::DoDataExchange(pDX);
}

//int sortByGeomPad(const void* element0,const void* element1)
//{
//   const CStencilPin* stencilHole0 = (CStencilPin*)(((SElement*)element0)->pObject->m_object);
//   const CStencilPin* stencilHole1 = (CStencilPin*)(((SElement*)element1)->pObject->m_object);
//
//   int retval = stencilHole0->getGeometryName().Compare(stencilHole1->getGeometryName());
//
//   if (retval == 0)
//   {
//      retval = stencilHole0->getPadStackName().Compare(stencilHole1->getPadStackName());
//   }
//
//   return retval;
//}

int sortByGeomPadPin(const void* element0,const void* element1)
{
   const CStencilSubPin* stencilHole0 = (CStencilSubPin*)(((SElement*)element0)->pObject->m_object);
   const CStencilSubPin* stencilHole1 = (CStencilSubPin*)(((SElement*)element1)->pObject->m_object);

   int retval = stencilHole0->getGeometryName().Compare(stencilHole1->getGeometryName());

   if (retval == 0)
   {
      retval = stencilHole0->getPadStackName().Compare(stencilHole1->getPadStackName());

      if (retval == 0)
      {
         retval = stencilHole0->getSubPinIndex() - stencilHole1->getSubPinIndex();

         if (retval == 0)
         {
            retval = stencilHole0->getSortablePinName().Compare(stencilHole1->getSortablePinName());
         }
      }
   }

   return retval;
}

//int sortByGeomTopPad(const void* element0,const void* element1)
//{
//   const CStencilPin* stencilHole0 = (CStencilPin*)(((SElement*)element0)->pObject->m_object);
//   const CStencilPin* stencilHole1 = (CStencilPin*)(((SElement*)element1)->pObject->m_object);
//
//   int retval = stencilHole0->getGeometryName().Compare(stencilHole1->getGeometryName());
//
//   if (retval == 0)
//   {
//      retval = stencilHole0->getStencilHole(0,true).getCopperApertureDescriptor().Compare(
//               stencilHole1->getStencilHole(0,true).getCopperApertureDescriptor());
//   }
//
//   return retval;
//}

//int sortByGeomBottomPad(const void* element0,const void* element1)
//{
//   const CStencilPin* stencilHole0 = (CStencilPin*)(((SElement*)element0)->pObject->m_object);
//   const CStencilPin* stencilHole1 = (CStencilPin*)(((SElement*)element1)->pObject->m_object);
//
//   int retval = stencilHole0->getGeometryName().Compare(stencilHole1->getGeometryName());
//
//   if (retval == 0)
//   {
//      retval = stencilHole0->getStencilHole(0,false).getCopperApertureDescriptor().Compare(
//               stencilHole1->getStencilHole(0,false).getCopperApertureDescriptor());
//   }
//
//   return retval;
//}

void CStencilGeomPadPropertyPage::fillTree()
{
   m_fillingTreeFlag = true;

   //if (!m_treeInitialized)
   //{
   //   return;
   //}

   COperationProgress progress;
   CStencilTreeListCtrl& treeListControl = m_treeListFrame.getStencilTree();
   //treeListControl.DeleteAllItems();

   //if (treeListControl.GetColumnsNum() < 3)
   {
      treeListControl.InsertColumn(ColumnStencilObject,QPcbSurfaceTypeGeomPad,
         LVCFMT_LEFT,getParent()->getColumnStencilObjectWidth());

      treeListControl.InsertColumn(ColumnRule         ,QRule                      ,
         LVCFMT_LEFT,getParent()->getColumnRuleWidth());

      treeListControl.InsertColumn(ColumnRuleValue    ,QRuleValue                 ,
         LVCFMT_LEFT,getParent()->getColumnRuleValueWidth());

      treeListControl.InsertColumn(ColumnRuleModifier ,QRuleModifier              ,
         LVCFMT_LEFT,getParent()->getColumnRuleModifierWidth());
   }

   CStencilPins& stencilPins = getParent()->getStencilGenerator().getStencilPins();
   CStencilSubPin* firstSubPin = (stencilPins.IsEmpty() ? NULL : stencilPins.GetHead()->getSubPin(0));

   HTREEITEM pcbItem     = treeListControl.InsertItem(QPCB,attributeSourcePcb,true,
      treeListControl.getPcbIconIndex(),treeListControl.getPcbIconIndex());
   treeListControl.setSubPinItemData(pcbItem,firstSubPin);

   HTREEITEM topSurfaceItem     = treeListControl.InsertItem(QTopSurface,attributeSourceSurface,true,
      treeListControl.getSurfaceIconIndex(),treeListControl.getSurfaceIconIndex(),pcbItem);
   treeListControl.setSubPinItemData(topSurfaceItem,firstSubPin);

   HTREEITEM topSmdSurfaceItem  = treeListControl.InsertItem(QSmdGeometries,attributeSourceMount,true,
      treeListControl.getSmdThIconIndex(),treeListControl.getSmdThIconIndex(),topSurfaceItem);
   treeListControl.setSubPinItemData(topSmdSurfaceItem,NULL);

   HTREEITEM topThSurfaceItem   = treeListControl.InsertItem(QThGeometries,attributeSourceMount,true,
      treeListControl.getSmdThIconIndex(),treeListControl.getSmdThIconIndex(),topSurfaceItem);
   treeListControl.setSubPinItemData(topThSurfaceItem,NULL);

   HTREEITEM bottomSurfaceItem     = treeListControl.InsertItem(QBottomSurface,attributeSourceSurface,false,
      treeListControl.getSurfaceIconIndex(),treeListControl.getSurfaceIconIndex(),pcbItem);
   treeListControl.setSubPinItemData(bottomSurfaceItem,firstSubPin);

   HTREEITEM bottomSmdSurfaceItem  = treeListControl.InsertItem(QSmdGeometries,attributeSourceMount,false,
      treeListControl.getSmdThIconIndex(),treeListControl.getSmdThIconIndex(),bottomSurfaceItem);
   treeListControl.setSubPinItemData(bottomSmdSurfaceItem,NULL);

   HTREEITEM bottomThSurfaceItem   = treeListControl.InsertItem(QThGeometries,attributeSourceMount,false,
      treeListControl.getSmdThIconIndex(),treeListControl.getSmdThIconIndex(),bottomSurfaceItem);
   treeListControl.setSubPinItemData(bottomThSurfaceItem,NULL);

   CCamCadDatabase& camCadDatabase = getParent()->getCamCadDatabase();
   int mapSize = nextPrime2n(max(200 + stencilPins.GetSize(),round(1.5*stencilPins.GetSize())));
   CMapSortedStringToOb<CStencilSubPin> topSortedStencilHoles(mapSize,false);
   CMapSortedStringToOb<CStencilSubPin> bottomSortedStencilHoles(mapSize,false);
   topSortedStencilHoles.setSortFunction(sortByGeomPadPin);
   bottomSortedStencilHoles.setSortFunction(sortByGeomPadPin);

   progress.updateStatus("Scanning Pins",stencilPins.GetCount());

   for (POSITION pos = stencilPins.GetHeadPosition();pos != NULL;)
   {
      CStencilPin* stencilPin = stencilPins.GetNext(pos);
      stencilPin->updateStencilHoleAttribute();

      for (int subPinIndex = 0;subPinIndex < stencilPin->getSubPinCount();subPinIndex++)
      {
         CStencilSubPin* subPin = stencilPin->getSubPin(subPinIndex);

         DataStruct* topCopperPad    = subPin->getStencilHole(true ).getCopperPad();
         DataStruct* bottomCopperPad = subPin->getStencilHole(false).getCopperPad();

         if (topCopperPad != NULL)
         {
            topSortedStencilHoles.SetAt(subPin->getSortableGeomPadstackSubpinPin(true),subPin);
         }

         if (bottomCopperPad != NULL)
         {
            bottomSortedStencilHoles.SetAt(subPin->getSortableGeomPadstackSubpinPin(false),subPin);
         }
      }
   }

   int totalHolesToProcess = topSortedStencilHoles.GetCount() + bottomSortedStencilHoles.GetCount();
   progress.updateStatus("Processing Tree Hierarchy",totalHolesToProcess);

   for (int surfaceIndex = 0;surfaceIndex < 2;surfaceIndex++)
   {
      bool topFlag = (surfaceIndex == 0);

      HTREEITEM surfaceItem;
      HTREEITEM smdSurfaceItem;
      HTREEITEM thSurfaceItem;
      CMapSortedStringToOb<CStencilSubPin>& sortedStencilHoles = (topFlag ? topSortedStencilHoles : bottomSortedStencilHoles);

      if (topFlag)
      {
         surfaceItem    = topSurfaceItem;
         smdSurfaceItem = topSmdSurfaceItem;
         thSurfaceItem  = topThSurfaceItem;
      }
      else
      {
         surfaceItem    = bottomSurfaceItem;
         smdSurfaceItem = bottomSmdSurfaceItem;
         thSurfaceItem  = bottomThSurfaceItem;
      }

      int index;
      CString key;
      CString* pKey = &key;
      CStencilSubPin* subPin;
      CString previousGeometryName(" ")   ,geometryName;
      CString previousPadStackName(" ")   ,padStackName;
      CString previousSubPadStackName(" "),subPadStackName;
      CString pinName,padDescriptor;
      HTREEITEM geometryItem    = NULL;
      HTREEITEM padStackItem    = NULL;
      HTREEITEM subPadStackItem = NULL;
      HTREEITEM stencilHoleItem = NULL;
      HTREEITEM mountItem       = NULL;

      for (sortedStencilHoles.rewind(index);sortedStencilHoles.next(subPin,index);)
      {
         progress.incrementProgress();

         CStencilHole& stencilHole = subPin->getStencilHole(topFlag);

         CStencilPin* stencilPin = &(subPin->getStencilPin());
         geometryName    = stencilPin->getGeometryName();
         padStackName    = stencilPin->getPadStackName();
         subPadStackName = stencilHole.getSubPadstackName();
         pinName         = stencilPin->getPinName();

         padDescriptor = stencilHole.getCopperApertureDescriptor();

         CStencilRuleAttributes stencilRuleAttributes = stencilHole.getStencilRuleAttributes();
         CString attributeValue = stencilRuleAttributes.getRule().getRuleValue();

         //CString attributeName     = stencilHole.getStencilAttributeName();
         //CString attributeValue    = stencilHole.getStencilAttributeStringValue();
         //CString attributeModifier = stencilHole.getStencilModifierAttributeStringValue();

         DataStruct* topCopperPad    = subPin->getStencilHole(true ).getCopperPad();
         DataStruct* bottomCopperPad = subPin->getStencilHole(false).getCopperPad();

         mountItem = (getStencilGenerator().isSmdGeometry(geometryName) ? smdSurfaceItem : thSurfaceItem);

         if (geometryName != previousGeometryName)
         {
            geometryItem    = NULL;
            padStackItem    = NULL;
            subPadStackItem = NULL;
         }

         if (padStackName != previousPadStackName)
         {
            padStackItem = NULL;
            subPadStackItem = NULL;
         }

         if (subPadStackName != previousSubPadStackName)
         {
            subPadStackItem = NULL;
         }

         if (geometryItem == NULL)
         {
            geometryItem = treeListControl.InsertItem(geometryName,attributeSourceGeometry,topFlag,
               treeListControl.getGeomIconIndex(),treeListControl.getGeomIconIndex(),mountItem);
            treeListControl.setSubPinItemData(geometryItem ,subPin);
            treeListControl.setSubPinItemData(mountItem    ,subPin);

            if (topCopperPad    != NULL &&  topFlag ||
                bottomCopperPad != NULL && !topFlag     )
            {
               CStencilRuleAttributes stencilRuleAttributes = stencilHole.getStencilRuleAttributes(attributeSourceGeometry);

               if (stencilRuleAttributes.hasRule())
               {
                  StencilRuleTag ruleTag = stencilRuleAttributes.getRule().getRule();
                  CTreeListAttributeValueComboBox::addAttributeValue(ruleTag,stencilRuleAttributes.getRuleValue());
               }

               //Attrib* attribute;
               //Attrib* modifierAttribute;
               //int keywordIndex;
               //int modifierKeywordIndex;

               //if (stencilHole.getStencilAttributesForSource(attributeSourceGeometry,attribute,keywordIndex,modifierAttribute,modifierKeywordIndex))
               //{
               //   const KeyWordStruct* keyword = camCadDatabase.getKeywordAt(keywordIndex);
               //   StencilAttributeTag attributeTag =
               //      attributeNameToStencilAttributeTag(keyword->getCCKeyword());
               //   CString attributeDisplayValue =
               //      CStencilPin::getStencilAttributeStringValue(camCadDatabase,keywordIndex,attribute);

               //   CTreeListAttributeValueComboBox::addAttributeValue(attributeTag,attributeDisplayValue);
               //}
            }

            previousGeometryName = geometryName;
         }

         if (geometryItem != NULL)
         {
            if (padStackItem == NULL)
            {
               CString description = padStackName + ":" + padDescriptor;
               padStackItem = treeListControl.InsertItem(description,attributeSourcePadstack,topFlag,
                  treeListControl.getPadIconIndex(),treeListControl.getPadIconIndex(),geometryItem);
               treeListControl.setSubPinItemData(padStackItem ,subPin);
            }

            if (padStackItem != NULL)
            {
               if (subPadStackItem == NULL)
               {
                  CString description = subPadStackName + ":" + padDescriptor;
                  subPadStackItem = treeListControl.InsertItem(description,attributeSourceSubPadstack,topFlag,
                     treeListControl.getSubPadIconIndex(),treeListControl.getSubPadIconIndex(),padStackItem);
                  treeListControl.setSubPinItemData(subPadStackItem ,subPin);
               }

               if (subPadStackItem != NULL)
               {
                  stencilHoleItem = treeListControl.InsertItem(pinName,attributeSourceGeometryPin,topFlag,
                        treeListControl.getPinIconIndex(),treeListControl.getPinIconIndex(),subPadStackItem);
                  treeListControl.setSubPinItemData(stencilHoleItem,subPin);

                  previousSubPadStackName = subPadStackName;
               }
            }
         }
      }
   }

   treeListControl.collapseLowerHierarchies();

   m_fillingTreeFlag = false;
}

void CStencilGeomPadPropertyPage::restoreColumnWidths()
{
   CStencilTreeListCtrl& treeListControl = m_treeListFrame.getStencilTree();

   treeListControl.SetColumnWidth(ColumnStencilObject,getParent()->getColumnStencilObjectWidth());
   treeListControl.SetColumnWidth(ColumnRule         ,getParent()->getColumnRuleWidth());
   treeListControl.SetColumnWidth(ColumnRuleValue    ,getParent()->getColumnRuleValueWidth());
   treeListControl.SetColumnWidth(ColumnRuleModifier ,getParent()->getColumnRuleModifierWidth());
}

void CStencilGeomPadPropertyPage::saveColumnWidths()
{
   CStencilTreeListCtrl& treeListControl = m_treeListFrame.getStencilTree();

   getParent()->setColumnStencilObjectWidth(treeListControl.GetColumnWidth(ColumnStencilObject));
   getParent()->setColumnRuleWidth(         treeListControl.GetColumnWidth(ColumnRule));
   getParent()->setColumnRuleValueWidth(    treeListControl.GetColumnWidth(ColumnRuleValue));
   getParent()->setColumnRuleModifierWidth( treeListControl.GetColumnWidth(ColumnRuleModifier));
}

BEGIN_MESSAGE_MAP(CStencilGeomPadPropertyPage, CResizingPropertyPage)
   ON_BN_CLICKED(IDC_EXPANDALL, OnBnClickedExpandAll)
   ON_BN_CLICKED(IDC_COLLAPSEALL, OnBnClickedCollapseAll)
   ON_BN_CLICKED(IDC_ShowRules, OnBnClickedShowRules)
   ON_BN_CLICKED(IDC_RemoveAllRules, OnBnClickedRemoveAllRules)
END_MESSAGE_MAP()

// CStencilGeomPadPropertyPage message handlers

BOOL CStencilGeomPadPropertyPage::OnInitDialog()
{
#ifdef STENCIL_TIMER
   GetStencilTimer().AddMessage("enter CStencilGeomPadPropertyPage::OnInitDialog()");
#endif

   CResizingPropertyPage::OnInitDialog();

   // TODO:  Add extra initialization here
   m_treeListFrame.SubclassDlgItem(IDC_TreeListCtrl,this);
   //m_treeListFrame.ModifyStyleEx(WS_EX_TOPMOST,WS_EX_CLIENTEDGE);
   m_treeListFrame.getTree().setComboBoxAt(ColumnRule        ,new CTreeListStencilRuleComboBox(ColumnRule));
   m_treeListFrame.getTree().setComboBoxAt(ColumnRuleValue   ,new CTreeListStencilRuleValueComboBox(ColumnRuleValue));
   m_treeListFrame.getTree().setComboBoxAt(ColumnRuleModifier,new CTreeListStencilRuleModifierComboBox(ColumnRuleModifier));

   getParent()->initializeAttributeValues();

   //m_treeInitialized = true;

   fillTree();

   UpdateData(false);
#ifdef STENCIL_TIMER
   GetStencilTimer().AddMessage("exit  CStencilGeomPadPropertyPage::OnInitDialog()");
#endif

   return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CStencilGeomPadPropertyPage::OnSetActive()
{
   restoreColumnWidths();

   return true;
}

BOOL CStencilGeomPadPropertyPage::OnKillActive()
{
   saveColumnWidths();

   return CResizingPropertyPage::OnKillActive();
}

void CStencilGeomPadPropertyPage::OnBnClickedExpandAll()
{
   getTreeListControl().expandAll();
}

void CStencilGeomPadPropertyPage::OnBnClickedCollapseAll()
{
   getTreeListControl().collapseLowerHierarchies();
}

void CStencilGeomPadPropertyPage::OnBnClickedShowRules()
{
   getTreeListControl().expandToShowRules();
}

void CStencilGeomPadPropertyPage::OnBnClickedRemoveAllRules()
{
   getParent()->clearStencilSettings();
}

BOOL CStencilGeomPadPropertyPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	HD_NOTIFY *pHDN = (HD_NOTIFY*)lParam;

	if (wParam == ID_TREE_LIST_HEADER)
	{
      if (pHDN->hdr.code == HDN_ITEMCHANGED)
      {
         if (!m_fillingTreeFlag)
         {
            saveColumnWidths();
         }
      }
   }

	return CWnd::OnNotify(wParam, lParam, pResult);
}

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CStencilCompPinPropertyPage, CResizingPropertyPage)

CStencilCompPinPropertyPage::CStencilCompPinPropertyPage()
: CResizingPropertyPage(CStencilCompPinPropertyPage::IDD)
, m_treeListFrame(*this)
{
   m_psp.pszTitle = QStencilCompPinCaption;
   m_psp.dwFlags |= PSP_USETITLE;

   addFieldControl(IDC_TreeListCtrl,anchorLeft,growBoth);

   //m_treeInitialized = false;
   m_fillingTreeFlag = false;
}

CStencilCompPinPropertyPage::~CStencilCompPinPropertyPage()
{
}

void CStencilCompPinPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CResizingPropertyPage::DoDataExchange(pDX);
}

int sortByPinRef(const void* element0,const void* element1)
{
   const CStencilSubPin* stencilHole0 = (CStencilSubPin*)(((SElement*)element0)->pObject->m_object);
   const CStencilSubPin* stencilHole1 = (CStencilSubPin*)(((SElement*)element1)->pObject->m_object);

   int retval = stencilHole0->getSortablePinRef().Compare(stencilHole1->getSortablePinRef());

   if (retval == 0)
   {
      retval = stencilHole0->getSubPinIndex() - stencilHole1->getSubPinIndex();
   }

   return retval;
}

void CStencilCompPinPropertyPage::fillTree()
{
   m_fillingTreeFlag = true;

   //if (! m_treeInitialized)
   //{
   //   return;
   //}

   COperationProgress progress;
   CStencilTreeListCtrl& treeListControl = m_treeListFrame.getStencilTree();
   //treeListControl.DeleteAllItems();

   //if (treeListControl.GetColumnsNum() < 4)
   {
      treeListControl.InsertColumn(ColumnStencilObject,QPcbSurfaceTypeCompPin,
         LVCFMT_LEFT,getParent()->getColumnStencilObjectWidth());

      treeListControl.InsertColumn(ColumnRule         ,QRule                      ,
         LVCFMT_LEFT,getParent()->getColumnRuleWidth());

      treeListControl.InsertColumn(ColumnRuleValue    ,QRuleValue                 ,
         LVCFMT_LEFT,getParent()->getColumnRuleValueWidth());

      treeListControl.InsertColumn(ColumnRuleModifier ,QRuleModifier              ,
         LVCFMT_LEFT,getParent()->getColumnRuleModifierWidth());

    //treeListControl.InsertColumn(ColumnStencilDerivationMethod,QDerivationMethod          ,LVCFMT_LEFT,150);
   }

   CStencilPins& stencilPins = getParent()->getStencilGenerator().getStencilPins();
   CStencilSubPin* firstSubPin = (stencilPins.IsEmpty() ? NULL : stencilPins.GetHead()->getSubPin(0));

   HTREEITEM pcbItem     = treeListControl.InsertItem(QPCB,attributeSourcePcb,true,
      treeListControl.getPcbIconIndex(),treeListControl.getPcbIconIndex());
   treeListControl.setSubPinItemData(pcbItem,firstSubPin);

   HTREEITEM topSurfaceItem     = treeListControl.InsertItem(QTopSurface,attributeSourceSurface,true,
      treeListControl.getSurfaceIconIndex(),treeListControl.getSurfaceIconIndex(),pcbItem);
   treeListControl.setSubPinItemData(topSurfaceItem,firstSubPin);

   HTREEITEM bottomSurfaceItem     = treeListControl.InsertItem(QBottomSurface,attributeSourceSurface,false,
      treeListControl.getSurfaceIconIndex(),treeListControl.getSurfaceIconIndex(),pcbItem);
   treeListControl.setSubPinItemData(bottomSurfaceItem,firstSubPin);

   CCamCadDatabase& camCadDatabase = getParent()->getCamCadDatabase();
   int mapSize = nextPrime2n(max(200 + stencilPins.GetSize(),round(1.5*stencilPins.GetSize())));
   CMapSortedStringToOb<CStencilSubPin> topSortedStencilHoles(mapSize,false);
   CMapSortedStringToOb<CStencilSubPin> bottomSortedStencilHoles(mapSize,false);
   topSortedStencilHoles.setSortFunction(sortByPinRef);
   bottomSortedStencilHoles.setSortFunction(sortByPinRef);

   progress.updateStatus("Scanning Pins",stencilPins.GetCount());

   for (POSITION pos = stencilPins.GetHeadPosition();pos != NULL;)
   {
      CStencilPin* stencilPin = stencilPins.GetNext(pos);
      stencilPin->updateStencilHoleAttribute();

      for (int subPinIndex = 0;subPinIndex < stencilPin->getSubPinCount();subPinIndex++)
      {
         CStencilSubPin* subPin = stencilPin->getSubPin(subPinIndex);

         DataStruct* topCopperPad    = subPin->getStencilHole(true ).getCopperPad();
         DataStruct* bottomCopperPad = subPin->getStencilHole(false).getCopperPad();

         if (topCopperPad != NULL)
         {
            topSortedStencilHoles.SetAt(stencilPin->getSortablePinRef(),subPin);
         }

         if (bottomCopperPad != NULL)
         {
            bottomSortedStencilHoles.SetAt(stencilPin->getSortablePinRef(),subPin);
         }
      }
   }

   int totalHolesToProcess = topSortedStencilHoles.GetCount() + bottomSortedStencilHoles.GetCount();
   progress.updateStatus("Processing Tree Hierarchy",totalHolesToProcess);

   for (int surfaceIndex = 0;surfaceIndex < 2;surfaceIndex++)
   {
      bool topFlag = (surfaceIndex == 0);

      HTREEITEM surfaceItem;
      CMapSortedStringToOb<CStencilSubPin>& sortedStencilHoles =
         (topFlag ? topSortedStencilHoles : bottomSortedStencilHoles);

      if (topFlag)
      {
         surfaceItem    = topSurfaceItem;
      }
      else
      {
         surfaceItem    = bottomSurfaceItem;
      }

      int index;
      CString key;
      CString* pKey = &key;
      CStencilSubPin* subPin;
      CString previousRefDes(" "),refDes,pinName,padDescriptor;
      HTREEITEM componentItem = NULL;
      HTREEITEM stencilHoleItem = NULL;

      for (sortedStencilHoles.rewind(index);sortedStencilHoles.next(subPin,index);)
      {
         progress.incrementProgress();

         CStencilPin* stencilPin = &(subPin->getStencilPin());

         refDes        = stencilPin->getRefDes();
         pinName       = stencilPin->getPinName();

         CStencilHole& stencilHole = subPin->getStencilHole(topFlag);

         padDescriptor = stencilHole.getCopperApertureDescriptor();

         CStencilRuleAttributes stencilRuleAttributes = stencilHole.getStencilRuleAttributes();
         CString attributeValue = stencilRuleAttributes.getRule().getRuleValue();

         //CString attributeName     = stencilHole.getStencilAttributeName();
         //CString attributeValue    = stencilHole.getStencilAttributeStringValue();
         //CString attributeModifier = stencilHole.getStencilModifierAttributeStringValue();

         DataStruct* topCopperPad    = subPin->getStencilHole(true ).getCopperPad();
         DataStruct* bottomCopperPad = subPin->getStencilHole(false).getCopperPad();

         if (refDes != previousRefDes)
         {
            componentItem = NULL;
         }

         if (componentItem == NULL)
         {
            if (topCopperPad    != NULL &&  topFlag ||
                bottomCopperPad != NULL && !topFlag     )
            {
               componentItem = treeListControl.InsertItem(refDes,attributeSourceComponent,topFlag,
                  treeListControl.getCompIconIndex(),treeListControl.getCompIconIndex(),surfaceItem);
               treeListControl.setSubPinItemData(componentItem,subPin);

               CStencilRuleAttributes stencilRuleAttributes = stencilHole.getStencilRuleAttributes(attributeSourceComponent);

               if (stencilRuleAttributes.hasNonInheritedRule())
               {
                  StencilRuleTag ruleTag = stencilRuleAttributes.getRule().getRule();
                  CTreeListAttributeValueComboBox::addAttributeValue(ruleTag,stencilRuleAttributes.getRuleValue());
               }

               //Attrib* attribute;
               //Attrib* modifierAttribute;
               //int keywordIndex;
               //int modifierKeywordIndex;

               //if (stencilHole.getStencilAttributesForSource(attributeSourceComponent,attribute,keywordIndex,modifierAttribute,modifierKeywordIndex))
               //{
               //   const KeyWordStruct* keyword = camCadDatabase.getKeywordAt(keywordIndex);
               //   StencilAttributeTag attributeTag =
               //      attributeNameToStencilAttributeTag(keyword->getCCKeyword());
               //   CString attributeDisplayValue =
               //      CStencilPin::getStencilAttributeStringValue(camCadDatabase,keywordIndex,attribute);

               //   //treeListControl.SetItemText(componentItem,ColumnRule     ,
               //   //   camCadDatabase.getKeywordAt(keywordIndex)->cc);

               //   //treeListControl.SetItemText(componentItem,ColumnRuleValue,
               //   //   CStencilPin::getStencilAttributeStringValue(camCadDatabase,keywordIndex,attribute));

               //   CTreeListAttributeValueComboBox::addAttributeValue(attributeTag,attributeDisplayValue);
               //}

               //treeListControl.SetItemText(componentItem,ColumnStencilDerivationMethod,stencilPin->getDerivationMethod());
            }
         }

         if (componentItem != NULL)
         {
            stencilHoleItem = treeListControl.InsertItem(pinName,attributeSourceComponentPin,topFlag,
                  treeListControl.getPinInstanceIconIndex(),treeListControl.getPinInstanceIconIndex(),componentItem);
            treeListControl.setSubPinItemData(stencilHoleItem,subPin);

            //Attrib* attribute;
            //int keywordIndex;

            //if (stencilPin->getStencilAttributeForSource(attributeSourcePin,attribute,keywordIndex))
            //{
            //   treeListControl.SetItemText(stencilHoleItem,ColumnRule     ,
            //      camCadDatabase.getKeywordAt(keywordIndex)->cc);

            //   treeListControl.SetItemText(stencilHoleItem,ColumnRuleValue,
            //      CStencilPin::getStencilAttributeStringValue(camCadDatabase,keywordIndex,attribute));
            //}

            //treeListControl.SetItemText(stencilHoleItem,ColumnStencilDerivationMethod,stencilPin->getDerivationMethod());

            previousRefDes = refDes;
         }
      }
   }

   treeListControl.collapseLowerHierarchies();

   m_fillingTreeFlag = false;
}

void CStencilCompPinPropertyPage::restoreColumnWidths()
{
   CStencilTreeListCtrl& treeListControl = m_treeListFrame.getStencilTree();

   treeListControl.SetColumnWidth(ColumnStencilObject,getParent()->getColumnStencilObjectWidth());
   treeListControl.SetColumnWidth(ColumnRule         ,getParent()->getColumnRuleWidth());
   treeListControl.SetColumnWidth(ColumnRuleValue    ,getParent()->getColumnRuleValueWidth());
   treeListControl.SetColumnWidth(ColumnRuleModifier ,getParent()->getColumnRuleModifierWidth());
}

void CStencilCompPinPropertyPage::saveColumnWidths()
{
   CStencilTreeListCtrl& treeListControl = m_treeListFrame.getStencilTree();

   getParent()->setColumnStencilObjectWidth(treeListControl.GetColumnWidth(ColumnStencilObject));
   getParent()->setColumnRuleWidth(         treeListControl.GetColumnWidth(ColumnRule));
   getParent()->setColumnRuleValueWidth(    treeListControl.GetColumnWidth(ColumnRuleValue));
   getParent()->setColumnRuleModifierWidth( treeListControl.GetColumnWidth(ColumnRuleModifier));
}

BEGIN_MESSAGE_MAP(CStencilCompPinPropertyPage, CResizingPropertyPage)
//   ON_WM_RBUTTONDOWN()
//   ON_WM_CONTEXTMENU()
ON_BN_CLICKED(IDC_EXPANDALL, OnBnClickedExpandAll)
ON_BN_CLICKED(IDC_COLLAPSEALL, OnBnClickedCollapseAll)
ON_BN_CLICKED(IDC_ShowRules, OnBnClickedShowRules)
ON_BN_CLICKED(IDC_RemoveAllRules, OnBnClickedRemoveAllRules)
END_MESSAGE_MAP()

// CStencilCompPinPropertyPage message handlers

BOOL CStencilCompPinPropertyPage::OnInitDialog()
{
#ifdef STENCIL_TIMER
   GetStencilTimer().AddMessage("enter CStencilCompPinPropertyPage::OnInitDialog()");
#endif

   CResizingPropertyPage::OnInitDialog();

   // TODO:  Add extra initialization here
   m_treeListFrame.SubclassDlgItem(IDC_TreeListCtrl,this);
   //m_treeListFrame.ModifyStyleEx(WS_EX_TOPMOST,WS_EX_CLIENTEDGE);
   m_treeListFrame.getTree().setComboBoxAt(ColumnRule        ,new CTreeListStencilRuleComboBox(ColumnRule));
   m_treeListFrame.getTree().setComboBoxAt(ColumnRuleValue   ,new CTreeListStencilRuleValueComboBox(ColumnRuleValue));
   m_treeListFrame.getTree().setComboBoxAt(ColumnRuleModifier,new CTreeListStencilRuleModifierComboBox(ColumnRuleModifier));

   getParent()->initializeAttributeValues();

   //m_treeInitialized = true;

   fillTree();

   UpdateData(false);

#ifdef STENCIL_TIMER
   GetStencilTimer().AddMessage("exit  CStencilCompPinPropertyPage::OnInitDialog()");
#endif

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CStencilCompPinPropertyPage::OnSetActive()
{
   restoreColumnWidths();

   return true;
}

BOOL CStencilCompPinPropertyPage::OnKillActive()
{
   saveColumnWidths();

   return CResizingPropertyPage::OnKillActive();
}

void CStencilCompPinPropertyPage::OnBnClickedExpandAll()
{
   getTreeListControl().expandAll();
}

void CStencilCompPinPropertyPage::OnBnClickedCollapseAll()
{
   getTreeListControl().collapseLowerHierarchies();
}

void CStencilCompPinPropertyPage::OnBnClickedShowRules()
{
   getTreeListControl().expandToShowRules();
}

void CStencilCompPinPropertyPage::OnBnClickedRemoveAllRules()
{
   getParent()->clearStencilSettings();
}

BOOL CStencilCompPinPropertyPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	HD_NOTIFY *pHDN = (HD_NOTIFY*)lParam;

	if (wParam == ID_TREE_LIST_HEADER)
	{
      if (pHDN->hdr.code == HDN_ITEMCHANGED)
      {
         if (!m_fillingTreeFlag)
         {
            saveColumnWidths();
         }
      }
   }

	return CWnd::OnNotify(wParam, lParam, pResult);
}

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CStencilValidationRulesPropertyPage, CResizingPropertyPage)

CStencilValidationRulesPropertyPage::CStencilValidationRulesPropertyPage(CStencilValidationParameters& stencilValidationParameters)
: CResizingPropertyPage(CStencilValidationRulesPropertyPage::IDD)
//, m_stencilValidationParameters(stencilValidationParameters)
{
   m_psp.pszTitle = QStencilRulesCaption;
   m_psp.dwFlags |= PSP_USETITLE;
}

CStencilValidationRulesPropertyPage::~CStencilValidationRulesPropertyPage()
{
}

CStencilValidationParameters& CStencilValidationRulesPropertyPage::getStencilValidationParameters() 
{ 
   return getParent()->getStencilUi().getStencilValidationParameters(); 
}

void CStencilValidationRulesPropertyPage::DoDataExchange(CDataExchange* pDX)
{
   if (! pDX->m_bSaveAndValidate && ::IsWindow(m_holeToWallAreaEditBox))
   {
      CString displayString;

      displayString.Format("%.3f",getStencilValidationParameters().getMinHoleAreaToWallAreaRatio());
      m_holeToWallAreaEditBox.SetWindowText(displayString);

      displayString.Format("%.3f",getStencilValidationParameters().getMinFeature());
      m_minFeatureEditBox.SetWindowText(displayString);

      displayString.Format("%.3f",getStencilValidationParameters().getMaxFeature());
      m_maxFeatureEditBox.SetWindowText(displayString);

      displayString.Format("%.3f",getStencilValidationParameters().getMinInset());
      m_minInsetEditBox.SetWindowText(displayString);

      //    -    -    -    -    -    -    -    -    -    -    -    -    -    -
      StencilStatusTag status;

      status = getParent()->getStencilUi().getGenerationStatus();
      m_generationStatusStatic.SetText("Generation Status: " + stencilStatusToString(status));
      m_generationStatusStatic.SetTextColor(getStatusColor(status));

      status = getParent()->getStencilUi().getValidationStatus();
      m_validationStatusStatic.SetText("Validation Status: " + stencilStatusToString(status));
      m_validationStatusStatic.SetTextColor(getStatusColor(status));

      StencilStatusTag reportStatus = getParent()->getStencilUi().getReportStatus();
      m_reportStatusStatic.SetText("Report Status: " + stencilStatusToString(reportStatus));
      m_reportStatusStatic.SetTextColor(getStatusColor(reportStatus));

      int validationErrorCount = getParent()->getStencilUi().getValidationErrorCount();
      CString validationErrorCountString;
      validationErrorCountString.Format("%d",validationErrorCount);
      COLORREF color = colorLtGray;

      if (validationErrorCount < 0)
      {
         validationErrorCountString = "Unknown";
         color = colorYellow;
      }
      else if (validationErrorCount == 0)
      {
         color = colorGreen;
      }
      else
      {
         color = colorRed;
      }

      m_validatationErrorCountStatic.SetText("Error Count: " + validationErrorCountString);
      m_validatationErrorCountStatic.SetTextColor(color);

      GetDlgItem(IDC_DisplayPinReportsButton)->EnableWindow(reportStatus != StencilStatusDoesNotExist);
   }

   //    -    -    -    -    -    -    -    -    -    -    -    -    -    -
   CResizingPropertyPage::DoDataExchange(pDX);

   DDX_Control(pDX, IDC_HoleToWallAreaEditBox, m_holeToWallAreaEditBox);
   DDX_Control(pDX, IDC_MinimumFeatureEditBox, m_minFeatureEditBox);
   DDX_Control(pDX, IDC_MaximumFeatureEditBox, m_maxFeatureEditBox);
   //DDX_Control(pDX, IDC_WebWidth, m_webWidthEditBox);
   DDX_Control(pDX, IDC_MinimumInsetEditBox, m_minInsetEditBox);
   DDX_Control(pDX, IDC_GenerationStatus, m_generationStatusStatic);
   DDX_Control(pDX, IDC_CheckStatus, m_validationStatusStatic);
   DDX_Control(pDX, IDC_ValidationErrorCount, m_validatationErrorCountStatic);
   DDX_Control(pDX, IDC_ReportStatus, m_reportStatusStatic);
   DDX_Control(pDX, IDC_AutoOpenLogFile, m_autoOpenLogFileCheckBox);

   //    -    -    -    -    -    -    -    -    -    -    -    -    -    -
   if (pDX->m_bSaveAndValidate)
   {
   }
   else
   {
      bool errorsBrowsableFlag =
         ((getParent()->getStencilUi().getValidationStatus() == StencilStatusUpToDate) &&
         (getParent()->getStencilUi().getValidationErrorCount() > 0) );

      GetDlgItem(IDC_BrowseValidationErrors)->EnableWindow(errorsBrowsableFlag);

      bool validationPossibleFlag =
         (getParent()->getStencilUi().getGenerationStatus() == StencilStatusUpToDate);

      GetDlgItem(IDC_ProcessValidationRules)->EnableWindow(validationPossibleFlag);
   }
}

//void CStencilValidationRulesPropertyPage::checkPinReportFile()
//{
//   m_reportFileExists = fileExists(getParent()->getStencilUi().getPinReportFilePath());
//}

//void CStencilValidationRulesPropertyPage::setValidationStatus(ValidationStatusTag validationStatus)
//{
//   m_validationStatus = validationStatus;
//
//   UpdateData(false);
//}
//
//void CStencilValidationRulesPropertyPage::setValidationErrorCount(int errorCount)
//{
//   m_validationErrorCount = errorCount;
//
//   UpdateData(false);
//}

BEGIN_MESSAGE_MAP(CStencilValidationRulesPropertyPage, CResizingPropertyPage)
   ON_BN_CLICKED(IDC_SetToDefaultsButton, OnBnClickedSetToDefaultsButton)
   ON_EN_KILLFOCUS(IDC_HoleToWallAreaEditBox, OnEnKillFocusHoleToWallAreaEditBox)
   ON_EN_KILLFOCUS(IDC_MinimumFeatureEditBox, OnEnKillFocusMinFeatureEditBox)
   ON_EN_KILLFOCUS(IDC_MaximumFeatureEditBox, OnEnKillFocusMaxFeatureEditBox)
   //ON_EN_KILLFOCUS(IDC_WebWidth, OnEnKillFocusWebWidthEditBox)
   ON_EN_KILLFOCUS(IDC_MinimumInsetEditBox, OnEnKillFocusMinInsetEditBox)
   ON_BN_CLICKED(IDC_ProcessValidationRules, OnBnClickedProcessValidationRules)
   ON_BN_CLICKED(IDC_BrowseValidationErrors, OnBnClickedBrowseValidationErrors)
   ON_BN_CLICKED(IDC_DisplayPinReportsButton, OnBnClickedDisplayPinReportsButton)
   ON_BN_CLICKED(IDC_GenerateStencils, OnBnClickedGenerateStencils)
   ON_WM_ACTIVATE()
   ON_BN_CLICKED(IDC_AutoOpenLogFile, OnBnClickedAutoOpenLogFile)
END_MESSAGE_MAP()


// CStencilValidationRulesPropertyPage message handlers

BOOL CStencilValidationRulesPropertyPage::OnInitDialog()
{
#ifdef STENCIL_TIMER
   GetStencilTimer().AddMessage("enter CStencilValidationRulesPropertyPage::OnInitDialog()");
#endif

   CResizingPropertyPage::OnInitDialog();

   // TODO:  Add extra initialization here
   //if (getParent()->getPcbFileNum() >= 0)
   //{
   //   CCamCadDatabase& camCadDatabase = getParent()->getCamCadDatabase();
   //   CWriteFormat& logFile = getParent()->getStencilUi().getLogFile();

   //   CStencilValidationParameters::registerStencilValidationKeywords(camCadDatabase,logFile);

   //   getStencilValidationParameters().readAttributes(getParent()->getPcbFileNum());
   //}

   UpdateData(false);

   //if (getParent()->getPcbFileNum() < 0)
   //{
   //   getStencilValidationParameters().setToDefaults();

   //   getParent()->getStencilUi().generateEvent(ID_StencilCommand_ModifiedValidationRules);
   //}

   m_generationStatusStatic.SetBorder(true);
   m_generationStatusStatic.SetBkColor(colorBlack);
   m_generationStatusStatic.SetFontBold(true);

   m_validationStatusStatic.SetBorder(true);
   m_validationStatusStatic.SetBkColor(colorBlack);
   m_validationStatusStatic.SetFontBold(true);

   m_reportStatusStatic.SetBorder(true);
   m_reportStatusStatic.SetBkColor(colorBlack);
   m_reportStatusStatic.SetFontBold(true);

   m_validatationErrorCountStatic.SetBorder(true);
   m_validatationErrorCountStatic.SetBkColor(colorBlack);
   m_validatationErrorCountStatic.SetFontBold(true);

   //checkPinReportFile();

   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey("Settings");
   m_autoOpenFlag = true;

   if (settings.isOpen())
   {
      CRegistryKey registryKey = settings.createSubKey("StencilGenerator");

      if (registryKey.isOpen())
      {
         registryKey.getValue("AutoOpenLogFile",m_autoOpenFlag);
         m_autoOpenLogFileCheckBox.SetCheck(m_autoOpenFlag);
      }
   }

   UpdateData(false);

#ifdef STENCIL_TIMER
   GetStencilTimer().AddMessage("exit  CStencilValidationRulesPropertyPage::OnInitDialog()");
#endif

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CStencilValidationRulesPropertyPage::OnKillActive()
{
   UpdateData(true);

   getStencilValidationParameters().writeAttributes(getParent()->getStencilUi().getPcbFileNum());

   return CResizingPropertyPage::OnKillActive();
}

void CStencilValidationRulesPropertyPage::OnBnClickedProcessValidationRules()
{
   getParent()->getStencilUi().performStencilValidationChecks();
   //checkPinReportFile();

   //UpdateData(false);
}

void CStencilValidationRulesPropertyPage::OnBnClickedSetToDefaultsButton()
{
   getStencilValidationParameters().setToDefaults();

   getParent()->getStencilUi().OnEventModifiedValidationRules();

   UpdateData(false);
}

void CStencilValidationRulesPropertyPage::OnEnKillFocusHoleToWallAreaEditBox()
{
   CString stringValue;

   m_holeToWallAreaEditBox.GetWindowText(stringValue);
   getStencilValidationParameters().setMinHoleAreaToWallAreaRatio(atof(stringValue));
   getStencilValidationParameters().writeAttributes(getParent()->getStencilUi().getPcbFileNum());

   getParent()->getStencilUi().OnEventModifiedValidationRules();

   UpdateData(false);
}

void CStencilValidationRulesPropertyPage::OnEnKillFocusMinFeatureEditBox()
{
   CString stringValue;

   m_minFeatureEditBox.GetWindowText(stringValue);
   getStencilValidationParameters().setMinFeature(atof(stringValue));
   getStencilValidationParameters().writeAttributes(getParent()->getStencilUi().getPcbFileNum());

   getParent()->getStencilUi().OnEventModifiedValidationRules();

   UpdateData(false);
}

void CStencilValidationRulesPropertyPage::OnEnKillFocusMaxFeatureEditBox()
{
   CString stringValue;

   m_maxFeatureEditBox.GetWindowText(stringValue);
   getStencilValidationParameters().setMaxFeature(atof(stringValue));
   getStencilValidationParameters().writeAttributes(getParent()->getStencilUi().getPcbFileNum());

   getParent()->getStencilUi().OnEventModifiedValidationRules();

   UpdateData(false);
}

void CStencilValidationRulesPropertyPage::OnEnKillFocusMinInsetEditBox()
{
   CString stringValue;

   m_minInsetEditBox.GetWindowText(stringValue);
   getStencilValidationParameters().setMinInset(atof(stringValue));
   getStencilValidationParameters().writeAttributes(getParent()->getStencilUi().getPcbFileNum());

   getParent()->getStencilUi().OnEventModifiedValidationRules();

   UpdateData(false);
}

void CStencilValidationRulesPropertyPage::OnBnClickedBrowseValidationErrors()
{
   // TODO: Add your control notification handler code here
   //getParent()->getCamCadDatabase().getCamCadDoc().OnRedraw();
   getParent()->EndDialog(BrowseValidationErrors);
}

void CStencilValidationRulesPropertyPage::OnBnClickedDisplayPinReportsButton()
{
   // TODO: Add your control notification handler code here
   CString reportFilePath(getParent()->getStencilUi().getPinReportFilePath());

   if (! reportFilePath.IsEmpty())
   {
      if (fileExists(reportFilePath))
      {
         ShellExecute(AfxGetApp()->GetMainWnd()->m_hWnd,"open",reportFilePath,NULL,NULL,SW_SHOW);
      }
   }
}

void CStencilValidationRulesPropertyPage::OnBnClickedGenerateStencils()
{
   getParent()->getStencilUi().renderStencils();
}

void CStencilValidationRulesPropertyPage::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
   CResizingPropertyPage::OnActivate(nState, pWndOther, bMinimized);
}

void CStencilValidationRulesPropertyPage::OnBnClickedAutoOpenLogFile()
{

   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey("Settings");

   if (settings.isOpen())
   {
      CRegistryKey registryKey = settings.createSubKey("StencilGenerator");

      if (registryKey.isOpen())
      {
         m_autoOpenFlag = (m_autoOpenLogFileCheckBox.GetCheck() != 0);
         registryKey.setValue("AutoOpenLogFile",m_autoOpenFlag);
      }
   }
}

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CStencilRulePreviewPage, CResizingPropertyPage)

CStencilRulePreviewPage::CStencilRulePreviewPage()
: CResizingPropertyPage(CStencilRulePreviewPage::IDD)
, m_treeListFrame(*this)
, m_parameters(true)
, m_treeItemLockedFlag(false)
{
   m_psp.pszTitle = "";
   m_psp.dwFlags |= PSP_USETITLE;

   addFieldControl(IDC_TreeListCtrl,anchorLeft,growVertical);

   m_treeRevLevel           = -1;    // valid values start at 0, this -1 ensures initial filling of tree
   m_fillingTreeFlag        = false;
   m_enableGeometryViewFlag = false;
   m_stencilRuleChangedFlag = false;
   m_updatingDataFlag       = false;
   m_stencilRuleComboBoxSelectedIndex = -1;
   m_geometryPlacedTopFlag  = true;
   //m_assignedFlag           = false;
   setRuleAssignmentType(ruleAssignmentTypeImplicitlyInherited);

   // standard aperture
   m_standardApertureShape        = NULL;
   m_genericStandardApertureShape = NULL;
   m_initialApertureType          = standardApertureCircle;

   m_componentViewGeometry        = NULL;
   //m_selectedStencilPin           = NULL;
   m_selectedStencilSubPin        = NULL;
   m_selectedTreeItem             = NULL;
   m_lockedTreeItem               = NULL;
   m_lockSelectedTreeItem         = NULL;
   //m_selectedImageIndex           = -1;
   //m_camCadDatabase        = NULL;

   m_parameters.addParameter(m_parameterCaption0,m_valueEditBox0,m_parameterCaption5);
   m_parameters.addParameter(m_parameterCaption1,m_valueEditBox1,m_parameterCaption5);
   m_parameters.addParameter(m_parameterCaption2,m_valueEditBox2,m_parameterCaption5);
   m_parameters.addParameter(m_parameterCaption3,m_valueEditBox3,m_parameterCaption5);
   m_parameters.addParameter(m_parameterCaption4,m_valueEditBox4,m_parameterCaption5);
   m_parameters.addCenters(m_originGroupBox,m_originOptionsComboBox,m_displayCenterAnnotationsCheckBox);

   // Resizing
   CResizingDialogField& expandButtonField = addFieldControl(IDC_EXPANDALL,anchorBottom);
   addFieldControl(IDC_COLLAPSEALL,anchorBottom);
   addFieldControl(IDC_ShowRules,anchorBottom);
   addFieldControl(IDC_RemoveAllRules,anchorBottom);

   // left side
   CResizingDialogField& listField = addFieldControl(IDC_TreeListCtrl,anchorTop);
   listField.getOperations().addOperation(glueBottomEdge   ,toTopEdge,&expandButtonField);

   // anchor fields
   CResizingDialogField& symmetryAxisGroupField = addFieldControl(IDC_SymmetryAxisGroup,anchorLeft);
   CResizingDialogField& rightAnchorStaticField = addFieldControl(IDC_AnchorStatic,anchorRight);

   // independent fields
   CResizingDialogField& apertureDiagramField   = addFieldControl(IDC_ApertureDiagram,anchorProportionalVertical,growProportionalVertical);
   apertureDiagramField.getOperations().addOperation(glueRightEdge   ,toRightEdge,&rightAnchorStaticField);

   // complete dependents
   CResizingDialogField& genericApertureDiagramField = addFieldControl(IDC_GenericApertureDiagram,anchorBottomRight);
   genericApertureDiagramField.getOperations().addOperation(glueTopLeftCorner   ,toBottomLeftCorner,&apertureDiagramField);

   CResizingDialogField& apertureUnitsStaticField   = addFieldControl(IDC_ApertureUnitsStatic,anchorLeft,growHorizontal);
   apertureUnitsStaticField.getOperations().addOperation(glueBottomEdge   ,toTopEdge,&apertureDiagramField);
   apertureUnitsStaticField.getOperations().addOperation(glueTopEdge      ,toTopEdge,&apertureDiagramField);

   CResizingDialogField& descriptorField   = addFieldControl(IDC_Descriptor,anchorLeft,growHorizontal);
   descriptorField.getOperations().addOperation(glueBottomEdge   ,toTopEdge,&apertureDiagramField);
   descriptorField.getOperations().addOperation(glueTopEdge      ,toTopEdge,&apertureDiagramField);

   CResizingDialogField& genericDescriptorField   = addFieldControl(IDC_GenericDescriptor,anchorLeft,growHorizontal);
   genericDescriptorField.getOperations().addOperation(glueTopEdge   ,toBottomEdge,&apertureDiagramField);
   genericDescriptorField.getOperations().addOperation(glueBottomEdge,toBottomEdge,&apertureDiagramField);

   CResizingDialogField& geometryDiagramField = addFieldControl(IDC_GeometryDiagram,anchorLeft);
   geometryDiagramField.getOperations().addOperation(glueBottomRightCorner   ,toTopRightCorner,&apertureDiagramField);

   // second level dependant anchor
   CResizingDialogField& geometryNameField   = addFieldControl(IDC_GeometryName,anchorLeft);
   geometryNameField.getOperations().addOperation(glueBottomEdge   ,toBottomEdge,&geometryDiagramField);
   geometryNameField.getOperations().addOperation(glueTopEdge      ,toBottomEdge,&geometryDiagramField);

   // second level dependencies
   addFieldControl(IDC_GeometryPlacedTopBottom,anchorRelativeToField,&geometryNameField);
   addFieldControl(IDC_InheritedRuleEditBox,anchorRelativeToField,&geometryNameField);
   addFieldControl(IDC_ParameterCaption0,anchorRelativeToField,&geometryNameField);
   addFieldControl(IDC_ParameterCaption1,anchorRelativeToField,&geometryNameField);
   addFieldControl(IDC_ParameterCaption2,anchorRelativeToField,&geometryNameField);
   addFieldControl(IDC_ParameterCaption3,anchorRelativeToField,&geometryNameField);
   addFieldControl(IDC_ParameterCaption4,anchorRelativeToField,&geometryNameField);
   addFieldControl(IDC_ParameterCaption5,anchorRelativeToField,&geometryNameField);
   addFieldControl(IDC_ApertureType,anchorRelativeToField,&geometryNameField);
   addFieldControl(IDC_APPLY,anchorRelativeToField,&geometryNameField);
   addFieldControl(IDC_OriginGroup,anchorRelativeToField,&geometryNameField);
   addFieldControl(IDC_SwapWidthAndHeight,anchorRelativeToField,&geometryNameField);
   addFieldControl(IDC_OriginOptions,anchorRelativeToField,&geometryNameField);
   addFieldControl(IDC_DisplayCenterAnnotations,anchorRelativeToField,&geometryNameField);

   addFieldControl(IDC_CopperPadExtentsGroup, anchorRelativeToField, &geometryNameField);
   addFieldControl(IDC_CopperPadExtentsStatic,anchorRelativeToField, &geometryNameField);
   addFieldControl(IDC_PinPitchGroup,         anchorRelativeToField, &geometryNameField);
   addFieldControl(IDC_PinPitchStatic,        anchorRelativeToField, &geometryNameField);

   addFieldControl(IDC_ParameterValue0,anchorRelativeToField,&geometryNameField);
   addFieldControl(IDC_ParameterValue1,anchorRelativeToField,&geometryNameField);
   addFieldControl(IDC_ParameterValue2,anchorRelativeToField,&geometryNameField);
   addFieldControl(IDC_ParameterValue3,anchorRelativeToField,&geometryNameField);
   addFieldControl(IDC_ParameterValue4,anchorRelativeToField,&geometryNameField);
}

CStencilRulePreviewPage::~CStencilRulePreviewPage()
{
   delete m_standardApertureShape;
   delete m_genericStandardApertureShape;
}

CCamCadDatabase& CStencilRulePreviewPage::getCamCadDatabase()
{
   return getParent()->getCamCadDatabase();
}

CStencilGenerator& CStencilRulePreviewPage::getStencilGenerator()
{
   return getParent()->getStencilGenerator();
}

PageUnitsTag CStencilRulePreviewPage::getPageUnits() const
{
   return getParent()->getCamCadDatabase().getPageUnits();
}

bool CStencilRulePreviewPage::getItemLocked() const
{
   return m_treeItemLockedFlag;
}

HTREEITEM CStencilRulePreviewPage::getLockSelectedItem() const
{
   return m_lockSelectedTreeItem;
}

void CStencilRulePreviewPage::setLockSelectedItem(HTREEITEM item)
{
   m_lockSelectedTreeItem = item;
}

void CStencilRulePreviewPage::setItemLocked(bool lockFlag)
{
   if (m_treeItemLockedFlag)
   {
      if (!lockFlag)
      {
         if (m_lockedTreeItem != NULL)
         {
            getTreeListControl().SelectItem(m_lockedTreeItem);
         }
      }
   }
   else
   {
      if (lockFlag)
      {
         m_lockedTreeItem = getTreeListControl().GetSelectedItem();
      }
   }

   if (!lockFlag)
   {
      m_lockedTreeItem = NULL;
   }

   m_treeItemLockedFlag = lockFlag;
}

void CStencilRulePreviewPage::DoDataExchange(CDataExchange* pDX)
{
   if (m_updatingDataFlag)
   {
      return;
   }

   m_updatingDataFlag = true;
   PageUnitsTag pageUnits = getCamCadDatabase().getPageUnits();

   if (! pDX->m_bSaveAndValidate && ::IsWindow(m_stencilRuleGroup))
   {
      //// test
      //m_symmetryAxisGroup.SetTextColor(getAssignedColor());
      //m_symmetryAxisGroup.SetBoxColors(getAssignedColor(),getAssignedColor());
      //m_assignSymmetryAxisButton.SetColor(COLOR_BTNTEXT,getAssignedColor());

      //m_orientationGroup.SetTextColor(getInheritedColor());
      //m_orientationGroup.SetBoxColors(getInheritedColor(),getInheritedColor());
      //m_assignOrientationButton.SetColor(COLOR_BTNTEXT,getInheritedColor());

      // tree item change
      HTREEITEM selectedTreeItem = getTreeListControl().GetSelectedItem();
      CStencilTreeListItem* treeListItem = NULL;
      bool treeItemChangedFlag = (selectedTreeItem != m_selectedTreeItem);

      // stencil pin change
      //CStencilPin* selectedStencilPin = ((selectedTreeItem != NULL) ? (CStencilPin*)getTreeListControl().GetItemData(selectedTreeItem) : NULL);
      CStencilSubPin* selectedStencilSubPin  = ((selectedTreeItem != NULL) ? getTreeListControl().getSubPinItemData(selectedTreeItem) : NULL);

      //bool stencilPinChangedFlag = (selectedStencilPin != m_selectedStencilPin);
      bool stencilPinChangedFlag = (selectedStencilSubPin != m_selectedStencilSubPin);

      HTREEITEM oldItem    = m_selectedTreeItem;
      m_selectedTreeItem   = selectedTreeItem;
      //m_selectedStencilPin = selectedStencilPin;
      m_selectedStencilSubPin = selectedStencilSubPin;

      bool stencilPinSelectedFlag = (selectedStencilSubPin != NULL);

      if (stencilPinSelectedFlag)
      {
         treeListItem  = (CStencilTreeListItem*)this->getTreeListControl().GetTreeItemData(selectedTreeItem);
         m_selectStencilPinTopFlag           = treeListItem->getTopFlag();
         m_selectedStencilPinAttributeSource = treeListItem->getAttributeSource();
      }
      else
      {
         m_ruleStatusStatic.setStatus(ruleStatusNoSelectedPin);
         m_enableGeometryViewFlag = false;
      }

      if (treeItemChangedFlag)
      {
         StencilRuleTag ruleTag = m_stencilRuleComboBox.getSelected();
         getParent()->initializeAttributeValues(true);
         //m_stencilRuleValueComboBox.initialize(stencilAttribute);
         //m_stencilRuleModifierComboBox.initialize(stencilAttribute);

         if (oldItem != NULL)
         {
            getTreeListControl().invalidateItem(oldItem);
         }

         // knv - experimental fix 20080114
         if (stencilPinSelectedFlag)
         {
            m_geometryPlacedTopFlag = m_selectStencilPinTopFlag;
         }
      }

      updateStencilPinAttributeDataControls();

      bool apertureRuleFlag = stencilPinSelectedFlag && m_stencilRuleComboBox.getSelected() == stencilRuleAperture;
      //bool offsetModifierFlag = apertureRuleFlag && m_stencilRuleModifierComboBox.getSelected() == stencilRuleModifierOffsetRelativeToPadstack;
      CString apertureDescriptor = (apertureRuleFlag ? m_stencilRuleValueComboBox.getSelected() : "");

      if (treeItemChangedFlag || stencilPinChangedFlag || m_stencilRuleChangedFlag)
      {
         if (stencilPinSelectedFlag)
         {
            CString description,geometryHeading,derivationMethod;

            if (selectedStencilSubPin != NULL)
            {
               DataStruct& component = selectedStencilSubPin->getComponent();
               BlockStruct* componentGeometry = getCamCadDatabase().getBlock(component.getInsert()->getBlockNumber());

               CStencilHole& stencilHole = selectedStencilSubPin->getStencilHole(m_selectStencilPinTopFlag);
               stencilHole.updateStencilHoleAttribute();
               derivationMethod = stencilHole.getDerivationMethodForSourceLevel(m_selectedStencilPinAttributeSource);

               CStencilRuleAttributes stencilRuleAttributes = stencilHole.getStencilRuleAttributes();
               CString attributeValue = stencilRuleAttributes.getRule().getRuleValue();

               //CString attributeName     = stencilHole.getStencilAttributeName();
               //CString attributeValue    = stencilHole.getStencilAttributeStringValue();
               CString itemDescription   = treeListItem->GetItemString();
               geometryHeading           = "Geometry: " + componentGeometry->getName();

               m_enableGeometryViewFlag = true;

               switch (m_selectedStencilPinAttributeSource)
               {
               case attributeSourceGeometry:
                  if (apertureRuleFlag)
                  {
                     setApertureTypeFromDescriptor(m_stencilRuleValueComboBox.getSelected());
                  }

                  m_ruleStatusStatic.setStatus(
                     setComponentViewGeometry(componentGeometry,*selectedStencilSubPin,m_selectStencilPinTopFlag,
                     m_geometryPlacedTopFlag,m_selectedStencilPinAttributeSource));

                  description = "Geometry: " + itemDescription;
                  break;
               case attributeSourcePadstack:
                  if (apertureRuleFlag)
                  {
                     setApertureTypeFromDescriptor(m_stencilRuleValueComboBox.getSelected());
                  }

                  m_ruleStatusStatic.setStatus(
                     setComponentViewGeometry(componentGeometry,*selectedStencilSubPin,m_selectStencilPinTopFlag,
                     m_geometryPlacedTopFlag,m_selectedStencilPinAttributeSource));

                  description = "Padstack: " + itemDescription;
                  break;
               case attributeSourceSubPadstack:
                  if (apertureRuleFlag)
                  {
                     setApertureTypeFromDescriptor(m_stencilRuleValueComboBox.getSelected());
                  }

                  m_ruleStatusStatic.setStatus(
                     setComponentViewGeometry(componentGeometry,*selectedStencilSubPin,m_selectStencilPinTopFlag,
                     m_geometryPlacedTopFlag,m_selectedStencilPinAttributeSource));

                  description = "SubPadstack: " + itemDescription;
                  break;
               case attributeSourceComponent:
                  if (apertureRuleFlag)
                  {
                     setApertureTypeFromDescriptor(m_stencilRuleValueComboBox.getSelected());
                  }

                  m_ruleStatusStatic.setStatus(
                     setComponentViewComponent(&component,*selectedStencilSubPin,m_selectStencilPinTopFlag,
                     m_geometryPlacedTopFlag,m_selectedStencilPinAttributeSource));

                  description = "Component: " + itemDescription;
                  break;
               case attributeSourceGeometryPin:
                  if (apertureRuleFlag)
                  {
                     setApertureTypeFromDescriptor(m_stencilRuleValueComboBox.getSelected());
                  }

                  m_ruleStatusStatic.setStatus(
                     setComponentViewGeometry(componentGeometry,*selectedStencilSubPin,m_selectStencilPinTopFlag,
                     m_geometryPlacedTopFlag,m_selectedStencilPinAttributeSource));

                  description = "Geometry Pin: " + componentGeometry->getName() + "." + itemDescription;
                  break;
               case attributeSourceComponentPin:
                  {
                     if (apertureRuleFlag)
                     {
                        setApertureTypeFromDescriptor(m_stencilRuleValueComboBox.getSelected());
                     }

                     // debug
                     //CStencilRuleAttributes stencilRuleAttributes = 
                     //   stencilHole.getStencilRuleAttributes(m_selectedStencilPinAttributeSource);
                     //CString iii = stencilRuleAttributes.getRule().getRuleString();

                     m_ruleStatusStatic.setStatus(
                        setComponentViewComponent(&component,*selectedStencilSubPin,m_selectStencilPinTopFlag,
                        m_geometryPlacedTopFlag,m_selectedStencilPinAttributeSource));

                     description = "Physical Pin: " + selectedStencilSubPin->getPinRef();
                  }

                  break;
               case attributeSourceComponentSubPin:
                  {
                     if (apertureRuleFlag)
                     {
                        setApertureTypeFromDescriptor(m_stencilRuleValueComboBox.getSelected());
                     }

                     // debug
                     //CStencilRuleAttributes stencilRuleAttributes = 
                     //   stencilHole.getStencilRuleAttributes(m_selectedStencilPinAttributeSource);
                     //CString iii = stencilRuleAttributes.getRule().getRuleString();

                     m_ruleStatusStatic.setStatus(
                        setComponentViewComponent(&component,*selectedStencilSubPin,m_selectStencilPinTopFlag,
                        m_geometryPlacedTopFlag,m_selectedStencilPinAttributeSource));

                     description = "Physical SubPin: " + selectedStencilSubPin->getSubPinRef();
                  }

                  break;
               default:
                  if (apertureRuleFlag)
                  {
                     setApertureTypeFromDescriptor(m_stencilRuleValueComboBox.getSelected());
                  }

                  m_ruleStatusStatic.setStatus(
                     setComponentViewComponent(NULL,*selectedStencilSubPin,m_selectStencilPinTopFlag,
                     m_geometryPlacedTopFlag,m_selectedStencilPinAttributeSource));

                  description = itemDescription;
                  geometryHeading.Empty();
                  m_enableGeometryViewFlag = false;
                  break;
               }
            }
            else
            {
               m_ruleStatusStatic.setStatus(ruleStatusNoSelectedPin);
               derivationMethod = "None";
            }

            //m_selectedEntityStatic.SetWindowText(description);
            m_geometryNameStatic.SetWindowText(geometryHeading);
            //m_inheritedRuleStatic.SetWindowText("Derivation Method: " + derivationMethod);
         }
         else // no stencil pin is selected
         {
            m_ruleStatusStatic.setStatus(ruleStatusNoSelectedPin);
         }
      }
      else // no change in tree or stencil pin
      {
      }

      //updateStencilPinAttributeDataControls();

      int showValue = (apertureRuleFlag ? SW_SHOW : SW_HIDE);

      // generic aperture view
      m_genericApertureViewStatic.ShowWindow(showValue);
      m_genericDescriptorStatic.ShowWindow(showValue);

      // aperture type
      m_apertureTypeComboBox.ShowWindow(showValue);

      // apeture units
      m_apertureUnitsStatic.ShowWindow(showValue);
      //m_apertureUnitsGroup.ShowWindow(showValue);

      // descriptor
      m_descriptorEditBox.ShowWindow(showValue);

      // apply button
      m_applyButton.ShowWindow(showValue);
      m_applyButton2.ShowWindow(SW_SHOW + SW_HIDE - showValue);

      if (!apertureRuleFlag)
      {
         m_parameters.clear();
      }
      else
      {
         CString apertureDescriptor = m_stencilRuleValueComboBox.getSelected();

         if (!m_enableGeometryViewFlag)
         {
            setApertureFromDescriptor(apertureDescriptor);
         }
         else
         {
            CStandardApertureShape* standardApertureShape =
               CStandardApertureShape::create(apertureDescriptor,CStandardApertureShape::getDefaultPageUnits());

            standardApertureShape->makeValid();
            standardApertureShape->storeToApertureParameters(m_parameters);
            m_descriptorEditBox.SetWindowText(apertureDescriptor);
            m_stencilRuleValueComboBox.setSelected(apertureDescriptor);

            PageUnitsTag pageUnits = getPageUnits();
            int exponent = CStandardApertureShape::getDefaultDescriptorDimensionFactorExponent(pageUnits);

            CStandardApertureShape* genericApertureShape =
               CStandardApertureShape::create(standardApertureShape->getType(),pageUnits,exponent);
            setGenericStandardApertureShape(*genericApertureShape);
            delete genericApertureShape;

            delete standardApertureShape;
         }
      }

      // swap width and height button
      m_swapWidthWithHeightButton.ShowWindow((apertureRuleFlag && 
         m_parameters.lookupCaptionPrefix("w") != NULL &&
         m_parameters.lookupCaptionPrefix("h") != NULL   ) ? SW_SHOW : SW_HIDE);

      if (m_enableGeometryViewFlag)
      {
         CString extentsString;
         //CString unitsString;
         CExtent copperExtent = m_apertureViewStatic.getCopperExtent();
         //CUnits docUnits(getCamCadDatabase().getPageUnits());
         //double xExtent,yExtent;

         //if (isEnglish(getCamCadDatabase().getPageUnits()))
         //{  // use mils for display of english units
         //   xExtent = docUnits.convertTo(pageUnitsMils,copperExtent.getXsize());
         //   yExtent = docUnits.convertTo(pageUnitsMils,copperExtent.getYsize());
         //   unitsString = "mils";
         //}
         //else
         //{  // use centimillimeters for display of metric units
         //   xExtent = 100. * docUnits.convertTo(pageUnitsMilliMeters,copperExtent.getXsize());
         //   yExtent = 100. * docUnits.convertTo(pageUnitsMilliMeters,copperExtent.getYsize());
         //   unitsString = "centimillimeters";
         //}

         //extentsString.Format("x=%s y=%s (%s)",fpfmt(xExtent,3),fpfmt(yExtent,3),unitsString);
         double sizeX = 0.0;
         double sizeY = 0.0;
         if (copperExtent.isValid())
         {
            sizeX = copperExtent.getXsize();
            sizeY = copperExtent.getYsize();
         }

         extentsString.Format("x=%s y=%s",fpfmt(sizeX,3),fpfmt(sizeY,3));
         m_copperPadExtentsStatic.SetWindowText(extentsString);

         // Pin pitch attribute, not necessarily present
         DataStruct& component = selectedStencilSubPin->getComponent();
         Attrib *ppattrib = NULL;
         CString ppattribVal;
         // It would be resonable to cache this keyword index so we don't have to do this lookup all the time.
         // It would even be reasonable to cache the pin_pitch attrib in the SG stencilPin data so we don't have to look it
         // up at all, it is just there. But that is more change, this works, and I'm not seeing a speed problem.
         int pinPitchKw = this->getCamCadDatabase().getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributePinPitch);
         if (component.lookUpAttrib(pinPitchKw, ppattrib))
            ppattribVal = ppattrib->getStringValue();
         else
            ppattribVal.Format("%s attribute not found", StandardAttributeTagToName(standardAttributePinPitch));
         m_geometryPinPitchStatic.SetWindowText(ppattribVal);
      }

      m_geometryViewStatic.ShowWindow(m_enableGeometryViewFlag ? SW_SHOW : SW_HIDE);
      m_apertureViewStatic.ShowWindow((m_enableGeometryViewFlag || apertureRuleFlag) ? SW_SHOW : SW_HIDE);
      m_copperPadExtentsGroup.ShowWindow(m_enableGeometryViewFlag ? SW_SHOW : SW_HIDE);
      m_copperPadExtentsStatic.ShowWindow(m_enableGeometryViewFlag ? SW_SHOW : SW_HIDE);
      m_geometryPinPitchGroup.ShowWindow(m_enableGeometryViewFlag ? SW_SHOW : SW_HIDE);
      m_geometryPinPitchStatic.ShowWindow(m_enableGeometryViewFlag ? SW_SHOW : SW_HIDE);
      m_geometryPlacedTopBottomButton.SetWindowText(m_geometryPlacedTopFlag ? "Placed Top" : "Placed Bottom");
      m_geometryPlacedTopBottomButton.ShowWindow(m_enableGeometryViewFlag ? SW_SHOW : SW_HIDE);

      m_stencilRuleChangedFlag = false;
   }

   CResizingPropertyPage::DoDataExchange(pDX);

   // standard aperture
   //DDX_Control(pDX, IDC_GenericParameterCaption0, m_genericParameterCaption0);
   //DDX_Control(pDX, IDC_GenericParameterCaption1, m_genericParameterCaption1);
   //DDX_Control(pDX, IDC_GenericParameterCaption2, m_genericParameterCaption2);
   //DDX_Control(pDX, IDC_GenericParameterCaption3, m_genericParameterCaption3);
   //DDX_Control(pDX, IDC_GenericParameterCaption4, m_genericParameterCaption4);
   //DDX_Control(pDX, IDC_GenericParameterCaption5, m_genericParameterCaption5);
   DDX_Control(pDX, IDC_ParameterCaption0, m_parameterCaption0);
   DDX_Control(pDX, IDC_ParameterCaption1, m_parameterCaption1);
   DDX_Control(pDX, IDC_ParameterCaption2, m_parameterCaption2);
   DDX_Control(pDX, IDC_ParameterCaption3, m_parameterCaption3);
   DDX_Control(pDX, IDC_ParameterCaption4, m_parameterCaption4);
   DDX_Control(pDX, IDC_ParameterCaption5, m_parameterCaption5);
   DDX_Control(pDX, IDC_ParameterValue0, m_valueEditBox0);
   DDX_Control(pDX, IDC_ParameterValue1, m_valueEditBox1);
   DDX_Control(pDX, IDC_ParameterValue2, m_valueEditBox2);
   DDX_Control(pDX, IDC_ParameterValue3, m_valueEditBox3);
   DDX_Control(pDX, IDC_ParameterValue4, m_valueEditBox4);
   //DDX_Control(pDX, IDC_ParameterValue5, m_valueEditBox5);
   DDX_Control(pDX, IDC_Descriptor, m_descriptorEditBox);
   DDX_Control(pDX, IDC_GenericDescriptor, m_genericDescriptorStatic);
   DDX_Control(pDX, IDC_ApertureType, m_apertureTypeComboBox);
   //DDX_Control(pDX, IDC_GenericParameterDescription0, m_parameterDescriptionStatic0);
   //DDX_Control(pDX, IDC_GenericParameterDescription1, m_parameterDescriptionStatic1);
   //DDX_Control(pDX, IDC_GenericParameterDescription2, m_parameterDescriptionStatic2);
   //DDX_Control(pDX, IDC_GenericParameterDescription3, m_parameterDescriptionStatic3);
   //DDX_Control(pDX, IDC_GenericParameterDescription4, m_parameterDescriptionStatic4);
   //DDX_Control(pDX, IDC_GenericParameterDescription5, m_parameterDescriptionStatic5);

   DDX_Control(pDX, IDC_GenericApertureDiagram, m_genericApertureViewStatic);
   DDX_Control(pDX, IDC_ApertureDiagram, m_apertureViewStatic);
   DDX_Control(pDX, IDC_GeometryDiagram, m_geometryViewStatic);

   DDX_Control(pDX, IDC_ApertureUnitsStatic, m_apertureUnitsStatic);
   DDX_Control(pDX, IDC_OriginOptions, m_originOptionsComboBox);
   DDX_Control(pDX, IDC_DisplayCenterAnnotations, m_displayCenterAnnotationsCheckBox);
   DDX_Control(pDX, IDC_OriginGroup, m_originGroupBox);
   DDX_Control(pDX, IDC_GeometryName, m_geometryNameStatic);
   DDX_Control(pDX, IDC_StencilRules, m_stencilRuleComboBox);
   DDX_Control(pDX, IDC_StencilRuleValues, m_stencilRuleValueComboBox);
   DDX_Control(pDX, IDC_CommentEditBox, m_commentEditBox);

   //DDX_Control(pDX, IDC_StencilRuleModifiers, m_stencilRuleModifierComboBox);
   //DDX_Control(pDX, IDC_SelectedEntityStatic, m_selectedEntityStatic);
   //DDX_Control(pDX, IDC_ApertureUnitsGroup, m_apertureUnitsGroup);
   DDX_Control(pDX, IDC_InheritedRuleEditBox, m_inheritedRuleEditBox);
   DDX_Control(pDX, IDC_StencilRuleStatic, m_stencilRuleGroup);
   //DDX_Control(pDX, IDC_StencilRuleValueStatuc, m_stencilRuleValueStatic);
   //DDX_Control(pDX, IDC_StencilRuleModifierStatuc, m_stencilRuleModifierStatic);
   DDX_Control(pDX, IDC_CopperPadExtentsStatic, m_copperPadExtentsStatic);
   DDX_Control(pDX, IDC_CopperPadExtentsGroup, m_copperPadExtentsGroup);

   DDX_Control(pDX, IDC_PinPitchStatic, m_geometryPinPitchStatic);
   DDX_Control(pDX, IDC_PinPitchGroup, m_geometryPinPitchGroup);

   DDX_Control(pDX, IDC_APPLY, m_applyButton);
   DDX_Control(pDX, IDC_APPLY2, m_applyButton2);
   DDX_Control(pDX, IDC_XOffsetEditBox, m_xOffsetEditBox);
   DDX_Control(pDX, IDC_YOffsetEditBox, m_yOffsetEditBox);
   DDX_Control(pDX, IDC_AngleOffsetEditBox, m_angleOffsetEditBox);
   DDX_Control(pDX, IDC_XOffsetStatic, m_xOffsetStatic);
   DDX_Control(pDX, IDC_YOffsetStatic, m_yOffsetStatic);
   DDX_Control(pDX, IDC_AngleOffsetStatic, m_angleOffsetStatic);
   DDX_Control(pDX, IDC_OffsetGroup, m_offsetGroup);
   DDX_Control(pDX, IDC_RuleStatus, m_ruleStatusStatic);
   DDX_Control(pDX, IDC_SwapWidthAndHeight, m_swapWidthWithHeightButton);
   DDX_Control(pDX, IDC_GeometryPlacedTopBottom, m_geometryPlacedTopBottomButton);

   DDX_Control(pDX, IDC_AssignShapeRule, m_assignShapeRuleButton);
   DDX_Control(pDX, IDC_InheritShapeRule, m_inheritShapeRuleButton);
   //DDX_Control(pDX, IDC_AssignSymmetryAxis, m_assignSymmetryAxisButton);
   //DDX_Control(pDX, IDC_AssignOrientation, m_assignOrientationButton);
   //DDX_Control(pDX, IDC_AssignOffset, m_assignOffsetButton);
   //DDX_Control(pDX, IDC_AssignSplitParameters, m_assignSplitParametersButton);
   DDX_Control(pDX, IDC_FlippedCheckBox, m_flippedCheckBox);
   DDX_Control(pDX, IDC_OrientTowardCenterRadio, m_orientTowardCenterRadioButton);
   DDX_Control(pDX, IDC_OrientAwayFromCenterRadio, m_orientAwayFromCenterRadioButton);
   DDX_Control(pDX, IDC_OrientNormalRadio, m_orientNormalRadioButton);
   DDX_Control(pDX, IDC_OffsetTowardCenterRadio, m_offsetTowardCenterRadioButton);
   DDX_Control(pDX, IDC_OffsetAwayFromCenterRadio, m_offsetAwayFromCenterRadioButton);
   DDX_Control(pDX, IDC_OffsetNormalRadio, m_offsetNormalRadioButton);
   DDX_Control(pDX, IDC_SymmetryAxisGroup, m_symmetryAxisGroup);
   DDX_Control(pDX, IDC_OrientationGroup, m_orientationGroup);

   DDX_Control(pDX, IDC_SplitGroup, m_splitGroup);
   DDX_Control(pDX, IDC_SplitMaxFeatureStatic, m_splitMaxFeatureStatic);
   DDX_Control(pDX, IDC_SplitWebWidthStatic, m_splitWebWidthStatic);
   DDX_Control(pDX, IDC_SplitWebAngleStatic, m_splitWebAngleStatic);
   DDX_Control(pDX, IDC_MaxFeatureEditBox, m_splitMaxFeatureEditBox);
   DDX_Control(pDX, IDC_WebWidthEditBox, m_splitWebWidthEditBox);
   DDX_Control(pDX, IDC_WebAngleEditBox, m_splitWebAngleEditBox);
   DDX_Control(pDX, IDC_SplitGridRadio, m_splitGridRadioButton);
   DDX_Control(pDX, IDC_SplitStripeRadio, m_splitStripeRadioButton);

   DDX_Control(pDX, IDC_ExteriorCornerRadiusGroup, m_exteriorCornerRadiusGroup);
   DDX_Control(pDX, IDC_ExteriorCornerRadiusStatic, m_exteriorCornerRadiusStatic);
   DDX_Control(pDX, IDC_ExteriorCornerRadiusEditBox, m_exteriorCornerRadiusEditBox);

   if (pDX->m_bSaveAndValidate)
   {
      if (m_selectedStencilSubPin != NULL && isCurrentRuleValid() && isRuleAssigned())
      {
         StencilRuleTag ruleTag = m_stencilRuleComboBox.getSelected();
         CString ruleValue      = m_stencilRuleValueComboBox.getSelected();
         CString comment;
         m_commentEditBox.GetWindowText(comment);

         CStencilHole& stencilHole = m_selectedStencilSubPin->getStencilHole(m_selectStencilPinTopFlag);

         CStencilRuleModifierValues modifierValues;

         CStencilRule stencilRule;
         stencilRule.setRule(ruleTag);
         stencilRule.setRuleValue(ruleValue);
         stencilRule.setComment(comment);
         //stencilRule.addModifier(stencilRuleModifier,modifierValues);

         if (ruleTag != stencilRuleInheritFromParent)
         {
            // symmetry axis modifier
            if (m_flippedCheckBox.GetCheck() == 1)
            {
               modifierValues.empty();
               modifierValues.add(QFlipped);

               stencilRule.addModifier(stencilRuleModifierFlipSymmetryAxis,modifierValues);
            }

            // orientation modifier
            if (getOrientation() != orientationNormal)
            {
               modifierValues.empty();
               modifierValues.add(stencilOrientationModifierToString(getOrientation()));

               stencilRule.addModifier(stencilRuleModifierOrientSymmetryAxis,modifierValues);
            }

            // offset modifier
            if (getOffsetOrientation() != orientationNormal ||
               atof(m_xOffsetEditBox.getStringValue()) != 0. ||
               atof(m_yOffsetEditBox.getStringValue()) != 0. ||
               atof(m_angleOffsetEditBox.getStringValue()) != 0. )
            {
               modifierValues.empty();
               modifierValues.add(m_xOffsetEditBox.getStringValue(),pageUnits);
               modifierValues.add(m_yOffsetEditBox.getStringValue(),pageUnits);
               modifierValues.add(m_angleOffsetEditBox.getStringValue());
               modifierValues.add(stencilOrientationModifierToString(getOffsetOrientation()));

               stencilRule.addModifier(stencilRuleModifierOffsetRelativeToPadstack,modifierValues);
            }

            // split modifier
            bool stripeFlag = (m_splitStripeRadioButton.GetCheck() > 0);

            double defaultMaxFeatureSize = getStencilGenerator().getStencilGenerationParameters().getMaxFeatureSize(m_selectStencilPinTopFlag);
            double defaultWebSize        = getStencilGenerator().getStencilGenerationParameters().getWebSize(m_selectStencilPinTopFlag);
            double defaultAngle          = 0.;

            if (!fpeq(m_splitMaxFeatureEditBox.getDoubleValue(defaultMaxFeatureSize),defaultMaxFeatureSize) ||
                !fpeq(m_splitWebWidthEditBox.getDoubleValue(defaultWebSize)         ,defaultWebSize)        ||
                !fpeq(m_splitWebAngleEditBox.getDoubleValue(0.)                     ,defaultAngle)          || stripeFlag)
            {
               modifierValues.empty();
               modifierValues.add(m_splitMaxFeatureEditBox.getDoubleStringValue(defaultMaxFeatureSize),pageUnits);
               modifierValues.add(m_splitWebWidthEditBox.getDoubleStringValue(defaultWebSize)  ,pageUnits);
               modifierValues.add(m_splitWebAngleEditBox.getDoubleStringValue(defaultAngle));
               modifierValues.add(stencilSplitTypeToString(getSplitType()));

               stencilRule.addModifier(stencilRuleModifierWebSettings,modifierValues);
            }

            // exterior corner radius modifier
            if (!fpeq(m_exteriorCornerRadiusEditBox.getDoubleValue(),getStencilGenerator().getStencilGenerationParameters().getExteriorCornerRadius()))
            {
               modifierValues.empty();
               modifierValues.add(m_exteriorCornerRadiusEditBox.getStringValue(),pageUnits);

               stencilRule.addModifier(stencilRuleModifierExteriorCornerRadius,modifierValues);
            }
         }

         CString stencilRuleString = stencilRule.getRuleString();

         stencilHole.setStencilRuleForLevel(m_selectedStencilPinAttributeSource,stencilRule);

         // debug
         //CString debugRuleString = stencilRule.getRuleString();

         if (stencilRule.isValid())
         {
            CTreeListAttributeValueComboBox::addAttributeValue(ruleTag,stencilRule.getRuleValue());
         }

         //Attrib* attribute = stencilHole.setAttribute(m_selectedStencilPinAttributeSource,stencilAttribute,ruleValue);

         //if (attribute != NULL)
         //{
         //   int keywordIndex = CStencilPin::getKeywordIndex(stencilAttribute);

         //   CString attributeDisplayValue =
         //      CStencilPin::getStencilAttributeStringValue(m_selectedStencilPin->getCamCadDatabase(),
         //      keywordIndex,attribute);

         //   CTreeListAttributeValueComboBox::addAttributeValue(stencilAttribute,attributeDisplayValue);
         //}

         //stencilHole.setModifierAttribute(m_selectedStencilPinAttributeSource,stencilRuleModifier,
         //   m_xOffsetEditBox.getStringValue(),
         //   m_yOffsetEditBox.getStringValue(),
         //   m_angleOffsetEditBox.getStringValue());
      }
   }

   // Case dts0100495755 - stop opening HELP all the time : getParent()->SendMessage(DM_SETDEFID,IDHELP,0);

   m_updatingDataFlag = false;
}

bool stencilAttributeRuleHasValue(StencilRuleTag ruleTag)
{
   bool retval = false;

   switch (ruleTag)
   {
   case stencilRuleUndefined:           retval = false;  break;
   case stencilRuleNone:                retval = false;  break;
   case stencilRuleStencilSourceLayer:  retval = false;  break;
   case stencilRulePassthru:            retval = false;  break;
   case stencilRuleCopperSourceLayer:   retval = false;  break;
   case stencilRuleArea:                retval = true;   break;
   case stencilRuleInset:               retval = true;   break;
   case stencilRuleAperture:            retval = true;   break;
   case stencilRuleApertureGeometry:    retval = true;   break;
   case stencilRuleInheritFromParent:   retval = false;  break;
   }

   return retval;
}

bool stencilAttributeRuleHasModifier(StencilRuleTag ruleTag)
{
   bool retval = false;

   switch (ruleTag)
   {
   case stencilRuleUndefined:           retval = false;  break;
   case stencilRuleNone:                retval = false;  break;
   case stencilRuleStencilSourceLayer:  retval = false;  break;
   case stencilRulePassthru:            retval = false;  break;
   case stencilRuleCopperSourceLayer:   retval = false;  break;
   case stencilRuleArea:                retval = true;   break;
   case stencilRuleInset:               retval = true;   break;
   case stencilRuleAperture:            retval = true;   break;
   case stencilRuleApertureGeometry:    retval = true;   break;
   case stencilRuleInheritFromParent:   retval = false;  break;
   }

   return retval;
}

void showWindowText(CWnd* labelWnd=NULL,const char* label=NULL,CWnd* valueWnd=NULL,const char* value=NULL)
{
   bool labelFlag = false;

   if (labelWnd != NULL)
   {
      if (label != NULL && strlen(label) > 0)
      {
         labelWnd->SetWindowText(label);
         labelWnd->ShowWindow(SW_SHOW);
         labelFlag = true;
      }
      else
      {
         labelWnd->ShowWindow(SW_HIDE);
      }
   }

   if (valueWnd != NULL)
   {
      if (value != NULL && strlen(value) > 0 && labelFlag)
      {
         valueWnd->SetWindowText(value);
         valueWnd->ShowWindow(SW_SHOW);
      }
      else
      {
         valueWnd->ShowWindow(SW_HIDE);
      }
   }
}

COLORREF CStencilRulePreviewPage::getAssignedInheritedColor(RuleAssignmentTypeTag ruleAssignmentType)
{
   COLORREF color = colorBlack;

   switch (ruleAssignmentType)
   {
   case ruleAssignmentTypeImplicitlyInherited:
      color = getInheritedColor();
      break;
   case ruleAssignmentTypeExplicitlyInherited:
      color = getExplicitlyInheritedColor();
      break;
   case ruleAssignmentTypeAssigned:
      color = getAssignedColor();
      break;
   }

   return color;
}

void CStencilRulePreviewPage::updateStencilPinAttributeDataControls()
{
   bool selectedStencilPinFlag = (m_selectedStencilSubPin != NULL);

   int showCmd = (selectedStencilPinFlag ? SW_SHOW : SW_HIDE);
   int showCmdShapeRuleControls            = showCmd;
   int showCmdSymmetryAxisControls         = showCmd;
   int showCmdOrientationControls          = showCmd;
   int showCmdOffsetControls               = showCmd;
   int showCmdSplitControls                = showCmd;
   int showCmdExteriorCornerRadiusControls = showCmd;

   COLORREF backgroundColor;

   //m_assignedFlag = false;
   setRuleAssignmentType(ruleAssignmentTypeImplicitlyInherited);

   if (selectedStencilPinFlag)
   {
      CStencilHole& stencilHole = m_selectedStencilSubPin->getStencilHole(m_selectStencilPinTopFlag);
      stencilHole.updateStencilHoleAttribute();

      CStencilRuleAttributes stencilRuleAttributes = stencilHole.getStencilRuleAttributes(m_selectedStencilPinAttributeSource);
      const CStencilRule& assignedStencilRule = stencilRuleAttributes.getRule();

      CString assignedStencilRuleString = assignedStencilRule.getRuleString();

      CStencilRule inheritedStencilRule;
      AttributeSourceTag inheritedSource = stencilHole.getStencilRuleForSourceLevel(m_selectedStencilPinAttributeSource,inheritedStencilRule);

      //m_assignedFlag = (inheritedSource == m_selectedStencilPinAttributeSource);
      RuleAssignmentTypeTag ruleAssignmentTypeTag = ruleAssignmentTypeImplicitlyInherited;

      if (assignedStencilRule.getRule() == stencilRuleInheritFromParent)
      {
         ruleAssignmentTypeTag = ruleAssignmentTypeExplicitlyInherited;
      }
      else if (inheritedSource == m_selectedStencilPinAttributeSource)
      {
         ruleAssignmentTypeTag = ruleAssignmentTypeAssigned;
      }

      setRuleAssignmentType(ruleAssignmentTypeTag);

      const CStencilRule& displayedStencilRule = (isRuleAssigned() ? assignedStencilRule : inheritedStencilRule);
      StencilRuleTag ruleTag = displayedStencilRule.getRule();
      CString attributeValue = displayedStencilRule.getRuleValue();
      CStencilRuleModifier* stencilRuleModifier;
      //displayedStencilRule.hasModifier()

      // shape rule controls
      m_stencilRuleGroup.setBoxColor(getAssignedInheritedColor(getRuleAssignmentType()));
      COLORREF assignShapeRuleButtonBackgroundColor  = getButtonColor();
      COLORREF inheritShapeRuleButtonBackgroundColor = getButtonColor();

      switch (getRuleAssignmentType())
      {
      case ruleAssignmentTypeImplicitlyInherited:
         inheritShapeRuleButtonBackgroundColor = getInheritedColor();
         break;
      case ruleAssignmentTypeExplicitlyInherited:
         inheritShapeRuleButtonBackgroundColor = getExplicitlyInheritedColor();
         break;
      case ruleAssignmentTypeAssigned:
         assignShapeRuleButtonBackgroundColor = getAssignedColor();
         break;
      }

      m_assignShapeRuleButton.setTextBackgroundColor(assignShapeRuleButtonBackgroundColor);
      m_inheritShapeRuleButton.setTextBackgroundColor(inheritShapeRuleButtonBackgroundColor);

      m_stencilRuleComboBox.setSelected(ruleTag);
      m_commentEditBox.SetWindowText(displayedStencilRule.getComment());

      AttributeSourceTag ancestorSource = stencilHole.getStencilRuleForSourceLevel(parentAttributeSource(m_selectedStencilPinAttributeSource),inheritedStencilRule);
      CString ancestor = attributeSourceToString(ancestorSource);

      if (isRuleAssigned())
      {
         m_assignShapeRuleButton.SetWindowText("Assigned");
         m_inheritShapeRuleButton.SetWindowText("Inherit from: " + ancestor);
         m_inheritedRuleEditBox.SetWindowText("Assigned Rule: " + displayedStencilRule.getRuleString());
      }
      else
      {
         m_assignShapeRuleButton.SetWindowText("Assign");
         m_inheritShapeRuleButton.SetWindowText("Inherited from: " + ancestor);
         m_inheritedRuleEditBox.SetWindowText("Inherited Rule: " + displayedStencilRule.getRuleString());
      }

      // controls
      bool ruleAllowsModifierFlag;
      bool ruleHasModifierFlag;

      // symmetry axis controls  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
      stencilRuleModifier    = displayedStencilRule.getModifier(stencilRuleModifierFlipSymmetryAxis);
      ruleAllowsModifierFlag = ruleAllowsModifier(displayedStencilRule.getRule(),stencilRuleModifierFlipSymmetryAxis);
      ruleHasModifierFlag    = (stencilRuleModifier != NULL);
      showCmdSymmetryAxisControls = (ruleAllowsModifierFlag ? SW_SHOW : SW_HIDE);

      m_symmetryAxisGroup.setBoxColor(getRuleAssignmentTypeColor());
      //m_assignSymmetryAxisButton.setTextBackgroundColor(getRuleAssignmentTypeColor());

      if (ruleAllowsModifierFlag && ruleHasModifierFlag)
      {
         m_flippedCheckBox.SetCheck(1);
      }
      else
      {
         m_flippedCheckBox.SetCheck(0);
      }

      // orientation controls  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
      stencilRuleModifier    = displayedStencilRule.getModifier(stencilRuleModifierOrientSymmetryAxis);
      ruleAllowsModifierFlag = ruleAllowsModifier(displayedStencilRule.getRule(),stencilRuleModifierOrientSymmetryAxis);
      ruleHasModifierFlag    = (stencilRuleModifier != NULL);
      showCmdOrientationControls = (ruleAllowsModifierFlag ? SW_SHOW : SW_HIDE);

      m_orientationGroup.setBoxColor(getRuleAssignmentTypeColor());
      //m_assignOrientationButton.setTextBackgroundColor(getRuleAssignmentTypeColor());

      if (ruleAllowsModifierFlag && ruleHasModifierFlag)
      {
         StencilOrientationModifierTag orientation = stringToStencilOrientationModifierTag(stencilRuleModifier->getValueAt(0));
         setOrientation(orientation);
      }
      else
      {
         setOrientation(orientationNormal);
      }

      // offset controls  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
      PageUnitsTag pageUnits = getCamCadDatabase().getPageUnits();
      stencilRuleModifier    = displayedStencilRule.getModifier(stencilRuleModifierOffsetRelativeToPadstack);
      ruleAllowsModifierFlag = ruleAllowsModifier(displayedStencilRule.getRule(),stencilRuleModifierOffsetRelativeToPadstack);
      ruleHasModifierFlag    = (stencilRuleModifier != NULL);
      showCmdOffsetControls  = (ruleAllowsModifierFlag ? SW_SHOW : SW_HIDE);

      m_offsetGroup.setBoxColor(getRuleAssignmentTypeColor());
      //m_assignOffsetButton.setTextBackgroundColor(getRuleAssignmentTypeColor());

      if (ruleAllowsModifierFlag && ruleHasModifierFlag)
      {
         m_xOffsetEditBox.setDoubleValue(stencilRuleModifier->getUnitValueAt(0,pageUnits,0.));
         m_yOffsetEditBox.setDoubleValue(stencilRuleModifier->getUnitValueAt(1,pageUnits,0.));
         m_angleOffsetEditBox.setDoubleValue(stencilRuleModifier->getValueAt(2),0.);

         StencilOrientationModifierTag orientation = stringToStencilOrientationModifierTag(stencilRuleModifier->getValueAt(3));
         setOffsetOrientation(orientation);
      }
      else
      {
         m_xOffsetEditBox.setDoubleValue(0.);
         m_yOffsetEditBox.setDoubleValue(0.);
         m_angleOffsetEditBox.setDoubleValue(0.);
         setOffsetOrientation(orientationNormal);         
      }

      // split controls  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
      CStencilGenerationParameters& stencilGenerationParameters = getStencilGenerator().getStencilGenerationParameters();
      double defaultMaxFeatureSize = stencilGenerationParameters.getMaxFeatureSize(m_selectStencilPinTopFlag);
      double defaultWebSize        = stencilGenerationParameters.getWebSize(m_selectStencilPinTopFlag);
      double defaultAngle          = 0.;

      stencilRuleModifier    = displayedStencilRule.getModifier(stencilRuleModifierWebSettings);
      ruleAllowsModifierFlag = ruleAllowsModifier(displayedStencilRule.getRule(),stencilRuleModifierWebSettings);
      ruleHasModifierFlag    = (stencilRuleModifier != NULL);
      showCmdSplitControls   = (ruleAllowsModifierFlag ? SW_SHOW : SW_HIDE);

      m_splitGroup.setBoxColor(getRuleAssignmentTypeColor());
      //m_assignSplitParametersButton.setTextBackgroundColor(getRuleAssignmentTypeColor());

      StencilSplitTypeTag splitType = splitTypeUndefined;

      if (ruleAllowsModifierFlag && ruleHasModifierFlag)
      {
         m_splitMaxFeatureEditBox.setDoubleValue(stencilRuleModifier->getUnitValueAt(0,pageUnits,defaultMaxFeatureSize));
         m_splitWebWidthEditBox.setDoubleValue(  stencilRuleModifier->getUnitValueAt(1,pageUnits,defaultWebSize       ));
         m_splitWebAngleEditBox.setDoubleValue(  stencilRuleModifier->getValueAt(2),defaultAngle);

         splitType = stringToSplitTypeTag(stencilRuleModifier->getValueAt(3));
         setSplitType(splitType);

         backgroundColor = colorHtmlPaleTurquoise;
      }
      else
      {
         CStencilGenerationParameters& stencilGenerationParameters = getStencilGenerator().getStencilGenerationParameters();
         m_splitMaxFeatureEditBox.setDoubleValue(defaultMaxFeatureSize);
         m_splitWebWidthEditBox.setDoubleValue(defaultWebSize);
         m_splitWebAngleEditBox.setDoubleValue(defaultAngle);

         splitType = splitTypeGrid;
         setSplitType(splitType);

         backgroundColor = colorWhite;
      }

      m_splitMaxFeatureEditBox.setBackGroundColor(backgroundColor);
      m_splitWebWidthEditBox.setBackGroundColor(backgroundColor);
      m_splitWebAngleEditBox.setBackGroundColor(backgroundColor);

      m_splitMaxFeatureStatic.SetWindowText(splitType == splitTypeStripe ? "Stripe\nWidth" : "Max\nFeature");

      // exterior corner radius controls  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
      double defaultExteriorCornerRadius = stencilGenerationParameters.getExteriorCornerRadius();

      stencilRuleModifier    = displayedStencilRule.getModifier(stencilRuleModifierExteriorCornerRadius);
      ruleAllowsModifierFlag = ruleAllowsModifier(displayedStencilRule.getRule(),stencilRuleModifierExteriorCornerRadius);
      ruleHasModifierFlag    = (stencilRuleModifier != NULL);
      showCmdExteriorCornerRadiusControls = (ruleAllowsModifierFlag ? SW_SHOW : SW_HIDE);

      m_exteriorCornerRadiusGroup.setBoxColor(getRuleAssignmentTypeColor());

      if (ruleAllowsModifierFlag && ruleHasModifierFlag)
      {
         m_exteriorCornerRadiusEditBox.setDoubleValue(stencilRuleModifier->getUnitValueAt(0,pageUnits,defaultExteriorCornerRadius));
         backgroundColor = colorHtmlPaleTurquoise;
      }
      else
      {
         CStencilGenerationParameters& stencilGenerationParameters = getStencilGenerator().getStencilGenerationParameters();
         m_exteriorCornerRadiusEditBox.setDoubleValue(defaultExteriorCornerRadius);
         backgroundColor = colorWhite;
      }

      m_exteriorCornerRadiusEditBox.setBackGroundColor(backgroundColor);

      //// rule  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
      m_stencilRuleValueComboBox.initialize(ruleTag);
      m_stencilRuleValueComboBox.setSelected(attributeValue);

      m_stencilRuleValueComboBox.setEnableEditing(ruleTag != stencilRuleAperture &&
                                                  ruleTag != stencilRuleApertureGeometry);

      // inherited attribute processing
      AttributeSourceTag attributeSource = stencilHole.getAttributeSource();

      stencilRuleAttributes = stencilHole.getStencilRuleAttributes(attributeSource);
      //StencilAttributeTag inheritedAttributeTag = stencilRuleTagToStencilAttribute(stencilRuleAttributes.getRule().getRule());
      CString inheritedAttributeValue = stencilRuleAttributes.getRule().getRuleValue();

   }
   else
   {
      m_stencilRuleComboBox.setSelected(stencilRuleUndefined);
   }

   // shape rule controls
   m_stencilRuleGroup.ShowWindow(showCmdShapeRuleControls);
   m_assignShapeRuleButton.ShowWindow(showCmdShapeRuleControls);
   m_inheritShapeRuleButton.ShowWindow((m_selectedStencilPinAttributeSource == attributeSourcePcb) ? SW_HIDE : showCmdShapeRuleControls);
   m_stencilRuleComboBox.ShowWindow(showCmdShapeRuleControls);
   m_stencilRuleValueComboBox.ShowWindow(showCmdShapeRuleControls);
   m_commentEditBox.ShowWindow(showCmdShapeRuleControls);

   // symmetry axis controls
   m_symmetryAxisGroup.ShowWindow(showCmdSymmetryAxisControls);
   m_flippedCheckBox.ShowWindow(showCmdSymmetryAxisControls);

   // orientation controls
   m_orientationGroup.ShowWindow(showCmdOrientationControls);
   m_orientTowardCenterRadioButton.ShowWindow(showCmdOrientationControls);
   m_orientAwayFromCenterRadioButton.ShowWindow(showCmdOrientationControls);
   m_orientNormalRadioButton.ShowWindow(showCmdOrientationControls);

   // offset controls
   m_offsetGroup.ShowWindow(showCmdOffsetControls);
   m_offsetTowardCenterRadioButton.ShowWindow(showCmdOffsetControls);
   m_offsetAwayFromCenterRadioButton.ShowWindow(showCmdOffsetControls);
   m_offsetNormalRadioButton.ShowWindow(showCmdOffsetControls);
   m_xOffsetEditBox.ShowWindow(showCmdOffsetControls);
   m_yOffsetEditBox.ShowWindow(showCmdOffsetControls);
   m_angleOffsetEditBox.ShowWindow(showCmdOffsetControls);
   m_xOffsetStatic.ShowWindow(showCmdOffsetControls);
   m_yOffsetStatic.ShowWindow(showCmdOffsetControls);
   m_angleOffsetStatic.ShowWindow(showCmdOffsetControls);

   // split controls
   m_splitGroup.ShowWindow(showCmdSplitControls);
   m_splitGridRadioButton.ShowWindow(showCmdSplitControls);
   m_splitStripeRadioButton.ShowWindow(showCmdSplitControls);
   m_splitMaxFeatureEditBox.ShowWindow(showCmdSplitControls);
   m_splitWebWidthEditBox.ShowWindow(showCmdSplitControls);
   m_splitWebAngleEditBox.ShowWindow(showCmdSplitControls);
   m_splitMaxFeatureStatic.ShowWindow(showCmdSplitControls);
   m_splitWebWidthStatic.ShowWindow(showCmdSplitControls);
   m_splitWebAngleStatic.ShowWindow(showCmdSplitControls);

   // exterior corner radius controls
   m_exteriorCornerRadiusGroup.ShowWindow(showCmdExteriorCornerRadiusControls);
   m_exteriorCornerRadiusStatic.ShowWindow(showCmdExteriorCornerRadiusControls);
   m_exteriorCornerRadiusEditBox.ShowWindow(showCmdExteriorCornerRadiusControls);

   // other controls
   m_inheritedRuleEditBox.ShowWindow(showCmd);
   m_geometryNameStatic.ShowWindow(showCmd);
}

bool CStencilRulePreviewPage::isRuleInherited() const
{
   bool retval = (m_ruleAssignmentType == ruleAssignmentTypeImplicitlyInherited ||
                  m_ruleAssignmentType == ruleAssignmentTypeExplicitlyInherited    );

   return retval;
}

bool CStencilRulePreviewPage::isRuleImplicitlyInherited() const
{
   bool retval = (m_ruleAssignmentType == ruleAssignmentTypeImplicitlyInherited);

   return retval;
}

bool CStencilRulePreviewPage::isRuleExplicitlyInherited() const
{
   bool retval = (m_ruleAssignmentType == ruleAssignmentTypeExplicitlyInherited);

   return retval;
}

bool CStencilRulePreviewPage::isRuleAssigned() const
{
   bool retval = (m_ruleAssignmentType == ruleAssignmentTypeAssigned);

   return retval;
}

RuleAssignmentTypeTag CStencilRulePreviewPage::getRuleAssignmentType() const
{
   return m_ruleAssignmentType;
}

void CStencilRulePreviewPage::setRuleAssignmentType(RuleAssignmentTypeTag ruleAssignmentType)
{
   m_ruleAssignmentType = ruleAssignmentType;
}

COLORREF CStencilRulePreviewPage::getRuleAssignmentTypeColor() const
{
   COLORREF ruleAssignmentTypeColor = colorBlack;

   switch (getRuleAssignmentType())
   {
   case ruleAssignmentTypeImplicitlyInherited:  ruleAssignmentTypeColor = getInheritedColor();            break;
   case ruleAssignmentTypeExplicitlyInherited:  ruleAssignmentTypeColor = getExplicitlyInheritedColor();  break;
   case ruleAssignmentTypeAssigned:             ruleAssignmentTypeColor = getAssignedColor();             break;
   }

   return ruleAssignmentTypeColor;
}

StencilOrientationModifierTag CStencilRulePreviewPage::getOrientation() const
{
   StencilOrientationModifierTag orientation = orientationUndefined;

   if (m_orientTowardCenterRadioButton.GetCheck() == 1)
   {
      orientation = orientationTowardCenter;
   }
   else if (m_orientAwayFromCenterRadioButton.GetCheck() == 1)
   {
      orientation = orientationAwayFromCenter;
   }
   else if (m_orientNormalRadioButton.GetCheck() == 1)
   {
      orientation = orientationNormal;
   }

   return orientation;
}

void CStencilRulePreviewPage::setOrientation(StencilOrientationModifierTag orientation)
{
   // #define BST_UNCHECKED      0x0000
   // #define BST_CHECKED        0x0001

   if (orientation != orientationTowardCenter &&
       orientation != orientationAwayFromCenter  )
   {
      orientation = orientationNormal;
   }

   if ((m_orientTowardCenterRadioButton.GetCheck() == 1) !=
       (orientation == orientationTowardCenter)               )
   {
      m_orientTowardCenterRadioButton.SetCheck(orientation == orientationTowardCenter);
   }

   if ((m_orientAwayFromCenterRadioButton.GetCheck() == 1) !=
       (orientation == orientationAwayFromCenter)               )
   {
      m_orientAwayFromCenterRadioButton.SetCheck(orientation == orientationAwayFromCenter);
   }

   if ((m_orientNormalRadioButton.GetCheck() == 1) !=
       (orientation == orientationNormal)               )
   {
      m_orientNormalRadioButton.SetCheck(orientation == orientationNormal);
   }
}

StencilOrientationModifierTag CStencilRulePreviewPage::getOffsetOrientation() const
{
   StencilOrientationModifierTag orientation = orientationUndefined;

   if (m_offsetTowardCenterRadioButton.GetCheck() == 1)
   {
      orientation = orientationTowardCenter;
   }
   else if (m_offsetAwayFromCenterRadioButton.GetCheck() == 1)
   {
      orientation = orientationAwayFromCenter;
   }
   else if (m_offsetNormalRadioButton.GetCheck() == 1)
   {
      orientation = orientationNormal;
   }

   return orientation;
}

void CStencilRulePreviewPage::setOffsetOrientation(StencilOrientationModifierTag orientation)
{
   if (orientation != orientationTowardCenter &&
       orientation != orientationAwayFromCenter  )
   {
      orientation = orientationNormal;
   }

   if ((m_offsetTowardCenterRadioButton.GetCheck() == 1) !=
       (orientation == orientationTowardCenter)               )
   {
      m_offsetTowardCenterRadioButton.SetCheck(orientation == orientationTowardCenter);
   }

   if ((m_offsetAwayFromCenterRadioButton.GetCheck() == 1) !=
       (orientation == orientationAwayFromCenter)               )
   {
      m_offsetAwayFromCenterRadioButton.SetCheck(orientation == orientationAwayFromCenter);
   }

   if ((m_offsetNormalRadioButton.GetCheck() == 1) !=
       (orientation == orientationNormal)               )
   {
      m_offsetNormalRadioButton.SetCheck(orientation == orientationNormal);
   }
}

StencilSplitTypeTag CStencilRulePreviewPage::getSplitType() const
{
   StencilSplitTypeTag splitType = splitTypeGrid;

   if (m_splitGridRadioButton.GetCheck() == 1)
   {
      splitType = splitTypeGrid;
   }
   else if (m_splitStripeRadioButton.GetCheck() == 1)
   {
      splitType = splitTypeStripe;
   }

   return splitType;
}

void CStencilRulePreviewPage::setSplitType(StencilSplitTypeTag splitType)
{
   if (splitType != splitTypeStripe)
   {
      splitType = splitTypeGrid;
   }

   if ((m_splitGridRadioButton.GetCheck() == 1) !=
       (splitType == splitTypeGrid)               )
   {
      m_splitGridRadioButton.SetCheck(splitType == splitTypeGrid);
   }

   if ((m_splitStripeRadioButton.GetCheck() == 1) !=
       (splitType == splitTypeStripe)               )
   {
      m_splitStripeRadioButton.SetCheck(splitType == splitTypeStripe);
   }
}

void CStencilRulePreviewPage::fillTree()
{
}

StencilRuleStatusTag CStencilRulePreviewPage::setComponentViewGeometry(BlockStruct* componentViewGeometry,
   CStencilSubPin& sourceStencilSubPin,bool sourceTopFlag,bool placedTopFlag,AttributeSourceTag attributeSource)
{
   StencilRuleStatusTag retval = ruleStatusOk;

   CStencilGenerator& stencilGenerator = getParent()->getStencilGenerator();
   FileStruct* fileStruct = stencilGenerator.getFileStruct();

   if (componentViewGeometry != NULL && fileStruct != NULL)
   {
      CStencilGeometryMicrocosm stencilGeometryMicrocosm(stencilGenerator);
      stencilGeometryMicrocosm.setGeometry(*componentViewGeometry,*fileStruct,
         sourceStencilSubPin,sourceTopFlag,attributeSource);

      m_geometryViewStatic.setGeometryMicrocosm(stencilGeometryMicrocosm,placedTopFlag);
      retval = stencilGeometryMicrocosm.getRuleStatus();
      m_apertureViewStatic.setGeometryMicrocosm(stencilGeometryMicrocosm,placedTopFlag);
   }
   else
   {
      m_geometryViewStatic.empty();
      m_apertureViewStatic.empty();
   }

   return retval;
}

StencilRuleStatusTag CStencilRulePreviewPage::setComponentViewComponent(DataStruct* componentViewComponent,
   CStencilSubPin& sourceStencilSubPin,bool sourceTopFlag,bool placedTopFlag,AttributeSourceTag attributeSource)
{
   StencilRuleStatusTag retval = ruleStatusOk;

   CStencilGenerator& stencilGenerator = getParent()->getStencilGenerator();
   FileStruct* fileStruct = stencilGenerator.getFileStruct();

   if (componentViewComponent != NULL && fileStruct != NULL)
   {
      CStencilGeometryMicrocosm stencilGeometryMicrocosm(stencilGenerator);
      stencilGeometryMicrocosm.setComponent(*componentViewComponent,*fileStruct,
         sourceStencilSubPin,sourceTopFlag,attributeSource);

      m_geometryViewStatic.setGeometryMicrocosm(stencilGeometryMicrocosm,placedTopFlag);
      retval = stencilGeometryMicrocosm.getRuleStatus();
      m_apertureViewStatic.setGeometryMicrocosm(stencilGeometryMicrocosm,placedTopFlag);
   }
   else
   {
      m_geometryViewStatic.empty();
      m_apertureViewStatic.empty();
   }

   return retval;
}

void CStencilRulePreviewPage::setStandardApertureShape(const CStandardApertureShape& apertureShape)
{
   delete m_standardApertureShape;

   m_standardApertureShape = CStandardApertureShape::create(apertureShape);
   m_standardApertureShape->makeValid();
   m_standardApertureShape->storeToApertureParameters(m_parameters);

   //m_apertureViewStatic.setStandardApertureShape(*m_standardApertureShape);
   //m_standardApertureShape->addPointAnnotation(*(m_apertureViewStatic.getCamCadDatabase()),0.,0.);

   //if (getDisplayCenterAnnotations())
   //{
   //   m_standardApertureShape->addCentroidAnnotations(*(m_apertureViewStatic.getCamCadDatabase()));
   //}

   m_descriptorEditBox.SetWindowText(m_standardApertureShape->getDescriptor());
   m_apertureUnitsStatic.SetWindowText("Aperture Units: " + m_standardApertureShape->getDescriptorDimensionUnitsString());

   CString descriptorPrefix = standardApertureTypeTagToApertureNameDescriptor(m_standardApertureShape->getType());
   m_apertureTypeComboBox.SelectString(0,descriptorPrefix);

   m_apertureViewStatic.setStandardApertureShape(*m_standardApertureShape);
   m_apertureViewStatic.calcScaling(1.2);
}

void CStencilRulePreviewPage::setGenericStandardApertureShape(const CStandardApertureShape& apertureShape)
{
   delete m_genericStandardApertureShape;

   m_genericStandardApertureShape = CStandardApertureShape::create(apertureShape);

   m_genericApertureViewStatic.setStandardApertureShape(*m_genericStandardApertureShape);

   CCamCadData& camCadDataDataBase = m_genericApertureViewStatic.getCamCadDatabase()->getCamCadData();

   m_genericStandardApertureShape->addDiagramAnnotations(camCadDataDataBase);

   if (getDisplayCenterAnnotations())
   {
      m_genericStandardApertureShape->addCentroidAnnotations(camCadDataDataBase);
   }

   m_genericDescriptorStatic.SetWindowText(m_parameters.getDescriptorTemplate());
}

void CStencilRulePreviewPage::setApertureType(StandardApertureTypeTag apertureType)
{
   m_parameters.setDescriptorPrefix(standardApertureTypeTagToDescriptorPrefix(apertureType));

   PageUnitsTag pageUnits = getPageUnits();
   int exponent = CStandardApertureShape::getDefaultDescriptorDimensionFactorExponent(pageUnits);

   CStandardApertureShape* standardApertureShape = CStandardApertureShape::create(apertureType,pageUnits,exponent);

   if (m_apertureViewStatic.getCopperExtent().isValid())
   {
      standardApertureShape->setFromExtent(m_apertureViewStatic.getCopperExtent());
   }

   setStandardApertureShape(*standardApertureShape);
   delete standardApertureShape;

   CStandardApertureShape* genericStandardApertureShape = CStandardApertureShape::create(apertureType,pageUnits,exponent);
   setGenericStandardApertureShape(*genericStandardApertureShape);
   delete genericStandardApertureShape;

}

StandardApertureTypeTag CStencilRulePreviewPage::setApertureTypeFromDescriptor(const CString& descriptor)
{
   CStandardApertureShape* standardApertureShape =
      CStandardApertureShape::create(descriptor,CStandardApertureShape::getDefaultPageUnits());

   StandardApertureTypeTag type = standardApertureShape->getType();
   setApertureType(type);

   delete standardApertureShape;

   return type;
}

bool CStencilRulePreviewPage::setApertureFromDescriptor(const CString& descriptor)
{
   CStandardApertureShape* standardApertureShape =
      CStandardApertureShape::create(descriptor,CStandardApertureShape::getDefaultPageUnits());

   standardApertureShape->makeValid();

   setApertureType(standardApertureShape->getType());
   setStandardApertureShape(*standardApertureShape);

   bool retval = standardApertureShape->isValid();

   delete standardApertureShape;

   return retval;
}

const CStandardApertureShape& CStencilRulePreviewPage::getStandardApertureShape()
{
   if (m_standardApertureShape == NULL)
   {
      setApertureType(standardApertureCircle);
   }

   return *m_standardApertureShape;
}

CString CStencilRulePreviewPage::getApertureDescriptor() const
{
   CString descriptor;

   if (m_standardApertureShape != NULL && m_standardApertureShape->isValid())
   {
      descriptor = m_standardApertureShape->getDescriptor();
   }

   return descriptor;
}

//void CStencilRulePreviewPage::drawApertureView()
//{
//   //if (m_camCadDatabase != NULL)
//   //{
//   //   //m_camCadDatabase->getCamCadDoc().dra
//   //}
//}

void CStencilRulePreviewPage::updateParameters()
{
   m_standardApertureShape->loadFromApertureParameters(m_parameters);
   //m_standardApertureShape->storeToApertureParameters(m_parameters);

   //m_descriptorEditBox.SetWindowText(m_standardApertureShape->getDescriptor());

   m_stencilRuleValueComboBox.setSelected(m_standardApertureShape->getDescriptor());

   UpdateData(true);
   setStencilRuleChanged();
   UpdateData(false);
}

bool CStencilRulePreviewPage::isCurrentRuleValid()
{
   bool retval = false;

   if (m_selectedStencilSubPin != NULL)
   {
      StencilRuleTag ruleTag = m_stencilRuleComboBox.getSelected();
      CString ruleValue      = m_stencilRuleValueComboBox.getSelected();

      switch (ruleTag)
      {
      case stencilRuleUndefined:
      case stencilRuleNone:
      case stencilRuleStencilSourceLayer:
      case stencilRuleCopperSourceLayer:
      case stencilRuleInheritFromParent:
         retval = true;
         break;
      case stencilRuleArea:
      case stencilRuleInset:
         if (!ruleValue.IsEmpty())
         {
            char* endChar;
            strtod(ruleValue,&endChar);

            retval = (*endChar == '\0');
         }

         break;
      case stencilRuleAperture:
         if (!ruleValue.IsEmpty())
         {
            CStandardApertureShape* standardApertureShape = CStandardApertureShape::create(ruleValue);

            retval = (isStandardApertureShapeSupportedForStencils(standardApertureShape->getType()) &&
                      standardApertureShape->isValid());

            delete standardApertureShape;
         }

         break;
      case stencilRuleApertureGeometry:
         if (!ruleValue.IsEmpty())
         {
            BlockStruct* apertureGeometry = getCamCadDatabase().getBlock(ruleValue);

            retval = (apertureGeometry != NULL && apertureGeometry->isAperture());
         }
      case stencilRulePassthru:
         AttributeSourceTag srctag = m_selectedStencilPinAttributeSource;
         if (!ruleValue.IsEmpty())
         {
            BlockStruct* apertureGeometry = getCamCadDatabase().getBlock(ruleValue);

            //*rcf need to check what the rule is on?  Only on geometries is valid, not on padstacks, not on pcb, etc.
            retval = true;//(apertureGeometry != NULL && apertureGeometry->isAperture());
         }
         retval = (m_selectedStencilPinAttributeSource == attributeSourceGeometry);

         break;
      }
   }

   return retval;
}

//bool CStencilRulePreviewPage::updateStencilHoleIfCurrentRuleValid()
//{
//   bool retval = isCurrentRuleValid();
//
//   if (retval)
//   {
//      StencilAttributeTag stencilAttribute       = m_stencilRuleComboBox.getSelected();
//      CString ruleValue                          = m_stencilRuleValueComboBox.getSelected();
//      StencilRuleModifierTag stencilRuleModifier = m_stencilRuleModifierComboBox.getSelected();
//
//      CStencilHole& stencilHole = m_selectedStencilPin->getStencilHole(m_selectStencilPinTopFlag);
//
//      stencilHole.setAttribute(m_selectedStencilPinAttributeSource,stencilAttribute,ruleValue);
//
//      stencilHole.setModifierAttribute(m_selectedStencilPinAttributeSource,
//         stencilAttributeApertureModifier,
//         stencilRuleModifierToString(stencilRuleModifier));
//
//      UpdateData(false);
//      updateGraphicViews();
//   }
//
//   return retval;
//}

//void CStencilRulePreviewPage::updateGraphicViews()
//{
//   HTREEITEM treeItem = getTreeListControl().GetSelectedItem();
//
//   if (treeItem != NULL)
//   {
//      selectStencilPin(treeItem);
//   }
//}

//void CStencilRulePreviewPage::selectStencilPin()
//{
//   HTREEITEM treeItem = getTreeListControl().GetSelectedItem();
//
//   selectStencilPin(treeItem);
//}

//void CStencilRulePreviewPage::selectStencilPin(HTREEITEM treeItem)
//{
//   m_enableGeometryViewFlag = false;
//
//   if (treeItem != NULL)
//   {
//      m_selectedStencilPin = (CStencilPin*)this->getTreeListControl().GetItemData(treeItem);
//      CStencilTreeListItem* treeListItem  = (CStencilTreeListItem*)this->getTreeListControl().GetTreeItemData(treeItem);
//      m_selectStencilPinTopFlag           = treeListItem->getTopFlag();
//      m_selectedStencilPinAttributeSource = treeListItem->getAttributeSource();
//      CString description,geometryHeading,derivationMethod;
//
//      CCamCadDatabase& camCadDatabase = getParent()->getCamCadDatabase();
//
//      if (m_selectedStencilPin != NULL)
//      {
//         m_enableGeometryViewFlag = true;
//
//         DataStruct& component = m_selectedStencilPin->getComponent();
//         BlockStruct* componentGeometry = camCadDatabase.getBlock(component.getInsert()->getBlockNumber());
//
//         CStencilHole& stencilHole = m_selectedStencilPin->getStencilHole(m_selectStencilPinTopFlag);
//         stencilHole.updateStencilHoleAttribute();
//         derivationMethod = stencilHole.getDerivationMethodForSourceLevel(m_selectedStencilPinAttributeSource);
//
//         CString attributeName     = stencilHole.getStencilAttributeName();
//         CString attributeValue    = stencilHole.getStencilAttributeStringValue();
//         CString itemDescription   = treeListItem->GetItemString();
//         geometryHeading           = "Geometry: " + componentGeometry->getName();
//
//         switch (m_selectedStencilPinAttributeSource)
//         {
//         case attributeSourceGeom:
//            setComponentViewGeometry(componentGeometry,*m_selectedStencilPin,m_selectStencilPinTopFlag,
//               m_selectedStencilPinAttributeSource);
//            description = "Geometry: " + itemDescription;
//            break;
//         case attributeSourcePad:
//            setComponentViewGeometry(componentGeometry,*m_selectedStencilPin,m_selectStencilPinTopFlag,
//               m_selectedStencilPinAttributeSource);
//            description = "Padstack: " + itemDescription;
//            break;
//         case attributeSourceComp:
//            setComponentViewComponent(&component,*m_selectedStencilPin,m_selectStencilPinTopFlag,
//               m_selectedStencilPinAttributeSource);
//            description = "Component: " + itemDescription;
//            break;
//         case attributeSourcePin:
//            setComponentViewGeometry(componentGeometry,*m_selectedStencilPin,m_selectStencilPinTopFlag,
//               m_selectedStencilPinAttributeSource);
//            description = "Geometry Pin: " + componentGeometry->getName() + "." + itemDescription;
//            break;
//         case attributeSourcePinInstance:
//            setComponentViewComponent(&component,*m_selectedStencilPin,m_selectStencilPinTopFlag,
//               m_selectedStencilPinAttributeSource);
//            description = "Physical Pin: " + m_selectedStencilPin->getPinRef();
//            break;
//         default:
//            setComponentViewComponent(NULL,*m_selectedStencilPin,m_selectStencilPinTopFlag,
//               m_selectedStencilPinAttributeSource);
//            description = itemDescription;
//            geometryHeading.Empty();
//            m_enableGeometryViewFlag = false;
//            break;
//         }
//      }
//      else
//      {
//         derivationMethod = "None";
//      }
//
//      m_selectedEntityStatic.SetWindowText(description);
//      m_geometryNameStatic.SetWindowText(geometryHeading);
//      m_inheritedRuleEditBox.SetWindowText("Derivation Method: " + derivationMethod);
//   }
//
//   updateStencilPinAttributeDataControls();
//
//   UpdateData(false);
//}

BEGIN_MESSAGE_MAP(CStencilRulePreviewPage, CResizingPropertyPage)

   ON_BN_CLICKED(IDC_EXPANDALL, OnBnClickedExpandAll)
   ON_BN_CLICKED(IDC_COLLAPSEALL, OnBnClickedCollapseAll)
   ON_BN_CLICKED(IDC_ShowRules, OnBnClickedShowRules)
   ON_BN_CLICKED(IDC_RemoveAllRules, OnBnClickedRemoveAllRules)

// standard aperture
   ON_CBN_EDITCHANGE(IDC_ApertureType, OnEditChangeApertureTypeComboBox)
   ON_CBN_SELCHANGE(IDC_ApertureType, OnSelChangeApertureTypeComboBox)
   ON_EN_KILLFOCUS(IDC_ParameterValue0, OnEnKillFocusParameterValue0)
   ON_EN_KILLFOCUS(IDC_ParameterValue1, OnEnKillFocusParameterValue1)
   ON_EN_KILLFOCUS(IDC_ParameterValue2, OnEnKillFocusParameterValue2)
   ON_EN_KILLFOCUS(IDC_ParameterValue3, OnEnKillFocusParameterValue3)
   ON_EN_KILLFOCUS(IDC_ParameterValue4, OnEnKillFocusParameterValue4)
   //ON_EN_KILLFOCUS(IDC_ParameterValue5, OnEnKillFocusParameterValue5)
   //ON_EN_KILLFOCUS(IDC_Descriptor, OnEnKillFocusDescriptor)
   ON_BN_CLICKED(IDC_DisplayCenterAnnotations, OnBnClickedDisplayCenterAnnotations)
   ON_CBN_SELCHANGE(IDC_OriginOptions, OnCbnSelchangeOriginOptions)

   ON_CBN_CLOSEUP(IDC_StencilRules, OnCloseupStencilRulesComboBox)
   ON_CBN_CLOSEUP(IDC_StencilRuleValues, OnCloseupStencilRuleValuesComboBox)
   ON_CBN_CLOSEUP(IDC_StencilRuleModifiers, OnCloseupStencilRuleModifiersComboBox)
   ON_WM_SETFOCUS()
   ON_CBN_KILLFOCUS(IDC_StencilRuleValues, OnKillFocusStencilRuleValuesComboBox)
   ON_WM_KEYUP()
   ON_WM_NCACTIVATE()
   ON_WM_ACTIVATE()
   ON_WM_KEYDOWN()
   ON_BN_CLICKED(IDC_APPLY,  OnBnClickedApply)
   ON_BN_CLICKED(IDC_APPLY2, OnBnClickedApply)
   ON_WM_SIZE()

   ON_MESSAGE(WM_StencilPinChanged,OnStencilPinChanged)
   ON_CBN_DROPDOWN(IDC_StencilRules, OnDropdownStencilRulesComboBox)
   ON_BN_CLICKED(IDC_SwapWidthAndHeight, OnBnClickedSwapWidthWithHeight)
   ON_BN_CLICKED(IDC_GeometryPlacedTopBottom, OnBnClickedGeometryPlacedTopBottom)
   ON_BN_CLICKED(IDC_OrientTowardCenterRadio, OnBnClickedOrientTowardCenter)
   ON_BN_CLICKED(IDC_OrientAwayFromCenterRadio, OnBnClickedOrientAwayFromCenter)
   ON_BN_CLICKED(IDC_OrientNormalRadio, OnBnClickedOrientNormal)
   ON_BN_CLICKED(IDC_OffsetTowardCenterRadio, OnBnClickedOffsetTowardCenter)
   ON_BN_CLICKED(IDC_OffsetAwayFromCenterRadio, OnBnClickedOffsetAwayFromCenter)
   ON_BN_CLICKED(IDC_OffsetNormalRadio, OnBnClickedOffsetNormal)
   ON_BN_CLICKED(IDC_FlippedCheckBox, OnBnClickedFlippedCheckBox)
   ON_BN_CLICKED(IDC_InheritShapeRule, OnBnClickedInheritShapeRule)
   ON_BN_CLICKED(IDC_AssignShapeRule, OnBnClickedAssignShapeRule)
   ON_BN_CLICKED(IDC_SplitGridRadio, OnBnClickedSplitGrid)
   ON_BN_CLICKED(IDC_SplitStripeRadio, OnBnClickedSplitStripe)

   ON_EN_KILLFOCUS(IDC_CommentEditBox, OnKillFocusCommentEditBox)
   ON_EN_KILLFOCUS(IDC_XOffsetEditBox, OnKillFocusXoffsetEditBox)
   ON_EN_KILLFOCUS(IDC_YOffsetEditBox, OnKillFocusYoffsetEditBox)
   ON_EN_KILLFOCUS(IDC_AngleOffsetEditBox, OnKillFocusAngleOffsetEditBox)
   ON_EN_KILLFOCUS(IDC_MaxFeatureEditBox, OnKillFocusMaxFeatureEditBox)
   ON_EN_KILLFOCUS(IDC_WebWidthEditBox, OnKillFocusWebWidthEditBox)
   ON_EN_KILLFOCUS(IDC_ExteriorCornerRadiusEditBox, OnKillFocusExteriorCornerRadiusEditBox)
   ON_EN_KILLFOCUS(IDC_WebAngleEditBox, OnKillFocusWebAngleEditBox)

   ON_EN_SETFOCUS(IDC_CommentEditBox, OnSetFocusCommentEditBox)
   ON_EN_SETFOCUS(IDC_XOffsetEditBox, OnSetFocusXoffsetEditBox)
   ON_EN_SETFOCUS(IDC_YOffsetEditBox, OnSetFocusYoffsetEditBox)
   ON_EN_SETFOCUS(IDC_AngleOffsetEditBox, OnSetFocusAngleOffsetEditBox)
   ON_EN_SETFOCUS(IDC_MaxFeatureEditBox, OnSetFocusMaxFeatureEditBox)
   ON_EN_SETFOCUS(IDC_WebWidthEditBox, OnSetFocusWebWidthEditBox)
   ON_EN_SETFOCUS(IDC_ExteriorCornerRadiusEditBox, OnSetFocusExteriorCornerRadiusEditBox)
   ON_EN_SETFOCUS(IDC_WebAngleEditBox, OnSetFocusWebAngleEditBox)
END_MESSAGE_MAP()


// CStencilRulePreviewPage message handlers

BOOL CStencilRulePreviewPage::OnInitDialog()
{
#ifdef STENCIL_TIMER
   GetStencilTimer().AddMessage("enter CStencilRulePreviewPage::OnInitDialog()");
#endif

   CResizingPropertyPage::OnInitDialog();

   //m_selectedEntityStatic.SetWindowText("");
   m_geometryNameStatic.SetWindowText("");
   m_inheritedRuleEditBox.SetWindowText("");

   m_treeListFrame.SubclassDlgItem(IDC_TreeListCtrl,this);
   //m_treeListFrame.ModifyStyleEx(WS_EX_TOPMOST,WS_EX_CLIENTEDGE);
   m_treeListFrame.getTree().setComboBoxAt(ColumnRule        ,new CTreeListStencilRuleComboBox(ColumnRule));
   m_treeListFrame.getTree().setComboBoxAt(ColumnRuleValue   ,new CTreeListStencilRuleValueComboBox(ColumnRuleValue));
   m_treeListFrame.getTree().setComboBoxAt(ColumnRuleModifier,new CTreeListStencilRuleModifierComboBox(ColumnRuleModifier));

   getParent()->initializeAttributeValues();

   fillTree();

   //UpdateData(false);

   // standard aperture
   CString descriptorPrefix;
   int selectedIndex = 0;

   for (int index = standardApertureFirstValid;index < standardApertureInvalid;index++)
   {
      StandardApertureTypeTag apertureType = intToStandardApertureTypeTag(index);
      descriptorPrefix = standardApertureTypeTagToApertureNameDescriptor(apertureType);

      if (isStandardApertureShapeSupportedForStencils(apertureType))
      {
         int itemIndex = m_apertureTypeComboBox.AddString(descriptorPrefix);
         m_apertureTypeComboBox.SetItemData(itemIndex,index);

         if (apertureType == m_initialApertureType)
         {
            selectedIndex = index;
         }
      }
   }

   // aperture geometries
   //CMapStringToAperture& apertureMap = getParent()->getApertureMap();

   //for (POSITION pos = apertureMap.GetStartPosition();pos != NULL;)
   //{
   //   BlockStruct* aperture;
   //   CString key;
   //   apertureMap.GetNextAssoc(pos,key,aperture);

   //   m_apertureGeometryListBox.AddString(key);
   //}

   ::SetWindowLong(m_genericApertureViewStatic.m_hWnd,GWL_STYLE,
      m_genericApertureViewStatic.GetStyle() | SS_OWNERDRAW);

   ::SetWindowLong(m_apertureViewStatic.m_hWnd,GWL_STYLE,
      m_apertureViewStatic.GetStyle() | SS_OWNERDRAW);

   //if (!setApertureFromDescriptor(m_initialDescriptor))
   //{
   //   m_apertureTypeComboBox.SetCurSel(selectedIndex);
   //   setApertureType(m_initialApertureType);
   //}

   // stencil rules
   m_stencilRuleComboBox.initialize();

   //this->selectStencilPin();
   //updateStencilPinAttributeDataControls();
   UpdateData(false);

#ifdef STENCIL_TIMER
   GetStencilTimer().AddMessage("exit  CStencilRulePreviewPage::OnInitDialog()");
#endif

   return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CStencilRulePreviewPage::OnSetActive()
{
   BOOL retval = CResizingPropertyPage::OnSetActive();
   // Case dts0100495755 - stop opening HELP all the time : getParent()->SendMessage(DM_SETDEFID,IDHELP,0);

   UpdateData(false);

   return retval;
}

afx_msg void CStencilRulePreviewPage::OnSize(UINT nType, int cx, int cy)
{
   CResizingPropertyPage::OnSize(nType,cx,cy);
}

void CStencilRulePreviewPage::OnBnClickedExpandAll()
{
   getTreeListControl().expandAll();
   //selectStencilPin();
   UpdateData(false);
}

void CStencilRulePreviewPage::OnBnClickedCollapseAll()
{
   getTreeListControl().collapseLowerHierarchies();
   //selectStencilPin();
   UpdateData(false);
}

void CStencilRulePreviewPage::OnBnClickedShowRules()
{
   getTreeListControl().expandToShowRules();
   //selectStencilPin();
   UpdateData(false);
}

void CStencilRulePreviewPage::OnBnClickedRemoveAllRules()
{
   getParent()->clearStencilSettings();
   //selectStencilPin();
   UpdateData(false);
}

void CStencilRulePreviewPage::OnEditChangeApertureTypeComboBox()
{
   // TODO: Add your control notification handler code here
}

void CStencilRulePreviewPage::OnSelChangeApertureTypeComboBox()
{
   int selectedItem = m_apertureTypeComboBox.GetCurSel();

   if (selectedItem != CB_ERR)
   {
      DWORD_PTR itemData = m_apertureTypeComboBox.GetItemData(selectedItem);
      setApertureType(intToStandardApertureTypeTag(itemData));

      //drawApertureView();
      updateParameters();
   }
}

void CStencilRulePreviewPage::OnEnKillFocusParameterValue0()
{
   updateParameters();
}

void CStencilRulePreviewPage::OnEnKillFocusParameterValue1()
{
   updateParameters();
}

void CStencilRulePreviewPage::OnEnKillFocusParameterValue2()
{
   updateParameters();
}

void CStencilRulePreviewPage::OnEnKillFocusParameterValue3()
{
   updateParameters();
}

void CStencilRulePreviewPage::OnEnKillFocusParameterValue4()
{
   updateParameters();
}

//void CStencilRulePreviewPage::OnEnKillFocusParameterValue5()
//{
//   updateParameters();
//}

//void CStencilRulePreviewPage::OnEnKillFocusDescriptor()
//{
//   CString descriptor;
//   m_descriptorEditBox.GetWindowText(descriptor);
//   //setApertureFromDescriptor(descriptor);
//
//   updateParameters();
//}

void CStencilRulePreviewPage::OnBnClickedDisplayCenterAnnotations()
{
   //CStandardApertureShape* genericStandardApertureShape =  CStandardApertureShape::create(*m_genericStandardApertureShape);
   //setGenericStandardApertureShape(*genericStandardApertureShape);
   //delete genericStandardApertureShape;

   CStandardApertureShape* standardApertureShape = CStandardApertureShape::create(*m_standardApertureShape);
   setStandardApertureShape(*standardApertureShape);
   delete standardApertureShape;

   updateParameters();
}

void CStencilRulePreviewPage::OnBnClickedApply()
{
   UpdateData(true);
   m_selectedTreeItem = NULL;
   UpdateData(false);

   if (this->getStencilGenerator().getStencilGenerationParameters().getPropagateRulesSideToSide())
      this->propagateCurrentRuleToOppositeSide();
}

void CStencilRulePreviewPage::OnBnClickedSwapWidthWithHeight()
{
   CStandardApertureDialogParameter* widthParameter  = m_parameters.lookupCaptionPrefix("w");
   CStandardApertureDialogParameter* heightParameter = m_parameters.lookupCaptionPrefix("h");

   if (widthParameter != NULL && heightParameter != NULL)
   {
      CString width  = widthParameter->getValue();
      CString height = heightParameter->getValue();

      widthParameter->setValue(height);
      heightParameter->setValue(width);

      updateParameters();
   }
}

void CStencilRulePreviewPage::OnBnClickedGeometryPlacedTopBottom()
{
   UpdateData(true);

   m_geometryPlacedTopFlag = !m_geometryPlacedTopFlag;

   setStencilRuleChanged();

   UpdateData(false);
}

void CStencilRulePreviewPage::OnCbnSelchangeOriginOptions()
{
   updateParameters();
}

void CStencilRulePreviewPage::OnCloseupStencilRulesComboBox()
{
   CString selectedText;
   int selectionIndex = m_stencilRuleComboBox.GetCurSel();

   if (selectionIndex != m_stencilRuleComboBoxSelectedIndex)
   {
      if (selectionIndex != CB_ERR)
      {
         StencilRuleTag ruleTag = intToStencilRuleTag(m_stencilRuleComboBox.GetItemData(selectionIndex));
         m_stencilRuleValueComboBox.initialize(ruleTag);
         //m_stencilRuleModifierComboBox.initialize(ruleTag);
      }
   }

   onStencilControlChanged();
}

void CStencilRulePreviewPage::OnDropdownStencilRulesComboBox()
{
   m_stencilRuleComboBoxSelectedIndex = m_stencilRuleComboBox.GetCurSel();
}

void CStencilRulePreviewPage::OnCloseupStencilRuleValuesComboBox()
{
   //UpdateData(true);

   //if (m_selectedStencilPin != NULL)
   //{
   //   CString attributeValue = m_stencilRuleValueComboBox.getSelected();
   //   StencilAttributeTag attributeTag = m_stencilRuleComboBox.getSelected();

      //if (attributeTag == stencilAttributeAperture)
      //{
      //   if (attributeValue.CompareNoCase(QSelectAperture) == 0)
      //   {
      //      CStandardApertureDialog standardApertureDialog(getActiveView()->GetDocument()->getPageUnits());
      //      standardApertureDialog.setInitialDescriptor(m_stencilRuleValueComboBox.getCurrentEditBoxText());

      //      if (standardApertureDialog.DoModal() == IDOK)
      //      {
      //         attributeValue = standardApertureDialog.getApertureDescriptor();
      //      }
      //      else
      //      {
      //         attributeValue.Empty();
      //      }
      //   }
      //}
      //else
      //if (attributeTag == stencilAttributeApertureGeometry)
      //{
      //   //if (attributeValue.CompareNoCase(QSelectApertureGeometry) == 0)
      //   //{
      //   //   CApertureGeometryDialog apertureGeometryDialog;

      //   //   if (apertureGeometryDialog.DoModal() == IDOK)
      //   //   {
      //   //      attributeValue = apertureGeometryDialog.getApertureName();
      //   //   }
      //   //   else
      //   //   {
      //   //      attributeValue.Empty();
      //   //   }
      //   //}
      //}
      //else
      //if (!attributeValue.IsEmpty())
      //{
      //   CStencilHole& stencilHole = m_selectedStencilPin->getStencilHole(m_selectStencilPinTopFlag);
      //   Attrib* attribute = stencilHole.setAttribute(m_selectedStencilPinAttributeSource,attributeTag,attributeValue);

      //   if (attribute != NULL)
      //   {
      //      int keywordIndex = CStencilPin::getKeywordIndex(attributeTag);

      //      CString attributeDisplayValue =
      //         CStencilPin::getStencilAttributeStringValue(m_selectedStencilPin->getCamCadDatabase(),
      //            keywordIndex,attribute);

      //      CTreeListAttributeValueComboBox::addAttributeValue(attributeTag,attributeDisplayValue);
      //   }
      //}

      //updateStencilHoleIfCurrentRuleValid();
   //}

   onStencilControlChanged();
}

void CStencilRulePreviewPage::invalidateSelectedTreeItem()
{
   if (m_selectedTreeItem != NULL)
   {
      getTreeListControl().invalidateItem(m_selectedTreeItem);
   }
}

void CStencilRulePreviewPage::setStencilRuleChanged()
{
   m_stencilRuleChangedFlag = true;

   invalidateSelectedTreeItem();
}

void CStencilRulePreviewPage::onStencilControlChanged()
{
   //m_assignedFlag = true;
   setRuleAssignmentType(ruleAssignmentTypeAssigned);

   UpdateData(true);
   setStencilRuleChanged();
   UpdateData(false);

   invalidateSelectedTreeItem();

   if (this->getStencilGenerator().getStencilGenerationParameters().getPropagateRulesSideToSide())
      this->propagateCurrentRuleToOppositeSide();
}

void CStencilRulePreviewPage::propagateCurrentRuleToOppositeSide()
{
   // Find the current item and propagate its rule settings to the same item on opposite side

   if (m_selectedTreeItem != NULL)
   {
      CStencilSubPin* selectedStencilSubPin  = ((m_selectedTreeItem != NULL) ? getTreeListControl().getSubPinItemData(m_selectedTreeItem) : NULL);

      HTREEITEM oldItem    = m_selectedTreeItem;

      bool stencilPinSelectedFlag = (selectedStencilSubPin != NULL);

      if (stencilPinSelectedFlag)
      {
         CStencilTreeListItem* treeListItem  = (CStencilTreeListItem*)this->getTreeListControl().GetTreeItemData(m_selectedTreeItem);

         //CString itemString( treeListItem->GetItemString() );
         bool topFlag( treeListItem->getTopFlag() );

         CString itemText( treeListItem->GetItemText() );
         //CString subString( treeListItem->GetSubstring() );

         AttributeSourceTag itemSource = treeListItem->getAttributeSource();

         RuleAssignmentTypeTag ruleAssignmentTypeTag = ruleAssignmentTypeUndefined;

         // Determine ruleAssignmentType to propagate
         if (m_selectedStencilSubPin != NULL)
         {
            CStencilHole& stencilHole = m_selectedStencilSubPin->getStencilHole(m_selectStencilPinTopFlag);
            stencilHole.updateStencilHoleAttribute();

            CStencilRuleAttributes stencilRuleAttributes = stencilHole.getStencilRuleAttributes(m_selectedStencilPinAttributeSource);
            const CStencilRule& assignedStencilRule = stencilRuleAttributes.getRule();

            CString assignedStencilRuleString = assignedStencilRule.getRuleString();

            CStencilRule inheritedStencilRule;
            AttributeSourceTag inheritedSource = stencilHole.getStencilRuleForSourceLevel(m_selectedStencilPinAttributeSource,inheritedStencilRule);

            ruleAssignmentTypeTag = ruleAssignmentTypeImplicitlyInherited;

            if (assignedStencilRule.getRule() == stencilRuleInheritFromParent)
            {
               ruleAssignmentTypeTag = ruleAssignmentTypeExplicitlyInherited;
            }
            else if (inheritedSource == m_selectedStencilPinAttributeSource)
            {
               ruleAssignmentTypeTag = ruleAssignmentTypeAssigned;
            }
         }

         DataStruct &compData = selectedStencilSubPin->getComponent();
         int insertedBlkNum = -1;
         if (compData.getDataType() == dataTypeInsert)
            insertedBlkNum = compData.getInsert()->getBlockNumber();
         BlockStruct *insertedBlk = NULL;
         if (insertedBlkNum >= 0)
            insertedBlk = this->getCamCadDatabase().getBlock(insertedBlkNum);
         CString insertedBlkName(insertedBlk != NULL ? insertedBlk->getName() : "unknown");

         // With the latest change, ie dropping of side-specific support, I think these few lines
         // below here are all we really need. If you diff with past revs you will see a lot of code
         // use to be below here to do the propagation, it is all gone now, we just need to force the
         // tree view to update the other side.
         // I think a lot of the code above here is not necessary anymore either, but I am leaving
         // it in place today because things are working as-is, and DC is, well, two days ago.

         // Note that we want the item on the OPPOSITE surface
         HTREEITEM foundItem = findTreeItem(itemSource, !topFlag, itemText, insertedBlkName, CString(""), CString(""));

         if (foundItem && ruleAssignmentTypeTag != ruleAssignmentTypeUndefined)
         {
            //CStencilTreeListItem* oppSideTreeListItem  = (CStencilTreeListItem*)this->getTreeListControl().GetTreeItemData(foundItem);
            this->getTreeListControl().invalidateItem(foundItem);
         }
      }
   }
}

HTREEITEM CStencilRulePreviewPage::findTreeItem(HTREEITEM rootItem, AttributeSourceTag itemSource, bool topFlag, CString& itemText, CString& compGeomName, CString& pinGeomName, CString& pinName)
{
   // Support function, don't call this one directly elsewhere in app, only from top level findTreeItem().

   if (!itemText.IsEmpty())
   {
      while (rootItem != NULL)
      {

         CStencilTreeListItem* treeListItem  = (CStencilTreeListItem*)this->getTreeListControl().GetTreeItemData(rootItem);

         bool treeItemTopFlag( treeListItem->getTopFlag() );

         //if (treeItemTopFlag == topFlag)
         {
            ///CStencilSubPin* selectedStencilSubPin  = ((m_selectedTreeItem != NULL) ? getTreeListControl().getSubPinItemData(m_selectedTreeItem) : NULL);

            CString treeItemString( treeListItem->GetItemString() );
            CString treeItemText( treeListItem->GetItemText() );
            AttributeSourceTag treeItemSource = treeListItem->getAttributeSource();

            if (treeItemSource == itemSource)
            {
               // int jjj = 0;
            }

            if (treeItemText.Compare(itemText) == 0 && treeItemSource == itemSource && treeItemTopFlag == topFlag)
            {
               return rootItem;
            }
            else
            {
               //*rcf Can add opt here by checking current itemSource and comparing against auxiliary data
               // i.e. don't continue down branch if not appropriate

               HTREEITEM childItem = this->getTreeListControl().GetChildItem(rootItem);

               HTREEITEM ffii = findTreeItem(childItem, itemSource, topFlag, itemText, compGeomName, pinGeomName, pinName);
               if (ffii != NULL)
                  return ffii;
            }
         }

         rootItem = this->getTreeListControl().GetNextSiblingItem(rootItem);
      }
   }

   return NULL;
}

HTREEITEM CStencilRulePreviewPage::findTreeItem(AttributeSourceTag itemSource, bool topFlag, CString& itemText, CString& compGeomName, CString& pinGeomName, CString& pinName)
{
   // Top level, this is the one to call.

   HTREEITEM selectedItem = NULL;

   if (!itemText.IsEmpty())
   {
      HTREEITEM rootItem = this->getTreeListControl().GetRootItem();

      selectedItem = findTreeItem(rootItem, itemSource, topFlag, itemText, compGeomName, pinGeomName, pinName);
   }


   return selectedItem;
}

void CStencilRulePreviewPage::OnKillFocusStencilRuleValuesComboBox()
{
   //*rcf Questionable to not check this, but it causes propagation problems
   //if (m_stencilRuleValueComboBox.getEditFlag())
   {
      UpdateData(true);
      setStencilRuleChanged();
      UpdateData(false);

      if (this->getStencilGenerator().getStencilGenerationParameters().getPropagateRulesSideToSide())
         this->propagateCurrentRuleToOppositeSide();
   }
}

CString CStencilRulePreviewPage::getSavedEditBoxContents() const
{
   return m_savedEditBoxContents;
}

void CStencilRulePreviewPage::setSavedEditBoxContents(const CString& contentsString)
{
   m_savedEditBoxContents = contentsString;
}

// Kill Focus handlers
void CStencilRulePreviewPage::onKillFocusStencilControlEditBox(CEdit& editBox)
{
   CString contentsString;
   editBox.GetWindowText(contentsString);

   bool itemLockedFlag = getItemLocked();
   HTREEITEM lockSelectedItem = getLockSelectedItem();

   CString savedContentsString = getSavedEditBoxContents();
   
   if (contentsString != savedContentsString)
   {
      onStencilControlChanged();
   }

   setItemLocked(false);

   if (itemLockedFlag)
   {
      if (lockSelectedItem != NULL)
      {
         getTreeListControl().SelectItem(lockSelectedItem);
      }

      UpdateData(false);
   }
}

void CStencilRulePreviewPage::OnKillFocusXoffsetEditBox()
{
   onKillFocusStencilControlEditBox(m_xOffsetEditBox);
}

void CStencilRulePreviewPage::OnKillFocusYoffsetEditBox()
{
   onKillFocusStencilControlEditBox(m_yOffsetEditBox);
}

void CStencilRulePreviewPage::OnKillFocusAngleOffsetEditBox()
{
   onKillFocusStencilControlEditBox(m_angleOffsetEditBox);
}

void CStencilRulePreviewPage::OnKillFocusMaxFeatureEditBox()
{
   onKillFocusStencilControlEditBox(m_splitMaxFeatureEditBox);
}

void CStencilRulePreviewPage::OnKillFocusWebWidthEditBox()
{
   onKillFocusStencilControlEditBox(m_splitWebWidthEditBox);
}

void CStencilRulePreviewPage::OnKillFocusWebAngleEditBox()
{
   onKillFocusStencilControlEditBox(m_splitWebAngleEditBox);
}

void CStencilRulePreviewPage::OnKillFocusExteriorCornerRadiusEditBox()
{
   onKillFocusStencilControlEditBox(m_exteriorCornerRadiusEditBox);
}

void CStencilRulePreviewPage::OnKillFocusCommentEditBox()
{
   onKillFocusStencilControlEditBox(m_commentEditBox);
}

// Set Focus handlers
void CStencilRulePreviewPage::onSetFocusStencilControlEditBox(CEdit& editBox)
{
   CString contentsString;
   editBox.GetWindowText(contentsString);

   setSavedEditBoxContents(contentsString);
   setItemLocked(true);
}

void CStencilRulePreviewPage::OnSetFocusXoffsetEditBox()
{
   onSetFocusStencilControlEditBox(m_xOffsetEditBox);
}

void CStencilRulePreviewPage::OnSetFocusYoffsetEditBox()
{
   onSetFocusStencilControlEditBox(m_yOffsetEditBox);
}

void CStencilRulePreviewPage::OnSetFocusAngleOffsetEditBox()
{
   onSetFocusStencilControlEditBox(m_angleOffsetEditBox);
}

void CStencilRulePreviewPage::OnSetFocusMaxFeatureEditBox()
{
   onSetFocusStencilControlEditBox(m_splitMaxFeatureEditBox);
}

void CStencilRulePreviewPage::OnSetFocusWebWidthEditBox()
{
   onSetFocusStencilControlEditBox(m_splitWebWidthEditBox);
}

void CStencilRulePreviewPage::OnSetFocusWebAngleEditBox()
{
   onSetFocusStencilControlEditBox(m_splitWebAngleEditBox);
}

void CStencilRulePreviewPage::OnSetFocusExteriorCornerRadiusEditBox()
{
   onSetFocusStencilControlEditBox(m_exteriorCornerRadiusEditBox);
}

void CStencilRulePreviewPage::OnSetFocusCommentEditBox()
{
   onSetFocusStencilControlEditBox(m_commentEditBox);
}

//
void CStencilRulePreviewPage::OnBnClickedSplitGrid()
{
   setSplitType(splitTypeGrid);

   onStencilControlChanged();
}

void CStencilRulePreviewPage::OnBnClickedSplitStripe()
{
   setSplitType(splitTypeStripe);

   onStencilControlChanged();
}

void CStencilRulePreviewPage::OnBnClickedOrientTowardCenter()
{
   setOrientation(orientationTowardCenter);

   onStencilControlChanged();
}

void CStencilRulePreviewPage::OnBnClickedOrientAwayFromCenter()
{
   setOrientation(orientationAwayFromCenter);

   onStencilControlChanged();
}

void CStencilRulePreviewPage::OnBnClickedOrientNormal()
{
   setOrientation(orientationNormal);

   onStencilControlChanged();
}

void CStencilRulePreviewPage::OnBnClickedOffsetTowardCenter()
{
   setOffsetOrientation(orientationTowardCenter);

   onStencilControlChanged();
}

void CStencilRulePreviewPage::OnBnClickedOffsetAwayFromCenter()
{
   setOffsetOrientation(orientationAwayFromCenter);

   onStencilControlChanged();
}

void CStencilRulePreviewPage::OnBnClickedOffsetNormal()
{
   setOffsetOrientation(orientationNormal);

   onStencilControlChanged();
}

void CStencilRulePreviewPage::OnBnClickedFlippedCheckBox()
{
   onStencilControlChanged();
}

void CStencilRulePreviewPage::OnBnClickedInheritShapeRule()
{
   if (m_selectedStencilSubPin != NULL)
   {
      CStencilHole& stencilHole = m_selectedStencilSubPin->getStencilHole(m_selectStencilPinTopFlag);
      CStencilRule stencilRule;
      stencilHole.setStencilRuleForLevel(m_selectedStencilPinAttributeSource,stencilRule);

      //m_assignedFlag = false;
      setRuleAssignmentType(ruleAssignmentTypeImplicitlyInherited);

      setStencilRuleChanged();

      UpdateData(false);

      if (this->getStencilGenerator().getStencilGenerationParameters().getPropagateRulesSideToSide())
         this->propagateCurrentRuleToOppositeSide();
   }
}

void CStencilRulePreviewPage::OnBnClickedAssignShapeRule()
{
   onStencilControlChanged();
}

void CStencilRulePreviewPage::OnCloseupStencilRuleModifiersComboBox()
{
   UpdateData(true);
   setStencilRuleChanged();
   UpdateData(false);
}

afx_msg LRESULT CStencilRulePreviewPage::OnStencilPinChanged(WPARAM wParem,LPARAM lParam)
{
   setStencilRuleChanged();
   UpdateData(false);
   UpdateData(true);

   return 0;
}

static int selectIconIndex(bool hasAttributeFlag,bool hasExplicitInheritFlag,int iconIndex,int iconIndexA,int iconIndexB)
{
   if (hasAttributeFlag)
   {
      iconIndex = (hasExplicitInheritFlag ? iconIndexB : iconIndexA);
   }

   return iconIndex;
}

int CStencilRulePreviewPage::getIconIndexForSource(AttributeSourceTag attributeSource,bool viaFlag,bool fiducialFlag,int typeIndex)
{
   int iconIndex = 0;

   if (typeIndex == 0)
   {
      switch (attributeSource)
      {
      case attributeSourcePcb:              iconIndex = getTreeListControl().getPcbIconIndex();             break;
      case attributeSourceSurface:          iconIndex = getTreeListControl().getSurfaceIconIndex();         break;
      case attributeSourceMount:            iconIndex = getTreeListControl().getSmdThIconIndex();           break;
      case attributeSourcePadstack:         iconIndex = getTreeListControl().getPadIconIndex();             break;
      case attributeSourceSubPadstack:      iconIndex = getTreeListControl().getSubPadIconIndex();          break;
      case attributeSourceGeometryPin:      iconIndex = getTreeListControl().getPinIconIndex();             break;
      case attributeSourceComponentPin:     iconIndex = getTreeListControl().getPinInstanceIconIndex();     break;
      case attributeSourceComponentSubPin:  iconIndex = getTreeListControl().getSubPinInstanceIconIndex();  break;
      case attributeSourceGeometry:
         if (viaFlag)
         {
            iconIndex = getTreeListControl().getViaIconIndex();
         }
         else if (fiducialFlag)
         {
            iconIndex = getTreeListControl().getFiducialIconIndex();
         }
         else
         {
            iconIndex = getTreeListControl().getGeomIconIndex();
         }

         break;
      case attributeSourceComponent:
         if (viaFlag)
         {
            iconIndex = getTreeListControl().getViaIconIndex();
         }
         else if (fiducialFlag)
         {
            iconIndex = getTreeListControl().getFiducialIconIndex();
         }
         else
         {
            iconIndex = getTreeListControl().getCompIconIndex();
         }

         break;
      default:
         break;
      }
   }
   else if (typeIndex == 1)
   {
      switch (attributeSource)
      {
      case attributeSourcePcb:              iconIndex = getTreeListControl().getPcbIconAIndex();             break;
      case attributeSourceSurface:          iconIndex = getTreeListControl().getSurfaceIconAIndex();         break;
      case attributeSourceMount:            iconIndex = getTreeListControl().getSmdThIconAIndex();           break;
      case attributeSourcePadstack:         iconIndex = getTreeListControl().getPadIconAIndex();             break;
      case attributeSourceGeometryPin:      iconIndex = getTreeListControl().getPinIconAIndex();             break;
      case attributeSourceSubPadstack:      iconIndex = getTreeListControl().getSubPadIconAIndex();          break;
      case attributeSourceComponentPin:     iconIndex = getTreeListControl().getPinInstanceIconAIndex();     break;
      case attributeSourceComponentSubPin:  iconIndex = getTreeListControl().getSubPinInstanceIconAIndex();  break;
      case attributeSourceGeometry:
         if (viaFlag)
         {
            iconIndex = getTreeListControl().getViaIconAIndex();
         }
         else if (fiducialFlag)
         {
            iconIndex = getTreeListControl().getFiducialIconAIndex();
         }
         else
         {
            iconIndex = getTreeListControl().getGeomIconAIndex();
         }

         break;
      case attributeSourceComponent:
         if (viaFlag)
         {
            iconIndex = getTreeListControl().getViaIconAIndex();
         }
         else if (fiducialFlag)
         {
            iconIndex = getTreeListControl().getFiducialIconAIndex();
         }
         else
         {
            iconIndex = getTreeListControl().getCompIconAIndex();
         }

         break;
      default:
         break;
      }
   }
   else if (typeIndex == 2)
   {
      switch (attributeSource)
      {
      case attributeSourcePcb:              iconIndex = getTreeListControl().getPcbIconBIndex();             break;
      case attributeSourceSurface:          iconIndex = getTreeListControl().getSurfaceIconBIndex();         break;
      case attributeSourceMount:            iconIndex = getTreeListControl().getSmdThIconBIndex();           break;
      case attributeSourcePadstack:         iconIndex = getTreeListControl().getPadIconBIndex();             break;
      case attributeSourceGeometryPin:      iconIndex = getTreeListControl().getPinIconBIndex();             break;
      case attributeSourceSubPadstack:      iconIndex = getTreeListControl().getSubPadIconBIndex();          break;
      case attributeSourceComponentPin:     iconIndex = getTreeListControl().getPinInstanceIconBIndex();     break;
      case attributeSourceComponentSubPin:  iconIndex = getTreeListControl().getSubPinInstanceIconBIndex();  break;
      case attributeSourceGeometry:
         if (viaFlag)
         {
            iconIndex = getTreeListControl().getViaIconBIndex();
         }
         else if (fiducialFlag)
         {
            iconIndex = getTreeListControl().getFiducialIconBIndex();
         }
         else
         {
            iconIndex = getTreeListControl().getGeomIconBIndex();
         }

         break;
      case attributeSourceComponent:
         if (viaFlag)
         {
            iconIndex = getTreeListControl().getViaIconBIndex();
         }
         else if (fiducialFlag)
         {
            iconIndex = getTreeListControl().getFiducialIconBIndex();
         }
         else
         {
            iconIndex = getTreeListControl().getCompIconBIndex();
         }

         break;
      default:
         break;
      }
   }

   return iconIndex;
}

int CStencilRulePreviewPage::getImageIndex(HTREEITEM treeItem)
{
   CStencilTreeListItem* treeListItem  = (CStencilTreeListItem*)getTreeListControl().GetTreeItemData(treeItem);
   bool topFlag  = treeListItem->getTopFlag();
   //int subPinIndex = treeListItem->getSubPinIndex();
   AttributeSourceTag attributeSource = treeListItem->getAttributeSource();

   //CStencilPin* stencilPin = (CStencilPin*)getTreeListControl().GetItemData(treeItem);
   CStencilSubPin* subPin = getTreeListControl().getSubPinItemData(treeItem);

   bool viaFlag                = false;
   bool fiducialFlag           = false;
   bool hasAttributeFlag       = false;
   bool hasExplicitInheritFlag = false;

   if (AfxIsValidAddress(subPin,sizeof(CStencilSubPin),true))
   {
      CStencilRuleAttributes stencilRuleAttributes = subPin->getStencilHole(topFlag).getStencilRuleAttributes(attributeSource);
      hasAttributeFlag = stencilRuleAttributes.hasRule();
      hasExplicitInheritFlag = (stencilRuleAttributes.getRule().getRule() == stencilRuleInheritFromParent);

      viaFlag       = getStencilGenerator().isViaGeometry(subPin->getGeometryName());
      fiducialFlag  = getStencilGenerator().isFiducialGeometry(subPin->getGeometryName());
   }

   int typeIndex  = hasAttributeFlag + hasExplicitInheritFlag;
   int imageIndex = getIconIndexForSource(attributeSource,viaFlag,fiducialFlag,typeIndex);

   return imageIndex;
}

BOOL CStencilRulePreviewPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
   static HTREEITEM prevOldItem = NULL;
   static HTREEITEM prevNewItem = NULL;

	if (wParam == ID_TREE_LIST_CTRL)
	{
	   NMTREEVIEW* pNmtv = (NMTREEVIEW*)lParam;
      bool treeViewSelectionChanged = false;
      bool itemLockedFlag = getItemLocked();

      bool treeItemSelectionChangedFlag = (pNmtv->hdr.code == TVN_SELCHANGED);
      bool treeItemItemExpandedFlag     = (pNmtv->hdr.code == TVN_ITEMEXPANDED);
      bool treeItemGetDisplayInfoFlag   = (pNmtv->hdr.code == TVN_GETDISPINFO);

      if (treeItemSelectionChangedFlag)
      {
         bool sameNotificationFlag = (pNmtv->itemOld.hItem == prevOldItem && pNmtv->itemNew.hItem == prevNewItem);
         //CWnd* wndWithFocus = GetFocus();
         //bool treeHasFocus = (&(getTreeListControl()) == wndWithFocus);

         prevOldItem = pNmtv->itemOld.hItem;
         prevNewItem = pNmtv->itemNew.hItem;

         if (!sameNotificationFlag)
         {
            if (itemLockedFlag)
            {
               setLockSelectedItem(prevNewItem);
            }
            else
            {
               setLockSelectedItem(NULL);

               m_stencilRuleValueComboBox.getSelected();
               UpdateData(true);
               UpdateData(false);
            }
         }
      }

      if (treeItemItemExpandedFlag && !itemLockedFlag)
      {
         m_stencilRuleValueComboBox.getSelected();
         UpdateData(false);
      }

      if (treeItemGetDisplayInfoFlag)
      {
         NMTVDISPINFO* pNmtvDispInfo = (NMTVDISPINFO*)lParam;
         HTREEITEM treeItem = pNmtvDispInfo->item.hItem;

         int imageIndex = getImageIndex(treeItem);

         //CStencilTreeListItem* treeListItem  = (CStencilTreeListItem*)getTreeListControl().GetTreeItemData(treeItem);
         //bool topFlag = treeListItem->getTopFlag();
         //AttributeSourceTag attributeSource = treeListItem->getAttributeSource();

         //CStencilPin* stencilPin = (CStencilPin*)getTreeListControl().GetItemData(treeItem);
         ////CString attributeName;
         //bool viaFlag          = false;
         //bool fiducialFlag     = false;
         //bool hasAttributeFlag = false;

         //if (AfxIsValidAddress(stencilPin,sizeof(CStencilPin),true))
         //{
         //   CStencilRuleAttributes stencilRuleAttributes = stencilPin->getStencilHole(topFlag).getStencilRuleAttributes(attributeSource);
         //   hasAttributeFlag = stencilRuleAttributes.hasRule();

         //   //attributeName = stencilPin->getStencilHole(topFlag).getStencilAttributeName(attributeSource);
         //   viaFlag       = getStencilGenerator().isViaGeometry(stencilPin->getGeometryName());
         //   fiducialFlag  = getStencilGenerator().isFiducialGeometry(stencilPin->getGeometryName());
         //}

         ////bool hasAttributeFlag = !attributeName.IsEmpty();
         //int imageIndex = 0;

         //switch (attributeSource)
         //{
         //case attributeSourcePcb:
         //   imageIndex = (hasAttributeFlag ? getTreeListControl().getPcbIconAIndex()     : getTreeListControl().getPcbIconIndex());
         //   break;
         //case attributeSourceSurface:
         //   imageIndex = (hasAttributeFlag ? getTreeListControl().getSurfaceIconAIndex() : getTreeListControl().getSurfaceIconIndex());
         //   break;
         //case attributeSourceMount:
         //   imageIndex = (hasAttributeFlag ? getTreeListControl().getSmdThIconAIndex()   : getTreeListControl().getSmdThIconIndex());
         //   break;
         ////case attributeSourceVia:
         ////   imageIndex = (hasAttributeFlag ? getTreeListControl().getViaIconAIndex()         : getTreeListControl().getViaIconIndex());
         ////   break;
         //case attributeSourceGeometry:
         //   if (viaFlag)
         //   {
         //      imageIndex = (hasAttributeFlag ? getTreeListControl().getViaIconAIndex()      : getTreeListControl().getViaIconIndex());
         //   }
         //   else if (fiducialFlag)
         //   {
         //      imageIndex = (hasAttributeFlag ? getTreeListControl().getFiducialIconAIndex() : getTreeListControl().getFiducialIconIndex());
         //   }
         //   else
         //   {
         //      imageIndex = (hasAttributeFlag ? getTreeListControl().getGeomIconAIndex()     : getTreeListControl().getGeomIconIndex());
         //   }

         //   break;
         //case attributeSourcePadstack:
         //   imageIndex = (hasAttributeFlag ? getTreeListControl().getPadIconAIndex() : getTreeListControl().getPadIconIndex());
         //   break;
         //case attributeSourceGeometryPin:
         //   imageIndex = (hasAttributeFlag ? getTreeListControl().getPinIconAIndex() : getTreeListControl().getPinIconIndex());
         //   break;
         //case attributeSourceComponent:
         //   if (viaFlag)
         //   {
         //      imageIndex = (hasAttributeFlag ? getTreeListControl().getViaIconAIndex()      : getTreeListControl().getViaIconIndex());
         //   }
         //   else if (fiducialFlag)
         //   {
         //      imageIndex = (hasAttributeFlag ? getTreeListControl().getFiducialIconAIndex() : getTreeListControl().getFiducialIconIndex());
         //   }
         //   else
         //   {
         //      imageIndex = (hasAttributeFlag ? getTreeListControl().getCompIconAIndex()     : getTreeListControl().getCompIconIndex());
         //   }

         //   break;
         //case attributeSourceComponentPin:
         //   imageIndex = (hasAttributeFlag ? getTreeListControl().getPinInstanceIconAIndex() : getTreeListControl().getPinInstanceIconIndex());
         //   break;
         //default:
         //   break;
         //}

         pNmtvDispInfo->item.iImage         = imageIndex;
         pNmtvDispInfo->item.iSelectedImage = imageIndex;
      }

//      if (pNmtv->hdr.code == TVN_SELCHANGED)
//      {
//         TVITEM itemOld = pNmtv->itemOld;
//         TVITEM itemNew = pNmtv->itemNew;
//         HTREEITEM htreeItemOld = itemOld.hItem;
//         HTREEITEM htreeItemNew = itemNew.hItem;
//
//         if (htreeItemOld != NULL)
//         {
//            CStencilPin* stencilPin = (CStencilPin*)this->getTreeListControl().GetItemData(htreeItemOld);
//            CString pinRef = stencilPin->getPinRef();
//            int iii = 3;
//         }
///*
//      DWORD treeItemData = GetTreeItemData(item);
//      DWORD itemData     = GetItemData(item);
//
//      if (itemData != NULL)
//      {
//         CStencilTreeListItem* treeListItem = (CStencilTreeListItem*)treeItemData;
//
//*/
//         if (htreeItemNew != NULL)
//         {
//            selectStencilPin(htreeItemNew);
//         }
//      }
   }

   return CResizingPropertyPage::OnNotify(wParam, lParam, pResult);
}

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CStencilGeometryRulePreviewPage, CStencilRulePreviewPage)


CStencilGeometryRulePreviewPage::CStencilGeometryRulePreviewPage()
{
   m_psp.pszTitle = "Geometry Rules";
}

CStencilGeometryRulePreviewPage::~CStencilGeometryRulePreviewPage()
{
}

afx_msg void CStencilGeometryRulePreviewPage::OnSize(UINT nType, int cx, int cy)
{
   CStencilRulePreviewPage::OnSize(nType,cx,cy);
}

CString cleanGeometryName(const CString& geometryName)
{
   CString cleanName(geometryName);

   if (geometryName.GetLength() > 2 &&
       geometryName.Left(1)  == "$" &&
       geometryName.Right(1) == "$")
   {
      int pos = geometryName.ReverseFind('-');

      if (pos > 0)
      {
         cleanName = geometryName.Mid(pos + 1,geometryName.GetLength() - pos - 2);
      }
   }

   return cleanName;
}

void CStencilGeometryRulePreviewPage::fillTree()
{
   m_fillingTreeFlag = true;

   //if (!m_treeInitialized)
   //{
   //   return;
   //}

   COperationProgress progress;
   CStencilTreeListCtrl& treeListControl = m_treeListFrame.getStencilTree();
   treeListControl.DeleteAllItems();

   //if (treeListControl.GetColumnsNum() < 3)
   {
      treeListControl.InsertColumn(ColumnStencilObject,QPcbSurfaceTypeGeomPad,
         LVCFMT_LEFT,getParent()->getColumnStencilObjectWidth());

      //treeListControl.InsertColumn(ColumnRule         ,QRule                      ,
      //   LVCFMT_LEFT,getParent()->getColumnRuleWidth());

      //treeListControl.InsertColumn(ColumnRuleValue    ,QRuleValue                 ,
      //   LVCFMT_LEFT,getParent()->getColumnRuleValueWidth());

      //treeListControl.InsertColumn(ColumnRuleModifier ,QRuleModifier              ,
      //   LVCFMT_LEFT,getParent()->getColumnRuleModifierWidth());
   }

   CStencilPins& stencilPins = getParent()->getStencilGenerator().getStencilPins();
   stencilPins.initializeRoot();
   CStencilSubPin* firstSubPin = (stencilPins.IsEmpty() ? NULL : stencilPins.GetHead()->getSubPin(0));

   HTREEITEM pcbItem     = treeListControl.InsertItem(QPCB,attributeSourcePcb,true,
      I_IMAGECALLBACK,I_IMAGECALLBACK);
   treeListControl.setSubPinItemData(pcbItem,firstSubPin);

   HTREEITEM topSurfaceItem     = treeListControl.InsertItem(QTopSurface,attributeSourceSurface,true,
      I_IMAGECALLBACK,I_IMAGECALLBACK,pcbItem);
   treeListControl.setSubPinItemData(topSurfaceItem,firstSubPin);

   HTREEITEM topSmdSurfaceItem  = treeListControl.InsertItem(QSmdGeometries,attributeSourceMount,true,
      I_IMAGECALLBACK,I_IMAGECALLBACK,topSurfaceItem);
   treeListControl.setSubPinItemData(topSmdSurfaceItem,NULL);

   HTREEITEM topThSurfaceItem   = treeListControl.InsertItem(QThGeometries,attributeSourceMount,true,
      I_IMAGECALLBACK,I_IMAGECALLBACK,topSurfaceItem);
   treeListControl.setSubPinItemData(topThSurfaceItem,NULL);

   HTREEITEM topViaSurfaceItem   = treeListControl.InsertItem(QViaGeometries,attributeSourceMount,true,
      I_IMAGECALLBACK,I_IMAGECALLBACK,topSurfaceItem);
   treeListControl.setSubPinItemData(topViaSurfaceItem,NULL);

   HTREEITEM topFiducialSurfaceItem   = treeListControl.InsertItem(QFiducialGeometries,attributeSourceMount,true,
      I_IMAGECALLBACK,I_IMAGECALLBACK,topSurfaceItem);
   treeListControl.setSubPinItemData(topFiducialSurfaceItem,NULL);

   HTREEITEM bottomSurfaceItem     = treeListControl.InsertItem(QBottomSurface,attributeSourceSurface,false,
      I_IMAGECALLBACK,I_IMAGECALLBACK,pcbItem);
   treeListControl.setSubPinItemData(bottomSurfaceItem,firstSubPin);

   HTREEITEM bottomSmdSurfaceItem  = treeListControl.InsertItem(QSmdGeometries,attributeSourceMount,false,
      I_IMAGECALLBACK,I_IMAGECALLBACK,bottomSurfaceItem);
   treeListControl.setSubPinItemData(bottomSmdSurfaceItem,NULL);

   HTREEITEM bottomThSurfaceItem   = treeListControl.InsertItem(QThGeometries,attributeSourceMount,false,
      I_IMAGECALLBACK,I_IMAGECALLBACK,bottomSurfaceItem);
   treeListControl.setSubPinItemData(bottomThSurfaceItem,NULL);

   HTREEITEM bottomViaSurfaceItem  = treeListControl.InsertItem(QViaGeometries,attributeSourceMount,false,
      I_IMAGECALLBACK,I_IMAGECALLBACK,bottomSurfaceItem);
   treeListControl.setSubPinItemData(bottomViaSurfaceItem,NULL);

   HTREEITEM bottomFiducialSurfaceItem  = treeListControl.InsertItem(QFiducialGeometries,attributeSourceMount,false,
      I_IMAGECALLBACK,I_IMAGECALLBACK,bottomSurfaceItem);
   treeListControl.setSubPinItemData(bottomFiducialSurfaceItem,NULL);

   CCamCadDatabase& camCadDatabase = getParent()->getCamCadDatabase();
   int mapSize = nextPrime2n(max(200 + stencilPins.GetSize(),round(1.5*stencilPins.GetSize())));
   CMapSortedStringToOb<CStencilSubPin> topSortedStencilHoles(mapSize,false);
   CMapSortedStringToOb<CStencilSubPin> bottomSortedStencilHoles(mapSize,false);
   topSortedStencilHoles.setSortFunction(sortByGeomPadPin);
   bottomSortedStencilHoles.setSortFunction(sortByGeomPadPin);

   progress.updateStatus("Scanning Pins",stencilPins.GetCount());

   for (POSITION pos = stencilPins.GetHeadPosition();pos != NULL;)
   {
      CStencilPin* stencilPin = stencilPins.GetNext(pos);
      stencilPin->updateStencilHoleAttribute();

      for (int subPinIndex = 0;subPinIndex < stencilPin->getSubPinCount();subPinIndex++)
      {
         CStencilSubPin* subPin  = stencilPin->getSubPin(subPinIndex);

         CString geometryName    = subPin->getGeometryName();
         CString padStackName    = subPin->getPadStackName();
         CString subPadStackName = subPin->getStencilHole(true ).getSubPadstackName();
         CString pinName         = subPin->getPinName();

         DataStruct* topCopperPad    = subPin->getStencilHole(true ).getCopperPad();
         DataStruct* bottomCopperPad = subPin->getStencilHole(false).getCopperPad();

         if (topCopperPad != NULL)
         {
            topSortedStencilHoles.SetAt(subPin->getSortableGeomPadstackSubpinPin(true),subPin);
         }
#ifdef SUPPORT_NONCOPPER_PADS
         else if (subPin->getStencilHole(true).getStencilSourcePad() != NULL)
         {
            topSortedStencilHoles.SetAt(subPin->getSortableGeomPadstackSubpinPin(true),subPin);
            topCopperPad = subPin->getStencilHole(true).getStencilSourcePad(); // NOT REALLY COPPER
         }
#endif

         if (bottomCopperPad != NULL)
         {
            bottomSortedStencilHoles.SetAt(subPin->getSortableGeomPadstackSubpinPin(false),subPin);
         }
      }
   }

   int totalHolesToProcess = topSortedStencilHoles.GetCount() + bottomSortedStencilHoles.GetCount();
   progress.updateStatus("Processing Tree Hierarchy",totalHolesToProcess);

   for (int surfaceIndex = 0;surfaceIndex < 2;surfaceIndex++)
   {
      bool topFlag = (surfaceIndex == 0);

      HTREEITEM surfaceItem;
      HTREEITEM smdSurfaceItem;
      HTREEITEM thSurfaceItem;
      HTREEITEM viaSurfaceItem;
      HTREEITEM fiducialSurfaceItem;
      CMapSortedStringToOb<CStencilSubPin>& sortedStencilHoles = (topFlag ? topSortedStencilHoles : bottomSortedStencilHoles);

      if (topFlag)
      {
         surfaceItem         = topSurfaceItem;
         smdSurfaceItem      = topSmdSurfaceItem;
         thSurfaceItem       = topThSurfaceItem;
         viaSurfaceItem      = topViaSurfaceItem;
         fiducialSurfaceItem = topFiducialSurfaceItem;
      }
      else
      {
         surfaceItem         = bottomSurfaceItem;
         smdSurfaceItem      = bottomSmdSurfaceItem;
         thSurfaceItem       = bottomThSurfaceItem;
         viaSurfaceItem      = bottomViaSurfaceItem;
         fiducialSurfaceItem = bottomFiducialSurfaceItem;
      }

      bool enableHidingOfRedundantSubPadstackItemFlag = true;

      int index;
      CString key;
      CString* pKey = &key;
      CStencilSubPin* subPin;
      CString previousGeometryName(" "),geometryName;
      CString previousPadStackName(" "),padStackName;
      CString previousSubPadStackName(" "),subPadStackName;
      CString pinName,padDescriptor;
      HTREEITEM geometryItem    = NULL;
      HTREEITEM padStackItem    = NULL;
      HTREEITEM subPadStackItem = NULL;
      HTREEITEM stencilHoleItem = NULL;
      HTREEITEM mountItem       = NULL;

      for (sortedStencilHoles.rewind(index);sortedStencilHoles.next(subPin,index);)
      {
         progress.incrementProgress();

         CStencilHole& stencilHole = subPin->getStencilHole(topFlag);

         CStencilPin* stencilPin = &(subPin->getStencilPin());
         geometryName    = stencilPin->getGeometryName();
         padStackName    = stencilPin->getPadStackName();
         subPadStackName = stencilHole.getSubPadstackName();
         pinName         = stencilPin->getPinName();
         int subPinCount = stencilPin->getSubPinCount();
         bool hideSubPadstackItemFlag = (enableHidingOfRedundantSubPadstackItemFlag && subPinCount < 2);

         padDescriptor = stencilHole.getCopperApertureDescriptor();

         CStencilRuleAttributes stencilRuleAttributes = stencilHole.getStencilRuleAttributes();
         CString attributeValue = stencilRuleAttributes.getRule().getRuleValue();

         //CString attributeName     = stencilHole.getStencilAttributeName();
         //CString attributeValue    = stencilHole.getStencilAttributeStringValue();
         //CString attributeModifier = stencilHole.getStencilModifierAttributeStringValue();

         DataStruct* topCopperPad    = subPin->getStencilHole(true ).getCopperPad();
         DataStruct* bottomCopperPad = subPin->getStencilHole(false).getCopperPad();

         if (getStencilGenerator().isViaGeometry(geometryName))
         {
            mountItem = viaSurfaceItem;
         }
         else if (getStencilGenerator().isFiducialGeometry(geometryName))
         {
            mountItem = fiducialSurfaceItem;
         }
         else if (getStencilGenerator().isSmdGeometry(geometryName))
         {
            mountItem = smdSurfaceItem;
         }
         else
         {
            mountItem = thSurfaceItem;
         }

         if (geometryName != previousGeometryName)
         {
            geometryItem    = NULL;
            padStackItem    = NULL;
            subPadStackItem = NULL;
         }

         if (padStackName != previousPadStackName)
         {
            padStackItem    = NULL;
            subPadStackItem = NULL;
         }

         if (subPadStackName != previousSubPadStackName)
         {
            subPadStackItem = NULL;
         }

         if (geometryItem == NULL)
         {
            geometryItem = treeListControl.InsertItem(cleanGeometryName(geometryName),attributeSourceGeometry,topFlag,
               I_IMAGECALLBACK,I_IMAGECALLBACK,mountItem);
            treeListControl.setSubPinItemData(geometryItem ,subPin);
            treeListControl.setSubPinItemData(mountItem    ,subPin);

            if (topCopperPad    != NULL &&  topFlag ||
                bottomCopperPad != NULL && !topFlag     )
            {
               CStencilRuleAttributes stencilRuleAttributes = stencilHole.getStencilRuleAttributes(attributeSourceGeometry);

               if (stencilRuleAttributes.hasNonInheritedRule())
               {
                  StencilRuleTag ruleTag = stencilRuleAttributes.getRule().getRule();
                  CTreeListAttributeValueComboBox::addAttributeValue(ruleTag,stencilRuleAttributes.getRuleValue());
               }

               //Attrib* attribute;
               //Attrib* modifierAttribute;
               //int keywordIndex;
               //int modifierKeywordIndex;

               //if (stencilHole.getStencilAttributesForSource(attributeSourceGeometry,attribute,keywordIndex,modifierAttribute,modifierKeywordIndex))
               //{
               //   const KeyWordStruct* keyword = camCadDatabase.getKeywordAt(keywordIndex);
               //   StencilAttributeTag attributeTag =
               //      attributeNameToStencilAttributeTag(keyword->getCCKeyword());
               //   CString attributeDisplayValue =
               //      CStencilPin::getStencilAttributeStringValue(camCadDatabase,keywordIndex,attribute);

               //   CTreeListAttributeValueComboBox::addAttributeValue(attributeTag,attributeDisplayValue);
               //}
            }

            previousGeometryName = geometryName;
         }

         if (geometryItem != NULL)
         {
            if (padStackItem == NULL)
            {
               CString description = padStackName + ":" + padDescriptor;
               padStackItem = treeListControl.InsertItem(description,attributeSourcePadstack,topFlag,
                  I_IMAGECALLBACK,I_IMAGECALLBACK,geometryItem);
               treeListControl.setSubPinItemData(padStackItem ,subPin);

                  previousPadStackName = padStackName;
            }

            if (padStackItem != NULL)
            {
               if (subPadStackItem == NULL)
               {
                  CString description;
                  description.Format("SubPad %d",subPin->getSubPinIndex());

                  if (hideSubPadstackItemFlag)
                  {
                     subPadStackItem = padStackItem;
                  }
                  else
                  {
                     subPadStackItem = treeListControl.InsertItem(description,attributeSourceSubPadstack,topFlag,
                                          I_IMAGECALLBACK,I_IMAGECALLBACK,padStackItem);
                     treeListControl.setSubPinItemData(subPadStackItem ,subPin);
                  }

                  previousSubPadStackName = subPadStackName;
               }

               if (subPadStackItem != NULL)
               {
                  stencilHoleItem = treeListControl.InsertItem(pinName,attributeSourceGeometryPin,topFlag,
                                       I_IMAGECALLBACK,I_IMAGECALLBACK,subPadStackItem);
                  treeListControl.setSubPinItemData(stencilHoleItem,subPin);
               }
            }
         }
      }
   }

   treeListControl.collapseLowerHierarchies();
   treeListControl.RedrawWindow();

   m_fillingTreeFlag = false;
}

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CStencilComponentRulePreviewPage, CStencilRulePreviewPage)

CStencilComponentRulePreviewPage::CStencilComponentRulePreviewPage()
{
   m_psp.pszTitle = "Component Rules";
}

CStencilComponentRulePreviewPage::~CStencilComponentRulePreviewPage()
{
}

void CStencilComponentRulePreviewPage::fillTree()
{
   m_fillingTreeFlag = true;

   //if (! m_treeInitialized)
   //{
   //   return;
   //}

   COperationProgress progress;
   CStencilTreeListCtrl& treeListControl = m_treeListFrame.getStencilTree();
   treeListControl.DeleteAllItems();

   //if (treeListControl.GetColumnsNum() < 4)
   {
      treeListControl.InsertColumn(ColumnStencilObject,QPcbSurfaceTypeCompPin,
         LVCFMT_LEFT,getParent()->getColumnStencilObjectWidth());

      //treeListControl.InsertColumn(ColumnRule         ,QRule                      ,
      //   LVCFMT_LEFT,getParent()->getColumnRuleWidth());

      //treeListControl.InsertColumn(ColumnRuleValue    ,QRuleValue                 ,
      //   LVCFMT_LEFT,getParent()->getColumnRuleValueWidth());

      //treeListControl.InsertColumn(ColumnRuleModifier ,QRuleModifier              ,
      //   LVCFMT_LEFT,getParent()->getColumnRuleModifierWidth());

    //treeListControl.InsertColumn(ColumnStencilDerivationMethod,QDerivationMethod          ,LVCFMT_LEFT,150);
   }

   CStencilPins& stencilPins = getParent()->getStencilGenerator().getStencilPins();
   stencilPins.initializeRoot();
   CStencilSubPin* firstSubPin = (stencilPins.IsEmpty() ? NULL : stencilPins.GetHead()->getSubPin(0));

   HTREEITEM pcbItem     = treeListControl.InsertItem(QPCB,attributeSourcePcb,true,
      I_IMAGECALLBACK,I_IMAGECALLBACK);
   treeListControl.setSubPinItemData(pcbItem,firstSubPin);

   HTREEITEM topSurfaceItem     = treeListControl.InsertItem(QTopSurface,attributeSourceSurface,true,
      I_IMAGECALLBACK,I_IMAGECALLBACK,pcbItem);
   treeListControl.setSubPinItemData(topSurfaceItem,firstSubPin);

   HTREEITEM bottomSurfaceItem     = treeListControl.InsertItem(QBottomSurface,attributeSourceSurface,false,
      I_IMAGECALLBACK,I_IMAGECALLBACK,pcbItem);
   treeListControl.setSubPinItemData(bottomSurfaceItem,firstSubPin);

   CCamCadDatabase& camCadDatabase = getParent()->getCamCadDatabase();
   int mapSize = nextPrime2n(max(200 + stencilPins.GetSize(),round(1.5*stencilPins.GetSize())));
   CMapSortedStringToOb<CStencilSubPin> topSortedStencilHoles(mapSize,false);
   CMapSortedStringToOb<CStencilSubPin> bottomSortedStencilHoles(mapSize,false);
   topSortedStencilHoles.setSortFunction(sortByPinRef);
   bottomSortedStencilHoles.setSortFunction(sortByPinRef);

   progress.updateStatus("Scanning Pins",stencilPins.GetCount());

   for (POSITION pos = stencilPins.GetHeadPosition();pos != NULL;)
   {
      CStencilPin* stencilPin = stencilPins.GetNext(pos);
      stencilPin->updateStencilHoleAttribute();
      CString geometryName  = stencilPin->getGeometryName();

      if (!getStencilGenerator().isViaGeometry(geometryName)      && 
          !getStencilGenerator().isFiducialGeometry(geometryName)    )
      {
         for (int subPinIndex = 0;subPinIndex < stencilPin->getSubPinCount();subPinIndex++)
         {
            CStencilSubPin* subPin = stencilPin->getSubPin(subPinIndex);

            DataStruct* topCopperPad    = subPin->getStencilHole(true ).getCopperPad();
            DataStruct* bottomCopperPad = subPin->getStencilHole(false).getCopperPad();

            CString sortableSubPinRef = subPin->getSortableSubPinRef();

            // knv - may need to add subPinIndex to pinRef

            if (topCopperPad != NULL)
            {
               topSortedStencilHoles.SetAt(sortableSubPinRef,subPin);
            }

            if (bottomCopperPad != NULL)
            {
               bottomSortedStencilHoles.SetAt(sortableSubPinRef,subPin);
            }
         }
      }
   }

   int totalHolesToProcess = topSortedStencilHoles.GetCount() + bottomSortedStencilHoles.GetCount();
   progress.updateStatus("Processing Tree Hierarchy",totalHolesToProcess);

   for (int surfaceIndex = 0;surfaceIndex < 2;surfaceIndex++)
   {
      bool topFlag = (surfaceIndex == 0);

      HTREEITEM surfaceItem;
      CMapSortedStringToOb<CStencilSubPin>& sortedStencilHoles = (topFlag ? topSortedStencilHoles : bottomSortedStencilHoles);

      if (topFlag)
      {
         surfaceItem    = topSurfaceItem;
      }
      else
      {
         surfaceItem    = bottomSurfaceItem;
      }

      int index;
      CString key;
      CString* pKey = &key;
      CStencilSubPin* subPin;
      CString previousRefDes(" "),refDes,pinName,padDescriptor;
      HTREEITEM componentItem = NULL;
      HTREEITEM stencilHoleItem = NULL;

      for (sortedStencilHoles.rewind(index);sortedStencilHoles.next(subPin,index);)
      {
         progress.incrementProgress();

         CStencilPin* stencilPin = &(subPin->getStencilPin());

         bool hasSubPinsFlag = (stencilPin->getSubPinCount() > 1);

         refDes        = stencilPin->getRefDes();
         pinName       = stencilPin->getPinName();

         CStencilHole& stencilHole = subPin->getStencilHole(topFlag);

         padDescriptor = stencilHole.getCopperApertureDescriptor();

         CStencilRuleAttributes stencilRuleAttributes = stencilHole.getStencilRuleAttributes();
         CString attributeValue = stencilRuleAttributes.getRule().getRuleValue();

         //CString attributeName     = stencilHole.getStencilAttributeName();
         //CString attributeValue    = stencilHole.getStencilAttributeStringValue();
         //CString attributeModifier = stencilHole.getStencilModifierAttributeStringValue();

         DataStruct* topCopperPad    = subPin->getStencilHole(true ).getCopperPad();
         DataStruct* bottomCopperPad = subPin->getStencilHole(false).getCopperPad();

         if (refDes != previousRefDes)
         {
            componentItem = NULL;
         }

         if (componentItem == NULL)
         {
            if (topCopperPad    != NULL &&  topFlag ||
                bottomCopperPad != NULL && !topFlag     )
            {
               componentItem = treeListControl.InsertItem(refDes,attributeSourceComponent,topFlag,I_IMAGECALLBACK,I_IMAGECALLBACK,surfaceItem);
               treeListControl.setSubPinItemData(componentItem,subPin);

               CStencilRuleAttributes stencilRuleAttributes = stencilHole.getStencilRuleAttributes(attributeSourceComponent);

               if (stencilRuleAttributes.hasNonInheritedRule())
               {
                  StencilRuleTag ruleTag = stencilRuleAttributes.getRule().getRule();
                  CTreeListAttributeValueComboBox::addAttributeValue(ruleTag,stencilRuleAttributes.getRuleValue());
               }

               //Attrib* attribute;
               //Attrib* modifierAttribute;
               //int keywordIndex;
               //int modifierKeywordIndex;

               //if (stencilHole.getStencilAttributesForSource(attributeSourceComponent,attribute,keywordIndex,modifierAttribute,modifierKeywordIndex))
               //{
               //   const KeyWordStruct* keyword = camCadDatabase.getKeywordAt(keywordIndex);
               //   StencilAttributeTag attributeTag =
               //      attributeNameToStencilAttributeTag(keyword->getCCKeyword());
               //   CString attributeDisplayValue =
               //      CStencilPin::getStencilAttributeStringValue(camCadDatabase,keywordIndex,attribute);

               //   //treeListControl.SetItemText(componentItem,ColumnRule     ,
               //   //   camCadDatabase.getKeywordAt(keywordIndex)->cc);

               //   //treeListControl.SetItemText(componentItem,ColumnRuleValue,
               //   //   CStencilPin::getStencilAttributeStringValue(camCadDatabase,keywordIndex,attribute));

               //   CTreeListAttributeValueComboBox::addAttributeValue(attributeTag,attributeDisplayValue);
               //}

               //treeListControl.SetItemText(componentItem,ColumnStencilDerivationMethod,stencilPin->getDerivationMethod());
            }
         }

         if (componentItem != NULL)
         {
            AttributeSourceTag sourceTag = attributeSourceComponentPin;

            if (hasSubPinsFlag)
            {
               pinName.AppendFormat(".%d",subPin->getSubPinIndex() + 1);
               sourceTag = attributeSourceComponentSubPin;
            }

            stencilHoleItem = treeListControl.InsertItem(pinName,sourceTag,topFlag,I_IMAGECALLBACK,I_IMAGECALLBACK,componentItem);
            treeListControl.setSubPinItemData(stencilHoleItem,subPin);

            //Attrib* attribute;
            //int keywordIndex;

            //if (stencilPin->getStencilAttributeForSource(attributeSourcePin,attribute,keywordIndex))
            //{
            //   treeListControl.SetItemText(stencilHoleItem,ColumnRule     ,
            //      camCadDatabase.getKeywordAt(keywordIndex)->cc);

            //   treeListControl.SetItemText(stencilHoleItem,ColumnRuleValue,
            //      CStencilPin::getStencilAttributeStringValue(camCadDatabase,keywordIndex,attribute));
            //}

            //treeListControl.SetItemText(stencilHoleItem,ColumnStencilDerivationMethod,stencilPin->getDerivationMethod());

            previousRefDes = refDes;
         }
      }
   }

   treeListControl.collapseLowerHierarchies();
   treeListControl.RedrawWindow();

   m_fillingTreeFlag = false;
}

//_____________________________________________________________________________
//CMapStringToBlock::CMapStringToBlock() : CTypedMapStringToPtrContainer<BlockStruct*>(nextPrime2n(200),false)
//{
//}

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CStencilGeneratorPropertySheet, CResizingPropertySheet)

//CStencilGeneratorPropertySheet::CStencilGeneratorPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
//	:CResizingPropertySheet(nIDCaption, pParentWnd, iSelectPage)
//{
//}

//CStencilGeneratorPropertySheet::CStencilGeneratorPropertySheet(CCamCadDatabase& camCadDatabase,
//   LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
//	: CResizingPropertySheet(pszCaption, pParentWnd, iSelectPage),
//   m_camCadDatabase(camCadDatabase)
CStencilGeneratorPropertySheet::CStencilGeneratorPropertySheet(CStencilUi& stencilUi)
	: CResizingPropertySheet("Stencil Generator"),
     m_stencilUi(stencilUi),
     m_configurationPropertyPage(stencilUi.getStencilGenerationParameters()),
     m_validationRulesPropertyPage(stencilUi.getStencilValidationParameters())
{
   init();
}

CStencilGeneratorPropertySheet::~CStencilGeneratorPropertySheet()
{
   //delete m_stencilGenerator;
}

void CStencilGeneratorPropertySheet::init()
{
   //m_stencilGenerator = NULL;
   m_stencilGeneratorInitialized = false;
   //m_pcbFileNum       = -1;

   AddPage(&m_configurationPropertyPage);
   AddPage(&m_geometryRulePropertyPage);
   AddPage(&m_componentRulePropertyPage);
   //AddPage(&m_geomPadPropertyPage);
   //AddPage(&m_compPinPropertyPage);
   AddPage(&m_validationRulesPropertyPage);

   m_psh.dwFlags |= PSH_NOAPPLYNOW;
   m_psh.dwFlags &= ~PSH_HASHELP;

   m_attributeValuesInitializedFlag = false;
   m_columnStencilObjectWidth       = ColumnStencilObjectWidth;
   m_columnRuleWidth                = ColumnRuleWidth;
   m_columnRuleValueWidth           = ColumnRuleValueWidth;
   m_columnRuleModifierWidth        = ColumnRuleModifierWidth;
}

void CStencilGeneratorPropertySheet::initializeAttributeValues(CStencilPin& stencilPin,AttributeSourceTag sourceTag)
{
   //Attrib* attribute;
   //Attrib* modifierAttribute;
   //int stencilKeyword;
   //int stencilModifierKeyword;

   for (int pass = 0;pass <= 1;pass++)
   {
      bool topFlag = (pass == 0);

      for (int subPinIndex = 0;subPinIndex < stencilPin.getSubPinCount();subPinIndex++)
      {
         CStencilRuleAttributes stencilRuleAttributes = stencilPin.getStencilHole(subPinIndex,topFlag).getStencilRuleAttributes(sourceTag);

         if (stencilRuleAttributes.hasNonInheritedRule())
         {
            StencilRuleTag ruleTag = stencilRuleAttributes.getRule().getRule();
            CTreeListAttributeValueComboBox::addAttributeValue(ruleTag,stencilRuleAttributes.getRuleValue());
         }

         //if (stencilPin.getStencilHole(topFlag).getStencilAttributesForSource(sourceTag,attribute,stencilKeyword,modifierAttribute,stencilModifierKeyword))
         //{
         //   CString attributeDisplayValue =
         //      CStencilPin::getStencilAttributeStringValue(getCamCadDatabase(),
         //         stencilKeyword,attribute);

         //   StencilAttributeTag attributeTag = CStencilPin::getStencilAttributeTagFromKeywordIndex(stencilKeyword);

         //   CTreeListAttributeValueComboBox::addAttributeValue(attributeTag,attributeDisplayValue);
         //}
      }
   }
}

void CStencilGeneratorPropertySheet::initializeAttributeValues(bool resetFlag)
{
   if (!m_attributeValuesInitializedFlag || resetFlag)
   {
      CTreeListAttributeValueComboBox::clearAttributeValues();
      m_attributeValuesInitializedFlag = true;

      CStencilGenerator& stencilGenerator = getStencilGenerator();
      CStencilPins& stencilPins = stencilGenerator.getStencilPins();
      CStencilPin* stencilPin;

      for (POSITION pos = stencilPins.GetHeadPosition();pos != NULL;)
      {
         stencilPin = stencilPins.GetNext(pos);

         initializeAttributeValues(*stencilPin,attributeSourceGeometryPin  );
         initializeAttributeValues(*stencilPin,attributeSourceComponentPin );
         initializeAttributeValues(*stencilPin,attributeSourceComponentSubPin );
         initializeAttributeValues(*stencilPin,attributeSourceComponent    );
         initializeAttributeValues(*stencilPin,attributeSourcePadstack     );
         initializeAttributeValues(*stencilPin,attributeSourceSubPadstack  );
         initializeAttributeValues(*stencilPin,attributeSourceGeometry     );
         initializeAttributeValues(*stencilPin,attributeSourceMount      );
       //initializeAttributeValues(*stencilPin,attributeSourceVia        );
         initializeAttributeValues(*stencilPin,attributeSourceSurface    );
         initializeAttributeValues(*stencilPin,attributeSourcePcb        );
      }

      // aperture geometries
      for (int blockIndex = 0;blockIndex < getCamCadDatabase().getNumBlocks();blockIndex++)
      {
         BlockStruct* aperture = getCamCadDatabase().getBlock(blockIndex);

         if (aperture != NULL && aperture->isAperture())
         {
            CString apertureName = aperture->getName();

            if (!apertureName.IsEmpty())
            {
               CTreeListAttributeValueComboBox::addAttributeValue(stencilRuleApertureGeometry,apertureName);
            }
         }
      }

      if (CTreeListAttributeValueComboBox::getValues(stencilRuleArea).IsEmpty())
      {
         CTreeListAttributeValueComboBox::addAttributeValue(stencilRuleArea,"1.0");
      }

      if (CTreeListAttributeValueComboBox::getValues(stencilRuleInset).IsEmpty())
      {
         CTreeListAttributeValueComboBox::addAttributeValue(stencilRuleInset,"0.0");
      }

      if (CTreeListAttributeValueComboBox::getValues(stencilRuleAperture).IsEmpty())
      {
         CTreeListAttributeValueComboBox::addAttributeValue(stencilRuleAperture,"null0");
      }
   }
}

void CStencilGeneratorPropertySheet::clearStencilSettings()
{
   if (formatMessageBox(MB_ICONQUESTION | MB_YESNO,"All stencil rules will be removed from this PCB.  OK to continue?") == IDYES)
   {
      getStencilGenerator().clearStencilSettings();
      getStencilGenerator().clearStencilVessels();
      Invalidate();
   }
}

void CStencilGeneratorPropertySheet::updateStatus()
{
   if (::IsWindow(m_validationRulesPropertyPage))
   {
      m_validationRulesPropertyPage.UpdateData(false);
   }
}

void CStencilGeneratorPropertySheet::updateData(bool saveAndValidate)
{
   if (::IsWindow(m_configurationPropertyPage))
   {
      m_configurationPropertyPage.UpdateData(saveAndValidate);
   }

   if (::IsWindow(m_validationRulesPropertyPage))
   {
      m_validationRulesPropertyPage.UpdateData(saveAndValidate);
   }

   if (::IsWindow(m_geometryRulePropertyPage))
   {
      m_geometryRulePropertyPage.UpdateData(saveAndValidate);
   }

   if (::IsWindow(m_componentRulePropertyPage))
   {
      m_componentRulePropertyPage.UpdateData(saveAndValidate);
   }
}

int CStencilGeneratorPropertySheet::getColumnStencilObjectWidth() const
{
   return m_columnStencilObjectWidth;
}

void CStencilGeneratorPropertySheet::setColumnStencilObjectWidth(int width)
{
   m_columnStencilObjectWidth = width;
}

int CStencilGeneratorPropertySheet::getColumnRuleWidth() const
{
   return m_columnRuleWidth;
}

void CStencilGeneratorPropertySheet::setColumnRuleWidth(int width)
{
   m_columnRuleWidth = width;
}

int CStencilGeneratorPropertySheet::getColumnRuleValueWidth() const
{
   return m_columnRuleValueWidth;
}

void CStencilGeneratorPropertySheet::setColumnRuleValueWidth(int width)
{
   m_columnRuleValueWidth = width;
}

int CStencilGeneratorPropertySheet::getColumnRuleModifierWidth() const
{
   return m_columnRuleModifierWidth;
}

void CStencilGeneratorPropertySheet::setColumnRuleModifierWidth(int width)
{
   m_columnRuleModifierWidth = width;
}

void CStencilGeneratorPropertySheet::saveWindowState()
{
   CResizingPropertySheet::saveWindowState();

   CWnd* mainWnd = getMainWnd();
   CRect rc;
   GetWindowRect(&rc);
   mainWnd->ScreenToClient(&rc);

   CString dialogName(GetDialogProfileEntry());
   CString listViewParameters;

   listViewParameters.Format("%d %d %d %d",
      getColumnStencilObjectWidth(),
      getColumnRuleWidth(),
      getColumnRuleValueWidth(),
      getColumnRuleModifierWidth()  );

   AfxGetApp()->WriteProfileString(dialogName,"ListView Parameters",listViewParameters);
}

void CStencilGeneratorPropertySheet::restoreWindowState()
{
   CResizingPropertySheet::restoreWindowState();

   CString dialogName(GetDialogProfileEntry());
   CSupString listViewParameters = AfxGetApp()->GetProfileString(dialogName,"ListView Parameters","");

   if (!listViewParameters.IsEmpty())
   {
      CStringArray param;
      listViewParameters.Parse(param);

      if (param.GetSize() >= 4)
      {
         setColumnStencilObjectWidth(atoi(param[0]));
         setColumnRuleWidth(         atoi(param[1]));
         setColumnRuleValueWidth(    atoi(param[2]));
         setColumnRuleModifierWidth( atoi(param[3]));
      }
   }
}

//void CStencilGeneratorPropertySheet::reloadStencilHoles()
//{
//   m_geomPadPropertyPage.fillTree();
//   m_compPinPropertyPage.fillTree();
//}

//CStencilGenerator& CStencilGeneratorPropertySheet::getStencilGenerator(COperationProgress* progress)
//{
//   if (!m_stencilGeneratorInitialized)
//   {
//      m_stencilGeneratorInitialized = true;
//      //m_stencilGenerator = new CStencilGenerator(m_camCadDatabase);
//
//      getPcbFileNum();
//
//      if (m_pcbFileNum >= 0)
//      {
//         CStencilPin::registerStencilKeywords(m_camCadDatabase,getLogFile());
//         m_stencilGenerator.initializeStencilEntities(m_pcbFileNum,progress);
//      }
//   }
//
//   return m_stencilGenerator;
//}

//int CStencilGeneratorPropertySheet::getPcbFileNum()
//{
//   if (m_pcbFileNum < 0)
//   {
//      FileStruct* pcbFileStruct = getCamCadDatabase().getSingleVisiblePcb();
//
//      if (pcbFileStruct != NULL)
//      {
//         m_pcbFileNum = pcbFileStruct->num;
//      }
//      else
//      {
//         m_pcbFileNum = -1;
//         ErrorMessage("One and only one board must be visible to execute the stencil generator.", "Stencil Generator", MB_OK | MB_ICONHAND);
//      }
//   }
//
//   return m_pcbFileNum;
//}

BEGIN_MESSAGE_MAP(CStencilGeneratorPropertySheet, CResizingPropertySheet)
   ON_BN_CLICKED(IDOK, OnBnClickedOK)
   ON_WM_SIZE()
END_MESSAGE_MAP()


// CStencilGeneratorPropertySheet message handlers

BOOL CStencilGeneratorPropertySheet::OnInitDialog()
{
#ifdef STENCIL_TIMER
   GetStencilTimer().AddMessage("enter CStencilGeneratorPropertySheet::OnInitDialog()");
#endif

   //WINDOWPLACEMENT windowPlacement;
   //CWnd* okButton = GetDlgItem(IDOK);

   //okButton->GetWindowPlacement(&windowPlacement);
   //windowPlacement.rcNormalPosition.left -= 40;
   //okButton->SetWindowPlacement(&windowPlacement);
   //okButton->SetWindowText("Generate Stencils");

   GetDlgItem(IDCANCEL)->SetWindowText("Close");
   GetDlgItem(IDHELP  )->ShowWindow(SW_HIDE);
   GetDlgItem(IDOK    )->SetWindowText("Hide");
   BOOL bResult = CResizingPropertySheet::OnInitDialog();
   // Case dts0100495755 - stop opening HELP all the time : SendMessage(DM_SETDEFID,IDHELP,0);

   //if (getPcbFileNum() < 0)
   //{
   //   okButton->EnableWindow(false);
   //}

#ifdef STENCIL_TIMER
   GetStencilTimer().AddMessage("exit CStencilGeneratorPropertySheet::OnInitDialog()");
#endif

   return bResult;
}

afx_msg void CStencilGeneratorPropertySheet::OnSize(UINT nType, int cx, int cy)
{
   CResizingPropertySheet::OnSize(nType,cx,cy);
}

void CStencilGeneratorPropertySheet::OnBnClickedOK()
{
   EndDialog(HidePropertySheet);
}

//_____________________________________________________________________________
//IMPLEMENT_DYNAMIC(CStencilErrorBrowseDialog, CResizingDialog)

CStencilErrorBrowseDialog::CStencilErrorBrowseDialog(CStencilUi& stencilUi,CWnd* pParent /*=NULL*/)
   : CResizingDialog(CStencilErrorBrowseDialog::IDD, pParent)
   , m_stencilUi(stencilUi)
   , m_disableHidingChecked(FALSE)
   , m_treeListFrame(*this)
{
   m_pcbItem                   = NULL;
   m_topSurfaceItem            = NULL;
   m_topSmdSurfaceItem         = NULL;
   m_topThSurfaceItem          = NULL;
   m_topViaSurfaceItem         = NULL;
   m_topFiducialSurfaceItem    = NULL;
   m_bottomSurfaceItem         = NULL;
   m_bottomSmdSurfaceItem      = NULL;
   m_bottomThSurfaceItem       = NULL;
   m_bottomViaSurfaceItem      = NULL;
   m_bottomFiducialSurfaceItem = NULL;

   m_selectedStencilViolation  = NULL;

   m_fillingList = false;
   m_initialized = false;

   addFieldControl(IDC_ZoomToButton         ,anchorBottomLeft);
   addFieldControl(IDC_PanToButton          ,anchorBottomLeft);
   addFieldControl(IDC_DisableHidingCheckBox,anchorBottomLeft);
   addFieldControl(IDC_HideButton           ,anchorBottomRight);
   addFieldControl(IDC_CloseButton          ,anchorBottomRight);

   addFieldControl(IDC_ListControl          ,anchorLeft      ,growBoth      );
   addFieldControl(IDC_TreeListCtrl         ,anchorBottomLeft,growHorizontal);
}

CStencilErrorBrowseDialog::~CStencilErrorBrowseDialog()
{
}

void CStencilErrorBrowseDialog::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_ListControl, m_errorListControl);
   DDX_Check(pDX, IDC_DisableHidingCheckBox, m_disableHidingChecked);
}

CStencilTreeListCtrl& CStencilErrorBrowseDialog::getStencilTreeListCtrl()
{
   return m_treeListFrame.getStencilTree();
}

CStencilPin* CStencilErrorBrowseDialog::getSelectedStencilHole()
{
   CStencilPin* selectedStencilPin = NULL;

   if (m_selectedStencilViolation != NULL)
   {
      selectedStencilPin = &(m_selectedStencilViolation->getStencilPin());
   }

   return selectedStencilPin;
}

CStencilSubPin* CStencilErrorBrowseDialog::getSelectedStencilSubPin()
{
   CStencilSubPin* selectedStencilSubPin = NULL;

   if (m_selectedStencilViolation != NULL)
   {
      selectedStencilSubPin = &(m_selectedStencilViolation->getStencilSubPin());
   }

   return selectedStencilSubPin;
}

#define PinRefColumn 0
#define SurfaceColumn 1
#define ViolationColumn 2

void CStencilErrorBrowseDialog::fillList()
{
   m_fillingList = true;
   m_errorListControl.DeleteAllItems();

   if (m_errorListControl.GetColumnCount() < 2)
   {
      m_errorListControl.InsertColumn(PinRefColumn   ,"PinRef"   ,LVCFMT_LEFT,56 );
      m_errorListControl.InsertColumn(SurfaceColumn  ,"Surface"  ,LVCFMT_LEFT,50);
      m_errorListControl.InsertColumn(ViolationColumn,"Violation",LVCFMT_LEFT,500);
   }

   m_selectedStencilViolation = NULL;

   int violationIndex;
   CStencilViolation* stencilViolation;
   CString sortKey;
   int rowIndex = 0;
   int firstItemIndex = -1;

   for (getStencilViolations().rewind(violationIndex);
        getStencilViolations().next(stencilViolation,sortKey,violationIndex);rowIndex++)
   {
      int imageIndex = m_undefinedIconIndex;

      switch (stencilViolation->getStatus())
      {
      case violationStatusOpen:    imageIndex = m_openIconIndex;    break;
      case violationStatusClosed:  imageIndex = m_closedIconIndex;  break;
      case violationStatusInfo:    imageIndex = m_infoIconIndex;    break;
      }

      int itemIndex = m_errorListControl.InsertItem(LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE,rowIndex,
         stencilViolation->getStencilPin().getPinRef(),0,0,imageIndex,(LPARAM)stencilViolation);

      CString surface = (stencilViolation->getTopFlag() ? QTop : QBottom);

      m_errorListControl.SetItemText(itemIndex,SurfaceColumn  ,surface);
      m_errorListControl.SetItemText(itemIndex,ViolationColumn,stencilViolation->getDescription());

      if (rowIndex == 0)
      {
         firstItemIndex = itemIndex;
         m_selectedStencilViolation = stencilViolation;
      }
   }

   if (firstItemIndex >= 0)
   {
      m_errorListControl.SetItemState(firstItemIndex,LVIS_SELECTED,LVIS_SELECTED);
   }

   m_fillingList = false;
}

HTREEITEM CStencilErrorBrowseDialog::getPcbItem()
{
   if (m_pcbItem == NULL)
   {
      m_pcbItem = getStencilTreeListCtrl().InsertItem(QPCB,attributeSourcePcb,true,
         getStencilTreeListCtrl().getPcbIconIndex(),getStencilTreeListCtrl().getPcbIconIndex());
      getStencilTreeListCtrl().setSubPinItemData(m_pcbItem,getSelectedStencilSubPin());
   }

   return m_pcbItem;
}

HTREEITEM CStencilErrorBrowseDialog::getTopSurfaceItem()
{
   if (m_topSurfaceItem == NULL)
   {
      m_topSurfaceItem = getStencilTreeListCtrl().InsertItem(QTopSurface,attributeSourceSurface,true,
         getStencilTreeListCtrl().getSurfaceIconIndex(),getStencilTreeListCtrl().getSurfaceIconIndex(),
         getPcbItem());
      getStencilTreeListCtrl().setSubPinItemData(m_topSurfaceItem,getSelectedStencilSubPin());
   }

   return m_topSurfaceItem;
}

HTREEITEM CStencilErrorBrowseDialog::getTopSmdSurfaceItem()
{
   if (m_topSmdSurfaceItem == NULL)
   {
      m_topSmdSurfaceItem = getStencilTreeListCtrl().InsertItem(QSmdGeometries,attributeSourceMount,true,
         getStencilTreeListCtrl().getSmdThIconIndex(),getStencilTreeListCtrl().getSmdThIconIndex(),
         getTopSurfaceItem());
      getStencilTreeListCtrl().setSubPinItemData(m_topSmdSurfaceItem,getSelectedStencilSubPin());
   }

   return m_topSmdSurfaceItem;
}

HTREEITEM CStencilErrorBrowseDialog::getTopThSurfaceItem()
{
   if (m_topThSurfaceItem == NULL)
   {
      m_topThSurfaceItem = getStencilTreeListCtrl().InsertItem(QThGeometries,attributeSourceMount,true,
         getStencilTreeListCtrl().getSmdThIconIndex(),getStencilTreeListCtrl().getSmdThIconIndex(),
         getTopSurfaceItem());
      getStencilTreeListCtrl().setSubPinItemData(m_topThSurfaceItem,getSelectedStencilSubPin());
   }

   return m_topThSurfaceItem;
}

HTREEITEM CStencilErrorBrowseDialog::getTopViaSurfaceItem()
{
   if (m_topViaSurfaceItem == NULL)
   {
      m_topViaSurfaceItem = getStencilTreeListCtrl().InsertItem(QViaGeometries,attributeSourceMount,true,
         getStencilTreeListCtrl().getSmdThIconIndex(),getStencilTreeListCtrl().getSmdThIconIndex(),
         getTopSurfaceItem());
      getStencilTreeListCtrl().setSubPinItemData(m_topViaSurfaceItem,getSelectedStencilSubPin());
   }

   return m_topViaSurfaceItem;
}

HTREEITEM CStencilErrorBrowseDialog::getTopFiducialSurfaceItem()
{
   if (m_topFiducialSurfaceItem == NULL)
   {
      m_topFiducialSurfaceItem = getStencilTreeListCtrl().InsertItem(QFiducialGeometries,attributeSourceMount,true,
         getStencilTreeListCtrl().getFiducialIconIndex(),getStencilTreeListCtrl().getFiducialIconIndex(),
         getTopSurfaceItem());
      getStencilTreeListCtrl().setSubPinItemData(m_topFiducialSurfaceItem,getSelectedStencilSubPin());
   }

   return m_topFiducialSurfaceItem;
}

HTREEITEM CStencilErrorBrowseDialog::getBottomSurfaceItem()
{
   if (m_bottomSurfaceItem == NULL)
   {
      m_bottomSurfaceItem = getStencilTreeListCtrl().InsertItem(QBottomSurface,attributeSourceSurface,false,
         getStencilTreeListCtrl().getSurfaceIconIndex(),getStencilTreeListCtrl().getSurfaceIconIndex(),
         getPcbItem());
      getStencilTreeListCtrl().setSubPinItemData(m_bottomSurfaceItem,getSelectedStencilSubPin());
   }

   return m_bottomSurfaceItem;
}

HTREEITEM CStencilErrorBrowseDialog::getBottomSmdSurfaceItem()
{
   if (m_bottomSmdSurfaceItem == NULL)
   {
      m_bottomSmdSurfaceItem = getStencilTreeListCtrl().InsertItem(QSmdGeometries,attributeSourceMount,false,
         getStencilTreeListCtrl().getSmdThIconIndex(),getStencilTreeListCtrl().getSmdThIconIndex(),
         getBottomSurfaceItem());
      getStencilTreeListCtrl().setSubPinItemData(m_bottomSmdSurfaceItem,getSelectedStencilSubPin());
   }

   return m_bottomSmdSurfaceItem;
}

HTREEITEM CStencilErrorBrowseDialog::getBottomThSurfaceItem()
{
   if (m_bottomThSurfaceItem == NULL)
   {
      m_bottomThSurfaceItem = getStencilTreeListCtrl().InsertItem(QThGeometries,attributeSourceMount,false,
         getStencilTreeListCtrl().getSmdThIconIndex(),getStencilTreeListCtrl().getSmdThIconIndex(),
         getBottomSurfaceItem());
      getStencilTreeListCtrl().setSubPinItemData(m_bottomThSurfaceItem,getSelectedStencilSubPin());
   }

   return m_bottomThSurfaceItem;
}

HTREEITEM CStencilErrorBrowseDialog::getBottomViaSurfaceItem()
{
   if (m_bottomViaSurfaceItem == NULL)
   {
      m_bottomViaSurfaceItem = getStencilTreeListCtrl().InsertItem(QViaGeometries,attributeSourceMount,false,
         getStencilTreeListCtrl().getSmdThIconIndex(),getStencilTreeListCtrl().getSmdThIconIndex(),
         getBottomSurfaceItem());
      getStencilTreeListCtrl().setSubPinItemData(m_bottomViaSurfaceItem,getSelectedStencilSubPin());
   }

   return m_bottomViaSurfaceItem;
}

HTREEITEM CStencilErrorBrowseDialog::getBottomFiducialSurfaceItem()
{
   if (m_bottomFiducialSurfaceItem == NULL)
   {
      m_bottomFiducialSurfaceItem = getStencilTreeListCtrl().InsertItem(QFiducialGeometries,attributeSourceMount,false,
         getStencilTreeListCtrl().getFiducialIconIndex(),getStencilTreeListCtrl().getFiducialIconIndex(),
         getBottomSurfaceItem());
      getStencilTreeListCtrl().setSubPinItemData(m_bottomFiducialSurfaceItem,getSelectedStencilSubPin());
   }

   return m_bottomFiducialSurfaceItem;
}

void CStencilErrorBrowseDialog::expandTree(HTREEITEM item)
{
   CStencilTreeListCtrl& treeCtrl = getStencilTreeListCtrl();

   if (treeCtrl.ItemHasChildren(item))
   {
      treeCtrl.Expand(item,TVE_EXPAND);

      HTREEITEM childItem = treeCtrl.GetChildItem(item);

      while (childItem != NULL)
      {
         expandTree(childItem);
         childItem = treeCtrl.GetNextSiblingItem(childItem);
      }
   }
}

void CStencilErrorBrowseDialog::empty()
{
   emptyTree();
   emptyList();
}

void CStencilErrorBrowseDialog::emptyTree()
{
   getStencilTreeListCtrl().DeleteAllItems();

   m_pcbItem                   = NULL;
   m_topSurfaceItem            = NULL;
   m_topSmdSurfaceItem         = NULL;
   m_topThSurfaceItem          = NULL;
   m_topViaSurfaceItem         = NULL;
   m_topFiducialSurfaceItem    = NULL;
   m_bottomSurfaceItem         = NULL;
   m_bottomSmdSurfaceItem      = NULL;
   m_bottomThSurfaceItem       = NULL;
   m_bottomViaSurfaceItem      = NULL;
   m_bottomFiducialSurfaceItem = NULL;
}

void CStencilErrorBrowseDialog::emptyList()
{
   m_errorListControl.DeleteAllItems();
}

void CStencilErrorBrowseDialog::fillTree()
{
   CStencilTreeListCtrl& treeListControl = getStencilTreeListCtrl();

   emptyTree();

   if (treeListControl.GetColumnsNum() < 3)
   {
      treeListControl.InsertColumn(ColumnStencilObject,QPcbSurfaceTypeGeomPadCompPin,LVCFMT_LEFT,ColumnStencilObjectWidth);
      treeListControl.InsertColumn(ColumnRule         ,QRule                        ,LVCFMT_LEFT,ColumnRuleWidth);
      treeListControl.InsertColumn(ColumnRuleValue    ,QRuleValue                   ,LVCFMT_LEFT,ColumnRuleValueWidth);
      treeListControl.InsertColumn(ColumnRuleModifier ,QRuleModifier                ,LVCFMT_LEFT,ColumnRuleModifierWidth);
    //treeListControl.InsertColumn(ColumnStencilDerivationMethod,QDerivationMethod,LVCFMT_LEFT,150);
   }

   CStencilSubPin* subPin = getSelectedStencilSubPin();

   if (subPin != NULL)
   {
      CStencilPin& stencilPin = subPin->getStencilPin();
      CString geometryName  = stencilPin.getGeometryName();

      for (int subPinIndex = 0;subPinIndex < stencilPin.getSubPinCount();subPinIndex++)
      {
         DataStruct* topCopperPad    = subPin->getStencilHole(true ).getCopperPad();
         DataStruct* bottomCopperPad = subPin->getStencilHole(false).getCopperPad();
         bool thFlag = ! getStencilGenerator().isSmdGeometry(geometryName);
         bool topFlag;

         if (topCopperPad != NULL)
         {
            topFlag = true;

            HTREEITEM geometryItem = treeListControl.InsertItem(stencilPin.getGeometryName(),
               attributeSourceGeometry,topFlag,
               treeListControl.getGeomIconIndex(),treeListControl.getGeomIconIndex(),
               thFlag ? getTopThSurfaceItem() : getTopSmdSurfaceItem());

            treeListControl.setSubPinItemData(geometryItem ,subPin);

            if (geometryItem != NULL)
            {
               CString description = stencilPin.getPadStackName() + ":" +
                                    stencilPin.getStencilHole(subPinIndex,topFlag).getCopperApertureDescriptor();

               HTREEITEM padItem = treeListControl.InsertItem(description,
                  attributeSourcePadstack,topFlag,
                  treeListControl.getPadIconIndex(),treeListControl.getPadIconIndex(),
                  geometryItem);

               treeListControl.setSubPinItemData(padItem,subPin);

               HTREEITEM pinItem = treeListControl.InsertItem(stencilPin.getPinName(),
                  attributeSourceGeometryPin,topFlag,
                  treeListControl.getPinIconIndex(),treeListControl.getPinIconIndex(),
                  padItem);

               treeListControl.setSubPinItemData(pinItem ,subPin);
            }

            HTREEITEM componentItem = treeListControl.InsertItem(stencilPin.getRefDes(),
               attributeSourceComponent,topFlag,
               treeListControl.getCompIconIndex(),treeListControl.getCompIconIndex(),
               getTopSurfaceItem());

            treeListControl.setSubPinItemData(componentItem,subPin);

            if (componentItem != NULL)
            {
               HTREEITEM pinItem = treeListControl.InsertItem(stencilPin.getPinName(),
                  attributeSourceComponentPin,topFlag,
                  treeListControl.getPinInstanceIconIndex(),treeListControl.getPinInstanceIconIndex(),
                  componentItem);

               treeListControl.setSubPinItemData(pinItem,subPin);
            }
         }

         if (thFlag)
         {
            HTREEITEM surfaceItem = getBottomSurfaceItem();
         }
         else if (bottomCopperPad != NULL)
         {
            topFlag = false;

            HTREEITEM geometryItem = treeListControl.InsertItem(stencilPin.getGeometryName(),
               attributeSourceGeometry,topFlag,
               treeListControl.getGeomIconIndex(),treeListControl.getGeomIconIndex(),
               thFlag ? getBottomThSurfaceItem() : getBottomSmdSurfaceItem());

            treeListControl.setSubPinItemData(geometryItem ,subPin);

            if (geometryItem != NULL)
            {
               CString description = stencilPin.getPadStackName() + ":" +
                                    stencilPin.getStencilHole(subPinIndex,topFlag).getCopperApertureDescriptor();

               HTREEITEM padItem = treeListControl.InsertItem(description,
                  attributeSourcePadstack,topFlag,
                  treeListControl.getPadIconIndex(),treeListControl.getPadIconIndex(),
                  geometryItem);

               treeListControl.setSubPinItemData(padItem,subPin);

               HTREEITEM pinItem = treeListControl.InsertItem(stencilPin.getPinName(),
                  attributeSourceGeometryPin,topFlag,
                  treeListControl.getPinIconIndex(),treeListControl.getPinIconIndex(),
                  padItem);

               treeListControl.setSubPinItemData(pinItem ,subPin);
            }

            HTREEITEM componentItem = treeListControl.InsertItem(stencilPin.getRefDes(),
               attributeSourceComponent,topFlag,
               treeListControl.getCompIconIndex(),treeListControl.getCompIconIndex(),
               getBottomSurfaceItem());

            treeListControl.setSubPinItemData(componentItem,subPin);

            if (componentItem != NULL)
            {
               HTREEITEM pinItem = treeListControl.InsertItem(stencilPin.getPinName(),
                  attributeSourceComponentPin,topFlag,
                  treeListControl.getPinInstanceIconIndex(),treeListControl.getPinInstanceIconIndex(),
                  componentItem);

               treeListControl.setSubPinItemData(pinItem,subPin);
            }
         }
      }
   }

   expandTree(getPcbItem());
}

BEGIN_MESSAGE_MAP(CStencilErrorBrowseDialog, CResizingDialog)
   ON_BN_CLICKED(IDC_ZoomToButton, OnBnClickedZoomToButton)
   ON_BN_CLICKED(IDC_PanToButton, OnBnClickedPanToButton)
   ON_BN_CLICKED(IDC_CloseButton, OnBnClickedCloseButton)
   ON_BN_CLICKED(IDOK, OnBnClickedCloseButton)
   ON_BN_CLICKED(IDCANCEL, OnBnClickedCloseButton)
   ON_NOTIFY(LVN_ODSTATECHANGED, IDC_ListControl, OnLvnOdstatechangedListcontrol)
   ON_NOTIFY(LVN_ITEMACTIVATE, IDC_ListControl, OnLvnItemActivateListcontrol)
   ON_NOTIFY(LVN_ITEMCHANGED, IDC_ListControl, OnLvnItemchangedListcontrol)
   ON_WM_CLOSE()
   ON_WM_ACTIVATE()
   ON_BN_CLICKED(IDC_HideButton, OnBnClickedHideButton)
END_MESSAGE_MAP()

#define QStencilErrorBrowseDialog "StencilErrorBrowseDialog"
#define QDisableHiding            "DisableHiding"

// CStencilErrorBrowseDialog message handlers
BOOL CStencilErrorBrowseDialog::OnInitDialog()
{
#ifdef STENCIL_TIMER
   GetStencilTimer().AddMessage("enter CStencilErrorBrowseDialog::OnInitDialog()");
#endif

   CResizingDialog::OnInitDialog();

   m_disableHidingChecked = AfxGetApp()->GetProfileInt(QStencilErrorBrowseDialog,QDisableHiding,0);

   // instantiate the control windows
   UpdateData(false);

   // tree
   m_treeListFrame.SubclassDlgItem(IDC_TreeListCtrl,this);
   //m_treeListFrame.ModifyStyleEx(WS_EX_TOPMOST,WS_EX_CLIENTEDGE);
   m_treeListFrame.getTree().setComboBoxAt(ColumnRule        ,new CTreeListStencilRuleComboBox(ColumnRule));
   m_treeListFrame.getTree().setComboBoxAt(ColumnRuleValue   ,new CTreeListStencilRuleValueComboBox(ColumnRuleValue));
   m_treeListFrame.getTree().setComboBoxAt(ColumnRuleModifier,new CTreeListStencilRuleModifierComboBox(ColumnRuleModifier));

   //getParent()->initializeAttributeValues();
   //fillTree();

   // list
   m_errorListControl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);

   CWinApp* app = AfxGetApp();
   m_listImageList.Create(16, 16, ILC_MASK, 16, 0);

   m_listImageList.Add(app->LoadIcon(IDI_RED_DOT  ));  // because of one based array
   m_openIconIndex      = m_listImageList.Add(app->LoadIcon(IDI_RED_DOT  ));
   m_closedIconIndex    = m_listImageList.Add(app->LoadIcon(IDI_GREEN_DOT));
   m_infoIconIndex      = m_listImageList.Add(app->LoadIcon(IDI_INFO));
   m_undefinedIconIndex = m_listImageList.Add(app->LoadIcon(IDI_UNDEFINED));

   m_errorListControl.SetImageList(&m_listImageList,LVSIL_SMALL);

   fillList();

   // other
   m_initialized = true;
#ifdef STENCIL_TIMER
   GetStencilTimer().AddMessage("exit CStencilErrorBrowseDialog::OnInitDialog()");
#endif

   return true;
}

void CStencilErrorBrowseDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
   CResizingDialog::OnActivate(nState, pWndOther, bMinimized);

   if (nState == WA_ACTIVE && m_initialized)
   {
      if (m_errorListControl.GetItemCount() == 0)
      {
         fillList();
         fillTree();
      }
   }

   // TODO: Add your message handler code here
}

void CStencilErrorBrowseDialog::OnBnClickedZoomToButton()
{
   UpdateData(true);

   CStencilPin* stencilPin = getSelectedStencilHole();

   if (stencilPin != NULL && getActiveView() != NULL)
   {
      if (! m_disableHidingChecked)
      {
         getStencilUi().generateEvent(ID_StencilCommand_PanZoom);
      }

      CPoint2d origin = stencilPin->getOrigin();
      CCEtoODBDoc* doc = &(getCamCadDatabase().getCamCadDoc());

      double xmin, xmax, ymin, ymax;

      if (!ComponentsExtents(doc,stencilPin->getRefDes(),
            &xmin,&xmax,&ymin,&ymax,FALSE))
      {
         PanReference(getActiveView(),stencilPin->getRefDes());
         ErrorMessage("Component has no extents", "Panning");
      }
      else
      {
         double marginSizeX = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) * doc->getSettings().CompExtentMargin / 100;
         double marginSizeY = (doc->getSettings().getYmax() - doc->getSettings().getYmin()) * doc->getSettings().CompExtentMargin / 100;

         double marginSize = max(marginSizeX, marginSizeY);

         xmin -= marginSize;
         xmax += marginSize;

         ymin -= marginSize;
         ymax += marginSize;

         getActiveView()->ZoomBox(xmin, xmax, ymin, ymax);
         getActiveView()->showCross(origin.x,origin.y);
      }
   }
}

void CStencilErrorBrowseDialog::OnBnClickedPanToButton()
{
   UpdateData(true);

   CStencilPin* stencilPin = getSelectedStencilHole();

   if (stencilPin != NULL && getActiveView() != NULL)
   {
      if (! m_disableHidingChecked)
      {
         getStencilUi().generateEvent(ID_StencilCommand_PanZoom);
      }

      CPoint2d origin = stencilPin->getOrigin();

      if (getCamCadDatabase().getCamCadDoc().getBottomView())
      {
         origin.x *= -1.;
      }

      getActiveView()->PanCoordinate(origin.x,origin.y,TRUE);
   }
}

void CStencilErrorBrowseDialog::OnBnClickedCloseButton()
{
   OnClose();
}

void CStencilErrorBrowseDialog::OnBnClickedHideButton()
{
   getStencilUi().generateEvent(ID_StencilCommand_PanZoom);
}

void CStencilErrorBrowseDialog::OnClose()
{
   // TODO: Add your message handler code here and/or call default
   //CResizingDialog::OnClose();
   UpdateData(true);

   AfxGetApp()->WriteProfileInt(QStencilErrorBrowseDialog,QDisableHiding,m_disableHidingChecked);

   getStencilUi().generateEvent(ID_StencilCommand_TerminateBrowseStencilErrors);

   ShowWindow(SW_HIDE);
   //DestroyWindow();
}

void CStencilErrorBrowseDialog::OnLvnOdstatechangedListcontrol(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMLVODSTATECHANGE pStateChanged = reinterpret_cast<LPNMLVODSTATECHANGE>(pNMHDR);
   // TODO: Add your control notification handler code here

   m_selectedStencilViolation = NULL;

   for (POSITION pos = m_errorListControl.GetFirstSelectedItemPosition();pos != NULL;)
   {
      int itemIndex = m_errorListControl.GetNextSelectedItem(pos);

      m_selectedStencilViolation = (CStencilViolation*)m_errorListControl.GetItemData(itemIndex);
   }

   *pResult = 0;
}

void CStencilErrorBrowseDialog::OnLvnItemActivateListcontrol(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
   // TODO: Add your control notification handler code here
   *pResult = 0;
}

void CStencilErrorBrowseDialog::OnLvnItemchangedListcontrol(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
   // TODO: Add your control notification handler code here

   //if (! m_fillingList)
   {
      if ((pNMLV->uChanged & LVIF_STATE) != 0)
      {
         if ((pNMLV->uOldState & LVIS_SELECTED) != 0)
         {
            m_selectedStencilViolation = NULL;
            //emptyTree();
         }

         if ((pNMLV->uNewState & LVIS_SELECTED) != 0)
         {
            for (POSITION pos = m_errorListControl.GetFirstSelectedItemPosition();pos != NULL;)
            {
               int itemIndex = m_errorListControl.GetNextSelectedItem(pos);

               m_selectedStencilViolation = (CStencilViolation*)m_errorListControl.GetItemData(itemIndex);

               fillTree();

               break;
            }
         }
      }
   }

   *pResult = 0;
}

//_____________________________________________________________________________
//IMPLEMENT_DYNAMIC(CStencilWaitDialog, CResizingDialog)

CStencilWaitDialog::CStencilWaitDialog(CStencilUi& stencilUi,CWnd* pParent /*=NULL*/)
	: CResizingDialog(CStencilWaitDialog::IDD, pParent) ,
   m_stencilUi(stencilUi)
{
}

CStencilWaitDialog::~CStencilWaitDialog()
{
}

void CStencilWaitDialog::DoDataExchange(CDataExchange* pDX)
{
	CResizingDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CStencilWaitDialog, CResizingDialog)
   ON_BN_CLICKED(IDOK, OnBnClickedContinue)
   ON_BN_CLICKED(IDCANCEL, OnBnClickedTerminate)
END_MESSAGE_MAP()

// CStencilWaitDialog message handlers

void CStencilWaitDialog::OnBnClickedContinue()
{
   // TODO: Add your control notification handler code here
   getStencilUi().generateEvent(ID_StencilCommand_ContinueStencilSession);
   ShowWindow(SW_HIDE);
   //OnOK();
}

void CStencilWaitDialog::OnBnClickedTerminate()
{
   // TODO: Add your control notification handler code here
   getStencilUi().generateEvent(ID_StencilCommand_TerminateStencilSession);
   ShowWindow(SW_HIDE);
   //OnCancel();
}

//_____________________________________________________________________________
//IMPLEMENT_DYNAMIC(CApertureGeometryDialog, CResizingDialog)

CCamCadDatabase* CApertureGeometryDialog::m_camCadDatabase = NULL;

CApertureGeometryDialog::CApertureGeometryDialog(CWnd* pParent /*=NULL*/)
	: CResizingDialog(CApertureGeometryDialog::IDD, pParent)
{
   addFieldControl(IDC_ApertureGeometryList,anchorLeft,growBoth);
   addFieldControl(IDCANCEL                ,anchorBottomLeft);
   addFieldControl(IDOK                    ,anchorBottomLeft);
}

CApertureGeometryDialog::~CApertureGeometryDialog()
{
}

void CApertureGeometryDialog::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_ApertureGeometryList, m_apertureGeometryListBox);
}

CString CApertureGeometryDialog::getApertureName()
{
   return m_apertureName;
}

BEGIN_MESSAGE_MAP(CApertureGeometryDialog, CResizingDialog)
   ON_LBN_SELCHANGE(IDC_ApertureGeometryList, OnSelChangeApertureGeometryList)
   ON_WM_ACTIVATE()
END_MESSAGE_MAP()

// CApertureGeometryDialog message handlers

BOOL CApertureGeometryDialog::OnInitDialog()
{
#ifdef STENCIL_TIMER
   GetStencilTimer().AddMessage("enter CApertureGeometryDialog::OnInitDialog()");
#endif

   CResizingDialog::OnInitDialog();

   if (m_camCadDatabase != NULL)
   {
      for (int blockIndex = 0;blockIndex < m_camCadDatabase->getNumBlocks();blockIndex++)
      {
         BlockStruct* aperture = m_camCadDatabase->getBlock(blockIndex);

         if (aperture != NULL && aperture->isAperture())
         {
            CString apertureName = aperture->getName();

            if (!apertureName.IsEmpty())
            {
               m_apertureGeometryListBox.AddString(apertureName);
            }
         }
      }
   }
#ifdef STENCIL_TIMER
   GetStencilTimer().AddMessage("exit CApertureGeometryDialog::OnInitDialog()");
#endif

   return true;
}

void CApertureGeometryDialog::OnSelChangeApertureGeometryList()
{
   int selectionIndex = m_apertureGeometryListBox.GetCurSel();

   if (selectionIndex != LB_ERR)
   {
      m_apertureGeometryListBox.GetText(selectionIndex,m_apertureName);
   }
}


//_____________________________________________________________________________


//void CStencilCompPinPropertyPage::OnRButtonDown(UINT nFlags, CPoint point)
//{
//   // TODO: Add your message handler code here and/or call default
//   CWnd* childWindow = ChildWindowFromPoint(point);
//
//   if (childWindow == &m_treeListFrame)
//   {
//   }
//   else
//   {
//      CResizingPropertyPage::OnRButtonDown(nFlags, point);
//   }
//}

//void CStencilCompPinPropertyPage::OnContextMenu(CWnd* pWnd, CPoint screenPoint)
//{
//   // TODO: Add your message handler code here
//   CPoint windowRelativePoint(screenPoint);
//   ScreenToClient(&windowRelativePoint);
//
//   CPopupMenu popupMenu;
//   CString prompt;
//   prompt.Format("Action");
//
//   popupMenu.addItem(0,prompt);
//   popupMenu.addSeparator();
//   popupMenu.addItem(1001 /*ID_EditListProperties*/,"   Edit &Track view properties");
//   popupMenu.trackMenu(TPM_LEFTALIGN,windowRelativePoint,this);
//}
// C:\RSI\CamCad\4.5\StencilDialog.cpp : implementation file
//
// C:\Development\CamCad\4.5\StencilDialog.cpp : implementation file
//


BEGIN_MESSAGE_MAP(CStencilGeometryRulePreviewPage, CStencilRulePreviewPage)
   ON_WM_ACTIVATE()
END_MESSAGE_MAP()

void CStencilRulePreviewPage::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
   CResizingPropertyPage::OnActivate(nState, pWndOther, bMinimized);
   // Case dts0100495755 - stop opening HELP all the time : getParent()->SendMessage(DM_SETDEFID,IDHELP,0);
}

void CStencilGeometryRulePreviewPage::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
   CResizingPropertyPage::OnActivate(nState, pWndOther, bMinimized);
   // Case dts0100495755 - stop opening HELP all the time : getParent()->SendMessage(DM_SETDEFID,IDHELP,0);
}

void CStencilComponentRulePreviewPage::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
   CResizingPropertyPage::OnActivate(nState, pWndOther, bMinimized);
   // Case dts0100495755 - stop opening HELP all the time : getParent()->SendMessage(DM_SETDEFID,IDHELP,0);
}

BOOL CStencilGeometryRulePreviewPage::OnSetActive()
{
   int masterSmdListRevLevel = this->getStencilGenerator().getSMDListRevLevel();

   if (m_treeRevLevel < masterSmdListRevLevel)
   {
      this->fillTree();
      this->m_treeRevLevel = masterSmdListRevLevel;
   }

   return CStencilRulePreviewPage::OnSetActive();
}


BOOL CStencilComponentRulePreviewPage::OnSetActive()
{
   int masterSmdListRevLevel = this->getStencilGenerator().getSMDListRevLevel();

   if (m_treeRevLevel < masterSmdListRevLevel)
   {
      this->fillTree();
      this->m_treeRevLevel = masterSmdListRevLevel;
   }

   return CStencilRulePreviewPage::OnSetActive();
}

void CStandardApertureParameterEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   // TODO: Add your message handler code here and/or call default

   CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

