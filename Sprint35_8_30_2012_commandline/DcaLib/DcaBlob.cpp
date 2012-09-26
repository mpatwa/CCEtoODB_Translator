// $Header: /CAMCAD/5.0/Dca/DcaBlob.cpp 4     3/19/07 4:32a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaBlob.h"
#include "DcaBasesVector.h"
#include "DcaWriteFormat.h"
#include "DcaJpeg.h"

//_____________________________________________________________________________
BlobStruct::BlobStruct()
: m_bitmap(NULL)
{
   width    = 0.f;
   height   = 0.f;
   rotation = 0.f;
   top      = 0;
   bottom   = 0;
}

BlobStruct::BlobStruct(const BlobStruct& other)
{
   m_bitmap = NULL;

   *this = other;
}

BlobStruct& BlobStruct::operator=(const BlobStruct& other)
{
   if (&other != this)
   {
      pnt      = other.pnt;
      width    = other.width;
      height   = other.height;
      rotation = other.rotation;
      top      = other.top;
      bottom   = other.bottom;

      delete m_bitmap;
      m_bitmap = NULL;

      setFileName(other.filename);
   }

   return *this;
}

BlobStruct::~BlobStruct()
{
   delete m_bitmap;
}

void BlobStruct::setBitmap(CBitmap* bitmap)
{
   delete m_bitmap;
   m_bitmap = bitmap;
}

void BlobStruct::setFileName(const CString& fileName)
{
   this->filename = fileName;

   attach();
}

bool BlobStruct::attach()
{
   bool retval = false;

   HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

   if (hBitmap == NULL)
   {
      hBitmap = LoadJPEG(filename);
   }

   if (hBitmap == NULL)
   {
      ErrorMessage(filename, "Bitmap Load Failed");
   }
   else
   {
      retval = true;

      CBitmap* bitmap = new CBitmap();

      setBitmap(bitmap);
      bitmap->Attach(hBitmap);

      BITMAP bmpInfo;
      bitmap->GetBitmap(&bmpInfo);
   }

   return retval;
}

void BlobStruct::transform(const CTMatrix& transformationMatrix)
{
   CBasesVector basesVector(pnt.x,pnt.y,1.,radiansToDegrees(rotation),false);
   basesVector.transform(transformationMatrix);

   pnt.x = (DbUnit)basesVector.getOrigin().x;
   pnt.y = (DbUnit)basesVector.getOrigin().y;

   double scale = basesVector.getScale();
   height = (DbUnit)(height * scale);
   width  = (DbUnit)(width  * scale);

   rotation = (DbUnit)degreesToRadians(basesVector.getRotation());
   //mirror = basesVector.getMirror();
}

void BlobStruct::dump(CWriteFormat& writeFormat,int depth) const
{
   writeFormat.writef(
"BlobStruct\n"
"{\n"
"   filename='%s'\n",
"   pnt=(%.3f,%.3f)\n"
"   height=%.3f\n"
"   width=%.3f\n"
"   rotation=%.3f\n"
"   top=%d\n"
"   bottom=%d\n"
"}\n",
(const char*)filename,
(double)pnt.x,(double)pnt.y,
(double)height,
(double)width,
(double)rotation,
top,
bottom);
}
