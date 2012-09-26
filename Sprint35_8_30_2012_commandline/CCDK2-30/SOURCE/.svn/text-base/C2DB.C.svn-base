/* -2 -3 */
/****************************** C2DB.C **********************************/
/******* Two-dimensional Bezier curve construction routines *************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef  SPLINE
#include <aladefs.h>
#include <c2ndefs.h>
#include <c2adefs.h>
#include <c2ddefs.h>
#include <c2dpriv.h>
#include <c2vmcrs.h>

static BOOLEAN eval_through_node __(( PT2, PT2, PT2, PT2, C2_KNOT_OPTIONS, 
    REAL[2] )) ;

/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_bezier_ctl_pts ( b0, b1, b2, b3 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 b0, b1, b2, b3 ;
{
    C2_CURVE curve ;

    curve = c2d_curve() ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    C2_CURVE_TYPE(curve) = C2_BEZIER_TYPE ;
    C2_CURVE_T0(curve) = 0.0 ;
    C2_CURVE_J0(curve) = 3 ;
    C2_CURVE_T1(curve) = 1.0 ;
    C2_CURVE_J1(curve) = 3 ;

    C2_CURVE_NURB(curve) = c2n_bezier_ctl_pts ( b0, b1, b2, b3 ) ;
    if ( C2_CURVE_NURB(curve) == NULL ) {
        c2d_free_curve ( curve ) ;
        RETURN ( NULL ) ;
    }
    c2n_box ( C2_CURVE_NURB(curve), C2_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_CURVE c2d_bezier_dctl_hpts ( b, d ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *b ;
INT d ;
{
    C2_CURVE curve ;

    curve = c2d_curve() ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    C2_CURVE_TYPE(curve) = C2_BEZIER_TYPE ;
    C2_CURVE_T0(curve) = 0.0 ;
    C2_CURVE_J0(curve) = (INT)(d-1) ;
    C2_CURVE_T1(curve) = 1.0 ;
    C2_CURVE_J1(curve) = C2_CURVE_J0(curve) ;

    C2_CURVE_NURB(curve) = c2n_bezier_dctl_hpts ( b, d ) ;
    if ( C2_CURVE_NURB(curve) == NULL ) {
        c2d_free_curve ( curve ) ;
        RETURN ( NULL ) ;
    }
    c2n_box ( C2_CURVE_NURB(curve), C2_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_bezier_2pts_2tans ( b0, tan0, b1, tan1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 b0, tan0, b1, tan1 ;
{
    PT2 a0, a1 ;

    C2V_ADD ( b0, tan0, a0 ) ;
    C2V_SUB ( b1, tan1, a1 ) ;
    RETURN ( c2d_bezier_ctl_pts ( b0, a0, a1, b1 ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_bezier_through ( a0, a1, a2, a3 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 a0, a1, a2, a3 ;
{
    RETURN ( c2d_bezier_through_opt ( a0, a1, a2, a3, C2_DEFAULT ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_CURVE c2d_bezier_through_opt ( a0, a1, a2, a3, node_options ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 a0, a1, a2, a3 ;
C2_KNOT_OPTIONS node_options ;
{
    REAL through_node[2] ;

    if ( !eval_through_node ( a0, a1, a2, a3, node_options, through_node ) )
        RETURN ( NULL ) ;
    RETURN ( c2d_bezier_through_node ( a0, a1, a2, a3, through_node[0], 
        through_node[1] ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_CURVE c2d_bezier_through_node ( a0, a1, a2, a3, t0, t1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 a0, a1, a2, a3 ;
REAL t0, t1 ;
{
    PT2 b1, b2, c1, c2 ;
    REAL u0=1.0-t0, u1=1.0-t1 ;

    if ( ( t0 <= BBS_ZERO ) || ( t1 >= 1.0 - BBS_ZERO ) || 
        ( t1 - t0 <= BBS_ZERO ) )
        RETURN ( NULL ) ;

    b1[0] = ( a1[0] - a0[0] * u0*u0*u0 - a3[0] * t0*t0*t0 ) / ( 3.0 *u0*t0 ) ;
    b1[1] = ( a1[1] - a0[1] * u0*u0*u0 - a3[1] * t0*t0*t0 ) / ( 3.0 *u0*t0 ) ;
    b2[0] = ( a2[0] - a0[0] * u1*u1*u1 - a3[0] * t1*t1*t1 ) / ( 3.0 *u1*t1 ) ;
    b2[1] = ( a2[1] - a0[1] * u1*u1*u1 - a3[1] * t1*t1*t1 ) / ( 3.0 *u1*t1 ) ;

    c1[0] = ( b1[0] * t1 - b2[0] * t0 ) / ( t1 - t0 ) ;
    c1[1] = ( b1[1] * t1 - b2[1] * t0 ) / ( t1 - t0 ) ;
    c2[0] = ( b2[0] * u0 - b1[0] * u1 ) / ( t1 - t0 ) ;
    c2[1] = ( b2[1] * u0 - b1[1] * u1 ) / ( t1 - t0 ) ;

    RETURN ( c2d_bezier_ctl_pts ( a0, c1, c2, a3 ) ) ;
}



/*-------------------------------------------------------------------------*/
static BOOLEAN eval_through_node ( a0, a1, a2, a3, node_options, through_node ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 a0, a1, a2, a3 ;
C2_KNOT_OPTIONS node_options ;
REAL through_node[2] ;
{
    REAL d, d1, d2, d3 ;

    if ( node_options == C2_UNIFORM ) {
        through_node[0] = 1.0 / 3.0 ;
        through_node[1] = 2.0 / 3.0 ;
    }
    else {
        d1 = C2V_DIST ( a1, a0 ) ;
        d2 = C2V_DIST ( a2, a1 ) ;
        d3 = C2V_DIST ( a3, a2 ) ;

        if ( ( d1 <= BBS_TOL ) || ( d2 <= BBS_TOL ) || ( d3 <= BBS_TOL ) )
            RETURN ( FALSE ) ;

        if ( node_options == C2_DEFAULT || node_options == C2_NONUNI ) {
            d1 = sqrt ( d1 ) ;
            d2 = sqrt ( d2 ) ;
            d3 = sqrt ( d3 ) ;
        }

        d = d1 + d2 + d3 ;
        through_node[0] = d1 / d ;
        through_node[1] = ( d1 + d2 ) / d ;
    }
    RETURN ( TRUE ) ;
}

#endif  /* SPLINE */

