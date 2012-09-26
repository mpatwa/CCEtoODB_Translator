/*******************************  QGLDEFS.H ********************************/
/**************** main include file for all QGL components *****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     QGLDEFS
#define     QGLDEFS
#define     BBSDEFS

#include    <qglcmpl.h>

#ifndef     NULL
#ifndef     STDIO
#include    <stdio.h>
#define     STDIO
#endif
#endif

#ifndef     FILE
#ifndef     STDIO
#include    <stdio.h>
#define     STDIO
#endif
#endif
#ifndef     BBS_PUBLIC
#define     BBS_PUBLIC
#endif
#ifndef     BBS_PRIVATE
#define     BBS_PRIVATE
#endif

#include    <math.h>
#define     EXTERN extern 
#define     GLOBAL

#ifdef      __MCRSFTC__
#include    <malloc.h>
#define     STATIC  static
#define     HYPOT(X,Y)  hypot((X),(Y))
#endif

#ifdef      __TURBOC__
#include    <alloc.h>
#define     STATIC  static
#define     HYPOT(X,Y)  hypot((X),(Y))
#endif

#ifdef      __HIGHC__
#include    <malloc.h>
#define     STATIC  
#define     HYPOT(X,Y)  hypot((X),(Y))
#endif

#ifdef      __WATCOM__
#include    <malloc.h>
#define     STATIC      static
#define     HYPOT(X,Y)  hypot((X),(Y))
#endif

#ifdef      __NDP__
#define     STATIC      static
#define     HYPOT(X,Y)  hypot((X),(Y))
typedef     long    fpos_t ;
extern      void *malloc();
extern      void *realloc();
#endif

#ifdef      __UNIX__
#define     fpos_t long
#define     STATIC
#define     HYPOT(X,Y)  hypot((X),(Y))
extern      void *malloc();
extern      void *realloc();
#endif

#ifdef      __HIGHC24__
int abs ( int ) ;
#endif
#ifdef      __HIGHC30__
int abs ( int ) ;
#endif

/**** Main data types *************/

typedef     double      REAL;
typedef     int         INT ;
typedef     void        *ANY ;
typedef     char        *STRING ;
typedef     unsigned    ATTR ;

#ifdef __NDP__
#define size_t int
#endif
/*
#ifdef __UNIX__
#ifndef size_t 
#define size_t int
#endif
#endif
*/
#ifdef      STACK_CHECK
#ifdef  __MCRSFTC__
size_t      STACK_SIZE ;
#define     STACK_AVAIL {STACK_SIZE=stackavail();}
#else
#define     STACK_AVAIL 
#endif
#else
#define     STACK_AVAIL 
#endif

#define     MALLOC(X,T) (T*) dmm_malloc ( sizeof(T)*(X))
#define     FREE(X)     if ( (X) != NULL ) dmm_free((ANY)(X)) ; else

#define     REALLOC(A,X,T)    {(A)=(T*)dmm_realloc((ANY)(A),sizeof(T)*(X));}

/**** Derived data types **********/

typedef     REAL        HREAL[2];   /* Homogeneous number */
typedef     INT         BOOLEAN;    /* Yes or No */
#ifndef     TRUE
#define     TRUE        (BOOLEAN)1
#endif
#ifndef     FALSE
#define     FALSE       (BOOLEAN)0
#endif

/******** Definitions of constants common to all components ****/

EXTERN      REAL        BBS_TOL ;           
EXTERN      REAL        BBS_ZERO ;           
EXTERN      REAL        BBS_CONV_TOL ;           
#define     LENGTH_TOL  1e-3    

#define     IS_ZERO(X)  (fabs(X)<=BBS_ZERO)
#define     IS_SMALL(X) (fabs(X)<=BBS_TOL)
#define     IS_CONVERGED(X)  (fabs(X)<=BBS_CONV_TOL)

#define     MAX_COUNT   10          /* used in iterations */


typedef     struct          parm_index {
            REAL            parameter ;
            INT             index ;
            } PARM_S, *PARM ;

#define     PARM_T(P)    ((P)->parameter)
#define     PARM_J(P)    ((P)->index)

#define     COPY_PARM(P0,P1)  {PARM_T(P1)=PARM_T(P0);PARM_J(P1)=PARM_J(P0);}
#define     SET_PARM(T,P)     {PARM_T(P)=T;}
#define     SET_PARMJ(T,J,P)  {PARM_T(P)=T;PARM_J(P)=J;}
#define     PARM_COPY(P0,P1)  {PARM_T(P1)=PARM_T(P0);PARM_J(P1)=PARM_J(P0);}
#define     PARM_SET(T,P)     {PARM_T(P)=T;}
#define     PARM_SETJ(T,J,P)  {PARM_T(P)=T;PARM_J(P)=J;}

#define     PF_NULL     0
typedef     INT         (*PF_INT)(); 
                            /* pointer to a function returning an INT */
typedef     BOOLEAN     (*PF_BOOLEAN)(); 
                            /* pointer to a function returning a BOOLEAN */
typedef     REAL        (*PF_REAL)(); 
                            /* pointer to a function returning a BOOLEAN */
typedef     void        (*PF_VOID)(); 
                            /* pointer to a function returning VOID */
typedef     ANY         (*PF_ANY)(); 
                            /* pointer to a function returning a pointer */
#ifdef __MCRSFTC__
typedef     void        (*PF_ACTION) __(( ANY )) ; 
typedef     void        (*PF_ACTION_DATA) __(( ANY, ANY )) ; 
typedef     void        (*PF_ACTION_DATA2) __(( ANY, ANY, ANY )) ; 
typedef     void        (*PF_DISPLAY) __(( ANY, INT, ANY )) ; 
typedef     BOOLEAN     (*PF_TEST) __(( ANY )) ; 
typedef     BOOLEAN     (*PF_TEST_DATA) __(( ANY, ANY )) ; 
typedef     BOOLEAN     (*PF_SORT) __(( ANY, ANY )) ; 
typedef     BOOLEAN     (*PF_SORT_DATA) __(( ANY, ANY, ANY )) ; 
typedef     REAL        (*PF_COMP) __(( ANY, ANY )) ; 
typedef     REAL        (*PF_COMP_DATA) __(( ANY, ANY, ANY )) ; 
typedef     ANY         (*PF_ANY1) __(( ANY )) ; 
                            /* pointer to a function returning a pointer */
typedef     INT         (*PF_INT1) __(( ANY )) ; 
#else
#ifdef __HIGHC__
typedef     void        (*PF_ACTION) __(( ANY )) ; 
typedef     void        (*PF_ACTION_DATA) __(( ANY, ANY )) ; 
typedef     void        (*PF_ACTION_DATA2) __(( ANY, ANY, ANY )) ; 
typedef     void        (*PF_DISPLAY) /*__(( ANY, INT, ANY )) */ () ; 
typedef     BOOLEAN     (*PF_TEST) __(( ANY )) ; 
typedef     BOOLEAN     (*PF_TEST_DATA) __(( ANY, ANY )) ; 
typedef     BOOLEAN     (*PF_SORT) __(( ANY, ANY )) ; 
typedef     BOOLEAN     (*PF_SORT_DATA) __(( ANY, ANY, ANY )) ; 
typedef     REAL        (*PF_COMP) __(( ANY, ANY )) ; 
typedef     REAL        (*PF_COMP_DATA) __(( ANY, ANY, ANY )) ; 
typedef     ANY         (*PF_ANY1) __(( ANY )) ; 
                            /* pointer to a function returning a pointer */
typedef     INT         (*PF_INT1) __(( ANY )) ; 
#else 
typedef     PF_VOID     PF_ACTION ;
typedef     PF_VOID     PF_ACTION_DATA ;
typedef     PF_VOID     PF_ACTION_DATA2 ;
typedef     PF_VOID     PF_DISPLAY ;
typedef     PF_BOOLEAN  PF_TEST ; 
typedef     PF_BOOLEAN  PF_TEST_DATA ; 
typedef     PF_BOOLEAN  PF_SORT ; 
typedef     PF_BOOLEAN  PF_SORT_DATA ; 
typedef     PF_REAL     PF_COMP ;
typedef     PF_ANY      PF_ANY1 ; 
typedef     PF_INT      PF_INT1 ; 
#endif  
#endif  

/******* World constants ******/

#ifndef     SQRT_2
#define     SQRT_2      1.414213562373095e0
#endif
#ifndef     SQRT_3
#define     SQRT_3      1.732050807568877e0
#endif
#ifndef     TWO_PI
#define     TWO_PI      6.283185307179586e0
#endif
#ifndef     PI
#define     PI          3.141592653589793e0
#endif
#ifndef     HALF_PI
#define     HALF_PI     1.570796326794897e0
#endif
#ifndef     PI_OVER_180 
#define     PI_OVER_180 1.74532925199433e-2
#endif

/******* Macros and other definitions */

#define     MAX(X,Y)    ((Y)<(X)?(X):(Y))
#define     MIN(X,Y)    ((Y)>(X)?(X):(Y))
#define     ATAN2(Y,X)  ( (Y)>=0.0 ? atan2((Y),(X)) : atan2((Y),(X))+TWO_PI ) 
#define     RETURN      return

#define     ALLOCA(X,T) (T*) alloca( sizeof(T)*(X))

#ifdef      CREATE_CHECK
#define     CREATE(X,T) (T*) qgl_create( sizeof(T)*(X))
#define     KILL(X)     qgl_kill((ANY)(X))
#else
#define     CREATE(X,T) (T*) malloc( sizeof(T)*(X))
#define     KILL(X)     free((ANY)(X))
#endif

/******************* function definitions *******************************/
extern  void*   dmm_malloc __(( size_t DUMMY0 ));
extern  void*   dmm_realloc __(( void* DUMMY0 , size_t DUMMY1 ));
extern  void    dmm_free __(( void* DUMMY0 ));

#define     bbs_is_small        qgl_is_small 
#define     bbs_is_zero         qgl_is_zero 
#define     bbs_put_world_size  qgl_put_world_size 
#define     bbs_put_tol         qgl_put_tol 
#define     bbs_get_world_size  qgl_get_world_size 
#define     bbs_get_tol         qgl_get_tol 
#define     bbs_atan2           qgl_atan2 

extern  BOOLEAN qgl_is_small __(( REAL DUMMY0 )) ;
extern  BOOLEAN qgl_is_zero __(( REAL DUMMY0 )) ;
extern  void    qgl_put_world_size __(( REAL DUMMY0 )) ;
extern  void    qgl_put_tol __(( REAL DUMMY0 )) ;
extern  REAL    qgl_get_world_size __(( void )) ;
extern  REAL    qgl_get_tol __(( void )) ;
extern  REAL    qgl_atan2 __(( REAL, REAL )) ;

#endif /* QGLDEFS*/

