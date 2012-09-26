// $Header: /CAMCAD/DcaLib/DcaPolygon.h 4     6/21/07 8:30p Kurt Van Ness $

#if !defined(__DcaPolygon_h__)
#define __DcaPolygon_h__

#pragma once

#include "DcaContainer.h"
#include "DcaPoint2d.h"
#include "DcaExtent.h"
#include "Dca.h"
#include "DcaQfe.h"

class CPolygonEdgeIterator;
class CTMatrix;
class CLine2d;
class CPoly;
class CPolygonCluster;
enum PageUnitsTag;

//class CPolygonQfeList;

#if defined(UseAlgoCOMsPolygonLibrary)
#import "..\\AlgoCOMsPolygon.dll" no_namespace
#endif

//_____________________________________________________________________________
class CPolygonPointList 
{
   friend CPolygonEdgeIterator;

private:
   CTypedPtrListContainer<CPoint2d*> m_pointList;

public:
   CPolygonPointList& operator=(const CPolygonPointList& other);

   void empty() { m_pointList.empty(); }
   int addPoint(const CPoint2d& point);
   POSITION GetHeadPosition() const { return m_pointList.GetHeadPosition(); }
   CPoint2d* GetNext(POSITION& pos) const { return m_pointList.GetNext(pos); }
   CPoint2d* GetHead() const { return m_pointList.GetHead(); }
   CPoint2d* GetTail() const { return m_pointList.GetTail(); }
   void RemoveAt(POSITION pos) { m_pointList.RemoveAt(pos); }
   void rotateHeadToTail() { m_pointList.AddTail(m_pointList.RemoveHead()); }
   void reverse();
   INT_PTR GetCount() const { return m_pointList.GetCount(); }
   void transform(const CTMatrix& matrix);
};

//_____________________________________________________________________________
class CPolygonEdgeIterator 
{
private:
   const CPolygonPointList& m_list;
   bool m_atEnd;
   POSITION m_pos;
   CPoint2d m_firstPoint;
   CPoint2d m_currentPoint;
   int m_index;

public:
   CPolygonEdgeIterator(const CPolygonPointList& list);

   bool rewind();
   bool getNext(CLine2d& line);
   bool posEqual(const CPolygonEdgeIterator& other) const { return (m_pos == other.m_pos); }
   int getIndex() const { return m_index; }
};

//_____________________________________________________________________________
class CPolygon 
{
private:
   CPolygonPointList m_pointList;
   PageUnitsTag m_pageUnits;
   mutable CExtent m_extent;
   mutable Bool3Tag m_isValid;
   mutable Bool3Tag m_isConvex;
   mutable Bool3Tag m_isCcw;
   mutable bool m_isNormalized;

public:
   CPolygon(PageUnitsTag pageUnits);
   CPolygon(const CPolygon& other);
   CPolygon(const CPoly& poly,PageUnitsTag pageUnits);
   CPolygon& operator=(const CPolygon& other);

   void resetState();
   void empty();

   // accessors
   const CExtent& getExtent() const;
   CString getSignature() const;
   CString getSignature2() const;
   PageUnitsTag getPageUnits() const { return m_pageUnits; }

   // operations
   bool makeCcw();
   bool makeConvex();
   void transform(const CTMatrix& matrix);

   bool normalizeByLongestEdgeAndCenterOfMass(CTMatrix& matrix);
   bool normalizeByMinimizingExtents(CTMatrix& matrix);
   bool normalizeForSignature(CTMatrix& matrix);

#ifdef DefineNormalizeByMinimizingEdgeAngles
   bool normalizeByMinimizingEdgeAngles(CTMatrix& matrix);
#endif

   void makePoly(CPoly& poly) const;
   CPoly* makeNewPoly() const;

   // queries
   bool intersects(const CPolygon& other) const;
   bool isValid() const;
   bool isConvex() const;
   bool isCcw() const;
   bool getCircumcircleRadii(double& minRadius,double& maxRadius) const;
   bool allVerticesOnCircumcircle(double tolerance) const;
   bool isNormalized() const { return m_isNormalized; }
   bool areAllPointsInside(const CPolygon& other) const;
   bool isInside(const CPoint2d& point) const;
   bool isOutsideAndCloserThanDistanceSquared(const CPoint2d& point,double distanceSquared) const;
   bool isInsideAndCloserThanDistanceSquared(const CPoint2d& point,double distanceSquared) const;
   bool isInsideAndCloserThanDistanceSquared(const CLine2d& line,double distanceSquared) const;
   bool selfIntersects() const;
   double distanceTo(const CPoint2d& point) const;
   double getArea2() const;
   double getArea() const;
   void getLeftRightAreasFromPoint(double& leftArea,double& rightArea,const CPoint2d& point,bool debugFlag = false) const;
   CPoint2d getCenterOfVertices() const;
   CPoint2d getCenterOfMass() const;
   bool normalizedFpeq(const CPolygon& other) const;
   bool normalizedFpeq(const CPolygon& other,double tolerance) const;
   bool normalizedVerticesEqual(const CPolygon& other,double toleranceDistance) const;

   //bool depthSearchNearestInCircleCenter(
   //   CPoint2d& pointNearestInCircleCenter,double& inCircleRadius,
   //   int depth,const CPoint2d& probePoint,const CSize2d& gridSize,double tolerance) const;

   //bool depthSearchNearestInCircleCenterWithHoleAvoidance(
   //   CPoint2d& pointNearestInCircleCenter,double& inCircleRadius,
   //   CPoint2d holeCenter,double holeRadius,
   //   int depth,const CPoint2d& probePoint,const CSize2d& gridSize,double tolerance) const;

   //bool breadthSearchNearestInCircleCenter(
   //   CPoint2d& pointNearestInCircleCenter,double& inCircleRadius,
   //   int depth,const CPoint2d& probePoint,const CSize2d& gridSize,int maxDepth) const;

   //bool breadthSearchNearestInCircleCenterWithHoleAvoidance(
   //   CPoint2d& pointNearestInCircleCenter,double& inCircleRadius,
   //   CPoint2d holeCenter,double holeRadius,
   //   int depth,const CPoint2d& probePoint,const CSize2d& gridSize,int maxDepth) const;

   //bool getInCircleCenter(CPoint2d& inCircleCenter,double& circleRadius,double tolerance);
   //bool getInCircleCenterWithHoleAvoidance(CPoint2d& inCircleCenter,double& circleRadius,double tolerance,
   //   CPoint2d holeCenter,double holeRadius);

   // vertices
   int getVertexCount() const { return m_pointList.GetCount(); }
   POSITION getHeadPointPosition() const { return m_pointList.GetHeadPosition(); }
   CPoint2d getNextPoint(POSITION& pos) const { return *(m_pointList.GetNext(pos)); }
   int addVertex(const CPoint2d& point);
   bool merge(CPolygon& other);
   void setVertices(const CPoly& poly);

   // diagnostics
   void write(CWriteFormat& writeFormat);
};

//_____________________________________________________________________________
class CPolygonSubdivisionVertex 
{
private:
   int m_id;
   CPoint2d m_point;
   CPolygonSubdivisionVertex* m_previous[2];
   CPolygonSubdivisionVertex* m_next[2];

public:
   CPolygonSubdivisionVertex(int id,const CPoint2d& point);

   CPoint2d getPoint() const { return m_point; }
   CPolygonSubdivisionVertex* getPrevious(int index) const;
   void setPrevious(int index,CPolygonSubdivisionVertex* previous);
   CPolygonSubdivisionVertex* getNext(int index) const;
   void setNext(int index,CPolygonSubdivisionVertex* next);

   void append(int index,CPolygonSubdivisionVertex* appendedVertex);
   void merge(int index,CPolygonSubdivisionVertex* mergedVertex);
   bool isIntersection() const;

   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
class CPolygonSubdivision
{
private:
   static unsigned int m_logicErrorFlags;
   static bool m_debugMergeFlag;

   CPolygon& m_polygon0;
   CPolygon& m_polygon1;
   const CPolygon* m_polygons[2];
   CPolygonSubdivisionVertex* m_head[2];
   CTypedPtrListContainer<CPolygonSubdivisionVertex*> m_vertices;

public:
   CPolygonSubdivision(CPolygon& polygon0,CPolygon& polygon1);
   ~CPolygonSubdivision();

   bool merge(CPolygon& mergedPolygon);

private:
   CPolygonSubdivisionVertex* addNewVertex(const CPoint2d& point);
   void addPolygon(int index);   

public:
   static void logicError(int errorNumber);
   static bool getDebugMergeFlag() { return m_debugMergeFlag; }
   static void setDebugMergeFlag(bool flag) { m_debugMergeFlag = flag; }

   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
class CConvexPolygon : public CPolygon 
{
};

//_____________________________________________________________________________
class COvalPolygon : public CConvexPolygon 
{
private:
   CPoint2d m_p0;
   CPoint2d m_p1;
   double m_width;

public:
   COvalPolygon(const CPoint2d& p0,const CPoint2d& p1,double width);

};

//_____________________________________________________________________________
class CRectanglePolygon : public COvalPolygon 
{
public:
   CRectanglePolygon(const CPoint2d& p0,const CPoint2d& p1,double width);
};

//_____________________________________________________________________________
class CTrianglePolygon : public CConvexPolygon 
{
public:
   CTrianglePolygon(const CPoint2d& p0,const CPoint2d& p1,const CPoint2d& p2);
};

//_____________________________________________________________________________
class CPolygonList : public CTypedPtrListContainer<CPolygon*> 
{
};

//_____________________________________________________________________________
class CPolygonQfe : public CTypedQfe<CPolygon>
{
private:
   CPolygon* m_polygon;
   CPolygonCluster* m_cluster;

public:
   CPolygonQfe(const CPoint2d& origin,CPolygon* polygon);
   virtual ~CPolygonQfe();

   CPolygon& getPolygon() { return *m_polygon; }

   CPolygonCluster* getCluster() { return m_cluster; }
   void setCluster(CPolygonCluster& cluster) { m_cluster = &cluster; }
};

//_____________________________________________________________________________
class CPolygonQfeTree : public CDerivedQfeTree<CPolygonQfe>
{
public:
   CPolygonQfeTree(bool isContainer = true);
};

//_____________________________________________________________________________
class CPolygonQfeList : public CDerivedQfeList<CPolygonQfe>
{
};

//_____________________________________________________________________________
class CPolygonCluster
{
private:
   CPolygonQfeList m_list;
   CPolygon m_polygon;       // merged polygon

public:
   CPolygonCluster(CPolygonQfe& polygonQfe);

   CPolygon& getPolygon() { return m_polygon; }
   CPolygonQfeList& getList() { return m_list; }
   bool merge(CPolygonCluster& other);

};

//_____________________________________________________________________________
class CPolygonClusterMap
{
private:
   CTypedPtrMap<CMapPtrToPtr,CPolygonQfe*,CPolygonCluster*> m_map;

public:
   CPolygonClusterMap();
   ~CPolygonClusterMap();

   //bool lookup(CPolygonQfeList* qfeList);
   //bool setAt(CPolygonQfeList* qfeList);
   //bool deleteAt(CPolygonQfeList* qfeList);
   bool merge(CPolygonQfe& polygonQfe0,CPolygonQfe& polygonQfe1);
   POSITION getStartPosition() { return m_map.GetStartPosition(); }
   void getNextAssoc(POSITION& pos,CPolygonCluster*& list) 
      { CPolygonQfe* key;  return m_map.GetNextAssoc(pos,key,list); }
};

#if defined(UseAlgoCOMsPolygonLibrary)

//_____________________________________________________________________________
class CAlgoComPolygon
{
private:
   IACGenPolygonPtr m_algoComPolygon;

public:
   CAlgoComPolygon();
   CAlgoComPolygon(CPolygon& polygon);

   void addPolygon(CPolygon& polygon);
   void addPolygon(CAlgoComPolygon& algoComPolygon);

   int getPolygonCount();

   bool getPolygonAt(CPolygon& polygon,int index);
};

#endif  // defined(UseAlgoCOMsPolygonLibrary)

#endif
