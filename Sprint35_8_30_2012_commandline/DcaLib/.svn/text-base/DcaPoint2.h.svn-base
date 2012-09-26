// $Header: /CAMCAD/DcaLib/DcaPoint2.h 1     3/23/07 5:19a Kurt Van Ness $

#if !defined(__DcaPoint2_h__)
#define __DcaPoint2_h__

#pragma once

class CPnt;
class CTMatrix;

//_____________________________________________________________________________
class Point2
{
public:
   double x;
   double y;
   double bulge;

   Point2(double X = 0.,double Y = 0.,double Bulge = 0.);
   Point2(const Point2& other);
   Point2(const CPnt& pnt);

   Point2 operator=(const Point2& other);
   Point2 operator=(const CPnt& pnt);

   void transform(const CTMatrix& matrix);
};

#endif
