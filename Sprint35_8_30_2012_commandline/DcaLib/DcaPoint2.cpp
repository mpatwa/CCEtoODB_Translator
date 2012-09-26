// $Header: /CAMCAD/DcaLib/DcaPoint2.cpp 1     3/23/07 5:19a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaPoint2.h"
#include "DcaPnt.h"
#include "DcaTMatrix.h"

//_____________________________________________________________________________
Point2::Point2(double X,double Y,double Bulge)
: x(X)
, y(Y)
, bulge(Bulge)
{
}

Point2::Point2(const Point2& other)
: x(other.x)
, y(other.y)
, bulge(other.bulge)
{
}

Point2::Point2(const CPnt& pnt)
: x(pnt.x)
, y(pnt.y)
, bulge(pnt.bulge)
{
}

Point2 Point2::operator=(const Point2& other)
{
   x     = other.x;
   y     = other.y;
   bulge = other.bulge;

   return *this;
}

Point2 Point2::operator=(const CPnt& pnt)
{
   x     = pnt.x;
   y     = pnt.y;
   bulge = pnt.bulge;

   return *this;
}

void Point2::transform(const CTMatrix& matrix)
{
   matrix.transform(x,y);

   if (matrix.getMirror())
   {
      bulge = -bulge;
   }
}

