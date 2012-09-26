// $Header: /CAMCAD/5.0/Dca/DcaCamCadDataSettings.cpp 4     3/22/07 12:55a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaCamCadDataSettings.h"
#include "DcaUnits.h"
#include "DcaSettingsFile.h"
#include "DcaExtent.h"

//_____________________________________________________________________________
CCamCadDataSettings::CCamCadDataSettings()
: m_pageUnits(pageUnitsUndefined)
, m_xMin(0.)
, m_yMin(0.)
, m_xMax(0.)
, m_yMax(0.)
, m_bottomView(false)
, m_textSpacingRatio(1.)
, m_smallWidth(0.)
{
}

PageUnitsTag CCamCadDataSettings::getPageUnits() const
{
   return m_pageUnits;
}

void CCamCadDataSettings::setPageUnits(PageUnitsTag pageUnits)
{
   m_pageUnits = pageUnits;
}

double CCamCadDataSettings::getXmin() const
{
   return m_xMin;
}

void CCamCadDataSettings::setXmin(double xMin)
{
   m_xMin = xMin;
}

double CCamCadDataSettings::getYmin() const
{
   return m_yMin;
}

void CCamCadDataSettings::setYmin(double yMin)
{
   m_yMin = yMin;
}

double CCamCadDataSettings::getXmax() const
{
   return m_xMax;
}

void CCamCadDataSettings::setXmax(double xMax)
{
   m_xMax = xMax;
}

double CCamCadDataSettings::getYmax() const
{
   return m_yMax;
}

void CCamCadDataSettings::setYmax(double yMax)
{
   m_yMax = yMax;
}

CExtent CCamCadDataSettings::getExtent() const
{
   CExtent extent(m_xMin,m_yMin,m_xMax,m_yMax);

   return extent;
}

void CCamCadDataSettings::setExtent(const CExtent& extent)
{
   m_xMin = extent.getXmin();
   m_yMin = extent.getYmin();
   m_xMax = extent.getXmax();
   m_yMax = extent.getYmax();
}

//BOOL& CCamCadDataSettings::getBottomView()
//{
//   return m_bottomView;
//}

bool  CCamCadDataSettings::getBottomView() const
{
   return (m_bottomView != 0);
}

void  CCamCadDataSettings::setBottomView(bool bottomView)
{
   m_bottomView = bottomView;
}

double CCamCadDataSettings::getTextSpacingRatio() const
{
   return m_textSpacingRatio;
}

void   CCamCadDataSettings::setTextSpacingRatio(double ratio)
{
   m_textSpacingRatio = ratio;
}

double CCamCadDataSettings::getSmallWidth() const
{
   return m_smallWidth;
}

void CCamCadDataSettings::setSmallWidth(double width)
{
   m_smallWidth = width;
}

bool CCamCadDataSettings::loadSettings(const CString& settingsFilePath)
{
   bool retval = false;

   CSettingsFile settingsFile;

   if (settingsFile.open(settingsFilePath))
   {
      settingsFile.setFilterEqualsSign(true);

      while (settingsFile.getNextCommandLine())
      {
         if      (settingsFile.isCommand(".Units"            ,2))
         {
            int units = settingsFile.getIntParam(1);
            PageUnitsTag pageUnits = intToPageUnitsTag(units);

            setPageUnits(pageUnits);
         }
         else if (settingsFile.isCommand(".SmallWidth"        ,2))
         {
            double smallWidth = settingsFile.getDoubleParam(1);

            setSmallWidth(smallWidth);
         }
         else if (settingsFile.isCommand(".TextSpacingRatio"  ,2))
         {
            double smallWidth = settingsFile.getDoubleParam(1);

            setTextSpacingRatio(smallWidth);
         }
         else if (settingsFile.isCommand(".PageSize"          ,2))
         {
            for (int index = 1;index < settingsFile.getParamCount();index += 2)
            {
               if (settingsFile.getParam(index).CompareNoCase("xMin") == 0)
               {
                  setXmin(settingsFile.getDoubleParam(index + 1));
               }
               else if (settingsFile.getParam(index).CompareNoCase("yMin") == 0)
               {
                  setYmin(settingsFile.getDoubleParam(index + 1));
               }
               else if (settingsFile.getParam(index).CompareNoCase("xMax") == 0)
               {
                  setXmax(settingsFile.getDoubleParam(index + 1));
               }
               else if (settingsFile.getParam(index).CompareNoCase("yMax") == 0)
               {
                  setYmax(settingsFile.getDoubleParam(index + 1));
               }
            }
         }
      }

      retval = true;
   }

   return retval;
}
