// $Header: /CAMCAD/DcaLib/DcaLayerType.cpp 3     3/09/07 5:15p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaLayerType.h"

//_____________________________________________________________________________
LayerTypeTag intToLayerTypeTag(int intVal)
{
   LayerTypeTag retval = layerTypeUndefined;

   if (intVal >= layerTypeLowerBound && intVal <= layerTypeUpperBound)
   {
      retval = (LayerTypeTag)intVal;
   }

   return retval;
}

LayerTypeTag macroStringToLayerTypeTag(const CString& layerTypeString)
{
   LayerTypeTag layerType = layerTypeUndefined;

   for (int layerTypeIndex = layerTypeLowerBound;layerTypeIndex <= layerTypeUpperBound;layerTypeIndex++)
   {
      if (layerTypeString.CompareNoCase(layerTypeToMacroString(layerTypeIndex)) == 0)
      {
         layerType = (LayerTypeTag)layerTypeIndex;
         break;
      }
   }

   return layerType;
}

LayerTypeTag stringToLayerTypeTag(const CString& layerTypeString)
{
   LayerTypeTag layerType = layerTypeUndefined;

   for (int layerTypeIndex = layerTypeLowerBound;layerTypeIndex <= layerTypeUpperBound;layerTypeIndex++)
   {
      if (layerTypeString.CompareNoCase(layerTypeToString(layerTypeIndex)) == 0)
      {
         layerType = (LayerTypeTag)layerTypeIndex;
         break;
      }
   }

   return layerType;
}

CString layerTypeToMacroString(int layerType)
{
   const char* retval;

   switch (layerType)
   {
   case layerTypeUnknown:              retval = "LAYTYPE_UNKNOWN";              break;
   case layerTypeSignalTop:            retval = "LAYTYPE_SIGNAL_TOP";           break;
   case layerTypeSignalBottom:         retval = "LAYTYPE_SIGNAL_BOT";           break;
   case layerTypeSignalInner:          retval = "LAYTYPE_SIGNAL_INNER";         break;
   case layerTypeSignalAll:            retval = "LAYTYPE_SIGNAL_ALL";           break;
   case layerTypeDielectric:           retval = "LAYTYPE_DIELECTRIC";           break;
   case layerTypePadTop:               retval = "LAYTYPE_PAD_TOP";              break;
   case layerTypePadBottom:            retval = "LAYTYPE_PAD_BOTTOM";           break;
   case layerTypePadInner:             retval = "LAYTYPE_PAD_INNER";            break;
   case layerTypePadAll:               retval = "LAYTYPE_PAD_ALL";              break;
   case layerTypePadThermal:           retval = "LAYTYPE_PAD_THERMAL";          break;
   case layerTypePasteTop:             retval = "LAYTYPE_PASTE_TOP";            break;
   case layerTypePasteBottom:          retval = "LAYTYPE_PASTE_BOTTOM";         break;
   case layerTypeMaskTop:              retval = "LAYTYPE_MASK_TOP";             break;
   case layerTypeMaskBottom:           retval = "LAYTYPE_MASK_BOTTOM";          break;
   case layerTypeSilkTop:              retval = "LAYTYPE_SILK_TOP";             break;
   case layerTypeSilkBottom:           retval = "LAYTYPE_SILK_BOTTOM";          break;
   case layerTypePowerNegative:        retval = "LAYTYPE_POWERNEG";             break;
   case layerTypePowerPositive:        retval = "LAYTYPE_POWERPOS";             break;
   case layerTypeSplitPlane:           retval = "LAYTYPE_SPLITPLANE";           break;
   case layerTypeSignal:               retval = "LAYTYPE_SIGNAL";               break;
   case layerTypeMaskAll:              retval = "LAYTYPE_MASK_ALL";             break;
   case layerTypePasteAll:             retval = "LAYTYPE_PASTE_ALL";            break;
   case layerTypeRedLine:              retval = "LAYTYPE_REDLINE";              break;
   case layerTypeDrill:                retval = "LAYTYPE_DRILL";                break;
   case layerTypeTop:                  retval = "LAYTYPE_TOP";                  break;
   case layerTypeBottom:               retval = "LAYTYPE_BOTTOM";               break;
   case layerTypeAll:                  retval = "LAYTYPE_ALL";                  break;
   case layerTypeBoardOutline:         retval = "LAYTYPE_BOARD_OUTLINE";        break;
   case layerTypePadOuter:             retval = "LAYTYPE_PAD_OUTER";            break;
   case layerTypeSignalOuter:          retval = "LAYTYPE_SIGNAL_OUTER";         break;
   case layerTypeCriticalDrc:          retval = "LAYTYPE_DRC_CRITICAL";         break;
   case layerTypePlaneClearance:       retval = "LAYTYPE_PLANECLEARANCE";       break;
   case layerTypeComponentOutline:     retval = "LAYTYPE_COMPONENTOUTLINE";     break;
   case layerTypeComponentOutlineBot:  retval = "LAYTYPE_COMPONENTOUTLINE_BOTTOM";     break;
   case layerTypePanelOutline:         retval = "LAYTYPE_PANEL_OUTLINE";        break;
   case layerTypeComponentDftTop:      retval = "LAYTYPE_COMPONENT_DFT_TOP";    break;
   case layerTypeComponentDftBottom:   retval = "LAYTYPE_COMPONENT_DFT_BOTTOM"; break;
   case layerTypeMarginalDrc:          retval = "LAYTYPE_DRC_MARGINAL";         break;
   case layerTypeAcceptableDrc:        retval = "LAYTYPE_DRC_ACCEPTABLE";       break;
   case layerTypeFluxTop:              retval = "LAYTYPE_FLUX_TOP";             break;
   case layerTypeFluxBottom:           retval = "LAYTYPE_FLUX_BOTTOM";          break;
   case layerTypePackagePinLegTop:     retval = "LAYTYPE_PKG_PIN_LEG_TOP";      break;
   case layerTypePackagePinLegBottom:  retval = "LAYTYPE_PKG_PIN_LEG_BOT";      break;
   case layerTypePackagePinFootTop:    retval = "LAYTYPE_PKG_PIN_FOOT_TOP";     break;
   case layerTypePackagePinFootBottom: retval = "LAYTYPE_PKG_PIN_FOOT_BOT";     break;
   case layerTypeStencilTop:           retval = "LAYTYPE_STENCIL_TOP";          break;
   case layerTypeStencilBottom:        retval = "LAYTYPE_STENCIL_BOTTOM";       break;
   case layerTypeCentroidTop:          retval = "LAYTYPE_CENTROID_TOP";         break;
   case layerTypeCentroidBottom:       retval = "LAYTYPE_CENTROID_BOTTOM";      break;
   case layerTypePackageBodyTop:       retval = "LAYTYPE_PKG_BODY_TOP";         break;
   case layerTypePackageBodyBottom:    retval = "LAYTYPE_PKG_BODY_BOT";         break;
   case layerTypeDftTop:               retval = "LAYTYPE_DFT_TOP";              break;
   case layerTypeDftBottom:            retval = "LAYTYPE_DFT_BOTTOM";           break;
   case layerTypeDrillDrawingThru:     retval = "LAYTYPE_DRILL_DRAWING_THRU";   break;
   case layerTypeStackLevelTop:        retval = "LAYTYPE_STACKLEVEL_TOP";       break;
   case layerTypeStackLevelBottom:     retval = "LAYTYPE_STACKLEVEL_BOTTOM";    break;
   default:                            retval = "Undefined";                    break;
   }

   return CString(retval);
}

CString layerTypeToString(int layerType)
{
   const char* retval;

   switch (layerType)
   {
   case layerTypeUnknown:               retval = "UNKNOWN GRAPHIC";          break;
   case layerTypeSignalTop:             retval = "SIGNAL TOP";               break;
   case layerTypeSignalBottom:          retval = "SIGNAL BOTTOM";            break;
   case layerTypeSignalInner:           retval = "SIGNAL INNER";             break;
   case layerTypeSignalAll:             retval = "SIGNAL ALL";               break;
   case layerTypeDielectric:            retval = "DIELECTRIC";               break;
   case layerTypePadTop:                retval = "PAD TOP";                  break;
   case layerTypePadBottom:             retval = "PAD BOTTOM";               break;
   case layerTypePadInner:              retval = "PAD INNER";                break;
   case layerTypePadAll:                retval = "PAD ALL";                  break;
   case layerTypePadThermal:            retval = "PAD THERMAL";              break;
   case layerTypePasteTop:              retval = "PASTE TOP";                break;
   case layerTypePasteBottom:           retval = "PASTE BOTTOM";             break;
   case layerTypeMaskTop:               retval = "SOLDERMASK TOP";           break;
   case layerTypeMaskBottom:            retval = "SOLDERMASK BOTTOM";        break;
   case layerTypeSilkTop:               retval = "SILKSCREEN TOP";           break;
   case layerTypeSilkBottom:            retval = "SILKSCREEN BOTTOM";        break;
   case layerTypePowerNegative:         retval = "POWER NEGATIVE";           break;
   case layerTypePowerPositive:         retval = "POWER POSITIVE";           break;
   case layerTypeSplitPlane:            retval = "SPLIT PLANE";              break;
   case layerTypeSignal:                retval = "GENERIC SIGNAL";           break;
   case layerTypeMaskAll:               retval = "SOLDERMASK ALL";           break;
   case layerTypePasteAll:              retval = "PASTEMASK ALL";            break;
   case layerTypeRedLine:               retval = "REDLINE";                  break;
   case layerTypeDrill:                 retval = "DRILL";                    break;
   case layerTypeTop:                   retval = "GENERIC TOP";              break;
   case layerTypeBottom:                retval = "GENERIC BOTTOM";           break;
   case layerTypeAll:                   retval = "GENERIC ALL";              break;
   case layerTypeBoardOutline:          retval = "BOARD OUTLINE";            break;
   case layerTypePadOuter:              retval = "PAD OUTER";                break;
   case layerTypeSignalOuter:           retval = "SIGNAL OUTER";             break;
   case layerTypeCriticalDrc:           retval = "DRC CRITICAL";             break;
   case layerTypePlaneClearance:        retval = "PLANE CLEARANCE";          break;
   case layerTypeComponentOutline:      retval = "COMPONENT OUTLINE";        break;
   case layerTypeComponentOutlineBot:   retval = "COMPONENT OUTLINE BOTTOM"; break;
   case layerTypePanelOutline:          retval = "PANEL OUTLINE";            break;
   case layerTypeDocumentation:         retval = "DOCUMENTATION";            break;
   case layerTypeScoringLine:           retval = "SCORING LINE";             break;
   case layerTypeShearingLine:          retval = "SHEARING LINE";            break;   
   case layerTypeComponentDftTop:       retval = "COMPONENT DFT TOP";        break;
   case layerTypeComponentDftBottom:    retval = "COMPONENT DFT BOTTOM";     break;
   case layerTypeMarginalDrc:           retval = "DRC MARGINAL";             break;
   case layerTypeAcceptableDrc:         retval = "DRC ACCEPTABLE";           break;
   case layerTypeFluxTop:               retval = "FLUX TOP";                 break;
   case layerTypeFluxBottom:            retval = "FLUX BOTTOM";              break;
   case layerTypePackagePinLegTop:      retval = "PACKAGE_PIN_LEG_TOP";      break;
   case layerTypePackagePinLegBottom:   retval = "PACKAGE_PIN_LEG_BOTTOM";   break;
   case layerTypePackagePinFootTop:     retval = "PACKAGE_PIN_FOOT_TOP";     break;
   case layerTypePackagePinFootBottom:  retval = "PACKAGE_PIN_FOOT_BOTTOM";  break;
   case layerTypeStencilTop:            retval = "STENCIL TOP";              break;
   case layerTypeStencilBottom:         retval = "STENCIL BOTTOM";           break;
   case layerTypeCentroidTop:           retval = "CENTROID TOP";             break;
   case layerTypeCentroidBottom:        retval = "CENTROID BOTTOM";          break;
   case layerTypePackageBodyTop:        retval = "PACKAGE_BODY_TOP";         break;
   case layerTypePackageBodyBottom:     retval = "PACKAGE_BODY_BOTTOM";      break;
   case layerTypeDftTop:                retval = "DFT TOP";                  break;
   case layerTypeDftBottom:             retval = "DFT BOTTOM";               break;
   case layerTypeDrillDrawingThru:      retval = "DRILL DRAWING THRU";       break;
   case layerTypeStackLevelTop:         retval = "STACK LEVEL TOP";          break;
   case layerTypeStackLevelBottom:      retval = "STACK LEVEL BOTTOM";       break;
   default:                             retval = "Undefined";                break;
   }

   return CString(retval);
}

CString layerTypeToDisplayString(int layerType)
{
   const char* retval;

   switch (layerType)
   {
   case layerTypeUnknown:              retval = "Unknown";                  break;
   case layerTypeSignalTop:            retval = "Signal Top";               break;
   case layerTypeSignalBottom:         retval = "Signal Bottom";            break;
   case layerTypeSignalInner:          retval = "Signal Inner";             break;
   case layerTypeSignalAll:            retval = "Signal All";               break;
   case layerTypeDielectric:           retval = "Dielectric";               break;
   case layerTypePadTop:               retval = "Pad Top";                  break;
   case layerTypePadBottom:            retval = "Pad Bottom";               break;
   case layerTypePadInner:             retval = "Pad Inner";                break;
   case layerTypePadAll:               retval = "Pad All";                  break;
   case layerTypePadThermal:           retval = "Pad Thermal";              break;
   case layerTypePasteTop:             retval = "Paste Top";                break;
   case layerTypePasteBottom:          retval = "Paste Bottom";             break;
   case layerTypeMaskTop:              retval = "Mask Top";                 break;
   case layerTypeMaskBottom:           retval = "Mask Bottom";              break;
   case layerTypeSilkTop:              retval = "Silk Top";                 break;
   case layerTypeSilkBottom:           retval = "Silk Bottom";              break;
   case layerTypePowerNegative:        retval = "Power Negative";           break;
   case layerTypePowerPositive:        retval = "Power Positive";           break;
   case layerTypeSplitPlane:           retval = "Split Plane";              break;
   case layerTypeSignal:               retval = "Generic Signal";           break;
   case layerTypeMaskAll:              retval = "Solder Mask All";          break;
   case layerTypePasteAll:             retval = "Paste Mask All";           break;
   case layerTypeRedLine:              retval = "Redline";                  break;
   case layerTypeDrill:                retval = "Drill";                    break;
   case layerTypeDrillDrawingThru:     retval = "Drill Drawing Thru";       break;
   case layerTypeTop:                  retval = "Generic Top";              break;
   case layerTypeBottom:               retval = "Generic Bottom";           break;
   case layerTypeAll:                  retval = "Generic All";              break;
   case layerTypeBoardOutline:         retval = "Board Outline";            break;
   case layerTypePadOuter:             retval = "Pad Outer";                break;
   case layerTypeSignalOuter:          retval = "Signal Outer";             break;
   case layerTypeCriticalDrc:          retval = "DRC Critical";             break;
   case layerTypePlaneClearance:       retval = "Plane Clearance";          break;
   case layerTypeComponentOutline:     retval = "Component Outline";        break;
   case layerTypeComponentOutlineBot:  retval = "Component Outline Bottom"; break;
   case layerTypePanelOutline:         retval = "Panel Outline";            break;
   case layerTypeComponentDftTop:      retval = "Component DFT Top";        break;
   case layerTypeComponentDftBottom:   retval = "Component DFT Bottom";     break;
   case layerTypeMarginalDrc:          retval = "DRC Marginal";             break;
   case layerTypeAcceptableDrc:        retval = "DRC Acceptable";           break;
   case layerTypeFluxTop:              retval = "Flux Top";                 break;
   case layerTypeFluxBottom:           retval = "Flux Bottom";              break;
   case layerTypePackagePinLegTop:     retval = "Package Pin Leg Top";      break;
   case layerTypePackagePinLegBottom:  retval = "Package Pin Leg Bottom";   break;
   case layerTypePackagePinFootTop:    retval = "Package Pin Foot Top";     break;
   case layerTypePackagePinFootBottom: retval = "Package Pin Foot Bottom";  break;
   case layerTypeStencilTop:           retval = "Stencil Top";              break;
   case layerTypeStencilBottom:        retval = "Stencil Bottom";           break;
   case layerTypeCentroidTop:          retval = "Centroid Top";             break;
   case layerTypeCentroidBottom:       retval = "Centroid Bottom";          break;
   case layerTypePackageBodyTop:       retval = "Package Body Top";         break;
   case layerTypePackageBodyBottom:    retval = "Package Body Bottom";      break;
   case layerTypeDftTop:               retval = "DFT Top";                  break;
   case layerTypeDftBottom:            retval = "DFT Bottom";               break;
   case layerTypeStackLevelTop:        retval = "Stack Level Top";          break;
   case layerTypeStackLevelBottom:     retval = "Stack Level Bottom";       break;
   default:                            retval = "Undefined";                break;
   }

   return CString(retval);
}

LayerTypeTag getOppositeSideLayerType(LayerTypeTag layerType)
{
   LayerTypeTag oppositeSideLayerType = layerType;

   switch (layerType)
   {
   case layerTypeSignalTop:            oppositeSideLayerType = layerTypeSignalBottom;          break;
   case layerTypeSignalBottom:         oppositeSideLayerType = layerTypeSignalTop;             break;
   case layerTypePadTop:               oppositeSideLayerType = layerTypePadBottom;             break;
   case layerTypePadBottom:            oppositeSideLayerType = layerTypePadTop;                break;
   case layerTypePasteTop:             oppositeSideLayerType = layerTypePasteBottom;           break;
   case layerTypePasteBottom:          oppositeSideLayerType = layerTypePasteTop;              break;
   case layerTypeMaskTop:              oppositeSideLayerType = layerTypeMaskBottom;            break;
   case layerTypeMaskBottom:           oppositeSideLayerType = layerTypeMaskTop;               break;
   case layerTypeSilkTop:              oppositeSideLayerType = layerTypeSilkBottom;            break;
   case layerTypeSilkBottom:           oppositeSideLayerType = layerTypeSilkTop;               break;
   case layerTypeTop:                  oppositeSideLayerType = layerTypeBottom;                break;
   case layerTypeBottom:               oppositeSideLayerType = layerTypeTop;                   break;
   case layerTypeComponentDftTop:      oppositeSideLayerType = layerTypeComponentDftBottom;    break;
   case layerTypeComponentDftBottom:   oppositeSideLayerType = layerTypeComponentDftTop;       break;
   case layerTypeFluxTop:              oppositeSideLayerType = layerTypeFluxBottom;            break;
   case layerTypeFluxBottom:           oppositeSideLayerType = layerTypeFluxTop;               break;
   case layerTypePackagePinLegTop:     oppositeSideLayerType = layerTypePackagePinLegBottom;   break;
   case layerTypePackagePinLegBottom:  oppositeSideLayerType = layerTypePackagePinLegTop;      break;
   case layerTypePackagePinFootTop:    oppositeSideLayerType = layerTypePackagePinFootBottom;  break;
   case layerTypePackagePinFootBottom: oppositeSideLayerType = layerTypePackagePinFootTop;     break;
   case layerTypeStencilTop:           oppositeSideLayerType = layerTypeStencilBottom;         break;
   case layerTypeStencilBottom:        oppositeSideLayerType = layerTypeStencilTop;            break;
   case layerTypeCentroidTop:          oppositeSideLayerType = layerTypeCentroidBottom;        break;
   case layerTypeCentroidBottom:       oppositeSideLayerType = layerTypeCentroidTop;           break;
   case layerTypePackageBodyTop:       oppositeSideLayerType = layerTypePackageBodyBottom;     break;
   case layerTypePackageBodyBottom:    oppositeSideLayerType = layerTypePackageBodyTop;        break;
   case layerTypeDftTop:               oppositeSideLayerType = layerTypeDftBottom;             break;
   case layerTypeDftBottom:            oppositeSideLayerType = layerTypeDftTop;                break;
   }

   return oppositeSideLayerType;
}

//_____________________________________________________________________________
LayerGroupTag getLayerGroupForLayerType(int layerType)
{
   LayerGroupTag layerGroup = layerGroupUndefined;

   switch (layerType)
   {
   case layerTypeUnknown:              layerGroup = layerGroupMisc;     break;
   case layerTypeSignalTop:            layerGroup = layerGroupTop;      break;
   case layerTypeSignalBottom:         layerGroup = layerGroupBottom;   break;
   case layerTypeSignalInner:          layerGroup = layerGroupInner;    break;
   case layerTypeSignalAll:            layerGroup = layerGroupAll;      break;
   case layerTypeDielectric:           layerGroup = layerGroupMisc;     break;
   case layerTypePadTop:               layerGroup = layerGroupTop;      break;
   case layerTypePadBottom:            layerGroup = layerGroupBottom;   break;
   case layerTypePadInner:             layerGroup = layerGroupInner;    break;
   case layerTypePadAll:               layerGroup = layerGroupAll;      break;
   case layerTypePadThermal:           layerGroup = layerGroupPlanes;   break;
   case layerTypePasteTop:             layerGroup = layerGroupTop;      break;
   case layerTypePasteBottom:          layerGroup = layerGroupBottom;   break;
   case layerTypeMaskTop:              layerGroup = layerGroupTop;      break;
   case layerTypeMaskBottom:           layerGroup = layerGroupBottom;   break;
   case layerTypeSilkTop:              layerGroup = layerGroupTop;      break;
   case layerTypeSilkBottom:           layerGroup = layerGroupBottom;   break;
   case layerTypePowerNegative:        layerGroup = layerGroupPlanes;   break;
   case layerTypePowerPositive:        layerGroup = layerGroupPlanes;   break;
   case layerTypeSplitPlane:           layerGroup = layerGroupPlanes;   break;
   case layerTypeSignal:               layerGroup = layerGroupMisc;     break;
   case layerTypeMaskAll:              layerGroup = layerGroupAll;      break;
   case layerTypePasteAll:             layerGroup = layerGroupAll;      break;
   case layerTypeRedLine:              layerGroup = layerGroupMisc;     break;
   case layerTypeDrill:                layerGroup = layerGroupMisc;     break;
   case layerTypeTop:                  layerGroup = layerGroupTop;      break;
   case layerTypeBottom:               layerGroup = layerGroupBottom;   break;
   case layerTypeAll:                  layerGroup = layerGroupAll;      break;
   case layerTypeBoardOutline:         layerGroup = layerGroupMisc;     break;
   case layerTypePadOuter:             layerGroup = layerGroupOuter;    break;
   case layerTypeSignalOuter:          layerGroup = layerGroupOuter;    break;
   case layerTypeCriticalDrc:          layerGroup = layerGroupDrc;      break;
   case layerTypePlaneClearance:       layerGroup = layerGroupPlanes;   break;
   case layerTypeComponentOutline:     layerGroup = layerGroupMisc;     break;
   case layerTypeComponentOutlineBot:  layerGroup = layerGroupMisc;     break;
   case layerTypePanelOutline:         layerGroup = layerGroupMisc;     break;
   case layerTypeComponentDftTop:      layerGroup = layerGroupUnknown;  break;
   case layerTypeComponentDftBottom:   layerGroup = layerGroupUnknown;  break;
   case layerTypeMarginalDrc:          layerGroup = layerGroupDrc;      break;
   case layerTypeAcceptableDrc:        layerGroup = layerGroupDrc;      break;
   case layerTypeFluxTop:              layerGroup = layerGroupUnknown;  break;
   case layerTypeFluxBottom:           layerGroup = layerGroupUnknown;  break;
   case layerTypePackagePinLegTop:     layerGroup = layerGroupUnknown;  break;
   case layerTypePackagePinLegBottom:  layerGroup = layerGroupUnknown;  break;
   case layerTypePackagePinFootTop:    layerGroup = layerGroupUnknown;  break;
   case layerTypePackagePinFootBottom: layerGroup = layerGroupUnknown;  break;
   case layerTypeStencilTop:           layerGroup = layerGroupUnknown;  break;
   case layerTypeStencilBottom:        layerGroup = layerGroupUnknown;  break;
   case layerTypeCentroidTop:          layerGroup = layerGroupUnknown;  break;
   case layerTypeCentroidBottom:       layerGroup = layerGroupUnknown;  break;
   case layerTypePackageBodyTop:       layerGroup = layerGroupUnknown;  break;
   case layerTypePackageBodyBottom:    layerGroup = layerGroupUnknown;  break;
   case layerTypeDftTop:               layerGroup = layerGroupUnknown;  break;
   case layerTypeDftBottom:            layerGroup = layerGroupUnknown;  break;
   case layerTypeStackLevelTop:        layerGroup = layerGroupUnknown;  break;
   case layerTypeStackLevelBottom:     layerGroup = layerGroupUnknown;  break;
   default:                            layerGroup = layerGroupUnknown;  break;
   }

   return layerGroup;
}

CString layerGroupToDisplayString(int layerGroup)
{
   const char* retval;

   switch (layerGroup)
   {
   case layerGroupTop:     retval = "Top";            break;
   case layerGroupBottom:  retval = "Bottom";         break;
   case layerGroupAll:     retval = "All";            break;
   case layerGroupInner:   retval = "Inner";          break;
   case layerGroupOuter:   retval = "Outer";          break;
   case layerGroupPlanes:  retval = "Planes";         break;
   case layerGroupDrc:     retval = "DRC";            break;
   case layerGroupMisc:    retval = "Miscellaneous";  break;
   default:                retval = "Undefined";      break;
   }

   return CString(retval);
}
