// $Header: /CAMCAD/5.0/Dca/DcaJpeg.cpp 1     3/19/07 4:28a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaJpeg.h"

// byte offsets
#define BO_BLUE     0
#define BO_GREEN    1
#define BO_RED      2

HBITMAP LoadJPEG(LPCTSTR lpszPathName)
{
   CWaitCursor w;

   JPEG_CORE_PROPERTIES image;
   ZeroMemory( &image, sizeof( JPEG_CORE_PROPERTIES ) );
   BYTE* imageData;

   TRY
   {
      if( ijlInit( &image ) != IJL_OK )
      {
         TRACE( "Cannot initialize Intel JPEG library\n" );
         AfxThrowUserException();
      }
         
      image.JPGFile = const_cast<char*>(lpszPathName);
      if( ijlRead( &image, IJL_JFILE_READPARAMS ) != IJL_OK )
      {
         TRACE( "Cannot read JPEG file header from %s file\n", image.JPGFile );
         AfxThrowUserException();
      }

      int nchannels = 3;

      int imageSize = image.JPGWidth*image.JPGHeight*nchannels;
//    int imageSize = (image.JPGHeight * ((((image.JPGWidth * nchannels) + 3) / 4) * 4));

      imageData = new BYTE[ imageSize ];
      if( imageData == NULL )
      {
         TRACE( "Cannot allocate memory for image\n" );
         AfxThrowUserException();
      }

      image.DIBWidth    = image.JPGWidth;
      image.DIBHeight   = image.JPGHeight;
      image.DIBChannels = nchannels;
      image.DIBBytes    = imageData;
//    image.DIBPadBytes = (((((image.JPGWidth * nchannels) + 3) / 4) * 4) - (image.JPGWidth * 3));

      if( ijlRead( &image, IJL_JFILE_READWHOLEIMAGE ) != IJL_OK )
      {
         TRACE( "Cannot read image data from %s file\n", image.JPGFile );
         delete[] imageData;
         AfxThrowUserException();
      }

      if( ijlFree( &image ) != IJL_OK )
         TRACE( "Cannot free Intel(R) JPEG library" );
   }


   CATCH_ALL( e )
   {
      ijlFree( &image );
      AfxMessageBox( "Error opening JPEG file" );
      return NULL;
   }
   END_CATCH_ALL


   int imageSize = image.DIBHeight*(image.DIBWidth*3+((4-((image.DIBWidth*3)&3))&3));
   BYTE *bmp = new BYTE[imageSize];
   if( bmp == NULL ) return FALSE;

   int bidx;
   int lag = 0;
   const int lagDelta = ((4-((image.DIBWidth*3)&3))&3);
   for( int j = 0; j < image.DIBHeight; j++ )
   {
      for( int i = 0; i < (int)image.DIBWidth; i++ )
      {                                                           
         bidx = j*image.DIBWidth*3 + i*3;
         bmp[ bidx+lag+BO_RED ] = imageData[ bidx+BO_RED ];
         bmp[ bidx+lag+BO_GREEN ] = imageData[ bidx+BO_GREEN ];
         bmp[ bidx+lag+BO_BLUE ] = imageData[ bidx+BO_BLUE ];
      }
      lag += lagDelta;
   }

   delete imageData;


   CBitmap temp;
   if (!temp.CreateBitmap(image.DIBWidth, image.DIBHeight, 1, 24, bmp))
   {
      delete bmp;
      return NULL;
   }

   // create bitmap from DIB
   BITMAP bitmapStruct;
   temp.GetBitmap(&bitmapStruct);
   BITMAPINFO bitmapinfoStruct;
   ZeroMemory( &bitmapinfoStruct, sizeof( BITMAPINFO ) );
   BITMAPINFOHEADER& bitmapinfoheaderStruct = bitmapinfoStruct.bmiHeader;
   ZeroMemory( &bitmapinfoheaderStruct, sizeof( BITMAPINFOHEADER ) );
   bitmapinfoheaderStruct.biSize = sizeof( BITMAPINFOHEADER );
   bitmapinfoheaderStruct.biWidth = bitmapStruct.bmWidth;
   bitmapinfoheaderStruct.biHeight = -bitmapStruct.bmHeight;                                         
   bitmapinfoheaderStruct.biCompression = BI_RGB;
   bitmapinfoheaderStruct.biPlanes = 1;
   bitmapinfoheaderStruct.biBitCount = 24;

   CPaintDC dc(AfxGetMainWnd());
   HBITMAP hBitmap = CreateDIBitmap(dc.m_hDC, &bitmapinfoheaderStruct, CBM_INIT, bmp, &bitmapinfoStruct, DIB_RGB_COLORS);
   delete bmp;
   return hBitmap;
}

