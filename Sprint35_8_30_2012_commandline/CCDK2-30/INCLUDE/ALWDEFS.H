/***************************** ALWDEFS.H ********************************/
/******************* Definitions of alw routines ************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!   (C) Copyright 1989 - 1995             Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef ALWDEFS
#define ALWDEFS
#include <aldefs.h>

extern  BOOLEAN alw_get_string __(( FILE* DUMMY0 , STRING DUMMY1 , 
            INT DUMMY2 )) ;
extern  BOOLEAN alw_get_int __(( FILE* DUMMY0 , INT* DUMMY1 )) ;
extern  BOOLEAN alw_get_real __(( FILE* DUMMY0 , REAL* DUMMY1 )) ;
extern  BOOLEAN alw_get_array __(( FILE* DUMMY0 , REAL* DUMMY1, INT DUMMY2 )) ;
extern  BOOLEAN alw_get_parm __(( FILE* DUMMY0 , PARM DUMMY1 )) ;
extern  BOOLEAN alw_put_string __(( FILE* DUMMY0 , STRING DUMMY1 , 
            STRING DUMMY2 )) ;
extern  BOOLEAN alw_put_int __(( FILE* DUMMY0 , INT DUMMY1 , 
            STRING DUMMY2 )) ;
extern  BOOLEAN alw_put_real __(( FILE* DUMMY0 , REAL DUMMY1 , 
            STRING DUMMY2 )) ;
extern  BOOLEAN alw_put_array __(( FILE* DUMMY0 , REAL* DUMMY1, INT DUMMY2 , 
            STRING DUMMY3 )) ;
extern  BOOLEAN alw_put_parm __(( FILE* DUMMY0 , PARM DUMMY1 , 
            STRING DUMMY2 )) ;
#endif  /* ALWDEFS */


