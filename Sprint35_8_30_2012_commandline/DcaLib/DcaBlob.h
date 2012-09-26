// $Header: /CAMCAD/5.0/dca/DcaBlob.h 4     3/19/07 4:32a Kurt Van Ness $

#if !defined(__DcaBlob_h__)
#define __DcaBlob_h__

#pragma once

#include "DcaPoint.h"
#include "DcaPoint2d.h"

//_____________________________________________________________________________
class BlobStruct
{
private:
   CBitmap* m_bitmap;

public:
   PointStruct    pnt;
   DbUnit         width;
   DbUnit         height;
   DbUnit         rotation;
   unsigned char  top:1;
   unsigned char  bottom:1;
   CString        filename;

public:
   BlobStruct();
   BlobStruct(const BlobStruct& other);
   ~BlobStruct();
   BlobStruct& operator=(const BlobStruct& other);

   CBitmap* getBitmap() { return m_bitmap; }
   void setBitmap(CBitmap* bitmap);

   CPoint2d getOrigin() const { return CPoint2d(pnt.x,pnt.y); }
   void     setOrigin(const CPoint2d& origin) { pnt.x = (DbUnit)origin.x;  pnt.y = (DbUnit)origin.y; }
   void     setOrigin(const PointStruct& origin) { pnt = origin; }

   DbUnit getWidth() const { return width; }
   void   setWidth(DbUnit width) { this->width = width; }

   DbUnit getHeight() const { return height; }
   void   setHeight(DbUnit height) { this->height = height; }

   DbUnit getRotationRadians() const { return rotation; }
   void   setRotationRadians(DbUnit radians) { rotation = radians; }

   bool getTopFlag() const { return (top != 0); }
   void setTopFlag(bool flag) { top = flag; }

   bool getBottomFlag() const { return (bottom != 0); }
   void setBottomFlag(bool flag) { bottom = flag; }

   CString getFileName() const { return filename; }
   void    setFileName(const CString& fileName);

   void transform(const CTMatrix& transformationMatrix);

   void dump(CWriteFormat& writeFormat,int depth = -1) const;

private:
   bool attach();
};

#endif
