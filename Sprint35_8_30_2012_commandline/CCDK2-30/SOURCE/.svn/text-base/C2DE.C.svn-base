/* -2 -3*/
/******************************* C2DE.C *********************************/
/********** Two-dimensional curve construction routines *****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#include <alsdefs.h>
#include <c2ndefs.h>
#include <c2cdefs.h>
#include <c2ddefs.h>
#include <c2dpriv.h>
#include <c2vmcrs.h>
#include <c2cpriv.h>
STATIC C2_CURVE c2d_ell_to_pcurve_1 __(( C2_CURVE, PT2, REAL, REAL, REAL, 
            REAL, REAL, REAL, REAL, PT2 )) ;
#ifdef  SPLINE
/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_ellipse ( ctr, major_axis, minor_axis, angle ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr ;
REAL major_axis, minor_axis, angle ;
{
    C2_CURVE ellipse = c2d_curve() ;
    if ( ellipse == NULL ) 
        RETURN ( NULL ) ;

    C2_CURVE_TYPE(ellipse) = C2_ELLIPSE_TYPE ;
    C2_CURVE_NURB(ellipse) = 
        c2n_ellipse ( ctr, major_axis, minor_axis, angle ) ;
    c2n_box ( C2_CURVE_NURB(ellipse), C2_CURVE_BOX(ellipse) ) ;
    C2_CURVE_T0(ellipse) = 0.0 ;
    C2_CURVE_J0(ellipse) = 2 ;
    C2_CURVE_T1(ellipse) = 2.0 ;
    C2_CURVE_J1(ellipse) = 8 ;
    RETURN ( ellipse ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_ellipse_to_pcurve ( ctr, major_axis, minor_axis, 
            angle, t0, t1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr ;
REAL major_axis, minor_axis, angle, t0, t1 ;
{
    REAL r, u0, u1 ;
    PT2 pt, tan0 ;
    C2_CURVE pcurve ;
    PARM_S parms0, parms1 ;
    PARM   parm0, parm1 ;

    r = minor_axis / major_axis ;
    if ( !als_ellipse_break_t ( 0.0, 0.25*PI, r, &u0 ) )
        RETURN ( NULL ) ;
    if ( !als_ellipse_break_t ( 0.25*PI, HALF_PI, r, &u1 ) )
        RETURN ( NULL ) ;

    c2c_ellipse_pt_tan ( ctr, major_axis, minor_axis, angle, 0.0, pt, tan0 ) ;
    pcurve = c2d_pcurve_init_size ( pt, 17 ) ;
    if ( pcurve == NULL ) 
        RETURN ( NULL ) ;

    if ( c2d_ell_to_pcurve_1 ( pcurve, ctr, major_axis, minor_axis, 
        angle, u0, 0.25*PI, u1, HALF_PI, tan0 ) == NULL ) 
    {
        c2d_free_curve ( pcurve ) ;
        RETURN ( NULL ) ;
    }
    if ( c2d_ell_to_pcurve_1 ( pcurve, ctr, major_axis, minor_axis, 
        angle, PI - u1, 0.75*PI, PI - u0, PI, NULL ) == NULL ) 
    {
        c2d_free_curve ( pcurve ) ;
        RETURN ( NULL ) ;
    }
    if ( c2d_ell_to_pcurve_1 ( pcurve, ctr, major_axis, minor_axis, 
        angle, PI + u0, 1.25*PI, PI + u1, 1.5 * PI, NULL ) == NULL ) 
    {
        c2d_free_curve ( pcurve ) ;
        RETURN ( NULL ) ;
    }
    if ( c2d_ell_to_pcurve_1 ( pcurve, ctr, major_axis, minor_axis, 
        angle, TWO_PI - u1, 1.75*PI, TWO_PI - u0, TWO_PI, NULL ) == NULL ) 
    {
        c2d_free_curve ( pcurve ) ;
        RETURN ( NULL ) ;
    }

    if ( t0 > BBS_ZERO ) 
    {
        c2c_ellipse_pt_tan ( ctr, major_axis, minor_axis, angle, 
            t0, pt, NULL ) ;
	parm0 = &parms0 ;
        c2c_project ( pcurve, pt, parm0, NULL ) ;
    }
    else
      parm0 = NULL ;

    if ( t1 < TWO_PI - BBS_ZERO ) 
    {
        c2c_ellipse_pt_tan ( ctr, major_axis, minor_axis, angle, 
            t1, pt, NULL ) ;
	parm1 = &parms1 ;
        c2c_project ( pcurve, pt, parm1, NULL ) ;
    }
    else
      parm1 = NULL ;

    if ( parm0 != NULL || parm1 != NULL )
      c2c_pcurve_trim ( pcurve, parm0, parm1 ) ;

    RETURN ( pcurve ) ;
}


/*----------------------------------------------------------------------*/
STATIC C2_CURVE c2d_ell_to_pcurve_1 ( pcurve, ctr, major_axis, minor_axis, 
            angle, a0, a1, a2, a3, tan0 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE pcurve ;
PT2 ctr ;
REAL major_axis, minor_axis, angle, a0, a1, a2, a3 ;
PT2 tan0 ;
{
    PT2 pt ;

    c2c_ellipse_pt_tan ( ctr, major_axis, minor_axis, angle, a0, pt, NULL ) ;
    if ( tan0 == NULL )
    {
        if ( c2d_pcurve_add_arc_tan ( pcurve, pt ) == NULL )
            RETURN ( NULL ) ;
    }
    else
    {
        if ( c2d_pcurve_add_arc_tan0 ( pcurve, pt, tan0 ) == NULL )
            RETURN ( NULL ) ;
    }
    c2c_ellipse_pt_tan ( ctr, major_axis, minor_axis, angle, a1, pt, NULL ) ;
    if ( c2d_pcurve_add_arc_tan ( pcurve, pt ) == NULL )
        RETURN ( NULL ) ;
    c2c_ellipse_pt_tan ( ctr, major_axis, minor_axis, angle, a2, pt, NULL ) ;
    if ( c2d_pcurve_add_arc_tan ( pcurve, pt ) == NULL )
        RETURN ( NULL ) ;
    c2c_ellipse_pt_tan ( ctr, major_axis, minor_axis, angle, a3, pt, NULL ) ;
    if ( c2d_pcurve_add_arc_tan ( pcurve, pt ) == NULL )
        RETURN ( NULL ) ;
    RETURN ( pcurve ) ;
}


#endif  /* SPLINE */

