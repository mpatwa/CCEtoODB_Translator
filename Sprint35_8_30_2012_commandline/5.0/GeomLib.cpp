// $Header: /CAMCAD/5.0/GeomLib.cpp 27    3/26/07 11:19a Kurt Van Ness $

/*********************************************************************
*  geometry function library
*
*  Header File:
*     "geomlib.h"
*
*  C-Functions:
*     DegToRad          :Convert degrees to radians
*     RadToDeg          :Convert radians to degrees
*     ArcTan2           :Calculate arc-tangent of point
*     LengthPoint3         :Calculate length 3-D line
*     LengthPoint2         :Calculate length 2-D line
*     EqualPoint2       :2-D coordinates equal?
*     SetPoint2            :Set 2-D coordianate
*     ManLengthPoint2      :Calculate 2-D "Manhattan" length
*     Point2InSeg       :2-D point in 2-D segment?
*     GenRect2          :Create closed rectangular 2-D polyline
*     arc2poly          :Convert arc to 2-D polyline
*     StepAngle            :Calculate step angle for arc polyline
*     ArcPoly2          :Convert arc to 2-D polyline
*     CirclePoly2       :Convert circle to 2-D polyline
*     CirclePoint3         :Convert three 2-D points to circle
*     ArcPoint3            :Convert three 2-D points to arc
*     ArcCenter2        :Convert two 2-D points and center to arc
*     CenterArc2        :Converte two 2-D points and arc to center
*     ArcPoint2Angle    :Converte two 2-D points and delta angle (bulge)
*                        to arc values needed in DB.
*     ArcFrom2Pts       :Converte two 2-D points and radius
*                        to arc values needed in DB.
*     ArcFromStartEndRadius   :Converte two 2-D points and radius and 
*                              if Clockwise or Smaller to arc values 
*		ArcFromStartEndRadiusDeltaXY: Converte two 2-D points and radius and 
*                              if Clockwise and start delta X&Y to arc values 
*     RotMat3           :Calculate 3-D rotation matrix
*     RotMat2           :Calculate 2-D rotation matrix
*     TransPoint3       :Rotate+Translate 3-D polyline
*     TransPoint2       :Rotate+Translate 2-D polyline
*     NearestLinePoint2    :Find nearest 2-D point to infinite line
*     NearestPolyPoint2    :Find nearest 2-D point to polyline
*     IntersectInfiniteLine2  :Find 2-D intersection two infinite lines
*     BoxTextSeg2          :2-D segment areas overlap?
*     IntersectSeg2        :Find intersection of two 2-D segments
*     SegToSeg2            :Shortest joining segment of 2 segments
*     Rotate               :
*
*  Comments:
*  1/31/90: All routines updated and documented.  Routines
*     dealing with the old Line2 and Line3 structures have
*     bean removed (perpendicular line, parallel lines, etc.), and
*     the line structures have been removed from all other
*     routines.  All routines have been reviewed and modified
*     when needed to optimize speed performance. The following
*     routines have been added:
*
*        NearestLinePoint2
*        NearestPolyPoint2
*
*         The names of IntersLine2 and IntersSeg2 have been changed
*     to IntersectInfiniteLine2 and IntersectSeg2 respectively.  The
*     standard Turbo-C header files <stdlib.h>, <stdio.h>, and
*     <math.h> have been removed from "geometry.h" and placed
*     withim this module.  All other names, parameter lists,
*     and returned values remain unchanged.  Currently,
*     TransPoint3 and TransPoint2 expect polylines as parameters.
*     They have not been changed, but their names should
*     probably be changed to TransPoly3 and TransPoly2 and
*     single point functions TransPoint3 and TransPoint2 should
*     be added.
*
*    1/18/91: Added function to calculate the arc center point for
*     two 2-D points and the arc angle between them.
*
*        CenterArc2( double x1, double y1, double x2, double y2,
*                 double angle, double *cx, double *cy );
*
*********************************************************************/

#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include "geomlib.h"
#include "general.h"
#include "data.h"

#include "DcaPnt.h"
#include "DcaTMatrix.h"
#include "DcaLib.h"

// Building Block includes
// needed for BuildingBlock software
extern "C"
{
#include <bbsdefs.h>
#include <c2vdefs.h>
#include <c2ddefs.h>
#include <c2cdefs.h>
#include <vpidefs.h>
#include <vp2defs.h>
#include <grrdefs.h>
#include <v2ddefs.h>
#include <dmldefs.h>
#include <dmdefs.h>
}

/*********************************************************************
*  Convert Degrees to Radians
*
*  C-Prototype:
*     double DegToRad( double degrees );
*
*  Parameters:
*     degrees  = angle in degrees
*
*  Returns:
*     angle in radians as double;
*
*  Comments:
*********************************************************************/
double DegToRad( double degrees )
{
   return (PI/180.0)*degrees;
}

/*********************************************************************
*  Convert Radians to Degrees
*
*  C-Prototype:
*     double RadToDeg( double radians );
*
*  Parameters:
*     radians  = angle in radians
*
*  Returns:
*     angle in degrees as double;
*
*  Comments:
*********************************************************************/
double RadToDeg( double radians )
{
   return (180.0/PI)*radians;
}

/*********************************************************************
*  Comments:
*********************************************************************/
double ACos ( double x )
{
   if (fabs(x) > 1)
      return 0.0;
   return acos(x);
}

/*********************************************************************
*  Calculate the ARCTAN of a point
*
*  C-Prototype:
*     double ArcTan2( double y, double x );
*
*  Parameters:
*     y  = point's y coordinate in 2-dimensions
*     x  = point's x coordinate in 2-dimensions
*
*  Returns:
*     the angle from 0 formed by the vector starting at the
*     origin and ending at the point (x,y)
*
*  Comments:
*     ArcTan2 is an extension of the Turbo-C library function
*     atan2.  It will return valid angles for cases where
*     atan2 returns overflow or domain erros.
*********************************************************************/
double ArcTan2( double y, double x )
{
//   if( (fabs(x) < SMALLNUMBER) && (fabs(y) < SMALLNUMBER) )
//      return 0.0;
//   if( fabs(x) < SMALLNUMBER)
//   {
//      if( y > 0.0 )
//  return PI/2;
//      else
//  return -PI/2;
//   }
//   else if( fabs(y) < SMALLNUMBER )
//   {
//      if( x > 0.0 )
//         return 0.0;
//      else
//  return -PI;
//   }
//   else
   return atan2( y, x );
}

/*********************************************************************
*  Calculate the distance between two points in 3-dimensions
*
*  C-Prototype:
*     double LengthPoint3( Point3 *v1, Point3 *v2 );
*
*  Parameters:
*     *v1   = pointer to 3-D point structure defined in "geometry.h"
*     *v2   = pointer to 3-D point structure defined in "geometry.h"
*
*  Returns:
*     the distance between the two points in 3-D euclidian space
*
*  Comments:
*     Point3 is a point structure defined with the following
*     elements:
*
*        Point3.x, Point3.y, Point3.z, Point3.f
*********************************************************************/
double LengthPoint3( Point3 *v1, Point3 *v2 ) {
   return sqrt( (v2->x - v1->x)*(v2->x - v1->x)+
            (v2->y - v1->y)*(v2->y - v1->y)+
            (v2->z - v1->z)*(v2->z - v1->z) );
}

/*********************************************************************
*  Calculate the distance between two points in 2-dimensions
*
*  C-Prototype:
*     double LengthPoint2( Point2 *v1, Point2 *v2 );
*
*  Parameters:
*     *v1   = pointer to 2-D point structure defined in "geometry.h"
*     *v2   = pointer to 2-D point structure defined in "geometry.h"
*
*  Returns:
*     the distance between the two points in 2-D euclidian space
*
*  Comments:
*     Point2 is a point structure defined with the following
*     elements:
*
*        Point2.x, Point2.y, Point2.f
*
*     LengthPoint2 attempts to optimize the length calculations
*     by isolating the simple cases of horizontal and vertical
*     lines from the more time consuming general square root
*     calculation.
*********************************************************************/
double LengthPoint2( Point2 *v1, Point2 *v2 ) {
   if( v1->x == v2->x )
      return fabs(v2->y - v1->y);  /* segment is vertical or 0 length */
   else if( v1->y == v2->y )
      return fabs(v2->x - v1->x);  /* segment is horizontal or 0 length */
   else  /* general length */
      return sqrt( (v2->x - v1->x)*(v2->x - v1->x)+
                  (v2->y - v1->y)*(v2->y - v1->y) );
}

/*********************************************************************
*  Check for equality of point coordinates
*
*  C-Prototype:
*     int EqualPoint2( Point2 *v1, Point2 *v2 );
*
*  Parameters:
*     *v1   = pointer to 2-D point structure defined in "geometry.h"
*     *v2   = pointer to 2-D point structure defined in "geometry.h"
*
*  Returns:
*     1  = if point coordinates are equal
*     0  = if point coordinates are not equal
*
*  Comments:
*     Point2 is a point structure defined with the following
*     elements:
*
*        Point2.x, Point2.y, Point2.f
*********************************************************************/
int EqualPoint2( Point2 *v1, Point2 *v2 ) {
   if( (v1->x == v2->x) && (v1->y == v2->y) )
      return 1;
   return 0;
}

/*********************************************************************
*  Assign coordinate values to point structure
*
*  C-Prototype:
*     void SetPoint2( Point2 *v, double x, double y);
*
*  Parameters:
*     *v = pointer to 2-D point structure defined in "geometry.h"
*     x  = x coordinate for point
*     y  = y coordinate for point
*
*  Returns:
*
*  Comments:
*     Point2 is a point structure defined with the following
*     elements:
*
*        Point2.x, Point2.y, 
*********************************************************************/
void SetPoint2(Point2 *v, double x, double y) 
{
   v->x = x;
   v->y = y;
   v->bulge = 0;
}

/*********************************************************************
*  Calulate "Manhattan" approximation to distance between points
*
*  C-Prototype:
*     double ManLengthPoint2( Point2 *v1, Point2 *v2 );
*
*  Parameters:
*     *v1   = pointer to 2-D point structure defined in "geometry.h"
*     *v2   = pointer to 2-D point structure defined in "geometry.h"
*
*  Returns:
*     "Manhattan" approximation to distance between points
*
*  Comments:
*     Point2 is a point structure defined with the following
*     elements:
*
*        Point2.x, Point2.y, Point2.f
*
*         The "Manhattan" approximation to length is based upon
*     the Cauchy-Schwarz inequality which states that the
*     length of the hypotenuse of a right triangle is always
*     less than or equal to the sum of the lengths of the other
*     two sides.  For cases where approximations are sufficient,
*     its much faster than the slower square root calculation.
*********************************************************************/
double ManLengthPoint2( Point2 *v1, Point2 *v2 ) 
{
   return ( fabs(v2->x - v1->x) + fabs(v2->y - v1->y) );
}

/******************************************************************************
* ManLength()
*/
double ManLength(double x1, double y1, double x2, double y2)
{
   return ( fabs(x1 - x2) + fabs(y1 - y2) );
}

/*********************************************************************
*  Create a rectangular polyline in 2-dimensions
*
*  C-Prototype:
*     void GenRect2( Point2 *polyline, int *vert, double tlx,
*                 double tly, double brx, double bry  );
*
*  Parameters:
*     *polyline   = array of 2-D points to hold rectangle's edges
*     *vert = number of points (verticies) in polyline (always 5)
*     tlx,tly  = coordinate of rectangle's top left corner
*     brx,bry  = coordinate of rectangle's bottom right corner
*
*  Returns:
*     *polyline = 5 points defining closed rectangle
*     *vert = always set to 5
*
*  Comments:
*     Point2 is a point structure defined in "geometry.h" with
*     the following elements:
*
*        Point2.x, Point2.y, Point2.f
*
*     GenRect2 assumes that memory for five Point2 structures has
*     been properly allocated to the polyline array.
*********************************************************************/
void GenRect2( Point2 *polyline, int *vert, double tlx, double tly,
                  double brx, double bry ) {
   polyline[ 0 ].x = tlx;
   polyline[ 0 ].y = tly;
   polyline[ 1 ].x = tlx;
   polyline[ 1 ].y = bry;
   polyline[ 2 ].x = brx;
   polyline[ 2 ].y = bry;
   polyline[ 3 ].x = brx;
   polyline[ 3 ].y = tly;
   polyline[ 4 ].x = tlx;
   polyline[ 4 ].y = tly;
   *vert = 5;
}

/*********************************************************************
*  Convert an arc to a polyline in 2-dimensions
*
*  C-Prototype:
*     void arc2poly( double startangle, double deltaangle,
*                 double stepangle, double x, double y,
*                 double rad, Point2 *poly, int *cnt    );
*
*  Parameters:
*     startangle  = start angle for arc in radians
*     deltaangle  = delta angle for generating arc in radians
*     stepangle      = angle defines length of line segments in poly
*     x, y        = arc's center coordinate
*     rad         = arc's radius
*     *poly    = array of 2-D points to hold arc's edges
*     *cnt        = number of points (verticies) in polyline
*
*  Returns:
*     *poly = points defining arc as line segments
*     *cnt     = number of points in polyline
*
*  Comments:
*     Point2 is a point structure defined in "geometry.h" with
*     the following elements:
*
*        Point2.x, Point2.y, Point2.f
*
*********************************************************************/
/*
   stepangle in degree
*/
void arc2poly(double startangle,double deltaangle,double stepangle,
              double x,double y,double rad,Point2 *poly,int *cnt)
{
   int   p;
   int   i;
   double   s = startangle;
   int   maxpoly = *cnt;
   p = *cnt = 0;

   /* make first koo exact*/
   poly[p].x = x + cos(startangle) * rad;
   poly[p].y = y + sin(startangle) * rad;
   poly[p].bulge = 0;
   p++;

   /* limit to 2 degree acc. */
   if (fabs(stepangle) < DegToRad(2))  stepangle = DegToRad(2);
   if (deltaangle < 0)  stepangle = -stepangle;

   // first point is done above
   // last point will be done on the end.
   int steps = round(deltaangle/stepangle);  // not round
   for (i=0;i<steps-1;i++)
   {
      s += stepangle;

      if (p < maxpoly-1)
      {
         poly[p].x = x + cos(s) * rad;
         poly[p].y = y + sin(s) * rad;
         poly[p].bulge = 0;
         p++;
      }
      else
      {
         *cnt = 0;
         return;
      }
   }

   /* make last koo exact*/
   poly[p].x = x + cos(startangle+deltaangle) * rad;
   poly[p].y = y + sin(startangle+deltaangle) * rad;
   poly[p].bulge = 0;

   // only add last koo if the last koo is not the last of the calculated step angle
// if (fabs(poly[p-1].x - poly[p].x) > SMALLNUMBER || fabs(poly[p-1].y - poly[p].y) > SMALLNUMBER)
   p++;

   *cnt = p;
}

/*********************************************************************
*  Calculate step angle for generating arc segments
*
*  C-Prototype:
*     double StepAngle( double radius, double bulge );
*
*  Parameters:
*     radius   = arc's radius
*     bulge = defines maximum allowable length of arc segment
*
*  Returns:
*     maximum step angle for specified bulge
*
*  Comments:
*     If any two line segments of the polyline used to approximate
*     the arc form the two equal sides of an isosceles triangle,
*     the bulge defines the maximum hight this triangle is allowd
*     to have.  This limits the length the arc's line segments and
*     defines the maximum step angle between adjacent segments.
*     This relationship is only defined for bulge parameters
*     that are less than the radius of the arc, and for bulges
*     greater than the radius, StepAngle returns 0.0.
*********************************************************************/
double StepAngle( double radius, double bulge ) 
{
   if( bulge < radius && fabs(radius) > SMALLNUMBER)
      return ACos( (radius-bulge)/radius );
   return 0.0;
}

/*********************************************************************
*  Convert an arc to a polyline in two dimensions
*
*  C-Prototype:
*     void ArcPoly2( double sx, double sy, double cx, double cy,
*                 double angle, double bulge,
*                 Point2 *polyline, int *vert    );
*
*  Parameters:
*     sx,sy = arc's starting coordinate
*     cx,cy = arc's center coordiante
*     angle = arc's delta angle
*     bulge = defines maximum allowable length of arc segment
*     *polyline   = array of 2-D points defining arc as line segments
*     *vert = number of 2-D points in polyline
*
*  Returns:
*     *polyline   = array of 2-D points defining arc as line segments
*     *vert = number of 2-D points in polyline
*
*  Comments:
*     Similar to arc2poly except that it generates the polyline
*     from a different set of arc parameters.
*********************************************************************/
void ArcPoly2( double sx, double sy, double cx, double cy,
         double angle, double bulge, Point2 *polyline, int *vert, double stepangle)
{
   double radius;
   double startangle;

   radius = sqrt( (sy-cy)*(sy-cy) + (sx-cx)*(sx-cx) );
   startangle = ArcTan2( (sy-cy), (sx-cx) );

   arc2poly( startangle, angle, stepangle, cx, cy, radius, polyline, vert );
}

/*********************************************************************
*  Convert a circle to a plolyline in two dimensions
*
*  C-Prototype:
*     void CirclePoly2( double cx, double cy, double radius,
*                 double bulge, Point2 *polyline, int *vert );
*
*  Parameters:
*     cx,cy = circle's center coordiante
*     radius   = circle's radius
*     bulge = defines maximum allowable length of circle segment
*     *polyline   = array of 2-D points defining circle as line segments
*     *vert = number of 2-D points in polyline
*
*  Returns:
*     *polyline   = array of 2-D points defining circle as line segments
*     *vert = number of 2-D points in polyline
*
*  Comments:
*     Uses arc2poly.
*********************************************************************/
void CirclePoly2( double cx, double cy, double radius, double bulge,
                  Point2 *polyline, int *vert ) 
{
   double stepangle;

   if( (stepangle = StepAngle( radius, bulge )) != 0.0 )
   {
      arc2poly( 0.0, PI2, stepangle, cx, cy, radius, polyline, vert );
      /* ensure closed circle */
      if( (polyline[*vert-1].x-polyline[0].x >= 0.0005) ||
          (polyline[*vert-1].y-polyline[0].y >= 0.0005)   ) 
      {
          polyline[*vert].x = polyline[0].x;
          polyline[*vert].y = polyline[0].y;
      }
   }
}

/*********************************************************************
*  Convert three 2-D points to a circle
*
*  C-Prototype:
*     void CirclePoint3( double x1, double y1, double x2,
*                 double y2, double x3, double y3,
*                 double *cx, double *cy, double *r );
*
*  Parameters:
*     x1,y1 = first point of triangle defining circle
*     x2,y2 = second point of triangle defining circle
*     x3,y3 = third point of triangle defining circle
*     *cx,*cy  = circle's center coordiante
*     *r    = circle's radius
*
*  Returns:
*     *cx,*cy  = circle's center coordinate
*     *r    = circle's radius
*
*  Comments:
*     The three points must define a valid triangle; the
*     three points must be unique (no two can be equal) and
*     the points must not be colinear.  If not valid, the
*     routine returns cx = cy = r = 0.0.  CirclePoint3 finds
*     the circle's parameters by solving 3 simultanius
*     equations in three unknowns:
*
*        (x1-cx)^2 + (y1-cy)^2 = r^2
*        (x2-cx)^2 + (y2-cy)^2 = r^2
*        (x3-cx)^2 + (y3-cy)^2 = r^2
*        solve for cx, cy, and r
*********************************************************************/
void CirclePoint3( double x1, double y1, double x2, double y2,
          double x3, double y3, double *cx, double *cy, double *r ) 
{
// this c2d_arc_3pts does not work is start and end point is the same
   double g1, g2, g3, g4;
   double h1, h2, h3, h4;

   /* check for valid circle !(x1=x2=x3) and !(y1=y2=y3) and */
   /* (x1, y1) != (x3, y3) */

   /* if (x1,y1) == (x3, y3) -> this is full circle */
   if ((fabs(x1-x3) < SMALLNUMBER && fabs(y1-y3) < SMALLNUMBER))
   {
      if (fabs(y2-y1) < SMALLNUMBER)
      {
         *cy = y2;
         *cx = (x1 - x2) /2 + x2;
         *r  = (x1 - x2) /2;
         return;
      }
      else
      if (fabs(x2-x1) < SMALLNUMBER)
      {
         *cx = x2;
         *cy = (y1 - y2) /2 + y2;
         *r = (y1 - y2) /2;
         return;
      }
      else
      {
         // calc center from diagonal
         *cx = *cy = 0.0;
         *r = 0.0;
         return;
      }
   }

   /* assign intermediate factors */
   g1 = x1*x1 - x3*x3;
   g2 = x1*x1 - x2*x2;
   g3 = x3 - x1;
   g4 = x2 - x1;
   h1 = y3*y3 - y1*y1;
   h2 = y2*y2 - y1*y1;
   h3 = y1 - y3;
   h4 = y1 - y2;

   /* solve for circles's center coordinates */
   *cx = (g1*h4 - g2*h3 - h1*h4 + h2*h3)/(2*(g4*h3 - g3*h4));
   if( y1 != y2 )
      *cy = (g2 + 2*(*cx)*g4 - h2)/(2*h4);  /* h4=(y1-y2) != 0 */
   else /* y1 != y3 */
      *cy = (g1 + 2*(*cx)*g3 - h1)/(2*h3);  /* h3=(y1-y3) != 0 */

   /* solve for circle's radius and check for domain error */
   *r = sqrt( (x1 - *cx)*(x1 - *cx) + (y1 - *cy)*(y1 - *cy) );
}

/*********************************************************************
*  Convert three 2-D points to an arc
*
*  C-Prototype:
*     void ArcPoint3( double x1, double y1, double x2,
*                 double y2, double x3, double y3,
*                 double *cx, double *cy, double *r,
*                 double *sa, double *da );
*
*  Parameters:
*     x1,y1 = start point of triangle defining arc
*     x2,y2 = middle point of triangle defining arc
*     x3,y3 = end point of triangle defining arc
*     *cx,*cy  = arc's center coordiante
*     *r    = arc's radius
*     *sa   = start angle for arc
*     *da   = delta angle for arc
*
*  Returns:
*     *cx,*cy  = arc's center coordinate
*     *r    = arc's radius
*     *sa      = start angle for arc
*     *da      = delta angle for arc
*
*  Comments:
*     ArcPoint3 uses CirclePoint3 to define the circle
*     containing the arc.  
*     Figures start angle from first point and center
*     Figures end angle from last point and center
*     Normalizes and subtracts to find a positive delta angle
*     If counter-clockwise, delta = delta - 360
*********************************************************************/
void ArcPoint3(double x1, double y1, double x2, double y2, double x3, double y3, 
               double *cx, double *cy, double *r, double *sa, double *da)
{
   PT2   *pt0, *pt1, *pt2;
   pt0 = c2d_point(x1,y1);
   pt1 = c2d_point(x2,y2);
   pt2 = c2d_point(x3,y3);

   C2_CURVE    curve;

   curve = c2d_arc_3pts ( *pt0, *pt1, *pt2 );

   PT2   ctr;
   REAL  rad, ssa, ssw;
   INT   dir ;

   if (c2c_get_arc_data ( curve, ctr, &rad, &ssa, &ssw, &dir ))
   {
      *cx = ctr[0];
      *cy = ctr[1];
      *r = rad;
      *sa = ssa;
      *da = ssw * dir;
   }
   else
   {
      *cx = *cy = 0.0;
      *r = 0.0;
      *sa = 0.0;
      *da = 0.0;
   }
   c2d_free_point( pt0);
   c2d_free_point( pt1);
   c2d_free_point( pt2);

   c2d_free_curve( curve);

   return; 

#ifdef OLD_DEF
   CirclePoint3(x1, y1, x2, y2, x3, y3, cx, cy, r); // find center and radius (same as circle)

   if(*r == 0.0)
   {
      *sa = 0.0;
      *da = 0.0;
      return;
   }

   *sa = ArcTan2(y1 - *cy, x1 - *cx); // start angle
   double ea = ArcTan2(y3 - *cy, x3 - *cx); // end angle

   if (ea < *sa) ea += PI2; // normalize for positive delta

   *da = ea - *sa; // calculate positive delta

   // if clockwise instead of counter-clockwise, change delta
   if (cross_product(x1, y1, x2, y2) + cross_product(x2, y2, x3, y3) + cross_product(x3, y3, x1, y1) < 0.0) 
      *da -= PI2;
#endif
}

/*********************************************************************
*  Convert the center and a point to a circle, radius
*
*  C-Prototype:
*     void CircleCenterPt( double x1, double y1, double x2, double y2, double *r );
*
*/
void CircleCenterPt( double xc, double yc, double x, double y, double *r )

{
   PT2   *center, *pt;
   center = c2d_point(xc, yc);
   pt = c2d_point(x , y);

   C2_CURVE    curve;

   curve = c2d_circle_ctr_pt ( *center, *pt );

   REAL  rad;
   if (c2c_get_arc_radius ( curve, &rad ))
   {
      *r = rad;
   }

   c2d_free_point( center);
   c2d_free_point( pt);

   c2d_free_curve( curve);

   return; 
}


/*********************************************************************
*  Create 3-D rotation matrix
*
*  C-Prototype:
*     void RotMat3( Mat3x3 *m, double ax, double ay, double az );
*
*  Parameters:
*     *m = 3x3 rotation matrix
*     ax = angular rotation about x-axis
*     ay = angular rotation about y-axis
*     az = angular rotation about z-axis
*
*  Returns:
*     *m = 3x3 rotation matrix
*
*  Comments:
*     Mat3x3 is a matrix structure defined in "geometry.h" with
*     the following elements:
*
*        Mat3x3.m3[3][3]; 3x3 array of doubles
*
*********************************************************************/
void RotMat3( Mat3x3 *m, double ax, double ay, double az ) {
   double cosax, cosay, cosaz;
   double sinax, sinay, sinaz;

   cosax = cos( ax ); cosay = cos( ay ); cosaz = cos( az );
   sinax = sin( ax ); sinay = sin( ay ); sinaz = sin( az );

   m->m3[ 0 ][ 0 ] = cosaz*cosay;
   m->m3[ 0 ][ 1 ] = -sinaz*cosax-cosaz*sinay*sinax;
   m->m3[ 0 ][ 2 ] = sinaz*sinax-cosaz*sinay*cosax;
   m->m3[ 1 ][ 0 ] = sinaz*cosay;
   m->m3[ 1 ][ 1 ] = cosaz*cosax-sinaz*sinay*sinax;
   m->m3[ 1 ][ 2 ] = -cosaz*sinax-sinaz*sinay*cosax;
   m->m3[ 2 ][ 0 ] = sinay;
   m->m3[ 2 ][ 1 ] = cosay*sinax;
   m->m3[ 2 ][ 2 ] = cosay*cosax;
}

/*********************************************************************
*  Create 2-D rotation matrix
*
*  C-Prototype:
*     void RotMat2( Mat2x2 *m, double az );
*
*  Parameters:
*     *m = 2x2 rotation matrix
*     az = angular rotation about z-axis
*
*  Returns:
*     *m = 2x2 rotation matrix
*
*  Comments:
*     Mat2x2 is a matrix structure defined in "geometry.h" with
*     the following elements:
*
*        Mat2x2.m2[2][2]; 2x2 array of doubles
*
*********************************************************************/
#if !defined(EnableDcaCamCadDocLegacyCode)
void RotMat2( Mat2x2 *m, double az ) 
{
   m->m2[ 0 ][ 0 ] = cos( az );
   m->m2[ 0 ][ 1 ] = -sin( az );
   m->m2[ 1 ][ 0 ] = sin( az );
   m->m2[ 1 ][ 1 ] = cos( az );
}
#endif

/*********************************************************************
*  Perform a linear translation and rotation on an array of
*   3-dimensional points.
*
*  C-Prototype:
*     void TransPoint3( Point3 *v, int n, Mat3x3 *m,
*                 double tx, double ty, double tz );
*
*  Parameters:
*     *v = array of 3-D point structures
*     *n = number of points in array
*     *m = 3x3 rotation matrix defining rotation to perform
*     tx = translation in x direction
*     ty = translation in y direction
*     tz = translation in z direction
*
*  Returns:
*     *v = array of translated/rotated 3-D point structures
*     *n = number of points in translated/rotated array
*
*  Comments:
*     Mat3x3 is a matrix structure defined in "geometry.h" with
*     the following elements:
*
*        Mat3x3.m3[3][3]; 3x3 array of doubles
*
*     Point3 is a point structure defined in "geometry.h" with
*     the following elements:
*
*        Point3.x, Point3.y, Point3.z, Point3.f
*
*     The rotation is performed first, then the translation
*********************************************************************/
void TransPoint3( Point3 *v, int n, Mat3x3 *m,
           double tx, double ty, double tz )
{
   short i;
   double x, y, z;

   for( i=0; i<n; i++ ) 
   {
      x = v[ i ].x;
      y = v[ i ].y;
      z = v[ i ].z;
      v[ i ].x = x*m->m3[0][0]+y*m->m3[0][1]+z*m->m3[0][2]+tx;
      v[ i ].y = x*m->m3[1][0]+y*m->m3[1][1]+z*m->m3[1][2]+ty;
      v[ i ].z = x*m->m3[2][0]+y*m->m3[2][1]+z*m->m3[2][2]+tz;
   }
}

/*********************************************************************
*  Perform a linear translation and rotation on an array of
*   2-dimensional points.
*
*  C-Prototype:
*     void TransPoint2( Point2 *v, int n, Mat2x2 *m,
*                 double tx, double ty );
*
*  Parameters:
*     *v = array of 2-D point structures
*     *n = number of points in array
*     *m = 2x2 rotation matrix defining rotation to perform
*     tx = translation in x direction
*     ty = translation in y direction
*
*  Returns:
*     *v = array of translated/rotated 2-D point structures
*     *n = number of points in translated/rotated array
*
*  Comments:
*     Mat2x2 is a matrix structure defined in "geometry.h" with
*     the following elements:
*
*        Mat2x2.m2[2][2]; 2x2 array of doubles
*
*     Point2 is a point structure defined in "geometry.h" with
*     the following elements:
*
*        Point2.x, Point2.y, Point2.f
*
*     The rotation is performed first, then the translation
*********************************************************************/
#if !defined(EnableDcaCamCadDocLegacyCode)
void TransPoint2( Point2 *v, int n, const Mat2x2 *m, double tx, double ty ) 
{
   short i;
   double x, y;

   for( i=0; i<n; i++ ) 
   {
      x = v[ i ].x;
      y = v[ i ].y;
      v[ i ].x = x*m->m2[ 0 ][ 0 ]+y*m->m2[ 0 ][ 1 ]+tx;
      v[ i ].y = x*m->m2[ 1 ][ 0 ]+y*m->m2[ 1 ][ 1 ]+ty;
   }
}
#endif

/*********************************************************************
*  Find the point on an infinite geometric line nearest to a
*  specified point.
*
*  C-Prototype:
*     void NearestLinePoint2( Point2 *x1, Point2 *x2, Point2 *v,
*                    Point2 *p );
*
*  Parameters:
*     *x1,*x2  = 2-D point structures which define geometric line
*     *v    = specified 2-D point
*     *p    = point on infinite line nearest specified point (*v)
*
*  Returns:
*     *p    = point on infinite line nearest specified point (*v)
*
*  Comments:
*     Point2 is a point structure defined in "geometry.h" with
*     the following elements:
*
*        Point2.x, Point2.y, Point2.f
*
*     With (*v) == vx,vy, the routine finds the nearest point
*     with the following derived relationship:
*
*        px = (vx + slope12*(vy+intercept12))/(1+slope12^2)
*        py = slope12*px + intercept12
*
*     The cases where the line is vertical (slope12 = ?) or
*     horizontal (slope12 = 0) are treated seperately for
*     speed optimization.
*********************************************************************/
void NearestLinePoint2( Point2 *x1, Point2 *x2, Point2 *v, Point2 *p )
{
   double slope, intercept;

   if( x1->x == x2->x ) {  /* line defined by x1 and x2 is vertical */
      p->x = x1->x;
      p->y = v->y;
   }
   else if( x1->y == x2->y ) {  /* line is vertical */
      p->x = v->x;
      p->y = x1->y;
   }
   else {  /* general case */
      slope = (x2->y - x1->y)/(x2->x - x1->x);
      intercept = x1->y - slope*(x1->x);
      p->x = (v->x + slope*(v->y + intercept))/(1+slope*slope);
      p->y = slope*(p->x) + intercept;
   }
}

/*********************************************************************
*  Find the point on a 2-D polyline nearest to a specified point.
*
*  C-Prototype:
*     double NearestPolyPoint2( Point2 *poly, int points,
*                       Point2 *v, Point2 *p    );
*
*  Parameters:
*     *poly = array of 2-D points forming a polyline
*     points   = number of 2-D points in polyline
*     *v    = specified 2-D point
*     *p    = point on polyline nearest specified point (*v)
*
*  Returns:
*     distance between nearest point (*p) and specified point (*v)
*     *p    = point on polyline nearest specified point (*v)
*
*  Comments:
*     Point2 is a point structure defined in "geometry.h" with
*     the following elements:
*
*        Point2.x, Point2.y, Point2.f
*
*         For each line segment, the nearest point to (*v) will be
*     the result of NearestLinePoint2, or, if this result is not
*     within the line segment, the nearest point will be one
*     of the segment's endpoints.  All segments in polyline are
*     checked.  To speed up the search, distances between (*v)
*     and (*p) on different segments are compared using the
*     "Manhattan" distance.  One true distance is calculated
*     at the end.
*********************************************************************/
double NearestPolyPoint2( Point2 *poly, int points, Point2 *v, Point2 *p ) 
{
   int i;
   double dist = -1.0;
   double manlength;
   Point2 test;

   /* polyline must have at least 1 point */
   if( points <= 0 )
      return dist;   /* dist = -1.0 flags error */

   /* initialize distance from v to first point in polyline */
   dist = ManLengthPoint2( &(poly[0]), v );
   SetPoint2( p, poly[0].x, poly[0].y);

   /* check distance from v and points in all polyline segments */
   for( i=1; i<points; i++ ) {
      NearestLinePoint2( &(poly[i-1]), &(poly[i]), v, &test );
      /* check for nearest point within current segment */
      if( Point2InSeg( &(poly[i-1]), &(poly[i]), &test ) ) 
      {
         manlength = ManLengthPoint2( v, &test );
         if( manlength < dist ) 
         {
            SetPoint2( p, test.x, test.y);
            dist = manlength;
         }
      }
      /* nearest to line outside current segment, use endpoint */
      else 
      {
         manlength = ManLengthPoint2( &(poly[i]), v );
         if( manlength < dist ) 
         {
            SetPoint2( p, poly[i].x, poly[i].y);
            dist = manlength;
         }
      }
   }
   /* calculate actual distance between v and nearest point p to polyline */
   dist = LengthPoint2( v, p );
   return dist;
}


/*********************************************************************
*  Perform quick area test to see if segments can intersect
*
*  C-Prototype:
*     int BoxTestSeg2Width(Point2 *a1, Point2 *a2, Point2 *b1, Point2 *b2 );
*
*  Parameters:
*     *a1,*a2  = 2-D points defining first line segment
*     *b1,*b2  = 2-D points defining second line segment
*
*  Returns:
*     1     = if segment boxes overlap
*     0     = if segment boxes do not overlap
*
*  Comments:
*     Point2 is a point structure defined in "geometry.h" with
*     the following elements:
*
*        Point2.x, Point2.y, Point2.f
*********************************************************************/
int BoxTestSeg2Width(Point2 *a1, Point2 *a2, double aWidthRadius, Point2 *b1, Point2 *b2 , double bWidthRadius)
{
   double aLeft, aRight, aTop, aBottom;
   double bLeft, bRight, bTop, bBottom;

   // find correctly oriented corners for first segment
   aLeft = min(a1->x, a2->x) - aWidthRadius;
   aRight = max(a1->x, a2->x) + aWidthRadius;
   aTop = max(a1->y, a2->y) - aWidthRadius;
   aBottom = min(a1->y, a2->y) + aWidthRadius;

   bLeft = min(b1->x, b2->x) - bWidthRadius;
   bRight = max(b1->x, b2->x) + bWidthRadius;
   bTop = max(b1->y, b2->y) + bWidthRadius;
   bBottom = min(b1->y, b2->y) - bWidthRadius;

   // compare box area overlap by checking corners
   if (aLeft > bRight + SMALLNUMBER)
      return FALSE;
   if (bLeft > aRight + SMALLNUMBER)
      return FALSE;
   if (aBottom > bTop + SMALLNUMBER)
      return FALSE;
   if (bBottom > aTop + SMALLNUMBER)
      return FALSE;

   return TRUE;
}

/*********************************************************************
*  Perform quick area test to see if segments can intersect
*
*  C-Prototype:
*     int BoxTestBox( Point2 *x1, Point2 *x2, Point2 *y1, Point2 *y2 );
*
*  Parameters:
*     *x1,*x2  = 2-D points defining first box min, max
*     *y1,*y2  = 2-D points defining second box min, max
*
*  Returns:
*     1     = if box boxes overlap
*     0     = if boxes do not overlap
*
*********************************************************************/
int BoxTestBox( Point2 *x1, Point2 *x2, Point2 *y1, Point2 *y2 ) 
{              //  Box1 min         max   Box2 min         max

   // if box1min is larger box2max return 0;
   if (x1->x > y2->x)   return 0;
   if (x1->y > y2->y)   return 0;

   // if box1max is smaller box2min return 0;
   if (x2->x < y1->x)   return 0;
   if (x2->y < y1->y)   return 0;

   return 1;
}

/*********************************************************************
*  Perform quick area test to see if segments can intersect
*
*  C-Prototype:
*     int BoxTestBox( Point2 *x1, Point2 *x2, Point2 *y1, Point2 *y2 );
*
*  Parameters:
*     *x1,*x2  = 2-D points defining first box min, max
*     *y1,*y2  = 2-D points defining second box min, max
*
*  Returns:
*     1     = if box boxes overlap
*     0     = if boxes do not overlap
*
*********************************************************************/
BOOL BoxTestPoint(Point2 *p1, Point2 *p2, Point2 *p) 
{
   if (p->x > p1->x + SMALLNUMBER && p->x > p2->x + SMALLNUMBER)
      return FALSE;

   if (p->y > p1->y + SMALLNUMBER && p->y > p2->y + SMALLNUMBER)
      return FALSE;

   return TRUE;
}


/*********************************************************************
*  Find the segment with the shortest length witch joins
*  two line segments.
*
*  C-Prototype:
*     double SegToSeg2(Point2 *a1, Point2 *a2, Point2 *b1, Point2 *b2, Point2 *a, Point2 *b);
*
*  Parameters:
*     *a1,*a2  = 2-D points defining first line segment
*     *b1,*b2  = 2-D points defining second line segment
*     *a,*b  = 2-D segment with shortest length joining
*              first and second segments.
*
*  Returns:
*     Distance of segment with shortest length joining first
*        and second segments.
*     1     = if segments intersect
*     0     = if segments do not intersect
*     *a,*b  = 2-D segment with shortest length joining
*              first and second segments.
*
*  Comments:
*     Point2 is a point structure defined in "geometry.h" with
*     the following elements:
*
*        Point2.x, Point2.y, Point2.f
*
*     The routine is optimized by looking for the fast case
*     where the segments intersect.  In this case, the joining
*     segment has no length and p1 = p2 = point of intersection.
*     Also, the "Manhattan" length is used when comparing joining
*     segment lengths. For other cases, one of the joining
*     segment's endpoints  must equal one of the first or second's
*     endpoints.  The other joining segment endpoint will be a point
*     within the other segment or one of the other segment's endpoints.
*     Therefore, all endpoints must be checked against the others.
*********************************************************************
double SegToSeg2(Point2 *a1, Point2 *a2, Point2 *b1, Point2 *b2, Point2 *a, Point2 *b) 
{
   // initialize distance and connecting segment, choose first endpoints
   SetPoint2(a, a1->x, a1->y);
   SetPoint2(b, b1->x, b1->y);

   // check for case where segments intersect
   Point2 pi;
   if (IntersectSeg2(a1, a2, b1, b2, &pi)) 
   {
      a->x = b->x = pi.x;
      a->y = b->y = pi.y;
      return 0.0;
   }

   double manLength = ManLengthPoint2(a1, b1);
   Point2 test;

   // check a1 to segment b1-b2
   NearestLinePoint2(b1, b2, a1, test);
   if (Point2InSeg(b1, b2, &test)) 
   {
      double temp = ManLengthPoint2(a1, &test);
      if (temp < manLength)
      {
         b->x = test.x;
         b->y = test.y;
         manLength = temp;
      }
   }
   else 
   {  // check with other endpoint (y2)
      manlength = ManLengthPoint2( x1, y2 );
      if( manlength < dist ) 
      {
         SetPoint2( p2, y2->x, y2->y );
         dist = manlength;
      }
   }

   // check x2 to segment y1-y2 
   NearestLinePoint2( y1, y2, x2, &test );
   if( Point2InSeg( y1, y2, &test ) ) {
      manlength = ManLengthPoint2( x2, &test );
      if( manlength < dist ) {
         SetPoint2( p2, test.x, test.y );
         SetPoint2( p1, x2->x, x2->y );
         dist = manlength;
      }
   }
   else {  // check with other endpoints (y1 and y2) 
      manlength = ManLengthPoint2( x2, y1 );
      if( manlength < dist ) {
         SetPoint2( p1, x2->x, x2->y );
         SetPoint2( p2, y1->x, y1->y );
         dist = manlength;
      }
      else {
         manlength = ManLengthPoint2( x2, y2 );
         if( manlength < dist ) {
            SetPoint2( p1, x2->x, x2->y );
            SetPoint2( p2, y2->x, y2->y );
            dist = manlength;
         }
      }
   }

   // check y1 to segment x1-x2 
   NearestLinePoint2( x1, x2, y1, &test );
   if( Point2InSeg( x1, x2, &test ) ) 
   {
      manlength = ManLengthPoint2( y1, &test );
      if( manlength < dist ) 
      {
         SetPoint2( p1, test.x, test.y);
         SetPoint2( p2, y1->x, y1->y );
         dist = manlength;
      }
   }

   // check y2 to segment x1-x2 
   NearestLinePoint2( x1, x2, y2, &test );
   if( Point2InSeg( x1, x2, &test ) ) 
   {
      manlength = ManLengthPoint2( y2, &test );
      if( manlength < dist ) 
      {
         SetPoint2( p1, test.x, test.y);
         SetPoint2( p2, y2->x, y2->y );
         dist = manlength;
      }
   }

   dist = LengthPoint2( p1, p2 );
   return dist;
}

/*****************************************************************************/
/*
*/
double dbunitround(double r)
{
   double   x;
   // float has the FLT_EPSILON accuracy
   x = floor(r * 1e7 + 0.5) / 1e7;

   return x;
}

/*****************************************************************************/
/*
*/
long lround(double r)
{
   return((long)floor(r + 0.5));
}

/*********************************************************************
*  Calculates a 2 Dimensional Angle from a 3D Matrix.
*
*  C-Prototype:
*     double Mat3ToRad( Mat3x3 *m );
*
*  Parameters:
*     m  = 3-Dimensional Matrix
*
*  Returns:
*     Real value for radiens.

*  Comments:
*********************************************************************/
double   Mat3ToRad(Mat3x3 *m)
{
   Point3   pp;
   double   z;
   /* normalize vector */
   pp.x = 1;
   pp.z = pp.y = 0;

   TransPoint3(&pp,1,m,0.0,0.0,0.0);
   z = ArcTan2(pp.y,pp.x);
   return(z);
}


/*********************************************************************
*  Calculates a 2 Dimensional Angle from a 2D Matrix.
*
*  C-Prototype:
*     double Mat2ToRad( Mat2x2 *m );
*
*  Parameters:
*     m  = 2-Dimensional Matrix
*
*  Returns:
*     Real value for radians.

*  Comments:
*********************************************************************/
double   Mat2ToRad(Mat2x2 *m)
{
   Point2   pp;
   double   z;
   /* normalize vector */
   pp.x = 1;
   pp.y = 0;

   TransPoint2(&pp,1,m,0.0,0.0);
   z = ArcTan2(pp.y,pp.x);
   return(z);
}

/*------------------------------------------------------------------\
| SYNTAX:
|  CombineTransf3( Mat3x3 *m2, Point3 *t2, Mat3x3 *m1, Point3 *t1,
|                             Mat3x3 *M, Point3 *T );
|
| PARAMETERS:
|
| RETURNS:
|
| DESCRIPTION:
|  Combines two coordinate transformations into one by applying the first
|  to the second.  The new transformation is returned in the
|  transformatin matrix M and the translation vector T.
|
|     T1 = m1+t1  {transformation matrix plus rotation}
|     T2 = m2+t2  {transformation matrix plus rotation}
|
|     M  = T1 applied to T2
|        = T2xT1
|        = m1x(m2+t2)+t1
|        = {dot product}(m2xm1)+{vector product}(m1xt2)+{vector sum}t1
|        = M + T
|
|     M  = {dot product}(m2xm1}
|     T  = {vector product}(m1xt2)+{vector sum}t1
|
| HISTORY:
\------------------------------------------------------------------*/
void CombineTransf3( Mat3x3 *m2, Point3 *t2, Mat3x3 *m1, Point3 *t1,
                              Mat3x3 *M, Point3 *T ) 
{
                              
   // Mat3x3 m = {{{1,0,0},{0,1,0},{0,0,1}}};   /* identity matrix */
   Mat3x3   m;
   Point3   t;                      /* null vector, no translation */
   
   m.m3[0][0] = 1;
   m.m3[0][1] = m.m3[0][2] = 0;
   m.m3[1][1] = 1;
   m.m3[1][0] = m.m3[1][2] = 0;
   m.m3[2][2] = 1;
   m.m3[2][0] = m.m3[2][1] = 0;
   
   t.x = t.y = t.z = 0.0;
   /* M = dot product (m1xm2) */
   m.m3[0][0] = (m2->m3[0][0]*m1->m3[0][0])+(m2->m3[0][1]*m1->m3[1][0])+(m2->m3[0][2]*m1->m3[2][0]);
   m.m3[0][1] = (m2->m3[0][0]*m1->m3[0][1])+(m2->m3[0][1]*m1->m3[1][1])+(m2->m3[0][2]*m1->m3[2][1]);
   m.m3[0][2] = (m2->m3[0][0]*m1->m3[0][2])+(m2->m3[0][1]*m1->m3[1][2])+(m2->m3[0][2]*m1->m3[2][2]);
   m.m3[1][0] = (m2->m3[1][0]*m1->m3[0][0])+(m2->m3[1][1]*m1->m3[1][0])+(m2->m3[1][2]*m1->m3[2][0]);
   m.m3[1][1] = (m2->m3[1][0]*m1->m3[0][1])+(m2->m3[1][1]*m1->m3[1][1])+(m2->m3[1][2]*m1->m3[2][1]);
   m.m3[1][2] = (m2->m3[1][0]*m1->m3[0][2])+(m2->m3[1][1]*m1->m3[1][2])+(m2->m3[1][2]*m1->m3[2][2]);
   m.m3[2][0] = (m2->m3[2][0]*m1->m3[0][0])+(m2->m3[2][1]*m1->m3[1][0])+(m2->m3[2][2]*m1->m3[2][0]);
   m.m3[2][1] = (m2->m3[2][0]*m1->m3[0][1])+(m2->m3[2][1]*m1->m3[1][1])+(m2->m3[2][2]*m1->m3[2][1]);
   m.m3[2][2] = (m2->m3[2][0]*m1->m3[0][2])+(m2->m3[2][1]*m1->m3[1][2])+(m2->m3[2][2]*m1->m3[2][2]);

   /* T = {vector product}(m1xt2)+t1 */
   t.x = (m1->m3[0][0]*t2->x)+(m1->m3[0][1]*t2->y)+(m1->m3[0][2]*t2->z)+t1->x;
   t.y = (m1->m3[1][0]*t2->x)+(m1->m3[1][1]*t2->y)+(m1->m3[1][2]*t2->z)+t1->y;
   t.z = (m1->m3[2][0]*t2->x)+(m1->m3[2][1]*t2->y)+(m1->m3[2][2]*t2->z)+t1->z;

   memcpy( M, &m, sizeof(Mat3x3) );
   memcpy( T, &t, sizeof(Point3) );
}

/*------------------------------------------------------------------\
| SYNTAX:
|  CombineTransf2( Mat2x2 *m2, Point2 *t2, Mat2x2 *m1, Point2 *t1,
|                             Mat2x2 *M, Point2 *T );
|
| PARAMETERS:
|
| RETURNS:
|
| DESCRIPTION:
|  Combines two coordinate transformations into one by applying the first
|  to the second.  The new transformation is returned in the
|  transformatin matrix M and the translation vector T.
|
|     T1 = m1+t1  {transformation matrix plus rotation}
|     T2 = m2+t2  {transformation matrix plus rotation}
|
|     M  = T1 applied to T2
|        = T2xT1
|        = m1x(m2+t2)+t1
|        = {dot product}(m2xm1)+{vector product}(m1xt2)+{vector sum}t1
|        = M + T
|
|     M  = {dot product}(m2xm1}
|     T  = {vector product}(m1xt2)+{vector sum}t1
|
| HISTORY:
\------------------------------------------------------------------*/
void CombineTransf2( Mat2x2 *m2, Point2 *t2, Mat2x2 *m1, Point2 *t1,
                              Mat2x2 *M, Point2 *T ) 
{
   // Mat2x2 m = {{{1,0},{0,1}}};   /* identity matrix */
   Mat2x2   m;
   Point2 t;            /* null vector, no translation */

   m.m2[0][0] = 1;
   m.m2[0][1] = 0;
   m.m2[1][0] = 0;
   m.m2[1][1] = 1;
   t.x = t.y = 0;
   /* M = dot product (m1xm2) */
   m.m2[0][0] = (m2->m2[0][0]*m1->m2[0][0])+(m2->m2[0][1]*m1->m2[1][0]);
   m.m2[0][1] = (m2->m2[0][0]*m1->m2[0][1])+(m2->m2[0][1]*m1->m2[1][1]);
   m.m2[1][0] = (m2->m2[1][0]*m1->m2[0][0])+(m2->m2[1][1]*m1->m2[1][0]);
   m.m2[1][1] = (m2->m2[1][0]*m1->m2[0][1])+(m2->m2[1][1]*m1->m2[1][1]);

   /* T = {vector product}(m1xt2)+t1 */
   t.x = (m1->m2[0][0]*t2->x)+(m1->m2[0][1]*t2->y);
   t.y = (m1->m2[1][0]*t2->x)+(m1->m2[1][1]*t2->y);

   memcpy( M, &m, sizeof(Mat2x2) );
   memcpy( T, &t, sizeof(Point2) );
}

/****************************************************************************/
/*
*/
int Line_2_Box(double x1,double y1,double x2,double y2,double width,
           Point2 *box,int *vert)
{
   Mat2x2 m;
   double a,l;

   a = ArcTan2( (y2-y1), (x2-x1) );

   /* make bounding box with x1,y1 centered at origin */
   l = sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );
   GenRect2( box, vert, (-width/2), (width/2), (l+width/2), (-width/2) );

   /* translate and rotate line to correct position */
   RotMat2( &m, a );
   TransPoint2( box, *vert, &m, x1, y1 );
   return 1;
}

/*******************************************************************\
* FUNCTION: PntInPoly()
*--------------------------------------------------------------------
* PROTOTYPE:
*  int PntInPoly( Point2 *p, Point2 *poly, int polycount );
*
* PARAMETERS:
*  p = pointer to 2-D point structure
*  poly = pointer to 2-D array of point structures (polyline)
*  polycount = number of points in polyline
*
* RETURNS:
*  2  = point p is on polyline vertex
*  1  = point p is on polyline segment (not vertex)
*  0  = point p is inside polyline
*  -1 = point p is outside of polyline
*  -2 = polyline is invalid
*                                   
* DESCRIPTION:
*  For a simple closed polygon (no crossings of perimeter), the
*  relative location of a point to the polygon can be determined
*  by summing the angles formed by vectors p->p1 and p->p2 (p1 and p2
*  being adjacent points on the polygon).  For points outside the
*  polygon, the sum of the angles will be 0; inside the polygon and
*  the sum of the angles will be +/-2*PI.  Points on the polygon are
*  found as special cases.  THIS ONLY WORKS FOR SIMPLE CLOSED POLYGONS!
*  Complex polygons are less well defined and may or may not yield
*  correct results (integer multple of +/-2*PI or 0).
*
* COMMENTS:
*  Speed could be increased by passing bounding box information for
*  the polygon.  A quick check could then be made for points clearly
*  outside the polygon without waisting time calculating the general
*  case.
\*******************************************************************/
int PntInPoly(Point2 *p,Point2 *poly,int polycount)
{

double radsqr1;                  /* dist between p and p1 of poly segment */
double radsqr2;                  /* dist between p and p2 of poly segment */
double sweepangle;               /* angle of segment p1,p2 about p */
double dotprod;                  /* vector product of p->p1 and p->p2 */
double crossprod;                /* vector cross product of p->p1 and p->p2 */
double anglesum;                 /* sum of angles of poly segs about p */
int i;                           /* current segment index for polyline */

   /*----------------------------------------------------------------\
   | check for valid polygon.  must have at least 2 points.
   \----------------------------------------------------------------*/
   if( polycount >= 2 )
   {

      /*----------------------------------------------------------------\
      | sum of all angles of plolygon segments(arcs) about point p
      |  starts at zero.
      \----------------------------------------------------------------*/
      anglesum = 0.0;

      /*----------------------------------------------------------------\
      | sum all angles of polygon segments(arcs) about point p. along
      |  the way, check for simple case where p is on polygon segment.
      \----------------------------------------------------------------*/
      for( i=1; i<polycount; i++ ) {

         /*----------------------------------------------------------------\
         | check for point on polygon line segment endpoint.
         \----------------------------------------------------------------*/
         if( (fabs(poly[i].x-p->x)+fabs(poly[i].y-p->y) < SMALLNUMBER) ||
               (fabs(p->x-poly[i-1].x)+fabs(p->y-poly[i-1].y) < SMALLNUMBER) )
         {
            /*----------------------------------------------------------------\
            | point on segment vertex of polygon.
            \----------------------------------------------------------------*/
            return 2;

         }

         /*----------------------------------------------------------------\
         | check for point on polygon line segment.
         \----------------------------------------------------------------*/
         if( (fabs((poly[i].y-p->y)/(poly[i].x-p->x) - (p->y-poly[i-1].y)/(p->x-poly[i-1].x)) <= SMALLNUMBER) )
         {
            /*----------------------------------------------------------------\
            | point is on same line
            | check to see if on line segment by checking to see if between endpoints
            \----------------------------------------------------------------*/
            if (poly[i].x > poly[i-1].x)
            {
               if ( (poly[i].x >= p->x) && (p->x >= poly[i-1].x) )
                  return 1;
            }
            else
            {
               if ( (poly[i-1].x >= p->x) && (p->x >= poly[i].x) )
                  return 1;
            }
         }

         /*----------------------------------------------------------------\
         | point not on line segment (forms triangle).  calculate angle of
         |  triangle at 'p'and add it to sum.
         \----------------------------------------------------------------*/
         radsqr1 = sqrt( (p->x-poly[i-1].x)*(p->x-poly[i-1].x) +
                           (p->y-poly[i-1].y)*(p->y-poly[i-1].y) );
         radsqr2 = sqrt( (poly[i].x-p->x)*(poly[i].x-p->x) +
                           (poly[i].y-p->y)*(poly[i].y-p->y) );
         dotprod = (poly[i-1].x-p->x)*(poly[i].x-p->x) +
                     (poly[i-1].y-p->y)*(poly[i].y-p->y);
         sweepangle = ACos( dotprod/(radsqr1*radsqr2) );
         crossprod = (poly[i-1].x-p->x)*(poly[i].y-p->y) -
                        (poly[i-1].y-p->y)*(poly[i].x-p->x);
         if( crossprod > 0.0 ) {
            anglesum = anglesum+sweepangle;
         }
         else {
            anglesum = anglesum-sweepangle;
         }

      }

      /*----------------------------------------------------------------\
      | calculate angle between last and first point (forces closed
      |  polygon for sum of angles) and add it to sum.
      \----------------------------------------------------------------*/
      radsqr1 = sqrt( (p->x-poly[i-1].x)*(p->x-poly[i-1].x) +
                        (p->y-poly[i-1].y)*(p->y-poly[i-1].y) );
      radsqr2 = sqrt( (poly[0].x-p->x)*(poly[0].x-p->x) +
                        (poly[0].y-p->y)*(poly[0].y-p->y) );
      dotprod = (poly[i-1].x-p->x)*(poly[0].x-p->x) +
                  (poly[i-1].y-p->y)*(poly[0].y-p->y);
      sweepangle = ACos( dotprod/(radsqr1*radsqr2) );
      crossprod = (poly[i-1].x-p->x)*(poly[0].y-p->y) -
                     (poly[i-1].y-p->y)*(poly[0].x-p->x);
      if( crossprod > 0.0 )
      {
         anglesum = anglesum+sweepangle;
      }
      else
      {
         anglesum = anglesum-sweepangle;
      }

      /*----------------------------------------------------------------\
      | check status of point with respect to polygon.
      \----------------------------------------------------------------*/
      if( fabs(anglesum) > SMALLNUMBER )
      {
         /*----------------------------------------------------------------\
         | inside simple polygon if sum of angles = multiple of 2*PI
         \----------------------------------------------------------------*/
         return 0;

      }
      else {

         /*----------------------------------------------------------------\
         | outside simple polygon if sum of angles = 0
         \----------------------------------------------------------------*/
         return -1;

      }

   }
   else {

      /*----------------------------------------------------------------\
      | invalid polygon.
      \----------------------------------------------------------------*/
      return -2;
   }
}

/*******************************************************************\
* FUNCTION: ArcFrom2Pts()
*--------------------------------------------------------------------
* PROTOTYPE:
*  int ArcFrom2Pts( double x1, double y1, double x2, double y2, double radius,
*                 double *cx1, double *cy1, double *cx2, double *cy2 );
*
* PARAMETERS:
*  x1,y1 = 2D arc start point
*  x2,y2 = 2D arc stop point
*  radius = radius of traced arc(s)
*  *cx1,*cy1 = 2D center coordinates for circle containing arc
*  *cx2,*cy2 = 2D center coordinates for circle containig complimentary arc
*
* RETURNS:
*  *cx1,*cy1 = 2D center coordinates for circle containing arc.
*                 unchanged if no unique arc.
*  *cx2,*cy2 = 2D center coordinates for circle containig complimentary arc.
*                 equals *cx1,*cy1 if only one unique arc.
*                 unchanged if no unique arc.
*  res = number of unqique arcs for given start point, stop point,
*           and radius.
*
* DESCRIPTION:
*  Calculates the center coordinates for the arc(s) specified by start
*     coordinates, stop coordinates, and a radius.  Such a description
*     can not define a unique arc traced between start and stop points.
*     Three possibilities arrise:
*
*           1). When the start point equals the stop point, the connecting
*                 arc has zero length and the center coordinates can lie
*                 anywhere on the circle traced around the points by the
*                 given radius.  No unique arcs can be determined.
*
*           2). When the distance between the start and stop points equals
*                 twice the radius, only one circle can be found containing
*                 which contains an arc and its compliment (2 arcs are
*                 possible; if direction is specified, arc is unique).
*
*           3). Otherwise, two unique circles can be found containing arcs
*                 and their compliments (4 arcs are possible; if direction
*                 is specified, 2 arcs are possible - 1 on each circle).
*
*     The appropriate case for the given parameter values is returned as
*     the function's value.
*
*  In general, the coordinates the circle(s) will lie on the perpendicular
*     bisector of the line connecting the start and stop points.  If the
*     general case must be calculated, the function creates the bisector,
*     calculates the distance to the circle center(s), and determines the
*     cx,cy offsets from the midpoint of the line connecting the start and
*     stop points.
*
*     center1(cx1,cy1) always on line(start-->stop)'s clockwise side.
*     center2(cx2,cy2) always on line(start-->stop)'s counter clockwise 
*     side.

\*******************************************************************/
//int ArcFrom2Pts( double x1, double y1, double x2, double y2, double radius,
//                  double *cx1, double *cy1, double *cx2, double *cy2 ) 
//{
//
//   double dx;                 /* x-distance between start and stop */
//   double dy;                 /* y-distance between start and stop */
//   double mx,my;              /* midpoint of line connecting start and stop */
//   double dl;                 /* distance between start and stop */
//   double dlm;                /* slope of line connecting start and stop */
//   double ds;                 /* distance between midpoint and centers */
//   double dsm;                /* slope of line connecting centers */
//   double dsa;                /* interrior angle of line connecting centers */
//   double dcx;                /* relative x to circle center from mx,my */
//   double dcy;                /* relative y to circle center from mx,my */
//
//   /*----------------------------------------------------------------\
//   | calculate deltas in x and y directions.
//   \----------------------------------------------------------------*/
//   dx = fabs(x2-x1);
//   dy = fabs(y2-y1);
//
//   /*----------------------------------------------------------------\
//   | calculate length and slope of line connecting start and stop points.
//   \----------------------------------------------------------------*/
//   dl = sqrt( dx*dx + dy*dy );
//
//   /*----------------------------------------------------------------\
//   | when (x1,y1) = (x2,y2), center could lie anywhere on locus of
//   |  points forming circle around (x1,y1).  check for unresolvalble
//   |  case.
//   \----------------------------------------------------------------*/
//   if( (dx > SMALLNUMBER) || (dy > SMALLNUMBER) )
//   {
//      /*----------------------------------------------------------------\
//      | check for invalid radius.
//      \----------------------------------------------------------------*/
//      if(radius > (dl/2 - SMALLNUMBER))
//      {
//         /*----------------------------------------------------------------\
//         | calculate midpoint of line connecting start and stop point.
//         \----------------------------------------------------------------*/
//         mx = (x2+x1)/2;
//         my = (y2+y1)/2;
//
//         /*----------------------------------------------------------------\
//         | check for simple case where centers lie on horizontal line
//         |  bisecting line connecting start and stop.
//         \----------------------------------------------------------------*/
//         if( dx < SMALLNUMBER )
//         {
//            if (y2 > y1)
//            {
//               double sx = sqrt( fabs(radius*radius - dy*dy/4) );
//               *cx1 = mx-sx;
//               *cx2 = mx+sx;
//            }
//            else
//            {
//               double sx = sqrt( fabs(radius*radius - dy*dy/4) );
//               *cx1 = mx+sx;
//               *cx2 = mx-sx;
//            }
//            *cy1 = *cy2 = my;
//         }
//
//         /*----------------------------------------------------------------\
//         | check for simple case where centers lie on vertical line
//         |  bisecting line connecting start and stop.
//         \----------------------------------------------------------------*/
//         else if( dy < SMALLNUMBER )
//         {
//            if (x2 > x1)
//            {
//               *cy1 = my+sqrt( fabs(radius*radius - dx*dx/4) );
//               *cy2 = my-sqrt( fabs(radius*radius - dx*dx/4) );
//            }
//            else
//            {
//               *cy1 = my-sqrt( fabs(radius*radius - dx*dx/4) );
//               *cy2 = my+sqrt( fabs(radius*radius - dx*dx/4) );
//            }
//            *cx1 = *cx2 = mx;
//         }
//
//         /*----------------------------------------------------------------\
//         | check general case.
//         \----------------------------------------------------------------*/
//         else
//         {
//            /*----------------------------------------------------------------\
//            |calculate slope of connecting line.
//            \----------------------------------------------------------------*/
//            dlm = (y2-y1)/(x2-x1);
//
//            /*----------------------------------------------------------------\
//            | calculate length, slope, and angle of perpendicular bisector of
//            |  line connecting start and stop point (through midpoint mx,my).
//            \----------------------------------------------------------------*/
//            ds = sqrt( fabs(radius*radius - dl*dl/4) );
//            dsm = -1/dlm;
//            dsa = atan(dsm);
//
//            /*----------------------------------------------------------------\
//            | calculate relative distances for arc center points.
//            \----------------------------------------------------------------*/
//            dcx = fabs(ds*cos(dsa));
//            dcy = fabs(ds*sin(dsa));
//
//            /*----------------------------------------------------------------\
//            | calculate center of both circles.
//            \----------------------------------------------------------------*/
//            if (y2 > y1 && x2 > x1) 
//            {
//               *cx1 = mx-dcx;
//               *cy1 = my+dcy;
//               *cx2 = mx+dcx;
//               *cy2 = my-dcy;
//            }
//            else if (y2 < y1 && x2 > x1)
//            {
//               *cx1 = mx+dcx;
//               *cy1 = my+dcy;
//               *cx2 = mx-dcx;
//               *cy2 = my-dcy;
//            }
//            else if (y2 > y1 && x2 < x1)
//            {
//               *cx1 = mx-dcx;
//               *cy1 = my-dcy;
//               *cx2 = mx+dcx;
//               *cy2 = my+dcy;
//            }
//            else//(y2 < y1 && x2 < x1)
//            {
//               *cx1 = mx+dcx;
//               *cy1 = my-dcy;
//               *cx2 = mx-dcx;
//               *cy2 = my+dcy;
//            }
//         }
//
//         /*----------------------------------------------------------------\
//         | check for only one unique circle (distance between start and
//         |  stop points equal 2*radius).
//         \----------------------------------------------------------------*/
//         if( fabs(radius-dl/2) < SMALLNUMBER )
//         {
//            return 1;
//         }
//         else
//         {
//            return 2;
//         }
//      }
//      else
//      {
//         /*----------------------------------------------------------------\
//         | unresolvable case, radius must be >= length (x1,y1)->(x2,y2)/2
//         \----------------------------------------------------------------*/
//         return 0;
//      }
//   }
//   else
//   {
//      /*----------------------------------------------------------------\
//      | unresolvable case (x1,y1) = (x2,y2), 0=no center x,y found
//      \----------------------------------------------------------------*/
//      return 0;
//   }
//   return 0;
//}


/****************************************************************************/
/*
      wolfgang 20-jun-89
*/
/* Polygon fill module V1.0 */

/* Standard library includes */
/*
   fill_polygon(poly, polycnt,step);       Polygon loading
   fillin(xa,xe,y);                        output routines for scan lines
*/

typedef   struct
{
   double   ymax;
   double   ymin;
   double   xa;
   double   dx;
} scr_ply_t;
                      /* Array zum merken der Linienzuege*/
#define ARMINI  0     /* Min Wert */
#define ARSIZE  400   /* Max Wert */

static scr_ply_t *sply;

#define PT2(x,y)  double x,double y

/*
  fill_scan : Routine zum Zeichen der Horizontalen LinienzÅge

  ruft fillin auf (linie zeichen von .. nach )
*/

static void  fill_scan(int end_edge,int start_edge,double scan,
                       int (*fillin)(double xa, double xe, double y))
{
  int nx,                /* die anzahl der zu zeichnenden strecken */
      j,k;               /* laufvariablen*/

  nx = (end_edge - start_edge) / 2;          /* anzahl der linien */
  for( j=start_edge, k=1 ;k<= nx;k++,j +=2)
       fillin(sply[j].xa,sply[j+1].xa,scan);
}

static void  poly_insert(int edges,PT2(x1,y1),PT2(x2,y2),double step )
{
   int j1 ;       /* laufvariable fuer die gespeicherten seiten*/
   double ym;     /* der maximale y wert der neuen kante */
                  /* einsetzsortierung in globale arrays nach maximalem y*/

   j1 = edges;
   ym =  ((y1>y2)?y1:y2);            /* das grî·te y bestimmen*/

   /* den korrekten einsetzpunkt bestimmen, dabei elemente
      aus dem weg bewegen
   */

   while( (j1 > 0) && (sply[j1-1].ymax < ym) )
   {
      sply[j1] = sply[j1-1];
      j1--;
   }
   sply[j1].ymax = ym;                  /* informationen ueber die kante
                                            einsetzen */
   // here is the inaccuracy. this must calculate the exact delta, not an
   // approx. If step gets smaller, it is more accurate.
   sply[j1].dx = ((x2 - x1) / (y2 - y1)) * (-step);
   if( y1 > y2 )       /*nachsehen welches ende oben liegt */
   {
      sply[j1].ymin = y2;
      sply[j1].xa   = x1;
   }
   else
   {
      sply[j1].ymin = y1;
      sply[j1].xa   = x2;
   }
} /*poly_insert*/

/* die reihenfolge der schnittstellen pruefen */
static void  xsort(int start_edge,int last_edge)
{
 int  k,l;              /* laufvariablen */
 double  t1;

  for( k= start_edge ;k<= last_edge;k++)
  {
    l = k;
    while( (l>start_edge) && (sply[l].xa <sply[l-1].xa) )
    {
      scr_ply_t t0;              /* tempspeicher */
      scr_ply_t *spl1=&sply[l],
      *spl2=&sply[l-1];
      t0 = *spl1; *spl1 = *spl2; *spl2 = t0;       /*strc swap*/
      t1 = spl1->ymax; spl1->ymax = spl2->ymax; spl2->ymax = t1;
      l--;
    }/*while*/
  }/*for*/
}/*xsort*/

static void  update_x_values(int last_edge,int *start_edge,double scan)
{
int k1,      /* index der kante, die fuer aktualisierung betr.wird*/
    k2;      /* index schleife ,an der die aktualiesierte kant.gespeichert
                 werden soll*/

  k2 = last_edge;
  for( k1 = last_edge ;k1>= *start_edge;k1--)
    if( sply[k1].ymin < scan )
    {
      sply[k2].xa = sply[k1].xa + sply[k1].dx;
      if( k1 != k2 )
      {
        sply[k2].ymin = sply[k1].ymin;
        sply[k2].dx   = sply[k1].dx;
      }
      k2--;
    }
  *start_edge = k2 +1;
}/*update_x_values*/

/******************************************************************/
/* Hauproutine fuer fillPolygon :              */
/*  entry : index -  zeiger auf Start in Polygontabelle           */
/******************************************************************/

/*
     sub of fill_polygon : load_polygon

   holt polygone aus Bildschirmpuffer und rechnet
   sie in Bildschirmwerte um

     entry :
      i  - zeiger auf structur mit array
     entry & return :
      edges : die zu liefernde anzahl der gespeicherten kanten
*/
static int  load_polygon( Point2 *i,int sides, double step)
{
   double px1,py1, px2,py2;
   int
   k,                    /* laufvariable fuer die polygonseiten */
   edges = ARMINI;       /* und init                            */
                         /* ende holen                          */
   px1 = i[sides-1].x;
   py1 = i[sides-1].y;

  /* passt die xy koordinate an die nÑchstgelegen abtastzeile an*/

   for( k=0 ;k< sides;k++)
   {
      px2 = i[k].x;
      py2 = i[k].y;

      if( py1 == py2 )
          px1 =  px2;
      else
      {                        /* speichere die daten Åber die kante */
                               /* nach dem grîssten y-wert sortiert  */
         poly_insert( edges,px1,py1,px2,py2,step);
         edges++;
                               /* alten punkt neu setzen */
         px1  = px2;
         py1  = py2;
      }    
   }
   return  --edges;                    /* set edges als anzahl der seiten */
} /* load_polygon */

/*----------------------------------------------------------------*/
/*               MAIN ROUTINE OF FILL POLYGON                     */
/*----------------------------------------------------------------*/

#define InClude(end_edge,final_edge,scan)  \
  while( (end_edge <= final_edge) &&      \
    (sply[end_edge].ymax >= scan) ) end_edge++;         

// there is this accorary error in poly_insert dx calculation. The trick
// now is to calc 10 times more accurate, but only write out every
// 10th step. 
#define  SCAN_DIVIDE 10
int fill_polygon(Point2 *index,int sides,double step,
                  int (*fillin)(double xa, double xe, double y))
{
   int   edges,start_edge,end_edge;
   double scan ;                /* position der letzten abtastzeile */

   step = step / SCAN_DIVIDE;  // for accuracy

   // allocate for filled poly
   if ((sply = (scr_ply_t *)calloc(ARSIZE, sizeof(scr_ply_t))) == NULL)
   {
      return 0;
   }

   edges = load_polygon(index,sides,step);
   if( edges < 1+ARMINI ) return 0;
   scan = sply[ARMINI].ymax;

   start_edge = end_edge   = ARMINI;
   InClude( end_edge, edges, scan);      /*inc end_edge if nessesary*/
   int loopcnt = 0;
   while( end_edge != start_edge )
   {
      xsort(start_edge,end_edge -1);
      if (!(loopcnt % SCAN_DIVIDE))   // only every 10th step
         fill_scan( end_edge,start_edge,scan,fillin);
      loopcnt++;
      scan -= step;
      update_x_values(end_edge-1, &start_edge, scan );
      InClude(end_edge,edges,scan);
   }
   free((char *)sply);
   return 1;
}/* fill_polygon */

/*======================== Ende of fill_polygon ============================*/

/******************************************************************************
* Rotate
*  - Rotation on given angle
*  - Input: (x,y) and angle in Degrees
*  - Output: Pointers to new (x,y)
*/
//void Rotate (double x, double y, double rot, double *xrot, double *yrot) 
//{
//   double sin_rot,tmp;
//   double cos_rot;
//   int   r = (int)floor(rot + 0.5);
//   int   i;
//
//   while (r < 0)     r += 360;
//   while (r >= 360)  r -= 360;
//
//   /* for accuracy seperate treatment for 90 increments */
//   if ((r % 90) == 0)
//   {
//      for (i=0;i<r / 90;i++)
//      {
//         tmp = x;
//         x = -y;
//         y = tmp;
//      }  
//      *xrot = x;
//      *yrot = y;
//   }
//   else
//   {
//      sin_rot = sin ((PI/180)*rot);
//      cos_rot = cos ((PI/180)*rot);
//      *xrot = x*cos_rot - y*sin_rot;
//      *yrot = x*sin_rot + y*cos_rot;
//   }
//}

/***********************************************************

Find corners takes an arc with width and makes the hollow outline 
around the arc even different width (start and end)

void FindCorners_arc(double bulge,Point2 *p1,Point2 *p2,
                      double w1,double w2,Point2 *p1a,Point2 *p1b,
                      Point2 *p2a, Point2 *p2b)
            --- p1 & p2 are the end points of the corner.
            --- w1 is p1's width, w2 is p2's width.
*  return:  --- p1a,p1b,p2a,p2b.(p1a & p2a in the same side of poly
                                 p1b & p2b in another same side of poly)  

the polygon goes from p1a -> p2a -> p2b ->p1b -> p1a

***********************************************************/
void FindCorners_arc(double bulge,Point2 *p1,Point2 *p2,
                      double w1,double w2,Point2 *p1a,Point2 *p1b,
                      Point2 *p2a, Point2 *p2b)
{
   double p1x, p1y, p2x, p2y, delta, start, radius;
   Point2 center;
   
   delta = atan(bulge) * 4;
   p1x = p1->x; p1y = p1->y;
   p2x = p2->x; p2y = p2->y;


   ArcPoint2Angle(p1x, p1y, p2x, p2y, delta, 
                     &center.x, &center.y, &radius, &start);
                     

   p1a->x = center.x + (radius + (w1 / 2)) * cos(start);
   p1a->y = center.y + (radius + (w1 / 2)) * sin(start);
   p1b->x = center.x + (radius - (w1 / 2)) * cos(start);
   p1b->y = center.y + (radius - (w1 / 2)) * sin(start);

   p2a->x = center.x + (radius + (w2 / 2)) * cos(start+delta);
   p2a->y = center.y + (radius + (w2 / 2)) * sin(start+delta);
   p2b->x = center.x + (radius - (w2 / 2)) * cos(start+delta);
   p2b->y = center.y + (radius - (w2 / 2)) * sin(start+delta);
}

/*************************************************************************

Find an hollow outline for with width (start and end width)

* FindCorners
*  Parameters -
*     - p1 & p2 are the start and end points of the autoCAD Pline
*     - w1 & w2 are the start width and end width of the autoCAD Pline
*
*  Returns -
*     - p1a,p1b,p2a,p2b are the four corners points
*       p1a & p1b stay with the start point p1 & start width w1
*       p2a & p2b stay with the end point p2 & end width w2
*
************************************************************************/
void FindCorners(double w1, double w2, Point2 *p1, Point2 *p2, 
                  Point2 *p1a, Point2 *p1b, Point2 *p2a, Point2 *p2b)
{
   double slope, p1p2;

   // undefinded slope
   if (fabs(p1->x - p2->x) < SMALLNUMBER)
   {
      p1a->x = p1->x - w1/2;
      p1b->x = p1->x + w1/2;
      p1a->y = p1b->y = p1->y;
      p2a->x = p2->x - w2/2;
      p2b->x = p2->x + w2/2;
      p2a->y = p2b->y = p2->y;
   }

   // has slope
   slope = (p2->y - p1->y) / (p2->x - p1->x);
   p1p2 = sqrt(pow(p1->x - p2->x,2) + pow(p1->y - p2->y,2));
   if (fabs(slope) < SMALLNUMBER) // no slope 
   {
      p1a->y = p1->y - w1/2;
      p1b->y = p1->y + w1/2;
      p1a->x = p1b->x = p1->x;
      p2a->y = p2->y - w2/2;
      p2b->y = p2->y + w2/2;
      p2a->x = p2b->x = p2->x;
   }
   else if (slope > 0.0)
   {
      p1a->x = p1->x - (fabs(p2->y - p1->y) * (w1/2) / p1p2); 
      p1a->y = p1->y + (fabs(p2->x - p1->x) * (w1/2) / p1p2);
      p1b->x = p1->x + (fabs(p2->y - p1->y) * (w1/2) / p1p2); 
      p1b->y = p1->y - (fabs(p2->x - p1->x) * (w1/2) / p1p2);
      p2a->x = p2->x - (fabs(p2->y - p1->y) * (w2/2) / p1p2); 
      p2a->y = p2->y + (fabs(p2->x - p1->x) * (w2/2) / p1p2);
      p2b->x = p2->x + (fabs(p2->y - p1->y) * (w2/2) / p1p2); 
      p2b->y = p2->y - (fabs(p2->x - p1->x) * (w2/2) / p1p2);
   }
   else // if (slope < 0.0)
   {
      p1a->x = p1->x - (fabs(p2->y - p1->y) * (w1/2) / p1p2); 
      p1a->y = p1->y - (fabs(p2->x - p1->x) * (w1/2) / p1p2);
      p1b->x = p1->x + (fabs(p2->y - p1->y) * (w1/2) / p1p2); 
      p1b->y = p1->y + (fabs(p2->x - p1->x) * (w1/2) / p1p2);
      p2a->x = p2->x - (fabs(p2->y - p1->y) * (w2/2) / p1p2); 
      p2a->y = p2->y - (fabs(p2->x - p1->x) * (w2/2) / p1p2);
      p2b->x = p2->x + (fabs(p2->y - p1->y) * (w2/2) / p1p2); 
      p2b->y = p2->y + (fabs(p2->x - p1->x) * (w2/2) / p1p2);
   }
}

/*************************************************************************
* FindShoot
*  Parameters -
*     - p1 & p2 are the start and end points of the autoCAD Pline
*     - w1 & w2 are the start width and end width of the autoCAD Pline
*     - p3 & p4 are the start and end points of the autoCAD Pline
*     - w3 & w4 are the start width and end width of the autoCAD Pline
*  
*  Returns -
*     - p is the shoot point which two box cross
*       
*
************************************************************************/
void FindShoot(double w1, double w2, double w3, double w4, Point2 *p1, 
               Point2 *p2,Point2 *p3, Point2 *p4, Point2 *p)
{


      // p1a,p1b,p2a,p2b are the four corners points
      // p1a & p1b stay with the start point p1 & start width w1
      // p2a & p2b   stay with the end point p2 & end width w2
   Point2 p1a,p1b,p2a,p2b,p3a,p3b,p4a,p4b;
   FindCorners(w1, w2, p1, p2, &p1a, &p1b, &p2a, &p2b);
   FindCorners(w3, w4, p3, p4, &p3a, &p3b, &p4a, &p4b);

   Point2 s1, s2;
   double slope1, slope2, slope3, slope4, 
            y_int1, y_int2, y_int3, y_int4;
   if (fabs(p2a.x - p1a.x) < SMALLNUMBER )  // undefined slope
   {  
      if (fabs(p4b.x - p3b.x) < SMALLNUMBER) // UNDEFINED SLOPE
      {
         double v1,v2;
         v1 = min(p4b.y, p3b.y);
         v2 = min(p2a.y, p1a.y);
         s2.x = fabs(p4b.x - p2a.x) / 2 + min(p4b.x, p2a.x);
         s2.y = min(v1, v2) + fabs(v1 - v2) / 2;
      }
      else  // has slope
      {
         s2.x = p2a.x;
         slope4 = (p4b.y - p3b.y) / (p4b.x - p3b.x);
         y_int4 = p3b.y - p3b.x * slope4;
         s2.y = s2.x * slope4 + y_int4;
      }
   }
   else 
   {
      if (fabs(p4b.x - p3b.x) < SMALLNUMBER) // UNDEFINED SLOPE
      {
         s2.x = p4b.x ;
         slope1 = (p2a.y - p1a.y) / (p2a.x - p1a.x);
         y_int1 = p1a.y - p1a.x * slope1;
         s2.y = s2.x * slope1 + y_int1;
      }
      else
      {
         slope1 = (p2a.y - p1a.y) / (p2a.x - p1a.x);
         y_int1 = p1a.y - p1a.x * slope1;
         slope4 = (p4b.y - p3b.y) / (p4b.x - p3b.x);
         y_int4 = p3b.y - p3b.x * slope4;
         if (fabs(slope1 - slope4) < SMALLNUMBER)
         {
            s2.x = fabs(min(p4b.x,p3b.x) - min(p2a.x,p1a.x)) / 2 
                     + min(min(p4b.x,p3b.x), min(p2a.x,p1a.x));
            s2.y = fabs(min(p4b.y,p3b.y) - min(p2a.y,p1a.y)) / 2 
                     + min(min(p4b.y,p3b.y), min(p2a.y,p1a.y));
         }
         else
         {
            s2.x = (y_int4 - y_int1) / (slope1 - slope4);
            s2.y = slope1 * s2.x + y_int1;
         }
      }
   }
   if ((s2.x >= max(p3b.x, p4b.x) || s2.x <= min(p3b.x, p4b.x))
      && (s2.x >= max(p2a.x, p1a.x) ||  s2.x <= min(p1a.x, p2a.x))
         && (s2.y >= max(p3b.y, p4b.y) || s2.y <= min(p3b.y, p4b.y))
         && (s2.y >= max(p1a.y, p2a.y) || s2.y <= min(p1a.y, p2a.y)))
   {
      p->x = s2.x;
      p->y = s2.y;
   }
   else
   {
      if (fabs(p2b.x - p1b.x) < SMALLNUMBER ) // undefined slope
      {  
         if (fabs(p4a.x - p3a.x) < SMALLNUMBER) // UNDEFINED SLOPE
         {
            double v1,v2;
            v1 = min(p4a.y,p3a.y) ;
            v2 = min(p2b.y,p1b.y);
            s1.x = fabs(p4a.x - p2b.x) / 2 + min(p4a.x,p2b.x);
            //s1.y = max(max(p4a.y,p3a.y), max(p2b.y,p1b.y)) 
            //    - fabs(max(p4a.y,p3a.y) - max(p2b.y,p1b.y)) / 2;
            s1.y = min(v1,v2);
         }
         else
         {
            s1.x = p2b.x;
            slope3 = (p4a.y - p3a.y) / (p4a.x - p3a.x);
            y_int3 = p3a.y - p3a.x * slope3;
            s1.y = s1.x * slope3 + y_int3;
         }
      }
      else 
      {
         if (fabs(p4a.x - p3a.x) < SMALLNUMBER) // UNDEFINED SLOPE
         {
            s1.x = p4a.x ;
            slope2 = (p2b.y - p1b.y) / (p2b.x - p1b.x);
            y_int2 = p1b.y - p1b.x * slope2;
            s1.y = s1.x * slope2 + y_int2;
         }
         else
         {
   
            slope2 = (p2b.y - p1b.y) / (p2b.x - p1b.x);
            y_int2 = p1b.y - p1b.x * slope2;
            slope3 = (p4a.y - p3a.y) / (p4a.x - p3a.x);
            y_int3 = p3a.y - p3a.x * slope3;
            if (fabs(slope2 - slope3) < SMALLNUMBER)
            {
               s1.x = fabs(min(p4a.x,p3a.x) - min(p2b.x,p1b.x)) / 2 
                        + min(min(p4a.x,p3a.x), min(p2b.x,p1b.x));
               s2.y = fabs(min(p4a.y,p3a.y) - min(p2b.y,p1b.y)) / 2 
                        + min(min(p4a.y,p3a.y), min(p2b.y,p1b.y));
            }
            else
            {
            s1.x = (y_int3 - y_int2) / (slope2 - slope3);
            s1.y = slope2 * s1.x + y_int2; 
            }
         }  
         p->x = s1.x;
         p->y = s1.y;
      }
   }

}

/*************************************************************************
*   void RoundCorner(Point2 *p_end1, Point2 *p_end2, Point2 *p_vertex, 
             double radius, double start, double delta, Point2 *center)
*
*  Parameters -
*     - p_end1, p_end2 & p_ver are the end points of two lines
*     - radius  is the radius of the arc
*  
*  Returns -
*       - center is the center of arc
*     - start is the start angle of the arc 
*       - delta is the angle from the start to end of the arc
*       - radius is the radius of the arc
*
*   Description -
*       - find the line which is pass the point p_ver and vertical to 
        p_end1--p_ver
*       then find another line which is pass the center of arc and 
*       parallel this line, then can find the points center of arc and
*       start 
*       then we can find the start and delta from the angles.
*
************************************************************************/
void RoundCorner(Point2 *p_end1, Point2 *p_end2, Point2 *p_ver, 
             double radius, double start, double delta, Point2 *center)
{
   double angle1, angle2, //  the angle of p_end1--p_ver and 
                     //  p_end2--p_ver
         angle,  // the angle of p_end1--p_ver--p_end2
         distance; // the distance of p_ver to start of the arc
                // and p_ver to end of the arc
   
   // find the angle of p_end1--p_ver--p_end2
   angle2 = ArcTan2(p_end2->y - p_ver->y, p_end2->x - p_ver->x);
   angle1 = ArcTan2(p_end1->y - p_ver->y, p_end1->x - p_ver->x);
   //because after ArcTan2(), -PI <= angle <= PI
   angle = angle2 - angle1;
   // find the positive angle less than PI
   if (angle < 0)
      angle = - angle;
   if (angle >= PI)
      angle = 2.0 * PI - angle;

   if (angle < SMALLNUMBER || fabs(angle - PI) < SMALLNUMBER) // angle too small
   {
      return;
   }

   distance = radius / tan(angle/2);
   double d1, d2; // the distance of p_end1--p_ver and p_end2--p_ver
   d1 = sqrt(pow(p_end1->x - p_ver->x,2) +
           pow(p_end1->y - p_ver->y,2));
   d2 = sqrt(pow(p_end2->x - p_ver->x,2) +
           pow(p_end2->y - p_ver->y,2));

   if (distance > d1 || distance > d2) //the line is too short to draw the arc
   {
      //TODO;
   }
   else // the line is long enought to draw the arc 
   {
      if (fabs(p_end1->x - p_ver->x) < SMALLNUMBER)
         //  p_end1--p_ver undefined but p_end2--p_ver can't undefined
      {
         
         if (p_end1->y > p_ver->y)
         {
            center->y = p_ver->y + distance;
            if (p_end1->x > p_end2->x)
            {
               center->x = p_ver->x - radius;
               start = 0; // arc clockwise
               delta = -(PI - angle);
            }
            else
            {
               center->x = p_ver->x + radius;
               start = PI; // arc negtive clockwise
               delta = PI - angle;
            }
         }
         else
         {
            center->y = p_ver->y - distance;
            if (p_end1->x > p_end2->x)
            {
               center->x = p_ver->x - radius;
               start = 0; // arc negtive clockwise
               delta = PI - angle;
            }
            else
            {
               center->x = p_ver->x + radius;
               start = PI; // arc  clockwise
               delta = -(PI - angle);
            }
         }
      } // end p_end1--p_ver undefined
      else if(fabs(p_end1->y - p_ver->y) < SMALLNUMBER) //  p_end1--p_vertex has no slope
      {
         if (p_end1->x > p_ver->x)
         {
            center->x = p_ver->x + distance;
            if (p_end1->y > p_end2->y)
            {
               center->y = p_ver->y - radius;
               start = PI/2; //arc negtive clockwise
               delta = PI - angle;
            }
            else
            {
               center->y = p_ver->y + radius;
               start = -PI/2; //arc clockwise
               delta = -(PI - angle);
            }
         }
         else
         {
            center->x = p_ver->x - distance;
            if (p_end1->y > p_end2->y)
            {
               center->y = p_ver->y - radius;
               start = PI/2; // arc clockwise
               delta = -(PI - angle);
            }
            else
            {
               center->y = p_ver->y + radius;
               start =  -PI/2; // arc negtive clockwise
               delta = PI - angle;
            }
         }
      }
      else  // p_end1--p_ver has slope
      {  
         if (fabs(p_end2->x - p_ver->x) < SMALLNUMBER)
         //  p_end2--p_ver undefined 
         {
         
            if (p_end2->y > p_ver->y)
            {
               center->y = p_ver->y + distance;
               if (p_end2->x > p_end1->x)
               {
                  center->x = p_ver->x - radius;
                  start = 0; // arc  clockwise
                  delta = -(PI - angle);
               }
               else
               {
                  center->x = p_ver->x + radius;
                  start = PI; // arc negtive clockwise
                  delta = PI - angle;
               }
            }
            else
            {
               center->y = p_ver->y - distance;
               if (p_end2->x > p_end1->x)
               {
                  center->x = p_ver->x - radius;
                   start = 0; // arc  negtive clockwise
                  delta = PI - angle;
               }
               else
               {
                  center->x = p_ver->x + radius;
                   start = PI; // arc  clockwise
                  delta = -(PI - angle);
               }
            }
         } // end p_end2--p_ver undefined
         else if(fabs(p_end2->y - p_ver->y) < SMALLNUMBER) //  p_end1--p_vertex has no slope
         {
            if (p_end2->x > p_ver->x)
            {
               center->x = p_ver->x + distance;
               if (p_end2->y > p_end1->y)
               {
                  center->y = p_ver->y - radius;
                  start = PI/2; //arc negtive clockwise
                  delta = PI - angle;
               }
               else
               {
                  center->y = p_ver->y + radius;
                   start = -PI/2; //arc clockwise
                  delta = -(PI - angle);
               }
            }
            else
            {
               center->x = p_ver->x - distance;
               if (p_end2->y > p_end1->y)
               {
                  center->y = p_ver->y - radius;
                  start = PI/2; // arc  clockwise
                  delta = -(PI - angle);
               }
               else
               {
                  center->y = p_ver->y + radius;
                  start = -PI/2; // arc  negtive clockwise
                  delta = PI - angle;
               }
            }
         } // end of p_end2--p_ver no slope
         else // p_end2--p_ver has slope
         {
            // l1 is the line which pass p_ver and vertical to 
            // p_end1--p_ver
            // L1 is the line which pass center and vertical to 
            // p_end1--p_ver
            // find the line(L1) which pass center and parallel the line(l1)
            // find the line(L2) which pass center and parallel the line(l2)
         
            double  slope1, // the slope of the line l1 and L1
                  slope2, // the slope of the line l2 and L2
                  y_int_1, // the y_int of the line l1
                  y_int1, // the y_int of the line L1
                        y_int_2, // the y_int of the line l2
                  y_int2; // the y_int of the line L2 
            slope1 = (p_ver->x - p_end1->x) / (p_end1->y - p_ver->y);
            slope2 = (p_ver->x - p_end2->x) / (p_end2->y - p_ver->y);
            y_int_1 = p_ver->y - slope1 * p_ver->x;
            y_int_2 = p_ver->y - slope2 * p_ver->x;
            if (p_end1->y > p_ver->y)
            {
               if (p_end1->x > p_ver->x)
                  // arc negtive clockwise
               {
                  start = PI + atan(slope1);
                  delta = PI - angle;
               }
               else // arc  clockwise
               {
                  start = atan(slope1);
                  delta = -(PI - angle);
               }
            }
            else
            {
               if (p_end1->x > p_ver->x)
                   // arc clockwise
               {
                  start = PI + atan(slope1);
                  delta = -(PI - angle);
               }
               else // arc  negtive clockwise
               {
                  start= atan(slope1);
                  delta = PI - angle;
               }
            }
            
            if (p_end1->y > p_ver->y)
               y_int1 = y_int_1 + distance / cos(atan(slope1)); 
            else
               y_int1 = y_int_1 - distance / cos(atan(slope1)); 
            if (p_end2->y > p_ver->y)
               y_int2 = y_int_2 + distance / cos(atan(slope2)); 
            else
               y_int2 = y_int_2 - distance / cos(atan(slope2)); 
            center->x = (y_int2 - y_int1) / (slope1 - slope2);
            center->y = slope1 * center->x + y_int1;
         } // end of p_end2--p_ver has slope
      
      }   // end of p_end1--p_ver has slope
   }  // end of the line is long engouth to draw the arc
}
// End of RoundCorner

/******************************************************************************
* normalize_text
   In double and degree
   Mirror mirrors around x axis.
*/
void normalize_text(double *x, double *y, DbFlag flg, double rot, unsigned char mir, double height, double len)
{
   double   l = len, h = height;

   if (flg == 0)
      return;

   // this here is changed. First the text is mirrored and then rotate.
   if (mir)
      l = -l;

   if (flg & GRTEXT_W_C)
      l = l/2;
   else if (flg & GRTEXT_W_R)
      ;// l = l;
   else
      l = 0;

   if (flg & GRTEXT_H_C)
      h = h/2;
   else if (flg & GRTEXT_H_T)
      ;// h = h
   else
      h = 0;

   double tx = 0, ty = 0;
   Rotate(l, h, rot, &tx, &ty);

   *x -= tx;
   *y -= ty;
}



/***************************************************
int Lines_overlapping(Point2 *x1,Point2 *x2,Point2 *y1,Point2 *y2,
                       Point2 *p1,Point2 *p2)

PARAMETERS:
   segment1       --- x1 to x2
   segment2       --- y1 to y2

RETURN:
   output segment --- p1 to p2
   return 1       --- if segments overlap
   return 0       --- if segments don't overlap

DESCRIPTION:
   ?????

****************************************************************************/
int Lines_overlapping(Point2 *x1,Point2 *x2,Point2 *y1,Point2 *y2,
                       Point2 *p1,Point2 *p2)
{
   double mx, my, bx, by;
   if( fabs(x1->x - x2->x) > SMALLNUMBER ) // if not vertical (x-line)
   {
      mx = (x2->y - x1->y)/(x2->x - x1->x);
      bx = x1->y - x1->x * (x2->y - x1->y) / (x2->x - x1->x); 
      if( fabs(y1->x - y2->x) > SMALLNUMBER ) // if not vertical (y-line)
      {
         my = (y2->y - y1->y)/(y2->x - y1->x);
         by = y1->y - y1->x * (y2->y - y1->y) / (y2->x - y1->x);   
         if( fabs(mx - my) < SMALLNUMBER ) // if same slope (parallel or same line)
         {
            if (fabs(bx-by)< SMALLNUMBER) // operlapping
            {
               if (fabs(mx) < SMALLNUMBER) // if horizontal (x-line)
               {
                  if (max(x1->x,x2->x) < max(y1->x,y2->x) + SMALLNUMBER &&
                   max(x1->x,x2->x) > min(y1->x,y2->x) - SMALLNUMBER &&
                   min(y1->x,y2->x) < max(x1->x,x2->x) + SMALLNUMBER &&
                   min(y1->x,y2->x) > min(x1->x,x2->x) - SMALLNUMBER )
                  {
                     p1->y = p2->y = x1->y;
                     p1->x = max(x1->x, x2->x);
                     p2->x = min(y1->x, y2->x);
                     return 1;
                  }
                  else if (max(y1->x,y2->x) < max(x1->x,x2->x) + SMALLNUMBER &&
                           max(y1->x,y2->x) > min(x1->x,x2->x) - SMALLNUMBER &&
                           min(x1->x,x2->x) < max(y1->x,y2->x) + SMALLNUMBER &&
                           min(x1->x,x2->x) > min(y1->x,y2->x) - SMALLNUMBER )
                  {
                     p1->y = p2->y = x1->y;
                     p1->x = min(x1->x, x2->x);
                     p2->x = max(y1->x, y2->x);
                     return 1;
                  }
                  else if (max(x1->x,x2->x) < max(y1->x,y2->x) + SMALLNUMBER &&
                           max(x1->x,x2->x) > min(y1->x,y2->x) - SMALLNUMBER &&
                           min(x1->x,x2->x) < max(y1->x,y2->x) + SMALLNUMBER &&
                           min(x1->x,x2->x) > min(y1->x,y2->x) - SMALLNUMBER )
                  {
                     p1->y = p2->y = x1->y;
                     p1->x = x1->x;
                     p2->x = x2->x;
                     return 1;
                  }
                  else if (max(y1->x,y2->x) < max(x1->x,x2->x) + SMALLNUMBER &&
                           max(y1->x,y2->x) > min(x1->x,x2->x) - SMALLNUMBER &&
                           min(y1->x,y2->x) < max(x1->x,x2->x) + SMALLNUMBER &&
                           min(y1->x,y2->x) > min(x1->x,x2->x) - SMALLNUMBER )
                  {
                     p1->y = p2->y = x1->y;
                     p1->x = y1->x;
                     p2->x = y2->x;
                     return 1;
                  }
               }
               else if (mx > SMALLNUMBER) // if positive slope (x-line)
               {
                  if (max(x1->x,x2->x) < max(y1->x,y2->x) + SMALLNUMBER &&
                      max(x1->x,x2->x) > min(y1->x,y2->x) - SMALLNUMBER &&
                      min(y1->x,y2->x) < max(x1->x,x2->x) + SMALLNUMBER &&
                      min(y1->x,y2->x) > min(x1->x,x2->x) - SMALLNUMBER &&
                      max(x1->y,x2->y) < max(y1->y,y2->y) + SMALLNUMBER &&
                      max(x1->y,x2->y) > min(y1->y,y2->y) - SMALLNUMBER &&
                      min(y1->y,y2->y) < max(x1->y,x2->y) + SMALLNUMBER &&
                      min(y1->y,y2->y) > min(x1->y,x2->y) - SMALLNUMBER)
                  {
                     p1->x = max(x1->x, x2->x);
                     p2->x = min(y1->x, y2->x);
                     p1->y = max(x1->y, x2->y);
                     p2->y = min(y1->y, y2->y);
                     return 1;
                  }
                  else if (max(y1->x,y2->x) < max(x1->x,x2->x) + SMALLNUMBER &&
                           max(y1->x,y2->x) > min(x1->x,x2->x) - SMALLNUMBER &&
                           min(x1->x,x2->x) < max(y1->x,y2->x) + SMALLNUMBER &&
                           min(x1->x,x2->x) > min(y1->x,y2->x) - SMALLNUMBER &&
                           max(y1->y,y2->y) < max(x1->y,x2->y) + SMALLNUMBER &&
                           max(y1->y,y2->y) > min(x1->y,x2->y) - SMALLNUMBER &&
                           min(x1->y,x2->y) < max(y1->y,y2->y) + SMALLNUMBER &&
                           min(x1->y,x2->y) > min(y1->y,y2->y) - SMALLNUMBER)
                  {
                     p1->x = min(x1->x, x2->x);
                     p2->x = max(y1->x, y2->x);
                     p1->y = min(x1->y, x2->y);
                     p2->y = max(y1->y, y2->y);
                     return 1;
                  }
                  else if (max(x1->x,x2->x) < max(y1->x,y2->x) + SMALLNUMBER &&
                           max(x1->x,x2->x) > min(y1->x,y2->x) - SMALLNUMBER &&
                           min(x1->x,x2->x) < max(y1->x,y2->x) + SMALLNUMBER &&
                           min(x1->x,x2->x) > min(y1->x,y2->x) - SMALLNUMBER &&
                           max(x1->y,x2->y) < max(y1->y,y2->y) + SMALLNUMBER &&
                           max(x1->y,x2->y) > min(y1->y,y2->y) - SMALLNUMBER &&
                           min(x1->y,x2->y) < max(y1->y,y2->y) + SMALLNUMBER &&
                           min(x1->y,x2->y) > min(y1->y,y2->y) - SMALLNUMBER)
                  {
                     p1->y = x1->y;
                     p2->y = x2->y;
                     p1->x = x1->x;
                     p2->x = x2->x;
                     return 1;
                  }
                  else if (max(y1->x,y2->x) < max(x1->x,x2->x) + SMALLNUMBER &&
                           max(y1->x,y2->x) > min(x1->x,x2->x) - SMALLNUMBER &&
                           min(y1->x,y2->x) < max(x1->x,x2->x) + SMALLNUMBER &&
                           min(y1->x,y2->x) > min(x1->x,x2->x) - SMALLNUMBER &&
                           max(y1->y,y2->y) < max(x1->y,x2->y) + SMALLNUMBER &&
                           max(y1->y,y2->y) > min(x1->y,x2->y) - SMALLNUMBER &&
                           min(y1->y,y2->y) < max(x1->y,x2->y) + SMALLNUMBER &&
                           min(y1->y,y2->y) > min(x1->y,x2->y) - SMALLNUMBER)
                  {
                     p1->y = y1->y;
                     p2->y = y2->y;
                     p1->x = y1->x;
                     p2->x = y2->x;
                     return 1;
                  }
               }
               else if (mx < -SMALLNUMBER) // if negative slope (x-line)
               {
                  if (max(x1->x,x2->x) < max(y1->x,y2->x) + SMALLNUMBER &&
                      max(x1->x,x2->x) > min(y1->x,y2->x) - SMALLNUMBER &&
                      min(y1->x,y2->x) < max(x1->x,x2->x) + SMALLNUMBER &&
                      min(y1->x,y2->x) > min(x1->x,x2->x) - SMALLNUMBER &&
                      min(x1->y,x2->y) < max(y1->y,y2->y) + SMALLNUMBER &&
                      min(x1->y,x2->y) > min(y1->y,y2->y) - SMALLNUMBER &&
                      max(y1->y,y2->y) < max(x1->y,x2->y) + SMALLNUMBER &&
                      max(y1->y,y2->y) > min(x1->y,x2->y) - SMALLNUMBER)
                  {
                     p1->x = max(x1->x, x2->x);
                     p2->x = min(y1->x, y2->x);
                     p1->y = min(x1->y, x2->y);
                     p2->y = max(y1->y, y2->y);
                     return 1;
                  }
                  else if (max(y1->x,y2->x) < max(x1->x,x2->x) + SMALLNUMBER &&
                           max(y1->x,y2->x) > min(x1->x,x2->x) - SMALLNUMBER &&
                           min(x1->x,x2->x) < max(y1->x,y2->x) + SMALLNUMBER &&
                           min(x1->x,x2->x) > min(y1->x,y2->x) - SMALLNUMBER &&
                           min(y1->y,y2->y) < max(x1->y,x2->y) + SMALLNUMBER &&
                           min(y1->y,y2->y) > min(x1->y,x2->y) - SMALLNUMBER &&
                           max(x1->y,x2->y) < max(y1->y,y2->y) + SMALLNUMBER &&
                           max(x1->y,x2->y) > min(y1->y,y2->y) - SMALLNUMBER)
                  {
                     p1->x = min(x1->x, x2->x);
                     p2->x = max(y1->x, y2->x);
                     p1->y = max(x1->y, x2->y);
                     p2->y = min(y1->y, y2->y);
                     return 1;
                  }
                  else if (max(x1->x,x2->x) < max(y1->x,y2->x) + SMALLNUMBER &&
                           max(x1->x,x2->x) > min(y1->x,y2->x) - SMALLNUMBER &&
                           min(x1->x,x2->x) < max(y1->x,y2->x) + SMALLNUMBER &&
                           min(x1->x,x2->x) > min(y1->x,y2->x) - SMALLNUMBER &&
                           max(x1->y,x2->y) < max(y1->y,y2->y) + SMALLNUMBER &&
                           max(x1->y,x2->y) > min(y1->y,y2->y) - SMALLNUMBER &&
                           min(x1->y,x2->y) < max(y1->y,y2->y) + SMALLNUMBER &&
                           min(x1->y,x2->y) > min(y1->y,y2->y) - SMALLNUMBER)
                  {
                     p1->y = x1->y;
                     p2->y = x2->y;
                     p1->x = x1->x;
                     p2->x = x2->x;
                     return 1;
                  }
                  else if (max(y1->x,y2->x) < max(x1->x,x2->x) + SMALLNUMBER &&
                           max(y1->x,y2->x) > min(x1->x,x2->x) - SMALLNUMBER &&
                           min(y1->x,y2->x) < max(x1->x,x2->x) + SMALLNUMBER &&
                           min(y1->x,y2->x) > min(x1->x,x2->x) - SMALLNUMBER &&
                           max(y1->y,y2->y) < max(x1->y,x2->y) + SMALLNUMBER &&
                           max(y1->y,y2->y) > min(x1->y,x2->y) - SMALLNUMBER &&
                           min(y1->y,y2->y) < max(x1->y,x2->y) + SMALLNUMBER &&
                           min(y1->y,y2->y) > min(x1->y,x2->y) - SMALLNUMBER)
                  {
                     p1->y = y1->y;
                     p2->y = y2->y;
                     p1->x = y1->x;
                     p2->x = y2->x;
                     return 1;
                  }
               }
            }
            return 0; // lines are parallel, never intersect 
         }
         return 0; //lines are not operlapping
      }
      else
      {  // y-line vertical 
         return 0;//lines are not operlapping
      }
      return 0;
   }
   else // x-line vertical 
   {  
      if( fabs(y1->x - y2->x) > SMALLNUMBER )  // if not vertical (y-line)
      {
         return 0;//lines not operlapping
      } 
      else // y-line also vertical
      {
         if (fabs(x1->x - y1->x) < SMALLNUMBER) // operlapping
         {
            if (max(x1->y,x2->y) < max(y1->y,y2->y) + SMALLNUMBER &&
                max(x1->y,x2->y) > min(y1->y,y2->y) - SMALLNUMBER &&
                min(y1->y,y2->y) < max(x1->y,x2->y) + SMALLNUMBER &&
                min(y1->y,y2->y) > min(x1->y,x2->y) - SMALLNUMBER )
            {
               p1->x = p2->x = x1->x;
               p1->y = max(x1->y, x2->y);
               p2->y = min(y1->y, y2->y);
               return 1;
            }
            else if (max(y1->y,y2->y) < max(x1->y,x2->y) + SMALLNUMBER &&
                     max(y1->y,y2->y) > min(x1->y,x2->y) - SMALLNUMBER &&
                     min(x1->y,x2->y) < max(y1->y,y2->y) + SMALLNUMBER &&
                     min(x1->y,x2->y) > min(y1->y,y2->y) - SMALLNUMBER )
            {
               p1->x = p2->x = x1->x;
               p1->y = min(x1->y, x2->y);
               p2->y = max(y1->y, y2->y);
               return 1;
            }
            else if (max(x1->y,x2->y) < max(y1->y,y2->y) + SMALLNUMBER &&
                     max(x1->y,x2->y) > min(y1->y,y2->y) - SMALLNUMBER &&
                     min(x1->y,x2->y) < max(y1->y,y2->y) + SMALLNUMBER &&
                     min(x1->y,x2->y) > min(y1->y,y2->y) - SMALLNUMBER )
            {
               p1->x = p2->x = x1->x;
               p1->y = x1->y;
               p2->y = x2->y;
               return 1;
            }
            else if (max(y1->y,y2->y) < max(x1->y,x2->y) + SMALLNUMBER &&
                     max(y1->y,y2->y) > min(x1->y,x2->y) - SMALLNUMBER &&
                     min(y1->y,y2->y) < max(x1->y,x2->y) + SMALLNUMBER &&
                     min(y1->y,y2->y) > min(x1->y,x2->y) - SMALLNUMBER )
            {
               p1->x = p2->x = x1->x;
               p1->y = y1->y;
               p2->y = y2->y;
               return 1;
            }
            return 0;
         }
         return 0; // lines parallel, never intersect 
      }
      return 0;
   }
   return 0;
}//end of Lines_operlapping

 
/***************************************************
int ArcFromStartEndRadius(double x1,double y1,double x2,double y2,
                       double radius,BOOL Clockwise,BOOL Smaller,
                       double *cx,double *cy,double *start,double *delta)

PARAMETERS:
   start point    ---(x1,y1)
   stop point     ---(x2,y2)
   radius         ---the radius of arc
   clockwise   1  --- if arc is clockwise
               0  --- if arc is counter clockwise
   Smaller     1  --- if the arc is small than PI
               0  --- if the arc is grate than PI

RETURN:
   center         ---(cx,cy)
   start          ---the angle of start
   delta          ---the angle between start and stop
   return 1  ---- if resolvable case
   return 0  ---- if unresolvable case (start and stop are same points
                                    or start--stop longer than 2*radius)   


DESCRIPTION:
   Calculates the center coordinates for the arc(s) specified by 
   start coordinates, stop coordinates, and a radius. also, Clockwise or not,
   Smaller or not.
   determines  the center and start and delta for the arc.

/****************************************************************************/
//int ArcFromStartEndRadius(double x1,double y1,double x2,double y2,
//                       double radius,int Clockwise,int Smaller,
//                       double *cx,double *cy,double *start,double *delta)
//{
//   double cx1,cy1,cx2,cy2;
//   int n;
//   n = ArcFrom2Pts(x1,y1,x2,y2,radius,&cx1,&cy1,&cx2,&cy2);
//   if (n == 0)
//      return 0;
//   if (Clockwise ^ Smaller)
//   {
//      *cx = cx1;
//      *cy = cy1;
//   }
//   else 
//   {
//      *cx = cx2;
//      *cy = cy2;
//   }
//
//   *start = ArcTan2((y1 - *cy),(x1 - *cx));
//   *delta = fabs(ArcTan2((y2 - *cy),(x2 - *cx)) - *start);
//   if (*delta > PI)
//      *delta = 2*PI - *delta;
//   if (Clockwise && !Smaller)
//      *delta = -2*PI + *delta;
//   else if (!Clockwise && !Smaller)
//      *delta = 2*PI - *delta;
//   else if (Clockwise && Smaller)
//      *delta = - *delta;
//
//  return 1;
//}

/***************************************************
int ArcFromStartEndRadiusDeltaXY(double x1, double y1, double x2, double y2, 
											double radius,int Clockwise, double startDeltaX, double startDeltaY,
											double *cx, double *cy, double *start, double *delta)

PARAMETERS:
   start point    --- (x1,y1)
   stop point     --- (x2,y2)
   radius         --- the radius of arc
   clockwise true --- if arc is clockwise
             false--- if arc is counter clockwise
	start delta pt --- (startDeltaX,startDeltaY)
   Smaller     1  --- if the arc is small than PI
               0  --- if the arc is grate than PI

RETURN:
   center         --- (cx,cy)
   start          --- the angle of start
   delta          --- the angle between start and stop
   return 1  ---- if resolvable case
   return 0  ---- if unresolvable case (start and stop are same points
                                    or start--stop longer than 2*radius)   


DESCRIPTION:
   Calculates the center coordinates for the arc(s) specified by 
   start coordinates, stop coordinates, and a radius. Clockwise or not, and the 
	start delta X and Y.
   determines  the center and start and delta for the arc.
/****************************************************************************/
int ArcFromStartEndRadiusDeltaXY(double x1, double y1, double x2, double y2, 
											double radius, bool clockwise, double startDeltaX, double startDeltaY,
											double *cx, double *cy, double *start, double *delta, double tolerance)
{
	*cx = 0.0;
	*cy = 0.0;
	*start = 0.0;
	*delta = 0.0;

   double cx1,cy1,cx2,cy2;
   int n = ArcFrom2Pts(x1, y1, x2, y2, radius, &cx1, &cy1, &cx2, &cy2);

   if (n == 0)
      return 0;

   // The direction vector from the start point to the center point is specified by (startDeltaX,startDeltaY)

   double dx1 = cx1 - x1;
   double dy1 = cy1 - y1;
   double dx2 = cx2 - x1;
   double dy2 = cy2 - y1;

   double direction1   = atan2(dy1,dx1);
   double direction2   = atan2(dy2,dx2);
   double arcDirection = atan2(startDeltaY,startDeltaX);

   double deltaDirection1 = fabs(arcDirection - direction1);
   double deltaDirection2 = fabs(arcDirection - direction2);

#define SMALL_RADIAN_NUMBER  0.005

   if (deltaDirection1 < SMALL_RADIAN_NUMBER )
   {
		*cx = cx1;
		*cy = cy1;
   }
   else if (deltaDirection2 < SMALL_RADIAN_NUMBER)
   {
		*cx = cx2;
		*cy = cy2;
   }
	else
	{
		// Possible round off error has caused reversal in signs of atan2 result.
		// Reverse signs of directionX (by adding instead of subtracting) and try again.
		deltaDirection1 = fabs(arcDirection + direction1);
		deltaDirection2 = fabs(arcDirection + direction2);
		if (deltaDirection1 < SMALL_RADIAN_NUMBER )
		{
			*cx = cx1;
			*cy = cy1;
		}
		else if (deltaDirection2 < SMALL_RADIAN_NUMBER)
		{
			*cx = cx2;
			*cy = cy2;
		}
		else
		{
			// Failed, return "no result"
			return 0;
		}
	}

	double startAngle = atan2((y1 - *cy),(x1 - *cx));
	double endAngle   = atan2((y2 - *cy),(x2 - *cx));

   if (endAngle < startAngle) 
		endAngle += PI2; // normalize for positive delta

	double deltaAngle = endAngle - startAngle;

	if (clockwise)
		deltaAngle -= PI2;

	*start = startAngle;
   *delta = deltaAngle;

	return 1;
}

/***************************************************
double   AngleStartEndDegree(double start, double end, int clockwise)

PARAMETERS:
   start = start angle in degree
   end   = end angle in degree
   clockwise = BOOL for clockwise, FALSE for counter clock

RETURN:
   da = delta angle in degree
        positive for counter clock, neg for clock
DESCRIPTION:

/****************************************************************************/
double   AngleStartEndDegree(double start, double end, int clockwise)
{
   double   da;
   while (start < 0)    start += 360;
   while (start >= 360) start -= 360;
   while (end < 0)      end += 360;
   while (end >= 360)   end -= 360;

   if (!clockwise)
   {
      if (start < end)
      {
         da = end - start;
      }
      else
      {
         da = 360 - start + end;
      }  
   }
   else  // here clockwise
   {
      if (start < end)
      {
         da = end - start - 360;
      }
      else
      {
         da = end  - start;
      }  
   }

   if (fabs(da) > 360)
   {
      if (da < 0) 
         da += 360;
      else
         da -= 360;
   }

   return da;
}


/****************************************************************************
* int SegmentsOverlap(double l1ax, double l1ay, double l1bx, double l1by, double l2ax, double l2ay, double l2bx, double l2by)
*
* DESCRIPTION:
*     --- decides if lines overlap
*
* PARAMETERS:
*     --- l1ax, l1ay - point a of line 1
*     --- l1bx, l1by - point b of line 1
*     --- l2ax, l2ay - point a of line 2
*     --- l2bx, l2by - point b of line 2
*
* RETURNS:
*     - OVERLAP_NONE - do not overlap
*     - OVERLAP_1in2 - line 1 is contained in line 2
*     - OVERLAP_2in1 - line 2 is contained in line 1
*     - OVERLAP_OVERLAPPING - there is an overlap of the two lines
*/  
int SegmentsOverlap(double l1ax, double l1ay, double l1bx, double l1by,
                    double l2ax, double l2ay, double l2bx, double l2by,
                    double accuracy)
{
   /*** check line slopes ***/
   double m1, m2;
   BOOL Undefined1 = FALSE, Undefined2 = FALSE;

   // line 1
   if (fabs(l1ax - l1bx) < accuracy) 
      Undefined1 = TRUE;
   else
      m1 = (l1ay - l1by) / (l1ax - l1bx);

   // line 2
   if (fabs(l2ax - l2bx) < accuracy) 
      Undefined2 = TRUE;
   else
      m2 = (l2ay - l2by) / (l2ax - l2bx);

   // compare
   if (Undefined1 ^ Undefined2) // one undefined slope and one not
      return OVERLAP_NONE;
   if (!(Undefined1 && Undefined2)) // if not both undefined slopes
      if (fabs(m1 - m2) > ALMOSTZERO) // if different slopes
         return OVERLAP_NONE;


   /*** check line y-intercepts ***/
   // if got here, then have same slope (parallel)
   // see if on same line
   if (Undefined1 && Undefined2) // if both undefined, check x-intercepts
   {
      if (fabs(l1ax - l2ax) > accuracy)
         return OVERLAP_NONE;
   }
   else // parallel lines with defined slope
   {
      double b1, b2;
      b1 = l1ay - m1 * l1ax;
      b2 = l2ay - m2 * l2ax;
      if (fabs(b1 - b2) > ALMOSTZERO)
         return OVERLAP_NONE;
   }


   /*** check segments ***/
   // if got here, then on same infinite line
   // see segments overlap
   double min1, max1, min2, max2;
   if (Undefined1 && Undefined2) // if both undefined, check y range
   {
      min1 = min(l1ay, l1by);
      max1 = max(l1ay, l1by);
      min2 = min(l2ay, l2by);
      max2 = max(l2ay, l2by);
   }
   else // defined slopes, check x range
   {
      min1 = min(l1ax, l1bx);
      max1 = max(l1ax, l1bx);
      min2 = min(l2ax, l2bx);
      max2 = max(l2ax, l2bx);
   }

   // if overlap
   if (max2-min1>-accuracy && max1-min2>-accuracy) // (min1 <= max2 && min2 <= max1) 
   {
      if (min1 >= min2 && max1 <= max2)
         return OVERLAP_1in2;
      if (min2 >= min1 && max2 <= max1)
         return OVERLAP_2in1;

      // figure intersection
      return OVERLAP_OVERLAPPING;
   }

   return OVERLAP_NONE;
}

/*****************************************************************************/
/*
*/
double   LengthOfArc(Point2 *center, double radius, double startangle, double deltaangle)
{
   double   length;
   PT2      pt0;

   c2v_set(center->x, center->y, pt0);

   C2_CURVE    curve;
   int         dir = 1; // 1 = counterclock, -1 = clock

   if (deltaangle < 0)  dir = -1;
   curve = c2d_arc ( pt0, radius, startangle, deltaangle, dir );

   length = c2c_length(curve);
   c2d_free_curve( curve);

   return length; 
}

