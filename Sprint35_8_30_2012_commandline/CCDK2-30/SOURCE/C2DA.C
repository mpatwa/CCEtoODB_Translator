/* -2 -3 */
/******************************* C2DA.C *********************************/
/********** Two-dimensional curve construction routines *****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2gdefs.h>
#include <c2zdefs.h>
#include <c2adefs.h>
#include <c2ddefs.h>
#include <c2dpriv.h>
#include <c2mem.h>
#include <c2vmcrs.h>

STATIC  C2_CURVE    create_arc0 __(( void )) ;
STATIC  void        create_arc1 __(( C2_CURVE* DUMMY0 )) ;

/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_arc ( ctr, rad, st_angle, sweep, dir ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr ;
REAL rad, st_angle, sweep ;
INT dir ;
{
    C2_CURVE curve ;

    curve = create_arc0() ;
    if ( curve != NULL ) {
        C2_CURVE_ARC(curve) = c2g_create_arc ( ctr, rad, st_angle, 
            sweep, dir, &(C2_CURVE_T1(curve)) ) ;
        create_arc1 ( &curve ) ;
    }
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_arc_pts_bulge ( pt0, pt1, bulge )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1 ;
REAL bulge ;
{
    C2_CURVE curve ;

    curve = create_arc0() ;
    if ( curve != NULL ) {
        C2_CURVE_ARC(curve) = c2g_alloc() ;
        if ( C2_CURVE_ARC(curve) == NULL ) 
            RETURN ( NULL ) ;
        c2g_arc_pts_bulge ( pt0, pt1, bulge, C2_CURVE_ARC(curve) ) ;
        C2_CURVE_T1(curve) = 1.0 ;
        create_arc1 ( &curve ) ;
    }
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_arc_3pts ( pt0, pt1, pt2 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1, pt2 ;
{
    C2_CURVE curve ;

    curve = create_arc0() ;
    if ( curve != NULL ) {
        C2_CURVE_ARC(curve) = c2g_create_3pts ( pt0, pt1, pt2 ) ;
        C2_CURVE_T1(curve) = 1.0 ;
        create_arc1 ( &curve ) ;
    }
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_arc_2pts_tan ( pt0, tan0, pt1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1, tan0 ;
{
    C2_CURVE curve ;

    curve = create_arc0() ;
    if ( curve != NULL ) {
        C2_CURVE_ARC(curve) = c2g_2pts_tan ( pt0, tan0, pt1 ) ;
        if ( C2_CURVE_ARC(curve) == NULL ) {
            c2d_free_curve ( curve ) ;
            RETURN ( NULL ) ;
        }
        C2_CURVE_T1(curve) = 1.0 ;
        create_arc1 ( &curve ) ;
    }
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_arc_2pts_tan1 ( pt0, pt1, tan1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1, tan1 ;
{
    C2_CURVE curve ;

    curve = create_arc0() ;
    if ( curve != NULL ) {
        C2_CURVE_ARC(curve) = c2g_2pts_tan1 ( pt0, pt1, tan1 ) ;
        if ( C2_CURVE_ARC(curve) == NULL ) {
            c2d_free_curve ( curve ) ;
            RETURN ( NULL ) ;
        }
        C2_CURVE_T1(curve) = 1.0 ;
        create_arc1 ( &curve ) ;
    }
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_CURVE c2d_arc_2pts_2tans ( pt0, tan0, pt1, tan1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1, tan0, tan1 ;
{
    C2_CURVE curve ;

    curve = create_arc0() ;
    if ( curve != NULL ) {
        C2_CURVE_ARC(curve) = c2g_2pts_2tans ( pt0, tan0, pt1, tan1 ) ;
        if ( C2_CURVE_ARC(curve) == NULL ) {
            c2d_free_curve ( curve ) ;
            RETURN ( NULL ) ;
        }
        C2_CURVE_T1(curve) = 1.0 ;
        create_arc1 ( &curve ) ;
    }
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_arc_ctr_2pts ( ctr, pt0, pt1, dir ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr, pt0, pt1 ;
INT dir ;
{
    C2_CURVE curve ;

    if ( C2V_IDENT_PTS ( pt0, pt1 ) ) 
        RETURN ( c2d_circle_ctr_pt ( ctr, pt0 ) ) ;
    curve = create_arc0() ;
    if ( curve != NULL ) {
        C2_CURVE_ARC(curve) = c2g_create_ctr_2pts ( ctr, pt0, pt1, dir ) ;
        C2_CURVE_T1(curve) = 1.0 ;
        create_arc1 ( &curve ) ;
    }
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_arc_complement ( curve0 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve0 ;
{
    C2_CURVE curve ;

    if ( !C2_CURVE_IS_ARC(curve0) )
        RETURN ( NULL ) ;
    curve = create_arc0() ;
    if ( curve != NULL ) {
        C2_CURVE_ARC(curve) = c2g_alloc() ;
        if ( C2_CURVE_ARC(curve) == NULL ) 
            RETURN ( NULL ) ;
        c2g_complement ( C2_CURVE_ARC(curve0), C2_CURVE_ARC(curve) ) ;
        C2_CURVE_T1(curve) = 1.0 ;
        create_arc1 ( &curve ) ;
    }
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_arc_ctr_2tans ( ctr, rad, tan0, tan1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr, tan0, tan1 ;
REAL rad ;
{
    C2_CURVE curve ;

    curve = create_arc0() ;
    if ( curve != NULL ) {
        C2_CURVE_ARC(curve) = c2g_create_ctr_2tans ( ctr, rad, tan0, tan1 ) ;
        C2_CURVE_T1(curve) = 1.0 ;
        create_arc1 ( &curve ) ;
    }
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_CURVE c2d_arc_zero_rad ( p0, p1, tan0, tan1, w ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 p0, p1, tan0, tan1 ;
REAL w ;
{
    C2_CURVE curve ;

    curve = create_arc0() ;
    if ( curve != NULL ) {
        C2_CURVE_ARC(curve) = c2g_create_zero_rad ( p0, p1, tan0, tan1, w ) ;
        C2_CURVE_T1(curve) = 1.0 ;
        create_arc1 ( &curve ) ;
    }
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_CURVE c2d_zero_arc ( ctr, tan0, tan1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr, tan0, tan1 ;
{
    C2_CURVE curve ;

    curve = c2d_curve() ;
    if ( curve == NULL ) 
        RETURN ( curve ) ;
    C2_CURVE_TYPE(curve) = C2_ZERO_ARC_TYPE ;
    C2_CURVE_ZERO_ARC(curve) = c2z_create ( ctr, tan0, tan1 ) ;
    C2_CURVE_T0(curve) = 0.0 ;
    C2_CURVE_J0(curve) = 0 ;
    C2_CURVE_T1(curve) = 1.0 ;
    C2_CURVE_J1(curve) = 0 ;
    c2a_box_init_pt ( C2_CURVE_BOX(curve), ctr ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_circle ( ctr, rad ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr ;
REAL rad ;
{
    C2_CURVE curve ;

    curve = create_arc0() ;
    if ( curve != NULL ) {
        C2_CURVE_ARC(curve) = c2g_create_circle ( ctr, rad ) ;
        C2_CURVE_T1(curve) = 2.0 ;
        create_arc1 ( &curve ) ;
    }
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_circle_ctr_pt ( ctr, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr, pt ;
{
    C2_CURVE curve ;
    curve = create_arc0() ;
    if ( curve != NULL ) {
        C2_CURVE_ARC(curve) = c2g_create_circle_ctr_pt ( ctr, pt ) ;
        C2_CURVE_T1(curve) = 2.0 ;
        create_arc1 ( &curve ) ;
    }
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_circle_ctr_pt_dir ( ctr, pt, dir ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr, pt ;
BOOLEAN dir ;
{
    C2_CURVE curve ;
    curve = create_arc0() ;
    if ( curve != NULL ) {
        C2_CURVE_ARC(curve) = c2g_create_circle_ctr_pt ( ctr, pt ) ;
        C2_CURVE_T1(curve) = 2.0 ;
        if ( dir != 1 ) 
            c2g_negate_d ( C2_CURVE_ARC(curve) ) ;
        create_arc1 ( &curve ) ;
    }
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
STATIC C2_CURVE create_arc0 () 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    C2_CURVE curve ;

    curve = c2d_curve() ;
    if ( curve != NULL ) 
        C2_CURVE_TYPE(curve) = C2_ARC_TYPE ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
STATIC void create_arc1 ( curve_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE *curve_ptr ;
{
    if ( C2_CURVE_ARC(*curve_ptr) == NULL ) {
        c2d_free_curve ( *curve_ptr ) ;
        *curve_ptr = NULL ;
    }
    else {
        C2_CURVE_T0(*curve_ptr) = 0.0 ;
        C2_CURVE_J0(*curve_ptr) = 0 ;
        C2_CURVE_J1(*curve_ptr) = 
            C2_CURVE_T1(*curve_ptr) > 1.0 + BBS_ZERO ? 1 : 0 ;
        c2g_box ( C2_CURVE_ARC(*curve_ptr), C2_CURVE_T0(*curve_ptr), 
            C2_CURVE_T1(*curve_ptr), C2_CURVE_BOX(*curve_ptr) ) ;
    }
}

