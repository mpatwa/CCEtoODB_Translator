// $Header: /CAMCAD/4.4/GeometryTree.cpp 12    3/21/04 3:36a Kurt Van Ness $

#include "StdAfx.h"
#include "CcDoc.h"
#include "GeometryTree.h"

#if defined (EnableGeometryTreeImplementation)

//_____________________________________________________________________________
CInsertInstance::CInsertInstance(CInsertNode& parentNode)
{
   m_parentNode = &parentNode;
}

CInsertInstance::~CInsertInstance()
{
}

CPoint2d CInsertInstance::getOrigin() const
{
   return m_extent.getCenter();
}

CExtent CInsertInstance::getExtent() const
{
   return m_extent;
}

bool CInsertInstance::isExtentIntersecting(const CExtent& extent)
{
   bool retval = extent.intersects(m_extent);

   return retval;
}

bool CInsertInstance::isInViolation(CObject2d& otherObject)
{
   return false;
}

//_____________________________________________________________________________
CPolyInsertInstance::CPolyInsertInstance(CInsertNode& parentNode,CPolyList& poly,
   const CTMatrix& transformationMatrix) :
   CInsertInstance(parentNode)
{
   m_poly = &poly;
   m_extent = m_poly->getExtent(transformationMatrix);
}

CPolyInsertInstance::~CPolyInsertInstance()
{
}

int CPolyInsertInstance::getObjectType() const
{
   return GteTypePolyInsertInstance;
}

CString CPolyInsertInstance::getInfoString() const
{
   return CString("CPolyInsertInstance");;
}

bool CPolyInsertInstance::isPointWithinDistance(const CPoint2d& point,double distance)
{
   bool retval = false;
   
   // To Do ---- Implementation

   return retval;
}

void CPolyInsertInstance::draw(CCEtoODBDoc& camCadDoc,CDC& dc)
{
            //if (ThumbTracking && data->getGraphicClass() == GR_CLASS_ETCH)
            // continue;

            //if (doc->showSeqNums || doc->showSeqArrows)
            //{
            // Point2 point2;
            // CPoly *poly = data->getPolyList()->GetHead();
      
            // if (poly->isHidden()) break;

            // CPnt *p1, *p2;
            // POSITION pos = poly->getPntList().GetHeadPosition();
            // p1 = poly->getPntList().GetNext(pos);
            // p2 = poly->getPntList().GetNext(pos);
            // double cx, cy, r;
            // if (PolyIsCircle(poly, &cx, &cy, &r)) // concentric circles would have same exact point
            // {
            //    point2.x = p1->x * scale;
            //    point2.y = p1->y * scale;
            // }
            // else
            // {
            //    point2.x = (p1->x + p2->x) / 2 * scale;
            //    point2.y = (p1->y + p2->y) / 2 * scale;
            // }
            // if (mirror & MIRROR_FLIP)  point2.x = -point2.x;
            // TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // if (doc->showSeqNums)
            // {
            //    char buf[10];
            //    sprintf(buf, "%d", seqnum++);
            //    pDC->SetBkMode(OPAQUE);
            //    pDC->TextOut(round(point2.x * scaleFactor), round(point2.y * scaleFactor), buf);
            //    pDC->SetBkMode(TRANSPARENT);
            // }
            // if (doc->showSeqArrows)
            // {
            //    if (data->flag & DATA_SEQ_HEAD)
            //    {
            //       seq_x = point2.x;
            //       seq_y = point2.y;
            //    }
            //    else if (data->flag & DATA_SEQ_CONT)
            //    {
            //       DrawArrow(pDC, seq_x, seq_y, point2.x, point2.y, scaleFactor, doc->Settings.RL_arrowSize, RGB(255, 0, 0));
            //       seq_x = point2.x;
            //       seq_y = point2.y;
            //    }
            // }
            //}

            //BOOL ShowFills = doc->showFills;
            //if (globalShowFills >=0)
            // ShowFills = globalShowFills;
            //if (DrawPolyStruct(pDC, data->getPolyList(), scale, mirror & MIRROR_FLIP, insert_x, insert_y, &m, layer, ShowFills, TRUE))
            // entityCount = BREAK_DRAW_RATE + 1; // check for ESCAPE after memDC polys because they take so long
}

//_____________________________________________________________________________
CPointInsertInstance::CPointInsertInstance(CInsertNode& parentNode,PointStruct& point,
   const CTMatrix& transformationMatrix) :
   CInsertInstance(parentNode)
{
   m_point= &point;

   CPoint2d xPoint(point.x,point.y);
   transformationMatrix.transform(xPoint);
   m_extent.update(xPoint);;
}

CPointInsertInstance::~CPointInsertInstance()
{
}

int CPointInsertInstance::getObjectType() const
{
   return GteTypePointInsertInstance;
}

CString CPointInsertInstance::getInfoString() const
{
   return CString("CPointInsertInstance");;
}

bool CPointInsertInstance::isPointWithinDistance(const CPoint2d& point,double distance)
{
   bool retval = false;
   
   // To Do ---- Implementation

   return retval;
}

void CPointInsertInstance::draw(CCEtoODBDoc& camCadDoc,CDC& dc)
{
   //Point2 p;
   //p.x = data->getPoint()->x * scale;
   //if (mirror) p.x = -p.x;
   //p.y = data->getPoint()->y * scale;
   //TransPoint2(&p, 1, &m, insert_x, insert_y);

   //int x, y, size;
   //size = round(GetCursorSize());
   //x = round(p.x * scaleFactor);
   //y = round(p.y * scaleFactor);

   //SetupPens(doc, pDC, FALSE, 0, 1.0, FALSE, 0);

   //pDC->MoveTo(x + size, y);
   //pDC->LineTo(x - size, y);
   //pDC->MoveTo(x, y + size);
   //pDC->LineTo(x, y - size);
}

//_____________________________________________________________________________
CTextInsertInstance::CTextInsertInstance(CInsertNode& parentNode,TextStruct& text,
   const CTMatrix& transformationMatrix) :
   CInsertInstance(parentNode)
{
   m_text   = &text;
   m_extent = m_text->getExtent(transformationMatrix);
}

CTextInsertInstance::~CTextInsertInstance()
{
}

int CTextInsertInstance::getObjectType() const
{
   return GteTypeTextInsertInstance;
}

CString CTextInsertInstance::getInfoString() const
{
   return CString("CTextInsertInstance");
}

bool CTextInsertInstance::isPointWithinDistance(const CPoint2d& point,double distance)
{
   bool retval = false;


   
   // To Do ---- Implementation

   return retval;
}

void CTextInsertInstance::draw(CCEtoODBDoc& camCadDoc,CDC& dc)
{
   //if (FontLoaded && !ThumbTracking)
   //{
   // DrawTextEntity(pDC, data, scale, mirror & MIRROR_FLIP, insert_x, insert_y, &m, rotation);
   //}
}

//_____________________________________________________________________________
CInsertNode::CInsertNode()
{
   m_parent = NULL;
   m_block  = NULL;
}

CInsertNode::CInsertNode(CInsertNode& parent,const CTMatrix& matrix)
{
   m_parent  = &parent;
   m_block   = NULL;
   m_TMatrix = matrix;
}

CInsertNode::~CInsertNode()
{
}

void CInsertNode::empty()
{
   m_childNodes.empty();
   m_childInstances.empty();
}

void CInsertNode::load(CCEtoODBDoc& camCadDoc,BlockStruct& block,int layerIndex,CGeometryTree& tree)
{
   m_block = &block;

   for (POSITION dataPos = m_block->getDataList().GetHeadPosition();dataPos != NULL;)
   {
      DataStruct* data = m_block->getDataList().GetNext(dataPos);

      if (data->getLayerIndex() != -1)
      {
         layerIndex = data->getLayerIndex();
      }

      if (data->getDataType() == T_INSERT)
      {
         InsertStruct* dataInsert = data->getInsert();
         BlockStruct* childBlock = camCadDoc.BlockArray.GetAt(dataInsert->num);

         if (childBlock != NULL)
         {
            CTMatrix matrix = dataInsert->getTMatrix() * m_TMatrix;

            CInsertNode* childNode = new CInsertNode(*this,matrix);
            m_childNodes.AddTail(childNode);
            childNode->load(camCadDoc,*childBlock,layerIndex,tree);
         }
      }
      else if (layerIndex >= 0)
      {
         switch (data->getDataType())
         {
         case T_POLY:
            if (data->getPolyList() != NULL)
            {
               CPolyInsertInstance* instance = new CPolyInsertInstance(*this,*(data->getPolyList()),m_TMatrix);
               m_childInstances.AddTail(instance);
               tree.setAt(layerIndex,instance);
            }
            
            break;
         case T_POINT:
            if (data->getPoint() != NULL)
            {
               CPointInsertInstance* instance = new CPointInsertInstance(*this,*(data->getPoint()),m_TMatrix);
               m_childInstances.AddTail(instance);
               tree.setAt(layerIndex,instance);
            }
            
            break;
         case T_TEXT:
            if (data->getText() != NULL)
            {
               CTextInsertInstance* instance = new CTextInsertInstance(*this,*(data->getText()),m_TMatrix);
               m_childInstances.AddTail(instance);
               tree.setAt(layerIndex,instance);
            }
            
            break;
         }
      }
   }
}

//_____________________________________________________________________________
CGeometryTreeExtentLimitedContainer::CGeometryTreeExtentLimitedContainer(const CSize2d& maxElementExtentSize) :
   m_maxElementExtentSize(maxElementExtentSize)
{
}

CGeometryTreeExtentLimitedContainer::~CGeometryTreeExtentLimitedContainer()
{
}

void CGeometryTreeExtentLimitedContainer::empty()
{
   m_tree.empty();
}

void CGeometryTreeExtentLimitedContainer::setAt(CGte* gte)
{
   m_tree.setAt(gte->getOrigin(),gte);
}

void CGeometryTreeExtentLimitedContainer::draw(CCEtoODBDoc& camCadDoc,CDC& dc,const CExtent& viewExtent)
{
   CGteList foundList;
   int foundCount = m_tree.search(viewExtent,foundList);

   for (POSITION pos = foundList.GetHeadPosition();pos != NULL;)
   {
      CGte* gte = foundList.GetNext(pos);
      gte->draw(camCadDoc,dc);
   }
}

//_____________________________________________________________________________
CGeometryTreeLayer::CGeometryTreeLayer()
{
   m_granularity    = .010;
   //m_maxFeatureSize = m_granularity;
   m_xSize          = 0;
   m_ySize          = 0;

   setSize(8,8);
}

CGeometryTreeLayer::~CGeometryTreeLayer()
{
}

void CGeometryTreeLayer::empty()
{
   m_containerArray.empty();
   m_list.RemoveAll();
}

void CGeometryTreeLayer::setSize(int xSize,int ySize)
{
   CTypedPtrArray<CObArray,CGeometryTreeExtentLimitedContainer*> newArray;
   newArray.SetSize(xSize * ySize);

   for (int yInd = 0;yInd < m_ySize && yInd < ySize;yInd++)
   {
      for (int xInd = 0;xInd < m_xSize && xInd < xSize;xInd++)
      {
         int oldInd = (yInd * m_xSize) + xInd;
         int newInd = (yInd * xSize  ) + xInd;

         CGeometryTreeExtentLimitedContainer* extentLimitedContainer = m_containerArray.GetAt(oldInd);

         if (extentLimitedContainer != NULL)
         {
            newArray.SetAt(newInd,extentLimitedContainer);
            m_containerArray.SetAt(oldInd,NULL);
         }
      }
   }

   m_containerArray.empty();
   m_containerArray.SetSize(newArray.GetSize());
   m_list.RemoveAll();

   for (int ind = 0;ind < newArray.GetSize();ind++)
   {
      CGeometryTreeExtentLimitedContainer* extentLimitedContainer = newArray.GetAt(ind);

      if (extentLimitedContainer != NULL)
      {
         m_containerArray.SetAt(ind,extentLimitedContainer);
         m_list.AddTail(extentLimitedContainer);
      }
   }

   m_xSize = xSize;
   m_ySize = ySize;
}

void CGeometryTreeLayer::setAt(CGte* gte)
{
   // array Index:   0    1    2     3     4     5      6
   //               ==0  <=g  <=2g  <=4g  <=8g  <=16g  >16g

   CExtent gteExtent(gte->getExtent());
   double xGteDimension = gteExtent.getXsize();
   double yGteDimension = gteExtent.getYsize();

   int xInd=0,yInd=0;
   double xExtentDimension=0.,yExtentDimension=0.;

   if (xGteDimension > 0.)
   {
      xExtentDimension = m_granularity;

      while (xGteDimension > xExtentDimension)
      {
         xInd++;
         xExtentDimension *= 2.;
      }
   }

   if (yGteDimension > 0.)
   {
      yExtentDimension = m_granularity;

      while (yGteDimension > yExtentDimension)
      {
         yInd++;
         yExtentDimension *= 2.;
      }
   }

   if (xInd >= m_xSize)
   {
      if (yInd >= m_ySize)
      {
         setSize(xInd + 1,yInd + 1);
      }
      else
      {
         setSize(xInd + 1,m_ySize);
      }
   }
   else if (yInd >= m_ySize)
   {
      setSize(m_xSize,yInd + 1);
   }

   int arrayIndex = (yInd * m_xSize) + xInd;

   CGeometryTreeExtentLimitedContainer* extentLimitedContainer = m_containerArray.GetAt(arrayIndex);

   if (extentLimitedContainer == NULL)
   {
      CSize2d maxElementExtentSize(xExtentDimension,yExtentDimension);

      extentLimitedContainer = new CGeometryTreeExtentLimitedContainer(maxElementExtentSize);
      m_containerArray.SetAtGrow(arrayIndex,extentLimitedContainer);
      m_list.AddTail(extentLimitedContainer);
   }

   extentLimitedContainer->setAt(gte);
}

void CGeometryTreeLayer::draw(CCEtoODBDoc& camCadDoc,CDC& dc,const CExtent& viewExtent)
{
   CGeometryTreeExtentLimitedContainer* extentLimitedContainer;

   for (POSITION pos = m_list.GetHeadPosition();pos != NULL;)
   {
      extentLimitedContainer = m_list.GetNext(pos);

      extentLimitedContainer->draw(camCadDoc,dc,viewExtent);
   }
}

//_____________________________________________________________________________
CGeometryTreeLayer* CGeometryTreeLayerArray::getAt(int layerIndex)
{
   CGeometryTreeLayer* layer = NULL;

   if (layerIndex < GetSize())
   {
      layer = GetAt(layerIndex);
   }

   if (layer == NULL)
   {
      layer = new CGeometryTreeLayer();
      SetAtGrow(layerIndex,layer);
   }

   return layer;
}

//_____________________________________________________________________________
CGeometryTree::CGeometryTree()
{
}

CGeometryTree::~CGeometryTree()
{
}

void CGeometryTree::empty()
{
   m_layers.empty();
   m_rootNode.empty();
}

void CGeometryTree::load(CCEtoODBDoc& camCadDoc,FileStruct& fileStruct)
{
   empty();

   BlockStruct* fileBlock = fileStruct.block_ptr;

   if (fileBlock != NULL)
   {
      m_rootNode.setTMatrix(fileStruct.getTMatrix());
      m_rootNode.load(camCadDoc,*fileBlock,-1,*this);
   }
}

void CGeometryTree::draw(CCEtoODBDoc& camCadDoc,CDC& dc,const CExtent& viewExtent)
{
   CLayerArray& layerArray = camCadDoc.LayerArray;

   for (int layerIndex = 0;layerIndex < layerArray.GetSize();layerIndex++)
   {
      LayerStruct* layer = layerArray.GetAt(layerIndex);

      if (layer != NULL)
      {
         CGeometryTreeLayer* treeLayer = m_layers.getAt(layer->getLayerIndex());

         if (layer->isVisible())
         {
            treeLayer->draw(camCadDoc,dc,viewExtent);
         }
      }
   }
}

void CGeometryTree::setAt(int layerIndex,CGte* gte)
{
   CGeometryTreeLayer* geometryTreeLayer = m_layers.getAt(layerIndex);
   geometryTreeLayer->setAt(gte);
}

CGeometryTreeLayer* CGeometryTree::getLayerAt(int layerIndex)
{
   return m_layers.getAt(layerIndex);
}

//_____________________________________________________________________________
CGeometryTreeFilesArray::CGeometryTreeFilesArray()
{
}

CGeometryTreeFilesArray::~CGeometryTreeFilesArray()
{
}

CGeometryTree* CGeometryTreeFilesArray::getAt(int fileIndex)
{
   CGeometryTree* geometryTree = NULL;

   if (fileIndex < m_geometryTrees.GetSize())
   {
      geometryTree = m_geometryTrees.GetAt(fileIndex);
   }

   if (geometryTree == NULL)
   {
      geometryTree = new CGeometryTree();
      m_geometryTrees.SetAtGrow(fileIndex,geometryTree);
   }

   return geometryTree;
}

void CGeometryTreeFilesArray::load(CCEtoODBDoc& camCadDoc)
{
   for (POSITION filePos = camCadDoc.FileList.GetHeadPosition();filePos != NULL;)
   {
      FileStruct* fileStruct = camCadDoc.FileList.GetNext(filePos);

      if (fileStruct != NULL)
      {
         CGeometryTree* fileTree = new CGeometryTree();

         m_geometryTrees.setAt(fileStruct->getFileNumber(),fileTree);

         fileTree->load(camCadDoc,*fileStruct);
      }
   }
}

#endif
