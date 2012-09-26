/* -2 -3 */
/******************************* C2C2.C *********************************/
/**************** Two-dimensional curves ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2gdefs.h>
#include <c2ldefs.h>
#include <c2ndefs.h>
#include <c2pdefs.h>
#include <c2adefs.h>
#include <c2cdefs.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2c_copy ( curve0, curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C2_CURVE curve0, curve ;
{
    C2_CURVE_TYPE(curve) = C2_CURVE_TYPE(curve0) ;
    C2_CURVE_T0(curve) = C2_CURVE_T0(curve0) ;
    C2_CURVE_J0(curve) = C2_CURVE_J0(curve0) ;
    C2_CURVE_T1(curve) = C2_CURVE_T1(curve0) ;
    C2_CURVE_J1(curve) = C2_CURVE_J1(curve0) ;

    if ( C2_CURVE_IS_LINE(curve) ) 
        C2_CURVE_LINE(curve) = c2l_create_copy ( C2_CURVE_LINE(curve0) ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        C2_CURVE_ARC(curve) = c2g_create_copy ( C2_CURVE_ARC(curve0) ) ;
    else if ( C2_CURVE_IS_PCURVE(curve) ) 
        C2_CURVE_PCURVE(curve) = c2p_create_copy ( C2_CURVE_PCURVE(curve0) ) ;
#ifdef  SPLINE
    else 
        C2_CURVE_NURB(curve) = c2n_create_copy_nurb ( C2_CURVE_NURB(curve0) ) ;
#endif  /* SPLINE */

    c2a_box_copy ( C2_CURVE_BOX(curve0), C2_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
}

