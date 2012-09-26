/* -2 -3 */
/********************************** C2R.C **********************************/
/***************** Routines for processing r-arc geometry *****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alrdefs.h>
#include <c2rdefs.h>
#include <c2edefs.h>
#include <c2vpriv.h>
#include <c2rmcrs.h>
#include <c2vmcrs.h>
#ifdef SPLINE
#include <c2bdefs.h>
#endif
#ifdef DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif
STATIC void c2r_eval_polynomial __(( C2_ASEG, REAL, INT, PT2* )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2r_eval ( arc, t, p, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
REAL t ;
INT p ;
PT2 *x ;
{
    if ( p==0 ) 
        c2r_pt_tan ( arc, t, x[0], NULL ) ;
    else if ( p==1 ) 
        c2r_pt_tan ( arc, t, x[0], x[1] ) ;
    else if ( p >= 2 ) 
        c2r_eval_polynomial ( arc, t, p, x ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2r_tan0 ( arc, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
PT2 tan_vec ;
{
    REAL u, v ;
    PT2 q, r ;

    C2R_QR ( arc, q, r ) ;
    C2R_UV ( arc, u, v ) ;
    C2V_ADDW ( q, u, r, -v, tan_vec ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2r_tan1 ( arc, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
PT2 tan_vec ;
{
    REAL u, v ;
    PT2 q, r ;

    C2R_QR ( arc, q, r ) ;
    C2R_UV ( arc, u, v ) ;
    C2V_ADDW ( q, u, r, v, tan_vec ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void c2r_eval_polynomial ( arc, t, p, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
REAL t ;
INT p ; /* p>= 2 */
PT2 *x ;
{
    HPT2 bez[3], b[3] ;
    HPT2 *f ;
    INT j ;

    f = CREATE ( p+1, HPT2 ) ;
    c2r_bez ( arc, bez ) ;
    for ( j=0 ; j<3 ; j++ ) {
        b[0][j] = bez[0][j] ;
        b[1][j] = 2.0 * ( bez[1][j] - bez[0][j] ) ;
        b[2][j] = bez[2][j] - 2.0 * bez[1][j] + bez[0][j] ;
    }
    c2e_polynomial2 ( b, t, p, f ) ;
    (void) c2e_ratio ( f, p, x ) ;
    KILL ( f ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2r_bez ( arc, bez ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
HPT2 *bez ;
{
    C2V_COPY ( C2_ASEG_PT0(arc), bez[0] ) ;
    bez[0][2] = 1.0 ;
    c2r_bez_mid_pt ( arc, bez[1] ) ;
    C2V_COPY ( C2_ASEG_PT1(arc), bez[2] ) ;
    bez[2][2] = 1.0 ;
#ifdef DEBUG
{
    REAL t0, t1, w, delta ;
    PT2 p0, p1 ;
    for ( t0 = 0.0 ; t0 <= 1.0 ; t0 += 0.1 ) {
        w = bez[0][2] * ( 1.0 - t0 ) * ( 1.0 - t0 ) + 
            2.0 * bez[1][2] * t0 * ( 1.0 - t0 ) + bez[2][2] * t0 * t0 ;
        p0[0] = ( bez[0][0] * ( 1.0 - t0 ) * ( 1.0 - t0 ) + 
            2.0 * bez[1][0] * t0 * ( 1.0 - t0 ) + bez[2][0] * t0 * t0 ) / w ;
        p0[1] = ( bez[0][1] * ( 1.0 - t0 ) * ( 1.0 - t0 ) + 
            2.0 * bez[1][1] * t0 * ( 1.0 - t0 ) + bez[2][1] * t0 * t0 ) / w ;
        c2r_project ( arc, p0, &t1, p1 ) ;
        delta = C2V_DISTL1 ( p0, p1 ) ;
    }
}
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2r_bez_mid_pt ( arc, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
HPT2 b ;
{
    PT2 q, r ;
    REAL u, v ;

    C2R_QR ( arc, q, r ) ;
    C2R_UV ( arc, u, v ) ;
    C2V_MID_PT ( C2_ASEG_PT1(arc), C2_ASEG_PT0(arc), q ) ;
    C2V_ADDW ( q, u, r, -0.5*v, b ) ;
    b[2] = u ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2r_length ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
{
    RETURN ( IS_ZERO(4.0*C2_ASEG_D(arc)) ? 
        C2V_DIST ( C2_ASEG_PT0(arc), C2_ASEG_PT1(arc) ) : 
        C2V_DIST ( C2_ASEG_PT0(arc), C2_ASEG_PT1(arc) ) * 
        ( 1.0 + C2_ASEG_D(arc) * C2_ASEG_D(arc) ) * 
        atan ( C2_ASEG_D(arc) ) / C2_ASEG_D(arc) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2r_select ( arc, pt, tol, t_ptr, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
PT2 pt ;
REAL tol ;
REAL *t_ptr ;
REAL *dist_ptr ;
{
    PT2 p ;

    if ( !c2r_project ( arc, pt, t_ptr, p ) )
        RETURN ( FALSE ) ;
    if ( *t_ptr < - BBS_ZERO || *t_ptr > 1.0 + BBS_ZERO )
        RETURN ( FALSE ) ;
    if ( C2V_DISTL1 ( p, pt ) > tol ) 
        RETURN ( FALSE ) ;
    if ( dist_ptr != NULL ) 
        *dist_ptr = C2V_DIST ( p, pt ) ;
    RETURN ( TRUE ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2r_project ( arc, pt, t_ptr, proj_pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
PT2 pt ;
REAL *t_ptr ;
PT2 proj_pt ;
{
    REAL m, n, r, a, b, u, v ;
    PT2 ctr ;
    c2v_coord ( C2_ASEG_PT0(arc), C2_ASEG_PT1(arc), pt, &u, &v ) ;
    C2R_UV ( arc, m, n ) ;
    a = m * u - n * v ;
    b = u - 0.5 ;
    r = a*a - 2.0*m*a*b + b*b ;
    if ( r < - BBS_ZERO ) 
        RETURN ( FALSE ) ;
    else if ( r > BBS_ZERO ) 
        r = sqrt ( r ) ;
    else 
        r = 0.0 ;

    if ( IS_ZERO(a-b+r) ) {
        c2r_ctr ( arc, ctr ) ;
        if ( IS_ZERO(a) && !C2V_IDENT_PTS(pt,ctr) )
            *t_ptr = 1.0 / ( 1.0 - m ) ;
        else {
            *t_ptr = 0.5 ;
            RETURN ( FALSE ) ;
        }    
    }
    else 
        *t_ptr = a / ( a - b + r ) ;
    if ( *t_ptr < - BBS_ZERO || *t_ptr > 1.0 + BBS_ZERO ) 
        RETURN ( FALSE ) ;
    if ( proj_pt != NULL ) 
        c2r_pt_tan ( arc, *t_ptr, proj_pt, NULL ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2r_curvature ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
{
    REAL d = C2V_DIST ( C2_ASEG_PT0(arc), C2_ASEG_PT1(arc) ) ;

    RETURN ( IS_SMALL(d) ? 0.0 : 4.0 * C2_ASEG_D(arc) / 
        ( d * ( 1.0 + C2_ASEG_D(arc) * C2_ASEG_D(arc) ) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2r_sbdv ( arc, arc0, arc1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc, arc0, arc1 ; 
{
    C2V_COPY ( C2_ASEG_PT0(arc), C2_ASEG_PT0(arc0) ) ;
    C2V_COPY ( C2_ASEG_PT1(arc), C2_ASEG_PT1(arc1) ) ;
    c2r_pt_tan ( arc, 0.5, C2_ASEG_PT1(arc0), NULL ) ;
    C2V_COPY ( C2_ASEG_PT1(arc0), C2_ASEG_PT0(arc1) ) ;
    C2_ASEG_D(arc0) = C2_ASEG_D(arc1) = alr_sbdv_d ( C2_ASEG_D(arc) ) ;
#ifdef DEBUG
{
    REAL t0, t1, delta ;
    PT2 p0, p1 ;
    for ( t0 = 0.0 ; t0 <= 1.0 ; t0+= 0.1 ) {
        c2r_pt_tan ( arc0, t0, p0, NULL ) ;
        c2r_project ( arc, p0, &t1, p1 ) ;
        delta = C2V_DISTL1 ( p0, p1 ) ;
    }
    for ( t0 = 0.0 ; t0 <= 1.0 ; t0+= 0.1 ) {
        c2r_pt_tan ( arc1, t0, p0, NULL ) ;
        c2r_project ( arc, p0, &t1, p1 ) ;
        delta = C2V_DISTL1 ( p0, p1 ) ;
    }
}
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2r_trim ( arc, t0, t1, arc1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc, arc1 ; 
REAL t0, t1 ;
{
    PT2 c0, c1 ;
    c2r_pt_tan ( arc, t0, c0, NULL ) ;
    c2r_pt_tan ( arc, t1, c1, NULL ) ;
    C2V_COPY ( c0, C2_ASEG_PT0(arc1) ) ;
    C2V_COPY ( c1, C2_ASEG_PT1(arc1) ) ;
    C2_ASEG_D(arc1) = alr_trim_d ( C2_ASEG_D(arc), t0, t1 ) ;
    RETURN ( arc1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2r_get_pts_d ( arc, t0, t1, pt0, pt1, d_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
REAL t0, t1 ;
PT2 pt0, pt1 ;
REAL* d_ptr ;
{
    c2r_pt_tan ( arc, t0, pt0, NULL ) ;
    c2r_pt_tan ( arc, t1, pt1, NULL ) ;
    *d_ptr = alr_trim_d ( C2_ASEG_D(arc), t0, t1 ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2r_untrim ( arc, t0, t1, arc1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc, arc1 ; 
REAL t0, t1 ;
{
    REAL dt, u0, u1 ;
    dt = t1 - t0 ;
    if ( IS_ZERO(dt) ) 
        RETURN ( NULL ) ;
    u0 = - t0 / dt ;
    u1 = ( 1.0 - t0 ) / dt ;
    RETURN ( c2r_trim ( arc, u0, u1, arc1 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2r_reverse ( arc0, arc1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc0, arc1 ;
{
    PT2 a ;
    C2V_COPY ( C2_ASEG_PT0(arc0), a ) ;
    C2V_COPY ( C2_ASEG_PT1(arc0), C2_ASEG_PT0(arc1) ) ;
    C2V_COPY ( a, C2_ASEG_PT1(arc1) ) ;
    C2_ASEG_D(arc1) = - C2_ASEG_D(arc0) ;
    RETURN ( arc1 ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2r_complement ( arc0, arc1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc0, arc1 ;
{
    PT2 a ;
    C2V_COPY ( C2_ASEG_PT0(arc0), a ) ;
    C2V_COPY ( C2_ASEG_PT1(arc0), C2_ASEG_PT0(arc1) ) ;
    C2V_COPY ( a, C2_ASEG_PT1(arc1) ) ;
    if ( IS_ZERO(4.0*C2_ASEG_D(arc0)) )
        RETURN ( NULL ) ;
    C2_ASEG_D(arc1) = 1.0 / C2_ASEG_D(arc0) ;
    RETURN ( arc1 ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2r_2pts_tan_ctr ( pt0, tan0, pt1, ctr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1, tan0, ctr ;
{
    REAL a, b, c, u, v, r ;
    PT2 q ;

    C2V_SUB ( pt1, pt0, q ) ;
    if ( C2V_IS_SMALL(q) || C2V_IS_SMALL(tan0) ) 
        RETURN ( FALSE ) ;
    a = C2V_DOT ( q, q ) ;
    b = C2V_DOT ( q, tan0 ) ;
    c = C2V_DOT ( tan0, tan0 ) ;
    r = 2.0 * ( a * c - b * b ) ;
    if ( IS_ZERO(r) )
        RETURN ( FALSE ) ;
    u = a * c / r ;
    v = - a * b / r ;
    ctr[0] = pt0[0] + u * q[0] + v * tan0[0] ;
    ctr[1] = pt0[1] + u * q[1] + v * tan0[1] ;
    RETURN ( TRUE ) ;
}    

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2r_negate_d ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
{
    C2_ASEG_D(arc) = - C2_ASEG_D(arc) ;
}


#ifdef SPLINE
/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2r_bezs ( arc, ext_arc, bez ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
BOOLEAN ext_arc ;
HPT2 *bez ;
{
    C2_ASEG_S arcc ;
    HPT2 bez0[3], *bez1 ;

    if ( ext_arc ) {
        c2r_bez ( arc, bez0 ) ;
        C2B_SBDV0 ( bez0, 3, bez ) ;
        c2r_bez ( c2r_complement ( arc, &arcc ), bez0 ) ;
        bez1 = bez + 4 ;
        C2B_SBDV0 ( bez0, 3, bez1 ) ;
        RETURN ( 9 ) ;
    }

    else if ( fabs ( C2_ASEG_D(arc) ) <= 1.0 - BBS_ZERO ) {
        c2r_bez ( arc, bez ) ;
        RETURN ( 3 ) ;
    }
    else {
        c2r_bez ( arc, bez0 ) ;
        C2B_SBDV0 ( bez0, 3, bez ) ;
        RETURN ( 5 ) ;
    }
}
#endif /*SPLINE*/

