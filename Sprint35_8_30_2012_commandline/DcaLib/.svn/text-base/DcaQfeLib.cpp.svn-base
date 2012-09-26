// $Header: /CAMCAD/DcaLib/DcaQfeLib.cpp 3     3/09/07 5:15p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaQfeLib.h"
#include "DcaWriteFormat.h"
#include "DcaExtent.h"
#include "DcaLib.h"
//#include <Float.h>

//=============================================================================

/*
History: QfeLib.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Kvanness     Date: 10/20/00   Time: 11:27p
 * Updated in $/PadsTool
 * 
 * *****************  Version 3  *****************
 * User: Kvanness     Date: 6/25/00    Time: 2:33p
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

//#include "StdAfx.h"
//#include "GraphicsTree2d.h"
//#include "LibKnvPP.h"
//#include "TraceFormat.h"
//#include <float.h>
//
//IMPLEMENT_DYNAMIC(CGraphicsTree2d,CObject);
//
//#ifdef USE_DEBUG_NEW
//#define new DEBUG_NEW
//#endif

//#define __TestTree2D__

//_____________________________________________________________________________
CObject2dList::CObject2dList(bool isContainer,int nBlockSize) : 
   CTypedPtrList<CObList,CObject2d*>(nBlockSize)
{
   m_isContainer = isContainer;
}

CObject2dList::CObject2dList(int nBlockSize) : 
   CTypedPtrList<CObList,CObject2d*>(nBlockSize)
{
   m_isContainer = false;
}

CObject2dList::~CObject2dList()
{
   empty();
}

void CObject2dList::empty()
{
   if (m_isContainer)
   {
      for (POSITION pos = GetHeadPosition();pos != NULL;)
      {
         CObject2d* object = GetNext(pos);
         delete object;
      }
   }

   RemoveAll();
}

//_____________________________________________________________________________
CTreeMetrics::CTreeMetrics()
{
   reset();
}

CTreeMetrics::~CTreeMetrics()
{
}

void CTreeMetrics::reset()
{
   m_nodeCount   = 0;
   m_objectCount = 0;
   m_depth       = 0;
   m_maxDepth    = 0;
   m_leafCnt     = 0;
   m_depthSum    = 0;

   m_depths.RemoveAll();
   m_depths.SetSize(100,100);
}

int CTreeMetrics::incNodeCount()
{
   m_nodeCount++;

   return m_nodeCount;
}

int CTreeMetrics::incObjectCount(int count)
{
   m_objectCount += count;

   return m_objectCount;
}

void CTreeMetrics::logNode(int objectCount)
{
   m_nodeCount++;
   m_objectCount += objectCount;

   int depthCnt = 1;

   if (m_depth <= m_depths.GetUpperBound())
   {
      depthCnt = m_depths.GetAt(m_depth) + 1;
   }

   m_depths.SetAtGrow(m_depth,depthCnt);
   m_depthSum += m_depth;
}

int CTreeMetrics::incDepth()
{
   m_depth++;

   if (m_depth > m_maxDepth)
   {
      m_maxDepth = m_depth;
   }

   return m_depth;
}

int CTreeMetrics::decDepth()
{
   m_depth--;

   return m_depth;
}

int CTreeMetrics::incLeaf()
{
   m_leafCnt++;

   return m_leafCnt;
}

int CTreeMetrics::logDepth()
{
   int depthCnt = 1;

   if (m_depth <= m_depths.GetUpperBound())
   {
      depthCnt = m_depths.GetAt(m_depth) + 1;
   }

   m_depths.SetAtGrow(m_depth,depthCnt);

   return depthCnt;
}

double CTreeMetrics::getAverageNodeDepth()
{
   double retval = 0.;

   if (m_nodeCount > 0)
   {
      retval = ((double)m_depthSum)/m_nodeCount;
   }

   return retval;
}

void CTreeMetrics::getOptimumDepths(int& optimumMaxDepth,double& optimumAverageDepth)
{
   optimumMaxDepth = 0;
   optimumAverageDepth = 0.;

   int depthSum = 0;
   int nodesRemaining = m_nodeCount;

   for (int inc = 1;nodesRemaining > 0;inc *= 2)
   {
      depthSum += optimumMaxDepth * ((inc < nodesRemaining) ? inc : nodesRemaining);
      nodesRemaining -= inc;

      if (nodesRemaining <= 0)
      {
         break;
      }

      optimumMaxDepth++;
   }

   if (m_nodeCount > 0)
   {
      optimumAverageDepth = ((double)depthSum) / m_nodeCount;
   }
}

void CTreeMetrics::report(CWriteFormat* writeFormat,const CString& title)
{
   writeFormat->writef(
"Tree metrics %s _________________________________________________\n\
node count = %d\n\
object count = %d\n\
max depth  = %d\n\
leaf count = %d\n\
depth sum = %d\n\
average depth = %.2f\n\
",
      (const char*)title,m_nodeCount,m_objectCount,m_maxDepth,
      m_leafCnt,m_depthSum,getAverageNodeDepth());

   int columns = 10;

   for (int depthIndex = 0;depthIndex <= m_depths.GetUpperBound() && depthIndex <= m_maxDepth;depthIndex++)
   {
      if ((depthIndex % columns) == 0)
      {
         writeFormat->writef("\n%3d: ",depthIndex);
      }

      writeFormat->writef("%5d ",m_depths.GetAt(depthIndex));
   }

   writeFormat->writef("\n\n");
}

//_____________________________________________________________________________
CGraphicsNode2d::CGraphicsNode2d(const CPoint2d& keyValue,CObject2d* object,bool isContainer) :
   m_objectList(isContainer,10)
{
   m_key = keyValue;
   m_objectList.AddTail(object);

   m_l = NULL;
   m_r = NULL;

#ifdef ImplementCGraphicsNode2dParent
   m_parent = NULL;  // only used for debugging
#endif
}

CGraphicsNode2d::~CGraphicsNode2d()
{
   empty();
}

void CGraphicsNode2d::empty()
{
   if (m_l != NULL)
   {
      m_l->empty();
   }

   if (m_r != NULL)
   {
      m_r->empty();
   }

   delete m_l;
   m_l = NULL;

   delete m_r;
   m_r = NULL;

   m_objectList.empty();
}

CGraphicsNode2d* CGraphicsNode2d::getLeft()
{
   return m_l;
}

CGraphicsNode2d* CGraphicsNode2d::getRight()
{
   return m_r;
}

#ifdef ImplementCGraphicsNode2dParent
// only used for debugging
CGraphicsNode2d* CGraphicsNode2d::getParent()
{
   return m_parent;
}
#endif

void CGraphicsNode2d::addObject(CObject2d* object)
{
   m_objectList.AddTail(object);
}

int CGraphicsNode2d::search(const CExtent& extent,CObject2dList& foundList,bool useX)
{
   int numFound = 0;
   bool searchL,searchR;

   if (useX)
   {
      searchL = m_key.x >  extent.getXmin();
      searchR = m_key.x <= extent.getXmax();
   }
   else
   {
      searchL = m_key.y >  extent.getYmin();
      searchR = m_key.y <= extent.getYmax();
   }

   if (searchL && m_l != NULL)
   {
      numFound += m_l->search(extent,foundList,!useX);
   }

   if (extent.isOnOrInside(m_key))
   {
      foundList.AddTail(&m_objectList);
      numFound += m_objectList.GetCount();
   }

   if (searchR && m_r != NULL)
   {
      numFound += m_r->search(extent,foundList,!useX);
   }

   return numFound;
}

bool CGraphicsNode2d::executeFunction(const CExtent& extent,bool useX,
      bool (*function)(const CExtent& extent,void* object,void* param),void* param)
{
   bool continueFlag = true;

   if (extent.isOnOrInside(m_key))
   {
      for (POSITION pos = m_objectList.GetHeadPosition();pos != NULL;)
      {
         CObject2d* object2d = m_objectList.GetNext(pos);

         continueFlag = (*function)(extent,object2d,param);

         if (!continueFlag)
         {
            break;
         }
      }
   }

   if (continueFlag)
   {
      bool searchL,searchR;

      if (useX)
      {
         searchL = m_key.x >  extent.getXmin();
         searchR = m_key.x <= extent.getXmax();
      }
      else
      {
         searchL = m_key.y >  extent.getYmin();
         searchR = m_key.y <= extent.getYmax();
      }

      if (searchL && m_l != NULL)
      {
         continueFlag = m_l->executeFunction(extent,!useX,function,param);
      }

      if (searchR && m_r != NULL && continueFlag)
      {
         continueFlag = m_r->executeFunction(extent,!useX,function,param);
      }
   }

   return continueFlag;
}

CObject2d* CGraphicsNode2d::findFirst(const CExtent& extent,bool useX)
{
   CObject2d* foundObject = NULL;

   if (extent.isOnOrInside(m_key))
   {
      for (POSITION pos = m_objectList.GetHeadPosition();pos != NULL;)
      {
         foundObject = m_objectList.GetNext(pos);

         break;
      }
   }

   if (foundObject == NULL)
   {
      bool searchL,searchR;

      if (useX)
      {
         searchL = m_key.x >  extent.getXmin();
         searchR = m_key.x <= extent.getXmax();
      }
      else
      {
         searchL = m_key.y >  extent.getYmin();
         searchR = m_key.y <= extent.getYmax();
      }

      if (searchL && m_l != NULL)
      {
         foundObject = m_l->findFirst(extent,!useX);
      }

      if (searchR && m_r != NULL && foundObject == NULL)
      {
         foundObject = m_r->findFirst(extent,!useX);
      }
   }

   return foundObject;
}

CObject2d* CGraphicsNode2d::findFirstEdgeToRectViolation(const CExtent& extent,int& entitiesChecked,bool useX)
{
   CObject2d* foundObject = NULL;
   entitiesChecked++;

   if (extent.isOnOrInside(m_key))
   {
      for (POSITION pos = m_objectList.GetHeadPosition();pos != NULL;)
      {
         CObject2d* object2d = m_objectList.GetNext(pos);

         if (object2d->isExtentIntersecting(extent))
         {
            foundObject = object2d;
            break;
         }
      }
   }

   if (foundObject == NULL)
   {
      bool searchL,searchR;

      if (useX)
      {
         searchL = m_key.x >  extent.getXmin();
         searchR = m_key.x <= extent.getXmax();
      }
      else
      {
         searchL = m_key.y >  extent.getYmin();
         searchR = m_key.y <= extent.getYmax();
      }

      if (searchL && m_l != NULL)
      {
         foundObject = m_l->findFirstEdgeToRectViolation(extent,entitiesChecked,!useX);
      }

      if (searchR && m_r != NULL && foundObject == NULL)
      {
         foundObject = m_r->findFirstEdgeToRectViolation(extent,entitiesChecked,!useX);
      }
   }

   return foundObject;
}

CObject2d* CGraphicsNode2d::findFirstEdgeToPointViolation(const CExtent& extent,
   const CPoint2d& point,double distance,int& entitiesChecked,bool useX)
{
   CObject2d* foundObject = NULL;
   entitiesChecked++;

   if (extent.isOnOrInside(m_key))
   {
      for (POSITION pos = m_objectList.GetHeadPosition();pos != NULL;)
      {
         CObject2d* object2d = m_objectList.GetNext(pos);

         if (object2d->isPointWithinDistance(point,distance))
         {
            foundObject = object2d;
            break;
         }
      }
   }

   if (foundObject == NULL)
   {
      bool searchL,searchR;

      if (useX)
      {
         searchL = m_key.x >  extent.getXmin();
         searchR = m_key.x <= extent.getXmax();
      }
      else
      {
         searchL = m_key.y >  extent.getYmin();
         searchR = m_key.y <= extent.getYmax();
      }

      if (searchL && m_l != NULL)
      {
         foundObject = m_l->findFirstEdgeToPointViolation(extent,point,distance,entitiesChecked,!useX);
      }

      if (searchR && m_r != NULL && foundObject == NULL)
      {
         foundObject = m_r->findFirstEdgeToPointViolation(extent,point,distance,entitiesChecked,!useX);
      }
   }

   return foundObject;
}

CObject2d* CGraphicsNode2d::findFirstViolation(const CExtent& extent,CObject2d& checkingObject,bool useX)
{
   CObject2d* foundObject = NULL;

   if (extent.isOnOrInside(m_key))
   {
      for (POSITION pos = m_objectList.GetHeadPosition();pos != NULL;)
      {
         CObject2d* object2d = m_objectList.GetNext(pos);

         if (checkingObject.isInViolation(*object2d))
         {
            foundObject = object2d;
            break;
         }
      }
   }

   if (foundObject == NULL)
   {
      bool searchL,searchR;

      if (useX)
      {
         searchL = m_key.x >  extent.getXmin();
         searchR = m_key.x <= extent.getXmax();
      }
      else
      {
         searchL = m_key.y >  extent.getYmin();
         searchR = m_key.y <= extent.getYmax();
      }

      if (searchL && m_l != NULL)
      {
         foundObject = m_l->findFirstViolation(extent,checkingObject,!useX);
      }

      if (searchR && m_r != NULL && foundObject == NULL)
      {
         foundObject = m_r->findFirstViolation(extent,checkingObject,!useX);
      }
   }

   return foundObject;
}

int CGraphicsNode2d::findAllViolations(const CExtent& extent,
   CObject2d& checkingObject,CObject2dList& foundList,bool useX)
{
   int retval = 0;

   if (extent.isOnOrInside(m_key))
   {
      for (POSITION pos = m_objectList.GetHeadPosition();pos != NULL;)
      {
         CObject2d* object2d = m_objectList.GetNext(pos);

         if (checkingObject.isInViolation(*object2d))
         {
            foundList.AddTail(object2d);
            retval++;
         }
      }
   }

   bool searchL,searchR;

   if (useX)
   {
      searchL = m_key.x >  extent.getXmin();
      searchR = m_key.x <= extent.getXmax();
   }
   else
   {
      searchL = m_key.y >  extent.getYmin();
      searchR = m_key.y <= extent.getYmax();
   }

   if (searchL && m_l != NULL)
   {
      retval += m_l->findAllViolations(extent,checkingObject,foundList,!useX);
   }

   if (searchR && m_r != NULL)
   {
      retval += m_r->findAllViolations(extent,checkingObject,foundList,!useX);
   }

   return retval;
}

void CGraphicsNode2d::getMetrics(CTreeMetrics& treeMetrics)
{
   treeMetrics.logNode(getObjectCount());
   treeMetrics.incDepth();

   if (m_l == NULL && m_r == NULL)
   {
      treeMetrics.incLeaf();
   }

   if (m_l != NULL)
   {
      m_l->getMetrics(treeMetrics);
   }

   if (m_r != NULL)
   {
      m_r->getMetrics(treeMetrics);
   }

   treeMetrics.decDepth();
}

void CGraphicsNode2d::getMetricsNonRecursive(CTreeMetrics& treeMetrics)
{
   CTypedPtrArray<CObArray,CGraphicsNode2d*> nodeArray0,nodeArray1,*currentArray,*otherArray,*tempArray;
   CGraphicsNode2d *currentNode,*leftNode,*rightNode;

   nodeArray0.SetSize(0,100);
   nodeArray1.SetSize(0,100);

   currentArray = &nodeArray0;
   otherArray   = &nodeArray1;

   currentArray->Add(this);
   treeMetrics.logNode(getObjectCount());

   while (currentArray->GetSize() > 0)
   {
      treeMetrics.incDepth();

      for (int index = 0;index < currentArray->GetSize();index++)
      {
         currentNode = currentArray->GetAt(index);
         leftNode    = currentNode->getLeft();
         rightNode   = currentNode->getRight();

         if (leftNode != NULL)
         {
            otherArray->Add(leftNode);
            treeMetrics.logNode(leftNode->getObjectCount());
         }

         if (rightNode != NULL)
         {
            otherArray->Add(rightNode);
            treeMetrics.logNode(rightNode->getObjectCount());
         }

         if (leftNode == NULL && rightNode == NULL)
         {
            treeMetrics.incLeaf();
         }
      }

      currentArray->RemoveAll();

      tempArray    = currentArray;
      currentArray = otherArray;
      otherArray   = tempArray;
   }
}

bool CGraphicsNode2d::isValid(bool useX) const
{
   static int level = 0;
   bool retval = isNodeValid(useX);

   level++;

   if (retval && m_l != NULL)
   {
      retval = m_l->isValid(!useX);
   }

   if (retval && m_r != NULL)
   {
      retval = m_r->isValid(!useX);
   }

   if (!retval)
   {
      bool invalidFlag = true;
   }

   level--;

   return retval;
}

bool CGraphicsNode2d::isNodeValid(bool useX) const
{
   bool retval = true;

   if (retval && m_l != NULL)
   {
#ifdef ImplementCGraphicsNode2dParent
      retval = m_l->getParent() == this;
#endif

      retval = ((useX ? (m_l->m_key.x <  m_key.x) : (m_l->m_key.y <  m_key.y)) && retval);
   }

   if (retval && m_r != NULL)
   {
#ifdef ImplementCGraphicsNode2dParent
      retval = m_r->getParent() == this;
#endif

      retval = ((useX ? (m_r->m_key.x >= m_key.x) : (m_r->m_key.y >= m_key.y)) && retval);
   }

   if (retval)
   {
      for (POSITION pos = m_objectList.GetHeadPosition();pos != NULL;)
      {
         CObject2d* object2d = m_objectList.GetNext(pos);

         if (object2d == NULL || !object2d->isValid())
         {
            retval = false;
            break;
         }
      }
   }

   if (!retval)
   {
      bool invalidFlag = true;
   }

   return retval;
}

void CGraphicsNode2d::trace(CWriteFormat* writeFormat,bool useX)
{
   bool isValid = isNodeValid(useX);

writeFormat->writef(
"(%p) - (%s,%s), %s\n",this,fpfmt(m_key.x),fpfmt(m_key.y),isValid ? "" : "*** Invalid ***");

writeFormat->pushHeader(".");

   if (m_l != NULL)
   {
writeFormat->writef("[Left of (%p) ]\n",this);

      m_l->trace(writeFormat,!useX);
   }
   else
   {
writeFormat->writef(
"[Left of (%p) ] - NULL\n",this);
   }

   if (m_r != NULL)
   {
writeFormat->writef("[Right of (%p) ]\n",this);

      m_r->trace(writeFormat,!useX);
   }
   else
   {
writeFormat->writef(
"[Right of (%p) ] - NULL\n",this);
   }

writeFormat->popHeader();
}

//_____________________________________________________________________________
CGraphicsTree2d::CGraphicsTree2d(bool isContainer) : m_head(CPoint2d(-DBL_MAX,-DBL_MAX),NULL,isContainer)
{
   m_isContainer = isContainer;
   m_objectCount = 0;
   m_nodeCount   = 0;
}

CGraphicsTree2d::~CGraphicsTree2d()
{
   empty();
}

void CGraphicsTree2d::empty()
{
   m_head.empty();
   m_objectCount = 0;
   m_nodeCount   = 0;
}

void CGraphicsTree2d::setAt(const CPoint2d& keyValue,CObject2d* object)
{
   CGraphicsNode2d* currentNode = &m_head;
   CGraphicsNode2d** childNode;
   bool useX = true,left;
   int depth = 0;

   while (true)
   {
      if (currentNode->getKey() == keyValue)
      {
         currentNode->addObject(object);
         break;
      }

      left = (useX ? keyValue.x < currentNode->m_key.x : keyValue.y < currentNode->m_key.y);
      childNode = (left ? &(currentNode->m_l) : &(currentNode->m_r));

      if (*childNode == NULL)
      {
         *childNode = new CGraphicsNode2d(keyValue,object,m_isContainer);
         m_nodeCount++;

#ifdef ImplementCGraphicsNode2dParent
         // for debug
         childNode->setParent(currentNode);  
#endif

         break;
      }
      else
      {
         currentNode = *childNode;
         depth++;
      }

      useX = !useX;
   }

   m_objectCount++;
}

int CGraphicsTree2d::search(const CExtent& extent,CObject2dList& foundList)
{
   int numFound = m_head.search(extent,foundList,true);

   return numFound;
}

bool CGraphicsTree2d::executeFunction(const CExtent& extent,
   bool (*function)(const CExtent& extent,void* object,void* param),void* param)
{
   bool retval = m_head.executeFunction(extent,true,function,param);

   return retval;
}

CObject2d* CGraphicsTree2d::findFirst(const CExtent& extent)
{
   CObject2d* foundObject = m_head.findFirst(extent,true);

   return foundObject;
}

CObject2d* CGraphicsTree2d::findFirstViolation(const CExtent& extent,CObject2d& object)
{
   CObject2d* foundObject = m_head.findFirstViolation(extent,object,true);

   return foundObject;
}

int CGraphicsTree2d::findAllViolations(const CExtent& extent,CObject2d& object,CObject2dList& foundList)
{
   int retval = m_head.findAllViolations(extent,object,foundList,true);

   return retval;
}

CObject2d* CGraphicsTree2d::findFirstEdgeToRectViolation(const CExtent& extent,int& entitiesChecked)
{
   CObject2d* foundObject = m_head.findFirstEdgeToRectViolation(extent,entitiesChecked,true);

   return foundObject;
}

CObject2d* CGraphicsTree2d::findFirstEdgeToPointViolation(const CExtent& extent,
   const CPoint2d& point,double distance,int& entitiesChecked)
{
   CObject2d* foundObject = m_head.findFirstEdgeToPointViolation(extent,point,distance,entitiesChecked,true);

   return foundObject;
}

void CGraphicsTree2d::getMetrics(CTreeMetrics& treeMetrics)
{
   treeMetrics.reset();

   m_head.getMetrics(treeMetrics);
}

void CGraphicsTree2d::getMetricsNonRecursive(CTreeMetrics& treeMetrics)
{
   treeMetrics.reset();

   m_head.getMetricsNonRecursive(treeMetrics);
}

void CGraphicsTree2d::reportMetrics(CWriteFormat* writeFormat,const char* title)
{
   CTreeMetrics treeMetrics;

   m_head.getMetrics(treeMetrics);

   treeMetrics.report(writeFormat,title);
}

void CGraphicsTree2d::trace(CWriteFormat* writeFormat)
{
   m_head.trace(writeFormat,true);
}

class CTestObject : public CObject2d
{
public:
   CPoint2d m_coord;
   CString m_name;

   CTestObject(const char* name,double x,double y) { m_name = name;  m_coord.x = x;  m_coord.y = y;  }
   virtual bool isExtentIntersecting(const CExtent& extent) { return false; }
   virtual bool isPointWithinDistance(const CPoint2d& point,double distance) { return false; }
   virtual bool isInViolation(CObject2d& otherObject) { return false; }
   virtual int getObjectType() const { return 100; }
};

class CTestObjectList : public CTypedPtrList<CObject2dList,CTestObject*>
{
public:
   CTestObjectList(int nBlockSize = 10) : CTypedPtrList<CObject2dList,CTestObject*>(nBlockSize) { }
};

bool CGraphicsTree2d::test()
{
   CTestObject a("a", 3, 9);
   CTestObject b("b",11, 1);
   CTestObject c("c", 6, 8);
   CTestObject d("d", 4, 3);
   CTestObject e("e", 5,15);
   CTestObject f("f", 8,11);
   CTestObject g("g", 1, 6);
   CTestObject h("h", 7, 4);
   CTestObject i("i", 9, 7);
   CTestObject j("j",14,15);
   CTestObject k("k",10,13);
   CTestObject l("l",16,14);
   CTestObject m("m",15, 2);
   CTestObject n("n",13,16);
   CTestObject o("o", 2,12);
   CTestObject p("p",12,10);

   CGraphicsTree2d testTree(false);

   testTree.setAt(a.m_coord,&a);
   testTree.setAt(b.m_coord,&b);
   testTree.setAt(c.m_coord,&c);
   testTree.setAt(d.m_coord,&d);
   testTree.setAt(e.m_coord,&e);
   testTree.setAt(f.m_coord,&f);
   testTree.setAt(g.m_coord,&g);
   testTree.setAt(h.m_coord,&h);
   testTree.setAt(i.m_coord,&i);
   testTree.setAt(j.m_coord,&j);
   testTree.setAt(k.m_coord,&k);
   testTree.setAt(l.m_coord,&l);
   testTree.setAt(m.m_coord,&m);
   testTree.setAt(n.m_coord,&n);
   testTree.setAt(o.m_coord,&o);

   CExtent extent(5,10,9,16);
   CTestObjectList foundList;

   testTree.search(extent,foundList);

   ASSERT(foundList.Find(&e) != NULL);
   ASSERT(foundList.Find(&f) != NULL);
   ASSERT(foundList.GetCount() == 2);

   CTraceFormat traceFormat;
   testTree.trace(&traceFormat);

   CTreeMetrics treeMetrics;
   testTree.getMetrics(treeMetrics);
   treeMetrics.report(&traceFormat,"CGraphicsTree2d::test()");

   return true;
}

bool CGraphicsTree2d::isValid() const
{
   bool retval = true;

   if (retval && m_head.m_l != NULL)
   {
      retval = m_head.m_l->isValid(false);
   }

   if (retval && m_head.m_r != NULL)
   {
      retval = m_head.m_r->isValid(false);
   }

   return retval;
}

//=============================================================================

/*
History: QfeLib.cpp $
 * 
 * *****************  Version 14  *****************
 * User: Kurt Van Ness Date: 4/22/04    Time: 8:28p
 * Updated in $/CAMCAD/4.4/read_wrt
 * 
 * *****************  Version 12  *****************
 * User: Kurt Van Ness Date: 11/17/03   Time: 8:54a
 * Updated in $/CAMCAD/4.3/read_wrt
 * 
 * *****************  Version 11  *****************
 * User: Kurt Van Ness Date: 11/13/03   Time: 9:34a
 * Updated in $/CAMCAD/4.3/read_wrt
 * 
 * *****************  Version 10  *****************
 * User: Kurt Van Ness Date: 8/19/03    Time: 4:23p
 * Updated in $/CAMCAD/4.3/read_wrt
 * 
 * *****************  Version 9  *****************
 * User: Kurt Van Ness Date: 8/12/03    Time: 7:25p
 * Updated in $/CAMCAD/4.3/read_wrt
 * 
 * *****************  Version 5  *****************
 * User: Kvanness     Date: 10/20/00   Time: 11:27p
 * Updated in $/PadsTool
 * 
 * *****************  Version 4  *****************
 * User: Kvanness     Date: 6/25/00    Time: 2:14p
 * Updated in $/PadsTool
 * Added dual count reporting, objectCount and nodeCount, to
 * CQfeGraduatedExtentLimitedContainer::traceDump()
 * Removed code in CQfeEntitied::load() to call CQfeEntitied::traceDump()
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

//#include "StdAfx.h"
//#include "Qfe.h"
//#include "LibKnvPP.h"
//#include "TraceFormat.h"
//#include "TestPoint.h"
//#include "PadsDbFile.h"
//#include "Sections.h"
//#include "PinPad.h"
//#include "VariableTable.h"
//#include "PadsTool.h"
//#include "LibGraphics.h"
////#include <float.h>
//
//IMPLEMENT_DYNAMIC(CObject2d,CObject);
//IMPLEMENT_DYNAMIC(CQfe,CObject2d);
//IMPLEMENT_DYNAMIC(CQfePinPad,CQfe);
//IMPLEMENT_DYNAMIC(CQfeTestPoint,CQfe);
//IMPLEMENT_DYNAMIC(CQfeTrack,CQfe);
//IMPLEMENT_DYNAMIC(CQfeVia,CQfe);
//
//#ifdef USE_DEBUG_NEW
//#define new DEBUG_NEW
//#endif

#define MinimumGranularity 5
#define MinimumMaxFeatureSize 5

//extern CVariableTable& getVariableTable();

//_____________________________________________________________________________
CLayerSpec::CLayerSpec(int minLayer,int maxLayer,int topLayer,int bottomLayer)
{
   m_minLayer = minLayer;
   m_maxLayer = maxLayer;
   m_topLayer = topLayer;
   m_botLayer = bottomLayer;

   m_layerArray.SetSize(m_maxLayer + 1,1);
}

CLayerSpec::~CLayerSpec()
{
}

void CLayerSpec::empty()
{
   for (int layer = m_minLayer;layer <= m_maxLayer;layer++)
   {
      m_layerArray.SetAt(layer,0);
   }

   m_layerList.RemoveAll();
}

void CLayerSpec::set(int layer,bool select)
{
   if (layer >= m_minLayer && layer <= m_maxLayer)
   {
      m_layerArray.SetAt(layer,select);
   }

   m_layerList.RemoveAll();
}

void CLayerSpec::set(layerSpecTag layerSpec,bool select)
{
   int layer;

   switch(layerSpec)
   {
   case lsTop:      set(m_topLayer,select);  break;
   case lsBottom:   set(m_botLayer,select);  break;
   case lsExterior: set(m_topLayer,select);  set(m_botLayer,select);  break;
   case lsInterior: 
      for (layer = m_topLayer + 1;layer < m_botLayer;layer++)
      {
         set(layer,select);
      }

      break;
   case lsAll:
      for (layer = m_minLayer;layer <= m_maxLayer;layer++)
      {
         set(layer,select);
      }

      break;
   }
}

bool CLayerSpec::isSet(int layer)
{
   bool retval = false;

   if (layer >= m_minLayer && layer <= m_maxLayer)
   {
      retval = (m_layerArray.GetAt(layer) != 0);
   }

   return retval;
}

bool CLayerSpec::isSet(layerSpecTag layerSpec)
{
   bool retval = false;
   int layer;

   switch(layerSpec)
   {
   case lsTop:      retval = isSet(m_topLayer);  break;
   case lsBottom:   retval = isSet(m_botLayer);  break;
   case lsExterior: retval = (isSet(m_topLayer) && isSet(m_botLayer));  break;
   case lsInterior: 
      retval = true;   

      for (layer = m_topLayer + 1;layer < m_botLayer && retval;layer++)
      {
         retval = isSet(layer);
      }

      break;
   case lsAll:
      retval = true;   

      for (layer = m_minLayer;layer <= m_maxLayer && retval;layer++)
      {
         retval = isSet(layer);
      }

      break;
   }

   return retval;
}

POSITION CLayerSpec::GetHeadPosition() const
{
   if (m_layerList.IsEmpty())
   {
      for (int layer = m_minLayer;layer <= m_maxLayer;layer++)
      {
         if (m_layerArray.GetAt(layer) != 0)
         {
            m_layerList.AddTail(layer);
         }
      }
   }

   POSITION pos = m_layerList.GetHeadPosition();

   return pos;
}

int CLayerSpec::GetNext(POSITION& pos) const
{
   int layer = m_layerList.GetNext(pos);

   return layer;
}

//_____________________________________________________________________________
CQfe::CQfe()
{
}

CQfe::~CQfe()
{
}

bool CQfe::isValid() const
{
   return true;
}

void CQfe::assertValid() const
{
   if (! isValid())
   {
      AfxDebugBreak();
      isValid();
   }
}

//_____________________________________________________________________________
CQfeList::CQfeList(int nBlockSize) : CTypedPtrList<CObject2dList,CQfe*>(nBlockSize)
{
}

bool CQfeList::isValid() const
{
   bool retval = true;

   for (POSITION pos = GetHeadPosition();pos != NULL && retval;)
   {
      CQfe* element = GetNext(pos);

      if (element != NULL)
      {
         retval = element->isValid();
      }
   }

   return retval;
}

void CQfeList::assertValid() const
{
   if (! isValid())
   {
      AfxDebugBreak();
      isValid();
   }
}

//_____________________________________________________________________________
CQfeTree::CQfeTree(bool isContainer) : CGraphicsTree2d(isContainer)
{
}

CQfeTree::~CQfeTree()
{
}

//_____________________________________________________________________________
bool CQfeSearchableContainer::executeFunction(const CExtent& extent,
   bool (*function)(const CExtent& extent,void* object,void* param),void* param)
{
   return false;
}

void CQfeSearchableContainer::assertValid() const
{
   if (! isValid())
   {
      AfxDebugBreak();
      isValid();
   }
}

//_____________________________________________________________________________
CQfeExtentLimitedContainer::CQfeExtentLimitedContainer(const CSize2d& maxElementExtentSize,bool updateExtentSize,bool isContainer) :
   m_tree(isContainer)
{
   m_maxElementExtentSize = maxElementExtentSize;
   m_updateExtentSize      = updateExtentSize;
}

CQfeExtentLimitedContainer::~CQfeExtentLimitedContainer()
{
   deleteAll();
}


void CQfeExtentLimitedContainer::printMetricsReport(CWriteFormat& writeFormat)
{
   CTreeMetrics treeMetrics;
   m_tree.getMetricsNonRecursive(treeMetrics);
   treeMetrics.report(&writeFormat,"CQfeExtentLimitedContainer::printMetricsReport()");
}

void CQfeExtentLimitedContainer::getTreeMetrics(CTreeMetrics& treeMetrics)
{
   m_tree.getMetricsNonRecursive(treeMetrics);
}

void CQfeExtentLimitedContainer::deleteAll()
{
   m_tree.empty();
}

int CQfeExtentLimitedContainer::search(const CExtent& extent,CQfeList& foundList)
{
   CExtent searchExtent(extent);
   searchExtent.expand(m_maxElementExtentSize);

   int retval = m_tree.search(searchExtent,foundList);

   return retval;
}

bool CQfeExtentLimitedContainer::executeFunction(const CExtent& extent,
   bool (*function)(const CExtent& extent,void* object,void* param),void* param)
{
   CExtent searchExtent(extent);
   searchExtent.expand(m_maxElementExtentSize);

   bool retval = m_tree.executeFunction(searchExtent,function,param);

   return retval;
}

CQfe* CQfeExtentLimitedContainer::findFirst(const CExtent& extent)
{
   CExtent searchExtent(extent);
   searchExtent.expand(m_maxElementExtentSize);

   CQfe* foundQfe = (CQfe*)(m_tree.findFirst(searchExtent));

   return foundQfe;
}

CQfe* CQfeExtentLimitedContainer::findFirstViolation(const CExtent& extent,CQfe& qfe)
{
   CExtent searchExtent(extent);
   searchExtent.expand(m_maxElementExtentSize);

   CQfe* foundQfe = (CQfe*)(m_tree.findFirstViolation(searchExtent,qfe));

   return foundQfe;
}

int CQfeExtentLimitedContainer::findAllViolations(const CExtent& extent,CQfe& qfe,CQfeList& foundList)
{
   CExtent searchExtent(extent);
   searchExtent.expand(m_maxElementExtentSize);

   int retval = m_tree.findAllViolations(searchExtent,qfe,foundList);

   return retval;
}

CQfe* CQfeExtentLimitedContainer::findFirstEdgeToRectViolation(const CExtent& extent,int& entitiesChecked)
{
   CExtent searchExtent(extent);
   searchExtent.expand(m_maxElementExtentSize);

   CQfe* foundQfe = (CQfe*)(m_tree.findFirstEdgeToRectViolation(searchExtent,entitiesChecked));

   return foundQfe;
}

CQfe* CQfeExtentLimitedContainer::findFirstEdgeToPointViolation(const CExtent& extent,
   const CPoint2d& point,double distance,int& entitiesChecked)
{
   CExtent searchExtent(extent);
   searchExtent.expand(m_maxElementExtentSize);

   CQfe* foundQfe = (CQfe*)(m_tree.findFirstEdgeToPointViolation(searchExtent,point,distance,entitiesChecked));

   return foundQfe;
}

void CQfeExtentLimitedContainer::setAt(CQfe* qfe)
{
   if (m_updateExtentSize)
   {
      CSize2d qfeExtentSize = qfe->getExtent().getSize();

      if (qfeExtentSize.cx > m_maxElementExtentSize.cx)
      {
         m_maxElementExtentSize.cx = qfeExtentSize.cx;
      }

      if (qfeExtentSize.cy > m_maxElementExtentSize.cy)
      {
         m_maxElementExtentSize.cy = qfeExtentSize.cy;
      }
   }

   m_tree.setAt(qfe->getOrigin(),qfe);
}

bool CQfeExtentLimitedContainer::isValid() const
{
   bool retval = m_tree.isValid();

   return retval;
}

void CQfeExtentLimitedContainer::assertValid() const
{
   if (! isValid())
   {
      AfxDebugBreak();
      isValid();
   }
}

//_____________________________________________________________________________
bool CQfeExtentLimitedContainerArray::isValid() const
{
   bool retval = true;

   for (int index = 0;index < GetSize() && retval;index++)
   {
      CQfeExtentLimitedContainer* element = GetAt(index);

      if (element != NULL)
      {
         retval = element->isValid();
      }
   }

   return retval;
}

void CQfeExtentLimitedContainerArray::assertValid() const
{
   if (! isValid())
   {
      AfxDebugBreak();
      isValid();
   }
}

//_____________________________________________________________________________
bool CQfeExtentLimitedContainerList::isValid() const
{
   bool retval = true;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CQfeExtentLimitedContainer* element = GetNext(pos);

      if (element != NULL)
      {
         retval = element->isValid();
      }
   }

   return retval;
}

void CQfeExtentLimitedContainerList::assertValid() const
{
   if (! isValid())
   {
      AfxDebugBreak();
      isValid();
   }
}

//_____________________________________________________________________________
CQfeGraduatedExtentLimitedContainer::CQfeGraduatedExtentLimitedContainer(double granularity,double maxFeatureSize)
{
   // array Index:   0    1    2     3     4     5      6
   //               ==0  <=g  <=2g  <=4g  <=8g  <=16g  >16g

   m_granularity = granularity;
   if (m_granularity < 0.) m_granularity = 0.;

   m_maxFeatureSize = maxFeatureSize;
   if (m_maxFeatureSize < m_granularity) m_maxFeatureSize = m_granularity;

   double granularitySize = m_granularity;

   for (m_arrayIndexSize = 2;;m_arrayIndexSize++)
   {
      if (granularitySize >= m_maxFeatureSize || granularitySize == 0.)
      {
         break;
      }

      granularitySize *= 2.;
   }

   int size = m_arrayIndexSize * m_arrayIndexSize;
   m_containerArray.SetSize(size,10);
}

CQfeGraduatedExtentLimitedContainer::~CQfeGraduatedExtentLimitedContainer()
{
   deleteAll();
}

void CQfeGraduatedExtentLimitedContainer::deleteAll()
{
   CQfeExtentLimitedContainer* qfeExtentLimitedContainer;

   for (POSITION pos = m_containerList.GetHeadPosition();pos != NULL;)
   {
      qfeExtentLimitedContainer = m_containerList.GetNext(pos);
      delete qfeExtentLimitedContainer;
   }

   m_containerList.RemoveAll();

   int linearIndexSize = m_arrayIndexSize * m_arrayIndexSize;

   for (int ind = 0;ind < linearIndexSize;ind++)
   {
      m_containerArray.SetAt(ind,NULL);
   }
}

int CQfeGraduatedExtentLimitedContainer::search(const CExtent& extent,CQfeList& foundList)
{
   int retval = 0;
   POSITION pos;
   CQfeExtentLimitedContainer* extentLimitedContainer;

   for (pos = m_containerList.GetHeadPosition();pos != NULL;)
   {
      extentLimitedContainer = m_containerList.GetNext(pos);

      retval += extentLimitedContainer->search(extent,foundList);
   }

   return retval;
}

bool CQfeGraduatedExtentLimitedContainer::executeFunction(const CExtent& extent,
   bool (*function)(const CExtent& extent,void* object,void* param),void* param)
{
   bool retval = true;
   POSITION pos;
   CQfeExtentLimitedContainer* extentLimitedContainer;

   for (pos = m_containerList.GetHeadPosition();pos != NULL && retval;)
   {
      extentLimitedContainer = m_containerList.GetNext(pos);

      retval = extentLimitedContainer->executeFunction(extent,function,param);
   }

   return retval;
}

CQfe* CQfeGraduatedExtentLimitedContainer::findFirst(const CExtent& extent)
{
   CQfe* foundQfe = NULL;
   POSITION pos;
   CQfeExtentLimitedContainer* extentLimitedContainer;

   for (pos = m_containerList.GetHeadPosition();pos != NULL;)
   {
      extentLimitedContainer = m_containerList.GetNext(pos);

      foundQfe = extentLimitedContainer->findFirst(extent);

      if (foundQfe != NULL)
      {
         break;
      }
   }

   return foundQfe;
}

CQfe* CQfeGraduatedExtentLimitedContainer::findFirstViolation(const CExtent& extent,CQfe& qfe)
{
   CQfe* foundQfe = NULL;
   POSITION pos;
   CQfeExtentLimitedContainer* extentLimitedContainer;

   for (pos = m_containerList.GetHeadPosition();pos != NULL;)
   {
      extentLimitedContainer = m_containerList.GetNext(pos);

      foundQfe = extentLimitedContainer->findFirstViolation(extent,qfe);

      if (foundQfe != NULL)
      {
         break;
      }
   }

   return foundQfe;
}

int CQfeGraduatedExtentLimitedContainer::findAllViolations(const CExtent& extent,CQfe& qfe,CQfeList& foundList)
{
   int retval = 0;
   POSITION pos;
   CQfeExtentLimitedContainer* extentLimitedContainer;

   for (pos = m_containerList.GetHeadPosition();pos != NULL;)
   {
      extentLimitedContainer = m_containerList.GetNext(pos);

      retval += extentLimitedContainer->findAllViolations(extent,qfe,foundList);
   }

   return retval;
}

CQfe* CQfeGraduatedExtentLimitedContainer::findFirstEdgeToRectViolation(const CExtent& extent,int& entitiesChecked)
{
   CQfe* foundQfe = NULL;
   POSITION pos;
   CQfeExtentLimitedContainer* extentLimitedContainer;

   for (pos = m_containerList.GetHeadPosition();pos != NULL;)
   {
      extentLimitedContainer = m_containerList.GetNext(pos);

      foundQfe = extentLimitedContainer->findFirstEdgeToRectViolation(extent,entitiesChecked);

      if (foundQfe != NULL)
      {
         break;
      }
   }

   return foundQfe;
}

CQfe* CQfeGraduatedExtentLimitedContainer::findFirstEdgeToPointViolation(const CExtent& extent,
   const CPoint2d& point,double distance,int& entitiesChecked)
{
   CQfe* foundQfe = NULL;
   POSITION pos;
   CQfeExtentLimitedContainer* extentLimitedContainer;

   for (pos = m_containerList.GetHeadPosition();pos != NULL;)
   {
      extentLimitedContainer = m_containerList.GetNext(pos);

      foundQfe = extentLimitedContainer->findFirstEdgeToPointViolation(extent,point,distance,entitiesChecked);

      if (foundQfe != NULL)
      {
         break;
      }
   }

   return foundQfe;
}

void CQfeGraduatedExtentLimitedContainer::setAt(CQfe* qfe)
{
   // array Index:   0    1    2     3     4     5      6
   //               ==0  <=g  <=2g  <=4g  <=8g  <=16g  >16g

   CQfeExtentLimitedContainer* extentLimitedContainer = NULL;
   CExtent qfeExtent(qfe->getExtent());
   double xQfeDimension = qfeExtent.getXsize();
   double yQfeDimension = qfeExtent.getYsize();

   int xInd=0,yInd=0;
   double xExtentDimension=0.,yExtentDimension=0.;
   int arrayIndexUpperBound = m_arrayIndexSize - 1;

   if (xQfeDimension > 0.)
   {
      xExtentDimension = m_granularity;

      for (xInd = 1;xInd < arrayIndexUpperBound;xInd++)
      {
         if (xQfeDimension <= xExtentDimension)
         {
            break;
         }

         xExtentDimension *= 2.;
      }
   }

   if (yQfeDimension > 0.)
   {
      yExtentDimension = m_granularity;

      for (yInd = 1;yInd < arrayIndexUpperBound;yInd++)
      {
         if (yQfeDimension <= yExtentDimension)
         {
            break;
         }

         yExtentDimension *= 2.;
      }
   }

   int arrayIndex = m_arrayIndexSize*yInd + xInd;
   bool lastRowColIndex = (xInd == (m_arrayIndexSize - 1) || yInd == (m_arrayIndexSize - 1));

   extentLimitedContainer = m_containerArray.GetAt(arrayIndex);

   if (extentLimitedContainer == NULL)
   {
      CSize2d maxElementExtentSize(xExtentDimension,yExtentDimension);

      extentLimitedContainer = new CQfeExtentLimitedContainer(maxElementExtentSize,lastRowColIndex);
      m_containerArray.SetAt(arrayIndex,extentLimitedContainer);
      m_containerList.AddTail(extentLimitedContainer);
   }

   extentLimitedContainer->setAt(qfe);
}

void CQfeGraduatedExtentLimitedContainer::traceDump(CWriteFormat* writeFormat,const char* header)
{
   bool reportDepths = true;

   writeFormat->writef(
      "CQfeGraduatedExtentLimitedContainer - maxFeatureSize=%s\n",fpfmt(m_maxFeatureSize));
   writeFormat->pushHeader(".  ");

   int row,col,index;
   double granularity,granularitySize;
   CQfeExtentLimitedContainer* extentLimitedContainer;

   writeFormat->writef("\n%14s ","");

   for (col = 0;col < m_arrayIndexSize;col++)
   {
      writeFormat->writef("%10d ",col);
   }

   writeFormat->writef("\n%14s ","");
   granularity = 0.;
   granularitySize = m_granularity;

   for (col = 0;col < m_arrayIndexSize;col++)
   {
      writeFormat->writef("%10s ",fpfmt(granularity));
      granularity = granularitySize;
      granularitySize *= 2.;
   }

   writeFormat->writef("\n");
   granularity = 0.;
   granularitySize = m_granularity;

   for (row = 0;row < m_arrayIndexSize;row++)
   {
      writeFormat->writef("\n%3d %10s      ",row,fpfmt(granularity));

      for (col = 0;col < m_arrayIndexSize;col++)
      {
         CString count(".     ");
         index = col + m_arrayIndexSize * row;
         extentLimitedContainer = m_containerArray.GetAt(index);

         if (extentLimitedContainer != NULL)
         {
            count.Format("%4d/%-5d",
               extentLimitedContainer->getNodeCount(),
               extentLimitedContainer->getObjectCount());
         }

         writeFormat->writef("%10s ",(const char*)count);
      }

      if (reportDepths)
      {
         writeFormat->writef("\n%3s %10s      ","","");

         for (col = 0;col < m_arrayIndexSize;col++)
         {
            CString count("      ");
            index = col + m_arrayIndexSize * row;
            extentLimitedContainer = m_containerArray.GetAt(index);

            if (extentLimitedContainer != NULL)
            {
               CTreeMetrics treeMetrics;

               extentLimitedContainer->getTreeMetrics(treeMetrics);

               count.Format("%4d/%-5.1f",
                  treeMetrics.getMaxDepth(),
                  treeMetrics.getAverageNodeDepth());
            }

            writeFormat->writef("%10.10s ",(const char*)count);
         }

         writeFormat->writef("\n%3s %10s      ","","");

         for (col = 0;col < m_arrayIndexSize;col++)
         {
            CString count("      ");
            index = col + m_arrayIndexSize * row;
            extentLimitedContainer = m_containerArray.GetAt(index);

            if (extentLimitedContainer != NULL)
            {
               CTreeMetrics treeMetrics;
               int optimumMaxNodeDepth;
               double optimumAverageNodeDepth;

               extentLimitedContainer->getTreeMetrics(treeMetrics);
               treeMetrics.getOptimumDepths(optimumMaxNodeDepth,optimumAverageNodeDepth);

               count.Format("%4d/%-5.1f",optimumMaxNodeDepth,optimumAverageNodeDepth);
            }

            writeFormat->writef("%10.10s ",(const char*)count);
         }

         writeFormat->writef("\n");
      }

      granularity = granularitySize;
      granularitySize *= 2.;
   }

   writeFormat->writef("\n\n");
   writeFormat->popHeader();
}

bool CQfeGraduatedExtentLimitedContainer::isValid() const
{
   bool retval = true;
   CQfeExtentLimitedContainer* qfeExtentLimitedContainer;

   for (POSITION pos = m_containerList.GetHeadPosition();retval && pos != NULL;)
   {
      qfeExtentLimitedContainer = m_containerList.GetNext(pos);

      if (qfeExtentLimitedContainer != NULL)
      {
         retval = qfeExtentLimitedContainer->isValid();
      }
   }

   return retval;
}

void CQfeGraduatedExtentLimitedContainer::assertValid() const
{
   if (! isValid())
   {
      AfxDebugBreak();
      isValid();
   }
}

void CQfeGraduatedExtentLimitedContainer::printMetricsReport(CWriteFormat& writeFormat)
{
   CQfeExtentLimitedContainer* qfeExtentLimitedContainer;

   for (POSITION pos = m_containerList.GetHeadPosition();pos != NULL;)
   {
      POSITION thisPos = pos;
      qfeExtentLimitedContainer = m_containerList.GetNext(pos);

      if (qfeExtentLimitedContainer != NULL)
      {
         writeFormat.writef("\nCQfeGraduatedExtentLimitedContainer::printMetricsReport(), pos = 0x%0x\n",thisPos);
         qfeExtentLimitedContainer->printMetricsReport(writeFormat);
      }
   }
}

//_____________________________________________________________________________
bool CQfeGraduatedExtentLimitedContainerArray::isValid() const
{
   bool retval = true;

   for (int index = 0;index < GetSize() && retval;index++)
   {
      CQfeGraduatedExtentLimitedContainer* element = GetAt(index);

      if (element != NULL)
      {
         retval = element->isValid();
      }
   }

   return retval;
}

void CQfeGraduatedExtentLimitedContainerArray::assertValid() const
{
   if (! isValid())
   {
      AfxDebugBreak();
      isValid();
   }
}

//_____________________________________________________________________________
CQfeLayeredContainer::CQfeLayeredContainer(int maxLayer,double granularity,double maxFeatureSize)
{
   ASSERT(maxLayer > 0);

   m_maxLayer = maxLayer;

   m_qfeLayers.SetSize(m_maxLayer + 1,10);

   m_granularity = granularity;
   if (m_granularity < MinimumGranularity) m_granularity = MinimumGranularity;

   m_maxFeatureSize = maxFeatureSize;
   if (m_maxFeatureSize < MinimumMaxFeatureSize) m_maxFeatureSize = MinimumMaxFeatureSize;
}

CQfeLayeredContainer::~CQfeLayeredContainer()
{
   deleteAll();
}

void CQfeLayeredContainer::deleteAll()
{
   for (int layer = 0;layer <= m_maxLayer;layer++)
   {
      delete m_qfeLayers.GetAt(layer);
      m_qfeLayers.SetAt(layer,NULL);
   }
}

int CQfeLayeredContainer::search(const CLayerSpec& layerSpec,const CExtent& extent,CQfeList& foundList)
{
   int retval = 0;
   POSITION layerSpecPos;
   int layerIndex;
   CQfeGraduatedExtentLimitedContainer* graduatedExtentLimitedContainer;

   for (layerSpecPos = layerSpec.GetHeadPosition();layerSpecPos != NULL;)
   {
      layerIndex = layerSpec.GetNext(layerSpecPos);

      if (layerIndex <= m_maxLayer)
      {
         graduatedExtentLimitedContainer = m_qfeLayers.GetAt(layerIndex);

         if (graduatedExtentLimitedContainer != NULL)
         {
            retval += graduatedExtentLimitedContainer->search(extent,foundList);
         }
      }
   }

   return retval;
}

bool CQfeLayeredContainer::executeFunction(const CLayerSpec& layerSpec,const CExtent& extent,
   bool (*function)(const CExtent& extent,void* object,void* param),void* param)
{
   bool retval = true;
   POSITION layerSpecPos;
   int layerIndex;
   CQfeGraduatedExtentLimitedContainer* graduatedExtentLimitedContainer;

   for (layerSpecPos = layerSpec.GetHeadPosition();layerSpecPos != NULL && retval;)
   {
      layerIndex = layerSpec.GetNext(layerSpecPos);

      if (layerIndex <= m_maxLayer)
      {
         graduatedExtentLimitedContainer = m_qfeLayers.GetAt(layerIndex);

         if (graduatedExtentLimitedContainer != NULL)
         {
            retval = graduatedExtentLimitedContainer->executeFunction(extent,function,param);
         }
      }
   }

   return retval;
}

CQfe* CQfeLayeredContainer::findFirst(const CLayerSpec& layerSpec,const CExtent& extent)
{
   CQfe* foundQfe = NULL;
   POSITION layerSpecPos;
   int layerIndex;
   CQfeGraduatedExtentLimitedContainer* graduatedExtentLimitedContainer;

   for (layerSpecPos = layerSpec.GetHeadPosition();layerSpecPos != NULL;)
   {
      layerIndex = layerSpec.GetNext(layerSpecPos);

      if (layerIndex <= m_maxLayer)
      {
         graduatedExtentLimitedContainer = m_qfeLayers.GetAt(layerIndex);

         if (graduatedExtentLimitedContainer != NULL)
         {
            foundQfe = graduatedExtentLimitedContainer->findFirst(extent);

            if (foundQfe != NULL)
            {
               break;
            }
         }
      }
   }

   return foundQfe;
}

CQfe* CQfeLayeredContainer::findFirstViolation(const CLayerSpec& layerSpec,const CExtent& extent,CQfe& qfe)
{
   CQfe* foundQfe = NULL;
   POSITION layerSpecPos;
   int layerIndex;
   CQfeGraduatedExtentLimitedContainer* graduatedExtentLimitedContainer;

   for (layerSpecPos = layerSpec.GetHeadPosition();layerSpecPos != NULL;)
   {
      layerIndex = layerSpec.GetNext(layerSpecPos);

      if (layerIndex <= m_maxLayer)
      {
         graduatedExtentLimitedContainer = m_qfeLayers.GetAt(layerIndex);

         if (graduatedExtentLimitedContainer != NULL)
         {
            foundQfe = graduatedExtentLimitedContainer->findFirstViolation(extent,qfe);

            if (foundQfe != NULL)
            {
               break;
            }
         }
      }
   }

   return foundQfe;
}

int CQfeLayeredContainer::findAllViolations(const CLayerSpec& layerSpec,
   const CExtent& extent,CQfe& qfe,CQfeList& foundList)
{
   int retval = 0;
   POSITION layerSpecPos;
   int layerIndex;
   CQfeGraduatedExtentLimitedContainer* graduatedExtentLimitedContainer;

   for (layerSpecPos = layerSpec.GetHeadPosition();layerSpecPos != NULL;)
   {
      layerIndex = layerSpec.GetNext(layerSpecPos);

      if (layerIndex <= m_maxLayer)
      {
         graduatedExtentLimitedContainer = m_qfeLayers.GetAt(layerIndex);

         if (graduatedExtentLimitedContainer != NULL)
         {
            retval += graduatedExtentLimitedContainer->findAllViolations(extent,qfe,foundList);
         }
      }
   }

   return retval;
}

CQfe* CQfeLayeredContainer::findFirstEdgeToRectViolation(const CLayerSpec& layerSpec,
                                                         const CExtent& extent,int& entitiesChecked)
{
   CQfe* foundQfe = NULL;
   POSITION layerSpecPos;
   int layerIndex;
   CQfeGraduatedExtentLimitedContainer* graduatedExtentLimitedContainer;

   for (layerSpecPos = layerSpec.GetHeadPosition();layerSpecPos != NULL;)
   {
      layerIndex = layerSpec.GetNext(layerSpecPos);

      if (layerIndex <= m_maxLayer)
      {
         graduatedExtentLimitedContainer = m_qfeLayers.GetAt(layerIndex);

         if (graduatedExtentLimitedContainer != NULL)
         {
            foundQfe = graduatedExtentLimitedContainer->findFirstEdgeToRectViolation(extent,entitiesChecked);

            if (foundQfe != NULL)
            {
               break;
            }
         }
      }
   }

   return foundQfe;
}

CQfe* CQfeLayeredContainer::findFirstEdgeToPointViolation(const CLayerSpec& layerSpec,const CExtent& extent,
   const CPoint2d& point,double distance,int& entitiesChecked)
{
   CQfe* foundQfe = NULL;
   POSITION layerSpecPos;
   int layerIndex;
   CQfeGraduatedExtentLimitedContainer* graduatedExtentLimitedContainer;

   for (layerSpecPos = layerSpec.GetHeadPosition();layerSpecPos != NULL;)
   {
      layerIndex = layerSpec.GetNext(layerSpecPos);

      if (layerIndex <= m_maxLayer)
      {
         graduatedExtentLimitedContainer = m_qfeLayers.GetAt(layerIndex);

         if (graduatedExtentLimitedContainer != NULL)
         {
            foundQfe = graduatedExtentLimitedContainer->findFirstEdgeToPointViolation(extent,point,distance,entitiesChecked);

            if (foundQfe != NULL)
            {
               break;
            }
         }
      }
   }

   return foundQfe;
}

void CQfeLayeredContainer::setAt(int layer,CQfe* qfe)
{
   ASSERT(layer > 0);
   ASSERT(layer <= m_maxLayer);

   CQfeGraduatedExtentLimitedContainer* graduatedExtentLimitedContainer = m_qfeLayers.GetAt(layer);

   if (graduatedExtentLimitedContainer == NULL)
   {
      graduatedExtentLimitedContainer = new CQfeGraduatedExtentLimitedContainer(m_granularity,m_maxFeatureSize);
      m_qfeLayers.SetAt(layer,graduatedExtentLimitedContainer);
   }

   graduatedExtentLimitedContainer->setAt(qfe);
}

void CQfeLayeredContainer::traceDump(CWriteFormat* writeFormat,const char* header)
{
   writeFormat->writef(
      "______________________________________________________\n%s CQfeLayeredContainer - m_maxLayer=%d\n",header,m_maxLayer);
   writeFormat->pushHeader(".  ");

   CQfeGraduatedExtentLimitedContainer* graduatedExtentLimitedContainer;

   for (int layer = 0;layer <= m_maxLayer;layer++)
   {
      graduatedExtentLimitedContainer = m_qfeLayers.GetAt(layer);

      writeFormat->writef(
         "layer %d%s\n\n",layer,(graduatedExtentLimitedContainer == NULL ? " NULL" : ""));

      if (graduatedExtentLimitedContainer != NULL)
      {
         graduatedExtentLimitedContainer->traceDump(writeFormat,"");
      }
   }

   writeFormat->popHeader();
}

bool CQfeLayeredContainer::isValid() const
{
   bool retval = true;
   CQfeGraduatedExtentLimitedContainer* graduatedExtentLimitedContainer;

   for (int layer = 0;retval && layer <= m_maxLayer;layer++)
   {
      graduatedExtentLimitedContainer = m_qfeLayers.GetAt(layer);

      if (graduatedExtentLimitedContainer != NULL)
      {
         retval = graduatedExtentLimitedContainer->isValid();
      }
   }

   return retval;
}

void CQfeLayeredContainer::assertValid() const
{
   if (! isValid())
   {
      AfxDebugBreak();
      isValid();
   }
}

void CQfeLayeredContainer::printMetricsReport(CWriteFormat& writeFormat)
{
   CQfeGraduatedExtentLimitedContainer* graduatedExtentLimitedContainer;

   for (int layer = 0;layer <= m_maxLayer;layer++)
   {
      graduatedExtentLimitedContainer = m_qfeLayers.GetAt(layer);

      if (graduatedExtentLimitedContainer != NULL)
      {
         writeFormat.writef("CQfeLayeredContainer::printMetricsReport(), layer = %d\n",layer);
         graduatedExtentLimitedContainer->printMetricsReport(writeFormat);
      }
   }
}







