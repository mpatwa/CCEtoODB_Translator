/* -2 -3 */
/******************************* C2CM.C *********************************/
/**************** Two-dimensional curves ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <c2gdefs.h>
#include <c2ldefs.h>
#include <c2pdefs.h>
#include <c2ndefs.h>
#include <c2cdefs.h>
#include <c2vmcrs.h>

#ifdef  SPLINE
STATIC void length_eval __(( PT2* DUMMY0 , REAL* DUMMY1 )) ;
#endif  /*SPLINE*/

/*----------------------------------------------------------------------*/
BBS_PUBLIC REAL c2c_length ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C2_CURVE curve ;
{
#ifdef  SPLINE
    REAL length ;
#endif  /*SPLINE*/
    if ( C2_CURVE_IS_LINE(curve) )
        RETURN ( c2l_length ( C2_CURVE_LINE(curve), 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve) ) ) ;
    else if ( C2_CURVE_IS_ARC(curve) )
        RETURN ( c2g_length ( C2_CURVE_ARC(curve), 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve) ) ) ;
    else if ( C2_CURVE_IS_PCURVE(curve) )
        RETURN ( c2p_length ( C2_CURVE_PCURVE(curve), 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve) ) ) ;
    else 
#ifdef  SPLINE
        c2n_mass_prop ( C2_CURVE_NURB(curve), C2_CURVE_PARM0(curve), 
            C2_CURVE_PARM1(curve), 1, 1, LENGTH_TOL, length_eval, &length ) ;
        RETURN ( length ) ;
#else  /*SPLINE*/
        RETURN ( 0.0 ) ;
#endif  /*SPLINE*/
}

#ifdef  SPLINE
/*----------------------------------------------------------------------*/
STATIC void length_eval ( x, result ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *x ;
REAL *result ;
{
    *result = C2V_NORM ( x[1] ) ;
}
#endif  /*SPLINE*/

