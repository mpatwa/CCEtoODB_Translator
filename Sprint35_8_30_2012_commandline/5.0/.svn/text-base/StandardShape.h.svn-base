// $Header: /CAMCAD/4.4/StandardShape.h 1     4/26/04 7:16p Kurt Van Ness $

#if ! defined (__StandardShape_h__)
#define __StandardShape_h__

#pragma once

//#include "Polygon.h"

//class CCamCadDatabase;
class CPolygon;

//_____________________________________________________________________________
enum StandardShapeTypeTag
{
   standardShapeCircle,
   standardShapeSquare,
   standardShapeRectangle,
   standardShapeRoundedRectangle,
   standardShapeChamferedRectangle,
   standardShapeOval,
   standardShapeRhombus,
   standardShapeOctagon,
   standardShapeHexagon,
   standardShapeTriangle,
   standardShapeHalfOval,
   standardShapeEllipse,
   standardShapeNgon,
   standardShapeInvalid,
   standardShapeUndefined
};

//_____________________________________________________________________________
class CStandardShape
{
private:

public:
   CStandardShape();

   // core functions
   virtual int getSymmetry() const = 0;
   virtual StandardShapeTypeTag getType() const = 0;
   virtual double getDimension(int index) const = 0;
   virtual void setDimensions(double dimension0=0.,double dimension1=0.,double dimension2=0.,
                              double dimension3=0.,double dimension4=0.,double dimension5=0.) = 0;

   // static functions
   static CStandardShape* getShape(const CPolygon& polygon);
};

//_____________________________________________________________________________
class CStandardShapeCircle : CStandardShape
{
private:
   double m_radius;

public:
   CStandardShapeCircle(double radius);

   // core functions
   virtual int getSymmetry() const;
   virtual StandardShapeTypeTag getType() const;
   virtual double getDimension(int index) const;
   virtual void setDimensions(double dimension0=0.,double dimension1=0.,double dimension2=0.,
                              double dimension3=0.,double dimension4=0.,double dimension5=0.);

};

#endif


