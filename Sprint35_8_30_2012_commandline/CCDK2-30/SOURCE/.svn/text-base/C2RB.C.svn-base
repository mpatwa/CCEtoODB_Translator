/* -2 -3 */
/********************************** C2RB.C *********************************/
/****************** Routines for processing r-arc geometry *****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alpdefs.h>
#include <alrdefs.h>
#include <c2rdefs.h>
#include <c2adefs.h>
#include <c2vdefs.h>
#include <c2rmcrs.h>
#include <c2vmcrs.h>
STATIC void c2r_box0 __(( C2_ASEG, REAL, REAL, C2_BOX )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2r_pt_tan ( arc, t, pt, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
REAL t ;
PT2 pt, tan_vec ;
{
    REAL t0, t1, t2, u, v, z ;
    PT2 q, r ;

    C2R_QR ( arc, q, r ) ;
    C2R_UV ( arc, u, v ) ;
    t0 = ( 1.0 - t ) * ( 1.0 - t ) ;
    t1 = 2.0 * t * ( 1.0 - t ) ;
    t2 = t * t ;

    z = t0 + u * t1 + t2 ;

    if ( pt != NULL ) {
        pt[0] = ( C2_ASEG_PT0(arc)[0] * t0 + C2_ASEG_PT1(arc)[0] * t2 + 
            0.5 * ( u * ( C2_ASEG_PT1(arc)[0] + C2_ASEG_PT0(arc)[0] ) - 
            v * r[0] ) * t1 ) / z ;
        pt[1] = ( C2_ASEG_PT0(arc)[1] * t0 + C2_ASEG_PT1(arc)[1] * t2 + 
            0.5 * ( u * ( C2_ASEG_PT1(arc)[1] + C2_ASEG_PT0(arc)[1] ) - 
            v * r[1] ) * t1 ) / z ;
    }

    if ( tan_vec != NULL ) {
        z *= z ;
        tan_vec[0] = ( q[0] * ( u * ( t0 + t2 ) + t1 ) + 
            r[0] * v * ( t2 - t0 ) ) / z ;
        tan_vec[1] = ( q[1] * ( u * ( t0 + t2 ) + t1 ) + 
            r[1] * v * ( t2 - t0 ) ) / z ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE void c2r_box ( arc, t0, t1, box ) 
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                          !!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C2_ASEG arc ; 
REAL t0, t1 ;
C2_BOX box ;
{
    PT2 p ;
    c2r_pt_tan ( arc, t0, p, NULL ) ;
    c2a_box_init_pt ( box, p ) ;
    c2r_box0 ( arc, t0, t1, box ) ;
}


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE void c2r_box_append ( arc, t0, t1, box ) 
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                          !!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C2_ASEG arc ; 
REAL t0, t1 ;
C2_BOX box ;
{
/*
    PT2 p ;
    c2r_pt_tan ( arc, t0, p, NULL ) ;
    c2a_box_append_pt ( box, p ) ;
*/
    c2r_box0 ( arc, t0, t1, box ) ;
}


/*-------------------------------------------------------------------------*/ 
STATIC void c2r_box0 ( arc, t0, t1, box ) 
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                          !!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C2_ASEG arc ; 
REAL t0, t1 ;
C2_BOX box ;
{
    PT2 p ;
    INT i, j, n ;
    REAL t[2] ;

    c2r_pt_tan ( arc, t1, p, NULL ) ;
    c2a_box_append_pt ( box, p ) ;

    for ( j=0 ; j<2 ; j++ ) {
        n = c2r_coord_extrs ( arc, j, t ) ;
        for ( i=0 ; i<n ; i++ ) {
            if ( t[i] >= t0 - BBS_ZERO && t[i] <= t1 + BBS_ZERO ) {
                c2r_pt_tan ( arc, t[i], p, NULL ) ;
                c2a_box_append_pt ( box, p ) ;
            }
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2r_coord_extrs ( arc, coord, x_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
INT     coord ;
REAL    *x_t ;
{
    PT2 q, r ;
    REAL z[3], t[2], u, v ;
    INT i, m, n, mult[2] ;

    C2R_QR ( arc, q, r ) ;
    C2R_UV ( arc, u, v ) ;

    z[0] = q[coord] * u - r[coord] * v ;
    z[1] = q[coord]  ;
    z[2] = q[coord] * u + r[coord] * v ;
    m = alp_solve_bez3 ( z[0], z[1], z[2], t, mult ) ;
    n = 0 ;
    for ( i=0 ; i<m ; i++ ) {
        if ( - BBS_ZERO <= t[i] && t[i] <= 1.0 + BBS_ZERO ) {
            x_t[n] = t[i] ;
            n++ ;
        }
    }
    RETURN ( n ) ;
}


/*--------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2r_get_data ( arc, ctr, ept1, rad_ptr, 
    st_angle_ptr, sweep_ptr, dir_ptr ) 
/*--------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
PT2 ctr, ept1 ;
REAL *rad_ptr, *st_angle_ptr, *sweep_ptr ;
INT *dir_ptr ;
{
    PT2 v ;

    if ( ept1 != NULL ) 
        C2V_COPY ( C2_ASEG_PT1(arc), ept1 ) ;
    if ( IS_ZERO ( 4.0*C2_ASEG_D(arc) ) )
        RETURN ( FALSE ) ;
    if ( ctr != NULL ) {
        if ( c2r_ctr ( arc, ctr ) == NULL ) 
            RETURN ( FALSE ) ;
    }
    if ( rad_ptr != NULL ) {
        *rad_ptr = c2r_rad ( arc ) ;
        if ( *rad_ptr < 0.0 ) 
            RETURN ( FALSE ) ;
    }
    if ( st_angle_ptr != NULL ) {
         if ( ctr == NULL ) {
            (void)c2r_ctr ( arc, v ) ;
            C2V_SUB ( C2_ASEG_PT0(arc), v, v ) ;
        }
        else
            C2V_SUB ( C2_ASEG_PT0(arc), ctr, v ) ;
        *st_angle_ptr = c2v_atan2 ( v ) ;
    }
    if ( sweep_ptr != NULL ) 
        *sweep_ptr = alr_sweep ( C2_ASEG_D(arc) ) ;
    if ( dir_ptr != NULL ) 
        *dir_ptr = c2r_dir ( arc ) ;
    RETURN ( TRUE ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL *c2r_ctr ( arc, ctr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
PT2 ctr ;
{
    PT2 q, r ;
    REAL u ;

    if ( IS_ZERO ( 4.0*C2_ASEG_D(arc) ) )
        RETURN ( NULL ) ;

    C2R_QR ( arc, q, r ) ;
    C2V_MID_PT ( C2_ASEG_PT1(arc), C2_ASEG_PT0(arc), q ) ;
    u = 0.25 * ( 1.0 - C2_ASEG_D(arc)*C2_ASEG_D(arc) ) / C2_ASEG_D(arc) ;
    C2V_ADDT ( q, r, u, ctr ) ;
    RETURN ( ctr ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2r_rad ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
{
    if ( IS_ZERO ( 4.0*C2_ASEG_D(arc) ) )
        RETURN ( -1.0 ) ;

    RETURN ( C2V_DIST ( C2_ASEG_PT0(arc), C2_ASEG_PT1(arc) ) * 0.25 * 
        ( 1.0 + C2_ASEG_D(arc) * C2_ASEG_D(arc) ) / fabs(C2_ASEG_D(arc)) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2r_sweep ( arc, t0, t1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
REAL t0, t1 ;
{
    REAL sweep ;

    if ( IS_ZERO(t0) && IS_ZERO(t1-1.0) )
        sweep = alr_sweep ( C2_ASEG_D(arc) ) ;
    else if ( IS_ZERO(t0) && IS_ZERO(t1-2.0) )
        sweep = TWO_PI ;
    else if ( t1 <= 1.0 )
        sweep = alr_sweep ( alr_trim_d ( C2_ASEG_D(arc), t0, t1 ) ) ;
    else if ( t0 >= 1.0 )
        sweep = alr_sweep ( alr_trim_d ( C2_ASEG_D(arc), t0-1.0, t1-1.0 ) ) ;
    else 
        sweep = alr_sweep ( alr_trim_d ( C2_ASEG_D(arc), t0, 1.0 ) ) + 
            alr_sweep ( alr_trim_d ( C2_ASEG_D(arc), 0.0, t1-1.0 ) ) ;
    RETURN ( fabs(sweep) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2r_t_to_angle ( arc, t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
REAL t ;
{
    RETURN ( t <= 1.0 ? alr_angle ( t, C2_ASEG_D(arc) ) :
        alr_angle ( t-1.0, 1.0 / C2_ASEG_D(arc) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2r_angle_to_t ( arc, angle ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
REAL angle ;
{
    REAL sweep ;
    sweep = alr_sweep ( C2_ASEG_D(arc) ) ;
    RETURN ( angle <= sweep ? 
        alr_parm ( angle, C2_ASEG_D(arc) ) : 
        alr_parm ( angle - sweep, 1.0 / C2_ASEG_D(arc) ) + 1.0 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2r_dir ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
{
    RETURN ( C2_ASEG_D(arc) > 0.0 ? 1 : -1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2r_flatness ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
{
    RETURN ( 0.5 * C2V_DIST ( C2_ASEG_PT0(arc), C2_ASEG_PT1(arc) ) * 
        fabs(C2_ASEG_D(arc)) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL* c2r_d_vec ( arc, d_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
PT2 d_vec ;
{
    d_vec[0] = C2_ASEG_PT0(arc)[1] - C2_ASEG_PT1(arc)[1] ;
    d_vec[1] = C2_ASEG_PT1(arc)[0] - C2_ASEG_PT0(arc)[0] ;
    c2v_normalize ( d_vec, d_vec ) ;
    C2V_SCALE ( d_vec, -C2_ASEG_D(arc), d_vec ) ;
    RETURN ( d_vec ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2r_get_epts_d ( arc, ept0, ept1, d_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
PT2 ept0, ept1 ;
REAL *d_ptr ;
{
  if ( ept0 != NULL )
    {
    C2V_COPY ( C2_ASEG_PT0(arc), ept0 ) ;
  }
  if ( ept1 != NULL )
    {
    C2V_COPY ( C2_ASEG_PT1(arc), ept1 ) ;
  }
  if ( d_ptr != NULL )
    *d_ptr = C2_ASEG_D(arc) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2r_get_d ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
{
    RETURN ( C2_ASEG_D(arc) ) ;
}

