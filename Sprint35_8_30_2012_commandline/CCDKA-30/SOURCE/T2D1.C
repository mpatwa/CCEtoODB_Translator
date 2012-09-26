/* -R -Z -S -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2D1.C **********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <c2adefs.h> 
#include <c2cdefs.h> 
#include <c2ddefs.h> 
#include <c2vdefs.h> 
#include <c2vmcrs.h> 
#include <dmldefs.h> 
#include <m2cdefs.h> 
#include <t2cdefs.h> 
#include <t2cpriv.h> 
#include <t2ddefs.h> 
#include <t2dpriv.h> 
#include <t2idefs.h> 
#include <t2xmax.h> 

#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC T2_EDGE t2d_append_curve_epts __(( T2_LOOP, C2_CURVE, PT2, PT2, 
            PT2, PT2, INT, REAL )) ;
STATIC T2_EDGE t2d_append_curve_at __(( T2_LOOP, C2_CURVE, INT, 
            T2_VTX_STATUS, BOOLEAN )) ;
STATIC BOOLEAN x_max_sort __(( T2_X_MAX, T2_X_MAX )) ;
STATIC void t2d_form_region __(( DML_LIST, DML_LIST, DML_LIST, BOOLEAN )) ;
STATIC DML_LIST t2d_form_region1 __(( T2_LOOP, BOOLEAN, DML_LIST, DML_LIST )) ;
STATIC DML_LIST t2d_form_region2 __(( T2_LOOP, BOOLEAN, DML_LIST, DML_LIST )) ;
STATIC BOOLEAN t2d_loop_pos __(( DML_LIST, T2_LOOP )) ;
STATIC void t2d_orient_looplist __(( DML_LIST )) ;
STATIC DML_ITEM t2d_clst_curve __(( DML_LIST, PT2, PT2, PT2, PT2, 
        REAL*, INT* )) ;

/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_REGION t2d_build_region ( curvelist, curve0, tol )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Softwar    !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curvelist ;
C2_CURVE curve0 ;
REAL tol ;
{
    DML_LIST looplist ;
    T2_REGION region ;
    T2_LOOP x_loop, loop ;
    DML_ITEM item, item1 ;
    PT2 pt ;

    looplist = t2d_build_looplist ( NULL, curvelist, curve0, FALSE, tol ) ;
    if ( looplist == NULL ) 
        RETURN ( NULL ) ;
    if ( DML_LENGTH(looplist) == 0 ) {
        dml_free_list ( looplist ) ;
        RETURN ( NULL ) ;
    }
    t2d_orient_looplist ( looplist ) ;
    item = dml_first ( looplist ) ;
    x_loop = dml_record ( item ) ;

    for ( item = dml_next(item) ; item != NULL ; item = item1 ) {
        item1 = dml_next(item) ;
        loop = DML_RECORD(item) ;
        t2c_loop_pt ( loop, pt, NULL ) ;
        if ( t2c_pt_pos_loop ( x_loop, pt ) != T2_PT_INSIDE ) {
            t2d_decomp_loop ( loop, curvelist ) ;
            dml_remove_item ( looplist, item ) ;
        }
    }
    region = t2d_create_region ( looplist ) ;
    RETURN ( region ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2d_build_regions ( curvelist, curve0, tol, regionlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Softwar    !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curvelist, regionlist ;
C2_CURVE curve0 ;
REAL tol ;
{
    DML_LIST looplist, outlist ;
    DML_ITEM item ;

    looplist = t2d_build_looplist ( NULL, curvelist, curve0, FALSE, tol ) ;
    if ( looplist == NULL ) 
        RETURN ( NULL ) ;
    DML_WALK_LIST ( looplist, item ) 
        t2c_orient_loop ( DML_RECORD(item), TRUE ) ;
    outlist = t2d_create_regions ( looplist, regionlist ) ;
    dml_free_list ( looplist ) ;
    RETURN ( outlist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2d_build_looplist ( region, curvelist, curve0, 
            clsd_only, tol )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
DML_LIST curvelist ;
C2_CURVE curve0 ;
BOOLEAN clsd_only ;
REAL tol ;
{
    DML_LIST looplist = dml_create_list(), openlist = NULL ;
    T2_LOOP loop ;
    C2_CURVE init = curve0 ;
    DML_ITEM item ;

    if ( looplist == NULL ) 
        RETURN ( NULL ) ;
    m2c_break_curves ( curvelist ) ;
    while ( DML_LENGTH(curvelist) ) {
        loop = t2d_build_loop ( region, curvelist, init, tol ) ; 
        if ( loop != NULL ) {
            if ( T2_LOOP_CLOSED(loop) ) {
                 dml_append_data ( looplist, loop ) ;
            }
            else {
                if ( openlist == NULL ) 
                    openlist = dml_create_list() ;
                dml_append_data ( openlist, loop ) ;
            }
        }
        init = NULL ;
#ifdef __DMG__
        if ( DML_LENGTH ( looplist ) > 5 ) 
            RETURN ( looplist ) ;
#endif
    }

    if ( openlist != NULL ) {
        if ( clsd_only ) {
            DML_WALK_LIST ( openlist, item ) 
                t2d_decomp_loop ( (T2_LOOP)DML_RECORD(item), curvelist ) ;
        }
        else 
            dml_append_list ( looplist, openlist ) ;
        dml_free_list ( openlist ) ;
        openlist = NULL ;
    }
    RETURN ( looplist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2d_decomp_loop ( loop, curvelist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
DML_LIST curvelist ;
{
    DML_ITEM item ;
    T2_EDGE edge ;
    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ;
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = (T2_EDGE)DML_RECORD(item) ;
        dml_append_data ( curvelist, T2_EDGE_CURVE(edge) ) ;
        T2_EDGE_CURVE(edge) = NULL ;
        t2d_free_edge ( edge ) ;
        edge = NULL ;
        DML_RECORD(item) = NULL ;
    }
    t2d_free_loop ( loop ) ;
    loop = NULL ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2d_decomp_region ( region, curvelist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
DML_LIST curvelist ;
{
    DML_ITEM item ;

    if ( region == NULL || T2_REGION_LOOP_LIST(region) == NULL ) 
        RETURN ;
    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) 
        t2d_decomp_loop ( DML_RECORD(item), curvelist ) ;
    t2d_free_region ( region ) ;
    region = NULL ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2d_build_loop ( region, curvelist, curve0, tol )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
DML_LIST curvelist ;
C2_CURVE curve0 ;
REAL tol ;
{
    T2_LOOP loop = t2d_create_loop ( region, dml_create_list() ) ;
    if ( loop != NULL ) {
        t2d_append_curvelist ( loop, curvelist, curve0, tol ) ;
        (void)t2d_adjust_epts_loop ( loop, tol ) ;
    }
    if ( T2_LOOP_EDGE_LIST(loop) == NULL || 
        DML_LENGTH ( T2_LOOP_EDGE_LIST(loop) ) == 0 ) {
        t2d_free_loop ( loop ) ;
        loop = NULL ;
    }
    RETURN ( loop ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2d_loop_pcurve ( region, pcurve )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
C2_CURVE pcurve ;
{
    DML_LIST curvelist ;
    T2_LOOP loop ;

    curvelist = c2d_pcurve_smash ( pcurve, NULL ) ;
    loop = t2d_build_loop ( region, curvelist, NULL, BBS_TOL * 10.0 ) ;
    dml_free_list ( curvelist ) ;
    RETURN ( loop ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2d_orient_looplist ( looplist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
{
    T2_PARM_S *x_max_parm ;
    INT i, i_max, n ;
    T2_LOOP loop ;
    REAL x, x_max = 0.0 ;
    DML_ITEM item, item_max=NULL ;

    n = DML_LENGTH(looplist) ;
    if ( n <= 0 ) 
        RETURN ;
    x_max_parm = CREATE ( n, T2_PARM_S ) ;
    if ( x_max_parm == NULL ) 
        RETURN ;
    i = 0 ;
    DML_WALK_LIST ( looplist, item ) {
        loop = (T2_LOOP)DML_RECORD(item) ;
        x = t2c_loop_x_max ( loop, x_max_parm+i ) ;
        if ( i==0 || x > x_max ) {
            x_max = x ;
            i_max = i ;
        }
        i++ ;
    }

    /* The rest of the loops are assumed to be inside of the exterior loop */
    i = 0 ;
    DML_WALK_LIST ( looplist, item ) {
        loop = (T2_LOOP)DML_RECORD(item) ;
        t2c_orient_loop ( loop, i==i_max ) ;
        if ( i == i_max ) 
            item_max = item ;
        i++ ;
    }

    dml_make_first ( looplist, item_max ) ;

    KILL ( x_max_parm ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2d_append_curvelist ( loop, curvelist, curve0, tol ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
DML_LIST curvelist ;
C2_CURVE curve0 ;
REAL tol ;
{
    PT2 p0, p1, q0, q1 ;
    T2_EDGE edge ;
    DML_ITEM item ;
    INT pos ;
    REAL dist ;

    if ( DML_LENGTH(T2_LOOP_EDGE_LIST(loop)) ) {
        if ( !t2c_loop_ept0 ( loop, p0 ) || !t2c_loop_ept1 ( loop, p1 ) )
            RETURN ( FALSE ) ;
        if ( curve0 != NULL && 
            t2d_append_curve_epts ( loop, curve0, p0, p1, 
                NULL, NULL, -1, tol ) != NULL ) 
                dml_remove_data ( curvelist, curve0 ) ;
    }

    else {
        if ( curve0 == NULL ) {
            curve0 = DML_FIRST_RECORD(curvelist) ;
            if ( curve0 == NULL ) 
                RETURN ( FALSE ) ;
        }
        if ( t2d_append_curve_epts ( loop, curve0, p0, p1, 
            NULL, NULL, -1, tol ) == NULL ) 
            RETURN ( FALSE ) ;
        dml_remove_data ( curvelist, curve0 ) ;
    }

    while ( /* !C2V_IDENT_PTS(p0,p1) */ C2V_DIST(p0,p1) > tol  && 
        DML_LENGTH(curvelist) ) {
        item = t2d_clst_curve ( curvelist, p0, p1, q0, q1, &dist, &pos ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop, 10 ) ;
    if ( item != NULL ) 
        paint_curve ( DML_RECORD(item), 12 ) ;
    getch();
}
else
    DISPLAY-- ;
#endif
        if ( item != NULL && dist <= tol ) {
            edge = t2d_append_curve_epts ( loop, DML_RECORD(item), 
                p0, p1, q0, q1, pos, tol ) ;
            dml_remove_item ( curvelist, item ) ;
            item = NULL ;
        }
        else 
            RETURN ( TRUE ) ;
        if ( edge == NULL ) 
            break ;
#ifdef __DMG__
        if ( DML_LENGTH ( T2_LOOP_EDGE_LIST(loop) ) > 20 ) 
            RETURN ( TRUE ) ;
#endif
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_ITEM t2d_clst_curve ( curvelist, p0, p1, q0, q1, dist_min, pos ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curvelist ;
PT2 p0, p1, q0, q1 ;
REAL *dist_min ;
INT *pos ;
{
    DML_ITEM item, item0 ;
    C2_CURVE curve ;
    PT2 r0, r1 ;
    REAL d, dist ;
    INT pos0 ;

    *dist_min = 0.0 ;

    if ( curvelist == NULL || DML_LENGTH(curvelist) == 0 ) 
        RETURN ( NULL ) ;
    item0 = NULL ;
    DML_WALK_LIST ( curvelist, item ) {
    curve = DML_RECORD(item) ;
    c2c_ept0 ( curve, r0 ) ;
    c2c_ept1 ( curve, r1 ) ;
        dist = C2V_DIST ( p1, r0 ) ;
        pos0 = 10 ;
        d = C2V_DIST ( p1, r1 ) ;
        if ( d < dist ) {
            dist = d ;
            pos0 = 11 ;
        }
        d = C2V_DIST ( p0, r0 ) ;
        if ( d < dist ) {
            dist = d ;
            pos0 = 0 ;
        }
        d = C2V_DIST ( p0, r1 ) ;
        if ( d < dist ) {
            dist = d ;
            pos0 = 1 ;
        }
        if ( item0 == NULL || dist < *dist_min ) {
            item0 = item ;
            *dist_min = dist ;
            C2V_COPY ( r0, q0 ) ;
            C2V_COPY ( r1, q1 ) ;
            *pos = pos0 ;
        }
    }
    RETURN ( item0 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_EDGE t2d_append_curve ( loop, curve, tol ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
C2_CURVE curve ;
REAL tol ;
{
    PT2 p0, p1 ;

    if ( T2_LOOP_EDGE_LIST(loop) == NULL ) 
        T2_LOOP_EDGE_LIST(loop) = dml_create_list();
    RETURN ( ( DML_LENGTH(T2_LOOP_EDGE_LIST(loop)) == 0 || 
            ( t2c_loop_ept0 ( loop, p0 ) && t2c_loop_ept1 ( loop, p1 ) ) ) ?
            t2d_append_curve_epts ( loop, curve, p0, p1, 
                NULL, NULL, -1, tol ) : NULL ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2d_append_curve_epts ( loop, curve, p0, p1, q0, q1, 
            pos, tol ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
C2_CURVE curve ;
PT2 p0, p1, q0, q1 ;    /* p0 and p1 are the endpoints of the loop */
INT pos ;
REAL tol ;
{
    PT2 ept0, ept1 ;
    T2_EDGE edge = NULL ;
    if ( tol < BBS_TOL ) 
        tol = BBS_TOL ;

    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    if ( q0 == NULL ) 
        c2c_ept0 ( curve, ept0 ) ;
    else
        C2V_COPY ( q0, ept0 ) ;
    if ( q1 == NULL ) 
        c2c_ept1 ( curve, ept1 ) ;
    else
        C2V_COPY ( q1, ept1 ) ;

    if ( !DML_LENGTH(T2_LOOP_EDGE_LIST(loop)) ) {
        C2V_COPY ( ept0, p0 ) ;
        C2V_COPY ( ept1, p1 ) ;
        edge = t2d_append_curve_at ( loop, curve, 1, T2_VTX_UNDEF, FALSE ) ;
    }
    else if ( pos == 10 || ( pos < 0 && C2V_DIST ( p1, ept0 ) <= tol ) ) {
        C2V_COPY ( ept1, p1 ) ;
        edge = t2d_append_curve_at ( loop, curve, 1, T2_VTX_UNDEF, TRUE ) ;
    }
    else if ( pos == 11 || ( pos < 0 && C2V_DIST ( p1, ept1 ) <= tol ) ) {
        C2V_COPY ( ept0, p1 ) ;
        edge = t2d_append_curve_at ( loop, curve, -1, T2_VTX_UNDEF, TRUE ) ;
    }
    else if ( pos == 0 || ( pos < 0 && C2V_DIST ( p0, ept0 ) <= tol ) ) {
        C2V_COPY ( ept1, p0 ) ;
        edge = t2d_append_curve_at ( loop, curve, -1, T2_VTX_UNDEF, FALSE ) ;
    }
    else if ( pos == 1 || ( pos < 0 && C2V_DIST ( p0, ept1 ) <= tol ) ) {
        C2V_COPY ( ept0, p0 ) ;
        edge = t2d_append_curve_at ( loop, curve, 1, T2_VTX_UNDEF, FALSE ) ;
    }
    if ( edge != NULL ) 
        T2_LOOP_CLOSED(loop) = ( C2V_DIST ( p0, p1 ) <= tol ) ;

    RETURN ( edge ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2d_append_curve_at ( loop, curve, dir, vtx, after ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
C2_CURVE curve ;
INT dir ;
BOOLEAN after ;
T2_VTX_STATUS vtx ;
{
    T2_EDGE edge = t2d_create_edge ( loop, curve, dir ) ;

    if ( edge == NULL ) 
        RETURN ( NULL ) ;
    T2_EDGE_VTX(edge) = vtx ;
    if ( DML_LENGTH(T2_LOOP_EDGE_LIST(loop)) == 0 ) 
        c2a_box_copy ( C2_CURVE_BOX(curve), T2_LOOP_BOX(loop) ) ;
    else
        c2a_box_append ( T2_LOOP_BOX(loop), C2_CURVE_BOX(curve) ) ;

    if ( after )
        dml_insert_prior ( T2_LOOP_EDGE_LIST(loop), NULL, edge ) ;
    else
        dml_insert_after ( T2_LOOP_EDGE_LIST(loop), NULL, edge ) ;
    t2c_loop_closed ( loop ) ;
/*
    after means insert after the last item == prior NULL, and vice versa */

    RETURN ( edge ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_ITEM t2d_append_edge ( loop, edge, pos_first ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_EDGE edge ;
BOOLEAN pos_first ;
{
    DML_ITEM item ;
    if ( DML_LENGTH(T2_LOOP_EDGE_LIST(loop)) == 0 ) 
        c2a_box_copy ( T2_EDGE_BOX(edge), T2_LOOP_BOX(loop) ) ;
    else
        c2a_box_append ( T2_LOOP_BOX(loop), T2_EDGE_BOX(edge) ) ;
    T2_EDGE_LOOP(edge) = loop ;
    item = dml_insert ( T2_LOOP_EDGE_LIST(loop), edge, pos_first ) ;
    if ( item != NULL ) 
        t2c_loop_closed ( loop ) ;
    RETURN ( item ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_ITEM t2d_insert_edge ( loop, item, edge, insert_prior )   
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
DML_ITEM item ;
T2_EDGE edge ;
BOOLEAN insert_prior ;
{
    if ( DML_LENGTH(T2_LOOP_EDGE_LIST(loop)) == 0 ) 
        c2a_box_copy ( T2_EDGE_BOX(edge), T2_LOOP_BOX(loop) ) ;
    else
        c2a_box_append ( T2_LOOP_BOX(loop), T2_EDGE_BOX(edge) ) ;

    T2_EDGE_LOOP(edge) = loop ;

    if ( insert_prior == TRUE )
        item = dml_insert_prior ( T2_LOOP_EDGE_LIST(loop), item, edge ) ;
    else
        item = dml_insert_after ( T2_LOOP_EDGE_LIST(loop), item, edge ) ;

    if ( item != NULL ) 
        t2c_loop_closed ( loop ) ;

    RETURN ( item ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2d_create_regions ( looplist, regionlist ) 
/* This routine creates regions out of a bunch of oriented loops. Loops used 
for construction are removed from the looplist, so the remaining loops could 
not be used in the construction. The loops are supposed to have no 
intersections */
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist, regionlist ;
{
    RETURN ( t2d_create_regions_ort ( looplist, regionlist, FALSE ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2d_create_regions_ort ( looplist, regionlist, ortn ) 
/* This routine creates regions out of a bunch of oriented loops. Loops used 
for construction are removed from the looplist, so the remaining loops could 
not be used in the construction. The loops are supposed to have no 
intersections */
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist, regionlist ;
BOOLEAN ortn ;
{
    DML_LIST x_max_list ;

    if ( regionlist == NULL ) 
        regionlist = dml_create_list () ;
    x_max_list = t2d_create_x_max_list ( looplist ) ;

    while ( DML_LENGTH(x_max_list) ) 
        t2d_form_region ( looplist, x_max_list, regionlist, ortn ) ;
    dml_free_list ( x_max_list ) ;
    x_max_list = NULL ;
    RETURN ( regionlist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2d_create_x_max_list ( looplist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
{
    T2_X_MAX x_max ;
    DML_LIST x_max_list ;
    DML_ITEM item ;
    T2_LOOP loop ;

    x_max_list = dml_create_list () ;

    DML_WALK_LIST ( looplist, item ) {
        loop = (T2_LOOP)DML_RECORD(item) ;
        if ( t2c_loop_closed ( loop ) ) {
            x_max = MALLOC ( 1, T2_X_MAX_S ) ;
            T2_X_MAX_VALUE(x_max) = 
                t2c_loop_x_max ( loop, T2_X_MAX_PARM(x_max) ) ;
            T2_X_MAX_ORTN(x_max) = 
                ( t2c_orientation ( loop ) == T2_DIR_CCW ) ; 
            dml_append_data ( x_max_list, x_max ) ;
        }
    }
    dml_sort_list ( x_max_list, ( PF_SORT ) x_max_sort ) ;
    RETURN ( x_max_list ) ;
}

/*-------------------------------------------------------------------------*/
STATIC BOOLEAN x_max_sort ( x_max1, x_max2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_X_MAX x_max1, x_max2 ;
{
    REAL s1, s2 ;
    if ( IS_SMALL ( T2_X_MAX_VALUE(x_max1) - T2_X_MAX_VALUE(x_max2) ) ) {
/*
        RETURN ( T2_X_MAX_ORTN(x_max1) ) ;
*/ 
        s1 = t2c_loop_area ( T2_X_MAX_LOOP(x_max1) ) ;
        s2 = t2c_loop_area ( T2_X_MAX_LOOP(x_max2) ) ;
        if ( IS_SMALL ( fabs(s1) - fabs(s2) ) )
            RETURN ( T2_X_MAX_ORTN(x_max1) ) ;
        else 
/*
            RETURN ( fabs(s1) > fabs(s2) ) ;
*/
            RETURN ( s1 > s2 ) ;
    }
    else 
        RETURN ( T2_X_MAX_VALUE(x_max1) > T2_X_MAX_VALUE(x_max2) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2d_form_region ( inlooplist, x_max_list, regionlist, 
        ortn_correct ) 
/* This routine tries to form a region out of a bunch of oriented loops. 
Loops used for construction are removed from the inlooplist, so the remaining 
loops could not be used in the construction. The loops are supposed to have 
no intersections */
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST inlooplist, x_max_list, regionlist ;
BOOLEAN ortn_correct ;
{
    T2_LOOP loop0 ;
    T2_X_MAX x_max ; 
    DML_ITEM item ;
    DML_LIST looplist ;
    BOOLEAN orntn ;

    x_max = dml_first_record ( x_max_list ) ;
    item = dml_find_data ( inlooplist, T2_X_MAX_LOOP(x_max) ) ;
    loop0 = dml_record ( item ) ;

    orntn = T2_X_MAX_ORTN(x_max) ;
    FREE ( x_max ) ;
    x_max = NULL ;
    dml_remove_first ( x_max_list ) ;
    dml_remove_item ( inlooplist, item ) ;
    item = NULL ;
    looplist = ortn_correct ?
        t2d_form_region1 ( loop0, orntn, inlooplist, x_max_list ) :
        t2d_form_region2 ( loop0, orntn, inlooplist, x_max_list ) ;
    if ( looplist != NULL ) 
        dml_append_data ( regionlist, t2d_create_region ( looplist ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_LIST t2d_form_region1 ( loop0, orntn0, inlooplist, x_max_list ) 
/* This routine tries to form a region out of a bunch of oriented loops. 
Loops used for construction are removed from the inlooplist, so the remaining 
loops could not be used in the construction. The loops are supposed to have 
no intersections and are oriented correctly */
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0 ;
BOOLEAN orntn0 ;
DML_LIST inlooplist, x_max_list ;
{
    T2_X_MAX x_max ; 
    DML_ITEM item, next ;
    DML_LIST looplist ;
    PT2 pt ;
    T2_PT_POSITION pt_pos ;

    if ( !orntn0 ) {
/*
        for ( item = dml_first(x_max_list), next = dml_next(item) ; 
            item != NULL ; item = next, next = dml_next(next) ) {

            x_max = dml_record(item) ;
            if ( T2_X_MAX_VALUE(x_max) < T2_LOOP_MIN_X(loop0) ) 
                break ;

            if ( T2_X_MAX_ORTN(x_max) && 
                t2c_loop_ept0 ( T2_X_MAX_LOOP(x_max), pt ) 
                && ( t2c_pt_pos_loop ( loop0, pt ) == T2_PT_INSIDE ) ) {
                dml_remove_data ( inlooplist, T2_X_MAX_LOOP(x_max) ) ;
                t2d_free_loop ( T2_X_MAX_LOOP(x_max) ) ;
                FREE ( x_max ) ;
                x_max = NULL ; 
                dml_remove_item ( x_max_list, item ) ; 
                item = NULL ;
            } 
        }
*/
        t2d_free_loop ( loop0 ) ;
        loop0 = NULL ;
        RETURN ( NULL ) ;
    }

    looplist = dml_create_list () ;
    dml_append_data ( looplist, loop0 ) ;

    for ( item = dml_first(x_max_list), next = dml_next(item) ; 
        item != NULL ; item = next, next = dml_next(next) ) {

        x_max = DML_RECORD(item) ;
        if ( T2_X_MAX_VALUE(x_max) >= T2_LOOP_MIN_X(loop0) &&
            !T2_X_MAX_ORTN(x_max) ) {
            t2c_loop_ept0 ( T2_X_MAX_LOOP(x_max), pt ) ;
            pt_pos = t2c_pt_pos_loop ( loop0, pt ) ; 
            if ( pt_pos == T2_PT_INSIDE || pt_pos == T2_PT_ON_BOUNDARY ) {
/*
                t2c_orient_loop ( T2_X_MAX_LOOP(x_max), FALSE ) ; 
*/
                dml_append_data ( looplist, T2_X_MAX_LOOP(x_max) ) ;
                dml_remove_data ( inlooplist, T2_X_MAX_LOOP(x_max) ) ;
                FREE ( x_max ) ;
                x_max = NULL ; 
                dml_remove_item ( x_max_list, item ) ; 
                item = NULL ;
            } 
        }
    }
    RETURN ( looplist ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_LIST t2d_form_region2 ( loop0, orntn0, inlooplist, x_max_list )
/* This routine tries to form a region out of a bunch of oriented loops. 
Loops used for construction are removed from the inlooplist, so the remaining 
loops could not be used in the construction. The loops are supposed to have 
no intersections */
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0 ;
BOOLEAN orntn0 ;
DML_LIST inlooplist, x_max_list ;
{
    T2_X_MAX x_max ; 
    DML_ITEM item, next ;
    DML_LIST looplist ;
    PT2 pt ;
    T2_PT_POSITION pt_pos ;

    if ( !orntn0 ) 
        t2c_orient_loop ( loop0, TRUE ) ;

    looplist = dml_create_list () ;
    dml_append_data ( looplist, loop0 ) ;
    DML_WALK_LIST ( x_max_list, item ) {
        x_max = DML_RECORD(item) ;
        T2_X_MAX_POS(x_max) = 
            t2d_loop_pos ( x_max_list, T2_X_MAX_LOOP(x_max) ) ;
/* POS is TRUE, if it is not inside of another loop on the list, except 
for the first exterior one */
    }
    for ( item = dml_first(x_max_list), next = dml_next(item) ; 
        item != NULL ; item = next, next = dml_next(next) ) {
        x_max = DML_RECORD(item) ;
        if ( T2_X_MAX_VALUE(x_max) >= T2_LOOP_MIN_X(loop0) && 
            T2_X_MAX_POS(x_max) ) {
            t2c_loop_ept0 ( T2_X_MAX_LOOP(x_max), pt ) ;
            pt_pos = t2c_pt_pos_loop ( loop0, pt ) ; 
            if ( pt_pos == T2_PT_INSIDE || pt_pos == T2_PT_ON_BOUNDARY ) {
                t2c_orient_loop ( T2_X_MAX_LOOP(x_max), FALSE ) ; 
                dml_append_data ( looplist, T2_X_MAX_LOOP(x_max) ) ;
                dml_remove_data ( inlooplist, T2_X_MAX_LOOP(x_max) ) ;
                FREE ( x_max ) ;
                x_max = NULL ; 
                dml_remove_item ( x_max_list, item ) ; 
                item = NULL ;
            } 
        }
    }
    RETURN ( looplist ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2d_loop_pos ( x_max_list, loop0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* Returns FALSE, if this loop is inside of another loop on the list */
DML_LIST x_max_list ;
T2_LOOP loop0 ;
{
    DML_ITEM item ;
    PT2 p ;
    T2_PT_POSITION pt_pos ;
    T2_LOOP loop ;
    T2_X_MAX x_max ;

    t2c_loop_ept0 ( loop0, p ) ;

    DML_WALK_LIST ( x_max_list, item ) {
        x_max = DML_RECORD(item) ;
        loop = T2_X_MAX_LOOP(x_max) ;
        if ( loop != loop0 ) {
            pt_pos = t2c_pt_pos_loop ( loop, p ) ; 
            if ( ( pt_pos == T2_PT_INSIDE && T2_X_MAX_ORTN(x_max) ) ||
                 ( pt_pos == T2_PT_OUTSIDE && !T2_X_MAX_ORTN(x_max) ) )
                RETURN ( FALSE ) ;
        }
    }
    RETURN ( TRUE ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

