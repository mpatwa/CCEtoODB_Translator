// $Header: /CAMCAD/4.6/BitMap.h 10    4/18/07 6:40p Kurt Van Ness $

/////////////////////////////////////////////////////////////////////////////
// BitmapFormat dialog

class BitmapFormat : public CDialog
{
private:
   double m_scaleFactor;
   BOOL  m_compression;

// Construction
public:
   BitmapFormat(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(BitmapFormat)
   enum { IDD = IDD_BITMAP_FORMAT };
   int      m_bits;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(BitmapFormat)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// access
public:
   double getScaleFactor() const { return m_scaleFactor; }
   int getBitsPerPixel() const;

   bool getCompressFlag() const { return (m_compression != 0); }

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(BitmapFormat)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//_____________________________________________________________________________
class CExtendedPalette
{
private:
   CPalette& m_palette;
   //char m_paletteBuf[sizeof(LOGPALETTE) + NumAppColors * sizeof(PALETTEENTRY)];
   //int m_paletteSize;
   CString m_paletteBuffer;
   LOGPALETTE* m_logPalette;
   //PALETTEENTRY* m_paletteEntries;
   //int m_editIndex;
   //CTypedPtrList<CObList,CPalette*> m_paletteStack;
   //bool m_paletteDevice;
   //int m_paletteSize;

public:
   CExtendedPalette(CPalette& palette);
   ~CExtendedPalette();

   LOGPALETTE* getLogPalette();
   int getPaletteSize();
   PALETTEENTRY* getPaletteEntries();
   void releaseLogPalette();

   //int create();
   //void initColors();

   //bool isPaletteDevice() { return m_paletteDevice; }
   //int getEditIndex() { return m_editIndex; }
   //COLORREF getEditColor() { return getColorAt(m_editIndex); }
   PALETTEENTRY* getPaletteEntry(int index);
   void setPaletteEntry(int index,COLORREF color,BYTE flags=0);
   void setEditPaletteEntry(COLORREF color);
   COLORREF getColorAt(int colorIndex);
   //COLORREF getRgbAt(int colorIndex);

   //void animatePaletteEntry(int index,COLORREF color);
   //void realizePalette(CWnd* wnd);
   //void realizePalette(CDC* dc);
   //void pushPalette(CDC* dc);
   //void popPalette(CDC* dc);

//private:
//   COLORREF getDefaultColor(int colorIndex);

   void dump(CWriteFormat& writeFormat,int depth= -1);
};
