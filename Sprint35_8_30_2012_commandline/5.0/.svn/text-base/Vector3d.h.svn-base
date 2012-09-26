// $Header: /CAMCAD/4.5/Vector3d.h 1     3/31/05 10:10a Kurt Van Ness $

#if !defined(__Vector3d_H__)
#define __Vector3d_H__ 

#pragma once

#include "Point3d.h"
#include "Vector2d.h"

//_____________________________________________________________________________
class CVector3d
{
private:
   CSize3d  m_components;
   CPoint3d m_origin;

public:
   CVector3d();
   CVector3d(double xComponent,double yComponent,double zComponent,double xOrigin=0.,double yOrigin=0,double zOrigin=0.);
   CVector3d(const CSize3d& components);
   CVector3d(const CSize2d& components);
   CVector3d(const CPoint3d& components);
   CVector3d(const CPoint2d& components);
   CVector3d(const CSize3d& components,const CPoint3d& origin);
   CVector3d(const CPoint3d& origin,const CPoint3d& head);
   CVector3d(const CVector3d& other);
   CVector3d(const CVector2d& other);
   CVector3d& operator=(const CVector3d& other);

   const CSize3d& getComponents() const { return m_components; }
   void setComponents(const CSize3d& components) { m_components = components; }

   double getComponentX() const { return m_components.cx; }
   double getComponentY() const { return m_components.cy; }
   double getComponentZ() const { return m_components.cz; }

   const CPoint3d& getOrigin() const { return m_origin; }
   void setComponents(const CPoint3d& origin) { m_origin = origin; }

   CPoint3d getTip() const { return m_origin + m_components; }
   void setTip(const CPoint3d& tip) { m_components = tip - m_origin; }

   void set(const CSize3d& components,const CPoint3d& origin) { m_components = components;  m_origin = origin; }

   // 
   double getLengthSquared() const;
   double getLength() const { return sqrt(getLengthSquared()); }
   double getTheta() const { return m_components.getTheta(); }
   //CExtent getExtent() const;
   //void transform(const CTMatrix& matrix);
   double dotProduct(const CVector3d& other);
   CVector3d crossProduct(const CVector3d& other);
};

#endif

