// $Header: /CAMCAD/5.0/Dca/DcaVector2d.cpp 4     3/12/07 12:48p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaVector2d.h"
#include "DcaExtent.h"
#include "DcaTMatrix.h"

//_____________________________________________________________________________
CVector2d::CVector2d()
{
}

CVector2d::CVector2d(double xComponent,double yComponent,double xOrigin,double yOrigin) :
   m_components(xComponent,yComponent),m_origin(xOrigin,yOrigin)
{
}

CVector2d::CVector2d(const CSize2d& components) :
   m_components(components)
{
}

CVector2d::CVector2d(const CSize2d& components,const CPoint2d& origin) :
   m_components(components),m_origin(origin)
{
}

CVector2d::CVector2d(const CPoint2d& origin,const CPoint2d& head) :
   m_components(head - origin),m_origin(origin)
{
}

CVector2d::CVector2d(const CVector2d& other)
{
   m_components = other.m_components;
   m_origin     = other.m_origin;
}

CVector2d& CVector2d::operator=(const CVector2d& other)
{
   m_components = other.m_components;
   m_origin     = other.m_origin;

   return *this;
}

double CVector2d::dotProduct(const CVector2d& other)
{
   double dotProduct = (m_components.cx * other.m_components.cx) + (m_components.cy * other.m_components.cy);

   return dotProduct;
}

double CVector2d::crossProductMagnitude(const CVector2d& other)
{
   double crossProductMagnitude = (m_components.cx * other.m_components.cy) - (m_components.cy * other.m_components.cx);

   return crossProductMagnitude;
}

CExtent CVector2d::getExtent() const
{
   CExtent extent;
   extent.update(m_origin);
   extent.update(getTip());

   return extent;
}

void CVector2d::transform(const CTMatrix& matrix)
{
   CPoint2d tip(getTip());

   matrix.transform(m_origin);
   matrix.transform(tip);

   setTip(tip);
}

void CVector2d::setLength(double newLength)
{
   double length = getLength();

   if (length != 0.)
   {
      double ratio = newLength / length;

      m_components.cx *= ratio;
      m_components.cy *= ratio;
   }
}

void CVector2d::makePerpendicular(bool ccwFlag)
{
   double temp = m_components.cx;

   if (ccwFlag)
   {
      m_components.cx = -m_components.cy;
      m_components.cy =  temp;
   }
   else
   {
      m_components.cx =  m_components.cy;
      m_components.cy = -temp;
   }
}

void CVector2d::exchangeOriginAndTip()
{
   m_origin = getTip();

   m_components = -m_components;
}

