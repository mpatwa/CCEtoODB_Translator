// $Header: /CAMCAD/4.6/Update.cpp 25    7/12/06 7:01p Kurt Van Ness $
        
/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/           

#include "stdafx.h"
#include "ccdoc.h"
#include "ccview.h"
#include "lic.h"

// View
void CCEtoODBView::OnUpdateJoinSnap(CCmdUI* pCmdUI) 
{ pCmdUI->SetRadio(cursorMode == Join_Snap); }

void CCEtoODBView::OnUpdateJoinExtend(CCmdUI* pCmdUI) 
{ pCmdUI->SetRadio(cursorMode == Join_Extend); }

void CCEtoODBView::OnUpdateJoinChamfer(CCmdUI* pCmdUI) 
{ pCmdUI->SetRadio(cursorMode == Join_Chamfer); }

void CCEtoODBView::OnUpdateJoinFillet(CCmdUI* pCmdUI) 
{ pCmdUI->SetRadio(cursorMode == Join_Fillet); }

// Doc
void CCEtoODBDoc::OnUpdateDoneEditing(CCmdUI* pCmdUI) 
{ pCmdUI->Enable(GeometryEditing); }

void CCEtoODBDoc::OnUpdateShowPinnrTop(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(showPinnrsTop); }

void CCEtoODBDoc::OnUpdateShowPinnrBottom(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(showPinnrsBottom); }

void CCEtoODBDoc::OnUpdateShowPinnetsTop(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(showPinnetsTop); }

void CCEtoODBDoc::OnUpdateShowPinnetsBottom(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(showPinnetsBottom); }

void CCEtoODBDoc::OnUpdateTopView(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(!getBottomView()); }

void CCEtoODBDoc::OnUpdateBottomView(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getBottomView()); }

void CCEtoODBDoc::OnUpdatePanelView(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(PanelBoardView == 1); }

void CCEtoODBDoc::OnUpdateBoardView(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(PanelBoardView == 2); }

void CCEtoODBDoc::OnUpdateNegativeView(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(NegativeView); }

void CCEtoODBDoc::OnUpdateShowGrid(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getSettings().Grid); }

void CCEtoODBDoc::OnUpdateShowOrigin(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getSettings().Crosshairs); }

void CCEtoODBDoc::OnUpdateSnapToGrid(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getSettings().Snap); }

void CCEtoODBDoc::OnUpdateShowSequenceNumbers(CCmdUI* pCmdUI) 
{ pCmdUI->SetRadio(showSeqNums); }

void CCEtoODBDoc::OnUpdateShowSequenceArrows(CCmdUI* pCmdUI) 
{ pCmdUI->SetRadio(showSeqArrows); }

void CCEtoODBDoc::OnUpdateShowTestOffsetsTop(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(showTAOffsetsTop); }

void CCEtoODBDoc::OnUpdateShowTestOffsetsBottom(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(showTAOffsetsBottom); }

void CCEtoODBDoc::OnUpdateFillSolid(CCmdUI* pCmdUI) 
{ pCmdUI->SetRadio(getSettings().FillStyle == HS_SOLID); }

void CCEtoODBDoc::OnUpdateFillHorizontal(CCmdUI* pCmdUI) 
{ pCmdUI->SetRadio(getSettings().FillStyle == HS_HORIZONTAL); }

void CCEtoODBDoc::OnUpdateFillVertical(CCmdUI* pCmdUI) 
{ pCmdUI->SetRadio(getSettings().FillStyle == HS_VERTICAL); }

void CCEtoODBDoc::OnUpdateFillCross(CCmdUI* pCmdUI) 
{ pCmdUI->SetRadio(getSettings().FillStyle == HS_CROSS); }

void CCEtoODBDoc::OnUpdateFillDiagonal1(CCmdUI* pCmdUI) 
{ pCmdUI->SetRadio(getSettings().FillStyle == HS_FDIAGONAL); }

void CCEtoODBDoc::OnUpdateFillDiagonal2(CCmdUI* pCmdUI) 
{ pCmdUI->SetRadio(getSettings().FillStyle == HS_BDIAGONAL); }

void CCEtoODBDoc::OnUpdateFillDiagonalCross(CCmdUI* pCmdUI) 
{ pCmdUI->SetRadio(getSettings().FillStyle == HS_DIAGCROSS); }

void CCEtoODBDoc::OnUpdateFilter(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getSettings().FilterOn); }

void CCEtoODBDoc::OnUpdateFilterEndpoint(CCmdUI* pCmdUI) 
{ pCmdUI->SetRadio(getSettings().FilterMode == Filter_Endpoint); }

void CCEtoODBDoc::OnUpdateFilterEntity(CCmdUI* pCmdUI) 
{ pCmdUI->SetRadio(getSettings().FilterMode == Filter_Entity); }

void CCEtoODBDoc::OnUpdateFilterCenter(CCmdUI* pCmdUI) 
{ pCmdUI->SetRadio(getSettings().FilterMode == Filter_Center); }

void CCEtoODBDoc::OnUpdateFilterInserts(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getSettings().FilterInserts); }

void CCEtoODBDoc::OnUpdateFilterLines(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getSettings().FilterLines); }

void CCEtoODBDoc::OnUpdateFilterPoints(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getSettings().FilterPoints); }

void CCEtoODBDoc::OnUpdateFiltertypesAll(CCmdUI* pCmdUI) 
{ pCmdUI->SetRadio(getSettings().FilterTypes == Filter_All); }

void CCEtoODBDoc::OnUpdateFiltertypesPins(CCmdUI* pCmdUI) 
{ pCmdUI->SetRadio(getSettings().FilterTypes == Filter_Pins); }

// Graphic Classes
void CCEtoODBDoc::OnUpdateShowFills(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getShowFills()); }

void CCEtoODBDoc::OnUpdateShowApFills(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getShowApFills()); }

void CCEtoODBDoc::OnUpdateShowAttribs(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getShowAttribs()); }

void CCEtoODBDoc::OnUpdateShowNormal(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassNormal)); }

void CCEtoODBDoc::OnUpdateShowAllkeepin(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(GR_CLASS_ALLKEEPIN)); }

void CCEtoODBDoc::OnUpdateShowAllkeepout(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(GR_CLASS_ALLKEEPOUT)); }

void CCEtoODBDoc::OnUpdateShowAnalysis(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassAnalysis)); }

void CCEtoODBDoc::OnUpdateShowAntiEtch(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(GR_CLASS_ANTI_ETCH)); }

void CCEtoODBDoc::OnUpdateShowBoardCutout(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassBoardCutout)); }

void CCEtoODBDoc::OnUpdateShowBoardGeom(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(GR_CLASS_BOARD_GEOM)); }

void CCEtoODBDoc::OnUpdateShowBoardoutline(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(GR_CLASS_BOARDOUTLINE)); }

void CCEtoODBDoc::OnUpdateShowComponentBoundary(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(GR_CLASS_COMPONENT_BOUNDARY)); }

void CCEtoODBDoc::OnUpdateShowCompoutline(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(GR_CLASS_COMPOUTLINE)); }

void CCEtoODBDoc::OnUpdateShowPackageOutline(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(GR_CLASS_PACKAGEOUTLINE)); }

void CCEtoODBDoc::OnUpdateShowDrawing(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(GR_CLASS_DRAWING)); }

void CCEtoODBDoc::OnUpdateShowDrc(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassDrc)); }

void CCEtoODBDoc::OnUpdateShowEtch(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassEtch)); }

void CCEtoODBDoc::OnUpdateShowManufacturing(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassManufacturing)); }

void CCEtoODBDoc::OnUpdateShowMilling(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(GR_CLASS_MILLINGPATH)); }

void CCEtoODBDoc::OnUpdateShowPaneloutline(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(GR_CLASS_PANELOUTLINE)); }

void CCEtoODBDoc::OnUpdateShowPanelCutout(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassPanelCutout)); }

void CCEtoODBDoc::OnUpdateShowPlacekeepin(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassPlaceKeepIn)); }

void CCEtoODBDoc::OnUpdateShowPlacekeepout(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassPlaceKeepOut)); }

void CCEtoODBDoc::OnUpdateShowPlacementRegion(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassPlacementRegion)); }

void CCEtoODBDoc::OnUpdateShowRoutkeepin(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassRouteKeepIn)); }

void CCEtoODBDoc::OnUpdateShowRoutkeepout(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassRouteKeepOut)); }

void CCEtoODBDoc::OnUpdateShowTestpointKeepin(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassTestPointKeepIn)); }

void CCEtoODBDoc::OnUpdateShowTestpointKeepout(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassTestPointKeepOut)); }

void CCEtoODBDoc::OnUpdateShowTraceFanout(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(GR_CLASS_TRACEFANOUT)); }

void CCEtoODBDoc::OnUpdateShowViakeepin(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(GR_CLASS_VIAKEEPIN)); }

void CCEtoODBDoc::OnUpdateShowViakeepout(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(GR_CLASS_VIAKEEPOUT)); }

void CCEtoODBDoc::OnUpdateShowUnroute(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(GR_CLASS_UNROUTE)); }

void CCEtoODBDoc::OnUpdateShowPackageBody(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(GR_CLASS_PACKAGEBODY)); }

void CCEtoODBDoc::OnUpdateShowPackagePin(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(GR_CLASS_PACKAGEPIN)); }

void CCEtoODBDoc::OnUpdateShowSignal(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassSignal)); }

void CCEtoODBDoc::OnUpdateShowBus(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassBus)); }

void CCEtoODBDoc::OnUpdateShowPanelBorder(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassPanelBorder)); }

void CCEtoODBDoc::OnUpdateShowManufacturingOutl(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassManufacturingOutl)); }

void CCEtoODBDoc::OnUpdateShowContour(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassContour)); }

void CCEtoODBDoc::OnUpdateShowPlaneKeepout(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassPlaneKeepout)); }

void CCEtoODBDoc::OnUpdateShowPlaneNoConnect(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassPlaneNoConnect)); }

void CCEtoODBDoc::OnUpdateShowBoardKeepout(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassBoardKeepout)); }

void CCEtoODBDoc::OnUpdateShowPadKeepout(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassPadKeepout)); }

void CCEtoODBDoc::OnUpdateShowEPBody(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassEPBody)); }

void CCEtoODBDoc::OnUpdateShowEPMask(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassEPMask)); }

void CCEtoODBDoc::OnUpdateShowEPOverGlaze(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassEPOverGlaze)); }

void CCEtoODBDoc::OnUpdateShowEPDielectric(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassEPDielectric)); }

void CCEtoODBDoc::OnUpdateShowEPConductive(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassEPConductive)); }

void CCEtoODBDoc::OnUpdateShowBusPath(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassBusPath)); }

void CCEtoODBDoc::OnUpdateShowRegion(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassRegion)); }

void CCEtoODBDoc::OnUpdateShowCavity(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getGraphicsClassFilter().contains(graphicClassCavity)); }


// Insert Types
void CCEtoODBDoc::OnUpdateItUnknown(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_UNKNOWN)); }

void CCEtoODBDoc::OnUpdateItVia(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_VIA)); }

void CCEtoODBDoc::OnUpdateItPin(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_PIN)); }

void CCEtoODBDoc::OnUpdateItPcbcomp(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_PCBCOMPONENT)); }

void CCEtoODBDoc::OnUpdateItMechcomp(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_MECHCOMPONENT)); }

void CCEtoODBDoc::OnUpdateItGenericcomp(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_GENERICCOMPONENT)); }

void CCEtoODBDoc::OnUpdateItPcb(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_PCB)); }

void CCEtoODBDoc::OnUpdateItFiducial(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_FIDUCIAL)); }

void CCEtoODBDoc::OnUpdateItTooling(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_TOOLING)); }

void CCEtoODBDoc::OnUpdateItTestpoint(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_TESTPOINT)); }

void CCEtoODBDoc::OnUpdateItTestprobe(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_TEST_PROBE)); }

void CCEtoODBDoc::OnUpdateItFreepad(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_FREEPAD)); }

void CCEtoODBDoc::OnUpdateItGate(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_SYMBOL)); }

void CCEtoODBDoc::OnUpdateItPortinstance(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_PORTINSTANCE)); }

void CCEtoODBDoc::OnUpdateItDrillhole(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_DRILLHOLE)); }

void CCEtoODBDoc::OnUpdateItMechanicalpin(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_MECHANICALPIN)); }

void CCEtoODBDoc::OnUpdateItDrillsymbol(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_DRILLSYMBOL)); }

void CCEtoODBDoc::OnUpdateItCentroid(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_CENTROID)); }

void CCEtoODBDoc::OnUpdateItClearpad(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_CLEARPAD)); }

void CCEtoODBDoc::OnUpdateItReliefpad(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_RELIEFPAD)); }

void CCEtoODBDoc::OnUpdateItObstacle(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_OBSTACLE)); }

void CCEtoODBDoc::OnUpdateItDrcmarker(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_DRCMARKER)); }

void CCEtoODBDoc::OnUpdateItTestAccess(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_TEST_ACCESSPOINT)); }

void CCEtoODBDoc::OnUpdateItTestpad(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_TESTPAD)); }

void CCEtoODBDoc::OnUpdateItSchemJunction(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_SCHEM_JUNCTION)); }

void CCEtoODBDoc::OnUpdateItGluepoint(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_GLUEPOINT)); }

void CCEtoODBDoc::OnUpdateItRejectMarker(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_REJECTMARK)); }

void CCEtoODBDoc::OnUpdateItXOut(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_XOUT)); }

void CCEtoODBDoc::OnUpdateItHierarchicalSymbol(CCmdUI* pCmdUI) 
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_HIERARCHYSYMBOL)); }

void CCEtoODBDoc::OnUpdateItSheetConnector(CCmdUI* pCmdUI)
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_SHEETCONNECTOR)); }

void CCEtoODBDoc::OnUpdateItTiedot(CCmdUI* pCmdUI)
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_TIEDOT)); }

void CCEtoODBDoc::OnUpdateItRipper(CCmdUI* pCmdUI)
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_RIPPER)); }

void CCEtoODBDoc::OnUpdateItGround(CCmdUI* pCmdUI)
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_GROUND)); }

void CCEtoODBDoc::OnUpdateItTerminator(CCmdUI* pCmdUI)
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_TERMINATOR)); }

void CCEtoODBDoc::OnUpdateItAperture(CCmdUI* pCmdUI)
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_APERTURE)); }

void CCEtoODBDoc::OnUpdateItRealPart(CCmdUI* pCmdUI)
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_REALPART)); }

void CCEtoODBDoc::OnUpdateItPad(CCmdUI* pCmdUI)
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_PAD)); }

void CCEtoODBDoc::OnUpdateItPackage(CCmdUI* pCmdUI)
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_PACKAGE)); }

void CCEtoODBDoc::OnUpdateItPackagePin(CCmdUI* pCmdUI)
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(INSERTTYPE_PACKAGEPIN)); }

void CCEtoODBDoc::OnUpdateItStencilHole(CCmdUI* pCmdUI)
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(insertTypeStencilHole)); }

void CCEtoODBDoc::OnUpdateItComplexDrillHole(CCmdUI* pCmdUI)
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(insertTypeComplexDrillHole)); }

void CCEtoODBDoc::OnUpdateItCompositeComp(CCmdUI* pCmdUI)
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(insertTypeCompositeComp)); }

void CCEtoODBDoc::OnUpdateItRouteTarget(CCmdUI* pCmdUI)
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(insertTypeRouteTarget)); }

void CCEtoODBDoc::OnUpdateItDie(CCmdUI* pCmdUI)
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(insertTypeDie)); }

void CCEtoODBDoc::OnUpdateItDiePin(CCmdUI* pCmdUI)
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(insertTypeDiePin)); }

void CCEtoODBDoc::OnUpdateItBondPad(CCmdUI* pCmdUI)
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(insertTypeBondPad)); }

void CCEtoODBDoc::OnUpdateItBondWire(CCmdUI* pCmdUI)
{ pCmdUI->SetCheck(getInsertTypeFilter().contains(insertTypeBondWire)); }




/* The saving of .CC files is allowed in CAMCAD Vision */
void CCEtoODBDoc::OnUpdateSaveDataFile(CCmdUI* pCmdUI) 
{ 
#ifdef SHAREWARE
   pCmdUI->Enable(FALSE);
#endif
}

void CCEtoODBDoc::OnUpdateSaveCompressedDataFile(CCmdUI* pCmdUI) 
{ 
#ifdef SHAREWARE
   pCmdUI->Enable(FALSE);
#endif
}

void CCEtoODBDoc::OnUpdateSaveDataFileAs(CCmdUI* pCmdUI) 
{ 
#ifdef SHAREWARE
   pCmdUI->Enable(FALSE);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// update.cpp
///////////////////////////////////////////////////////////////////////////////

