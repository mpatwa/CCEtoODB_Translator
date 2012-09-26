// $Header: /CAMCAD/5.0/Units.h 17    12/10/06 3:57p Kurt Van Ness $

#if ! defined (__Units_h__)
#define __Units_h__

#pragma once

CString formatUnits(double inches,int units);

#include "DcaUnits.h"

//_________________________________________________________________________________________________
class CUnitInfo
{
private:
   CString m_name;
   int m_decimals;

public:
   CUnitInfo(const CString& name,int decimals);

   CString getName() const;

   int getDecimals() const;
   void setDecimals(int decimals);
};

//_________________________________________________________________________________________________
class CUnitsInfo
{
private:
   static CUnitInfo** m_unitInfo;

public:
   static void init();
   static void release();

   static bool isValidUnit(int pageUnits);

   static CUnitInfo* getDefinedUnitInfo(int pageUnits);
   static CUnitInfo* getUnitInfo(int pageUnits);

   static int getDecimals(int pageUnits);
   static void setDecimals(int pageUnits,int decimals);

   static CString getUnitName(int pageUnits);
   static int getUnitIndex(const CString& unitName);

   static CString formatUnits(double number,int pageUnits);

private:
   PageUnitsTag m_pageUnits;

public:
   CUnitsInfo(PageUnitsTag pageUnits);

   CString formatUnits(double number);
};


#endif
