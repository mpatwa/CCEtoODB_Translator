// $Header: /CAMCAD/4.5/Meas_Ent.cpp 21    4/28/06 6:48p Kurt Van Ness $

/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/
                   
#include "stdafx.h"
#include <float.h>
#include <math.h>
#include "data.h"
#include "apertur2.h"
#include "ccdoc.h"
#include "measure.h"
#include "outline.h"
#include "polylib.h"
#include "pcbutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/******************************************************************************
* FindClosestPoints
*
* DESCRIPTION:
*  - finds point on data1 closest to point on data2
*  - point result1 is on data1, point result2 is on data2
*
* RETURNS:
*  - distance between result1 and result2
*  
* PARAMETERS:
*  - Point2 *result1 = used to return closest point on data1
*  - Point2 *result2 = used to return closest point on data2
*/
double FindClosestPoints(CCEtoODBDoc *doc, 
      DataStruct *data1, DTransform *xform1, BOOL Centerline1, 
      DataStruct *data2, DTransform *xform2, BOOL Centerline2,
      Point2 *result1, Point2 *result2)
{
#ifdef SHAREWARE
   return 0.0;
#endif

   CPolyList *polylist1 = NULL, *polylist2 = NULL;
   switch (data1->getDataType())
   {
      case T_POLY:
         polylist1 = data1->getPolyList();
         break;

      case T_INSERT:
      {
         if (!Centerline1)
         {
            BlockStruct *subblock = doc->Find_Block_by_Num(data1->getInsert()->getBlockNumber());
            polylist1 = ApertureToPoly(subblock, data1->getInsert(), 0);//xform1->mirror);
         }
      }
      break;
   } 

   switch (data2->getDataType())
   {
      case T_POLY:
         polylist2 = data2->getPolyList();
         break;

      case T_INSERT:
      {
         if (!Centerline2)
         {
            BlockStruct *subblock = doc->Find_Block_by_Num(data2->getInsert()->getBlockNumber());
            polylist2 = ApertureToPoly(subblock, data2->getInsert(), 0);//xform2->mirror);
         }
      }
      break;
   } 

   double distance;
   if (polylist1)
   {
      if (polylist2)
         distance = MeasurePolyListToPolyList(doc, polylist1, xform1, Centerline1, polylist2, xform2, Centerline2, 
               result1, result2);
      else
      {
         Point2 point;

         point.x = data2->getInsert()->getOriginX();
         point.y = data2->getInsert()->getOriginY();
         xform2->TransformPoint(&point);
         result2->x = point.x;
         result2->y = point.y;

         distance = MeasurePointToPolylist(doc, &point,
               polylist1, xform1, Centerline1,
               result1);
      }
   }
   else
   {
      Point2 point;
      point.x = data1->getInsert()->getOriginX();
      point.y = data1->getInsert()->getOriginY();
      xform1->TransformPoint(&point);
      result1->x = point.x;
      result1->y = point.y;

      if (polylist2)
         distance = MeasurePointToPolylist(doc, &point,
               polylist2, xform2, Centerline2,
               result2);
      else
      {
         Point2 point;
         point.x = data2->getInsert()->getOriginX();
         point.y = data2->getInsert()->getOriginY();
         xform2->TransformPoint(&point);
         result2->x = point.x;
         result2->y = point.y;

         distance = sqrt(pow(result1->x - result2->x,2) + pow(result1->y - result2->y,2));
      }
   }

   if (polylist1 && data1->getDataType() == dataTypeInsert)
      FreePolyList(polylist1);

   if (polylist2 && data2->getDataType() == dataTypeInsert)
      FreePolyList(polylist2);

   return distance;
}

/******************************************************************************
* MeasurePolyListToPolyList
*/
double MeasurePolyListToPolyList(CCEtoODBDoc *doc, 
      CPolyList *polylist1, DTransform *xform1, BOOL Centerline1, 
      CPolyList *polylist2, DTransform *xform2, BOOL Centerline2, 
      Point2 *result1, Point2 *result2)
{
   double tempDistance;
   double distance = DBL_MAX;
   Point2 tempResult1, tempResult2; 

   if (polylist2 == NULL || polylist1 == NULL)   // Joanne: it will crash CAMCAD when it tries to get GetHeadPosition with null polylist2
      return -1;

   POSITION pos = polylist2->GetHeadPosition();
   while (pos)
   {
      CPoly *poly2 = polylist2->GetNext(pos);
      tempDistance = MeasurePolyListToPoly(doc, 
            polylist1, xform1, Centerline1, 
            poly2, xform2, Centerline2, 
            &tempResult1, &tempResult2);
      if (tempDistance < distance)
      {
         distance = tempDistance;
         result1->x = tempResult1.x;
         result1->y = tempResult1.y;
         result2->x = tempResult2.x;
         result2->y = tempResult2.y;
      }
   }

   return distance;
}

/******************************************************************************
* MeasurePolyListToPoly
*/
double MeasurePolyListToPoly(CCEtoODBDoc *doc, 
      CPolyList *polylist, DTransform *plXform, BOOL plCenterline, 
      CPoly *poly, DTransform *pXform, BOOL pCenterline, 
      Point2 *plResult, Point2 *pResult)
{
   double tempDistance;
   double distance = DBL_MAX;
   Point2 plTempResult, pTempResult;

   if (polylist == NULL) // Joanne: it will crash CAMCAD when it tries to get GetHeadPosition with null polylist
      return -1;

   POSITION pos = polylist->GetHeadPosition();
   while (pos)
   {
      CPoly *plPoly = polylist->GetNext(pos);
      tempDistance = MeasurePolyToPoly(doc, plPoly, plXform, plCenterline, poly, pXform, pCenterline, &plTempResult, &pTempResult);

      if (tempDistance < distance)
      {
         distance = tempDistance;
         plResult->x = plTempResult.x;
         plResult->y = plTempResult.y;
         pResult->x = pTempResult.x;
         pResult->y = pTempResult.y;
      }
   }

   return distance;
}

/******************************************************************************
* MeasurePolyToPoly
*/
double MeasurePolyToPoly(CCEtoODBDoc *doc, 
      CPoly *poly1, DTransform *xform1, BOOL Centerline1, 
      CPoly *poly2, DTransform *xform2, BOOL Centerline2,
      Point2 *result1, Point2 *result2)
{
   if (poly2->getPntList().GetCount() == 1)
   {
      CPnt *pnt = poly2->getPntList().GetHead();

      result1->x = pnt->x;
      result1->y = pnt->y;

      return MeasurePointToPoly(doc, result1, poly1, xform1, Centerline1, result2);
   }

   double width1, width2;
   
   if (Centerline1 || poly1->getWidthIndex() < 0)
      width1 = 0.0;
   else
      width1 = doc->getWidthTable()[poly1->getWidthIndex()]->getSizeA() * xform1->scale;

   if (Centerline2 || poly2->getWidthIndex() < 0)
      width2 = 0.0;
   else
      width2 = doc->getWidthTable()[poly2->getWidthIndex()]->getSizeA() * xform2->scale;

/* Commented out : Problems - Concentric circles returned touching, touching circles need to return a touching point
   // optimization for circle to circle
   Point2 c1, c2;
   double r1, r2;
   if (PolyIsCircle(poly1, &c1.x, &c1.y, &r1) && PolyIsCircle(poly2, &c2.x, &c2.y, &r2))
   {
      xform1->TransformPoint(&c1);
      r1 *= xform1->scale;
      width1 *= xform1->scale;

      xform2->TransformPoint(&c2);
      r2 *= xform2->scale;
      width2 *= xform2->scale;

      double tempResult1x, tempResult1y, tempResult2x, tempResult2y;
      FindPointOnLine(c1.x, c1.y, c2.x, c2.y, r1 + width1/2, &tempResult1x, &tempResult1y);
      FindPointOnLine(c2.x, c2.y, c1.x, c1.y, r2 + width2/2, &tempResult2x, &tempResult2y);

      result1->x = tempResult1x;
      result1->y = tempResult1y;
      result2->x = tempResult2x;
      result2->y = tempResult2y;

      double distanceToCenters = sqrt((c1.x-c2.x)*(c1.x-c2.x) + (c1.y-c2.y)*(c1.y-c2.y));
      if (distanceToCenters < r1 + width1 + r2 + width2)
      {
         if (distanceToCenters + r1 + w1 < r2 - w2)
         else if (distanceToCenters + r1 + w1 < r2 - w2)
         else
            return 0;
      }

      return distanceToCenters - r1 - width1 - r2 - width2;
   }*/

   double tempDistance;
   double distance = DBL_MAX;
   Point2 tempResult1, tempResult2;

   CPnt *p2a, *p2b;
   POSITION pos = poly2->getPntList().GetHeadPosition();
   if (pos)
      p2b = poly2->getPntList().GetNext(pos);

   while (pos)
   {
      p2a = p2b;
      p2b = poly2->getPntList().GetNext(pos);

      if (fabs(p2a->bulge) < BULGE_THRESHOLD)
         tempDistance = MeasurePolyToLine(poly1, width1, xform1, 
               p2a, p2b, width2, xform2, 
               &tempResult1, &tempResult2);
      else
      {
         double da, sa, r, cx, cy;
         CPnt c;
         da = atan(p2a->bulge) * 4;
         ArcPoint2Angle(p2a->x, p2a->y, p2b->x, p2b->y, da, &cx, &cy, &r, &sa);
         c.x = (DbUnit)cx;
         c.y = (DbUnit)cy;
         tempDistance = MeasurePolyToArc(poly1, width1, xform1,
               &c, r, sa, da, width2, xform2, 
               &tempResult1, &tempResult2);
      }

      if (tempDistance < distance)
      {
         distance = tempDistance;
         result1->x = tempResult1.x;
         result1->y = tempResult1.y;
         result2->x = tempResult2.x;
         result2->y = tempResult2.y;
      }
   }

   return distance;
}

/******************************************************************************
* MeasurePolyToPoly
*/
double MeasurePolyToPoly( 
      const CPoly* poly1,const DTransform* xform1, BOOL Centerline1, double width1, 
      const CPoly* poly2,const DTransform* xform2, BOOL Centerline2, double width2, 
      Point2 *result1, Point2 *result2)
{
   if (poly2->getPntList().GetCount() == 1)
   {
      CPnt *pnt = poly2->getPntList().GetHead();

      result1->x = pnt->x;
      result1->y = pnt->y;

      return MeasurePointToPoly(result1, poly1, xform1, Centerline1, width1, result2);
   }
   
   if (Centerline1)
   {
      width1 = 0.0;
   }

   if (Centerline2)
   {
      width2 = 0.0;
   }

   double tempDistance;
   double distance = DBL_MAX;
   Point2 tempResult1, tempResult2;

   CPnt *p2a, *p2b;
   POSITION pos = poly2->getPntList().GetHeadPosition();
   if (pos)
      p2b = poly2->getPntList().GetNext(pos);

   while (pos)
   {
      p2a = p2b;
      p2b = poly2->getPntList().GetNext(pos);

      if (fabs(p2a->bulge) < BULGE_THRESHOLD)
      {
         tempDistance = MeasurePolyToLine(poly1, width1, xform1, 
               p2a, p2b, width2, xform2, 
               &tempResult1, &tempResult2);
      }
      else
      {
         double da, sa, r, cx, cy;
         CPnt c;
         da = atan(p2a->bulge) * 4;
         ArcPoint2Angle(p2a->x, p2a->y, p2b->x, p2b->y, da, &cx, &cy, &r, &sa);
         c.x = (DbUnit)cx;
         c.y = (DbUnit)cy;
         tempDistance = MeasurePolyToArc(poly1, width1, xform1,
               &c, r, sa, da, width2, xform2, 
               &tempResult1, &tempResult2);
      }

      if (tempDistance < distance)
      {
         distance = tempDistance;
         result1->x = tempResult1.x;
         result1->y = tempResult1.y;
         result2->x = tempResult2.x;
         result2->y = tempResult2.y;
      }
   }

   return distance;
}

/******************************************************************************
* MeasurePolyToLine
*/
double MeasurePolyToLine(const CPoly *poly, double pWidth, const DTransform *pXform, 
      const CPnt *la, const CPnt *lb, double lWidth, const DTransform *lXform, 
      Point2 *pResult, Point2 *lResult)
{
   if (poly->getPntList().GetCount() == 1)
   {
      CPnt *pnt = poly->getPntList().GetHead();

      pResult->x = pnt->x;
      pResult->y = pnt->y;

      return MeasurePointToLine(pResult, la, lb, lWidth, lXform, lResult);
   }

   double tempDistance;
   double distance = DBL_MAX;
   Point2 pTempResult, lTempResult;

   CPnt *pa, *pb;
   POSITION pos = poly->getPntList().GetHeadPosition();
   if (pos)
      pb = poly->getPntList().GetNext(pos);

   while (pos)
   {
      pa = pb;
      pb = poly->getPntList().GetNext(pos);

      if (fabs(pa->x-pb->x) < SMALLNUMBER && fabs(pa->y-pb->y) < SMALLNUMBER)
      {
			pTempResult.x = pa->x;
			pTempResult.y = pa->y;
         tempDistance = MeasurePointToLine(&pTempResult, la, lb, lWidth, lXform, &lTempResult);
      }
      else if (fabs(pa->bulge) < BULGE_THRESHOLD)
      {
         tempDistance = MeasureLineToLine(pa, pb, pWidth, pXform, 
               la, lb, lWidth, lXform, 
               &pTempResult, &lTempResult);
      }
      else
      {
         double da, sa, r, cx, cy;
         CPnt c;
         da = atan(pa->bulge) * 4;
         ArcPoint2Angle(pa->x, pa->y, pb->x, pb->y, da, &cx, &cy, &r, &sa);
         c.x = (DbUnit)cx;
         c.y = (DbUnit)cy;
         tempDistance = MeasureLineToArc(la, lb, lWidth, lXform, 
               &c, r, sa, da, pWidth, pXform,
               &pTempResult, &lTempResult);
      }

      if (tempDistance < distance)
      {
         distance = tempDistance;
         pResult->x = pTempResult.x;
         pResult->y = pTempResult.y;
         lResult->x = lTempResult.x;
         lResult->y = lTempResult.y;
      }
   }

   return distance;
}

/******************************************************************************
* MeasurePolyToArc
*/
double MeasurePolyToArc(const CPoly *poly, double pWidth, const DTransform *pXform, 
      const CPnt *aC, double aR, double aSa, double aDa, double aWidth, const DTransform *aXform, 
      Point2 *pResult, Point2 *aResult)
{
   if (poly->getPntList().GetCount() == 1)
   {
      CPnt *pnt = poly->getPntList().GetHead();

      pResult->x = pnt->x;
      pResult->y = pnt->y;

      return MeasurePointToArc(pResult, aC, aR, aSa, aDa, aWidth, aXform, aResult);
   }

   double tempDistance;
   double distance = DBL_MAX;
   Point2 pTempResult, aTempResult;

   CPnt *pa, *pb;
   POSITION pos = poly->getPntList().GetHeadPosition();
   if (pos)
      pb = poly->getPntList().GetNext(pos);

   while (pos != NULL)
   {
      pa = pb;
      pb = poly->getPntList().GetNext(pos);

      if (fabs(pa->x-pb->x) < SMALLNUMBER && fabs(pa->y-pb->y) < SMALLNUMBER)
      {
         Point2 p;
         p.x = pa->x;
         p.y = pa->y;
         tempDistance = MeasurePointToArc(&p, aC, aR, aSa, aDa, aWidth, aXform, aResult);
      }
      else if (fabs(pa->bulge) < BULGE_THRESHOLD)
      {
         tempDistance = MeasureLineToArc(pa, pb, pWidth, pXform,
               aC, aR, aSa, aDa, aWidth, aXform,
               &pTempResult, &aTempResult);
      }
      else
      {
         double pCx, pCy, pR, pSa, pDa;
         pDa = atan(pa->bulge) * 4;
         ArcPoint2Angle(pa->x, pa->y, pb->x, pb->y, pDa, &pCx, &pCy, &pR, &pSa);
         CPnt pC;
         pC.x = (DbUnit)pCx;
         pC.y = (DbUnit)pCy;
         tempDistance = MeasureArcToArc(&pC, pR, pSa, pDa, pWidth, pXform,
               aC, aR, aSa, aDa, aWidth, aXform,
               &pTempResult, &aTempResult);
      }

      if (tempDistance < distance)
      {
         distance = tempDistance;
         pResult->x = pTempResult.x;
         pResult->y = pTempResult.y;
         aResult->x = aTempResult.x;
         aResult->y = aTempResult.y;
      }
   }

   return distance;
}

/******************************************************************************
* MeasureLineToLine
*/
double MeasureLineToLine(const CPnt *l1a, const CPnt *l1b, double width1, const DTransform *xform1, 
      const CPnt *l2a, const CPnt *l2b, double width2, const DTransform *xform2,
      Point2 *result1, Point2 *result2)
{
   Point2 L1a, L1b, L2a, L2b;
   
   L1a.x = l1a->x;
   L1a.y = l1a->y;
   xform1->TransformPoint(&L1a);

   L1b.x = l1b->x;
   L1b.y = l1b->y;
   xform1->TransformPoint(&L1b);

   L2a.x = l2a->x;
   L2a.y = l2a->y;
   xform2->TransformPoint(&L2a);

   L2b.x = l2b->x;
   L2b.y = l2b->y;
   xform2->TransformPoint(&L2b);

   return DistanceLineToLine(&L1a, &L1b, width1, &L2a, &L2b, width2, result1, result2);
}

/******************************************************************************
* MeasureLineToArc
*/
double MeasureLineToArc(const CPnt *la, const CPnt *lb, double lWidth, const DTransform *lXform,
      const CPnt *c, double r, double sa, double da, double aWidth, const DTransform *aXform,
      Point2 *lResult, Point2 *aResult)
{
   Point2 La, Lb;
   
   La.x = la->x;
   La.y = la->y;
   lXform->TransformPoint(&La);

   Lb.x = lb->x;
   Lb.y = lb->y;
   lXform->TransformPoint(&Lb);

   
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

   return DistanceLineToArc(&La, &Lb, lWidth, &C, R, SA, DA, aWidth, lResult, aResult);
}

/******************************************************************************
* MeasureArcToArc
*/
static double MeasureArcToArc(const CPnt *c1, double r1, double sa1, double da1, double width1, const DTransform *xform1, 
                              const CPnt *c2, double r2, double sa2, double da2, double width2, const DTransform *xform2,
                              Point2 *result1, Point2 *result2)
{
   double R1, SA1, DA1, R2, SA2, DA2;
   Point2 C1, C2; 

   C1.x = c1->x;
   C1.y = c1->y;
   xform1->TransformPoint(&C1);
   R1 = r1 * xform1->scale;
   DA1 = da1;
   if (xform1->mirror)
   {
      SA1 = PI - (sa1 - xform1->rotation);
      C1.x = -C1.x;
      DA1 = -da1;
   }
   else
      SA1 = sa1 + xform1->rotation;

   C2.x = c2->x;
   C2.y = c2->y;
   xform2->TransformPoint(&C2);
   R2 = r2 * xform2->scale;
   DA2 = da2;
   if (xform2->mirror)
   {
      SA2 = PI - (sa2 - xform2->rotation);
      C2.x = -C2.x;
      DA2 = -da2;
   }
   else
      SA2 = sa2 + xform2->rotation;

   return DistanceArcToArc(&C1, R1, SA1, DA1, width1, &C2, R2, SA2, DA2, width2, result1, result2);
}


/******************************************************************************
* MeasureApertureToAperture
*/
double MeasureApertureToAperture(CCEtoODBDoc *doc,
      BlockStruct *b1, double x1, double y1, double rotation1, int mirror1,
      BlockStruct *b2, double x2, double y2, double rotation2, int mirror2,
      Point2 *result1, Point2 *result2)
{
   CPolyList *ap1, *ap2;
   double   accuracy = get_accuracy(doc);

   if (b1->getShape() == T_COMPLEX)
   {
      BlockStruct *block = doc->Find_Block_by_Num((int)(b1->getSizeA()));
      Outline_Start(doc);
      POSITION pos = block->getDataList().GetHeadPosition();
      while (pos)
         Outline_Add(block->getDataList().GetNext(pos), 1.0, x1, y1, rotation1, mirror1);
      int returnCode;
      CPntList *pntlist = Outline_GetOutline(&returnCode, accuracy);

      if (!pntlist)
         return DBL_MAX;

      ap1 = new CPolyList;
      CPoly *poly = new CPoly;
      poly->setFloodBoundary(false);
      poly->setClosed(false);
      poly->setFilled(false);
      poly->setHatchLine(false);
      poly->setHidden(false);
      poly->setThermalLine(false);
      poly->setVoid(false);
      poly->setWidthIndex(0);
      ap1->AddHead(poly);
      while (pntlist->GetCount())
         poly->getPntList().AddTail(pntlist->RemoveHead());
      Outline_FreeResults();
   }
   else
      ap1 = ApertureToPoly_Base(b1, x1, y1, rotation1, mirror1);

   if (b2->getShape() == T_COMPLEX)
   {
      BlockStruct *block = doc->Find_Block_by_Num((int)(b2->getSizeA()));
      Outline_Start(doc);
      POSITION pos = block->getDataList().GetHeadPosition();
      while (pos)
         Outline_Add(block->getDataList().GetNext(pos), 1.0, x2, y2, rotation2, mirror2);
      int returnCode;
      CPntList *pntlist = Outline_GetOutline(&returnCode, accuracy);

      if (!pntlist)
         return DBL_MAX;

      ap2 = new CPolyList;
      CPoly *poly = new CPoly;
      poly->setFloodBoundary(false);
      poly->setClosed(false);
      poly->setFilled(false);
      poly->setHatchLine(false);
      poly->setHidden(false);
      poly->setThermalLine(false);
      poly->setVoid(false);
      poly->setWidthIndex(0);
      ap2->AddHead(poly);
      while (pntlist->GetCount())
         poly->getPntList().AddTail(pntlist->RemoveHead());
      Outline_FreeResults();
   }
   else
      ap2 = ApertureToPoly_Base(b2, x2, y2, rotation2, mirror2);

   DTransform xform1, xform2;
   double distance = MeasurePolyListToPolyList(doc,
         ap1, &xform1, FALSE, 
         ap2, &xform2, FALSE,
         result1, result2);

   FreePolyList(ap1);
   FreePolyList(ap2);

   return distance;
}

