// $Header: /CAMCAD/4.6/CompValues.cpp 28    5/07/07 2:12p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "stdafx.h"
#include "compvalues.h"
#include "geomlib.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// this are legal m_value units. <== UNITS_xxx
//char *value_units[] =
//{
//   "UNKNOWN",
//   "OHM",
//   "FARAD",
//   "HENRY",
//   "VOLT",
//   "AMP",
//   "WATT",
//   "HERTZ",
//   "JOULE",
//   "LUMEN",
//};

CString valueUnitToAbbreviatedString(ValueUnitTag valueUnits)
{
   const char* prefix;

   switch (valueUnits)
   {
	case valueUnitOhm:      prefix = "Ohm";  break;
	case valueUnitFarad:    prefix = "F";    break;
	case valueUnitHenry:    prefix = "H";    break;
	case valueUnitVolt:     prefix = "V";    break;
	case valueUnitAmphere:  prefix = "A";    break;
	case valueUnitWatt:     prefix = "W";    break;
	case valueUnitHertz:    prefix = "Hz";   break;
	case valueUnitJoule:    prefix = "J";    break;
	case valueUnitLumen:    prefix = "L";    break;
	case valueUnitPercent:  prefix = "%";    break;
	default:                prefix = "?";    break;
   }

   return prefix;
}

CString valueUnitToString(ValueUnitTag valueUnits)
{
   const char* prefix;

   switch (valueUnits)
   {
	case valueUnitOhm:      prefix = "Ohm";      break;
	case valueUnitFarad:    prefix = "Farad";    break;
	case valueUnitHenry:    prefix = "Henry";    break;
	case valueUnitVolt:     prefix = "Volt";     break;
	case valueUnitAmphere:  prefix = "Amphere";  break;
	case valueUnitWatt:     prefix = "Watt";     break;
	case valueUnitHertz:    prefix = "Hertz";    break;
	case valueUnitJoule:    prefix = "Joule";    break;
	case valueUnitLumen:    prefix = "Lumen";    break;
	case valueUnitPercent:  prefix = "Percent";  break;
	default:                prefix = "?";        break;
   }

   return prefix;
}

CString valueUnitToUppercaseString(ValueUnitTag valueUnits)
{
   return valueUnitToString(valueUnits).MakeUpper();
}

ValueUnitTag stringToValueUnit(CString valueUnitString)
{
   return scanValueUnit(valueUnitString);
}

ValueUnitTag scanValueUnit(CString& valueUnitString,CString* valueUnitResultString)
{
   ValueUnitTag valueUnit;

   for (valueUnit = valueUnitFirst;;valueUnit = (ValueUnitTag)(valueUnit + 1))
   {
      if (valueUnit > valueUnitLast)
      {
         if (valueUnitString.CompareNoCase("R") == 0 ||
             valueUnitString.CompareNoCase("o") == 0      )
         {
            valueUnit = valueUnitOhm;

            if (valueUnitResultString != NULL)
            {
               *valueUnitResultString = valueUnitString.Left(1);
            }
         }
         else
         {
            valueUnit = valueUnitUndefined;
         }

         break;
      }

      CString abbreviated = valueUnitToAbbreviatedString(valueUnit);

      if (valueUnitString.CompareNoCase(abbreviated) == 0)
      {
         if (valueUnitResultString != NULL)
         {
            *valueUnitResultString = valueUnitString.Left(abbreviated.GetLength());
         }

         break;
      }

      CString full = valueUnitToString(valueUnit);

      if (valueUnitString.CompareNoCase(full) == 0)
      {
         if (valueUnitResultString != NULL)
         {
            *valueUnitResultString = valueUnitString.Left(full.GetLength());
         }

         break;
      }
   }

   return valueUnit;
}

CString metricPrefixTagToAbbreviatedString(MetricPrefixTag metricPrefixTag)
{
   const char* prefix;

   switch (metricPrefixTag)
   {
   case metricPrefixAtto:   prefix = "a";  break;
   case metricPrefixFemto:  prefix = "f";  break;
   case metricPrefixPico:   prefix = "p";  break;
   case metricPrefixNano:   prefix = "n";  break;
   case metricPrefixMicro:  prefix = "u";  break;
   case metricPrefixMilli:  prefix = "m";  break;
   case metricPrefixUnity:  prefix = "";   break;
   case metricPrefixKilo:   prefix = "K";  break;
   case metricPrefixMega:   prefix = "M";  break;
   case metricPrefixGiga:   prefix = "G";  break;
   case metricPrefixTera:   prefix = "T";  break;
   case metricPrefixPeta:   prefix = "P";  break;
   case metricPrefixExa:    prefix = "E";  break;
   default:                 prefix = "X";  break;
   }

   return prefix;
}

CString metricPrefixTagToString(MetricPrefixTag metricPrefixTag)
{
   const char* prefix;

   switch (metricPrefixTag)
   {
   case metricPrefixAtto:   prefix = "atto";     break;
   case metricPrefixFemto:  prefix = "femto";    break;
   case metricPrefixPico:   prefix = "pico";     break;
   case metricPrefixNano:   prefix = "nano";     break;
   case metricPrefixMicro:  prefix = "micro";    break;
   case metricPrefixMilli:  prefix = "millo";    break;
   case metricPrefixUnity:  prefix = "";         break;
   case metricPrefixKilo:   prefix = "kilo";     break;
   case metricPrefixMega:   prefix = "mega";     break;
   case metricPrefixGiga:   prefix = "giga";     break;
   case metricPrefixTera:   prefix = "tera";     break;
   case metricPrefixPeta:   prefix = "peta";     break;
   case metricPrefixExa:    prefix = "exa";      break;
   default:                 prefix = "unknown";  break;
   }

   return prefix;
}

double metricPrefixTagToFactor(MetricPrefixTag metricPrefixTag)
{
   double factor;

   switch (metricPrefixTag)
   {
   case metricPrefixAtto:   factor = 1.0e-18;     break;
   case metricPrefixFemto:  factor = 1.0e-15;     break;
   case metricPrefixPico:   factor = 1.0e-12;     break;
   case metricPrefixNano:   factor = 1.0e-9;      break;
   case metricPrefixMicro:  factor = 1.0e-6;      break;
   case metricPrefixMilli:  factor = 1.0e-3;      break;
   case metricPrefixKilo:   factor = 1.0e3;       break;
   case metricPrefixMega:   factor = 1.0e6;       break;
   case metricPrefixGiga:   factor = 1.0e9;       break;
   case metricPrefixTera:   factor = 1.0e12;      break;
   case metricPrefixPeta:   factor = 1.0e15;      break;
   case metricPrefixExa:    factor = 1.0e18;      break;
   case metricPrefixUnity:
   default:                 factor = 1.0;         break;
   }

   return factor;
}

MetricPrefixTag stringToMetricPrefixTag(CString metricPrefixString)
{
   return scanMetricPrefixTag(metricPrefixString);
}

MetricPrefixTag scanMetricPrefixTag(CString& metricPrefixString, CString* metricPrefixResultString)
{
   // Note that metricPrefixString is both input and output.
   // As input it is the whole string we are looking at.
   // Upon output it is the remainder of the input string after the metric prefix is removed.
   // Generally that would be the units, e.g. the OHMS out of input KILOOHMS.

   MetricPrefixTag metricPrefixTag = metricPrefixUnity;
   metricPrefixString.TrimLeft();

   if (metricPrefixString.GetLength() > 2)
   {
      for (metricPrefixTag = metricPrefixAtto;;metricPrefixTag = (MetricPrefixTag)(metricPrefixTag + 3))
      {
         if (metricPrefixTag > metricPrefixExa)
         {
            metricPrefixTag = metricPrefixUnity;
            break;
         }

         CString metricPrefixTagString = metricPrefixTagToString(metricPrefixTag);

         // dts0100408178
         // Can't do a compare on whole string like this line of original code:
         //    if (metricPrefixString.CompareNoCase(metricPrefixTagString) == 0)
         // When we scan values (elswhere) we put everything after the number all together with no spaces.
         // (Actually, it fails even if the space is left in place.)
         // E.G. A compare on whole string fails to find the KILO in KILOOHM (which is real user example data).
         // It would fail for "KILO OHM" too.
         // What we really want to know is if the input string begins with one of these prefixes.
         if (!metricPrefixTagString.IsEmpty())
         {
            metricPrefixTagString.MakeUpper();
            CString metricPrefixStringUpper(metricPrefixString);
            metricPrefixStringUpper.MakeUpper();
            if (metricPrefixStringUpper.Find(metricPrefixTagString) == 0) // must start at first char
            {
               int prefixLength = metricPrefixTagString.GetLength();

               if (metricPrefixResultString != NULL)
               {
                  *metricPrefixResultString = metricPrefixString.Left(prefixLength);
               }

               metricPrefixString = metricPrefixString.Mid(prefixLength);
               break;
            }
         }
      }
   }

   if (metricPrefixTag == metricPrefixUnity)
   {
      if (!metricPrefixString.IsEmpty())
      {
         switch (metricPrefixString[0])
         {
         case 'A':
         case 'a':  metricPrefixTag = metricPrefixAtto;   break;
         case 'E':
         case 'e':  metricPrefixTag = metricPrefixExa;    break;
         case 'F':
         case 'f':  metricPrefixTag = metricPrefixFemto;  break;
         case 'G':
         case 'g':  metricPrefixTag = metricPrefixGiga;   break;
         case 'K':  
         case 'k':  metricPrefixTag = metricPrefixKilo;   break;
         case 'M':  metricPrefixTag = metricPrefixMega;   break;
         case 'm':  metricPrefixTag = metricPrefixMilli;  break;
         case 'N':
         case 'n':  metricPrefixTag = metricPrefixNano;   break;
         case 'P':  metricPrefixTag = metricPrefixPeta;   break;
         case 'p':  metricPrefixTag = metricPrefixPico;   break;
         case 'T':
         case 't':  metricPrefixTag = metricPrefixTera;   break;
         case 'µ':
         case 'U':
         case 'u':  metricPrefixTag = metricPrefixMicro;  break;
         default:   metricPrefixTag = metricPrefixUnity;  break;
         }

         if (metricPrefixResultString != NULL)
         {
            *metricPrefixResultString = metricPrefixString.Left(1);
         }

         if (metricPrefixTag != metricPrefixUnity)
         {
            metricPrefixString = metricPrefixString.Mid(1);
         }
      }
   }

   return metricPrefixTag;
}

//_____________________________________________________________________________
ComponentValues::ComponentValues(ValueUnitTag defaultUnit) :
   m_defaultUnit(defaultUnit)
{
	m_value      = 0.0;
	m_unit       = valueUnitUndefined;
	m_validValue = false;
}

ComponentValues::ComponentValues(CString compValue,ValueUnitTag defaultUnit) :
   m_defaultUnit(defaultUnit)
{
	m_value = 0.0;
	m_unit  = valueUnitUndefined;

	SetValue(compValue);
}

ComponentValues::ComponentValues(double value, CString unit)
{
	int exponent = getUnitExponent(unit);
	m_value      = value * pow((double)10, (double)exponent);
	m_unit       = parseUnits(unit);
	m_validValue = true;
}

ComponentValues::ComponentValues(double value, ValueUnitTag unit)
{
	m_value = value;
	m_unit = unit;
	m_validValue = true;
}

/******************************************************************************
* normalizeCompValue
*
   devicetype is a CAMCAD device type, which helps if the m_value has not m_unit
   return 1 for successful normalized
          0 not complete

#define  UNITS_UNKNOWN                    0                    
#define  UNITS_OHM                        1                    
#define  UNITS_FARAD                      2
#define  UNITS_HENRY                      3                    
#define  UNITS_VOLT                       4                    
#define  UNITS_AMP                        5                    
#define  UNITS_WATT                       6                    
#define  UNITS_HERTZ                      7
#define  UNITS_JOULE                      8                    
#define  UNITS_LUMEN                      9

  Must be 
      00c0
      c00

*/
bool ComponentValues::normalizeCompValue(CString valueString, double &valueNumber, ValueUnitTag &valueUnit)
{
   valueNumber = 0;
   valueUnit = valueUnitUndefined;

   valueString.TrimLeft();
   valueString.TrimRight();
   valueString.Replace('µ','u');

   if (valueString.IsEmpty())
      return false;

   bool decimalFlag = false;
   const char* p = valueString;
   CString wholeNumber,curUnit,fraction;

   // get wholeNumber
   for (;*p != '\0' && !isalpha(*p);p++)
   {
      if (*p == '.' || *p == ',') // accept period or comma as decimal point
      {
         decimalFlag = true;
         wholeNumber += '.';   // keep period or convert comma to period
      }
      else
         wholeNumber += *p;  // collect all that are not comma or period (or alpha)
   }

   // get m_unit
   for (;*p != '\0' && (isalpha(*p) || isspace(*p));p++)
   {
      if (!isspace(*p))
      {
         curUnit += *p;
      }
   }

   // get fraction
   for (;*p != '\0' && !isalpha(*p);p++)
   {
      fraction += *p;
   }

   if (*p != '\0')
   {
      return false;
   }

   CString valueNumberString = wholeNumber + (decimalFlag ? "" : ".") + fraction;
   valueNumber = atof(valueNumberString);

   CString metricPrefixString,valueUnitString;
   MetricPrefixTag metricPrefixTag = scanMetricPrefixTag(curUnit,&metricPrefixString);

   bool emptyValueUnit = curUnit.IsEmpty();

   if (emptyValueUnit)
   {
      if (metricPrefixTag == metricPrefixFemto)
      {
         metricPrefixTag = metricPrefixUnity;
         valueUnit       = valueUnitFarad;
      }
      else if (metricPrefixTag == metricPrefixAtto)
      {
         metricPrefixTag = metricPrefixUnity;
         valueUnit       = valueUnitAmphere;
      }
      else
      {
         valueUnit = m_defaultUnit;
      }
   }
   else
   {
      valueUnit = scanValueUnit(curUnit,&valueUnitString);
   }

   adjustForUpperCaseUnitString(metricPrefixString,valueUnitString,metricPrefixTag,valueUnit);

   bool retval = (valueUnit != valueUnitUndefined || emptyValueUnit);

   if (retval)
   {
      double metricFactor = metricPrefixTagToFactor(metricPrefixTag);
      valueNumber *= metricFactor;
   }

   return retval;
}

void ComponentValues::adjustForUpperCaseUnitString(const CString& metricPrefixString,
   const CString& valueUnitString,MetricPrefixTag& metricPrefixTag,ValueUnitTag valueUnit)
{
   // in case pico was P or milli was M
   
   if (metricPrefixTag == metricPrefixPeta)
   {
      // Peta vs. pico
      switch (valueUnit)
      {
	   case valueUnitOhm:      metricPrefixTag = metricPrefixPeta;  break;
	   case valueUnitFarad:    metricPrefixTag = metricPrefixPico;  break;
	   case valueUnitHenry:    metricPrefixTag = metricPrefixPico;  break;
	   case valueUnitVolt:     metricPrefixTag = metricPrefixPico;  break;
	   case valueUnitAmphere:  metricPrefixTag = metricPrefixPico;  break;
	   case valueUnitWatt:     metricPrefixTag = metricPrefixPico;  break;
	   case valueUnitHertz:    metricPrefixTag = metricPrefixPeta;  break;
	   case valueUnitJoule:    metricPrefixTag = metricPrefixPico;  break;
	   case valueUnitLumen:    metricPrefixTag = metricPrefixPico;  break;
	   case valueUnitPercent:  metricPrefixTag = metricPrefixPico;  break;
      }
   }   
   else if (metricPrefixTag == metricPrefixMega)
   {
      // Mega vs. milli
      switch (valueUnit)
      {
	   case valueUnitOhm:      metricPrefixTag = metricPrefixMega;   break;
	   case valueUnitFarad:    metricPrefixTag = metricPrefixMilli;  break;
	   case valueUnitHenry:    metricPrefixTag = metricPrefixMilli;  break;
	   case valueUnitVolt:     metricPrefixTag = metricPrefixMilli;  break;
	   case valueUnitAmphere:  metricPrefixTag = metricPrefixMilli;  break;
	   case valueUnitWatt:     metricPrefixTag = metricPrefixMilli;  break;
	   case valueUnitHertz:    metricPrefixTag = metricPrefixMega;   break;
	   case valueUnitJoule:    metricPrefixTag = metricPrefixMilli;  break;
	   case valueUnitLumen:    metricPrefixTag = metricPrefixMilli;  break;
	   case valueUnitPercent:  metricPrefixTag = metricPrefixMilli;  break;
      }
   }
}

int ComponentValues::getUnitExponent(CString unit) const
{
   MetricPrefixTag metricPrefixTag = stringToMetricPrefixTag(unit);

   return metricPrefixTag;
}

ValueUnitTag ComponentValues::parseUnits(CString unit)
{
   ValueUnitTag retval;

   if (unit.IsEmpty())
   {
      retval = m_defaultUnit;
   }
   else if ((unit.CompareNoCase("R"  ) == 0) || 
            (unit.CompareNoCase("OHM") == 0)    ) // resistor
   {
      retval = valueUnitOhm;
   }
   else
   {
      retval = stringToValueUnit(unit);
   }

   return retval;
}

bool ComponentValues::SetValue(CString compValue)
{
	m_validValue = false;

	double curValue = 0.0;
	ValueUnitTag curUnit = valueUnitUndefined;

	if (!normalizeCompValue(compValue, curValue, curUnit))
		return false;

	m_value = curValue;
	m_unit = curUnit;
	m_validValue = true;

	return true;
}

bool ComponentValues::GetValue(double &curValue, ValueUnitTag &curUnit) const
{
	curValue = m_value;
	curUnit = m_unit;
	
	return m_validValue;
}

bool ComponentValues::GetValue(double &curValue, CString &curUnit) const
{
	curValue = m_value;
	curUnit  = valueUnitToUppercaseString(m_unit);

	return m_validValue;
}

double ComponentValues::getScientificValue() const
{
   double value = 0.;

   if (IsValid())
   {
      value = m_value;
      MetricPrefixTag metricPrefix;

      scientificNormalization(value,metricPrefix);
   }

   return value;
}

MetricPrefixTag ComponentValues::getMetricPrefix() const
{
   MetricPrefixTag metricPrefix = metricPrefixUnity;

   if (IsValid())
   {
      double value = m_value;

      scientificNormalization(value,metricPrefix);
   }

   return metricPrefix;
}

bool ComponentValues::GetSmallestWholeNumber(double &newValue, MetricPrefixTag &newPrefixTag)
{	
	int exponent;
	newValue = scientificNormalization(m_value,exponent);

	switch (exponent)
	{
	case -18:	newPrefixTag = metricPrefixAtto;		break;
	case -15:	newPrefixTag = metricPrefixFemto;	break;
	case -12:	newPrefixTag = metricPrefixPico;		break;
	case -9:		newPrefixTag = metricPrefixNano;		break;
	case -6:		newPrefixTag = metricPrefixMicro;	break;
	case -3:		newPrefixTag = metricPrefixMilli;	break;
	case 3:		newPrefixTag = metricPrefixKilo;		break;
	case 6:		newPrefixTag = metricPrefixMega;		break;
	case 9:		newPrefixTag = metricPrefixGiga;		break;
	case 12:		newPrefixTag = metricPrefixTera;		break;
	case 15:		newPrefixTag = metricPrefixPeta;		break;
	case 18:		newPrefixTag = metricPrefixExa;		break;
	default:		newPrefixTag = metricPrefixUnity;	break;
	}

	return true;
}

bool ComponentValues::IsUnitEqual(ComponentValues compVal) const
{
	if (!m_validValue)
		return false;

	double value;
	ValueUnitTag unit;

	if (!compVal.GetValue(value, unit))
		return false;

	return (m_unit == unit);
}

int ComponentValues::CompareValueTo(ComponentValues compVal) const
{
	if (!m_validValue)
		return -1;	// if this value is invalid, then default to less than

	double value;
	ValueUnitTag unit;

	if (!compVal.GetValue(value, unit))
		return 1;	// if the value is invalid, then default to greater than

	double lvalue = m_value, rvalue = value;

	// if we are comparing capacitance, we need to compare by microFarads
	if (unit = valueUnitFarad)
	{
		lvalue *= 1e+6;
		rvalue *= 1e+6;
	}

	if (fabs(lvalue - rvalue) < SMALLNUMBER)
		return 0;	// equal
	else if (lvalue > rvalue)
		return 1;	// greater than
	else // (lvalue > rvalue)
		return -1;	// less than
}

CString ComponentValues::GetPrintableString(bool spaceDelim) const
{
	CString retString;

	if (!m_validValue)
		return "";

	if (!ComponentValues::GetPrintableString(m_value, m_unit, retString, spaceDelim))
   	return "";
	
	return retString;
}

bool ComponentValues::GetPrintableString(CString &str) const
{
	if (!m_validValue)
		return false;

	return ComponentValues::GetPrintableString(m_value, m_unit, str);
}

/* static */ CString ComponentValues::GetUnitName(ValueUnitTag curUnit)
{
	if (curUnit < valueUnitFirst || curUnit > valueUnitLast)
   {
		return "";
   }
   else
   {
      return valueUnitToUppercaseString(curUnit);
   }
}

/* static */ CString ComponentValues::GetPrintableString(double valueNumber, CString valueUnit)
{
	CString retString;

	if (!ComponentValues::GetPrintableString(valueNumber, valueUnit, retString))
		return "";
	
	return retString;
}

/******************************************************************************
* GetPrintableString
*
   valunit OHM,      FARAD, HENRY, VOLT, AMP, WATT, HERTZ, JOULE, LUMEN
           nothing   F      H      V     A    W     Hz     J      L
   OHM == (nothing)
      K = Kilo
      M = Mega

   FARAD = F
      p = pico e-12
      n = nano e-9
      m = milli e-6

   VOLT = 
*/
/* static */ 
bool ComponentValues::GetPrintableString(double valueNumber, CString valueUnit, CString &returnString)
{	
   return GetPrintableString(valueNumber,stringToValueUnit(valueUnit),returnString);
}

/* static */ 
bool ComponentValues::GetPrintableString(double valueNumber,ValueUnitTag valueUnit,CString &returnString, bool spaceDelim)
{	
   bool retval = true;

   MetricPrefixTag metricPrefix;
   double value = scientificNormalization(valueNumber,metricPrefix);

   CString valueType;

   if (valueUnit != valueUnitUndefined)
   {
      valueType = valueUnitToAbbreviatedString(valueUnit);
   }

   if (valueUnit == valueUnitOhm)
   {
#if !defined(IgnoreCase1226)
      valueType.Empty();
#else
      if (metricPrefix == metricPrefixKilo || metricPrefix == metricPrefixMega)
      {
         valueType.Empty();
      }
      else
      {
         valueType = " " + valueType;
      }
#endif
   }

   if (valueUnit == valueUnitPercent)
   {
      if (metricPrefix == metricPrefixMilli)
      {
         value /= 1000.;
         metricPrefix = metricPrefixUnity;
      }
      else if (metricPrefix == metricPrefixKilo)
      {
         value *= 1000.;
         metricPrefix = metricPrefixUnity;
      }
   }

   CString prefix = metricPrefixTagToAbbreviatedString(metricPrefix);

   returnString.Format("%lg%s%s%s",value,(spaceDelim ? " " : ""),prefix,valueType);

   return retval;
}

double scientificNormalization(double number,MetricPrefixTag& metricPrefix)
{
   metricPrefix = metricPrefixUnity;
   double sign = 1.;
   const double groupFactor  = 1000.;
   
   if (number < 0.)
   {
      sign = -1;
      number = -number;
   }

   while (number >= groupFactor && metricPrefix < metricPrefixExa)
   {
      number /= groupFactor;

      metricPrefix = (MetricPrefixTag)(metricPrefix + 3); 
   }

   while (number < 1.)
   {
      if (metricPrefix <= metricPrefixAtto)
      {
         metricPrefix = metricPrefixUnity;
         number = 0.;
         break;
      }

      number *= groupFactor;

      metricPrefix = (MetricPrefixTag)(metricPrefix - 3); 
   }

   return number;
}


/*****************************************************************************/
/*
   now get it into e-3,6,9,12
*/
double scientificNormalization(double real,int &exponent )
{
   exponent = 0;
   double r1 = real;
   const double groupFactor = 1000.;
   double factor = 1.;

   if (real < 1)
   {
      // minus exponent
      while (fabs(r1) < 1 && exponent < 12)
      {
         exponent += 3;
         factor *= groupFactor;
         r1 = real * factor;
      }
      // done r1 is new m_value, *exponent is a new mantissa  
      exponent *= -1;
   }
   else if (real > 1)
   {
      // plus exponent
      while (fabs(r1) >= 1000 && exponent < 6)
      {
         exponent += 3;
         factor *= groupFactor;
         r1 = real / factor;
      }
      // done r1 is new m_value, *exponent is a new mantissa        
   }
   else
   {
      exponent = 0;
   }

   return r1;
}
