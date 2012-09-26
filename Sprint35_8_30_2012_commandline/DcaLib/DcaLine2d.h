// $Header: /CAMCAD/DcaLib/DcaLine2d.h 3     3/09/07 5:15p Kurt Van Ness $

#if !defined(__DcaLine2d_h__)
#define __DcaLine2d_h__

#pragma once

#include "DcaPoint2d.h"

class CExtent;
class CTMatrix;

//_____________________________________________________________________________
class CLine2d
{
private:
   CPoint2d m_p0;
   CPoint2d m_p1;

public:
   CLine2d();
   CLine2d(double x0,double y0,double x1,double y1);
   CLine2d(const CPoint2d& p0,const CPoint2d& p1);
   CLine2d(const CLine2d& other);
   CLine2d& operator=(const CLine2d& other);

   const CPoint2d& get0() const { return m_p0; }
   const CPoint2d& get1() const { return m_p1; }
   const CPoint2d& getEndPoint(int endIndex) const { return (endIndex == 0) ? m_p0 : m_p1; }
   void set0(const CPoint2d& point) { m_p0 = point; }
   void set1(const CPoint2d& point) { m_p1 = point; }
   void setEndPoint(int endIndex,const CPoint2d& point) 
   { 
      if      (endIndex == 0) m_p0 = point; 
      else if (endIndex == 1) m_p1 = point;
   }
   void set(const CPoint2d& point0,const CPoint2d& point1) { m_p0 = point0;  m_p1 = point1; }

   // 
   int ccw(const CPoint2d& point) const;
   int intersect(const CLine2d& otherLine,CPoint2d& ps1,CPoint2d& ps2) const;
   bool intersects(const CLine2d& otherLine) const;
   double distanceToLineSquared(const CPoint2d& point) const;
   double distanceToLine(const CPoint2d& point) const { return sqrt(distanceToLineSquared(point)); }
   bool verticesWithinDistanceSquared(const CLine2d& otherLine,double distanceSquared) const;
   double getLengthSquared() const;
   double getLength() const { return sqrt(getLengthSquared()); }
   double getTheta() const { return CPoint2d(m_p1 - m_p0).getTheta(); }
   CExtent getExtent() const;
   void splitAt(CPoint2d point,CLine2d*& edgePart1,CLine2d*& edgePart2);
   bool mutualBisect(CLine2d& otherEdge,CLine2d*& linePart1     ,CLine2d*& linePart2,
                                        CLine2d*& otherLinePart1,CLine2d*& otherLinePart2);
   void transform(const CTMatrix& matrix);
   void expand(double deltaLength);

};

#endif
