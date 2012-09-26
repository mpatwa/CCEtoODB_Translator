// $Header: /CAMCAD/DcaLib/DcaQfeLib.h 3     3/09/07 5:15p Kurt Van Ness $

#if !defined(__DcaQfeLib_h__)
#define __DcaQfeLib_h__

#pragma once

#include "DcaPoint2d.h"

class CExtent;
class CWriteFormat;

//=============================================================================

/*
History: QfeLib.h $
 * 
 * *****************  Version 3  *****************
 * User: Kvanness     Date: 6/25/00    Time: 2:35p
 * Updated in $/PadsTool
 * Replaced CGraphicsNode2d::m_object with m_objectList and 
 * modified all logic.
 * Added m_isContainer member to CObject2dList and
 * constructor, destructor, and empty().
 * Replaced CTreeMetrics::m_count with m_nodeCount and m_objectCount.
 * Replaced CTreeMetrics::incCount() with incNodeCount() and
 * incObjectCount().
 * 
 * *****************  Version 2  *****************
 * User: Kvanness     Date: 6/24/00    Time: 3:02p
 * Updated in $/PadsTool
 * Added m_parent, depth measurement and other debugging aids.
 * Added nonrecursive metric reports.
 * 
 * *****************  Version 1  *****************
 * User: Kvanness     Date: 9/16/99    Time: 6:42a
 * Created in $/PadsTool
 * Initial add.
*/

//_____________________________________________________________________________
class CObject2d : public CObject
{
   //DECLARE_DYNAMIC(CObject2d);

public:
   virtual bool isExtentIntersecting(const CExtent& extent) = 0;
   virtual bool isPointWithinDistance(const CPoint2d& point,double distance) = 0;
   virtual bool isInViolation(CObject2d& otherObject) = 0;
   virtual bool isValid() const { return true; }
   virtual int getObjectType() const = 0;
};

//_____________________________________________________________________________
class CObject2dList : public CTypedPtrList<CObList,CObject2d*>
{
private:
   bool m_isContainer;

public:
   CObject2dList(int nBlockSize);
   CObject2dList(bool isContainer=false,int nBlockSize = 10);
   ~CObject2dList();

   void empty();
};

//_____________________________________________________________________________
class CTreeMetrics : public CObject
{
private:
   int m_nodeCount;
   int m_objectCount;
   int m_depth;
   int m_maxDepth;
   int m_leafCnt;
   int m_depthSum;
   CWordArray m_depths;

public:
   CTreeMetrics();
   ~CTreeMetrics();

   int getMaxDepth() { return m_maxDepth; }
   double getAverageNodeDepth();
   void getOptimumDepths(int& optimumMaxDepth,double& optimumAverageDepth);

   void reset();
   int incNodeCount();
   int incObjectCount(int count=1);
   int incDepth();
   int decDepth();
   int incLeaf();
   int logDepth();
   void logNode(int objectCount);
   void report(CWriteFormat* writeFormat,const CString& title);
};

//#define ImplementCGraphicsNode2dParent

//_____________________________________________________________________________
class CGraphicsNode2d : public CObject
{
public:
   CGraphicsNode2d* m_l;  // left son node
   CGraphicsNode2d* m_r;  // right son node
   CPoint2d m_key;
   CObject2dList m_objectList;

#ifdef ImplementCGraphicsNode2dParent
   CGraphicsNode2d* m_parent;  // parent node (added for debugging, only used for debugging)
#endif

public:
   CGraphicsNode2d(const CPoint2d& keyValue,CObject2d* object,bool isContainer);
   ~CGraphicsNode2d();
   void empty();

   // access
   CGraphicsNode2d* getLeft();
   CGraphicsNode2d* getRight();
   CPoint2d getKey() { return m_key; }
   int getObjectCount() { return m_objectList.GetCount(); }

#ifdef ImplementCGraphicsNode2dParent
   CGraphicsNode2d* getParent();
   void setParent(CGraphicsNode2d* node) { m_parent = node; }
#endif

   int search(const CExtent& extent,CObject2dList& foundList,bool useX);
   bool executeFunction(const CExtent& extent,bool useX,
      bool (*function)(const CExtent& extent,void* object,void* param),void* param);
   void addObject(CObject2d* object);

   CObject2d* findFirst(const CExtent& extent,bool useX);
   CObject2d* findFirstEdgeToRectViolation(const CExtent& extent,int& entitiesChecked,bool useX);
   CObject2d* findFirstEdgeToPointViolation(const CExtent& extent,
      const CPoint2d& point,double distance,int& entitiesChecked,bool useX);
   CObject2d* findFirstViolation(const CExtent& extent,CObject2d& checkingObject,bool useX);
   int findAllViolations(const CExtent& extent,CObject2d& checkingObject,CObject2dList& foundList,bool useX);

   void getMetrics(CTreeMetrics& treeMetrics);
   void getMetricsNonRecursive(CTreeMetrics& treeMetrics);

   void trace(CWriteFormat* writeFormat,bool useX);
   bool isValid(bool useX) const;
   bool isNodeValid(bool useX) const;
};

//_____________________________________________________________________________
class CGraphicsTree2d : public CObject
{
   //DECLARE_DYNAMIC(CGraphicsTree2d);

private: 
   bool m_isContainer;
   int m_objectCount;
   int m_nodeCount;

protected:
   CGraphicsNode2d m_head;

public:
   CGraphicsTree2d(bool isContainer = false);
   ~CGraphicsTree2d();

public:
   int getObjectCount() { return m_objectCount; }
   int getNodeCount()   { return m_nodeCount; }
   void empty();

   // Inserts object at key keyValue, multiple objects may be inserted with the same key
   virtual void setAt(const CPoint2d& keyValue,CObject2d* object);

   // returns all objects with keys inside or on the extent
   virtual int search(const CExtent& extent,CObject2dList& foundList);
   virtual bool executeFunction(const CExtent& extent,
      bool (*function)(const CExtent& extent,void* object,void* param),void* param);

   CObject2d* findFirst(const CExtent& extent);
   CObject2d* findFirstEdgeToRectViolation(const CExtent& extent,int& entitiesChecked);
   CObject2d* findFirstEdgeToPointViolation(const CExtent& extent,
      const CPoint2d& point,double distance,int& entitiesChecked);
   CObject2d* findFirstViolation(const CExtent& extent,CObject2d& checkingObject);
   int findAllViolations(const CExtent& extent,CObject2d& checkingObject,CObject2dList& foundList);

   void getMetrics(CTreeMetrics& treeMetrics);
   void getMetricsNonRecursive(CTreeMetrics& treeMetrics);
   void trace(CWriteFormat* writeFormat);
   void reportMetrics(CWriteFormat* writeFormat,const char* title);

   bool isValid() const;
   bool test();
};

//=============================================================================

/*
History: QfeLib.h $
 * 
 * *****************  Version 4  *****************
 * User: Kvanness     Date: 6/25/00    Time: 2:17p
 * Updated in $/PadsTool
 * Replaced CQfeExtentLimitedContainer::getCount() with 
 * getObjectCount() and getNodeCount();
 * 
 * *****************  Version 3  *****************
 * User: Kvanness     Date: 6/24/00    Time: 2:59p
 * Updated in $/PadsTool
 * Virtual rollback to undo stubbed test point implementation.
 * Added isValid()
 * 
 * *****************  Version 1  *****************
 * User: Kvanness     Date: 9/16/99    Time: 6:42a
 * Created in $/PadsTool
 * Initial add.
*/

//_____________________________________________________________________________
typedef enum layerSpecTag
{
   lsTop, lsBottom, lsExterior, lsInterior, lsAll

};

class CLayerIndexList : public CList<int,int>
{
};

class CLayerSpec : public CObject
{
private:
   int m_minLayer;
   int m_maxLayer;
   int m_topLayer;
   int m_botLayer;
   mutable CLayerIndexList m_layerList;
   CWordArray m_layerArray;

public:
   CLayerSpec(int minLayer,int maxLayer,int topLayer,int bottomLayer);
   ~CLayerSpec();

   void add(int layer) { set(layer,true); }
   void add(layerSpecTag layerSpec) { set(layerSpec,true); }
   void remove(int layer) { set(layer,false); }
   void remove(layerSpecTag layerSpec) { set(layerSpec,false); }
   void set(int layer,bool select);
   void set(layerSpecTag layerSpec,bool select);
   void set(int layer) { empty();  set(layer,true); }
   void set(layerSpecTag layerSpec) { empty();  set(layerSpec,true); }
   void empty();
   bool isSet(int layer);
   bool isSet(layerSpecTag);

   int getTopLayer() { return m_topLayer; }
   int getBottomLayer() { return m_botLayer; }
   POSITION GetHeadPosition() const;
   int GetNext(POSITION& pos) const;
};

//_____________________________________________________________________________
class CQfe : public CObject2d
{
   //DECLARE_DYNAMIC(CQfe);

public:
   CQfe();
   ~CQfe();

public:
   virtual CPoint2d getOrigin() const = 0;
   virtual CExtent getExtent() const = 0;
   virtual CString getInfoString() const = 0;
   virtual bool isValid() const;
   virtual void assertValid() const;
};

//_____________________________________________________________________________
class CQfeList : public CTypedPtrList<CObject2dList,CQfe*>
{
public:
   CQfeList(int nBlockSize=200);

   bool takeData(CQfeList& other);

   virtual bool isValid() const;
   virtual void assertValid() const;
};

//_____________________________________________________________________________
class CQfeTree : public CGraphicsTree2d
{
public:
   CQfeTree(bool isContainer=true);
   ~CQfeTree();
};

//_____________________________________________________________________________
class CQfeSearchableContainer : public CObject
{
public:
   //CQfeSearchableContainer();
   //~CQfeSearchableContainer();

   //
   virtual int search(const CExtent& extent,CQfeList& foundList) = 0;
   virtual bool executeFunction(const CExtent& extent,
      bool (*function)(const CExtent& extent,void* object,void* param),void* param);

   virtual CQfe* findFirst(const CExtent& extent) = 0;
   virtual CQfe* findFirstViolation(const CExtent& extent,CQfe& qfe) = 0;
   virtual CQfe* findFirstEdgeToRectViolation(const CExtent& extent,int& entitiesChecked) = 0;
   virtual CQfe* findFirstEdgeToPointViolation(const CExtent& extent,
      const CPoint2d& point,double distance,int& entitiesChecked) = 0;

   virtual void setAt(CQfe* qfe) = 0;

   virtual bool isValid() const = 0;
   virtual void assertValid() const = 0;
};

//_____________________________________________________________________________
class CQfeExtentLimitedContainer : public CQfeSearchableContainer
{
private:
   CSize2d m_maxElementExtentSize;
   CQfeTree m_tree;
   bool m_updateExtentSize;

public:
   CQfeExtentLimitedContainer(const CSize2d& maxElementExtentSize,bool updateExtentSize=false,bool isContainer=true);
   ~CQfeExtentLimitedContainer();
   void deleteAll();

public:
   int getObjectCount() { return m_tree.getObjectCount(); }
   int getNodeCount()   { return m_tree.getNodeCount(); }

public:
   virtual int search(const CExtent& extent,CQfeList& foundList);
   virtual bool executeFunction(const CExtent& extent,
      bool (*function)(const CExtent& extent,void* object,void* param),void* param);

   virtual CQfe* findFirst(const CExtent& extent);
   virtual CQfe* findFirstViolation(const CExtent& extent,CQfe& qfe);
   virtual CQfe* findFirstEdgeToRectViolation(const CExtent& extent,int& entitiesChecked);
   virtual CQfe* findFirstEdgeToPointViolation(const CExtent& extent,
      const CPoint2d& point,double distance,int& entitiesChecked);
   virtual int findAllViolations(const CExtent& extent,CQfe& qfe,CQfeList& foundList);

   virtual void setAt(CQfe* qfe);
   void printMetricsReport(CWriteFormat& writeFormat);
   void getTreeMetrics(CTreeMetrics& treeMetrics);
   void setMaxElementExtentSize(const CSize2d& extentSize) { m_maxElementExtentSize = extentSize; }

   virtual bool isValid() const;
   virtual void assertValid() const;
};

//_____________________________________________________________________________
class CQfeExtentLimitedContainerArray : 
   public CTypedPtrArray<CObArray,CQfeExtentLimitedContainer*>
{
public:
   virtual bool isValid() const;
   virtual void assertValid() const;
};

//_____________________________________________________________________________
class CQfeExtentLimitedContainerList : 
   public CTypedPtrList<CObList,CQfeExtentLimitedContainer*>
{
public:
   virtual bool isValid() const;
   virtual void assertValid() const;
};

//_____________________________________________________________________________
class CQfeGraduatedExtentLimitedContainer : public CQfeSearchableContainer
{
private:
   double m_granularity;    // minimum feature size
   double m_maxFeatureSize; // maximum feature size
   int m_arrayIndexSize;
   CQfeExtentLimitedContainerArray m_containerArray;
   CQfeExtentLimitedContainerList m_containerList;

public:
   CQfeGraduatedExtentLimitedContainer(double granularity,double maxFeatureSize);
   ~CQfeGraduatedExtentLimitedContainer();
   void deleteAll();

public:
   virtual int search(const CExtent& extent,CQfeList& foundList);
   virtual bool executeFunction(const CExtent& extent,
      bool (*function)(const CExtent& extent,void* object,void* param),void* param);

   virtual CQfe* findFirst(const CExtent& extent);
   virtual CQfe* findFirstViolation(const CExtent& extent,CQfe& qfe);
   virtual CQfe* findFirstEdgeToRectViolation(const CExtent& extent,int& entitiesChecked);
   virtual CQfe* findFirstEdgeToPointViolation(const CExtent& extent,
      const CPoint2d& point,double distance,int& entitiesChecked);
   virtual int findAllViolations(const CExtent& extent,CQfe& qfe,CQfeList& foundList);

   virtual void setAt(CQfe* qfe);

public:
   void traceDump(CWriteFormat* writeFormat,const char* header);
   void printMetricsReport(CWriteFormat& writeFormat);

   virtual bool isValid() const;
   virtual void assertValid() const;
};

//_____________________________________________________________________________
class CQfeGraduatedExtentLimitedContainerArray : 
   public CTypedPtrArray<CObArray,CQfeGraduatedExtentLimitedContainer*>
{
public:
   virtual bool isValid() const;
   virtual void assertValid() const;
};

//_____________________________________________________________________________
class CQfeLayeredContainer : public CObject
{
private:
   int m_maxLayer;
   double m_granularity;    // minimum feature size
   double m_maxFeatureSize; // maximum feature size
   CQfeGraduatedExtentLimitedContainerArray m_qfeLayers;

public:
   CQfeLayeredContainer(int numLayers,double granularity,double maxFeatureSize);
   ~CQfeLayeredContainer();
   void deleteAll();

protected:
   int search(const CLayerSpec& layerSpec,const CExtent& extent,CQfeList& foundList);
   bool executeFunction(const CLayerSpec& layerSpec,const CExtent& extent,
      bool (*function)(const CExtent& extent,void* object,void* param),void* param);

   CQfe* findFirst(const CLayerSpec& layerSpec,const CExtent& extent);
   CQfe* findFirstViolation(const CLayerSpec& layerSpec,const CExtent& extent,CQfe& qfe);
   CQfe* findFirstEdgeToRectViolation(const CLayerSpec& layerSpec,const CExtent& extent,int& entitiesChecked);
   CQfe* findFirstEdgeToPointViolation(const CLayerSpec& layerSpec,const CExtent& extent,
      const CPoint2d& point,double distance,int& entitiesChecked);

   void setAt(int layer,CQfe* qfe);

public:
   int findAllViolations(const CLayerSpec& layerSpec,const CExtent& extent,CQfe& qfe,CQfeList& foundList);
   void traceDump(CWriteFormat* writeFormat,const char* header);
   void printMetricsReport(CWriteFormat& writeFormat);

public:
   virtual bool isValid() const;
   virtual void assertValid() const;
};

#endif
