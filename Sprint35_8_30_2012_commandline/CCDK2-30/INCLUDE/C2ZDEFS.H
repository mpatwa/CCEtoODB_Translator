/******************************** C2ZDEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     C2ZDEFS
#define     C2ZDEFS
#include <c2defs.h>
extern  void    c2z_free __(( C2_ZERO_ARC DUMMY0 )) ;
extern  C2_ZERO_ARC  c2z_create __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 )) ;
extern  BOOLEAN c2z_offset __(( C2_ZERO_ARC DUMMY0 , REAL DUMMY1 , 
            C2_ARC DUMMY2 )) ;
extern  BOOLEAN c2z_eval __(( C2_ZERO_ARC DUMMY0 , REAL DUMMY1 , 
            INT DUMMY2 , PT2* DUMMY3 )) ;
extern  BOOLEAN c2z_pt_tan __(( C2_ZERO_ARC DUMMY0 , REAL DUMMY1 , 
            PT2 DUMMY2 , PT2 DUMMY3 )) ;
#endif
