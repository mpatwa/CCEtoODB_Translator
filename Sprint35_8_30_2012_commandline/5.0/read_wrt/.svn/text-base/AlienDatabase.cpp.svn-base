// $Header: /CAMCAD/5.0/read_wrt/AlienDatabase.cpp 19    3/12/07 12:48p Kurt Van Ness $

/*
*/

//   Router Solutions Inc.
//   Copyright © 2002. All Rights Reserved.

#include "StdAfx.h"
#include "AlienDatabase.h"

//_____________________________________________________________________________
CAlienDatabase::CAlienDatabase(CCamCadDatabase& camCadDatabase,FileTypeTag databaseTypeId)
: m_camCadDatabase(camCadDatabase)
, m_databaseTypeId(databaseTypeId)
{
   m_inchesPerUnit  = 0.;
   m_pageUnitsPerUnit = 0.;
   m_boardFile      = NULL;
   m_panelFile      = NULL;
}

CAlienDatabase::~CAlienDatabase()
{
}

FileStruct* CAlienDatabase::getBoardFile()
{
   if (m_boardFile == NULL)
   {
      m_boardFile = Graph_File_Start(getBoardName(),m_databaseTypeId);
      m_boardFile->setBlockType(blockTypePcb);
      m_boardFile->getBlock()->setBlockType(blockTypePcb);
   }

   return m_boardFile;
}

FileStruct* CAlienDatabase::getPanelFile()
{
   if (m_panelFile == NULL)
   {
      m_panelFile = Graph_File_Start(getBoardName(),m_databaseTypeId);
      m_panelFile->setBlockType(blockTypePanel);
      m_panelFile->getBlock()->setBlockType(blockTypePanel);
      m_panelFile->setShow(false);
   }

   return m_panelFile;
}

void CAlienDatabase::setInchesPerUnit(double inchesPerUnit)
{
   m_inchesPerUnit = inchesPerUnit;
   m_pageUnitsPerUnit = Units_Factor(UNIT_INCHES,getCamCadDatabase().getCamCadDoc().getSettings().getPageUnits());
}

void CAlienDatabase::convertToInches(CPoint2d& coordinate)
{
   ASSERT(m_inchesPerUnit != 0.);

   coordinate.x *= m_inchesPerUnit;
   coordinate.y *= m_inchesPerUnit;
}

void CAlienDatabase::convertToInches(double& unit)
{
   ASSERT(m_inchesPerUnit != 0.);

   unit *= m_inchesPerUnit;
}

void CAlienDatabase::convertToInches(int& unit)
{
   ASSERT(m_inchesPerUnit != 0.);

   unit = (int)(unit * m_inchesPerUnit);
}

double CAlienDatabase::toInches(double unit) 
{
   ASSERT(m_inchesPerUnit != 0.);

   return unit * m_inchesPerUnit;
}

double CAlienDatabase::toInches(const CString& unitString) 
{
   ASSERT(m_inchesPerUnit != 0.);

   double unit = atof(unitString);

   return unit * m_inchesPerUnit;
}

double CAlienDatabase::toInches(double unit,PageUnitsTag fromUnits) 
{
   double unitsPerInch = 0.;

   switch (fromUnits)
   {
   case pageUnitsInches:            unitsPerInch =    1.0;  break;
   case pageUnitsMils:              unitsPerInch = 1000.0;  break;
   case pageUnitsMilliMeters:       unitsPerInch =   25.4;  break;
   case pageUnitsHpPlotter:         unitsPerInch = 1016.0;  break;
   case pageUnitsCentiMicroMeters:  unitsPerInch =   25.4 *  100. * 1000. ;  break;
   case pageUnitsMicroMeters:       unitsPerInch =   25.4 * 1000. ;          break;
   case pageUnitsNanoMeters:        unitsPerInch =   25.4 * 1000. * 1000. ;  break;
   }

   ASSERT(unitsPerInch != 0.);

   return unit / unitsPerInch;
}

double CAlienDatabase::toInches(const CString& unitString,PageUnitsTag fromUnits) 
{
   double unit = atof(unitString);

   return toInches(unit,fromUnits);
}

double CAlienDatabase::toPageUnits(double unit) 
{
   ASSERT(m_pageUnitsPerUnit != 0.);

   return unit * m_pageUnitsPerUnit;
}

double CAlienDatabase::toPageUnits(const CString& unitString) 
{
   ASSERT(m_pageUnitsPerUnit != 0.);

   double unit = atof(unitString);

   return unit * m_pageUnitsPerUnit;
}

double CAlienDatabase::toPageUnits(double unit,PageUnitsTag fromUnits) 
{
   return unit * Units_Factor(fromUnits,getCamCadDatabase().getCamCadDoc().getSettings().getPageUnits());
}

double CAlienDatabase::toPageUnits(const CString& unitString,PageUnitsTag fromUnits) 
{
   double unit = atof(unitString);

   return toPageUnits(unit,fromUnits);
}

