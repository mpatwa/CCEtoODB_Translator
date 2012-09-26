/* -2 -3 */
/******************************* C2CS.C *********************************/
/**************** Two-dimensional curves ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <alrdefs.h>
#include <c2gdefs.h>
#include <c2ldefs.h>
#include <c2pdefs.h>
#include <c2ndefs.h>
#include <c2cdefs.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_select ( curve, pt, tol, sel_parm, dist_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C2_CURVE curve ;
PT2 pt ;
REAL tol ;
PARM sel_parm ;
REAL *dist_ptr ;
{
    if ( C2_CURVE_IS_LINE(curve) ) {
        if ( sel_parm != NULL ) {
            if ( c2l_select ( C2_CURVE_LINE(curve), 
                C2_CURVE_T0(curve), C2_CURVE_T1(curve), pt, tol, 
                &(PARM_T(sel_parm)), dist_ptr ) ) {
                PARM_J(sel_parm) = 1 ;
                RETURN ( TRUE ) ;
            }
            else 
                RETURN ( FALSE ) ;
        }
        else 
            RETURN ( c2l_select ( C2_CURVE_LINE(curve), C2_CURVE_T0(curve), 
                C2_CURVE_T1(curve), pt, tol, NULL, dist_ptr ) ) ;
    }

    else if ( C2_CURVE_IS_ARC(curve) ) {
        if ( sel_parm != NULL ) {
            if ( c2g_select ( C2_CURVE_ARC(curve), 
                C2_CURVE_T0(curve), C2_CURVE_T1(curve), pt, tol, 
                &(PARM_T(sel_parm)), dist_ptr ) ) {
                alr_parm_set ( sel_parm ) ;
                RETURN ( TRUE ) ;
            }
            else 
                RETURN ( FALSE ) ;
        }
        else 
            RETURN ( c2g_select ( C2_CURVE_ARC(curve), 
                C2_CURVE_T0(curve), C2_CURVE_T1(curve), 
                pt, tol, NULL, dist_ptr ) ) ;
    }
    else if ( C2_CURVE_IS_PCURVE(curve) ) {
        if ( sel_parm != NULL ) {
            if ( c2p_select ( C2_CURVE_PCURVE(curve), 
                C2_CURVE_T0(curve), C2_CURVE_T1(curve), pt, tol, 
                &(PARM_T(sel_parm)), dist_ptr ) ) {
                alr_parm_set ( sel_parm ) ;
                RETURN ( TRUE ) ;
            }
            else 
                RETURN ( FALSE ) ;
        }
        else 
            RETURN ( c2p_select ( C2_CURVE_PCURVE(curve), C2_CURVE_T0(curve), 
                C2_CURVE_T1(curve), pt, tol, NULL, dist_ptr ) ) ;
    }

    else 
#ifdef  SPLINE
        RETURN ( c2n_select ( C2_CURVE_NURB(curve), C2_CURVE_PARM0(curve), 
            C2_CURVE_PARM1(curve), pt, tol, sel_parm, dist_ptr ) ) ;
#else  /*SPLINE*/
        RETURN ( FALSE ) ;
#endif  /*SPLINE*/
}

