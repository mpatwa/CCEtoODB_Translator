// $Header: /CAMCAD/DcaLib/DcaBasesVector.h 5     3/23/07 5:19a Kurt Van Ness $

#if !defined(__DcaBasesVector_h__)
#define __DcaBasesVector_h__

#pragma once

#include "DcaPoint2d.h"

class CVector2d;

//_____________________________________________________________________________
class CBasesVector : public CObject
{
private:
   CPoint2d m_origin;
   CPoint2d m_xUnitVector;
   CPoint2d m_yUnitVector;

   mutable double m_rotationDegrees;
   mutable double m_scale;
   mutable bool m_mirror;
   mutable bool m_staleFlag;

public:
   CBasesVector();
   CBasesVector(double xOrigin,double yOrigin);
   CBasesVector(double xOrigin,double yOrigin,double rotationInDegrees);
   CBasesVector(double xOrigin,double yOrigin,double rotationInDegrees,bool mirror);
   CBasesVector(double xOrigin,double yOrigin,double scale,double rotationInDegrees,bool mirror);
   CBasesVector(const CPoint2d& origin);
   CBasesVector(const CPoint2d& origin,double rotationInDegrees);
   CBasesVector(const CPoint2d& origin,double rotationInDegrees,bool mirror);
   CBasesVector(const CPoint2d& origin,double scale,double rotationInDegrees,bool mirror);
   CBasesVector(const CBasesVector& other);
   ~CBasesVector();
   CBasesVector& operator=(const CBasesVector& other);

   double getScale() const;
   double getRotation() const { return getRotationDegrees(); }  // degrees
   double getRotationDegrees() const;
   double getRotationRadians() const;

   bool getMirror() const;
   void setMirror(bool mirror);
   void mirrorRotation();

   CPoint2d getOrigin() const { return m_origin; }
   double getX() const { return m_origin.x; }
   double getY() const { return m_origin.y; }
   CVector2d getXaxis() const;
   CVector2d getYaxis() const;
   void initRotationMirror(double rotationInDegrees,bool mirror);
   CTMatrix getTransformationMatrix() const;
   void set(double xOrigin=0.,double yOrigin=0.,double rotationInDegrees=0.,bool mirror=false,double scale=1.);
   void set(const CPoint2d& origin,double rotationInDegrees=0.,bool mirror=false,double scale=1.);

   void transform(const CTMatrix& transformationMatrix);
   CString getDescriptor(int maxPrecision=3) const;

private:
   void refresh() const;
};

#endif
