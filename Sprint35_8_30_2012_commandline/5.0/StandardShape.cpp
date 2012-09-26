// $Header: /CAMCAD/4.4/StandardShape.cpp 1     4/26/04 7:16p Kurt Van Ness $

#include "StdAfx.h"
#include "StandardShape.h"

//_____________________________________________________________________________
CStandardShape::CStandardShape()
{
}

CStandardShape* CStandardShape::getShape(const CPolygon& polygon)
{
   return NULL;
}

//_____________________________________________________________________________
CStandardShapeCircle::CStandardShapeCircle(double radius)
{
}

int CStandardShapeCircle::getSymmetry() const
{
   return 1;
}

StandardShapeTypeTag CStandardShapeCircle::getType() const
{
   return standardShapeCircle;
}

double CStandardShapeCircle::getDimension(int index) const
{
   double retval = ((index == 0) ? m_radius : 0.);

   return retval;
}

void CStandardShapeCircle::setDimensions(double dimension0,double dimension1,double dimension2,
                                         double dimension3,double dimension4,double dimension5)
{
   m_radius = dimension0;
}

