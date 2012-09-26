/* -R __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************** M2R.C **********************************/
/****************** Routines for processing arc geometry *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alrdefs.h>
#include <alpdefs.h>
#include <c2rdefs.h>
#include <c2vdefs.h>
#include <m2ldefs.h>
#include <m2rdefs.h>
#include <c2lmcrs.h>
#include <c2rmcrs.h>
#include <c2vmcrs.h>

STATIC REAL m2r_area_sub __(( REAL, REAL, REAL, REAL, REAL, REAL, REAL )) ;
STATIC INT  m2r_adjust_2arcs_solve __(( REAL, REAL, REAL, REAL, 
            REAL, REAL, REAL, REAL[2], REAL[2] )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2r_hor_ray_int_no ( arc, t0, t1, pt, pt_on_arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
REAL t0, t1 ;
PT2 pt ;
BOOLEAN pt_on_arc ;
{
    REAL p, q, r, d2, d1, z0, z1, z2, root[2], x ;
    INT n, i, mult[2], m ;
    PT2 pp ;

    p = C2_ASEG_PT0(arc)[1] - pt[1] ;
    q = C2_ASEG_PT1(arc)[1] - pt[1] ;
    r = C2_ASEG_PT1(arc)[0] - C2_ASEG_PT0(arc)[0] ;
    d2 = C2_ASEG_D(arc) * C2_ASEG_D(arc) ;
    d1 = 1.0 + d2 ;
    z0 = d1 * p ;
    z1 = 0.5 * ( 1.0 - d2 ) * ( p + q ) - C2_ASEG_D(arc) * r ;
    z2 = d1 * q ;
    if ( z0 >= BBS_TOL && z1 >= BBS_TOL && z2 >= BBS_TOL ) 
        RETURN ( 0 ) ;
    if ( z0 <= -BBS_TOL && z1 <= -BBS_TOL && z2 <= -BBS_TOL ) 
        RETURN ( 0 ) ;
    n = alp_solve_bez3 ( z0, z1, z2, root, mult ) ;
    m = 0 ;
    for ( i=0 ; i<n && mult[i] ; i++ ) {
        if ( root[i]>=t0-BBS_ZERO && root[i]<=t1+BBS_ZERO ) {
            c2r_pt_tan ( arc, root[i], pp, NULL ) ;
            x = pp[0] - pt[0] ;
            if ( x > BBS_TOL ) {
                if ( IS_ZERO(root[i]-t0) || IS_ZERO(root[i]-t1) ) 
                    RETURN ( M2_END_PT_ON_RAY ) ;
                else
                    m += mult[i] ;
            }
            else if ( x >= - BBS_TOL && pt_on_arc ) 
                RETURN ( M2_PT_ON_CURVE ) ;
        }
    }
    RETURN ( m ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2r_ray_int_no ( arc, t0, t1, pt, c, s, pt_on_arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
REAL t0, t1 ;
PT2 pt ;
REAL c, s ;
BOOLEAN pt_on_arc ;
{
    C2_ASEG_S rot_arc ;

    c2v_rotate_pt_cs ( C2_ASEG_PT0(arc), pt, c, -s, C2_ASEG_PT0(&rot_arc) ) ;
    c2v_rotate_pt_cs ( C2_ASEG_PT1(arc), pt, c, -s, C2_ASEG_PT1(&rot_arc) ) ;
    C2_ASEG_D(&rot_arc) = C2_ASEG_D(arc) ;
    RETURN ( m2r_hor_ray_int_no ( &rot_arc, t0, t1, pt, pt_on_arc ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2r_x_max ( arc, t0, t1, x_max_ptr, t_max_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
REAL t0, t1 ;
REAL *x_max_ptr ;
REAL *t_max_ptr ;
{
    REAL x0, x1, x, t ;
    PT2 ctr, pt, p ;

    if ( c2r_ctr ( arc, ctr ) == NULL ) {
        C2_LINE_S line ;
        C2V_COPY ( C2_ASEG_PT0(arc), C2_LINE_PT0(&line) ) ;
        C2V_COPY ( C2_ASEG_PT1(arc), C2_LINE_PT1(&line) ) ;
        RETURN ( m2l_x_max ( &line, t0, t1, x_max_ptr, t_max_ptr ) ) ;
    }

    pt[0] = ctr[0] + c2r_rad ( arc ) ;
    pt[1] = ctr[1] ;
    if ( c2r_project ( arc, pt, &t, p ) && t0 <= t && t <= t1 ) 
        x = p[0] ;
    else {
        c2r_pt_tan ( arc, t0, pt, NULL ) ;
        x0 = pt[0] ;
        c2r_pt_tan ( arc, t1, pt, NULL ) ;
        x1 = pt[0] ;
        if ( x1 >= x0 + BBS_TOL ) {
            x = x1 ;
            t = t1 ;
        }
        else {
            x = x0 ;
            t = t0 ;
        }
    }

    if ( x >= *x_max_ptr + BBS_TOL ) {
        *x_max_ptr = x ;
        *t_max_ptr = t ;
        RETURN ( TRUE ) ;
    }
    else
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL m2r_area ( arc, t0, t1, origin ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
REAL t0, t1 ;
PT2 origin ;
{
    PT2 a, b, q, r ;
    REAL a_b, a_r, b_r, u, v ;

    C2R_QR ( arc, q, r ) ;
    C2R_UV ( arc, u, v ) ;

    if ( origin == NULL ) {
        C2V_COPY ( C2_ASEG_PT0(arc), a ) ;
        C2V_COPY ( C2_ASEG_PT1(arc), b ) ;
    }
    else {
        C2V_SUB ( C2_ASEG_PT0(arc), origin, a ) ;
        C2V_SUB ( C2_ASEG_PT1(arc), origin, b ) ;
    }

    a_b = C2V_CROSS ( a, b ) ;
    a_r = C2V_CROSS ( a, r ) ;
    b_r = C2V_CROSS ( b, r ) ;

    if ( IS_ZERO ( C2_ASEG_D(arc) ) )
        RETURN ( 0.5 * ( t1 - t0 ) * a_b ) ;

    RETURN ( m2r_area_sub ( a_b, a_r, b_r, C2_ASEG_D(arc), u, v, t1 ) - 
        m2r_area_sub ( a_b, a_r, b_r, C2_ASEG_D(arc), u, v, t0 ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC REAL m2r_area_sub ( a_b, a_r, b_r, d, u, v, t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL a_b, a_r, b_r, d, u, v, t ;
{
    REAL a0, a1, a2, s, z0, z1, z ;

    s = 1.0 - t ;
    z0 = u * s + t ;
    z1 = s + u * t ;
    z = s * z1 + t * z0 ;
    z0 *= s ;
    z1 *= t ;

    a0 = a_b * ( t - 0.5 ) / z ;
    a1 = 0.5 * ( b_r - a_r ) * atan ( d * ( 2.0 * t - 1.0 ) ) / ( v * v ) ;
    a2 = 0.5 * ( a_r * z0 + b_r * z1 ) / ( v * z ) ;

    RETURN ( 0.5 * ( a0 + a1 - a2 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2r_tan_angle ( arc, t0, t1, angle, t )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
REAL t0, t1, angle ;
REAL *t ;
{
    REAL u, v, z0, z1, z2, c, s, root[2] ;
    PT2 q, r ;
    INT i, m, n, mult[2] ;

    C2R_QR ( arc, q, r ) ;
    C2R_UV ( arc, u, v ) ;
    s = sin ( angle ) ;
    c = cos ( angle ) ;
    z0 = ( q[0] * u - r[0] * v ) * s - ( q[1] * u - r[1] * v ) * c ;
    z1 = q[0] * s - q[1] * c ;
    z2 = ( q[0] * u + r[0] * v ) * s - ( q[1] * u + r[1] * v ) * c ;
    n = alp_solve_bez3 ( z0, z1, z2, root, mult ) ;
    m = 0 ;
    for ( i=0 ; i<n && mult[i] ; i++ ) {
        if ( root[i]>=t0-BBS_ZERO && root[i]<=t1+BBS_ZERO ) {
            t[m] = root[i] ;
            m++ ;
        }
    }
    RETURN ( m ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE INT m2r_polygon ( arc, m, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
INT m ;
PT2 *a ;
{
    INT n ;
    PT2 q, r ;
    REAL t ;

    n = m ;
    if ( n == 3 && fabs(C2_ASEG_D(arc)) >= 1.0 - BBS_ZERO )
        n = 4 ;
#ifdef CCDK_DEBUG
{
    PT2 t0, t1 ;
    c2r_tan0 ( arc, t0 ) ;
    c2r_tan1 ( arc, t1 ) ;
}
#endif

    C2V_COPY ( C2_ASEG_PT0(arc), a[0] ) ;
    C2V_COPY ( C2_ASEG_PT1(arc), a[n-1] ) ;
    C2R_QR ( arc, q, r ) ;

    if ( n == 3 ) {
        C2V_MID_PT ( a[0], a[2], a[1] ) ;
        t = - C2_ASEG_D(arc) / ( 1.0 - C2_ASEG_D(arc) * C2_ASEG_D(arc) ) ;
        C2V_ADDT ( a[1], r, t, a[1] ) ;
    }

    else if ( n == 4 ) {
        t = 0.25 * ( 1.0 - C2_ASEG_D(arc) * C2_ASEG_D(arc) ) ;
        C2V_ADDC ( a[0], q, t, r, - 0.5*C2_ASEG_D(arc), a[1] ) ;
        C2V_ADDC ( a[3], q, -t, r, - 0.5*C2_ASEG_D(arc), a[2] ) ;
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE BOOLEAN m2r_furthest_pt ( arc, pt, arc_pt )
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C2_ASEG arc ;
PT2 pt ;
PT2 arc_pt ;
{
    PT2 ctr, vec, p ;
    REAL t, rad, dist0, dist1 ;

    if ( c2r_ctr ( arc, ctr ) == NULL ) {
        C2_LINE_S line ;
        C2V_COPY ( C2_ASEG_PT0(arc), C2_LINE_PT0(&line) ) ;
        C2V_COPY ( C2_ASEG_PT1(arc), C2_LINE_PT1(&line) ) ;
        RETURN ( m2l_furthest_pt ( &line, pt, 0.0, 1.0, arc_pt ) ) ;
    }

    C2V_SUB ( pt, ctr, vec ) ;
    dist1 = C2V_NORM ( vec ) ;

    if ( IS_SMALL(dist1) ) {
        c2r_pt_tan ( arc, 0.5, arc_pt, NULL ) ;
        RETURN ( TRUE ) ;
    }

    dist0 = C2V_DIST ( pt, C2_ASEG_PT0(arc) ) ;
    dist1 = C2V_DIST ( pt, C2_ASEG_PT1(arc) ) ;

    if ( dist0 < dist1 ) {
        C2V_COPY ( C2_ASEG_PT1(arc), arc_pt ) ;
        dist0 = dist1 ;
    }
    else {
        C2V_COPY ( C2_ASEG_PT0(arc), arc_pt ) ;
    }

    C2V_SUB ( pt, ctr, vec ) ;
    dist1 = C2V_NORM ( vec ) ;

    rad = c2r_rad ( arc ) ;
    dist1 = rad / dist1 ;
    C2V_SCALE ( vec, dist1, vec ) ;
    C2V_ADD ( ctr, vec, p ) ;
    if ( c2r_project ( arc, p, &t, NULL ) && 0.0 <= t && t <= 1.0 ) {
        dist1 = C2V_DIST ( p, pt ) ;
        if ( dist1 > dist0 ) {
            C2V_COPY ( p, arc_pt ) ;
            dist0 = dist1 ;
        }
    }
    C2V_SUB ( ctr, vec, p ) ;
    if ( c2r_project ( arc, p, &t, NULL ) && 0.0 <= t && t <= 1.0 ) {
        dist1 = C2V_DIST ( p, pt ) ;
        if ( dist1 > dist0 ) {
            C2V_COPY ( p, arc_pt ) ;
            dist0 = dist1 ;
        }
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE BOOLEAN m2r_increment ( arc, dir, t0, t1, w_ptr, pt, parm )
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C2_ASEG arc ;
INT dir ;
REAL t0, t1 ;
REAL *w_ptr ;
PT2 pt ;
PARM parm ;
{
    REAL t ;
    BOOLEAN status ;

    status = m2r_along ( arc, t0, *w_ptr, dir, &t ) && 
        ( t <= t1 + BBS_ZERO ) && ( t >= t0 - BBS_ZERO ) ;
    if ( status ) {
        *w_ptr = 0.0 ;
        c2r_pt_tan ( arc, t, pt, NULL ) ;
        if ( parm != NULL ) {
            PARM_SETJ ( t, 0, parm ) ;
        }
    }

    else {
        C2_ASEG_S arc1 ;
        *w_ptr -= c2r_length ( c2r_trim ( arc, t0, t1, &arc1 ) ) ;
    }
    RETURN ( status ) ;

#ifdef OLD_CODE
    PT2 ctr, p, p0, p1, v0, v1 ;
    REAL c, s, t, rad, angle, sweep ;
    BOOLEAN status ;

    if ( c2r_ctr ( arc, ctr ) == NULL ) {
        C2_LINE_S line ;
        C2V_COPY ( C2_ASEG_PT0(arc), C2_LINE_PT0(&line) ) ;
        C2V_COPY ( C2_ASEG_PT1(arc), C2_LINE_PT1(&line) ) ;
        RETURN ( m2l_increment ( &line, dir, t0, t1, l, w_ptr, pt, parm ) ) ;
    }

    rad = c2r_rad ( arc ) ;
    angle = l / rad ;
    c2r_pt_tan ( arc, t0, p0, NULL ) ;
    C2V_SUB ( p0, ctr, v0 ) ;
    C2V_SET ( -v0[1], v0[0], v1 ) ;
    if ( C2_ASEG_D(arc) < 0.0 ) {
        C2V_NEGATE ( v1, v1 ) ;
    }
    if ( dir != 1 ) {
        C2V_NEGATE ( v1, v1 ) ;
    }
    c = cos ( angle ) ;
    s = sin ( angle ) ;
    C2V_ADDC ( ctr, v0, c, v1, s, p ) ;

    status = c2r_project ( arc, p, &t, NULL ) && 
        ( ( dir == 1 ) ? ( t <= t1 ) : ( t >= t1 ) ) ;

    if ( status ) {
        *w_ptr = 0.0 ;
        if ( pt != NULL ) {
            C2V_COPY ( p, pt ) ;
        }
        if ( parm != NULL ) {
            PARM_SETJ ( t, 0, parm ) ;
        }
    }

    else {
        c2r_pt_tan ( arc, t1, p1, NULL ) ;
        C2V_SUB ( p1, ctr, v1 ) ;
        sweep = ( dir == 1 ) ? c2v_vecs_angle ( v0, v1 ) :
            c2v_vecs_angle ( v1, v0 ) ;
        *w_ptr += ( sweep * rad ) ;
    }
    RETURN ( status ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void m2r_set ( arc, p0, p1, t0, t1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
PT2 p0, p1 ;
REAL t0, t1 ;
{
    PT2 q0, q1 ;
    REAL m0, m1, n0, n1, k0, k1, z0, z1, u, p00, p01, p10, p11, w0, w1, w, v, 
        c0, c1, det ;

    C2R_UV ( arc, u, v ) ;
    m0 = ( 1.0 - t0 ) * ( 1.0 - t0 + u * t0 ) ;
    n0 = t0 * ( t0 + u * ( 1.0 - t0 ) ) ;
    m1 = ( 1.0 - t1 ) * ( 1.0 - t1 + u * t1 ) ;
    n1 = t1 * ( t1 + u * ( 1.0 - t1 ) ) ;
    k0 = 2.0 * v * t0 * ( 1.0 - t0 ) ;
    k1 = 2.0 * v * t1 * ( 1.0 - t1 ) ;
    z0 = m0 + n0 ;
    z1 = m1 + n1 ;

    w0 = m0 * n1 - m1 * n0 ;
    if ( IS_ZERO ( w0 ) ) 
        RETURN ;
    w1 = k0 * z1 - k1 * z0 ;
    w = ( n1 - m1 ) * z0 * p0[1] - ( n0 - m0 ) * z1 * p1[1] ;

    p00 = w0 * m0 + k0 * w1 ;
    p01 = w0 * n0 - k0 * w1 ;
    p10 = w0 * m1 + k1 * w1 ;
    p11 = w0 * n1 - k1 * w1 ;

    c0 = w0 * z0 * p0[0] + k0 * w ;
    c1 = w0 * z1 * p1[0] + k1 * w ;
    det = p00 * p11 - p10 * p01 ;
    if ( IS_SMALL(det) ) 
        RETURN ;
    q0[0] = ( c0 * p11 - c1 * p01 ) / det ;
    q1[0] = ( c1 * p00 - c0 * p10 ) / det ;

    c0 = z0 * p0[1] - k0 * ( p0[0] - p1[0] ) ;
    c1 = z1 * p1[1] - k1 * ( p0[0] - p1[0] ) ;

    q0[1] = ( n1 * c0 - n0 * c1 ) / w0 ;
    q1[1] = ( m0 * c1 - m1 * c0 ) / w0 ;

    C2V_COPY ( q0, C2_ASEG_PT0(arc) ) ;
    C2V_COPY ( q1, C2_ASEG_PT1(arc) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2r_is_small ( arc, t0, t1, tol ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
REAL t0, t1, tol ;
{
    REAL delta ;
    C2_ASEG_S arc1 ;

    c2r_trim ( arc, t0, t1, &arc1 ) ;
    delta = C2V_DIST ( C2_ASEG_PT1(&arc1), C2_ASEG_PT0(&arc1) ) ;
    if ( delta > tol ) 
        RETURN ( FALSE ) ;
    delta *= C2_ASEG_D(&arc1) ;
    if ( delta <= BBS_TOL * ( 1.0 - C2_ASEG_D(&arc1) * C2_ASEG_D(&arc1) ) )
        RETURN ( TRUE ) ;
    else if ( delta <= 2.0 * BBS_TOL ) 
        C2_ASEG_D(arc) = 0.0 ;
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2r_adjust_2arcs_tan ( arc0, t0_0, dir0, a1, p1, 
            arc1, t1_1, dir1, b0, q0 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc0, arc1 ;
REAL t0_0, t1_1 ;
INT dir0, dir1 ;
PT2 a1, p1, b0, q0 ;
{
    PT2 a0, p0, p, b1, q1, q, c, z, x[2] ;
    REAL r0, r1, d0, d1, cz, cp, cq, pz, qz, cc, pq, u[2], v[2] ;
    INT i, n ;

    c2r_pt_tan ( arc0, t0_0, a0, p0 ) ;
    c2r_pt_tan ( arc1, t1_1, b1, q1 ) ;
    c2v_normalize ( p0, p0 ) ;
    c2v_normalize ( q1, q1 ) ;
    if ( dir0 == -1 ) 
        C2V_NEGATE ( p0, p0 ) ;
    if ( dir1 == -1 ) 
        C2V_NEGATE ( q1, q1 ) ;
    C2V_SUB ( b1, a0, c ) ;
    C2V_MID_PT ( p1, q0, z ) ;
    c2v_normalize ( z, z ) ;
    p[0] = -p0[1] ;
    p[1] = p0[0] ;
    if ( C2_ASEG_D(arc0) < 0.0 ) {
        C2V_NEGATE ( p, p ) ;
    }
    q[0] = -q1[1] ;
    q[1] = q1[0] ;
    if ( C2_ASEG_D(arc1) < 0.0 ) {
        C2V_NEGATE ( q, q ) ;
    }

    cz = C2V_DOT ( c, z ) ;
    pz = C2V_DOT ( p, z ) ;
    qz = C2V_DOT ( q, z ) ;
    cp = C2V_DOT ( c, p ) ;
    cq = C2V_DOT ( c, q ) ;
    cc = C2V_DOT ( c, c ) ;
    pq = C2V_DOT ( p, q ) ;

    n = m2r_adjust_2arcs_solve ( 1.0 + pq, cp, -cq, -0.5*cc, -pz, qz, cz, 
        u, v ) ;

    if ( n <= 0 ) {
        C2V_MID_PT ( a1, b0, x[0] ) ;
        r0 = c2r_rad ( arc0 ) ;
        r1 = c2r_rad ( arc1 ) ;
        if ( r0 > r1 ) 
            c2r_ctr ( arc0, c ) ;
        else {
            c2r_ctr ( arc1, c ) ;
            r0 = r1 ;
        }
        cc = r0 / C2V_DIST ( x[0], c ) ;
        C2V_ADDU ( c, x[0], cc, x[0] ) ;
        n = 1 ;
    }

    else {
        for ( i = 0 ; i < n ; i++ ) {
            x[i][0] = ( v[i] * a0[0] + u[i] * b1[0] + 
                u[i] * v[i] * ( p[0] + q[0] ) ) / ( u[i] + v[i] ) ;
            x[i][1] = ( v[i] * a0[1] + u[i] * b1[1] + 
                u[i] * v[i] * ( p[1] + q[1] ) ) / ( u[i] + v[i] ) ;
        }
        if ( n < 2 ) {
            C2V_MID_PT ( a1, b0, x[n] ) ;
            n++ ;
        }
    }

    if ( n == 2 ) {
        d0 = C2V_DIST ( a1, x[0] ) + C2V_DIST ( b0, x[0] ) ;
        d1 = C2V_DIST ( a1, x[1] ) + C2V_DIST ( b0, x[1] ) ;
        if ( d1 < d0 ) 
            C2V_COPY ( x[1], x[0] ) ;
    }
    m2r_set1 ( arc0, x[0], t0_0, dir0 ) ;
    m2r_set0 ( arc1, x[0], t1_1, dir1 ) ;
#ifdef CCDK_DEBUG
{
PT2 x0, x1, u0, u1 ;
    c2g_pt_tan ( arc0, t0_0, x0, u0 ) ;
    c2g_pt_tan ( arc1, t1_1, x1, u1 ) ;
}
{
PT2 c0, c1 ;
REAL r0, r1 ;
c2r_ctr ( arc0, c0 ) ;
r0 = c2r_rad ( arc0 ) ;
c2r_ctr ( arc1, c1 ) ;
r1 = c2r_rad ( arc1 ) ;
}
#endif
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void m2r_set0 ( arc, p0, t1, dir )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
PT2 p0 ;
REAL t1 ;
INT dir ;
{
    PT2 p1, tan1 ;
    c2r_pt_tan ( arc, t1, p1, tan1 ) ;
    if ( dir == 1 ) 
        C2V_NEGATE ( tan1, tan1 ) ;
    c2r_2pts_tan ( p1, tan1, p0, arc ) ;
    if ( dir == 1 ) 
        c2r_reverse ( arc, arc ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void m2r_set1 ( arc, p1, t0, dir )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
PT2 p1 ;
REAL t0 ;
INT dir ;
{
    PT2 p0, tan0 ;
    c2r_pt_tan ( arc, t0, p0, tan0 ) ;
    if ( dir == -1 ) 
        C2V_NEGATE ( tan0, tan0 ) ;
    c2r_2pts_tan ( p0, tan0, p1, arc ) ;
    if ( dir == -1 ) 
        c2r_reverse ( arc, arc ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT m2r_adjust_2arcs_solve ( m0, m1, m2, m3, n1, n2, n3, u, v )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* finds non-negative solutions of the system                              */
/*  m0*u*v + m1*u + m2*v + m3 = 0                                          */
/*  n1*u + n2*v + n3 = 0                                                   */
REAL m0, m1, m2, m3, n1, n2, n3, u[2], v[2] ;
{
    REAL a0, a1, a2 ;
    INT n, i, mult[2] ;

    if ( IS_ZERO(n1) && IS_ZERO(n2) )
        RETURN ( 0 ) ;
    if ( fabs(n1) < fabs(n2) ) {
        a0 = m0 * n1 ;
        a1 = m0 * n3 - m1 * n2 + m2 * n1 ;
        a2 = m2 * n3 - m3 * n2 ;
        n = alp_solve3 ( a0, a1, a2, u, mult ) ;
        for ( i = 0 ; i < n ; i++ ) 
            v[i] = - ( n1 * u[i] + n3 ) / n2 ;
    }
    else {
        a0 = m0 * n2 ;
        a1 = m0 * n3 + m1 * n2 - m2 * n1 ;
        a2 = m1 * n3 - m3 * n1 ;
        n = alp_solve3 ( a0, a1, a2, v, mult ) ;
        for ( i = 0 ; i < n ; i++ ) 
            u[i] = - ( n2 * v[i] + n3 ) / n1 ;
    }
#ifdef CCDK_DEBUG
    for ( i = 0 ; i < n ; i++ ) {
        a0 = m0 * u[i] * v[i] + m1 * u[i] + m2 * v[i] + m3 ;
        a1 = n1 * u[i] + n2 * v[i] + n3 ;
    }
#endif
    if ( ( u[0] < -BBS_ZERO ) || ( v[0] < -BBS_ZERO ) ||
        ( u[0] < BBS_ZERO && v[0] < BBS_ZERO ) ) {
        n-- ;
        if ( n == 1 ) {
            u[0] = u[1] ;
            v[0] = v[1] ;
        }
    }
    if ( n == 2 && ( ( u[1] < - BBS_ZERO ) || ( v[1] < - BBS_ZERO ) ||
        ( u[1] < BBS_ZERO && v[1] < BBS_ZERO ) ) )
        n-- ;
    if ( n == 1 && ( ( u[0] < - BBS_ZERO ) || ( v[0] < - BBS_ZERO ) ||
        ( u[0] < BBS_ZERO && v[0] < BBS_ZERO ) ) )
        n-- ;
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2r_approx_n ( arc, t0, t1, acc )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
REAL t0, t1, acc ;
{
    REAL d, u, sweep ;
    INT n ;

    d = alr_trim_d ( C2_ASEG_D(arc), t0, t1 ) ;
    sweep = alr_sweep ( d ) ;
    u = C2V_DIST ( C2_ASEG_PT0(arc), C2_ASEG_PT1(arc) ) ;
    if ( IS_SMALL(u) ) 
        n = 1 ;
    else if ( fabs(d) * u <= 0.5 * acc ) 
        n = (INT) ( sqrt ( 0.5 * fabs(sweep) * 
            ( 1.0 + d*d ) * u / acc ) + 1.0 ) ;
    else {
        u = 2.0 * fabs(d) * acc / ( u * ( 1.0 + d * d ) ) ;
        u = sqrt ( u ) ;
        u = u > 1.0 ? HALF_PI : asin ( u ) ;
        n = (INT) ( 0.5 * fabs(sweep) / u + 1.0 ) ;
    }
    RETURN ( n ) ;
} 


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2r_along ( arc, t0, l, dir, t_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
REAL t0, l ;
INT dir ;
REAL *t_ptr ;
{
    REAL u, a0, a, rad, sweep ;

    u = C2V_DIST ( C2_ASEG_PT0(arc), C2_ASEG_PT1(arc) ) ;
    if ( IS_SMALL(u) ) {
        *t_ptr = t0 ;
        RETURN ( TRUE ) ;
    }

    u = l / u ;
    if ( dir == -1 )
        u = -u ;

    if ( IS_SMALL ( C2_ASEG_D(arc) ) ) {
        (*t_ptr) = t0 + u ;
        if ( (*t_ptr) > 1.0 + BBS_ZERO ) {
            (*t_ptr) = 1.0 ;
            RETURN ( FALSE ) ;
        }
        else if ( (*t_ptr) < - BBS_ZERO ) {
            (*t_ptr) = 0.0 ;
            RETURN ( FALSE ) ;
        }
        else
            RETURN ( TRUE ) ;
    }

    else if ( C2_ASEG_D(arc) > 0.0 ) {
        if ( t0 > 1.0 + BBS_ZERO ) {
            *t_ptr = 1.0 ;
            RETURN ( FALSE ) ;
        }
        else if ( t0 < - BBS_ZERO ) {
            *t_ptr = 0.0 ;
            RETURN ( FALSE ) ;
        }
        rad = c2r_rad ( arc ) ;
        a0 = alr_angle ( t0, C2_ASEG_D(arc) ) ;
        sweep = alr_sweep ( C2_ASEG_D(arc) ) ;
        a = a0 + (REAL)dir * l / rad ;
        if ( a > sweep ) {
            *t_ptr = 1.0 ;
            RETURN ( FALSE ) ;
        }
        if ( a < 0.0 ) {
            *t_ptr = 0.0 ;
            RETURN ( FALSE ) ;
        }
        *t_ptr = alr_parm ( a, C2_ASEG_D(arc) ) ;
        if ( *t_ptr > 1.0 + BBS_ZERO ) {
            *t_ptr = 1.0 ;
            RETURN ( FALSE ) ;
        }
        else if ( *t_ptr < - BBS_ZERO ) {
            *t_ptr = 0.0 ;
            RETURN ( FALSE ) ;
        }
        else
            RETURN ( TRUE ) ;
    }

    else {
        C2_ASEG_S arc1 ;
        REAL tt0, l1 ;

        c2r_reverse ( arc, &arc1 ) ;
        l1 = c2r_length ( arc ) - l ;
        tt0 = 1.0 - t0 ;
        RETURN ( m2r_along ( &arc1, tt0, l1, -dir, t_ptr ) ) ;
    }

#ifdef OLD_CODE
    else if ( dir == 1 ) {
        a0 = atan ( C2_ASEG_D(arc) * ( 2.0 * t0 - 1.0 ) ) ;
        a1 = a0 + u * 2.0 * C2_ASEG_D(arc) / 
            ( 1.0 + C2_ASEG_D(arc) * C2_ASEG_D(arc) ) ;
        if ( atan ( C2_ASEG_D(arc) ) <= a1 - BBS_ZERO ) {
            *t_ptr = 1.0 ;
            RETURN ( FALSE ) ;
        }
        else {
            (*t_ptr) = 0.5 * ( tan ( a1 ) / C2_ASEG_D(arc) + 1.0 ) ;
            RETURN ( TRUE ) ;
        }
    }
    else {
        a0 = atan ( C2_ASEG_D(arc) * ( 2.0 * t0 - 1.0 ) ) ;
        a1 = a0 + u * 2.0 * C2_ASEG_D(arc) / 
            ( 1.0 + C2_ASEG_D(arc) * C2_ASEG_D(arc) ) ;
        if ( atan ( C2_ASEG_D(arc) ) <= a1 - BBS_ZERO ) {
            *t_ptr = ( dir == 1 ) ? 1.0 : 0.0 ;
            RETURN ( FALSE ) ;
        }
        else {
            (*t_ptr) = 0.5 * ( tan ( a1 ) / C2_ASEG_D(arc) + 1.0 ) ;
            RETURN ( TRUE ) ;
        }
    }
#endif
} 

