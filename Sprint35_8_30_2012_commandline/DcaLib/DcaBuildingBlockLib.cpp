// $Header: /CAMCAD/DcaLib/DcaBuildingBlockLib.cpp 6     6/21/07 8:29p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaBuildingBlockLib.h"
#include "DcaPnt.h"
#include "DcaPolyLib.h"
#include "Dca.h"
#include "DcaBbLib.h"

// Building Block includes
// needed for BuildingBlock software
extern "C"
{
//#define __WINDOWS__

#include <grdefs.h>
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
#include "c2ddefs.h"
#include "c2cdefs.h"
#include "t2ddefs.h"
#include "t2bdefs.h"
#include "t2cdefs.h"
#include "t2idefs.h"
}

/*********************************************************************
*  Calculate arc center point from the start & stop points and arc angle
*
*  Prototype:
*     void CenterArc2( double x1, double y1, double x2, double y2,
*                 double da, double *cx, double *cy );
*
*  Parameters:
*     x1,y1 = first point of arc
*     x2,y2 = second point of arc (clockwise)
*     da    = delta angle for connecting arc
*     *cx,*cy  = arc's center coordinate
*
*  Returns:
*     *cx,*cy  = arc's center coordinate
*
*  Comments:
*     The center point is calculated by calculating the slopes
*  of the lines connecting (x1,y1) and (x2,y2) with the arc's center.
*
*     y1c = m1c*x1c + c1c: line connecting (x1,y1) with center
*     y2c = m2c*x2c + c2c: line connecting (x2,y2) with center
*
*  At the arc's center, these two lines intersect, and the center
*  point can be found as follows:
*
*        *cx = (c2c-c1c)/(m1c-m2c);
*        *cy = m1c*x1c + c1c;
*
*  The slopes m1c and m2c can be found from the perpendicular bisector
*  of the line connecting (x1,y1) and (x2,y2).  The slope of the
*  perpendicular bisector can be calculated from the slope of the line
*  connecting (x1,y1) and (x2,y2).
*
*        mbis = -1*(x2-x1)/(y2-y1);
*
*  The angle of the perpendicular bisector is the ArcTangent of its slope.
*
*        abis = atan( mbis );
*
*  The center line slopes are then given by:
*
*        m1c = tan( abis+da/2 );
*        m2c = tan( abis-da/2 );
*
*********************************************************************/
void CenterArc2( double x1, double y1, double x2, double y2,double da, double *cx, double *cy )
{
   PT2   pt0, pt1;
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
   }
   else
   {
      *cx = *cy = 0.0;
   }

   c2d_free_curve( curve);

   return; 

#if OLD_DEF
   double ax1, ay1;  /* x1,y1 with closest angle to 0 */
   double ax2, ay2;  /* x2,y2 with furthest angle from 0 */

   /* orient x1,y1 and x2,y2 so that x1,y1 is first point in arc when */
   /*    traced in counter clockwise direction from angle=0 */
   if( da >= 0 ) 
   {
      ax1 = x1;
      ay1 = y1;
      ax2 = x2;
      ay2 = y2;
   }
   else 
   {
      ax1 = x2;
      ay1 = y2;
      ax2 = x1;
      ay2 = y1;
   }

   // if p1 and p2 are same
   if (fabs(ax2-ax1) < 0.1*ALMOSTZERO && fabs(ay2-ay1) < 0.1*ALMOSTZERO)
   {
      *cx = x1;
      *cy = y1;
      return;
   }

   /* check for special case where perpendicular bisector of line 1->2 */
   /* is vertical */
   if (fabs(ay2-ay1) < 0.1*ALMOSTZERO)
   {
      *cx = (ax1 + ax2) / 2; // cx is half way between ax1 and ax2

      // if half circle
      if ((fabs(fabs(da) - PI)) < ALMOSTZERO) 
      {
         *cy = ay1;
         return;
      }

      double dx, dy, angle;

      // use smaller angle to calculate cy
      // ex. 90° and 270° have same dy
      if (fabs(da) < PI)
         angle = fabs(da/2);
      else
         angle = fabs(PI - fabs(da/2)); // (2PI - da) / 2

      dx = fabs((ax1 - ax2) /2);
      dy = dx / tan(angle);

      // above or below chord
      if ( (ax1 < ax2) ^ (fabs(da) > PI) )
         *cy = ay1 + dy;
      else
         *cy = ay1 - dy;

      return;
   }

   /* check for special case where perpendicular bisector of line 1->2 */
   /* is horizontal */
   if (fabs(ax2-ax1) < 0.1*ALMOSTZERO)
   {
      *cy = (ay1 + ay2) / 2; // cy is half way between ay1 and ay2

      // if half circle
      if ((fabs(fabs(da) - PI)) < ALMOSTZERO) 
      {
         *cx = ax1;
         return;
      }

      double dx, dy, angle;

      // use smaller angle to calculate cx
      // ex. 90° and 270° have same dx
      if (fabs(da) < PI)
         angle = fabs(da/2);
      else
         angle = fabs(PI - fabs(da/2)); // (2PI - da) / 2

      dy = fabs((ay1 - ay2) /2);
      dx = dy / tan(angle);

      // left or right chord
      if ( (ay1 < ay2) ^ (fabs(da) > PI) )
         *cx = ax1 - dx;
      else
         *cx = ax1 + dx;

      return;
   }

   /* general case */
   double mbis;   // slope of perpendicular bisector of line (x1,y1)->(x2,y2) 
   double abis;   // angle of perp. bisector 
   double m1c, m2c; // slopes for lines 1->c and 2->c
   double c1c, c2c;  // constants for lines connecting points to center 
     
   // calculate slope of perp. bisector of line 1->2 
   mbis = -(ax2-ax1)/(ay2-ay1);

   // calculate angle of perp. bisector relative to origin 
   abis = atan( mbis );

   // calculate slopes for lines 1->c and 2->c
   m1c = tan( abis-fabs(da)/2 );
   m2c = tan( abis+fabs(da)/2 );
      
   // half circle
   if ((fabs(m1c - m2c) < ALMOSTZERO) || fabs(fabs(da)-PI) < 0.05)   // 3 degrees
   {
      // center is midpoint of chord
      *cx = (ax1 + ax2) / 2;
      *cy = (ay1 + ay2) / 2;
      return;
   }

   // 1->c is vertical, but 2->c is not
   else if (fabs(fabs(abis-fabs(da)/2) - PI/2) < ALMOSTZERO)
   {
      *cx = ax1;
      if (fabs(m2c) < SMALLNUMBER)
         m2c = 0.0;
      c2c = ay2-ax2*m2c;
      *cy = (*cx)*m2c+c2c;

      return;
   }

   // 2->c is vertical, but 1->c is not 
   else if (fabs(fabs(abis+fabs(da)/2) - PI/2) < ALMOSTZERO) 
   {
      *cx = ax2;
      if (fabs(m1c) < SMALLNUMBER)
         m1c = 0.0;
      c1c = ay1-ax1*m1c;
      *cy = (*cx)*m1c+c1c;

      return;
   }
     
   // normal case (calculate center from intersection of lines 1->c and 2->c)
   else
   {
      /* calculate constants for lines 1->c and 2->c */
      if (fabs(m1c) < SMALLNUMBER)
         m1c = 0.0;
      if (fabs(m2c) < SMALLNUMBER)
         m2c = 0.0;
      c1c = ay1-ax1*m1c;
      c2c = ay2-ax2*m2c;

      /* calculate center coordinates */
      *cx = (c2c-c1c)/(m1c-m2c);
      *cy = (*cx)*m1c+c1c;

      return;
   }
#endif
}

/******************************************************************************
* ShrinkPoly()
*  - caller must call FreePntList() 
*/
CPntList *ShrinkPoly(CPntList *pntList,PageUnitsTag pageUnits, double dist,bool suppressErrorMessages)
{
	if (pntList == NULL)
		return NULL;

	CPntList* pntListToShrink = pntList;

	if (pntList->hasBulge())
	{
		pntListToShrink = new CPntList(*pntList);
		pntListToShrink->vectorize(pageUnits);
	}

   if (!CleanPntList(pntListToShrink, 0.00001))
   {
      return NULL;
   }

   // if CleanPntList reduces a pntlist to less elements.
   if (pntListToShrink->GetCount() < 2)
      return NULL;

   if (!IsClockwise(pntListToShrink))
      dist = -dist;

   DML_LIST list = BB_CreateCurveList(pntListToShrink);

   REAL tol = 0.00001;
   T2_LOOP loop = t2d_build_loop(NULL, list, NULL, tol);

   t2b_offset_loop(loop, dist, NULL, list);

   t2d_free_loop(loop);
   loop = NULL;

   if (DML_LENGTH(list) != 1)
   {
      if (!suppressErrorMessages)
      {
         if (DML_LENGTH(list) >= 1)
            ErrorMessage("More than one loop");
         else if (DML_LENGTH(list) == 0)
            ErrorMessage("Can not shrink because the size is too small.");
         else
            ErrorMessage("Shrink failed");
      }

      dml_destroy_list(list, (PF_ACTION)t2d_free_loop); 

      return NULL;
   }

   CPntList *newPntList = new CPntList;
   CPnt *lastPnt = new CPnt;

   loop = (T2_LOOP)dml_record(DML_FIRST(list));

   DML_ITEM item;
   DML_WALK_LIST(T2_LOOP_EDGE_LIST(loop), item)
   {
      T2_EDGE edge = (T2_EDGE)dml_record(item);
      C2_CURVE curve = T2_EDGE_CURVE(edge);

      PT2 pt;
      t2c_ept0(edge, pt);

      CPnt *pnt = new CPnt;
      pnt->x = (DbUnit)PT2_X(pt);
      pnt->y = (DbUnit)PT2_Y(pt);
      pnt->bulge = 0;
      newPntList->AddTail(pnt);

      if (C2_CURVE_IS_ARC(curve))
      {
         REAL sweep;
         c2c_get_arc_sweep(curve, &sweep);
         int dir;
         c2c_get_arc_direction(curve, &dir);
         pnt->bulge = (DbUnit)(tan(sweep/4) * dir);
         if (T2_EDGE_REVERSED(edge))
            pnt->bulge *= -1;
      }

      t2c_ept1(edge, pt);
      lastPnt->x = (DbUnit)PT2_X(pt);
      lastPnt->y = (DbUnit)PT2_Y(pt);
      lastPnt->bulge = 0;
   }

   newPntList->AddTail(lastPnt);

   dml_destroy_list(list, (PF_ACTION)t2d_free_loop); 

	if (pntListToShrink != pntList)
	{
		delete pntListToShrink;

		pntListToShrink = NULL;
	}

   return newPntList; 
}
