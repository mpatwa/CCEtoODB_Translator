// $Header: /CAMCAD/5.0/Units.cpp 28    12/10/06 3:57p Kurt Van Ness $

/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-6. All Rights Reserved.
*/           

#include "StdAfx.h"
#include "DbUtil.h"
#include "Units.h"

static double U[pageUnitsLast][pageUnitsLast];

struct UnitStruct 
{
   char *name;
   int decimals;
};

//static UnitStruct Units[MAX_UNITS];

int GetDecimals(int i)
{ 
   //return Units[i].decimals; 
   return CUnitsInfo::getDecimals(i);
}

void SetDecimals(int i, int decimals)
{ 
   //Units[i].decimals = decimals; 
   return CUnitsInfo::setDecimals(i,decimals);
}

const char* GetUnitName(int i)
{ 
   //return Units[i].name; 
   return CUnitsInfo::getUnitName(i);
}

int GetUnitIndex(const char *nm)
{ 
   //int   i;

   //for (i=0;i<MAX_UNITS;i++)
   //{
   //   if (!strcmpi(Units[i].name, nm))
   //      return i;
   //}

   //return -1; 
   return CUnitsInfo::getUnitIndex(nm);
}

double Units_Factor(int fromUnits, int toUnits)
{ 
   return U[fromUnits][toUnits]; 
}

CString formatUnits(double value,int units)
{
   //CString retval;

   //retval.Format("%1.*f",Units[units].decimals,value);

   //return retval;
   return CUnitsInfo::formatUnits(value,units);
}

void init_Units()
{
   //Units[pageUnitsInches          ].name = "Inches";
   //Units[pageUnitsMils            ].name = "Mils";
   //Units[pageUnitsMilliMeters     ].name = "mm";
   //Units[pageUnitsHpPlotter       ].name = "HP Plotter";
   //Units[pageUnitsCentiMicroMeters].name = "cum";
   //Units[pageUnitsMicroMeters     ].name = "um";
   //Units[pageUnitsNanoMeters      ].name = "nm";
   //Units[pageUnitsCentimeters     ].name = "cm";

   //// unit format
   //Units[pageUnitsInches          ].decimals = 3;
   //Units[pageUnitsMils            ].decimals = 0;
   //Units[pageUnitsMilliMeters     ].decimals = 2;
   //Units[pageUnitsHpPlotter       ].decimals = 0;
   //Units[pageUnitsCentiMicroMeters].decimals = 0;
   //Units[pageUnitsMicroMeters     ].decimals = 0;
   //Units[pageUnitsNanoMeters      ].decimals = 0;
   //Units[pageUnitsCentimeters     ].decimals = 3;

   U[pageUnitsInches   ][pageUnitsInches          ] =       1.0;
   U[pageUnitsInches   ][pageUnitsMils            ] =    1000.0;
   U[pageUnitsInches   ][pageUnitsMilliMeters     ] =      25.4;
   U[pageUnitsInches   ][pageUnitsHpPlotter       ] =    1016.0;
   U[pageUnitsInches   ][pageUnitsCentiMicroMeters] = 2540000.0;
   U[pageUnitsInches   ][pageUnitsMicroMeters     ] =   25400.0;
   U[pageUnitsInches   ][pageUnitsNanoMeters      ] =   25400.0 * 1000.;
   U[pageUnitsInches   ][pageUnitsCentimeters     ] =     2.54;

   U[pageUnitsMils     ][pageUnitsInches          ] =       0.001;
   U[pageUnitsMils     ][pageUnitsMils            ] =       1.0;
   U[pageUnitsMils     ][pageUnitsMilliMeters     ] =       0.0254;
   U[pageUnitsMils     ][pageUnitsHpPlotter       ] =       1.016;
   U[pageUnitsMils     ][pageUnitsCentiMicroMeters] =    2540.0;
   U[pageUnitsMils     ][pageUnitsMicroMeters     ] =      25.4;
   U[pageUnitsMils     ][pageUnitsNanoMeters      ] =   25400.0;
   U[pageUnitsMils     ][pageUnitsCentimeters     ] =   0.00254;

   U[pageUnitsMilliMeters       ][pageUnitsInches          ] =       1.0 / 25.4;
   U[pageUnitsMilliMeters       ][pageUnitsMils            ] =    1000.0 / 25.4;
   U[pageUnitsMilliMeters       ][pageUnitsMilliMeters     ] =       1.0;
   U[pageUnitsMilliMeters       ][pageUnitsHpPlotter       ] =    1016.0 / 25.4;
   U[pageUnitsMilliMeters       ][pageUnitsCentiMicroMeters] =  100000.0;
   U[pageUnitsMilliMeters       ][pageUnitsMicroMeters     ] =     1000.0;
   U[pageUnitsMilliMeters       ][pageUnitsNanoMeters      ] =     1000.0 * 1000.;
   U[pageUnitsMilliMeters       ][pageUnitsCentimeters     ] =     0.1;

   U[pageUnitsHpPlotter][pageUnitsInches          ] =        1.0 / 1016.0;
   U[pageUnitsHpPlotter][pageUnitsMils            ] =     1000.0 / 1016.0;
   U[pageUnitsHpPlotter][pageUnitsMilliMeters     ] =       25.4 / 1016.0;
   U[pageUnitsHpPlotter][pageUnitsHpPlotter       ] =        1.0;
   U[pageUnitsHpPlotter][pageUnitsCentiMicroMeters] =   254000.0 / 1016.0;
   U[pageUnitsHpPlotter][pageUnitsMicroMeters     ] =    25400.0 / 1016.0;
   U[pageUnitsHpPlotter][pageUnitsNanoMeters      ] = 25400000.0 / 1016.0;
   U[pageUnitsHpPlotter][pageUnitsCentimeters     ] =       2.54 / 1016.0;

   U[pageUnitsCentiMicroMeters  ][pageUnitsInches          ] =       1.0 / 2540000.0;
   U[pageUnitsCentiMicroMeters  ][pageUnitsMils            ] =       1.0 / 2540.0;
   U[pageUnitsCentiMicroMeters  ][pageUnitsMilliMeters     ] =       1.0 / 100000.0;
   U[pageUnitsCentiMicroMeters  ][pageUnitsHpPlotter       ] =    1016.0 / 2540000.0;
   U[pageUnitsCentiMicroMeters  ][pageUnitsCentiMicroMeters] =       1.0;
   U[pageUnitsCentiMicroMeters  ][pageUnitsMicroMeters     ] =       1.0 / 100.0;
   U[pageUnitsCentiMicroMeters  ][pageUnitsNanoMeters      ] =      10.0;
   U[pageUnitsCentiMicroMeters  ][pageUnitsCentimeters     ] =       0.1 / 100000.0;

   U[pageUnitsMicroMeters  ][pageUnitsInches          ] =       1.0 / 25400.0;
   U[pageUnitsMicroMeters  ][pageUnitsMils            ] =       1.0 / 25.4;
   U[pageUnitsMicroMeters  ][pageUnitsMilliMeters     ] =       0.001;
   U[pageUnitsMicroMeters  ][pageUnitsHpPlotter       ] =    1016.0 / 25400.0; 
   U[pageUnitsMicroMeters  ][pageUnitsCentiMicroMeters] =     100.0;
   U[pageUnitsMicroMeters  ][pageUnitsMicroMeters     ] =       1.0;
   U[pageUnitsMicroMeters  ][pageUnitsNanoMeters      ] =    1000.0;
   U[pageUnitsMicroMeters  ][pageUnitsCentimeters     ] =       0.0001;

   U[pageUnitsNanoMeters  ][pageUnitsInches          ] =       1.0 / 25400000.0;
   U[pageUnitsNanoMeters  ][pageUnitsMils            ] =       1.0 / 25400.;
   U[pageUnitsNanoMeters  ][pageUnitsMilliMeters     ] =       1.0 / 1000000.0;
   U[pageUnitsNanoMeters  ][pageUnitsHpPlotter       ] =    1016.0 / 25400000.0; 
   U[pageUnitsNanoMeters  ][pageUnitsCentiMicroMeters] =        .1;
   U[pageUnitsNanoMeters  ][pageUnitsMicroMeters     ] =        .001;
   U[pageUnitsNanoMeters  ][pageUnitsNanoMeters      ] =       1.0;
   U[pageUnitsNanoMeters  ][pageUnitsCentimeters      ] =      0.1 / 1000000.0;;

   U[pageUnitsCentimeters       ][pageUnitsInches          ] =       1.0 / 2.54;
   U[pageUnitsCentimeters       ][pageUnitsMils            ] =    1000.0 / 2.54;
   U[pageUnitsCentimeters       ][pageUnitsMilliMeters     ] =       10.0;
   U[pageUnitsCentimeters       ][pageUnitsHpPlotter       ] =    1016.0 / 2.54;
   U[pageUnitsCentimeters       ][pageUnitsCentiMicroMeters] =   1000000.0;
   U[pageUnitsCentimeters       ][pageUnitsMicroMeters     ] =     10000.0;
   U[pageUnitsCentimeters       ][pageUnitsNanoMeters      ] =     10000.0 * 1000.;
   U[pageUnitsCentimeters       ][pageUnitsCentimeters     ] =     1;
}

//_________________________________________________________________________________________________
CUnitInfo::CUnitInfo(const CString& name,int decimals)
: m_name(name)
, m_decimals(decimals)
{
}

CString CUnitInfo::getName() const
{
   return m_name;
}

int CUnitInfo::getDecimals() const
{
   return m_decimals;
}

void CUnitInfo::setDecimals(int decimals)
{
   m_decimals = decimals;
}

//_________________________________________________________________________________________________
CUnitInfo** CUnitsInfo::m_unitInfo = NULL;

void CUnitsInfo::init()
{
   if (m_unitInfo == NULL)
   {
      m_unitInfo = new (CUnitInfo(*[PageUnitsTagMax + 1]));

      m_unitInfo[pageUnitsInches          ] = new CUnitInfo("Inches"    ,3);
      m_unitInfo[pageUnitsMils            ] = new CUnitInfo("Mils"      ,0);
      m_unitInfo[pageUnitsMilliMeters     ] = new CUnitInfo("mm"        ,2);
      m_unitInfo[pageUnitsHpPlotter       ] = new CUnitInfo("HP Plotter",0);
      m_unitInfo[pageUnitsCentiMicroMeters] = new CUnitInfo("cum"       ,0);
      m_unitInfo[pageUnitsMicroMeters     ] = new CUnitInfo("um"        ,0);
      m_unitInfo[pageUnitsNanoMeters      ] = new CUnitInfo("nm"        ,0);
      m_unitInfo[pageUnitsCentimeters     ] = new CUnitInfo("cm"        ,3);
   }
}

void CUnitsInfo::release()
{
   if (m_unitInfo != NULL)
   {
      for (int pageUnit = PageUnitsTagMin;pageUnit <= PageUnitsTagMax;pageUnit++)
      {
         delete m_unitInfo[pageUnit];
         m_unitInfo[pageUnit] = NULL;
      }

      delete [] m_unitInfo;
      m_unitInfo = NULL;
   }
}

bool CUnitsInfo::isValidUnit(int pageUnits)
{
   bool retval = (pageUnits >= PageUnitsTagMin && pageUnits <= PageUnitsTagMax);

   return retval;
}

CUnitInfo* CUnitsInfo::getDefinedUnitInfo(int pageUnits)
{
   init();

   if (pageUnits < PageUnitsTagMin)
   {
      pageUnits = PageUnitsTagMin;
   }
   else if (pageUnits > PageUnitsTagMax)
   {
      pageUnits = PageUnitsTagMax;
   }

   CUnitInfo* unitInfo = m_unitInfo[pageUnits];

   return unitInfo;
}

CUnitInfo* CUnitsInfo::getUnitInfo(int pageUnits)
{
   CUnitInfo* unitInfo = NULL;

   init();

   if (isValidUnit(pageUnits))
   {
      unitInfo = m_unitInfo[pageUnits];
   }

   return unitInfo;
}

int CUnitsInfo::getDecimals(int pageUnits)
{
   int decimals = 0;

   CUnitInfo* unitInfo = getUnitInfo(pageUnits);

   if (unitInfo != NULL)
   {
      decimals = unitInfo->getDecimals();
   }

   return decimals;
}

void CUnitsInfo::setDecimals(int pageUnits,int decimals)
{
   CUnitInfo* unitInfo = getUnitInfo(pageUnits);

   if (unitInfo != NULL)
   {
      unitInfo->setDecimals(decimals);
   }
}

CString CUnitsInfo::getUnitName(int pageUnits)
{
   CString unitName;

   CUnitInfo* unitInfo = getUnitInfo(pageUnits);

   if (unitInfo != NULL)
   {
      unitName = unitInfo->getName();
   }

   return unitName;
}

int CUnitsInfo::getUnitIndex(const CString& unitName)
{
   int retval = -1;

   init();

   for (int pageUnit = PageUnitsTagMin;pageUnit <= PageUnitsTagMax;pageUnit++)
   {
      CUnitInfo* unitInfo = m_unitInfo[pageUnit];

      if (unitInfo->getName().CompareNoCase(unitName) == 0)
      {
         retval = pageUnit;
         break;
      }
   }

   return retval;
}

CString CUnitsInfo::formatUnits(double number,int pageUnits)
{
   CString retval;

   int decimals = getDefinedUnitInfo(pageUnits)->getDecimals();

   retval.Format("%1.*f",decimals,number);

   return retval;
}

CUnitsInfo::CUnitsInfo(PageUnitsTag pageUnits)
: m_pageUnits(pageUnits)
{
}

CString CUnitsInfo::formatUnits(double number)
{
   return formatUnits(number,m_pageUnits);
}

// end UNITS.CPP