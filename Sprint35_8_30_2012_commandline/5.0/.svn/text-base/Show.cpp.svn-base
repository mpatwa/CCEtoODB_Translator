// $Header: /CAMCAD/4.6/Show.cpp 23    3/27/07 3:44p Lynn Phung $

/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-97. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "mainfrm.h"
#include "CCEtoODB.h"

void CCEtoODBDoc::OnShowPinnrTop() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License for PCB Edit!");
      return;
   }*/

   showPinnrsTop = !showPinnrsTop;

   if (showPinnrsTop)
      showPinnetsTop = showPinnetsBottom = FALSE;

   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnShowPinnrBottom() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License for PCB Edit!");
      return;
   }*/

   showPinnrsBottom = !showPinnrsBottom;

   if (showPinnrsBottom)
      showPinnetsTop = showPinnetsBottom = FALSE;

   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnShowPinnetsTop() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License for PCB Edit!");
      return;
   }*/

   showPinnetsTop = !showPinnetsTop;

   if (showPinnetsTop)
      showPinnrsTop = showPinnrsBottom = FALSE;

   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnShowPinnetsBottom() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License for PCB Edit!");
      return;
   }*/

   showPinnetsBottom = !showPinnetsBottom;

   if (showPinnetsBottom)
      showPinnrsTop = showPinnrsBottom = FALSE;

   UpdateAllViews(NULL);
}


void CCEtoODBDoc::OnShowAll() 
{
   getGraphicsClassFilter().addAll();
   //for (int i=0; i<MAX_CLASSTYPE; i++) showClasses[i] = TRUE;
}

void CCEtoODBDoc::OnShowNone() 
{
   getGraphicsClassFilter().empty();
   //for (int i=0; i<MAX_CLASSTYPE; i++) showClasses[i] = FALSE;
}

void CCEtoODBDoc::OnShowFills() 
{
   m_showFills = !m_showFills;
}

void CCEtoODBDoc::OnShowApFills() 
{
   m_showApFills = !m_showApFills;
}

void CCEtoODBDoc::OnShowAttribs() 
{
   m_showAttribs = !m_showAttribs;
}

/******************************************************************************
/******************************************************************************
* Graphic class visiblity
*/
void CCEtoODBDoc::OnShowMilling() 
{ getGraphicsClassFilter().toggle(graphicClassMillingPath); }

void CCEtoODBDoc::OnShowAllkeepin() 
{ getGraphicsClassFilter().toggle(graphicClassAllKeepIn); }

void CCEtoODBDoc::OnShowAllkeepout() 
{ getGraphicsClassFilter().toggle(graphicClassAllKeepOut); }

void CCEtoODBDoc::OnShowAnalysis() 
{ getGraphicsClassFilter().toggle(graphicClassAnalysis); }

void CCEtoODBDoc::OnShowAntiEtch() 
{ getGraphicsClassFilter().toggle(graphicClassAntiEtch); }

void CCEtoODBDoc::OnShowBoardcutout() 
{ getGraphicsClassFilter().toggle(graphicClassBoardCutout); }

void CCEtoODBDoc::OnShowBoardGeom() 
{ getGraphicsClassFilter().toggle(graphicClassBoardGeometry); }

void CCEtoODBDoc::OnShowBoardoutline() 
{ getGraphicsClassFilter().toggle(graphicClassBoardOutline); }

void CCEtoODBDoc::OnShowComponentboundary() 
{ getGraphicsClassFilter().toggle(graphicClassComponentBoundary); }

void CCEtoODBDoc::OnShowCompoutline() 
{ getGraphicsClassFilter().toggle(graphicClassComponentOutline); }

void CCEtoODBDoc::OnShowDrawing() 
{ getGraphicsClassFilter().toggle(graphicClassDrawing); }

void CCEtoODBDoc::OnShowPackageOutline() 
{ getGraphicsClassFilter().toggle(graphicClassPackageOutline); }

void CCEtoODBDoc::OnShowDrc() 
{ getGraphicsClassFilter().toggle(graphicClassDrc); }

void CCEtoODBDoc::OnShowEtch() 
{ getGraphicsClassFilter().toggle(graphicClassEtch); }

void CCEtoODBDoc::OnShowManufacturing() 
{ getGraphicsClassFilter().toggle(graphicClassManufacturing); }

void CCEtoODBDoc::OnShowNormal() 
{ getGraphicsClassFilter().toggle(graphicClassNormal); }

void CCEtoODBDoc::OnShowPanelcutout() 
{ getGraphicsClassFilter().toggle(graphicClassPanelCutout); }

void CCEtoODBDoc::OnShowPaneloutline() 
{ getGraphicsClassFilter().toggle(graphicClassPanelOutline); }

void CCEtoODBDoc::OnShowPlacekeepin() 
{ getGraphicsClassFilter().toggle(graphicClassPlaceKeepIn); }

void CCEtoODBDoc::OnShowPlacekeepout() 
{ getGraphicsClassFilter().toggle(graphicClassPlaceKeepOut); }

void CCEtoODBDoc::OnShowPlacementregion() 
{ getGraphicsClassFilter().toggle(graphicClassPlacementRegion); }

void CCEtoODBDoc::OnShowRoutkeepin() 
{ getGraphicsClassFilter().toggle(graphicClassRouteKeepIn); }

void CCEtoODBDoc::OnShowRoutkeepout() 
{ getGraphicsClassFilter().toggle(graphicClassRouteKeepOut); }

void CCEtoODBDoc::OnShowTestpointkeepin() 
{ getGraphicsClassFilter().toggle(graphicClassTestPointKeepIn); }

void CCEtoODBDoc::OnShowTestpointkeepout() 
{ getGraphicsClassFilter().toggle(graphicClassTestPointKeepOut); }

void CCEtoODBDoc::OnShowTraceFanout() 
{ getGraphicsClassFilter().toggle(graphicClassTraceFanout); }

void CCEtoODBDoc::OnShowViakeepin() 
{ getGraphicsClassFilter().toggle(graphicClassViaKeepIn); }

void CCEtoODBDoc::OnShowViakeepout() 
{ getGraphicsClassFilter().toggle(graphicClassViaKeepOut); }

void CCEtoODBDoc::OnShowUnroute() 
{ getGraphicsClassFilter().toggle(graphicClassUnroute); }

void CCEtoODBDoc::OnShowPackageBody() 
{ getGraphicsClassFilter().toggle(graphicClassPackageBody); }

void CCEtoODBDoc::OnShowPackagePin() 
{ getGraphicsClassFilter().toggle(graphicClassPackagePin); }

void CCEtoODBDoc::OnShowSignal() 
{ getGraphicsClassFilter().toggle(graphicClassSignal); }

void CCEtoODBDoc::OnShowBus() 
{ getGraphicsClassFilter().toggle(graphicClassBus); }

void CCEtoODBDoc::OnShowPanelBorder() 
{ getGraphicsClassFilter().toggle(graphicClassPanelBorder); }

void CCEtoODBDoc::OnShowManufacturingOutl() 
{ getGraphicsClassFilter().toggle(graphicClassManufacturingOutl); }

void CCEtoODBDoc::OnShowContour() 
{ getGraphicsClassFilter().toggle(graphicClassContour); }

void CCEtoODBDoc::OnShowPlaneKeepout() 
{ getGraphicsClassFilter().toggle(graphicClassPlaneKeepout); }

void CCEtoODBDoc::OnShowPlaneNoConnect() 
{ getGraphicsClassFilter().toggle(graphicClassPlaneNoConnect); }

void CCEtoODBDoc::OnShowBoardKeepout() 
{ getGraphicsClassFilter().toggle(graphicClassBoardKeepout); }

void CCEtoODBDoc::OnShowPadKeepout() 
{ getGraphicsClassFilter().toggle(graphicClassPadKeepout); }

void CCEtoODBDoc::OnShowEPBody() 
{ getGraphicsClassFilter().toggle(graphicClassEPBody); }

void CCEtoODBDoc::OnShowEPMask() 
{ getGraphicsClassFilter().toggle(graphicClassEPMask); }

void CCEtoODBDoc::OnShowEPOverGlaze() 
{ getGraphicsClassFilter().toggle(graphicClassEPOverGlaze); }

void CCEtoODBDoc::OnShowEPDielectric() 
{ getGraphicsClassFilter().toggle(graphicClassEPDielectric); }

void CCEtoODBDoc::OnShowEPConductive() 
{ getGraphicsClassFilter().toggle(graphicClassEPConductive); }

void CCEtoODBDoc::OnShowBusPath() 
{ getGraphicsClassFilter().toggle(graphicClassBusPath); }

void CCEtoODBDoc::OnShowRegion() 
{ getGraphicsClassFilter().toggle(graphicClassRegion); }

void CCEtoODBDoc::OnShowCavity() 
{ getGraphicsClassFilter().toggle(graphicClassCavity); }


/******************************************************************************
/******************************************************************************
* Insert type visiblity
*/
void CCEtoODBDoc::OnItAll() 
{ 
   getInsertTypeFilter().addAll();
   //for (int i=0; i<MAX_INSERTTYPE; i++) showInsertTypes[i] = TRUE; 
}

void CCEtoODBDoc::OnItNone() 
{
   getInsertTypeFilter().empty();
   //for (int i=0; i<MAX_INSERTTYPE; i++) showInsertTypes[i] = FALSE;
}

void CCEtoODBDoc::OnItUnknown() 
{ getInsertTypeFilter().toggle(INSERTTYPE_UNKNOWN); }

void CCEtoODBDoc::OnItVia() 
{ getInsertTypeFilter().toggle(INSERTTYPE_VIA); }

void CCEtoODBDoc::OnItPin() 
{ getInsertTypeFilter().toggle(INSERTTYPE_PIN); }

void CCEtoODBDoc::OnItPcbcomp() 
{ getInsertTypeFilter().toggle(INSERTTYPE_PCBCOMPONENT); }

void CCEtoODBDoc::OnItMechcomp() 
{ getInsertTypeFilter().toggle(INSERTTYPE_MECHCOMPONENT); }

void CCEtoODBDoc::OnItGenericcomp() 
{ getInsertTypeFilter().toggle(INSERTTYPE_GENERICCOMPONENT); }

void CCEtoODBDoc::OnItPcb() 
{ getInsertTypeFilter().toggle(INSERTTYPE_PCB); }

void CCEtoODBDoc::OnItFiducial() 
{ getInsertTypeFilter().toggle(INSERTTYPE_FIDUCIAL); }

void CCEtoODBDoc::OnItTooling() 
{ getInsertTypeFilter().toggle(INSERTTYPE_TOOLING); }

void CCEtoODBDoc::OnItTestpoint() 
{ getInsertTypeFilter().toggle(INSERTTYPE_TESTPOINT); }

void CCEtoODBDoc::OnItFreepad() 
{ getInsertTypeFilter().toggle(INSERTTYPE_FREEPAD); }

void CCEtoODBDoc::OnItGate() 
{ getInsertTypeFilter().toggle(INSERTTYPE_SYMBOL); }

void CCEtoODBDoc::OnItPortinstance() 
{ getInsertTypeFilter().toggle(INSERTTYPE_PORTINSTANCE); }

void CCEtoODBDoc::OnItDrillhole() 
{ getInsertTypeFilter().toggle(INSERTTYPE_DRILLHOLE); }

void CCEtoODBDoc::OnItMechanicalpin() 
{ getInsertTypeFilter().toggle(INSERTTYPE_MECHANICALPIN); }

void CCEtoODBDoc::OnItTestprobe() 
{ getInsertTypeFilter().toggle(INSERTTYPE_TEST_PROBE); }

void CCEtoODBDoc::OnItDrillsymbol() 
{ getInsertTypeFilter().toggle(INSERTTYPE_DRILLSYMBOL); }

void CCEtoODBDoc::OnItCentroid() 
{ getInsertTypeFilter().toggle(INSERTTYPE_CENTROID); }

void CCEtoODBDoc::OnItClearpad() 
{ getInsertTypeFilter().toggle(INSERTTYPE_CLEARPAD); }

void CCEtoODBDoc::OnItReliefpad() 
{ getInsertTypeFilter().toggle(INSERTTYPE_RELIEFPAD); }

void CCEtoODBDoc::OnItObstacle() 
{ getInsertTypeFilter().toggle(INSERTTYPE_OBSTACLE); }

void CCEtoODBDoc::OnItDrcmarker() 
{ getInsertTypeFilter().toggle(INSERTTYPE_DRCMARKER); }

void CCEtoODBDoc::OnItTestAccess() 
{ getInsertTypeFilter().toggle(INSERTTYPE_TEST_ACCESSPOINT); }

void CCEtoODBDoc::OnItTestpad() 
{ getInsertTypeFilter().toggle(INSERTTYPE_TESTPAD); }

void CCEtoODBDoc::OnItSchemJunction() 
{ getInsertTypeFilter().toggle(INSERTTYPE_SCHEM_JUNCTION); }

void CCEtoODBDoc::OnItGluepoint() 
{ getInsertTypeFilter().toggle(INSERTTYPE_GLUEPOINT); }

void CCEtoODBDoc::OnItRejectMarker() 
{ getInsertTypeFilter().toggle(INSERTTYPE_REJECTMARK); }

void CCEtoODBDoc::OnItXOut() 
{ getInsertTypeFilter().toggle(INSERTTYPE_XOUT); }

void CCEtoODBDoc::OnItHierarchicalSymbol()
{ getInsertTypeFilter().toggle(INSERTTYPE_HIERARCHYSYMBOL); }

void CCEtoODBDoc::OnItSheetConnector()
{ getInsertTypeFilter().toggle(INSERTTYPE_SHEETCONNECTOR); }

void CCEtoODBDoc::OnItTiedot()
{ getInsertTypeFilter().toggle(INSERTTYPE_TIEDOT); }

void CCEtoODBDoc::OnItRipper()
{ getInsertTypeFilter().toggle(INSERTTYPE_RIPPER); }

void CCEtoODBDoc::OnItGround()
{ getInsertTypeFilter().toggle(INSERTTYPE_GROUND); }

void CCEtoODBDoc::OnItTerminator()
{ getInsertTypeFilter().toggle(INSERTTYPE_TERMINATOR); }

void CCEtoODBDoc::OnItAperture()
{ getInsertTypeFilter().toggle(INSERTTYPE_APERTURE); }

void CCEtoODBDoc::OnItRealPart()
{ getInsertTypeFilter().toggle(INSERTTYPE_REALPART); }

void CCEtoODBDoc::OnItPad()
{ getInsertTypeFilter().toggle(INSERTTYPE_PAD); }

void CCEtoODBDoc::OnItPackage()
{ getInsertTypeFilter().toggle(INSERTTYPE_PACKAGE); }

void CCEtoODBDoc::OnItPackagePin()
{ getInsertTypeFilter().toggle(INSERTTYPE_PACKAGEPIN); }

void CCEtoODBDoc::OnItStencilHole()
{ getInsertTypeFilter().toggle(insertTypeStencilHole); }

void CCEtoODBDoc::OnItComplexDrillHole()
{ getInsertTypeFilter().toggle(insertTypeComplexDrillHole); }

void CCEtoODBDoc::OnItCompositeComp()
{ getInsertTypeFilter().toggle(insertTypeCompositeComp); }

void CCEtoODBDoc::OnItRouteTarget()
{ getInsertTypeFilter().toggle(insertTypeRouteTarget); }

void CCEtoODBDoc::OnItDie()
{ getInsertTypeFilter().toggle(insertTypeDie); }

void CCEtoODBDoc::OnItDiePin()
{ getInsertTypeFilter().toggle(insertTypeDiePin); }

void CCEtoODBDoc::OnItBondPad()
{ getInsertTypeFilter().toggle(insertTypeBondPad); }

void CCEtoODBDoc::OnItBondWire()
{ getInsertTypeFilter().toggle(insertTypeBondWire); }


/*****************************************************************************
* Fill Style
*/
void CCEtoODBDoc::OnFillSolid() 
{
   getSettings().FillStyle = HS_SOLID;

   CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   CMenu *menu = frame->GetMenu();
   if (menu)
   {
      menu->CheckMenuItem(ID_FILL_SOLID, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_HORIZONTAL, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_VERTICAL, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_CROSS, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL1, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL2, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL_CROSS, MF_UNCHECKED);

      menu->CheckMenuItem(ID_FILL_SOLID, MF_CHECKED);
   }

   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnFillHorizontal() 
{
   getSettings().FillStyle = HS_HORIZONTAL;

   CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   CMenu *menu = frame->GetMenu();
   if (menu)
   {
      menu->CheckMenuItem(ID_FILL_SOLID, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_HORIZONTAL, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_VERTICAL, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_CROSS, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL1, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL2, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL_CROSS, MF_UNCHECKED);

      menu->CheckMenuItem(ID_FILL_HORIZONTAL, MF_CHECKED);
   }

   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnFillVertical() 
{
   getSettings().FillStyle = HS_VERTICAL;

   CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   CMenu *menu = frame->GetMenu();
   if (menu)
   {
      menu->CheckMenuItem(ID_FILL_SOLID, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_HORIZONTAL, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_VERTICAL, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_CROSS, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL1, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL2, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL_CROSS, MF_UNCHECKED);

      menu->CheckMenuItem(ID_FILL_VERTICAL, MF_CHECKED);
   }

   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnFillCross() 
{
   getSettings().FillStyle = HS_CROSS;

   CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   CMenu *menu = frame->GetMenu();
   if (menu)
   {
      menu->CheckMenuItem(ID_FILL_SOLID, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_HORIZONTAL, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_VERTICAL, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_CROSS, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL1, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL2, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL_CROSS, MF_UNCHECKED);

      menu->CheckMenuItem(ID_FILL_CROSS, MF_CHECKED);
   }

   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnFillDiagonal1() 
{
   getSettings().FillStyle = HS_FDIAGONAL;

   CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   CMenu *menu = frame->GetMenu();
   if (menu)
   {
      menu->CheckMenuItem(ID_FILL_SOLID, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_HORIZONTAL, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_VERTICAL, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_CROSS, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL1, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL2, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL_CROSS, MF_UNCHECKED);

      menu->CheckMenuItem(ID_FILL_DIAGONAL1, MF_CHECKED);
   }

   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnFillDiagonal2() 
{
   getSettings().FillStyle = HS_BDIAGONAL;

   CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   CMenu *menu = frame->GetMenu();
   if (menu)
   {
      menu->CheckMenuItem(ID_FILL_SOLID, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_HORIZONTAL, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_VERTICAL, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_CROSS, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL1, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL2, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL_CROSS, MF_UNCHECKED);

      menu->CheckMenuItem(ID_FILL_DIAGONAL2, MF_CHECKED);
   }

   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnFillDiagonalCross() 
{
   getSettings().FillStyle = HS_DIAGCROSS;

   CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   CMenu *menu = frame->GetMenu();
   if (menu)
   {
      menu->CheckMenuItem(ID_FILL_SOLID, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_HORIZONTAL, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_VERTICAL, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_CROSS, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL1, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL2, MF_UNCHECKED);
      menu->CheckMenuItem(ID_FILL_DIAGONAL_CROSS, MF_UNCHECKED);

      menu->CheckMenuItem(ID_FILL_DIAGONAL_CROSS, MF_CHECKED);
   }

   UpdateAllViews(NULL);
}

// end SHOW.CPP
