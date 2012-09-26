/* -R __BBS_MILL__=1 __BBS_TURN__=1 */
/******************************* M2C.C **********************************/
/**************** Two-dimensional curves ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alrdefs.h> 
#include <c2vmcrs.h> 
#include <c2adefs.h > 
#include <c2cdefs.h> 
#include <c2cpriv.h> 
#include <c2ddefs.h> 
#include <c2dpriv.h> 
#include <c2vdefs.h> 
#include <dmldefs.h> 
#include <m2cdefs.h> 
#include <m2gdefs.h> 
#include <m2ldefs.h> 
#include <m2ndefs.h> 
#ifdef NEW_CODE
#include <m2pdefs.h> 
#endif
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif
STATIC INT m2c_axis_side_curve __(( C2_CURVE, REAL )) ;

/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN     m2c_x_max ( curve, x_max_ptr, x_max_parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL *x_max_ptr ;
PARM x_max_parm ;
{
    if ( C2_CURVE_X_MAX(curve) < *x_max_ptr ) 
        RETURN ( FALSE ) ;

    if ( C2_CURVE_IS_LINE(curve) ) {
        if ( m2l_x_max ( C2_CURVE_LINE(curve), C2_CURVE_T0(curve),
            C2_CURVE_T1(curve), x_max_ptr, &PARM_T(x_max_parm) ) ) {
            PARM_J(x_max_parm) = 1 ;
            RETURN ( TRUE ) ;
        }
        else
            RETURN ( FALSE ) ;
    }

    else if ( C2_CURVE_IS_ARC(curve) ) {
        if ( m2g_x_max ( C2_CURVE_ARC(curve), C2_CURVE_T0(curve),
            C2_CURVE_T1(curve), x_max_ptr, &PARM_T(x_max_parm) ) ) {
            alr_parm_set ( x_max_parm ) ;
            RETURN ( TRUE ) ;
        }
        else
            RETURN ( FALSE ) ;
    }
    else 
#ifdef  SPLINE
        RETURN ( m2n_x_max ( C2_CURVE_NURB(curve), C2_CURVE_PARM0(curve), 
            C2_CURVE_PARM1(curve), x_max_ptr, x_max_parm ) ) ;
#else  /*SPLINE*/
        RETURN ( FALSE ) ;
#endif  /*SPLINE*/
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE INT m2c_hor_ray_int_no ( curve, pt, pt_on_curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 pt ;
BOOLEAN pt_on_curve ;
{
    if ( C2_CURVE_X_MAX(curve) < PT2_X(pt) - BBS_TOL ) 
        RETURN ( 0 ) ;
    if ( C2_CURVE_Y_MAX(curve) < PT2_Y(pt) - BBS_TOL )
        RETURN ( 0 ) ;
    if ( C2_CURVE_Y_MIN(curve) > PT2_Y(pt) + BBS_TOL )
        RETURN ( 0 ) ;
    if ( C2_CURVE_IS_LINE(curve) ) 
        RETURN ( m2l_hor_ray_int_no ( C2_CURVE_LINE(curve), 
                C2_CURVE_T0(curve), C2_CURVE_T1(curve), pt, pt_on_curve ) ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        RETURN ( m2g_hor_ray_int_no ( C2_CURVE_ARC(curve), 
                C2_CURVE_T0(curve), C2_CURVE_T1(curve), pt, pt_on_curve ) ) ;
#ifdef NEW_CODE
    else if ( C2_CURVE_IS_PCURVE(curve) ) 
        RETURN ( m2p_hor_ray_int_no ( C2_CURVE_PCURVE(curve), 
                C2_CURVE_T0(curve), C2_CURVE_T1(curve), pt, pt_on_curve ) ) ;
#endif
    else 
#ifdef  SPLINE
        RETURN ( m2n_hor_ray_int_no ( C2_CURVE_NURB(curve), 
            C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve), pt, pt_on_curve ) ) ;
#else
        RETURN ( 0 ) ;
#endif  /*SPLINE*/
}    


/*----------------------------------------------------------------------*/
BBS_PRIVATE INT m2c_ray_int_no ( curve, pt, c, s, pt_on_curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 pt ;
REAL c, s ;
BOOLEAN pt_on_curve ;
{
    /* Do a bounding box test */

    if ( C2_CURVE_IS_LINE(curve) ) 
        RETURN ( m2l_ray_int_no ( C2_CURVE_LINE(curve), 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve), pt, c, s, pt_on_curve ) ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        RETURN ( m2g_ray_int_no ( C2_CURVE_ARC(curve), 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve), pt, c, s, pt_on_curve ) ) ;
#ifdef NEW_CODE
    else if ( C2_CURVE_IS_PCURVE(curve) ) 
        RETURN ( m2p_ray_int_no ( C2_CURVE_PCURVE(curve), 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve), pt, c, s, pt_on_curve ) ) ;
#endif
    else 
#ifdef  SPLINE
        RETURN ( m2n_ray_int_no ( C2_CURVE_NURB(curve), C2_CURVE_PARM0(curve), 
            C2_CURVE_PARM1(curve), pt, c, s, pt_on_curve ) ) ;
#else
        RETURN ( 0 ) ;
#endif  /*SPLINE*/
}    


/*----------------------------------------------------------------------*/
BBS_PRIVATE REAL m2c_area ( curve, origin ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 origin ;
{
    if ( C2_CURVE_IS_LINE(curve) ) 
        RETURN ( m2l_area ( C2_CURVE_LINE(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), origin ) ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        RETURN ( m2g_area ( C2_CURVE_ARC(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), origin ) ) ;
#ifdef NEW_CODE
    else if ( C2_CURVE_IS_PCURVE(curve) ) 
        RETURN ( m2p_area ( C2_CURVE_PCURVE(curve), 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve), origin ) ) ;
#endif
    else 
#ifdef  SPLINE
        RETURN ( m2n_area ( C2_CURVE_NURB(curve), 
            C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve), origin ) ) ;
#else
        RETURN ( 0.0 ) ;
#endif  /* SPLINE */
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2c_area_mass_ctr ( curve, ctr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 ctr ;
{
    if ( C2_CURVE_IS_LINE(curve) ) 
        RETURN ( FALSE ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        RETURN ( FALSE ) ;
#ifdef NEW_CODE
    else if ( C2_CURVE_IS_PCURVE(curve) ) 
        RETURN ( FALSE ) ;
#endif
    else 
#ifdef  SPLINE
        RETURN ( m2n_area_mass_ctr ( C2_CURVE_NURB(curve), 
            C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve), ctr ) ) ;
#else
    RETURN ( FALSE ) ;
#endif  /* SPLINE */

}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2c_tan_angle ( curve, angle, parm )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL angle ;
PARM parm ;
{
    REAL t[2] ;
    INT i, n ;

    if ( C2_CURVE_IS_LINE(curve) ) 
        n = 0 ; /* m2l_tan_angle ( C2_CURVE_LINE(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), angle, t ) ; */
    else if ( C2_CURVE_IS_ARC(curve) ) 
        n = m2g_tan_angle ( C2_CURVE_ARC(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), angle, t ) ;
    else n = 0 ;

    for ( i=0 ; i<n ; i++ ) {
        PARM_T(parm+i) = t[i] ;
        if ( C2_CURVE_IS_LINE(curve) )
            PARM_J(parm+i) = 1 ;
        else if ( C2_CURVE_IS_ARC(curve) ) 
            alr_parm_set ( parm+i ) ;
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2c_break_curves ( curvelist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curvelist ;
{
    DML_ITEM item, item1 ;
    C2_CURVE curve, curve1 ;
    BOOLEAN status = FALSE ;
    INT i, n ;

    for ( item = DML_FIRST(curvelist) ; item != NULL ; item = item1 ) {
        item1 = DML_NEXT(item) ;
        curve = DML_RECORD(item) ;
        if ( C2_CURVE_IS_ARC(curve) && IS_SMALL ( C2_CURVE_T0(curve) ) &&
            IS_SMALL ( C2_CURVE_T1(curve) - 2.0 ) ) {
            status = TRUE ;
            c2c_trim_t1 ( curve, 1.0 ) ;
            curve1 = c2d_arc_complement ( curve ) ;
            dml_insert_after ( curvelist, item, curve1 ) ;
        }
        else if ( C2_CURVE_IS_PCURVE(curve) ) {
            status = TRUE ;
            n = c2c_pcurve_n ( curve ) - 1 ;
            for ( i = 0 ; i < n ; i++ ) {
                curve1 = c2d_pcurve_segment ( curve, i ) ;
                dml_insert_prior ( curvelist, item, curve1 ) ;
            }
            c2d_free_curve ( curve ) ;
            dml_remove_item ( curvelist, item ) ;
        }
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2c_break_circles ( curvelist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curvelist ;
{
    DML_ITEM item, item1 ;
    C2_CURVE curve, curve1 ;
    BOOLEAN status = FALSE ;

    for ( item = DML_FIRST(curvelist) ; item != NULL ; item = item1 ) {
        item1 = DML_NEXT(item) ;
        curve = DML_RECORD(item) ;
        if ( C2_CURVE_IS_ARC(curve) && IS_SMALL ( C2_CURVE_T0(curve) ) &&
            IS_SMALL ( C2_CURVE_T1(curve) - 2.0 ) ) {
            status = TRUE ;
            c2c_trim_t1 ( curve, 1.0 ) ;
            curve1 = c2d_arc_complement ( curve ) ;
            dml_insert_after ( curvelist, item, curve1 ) ;
        }
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2c_break_pcurves ( curvelist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curvelist ;
{
    DML_ITEM item, item1 ;
    C2_CURVE curve, curve1 ;
    BOOLEAN status = FALSE ;
    INT i, n ;

    for ( item = DML_FIRST(curvelist) ; item != NULL ; item = item1 ) {
        item1 = DML_NEXT(item) ;
        curve = DML_RECORD(item) ;
        if ( C2_CURVE_IS_PCURVE(curve) ) {
            status = TRUE ;
            n = c2c_pcurve_n ( curve ) - 1 ;
            for ( i = 0 ; i < n ; i++ ) {
                curve1 = c2d_pcurve_segment ( curve, i ) ;
                dml_insert_prior ( curvelist, item, curve1 ) ;
            }
            c2d_free_curve ( curve ) ;
            dml_remove_item ( curvelist, item ) ;
        }
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST m2c_reduce_lines ( curvelist, tol )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curvelist ;
REAL tol ;
{
    DML_ITEM item0, item1 ;
    C2_CURVE curve0, curve1 ;
    BOOLEAN status = FALSE ;
    PT2 p0, p1, q0, q1 ;
    REAL area, dist, h ;

    if ( curvelist == NULL ) 
        RETURN ( NULL ) ;
    DML_WALK_LIST ( curvelist, item0 ) {
        item1 = dml_next ( item0 ) ;
        if ( item1 == NULL ) 
            RETURN ( curvelist ) ;
        curve0 = DML_RECORD(item0) ;
        curve1 = DML_RECORD(item1) ;
        status = C2_CURVE_IS_LINE(curve0) && C2_CURVE_IS_LINE(curve1) ;

        if ( status ) {
            c2c_ept0 ( curve0, p0 ) ;
            c2c_ept1 ( curve0, p1 ) ;
            c2c_ept0 ( curve1, q0 ) ;
            c2c_ept1 ( curve1, q1 ) ;
            status = C2V_IDENT_PTS ( p1, q0 ) ;
        }
        if ( status ) {
            dist = C2V_DISTL1 ( p0, q1 ) ;
            area = fabs ( c2v_area ( p0, p1, q1 ) ) ;
            if ( dist >= tol ) {
                h = 2.0 * area / dist ;
                status = ( h <= tol ) ;
            }
            else
                status = FALSE ;
        }

        if ( status ) {
            c2d_free_curve ( curve0 ) ;
            c2d_free_curve ( curve1 ) ;
            dml_remove_item ( curvelist, item0 ) ;
            DML_RECORD(item1) = c2d_line ( p0, q1 ) ;
            item0 = item1 ;
        }
    }
    RETURN ( curvelist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void m2c_set_ept0 ( curve, pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 pt ;
{
    if ( C2_CURVE_IS_LINE(curve) ) 
        m2l_set_ept0 ( C2_CURVE_LINE(curve), pt, 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve) ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        m2g_set_ept0 ( C2_CURVE_ARC(curve), pt, 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve) ) ;
    c2c_box ( curve ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void m2c_set_ept1 ( curve, pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 pt ;
{
    if ( C2_CURVE_IS_LINE(curve) ) 
        m2l_set_ept1 ( C2_CURVE_LINE(curve), pt, 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve) ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        m2g_set_ept1 ( C2_CURVE_ARC(curve), pt, 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve) ) ;
    c2c_box ( curve ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL m2c_size ( curve )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
{
    if ( C2_CURVE_IS_LINE(curve) ) 
        RETURN ( m2l_size ( C2_CURVE_LINE(curve), 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve) ) ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        RETURN ( m2g_size ( C2_CURVE_ARC(curve), 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve) ) ) ;
    else
        RETURN ( -1.0 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2c_adjust ( curve0, dir0, curve1, dir1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve0, curve1 ;
INT dir0, dir1 ;
{
    REAL t0_0, t0_1, t1_0, t1_1 ;
    BOOLEAN status = FALSE ;

    if ( dir0 == 1 ) {
        t0_0 = C2_CURVE_T0(curve0) ;
        t0_1 = C2_CURVE_T1(curve0) ;
    }
    else {
        t0_0 = C2_CURVE_T1(curve0) ;
        t0_1 = C2_CURVE_T0(curve0) ;
    }
    if ( dir1 == 1 ) {
        t1_0 = C2_CURVE_T0(curve1) ;
        t1_1 = C2_CURVE_T1(curve1) ;
    }
    else {
        t1_0 = C2_CURVE_T1(curve1) ;
        t1_1 = C2_CURVE_T0(curve1) ;
    }
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
paint_curve ( curve0, 9 ) ;
paint_curve ( curve1, 10 ) ;
getch ();
}
else
    DISPLAY-- ;
#endif
    if ( C2_CURVE_IS_LINE(curve0) ) {
        if ( C2_CURVE_IS_LINE(curve1) ) {
            status = m2l_adjust_2lines ( 
                C2_CURVE_LINE(curve0), t0_0, t0_1, 
                C2_CURVE_LINE(curve1), t1_0, t1_1 ) ; 
        }
        else if ( C2_CURVE_IS_ARC(curve1) ) {
            status = m2g_adjust_line_arc ( 
                C2_CURVE_LINE(curve0), t0_0, t0_1, dir0,  
                C2_CURVE_ARC(curve1), t1_0, t1_1, dir1 ) ; 
        }
        else
            RETURN ( FALSE ) ;
    }

    else if ( C2_CURVE_IS_ARC(curve0) ) {
        if ( C2_CURVE_IS_LINE(curve1) ) {
            status = m2g_adjust_arc_line ( 
                C2_CURVE_ARC(curve0), t0_0, t0_1, dir0, 
                C2_CURVE_LINE(curve1), t1_0, t1_1, dir1 ) ; 
        }
        else if ( C2_CURVE_IS_ARC(curve1) ) {
            status = m2g_adjust_2arcs ( 
                C2_CURVE_ARC(curve0), t0_0, t0_1, dir0, 
                C2_CURVE_ARC(curve1), t1_0, t1_1, dir1 ) ; 
        }
        else
            RETURN ( FALSE ) ;
    }

    else
        RETURN ( FALSE ) ;

    if ( C2_CURVE_IS_ARC(curve0) ) {
        C2_CURVE_T0(curve0) = 0.0 ;
        C2_CURVE_T1(curve0) = 1.0 ;
        C2_CURVE_J0(curve0) = 0 ;
        C2_CURVE_J1(curve0) = 0 ;
    }
    if ( C2_CURVE_IS_ARC(curve1) ) {
        C2_CURVE_T0(curve1) = 0.0 ;
        C2_CURVE_T1(curve1) = 1.0 ;
        C2_CURVE_J0(curve1) = 0 ;
        C2_CURVE_J1(curve1) = 0 ;
    }

#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
paint_curve ( curve0, 11 ) ;
paint_curve ( curve1, 12 ) ;
getch ();
}
else
    DISPLAY-- ;
#endif
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2c_remove_curve ( curve, tol ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL tol ;
{
#ifdef DDEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
paint_curve ( curve, 11 ) ;
getch ();
}
else
    DISPLAY-- ;
#endif
    if ( C2_CURVE_IS_LINE(curve) ) 
        RETURN ( m2l_is_small ( C2_CURVE_LINE(curve), 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve), tol ) ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        RETURN ( m2g_is_small ( C2_CURVE_ARC(curve), 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve), tol ) ) ;
    else
        RETURN ( FALSE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE INT m2c_curves_above ( curves, y ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curves ;
REAL y ;
{
    C2_BOX_S box ;

    c2c_box_curves ( curves, &box ) ;
    if ( C2_MIN_Y(&box) > y ) 
        RETURN ( 1 ) ;
    else if ( C2_MAX_Y(&box) < y ) 
        RETURN ( -1 ) ;
    else 
        RETURN ( 0 ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE INT m2c_arc_polygon ( curve, m, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
INT m ;
PT2 *a ;
{
    RETURN ( C2_CURVE_IS_ARC(curve) ? 
        m2g_polygon ( C2_CURVE_ARC(curve), m, a ) : 0 ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE INT m2c_intersect ( curve1, curve2, 
            vtx_status, no_coinc, inters_list )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve1, curve2 ;
BOOLEAN vtx_status, no_coinc ;
DML_LIST inters_list ;
{
    INT i, n, m, type[4] ;
    REAL t1[4], t2[4] ;
    PT2 int_pt[4] ;
    PARM_S parm1, parm2 ;

#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -2 ) {
        c2c_write_curve ( "curve21.dat", curve1 ) ;
        c2c_write_curve ( "curve22.dat", curve2 ) ;
    }
    else if ( DIS_LEVEL == -3 ) {
        c2c_write_curve ( "curve31.dat", curve1 ) ;
        c2c_write_curve ( "curve32.dat", curve2 ) ;
    }
    DISPLAY-- ;
#endif

    if ( curve1 == curve2 ) 
        RETURN ( c2c_self_intersect ( curve1, inters_list ) ) ;
    if ( !c2a_box_overlap ( C2_CURVE_BOX(curve1), C2_CURVE_BOX(curve2) ) ) 
        RETURN ( 0 ) ;
    if ( C2_CURVE_IS_LINE(curve1) ) {
        if ( C2_CURVE_IS_LINE(curve2) ) 
            n = m2l_intersect ( C2_CURVE_LINE(curve1), C2_CURVE_T0(curve1), 
                C2_CURVE_T1(curve1), C2_CURVE_LINE(curve2), 
                C2_CURVE_T0(curve2), C2_CURVE_T1(curve2), 
                vtx_status, no_coinc, t1, t2, int_pt, type ) ;
        else if ( C2_CURVE_IS_ARC(curve2) ) 
            n = m2g_int_line_arc ( C2_CURVE_LINE(curve1), C2_CURVE_T0(curve1), 
                C2_CURVE_T1(curve1), C2_CURVE_ARC(curve2), 
                C2_CURVE_T0(curve2), C2_CURVE_T1(curve2), 
                vtx_status, t1, t2, int_pt, type ) ;
        else 
            RETURN ( c2c_intersect ( curve1, curve2, inters_list ) ) ;
    }

    else if ( C2_CURVE_IS_ARC(curve1) ) {
        if ( C2_CURVE_IS_LINE(curve2) ) 
            n = m2g_int_line_arc ( C2_CURVE_LINE(curve2), C2_CURVE_T0(curve2), 
                C2_CURVE_T1(curve2), C2_CURVE_ARC(curve1), 
                C2_CURVE_T0(curve1), C2_CURVE_T1(curve1), 
                vtx_status, t2, t1, int_pt, type ) ;
        else if ( C2_CURVE_IS_ARC(curve2) ) 
            n = m2g_int_arc_arc ( C2_CURVE_ARC(curve1), C2_CURVE_T0(curve1), 
                C2_CURVE_T1(curve1), C2_CURVE_ARC(curve2), 
                C2_CURVE_T0(curve2), C2_CURVE_T1(curve2), 
                vtx_status, no_coinc, t1, t2, int_pt, type ) ;
        else 
            RETURN ( c2c_intersect ( curve1, curve2, inters_list ) ) ;
    }
    else 
        RETURN ( c2c_intersect ( curve1, curve2, inters_list ) ) ;

    m = 0 ;
    for ( i=0 ; i<n ; i++ ) {
        PARM_T(&parm1) = t1[i] ;
        if ( C2_CURVE_IS_LINE(curve1) ) 
            PARM_J(&parm1) = 1 ;
        else if ( C2_CURVE_IS_ARC(curve1) ) 
            alr_parm_set ( &parm1 ) ;

        PARM_T(&parm2) = t2[i] ;
        if ( C2_CURVE_IS_LINE(curve2) ) 
            PARM_J(&parm2) = 1 ;
        else if ( C2_CURVE_IS_ARC(curve2) ) 
            alr_parm_set ( &parm2 ) ;
        if ( c2c_append_int_rec ( inters_list, &parm1, &parm2, int_pt[i], 
            0.0, type[i], FALSE ) )
            m++ ;
    }
    RETURN ( m ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE INT m2c_axis_side ( curves, y )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curves ;
REAL y ;
{
    INT axis_side, curve_side ;
    DML_ITEM item ;

    axis_side = 0 ;
    DML_WALK_LIST ( curves, item ) {
        curve_side = m2c_axis_side_curve ( DML_RECORD(item), y ) ;
        if ( axis_side == 0 )
            axis_side = curve_side ;
        else if ( axis_side != curve_side )
            RETURN ( 0 ) ;
    }
    RETURN ( axis_side ) ;
}


/*----------------------------------------------------------------------*/
STATIC INT m2c_axis_side_curve ( curve, y )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL y ;
{
    if ( C2_CURVE_Y_MIN(curve) > y ) 
        RETURN ( 1 ) ;
    else if ( C2_CURVE_Y_MAX(curve) < y ) 
        RETURN ( -1 ) ; 
    else 
        RETURN ( 0 ) ; 
    /* check if the curve intersects the axis */
} 


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE BOOLEAN m2c_furthest_pt ( curve, pt, parm0, parm1, curve_pt )
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C2_CURVE curve ;
PT2 pt ;
PARM parm0, parm1 ;
PT2 curve_pt ;
{
    REAL t0, t1 ;

    t0 = ( parm0 == NULL ) ? C2_CURVE_T0(curve) : PARM_T(parm0) ;
    t1 = ( parm1 == NULL ) ? C2_CURVE_T1(curve) : PARM_T(parm1) ;

    if ( C2_CURVE_IS_LINE(curve) ) 
        RETURN ( m2l_furthest_pt ( C2_CURVE_LINE(curve), pt, 
            t0, t1, curve_pt ) ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        RETURN ( m2g_furthest_pt ( C2_CURVE_ARC(curve), pt, 
            t0, t1, curve_pt ) ) ;
    else
        RETURN ( FALSE ) ;
}

#ifdef OLD_CODE
/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE BOOLEAN m2c_increment ( curve, dir, parm0, parm1, w_ptr, pt, parm )
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C2_CURVE curve ;
INT dir ;
PARM parm0, parm1 ;
REAL *w_ptr ;
PT2 pt ;
PARM parm ;
{
    REAL t0, t1 ;

    t0 = ( parm0 == NULL ) ? C2_CURVE_T0(curve) : PARM_T(parm0) ;
    t1 = ( parm1 != NULL ) ? PARM_T(parm1) :
        ( ( dir == 1 ) ? C2_CURVE_T1(curve) : C2_CURVE_T0(curve) ) ;

    if ( C2_CURVE_IS_LINE(curve) ) 
        RETURN ( m2l_increment ( C2_CURVE_LINE(curve), dir, t0, t1, 
            w_ptr, pt, parm ) ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        RETURN ( m2g_increment ( C2_CURVE_ARC(curve), dir, t0, t1, 
            w_ptr, pt, parm ) ) ;
    else
        RETURN ( FALSE ) ;
}
#endif

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2c_curve_is_horline ( curve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                 !!!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
{
    RETURN ( C2_CURVE_IS_LINE(curve) && 
        m2l_horline ( C2_CURVE_LINE(curve) ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void m2c_inflate ( curve, dist, curve1, curve2, arc0, arc1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL dist ;
C2_CURVE *curve1, *curve2, *arc0, *arc1 ;
{
    PT2 p10, p11, p20, p21 ;

    *curve1 = c2d_offset_curve ( curve, dist ) ;
    *curve2 = c2d_offset_curve ( curve, -dist ) ;
    if ( *curve1 == NULL && *curve2 == NULL ) 
        RETURN ;
    if ( *curve1 == NULL ) 
        *curve1 = c2d_copy ( curve ) ;
    if ( *curve2 == NULL ) 
        *curve2 = c2d_copy ( curve ) ;
    c2c_ept0 ( *curve1, p10 ) ;
    c2c_ept1 ( *curve1, p11 ) ;
    c2c_ept0 ( *curve2, p20 ) ;
    c2c_ept1 ( *curve2, p21 ) ;
    *arc0 = c2d_arc_pts_bulge ( p20, p10, 1.0 ) ;
    *arc1 = c2d_arc_pts_bulge ( p11, p21, 1.0 ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE INT m2c_approx_n ( curve, parm0, parm1, acc )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm0, parm1 ;
REAL acc ;
{
    REAL t0, t1 ;

    t0 = ( parm0 == NULL ) ? 0.0 : PARM_T(parm0) ;
    t1 = ( parm1 == NULL ) ? 1.0 : PARM_T(parm1) ;
    if ( IS_ZERO ( t0 - t1 ) )
        RETURN ( 1 ) ;
    else if ( C2_CURVE_IS_LINE(curve) )
        RETURN ( 1 ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        RETURN ( m2g_approx_n ( C2_CURVE_ARC(curve), t0, t1, acc ) ) ;
    else
        RETURN ( 0 ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2c_along ( curve, parm0, parm1, l, dir, parm )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm0, parm1 ;
REAL l ;
INT dir ;
PARM parm ;
{
    REAL t0, t1 ;
    BOOLEAN status ;

    if ( parm0 == NULL ) 
        t0 = ( dir == 1 ) ? C2_CURVE_T0(curve) : C2_CURVE_T1(curve) ;
    else 
        t0 = PARM_T(parm0) ;
    if ( parm1 == NULL ) 
        t1 = ( dir == 1 ) ? C2_CURVE_T1(curve) : C2_CURVE_T0(curve) ;
    else 
        t1 = PARM_T(parm1) ;

    if ( C2_CURVE_IS_LINE(curve) ) {
        PARM_T(parm) = m2l_along ( C2_CURVE_LINE(curve), t0, l, dir ) ;
        if ( dir == 1 ) {
            if ( PARM_T(parm) > t1 + BBS_ZERO ) {
                PARM_J(parm) = 2 ;
                RETURN ( FALSE ) ;
            }
            else if ( PARM_T(parm) < t0 - BBS_ZERO ) {
                PARM_J(parm) = 0 ;
                RETURN ( FALSE ) ;
            }
            else {
                PARM_J(parm) = 1 ;
                RETURN ( TRUE ) ;
            }
        }
        else {
            if ( PARM_T(parm) > t0 + BBS_ZERO ) {
                PARM_J(parm) = 2 ;
                RETURN ( FALSE ) ;
            }
            else if ( PARM_T(parm) < t1 - BBS_ZERO ) {
                PARM_J(parm) = 0 ;
                RETURN ( FALSE ) ;
            }
            else {
                PARM_J(parm) = 1 ;
                RETURN ( TRUE ) ;
            }
        }
    }

    else if ( C2_CURVE_IS_ARC(curve) ) {
        status = m2g_along ( C2_CURVE_ARC(curve), t0, l, dir, &PARM_T(parm) ) ;
        if ( status ) {
            alr_parm_set ( parm ) ;
            RETURN ( TRUE ) ;
        }
        else if ( PARM_T(parm) > t1 + BBS_ZERO ) {
            PARM_J(parm) = 2 ;
            RETURN ( FALSE ) ;
        }
        else if ( PARM_T(parm) < t0 - BBS_ZERO ) {
            PARM_J(parm) = 0 ;
            RETURN ( FALSE ) ;
        }
    }
    else
        RETURN ( FALSE ) ;
#ifdef __BRLNDC__
	RETURN ( FALSE ) ;
#endif
#ifdef __WATCOM__
	RETURN ( FALSE ) ;
#endif
}

