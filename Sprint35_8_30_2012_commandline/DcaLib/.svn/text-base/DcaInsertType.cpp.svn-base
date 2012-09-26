// $Header: /CAMCAD/DcaLib/DcaInsertType.cpp 4     3/23/07 5:19a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaInsertType.h"
#include "DcaEnumIterator.h"
#include "DcaLib.h"

//_____________________________________________________________________________
InsertTypeTag intToInsertTypeTag(int insertType) 
{ 
   return ((insertType < 0 || insertType >= insertTypeUndefined) ? 
              insertTypeUndefined : (InsertTypeTag)insertType); 
}

CString insertTypeToString(InsertTypeTag insertType)
{
   const char* retval;

   switch (insertType)
   {
   case insertTypeUnknown:              retval = "INSERTTYPE_UNKNOWN";           break;
   case insertTypeVia:                  retval = "INSERTTYPE_VIA";               break;
   case insertTypePin:                  retval = "INSERTTYPE_PIN";               break;
   case insertTypePcbComponent:         retval = "INSERTTYPE_PCBCOMPONENT";      break;
   case insertTypeMechanicalComponent:  retval = "INSERTTYPE_MECHCOMPONENT";     break;
   case insertTypeGenericComponent:     retval = "INSERTTYPE_GENERICCOMPONENT";  break;
   case insertTypePcb:                  retval = "INSERTTYPE_PCB";               break;
   case insertTypeFiducial:             retval = "INSERTTYPE_FIDUCIAL";          break;
   case insertTypeDrillTool:            retval = "INSERTTYPE_TOOLING";           break;
   case insertTypeTestPoint:            retval = "INSERTTYPE_TESTPOINT";         break;
   case insertTypeFreePad:              retval = "INSERTTYPE_FREEPAD";           break;
   case insertTypeSymbol:               retval = "INSERTTYPE_SYMBOL";            break;
   case insertTypePortInstance:         retval = "INSERTTYPE_PORTINSTANCE";      break;
   case insertTypeDrillHole:            retval = "INSERTTYPE_DRILLHOLE";         break;
   case insertTypeMechanicalPin:        retval = "INSERTTYPE_MECHANICALPIN";     break;
   case insertTypeTestProbe:            retval = "INSERTTYPE_TEST_PROBE";        break;
   case insertTypeDrillSymbol:          retval = "INSERTTYPE_DRILLSYMBOL";       break;
   case insertTypeCentroid:             retval = "INSERTTYPE_CENTROID";          break;
   case insertTypeClearancePad:         retval = "INSERTTYPE_CLEARPAD";          break;
   case insertTypeThermalPad:           retval = "INSERTTYPE_RELIEFPAD";         break;
   case insertTypeObstacle:             retval = "INSERTTYPE_OBSTACLE";          break;
   case insertTypeDrcMarker:            retval = "INSERTTYPE_DRCMARKER";         break;
   case insertTypeTestAccessPoint:      retval = "INSERTTYPE_TEST_ACCESSPOINT";  break;
   case insertTypeTestPad:              retval = "INSERTTYPE_TESTPAD";           break;
   case insertTypeSchematicJunction:    retval = "INSERTTYPE_SCHEM_JUNCTION";    break;
   case insertTypeGluePoint:            retval = "INSERTTYPE_GLUEPOINT";         break;
   case insertTypeRejectMark:           retval = "INSERTTYPE_REJECTMARK";        break;
   case insertTypeXout:                 retval = "INSERTTYPE_XOUT";              break;
   case insertTypeHierarchicalSymbol:   retval = "INSERTTYPE_HIERARCHYSYMBOL";   break;
   case insertTypeSheetConnector:       retval = "INSERTTYPE_SHEETCONNECTOR";    break;
   case insertTypeTieDot:               retval = "INSERTTYPE_TIEDOT";            break;
   case insertTypeRipper:               retval = "INSERTTYPE_RIPPER";            break;
   case insertTypeGround:               retval = "INSERTTYPE_GROUND";            break;
   case insertTypeTerminator:           retval = "INSERTTYPE_TERMINATOR";        break;
   case insertTypeAperture:             retval = "INSERTTYPE_APERTURE";          break;
   case insertTypeRealPart:             retval = "INSERTTYPE_REALPART";          break;
   case insertTypePad:                  retval = "INSERTTYPE_PAD";               break;
   case insertTypePackage:              retval = "INSERTTYPE_PACKAGE";           break;
   case insertTypePackagePin:           retval = "INSERTTYPE_PACKAGEPIN";        break;
   case insertTypeStencilHole:          retval = "INSERTTYPE_STENCILHOLE";       break;
   case insertTypeComplexDrillHole:     retval = "INSERTTYPE_COMPLEXEDRILLHOLE"; break;
   case insertTypeCompositeComp:        retval = "INSERTTYPE_COMPOSITECOMP";     break;
   case insertTypeRouteTarget:          retval = "INSERTTYPE_ROUTETARGET";       break;
   case insertTypeDie:                  retval = "INSERTTYPE_DIE";               break;
   case insertTypeDiePin:               retval = "INSERTTYPE_DIEPIN";            break;
   case insertTypeBondPad:              retval = "INSERTTYPE_BONDPAD";           break;
   case insertTypeBondWire:             retval = "INSERTTYPE_BONDWIRE";          break;
   default:                             retval = "Undefined";                    break;
   }

   return CString(retval);
}

CString insertTypeToDisplayString(InsertTypeTag insertType)
{
   const char* retval;

   switch (insertType)
   {
   case insertTypeUnknown:              retval = "Unknown";               break;
   case insertTypeVia:                  retval = "Via";                   break;
   case insertTypePin:                  retval = "Pin";                   break;
   case insertTypePcbComponent:         retval = "PCB Component";         break;
   case insertTypeMechanicalComponent:  retval = "Mechanical Component";  break;
   case insertTypeGenericComponent:     retval = "Generic Component";     break;
   case insertTypePcb:                  retval = "PCB";                   break;
   case insertTypeFiducial:             retval = "Fiducial";              break;
   case insertTypeDrillTool:            retval = "Tooling";               break;
   case insertTypeTestPoint:            retval = "Test Point";            break;
   case insertTypeFreePad:              retval = "Free Pad";              break;
   case insertTypeSymbol:               retval = "Symbol";                break;
   case insertTypePortInstance:         retval = "Port Instance";         break;
   case insertTypeDrillHole:            retval = "Drill Hole";            break;
   case insertTypeMechanicalPin:        retval = "Mechanical Pin";        break;
   case insertTypeTestProbe:            retval = "Test Probe";            break;
   case insertTypeDrillSymbol:          retval = "Drill Symbol";          break;
   case insertTypeCentroid:             retval = "Centroid";              break;
   case insertTypeClearancePad:         retval = "Clearance Pad";         break;
   case insertTypeThermalPad:           retval = "Thermal Pad";           break;
   case insertTypeObstacle:             retval = "Obstacle";              break;
   case insertTypeDrcMarker:            retval = "Drc Marker";            break;
   case insertTypeTestAccessPoint:      retval = "Test Access Point";     break;
   case insertTypeTestPad:              retval = "Test Pad";              break;
   case insertTypeSchematicJunction:    retval = "Schematic Junction";    break;
   case insertTypeGluePoint:            retval = "Glue Point";            break;
   case insertTypeRejectMark:           retval = "Reject Mark";           break;
   case insertTypeXout:                 retval = "X Out";                 break;
   case insertTypeHierarchicalSymbol:   retval = "Hierarchical Symbol";   break;
   case insertTypeSheetConnector:       retval = "Sheet Connector";       break;
   case insertTypeTieDot:               retval = "Tie Dot";               break;
   case insertTypeRipper:               retval = "Ripper";                break;
   case insertTypeGround:               retval = "Ground";                break;
   case insertTypeTerminator:           retval = "Terminator";            break;
   case insertTypeAperture:             retval = "Aperture";              break;
   case insertTypeRealPart:             retval = "Real Part";             break;
   case insertTypePad:                  retval = "Pad";                   break;
   case insertTypePackage:              retval = "Package";               break;
   case insertTypePackagePin:           retval = "Package Pin";           break;
   case insertTypeStencilHole:          retval = "Stencil Hole";          break;
   case insertTypeComplexDrillHole:     retval = "Complex Drill Hole";    break;
   case insertTypeCompositeComp:        retval = "Composite Component";   break;
   case insertTypeRouteTarget:          retval = "Route Target";          break;
   case insertTypeDie:                  retval = "Die";                   break;
   case insertTypeDiePin:               retval = "Die Pin";               break;
   case insertTypeBondPad:              retval = "Bond Pad";              break;
   case insertTypeBondWire:             retval = "Bond Wire";             break;
   default:                             retval = "Undefined";             break;
   }

   return CString(retval);
}

InsertTypeTag insertDisplayStringToTypeTag(CString insertDisplayString)
{
   if (insertDisplayString.CompareNoCase("Unknown"             ) == 0) return insertTypeUnknown; 
   if (insertDisplayString.CompareNoCase("Via"                 ) == 0) return insertTypeVia; 
   if (insertDisplayString.CompareNoCase("Pin"                 ) == 0) return insertTypePin; 
   if (insertDisplayString.CompareNoCase("PCB Component"       ) == 0) return insertTypePcbComponent; 
   if (insertDisplayString.CompareNoCase("Mechanical Component") == 0) return insertTypeMechanicalComponent; 
   if (insertDisplayString.CompareNoCase("Generic Component"   ) == 0) return insertTypeGenericComponent; 
   if (insertDisplayString.CompareNoCase("PCB"                 ) == 0) return insertTypePcb; 
   if (insertDisplayString.CompareNoCase("Fiducial"            ) == 0) return insertTypeFiducial; 
   if (insertDisplayString.CompareNoCase("Tooling"             ) == 0) return insertTypeDrillTool; 
   if (insertDisplayString.CompareNoCase("Test Point"          ) == 0) return insertTypeTestPoint; 
   if (insertDisplayString.CompareNoCase("Free Pad"            ) == 0) return insertTypeFreePad; 
   if (insertDisplayString.CompareNoCase("Symbol"              ) == 0) return insertTypeSymbol; 
   if (insertDisplayString.CompareNoCase("Port Instance"       ) == 0) return insertTypePortInstance; 
   if (insertDisplayString.CompareNoCase("Drill Hole"          ) == 0) return insertTypeDrillHole; 
   if (insertDisplayString.CompareNoCase("Complex Drill Hole"  ) == 0) return	insertTypeComplexDrillHole; 
   if (insertDisplayString.CompareNoCase("Mechanical Pin"      ) == 0) return insertTypeMechanicalPin; 
   if (insertDisplayString.CompareNoCase("Test Probe"          ) == 0) return insertTypeTestProbe;
   if (insertDisplayString.CompareNoCase("Drill Symbol"        ) == 0) return insertTypeDrillSymbol; 
   if (insertDisplayString.CompareNoCase("Centroid"            ) == 0) return insertTypeCentroid;             
   if (insertDisplayString.CompareNoCase("Clearance Pad"       ) == 0) return insertTypeClearancePad;         
   if (insertDisplayString.CompareNoCase("Thermal Pad"         ) == 0) return insertTypeThermalPad;           
   if (insertDisplayString.CompareNoCase("Obstacle"            ) == 0) return insertTypeObstacle;            
   if (insertDisplayString.CompareNoCase("Drc Marker"          ) == 0) return insertTypeDrcMarker;            
   if (insertDisplayString.CompareNoCase("Test Access Point"   ) == 0) return insertTypeTestAccessPoint;      
   if (insertDisplayString.CompareNoCase("Test Pad"            ) == 0) return insertTypeTestPad;    
   if (insertDisplayString.CompareNoCase("Schematic Junction"  ) == 0) return insertTypeSchematicJunction;    
   if (insertDisplayString.CompareNoCase("Glue Point"          ) == 0) return insertTypeGluePoint;            
   if (insertDisplayString.CompareNoCase("Reject Mark"         ) == 0) return insertTypeRejectMark;           
   if (insertDisplayString.CompareNoCase("X Out"               ) == 0) return insertTypeXout;                 
   if (insertDisplayString.CompareNoCase("Hierarchical Symbol" ) == 0) return insertTypeHierarchicalSymbol;   
   if (insertDisplayString.CompareNoCase("Sheet Connector"     ) == 0) return insertTypeSheetConnector;       
   if (insertDisplayString.CompareNoCase("Tie Dot"             ) == 0) return insertTypeTieDot;               
   if (insertDisplayString.CompareNoCase("Ripper"              ) == 0) return insertTypeRipper;               
   if (insertDisplayString.CompareNoCase("Ground"              ) == 0) return insertTypeGround;               
   if (insertDisplayString.CompareNoCase("Terminator"          ) == 0) return insertTypeTerminator;           
   if (insertDisplayString.CompareNoCase("Aperture"            ) == 0) return insertTypeAperture;             
   if (insertDisplayString.CompareNoCase("Real Part"           ) == 0) return insertTypeRealPart;            
   if (insertDisplayString.CompareNoCase("Pad"                 ) == 0) return insertTypePad;                  
   if (insertDisplayString.CompareNoCase("Package"             ) == 0) return insertTypePackage;              
   if (insertDisplayString.CompareNoCase("Package Pin"         ) == 0) return insertTypePackagePin;           
   if (insertDisplayString.CompareNoCase("Stencil Hole"        ) == 0) return insertTypeStencilHole;
   if (insertDisplayString.CompareNoCase("Complex Drill Hole"  ) == 0) return insertTypeComplexDrillHole;
   if (insertDisplayString.CompareNoCase("Composite Component" ) == 0) return insertTypeCompositeComp;
   if (insertDisplayString.CompareNoCase("Route Target"        ) == 0) return insertTypeRouteTarget;

   if (insertDisplayString.CompareNoCase("Die"                 ) == 0) return insertTypeDie;
   if (insertDisplayString.CompareNoCase("Die Pin"             ) == 0) return insertTypeDiePin;
   if (insertDisplayString.CompareNoCase("Bond Pad"            ) == 0) return insertTypeBondPad;
   if (insertDisplayString.CompareNoCase("Bond Wire"           ) == 0) return insertTypeBondWire;

   return insertTypeUndefined;
}

//_____________________________________________________________________________
void CInsertTypeFilter::setToDefault()
{
   addAll();
   remove(insertTypeThermalPad);
}

CString CInsertTypeFilter::getSetString(const CString& delimeter)
{
   CString setString;

   for (EnumIterator(InsertTypeTag,insertTypeIterator);insertTypeIterator.hasNext();)
   {
      InsertTypeTag insertType = insertTypeIterator.getNext();

      if (contains(insertType))
      {
         if (! setString.IsEmpty())
         {
            setString += delimeter;
         }

         setString += quoteString(insertTypeToDisplayString(insertType));
      }
   }

   return setString;
}

void CInsertTypeFilter::setFromString(const CString& setString,const CString& delimiterList)
{
   empty();

   CSupString setSupString(setString);
   setSupString.Trim();
   CStringArray params;

   setSupString.ParseQuote(params,delimiterList);

   for (int index = 0;index < params.GetSize();index++)
   {
      CString param = params.GetAt(index);
      InsertTypeTag insertType = insertDisplayStringToTypeTag(param);

      add(insertType);
   }
}

//_____________________________________________________________________________
CInsertTypeMask::CInsertTypeMask()
: CMask64()
{
}

CInsertTypeMask::CInsertTypeMask(const CInsertTypeMask& other)
: CMask64(other)
{
}

CInsertTypeMask::CInsertTypeMask(InsertTypeTag insertType)
: CMask64(insertType)
{
}

CInsertTypeMask::CInsertTypeMask(InsertTypeTag insertType1,InsertTypeTag insertType2)
: CMask64(insertType1,insertType2)
{
}

CInsertTypeMask::CInsertTypeMask(InsertTypeTag insertType1,InsertTypeTag insertType2,InsertTypeTag insertType3)
: CMask64(insertType1,insertType2,insertType3)
{
}

CInsertTypeMask::CInsertTypeMask(InsertTypeTag insertType1,InsertTypeTag insertType2,InsertTypeTag insertType3,InsertTypeTag insertType4)
: CMask64(insertType1,insertType2,insertType3,insertType4)
{
}

CInsertTypeMask::CInsertTypeMask(InsertTypeTag insertType1,InsertTypeTag insertType2,InsertTypeTag insertType3,InsertTypeTag insertType4,InsertTypeTag insertType5)
: CMask64(insertType1,insertType2,insertType3,insertType4,insertType5)
{
}

CInsertTypeMask::~CInsertTypeMask()
{
}

CInsertTypeMask& CInsertTypeMask::operator=(const CInsertTypeMask& other)
{
   CMask64::operator=(other);

   return *this;
}

