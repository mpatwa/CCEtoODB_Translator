// $Header: /CAMCAD/4.5/PinLabelSettings.cpp 6     1/27/05 7:43p Kurt Van Ness $

/*
$History: PinLabelSettings.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Kurt Van Ness Date: 1/27/05    Time: 7:43p
 * Updated in $/CAMCAD/4.5
 * 
 * *****************  Version 4  *****************
 * User: Lynn Phung   Date: 4/30/04    Time: 4:54p
 * Updated in $/CAMCAD/4.4
 * Version = "4.4.0fx"
 *  - Case #486 - ExcelIn.cpp - Excellon Import, removed a popup dialog
 * during import - LP
 * - Case #504 - Draw_Msc.cpp, PinLabelSetting.cpp/.h, Settings.cpp -
 * Added "TextAngle" to default.set file - LP
 * - Case #551 - Net_Util.cpp - "Comp/Pins loaded" count was incorrect
 * after loading netlist - LP
 * - Case #614 - CompPin.cpp - Added the netname next to the comppin when
 * query comppin attributes - LP
 * 
 * *****************  Version 2  *****************
 * User: Kurt Van Ness Date: 8/12/03    Time: 5:07p
 * Updated in $/CAMCAD/4.3
*/

#include "StdAfx.h"
#include "General.h"
#include "PinLabelSettings.h"
#include "RwLib.h"

//_____________________________________________________________________________
CPinLabelSettings::CPinLabelSettings()
{
   m_textBoxPixels       = DefaultPinLabelTextBoxPixels;
   m_minTextHeightPixels = DefaultMinPinLabelTextHeightPixels;
   m_maxTextHeightPixels = DefaultMaxPinLabelTextHeightPixels;
   m_textHeightInches    = DefaultNominalPinLabelTextHeightMils / 1000.;
	m_textDegrees			 = DefaultPinLabelTextAngle;

   m_logFont.lfHeight         = 0;
   m_logFont.lfWidth          = 0;
   m_logFont.lfEscapement     = 0;
   m_logFont.lfOrientation    = 0;
   m_logFont.lfWeight         = FW_NORMAL;
   m_logFont.lfItalic         = 0;
   m_logFont.lfUnderline      = 0;
   m_logFont.lfStrikeOut      = 0;
   m_logFont.lfCharSet        = ANSI_CHARSET;
   m_logFont.lfOutPrecision   = OUT_DEFAULT_PRECIS;
   m_logFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
   m_logFont.lfQuality        = ANTIALIASED_QUALITY;
   m_logFont.lfPitchAndFamily = DEFAULT_PITCH || FF_SWISS;
   strncpy(m_logFont.lfFaceName,DefaultPinLabelFontFaceName,LF_FACESIZE);
}

CPinLabelSettings& CPinLabelSettings::operator=(const CPinLabelSettings& other)
{
   if (&other != this)
   {
      m_textBoxPixels       = other.m_textBoxPixels;
      m_minTextHeightPixels = other.m_minTextHeightPixels;
      m_maxTextHeightPixels = other.m_maxTextHeightPixels;
      m_textHeightInches    = other.m_textHeightInches;
		m_textDegrees			 = other.m_textDegrees;

      memcpy(&m_logFont,&(other.m_logFont),sizeof(LOGFONT));
   }

   return *this;
}

void  CPinLabelSettings::setTextDegrees(double degrees)
{
   int tmpDegrees = (int)degrees;
   tmpDegrees = ((tmpDegrees % 360) + 360) % 360;

   m_logFont.lfEscapement  = -10 * tmpDegrees;
   m_logFont.lfOrientation = -10 * tmpDegrees;
	m_textDegrees = degrees;
}

void CPinLabelSettings::normalize() const
{
   if (m_textBoxPixels < 1) m_textBoxPixels = 1;

   if (m_minTextHeightPixels < m_textBoxPixels      ) m_minTextHeightPixels = m_textBoxPixels;

   if (m_maxTextHeightPixels < m_minTextHeightPixels) m_maxTextHeightPixels = m_minTextHeightPixels;
}

void CPinLabelSettings::loadFromRegistry()
{
   CAppRegistryKey appKey; 
   CRegistryKey settings = appKey.createSubKey("Settings");

   if (settings.isOpen())
   {
      CRegistryKey pinLabels = settings.createSubKey("PinLabels");

      if (pinLabels.isOpen())
      {
         DWORD value;

         m_textBoxPixels       = (pinLabels.getIntValue("TextBoxPixels"      ,value) ? value : DefaultPinLabelTextBoxPixels);
         m_minTextHeightPixels = (pinLabels.getIntValue("MinTextHeightPixels",value) ? value : DefaultMinPinLabelTextHeightPixels);
         m_maxTextHeightPixels = (pinLabels.getIntValue("MaxTextHeightPixels",value) ? value : DefaultMaxPinLabelTextHeightPixels);
         m_textHeightInches    = (pinLabels.getIntValue("TextHeightMils"     ,value) ? value : DefaultNominalPinLabelTextHeightMils) / 1000.;
			m_textDegrees			 = (pinLabels.getIntValue("TextAngle"			  ,value) ? value : DefaultPinLabelTextAngle);

         CRegistryKey font = pinLabels.createSubKey("Font");

         if (font.isOpen())
         {
            CString faceName;

            if (! font.getStringValue("FaceName",faceName))
            {
               faceName = DefaultPinLabelFontFaceName;
            }

            strncpy(m_logFont.lfFaceName,faceName,LF_FACESIZE);
            m_logFont.lfEscapement  = (font.getIntValue("Escapement",value) ? value : 0) * -10;
            m_logFont.lfOrientation = m_logFont.lfEscapement;
         }
      }
   }
}

void CPinLabelSettings::storeInRegistry()
{
   CAppRegistryKey appKey; 
   CRegistryKey settings = appKey.createSubKey("Settings");

   if (settings.isOpen())
   {
      CRegistryKey pinLabels = settings.createSubKey("PinLabels");

      if (pinLabels.isOpen())
      {
         pinLabels.setValue("TextBoxPixels"      ,m_textBoxPixels);
         pinLabels.setValue("MinTextHeightPixels",m_minTextHeightPixels);
         pinLabels.setValue("MaxTextHeightPixels",m_maxTextHeightPixels);
         pinLabels.setValue("TextHeightMils"     ,(int)round(m_textHeightInches*1000.));
			pinLabels.setValue("TextAngle"			 ,(int)m_textDegrees);

         CRegistryKey font = pinLabels.createSubKey("Font");

         if (font.isOpen())
         {
            font.setValue("FaceName"  ,m_logFont.lfFaceName);
            font.setValue("Escapement",m_logFont.lfEscapement / -10);
         }
      }
   }
}

