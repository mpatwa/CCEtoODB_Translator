/* -2 -3*/
/******************************* C2CA.C *********************************/ 
/***************** Two-dimensional arcs *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alrdefs.h>
#include <c2gdefs.h>
#include <c2ldefs.h>
#include <c2ndefs.h>
#include <c2pdefs.h>
#include <c2vdefs.h>
#include <c2cdefs.h>

/*--------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_get_arc_data ( curve, ctr, rad_ptr, 
            st_angle_ptr, sweep_ptr, dir_ptr )      
/*--------------------------------------------------------------------*/
C2_CURVE curve ;
PT2 ctr ;
REAL *rad_ptr, *st_angle_ptr, *sweep_ptr ;
INT *dir_ptr ;
{
    if ( !C2_CURVE_IS_ARC(curve) ) 
        RETURN ( FALSE ) ;
    if ( ctr != NULL ) {
        if ( !c2c_get_arc_center ( curve, ctr ) )
            RETURN ( FALSE ) ;
    }
    if ( rad_ptr != NULL ) {
        if ( !c2c_get_arc_radius ( curve, rad_ptr ) )
            RETURN ( FALSE ) ;
    }
    if ( dir_ptr != NULL ) { 
        if ( !c2c_get_arc_direction ( curve, dir_ptr ) )
            RETURN ( FALSE ) ;
    }
    if ( st_angle_ptr != NULL ) {
        if ( !c2c_get_arc_start_angle ( curve, st_angle_ptr ) )
            RETURN ( FALSE ) ;
    }
    if ( sweep_ptr != NULL ) {
        if ( !c2c_get_arc_sweep ( curve, sweep_ptr ) )
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_get_arc_radius ( curve, rad_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL *rad_ptr ;
{
    if ( !C2_CURVE_IS_ARC(curve) ) 
        RETURN ( FALSE ) ;
    else {
        *rad_ptr = c2g_rad ( C2_CURVE_ARC(curve) ) ;
        RETURN ( TRUE ) ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_get_arc_center ( curve, ctr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 ctr ;
{
    if ( !C2_CURVE_IS_ARC(curve) ) 
        RETURN ( FALSE ) ;
    RETURN ( c2g_ctr ( C2_CURVE_ARC(curve), ctr ) != NULL ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_get_arc_direction ( curve, dir_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
INT *dir_ptr ;
{
    if ( !C2_CURVE_IS_ARC(curve) ) 
        RETURN ( FALSE ) ;
    else {
        *dir_ptr = c2g_dir ( C2_CURVE_ARC(curve) ) ;
        RETURN ( TRUE ) ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_get_arc_start_angle ( curve, angle_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL *angle_ptr ;
{
    PT2 p0, ctr ;

    if ( !c2c_get_arc_center ( curve, ctr ) ) 
        RETURN ( FALSE ) ;
    if ( C2_CURVE_T0(curve) <= 1.0 ) 
        c2g_pt_tan ( C2_CURVE_ARC(curve), C2_CURVE_T0(curve), p0, NULL ) ;
    else {
        C2_ARC_S arc1 ;
        if ( C2_CURVE_T0(curve) >= 2.0 ) 
            C2_CURVE_T0(curve) = 2.0 ;
        c2g_pt_tan ( c2g_complement ( C2_CURVE_ARC(curve), &arc1 ), 
            C2_CURVE_T0(curve) - 1.0, p0, NULL ) ;
    }

    c2v_sub ( p0, ctr, p0 );
    c2v_normalize_l1 ( p0, p0 );

    *angle_ptr = bbs_atan2 ( p0[1], p0[0] );
// p0[1]-ctr[1], p0[0]-ctr[0]

    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_get_arc_sweep ( curve, sweep_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL *sweep_ptr ;
{
    if ( !C2_CURVE_IS_ARC(curve) ) 
        RETURN ( FALSE ) ;
    else {
        *sweep_ptr = c2g_sweep ( C2_CURVE_ARC(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve) ) ;
        RETURN ( TRUE ) ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2c_get_arc_pts_d ( curve, pt0, pt1, d_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 pt0, pt1 ;
REAL* d_ptr ;
{
    if ( C2_CURVE_IS_LINE(curve) ) 
        RETURN ( c2l_get_pts_d ( C2_CURVE_LINE(curve), 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve), pt0, pt1, d_ptr ) ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        RETURN ( c2g_get_pts_d ( C2_CURVE_ARC(curve), 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve), pt0, pt1, d_ptr ) ) ;
    else 
        RETURN ( FALSE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_arc_angle ( curve, parm, angle_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm ;
REAL *angle_ptr ;
{
    if ( !C2_CURVE_IS_ARC(curve) ) 
        RETURN ( FALSE ) ;
    else {
        *angle_ptr = c2g_t_to_angle ( C2_CURVE_ARC(curve), PARM_T(parm) ) ;
        RETURN ( TRUE ) ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_arc_parm ( curve, angle, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL angle ;
PARM parm ;
{
    if ( !C2_CURVE_IS_ARC(curve) ) 
        RETURN ( FALSE ) ;
    else {
        PARM_T(parm) = c2g_angle_to_t ( C2_CURVE_ARC(curve), angle ) ;
        alr_parm_set ( parm ) ;
        RETURN ( TRUE ) ;
    }
}


/*--------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_get_pcurve_data ( curve, i, is_arc, ctr, ept1, 
    rad_ptr, st_angle_ptr, sweep_ptr, dir_ptr ) 
/*--------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
INT i ;
BOOLEAN *is_arc ;
PT2 ctr, ept1 ;
REAL *rad_ptr, *st_angle_ptr, *sweep_ptr ;
INT *dir_ptr ;
{
    INT n ;

    if ( !C2_CURVE_IS_PCURVE(curve) )
        RETURN ( FALSE ) ;
    n = c2p_n ( C2_CURVE_PCURVE(curve) ) ;
    if ( i < 0 || i >= n - 1 )
        RETURN ( FALSE ) ;
    if ( is_arc != NULL )
        *is_arc = c2p_get_data ( C2_CURVE_PCURVE(curve), i, 
            ctr, ept1, rad_ptr, st_angle_ptr, sweep_ptr, dir_ptr ) ;
    else
        (void) c2p_get_data ( C2_CURVE_PCURVE(curve), i, 
            ctr, ept1, rad_ptr, st_angle_ptr, sweep_ptr, dir_ptr ) ;
    RETURN ( TRUE ) ;
}


/*--------------------------------------------------------------------*/
BBS_PUBLIC INT c2c_pcurve_n ( curve )
/*--------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
{
    RETURN ( C2_CURVE_IS_PCURVE(curve) ? 
        c2p_n ( C2_CURVE_PCURVE(curve) ) : 0 ) ;
}


/*--------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_pcurve_segment_epts_d ( curve, i, ept0, ept1, d_ptr )
/*--------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
INT i ;
PT2 ept0, ept1 ;
REAL *d_ptr ;
{
  RETURN ( c2p_segment_epts_d ( C2_CURVE_PCURVE(curve), 
				      i, ept0, ept1, d_ptr ) ) ;
}



/*--------------------------------------------------------------------*/
BBS_PRIVATE void c2c_pcurve_trim ( curve, parm0, parm1 )
/*--------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm0, parm1 ;
{
  C2_PCURVE pcurve ;
  INT n ;

  if ( !C2_CURVE_IS_PCURVE(curve) )
    RETURN ;
  pcurve = c2p_trim ( C2_CURVE_PCURVE(curve), 
     parm0 == NULL ? C2_CURVE_T0(curve) : PARM_T(parm0), 
     parm1 == NULL ? C2_CURVE_T1(curve) : PARM_T(parm1) ) ;
  n = c2p_n ( pcurve ) ;
  C2_CURVE_T0(curve) = 0.0 ;
  C2_CURVE_J0(curve) = 0 ;
  C2_CURVE_T1(curve) = (REAL)(n-1) ;
  C2_CURVE_J1(curve) = n-2 ;
  c2p_free ( C2_CURVE_PCURVE(curve) ) ;
  C2_CURVE_PCURVE(curve) = pcurve ;
}


#ifdef SPLINE
/*--------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_get_ellipse_data ( curve, ctr, 
            major_axis, minor_axis, angle ) 
/*--------------------------------------------------------------------*/
C2_CURVE curve ;
PT2 ctr ;
REAL *major_axis, *minor_axis, *angle ;
{
    RETURN ( C2_CURVE_IS_ELLIPSE(curve) && 
        c2n_get_ellipse_data ( C2_CURVE_NURB(curve), 
            ctr, major_axis, minor_axis, angle ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2c_ellipse_to_nurb ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
{
    if ( !C2_CURVE_IS_ELLIPSE(curve) ) 
        RETURN ( FALSE ) ;
    else {
        C2_CURVE_TYPE(curve) = C2_NURB_TYPE ;
        RETURN ( TRUE ) ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_ellipse_pt_tan ( ctr, 
            major_axis, minor_axis, angle, t, pt, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr ;
REAL major_axis, minor_axis, angle, t ;
PT2 pt, tan_vec ;
{
    REAL cosa, sina, cost, sint ;

    cosa = (REAL)cos ( (double)angle ) ;
    sina = (REAL)sin ( (double)angle ) ;
    cost = (REAL)cos ( (double)t ) ;
    sint = (REAL)sin ( (double)t ) ;

    if ( pt != NULL )
    {
        pt[0] = ctr[0] + major_axis * cost * cosa - minor_axis * sint * sina ;
        pt[1] = ctr[1] + major_axis * cost * sina + minor_axis * sint * cosa ;
    }
    if ( tan_vec != NULL )
    {
        tan_vec[0] = - major_axis * sint * cosa - minor_axis * cost * sina ;
        tan_vec[1] = - major_axis * sint * sina + minor_axis * cost * cosa ;
    }
    RETURN ( TRUE ) ;
}
#endif /*SPLINE*/

