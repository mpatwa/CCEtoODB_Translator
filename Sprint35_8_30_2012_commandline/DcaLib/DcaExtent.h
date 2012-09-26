// $Header: /CAMCAD/DcaLib/DcaExtent.h 3     3/09/07 5:15p Kurt Van Ness $

/*
History: Extent.h $
 * 
 * *****************  Version 6  *****************
 * User: Kvanness     Date: 9/16/99    Time: 7:01a
 * Updated in $/LibCad
 * Added new functions.
 * 
 * *****************  Version 5  *****************
 * User: Kurtv        Date: 5/03/99    Time: 8:17a
 * Updated in $/LibCad
 * Added distanceTo() and withinDistance() functions
 * 
 * *****************  Version 4  *****************
 * User: Kurtv        Date: 12/17/98   Time: 9:35a
 * Updated in $/LibCad
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 12/11/98   Time: 3:57a
 * Updated in $/Libpadsd
 * Removed #include "Globals.h"
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 10/08/98   Time: 4:23a
 * Updated in $/Libpadsd
 * Added support for merge(), transform()
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 4/23/98    Time: 2:20p
 * Created in $/PadsDRC
 * Initial add.
*/

#if !defined(__DcaExtent_h__)
#define __DcaExtent_h__

#pragma once

#include "DcaPoint2d.h"

//class CExtent;
//class CSize2d;
//class CTMatrix;
//class CWriteFormat;

#if !defined(__PointStateTag__)
#define __PointStateTag__
enum pointStateTag { psInside, psOutside, psOnEdge, psIndeterminate, psInvalid };
#endif

//_____________________________________________________________________________
class CExtent : public CObject
{
   //DECLARE_DYNAMIC(CExtent);

private:
   double m_xMin;
   double m_yMin;
   double m_xMax;
   double m_yMax;
   
public:
   CExtent();
   CExtent(double x0,double y0,double x1,double y1);
   CExtent(const CPoint2d& p0,const CPoint2d& p1);
   CExtent(const CPoint2d& center,double radius);
   CExtent(const CExtent& other);
   ~CExtent();

   double getXmin()     const { return m_xMin; }
   double getYmin()     const { return m_yMin; }
   double getXmax()     const { return m_xMax; }
   double getYmax()     const { return m_yMax; }
   double getYsize()    const { return m_yMax - m_yMin; }
   double getXsize()    const { return m_xMax - m_xMin; }
   double getMinSize()  const { return min(getXsize(),getYsize()); }
   double getMaxSize()  const { return max(getXsize(),getYsize()); }
   double getArea()     const { return (isValid() ? getXsize() * getYsize() : 0.); }
   CPoint2d getMin()    const { return CPoint2d(m_xMin,m_yMin); }
   CPoint2d getLL()     const { return CPoint2d(m_xMin,m_yMin); }
   CPoint2d getMax()    const { return CPoint2d(m_xMax,m_yMax); }
   CPoint2d getUR()     const { return CPoint2d(m_xMax,m_yMax); }
   CPoint2d getUL()     const { return CPoint2d(m_xMin,m_yMax); }
   CPoint2d getLR()     const { return CPoint2d(m_xMax,m_yMin); }
   CPoint2d getCenter() const { return CPoint2d((m_xMin + m_xMax)/2.,(m_yMin + m_yMax)/2.); }
   CSize2d getSize()    const { return CSize2d(m_xMax - m_xMin,m_yMax - m_yMin); }

   void setXmin(double xMin);
   void setXmax(double xMax);
   void setYmin(double yMin);
   void setYmax(double yMax);
   void set(double x0,double y0,double x1,double y1);
   void set(const CPoint2d& p0,const CPoint2d& p1);
   void setMin(const CPoint2d& point);
   void setMax(const CPoint2d& point);
   void setToSearchRadius(const CPoint2d& point,double radius);
   void maximize();

   void reset();
   bool update(double x,double y);
   bool update(const CExtent& extent);
   bool update(const CPoint2d& point) { return update(point.x,point.y); }
   bool intersect(const CExtent& extent);
   bool intersects(const CExtent& extent) const;
   bool isValid() const;
   bool isFloat() const;  // is valid and values can be contained in float
   bool isSensible(double sensibleLimit) const;
   bool assertFloat() const;
   bool contains(const CExtent& otherExtent) const;
   bool contains(const CExtent& otherExtent,double fudgeFactor) const;
   void scale(double scaleFactor);
   void expand(const CSize2d& size);
   void expand(double xSize,double ySize);
   void expand(double size) { expand(size,size); }
   void translate(double dx,double dy);
   void translate(const CPoint2d& point);
   void transform(CTMatrix& transformationMatrix);
   pointStateTag pointState(const CPoint2d& point) const;
   double distanceTo(const CPoint2d& point) const { return distanceTo(point.x,point.y); }
   double distanceTo(double x,double y) const;
   double distanceTo(const CExtent& otherExtent) const;
   bool withinOrthogonalDistance(double x,double y,double distance) const;
   bool withinOrthogonalDistance(const CExtent& otherExtent,double distance) const;
   bool isOnOrInside(const CPoint2d& point) const;

   CExtent& operator=(const CExtent& extent);

   void dump(CWriteFormat& writeFormat);
   //void trace();
};

//_____________________________________________________________________________
class CIntExtent
{

private:
   int m_xMin;
   int m_yMin;
   int m_xMax;
   int m_yMax;
   
public:
   CIntExtent();
   CIntExtent(int x0,int y0,int x1,int y1);
   CIntExtent(const CPoint& p0,const CPoint& p1);
   CIntExtent(const CPoint& center,int radius);
   CIntExtent(const CIntExtent& other);
   ~CIntExtent();

   int getXmin()      const { return m_xMin; }
   int getYmin()      const { return m_yMin; }
   int getXmax()      const { return m_xMax; }
   int getYmax()      const { return m_yMax; }
   int getYsize()     const { return m_yMax - m_yMin; }
   int getXsize()     const { return m_xMax - m_xMin; }
   int getArea()      const { return (isValid() ? getXsize() * getYsize() : 0); }
   CPoint getMin()    const { return CPoint(m_xMin,m_yMin); }
   CPoint getLL()     const { return CPoint(m_xMin,m_yMin); }
   CPoint getMax()    const { return CPoint(m_xMax,m_yMax); }
   CPoint getUR()     const { return CPoint(m_xMax,m_yMax); }
   CPoint getUL()     const { return CPoint(m_xMin,m_yMax); }
   CPoint getLR()     const { return CPoint(m_xMax,m_yMin); }
   //CPoint getCenter() const { return CPoint((m_xMin + m_xMax)/2.,(m_yMin + m_yMax)/2.); }
   CSize getSize()    const { return CSize(m_xMax - m_xMin,m_yMax - m_yMin); }

   void setXmin(int xMin);
   void setXmax(int xMax);
   void setYmin(int yMin);
   void setYmax(int yMax);
   void set(int x0,int y0,int x1,int y1);
   void set(const CPoint& p0,const CPoint& p1);
   void setMin(const CPoint& point);
   void setMax(const CPoint& point);
   void setToSearchRadius(const CPoint& point,int radius);
   void maximize();

   void reset();
   bool update(int x,int y);
   bool update(const CIntExtent& extent);
   bool update(const CPoint& point) { return update(point.x,point.y); }
   bool isValid() const;
   bool contains(const CIntExtent& otherExtent) const;
   bool contains(const CIntExtent& otherExtent,int fudgeFactor) const;
   void translate(int dx,int dy);
   void translate(const CPoint& point);
   pointStateTag pointState(const CPoint& point) const;
   bool withinOrthogonalDistance(int x,int y,int distance) const;
   bool withinOrthogonalDistance(const CIntExtent& otherExtent,int distance) const;
   bool isOnOrInside(const CPoint& point) const;

   CIntExtent& operator=(const CIntExtent& extent);

   void dump(CWriteFormat& writeFormat);
};

#endif
