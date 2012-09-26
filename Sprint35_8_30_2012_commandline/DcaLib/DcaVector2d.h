// $Header: /CAMCAD/DcaLib/DcaVector2d.h 5     6/04/07 5:21p Lynn Phung $

#if !defined(__DcaVector2d_h__)
#define __DcaVector2d_h__

#pragma once

#include "DcaPoint2d.h"

class CExtent;
class CTMatrix;

//_____________________________________________________________________________
class CVector2d
{
private:
   CSize2d  m_components;
   CPoint2d m_origin;

public:
   CVector2d();
   CVector2d(double xComponent,double yComponent,double xOrigin=0.,double yOrigin=0.);
   CVector2d(const CSize2d& components);
   CVector2d(const CSize2d& components,const CPoint2d& origin);
   CVector2d(const CPoint2d& origin,const CPoint2d& head);
   CVector2d(const CVector2d& other);
   CVector2d& operator=(const CVector2d& other);

   const CSize2d& getComponents() const { return m_components; }
   void setComponents(const CSize2d& components) { m_components = components; }

   const CPoint2d& getOrigin() const { return m_origin; }
   void setOrigin(const CPoint2d& origin) { m_origin = origin; }

   CPoint2d getTip()    const { return m_origin + m_components; }
   CPoint2d getCenter() const { return m_origin + m_components/2.; }
   void setTip(const CPoint2d& tip) { m_components = tip - m_origin; }

   void set(const CSize2d& components,const CPoint2d& origin) { m_components = components;  m_origin = origin; }

   // 
   double getLengthSquared() const { return m_components.cx * m_components.cx + m_components.cy * m_components.cy; }
   double getLength() const { return sqrt(getLengthSquared()); }
   double getTheta() const { return m_components.getTheta(); }
   CExtent getExtent() const;
   void transform(const CTMatrix& matrix);
   double dotProduct(const CVector2d& other);
   double crossProductMagnitude(const CVector2d& other);

   void setLength(double length);
   void makePerpendicular(bool ccwFlag);
   void exchangeOriginAndTip();
   void reverse() { m_components = -m_components; }
};

#endif
