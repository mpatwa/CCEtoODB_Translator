// $Header: /CAMCAD/DcaLib/DcaLayerType.h 3     3/09/07 5:15p Kurt Van Ness $

#if !defined(__DcaLayerType_h__)
#define __DcaLayerType_h__

#pragma once

//  LAYTYPE_UNKNOWN                  0
//  LAYTYPE_SIGNAL_TOP               1 
//  LAYTYPE_SIGNAL_BOT               2 
//  LAYTYPE_SIGNAL_INNER             3 
//  LAYTYPE_SIGNAL_ALL               4 
//  LAYTYPE_DIALECTRIC               5 
//  LAYTYPE_PAD_TOP                  6 
//  LAYTYPE_PAD_BOTTOM               7 
//  LAYTYPE_PAD_INNER                8 
//  LAYTYPE_PAD_ALL                  9 
//  LAYTYPE_PAD_THERMAL              10
//  LAYTYPE_PASTE_TOP                11
//  LAYTYPE_PASTE_BOTTOM             12
//  LAYTYPE_MASK_TOP                 13
//  LAYTYPE_MASK_BOTTOM              14
//  LAYTYPE_SILK_TOP                 15
//  LAYTYPE_SILK_BOTTOM              16
//  LAYTYPE_POWERNEG                 17
//  LAYTYPE_POWERPOS                 18
//  LAYTYPE_SPLITPLANE               19
//  LAYTYPE_SIGNAL                   20
//  LAYTYPE_MASK_ALL                 21
//  LAYTYPE_PASTE_ALL                22
//  LAYTYPE_REDLINE                  23
//  LAYTYPE_DRILL                    24
//  LAYTYPE_TOP                      25
//  LAYTYPE_BOTTOM                   26
//  LAYTYPE_ALL                      27
//  LAYTYPE_BOARD_OUTLINE            28
//  LAYTYPE_PAD_OUTER                29
//  LAYTYPE_SIGNAL_OUTER             30
//  LAYTYPE_DRC_CRITICAL             31
//  LAYTYPE_PLANECLEARANCE           32
//  LAYTYPE_COMPONENTOUTLINE         33
//  LAYTYPE_PANEL_OUTLINE            34
//  LAYTYPE_COMPONENT_DFT_TOP        35
//  LAYTYPE_COMPONENT_DFT_BOTTOM     36
//  LAYTYPE_DRC_MARGINAL             37
//  LAYTYPE_DRC_ACCEPTABLE           38
//  LAYTYPE_FLUX_TOP                 39
//  LAYTYPE_FLUX_BOTTOM              40
//  LAYTYPE_PKG_PIN_LEG_TOP          41
//  LAYTYPE_PKG_PIN_LEG_BOT          42
//  LAYTYPE_PKG_PIN_FOOT_TOP         43
//  LAYTYPE_PKG_PIN_FOOT_BOT         44
//  LAYTYPE_STENCIL_TOP              45
//  LAYTYPE_STENCIL_BOTTOM           46
//  LAYTYPE_CENTROID_TOP             47
//  LAYTYPE_CENTROID_BOTTOM          48
//  LAYTYPE_PKG_BODY_TOP             49
//  LAYTYPE_PKG_BODY_BOT             50
//  LAYTYPE_DFT_TOP                  51
//  LAYTYPE_DFT_BOTTOM               52

//_____________________________________________________________________________
enum LayerTypeTag
{
   layerTypeAcceptableDrc        = 38 ,  //  LAYTYPE_DRC_ACCEPTABLE           38
   layerTypeAll                  = 27 ,  //  LAYTYPE_ALL                      27
   layerTypeBoardOutline         = 28 ,  //  LAYTYPE_BOARD_OUTLINE            28
   layerTypeBottom               = 26 ,  //  LAYTYPE_BOTTOM                   26
   layerTypeCentroidBottom       = 48 ,  //  LAYTYPE_CENTROID_BOTTOM          48
   layerTypeCentroidTop          = 47 ,  //  LAYTYPE_CENTROID_TOP             47
   layerTypeComponentDftBottom   = 36 ,  //  LAYTYPE_COMPONENT_DFT_BOTTOM     36
   layerTypeComponentDftTop      = 35 ,  //  LAYTYPE_COMPONENT_DFT_TOP        35
   layerTypeComponentOutline     = 33 ,  //  LAYTYPE_COMPONENTOUTLINE         33
   layerTypeCriticalDrc          = 31 ,  //  LAYTYPE_DRC_CRITICAL             31
   layerTypeDftBottom            = 52 ,  //  LAYTYPE_DFT_BOTTOM               52
   layerTypeDftTop               = 51 ,  //  LAYTYPE_DFT_TOP                  51
   layerTypeDielectric           = 5  ,  //  LAYTYPE_DIELECTRIC               5 
   layerTypeDrill                = 24 ,  //  LAYTYPE_DRILL                    24
   layerTypeFluxBottom           = 40 ,  //  LAYTYPE_FLUX_BOTTOM              40
   layerTypeFluxTop              = 39 ,  //  LAYTYPE_FLUX_TOP                 39
   layerTypeMarginalDrc          = 37 ,  //  LAYTYPE_DRC_MARGINAL             37
   layerTypeMaskAll              = 21 ,  //  LAYTYPE_MASK_ALL                 21
   layerTypeMaskBottom           = 14 ,  //  LAYTYPE_MASK_BOTTOM              14
   layerTypeMaskTop              = 13 ,  //  LAYTYPE_MASK_TOP                 13
   layerTypePackageBodyBottom    = 50 ,  //  LAYTYPE_PKG_BODY_BOT             50
   layerTypePackageBodyTop       = 49 ,  //  LAYTYPE_PKG_BODY_TOP             49
   layerTypePackagePinFootBottom = 44 ,  //  LAYTYPE_PKG_PIN_FOOT_BOT         44
   layerTypePackagePinFootTop    = 43 ,  //  LAYTYPE_PKG_PIN_FOOT_TOP         43
   layerTypePackagePinLegBottom  = 42 ,  //  LAYTYPE_PKG_PIN_LEG_BOT          42
   layerTypePackagePinLegTop     = 41 ,  //  LAYTYPE_PKG_PIN_LEG_TOP          41
   layerTypePadAll               = 9  ,  //  LAYTYPE_PAD_ALL                  9 
   layerTypePadBottom            = 7  ,  //  LAYTYPE_PAD_BOTTOM               7 
   layerTypePadInner             = 8  ,  //  LAYTYPE_PAD_INNER                8 
   layerTypePadOuter             = 29 ,  //  LAYTYPE_PAD_OUTER                29
   layerTypePadThermal           = 10 ,  //  LAYTYPE_PAD_THERMAL              10
   layerTypePadTop               = 6  ,  //  LAYTYPE_PAD_TOP                  6 
   layerTypePanelOutline         = 34 ,  //  LAYTYPE_PANEL_OUTLINE            34
   layerTypePasteAll             = 22 ,  //  LAYTYPE_PASTE_ALL                22
   layerTypePasteBottom          = 12 ,  //  LAYTYPE_PASTE_BOTTOM             12
   layerTypePasteTop             = 11 ,  //  LAYTYPE_PASTE_TOP                11
   layerTypePlaneClearance       = 32 ,  //  LAYTYPE_PLANECLEARANCE           32
   layerTypePowerNegative        = 17 ,  //  LAYTYPE_POWERNEG                 17
   layerTypePowerPositive        = 18 ,  //  LAYTYPE_POWERPOS                 18
   layerTypeRedLine              = 23 ,  //  LAYTYPE_REDLINE                  23
   layerTypeSignal               = 20 ,  //  LAYTYPE_SIGNAL                   20
   layerTypeSignalAll            = 4  ,  //  LAYTYPE_SIGNAL_ALL               4 
   layerTypeSignalBottom         = 2  ,  //  LAYTYPE_SIGNAL_BOT               2 
   layerTypeSignalInner          = 3  ,  //  LAYTYPE_SIGNAL_INNER             3 
   layerTypeSignalOuter          = 30 ,  //  LAYTYPE_SIGNAL_OUTER             30
   layerTypeSignalTop            = 1  ,  //  LAYTYPE_SIGNAL_TOP               1 
   layerTypeSilkBottom           = 16 ,  //  LAYTYPE_SILK_BOTTOM              16
   layerTypeSilkTop              = 15 ,  //  LAYTYPE_SILK_TOP                 15
   layerTypeSplitPlane           = 19 ,  //  LAYTYPE_SPLITPLANE               19
   layerTypeStencilBottom        = 46 ,  //  LAYTYPE_STENCIL_BOTTOM           46
   layerTypeStencilTop           = 45 ,  //  LAYTYPE_STENCIL_TOP              45
   layerTypeTop                  = 25 ,  //  LAYTYPE_TOP                      25
   layerTypeDocumentation        = 53 ,  // LAYTYPE_DOCUMENTATION             53
   layerTypeScoringLine          = 54 ,  // LAYTYPE_SCORING_LINE              54
   layerTypeShearingLine         = 55 ,  // LAYTYPE_SHEARING_LINE             55
   layerTypeDrillDrawingThru     = 56 ,  // LAYTYPE_DRILL_DRAWING_THRU        56
   layerTypeComponentOutlineBot  = 57 ,  // LAYTYPE_COMPONENTOUTLINE_BOTTOM   57
   layerTypeStackLevelTop        = 58 ,  // LAYTYPE_STACKLEVEL_TOP            58
   layerTypeStackLevelBottom     = 59 ,  // LAYTYPE_STACKLEVEL_BOTTOM         59
   layerTypeUnknown              = 0  ,  //  LAYTYPE_UNKNOWN                  0

   layerTypeLowerBound           = 0,
   layerTypeUpperBound           = 59,
   layerTypeUndefined            = layerTypeUpperBound + 1
};

LayerTypeTag intToLayerTypeTag(int layerType); 
LayerTypeTag macroStringToLayerTypeTag(const CString& layerTypeString); 
LayerTypeTag stringToLayerTypeTag(const CString& layerTypeString); 
CString layerTypeToString(int layerType);
CString layerTypeToMacroString(int layerType);
CString layerTypeToDisplayString(int layerType);
LayerTypeTag getOppositeSideLayerType(LayerTypeTag layerType);

//_____________________________________________________________________________
enum LayerGroupTag
{
   layerGroupUnknown    = 0,   // LAYGROUP_UNKNOWN   0 
   layerGroupTop        = 1,   // LAYGROUP_TOP       1
   layerGroupBottom     = 2,   // LAYGROUP_BOTTOM    2
   layerGroupAll        = 3,   // LAYGROUP_ALL       3
   layerGroupInner      = 4,   // LAYGROUP_INNER     4
   layerGroupOuter      = 5,   // LAYGROUP_OUTER     5
   layerGroupPlanes     = 6,   // LAYGROUP_PLANES    6
   layerGroupDrc        = 7,   // LAYGROUP_DRC       7
   layerGroupMisc       = 8,   // LAYGROUP_MISC      8

   layerGroupLowerBound = 0,
   layerGroupUpperBound = 8,
   layerGroupUndefined  = layerGroupUpperBound + 1
};

LayerGroupTag getLayerGroupForLayerType(int layerType);
CString layerGroupToDisplayString(int layerGroup);

#endif
