// $Header: /CAMCAD/4.6/BitMap.cpp 13    4/18/07 6:40p Kurt Van Ness $

/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCview.h"
#include "bitmap.h"
#include "license.h"
#include "crypt.h"
#include "CCEtoODB.h"
#include "AtlImage.h"
#include "AppPalette.h"

#define HDIB HANDLE
#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')
#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))

// WIDTHBYTES performs DWORD-aligning of DIB scanlines.  The "bits"
// parameter is the bit count for the scanline (biWidth * biBitCount),
// and this macro returns the number of DWORD-aligned bytes needed 
// to hold those bits.
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

HDIB ChangeBitmapFormat(HBITMAP hBitmap, WORD wBitCount, DWORD dwCompression,HPALETTE hPal);
WORD SaveDIB(HDIB hDib, const char *lpFileName);
WORD PaletteSize(LPSTR lpDIB);
HANDLE AllocRoomForDIB(BITMAPINFOHEADER bi, HBITMAP hBitmap);
WORD DIBNumColors(LPSTR lpDIB);

/* Error constants */
enum {
      ERR_MIN = 0,                     // All error #s >= this value
      ERR_NOT_DIB = 0,                 // Tried to load a file, NOT a DIB!
      ERR_LOCK,                        // Error on a GlobalLock()!
      ERR_OPEN,                        // Error opening a file!
      ERR_INVALIDHANDLE,               // Invalid Handle
      ERR_MAX                          // All error #s < this value
     };

/*****************************************************************
 * GetClientBitmap
 */
HBITMAP CCEtoODBView::GetClientBitmap() 
{
   //HideSearchCursor();
   FlushQueue();

   CRect clientRect;
   GetClientRect(&clientRect);

   // create a DC for the screen and create
   // a memory DC compatible to screen
   CClientDC srcDC(this);
   CDC memDC;
   memDC.CreateCompatibleDC(&srcDC);

   // create a bitmap compatible with the screen DC
   HBITMAP hBitmap = CreateCompatibleBitmap(srcDC.m_hDC, clientRect.Width(), clientRect.Height()),
      hOldBitmap;

   // select new bitmap into memory DC
   hOldBitmap = (HBITMAP) memDC.SelectObject(hBitmap);

   // bitblt screen DC to memory DC
   memDC.BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &srcDC, clientRect.left, clientRect.top, SRCCOPY);

   // select old bitmap back into memory DC and get handle to bitmap of screen
   hBitmap = (HBITMAP) memDC.SelectObject(hOldBitmap);

   return hBitmap;
}

void CCEtoODBView::PrepareBitmapDC(CDC& dc,double factor)
{
   CCEtoODBView* view = this;
   CCEtoODBDoc* doc = view->GetDocument();

	CRect r;
	GetClientRect( r );

	dc.SetMapMode( MM_ISOTROPIC );
	dc.SetWindowOrg( (int)((double)GetXPos()*factor), (int)((double)GetYPos()*factor)); // scroller pos

	CSize winExt(
			(int)((doc->maxXCoord - doc->minXCoord) / view->ScaleDenom * factor),
			(int)(-(doc->maxYCoord - doc->minYCoord) / view->ScaleDenom * factor));
	dc.SetWindowExt(winExt);

	CSize viewExt((int)((double)r.right*factor), (int)((double)r.bottom*factor));
	dc.SetViewportExt(viewExt); // set size of what it is to be shown within

	dc.SetViewportOrg(0, 0);
}

HBITMAP CCEtoODBView::getClientBitmap(double bitmapFactor,int bitsPerPixel) 
{
   CCEtoODBView* view = this;
   CCEtoODBDoc* doc = view->GetDocument();

   //HideSearchCursor();
   FlushQueue();

	int sclNum = -1;
	double sclDenom = -1.0;

	if (view->ScaleNum > 1)
	{
		// if we are zoomed out passed 1to1, then only save a bitmap that is at 1to1
		sclNum = view->ScaleNum;
		sclDenom = view->ScaleDenom;
		
		view->ScaleNum = 1;
		view->ScaleDenom = 1.0;
	}

	doc->SetMaxCoords();
	double factor = bitmapFactor * (doc->maxXCoord - doc->minXCoord) / (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / view->ScaleNum;

	CDC memDC;
	memDC.CreateCompatibleDC(NULL);
	PrepareBitmapDC(memDC, bitmapFactor);

	CSize size;
	CRect pageRect((int)floor(doc->getSettings().getXmin() * factor/bitmapFactor), (int)floor(doc->getSettings().getYmax() * factor/bitmapFactor),
						(int)floor(doc->getSettings().getXmax() * factor/bitmapFactor), (int)floor(doc->getSettings().getYmin() * factor/bitmapFactor));
	memDC.LPtoDP(&pageRect);

	CRect clientRect;
	GetClientRect(&clientRect);

	// make sure we only include the page size with atleast the client area
	size.cx = (int)((double)min(clientRect.Width() , pageRect.Width() ) * bitmapFactor);
	size.cy = (int)((double)min(clientRect.Height(), pageRect.Height()) * bitmapFactor);

	BITMAPINFO bi;

	ZeroMemory( &bi.bmiHeader, sizeof(BITMAPINFOHEADER) );
	bi.bmiHeader.biWidth = size.cx;     // Set size you need
	bi.bmiHeader.biHeight = size.cy;    // Set size you need
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biSizeImage = 0;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biBitCount = bitsPerPixel; // Can be 8, 16, 24, 32 bpp or 
										// other number
	bi.bmiHeader.biSizeImage = 0;
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;
	VOID *pvBits;
	HBITMAP hBitmap= CreateDIBSection( memDC.m_hDC, &bi, DIB_RGB_COLORS, &pvBits, NULL, 0 ),
      hOldBitmap;

   // select new bitmap into memory DC
   hOldBitmap = (HBITMAP) memDC.SelectObject(hBitmap);

   // bitblt screen DC to memory DC

	//pInfo.isMonochrome = FALSE;
	//Draw(doc, &memDC, this);
	//pInfo.isMonochrome = FALSE;

   //double viewWidth  = pageRect.Width()  * bitmapFactor;
   //double viewHeight = pageRect.Height() * bitmapFactor;

   //view->DrawFiles(&memDC,viewWidth,viewHeight);

   double originalScaleFactor = view->getScaleFactor();

   //view->Draw(&memDC,bitmapFactor,true);

   //view->setScaleFactor(originalScaleFactor);
		
	if (sclNum != -1 && sclDenom != -1.0)
	{
		view->ScaleNum = sclNum;
		view->ScaleDenom = sclDenom;
	}

   // select old bitmap back into memory DC and get handle to bitmap of screen
   hBitmap = (HBITMAP) memDC.SelectObject(hOldBitmap);

	memDC.DeleteDC();

   return hBitmap;
}

bool CCEtoODBView::WriteImage(const CString& filename, double imageFactor,int bitsPerPixel,bool compressFlag)
{
   bool retval = false;

	HBITMAP hBitmap = getClientBitmap(imageFactor,bitsPerPixel);

   CPalette palette;
   getScreenPalette(palette,bitsPerPixel);

   DWORD compression = BI_RGB;

   switch (bitsPerPixel)
   {
   case 1:
   case 24:
      break;
   case 4:
      if (compressFlag)
      {
         compression = BI_RLE4;
      }

      break;
   case 8:
   default:
      bitsPerPixel = 8;

      if (compressFlag)
      {
         compression = BI_RLE8;
      }

      break;
   }

   // change palette to black and white for monochrome bitmaps
   if (bitsPerPixel <= 8)
   {
      CImage image;
      image.Attach(hBitmap);

      if (image.IsDIBSection())
      {
         int colorTableSize = image.GetMaxColorTableEntries();

         if (colorTableSize > 1 && bitsPerPixel == 1)
         {
            // for monochrome bitmaps, set the background color to black and the foreground color to white
            colorTableSize = 2;
         }
         else
         {
            // for 16 color and 256 color bitmaps, set the background color to black
            colorTableSize = 1;
         }

         RGBQUAD* colorTable = new RGBQUAD[colorTableSize];

         image.GetColorTable(0,colorTableSize,colorTable);

         for (int colorIndex = 0;colorIndex < colorTableSize;colorIndex++)
         {
            // index 0 - background color
            // index 1 - foreground color

            int componentColor = ((colorIndex == 0) ? 0 : 255);  // white foreground on black background

            colorTable[colorIndex].rgbRed      = componentColor;
            colorTable[colorIndex].rgbGreen    = componentColor;
            colorTable[colorIndex].rgbBlue     = componentColor;
            colorTable[colorIndex].rgbReserved = 0;
         }

         image.SetColorTable(0,colorTableSize,colorTable);

         delete[] colorTable;
      }

      HBITMAP detachedBitmap = image.Detach();
   }

   HDIB hDIB = ChangeBitmapFormat(hBitmap, bitsPerPixel, compression, palette);

   if (hDIB != NULL)
   {
      int err = SaveDIB(hDIB,filename);

      switch (err)
      {
      case  ERR_INVALIDHANDLE:
         ErrorMessage("Error in SaveDIB()", "ERROR: INVALIDHANDLE", MB_OK | MB_ICONSTOP);
         break;
      case  ERR_OPEN:
         ErrorMessage("Error in SaveDIB()", "ERROR: ERR_OPEN", MB_OK | MB_ICONSTOP);
         break;
      case  ERR_LOCK:
         ErrorMessage("Error in SaveDIB()", "ERROR: ERR_LOCK", MB_OK | MB_ICONSTOP);
         break;
      default:
         retval = true;
         break;
      }
   }

	//CImage image;
	//image.Attach(hBitmap);
	//
	//bool retValue = (image.Save(filename) == S_OK);

	//image.Detach();
	//DeleteObject(hBitmap);

	return retval;
}

/*****************************************************************
 * OnClipboard
 */
/*void CCEtoODBView::OnClipboard() 
{
   if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint)) 
   {
#ifdef SHAREWARE
      ErrorMessage("To purchase a copy of CAMCAD please contact you reseller or email Rsi_sales@mentor.com", "CAMCAD Shareware does not support Printing!");
      CString file = getApp().getCamcadExeFolderPath();
      file += "order.txt";
      Notepad(file);
#else
      CString errorMessage;
      errorMessage.Format("You do not have a \"%s\" license!", getCamcadProductDisplayString(camcadProductPrint)); 
      ErrorAccess(errorMessage);
#endif
      return;
   }

   HBITMAP hBitmap = GetClientBitmap();

   OpenClipboard();
   EmptyClipboard();
   SetClipboardData(CF_BITMAP, hBitmap);
   CloseClipboard();

   WinExec("CLIPBRD.EXE", SW_SHOWNORMAL);
   ShowSearchCursor();
}*/

void CCEtoODBView::getScreenPalette(CPalette& palette,int bitsPerPixel)
{
   CClientDC dc(this);

   int numColors = dc.GetDeviceCaps(BITSPIXEL);
   LOGPALETTE* logPalette = (LOGPALETTE *)calloc(1, sizeof(LOGPALETTE) + numColors * sizeof(PALETTEENTRY));
   logPalette->palVersion = 0x300;
   logPalette->palNumEntries = numColors;

   COLORREF color = colorRed;

   for (int i = 0; i < numColors; i++)
   {
      if (bitsPerPixel != 1)
      {
         color = PALETTEINDEX(i);
      }

      logPalette->palPalEntry[i].peRed   = GetRValue(color);
      logPalette->palPalEntry[i].peGreen = GetGValue(color);
      logPalette->palPalEntry[i].peBlue  = GetBValue(color);
   }

   palette.CreatePalette(logPalette);

   free(logPalette);
}

/*****************************************************************
 * OnWriteBitmap
 */
/*void CCEtoODBView::OnWriteBitmap()
{
   if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint)) 
   {
#ifdef SHAREWARE
      ErrorMessage("To purchase a copy of CAMCAD please contact you reseller or email Rsi_sales@mentor.com", "CAMCAD Shareware does not support Bitmap!");
      Notepad("ORDER.TXT");
#else
      CString errorMessage;
      errorMessage.Format("You do not have a \"%s\" license!", getCamcadProductDisplayString(camcadProductPrint)); 
      ErrorAccess(errorMessage);
#endif
      return;
   }

   CFileDialog FileDialog(FALSE, "BMP", "*.bmp",
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Bitmap File (*.bmp)|*.BMP|All Files (*.*)|*.*||", NULL);

   if (FileDialog.DoModal() != IDOK) 
   {
      ShowSearchCursor();
      return;
   }

   BitmapFormat bitmapDialog;
   bitmapDialog.m_bits = 2;

   if (bitmapDialog.DoModal() != IDOK)
   {
      ShowSearchCursor();
      return;
   }

   bool compressFlag = bitmapDialog.getCompressFlag();

   WriteImage(FileDialog.GetFileName(),bitmapDialog.getScaleFactor(),bitmapDialog.getBitsPerPixel(),compressFlag);
   return;

#ifdef oldOnWriteBitmap
   HBITMAP hBitmap = GetClientBitmap();

   //HPALETTE hPal = GetSystemPalette();
   CPalette pal;
   CClientDC dc(this);
   int numColors = dc.GetDeviceCaps(BITSPIXEL);
   LOGPALETTE *logPalette = (LOGPALETTE *)calloc(1, sizeof(LOGPALETTE) + numColors * sizeof(PALETTEENTRY));
   logPalette->palVersion = 0x300;
   logPalette->palNumEntries = numColors;

   for (int i = 0; i < numColors; i++)
   {
      logPalette->palPalEntry[i].peRed   = GetRValue(PALETTEINDEX(i));
      logPalette->palPalEntry[i].peGreen = GetGValue(PALETTEINDEX(i));
      logPalette->palPalEntry[i].peBlue  = GetBValue(PALETTEINDEX(i));
   }

   pal.CreatePalette(logPalette);

   WORD bits = 8;
   DWORD compression = BI_RGB;

   switch (bitmapDialog.m_bits)
   {
   case 0:
      bits = 1;

      break;
   case 1:
      bits = 4;

      if (bitmapDialog.m_compression)
         compression = BI_RLE4;

      break;
   case 2:
      bits = 8;

      if (bitmapDialog.m_compression)
         compression = BI_RLE8;

      break;
   case 3:
      bits = 24;

      break;
   }

   HDIB hDIB = ChangeBitmapFormat(hBitmap, bits, compression, pal);

   if (hDIB == NULL)
      return;

   int err = SaveDIB(hDIB, FileDialog.GetPathName());

   switch (err)
   {
   case  ERR_INVALIDHANDLE:
      ErrorMessage("Error in SaveDIB()", "ERROR: INVALIDHANDLE", MB_OK | MB_ICONSTOP);
      break;
   case  ERR_OPEN:
      ErrorMessage("Error in SaveDIB()", "ERROR: ERR_OPEN", MB_OK | MB_ICONSTOP);
      break;
   case  ERR_LOCK:
      ErrorMessage("Error in SaveDIB()", "ERROR: ERR_LOCK", MB_OK | MB_ICONSTOP);
      break;
   }

   ShowSearchCursor();
#endif
}*/

/////////////////////////////////////////////////////////////////////////////
// BitmapFormat dialog
BitmapFormat::BitmapFormat(CWnd* pParent /*=NULL*/)
: CDialog(BitmapFormat::IDD, pParent)
, m_scaleFactor(1.0)
{
   //{{AFX_DATA_INIT(BitmapFormat)
   m_bits = -1;
   m_compression = FALSE;
   //}}AFX_DATA_INIT
}

void BitmapFormat::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(BitmapFormat)
   DDX_Radio(pDX, IDC_1BIT, m_bits);
   DDX_Check(pDX, IDC_COMPRESSION, m_compression);
   //}}AFX_DATA_MAP
   DDX_Text(pDX, IDC_ScaleFactor, m_scaleFactor);
	DDV_MinMaxDouble(pDX, m_scaleFactor, .001, 1000);
}

int BitmapFormat::getBitsPerPixel() const
{
   int bitsPerPixel = 1;

   switch (m_bits)
   {
   case 0:  bitsPerPixel =  1;  break;
   case 1:  bitsPerPixel =  4;  break;
   case 2:  bitsPerPixel =  8;  break;
   case 3:  bitsPerPixel = 24;  break;
   }

   return bitsPerPixel;
}

BEGIN_MESSAGE_MAP(BitmapFormat, CDialog)
   //{{AFX_MSG_MAP(BitmapFormat)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*************************************************************************
 *
 * ChangeBitmapFormat()
 *
 * Parameter:
 *
 * HBITMAP          - handle to a bitmap
 *
 * WORD             - desired bits per pixel
 *
 * DWORD            - desired compression format
 *
 * HPALETTE         - handle to palette
 *
 * Return Value:
 *
 * HDIB             - handle to the new DIB if successful, else NULL
 *
 * Description:
 *
 * This function will convert a bitmap to the specified bits per pixel
 * and compression format. The bitmap and it's palette will remain
 * after calling this function.
 *
 ************************************************************************/
HDIB ChangeBitmapFormat(HBITMAP hBitmap, WORD wBitCount, DWORD dwCompression,HPALETTE hPal)
{
   HDC                hDC;          // Screen DC
   HDIB               hNewDIB=NULL; // Handle to new DIB
   BITMAP             Bitmap;       // BITMAP data structure
   BITMAPINFOHEADER   bi;           // Bitmap info. header
   LPBITMAPINFOHEADER lpbi;         // Pointer to bitmap header
   HPALETTE           hOldPal=NULL; // Handle to palette
   WORD               NewBPP;       // New bits per pixel
   DWORD              NewComp;      // New compression format

    // Check for a valid bitmap handle

   if (!hBitmap)
   {
      ErrorMessage("Error in ChangeBitmapFormat()", "ERROR: No hBitmap", MB_OK | MB_ICONSTOP);
      return NULL;
   }

   // Validate wBitCount and dwCompression
   // They must match correctly (i.e., BI_RLE4 and 4 BPP or
   // BI_RLE8 and 8BPP, etc.) or we return failure
    
   if (wBitCount == 0)
   {
      NewComp = dwCompression;

      if (NewComp == BI_RLE4)
         NewBPP = 4;
      else if (NewComp == BI_RLE8)
         NewBPP = 8;
      else // Not enough info */
      {
         ErrorMessage("Error in ChangeBitmapFormat()", "ERROR: No wBitCount", MB_OK | MB_ICONSTOP);
         return NULL;
      }
   }
   else if (wBitCount == 1 && dwCompression == BI_RGB)
   {
      NewBPP = wBitCount;
      NewComp = BI_RGB;
   }
   else if (wBitCount == 4)
   {
      NewBPP = wBitCount;

      if (dwCompression == BI_RGB || dwCompression == BI_RLE4)
         NewComp = dwCompression;
      else
      {
         ErrorMessage("Error in ChangeBitmapFormat()", "ERROR: No wBitCount4", MB_OK | MB_ICONSTOP);
         return NULL;
      }
   }
   else if (wBitCount == 8)
   {
      NewBPP = wBitCount;

      if (dwCompression == BI_RGB || dwCompression == BI_RLE8)
         NewComp = dwCompression;
      else
      {
         ErrorMessage("Error in ChangeBitmapFormat()", "ERROR: No wBitCount8", MB_OK | MB_ICONSTOP);
         return NULL;
      }
   }
   else if (wBitCount == 24 && dwCompression == BI_RGB)
   {
      NewBPP = wBitCount;
      NewComp = BI_RGB;
   }
   else
   {
      ErrorMessage("Error in ChangeBitmapFormat()", "ERROR: No wBitCount24", MB_OK | MB_ICONSTOP);
      return NULL;
   }

   // Get info about the bitmap
   GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);

   // Fill in the BITMAPINFOHEADER appropriately
   bi.biSize               = sizeof(BITMAPINFOHEADER);
   bi.biWidth              = Bitmap.bmWidth;
   bi.biHeight             = Bitmap.bmHeight;
   bi.biPlanes             = 1;
   bi.biBitCount           = NewBPP;
   bi.biCompression        = NewComp;
   bi.biSizeImage          = 0;
   bi.biXPelsPerMeter      = 0;
   bi.biYPelsPerMeter      = 0;
   bi.biClrUsed            = 0;
   bi.biClrImportant       = 0;

   // Go allocate room for the new DIB
   hNewDIB = AllocRoomForDIB(bi, hBitmap);

   if (!hNewDIB)
   {
      ErrorMessage("Error in ChangeBitmapFormat()", "ERROR: No AllocRoom", MB_OK | MB_ICONSTOP);
      return NULL;
   }

   // Get a pointer to the new DIB
   lpbi = (LPBITMAPINFOHEADER)GlobalLock(hNewDIB);

   // If we have a palette, get a DC and select/realize it
   if (hPal)
   {
      hDC  = GetDC(NULL);
      hOldPal = SelectPalette(hDC, hPal, FALSE);
      RealizePalette(hDC);
   }

   // Call GetDIBits and get the new DIB bits
   if (!GetDIBits(hDC, hBitmap, 0, (UINT) lpbi->biHeight, (LPSTR)lpbi +
         (WORD)lpbi->biSize + PaletteSize((LPSTR)lpbi), (LPBITMAPINFO)lpbi,
         DIB_RGB_COLORS))
   {
      LPVOID lpMsgBuf;
      FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
         NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
         (LPTSTR) &lpMsgBuf, 0, NULL);

      GlobalUnlock(hNewDIB);
      GlobalFree(hNewDIB);
      hNewDIB = NULL;

      ErrorMessage("Error in GetDIBits()", (LPTSTR)lpMsgBuf, MB_OK | MB_ICONSTOP);
      LocalFree( lpMsgBuf );
   }

    // Clean up and return

    if (hOldPal)
    {
        SelectPalette(hDC, hOldPal, TRUE);
        RealizePalette(hDC);
        ReleaseDC(NULL, hDC);
    }

    // Unlock the new DIB's memory block

    if (hNewDIB)
        GlobalUnlock(hNewDIB);

    return hNewDIB;
}

/*************************************************************************
 *
 * SaveDIB()
 *
 * Saves the specified DIB into the specified file name on disk.  No
 * error checking is done, so if the file already exists, it will be
 * written over.
 *
 * Parameters:
 *
 * HDIB hDib - Handle to the dib to save
 *
 * LPSTR lpFileName - pointer to full pathname to save DIB under
 *
 * Return value: 0 if successful, or one of:
 *        ERR_INVALIDHANDLE
 *        ERR_OPEN
 *        ERR_LOCK
 *
 *************************************************************************/
WORD SaveDIB(HDIB hDib, const char *lpFileName)
{
    BITMAPFILEHEADER    bmfHdr;     // Header for Bitmap file
    LPBITMAPINFOHEADER  lpBI;       // Pointer to DIB info structure
    HANDLE              fh;         // file handle for opened file
    DWORD               dwDIBSize;
    DWORD               dwWritten;

    if (!hDib)
        return ERR_INVALIDHANDLE;

    fh = CreateFile(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (fh == INVALID_HANDLE_VALUE)
        return ERR_OPEN;

    // Get a pointer to the DIB memory, the first of which contains
    // a BITMAPINFO structure

    lpBI = (LPBITMAPINFOHEADER)GlobalLock(hDib);

    if (!lpBI)
    {
        CloseHandle(fh);
        return ERR_LOCK;
    }

    // Check to see if we're dealing with an OS/2 DIB.  If so, don't
    // save it because our functions aren't written to deal with these
    // DIBs.

    if (lpBI->biSize != sizeof(BITMAPINFOHEADER))
    {
        GlobalUnlock(hDib);
        CloseHandle(fh);
        return ERR_NOT_DIB;
    }

    // Fill in the fields of the file header

    // Fill in file type (first 2 bytes must be "BM" for a bitmap)

    bmfHdr.bfType = DIB_HEADER_MARKER;  // "BM"

    // Calculating the size of the DIB is a bit tricky (if we want to
    // do it right).  The easiest way to do this is to call GlobalSize()
    // on our global handle, but since the size of our global memory may have
    // been padded a few bytes, we may end up writing out a few too
    // many bytes to the file (which may cause problems with some apps,
    // like HC 3.0).
    //
    // So, instead let's calculate the size manually.
    //
    // To do this, find size of header plus size of color table.  Since the
    // first DWORD in both BITMAPINFOHEADER and BITMAPCOREHEADER conains
    // the size of the structure, let's use this.

    // Partial Calculation

    dwDIBSize = *(LPDWORD)lpBI + PaletteSize((LPSTR)lpBI);  

    // Now calculate the size of the image

    // It's an RLE bitmap, we can't calculate size, so trust the biSizeImage
    // field

    if ((lpBI->biCompression == BI_RLE8) || (lpBI->biCompression == BI_RLE4))
    {
        dwDIBSize += lpBI->biSizeImage;
    }
    else
    {
        DWORD dwBmBitsSize;  // Size of Bitmap Bits only

        // It's not RLE, so size is Width (DWORD aligned) * Height

        dwBmBitsSize = WIDTHBYTES((lpBI->biWidth)*((DWORD)lpBI->biBitCount)) *
                lpBI->biHeight;

        dwDIBSize += dwBmBitsSize;

        // Now, since we have calculated the correct size, why don't we
        // fill in the biSizeImage field (this will fix any .BMP files which 
        // have this field incorrect).

        lpBI->biSizeImage = dwBmBitsSize;
    }


    // Calculate the file size by adding the DIB size to sizeof(BITMAPFILEHEADER)
                   
    bmfHdr.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);
    bmfHdr.bfReserved1 = 0;
    bmfHdr.bfReserved2 = 0;

    // Now, calculate the offset the actual bitmap bits will be in
    // the file -- It's the Bitmap file header plus the DIB header,
    // plus the size of the color table.
    
    bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + lpBI->biSize +
            PaletteSize((LPSTR)lpBI);

    // Write the file header

    WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

    // Write the DIB header and the bits -- use local version of
    // MyWrite, so we can write more than 32767 bytes of data
    
    WriteFile(fh, (LPSTR)lpBI, dwDIBSize, &dwWritten, NULL);

    GlobalUnlock(hDib);
    CloseHandle(fh);

    if (dwWritten == 0)
        return ERR_OPEN; // oops, something happened in the write
    else
        return 0; // Success code
}

/*************************************************************************
 *
 * PaletteSize()
 *
 * Parameter:
 *
 * LPSTR lpDIB      - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * WORD             - size of the color palette of the DIB
 *
 * Description:
 *
 * This function gets the size required to store the DIB's palette by
 * multiplying the number of colors by the size of an RGBQUAD (for a
 * Windows 3.0-style DIB) or by the size of an RGBTRIPLE (for an OS/2-
 * style DIB).
 *
 ************************************************************************/
WORD PaletteSize(LPSTR lpDIB)
{
    // calculate the size required by the palette
    if (IS_WIN30_DIB (lpDIB))
        return (DIBNumColors(lpDIB) * sizeof(RGBQUAD));
    else
        return (DIBNumColors(lpDIB) * sizeof(RGBTRIPLE));
}

/*************************************************************************
 *
 * AllocRoomForDIB()
 *
 * Parameters:
 *
 * BITMAPINFOHEADER - bitmap info header stucture
 *
 * HBITMAP          - handle to the bitmap
 *
 * Return Value:
 *
 * HDIB             - handle to memory block
 *
 * Description:
 *
 *  This routine takes a BITMAPINOHEADER, and returns a handle to global
 *  memory which can contain a DIB with that header.  It also initializes
 *  the header portion of the global memory.  GetDIBits() is used to determine
 *  the amount of room for the DIB's bits.  The total amount of memory
 *  needed = sizeof(BITMAPINFOHEADER) + size of color table + size of bits.
 *
 ************************************************************************/
HANDLE AllocRoomForDIB(BITMAPINFOHEADER bi, HBITMAP hBitmap)
{
    DWORD               dwLen;
    HANDLE              hDIB;
    HDC                 hDC;
    LPBITMAPINFOHEADER  lpbi;
    HANDLE              hTemp;

    // Figure out the size needed to hold the BITMAPINFO structure
    // (which includes the BITMAPINFOHEADER and the color table).

    dwLen = bi.biSize + PaletteSize((LPSTR) &bi);
    hDIB  = GlobalAlloc(GHND,dwLen);

    // Check that DIB handle is valid

    if (!hDIB)
        return NULL;

    // Set up the BITMAPINFOHEADER in the newly allocated global memory,
    // then call GetDIBits() with lpBits = NULL to have it fill in the
    // biSizeImage field for us.

    lpbi  = (LPBITMAPINFOHEADER)GlobalLock(hDIB);
    *lpbi = bi;

    hDC   = GetDC(NULL);

    GetDIBits(hDC, hBitmap, 0, (UINT) bi.biHeight, NULL, (LPBITMAPINFO)lpbi,
            DIB_RGB_COLORS);
    ReleaseDC(NULL, hDC);

    // If the driver did not fill in the biSizeImage field,
    // fill it in -- NOTE: this is a bug in the driver!
    
    if (lpbi->biSizeImage == 0)
        lpbi->biSizeImage = WIDTHBYTES((DWORD)lpbi->biWidth *
                lpbi->biBitCount) * lpbi->biHeight;

    // Get the size of the memory block we need

    dwLen = lpbi->biSize + PaletteSize((LPSTR) &bi) + lpbi->biSizeImage;

    // Unlock the memory block

    GlobalUnlock(hDIB);

    // ReAlloc the buffer big enough to hold all the bits 

    if (hTemp = GlobalReAlloc(hDIB,dwLen,0))
    {
        return hTemp;
    }
    else
    {
        // Else free memory block and return failure

        GlobalFree(hDIB);
        return NULL;
    }
}

/*************************************************************************
 *
 * DIBNumColors()
 *
 * Parameter:
 *
 * LPSTR lpDIB      - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * WORD             - number of colors in the color table
 *
 * Description:
 *
 * This function calculates the number of colors in the DIB's color table
 * by finding the bits per pixel for the DIB (whether Win3.0 or OS/2-style
 * DIB). If bits per pixel is 1: colors=2, if 4: colors=16, if 8: colors=256,
 * if 24, no colors in color table.
 *
 ************************************************************************/
WORD DIBNumColors(LPSTR lpDIB)
{
    WORD wBitCount;  // DIB bit count

    // If this is a Windows-style DIB, the number of colors in the
    // color table can be less than the number of bits per pixel
    // allows for (i.e. lpbi->biClrUsed can be set to some value).
    // If this is the case, return the appropriate value.
    

    if (IS_WIN30_DIB(lpDIB))
    {
        DWORD dwClrUsed;

        dwClrUsed = ((LPBITMAPINFOHEADER)lpDIB)->biClrUsed;
        if (dwClrUsed)

        return (WORD)dwClrUsed;
    }

    // Calculate the number of colors in the color table based on
    // the number of bits per pixel for the DIB.
    
    if (IS_WIN30_DIB(lpDIB))
        wBitCount = ((LPBITMAPINFOHEADER)lpDIB)->biBitCount;
    else
        wBitCount = ((LPBITMAPCOREHEADER)lpDIB)->bcBitCount;

    // return number of colors based on bits per pixel

    switch (wBitCount)
    {
        case 1:
            return 2;

        case 4:
            return 16;

        case 8:
            return 256;

        default:
            return 0;
    }
}

//_____________________________________________________________________________
CExtendedPalette::CExtendedPalette(CPalette& palette)
: m_palette(palette)
, m_logPalette(NULL)
//, m_paletteSize(0)
{
   //CDC* dc = AfxGetMainWnd()->GetDC();

   //m_paletteDevice = ((dc->GetDeviceCaps(RASTERCAPS) & RC_PALETTE) != 0);

   //m_logPalette = (LOGPALETTE*)&m_paletteBuf;
   //m_logPalette->palVersion    = 0x300;
   //m_logPalette->palNumEntries = NumAppColors;
   //m_paletteEntries = &(m_logPalette->palPalEntry[0]);

   //m_editIndex = NumAppColors - 1;

   //initColors();
}

CExtendedPalette::~CExtendedPalette()
{
}

LOGPALETTE* CExtendedPalette::getLogPalette()
{
   if (m_logPalette == NULL)
   {
      int paletteSize = getPaletteSize();
      int bufferSize = sizeof(LOGPALETTE) + paletteSize * sizeof(PALETTEENTRY);

      m_logPalette = (LOGPALETTE*)m_paletteBuffer.GetBufferSetLength(bufferSize);
      m_logPalette->palVersion    = 0x300;
      m_logPalette->palNumEntries = paletteSize;

      PALETTEENTRY* paletteEntries = &(m_logPalette->palPalEntry[0]);

      m_palette.GetPaletteEntries(0,paletteSize,paletteEntries);

      //m_paletteEntries = &(m_logPalette->palPalEntry[0]);
   }

   return m_logPalette;
}

void CExtendedPalette::releaseLogPalette()
{
   m_logPalette = NULL;
}

int CExtendedPalette::getPaletteSize()
{
   return m_palette.GetEntryCount();
}

PALETTEENTRY* CExtendedPalette::getPaletteEntries()
{
   PALETTEENTRY* paletteEntries = &(getLogPalette()->palPalEntry[0]);

   return paletteEntries;
}

PALETTEENTRY* CExtendedPalette::getPaletteEntry(int index)
{
   //ASSERT(index >= 0 && index < NumAppColors);

   PALETTEENTRY* paletteEntry = NULL;
   
   if (index >= 0 && index < getPaletteSize())
   {
      paletteEntry = &(getPaletteEntries()[index]);
   }

   return paletteEntry;
}

void CExtendedPalette::setPaletteEntry(int index,COLORREF color,BYTE flags)
{
   PALETTEENTRY* paletteEntry = getPaletteEntry(index);

   if (paletteEntry != NULL)
   {
      paletteEntry->peRed   = GetRValue(color);
      paletteEntry->peGreen = GetGValue(color);
      paletteEntry->peBlue  = GetBValue(color);
      paletteEntry->peFlags = flags;
   }
}

COLORREF CExtendedPalette::getColorAt(int index)
{
   COLORREF retval = colorBlack;
   
   PALETTEENTRY* paletteEntry = getPaletteEntry(index);

   if (paletteEntry != NULL)
   {
      retval = RGB(paletteEntry->peRed,paletteEntry->peGreen,paletteEntry->peBlue);
   }

   return retval;
}

void CExtendedPalette::dump(CWriteFormat& writeFormat,int depth)
{
   writeFormat.writef(
"CExtendedPalette\n"
"{\n"
);

   writeFormat.pushHeader("   ");

   for (int index = 0;index < getPaletteSize();index++)
   {
      PALETTEENTRY* paletteEntry = getPaletteEntry(index);

      writeFormat.writef("index=%d, color=0x%08x, red=0x%02x, green=0x%02x, blue=0x%02x, flags=0x%02x\n",
         index,
         RGB(paletteEntry->peRed,paletteEntry->peGreen,paletteEntry->peBlue),
         paletteEntry->peRed,
         paletteEntry->peGreen,
         paletteEntry->peBlue,
         paletteEntry->peFlags);
   }

   writeFormat.popHeader();

   writeFormat.writef(
"}\n"
);
}

// end BITMAP.CPP
