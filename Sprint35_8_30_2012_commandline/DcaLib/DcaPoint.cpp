// $Header: /CAMCAD/DcaLib/DcaPoint.cpp 3     3/09/07 5:15p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaPoint.h"
#include "DcaPoint2d.h"
#include "DcaTMatrix.h"
#include "DcaWriteFormat.h"

//_____________________________________________________________________________
PointStruct::PointStruct()
{
   x = 0.f;
   y = 0.f;
}

PointStruct::PointStruct(DbUnit x,DbUnit y)
{
   this->x = x;
   this->y = y;
}

PointStruct::PointStruct(const PointStruct& other)
{
   x = other.x;
   y = other.y;
}

CPoint2d PointStruct::getPoint2d() const
{
   return CPoint2d(x,y);
}

void PointStruct::setOrigin(const CPoint2d& point)
{
   x = (DbUnit)point.x;
   y = (DbUnit)point.y;
}

void PointStruct::transform(const CTMatrix& transformationMatrix)
{
   transformationMatrix.transform(x,y);
}

void PointStruct::dump(CWriteFormat& writeFormat,int depth) const
{
   writeFormat.writef(
"PointStruct\n"
"{\n"
"   (x,y)=(%.3f,%.3f)\n"
"}\n",
x,y);
}
