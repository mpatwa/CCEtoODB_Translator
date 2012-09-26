// $Header: /CAMCAD/5.0/Dca/DcaCamCadDataSettings.h 5     3/22/07 12:55a Kurt Van Ness $

#if !defined(__DcaCamCadDataSettings_h__)
#define __DcaCamCadDataSettings_h__

#pragma once

class CExtent;

enum PageUnitsTag;

//_____________________________________________________________________________
class CCamCadDataSettings
{
private:
   PageUnitsTag m_pageUnits;
   double m_xMin;
   double m_xMax;
   double m_yMin;
   double m_yMax;

   //BOOL m_bottomView;  // BOOL for backwards compatibility with CCEtoODBDoc (temporary)
   bool m_bottomView;  // BOOL for backwards compatibility with CCEtoODBDoc (temporary)

   double m_textSpacingRatio;  // todo - link to CCEtoODBDoc::SettingsStruct
   double m_smallWidth;

public:
   CCamCadDataSettings();

   // accessors
   PageUnitsTag getPageUnits() const;
   void setPageUnits(PageUnitsTag pageUnits);

   double getXmin() const;
   void setXmin(double xMin);

   double getYmin() const;
   void setYmin(double yMin);

   double getXmax() const;
   void setXmax(double xMax);

   double getYmax() const;
   void setYmax(double yMax);

   CExtent getExtent() const;
   void setExtent(const CExtent& extent);

   //BOOL& getBottomView();
   bool  getBottomView() const;
   void  setBottomView(bool bottomView);

   double getTextSpacingRatio() const;
   void   setTextSpacingRatio(double ratio);

   double getSmallWidth() const;
   void   setSmallWidth(double width);

   // operations
   bool loadSettings(const CString& settingsFilePath);
};

#endif
