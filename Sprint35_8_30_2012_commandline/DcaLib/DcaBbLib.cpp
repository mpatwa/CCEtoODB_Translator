// $Header: /CAMCAD/DcaLib/DcaBbLib.cpp 3     3/23/07 5:19a Kurt Van Ness $

#include "StdAfx.h"
 
#include <math.h>
#include "DcaGeomlib.h"
#include "DcaBbLib.h"
#include "DcaPolyLib.h"
#include "DcaPnt.h"
#include "DcaPoly.h"
#include "DcaPoint2.h"

// Building Block includes
// needed for BuildingBlock software
extern "C"
{
//#define __WINDOWS__

#include "c2vdefs.h"
#include "c2ddefs.h"
#include "c2cdefs.h"
#include "t2ddefs.h"
#include "t2bdefs.h"
#include "t2cdefs.h"
#include "dmldefs.h"
}

/******************************************************************************
* BB_CreateCurve
*/
C2_CURVE BB_CreateCurve(double x1, double y1, double x2, double y2, double bulge)
{
   C2_CURVE curve;

   if (fabs(bulge) < SMALLNUMBER)
   {
      PT2 pt1, pt2;

      c2v_set(x1, y1, pt1);
      c2v_set(x2, y2, pt2);

      curve = c2d_line(pt1, pt2);
   }
   else
   {
      PT2 pt1, pt2;

      c2v_set(x1, y1, pt1);
      c2v_set(x2, y2, pt2);                                             

      curve = c2d_arc_pts_bulge(pt1, pt2, bulge);
   }

   return curve;
}


/******************************************************************************
* BB_CreatePCurve
*/
C2_CURVE BB_CreatePCurve(CPntList *pntList)
{
   CPnt     *pnt, *prev;
   C2_CURVE pcurve;
   PT2      pt;
   
   POSITION pos = pntList->GetHeadPosition();
   if (pos)
   {
      pnt = pntList->GetNext(pos);

      c2v_set(pnt->x, pnt->y, pt);

      // create a poly curve with the first point
      pcurve = c2d_pcurve_init(pt);
   }

   if (!pcurve)
      return NULL;

   // loop through the poly and put the points in pcurve
   while (pos != NULL)
   {
      prev = pnt;
      pnt = pntList->GetNext(pos);
      
      c2v_set(pnt->x, pnt->y, pt);

      // if this is a arc
      if (fabs(prev->bulge) > SMALLNUMBER)
      {
         double   da = atan(prev->bulge) * 4, sa, r;
         Point2   center;
         PT2      middle;

         ArcPoint2Angle(prev->x, prev->y, pnt->x, pnt->y, da, &center.x, &center.y, &r, &sa );

         double mx, my;
         GetBulgeMidPoint(prev->x, prev->y, pnt->x, pnt->y, prev->bulge, &mx, &my);
         c2v_set(mx, my, middle);

         c2d_pcurve_add_arc_2pts(pcurve, middle, pt);
      }
      else
         // is this is line
         c2d_pcurve_add_line(pcurve, pt);
   }

   return pcurve;
}


/******************************************************************************
* BB_CreateCurveList
*/
DML_LIST BB_CreateCurveList(CPntList *pntList)
{
   DML_LIST curvelist = dml_create_list();

   CPnt *lastPnt;
   POSITION pos = pntList->GetHeadPosition();
   if (pos != NULL)
      lastPnt = pntList->GetNext(pos);
   while (pos != NULL)
   {
      CPnt *pnt = pntList->GetNext(pos);

      C2_CURVE curve = BB_CreateCurve(lastPnt->x, lastPnt->y, pnt->x, pnt->y, lastPnt->bulge);

      dml_append_data(curvelist, curve);

      lastPnt = pnt;
   }

   return curvelist;
}


/******************************************************************************
* BB_purge_small_segments 
*/
static void BB_purge_small_segments(DML_LIST curves, REAL min_length)
{
   DML_ITEM item;
   C2_CURVE curve;
   DML_LIST outlist = dml_create_list();

   DML_WALK_LIST ( curves, item )
   {
      curve = ( C2_CURVE ) dml_record ( item );

      if ( c2c_length ( curve ) > min_length )
      {
         dml_append_data ( outlist, ( ANY ) curve );
      }
      else
      {
         c2d_free_curve ( curve );
      }
   }

   dml_clear_list ( curves );

   dml_append_list ( curves, outlist );

   dml_free_list ( outlist );
}


/******************************************************************************
* BB_CreateRegion
*/
T2_REGION BB_CreateRegion(CPntList *pntList)
{
/* POSITION pos = pntList->GetHeadPosition();
   CPnt *pnt, *prev;
   if (pos)
      pnt = pntList->GetNext(pos);
   while (pos)
   {
      CPnt *prevPrev = prev;
      prev = pnt;
      POSITION tempPos = pos;
      pnt = pntList->GetNext(pos);

      if (pos && sqrt((pnt->x-prev->x)*(pnt->x-prev->x)+(pnt->y-prev->y)*(pnt->y-prev->y)) < 0.001)
      {
         delete prev;
         prev = prevPrev;
         pntList->RemoveAt(tempPos);
      }
   }*/

   DML_LIST curvelist = BB_CreateCurveList(pntList);

// BB_purge_small_segments(curvelist, 0.00001);

   T2_REGION polyline = t2d_build_region(curvelist, NULL, 0.00001);//BBS_TOL);
  
   dml_destroy_list(curvelist, (PF_ACTION)c2d_free_curve); 

   return polyline;
}


/******************************************************************************
* free_region
*/
void BB_free_region(ANY region)  
{  
   t2d_free_region((T2_REGION)region);  
}


/******************************************************************************
* BB_DebugPCurve()
*
* DESCRIPTION  : 
* RETURNS      : CString 
* PARAMETER    : C2_CURVE pcurve
*/
CString BB_DebugPCurve(C2_CURVE pcurve)
{
   double x, y;
   CString buf, string;

   PT2 ept;

   c2c_ept0(pcurve, ept);

   x = PT2_X(ept);
   y = PT2_Y(ept);

   buf.Format("(%.3lf,%.3lf)\n", x, y);
   string += buf;

   int dir;
   BOOLEAN IsArc, more;
   REAL rad, sa, da;
   PT2 ctr;
   int i=0;
   while (more = c2c_get_pcurve_data(pcurve, i, &IsArc, ctr, ept, &rad, &sa, &da, &dir))
   {
      x = PT2_X(ept);
      y = PT2_Y(ept);

      buf.Format("%s(%.3lf,%.3lf)\n", (IsArc?"ARC":"LINE"), x, y);
      string += buf;
      i++;
   }

   return string;
}

/******************************************************************************
* BuildUnion()
*/
CPntList *BuildUnion(CPolyList *partList)
{
   DML_LIST unionRgnList = BB_Union(partList);

   // extract results
   CPntList *result = BB_ExtractResults(unionRgnList);

   // free region lists
   dml_destroy_list(unionRgnList, (PF_ACTION)BB_free_region);

   return result;
}

/******************************************************************************
* BuildUnion()
*/
CPntList *BuildUnion(CPntList *pntList1, CPntList *pntList2)
{
   DML_LIST unionRgnList = BB_Union(pntList1, pntList2);

   // extract results
   CPntList *result = NULL;
   
   result = BB_ExtractResults(unionRgnList);

   // free region lists
   dml_destroy_list(unionRgnList, (PF_ACTION)BB_free_region);

   return result;
}

/******************************************************************************
* BB_Union
*/
DML_LIST BB_Union(CPolyList *polyList)
{
   // build regions
   DML_LIST rgnList = dml_create_list();

   POSITION pos = polyList->GetHeadPosition();
   while (pos)
   {
      CPoly *poly = polyList->GetNext(pos);

      T2_REGION rgn = BB_CreateRegion(&(poly->getPntList()));

      dml_append_data(rgnList, (ANY) rgn);
   }

   // find union
   DML_LIST unionRgnList = t2b_union_l(rgnList, NULL);

   dml_destroy_list(rgnList, (PF_ACTION)BB_free_region);
                      
   return unionRgnList;
}

/******************************************************************************
* BB_Union
*/
DML_LIST BB_Union(CPntList *pntList1, CPntList *pntList2)
{
   T2_REGION region1 = BB_CreateRegion(pntList1);
   T2_REGION region2 = BB_CreateRegion(pntList2);
   
   return t2b_union_rr(region1, region2, NULL);
}

/******************************************************************************
* BB_ExtractResults
*/
CPntList *BB_ExtractResults(DML_LIST rgnList)
{
   if (dml_length(rgnList) != 1)
      return NULL;

   CPntList *result = new CPntList;

   DML_ITEM item;
   item = DML_FIRST(rgnList);
   if (!item) return NULL;
   T2_REGION unionRgn = (T2_REGION) dml_record(item);

   item = DML_FIRST(T2_REGION_LOOP_LIST(unionRgn));
   if (!item) return NULL;
   T2_LOOP loop = (T2_LOOP)DML_RECORD(item);

   T2_EDGE edge;
   item = DML_FIRST(T2_LOOP_EDGE_LIST(loop));
   if (!item) return NULL;
   edge = (T2_EDGE)DML_RECORD(item);

   PT2 pt;
   t2c_ept0(edge, pt);

   CPnt *pnt = new CPnt;
   pnt->x = (DbUnit)(PT2_X(pt));
   pnt->y = (DbUnit)(PT2_Y(pt));
   pnt->bulge = 0;
   result->AddTail(pnt);

   CPnt *prevPnt = pnt;
   DML_WALK_LIST(T2_LOOP_EDGE_LIST(loop), item)
   {
      edge = (T2_EDGE)DML_RECORD(item);

      t2c_ept1(edge, pt);

      CPnt *pnt = new CPnt;
      pnt->x = (DbUnit)(PT2_X(pt));
      pnt->y = (DbUnit)(PT2_Y(pt));
      pnt->bulge = 0;
      result->AddTail(pnt);

      C2_CURVE curve = T2_EDGE_CURVE(edge);
      if (C2_CURVE_IS_ARC(curve))
      {
         REAL sweep;
         c2c_get_arc_sweep(curve, &sweep);
         int dir;
         c2c_get_arc_direction(curve, &dir);
         prevPnt->bulge = (DbUnit)(tan(sweep/4) * dir);
         if (T2_EDGE_REVERSED(edge))
            prevPnt->bulge *= -1;
      }
      
      prevPnt = pnt;
   }

   return result;
}
