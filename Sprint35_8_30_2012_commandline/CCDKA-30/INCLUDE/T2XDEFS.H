/******************************* T2XDEFS.H *********************************/
/**************************  2-D crosshatch ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!       (C) Copyright 1989 - 1995       Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef T2XDEFS
#define T2XDEFS
#include <bbsdefs.h>
#include <dmdefs.h>
#include <c2defs.h>
#include <t2defs.h>
#include <m2defs.h>

extern  T2_XHATCH   t2x_region __(( T2_REGION DUMMY0 , PT2 DUMMY1 , 
            REAL DUMMY2 , REAL DUMMY3 )) ;
extern  T2_XHATCH   t2x_loop __(( T2_LOOP DUMMY0 , PT2 DUMMY1 , 
            REAL DUMMY2 , REAL DUMMY3 )) ;
extern  void        t2x_free __(( T2_XHATCH DUMMY0 )) ;
extern  void        t2x_reverse __(( T2_XHATCH DUMMY0 )) ;
extern  void t2x_info __(( T2_XHATCH DUMMY0 , FILE* DUMMY1 )) ;
#endif /* T2XDEFS */

