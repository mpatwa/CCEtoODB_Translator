// $Header: /CAMCAD/4.5/AppPalette.cpp 2     10/18/05 8:13p Kurt Van Ness $

/*
History: AppPalette.cpp $
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
#include "AppPalette.h"
#include "Colors.h"

#include <math.h>

extern COLORREF getDefaultColor(int colorIndex);

IMPLEMENT_DYNAMIC(CAppPalette,CPalette);
   
#ifdef USE_DEBUG_NEW
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////////
CAppPalette::CAppPalette()
{
   CDC* dc = AfxGetMainWnd()->GetDC();

   m_paletteDevice = ((dc->GetDeviceCaps(RASTERCAPS) & RC_PALETTE) != 0);

   m_logPalette = (LOGPALETTE*)&m_paletteBuf;
   m_logPalette->palVersion    = 0x300;
   m_logPalette->palNumEntries = NumAppColors;
   m_paletteEntries = &(m_logPalette->palPalEntry[0]);

   m_editIndex = NumAppColors - 1;

   initColors();
}

CAppPalette::~CAppPalette()
{
}

PALETTEENTRY* CAppPalette::getPaletteEntry(int index)
{
   ASSERT(index >= 0 && index < NumAppColors);

   PALETTEENTRY* paletteEntry = &(m_paletteEntries[index]);

   return paletteEntry;
}

void CAppPalette::setPaletteEntry(int index,COLORREF color,BYTE flags)
{
   PALETTEENTRY* paletteEntry = getPaletteEntry(index);

   paletteEntry->peRed   = GetRValue(color);
   paletteEntry->peGreen = GetGValue(color);
   paletteEntry->peBlue  = GetBValue(color);
   paletteEntry->peFlags = flags;
}

void CAppPalette::animatePaletteEntry(int index,COLORREF color)
{
   PALETTEENTRY* paletteEntry = getPaletteEntry(index);

   paletteEntry->peRed   = GetRValue(color);
   paletteEntry->peGreen = GetGValue(color);
   paletteEntry->peBlue  = GetBValue(color);
   paletteEntry->peFlags = PC_RESERVED;

   if (m_paletteDevice)
   {
      AnimatePalette(index,1,paletteEntry);
   }
}

COLORREF CAppPalette::getColorAt(int index)
{
   ASSERT(index >= 0 && index < NumAppColors);

   COLORREF retval = (m_paletteDevice ? PALETTEINDEX(index) : getRgbAt(index));

   return retval;
}

COLORREF CAppPalette::getRgbAt(int index)
{
   PALETTEENTRY* paletteEntry = getPaletteEntry(index);

   COLORREF retval = RGB(paletteEntry->peRed,paletteEntry->peGreen,paletteEntry->peBlue);

   return retval;
}

void CAppPalette::setEditPaletteEntry(COLORREF color)
{
   animatePaletteEntry(m_editIndex,color);
}

COLORREF CAppPalette::getDefaultColor(int colorIndex)
{
   static COLORREF defaultColors[] =
   {
      colorWhite  , colorLtGray   , colorDkGray , colorBlack    ,
      colorMagenta, colorDkMagenta, colorViolet , colorDkViolet ,
      colorBlue   , colorDkBlue   , colorLoBlue , colorDkLoBlue , 
      colorCyan   , colorDkCyan   , colorHiGreen, colorDkHiGreen,
      colorGreen  , colorDkGreen  , colorLoGreen, colorDkLoGreen, 
      colorYellow , colorDkYellow , colorOrange , colorDkOrange , 
      colorRed    , colorDkRed    , colorBlack  , colorBlack    ,
      colorBlack  , colorBlack    , colorBlack  , colorBlack     
   };

   const int numColors = sizeof(defaultColors) / sizeof(COLORREF);

   COLORREF color = colorBlack;

   if (colorIndex >= 0 && colorIndex < numColors)
   {
      color = defaultColors[colorIndex];
   }

   return color;
}

void CAppPalette::initColors()
{
   for (int colorIndex = 0;colorIndex < NumAppColors - 1;colorIndex++)
   {
      setPaletteEntry(colorIndex,getDefaultColor(colorIndex),PC_RESERVED);
   }

   setPaletteEntry(m_editIndex,colorWhite,PC_RESERVED);
}

int CAppPalette::create()
{
   int retval = CreatePalette(m_logPalette);

   if (!retval)
   {
      TRACE("CAppPalette::create() - CreatePalette() failure.\n");
   }

   return retval;
}

void CAppPalette::realizePalette(CDC* dc)
{
   if (m_paletteDevice)
   {
      CPalette* oldPalette = dc->SelectPalette(this,false);
      int numReplacements  = dc->RealizePalette();

      //TRACE("CAppPalette::realizePalette() - oldPalette = 0x%08x, numRemaps = %d\n",oldPalette,numReplacements);
   }
}

void CAppPalette::realizePalette(CWnd* wnd)
{
   CDC* dc = wnd->GetDC();

   realizePalette(dc);
}

void CAppPalette::pushPalette(CDC* dc)
{
   if (m_paletteDevice)
   {
      CPalette* oldPalette = dc->SelectPalette(this,false);
      //m_paletteStack.AddTail(oldPalette);

      int numReplacements  = dc->RealizePalette();

      TRACE("CAppPalette::pushPalette() - oldPalette = 0x%08x, numRemaps = %d, numStackEntries = %d\n",
         oldPalette,numReplacements,m_paletteStack.GetCount());
   }
}

void CAppPalette::popPalette(CDC* dc)
{
   if (m_paletteDevice && false)
   {
      ASSERT(m_paletteStack.GetCount() > 0);

      CPalette* oldPalette = dc->SelectPalette(m_paletteStack.GetTail(),false);
      m_paletteStack.RemoveTail();
   }
}



