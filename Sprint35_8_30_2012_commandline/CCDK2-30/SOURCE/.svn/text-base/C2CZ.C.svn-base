/* -2 -3 */
/******************************* C2CZ.C *********************************/
/**************** Two-dimensional curves ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2gdefs.h>
#include <c2ldefs.h>
#include <c2cdefs.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC void c2c_divide ( curve, n, parm0, parm1, pt, parm )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
INT n ;
PARM parm0, parm1 ;
PT2* pt ;
PARM parm ;
{
    REAL t0, t1 ;

    t0 = ( parm0 == NULL ) ? C2_CURVE_T0(curve) : PARM_T(parm0) ;
    t1 = ( parm1 == NULL ) ? C2_CURVE_T1(curve) : PARM_T(parm1) ;

    if ( C2_CURVE_IS_LINE(curve) ) 
        c2l_divide ( C2_CURVE_LINE(curve), n, t0, t1, pt, parm ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        c2g_divide ( C2_CURVE_ARC(curve), n, t0, t1, pt, parm ) ;
}


