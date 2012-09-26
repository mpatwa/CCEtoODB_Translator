/***************************** M2BDEFS.H ***********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef M2BDEFS
#define M2BDEFS
#include <c2defs.h>
#include <dmdefs.h>
#include <albdefs.h>

extern  BOOLEAN m2b_x_max __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            REAL* DUMMY3 , REAL* DUMMY4 )) ;
extern  void    m2b_appr_arc __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY4 , DML_LIST DUMMY5 )) ;
extern  REAL    m2b_arc_deviation __(( HPT2*, INT, PT2, REAL )) ;
#endif /* M2BDEFS */

