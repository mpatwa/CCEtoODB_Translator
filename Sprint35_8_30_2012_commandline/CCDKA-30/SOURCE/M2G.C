/* -R __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************** M2G.C **********************************/
/****************** Routines for processing arc geometry *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alpdefs.h>
#include <alrdefs.h>
#include <dmldefs.h>
#include <c2gdefs.h>
#include <c2rdefs.h>
#include <c2vdefs.h>
#include <m2gdefs.h>
#include <m2ldefs.h>
#include <m2rdefs.h>
#include <c2gmcrs.h>
#include <c2lmcrs.h>
#include <c2vmcrs.h>

STATIC void m2g_set __(( C2_ARC, PT2, PT2, REAL, REAL )) ;
STATIC void m2g_set0 __(( C2_ARC, PT2, REAL, INT )) ;
STATIC void m2g_set1 __(( C2_ARC, PT2, REAL, INT )) ;
STATIC BOOLEAN m2g_adjust_2arcs_tan __(( C2_ARC, REAL, INT, PT2, PT2, 
            C2_ARC, REAL, INT, PT2, PT2 )) ;
STATIC INT m2g_intersect_coinc __(( C2_ARC, REAL, REAL, C2_ARC, REAL, REAL, 
    REAL*, REAL*, PT2*, INT* )) ;
STATIC BOOLEAN m2g_new_inters __(( REAL*, REAL*, INT, INT )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2g_hor_ray_int_no ( arc, t0, t1, pt, pt_on_arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL t0, t1 ;
PT2 pt ;
BOOLEAN pt_on_arc ;
{
    C2_ASEG_S carc ;
    INT m0, m1 ;

    if ( C2_ARC_ZERO_RAD(arc) ) {
        PT2 c ;
        c2g_ctr ( arc, c ) ;
        if ( !IS_SMALL(c[1]-pt[1]) ) 
            RETURN ( 0 ) ;
        else 
            RETURN ( c[0] - pt[0] >= - BBS_TOL ? M2_PT_ON_CURVE : 0 ) ; 
    }

    if ( t1 <= 1.0 + BBS_ZERO ) 
        RETURN ( m2r_hor_ray_int_no ( C2_ARC_SEG(arc), 
            t0, t1, pt, pt_on_arc ) ) ;
    else if ( t0 >= 1.0 - BBS_ZERO ) 
        RETURN ( m2r_hor_ray_int_no ( c2r_complement ( C2_ARC_SEG(arc), 
            &carc ), t0-1.0, t1-1.0, pt, pt_on_arc ) ) ;
    else {
        m0 = m2r_hor_ray_int_no ( C2_ARC_SEG(arc), t0, 1.0, pt, pt_on_arc ) ;
        if ( m0 < 0 ) 
            RETURN ( m0 ) ;
        m1 = m2r_hor_ray_int_no ( c2r_complement ( C2_ARC_SEG(arc), &carc ), 
            0.0, t1-1.0, pt, pt_on_arc ) ;
        if ( m1 < 0 ) 
            RETURN ( m1 ) ;
        RETURN ( m0 + m1 ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2g_ray_int_no ( arc, t0, t1, pt, c, s, pt_on_arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL t0, t1 ;
PT2 pt ;
REAL c, s ;
BOOLEAN pt_on_arc ;
{
    C2_ARC_S rot_arc ;

    c2g_rotate_cs ( arc, pt, c, -s, &rot_arc ) ;
    RETURN ( m2g_hor_ray_int_no ( &rot_arc, t0, t1, pt, pt_on_arc ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2g_x_max ( arc, t0, t1, x_max_ptr, t_max_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL t0, t1 ;
REAL *x_max_ptr ;
REAL *t_max_ptr ;
{
    C2_ASEG_S carc ;
    BOOLEAN status1, status2 ;
    REAL x1, x2, u1, u2 ;

    if ( C2_ARC_ZERO_RAD(arc) ) {
        PT2 c ;
        c2g_ctr ( arc, c ) ;
        if ( x_max_ptr != NULL ) 
            *x_max_ptr = c[0] ;
        if ( t_max_ptr != NULL ) 
            *t_max_ptr = t0 ;
    }

    if ( t1 <= 1.0 + BBS_ZERO ) 
        RETURN ( m2r_x_max ( C2_ARC_SEG(arc), t0, t1, 
            x_max_ptr, t_max_ptr ) ) ;
    else if ( t0 >= 1.0 - BBS_ZERO ) {
        status1 = m2r_x_max ( c2r_complement ( C2_ARC_SEG(arc), 
            &carc ), t0-1.0, t1-1.0, x_max_ptr, t_max_ptr ) ;
        if ( status1 ) 
            *t_max_ptr += 1.0 ;
        RETURN ( status1 ) ;
    }
    else {
        x1 = *x_max_ptr ;
        status1 = m2r_x_max ( C2_ARC_SEG(arc), t0, 1.0, &x1, &u1 ) ;
        x2 = *x_max_ptr ;
        status2 = m2r_x_max ( c2r_complement ( C2_ARC_SEG(arc), &carc ), 
            0.0, t1-1.0, &x2, &u2 ) ;
        if ( status1 && status2 ) {
            if ( x1 < x2 ) 
                status1 = FALSE ;
            else
                status2 = FALSE ;
        }
        if ( status1 ) {
            *x_max_ptr = x1 ;
            *t_max_ptr = u1 ;
        }
        else if ( status2 ) {
            *x_max_ptr = x2 ;
            *t_max_ptr = u2+1.0 ;
        }
        RETURN ( status1 || status2 ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL m2g_area ( arc, t0, t1, origin )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL t0, t1 ;
PT2 origin ;
{
    C2_ASEG_S carc ;
    if ( C2_ARC_ZERO_RAD(arc) ) 
        RETURN ( 0.0 ) ;
    if ( t1 <= 1.0 + BBS_ZERO ) 
        RETURN ( m2r_area ( C2_ARC_SEG(arc), t0, t1, origin ) ) ;
    else if ( t0 >= 1.0 - BBS_ZERO ) 
        RETURN ( m2r_area ( c2r_complement ( C2_ARC_SEG(arc), &carc ), 
            t0-1.0, t1-1.0, origin ) ) ;
    else 
        RETURN ( m2r_area ( C2_ARC_SEG(arc), t0, 1.0, origin ) + 
            m2r_area ( c2r_complement ( C2_ARC_SEG(arc), &carc ), 
            0.0, t1-1.0, origin ) ) ;
} 


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2g_tan_angle ( arc, t0, t1, angle, t )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL t0, t1, angle ;
REAL *t ;
{
    C2_ASEG_S carc ;
    INT i, n0, n1 ;

    if ( C2_ARC_ZERO_RAD(arc) ) 
        RETURN ( 0 ) ;
    if ( t1 <= 1.0 + BBS_ZERO ) 
        RETURN ( m2r_tan_angle ( C2_ARC_SEG(arc), t0, t1, angle, t ) ) ;
    else if ( t0 >= 1.0 - BBS_ZERO ) {
        n1 = m2r_tan_angle ( c2r_complement ( C2_ARC_SEG(arc), &carc ), 
            t0-1.0, t1-1.0, angle, t ) ;
        for ( i=0 ; i<n1 ; i++ ) 
            t[i] += 1.0 ;
        RETURN ( n1 ) ;
    }
    else {
        n0 = m2r_tan_angle ( C2_ARC_SEG(arc), t0, 1.0, angle, t ) ;
        n1 = m2r_tan_angle ( c2r_complement ( C2_ARC_SEG(arc), &carc ), 
            0.0, t1-1.0, angle, t+n0 ) ;
        for ( i=0 ; i<n1 ; i++ ) 
            t[n0+i] += 1.0 ;
        RETURN ( n0+n1 ) ;
    }
}



/*-------------------------------------------------------------------------*/
BBS_PRIVATE void m2g_set_ept0 ( arc, pt, t0, t1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
PT2 pt ;
REAL t0, t1 ;
{
    PT2 p1 ;
    c2g_pt_tan ( arc, t1, p1, NULL ) ;
    m2g_set ( arc, pt, p1, t0, t1 ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void m2g_set0 ( arc, p0, t1, dir )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
PT2 p0 ;
REAL t1 ;
INT dir ;
{
    m2r_set0 ( C2_ARC_SEG(arc), p0, t1, dir ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void m2g_set1 ( arc, p1, t0, dir )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
PT2 p1 ;
REAL t0 ;
INT dir ;
{
    m2r_set1 ( C2_ARC_SEG(arc), p1, t0, dir ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void m2g_set_ept1 ( arc, pt, t0, t1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
PT2 pt ;
REAL t0, t1 ;
{
    PT2 p0 ;
    c2g_pt_tan ( arc, t0, p0, NULL ) ;
    m2g_set ( arc, p0, pt, t0, t1 ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void m2g_set ( arc, p0, p1, t0, t1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
PT2 p0, p1 ;
REAL t0, t1 ;
{
    m2r_set ( C2_ARC_SEG(arc), p0, p1, t0, t1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL m2g_size ( arc, t0, t1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL t0, t1 ;
{
    PT2 p0, p1, p, q ;
    REAL s1, s2 ;

    c2g_pt_tan ( arc, t0, p0, NULL ) ;
    c2g_pt_tan ( arc, t1, p1, NULL ) ;
    c2g_pt_tan ( arc, 0.5*(t0+t1), p, NULL ) ;
    C2V_MID_PT ( p0, p1, q ) ;
    s1 = C2V_DIST ( p0, p1 ) ;
    s2 = C2V_DIST ( q, p ) ;
    RETURN ( s1 > s2 ? s1 : s2 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2g_adjust_line_arc ( line, t0_0, t0_1, dir0, arc, 
            t1_0, t1_1, dir1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
C2_ARC arc ;
REAL t0_0, t0_1, t1_0, t1_1 ;
INT dir0, dir1 ;
{
    INT i, type[2] ;
    REAL t0[2], t1[2], dot_pq, dot_px, dot_qx ;
    PT2 a0, a1, b0, b1, p[2], q, p0, p1, x ;

    if ( C2_ARC_ZERO_RAD(arc) )
        RETURN ( FALSE ) ;
#ifdef CCDK_DEBUG
{
PT2 c ;
REAL r ;
c2g_ctr ( arc, c ) ;
r = c2g_rad ( arc ) ;
}
#endif
    C2V_ADDU ( C2_LINE_PT0(line), C2_LINE_PT1(line), t0_0, a0 ) ;
    C2V_ADDU ( C2_LINE_PT0(line), C2_LINE_PT1(line), t0_1, a1 ) ;
    C2V_SUB ( C2_LINE_PT1(line), C2_LINE_PT0(line), q ) ;
    c2g_pt_tan ( arc, t1_0, b0, p0 ) ;
    c2g_pt_tan ( arc, t1_1, b1, p1 ) ;
    c2v_normalize ( q, q ) ;
    c2v_normalize ( p0, p0 ) ;
    if ( dir0 == -1 ) 
        C2V_NEGATE ( q, q ) ;
    if ( dir1 == -1 ) {
        C2V_NEGATE ( p0, p0 ) ;
        C2V_NEGATE ( p1, p1 ) ;
    }

    if ( !IS_ZERO ( C2V_CROSS ( p0, q ) ) && 
        ( c2g_int_line_arc_ext ( line, arc, t0, t1, p, type, NULL ) == 2 ) ) {
        t0[0] = C2V_DIST ( p[0], a1 ) + C2V_DIST ( p[0], b0 ) ; 
        t0[1] = C2V_DIST ( p[1], a1 ) + C2V_DIST ( p[1], b0 ) ; 
        i = ( t0[0] < t0[1] ) ? 0 : 1 ;
        m2l_set ( line, a0, p[i], t0_0, t0_1 ) ;
        m2g_set0 ( arc, p[i], t1_1, dir1 ) ;
#ifdef CCDK_DEBUG
{
PT2 c ;
REAL r ;
c2g_pt_tan ( arc, t1_1, b1, p1 ) ;
c2g_ctr ( arc, c ) ;
r = c2g_rad ( arc ) ;
}
#endif
        RETURN ( TRUE ) ;
    }

    c2v_normalize ( p1, p1 ) ;
    C2V_SUB ( b1, a0, x ) ;
    dot_pq = C2V_DOT ( p1, q ) ;
    dot_px = C2V_DOT ( p1, x ) ;
    dot_qx = C2V_DOT ( q, x ) ;
    if ( fabs(dot_pq) < 1.0 - BBS_ZERO ) {
        t0[0] = ( dot_qx + dot_px ) / ( 1.0 + dot_pq ) ;
        t0[1] = ( dot_qx - dot_px ) / ( 1.0 - dot_pq ) ;
        C2V_ADDT ( a0, q, t0[0], p[0] ) ;
        C2V_ADDT ( a0, q, t0[1], p[1] ) ;
        t0[0] = C2V_DIST ( p[0], a1 ) + C2V_DIST ( p[0], b0 ) ; 
        t0[1] = C2V_DIST ( p[1], a1 ) + C2V_DIST ( p[1], b0 ) ; 
        i = ( t0[0] < t0[1] ) ? 0 : 1 ;
        m2l_set ( line, a0, p[i], t0_0, t0_1 ) ;
        m2g_set0 ( arc, p[i], t1_1, dir1 ) ;
    }
    else 
        m2g_set0 ( arc, a1, t1_1, dir1 ) ;
#ifdef CCDK_DEBUG
{
PT2 x0, u0, x1, u1, c ;
REAL r ;
c2g_pt_tan ( arc, t1_0, x0, u0 ) ;
c2g_pt_tan ( arc, t1_1, x1, u1 ) ;
c2g_ctr ( arc, c ) ;
r = c2g_rad ( arc ) ;
}
#endif
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2g_adjust_arc_line ( arc, t0_0, t0_1, dir0, 
            line, t1_0, t1_1, dir1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
C2_LINE line ;
REAL t0_0, t0_1, t1_0, t1_1 ;
INT dir0, dir1 ;
{
    RETURN ( m2g_adjust_line_arc ( line, t1_1, t1_0, -dir1, 
            arc, t0_1, t0_0, -dir0 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2g_adjust_2arcs ( arc0, t0_0, t0_1, dir0, 
            arc1, t1_0, t1_1, dir1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc0, arc1 ;
REAL t0_0, t0_1, t1_0, t1_1 ;
INT dir0, dir1 ;
{
    INT i, type[2] ;
    REAL t0[2], t1[2] ;
    PT2 a1, b0, p[2], p1, q0, x ;

    if ( C2_ARC_ZERO_RAD(arc0) || C2_ARC_ZERO_RAD(arc1) )
        RETURN ( FALSE ) ;
#ifdef CCDK_DEBUG
{
PT2 c0, c1 ;
REAL r0, r1 ;
c2g_ctr ( arc0, c0 ) ;
r0 = c2g_rad ( arc0 ) ;
c2g_ctr ( arc1, c1 ) ;
r1 = c2g_rad ( arc1 ) ;
}
#endif
    c2g_pt_tan ( arc0, t0_1, a1, p1 ) ;
    c2g_pt_tan ( arc1, t1_0, b0, q0 ) ;
    c2v_normalize ( p1, p1 ) ;
    c2v_normalize ( q0, q0 ) ;
    if ( dir0 == -1 ) 
        C2V_NEGATE ( p1, p1 ) ;
    if ( dir1 == -1 ) 
        C2V_NEGATE ( q0, q0 ) ;

    if ( !IS_ZERO ( C2V_CROSS ( p1, q0 ) ) && 
        ( c2g_int_arc_arc_ext ( arc0, arc1, t0, t1, p, type, NULL ) == 2 ) ) {
        t0[0] = C2V_DIST ( p[0], a1 ) + C2V_DIST ( p[0], b0 ) ; 
        t0[1] = C2V_DIST ( p[1], a1 ) + C2V_DIST ( p[1], b0 ) ; 
        i = ( t0[0] < t0[1] ) ? 0 : 1 ;
        m2g_set1 ( arc0, p[i], t0_0, dir0 ) ;
        m2g_set0 ( arc1, p[i], t1_1, dir1 ) ;
#ifdef CCDK_DEBUG
    c2g_pt_tan ( arc0, t0_1, a1, p1 ) ;
    c2g_pt_tan ( arc1, t1_0, b0, q0 ) ;
#endif
        RETURN ( TRUE ) ;
    }

    if ( C2V_DOT ( p1, q0 ) < 0.0 ) {
        C2V_MID_PT ( a1, b0, x ) ;
        m2g_set1 ( arc0, x, t0_0, dir0 ) ;
        m2g_set0 ( arc1, x, t1_1, dir1 ) ;
#ifdef CCDK_DEBUG
    c2g_pt_tan ( arc0, t0_1, a1, p1 ) ;
    c2g_pt_tan ( arc1, t1_0, b0, q0 ) ;
{
PT2 c0, c1 ;
REAL r0, r1 ;
c2g_ctr ( arc0, c0 ) ;
r0 = c2g_rad ( arc0 ) ;
c2g_ctr ( arc1, c1 ) ;
r1 = c2g_rad ( arc1 ) ;
}
#endif
        RETURN ( TRUE ) ;
    }
    else 
        RETURN ( m2g_adjust_2arcs_tan ( arc0, t0_0, dir0, a1, p1, 
            arc1, t1_1, dir1, b0, q0 ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN m2g_adjust_2arcs_tan ( arc0, t0_0, dir0, a1, p1, 
            arc1, t1_1, dir1, b0, q0 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc0, arc1 ;
REAL t0_0, t1_1 ;
INT dir0, dir1 ;
PT2 a1, p1, b0, q0 ;
{
    RETURN ( m2r_adjust_2arcs_tan ( C2_ARC_SEG(arc0), t0_0, dir0, a1, p1, 
            C2_ARC_SEG(arc1), t1_1, dir1, b0, q0 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2g_is_small ( arc, t0, t1, tol ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL t0, t1, tol ;
{
    RETURN ( m2r_is_small ( C2_ARC_SEG(arc), t0, t1, tol ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE INT m2g_polygon ( arc, m, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
INT m ;
PT2 *a ;
{
    RETURN ( m2r_polygon ( C2_ARC_SEG(arc), m, a ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2g_int_line_arc ( line, t10, t11, arc, 
            t20, t21, vtx_status, t1, t2, pt, type ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
REAL t10, t11 ;
C2_ARC arc ;
REAL t20, t21 ;
BOOLEAN vtx_status;
REAL *t1, *t2 ;
PT2 *pt ;
INT *type ;
{
    INT i, m, n ;
    REAL zero = /* vtx_status ? ( 100.0 * BBS_ZERO ) : */ BBS_ZERO ;

    if ( t20 >= -zero && t21 <= 1.0 + zero ) 
        m = c2r_inters_line ( C2_ARC_SEG(arc), line, t2, t1, pt, type ) ;
    else
        m = c2g_int_line_arc_ext ( line, arc, t1, t2, pt, type, NULL ) ;

    n = 0 ;
    for ( i = 0 ; i < m ; i++ ) {
        if ( t1[i] >= t10 - zero && t1[i] <= t11 + zero &&
             t2[i] >= t20 - zero && t2[i] <= t21 + zero ) {
            if ( i != n ) {
                t1[n] = t1[i] ;
                t2[n] = t2[i] ;
                C2V_COPY ( pt[i], pt[n] ) ;
                type[n] = type[i] ;
            }
            n++ ;
        }
    }
    RETURN ( n ) ;
}   


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2g_int_arc_arc ( arc1, t10, t11, arc2, t20, t21, 
            vtx_status, no_coinc, t1, t2, pt, type ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc1, arc2 ;
REAL t10, t11, t20, t21 ;
BOOLEAN vtx_status, no_coinc ;
REAL *t1, *t2 ;
PT2 *pt ;
INT *type ;
{
    INT i, m, n ;
    REAL zero = /* vtx_status ? ( 100.0 * BBS_ZERO ) : */ BBS_ZERO ;

    if ( t20 >= -zero && t21 <= 1.0 + zero ) 
        m = c2r_intersect ( C2_ARC_SEG(arc1), C2_ARC_SEG(arc2), 
            t1, t2, pt, type ) ;
    else
        m = c2g_int_arc_arc_ext ( arc1, arc2, t1, t2, pt, type, NULL ) ;

    if ( !no_coinc && m < 0 ) 
        RETURN ( m2g_intersect_coinc ( arc1, t10, t11, arc2, t20, t21, 
            t1, t2, pt, type ) ) ;
    n = 0 ;
    for ( i = 0 ; i < m ; i++ ) {
        if ( t1[i] >= t10 - zero && t1[i] <= t11 + zero &&
             t2[i] >= t20 - zero && t2[i] <= t21 + zero ) {
            if ( i != n ) {
                t1[n] = t1[i] ;
                t2[n] = t2[i] ;
                C2V_COPY ( pt[i], pt[n] ) ;
                type[n] = type[i] ;
            }
            n++ ;
        }
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT m2g_intersect_coinc ( arc1, t10, t11, arc2, t20, t21, 
    t1, t2, int_pt, type ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc1, arc2 ;
REAL t10, t11, t20, t21 ;
REAL *t1, *t2 ;
PT2 *int_pt ;
INT *type ;
{
    REAL t ;
    INT n = 0 ;
    PT2 proj_pt ;
    BOOLEAN ext, ext1, ext2 ;

    ext1 = IS_ZERO(t10) && IS_ZERO(t11-2.0) ;
    ext2 = IS_ZERO(t20) && IS_ZERO(t21-2.0) ;
    ext = ext1 || ext2 ;

/***************
    n = 2 ;
    t1[0] = t2[0] = 0.0 ;
    t1[1] = t2[1] = 2.0 ;
    type[0] = type[1] = -2 ;
    RETURN ( n ) ;
***************/

    c2g_pt_tan ( arc1, t10, int_pt[n], NULL ) ;
    if ( c2g_project ( arc2, t20, t21, int_pt[n], &t, proj_pt ) ) {
        t1[n] = t10 ;
        t2[n] = t ;
        type[n] = ( ext || ( !IS_ZERO(t-t20) && !IS_ZERO(t-t21) ) ) ? -2 : -1 ;
        if ( m2g_new_inters ( t1, t2, type[n], n ) )
            n++ ;
    }

    c2g_pt_tan ( arc1, t11, int_pt[n], NULL ) ;
    if ( c2g_project ( arc2, t20, t21, int_pt[n], &t, proj_pt ) ) {
        t1[n] = t11 ;
        t2[n] = t ;
        type[n] = ( ext || ( !IS_ZERO(t-t20) && !IS_ZERO(t-t21) )) ? -2 : -1 ;
        if ( m2g_new_inters ( t1, t2, type[n], n ) )
            n++ ;
    }

    if ( ext1 && ext2 && n == 2 ) {
        if ( type[0] == -1 && type[1] == -1 && IS_ZERO(t1[0]-t10) && 
            IS_ZERO(t2[0]-t20) && IS_ZERO(t1[1]-t11) && IS_ZERO(t2[1]-t20) ) {
            t2[1] = t21 ;
            type[0] = type[1] = -2 ;
        }
        RETURN ( 2 ) ;
    }

    c2g_pt_tan ( arc2, t20, int_pt[n], NULL ) ;
    if ( c2g_project ( arc1, t10, t11, int_pt[n], &t, proj_pt ) ) {
        t1[n] = t ;
        t2[n] = t20 ;
        type[n] = ( ext || ( !IS_ZERO(t-t10) && !IS_ZERO(t-t11) )) ? -2 : -1 ;
        if ( m2g_new_inters ( t1, t2, type[n], n ) )
            n++ ;
    }

    c2g_pt_tan ( arc2, t21, int_pt[n], NULL ) ;
    if ( c2g_project ( arc1, t10, t11, int_pt[n], &t, proj_pt ) ) {
        t1[n] = t ;
        t2[n] = t21 ;
        type[n] = ( ext || ( !IS_ZERO(t-t10) && !IS_ZERO(t-t11) )) ? -2 : -1 ;
        if ( m2g_new_inters ( t1, t2, type[n], n ) )
            n++ ;
    }
    if ( n == 2 && type[0] == -1 && type[1] == -1 && IS_ZERO(t1[0]-t10) 
        && IS_ZERO(t2[0]-t20) && IS_ZERO(t1[1]-t11) && IS_ZERO(t2[1]-t21) ) 
        type[0] = type[1] = -2 ;
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN m2g_new_inters ( t1, t2, type, n ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *t1, *t2 ;
INT type, n ;
{
    INT i ;
    if ( type != -1 ) 
        RETURN ( TRUE ) ;
    for ( i=0 ; i<n ; i++ ) {
        if ( ( IS_ZERO(t1[i]-t1[n]) || IS_ZERO(fabs(t1[i]-t1[n])-2.0) ) && 
             ( IS_ZERO(t2[i]-t2[n]) || IS_ZERO(fabs(t2[i]-t2[n])-2.0) ) ) 
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE BOOLEAN m2g_furthest_pt ( arc, pt, t0, t1, arc_pt )
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C2_ARC arc ;
PT2 pt ;
REAL t0, t1 ;
PT2 arc_pt ;
{
    if ( t0 > BBS_ZERO || t1 < 1.0 - BBS_ZERO ) {
        C2_ASEG_S arc1 ;
        c2r_trim ( C2_ARC_SEG(arc), t0, t1, &arc1 ) ;
        RETURN ( m2r_furthest_pt ( &arc1, pt, arc_pt ) ) ;
    }
    else
        RETURN ( m2r_furthest_pt ( C2_ARC_SEG(arc), pt, arc_pt ) ) ;
}


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE BOOLEAN m2g_increment ( arc, dir, t0, t1, w_ptr, pt, parm )
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C2_ARC arc ;
INT dir ;
REAL t0, t1 ;
REAL *w_ptr ;
PT2 pt ;
PARM parm ;
{
    BOOLEAN status ;
    REAL t ;

    status = m2g_along ( arc, t0, *w_ptr, dir, &t ) && 
        ( t <= t1 + BBS_ZERO ) && ( t >= t0 - BBS_ZERO ) ;
    if ( status ) {
        *w_ptr = 0.0 ;
        c2g_pt_tan ( arc, t, pt, NULL ) ;
        if ( parm != NULL ) {
            PARM_SETJ ( t, 0, parm ) ;
        }
    }

    else {
        *w_ptr -= c2g_length ( arc, t0, t1 ) ;
    }
    RETURN ( status ) ;

#ifdef OLD_CODE
    if ( C2_ARC_ZERO_RAD(arc) ) 
        RETURN ( FALSE ) ;
    else
        RETURN ( m2r_increment ( C2_ARC_SEG(arc), dir, t0, t1, 
            w_ptr, pt, parm ) ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2g_approx_n ( arc, t0, t1, acc )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL t0, t1, acc ;
{
    C2_ASEG_S carc ;
    if ( C2_ARC_ZERO_RAD(arc) ) 
        RETURN ( 0 ) ;
    if ( t1 <= 1.0 + BBS_ZERO ) 
        RETURN ( m2r_approx_n ( C2_ARC_SEG(arc), t0, t1, acc ) ) ;
    else if ( t0 >= 1.0 - BBS_ZERO ) 
        RETURN ( m2r_approx_n ( c2r_complement ( C2_ARC_SEG(arc), &carc ), 
            t0-1.0, t1-1.0, acc ) ) ;
    else 
        RETURN ( m2r_approx_n ( C2_ARC_SEG(arc), t0, 1.0, acc ) + 
            m2r_approx_n ( c2r_complement ( C2_ARC_SEG(arc), &carc ), 
            0.0, t1-1.0, acc ) ) ;
} 


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2g_along ( arc, t0, l, dir, t_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL t0, l ;
INT dir ;
REAL *t_ptr ;
{
    PT2 ctr, pt, pt1, vec, tan ;
    REAL rad, c, s, angle ;
    BOOLEAN status ;

    rad = c2g_rad ( arc ) ;
    c2g_ctr ( arc, ctr ) ;
    c2g_pt_tan ( arc, t0, pt, tan ) ;
    C2V_SUB ( pt, ctr, vec ) ;
    c2v_normalize ( tan, tan ) ;
    C2V_SCALE ( tan, rad, tan ) ;
    angle = l / rad ;
    c = cos ( angle ) ;
    s = sin ( angle ) ;
    if ( dir == -1 )
        s = -s ;
    pt1[0] = ctr[0] + c * vec[0] + s * tan[0] ;
    pt1[1] = ctr[1] + c * vec[1] + s * tan[1] ;
    status = c2g_project ( arc, 0.0, 2.0, pt1, t_ptr, NULL ) ;
    if ( dir == 1 ) {
        if ( *t_ptr < t0 - BBS_ZERO ) {
            status = IS_ZERO ( *t_ptr ) ;
            *t_ptr = 2.0 ;
        }
    }
    else {
        if ( *t_ptr > t0 + BBS_ZERO ) {
            status = IS_ZERO ( *t_ptr - 2.0 ) ;
            *t_ptr = 0.0 ;
        }
    }
    RETURN ( status ) ;
#ifdef OLD_CODE
    C2_ASEG_S carc ;
    BOOLEAN status ;

    if ( C2_ARC_ZERO_RAD(arc) ) {
        *t_ptr = t0 ;
        RETURN ( TRUE ) ;
    }
    status = m2r_along ( C2_ARC_SEG(arc), t0, l, dir, t_ptr ) ;
    if ( status )
        RETURN ( TRUE ) ;
    if ( ( dir == 1 ) && ( *t_ptr > 1.0 - BBS_ZERO ) ) {
        if ( t0 <= 1.0 - BBS_ZERO ) {
            l -= c2g_length ( arc, t0, 1.0 ) ;
            status = m2r_along ( c2r_complement ( C2_ARC_SEG(arc), &carc ), 
                0.0, l, dir, t_ptr ) ;
        }
        else {
            status = m2r_along ( c2r_complement ( C2_ARC_SEG(arc), &carc ), 
                t0 - 1.0, l, dir, t_ptr ) ;
        }
        (*t_ptr) += 1.0 ;
    }
    else if ( ( dir == -1 ) && ( (*t_ptr) < BBS_ZERO ) ) {
        l = c2g_length ( arc, 0.0, 2.0 ) - l ;
        status = m2g_along ( arc, t0, l, 1, t_ptr ) ;
    }
    RETURN ( status ) ;
#endif
} 

