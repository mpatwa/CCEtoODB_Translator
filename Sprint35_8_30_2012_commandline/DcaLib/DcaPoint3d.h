// $Header: /CAMCAD/4.5/DcaPoint3d.h 2     7/28/09 10:11a Sharry Huang $

#if !defined(__DcaPoint3d_H__)
#define __DcaPoint3d_H__ 

#pragma once

#include "DcaFpeq.h"
#include "DcaPoint2d.h"
#include <math.h>

class CSize3d;

//_____________________________________________________________________________
class CPoint3d
{
public:
   double x;
   double y;
   double z;

// Constructors
   CPoint3d() { x = 0.; y = 0.; z = 0.; }
   CPoint3d(double initX, double initY, double initZ) { x = initX;  y = initY;  z = initZ; }
   CPoint3d(const CPoint& point) { x = point.x;  y = point.y;  z = 0.; }
   CPoint3d(const CPoint2d& point) { x = point.x;  y = point.y;  z = 0.; }
   CPoint3d(const CPoint3d& point) { x = point.x;  y = point.y;  z = point.z; }

// Operations
   double distance(const CPoint3d& otherPoint) const  { return sqrt(distanceSqr(otherPoint)); }
   double distance(double xOther,double yOther,double zOther) const  { return sqrt(distanceSqr(xOther,yOther,zOther)); }
   double distanceSqr(const CPoint3d& otherPoint) const { return distanceSqr(otherPoint.x,otherPoint.y,otherPoint.z); }
   double distanceSqr(double xOther,double yOther,double zOther) const
      { double dx = x - xOther;  double dy = y - yOther;  double dz = z - zOther;  return dx*dx + dy*dy + dz*dz; }
   double getRadius() const { return distance(x,y,z); }
   double getRadiusSqr() const { return x*x + y*y + z*z; }
   double getTheta() const { return atan2(y,x); }

// Operators returning CPoint3d values
   CPoint3d operator+(const CPoint3d& otherPoint) const
      { return CPoint3d(x + otherPoint.x,y + otherPoint.y,z + otherPoint.z); }
   CPoint3d operator+(const CSize3d& size) const;

   CPoint3d operator-(const CPoint3d& otherPoint) const
      { return CPoint3d(x - otherPoint.x,y - otherPoint.y,z - otherPoint.z); }
   CPoint3d operator-(const CSize3d& size) const;

   CPoint3d operator-() const
      { return CPoint3d(-x,-y,-z); }

// Comparison operators (used to define a sort order)
   bool operator==(const CPoint3d& otherPoint) const {  return  x == otherPoint.x && y == otherPoint.y && z == otherPoint.z;  }
   bool operator!=(const CPoint3d& otherPoint) const {  return  x != otherPoint.x || y != otherPoint.y || z != otherPoint.z;  }
   bool fpeq(const CPoint3d& otherPoint) const { return ::fpeq(x,otherPoint.x) && ::fpeq(y,otherPoint.y) && ::fpeq(z,otherPoint.z); }
   bool fpeq(const CPoint3d& otherPoint,double tolerance) const
   { 
      return ::fpeq(x,otherPoint.x,tolerance) && ::fpeq(y,otherPoint.y,tolerance) && ::fpeq(z,otherPoint.z,tolerance); 
   }

   CString getString() const;
};

//_____________________________________________________________________________
class CSize3d
{
public:
   double cx;
   double cy;
   double cz;

// Constructors
   CSize3d() { cx = 0.; cy = 0.; cz = 0.; }
   CSize3d(double initX, double initY, double initZ) { cx = initX;  cy = initY;  cz = initZ; }
   CSize3d(const CSize3d& size)   { cx = size.cx;  cy = size.cy;  cz = size.cz; }
   CSize3d(const CSize2d& size)   { cx = size.cx;  cy = size.cy;  cz = 0.; }
   CSize3d(const CSize&   size)   { cx = size.cx;  cy = size.cy;  cz = 0.; }
   CSize3d(const CPoint3d& point) { cx = point.x;  cy = point.y;  cz = point.z; }
   CSize3d(const CPoint2d& point) { cx = point.x;  cy = point.y;  cz = 0.; }

// Operations
   bool operator==(const CSize3d& size) const { return cx == size.cx && cy == size.cy && cz == size.cz; };
   bool operator!=(const CSize3d& size) const { return cx != size.cx || cy != size.cy || cz != size.cz; };
   void operator+=(const CSize3d& size) { cx += size.cx;  cy += size.cy;  cz += size.cz; };
   void operator-=(const CSize3d& size) { cx -= size.cx;  cy -= size.cy;  cz -= size.cz; };

// Operators returning CSize3d values
   CSize3d operator+(const CSize3d& size) const { return CSize3d(cx + size.cx, cy + size.cy, cz + size.cz); };
   CSize3d operator-(const CSize3d& size) const { return CSize3d(cx - size.cx, cy - size.cy, cz - size.cz); };
   CSize3d operator-() const { return CSize3d(-cx, -cy, -cz); };

// Operators returning CPoint values
   CPoint3d operator+(const CPoint3d& point) const { return CPoint3d(cx + point.x, cy + point.y, cz + point.z); }
   CPoint3d operator-(const CPoint3d& point) const { return CPoint3d(cx - point.x, cy - point.y, cz - point.z); }

   double getLengthSquared() const { return cx*cx + cy*cy + cz*cz; }
   double getLength() const { return sqrt(getLengthSquared()); }
   double getTheta() const { return atan2(cy,cx); }
};

#endif

