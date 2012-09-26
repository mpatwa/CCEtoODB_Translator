/******************************* V2DPRIV.H *********************************/
/*************************** View prototypes *******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef V2DPRIV
#define V2DPRIV

#include <vpdefs.h>
#include <c2defs.h>

#define V2D_PT_TRANSFORM(A,VP,B)    {   /* assume that A[2]=B[2]=1 */\
B[0]=VP_VIEWPORT_TRANSFORM(VP)[0][0]*A[0]+VP_VIEWPORT_TRANSFORM(VP)[0][1]*A[1]\
+VP_VIEWPORT_TRANSFORM(VP)[0][3];\
B[1]=VP_VIEWPORT_TRANSFORM(VP)[1][0]*A[0]+VP_VIEWPORT_TRANSFORM(VP)[1][1]*A[1]\
+VP_VIEWPORT_TRANSFORM(VP)[1][3];\
if ( !VP_VIEWPORT_CLIPPED(VP) ) {\
B[0] += VP_VIEWPORT_PX0(VP) ; B[1] += VP_VIEWPORT_PY0(VP);}\
VP_VIEWPORT_PREV_X(VP)=A[0];VP_VIEWPORT_PREV_Y(VP)=A[1];\
VP_VIEWPORT_PREV_Z(VP)=0.0;}

extern  void    v2d_pt_transform __(( PT2 DUMMY0 , VP_VIEWPORT DUMMY1 , 
            PT2 DUMMY2 )) ;
extern  void    v2d_define_inf_line __(( PT2 DUMMY0 , REAL DUMMY1 , 
            PT2 DUMMY2 , PT2 DUMMY3 , VP_VIEWPORT DUMMY4 )) ;
#ifdef SPLINE
extern  void    v2d_hpoly __(( HPT2* DUMMY0 , INT DUMMY1 , 
            VP_VIEWPORT DUMMY2 )) ;
#endif
extern  void    v2d_ctl_poly __(( C2_CURVE DUMMY0 , VP_VIEWPORT DUMMY1 )) ;
#endif /* V2DPRIV */

