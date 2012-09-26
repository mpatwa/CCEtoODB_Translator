// $Header: /CAMCAD/5.0/GeomLib.h 28    3/26/07 11:19a Kurt Van Ness $

/*************************************************************************/
/* Geometry library header file and function prototypes                */
/*************************************************************************/

/*************************************************************************/
/* Global Variables and Variable Types                                 */
/*************************************************************************/

#if !defined(__GeomLib_H__)
#define __GeomLib_H__ 

#pragma once

#include "dbutil.h"     // needed to define DbUnit
//#include "data.h"
#include "Fpeq.h"
#include "Dca.h"
#include "DcaBuildingBlockLib.h"
#include "DcaGeomLib.h"
#include "DcaPoint2.h"
#include <math.h>

// needed for text normalization
#define  GRTEXT_W_L              0x0000
#define  GRTEXT_W_C              0x0001
#define  GRTEXT_W_R              0x0002
#define  GRTEXT_H_B              0x0000
#define  GRTEXT_H_C              0x0010
#define  GRTEXT_H_T              0x0020

#ifndef PI
#define  PI             3.14159265358979323846
#endif

#define  PI2            (2.0*PI)

#define OVERLAP_NONE          0
#define OVERLAP_1in2          1
#define OVERLAP_2in1          2
#define OVERLAP_OVERLAPPING   3

typedef struct Point3
{
   double x;
   double y;
   double z;

   Point3(double X = 0.,double Y = 0.,double Z = 0.) :
      x(X), y(Y), z(Z) {}

} Point3;

typedef struct PolyPoint2
{
   Point2 *poly;
   int     points;
} PolyPoint2;

typedef struct Circle2 
{
   double r;
   double cx;
   double cy;

   Circle2(double R = 0.,double cX = 0.,double cY = 0.) :
      r(R), cx(cX), cy(cY) {}

} Circle2;

typedef struct Arc2 
{
   double r;
   double cx;
   double cy;
   double sa; /* start angle for arc */
   double da; /* delta angle for arc */

   Arc2(double R = 0.,double cX = 0.,double cY = 0.,double sA = 0.,double dA = 0.) :
      r(R), cx(cX), cy(cY), sa(sA), da(dA) {}

} Arc2;

typedef struct Mat3x3 
{
   double m3[ 3 ][ 3 ];
} Mat3x3;

#if !defined(EnableDcaCamCadDocLegacyCode)
class Mat2x2 
{
public:
   double m2[ 2 ][ 2 ];
};
#endif

typedef int BOOL;

/*************************************************************************/
/* Function Prototypes                                                   */
/*************************************************************************/


// angle in degree
//void Rotate (double x, double y, double rot, double *xrot, double *yrot);

int fill_polygon(Point2 *index,int sides,double step, int (*fillin)(double xa, double xe, double y));
double DegToRad(double degrees);
double RadToDeg(double radians);
double ArcTan2( double y, double x);
double LengthPoint3(Point3 *v1, Point3 *v2);
double LengthPoint2(Point2 *v1, Point2 *v2);
double ManLengthPoint2(Point2 *v1, Point2 *v2);
double ManLength(double x1, double y1, double x2, double y2);
int EqualPoint2(Point2 *v1, Point2 *v2);
void SetPoint2(Point2 *v, double x, double y, short f);

void     GenRect2( Point2 *polyline, int *vert, double tlx,
               double tly, double brx, double bry  );
void     arc2poly( double startangle, double deltaangle,
               double stepangle, double x, double y,
               double rad, Point2 *poly, int *cnt    );
double   StepAngle( double radius, double bulge );
void     ArcPoly2( double sx, double sy, double cx, double cy,
               double angle, double bulge,
               Point2 *polyline, int *vert, double sectorizeangle = PI/12 );
void     CirclePoly2( double cx, double cy, double radius,
               double bulge, Point2 *polyline, int *vert );
void     CirclePoint3( double x1, double y1, double x2,
               double y2, double x3, double y3,
               double *cx, double *cy, double *r );
void     ArcPoint3( double x1, double y1, double x2,
               double y2, double x3, double y3,
               double *cx, double *cy, double *r,
               double *sa, double *da );

void     CircleCenterPt( double xc, double yc, double x, double y, double *r );

//void     ArcCenter2( double x1, double y1, double x2,
//               double y2, double cx, double cy,
//               double *r, double *sa, double *da, int cw ); // clockwise = TRUE, counterclosckwise = FALSE
//void ArcPoint2Angle(double x1, double y1, double x2, double y2,
//      double da,double *cx,double *cy, double *r, double *sa);
//void     CenterArc2( double x1, double y1, double x2,
//               double y2, double da, double *cx, double *cy );
void     RotMat3( Mat3x3 *m, double ax, double ay, double az );

double   AngleStartEndDegree(double start, double end, int clockwise);
void     RotMat2( Mat2x2 *m, double az );
void     TransPoint3( Point3 *v, int n, Mat3x3 *m,
               double tx, double ty, double tz );
void     TransPoint2( Point2 *v, int n, const Mat2x2 *m,
               double tx, double ty );
void     NearestLinePoint2( Point2 *x1, Point2 *x2, Point2 *v, Point2 *p );
double   NearestPolyPoint2( Point2 *poly, int points, Point2 *v, Point2 *p );
BOOL BoxTestPoint(Point2 *p1, Point2 *p2, Point2 *p);
BOOL BoxTestBox(Point2 *a1, Point2 *a2, Point2 *b1, Point2 *b2);
BOOL BoxTestSeg2Width(Point2 *a1, Point2 *a2, double aWidthRadius, Point2 *b1, Point2 *b2, double bWidthRadius);
double SegToSeg2(Point2 *a1, Point2 *a2, Point2 *b1, Point2 *b2, Point2 *ai, Point2 *bi);

long     lround( double r );
double   dbunitround( double r );

double   Mat3ToRad(Mat3x3 *m);
double   Mat2ToRad(Mat2x2 *m);

int      Line_2_Box(double x1,double y1,double x2,double y2,double width,
                  Point2 *box,int *vert);

/* Combines Transformations */
void     CombineTransf3( Mat3x3 *m2, Point3 *t2, Mat3x3 *m1, Point3 *t1,
                              Mat3x3 *M, Point3 *T );
void     CombineTransf2( Mat2x2 *m2, Point2 *t2, Mat2x2 *m1, Point2 *t1,
                              Mat2x2 *M, Point2 *T );

int      PntInPoly( Point2 *p, Point2 *poly, int polycount );

//int      ArcFrom2Pts( double x1, double y1, double x2, double y2, double radius,
//                  double *cx1, double *cy1, double *cx2, double *cy2 );

//int      ArcFromStartEndRadius(double x1,double y1,double x2,double y2,
//                       double radius,int Clockwise,int Smaller,
//                       double *cx,double *cy,double *start,double *delta);

// knv 20060903 - since the tolerance parameter is not used, a default value was specified to this function can be called when it is omitted
int ArcFromStartEndRadiusDeltaXY(double x1, double y1, double x2, double y2, 
											double radius, bool clockwise, double startDeltaX, double startDeltaY,
											double *cx, double *cy, double *start, double *delta, double tolerance=0.);

void     FindCorners(double w1, double w2, Point2 *p1, Point2 *p2, 
                  Point2 *p1a, Point2 *p1b, Point2 *p2a, Point2 *p2b);

void     FindCorners_arc(double bulge,Point2 *p1,Point2 *p2,
                      double w1,double w2,Point2 *p1a,Point2 *p1b,
                      Point2 *p2a, Point2 *p2b);

void     FindShoot(double w1, double w2, double w3, double w4, Point2 *p1, Point2 *p2,
            Point2 *p3, Point2 *p4, Point2 *p);

// rotation in degrees
void     normalize_text(double *x,double *y,DbFlag flg,double rot,
                    unsigned char mir,double height,double len);

//BOOL     IsAngleOnArc(double sa, double da, double angle);


//double DistancePointToPoint(Point2 *p1, double width1, Point2 *p2, double width2);

//double DistancePointToLine(Point2 *p, 
//                           const Point2 *la, const Point2 *lb, double lWidth, 
//                           Point2 *lResult);

//double DistancePointToArc(Point2 *p, 
//                          const Point2 *center, double radius, double start, double delta, double aWidth,
//                          Point2 *aResult);


int      Lines_overlapping(Point2 *x1,Point2 *x2,Point2 *y1,Point2 *y2,
                       Point2 *p1,Point2 *p2);
int      SegmentsOverlap(double l1ax, double l1ay, double l1bx, double l1by, 
                    double l2ax, double l2ay, double l2bx, double l2by,
                    double accuracy);

double   LengthOfArc(Point2 *center, double radius, double startangle, double deltaangle);


/*************************************************************************/
/* End Geometry library header file and function prototypes              */
/*************************************************************************/

#endif
