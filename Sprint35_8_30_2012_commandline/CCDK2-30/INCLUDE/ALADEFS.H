/***************************** ALADEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!   (C) Copyright 1989 - 1995             Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef ALADEFS
#define ALADEFS
#include <aldefs.h>

extern  void    ala_binom __(( INT DUMMY0 , REAL* DUMMY1 )) ;
extern  REAL*   ala_copy __(( REAL* DUMMY0 , INT DUMMY1 , REAL* DUMMY2 )) ;
extern  void ala_min_max __(( REAL DUMMY0 , REAL DUMMY1 , REAL* DUMMY2 ,
            REAL* DUMMY3 )) ;
extern  REAL ala_max_fabs __(( REAL* DUMMY0 , INT DUMMY1 )) ;
extern  void    ala_set_zero __(( REAL* DUMMY0 , INT DUMMY1 )) ;
extern  void    ala_swap __(( REAL* DUMMY0 , REAL* DUMMY1 , INT DUMMY2 )) ;
extern  BOOLEAN ala_overlap __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 )) ;
#ifdef  SPLINE
extern  void    ala_add __(( REAL* DUMMY0 , REAL* DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 )) ;
extern  void    ala_scale __(( REAL* DUMMY0 , REAL DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 )) ;
extern  void    ala_addt __(( REAL* DUMMY0 , REAL* DUMMY1 , REAL DUMMY2 ,
            INT DUMMY3 , REAL* DUMMY4 )) ;
extern  void    ala_diff __(( REAL* DUMMY0 , INT DUMMY1 , REAL* DUMMY2 )) ;
extern  BOOLEAN ala_diff_small __(( REAL* DUMMY0 , REAL* DUMMY1 , INT DUMMY2 ,
            REAL DUMMY3 )) ;
extern  BOOLEAN ala_coinc __(( REAL* DUMMY0 , REAL* DUMMY1 , INT DUMMY2 )) ;
#endif  /* SPLINE */

#endif /* ALADEFS */

