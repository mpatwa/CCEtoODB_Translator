// $Header: /CAMCAD/4.4/PolarCoordinate.cpp 2     4/20/04 7:27p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "StdAfx.h"
#include "PolarCoordinate.h"
#include "Point2d.h"
#include "WriteFormat.h"
#include "RwLib.h"

//_____________________________________________________________________________
CPolarCoordinate::CPolarCoordinate(const CPoint2d& rectangularCoordinate)
{
   m_r     = _hypot(rectangularCoordinate.x,rectangularCoordinate.y);
   m_theta = atan2(rectangularCoordinate.y,rectangularCoordinate.x);
}

CPoint2d CPolarCoordinate::getRectangularCoordinate() const
{ 
   CPoint2d rectangularCoordinate;

   rectangularCoordinate.x = m_r * cos(m_theta);
   rectangularCoordinate.y = m_r * sin(m_theta);

   return rectangularCoordinate;
}

void CPolarCoordinate::trace(CWriteFormat* writeFormat)
{
   if (writeFormat != NULL)
   {
      writeFormat->writef("r=%s, theta=%s",fpfmt(m_r),fpfmt(m_theta));
   }
}

//_____________________________________________________________________________
CPolarCoordinateCircularList::CPolarCoordinateCircularList(bool isContainer) : 
   m_list(isContainer)
{
}

CPolarCoordinate* CPolarCoordinateCircularList::GetNext(POSITION& rPosition) 
{ 
   CPolarCoordinate* polarCoordinate = m_list.GetNext(rPosition); 

   if (rPosition == NULL)
   {
      rPosition = m_list.GetHeadPosition();
   }

   return polarCoordinate;
}

CPolarCoordinate* CPolarCoordinateCircularList::GetPrev(POSITION& rPosition) 
{ 
   CPolarCoordinate* polarCoordinate = m_list.GetPrev(rPosition); 

   if (rPosition == NULL)
   {
      rPosition = m_list.GetTailPosition();
   }

   return polarCoordinate;
}
