// $Header: /CAMCAD/4.3/GeometryTree.h 6     8/12/03 9:06p Kurt Van Ness $

#if !defined(__GeometryTree_h__)
#define __GeometryTree_h__

#pragma once

#if defined(EnableGeometryTreeImplementation)

#include "Data.h"
#include "TMState.h"
#include "Extent.h"
#include "QfeLib.h"
#include "TypedContainer.h"

// Gte - Geometry Tree Entity
#define GteTypePolyInsertInstance  1
#define GteTypePointInsertInstance 2
#define GteTypeTextInsertInstance  3

class CInsertNode;
class CGeometryTree;

//_____________________________________________________________________________
class CXxxxx : public CObject
{
private:

public:
   CXxxxx();
   ~CXxxxx();
};

//_____________________________________________________________________________
class CGte : public CQfe
{
public:
   virtual void draw(CCEtoODBDoc& camCadDoc,CDC& dc) = 0;
};

//_____________________________________________________________________________
class CGteList : public CTypedPtrList<CQfeList,CGte*>
{
public:
   CGteList(int nBlockSize=200) : CTypedPtrList<CQfeList,CGte*>(nBlockSize) { }
};

//_____________________________________________________________________________
class CInsertInstance : public CGte
{
protected:
   CExtent m_extent;
   CInsertNode* m_parentNode;

public:
   CInsertInstance(CInsertNode& parentNode);
   virtual ~CInsertInstance();

public:
   // CGte virtual overrides
   virtual void draw(CCEtoODBDoc& camCadDoc,CDC& dc) = 0;

   // CQfe virtual overrides
   virtual CPoint2d getOrigin() const;
   virtual CExtent getExtent() const;
   virtual CString getInfoString() const = 0;

   // CObjetct2d virtual overrides
   virtual bool isExtentIntersecting(const CExtent& extent);
   virtual bool isPointWithinDistance(const CPoint2d& point,double distance) = 0;
   virtual bool isInViolation(CObject2d& otherObject);
   virtual int getObjectType() const = 0;
};

//_____________________________________________________________________________
class CPolyInsertInstance : public CInsertInstance
{
private:
   CPolyList* m_poly;

public:
   CPolyInsertInstance(CInsertNode& parentNode,CPolyList& poly,
      const CTMatrix& transformationMatrix);
   ~CPolyInsertInstance();

public:
   // CGte virtual overrides
   virtual void draw(CCEtoODBDoc& camCadDoc,CDC& dc);

   // CQfe virtual overrides
   virtual CString getInfoString() const;

   // CObjetct2d virtual overrides
   virtual bool isPointWithinDistance(const CPoint2d& point,double distance);
   virtual int getObjectType() const;
};

//_____________________________________________________________________________
class CPointInsertInstance : public CInsertInstance
{
private:
   PointStruct* m_point;

public:
   CPointInsertInstance(CInsertNode& parentNode,PointStruct& point,
      const CTMatrix& transformationMatrix);
   ~CPointInsertInstance();

public:
   // CGte virtual overrides
   virtual void draw(CCEtoODBDoc& camCadDoc,CDC& dc);

   // CQfe virtual overrides
   virtual CString getInfoString() const;

   // CObjetct2d virtual overrides
   virtual bool isPointWithinDistance(const CPoint2d& point,double distance);
   virtual int getObjectType() const;
};

//_____________________________________________________________________________
class CTextInsertInstance : public CInsertInstance
{
private:
   TextStruct* m_text;

public:
   CTextInsertInstance(CInsertNode& parentNode,TextStruct& text,
      const CTMatrix& transformationMatrix);
   ~CTextInsertInstance();

public:
   // CGte virtual overrides
   virtual void draw(CCEtoODBDoc& camCadDoc,CDC& dc);

   // CQfe virtual overrides
   virtual CString getInfoString() const;

   // CObjetct2d virtual overrides
   virtual bool isPointWithinDistance(const CPoint2d& point,double distance);
   virtual int getObjectType() const;
};

//_____________________________________________________________________________
class CInsertInstanceList : public CTypedObListContainer<CInsertInstance*>
{
private:

public:
   //CInsertInstanceList();
   //~CInsertInstanceList();
};

//_____________________________________________________________________________
class CInsertNodeList : public CTypedObListContainer<CInsertNode*>
{
private:

public:
   //CInsertNodeList();
   //~CInsertNodeList();
};

//_____________________________________________________________________________
class CInsertNode : public CObject
{
private:
   CInsertNode* m_parent;
   BlockStruct* m_block;

   CTMatrix m_TMatrix;
   CInsertNodeList m_childNodes;
   CInsertInstanceList m_childInstances;

public:
   CInsertNode();
   CInsertNode(CInsertNode& parent,const CTMatrix& matrix);
   ~CInsertNode();
   void empty();

public:
   void setTMatrix(const CTMatrix& matrix) { m_TMatrix = matrix; }
   void load(CCEtoODBDoc& camCadDoc,BlockStruct& block,int layerIndex,CGeometryTree& geometryTree);
   //void setBlock(BlockStruct* block) { m_block = block; }
};

//_____________________________________________________________________________
class CGeometryTreeExtentLimitedContainer : public CObject
{
private:
   CSize2d m_maxElementExtentSize;
   CQfeTree m_tree;

public:
   CGeometryTreeExtentLimitedContainer(const CSize2d& maxElementExtentSize);
   ~CGeometryTreeExtentLimitedContainer();
   void empty();

public:
   int getObjectCount() { return m_tree.getObjectCount(); }
   int getNodeCount()   { return m_tree.getNodeCount(); }

   void setAt(CGte* gte);
   void draw(CCEtoODBDoc& camCadDoc,CDC& dc,const CExtent& viewExtent);
};

//_____________________________________________________________________________
class CGeometryTreeLayer : public CObject
{
private:
   double m_granularity;    // minimum feature size
   //double m_maxFeatureSize; // maximum feature size
   int m_xSize;
   int m_ySize;
   CTypedObArrayContainer<CGeometryTreeExtentLimitedContainer*> m_containerArray;
   CTypedPtrList<CObList,CGeometryTreeExtentLimitedContainer*> m_list;

public:
   CGeometryTreeLayer();
   ~CGeometryTreeLayer();
   void empty();

   void setAt(CGte* gte);
   void draw(CCEtoODBDoc& camCadDoc,CDC& dc,const CExtent& viewExtent);

private:
   //CGeometryTreeExtentLimitedContainer* getContainerAt(int xIndex,int yIndex);
   void setSize(int xSize,int ySize);
};

//_____________________________________________________________________________
class CGeometryTreeLayerArray : public CTypedObArrayContainer<CGeometryTreeLayer*>
{
public:
   CGeometryTreeLayer* getAt(int layerIndex);
};

//_____________________________________________________________________________
class CGeometryTree : public CObject
{
private:
   //static const double m_granularity = .01;
   CGeometryTreeLayerArray m_layers;
   CInsertNode m_rootNode;

public:
   CGeometryTree();
   ~CGeometryTree();
   void empty();

public:
   void load(CCEtoODBDoc& camCadDoc,FileStruct& fileStruct);
   void setAt(int layerIndex,CGte* gte);
   void draw(CCEtoODBDoc& camCadDoc,CDC& dc,const CExtent& viewExtent);

private:
   CGeometryTreeLayer* getLayerAt(int layerIndex);
};

//_____________________________________________________________________________
class CGeometryTreeFilesArray : public CObject
{
private:
   CTypedObArrayContainer<CGeometryTree*> m_geometryTrees;

public:
   CGeometryTreeFilesArray();
   ~CGeometryTreeFilesArray();

public:
   void load(CCEtoODBDoc& camCadDoc);
   CGeometryTree* getAt(int fileIndex);
};

#endif  // EnableGeometryTreeImplementation
#endif  // __GeometryTree_h__

