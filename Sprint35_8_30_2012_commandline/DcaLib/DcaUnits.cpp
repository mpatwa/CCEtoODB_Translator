// $Header: /CAMCAD/DcaLib/DcaUnits.cpp 4     3/09/07 5:15p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaUnits.h"

#define InchesPerInch                 1.0
#define MilsPerInch                1000.0
#define MilliMetersPerInch           25.4
#define HpPlotterPerInch           1016.0
#define CentiMicroMetersPerInch 2540000.0
#define MicroMetersPerInch        25400.0
#define NanoMetersPerInch        (25400.0 * 1000.)
#define CentimetersPerInch           2.54

//_____________________________________________________________________________
double getUnitsFactor(PageUnitsTag fromUnits,PageUnitsTag toUnits)
{
   double fromUnitsPerInch = 1.;
   double toUnitsPerInch   = 1.;

   switch (fromUnits)
   {
   case pageUnitsInches:            fromUnitsPerInch = InchesPerInch;            break;
   case pageUnitsMils:              fromUnitsPerInch = MilsPerInch;              break;
   case pageUnitsMilliMeters:       fromUnitsPerInch = MilliMetersPerInch;       break;
   case pageUnitsHpPlotter:         fromUnitsPerInch = HpPlotterPerInch;         break;
   case pageUnitsCentiMicroMeters:  fromUnitsPerInch = CentiMicroMetersPerInch;  break;
   case pageUnitsMicroMeters:       fromUnitsPerInch = MicroMetersPerInch;       break;
   case pageUnitsNanoMeters:        fromUnitsPerInch = NanoMetersPerInch;        break;
   case pageUnitsCentimeters:       fromUnitsPerInch = CentimetersPerInch;       break;
   }

   switch (toUnits)
   {
   case pageUnitsInches:            toUnitsPerInch = InchesPerInch;            break;
   case pageUnitsMils:              toUnitsPerInch = MilsPerInch;              break;
   case pageUnitsMilliMeters:       toUnitsPerInch = MilliMetersPerInch;       break;
   case pageUnitsHpPlotter:         toUnitsPerInch = HpPlotterPerInch;         break;
   case pageUnitsCentiMicroMeters:  toUnitsPerInch = CentiMicroMetersPerInch;  break;
   case pageUnitsMicroMeters:       toUnitsPerInch = MicroMetersPerInch;       break;
   case pageUnitsNanoMeters:        toUnitsPerInch = NanoMetersPerInch;        break;
   case pageUnitsCentimeters:       toUnitsPerInch = CentimetersPerInch;       break;
   }

   double unitsFactor = toUnitsPerInch / fromUnitsPerInch;

   return unitsFactor;
}

//_____________________________________________________________________________
CString unitsString(int units)
{
   CString retval;

   switch (units)
   {
   case pageUnitsInches:            retval = "Inches";            break;
   case pageUnitsMils:              retval = "Mils";              break;
   case pageUnitsMilliMeters:       retval = "Millimeters";       break;
   case pageUnitsHpPlotter:         retval = "HP Plotter Units";  break;
   case pageUnitsCentiMicroMeters:  retval = "Centimicrometers";  break;
   case pageUnitsMicroMeters:       retval = "Micrometers";       break;
   case pageUnitsNanoMeters:        retval = "Nanometers";        break;
   case pageUnitsCentimeters:       retval = "Centimeters";       break;
   default:                         retval = "Unknown Units";     break;
   }

   return retval;
}

//_____________________________________________________________________________
CString unitString(int units)
{
   CString retval;

   switch (units)
   {
   case pageUnitsInches:            retval = "Inch";             break;
   case pageUnitsMils:              retval = "Mil";              break;
   case pageUnitsMilliMeters:       retval = "Millimeter";       break;
   case pageUnitsHpPlotter:         retval = "HP Plotter Unit";  break;
   case pageUnitsCentiMicroMeters:  retval = "Decananometer";    break;
   case pageUnitsMicroMeters:       retval = "Micrometer";       break;
   case pageUnitsNanoMeters:        retval = "Nanometer";        break;
   case pageUnitsCentimeters:       retval = "Centimeter";      break;
   default:                         retval = "Unknown Unit";     break;
   }

   return retval;
}

//_____________________________________________________________________________
CString unitStringAbbreviation(int units)
{
   CString retval;

   switch (units)
   {
   case pageUnitsInches:            retval = "In";            break;
   case pageUnitsMils:              retval = "Mil";           break;
   case pageUnitsMilliMeters:       retval = "MM";            break;
   case pageUnitsHpPlotter:         retval = "HP PU";         break;
   case pageUnitsCentiMicroMeters:  retval = "cum";           break;
   case pageUnitsMicroMeters:       retval = "um";            break;
   case pageUnitsNanoMeters:        retval = "nm";            break;
   case pageUnitsCentimeters:       retval = "cm";            break;
   default:                         retval = "Unknown Unit";  break;
   }

   return retval;
}

//_____________________________________________________________________________
CString PageUnitsTagToString(PageUnitsTag units)
{
   return unitsString(units);
}

//_____________________________________________________________________________
CString pageUnitsTagToString(PageUnitsTag units)
{
   return unitsString(units);
}

//_____________________________________________________________________________
PageUnitsTag intToPageUnitsTag(int pageUnits)
{
   PageUnitsTag pageUnitsTag = pageUnitsUndefined;

   if (pageUnits >= 0 && pageUnits < pageUnitsLast)
   {
      pageUnitsTag = (PageUnitsTag)pageUnits;
   }

   return pageUnitsTag;
}

//_____________________________________________________________________________
PageUnitsTag unitStringAbbreviationToTag(CString unitAbbreviation)
{
   if (unitStringAbbreviation(pageUnitsInches          ).CompareNoCase(unitAbbreviation) == 0)  return pageUnitsInches;
   if (unitStringAbbreviation(pageUnitsMils            ).CompareNoCase(unitAbbreviation) == 0)  return pageUnitsMils;
   if (unitStringAbbreviation(pageUnitsMilliMeters     ).CompareNoCase(unitAbbreviation) == 0)  return pageUnitsMilliMeters;
   if (unitStringAbbreviation(pageUnitsHpPlotter       ).CompareNoCase(unitAbbreviation) == 0)  return pageUnitsHpPlotter;
   if (unitStringAbbreviation(pageUnitsCentiMicroMeters).CompareNoCase(unitAbbreviation) == 0)  return pageUnitsCentiMicroMeters;
   if (unitStringAbbreviation(pageUnitsMicroMeters     ).CompareNoCase(unitAbbreviation) == 0)  return pageUnitsMicroMeters;
   if (unitStringAbbreviation(pageUnitsNanoMeters      ).CompareNoCase(unitAbbreviation) == 0)  return pageUnitsNanoMeters;
   if (unitStringAbbreviation(pageUnitsCentimeters     ).CompareNoCase(unitAbbreviation) == 0)  return pageUnitsCentimeters;

   return pageUnitsUndefined;
}

//_____________________________________________________________________________
PageUnitsTag unitStringToTag(const CString& unitString)
{
   return StringToPageUnitsTag(unitString);
}

PageUnitsTag StringToPageUnitsTag(const CString& unitString)
{
   // This function will cover unit string for all the following scenerio
   //  - Inches, Inch, In
   //  - Mils, Mil
   //  - Millimeters, Millimeter, MM
   //  - HP Plotter Units, HP Plotter Unit, HP Plotter, HP PU
   //  - Centimicrometers, Centimicrometer, cum
   //  - Micrometers, Micrometer, um
   //  - Nanometers, Nanometer, nm

   if (unitString.CompareNoCase("Inches") == 0 || unitString.CompareNoCase("Inch") == 0 || unitString.CompareNoCase("In") == 0)
      return pageUnitsInches;

   if (unitString.CompareNoCase("Mils") == 0 || unitString.CompareNoCase("Mil") == 0)
      return pageUnitsMils;

   if (unitString.CompareNoCase("Millimeters") == 0 || unitString.CompareNoCase("Millimeter") == 0 || unitString.CompareNoCase("MM") == 0)
      return pageUnitsMilliMeters;

   if (unitString.CompareNoCase("HP Plotter Units") == 0 || unitString.CompareNoCase("HP Plotter Unit") == 0 || unitString.CompareNoCase("HP Plotter") == 0 || unitString.CompareNoCase("HP PU") == 0)
      return pageUnitsHpPlotter;

   if (unitString.CompareNoCase("Centimicrometers") == 0 || unitString.CompareNoCase("Centimicrometer") == 0 || unitString.CompareNoCase("cum") == 0)
      return pageUnitsCentiMicroMeters;

   if (unitString.CompareNoCase("Micrometers") == 0 || unitString.CompareNoCase("Micrometer") == 0 || unitString.CompareNoCase("um") == 0)
      return pageUnitsMicroMeters;

   if (unitString.CompareNoCase("Nanometers") == 0 || unitString.CompareNoCase("Nanometer") == 0 || unitString.CompareNoCase("nm") == 0)
      return pageUnitsNanoMeters;

   if (unitString.CompareNoCase("Centimeters") == 0 || unitString.CompareNoCase("Centimeter") == 0 || unitString.CompareNoCase("cm") == 0)
      return pageUnitsCentimeters;

   return pageUnitsUndefined;
}

//_____________________________________________________________________________
bool isMetric(PageUnitsTag units)
{
   bool retval = (units == pageUnitsMilliMeters      || 
                  units == pageUnitsCentiMicroMeters || 
                  units == pageUnitsMicroMeters      ||
                  units == pageUnitsNanoMeters       ||
                  units == pageUnitsCentimeters        );

   return retval;
}

//_____________________________________________________________________________
bool isEnglish(PageUnitsTag units)
{
   bool retval = (units == pageUnitsInches    || 
                  units == pageUnitsMils      || 
                  units == pageUnitsHpPlotter   );

   return retval;
}

//_____________________________________________________________________________
bool isValidCamCadUnit(PageUnitsTag units)
{
   bool retval = (units == pageUnitsInches           || 
                  units == pageUnitsMils             || 
                  units == pageUnitsHpPlotter        ||
                  units == pageUnitsMilliMeters      || 
                  units == pageUnitsCentiMicroMeters || 
                  units == pageUnitsMicroMeters        );

   return retval;
}

//_____________________________________________________________________________
CUnits::CUnits(PageUnitsTag pageUnits)
: m_portUnits(pageUnitsUndefined)
{
   m_pageUnits = pageUnits;
}

CUnits::CUnits(const CUnits& other)
{
   m_pageUnits = other.m_pageUnits;
   m_portUnits = other.m_portUnits;
}

CUnits& CUnits::operator=(const CUnits& other)
{
   m_pageUnits = other.m_pageUnits;
   m_portUnits = other.m_portUnits;

   return *this;
}

double CUnits::convertFrom(PageUnitsTag fromUnits,double value) const
{
   return value * getUnitsFactor(fromUnits,m_pageUnits);
}

double CUnits::convertTo(PageUnitsTag toUnits,double value) const
{
   return value * getUnitsFactor(m_pageUnits,toUnits);
}

double CUnits::convertFromPortUnits(double value) const
{
   double retval = ((m_portUnits == pageUnitsUndefined) ? value : value * getUnitsFactor(m_portUnits,m_pageUnits));

   return retval;
}

double CUnits::convertToPortUnits(double value) const
{
   double retval = ((m_portUnits == pageUnitsUndefined) ? value : value * getUnitsFactor(m_pageUnits,m_portUnits));

   return retval;
}
