/* -R -Z -S -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/******************************* T2C3.C *********************************/ 
/**************** Two-dimensional topology ******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h> 
#include <c2cdefs.h>
#include <c2ddefs.h>
#include <c2vdefs.h>
#include <c2vmcrs.h>
#include <m2cdefs.h>
#include <t2cdefs.h>
#include <t2cpriv.h>
#include <t2ddefs.h>
#include <t2dpriv.h>
#include <t2idefs.h>
#include <t2attrd.h>
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC BOOLEAN t2c_ray_pt __(( DML_LIST, PT2 )) ;
STATIC BOOLEAN t2c_join_lines_loop1 __(( T2_LOOP )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_mark_region ( region, s ) 
/*-------------------------------------------------------------------------*/
/* changes edges attr values from to s */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
ATTR s ;
{
    if ( region != NULL ) 
        t2c_mark_looplist ( T2_REGION_LOOP_LIST(region), s ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_mark_looplist ( looplist, s ) 
/*-------------------------------------------------------------------------*/
/* changes edges attr values from to s */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
ATTR s ;
{
    DML_ITEM item ;
    if ( looplist == NULL ) 
        RETURN ;
    DML_WALK_LIST ( looplist, item ) 
        t2c_mark_loop ( DML_RECORD(item), s ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_mark_loop ( loop, s ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
ATTR s ;
{
    if ( loop == NULL )
        RETURN ;
    t2c_mark_edgelist ( T2_LOOP_EDGE_LIST(loop), s );
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_mark_edgelist ( edgelist, s ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST edgelist ;
ATTR s ;
{
    DML_ITEM item ;
    T2_EDGE edge ;
    if ( edgelist == NULL )
        RETURN ;
    DML_WALK_LIST ( edgelist, item ) {
        edge = ( T2_EDGE ) dml_record ( item ) ;
        T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | s ; 
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_mark_region_part ( region ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
{
    DML_ITEM item ;
    if ( region == NULL || T2_REGION_LOOP_LIST(region) == NULL ) 
        RETURN ;
    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) 
        t2c_mark_loop_part ( DML_RECORD(item) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_mark_loop_part ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    if ( loop == NULL )
        RETURN ;
    t2c_mark_edgelist_part ( T2_LOOP_EDGE_LIST(loop) );
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_mark_edgelist_part ( edgelist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST edgelist ;
{
    DML_ITEM item ;
    T2_EDGE edge ;
    if ( edgelist == NULL )
        RETURN ;
    DML_WALK_LIST ( edgelist, item ) {
        edge = ( T2_EDGE ) dml_record ( item ) ;
        T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_ATTR_PART ;
        T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) & ~T2_ATTR_ROUGH ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_mark_region_rough ( region ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
{
    DML_ITEM item ;
    if ( region == NULL || T2_REGION_LOOP_LIST(region) == NULL ) 
        RETURN ;
    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) 
        t2c_mark_loop_rough ( DML_RECORD(item) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_mark_loop_rough ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    if ( loop == NULL )
        RETURN ;
    t2c_mark_edgelist_rough ( T2_LOOP_EDGE_LIST(loop) );
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_mark_edgelist_rough ( edgelist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST edgelist ;
{
    DML_ITEM item ;
    T2_EDGE edge ;
    if ( edgelist == NULL )
        RETURN ;
    DML_WALK_LIST ( edgelist, item ) {
        edge = ( T2_EDGE ) dml_record ( item ) ;
        T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_ATTR_ROUGH ;
        T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) & ~T2_ATTR_PART ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_attr_init_region ( region ) 
/*-------------------------------------------------------------------------*/
/* changes edges attr values from to s */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
{
    if ( region != NULL && T2_REGION_LOOP_LIST(region) != NULL )  
        dml_apply ( T2_REGION_LOOP_LIST(region), 
        			( PF_ACTION ) t2c_attr_init_loop ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_attr_init_loop ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    if ( loop != NULL ) 
        t2c_attr_init_edgelist ( T2_LOOP_EDGE_LIST(loop) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_attr_init_edgelist ( edgelist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST edgelist ;
{
    DML_ITEM item ;
    T2_EDGE edge ;

    if ( edgelist != NULL ) {
        DML_WALK_LIST ( edgelist, item ) {
            edge = ( T2_EDGE ) dml_record ( item ) ;
            T2_EDGE_ATTR(edge) = (ATTR)0 ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2c_edge_by_attr ( loop, s ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
ATTR s ;
{
    DML_ITEM item ;
    T2_EDGE edge ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ( NULL ) ;
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = dml_record ( item ) ;
        if ( T2_EDGE_ATTR(edge) & s ) 
            RETURN ( edge ) ;
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2c_edge_by_attr_rgn ( region, s ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
ATTR s ;
{
    DML_ITEM item ;
    T2_LOOP loop ;
    T2_EDGE edge ;

    if ( region == NULL || T2_REGION_LOOP_LIST(region) == NULL ) 
        RETURN ( NULL ) ;
    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) {
        loop = dml_record ( item ) ;
        edge = t2c_edge_by_attr ( loop, s ) ;
        if ( edge != NULL ) 
            RETURN ( edge ) ;
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_lmark_region ( region, s ) 
/*-------------------------------------------------------------------------*/
/* changes edges attr values from to s */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
ATTR s ;
{
    DML_ITEM item ;
    if ( region != NULL && T2_REGION_LOOP_LIST(region) != NULL ) {
        DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) 
            t2c_lmark_loop ( DML_RECORD(item), s ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_lmark_loop ( loop, s ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
ATTR s ;
{
    if ( loop != NULL ) 
        T2_LOOP_ATTR(loop) = T2_LOOP_ATTR(loop) | s ; 
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_lattr_init_region ( region ) 
/*-------------------------------------------------------------------------*/
/* changes edges attr values from to s */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
{
    if ( region != NULL && T2_REGION_LOOP_LIST(region) != NULL ) 
        dml_apply ( T2_REGION_LOOP_LIST(region), 
        			( PF_ACTION ) t2c_lattr_init_loop ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_lattr_init_loop ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    if ( loop != NULL ) 
        T2_LOOP_ATTR(loop) = (ATTR)0 ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_orphan_region ( region ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
{
    if ( region != NULL && T2_REGION_LOOP_LIST(region) != NULL ) {
        T2_REGION_PARENT(region) = NULL ;
        dml_apply ( T2_REGION_LOOP_LIST(region), 
        			( PF_ACTION ) t2c_orphan_loop ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_orphan_loop ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    if ( loop != NULL && T2_LOOP_EDGE_LIST(loop) != NULL ) {
        T2_LOOP_PARENT(loop) = NULL ;
        dml_apply ( T2_LOOP_EDGE_LIST(loop), 
        			( PF_ACTION ) t2c_orphan_edge ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_orphan_edge ( edge ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
{
    if ( edge != NULL ) 
        T2_EDGE_PARENT(edge) = NULL ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2c_child_in_loop ( edge, loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
T2_LOOP loop ;
{
    DML_ITEM item ;
    T2_EDGE child ;
    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ( NULL ) ;
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        child = DML_RECORD(item) ;
        if ( ( T2_EDGE_PARENT(child) == edge ) || 
            ( ( T2_EDGE_PARENT(edge) != NULL ) && 
            ( T2_EDGE_PARENT(child) == T2_EDGE_PARENT(edge) ) ) )
            RETURN ( child ) ;
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2c_child_in_region ( edge, region ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
T2_REGION region ;
{
    DML_ITEM item ;
    T2_LOOP loop ;
    T2_EDGE child ;
    if ( region == NULL || T2_REGION_LOOP_LIST(region) == NULL ) 
        RETURN ( NULL ) ;
    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) {
        loop = DML_RECORD(item) ;
        child = t2c_child_in_loop ( edge, loop ) ;
        if ( child != NULL ) 
            RETURN ( child ) ;
    }
    RETURN ( NULL ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2c_child_in_looplist ( edge0, loop_list ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge0 ;
DML_LIST loop_list ;
{
    T2_EDGE edge = NULL ;
    DML_ITEM item ;

    if ( loop_list == NULL || DML_LENGTH(loop_list) == 0 ) 
        RETURN ( NULL ) ;
    DML_WALK_LIST ( loop_list, item ) {
        edge = t2c_child_in_loop ( edge0, DML_RECORD(item) ) ;
        if ( edge != NULL ) 
            RETURN ( edge ) ;
    }
    RETURN ( NULL ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2c_child_in_regionlist ( edge0, region_list ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge0 ;
DML_LIST region_list ;
{
    T2_EDGE edge = NULL ;
    DML_ITEM item ;

    if ( region_list == NULL || DML_LENGTH(region_list) == 0 ) 
        RETURN ( NULL ) ;
    DML_WALK_LIST ( region_list, item ) {
        edge = t2c_child_in_region ( edge0, DML_RECORD(item) ) ;
        if ( edge != NULL ) 
            RETURN ( edge ) ;
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_pt_in_region ( region, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
PT2 pt ;
{
    T2_LOOP loop ;
    T2_EDGE edge ;
    DML_ITEM item ;
    DML_LIST intlist = dml_create_list ();
    REAL angle ;
    C2_CURVE ray ;
    PT2 tan ;

    if ( region == NULL || T2_REGION_LOOP_LIST(region) == NULL ) 
        RETURN ( FALSE ) ;
    loop = T2_REGION_EXT_LOOP(region) ;
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        if ( t2c_mid_pt_tan ( edge, pt, tan ) ) {
            angle = c2v_atan2 ( tan ) + HALF_PI ;
            ray = c2d_ray ( pt, angle ) ;
            t2i_intersect_ray_region ( ray, region, FALSE, FALSE, intlist ) ;
            if ( t2c_ray_pt ( intlist, pt ) ) {
                dml_destroy_list ( intlist, ( PF_ACTION ) t2i_free_int_rec ) ;
                intlist = NULL ;
                RETURN ( TRUE ) ;
            }
            dml_destroy_list ( intlist, ( PF_ACTION ) t2i_free_int_rec ) ;
            intlist = NULL ;
        }
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_pt_in_loop ( loop, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt ;
{
    T2_EDGE edge ;
    DML_LIST intlist = dml_create_list ();
    REAL angle ;
    C2_CURVE ray ;
    PT2 tan ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ( FALSE ) ;
    edge = t2c_biggest_edge ( loop ) ;
    if ( t2c_mid_pt_tan ( edge, pt, tan ) ) {
        angle = c2v_atan2 ( tan ) + HALF_PI ;
        ray = c2d_ray ( pt, angle ) ;
        t2i_intersect_ray_loop ( ray, loop, FALSE, FALSE, TRUE, intlist ) ;
        c2d_free_curve ( ray ) ;
        ray = NULL ;
        if ( t2c_ray_pt ( intlist, pt ) ) {
            dml_destroy_list ( intlist, ( PF_ACTION ) t2i_free_int_rec ) ;
            intlist = NULL ;
            RETURN ( TRUE ) ;
        }
        dml_destroy_list ( intlist, ( PF_ACTION ) t2i_free_int_rec ) ;
        intlist = NULL ;
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2c_ray_pt ( intlist, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST intlist ;
PT2 pt ;
{
    REAL w = 0.0, w0 ;
    DML_ITEM item0, item1 ;
    T2_INT_REC ti0, ti1 ;

    if ( intlist == NULL || DML_LENGTH(intlist) == 0 ) 
        RETURN ( FALSE ) ;
    ti0 = DML_FIRST_RECORD(intlist) ;
    C2V_MID_PT ( pt, T2_INT_REC_PT(ti0), pt ) ;
    w = fabs ( T2_INT_REC_T1(ti0) ) ;

    for ( item0 = dml_second(intlist), item1 = dml_next(item0) ; 
        item0 != NULL && item1 != NULL ; 
        item0 = dml_next(item1),  item1 = dml_next(item0) ) {
        ti0 = DML_RECORD(item0) ;
        ti1 = DML_RECORD(item1) ;
        w0 = fabs ( T2_INT_REC_T1(ti0) - T2_INT_REC_T1(ti1) ) ;
        if ( w0 > w ) {
            C2V_MID_PT ( T2_INT_REC_PT(ti0), T2_INT_REC_PT(ti1), pt ) ;
            w = w0 ;
        }
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_loop_coord_extr ( loop, coord, extr, value ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
INT coord, extr ;
REAL *value ;
{
    RETURN ( t2c_loop_coord_extr_attr ( loop, coord, extr, 
        T2_ATTR_INIT, value ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_loop_coord_extr_attr ( loop, coord, extr, attr, value ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
INT coord, extr ;
ATTR attr ;
REAL *value ;
{
    DML_ITEM item ;
    BOOLEAN valid_z = FALSE, status ;
    REAL z ;
    T2_EDGE edge ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ( FALSE ) ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        status = ( attr == T2_ATTR_INIT ) || 
            ( ( T2_EDGE_ATTR(edge) & attr ) != 0 ) ;
        if ( status && t2c_edge_coord_extr ( edge, coord, extr, &z ) ) {
            if ( !valid_z || ( ( z < *value ) == ( extr == -1 ) ) ) {
                *value = z ;
                valid_z = TRUE ;
            }
        }
    }
    RETURN ( valid_z ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_edge_coord_extr ( edge, coord, extr, value ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
INT coord, extr ;
REAL *value ;
{
    RETURN ( c2c_coord_extr ( T2_EDGE_CURVE(edge), coord, extr, value ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2c_close_loop ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    PT2 p0, p1 ;
    T2_EDGE edge0, edge ;
    DML_ITEM item, item1 ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ;
    for ( item = DML_FIRST(T2_LOOP_EDGE_LIST(loop)) ; 
        item != NULL ; item = item1 ) {
        item1 = DML_NEXT(item) ;
        edge0 = DML_RECORD(item) ;
        t2c_ept1 ( edge0, p0 ) ;
        t2c_ept0 ( edge0, p1 ) ;
        if ( !C2V_IDENT_PTS ( p0, p1 ) ) {
            edge = t2d_create_edge ( loop, c2d_line ( p0, p1 ), 1 ) ;
            T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) & T2_ATTR_AUX ;
            dml_insert_after ( T2_LOOP_EDGE_LIST(loop), item, edge ) ;
        }
    }
    t2c_loop_closed ( loop ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_join_lines_region ( region ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
{
    if ( region != NULL && T2_REGION_LOOP_LIST(region) != NULL ) 
        dml_apply ( T2_REGION_LOOP_LIST(region), 
        			( PF_ACTION ) t2c_join_lines_loop ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_join_lines_loop ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    BOOLEAN done = FALSE ;

    while ( !done ) 
        done = t2c_join_lines_loop1 ( loop ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2c_join_lines_loop1 ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    DML_ITEM item0, item1, item2 ;
    BOOLEAN done = TRUE ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ( done ) ;
    for ( item0 = dml_first(T2_LOOP_EDGE_LIST(loop)), item1 = dml_next(item0) ;
        item1 != NULL ; item0 = item2, item1 = item1 = dml_next(item0) ) {
        item2 = item1 ;
        if ( t2c_join_lines_edges ( DML_RECORD(item0), DML_RECORD(item1) ) ) {
            item2 = dml_next(item1) ;
            t2d_free_edge ( DML_RECORD(item1) ) ;
            dml_remove_item ( T2_LOOP_EDGE_LIST(loop), item1 ) ; 
            done = FALSE ;
        }
    }
    item0 = dml_last(T2_LOOP_EDGE_LIST(loop)) ;
    item1 = dml_first(T2_LOOP_EDGE_LIST(loop)) ;
    if ( t2c_join_lines_edges ( DML_RECORD(item0), DML_RECORD(item1) ) ) {
        t2d_free_edge ( DML_RECORD(item1) ) ;
        dml_remove_first ( T2_LOOP_EDGE_LIST(loop) ) ; 
        done = FALSE ;
    }
    RETURN ( done ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_join_lines_edges ( edge0, edge1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge0, edge1 ;
{
    PT2 p0, p1, q, q1 ;
    BOOLEAN status ;

    if ( !T2_EDGE_IS_LINE(edge0) )
        RETURN ( FALSE ) ;
    if ( !T2_EDGE_IS_LINE(edge1) )
        RETURN ( FALSE ) ;
    if ( ( T2_EDGE_ATTR(edge0) & T2_ATTR_PART ) && 
         ( T2_EDGE_ATTR(edge1) & T2_ATTR_ROUGH ) )
        RETURN ( FALSE ) ;
    if ( ( T2_EDGE_ATTR(edge0) & T2_ATTR_ROUGH ) && 
         ( T2_EDGE_ATTR(edge1) & T2_ATTR_PART ) )
        RETURN ( FALSE ) ;

    t2c_ept0 ( edge0, p0 ) ;
    t2c_ept1 ( edge0, q ) ;
    t2c_ept1 ( edge1, p1 ) ;
    if ( C2V_IDENT_PTS ( p0, p1 ) )
        RETURN ( FALSE ) ;
    c2v_project_line ( p0, p1, q, q1 ) ;
    status = C2V_IDENT_PTS ( q, q1 ) ;
    if ( status ) {
        t2d_set_ept1 ( edge0, p1 ) ;
        T2_EDGE_ATTR(edge0) = T2_EDGE_ATTR(edge0) & T2_EDGE_ATTR(edge1) ;
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_trim_curve_by_loop_0 ( curve, loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
T2_LOOP loop ;
{
    DML_LIST list ;
    DML_ITEM item ;
    PARM parm ;
    T2_INT_REC ti ;
#ifdef CCDK_DEBUG
    REAL t ;
#endif

    list = dml_create_list () ;
    t2i_loop_curve ( loop, curve, FALSE, list ) ;
    parm = NULL ;

    DML_WALK_LIST ( list, item ) {
        ti = (T2_INT_REC)DML_RECORD(item) ;
#ifdef CCDK_DEBUG
        t = T2_INT_REC_T2(ti) ;
#endif
        if ( ( T2_INT_REC_T2(ti) > C2_CURVE_T0(curve) + BBS_ZERO ) && 
             ( T2_INT_REC_T2(ti) < C2_CURVE_T1(curve) - BBS_ZERO ) ) {
            if ( ( parm == NULL ) || ( T2_INT_REC_T2(ti) > PARM_T(parm) ) )
                parm = T2_INT_REC_PARM2(ti) ;
        }
    }

    if ( parm != NULL ) 
        c2c_trim0 ( curve, parm ) ;
    dml_destroy_list ( list, ( PF_ACTION ) t2i_free_int_rec ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2c_trim_curve_by_loop_1 ( curve, loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
T2_LOOP loop ;
{
    DML_LIST list ;
    DML_ITEM item ;
    PARM parm ;
    T2_INT_REC ti ;
#ifdef CCDK_DEBUG
    REAL t ;
#endif

    list = dml_create_list () ;
    t2i_loop_curve ( loop, curve, FALSE, list ) ;
    parm = NULL ;

    DML_WALK_LIST ( list, item ) {
        ti = (T2_INT_REC)DML_RECORD(item) ;
#ifdef CCDK_DEBUG
        t = T2_INT_REC_T2(ti) ;
#endif
        if ( ( T2_INT_REC_T2(ti) > C2_CURVE_T0(curve) + BBS_ZERO ) && 
             ( T2_INT_REC_T2(ti) < C2_CURVE_T1(curve) - BBS_ZERO ) ) {
            if ( ( parm == NULL ) || ( T2_INT_REC_T2(ti) < PARM_T(parm) ) )
                parm = T2_INT_REC_PARM2(ti) ;
        }
    }

    if ( parm != NULL ) 
        c2c_trim1 ( curve, parm ) ;
    dml_destroy_list ( list, ( PF_ACTION ) t2i_free_int_rec ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2c_break_loop_by_attr ( loop0 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0 ;
{
    DML_LIST looplist ;
    DML_ITEM item ;
    ATTR curr_attr ;
    BOOLEAN new_attr ;
    T2_LOOP loop ;
    T2_EDGE edge, new ;

    looplist = dml_create_list () ;

    if ( loop0 == NULL || T2_LOOP_EDGE_LIST(loop0) == NULL ) 
        RETURN ( looplist ) ;

    curr_attr = T2_ATTR_INIT ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop0), item ) {
        edge = (T2_EDGE)DML_RECORD(item) ;
        if ( curr_attr & T2_ATTR_PART ) 
            new_attr = ( ( T2_EDGE_ATTR(edge) & T2_ATTR_PART ) == 0 ) ;
        else if ( curr_attr & T2_ATTR_ROUGH ) 
            new_attr = ( ( T2_EDGE_ATTR(edge) & T2_ATTR_ROUGH ) == 0 ) ;
        else
            new_attr = TRUE ;

        if ( new_attr ) {
            loop = t2d_create_loop ( NULL, dml_create_list () ) ;
            dml_append_data ( looplist, loop ) ;
            if ( T2_EDGE_ATTR(edge) & T2_ATTR_PART ) 
                curr_attr = T2_ATTR_PART ;
            else if ( T2_EDGE_ATTR(edge) & T2_ATTR_ROUGH ) 
                curr_attr = T2_ATTR_ROUGH ;
            T2_LOOP_ATTR(loop) = T2_LOOP_ATTR(loop) | curr_attr ;
        }
        new = t2d_copy_edge ( edge ) ;
        t2d_append_edge ( loop, new, FALSE ) ;
    }
    RETURN ( looplist ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

