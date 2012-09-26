// $Header: /CAMCAD/5.0/Dca/DcaTMatrix.h 4     3/12/07 12:48p Kurt Van Ness $

#if !defined(__DcaTMatrix_h__)
#define __DcaTMatrix_h__

#pragma once

class CBasesVector;
class CPoint2d;

//=============================================================================

//_____________________________________________________________________________
class CQuadAngle : public CObject
{
private:
   int m_degrees;

public:
   CQuadAngle();
   CQuadAngle(const CQuadAngle& quadAngle);
   CQuadAngle(int angle);
   CQuadAngle(double angle);
   ~CQuadAngle();
   
   int getDegrees() const { return m_degrees; };
   
   double getRadians() const;
   
   void getSinCos(int& sin,int& cos) const;
   void getSinCos(double& sin,double& cos) const;
   
   CQuadAngle& operator=(const CQuadAngle& other);
   operator int() const;  
   operator float() const;  
   operator double() const;  
};

//_____________________________________________________________________________
class CTMatrix : public CObject
{
   //DECLARE_DYNAMIC(CTMatrix);

private:
   double m_a;
   double m_b;
   double m_c;
   double m_d;
   double m_transX;
   double m_transY;

   mutable CBasesVector* m_basesVector;
   
public:
   CTMatrix();
   CTMatrix(double a,double b,double c,double d,double transX,double transY);
   CTMatrix(const CTMatrix& matrix);
   ~CTMatrix();
   void initMatrix();
   
   CTMatrix operator*(const CTMatrix& m2) const;
   CTMatrix& operator=(const CTMatrix& m2);
   
   // operations done on the transformation matrix
   void translate(double xOffset,double yOffset);
   void translate(CPoint offset);
   void translate(CPoint2d offset);
   void translateCtm(double xOffset,double yOffset);
   void translateCtm(CPoint offset);
   void translateCtm(CPoint2d offset);

   void scale(double xScale,double yScale);
   void scaleCtm(double xScale,double yScale);
   void scale(double xyScale);
   void scaleCtm(double xyScale);

   void mirrorAboutYAxis(bool mirrorFlag = true);
   void mirrorAboutYAxisCtm(bool mirrorFlag = true);

   void rotateDegrees(int degrees);
   void rotateDegrees(double degrees);
   void rotateDegrees(const CQuadAngle& quadAngle);
   void rotateRadians(double radians);
   void rotateDegreesCtm(int degrees);
   void rotateDegreesCtm(double degrees);
   void rotateDegreesCtm(const CQuadAngle& quadAngle);
   void rotateRadiansCtm(double radians);
   
   // operations done by the transformation matrix
   CPoint transform(const CPoint& point) const;
   void transform(CPoint& point) const;
   CPoint2d transform(const CPoint2d& point) const;
   void transform(CPoint2d& point) const;
   void transform(double& xp,double& yp) const;
   void transform(float& xp,float& yp) const;
   void transform(int& xp,int& yp) const;
   void transform(double& angleDegrees) const;
   void transform(int& angleDegrees) const;
   void transform(CQuadAngle& quadAngle) const;
   void transformRadians(double& angleRadians) const;

   // inversion
   CTMatrix& invert();
   void inverse(CTMatrix& other) const;
   CTMatrix inverted() const;

   void equivRotationMirror(int& rotation,bool& mirror);
   bool getMirror() const;
   double getScale() const;

   void releaseBasesVector();
   const CBasesVector& getBasesVector() const;
   CString getDescriptor() const;
};

//_____________________________________________________________________________
class CTMatrices : public CObArray
{
   //DECLARE_DYNAMIC(CTMatrices);

public:
   CTMatrices();
   ~CTMatrices();
   CTMatrix* GetAt(int nindex);
   void SetAt(int nindex,CTMatrix* element);
   void SetAtGrow(int nindex,CTMatrix* element);
   int Add(CTMatrix* element);
};

//_____________________________________________________________________________
class CTMstate : public CObject
{
   //DECLARE_DYNAMIC(CTMstate);

private:
   int m_sp;  // always >= 0, m_matrics.GetAt(m_sp) should always contain m_ctm
   CTMatrices m_matrices;
   CTMatrix* m_ctm;
   mutable CTMatrix* m_ictm;  // inverse of m_ctm
   
public:
   CTMstate();
   ~CTMstate();
   
   CTMatrix getCtm();
   CTMatrix* getIctm() const;

   void deleteIctm() { if (m_ictm != NULL) { delete m_ictm;  m_ictm = NULL; } }
   
   // operations that change the transformation matrix state
   void restoreCtm();
   void saveCtm();
   void setCtm(CTMatrix& matrix);
   void initCtm();
   void translateCtm(double xOffset,double yOffset);
   void translateCtm(CPoint offset);
   void scaleCtm(double xScale,double yScale);
   void rotateDegreesCtm(int degrees);
   void rotateDegreesCtm(double degrees);
   void rotateDegreesCtm(const CQuadAngle& quadAngle);
   
   // operations done by the transformation matrix state
   CPoint transform(const CPoint& point) const;
   void transform(CPoint& point) const;
   CPoint2d transform(const CPoint2d& point) const;
   void transform(CPoint2d& point) const;
   void transform(double& x,double& y) const;
   void transform(int& x,int& y) const;
   void transform(double& angleDegrees) const;
   void transform(int& angleDegrees) const;
   void transform(CQuadAngle& quadAngle) const;
   
   // operations done by the transformation matrix state
   CPoint itransform(const CPoint& point) const;
   void itransform(CPoint& point) const;
   CPoint2d itransform(const CPoint2d& point) const;
   void itransform(CPoint2d& point) const;
   void itransform(double& x,double& y) const;
   void itransform(int& x,int& y) const;
   void itransform(double& angleDegrees) const;
   void itransform(int& angleDegrees) const;
   void itransform(CQuadAngle& quadAngle) const;
};

#endif
