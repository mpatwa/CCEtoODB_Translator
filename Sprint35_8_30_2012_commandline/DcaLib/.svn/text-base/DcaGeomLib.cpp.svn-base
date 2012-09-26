// $Header: /CAMCAD/DcaLib/DcaGeomLib.cpp 5     3/23/07 5:19a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaGeomLib.h"
#include "Dca.h"
#include "DcaPnt.h"
#include "DcaBuildingBlockLib.h"
#include "DcaPoint2.h"
#include <Math.h>

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if defined(EnableDcaCamCadDocLegacyCode)

#include "DcaData.h"
#include "DcaExtent.h"
#include "DcaLib.h"

void RotMat2( Mat2x2 *m, double az ) 
{
   m->m2[ 0 ][ 0 ] = cos( az );
   m->m2[ 0 ][ 1 ] = -sin( az );
   m->m2[ 1 ][ 0 ] = sin( az );
   m->m2[ 1 ][ 1 ] = cos( az );
}

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

bool PolyPntExtents(CPntList *pntList, ExtentRect *extents, double scale, int mirror,
      double insert_x, double insert_y, Mat2x2 *m, double width)
{
	if (pntList == NULL)
		return false;

   CPnt *pnt;
   Point2 vertex, last;
   double bulge;

   POSITION pntPos = pntList->GetHeadPosition();
   bool retval = (pntPos != NULL);

   if (retval)
   {
      pnt = pntList->GetNext(pntPos);
      vertex.x = pnt->x * scale;  vertex.y = pnt->y * scale;

      if (mirror & MIRROR_FLIP) vertex.x = -vertex.x;

      TransPoint2(&vertex, 1, m, insert_x, insert_y);

      bulge = pnt->bulge;

      extents->left   = ( (vertex.x-width <= extents->left  ) ? vertex.x-width : extents->left  );
      extents->right  = ( (vertex.x+width >= extents->right ) ? vertex.x+width : extents->right );
      extents->top    = ( (vertex.y+width >= extents->top   ) ? vertex.y+width : extents->top   );
      extents->bottom = ( (vertex.y-width <= extents->bottom) ? vertex.y-width : extents->bottom);

      while (pntPos != NULL)
      {
         pnt = pntList->GetNext(pntPos);

         last.x = vertex.x;  last.y = vertex.y;

         vertex.x = pnt->x * scale;  vertex.y = pnt->y * scale;

         if (mirror & MIRROR_FLIP) vertex.x = -vertex.x;

         TransPoint2(&vertex, 1, m, insert_x, insert_y);

         extents->left   = ( (vertex.x-width <= extents->left  ) ? vertex.x - width : extents->left  );
         extents->right  = ( (vertex.x+width >= extents->right ) ? vertex.x + width : extents->right );
         extents->top    = ( (vertex.y+width >= extents->top   ) ? vertex.y + width : extents->top   );
         extents->bottom = ( (vertex.y-width <= extents->bottom) ? vertex.y - width : extents->bottom);

         if (fabs(bulge) > SMALLNUMBER) // bulge
         {
            double da, sa, r, cx, cy;
            da = atan(bulge) * 4;

            if (mirror & MIRROR_FLIP) da = -da;

            ArcPoint2Angle(last.x, last.y, vertex.x, vertex.y, da, &cx, &cy, &r, &sa);
            // if an extent point of circle is on correct side of line between last & vertex,
            // then it should be considered in the arc extents
            BOOL Undefined = fabs(last.x - vertex.x) < SMALLNUMBER;

            if (Undefined)
            {
               if ((last.y > vertex.y) ^ (da < 0)) // if Left of line between last & vertex 
               {
                  // left side of circle
                  extents->left = min(extents->left,cx - r - width);
                  // top and bottom of circle
                  if (cx < vertex.x)
                  {
                     extents->top    = max(extents->top   ,cy + r + width);
                     extents->bottom = min(extents->bottom,cy - r - width);
                  }
               }
               else // if Right of line between last & vertex 
               {
                  // right side of circle
                  extents->right = max(extents->right,cx + r + width);
                  // top and bottom of circle
                  if (cx > vertex.x)
                  {
                     extents->top    = max(extents->top   ,cy + r + width);
                     extents->bottom = min(extents->bottom,cy - r - width);
                  }
               }
            }
            else // defined slope of line between last & vertex
            {
               BOOL Below; // consider points Above or Below line between last & vertex 
               double m, b;
               m = (last.y - vertex.y) / (last.x - vertex.x); // m = dy/dx
               b = vertex.y - m * vertex.x; // y - mx = b

               Below = (last.x < vertex.x) ^ (da < 0);

               // left side of circle
               if ((cy > m * (cx-r) + b) ^ Below)
                  extents->left = min(extents->left, cx-r-width);

               // right side of circle
               if ((cy > m * (cx+r) + b) ^ Below)
                  extents->right = max(extents->right, cx+r+width);

               // top of circle
               if ((cy+r > m * cx + b) ^ Below)
                  extents->top = max(extents->top, cy+r+width);

               // bottom of circle
               if ((cy-r > m * cx + b) ^ Below)
                  extents->bottom = min(extents->bottom, cy-r-width);
            }
         }

         bulge = pnt->bulge;
      }
   }

   //extents->top    = dbunitround(extents->top   );
   //extents->bottom = dbunitround(extents->bottom);
   //extents->left   = dbunitround(extents->left  );
   //extents->right  = dbunitround(extents->right );

   return retval;
}  // END PolyPntExtents

bool PolyPntExtents(CPntList *pntList,CExtent& extent, double scale, int mirror,
      double insert_x, double insert_y, Mat2x2 *m, double width)
{
   CPnt *pnt;
   Point2 lastVertex, vertex;
   double bulge;
   int pointCount = 0;
   double xScale = ((mirror & MIRROR_FLIP) ? -scale : scale);

   for (POSITION pntPos = pntList->GetHeadPosition();pntPos != NULL;pointCount++)
   {
      pnt = pntList->GetNext(pntPos);
      vertex.x = pnt->x * xScale;  
      vertex.y = pnt->y * scale;

      TransPoint2(&vertex, 1, m, insert_x, insert_y);

      extent.update(vertex.x - width,vertex.y - width);
      extent.update(vertex.x + width,vertex.y + width);

      if (pointCount > 0 && fabs(bulge) > SMALLNUMBER) // bulge
      {
         double da, sa, r, cx, cy;
         da = atan(bulge) * 4;

         if (mirror & MIRROR_FLIP) da = -da;

         ArcPoint2Angle(lastVertex.x, lastVertex.y, vertex.x, vertex.y, da, &cx, &cy, &r, &sa);

         double startAngleDegrees,endAngleDegrees;

         if (da >= 0.)
         {
            startAngleDegrees = normalizeDegrees(radiansToDegrees(sa));
            endAngleDegrees   = startAngleDegrees + normalizeDegrees(radiansToDegrees(da));
         }
         else
         {
            startAngleDegrees = normalizeDegrees(radiansToDegrees(sa + da));
            endAngleDegrees   = startAngleDegrees - normalizeDegrees(radiansToDegrees(da));
         }

         double rCos = r + width;
         double rSin = 0.;

         for (int angleDegrees = 0;angleDegrees <= 720;angleDegrees += 90)
         {
            if (angleDegrees >= startAngleDegrees && angleDegrees <= endAngleDegrees)
            {
               extent.update(cx + rCos,cy + rSin);
            }

            double temp = rCos;
            rCos = -rSin;
            rSin = temp;
         }
      }

      lastVertex.x = vertex.x;
      lastVertex.y = vertex.y;
      bulge        = pnt->bulge;
   }

   return (pointCount > 0);
}  // END PolyPntExtents

#endif // #if defined(EnableDcaCamCadDocLegacyCode)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

double Length(double x1, double y1, double x2, double y2)
{
   return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

int CPntInSeg(CPnt *a, CPnt *b, CPnt *pnt, double tolerance) 
{
   return PointInSeg(a->x, a->y, b->x, b->y, pnt->x, pnt->y, tolerance);
}

double MeasurePointToLine(Point2 *point,const CPnt *a, const CPnt *b, double width,Point2 *result)
{
   Point2 A, B;
   A.x = a->x;
   A.y = a->y;

   B.x = b->x;
   B.y = b->y;

   return DistancePointToLine(point, &A, &B, width, result);
}

double MeasurePointToArc(Point2 *point,const CPnt *c, double r, double sa, double da, double aWidth,Point2 *result)
{
   double R, SA, DA;
   Point2 C; 
   C.x = c->x;
   C.y = c->y;
   R = r;
   DA = da;

   SA = sa;

   while (SA < 0)
      SA += 2.0*Pi;

   while (SA > 2.0*Pi)
      SA -= 2.0*Pi;

   return DistancePointToArc(point, &C, R, SA, DA, aWidth, result);
}

double MeasurePointToPntList(Point2 *point,const CPntList *pntList, double width, BOOL Centerline,Point2 *result)
{
   double tempDistance;
   double distance = DBL_MAX;
   Point2 tempResult;

   if (pntList->GetCount() == 1)
   {
      CPnt *pnt = pntList->GetHead();

      result->x = pnt->x;
      result->y = pnt->y;

      double dx = point->x - pnt->x;
      double dy = point->y - pnt->y;

      return sqrt(dx*dx + dy*dy);
   }

   CPnt *a, *b;
   POSITION pos = pntList->GetHeadPosition();
   if (pos)
      b = pntList->GetNext(pos);

   while (pos)
   {
      a = b;
      b = pntList->GetNext(pos);

      if (fabs(a->bulge) < BULGE_THRESHOLD)
         tempDistance = MeasurePointToLine(point, a, b, width, &tempResult);
      else
      {
         double da, sa, r, cx, cy;
         CPnt c;
         da = atan(a->bulge) * 4;
         ArcPoint2Angle(a->x, a->y, b->x, b->y, da, &cx, &cy, &r, &sa);
         c.x = (DbUnit)cx;
         c.y = (DbUnit)cy;
         tempDistance = MeasurePointToArc(point, &c, r, sa, da, width, &tempResult);
      }

      if (tempDistance < distance)
      {
         distance = tempDistance;
         result->x = tempResult.x;
         result->y = tempResult.y;
      }
   }

   return distance;
}

double DistancePointToLine(Point2 *p,const Point2 *la, const Point2 *lb, double lWidth,Point2 *lResult)
{
   double slope,y_int,y_int1,p1p,p2p;//p1p is distance between p1 &p;
   Point2 p1,p2;
   p1.x = la->x; p1.y = la->y;
   p2.x = lb->x; p2.y = lb->y;

   if (fabs(p1.x - p2.x) < SMALLNUMBER)  // undefined slope
   {
   // if p or result on the thin line segment.
      if ((p->y <= max(p1.y,p2.y)) && (p->y >= min(p1.y,p2.y))) 
      {
         lResult->y = p->y;
         lResult->x = p1.x;
         if (fabs(p->x - p1.x) < SMALLNUMBER) // p on the thin line segment 
            return 0.0;
         else  // p not on the thin line but result on the thin line
            return DistancePointToPoint(lResult, lWidth, p, 0);
      }
      else  // p and result not on line
      {
         p1p = sqrt(pow(p->y - p1.y,2) + pow(p->x - p1.x,2));
         p2p = sqrt(pow(p->y - p2.y,2) + pow(p->x - p2.x,2));
         if (p1p < p2p)
         {
            lResult->x = p1.x;
            lResult->y = p1.y;   
         }
         else  //p1p >= p2p
         {
            lResult->x = p2.x;
            lResult->y = p2.y;
         }
      }
      return DistancePointToPoint(lResult, lWidth, p, 0);
   }//end of undefined slope 
   else   //has slope
   {
      slope = (p1.y - p2.y) / (p1.x - p2.x) ; 
      y_int = p1.y - (slope * p1.x) ;
   
      //if p on line segment return 0.0;
      if ((fabs(p->y - slope * p->x - y_int) < SMALLNUMBER) && 
         (p->y >= min(p1.y,p2.y)) && (p->y <= max(p1.y,p2.y)) &&  
         (p->x >= min(p1.x,p2.x)) && (p->x <= max(p1.x,p2.x)))
      {
         lResult->y = p->y;
         lResult->x = p->x;
         return 0.0;
      }
      else // p not on the line segment
      {  
         if (fabs(slope) < SMALLNUMBER)  // no SLOPE
         {
            lResult->y = p1.y;
            lResult->x = p->x;
         }
         else  //slope != 0
         {
            y_int1 = p->y + ((1.0 / slope) * p->x);
            lResult->x = (y_int1 - y_int) *(slope / (pow(slope,2) + 1.0));
            lResult->y = slope * lResult->x + y_int;
         }
         //result point on the line;
         if ((lResult->y <= max(p1.y,p2.y)) && (lResult->y >= min(p1.y,p2.y))
              && (lResult->x <= max(p1.x,p2.x)) && (lResult->x >= min(p1.x,p2.x)))
            return DistancePointToPoint(lResult, lWidth, p, 0);
         else //else result point not on the line;
         {
            p1p = sqrt(pow(p->y - p1.y,2) + pow(p->x - p1.x,2));
            p2p = sqrt(pow(p->y - p2.y,2) + pow(p->x - p2.x,2));
            if (p1p < p2p)
            {
               lResult->x = p1.x;
               lResult->y = p1.y;   
            }
            else //p1p >= p2p
            {
               lResult->x = p2.x;
               lResult->y = p2.y;
            }
            return DistancePointToPoint(lResult, lWidth, p, 0);
         }
      }//end of else 
   }//end of has slope 
} 

double DistancePointToArc(Point2 *point,const Point2 *center, double radius, double sa, double da, double aWidth,Point2 *aResult)
{
   Point2 start, end;
   start.y = radius * sin(sa) + center->y;
   start.x = radius * cos(sa) + center->x;
   end.y = radius * sin(sa + da) + center->y;
   end.x = radius * cos(sa + da) + center->x;   

   double dist_center = sqrt(pow(point->y - center->y,2) + pow(point->x - center->x,2)); // distance from center to point

   // is point on arc's center
   if (dist_center < SMALLNUMBER) 
   {
      aResult->x = start.x;
      aResult->y = start.y;
      return DistancePointToPoint(aResult, aWidth, point, 0);
   }

   // is the cross point of the circle between sa and ea
   double angle = atan2(point->y - center->y, point->x - center->x);
   if (IsAngleOnArc(sa, da, angle))
   {
      aResult->y = (radius * (point->y - center->y) / dist_center) + center->y;
      aResult->x = (radius * (point->x - center->x) / dist_center) + center->x;
      return DistancePointToPoint(aResult, aWidth, point, 0);
   }

   double dist_start = sqrt(pow(point->x - start.x,2) + pow(point->y - start.y,2));
   double dist_end = sqrt(pow(point->x - end.x,2) + pow(point->y - end.y,2));

   if (dist_start < dist_end)
   {
      aResult->x = start.x;
      aResult->y = start.y;
   }
   else
   {
      aResult->x = end.x;
      aResult->y = end.y;
   }

   return DistancePointToPoint(aResult, aWidth, point, 0);
}

/*********************************************************************
*  Calculate arc values from the start,stop points and arc angle
*
*  C-Prototype:
*     void     ArcPoint2Angle( double x1, double y1, double x2,
*                    double y2, double da,
*                    double *cx,double *cy,double *r, double *sa);
*
*  Parameters:
*     x1,y1 = first point of arc
*     x2,y2 = second point of arc (counter clockwise)
*     da    = delta angle for connecting arc
*
*  Returns:
*     *cx,*cy  = arc's center coordinate
*
*********************************************************************/
void ArcPoint2Angle(double x1, double y1, double x2, double y2, double da,double *cx, double *cy, double *r, double *sa)
{
   CenterArc2(x1, y1, x2, y2, da, cx, cy);
   *r = sqrt( (x1 - *cx) * (x1 - *cx) + (y1 - *cy) * (y1 - *cy) );

   /* find start angle and delta angle, ignore orientaion */
   /*    assumes angle between points is <= pi */
   *sa = atan2(y1-*cy, x1-*cx);  /* -pi <= sa <= +pi */

/* PT2   pt0, pt1;
   REAL  bulge = tan(da/4);

   c2v_set( x1, y1, pt0);
   c2v_set( x2, y2, pt1);

   C2_CURVE    curve;

   curve = c2d_arc_pts_bulge ( pt0, pt1, bulge );

   PT2   ctr;
   REAL  rad, sa, sw;
   INT   dir ;

   if (c2c_get_arc_data ( curve, ctr, &rad, &sa, &sw, &dir ))
   {
      *cx = ctr[0];
      *cy = ctr[1];
      *r = rad;
      *ssa = sa;
   }
   else
   {
      *cx = *cy = 0.0;
      *r = 0.0;
      *ssa = 0.0;
   }

   c2d_free_curve( curve);
*/
}

/*****************************************************************************************************
* IsAngleOnArc
*
*  DESCRIPTION:
*     checks whether a ray from the center would be on the arc
*/
BOOL IsAngleOnArc(double sa, double da, double angle)
{
   // normalize sa and angle to be between 0 & 2Pi and da to be positive

   if (da < 0)
   {
      sa = sa + da;
      da = - da;
   }

   while (sa < 0)
      sa += 2.0*Pi;

   while (sa > 2.0*Pi)
      sa -= 2.0*Pi;

   while (angle < 0)
      angle += 2.0*Pi;

   while (angle > 2.0*Pi)
      angle -= 2.0*Pi;

   double ea = sa + da;

   if (sa > angle && ea > 2.0*Pi)
      angle += 2.0*Pi;

   return (angle > sa - SMALLNUMBER && angle < ea + SMALLNUMBER);
}

/******************************************************************************
* PointInSeg()
*
* DESCRIPTION  : Check if a point is part of a line segment
*
* RETURNS      : int = True if point is on segment
*
* PARAMETER    : ax, ay = First point of segment
* PARAMETER    : bx, by = Second point of segment
* PARAMETER    : pntx, pnty = point to check
*/
int PointInSeg(double ax, double ay, double bx, double by, double pntx, double pnty, double tolerance) 
{
   double slope, y_int;
   if (fabs(ax - bx) < tolerance)  // UNDEFINED SLOPE
   {
      // if pntx is same as ax (or bx) AND
      // pnty is between max y and min y
      return (pntx < max(ax, bx) + tolerance &&
            pntx > min(ax, bx) - tolerance &&
            pnty < max(ay, by) + tolerance && 
            pnty > min(ay, by) - tolerance); 
   }
   else // has slope 
   {
      slope = (ay - by) / (ax - bx);
      y_int = ay - slope * ax;

      // if v is not on the equation of the line, return FALSE
      if (fabs(pnty - slope * pntx - y_int) > tolerance)//0.01*ALMOSTZERO)
         return FALSE;

      // check if v is on the segment
      // if pntx is between max x and min x AND
      // pnty is between max y and min y 
      return (pntx < max(ax, bx) + tolerance &&
            pntx > min(ax, bx) - tolerance &&
            pnty < max(ay, by) + tolerance && 
            pnty > min(ay, by) - tolerance); 
   }
}

int Point2InSeg(Point2 *a, Point2 *b, Point2 *pnt, double tolerance) 
{
   return PointInSeg(a->x, a->y, b->x, b->y, pnt->x, pnt->y, tolerance);
}

/******************************************************************************
* cross_product()
*/
double cross_product(double ax,double ay,double bx,double by)
{
   return (ax * by - ay * bx);
}

/*********************************************************************
*  Convert the center and start,stop points to an arc
*
*  C-Prototype:
*     void ArcCenter2( double x1, double y1, double x2,
*                 double y2, double cx, double cy,
*                 double *r, double *sa, double *da, int clockWise );
*
*  Parameters:
*     x1,y1 = first point of arc
*     x2,y2 = second point of arc
*     cx,cy = arc's center coordinate
*     *r    = arc's radius
*     *sa      = start angle for arc
*     *da      = delta angle for arc
*
*  Returns:
*     *r    = circle's radius
*     *sa      = start angle for arc
*     *da      = delta angle for arc (0.0 < *da < PI)
*
*  Comments:
*     ArcCenter2 requires that the delta angle of the arc
*     is less than or equal to PI.  For angles larger than
*     PI, the complement arc will be returned.
*     The direction is determined counter clockwise
*     from the xy1 to xy2.
*
*********************************************************************/
void ArcCenter2(double x1, double y1, double x2, double y2,
           double cx, double cy, double *r, double *sa, double *da, int clockWise)
{
   *r = sqrt((x1-cx)*(x1-cx) + (y1-cy)*(y1-cy));

   // Case dts0100461877
   // SMALLNUMBER (1E-6) is not small enough!
   // Data in poly with arcs has segments that are indeed unique coords but differ
   // by less than SMALLNUMBER (1E-6). Using this value in coord comparison caused
   // tiny (really tiny) arcs to turn into full circles.
   double reallySmallNumber = 1.0E-9;

   // full circle 
   if (fabs(x1-x2) < reallySmallNumber && fabs(y1-y2) < reallySmallNumber)
   {
      *sa = 0.0;
      *da = TwoPi;
      return;
   }

   // find start angle and delta angle
   *sa = atan2(y1-cy, x1-cx);  /* -pi <= sa <= +pi */
   double ea = atan2(y2-cy, x2-cx);   /* -pi <= da <= +pi */

   if (ea < *sa) ea += TwoPi; // normalize for positive delta

   *da = ea - *sa; // calculate positive delta

   // if clockwise instead of counter-clockwise, change delta
   if (clockWise)
      *da -= TwoPi;

/*
   C2_CURVE    arc;
   PT2         *ctr, *pt1, *pt2;
   REAL        rad, st_ang, de_ang;

   // create the points need to create the arc
   ctr = c2d_point( cx, cy );
   pt1 = c2d_point( x1, y1 );
   pt2 = c2d_point( x2, y2 );

   if (clockWise) // in buildingblock -1 is clockwise, +1 is counter clockwise
      clockWise = -1;
   else
      clockWise = +1;

   // create an arc from start point to end point counter-clockwise
   arc = c2d_arc_ctr_2pts( *ctr, *pt1, *pt2, clockWise); // arc can return a NULL on wrong arc !!!
   
   if ( arc && c2c_get_arc_data( arc, NULL, &rad, &st_ang, &de_ang, NULL ) )
   {
      *r = rad;
      *sa = st_ang;
      *da = de_ang;
      if (clockWise == -1) *da = -de_ang;
   }
   else
   {
/*
      *r = 0.0;
      *sa = 0.0;
      *da = 0.0;
/
      // I have an error that somehow does not compute in building block!
      // we need to research this. until then lets try it with the "old" routines.
      // cx 1.135 cy 0.865 x1 1.134999 y1 0.847 x2 1.135 y2 0.883
      *r = sqrt( (x1 - cx)*(x1 - cx) + (y1 - cy)*(y1 - cy) );

      /* full circle *
      if (fabs(x1-x2) < SMALLNUMBER && fabs(y1-y2) < SMALLNUMBER)
      {
         *sa = 0.0;
         *da = PI2;
         return;
      }

      /* find start angle and delta angle *
      /*    assumes angle between points is <= pi *
      *sa = atan2( (y1-cy), (x1-cx) );  /* -pi <= sa <= +pi *
      double ea = atan2( (y2-cy), (x2-cx));   /* -pi <= da <= +pi *

      if (clockWise == -1) // clockwise
         *da = *sa - ea;
      else
         *da = ea - *sa;
   }

   // free the memory created
   c2d_free_point( ctr );
   c2d_free_point( pt1 );
   c2d_free_point( pt2 );
   c2d_free_curve( arc );

   // do not make any radius calc checks here. Because we do not know the units and the
   // accuracy, we can not check here.
*/
}
 
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
int ArcFromStartEndRadius(double x1,double y1,double x2,double y2, double radius,int Clockwise,int Smaller, double *cx,double *cy,double *start,double *delta)
{
   double cx1,cy1,cx2,cy2;
   int n;
   n = ArcFrom2Pts(x1,y1,x2,y2,radius,&cx1,&cy1,&cx2,&cy2);
   if (n == 0)
      return 0;
   if (Clockwise ^ Smaller)
   {
      *cx = cx1;
      *cy = cy1;
   }
   else 
   {
      *cx = cx2;
      *cy = cy2;
   }

   *start = atan2((y1 - *cy),(x1 - *cx));
   *delta = fabs(atan2((y2 - *cy),(x2 - *cx)) - *start);

   if (*delta > Pi)
      *delta = 2*Pi - *delta;

   if (Clockwise && !Smaller)
      *delta = -2*Pi + *delta;
   else if (!Clockwise && !Smaller)
      *delta = 2*Pi - *delta;
   else if (Clockwise && Smaller)
      *delta = - *delta;

  return 1;
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
int ArcFrom2Pts( double x1, double y1, double x2, double y2, double radius,
                  double *cx1, double *cy1, double *cx2, double *cy2 ) 
{

   double dx;                 /* x-distance between start and stop */
   double dy;                 /* y-distance between start and stop */
   double mx,my;              /* midpoint of line connecting start and stop */
   double dl;                 /* distance between start and stop */
   double dlm;                /* slope of line connecting start and stop */
   double ds;                 /* distance between midpoint and centers */
   double dsm;                /* slope of line connecting centers */
   double dsa;                /* interrior angle of line connecting centers */
   double dcx;                /* relative x to circle center from mx,my */
   double dcy;                /* relative y to circle center from mx,my */

   /*----------------------------------------------------------------\
   | calculate deltas in x and y directions.
   \----------------------------------------------------------------*/
   dx = fabs(x2-x1);
   dy = fabs(y2-y1);

   /*----------------------------------------------------------------\
   | calculate length and slope of line connecting start and stop points.
   \----------------------------------------------------------------*/
   dl = sqrt( dx*dx + dy*dy );

   /*----------------------------------------------------------------\
   | when (x1,y1) = (x2,y2), center could lie anywhere on locus of
   |  points forming circle around (x1,y1).  check for unresolvalble
   |  case.
   \----------------------------------------------------------------*/
   if( (dx > SMALLNUMBER) || (dy > SMALLNUMBER) )
   {
      /*----------------------------------------------------------------\
      | check for invalid radius.
      \----------------------------------------------------------------*/
      if(radius > (dl/2 - SMALLNUMBER))
      {
         /*----------------------------------------------------------------\
         | calculate midpoint of line connecting start and stop point.
         \----------------------------------------------------------------*/
         mx = (x2+x1)/2;
         my = (y2+y1)/2;

         /*----------------------------------------------------------------\
         | check for simple case where centers lie on horizontal line
         |  bisecting line connecting start and stop.
         \----------------------------------------------------------------*/
         if( dx < SMALLNUMBER )
         {
            if (y2 > y1)
            {
               double sx = sqrt( fabs(radius*radius - dy*dy/4) );
               *cx1 = mx-sx;
               *cx2 = mx+sx;
            }
            else
            {
               double sx = sqrt( fabs(radius*radius - dy*dy/4) );
               *cx1 = mx+sx;
               *cx2 = mx-sx;
            }
            *cy1 = *cy2 = my;
         }

         /*----------------------------------------------------------------\
         | check for simple case where centers lie on vertical line
         |  bisecting line connecting start and stop.
         \----------------------------------------------------------------*/
         else if( dy < SMALLNUMBER )
         {
            if (x2 > x1)
            {
               *cy1 = my+sqrt( fabs(radius*radius - dx*dx/4) );
               *cy2 = my-sqrt( fabs(radius*radius - dx*dx/4) );
            }
            else
            {
               *cy1 = my-sqrt( fabs(radius*radius - dx*dx/4) );
               *cy2 = my+sqrt( fabs(radius*radius - dx*dx/4) );
            }
            *cx1 = *cx2 = mx;
         }

         /*----------------------------------------------------------------\
         | check general case.
         \----------------------------------------------------------------*/
         else
         {
            /*----------------------------------------------------------------\
            |calculate slope of connecting line.
            \----------------------------------------------------------------*/
            dlm = (y2-y1)/(x2-x1);

            /*----------------------------------------------------------------\
            | calculate length, slope, and angle of perpendicular bisector of
            |  line connecting start and stop point (through midpoint mx,my).
            \----------------------------------------------------------------*/
            ds = sqrt( fabs(radius*radius - dl*dl/4) );
            dsm = -1/dlm;
            dsa = atan(dsm);

            /*----------------------------------------------------------------\
            | calculate relative distances for arc center points.
            \----------------------------------------------------------------*/
            dcx = fabs(ds*cos(dsa));
            dcy = fabs(ds*sin(dsa));

            /*----------------------------------------------------------------\
            | calculate center of both circles.
            \----------------------------------------------------------------*/
            if (y2 > y1 && x2 > x1) 
            {
               *cx1 = mx-dcx;
               *cy1 = my+dcy;
               *cx2 = mx+dcx;
               *cy2 = my-dcy;
            }
            else if (y2 < y1 && x2 > x1)
            {
               *cx1 = mx+dcx;
               *cy1 = my+dcy;
               *cx2 = mx-dcx;
               *cy2 = my-dcy;
            }
            else if (y2 > y1 && x2 < x1)
            {
               *cx1 = mx-dcx;
               *cy1 = my-dcy;
               *cx2 = mx+dcx;
               *cy2 = my+dcy;
            }
            else//(y2 < y1 && x2 < x1)
            {
               *cx1 = mx+dcx;
               *cy1 = my-dcy;
               *cx2 = mx-dcx;
               *cy2 = my+dcy;
            }
         }

         /*----------------------------------------------------------------\
         | check for only one unique circle (distance between start and
         |  stop points equal 2*radius).
         \----------------------------------------------------------------*/
         if( fabs(radius-dl/2) < SMALLNUMBER )
         {
            return 1;
         }
         else
         {
            return 2;
         }
      }
      else
      {
         /*----------------------------------------------------------------\
         | unresolvable case, radius must be >= length (x1,y1)->(x2,y2)/2
         \----------------------------------------------------------------*/
         return 0;
      }
   }
   else
   {
      /*----------------------------------------------------------------\
      | unresolvable case (x1,y1) = (x2,y2), 0=no center x,y found
      \----------------------------------------------------------------*/
      return 0;
   }
   return 0;
}

/************************************************************
* DistancePointToPoint
*  - used to measure the distance between two thick entities
*
*  Algorithm -
*     - finds distance between  result points (closest point on center-line of entity)
*     - compares to width/2 to see if intersection with thick entity
*     - if no intersection, moves result to edge of thick entity and returns new distance
*
*  Paramenter -
*     - width  if width < SMALLNUMBER it's thin entity
*                     if width > 0.0 it's thick entity 
*     - result the point on thin entity which clostest
*     
*  return - 
*     - distance changed to new distance if the entity is thick and two entities
*                     not intersect
*     - result changed to new result if the entity is thick and two entities
*                     not intersect
****************************************************************/
double DistancePointToPoint(Point2 *p1, double width1, Point2 *p2, double width2)
{
   double distance = sqrt(pow(p1->x - p2->x,2) + pow(p1->y - p2->y,2));

   if (width1 < SMALLNUMBER && width2 < SMALLNUMBER)
      return distance;
   else
   {
      if (distance <= (width1/2 + width2/2))
      {
         // good enough to find a point close to the two end points
         p1->x = p2->x = (max(p2->x, p1->x) - min(p2->x, p1->x))/2 + min(p2->x, p1->x);
         p1->y = p2->y = (max(p2->y, p1->y) - min(p2->y, p1->y))/2 + min(p2->y, p1->y);
         return 0.0;
      }
      else
      {
         p1->x = p1->x + (p2->x - p1->x) * (width1/2) / distance;
         p1->y = p1->y + (p2->y - p1->y) * (width1/2) / distance;
         distance = distance - width1/2 - width2/2;
         p2->x = p2->x + (p1->x - p2->x) * (width2/2) / (distance + width2/2);
         p2->y = p2->y + (p1->y - p2->y) * (width2/2) / (distance + width2/2);
         return distance;
      }
   }
} 

/*****************************************************************************************************
* DistanceLineToArc
*
*  DESCRIPTION:
*     Measures shortest distance between arc and line a-b
*
*  RETURNS:
*     distance
*     Point2 *p_arc, *p_line
*/
double DistanceLineToArc(
      Point2 *a, Point2 *b, double width1, 
      Point2 *center,double radius, double sa, double da, double width2, 
      Point2 *p_arc, Point2 *p_line)
{
   // arc and line touch
   if (CrossArcToLine(center, radius, sa, da, a, b, p_arc, p_line))
      return 0.0;
   
   // make the arc counter-clockwise.
   Point2 start, end;
   if (da >= 0)                                           
   {
      start.y = radius * sin(sa) + center->y;
      start.x = radius * cos(sa) + center->x;
      end.y = radius * sin(da + sa) + center->y;
      end.x = radius * cos(da + sa) + center->x;   
   }
   else
   {
      end.y = radius * sin(sa) + center->y;
      end.x = radius * cos(sa) + center->x;
      start.y = radius * sin(da + sa) + center->y;
      start.x = radius * cos(da + sa) + center->x; 
      sa = sa + da;
      da = -da;
   }

   // set inter to intersection of infinite line and perpendicular line through circle center
   Point2 inter;
   BOOL InterOnSegment;
   if (fabs(a->x - b->x) < SMALLNUMBER) // undefined slope
   {
      inter.x = a->x;
      inter.y = center->y;
      InterOnSegment = inter.y > min(a->y, b->y) - SMALLNUMBER && inter.y < max(a->y, b->y) + SMALLNUMBER;
   }
   else if (fabs(a->y - b->y)  < SMALLNUMBER) // no SLOPE
   {
      inter.x = center->x;
      inter.y = a->y;
      InterOnSegment = inter.x > min(a->x, b->x) - SMALLNUMBER && inter.x < max(a->x, b->x) + SMALLNUMBER;
   }
   else // defined slope
   {
      double slope, 
         b_line, // y intercept of line
         b_perp; // y intercept of line perpendicular to line and through circle center

      slope = (a->y - b->y) / (a->x - b->x); 
      b_line = a->y - (slope * a->x);
      b_perp = center->y + ((1.0 / slope) * center->x);

      inter.x = (b_perp - b_line) / (slope + 1.0 / slope);
      inter.y = slope * inter.x + b_line;

      InterOnSegment = inter.x > min(a->x, b->x) - SMALLNUMBER && inter.x < max(a->x, b->x) + SMALLNUMBER;
   }

   // distances to center
   double inter_c = sqrt(pow(inter.x - center->x,2) + pow(inter.y - center->y,2));

   // simple case where circle/line result lands on arc and segment
   if (InterOnSegment && inter_c > radius)
   {
      double angle = atan2(inter.y - center->y, inter.x - center->x);
      if (IsAngleOnArc(sa, da, angle)) // is closest point of circle on the arc
      {
         p_line->x = inter.x;
         p_line->y = inter.y;
         p_arc->x = radius * cos(angle) + center->x;
         p_arc->y = radius * sin(angle) + center->y;
         return DistancePointToPoint(p_line, width1, p_arc, width2); 
      }
   }

   double distance = DBL_MAX, temp;

   // check normal to arc to each endpoint of line
   {
      double distanceToCenter, angle;

      // point a
      angle = atan2(a->y - center->y, a->x - center->x);
      if (IsAngleOnArc(sa, da, angle)) 
      {
         distanceToCenter = sqrt(pow(a->x - center->x, 2) + pow(a->y - center->y, 2));
         temp = fabs(distanceToCenter - radius);
         if (temp < distance)
         {
            distance = temp;
            p_arc->x = (a->x - center->x) * radius / distanceToCenter + center->x;
            p_arc->y = (a->y - center->y) * radius / distanceToCenter + center->y;
            p_line->x = a->x;
            p_line->y = a->y;
         }
      }

      // point b
      angle = atan2(b->y - center->y, b->x - center->x);
      if (IsAngleOnArc(sa, da, angle)) 
      {
         distanceToCenter = sqrt(pow(b->x - center->x, 2) + pow(b->y - center->y, 2));
         temp = fabs(distanceToCenter - radius);
         if (temp < distance)
         {
            distance = temp;
            p_arc->x = (b->x - center->x) * radius / distanceToCenter + center->x;
            p_arc->y = (b->y - center->y) * radius / distanceToCenter + center->y;
            p_line->x = b->x;
            p_line->y = b->y;
         }
      }
   }

   // check normal to line to each endpoint of arc
   {
      Point2 result_line;

      temp = DistancePointToLine(&start, a, b, 0.0, &result_line); 
      if (temp < distance)
      {
         distance = temp;
         p_line->x = result_line.x;
         p_line->y = result_line.y;
         p_arc->x = start.x;
         p_arc->y = start.y;
      }

      temp = DistancePointToLine(&end, a, b, 0.0, &result_line); 
      if (temp < distance)
      {
         distance = temp;
         p_line->x = result_line.x;
         p_line->y = result_line.y;
         p_arc->x = end.x;
         p_arc->y = end.y;
      }
   }

   // check each endpoint of line to each endpoint of arc
   ClosestPoints(distance, a, b, &start, &end, p_line, p_arc);

   return DistancePointToPoint(p_line, width1, p_arc, width2);
}

/******************************************************************************
* DistanceLineToLine
*
*  DESCRIPTION:
*     Measures shortest distance between line a1-a2 and line b1-b2
*
*  RETURNS:
*     distance
*     Point2 *p1, *p2 : p1 on line a1-a2 and p2 on line b1-b2
*/
double DistanceLineToLine(Point2 *a1, Point2* a2, double aWidth, Point2 *b1, Point2 *b2, double bWidth, 
                         Point2 *p1, Point2 *p2)
{
   /*-------------------------------\
   | two line segments intersection |
   \-------------------------------*/
   Point2 pi; //pi is the intersect point of two lines
   if (IntersectSeg2(a1, a2, b1, b2, &pi))
   {
      p1->x = p2->x = pi.x;
      p1->y = p2->y = pi.y;
      return 0.0;
   }


   /*----------------------------------------\
   | else two line segments do not intersect |
   \----------------------------------------*/  

                                                                                    
   /*--------------------------\
   | if either line is a point |
   \--------------------------*/
   if (fabs(a1->x - a2->x) < SMALLNUMBER && fabs(a1->y - a2->y) < SMALLNUMBER)
   {
      double distance = DistancePointToLine(a1, b1, b2, bWidth, p2);

      if (aWidth < SMALLNUMBER)
      {
         p1->x = a1->x;
         p1->y = a1->y;
         return distance;
      }

      double result_x, result_y;
      FindPointOnLine(a1->x, a1->y, p2->x, p2->y, aWidth/2, &result_x, &result_y);
      p1->x = result_x;
      p1->y = result_y;
      return distance - aWidth/2;
   }
   else if (fabs(b1->x - b2->x) < SMALLNUMBER && fabs(b1->y - b2->y) < SMALLNUMBER)
   {
      double distance = DistancePointToLine(b1, a1, a2, aWidth, p1);

      if (bWidth < SMALLNUMBER)
      {
         p2->x = b1->x;
         p2->y = b1->y;
         return distance;
      }

      double result_x, result_y;
      FindPointOnLine(b1->x, b1->y, p1->x, p1->y, bWidth/2, &result_x, &result_y);
      p2->x = result_x;
      p2->y = result_y;
      return distance - bWidth/2;
   }

   
   BOOL undef_a, undef_b;
   undef_a = fabs(a1->x - a2->x) < ALMOSTZERO;
   undef_b = fabs(b1->x - b2->x) < ALMOSTZERO;

   double slope_a, slope_b;
   if (!undef_a)
      slope_a = (a1->y - a2->y) / (a1->x - a2->x);
   if (!undef_b)
      slope_b = (b1->y - b2->y) / (b1->x - b2->x);

   double distance = DBL_MAX;

   /*---------\
   | parallel |
   \---------*/   
   if ( (undef_a && undef_b) || 
         ((!undef_a && !undef_b) && fabs(slope_a - slope_b) < ALMOSTZERO) )
   {
      /*-------------------\
      | undefined parallel |
      \-------------------*/  
      if (undef_a)
      {
         /*-----------------------\
         | on same undefined line |
         \-----------------------*/ 
         if (fabs(a1->x - b1->x) < SMALLNUMBER)
         {
            ClosestPoints(distance, a1, a2, b1, b2, p1, p2);
            return DistancePointToPoint(p1, aWidth, p2, bWidth);    
         }

         else // undefined parallel
         {
            // horizontal distance is shortest
            if (!(min(a1->y, a2->y) > max(b1->y, b2->y) || min(b1->y, b2->y) > max(a1->y, a2->y)))
            {
               p1->x = a1->x;
               p2->x = b1->x;

               p1->y = p2->y = min(max(a1->y, a2->y), max(b1->y, b2->y));
   
               return DistancePointToPoint(p1, aWidth, p2, bWidth);    
            }

            else // endpoint to endpoint is shortest
            {
               ClosestPoints(distance, a1, a2, b1, b2, p1, p2);
               return DistancePointToPoint(p1, aWidth, p2, bWidth);    
            }
         }
      }

      /*-----------------\
      | defined parallel |
      \-----------------*/ 
      else 
      {
         // yint = y - mx
         double yint_a, yint_b;
         yint_a = a1->y - slope_a * a1->x;
         yint_b = b1->y - slope_b * b1->x;

         /*---------------------\
         | on same defined line |
         \---------------------*/   
         if (fabs(yint_a - yint_b) < ALMOSTZERO)
         {
            ClosestPoints(distance, a1, a2, b1, b2, p1, p2);
            return DistancePointToPoint(p1, aWidth, p2, bWidth);    
         }
      }
   }


   /*--------------------------------\
   | Not Parallel & not Intersecting |
   \--------------------------------*/ 
   // check perpendiculars to one line passing through endpoints of other line
   double temp;
   
   // line a
   if (fabs(a1->y - a2->y) < SMALLNUMBER) // undefined perpendicular
   {
      if (b1->x > min(a1->x, a2->x) && b1->x < max(a1->x, a2->x))
      {
         temp = fabs(b1->y - a1->y);
         if (temp < distance)
         {
            distance = temp;
            p1->x = p2->x = b1->x;
            p1->y = a1->y;
            p2->y = b1->y;
         }
      }

      if (b2->x > min(a1->x, a2->x) && b2->x < max(a1->x, a2->x))
      {
         temp = fabs(b1->y - a1->y);
         if (temp < distance)
         {
            distance = temp;
            p1->x = p2->x = b2->x;
            p1->y = a1->y;
            p2->y = b2->y;
         }
      }
   }
   else // b = y - (-1/m)x
   {
      double yint_1, yint_2, yint_p, perp;

      if (undef_a)
         perp = 0.0;
      else
         perp = -1 / slope_a;
      yint_1 = a1->y - perp * a1->x; 
      yint_2 = a2->y - perp * a2->x; 
      yint_p = b1->y - perp * b1->x; 
      if ((yint_p > min(yint_1, yint_2)) && (yint_p < max(yint_1, yint_2)))
      {
         Point2 yint;
         yint.x = 0;
         yint.y = yint_p;
         if (!IntersectInfiniteLine2(a1, a2, b1, &yint, &pi))
         {
            // returned parallel...  this estimate will be better than nothing
            ClosestPoints(distance, a1, a2, b1, b2, p1, p2);
            return DistancePointToPoint(p1, aWidth, p2, bWidth);    
         }

         temp = sqrt(pow(b1->x - pi.x, 2) + pow(b1->y - pi.y, 2));
         if (temp < distance)
         {
            distance = temp;
            p2->x = b1->x;
            p2->y = b1->y;
            p1->x = pi.x;
            p1->y = pi.y;
         }
      }

      yint_p = b2->y - perp * b2->x; 
      if ((yint_p > min(yint_1, yint_2)) && (yint_p < max(yint_1, yint_2)))
      {
         Point2 yint;
         yint.x = 0;
         yint.y = yint_p;
         if (!IntersectInfiniteLine2(a1, a2, b2, &yint, &pi))
         {
            // returned parallel...  this estimate will be better than nothing
            ClosestPoints(distance, a1, a2, b1, b2, p1, p2);
            return DistancePointToPoint(p1, aWidth, p2, bWidth);    
         }

         temp = sqrt(pow(b2->x - pi.x, 2) + pow(b2->y - pi.y, 2));
         if (temp < distance)
         {
            distance = temp;
            p2->x = b2->x;
            p2->y = b2->y;
            p1->x = pi.x;
            p1->y = pi.y;
         }
      }
   }

   // line b
   if (fabs(b1->y - b2->y) < SMALLNUMBER) // undefined perpendicular
   {
      if (a1->x > min(b1->x, b2->x) && a1->x < max(b1->x, b2->x))
      {
         temp = fabs(a1->y - b1->y);
         if (temp < distance)
         {
            distance = temp;
            p1->x = p2->x = a1->x;
            p2->y = b1->y;
            p1->y = a1->y;
         }
      }

      if (a2->x > min(b1->x, b2->x) && a2->x < max(b1->x, b2->x))
      {
         temp = fabs(a1->y - b1->y);
         if (temp < distance)
         {
            distance = temp;
            p1->x = p2->x = a2->x;
            p2->y = b1->y;
            p1->y = a2->y;
         }
      }
   }
   else // b = y - (-1/m)x
   {
      double yint_1, yint_2, yint_p, perp;

      if (undef_b)
         perp = 0.0;
      else
         perp = -1 / slope_b;
      yint_1 = b1->y - perp * b1->x; 
      yint_2 = b2->y - perp * b2->x; 
      yint_p = a1->y - perp * a1->x; 
      if ((yint_p > min(yint_1, yint_2)) && (yint_p < max(yint_1, yint_2)))
      {
         Point2 yint;
         yint.x = 0;
         yint.y = yint_p;
         if (!IntersectInfiniteLine2(b1, b2, a1, &yint, &pi))
         {
            // returned parallel...  this estimate will be better than nothing
            ClosestPoints(distance, a1, a2, b1, b2, p1, p2);
            return DistancePointToPoint(p1, aWidth, p2, bWidth);    
         }

         temp = sqrt(pow(a1->x - pi.x, 2) + pow(a1->y - pi.y, 2));
         if (temp < distance)
         {
            distance = temp;
            p1->x = a1->x;
            p1->y = a1->y;
            p2->x = pi.x;
            p2->y = pi.y;
         }
      }

      yint_p = a2->y - perp * a2->x; 
      if ((yint_p > min(yint_1, yint_2)) && (yint_p < max(yint_1, yint_2)))
      {
         Point2 yint;
         yint.x = 0;
         yint.y = yint_p;
         if (!IntersectInfiniteLine2(b1, b2, a2, &yint, &pi))
         {
            // returned parallel...  this estimate will be better than nothing
            ClosestPoints(distance, a1, a2, b1, b2, p1, p2);
            return DistancePointToPoint(p1, aWidth, p2, bWidth);    
         }

         temp = sqrt(pow(a2->x - pi.x, 2) + pow(a2->y - pi.y, 2));
         if (temp < distance)
         {
            distance = temp;
            p1->x = a2->x;
            p1->y = a2->y;
            p2->x = pi.x;
            p2->y = pi.y;
         }
      }
   }
   
   ClosestPoints(distance, a1, a2, b1, b2, p1, p2);
   return DistancePointToPoint(p1, aWidth, p2, bWidth);    
}

/***************************************************************
* DistanceArcToArc
*
* return 0 if intersection and sets p1 to intersection
* return distance if no intersect and sets p1 and p2 
*
* if (the distance of two centers is <= sum of two radius
* and >= longer radius - shorter radius ) and p1,p2 on the arc
* segment   intersection
* else not intersection 
*/
double DistanceArcToArc(Point2 *center1, double radius1, double sa1, double da1, double Width1, 
              Point2 *center2, double radius2, double sa2, double da2, double Width2, 
              Point2 *p1, Point2 *p2)
{  
   Point2 start1, end1, start2, end2;

   if (da1 > 0)
   {
      start1.y = radius1 * sin(sa1) + center1->y;
      start1.x = radius1 * cos(sa1) + center1->x;
      end1.y = radius1 * sin(da1 + sa1) + center1->y;
      end1.x = radius1 * cos(da1 + sa1) + center1->x; 
   }
   else
   {  
      end1.y = radius1 * sin(sa1) + center1->y;
      end1.x = radius1 * cos(sa1) + center1->x;
      start1.y = radius1 * sin(da1 + sa1) + center1->y;
      start1.x = radius1 * cos(da1 + sa1) + center1->x;  
      sa1 = sa1 + da1;
      da1 = -da1;
   }

   if (da2 > 0)
   {
      start2.y = radius2 * sin(sa2) + center2->y;
      start2.x = radius2 * cos(sa2) + center2->x;
      end2.y = radius2 * sin(da2 + sa2) + center2->y;
      end2.x = radius2 * cos(da2 + sa2) + center2->x; 
   }
   else
   {
      end2.y = radius2 * sin(sa2) + center2->y;
      end2.x = radius2 * cos(sa2) + center2->x;
      start2.y = radius2 * sin(da2 + sa2) + center2->y;
      start2.x = radius2 * cos(da2 + sa2) + center2->x;  
      sa2 = sa2 + da2;
      da2 = -da2;
   }

   if (CrossArcToArc(&start1, &end1, center1, radius1, sa1, da1, &start2, &end2, center2, radius2, sa2, da2, p1, p2))
      return 0.0;

   double distance = DBL_MAX, c1c2;

   // c1c2 is distance between centers
   c1c2 = sqrt(pow((center1->x - center2->x),2) + pow((center1->y - center2->y),2));
   if (c1c2 > SMALLNUMBER)
   {
      p1->y = (radius1 * (center2->y - center1->y) / c1c2) + center1->y;
      p1->x = (radius1 * (center2->x - center1->x) / c1c2) + center1->x;
      p2->y = ((c1c2 - radius2) * (center2->y - center1->y) / c1c2) + center1->y;
      p2->x = ((c1c2 - radius2) * (center2->x - center1->x) / c1c2) + center1->x;
      if (IsPointOnArc(sa1, da1, radius1, center1, p1) &&
         IsPointOnArc(sa2, da2, radius2, center2, p2))
      {
         distance = sqrt(pow(p1->x - p2->x,2) + pow(p1->y - p2->y,2));
         return DistancePointToPoint(p1, Width1, p2, Width2);
      }
   }

   double temp;
   Point2 result;

   DistancePointToArc(&start1, center2, radius2, sa2, da2, 0.0, &result); 
   temp = sqrt(pow(start1.x - result.x, 2) + pow(start1.y - result.y, 2));
   if (temp < distance)
   {
      distance = temp;
      p1->y = start1.y;
      p1->x = start1.x;
      p2->y = result.y;
      p2->x = result.x;
   }
   DistancePointToArc(&end1, center2, radius2, sa2, da2, 0.0, &result); 
   temp = sqrt(pow(end1.x - result.x, 2) + pow(end1.y - result.y, 2));
   if (temp < distance)
   {
      distance = temp;
      p1->y = end1.y;
      p1->x = end1.x;
      p2->y = result.y;
      p2->x = result.x;
   }
   DistancePointToArc(&end2, center1, radius1, sa1, da1, 0.0,  &result); 
   temp = sqrt(pow(end2.x - result.x, 2) + pow(end2.y - result.y, 2));
   if (temp < distance)
   {
      distance = temp;
      p1->x = result.x;
      p1->y = result.y;
      p2->y = end2.y;
      p2->x = end2.x;
   }
   DistancePointToArc(&start2, center1, radius1, sa1, da1, 0.0, &result); 
   temp = sqrt(pow(start2.x - result.x, 2) + pow(start2.y - result.y, 2));
   if (temp < distance)
   {
      distance = temp;
      p1->x = result.x;
      p1->y = result.y;
      p2->y = start2.y;
      p2->x = start2.x;
   }

   temp = sqrt(pow(start2.x - start1.x, 2) + pow(start2.y - start1.y, 2));
   if (temp < distance)
   {
      distance = temp;
      p1->x = start1.x;
      p1->y = start1.y;
      p2->y = start2.y;
      p2->x = start2.x;
   }
   temp = sqrt(pow(start2.x - end1.x, 2) + pow(start2.y - end1.y, 2));
   if (temp < distance)
   {
      distance = temp;
      p1->x = end1.x;
      p1->y = end1.y;
      p2->y = start2.y;
      p2->x = start2.x;
   }
   temp = sqrt(pow(end2.x - start1.x, 2) + pow(end2.y - start1.y, 2));
   if (temp < distance)
   {
      distance = temp;
      p1->x = start1.x;
      p1->y = start1.y;
      p2->y = end2.y;
      p2->x = end2.x;
   }
   temp = sqrt(pow(end2.x - end1.x, 2) + pow(end2.y - end1.y, 2));
   if (temp < distance)
   {
      distance = temp;
      p1->x = end1.x;
      p1->y = end1.y;
      p2->y = end2.y;
      p2->x = end2.x;
   }

   return DistancePointToPoint(p1, Width1, p2, Width2);
}

/****************************************************************
BOOL CrossArcToLine(Point2 *center,double radius, double start, double delta,
               Point2 *a, Point2 *b, Point2 *p1, Point2 *p2)
*****************************************************************/
int CrossArcToLine(Point2 *center,double radius, double start, double delta,
               Point2 *a, Point2 *b,  Point2 *p1, Point2 *p2)
{
   double p2c;
   Point2 p3,p4,inter,
            startp, endp;//startp is the start point of Arc
                           //endp is the end point of Arc
   //figure start point and delta point of the Arc,
   //if delta > 0 ,it is neg clockwise; if delta < 0, it is clockwise;
   if (delta > -SMALLNUMBER)//delta>=0, it is neg clockwise;                                        
   {
      startp.y = radius * sin(start) + center->y;
      startp.x = radius * cos(start) + center->x;
      endp.y = radius * sin(delta + start) + center->y;
      endp.x = radius * cos(delta + start) + center->x;  
   }
   else //it is clockwise;  
   {
      endp.y = radius * sin(start) + center->y;
      endp.x = radius * cos(start) + center->x;
      startp.y = radius * sin(delta + start) + center->y;
      startp.x = radius * cos(delta + start) + center->x;   
      start = start + delta;
      delta = -delta;
   }
   p3.x = a->x; p3.y = a->y;
   p4.x = b->x; p4.y = b->y;
   // set inter to intersection of infinite line 
   // and perpendicular line through arc center
   if (fabs(p3.x - p4.x) < 0.1*ALMOSTZERO) // undefined slope
   {
      inter.x = p3.x;
      inter.y = center->y;
   }
   else // has slope
   {
      if (fabs(p3.y - p4.y)  < 0.1*ALMOSTZERO) // no SLOPE
      {
        inter.x = center->x;
        inter.y = p3.y;
      }
      else
      {
         double slope, y_int, // slope & intercept of line segment
            y_int1; // y intercept of line perpendicular to line and through circle center
         slope = (p3.y - p4.y) / (p3.x - p4.x) ; 
         y_int = p3.y - (slope * p3.x) ;
         y_int1 = center->y + ((1.0 / slope) * center->x);

         inter.x = (y_int1 - y_int) * (slope / (pow(slope,2) + 1));
         inter.y = slope * inter.x + y_int;
      }
   }
   // distance from inter to center
   // inter is currently set to intersection of infinite line 
   // and perpendicular line through arc center
   p2c = sqrt(pow(inter.x - center->x,2) + pow(inter.y - center->y,2));
   /*---------(1)-------------\
   | line tangent to arc      |
   \-------------------------*/
   if (fabs(p2c - radius) < 0.1*ALMOSTZERO && 
       inter.y < max(p3.y,p4.y) + 0.1*ALMOSTZERO &&
       inter.y > min(p3.y,p4.y) - 0.1*ALMOSTZERO &&
       inter.x < max(p3.x,p4.x) + 0.1*ALMOSTZERO && 
       inter.x > min(p3.x,p4.x) - 0.1*ALMOSTZERO)  // if inter is on segment
   {
      if (IsPointOnArc(start, delta, radius, center, &inter))  //if inter on the arc segment
      {  
         p1->x = p2->x = inter.x;
         p1->y = p2->y = inter.y;
         return TRUE;
      }
   }
   /*---------(2)------------------------------------------------------\
   |  line cross the arc (one end of line inside ,other end outside)   |
   |  line cross the arc (two ends all not inside the circle)          |
   \------------------------------------------------------------------*/
   double p3c, p4c, angle1,angle2;
   p3c = sqrt(pow(p3.y - center->y,2) + pow(p3.x - center->x,2));
   p4c = sqrt(pow(p4.y - center->y,2) + pow(p4.x - center->x,2));
   if ((p3c > radius - 0.1*ALMOSTZERO && p4c <= radius + 0.1*ALMOSTZERO) 
      || (p3c <= radius + 0.1*ALMOSTZERO && p4c > radius - 0.1*ALMOSTZERO)
         || ((inter.y < max(p3.y,p4.y) + 0.1*ALMOSTZERO &&
            inter.y > min(p3.y,p4.y) - 0.1*ALMOSTZERO &&
            inter.x < max(p3.x,p4.x) + 0.1*ALMOSTZERO && 
            inter.x > min(p3.x,p4.x) - 0.1*ALMOSTZERO)
         && p2c <= radius + 0.1*ALMOSTZERO && p3c > radius - 0.1*ALMOSTZERO 
         && p4c > radius - 0.1*ALMOSTZERO))
   {  
      if (p2c < 0.1*ALMOSTZERO)// LINE THROUGH ARC CENTER
      {  
         if (p3c > (radius - 0.1*ALMOSTZERO))//p3 is outside or on arc
         {
            p1->y = ((p3.y - center->y) * radius) / p3c + center->y;
            p1->x = ((p3.x - center->x) * radius) / p3c + center->x;
         }
         if (p4c > (radius - 0.1*ALMOSTZERO))//p4 is outside or on arc)
         {
            p2->y = ((p4.y - center->y) * radius) / p4c + center->y;
            p2->x = ((p4.x - center->x) * radius) / p4c + center->x;
         }
         if (p3c > (radius - 0.1*ALMOSTZERO) 
            && IsPointOnArc(start, delta, radius, center, p1) &&     
             (p4c < (radius + 0.1*ALMOSTZERO) || !IsPointOnArc(start,delta,radius, center,p2)))
         {
            p2->x = p1->x;
            p2->y = p1->y;
            return TRUE;
         }
         if (p4c > (radius - 0.1*ALMOSTZERO) 
            && IsPointOnArc(start, delta, radius, center, p2) &&
            (p3c < (radius + 0.1*ALMOSTZERO) || !IsPointOnArc(start, delta, radius, center, p1)))  //if p1 on the arc segment 
         {
            p1->x = p2->x;
            p1->y = p2->y;
            return TRUE;
         }
         if (p3c > (radius - 0.1*ALMOSTZERO) 
             && IsPointOnArc(start, delta, radius, center, p1) && //if p1 on the arc segment 
             (p4c > (radius - 0.1*ALMOSTZERO)) 
             && IsPointOnArc(start,delta,radius, center,p2))
             return TRUE;
      }// end of LINE THROUGH ARC CENTER
      else if (p2c < (radius + 0.1*ALMOSTZERO) ||
            fabs(p2c - radius) < 0.1*ALMOSTZERO)// LINE DO NOT THROUGH ARC CENTER
      {  
         if (fabs(p3.x - p4.x) < 0.1*ALMOSTZERO) // undefined slope
         {
            p1->x = p2->x = p3.x;
            p1->y = center->y + sqrt(pow(radius,2) - pow(p2c,2)) ;
            p2->y = center->y - sqrt(pow(radius,2) - pow(p2c,2)) ;
            if (p1->y < max(p3.y,p4.y) + 0.1*ALMOSTZERO &&
                 p1->y > min(p3.y,p4.y) - 0.1*ALMOSTZERO)
            {
               if ((!IsPointOnArc(start, delta, radius, center, p2) ||
                     p2->y > max(p3.y,p4.y) + 0.1*ALMOSTZERO ||
                     p2->y < min(p3.y,p4.y) - 0.1*ALMOSTZERO)  //p2 not on p3-p4
                 && IsPointOnArc(start, delta, radius, center, p1))  //if p1 on the arc segment 
               {
                  p2->y = p1->y;
                  return TRUE;
               }
             }
             if (p2->y < max(p3.y,p4.y) + 0.1*ALMOSTZERO &&
                  p2->y > min(p3.y,p4.y) - 0.1*ALMOSTZERO)
             {
               if ((!IsPointOnArc(start, delta, radius, center, p1) ||
                     p1->y > max(p3.y,p4.y) + 0.1*ALMOSTZERO ||
                     p1->y < min(p3.y,p4.y) - 0.1*ALMOSTZERO ) //p1 not on p3-p4
                 && IsPointOnArc(start, delta, radius, center, p2))  //if p2 on the arc segment 
               {
                  p1->y = p2->y; 
                  return TRUE;
               }
            }
            if (p1->y < max(p3.y,p4.y) + 0.1*ALMOSTZERO &&
                p1->y > min(p3.y,p4.y) - 0.1*ALMOSTZERO &&
                p2->y < max(p3.y,p4.y) + 0.1*ALMOSTZERO &&
                p2->y > min(p3.y,p4.y) - 0.1*ALMOSTZERO &&
               IsPointOnArc(start, delta, radius, center, p1) &&
               IsPointOnArc(start, delta, radius, center, p2))
               return TRUE;
         }// end of undefined slope
         // has slope
         else if (fabs(p3.y - p4.y)  < 0.1*ALMOSTZERO) // no SLOPE
         {
             p1->x = center->x + sqrt(pow(radius,2) - pow(p2c,2));
             p2->x = center->x - sqrt(pow(radius,2) - pow(p2c,2));
             p1->y = p2->y = p3.y;
             if (p1->x < max(p3.x,p4.x) + 0.1*ALMOSTZERO && 
                p1->x > min(p3.x,p4.x) - 0.1*ALMOSTZERO)
            {
               if ((!IsPointOnArc(start, delta, radius, center, p2) ||
                     p2->x > max(p3.x,p4.x) + 0.1*ALMOSTZERO || 
                     p2->x < min(p3.x,p4.x) - 0.1*ALMOSTZERO)  //p2 not on p3-p4
                 && IsPointOnArc(start, delta, radius, center, p1))  //if p1 on the arc segment 
               {
                  p2->x = p1->x;
                  return TRUE;
               }
             }
             if (p2->x < max(p3.x,p4.x) + 0.1*ALMOSTZERO && 
                  p2->x > min(p3.x,p4.x) - 0.1*ALMOSTZERO)
             {
               if ((!IsPointOnArc(start, delta, radius, center, p1) ||
                     p1->x > max(p3.x,p4.x) + 0.1*ALMOSTZERO || 
                     p1->x < min(p3.x,p4.x) - 0.1*ALMOSTZERO)  //p1 not on p3-p4
                 && IsPointOnArc(start, delta, radius, center, p2))  //if p2 on the arc segment 
               {
                  p1->x = p2->x; 
                  return TRUE;
               }
            }
            if (p1->x < max(p3.x,p4.x) + 0.1*ALMOSTZERO && 
                p1->x > min(p3.x,p4.x) - 0.1*ALMOSTZERO &&
                p2->x < max(p3.x,p4.x) + 0.1*ALMOSTZERO && 
                p2->x > min(p3.x,p4.x) - 0.1*ALMOSTZERO &&
               IsPointOnArc(start, delta, radius, center, p1) &&
               IsPointOnArc(start, delta, radius, center, p2))
               return TRUE;
         }// end of no SLOPE
         else//has slope and slope not equle to 0;
         {
            angle1 = acos(p2c / radius);
            angle2 = acos(fabs(inter.x - center->x) / p2c);
            if (inter.y > center->y)
               p1->y = radius * sin(angle2 - angle1) + center->y;
            if (inter.y < center->y) 
               p1->y = -radius * sin(angle2 - angle1) + center->y;
            if (inter.x > center->x)  
               p1->x = radius * cos(angle2 - angle1) + center->x;
            if (inter.x < center->x)
               p1->x = -radius * cos(angle2 - angle1) + center->x;
            angle2 = acos(fabs(inter.y - center->y) / p2c);
            if (inter.y > center->y) 
               p2->y = radius * cos(angle2 - angle1) + center->y;
            if (inter.y < center->y)
               p2->y = -radius * cos(angle2 - angle1) + center->y;
            if (inter.x > center->x)
               p2->x = radius * sin(angle2 - angle1) + center->x;
            if (inter.x < center->x)
               p2->x = -radius * sin(angle2 - angle1) + center->x;

            if (p1->y < max(p3.y,p4.y) + 0.1*ALMOSTZERO &&
                 p1->y > min(p3.y,p4.y) - 0.1*ALMOSTZERO &&
                p1->x < max(p3.x,p4.x) + 0.1*ALMOSTZERO && 
                p1->x > min(p3.x,p4.x) - 0.1*ALMOSTZERO)
            {
               if ((!IsPointOnArc(start, delta, radius, center, p2) ||
                     p2->y > max(p3.y,p4.y) + 0.1*ALMOSTZERO ||
                     p2->y < min(p3.y,p4.y) - 0.1*ALMOSTZERO ||
                     p2->x > max(p3.x,p4.x) + 0.1*ALMOSTZERO || 
                     p2->x < min(p3.x,p4.x) - 0.1*ALMOSTZERO)  //p2 not on p3-p4
                 && IsPointOnArc(start, delta, radius, center, p1))  //if p1 on the arc segment 
               {
                  p2->x = p1->x;
                  p2->y = p1->y;
                  return TRUE;
               }
             }
             if (p2->y < max(p3.y,p4.y) + 0.1*ALMOSTZERO &&
                  p2->y > min(p3.y,p4.y) - 0.1*ALMOSTZERO &&
                  p2->x < max(p3.x,p4.x) + 0.1*ALMOSTZERO && 
                  p2->x > min(p3.x,p4.x) - 0.1*ALMOSTZERO)
             {
               if ((!IsPointOnArc(start, delta, radius, center, p1) ||
                     p1->y > max(p3.y,p4.y) + 0.1*ALMOSTZERO ||
                     p1->y < min(p3.y,p4.y) - 0.1*ALMOSTZERO ||
                     p1->x > max(p3.x,p4.x) + 0.1*ALMOSTZERO || 
                     p1->x < min(p3.x,p4.x) - 0.1*ALMOSTZERO)  //p1 not on p3-p4
                 && IsPointOnArc(start, delta, radius, center, p2))  //if p2 on the arc segment 
               {
                  p1->x = p2->x; 
                  p1->y = p2->y;
                  return TRUE;
               }
            }
            if (p1->y < max(p3.y,p4.y) + 0.1*ALMOSTZERO &&
                p1->y > min(p3.y,p4.y) - 0.1*ALMOSTZERO &&
                p1->x < max(p3.x,p4.x) + 0.1*ALMOSTZERO && 
                p1->x > min(p3.x,p4.x) - 0.1*ALMOSTZERO &&
                p2->y < max(p3.y,p4.y) + 0.1*ALMOSTZERO &&
                p2->y > min(p3.y,p4.y) - 0.1*ALMOSTZERO &&
                p2->x < max(p3.x,p4.x) + 0.1*ALMOSTZERO && 
                p2->x > min(p3.x,p4.x) - 0.1*ALMOSTZERO &&
               IsPointOnArc(start, delta, radius, center, p1) &&
               IsPointOnArc(start, delta, radius, center, p2))
               return TRUE;
         }//end of has slope and slope not equle to 0;
      }//end of LINE DO NOT THROUGH ARC CENTER
   }
   return FALSE;
} // END CrossArcToLine

/***************************************************************
*int CrossArcToArc(Point2 *startp1,Point2 *endp1, Point2 *center1, double radius1, double start1, double delta1, 
               Point2 *startp2, Point *endp2,Point2 *center2, double radius2, double start2,double delta2, 
               Point2 *p1, Point2 *p2)


****************************************************************/
int CrossArcToArc(Point2 *startp1,Point2 *endp1,Point2 *center1, double radius1, double start1, double delta1, 
               Point2 *startp2,Point2 *endp2,Point2 *center2, double radius2, double start2,double delta2, 
               Point2 *p1, Point2 *p2)
{  
   double c1c2;
   Point2 temp;
   //------------------------------------
   //makd the Arcs be neg clockwise      |
   //------------------------------------
   if (delta1 < -SMALLNUMBER)//it is clockwise
   {  
      temp.y = endp1->y;
      temp.x = endp1->x;
      temp.bulge = endp1->bulge;
      endp1->y = startp1->y;
      endp1->x = startp1->x;
      endp1->bulge = startp1->bulge;
      startp1->y = temp.y;
      startp1->x = temp.x; 
      startp1->bulge = temp.bulge;
      start1 = start1 + delta1;
      delta1 = -delta1;
   }
   if (delta2 < -SMALLNUMBER)//it is clockwise
   {  
      temp.y = endp2->y;
      temp.x = endp2->x;
      temp.bulge = endp2->bulge;
      endp2->y = startp2->y;
      endp2->x = startp2->x;
      endp2->bulge = startp2->bulge;
      startp2->y = temp.y;
      startp2->x = temp.x;
      startp2->bulge = temp.bulge;
      start2 = start2 + delta2;
      delta2 = -delta2;
   }
   //find distance between centers; c1c2 is distance between centers
   c1c2 = sqrt(pow((center1->x - center2->x),2) + pow((center1->y - center2->y),2));   

   if (c1c2 < 0.1*ALMOSTZERO)//same center
   {
      if (fabs(radius1 - radius2) < 0.1*ALMOSTZERO)//on same circle
      {
         if (IsPointOnArc(start1, delta1, radius1, center1, startp2))
         {
            p1->x = startp2->x;
            p1->y = startp2->y;
            if (IsPointOnArc(start1, delta1, radius1, center1, endp2))
            {
               p2->x = endp2->x;
               p2->y = endp2->y;
               return 1;
            }
            else if (IsPointOnArc(start2, delta2, radius2, center2, startp1)
                  && (fabs(startp1->x - p1->x) > 0.1*ALMOSTZERO ||
                     fabs(startp1->y - p1->y) > 0.1*ALMOSTZERO))
            {
               p2->x = startp1->x;
               p2->y = startp1->y;
               return 1;
            }
            else if (IsPointOnArc(start2, delta2, radius2, center2, endp1)
                  && (fabs(endp1->x - p1->x) > 0.1*ALMOSTZERO ||
                     fabs(endp1->y - p1->y) > 0.1*ALMOSTZERO))
            {
               p2->x = endp1->x;
               p2->y = endp1->y;
               return 1;
            }
            p2->x = p1->x;
            p2->y = p1->y;
            return 1;
         }
         else if (IsPointOnArc(start1, delta1, radius1, center1, endp2))
         {
            p1->x = endp2->x;
            p1->y = endp2->y;
            if (IsPointOnArc(start2, delta2, radius2, center2, startp1)
               && (fabs(startp1->x - p1->x) > 0.1*ALMOSTZERO ||
                  fabs(startp1->y - p1->y) > 0.1*ALMOSTZERO))
            {
               p2->x = startp1->x;
               p2->y = startp1->y;
               return 1;
            }
            else if (IsPointOnArc(start2, delta2, radius2, center2, endp1)
                  && (fabs(endp1->x - p1->x) > 0.1*ALMOSTZERO ||
                     fabs(endp1->y - p1->y) > 0.1*ALMOSTZERO))
            {
               p2->x = endp1->x;
               p2->y = endp1->y;
               return 1;
            }
            p2->x = p1->x;
            p2->y = p1->y;
            return 1;
         }
         else if (IsPointOnArc(start2, delta2, radius2, center2, startp1))
         {//if startp1 on Arc2,endp1 should be (have be) on Arc2
            p1->x = startp1->x;
            p1->y = startp1->y;
            p2->x = endp1->x;
            p2->y = endp1->y;
            return 1;
         }
         if (fabs(startp1->x - startp2->x) < 0.1*ALMOSTZERO && fabs(startp1->y - startp2->y) < 0.1*ALMOSTZERO
            || fabs(startp1->x - endp2->x) < 0.1*ALMOSTZERO && fabs(startp1->y - endp2->y) < 0.1*ALMOSTZERO)
         {
            p1->x = p2->x = startp1->x; 
            p1->y = p2->y = startp1->y; 
            p1->bulge = p2->bulge = 0.0;
            return 1;
         }
         else if (fabs(endp1->x - startp2->x) < 0.1*ALMOSTZERO && fabs(endp1->y - startp2->y) < 0.1*ALMOSTZERO
            || fabs(endp1->x - endp2->x) < 0.1*ALMOSTZERO && fabs(endp1->y - endp2->y) < 0.1*ALMOSTZERO) 
         {
            p1->x = p2->x = endp1->x; 
            p1->y = p2->y = endp1->y; 
            p1->bulge = p2->bulge = 0.0;
            return 1;
         }
         else if (IsPointOnArc(start1, delta1,radius1, center1, startp2))
      {
         p1->x = p2->x = startp2->x; 
         p1->y = p2->y = startp2->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      else if (IsPointOnArc(start1, delta1,radius1, center1, endp2))
      {
         p1->x = p2->x = endp2->x; 
         p1->y = p2->y = endp2->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      else if (IsPointOnArc(start2, delta2,radius2, center2, startp1))
      {
         p1->x = p2->x = startp1->x; 
         p1->y = p2->y = startp1->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      else if (IsPointOnArc(start2, delta2,radius2, center2, endp1))
      {
         p1->x = p2->x = endp1->x; 
         p1->y = p2->y = endp1->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
         return 0;
      }
      if (fabs(startp1->x - startp2->x) < 0.1*ALMOSTZERO && fabs(startp1->y - startp2->y) < 0.1*ALMOSTZERO
         || fabs(startp1->x - endp2->x) < 0.1*ALMOSTZERO && fabs(startp1->y - endp2->y) < 0.1*ALMOSTZERO)
      {
         p1->x = p2->x = startp1->x; 
         p1->y = p2->y = startp1->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      else if (fabs(endp1->x - startp2->x) < 0.1*ALMOSTZERO && fabs(endp1->y - startp2->y) < 0.1*ALMOSTZERO
         || fabs(endp1->x - endp2->x) < 0.1*ALMOSTZERO && fabs(endp1->y - endp2->y) < 0.1*ALMOSTZERO)
      {
         p1->x = p2->x = endp1->x; 
         p1->y = p2->y = endp1->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      return 0;
   }//end of c1c2 = 0
   else if (fabs(c1c2 - radius1 - radius2) < 0.1*ALMOSTZERO//c1c2==(r1+r2)
       || fabs(c1c2 - max(radius1,radius2) + min(radius1,radius2)) < 0.1*ALMOSTZERO)
       //circles inside touch or outside touch
   {
      p1->y = p2->y = (radius1 * (center2->y - center1->y) / c1c2) + center1->y ;
      p1->x = p2->x = (radius1 * (center2->x - center1->x) / c1c2) + center1->x ;
      if (IsPointOnArc(start1, delta1,radius1, center1, p1) && 
         IsPointOnArc(start2, delta2, radius2, center2, p1))
         return 1;
      p1->y = p2->y = (radius1 * (center1->y - center2->y) / c1c2) + center1->y ;
      p1->x = p2->x = (radius1 * (center1->x - center2->x) / c1c2) + center1->x ;
      if (IsPointOnArc(start1, delta1,radius1, center1, p1) && 
         IsPointOnArc(start2, delta2, radius2, center2, p1))
         return 1;
      if (fabs(startp1->x - startp2->x) < 0.1*ALMOSTZERO && fabs(startp1->y - startp2->y) < 0.1*ALMOSTZERO
         || fabs(startp1->x - endp2->x) < 0.1*ALMOSTZERO && fabs(startp1->y - endp2->y) < 0.1*ALMOSTZERO)
      {
         p1->x = p2->x = startp1->x; 
         p1->y = p2->y = startp1->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      else if (fabs(endp1->x - startp2->x) < 0.1*ALMOSTZERO && fabs(endp1->y - startp2->y) < 0.1*ALMOSTZERO
         || fabs(endp1->x - endp2->x) < 0.1*ALMOSTZERO && fabs(endp1->y - endp2->y) < 0.1*ALMOSTZERO) 
      {
         p1->x = p2->x = endp1->x; 
         p1->y = p2->y = endp1->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      else if (IsPointOnArc(start1, delta1,radius1, center1, startp2))
      {
         p1->x = p2->x = startp2->x; 
         p1->y = p2->y = startp2->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      else if (IsPointOnArc(start1, delta1,radius1, center1, endp2))
      {
         p1->x = p2->x = endp2->x; 
         p1->y = p2->y = endp2->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      else if (IsPointOnArc(start2, delta2,radius2, center2, startp1))
      {
         p1->x = p2->x = startp1->x; 
         p1->y = p2->y = startp1->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      else if (IsPointOnArc(start2, delta2,radius2, center2, endp1))
      {
         p1->x = p2->x = endp1->x; 
         p1->y = p2->y = endp1->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      return 0;
   }
   else if (c1c2 < (radius1 + radius2 + 0.1*ALMOSTZERO) &&
         c1c2 > (max(radius1,radius2) - min(radius1,radius2) - 0.1*ALMOSTZERO))
   {//one is not completely inside another one or not completely outside
      double angle1, angle2;
      if (center1->x <= center2->x)    
      {
         angle1 = acos(((pow(radius1,2) - pow(radius2,2) + pow(c1c2,2)) 
               / (2.0 * radius1 * c1c2)));    //angle1 is the angle of p1_c1_c2;
         angle2 = atan2((center2->y - center1->y) , (center2->x - center1->x));//angle2 is the angle of c2_c1   ;
         p1->y = radius1 * sin(angle1 + angle2) + center1->y;//angle 1+2 is the angle of p1_c1;
         p1->x = radius1 * cos(angle1 + angle2) + center1->x;
         p2->y = radius1 * sin(angle2 - angle1) + center1->y;//angle 1-2 is the angle of p2_c1;
         p2->x = radius1 * cos(angle2 - angle1) + center1->x;
      }
      else
      {
         angle1 = acos(((pow(radius2,2) - pow(radius1,2) + pow(c1c2,2)) 
               / (2.0 * radius2 * c1c2)));    //angle1 is the angle of p1_c1_c2;
         angle2 = atan2((center1->y - center2->y), (center1->x - center2->x));//angle2 is the angle of c2_c1 ;
         p1->y = radius2 * sin(angle1 + angle2) + center2->y;//angle 1+2 is the angle of p1_c1;
         p1->x = radius2 * cos(angle1 + angle2) + center2->x;
         p2->y = radius2 * sin(angle2 - angle1) + center2->y;//angle 1-2 is the angle of p2_c1;
         p2->x = radius2 * cos(angle2 - angle1) + center2->x;
      }
      if (IsPointOnArc(start1, delta1, radius1, center1, p1)   //if p1 on the arc1 segment   
         && IsPointOnArc(start2, delta2, radius2, center2, p1))   //if p1 on the arc2 segment   
      {
         if (IsPointOnArc(start1, delta1, radius1, center1, p2)   //if p1 on the arc1 segment   
            && IsPointOnArc(start2, delta2, radius2, center2, p2))
            return 1;
         else
         {
            p2->x = p1->x;
            p2->y = p1->y;
            return 1;
         }
      }
      else if (IsPointOnArc(start1, delta1, radius1, center1, p2)    //if p2 on the arc1 segment   
            && IsPointOnArc(start2, delta2, radius2, center2, p2))   //if p2 on the arc2 segment   
      {
         p1->x = p2->x;
         p1->y = p2->y;
         return 1;
      }
      if (fabs(startp1->x - startp2->x) < 0.1*ALMOSTZERO && fabs(startp1->y - startp2->y) < 0.1*ALMOSTZERO
         || fabs(startp1->x - endp2->x) < 0.1*ALMOSTZERO && fabs(startp1->y - endp2->y) < 0.1*ALMOSTZERO)
      {
         p1->x = p2->x = startp1->x; 
         p1->y = p2->y = startp1->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      else if (fabs(endp1->x - startp2->x) < 0.1*ALMOSTZERO && fabs(endp1->y - startp2->y) < 0.1*ALMOSTZERO
         || fabs(endp1->x - endp2->x) < 0.1*ALMOSTZERO && fabs(endp1->y - endp2->y) < 0.1*ALMOSTZERO) 
      {
         p1->x = p2->x = endp1->x; 
         p1->y = p2->y = endp1->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      else if (IsPointOnArc(start1, delta1,radius1, center1, startp2))
      {
         p1->x = p2->x = startp2->x; 
         p1->y = p2->y = startp2->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      else if (IsPointOnArc(start1, delta1,radius1, center1, endp2))
      {
         p1->x = p2->x = endp2->x; 
         p1->y = p2->y = endp2->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      else if (IsPointOnArc(start2, delta2,radius2, center2, startp1))
      {
         p1->x = p2->x = startp1->x; 
         p1->y = p2->y = startp1->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      else if (IsPointOnArc(start2, delta2,radius2, center2, endp1))
      {
         p1->x = p2->x = endp1->x; 
         p1->y = p2->y = endp1->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      return 0;
   }
   if (fabs(startp1->x - startp2->x) < 0.1*ALMOSTZERO && fabs(startp1->y - startp2->y) < 0.1*ALMOSTZERO
      || fabs(startp1->x - endp2->x) < 0.1*ALMOSTZERO && fabs(startp1->y - endp2->y) < 0.1*ALMOSTZERO)
   {
      p1->x = p2->x = startp1->x; 
      p1->y = p2->y = startp1->y; 
      p1->bulge = p2->bulge = 0.0;
      return 1;
   }
   else if (fabs(endp1->x - startp2->x) < 0.1*ALMOSTZERO && fabs(endp1->y - startp2->y) < 0.1*ALMOSTZERO
      || fabs(endp1->x - endp2->x) < 0.1*ALMOSTZERO && fabs(endp1->y - endp2->y) < 0.1*ALMOSTZERO) 
   {
      p1->x = p2->x = endp1->x; 
      p1->y = p2->y = endp1->y; 
      p1->bulge = p2->bulge = 0.0;
      return 1;
   }
   else if (IsPointOnArc(start1, delta1,radius1, center1, startp2))
      {
         p1->x = p2->x = startp2->x; 
         p1->y = p2->y = startp2->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      else if (IsPointOnArc(start1, delta1,radius1, center1, endp2))
      {
         p1->x = p2->x = endp2->x; 
         p1->y = p2->y = endp2->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      else if (IsPointOnArc(start2, delta2,radius2, center2, startp1))
      {
         p1->x = p2->x = startp1->x; 
         p1->y = p2->y = startp1->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
      else if (IsPointOnArc(start2, delta2,radius2, center2, endp1))
      {
         p1->x = p2->x = endp1->x; 
         p1->y = p2->y = endp1->y; 
         p1->bulge = p2->bulge = 0.0;
         return 1;
      }
   return 0;
}// END CrossArcToArc
   
/**************************************************************************************************************************
* ClosestPoints
*
*  DESCRIPTION:
*     - find the shortest distance between 2 sets of points (typically endpoints of 2 lines)
*
*  RETURNS:
*     distance between points (or parameter distance if it was smaller)
*     Point2 *p1, *p2 : points that are closest together
*
*  PARAMETERS:
*     double distance - a minimum distance (if this is smaller than all distances between points, then it is just returned)
*     Point2 *a1, *a2 - endpoints of line1
*     Point2 *b1, *b2 - endpoints of line2
*     Point2 *p1, *p2 - resulting closest points (p1 from line1 and p2 from line2)
*/
double ClosestPoints(double distance, Point2 *a1, Point2 *a2, Point2 *b1,
                     Point2 *b2, Point2 *p1, Point2 *p2)
{
   double temp;//use temp to prepare the distance
   //from point a1 on line1 to b1 on line2
   temp = sqrt(pow(a1->x - b1->x,2) + pow(a1->y - b1->y,2));
   if (temp < distance)
   {
      distance = temp;
      p1->x = a1->x; p1->y = a1->y;
      p2->x = b1->x; p2->y = b1->y;
   }
   //from point a1 on line1 to b2 on line2
   temp = sqrt(pow(a1->x - b2->x,2) + pow(a1->y - b2->y,2));
   if (temp < distance)
   {
      distance = temp;
      p1->x = a1->x; p1->y = a1->y;
      p2->x = b2->x; p2->y = b2->y;
   }
   //from point b1 on line2 to a2 on line1
   temp = sqrt(pow(b1->x - a2->x,2) + pow(b1->y - a2->y,2));
   if (temp < distance)
   {
      distance = temp;
      p1->x = a2->x; p1->y = a2->y;
      p2->x = b1->x; p2->y = b1->y;
   }
   //from point a2 on line1 to b2 on line2
   temp = sqrt(pow(a2->x - b2->x,2) + pow(a2->y - b2->y,2));
   if (temp < distance)
   {
      distance = temp;
      p1->x = a2->x; p1->y = a2->y;
      p2->x = b2->x; p2->y = b2->y;
   }
   return distance;
} // end ClosestPoints
 
/*********************************************************************
*  Find point of intersection between two 2-D line segments.
*
*  C-Prototype:
*     int IntersectSeg2(Point2 *a1, Point2 *a2, Point2 *b1, Point2 *b2, Point2 *pi);
*
*  Parameters:
*     *a1,*a2  = 2-D points defining first line segment
*     *b1,*b2  = 2-D points defining second line segment
*     *pi      = 2-D point of intersection between segments,
*              assigned only if segments intersect
*
*  Returns:
*     1     = if segments intersect
*     0     = if segments do not intersect
*     *pi      = 2-D point of intersection between segments,
*              assigned only if segments intersect
*
*  Comments:
*     -- Point2 is a point structure defined in "geometry.h" with
*     the following elements:
*
*        Point2.x, Point2.y, Point2.f
*
*     -- The routine is optimized by performing a quick box test
*     to exclude obviously non-intersecting segments.
*
*********************************************************************
** PROBLEM:
*    - ambiguous for segments with same slope and intercept (on top of each other)
*    - IntersectInfiniteLine2 can return a point that is not on the segments, 
*     and so the function would return false.
*********************************************************************/
int IntersectSeg2(Point2 *a1, Point2 *a2, Point2 *b1, Point2 *b2,
            Point2 *pi)
{
   if (BoxTestSeg2(a1, a2, b1, b2))
   {
      if (IntersectInfiniteLine2(a1, a2, b1, b2, pi))
      {
         if (Point2InSeg(a1, a2, pi) && Point2InSeg(b1, b2, pi) )
            return 1;
      }
   }

   return 0;
}
 
/**********************************************************************
* FindPointOnLine 
*
*  - Finds a point on the line p1-p2 that is distance away from p1 and on the p2 side of p1
*     - (a negative distance could be used to find a point an the infinite line a distance away from the end of the line segment)
*
*  - returns TRUE if a valid answer
*/
BOOL FindPointOnLine(double p1x, double p1y, double p2x, double p2y, double distance, double *nx, double *ny)
{
   if (fabs(p1x - p2x) < SMALLNUMBER && fabs(p1y - p2y) < SMALLNUMBER)
      return FALSE; // p1 = p2

   // horizontal
   if (fabs(p1y - p2y) < SMALLNUMBER)
   {
      *ny = p1y;
      if (p1x < p2x)
         *nx = p1x + distance;
      else
         *nx = p1x - distance;

      return TRUE;
   }

   // vertical
   if (fabs(p1x - p2x) < SMALLNUMBER)
   {
      *nx = p1x;
      if (p1y < p2y)
         *ny = p1y + distance;
      else
         *ny = p1y - distance;

      return TRUE;
   }

   double angle = atan2(p2y - p1y, p2x - p1x);
   *nx = p1x + cos(angle) * distance;
   *ny = p1y + sin(angle) * distance;

   return TRUE;
}

/*********************************************************************
*  Find the point of intersection between tow infinite geometric
*  lines.
*
*  C-Prototype:
*     int IntersectInfiniteLine2( Point2 *x1, Point2 *x2, Point2 *y1,
*                    Point2 *y2, Point2 *pi );
*
*  Parameters:
*     *x1,*x2  = 2-D points defining first infinite line
*     *y1,*y2  = 2-D points defining second infinite line
*     *pi      = 2-D point of intersection between two lines
*
*  Returns:
*     1     = if lines intersect
*     0     = if lines do not intersect
*     *pi      = 2-D point of intersection between two lines,
*              assigned only if lines intersect
*
*  Comments:
*     Point2 is a point structure defined in "geometry.h" with
*     the following elements:
*
*        Point2.x, Point2.y, Point2.f
*
*     The routine is optimized by isolating vertical and horizonatal
*     lines as special cases.
*********************************************************************/
int IntersectInfiniteLine2( Point2 *x1, Point2 *x2, Point2 *y1, Point2 *y2,
            Point2 *pi )
{
   double mx, bx; /* slope and y-intercept for x-line */
   double my, by; /* slope and y-intercept for y-line */

   /* check Endpoints */
   if (fabs(x1->x - y1->x) < SMALLNUMBER && fabs(x1->y - y1->y) < SMALLNUMBER)
   {
      pi->x = x1->x;
      pi->y = x1->y;
      return 1;
   }

   if (fabs(x2->x - y1->x) < SMALLNUMBER && fabs(x2->y - y1->y) < SMALLNUMBER)
   {
      pi->x = x2->x;
      pi->y = x2->y;
      return 1;
   }

   if (fabs(x1->x - y2->x) < SMALLNUMBER && fabs(x1->y - y2->y) < SMALLNUMBER)
   {
      pi->x = x1->x;
      pi->y = x1->y;
      return 1;
   }

   if (fabs(x2->x - y2->x) < SMALLNUMBER && fabs(x2->y - y2->y) < SMALLNUMBER)
   {
      pi->x = x2->x;
      pi->y = x2->y;
      return 1;
   }

   if( fabs(x1->x - x2->x) > SMALLNUMBER ) // if not vertical (x-line)
   {
      mx = (x2->y - x1->y)/(x2->x - x1->x);
     // bx = x1->y - mx*x1->x;
      bx = x1->y - x1->x * (x2->y - x1->y) / (x2->x - x1->x); 
      if( fabs(y1->x - y2->x) > SMALLNUMBER ) // if not vertical (y-line)
      {
         my = (y2->y - y1->y)/(y2->x - y1->x);
         //by = y1->y - my*y1->x; // fixed 6/3/96 - Devin
         by = y1->y - y1->x * (y2->y - y1->y) / (y2->x - y1->x);   

         if( fabs(mx - my) < SMALLNUMBER ) // if same slope (parallel or same line)
         {
            if (fabs(bx-by)< SMALLNUMBER) // operlapping
            {
               // return a good intersection point even though any point on the line is ok because IntersectSeg uses this intersection point
               pi->x = max(min(x1->x, x2->x), min(y1->x, y2->x));
               pi->y = max(min(x1->y, x2->y), min(y1->y, y2->y));
               return 1;
            }
            return 0; // lines are parallel, never intersect 
         }
         pi->x = (y1->y - y1->x * (y2->y - y1->y) / (y2->x - y1->x)
                  - (x1->y - x1->x * (x2->y - x1->y) / (x2->x - x1->x))) /
            ((x2->y - x1->y)/(x2->x - x1->x) 
                           - (y2->y - y1->y)/(y2->x - y1->x));
      }
      else
      {  // y-line vertical 
         pi->x = y1->x;
      }
      //++++++++++++FawnMay13+++++++++++++
      if (fabs(x1->x - x2->x) > SMALLNUMBER  // if not vertical (x-line)
         && fabs(mx) < SMALLNUMBER)  //for the case of slope=0
         pi->y = x1->y ;//- x1->x * (x2->y - x1->y) / (x2->x - x1->x);//can get exectaly value

      else if (fabs(y1->x - y2->x) > SMALLNUMBER  // if not vertical (y-line)
         && fabs(my) < SMALLNUMBER)  //for the case of slope=0
         pi->y = y1->y ;//- y1->x * (y2->y - y1->y) / (y2->x - y1->x);               //can get exectaly value
      else
      //++++++++++++++++++++++++++++++++++
         pi->y = ((x2->y - x1->y)/(x2->x - x1->x))*pi->x 
               + x1->y - x1->x * (x2->y - x1->y) / (x2->x - x1->x);
   }
   else // x-line vertical 
   {  
      if( fabs(y1->x - y2->x) > SMALLNUMBER )  // if not vertical (y-line)
      {
         my = (y2->y - y1->y)/(y2->x - y1->x);
         by = y1->y - ((y2->y - y1->y)/(y2->x - y1->x)) * y1->x;
         pi->x = x1->x;
         if (fabs(my) < SMALLNUMBER)  //for the case of slope=0
            pi->y = y1->y ;//- y1->x * (y2->y - y1->y) / (y2->x - y1->x);               //can get exectaly value
         else

            pi->y = ((y2->y - y1->y)/(y2->x - y1->x)) * pi->x 
                  + y1->y - ((y2->y - y1->y)/(y2->x - y1->x)) * y1->x;
      } 
      else // y-line also vertical
      {
         if (fabs(x1->x - y1->x) < SMALLNUMBER) // operlapping
         {
            pi->x = x2->x;
            pi->y = max(min(x1->y, x2->y), min(y1->y, y2->y));
            return 1;
         }
         return 0; // lines parallel, never intersect 
      }
   }
   return 1;
}

/**************************************************************
* IsPointOnArc
* 
*  DESCRIPTION -
*     - check if the point p is on the arc 
*     
*  PARAMETERS -
*     - arc defined by sa, da, radius, center
*     - p is the point to check
*/
BOOL IsPointOnArc(double sa, double da, double radius, Point2 *center, Point2 *p)
{
   double dx = p->x - center->x;
   double dy = p->y - center->y;
   double angle = atan2(dy, dx);

   // check if it's on the circle
   double distance = sqrt(pow(p->x - center->x, 2) + pow(p->y - center->y, 2));
   if (fabs(radius - distance) > ALMOSTZERO)
      return FALSE;

   return IsAngleOnArc(sa, da, angle);
}

/*********************************************************************
*  Perform quick area test to see if segments can intersect
*
*  C-Prototype:
*     int BoxTestSeg2(Point2 *a1, Point2 *a2, Point2 *b1, Point2 *b2);
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
BOOL BoxTestSeg2(Point2 *a1, Point2 *a2, Point2 *b1, Point2 *b2)
{
   double aLeft, aRight, aTop, aBottom;
   double bLeft, bRight, bTop, bBottom;

   // find correctly oriented corners for first segment
   aLeft = min(a1->x, a2->x);
   aRight = max(a1->x, a2->x);
   aTop = max(a1->y, a2->y);
   aBottom = min(a1->y, a2->y);

   bLeft = min(b1->x, b2->x);
   bRight = max(b1->x, b2->x);
   bTop = max(b1->y, b2->y);
   bBottom = min(b1->y, b2->y);

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

/******************************************************************************
* Rotate
*  - Rotation on given angle
*  - Input: (x,y) and angle in Degrees
*  - Output: Pointers to new (x,y)
*/
void Rotate(double x,double y,double angleDegrees,double* xrot,double* yrot) 
{
   double sin_rot,tmp;
   double cos_rot;
   int   r = (int)floor(angleDegrees + 0.5);
   int   i;

   r = normalizeDegrees(r);

   /* for accuracy seperate treatment for 90 increments */
   if ((r % 90) == 0)
   {
      for (i=0;i<r / 90;i++)
      {
         tmp = x;
         x = -y;
         y = tmp;
      }  

      *xrot = x;
      *yrot = y;
   }
   else
   {
      sin_rot = sin(degreesToRadians(angleDegrees));
      cos_rot = cos(degreesToRadians(angleDegrees));
      *xrot = x*cos_rot - y*sin_rot;
      *yrot = x*sin_rot + y*cos_rot;
   }
}



