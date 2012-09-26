// $Header: /CAMCAD/4.4/PolarCoordinate.h 2     4/20/04 7:27p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#if ! defined (__PolarCoordinate_h__)
#define __PolarCoordinate_h__

#pragma once

#include "TypedContainer.h"

class CPoint2d;
class CWriteFormat;

//_____________________________________________________________________________
class CPolarCoordinate
{
private:
   double m_r;
   double m_theta;

public:

// Constructors
   CPolarCoordinate() { m_r = m_theta = 0.; }
   CPolarCoordinate(double r, double theta) { m_r = r;  m_theta = theta; }
   CPolarCoordinate(const CPolarCoordinate& coordinate) { m_r = coordinate.m_r;  m_theta = coordinate.m_theta; }
   CPolarCoordinate(const CPoint2d& rectangularCoordinate);

// Access
   double getR() const { return m_r; }
   void setR(double r) { m_r = r; }
   double getTheta() const { return m_theta; }
   void setTheta(double theta) { m_theta = theta; }
   CPoint2d getRectangularCoordinate() const;

// Operations
   //double distance(const CPoint2d& otherPoint) const { return _hypot(x - otherPoint.x,y - otherPoint.y); }
   //double distance(double xOther,double yOther) const { return _hypot(x - xOther,y - yOther); }
   //double distanceSqr(const CPoint2d& otherPoint) const { return distanceSqr(otherPoint.x,otherPoint.y); }
   //double distanceSqr(double xOther,double yOther) const
   //   { double dx = x - xOther;  double dy = y - yOther;  return dx*dx + dy*dy; }

// Operators returning CPoint2d values
   //CPoint2d operator-(const CPoint2d& otherPoint) const
   //   { return CPoint2d(x - otherPoint.x,y - otherPoint.y); }
   //CPoint2d operator+(const CPoint2d& otherPoint) const
   //   { return CPoint2d(x + otherPoint.x,y + otherPoint.y); }
   //CPoint2d operator-() const
   //   { return CPoint2d(-x,-y); }

// Operators returning CSize values

// Operators returning CRect values

// Comparison operators (used to define a sort order)
   bool operator==(const CPolarCoordinate& otherCoordinate) const {  return  m_r == otherCoordinate.m_r  && m_theta == otherCoordinate.m_theta;  }
   bool operator!=(const CPolarCoordinate& otherCoordinate) const {  return  m_r != otherCoordinate.m_r  || m_theta != otherCoordinate.m_theta;  }
   bool operator< (const CPolarCoordinate& otherCoordinate) const {  return (m_r <  otherCoordinate.m_r) || (m_r == otherCoordinate.m_r && m_theta < otherCoordinate.m_theta);  }
   bool operator> (const CPolarCoordinate& otherCoordinate) const {  return (m_r >  otherCoordinate.m_r) || (m_r == otherCoordinate.m_r && m_theta > otherCoordinate.m_theta);  }
   bool operator<=(const CPolarCoordinate& otherCoordinate) const {  return (m_r <  otherCoordinate.m_r) || (m_r == otherCoordinate.m_r && m_theta <= otherCoordinate.m_theta);  }
   bool operator>=(const CPolarCoordinate& otherCoordinate) const {  return (m_r >  otherCoordinate.m_r) || (m_r == otherCoordinate.m_r && m_theta >= otherCoordinate.m_theta);  }

   void trace(CWriteFormat* writeFormat);
};

class CPolarCoordinateArray : public CTypedPtrArrayContainer<CPolarCoordinate*>
{
public:
   CPolarCoordinateArray(bool isContainer=true) : 
      CTypedPtrArrayContainer<CPolarCoordinate*>(isContainer) 
   {}
};

class CPolarCoordinateList : public CTypedPtrListContainer<CPolarCoordinate*>
{
public:
   CPolarCoordinateList(bool isContainer=true) : 
      CTypedPtrListContainer<CPolarCoordinate*>(isContainer) 
   {}
};

class CPolarCoordinateCircularList
{
private:
   CTypedPtrListContainer<CPolarCoordinate*> m_list;

public:
   CPolarCoordinateCircularList(bool isContainer=true);

	POSITION AddHead(CPolarCoordinate* newElement)
		{ return m_list.AddHead(newElement); }
	POSITION AddTail(CPolarCoordinate*  newElement)
		{ return m_list.AddTail(newElement); }

	// peek at head or tail
	CPolarCoordinate* GetHead()
		{ return m_list.GetHead(); }
	CPolarCoordinate* GetTail()
		{ return m_list.GetTail(); }

	// iteration
   POSITION GetHeadPosition() const { return m_list.GetHeadPosition(); }
	POSITION GetTailPosition() const { return m_list.GetTailPosition(); }
   CPolarCoordinate* GetNext(POSITION& rPosition);
	CPolarCoordinate* GetPrev(POSITION& rPosition);

   //
   CPolarCoordinate* GetAt(POSITION position) { return m_list.GetAt(position); }
   CPolarCoordinate* RemoveHead() { return m_list.RemoveHead(); }
	CPolarCoordinate* RemoveTail() { return m_list.RemoveTail(); }
};

#endif

