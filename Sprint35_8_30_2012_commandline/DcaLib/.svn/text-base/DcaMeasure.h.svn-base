// $Header: /CAMCAD/5.0/Dca/DcaMeasure.h 4     3/19/07 4:34a Kurt Van Ness $

#if !defined(__DcaMeasure_h__)
#define __DcaMeasure_h__

#pragma once

class CPoly;
class CTMatrix;
class CPnt;
class Point2;
class CPntList;

double MeasurePolyToPoly( 
   const CPoly& poly1,const CTMatrix& matrix1, bool Centerline1, double width1, 
   const CPoly& poly2,const CTMatrix& matrix2, bool Centerline2, double width2, 
   Point2& result1, Point2& result2);

double MeasurePolyToLine(const CPoly& poly, double pWidth, const CTMatrix& pMatrix, 
   const CPnt& la, const CPnt& lb, double lWidth, const CTMatrix& lMatrix, 
      Point2& pResult, Point2& lResult);

double MeasurePointToPoly(Point2& point,const CPoly& poly, const CTMatrix& matrix, bool Centerline, double width,Point2& result);

double MeasurePointToPntList(const Point2& point,const CPntList& pntList,double width,const CTMatrix& matrix,bool Centerline,Point2& result);

double MeasurePolyToArc(const CPoly& poly,            double pWidth, const CTMatrix& pMatrix, 
   const CPnt& aC, double aR, double aSa, double aDa, double aWidth, const CTMatrix& aMatrix, 
   Point2& pResult, Point2& aResult);

double MeasurePointToLine(const Point2& point,const CPnt& a, const CPnt& b, double width, const CTMatrix& lMatrix,Point2& result);

double MeasureLineToArc(const CPnt& la, const CPnt& lb, double lWidth, const CTMatrix& lMatrix,
   const CPnt& c, double r, double sa, double da,       double aWidth, const CTMatrix& aMatrix,
   Point2& lResult, Point2& aResult);

double MeasureLineToLine(
   const CPnt& l1a, const CPnt& l1b, double width1, const CTMatrix& matrix1, 
   const CPnt& l2a, const CPnt& l2b, double width2, const CTMatrix& matrix2,
   Point2& result1, Point2& result2);

double MeasurePointToArc(const Point2& point,const CPnt& c,double r,double sa,double da,double aWidth,const CTMatrix& aMatrix,Point2& result);

double MeasureArcToArc(
   const CPnt& c1, double r1, double sa1, double da1, double width1, const CTMatrix& matrix1, 
   const CPnt& c2, double r2, double sa2, double da2, double width2, const CTMatrix& matrix2,
   Point2& result1, Point2& result2);

#endif
