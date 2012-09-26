// $Header: /CAMCAD/DcaLib/DcaGraphicClass.cpp 4     3/09/07 5:15p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaGraphicClass.h"
#include "DcaEnumIterator.h"
#include "DcaLib.h"

//_____________________________________________________________________________
GraphicClassTag intToGraphicClassTag(int candidateGrClass)
{
   // If out of range then return "undefined"
   if (candidateGrClass < graphicClassLowerBound || candidateGrClass > graphicClassUpperBound)
      return graphicClassUndefined;

   // Okay, is in range so just cast in return

   return (GraphicClassTag)candidateGrClass;
}

//_____________________________________________________________________________
CString graphicClassToString(GraphicClassTag graphicClass)
{
   const char* retval;

   switch (graphicClass)
   {
   case graphicClassNormal:              retval = "GR_CLASS_NORMAL";              break;
   case graphicClassAnalysis:            retval = "GR_CLASS_ANALYSIS";            break;
   case graphicClassAntiEtch:            retval = "GR_CLASS_ANTI_ETCH";           break;
   case graphicClassBoardGeometry:       retval = "GR_CLASS_BOARD_GEOM";          break;
   case graphicClassDrawing:             retval = "GR_CLASS_DRAWING";             break;
   case graphicClassDrc:                 retval = "GR_CLASS_DRC";                 break;
   case graphicClassEtch:                retval = "GR_CLASS_ETCH";                break;                                
   case graphicClassManufacturing:       retval = "GR_CLASS_MANUFACTURING";       break;
   case graphicClassRouteKeepIn:         retval = "GR_CLASS_ROUTKEEPIN";          break;
   case graphicClassRouteKeepOut:        retval = "GR_CLASS_ROUTKEEPOUT";         break;
   case graphicClassPlaceKeepIn:         retval = "GR_CLASS_PLACEKEEPIN";         break;
   case graphicClassPlaceKeepOut:        retval = "GR_CLASS_PLACEKEEPOUT";        break;
   case graphicClassViaKeepIn:           retval = "GR_CLASS_VIAKEEPIN";           break;
   case graphicClassViaKeepOut:          retval = "GR_CLASS_VIAKEEPOUT";          break;
   case graphicClassAllKeepIn:           retval = "GR_CLASS_ALLKEEPIN";           break;
   case graphicClassAllKeepOut:          retval = "GR_CLASS_ALLKEEPOUT";          break;
   case graphicClassBoardOutline:        retval = "GR_CLASS_BOARDOUTLINE";        break;
   case graphicClassComponentOutline:    retval = "GR_CLASS_COMPOUTLINE";         break;
   case graphicClassPanelOutline:        retval = "GR_CLASS_PANELOUTLINE";        break;
   case graphicClassMillingPath:         retval = "GR_CLASS_MILLINGPATH";         break;
   case graphicClassUnroute:             retval = "GR_CLASS_UNROUTE";             break;
   case graphicClassTraceFanout:         retval = "GR_CLASS_TRACEFANOUT";         break;
   case graphicClassPanelCutout:         retval = "GR_CLASS_PANELCUTOUT";         break;
   case graphicClassBoardCutout:         retval = "GR_CLASS_BOARDCUTOUT";         break;
   case graphicClassTestPointKeepOut:    retval = "GR_CLASS_TESTPOINTKEEPOUT";    break;
   case graphicClassTestPointKeepIn:     retval = "GR_CLASS_TESTPOINTKEEPIN";     break;
   case graphicClassComponentBoundary:   retval = "GR_CLASS_COMPONENT_BOUNDARY";  break;
   case graphicClassPlacementRegion:     retval = "GR_CLASS_PLACEMENT_REGION";    break;
   case graphicClassPackageOutline:      retval = "GR_CLASS_PACKAGEOUTLINE";      break;
   case graphicClassPackageBody:         retval = "GR_CLASS_PACKAGEBODY";         break;
   case graphicClassPackagePin:          retval = "GR_CLASS_PACKAGEPIN";          break;
   case graphicClassSignal:              retval = "GR_CLASS_SIGNAL";              break;
   case graphicClassBus:                 retval = "GR_CLASS_BUS";                 break;
   case graphicClassPanelBorder:         retval = "GR_CLASS_PANELBORDER";         break;
   case graphicClassManufacturingOutl:   retval = "GR_CLASS_MANUFACTURINGOUTL";   break;
   case graphicClassContour:             retval = "GR_CLASS_CONTOUR";             break;
   case graphicClassPlaneKeepout:        retval = "GR_CLASS_PLANEKEEPOUT";        break;
   case graphicClassPlaneNoConnect:      retval = "GR_CLASS_PLANENOCONNECT";      break;
   case graphicClassBoardKeepout:        retval = "GR_CLASS_BOARDKEEPOUT";        break;
   case graphicClassPadKeepout:          retval = "GR_CLASS_PADKEEPOUT";          break;
   case graphicClassEPBody:              retval = "GR_CLASS_EPBODY";              break;
   case graphicClassEPMask:              retval = "GR_CLASS_EPMASK";              break;
   case graphicClassEPOverGlaze:         retval = "GR_CLASS_EPOVERGLAZE";         break;
   case graphicClassEPDielectric:        retval = "GR_CLASS_EPDIELECTRIC";        break;
   case graphicClassEPConductive:        retval = "GR_CLASS_EPCONDUCTIVE";        break;
   case graphicClassBusPath:             retval = "GR_CLASS_BUSPATH";             break;
   case graphicClassRegion:              retval = "GR_CLASS_REGION";              break;
   case graphicClassCavity:              retval = "GR_CLASS_CAVITY";              break;

   case graphicClassUndefined:
   default:                              retval = "Undefined";                    break;
   }

   return CString(retval);
}

// This is the one to use to convert to Display String
CString graphicClassTagToDisplayString(GraphicClassTag graphicClass)
{
   const char* retval;

   switch (graphicClass)
   {
   case graphicClassNormal:              retval = "Normal";                   break;
   case graphicClassAnalysis:            retval = "Analysis";                 break;
   case graphicClassAntiEtch:            retval = "Anti-Etch";                break;
   case graphicClassBoardGeometry:       retval = "Board Geometry";           break;
   case graphicClassDrawing:             retval = "Drawing";                  break;
   case graphicClassDrc:                 retval = "DRC";                      break;
   case graphicClassEtch:                retval = "Etch";                     break;                                
   case graphicClassManufacturing:       retval = "Manufacturing";            break;
   case graphicClassRouteKeepIn:         retval = "Route Keep-In";            break;
   case graphicClassRouteKeepOut:        retval = "Route Keep-Out";           break;
   case graphicClassPlaceKeepIn:         retval = "Place Keep-In";            break;
   case graphicClassPlaceKeepOut:        retval = "Place Keep-Out";           break;
   case graphicClassViaKeepIn:           retval = "Via Keep-In";              break;
   case graphicClassViaKeepOut:          retval = "Via Keep-Out";             break;
   case graphicClassAllKeepIn:           retval = "All Keep-In";              break;
   case graphicClassAllKeepOut:          retval = "All Keep-Out";             break;
   case graphicClassBoardOutline:        retval = "Primary Board Outline";    break;
   case graphicClassComponentOutline:    retval = "Primary Comp. Outline";    break;
   case graphicClassPanelOutline:        retval = "Primary Panel Outline";    break;
   case graphicClassMillingPath:         retval = "Milling Path";             break;
   case graphicClassUnroute:             retval = "Unroute";                  break;
   case graphicClassTraceFanout:         retval = "Trace Fanout";             break;
   case graphicClassPanelCutout:         retval = "Panel Cutout";             break;
   case graphicClassBoardCutout:         retval = "Board Cutout";             break;
   case graphicClassTestPointKeepOut:    retval = "Testpoint Keep-Out";       break;
   case graphicClassTestPointKeepIn:     retval = "Testpoint Keep-In";        break;
   case graphicClassComponentBoundary:   retval = "Component Boundary";       break;
   case graphicClassPlacementRegion:     retval = "Placement Region";         break;
   case graphicClassPackageOutline:      retval = "Package Outline";          break;
   case graphicClassPackageBody:         retval = "Package Body";             break;
   case graphicClassPackagePin:          retval = "Package Pin";              break;
   case graphicClassSignal:              retval = "Signal";                   break;
   case graphicClassBus:                 retval = "Bus";                      break;
   case graphicClassPanelBorder:         retval = "Panel Border";             break;
   case graphicClassManufacturingOutl:   retval = "Manufacturing Outline";    break;
   case graphicClassContour:             retval = "Contour";                  break;
   case graphicClassPlaneKeepout:        retval = "Plane Keepout";            break;
   case graphicClassPlaneNoConnect:      retval = "Plane Noconnect";          break;
   case graphicClassBoardKeepout:        retval = "Board Keepout";            break;
   case graphicClassPadKeepout:          retval = "Pad Keepout";              break;
   case graphicClassEPBody:              retval = "EP Body";                  break;
   case graphicClassEPMask:              retval = "EP Mask";                  break;
   case graphicClassEPOverGlaze:         retval = "EP Overglaze";             break;
   case graphicClassEPDielectric:        retval = "EP Dielectric";            break;
   case graphicClassEPConductive:        retval = "EP Conductive";            break;
   case graphicClassBusPath:             retval = "Bus Path";                 break;
   case graphicClassRegion:              retval = "Region";                   break;
   case graphicClassCavity:              retval = "Cavity";                   break;

   case graphicClassUndefined:
   default:                              retval = "Undefined";                break;
   }

   return CString(retval);
}

// Do Not Use This Except In DisplayStringToGraphicClass Function !!!
// This is for backward compatibility only. There where two "gr class name" schemes in
// CAMCAD for awhile. The DCA had one and CAMCAD had its own. Most names were the same
// but a few were different. The duplicate code and implementations were consolidated.
// All display string names being generated hence forth should come from the function
// graphicClassTagToDisplayString(), seen above.
//
// This function is used to get the old name and is to be used solely in the converter
// from string to graphicClassTag, so that both old and new names are recognized.
//
// This converter contains only those tags that had a different name.
// Tags that used the same name do not need to be here.
// This does not need to be updated when new graphic classes are added.
// It is only for backward compatibility for the few that underwent a name alteration.

static CString graphicClassTagToOtherDisplayString(GraphicClassTag graphicClass)
{
   const char* retval;

   switch (graphicClass)
   {
   case graphicClassAntiEtch:            retval = "Anti Etch";            break;
   case graphicClassRouteKeepIn:         retval = "Route Keepin";         break;
   case graphicClassRouteKeepOut:        retval = "Route Keepout";        break;
   case graphicClassPlaceKeepIn:         retval = "Place Keepin";         break;
   case graphicClassPlaceKeepOut:        retval = "Place Keepout";        break;
   case graphicClassViaKeepIn:           retval = "Via Keepin";           break;
   case graphicClassViaKeepOut:          retval = "Via Keepout";          break;
   case graphicClassAllKeepIn:           retval = "All Keepin";           break;
   case graphicClassAllKeepOut:          retval = "All Keepout";          break;
   case graphicClassBoardOutline:        retval = "Board Outline";        break;
   case graphicClassComponentOutline:    retval = "Component Outline";    break;
   case graphicClassPanelOutline:        retval = "Panel Outline";        break;
   case graphicClassTestPointKeepOut:    retval = "Testpoint Keepout";    break;
   case graphicClassTestPointKeepIn:     retval = "Testpoint Keepin";     break;

   default:                              retval = "No Other Name";         break;  // "Undefined" would not be accurate here
   }

   return CString(retval);
}

GraphicClassTag displayStringToGraphicClassTag(const CString& displayString)
{
   GraphicClassTag retval = graphicClassUndefined;

   for (EnumIterator(GraphicClassTag,graphicClassIterator);graphicClassIterator.hasNext();)
   {
      GraphicClassTag graphicClass = graphicClassIterator.getNext();

      if (graphicClassTagToDisplayString(graphicClass).CompareNoCase(displayString) == 0 ||
         graphicClassTagToOtherDisplayString(graphicClass).CompareNoCase(displayString) == 0)
      {
         retval = graphicClass;
         break;
      }
   }

   return retval;
}

//_____________________________________________________________________________
void CGraphicsClassFilter::setToDefault()
{
   addAll();
}

CString CGraphicsClassFilter::getSetString(const CString& delimeter)
{
   CString setString;

   for (EnumIterator(GraphicClassTag,graphicClassIterator);graphicClassIterator.hasNext();)
   {
      GraphicClassTag graphicClass = graphicClassIterator.getNext();

      if (contains(graphicClass))
      {
         if (! setString.IsEmpty())
         {
            setString += delimeter;
         }

         setString += quoteString(graphicClassTagToDisplayString(graphicClass));
      }
   }

   return setString;
}

void CGraphicsClassFilter::setFromString(const CString& setString,const CString& delimiterList)
{
   empty();

   CSupString setSupString(setString);
   setSupString.Trim();
   CStringArray params;

   setSupString.ParseQuote(params,delimiterList);

   for (int index = 0;index < params.GetSize();index++)
   {
      CString param = params.GetAt(index);
      GraphicClassTag graphicClass = displayStringToGraphicClassTag(param);

      add(graphicClass);
   }
}
