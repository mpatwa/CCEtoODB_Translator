// $Header: /CAMCAD/4.5/Outln_rb.cpp 16    3/03/06 11:47a Rick Faltersack $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.
*/           

/******************************************************************************
* Rubber Band Algorithm - Jarvis' March Algorithm (gift-wrapping) 
*/

#include "stdafx.h"
#include <float.h>
#include "data.h"
#include "geomlib.h"
#include "polylib.h"

static void AddPointToPntList(CPntList* points,DbUnit x, DbUnit y);
static CPntList* ExtractPoints(CPolyList *partList);
static CPnt *FindHighestPoint(CPntList* points);
static CPnt *FindNextPoint(CPntList* points,CPnt *last, double lastAngle, CPnt *start, double accuracy);

#define ARC_VECTORIZATION_DEGREES   10

/******************************************************************************
* CanRubberBand()
*  Simply make sure there's at least 3 distinct points
*/
BOOL CanRubberBand(CPolyList *partList, double accuracy)
{
   CPnt *pnt1 = NULL;
   CPnt *pnt2 = NULL;

   POSITION polyPos = partList->GetHeadPosition();
   while (polyPos != NULL)
   {
      CPoly *poly = partList->GetNext(polyPos);

      POSITION pntPos = poly->getPntList().GetHeadPosition();
      while (pntPos)
      {
         CPnt *pnt = poly->getPntList().GetNext(pntPos);

         if (pnt1 == NULL)
            pnt1 = pnt;
         else if (pnt2 == NULL && (fabs(pnt->x-pnt1->x) > accuracy || fabs(pnt->y-pnt1->y) > accuracy || fabs(pnt->bulge-pnt1->bulge) > accuracy))
            pnt2 = pnt;
         else if ((fabs(pnt->x-pnt1->x) > accuracy || fabs(pnt->y-pnt1->y) > accuracy || fabs(pnt->bulge-pnt1->bulge) > accuracy) && 
               (fabs(pnt->x-pnt2->x) > accuracy || fabs(pnt->y-pnt2->y) > accuracy || fabs(pnt->bulge-pnt2->bulge) > accuracy))
            return TRUE;
      }
   }

   return FALSE;
}

/******************************************************************************
* RubberBand()
*/
CPntList *RubberBand(CPolyList *partList, double accuracy)
{
   // extract points from partlist
   CPntList* points = ExtractPoints(partList);

   // starting point
   CPnt *start = FindHighestPoint(points);

   // result to return
   CPntList *result = new CPntList;

   // add to start (without removing from leftover points
   CPnt *newPnt = new CPnt(*start);
   result->AddTail(newPnt);

   // gift wrap until back to start
   CPnt *pnt = newPnt;
   double angle = 0;
   do 
   {
      CPnt *last = pnt;
      pnt = FindNextPoint(points,last, angle, start, accuracy);
      result->AddTail(pnt); 
      angle = ArcTan2(last->y - pnt->y, last->x - pnt->x);
   }
   while (! ( fabs(pnt->x-start->x) < accuracy && fabs(pnt->y-start->y) < accuracy ) );

   // free leftover points and CPntList*
   FreePntList(points);

   return result;
}

/******************************************************************************
* ExtractPoints
*/
CPntList* ExtractPoints(CPolyList *partList)
{
   // Extract Points from partList
   CPntList* points = new CPntList;

   POSITION polyPos = partList->GetHeadPosition();
   while (polyPos != NULL)
   {
      CPoly *poly = partList->GetNext(polyPos);

      POSITION pntPos = poly->getPntList().GetHeadPosition();
      while (pntPos != NULL)
      {
         CPnt *pnt = poly->getPntList().GetNext(pntPos);

         AddPointToPntList(points,pnt->x, pnt->y);

         // vectorize arcs
         if (fabs(pnt->bulge) > 0.05 && pntPos)
         {
            const double resolution = DegToRad(ARC_VECTORIZATION_DEGREES);

            POSITION tempPos = pntPos;
            CPnt *next = poly->getPntList().GetNext(tempPos);

            double da, sa, r, cx, cy;
            da = atan(pnt->bulge) * 4;
            pnt->bulge = 0;
            ArcPoint2Angle(pnt->x, pnt->y, next->x, next->y, da, &cx, &cy, &r, &sa);

            int segments = (int)ceil(fabs(da) / resolution);
            double degrees = da / segments;

            for (int i=1; i<segments; i++)
            {
               AddPointToPntList(points,(DbUnit)(cx + r * cos(sa + i * degrees)), (DbUnit)(cy + r * sin(sa + i * degrees)));
            }                         
         }
      }
   }

   return points;
}

/******************************************************************************
* AddPointToPntList
*/
void AddPointToPntList(CPntList* points,DbUnit x, DbUnit y)
{
/*
   Too slow for lots of points, and doesn't seem to speed up FindNextPoint() much because of relatively low number of calls.  DD 4/9/01   

  POSITION pos = points->GetHeadPosition();
   while (pos)
   {
      CPnt *pnt = points->GetNext(pos);

      if (fabs(pnt->x-x) < ALMOSTZERO && fabs(pnt->y-y) < ALMOSTZERO)
         return;
   }*/

   CPnt *newPnt = new CPnt;
   newPnt->bulge = 0;
   newPnt->x = x;
   newPnt->y = y;
   points->AddTail(newPnt);
}

/******************************************************************************
* FindHighestPoint
*/
CPnt *FindHighestPoint(CPntList* points)
{
   double maxY = -DBL_MAX;
   double maxX = -DBL_MAX;
   CPnt *highestPoint = NULL;

   POSITION pos = points->GetHeadPosition();
   while (pos != NULL)
   {
      CPnt *pnt = points->GetNext(pos);

      BOOL SameHeightAsHighest = fabs(pnt->y - maxY) < SMALLNUMBER;

      if ( (SameHeightAsHighest && pnt->x > maxX) || (pnt->y > maxY) )
      {
         highestPoint = pnt;
         maxY = pnt->y;
         maxX = pnt->x;
      }
   }

   return highestPoint;
}

/******************************************************************************
* FindNextPoint
*  - find next point and remove it from leftover points
*/
CPnt *FindNextPoint(CPntList* points,CPnt *last, double lastAngle, CPnt *start, double accuracy)
{
   CPnt *next = NULL;
   double smallestAnglularDistance = DBL_MAX;
   double distanceToNextPoint = 0;

   POSITION pos = points->GetHeadPosition();
   while (pos != NULL)
   {                                                    
      CPnt *pnt = points->GetNext(pos);

      if (fabs(pnt->x-last->x) < accuracy && fabs(pnt->y-last->y) < accuracy)
         continue;

      // find outside angle size (obtuse)
      double angle = ArcTan2(pnt->y - last->y, pnt->x - last->x);
      double angularDistance = fabs(lastAngle - angle);
      if (angularDistance < PI)
         angularDistance = 2.0*PI - angularDistance;
      double distance = ManLength(pnt->x, pnt->y, last->x, last->y);

      // take largest obtuse angle and furthest point at that angle
/*    if ( (angularDistance < smallestAnglularDistance - 0.001) ||
            (fabs(angularDistance - smallestAnglularDistance) < 0.001 && (distance > distanceToNextPoint) ) ) */
      if (angularDistance < smallestAnglularDistance)
      {
         smallestAnglularDistance = angularDistance;
         distanceToNextPoint = distance;
         next = pnt;
      }
   }

   // if start point is at about the same angle, finish with start instead
   double startAngle = ArcTan2(start->y - last->y, start->x - last->x);
   double startAngularDistance = fabs(lastAngle - startAngle);
   if (startAngularDistance < PI)
      startAngularDistance = 2.0*PI - startAngularDistance;
   if (fabs(startAngularDistance - smallestAnglularDistance) < ALMOSTZERO)
      next = start;

   points->RemoveAt(points->Find(next));

   return next;
}
