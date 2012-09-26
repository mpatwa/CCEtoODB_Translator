/*************************** ALBDEFS.H *************************************/
/********* definitions and macros for al routines **************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef     ALBDEFS
#define     ALBDEFS
#include    <aldefs.h>
#include    <dmdefs.h>

#ifdef      SPLINE

#define ALB_SBDV0(A,D,DIM,BL,BR)   \
{ (BL)=ALLOCA((2*(D)-1)*(DIM),REAL) ;\
(BR)=(BL)+((D)-1)*(DIM); \
alb_sbdv0(A,D,DIM,BL) ; }

#define ALB_BRK0(A,D,DIM,T,BL,BR)   \
{ (BL)=ALLOCA((2*(D)-1)*(DIM),REAL) ;\
(BR)=(BL)+((D)-1)*(DIM); \
alb_brk0(A,D,DIM,T,BL) ; }

#define ALB_BRKL(A,D,DIM,T,B)   \
{ (B)=ALLOCA((D)*(DIM),REAL) ; alb_brkl(A,D,DIM,T,B) ; }

#define ALB_BRKR(A,D,DIM,T,B)   \
{ (B)=ALLOCA((D)*(DIM),REAL) ; alb_brkr(A,D,DIM,T,B) ; }

#define ALB_EVAL_MTX(A,D,DIM,MTX)   \
{ INT I,DD=D*DIM ; MTX=CREATE(D*D*DIM,REAL) ; \
for ( I=0 ; I<DD ; I++ ) MTX(I)=A[I] ; } 

extern  void    alb_sbdv0 __(( REAL* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 )) ;
extern  void    alb_sbdvl __(( REAL* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 )) ;
extern  void    alb_sbdvr __(( REAL* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 )) ;
extern  void    alb_brk0  __(( REAL* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL DUMMY3 , REAL* DUMMY4 )) ;
extern  void    alb_brkl  __(( REAL* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL DUMMY3 , REAL* DUMMY4 )) ;
extern  void    alb_brkr  __(( REAL* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL DUMMY3 , REAL* DUMMY4 )) ;
extern  void    alb_mult  __(( REAL* DUMMY0 , INT DUMMY1 , REAL* DUMMY2 ,
            INT DUMMY3 , REAL* DUMMY4 )) ;
extern  INT     alb_root_iter __(( REAL* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            REAL* DUMMY3 )) ;
extern  INT     alb_roots __(( REAL* DUMMY0 , INT DUMMY1 , DML_LIST DUMMY2 )) ;
#endif    /*SPLINE*/
extern  void    alb_eval  __(( REAL* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL DUMMY3 , INT DUMMY4 , REAL* DUMMY5 )) ;
extern  void alb_trim __(( REAL* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , REAL* DUMMY5 )) ;
#endif /*ALBDEFS*/

