// $Header: /CAMCAD/DcaLib/DcaPolyLib.cpp 3     3/09/07 5:15p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaPolyLib.h"
#include "DcaPnt.h"
#include "DcaGeomLib.h"
#include <Math.h>

//_____________________________________________________________________________
bool CleanPntList(CPntList *pntList, double dist)
{
   if (pntList->GetCount() < 3)
      return false;

   // check for very small polys
   {
      double xmin, xmax, ymin, ymax;
      xmin = ymin = DBL_MAX;
      xmax = ymax = -DBL_MAX;
      POSITION pos = pntList->GetHeadPosition();
      while (pos)
      {
         CPnt *pnt = pntList->GetNext(pos);

         if (pnt->x < xmin)   xmin = pnt->x;
         if (pnt->x > xmax)   xmax = pnt->x;
         if (pnt->y < ymin)   ymin = pnt->y;
         if (pnt->y > ymax)   ymax = pnt->y;
      }

      if (xmax - xmin < dist*2 && ymax - ymin < dist*2)
         return false;
   }
   
   CPnt *pnt, *prev, *next;
   POSITION pos = pntList->GetHeadPosition();
   POSITION pntPos = pos;
   pnt = pntList->GetNext(pos);     
   POSITION nextPos = pos;
   next = pntList->GetNext(pos);

   while (pos)
   {
      prev = pnt;
      pnt = next;
      pntPos = nextPos;
      nextPos = pos;
      next = pntList->GetNext(pos);

      // Pnts with same x,y
      if (fabs(pnt->x-prev->x) < dist && fabs(pnt->y-prev->y) < dist)
      {
         delete ((CPnt*)pntList->GetAt(pntPos));
         pntList->RemoveAt(pntPos);
         pnt = prev;
         continue;
      }

      if (prev->bulge > BULGE_THRESHOLD || pnt->bulge > BULGE_THRESHOLD)
         continue;

      // Pnts on same line
      if (CPntInSeg(prev, next, pnt))
      {
         delete ((CPnt*)pntList->GetAt(pntPos));
         pntList->RemoveAt(pntPos);
         pnt = prev;
         continue;
      }
   }

   // last Pnt with same x,y
   if (fabs(pnt->x-next->x) < dist && fabs(pnt->y-next->y) < dist)
   {
      POSITION tempPos = pntList->Find(pnt);
      delete ((CPnt*)pntList->GetAt(tempPos));
      pntList->RemoveAt(tempPos);
   }

   return true;
}

/******************************************************************************
* IsClockwise()
*
*  - Assumes poly does not self-intersect
*/
BOOL IsClockwise(const CPntList *pntList)
{
   CPnt *xmin = NULL, *xmax = NULL, *ymin = NULL, *ymax = NULL;

   // find extremum rectangle
   POSITION pos = pntList->GetHeadPosition();
   while (pos)
   {
      CPnt *pnt = pntList->GetNext(pos);

      if (!xmin || xmin->x - SMALLNUMBER > pnt->x)
         xmin = pnt;

      if (!xmax || xmax->x + SMALLNUMBER < pnt->x)
         xmax = pnt;

      if (!ymin || ymin->y - SMALLNUMBER > pnt->y)
         ymin = pnt;

      if (!ymax || ymax->y + SMALLNUMBER < pnt->y)
         ymax = pnt;
   }

   // center of extremum rectangle
   CPnt center;
   if (xmin != NULL && xmax != NULL)
      center.x = (xmin->x + xmax->x) / 2;
   if (ymin != NULL && ymax != NULL)
      center.y = (ymin->y + ymax->y) / 2;

   // first and second extremum points as you travel around poly from head to tail
   CPnt *first = NULL, *second = NULL;

   pos = pntList->GetHeadPosition();
   while (pos && !first)
   {
      CPnt *pnt = pntList->GetNext(pos);

      if (pnt == xmin)
         first = xmin;
      else if (pnt == xmax)
         first = xmax;
      else if (pnt == ymin)
         first = ymin;
      else if (pnt == ymax)
         first = ymax;
   }

   while (pos && !second)
   {
      CPnt *pnt = pntList->GetNext(pos);

      if (pnt == xmin)
         second = xmin;
      else if (pnt == xmax)
         second = xmax;
      else if (pnt == ymin)
         second = ymin;
      else if (pnt == ymax)
         second = ymax;
   }

   if (!first || !second)
      return -1;

	double result = cross_product(first->x - center.x, first->y - center.y, second->x - center.x, second->y - center.y);
   return (result < 0);
}

/******************************************************************************
* GetBulgeMidPoint()
*
* DESCRIPTION  : Finds mid point of arc (point on arc half way between start and end) 
*
* PARAMETER    : double ax, ay = start point
* PARAMETER    : double bx, by = end point
* PARAMETER    : double bulge
*
* RETURNS      : double *mx, *my = midpoint
*/
void GetBulgeMidPoint(double ax, double ay, double bx, double by, double bulge, double *mx, double *my)
{
   double da = atan(bulge) * 4;
   double cx, cy, r, sa;                                                   
   ArcPoint2Angle(ax, ay, bx, by, da, &cx, &cy, &r, &sa);

   *mx = cx + r * cos(sa + da/2);
   *my = cy + r * sin(sa + da/2);
}
