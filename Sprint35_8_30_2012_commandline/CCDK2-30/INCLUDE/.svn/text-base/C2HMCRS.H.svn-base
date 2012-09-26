/******************************* C2HMCRS.H *********************************/
/*************** Include file for 2d definitions and macros ****************/
/***************** Homogeneous vector manipulation macros ******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef     C2HMCRS
#define     C2HMCRS
#ifdef      BBSMACROS

#ifdef SPLINE

#define C2H_SET_ZERO(A) {A[0] = 0.0; A[1] = 0.0; A[2] = 1.0;}
#define C2H_SET(X,Y,W,A) {(A)[0]=(X);(A)[1]=(Y);(A)[2]=(W);}
#define C2H_COPY(A,B)   {B[0] = A[0]; B[1] = A[1]; B[2] = A[2];}
#define C2H_PT(A,B)     {B[0] = A[0]; B[1] = A[1]; B[2] = 1.0;}
#define C2H_DIFF(A,B,C) {C[0]=A[0]*B[2]-B[0]*A[2];C[1]=A[1]*B[2]-B[1]*A[2];} 
#define C2H_SUB(A,V,B)  {B[0]=A[0]-V[0]*A[2];B[1]=A[1]-V[1]*A[2];}
#define C2H_HDIFF(A,B,C) {C[0]=A[0]/A[2]-B[0]/B[2];C[1]=A[1]/A[2]-B[1]/B[2];} 
#define C2H_HPT_PT(B,A) {A[0]=B[0]/B[2];A[1]=B[1]/B[2];} 

#else       /* !BBSMACROS */
#include    <c2hdefs.h>

#define     C2H_SET_ZERO    c2h_set_zero        
#define     C2H_COPY        c2h_copy        
#define     C2H_DIFF        c2h_diff        
#define     C2H_SUB         c2h_sub     
#define     C2H_PT_HPT      c2h_pt_hpt
#define     C2H_HPT_PT      c2h_hpt_pt
#endif  /* SPLINE */
#endif  /* BBSMACROS*/

#endif  /* C2HMCRS*/

