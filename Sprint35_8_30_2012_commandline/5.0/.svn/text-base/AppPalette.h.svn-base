
#if !defined(__AppPalette_H__)
#define __AppPalette_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "PointVector.h"
#include <afxtempl.h>

#define NumAppColors 33

//_____________________________________________________________________________
class CAppPalette : public CPalette
{
   DECLARE_DYNAMIC(CAppPalette);
   
private:
   char m_paletteBuf[sizeof(LOGPALETTE) + NumAppColors * sizeof(PALETTEENTRY)];
   LOGPALETTE* m_logPalette;
   PALETTEENTRY* m_paletteEntries;
   int m_editIndex;
   CTypedPtrList<CObList,CPalette*> m_paletteStack;
   bool m_paletteDevice;

public:
   CAppPalette();
   ~CAppPalette();

   int create();
   void initColors();

   bool isPaletteDevice() { return m_paletteDevice; }
   int getEditIndex() { return m_editIndex; }
   COLORREF getEditColor() { return getColorAt(m_editIndex); }
   PALETTEENTRY* getPaletteEntry(int index);
   void setPaletteEntry(int index,COLORREF color,BYTE flags=0);
   void setEditPaletteEntry(COLORREF color);
   COLORREF getColorAt(int colorIndex);
   COLORREF getRgbAt(int colorIndex);

   void animatePaletteEntry(int index,COLORREF color);
   void realizePalette(CWnd* wnd);
   void realizePalette(CDC* dc);
   void pushPalette(CDC* dc);
   void popPalette(CDC* dc);

private:
   COLORREF getDefaultColor(int colorIndex);
};

#endif
