/*************************** FNBDEFS.H *************************************/
/********* definitions and macros for fnb routines *************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!   (C) Copyright 1989 - 1995             Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef      SPLINE
#ifndef     FNBDEFS
#define     FNBDEFS
#include <fndefs.h>
#include <dmdefs.h>

extern  BOOLEAN     fnb_extr __(( HREAL* DUMMY0 , INT DUMMY1 , BOOLEAN DUMMY2 ,
            REAL* DUMMY3 , REAL* DUMMY4 )) ;
extern  BOOLEAN     fnb_extr_init __(( HREAL* DUMMY0 , INT DUMMY1 ,
            REAL DUMMY2 , REAL* DUMMY3 , REAL* DUMMY4 )) ;
extern  void        fnb_extrs __(( HREAL* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , INT DUMMY4 , DML_LIST DUMMY5 )) ;
extern  INT         fnb_coord_break_parm __(( REAL* DUMMY0 , INT DUMMY1 ,
            INT DUMMY2 , INT DUMMY3 , REAL* DUMMY4 , REAL* DUMMY5 , 
            REAL* DUMMY6 )) ;
extern  BOOLEAN     fnb_select __(( HREAL* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 ,  REAL* DUMMY5 , REAL* DUMMY6 )) ;
#endif /* FNBDEFS*/
#endif /* SPLINE */

