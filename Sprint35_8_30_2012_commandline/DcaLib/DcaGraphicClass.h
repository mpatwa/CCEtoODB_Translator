// $Header: /CAMCAD/DcaLib/DcaGraphicClass.h 5     3/23/07 5:19a Kurt Van Ness $

#if !defined(__DcaGraphicClass_h__)
#define __DcaGraphicClass_h__

#pragma once

#include "Dca.h"
#include "DcaMask64.h"

//_____________________________________________________________________________
enum GraphicClassTag
{
   graphicClassNormal            = 0,    // GR_CLASS_NORMAL             0  // default init.
   graphicClassAnalysis          = 1,    // GR_CLASS_ANALYSIS           1
   graphicClassAntiEtch          = 2,    // GR_CLASS_ANTI_ETCH          2
   graphicClassBoardGeometry     = 3,    // GR_CLASS_BOARD_GEOM         3  // must be used to get a graphic element into the board outline section
   graphicClassDrawing           = 4,    // GR_CLASS_DRAWING            4
   graphicClassDrc               = 5,    // GR_CLASS_DRC                5
   graphicClassEtch              = 6,    // GR_CLASS_ETCH               6  // must be used to get a graphic element into a netlist
                                                                            // a line can have a netname, but not _ETCH. This means a line is a
                                                                            // copper line with a netname, but not part of a route (i.e. PADS, MENTOR)
   graphicClassManufacturing     = 7,    // GR_CLASS_MANUFACTURING      7
   graphicClassRouteKeepIn       = 8,    // GR_CLASS_ROUTKEEPIN         8
   graphicClassRouteKeepOut      = 9,    // GR_CLASS_ROUTKEEPOUT        9
   graphicClassPlaceKeepIn       = 10,   // GR_CLASS_PLACEKEEPIN        10
   graphicClassPlaceKeepOut      = 11,   // GR_CLASS_PLACEKEEPOUT       11 //
   graphicClassViaKeepIn         = 12,   // GR_CLASS_VIAKEEPIN          12
   graphicClassViaKeepOut        = 13,   // GR_CLASS_VIAKEEPOUT         13
   graphicClassAllKeepIn         = 14,   // GR_CLASS_ALLKEEPIN          14
   graphicClassAllKeepOut        = 15,   // GR_CLASS_ALLKEEPOUT         15
   graphicClassBoardOutline      = 16,   // GR_CLASS_BOARDOUTLINE       16 // this is a primary, good boardoutline
   graphicClassComponentOutline  = 17,   // GR_CLASS_COMPOUTLINE        17 // this is a primary good component silkscreen outline
   graphicClassPanelOutline      = 18,   // GR_CLASS_PANELOUTLINE       18 // this is a primary good paneloutline
   graphicClassMillingPath       = 19,   // GR_CLASS_MILLINGPATH        19 // for excellon milling output
   graphicClassUnroute           = 20,   // GR_CLASS_UNROUTE            20 // for rubberbands, open connections.
   graphicClassTraceFanout       = 21,   // GR_CLASS_TRACEFANOUT        21 // this are graphic elements, used for SMD fanout
   graphicClassPanelCutout       = 22,   // GR_CLASS_PANELCUTOUT        22 // this are graphic elements
   graphicClassBoardCutout       = 23,   // GR_CLASS_BOARDCUTOUT        23 // this are graphic elements
   graphicClassTestPointKeepOut  = 24,   // GR_CLASS_TESTPOINTKEEPOUT   24
   graphicClassTestPointKeepIn   = 25,   // GR_CLASS_TESTPOINTKEEPIN    25
   graphicClassComponentBoundary = 26,   // GR_CLASS_COMPONENT_BOUNDARY 26 // this must be a closed polyline.
   graphicClassPlacementRegion   = 27,   // GR_CLASS_PLACEMENT_REGION   27 // this must be a closed polyline.
   graphicClassPackageOutline    = 28,   // GR_CLASS_PACKAGEOUTLINE     28 // this is the outline, which DFT uses
   graphicClassPackageBody       = 29,   // GR_CLASS_PACKAGEBODY        29 // Added 07/25/03, Used for new package information from RealPart
   graphicClassPackagePin        = 30,   // GR_CLASS_PACKAGEPIN         30 // Added 07/25/03, Used for new package information from RealPart
   graphicClassSignal            = 31,   // GR_CLASS_SIGNAL             31 // Added 07/19/04, used for Schematic file
   graphicClassBus               = 32,   // GR_CLASS_BUS                32 // Added 07/19/04, used for Schematic file
   graphicClassPanelBorder       = 33,   // GR_CLASS_PANELBORDER        33  // Part of fix for dts0100505067
   graphicClassManufacturingOutl = 34,   // GR_CLASS_MANUFACTURINGOUTL  34 // Part of fix for dts0100505065
   graphicClassContour           = 35,   // GR_CLASS_CONTOUR            35  // dts0100457517
   graphicClassPlaneKeepout      = 36,   // GR_CLASS_PLANEKEEPOUT       36  dts0100552575
   graphicClassPlaneNoConnect    = 37,   // GR_CLASS_PLANENOCONNECT     37  dts0100552575
   graphicClassBoardKeepout      = 38,   // GR_CLASS_BOARDKEEPOUT       38  dts0100552575
   graphicClassPadKeepout        = 39,   // GR_CLASS_PADKEEPOUT         39  dts0100552575
   graphicClassEPBody            = 40,   // GR_CLASS_EPBODY             40  dts0100552575
   graphicClassEPMask            = 41,   // GR_CLASS_EPMASK             41  dts0100552575
   graphicClassEPOverGlaze       = 42,   // GR_CLASS_EPOVERGLAZE        42  dts0100552575
   graphicClassEPDielectric      = 43,   // GR_CLASS_EPDIELECTRIC       dts0100559099
   graphicClassEPConductive      = 44,   // GR_CLASS_EPCONDUCTIVE       dts0100559099
   graphicClassBusPath           = 45,   // GR_CLASS_BUSPATH            45
   graphicClassRegion            = 46,   // GR_CLASS_REGION             46
   graphicClassCavity            = 47,   // GR_CLASS_CAVITY             47

   graphicClassLowerBound        = 0                           ,
   graphicClassUpperBound        = 47                          ,
   graphicClassUndefined         = -1                          ,
   GraphicClassTagMin            = graphicClassLowerBound      ,
   GraphicClassTagMax            = graphicClassUpperBound
};

CString graphicClassToString(GraphicClassTag graphicClass);
CString graphicClassTagToDisplayString(GraphicClassTag graphicClass);
GraphicClassTag displayStringToGraphicClassTag(const CString& displayString);
GraphicClassTag intToGraphicClassTag(int candidateGrClass);

//_____________________________________________________________________________
class CGraphicsClassFilter : public CMask64
{
public:
   void setToDefault();

   CString getSetString(const CString& delimeter=CString(" "));
   void setFromString(const CString& setString,const CString& delimiterList=CString(" "));
};

#endif
