// $Header: /CAMCAD/DcaLib/DcaTMatrix.cpp 5     6/05/07 4:57p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaTMatrix.h"
#include "DcaBasesVector.h"
#include "DcaLib.h"

//#include "Stdafx.h"
//#include "TMstate.h"
//#include "Vector2d.h"
//#include "RwLib.h"

//
//IMPLEMENT_DYNAMIC(CTMatrix,CObject);   
//IMPLEMENT_DYNAMIC(CTMatrices,CObArray);
//IMPLEMENT_DYNAMIC(CTMstate,CObject);
//   
//#ifdef USE_DEBUG_NEW
//#define new DEBUG_NEW
//#endif

//=============================================================================

/*
History: RwLib.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Kurtv        Date: 4/22/98    Time: 9:31a
 * Updated in $/LibKnvPP
 * changed references to round() to Round().
 * 
 * *****************  Version 5  *****************
 * User: Kurtv        Date: 2/12/98    Time: 1:44p
 * Updated in $/libknvpp
 * Changed code to support change of float members to double and float
 * parameters to double
 * 
 * *****************  Version 4  *****************
 * User: Kurtv        Date: 3/05/97    Time: 3:38a
 * Updated in $/LibKnvPP
 * Renamed files, (lengthened and fixed case)
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 10/08/96   Time: 1:17a
 * Updated in $/libknvpp
 * added casts to conform to msdev 4.2 enhanced type checking
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 12/27/95   Time: 8:36p
 * Updated in $/libknvpp
*/

//_____________________________________________________________________________
CQuadAngle::CQuadAngle()
{
   m_degrees = 0;
}

CQuadAngle::CQuadAngle(const CQuadAngle& other)
{
   m_degrees = other.m_degrees;
}

CQuadAngle::CQuadAngle(int degrees)
{  
   degrees %= 360;
   degrees += 360;
   degrees %= 360;
   degrees /= 90;
   degrees *= 90;
   
   m_degrees = degrees;
}

CQuadAngle::CQuadAngle(double fdegrees)
{
   fdegrees = fmod((double)fdegrees,360.);
   fdegrees += 360.;
   fdegrees = fmod(fdegrees,360.);
   
   int degrees = DcaRound(fdegrees);
     
   degrees %= 360;
   degrees /= 90;
   degrees *= 90;
   
   m_degrees = degrees;
}

CQuadAngle::~CQuadAngle()
{
}

double CQuadAngle::getRadians() const
{
   return m_degrees * (3.14159265358979 / 180.);
}

void CQuadAngle::getSinCos(int& sin,int& cos) const
{
   switch (m_degrees)
   {
      case   0:  cos =  1;  sin =  0;  break;
      case 180:  cos = -1;  sin =  0;  break;
      case  90:  cos =  0;  sin =  1;  break;
      case 270:  cos =  0;  sin = -1;  break;
   }
}

void CQuadAngle::getSinCos(double& sin,double& cos) const
{
   switch (m_degrees)
   {
      case   0:  cos =  1.;  sin =  0.;  break;
      case 180:  cos = -1.;  sin =  0.;  break;
      case  90:  cos =  0.;  sin =  1.;  break;
      case 270:  cos =  0.;  sin = -1.;  break;
   }
}

CQuadAngle& CQuadAngle::operator=(const CQuadAngle& other)
{
   m_degrees = other.m_degrees;
   
   return *this;
}

CQuadAngle::operator int() const
{
   return m_degrees;
}

CQuadAngle::operator float() const
{
   return (float)m_degrees;
}

CQuadAngle::operator double() const
{
   return (double)m_degrees;
}

/*
Using matrices for transforming coordinates.

  [x' y' 1] = [x  y  1] | a  b  0 |
                        | c  d  0 |
                        | tx ty 1 |

     or

  X' = X M

When performing composite transformations

  X' = X M1 M2 M3   (postmultiplication)
  MT = M1 M2 M3
  X' = X MT

     or

  X' = X M3 M2 M1   (premultiplication)
  MT = M3 M2 M1
  X' = X MT

Which multiplication method should be used?
The answer depends on your view points.
Mi - original CTM
Mt - translating matrix
Ms - scaling matrix
Mr - rotating matrix

Premultiplication converts points from a transformed coordinate system to the original coordinate system.
Each succesive transformation transforms the original axes to new axes.
This is how postScript and PDF are modeled.  
X represents coordinates in the transformed (user) space, and X' represents coordinates in device space.
Premultiplication moves axes.

Postmultiplication transforms points with respect to the original coordinate system.
Each successive transformation transforms the original point to a new point.  
The original and tranformed points are both in the original coordinate system.
Postmultiplication moves objects.

postmultiplication functions:
translate()
rotate()
scale()

premultiplication functions:
translateCtm()
rotateCtm()
scaleCtm()

For example: In a printed circuit board data base, a device, U1, is positioned at (200,1000) with a rotation of 90 degrees.
The outline of the part in local coordinates is a rectangle: (0,0), (200,0), (200,-700), (0,-700), (0,0).
To draw the device, postmultiplication is used.  
First a translation of (200,1000) is performed, then a rotation of 90 degrees.
The transformed axii are now located at the origin of the part and oriented with the part's original coordinate space.
MT = M2 M1 Mi = Mr Mt Mi
Transforming the rectangle converts the part coordinates to device coordinates.
*/

/*
   a  b  0
   c  d  0
   tx ty 1
*/
   
CTMstate ctm;

//_____________________________________________________________________________
CTMatrix::CTMatrix()
: m_basesVector(NULL)
{
   initMatrix();
}

CTMatrix::CTMatrix(double a,double b,double c,double d,double transX,double transY)
: m_basesVector(NULL)
, m_a(a)
, m_b(b)
, m_c(c)
, m_d(d)
, m_transX(transX)
, m_transY(transY)
{
}

CTMatrix::CTMatrix(const CTMatrix& matrix)
: m_basesVector(NULL)
{
   m_a = matrix.m_a;
   m_b = matrix.m_b;
   m_c = matrix.m_c;
   m_d = matrix.m_d;
   m_transX = matrix.m_transX;
   m_transY = matrix.m_transY;
}

CTMatrix::~CTMatrix()
{
   delete m_basesVector;
}

CTMatrix& CTMatrix::operator=(const CTMatrix& matrix)
{
   if (&matrix != this)
   {   
      m_a = matrix.m_a;
      m_b = matrix.m_b;
      m_c = matrix.m_c;
      m_d = matrix.m_d;
      m_transX = matrix.m_transX;
      m_transY = matrix.m_transY;

      releaseBasesVector();
   }
   
   return *this;
}

void CTMatrix::releaseBasesVector() 
{ 
   if (m_basesVector != NULL) 
   { 
      delete m_basesVector;  
      m_basesVector = NULL; 
   } 
}

void CTMatrix::initMatrix()
{
   m_a = m_d = 1.;
   m_b = m_c = m_transX = m_transY = 0.;

   releaseBasesVector();
}

void CTMatrix::inverse(CTMatrix& other) const
{
   other = *this;
   other.invert();
}

CTMatrix& CTMatrix::invert()
{
   double a      = m_a;
   double b      = m_b;
   double c      = m_c;
   double d      = m_d;
   double transX = m_transX;
   double transY = m_transY;

   double determinant = a*d - c*b;

   if (determinant != 0.)
   {
      m_a      =  d/determinant;
      m_b      = -b/determinant;
      m_c      = -c/determinant;
      m_d      =  a/determinant;
      m_transX = (c*transY - d*transX) / determinant;
      m_transY = (b*transX - a*transY) / determinant;
   }

   releaseBasesVector();

   return *this;
}

CTMatrix CTMatrix::inverted() const
{
   CTMatrix matrix(*this);

   return matrix.invert();
}

void CTMatrix::translate(CPoint offset)
{
   translate((double)offset.x,(double)offset.y);
}

void CTMatrix::translate(CPoint2d offset)
{
   translate(offset.x,offset.y);
}

void CTMatrix::translate(double x,double y)
{
   CTMatrix transMatrix(1.,0.,0.,1.,x,y);
   CTMatrix newMatrix;
   
   newMatrix = *this * transMatrix;
   *this = newMatrix;
}

void CTMatrix::translateCtm(CPoint offset)
{
   translateCtm((double)offset.x,(double)offset.y);
}

void CTMatrix::translateCtm(CPoint2d offset)
{
   translateCtm(offset.x,offset.y);
}

void CTMatrix::translateCtm(double x,double y)
{
   CTMatrix transMatrix(1.,0.,0.,1.,x,y);
   CTMatrix newMatrix;
   
   newMatrix = transMatrix * *this;
   *this = newMatrix;
}

void CTMatrix::rotateDegrees(int degrees)
{
   CQuadAngle quadAngle(degrees);
   
   if (degrees == quadAngle.getDegrees())
   {
      rotateDegrees(quadAngle);
   }
   else
   {
      rotateDegrees((double)degrees);
   }
}

void CTMatrix::rotateDegrees(double degrees)
{
   rotateRadians(degreesToRadians(degrees));
}

void CTMatrix::rotateRadians(double radians)
{
   double sinAng = sin(radians);
   double cosAng = cos(radians);

   CTMatrix rotateMatrix(cosAng,sinAng,-sinAng,cosAng,0.,0.);
   CTMatrix newMatrix;
   
   newMatrix = *this * rotateMatrix;
   *this = newMatrix;
}

void CTMatrix::rotateDegrees(const CQuadAngle& quadAngle)
{
   double sin,cos;

   quadAngle.getSinCos(sin,cos);
   
   CTMatrix rotateMatrix(cos,sin,-sin,cos,0.,0.);
   CTMatrix newMatrix;
   
   newMatrix = *this * rotateMatrix;
   *this = newMatrix;
}

void CTMatrix::rotateDegreesCtm(int degrees)
{
   CQuadAngle quadAngle(degrees);
   
   if (degrees == quadAngle.getDegrees())
   {
      rotateDegreesCtm(quadAngle);
   }
   else
   {
      rotateDegreesCtm((double)degrees);
   }
}

void CTMatrix::rotateDegreesCtm(double degrees)
{
   rotateRadiansCtm(degreesToRadians(degrees));
}

void CTMatrix::rotateRadiansCtm(double radians)
{
   double sinAng = sin(radians);
   double cosAng = cos(radians);

   CTMatrix rotateMatrix(cosAng,sinAng,-sinAng,cosAng,0.,0.);
   CTMatrix newMatrix;
   
   newMatrix = rotateMatrix * *this;
   *this = newMatrix;
}

void CTMatrix::rotateDegreesCtm(const CQuadAngle& quadAngle)
{
   double sin,cos;

   quadAngle.getSinCos(sin,cos);
   
   CTMatrix rotateMatrix(cos,sin,-sin,cos,0.,0.);
   CTMatrix newMatrix;
   
   newMatrix = rotateMatrix * *this;
   *this = newMatrix;
}

void CTMatrix::scale(double xFactor,double yFactor)
{
   CTMatrix scaleMatrix(xFactor,0.,0.,yFactor,0.,0.);
   CTMatrix newMatrix;
   
   newMatrix = *this * scaleMatrix;
   *this = newMatrix;
}

void CTMatrix::scaleCtm(double xFactor,double yFactor)
{
   CTMatrix scaleMatrix(xFactor,0.,0.,yFactor,0.,0.);
   CTMatrix newMatrix;
   
   newMatrix = scaleMatrix * *this;
   *this = newMatrix;
}

void CTMatrix::scale(double xyScale)
{
   scale(xyScale,xyScale);
}

void CTMatrix::scaleCtm(double xyScale)
{
   scaleCtm(xyScale,xyScale);
}

void CTMatrix::mirrorAboutYAxis(bool mirrorFlag)
{
   if (mirrorFlag) scale(-1.,1.);
}

void CTMatrix::mirrorAboutYAxisCtm(bool mirrorFlag)
{
   if (mirrorFlag) scaleCtm(-1.,1.);
}

CTMatrix CTMatrix::operator*(const CTMatrix& m2) const
{
   CTMatrix newMatrix;
   
   newMatrix.m_a      = (m_a * m2.m_a) + (m_b * m2.m_c);
   newMatrix.m_b      = (m_a * m2.m_b) + (m_b * m2.m_d);
   newMatrix.m_c      = (m_c * m2.m_a) + (m_d * m2.m_c);
   newMatrix.m_d      = (m_c * m2.m_b) + (m_d * m2.m_d);
   newMatrix.m_transX = (m_transX * m2.m_a) + (m_transY * m2.m_c) + m2.m_transX;
   newMatrix.m_transY = (m_transX * m2.m_b) + (m_transY * m2.m_d) + m2.m_transY;
   
   return newMatrix;
}

CPoint CTMatrix::transform(const CPoint& point) const
{
   CPoint newPoint;
   
   double x = (point.x * m_a) + (point.y * m_c) + m_transX;
   double y = (point.x * m_b) + (point.y * m_d) + m_transY;
   newPoint.x = (int)((x < 0.) ? x - .5 : x + .5);
   newPoint.y = (int)((y < 0.) ? y - .5 : y + .5);
   
   return newPoint;
}

void CTMatrix::transform(CPoint& point) const
{
   double x = (point.x * m_a) + (point.y * m_c) + m_transX;
   double y = (point.x * m_b) + (point.y * m_d) + m_transY;
   point.x = (int)((x < 0.) ? x - .5 : x + .5);
   point.y = (int)((y < 0.) ? y - .5 : y + .5);
}

CPoint2d CTMatrix::transform(const CPoint2d& point) const
{
   CPoint2d newPoint;
   
   double x = (point.x * m_a) + (point.y * m_c) + m_transX;
   double y = (point.x * m_b) + (point.y * m_d) + m_transY;
   newPoint.x = x;
   newPoint.y = y;
   
   return newPoint;
}

void CTMatrix::transform(CPoint2d& point) const
{
   double x = (point.x * m_a) + (point.y * m_c) + m_transX;
   double y = (point.x * m_b) + (point.y * m_d) + m_transY;
   point.x = x;
   point.y = y;
}

void CTMatrix::transform(double& xp,double& yp) const
{
   double x,y;
   
   x = (xp * m_a) + (yp * m_c) + m_transX;
   y = (xp * m_b) + (yp * m_d) + m_transY;
   xp = x;
   yp = y;
}

void CTMatrix::transform(float& xp,float& yp) const
{
   double x,y;
   
   x = (xp * m_a) + (yp * m_c) + m_transX;
   y = (xp * m_b) + (yp * m_d) + m_transY;
   xp = (float)x;
   yp = (float)y;
}

void CTMatrix::transform(int& xp,int& yp) const
{
   double x,y;
   
   x = (xp * m_a) + (yp * m_c) + m_transX;
   y = (xp * m_b) + (yp * m_d) + m_transY;
   xp = (int)((x < 0.) ? x - .5 : x + .5);
   yp = (int)((y < 0.) ? y - .5 : y + .5);
}

void CTMatrix::transformRadians(double& angleRadians) const
{
   double x0=0.0,y0=0.0,x1=1.0,y1=0.0;
   
   transform(x0,y0);
   transform(x1,y1);
   
   double dx = x1 - x0;
   double dy = y1 - y0;
   double xformAngle = atan2(dy,dx);
   
   angleRadians += xformAngle;
}

void CTMatrix::transform(double& angleDegrees) const
{
   double x0=0.0,y0=0.0,x1=1.0,y1=0.0;
   
   transform(x0,y0);
   transform(x1,y1);
   
   double dx = x1 - x0;
   double dy = y1 - y0;
   double xformAngle = atan2(dy,dx);
   
   xformAngle *= (180. / 3.1415926);
   angleDegrees += xformAngle;
}

void CTMatrix::transform(int& angleDegrees) const
{
   double fAngleDegrees = (double)angleDegrees;
   transform(fAngleDegrees);
   angleDegrees = DcaRound(fAngleDegrees);
}

void CTMatrix::transform(CQuadAngle& quadAngle) const
{
   double fAngleDegrees = quadAngle;
   transform(fAngleDegrees);
   quadAngle = fAngleDegrees;
}

void CTMatrix::equivRotationMirror(int& rotation,bool& mirror)
{  // only valid for orthoganal rotations
   int a,b,c,d;
   int invalid=0;
   
   a = ( (m_a < -.05) ? -1 : ((m_a > .05) ? 1 : 0) );
   b = ( (m_b < -.05) ? -1 : ((m_b > .05) ? 1 : 0) );
   c = ( (m_c < -.05) ? -1 : ((m_c > .05) ? 1 : 0) );
   d = ( (m_d < -.05) ? -1 : ((m_d > .05) ? 1 : 0) );
   
   rotation = 0;
   mirror = false;
   
   if (a == 0 && d == 0)
   {
      if (b == 1)
      {
         if (c == 1)
         {
            rotation = 90;
            mirror = true;
         }
         else if (c == -1)
         {
            rotation = 90;
            mirror = false;
         }
         else
         {
            invalid = 1;
         }
      }
      else if (b == -1)
      {
         if (c == 1)
         {
            rotation = 270;
            mirror = false;
         }
         else if (c == -1)
         {
            rotation = 270;
            mirror = true;
         }
         else
         {
            invalid = 2;
         }
      }
   }
   else if (b == 0 && c == 0)
   {
      if (d == 1)
      {
         if (a == 1)
         {
            rotation = 0;
            mirror = false;
         }
         else if (a == -1)
         {
            rotation = 0;
            mirror = true;
         }
         else
         {
            invalid = 3;
         }
      }
      else if (d == -1)
      {
         if (a == 1)
         {
            rotation = 180;
            mirror = true;
         }
         else if (a == -1)
         {
            rotation = 180;
            mirror = false;
         }
         else
         {
            invalid = 4;
         }
      }
      else
      {
         invalid = 5;
      }
   }
   
   if (invalid)
   {
      invalid = -invalid;
   }
}

bool CTMatrix::getMirror() const
{ 
   return getBasesVector().getMirror();
}

double CTMatrix::getScale() const
{ 
   return getBasesVector().getScale();
}

const CBasesVector& CTMatrix::getBasesVector() const
{
   if (m_basesVector == NULL)
   {
      m_basesVector = new CBasesVector();
      m_basesVector->transform(*this);
   }

   return *m_basesVector;
}

CString CTMatrix::getDescriptor() const
{
   return getBasesVector().getDescriptor();
}

//_____________________________________________________________________________
CTMatrices::CTMatrices()
{
}

CTMatrices::~CTMatrices()
{
   int i;
   
   for (i = 0;i < GetSize();i++)
   {
      CTMatrix* matrix = GetAt(i);
      
      delete matrix;
   }
   
   RemoveAll();
}

CTMatrix* CTMatrices::GetAt(int nindex)
{
   return (CTMatrix*) CObArray::GetAt(nindex);
}

void CTMatrices::SetAt(int nindex,CTMatrix* element)
{
   CObArray::SetAt(nindex,(CObject*)element);
}

void CTMatrices::SetAtGrow(int nindex,CTMatrix* element)
{
   CObArray::SetAtGrow(nindex,(CObject*)element);
}

int CTMatrices::Add(CTMatrix* element)
{
   return CObArray::Add((CObject*)element);
}

//_____________________________________________________________________________
CTMstate::CTMstate()
{
   m_sp = 0;
   m_ctm = new CTMatrix();
   m_matrices.SetAtGrow(m_sp,m_ctm);
   m_ictm = NULL;
}

CTMstate::~CTMstate()
{
   delete m_ictm;
}

CTMatrix CTMstate::getCtm()
{
   return *m_ctm;
}

void CTMstate::restoreCtm()
{
   if (m_sp > 0)
   {
      CTMatrix* matrix = m_matrices.GetAt(m_sp);
      delete matrix;
      m_matrices.SetAt(m_sp,NULL);
      
      m_sp--;
      m_ctm = m_matrices.GetAt(m_sp);
   }

   deleteIctm();
}

void CTMstate::saveCtm()
{
   m_ctm = new CTMatrix(*m_ctm);
   
   m_matrices.SetAtGrow(++m_sp,m_ctm);
}

void CTMstate::setCtm(CTMatrix& matrix)
{
   *m_ctm = matrix;

   deleteIctm();
}

void CTMstate::initCtm()
{
   m_ctm->initMatrix();

   deleteIctm();
}

CTMatrix* CTMstate::getIctm() const
{
   if (m_ictm == NULL)
   {
      m_ictm = new CTMatrix(*m_ctm);
      m_ictm->invert();
   }

   return m_ictm;
}

void CTMstate::translateCtm(CPoint offset)
{
   m_ctm->translateCtm((double)offset.x,(double)offset.y);

   deleteIctm();
}

void CTMstate::translateCtm(double x,double y)
{
   m_ctm->translateCtm(x,y);

   deleteIctm();
}

void CTMstate::rotateDegreesCtm(int angle)
{
   m_ctm->rotateDegreesCtm(angle);

   deleteIctm();
}

void CTMstate::rotateDegreesCtm(double angle)
{
   m_ctm->rotateDegreesCtm(angle);

   deleteIctm();
}

void CTMstate::rotateDegreesCtm(const CQuadAngle& quadAngle)
{
   m_ctm->rotateDegreesCtm(quadAngle);

   deleteIctm();
}

void CTMstate::scaleCtm(double xFactor,double yFactor)
{
   m_ctm->scaleCtm(xFactor,yFactor);

   deleteIctm();
}

CPoint CTMstate::transform(const CPoint& point) const
{
   return m_ctm->transform(point);
}

void CTMstate::transform(CPoint& point) const
{
   m_ctm->transform(point);
}

CPoint2d CTMstate::transform(const CPoint2d& point) const
{
   return m_ctm->transform(point);
}

void CTMstate::transform(CPoint2d& point) const
{
   m_ctm->transform(point);
}

void CTMstate::transform(double& x,double& y) const
{
   m_ctm->transform(x,y);
}

void CTMstate::transform(int& x,int& y) const
{
   m_ctm->transform(x,y);
}

void CTMstate::transform(double& angleDegrees) const
{
   m_ctm->transform(angleDegrees);
}

void CTMstate::transform(int& angleDegrees) const
{
   m_ctm->transform(angleDegrees);
}

void CTMstate::transform(CQuadAngle& quadAngle) const
{
   m_ctm->transform(quadAngle);
}

CPoint CTMstate::itransform(const CPoint& point) const
{
   return getIctm()->transform(point);
}

void CTMstate::itransform(CPoint& point) const
{
   getIctm()->transform(point);
}

CPoint2d CTMstate::itransform(const CPoint2d& point) const
{
   return getIctm()->transform(point);
}

void CTMstate::itransform(CPoint2d& point) const
{
   getIctm()->transform(point);
}

void CTMstate::itransform(double& x,double& y) const
{
   getIctm()->transform(x,y);
}

void CTMstate::itransform(int& x,int& y) const
{
   getIctm()->transform(x,y);
}

void CTMstate::itransform(double& angleDegrees) const
{
   getIctm()->transform(angleDegrees);
}

void CTMstate::itransform(int& angleDegrees) const
{
   getIctm()->transform(angleDegrees);
}

void CTMstate::itransform(CQuadAngle& quadAngle) const
{
   getIctm()->transform(quadAngle);
}

