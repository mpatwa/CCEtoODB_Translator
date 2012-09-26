/***************************** ALPDEFS.H ********************************/
/******************* Definitions of alp routines ************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!   (C) Copyright 1989 - 1995             Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef ALPDEFS
#define ALPDEFS
#include <aldefs.h>

extern  INT alp_solve2 __(( REAL DUMMY0 , REAL DUMMY1 , REAL* DUMMY2 ,
            INT* DUMMY3 )) ;
extern  INT alp_solve3 __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL* DUMMY3 , INT* DUMMY4 )) ;
extern  INT alp_solve_bez3 __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL* DUMMY3 , INT* DUMMY4 )) ;

#ifdef  SPLINE
#ifdef NEW_CODE
extern  INT alp_solve4 __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL* DUMMY4 , INT* DUMMY5 )) ;
extern  INT alp_solve5 __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , REAL* DUMMY5 , INT* DUMMY6 )) ;

#endif  /*NEW_CODE*/
#endif  /*SPLINE*/
extern  INT alp_ratio_extrs __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , REAL DUMMY5 ,  REAL* DUMMY6 ,
            INT* DUMMY7 )) ;
extern  BOOLEAN alp_ratio_min __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , REAL* DUMMY6 )) ;
extern  INT alp_quad_eqn __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL* DUMMY3 )) ;

#endif  /* ALPDEFS */

