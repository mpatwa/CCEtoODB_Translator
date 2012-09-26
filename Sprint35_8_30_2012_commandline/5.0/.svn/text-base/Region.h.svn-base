// $Header: /CAMCAD/5.0/Region.h 31    6/21/07 8:27p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#if !defined(__Region_h__)
#define __Region_h__

#pragma once

#include "data.h"

class CCEtoODBDoc;
class Region;
class Rect;
class CSplitRegionPolygons;

Region *RegionFromRgn(CRgn *rgn);
Region *RegionFromPoly(CPoly *poly, float scaleFactor,PageUnitsTag pageUnits);
Region *RegionFromPolylist(CCEtoODBDoc *doc, CPolyList *polyList, float scaleFactor);

Region *IntersectRegions(Region *region1, Region *region2);
Region *UnionRegions(Region *region1, Region *region2);
Region *DiffRegions(Region *parentRegion, Region *voidRegion);
bool AreRegionsEqual(Region *region1, Region *region2);
bool AreRectsEqual(Rect *rect1, Rect *rect2);
bool DoRegionsTouch(Region *region1, Region *region2);
bool DoRectsTouch(Rect *rect1, Rect *rect2);
bool DoRegionsIntersect(Region *region1, Region *region2);
bool DoRectsIntersect(Rect *rect1, Rect *rect2);
Rect *IntersectRects(Rect *rect1, Rect *rect2);

/******************************************************************************
* class Rect
*/
class Rect
{
public:
   Rect(long left, long right, long top, long bottom);
   Rect(const Rect& other);

   long left, right, top, bottom; 

   bool Contains(Rect *rect);

	Rect *Clone()
	{
		return new Rect(left, right, top, bottom);
	}
};

/******************************************************************************
* class Region
*/
class Region
{
private:
   bool m_filledFlag;
   CTypedPtrListContainer<Rect*> m_rects;

public:
   Region(bool filledFlag=false);
   ~Region();

   // accessors
   bool getFilledFLag() { return m_filledFlag; }
   void setFilledFlag(bool flag) { m_filledFlag = true; }

   // Comparison operators based on size (area)
   bool operator> (Region *rvalue) const { return getArea() >  rvalue->getArea(); }
   bool operator>=(Region *rvalue) const { return getArea() >= rvalue->getArea(); }
   bool operator< (Region *rvalue) const { return getArea() <  rvalue->getArea(); }
   bool operator<=(Region *rvalue) const { return getArea() <= rvalue->getArea(); }

   void ClearRegion();
   bool IsEmpty() { return (m_rects.IsEmpty() != 0); }
	Region* Clone();

   void GetExtents(ExtentRect *extents, float scaleFactor);

   CPolyList* GetPolys(float scaleFactor);
   CPolyList* GetSmoothedPolys(float scaleFactor);
   CPolyList* BuildRectPolys(float scaleFactor);
   CPolyList* WrapRects(float scaleFactor);
   CPolyList* SmoothlyWrapRects(float scaleFactor);
   bool CanWrapRegion();
   void fillVerticalConcavities();

   void CombineWith(Region *region);

	void RemoveRect(Rect *rectToRemove);
   void InsertRectInRegion(Rect *rect);

   void Optimize();

   void Dump(CString filename);

   long getArea() const;

   POSITION getHeadPosition()   const { return m_rects.GetHeadPosition(); }
   POSITION getTailPosition()   const { return m_rects.GetTailPosition(); }
   Rect* getNext(POSITION& pos) const { return m_rects.GetNext(pos); }
   Rect* getPrev(POSITION& pos) const { return m_rects.GetPrev(pos); }
   Rect* getAt(POSITION pos)    const { return m_rects.GetAt(pos); }
   Rect* getHead()              const { return m_rects.GetHead(); }
   Rect* getTail()              const { return m_rects.GetTail(); }
   POSITION addTail(Rect* rect)       { return m_rects.AddTail(rect); }
   POSITION addHead(Rect* rect)       { return m_rects.AddHead(rect); }
   void removeAt(POSITION pos)        { m_rects.RemoveAt(pos); }
   POSITION insertAfter(POSITION pos,Rect* rect)  { return m_rects.InsertAfter(pos,rect); }
   POSITION insertBefore(POSITION pos,Rect* rect) { return m_rects.InsertBefore(pos,rect); }
   int getCount() const { return m_rects.GetCount(); }

private:
   void addPntToPoly(CPoly* poly,POINT point,int prevPrevDx,double scale);
};

//_____________________________________________________________________________
class CExtendedRgn : public CRgn
{
private:
   mutable RGNDATA* m_regionData;

public:
   CExtendedRgn();
   CExtendedRgn(const CRgn* rgn);
   ~CExtendedRgn();

   RGNDATA* getRegionData();
   const RGNDATA* getRegionData() const;
   int getRegionDataRectCount() const;
   void setRegionDataRectCount(int newCount);
   RECT* getRegionDataRect(int index);
   const RECT* getRegionDataRect(int index) const;

   bool isValid() const;
   void dump(CWriteFormat& writeFormat,int depth = -1) const;

private:
   void releaseRegionData();
};

//_____________________________________________________________________________
enum RegionRectRelationTag
{
   rectLeftOf = 0,
   rectLeftOfGoingDown,
   rectLeftOfGoingUp,
   rectRightOf,
   rectRightOfGoingDown,
   rectRightOfGoingUp,
   regionRectRelationTagFirst = rectLeftOf,
   regionRectRelationTagLast  = rectRightOfGoingUp,
};

//_____________________________________________________________________________
class CRgnRectList
{
private:
   CExtendedRgn* m_rgn;
   CTypedPtrListContainer<CRect*> m_rectList;

public:
   CRgnRectList();
   ~CRgnRectList();
   void detachRgn();

   void addRectCopy(RECT& rect);
   CExtendedRgn* getRgn();
};

//_____________________________________________________________________________
class CRegionRect
{
private:
   int m_minX;
   int m_minY;
   int m_maxX;
   int m_maxY;
   //int m_row;
   //int m_column;
   CRegionRect* m_relatedRects[regionRectRelationTagLast + 1];
   int m_index;
   int m_perimeterIndex;

public:
   //CRegionRect(int x0,int y0,int x1,int y1);
   CRegionRect(const Rect& rect,int index= -1);
   CRegionRect(const RECT& rect,int index= -1);
   CRegionRect(const CRegionRect& rect,int index= -1);
   void initRelatedRects();

   //void setRowAndColumn(int row,int column);

   int getMinX()   const { return m_minX; }
   int getMinY()   const { return m_minY; }
   int getMaxX()   const { return m_maxX; }
   int getMaxY()   const { return m_maxY; }
   int getWidth()  const { return m_maxX - m_minX; }
   int getHeight() const { return m_maxY - m_minY; }
   CIntExtent getExtent() const;
   int getArea() const;

   void setMinY(int minY);
   void setMaxY(int maxY);

   int getIndex()  const { return m_index; }
   void setIndex(int index) { m_index = index; }

   int getPerimeterIndex()  const { return m_perimeterIndex; }
   void setPerimeterIndex(int index) { m_perimeterIndex = index; }

   void setRelatedRect(RegionRectRelationTag relation,CRegionRect* relatedRect);
   CRegionRect* getRelatedRect(RegionRectRelationTag relation);

   void dump(CWriteFormat& writeFormat,int depth) const;
};

//_____________________________________________________________________________
class CRegionRectList : public CTypedPtrListContainer<CRegionRect*>
{
public:
   CRegionRectList(bool isContainer=true);
   CRegionRectList(const CRegionRectList& other,bool isContainer=true);
   CRegionRectList(CRgn& rgn,bool isContainer=true);
   CRegionRectList(const Region& region,bool isContainer=true);

   CRegionRect* addRect(Rect& rect);
   CRegionRect* addRect(CRegionRect& rect);
   void set(CRgn& rgn);
   void set(const Region& region);
   CIntExtent getExtent() const;
   int getArea() const;
   CExtendedRgn* getRgn() const;
   void initPerimeterIndices();

   void dump(CWriteFormat& writeFormat,int depth = -1) const;
};

//_____________________________________________________________________________
class CPointList
{
private:
   CTypedPtrListContainer<CPoint*> m_points;

public:
   CPointList();
   virtual ~CPointList();
   void empty();
   void takeData(CPointList& other);

   int GetCount() const { return (int)m_points.GetCount(); }
   POSITION GetHeadPosition() const { return m_points.GetHeadPosition(); }
   CPoint* GetNext(POSITION& pos) const { return m_points.GetNext(pos); }

   CPoint getHead();

   CPoint* addPoint(int x,int y);
   CPoint* addPoint(const CPoint& point);
   void    removePoint(POSITION pos);
   void removeCollinearPoints();
   CPolyList* getPolyList(double pixelsPerPageUnits) const;
   void moveHeadToTail();
};

//_____________________________________________________________________________
class CRegionPolygon
{
private:
   CCEtoODBDoc& m_camCadDoc;
   CRegionRectList m_rects;
   bool m_goingDownFlag;
   bool m_leftSideFlag;
   RegionRectRelationTag m_nextRectRelation;
   RegionRectRelationTag m_siblingRectRelation;
   double m_pixelsPerPageUnit;

public:
   CRegionPolygon(CCEtoODBDoc& camCadDoc,CPolyList& polyList,double pixelsPerPageUnit);
   CRegionPolygon(CCEtoODBDoc& camCadDoc,const CRegionRectList& rects,double pixelsPerPageUnit);
   CRegionPolygon(CCEtoODBDoc& camCadDoc,const Region& region,double pixelsPerPageUnit);
   CRegionPolygon(CCEtoODBDoc& camCadDoc,double pixelsPerPageUnit);
   //CRegionPolygon(Region& region);

   CPolyList* getSmoothedPolys();
   CPolyList* getPolys();
   CPolyList* getRectangles();
   bool splitRegion(CSplitRegionPolygons& regionPolygons,
      int lineWidthPixels, int maxFeatureSizePixels,int webSizePixels,bool stripeFlag=false);

   bool getGoingDownFlag() const { return m_goingDownFlag; }
   void setGoingDownFlag(bool flag);
   void toggleGoingDownFlag();

   bool getLeftSideFlag() const { return m_leftSideFlag; }
   void setLeftSideFlag(bool flag);
   void toggleLeftSideFlag();

   RegionRectRelationTag getNextRectRelation() const { return m_nextRectRelation; }
   RegionRectRelationTag getSiblingRectRelation() const { return m_siblingRectRelation; }

   CPoint getPoint(CRegionRect& rect,int pointOrdinate) const;

   static bool logicError(int errorNumber);

private:
   void calcRectRelationships();
   void updateRelations();
   bool getPerimeter(CPointList& perimeterPointList);
   bool getPerimeter(POSITION startPos,int perimeterIndex,CPointList& perimeterPointList,int& area,int& rectCount);
   bool horizontallyBisect(int bisectorLine);
   void verticallySplitRegion(CSplitRegionPolygons& regionPolygons,const CRegionPolygon& polygon,
      int maxFeatureSizePixels,int webSizePixels,const CIntExtent& extent);
   void horizontallySplitRegion(CSplitRegionPolygons& regionPolygons,const CRegionPolygon& polygon,
      int maxFeatureSizePixels,int webSizePixels,const CIntExtent& extent);
};

//_____________________________________________________________________________
class CSplitRegionPolygons
{
private:
   int m_xSize;
   int m_ySize;
   CTypedPtrArrayContainer<CRegionPolygon*> m_polygons;

public:
   CSplitRegionPolygons();

   int getXSize() const { return m_xSize; }
   int getYSize() const { return m_ySize; }
   void setDimensions(int xSize,int ySize);

   CRegionPolygon* getAt(int x,int y);
   void setAt(int x,int y,CRegionPolygon* polygon);

private:
   int getIndex(int x, int y);
};

#endif
