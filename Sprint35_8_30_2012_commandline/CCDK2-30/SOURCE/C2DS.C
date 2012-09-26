/* -2 -3 */
/******************************* C2DS.C *********************************/
/********** Two-dimensional curve construction routines *****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#include <c2ndefs.h>
#include <c2adefs.h>
#include <c2cdefs.h>
#include <c2dpriv.h>
#include <c2ddefs.h>
#include <c2vmcrs.h>

#ifdef  SPLINE

STATIC  BOOLEAN c2d_spline_header0 __(( C2_CURVE DUMMY0 , INT DUMMY1 , 
            INT DUMMY2 )) ;
STATIC  void    c2d_spline_header1 __(( C2_CURVE DUMMY0 )) ;

/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_spline ( a, n ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a      ;   /* a[n] */
INT n ;
{
    RETURN ( c2d_spline_knots ( a, n, C2_DEFAULT ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_nurb ( a, n, d, knot ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a      ;   /* a[n] */
INT n, d ;
REAL *knot ;
{
    C2_CURVE curve = c2d_curve () ;

    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    if ( !c2d_spline_header0 ( curve, n, d ) ) {
        c2d_free_curve ( curve ) ;
        RETURN ( NULL ) ;
    }
    c2n_set_cthpts ( a, C2_CURVE_NURB(curve) ) ;
    c2n_set_knots ( knot, C2_CURVE_NURB(curve) ) ;
    c2d_spline_header1 ( curve ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_nu_bspline ( a, n, d, knot ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a      ;   /* a[n] */
INT n, d ;
REAL *knot ;
{
    C2_CURVE curve = c2d_curve () ;

    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    if ( !c2d_spline_header0 ( curve, n, d ) ) {
        c2d_free_curve ( curve ) ;
        RETURN ( NULL ) ;
    }
    c2n_set_knots ( knot, C2_CURVE_NURB(curve) ) ;
    c2n_set_ctpts ( a, C2_CURVE_NURB(curve) ) ;
    c2d_spline_header1 ( curve ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_spline_knots ( a, n, knot_options ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a      ;   /* a[n] */
INT n ;
C2_KNOT_OPTIONS knot_options ;
{
    RETURN ( c2d_spline_tan ( a, n, knot_options, 
        NULL, C2_NDEF_TAN, NULL, C2_NDEF_TAN ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_CURVE c2d_spline_frame ( n, d, knot_options ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT n, d ;
C2_KNOT_OPTIONS knot_options ;
{
    C2_CURVE curve ;

    curve = c2d_curve() ;
    if ( curve != NULL ) {
        if ( !c2d_spline_header0 ( curve, n, d ) ) {
            c2d_free_curve ( curve ) ;
            RETURN ( NULL ) ;
        }
        if ( knot_options == C2_UNIFORM ) 
            c2n_set_uniform_knots ( C2_CURVE_NURB(curve) ) ;
        else if ( knot_options == C2_CLSC_UNI ) 
            c2n_set_clsc_uni_knots ( C2_CURVE_NURB(curve) ) ;
        c2n_init_parms ( C2_CURVE_NURB(curve), C2_CURVE_PARM0(curve), 
            C2_CURVE_PARM1(curve) ) ;
    }   
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_spline_tan ( a, n, knot_options, 
                tan0, tan0_options, tan1, tan1_options ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a      ;   /* a[n] */
INT n ;
C2_KNOT_OPTIONS knot_options ;
PT2 tan0    ;
C2_TAN_OPTIONS tan0_options ;
PT2 tan1    ;
C2_TAN_OPTIONS tan1_options ;
{
    C2_CURVE curve ;

    curve = c2d_curve() ;
    if ( curve != NULL ) {
        if ( !c2d_spline_header0 ( curve, n+2, 4 ) ) {
            c2d_free_curve ( curve ) ;
            RETURN ( NULL ) ;
        }
        if ( !c2n_interp_tan ( a, n, knot_options, tan0, tan0_options, 
            tan1, tan1_options, C2_CURVE_NURB(curve) ) ) {
            c2d_free_curve ( curve ) ;
            curve = NULL ;
        }
        else
            c2d_spline_header1 ( curve ) ;
    }   
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_spline_clsd ( a, n, knot_options ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a      ;   /* a[n] */
INT n ;
C2_KNOT_OPTIONS knot_options ;
{
    C2_CURVE curve ;

    curve = c2d_curve() ;
    if ( curve != NULL ) {
        if ( !c2d_spline_header0 ( curve, n+3, 4 ) ) {
            c2d_free_curve ( curve ) ;
            RETURN ( NULL ) ;
        }
        if ( !c2n_interp_clsd ( a, n, knot_options, C2_CURVE_NURB(curve) ) ) {
            c2d_free_curve ( curve ) ;
            curve = NULL ;
        }
        else
            c2d_spline_header1 ( curve ) ;
    }   
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
STATIC BOOLEAN c2d_spline_header0 ( curve, n, d ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
INT n, d ;    
{
    C2_NURB nurb ;

    nurb = c2n_create_nurb ( n, d ) ;
    if ( nurb == NULL ) 
        RETURN ( FALSE ) ;
    C2_CURVE_NURB(curve) = nurb ;
/*
    C2_CURVE_NURB(curve) = c2n_create_nurb ( n, d ) ;
*/
    if ( C2_CURVE_NURB(curve) == NULL ) 
        RETURN ( FALSE ) ;
    C2_CURVE_TYPE(curve) = C2_NURB_TYPE ;
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
STATIC void c2d_spline_header1 ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
{
    c2n_init_parms ( C2_CURVE_NURB(curve), C2_CURVE_PARM0(curve), 
        C2_CURVE_PARM1(curve) ) ;
    c2n_box ( C2_CURVE_NURB(curve), C2_CURVE_BOX(curve) ) ;
}   


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_curve_to_spline ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
{
    C2_CURVE spline ;
    C2_NURB nurb ;

    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    if ( C2_CURVE_IS_SPLINE(curve) || C2_CURVE_IS_ELLIPSE(curve) ||
        C2_CURVE_IS_BEZIER(curve) ) {

        if ( c2n_trimmed ( C2_CURVE_NURB(curve), 
            C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve) ) ) {
            nurb = c2n_trim ( C2_CURVE_NURB(curve), 
                C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve) ) ;
            spline = c2d_curve() ;
            if ( spline == NULL ) 
                RETURN ( NULL ) ;
            c2n_set_w ( nurb, c2n_get_w ( C2_CURVE_NURB(curve) ) ) ;
            C2_CURVE_NURB(spline) = nurb ;
            C2_CURVE_TYPE(spline) = C2_NURB_TYPE ;
            C2_CURVE_T1(spline) = C2_CURVE_T1(curve) ;
            C2_CURVE_J1(spline) = C2_CURVE_J1(curve) - C2_CURVE_J0(curve) 
                + c2n_get_d ( C2_CURVE_NURB(curve) ) - 1 ;
            C2_CURVE_T0(spline) = C2_CURVE_T0(curve) ;
            C2_CURVE_J0(spline) = c2n_get_d ( C2_CURVE_NURB(curve) ) - 1 ;
            c2n_box ( C2_CURVE_NURB(spline), C2_CURVE_BOX(spline) ) ;
            RETURN ( spline ) ;
        }
        else
            RETURN ( c2d_copy ( curve ) ) ;
    }

    spline = c2d_curve() ;
    if ( spline == NULL ) 
        RETURN ( NULL ) ;
    C2_CURVE_TYPE(spline) = C2_NURB_TYPE ;
    C2_CURVE_T0(spline) = C2_CURVE_T0(curve) ;
    C2_CURVE_T1(spline) = C2_CURVE_T1(curve) ;
    c2a_box_copy ( C2_CURVE_BOX(curve), C2_CURVE_BOX(spline) ) ;

    if (C2_CURVE_IS_LINE(curve) ) {
        C2_CURVE_NURB(spline) = c2n_line ( C2_CURVE_LINE(curve) ) ;
        C2_CURVE_J0(spline) = 1 ;
        C2_CURVE_J1(spline) = 2 ;
    }
    else if (C2_CURVE_IS_ARC(curve) ) {
        C2_CURVE_TYPE(spline) = C2_ELLIPSE_TYPE ;
        C2_CURVE_NURB(spline) = c2n_arc ( C2_CURVE_ARC(curve), 
            C2_CURVE_IS_EXT_ARC(curve) ) ;
        C2_CURVE_J0(spline) = 2 ;
        C2_CURVE_J1(spline) = c2n_get_n ( C2_CURVE_NURB(spline) ) - 1 ;
    }
    else if (C2_CURVE_IS_PCURVE(curve) ) {
        C2_CURVE_NURB(spline) = c2n_pcurve ( C2_CURVE_PCURVE(curve) ) ;
        C2_CURVE_J0(spline) = C2_CURVE_J0(curve) + 2 ;
        C2_CURVE_J1(spline) = c2n_get_n ( C2_CURVE_NURB(spline) ) - 1 ;
    }

    RETURN ( C2_CURVE_NURB(spline) == NULL ? NULL : spline ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_CURVE c2d_spline_approx ( curve, parm0, parm1, m ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm0, parm1 ;
INT m ;
{
    PT2 *x, tan0, tan1 ;
    REAL scale ;
    C2_CURVE spline ;

    if ( m < 4 ) 
        RETURN ( NULL ) ;

    x = CREATE ( m-2, PT2 ) ;
    if ( !c2c_eval_equi_parm ( curve, parm0, parm1, m-2, x, tan0, tan1 ) ) {
        KILL ( x ) ;
        RETURN ( NULL ) ;
    }

    scale = ( ( parm1 == NULL ? C2_CURVE_T1(curve) : PARM_T(parm1) ) -
        ( parm0 == NULL ? C2_CURVE_T0(curve) : PARM_T(parm0) ) ) / 
        (REAL)(m-3) ;
    C2V_SCALE ( tan0, scale, tan0 ) ;
    C2V_SCALE ( tan1, scale, tan1 ) ;
    spline = c2d_spline_tan ( x, m-2, C2_CLSC_UNI, tan0, C2_DEF_TAN, tan1, 
        C2_DEF_TAN ) ;
    KILL ( x ) ;
    RETURN ( spline ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_CURVE c2d_ellipse_to_nurb ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
{
    C2_CURVE copy ;
    if ( !C2_CURVE_IS_ELLIPSE(curve) ) 
        RETURN ( NULL ) ;
    else {
        copy = c2d_copy ( curve ) ;
        if ( c2c_ellipse_to_nurb ( copy ) ) 
            RETURN ( copy ) ;
        else {
            c2d_free_curve ( copy ) ;
            RETURN ( NULL ) ;
        }
    }
}
#endif  /* SPLINE */

