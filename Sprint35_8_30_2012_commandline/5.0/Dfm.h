// $Header: /CAMCAD/4.5/Dfm.h 68    4/13/06 9:06a Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once

#define ATT_DFMCOMP_REFNAME   "COMP_REFNAME"

class CCEtoODBDoc;
class BlockStruct;
class DataStruct;
class FileStruct;
int GetDirectionOfTravel(CCEtoODBDoc *doc, BlockStruct *fileGeom);


class DFM : public CCmdTarget
{
   DECLARE_DYNCREATE(DFM)
private:
	void AnnularRingPadDrillCheck(CCEtoODBDoc *doc, FileStruct *file, CString padDescription, DataStruct *compData, DataStruct *pinData, DataStruct *padData, double x, double y, double drillSize, double minAllowable, LPCTSTR algName);
	void AnnularRingReportAbsentPad(CCEtoODBDoc *doc, FileStruct *file, CString padDescription, DataStruct *compData, DataStruct *pinData, DataStruct *padData, double x, double y, double drillSize, double minAllowable, LPCTSTR algName);

public:
   DFM();
   virtual ~DFM();

   virtual void OnFinalRelease();

protected:
   DECLARE_MESSAGE_MAP()
   DECLARE_OLECREATE(DFM)
   DECLARE_DISPATCH_MAP()
   DECLARE_INTERFACE_MAP()
   void OnMaxErrorsPerTestChanged(void);
   SHORT m_maxErrorsPerTest;

   enum 
   {
		dispidCombinePolies = 37L,
      dispidComponentHeight = 36L,
      dispidComponentNotAllowedPerSurface = 35L,
      dispidTestPadDensityCheck = 34L,
      dispidPin1Orientation = 33L,
      dispidGerberThermalBarrelPlugCheck = 32L,
      dispidDistanceCheck2 = 31L,
      dispidAnnularRing = 30L,
      dispidGenerateEtchBlockers = 29L,
      dispidBoardThicknessToDrillDiameter = 28L,
      dispidAllowableComponentOrientation = 27L,
      dispidRouteTraceWidthMinimum = 26L,
      dispidThruHolePadShapes = 25L,
      dispidExposedTestpointPadSize = 24L,
      dispidViaHoleSize = 23L,
      dispidViaPadSize = 22L,
      dispidViaUnderPart = 21L,
      dispidProcessSolderMask = 20L,
      dispidTestpointsPerNet = 19L,
      dispidComponentFlushWithOutline = 18L,
      dispidItemsInCorners = 17L,
      dispidSMDSoldermaskClearance = 16L,
      dispidClearanceToPanelEdge = 15L,
      dispidPanelLengthToWidthRatio = 14L,
      dispidPanelDimensions = 13L,
      dispidComponentShadow = 12L,
      dispidGenerateDFxOutlines = 11L,
      dispidDistanceCheckTwoNets = 10L,
      dispidDistanceCheckDiffNets = 9L,
      dispidDistanceCheck = 8L,
      dispidFilterGeometry = 7L,
      dispidFlattenByAttrib = 6L,
      dispidFlattenByInserttype = 5L,
      dispidFlattenByGraphicClass = 4L,
      dispidFlatten = 3L,
      dispidCreateFlattenGeometry = 2L,
      dispidmaxErrorsPerTest = 1
   };
   SHORT Flatten(SHORT geometryNumber, SHORT layerNumber, SHORT TopLevelOnly);
   SHORT CreateFlattenGeometry(LONG filePosition, SHORT* geometryNumber);
   SHORT FlattenByInserttype(SHORT geometryNumber, SHORT layerNumber, SHORT inserttype, SHORT InsertPointOnly, SHORT surface);
   SHORT DistanceCheck(SHORT geometryNumber1, SHORT geometryNumber2, DOUBLE marginalDistance, DOUBLE criticalDistance, LPCTSTR algName, SHORT algType, SHORT CanTouch, SHORT SkipSameNets, SHORT OneDRCPerNetPair);
   SHORT DistanceCheckDiffNets(SHORT geometryNumber, DOUBLE marginalDistance, DOUBLE criticalDistance, LPCTSTR algName, SHORT algType, SHORT CanTouch);
   SHORT DistanceCheckTwoNets(SHORT geometryNumber, LPCTSTR netname1, LPCTSTR netname2, DOUBLE marginalDistance, DOUBLE criticalDistance, LPCTSTR algName, SHORT algType, SHORT CanTouch);
   SHORT ComponentShadow(LONG filePosition, DOUBLE marLeftMargin, DOUBLE marRightMargin, DOUBLE marTopMargin, DOUBLE marBottomMargin, DOUBLE crtLeftMargin, DOUBLE crtRightMargin, DOUBLE crtTopMargin, DOUBLE crtBottomMargin);
   SHORT PanelDimensions(LONG filePosition, DOUBLE minWidth, DOUBLE maxWidth, DOUBLE minLength, DOUBLE maxLength, LPCTSTR algName);
   SHORT PanelLengthToWidthRatio(LONG filePosition, DOUBLE minRatio, LPCTSTR algName);
   SHORT ClearanceToPanelEdge(SHORT geometryNumber, DOUBLE minRail, DOUBLE minNonRail, LPCTSTR algName);
   SHORT FlattenByAttrib(SHORT geometryNumber, SHORT layerNumber, LPCTSTR attribKeyword, LPCTSTR attribValue, SHORT comparisonType, SHORT inserttype, SHORT InsertPointOnly, SHORT surface);
   SHORT ItemsInCorners(SHORT geometryNumber, DOUBLE minRail, DOUBLE maxRail, DOUBLE minNonRail, DOUBLE maxNonRail, SHORT minCorners, LPCTSTR algName, SHORT surface);
   SHORT ComponentFlushWithOutline(SHORT geometryNumber, DOUBLE maxDist, LPCTSTR algName);
   SHORT TestpointsPerNet(SHORT geometryNumber, SHORT minTestpoints, LPCTSTR algName);
   SHORT ProcessSolderMask(SHORT soldermaskGeometryNumber, SHORT copperGeometryNumber);
   SHORT ViaUnderPart(SHORT viaGeometryNumber, SHORT compGeometryNumber, LPCTSTR algName);
   SHORT ExposedTestpointPadSize(SHORT geometryNumber, DOUBLE minSize, DOUBLE maxSize, LPCTSTR algName);
   SHORT ViaPadSize(LONG filePosition, DOUBLE topMinSize, DOUBLE topMaxSize, DOUBLE bottomMinSize, DOUBLE bottomMaxSize, DOUBLE ignoreHoleSize, LPCTSTR algName);
   SHORT ViaHoleSize(LONG filePosition, DOUBLE minSize, DOUBLE maxSize, DOUBLE ignoreHoleSize, LPCTSTR algName);
   SHORT FilterGeometry(SHORT geometryNumber, LPCTSTR keyword, LPCTSTR value, SHORT comparisonType);
   SHORT ThruHolePadShapes(LONG filePosition, SHORT topAll, SHORT bottomAll, SHORT topOne, SHORT bottomOne, LPCTSTR algName);
   SHORT RouteTraceWidthMinimum(SHORT geometryNumber, DOUBLE marginal, DOUBLE critical, LPCTSTR excludeNets, LPCTSTR algName);
   SHORT AllowableComponentOrientation(LONG filePosition, SHORT top, SHORT horizontal, LPCTSTR algName);
   SHORT BoardThicknessToDrillDiameter(LONG filePosition, DOUBLE maxRatio, LPCTSTR algName);
   SHORT GenerateEtchBlockers(DOUBLE maxPinPitch, SHORT* topLayerNumber, SHORT* bottomLayerNumber);
   SHORT FlattenByGraphicClass(SHORT geometryNumber, SHORT layerNumber, SHORT graphicClass, SHORT TopLevelOnly);
   SHORT AnnularRing(LONG filePosition, DOUBLE padTop, DOUBLE padBottom, DOUBLE soldTop, DOUBLE soldBottom, LPCTSTR algName);
   SHORT SMDSoldermaskClearance(SHORT pinGeometryNumber, SHORT soldermaskGeometryNumber, DOUBLE minXMargin, DOUBLE minYMargin, LPCTSTR algName);
   SHORT GenerateDFxOutlines(void);
   SHORT DistanceCheck2(SHORT geometryNumber1, SHORT geometryNumber2, DOUBLE marginalDistance, DOUBLE criticalDistance, LPCTSTR algName, SHORT algType, SHORT CanTouch, SHORT SkipSameNets, SHORT OneDRCPerNetPair, SHORT SkipSameComponents);
   SHORT GerberThermalBarrelPlugCheck(LONG filePosition);
   SHORT Pin1Orientation(LONG filePosition, SHORT top, SHORT bottom, SHORT thruHole, SHORT smd, LONG ignoreCount);
   SHORT TestPadDensityCheck(LONG filePosition, SHORT marginal, SHORT critical, DOUBLE gridSize, SHORT addTPDensityAttrib);
   SHORT ComponentNotAllowedPerSurface(LONG filePosition, SHORT top, SHORT bottom, LPCTSTR attribKeyword, LPCTSTR attribValue);
   SHORT ComponentHeight(LONG filePosition, SHORT top, SHORT bottom, LPCTSTR componentHeightAttributeKeyword, DOUBLE topMarginalHeight, DOUBLE topCriticalHeight, DOUBLE bottomMarginalHeight, DOUBLE bottomCriticalHeight);
   SHORT CombinePolies(SHORT geometryNumber);
};
