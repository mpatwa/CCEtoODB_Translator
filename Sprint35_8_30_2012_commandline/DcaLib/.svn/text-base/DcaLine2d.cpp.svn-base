// $Header: /CAMCAD/DcaLib/DcaLine2d.cpp 3     3/09/07 5:15p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaLine2d.h"
#include "DcaTMatrix.h"
#include "DcaExtent.h"

//_____________________________________________________________________________
CLine2d::CLine2d()
{
}

CLine2d::CLine2d(double x0,double y0,double x1,double y1) :
   m_p0(x0,y0),m_p1(x1,y1)
{
}

CLine2d::CLine2d(const CPoint2d& p0,const CPoint2d& p1) :
   m_p0(p0),m_p1(p1)
{
}

CLine2d::CLine2d(const CLine2d& other) :
   m_p0(other.m_p0),m_p1(other.m_p1)
{
}

CLine2d& CLine2d::operator=(const CLine2d& other)
{
   m_p0 = other.m_p0;
   m_p1 = other.m_p1;

   return *this;
}

double CLine2d::getLengthSquared() const
{
   double dx = m_p1.x  - m_p0.x;
   double dy = m_p1.y  - m_p0.y;

   return dx*dx + dy*dy;
}

CExtent CLine2d::getExtent() const
{
   CExtent extent;
   extent.update(m_p0);
   extent.update(m_p1);

   return extent;
}

void CLine2d::transform(const CTMatrix& matrix)
{
   matrix.transform(m_p0);
   matrix.transform(m_p1);
}

void CLine2d::expand(double deltaLength)
{
   double length = getLength();

   if (length > 0.)
   {
      CPoint2d delta = m_p1 - m_p0;
      double scale = deltaLength / getLength();

      if (scale < -.5)
      {
         scale = -.4;
      }

      delta.x *= scale;
      delta.y *= scale;

      m_p0 = m_p0 - delta;
      m_p1 = m_p1 + delta;
   }
}

// returns  1 if point is ccw from line.
// returns -1 if point is cw from line.
// if point is collinear with line, then:
//    returns  0 if point is between m_p0 and m_p1 or coincident with one of them
//    returns -1 if m_p0 is between point and m_p1
//    returns  1 if m_p1 is between m_p0 and point
int CLine2d::ccw(const CPoint2d& point) const
{
   int retval = 0;

   double dx1 = m_p1.x  - m_p0.x;
   double dy1 = m_p1.y  - m_p0.y;
   double dx2 = point.x - m_p0.x;
   double dy2 = point.y - m_p0.y;

   double dx1dy2 = dx1*dy2;
   double dy1dx2 = dy1*dx2;

   if (dx1dy2 > dy1dx2)
   {
      retval = 1;  // point is ccw from line.
   }
   else if (dx1dy2 < dy1dx2)
   {
      retval = -1; // point is cw from line.
   }
   else
   {
      double dx1dx2 = dx1*dx2;
      double dy1dy2 = dy1*dy2;

      if ((dx1dx2 < 0.) || (dy1dy2 < 0.))
      {
         retval = -1; // points are collinear, m_p0 is between point and m_p1
      }
      else
      {
         double dx1dx1 = dx1*dx1;
         double dy1dy1 = dy1*dy1;
         double dx2dx2 = dx2*dx2;
         double dy2dy2 = dy2*dy2;

         if ((dx1dx1 + dy1dy1) < (dx2dx2 + dy2dy2))
         {
            retval = 1;  // points are collinear, m_p1 is between m_p0 and point
         }
         else
         {
            retval = 0;  // points are collinear, point is between m_p0 and m_p1 or coincident with one of them.
         }
      }
   }

   return retval;
}

bool CLine2d::intersects(const CLine2d& otherLine) const
{
   bool retval = false;

   if (getExtent().intersects(otherLine.getExtent()))
   {
      int ccw0 = ccw(otherLine.m_p0) * ccw(otherLine.m_p1);

      if (ccw0 == 0)
      {
         retval = true;
      }
      else
      {
         int ccw1 = otherLine.ccw(m_p0) * otherLine.ccw(m_p1);

         if (ccw1 == 0)
         {
            retval = true;
         }
         else
         {
            retval = ((ccw0 < 0) && (ccw1 < 0));
         }
      }
   }

   return retval;
}

/*
Intersection of Two Line Segments
We have two line segments expressed by the following two equations. Parameters s and t need to be solved for. 
But you can eliminate one of them by multiplying through by the cross product of the vector it is associated with. 
This is done with  in this example. Then you can solve for that parameter. 

   v = v1 + s*(v2 - v1)                              [3.12]
   v = v3 + t*(v4 - v3)                              [3.13]
   (v1 - v3) + (s*(v2 - v1) - t*(v4 - v3) = 0        [3.14]
   s*(v2 - v1) x (v4 - v3) = (v3 - v1) x (v4 - v3)   [3.15]


In 2D 

      | x3 - x1    y3 - y1 |
      | x4 - x3    y4 - y3 |
   s = _____________________
      | x2 - x1    y2 - y1 |
      | x4 - x3    y4 - y3 |


   x = x1 + s*(x2 - x1)
   y = y1 + s*(y2 - y1)




Note that bottom determinant is zero if line segments are parallel or if one or both segments
are points and is equivalent to 

   y2 - y1     y4 - y3
   ________ = _________
   x2 - x1     x4 - x3


Intersection at end points - x3 equals x2 and y3 equals y2.


      | x2 - x1    y2 - y1 |
      | x4 - x2    y4 - y2 |
   s = _____________________ = 1
      | x2 - x1    y2 - y1 |
      | x4 - x2    y4 - y2 |

Intersection at end points - x4 equals x2 and y4 equals y2.


      | x3 - x1    y3 - y1 |
      | x2 - x3    y2 - y3 |
   s = _____________________
      | x2 - x1    y2 - y1 |
      | x2 - x3    y2 - y3 |
*/

// Return value
//  0 - if lines do not intersect
//  1 - if the lines intersect at a single point
//      (ps1 containts intersection point)
//  2 - if the lines intersect at two points (overlappiing colinear segments)
//      (ps1 and ps2 contain the intersection points
// -1 - if either of the lines is a point.
// -2 - if a pathological error occured
int CLine2d::intersect(const CLine2d& otherLine,CPoint2d& ps1,CPoint2d& ps2) const
{
   int retval = -2;

   const CPoint2d& p1 = m_p0;
   const CPoint2d& p2 = m_p1;
   const CPoint2d& p3 = otherLine.m_p0;
   const CPoint2d& p4 = otherLine.m_p1;

   if (p1.x < p3.x && p1.x < p4.x && p2.x < p3.x && p2.x < p4.x ||
       p1.x > p3.x && p1.x > p4.x && p2.x > p3.x && p2.x > p4.x ||
       p1.y < p3.y && p1.y < p4.y && p2.y < p3.y && p2.y < p4.y ||
       p1.y > p3.y && p1.y > p4.y && p2.y > p3.y && p2.y > p4.y     )
   {
      retval = 0;
   }
   else if (p1 == p2 || p3 == p4)
   {
      retval = -1;
   }
   else if ((p1 == p3 && p2 == p4) || (p1 == p4 && p2 == p3))
   {
      retval = 2;
      ps1 = p1;
      ps2 = p2;
   }
   else
   {
      double dx21 = p2.x - p1.x;
      double dy21 = p2.y - p1.y;
      double crossProduct1 = dx21*(p3.y - p1.y) - dy21*(p3.x - p1.x);
      double crossProduct2 = dx21*(p4.y - p1.y) - dy21*(p4.x - p1.x);

      double dx43 = p4.x - p3.x;
      double dy43 = p4.y - p3.y;
      double crossProduct3 = dx43*(p1.y - p3.y) - dy43*(p1.x - p3.x);
      double crossProduct4 = dx43*(p2.y - p3.y) - dy43*(p2.x - p3.x);

      if (crossProduct1 == 0. && crossProduct2 == 0. ||
          crossProduct3 == 0. && crossProduct4 == 0.     )
      {  // the lines are colinear and, 
         // one or both of the endpoints lie on the other line, 
         // or one or both lines are points

         const CPoint2d *c1 = &p1,*c2 = &p2,*c3 = &p3,*c4 = &p4,*ct;

         if (p1.x != p2.x)
         {
            if (c2->x < c1->x)
            {
               c1 = &p2;  c2 = &p1;
            }

            if (c4->x < c3->x)
            {
               c3 = &p4;  c4 = &p3;
            }

            if (c3->x < c1->x)
            {
               ct = c1;  c1 = c3;  c3 = ct;
               ct = c2;  c2 = c4;  c4 = ct;
            }

            if (c2->x > c3->x)
            {  // two intersections
               retval = 2;  ps1 = *c3;

               ps2 = ((c2->x > c4->x) ? *c4 : *c2);
            }
            else if (c2->x == c3->x)
            {  // one intersection
               retval = 1;  ps1 = *c2;
            }
            else
            {  // no intersections
               retval = 0;
            }
         }
         else if (p1.y != p2.y)
         {
            if (c2->y < c1->y)
            {
               c1 = &p2;  c2 = &p1;
            }

            if (c4->y < c3->y)
            {
               c3 = &p4;  c4 = &p3;
            }

            if (c3->y < c1->y)
            {
               ct = c1;  c1 = c3;  c3 = ct;
               ct = c2;  c2 = c4;  c4 = ct;
            }

            if (c2->y > c3->y)
            {  // two intersections
               retval = 2;  ps1 = *c3;

               ps2 = ((c2->y > c4->y) ? *c4 : *c2);
            }
            else if (c2->y == c3->y)
            {  // one intersection
               retval = 1;  ps1 = *c2;
            }
            else
            {  // no intersections
               retval = 0;
            }
         }
         else
         {  // point case - should never reach here
            ASSERT(0);
         }
      }
   }

   if (retval == 0 || retval == 1 || retval == -2)
   {
      if (p2 == p3 || p2 == p4)
      {
         retval = 1;
         ps1 = p2;
      }
      else if (p1 == p3 || p1 == p4)
      {
         retval = 1;
         ps1 = p1;
      }
      else
      {  // lines intersect at one point or are parallel and not colinear
         double sd = (p2.x - p1.x) * (p4.y - p3.y) - (p4.x - p3.x) * (p2.y - p1.y);
         double sn = (p3.x - p1.x) * (p4.y - p3.y) - (p4.x - p3.x) * (p3.y - p1.y);

         if (sd == 0.)
         {  // parallel line case or point case
            retval = 0;
         }
         else
         {
            double s = sn / sd;

            if (p3.x == p4.x)
            {
               ps1.x = p3.x;
            }
            else
            {
               ps1.x = p1.x + s*(p2.x - p1.x);
            }

            if (p3.y == p4.y)
            {
               ps1.y = p3.y;
            }
            else
            {
               ps1.y = p1.y + s*(p2.y - p1.y);
            }

            if (ps1.x < p1.x && ps1.x < p2.x ||
                ps1.x > p1.x && ps1.x > p2.x ||
                ps1.x < p3.x && ps1.x < p4.x ||
                ps1.x > p3.x && ps1.x > p4.x ||
                ps1.y < p1.y && ps1.y < p2.y ||
                ps1.y > p1.y && ps1.y > p2.y ||
                ps1.y < p3.y && ps1.y < p4.y ||
                ps1.y > p3.y && ps1.y > p4.y     )
            {
               retval = 0;
            }
            else
            {
               retval = 1;
            }
         }
      }
   }

   return retval;
}

void CLine2d::splitAt(CPoint2d point,CLine2d*& linePart1,CLine2d*& linePart2)
{
   linePart1 = linePart2 = NULL;

   if ((point.x < m_p0.x && point.x < m_p1.x) ||
       (point.x > m_p0.x && point.x > m_p1.x) ||
       (point.y < m_p0.y && point.y < m_p1.y) ||
       (point.y > m_p0.y && point.y > m_p1.y)    )
   {
      double d0 = point.distanceSqr(m_p0);
      double d1 = point.distanceSqr(m_p1);
      
      if (d0 < d1)
      {
         point = m_p0;
      }
      else
      {
         point = m_p1;
      }
   }

   if (m_p0 == point)
   {
      linePart2 = this;
   }
   else if (m_p1 == point)
   {
      linePart1 = this;
   }
   else
   {
      linePart1 = this;
      linePart2 = new CLine2d(*this);

      linePart1->m_p1 = point;
      linePart2->m_p0 = point;
   }
}

bool CLine2d::mutualBisect(CLine2d& otherLine,CLine2d*& linePart1     ,CLine2d*& linePart2,
                                              CLine2d*& otherLinePart1,CLine2d*& otherLinePart2)
{
   bool retval = false;

   linePart1 = linePart2 = otherLinePart1 = otherLinePart2 = NULL;

   CPoint2d p0,p1;
   int numIntersections = intersect(otherLine,p0,p1);

   if (numIntersections > 0)
   {
      if (numIntersections > 1)
      {  // use other intersection point if both edges intersect at their endpoints at p0
         if ((p0 == m_p0 || p0 == m_p1) &&
             (p0 == otherLine.m_p0 || p0 == otherLine.m_p1)  )
         {
            p0 = p1;
         }
      }

      splitAt(p0,linePart1,linePart2);
      otherLine.splitAt(p0,otherLinePart1,otherLinePart2);
      retval = true;
   }

   return retval;
}

double CLine2d::distanceToLineSquared(const CPoint2d& point) const
{
/*
                   x,y
                    o
                   ^|^
                  / | \
                 /  |  \
                a   |   b
               /    d    \
              /     |     \
             / B    |     A\
            o---a'-----b'-->o
          x0,y0      c    x1,y1
  
  
      Law of Cosines:   a^2 = b^2 + c^2 - 2bc*cos(A)

      c = a' + b'
      cos(A) = b'/b
      aa = a'a' + dd
      bb = b'b' + dd

      aa = bb + cc - 2bc(b'/b)
         = bb + cc - 2cb'
      b' = (bb + cc - aa)/2c
      b'b' = (bb + cc - aa)^2/4cc

      dd = bb - b'b'
*/
   double dx = (point.x - m_p0.x);
   double dy = (point.y - m_p0.y);
   double aa = dx*dx + dy*dy;

   dx = (point.x - m_p1.x);
   dy = (point.y - m_p1.y);
   double bb = dx*dx + dy*dy;

   dx = (m_p1.x - m_p0.x);
   dy = (m_p1.y - m_p0.y);
   double cc = dx*dx + dy*dy;

   double distanceSqr;

   if (aa == 0. || bb == 0.)
   {
      distanceSqr = 0.;
   }
   else if (aa > cc || bb > cc)
   {
      distanceSqr = min(aa,bb);   
   }
   else
   {
      double temp = (bb + cc - aa);
      double bpbp = temp*temp/(4*cc);
      distanceSqr = bb - bpbp;
   }

   return distanceSqr;
}

bool CLine2d::verticesWithinDistanceSquared(const CLine2d& otherLine,double distanceSquared) const
{
   bool retval = false;
   double distanceSquared00 = m_p0.distanceSqr(otherLine.m_p0);

   if (distanceSquared00 <= distanceSquared)
   {
      double distanceSquared11 = m_p1.distanceSqr(otherLine.m_p1);

      retval = (distanceSquared11 <= distanceSquared);
   }

   if (!retval)
   {
      double distanceSquared01 = m_p0.distanceSqr(otherLine.m_p1);

      if (distanceSquared01 <= distanceSquared)
      {
         double distanceSquared10 = m_p1.distanceSqr(otherLine.m_p0);

         retval = (distanceSquared10 <= distanceSquared);
      }
   }

   return retval;
}





