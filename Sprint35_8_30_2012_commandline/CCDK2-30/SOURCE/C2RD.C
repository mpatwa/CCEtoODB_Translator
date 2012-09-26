/* -2 -3 */
/********************************* C2RD.C **********************************/
/****************** Routines for processing arc geometry *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alrdefs.h>
#include <c2rdefs.h>
#include <c2vdefs.h>
#include <c2vpriv.h>
#include <c2mem.h>
#include <c2rmcrs.h>
#include <c2vmcrs.h>
STATIC REAL c2r_2pts_2tans_d __(( REAL, REAL )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2r_offset ( arc0, offset, arc1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc0, arc1 ;
REAL offset ;
{
    PT2 q, r ;
    REAL u, v ;

    C2R_QR ( arc0, q, r ) ;
    C2R_UV ( arc0, u, v ) ;
    offset /= C2V_NORM ( q ) ;
    if ( C2V_IS_SMALL(q) ) 
        RETURN ( FALSE ) ;
    u *= offset ;
    v *= offset ;
    C2V_ADDC ( C2_ASEG_PT0(arc0), q, -v, r, -u, C2_ASEG_PT0(arc1) ) ;
    C2V_ADDC ( C2_ASEG_PT1(arc0), q, v, r, -u, C2_ASEG_PT1(arc1) ) ;
    C2_ASEG_D(arc1) = C2_ASEG_D(arc0) ;
    RETURN ( v > -0.5 + BBS_ZERO ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2r_offset_zero_arc ( arc0, w, arc1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc0, arc1 ;
REAL w ;
{
    c2r_inflate ( arc0, C2_ASEG_D(arc0)>0.0 ? w : -w, arc1 ) ;
    RETURN ( ( C2_ASEG_D(arc0)>0.0 ) ? ( w > 0.0 ) : ( -w > 0.0 ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2r_arc ( ctr, rad, st_angle, sweep, dir, arc ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr ;
REAL rad, st_angle, sweep ;
INT dir ;
C2_ASEG arc ;
{
    C2_ASEG_PT0(arc)[0] = ctr[0] + cos ( st_angle ) * rad ;
    C2_ASEG_PT0(arc)[1] = ctr[1] + sin ( st_angle ) * rad ;
    if ( dir == 1 ) {
        C2_ASEG_PT1(arc)[0] = ctr[0] + cos ( st_angle + sweep ) * rad ;
        C2_ASEG_PT1(arc)[1] = ctr[1] + sin ( st_angle + sweep ) * rad ;
        C2_ASEG_D(arc) = tan ( 0.25 * sweep ) ;
    }
    else {
        C2_ASEG_PT1(arc)[0] = ctr[0] + cos ( st_angle - sweep ) * rad ;
        C2_ASEG_PT1(arc)[1] = ctr[1] + sin ( st_angle - sweep ) * rad ;
        C2_ASEG_D(arc) = - tan ( 0.25 * sweep ) ;
    }
    RETURN ( arc ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2r_3pts ( ept0, mid_pt, ept1, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ept0, ept1, mid_pt ;
C2_ASEG arc ;
{
    REAL u, v, v2, w ;

    C2V_COPY ( ept0, C2_ASEG_PT0(arc) ) ;
    C2V_COPY ( ept1, C2_ASEG_PT1(arc) ) ;
    c2v_coord ( ept0, ept1, mid_pt, &u, &v ) ;
    if ( IS_ZERO(v) )
        C2_ASEG_D(arc) = 0.0 ;
    else {
        v2 = v * v ;
        w = u * ( 1.0 - u ) - v2 ;
        C2_ASEG_D(arc) = - v / ( sqrt ( v2 + w*w ) + w ) ;
    }
    RETURN ( arc ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2r_2pts_tan ( pt0, tan0, pt1, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1, tan0 ;
C2_ASEG arc ;
{
    REAL c, s ;
    PT2 q ;

    C2V_COPY ( pt0, C2_ASEG_PT0(arc) ) ;
    C2V_COPY ( pt1, C2_ASEG_PT1(arc) ) ;

    C2V_SUB ( pt1, pt0, q ) ;
    if ( C2V_IS_SMALL(q) || C2V_IS_SMALL(tan0) ) {
        C2_ASEG_D(arc) = 0.0 ;
        RETURN ( NULL ) ;
    }
    c = C2V_DOT ( q, tan0 ) ;
    s = C2V_CROSS ( q, tan0 ) ;
    C2_ASEG_D(arc) = - s / ( C2V_NORM(q) * C2V_NORM(tan0) + c ) ;
    RETURN ( arc ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2r_2pts_2tans ( pt0, tan0, pt1, tan1, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1, tan0, tan1 ;
C2_ASEG arc ;
{
    REAL c, s, d0, d1 ;
    PT2 q, t0, t1 ;

    C2V_COPY ( pt0, C2_ASEG_PT0(arc) ) ;
    C2V_COPY ( pt1, C2_ASEG_PT1(arc) ) ;
    C2V_SUB ( pt1, pt0, q ) ;
    if ( C2V_IS_SMALL(q) || !c2v_normalize_l1 ( tan0, t0 ) ||
        !c2v_normalize_l1 ( tan1, t1 ) ) {
        C2_ASEG_D(arc) = 0.0 ;
        RETURN ( NULL ) ;
    }
    c = C2V_DOT ( t0, q ) ;
    s = C2V_CROSS ( t0, q ) ;
    d0 = c2r_2pts_2tans_d ( c, s ) ;
    if ( IS_ZERO(d0) ) {
        C2_ASEG_D(arc) = 0.0 ;
        RETURN ( NULL ) ;
    }

    c = C2V_DOT ( q, t1 ) ;
    s = C2V_CROSS ( q, t1 ) ;
    d1 = c2r_2pts_2tans_d ( c, s ) ;
    if ( IS_ZERO(d1) ) {
        C2_ASEG_D(arc) = 0.0 ;
        RETURN ( NULL ) ;
    }
    C2_ASEG_D(arc) = 0.5 * ( d0 + d1 ) ;
    RETURN ( arc ) ;
}    


/*-------------------------------------------------------------------------*/
STATIC REAL c2r_2pts_2tans_d ( c, s )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL c, s ;
{
    REAL r ;

    if ( IS_SMALL(s) ) 
        RETURN ( 0.0 ) ;
    r = sqrt ( c * c + s * s ) ;
    if ( IS_SMALL(r) ) 
        RETURN ( 1.0 ) ;
    else if ( c > 0.0 ) 
        RETURN ( ( -c + r ) / s ) ;
    else if ( c < 0.0 ) 
        RETURN ( ( -c - r ) / s ) ;
    else 
        RETURN ( 0.0 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2r_2pts_ctr ( pt0, pt1, ctr, dir, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1, ctr ;
INT dir ;
C2_ASEG arc ;
{
    REAL a, b, u ;
    PT2 pt, v0, v1 ;

    C2V_SUB ( pt0, ctr, v0 ) ;
    C2V_SUB ( pt1, ctr, v1 ) ;
    a = C2V_DOT ( v0, v0 ) ;
    b = C2V_DOT ( v1, v1 ) ;
    if ( !IS_SMALL(a-b) ) 
    {
        if ( dir == 1 )
            c2v_rotate_vec ( v0, PI / 2.0, v1 );
        else
            c2v_rotate_vec ( v0, - PI / 2.0, v1 );

        c2v_normalize_l1 ( v1, v1 );

        RETURN ( c2r_2pts_tan ( pt0, v1, pt1, arc ) );
/*        RETURN ( NULL ) ; */
    }
    b = C2V_DOT ( v0, v1 ) ;
    if ( fabs ( a + b ) <= BBS_ZERO * a ) {
        if ( dir == 1 ) {
            pt[0] = ctr[0] - v0[1] ;
            pt[1] = ctr[1] + v0[0] ;
        }
        else {
            pt[0] = ctr[0] + v0[1] ;
            pt[1] = ctr[1] - v0[0] ;
        }
    }
    else {
        u = sqrt ( 0.5 * a / ( a + b ) ) ;
        b = C2V_CROSS ( v0, v1 ) ;
        if ( ( dir == 1 ) != ( b > 0.0 ) ) 
            u = -u ;
        pt[0] = ctr[0] + u * ( v0[0] + v1[0] ) ;
        pt[1] = ctr[1] + u * ( v0[1] + v1[1] ) ;
    }
    RETURN ( c2r_3pts ( pt0, pt, pt1, arc ) ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2r_ctr_2tans ( ctr, rad, tan0, tan1, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr, tan0, tan1 ;
REAL rad ;
C2_ASEG arc ;
{
    REAL c, s ;
    PT2 t0, t1 ;

    c2v_normalize ( tan0, t0 ) ;
    c2v_normalize ( tan1, t1 ) ;
    C2_ASEG_PT0(arc)[0] = ctr[0] + rad * t0[1] ;
    C2_ASEG_PT0(arc)[1] = ctr[1] - rad * t0[0] ;
    C2_ASEG_PT1(arc)[0] = ctr[0] + rad * t1[1] ;
    C2_ASEG_PT1(arc)[1] = ctr[1] - rad * t1[0] ;
/*
    c = 1.0 - C2V_DOT ( t0, t1 ) ;
    s = C2V_CROSS ( t0, t1 ) ;
    if ( s > 0.0 ) 
        C2_ASEG_D(arc) = ( -s + sqrt ( 2.0 * c ) ) / c ;
    else
        C2_ASEG_D(arc) = ( -s - sqrt ( 2.0 * c ) ) / c ;
*/
    c = C2V_DOT ( t0, t1 ) ;
    s = C2V_CROSS ( t0, t1 ) ;
    if ( c >= 0.0 ) {
        c += 1.0 ;
        C2_ASEG_D(arc) = s / ( c + sqrt ( 2.0 * c ) ) ;
    }
    else {
        c = 1.0 - c ;
        C2_ASEG_D(arc) = ( sqrt ( 2.0 * c ) - s ) / c ;
    }
    RETURN ( arc ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2r_zero_rad ( p0, p1, tan0, tan1, w, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 p0, p1, tan0, tan1 ;
REAL w ;
C2_ASEG arc ;
{
    REAL c, s ;
    PT2 t0, t1 ;

    c2v_normalize ( tan0, t0 ) ;
    c2v_normalize ( tan1, t1 ) ;

    C2_ASEG_PT0(arc)[0] = p0[0] + w * t0[1] ;
    C2_ASEG_PT0(arc)[1] = p0[1] - w * t0[0] ;
    C2_ASEG_PT1(arc)[0] = p1[0] + w * t1[1] ;
    C2_ASEG_PT1(arc)[1] = p1[1] - w * t1[0] ;
/*
    c = 1.0 - C2V_DOT ( t0, t1 ) ;
    s = C2V_CROSS ( t0, t1 ) ;
    C2_ASEG_D(arc) = ( -s + sqrt ( 2.0 * c ) ) / c ;
*/
    c = 1.0 + C2V_DOT ( t0, t1 ) ;
    s = C2V_CROSS ( t0, t1 ) ;
    C2_ASEG_D(arc) = s / ( c + sqrt ( 2.0 * c ) ) ;
    RETURN ( arc ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2r_circle ( ctr, rad, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr ;
REAL rad ;
C2_ASEG arc ;
{
    C2V_SET ( ctr[0]+rad, ctr[1], C2_ASEG_PT0(arc) ) ;
    C2V_SET ( ctr[0]-rad, ctr[1], C2_ASEG_PT1(arc) ) ;
    C2_ASEG_D(arc) = 1.0 ; 
    RETURN ( arc ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2r_line ( pt0, pt1, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1 ;
C2_ASEG arc ;
{
    C2V_COPY ( pt0, C2_ASEG_PT0(arc) ) ;
    C2V_COPY ( pt1, C2_ASEG_PT1(arc) ) ;
    C2_ASEG_D(arc) = 0.0 ;
    RETURN ( arc ) ;
}    

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2r_copy ( arc0, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc0, arc ;
{
    if ( arc0 == NULL || arc == NULL ) 
        RETURN ( NULL ) ;
    C2V_COPY ( C2_ASEG_PT0(arc0), C2_ASEG_PT0(arc) ) ;
    C2V_COPY ( C2_ASEG_PT1(arc0), C2_ASEG_PT1(arc) ) ;
    C2_ASEG_D(arc) = C2_ASEG_D(arc0) ;
    RETURN ( arc ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2r_inflate ( arc0, rad, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc0, arc ;
REAL rad ;
{
    REAL w ;
    PT2 ctr ;

    C2_ASEG_D(arc) = C2_ASEG_D(arc0) ;
    if ( c2r_ctr ( arc, ctr ) == NULL ) {
        C2V_COPY ( C2_ASEG_PT0(arc0), C2_ASEG_PT0(arc) ) ;
        C2V_COPY ( C2_ASEG_PT1(arc0), C2_ASEG_PT1(arc) ) ;
    }
    else {
        w = rad / c2r_rad ( arc ) ;
        C2V_ADDU ( ctr, C2_ASEG_PT0(arc0), w, C2_ASEG_PT0(arc) ) ;
        C2V_ADDU ( ctr, C2_ASEG_PT1(arc0), w, C2_ASEG_PT1(arc) ) ;
    }
    RETURN ( arc ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2r_arc_pts_bulge ( pt0, pt1, bulge, arc )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1 ;
REAL bulge ;
C2_ASEG arc ;
{
    C2V_COPY ( pt0, C2_ASEG_PT0(arc) ) ;
    C2V_COPY ( pt1, C2_ASEG_PT1(arc) ) ;
    C2_ASEG_D(arc) = bulge ;
    RETURN ( arc ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2r_circle_ctr_pt ( ctr, pt, arc ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr, pt ;
C2_ASEG arc ;
{
    C2V_COPY ( pt, C2_ASEG_PT0(arc) ) ;
    C2V_ADDW ( ctr, 2.0, pt, -1.0, C2_ASEG_PT1(arc) ) ;
    C2_ASEG_D(arc) = 1.0 ;
    RETURN ( arc ) ;
}

