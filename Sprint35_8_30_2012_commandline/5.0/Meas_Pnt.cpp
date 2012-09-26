// $Header: /CAMCAD/4.5/Meas_Pnt.cpp 17    4/28/06 6:48p Kurt Van Ness $

/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/

#include "stdafx.h"
#include <math.h>
#include <float.h>
#include "ccdoc.h"
#include "data.h"
#include "apertur2.h"
#include "polylib.h"
#include "measure.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/******************************************************************************
* FindClosestPoint 
*
* DESCRIPTION:
*  - finds point on data closest to point p
*  - if point p is on data, result is same as p
*
* RETURNS:
*  - distance between result and p
*  
* PARAMETERS:
*  - data
*  - Point2 *p = point to check against data (is not Tranformed because not in a file)
*  - Point2 *result = used to return Closest point on data
*/
double FindClosestPoint(CCEtoODBDoc *doc, Point2 *Point, DTransform *pointXform,
      DataStruct *data, DTransform *dataXform, BOOL Centerline,
      Point2 *result)
{
#ifdef SHAREWARE
   return 0.0;
#endif

   Point2 point;
   point.x = Point->x;
   point.y = Point->y;
   pointXform->TransformPoint(&point);

   CPolyList *polylist;
   switch (data->getDataType())
   {
      case T_POLY:
         polylist = data->getPolyList();
         break;

      case T_INSERT:
      {
         BlockStruct *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
         polylist = ApertureToPoly(subblock, data->getInsert(), dataXform->mirror);
      }
      break;
   } 


   double distance;
   if (polylist)
   {
      distance = MeasurePointToPolylist(doc, &point, 
            polylist, dataXform, Centerline, 
            result);
   }
   else
   {
      Point2 insertPoint;
      insertPoint.x = data->getInsert()->getOriginX();
      insertPoint.y = data->getInsert()->getOriginY();
      dataXform->TransformPoint(&insertPoint);
      result->x = insertPoint.x;
      result->y = insertPoint.y;

      distance = sqrt(pow(result->x - point.x,2) + pow(result->y - point.y,2));
   }

   if (polylist && data->getDataType() == T_INSERT)
      FreePolyList(polylist);

   return distance;
}

/******************************************************************************
* MeasurePointToPolyList
*/
double MeasurePointToPolylist(CCEtoODBDoc *doc, Point2 *point,
      CPolyList *polylist, DTransform *polylistXform, BOOL Centerline,
      Point2 *result)
{
   double tempDistance;
   double distance = DBL_MAX;
   Point2 tempResult;

   POSITION pos = polylist->GetHeadPosition();
   while (pos != NULL)
   {
      CPoly *poly = polylist->GetNext(pos);
      tempDistance = MeasurePointToPoly(doc, point,
            poly, polylistXform, Centerline,
            &tempResult);

      if (tempDistance < distance)
      {
         distance = tempDistance;
         result->x = tempResult.x;
         result->y = tempResult.y;
      }
   }

   return distance;
}

/******************************************************************************
* MeasurePointToPoly
*/
double MeasurePointToPoly(CCEtoODBDoc *doc, Point2 *point,
      CPoly *poly, DTransform *polyXform, BOOL Centerline, 
      Point2 *result)
{
   double width;
   if (Centerline || poly->getWidthIndex() < 0)
      width = 0.0;
   else
      width = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * polyXform->scale;

   return MeasurePointToPntList(point, &poly->getPntList(), width, polyXform, Centerline, result);
}

double MeasurePointToPoly(Point2 *point,
      const CPoly *poly, const DTransform *polyXform, BOOL Centerline, double width,
      Point2 *result)
{
   if (Centerline)
   {
      width = 0.0;
   }

   return MeasurePointToPntList(point, &poly->getPntList(), width, polyXform, Centerline, result);
}

/******************************************************************************
* MeasurePointToPntList
*/
double MeasurePointToPntList(Point2 *point,
      const CPntList *pntList, double width, const DTransform *polyXform, BOOL Centerline, 
      Point2 *result)
{
   double tempDistance;
   double distance = DBL_MAX;
   Point2 tempResult;

/* // optimization for circle
   double cx, cy, r;
   if (PolyIsCircle(poly, &cx, &cy, &r))
   {
      double tempDistance = sqrt( (cx-x)*(cx-x) + (cy-y)*(cy-y) ) - r;

      if (tempDistance < distance)
      {
         distance = tempDistance;

         double tx, ty;
         FindPointOnLine(cx, cy, x, y, r, &tx, &ty);

         result->x = tx;
         result->y = ty;
      }
      return;
   }
*/

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
         tempDistance = MeasurePointToLine(point, a, b, width, polyXform, &tempResult);
      else
      {
         double da, sa, r, cx, cy;
         CPnt c;
         da = atan(a->bulge) * 4;
         ArcPoint2Angle(a->x, a->y, b->x, b->y, da, &cx, &cy, &r, &sa);
         c.x = (DbUnit)cx;
         c.y = (DbUnit)cy;
         tempDistance = MeasurePointToArc(point, &c, r, sa, da, width, polyXform, &tempResult);
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

/******************************************************************************
* MeasurePointToLine
*/
double MeasurePointToLine(Point2 *point,
      const CPnt *a, const CPnt *b, double width, const DTransform *lXform,
      Point2 *result)
{
   Point2 A, B;
   A.x = a->x;
   A.y = a->y;
   lXform->TransformPoint(&A);

   B.x = b->x;
   B.y = b->y;
   lXform->TransformPoint(&B);

   return DistancePointToLine(point, &A, &B, width, result);
}

/******************************************************************************
* MeasurePointToArc
*/
double MeasurePointToArc(Point2 *point,
      const CPnt *c, double r, double sa, double da, double aWidth, const DTransform *aXform,
      Point2 *result)
{
   double R, SA, DA;
   Point2 C; 
   C.x = c->x;
   C.y = c->y;
   aXform->TransformPoint(&C);
   R = r * aXform->scale;
   DA = da;
   if (aXform->mirror)
   {
      SA = PI - (sa - aXform->rotation);
      C.x = -C.x;
      DA = -da;
   }
   else
      SA = sa + aXform->rotation;

   while (SA < 0)
      SA += 2.0*PI;
   while (SA > 2.0*PI)
      SA -= 2.0*PI;

   return DistancePointToArc(point, &C, R, SA, DA, aWidth, result);
}

// end MEAS_PNT.CPP