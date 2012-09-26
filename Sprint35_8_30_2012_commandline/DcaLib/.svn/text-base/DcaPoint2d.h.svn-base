// $Header: /CAMCAD/DcaLib/DcaPoint2d.h 3     3/09/07 5:15p Kurt Van Ness $

#if !defined(__DcaPoint2d_h__)
#define __DcaPoint2d_h__ 

#pragma once

/*
History: Point2d.h $
 * *****************  Version 5  *****************
 * User: Kvanness     Date: 2/06/00    Time: 11:53a
 * Updated in $/LibKnvPP
 * Added constructor
 * 
 * *****************  Version 4  *****************
 * User: Kvanness     Date: 9/16/99    Time: 6:55a
 * Updated in $/LibKnvPP
 * Added CSize2d class and fixed/modified CPoint2d
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 5/03/99    Time: 8:27a
 * Updated in $/LibKnvPP
 * Added declarations for distance() and distanceSqr()
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 4/22/98    Time: 11:54a
 * Updated in $/include
 * Added additive and comparitive operators
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 3/02/98    Time: 1:08p
 * Created in $/include
 * Initial Add
*/

#include "DcaFpeq.h"
#include <math.h>

class CWriteFormat;
class CTMatrix;

//_____________________________________________________________________________
typedef struct tagPOINT2d
{
   double  x;
   double  y;
} POINT2d;

typedef struct tagSIZE2d
{
   double  cx;
   double  cy;
} SIZE2d;

class CSize2d;

//_____________________________________________________________________________
class CPoint2d : public tagPOINT2d
{
public:

// Constructors
   CPoint2d() { x = 0.; y = 0.; }
   CPoint2d(double initX, double initY) { x = initX;  y = initY; }
   CPoint2d(const CPoint& point) { x = point.x;  y = point.y; }
   CPoint2d(const CPoint2d& point) { x = point.x;  y = point.y; }

// Operations
   double distance(const CPoint2d& otherPoint) const { return _hypot(x - otherPoint.x,y - otherPoint.y); }
   double distance(double xOther,double yOther) const { return _hypot(x - xOther,y - yOther); }
   double distanceSqr(const CPoint2d& otherPoint) const { return distanceSqr(otherPoint.x,otherPoint.y); }
   double distanceSqr(double xOther,double yOther) const
      { double dx = x - xOther;  double dy = y - yOther;  return dx*dx + dy*dy; }
   double getRadius() const { return _hypot(x,y); }
   double getRadiusSqr() const { return x*x + y*y; }
   double getTheta() const { return atan2(y,x); }
   void transform(const CTMatrix& matrix);

// Operators returning CPoint2d values
   CPoint2d operator+(const CPoint2d& otherPoint) const
      { return CPoint2d(x + otherPoint.x,y + otherPoint.y); }
   CPoint2d operator+(const CSize2d& size) const;

   CPoint2d operator-(const CPoint2d& otherPoint) const
      { return CPoint2d(x - otherPoint.x,y - otherPoint.y); }
   CPoint2d operator-(const CSize2d& size) const;

   CPoint2d operator-() const
      { return CPoint2d(-x,-y); }

// Operators returning CSize values

// Operators returning CRect values

// Comparison operators (used to define a sort order)
   bool operator==(const CPoint2d& otherPoint) const {  return  x == otherPoint.x && y == otherPoint.y;  }
   bool operator!=(const CPoint2d& otherPoint) const {  return  x != otherPoint.x || y != otherPoint.y;  }
   bool operator< (const CPoint2d& otherPoint) const {  return (x <  otherPoint.x) || (x == otherPoint.x && y <  otherPoint.y);  }
   bool operator> (const CPoint2d& otherPoint) const {  return (x >  otherPoint.x) || (x == otherPoint.x && y >  otherPoint.y);  }
   bool operator<=(const CPoint2d& otherPoint) const {  return (x <  otherPoint.x) || (x == otherPoint.x && y <= otherPoint.y);  }
   bool operator>=(const CPoint2d& otherPoint) const {  return (x >  otherPoint.x) || (x == otherPoint.x && y >= otherPoint.y);  }
   bool fpeq(const CPoint2d& otherPoint) const { return ::fpeq(x,otherPoint.x) && ::fpeq(y,otherPoint.y); }
   bool fpeq(const CPoint2d& otherPoint,double tolerance) const { return ::fpeq(x,otherPoint.x,tolerance) && ::fpeq(y,otherPoint.y,tolerance); }
   bool fpnear(const CPoint2d& otherPoint,double tolerance) const { return ::fpnear(x,otherPoint.x,tolerance) && ::fpnear(y,otherPoint.y,tolerance); }

   CString getString() const;

   void dump(CWriteFormat& writeFormat,int depth) const;
};

//_____________________________________________________________________________
class CSize2d : public tagSIZE2d
{
public:

// Constructors
   CSize2d() { cx = 0.; cy = 0.; }
   CSize2d(double initX, double initY) { cx = initX;  cy = initY; }
   CSize2d(const CSize2d& size) { cx = size.cx;  cy = size.cy; }
   CSize2d(const CSize& size) { cx = size.cx;  cy = size.cy; }
   CSize2d(const CPoint2d& point) { cx = point.x;  cy = point.y; }

// Operations
   bool operator==(SIZE2d size) const { return cx == size.cx && cy == size.cy; };
   bool operator!=(SIZE2d size) const { return cx != size.cx || cy != size.cy; };
   void operator+=(SIZE2d size) { cx += size.cx;  cy += size.cy; };
   void operator-=(SIZE2d size) { cx -= size.cx;  cy -= size.cy; };

// Operators returning CSize2d values
   CSize2d operator+(SIZE2d size) const { return CSize2d(cx + size.cx, cy + size.cy); };
   CSize2d operator-(SIZE2d size) const { return CSize2d(cx - size.cx, cy - size.cy); };
   CSize2d operator-() const { return CSize2d(-cx, -cy); };
   CSize2d operator*(double factor) const { return CSize2d(cx * factor,cy * factor); };
   CSize2d operator/(double factor) const { return CSize2d(cx / factor,cy / factor); };

// Operators returning CPoint values
   CPoint2d operator+(POINT2d point) const { return CPoint2d(cx + point.x, cy + point.y); }
   CPoint2d operator-(POINT2d point) const { return CPoint2d(cx - point.x, cy - point.y); }

// Operators returning CRect values
   //CRect operator+(const RECT* lpRect) const;
   //CRect operator-(const RECT* lpRect) const;

   double getLengthSquared() const { return cx*cx + cy*cy; }
   double getLength() const { return sqrt(getLengthSquared()); }
   double getTheta() const { return atan2(cy,cx); }
   double getMinSize() const { return min(cx,cy); }
   double getMaxSize() const { return max(cx,cy); }
};

#endif
