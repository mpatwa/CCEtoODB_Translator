// $Header: /CAMCAD/4.5/Vector3d.cpp 1     3/31/05 10:10a Kurt Van Ness $

#include "StdAfx.h"
#include "Vector3d.h"

//_____________________________________________________________________________
CVector3d::CVector3d()
{
}

CVector3d::CVector3d(double xComponent,double yComponent,double zComponent,double xOrigin,double yOrigin,double zOrigin) :
   m_components(xComponent,yComponent,zComponent),m_origin(xOrigin,yOrigin,zOrigin)
{
}

CVector3d::CVector3d(const CSize3d& components) :
   m_components(components)
{
}

CVector3d::CVector3d(const CSize2d& components) :
   m_components(components)
{
}

CVector3d::CVector3d(const CPoint3d& components) :
   m_components(components)
{
}

CVector3d::CVector3d(const CPoint2d& components) :
   m_components(components)
{
}

CVector3d::CVector3d(const CSize3d& components,const CPoint3d& origin) :
   m_components(components),m_origin(origin)
{
}

CVector3d::CVector3d(const CPoint3d& origin,const CPoint3d& head) :
   m_components(head - origin),m_origin(origin)
{
}

CVector3d::CVector3d(const CVector3d& other)
{
   m_components = other.m_components;
   m_origin     = other.m_origin;
}

CVector3d::CVector3d(const CVector2d& other)
{
   m_components = other.getComponents();
   m_origin     = other.getOrigin();
}

CVector3d& CVector3d::operator=(const CVector3d& other)
{
   m_components = other.m_components;
   m_origin     = other.m_origin;

   return *this;
}

double CVector3d::dotProduct(const CVector3d& other)
{
   double dotProduct = (m_components.cx * other.m_components.cx) + 
                       (m_components.cy * other.m_components.cy) + 
                       (m_components.cz * other.m_components.cz);

   return dotProduct;
}

CVector3d CVector3d::crossProduct(const CVector3d& other)
{
   /*
         i    j    k
         ux   uy   uz
         vx   vy   vz
   */

   const CSize3d& u = m_components;
   const CSize3d& v = other.m_components;

   CSize3d resultSize;

   resultSize.cx =  ((u.cy * v.cz) - (u.cz * v.cy)); 
   resultSize.cy = -((u.cx * v.cz) - (u.cz * v.cx)); 
   resultSize.cz =  ((u.cx * v.cy) - (u.cy * v.cx)); 

   CVector3d result(resultSize);

   return result;
}

//CExtent CVector3d::getExtent() const
//{
//   CExtent extent;
//   extent.update(m_origin);
//   extent.update(getTip());
//
//   return extent;
//}
//
//void CVector3d::transform(const CTMatrix& matrix)
//{
//   CPoint2d tip(getTip());
//
//   matrix.transform(m_origin);
//   matrix.transform(tip);
//
//   setTip(tip);
//}





