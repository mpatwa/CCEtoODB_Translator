// $Header: /CAMCAD/DcaLib/DcaBasesVector.cpp 5     3/23/07 5:19a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaBasesVector.h"
#include "DcaTMatrix.h"
#include "DcaVector2d.h"
#include "DcaLib.h"

//_____________________________________________________________________________
CBasesVector::CBasesVector()
: m_origin(0.,0.)
, m_xUnitVector(1.,0.)
, m_yUnitVector(0.,1.)
{
   m_staleFlag = true;
}

CBasesVector::CBasesVector(double xOrigin,double yOrigin)
{
   set(xOrigin,yOrigin);
}

CBasesVector::CBasesVector(double xOrigin,double yOrigin,double degrees)
{
   set(xOrigin,yOrigin,degrees);
}

CBasesVector::CBasesVector(double xOrigin,double yOrigin,double degrees,bool mirror)
{
   set(xOrigin,yOrigin,degrees,mirror);
}

CBasesVector::CBasesVector(double xOrigin,double yOrigin,double scale,double degrees,bool mirror)
{
   set(xOrigin,yOrigin,degrees,mirror,scale);
}

CBasesVector::CBasesVector(const CPoint2d& origin)
{
   set(origin);
}

CBasesVector::CBasesVector(const CPoint2d& origin,double degrees)
{
   set(origin,degrees);
}

CBasesVector::CBasesVector(const CPoint2d& origin,double degrees,bool mirror)
{
   set(origin,degrees,mirror);
}

CBasesVector::CBasesVector(const CPoint2d& origin,double scale,double degrees,bool mirror)
{
   set(origin,degrees,mirror,scale);
}

CBasesVector::CBasesVector(const CBasesVector& other)
{
   *this = other;
}

void CBasesVector::set(double xOrigin,double yOrigin,double degrees,bool mirror,double scale)
{
   CTMatrix matrix;
   matrix.rotateDegrees(degrees);
   matrix.scale(mirror ? -scale : scale,scale);
   matrix.translate(xOrigin,yOrigin);

   m_origin      = CPoint2d(0.,0.);
   m_xUnitVector = CPoint2d(1.,0.);
   m_yUnitVector = CPoint2d(0.,1.);

   transform(matrix);
}

void CBasesVector::set(const CPoint2d& origin,double degrees,bool mirror,double scale)
{
   set(origin.x,origin.y,degrees,mirror,scale);
}

CBasesVector& CBasesVector::operator=(const CBasesVector& other)
{
   if (&other != this)
   {
      m_origin      = other.m_origin;
      m_xUnitVector = other.m_xUnitVector;
      m_yUnitVector = other.m_yUnitVector;

      m_rotationDegrees = other.m_rotationDegrees;
      m_scale           = other.m_scale;
      m_mirror          = other.m_mirror;
      m_staleFlag       = other.m_staleFlag;
   }

   return *this;
}

CBasesVector::~CBasesVector()
{
}

double CBasesVector::getScale() const
{
   refresh();

   return m_scale;
}

double CBasesVector::getRotationDegrees() const
{
   refresh();

   return m_rotationDegrees;
}

double CBasesVector::getRotationRadians() const
{
   return degreesToRadians(getRotationDegrees());
}

bool CBasesVector::getMirror() const
{
   refresh();

   return m_mirror;
}

void CBasesVector::setMirror(bool mirror)
{
   refresh();

   if (m_mirror != mirror)
   {
      CTMatrix matrix;
      matrix.translate(-m_origin);
      matrix.mirrorAboutYAxis(true);
      matrix.translate(m_origin);

      transform(matrix);
   }
}

void CBasesVector::mirrorRotation()
{
   set(m_origin.x,m_origin.y,-getRotationDegrees(),getMirror());
}

void CBasesVector::initRotationMirror(double degrees,bool mirror)
{
   CTMatrix matrix;
   matrix.rotateDegrees(degrees);
   matrix.scale(mirror ? -1. : 1.,1.);

   m_origin      = CPoint2d(0.,0.);
   m_xUnitVector = CPoint2d(1.,0.);
   m_yUnitVector = CPoint2d(0.,1.);

   transform(matrix);
}

void CBasesVector::refresh() const
{
   if (m_staleFlag)
   {
      m_staleFlag = false;

      CPoint2d xUnitVector = m_xUnitVector - m_origin;
      CPoint2d yUnitVector = m_yUnitVector - m_origin;

      double crossProduct = (xUnitVector.x * yUnitVector.y) - (xUnitVector.y * yUnitVector.x);

      m_mirror          = (crossProduct < 0.);      
      m_rotationDegrees = radiansToDegrees(atan2(xUnitVector.y,xUnitVector.x * (m_mirror ? -1.0 : 1.0)));

      const double squareRootOfTwo = 1.4142135623730950488016887242097;
      CPoint2d sum = xUnitVector + yUnitVector;
      m_scale = HYPOT(sum.x,sum.y) / squareRootOfTwo;
   }
}

CVector2d CBasesVector::getXaxis() const
{
   CVector2d xAxis(m_origin,m_xUnitVector);

   return xAxis;
}

CVector2d CBasesVector::getYaxis() const
{
   CVector2d yAxis(m_origin,m_yUnitVector);

   return yAxis;
}

void CBasesVector::transform(const CTMatrix& transformationMatrix)
{
   transformationMatrix.transform(m_origin);
   transformationMatrix.transform(m_xUnitVector);
   transformationMatrix.transform(m_yUnitVector);

   m_staleFlag = true;
}

CTMatrix CBasesVector::getTransformationMatrix() const
{
   CTMatrix matrix;

   matrix.translateCtm(getOrigin());
   matrix.rotateRadiansCtm(getRotationRadians());
   matrix.scaleCtm(getMirror() ? -1. : 1.,1.);

   return matrix;
}

CString CBasesVector::getDescriptor(int maxPrecision) const
{
   CString descriptor;

   descriptor.Format("(%s,%s) %s deg%s",
      fpfmt(m_origin.x,maxPrecision),
      fpfmt(m_origin.y,maxPrecision),
      fpfmt(normalizeDegrees(getRotationDegrees()),maxPrecision),
      getMirror() ? " mir" : "");

   return descriptor;
}
