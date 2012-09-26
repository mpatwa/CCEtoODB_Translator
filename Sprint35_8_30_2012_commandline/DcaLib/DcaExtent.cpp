// $Header: /CAMCAD/DcaLib/DcaExtent.cpp 3     3/09/07 5:15p Kurt Van Ness $

/*
History: Extent.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Kvanness     Date: 9/16/99    Time: 7:01a
 * Updated in $/LibCad
 * Fixed  CExtent::distanceTo() and added new functionality.
 * 
 * *****************  Version 4  *****************
 * User: Kurtv        Date: 5/03/99    Time: 8:17a
 * Updated in $/LibCad
 * Added distanceTo() and withinDistance() functions
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 12/17/98   Time: 9:35a
 * Updated in $/LibCad
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

#include "StdAfx.h"
#include "DcaExtent.h"
#include "DcaTMatrix.h"
#include "DcaLib.h"
#include "DcaWriteFormat.h"

//_____________________________________________________________________________
CExtent::CExtent()
{
   reset();
}

CExtent::CExtent(double x0,double y0,double x1,double y1)
{
   reset();

   update(x0,y0);
   update(x1,y1);
}

CExtent::CExtent(const CPoint2d& p0,const CPoint2d& p1)
{
   reset();

   update(p0.x,p0.y);
   update(p1.x,p1.y);
}

CExtent::CExtent(const CPoint2d& center,double radius)
{
   reset();

   update(center.x - radius,center.y - radius);
   update(center.x + radius,center.y + radius);
}

CExtent::CExtent(const CExtent& other)
{
   m_xMin = other.m_xMin;
   m_yMin = other.m_yMin;
   m_xMax = other.m_xMax;
   m_yMax = other.m_yMax;
}

CExtent::~CExtent()
{
}

void CExtent::reset()
{
   m_xMin = m_yMin = DBL_MAX;
   m_xMax = m_yMax = -m_xMin;
}

void CExtent::maximize()
{
   m_xMax = m_yMax = DBL_MAX;
   m_xMin = m_yMin = -m_xMax;
}

void CExtent::set(double x0,double y0,double x1,double y1)
{
   reset();

   update(x0,y0);
   update(x1,y1);
}

void CExtent::set(const CPoint2d& p0,const CPoint2d& p1)
{
   reset();

   update(p0);
   update(p1);
}

void CExtent::setToSearchRadius(const CPoint2d& point,double radius)
{
   reset();

   update(point.x - radius,point.y - radius);
   update(point.x + radius,point.y + radius);
}

void CExtent::setXmin(double xMin)
{
   m_xMin = xMin;

   if (m_xMax < m_xMin) m_xMax = m_xMin;
}

void CExtent::setXmax(double xMax)
{
   m_xMax = xMax;

   if (m_xMin > m_xMax) m_xMin = m_xMax;
}

void CExtent::setYmin(double yMin)
{
   m_yMin = yMin;

   if (m_yMax < m_yMin) m_yMax = m_yMin;
}

void CExtent::setYmax(double yMax)
{
   m_yMax = yMax;

   if (m_yMin > m_yMax) m_yMin = m_yMax;
}

void CExtent::setMin(const CPoint2d& point)
{
   setXmin(point.x);
   setYmin(point.y);
}

void CExtent::setMax(const CPoint2d& point)
{
   setXmax(point.x);
   setYmax(point.y);
}

bool CExtent::update(double x,double y)
{
   bool retval = false;

   if (x < m_xMin)
   {
      m_xMin = x;
      retval = true;
   }

   if (x > m_xMax)
   {
      m_xMax = x;
      retval = true;
   }
   
   if (y < m_yMin)
   {
      m_yMin = y;
      retval = true;
   }

   if (y > m_yMax)
   {
      m_yMax = y;
      retval = true;
   }

   //assertFloat();
   
   return retval;
}

bool CExtent::update(const CExtent& extent)
{
   bool retval = false;

   if (extent.m_xMin < m_xMin)
   {
      m_xMin = extent.m_xMin;
      retval = true;
   }

   if (extent.m_yMin < m_yMin)
   {
      m_yMin = extent.m_yMin;
      retval = true;
   }
   
   if (extent.m_xMax > m_xMax)
   {
      m_xMax = extent.m_xMax;
      retval = true;
   }

   if (extent.m_yMax > m_yMax)
   {
      m_yMax = extent.m_yMax;
      retval = true;
   }
   
   return retval;
}

bool CExtent::intersect(const CExtent& extent)
{
   double xMin = max(m_xMin,extent.m_xMin);

   if (extent.m_xMin > m_xMin)
   {
      m_xMin = extent.m_xMin;
   }

   if (extent.m_yMin > m_yMin)
   {
      m_yMin = extent.m_yMin;
   }
   
   if (extent.m_xMax < m_xMax)
   {
      m_xMax = extent.m_xMax;
   }

   if (extent.m_yMax < m_yMax)
   {
      m_yMax = extent.m_yMax;
   }
   
   return isValid();
}

bool CExtent::intersects(const CExtent& extent) const
{
   bool retval = !(m_xMin > extent.m_xMax ||
                   m_xMax < extent.m_xMin ||
                   m_yMin > extent.m_yMax ||
                   m_yMax < extent.m_yMin    );

   return retval;
}

void CExtent::expand(double xSize,double ySize)
{
   double dx = xSize/2.;
   double dy = ySize/2.;

   m_xMin -= dx;
   m_yMin -= dy;
   m_xMax += dx;
   m_yMax += dy;
}

void CExtent::expand(const CSize2d& size)
{
   double dx = size.cx / 2.;
   double dy = size.cy / 2.;

   m_xMin -= dx;
   m_yMin -= dy;
   m_xMax += dx;
   m_yMax += dy;
}

// returns 0. if the point is on or inside the extent
double CExtent::distanceTo(double x,double y) const
{
/*  1|2|3
    -+-+-
    4|5|6
    -+-+-
    7|8|9
*/

   double xDistance = 0.,yDistance = 0.,distance;

   if (x <= m_xMin)
   {  // 1 4 7
      xDistance = m_xMin - x;

      if (y <= m_yMin)
      {  // 7
         yDistance = m_yMin - y;
         distance = _hypot(xDistance,yDistance);
      }
      else if (y >= m_yMax)
      {  // 1
         yDistance = y - m_yMax;
         distance = _hypot(xDistance,yDistance);
      }
      else 
      {  // 4
         distance = xDistance;
      }
   }
   else if (x >= m_xMax)
   {  // 3 6 9
      xDistance = x - m_xMax;

      if (y <= m_yMin)
      {  // 9
         yDistance = m_yMin - y;
         distance = _hypot(xDistance,yDistance);
      }
      else if (y >= m_yMax)
      {  // 3
         yDistance = y - m_yMax;
         distance = _hypot(xDistance,yDistance);
      }
      else 
      {  // 6
         distance = xDistance;
      }
   }
   else if (y <= m_yMin)
   {  // 8
      distance = m_yMin - y;
   }
   else if (y >= m_yMax)
   {  // 2
      distance = y - m_yMax;
   }
   else
   {  // 5
      distance = 0.;
   }

   return distance;
}

// returns 0. if the point is on or -1 if inside the extent
double CExtent::distanceTo(const CExtent& otherExtent) const
{
/*  1|2|3
    -+-+-
    4|5|6
    -+-+-
    7|8|9
*/

   double xDistance = 0.,yDistance = 0.,distance;

   if (otherExtent.m_xMax <= m_xMin)
   {  // 1 4 7
      xDistance = m_xMin - otherExtent.m_xMax;

      if (otherExtent.m_yMax <= m_yMin)
      {  // 7
         yDistance = m_yMin - otherExtent.m_yMax;
         distance = _hypot(xDistance,yDistance);
      }
      else if (otherExtent.m_yMin >= m_yMax)
      {  // 1
         yDistance = otherExtent.m_yMin - m_yMax;
         distance = _hypot(xDistance,yDistance);
      }
      else 
      {  // 4
         distance = xDistance;
      }
   }
   else if (otherExtent.m_xMin >= m_xMax)
   {  // 3 6 9
      xDistance = otherExtent.m_xMin - m_xMax;

      if (otherExtent.m_yMax <= m_yMin)
      {  // 9
         yDistance = m_yMin - otherExtent.m_yMax;
         distance = _hypot(xDistance,yDistance);
      }
      else if (otherExtent.m_yMin >= m_yMax)
      {  // 3
         yDistance = otherExtent.m_yMin - m_yMax;
         distance = _hypot(xDistance,yDistance);
      }
      else 
      {  // 6
         distance = xDistance;
      }
   }
   else if (otherExtent.m_yMax <= m_yMin)
   {  // 8
      distance = m_yMin - otherExtent.m_yMax;
   }
   else if (otherExtent.m_yMin >= m_yMax)
   {  // 2
      distance = otherExtent.m_yMin - m_yMax;
   }
   else
   {  // 5
      distance = -1.;
   }

   return distance;
}

// returns true if the point is on or inside the expanded extent
bool CExtent::withinOrthogonalDistance(double x,double y,double compareDistance) const
{
   bool retval = (x >= m_xMin - compareDistance &&
                  x <= m_xMax + compareDistance &&
                  y >= m_yMin - compareDistance &&
                  y <= m_yMax + compareDistance     );

   return retval;
}

bool CExtent::withinOrthogonalDistance(const CExtent& otherExtent,double compareDistance) const
{
   bool retval = !(otherExtent.m_xMax < m_xMin - compareDistance ||
                   otherExtent.m_xMin > m_xMax + compareDistance ||
                   otherExtent.m_yMax < m_yMin - compareDistance ||
                   otherExtent.m_yMin > m_yMax + compareDistance     );

   return retval;
}

bool CExtent::isOnOrInside(const CPoint2d& point) const
{
   bool retval = (point.x >= m_xMin && point.x <= m_xMax &&
                  point.y >= m_yMin && point.y <= m_yMax     );

   return retval;
}

void CExtent::scale(double scaleFactor)
{
   double xCenter = (m_xMin + m_xMax)/2.;
   double yCenter = (m_yMin + m_yMax)/2.;
   double dx = (m_xMax - m_xMin)*scaleFactor/2.;
   double dy = (m_yMax - m_yMin)*scaleFactor/2.;

   m_xMin = xCenter - dx;
   m_xMax = xCenter + dx;
   m_yMin = yCenter - dy;
   m_yMax = yCenter + dy;
}

void CExtent::translate(double dx,double dy)
{
   m_xMin += dx;
   m_xMax += dx;
   m_yMin += dy;
   m_yMax += dy;
}

void CExtent::translate(const CPoint2d& point)
{
   m_xMin += point.x;
   m_xMax += point.x;
   m_yMin += point.y;
   m_yMax += point.y;
}

void CExtent::transform(CTMatrix& transformationMatrix)
{
   CPoint2d ll(m_xMin,m_yMin);
   CPoint2d lr(m_xMax,m_yMin);
   CPoint2d ul(m_xMin,m_yMax);
   CPoint2d ur(m_xMax,m_yMax);

   transformationMatrix.transform(ll);
   transformationMatrix.transform(lr);
   transformationMatrix.transform(ul);
   transformationMatrix.transform(ur);

   reset();

   update(ll);
   update(lr);
   update(ul);
   update(ur);
}

bool CExtent::isValid() const
{
   bool retval = (m_xMin <= m_xMax && m_yMin <= m_yMax);

   return retval;
}

bool CExtent::isFloat() const
{
   bool retval = isValid();

   if (retval)
   {
      if (m_xMin < -FLT_MAX ||
          m_xMax >  FLT_MAX ||
          m_yMin < -FLT_MAX ||
          m_yMax >  FLT_MAX     )
      {
         retval = false;
      }
   }

   return retval;
}

bool CExtent::isSensible(double sensibleLimit) const
{
   bool retval = isValid();

   if (retval)
   {
      if (m_xMin <= -sensibleLimit ||
          m_xMax >=  sensibleLimit ||
          m_yMin <= -sensibleLimit ||
          m_yMax >=  sensibleLimit     )
      {
         retval = false;
      }
   }

   return retval;
}

bool CExtent::assertFloat() const
{
   bool retval = true;

   if (!isFloat())
   {
      retval = false;
   }

   return retval;
}

pointStateTag CExtent::pointState(const CPoint2d& point) const
{
   pointStateTag retval;

   if (!isValid())
   {
      retval = psIndeterminate;
   }
   else if (point.x > m_xMax || point.x < m_xMin || point.y > m_yMax || point.y < m_yMin)
   {
      retval = psOutside;
   }
   else if (point.x < m_xMax && point.x > m_xMin && point.y < m_yMax && point.y > m_yMin)
   {
      retval = psInside;
   }
   else
   {
      retval = psOnEdge;
   }

   return retval;
}

bool CExtent::contains(const CExtent& otherExtent) const
{
   bool retval = m_xMin <= otherExtent.m_xMin && m_yMin <= otherExtent.m_yMin &&
                 m_xMax >= otherExtent.m_xMax && m_yMax >= otherExtent.m_yMax ;

   return retval;
}

bool CExtent::contains(const CExtent& otherExtent,double fudgeFactor) const
{
   bool retval = m_xMin - otherExtent.m_xMin <  fudgeFactor && m_yMin - otherExtent.m_yMin <  fudgeFactor &&
                 m_xMax - otherExtent.m_xMax > -fudgeFactor && m_yMax - otherExtent.m_yMax > -fudgeFactor ;

   return retval;
}

CExtent& CExtent::operator=(const CExtent& other) 
{
   if (&other != this)
   {
      m_xMin = other.m_xMin;
      m_yMin = other.m_yMin;
      m_xMax = other.m_xMax;
      m_yMax = other.m_yMax;
   }

   return *this;
}

void CExtent::dump(CWriteFormat& writeFormat)
{
   if (isValid())
   {
writeFormat.writef(
"{  CExtent::dump(%p) min=(%s,%s), max=(%s,%s)  }\n",
this,
fpfmt(m_xMin),
fpfmt(m_yMin),
fpfmt(m_xMax),
fpfmt(m_yMax));
   }
   else
   {
writeFormat.writef(
"{  CExtent::dump(%p) <invalid> min=(?,?), max=(?,?)  }\n",
this);
   }
}

//void CExtent::trace()
//{
//   extern CTraceFormat g_traceFormat;
//
//   if (isValid())
//   {
//g_traceFormat.writef(
//"{  CExtent(%p) min=(%s,%s), max=(%s,%s)  }\n",
//this,
//fpfmt(m_xMin),
//fpfmt(m_yMin),
//fpfmt(m_xMax),
//fpfmt(m_yMax));
//   }
//   else
//   {
//g_traceFormat.writef(
//"{  CExtent(%p) <invalid> min=(?,?), max=(?,?)  }\n",
//this);
//   }
//}
// $Header: /CAMCAD/DcaLib/DcaExtent.cpp 3     3/09/07 5:15p Kurt Van Ness $


//_____________________________________________________________________________
CIntExtent::CIntExtent()
{
   reset();
}

CIntExtent::CIntExtent(int x0,int y0,int x1,int y1)
{
   reset();

   update(x0,y0);
   update(x1,y1);
}

CIntExtent::CIntExtent(const CPoint& p0,const CPoint& p1)
{
   reset();

   update(p0.x,p0.y);
   update(p1.x,p1.y);
}

CIntExtent::CIntExtent(const CPoint& center,int radius)
{
   reset();

   update(center.x - radius,center.y - radius);
   update(center.x + radius,center.y + radius);
}

CIntExtent::CIntExtent(const CIntExtent& other)
{
   m_xMin = other.m_xMin;
   m_yMin = other.m_yMin;
   m_xMax = other.m_xMax;
   m_yMax = other.m_yMax;
}

CIntExtent::~CIntExtent()
{
}

void CIntExtent::reset()
{
   m_xMin = m_yMin = INT_MAX;
   m_xMax = m_yMax = -m_xMin;
}

void CIntExtent::maximize()
{
   m_xMax = m_yMax = INT_MAX;
   m_xMin = m_yMin = -m_xMax;
}

void CIntExtent::set(int x0,int y0,int x1,int y1)
{
   reset();

   update(x0,y0);
   update(x1,y1);
}

void CIntExtent::set(const CPoint& p0,const CPoint& p1)
{
   reset();

   update(p0);
   update(p1);
}

void CIntExtent::setToSearchRadius(const CPoint& point,int radius)
{
   reset();

   update(point.x - radius,point.y - radius);
   update(point.x + radius,point.y + radius);
}

void CIntExtent::setXmin(int xMin)
{
   m_xMin = xMin;

   if (m_xMax < m_xMin) m_xMax = m_xMin;
}

void CIntExtent::setXmax(int xMax)
{
   m_xMax = xMax;

   if (m_xMin > m_xMax) m_xMin = m_xMax;
}

void CIntExtent::setYmin(int yMin)
{
   m_yMin = yMin;

   if (m_yMax < m_yMin) m_yMax = m_yMin;
}

void CIntExtent::setYmax(int yMax)
{
   m_yMax = yMax;

   if (m_yMin > m_yMax) m_yMin = m_yMax;
}

void CIntExtent::setMin(const CPoint& point)
{
   setXmin(point.x);
   setYmin(point.y);
}

void CIntExtent::setMax(const CPoint& point)
{
   setXmax(point.x);
   setYmax(point.y);
}

bool CIntExtent::update(int x,int y)
{
   bool retval = false;

   if (x < m_xMin)
   {
      m_xMin = x;
      retval = true;
   }

   if (x > m_xMax)
   {
      m_xMax = x;
      retval = true;
   }
   
   if (y < m_yMin)
   {
      m_yMin = y;
      retval = true;
   }

   if (y > m_yMax)
   {
      m_yMax = y;
      retval = true;
   }

   //assertFloat();
   
   return retval;
}

bool CIntExtent::update(const CIntExtent& extent)
{
   bool retval = false;

   if (extent.m_xMin < m_xMin)
   {
      m_xMin = extent.m_xMin;
      retval = true;
   }

   if (extent.m_yMin < m_yMin)
   {
      m_yMin = extent.m_yMin;
      retval = true;
   }
   
   if (extent.m_xMax > m_xMax)
   {
      m_xMax = extent.m_xMax;
      retval = true;
   }

   if (extent.m_yMax > m_yMax)
   {
      m_yMax = extent.m_yMax;
      retval = true;
   }
   
   return retval;
}

// returns true if the point is on or inside the expanded extent
bool CIntExtent::withinOrthogonalDistance(int x,int y,int compareDistance) const
{
   bool retval = (x >= m_xMin - compareDistance &&
                  x <= m_xMax + compareDistance &&
                  y >= m_yMin - compareDistance &&
                  y <= m_yMax + compareDistance     );

   return retval;
}

bool CIntExtent::withinOrthogonalDistance(const CIntExtent& otherExtent,int compareDistance) const
{
   bool retval = !(otherExtent.m_xMax < m_xMin - compareDistance ||
                   otherExtent.m_xMin > m_xMax + compareDistance ||
                   otherExtent.m_yMax < m_yMin - compareDistance ||
                   otherExtent.m_yMin > m_yMax + compareDistance     );

   return retval;
}

bool CIntExtent::isOnOrInside(const CPoint& point) const
{
   bool retval = (point.x >= m_xMin && point.x <= m_xMax &&
                  point.y >= m_yMin && point.y <= m_yMax     );

   return retval;
}

void CIntExtent::translate(int dx,int dy)
{
   m_xMin += dx;
   m_xMax += dx;
   m_yMin += dy;
   m_yMax += dy;
}

void CIntExtent::translate(const CPoint& point)
{
   m_xMin += point.x;
   m_xMax += point.x;
   m_yMin += point.y;
   m_yMax += point.y;
}

bool CIntExtent::isValid() const
{
   bool retval = (m_xMin <= m_xMax && m_yMin <= m_yMax);

   return retval;
}

pointStateTag CIntExtent::pointState(const CPoint& point) const
{
   pointStateTag retval;

   if (!isValid())
   {
      retval = psIndeterminate;
   }
   else if (point.x > m_xMax || point.x < m_xMin || point.y > m_yMax || point.y < m_yMin)
   {
      retval = psOutside;
   }
   else if (point.x < m_xMax && point.x > m_xMin && point.y < m_yMax && point.y > m_yMin)
   {
      retval = psInside;
   }
   else
   {
      retval = psOnEdge;
   }

   return retval;
}

bool CIntExtent::contains(const CIntExtent& otherExtent) const
{
   bool retval = m_xMin <= otherExtent.m_xMin && m_yMin <= otherExtent.m_yMin &&
                 m_xMax >= otherExtent.m_xMax && m_yMax >= otherExtent.m_yMax ;

   return retval;
}

bool CIntExtent::contains(const CIntExtent& otherExtent,int fudgeFactor) const
{
   bool retval = m_xMin - otherExtent.m_xMin <  fudgeFactor && m_yMin - otherExtent.m_yMin <  fudgeFactor &&
                 m_xMax - otherExtent.m_xMax > -fudgeFactor && m_yMax - otherExtent.m_yMax > -fudgeFactor ;

   return retval;
}

CIntExtent& CIntExtent::operator=(const CIntExtent& other) 
{
   if (&other != this)
   {
      m_xMin = other.m_xMin;
      m_yMin = other.m_yMin;
      m_xMax = other.m_xMax;
      m_yMax = other.m_yMax;
   }

   return *this;
}

void CIntExtent::dump(CWriteFormat& writeFormat)
{
   if (isValid())
   {
writeFormat.writef(
"{  CIntExtent::dump(%p) min=(%d,%d), max=(%d,%d)  }\n",
this,
m_xMin,
m_yMin,
m_xMax,
m_yMax);
   }
   else
   {
writeFormat.writef(
"{  CIntExtent::dump(%p) <invalid> min=(?,?), max=(?,?)  }\n",
this);
   }
}

//void CIntExtent::trace()
//{
//   extern CTraceFormat g_traceFormat;
//
//   if (isValid())
//   {
//g_traceFormat.writef(
//"{  CIntExtent(%p) min=(%s,%s), max=(%s,%s)  }\n",
//this,
//fpfmt(m_xMin),
//fpfmt(m_yMin),
//fpfmt(m_xMax),
//fpfmt(m_yMax));
//   }
//   else
//   {
//g_traceFormat.writef(
//"{  CIntExtent(%p) <invalid> min=(?,?), max=(?,?)  }\n",
//this);
//   }
//}

