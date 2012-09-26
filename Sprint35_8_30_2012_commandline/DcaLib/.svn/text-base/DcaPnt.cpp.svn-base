// $Header: /CAMCAD/DcaLib/DcaPnt.cpp 7     6/21/07 8:29p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaPnt.h"
#include "DcaGeomLib.h"
#include "DcaTMatrix.h"
#include "DcaPoint2d.h"
#include "DcaExtent.h"
#include "DcaWriteFormat.h"
#include "DcaGeomLib.h"
#include "DcaLib.h"
#include "DcaBuildingBlockLib.h"
#include "DcaPoint2.h"
#include "DcaUnits.h"

//_____________________________________________________________________________
CPnt& CPnt::operator=(const CPnt& other)
{
   x     = other.x;
   y     = other.y;
   bulge = other.bulge;

   return *this;
}

double CPnt::getChordArea(const CPnt& nextPnt) const
{
   double chordArea = 0.;

   if (!fpeq(bulge,0.))
   {
      double startAngle,radius,xCenter,yCenter;
      double deltaAngle = atan(bulge) * 4;
      ArcPoint2Angle(x,y,nextPnt.x,nextPnt.y,
         deltaAngle,&xCenter,&yCenter,&radius,&startAngle);

      chordArea = .5 * radius * radius * (deltaAngle - sin(deltaAngle));
   }

   return chordArea;
}

double CPnt::getLength(const CPnt& nextPnt) const
{
   double length = 0.;

   if (!fpeq(bulge,0.))
   {
      double startAngle,radius,xCenter,yCenter;
      double deltaAngle = atan(bulge) * 4;
      ArcPoint2Angle(x,y,nextPnt.x,nextPnt.y,
         deltaAngle,&xCenter,&yCenter,&radius,&startAngle);

      length = fabs(radius * deltaAngle);
   }
   else
   {
      length = _hypot(nextPnt.x - x,nextPnt.y - y);
   }

   return length;
}

void CPnt::transform(const CTMatrix& transformationMatrix)
{
   transformationMatrix.transform(x,y);

   if (transformationMatrix.getMirror())
   {
      bulge = -bulge;
   }
}

void CPnt::dump(CWriteFormat& writeFormat,int depth) const
{
   if (bulge == 0.)
   {
   writeFormat.writef(
"CPnt\n"
"{\n"
"   (x,y)=(%.3f,%.3f)\n"
"}\n",
x,y);
   }
   else
   {
   writeFormat.writef(
"CPnt\n"
"{\n"
"   (x,y,bulge)=(%.3f,%.3f,%.3f)\n"
"}\n",
x,y,bulge);
   }
}

//_____________________________________________________________________________
CPntList::CPntList()
{
}

CPntList::CPntList(const CPntList& other)
{
   for (POSITION pos = other.GetHeadPosition();pos != NULL;)
   {
      CPnt* pnt = other.GetNext(pos);

      if (pnt != NULL)
      {
         CPnt* pntCopy = new CPnt(*pnt);

         AddTail(pntCopy);
      }
   }
}

CPntList& CPntList::operator=(const CPntList& other)
{
   if (&other != this)
   {
      empty();

      for (POSITION pos = other.GetHeadPosition();pos != NULL;)
      {
         CPnt* pnt = other.GetNext(pos);

         AddTail(new CPnt(*pnt));
      }
   }

   return *this;
}

void CPntList::transform(const CTMatrix& transformationMatrix)
{
   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CPnt* pnt = GetNext(pos);

      if (pnt != NULL)
      {
         pnt->transform(transformationMatrix);
      }
   }
}

void CPntList::updateExtent(CExtent& extent,const CTMatrix* transformationMatrix) const
{
   const double piOver2 = Pi / 2.;
   const double twoPi   = Pi * 2.;
   CPnt* previousPnt = NULL;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CPnt* pnt = GetNext(pos);
      CPoint2d point(pnt->x,pnt->y);

      if (transformationMatrix != NULL)
      {
         transformationMatrix->transform(point);
      }

      extent.update(point);

      if (previousPnt != NULL && !fpeq(previousPnt->bulge,0.)) 
      {
         double startAngle,radius,xCenter,yCenter;
         double deltaAngle = atan(previousPnt->bulge) * 4;
         ArcPoint2Angle(previousPnt->x,previousPnt->y,pnt->x,pnt->y,deltaAngle,&xCenter,&yCenter,&radius,&startAngle);

         double a0      = fmod(fmod(startAngle,twoPi) + (twoPi*(deltaAngle>=0?1:-1)),twoPi);
         deltaAngle     = fmod(fmod(deltaAngle,twoPi) + (twoPi*(deltaAngle>=0?1:-1)),twoPi);
         double a1      = a0 + deltaAngle;

         while (a1 > twoPi || a0 > twoPi)
         {
            a1 -= twoPi;
            a0 -= twoPi;
         }
         while (a1 < 0 || a0 < 0)
         {
            a1 += twoPi;
            a0 += twoPi;
         }

         if (deltaAngle < 0)
         {
            double tmp = a0;
            a0 = a1;
            a1 = tmp;
         }

         double orthogonalAngle;
         int cosA = 1;
         int sinA = 0;
         int tmp;

         for (int index = 0;index < 8;index++)
         {
            orthogonalAngle = index * piOver2;

            if (orthogonalAngle >= a0)
            {
               if (orthogonalAngle <= a1)
               {
                  CPoint2d point(xCenter + radius*cosA,yCenter + radius*sinA);

                  if (transformationMatrix != NULL)
                  {
                     transformationMatrix->transform(point);
                  }

                  extent.update(point);
               }
               else
               {
                  break;
               }
            }

            tmp  = sinA;
            sinA = cosA;
            cosA = -tmp;
         }
      }

      previousPnt = pnt;
   }
}

CExtent CPntList::getExtent() const
{
   CExtent extent;

   updateExtent(extent);

   return extent;
}

CExtent CPntList::getExtent(const CTMatrix& transformationMatrix) const
{
   CExtent extent;

   updateExtent(extent,transformationMatrix);

   return extent;
}

bool CPntList::isSimpleCircle(double& xCenter,double& yCenter,double& radius) const
{
   const CPntList* pntList = this;

   if (pntList->GetCount() != 3)
      return false;

   CPnt *a, *b, *c;
   POSITION pos = pntList->GetHeadPosition();
   a = pntList->GetNext(pos);
   b = pntList->GetNext(pos);
   c = pntList->GetNext(pos);
   
   if (fabs(a->x - c->x) > SMALLNUMBER)
      return false;

   if (fabs(a->y - c->y) > SMALLNUMBER)
      return false;

   if (fabs(fabs(a->bulge) - 1) > SMALLNUMBER)
      return false;

   if (fabs(fabs(b->bulge) - 1) > SMALLNUMBER)
      return false;

   if (fabs(a->bulge - b->bulge) > SMALLNUMBER) // both bulge numbers must be -1 or both must be +1
      return false;

   xCenter = (a->x + b->x) / 2;
   yCenter = (a->y + b->y) / 2;
   
   // float accurary may be not good enough
   double px = a->x - xCenter;
   double py = a->y - yCenter;

   radius = sqrt((px * px) + (py * py));

   return true;
}

/*******************************************************
* int Is_Pnt_Inside_PntList(CPntList *list, CPnt *p)
*
* Description:
*              --- checks if point on poly, returns -1
*              --- sums angles of triangles to determine if inside poly  
*              --- assumes all polylines do not have bulges, i.e. considers
*                  all polylines with bulges to be straight line segments.
*
* Parameters:
*              --- poly -> one poly
*              --- p ->  pnt which need
*                           to check inside or outside or on poly
* Notes:
*              --- point p with every two points (each line) become
*                  to a triangle and the number of triangles is equar to
*                  the number of lines.
*              --- Angle is the angle of 
*                  (last_point)-->(p)-->(current_point)
*              --- Sum of all Angles is always equar to 0
*                  when the point p is outside 
*
* Return:      --- 1
*                        if point p is inside 
*
*              --- 0 
*                        if point p is outside 
*
*              --- -1 
*                        if point p on poly
*
*  Originally from:
*  int Is_Pnt_Inside_PntList(const CPntList* list, double xPoint, double yPoint, double tolerance)                            
*
*********************************************************/     
int CPntList::isPointInside(double xPoint,double yPoint,PageUnitsTag pageUnits,double tolerance) const                          
{
   const CPntList* list = this;
	double cx, cy, radius;

	if (isSimpleCircle(cx,cy,radius))
	{
      if (Length(cx, cy, xPoint, yPoint) < radius)
      {
			return 1;
      }
	}
	
	if (list->GetCount() == 3)
	{
		POSITION pos = list->GetHeadPosition();
		CPnt *first_pnt = list->GetNext(pos);
		CPnt *second_pnt = list->GetNext(pos);
		CPnt *last_pnt = list->GetNext(pos);

		if (fabs(first_pnt->x - last_pnt->x) < SMALLNUMBER && fabs(first_pnt->y - last_pnt->y) < SMALLNUMBER)
		{
			Point2 first, second, point;
			first.x = first_pnt->x;
			first.y = first_pnt->y;
			second.x = second_pnt->x;
			second.y = second_pnt->y;
			point.x = xPoint;
			point.y = yPoint;

			if (Point2InSeg(&first, &second, &point, tolerance))
			{
				if ((first_pnt->bulge > 0 && second_pnt->bulge > 0) || first_pnt->bulge < 0 && second_pnt->bulge < 0)
				{
					// the point is on the line and this is circle so it is in the list
					return 1;
				}
				else
				{
					// the point is on the line and this is NOT a circle so it is NOT in the list
					return 0;
				}
			}
		}
	}

	CPnt *current_pnt, // the point of polygon which current get
			*last_pnt;  // the point of polygon which before current one
	double angle1,angle2, // the Angle of (last_point)-->(p)-->(current_point)
	// if it is counter clockwise then   0 < angle < Pi
	// if it is clockwise then         -Pi < angle < 0
		sum_angle = 0.0, // the sum of Angles, 
		diff; // angle2 - angle1


	// Temperary create the tmpPntList so we can vertorized it and then analysis it to see
	// if the point is inside the list of points
	CPntList* tmpPntList = new CPntList(*list);
   tmpPntList->vectorize(pageUnits);

	POSITION pos = tmpPntList->GetHeadPosition();
	current_pnt = tmpPntList->GetNext(pos);

	while (pos != NULL)
	{
		last_pnt = current_pnt;
		current_pnt = tmpPntList->GetAt(pos);

		// if point on line, return FALSE.
		Point2 last,current,point;
		last.x = last_pnt->x;
		last.y = last_pnt->y;
		current.x = current_pnt->x;
		current.y = current_pnt->y;
		point.x = xPoint;
		point.y = yPoint;

		if (Point2InSeg(&last, &current, &point, tolerance))
		{
         bool tmpPntList_IsCw = tmpPntList->isCw();
         DbUnit last_pnt__bulge = last_pnt->bulge;
         delete tmpPntList;

			if (last_pnt__bulge > 0)
			{
				if (tmpPntList_IsCw)
					return 1;
				else
					return 0;
			}
			else if (last_pnt__bulge < 0)
			{
				if (tmpPntList_IsCw)
					return 0;
				else
					return 1;
			}
			else
			{
				return -1;
			}
		}

		angle2 = atan2(current_pnt->y - yPoint,
							current_pnt->x - xPoint);
		angle1 = atan2(last_pnt->y - yPoint, last_pnt->x - xPoint);
		//because after atan2(), -Pi <= angle <= Pi

		diff = angle2 - angle1;

		if (diff <= -Pi)
			diff += 2.0 * Pi;

		if (diff >= Pi)
			diff = diff - 2.0 * Pi;

		sum_angle += diff;

		tmpPntList->GetNext(pos);
	}

	delete tmpPntList;
	tmpPntList = NULL;

	if (fabs(sum_angle)  < SMALLNUMBER)
		return 0;

   return 1;
}

bool CPntList::isPointInside(const CPoint2d& point,PageUnitsTag pageUnits, double tolerance) const
{
   bool retval = false;
   
   if (GetCount() > 2)
   {
      retval = (isPointInside(point.x,point.y,pageUnits,tolerance) == 1);
   }

   return retval;
}

bool CPntList::isPointOnSegment(const CPoint2d& point,PageUnitsTag pageUnits, double tolerance) const
{
   bool retval = (isPointInside(point.x,point.y,pageUnits,tolerance) == -1);

   return retval;
}

double CPntList::distanceTo(const CPoint2d& point,CPoint2d* pointResult) const
{
   Point2 point2(point.x,point.y);
   Point2 pointResult2;

   double retval = MeasurePointToPntList(&point2,this,0.,false,&pointResult2);

   if (pointResult != NULL)
   {
      pointResult->x = pointResult2.x;
      pointResult->y = pointResult2.y;
   }

   return retval;
}

bool CPntList::shrink(double distance,PageUnitsTag pageUnits, bool suppressErrorMessages)
{
   static int errorMessageCount = 0;
   bool retval = false;

   try
   {
      CPntList* shrunkPoly = ShrinkPoly(this,pageUnits, distance, suppressErrorMessages);

      if (shrunkPoly != NULL)
      {
         *this = *shrunkPoly;

         delete shrunkPoly;
			shrunkPoly = NULL;

         retval = true;
      }
   }
   catch (...)
   {
      if (!suppressErrorMessages)
      {
         if (errorMessageCount == 0)
         {
            formatMessageBox("Exception in CPntList::shrink()");
         }

         errorMessageCount++;
      }
   }

   return retval;
}

void CPntList::vectorize(PageUnitsTag pageUnits)
{   
   double unitFactor = getUnitsFactor(pageUnitsInches,pageUnits);

   for (POSITION pntPos = GetHeadPosition();pntPos != NULL;)
   {
      CPnt* pnt = GetNext(pntPos);

      // vectorize arcs
      if (fabs(pnt->bulge) > 0.015 && pntPos != NULL)  // (fabs(pnt->bulge) > 0.05 && pntPos != NULL) 
      {
         POSITION tempPos = pntPos;
         CPnt* next = GetNext(tempPos);

         double da, sa, r, cx, cy;
         da = atan(pnt->bulge) * 4;
         pnt->bulge = 0;
         ArcPoint2Angle(pnt->x, pnt->y, next->x, next->y, da, &cx, &cy, &r, &sa);
         
         if (fpeq(da, 0) == false)
         {
            double resolution = da/3.;

            if (r > (0.1 * unitFactor))
            {
               if (fabs(da) > degreesToRadians(5))
                  resolution = degreesToRadians(5);
            }
            else if (r > (0.01 * unitFactor))
            {
               resolution = degreesToRadians(10);
            }
            else if (r > (0.0025 * unitFactor))
            {
               resolution = degreesToRadians(30);
            }

            int segments = (int)ceil(fabs(da) / resolution);
            double degrees = da / segments;

            for (int i=1;i < segments;i++)
            {
               CPnt* newPnt = new CPnt;
               newPnt->bulge = 0;
               newPnt->x = (DbUnit)(cx + r * cos(sa + i * degrees));
               newPnt->y = (DbUnit)(cy + r * sin(sa + i * degrees));
               InsertBefore(pntPos, newPnt);
            }  
         }

         pnt->bulge = 0;
      }
   }
}

void CPntList::vectorize(double angularResolutionRadians)
{   
	//CCEtoODBDoc* doc = getActiveDocument();
	//if (doc == NULL)
	//	return;

 //  double unitFactor = Units_Factor(pageUnitsInches, doc->getSettings().PageUnits);

   for (POSITION pntPos = GetHeadPosition();pntPos != NULL;)
   {
      CPnt* pnt = GetNext(pntPos);

      // vectorize arcs
      if (fabs(pnt->bulge) > 0.015 && pntPos != NULL)  // (fabs(pnt->bulge) > 0.05 && pntPos != NULL) 
      {
         POSITION tempPos = pntPos;
         CPnt* next = GetNext(tempPos);

         double da, sa, r, cx, cy;
         da = atan(pnt->bulge) * 4;
         pnt->bulge = 0;
         ArcPoint2Angle(pnt->x, pnt->y, next->x, next->y, da, &cx, &cy, &r, &sa);
         
         if (fpeq(da, 0) == false)
         {
            //double resolution = da/3.;

            //if (r > (0.1 * unitFactor))
            //{
            //   if (da > DegToRad(5))
            //      resolution = DegToRad(5);
            //}
            //else if (r > (0.01 * unitFactor))
            //{
            //   resolution = DegToRad(10);
            //}
            //else if (r > (0.0025 * unitFactor))
            //{
            //   resolution = DegToRad(30);
            //}

            int segments = (int)ceil(fabs(da) / angularResolutionRadians);
            double radians = da / segments;

            for (int i=1;i < segments;i++)
            {
               CPnt* newPnt = new CPnt;
               newPnt->bulge = 0;
               newPnt->x = (DbUnit)(cx + r * cos(sa + i * radians));
               newPnt->y = (DbUnit)(cy + r * sin(sa + i * radians));
               InsertBefore(pntPos, newPnt);
            }  
         }

         pnt->bulge = 0;
      }
   }
}

bool CPntList::isCcw() const
{
   double area2 = getArea2();

   return (area2 >= 0.);
}

bool CPntList::isCw() const
{
   return !isCcw();
}

double CPntList::getArea2() const
{
   CPnt* pnt0 = NULL;
   CPnt* previousPnt = NULL;
   double sum = 0.;
   double sumChordArea = 0.;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CPnt* pnt = GetNext(pos);

      if (pnt != NULL)
      {
         if (pnt0 == NULL)
         {
            pnt0 = pnt;
         }

         if (previousPnt != NULL)
         {
            sum += (previousPnt->x * pnt->y) - (pnt->x * previousPnt->y);
            sumChordArea += previousPnt->getChordArea(*pnt);

            if (pos == NULL)
            {
               sum += (pnt->x * pnt0->y) - (pnt0->x * pnt->y);
               sumChordArea += pnt->getChordArea(*pnt0);
            }
         }

         previousPnt = pnt;
      }
   }

   double area2 = sum + 2.*sumChordArea;

   return area2;
}

double CPntList::getArea() const
{
   double area = fabs(getArea2() / 2.);

   return area;
}

double CPntList::getLength() const 
{
   CPnt* previousPnt = NULL;
   double length = 0.;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CPnt* pnt = GetNext(pos);

      if (pnt != NULL)
      {
         if (previousPnt != NULL)
         {
            length += previousPnt->getLength(*pnt);
         }

         previousPnt = pnt;
      }
   }

   return length;
}

bool CPntList::reverse()
{
   bool retval = false;

   if (GetCount() > 1)
   {
      POSITION initialHeadPos = GetHeadPosition();

      do
      {
         CPnt* pnt = RemoveTail();
         CPnt* tail = GetTail();
         pnt->bulge = - tail->bulge;
         InsertBefore(initialHeadPos,pnt);
      }
      while (GetTailPosition() != initialHeadPos);

      GetTail()->bulge = 0.;

      retval = true;
   }

   return retval;
}

void CPntList::simplifyBulges(int segmentCountExponent)
{
   CPnt* previousPnt = NULL;
   double length = 0.;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      POSITION oldPos = pos;
      CPnt* pnt = GetNext(pos);

      if (pnt != NULL)
      {
         if (previousPnt != NULL && previousPnt->bulge != 0.)
         {
            double cx = pnt->x - previousPnt->x;
            double cy = pnt->y - previousPnt->y;
            double ratio = previousPnt->bulge / 2.;
            double sx = ( cy) * ratio;
            double sy = (-cx) * ratio;
            double x2 = ((pnt->x + previousPnt->x)/2.) + sx; 
            double y2 = ((pnt->y + previousPnt->y)/2.) + sy; 

            InsertBefore(oldPos,new CPnt(x2,y2));

            previousPnt->bulge = 0.;
         }

         previousPnt = pnt;
      }
   }
}

void CPntList::clean(double tolerance)
{
   CPnt* previousPnt = NULL;
   CPnt* pnt;
   POSITION pntPos,previousPntPos;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      pntPos = pos;
      pnt = GetNext(pos);

      if (pnt == NULL)
      {
         RemoveAt(pntPos);
         continue;
      }

      if (previousPnt != NULL && fpnear(previousPnt->x,pnt->x,tolerance) && fpnear(previousPnt->y,pnt->y,tolerance))
      {
         RemoveAt(previousPntPos);
         delete previousPnt;
         previousPnt = NULL;
      }

      previousPnt    = pnt;
      previousPntPos = pntPos;
   }
}

bool CPntList::hasBulge() const
{
	bool retval = false;
	for (POSITION pos = this->GetHeadPosition(); pos != NULL;)
	{
		CPnt* pnt = this->GetNext(pos);
		if (pnt != NULL && fabs(pnt->bulge) >= BULGE_THRESHOLD)
		{
			retval = true;
			break;
		}
	}

	return retval; 
}

void CPntList::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"CPntList\n"
"{\n"
"   Count=%d\n",
GetCount());

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      for (POSITION pos = GetHeadPosition();pos != NULL;)
      {
         CPnt* pnt = GetNext(pos);

         if (pnt != NULL)
         {
            pnt->dump(writeFormat,depth);
         }
      }

      writeFormat.popHeader();
   }

   writeFormat.writef("}\n");
}

//_____________________________________________________________________________
CPnt3D& CPnt3D::operator=(const CPnt3D& other)
{
   x     = other.x;
   y     = other.y;
   z     = other.z;
   
   return *this;
}

double CPnt3D::getLength(const CPnt3D& nextPnt) const
{
   double length = sqrt(pow(nextPnt.x - x,2) + pow(nextPnt.y - y, 2)+ pow(nextPnt.z - z, 2));

   return length;
}

void CPnt3D::transform(const CTMatrix& transformationMatrix)
{
   transformationMatrix.transform(x,y);
   this->setZ(z * transformationMatrix.getScale());
}

void CPnt3D::dump(CWriteFormat& writeFormat,int depth) const
{
   writeFormat.writef(
      "CPnt3D\n"
      "{\n"
      "   (x,y,z)=(%.3f,%.3f,%.3f)\n"
      "}\n",
      x,y,z);
}

//_____________________________________________________________________________
CPnt3DList::CPnt3DList()
{
}

CPnt3DList::CPnt3DList(const CPnt3DList& other)
{
   for (POSITION pos = other.GetHeadPosition();pos != NULL;)
   {
      CPnt3D* pnt = other.GetNext(pos);

      if (pnt != NULL)
      {
         CPnt3D* pntCopy = new CPnt3D(*pnt);

         AddTail(pntCopy);
      }
   }
}

CPnt3DList& CPnt3DList::operator=(const CPnt3DList& other)
{
   if (&other != this)
   {
      empty();

      for (POSITION pos = other.GetHeadPosition();pos != NULL;)
      {
         CPnt3D* pnt = other.GetNext(pos);

         AddTail(new CPnt3D(*pnt));
      }
   }

   return *this;
}

void CPnt3DList::transform(const CTMatrix& transformationMatrix)
{
   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CPnt3D* pnt = GetNext(pos);

      if (pnt != NULL)
      {
         pnt->transform(transformationMatrix);
      }
   }
}

void CPnt3DList::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
        "CPnt3DList\n"
        "{\n"
        "   Count=%d\n",
        GetCount());

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      for (POSITION pos = GetHeadPosition();pos != NULL;)
      {
         CPnt3D* pnt = GetNext(pos);

         if (pnt != NULL)
         {
            pnt->dump(writeFormat,depth);
         }
      }

      writeFormat.popHeader();
   }

   writeFormat.writef("}\n");
}