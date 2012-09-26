// $Header: /CAMCAD/5.0/DftAttributeCalculator.cpp 37    6/21/07 8:26p Kurt Van Ness $

#include "StdAfx.h"
#include "DftAttributeCalculator.h"
#include "CamCadDatabase.h"
#include "GerberThermalDfm.h"
#include "Xform.h"
#include "TMState.h"
#include "Region.h"
#include "WriteFormat.h"
#include "RwLib.h"
#include "RwUiLib.h"
#include <Float.h>
#include "CCEtoODB.h"

extern CView *activeView;

double MeasurePointToPolylist(CCEtoODBDoc *doc, Point2 *point,
      CPolyList *polylist, DTransform *polylistXform, BOOL Centerline,
      Point2 *result);
CPolyList *ApertureToPoly_Base(BlockStruct *block, double x, double y, double rot, int mirror);

static inline double getSearchTolerance() { return .150; }

//_____________________________________________________________________________
bool calculatePcbValidationAttributes(bool generateIndicatorsFlag)
{
   bool retval = false;

   CFilePath logFilePath(getApp().getUserPath());
   logFilePath.pushLeaf("DftAttrbuteCalculationLog.htm");

   CHtmlFileWriteFormat htmlLogFile(1024);

   if (!htmlLogFile.open(logFilePath.getPath()))
   {
      formatMessageBoxApp(MB_ICONSTOP,"Could not open the error log, '%s'",
         (const char*)logFilePath.getPath());
   }
   else if (activeView == NULL)
   {
      formatMessageBoxApp(MB_ICONSTOP,"No active view");
   }
   else
   {
      CCEtoODBView* view = (CCEtoODBView*)activeView;
      CCEtoODBDoc* camCadDoc = view->GetDocument();
      htmlLogFile.setNewLineMode(true);

      if (camCadDoc != NULL)
      {
         CDftAttributeCalculator dftAttributeCalculator(*camCadDoc,htmlLogFile);

         retval = dftAttributeCalculator.calculateDftAttributes(generateIndicatorsFlag,htmlLogFile);
      }
   }

   return retval;
}

//_____________________________________________________________________________
CComponentOutlineEntry::CComponentOutlineEntry(DataStruct* componentData,
   CPolyList* outlinePolyList)
{
   m_componentData   = componentData;
   m_outlinePolyList = outlinePolyList;
}

//_____________________________________________________________________________
void CComponentOutlines::add(DataStruct* componentData,CPolyList* outlinePolyList)
{
   CComponentOutlineEntry* entry = new CComponentOutlineEntry(componentData,outlinePolyList);

   m_outlines.AddTail(entry);
}

//_____________________________________________________________________________
CQfeComponentOutline::CQfeComponentOutline(DataStruct& componentData,
   const CPolyList& outlinePolyList,CTMatrix& componentMatrix)
{
   m_componentData = &componentData;
   m_outlinePolyList = new CPolyList(outlinePolyList);
   m_outlinePolyList->closePolys();
   m_outlinePolyList->transform(componentMatrix);
   m_extent = m_outlinePolyList->getExtent();
   m_origin = m_extent.getCenter();
}

CQfeComponentOutline::~CQfeComponentOutline()
{
   delete m_outlinePolyList;
}

double CQfeComponentOutline::distanceTo(const CPoint2d& point,PageUnitsTag pageUnits,CPoint2d* pointResult)
{
   double retval = 0.;

   if (! m_outlinePolyList->isPointInside(point,pageUnits))
   {
      retval = m_outlinePolyList->distanceTo(point,pointResult);
   }

   return retval;
}

bool CQfeComponentOutline::isExtentIntersecting(const CExtent& extent)
{
   bool retval = m_extent.intersects(extent);

   return retval;
}

bool CQfeComponentOutline::isPointWithinDistance(const CPoint2d& point,double distance)
{
   double d = m_extent.distanceTo(point);
   bool retval = (d <= distance);

   return retval;
}

bool CQfeComponentOutline::isInViolation(CObject2d& otherObject)
{
   return false;
}

int CQfeComponentOutline::getObjectType() const
{
   return 0;
}

//_____________________________________________________________________________
CQfeComponentOutlineList::CQfeComponentOutlineList(bool isContainer,int nBlockSize) : 
   CTypedPtrList<CQfeList,CQfeComponentOutline*>(nBlockSize)
{
   m_isContainer = isContainer;
}

CQfeComponentOutlineList::~CQfeComponentOutlineList()
{
   empty();
}

void CQfeComponentOutlineList::empty()
{
   if (m_isContainer)
   {
      for (POSITION pos = GetHeadPosition();pos != NULL;)
      {
         CQfeComponentOutline* qfe = GetNext(pos);
         delete qfe;
      }
   }

   RemoveAll();
}

//_____________________________________________________________________________
CQfeComponentOutlineTree::CQfeComponentOutlineTree() : CQfeGraduatedExtentLimitedContainer(.05,2.0)
{
}

CQfeComponentOutlineTree::~CQfeComponentOutlineTree()
{
}

//_____________________________________________________________________________

/*
   1) CalcBoard2Pin - Calculates the distance from a pin center to the nearest board edge.
      Adds "DftPinToBoardDistance" attribute.
      a) Need board outline.
      b) Need pin coordinates.
   2) CalcComp2Pin - Calculates the distance from a pin center to the nearest top and bottom component edge.
      Adds "DftPinToTopComponentDistance" and "DftPinToBottomComponentDistance" attributes.
      a) Need component outlines.
      b) Need pin coordinates.
   3) CalcCompCentXY - Calculates the pin centroid for each component.  
      Adds "DftComponentPinCentroidX" and "DftComponentPinCentroidY" attributes.
      b) Need pin coordinates.
   5) SetPinTBX - Calculates the exposed sides for a pin.
      Adds "DftPinAccess" attribute.  
      "t" - pin is exposed on top side only.
      "b" - pin is exposed on bottom side only.
      "x" - pin is exposed on both top and bottom sides.
      "n" - pin is not exposed on either the top or bottom sides.
   9) Calculates the extents of the exposed metal area for each pin.  An X and Y extent value
      is calculated for the top and bottom sides of the pin.  If the extent is not centered on the pin,
      then an offset value must also be specified.
      Add some combination of the following attributes:

      When the xTop, yTop, xBottom, yBottom values are all equal:
      "DftPinMetalExtent" ["DftPinMetalOffset"]

      When the xTop and xBottom values are equal:
      "DftPinMetalExtentX" ["DftPinMetalOffsetX"]

      When the yTop and yBottom values are equal:
      "DftPinMetalExtentY" ["DftPinMetalOffsetY"]

      When the xTop and yTop values are equal:
      "DftPinMetalExtentT" ["DftPinMetalOffsetT"]

      When the xBottom and yBottom values are equal:
      "DftPinMetalExtentB" ["DftPinMetalOffsetB"]

      When the xTop and yTop values are equal and the xBottom and yBottom values are equal:
      "DftPinMetalExtentXT" ["DftPinMetalOffsetXT"],
      "DftPinMetalExtentXB" ["DftPinMetalOffsetXB"],
      "DftPinMetalExtentYT" ["DftPinMetalOffsetYT"],
      "DftPinMetalExtentYB" ["DftPinMetalOffsetYB"]
   10) IsBoardOutlineOK 
   11) IsCompOutlineOK 

*/

CDftAttributeCalculator::CDftAttributeCalculator(CCEtoODBDoc& camCadDoc,CWriteFormat& errorLog) :
   m_camCadDatabase(camCadDoc) ,
   m_dftIndicators(m_camCadDatabase)
{
   m_boardFile     = NULL;
   m_boardBlock    = NULL;

   m_kwDftBoardOutlineOk               = m_camCadDatabase.registerKeyWord("DftBoardOutlineOk"              ,0,valueTypeString,errorLog);

   m_kwDftComponentPinCentroidX        = m_camCadDatabase.registerKeyWord("DftComponentPinCentroidX"       ,0,valueTypeDouble,errorLog);
   m_kwDftComponentPinCentroidY        = m_camCadDatabase.registerKeyWord("DftComponentPinCentroidY"       ,0,valueTypeDouble,errorLog);
   m_kwDftComponentOutlineOk           = m_camCadDatabase.registerKeyWord("DftComponentOutlineOk"          ,0,valueTypeString,errorLog);
   m_kwDftComponentOutlineSource       = m_camCadDatabase.registerKeyWord("DftComponentOutlineSource"      ,0,valueTypeString,errorLog);

   m_kwDftPinToBoardDistance           = m_camCadDatabase.registerKeyWord("DftPinToBoardDistance"          ,0,valueTypeDouble,errorLog);
   m_kwDftPinToTopComponentDistance    = m_camCadDatabase.registerKeyWord("DftPinToTopComponentDistance"   ,0,valueTypeDouble,errorLog);
   m_kwDftPinToBottomComponentDistance = m_camCadDatabase.registerKeyWord("DftPinToBottomComponentDistance",0,valueTypeDouble,errorLog);
   m_kwDftPinTopClosestComponent       = m_camCadDatabase.registerKeyWord("DftPinTopClosestComponent"      ,0,valueTypeString,errorLog);
   m_kwDftPinBottomClosestComponent    = m_camCadDatabase.registerKeyWord("DftPinBottomClosestComponent"   ,0,valueTypeString,errorLog);
   m_kwDftPinAccess                    = m_camCadDatabase.registerKeyWord("DftPinAccess"                   ,0,valueTypeString,errorLog);

   m_kwDftPinMetalExtent               = m_camCadDatabase.registerKeyWord("DftPinMetalExtent"              ,0,valueTypeDouble,errorLog);
   m_kwDftPinMetalExtentX              = m_camCadDatabase.registerKeyWord("DftPinMetalExtentX"             ,0,valueTypeDouble,errorLog);
   m_kwDftPinMetalExtentY              = m_camCadDatabase.registerKeyWord("DftPinMetalExtentY"             ,0,valueTypeDouble,errorLog);
   m_kwDftPinMetalExtentXT             = m_camCadDatabase.registerKeyWord("DftPinMetalExtentXT"            ,0,valueTypeDouble,errorLog);
   m_kwDftPinMetalExtentXB             = m_camCadDatabase.registerKeyWord("DftPinMetalExtentXB"            ,0,valueTypeDouble,errorLog);
   m_kwDftPinMetalExtentYT             = m_camCadDatabase.registerKeyWord("DftPinMetalExtentYT"            ,0,valueTypeDouble,errorLog);
   m_kwDftPinMetalExtentYB             = m_camCadDatabase.registerKeyWord("DftPinMetalExtentYB"            ,0,valueTypeDouble,errorLog);

   m_kwDftPinMetalOffset               = m_camCadDatabase.registerKeyWord("DftPinMetalOffset"              ,0,valueTypeDouble,errorLog);
   m_kwDftPinMetalOffsetX              = m_camCadDatabase.registerKeyWord("DftPinMetalOffsetX"             ,0,valueTypeDouble,errorLog);
   m_kwDftPinMetalOffsetY              = m_camCadDatabase.registerKeyWord("DftPinMetalOffsetY"             ,0,valueTypeDouble,errorLog);
   m_kwDftPinMetalOffsetXT             = m_camCadDatabase.registerKeyWord("DftPinMetalOffsetXT"            ,0,valueTypeDouble,errorLog);
   m_kwDftPinMetalOffsetXB             = m_camCadDatabase.registerKeyWord("DftPinMetalOffsetXB"            ,0,valueTypeDouble,errorLog);
   m_kwDftPinMetalOffsetYT             = m_camCadDatabase.registerKeyWord("DftPinMetalOffsetYT"            ,0,valueTypeDouble,errorLog);
   m_kwDftPinMetalOffsetYB             = m_camCadDatabase.registerKeyWord("DftPinMetalOffsetYB"            ,0,valueTypeDouble,errorLog);
}

CDftAttributeCalculator::~CDftAttributeCalculator()
{
}

void CDftAttributeCalculator::clearAttributes()
{
   if (m_boardBlock == NULL)
   {
      return;
   }

   // board
   m_camCadDatabase.removeAttribute(getAttributeMap(m_boardBlock),m_kwDftBoardOutlineOk);

   // package/via loop
   CDataList& boardDataList = m_boardBlock->getDataList();

   for (POSITION boardDataPos = boardDataList.GetHeadPosition();boardDataPos != NULL;)
   {
      DataStruct* componentData = boardDataList.GetNext(boardDataPos);

      if (componentData != NULL && componentData->getDataType() == dataTypeInsert)
      {
         InsertStruct* packageInsert = componentData->getInsert();

         if (packageInsert->getInsertType() == INSERTTYPE_PCBCOMPONENT ||
             packageInsert->getInsertType() == INSERTTYPE_VIA             )
         {
            m_camCadDatabase.removeAttribute(getAttributeMap(componentData),m_kwDftComponentPinCentroidX);
            m_camCadDatabase.removeAttribute(getAttributeMap(componentData),m_kwDftComponentPinCentroidY);
            m_camCadDatabase.removeAttribute(getAttributeMap(componentData),m_kwDftComponentOutlineOk);
            m_camCadDatabase.removeAttribute(getAttributeMap(componentData),m_kwDftComponentOutlineSource);
         }
      }
   }

   // net loop
   CNetList& netList = m_boardFile->getNetList();

   for (POSITION netPos = netList.GetHeadPosition();netPos != NULL;)
   {
      NetStruct* netStruct = netList.GetNext(netPos);

      CCompPinList& compPinList = netStruct->getCompPinList();

      // component pin loop
      for (POSITION compPinPos = compPinList.getHeadPosition();compPinPos != NULL;)
      {
         CompPinStruct* compPin = compPinList.getNext(compPinPos);

         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinToBoardDistance           );
         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinToTopComponentDistance    );
         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinToBottomComponentDistance );
         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinTopClosestComponent       );
         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinBottomClosestComponent    );
         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinAccess                    );

         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinMetalExtent               );
         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinMetalExtentX              );
         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinMetalExtentY              );
         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinMetalExtentXT             );
         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinMetalExtentXB             );
         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinMetalExtentYT             );
         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinMetalExtentYB             );

         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinMetalOffset               );
         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinMetalOffsetX              );
         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinMetalOffsetY              );
         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinMetalOffsetXT             );
         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinMetalOffsetXB             );
         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinMetalOffsetYT             );
         m_camCadDatabase.removeAttribute(getAttributeMap(compPin),m_kwDftPinMetalOffsetYB             );
      }
   }
}

bool CDftAttributeCalculator::calculateDftAttributes(bool generateIndicatorsFlag,CWriteFormat& errorLog)
{
   bool retval = true;

   CFileList& fileList = m_camCadDatabase.getCamCadDoc().getFileList();

   for (POSITION pos = fileList.GetHeadPosition();pos != NULL;)
   {
      FileStruct* fileStruct = fileList.GetNext(pos);

      if (fileStruct->getBlockType() == BLOCKTYPE_PCB)
      {
         retval = (calculateDftAttributes(fileStruct->getFileNumber(),generateIndicatorsFlag,errorLog) && retval);
      }
   }

   return retval;
}

bool CDftAttributeCalculator::calculateDftAttributes(int boardFileNumber,bool generateIndicatorsFlag,CWriteFormat& errorLog)
{
   CWaitCursor waitCursor;

   m_boardFile     = m_camCadDatabase.getFile(boardFileNumber);
   m_boardBlock    = ((m_boardFile != NULL) ? m_boardFile->getBlock() : NULL);

   if (m_boardBlock == NULL)
   {
      return false;
   }

   m_dftIndicators.setGenerateIndicatorsFlag(generateIndicatorsFlag);

   clearAttributes();

   bool retval = true;
   DTransform boardTransform;
   CPolyList* boardOutlinePolyList = NULL;
   CDataList& boardDataList = m_boardBlock->getDataList();

   // board outline
   boardTransform.x      = m_boardFile->getInsertX();
   boardTransform.y      = m_boardFile->getInsertY();
   boardTransform.scale  = m_boardFile->getScale();
   boardTransform.mirror = m_boardFile->isMirrored();
   boardTransform.SetRotation(m_boardFile->getRotation());

   // board data loop
   for (POSITION boardDataPos = boardDataList.GetHeadPosition();boardDataPos != NULL;)
   {
      DataStruct* boardDataStruct = boardDataList.GetNext(boardDataPos);

      if (boardDataStruct->getDataType() == dataTypePoly)
      {
         if (boardDataStruct->getGraphicClass() == GR_CLASS_BOARDOUTLINE)
         {
            boardOutlinePolyList = boardDataStruct->getPolyList();
            break;
         }
      }
   }

   m_camCadDatabase.addAttribute(getAttributeMap(m_boardBlock),
      m_kwDftBoardOutlineOk,((boardOutlinePolyList != NULL) ? "y" : "n"),errorLog);

   m_dftIndicators.addBoardOutlineIndicator(m_boardBlock,*boardOutlinePolyList);

   // package loop
   for (POSITION boardDataPos = boardDataList.GetHeadPosition();boardDataPos != NULL;)
   {
      DataStruct* componentData = boardDataList.GetNext(boardDataPos);

      if (componentData != NULL && componentData->getDataType() == dataTypeInsert)
      {
         InsertStruct* packageInsert = componentData->getInsert();

         if (packageInsert->getInsertType() == INSERTTYPE_PCBCOMPONENT)
         {
            CString refDes      = packageInsert->getRefname();
            int packageBlockNum = packageInsert->getBlockNumber();
            BlockStruct* packageBlock = m_camCadDatabase.getBlock(packageBlockNum);
            CDataList& packageDataList = packageBlock->getDataList();
            CPolyList* componentOutlinePolyList = NULL;
            CPolyList* componentSilkScreenOutlinePolyList = NULL;
            CPolyList padMetalExtentsExtentPoly;
            CExtent padMetalExtentsExtent;
            int pinCount = 0;
            double xPinCentroid = 0.;
            double yPinCentroid = 0.;

            CTMatrix packageMatrix;

            packageMatrix.translateCtm(packageInsert->getOriginX(),packageInsert->getOriginY());
            packageMatrix.rotateRadiansCtm(packageInsert->getAngle());
            packageMatrix.scaleCtm(((packageInsert->getMirrorFlags() & MIRROR_FLIP) != 0) ? -1. : 1.,1.);

            CTMatrix invertedPackageMatrix(packageMatrix);
            invertedPackageMatrix.invert();

            // package data loop
            for (POSITION packageDataPos = packageDataList.GetHeadPosition();packageDataPos != NULL;)
            {
               DataStruct* packageDataStruct = packageDataList.GetNext(packageDataPos);

               if (packageDataStruct != NULL)
               {
                  LayerStruct* packageDataLayer = m_camCadDatabase.getLayerAt(packageDataStruct->getLayerIndex());

                  bool silkScreenLayerFlag = (packageDataLayer != NULL && 
                     ( ( packageDataLayer->getLayerType() == LAYTYPE_SILK_TOP   ) ||
                       ( packageDataLayer->getLayerType() == LAYTYPE_SILK_BOTTOM)   ));  

                  if (packageDataStruct->getDataType() == dataTypeInsert)
                  {
                     InsertStruct* padstackInsert = packageDataStruct->getInsert();

                     CTMatrix padStackMatrix(packageMatrix);

                     padStackMatrix.translateCtm(padstackInsert->getOriginX(),padstackInsert->getOriginY());
                     padStackMatrix.rotateRadiansCtm(padstackInsert->getAngle());
                     padStackMatrix.scaleCtm(((padstackInsert->getMirrorFlags() & MIRROR_FLIP) != 0) ? -1. : 1.,1.);

                     // padstack
                     if (padstackInsert->getInsertType() == INSERTTYPE_PIN)
                     {
                        CString pinName      = padstackInsert->getRefname();

                        CompPinStruct* compPin = m_camCadDatabase.getDefinedPin(m_boardFile,refDes,pinName,NET_UNUSED_PINS);

                        CPoint2d padstackOrigin = calculatePadStackAttributes(padstackInsert,
                           getAttributeMap(compPin),packageMatrix,
                           boardOutlinePolyList,padMetalExtentsExtent,m_dftIndicators,errorLog);

                        compPin->setOrigin(padstackOrigin);
                     }
                  }
                  else if (packageDataStruct->getDataType() == dataTypePoly)
                  {
                     if (packageDataStruct->getGraphicClass() == GR_CLASS_COMPOUTLINE)
                     {
                        componentOutlinePolyList = packageDataStruct->getPolyList();
                     }
                     else if (silkScreenLayerFlag)
                     {
                        componentSilkScreenOutlinePolyList = packageDataStruct->getPolyList();
                     }
                  }
               }
            }

            CString outlineSource = "None";

            if (componentOutlinePolyList != NULL)
            {
               outlineSource = "ComponentOutline";
            }
            else if (componentSilkScreenOutlinePolyList != NULL)
            {
               outlineSource = "SilkScreen";
               componentOutlinePolyList = componentSilkScreenOutlinePolyList;
            }
            else if (padMetalExtentsExtent.isValid())
            {
               outlineSource = "PinMetalExtentsExtent";

               CPoly* metalExtentsExtentPoly = new CPoly();
               metalExtentsExtentPoly->addVertex(padMetalExtentsExtent.getLL());
               metalExtentsExtentPoly->addVertex(padMetalExtentsExtent.getLR());
               metalExtentsExtentPoly->addVertex(padMetalExtentsExtent.getUR());
               metalExtentsExtentPoly->addVertex(padMetalExtentsExtent.getUL());
               metalExtentsExtentPoly->addVertex(padMetalExtentsExtent.getLL());

               padMetalExtentsExtentPoly.empty();
               padMetalExtentsExtentPoly.AddTail(metalExtentsExtentPoly);
               padMetalExtentsExtentPoly.transform(invertedPackageMatrix);

               componentOutlinePolyList = &padMetalExtentsExtentPoly;
            }

            m_camCadDatabase.addAttribute(getAttributeMap(componentData),
               m_kwDftComponentOutlineOk,((componentOutlinePolyList != NULL) ? "y" : "n"),errorLog);

            m_camCadDatabase.addAttribute(getAttributeMap(componentData),
               m_kwDftComponentOutlineSource,outlineSource,errorLog);

            if (componentOutlinePolyList != NULL)
            {
               CQfeComponentOutline* componentOutline = 
                  new CQfeComponentOutline(*componentData,*componentOutlinePolyList,packageMatrix);

               if (packageInsert->getPlacedBottom())
               {
                  m_bottomComponentOutlineTree.setAt(componentOutline);

                  m_dftIndicators.addBottomComponentOutlineIndicator(m_boardBlock,*(componentOutline->getOutlinePolyList()));
               }
               else
               {
                  m_topComponentOutlineTree.setAt(componentOutline);

                  m_dftIndicators.addTopComponentOutlineIndicator(m_boardBlock,*(componentOutline->getOutlinePolyList()));
               }
            }

            if (pinCount > 0)
            {
               xPinCentroid /= pinCount;
               yPinCentroid /= pinCount;
            }

            m_camCadDatabase.addAttribute(getAttributeMap(componentData),
               m_kwDftComponentPinCentroidX,xPinCentroid,errorLog);

            m_camCadDatabase.addAttribute(getAttributeMap(componentData),
               m_kwDftComponentPinCentroidY,yPinCentroid,errorLog);
         }
      }
   }

   errorLog.writef(PrefixStatus,"m_topComponentOutlineTree ---------------------------------\n");
   m_topComponentOutlineTree.printMetricsReport(errorLog);

   errorLog.writef(PrefixStatus,"m_bottomComponentOutlineTree ---------------------------------\n");
   m_bottomComponentOutlineTree.printMetricsReport(errorLog);

   errorLog.flush();

   // calculate pin/via to component edge
   CQfeComponentOutlineList foundList,topFoundList,bottomFoundList;

   // pins
   for (POSITION netPos = m_boardFile->getNetList().GetHeadPosition();netPos != NULL;)
   {
      NetStruct* netStruct = m_boardFile->getNetList().GetNext(netPos);

      for (POSITION compPinPos = netStruct->getHeadCompPinPosition();compPinPos != NULL;)
      {
         CompPinStruct* compPinStruct = netStruct->getNextCompPin(compPinPos);
         CPoint2d pinOrigin(compPinStruct->getOrigin());

         calculateProbeToComponentAttributes(compPinStruct->getOrigin(),   
            getAttributeMap(compPinStruct),errorLog);
      }
   }

   // vias
   CTMatrix boardMatrix;
   CExtent padMetalExtentsExtent;

   for (POSITION boardDataPos = boardDataList.GetHeadPosition();boardDataPos != NULL;)
   {
      DataStruct* viaData = boardDataList.GetNext(boardDataPos);

      if (viaData != NULL && viaData->getDataType() == dataTypeInsert)
      {
         InsertStruct* padstackInsert = viaData->getInsert();

         if (padstackInsert->getInsertType() == INSERTTYPE_VIA)
         {
            calculatePadStackAttributes(padstackInsert,
               getAttributeMap(viaData),boardMatrix,
               boardOutlinePolyList,padMetalExtentsExtent,m_dftIndicators,errorLog);

            calculateProbeToComponentAttributes(CPoint2d(padstackInsert->getOriginX(),padstackInsert->getOriginY()),   
               getAttributeMap(viaData),errorLog);
         }
      }
   }

   m_dftIndicators.saveNamedView();

   return retval;
}

CPoint2d CDftAttributeCalculator::calculatePadStackAttributes(InsertStruct* padstackInsert,
   CAttributes** padstackParentAttributeMap,CTMatrix& padstackParentMatrix,
   CPolyList* boardOutlinePolyList,CExtent& padMetalExtentsExtent,
   CDftIndicators& dftIndicators,CWriteFormat& errorLog)
{
   CPoint2d padstackOrigin(padstackInsert->getOriginX(),padstackInsert->getOriginY());
   padstackParentMatrix.transform(padstackOrigin);

   CTMatrix padStackMatrix(padstackParentMatrix);

   padStackMatrix.translateCtm(padstackInsert->getOriginX(),padstackInsert->getOriginY());
   padStackMatrix.rotateRadiansCtm(padstackInsert->getAngle());
   padStackMatrix.scaleCtm(((padstackInsert->getMirrorFlags() & MIRROR_FLIP) != 0) ? -1. : 1.,1.);

   int padstackBlockNum = padstackInsert->getBlockNumber();
   BlockStruct* padstackBlock = m_camCadDatabase.getBlock(padstackBlockNum);
   CDataList& padstackDataList = padstackBlock->getDataList();
   double finishedHoleSize = 0.;
   DataStruct* topPad        = NULL;
   DataStruct* bottomPad     = NULL;
   DataStruct* topMaskPad    = NULL;
   DataStruct* bottomMaskPad = NULL;
   CPolyList* topPadPolyList        = NULL;
   CPolyList* bottomPadPolyList     = NULL;
   CPolyList* topMaskPadPolyList    = NULL;
   CPolyList* bottomMaskPadPolyList = NULL;

   // pad loop
   for (POSITION padstackDataPos = padstackDataList.GetHeadPosition();padstackDataPos != NULL;)
   {
      DataStruct* padDataStruct = padstackDataList.GetNext(padstackDataPos);

      if (padDataStruct != NULL && padDataStruct->getDataType() == dataTypeInsert)
      {
         InsertStruct* padInsert = padDataStruct->getInsert();
         CTMatrix padMatrix(padStackMatrix);

         padMatrix.translateCtm(padInsert->getOriginX(),padInsert->getOriginY());
         padMatrix.rotateRadiansCtm(padInsert->getAngle());
         padMatrix.scaleCtm(((padInsert->getMirrorFlags() & MIRROR_FLIP) != 0) ? -1. : 1.,1.);

         CBasesVector basesVector;
         basesVector.transform(padMatrix);
         bool mirroredFlag = basesVector.getMirror();

         int padBlockNum = padInsert->getBlockNumber();
         BlockStruct* padBlock = m_camCadDatabase.getBlock(padBlockNum);

         if (padBlock->isDrillHole())
         {
            finishedHoleSize = padBlock->getToolSize();
         }
         else if (padBlock->isAperture())
         {
            LayerStruct* layer = m_camCadDatabase.getLayerAt(padDataStruct->getLayerIndex());

            if (mirroredFlag && layer->getMirroredLayerIndex() >= 0)
            {
               layer = m_camCadDatabase.getLayerAt(layer->getMirroredLayerIndex());
            }

            bool suppressFlag = ((layer->getNeverMirror() &&  mirroredFlag) ||
                                 (layer->getMirrorOnly()  && !mirroredFlag)     );           

            if (!suppressFlag)
            {
               switch (layer->getLayerType())
               {
               case LAYTYPE_SIGNAL_TOP:
               case LAYTYPE_PAD_TOP:
                  topPad            = padDataStruct;
                  topPadPolyList    = getPadPolyList(*padBlock,padMatrix);
                  
                  break;
               case LAYTYPE_SIGNAL_BOT:
               case LAYTYPE_PAD_BOTTOM:
                  bottomPad         = padDataStruct;
                  bottomPadPolyList = getPadPolyList(*padBlock,padMatrix);
                  
                  break;
               case LAYTYPE_PAD_ALL:
                  topPad            = padDataStruct;
                  topPadPolyList    = getPadPolyList(*padBlock,padMatrix);
                  bottomPad         = padDataStruct;
                  bottomPadPolyList = getPadPolyList(*padBlock,padMatrix);
                  
                  break;
               case LAYTYPE_MASK_TOP:
                  topMaskPad         = padDataStruct;
                  topMaskPadPolyList = getPadPolyList(*padBlock,padMatrix);
                  
                  break;
               case LAYTYPE_MASK_BOTTOM:
                  bottomMaskPad         = padDataStruct;
                  bottomMaskPadPolyList = getPadPolyList(*padBlock,padMatrix);
                  
                  break;
               }
            }
         }
      }
   }

   if (boardOutlinePolyList != NULL)
   {
      CPoint2d closestPoint;

      double distance = boardOutlinePolyList->distanceTo(padstackOrigin,&closestPoint);

      m_dftIndicators.addBoardArrowIndicator(m_boardBlock,padstackOrigin,closestPoint);

      m_camCadDatabase.addAttribute(padstackParentAttributeMap,
         m_kwDftPinToBoardDistance,distance,errorLog);
   }

   CString accessValue;

   if (topPad == NULL)
   {
      accessValue = ((bottomPad == NULL) ? "n" : "b");
   }
   else
   {
      accessValue = ((bottomPad == NULL) ? "t" : "x");
   }

   // pin access attribute
   m_camCadDatabase.addAttribute(padstackParentAttributeMap,
      m_kwDftPinAccess,accessValue,errorLog);

   // extent attributes
   CExtent topPinMetalExtent,bottomPinMetalExtent;

   bool topMetalFlag    = calculatePinMetalExtent(topPinMetalExtent   ,topPadPolyList   ,topMaskPadPolyList   );
   bool bottomMetalFlag = calculatePinMetalExtent(bottomPinMetalExtent,bottomPadPolyList,bottomMaskPadPolyList);

   if (topMetalFlag && bottomMetalFlag)
   {
      if (fpeq(topPinMetalExtent.getXsize(),bottomPinMetalExtent.getXsize()) &&
            fpeq(topPinMetalExtent.getYsize(),bottomPinMetalExtent.getYsize())  )
      {
         if (fpeq(topPinMetalExtent.getXsize()   ,topPinMetalExtent.getYsize()))
         {
            m_camCadDatabase.addAttribute(padstackParentAttributeMap,
               m_kwDftPinMetalExtent,topPinMetalExtent.getXsize(),errorLog);
         }
         else
         {
            m_camCadDatabase.addAttribute(padstackParentAttributeMap,
               m_kwDftPinMetalExtentX,topPinMetalExtent.getXsize(),errorLog);

            m_camCadDatabase.addAttribute(padstackParentAttributeMap,
               m_kwDftPinMetalExtentY,topPinMetalExtent.getYsize(),errorLog);
         }
      }
      else
      {
         m_camCadDatabase.addAttribute(padstackParentAttributeMap,
            m_kwDftPinMetalExtentXT,topPinMetalExtent.getXsize(),errorLog);

         m_camCadDatabase.addAttribute(padstackParentAttributeMap,
            m_kwDftPinMetalExtentYT,topPinMetalExtent.getYsize(),errorLog);

         m_camCadDatabase.addAttribute(padstackParentAttributeMap,
            m_kwDftPinMetalExtentXB,bottomPinMetalExtent.getXsize(),errorLog);

         m_camCadDatabase.addAttribute(padstackParentAttributeMap,
            m_kwDftPinMetalExtentYB,bottomPinMetalExtent.getYsize(),errorLog);
      }
   }
   else if (topMetalFlag)
   {
      if (fpeq(topPinMetalExtent.getXsize()   ,topPinMetalExtent.getYsize()))
      {
         m_camCadDatabase.addAttribute(padstackParentAttributeMap,
            m_kwDftPinMetalExtent,topPinMetalExtent.getXsize(),errorLog);
      }
      else
      {
         m_camCadDatabase.addAttribute(padstackParentAttributeMap,
            m_kwDftPinMetalExtentX,topPinMetalExtent.getXsize(),errorLog);

         m_camCadDatabase.addAttribute(padstackParentAttributeMap,
            m_kwDftPinMetalExtentY,topPinMetalExtent.getYsize(),errorLog);
      }
   }
   else if (bottomMetalFlag)
   {
      if (fpeq(bottomPinMetalExtent.getXsize(),bottomPinMetalExtent.getYsize()))
      {
         m_camCadDatabase.addAttribute(padstackParentAttributeMap,
            m_kwDftPinMetalExtent,bottomPinMetalExtent.getXsize(),errorLog);
      }
      else
      {
         m_camCadDatabase.addAttribute(padstackParentAttributeMap,
            m_kwDftPinMetalExtentX,bottomPinMetalExtent.getXsize(),errorLog);

         m_camCadDatabase.addAttribute(padstackParentAttributeMap,
            m_kwDftPinMetalExtentY,bottomPinMetalExtent.getYsize(),errorLog);
      }
   }

   // offset attributes
   CPoint2d topPinMetalOffset,bottomPinMetalOffset;

   if (topMetalFlag)
   {
      topPinMetalOffset    = topPinMetalExtent.getCenter()    - padstackOrigin;

      if (fpeq(topPinMetalOffset.x   ,0.)) topPinMetalOffset.x = 0.;
      if (fpeq(topPinMetalOffset.y   ,0.)) topPinMetalOffset.y = 0.;

      m_dftIndicators.addTopPinExtentIndicator(m_boardBlock,padstackOrigin,topPinMetalExtent,topPinMetalOffset);

      padMetalExtentsExtent.update(topPinMetalExtent);
   }

   if (bottomMetalFlag)
   {
      bottomPinMetalOffset = bottomPinMetalExtent.getCenter() - padstackOrigin;

      if (fpeq(bottomPinMetalOffset.x,0.)) bottomPinMetalOffset.x = 0.;
      if (fpeq(bottomPinMetalOffset.y,0.)) bottomPinMetalOffset.y = 0.;  

      m_dftIndicators.addBottomPinExtentIndicator(m_boardBlock,padstackOrigin,bottomPinMetalExtent,bottomPinMetalOffset);

      padMetalExtentsExtent.update(bottomPinMetalExtent);
   }

   if (topMetalFlag && bottomMetalFlag)
   {
      if (fpeq(topPinMetalOffset.x         ,bottomPinMetalOffset.x         ) &&
            fpeq(topPinMetalOffset.y         ,bottomPinMetalOffset.y         )   )
      {
         if (!fpeq(topPinMetalOffset.x,0.) || !fpeq(topPinMetalOffset.y,0.))
         {
            m_camCadDatabase.addAttribute(padstackParentAttributeMap,
               m_kwDftPinMetalOffsetX,topPinMetalOffset.x,errorLog);

            m_camCadDatabase.addAttribute(padstackParentAttributeMap,
               m_kwDftPinMetalOffsetY,topPinMetalOffset.y,errorLog);
         }
      }
      else if (!fpeq(topPinMetalOffset.x   ,0.) || !fpeq(topPinMetalOffset.y   ,0.) ||
               !fpeq(bottomPinMetalOffset.x,0.) || !fpeq(bottomPinMetalOffset.y,0.)    )
      {
         m_camCadDatabase.addAttribute(padstackParentAttributeMap,
            m_kwDftPinMetalOffsetXT,topPinMetalOffset.x,errorLog);

         m_camCadDatabase.addAttribute(padstackParentAttributeMap,
            m_kwDftPinMetalOffsetYT,topPinMetalOffset.y,errorLog);

         m_camCadDatabase.addAttribute(padstackParentAttributeMap,
            m_kwDftPinMetalOffsetXB,bottomPinMetalOffset.x,errorLog);

         m_camCadDatabase.addAttribute(padstackParentAttributeMap,
            m_kwDftPinMetalOffsetYB,bottomPinMetalOffset.y,errorLog);
      }
   }
   else if (topMetalFlag)
   {
      if (!fpeq(topPinMetalOffset.x,0.) || !fpeq(topPinMetalOffset.y,0.))
      {
         m_camCadDatabase.addAttribute(padstackParentAttributeMap,
            m_kwDftPinMetalOffsetX,topPinMetalOffset.x,errorLog);

         m_camCadDatabase.addAttribute(padstackParentAttributeMap,
            m_kwDftPinMetalOffsetY,topPinMetalOffset.y,errorLog);
      }
   }
   else if (bottomMetalFlag)
   {
      if (!fpeq(bottomPinMetalOffset.x,0.) || !fpeq(bottomPinMetalOffset.y,0.))
      {
         m_camCadDatabase.addAttribute(padstackParentAttributeMap,
            m_kwDftPinMetalOffsetX,bottomPinMetalOffset.x,errorLog);

         m_camCadDatabase.addAttribute(padstackParentAttributeMap,
            m_kwDftPinMetalOffsetY,bottomPinMetalOffset.y,errorLog);
      }
   }

   return padstackOrigin;
}

void CDftAttributeCalculator::calculateProbeToComponentAttributes(const CPoint2d probeOrigin,   
   CAttributes** probeParentAttributeMap,CWriteFormat& errorLog)
{
   CQfeComponentOutlineList foundList,topFoundList,bottomFoundList;

   CExtent searchExtent(probeOrigin.x - getSearchTolerance(),probeOrigin.y - getSearchTolerance(),
                        probeOrigin.x + getSearchTolerance(),probeOrigin.y + getSearchTolerance());

   double topMinDistance    = DBL_MAX;
   double bottomMinDistance = DBL_MAX;
   CQfeComponentOutline* closestTopComponentOutline    = NULL;
   CQfeComponentOutline* closestBottomComponentOutline = NULL;
   CPoint2d topClosestPoint,bottomClosestPoint,closestPointResult;

   foundList.empty();
   m_topComponentOutlineTree.search(searchExtent,foundList);

   //errorLog.writef("\ntop foundList.GetCount()=%d, (%s.%s, (%.3f,%.3f))\n",
   //   foundList.GetCount(),(const char*)compPinStruct->comp,(const char*)compPinStruct->pin,
   //   compPinStruct->x,compPinStruct->y);
   //errorLog.flush();

   for (POSITION foundPos = foundList.GetHeadPosition();foundPos != NULL;)
   {
      CQfeComponentOutline* componentOutline = foundList.GetNext(foundPos);

      double distance = componentOutline->distanceTo(probeOrigin,m_camCadDatabase.getPageUnits(),&closestPointResult);

      //errorLog.writef("found componentOutline (%s, (%.3f,%.3f), distance=%.3f)\n",
      //   (const char*)componentOutline->getComponentData()->getInsert()->refname,
      //   componentOutline->getOrigin().x,componentOutline->getOrigin().y,
      //   distance);

      //componentOutline->getExtent().dump(errorLog);

      if (distance < topMinDistance)
      {
         topMinDistance = distance;
         closestTopComponentOutline = componentOutline;
         topClosestPoint = closestPointResult;

         if (topMinDistance <= 0.)
         {
            break;
         }
      }
   }

   foundList.empty();
   m_bottomComponentOutlineTree.search(searchExtent,foundList);

   //errorLog.writef("\nbottom foundList.GetCount()=%d, (%s.%s, (%.3f,%.3f))\n",
   //   foundList.GetCount(),(const char*)compPinStruct->comp,(const char*)compPinStruct->pin,
   //   compPinStruct->x,compPinStruct->y);
   //errorLog.flush();

   for (POSITION foundPos = foundList.GetHeadPosition();foundPos != NULL;)
   {
      CQfeComponentOutline* componentOutline = foundList.GetNext(foundPos);

      double distance = componentOutline->distanceTo(probeOrigin,m_camCadDatabase.getPageUnits(),&closestPointResult);

      //errorLog.writef("found componentOutline (%s, (%.3f,%.3f), distance=%.3f)\n",
      //   (const char*)componentOutline->getComponentData()->getInsert()->refname,
      //   componentOutline->getOrigin().x,componentOutline->getOrigin().y,
      //   distance);

      //componentOutline->getExtent().dump(errorLog);

      if (distance < bottomMinDistance)
      {
         bottomMinDistance = distance;
         closestBottomComponentOutline = componentOutline;
         bottomClosestPoint = closestPointResult;

         if (bottomMinDistance <= 0.)
         {
            break;
         }
      }
   }

   if (closestTopComponentOutline == NULL)
   {
      topMinDistance = getSearchTolerance();
   }
   else
   {
      m_camCadDatabase.addAttribute(probeParentAttributeMap,
         m_kwDftPinTopClosestComponent,closestTopComponentOutline->getComponentData()->getInsert()->getRefname(),errorLog);

      if (! fpeq(topMinDistance,0.))
      {
         m_dftIndicators.addTopPinArrowIndicator(m_boardBlock,probeOrigin,topClosestPoint);
      }
      else
      {
         m_dftIndicators.addTopPinCircleIndicator(m_boardBlock,probeOrigin,
            closestTopComponentOutline->getOrigin());
      }
   }

   m_camCadDatabase.addAttribute(probeParentAttributeMap,
      m_kwDftPinToTopComponentDistance,topMinDistance,errorLog);

   if (closestBottomComponentOutline == NULL)
   {
      bottomMinDistance = getSearchTolerance();
   }
   else
   {
      m_camCadDatabase.addAttribute(probeParentAttributeMap,
         m_kwDftPinBottomClosestComponent,closestBottomComponentOutline->getComponentData()->getInsert()->getRefname(),errorLog);

      if (! fpeq(bottomMinDistance,0.))
      {
         m_dftIndicators.addBottomPinArrowIndicator(m_boardBlock,probeOrigin,bottomClosestPoint);
      }
      else
      {
         m_dftIndicators.addBottomPinCircleIndicator(m_boardBlock,probeOrigin,
            closestBottomComponentOutline->getOrigin());
      }
   }

   m_camCadDatabase.addAttribute(probeParentAttributeMap,
      m_kwDftPinToBottomComponentDistance,bottomMinDistance,errorLog);
}

CPolyList* CDftAttributeCalculator::getPadPolyList(BlockStruct& padBlock,CTMatrix& padMatrix)
{
   CPolyList* padPolyList = padBlock.getAperturePolys(m_camCadDatabase.getCamCadData());

   if (padPolyList != NULL)
   {
      padPolyList->transform(padMatrix);
   }

   return padPolyList;
}

bool CDftAttributeCalculator::calculatePinMetalExtent(CExtent& pinMetalExtent,
   CPolyList* metalPadPolyList,CPolyList* maskPadPolyList)
{
   bool retval = true;

   if (metalPadPolyList != NULL)
   {
      pinMetalExtent     = metalPadPolyList->getExtent();

      if (maskPadPolyList != NULL)
      {
         CExtent maskExtent = maskPadPolyList->getExtent();

         retval = pinMetalExtent.intersect(maskExtent);
      }
   }
   else
   {
      retval = false;
   }

   return retval;
}

//_____________________________________________________________________________
CDftIndicators::CDftIndicators(CCamCadDatabase& camCadDatabase) :
   m_camCadDatabase(camCadDatabase)
{
}

void CDftIndicators::setGenerateIndicatorsFlag(bool flag)
{ 
   m_generateIndicatorsFlag = flag; 

   if (m_generateIndicatorsFlag)
   {
      m_boardIndicatorLayerIndex           = m_camCadDatabase.getDefinedLayerIndex("DftBoardIndicators");
      m_pinToBoardIndicatorLayerIndex      = m_camCadDatabase.getDefinedLayerIndex("DftPinToBoardIndicators");
      m_bottomPinIndicatorLayerIndex       = m_camCadDatabase.getDefinedLayerIndex("DftPinIndicatorsBottom");
      m_topPinIndicatorLayerIndex          = m_camCadDatabase.getDefinedLayerIndex("DftPinIndicatorsTop");
      m_bottomPinExtentIndicatorLayerIndex = m_camCadDatabase.getDefinedLayerIndex("DftPinExtentIndicatorsBottom");
      m_topPinExtentIndicatorLayerIndex    = m_camCadDatabase.getDefinedLayerIndex("DftPinExtentIndicatorsTop");

      m_boardIndicatorWidthIndex           = m_camCadDatabase.getDefinedWidthIndex(.000);
      m_pinToBoardIndicatorWidthIndex      = m_camCadDatabase.getDefinedWidthIndex(.000);
      m_bottomPinIndicatorWidthIndex       = m_camCadDatabase.getDefinedWidthIndex(.002);
      m_topPinIndicatorWidthIndex          = m_camCadDatabase.getDefinedWidthIndex(.001);
      m_bottomPinExtentIndicatorWidthIndex = m_camCadDatabase.getDefinedWidthIndex(.002);
      m_topPinExtentIndicatorWidthIndex    = m_camCadDatabase.getDefinedWidthIndex(.001);

      m_boardOutlineIndicatorWidthIndex          = m_camCadDatabase.getDefinedWidthIndex(.010);
      m_topComponenOutlineIndicatorWidthIndex    = m_camCadDatabase.getDefinedWidthIndex(.006);
      m_bottomComponenOutlineIndicatorWidthIndex = m_camCadDatabase.getDefinedWidthIndex(.008);
   }
   else
   {
      m_boardIndicatorLayerIndex                 = 0;
      m_pinToBoardIndicatorLayerIndex            = 0;
      m_bottomPinIndicatorLayerIndex             = 0;
      m_topPinIndicatorLayerIndex                = 0;
      m_bottomPinExtentIndicatorLayerIndex       = 0;
      m_topPinExtentIndicatorLayerIndex          = 0;

      m_boardIndicatorWidthIndex                 = 0;
      m_pinToBoardIndicatorWidthIndex            = 0;
      m_bottomPinIndicatorWidthIndex             = 0;
      m_topPinIndicatorWidthIndex                = 0;
      m_bottomPinExtentIndicatorWidthIndex       = 0;
      m_topPinExtentIndicatorWidthIndex          = 0;

      m_boardOutlineIndicatorWidthIndex          = 0;
      m_topComponenOutlineIndicatorWidthIndex    = 0;
      m_bottomComponenOutlineIndicatorWidthIndex = 0;
   }
} 

void CDftIndicators::saveNamedView()
{
   if (m_generateIndicatorsFlag)
   {
      CCEtoODBView* view = getActiveView();

      if (view != NULL && view->GetDocument() == &(m_camCadDatabase.getCamCadDoc()))
      {
         CNamedViewList& namedViewList = m_camCadDatabase.getCamCadDoc().getNamedViewList();

         CString preDftViewName("pre DftIndicators");
         namedViewList.deleteAt(preDftViewName);

         CString dftViewName("DftIndicators");
         namedViewList.deleteAt(dftViewName);

         CNamedView* namedViewPreDft = view->saveView(preDftViewName);
         CNamedView* namedView       = view->saveView(dftViewName);

         namedView->hideAll();

         namedView->setAt(m_topPinIndicatorLayerIndex         ,colorMagenta,true );
         namedView->setAt(m_bottomPinIndicatorLayerIndex      ,colorCyan   ,true );
         namedView->setAt(m_topPinExtentIndicatorLayerIndex   ,colorRed    ,true );
         namedView->setAt(m_bottomPinExtentIndicatorLayerIndex,colorBlue   ,true );
         namedView->setAt(m_pinToBoardIndicatorLayerIndex     ,colorGreen  ,false);
         namedView->setAt(m_boardIndicatorLayerIndex          ,colorYellow ,true );

         view->recallView(*namedView);
      }
   }
}

void CDftIndicators::addExtentIndicator(BlockStruct* boardBlock,
   int layerIndex,int widthIndex,const CPoint2d& padOrigin,const CExtent& padExtent,
   const CPoint2d& extentOffset)
{
   if (boardBlock != NULL && m_generateIndicatorsFlag)
   {
      DataStruct* indicatorData = m_camCadDatabase.addPolyStruct(boardBlock,layerIndex,
         0,false, graphicClassNormal);

      CPoly* indicatorPoly = m_camCadDatabase.addOpenPoly(indicatorData,widthIndex);

      m_camCadDatabase.addVertex(indicatorPoly,padExtent.getXmin(),padExtent.getYmin());
      m_camCadDatabase.addVertex(indicatorPoly,padExtent.getXmax(),padExtent.getYmin());
      m_camCadDatabase.addVertex(indicatorPoly,padExtent.getXmax(),padExtent.getYmax());
      m_camCadDatabase.addVertex(indicatorPoly,padExtent.getXmin(),padExtent.getYmax());
      m_camCadDatabase.addVertex(indicatorPoly,padExtent.getXmin(),padExtent.getYmin());

      if (! fpeq(extentOffset.x,0.) || ! fpeq(extentOffset.y,0.))
      {
         addArrowIndicator(boardBlock,layerIndex,widthIndex,padOrigin,padOrigin + extentOffset);
      }
   }
}

void CDftIndicators::addArrowIndicator(BlockStruct* boardBlock,
   int layerIndex,int widthIndex,const CPoint2d& fromPoint,const CPoint2d& toPoint)
{
   if (boardBlock != NULL && m_generateIndicatorsFlag)
   {
      DataStruct* indicatorData = m_camCadDatabase.addPolyStruct(boardBlock,layerIndex,
         0,false, graphicClassNormal);

      CPoly* indicatorPoly = m_camCadDatabase.addOpenPoly(indicatorData,widthIndex);

      double arrowSize = .010;
      double dx = toPoint.x - fromPoint.x;
      double dy = toPoint.y - fromPoint.y;
      double angle = atan2(dy,dx);
      CTMatrix matrix;
      matrix.translateCtm(toPoint);
      matrix.rotateRadiansCtm(angle);

      CPoint2d arrowPoint0(-arrowSize, arrowSize/4.);
      CPoint2d arrowPoint1(-arrowSize,-arrowSize/4.);

      matrix.transform(arrowPoint0);
      matrix.transform(arrowPoint1);

      m_camCadDatabase.addVertex(indicatorPoly,fromPoint.x  ,fromPoint.y);
      m_camCadDatabase.addVertex(indicatorPoly,toPoint.x    ,toPoint.y);
      m_camCadDatabase.addVertex(indicatorPoly,arrowPoint0.x,arrowPoint0.y);
      m_camCadDatabase.addVertex(indicatorPoly,arrowPoint1.x,arrowPoint1.y);
      m_camCadDatabase.addVertex(indicatorPoly,toPoint.x    ,toPoint.y);
   }
}

void CDftIndicators::addCircleIndicator(BlockStruct* boardBlock,
   int layerIndex,int widthIndex,const CPoint2d& fromPoint,const CPoint2d& toPoint)
{
   if (boardBlock != NULL && m_generateIndicatorsFlag)
   {
      DataStruct* indicatorData = m_camCadDatabase.addPolyStruct(boardBlock,layerIndex,
         0,false, graphicClassNormal);

      CPoly* indicatorPoly = m_camCadDatabase.addOpenPoly(indicatorData,widthIndex);

      double circleSize = .008;
      double dx = toPoint.x - fromPoint.x;
      double dy = toPoint.y - fromPoint.y;
      double angle = atan2(dy,dx);
      CTMatrix matrix;
      matrix.translateCtm(toPoint);
      matrix.rotateRadiansCtm(angle);

      CPoint2d circlePoint0(-circleSize,0.);
      CPoint2d circlePoint1( circleSize,0.);

      matrix.transform(circlePoint0);
      matrix.transform(circlePoint1);

      m_camCadDatabase.addVertex(indicatorPoly,fromPoint.x   ,fromPoint.y);
      m_camCadDatabase.addVertex(indicatorPoly,circlePoint0.x,circlePoint0.y,1.0);
      m_camCadDatabase.addVertex(indicatorPoly,circlePoint1.x,circlePoint1.y,1.0);
      m_camCadDatabase.addVertex(indicatorPoly,circlePoint0.x,circlePoint0.y);
   }
}

void CDftIndicators::addOutlineIndicator(BlockStruct* boardBlock,
   int layerIndex,int widthIndex,const CPolyList& polyList)
{
   if (boardBlock != NULL && m_generateIndicatorsFlag)
   {
      int widthIndex = m_camCadDatabase.getDefinedWidthIndex(.008);

      DataStruct* indicatorData = m_camCadDatabase.addPolyStruct(boardBlock,layerIndex,
         0,false, graphicClassNormal);

      delete indicatorData->getPolyList();

      indicatorData->getPolyList() = new CPolyList(polyList);
      indicatorData->getPolyList()->setWidthIndex(widthIndex);
   }
}

