
#include "StdAfx.h"
#include <math.h>
#include "Color.h"

//_____________________________________________________________________________
double CColor::m_nslLinRange = 10.;

CColor::CColor(double cmp0,double cmp1,double cmp2,ColorModelTag colorModel)
{
   m_nslLin = 1.;
   set(cmp0,cmp1,cmp2,colorModel);
}

CColor::CColor(double cmp0,double cmp1,double cmp2,double nslLin,ColorModelTag colorModel)
{
   m_nslLin = nslLin;

   if (m_nslLin <= 0.)
   {
      m_nslLin = 1.;
   }

   set(cmp0,cmp1,cmp2,colorModel);
}

CColor::CColor(COLORREF color)
{
   m_nslLin = 1.;
   setColor(color);
}

CColor::CColor(COLORREF color,double nslLin)
{
   m_nslLin = nslLin;

   if (m_nslLin <= 0.)
   {
      m_nslLin = 1.;
   }

   setColor(color);
}

CColor::CColor(const CColor& other)
{
   m_nslLin = other.m_nslLin;
   setRgb(other.m_red,other.m_grn,other.m_blu);
}

CColor& CColor::operator=(const CColor& other)
{
   m_nslLin = other.m_nslLin;
   setRgb(other.m_red,other.m_grn,other.m_blu);

   return *this;
}

void CColor::set(double cmp0,double cmp1,double cmp2,ColorModelTag colorModel)
{
   switch (colorModel)
   {
   case colorModelRgb:  setRgb(cmp0,cmp1,cmp2);  break;
   case colorModelHsv:  setHsv(cmp0,cmp1,cmp2);  break;
   case colorModelHsl:  setHsl(cmp0,cmp1,cmp2);  break;
   case colorModelNsl:  setNsl(cmp0,cmp1,cmp2);  break;
   }
}

void CColor::setRgb(double red,double grn,double blu)
{
   m_red = red;
   m_grn = grn;
   m_blu = blu;

   calcHsvFromRgb();
   calcHslFromRgb();
   calcNslFromHsl();
}

void CColor::setHsv(double hue,double sat,double val)
{
   m_hsvHue = hue;
   m_hsvSat = sat;
   m_hsvVal = val;

   calcRgbFromHsv();
   calcHslFromRgb();
   calcNslFromHsl();
}

void CColor::setHsl(double hue,double sat,double lum)
{
   m_hslHue = hue;
   m_hslSat = sat;
   m_hslLum = lum;

   calcHsvFromHsl();
   calcRgbFromHsv();
   calcNslFromHsl();
}

void CColor::setNsl(double hue,double sat,double lum)
{
   m_nslHue = hue;
   m_nslSat = sat;
   m_nslLum = lum;

   calcHslFromNsl();
   calcHsvFromHsl();
   calcRgbFromHsv();
}

void CColor::setNsl(double hue,double sat,double lum,double lin)
{
   m_nslLin = lin;

   setNsl(hue,sat,lum);
}

void CColor::getRgb(double& red,double& grn,double& blu) const
{
   red = m_red;
   grn = m_grn;
   blu = m_blu;
}

void CColor::getHsv(double& hue,double& sat,double& val) const
{
   hue = m_hsvHue;
   sat = m_hsvSat;
   val = m_hsvVal;
}

void CColor::getHsl(double& hue,double& sat,double& lum) const
{
   hue = m_hslHue;
   sat = m_hslSat;
   lum = m_hslLum;
}

void CColor::getNsl(double& hue,double& sat,double& lum) const
{
   hue = m_nslHue;
   sat = m_nslSat;
   lum = m_nslLum;
}

void CColor::getNsl(double& hue,double& sat,double& lum,double& lin) const
{
   hue = m_nslHue;
   sat = m_nslSat;
   lum = m_nslLum;
   lin = m_nslLin;
}

void CColor::get(double& cmp0,double& cmp1,double& cmp2,ColorModelTag colorModel) const
{
   switch (colorModel)
   {
   case colorModelRgb:  getRgb(cmp0,cmp1,cmp2);  break;
   case colorModelHsv:  getHsv(cmp0,cmp1,cmp2);  break;
   case colorModelHsl:  getHsl(cmp0,cmp1,cmp2);  break;
   case colorModelNsl:  getNsl(cmp0,cmp1,cmp2);  break;
   }
}

double CColor::getNslLinPos() const
{
   double linLog = log(m_nslLin) / log(m_nslLinRange);
   double pos = (linLog + 1.)/2.;

   forceDomain(pos);

   return pos;
}

void CColor::setNslLinFromPos(double pos)
{
   double logExponent = (2*pos) - 1;
   double lin = pow(m_nslLinRange,logExponent);

   setNslLin(lin);
}

COLORREF CColor::getColor()
{
   COLORREF color = RGB(toInt(m_red),toInt(m_grn),toInt(m_blu));

   return color;
}

void CColor::setColor(COLORREF color)
{
   setRgb(toFloat(GetRValue(color)),toFloat(GetGValue(color)),toFloat(GetBValue(color)));
}

int CColor::toInt(double value)
{
   return (int)(value * 255.);
}

double CColor::toFloat(int value)
{
   return value / 255.;
}

void CColor::forceDomain(double& value) const
{
   if (value < 0.)
   {
      value = 0.;
   }
   else if (value > 1.0)
   {
      value = 1.0;
   }
}

double CColor::getMinRgb() const
{
   double retval = m_red;

   if (m_grn < retval) retval = m_grn;
   if (m_blu < retval) retval = m_blu;

   return retval;
}

double CColor::getMaxRgb() const
{
   double retval = m_red;

   if (m_grn > retval) retval = m_grn;
   if (m_blu > retval) retval = m_blu;

   return retval;
}

double CColor::calcHueFromRgb()
{
   double hue = 0.;

   forceDomain(m_red);
   forceDomain(m_grn);
   forceDomain(m_blu);

	double cMin = getMinRgb();
	double cMax = getMaxRgb();

   double delta = cMax - cMin;

   if (cMax != 0.)
   {
      if (delta == 0.)
      {
         hue = 3.;
      }
      else if (cMax == m_red)
      {
         hue = (m_grn - m_blu) / delta;
      }
      else if (cMax == m_grn)
      {
         hue = 2. + (m_blu - m_red) / delta;
      }
      else
      {
         hue = 4. + (m_red - m_grn) / delta;
      }

      hue /= 6.;

      if (hue < 0.) hue += 1.;
   }

   return hue;
}

void CColor::calcHsvFromRgb()
{
   m_hsvHue = calcHueFromRgb();

	double cMin = getMinRgb();
	double cMax = getMaxRgb();

   m_hsvVal = cMax;
   double delta = cMax - cMin;

   if (cMax == 0.)
   {
      m_hsvSat = 0.;
   }
   else
   {
      m_hsvSat = delta / cMax;
   }
}

void CColor::calcHslFromRgb()
{
   m_hslHue = calcHueFromRgb();

	double cMin = getMinRgb();
	double cMax = getMaxRgb();

   m_hslLum = (cMin + cMax)/2.;

   double delta = cMax - cMin;

   if (m_hslLum < .5)
   {
      if (m_hslLum == 0.)
      {
         m_hslSat = 0.;
      }
      else
      {
         m_hslSat = delta / (2. * m_hslLum);
      }
   }
   else
   {
      if (m_hslLum == 1.)
      {
         m_hslSat = 0.;
      }
      else
      {
         m_hslSat = delta / (2. * (1. - m_hslLum));
      }
   }
}

void CColor::calcRgbFromHsv()
{
   forceDomain(m_hsvHue);
   forceDomain(m_hsvSat);
   forceDomain(m_hsvVal);

   if (m_hsvSat == 0.)
   {
      m_red = m_grn = m_blu = m_hsvVal;
   }
   else
   {
      int index = (int)(m_hsvHue * 6.);
      double fraction = (m_hsvHue * 6.) - index;
      double pComp = m_hsvVal * (1. - m_hsvSat);
      double qComp = m_hsvVal * (1. - m_hsvSat * fraction);
      double tComp = m_hsvVal * (1. - m_hsvSat * (1. - fraction));

      switch(index % 6)
      {
      case 0:  m_red = m_hsvVal;  m_grn = tComp;     m_blu = pComp;     break;
      case 1:  m_red = qComp;     m_grn = m_hsvVal;  m_blu = pComp;     break;
      case 2:  m_red = pComp;     m_grn = m_hsvVal;  m_blu = tComp;     break;
      case 3:  m_red = pComp;     m_grn = qComp;     m_blu = m_hsvVal;  break;
      case 4:  m_red = tComp;     m_grn = pComp;     m_blu = m_hsvVal;  break;
      default: m_red = m_hsvVal;  m_grn = pComp;     m_blu = qComp;     break;
      }
   }
}

void CColor::calcHsvFromHsl()
{
   forceDomain(m_hslHue);
   forceDomain(m_hslSat);
   forceDomain(m_hslLum);

   double delta;

   if (m_hslLum < .5)
   {
      delta = m_hslSat * (2. * m_hslLum);
   }
   else
   {
      delta = m_hslSat * (2. * (1. - m_hslLum));
   }

   // lum = (min + max)/2
   // delta = max - min
   //
   // delta + 2*lum = 2*max
   // max = delta/2 + lum

   double cMax = m_hslLum + delta/2.;
   double cMin = cMax - delta;

   m_hsvHue = m_hslHue;
   m_hsvVal = cMax;
   m_hsvSat = ((cMax == 0.) ? 0. : delta / cMax);
}

void CColor::calcHslFromNsl2()
{
   forceDomain(m_nslHue);
   forceDomain(m_nslSat);
   forceDomain(m_nslLum);

   const double biasOffset = 1.;

   if (m_nslLin <= 0.)
   {
      m_nslLin = 1.;
   }

   if (m_nslHue < 1.)
   {
      double angle = 2. * Pi * (m_nslHue / .2);
      double offset = .1 * m_nslLin * sin(angle);

      m_hslHue = m_nslHue + offset;
   }
   else
   {
      m_hslHue = 1.;
   }

   m_hslSat = m_nslSat;
   m_hslLum = m_nslLum;
}

void CColor::calcNslFromHsl2()
{
   forceDomain(m_hslHue);
   forceDomain(m_hslSat);
   forceDomain(m_hslLum);

   m_nslHue = 1.;

   m_nslSat = m_hslSat;
   m_nslLum = m_hslLum;
}

void CColor::calcHslFromNsl()
{
   forceDomain(m_nslHue);
   forceDomain(m_nslSat);
   forceDomain(m_nslLum);

   const double biasOffset = 1.;

   if (m_nslLin <= 0.)
   {
      m_nslLin = 1.;
   }

   if (m_nslHue < 1.)
   {
      double hue6 = fmod(12. * m_nslHue + (12. - biasOffset),12.);
      double subHue6 = fmod(hue6,2.);
      double peakHue6;

      if (subHue6 > 1.)
      {
         peakHue6 = 2. - subHue6;
      }
      else
      {
         peakHue6 = subHue6;
      }

      double hue = pow(peakHue6,m_nslLin);
      double newPeakHue;

      if (subHue6 > 1.)
      {
         newPeakHue = 2. - hue;
      }
      else
      {
         newPeakHue = hue;
      }

      int hue3 = (int)(hue6);
      hue3 = (hue3 / 2) * 2;

      hue6 = fmod(hue3 + newPeakHue + biasOffset,12.);

      m_hslHue = hue6/12.;
   }
   else
   {
      m_hslHue = 1.;
   }

   m_hslSat = m_nslSat;
   m_hslLum = m_nslLum;
}

void CColor::calcNslFromHsl()
{
   forceDomain(m_hslHue);
   forceDomain(m_hslSat);
   forceDomain(m_hslLum);

   const double biasOffset = 1.;

   if (m_hslHue < 1.)
   {
      if (m_nslLin <= 0.)
      {
         m_nslLin = 1.;
      }

      double hue6 = fmod(12. * m_hslHue + (12. - biasOffset),12.);
      double subHue6 = fmod(hue6,2.);
      double peakHue6;

      if (subHue6 > 1.)
      {
         peakHue6 = 2. - subHue6;
      }
      else
      {
         peakHue6 = subHue6;
      }

      double hue = pow(peakHue6,1./m_nslLin);
      double newPeakHue;

      if (subHue6 > 1.)
      {
         newPeakHue = 2. - hue;
      }
      else
      {
         newPeakHue = hue;
      }

      int hue3 = (int)(hue6);
      hue3 = (hue3 / 2) * 2;

      hue6 = fmod(hue3 + newPeakHue + biasOffset,12.);

      m_nslHue = hue6/12.;
   }
   else
   {
      m_nslHue = 1.;
   }

   m_nslSat = m_hslSat;
   m_nslLum = m_hslLum;
}

CColor CColor::interpolate(const CColor& other,double parameterMetric,
   ColorModelTag colorModel) const
{
   double thisCmp0,thisCmp1,thisCmp2;
   double otherCmp0,otherCmp1,otherCmp2;

   get(thisCmp0,thisCmp1,thisCmp2,colorModel);
   other.get(otherCmp0,otherCmp1,otherCmp2,colorModel);

   forceDomain(parameterMetric);

   double cmp0 = thisCmp0 + (otherCmp0 - thisCmp0)*parameterMetric;
   double cmp1 = thisCmp1 + (otherCmp1 - thisCmp1)*parameterMetric;
   double cmp2 = thisCmp2 + (otherCmp2 - thisCmp2)*parameterMetric;

   CColor interpolatedColor(cmp0,cmp1,cmp2,m_nslLin,colorModel);

   return interpolatedColor;
}
