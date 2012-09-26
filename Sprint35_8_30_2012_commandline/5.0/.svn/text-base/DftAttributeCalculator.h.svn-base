// $Header: /CAMCAD/5.0/DftAttributeCalculator.h 17    6/21/07 8:26p Kurt Van Ness $

#pragma once

#include "QfeLib.h"
#include "TypedContainer.h"
#include "CamCadDatabase.h"
#include "QfeLib.h"

class CCEtoODBDoc;
class FileStruct;

bool calculatePcbValidationAttributes(bool generateIndicatorsFlag=false);

//_____________________________________________________________________________
class CComponentOutlineEntry : public CObject
{
private:
   DataStruct* m_componentData;
   CPolyList* m_outlinePolyList;

public:
   CComponentOutlineEntry(DataStruct* componentData,CPolyList* outlinePolyList);
};

//_____________________________________________________________________________
class CComponentOutlines : public CObject
{
private:
   CTypedObListContainer<CComponentOutlineEntry*> m_outlines;

public:
   void add(DataStruct* componentData,CPolyList* outlinePolyList);
};

//_____________________________________________________________________________
class CQfeComponentOutline : public CQfe
{
private:
   DataStruct* m_componentData;
   CPolyList* m_outlinePolyList;
   CExtent m_extent;
   CPoint2d m_origin;

public:
   CQfeComponentOutline(DataStruct& componentData,const CPolyList& outlinePolyList,
      CTMatrix& componentMatrix);
   ~CQfeComponentOutline();

public:
   // 
   double distanceTo(const CPoint2d& point,PageUnitsTag pageUnits,CPoint2d* pointResult=NULL);
   DataStruct* getComponentData() { return m_componentData; }
   CPolyList* getOutlinePolyList() { return m_outlinePolyList; }

   // CQfe overrides
   virtual CPoint2d getOrigin()    const { return m_origin; }
   virtual CExtent getExtent()     const { return m_extent; }
   virtual CString getInfoString() const { return "CQfeComponentOutline"; }

   // CObject2d overrides
   virtual bool isExtentIntersecting(const CExtent& extent);
   virtual bool isPointWithinDistance(const CPoint2d& point,double distance);
   virtual bool isInViolation(CObject2d& otherObject);
   virtual int getObjectType() const;

private:
};

//_____________________________________________________________________________
class CQfeComponentOutlineList : public CTypedPtrList<CQfeList,CQfeComponentOutline*>
{
private:
   bool m_isContainer;

public:
   CQfeComponentOutlineList(bool isContainer=false,int nBlockSize=200);
   ~CQfeComponentOutlineList();
   void empty();
};

//_____________________________________________________________________________
class CQfeComponentOutlineTree : public CQfeGraduatedExtentLimitedContainer
{
private:

public:
   CQfeComponentOutlineTree();
   ~CQfeComponentOutlineTree();

public:
   virtual int search(const CExtent& extent,CQfeComponentOutlineList& foundList)
      { return CQfeGraduatedExtentLimitedContainer::search(extent,foundList); }

   virtual CQfeComponentOutline* findFirst(const CExtent& extent)
      { return (CQfeComponentOutline*)CQfeGraduatedExtentLimitedContainer::findFirst(extent); }

   virtual CQfeComponentOutline* findFirstViolation(const CExtent& extent,CQfe& qfe)
      { return (CQfeComponentOutline*)CQfeGraduatedExtentLimitedContainer::findFirstViolation(extent,qfe); }

   virtual CQfeComponentOutline* findFirstEdgeToRectViolation(const CExtent& extent,int& entitiesChecked)
      { return (CQfeComponentOutline*)CQfeGraduatedExtentLimitedContainer::findFirstEdgeToRectViolation(extent,entitiesChecked); }

   virtual CQfeComponentOutline* findFirstEdgeToPointViolation(const CExtent& extent,
      const CPoint2d& point,double distance,int& entitiesChecked)
      { return (CQfeComponentOutline*)CQfeGraduatedExtentLimitedContainer::findFirstEdgeToPointViolation(extent,point,
                                                        distance,entitiesChecked); }

   virtual void setAt(CQfeComponentOutline* qfe)
      { CQfeGraduatedExtentLimitedContainer::setAt(qfe); }
};

//_____________________________________________________________________________
class CDftIndicators : public CObject
{
private:
   CCamCadDatabase& m_camCadDatabase;
   bool m_generateIndicatorsFlag;

   int m_topPinIndicatorLayerIndex;
   int m_bottomPinIndicatorLayerIndex;
   int m_topPinExtentIndicatorLayerIndex;
   int m_bottomPinExtentIndicatorLayerIndex;
   int m_pinToBoardIndicatorLayerIndex;
   int m_boardIndicatorLayerIndex;

   int m_boardIndicatorWidthIndex;
   int m_pinToBoardIndicatorWidthIndex;
   int m_bottomPinIndicatorWidthIndex;
   int m_topPinIndicatorWidthIndex;
   int m_bottomPinExtentIndicatorWidthIndex;
   int m_topPinExtentIndicatorWidthIndex;

   int m_boardOutlineIndicatorWidthIndex;
   int m_topComponenOutlineIndicatorWidthIndex;
   int m_bottomComponenOutlineIndicatorWidthIndex;

public:
   CDftIndicators(CCamCadDatabase& camCadDatabase);

   void saveNamedView();

   int getTopPinIndicatorLayerIndex()          { return m_topPinIndicatorLayerIndex; }
   int getBottomPinIndicatorLayerIndex()       { return m_bottomPinIndicatorLayerIndex; }
   int getTopPinExtentIndicatorLayerIndex()    { return m_topPinExtentIndicatorLayerIndex; }
   int getBottomPinExtentIndicatorLayerIndex() { return m_bottomPinExtentIndicatorLayerIndex; }
   int getPinToBoardIndicatorLayerIndex()      { return m_pinToBoardIndicatorLayerIndex; }
   int getBoardIndicatorLayerIndex()           { return m_boardIndicatorLayerIndex; }

   void setGenerateIndicatorsFlag(bool flag);

   void addTopPinExtentIndicator(BlockStruct* boardBlock,
      const CPoint2d& padOrigin,const CExtent& padExtent,
      const CPoint2d& extentOffset)
   { 
      addExtentIndicator(boardBlock,
         m_topPinExtentIndicatorLayerIndex,m_topPinExtentIndicatorWidthIndex,
         padOrigin,padExtent,extentOffset); 
   }

   void addBottomPinExtentIndicator(BlockStruct* boardBlock,
      const CPoint2d& padOrigin,const CExtent& padExtent,
      const CPoint2d& extentOffset)
   { 
      addExtentIndicator(boardBlock,
         m_bottomPinExtentIndicatorLayerIndex,m_bottomPinExtentIndicatorWidthIndex,
         padOrigin,padExtent,extentOffset); 
   }

   void addBoardArrowIndicator(BlockStruct* boardBlock,
      const CPoint2d& fromPoint,const CPoint2d& toPoint)
   {
      addArrowIndicator(boardBlock,m_pinToBoardIndicatorLayerIndex,m_pinToBoardIndicatorWidthIndex,
         fromPoint,toPoint);
   }

   void addTopPinArrowIndicator(BlockStruct* boardBlock,
      const CPoint2d& fromPoint,const CPoint2d& toPoint)
   {
      addArrowIndicator(boardBlock,m_topPinIndicatorLayerIndex,m_topPinIndicatorWidthIndex,
         fromPoint,toPoint);
   }

   void addBottomPinArrowIndicator(BlockStruct* boardBlock,
      const CPoint2d& fromPoint,const CPoint2d& toPoint)
   {
      addArrowIndicator(boardBlock,m_bottomPinIndicatorLayerIndex,m_bottomPinIndicatorWidthIndex,
         fromPoint,toPoint);
   }

   void addTopPinCircleIndicator(BlockStruct* boardBlock,
      const CPoint2d& fromPoint,const CPoint2d& toPoint)
   {
      addCircleIndicator(boardBlock,m_topPinIndicatorLayerIndex,m_topPinIndicatorWidthIndex,
         fromPoint,toPoint);
   }

   void addBottomPinCircleIndicator(BlockStruct* boardBlock,
      const CPoint2d& fromPoint,const CPoint2d& toPoint)
   {
      addCircleIndicator(boardBlock,m_bottomPinIndicatorLayerIndex,m_bottomPinIndicatorWidthIndex,
         fromPoint,toPoint);
   }

   void addBoardOutlineIndicator(BlockStruct* boardBlock,const CPolyList& polyList)
   {
      addOutlineIndicator(boardBlock,m_boardIndicatorLayerIndex,
         m_boardOutlineIndicatorWidthIndex,polyList);
   }

   void addTopComponentOutlineIndicator(BlockStruct* boardBlock,const CPolyList& polyList)
   {
      addOutlineIndicator(boardBlock,m_topPinIndicatorLayerIndex,
         m_topComponenOutlineIndicatorWidthIndex,polyList);
   }

   void addBottomComponentOutlineIndicator(BlockStruct* boardBlock,const CPolyList& polyList)
   {
      addOutlineIndicator(boardBlock,m_bottomPinIndicatorLayerIndex,
         m_bottomComponenOutlineIndicatorWidthIndex,polyList);
   }

private:
   void addExtentIndicator(BlockStruct* boardBlock,
      int layerIndex,int widthIndex,const CPoint2d& padOrigin,const CExtent& padExtent,
      const CPoint2d& extentOffset);
   void CDftIndicators::addArrowIndicator(BlockStruct* boardBlock,
      int layerIndex,int widthIndex,const CPoint2d& fromPoint,const CPoint2d& toPoint);
   void addCircleIndicator(BlockStruct* boardBlock,
      int layerIndex,int widthIndex,const CPoint2d& fromPoint,const CPoint2d& toPoint);
   void addOutlineIndicator(BlockStruct* boardBlock,
      int layerIndex,int widthIndex,const CPolyList& polyList);
};

//_____________________________________________________________________________
class CDftAttributeCalculator : public CObject
{
private:
   CCamCadDatabase m_camCadDatabase;
   FileStruct* m_boardFile;
   BlockStruct* m_boardBlock;
   CQfeComponentOutlineTree m_topComponentOutlineTree;
   CQfeComponentOutlineTree m_bottomComponentOutlineTree;
   CDftIndicators m_dftIndicators;

   int m_kwDftBoardOutlineOk;          

   int m_kwDftComponentPinCentroidX;   
   int m_kwDftComponentPinCentroidY;   
   int m_kwDftComponentOutlineOk;      
   int m_kwDftComponentOutlineSource;  

   int m_kwDftPinToBoardDistance;      
   int m_kwDftPinToTopComponentDistance;
   int m_kwDftPinToBottomComponentDistance;
   int m_kwDftPinTopClosestComponent;
   int m_kwDftPinBottomClosestComponent;
   int m_kwDftPinAccess;                     

   int m_kwDftPinMetalExtent;
   int m_kwDftPinMetalExtentX;
   int m_kwDftPinMetalExtentY;
   int m_kwDftPinMetalExtentXT;
   int m_kwDftPinMetalExtentYT;
   int m_kwDftPinMetalExtentXB;
   int m_kwDftPinMetalExtentYB;

   int m_kwDftPinMetalOffset;
   int m_kwDftPinMetalOffsetX;
   int m_kwDftPinMetalOffsetY;
   int m_kwDftPinMetalOffsetXT;
   int m_kwDftPinMetalOffsetYT;
   int m_kwDftPinMetalOffsetXB;
   int m_kwDftPinMetalOffsetYB;

public:
   CDftAttributeCalculator(CCEtoODBDoc& camCadDoc,CWriteFormat& errorLog);
   ~CDftAttributeCalculator();

   bool calculateDftAttributes(bool generateIndicatorsFlag,CWriteFormat& errorLog);
   bool calculateDftAttributes(int boardFileNumber,bool generateIndicatorsFlag,CWriteFormat& errorLog);
   void clearAttributes();
   CPolyList* getPadPolyList(BlockStruct& padBlock,CTMatrix& padMatrix);
   bool calculatePinMetalExtent(CExtent& pinMetalExtent,
      CPolyList* metalPadPolyList,CPolyList* maskPadPolyList);
   CPoint2d calculatePadStackAttributes(InsertStruct* padstackInsert,
      CAttributes** padstackParentAttributeMap,CTMatrix& padstackParentMatrix,
      CPolyList* boardOutlinePolyList,CExtent& padMetalExtentsExtent,
      CDftIndicators& dftIndicators,CWriteFormat& errorLog);
   void calculateProbeToComponentAttributes(const CPoint2d probeOrigin,   
      CAttributes** probeParentAttributeMap,CWriteFormat& errorLog);
};
