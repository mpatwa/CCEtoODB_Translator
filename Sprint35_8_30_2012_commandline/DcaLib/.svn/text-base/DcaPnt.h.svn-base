// $Header: /CAMCAD/DcaLib/DcaPnt.h 5     6/21/07 8:29p Kurt Van Ness $

#if !defined(__DcaPnt_h__)
#define __DcaPnt_h__

#pragma once

#include "Dca.h"
#include "DcaFpeq.h"
#include "DcaContainer.h"  // todo - knv - needs to be replaced by DcaContainer.h

class CTMatrix;
class CExtent;
class CWriteFormat;
class CPoint2d;
enum PageUnitsTag;

//_____________________________________________________________________________
class CPnt
{
public:
   DbUnit x, y, bulge;

   CPnt(DbUnit X=0.f,DbUnit Y=0.f,DbUnit Bulge=0.f) :
      x(X),y(Y),bulge(Bulge) {}

   CPnt(double X,double Y,double Bulge=0.) :
      x((DbUnit)X),y((DbUnit)Y),bulge((DbUnit)Bulge) {}

   CPnt(const CPnt& other) :
      x(other.x),y(other.y),bulge(other.bulge) {}

   CPnt& operator=(const CPnt& other);

   void setX(double x) { this->x = (DbUnit)x; }
   void setY(double y) { this->y = (DbUnit)y; }
   void setBulge(double bulge) { this->bulge = (DbUnit)bulge; }
   void setX(DbUnit x) { this->x = x; }
   void setY(DbUnit y) { this->y = y; }
   void setBulge(DbUnit bulge) { this->bulge = bulge; }

   double getChordArea(const CPnt& nextPnt) const;
   double getLength(const CPnt& nextPnt) const;
   void transform(const CTMatrix& transformationMatrix);

   void dump(CWriteFormat& writeFormat,int depth = -1) const;
};

//_____________________________________________________________________________
class CPntList : public CTypedPtrListContainer<CPnt*>
{
public:
   CPntList();
   CPntList(const CPntList& other);
   CPntList& operator=(const CPntList& other);

   void transform(const CTMatrix& transformationMatrix);

   void updateExtent(CExtent& extent,const CTMatrix* transformationMatrix=NULL) const;
   void updateExtent(CExtent& extent,const CTMatrix& transformationMatrix) const
      { updateExtent(extent,&transformationMatrix); }
   CExtent getExtent() const;
   CExtent getExtent(const CTMatrix& transformationMatrix) const;

   bool isSimpleCircle(double& xCenter,double& yCenter,double& radius) const;
   int isPointInside(double xPoint,double yPoint,PageUnitsTag pageUnits,double tolerance) const;
   bool isPointInside(const CPoint2d& point,PageUnitsTag pageUnits, double tolerance = SMALLNUMBER) const;
   bool isPointOnSegment(const CPoint2d& point,PageUnitsTag pageUnits, double tolerance = SMALLNUMBER) const;
   bool isCcw() const;
   bool isCw() const;

   double distanceTo(const CPoint2d& point,CPoint2d* pointResult=NULL) const;
   bool shrink(double distance,PageUnitsTag pageUnits, bool suppressErrorMessages = true);
   double getArea2() const;
   double getArea() const;
   double getLength() const;
   void simplifyBulges(int segmentCountExponent=0);
   void vectorize(PageUnitsTag pageUnits);
   void vectorize(double angularResolutionRadians);
   bool reverse();
   void clean(double tolerance);
	bool hasBulge() const;

   void dump(CWriteFormat& writeFormat,int depth = -1) const;
   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
};

//_____________________________________________________________________________
class CPnt3D
{
public:
   DbUnit x, y, z;

   CPnt3D(DbUnit X=0.f,DbUnit Y=0.f,DbUnit Z=0.f) :
      x(X),y(Y),z(Z) {}

   CPnt3D(double X,double Y,double Z=0.) :
      x((DbUnit)X),y((DbUnit)Y),z((DbUnit)Z) {}

   CPnt3D(const CPnt3D& other) :
      x(other.x),y(other.y),z(other.z) {}

   CPnt3D& operator=(const CPnt3D& other);

   void setX(double x) { this->x = (DbUnit)x; }
   void setY(double y) { this->y = (DbUnit)y; }
   void setZ(double z) { this->z = (DbUnit)z; }
   void setX(DbUnit x) { this->x = x; }
   void setY(DbUnit y) { this->y = y; }
   void setZ(DbUnit z) { this->z = z; }
   
   double getLength(const CPnt3D& nextPnt) const;
   void transform(const CTMatrix& transformationMatrix);

   void dump(CWriteFormat& writeFormat,int depth = -1) const;
};

//_____________________________________________________________________________
class CPnt3DList : public CTypedPtrListContainer<CPnt3D*>
{
public:
   CPnt3DList();
   CPnt3DList(const CPnt3DList& other);
   CPnt3DList& operator=(const CPnt3DList& other);

   void transform(const CTMatrix& transformationMatrix);   

   void dump(CWriteFormat& writeFormat,int depth = -1) const;
   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
};
#endif
