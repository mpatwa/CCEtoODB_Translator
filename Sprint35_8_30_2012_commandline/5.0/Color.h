// $Header: /CAMCAD/4.6/read_wrt/Xxxxx.h 2     11/30/06 9:26p Kurt Van Ness $

#if !defined(__Color_h__)
#define __Color_h__

#pragma once

#include "Colors.h"

//_____________________________________________________________________________
enum ColorModelTag
{
   colorModelRgb,
   colorModelHsv,
   colorModelHsl,
   colorModelNsl,
   colorModelUndefined,
};

//_____________________________________________________________________________
class CColor
{
protected:
   // Red, Green, Blue
   double m_red;
   double m_grn;
   double m_blu;

   // Hue, Saturation, Value
   double m_hsvHue;
   double m_hsvSat;
   double m_hsvVal;

   // Hue, Saturation, Luminance
   double m_hslHue;
   double m_hslSat;
   double m_hslLum;

   // Non Linear Hue
   double m_nslHue;
   double m_nslSat;
   double m_nslLum;
   double m_nslLin;
   static double m_nslLinRange;

public:
   CColor(double cmp0,double cmp1,double cmp2,ColorModelTag colorModel=colorModelRgb);
   CColor(double cmp0,double cmp1,double cmp2,double nslLin,ColorModelTag colorModel=colorModelRgb);
   CColor(COLORREF color=colorBlack);
   CColor(COLORREF color,double nslLin);
   CColor(const CColor& other);

   CColor& operator=(const CColor& other);

   void set(double cmp0,double cmp1,double cmp2,ColorModelTag colorModel=colorModelRgb);
   void setRgb(double red,double grn,double blu);
   void setHsv(double hue,double sat,double val);
   void setHsl(double hue,double sat,double lum);
   void setNsl(double hue,double sat,double lum);
   void setNsl(double hue,double sat,double lum,double lin);

   void get(double& cmp0,double& cmp1,double& cmp2,ColorModelTag colorModel=colorModelRgb) const;
   void getRgb(double& red,double& grn,double& blu) const;
   void getHsv(double& hue,double& sat,double& val) const;
   void getHsl(double& hue,double& sat,double& lum) const;
   void getNsl(double& hue,double& sat,double& lum) const;
   void getNsl(double& hue,double& sat,double& lum,double& lin) const;

   COLORREF getColor();
   void setColor(COLORREF color);

   double getRed() const { return m_red; }
   double getGrn() const { return m_grn; }
   double getBlu() const { return m_blu; }
   double getHsvHue() const { return m_hsvHue; }
   double getHsvSat() const { return m_hsvSat; }
   double getHsvVal() const { return m_hsvVal; }
   double getHslHue() const { return m_hslHue; }
   double getHslSat() const { return m_hslSat; }
   double getHslLum() const { return m_hslLum; }
   double getNslHue() const { return m_nslHue; }
   double getNslSat() const { return m_nslSat; }
   double getNslLum() const { return m_nslLum; }
   double getNslLin() const { return m_nslLin; }
   double getNslLinPos() const;

   void setRed(double comp) { setRgb(comp ,m_grn,m_blu); }
   void setGrn(double comp) { setRgb(m_red,comp ,m_blu); }
   void setBlu(double comp) { setRgb(m_red,m_grn,comp ); }
   void setHsvHue(double comp) { setHsv(comp    ,m_hsvSat,m_hsvVal); }
   void setHsvSat(double comp) { setHsv(m_hsvHue,comp    ,m_hsvVal); }
   void setHsvVal(double comp) { setHsv(m_hsvHue,m_hsvSat,comp    ); }
   void setHslHue(double comp) { setHsl(comp    ,m_hslSat,m_hslLum); }
   void setHslSat(double comp) { setHsl(m_hslHue,comp    ,m_hslLum); }
   void setHslLum(double comp) { setHsl(m_hslHue,m_hslSat,comp    ); }
   void setNslHue(double comp) { setNsl(comp    ,m_nslSat,m_nslLum,m_nslLin); }
   void setNslSat(double comp) { setNsl(m_nslHue,comp    ,m_nslLum,m_nslLin); }
   void setNslLum(double comp) { setNsl(m_nslHue,m_nslSat,comp    ,m_nslLin); }
   void setNslLin(double comp) { setNsl(m_nslHue,m_nslSat,m_nslLum,comp    ); }
   void setNslLinFromPos(double pos);

   double getMinRgb() const;
   double getMaxRgb() const;
   CColor interpolate(const CColor& other,double parameterMetric,
      ColorModelTag colorModel=colorModelRgb) const;

protected:
   void forceDomain(double& value) const;
   //void calcHueModels();
   double calcHueFromRgb();
   void calcHsvFromRgb();
   void calcHsvFromHsl();
   void calcHslFromRgb();
   void calcRgbFromHsv();
   void calcNslFromHsl();
   void calcHslFromNsl();
   void calcNslFromHsl2();
   void calcHslFromNsl2();

   int toInt(double value);
   double toFloat(int value);
};

#endif
