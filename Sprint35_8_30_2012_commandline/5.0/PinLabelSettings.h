// $Header: /CAMCAD/4.4/PinLabelSettings.h 4     4/30/04 4:54p Lynn Phung $

/*
$History: PinLabelSettings.h $
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
 * User: Kurt Van Ness Date: 8/12/03    Time: 9:06p
 * Updated in $/CAMCAD/4.3
*/ 

#if !defined(__PinLabelSettings_h__)
#define __PinLabelSettings_h__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//_____________________________________________________________________________
#define DefaultPinLabelTextBoxPixels           2 
#define DefaultMinPinLabelTextHeightPixels     2
#define DefaultMaxPinLabelTextHeightPixels   250
#define DefaultNominalPinLabelTextHeightMils  15
#define DefaultPinLabelFontFaceName  "Microsoft Sans Serif"
#define DefaultPinLabelTextAngle					  0

class CPinLabelSettings : public CObject
{
private:
   mutable int m_textBoxPixels;
   mutable int m_minTextHeightPixels;
   mutable int m_maxTextHeightPixels;
   double m_textHeightInches;
	double m_textDegrees;
   LOGFONT m_logFont;

public:
   CPinLabelSettings();
   CPinLabelSettings& operator=(const CPinLabelSettings& other);

   void normalize() const;

   int getTextBoxPixels() const { normalize();  return m_textBoxPixels; }
   int getMinTextHeightPixels() const { normalize();  return m_minTextHeightPixels; }
   int getMaxTextHeightPixels() const { normalize();  return m_maxTextHeightPixels; }
   double getTextHeightInches() const { return m_textHeightInches; }
	double getTextDegrees() const { return m_textDegrees; }
   CString getFontFaceName() const { return m_logFont.lfFaceName; }

   void setTextBoxPixels(int pixels) { m_textBoxPixels = pixels; }
   void setMinTextHeightPixels(int pixels) { m_minTextHeightPixels = pixels; }
   void setMaxTextHeightPixels(int pixels) { m_maxTextHeightPixels = pixels; }
   void setTextHeightInches(double inches) { m_textHeightInches = inches; }
	void setTextDegrees(double degree);
   void setFontFaceName(const CString& fontFaceName) { strncpy(m_logFont.lfFaceName,fontFaceName,LF_FACESIZE); }

   void getLogFont(LOGFONT& logFont) const { memcpy(&logFont,&m_logFont,sizeof(LOGFONT)); }
   void setLogFont(LOGFONT& logFont) { memcpy(&m_logFont,&logFont,sizeof(LOGFONT)); }

   void loadFromRegistry();
   void storeInRegistry();
};

#endif
