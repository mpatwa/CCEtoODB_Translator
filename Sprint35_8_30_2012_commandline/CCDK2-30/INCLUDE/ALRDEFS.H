/***************************** ALRDEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!   (C) Copyright 1989 - 1995             Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef ALRDEFS
#define ALRDEFS
#include <aldefs.h>

extern  REAL    alr_parm __(( REAL DUMMY0 , REAL DUMMY1 )) ;
extern  REAL    alr_angle __(( REAL DUMMY0 , REAL DUMMY1 )) ;
extern  void    alr_parm_set __(( PARM DUMMY0 )) ;
extern  REAL    alr_sbdv_d __(( REAL DUMMY0 )) ;
extern  REAL    alr_trim_d __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 )) ;
extern  REAL    alr_trim_parm __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 )) ;
extern  REAL    alr_sweep __(( REAL DUMMY0 )) ;

#endif /* ALRDEFS */

