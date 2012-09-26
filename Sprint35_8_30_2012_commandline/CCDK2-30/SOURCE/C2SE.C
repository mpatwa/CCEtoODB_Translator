/* -2 -3 */
/******************************* C2SE.C *********************************/ 
/***************** Two-dimensional arcs *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <bbsdefs.h>
#ifdef SPLINE
#include <alpdefs.h>
#include <c2bdefs.h>
#include <c2sdefs.h>
#include <c2vdefs.h>
#include <c2hmcrs.h>
#include <c2vmcrs.h>

/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2s_get_ellipse_data ( a, ctr, 
            major_axis, minor_axis, angle ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *a  ;
PT2 ctr ;
REAL *major_axis, *minor_axis, *angle ;
{
    PT2 a0, a1, a2, x, y, p, q ;
    REAL r, w, k, m, pp, pq, qq, xq, yq, d, z0, z1, z2, t[2], t0, t1, u0, u1 ;
    INT mult[2] ;

    if ( IS_ZERO(a[1][2]) ) 
        RETURN ( FALSE ) ;
    k = sqrt ( a[0][2] * a[2][2] ) / a[1][2] ;
    if ( k <= 1.0 + BBS_ZERO ) 
        RETURN ( FALSE ) ;
    C2H_HPT_PT ( a[0], a0 ) ;
    C2H_HPT_PT ( a[1], a1 ) ;
    C2H_HPT_PT ( a[2], a2 ) ;
    C2V_SUB ( a0, a1, x ) ;
    C2V_SUB ( a2, a1, y ) ;

    m = sqrt ( k * k - 1.0 ) ;
    r = sqrt ( a[2][2] / a[0][2] ) ;

    w = 0.5 * ( 1.0 + 1.0 / ( m * m ) ) ;
    ctr[0] = a1[0] + w * ( x[0] + y[0] ) ;
    ctr[1] = a1[1] + w * ( x[1] + y[1] ) ;

    p[0] = m * m * ( x[0] - y[0] ) - x[0] - y[0] ;
    p[1] = m * m * ( x[1] - y[1] ) - x[1] - y[1] ;
    q[0] = 2.0 * m * x[0] ;
    q[1] = 2.0 * m * x[1] ;

    pp = C2V_DOT ( p, p ) ;
    pq = C2V_DOT ( p, q ) ;
    qq = C2V_DOT ( q, q ) ;

    if ( IS_SMALL(pq) ) 
        t[0] = 0.0 ;
    else {
        d = 0.5 * ( pp - qq ) ;
        w = sqrt ( pq * pq + d * d ) ;
        z0 = ( -d - w ) / pq ;
        w = sqrt ( z0 * z0 + 1.0 ) ;
        u0 = z0 + w ;
        u1 = z0 - w ;
        u0 = ( m * u0 - 1.0 ) / k ;
        u1 = ( m * u1 - 1.0 ) / k ;
        u0 = 1.0 + r * u0 ;
        u1 = 1.0 + r * u1 ;
        if ( IS_SMALL(u1) ) 
            t[0] = 1.0 / u0 ;
        else if ( IS_SMALL(u0) ) 
            t[0] = 1.0 / u1 ;
        else {
            t0 = 1.0 / u0 ;
            t1 = 1.0 / u1 ;
            if ( t0 > 0.0 ) {
                if ( t1 > 0.0 ) 
                    t[0] = ( t0 < t1 ? t0 : t1 ) ;
                else
                    t[0] = t0 ;
            }
            else {
                if ( t1 > 0.0 ) 
                    t[0] = t1 ;
                else
                    t[0] = t0 < t1 ? t1 : t0 ;
            }
        }
    }

    if ( !c2b_eval_pt ( a, 3, 0.0, t[0], p ) )
        RETURN ( FALSE ) ;
    C2V_SUB ( p, ctr, p ) ;
    if ( major_axis != NULL ) 
        *major_axis = C2V_NORM ( p ) ;
    if ( angle != NULL ) 
        *angle = c2v_atan2 ( p ) ;

    q[0] = - p[1] ;
    q[1] = p[0] ;
    c2v_normalize ( q, q ) ;
    if ( C2V_CROSS ( x, y ) > 0.0 ) {
        C2V_NEGATE ( q, q ) ;
    }

    xq = C2V_CROSS ( x, q ) ;
    yq = C2V_CROSS ( y, q ) ;

    d = k * k * ( xq - yq ) ;
    z0 = d - 2.0 * xq ;
    z1 = - k * ( xq + yq ) ;
    z2 = - d - 2.0 * yq ;
    alp_solve_bez3 ( z0, z1, z2, t, mult ) ;

    xq = C2V_DOT ( x, q ) ;
    yq = C2V_DOT ( y, q ) ;
    d = k * k * ( xq - yq ) ;
    z0 = d - 2.0 * xq ;
    z1 = - k * ( xq + yq ) ;
    z2 = - d - 2.0 * yq ;
    if ( mult[1] == 1 ) {
        w = ( z0 * ( 1.0 - t[0] ) + z1 * t[0] ) * ( 1.0 - t[0] ) + 
            ( z1 * ( 1.0 - t[0] ) + z2 * t[0] ) * t[0] ;
        if ( w < 0.0 ) 
            t[0] = t[1] ;
    }

    t[0]  = t[0] / ( t[0] + r * ( 1.0 - t[0] ) ) ;
    if ( !c2b_eval_pt ( a, 3, 0.0, t[0], p ) )
        RETURN ( FALSE ) ;
    C2V_SUB ( p, ctr, p ) ;
    if ( minor_axis != NULL ) 
        *minor_axis = C2V_NORM ( p ) ;
    RETURN ( TRUE ) ;
}
#endif /*SPLINE*/

