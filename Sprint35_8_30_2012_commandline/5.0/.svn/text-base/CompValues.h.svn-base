// $Header: /CAMCAD/4.5/CompValues.h 14    12/09/05 1:06p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#if !defined(__CompValues_h__)
#define __CompValues_h__

#pragma once

// unit definition as used in ComponentValues
enum ValueUnitTag
{
   valueUnitOhm       =  1,
   valueUnitFarad     =  2,
   valueUnitHenry     =  3,
   valueUnitVolt      =  4,
   valueUnitAmphere   =  5,
   valueUnitWatt      =  6,
   valueUnitHertz     =  7,
   valueUnitJoule     =  8,
   valueUnitLumen     =  9,
   valueUnitPercent   = 10,
   valueUnitUndefined = 11,
   valueUnitFirst     =  1,
   valueUnitLast      = valueUnitUndefined - 1,
};

CString valueUnitToAbbreviatedString(ValueUnitTag valueUnits);
CString valueUnitToString(ValueUnitTag valueUnits);
CString valueUnitToUppercaseString(ValueUnitTag valueUnits);
ValueUnitTag stringToValueUnit(CString valueUnitString);
ValueUnitTag scanValueUnit(CString& valueUnitString,CString* metricPrefixResultString=NULL);

enum MetricPrefixTag
{
   metricPrefixAtto  = -18,
   metricPrefixFemto = -15,
   metricPrefixPico  = -12,
   metricPrefixNano  =  -9,
   metricPrefixMicro =  -6,
   metricPrefixMilli =  -3,
   metricPrefixUnity =   0,
   metricPrefixKilo  =   3,
   metricPrefixMega  =   6,
   metricPrefixGiga  =   9,
   metricPrefixTera  =  12,
   metricPrefixPeta  =  15,
   metricPrefixExa   =  18,
};

CString metricPrefixTagToAbbreviatedString(MetricPrefixTag metricPrefixTag);
CString metricPrefixTagToString(MetricPrefixTag metricPrefixTag);
double metricPrefixTagToFactor(MetricPrefixTag metricPrefixTag);
MetricPrefixTag stringToMetricPrefixTag(CString metricPrefixString);
MetricPrefixTag scanMetricPrefixTag(CString& metricPrefixString,CString* metricPrefixResultString=NULL);

double scientificNormalization(double number,MetricPrefixTag& metricPrefix);

class ComponentValues
{
public:
   ComponentValues(ValueUnitTag defaultUnit=valueUnitOhm);
   ComponentValues(CString compValue,ValueUnitTag defaultUnit=valueUnitOhm);
   ComponentValues(double value, CString unit);
   ComponentValues(double value, ValueUnitTag unit);
   virtual ~ComponentValues() {};

private:
   bool m_validValue;
   double m_value;
   ValueUnitTag m_unit;
   ValueUnitTag m_defaultUnit;

   bool normalizeCompValue(CString valueString, double &valueNumber, ValueUnitTag &valueUnit);

   int getUnitExponent(CString unit) const;
   ValueUnitTag parseUnits(CString unit);
   void adjustForUpperCaseUnitString(const CString& metricPrefixString,
      const CString& valueUnitString,MetricPrefixTag& metricPrefixTag,ValueUnitTag valueUnit);

public:
   bool IsValid() const { return m_validValue; };
   double GetValue() const { return m_value; }
   ValueUnitTag GetUnits() const { return m_unit; }
   void SetUnits(ValueUnitTag units) { m_unit = units; }

   bool SetValue(CString compValue);
   void SetValue(double compValue) { m_value = compValue; }

   bool GetValue(double &curValue, ValueUnitTag &curUnit) const;
   bool GetValue(double &curValue, CString &curUnit) const;
   bool GetSmallestWholeNumber(double &newValue, MetricPrefixTag &newPrefixTag);

   bool IsUnitEqual(ComponentValues compVal) const;
   int CompareValueTo(ComponentValues compVal) const;

   double getScientificValue() const;
   MetricPrefixTag getMetricPrefix() const;

   // printable strings
   CString GetPrintableString(bool spaceDelim=false) const;
   bool GetPrintableString(CString &str) const;

   static CString GetUnitName(ValueUnitTag curUnit);
   static CString GetPrintableString(double valueNumber, CString valueUnit);
   static bool GetPrintableString(double valueNumber, CString valueUnit, CString &returnString);
   static bool GetPrintableString(double valueNumber,ValueUnitTag valueUnit, CString &returnString, bool spaceDelim=false);
};

double scientificNormalization(double real,int& exponent);

#endif
