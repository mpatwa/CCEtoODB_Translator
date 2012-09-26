// $Header: /CAMCAD/DcaLib/DcaPoly.cpp 9     6/21/07 8:30p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaPoly.h"
#include "DcaExtent.h"
#include "DcaCamCadData.h"
#include "DcaBasesVector.h"
#include "DcaPolyLib.h"
#include "DcaArcPoint.h"
#include "DcaMeasure.h"
#include "DcaWriteFormat.h"
#include "DcaPolygon.h"
#include "DcaLib.h"
#include "DcaTMatrix.h"
#include "DcaFillStyle.h"
#include "DcaPoint2.h"
#include "DcaPoint3d.h"
#include "DcaCrcCalculator.h"

//_____________________________________________________________________________
CPoly::CPoly()
{
   init();
}

CPoly::CPoly(const CPoly& other) : 
   list(other.list)
{
   setWidthIndex(other.getWidthIndex());

   Filled       = other.Filled;
   Closed       = other.Closed;
   VoidPoly     = other.VoidPoly;
   Hidden       = other.Hidden;
   ThermalLines = other.ThermalLines;
   BoundaryPoly = other.BoundaryPoly;
   Breakout     = other.Breakout;
   HatchLines   = other.HatchLines;
}

CPoly::CPoly(const CPntList& pntList) : 
   list(pntList)
{
   init();
}

CPoly::CPoly(const CExtent& extent) 
{
   init();

   list.AddTail(new CPnt(extent.getXmin(),extent.getYmin()));
   list.AddTail(new CPnt(extent.getXmax(),extent.getYmin()));
   list.AddTail(new CPnt(extent.getXmax(),extent.getYmax()));
   list.AddTail(new CPnt(extent.getXmin(),extent.getYmax()));
   list.AddTail(new CPnt(extent.getXmin(),extent.getYmin()));
}

CPoly::~CPoly()
{
}

void CPoly::init()
{
   setWidthIndex(0);
   Filled       = 0;
   Closed       = 0;
   VoidPoly     = 0;
   Hidden       = 0;
   ThermalLines = 0;
   BoundaryPoly = 0;
   Breakout     = 0;
   HatchLines   = 0;
}

void CPoly::empty()
{
   list.empty();
}

void CPoly::setWidthIndex(int index)
{ 
   m_widthIndex = index; 

   if (index > 400)
   {
      int iii = 3;
   }
}

CPnt* CPoly::addVertex(double x,double y,double bulge)
{
   // 572 is a bulge for 89.9 * 4 = 359.6
   // after this number, the bulge does into infinity.
   // infinity 1*e16 is for 90 degree * 4 = a full circle
   if (fabs(bulge) > 572)
   {
      bulge = 0;  
   }

   CPnt* pnt = new CPnt(x,y,bulge);

   list.AddTail(pnt);

   return pnt;
}

CPnt* CPoly::addVertex(const CPoint2d& point)
{
   return addVertex(point.x,point.y);
}

CPnt* CPoly::addVertex(const  Point2&  point)
{
   return addVertex(point.x,point.y,point.bulge);
}

void CPoly::transform(const CTMatrix& transformationMatrix,CCamCadData* camCadData)
{
   list.transform(transformationMatrix);

   if (camCadData != NULL)
   {
      BlockStruct& widthBlock = camCadData->getDefinedWidthBlock(getWidthIndex());

      CBasesVector basesVector(0.,0.,1.,radiansToDegrees(widthBlock.getRotation()),false);
      basesVector.transform(transformationMatrix);

      double scale = basesVector.getScale();

      if (!fpeq(scale,1.0))
      {
         //int newWidthIndex = 
         //   Graph_Aperture("",widthBlock.getShape(),widthBlock.getSizeA() * scale,
         //      widthBlock.getSizeB() * scale,widthBlock.getXoffset() * scale,
         //      widthBlock.getYoffset() * scale,degreesToRadians(basesVector.getRotation()),0, 
         //      widthBlock.getFlags(), FALSE, NULL);

         int newWidthIndex = camCadData->getDefinedApertureIndex("",widthBlock.getShape(),
            widthBlock.getSizeA()   * scale,widthBlock.getSizeB()   * scale,
            widthBlock.getXoffset() * scale,widthBlock.getYoffset() * scale,
            basesVector.getRotationRadians());

         setWidthIndex(newWidthIndex);
      }
   }
}

void CPoly::updateExtent(CExtent& extent) const
{
   list.updateExtent(extent);
}

void CPoly::updateExtent(CExtent& extent,const CTMatrix& transformationMatrix) const
{
   list.updateExtent(extent,transformationMatrix);
}

void CPoly::updateExtent(CExtent& extent,CCamCadData& camCadData) const
{
   BlockStruct* widthBlock = camCadData.getWidthBlock(getWidthIndex());

   if (widthBlock != NULL)
   {
      double width = widthBlock->getSizeA();
      CExtent listExtent = list.getExtent();
      listExtent.expand(width);
      extent.update(listExtent);
   }
   else
   {
      list.updateExtent(extent);
   }
}

CExtent CPoly::getExtent() const
{
   CExtent extent;

   updateExtent(extent);

   return extent;
}

CExtent CPoly::getExtent(const CTMatrix& transformationMatrix) const
{
   CExtent extent;

   updateExtent(extent,transformationMatrix);

   return extent;
}

bool CPoly::isPointInside(const CPoint2d& point,PageUnitsTag pageUnits,double tolerance) const
{
   bool retval = list.isPointInside(point,pageUnits,tolerance);

   return retval;
}

bool CPoly::isPointOnSegment(const CPoint2d& point,PageUnitsTag pageUnits,double tolerance) const
{
   bool retval = list.isPointOnSegment(point,pageUnits,tolerance);

   return retval;
}

double CPoly::distanceTo(const CPoint2d& point,CPoint2d* pointResult) const
{
   double retval = list.distanceTo(point,pointResult);

   return retval;
}

// returns true if the poly was modified
bool CPoly::close()
{
   bool retval = false;

   if (list.GetCount() > 0)
   {
      CPnt* head = list.GetHead();
      CPnt* tail = list.GetTail();

      if (head->x != tail->x || head->y != tail->y)
      {
         retval = true;

         if (fpeq(head->x,tail->x) && fpeq(head->y,tail->y))
         {
            tail->x = head->x;
            tail->y = head->y;
            tail->bulge = 0.;
         }
         else
         {
            tail->bulge = 0.;

            CPnt* newPoint = new CPnt(*head);
            newPoint->bulge = 0.;

            list.AddTail(newPoint);
         }

         Closed = 1;
      }

      if (!Closed)
      {
         Closed = 1;
         retval = true;
      }
   }

   return retval;
}

// returns true if the poly was modified
bool CPoly::closePolyWithCoincidentEndPoints()
{
   bool retval = false;

   if (list.GetCount() > 1)
   {
      CPnt* head = list.GetHead();
      CPnt* tail = list.GetTail();

      if (fpeq(head->x,tail->x) && fpeq(head->y,tail->y))
      {
         if (!Closed || head->x != tail->x || head->y != tail->y)
         {
            tail->x = head->x;
            tail->y = head->y;
            tail->bulge = 0.;

            Closed = 1;

            retval = true;
         }
      }
   }

   return retval;
}

void CPoly::vectorize(PageUnitsTag pageUnits)
{
   list.vectorize(pageUnits);
}

int CPoly::removeDuplicateVertices()
{
   int duplicateCount = 0;

   if (list.GetCount() > 2)
   {
      CPnt* prevPnt = NULL;

      for (POSITION pos = list.GetHeadPosition();pos != NULL;)
      {
         POSITION prevPos = pos;
         CPnt* pnt = list.GetNext(pos);

         if (prevPnt != NULL)
         {
            if ((pnt->x == prevPnt->x) && (pnt->y == prevPnt->y))
            {
               if (pnt->bulge == 0.)
               {
                  pnt->bulge = prevPnt->bulge;
               }

               list.RemoveAt(prevPos);
               duplicateCount++;
            }
         }

         prevPnt = pnt;
      }
   }

   return duplicateCount;
}

bool CPoly::isCcw() const
{
   bool retval = (IsClockwise(&list) == 0);

   return retval;
}

// return true if the polygon is modified
bool CPoly::makeCcw()
{
   bool retval = false;

   if (! isCcw())
   {
      retval = reverse();
   }

   return retval;
}

bool CPoly::isClosedInData()
{
   // We have the Closed flag, but data may or may not match.
   // This returns true if there are atleast three pts and first==last.

   if (this->getVertexCount() >= 3)
   {
      CPnt *headPnt = this->getPntList().GetHead();
      CPnt *tailPnt = this->getPntList().GetTail();
      if (headPnt != NULL && tailPnt != NULL) // paranoid
      {
         // Care about X and Y only.  Bulge does not matter.
         if (fpeq(headPnt->x, tailPnt->x, SMALLNUMBER) && fpeq(headPnt->y, tailPnt->y, SMALLNUMBER))
            return true;
      }
   }

   return false;
}

bool CPoly::reverse()
{
   return list.reverse();
}

double CPoly::getLinePolygonArea2() const
{
   double area2 = 0.;

   if (list.GetCount() > 2)
   {
      CPnt* previousPoint = list.GetTail();

      for (POSITION pos = list.GetHeadPosition();pos != NULL;)
      {
         CPnt* point = list.GetNext(pos);

         area2 += ((previousPoint->x + point->x)*(point->y - previousPoint->y));

         previousPoint = point;
      }
   }

   return area2;
}

bool CPoly::convertToOutline(double width,double tolerance,int zeroWidthIndex, ApertureShapeTag apShape)
{
   bool retval = false;

   clean(tolerance);
   makeCcw();

   CPolyArcLine polyArcLine(*this);
   CArcLinePolygon arcLinePolygon;

   if (polyArcLine.getOutline(arcLinePolygon,width,tolerance, apShape))
   {
      arcLinePolygon.getPoly(*this);
      retval = true;
   }

   m_widthIndex = zeroWidthIndex;

   return retval;
}

bool CPoly::convertToInline(double width,double tolerance,int zeroWidthIndex)
{
   bool retval = false;

   clean(tolerance);
   makeCcw();

   CPolyArcLine polyArcLine(*this);
   CArcLinePolygon arcLinePolygon;

   if (polyArcLine.getInline(arcLinePolygon,width,tolerance))
   {
      arcLinePolygon.getPoly(*this);
      retval = true;
   }

   m_widthIndex = zeroWidthIndex;

   return retval;
}

bool CPoly::contains(const CPoly& other,PageUnitsTag pageUnits,double* returnDistance)
{
   bool retval = false;

   CPoly thisPolyCopy(*this);
   thisPolyCopy.close();

   double localDistance;
   double& distance = (returnDistance == NULL ? localDistance : *returnDistance);
   distance = 0.;

   if (thisPolyCopy.list.GetCount() > 2 && other.list.GetCount() > 0)
   {
      thisPolyCopy.vectorize(pageUnits);

      CPoly otherPolyCopy(other);
      otherPolyCopy.vectorize(pageUnits);

      Point2 result1,result2;
      CTMatrix matrix;

      distance = MeasurePolyToPoly(thisPolyCopy,matrix,true,0.,otherPolyCopy,matrix,true,0.,result1,result2);

      if (distance > 0.)
      {
         CPnt* pnt = otherPolyCopy.list.GetHead();

         retval = thisPolyCopy.isPointInside(CPoint2d(pnt->x,pnt->y),pageUnits);
      }
   }

   return retval;
}

CString CPoly::getDescriptor() const
{
   CString descriptor;
   
   descriptor.Format("CPoly,%d",m_widthIndex);

   if (isFilled())
   {
      descriptor += ",filled";
   }

   if (isClosed())
   {
      descriptor += ",closed";
   }

   if (isVoid())
   {
      descriptor += ",void";
   }

   if (isHidden())
   {
      descriptor += ",hidden";
   }

   if (isThermalLine())
   {
      descriptor += ",thermalLine";
   }

   if (isFloodBoundary())
   {
      descriptor += ",floodBoundary";
   }

   if (isBreakOut())
   {
      descriptor += ",breakOut";
   }

   if (isHatchLine())
   {
      descriptor += ",hatchLine";
   }

   for (POSITION pos = list.GetHeadPosition();pos != NULL;)
   {
      CPnt* pnt = list.GetNext(pos);

      if (pnt->bulge == 0.)
      {
         descriptor.AppendFormat(",(%s,%s)",fpfmt(pnt->x),fpfmt(pnt->y));
      }
      else
      {
         descriptor.AppendFormat(",(%s,%s,%s)",fpfmt(pnt->x),fpfmt(pnt->y),fpfmt(pnt->bulge));
      }
   }

   return descriptor;
}

int CPoly::getSignature() const
{
   CCrcCalculator crcCalculator;

   crcCalculator.input(getDescriptor());
   
   int signature = crcCalculator.getCrc();

   return signature;
}

void CPoly::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"CPoly\n"
"{\n"
"   widthIndex=%d\n"
"   Filled=%d\n"
"   Closed=%d\n"
"   VoidPoly=%d\n"
"   Hidden=%d\n"
"   ThermalLines=%d\n"
"   BoundaryPoly=%d\n"
"   Breakout=%d\n"
"   HatchLines=%d\n",
getWidthIndex(),
Filled,
Closed,
VoidPoly,
Hidden,
ThermalLines,
BoundaryPoly,
Breakout,
HatchLines);

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      list.dump(writeFormat,depth);

      writeFormat.popHeader();
   }

   writeFormat.writef("}\n");
}

//_____________________________________________________________________________
CPolyList::CPolyList()
: m_polyList(true)
, m_fillStyle(HS_NOT_SET)
{
}

CPolyList::CPolyList(const CPolyList& other)
{
   *this = other;
}

CPolyList& CPolyList::operator=(const CPolyList& other)
{
   if (&other != this)
   {
      m_polyList.empty();

      this->m_fillStyle = other.m_fillStyle;

      for (POSITION pos = other.m_polyList.GetHeadPosition();pos != NULL;)
      {
         CPoly* poly = other.m_polyList.GetNext(pos);

         if (poly != NULL)
         {
            CPoly* polyCopy = new CPoly(*poly);
            AddTail(polyCopy);
         }
      }
   }

   return *this;
}

POSITION CPolyList::AddHead(CPoly* poly)
{
   POSITION retval = NULL;

   if (poly != NULL)
   {
      retval = m_polyList.AddHead(poly);
   }

   return retval;
}

POSITION CPolyList::AddTail(CPoly* poly)
{
   POSITION retval = NULL;

   if (poly != NULL)
   {
      retval = m_polyList.AddTail(poly);
   }

   return retval;
}

POSITION CPolyList::InsertBefore(POSITION pos,CPoly* poly)
{
   POSITION retval = NULL;

   if (poly != NULL)
   {
      retval = m_polyList.InsertBefore(pos,poly);
   }

   return retval;
}

POSITION CPolyList::InsertAfter(POSITION pos,CPoly* poly)
{
   POSITION retval = NULL;

   if (poly != NULL)
   {
      retval = m_polyList.InsertAfter(pos,poly);
   }

   return retval;
}

CPoly* CPolyList::removeHead()
{
   CPoly* poly = m_polyList.RemoveHead();

   return poly;
}

CPoly* CPolyList::removeTail()
{
   CPoly* poly = m_polyList.RemoveTail();

   return poly;
}

void CPolyList::takeData(CPolyList& other)
{
   for (POSITION pos = other.m_polyList.GetHeadPosition();pos != NULL;)
   {
      POSITION oldPos = pos;
      CPoly* poly = other.m_polyList.GetNext(pos);
      other.m_polyList.RemoveAt(oldPos);

      if (poly != NULL)
      {
         AddTail(poly);
      }
   }
}

void CPolyList::deleteAt(POSITION position)
{
   CPoly* poly = m_polyList.GetAt(position);
   m_polyList.RemoveAt(position);

   delete poly;
}

bool CPolyList::remove(CPoly* poly)
{
   bool retval = false;
   POSITION startPos = NULL;

   while (true)
   {
      POSITION foundPos = m_polyList.Find(poly,startPos);

      if (foundPos == NULL)
      {
         break;
      }

      startPos = foundPos;
      m_polyList.GetNext(startPos);
      m_polyList.RemoveAt(foundPos);

      retval = true;
   }

   return retval;
}

bool CPolyList::isEmpty()
{
   return (m_polyList.IsEmpty() != 0);
}

void CPolyList::transform(const CTMatrix& transformationMatrix,CCamCadData* camCadData)
{
   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly* poly = m_polyList.GetNext(pos);

      if (poly != NULL)
      {
         poly->transform(transformationMatrix,camCadData);
      }
   }
}

void CPolyList::updateExtent(CExtent& extent) const
{
   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly* poly = m_polyList.GetNext(pos);

      if (poly != NULL)
      {
         poly->updateExtent(extent);
      }
   }
}

void CPolyList::updateExtent(CExtent& extent,const CTMatrix& transformationMatrix) const
{
   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly* poly = m_polyList.GetNext(pos);

      if (poly != NULL)
      {
         poly->updateExtent(extent,transformationMatrix);
      }
   }
}

void CPolyList::updateExtent(CExtent& extent,CCamCadData& camCadData) const
{
   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly* poly = m_polyList.GetNext(pos);

      if (poly != NULL)
      {
         poly->updateExtent(extent,camCadData);
      }
   }
}

CExtent CPolyList::getExtent() const
{
   CExtent extent;

   updateExtent(extent);

   return extent;
}

CExtent CPolyList::getExtent(CCamCadData& camCadData) const
{
   CExtent extent;

   updateExtent(extent,camCadData);

   return extent;
}

CExtent CPolyList::getExtent(const CTMatrix& transformationMatrix) const
{
   CExtent extent;

   updateExtent(extent,transformationMatrix);

   return extent;
}

bool CPolyList::isPointInside(const CPoint2d& point,PageUnitsTag pageUnits,double tolerance) const
{
   bool retval = false;

   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL && !retval;)
   {
      CPoly* poly = m_polyList.GetNext(pos);

      if (poly != NULL)
      {
         retval = poly->isPointInside(point,pageUnits,tolerance);
      }
   }

   return retval;
}

bool CPolyList::isPointOnSegment(const CPoint2d& point,PageUnitsTag pageUnits,double tolerance) const
{
   bool retval = false;

   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL && !retval;)
   {
      CPoly* poly = m_polyList.GetNext(pos);

      if (poly != NULL)
      {
         retval = poly->isPointOnSegment(point,pageUnits,tolerance);
      }
   }

   return retval;
}

double CPolyList::distanceTo(const CPoint2d& point,CPoint2d* pointResult) const
{
   double retval = DBL_MAX;

   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly* poly = m_polyList.GetNext(pos);

      if (poly != NULL)
      {
         double distance = poly->distanceTo(point,pointResult);

         if (distance < retval)
         {
            retval = distance;
         }
      }
   }

   return retval;
}

void CPolyList::setWidthIndex(int widthIndex)
{
   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly* poly = m_polyList.GetNext(pos);

      if (poly != NULL)
      {
         poly->setWidthIndex(widthIndex);
      }
   }
}

int CPolyList::getCommonWidthIndex() const
{
   int commonWidthIndex = -1;

   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly* poly = m_polyList.GetNext(pos);
      int widthIndex = poly->getWidthIndex();

      if (commonWidthIndex < 0)
      {
         commonWidthIndex = widthIndex;
      }
      else if (commonWidthIndex != widthIndex)
      {
         commonWidthIndex = -1;
         break;
      }
   }

   return commonWidthIndex;
}


bool CPolyList::HasNonZeroWidthPoly(const CCamCadData& camCadData) const
{
   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly* poly = m_polyList.GetNext(pos);      
      if(poly->getWidthIndex() >= 0)
         if(camCadData.getWidth(poly->getWidthIndex()) > 0.0)
            return true;
   }
   return false;
}

bool CPolyList::closePolys()
{
   bool retval = false;

   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly* poly = m_polyList.GetNext(pos);

      if (poly != NULL)
      {
         retval = (poly->close() || retval);
      }
   }

   return retval;
}

bool CPolyList::hasPoints(int atleastThisManyPnts)
{
   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly* poly = m_polyList.GetNext(pos);

      if (poly != NULL && poly->getPntList().GetCount() >= atleastThisManyPnts)
      {
         return true;
      }
   }

   return false;
}

int CPolyList::getClosedPolyCount()
{
   int count = 0;

   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly* poly = m_polyList.GetNext(pos);

      if (poly != NULL)
      {
         if (poly->isClosed())
            count++;
      }
   }

   return count;
}

CPoint2d CPolyList::getCenterOfMass(PageUnitsTag pageUnits) const
{
   CPoint2d centerOfMass;
   int polyCount = 0;

   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly* poly = m_polyList.GetNext(pos);

      if (poly != NULL)
      {
         CPoly polyCopy(*poly);
         polyCopy.vectorize(pageUnits);

         CPolygon polygon(pageUnits);
         polygon.setVertices(polyCopy);

         centerOfMass = centerOfMass + polygon.getCenterOfMass();

         polyCount++;
      }
   }

   if (polyCount != 0)
   {
      centerOfMass.x /= polyCount;
      centerOfMass.y /= polyCount;
   }

   return centerOfMass;
}

CPoint2d CPolyList::getCenterOfVertices(PageUnitsTag pageUnits) const
{
   CPoint2d centerOfVertices;
   int polyCount = 0;

   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly* poly = m_polyList.GetNext(pos);

      if (poly != NULL)
      {
         CPoly polyCopy(*poly);
         polyCopy.vectorize(pageUnits);

         CPolygon polygon(pageUnits);
         polygon.setVertices(polyCopy);

         centerOfVertices = centerOfVertices + polygon.getCenterOfVertices();

         polyCount++;
      }
   }

   if (polyCount != 0)
   {
      centerOfVertices.x /= polyCount;
      centerOfVertices.y /= polyCount;
   }

   return centerOfVertices;
}

CPoly* CPolyList::getMaximumAreaClosedPoly(PageUnitsTag pageUnits) const
{
   CPoly* maxPoly = NULL;
   double maxArea = 0.;

   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly* poly = m_polyList.GetNext(pos);

      if (poly->isClosed())
      {
         CPolygon convexPolygon(*poly,pageUnits);

         double area = fabs(poly->getArea());

         if (area > maxArea)
         {
            maxArea = area;
            maxPoly = poly;
         }
      }
   }

   return maxPoly;
}

//bool CPolyList::getMaximumAreaConvexPolygon(CPolygon& polygon) const
//{
//   double maxArea = 0.;
//
//   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
//   {
//      CPoly* poly = m_polyList.GetNext(pos);
//
//      if (poly != NULL)
//      {
//         CPolygon convexPolygon(*poly);
//         convexPolygon.makeConvex();
//
//         double area = convexPolygon.getArea();
//
//         if (area > maxArea)
//         {
//            maxArea = area;
//            polygon = convexPolygon;
//         }
//      }
//   }
//
//   return (maxArea > 0.);
//}

//bool CPolyList::getMaximumInCircle(CPoint2d& inCircleCenter,double& inCircleRadius,double tolerance) const
//{
//   bool retval = false;
//   inCircleRadius = 0.;
//   double radius;
//   CPoint2d center;
//
//   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
//   {
//      CPoly* poly = m_polyList.GetNext(pos);
//      CPolygon polygon(*poly);
//
//      //addDebugPolygon(*(getActiveDocument()),polygon,"getMaximumInCircle()");
//
//      if (polygon.getInCircleCenter(center,radius,tolerance))
//      {
//         if (radius > inCircleRadius)
//         {
//            inCircleRadius = radius;
//            inCircleCenter = center;
//            retval = true;
//         }
//      }
//   }
//
//   return retval;
//}

//bool CPolyList::getMaximumInCircleWithHoleAvoidance(CPoint2d& inCircleCenter,double& inCircleRadius,
//   CPoint2d holeCenter,double holeRadius,double tolerance) const
//{
//   bool retval = false;
//   inCircleRadius = 0.;
//   double radius;
//   CPoint2d center;
//
//   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
//   {
//      CPoly* poly = m_polyList.GetNext(pos);
//      CPolygon polygon(*poly);
//
//      CDebug::addDebugPolygon(*(getActiveDocument()),polygon,"getMaximumInCircleWithHoleAvoidance()");
//
//      if (polygon.getInCircleCenterWithHoleAvoidance(center,radius,tolerance,holeCenter,holeRadius))
//      {
//         if (radius > inCircleRadius)
//         {
//            inCircleRadius = radius;
//            inCircleCenter = center;
//            retval = true;
//         }
//      }
//   }
//
//   return retval;
//}

void CPolyList::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"PolyList\n"
"{\n"
"   Count=%d\n",
GetCount());

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
      {
         CPoly* poly = m_polyList.GetNext(pos);

         if (poly != NULL)
         {
            poly->dump(writeFormat,depth);
         }
      }

      writeFormat.popHeader();
   }

   writeFormat.writef("}\n");
}

//_____________________________________________________________________________
CPoly3D::CPoly3D()
{
   init();
}

CPoly3D::CPoly3D(const CPoly3D& other) : 
   list(other.list)
{
   setWidthIndex(other.getWidthIndex());

   Filled       = other.Filled;
   Closed       = other.Closed;
   VoidPoly     = other.VoidPoly;
   Hidden       = other.Hidden;
   ThermalLines = other.ThermalLines;
   BoundaryPoly = other.BoundaryPoly;
   Breakout     = other.Breakout;
   HatchLines   = other.HatchLines;
}

CPoly3D::CPoly3D(const CPnt3DList& pntList) : 
   list(pntList)
{
   init();
}

CPoly3D::~CPoly3D()
{
}

void CPoly3D::init()
{
   setWidthIndex(0);
   Filled       = 0;
   Closed       = 0;
   VoidPoly     = 0;
   Hidden       = 0;
   ThermalLines = 0;
   BoundaryPoly = 0;
   Breakout     = 0;
   HatchLines   = 0;
}

void CPoly3D::empty()
{
   list.empty();
}

void CPoly3D::setWidthIndex(int index)
{ 
   m_widthIndex = index; 

   if (index > 400)
   {
      int iii = 3;
   }
}

CPnt3D* CPoly3D::addVertex(double x,double y,double z)
{

   CPnt3D* pnt = new CPnt3D(x,y,z);

   list.AddTail(pnt);

   return pnt;
}

CPnt3D* CPoly3D::addVertex(const CPoint3d& point)
{
   return addVertex(point.x,point.y, point.z);
}


void CPoly3D::transform(const CTMatrix& transformationMatrix,CCamCadData* camCadData)
{
   list.transform(transformationMatrix);

   if (camCadData != NULL)
   {
      BlockStruct& widthBlock = camCadData->getDefinedWidthBlock(getWidthIndex());

      CBasesVector basesVector(0.,0.,1.,radiansToDegrees(widthBlock.getRotation()),false);
      basesVector.transform(transformationMatrix);

      double scale = basesVector.getScale();

      if (!fpeq(scale,1.0))
      {
         int newWidthIndex = camCadData->getDefinedApertureIndex("",widthBlock.getShape(),
            widthBlock.getSizeA()   * scale,widthBlock.getSizeB()   * scale,
            widthBlock.getXoffset() * scale,widthBlock.getYoffset() * scale,
            basesVector.getRotationRadians());

         setWidthIndex(newWidthIndex);
      }
   }
}

void CPoly3D::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
      "CPoly3D\n"
      "{\n"
      "   widthIndex=%d\n"
      "   Filled=%d\n"
      "   Closed=%d\n"
      "   VoidPoly=%d\n"
      "   Hidden=%d\n"
      "   ThermalLines=%d\n"
      "   BoundaryPoly=%d\n"
      "   Breakout=%d\n"
      "   HatchLines=%d\n",
      getWidthIndex(),
      Filled,
      Closed,
      VoidPoly,
      Hidden,
      ThermalLines,
      BoundaryPoly,
      Breakout,
      HatchLines);

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      list.dump(writeFormat,depth);

      writeFormat.popHeader();
   }

   writeFormat.writef("}\n");
}


//_____________________________________________________________________________
CPoly3DList::CPoly3DList()
: m_polyList(true)
, m_fillStyle(HS_NOT_SET)
{
}

CPoly3DList::CPoly3DList(const CPoly3DList& other)
{
   *this = other;
}

CPoly3DList& CPoly3DList::operator=(const CPoly3DList& other)
{
   if (&other != this)
   {
      m_polyList.empty();

      this->m_fillStyle = other.m_fillStyle;

      for (POSITION pos = other.m_polyList.GetHeadPosition();pos != NULL;)
      {
         CPoly3D* poly = other.m_polyList.GetNext(pos);

         if (poly != NULL)
         {
            CPoly3D* polyCopy = new CPoly3D(*poly);
            AddTail(polyCopy);
         }
      }
   }

   return *this;
}

POSITION CPoly3DList::AddHead(CPoly3D* poly)
{
   POSITION retval = NULL;

   if (poly != NULL)
   {
      retval = m_polyList.AddHead(poly);
   }

   return retval;
}

POSITION CPoly3DList::AddTail(CPoly3D* poly)
{
   POSITION retval = NULL;

   if (poly != NULL)
   {
      retval = m_polyList.AddTail(poly);
   }

   return retval;
}

POSITION CPoly3DList::InsertBefore(POSITION pos,CPoly3D* poly)
{
   POSITION retval = NULL;

   if (poly != NULL)
   {
      retval = m_polyList.InsertBefore(pos,poly);
   }

   return retval;
}

POSITION CPoly3DList::InsertAfter(POSITION pos,CPoly3D* poly)
{
   POSITION retval = NULL;

   if (poly != NULL)
   {
      retval = m_polyList.InsertAfter(pos,poly);
   }

   return retval;
}

CPoly3D* CPoly3DList::removeHead()
{
   CPoly3D* poly = m_polyList.RemoveHead();

   return poly;
}

CPoly3D* CPoly3DList::removeTail()
{
   CPoly3D* poly = m_polyList.RemoveTail();

   return poly;
}

void CPoly3DList::takeData(CPoly3DList& other)
{
   for (POSITION pos = other.m_polyList.GetHeadPosition();pos != NULL;)
   {
      POSITION oldPos = pos;
      CPoly3D* poly = other.m_polyList.GetNext(pos);
      other.m_polyList.RemoveAt(oldPos);

      if (poly != NULL)
      {
         AddTail(poly);
      }
   }
}

void CPoly3DList::deleteAt(POSITION position)
{
   CPoly3D* poly = m_polyList.GetAt(position);
   m_polyList.RemoveAt(position);

   delete poly;
}

bool CPoly3DList::remove(CPoly3D* poly)
{
   bool retval = false;
   POSITION startPos = NULL;

   while (true)
   {
      POSITION foundPos = m_polyList.Find(poly,startPos);

      if (foundPos == NULL)
      {
         break;
      }

      startPos = foundPos;
      m_polyList.GetNext(startPos);
      m_polyList.RemoveAt(foundPos);

      retval = true;
   }

   return retval;
}

bool CPoly3DList::isEmpty()
{
   return (m_polyList.IsEmpty() != 0);
}

void CPoly3DList::transform(const CTMatrix& transformationMatrix,CCamCadData* camCadData)
{
   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly3D* poly = m_polyList.GetNext(pos);

      if (poly != NULL)
      {
         poly->transform(transformationMatrix,camCadData);
      }
   }
}

void CPoly3DList::setWidthIndex(int widthIndex)
{
   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly3D* poly = m_polyList.GetNext(pos);

      if (poly != NULL)
      {
         poly->setWidthIndex(widthIndex);
      }
   }
}

int CPoly3DList::getCommonWidthIndex() const
{
   int commonWidthIndex = -1;

   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly3D* poly = m_polyList.GetNext(pos);
      int widthIndex = poly->getWidthIndex();

      if (commonWidthIndex < 0)
      {
         commonWidthIndex = widthIndex;
      }
      else if (commonWidthIndex != widthIndex)
      {
         commonWidthIndex = -1;
         break;
      }
   }

   return commonWidthIndex;
}


bool CPoly3DList::HasNonZeroWidthPoly(const CCamCadData& camCadData) const
{
   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly3D* poly = m_polyList.GetNext(pos);      
      if(poly->getWidthIndex() >= 0)
         if(camCadData.getWidth(poly->getWidthIndex()) > 0.0)
            return true;
   }
   return false;
}


bool CPoly3DList::hasPoints()
{
   for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly3D* poly = m_polyList.GetNext(pos);

      if (poly != NULL && poly->getPntList().GetCount() > 0)
      {
         return true;
      }
   }

   return false;
}

void CPoly3DList::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
      "Poly3DList\n"
      "{\n"
      "   Count=%d\n",
      GetCount());

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      for (POSITION pos = m_polyList.GetHeadPosition();pos != NULL;)
      {
         CPoly3D* poly = m_polyList.GetNext(pos);

         if (poly != NULL)
         {
            poly->dump(writeFormat,depth);
         }
      }

      writeFormat.popHeader();
   }

   writeFormat.writef("}\n");
}