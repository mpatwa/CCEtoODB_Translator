/* -2 -3 */
/******************************* C2DP.C *********************************/
/********** Two-dimensional polycurve construction routines *************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2pdefs.h>
#include <c2adefs.h>
#include <c2cdefs.h>
#include <c2ddefs.h>
#include <c2dpriv.h>
#include <c2vmcrs.h>
STATIC C2_CURVE c2d_pcurve_add __(( C2_CURVE, PT2, PT2, REAL, REAL )) ;

/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_CURVE c2d_pcurve_frame ( s, n ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT s, n ;
{
    C2_CURVE curve ;

    curve = c2d_curve ();
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    C2_CURVE_TYPE(curve) = C2_PCURVE_TYPE ;
    C2_CURVE_T0(curve) = 0.0 ;
    C2_CURVE_J0(curve) = 0 ;
    C2_CURVE_T1(curve) = (REAL)(n-1) ;
    C2_CURVE_J1(curve) = n-2 ;
    C2_CURVE_PCURVE(curve) = c2p_create ( s, n ) ;
    if ( C2_CURVE_PCURVE(curve) == NULL ) {
        c2d_free_curve ( curve ) ;
        RETURN ( NULL ) ;
    }
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_pcurve_through ( a, n ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a      ;   /* a[n] */
INT n ;
{
    C2_CURVE curve ;

    curve = c2d_curve ();
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    C2_CURVE_TYPE(curve) = C2_PCURVE_TYPE ;
    C2_CURVE_T0(curve) = 0.0 ;
    C2_CURVE_J0(curve) = 0 ;
    C2_CURVE_T1(curve) = (REAL)(n-1) ;
    C2_CURVE_J1(curve) = n-2 ;
    C2_CURVE_PCURVE(curve) = c2p_through ( a, n ) ;
    if ( C2_CURVE_PCURVE(curve) == NULL ) {
        c2d_free_curve ( curve ) ;
        RETURN ( NULL ) ;
    }
    c2p_box ( C2_CURVE_PCURVE(curve), C2_CURVE_T0(curve), 
        C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_pcurve_init_size ( a, size ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 a ;
INT size ;
{
    C2_CURVE curve ;

    curve = c2d_pcurve_frame ( size, 1 ) ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    c2p_init ( C2_CURVE_PCURVE(curve), a ) ;
    c2a_box_init_pt ( C2_CURVE_BOX(curve), a ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_pcurve_init ( a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 a ;
{
    RETURN ( c2d_pcurve_init_size ( a, 10 ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_pcurve_add_arc_2pts ( curve, a0, a1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 a0, a1 ;
{
    if ( !C2_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    C2_CURVE_T1(curve) += 1.0 ;
    C2_CURVE_J1(curve) += 1 ;
    if ( c2p_add_arc_2pts ( C2_CURVE_PCURVE(curve), a0, a1 ) == NULL )
        RETURN ( NULL ) ;
    c2p_box_append ( C2_CURVE_PCURVE(curve), C2_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_pcurve_add_arc_ctr_pt ( curve, ctr, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 ctr, a ;
{
    if ( !C2_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    C2_CURVE_T1(curve) += 1.0 ;
    C2_CURVE_J1(curve) += 1 ;
    if ( c2p_add_arc_ctr_pt ( C2_CURVE_PCURVE(curve), ctr, a ) == NULL )
        RETURN ( NULL ) ;
    c2p_box_append ( C2_CURVE_PCURVE(curve), C2_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_pcurve_add_arc_tan ( curve, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 a ;
{
    if ( !C2_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    C2_CURVE_T1(curve) += 1.0 ;
    C2_CURVE_J1(curve) += 1 ;
    if ( c2p_add_arc_tan ( C2_CURVE_PCURVE(curve), a ) == NULL )
        RETURN ( NULL ) ;
    c2p_box_append ( C2_CURVE_PCURVE(curve), C2_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_pcurve_add_arc_tan0 ( curve, a, tan0 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 a, tan0 ;
{
    if ( !C2_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    C2_CURVE_T1(curve) += 1.0 ;
    C2_CURVE_J1(curve) += 1 ;
    if ( c2p_add_arc_tan0 ( C2_CURVE_PCURVE(curve), a, tan0 ) == NULL )
        RETURN ( NULL ) ;
    c2p_box_append ( C2_CURVE_PCURVE(curve), C2_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_pcurve_add_line ( curve, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 a ;
{
    if ( !C2_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    if ( c2p_add_line ( C2_CURVE_PCURVE(curve), a ) == NULL )
        RETURN ( NULL ) ;
    C2_CURVE_T1(curve) += 1.0 ;
    C2_CURVE_J1(curve) += 1 ;
    c2a_box_append_pt ( C2_CURVE_BOX(curve), a ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_pcurve_add_line_tan ( curve, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 a ;
{
    if ( !C2_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    if ( c2p_add_line_tan ( C2_CURVE_PCURVE(curve), a ) == NULL )
        RETURN ( NULL ) ;
    C2_CURVE_T1(curve) += 1.0 ;
    C2_CURVE_J1(curve) += 1 ;
    c2p_box_append ( C2_CURVE_PCURVE(curve), C2_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_pcurve_remove_last ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
{
    if ( !C2_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    C2_CURVE_T1(curve) -= 1.0 ;
    C2_CURVE_J1(curve) -= 1 ;
    RETURN ( c2p_remove_last ( C2_CURVE_PCURVE(curve) ) == NULL ? 
        NULL : curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_pcurve_close ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
{
    if ( !C2_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    C2_CURVE_T1(curve) += 1.0 ;
    C2_CURVE_J1(curve) += 1 ;
    c2p_close ( C2_CURVE_PCURVE(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_pcurve_segment ( curve, i ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
INT i ;
{
    PT2 pt0, pt1 ;
    REAL d ;
    C2_CURVE segment ;

    if ( !C2_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    i += C2_CURVE_J0(curve) ;
    if ( i > C2_CURVE_J1(curve) ) 
        RETURN ( NULL ) ;
    if ( !c2p_segment_epts_d ( C2_CURVE_PCURVE(curve), i, pt0, pt1, &d ) )
        RETURN ( NULL ) ;
    segment = IS_ZERO ( d ) ? c2d_line ( pt0, pt1 ) : 
        c2d_arc_pts_bulge ( pt0, pt1, d ) ;
    if ( i == C2_CURVE_J0(curve) )
        C2_CURVE_T0(segment) = C2_CURVE_T0(curve) - (INT)C2_CURVE_J0(curve) ;
    if ( i == C2_CURVE_J1(curve) ) {
        C2_CURVE_T1(segment) = C2_CURVE_T1(curve) - (INT)C2_CURVE_J1(curve) ;
        if ( C2_CURVE_IS_LINE(segment) && 
            ( C2_CURVE_T1(segment) < 1.0 - BBS_ZERO ) )
            C2_CURVE_J1(segment) = 1 ;
    }
    RETURN ( segment ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST c2d_pcurve_smash ( curve, curve_list ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
DML_LIST curve_list ;
{
    C2_CURVE crv ;
    INT i ;

    if ( !C2_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    if ( curve_list == NULL ) 
        curve_list = dml_create_list () ;

    for ( i = 0 ; ; i++ ) {
        crv = c2d_pcurve_segment ( curve, i ) ;
        if ( crv == NULL ) 
            RETURN ( curve_list ) ;
        dml_append_data ( curve_list, crv ) ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_pcurve_curves ( curve_list, tol ) 
/*----------------------------------------------------------------------*/
/***** The curves on the curve_list are deleted *************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curve_list ;
REAL tol ;
{
    C2_CURVE curve, pcurve ;
    DML_ITEM item ;
    PT2 p0 ;

    c2c_sort_curve_list ( curve_list, (PF_EVAL2)c2c_ept0, (PF_EVAL2)c2c_ept1, 
        FALSE, curve_list ) ;

    curve = (C2_CURVE) dml_first_record ( curve_list ) ;
    c2c_ept0 ( curve, p0 ) ;
    pcurve = c2d_pcurve_init ( p0 ) ;

    DML_WALK_LIST ( curve_list, item ) {
        curve = (C2_CURVE) DML_RECORD(item) ;
        if ( c2d_pcurve_add_curve ( pcurve, curve, tol ) == NULL ) 
            RETURN ( pcurve ) ;
    }
    RETURN ( pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST c2d_pcurve_list ( curve_list, tol, pcurve_list ) 
/*----------------------------------------------------------------------*/
/***** The curves on the curve_list are deleted *************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curve_list ;
REAL tol ;
DML_LIST pcurve_list ;
{
    C2_CURVE curve, pcurve ;
    DML_ITEM item ;
    PT2 p0 ;

    c2c_sort_curve_list ( curve_list, (PF_EVAL2)c2c_ept0, 
        (PF_EVAL2)c2c_ept1, FALSE, curve_list ) ;
    if ( pcurve_list == NULL ) 
        pcurve_list = dml_create_list () ;

    curve = (C2_CURVE) dml_first_record ( curve_list ) ;
    c2c_ept0 ( curve, p0 ) ;
    pcurve = c2d_pcurve_init ( p0 ) ;

    DML_WALK_LIST ( curve_list, item ) {
        curve = (C2_CURVE) DML_RECORD(item) ;
        if ( c2d_pcurve_add_curve ( pcurve, curve, tol ) == NULL ) {
            dml_append_data ( pcurve_list, pcurve ) ;
            c2c_ept0 ( curve, p0 ) ;
            pcurve = c2d_pcurve_init ( p0 ) ;
            c2d_pcurve_add_curve ( pcurve, curve, tol ) ;
        }
    }
    RETURN ( pcurve_list ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_pcurve_add_curve ( pcurve, curve, tol ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE pcurve, curve ;
REAL tol ;
{
    PT2 pt0, pt1 ;
    REAL d ;

    c2c_get_arc_pts_d ( curve, pt0, pt1, &d ) ;
    RETURN ( c2d_pcurve_add ( pcurve, pt0, pt1, d, tol ) ) ;
}


/*----------------------------------------------------------------------*/
STATIC C2_CURVE c2d_pcurve_add ( curve, pt0, pt1, d, tol ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 pt0, pt1 ;
REAL d, tol ;
{
    if ( !C2_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    if ( c2p_add ( C2_CURVE_PCURVE(curve), pt0, pt1, d, tol ) == NULL )
        RETURN ( NULL ) ;
    C2_CURVE_T1(curve) += 1.0 ;
    C2_CURVE_J1(curve) += 1 ;
    c2p_box_append ( C2_CURVE_PCURVE(curve), C2_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
}
