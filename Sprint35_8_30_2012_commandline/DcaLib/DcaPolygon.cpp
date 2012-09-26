// $Header: /CAMCAD/DcaLib/DcaPolygon.cpp 5     6/21/07 8:30p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaPolygon.h"
#include "DcaTMatrix.h"
#include "DcaLine2d.h"
#include "DcaPoly.h"
#include "DcaLib.h"
#include "DcaWriteFormat.h"

//_____________________________________________________________________________
CPolygonPointList& CPolygonPointList::operator=(const CPolygonPointList& other)
{
   if (&other != this)
   {
      m_pointList.empty();

      for (POSITION pos = other.m_pointList.GetHeadPosition();pos != NULL;)
      {
         addPoint(*(other.m_pointList.GetNext(pos)));
      }
   }

   return *this;
}

int CPolygonPointList::addPoint(const CPoint2d& point)
{
   m_pointList.AddTail(new CPoint2d(point));

   return m_pointList.GetCount();
}

void CPolygonPointList::reverse()
{
   if (m_pointList.GetCount() > 1)
   {
      POSITION initialHeadPos = m_pointList.GetHeadPosition();

      do
      {
         m_pointList.InsertBefore(initialHeadPos,m_pointList.RemoveTail());
      }
      while (m_pointList.GetTailPosition() != initialHeadPos);
   }
}

void CPolygonPointList::transform(const CTMatrix& matrix)
{
   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CPoint2d* point = GetNext(pos);
      matrix.transform(*point);
   }
}

//_____________________________________________________________________________
CPolygonEdgeIterator::CPolygonEdgeIterator(const CPolygonPointList& list) :
   m_list(list)
{
   rewind();
}

bool CPolygonEdgeIterator::rewind()
{
   m_index = -1;
   m_pos = m_list.m_pointList.GetHeadPosition();

   if (m_pos != NULL)
   {
      m_currentPoint = *(m_list.m_pointList.GetNext(m_pos));
      m_firstPoint   = m_currentPoint;
   }

   m_atEnd = (m_pos == NULL);

   return !m_atEnd;
}

bool CPolygonEdgeIterator::getNext(CLine2d& line)
{
   bool retval = true;

   if (m_pos != NULL)
   {
      CPoint2d lastPoint = m_currentPoint;
      m_currentPoint = *(m_list.m_pointList.GetNext(m_pos));
      line.set(lastPoint,m_currentPoint);

      m_index++;
   }
   else if (!m_atEnd)
   {
      line.set(m_currentPoint,m_firstPoint);
      m_atEnd = true;

      m_index++;
   }
   else
   {
      retval = false;
   }

   return retval;
}

//_____________________________________________________________________________
CPolygon::CPolygon(PageUnitsTag pageUnits)
: m_pageUnits(pageUnits)
{
   resetState();
}

CPolygon::CPolygon(const CPolygon& other)
{
   *this = other;
}

CPolygon::CPolygon(const CPoly& poly,PageUnitsTag pageUnits)
: m_pageUnits(pageUnits)
{
   resetState();
   CPoly polyCopy(poly);
   polyCopy.close();
   polyCopy.vectorize(getPageUnits());
   polyCopy.removeDuplicateVertices();

   setVertices(polyCopy);
}

CPolygon& CPolygon::operator=(const CPolygon& other)
{
   if (&other != this)
   {
      m_pointList     = other.m_pointList;
      m_extent        = other.m_extent;
      m_isValid       = other.m_isValid;
      m_isConvex      = other.m_isConvex;
      m_isCcw         = other.m_isCcw;
      m_isNormalized  = other.m_isNormalized;
   }

   return *this;
}

void CPolygon::resetState()
{
   m_isValid      = boolUnknown;
   m_isConvex     = boolUnknown;
   m_isCcw        = boolUnknown;
   m_isNormalized = false;

   m_extent.reset();
}

bool CPolygon::normalizedFpeq(const CPolygon& other,double tolerance) const
{
   bool retval = isNormalized() && other.isNormalized();

   POSITION pos      = m_pointList.GetHeadPosition();
   POSITION otherPos = other.m_pointList.GetHeadPosition();

   while (retval)
   {
      if (pos == NULL || otherPos == NULL)
      {
         retval = (pos == NULL && otherPos == NULL);
         break;
      }

      CPoint2d* point      = m_pointList.GetNext(pos);
      CPoint2d* otherPoint = other.m_pointList.GetNext(otherPos);

      retval = point->fpeq(*otherPoint,tolerance);
   }

   return retval;
}

bool CPolygon::normalizedFpeq(const CPolygon& other) const
{
   bool retval = isNormalized() && other.isNormalized();

   POSITION pos      = m_pointList.GetHeadPosition();
   POSITION otherPos = other.m_pointList.GetHeadPosition();

   while (retval)
   {
      if (pos == NULL || otherPos == NULL)
      {
         retval = (pos == NULL && otherPos == NULL);
         break;
      }

      CPoint2d* point      = m_pointList.GetNext(pos);
      CPoint2d* otherPoint = other.m_pointList.GetNext(otherPos);

      retval = point->fpeq(*otherPoint);
   }

   return retval;
}

bool CPolygon::normalizedVerticesEqual(const CPolygon& other,double toleranceDistance) const
{
   bool retval = isNormalized() && other.isNormalized();

   if (retval)
   {
      double toleranceDistanceSquared = toleranceDistance * toleranceDistance;

      POSITION pos      = m_pointList.GetHeadPosition();
      POSITION otherPos = other.m_pointList.GetHeadPosition();

      while (retval)
      {
         if (pos == NULL || otherPos == NULL)
         {
            retval = (pos == NULL && otherPos == NULL);
            break;
         }

         CPoint2d* point      = m_pointList.GetNext(pos);
         CPoint2d* otherPoint = other.m_pointList.GetNext(otherPos);

         double distanceSquared = point->distanceSqr(*otherPoint);

         retval = (distanceSquared <= toleranceDistanceSquared);
      }
   }

   return retval;
}

void CPolygon::empty()
{
   resetState();

   m_pointList.empty();
}

int CPolygon::addVertex(const CPoint2d& point)
{
   resetState();

   return m_pointList.addPoint(point);
}

//#if defined(UseLedaLibraryOld)  //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//int leda::rat_point::default_precision = 0;
//
//leda::memory_manager leda::std_memory_mgr;
//
//leda::LedaErrorHandler leda::std_error_handler;
//#endif

void CPolygon::setVertices(const CPoly& poly)
{
   empty();
   int pointCount = poly.getPntList().GetCount();

   if (poly.isClosed())
   {
      pointCount--;
   }

   int index = 0;

   for (POSITION pos = poly.getPntList().GetHeadPosition();pos != NULL && index < pointCount;index++)
   {
      CPnt* pnt = poly.getPntList().GetNext(pos);
      addVertex(CPoint2d(pnt->x,pnt->y));
   }
}

#if defined(UseLedaLibraryOld)  //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void getLedaPointList(leda::list<leda::POINT>& ledaPointList,CPolygonPointList& pointList)
{
   for (POSITION pos = pointList.GetHeadPosition();pos != NULL;)
   {
      CPoint2d* point2d = pointList.GetNext(pos);

      ledaPointList.append(leda::POINT(point2d->x,point2d->y));
   }
}

#endif  //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

#if defined(UseAlgoCOMsPolygonLibrary)  //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

IACPolygonPtr createAlgoComPolygon(CPolygonPointList& pointList)
{
   IACGenPolygonPtr algoComPolygon(__uuidof(AlgoCOMsGenPolygon));

   int ordinateCount = 2*pointList.GetCount();

   LPSAFEARRAY coords_SA = 0;
   algoComPolygon->MakeArrayOfDoubles(ordinateCount,&coords_SA);
   long index = 0;

   for (POSITION pos = pointList.GetHeadPosition();pos != NULL;)
   {
      CPoint2d* point2d = pointList.GetNext(pos);

      SafeArrayPutElement(coords_SA,&index,(void*)(&(point2d->x)));
      index++;

      SafeArrayPutElement(coords_SA,&index,(void*)(&(point2d->y)));
      index++;
   }

   algoComPolygon->CreateFromVerticesDouble(coords_SA);
   SafeArrayDestroy(coords_SA);

   return algoComPolygon;
}

#endif  //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

bool CPolygon::merge(CPolygon& other)
{
   bool retval = false;

#if defined(UseLedaLibraryOld) //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

   leda::list<leda::POINT> thisPointList,otherPointList;

   getLedaPointList(thisPointList,m_pointList);
   getLedaPointList(otherPointList,other.m_pointList);

   try
   {
      leda::GEN_POLYGON thisPolygon(thisPointList);
      leda::GEN_POLYGON otherPolygon(otherPointList);
      leda::GEN_POLYGON mergedPolygon = thisPolygon.unite(otherPolygon);

      if (mergedPolygon.is_simple())
      {
         retval = true;

         m_pointList.empty();

         leda::list<leda::POINT> mergedPointList = mergedPolygon.vertices();

         leda::list<leda::POINT>::iterator it;

         for ( it = mergedPointList.begin();it != mergedPointList.end();it++ )
         {
            leda::POINT ledaPOINT = *it;
            leda::point ledaPoint = ledaPOINT.to_float();
            m_pointList.addPoint(CPoint2d(ledaPoint.xcoord(),ledaPoint.ycoord()));
         }
      }
   }
   catch (...)
   {
      int iii = 3;
   }

#elif defined(UseAlgoCOMsPolygonLibrary)  //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

   try
   {
      IACGenPolygonPtr thisAlgoComPolygon  = createAlgoComPolygon(m_pointList);
      IACGenPolygonPtr otherAlgoComPolygon = createAlgoComPolygon(other.m_pointList);

      // Compute R as the union of P and Q
      IACGenPolygonPtr mergedAlgoComPolygon(__uuidof(AlgoCOMsGenPolygon));
      mergedAlgoComPolygon->Union(thisAlgoComPolygon,otherAlgoComPolygon);

      // Display the polygonal chains of R
      if (mergedAlgoComPolygon->GetNumberOfChains() == 1)
      {
         m_pointList.empty();

         LPSAFEARRAY R_polys = 0;
         mergedAlgoComPolygon->GetBoundaryRep(&R_polys);

         long c_idx = 0;

         IACPolygonPtr R_chain;
         SafeArrayGetElement(R_polys, &c_idx, &R_chain);

         LPSAFEARRAY xcoords = 0, ycoords = 0;
         R_chain->GetCoords(&xcoords, &ycoords);

         long ub;
         SafeArrayGetUBound(xcoords, 1, &ub);
         long v_idx;

         for (v_idx = 0; v_idx <= ub; ++v_idx) 
         {
            IACRationalPtr x,y;

            SafeArrayGetElement(xcoords, &v_idx, &x);
            SafeArrayGetElement(ycoords, &v_idx, &y);

            m_pointList.addPoint(CPoint2d(x->Approximate(),y->Approximate()));
         }

         SafeArrayDestroy(xcoords); 
         SafeArrayDestroy(ycoords);
         SafeArrayDestroy(R_polys);

         retval = true;
      }
   }
   catch(...)
   {
      int iii = 3;
   }

#else  //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

   CPolygonSubdivision polygonSubdivision(*this,other);
   CPolygon mergedPolygon(getPageUnits());

   retval = polygonSubdivision.merge(mergedPolygon);

   if (retval)
   {
      if (mergedPolygon.isValid())
      {
         *this = mergedPolygon;
      }
      else
      {
         int iii = 3;
      }
   }

#endif  //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

   return retval;
}

bool CPolygon::isInside(const CPoint2d& point) const
{
   bool retval = false;

   if (isValid() && getExtent().isOnOrInside(point))
   {
      int intersectionCount = 0;
      CSize2d extentSize = m_extent.getSize();

      double r = 2.*(extentSize.cx > extentSize.cy ? extentSize.cx : extentSize.cy);
      CPoint2d exteriorPoint(point);
      exteriorPoint.x += r;

      CLine2d testLine(point,exteriorPoint);

      POSITION headPos  = m_pointList.GetHeadPosition();
      POSITION startPos;
      CPoint2d* point;
      CLine2d edgeLine;
      CPoint2d lastPointNotOnTestLine;

      for (startPos = headPos;startPos != NULL;)
      {
         point = m_pointList.GetNext(startPos);

         if (testLine.ccw(*point) != 0)
         {
            lastPointNotOnTestLine = *point;
            break;
         }
      }

      for (POSITION pos = startPos;;)
      {
         CPoint2d* point = m_pointList.GetNext(pos);
         edgeLine.set0(*point);
         edgeLine.set1(*point);

         if (! testLine.intersects(edgeLine))
         {
            edgeLine.set0(lastPointNotOnTestLine);
            lastPointNotOnTestLine = *point;

            if (testLine.intersects(edgeLine))
            {
               intersectionCount++;
            }
         }

         if (pos == startPos)
         {
            break;
         }
         else if (pos == NULL)
         {
            pos = headPos;
         }
      }

      retval = (intersectionCount%2 != 0);
   }

   return retval;
}

bool CPolygon::intersects(const CPolygon& other) const
{
   bool retval = false;

   if (isValid() && other.isValid())
   {
      if (getExtent().intersects(other.getExtent()))
      {
         CPolygonEdgeIterator thisPos(m_pointList);
         CPolygonEdgeIterator otherPos(other.m_pointList);
         CLine2d thisEdge,otherEdge;

         while (!retval && thisPos.getNext(thisEdge))
         {
            otherPos.rewind();

            while (otherPos.getNext(otherEdge))
            {
               if (thisEdge.intersects(otherEdge))
               {
                  retval = true;
                  break;
               }
            }
         }

         //if (!retval)
         //{
         //   if (areAllPointsInside(other))
         //   {
         //      retval = true;
         //   }
         //   else if (other.areAllPointsInside(*this))
         //   {
         //      retval = true;
         //   }
         //}
      }
   }

   return retval;
}

bool CPolygon::selfIntersects() const
{
   bool retval = false;

   int pointCount = m_pointList.GetCount();

   if (pointCount > 3)
   {
      CPolygonEdgeIterator thisPos(m_pointList);
      CPolygonEdgeIterator otherPos(m_pointList);
      CLine2d thisEdge,otherEdge;

      while (!retval && thisPos.getNext(thisEdge))
      {
         otherPos.rewind();

         while (otherPos.getNext(otherEdge))
         {
            if ((otherPos.getIndex() ==   thisPos.getIndex()) ||
                (otherPos.getIndex() == ((thisPos.getIndex()  + 1)%pointCount)) ||
                (thisPos.getIndex()  == ((otherPos.getIndex() + 1)%pointCount)) )
            {
               continue;
            }

            if (thisEdge.intersects(otherEdge))
            {
               retval = true;
               break;
            }
         }
      }
   }

   return retval;
}

bool CPolygon::areAllPointsInside(const CPolygon& other) const
{
   bool retval = true;

   for (POSITION pos = m_pointList.GetHeadPosition();pos != NULL;)
   {
      CPoint2d* point = m_pointList.GetNext(pos);

      if (!other.isInside(*point))
      {
         retval = false;
         break;
      }
   }

   return retval;
}

bool CPolygon::isOutsideAndCloserThanDistanceSquared(const CPoint2d& point,double distanceSquared) const
{
   bool retval = isInside(point);

   if (!retval)
   {
      CPolygonEdgeIterator pos(m_pointList);
      CLine2d edge;

      while (pos.getNext(edge))
      {
         double edgeDistanceSquared = edge.distanceToLineSquared(point);

         if (edgeDistanceSquared < distanceSquared)
         {
            retval = true;
            break;
         }
      }
   }

   return retval;
}

bool CPolygon::isInsideAndCloserThanDistanceSquared(const CPoint2d& point,double distanceSquared) const
{
   bool retval = false;

   if (isInside(point))
   {
      CPolygonEdgeIterator pos(m_pointList);
      CLine2d edge;

      while (pos.getNext(edge))
      {
         double edgeDistanceSquared = edge.distanceToLineSquared(point);

         if (edgeDistanceSquared < distanceSquared)
         {
            retval = true;
            break;
         }
      }
   }

   return retval;
}

bool CPolygon::isInsideAndCloserThanDistanceSquared(const CLine2d& line,double distanceSquared) const
{
   bool retval = isInsideAndCloserThanDistanceSquared(line.get0(),distanceSquared) &&
                 isInsideAndCloserThanDistanceSquared(line.get1(),distanceSquared);

   return retval;
}

double CPolygon::distanceTo(const CPoint2d& point) const
{
   double distanceSquared = DBL_MAX;

   CPolygonEdgeIterator pos(m_pointList);
   CLine2d edge;

   while (pos.getNext(edge))
   {
      double edgeDistanceSquared = edge.distanceToLineSquared(point);

      if (edgeDistanceSquared < distanceSquared)
      {
         distanceSquared = edgeDistanceSquared;
      }
   }

   double distance = sqrt(distanceSquared);

   if (distance > 0. && isInside(point))
   {
      distance = -distance;
   }

   return distance;
}

double CPolygon::getArea2() const
{
   double area2 = 0.;

   if (m_pointList.GetCount() > 2)
   {
      CPoint2d* previousPoint = m_pointList.GetTail();

      for (POSITION pos = m_pointList.GetHeadPosition();pos != NULL;)
      {
         CPoint2d* point = m_pointList.GetNext(pos);

         area2 += ((previousPoint->x + point->x)*(point->y - previousPoint->y));

         previousPoint = point;
      }
   }

   return area2;
}

void CPolygon::getLeftRightAreasFromPoint(double& leftArea,double& rightArea,const CPoint2d& refPoint,bool debugFlag) const
{
   leftArea = rightArea = 0.;
   CPoint2d  previousLeftSplitPoint,previousRightSplitPoint,splitPoint;
   bool previousLeftSplitPointFlag  = false;
   bool previousRightSplitPointFlag = false;

#ifdef debugCPolygon_getLeftRightAreasFromPoint
   if (debugFlag)
   {
CDebugWriteFormat::getWriteFormat().writef(
"\nCPolygon::getLeftRightAreasFromPoint() ______________________________________________________________________________\nrefPoint=%s\n",
refPoint.getString());
   }
#endif

   if (m_pointList.GetCount() > 2)
   {
      CPoint2d* previousPoint = m_pointList.GetTail();

      int index = 0;

      for (POSITION pos = m_pointList.GetHeadPosition();pos != NULL;index++)
      {
         CPoint2d* point = m_pointList.GetNext(pos);

         bool leftFlag     = (point->x         <= refPoint.x);
         bool prevLeftFlag = (previousPoint->x <= refPoint.x);
         bool splitFlag = (leftFlag != prevLeftFlag);

         if (splitFlag)
         {
            double m = (point->y - previousPoint->y) / (point->x - previousPoint->x);
            double b = previousPoint->y - (m * previousPoint->x);

            splitPoint.x = refPoint.x;
            splitPoint.y = m*refPoint.x + b;
            
            if (leftFlag)
            {
               // closing right area, opening left area

               // close right area
               if (previousRightSplitPointFlag)
               {
#ifdef debugCPolygon_getLeftRightAreasFromPoint
                  if (debugFlag)
                  {
CDebugWriteFormat::getWriteFormat().writef(
"Right(close): index=%2d, previousPoint=%s, splitPoint=%s, rightArea=%s\n",
index,previousPoint->getString(),splitPoint.getString(),
fpfmt(rightArea,6));
                  }
#endif

                  rightArea += ((previousPoint->x + splitPoint.x)*(splitPoint.y - previousPoint->y));

#ifdef debugCPolygon_getLeftRightAreasFromPoint
                  if (debugFlag)
                  {
CDebugWriteFormat::getWriteFormat().writef(
"Right(close): index=%2d, splitPoint=%s, previousRightSplitPoint=%s, rightArea=%s\n",
index,splitPoint.getString(),previousRightSplitPoint.getString(),
fpfmt(rightArea,6));
                  }
#endif

                  rightArea += ((splitPoint.x + previousRightSplitPoint.x)*(previousRightSplitPoint.y - splitPoint.y));
                  previousRightSplitPointFlag = false;
               }
               else
               {
#ifdef debugCPolygon_getLeftRightAreasFromPoint
                  if (debugFlag)
                  {
CDebugWriteFormat::getWriteFormat().writef(
"Right(suspend): index=%2d, previousPoint=%s, splitPoint=%s, rightArea=%s\n",
index,previousPoint->getString(),splitPoint.getString(),
fpfmt(rightArea,6));
                  }
#endif

                  rightArea += ((previousPoint->x + splitPoint.x)*(splitPoint.y - previousPoint->y));
                  previousRightSplitPoint = splitPoint;
                  previousRightSplitPointFlag = true;
               }

               // open left area
               if (previousLeftSplitPointFlag)
               {
#ifdef debugCPolygon_getLeftRightAreasFromPoint
                  if (debugFlag)
                  {
CDebugWriteFormat::getWriteFormat().writef(
"Left(resume):  index=%2d, previousLeftSplitPoint =%s, splitPoint=%s, leftArea =%s\n",
index,previousLeftSplitPoint.getString(),splitPoint.getString(),
fpfmt(leftArea,6));
                  }
#endif

                  leftArea += ((previousLeftSplitPoint.x + splitPoint.x)*(splitPoint.y - previousLeftSplitPoint.y));

#ifdef debugCPolygon_getLeftRightAreasFromPoint
                  if (debugFlag)
                  {
CDebugWriteFormat::getWriteFormat().writef(
"Left(resume):  index=%2d, splitPoint=%s, point=%s, leftArea =%s\n",
index,splitPoint.getString(),point->getString(),
fpfmt(leftArea,6));
                  }
#endif

                  leftArea += ((splitPoint.x + point->x)*(point->y - splitPoint.y));

                  previousLeftSplitPointFlag = false;
               }
               else
               {
#ifdef debugCPolygon_getLeftRightAreasFromPoint
                  if (debugFlag)
                  {
CDebugWriteFormat::getWriteFormat().writef(
"Left(open):  index=%2d, splitPoint=%s, point=%s, leftArea =%s\n",
index,splitPoint.getString(),point->getString(),
fpfmt(leftArea,6));
                  }
#endif

                  leftArea += ((splitPoint.x + point->x)*(point->y - splitPoint.y));
                  previousLeftSplitPoint      = splitPoint;
                  previousLeftSplitPointFlag  = true;
               }
            }
            else
            {
               // closing left area, opening right area

               // close left area
               if (previousLeftSplitPointFlag)
               {
#ifdef debugCPolygon_getLeftRightAreasFromPoint
                  if (debugFlag)
                  {
CDebugWriteFormat::getWriteFormat().writef(
"Left(close):  index=%2d, previousPoint =%s, splitPoint=%s, leftArea =%s\n",
index,previousPoint->getString(),splitPoint.getString(),
fpfmt(leftArea,6));
                  }
#endif

                  leftArea += ((previousPoint->x + splitPoint.x)*(splitPoint.y - previousPoint->y));

#ifdef debugCPolygon_getLeftRightAreasFromPoint
                  if (debugFlag)
                  {
CDebugWriteFormat::getWriteFormat().writef(
"Left(close):  index=%2d, splitPoint=%s, previousLeftSplitPoint =%s, leftArea =%s\n",
index,splitPoint.getString(),previousLeftSplitPoint.getString(),
fpfmt(leftArea,6));
                  }
#endif

                  leftArea += ((splitPoint.x + previousLeftSplitPoint.x)*(previousLeftSplitPoint.y - splitPoint.y));
                  previousLeftSplitPointFlag = false;
               }
               else
               {
#ifdef debugCPolygon_getLeftRightAreasFromPoint
                  if (debugFlag)
                  {
CDebugWriteFormat::getWriteFormat().writef(
"Left(suspend):  index=%2d, previousPoint =%s, splitPoint=%s, leftArea =%s\n",
index,previousPoint->getString(),splitPoint.getString(),
fpfmt(leftArea,6));
                  }
#endif

                  leftArea += ((previousPoint->x + splitPoint.x)*(splitPoint.y - previousPoint->y));
                  previousLeftSplitPoint = splitPoint;
                  previousLeftSplitPointFlag = true;
               }

               // open right area
               if (previousRightSplitPointFlag)
               {
#ifdef debugCPolygon_getLeftRightAreasFromPoint
                  if (debugFlag)
                  {
CDebugWriteFormat::getWriteFormat().writef(
"Right(resume): index=%2d, previousRightSplitPoint=%s, splitPoint=%s, rightArea=%s\n",
index,previousRightSplitPoint.getString(),splitPoint.getString(),
fpfmt(rightArea,6));
                  }
#endif

                  rightArea += ((previousRightSplitPoint.x + splitPoint.x)*(splitPoint.y - previousRightSplitPoint.y));

#ifdef debugCPolygon_getLeftRightAreasFromPoint
                  if (debugFlag)
                  {
CDebugWriteFormat::getWriteFormat().writef(
"Right(resume):  index=%2d, splitPoint=%s, point=%s, rightArea =%s\n",
index,splitPoint.getString(),point->getString(),
fpfmt(rightArea,6));
                  }
#endif

                  rightArea += ((splitPoint.x + point->x)*(point->y - splitPoint.y));

                  previousRightSplitPointFlag = false;
               }
               else
               {
#ifdef debugCPolygon_getLeftRightAreasFromPoint
                  if (debugFlag)
                  {
CDebugWriteFormat::getWriteFormat().writef(
"Right(open):  index=%2d, splitPoint=%s, point=%s, rightArea =%s\n",
index,splitPoint.getString(),point->getString(),
fpfmt(rightArea,6));
                  }
#endif

                  rightArea += ((splitPoint.x + point->x)*(point->y - splitPoint.y));
                  previousRightSplitPoint     = splitPoint;
                  previousRightSplitPointFlag = true;
               }
            }
         }
         else
         {
            if (leftFlag)
            {
#ifdef debugCPolygon_getLeftRightAreasFromPoint
               if (debugFlag)
               {
CDebugWriteFormat::getWriteFormat().writef(
"Left:  index=%2d, previousPoint=%s, point =%s, LeftArea =%s\n",
index,previousPoint->getString(),point->getString(),
fpfmt(leftArea,6));
               }
#endif

               leftArea += ((previousPoint->x + point->x)*(point->y - previousPoint->y));
            }
            else
            {
#ifdef debugCPolygon_getLeftRightAreasFromPoint
               if (debugFlag)
               {
CDebugWriteFormat::getWriteFormat().writef(
"Right: index=%2d, previousPoint=%s, point =%s, LeftArea =%s\n",
index,previousPoint->getString(),point->getString(),
fpfmt(rightArea,6));
               }
#endif

               rightArea += ((previousPoint->x + point->x)*(point->y - previousPoint->y));
            }
         }

         previousPoint = point;

#ifdef debugCPolygon_getLeftRightAreasFromPoint
         if (debugFlag)
         {
CDebugWriteFormat::getWriteFormat().writef("\n");
         }
#endif
      }
   }

   leftArea  /= 2.;
   rightArea /= 2.;

#ifdef debugCPolygon_getLeftRightAreasFromPoint
   double area = getArea();

   if (debugFlag)
   {
CDebugWriteFormat::getWriteFormat().writef(
"leftArea=%s, rightArea=%s, area=%s, \n",
fpfmt(leftArea,6),fpfmt(rightArea,6),fpfmt(area,6));

      CDebugWriteFormat::getWriteFormat().flush();
   }
   else
   {
      if (!fpeq(leftArea + rightArea,area))
      {
         double leftArea2,rightArea2;
         getLeftRightAreasFromPoint(leftArea2,rightArea2,refPoint,true);
      }
   }
#endif
}

double CPolygon::getArea() const
{
   return fabs(getArea2()/2.);
}

CPoint2d CPolygon::getCenterOfMass() const
{
   double area2 = 0.;
   double areaTerm,xTerm,yTerm;
   CPoint2d centerOfMass;


   if (m_pointList.GetCount() > 2)
   {
      CPoint2d* previousPoint = m_pointList.GetTail();

      for (POSITION pos = m_pointList.GetHeadPosition();pos != NULL;)
      {
         CPoint2d* point = m_pointList.GetNext(pos);

         areaTerm = (previousPoint->x * point->y) - (point->x * previousPoint->y);
         xTerm    = previousPoint->x + point->x;
         yTerm    = previousPoint->y + point->y;

         area2          += areaTerm;
         centerOfMass.x += xTerm * areaTerm;
         centerOfMass.y += yTerm * areaTerm;

         previousPoint = point;
      }
   }

   centerOfMass.x /= 3. * area2;
   centerOfMass.y /= 3. * area2;

   return centerOfMass;
}

const CExtent& CPolygon::getExtent() const
{
   if (!m_extent.isValid())
   {
      for (POSITION pos = m_pointList.GetHeadPosition();pos != NULL;)
      {
         CPoint2d* point = m_pointList.GetNext(pos);
         m_extent.update(*point);
      }
   }

   return m_extent;
}

CPoint2d CPolygon::getCenterOfVertices() const
{
   CPoint2d centerOfVertices(0.,0.);
   int vertexCount = 0;

   for (POSITION pos = m_pointList.GetHeadPosition();pos != NULL;)
   {
      CPoint2d* point = m_pointList.GetNext(pos);
      centerOfVertices = centerOfVertices + *point;
      vertexCount++;
   }

   if (vertexCount > 0)
   {
      centerOfVertices.x /= vertexCount;
      centerOfVertices.y /= vertexCount;
   }

   return centerOfVertices;
}

bool CPolygon::normalizeByLongestEdgeAndCenterOfMass(CTMatrix& matrix)
{
   if (!m_isNormalized)
   {
      if (isValid())
      {
         makeCcw();

         // translate polygon to centroid
         CPoint2d centroid = getCenterOfVertices();
         matrix.initMatrix();
         matrix.translateCtm(centroid);

         CTMatrix translationMatrix;
         translationMatrix.translateCtm(-centroid);
         transform(translationMatrix);

         // find bottom edge
         CPolygonEdgeIterator edgeIterator(m_pointList);
         CLine2d bottomEdge;
         int bottomEdgeIndex = 0;
         CLine2d edge;
         CPoint2d origin(0.,0.);

         while (edgeIterator.getNext(edge))
         {
            if (edgeIterator.getIndex() == 0)
            {
               bottomEdge = edge;
            }
            else
            {
               double deltaLenSquared = edge.getLengthSquared() - bottomEdge.getLengthSquared();

               if (deltaLenSquared > 0.)
               {
                  bottomEdge = edge;
                  bottomEdgeIndex = edgeIterator.getIndex();
               }
               else if (deltaLenSquared == 0.)
               {
                  double deltaDistanceSquared = edge.distanceToLineSquared(origin) - bottomEdge.distanceToLineSquared(origin);

                  if (deltaDistanceSquared < 0.)
                  {
                     bottomEdge = edge;
                     bottomEdgeIndex = edgeIterator.getIndex();
                  }
               }
            }
         }

         // rotate polygon point list 
         for (int shiftedCount = 0;shiftedCount < bottomEdgeIndex;shiftedCount++)
         {
            m_pointList.rotateHeadToTail();
         }

         // rotate polygon so bottom edge is horizontal and below the origin
         double theta = bottomEdge.getTheta();
         CTMatrix rotationMatrix;
         rotationMatrix.rotateRadiansCtm(-theta);
         CPoint2d p0 = bottomEdge.get0();
         rotationMatrix.transform(p0);

         if (p0.y > 0.)
         {
            theta += Pi;
            rotationMatrix.initMatrix();
            rotationMatrix.rotateRadiansCtm(-theta);
         }

         transform(rotationMatrix);
         matrix.rotateRadiansCtm(theta);

         m_isNormalized = true;
      }
   }

   return m_isNormalized;
}

#ifdef DefineNormalizeByMinimizingEdgeAngles
bool CPolygon::normalizeByMinimizingEdgeAngles(CTMatrix& matrix)
{
   if (!m_isNormalized)
   {
      if (isValid())
      {
         makeCcw();

         // translate polygon to centroid
         CPoint2d centroid = getCenterOfVertices();
         matrix.initMatrix();
         matrix.translateCtm(centroid);
         CTMatrix translationMatrix;
         translationMatrix.translateCtm(-centroid);
         transform(translationMatrix);

         // setup polar coordinate array
         int pointCount = m_pointList.GetCount();
         CPolarCoordinateArray polarCoordinateArray;
         polarCoordinateArray.SetSize(pointCount);

         POSITION pos = m_pointList.GetHeadPosition();

         // initialize array
         for (int index = 0;index < pointCount;index++)
         {
            CPoint2d* point = m_pointList.GetNext(pos);
            CPolarCoordinate* polarCoordinate = new CPolarCoordinate(*point);
            polarCoordinateArray.SetAt(index,polarCoordinate);
         }

         // change theta from absolute to relative
         for (int index = 0;index < pointCount;index++)
         {
            CPolarCoordinate* polarCoordinate0 = polarCoordinateArray.GetAt(index);
            CPolarCoordinate* polarCoordinate1 = polarCoordinateArray.GetAt((index + 1)%pointCount);
            double deltaTheta = normalizeRadians(polarCoordinate1->getTheta() - polarCoordinate0->getTheta());

            polarCoordinate0->setTheta(deltaTheta);
         }

         int minIndex = 0;

         // find index of first minimum polar coordinate
         for (int index = 1;index < pointCount;index++)
         {
            if (*(polarCoordinateArray[index]) < *(polarCoordinateArray[minIndex]))
            {
               minIndex = index;
            }
         }

         // find index of any succeeding minimum polar coordinate with a longer run of minimums
         for (int index = minIndex + 1;index < pointCount;index++)
         {
            if (*(polarCoordinateArray[index]) == *(polarCoordinateArray[minIndex]))
            {
               for (int runIndex = 1;runIndex < pointCount;runIndex++)
               {
                  if (*(polarCoordinateArray[(index + runIndex)%pointCount]) == *(polarCoordinateArray[(minIndex + runIndex)%pointCount]))
                  {
                     continue;
                  }
                  else if (*(polarCoordinateArray[(index + runIndex)%pointCount]) < *(polarCoordinateArray[(minIndex + runIndex)%pointCount]))
                  {
                     minIndex = index;
                  }

                  break;
               }
            }
         }

         // rotate polygon list 
         for (int shiftedCount = 0;shiftedCount < minIndex;shiftedCount++)
         {
            m_pointList.rotateHeadToTail();
         }

         CTMatrix rotationMatrix;
         double theta = m_pointList.GetHead()->getTheta();
         rotationMatrix.rotateRadiansCtm(-theta);
         transform(rotationMatrix);
         matrix.rotateRadiansCtm(theta);

         m_isNormalized = true;
      }
   }

   return m_isNormalized;
}
#endif

/*  The signed area of a planar triangle specified by its vertices is:

               | x1 y1 1 |
      A = .5*  | x2 y2 1 | = .5 * (-x2*y1 + x3*y1 + x1*y2 - x3*y2 - x1*y3 + x2*y3)
               | x3 y3 1 |

   from http://mathworld.wolfram.com/TriangleArea.html
*/

bool CPolygon::normalizeByMinimizingExtents(CTMatrix& matrix)
{
   if (!m_isNormalized)
   {
      if (isValid())
      {
         makeCcw();

         // find rotation with a horizontal edge
         CPolygonEdgeIterator edgeIterator(m_pointList);
         CLine2d edge,bottomEdge;
         int bottomEdgeIndex = 0;
         CPoint2d origin,minExtentCenter;
         CTMatrix localMatrix;
         double minExtentArea = 0.;
         double minExtentHeight = 0.;
         double maxLengthSquared = 0.;
         double maxLeftArea = 0.;
         double leftArea = 0.;
         double rightArea = 0.;

         while (edgeIterator.getNext(edge))
         {
            double theta = edge.getTheta();
            localMatrix.initMatrix();
            localMatrix.rotateRadiansCtm(-theta);

            CPolygon rotatedPolygon(*this);
            rotatedPolygon.transform(localMatrix);

            CExtent extent        = rotatedPolygon.getExtent();
            CPoint2d extentCenter = extent.getCenter();
            double extentArea     = extent.getArea();
            double extentHeight   = extent.getYsize();
            double lengthSquared  = edge.getLengthSquared();
            bool selectEdgeFlag   = false;
            leftArea              = 0.;

            if (edgeIterator.getIndex() == 0)
            {
               selectEdgeFlag = true;
            }
            else if (fpeq(extentHeight,minExtentHeight))
            {
               if (fpeq(lengthSquared,maxLengthSquared))
               {
                  rotatedPolygon.getLeftRightAreasFromPoint(leftArea,rightArea,extentCenter);

                  if (leftArea > maxLeftArea)
                  {
                     selectEdgeFlag = true;
                     maxLeftArea    = leftArea;
                  }
               }
               else if (lengthSquared > maxLengthSquared)
               {
                  selectEdgeFlag = true;
                  maxLeftArea = 0.;
               }
               //else if (extentArea < minExtentArea)
               //{
               //   selectEdgeFlag = true;
               //   maxLeftArea = 0.;
               //}
            }
            else if (extentHeight <  minExtentHeight)
            {
               selectEdgeFlag = true;
               maxLeftArea = 0.;
            }

            if (selectEdgeFlag)
            {
               bottomEdge       = edge;
               bottomEdgeIndex  = edgeIterator.getIndex();
               minExtentArea    = extentArea;
               minExtentHeight  = extentHeight;
               maxLengthSquared = lengthSquared;
               minExtentCenter  = extent.getCenter();

               if (leftArea == 0.)
               {
                  rotatedPolygon.getLeftRightAreasFromPoint(leftArea,rightArea,extentCenter);
               }

               if (leftArea > maxLeftArea)
               {
                  maxLeftArea = leftArea;
               }

               localMatrix.invert();
               localMatrix.transform(minExtentCenter);
            }
         }

         // rotate polygon point list 
         for (int shiftedCount = 0;shiftedCount < bottomEdgeIndex;shiftedCount++)
         {
            m_pointList.rotateHeadToTail();
         }

         // translate polygon to extent center
         localMatrix.initMatrix();
         localMatrix.translateCtm(-minExtentCenter);
         transform(localMatrix);

         // rotate polygon so bottom edge is horizontal and below the origin
         double theta = bottomEdge.getTheta();
         CPoint2d p0 = bottomEdge.get0();
         localMatrix.transform(p0);

         localMatrix.initMatrix();
         localMatrix.rotateRadiansCtm(-theta);
         localMatrix.transform(p0);

         if (p0.y > 0.)
         {
            theta += Pi;
            localMatrix.initMatrix();
            localMatrix.rotateRadiansCtm(-theta);
         }

         transform(localMatrix);

         // setup matrix
         matrix.initMatrix();
         matrix.translateCtm(minExtentCenter);
         matrix.rotateRadiansCtm(theta);

         m_isNormalized = true;
      }
   }

   return m_isNormalized;
}

bool CPolygon::normalizeForSignature(CTMatrix& matrix)
{
   //return normalizeByLongestEdgeAndCenterOfMass(matrix);
   return normalizeByMinimizingExtents(matrix);
}

CString CPolygon::getSignature() const
{
   CString signature;

   signature.Format("%d",m_pointList.GetCount());

   return signature;
}

CString CPolygon::getSignature2() const
{
   CString signature;

   if (isNormalized())
   {
      signature.Format("%d",m_pointList.GetCount());
      CPoint2d* prevPoint = NULL;

      for (POSITION pos = m_pointList.GetHeadPosition();pos != NULL;)
      {
         CPoint2d* point = m_pointList.GetNext(pos);

         if (prevPoint != NULL)
         {
            CString angleString;
            int deltaAngle = DcaRound(normalizeDegrees(radiansToDegrees(point->getTheta() - prevPoint->getTheta())));
            angleString.Format(" %d",deltaAngle);
            signature += angleString;
         }

         prevPoint = point;
      }
   }

   return signature;
}

void CPolygon::transform(const CTMatrix& matrix)
{
   m_pointList.transform(matrix);

   resetState();
}

bool CPolygon::getCircumcircleRadii(double& minRadius,double& maxRadius) const
{
   bool retval = isValid();

   if (retval)
   {
      maxRadius = DBL_MIN;
      minRadius = DBL_MAX;

      CPoint2d radiusVector;
      CPoint2d centroid = getCenterOfVertices();

      for (POSITION pos = m_pointList.GetHeadPosition();pos != NULL;)
      {
         CPoint2d* point = m_pointList.GetNext(pos);

         double radius = centroid.distanceSqr(*point);

         if (radius < minRadius)
         {
            minRadius = radius;
         }

         if (radius > maxRadius)
         {
            maxRadius = radius;
         }
      }

      minRadius = sqrt(minRadius);
      maxRadius = sqrt(maxRadius);
   }

   return retval;
}

bool CPolygon::allVerticesOnCircumcircle(double tolerance) const
{
   bool retval = false;

   if (isValid())
   {
      double minRadius,maxRadius;
      getCircumcircleRadii(minRadius,maxRadius);

      retval = fpeq(minRadius,maxRadius,tolerance);
   }

   return retval;
}

bool CPolygon::isCcw() const
{
   if (m_isCcw == boolUnknown)
   {
      if (isValid())
      {
         double area2 = getArea2();

         if (area2 > 0.)
         {
            m_isCcw = boolTrue;
         }
         else if (area2 < 0.)
         {
            m_isCcw = boolFalse;
         }
      }
   }

   return (m_isCcw == boolTrue);
}

// return true if the polygon is modified
bool CPolygon::makeCcw()
{
   bool retval = false;

   if (isValid())
   {
      isCcw();

      if (m_isCcw == boolFalse)
      {
         retval = true;

         m_pointList.reverse();

         m_isCcw = boolTrue;
      }
   }

   return retval;
}

void CPolygon::makePoly(CPoly& poly) const
{
   if (isValid())
   {
      for (POSITION pos = m_pointList.GetHeadPosition();pos != NULL;)
      {
         CPoint2d* point = m_pointList.GetNext(pos);
         poly.addVertex(*point);
      }

      poly.close();
   }
}

CPoly* CPolygon::makeNewPoly() const
{
   CPoly* poly = NULL;

   if (isValid())
   {
      poly = new CPoly();

      makePoly(*poly);
   }

   return poly;
}

bool CPolygon::isConvex() const
{
   if (m_isConvex == boolUnknown)
   {
      m_isConvex = boolFalse;

      if (isValid() && m_pointList.GetCount() > 2)
      {
         m_isConvex = boolTrue;
         CLine2d line;
         int index = 0;
         int ccwState = 0;

         line.set1(*(m_pointList.GetTail()));

         for (POSITION pos = m_pointList.GetHeadPosition();pos != NULL;index++)
         {
            CPoint2d* point = m_pointList.GetNext(pos);

            if (index >= 1)
            {
               if (index == 1)
               {
                  ccwState = line.ccw(*point);
               }
               else if (ccwState != line.ccw(*point))
               {
                  m_isConvex = boolFalse;
                  break;
               }
            }

            line.set0(line.get1());
            line.set1(*point);
         }
      }
   }

   return (m_isConvex == boolTrue);
}

// return true if the polygon is modified
bool CPolygon::makeConvex()
{
   bool retval = false;

   if (isValid() && m_pointList.GetCount() > 2 && m_isConvex != boolTrue)
   {
      m_isConvex = boolTrue;
      CLine2d line;
      int index = 0;
      int ccwState = 0;
      CPolygonList list;

      line.set1(*(m_pointList.GetTail()));

      for (POSITION pos = m_pointList.GetHeadPosition();pos != NULL;index++)
      {
         POSITION oldPos = pos;
         CPoint2d* point = m_pointList.GetNext(pos);

         if (index >= 1)
         {
            if (index == 1)
            {
               ccwState = line.ccw(*point);
            }
            else if (ccwState != line.ccw(*point))
            {
               retval = true;

               CPolygon* protruberance = new CPolygon(getPageUnits());
               protruberance->m_isCcw = boolTrue;

               list.AddTail(protruberance);

               protruberance->addVertex(line.get1());

               while (pos != NULL)
               {
                  protruberance->addVertex(*point);
                  m_pointList.RemoveAt(oldPos);
                  oldPos = pos;
                  point = m_pointList.GetNext(pos);

                  if (ccwState == line.ccw(*point))
                  {
                     protruberance->addVertex(*point);
                     break;
                  }
               }
            }
         }

         line.set0(line.get1());
         line.set1(*point);
      }

      if (list.GetCount() > 0)
      {
         double maxArea = getArea();

         for (POSITION pos = list.GetHeadPosition();pos != NULL;)
         {
            CPolygon* polygon = list.GetNext(pos);

            polygon->makeConvex();
            double area = polygon->getArea();

            if (area > maxArea)
            {
               *this = *polygon;
               maxArea = area;
            }
         }
      }

      m_isConvex = boolTrue;
   }

   return retval;
}

//bool CPolygon::breadthSearchNearestInCircleCenter(
//   CPoint2d& pointNearestInCircleCenter,double& inCircleRadius,
//   int depth,const CPoint2d& probePoint,const CSize2d& gridSize,int maxDepth,double tolerance,
//   const CAccessOffsetItem* accessOffsetItem) const
//{
//   return breadthSearchNearestInCircleCenterWithHoleAvoidance(pointNearestInCircleCenter,inCircleRadius,
//             CPoint2d(),0.,depth,probePoint,gridSize,maxDepth,tolerance,accessOffsetItem);
//}
//
//bool CPolygon::breadthSearchNearestInCircleCenterWithHoleAvoidance(
//   CPoint2d& pointNearestInCircleCenter,double& inCircleRadius,
//   CPoint2d holeCenter,double holeRadius,
//   int depth,const CPoint2d& probePoint,const CSize2d& gridSize,int maxDepth,double tolerance,
//   const CAccessOffsetItem* accessOffsetItem) const
//{
//   bool retval  = false;
//   bool hasHole = true;
//
//   if (holeRadius <= 0.)
//   {
//      holeRadius = 0.;
//      hasHole    = false;
//   }
//
//#ifdef DebugAccessAnalysisOffset
//   // debug
//   //CDebug::addDebugPoint(probePoint,"db breadthSearchNearestInCircleCenter(%d)",depth);
//#endif
//
//   if (depth == maxDepth)
//   {
//      double probeDistance = distanceTo(probePoint);
//      double holeDistance  = holeCenter.distance(probePoint) - holeRadius;
//
//      // inside polygon and outside hole
//      if (probeDistance < 0. && holeDistance >= 0.)
//      {
//         inCircleRadius = min(-probeDistance,holeDistance);
//         pointNearestInCircleCenter = probePoint;
//         retval = true;
//
//#ifdef DebugAccessAnalysisOffset
//         // debug
//         //CDebug::addDebugPoint(pointNearestInCircleCenter,"db breadthSearch Found Point");
//#endif
//      }
//   }
//   else if (depth < maxDepth)
//   {
//      inCircleRadius = 0.;
//      CSize2d searchGrid = gridSize/3.;
//      bool validDirectionFlag = true;
//
//      for (int index = 0;index < 9;index++)
//      {
//         CPoint2d point;
//
//         switch (index)
//         {
//         case 0:
//            point.x = probePoint.x - searchGrid.cx;
//            point.y = probePoint.y + searchGrid.cy;
//            break;
//         case 1:
//            point.x = probePoint.x;
//            point.y = probePoint.y + searchGrid.cy;
//            break;
//         case 2:
//            point.x = probePoint.x + searchGrid.cx;
//            point.y = probePoint.y + searchGrid.cy;
//            break;
//
//         case 3:
//            point.x = probePoint.x - searchGrid.cx;
//            point.y = probePoint.y;
//            break;
//         case 4:
//            point.x = probePoint.x;
//            point.y = probePoint.y;
//            break;
//         case 5:
//            point.x = probePoint.x + searchGrid.cx;
//            point.y = probePoint.y;
//            break;
//
//         case 6:
//            point.x = probePoint.x - searchGrid.cx;
//            point.y = probePoint.y - searchGrid.cy;
//            break;
//         case 7:
//            point.x = probePoint.x;
//            point.y = probePoint.y - searchGrid.cy;
//            break;
//         case 8:
//            point.x = probePoint.x + searchGrid.cx;
//            point.y = probePoint.y - searchGrid.cy;
//            break;
//         }
//
//         if (accessOffsetItem != NULL)
//         {
//            if (hasHole)
//            {
//               AccessOffsetDirectionTag loopDirection = calcAccessOffsetDirection(holeCenter,point,tolerance);
//               validDirectionFlag = accessOffsetItem->getDirectionList().contains(loopDirection);
//            }
//         }
//
//         if (validDirectionFlag)
//         {
//            CPoint2d foundPoint;
//            double distance;
//
//#ifdef DebugAccessAnalysisOffset
//            // debug
//            CDebug::addDebugPoint(point,"db breadthSearchNearestInCircleCenter(depth=%d) - Valid Direction - %s",depth,
//                        (accessOffsetItem != NULL) ? accessItemTypeTagToString(accessOffsetItem->getItemType()) : "null item");
//#endif
//
//            if (breadthSearchNearestInCircleCenterWithHoleAvoidance(foundPoint,distance,
//                   holeCenter,holeRadius,depth + 1,point,searchGrid,maxDepth,tolerance,accessOffsetItem))
//            {
//               if (distance > inCircleRadius)
//               {
//                  inCircleRadius = distance;
//                  pointNearestInCircleCenter = foundPoint;
//                  retval = true;
//
//                  // debug
//                  //if (depth == 0)
//                  //{
//                  //   addDebugPoint(foundPoint,"db breadthSearchNearestInCircleCenter(nearest)");
//                  //}
//               }
//            }
//         }
//         else
//         {
//#ifdef DebugAccessAnalysisOffset
//            // debug
//            CDebug::addDebugPoint(point,"db breadthSearchNearestInCircleCenter(depth=%d) - Invalid Direction - %s",depth,
//                       (accessOffsetItem != NULL) ? accessItemTypeTagToString(accessOffsetItem->getItemType()) : "null item");
//#endif
//         }
//      }
//   }
//
//   return retval;
//}
//
//bool CPolygon::depthSearchNearestInCircleCenter(
//   CPoint2d& pointNearestInCircleCenter,double& inCircleRadius,
//   int depth,const CPoint2d& probePoint,const CSize2d& gridSize,double tolerance,
//   const CAccessOffsetItem* accessOffsetItem) const
//{
//   return depthSearchNearestInCircleCenterWithHoleAvoidance(pointNearestInCircleCenter,
//             inCircleRadius,CPoint2d(),0.,depth,probePoint,gridSize,tolerance,accessOffsetItem);
//}
//
//bool CPolygon::depthSearchNearestInCircleCenterWithHoleAvoidance(
//   CPoint2d& pointNearestInCircleCenter,double& inCircleRadius,
//   CPoint2d holeCenter,double holeRadius,
//   int depth,const CPoint2d& probePoint,const CSize2d& gridSize,double tolerance,
//   const CAccessOffsetItem* accessOffsetItem) const
//{
//   bool retval = false;
//
//#ifdef DebugAccessAnalysisOffset
//   // debug
//   //CDebug::addDebugPoint(probePoint,"db depthSearchNearestInCircleCenterWithHoleAvoidance(%d)",depth);
//#endif
//
//   double probeDistance = distanceTo(probePoint);
//   double holeDistance  = holeCenter.distance(probePoint) - holeRadius;
//
//   if (probeDistance < 0. && holeDistance >= 0.)
//   {
//      double distance = min(-probeDistance,holeDistance);
//      double gridMaxSize = gridSize.getMaxSize();
//
//      if (gridMaxSize <= tolerance)
//      {
//         inCircleRadius = distance;
//         pointNearestInCircleCenter = probePoint;
//         retval = true;
//      }
//      else if (inCircleRadius < distance + 2. * gridMaxSize)
//      {
//         inCircleRadius = 0.;
//         CSize2d searchGrid = gridSize/3.;
//         bool validDirectionFlag = true;
//
//         for (int index = 0;index < 9;index++)
//         {
//            CPoint2d point;
//
//            switch (index)
//            {
//            case 0:
//               point.x = probePoint.x - searchGrid.cx;
//               point.y = probePoint.y + searchGrid.cy;
//               break;
//            case 1:
//               point.x = probePoint.x;
//               point.y = probePoint.y + searchGrid.cy;
//               break;
//            case 2:
//               point.x = probePoint.x + searchGrid.cx;
//               point.y = probePoint.y + searchGrid.cy;
//               break;
//
//            case 3:
//               point.x = probePoint.x - searchGrid.cx;
//               point.y = probePoint.y;
//               break;
//            case 4:
//               point.x = probePoint.x;
//               point.y = probePoint.y;
//               break;
//            case 5:
//               point.x = probePoint.x + searchGrid.cx;
//               point.y = probePoint.y;
//               break;
//
//            case 6:
//               point.x = probePoint.x - searchGrid.cx;
//               point.y = probePoint.y - searchGrid.cy;
//               break;
//            case 7:
//               point.x = probePoint.x;
//               point.y = probePoint.y - searchGrid.cy;
//               break;
//            case 8:
//               point.x = probePoint.x + searchGrid.cx;
//               point.y = probePoint.y - searchGrid.cy;
//               break;
//            }
//
//            if (accessOffsetItem != NULL)
//            {
//               AccessOffsetDirectionTag loopDirection = calcAccessOffsetDirection(holeCenter,point,tolerance);
//               validDirectionFlag = accessOffsetItem->getDirectionList().contains(loopDirection);
//            }
//
//            if (validDirectionFlag)
//            {
//               CPoint2d foundPoint;
//
//               if (depthSearchNearestInCircleCenterWithHoleAvoidance(foundPoint,distance,
//                     holeCenter,holeRadius,depth + 1,point,searchGrid,tolerance,accessOffsetItem))
//               {
//                  if (distance > inCircleRadius)
//                  {
//                     inCircleRadius = distance;
//                     pointNearestInCircleCenter = foundPoint;
//                     retval = true;
//
//#ifdef DebugAccessAnalysisOffset
//                     // debug
//                     CDebug::addDebugPoint(pointNearestInCircleCenter,
//                                "db depthSearchNearestInCircleCenterWithHoleAvoidance(depth=%d) - Valid Direction - %s",depth,
//                                (accessOffsetItem != NULL) ? accessItemTypeTagToString(accessOffsetItem->getItemType()) : "null item");
//#endif
//                  }
//               }
//            }
//            else
//            {
//#ifdef DebugAccessAnalysisOffset
//               // debug
//               CDebug::addDebugPoint(point,
//                        "db depthSearchNearestInCircleCenterWithHoleAvoidance(depth=%d) - Invalid Direction - %s",depth,
//                        (accessOffsetItem != NULL) ? accessItemTypeTagToString(accessOffsetItem->getItemType()) : "null item");
//#endif
//            }
//         }
//      }
//   }
//
//   return retval;
//}
//
//bool CPolygon::getInCircleCenter(CPoint2d& inCircleCenter,double& circleRadius,double tolerance,
//   const CAccessOffsetItem* accessOffsetItem)
//{
//   bool retval = false;
//
//   makeCcw();
//   CExtent extent = getExtent();
//   CSize2d grid   = extent.getSize() * 1.5;
//   CPoint2d point = extent.getCenter();
//   int maxDepth = 2;
//
//   if (breadthSearchNearestInCircleCenter(inCircleCenter,circleRadius,0,point,grid,maxDepth,tolerance,accessOffsetItem))
//   {
//      point = inCircleCenter;
//      grid = grid / (1 << (maxDepth + 1));
//
//      if (depthSearchNearestInCircleCenter(inCircleCenter,circleRadius,maxDepth,point,grid,tolerance,accessOffsetItem))
//      {
//         retval = true;
//      }
//   }
//
//   return retval;
//}
//
//bool CPolygon::getInCircleCenterWithHoleAvoidance(CPoint2d& inCircleCenter,double& circleRadius,double tolerance,
//   CPoint2d holeCenter,double holeRadius,const CAccessOffsetItem* accessOffsetItem)
//{
//   bool retval = false;
//
//   makeCcw();
//   CExtent extent = getExtent();
//
//   if (accessOffsetItem != NULL)
//   {  // adjust extent so that it is centered on an orhogonal direction if it spans that direction
//      CPoint2d extentCenter = extent.getCenter();
//
//      bool northSpan = (accessOffsetItem->getDirectionList().contains(accessOffsetDirectionNorth) &&
//                        extent.getYmax() >= holeCenter.y + holeRadius &&
//                        extent.isOnOrInside(CPoint2d(holeCenter.x,extentCenter.y)));
//
//      bool southSpan = (accessOffsetItem->getDirectionList().contains(accessOffsetDirectionSouth) &&
//                        extent.getYmin() <= holeCenter.y - holeRadius &&
//                        extent.isOnOrInside(CPoint2d(holeCenter.x,extentCenter.y)));
//
//      bool westSpan  = (accessOffsetItem->getDirectionList().contains(accessOffsetDirectionWest) &&
//                        extent.getXmax() >= holeCenter.x + holeRadius &&
//                        extent.isOnOrInside(CPoint2d(extentCenter.x,holeCenter.y)));
//
//      bool eastSpan  = (accessOffsetItem->getDirectionList().contains(accessOffsetDirectionEast) &&
//                        extent.getXmin() <= holeCenter.x - holeRadius &&
//                        extent.isOnOrInside(CPoint2d(extentCenter.x,holeCenter.y)));
//
//      if (northSpan || southSpan)
//      {
//         double dx = max(holeCenter.x - extent.getXmin(),extent.getXmax() - holeCenter.x);
//
//         extent.update(CPoint2d(holeCenter.x + dx,extentCenter.y));
//         extent.update(CPoint2d(holeCenter.x - dx,extentCenter.y));
//      }
//
//      if (westSpan || eastSpan)
//      {
//         double dy = max(holeCenter.y - extent.getYmin(),extent.getYmax() - holeCenter.y);
//
//         extent.update(CPoint2d(extentCenter.x,holeCenter.y + dy));
//         extent.update(CPoint2d(extentCenter.x,holeCenter.y - dy));
//      }
//   }
//
//   CSize2d grid   = extent.getSize() * 1.5;
//   CPoint2d point = extent.getCenter();
//   int maxDepth = 2;
//
//   if (breadthSearchNearestInCircleCenterWithHoleAvoidance(inCircleCenter,circleRadius,
//          holeCenter,holeRadius,0,point,grid,maxDepth,tolerance,accessOffsetItem))
//   {
//      point = inCircleCenter;
//      grid = grid / (1 << (maxDepth + 1));
//
//      if (depthSearchNearestInCircleCenterWithHoleAvoidance(inCircleCenter,circleRadius,
//             holeCenter,holeRadius,maxDepth,point,grid,tolerance,accessOffsetItem))
//      {
//         retval = true;
//      }
//   }
//
//   return retval;
//}

bool CPolygon::isValid() const
{
   if (m_isValid == boolUnknown)
   {
      m_isValid = boolFalse;

      if (m_pointList.GetCount() > 2)
      {
         if (!selfIntersects())
         {
            m_isValid = boolTrue;
         }
      }
   }

   return (m_isValid == boolTrue);
}

void CPolygon::write(CWriteFormat& writeFormat)
{
   writeFormat.write("<CPolygon>\n");

   for (POSITION pos = m_pointList.GetHeadPosition();pos != NULL;)
   {
      CPoint2d* point = m_pointList.GetNext(pos);

      writeFormat.writef("%s %s\n",fpfmt(point->x,14),fpfmt(point->y,14));
   }

   writeFormat.write("</CPolygon>\n");
}

//_____________________________________________________________________________
CPolygonSubdivisionVertex::CPolygonSubdivisionVertex(int id,const CPoint2d& point) :
   m_id(id),
   m_point(point)
{
   m_previous[0] = NULL;
   m_previous[1] = NULL;
   m_next[0]     = NULL;
   m_next[1]     = NULL;
}

CPolygonSubdivisionVertex* CPolygonSubdivisionVertex::getPrevious(int index) const
{
   return m_previous[index];
}

void CPolygonSubdivisionVertex::setPrevious(int index,CPolygonSubdivisionVertex* previous)
{
   m_previous[index] = previous;
}

CPolygonSubdivisionVertex* CPolygonSubdivisionVertex::getNext(int index) const
{
   return m_next[index];
}

void CPolygonSubdivisionVertex::setNext(int index,CPolygonSubdivisionVertex* next)
{
   m_next[index] = next;
}

void CPolygonSubdivisionVertex::append(int index,CPolygonSubdivisionVertex* appendedVertex)
{
   if (this == appendedVertex)
   {
      CPolygonSubdivision::logicError(7);
   }
   else
   {
      appendedVertex->setPrevious(index,this);
      appendedVertex->setNext(index,this->getNext(index));

      this->getNext(index)->setPrevious(index,appendedVertex);
      this->setNext(index,appendedVertex);
   }
}

bool CPolygonSubdivisionVertex::isIntersection() const
{
   bool isIntersectionFlag = ((m_next[0] != NULL) && (m_next[1] != NULL));

   return isIntersectionFlag;
}

void CPolygonSubdivisionVertex::merge(int index,CPolygonSubdivisionVertex* mergedVertex)
{
   if (this == mergedVertex)
   {
      CPolygonSubdivision::logicError(6);
   }
   else
   {
      this->setPrevious(index,mergedVertex->getPrevious(index));
      this->setNext(index,mergedVertex->getNext(index));

      mergedVertex->getPrevious(index)->setNext(index,this);
      mergedVertex->getNext(index)->setPrevious(index,this);

      mergedVertex->setNext(index,NULL);
      mergedVertex->setPrevious(index,NULL);
   }
}

void CPolygonSubdivisionVertex::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;

   writeFormat.writef(
"CPolygonSubdivisionVertex { m_id=%3d  m_point=(%25.18f,%25.18f) next(%3d,%3d) prev(%3d,%3d)}\n",
m_id,
m_point.x,
m_point.y,
(m_next[0] == NULL) ? -1 : m_next[0]->m_id,
(m_next[1] == NULL) ? -1 : m_next[1]->m_id,
(m_previous[0] == NULL) ? -1 : m_previous[0]->m_id,
(m_previous[1] == NULL) ? -1 : m_previous[1]->m_id
);
}

//_____________________________________________________________________________
unsigned int CPolygonSubdivision::m_logicErrorFlags = 0;
bool         CPolygonSubdivision::m_debugMergeFlag  = false;

CPolygonSubdivision::CPolygonSubdivision(CPolygon& polygon0,CPolygon& polygon1) :
   m_polygon0(polygon0),
   m_polygon1(polygon1)
{
   m_polygon0.makeCcw();
   m_polygon1.makeCcw();

   m_polygons[0] = &m_polygon0;
   m_polygons[1] = &m_polygon1;
   m_head[0]     = NULL;
   m_head[1]     = NULL;

   addPolygon(0);
   addPolygon(1);
}

CPolygonSubdivision::~CPolygonSubdivision()
{
}

void CPolygonSubdivision::logicError(int errorNumber)
{
   unsigned int mask = 1 << errorNumber;
   m_logicErrorFlags |= mask;
}

CPolygonSubdivisionVertex* CPolygonSubdivision::addNewVertex(const CPoint2d& point)
{
   CPolygonSubdivisionVertex* vertex = new CPolygonSubdivisionVertex(m_vertices.GetCount(),point);
   m_vertices.AddTail(vertex);

   return vertex;
}

void CPolygonSubdivision::addPolygon(int index)
{
   const CPolygon& polygon = *(m_polygons[index]);
   CPolygonSubdivisionVertex* previousVertex = NULL;
   CPolygonSubdivisionVertex*& headVertex = m_head[index];

   for (POSITION pos = polygon.getHeadPointPosition();pos != NULL;)
   {
      CPoint2d point = polygon.getNextPoint(pos);
      CPolygonSubdivisionVertex* vertex = addNewVertex(point);

      if (previousVertex != NULL)
      {
         vertex->setPrevious(index,previousVertex);
         previousVertex->setNext(index,vertex);

         if (pos == NULL)
         {
            vertex->setNext(index,headVertex);
            headVertex->setPrevious(index,vertex);
         }
      }
      else
      {
         headVertex = vertex;
      }

      previousVertex = vertex;
   }
}

//#define debug_CPolygonSubdivision_merge

bool CPolygonSubdivision::merge(CPolygon& mergedPolygon)
{
#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
   CDebugWriteFormat::getWriteFormat().write("__________________________________________________________________________________________________\n"
                                             " Entering CPolygonSubdivision::merge()\n");
   dump(CDebugWriteFormat::getWriteFormat());
}
#endif

   bool retval = false;
   CPolygonSubdivisionVertex* headIntersect = NULL;
   CPolygonSubdivisionVertex* headExternal = NULL;
   int intersectCount = 0;
   int loopCount0 = 0;
   int maxLoopCount = 2*m_vertices.GetCount();
   bool loopFlag = true;

   for (CPolygonSubdivisionVertex* vertex0 = m_head[0];loopFlag;loopCount0++)
   {
      CPolygonSubdivisionVertex* next0 = vertex0->getNext(0);

      if (loopCount0 > maxLoopCount)
      {
         logicError(3);
         loopFlag = false;
         break;
      }

      int loopCount1 = 0;

      for (CPolygonSubdivisionVertex* vertex1 = m_head[1];loopFlag;loopCount1++)
      {
         if (loopCount1 > maxLoopCount)
         {
            logicError(3);
            loopFlag = false;
            break;
         }

         CPolygonSubdivisionVertex* next1 = vertex1->getNext(1);

         CLine2d line0(vertex0->getPoint(),next0->getPoint());
         CLine2d line1(vertex1->getPoint(),next1->getPoint());
         CLine2d *line0part0,*line0part1,*line1part0,*line1part1;

         if (line0.mutualBisect(line1,line0part0,line0part1,line1part0,line1part1))
         {
            intersectCount++;
            CPolygonSubdivisionVertex* intersectionVertex = NULL;

            bool split0 = (line0part0 != NULL && line0part1 != NULL);
            bool split1 = (line1part0 != NULL && line1part1 != NULL);

            if (split0 && split1)
            {  // both edges split, add a new vertex
               intersectionVertex = addNewVertex(line0part0->get1());

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
               CDebugWriteFormat::getWriteFormat().write("\nBoth edges split, add a new vertex\n");
               intersectionVertex->dump(CDebugWriteFormat::getWriteFormat());
}
#endif

               vertex0->append(0,intersectionVertex);
               vertex1->append(1,intersectionVertex);
               vertex1 = vertex1->getNext(1);  // advance vertex1 to the intersection point
            }
            else if (!split0 && !split1)
            {  // One or both vertices are coincident
               
               if (vertex0->getPoint() == vertex1->getPoint())
               {
                  intersectionVertex = vertex0;

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
               CDebugWriteFormat::getWriteFormat().write("\nOne or both vertices are coincident\n");

               CDebugWriteFormat::getWriteFormat().write("vertex0: ");
               vertex0->dump(CDebugWriteFormat::getWriteFormat());

               CDebugWriteFormat::getWriteFormat().write("vertex1: ");
               vertex1->dump(CDebugWriteFormat::getWriteFormat());

               CDebugWriteFormat::getWriteFormat().write("intersectionVertex: ");
               intersectionVertex->dump(CDebugWriteFormat::getWriteFormat());
}
#endif

                  // merge vertex1 into vertex0
                  vertex0->merge(1,vertex1);

                  if (vertex1 == m_head[1])
                  {
                     m_head[1] = vertex0;
                  }

                  vertex1 = vertex0;

                  if (next0->getPoint() == next1->getPoint())
                  {
                     // merge next1 into next0
                     next0->merge(1,next1);
                  }
               }
               else if (vertex0->getPoint() == next1->getPoint())
               {
                  intersectionVertex = vertex0;

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
                  CDebugWriteFormat::getWriteFormat().write("\nOne or both vertices are coincident\n");

                  CDebugWriteFormat::getWriteFormat().write("vertex0: ");
                  vertex0->dump(CDebugWriteFormat::getWriteFormat());

                  CDebugWriteFormat::getWriteFormat().write("next1: ");
                  next1->dump(CDebugWriteFormat::getWriteFormat());

                  CDebugWriteFormat::getWriteFormat().write("intersectionVertex: ");
                  intersectionVertex->dump(CDebugWriteFormat::getWriteFormat());
}
#endif

                  // merge next1 into vertex0
                  vertex0->merge(1,next1);

                  if (next0->getPoint() == vertex1->getPoint())
                  {
                     // merge vertex1 into next0
                     next0->merge(1,vertex1);

                     if (vertex1 == m_head[1])
                     {
                        m_head[1] = next0;
                     }

                     vertex1 = next0;
                  }
               }
               else if (next0->getPoint() == vertex1->getPoint())
               {
                  intersectionVertex = next0;

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
                  CDebugWriteFormat::getWriteFormat().write("\nOne or both vertices are coincident\n");

                  CDebugWriteFormat::getWriteFormat().write("next0: ");
                  next0->dump(CDebugWriteFormat::getWriteFormat());

                  CDebugWriteFormat::getWriteFormat().write("vertex1: ");
                  vertex1->dump(CDebugWriteFormat::getWriteFormat());

                  CDebugWriteFormat::getWriteFormat().write("intersectionVertex: ");
                  intersectionVertex->dump(CDebugWriteFormat::getWriteFormat());
}
#endif

                  // merge vertex1 into next0
                  next0->merge(1,vertex1);

                  if (vertex1 == m_head[1])
                  {
                     m_head[1] = next0;
                  }

                  vertex1 = next0;
               }
               else if (next0->getPoint() == next1->getPoint())
               {
                  intersectionVertex = next0;

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
                  CDebugWriteFormat::getWriteFormat().write("\nOne or both vertices are coincident\n");

                  CDebugWriteFormat::getWriteFormat().write("next0: ");
                  next0->dump(CDebugWriteFormat::getWriteFormat());

                  CDebugWriteFormat::getWriteFormat().write("next1: ");
                  next1->dump(CDebugWriteFormat::getWriteFormat());

                  CDebugWriteFormat::getWriteFormat().write("intersectionVertex: ");
                  intersectionVertex->dump(CDebugWriteFormat::getWriteFormat());
}
#endif

                  // merge next1 into next0
                  next0->merge(1,next1);
               }
               else
               {
                  logicError(1);
               }
            }
            else if (split0)
            {
               CPolygonSubdivisionVertex* otherVertex = ((line1part0 == NULL) ? vertex1 : next1);

               vertex0->append(0,otherVertex);
               intersectionVertex = otherVertex;

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
               CDebugWriteFormat::getWriteFormat().write("\nVertex1 splits line0\n");

               CDebugWriteFormat::getWriteFormat().write("intersectionVertex: ");
               intersectionVertex->dump(CDebugWriteFormat::getWriteFormat());
}
#endif

            }
            else if (split1)
            {
               CPolygonSubdivisionVertex* otherVertex = ((line0part0 == NULL) ? vertex0 : next0);

               vertex1->append(1,otherVertex);
               intersectionVertex = otherVertex;
               vertex1 = vertex1->getNext(1);  // advance vertex1 to the intersection point

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
               CDebugWriteFormat::getWriteFormat().write("\nVertex0 splits line1\n");

               CDebugWriteFormat::getWriteFormat().write("intersectionVertex: ");
               intersectionVertex->dump(CDebugWriteFormat::getWriteFormat());
}
#endif

            }
            else 
            {
               logicError(2);
            }

            if (split0)
            {
               delete line0part1;
            }

            if (split1)
            {
               delete line1part1;
            }

            if (headIntersect == NULL)
            {
               headIntersect = intersectionVertex;
            }

            maxLoopCount = 2*m_vertices.GetCount();
         }
         else if (headExternal == NULL && loopCount1 == 0 && !vertex0->isIntersection())
         {
            if (!m_polygon1.isInside(vertex0->getPoint()))
            {
               headExternal = vertex0;
            }
         }

         next0 = vertex0->getNext(0);
         next1 = vertex1->getNext(1);

         if (next1 == m_head[1])
         {
            break;
         }

         vertex1 = next1;
      }

      if (next0 == m_head[0])
      {
         break;
      }

      vertex0 = next0;
   }

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
   CDebugWriteFormat::getWriteFormat().writef("\nTraversing CPolygonSubdivision\n");
   dump(CDebugWriteFormat::getWriteFormat());
}
#endif

   if (headIntersect != NULL)
   {
      CPolygonSubdivisionVertex* headVertex = ((headExternal == NULL) ? headIntersect : headExternal);

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
      CDebugWriteFormat::getWriteFormat().write("headIntersect: ");
      headIntersect->dump(CDebugWriteFormat::getWriteFormat());

      if (headExternal != NULL) 
      {
         CDebugWriteFormat::getWriteFormat().write("headExternal: ");
         headExternal->dump(CDebugWriteFormat::getWriteFormat());
      }

      CDebugWriteFormat::getWriteFormat().write("headVertex: ");
      headVertex->dump(CDebugWriteFormat::getWriteFormat());
}
#endif

      int exteriorPolygonIndex = 0;
      CPolygonSubdivisionVertex* vertex = headVertex;
      mergedPolygon.empty();

      mergedPolygon.addVertex(vertex->getPoint());

      maxLoopCount = 2*m_vertices.GetCount();

      for (int loopCount = 0;;loopCount++)
      {
         if (loopCount > maxLoopCount)
         {
            logicError(9);

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
            CDebugWriteFormat::getWriteFormat().writef(">>> logicError(9)\n",exteriorPolygonIndex);
}
#endif

            break;
         }

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
      CDebugWriteFormat::getWriteFormat().writef("\nexteriorPolygonIndex=%d\n",exteriorPolygonIndex);
      vertex->dump(CDebugWriteFormat::getWriteFormat());
}
#endif

         if (vertex->isIntersection())
         {
            CPolygonSubdivisionVertex* next0 = vertex->getNext(0);
            CPolygonSubdivisionVertex* next1 = vertex->getNext(1);

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
            CDebugWriteFormat::getWriteFormat().write("\nvertex is intersection\n");

            CDebugWriteFormat::getWriteFormat().write("next0: ");
            next0->dump(CDebugWriteFormat::getWriteFormat());

            CDebugWriteFormat::getWriteFormat().write("next1: ");
            next1->dump(CDebugWriteFormat::getWriteFormat());
}
#endif

            if (next0 == next1)
            {
               vertex = next0;
               exteriorPolygonIndex = 0;

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
               CDebugWriteFormat::getWriteFormat().writef(">>> next0 == next1 - exteriorPolygonIndex=%d\n",exteriorPolygonIndex);
}
#endif

            }
            else if (!next0->isIntersection() && !next1->isIntersection())
            {
               if (!m_polygon0.isInside(next1->getPoint()))
               {
                  vertex = next1;
                  exteriorPolygonIndex = 1;

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
                  CDebugWriteFormat::getWriteFormat().writef(">>> next0 and next1 are not intersections; next1 is not inside polygon0 - exteriorPolygonIndex=%d\n",exteriorPolygonIndex);
}
#endif

               }
               else if (!m_polygon1.isInside(next0->getPoint()))
               {
                  vertex = next0;
                  exteriorPolygonIndex = 0;

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
                  CDebugWriteFormat::getWriteFormat().writef(">>> next0 and next1 are not intersections; next0 is not inside polygon1 - exteriorPolygonIndex=%d\n",exteriorPolygonIndex);
}
#endif

               }
               else
               {
                  logicError(4);

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
                  CDebugWriteFormat::getWriteFormat().writef(">>> logicError(4)\n",exteriorPolygonIndex);
}
#endif

                  break;
               }
            }
            else if (!next0->isIntersection())
            {
               if (m_polygon1.isInside(next0->getPoint()))
               {
                  vertex = next1;
                  exteriorPolygonIndex = 1;

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
                  CDebugWriteFormat::getWriteFormat().writef(">>> next0 is not intersection and is inside polyton1 - exteriorPolygonIndex=%d\n",exteriorPolygonIndex);
}
#endif
               }
               else
               {
                  vertex = next0;
                  exteriorPolygonIndex = 0;

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
                  CDebugWriteFormat::getWriteFormat().writef(">>> next0 is not intersection - exteriorPolygonIndex=%d\n",exteriorPolygonIndex);
}
#endif
               }

            }
            else if (!next1->isIntersection())
            {
               if (m_polygon0.isInside(next1->getPoint()))
               {
                  vertex = next0;
                  exteriorPolygonIndex = 0;

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
                  CDebugWriteFormat::getWriteFormat().writef(">>> next1 is not intersection and is inside polyton0 - exteriorPolygonIndex=%d\n",exteriorPolygonIndex);
}
#endif
               }
               else
               {
                  vertex = next1;
                  exteriorPolygonIndex = 1;

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
                  CDebugWriteFormat::getWriteFormat().writef(">>> next1 is not intersection - exteriorPolygonIndex=%d\n",exteriorPolygonIndex);
}
#endif
               }

            }
            else // if (next0->isIntersection() && next1->isIntersection())
            {
               CPoint2d p0 = next0->getPoint() + vertex->getPoint();
               p0.x /= 2.;
               p0.y /= 2.;

               CPoint2d p1 = next1->getPoint() + vertex->getPoint();
               p1.x /= 2.;
               p1.y /= 2.;

               // choose the exterior edge
               if (!m_polygon0.isInside(p1))
               {
                  vertex = next1;
                  exteriorPolygonIndex = 1;

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
                  CDebugWriteFormat::getWriteFormat().writef(">>> p1 is not inside polygon0 - exteriorPolygonIndex=%d\n",exteriorPolygonIndex);
}
#endif

               }
               else if (!m_polygon1.isInside(p0))
               {
                  vertex = next0;
                  exteriorPolygonIndex = 0;

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
                  CDebugWriteFormat::getWriteFormat().writef(">>> p0 is not inside polygon1 - exteriorPolygonIndex=%d\n",exteriorPolygonIndex);
}
#endif

               }
               else
               {
                  logicError(8);

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
                  CDebugWriteFormat::getWriteFormat().writef(">>> logicError(8)\n",exteriorPolygonIndex);
}
#endif

                  break;
               }
            }
         }
         else
         {
            vertex = vertex->getNext(exteriorPolygonIndex);

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
            CDebugWriteFormat::getWriteFormat().writef(">>> vertex is not an intersection\n",exteriorPolygonIndex);
}
#endif

         }

         if (vertex == headVertex)
         {
            retval = true;
            break;
         }
         else
         {
            mergedPolygon.addVertex(vertex->getPoint());

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
            CDebugWriteFormat::getWriteFormat().write("mergedPolygon.addVertex(): ");
            vertex->dump(CDebugWriteFormat::getWriteFormat());
}
#endif
         }
      }
   }

#ifdef debug_CPolygonSubdivision_merge
if (getDebugMergeFlag())
{
   CDebugWriteFormat::getWriteFormat().write(" Exiting CPolygonSubdivision::merge()\n"
                                             "__________________________________________________________________________________________________\n"
                                             );
   CDebugWriteFormat::getWriteFormat().flush();
}
#endif

   return retval;
}

void CPolygonSubdivision::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;

   writeFormat.writef(
"CPolygonSubdivision\n"
"{\n"
);

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      for (int index = 0;index <= 1;index++)
      {
         CString idHeader;
         idHeader.Format(" %d: ",index);
         writeFormat.pushHeader(idHeader);

         for (CPolygonSubdivisionVertex* vertex = m_head[index];;)
         {
            vertex->dump(writeFormat,depth);
            vertex = vertex->getNext(index);

            if (vertex == m_head[index])
            {
               break;
            }
         }

         writeFormat.popHeader();
         writeFormat.writef("\n");
      }

      writeFormat.popHeader();
   }

   writeFormat.writef("}\n");
}

//_____________________________________________________________________________
CPolygonQfeTree::CPolygonQfeTree(bool isContainer) :
   CDerivedQfeTree<CPolygonQfe>(isContainer)
{
}

//_____________________________________________________________________________
CPolygonQfe::CPolygonQfe(const CPoint2d& origin,CPolygon* polygon) :
   CTypedQfe<CPolygon>(origin,polygon)
{
   m_cluster = NULL;
}

CPolygonQfe::~CPolygonQfe()
{
   delete getObject();
}

//_____________________________________________________________________________
CPolygonCluster::CPolygonCluster(CPolygonQfe& polygonQfe)
: m_polygon(polygonQfe.getPolygon().getPageUnits())
{
   m_list.AddTail(&polygonQfe);
   m_polygon = polygonQfe.getPolygon();
}

bool CPolygonCluster::merge(CPolygonCluster& other)
{
   bool retval = m_polygon.merge(other.getPolygon());

   if (retval)
   {
      for (POSITION pos = other.getList().GetHeadPosition();pos != NULL;)
      {
         CPolygonQfe* polygonQfe = other.getList().GetNext(pos);
         polygonQfe->setCluster(*this);
         m_list.AddTail(polygonQfe);
      }

      other.getList().RemoveAll();
   }

   return retval;
}

//_____________________________________________________________________________
CPolygonClusterMap::CPolygonClusterMap()
{
}

CPolygonClusterMap::~CPolygonClusterMap()
{
}

//bool CPolygonClusterMap::lookup(CPolygonQfeList* qfeList)
//{
//   bool retval = true;
//
//   return retval;
//}
//
//bool CPolygonClusterMap::setAt(CPolygonQfeList* qfeList)
//{
//   bool retval = true;
//
//   return retval;
//}
//
//bool CPolygonClusterMap::deleteAt(CPolygonQfeList* qfeList)
//{
//   bool retval = true;
//
//   return retval;
//}

bool CPolygonClusterMap::merge(CPolygonQfe& polygonQfe0,CPolygonQfe& polygonQfe1)
{
   bool retval = false;

   CPolygonCluster* cluster0 = NULL;

   if (!m_map.Lookup(&polygonQfe0,cluster0))
   {
      cluster0 = new CPolygonCluster(polygonQfe0);
      m_map.SetAt(&polygonQfe0,cluster0);
   }

   CPolygonCluster* cluster1 = NULL;

   if (!m_map.Lookup(&polygonQfe1,cluster1))
   {
      cluster1 = new CPolygonCluster(polygonQfe1);
      m_map.SetAt(&polygonQfe1,cluster1);
   }

   if (cluster0->merge(*cluster1))
   {
      m_map.RemoveKey(&polygonQfe1);
      delete cluster1;

      retval = true;
   }

   return retval;
}

#if defined(UseAlgoCOMsPolygonLibrary)

//_____________________________________________________________________________
CAlgoComPolygon::CAlgoComPolygon() :
   m_algoComPolygon(__uuidof(AlgoCOMsGenPolygon))
{
}

CAlgoComPolygon::CAlgoComPolygon(CPolygon& polygon) :
   m_algoComPolygon(__uuidof(AlgoCOMsGenPolygon))
{
   int ordinateCount = 2*polygon.getVertexCount();

   LPSAFEARRAY coords_SA = 0;
   m_algoComPolygon->MakeArrayOfDoubles(ordinateCount,&coords_SA);
   long index = 0;
   CPoint2d point2d;

   for (POSITION pos = polygon.getHeadPointPosition();pos != NULL;)
   {
      point2d = polygon.getNextPoint(pos);

      SafeArrayPutElement(coords_SA,&index,(void*)(&(point2d.x)));
      index++;

      SafeArrayPutElement(coords_SA,&index,(void*)(&(point2d.y)));
      index++;
   }

   m_algoComPolygon->CreateFromVerticesDouble(coords_SA);
   SafeArrayDestroy(coords_SA);
}

void CAlgoComPolygon::addPolygon(CPolygon& polygon)
{
   CAlgoComPolygon algoComPolygon(polygon);

   addPolygon(algoComPolygon);
}

void CAlgoComPolygon::addPolygon(CAlgoComPolygon& algoComPolygon)
{
   try
   {
      IACGenPolygonPtr mergedAlgoComPolygon(__uuidof(AlgoCOMsGenPolygon));
      mergedAlgoComPolygon->Union(m_algoComPolygon,algoComPolygon.m_algoComPolygon);

      m_algoComPolygon = mergedAlgoComPolygon;
   }
   catch(...)
   {
      int iii = 3;
   }
}

int CAlgoComPolygon::getPolygonCount()
{
   int retval = m_algoComPolygon->GetNumberOfChains();

   return retval;
}

bool CAlgoComPolygon::getPolygonAt(CPolygon& polygon,int index)
{
   bool retval = false;

   try
   {
      if (index < m_algoComPolygon->GetNumberOfChains())
      {
         polygon.empty();

         LPSAFEARRAY R_polys = 0;
         m_algoComPolygon->GetBoundaryRep(&R_polys);

         long c_idx = index;

         IACPolygonPtr R_chain;
         SafeArrayGetElement(R_polys, &c_idx, &R_chain);

         LPSAFEARRAY xcoords = 0, ycoords = 0;
         R_chain->GetCoords(&xcoords, &ycoords);

         long ub;
         SafeArrayGetUBound(xcoords, 1, &ub);
         long v_idx;

         for (v_idx = 0; v_idx <= ub; ++v_idx) 
         {
            IACRationalPtr x,y;

            SafeArrayGetElement(xcoords, &v_idx, &x);
            SafeArrayGetElement(ycoords, &v_idx, &y);

            polygon.addVertex(CPoint2d(x->Approximate(),y->Approximate()));
         }

         SafeArrayDestroy(xcoords); 
         SafeArrayDestroy(ycoords);
         SafeArrayDestroy(R_polys);

         retval = true;
      }
   }
   catch(...)
   {
      int iii = 3;
   }

   return retval;
}

#endif  // defined(UseAlgoCOMsPolygonLibrary)
