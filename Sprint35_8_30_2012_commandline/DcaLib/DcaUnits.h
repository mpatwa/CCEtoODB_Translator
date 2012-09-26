// $Header: /CAMCAD/DcaLib/DcaUnits.h 4     3/09/07 5:15p Kurt Van Ness $

#if !defined(__DcaUnits_h__)
#define __DcaUnits_h__

#pragma once

//_____________________________________________________________________________
enum PageUnitsTag
{
   pageUnitsInches           =  0 ,
   pageUnitsMils             =  1 ,
   pageUnitsMilliMeters      =  2 ,
   pageUnitsHpPlotter        =  3 ,
   pageUnitsCentiMicroMeters =  4 ,
   pageUnitsMicroMeters      =  5 ,
   pageUnitsNanoMeters       =  6 ,
   pageUnitsCentimeters      =  7 ,
   pageUnitsLast             =  8 ,  // used to convert int to PageUnitsTag
   pageUnitsUndefined        = -1 ,
   PageUnitsTagMin           =  0 , // \used to iterate over valid values by EnumIterator
   PageUnitsTagMax           =  7   // /
};

CString PageUnitsTagToString(PageUnitsTag units);
PageUnitsTag StringToPageUnitsTag(const CString& unitString);

double getUnitsFactor(PageUnitsTag fromUnits,PageUnitsTag toUnits);
CString unitsString(int units);
CString unitString(int units);
CString unitStringAbbreviation(int units);

CString pageUnitsTagToString(PageUnitsTag units);  // deprecated in favor of PageUnitsTagToString()
PageUnitsTag intToPageUnitsTag(int pageUnits);
PageUnitsTag unitStringAbbreviationToTag(CString unitAbbreviation);
PageUnitsTag unitStringToTag(const CString& unitString);  // deprecated in favor of StringToPageUnitsTag()
bool isMetric(PageUnitsTag units);
bool isEnglish(PageUnitsTag units);
bool isValidCamCadUnit(PageUnitsTag units);

//_____________________________________________________________________________
class CUnits : public CObject
{
private:
   PageUnitsTag m_pageUnits;
   PageUnitsTag m_portUnits;

public:
   CUnits(PageUnitsTag pageUnits);
   CUnits(const CUnits& other);
   CUnits& operator=(const CUnits& other);

	PageUnitsTag GetPageUnits() const			{ return m_pageUnits; };
   void setPageUnits(PageUnitsTag pageUnits) { m_pageUnits = pageUnits; }

	PageUnitsTag GetPortUnits() const			{ return m_portUnits; };
   void setPortUnits(PageUnitsTag portUnits) { m_portUnits = portUnits; }

   double convertFrom(PageUnitsTag fromUnits,double value) const;
   double convertTo(PageUnitsTag toUnits,double value) const;
   double convertFromPortUnits(double value) const;
   double convertToPortUnits(double value) const;
};

#endif
