// $Header: /CAMCAD/DcaLib/DcaPoly.h 8     6/21/07 8:30p Kurt Van Ness $

#if !defined(__DcaPoly_h__)
#define __DcaPoly_h__

#pragma once

#include "DcaPnt.h"
#include "DcaApertureShape.h"

class CCamCadData;
class Point2;
class CPoint3d;

//_____________________________________________________________________________
class CPoly
{
private:
   int          m_widthIndex;

   unsigned short Filled:1;
   unsigned short Closed:1;
   unsigned short VoidPoly:1;       // this is drawn like negative, but must have a parent. If a void does not
                                    // have a parent, use a polyline negative.
   unsigned short Hidden:1;         // hide some polys of a polylist
   unsigned short ThermalLines:1;   // this are thermal lines to connect to a plane
   unsigned short BoundaryPoly:1;   // this is a boundary polygon, which is used to flood. This is not displayed 
                                    // and only used for flooding.
   unsigned short Breakout:1;       // this is a breakout line used in SMD padstack definitions.   
   unsigned short HatchLines:1;     // this are the contents draw's of a hatch, fill area.

   CPntList       list;

public:
   CPoly();
   CPoly(const CPoly& other);
   CPoly(const CPntList& pntList);
   CPoly(const CExtent& extent);
   ~CPoly();
   void init();
   void empty();

   // accessors
   int getWidthIndex() const        { return m_widthIndex; }
   int& getWidthIndexRef()          { return m_widthIndex; }
   void setWidthIndex(int index);

   bool isFilled() const              { return (Filled != 0); }
   void setFilled(bool flag)          { Filled = flag;        }
   void setFilled(BOOL flag)          { Filled = (flag != 0); }

   bool isClosed() const              { return (Closed != 0); }  // BEWARE ! This is just a flag, actual data may or may not be closed!
   void setClosed(bool flag)          { Closed = flag;        }
   void setClosed(BOOL flag)          { Closed = (flag != 0); }

   bool isClosedInData();     // Checks if first==last vertex, does not check Closed flag

   bool isVoid() const                { return (VoidPoly != 0); }
   void setVoid(bool flag)            { VoidPoly = flag;        }
   void setVoid(BOOL flag)            { VoidPoly = (flag != 0); }

   bool isHidden() const              { return (Hidden != 0); }
   void setHidden(bool flag)          { Hidden = flag;        }
   void setHidden(BOOL flag)          { Hidden = (flag != 0); }

   bool isThermalLine() const         { return (ThermalLines != 0); }
   void setThermalLine(bool flag)     { ThermalLines = flag;        }
   void setThermalLine(BOOL flag)     { ThermalLines = (flag != 0); }

   bool isFloodBoundary() const       { return (BoundaryPoly != 0); }
   void setFloodBoundary(bool flag)   { BoundaryPoly = flag;        }
   void setFloodBoundary(BOOL flag)   { BoundaryPoly = (flag != 0); }

   bool isBreakOut() const            { return (Breakout != 0); }
   void setBreakOut(bool flag)        { Breakout = flag;        }
   void setBreakOut(BOOL flag)        { Breakout = (flag != 0); }

   bool isHatchLine() const           { return (HatchLines != 0); }
   void setHatchLine(bool flag)       { HatchLines = flag;        }
   void setHatchLine(BOOL flag)       { HatchLines = (flag != 0); }

   CPntList& getPntList()             { return list;              }
   const CPntList& getPntList() const { return list;              }

   // operations
   CPnt* addVertex(double x,double y,double bulge=0.);
   CPnt* addVertex(const CPoint2d& point);
   CPnt* addVertex(const  Point2&  point);

   void transform(const CTMatrix& transformationMatrix,CCamCadData* camCadData=NULL);

   void updateExtent(CExtent& extent) const;
   void updateExtent(CExtent& extent,const CTMatrix& transformationMatrix) const;
   void updateExtent(CExtent& extent,CCamCadData& camCadData) const;
   CExtent getExtent() const;
   CExtent getExtent(const CTMatrix& transformationMatrix) const;

   bool isPointInside(const CPoint2d& point,PageUnitsTag pageUnits,double tolerance = SMALLNUMBER) const;
   bool isPointOnSegment(const CPoint2d& point,PageUnitsTag pageUnits,double tolerance = SMALLNUMBER) const;
   double distanceTo(const CPoint2d& point,CPoint2d* pointResult=NULL) const;
   bool close();
   bool closePolyWithCoincidentEndPoints();
   bool shrink(double distance,PageUnitsTag pageUnits,bool suppressErrorMessages=true) { return list.shrink(distance,pageUnits,suppressErrorMessages); }
   double getArea() const { return list.getArea(); }
   double getLength() const { return list.getLength(); }
   bool contains(const CPoly& other,PageUnitsTag pageUnits,double* returnDistance = NULL);
   void vectorize(PageUnitsTag pageUnits);
   void simplifyBulges(int segmentCountExponent=0) { list.simplifyBulges(segmentCountExponent); }
   void clean(double tolerance) { list.clean(tolerance); }
   int removeDuplicateVertices();
   bool isCcw() const;
   bool makeCcw();
   bool reverse();
   double getLinePolygonArea2() const;
   bool convertToOutline(double width,double tolerance,int zeroWidthIndex, ApertureShapeTag apShape = apertureRound);  // Only round and square actually supported now
   bool convertToInline(double width,double tolerance,int zeroWidthIndex);
	bool hasBulge() const { return list.hasBulge(); }

   // vertices
   POSITION getHeadVertexPosition()   const { return list.GetHeadPosition(); }
   CPnt* getNextVertex(POSITION& pos) const { return list.GetNext(pos); }
   int getVertexCount()               const { return (int)list.GetCount(); }

   // miscellaneous
   CString getDescriptor() const;
   int getSignature() const;

   void dump(CWriteFormat& writeFormat,int depth = -1) const;
   //void WriteXML(CWriteFormat& writeFormat,CCamCadFileWriteProgress& progress);
};

//_____________________________________________________________________________
// Version = "4.5.1469";	// 01/19/06 - Refactored CPolyList - knv
// CPolyList is no longer derived from CTypedPtrListContainer<CPoly*> but exposes access to the list thru methods.
// It is no longer possible to put NULL CPoly pointers into a CPolyList.
class CPolyList
{
private:
   CTypedPtrListContainer<CPoly*> m_polyList;
   short m_fillStyle;

public:
   CPolyList();
   CPolyList(const CPolyList& other);
   CPolyList& operator=(const CPolyList& other);

   // CTypedPtrListContainer access
   void empty() { m_polyList.empty(); }
   POSITION GetHeadPosition() const { return m_polyList.GetHeadPosition(); }
   POSITION GetTailPosition() const { return m_polyList.GetTailPosition(); }
   CPoly*& GetNext(POSITION& rPosition) { return m_polyList.GetNext(rPosition); } 
   CPoly*& GetPrev(POSITION& rPosition) { return m_polyList.GetPrev(rPosition); } 
   const CPoly* GetNext(POSITION& rPosition) const { return m_polyList.GetNext(rPosition); } 
   const CPoly* GetPrev(POSITION& rPosition) const { return m_polyList.GetPrev(rPosition); } 
   INT_PTR GetCount() const { return m_polyList.GetCount(); } 
   CPoly*& GetHead() { return m_polyList.GetHead(); } 
   CPoly*& GetTail() { return m_polyList.GetTail(); } 
   const CPoly* GetHead() const { return m_polyList.GetHead(); } 
   const CPoly* GetTail() const { return m_polyList.GetTail(); } 
   POSITION AddHead(CPoly* poly);
   POSITION AddTail(CPoly* poly);
   POSITION InsertBefore(POSITION pos,CPoly* poly);
   POSITION InsertAfter(POSITION pos,CPoly* poly);
   CPoly*& GetAt(POSITION position) { return m_polyList.GetAt(position); } 
   POSITION Find(CPoly* searchValue,POSITION startAfter = NULL) const { return m_polyList.Find(searchValue,startAfter); }

   int getClosedPolyCount();

   // 
   CPoly* removeHead(); 
   CPoly* removeTail();
   void deleteAt(POSITION position);
   bool isEmpty();
   bool remove(CPoly* poly);

   //
   void transform(const CTMatrix& transformationMatrix,CCamCadData* camCadData=NULL);

   void updateExtent(CExtent& extent) const;
   void updateExtent(CExtent& extent,const CTMatrix& transformationMatrix) const;
   void updateExtent(CExtent& extent,CCamCadData& camCadData) const;
   CExtent getExtent() const;
   CExtent getExtent(const CTMatrix& transformationMatrix) const;
   CExtent getExtent(CCamCadData& camCadData) const;

   bool isPointInside(const CPoint2d& point,PageUnitsTag pageUnits,double tolerance = SMALLNUMBER) const;
   bool isPointOnSegment(const CPoint2d& point,PageUnitsTag pageUnits,double tolerance = SMALLNUMBER) const;
   double distanceTo(const CPoint2d& point,CPoint2d* pointResult=NULL) const;

   void setWidthIndex(int widthIndex);
   bool closePolys();
   bool hasPoints(int atleastThisManyPnts = 1);  // True if atleast one poly in polylist has this many or more points.
   void takeData(CPolyList& other);

   CPoint2d getCenterOfMass(PageUnitsTag pageUnits) const;
   CPoint2d getCenterOfVertices(PageUnitsTag pageUnits) const;
   //bool getMaximumAreaConvexPolygon(CPolygon& polygon) const;
   //bool getMaximumInCircle(CPoint2d& inCircleCenter,double& inCircleRadius,double tolerance) const;
   //bool getMaximumInCircleWithHoleAvoidance(CPoint2d& inCircleCenter,double& inCircleRadius,
   //   CPoint2d holeCenter,double holeRadius,double tolerance) const;
   int getCommonWidthIndex() const;
   bool HasNonZeroWidthPoly(const CCamCadData& camCadData) const;
   CPoly* getMaximumAreaClosedPoly(PageUnitsTag pageUnits) const;

   void setFillStyle(short fillstyle)            { m_fillStyle = fillstyle; }
   //void setFillStyleValidated(short fillstyle)   { setFillStyle(ValidatedFillStyle(fillstyle)); }
   short getFillStyle() const                    { return m_fillStyle; }

   void dump(CWriteFormat& writeFormat,int depth = -1) const;
   //void WriteXML(CWriteFormat& writeFormat,CCamCadFileWriteProgress& progress);
};

//_____________________________________________________________________________
class CPoly3D
{
private:
   int          m_widthIndex;

   unsigned short Filled:1;
   unsigned short Closed:1;
   unsigned short VoidPoly:1;       // this is drawn like negative, but must have a parent. If a void does not
                                    // have a parent, use a polyline negative.
   unsigned short Hidden:1;         // hide some polys of a polylist
   unsigned short ThermalLines:1;   // this are thermal lines to connect to a plane
   unsigned short BoundaryPoly:1;   // this is a boundary polygon, which is used to flood. This is not displayed 
                                    // and only used for flooding.
   unsigned short Breakout:1;       // this is a breakout line used in SMD padstack definitions.   
   unsigned short HatchLines:1;     // this are the contents draw's of a hatch, fill area.

   CPnt3DList       list;

public:
   CPoly3D();
   CPoly3D(const CPoly3D& other);
   CPoly3D(const CPnt3DList& pntList);
   ~CPoly3D();
   void init();
   void empty();

   // accessors
   int getWidthIndex() const        { return m_widthIndex; }
   int& getWidthIndexRef()          { return m_widthIndex; }
   void setWidthIndex(int index);

   bool isFilled() const              { return (Filled != 0); }
   void setFilled(bool flag)          { Filled = flag;        }
   void setFilled(BOOL flag)          { Filled = (flag != 0); }

   bool isClosed() const              { return (Closed != 0); }
   void setClosed(bool flag)          { Closed = flag;        }
   void setClosed(BOOL flag)          { Closed = (flag != 0); }

   bool isVoid() const                { return (VoidPoly != 0); }
   void setVoid(bool flag)            { VoidPoly = flag;        }
   void setVoid(BOOL flag)            { VoidPoly = (flag != 0); }

   bool isHidden() const              { return (Hidden != 0); }
   void setHidden(bool flag)          { Hidden = flag;        }
   void setHidden(BOOL flag)          { Hidden = (flag != 0); }

   bool isThermalLine() const         { return (ThermalLines != 0); }
   void setThermalLine(bool flag)     { ThermalLines = flag;        }
   void setThermalLine(BOOL flag)     { ThermalLines = (flag != 0); }

   bool isFloodBoundary() const       { return (BoundaryPoly != 0); }
   void setFloodBoundary(bool flag)   { BoundaryPoly = flag;        }
   void setFloodBoundary(BOOL flag)   { BoundaryPoly = (flag != 0); }

   bool isBreakOut() const            { return (Breakout != 0); }
   void setBreakOut(bool flag)        { Breakout = flag;        }
   void setBreakOut(BOOL flag)        { Breakout = (flag != 0); }

   bool isHatchLine() const           { return (HatchLines != 0); }
   void setHatchLine(bool flag)       { HatchLines = flag;        }
   void setHatchLine(BOOL flag)       { HatchLines = (flag != 0); }

   CPnt3DList& getPntList()             { return list;              }
   const CPnt3DList& getPntList() const { return list;              }

   // operations
   CPnt3D* addVertex(double x,double y,double z);
   CPnt3D* addVertex(const CPoint3d& point);

   void transform(const CTMatrix& transformationMatrix,CCamCadData* camCadData=NULL);

   // vertices
   POSITION getHeadVertexPosition()   const { return list.GetHeadPosition(); }
   CPnt3D* getNextVertex(POSITION& pos) const { return list.GetNext(pos); }
   int getVertexCount()               const { return (int)list.GetCount(); }
  
   void dump(CWriteFormat& writeFormat,int depth = -1) const;
};

//_____________________________________________________________________________
class CPoly3DList
{
private:
   CTypedPtrListContainer<CPoly3D*> m_polyList;
   short m_fillStyle;

public:
   CPoly3DList();
   CPoly3DList(const CPoly3DList& other);
   CPoly3DList& operator=(const CPoly3DList& other);

   // CTypedPtrListContainer access
   void empty() { m_polyList.empty(); }
   POSITION GetHeadPosition() const { return m_polyList.GetHeadPosition(); }
   POSITION GetTailPosition() const { return m_polyList.GetTailPosition(); }
   CPoly3D*& GetNext(POSITION& rPosition) { return m_polyList.GetNext(rPosition); } 
   CPoly3D*& GetPrev(POSITION& rPosition) { return m_polyList.GetPrev(rPosition); } 
   const CPoly3D* GetNext(POSITION& rPosition) const { return m_polyList.GetNext(rPosition); } 
   const CPoly3D* GetPrev(POSITION& rPosition) const { return m_polyList.GetPrev(rPosition); } 
   INT_PTR GetCount() const { return m_polyList.GetCount(); } 
   CPoly3D*& GetHead() { return m_polyList.GetHead(); } 
   CPoly3D*& GetTail() { return m_polyList.GetTail(); } 
   const CPoly3D* GetHead() const { return m_polyList.GetHead(); } 
   const CPoly3D* GetTail() const { return m_polyList.GetTail(); } 
   POSITION AddHead(CPoly3D* poly);
   POSITION AddTail(CPoly3D* poly);
   POSITION InsertBefore(POSITION pos,CPoly3D* poly);
   POSITION InsertAfter(POSITION pos,CPoly3D* poly);
   CPoly3D*& GetAt(POSITION position) { return m_polyList.GetAt(position); } 
   POSITION Find(CPoly3D* searchValue,POSITION startAfter = NULL) const { return m_polyList.Find(searchValue,startAfter); }

   // 
   CPoly3D* removeHead(); 
   CPoly3D* removeTail();
   void deleteAt(POSITION position);
   bool isEmpty();
   bool remove(CPoly3D* poly);

   //
   void transform(const CTMatrix& transformationMatrix,CCamCadData* camCadData=NULL);

   void setWidthIndex(int widthIndex);
   bool hasPoints();
   void takeData(CPoly3DList& other);

   int getCommonWidthIndex() const;
   bool HasNonZeroWidthPoly(const CCamCadData& camCadData) const;

   void setFillStyle(short fillstyle)            { m_fillStyle = fillstyle; }
   short getFillStyle() const                    { return m_fillStyle; }

   void dump(CWriteFormat& writeFormat,int depth = -1) const;
};
#endif
