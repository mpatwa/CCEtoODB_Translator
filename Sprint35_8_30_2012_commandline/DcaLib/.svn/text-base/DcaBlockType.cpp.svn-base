// $Header: /CAMCAD/DcaLib/DcaBlockType.cpp 3     3/09/07 5:15p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaBlockType.h"

//_____________________________________________________________________________
CString blockTypeToDisplayString(BlockTypeTag blockType)
{
   const char* retval;

   switch (blockType)
   {
   case blockTypeGeometryEdit:      retval = "Geometry Edit";         break;
   case blockTypeUnknown:           retval = "Unknown";               break;
   case blockTypePcb:               retval = "PCB Design";            break;
   case blockTypePadshape:          retval = "Pad Shape";             break;
   case blockTypePadstack:          retval = "Pad Stack";             break;
   case blockTypePcbComponent:      retval = "PCB Component";         break;
   case blockTypeMechComponent:     retval = "Mechanical Component";  break;
   case blockTypeGenericComponent:  retval = "Generic Component";     break;
   case blockTypePanel:             retval = "PCB Panel";             break;
   case blockTypeDrawing:           retval = "Drawing";               break;
   case blockTypeFiducial:          retval = "Fiducial";              break;
   case blockTypeTooling:           retval = "Tooling";               break;
   case blockTypeTestPoint:         retval = "Test Point";            break;
   case blockTypeDimension:         retval = "Dimension";             break;
   case blockTypeLibrary:           retval = "Library";               break;
   case blockTypeLocalPcbComp:      retval = "Local PCB Component";   break;
   case blockTypeToolGraphic:       retval = "Tool Graphic";          break;
   case blockTypeSheet:             retval = "Schematic Sheet";       break;
   case blockTypeSymbol:            retval = "Schematic Symbol";      break;
   case blockTypeGatePort:          retval = "Gate Port";             break;
   case blockTypeDrillHole:         retval = "Drill Hole";            break;
   case blockTypeRedLine:           retval = "Red Line";              break;
   case blockTypeTestProbe:         retval = "Test Probe";            break;
   case blockTypeCentroid:          retval = "Centroid";              break;
   case blockTypeDrcMarker:         retval = "DRC Marker";            break;
   case blockTypeGeomLibrary:       retval = "Geometry Library";      break;
   case blockTypeTestPad:           retval = "Test Pad";              break;
   case blockTypeAccessPoint:       retval = "Test Access Point";     break;
   case blockTypeSchemJunction:     retval = "Schematic Junction Point";  break;
   case blockTypeGluePoint:         retval = "Glue Point";            break;
   case blockTypeRejectMark:        retval = "Reject Mark";           break;
   case blockTypeXOut:              retval = "X Out";                 break;
   case blockTypeRealPart:          retval = "Real Part";             break;
   case blockTypePackage:           retval = "Package";               break;
   case blockTypePackagePin:        retval = "Package Pin";           break;
   case blockTypeComplexDrillHole:  retval = "Complex Drill Hole";    break;
   case blockTypeCompositeComp:     retval = "Composite Component";   break;
   case blockTypeRouteTarget:       retval = "Route Target";          break;
   case blockTypeDie:               retval = "Die";                   break;
   case blockTypeDiePin:            retval = "Die Pin";               break;
   case blockTypeBondWire:          retval = "Bond Wire";             break;
   case blockTypeBondPad:           retval = "Bond Pad";              break;

   default:                         retval = "Undefined";             break;
   }

   return CString(retval);
}

//_____________________________________________________________________________
CString blockTypeToString(int blockType)
{
   const char* retval;

   switch (blockType)
   {
   case blockTypeGeometryEdit:      retval = "BLOCKTYPE_GEOMETRY_EDIT";     break;
   case blockTypeUnknown:           retval = "BLOCKTYPE_UNKNOWN";           break;
   case blockTypePcb:               retval = "BLOCKTYPE_PCB";               break;
   case blockTypePadshape:          retval = "BLOCKTYPE_PADSHAPE";          break;
   case blockTypePadstack:          retval = "BLOCKTYPE_PADSTACK";          break;
   case blockTypePcbComponent:      retval = "BLOCKTYPE_PCBCOMPONENT";      break;
   case blockTypeMechComponent:     retval = "BLOCKTYPE_MECHCOMPONENT";     break;
   case blockTypeGenericComponent:  retval = "BLOCKTYPE_GENERICCOMPONENT";  break;
   case blockTypePanel:             retval = "BLOCKTYPE_PANEL";             break;
   case blockTypeDrawing:           retval = "BLOCKTYPE_DRAWING";           break;
   case blockTypeFiducial:          retval = "BLOCKTYPE_FIDUCIAL";          break;
   case blockTypeTooling:           retval = "BLOCKTYPE_TOOLING";           break;
   case blockTypeTestPoint:         retval = "BLOCKTYPE_TESTPOINT";         break;
   case blockTypeDimension:         retval = "BLOCKTYPE_DIMENSION";         break;
   case blockTypeLibrary:           retval = "BLOCKTYPE_LIBRARY";           break;
   case blockTypeLocalPcbComp:      retval = "BLOCKTYPE_LOCALPCBCOMP";      break;
   case blockTypeToolGraphic:       retval = "BLOCKTYPE_TOOLGRAPHIC";       break;
   case blockTypeSheet:             retval = "BLOCKTYPE_SHEET";             break;
   case blockTypeSymbol:            retval = "BLOCKTYPE_SYMBOL";            break;
   case blockTypeGatePort:          retval = "BLOCKTYPE_GATEPORT";          break;
   case blockTypeDrillHole:         retval = "BLOCKTYPE_DRILLHOLE";         break;
   case blockTypeRedLine:           retval = "BLOCKTYPE_REDLINE";           break;
   case blockTypeTestProbe:         retval = "BLOCKTYPE_TEST_PROBE";        break;
   case blockTypeCentroid:          retval = "BLOCKTYPE_CENTROID";          break;
   case blockTypeDrcMarker:         retval = "BLOCKTYPE_DRCMARKER";         break;
   case blockTypeGeomLibrary:       retval = "BLOCKTYPE_GEOM_LIBRARY";      break;
   case blockTypeTestPad:           retval = "BLOCKTYPE_TESTPAD";           break;
   case blockTypeAccessPoint:       retval = "BLOCKTYPE_TEST_ACCESSPOINT";  break;
   case blockTypeSchemJunction:     retval = "BLOCKTYPE_SCHEM_JUNCTION";    break;
   case blockTypeGluePoint:         retval = "BLOCKTYPE_GLUEPOINT";         break;
   case blockTypeRejectMark:        retval = "BLOCKTYPE_REJECTMARK";        break;
   case blockTypeXOut:              retval = "BLOCKTYPE_XOUT";              break;
   case blockTypeRealPart:          retval = "BLOCKTYPE_REALPART";          break;
   case blockTypePackage:           retval = "BLOCKTYPE_PACKAGE";           break;
   case blockTypePackagePin:        retval = "BLOCKTYPE_PACKAGEPIN";        break;
   case blockTypeComplexDrillHole:  retval = "BLOCKTYPE_COMPLEXDRILLHOLE";  break;
   case blockTypeCompositeComp:     retval = "BLOCKTYPE_COMPOSITECOMP";     break;
   case blockTypeRouteTarget:       retval = "BLOCKTYPE_ROUTETARGET";       break;
   case blockTypeDie:               retval = "BLOCKTYPE_DIE";               break;
   case blockTypeDiePin:            retval = "BLOCKTYPE_DIEPIN";            break;
   case blockTypeBondWire:          retval = "BLOCKTYPE_BONDWIRE";          break;
   case blockTypeBondPad:           retval = "BLOCKTYPE_BONDPAD";           break;

   default:                         retval = "Undefined";                   break;
   }

   return CString(retval);
}

//_____________________________________________________________________________
BlockTypeTag intToBlockTypeTag(int blockType)
{
   BlockTypeTag blockTypeTag = blockTypeUndefined;

   if (blockType >= blockTypeLowerBound &&
       blockType <= blockTypeUpperBound     )
   {
      blockTypeTag = (BlockTypeTag)blockType;
   }

   return blockTypeTag;
}
