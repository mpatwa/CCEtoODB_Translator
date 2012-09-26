// $Header: /CAMCAD/DcaLib/DcaLibGraphics.cpp 3     3/09/07 5:15p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaLibGraphics.h"
#include "DcaPoint2d.h"

/*
History: SetEpsilon.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 4/22/98    Time: 10:04a
 * Created in $/LibGraphics
 * Initial add
*/

double epsilon = .001;

//_____________________________________________________________________________
double setEpsilon(double newEpsilon)
{
   double retval = epsilon;
   epsilon = newEpsilon;

   return retval;
}

/*
History: IntersectCircleToCircle.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 4/22/98    Time: 10:04a
 * Created in $/LibGraphics
 * Initial add
*/

//_____________________________________________________________________________
//  intersectCircleToCircle calculates the intersection point(s) of 2 arcs.
//  It returns the number of intersection points,
//  0, 1, or 2.  If the arcs are congruent, then ?
//  a0, b0, a1, and b1 are in radians
//_____________________________________________________________________________
int intersectCircleToCircle(double& sx0,double& sy0,double& sx1,double& sy1,
double h0,double k0,double r0,double a0,double b0,
double h1,double k1,double r1,double a1,double b1)
{
   double twoPi = 2.*Pi;
  
   double x,y,h,dx,dy,ang0,ang1,angle,sinang,cosang;
   int retval;
  
   dx = h1 - h0;
   dy = k1 - k0;
  
   h = _hypot(dx,dy);
   r0 = fabs(r0);
   r1 = fabs(r1);
  
   // check if circles are too far apart or close together to intersect
   if (h > r0 + r1 || h < fabs(r0 - r1)) return(0);
  
   // check if centers are congruent
   if (diff0(h,0.0) == 0.0)
   {
      if (diff0(r0,r1) != 0.0) return(0);
  
      // congruent circles if radii are equal
      normalizeAnglesRadians(a0,b0);
      normalizeAnglesRadians(a1,b1);
  
      retval = ordcoord(ang0,ang1,a0,b0,a1,b1);
  
      sx0 = sx1 = r0;
      sy0 = sy1 = 0.0;
  
      rotateCoord(sx0,sy0,sin(ang0),cos(ang0));
      rotateCoord(sx1,sy1,sin(ang1),cos(ang1));
  
      sx0 += h0;
      sy0 += k0;
      sx1 += h1;
      sy1 += k1;
  
      return(retval);
   }
  
/*
---------------------------------------------------------------------------
   r0^2 = x^2 + y^2
  
   r1^2 = (x - h)^2 + y^2
  
   r1^2 = x^2 - 2hx + h^2 + y^2
   r1^2 = x^2 - 2hx + h^2 + r0^2 - x^2
   r1^2 = r0^2 - 2hx + h^2
  
  
   x    = (r0^2 - r1^2 + h^2)/2h
  
   y = +sqrt(r0^2 - x^2)
       -sqrt(r0^2 - x^2)
---------------------------------------------------------------------------
*/
  
   x = (r0*r0 - r1*r1 + h*h)/(h + h);
   y = sqrt(r0*r0 - x*x);
  
   ang0  = atan2(y,x);
   ang1  = atan2(y,x - h);
   angle = atan2(dy,dx);
  
   a0 -= angle;
   b0 -= angle;
   a1 -= angle;
   b1 -= angle;
  
   cosang = dx/h;
   sinang = dy/h;
  
   retval = 0;
  
   if (isAngleBetweenRadians(ang0,a0,b0) && isAngleBetweenRadians(ang1,a1,b1))
   {
      retval = 1;
  
      sx0 = x;
      sy0 = y;
  
      rotateCoord(sx0,sy0,sinang,cosang);
  
      sx0 += h0;
      sy0 += k0;
   }
  
   ang0 = twoPi - ang0;
   ang1 = twoPi - ang1;
  
   if (isAngleBetweenRadians(ang0,a0,b0) && isAngleBetweenRadians(ang1,a1,b1))
   {
      retval++;
  
      sx1 = x;
      sy1 = -y;
  
      rotateCoord(sx1,sy1,sinang,cosang);
  
      sx1 += h0;
      sy1 += k0;
  
      if (retval == 1)
      {
         sx0 = sx1;
         sy0 = sy1;
      }
   }
  
   return(retval);
}

/*
History: Diff0.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 4/22/98    Time: 10:04a
 * Created in $/LibGraphics
 * Initial add
*/

//_____________________________________________________________________________
double diff0(double a,double b)
{
   double c;
  
   if (a != 0.0 && b != 0.0) c = 1.0 - a/b;
   else c = a - b;
  
   if (c > -.001 && c < .001) return 0.0;
  
   return(a - b);
}

/*
History: IntersectLineToCircle.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 4/22/98    Time: 10:04a
 * Created in $/LibGraphics
 * Initial add
*/

//_____________________________________________________________________________
//  intersectLineToCircle calculates the intersection point(s) of an arc and a
//  line segment.  It returns the number of intersection points,
//  0, 1, or 2
//  a0 and a1 are in radians
//_____________________________________________________________________________
int intersectLineToCircle(double& sx0,double& sy0,double& sx1,double& sy1,
int& num,double x0,double y0,double x1,double y1,double h,double k,double r,double a0,double a1)
{
   double zero = 0.0;
  
   CPolynomialRoots solution;
   double m,i,a,b,c,dx,dy,ang;
   int j,retval;
  
   if (ordEqual(x0,x1))
   {
      // no solutions if the line segment is a point
      if (ordEqual(y0,y1)) return(0);
  
      m = 1.0e6;
   }
   else
   {
      m = (y1 - y0)/(x1 - x0);
   }
  
   if (fabs(m) > (float)2.0)
   {
      // if the line segment has a big slope, recalculate with
      // the coordinate system mirrored about the line y=x .
      return(intersectLineToCircle(sy0,sx0,sy1,sx1,num,y0,x0,y1,x1,k,h,r,
             (float)(Pi/2. - a1),(float)(Pi/2. - a0)));
   }
  
   i = y1 - m*x1;
  
   a = (m*m + 1.0);
   b = 2.0*(m*(i - k) - h);
   c = i - k;
   c = h*h + c*c - r*r;
  
   solution.solve(a,b,c);
  
   sx0 = solution.getRealRoot(0);
   sx1 = solution.getRealRoot(1);
  
   if ((num = solution.getNumRealRoots()) == 0) return(0);
  
   // sort x0, x1 and *sx0, *sx1; simplifies comparison logic below
   if (x0 > x1)
   {
      a  = x0;
      x0 = x1;
      x1 = a;
   }
  
   if (sx0 > sx1)
   {
      a    = sx0;
      sx0 = sx1;
      sx1 = a;
   }
  
   // since the slope is < 2, if the solutions are close to equal,
   // then a tangent or near tangent must have occured
   if (ordEqual(sx0,sx1)) retval = 1;
   else retval = 2;
  
   num = retval;
 
   normalizeAnglesRadians(a0,a1);
  
   double* x = &sx0;
   double* y = &sy0;
  
   for (j = retval;j > 0;j--)
   {
      /* if (      *x < x0         ||       *x > x1        )   */
         if (diff0(*x , x0) < zero || diff0(*x , x1) > zero)
      {
         retval--;
  
         *x = sx1;
  
         continue;
      }
  
      *y = m*(*x) + i;
  
      dx = *x - h;
      dy = *y - k;
  
      ang = atan2(dy,dx);
  
   // if (      ang < a0        ) ang += (float)360.0;  */
   // while (diff0(ang , a0) < zero) ang += 2.*Pi;
      while (ang < a0) ang += 2.*Pi;
  
   // if (      ang > a1        )  */
   // if (diff0(ang , a1) > zero)
      if (ang > a1)
      {
         retval--;
  
         *x = sx1;
  
         continue;
      }
  
      // can only reach here if sx0, *y0 passed tests.
      // if we never reach here, then either both solutions failed
      // or the sx0, sy0 failed and then passed after sx1 copied to sx0
      x = &sx1;
      y = &sy1;
   }
  
   if (!retval) return(0);
  
   if (ordEqual(sx0,x0))  // OK since sx0 ,sx1 and x0,x1 sorted above
   {
      sx0 = x0;
      sy0 = y0;
   }
  
   if (retval == 1)
   {
      if (ordEqual(sx0,x1)) // in case retval == 1 and sx1 copied to sx0
      {
         sx0 = x1;
         sy0 = y1;
      }
   }
   else  // retval must equal 2
   {
      if (ordEqual(sx1,x1))  // OK since *sx0 ,*sx1 and x0,x1 sorted above
      {
         sx1 = x1;
         sy1 = y1;
      }
   }
  
   return(retval);
  
/*
---------------------------------------------------------------------------
   y = mx + i
  
   (x - h)^2 + (y - k)^2 = r^2
  
---------------------------------------------------------------------------
  
   x^2 - 2xh + h^2 + (mx + i)^2 - 2k(mx + i) + k^2 = r^2
  
   x^2 - 2xh + h^2 + m^2x^2 + 2mix + i^2 - 2kmx - 2ki + k^2 = r^2
  
   (m^2 + 1)x^2 + (-2h + 2mi - 2km)x + (h^2 + i^2 - 2ki + k^2 - r^2) = 0
  
   (m^2 + 1)x^2 + 2(m(i - k) - h)x + (h^2 + (i - k)^2 - r^2) = 0
---------------------------------------------------------------------------
*/
  
}

/*
History: IntersectLineToLine.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 5/20/98    Time: 4:42p
 * Updated in $/LibGraphics
 * Modified original version of intersectLineToLine and added new
 * overload.
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 4/22/98    Time: 10:04a
 * Created in $/LibGraphics
 * Initial add
*/

//_____________________________________________________________________________
//  intersectLineToLine calculates the intersection point of 2 line segments.
//  If the line segments are parallel and overlapping, the interior
//  endpoints are returned and the return value is 2.
//  If the lines intersect at only a single point, the return value is 1
//  If the lines do not intersect, then the return value is 0
//_____________________________________________________________________________
int intersectLineToLine(double& sx0,double& sy0,double& sx1,double& sy1,
           double x1,double y1,double x2,double y2,
           double x3,double y3,double x4,double y4)
{
   bool vert1,vert2;
   double m1,m2,i1,i2;
   int retval;
  
   //BOOLEAN fpeq();
   //double fmin(),fmax(),fabs(),diff0();
   //int ordcoord();
  
/*
      general equations:
  
      y = m*x + i
      m = (y2 - y1) / (x2 - x1)
      i = y - m*x
  
      at intersection:
  
      y = m1*x + i1 = m2*x + i2
      m1*x - m2*x = i2 - i1
      x = (i2 - i1) / (m1 - m2)
*/
  
  
   vert1 = ordEqual(x1,x2);
  
   if (vert1)    // vertical line 
   {
      if (ordEqual(y1,y2))  // point
      {
         return(0);
      }
   }
   else
   {
      m1 = (y2 - y1)/(x2 - x1);
   }
  
   // flip axii around if the slope is big
   if (vert1 || fabs(m1) > (float)2.0)
   {
      // cannot cause vert1 to be TRUE on recursive call because      
      // if vert1 is true, fpeq(y1,y2) == FALSE,                      
      // if m1 > 2.0, then y2 - y1 > x2 - x1                          
      retval = intersectLineToLine(sy0,sx0,sy1,sx1,y1,x1,y2,x2,y3,x3,y4,x4);
  
      return(retval);
   }
  
   retval = -1;
   i1 = y1 - m1*x1;
  
   vert2 = ordEqual(x3,x4);
  
   if (vert2) // vertical line 2 case
   {
      sx0 = x4 = x3;  // x4 set to x3 for the segment comparison below
   }
   else
   {
      m2 = (y4 - y3)/(x4 - x3);
      i2 = y3 - m2*x3;

      //if (fabs(diff0(m1,m2)) < (float).0001) // parallel line case
      if (fabs(diff0(m1,m2)) < (float).000001) // parallel line case
      {
         double y1t = m2*x1 + i2;
         double y2t = m2*x2 + i2;

         if (ordEqual(y1,y1t) && ordEqual(y2,y2t))
         {  // parallel congruent line case
            retval = ordcoord(sx0,sx1,x1,x2,x3,x4);
         }
         else
         {  // parallel non congruent line case
            return 0;
         }
  
/* ----------------
         // parallel non congruent line case
         if (fabs(diff0(i1,i2)) > (float).001) return(0);
  
         retval = ordcoord(sx0,sx1,x1,x2,x3,x4);
  
         // end points may be very close
         // !!! does not work well for horizontal lines!!!
      // if (retval != ordcoord(sy0,sy1,y1,y2,y3,y4)) return(0);
 ---------------- */
      }
      else
      {
         sx0 = (i2 - i1) / (m1 - m2);
      }
   }
  
   sy0 = m1*sx0 + i1;
  
   if (retval > 0)   /* can only occur if ordcoord called above */
   {
      sy1 = m1*sx1 + i1;  /* in case retval is 2 */
  
      return(retval);
   }
  
   /* force solution to an end point if it is very close to the end point */
  
   /* both x and y coordinates of solution must be close to endpoint  */
   /* Example: m1 = 0.0, m2 = 100.0                                   */
  
   if (ordEqual(sx0,x1) && ordEqual(sy0,y1))
   {
      sx0 = x1;
      sy0 = y1;
   }
   else if (ordEqual(sx0,x2) && ordEqual(sy0,y2))
   {
      sx0 = x2;
      sy0 = y2;
   }
   else if (vert2)
   {
      if (ordEqual(sy0,y3))   // if vert2, then sx0 is exacly equal to
      {                    // x3 and x4
         sy0 = y3;
      }
      else if (ordEqual(sy0,y4))
      {
         sy0 = y4;
      }
   }
   else
   {
      if (ordEqual(sx0,x3) && ordEqual(sy0,y3))
      {
         sx0 = x3;
         sy0 = y3;
      }
      else if (ordEqual(sx0,x4) && ordEqual(sy0,y4))
      {
         sx0 = x4;
         sy0 = y4;
      }
   }
  
   /* segment comparison */
   if (            sx0 < min(x1,x2) || sx0 > max(x1,x2)   ||
       (!vert2 && (sx0 < min(x3,x4) || sx0 > max(x3,x4))) ||
       ( vert2 && (sy0 < min(y3,y4) || sy0 > max(y3,y4)))   ) return(0);
  
   return(1);
}

//_____________________________________________________________________________
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
//_____________________________________________________________________________
int intersectLineToLine(CPoint2d& ps1,CPoint2d& ps2,const CPoint2d& p1,
   const CPoint2d& p2,const CPoint2d& p3,const CPoint2d& p4)
{
   int retval = -2;

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

/*
History: IsAngleBetween.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 4/22/98    Time: 10:04a
 * Created in $/LibGraphics
 * Initial add
*/

//_____________________________________________________________________________
//  isAngleBetween returns true if angle is between angle 1 and angle 2
//_____________________________________________________________________________
bool isAngleBetweenDegrees(double angle,double a0,double a1)
{
   bool between;
 
   normalizeAnglesDegrees(a0,a1);
   normalizeAnglesDegrees(a0,angle);
  
   between = (angle >= a0 && angle <= a1);
  
   return(between);
}

//_____________________________________________________________________________
bool isAngleBetweenRadians(double angle,double a0,double a1)
{
   bool between;
 
   normalizeAnglesRadians(a0,a1);
   normalizeAnglesRadians(a0,angle);
  
   between = (angle >= a0 && angle <= a1);
  
   return(between);
}

/*
History: NormalizeAngles.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 4/22/98    Time: 10:04a
 * Created in $/LibGraphics
 * Initial add
*/

//_____________________________________________________________________________
//  normalizeAngles normalizes 2 angles to each other,
//  such that  0 <= a0 < 360  and
//            a0 <  a1 < 720
//_____________________________________________________________________________
void normalizeAnglesDegrees(double& a0,double& a1)
{
   static double a360 = 360.0,zero = 0.0;
  
   a0 = fmod(a0,a360);
   a1 = fmod(a1,a360);
  
   if (a0 < zero) a0 += a360;
  
   while (a1 <= a0) a1 += a360;
  
   // circles which have been rotated by adding a displacement angle
   // to their start and stop angles may have their stop angle
   // slightly over 360 degrees beyond the start angle.
 
   if (a1 - a0 < .01) a1 = a0 + a360;
}

//_____________________________________________________________________________
void normalizeAnglesRadians(double& a0,double& a1)
{
   static double a360 = 2*3.141592653589793238,zero = 0.0;
  
   a0 = fmod(a0,a360);
   a1 = fmod(a1,a360);
  
   if (a0 < zero) a0 += a360;
  
   while (a1 <= a0) a1 += a360;
  
   // circles which have been rotated by adding a displacement angle
   // to their start and stop angles may have their stop angle
   // slightly over 360 degrees beyond the start angle.
 
   if (a1 - a0 < .0002) a1 = a0 + a360;
}

/*
History: OrdCoord.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 4/22/98    Time: 10:04a
 * Created in $/LibGraphics
 * Initial add
*/
  
//_____________________________________________________________________________
//  ordcoord calculates the intersect points of congruent entities
//  examples could be:
//  the angles of the endpoints of two arcs,
//  the x coordinates of two line segments or the
//  the y coordinates of two line segments.  
//_____________________________________________________________________________
int ordcoord(double& s0,double& s1,double a0,double b0,double a1,double b1)
{
   double temp;
   int retval;
 
   if (a0 > b0)
   {
      temp = a0;
      a0   = b0;
      b0   = temp;
   }
  
   if (a1 > b1)
   {
      temp = a1;
      a1   = b1;
      b1   = temp;
   }
 
/*                                                 <-  increasing angle
                      case 1a:                         or coordinate
                b1 o-----------o a1
            b0 o-------------------o a0
                   o           o
  
                      case 1b:
            b0 o-------------------o a0
                b1 o-----------o a1
                   o           o
  
                      case 2a:
        b1 o-------------------o a1
            b0 o-------------------o a0
               o               o
  
                      case 2b:
                b1 o-------------------o a1
            b0 o-------------------o a0
                   o               o
  
                      case 3a:
                    b1 o-------------------o a1
            b0 o-------o a0
                       o
  
                      case 3b:
            b1 o-------------------o a1
                                b0 o-------o a0
                                   o
  
                      case 4a:
                      b1 o-----------------o a1
            b0 o-------o a0
  
  
                      case 4b:
            b1 o-----------------o a1
                                b0 o-------o a0
  
  
  
  
*/
   s0 = a1;
   s1 = b1;
  
   if (a0 > b1) return(0);    /* case 4a */
  
   if (a1 > b0) return(0);    /* case 4b */
  
   if (a0 == b1)              /* case 4a */
   {
      retval = 1;
      s0    = a0;
   }
   else if (a1 == b0)         /* case 4b */
   {
      retval = 1;
      s0    = a1;
   }
   else                       /* case 1a or 1b or 2a or 2b */
   {
      retval = 2;
      s0    = max(a0,a1);
      s1    = min(b0,b1);
   }
  
   return(retval);
}

/*
History: OrdEqual.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 4/22/98    Time: 10:04a
 * Created in $/LibGraphics
 * Initial add
*/

//_____________________________________________________________________________
bool ordEqual(double coord0,double coord1)
{
   double diff = coord0 - coord1;

   if (diff < 0) diff = -diff;

   bool retval = (diff <= epsilon);

   return retval;
}

/*
History: RotateCoord.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 4/22/98    Time: 10:04a
 * Created in $/LibGraphics
 * Initial add
*/

//_____________________________________________________________________________
//  rotateCoord rotates the given coordinate.
//_____________________________________________________________________________
void rotateCoord(double& rotx,double& roty,double sinang,double cosang)
{ 
   double x,y; 
  
   x = rotx; 
   y = roty; 
  
   rotx = x*cosang - y*sinang; 
   roty = x*sinang + y*cosang; 
} 

/*
History: PolynomialRoots.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 4/22/98    Time: 10:04a
 * Created in $/LibGraphics
 * Initial add
*/

//_____________________________________________________________________________
CPolynomialRoots::CPolynomialRoots()
{
   m_numRealRoots = 0;
   m_order = 0;
   m_realRoot[0] = 0.;
   m_realRoot[1] = 0.;
   m_realRoot[2] = 0.;
   m_realRoot[3] = 0.;
}

CPolynomialRoots::~CPolynomialRoots()
{
}

double CPolynomialRoots::getRealRoot(int rootIndex)
{
   ASSERT(rootIndex >= 0 && rootIndex <= 3);

   return m_realRoot[rootIndex];
}

//_____________________________________________________________________________
//  solve calculates the real roots of the quadratic equation:
//  ax^2 + bx + c = 0
void CPolynomialRoots::solve(double a,double b,double c)
{
   double zero = 0.0,two = 2.0,four = 4.0;
   double errorfactor = -.0001;
 
   if (a == zero)
   {
      m_order   = 1;
      m_numRealRoots = 1;
  
      if (b == zero)
      {
         m_order   = 0;
         m_numRealRoots = 0;
      }
      else
      {
         m_realRoot[0] = -c/b;
      }
   }
   else
   {
      m_order = 2;
  
      double disc = b*b - four*a*c;
  
      if (disc < zero && disc > errorfactor) disc = zero;
  
      if (disc < zero)
      {
         m_numRealRoots = 0;
      }
      else
      {
         disc = diff0(sqrt(disc),zero);
  
         if (disc == zero)
         {
            m_numRealRoots = 1;
         }
         else
         {
            m_numRealRoots = 2;
         }
  
         m_realRoot[0] = (-b + disc)/(two * a);
         m_realRoot[1] = (-b - disc)/(two * a);
      }
   }
}
  
/*
   root2() has caused areslice() to fail when errorfactor was
   equal to -.001 .  A slice very close to a vertex with 2 arc edges
   on each side (signs of radii equal), was calculated to intersect
   the first edge full circle at 2 points, only one of which was
   on the arc, but the solution was not close enough the end point 1 of
   this first edge to set the nextvertex flag.  The slice was also
   calculated to intersect the second edge at 1 point, (tangent),
   because the disc was equal to 1.67e-4 .
*/

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________
