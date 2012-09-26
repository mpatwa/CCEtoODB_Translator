// $Header: /CAMCAD/DcaLib/DcaMeasure.cpp 7     3/23/07 5:19a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaMeasure.h"
#include "DcaPoly.h"
#include "DcaGeomLib.h"
#include "DcaBasesVector.h"
#include "DcaLib.h"
#include "DcaTMatrix.h"
#include "DcaPoint2.h"

//_____________________________________________________________________________
double MeasurePolyToPoly( 
   const CPoly& poly1,const CTMatrix& matrix1,bool Centerline1,double width1,
   const CPoly& poly2,const CTMatrix& matrix2,bool Centerline2,double width2,
   Point2& result1,Point2& result2)
{
   if (poly2.getPntList().GetCount() == 1)
   {
      CPnt* pnt = poly2.getPntList().GetHead();

      result1.x = pnt->x;
      result1.y = pnt->y;

      return MeasurePointToPoly(result1,poly1,matrix1,Centerline1,width1,result2);
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
   Point2 tempResult1,tempResult2;

   CPnt *p2a,*p2b;
   POSITION pos = poly2.getPntList().GetHeadPosition();
   if (pos)
      p2b = poly2.getPntList().GetNext(pos);

   while (pos)
   {
      p2a = p2b;
      p2b = poly2.getPntList().GetNext(pos);

      if (fabs(p2a->bulge) < BULGE_THRESHOLD)
      {
         tempDistance = MeasurePolyToLine(poly1,width1,matrix1,*p2a,*p2b,width2,matrix2,tempResult1,tempResult2);
      }
      else
      {
         double da,sa,r,cx,cy;
         CPnt c;
         da = atan(p2a->bulge) * 4;

         ArcPoint2Angle(p2a->x,p2a->y,p2b->x,p2b->y,da,&cx,&cy,&r,&sa);

         c.x = (DbUnit)cx;
         c.y = (DbUnit)cy;

         tempDistance = MeasurePolyToArc(poly1,width1,matrix1,c,r,sa,da,width2,matrix2,tempResult1,tempResult2);
      }

      if (tempDistance < distance)
      {
         distance = tempDistance;

         result1.x = tempResult1.x;
         result1.y = tempResult1.y;
         result2.x = tempResult2.x;
         result2.y = tempResult2.y;
      }
   }

   return distance;
}

//_____________________________________________________________________________
//double MeasurePolyToLine(const CPoly* poly,double pWidth,const DTransform* pXform,
//      const CPnt* la,const CPnt* lb,double lWidth,const DTransform* lXform,
//      Point2* pResult,Point2* lResult)
double MeasurePolyToLine(const CPoly& poly,double pWidth,const CTMatrix& pMatrix,
   const CPnt& la,const CPnt& lb,double lWidth,const CTMatrix& lMatrix,
      Point2& pResult,Point2& lResult)
{
   if (poly.getPntList().GetCount() == 1)
   {
      CPnt* pnt = poly.getPntList().GetHead();

      pResult.x = pnt->x;
      pResult.y = pnt->y;

      return MeasurePointToLine(pResult,la,lb,lWidth,lMatrix,lResult);
   }

   double tempDistance;
   double distance = DBL_MAX;
   Point2 pTempResult,lTempResult;

   CPnt *pa,*pb;
   POSITION pos = poly.getPntList().GetHeadPosition();
   if (pos != NULL)
      pb = poly.getPntList().GetNext(pos);

   while (pos != NULL)
   {
      pa = pb;
      pb = poly.getPntList().GetNext(pos);

      if (fabs(pa->x - pb->x) < SMALLNUMBER && fabs(pa->y - pb->y) < SMALLNUMBER)
      {
         pTempResult.x = pa->x;
         pTempResult.y = pa->y;
         tempDistance = MeasurePointToLine(pTempResult,la,lb,lWidth,lMatrix,lTempResult);
      }
      else if (fabs(pa->bulge) < BULGE_THRESHOLD)
      {
         tempDistance = MeasureLineToLine(*pa,*pb,pWidth,pMatrix,la,lb,lWidth,lMatrix,pTempResult,lTempResult);
      }
      else
      {
         double da,sa,r,cx,cy;
         CPnt c;
         da = atan(pa->bulge) * 4;

         ArcPoint2Angle(pa->x,pa->y,pb->x,pb->y,da,&cx,&cy,&r,&sa);
         c.x = (DbUnit)cx;
         c.y = (DbUnit)cy;

         tempDistance = MeasureLineToArc(la,lb,lWidth,lMatrix,c,r,sa,da,pWidth,pMatrix,pTempResult,lTempResult);
      }

      if (tempDistance < distance)
      {
         distance = tempDistance;
         pResult.x = pTempResult.x;
         pResult.y = pTempResult.y;
         lResult.x = lTempResult.x;
         lResult.y = lTempResult.y;
      }
   }

   return distance;
}

//_____________________________________________________________________________
//double MeasurePointToPoly(Point2* point,
//      const CPoly* poly,const DTransform* polyXform,BOOL Centerline,double width,
//      Point2* result)
double MeasurePointToPoly(Point2& point,const CPoly& poly,const CTMatrix& matrix,bool Centerline,double width,Point2& result)
{
   if (Centerline)
   {
      width = 0.0;
   }

   return MeasurePointToPntList(point,poly.getPntList(),width,matrix,Centerline,result);
}

//_____________________________________________________________________________
//double MeasurePointToPntList(Point2* point,
//      const CPntList* pntList,double width,const DTransform* polyXform,BOOL Centerline,
//      Point2* result)
double MeasurePointToPntList(const Point2& point,const CPntList& pntList,double width,const CTMatrix& matrix,bool Centerline,Point2& result)
{
   double tempDistance;
   double distance = DBL_MAX;
   Point2 tempResult;

/* // optimization for circle
   double cx,cy,r;
   if (PolyIsCircle(poly,&cx,&cy,&r))
   {
      double tempDistance = sqrt( (cx-x)*(cx-x) + (cy-y)*(cy-y) ) - r;

      if (tempDistance < distance)
      {
         distance = tempDistance;

         double tx,ty;
         FindPointOnLine(cx,cy,x,y,r,&tx,&ty);

         result->x = tx;
         result->y = ty;
      }
      return;
   }
*/

   if (pntList.GetCount() == 1)
   {
      CPnt* pnt = pntList.GetHead();

      result.x = pnt->x;
      result.y = pnt->y;

      double dx = point.x - pnt->x;
      double dy = point.y - pnt->y;

      return sqrt(dx*dx + dy*dy);
   }

   CPnt *a,*b;
   POSITION pos = pntList.GetHeadPosition();

   if (pos != NULL)
      b = pntList.GetNext(pos);

   while (pos != NULL)
   {
      a = b;
      b = pntList.GetNext(pos);

      if (fabs(a->bulge) < BULGE_THRESHOLD)
      {
         tempDistance = MeasurePointToLine(point,*a,*b,width,matrix,tempResult);
      }
      else
      {
         double da,sa,r,cx,cy;
         CPnt c;

         da = atan(a->bulge) * 4;
         ArcPoint2Angle(a->x,a->y,b->x,b->y,da,&cx,&cy,&r,&sa);

         c.x = (DbUnit)cx;
         c.y = (DbUnit)cy;

         tempDistance = MeasurePointToArc(point,c,r,sa,da,width,matrix,tempResult);
      }

      if (tempDistance < distance)
      {
         distance = tempDistance;
         result.x = tempResult.x;
         result.y = tempResult.y;
      }
   }

   return distance;
}

//_____________________________________________________________________________
//double MeasurePolyToArc(const CPoly* poly,double pWidth,const DTransform* pXform,
//      const CPnt* aC,double aR,double aSa,double aDa,double aWidth,const DTransform* aXform,
//      Point2* pResult,Point2* aResult)
double MeasurePolyToArc(const CPoly& poly,double pWidth,const CTMatrix& pMatrix,
   const CPnt& aC,double aR,double aSa,double aDa,double aWidth,const CTMatrix& aMatrix,
   Point2& pResult,Point2& aResult)
{
   if (poly.getPntList().GetCount() == 1)
   {
      CPnt* pnt = poly.getPntList().GetHead();

      pResult.x = pnt->x;
      pResult.y = pnt->y;

      return MeasurePointToArc(pResult,aC,aR,aSa,aDa,aWidth,aMatrix,aResult);
   }

   double tempDistance;
   double distance = DBL_MAX;
   Point2 pTempResult,aTempResult;

   CPnt *pa,*pb;
   POSITION pos = poly.getPntList().GetHeadPosition();

   if (pos != NULL)
      pb = poly.getPntList().GetNext(pos);

   while (pos != NULL)
   {
      pa = pb;
      pb = poly.getPntList().GetNext(pos);

      if (fabs(pa->x - pb->x) < SMALLNUMBER && fabs(pa->y - pb->y) < SMALLNUMBER)
      {
         Point2 p;
         p.x = pa->x;
         p.y = pa->y;

         tempDistance = MeasurePointToArc(p,aC,aR,aSa,aDa,aWidth,aMatrix,aResult);
      }
      else if (fabs(pa->bulge) < BULGE_THRESHOLD)
      {
         tempDistance = MeasureLineToArc(*pa,*pb,pWidth,pMatrix,aC,aR,aSa,aDa,aWidth,aMatrix,pTempResult,aTempResult);
      }
      else
      {
         double pCx,pCy,pR,pSa,pDa;
         pDa = atan(pa->bulge) * 4;
         ArcPoint2Angle(pa->x,pa->y,pb->x,pb->y,pDa,&pCx,&pCy,&pR,&pSa);

         CPnt pC;
         pC.x = (DbUnit)pCx;
         pC.y = (DbUnit)pCy;

         tempDistance = MeasureArcToArc(pC,pR,pSa,pDa,pWidth,pMatrix,aC,aR,aSa,aDa,aWidth,aMatrix,pTempResult,aTempResult);
      }

      if (tempDistance < distance)
      {
         distance = tempDistance;

         pResult.x = pTempResult.x;
         pResult.y = pTempResult.y;
         aResult.x = aTempResult.x;
         aResult.y = aTempResult.y;
      }
   }

   return distance;
}

//_____________________________________________________________________________
//double MeasurePointToLine(Point2* point,
//      const CPnt* a,const CPnt* b,double width,const DTransform* lXform,
//      Point2* result)
double MeasurePointToLine(const Point2& point,const CPnt& a,const CPnt& b,double width,const CTMatrix& lMatrix,Point2& result)
{
   Point2 A(a);
   A.transform(lMatrix);

   Point2 B(b);
   B.transform(lMatrix);

   Point2 p(point);

   return DistancePointToLine(&p,&A,&B,width,&result);
}

//_____________________________________________________________________________
double MeasureLineToArc(const CPnt& la,const CPnt& lb,double lWidth,const CTMatrix& lMatrix,
   const CPnt& c,double r,double sa,double da,double aWidth,const CTMatrix& aMatrix,
   Point2& lResult,Point2& aResult)
{
   Point2 La(la);
   La.transform(lMatrix);

   Point2 Lb(lb);
   Lb.transform(lMatrix);

   CBasesVector arcCenterStart(c.x,c.y,radiansToDegrees(sa));
   CBasesVector arcCenterEnd(  c.x,c.y,radiansToDegrees(sa + da));

   arcCenterStart.transform(aMatrix);
   arcCenterEnd.transform(aMatrix);

   Point2 C(arcCenterStart.getX(),arcCenterStart.getY());
   double R  = r * aMatrix.getScale();
   double SA = arcCenterStart.getRotationRadians();
   double DA = arcCenterEnd.getRotationRadians() - SA;

   SA = normalizeRadians(SA);

   return DistanceLineToArc(&La,&Lb,lWidth,&C,R,SA,DA,aWidth,&lResult,&aResult);
}

//_____________________________________________________________________________
double MeasureLineToLine(
   const CPnt& l1a,const CPnt& l1b,double width1,const CTMatrix& matrix1,
   const CPnt& l2a,const CPnt& l2b,double width2,const CTMatrix& matrix2,
   Point2& result1,Point2& result2)
{
   Point2 L1a(l1a);
   L1a.transform(matrix1);

   Point2 L1b(l1b);
   L1b.transform(matrix1);

   Point2 L2a(l2a);
   L2a.transform(matrix2);

   Point2 L2b(l2b);
   L2b.transform(matrix2);

   return DistanceLineToLine(&L1a,&L1b,width1,&L2a,&L2b,width2,&result1,&result2);
}

//_____________________________________________________________________________
double MeasurePointToArc(const Point2& point,const CPnt& c,double r,double sa,double da,double aWidth,const CTMatrix& aMatrix,Point2& result)
{
   CBasesVector arcCenterStart(c.x,c.y,radiansToDegrees(sa));
   CBasesVector arcCenterEnd(  c.x,c.y,radiansToDegrees(sa + da));

   arcCenterStart.transform(aMatrix);
   arcCenterEnd.transform(aMatrix);

   double R,SA,DA;
   Point2 C; 

   C.x = arcCenterStart.getX();
   C.y = arcCenterStart.getY();

   R = r * aMatrix.getScale();
   SA = arcCenterStart.getRotationRadians();
   DA = arcCenterEnd.getRotationRadians() - SA;

   SA = normalizeRadians(SA);

   Point2 p(point);

   return DistancePointToArc(&p,&C,R,SA,DA,aWidth,&result);
}

double MeasureArcToArc(
   const CPnt& c1,double r1,double sa1,double da1,double width1,const CTMatrix& matrix1,
   const CPnt& c2,double r2,double sa2,double da2,double width2,const CTMatrix& matrix2,
   Point2& result1,Point2& result2)
{
   CBasesVector arcCenterStart1(c1.x,c1.y,radiansToDegrees(sa1));
   CBasesVector arcCenterEnd1(  c1.x,c1.y,radiansToDegrees(sa1 + da1));

   arcCenterStart1.transform(matrix1);
   arcCenterEnd1.transform(matrix1);

   Point2 C1(arcCenterStart1.getX(),arcCenterStart1.getY());
   double R1  = r1 * matrix1.getScale();
   double SA1 = arcCenterStart1.getRotationRadians();
   double DA1 = arcCenterEnd1.getRotationRadians() - SA1;

   CBasesVector arcCenterStart2(c2.x,c2.y,radiansToDegrees(sa2));
   CBasesVector arcCenterEnd2(  c2.x,c2.y,radiansToDegrees(sa2 + da2));

   arcCenterStart2.transform(matrix2);
   arcCenterEnd2.transform(matrix2);

   Point2 C2(arcCenterStart2.getX(),arcCenterStart2.getY());
   double R2  = r2 * matrix2.getScale();
   double SA2 = arcCenterStart2.getRotationRadians();
   double DA2 = arcCenterEnd2.getRotationRadians() - SA2;

   return DistanceArcToArc(&C1,R1,SA1,DA1,width1,&C2,R2,SA2,DA2,width2,&result1,&result2);
}

