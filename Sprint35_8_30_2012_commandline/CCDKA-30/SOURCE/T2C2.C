/* -R -Z -S -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/******************************* T2C2.C *********************************/ 
/**************** Two-dimensional topology ******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2adefs.h> 
#include <c2ddefs.h> 
#include <c2vdefs.h> 
#include <dmldefs.h> 
#include <m2cdefs.h> 
#include <t2cdefs.h>
#include <t2cpriv.h>
#include <t2ddefs.h>
#include <t2ipriv.h>
#include <t2link.h>
#include <c2vmcrs.h>
#ifdef CCDK_DEBUG
#include <dxfdefs.h>
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
EXTERN BOOLEAN DIR ;
#endif
STATIC DML_ITEM t2c_loop_reset_links __(( T2_LOOP, T2_PARM, T2_EDGE )) ;
STATIC void t2c_loop_rearrange_links __(( T2_LOOP, DML_ITEM )) ;
STATIC BOOLEAN t2c_merge_hor_edges_pass1 __(( T2_LOOP )) ;

#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL t2c_loop_x_max ( loop, x_max_parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_PARM x_max_parm ;
{
    PT2 pt ;
    PARM_S x_parm ;
    T2_EDGE edge ;
    REAL x_max ;
    DML_ITEM item ;

    edge = DML_FIRST_RECORD ( T2_LOOP_EDGE_LIST(loop) ) ;

    t2c_ept0 ( edge, pt ) ;
    x_max = pt[0] ;
    T2_PARM_EDGE(x_max_parm) = edge ;
    PARM_T(T2_PARM_CPARM(x_max_parm)) = PARM_T(T2_EDGE_PARM0(edge)) ;
    PARM_J(T2_PARM_CPARM(x_max_parm)) = PARM_J(T2_EDGE_PARM0(edge)) ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop) , item ) {
        edge = DML_RECORD(item) ;
#ifdef CCDK_DEBUG
        DISPLAY++ ;
        if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
            paint_edge ( edge, 9 ) ;
            getch ();
            paint_edge ( edge, 10 ) ;
        }
        else
            DISPLAY-- ;
#endif
        if ( m2c_x_max ( T2_EDGE_CURVE(edge), &x_max, &x_parm ) ) {
            T2_PARM_EDGE(x_max_parm) = edge ;
            COPY_PARM ( &x_parm, T2_PARM_CPARM(x_max_parm) ) ;
#ifdef CCDK_DEBUG
        DISPLAY++ ;
        if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
            t2c_eval_pt ( edge, &x_parm, pt ) ;
            paint_point ( pt, 0.02, 11 ) ;
            getch ();
        }
        else
            DISPLAY-- ;
#endif
        }
    }
    RETURN ( x_max ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC T2_PT_POSITION t2c_pt_pos_region ( region, pt )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
PT2 pt ;
{
    REAL c, s, step, angle ;
    INT n = t2i_reg_hor_ray_int_no ( region, pt, TRUE ) ;

    if ( n == M2_PT_ON_CURVE )
        RETURN ( T2_PT_ON_BOUNDARY ) ;
    else if ( n >= 0 ) 
        RETURN ( n%2 ? T2_PT_INSIDE : T2_PT_OUTSIDE ) ;

    for ( step = HALF_PI ; step > BBS_ZERO ; step *= 0.51 ) {
        for ( angle = step ; angle < TWO_PI - BBS_ZERO ; angle += step ) {
            c = cos ( angle ) ;
            s = sin ( angle ) ;
            n = t2i_reg_ray_int_no ( region, pt, c, s, TRUE ) ;
            if ( n == M2_PT_ON_CURVE )
                RETURN ( T2_PT_ON_BOUNDARY ) ;
            else if ( n >= 0 ) 
                RETURN ( n%2 ? T2_PT_INSIDE : T2_PT_OUTSIDE ) ;
        }
    }

    RETURN ( T2_PT_UNKNOWN ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_PT_POSITION t2c_pt_pos_region_union ( region_list, pt )
/*-------------------------------------------------------------------------*/
DML_LIST region_list ;
PT2 pt ;
{
    DML_ITEM item ;
    T2_PT_POSITION pt_pos ;

    if ( region_list == NULL || DML_LENGTH(region_list) == 0 ) 
        RETURN ( T2_PT_OUTSIDE ) ;

    DML_WALK_LIST ( region_list, item ) {
        pt_pos = t2c_pt_pos_region ( DML_RECORD(item), pt ) ;
        if ( pt_pos == T2_PT_INSIDE || pt_pos == T2_PT_ON_BOUNDARY ) 
            RETURN ( pt_pos ) ;
    }
    RETURN ( T2_PT_OUTSIDE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC T2_PT_POSITION t2c_pt_pos_loop ( loop, pt )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt ;
{
    REAL c, s, step, angle ;
    INT n ;
    BOOLEAN orntn ;

    n = t2i_loop_hor_ray_int_no ( loop, pt, TRUE ) ;

    if ( n == M2_PT_ON_CURVE )
        RETURN ( T2_PT_ON_BOUNDARY ) ;
    orntn = ( t2c_orientation ( loop ) == T2_DIR_CCW ) ;
    if ( n >= 0 ) 
        RETURN ( n%2 == orntn ? T2_PT_INSIDE : T2_PT_OUTSIDE ) ;

    for ( step = HALF_PI ; step > BBS_ZERO ; step *= 0.51 ) {
        for ( angle = step ; angle < TWO_PI - BBS_ZERO ; angle += step ) {
            c = cos ( angle ) ;
            s = sin ( angle ) ;
            n = t2i_loop_ray_int_no ( loop, pt, c, s, TRUE ) ;
            if ( n == M2_PT_ON_CURVE )
                RETURN ( T2_PT_ON_BOUNDARY ) ;
            else if ( n >= 0 ) 
                RETURN ( n%2 == orntn ? T2_PT_INSIDE : T2_PT_OUTSIDE ) ;
        }
    }

    RETURN ( T2_PT_UNKNOWN ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_PT_POSITION t2c_pt_pos_looplist ( looplist, pt, item_ptr )
/*-------------------------------------------------------------------------*/
DML_LIST looplist ;
PT2 pt ;
DML_ITEM *item_ptr ;
{
    DML_ITEM item ;
    T2_PT_POSITION pt_pos ;

    if ( looplist == NULL || DML_LENGTH(looplist) == 0 ) {
        if ( item_ptr != NULL ) 
            *item_ptr = NULL ;
        RETURN ( T2_PT_OUTSIDE ) ;
    }

    DML_WALK_LIST ( looplist, item ) {
        pt_pos = t2c_pt_pos_loop ( DML_RECORD(item), pt ) ;
        if ( pt_pos == T2_PT_OUTSIDE ) {
            if ( item_ptr != NULL ) 
                *item_ptr = DML_PREV(item) ; 
                /* NULL if pt outside the exterior loop */
            RETURN ( DML_PREV(item) == NULL ? T2_PT_OUTSIDE : T2_PT_INSIDE ) ;
        }
        else if ( pt_pos == T2_PT_ON_BOUNDARY ) {
            if ( item_ptr != NULL ) 
                *item_ptr = item ;
            RETURN ( T2_PT_ON_BOUNDARY ) ;
        }
    }
    if ( item_ptr != NULL ) 
        *item_ptr = NULL ;
    RETURN ( T2_PT_INSIDE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_PT_POSITION t2c_pt_pos_loops_common ( looplist, pt )
/*-------------------------------------------------------------------------*/
DML_LIST looplist ;
PT2 pt ;
{
    T2_PT_POSITION pt_pos ;
    DML_ITEM item ;

    if ( looplist == NULL ) 
        RETURN ( T2_PT_OUTSIDE ) ;

    DML_WALK_LIST ( looplist, item ) {
        pt_pos = t2c_pt_pos_loop ( DML_RECORD(item), pt ) ;
        if ( pt_pos != T2_PT_INSIDE ) 
            RETURN ( T2_PT_OUTSIDE ) ;
    }
    RETURN ( T2_PT_INSIDE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_loop_box ( loop )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    DML_ITEM item0, item ;
    T2_EDGE edge ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL || 
        DML_LENGTH(T2_LOOP_EDGE_LIST(loop)) == 0 ) 
        RETURN ;
    item0 = DML_FIRST(T2_LOOP_EDGE_LIST(loop)) ;
    edge = (T2_EDGE)DML_RECORD(item0) ;
    c2a_box_copy ( T2_EDGE_BOX(edge), T2_LOOP_BOX(loop) ) ;

    DML_FOR_LOOP ( DML_NEXT(item0), item ) {
        edge = ( T2_EDGE ) dml_record ( item ) ;
        c2a_box_append ( T2_LOOP_BOX(loop), T2_EDGE_BOX(edge) ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT t2c_loop_rearrange_pt ( loop, pt, on_vtcs, loop_pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt, loop_pt ;
BOOLEAN on_vtcs ;
{
    T2_PARM_S parm ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ||
        DML_LENGTH ( T2_LOOP_EDGE_LIST(loop) ) == 0 ) 
        RETURN ( FALSE ) ;
    RETURN ( t2c_project_loop ( loop, pt, on_vtcs, &parm, loop_pt ) && 
        t2c_loop_rearrange_parm ( loop, &parm ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT t2c_loop_rearrange_parm ( loop, parm )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_PARM parm ;
{
    T2_EDGE edge0, edge1 ;
    DML_ITEM item0, link_item ;
    BOOLEAN status ;

    link_item = NULL ;
    if ( parm == NULL ) 
        RETURN ( TRUE ) ;
    else if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ||
        DML_LENGTH ( T2_LOOP_EDGE_LIST(loop) ) == 0 || !T2_LOOP_CLOSED(loop) ) 
        RETURN ( FALSE ) ;
    edge0 = T2_PARM_EDGE(parm) ;
    if ( edge0 == NULL ) 
        RETURN ( TRUE ) ;
    item0 = dml_find_data ( T2_LOOP_EDGE_LIST(loop), edge0 ) ;
    if ( IS_ZERO ( PARM_T(T2_PARM_CPARM(parm))-T2_EDGE_T1(edge0) ) ) {
        item0 = DML_NEXT(item0) ;
        if ( item0 == NULL ) 
            RETURN ( TRUE ) ;
    }
    else if ( !IS_ZERO ( PARM_T(T2_PARM_CPARM(parm))-T2_EDGE_T0(edge0) ) ) {
        edge1 = t2d_copy_edge ( edge0 ) ;
        t2c_trim_edge ( edge0, T2_PARM_CPARM(parm), NULL ) ;
        t2c_trim_edge ( edge1, NULL, T2_PARM_CPARM(parm) ) ;
        if ( T2_EDGE_VTX(edge1) == T2_VTX_SHARP ) 
            T2_EDGE_VTX(edge1) = T2_VTX_TANGENT ;
        dml_insert_prior ( T2_LOOP_EDGE_LIST(loop), item0, edge1 ) ;
        link_item = t2c_loop_reset_links ( loop, parm, edge1 ) ;
    }
    status = ( dml_rearrange ( T2_LOOP_EDGE_LIST(loop), item0 ) != NULL ) ;
    t2c_loop_rearrange_links ( loop, link_item ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_ITEM t2c_loop_reset_links ( loop, parm, edge1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_PARM parm ;
T2_EDGE edge1 ;
{
    DML_ITEM item, item0 ;
    T2_EDGE edge0 = T2_PARM_EDGE(parm) ;
    T2_LINK link ;

    if ( T2_LOOP_LINK_LIST(loop) == NULL ) 
        RETURN ( NULL ) ;
    item0 = NULL ;
    DML_WALK_LIST ( T2_LOOP_LINK_LIST(loop), item ) {
        link = (T2_LINK)DML_RECORD(item) ;
        if ( T2_LINK_EDGE(link) == edge0 ) {
            if ( ( T2_LINK_T(link) > T2_PARM_T(parm) ) == 
                ( T2_EDGE_DIR(edge0) == 1 ) ) {
                if ( item0 == NULL ) 
                    item0 = item ;
                T2_LINK_EDGE(link) = edge1 ;
            }
        }
        else if ( item0 != NULL ) 
            RETURN ( item0 ) ;
    }
    RETURN ( item0 ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2c_loop_rearrange_links ( loop, item0 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
DML_ITEM item0 ;
{
    DML_ITEM e_item, l_item ;
    T2_EDGE edge ;
    T2_LINK link ;

    if ( dml_length ( T2_LOOP_LINK_LIST(loop) ) == 0 ) 
        RETURN ;
    if ( item0 != NULL ) {
        dml_rearrange ( T2_LOOP_LINK_LIST(loop), item0 ) ;
        RETURN ;
    }

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), e_item ) {
        edge = DML_RECORD(e_item) ;
        DML_WALK_LIST ( T2_LOOP_LINK_LIST(loop), l_item ) { 
            link = DML_RECORD(l_item) ;
            if ( T2_LINK_EDGE(link) == edge ) {
                dml_rearrange ( T2_LOOP_LINK_LIST(loop), l_item ) ;
                RETURN ;
            }
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT t2c_compare_links ( link0, link1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LINK link0, link1 ;
{
    T2_LOOP loop = T2_LINK_LOOP(link0) ;
    DML_ITEM item = NULL ;
    T2_EDGE edge0, edge1 ;
    REAL t0, t1 ;;

    edge0 = T2_LINK_EDGE(link0) ;
    edge1 = T2_LINK_EDGE(link1) ;
    t0 = T2_LINK_T(link0) ;
    t1 = T2_LINK_T(link1) ;

    if ( edge0 == edge1 ) {
        if ( IS_ZERO ( t0 - t1 ) )  
            RETURN ( 0 ) ;
        else if ( T2_EDGE_DIR(edge1) == 1 ) 
            RETURN ( t0 < t1 ? 1 : -1 ) ;
        else
            RETURN ( t0 > t1 ? 1 : -1 ) ;
    }
    else if ( IS_SMALL ( t0 - T2_EDGE_T1(edge0) ) && 
        IS_SMALL ( t1 - T2_EDGE_T0(edge1) ) && 
        ( edge1 == t2c_next_edge ( edge0 ) ) )
        RETURN ( 0 ) ;
    else if ( IS_SMALL ( t0 - T2_EDGE_T0(edge0) ) && 
        IS_SMALL ( t1 - T2_EDGE_T1(edge1) ) && 
        ( edge0 == t2c_next_edge ( edge1 ) ) )
        RETURN ( 0 ) ;
    else {
        DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
            if ( DML_RECORD(item) == edge0 ) 
                RETURN ( 1 ) ;
            else if ( DML_RECORD(item) == edge1 ) 
                RETURN ( -1 ) ;
        }
        RETURN ( 1 ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_ITEM t2c_insert_link ( loop, link ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_LINK link ;
{
    DML_ITEM item ;
    T2_LINK link1 ;

    if ( T2_LOOP_LINK_LIST(loop) == NULL ) {
        T2_LOOP_LINK_LIST(loop) = dml_create_list() ;
        RETURN ( dml_append_data ( T2_LOOP_LINK_LIST(loop), link ) ) ;
    }

    DML_WALK_LIST ( T2_LOOP_LINK_LIST(loop), item ) {
        link1 = DML_RECORD(item) ;
        if ( t2c_compare_links ( link, link1 ) == 1 )
            RETURN ( dml_insert_prior ( T2_LOOP_LINK_LIST(loop), 
                item, link ) ) ;
    }
    RETURN ( dml_append_data ( T2_LOOP_LINK_LIST(loop), link ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_loop_make_first_edge ( loop, edge )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_EDGE edge ;
{
    DML_ITEM item ;

    item = dml_find_data ( T2_LOOP_EDGE_LIST(loop), edge ) ;
    RETURN ( dml_rearrange ( T2_LOOP_EDGE_LIST(loop), item ) != NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_loop_make_first_by_pt ( loop, pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt ;
{
    T2_PARM_S parm ;

    RETURN ( t2c_project_loop ( loop, pt, FALSE, &parm, NULL ) &&
        t2c_loop_make_first_edge ( loop, T2_PARM_EDGE(&parm) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT t2c_loop_rearrange_mid ( loop, edge, pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_EDGE edge ;
PT2 pt ;
{
    T2_PARM_S parm ;
    REAL dist, dist0 = 0.0 ;
    PT2 p ;
    DML_ITEM item ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ||
        DML_LENGTH ( T2_LOOP_EDGE_LIST(loop) ) == 0 ) 
        RETURN ( FALSE ) ;
    if ( edge == NULL ) { /* May want to choose the longest or 
            sufficiently long edge */
        if ( pt == NULL ) 
            edge = t2c_first_edge ( loop ) ;
        else {
            DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) { 
                t2c_mid_pt ( DML_RECORD(item), p ) ;
                dist = C2V_DIST ( p, pt ) ;
                if ( ( edge == NULL ) || ( dist < dist0 ) ) {
                    dist0 = dist ;
                    edge = DML_RECORD(item) ;
                }
            }
        }
    }
    if ( edge == NULL ) 
        RETURN ( FALSE ) ;
    T2_PARM_EDGE(&parm) = edge ;
    T2_PARM_T(&parm) = 0.5 * ( T2_EDGE_T0(edge) + T2_EDGE_T1(edge) ) ;
    t2c_parm_adjust ( edge, T2_PARM_CPARM(&parm) ) ;
    RETURN ( t2c_loop_rearrange_parm ( loop, &parm ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_EDGE t2c_loop_insert_vtx ( loop, pt, loop_pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt, loop_pt ;
{
    T2_PARM_S parm ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ||
        DML_LENGTH ( T2_LOOP_EDGE_LIST(loop) ) == 0 ) 
        RETURN ( NULL ) ;
    RETURN ( t2c_project_loop ( loop, pt, TRUE, &parm, loop_pt ) ? 
        t2c_loop_insert_vtx_parm ( loop, &parm ) : NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_EDGE t2c_loop_insert_vtx_parm ( loop, parm )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_PARM parm ;
{
    T2_EDGE edge0, edge1 ;
    DML_ITEM item0 ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ||
        DML_LENGTH ( T2_LOOP_EDGE_LIST(loop) ) == 0 || !T2_LOOP_CLOSED(loop) ) 
        RETURN ( NULL ) ;
    edge0 = T2_PARM_EDGE(parm) ;
    item0 = dml_find_data ( T2_LOOP_EDGE_LIST(loop), edge0 ) ;
    if ( !IS_ZERO ( PARM_T(T2_PARM_CPARM(parm))-T2_EDGE_T0(edge0) ) &&
         !IS_ZERO ( PARM_T(T2_PARM_CPARM(parm))-T2_EDGE_T1(edge0) ) ) {
        edge1 = t2d_copy_edge ( edge0 ) ;
        t2c_trim_edge ( edge0, T2_PARM_CPARM(parm), NULL ) ;
        t2c_trim_edge ( edge1, NULL, T2_PARM_CPARM(parm) ) ;
        dml_insert_prior ( T2_LOOP_EDGE_LIST(loop), item0, edge1 ) ;
    }
    RETURN ( edge1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_DIR t2c_orientation ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    REAL area ; 
    if ( T2_LOOP_CLOSED(loop) ) {
        area = t2c_loop_area ( loop ) ;
        if ( IS_SMALL(area) )
            RETURN ( T2_DIR_UNDEF ) ;
        else if ( area > 0.0 ) 
            RETURN ( T2_DIR_CCW ) ;
        else 
            RETURN ( T2_DIR_CW ) ;
    }
    else 
        RETURN ( T2_DIR_UNDEF ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2c_orient_loop ( loop, dir ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
BOOLEAN dir ;
{
    if ( T2_LOOP_CLOSED(loop) && 
        ( t2c_orientation ( loop ) == T2_DIR_CCW ) != dir ) 
        t2c_reverse_loop ( loop ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2c_walk_region_init ( walk ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION_WALK walk ;
{
    walk[0] = NULL ;
    walk[1] = NULL ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_EDGE t2c_walk_region ( region, walk ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
T2_REGION_WALK walk ;
{
    T2_LOOP loop ;
    if ( walk[1] == NULL ) {
        if ( walk[0] == NULL ) 
            walk[0] = DML_FIRST ( T2_REGION_LOOP_LIST(region) ) ;
        else {
            walk[0] = DML_NEXT ( walk[0] ) ;
            if ( walk[0] == NULL ) 
                RETURN ( NULL ) ;
        }
        loop = DML_RECORD(walk[0]) ;
        walk[1] = DML_FIRST ( T2_LOOP_EDGE_LIST(loop) ) ;
        RETURN ( DML_RECORD(walk[1]) ) ;
    }
    else {
        walk[1] = DML_NEXT ( walk[1] ) ;
        if ( walk[1] == NULL ) {
            walk[0] = DML_NEXT ( walk[0] ) ;
            if ( walk[0] == NULL ) 
                RETURN ( NULL ) ;
        }
        RETURN ( DML_RECORD(walk[1]) ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2c_loops_pos ( loop1, loop2 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop1, loop2 ;
{
    PT2 pt ;
    T2_PT_POSITION pos ;

    if ( t2c_loop_pt ( loop1, pt, NULL ) ) {
        pos = t2c_pt_pos_loop ( loop2, pt ) ;
        if ( pos == T2_PT_INSIDE ) 
            RETURN ( loop1 ) ;      /* loop1 is inside of loop2 */
    }
    if ( t2c_loop_pt ( loop2, pt, NULL ) ) {
        pos = t2c_pt_pos_loop ( loop1, pt ) ;
        if ( pos == T2_PT_INSIDE ) 
            RETURN ( loop2 ) ;      /* loop2 is inside of loop1 */
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2c_reverse_region ( region ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
{
    dml_apply ( T2_REGION_LOOP_LIST(region), ( PF_ACTION ) t2c_reverse_loop );
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2c_reverse_loop ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    t2c_reverse_edgelist ( T2_LOOP_EDGE_LIST(loop) );
    dml_reverse_list ( T2_LOOP_LINK_LIST(loop) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_reverse_edgelist ( edgelist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST edgelist ;
{
    DML_ITEM item, item1 ;
    T2_EDGE edge, edge1 ;
    PT2 sc_pt ;
    T2_VTX_STATUS vtx ;

    if ( dml_length(edgelist) == 0 )
        RETURN ;
    dml_reverse_list ( edgelist ) ;
    edge = DML_FIRST_RECORD(edgelist) ;
    C2V_COPY ( T2_EDGE_SC_PT(edge), sc_pt ) ;
    vtx = T2_EDGE_VTX(edge) ;

    DML_WALK_LIST ( edgelist, item ) {
        edge = DML_RECORD(item) ;
        T2_EDGE_DIR(edge) = -T2_EDGE_DIR(edge) ;
        item1 = DML_NEXT(item) ;
        if ( item1 == NULL ) {
            if ( vtx == T2_VTX_CONVEX )
                T2_EDGE_VTX(edge) = T2_VTX_CONCAVE ;
            else if ( vtx == T2_VTX_CONCAVE )
                T2_EDGE_VTX(edge) = T2_VTX_CONVEX ;
            else
                T2_EDGE_VTX(edge) = vtx ;
            C2V_COPY ( sc_pt, T2_EDGE_SC_PT(edge) ) ;
        }
        else {
            edge1 = DML_RECORD(item1) ;
            if ( T2_EDGE_VTX(edge1) == T2_VTX_CONVEX )
                T2_EDGE_VTX(edge) = T2_VTX_CONCAVE ;
            else if ( T2_EDGE_VTX(edge1) == T2_VTX_CONCAVE )
                T2_EDGE_VTX(edge) = T2_VTX_CONVEX ;
            else
                T2_EDGE_VTX(edge) = T2_EDGE_VTX(edge1) ;
            C2V_COPY ( T2_EDGE_SC_PT(edge1), T2_EDGE_SC_PT(edge) ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_start_open_loop ( loop, start_bias ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 start_bias ;
{
    PT2 p0, p1 ;

    if ( start_bias == NULL ) 
        RETURN ( FALSE ) ;
    if ( T2_LOOP_CLOSED(loop) ) 
        RETURN ( FALSE ) ;
    t2c_loop_ept0 ( loop, p0 ) ;
    t2c_loop_ept1 ( loop, p1 ) ;

    if ( C2V_DIST ( start_bias, p0 ) > C2V_DIST ( start_bias, p1 ) ) {
        t2c_reverse_loop ( loop ) ;
        RETURN ( TRUE ) ;
    }
    else
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL t2c_loop_min_angle ( loop, edge_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_EDGE *edge_ptr ;
{
    T2_EDGE edge0, edge1 ;
    REAL angle, min_angle = TWO_PI ;
    DML_ITEM item0, item1 ;
    PT2 tan0, tan1 ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item0 ) {
        item1 = DML_NEXT(item0) ;
        if ( item1 != NULL ) {
            edge0 = (T2_EDGE)DML_RECORD(item0) ;
            edge1 = (T2_EDGE)DML_RECORD(item1) ;
            t2c_etan1 ( edge0, tan0 ) ;
            t2c_etan0 ( edge1, tan1 ) ;
            angle = c2v_vecs_angle ( tan0, tan1 ) ;
            if ( angle < min_angle ) {
                min_angle = angle ;
                if ( edge_ptr != NULL ) 
                    *edge_ptr = edge0 ;
            }
        }
    }
    if ( T2_LOOP_CLOSED(loop) ) {
        edge0 = DML_LAST_RECORD ( T2_LOOP_EDGE_LIST(loop) ) ;
        edge1 = DML_FIRST_RECORD ( T2_LOOP_EDGE_LIST(loop) ) ;
        t2c_etan1 ( edge0, tan0 ) ;
        t2c_etan0 ( edge1, tan1 ) ;
        angle = c2v_vecs_angle ( tan0, tan1 ) ;
        if ( angle < min_angle ) 
            min_angle = angle ;
    }
    RETURN ( min_angle ) ;
}


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE REAL t2c_region_min_angle ( region, edge_ptr ) 
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
T2_REGION region ; 
T2_EDGE *edge_ptr ;
{ 
    REAL angle, min_angle = TWO_PI ; 
    DML_ITEM item ; 

    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) { 
        angle = t2c_loop_min_angle ( (T2_LOOP)DML_RECORD(item), edge_ptr ) ;
        if ( angle < min_angle ) 
            min_angle = angle ; 
    } 
    RETURN ( min_angle ) ; 
}


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE BOOLEAN t2c_merge_hor_edges ( loop )
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
T2_LOOP loop ;
{ 
    BOOLEAN merged, status ;

    merged = TRUE ;
    status = FALSE ;
    while ( merged ) {
        merged = t2c_merge_hor_edges_pass1 ( loop ) ;
        if ( !merged )
            RETURN ( status ) ;
        status = TRUE ;
    }
	RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/ 
STATIC BOOLEAN t2c_merge_hor_edges_pass1 ( loop )
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
T2_LOOP loop ;
{ 
    DML_ITEM item0, item1 ; 
    T2_EDGE edge0, edge1, edge ;
    BOOLEAN merged = FALSE ;
    PT2 p0, p1, p2 ;
    C2_CURVE line ;

    for ( item0 = dml_first ( T2_LOOP_EDGE_LIST(loop) ) ; 
        item0 != NULL ; item0 = item1 ) {
        item1 = dml_next ( item0 ) ;
        if ( item1 == NULL ) 
            RETURN ( merged ) ;
        edge0 = DML_RECORD(item0) ;
        edge1 = DML_RECORD(item1) ;
        if ( T2_EDGE_IS_LINE(edge0) && T2_EDGE_IS_LINE(edge1) ) {
            t2c_ept0 ( edge0, p0 ) ;
            t2c_ept1 ( edge0, p1 ) ;
            t2c_ept1 ( edge1, p2 ) ;
            if ( IS_SMALL(p0[1]-p1[1]) && IS_SMALL(p1[1]-p2[1]) ) {
                line = c2d_line ( p0, p2 ) ;
                edge = t2d_create_edge ( loop, line, 1 ) ;
                T2_EDGE_ATTR(edge) = 
                    T2_EDGE_ATTR(edge) | T2_EDGE_ATTR(edge0) ;
                T2_EDGE_ATTR(edge) = 
                    T2_EDGE_ATTR(edge) | T2_EDGE_ATTR(edge1) ;
                t2d_free_edge ( edge0 ) ;
                t2d_free_edge ( edge1 ) ;
                DML_RECORD(item1) = edge ;
                item1 = DML_NEXT(item1) ;
                DML_RECORD(item0) = NULL ;
                dml_remove_item ( T2_LOOP_EDGE_LIST(loop), item0 ) ;
                merged = TRUE ;
            }
        }
    }

	RETURN ( merged ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

