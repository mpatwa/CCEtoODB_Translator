/* -S -T -Z -L __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2BP.C **********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <dmldefs.h>
#include <c2cdefs.h>
#include <c2ddefs.h>
#include <c2vdefs.h>
#include <t2bdefs.h>
#include <t2bpriv.h>
#include <t2cdefs.h>
#include <t2ddefs.h>
#include <t2ipriv.h>
#include <t2link.h>
#include <c2vmcrs.h>
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC INT t2b_loop_side __(( T2_LOOP, PT2, PT2 )) ;
STATIC void t2b_pierce_divide_loop __(( T2_LOOP, PT2, PT2, 
            DML_LIST, DML_LIST, DML_LIST )) ;
STATIC void t2b_pierce_divide_ray __(( T2_LOOP, T2_LOOP, 
            DML_LIST, DML_LIST )) ;
STATIC T2_LOOP t2b_pierce_assemble __(( DML_LIST, DML_LIST, DML_LIST )) ;
#ifdef CCDK_DEBUG
EXTERN INT DIS_LEVEL ;
EXTERN INT DISPLAY ;
EXTERN BOOLEAN DIR ;
#endif

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2b_pierce_loop ( loop, pt, angle, inf_line, 
            right_list, left_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt ;
REAL angle ;
BOOLEAN inf_line ;
DML_LIST right_list, left_list ;
{
    C2_CURVE ray = c2d_ray ( pt, angle ) ;
    DML_LIST intlist = dml_create_list(), rlist1, llist1, rlist2, llist2, 
        list0 ;
    T2_LOOP ray_loop, loop1 ;
    DML_ITEM item ;
    PT2 p0, p1 ;

    ray_loop = t2d_create_loop ( NULL, dml_create_list () ) ;
    t2d_append_curve ( ray_loop, ray, BBS_TOL ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop, 10 ) ;
    paint_loop ( ray_loop, 11 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
    t2b_intersect_loop ( loop, ray_loop, 0, inf_line, intlist ) ;
    t2b_loop_link_epts ( loop, NULL ) ;

    rlist1 = ( right_list == NULL ) ? NULL : dml_create_list ();
    llist1 = ( left_list == NULL ) ? NULL : dml_create_list ();
    rlist2 = ( right_list == NULL ) ? NULL : dml_create_list ();
    llist2 = ( left_list == NULL ) ? NULL : dml_create_list ();
    list0 = dml_create_list ();

    c2c_etan0 ( ray, p0 ) ;
    c2v_normalize ( p0, p0 ) ;
    t2b_pierce_divide_loop ( loop, pt, p0, rlist1, llist1, list0 ) ;

    t2b_pierce_divide_ray ( ray_loop, loop, rlist2, llist2 ) ;
#ifdef CCDK_DEBUG
{
DML_ITEM item ;
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    if ( rlist1 != NULL ) {
        DML_WALK_LIST ( rlist1, item ) {
            paint_loop ( DML_RECORD(item), 9 ) ;
            getch () ;
        }
    }
    if ( llist1 != NULL ) {
        DML_WALK_LIST ( llist1, item ) {
            paint_loop ( DML_RECORD(item), 10 ) ;
            getch () ;
        }
    }
    if ( rlist2 != NULL ) {
        DML_WALK_LIST ( rlist2, item ) {
            paint_loop ( DML_RECORD(item), 9 ) ;
            getch () ;
        }
    }
    if ( llist2 != NULL ) {
        DML_WALK_LIST ( llist2, item ) {
            paint_loop ( DML_RECORD(item), 10 ) ;
            getch () ;
        }
    }
}
else
    DISPLAY-- ;
}
#endif
    if ( right_list != NULL ) {
        DML_WALK_LIST ( list0, item ) {
            loop1 = DML_RECORD(item) ;
            t2c_loop_ept0 ( loop1, p0 ) ;
            t2c_loop_ept1 ( loop1, p1 ) ;
            if ( p1[0] < p0[0] ) 
                t2c_reverse_loop ( loop1 ) ;
        }
        for ( loop1 = t2b_pierce_assemble ( rlist1, rlist2, list0 ) ; 
            loop1 != NULL ;
            loop1 = t2b_pierce_assemble ( rlist1, rlist2, list0 ) ) {
            dml_append_data ( right_list, loop1 ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop1, 12 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
        }
    }
    if ( left_list != NULL ) {
        DML_WALK_LIST ( list0, item ) {
            loop1 = DML_RECORD(item) ;
            t2c_loop_ept0 ( loop1, p0 ) ;
            t2c_loop_ept1 ( loop1, p1 ) ;
            if ( p0[0] < p1[0] ) 
                t2c_reverse_loop ( loop1 ) ;
        }
        for ( loop1 = t2b_pierce_assemble ( llist1, llist2, list0 ) ; 
            loop1 != NULL ;
            loop1 = t2b_pierce_assemble ( llist1, llist2, list0 ) ) {
            dml_append_data ( left_list, loop1 ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop1, 12 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
        }
    }
    dml_destroy_list ( rlist1, ( PF_ACTION ) t2d_free_loop ) ;
    dml_destroy_list ( rlist2, ( PF_ACTION ) t2d_free_loop ) ;
    dml_destroy_list ( llist1, ( PF_ACTION ) t2d_free_loop ) ;
    dml_destroy_list ( llist2, ( PF_ACTION ) t2d_free_loop ) ;
    dml_destroy_list ( list0, ( PF_ACTION ) t2d_free_loop ) ;
    t2d_free_loop ( ray_loop ) ;
    t2i_clear_links_loop ( loop ) ;
}    


/*-------------------------------------------------------------------------*/ 
STATIC INT t2b_loop_side ( loop, pt, tan ) 
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
T2_LOOP loop ; 
PT2 pt, tan ;
{
    DML_ITEM item ;
    T2_EDGE edge ;
    PT2 a ;
    INT side ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ( 0 ) ;
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        if ( t2c_mid_pt ( edge, a ) ) {
            side = c2v_side ( a, pt, tan ) ;
            if ( side != 0 ) 
                RETURN ( side ) ;
        }
    }
    RETURN ( 0 ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_pierce_divide_loop ( loop, pt, tan, rlist, llist, list0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt, tan ;
DML_LIST rlist, llist, list0 ;
{
    DML_ITEM item, item1 ;
    T2_LOOP loop1 ;
    T2_EDGE edge0, edge1 ;
    T2_LINK link0, link1 ;
    INT side ;

    if ( T2_LOOP_LINK_LIST(loop) == NULL || 
        DML_LENGTH(T2_LOOP_LINK_LIST(loop)) == 0 ) {
        side = t2b_loop_side ( loop, pt, tan ) ;
        if ( side == 1 ) {
            if ( rlist == NULL ) 
                t2d_free_loop ( loop /* loop1 */ ) ;
            else
                dml_append_data ( rlist, loop /* loop1 */ ) ;
        }
        else if ( side == -1 ) {
            if ( llist == NULL ) 
                t2d_free_loop ( loop ) ;
            else
                dml_append_data ( llist, loop ) ;
        }
        else 
            dml_append_data ( list0, loop ) ;
        RETURN ;
    }

    DML_WALK_LIST ( T2_LOOP_LINK_LIST(loop), item ) {
        link0 = dml_record ( item ) ;
        item1 = dml_next ( item ) ;
        if ( item1 == NULL ) 
            RETURN ;
        link1 = dml_record ( item1 ) ;

        edge0 = T2_LINK_EDGE(link0) ;
        edge1 = T2_LINK_EDGE(link1) ;
        if ( edge0 != edge1 || !IS_ZERO( PARM_T(T2_LINK_CPARM(link0)) - 
                PARM_T(T2_LINK_CPARM(link1)) ) ) {
            loop1 = t2d_copy_loop ( loop, edge0, edge1 ) ;
            edge0 = t2c_first_edge ( loop1 ) ;
            edge1 = t2c_last_edge ( loop1 ) ;
            t2d_trim_loop ( loop1, edge0, T2_LINK_CPARM(link0), 
                edge1, T2_LINK_CPARM(link1) ) ;
            if ( DML_LENGTH(T2_LOOP_EDGE_LIST(loop1)) == 0 )
                t2d_free_loop ( loop1 ) ;
            else {
                if ( T2_LINK_STATUS(link0) == T2_LINK_OUT_IN && 
                    T2_LINK_STATUS(link1) == T2_LINK_IN_OUT ) 
                    side = 1 ;
                else if ( T2_LINK_STATUS(link0) == T2_LINK_IN_OUT && 
                    T2_LINK_STATUS(link1) == T2_LINK_OUT_IN ) 
                    side = -1 ;
                else 
                    side = t2b_loop_side ( loop1, pt, tan ) ;
                if ( side == 1 ) {
                    if ( rlist == NULL ) 
                        t2d_free_loop ( loop1 ) ;
                    else
                        dml_append_data ( rlist, loop1 ) ;
                }
                else if ( side == -1 ) {
                    if ( llist == NULL ) 
                        t2d_free_loop ( loop1 ) ;
                    else
                        dml_append_data ( llist, loop1 ) ;
                }
                else 
                    dml_append_data ( list0, loop1 ) ;
            }
        }
    } 
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_pierce_divide_ray ( ray_loop, loop, rlist, llist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP ray_loop, loop ;
DML_LIST rlist, llist ;
{
    DML_ITEM item, item1 ;
    T2_LOOP rloop, lloop ;
    T2_EDGE edge0 ;
    T2_LINK link0, link1 ;
    BOOLEAN status ;
    PT2 p ;

    if ( T2_LOOP_LINK_LIST(ray_loop) == NULL || 
        DML_LENGTH(T2_LOOP_LINK_LIST(ray_loop)) == 0 ) 
        RETURN ;

    DML_WALK_LIST ( T2_LOOP_LINK_LIST(ray_loop), item ) {
        link0 = dml_record ( item ) ;
        item1 = dml_next ( item ) ;
        if ( item1 == NULL ) 
            RETURN ;
        link1 = dml_record ( item1 ) ;
        status = !IS_ZERO( PARM_T(T2_LINK_CPARM(link0)) - 
            PARM_T(T2_LINK_CPARM(link1)) ) ;
        if ( status && T2_LINK_STATUS(link0) == T2_LINK_OUT_IN &&
            T2_LINK_STATUS(link1) == T2_LINK_IN_OUT ) 
            status = FALSE ;
        if ( status && T2_LINK_STATUS(link0) != T2_LINK_IN_OUT &&
            T2_LINK_STATUS(link1) != T2_LINK_OUT_IN &&
            T2_LOOP_CLOSED(loop) ) {
            C2V_MID_PT ( T2_LINK_PT(link0), T2_LINK_PT(link1), p ) ;
            status = ( t2c_pt_pos_loop ( loop, p ) == T2_PT_INSIDE ) ;
        }
        if ( status ) {
            lloop = t2d_copy_loop ( ray_loop, NULL, NULL ) ;
            edge0 = t2c_first_edge ( lloop ) ;
            t2d_trim_loop ( lloop, edge0, T2_LINK_CPARM(link0), 
                edge0, T2_LINK_CPARM(link1) ) ;
            rloop = t2d_copy_loop ( lloop, NULL, NULL ) ;
            t2c_reverse_loop ( rloop ) ;
            if ( llist == NULL ) 
                t2d_free_loop ( lloop ) ;
            else
                dml_append_data ( llist, lloop ) ;
            if ( rlist == NULL ) 
                t2d_free_loop ( rloop ) ;
            else
                dml_append_data ( rlist, rloop ) ;
        }
    } 
}


/*-------------------------------------------------------------------------*/
STATIC T2_LOOP t2b_pierce_assemble ( list1, list2, list0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list1, list2, list0 ;
{
    DML_ITEM item ;
    T2_LOOP loop0, loop ;
    PT2 p0, p1, p ;
    INT l = 0, count = 0 ;
    DML_LIST list, list_1, list_2 ;

    if ( dml_length ( list1 ) == 0 )
        RETURN ( NULL ) ;

    l = DML_LENGTH(list1) + DML_LENGTH(list2) + DML_LENGTH(list0) ;
    list_1 = list1 ;
    list_2 = list2 ;
    item = dml_first ( list_1 ) ;
    loop0 = dml_record ( item ) ;
    t2c_loop_ept0 ( loop0, p0 ) ;
    t2c_loop_ept1 ( loop0, p1 ) ;
    if ( C2V_IDENT_PTS ( p0, p1 ) ) {
        dml_remove_item ( list_1, item ) ;
        item = NULL ;
        T2_LOOP_CLOSED(loop0) = TRUE ;
        RETURN ( loop0 ) ;
    }

    dml_remove_item ( list_1, item ) ;
    item = NULL ;

    while ( DML_LENGTH(list_2) || DML_LENGTH(list0) ) {
        if ( count >= l ) {
            T2_LOOP_CLOSED(loop0) = C2V_IDENT_PTS ( p0, p1 ) ;
            RETURN ( loop0 ) ;
        }
        count++ ;
        list = list_1 ;
        list_1 = list_2 ;
        list_2 = list ;
        item = t2b_clst_loop ( list_1, p1, p ) ;

        if ( item != NULL && C2V_IDENT_PTS ( p, p1 ) ) {
            loop = DML_RECORD(item) ;
            t2c_loop_ept1 ( loop, p1 ) ;
            t2d_append_loop ( loop0, loop ) ;
            dml_remove_item ( list_1, item ) ;
            item = NULL ;
            t2d_free_loop ( loop ) ; 
            loop = NULL ;
            if ( C2V_IDENT_PTS ( p0, p1 ) == TRUE ) {
                T2_LOOP_CLOSED(loop0) = TRUE ;
                RETURN ( loop0 ) ; 
            }
        }
        else {
            item = t2b_clst_loop ( list0, p1, p ) ;

            if ( item != NULL && C2V_IDENT_PTS ( p, p1 ) ) {
                loop = DML_RECORD(item) ;
                t2c_loop_ept1 ( loop, p1 ) ;
                t2d_append_edgelist ( loop0, T2_LOOP_EDGE_LIST(loop) ) ;
                dml_remove_item ( list0, item ) ;
                item = NULL ;
                t2d_free_loop ( loop ) ; 
                loop = NULL ;
                if ( C2V_IDENT_PTS ( p0, p1 ) == TRUE ) {
                    T2_LOOP_CLOSED(loop0) = TRUE ;
                    RETURN ( loop0 ) ; 
                }
            }
        }
    }

    if ( item == NULL )
        T2_LOOP_CLOSED(loop0) = FALSE ;
    RETURN ( loop0 ) ; 
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_ITEM t2b_clst_loop ( list, p1, p ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ; /* list of loops */
PT2 p1, p ;
{
    DML_ITEM item0, item ;
    T2_LOOP loop ;
    PT2 q ;
    REAL dist, dist_min=0.0 ;

    item0 = NULL ;
    DML_WALK_LIST ( list, item ) {
        loop = ( T2_LOOP ) dml_record ( item ) ;
        t2c_loop_ept0 ( loop, q ) ;
        dist = C2V_DIST ( q, p1 ) ;
        if ( item0 == NULL || dist < dist_min ) {
            item0 = item ;
            dist_min = dist ;
            C2V_COPY ( q, p ) ;
        }
    }
    RETURN ( item0 ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

