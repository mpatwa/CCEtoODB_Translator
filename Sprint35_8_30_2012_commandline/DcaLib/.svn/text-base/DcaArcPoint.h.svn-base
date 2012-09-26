// $Header: /CAMCAD/DcaLib/DcaArcPoint.h 4     5/21/07 3:47p Kurt Van Ness $

#if !defined(__DcaArcPoint_h__)
#define __DcaArcPoint_h__

#pragma once

#include "DcaPoint2d.h"
#include "DcaContainer.h"
#include "DcaApertureShape.h"

class CArcLinePolygon;
class CPoly;
class CPolyList;
class CArcPoint;
class CWriteFormat;
class CPnt;

enum ArcLineEndTag
{
   arcLineEndTail = 0,
   arcLineEndHead = 1,
   arcLineEndUndefined
};

ArcLineEndTag oppositeArcLineEnd(ArcLineEndTag arcLineEnd);

enum ArcLineJoinTypeTag
{
   arcLineJoinTypeExtension,
   arcLineJoinTypeIntersect,
   arcLineJoinTypeTangent,
   arcLineJoinTypeInvalid,
   arcLineJoinTypeUndefined
};

CString arcLineJoinTypeToString(ArcLineJoinTypeTag tagValue);

enum ArcLineTeriorTag
{
   arcLineInterior = 0,
   arcLineExterior = 1,
   arcLineTeriorUndefined
};

ArcLineTeriorTag oppositeArcLineTerior(ArcLineTeriorTag arcLineTerior);

void ccwTangentPoint(ArcLineEndTag& arcLineEnd,ArcLineTeriorTag& arcLineTerior);
ArcLineEndTag ccwArcLineEnd(ArcLineEndTag arcLineEnd,ArcLineTeriorTag arcLineTerior);
ArcLineTeriorTag ccwTerior(ArcLineEndTag arcLineEnd,ArcLineTeriorTag arcLineTerior);


//_____________________________________________________________________________
class CArcPoint : public CPoint2d
{
public:
   double bulge;

public:

   // Constructors
   CArcPoint() : bulge(0.) {}
   CArcPoint(double X,double Y,double Bulge) : CPoint2d(X,Y), bulge(Bulge) {}
   CArcPoint(const CPnt& point);
   CArcPoint(const CPoint& point) : CPoint2d(point), bulge(0.) {}
   CArcPoint(const CPoint2d& point) : CPoint2d(point), bulge(0.) {}
   CArcPoint(const CArcPoint& point) : CPoint2d(point), bulge(point.bulge) {}
   CArcPoint(const CPoint2d& p0,const CPoint2d& p1,const CPoint2d& center);

   // diagnostics
   void dump(CWriteFormat& writeFormat,int depth = -1) const;
};

//_____________________________________________________________________________
class CArcPointList
{
private:
   CTypedPtrListContainer<CArcPoint*> m_points;

public:
   CArcPointList();
   virtual ~CArcPointList();
   void empty();

public:
   int getCount() const;
   void addVertex(const CArcPoint& point);
   POSITION getHeadPosition() const;
   CArcPoint* getNext(POSITION& pos) const;

   // diagnostics
   void dump(CWriteFormat& writeFormat,int depth = -1) const;
};

//_____________________________________________________________________________
class CArcLine
{
private:
   CArcPoint& m_tail;
   CArcPoint& m_head;

   mutable bool m_parametersValid;
   mutable double m_length;
   mutable double m_radius;
   mutable double m_sagitta;
   mutable double m_apothem;
   mutable CPoint2d m_arcCenter;

   mutable bool m_arcAnglesValid;
   mutable double m_tailAngleRadians;
   mutable double m_headAngleRadians;

   double m_width;
   ApertureShapeTag m_apShape;

   bool m_tangentPointsValid;
   CArcPoint m_tangentPoints[2][2];
   mutable CArcPoint m_joinPoints[2][2];

public:
   CArcLine(CArcPoint& tail,CArcPoint& head);
   CArcLine(const CArcLine& other);
   CArcLine& operator=(const CArcLine& other);

public:
   bool isArc() const;
   bool isLine() const;

   const CArcPoint& getTail() const { return m_tail; }
   const CArcPoint& getHead() const { return m_head; }

   // Arc Properties
   // A chord divides a circle into two arcs. 
   // When the chord is not a diameter, 
   // the smaller arc is the circle sector, the large arc the circle segment.
   // 
   // Bulges == 0.0 describe a line.
   // The sagitta, apothem, radius, and circle center are undefined.
   // 
   // The chord divides the diameter perpendicular to the chord into two pieces.
   // The sagitta is the piece of the diameter from the chord to the circumference.
   // The apothem is the distance from the center of the chord to the center of the circle.
   //
   // Bulges == 1.0 describe a semicircular arc.
   // The chord is a diameter.
   // The sagitta equals the radius.
   // The apothem is zero.
   //
   // For bulges < 1.0, the arc is a circle sector.
   // The sagitta plus apothem equals the radius.
   //
   // For bulges > 1.0, the arc is a circle segment. 
   // The sagitta minus the apothem equals the radius.
   //
   CPoint2d getArcCenter() const;
   double getBulge() const { return m_tail.bulge; }
   double getApothem() const;
   double getSagitta() const;
   double getRadius() const;

   double getTailAngleRadians() const;
   double getHeadAngleRadians() const;
   double getStartAngleRadians() const;
   double getEndAngleRadians() const;

   bool setWidth(double width, ApertureShapeTag apShape = apertureRound);
   CArcPoint getTangentPoint(ArcLineEndTag arcLineEnd,ArcLineTeriorTag arcLineTerior) const;
   CArcPoint& getJoinPoint(ArcLineEndTag arcLineEnd,ArcLineTeriorTag arcLineTerior) const;
   CArcLine getBoundaryArcLine(ArcLineEndTag arcLineEnd,ArcLineTeriorTag arcLineTerior) const;
   bool getTangentPointsValid() const { return m_tangentPointsValid; }
   void moveEnd(ArcLineEndTag end,const CPoint2d& point);

   int intersects(const CArcLine& otherArcLine,CPoint2d& intersectPoint0,CPoint2d& intersectPoint1) const;

   // diagnostics
   void dump(CWriteFormat& writeFormat,int depth = -1) const;

private:
   void calculateArcProperties() const;
   void calculateArcAngles() const;
   void setTangentPoint(ArcLineEndTag arcLineEnd,ArcLineTeriorTag arcLineTerior,const CArcPoint& point);
};

//_____________________________________________________________________________
class CArcLineArray : public CTypedPtrArrayContainer<CArcLine*>
{
public:
   CArcLineArray(int growBySize = 10) : CTypedPtrArrayContainer<CArcLine*>(growBySize) {}

   // diagnostics
   void dump(CWriteFormat& writeFormat,int depth = -1) const;
};

//_____________________________________________________________________________
class CPolyArcLine
{
private:
   CArcPointList m_vertices;
   CArcLineArray* m_lines;
   bool m_closed;

   bool getTeriorLine(CArcLinePolygon& polygon,double width,double tolerance,bool outlineFlag, ApertureShapeTag apShape = apertureRound); // only round and square supported

public:
   CPolyArcLine();
   CPolyArcLine(CPoly& poly);
   virtual ~CPolyArcLine();
   void empty();

   int getLineCount() const;
   int getVertexCount() const;
   void addVertex(const CArcPoint& point);

   // operations
   bool setWidth(double width, ApertureShapeTag apShape = apertureRound);
   bool getOutline(CArcLinePolygon& polygon,double width, double tolerance, ApertureShapeTag apShape = apertureRound);
   bool getInline(CArcLinePolygon& polygon,double width, double tolerance);

   // diagnostics
   void dump(CWriteFormat& writeFormat,int depth = -1) const;

private:
   void allocateLines();
   void deallocateLines();
   CArcLine* getLineAt(int index);
   ArcLineJoinTypeTag getJoin(CArcPoint& joinPoint,const CArcLine& arcLine0,const CArcLine& arcLine1,
      ArcLineTeriorTag terior,double tolerance);

};

//_____________________________________________________________________________
class CArcLinePolygon
{
private:
   CArcPointList m_vertices;

public:
   CArcLinePolygon();
   virtual ~CArcLinePolygon();
   void empty();

public:
   void addVertex(const CArcPoint& point);

   void getPoly(CPoly& poly);
};

#endif
