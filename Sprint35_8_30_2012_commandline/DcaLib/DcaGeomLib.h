// $Header: /CAMCAD/5.0/Dca/DcaGeomLib.h 4     3/22/07 12:56a Kurt Van Ness $

#if !defined(__DcaGeomLib_h__)
#define __DcaGeomLib_h__

#pragma once

#include "DcaFpeq.h"

class Point2;
class CPnt;
class CPntList;

#if defined(EnableDcaCamCadDocLegacyCode)
class Mat2x2 
{
public:
   double m2[ 2 ][ 2 ];
};
#endif

void RotMat2( Mat2x2 *m, double az );
void TransPoint2( Point2 *v, int n, const Mat2x2 *m, double tx, double ty );
void Rotate(double x, double y, double angleDegrees, double *xrot, double *yrot);

// new functions with the DTransform parameter
double MeasurePointToLine(Point2 *point,const CPnt *a, const CPnt *b, double width,Point2 *result);
double MeasurePointToArc(Point2 *point,const CPnt *c, double r, double sa, double da, double aWidth,Point2 *result);
double MeasurePointToPntList(Point2 *point,const CPntList *pntList, double width, BOOL Centerline,Point2 *result);

// moved functions
double DistancePointToPoint(Point2 *p1, double width1, Point2 *p2, double width2);
double DistancePointToLine(Point2 *p,const Point2 *la, const Point2 *lb, double lWidth,Point2 *lResult);
double DistancePointToArc(Point2 *point,const Point2 *center, double radius, double sa, double da, double aWidth,Point2 *aResult);

double   DistanceLineToLine(Point2 *a1, Point2* a2, double width1, 
                            Point2 *b1, Point2 *b2, double width2, 
                            Point2 *p1, Point2 *p2);

double   DistanceLineToArc(Point2 *a, Point2 *b, double width1, 
                           Point2 *center,double radius, double sa, double da, double width2, 
                           Point2 *p_arc, Point2 *p_line);

double   DistanceArcToArc(Point2 *center1, double radius1, double start1, double delta1, double Width1, 
                          Point2 *center2, double radius2, double start2, double delta2, double Width2, 
                          Point2 *p1, Point2 *p2);

int      CrossArcToLine(Point2 *center,double radius, double start, double delta,
               Point2 *a, Point2 *b,  Point2 *p1, Point2 *p2);

int      CrossArcToArc(Point2 *startp1,Point2 *endp1,Point2 *center1, double radius1, double start1, double delta1, 
               Point2 *startp2,Point2 *endp2,Point2 *center2, double radius2, double start2,double delta2, 
               Point2 *p1, Point2 *p2);

double ClosestPoints(double distance, Point2 *a1, Point2 *a2, Point2 *b1, Point2 *b2, Point2 *p1, Point2 *p2);
BOOL IntersectSeg2(Point2 *a1, Point2 *a2, Point2 *b1, Point2 *b2, Point2 *pi);
BOOL FindPointOnLine(double p1x, double p1y, double p2x, double p2y, double distance, double *nx, double *ny);
BOOL IntersectInfiniteLine2(Point2 *a1, Point2 *a2, Point2 *b1, Point2 *b2, Point2 *pi);

void ArcPoint2Angle(double x1, double y1, double x2, double y2, double da,double *cx, double *cy, double *r, double *sa);
BOOL IsAngleOnArc(double sa, double da, double angle);
BOOL     IsPointOnArc(double sa, double da, double radius, Point2 *center, Point2 *p);
int CPntInSeg(CPnt *a, CPnt *b, CPnt *pnt, double tolerance = SMALLNUMBER);
double cross_product(double ax,double ay,double bx,double by);
void ArcCenter2( double x1, double y1, double x2,double y2, double cx, double cy,double *r, double *sa, double *da, int cw ); // clockwise = TRUE, counterclosckwise = FALSE

BOOL BoxTestSeg2(Point2 *a1, Point2 *a2, Point2 *b1, Point2 *b2);
int Point2InSeg(Point2 *a, Point2 *b, Point2 *pnt, double tolerance = SMALLNUMBER);
int PointInSeg(double ax, double ay, double bx, double by, double pntx, double pnty, double tolerance = SMALLNUMBER);

double Length(double x1, double y1, double x2, double y2);

int ArcFromStartEndRadius(double x1,double y1,double x2,double y2, double radius,int Clockwise,int Smaller, double *cx,double *cy,double *start,double *delta);
int ArcFrom2Pts( double x1, double y1, double x2, double y2, double radius, double *cx1, double *cy1, double *cx2, double *cy2 );


#endif
