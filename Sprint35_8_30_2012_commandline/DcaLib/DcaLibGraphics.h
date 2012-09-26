// $Header: /CAMCAD/DcaLib/DcaLibGraphics.h 3     3/09/07 5:15p Kurt Van Ness $

#if !defined(__DcaLibGraphics_h__)
#define __DcaLibGraphics_h__

#pragma once

/*
History: LibGraphics.h $
 * 
 * *****************  Version 4  *****************
 * User: Kurtv        Date: 5/03/99    Time: 8:21a
 * Updated in $/LibGraphics
 * Added declarations for distanceToLineSqr() and distanceToLine()
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 5/20/98    Time: 4:36p
 * Updated in $/include
 * Added declaration for new overload of intersectLineToLine()
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 4/22/98    Time: 11:48a
 * Updated in $/include
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 3/02/98    Time: 1:08p
 * Created in $/include
 * Initial Add
*/

#include <math.h>

class CPoint2d;

extern double epsilon;

bool calculateCircleCenter(double& x,double& y,double x0,double y0,double x1,double y1,double r);
bool coordEqual(double x0,double y0,double x1,double y1);
double diff0(double a,double b);

int intersectCircleToCircle(double& sx0,double& sy0,double& sx1,double& sy1,
   double h0,double k0,double r0,double a0,double b0,
   double h1,double k1,double r1,double a1,double b1);

int intersectLineToCircle(double& sx0,double& sy0,double& sx1,double& sy1,
int& num,double x0,double y0,double x1,double y1,double h,double k,double r,double a0,double a1);

int intersectLineToLine(double& sx0,double& sy0,double& sx1,double& sy1,
           double x1,double y1,double x2,double y2,
           double x3,double y3,double x4,double y4);
int intersectLineToLine(CPoint2d& ps1,CPoint2d& ps2,
   const CPoint2d& p1,const CPoint2d& p2,const CPoint2d& p3,const CPoint2d& p4);

bool isAngleBetweenDegrees(double angle,double a0,double a1);
bool isAngleBetweenRadians(double angle,double a0,double a1);
void normalizeAnglesDegrees(double& a0,double& a1);
void normalizeAnglesRadians(double& a0,double& a1);
int ordcoord(double& s0,double& s1,double a0,double b0,double a1,double b1);
bool ordEqual(double coord0,double coord1);
int pointsOnSameSideOfLine(const CPoint2d& linePoint0,const CPoint2d& linePoint1,
                           const CPoint2d& p0,const CPoint2d& p1);
void rotateCoord(double& rotx,double& roty,double sinang,double cosang);
double distanceToLineSqr(double xPoint,double yPoint,double xLine0,double yLine0,double xLine1,double yLine1);
inline double distanceToLine(double xPoint,double yPoint,double xLine0,double yLine0,double xLine1,double yLine1)
{ return sqrt(distanceToLineSqr(xPoint,yPoint,xLine0,yLine0,xLine1,yLine1)); }

/*
History: PolynomialRoots.h $
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 4/22/98    Time: 10:04a
 * Created in $/LibGraphics
 * Initial add
*/
  
//_____________________________________________________________________________
class CPolynomialRoots
{
protected:
   int m_order;
   int m_numRealRoots;
   double m_realRoot[4];
   
public:
   CPolynomialRoots();
   ~CPolynomialRoots();

   int getNumRealRoots() { return m_numRealRoots; }
   int getOrder() { return m_order; }

   double getRealRoot(int rootIndex);

   void solve(double a,double b,double c);
};

#endif
