// $Header: /CAMCAD/DcaLib/DcaArcPoint.cpp 5     5/21/07 3:47p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaArcPoint.h"
#include "DcaPnt.h"
#include "DcaGeomLib.h"
#include "DcaWriteFormat.h"
#include "DcaVector2d.h"
#include "DcaLine2d.h"
#include "DcaLib.h"
#include "DcaLibGraphics.h"
#include "DcaPoly.h"

//#define EnableArcPointDebugOutput

//_____________________________________________________________________________
CString arcLineJoinTypeToString(ArcLineJoinTypeTag tagValue)
{
   const char* stringValue;

   switch (tagValue)
   {
   case arcLineJoinTypeExtension:  stringValue = "Extension";  break;
   case arcLineJoinTypeIntersect:  stringValue = "Intersect";  break;
   case arcLineJoinTypeTangent:    stringValue = "Tangent";    break;
   case arcLineJoinTypeInvalid:    stringValue = "Invalid";    break;
   case arcLineJoinTypeUndefined:  stringValue = "Undefined";  break;
   default:                        stringValue = "Unknown";    break;
   }

   return stringValue;
}

//_____________________________________________________________________________
ArcLineEndTag oppositeArcLineEnd(ArcLineEndTag arcLineEnd)
{
   ArcLineEndTag oppositeArcLineEnd;

   switch (arcLineEnd)
   {
   case arcLineEndTail:  oppositeArcLineEnd = arcLineEndHead;       break;
   case arcLineEndHead:  oppositeArcLineEnd = arcLineEndTail;       break;
   default:              oppositeArcLineEnd = arcLineEndUndefined;  break;
   }

   return oppositeArcLineEnd;
}

ArcLineTeriorTag oppositeArcLineTerior(ArcLineTeriorTag arcLineTerior)
{
   ArcLineTeriorTag oppositeArcLineTerior;

   switch (arcLineTerior)
   {
   case arcLineInterior:  oppositeArcLineTerior = arcLineExterior;         break;
   case arcLineExterior:  oppositeArcLineTerior = arcLineInterior;         break;
   default:               oppositeArcLineTerior = arcLineTeriorUndefined;  break;
   }

   return oppositeArcLineTerior;
}

void ccwTangentPoint(ArcLineEndTag& arcLineEnd,ArcLineTeriorTag& arcLineTerior)
{
   if (arcLineEnd == arcLineEndTail)
   {
      if (arcLineTerior == arcLineInterior)
      {
         arcLineTerior = arcLineExterior;
      }
      else if (arcLineTerior == arcLineExterior)
      {
         arcLineEnd = arcLineEndHead;
      }
      else
      {
         arcLineEnd = arcLineEndUndefined;
      }
   }
   else if (arcLineEnd == arcLineEndHead)
   {
      if (arcLineTerior == arcLineInterior)
      {
         arcLineEnd = arcLineEndTail;
      }
      else if (arcLineTerior == arcLineExterior)
      {
         arcLineTerior = arcLineInterior;
      }
      else
      {
         arcLineEnd = arcLineEndUndefined;
      }
   }
}

ArcLineEndTag ccwArcLineEnd(ArcLineEndTag arcLineEnd,ArcLineTeriorTag arcLineTerior)
{
   ccwTangentPoint(arcLineEnd,arcLineTerior);

   return arcLineEnd;
}

ArcLineTeriorTag ccwTerior(ArcLineEndTag arcLineEnd,ArcLineTeriorTag arcLineTerior)
{
   ccwTangentPoint(arcLineEnd,arcLineTerior);

   return arcLineTerior;
}

//_____________________________________________________________________________
CArcPoint::CArcPoint(const CPnt& point)
: CPoint2d(point.x,point.y)
, bulge(point.bulge)
{
}

CArcPoint::CArcPoint(const CPoint2d& p0,const CPoint2d& p1,const CPoint2d& center)
: CPoint2d(p0.x,p0.y)
{
   double radius,startAngleRadians,deltaAngleRadians;

   ArcCenter2(p0.x,p0.y,p1.x,p1.y,center.x,center.y,&radius,&startAngleRadians,&deltaAngleRadians,false);

   bulge = tan(deltaAngleRadians/4.);
}

void CArcPoint::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"CArcPoint\n"
"{\n"
"   x     = %f\n"
"   y     = %f\n"
"   bulge = %f\n"
"}\n",
      x,y,bulge);
}

//_____________________________________________________________________________
CArcPointList::CArcPointList()
{
}

CArcPointList::~CArcPointList()
{
}

void CArcPointList::empty()
{
   m_points.empty();
}

int CArcPointList::getCount() const
{
   return m_points.GetCount();
}

void CArcPointList::addVertex(const CArcPoint& point)
{
   m_points.AddTail(new CArcPoint(point));
}

POSITION CArcPointList::getHeadPosition() const
{
   return m_points.GetHeadPosition();
}

CArcPoint* CArcPointList::getNext(POSITION& pos) const
{
   return m_points.GetNext(pos);
}

void CArcPointList::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"CArcPointList\n"
"{\n"
"   count=%d\n",
      m_points.GetCount());

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      int index = 0;

      for (POSITION pos = m_points.GetHeadPosition();pos != NULL;index++)
      {
         CArcPoint* arcPoint = m_points.GetNext(pos);

         writeFormat.writef("index = %d, pos=%p\n",index,pos);
         arcPoint->dump(writeFormat,depth);
      }

      writeFormat.popHeader();
   }

   writeFormat.writef("}\n");
}

//_____________________________________________________________________________
CArcLine::CArcLine(CArcPoint& tail,CArcPoint& head)
: m_tail(tail)
, m_head(head)
, m_parametersValid(false)
, m_arcAnglesValid(false)
, m_tangentPointsValid(false)
, m_width(0.)
, m_apShape(apertureRound)
{
}

CArcLine::CArcLine(const CArcLine& other)
: m_tail(other.m_tail)
, m_head(other.m_head)
, m_parametersValid(false)
, m_arcAnglesValid(false)
, m_tangentPointsValid(false)
, m_width(0.)
, m_apShape(apertureRound)
{
}

CArcLine& CArcLine::operator=(const CArcLine& other)
{
   if (&other != this)
   {
      m_tail = other.m_tail;
      m_head = other.m_head;

      m_parametersValid    = false;
      m_arcAnglesValid     = false;
      m_tangentPointsValid = false;
   }

   return *this;
}

bool CArcLine::isArc() const
{
   bool retval = (m_tail.bulge != 0.);

   return retval;
}

bool CArcLine::isLine() const
{
   return !isArc();
}

void CArcLine::calculateArcProperties() const
{
   if (! m_parametersValid)
   {
      m_parametersValid = true;

      CVector2d chord(m_tail,m_head);
      m_length = chord.getLength();

      if (isArc())
      {
         CPoint2d chordCenter((m_tail.x + m_head.x)/2.,(m_tail.y + m_head.y)/2.);
         CVector2d apothemVector(chordCenter,m_head);

         double halfLength = m_length / 2.;
         double bulgeMagnitude = fabs(getBulge());
         m_sagitta = bulgeMagnitude * halfLength;
         m_radius  = (halfLength*halfLength + m_sagitta*m_sagitta) / (2. * m_sagitta);
         m_apothem = fabs(m_radius - m_sagitta);

         apothemVector.makePerpendicular((getBulge() < 0.) != (bulgeMagnitude < 1.));
         apothemVector.setLength(m_apothem);

         m_arcCenter = apothemVector.getTip();
      }
      else
      {
         m_sagitta = 0.;
         m_radius  = 0.;
         m_apothem = 0.;
      }
   }
}

void CArcLine::calculateArcAngles() const
{
   if (! m_arcAnglesValid)
   {
      m_arcAnglesValid = true;

      CPoint2d arcCenter = getArcCenter();

      double dx = m_tail.x - arcCenter.x;
      double dy = m_tail.y - arcCenter.y;

      m_tailAngleRadians = atan2(dy,dx);

      dx = m_head.x - arcCenter.x;
      dy = m_head.y - arcCenter.y;

      m_headAngleRadians = atan2(dy,dx);
   }
}

CPoint2d CArcLine::getArcCenter() const
{
   calculateArcProperties();

   return m_arcCenter;
}

double CArcLine::getApothem() const
{
   calculateArcProperties();

   return m_apothem;
}

double CArcLine::getSagitta() const
{
   calculateArcProperties();

   return m_sagitta;
}

double CArcLine::getRadius() const
{
   calculateArcProperties();

   return m_radius;
}

double CArcLine::getTailAngleRadians() const
{
   calculateArcAngles();

   return m_tailAngleRadians;
}

double CArcLine::getHeadAngleRadians() const
{
   calculateArcAngles();

   return m_headAngleRadians;
}

double CArcLine::getStartAngleRadians() const
{
   double startAngle = (getBulge() < 0. ? getHeadAngleRadians() : getTailAngleRadians());

   return startAngle;
}

double CArcLine::getEndAngleRadians() const
{
   double endAngle = (getBulge() < 0. ? getTailAngleRadians() : getHeadAngleRadians());

   return endAngle;
}

bool CArcLine::setWidth(double width, ApertureShapeTag apShape)
{
   if (width > 0.)
   {
      int jj = 0;
   }
   if (width == 0.5)
   {
      int jj = 0;
   }
   if (apShape != m_apShape)
   {
      int jj = 0;
   }
   if (apShape != apertureRound)
   {
      int jj = 0;
   }

   if (width != m_width || apShape != m_apShape || !m_tangentPointsValid)
   {
      m_width = width;
      m_apShape = apShape;

      if (isArc())
      {
         double radius = getRadius();
         double halfWidth = width/2.;

         if (radius > halfWidth)
         {
            // calculate tangent points
            CPoint2d arcCenter = getArcCenter();
            double bulge = getBulge();

            if (bulge < 0.)
            {
               halfWidth = -halfWidth;
            }

            CVector2d interiorTailVector(arcCenter,m_tail);
            interiorTailVector.setLength(radius - halfWidth);
            CArcPoint interiorTailTangentPoint = interiorTailVector.getTip();
            interiorTailTangentPoint.bulge = 1.;

            CVector2d exteriorTailVector(arcCenter,m_tail);
            exteriorTailVector.setLength(radius + halfWidth);
            CArcPoint exteriorTailTangentPoint = exteriorTailVector.getTip();
            exteriorTailTangentPoint.bulge = bulge;

            CVector2d interiorHeadVector(arcCenter,m_head);
            interiorHeadVector.setLength(radius - halfWidth);
            CArcPoint interiorHeadTangentPoint = interiorHeadVector.getTip();
            interiorHeadTangentPoint.bulge = -bulge;

            CVector2d exteriorHeadVector(arcCenter,m_head);
            exteriorHeadVector.setLength(radius + halfWidth);
            CArcPoint exteriorHeadTangentPoint = exteriorHeadVector.getTip();
            exteriorHeadTangentPoint.bulge = 1.;

            setTangentPoint(arcLineEndTail,arcLineInterior,interiorTailTangentPoint);
            setTangentPoint(arcLineEndTail,arcLineExterior,exteriorTailTangentPoint);
            setTangentPoint(arcLineEndHead,arcLineInterior,interiorHeadTangentPoint);
            setTangentPoint(arcLineEndHead,arcLineExterior,exteriorHeadTangentPoint);

            m_tangentPointsValid = true;
         }
      }
      else if (apShape != apertureSquare)  // round and everything not square treated as round
      {
         // calculate tangent points
         double halfWidth = width/2.;

         CVector2d tailVector(m_tail,m_head);
         tailVector.makePerpendicular(true);

         tailVector.setLength( halfWidth);
         CArcPoint interiorTailTangentPoint = tailVector.getTip();
         tailVector.setLength(-halfWidth);
         CArcPoint exteriorTailTangentPoint = tailVector.getTip();

         CVector2d headVector(m_head,m_tail);
         headVector.makePerpendicular(true);

         headVector.setLength( halfWidth);
         CArcPoint exteriorHeadTangentPoint = headVector.getTip();
         headVector.setLength(-halfWidth);
         CArcPoint interiorHeadTangentPoint = headVector.getTip();

         interiorTailTangentPoint.bulge = 1.;
         exteriorHeadTangentPoint.bulge = 1.;

         setTangentPoint(arcLineEndTail,arcLineInterior,interiorTailTangentPoint);
         setTangentPoint(arcLineEndTail,arcLineExterior,exteriorTailTangentPoint);
         setTangentPoint(arcLineEndHead,arcLineExterior,exteriorHeadTangentPoint);
         setTangentPoint(arcLineEndHead,arcLineInterior,interiorHeadTangentPoint);

         m_tangentPointsValid = true;
      }
      else
      {
         // Square
         // calculate tangent points
         // A little coersion here, not really tangents but tangent list
         // is place data is later accessed from.

         double halfWidth = width/2.;

         // Extend vector by half width
         CVector2d tempTailVector(m_tail, m_head);
         double originalLength = tempTailVector.getLength();
         tempTailVector.setLength(originalLength + halfWidth);
         CPoint2d extendedHeadPt = tempTailVector.getTip();

         CVector2d tempHeadVector(m_head, m_tail);
         tempHeadVector.setLength(originalLength + halfWidth);
         CPoint2d extendedTailPt = tempHeadVector.getTip();

         // Get "tangents"
         //CVector2d tailVector(m_tail,m_head);
         CVector2d tailVector(extendedTailPt, extendedHeadPt);
         tailVector.makePerpendicular(true);

         tailVector.setLength( halfWidth);
         CArcPoint interiorTailTangentPoint = tailVector.getTip();
         tailVector.setLength(-halfWidth);
         CArcPoint exteriorTailTangentPoint = tailVector.getTip();

         //CVector2d headVector(m_head,m_tail);
         CVector2d headVector(extendedHeadPt, extendedTailPt);
         headVector.makePerpendicular(true);

         headVector.setLength( halfWidth);
         CArcPoint exteriorHeadTangentPoint = headVector.getTip();
         headVector.setLength(-halfWidth);
         CArcPoint interiorHeadTangentPoint = headVector.getTip();

         //interiorTailTangentPoint.bulge = 1.;
         //exteriorHeadTangentPoint.bulge = 1.;

         setTangentPoint(arcLineEndTail,arcLineInterior,interiorTailTangentPoint);
         setTangentPoint(arcLineEndTail,arcLineExterior,exteriorTailTangentPoint);
         setTangentPoint(arcLineEndHead,arcLineExterior,exteriorHeadTangentPoint);
         setTangentPoint(arcLineEndHead,arcLineInterior,interiorHeadTangentPoint);

         m_tangentPointsValid = true;

      }
   }

   return m_tangentPointsValid;
}

void CArcLine::moveEnd(ArcLineEndTag movingEnd,const CPoint2d& newEndPoint)
{
   bool movingEndIsTail = (movingEnd == arcLineEndTail);
   bool movingEndIsHead = (movingEnd == arcLineEndHead);

   if (movingEndIsTail || movingEndIsHead)
   {
      if (isLine())
      {
         if (movingEndIsTail)
         {
            m_tail.x = newEndPoint.x;
            m_tail.y = newEndPoint.y;
         }
         else
         {
            m_head.x = newEndPoint.x;
            m_head.y = newEndPoint.y;
         }
      }
      else
      {
         CArcPoint& stationaryEnd = (movingEndIsTail ? m_head : m_tail);
         CArcPoint& movingEnd     = (movingEndIsTail ? m_tail : m_head);

         CPoint2d arcCenter = getArcCenter();
         double radius = getRadius();
         double bulge  = getBulge();

         CLine2d stationaryRadialSegment(arcCenter,stationaryEnd);
         double stationaryTheta = stationaryRadialSegment.getTheta();

         CLine2d newRadialSegment(arcCenter,newEndPoint);
         double newTheta = newRadialSegment.getTheta();

         while (bulge > 0. && newTheta < stationaryTheta) newTheta += TwoPi;
         while (bulge < 0. && newTheta > stationaryTheta) newTheta -= TwoPi;

         double newDeltaAngle = newTheta - stationaryTheta;

         movingEnd = newEndPoint;
         m_tail.bulge = tan(newDeltaAngle/4.);

         if (m_tail.bulge * bulge < 0.)
         {
            newDeltaAngle = TwoPi - newDeltaAngle;
            m_tail.bulge = tan(newDeltaAngle/4.);
         }

         if ((abs(m_tail.bulge) > 1.) != (abs(bulge > 1.)))
         {
            newDeltaAngle = TwoPi - newDeltaAngle;
            m_tail.bulge = tan(newDeltaAngle/4.);
         }
      }

      m_tangentPointsValid = false;
   }
}

CArcPoint CArcLine::getTangentPoint(ArcLineEndTag arcLineEnd,ArcLineTeriorTag arcLineTerior) const
{
   return m_tangentPoints[arcLineEnd][arcLineTerior];
}

CArcPoint& CArcLine::getJoinPoint(ArcLineEndTag arcLineEnd,ArcLineTeriorTag arcLineTerior) const
{
   return m_joinPoints[arcLineEnd][arcLineTerior];
}

// gets the boundary arc line.
// The end and tail references of the arc line are set to the join points and are initialized to the tangent points.
CArcLine CArcLine::getBoundaryArcLine(ArcLineEndTag arcLineEndForTail,ArcLineTeriorTag arcLineTeriorForTail) const
{
   if (m_tangentPointsValid)
   {
      CArcPoint tailTangentPoint = getTangentPoint(arcLineEndForTail,arcLineTeriorForTail);
      CArcPoint& tailJoinPoint   = getJoinPoint(arcLineEndForTail,arcLineTeriorForTail);
      tailJoinPoint = tailTangentPoint;

      ArcLineEndTag    arcLineEndForHead    = arcLineEndForTail;
      ArcLineTeriorTag arcLineTeriorForHead = arcLineTeriorForTail;

      ccwTangentPoint(arcLineEndForHead,arcLineTeriorForHead);

      CArcPoint headTangentPoint = getTangentPoint(arcLineEndForHead,arcLineTeriorForHead);
      CArcPoint& headJoinPoint   = getJoinPoint(arcLineEndForHead,arcLineTeriorForHead);
      headJoinPoint = headTangentPoint;

      return CArcLine(tailJoinPoint,headJoinPoint);
   }
   else
   {
      formatMessageBox("Logic error in CArcLine::getBoundaryArcLine()");
      return *this;
   }
}

void CArcLine::setTangentPoint(ArcLineEndTag arcLineEnd,ArcLineTeriorTag arcLineTerior,const CArcPoint& point)
{
   m_tangentPoints[arcLineEnd][arcLineTerior] = point;
}

int CArcLine::intersects(const CArcLine& otherArcLine,CPoint2d& intersectPoint0,CPoint2d& intersectPoint1) const
{
   int retval = 0;

   if (isLine())
   {
      if (otherArcLine.isLine())
      {
         CLine2d line(m_head,m_tail);
         CLine2d otherLine(otherArcLine.m_head,otherArcLine.m_tail);

         retval = line.intersect(otherLine,intersectPoint0,intersectPoint1);
      }
      else
      {
         int num;

         retval = intersectLineToCircle(intersectPoint0.x,intersectPoint0.y,
                                        intersectPoint1.x,intersectPoint1.y,num,
                                        m_tail.x,m_tail.y,m_head.x,m_head.y,
                                        otherArcLine.getArcCenter().x,otherArcLine.getArcCenter().y,
                                        otherArcLine.getRadius(),
                                        otherArcLine.getStartAngleRadians(),otherArcLine.getEndAngleRadians());
      }
   }
   else
   {
      if (otherArcLine.isLine())
      {
         int num;

         retval = intersectLineToCircle(intersectPoint0.x,intersectPoint0.y,
                                        intersectPoint1.x,intersectPoint1.y,num,
                                        otherArcLine.m_tail.x,otherArcLine.m_tail.y,otherArcLine.m_head.x,otherArcLine.m_head.y,
                                        getArcCenter().x,getArcCenter().y,
                                        getRadius(),getStartAngleRadians(),getEndAngleRadians());
      }
      else
      {
         retval = intersectCircleToCircle(intersectPoint0.x,intersectPoint0.y,
                                          intersectPoint1.x,intersectPoint1.y,
                                          getArcCenter().x,getArcCenter().y,
                                          getRadius(),
                                          getStartAngleRadians(),getEndAngleRadians(),
                                          otherArcLine.getArcCenter().x,otherArcLine.getArcCenter().y,
                                          otherArcLine.getRadius(),
                                          otherArcLine.getStartAngleRadians(),otherArcLine.getEndAngleRadians());
      }
   }

   return retval;
}

void CArcLine::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"CArcLine\n"
"{\n");

   writeFormat.pushHeader("   ");

   writeFormat.writef("m_tail\n");
   m_tail.dump(writeFormat,depth);

   writeFormat.writef("m_head\n");
   m_head.dump(writeFormat,depth);

   writeFormat.writef("m_parametersValid = %d\n",m_parametersValid);

   if (m_parametersValid)
   {
      writeFormat.writef("m_length = %f\n",m_length);
      writeFormat.writef("m_radius = %f\n",m_radius);
      writeFormat.writef("m_sagitta = %f\n",m_sagitta);
      writeFormat.writef("m_apothem = %f\n",m_apothem);

      writeFormat.writef("m_arcCenter\n");
      m_arcCenter.dump(writeFormat,depth);
   }

   writeFormat.writef("m_width = %f\n",m_width);
   writeFormat.writef("m_tangentPointsValid = %d\n",m_tangentPointsValid);

   if (m_tangentPointsValid)
   {
      writeFormat.writef("m_tangentPoints[0][0]\n");
      m_tangentPoints[0][0].dump(writeFormat,depth);

      writeFormat.writef("m_tangentPoints[0][1]\n");
      m_tangentPoints[0][1].dump(writeFormat,depth);

      writeFormat.writef("m_tangentPoints[1][0]\n");
      m_tangentPoints[1][0].dump(writeFormat,depth);

      writeFormat.writef("m_tangentPoints[1][1]\n");
      m_tangentPoints[1][1].dump(writeFormat,depth);
   }

   writeFormat.popHeader();

   writeFormat.writef("}\n");
}

//_____________________________________________________________________________
void CArcLineArray::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"CArcLineArray\n"
"{\n"
"   size=%d\n",
      GetSize());

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      for (int index = 0;index < GetSize();index++)
      {
         CArcLine* arcLine = GetAt(index);

         writeFormat.writef("index = %d\n",index);
         arcLine->dump(writeFormat,depth);
      }

      writeFormat.popHeader();
   }

   writeFormat.writef("}\n");
}

//_____________________________________________________________________________
CPolyArcLine::CPolyArcLine()
: m_lines(NULL)
, m_closed(false)
{
}

CPolyArcLine::CPolyArcLine(CPoly& poly)
: m_lines(NULL)
{
   for (POSITION pos = poly.getPntList().GetHeadPosition();pos != NULL;)
   {
      CPnt* pnt = poly.getPntList().GetNext(pos);

      m_vertices.addVertex(CArcPoint(*pnt));
   }

   m_closed = poly.isClosed();
}

CPolyArcLine::~CPolyArcLine()
{
   deallocateLines();
}

void CPolyArcLine::empty()
{
   m_vertices.empty();
   deallocateLines();
}

int CPolyArcLine::getLineCount() const
{
   return max(0,m_vertices.getCount() - 1);
}

int CPolyArcLine::getVertexCount() const
{
   return m_vertices.getCount();
}

void CPolyArcLine::allocateLines()
{
   if (m_lines == NULL)
   {
      m_lines = new CArcLineArray(max(1,m_vertices.getCount()));

      CArcPoint* previousPoint = NULL;

      for (POSITION pos = m_vertices.getHeadPosition();pos != NULL;)
      {
         CArcPoint* point = m_vertices.getNext(pos);

         if (previousPoint != NULL)
         {
            CArcLine* arcLine = new CArcLine(*previousPoint,*point);
            m_lines->Add(arcLine);
         }

         previousPoint = point;
      }
   }
}

void CPolyArcLine::deallocateLines()
{
   delete m_lines;
   m_lines = NULL;
}

CArcLine* CPolyArcLine::getLineAt(int index)
{
   CArcLine* line = NULL;

   allocateLines();

   if (index < 0 || index >= m_lines->GetSize())
   {
      formatMessageBox("Logic error in CPolyArcLine::getAt()");
   }
   else
   {
      line = m_lines->GetAt(index);
   }

   return line;
}

ArcLineJoinTypeTag CPolyArcLine::getJoin(CArcPoint& joinPoint,const CArcLine& previousArcLine,const CArcLine& arcLine,
   ArcLineTeriorTag terior,double tolerance)
{
   ArcLineJoinTypeTag joinType = arcLineJoinTypeUndefined;

   if (terior == arcLineInterior || terior == arcLineExterior)
   {
      ArcLineEndTag end = ((terior == arcLineInterior) ? arcLineEndHead : arcLineEndTail);

      CArcLine previousBoundaryArcLine = previousArcLine.getBoundaryArcLine(end,terior);
      CArcLine boundaryArcLine         = arcLine.getBoundaryArcLine(end,terior);

      if (previousBoundaryArcLine.getHead().fpnear(boundaryArcLine.getTail(),tolerance))
      {
         joinType  = arcLineJoinTypeTangent;
         joinPoint = boundaryArcLine.getTail();
      }
      else
      {
         CVector2d head(previousArcLine.getHead(),previousArcLine.getTail());
         CVector2d tangent(arcLine.getTail(),arcLine.getTangentPoint(arcLineEndTail,terior));

         double dotProduct = head.dotProduct(tangent);

         if (dotProduct > 0.)
         {
            CPoint2d point0,point1;
            int numIntersections = previousBoundaryArcLine.intersects(boundaryArcLine,point0,point1);

            if (numIntersections == 2 && point0.fpnear(point1,tolerance))
            {
               numIntersections = 1;
            }

            if (numIntersections == 0)
            {
               joinType  = arcLineJoinTypeTangent;
               joinPoint = boundaryArcLine.getTail();
            }
            else if (numIntersections == 1)
            {
               joinType  = arcLineJoinTypeIntersect;
               joinPoint = point0;
            }
         }
         else
         {
            joinType  = arcLineJoinTypeExtension;
            joinPoint = previousBoundaryArcLine.getTangentPoint(arcLineEndHead,terior);
         }
      }
   }

   return joinType;
}

bool CPolyArcLine::setWidth(double width, ApertureShapeTag apShape)
{
   bool retval = true;

   //*rcf BUG Really only start endcap and end endcap should be square in case of square
   // aps, so for every line really the ap shape ought to be separate for start and end,
   // depending on if it is actual endpont of a joint end.
   // But for now we'll just treat it as one ap for first and last seg.
   // This will cause a glitch in first and last joints.

   for (int lineIndex = 0;lineIndex < getLineCount() && retval;lineIndex++)
   {
      CArcLine* arcLine = getLineAt(lineIndex);

      ApertureShapeTag activeApShape = apertureRound; // default and correct for all internal joints
      if (lineIndex == 0 || (lineIndex + 1) == getLineCount()) // if first or last then use apShape param
         activeApShape = apShape;

      retval = arcLine->setWidth(width, activeApShape);
   }

   return retval;
}

bool CPolyArcLine::getOutline(CArcLinePolygon& polygon,double width,double tolerance, ApertureShapeTag apShape)
{
   return getTeriorLine(polygon, width, tolerance, true, apShape);
}

bool CPolyArcLine::getInline(CArcLinePolygon& polygon,double width,double tolerance)
{
   return getTeriorLine(polygon,width,tolerance,false);
}

bool CPolyArcLine::getTeriorLine(CArcLinePolygon& polygon,double width,double tolerance,bool outlineFlag, ApertureShapeTag apShape)
{
   // Only round and square aperture shapes currently supported.
   // Need more size params for other shapes.
   // If it comes to that then probably best to arrange parameter list to just pass
   // whole width BlockStruct along, since all params are there, rather than go
   // nuts with multiple size params.

   bool retval = false;

   polygon.empty();

   if (setWidth(width, apShape))
   {
#if defined(EnableArcPointDebugOutput)
      dump(CDebugWriteFormat::getWriteFormat());
#endif

      if (getLineCount() == 1)
      {
         CArcLine* arcLine = getLineAt(0);

         ArcLineEndTag end = arcLineEndTail;
         ArcLineTeriorTag terior = arcLineExterior;

         polygon.addVertex(arcLine->getTangentPoint(end,terior));

         ccwTangentPoint(end,terior);

         polygon.addVertex(arcLine->getTangentPoint(end,terior));

         ccwTangentPoint(end,terior);

         polygon.addVertex(arcLine->getTangentPoint(end,terior));

         ccwTangentPoint(end,terior);

         polygon.addVertex(arcLine->getTangentPoint(end,terior));

         retval = true;
      }
      else if (getLineCount() > 1)
      {
         retval = true;

         if (! m_closed)
         {
#if defined(EnableArcPointDebugOutput)
            CDebugWriteFormat::getWriteFormat().writef("\nCPolyArcLine::getOutline() - open polygon\n");
#endif

            CArcPoint joinPoint;
            CArcLine* previousArcLine = getLineAt(0);
            CArcLine* arcLine;

            polygon.addVertex(previousArcLine->getTangentPoint(arcLineEndTail,arcLineInterior));
            polygon.addVertex(previousArcLine->getTangentPoint(arcLineEndTail,arcLineExterior));

            for (int lineIndex = 1;lineIndex < getLineCount();lineIndex++)
            {
               arcLine = getLineAt(lineIndex);

               ArcLineJoinTypeTag joinType = getJoin(joinPoint,*previousArcLine,*arcLine,arcLineExterior,tolerance);

#if defined(EnableArcPointDebugOutput)
               CDebugWriteFormat::getWriteFormat().writef(
                  "\nCPolyArcLine::getOutline() - lineIndex = %d, joinType = %d [%s]\n",
                  lineIndex,joinType,arcLineJoinTypeToString(joinType));
#endif

               if (joinType == arcLineJoinTypeInvalid)
               {
                  retval = false;
                  break;
               }
               else if (joinType == arcLineJoinTypeIntersect)
               {
                  CArcLine boundaryArcLine = arcLine->getBoundaryArcLine(arcLineEndTail,arcLineExterior);
                  boundaryArcLine.moveEnd(arcLineEndTail,joinPoint);

                  polygon.addVertex(boundaryArcLine.getTail());
               }
               else if (joinType == arcLineJoinTypeTangent)
               {
                  CArcLine boundaryArcLine = arcLine->getBoundaryArcLine(arcLineEndTail,arcLineExterior);

                  polygon.addVertex(boundaryArcLine.getTail());
               }
               else if (joinType == arcLineJoinTypeExtension)
               {
                  CArcLine boundaryArcLine = previousArcLine->getBoundaryArcLine(arcLineEndHead,arcLineExterior);
                  boundaryArcLine.moveEnd(arcLineEndHead,arcLine->getTangentPoint(arcLineEndTail,arcLineExterior));

                  polygon.addVertex(boundaryArcLine.getTail());
                  polygon.addVertex(arcLine->getTangentPoint(arcLineEndTail,arcLineExterior));
               }

               previousArcLine = arcLine;
            }

            if (retval)
            {
               polygon.addVertex(previousArcLine->getTangentPoint(arcLineEndHead,arcLineExterior));
               polygon.addVertex(previousArcLine->getTangentPoint(arcLineEndHead,arcLineInterior));

               for (int lineIndex = getLineCount() - 2;lineIndex >= 0;lineIndex--)
               {
                  previousArcLine = getLineAt(lineIndex);

                  ArcLineJoinTypeTag joinType = getJoin(joinPoint,*previousArcLine,*arcLine,arcLineInterior,tolerance);

#if defined(EnableArcPointDebugOutput)
                  CDebugWriteFormat::getWriteFormat().writef(
                     "\nCPolyArcLine::getOutline() - lineIndex = %d, joinType = %d [%s]\n",
                     lineIndex,joinType,arcLineJoinTypeToString(joinType));
#endif

                  if (joinType == arcLineJoinTypeInvalid)
                  {
                     retval = false;
                     break;
                  }
                  else if (joinType == arcLineJoinTypeIntersect)
                  {
                     CArcLine boundaryArcLine = previousArcLine->getBoundaryArcLine(arcLineEndHead,arcLineInterior);
                     boundaryArcLine.moveEnd(arcLineEndTail,joinPoint);

                     polygon.addVertex(boundaryArcLine.getTail());
                  }
                  else if (joinType == arcLineJoinTypeTangent)
                  {
                     CArcLine boundaryArcLine = previousArcLine->getBoundaryArcLine(arcLineEndHead,arcLineInterior);

                     polygon.addVertex(boundaryArcLine.getTail());
                  }
                  else if (joinType == arcLineJoinTypeExtension)
                  {
                     CArcLine boundaryArcLine = arcLine->getBoundaryArcLine(arcLineEndTail,arcLineInterior);
                     boundaryArcLine.moveEnd(arcLineEndHead,previousArcLine->getTangentPoint(arcLineEndHead,arcLineInterior));

                     polygon.addVertex(boundaryArcLine.getTail());
                     polygon.addVertex(previousArcLine->getTangentPoint(arcLineEndHead,arcLineInterior));
                  }

                  arcLine = previousArcLine;
               }
            }
         }
         else // closed
         {
            if (outlineFlag)
            {
               CArcPoint joinPoint;
               CArcLine* previousArcLine = getLineAt(getLineCount() - 1);
               CArcLine* arcLine;

               for (int lineIndex = 0;lineIndex < getLineCount();lineIndex++)
               {
                  arcLine = getLineAt(lineIndex);

                  ArcLineJoinTypeTag joinType = getJoin(joinPoint,*previousArcLine,*arcLine,arcLineExterior,tolerance);

#if defined(EnableArcPointDebugOutput)
                  CDebugWriteFormat::getWriteFormat().writef(
                     "\nCPolyArcLine::getOutline() - lineIndex = %d, joinType = %d [%s]\n",
                     lineIndex,joinType,arcLineJoinTypeToString(joinType));
#endif

                  if (joinType == arcLineJoinTypeInvalid)
                  {
                     retval = false;
                     break;
                  }
                  else if (joinType == arcLineJoinTypeIntersect)
                  {
                     CArcLine boundaryArcLine = arcLine->getBoundaryArcLine(arcLineEndTail,arcLineExterior);
                     boundaryArcLine.moveEnd(arcLineEndTail,joinPoint);

                     polygon.addVertex(boundaryArcLine.getTail());
                  }
                  else if (joinType == arcLineJoinTypeTangent)
                  {
                     CArcLine boundaryArcLine = arcLine->getBoundaryArcLine(arcLineEndTail,arcLineExterior);

                     polygon.addVertex(boundaryArcLine.getTail());
                  }
                  else if (joinType == arcLineJoinTypeExtension)
                  {
                     CArcLine boundaryArcLine = previousArcLine->getBoundaryArcLine(arcLineEndHead,arcLineExterior);
                     boundaryArcLine.moveEnd(arcLineEndHead,arcLine->getTangentPoint(arcLineEndTail,arcLineExterior));

                     polygon.addVertex(boundaryArcLine.getTail());
                     polygon.addVertex(arcLine->getTangentPoint(arcLineEndTail,arcLineExterior));
                  }

                  previousArcLine = arcLine;
               }
            }
            else
            {
               CArcPoint joinPoint;
               CArcLine* previousArcLine;
               CArcLine* arcLine = getLineAt(0);

               for (int lineIndex = getLineCount() - 1;lineIndex >= 0;lineIndex--)
               {
                  previousArcLine = getLineAt(lineIndex);

                  ArcLineJoinTypeTag joinType = getJoin(joinPoint,*previousArcLine,*arcLine,arcLineInterior,tolerance);

#if defined(EnableArcPointDebugOutput)
                  CDebugWriteFormat::getWriteFormat().writef(
                     "\nCPolyArcLine::getOutline() - lineIndex = %d, joinType = %d [%s]\n",
                     lineIndex,joinType,arcLineJoinTypeToString(joinType));
#endif

                  if (joinType == arcLineJoinTypeInvalid)
                  {
                     retval = false;
                     break;
                  }
                  else if (joinType == arcLineJoinTypeIntersect)
                  {
                     CArcLine boundaryArcLine = previousArcLine->getBoundaryArcLine(arcLineEndHead,arcLineInterior);
                     boundaryArcLine.moveEnd(arcLineEndTail,joinPoint);

                     polygon.addVertex(boundaryArcLine.getTail());
                  }
                  else if (joinType == arcLineJoinTypeTangent)
                  {
                     CArcLine boundaryArcLine = previousArcLine->getBoundaryArcLine(arcLineEndHead,arcLineInterior);

                     polygon.addVertex(boundaryArcLine.getTail());
                  }
                  else if (joinType == arcLineJoinTypeExtension)
                  {
                     CArcLine boundaryArcLine = arcLine->getBoundaryArcLine(arcLineEndTail,arcLineInterior);
                     boundaryArcLine.moveEnd(arcLineEndHead,previousArcLine->getTangentPoint(arcLineEndHead,arcLineInterior));

                     polygon.addVertex(boundaryArcLine.getTail());
                     polygon.addVertex(previousArcLine->getTangentPoint(arcLineEndHead,arcLineInterior));
                  }

                  arcLine = previousArcLine;
               }
            }
         }
      }
   }

   return retval;
}

void CPolyArcLine::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"CPolyArcLine\n"
"{\n"
"   m_closed=%d\n",
      m_closed);

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      writeFormat.writef("m_vertices\n");
      m_vertices.dump(writeFormat,depth);

      writeFormat.writef("m_lines = 0x%p\n",m_lines);

      if (m_lines != NULL)
      {
         m_lines->dump(writeFormat,depth);
      }

      writeFormat.popHeader();
   }

   writeFormat.writef("}\n");
}

//_____________________________________________________________________________
CArcLinePolygon::CArcLinePolygon()
{
}

CArcLinePolygon::~CArcLinePolygon()
{
}

void CArcLinePolygon::empty()
{
   m_vertices.empty();
}

void CArcLinePolygon::addVertex(const CArcPoint& point)
{
#if defined(EnableArcPointDebugOutput)
   CDebugWriteFormat::getWriteFormat().writef("\nCArcLinePolygon::addVertex()\n");
   point.dump(CDebugWriteFormat::getWriteFormat());
#endif

   m_vertices.addVertex(point);
}

void CArcLinePolygon::getPoly(CPoly& poly)
{
   poly.empty();
   CArcPoint* firstArcPoint = NULL;

   for (POSITION pos = m_vertices.getHeadPosition();pos != NULL;)
   {
      CArcPoint* arcPoint = m_vertices.getNext(pos);

      if (firstArcPoint == NULL)
      {
         firstArcPoint = arcPoint;
      }

      poly.addVertex(arcPoint->x,arcPoint->y,arcPoint->bulge);
   }

   if (firstArcPoint != NULL)
   {
      poly.addVertex(firstArcPoint->x,firstArcPoint->y);
   }

   poly.setClosed(true);
}






