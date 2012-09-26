// $Header: /CAMCAD/5.0/Dfm.cpp 78    6/17/07 8:51p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "DFM.h"
#include "crypt.h"
#include "API.h"
#include "ccview.h"
#include "graph.h"
#include "xform.h"
#include "apertur2.h"
#include "net_util.h"
#include "dfm_dist.h"
#include "drc.h"
#include "float.h"
#include "extents.h"

extern char *FailureRanges[]; // from DRC.CPP

int maxErrorsPerTest = 0;

/******************************************************************************
* DFM 
*/
IMPLEMENT_DYNCREATE(DFM, CCmdTarget)
DFM::DFM()
{
   EnableAutomation();
   
   // To keep the application running as long as an OLE automation 
   // object is active, the constructor calls AfxOleLockApp.
   
   AfxOleLockApp();
}

DFM::~DFM()
{
   // To terminate the application when all objects created with
   //    with OLE automation, the destructor calls AfxOleUnlockApp.
   
   AfxOleUnlockApp();
}

void DFM::OnFinalRelease()
{
   // When the last reference for an automation object is released
   // OnFinalRelease is called.  The base class will automatically
   // deletes the object.  Add additional cleanup required for your
   // object before calling the base class.

   CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(DFM, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(DFM, CCmdTarget)
   DISP_PROPERTY_NOTIFY_ID(DFM, "maxErrorsPerTest", dispidmaxErrorsPerTest, m_maxErrorsPerTest, OnMaxErrorsPerTestChanged, VT_I2)
   DISP_FUNCTION_ID(DFM, "Flatten", dispidFlatten, Flatten, VT_I2, VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION_ID(DFM, "CreateFlattenGeometry", dispidCreateFlattenGeometry, CreateFlattenGeometry, VT_I2, VTS_I4 VTS_PI2)
   DISP_FUNCTION_ID(DFM, "FlattenByInserttype", dispidFlattenByInserttype, FlattenByInserttype, VT_I2, VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION_ID(DFM, "DistanceCheck", dispidDistanceCheck, DistanceCheck, VT_I2, VTS_I2 VTS_I2 VTS_R8 VTS_R8 VTS_BSTR VTS_I2 VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION_ID(DFM, "DistanceCheckDiffNets", dispidDistanceCheckDiffNets, DistanceCheckDiffNets, VT_I2, VTS_I2 VTS_R8 VTS_R8 VTS_BSTR VTS_I2 VTS_I2)
   DISP_FUNCTION_ID(DFM, "DistanceCheckTwoNets", dispidDistanceCheckTwoNets, DistanceCheckTwoNets, VT_I2, VTS_I2 VTS_BSTR VTS_BSTR VTS_R8 VTS_R8 VTS_BSTR VTS_I2)
   DISP_FUNCTION_ID(DFM, "ComponentShadow", dispidComponentShadow, ComponentShadow, VT_I2, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8)
   DISP_FUNCTION_ID(DFM, "PanelDimensions", dispidPanelDimensions, PanelDimensions, VT_I2, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_BSTR)
   DISP_FUNCTION_ID(DFM, "PanelLengthToWidthRatio", dispidPanelLengthToWidthRatio, PanelLengthToWidthRatio, VT_I2, VTS_I4 VTS_R8 VTS_BSTR)
   DISP_FUNCTION_ID(DFM, "ClearanceToPanelEdge", dispidClearanceToPanelEdge, ClearanceToPanelEdge, VT_I2, VTS_I2 VTS_R8 VTS_R8 VTS_BSTR)
   DISP_FUNCTION_ID(DFM, "FlattenByAttrib", dispidFlattenByAttrib, FlattenByAttrib, VT_I2, VTS_I2 VTS_I2 VTS_BSTR VTS_BSTR VTS_I2 VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION_ID(DFM, "ItemsInCorners", dispidItemsInCorners, ItemsInCorners, VT_I2, VTS_I2 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I2 VTS_BSTR VTS_I2)
   DISP_FUNCTION_ID(DFM, "ComponentFlushWithOutline", dispidComponentFlushWithOutline, ComponentFlushWithOutline, VT_I2, VTS_I2 VTS_R8 VTS_BSTR)
   DISP_FUNCTION_ID(DFM, "TestpointsPerNet", dispidTestpointsPerNet, TestpointsPerNet, VT_I2, VTS_I2 VTS_I2 VTS_BSTR)
   DISP_FUNCTION_ID(DFM, "ProcessSolderMask", dispidProcessSolderMask, ProcessSolderMask, VT_I2, VTS_I2 VTS_I2)
   DISP_FUNCTION_ID(DFM, "ViaUnderPart", dispidViaUnderPart, ViaUnderPart, VT_I2, VTS_I2 VTS_I2 VTS_BSTR)
   DISP_FUNCTION_ID(DFM, "ExposedTestpointPadSize", dispidExposedTestpointPadSize, ExposedTestpointPadSize, VT_I2, VTS_I2 VTS_R8 VTS_R8 VTS_BSTR)
   DISP_FUNCTION_ID(DFM, "ViaPadSize", dispidViaPadSize, ViaPadSize, VT_I2, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_BSTR)
   DISP_FUNCTION_ID(DFM, "ViaHoleSize", dispidViaHoleSize, ViaHoleSize, VT_I2, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_BSTR)
   DISP_FUNCTION_ID(DFM, "FilterGeometry", dispidFilterGeometry, FilterGeometry, VT_I2, VTS_I2 VTS_BSTR VTS_BSTR VTS_I2)
   DISP_FUNCTION_ID(DFM, "ThruHolePadShapes", dispidThruHolePadShapes, ThruHolePadShapes, VT_I2, VTS_I4 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_BSTR)
   DISP_FUNCTION_ID(DFM, "RouteTraceWidthMinimum", dispidRouteTraceWidthMinimum, RouteTraceWidthMinimum, VT_I2, VTS_I2 VTS_R8 VTS_R8 VTS_BSTR VTS_BSTR)
   DISP_FUNCTION_ID(DFM, "AllowableComponentOrientation", dispidAllowableComponentOrientation, AllowableComponentOrientation, VT_I2, VTS_I4 VTS_I2 VTS_I2 VTS_BSTR)
   DISP_FUNCTION_ID(DFM, "BoardThicknessToDrillDiameter", dispidBoardThicknessToDrillDiameter, BoardThicknessToDrillDiameter, VT_I2, VTS_I4 VTS_R8 VTS_BSTR)
   DISP_FUNCTION_ID(DFM, "GenerateEtchBlockers", dispidGenerateEtchBlockers, GenerateEtchBlockers, VT_I2, VTS_R8 VTS_PI2 VTS_PI2)
   DISP_FUNCTION_ID(DFM, "FlattenByGraphicClass", dispidFlattenByGraphicClass, FlattenByGraphicClass, VT_I2, VTS_I2 VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION_ID(DFM, "AnnularRing", dispidAnnularRing, AnnularRing, VT_I2, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_BSTR)
   DISP_FUNCTION_ID(DFM, "SMDSoldermaskClearance", dispidSMDSoldermaskClearance, SMDSoldermaskClearance, VT_I2, VTS_I2 VTS_I2 VTS_R8 VTS_R8 VTS_BSTR)
   DISP_FUNCTION_ID(DFM, "GenerateDFxOutlines", dispidGenerateDFxOutlines, GenerateDFxOutlines, VT_I2, VTS_NONE)
   DISP_FUNCTION_ID(DFM, "DistanceCheck2", dispidDistanceCheck2, DistanceCheck2, VT_I2, VTS_I2 VTS_I2 VTS_R8 VTS_R8 VTS_BSTR VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION_ID(DFM, "GerberThermalBarrelPlugCheck", dispidGerberThermalBarrelPlugCheck, GerberThermalBarrelPlugCheck, VT_I2, VTS_I4)
   DISP_FUNCTION_ID(DFM, "Pin1Orientation", dispidPin1Orientation, Pin1Orientation, VT_I2, VTS_I4 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I4)
   DISP_FUNCTION_ID(DFM, "TestPadDensityCheck", dispidTestPadDensityCheck, TestPadDensityCheck, VT_I2, VTS_I4 VTS_I2 VTS_I2 VTS_R8 VTS_I2)
   DISP_FUNCTION_ID(DFM, "ComponentNotAllowedPerSurface", dispidComponentNotAllowedPerSurface, ComponentNotAllowedPerSurface, VT_I2, VTS_I4 VTS_I2 VTS_I2 VTS_BSTR VTS_BSTR)
   DISP_FUNCTION_ID(DFM, "ComponentHeight", dispidComponentHeight, ComponentHeight, VT_I2, VTS_I4 VTS_I2 VTS_I2 VTS_BSTR VTS_R8 VTS_R8 VTS_R8 VTS_R8)
   DISP_FUNCTION_ID(DFM, "CombinePolies", dispidCombinePolies, CombinePolies, VT_I2, VTS_I2)
END_DISPATCH_MAP()
     
// Note: we add support for IID_IDFM to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .IDL file.

// {DAD9B785-A34F-4786-8111-9E6B9DDA6DC1}
static const IID IID_IDFM =
{ 0xDAD9B785, 0xA34F, 0x4786, { 0x81, 0x11, 0x9E, 0x6B, 0x9D, 0xDA, 0x6D, 0xC1 } };

BEGIN_INTERFACE_MAP(DFM, CCmdTarget)
   INTERFACE_PART(DFM, IID_IDFM, Dispatch)
END_INTERFACE_MAP()

// {02CAD7F0-A311-4157-ACFC-A6666C55FF19}
IMPLEMENT_OLECREATE(DFM, "CAMCAD.DFM", 0x2cad7f0, 0xa311, 0x4157, 0xac, 0xfc, 0xa6, 0x66, 0x6c, 0x55, 0xff, 0x19)
//IMPLEMENT_OLECREATE_FLAGS(DFM, "CAMCAD.DFM", afxRegApartmentThreading, 0x2cad7f0, 0xa311, 0x4157, 0xac, 0xfc, 0xa6, 0x66, 0x6c, 0x55, 0xff, 0x19)


// DFM message handlers

/******************************************************************************
* DFM::OnMaxErrorsPerTestChanged
*/
void DFM::OnMaxErrorsPerTestChanged(void)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   maxErrorsPerTest = m_maxErrorsPerTest;
}


/******************************************************************************
* GetDirectionOfTravel
*/
int GetDirectionOfTravel(CCEtoODBDoc *doc, BlockStruct *fileGeom)
{
   if (!fileGeom->getAttributesRef())
      return 0;
   
   WORD keyword = doc->IsKeyWord(ATT_DIRECTION_OF_TRAVEL, 0);

   Attrib* attrib;

   if (!fileGeom->getAttributesRef()->Lookup(keyword, attrib))
      return 0;

   CString direction = attrib->getStringValue();

   if (!direction.CompareNoCase("Up"))
      return 3;

   if (!direction.CompareNoCase("Down"))
      return 4;

   if (!direction.CompareNoCase("Left"))
      return 2;

   return 1; // default right
}
