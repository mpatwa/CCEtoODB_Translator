/* -R -Z -S -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2D2.C **********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h>
#include <c2cdefs.h>
#include <c2ddefs.h>
#include <c2vdefs.h>
#include <m2cdefs.h>
#include <t2cdefs.h>
#include <t2cpriv.h>
#include <t2ddefs.h>
#include <t2dpriv.h>
#include <t2attrd.h>
#include <c2vmcrs.h>
#ifdef CCDK_DEBUG
#include <dxfdefs.h>
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC T2_LOOP t2d_close_loop_hor __(( DML_LIST, REAL )) ;
STATIC void t2d_remove_small_edgelist __(( DML_LIST, REAL )) ;
STATIC BOOLEAN t2d_remove_edge __(( T2_EDGE, REAL )) ;
STATIC REAL t2d_adjust_epts_edgelist __(( DML_LIST, REAL )) ;

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2d_create_edgelist ( curvelist, tol ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curvelist ;
REAL tol ;
{
    DML_LIST templist, edgelist ;
    DML_ITEM item ;
    C2_CURVE curve ;
    T2_EDGE edge ;

    if ( ( curvelist == NULL ) || ( DML_LENGTH(curvelist) == 0 ) )
        RETURN ( NULL ) ;
    templist = dml_create_list() ;
    edgelist = dml_create_list() ;
    if ( templist == NULL || edgelist == NULL ) 
        RETURN ( NULL ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    DML_ITEM item ;
    DML_WALK_LIST ( curvelist, item ) {
        paint_curve ( DML_RECORD(item), 9 ) ;
        getch ();
    }
}
else
    DISPLAY-- ;
#endif
    m2c_break_curves ( curvelist ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    DML_ITEM item ;
    DML_WALK_LIST ( curvelist, item ) {
        paint_curve ( DML_RECORD(item), 10 ) ;
        getch ();
    }
}
else
    DISPLAY-- ;
#endif
    DML_WALK_LIST ( curvelist, item )
    {
        curve = ( C2_CURVE ) dml_record ( item ) ;
        edge = t2d_create_edge ( NULL, curve, 1 ) ;
        if ( edge == NULL || dml_append_data ( templist, edge ) == NULL ) 
            RETURN ( NULL ) ;
    }
    t2d_sort_edgelist ( templist, tol, edgelist ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    DML_ITEM item ;
    DML_WALK_LIST ( edgelist, item ) {
        paint_edge ( DML_RECORD(item), 11 ) ;
        getch ();
    }
}
else
    DISPLAY-- ;
#endif
    t2d_adjust_epts_edgelist ( edgelist, tol ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    DML_ITEM item ;
    DML_WALK_LIST ( edgelist, item ) {
        paint_edge ( DML_RECORD(item), 12 ) ;
        getch ();
    }
}
else
    DISPLAY-- ;
#endif
    dml_destroy_list ( templist, (PF_ACTION) t2d_free_edge ) ;
    templist = NULL ;
    RETURN ( edgelist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2d_sort_edgelist ( inlist, tol, outlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST inlist, outlist ;
REAL tol ;
{
    DML_ITEM item, item0 ;
    PT2 q0, q1, r0, r1, p0, p1 ;
    T2_EDGE edge ;
    REAL d, dist, dist_min=0.0 ;
    INT pos0, pos ;

    if ( tol < BBS_TOL ) 
        tol = BBS_TOL ;
    item = dml_first ( inlist ) ;

    if ( item != NULL ) {
        edge = ( T2_EDGE ) dml_record ( item ) ;
        t2c_ept0 ( edge, p0 ) ;
        t2c_ept1 ( edge, p1 ) ;
        dml_append_data ( outlist, ( ANY ) edge ) ;
        dml_remove_item ( inlist, item ) ;
        item = NULL ;
    }

    while ( dml_length ( inlist ) > 0 ) {

        item0 = NULL ;
        DML_WALK_LIST ( inlist, item ) {
            edge = ( T2_EDGE ) dml_record ( item ) ;
            t2c_ept0 ( edge, r0 ) ;
            t2c_ept1 ( edge, r1 ) ;
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
            if ( item0 == NULL || dist < dist_min ) {
                item0 = item ;
                dist_min = dist ;
                C2V_COPY ( r0, q0 ) ;
                C2V_COPY ( r1, q1 ) ;
                pos = pos0 ;
            }
        }
        
        if ( item0 == NULL || dist_min > tol )
            RETURN ( FALSE ) ;
        edge = DML_RECORD(item0) ;
        if ( pos == 0 ) {
            T2_EDGE_DIR(edge) = -T2_EDGE_DIR(edge) ;
            dml_insert ( outlist, edge, TRUE ) ;
            C2V_COPY ( q1, p0 ) ;
        }
        else if ( pos == 10 ) {
            dml_insert ( outlist, edge, FALSE ) ;
            C2V_COPY ( q1, p1 ) ;
        }
        else if ( pos == 1 ) {
            dml_insert ( outlist, edge, TRUE ) ;
            C2V_COPY ( q0, p0 ) ;
        }
        else if ( pos == 11 ) {
            T2_EDGE_DIR(edge) = -T2_EDGE_DIR(edge) ;
            dml_insert ( outlist, edge, FALSE ) ;
            C2V_COPY ( q0, p1 ) ;
        }
        dml_remove_item ( inlist, item0 ) ;
    }

    RETURN ( dml_length ( inlist ) == 0 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL t2d_adjust_epts_region ( region, tol )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
REAL tol ;
{
    DML_ITEM item ;
    REAL loop_gap, gap = 0.0 ;

    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) {
        loop_gap = t2d_adjust_epts_loop ( DML_RECORD(item), tol ) ;
        if ( loop_gap > gap ) 
            gap = loop_gap ;
    }
    RETURN ( gap ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL t2d_adjust_epts_loop ( loop, tol )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL tol ;
{
    REAL gap ;
    if ( T2_LOOP_ATTR(loop) & T2_ATTR_ADJUSTED ) 
        RETURN ( 0.0 ) ;
    gap = ( loop == NULL ) ? 0.0 : 
        t2d_adjust_epts_edgelist ( T2_LOOP_EDGE_LIST(loop), tol ) ;
    T2_LOOP_ATTR(loop) = T2_LOOP_ATTR(loop) | T2_ATTR_ADJUSTED ;
    RETURN ( gap ) ;
}


/*-------------------------------------------------------------------------*/
STATIC REAL t2d_adjust_epts_edgelist ( edgelist, tol )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST edgelist ;
REAL tol ;
{
    DML_ITEM item0, item1 ;
    T2_EDGE edge0, edge1 ;
    REAL gap0, gap = 0.0 ;
    PT2 p0, p1 ;
#ifdef CCDK_DEBUG
    REAL dist, gap1 = 0.0 ;
#endif
    if ( edgelist == NULL ) 
        RETURN ( 0.0 ) ;
    t2d_remove_small_edgelist ( edgelist, BBS_TOL ) ;
    for ( item0 = DML_FIRST(edgelist), item1 = dml_next(item0) ;
        item1 != NULL && item0 != NULL ; 
        item0 = item1, item1 = dml_next(item1) ) {
        edge0 = DML_RECORD(item0) ;
        edge1 = DML_RECORD(item1) ;
        gap0 = t2d_adjust_epts ( edge0, edge1 ) ;
        if ( gap0 > gap ) 
            gap = gap0 ;
#ifdef CCDK_DEBUG
        t2c_ept1 ( edge0, p0 ) ;
        t2c_ept0 ( edge1, p1 ) ;
        dist = C2V_DIST ( p0, p1 ) ;
        if ( dist > gap1 )
            gap1 = dist ;
#endif
    }

    edge0 = dml_last_record ( edgelist ) ;
    edge1 = dml_first_record ( edgelist ) ;
    if ( edge0 != edge1 ) {
        t2c_ept1 ( edge0, p0 ) ;
        t2c_ept0 ( edge1, p1 ) ;
        if ( C2V_DIST ( p0, p1 ) <= tol ) {
            gap0 = t2d_adjust_epts ( edge0, edge1 ) ;
            if ( gap0 > gap ) 
                gap = gap0 ;
#ifdef CCDK_DEBUG
            t2c_ept1 ( edge0, p0 ) ;
            t2c_ept0 ( edge1, p1 ) ;
            dist = C2V_DIST ( p0, p1 ) ;
            if ( dist > gap1 )
                gap1 = dist ;
#endif
        }
    }
    RETURN ( gap ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL t2d_adjust_epts ( edge0, edge1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge0, edge1 ;
{
    PT2 p0, p1 ;
    REAL gap ;
    t2c_ept1 ( edge0, p0 ) ;
    t2c_ept0 ( edge1, p1 ) ;
    gap = C2V_DIST ( p0, p1 ) ;
    m2c_adjust ( T2_EDGE_CURVE(edge0), T2_EDGE_DIR(edge0), 
        T2_EDGE_CURVE(edge1), T2_EDGE_DIR(edge1) ) ;
    RETURN ( gap ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2d_set_ept0 ( edge, pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 pt ;
{
    if ( T2_EDGE_DIR(edge) == 1 ) 
        m2c_set_ept0 ( T2_EDGE_CURVE(edge), pt ) ;
    else
        m2c_set_ept1 ( T2_EDGE_CURVE(edge), pt ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2d_set_ept1 ( edge, pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 pt ;
{
    if ( T2_EDGE_DIR(edge) == 1 ) 
        m2c_set_ept1 ( T2_EDGE_CURVE(edge), pt ) ;
    else
        m2c_set_ept0 ( T2_EDGE_CURVE(edge), pt ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2d_remove_small_edgelist ( edgelist, tol ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST edgelist ;
REAL tol ;
{
    DML_ITEM item, item1 ;

    if ( edgelist == NULL || DML_LENGTH(edgelist) < 2 )
        RETURN ;

    for ( item = dml_first(edgelist) ; item != NULL ; item = item1 ) {
        item1 = DML_NEXT(item) ;
        if ( t2d_remove_edge ( dml_record(item), tol ) ) {
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( dml_record(item), 14 ) ;
    getch ();
}
else
    DISPLAY-- ;
#endif 
            t2d_free_edge ( dml_record(item) ) ;
            dml_remove_item ( edgelist, item ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2d_remove_edge ( edge, tol ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
REAL tol ;
{
    RETURN ( m2c_remove_curve ( T2_EDGE_CURVE(edge), tol ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2d_close_hor_loop ( loop0, y_min ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0 ;
REAL *y_min ;
{
    PT2 p, p0 ;
    REAL y ;
    T2_LOOP loop1, loop ;
    T2_REGION region ;
    DML_LIST region_list = dml_create_list () ;
    DML_ITEM item ;

    if ( y_min == NULL ) 
        y_min = &y ;
    loop1 = t2d_copy_loop ( loop0, NULL, NULL ) ;
    if ( t2c_loop_closed ( loop1 ) ) {
        t2c_orient_loop ( loop1, TRUE ) ;
        region = t2d_loop_to_region ( loop1 ) ;
        dml_append_data ( region_list, region ) ;
        RETURN ( region_list ) ;
    }

    t2c_break_loop_at_extrs ( loop1, 1 ) ;
    t2c_loop_ept0 ( loop1, p0 ) ;
    *y_min = p0[1] ;
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop1), item ) {
        t2c_ept1 ( DML_RECORD(item), p ) ;
        if ( p[1] < *y_min ) 
            *y_min = p[1] ;
    }
    
    while ( DML_LENGTH ( T2_LOOP_EDGE_LIST(loop1) ) ) {
        loop = t2d_close_loop_hor ( T2_LOOP_EDGE_LIST(loop1), *y_min ) ;
        region = t2d_loop_to_region ( loop ) ;
        dml_append_data ( region_list, region ) ;
    }
    t2d_free_loop ( loop1 ) ;
    RETURN ( region_list ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_LOOP t2d_close_loop_hor ( inlist, y_min ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST inlist ;
REAL y_min ;
{
    DML_LIST edgelist = dml_create_list () ;
    BOOLEAN end_loop ;
    T2_LOOP loop ;
    PT2 p, p0, q0, q1 ;
    C2_CURVE curve ;
    DML_ITEM item, item1 ;
    T2_EDGE edge ;

    edge = DML_FIRST_RECORD ( inlist ) ;
    t2c_ept0 ( edge, p0 ) ;

    C2V_SET ( p0[0], y_min, q0 ) ;
    if ( !IS_SMALL ( p0[1] - y_min ) ) {
        curve = c2d_line ( q0, p0 ) ;
        edge = t2d_create_edge ( NULL, curve, 1 ) ;
        T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_ATTR_AUX ;
        dml_append_data ( edgelist, edge ) ;
    }
    end_loop = FALSE ;
    for ( item = DML_FIRST(inlist) ; item!=NULL && !end_loop ; item=item1 ) {
        item1 = DML_NEXT(item) ;
        edge = DML_RECORD(item) ;
        t2c_ept1 ( edge, p ) ;
        end_loop = ( IS_SMALL(p[1]-y_min) ) ;
        dml_append_data ( edgelist, edge ) ;
        dml_remove_item ( inlist, item ) ;
    }

    C2V_SET ( p[0], y_min, q1 ) ;
    if ( !IS_SMALL ( p[1] - y_min ) ) {
        curve = c2d_line ( p, q1 ) ;
        edge = t2d_create_edge ( NULL, curve, 1 ) ;
        T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_ATTR_AUX ;
        dml_append_data ( edgelist, edge ) ;
    }
    if ( !IS_SMALL ( q0[0] - q1[0] ) ) {
        curve = c2d_line ( q1, q0 ) ;
        edge = t2d_create_edge ( NULL, curve, 1 ) ;
        T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_ATTR_AUX ;
        T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_ATTR_HORLINE ;
        dml_append_data ( edgelist, edge ) ;
    }
    loop = t2d_create_loop ( NULL, edgelist ) ;
    t2c_loop_closed ( loop ) ;
    t2c_orient_loop ( loop, TRUE ) ;
    RETURN ( loop ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_LOOP t2d_open_loop ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    T2_EDGE edge, edge0, edge1 ;

    edge = t2c_edge_by_attr ( loop, T2_ATTR_HORLINE ) ;
    if ( edge == NULL ) 
        RETURN ( NULL ) ;
    edge0 = t2c_prev_edge ( edge ) ;
    edge1 = t2c_next_edge ( edge ) ;
    RETURN ( t2d_copy_loop ( loop, edge1, edge0 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_LOOP t2d_close_loop ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    T2_LOOP new ;
    new = t2d_copy_loop ( loop, NULL, NULL ) ;
    t2c_close_loop ( new ) ;
    RETURN ( new ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2d_extend_edges ( edge0, edge1, p0, p1, tol )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge0, edge1 ;
PT2 p0, p1 ;
REAL tol ;
{
    DML_LIST list = dml_create_list () ;
    C2_INT_REC ci, ci0 = NULL ;
    REAL dist, dist0 = 0.0 ;
    DML_ITEM item ;

    c2c_intersect_ext ( T2_EDGE_CURVE(edge0), T2_EDGE_CURVE(edge1), list ) ;
    DML_WALK_LIST ( list, item ) {
        ci = DML_RECORD(item) ;
        dist = fabs ( T2_EDGE_T1(edge0) - C2_INT_REC_T1(ci) ) +
               fabs ( T2_EDGE_T0(edge1) - C2_INT_REC_T2(ci) ) ;
        if ( ci0 == NULL || dist < dist0 ) {
            ci0 = ci ;
            dist0 = dist ;
        }
    }
    if ( ci0 != NULL && ( C2V_DIST ( C2_INT_REC_PT(ci0), p0 ) <= tol ) 
        && ( C2V_DIST ( C2_INT_REC_PT(ci0), p1 ) <= tol ) ) {
        t2c_trim_edge ( edge0, NULL, C2_INT_REC_PARM1(ci0) ) ;
        t2c_trim_edge ( edge1, C2_INT_REC_PARM2(ci0), NULL ) ;
    }
    dml_destroy_list ( list, ( PF_ACTION ) c2d_free_int_rec ) ;
    RETURN ( ci0 != NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2d_close_extend ( loop, tol )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL tol ;
{
    PT2 p0, p1 ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL || 
        T2_LOOP_CLOSED(loop) )
        RETURN ( TRUE ) ;
    t2c_loop_ept0 ( loop, p0 ) ;
    t2c_loop_ept1 ( loop, p1 ) ;
    T2_LOOP_CLOSED(loop) = ( C2V_DIST ( p0, p1 ) <= tol ) && 
        t2d_extend_edges ( t2c_last_edge ( loop ), t2c_first_edge ( loop ), 
            p0, p1, tol ) ;
    RETURN ( T2_LOOP_CLOSED(loop) ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

