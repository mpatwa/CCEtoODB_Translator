/* -S -Z -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2B2.C **********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <c2vdefs.h>
#include <t2bdefs.h>
#include <t2bpriv.h>
#include <t2cdefs.h>
#include <t2cpriv.h>
#include <c2vmcrs.h>
#include <t2link.h>
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
EXTERN BOOLEAN DIR ;
#endif
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC void t2b_verify_inside_loop __(( T2_REGION DUMMY0 , 
            T2_REGION DUMMY1 )) ;
STATIC void t2b_loop_link0 __(( T2_LOOP DUMMY0 , T2_REGION DUMMY1 )) ;
STATIC void t2b_loop_link1 __(( T2_LOOP DUMMY0 , T2_REGION DUMMY1 )) ;
STATIC BOOLEAN t2b_edge_pt __(( T2_EDGE, PARM, PARM, PT2 )) ;
STATIC DML_ITEM t2b_prev_link_item __(( DML_ITEM )) ;
STATIC DML_ITEM t2b_next_link_item __(( DML_ITEM )) ;
STATIC T2_PT_POSITION t2b_prev_pos1 __(( DML_ITEM, T2_REGION )) ;
STATIC T2_PT_POSITION t2b_next_pos1 __(( DML_ITEM, T2_REGION )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_verify_inside ( region1, region2 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region1, region2 ;
{
    DML_ITEM item ;
    T2_LOOP loop ;

    t2b_verify_inside_loop ( region1, region2 ) ;
    t2b_verify_inside_loop ( region2, region1 ) ;
    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region1), item ) {
        loop = (T2_LOOP)DML_RECORD(item) ;
        t2b_delete_links ( loop, TRUE ) ;
    }
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_verify_inside_loop ( region1, region2 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region1, region2 ;
{
    DML_ITEM item ;
    T2_LOOP loop ;

    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region1), item ) {
        loop = (T2_LOOP)DML_RECORD(item) ;
        t2b_loop_link_epts ( loop, region2 ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_loop_link_epts ( loop, region )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_REGION region ;
{
    T2_LINK link0, link1 ;

    t2b_loop_link0 ( loop, region ) ;
    t2b_loop_link1 ( loop, region ) ;
    if ( region == NULL ) {
        link0 = dml_first_record ( T2_LOOP_LINK_LIST(loop) ) ;
        link1 = dml_last_record ( T2_LOOP_LINK_LIST(loop) ) ;
        if ( link0 != NULL && link1 != NULL && 
            T2_LINK_MATE(link0) == NULL && T2_LINK_MATE(link1) == NULL ) {
            T2_LINK_MATE(link0) = link1 ;
            T2_LINK_MATE(link1) = link0 ;
        }
    }
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_loop_link0 ( loop, region )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_REGION region ;
{
    PT2 pt ;
    T2_EDGE edge ;
    T2_PT_POSITION pos ;
    T2_LINK link ;

    link = t2b_loop_pt0 ( loop, pt ) ;
    if ( link != NULL ) {
        t2b_link_decouple ( link ) ;
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) & ~T2_LINK_DELETE ;
        if ( ( T2_LINK_STATUS(link) & T2_LINK_NEXT_IN ) ||
             ( T2_LINK_STATUS(link) & T2_LINK_NEXT_OUT ) ||
             ( T2_LINK_STATUS(link) & T2_LINK_CODIR0 ) )
                RETURN ;
    }

    edge = t2c_first_edge ( loop ) ;
    pos = t2b_pt_pos_region ( region, pt ) ;
    if ( pos == T2_PT_INSIDE ) {
        if ( link == NULL ) {
            t2b_create_link ( edge, T2_EDGE_PARM0(edge), 
                NULL, NULL, T2_LINK_NEXT_IN, NULL ) ;
        }
        else {
            T2_LINK_STATUS(link) = T2_LINK_STATUS(link) & ~T2_LINK_UNDEF ;
            T2_LINK_STATUS(link) = T2_LINK_STATUS(link) | T2_LINK_NEXT_IN ;
        }
    }
    else if ( pos == T2_PT_OUTSIDE ) {
        if ( link == NULL ) 
        t2b_create_link ( edge, T2_EDGE_PARM0(edge), 
            NULL, NULL, T2_LINK_NEXT_OUT, NULL ) ;
        else {
            T2_LINK_STATUS(link) = T2_LINK_STATUS(link) & ~T2_LINK_UNDEF ;
            T2_LINK_STATUS(link) = T2_LINK_STATUS(link) | T2_LINK_NEXT_OUT ;
        }
    }
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_loop_link1 ( loop, region )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_REGION region ;
{
    PT2 pt ;
    T2_EDGE edge ;
    T2_PT_POSITION pos ;
    T2_LINK link ;

    link = t2b_loop_pt1 ( loop, pt ) ;
    if ( link != NULL ) {
        t2b_link_decouple ( link ) ;
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) & ~T2_LINK_DELETE ;
        if ( ( T2_LINK_STATUS(link) & T2_LINK_PREV_IN ) ||
             ( T2_LINK_STATUS(link) & T2_LINK_PREV_OUT ) ||
             ( T2_LINK_STATUS(link) & T2_LINK_CODIR1 ) )
                RETURN ;
    }

    edge = t2c_last_edge ( loop ) ;
    pos = t2b_pt_pos_region ( region, pt ) ;
    if ( pos == T2_PT_INSIDE ) {
        if ( link == NULL ) {
            t2b_create_link ( edge, T2_EDGE_PARM1(edge), 
                NULL, NULL, T2_LINK_PREV_IN, NULL ) ;
        }
        else
            T2_LINK_STATUS(link) = T2_LINK_STATUS(link) | T2_LINK_PREV_IN ;
    }
    else if ( pos == T2_PT_OUTSIDE ) {
        if ( link == NULL ) 
            t2b_create_link ( edge, T2_EDGE_PARM1(edge), 
                NULL, NULL, T2_LINK_PREV_OUT, NULL ) ;
        else
            T2_LINK_STATUS(link) = T2_LINK_STATUS(link) | T2_LINK_PREV_OUT ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_LINK t2b_loop_pt0 ( loop, pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt ;
{
    DML_ITEM item, item1 ;
    T2_LINK link, link1 ;
    T2_EDGE edge ;

    item = dml_first ( T2_LOOP_LINK_LIST(loop) ) ;
    if ( item == NULL ) 
        t2c_loop_ept0 ( loop, pt ) ;

    else {
        link = DML_RECORD(item) ;
        edge = t2c_first_edge ( loop ) ;
        if ( T2_LINK_EDGE(link) == edge ) {
            if ( IS_ZERO ( T2_LINK_T(link) - T2_EDGE_T0(edge) ) ) {
                item1 = DML_NEXT(item) ;
                if ( item1 == NULL ) 
                    t2c_mid_pt ( edge, pt ) ;
                else {
                    link1 = DML_RECORD(item1) ;
                    if ( T2_LINK_EDGE(link1) == edge ) 
                        t2b_edge_pt ( edge, T2_EDGE_PARM0(edge), 
                            T2_LINK_CPARM(link1), pt ) ;
                    else
                        t2c_mid_pt ( edge, pt ) ;
                }
                RETURN ( link ) ;
            }
            else 
                t2b_edge_pt ( edge, T2_EDGE_PARM0(edge), 
                    T2_LINK_CPARM(link), pt ) ;
        }
        else
            t2c_mid_pt ( edge, pt ) ;
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_LINK t2b_loop_pt1 ( loop, pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt ;
{
    DML_ITEM item, item1 ;
    T2_LINK link, link1 ;
    T2_EDGE edge ;

    item = dml_last ( T2_LOOP_LINK_LIST(loop) ) ;
    if ( item == NULL ) 
        t2c_loop_ept1 ( loop, pt ) ;

    else {
        link = DML_RECORD(item) ;
        edge = t2c_last_edge ( loop ) ;
        if ( T2_LINK_EDGE(link) == edge ) {
            if ( IS_ZERO ( T2_LINK_T(link) - T2_EDGE_T1(edge) ) ) {
                item1 = DML_PREV(item) ;
                if ( item1 == NULL ) 
                    t2c_mid_pt ( edge, pt ) ;
                else {
                    link1 = DML_RECORD(item1) ;
                    if ( T2_LINK_EDGE(link1) == edge ) 
                        t2b_edge_pt ( edge, T2_LINK_CPARM(link1), 
                            T2_EDGE_PARM1(edge), pt ) ;
                    else
                        t2c_mid_pt ( edge, pt ) ;
                }
                RETURN ( link ) ;
            }
            else 
                t2b_edge_pt ( edge, T2_LINK_CPARM(link), 
                    T2_EDGE_PARM1(edge), pt ) ;
        }
        else
            t2c_mid_pt ( edge, pt ) ;
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_PT_POSITION t2b_pt_pos_region ( region, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
PT2 pt ;
{
    T2_PARM_S parm ;
    PT2 on_pt, tan, vec ;
    REAL cross ;
    T2_LOOP loop ;
    T2_PT_POSITION pos ;

    if ( region == NULL ) 
        RETURN ( T2_PT_INSIDE ) ;
    loop = T2_REGION_EXT_LOOP(region) ;
    if ( T2_LOOP_CLOSED(loop) ) {
        pos = t2c_pt_pos_region ( region, pt ) ;
        if ( t2c_orientation ( loop ) == T2_DIR_CW ) {
            if ( pos == T2_PT_INSIDE ) 
                pos = T2_PT_OUTSIDE ;
            else if ( pos == T2_PT_OUTSIDE ) 
                pos = T2_PT_INSIDE ;
        }
        RETURN ( pos ) ;
    }

    else if ( t2c_project_region ( region, pt, TRUE, &parm, on_pt ) &&
        t2c_eval_tan ( T2_PARM_EDGE(&parm), T2_PARM_CPARM(&parm), tan ) ) {
        C2V_SUB ( pt, on_pt, vec ) ;
        if ( C2V_IS_SMALL(vec) ) 
            RETURN ( T2_PT_ON_BOUNDARY ) ;
        cross = C2V_CROSS ( vec, tan ) ;
        RETURN ( cross > 0.0 ? T2_PT_OUTSIDE : T2_PT_INSIDE ) ;
    }
    else
        RETURN ( T2_PT_UNKNOWN ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2b_prev_pt ( item0, p ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM item0 ;
PT2 p ;
{
    T2_LINK link0, link1 ;
    T2_EDGE edge0, edge1 ;
    DML_ITEM item1 ;
    T2_LOOP loop ;
    PARM parm0, parm1 ;

    link0 = DML_RECORD(item0) ;
    edge0 = T2_LINK_EDGE(link0) ;
    loop = T2_EDGE_LOOP(edge0) ;

    item1 = t2b_prev_link_item ( item0 ) ;
/*  08-11-92 
    item1 = DML_PREV(item0) ;
*/

    if ( item1 == NULL ) {
        if ( edge0 == t2c_first_edge ( loop ) ) {
            if ( IS_ZERO ( T2_LINK_T(link0) - T2_EDGE_T0(edge0) ) ) {
                if ( !T2_LOOP_CLOSED(loop) )
                    RETURN ( FALSE ) ;
                item1 = DML_LAST ( T2_LOOP_LINK_LIST(loop) ) ;
                link1 = DML_RECORD(item1) ;
                edge1 = T2_LINK_EDGE(link1) ;
                if ( edge1 == t2c_last_edge ( loop ) && 
                    IS_ZERO ( T2_LINK_T(link1) - T2_EDGE_T1(edge1) ) ) {
                    item1 = DML_PREV(item1) ;
                    link1 = DML_RECORD(item1) ;
                    if ( T2_LINK_EDGE(link1) == edge1 ) 
                        t2b_edge_pt ( edge1, T2_LINK_CPARM(link1), 
                            T2_EDGE_PARM1(edge1), p ) ;
                    else 
                        t2b_edge_pt ( edge1, T2_EDGE_PARM0(edge1), 
                            T2_EDGE_PARM1(edge1), p ) ;
                }
                else {
                    edge1 = t2c_last_edge ( loop ) ;
                    t2b_edge_pt ( edge1, T2_EDGE_PARM0(edge1), 
                        T2_EDGE_PARM1(edge1), p ) ;
                }
            }
            else
                t2b_edge_pt ( edge0, T2_EDGE_PARM0(edge0), 
                    T2_LINK_CPARM(link0), p ) ;
        }
        else if ( !IS_ZERO ( T2_LINK_T(link0) - T2_EDGE_T0(edge0) ) ) {
            t2b_edge_pt ( edge0, T2_EDGE_PARM0(edge0), 
                T2_LINK_CPARM(link0), p ) ;
        }
        else {
            edge1 = t2c_prev_edge ( edge0 ) ;
            t2b_edge_pt ( edge1, T2_EDGE_PARM0(edge1), 
                T2_EDGE_PARM1(edge1), p ) ;
        }
    }

    else {
        link1 = DML_RECORD(item1) ;
        edge1 = T2_LINK_EDGE(link1) ;
        if ( IS_ZERO ( T2_LINK_T(link0) - T2_EDGE_T0(edge0) ) ) {
            edge0 = t2c_prev_edge ( edge0 ) ;
            parm0 = T2_EDGE_PARM1(edge0) ;
        }
        else 
            parm0 = T2_LINK_CPARM(link0) ;
        if ( IS_ZERO ( T2_LINK_T(link1) - T2_EDGE_T1(edge1) ) ) {
            edge1 = t2c_next_edge ( edge1 ) ;
            parm1 = T2_EDGE_PARM0(edge1) ;
        }
        else 
            parm1 = T2_LINK_CPARM(link1) ;
        if ( edge0 == edge1 ) 
            t2b_edge_pt ( edge0, parm0, parm1, p ) ;
        else
            t2c_ept0 ( edge0, p ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_PT_POSITION t2b_prev_pos ( link_item, other_region ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM link_item ;
T2_REGION other_region ;
{
    DML_ITEM item ;
    T2_PT_POSITION pos ;
    T2_LOOP loop ;
    DML_LIST link_list ;
    T2_LINK link ;

    for ( item = link_item ; item != NULL ; item = DML_PREV(item) ) {
        pos = t2b_prev_pos1 ( item, other_region ) ;
        if ( ( pos == T2_PT_INSIDE ) || ( pos == T2_PT_OUTSIDE ) )
            RETURN ( pos ) ;
    }

    link = DML_RECORD(link_item) ;
    loop = T2_LINK_LOOP(link) ;
    link_list = T2_LOOP_LINK_LIST(loop) ;

    for ( item = dml_last(link_list) ; item != link_item ; 
        item = DML_PREV(item) ) {
        pos = t2b_prev_pos1 ( item, other_region ) ;
        if ( ( pos == T2_PT_INSIDE ) || ( pos == T2_PT_OUTSIDE ) )
            RETURN ( pos ) ;
    }
    RETURN ( T2_PT_UNKNOWN ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_PT_POSITION t2b_prev_pos1 ( link_item, other_region ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM link_item ;
T2_REGION other_region ;
{
    PT2 p ;
    T2_LINK link, mate ;

    link = DML_RECORD(link_item) ;
    mate = T2_LINK_MATE(link) ;
    if ( mate == NULL && other_region == NULL )
        RETURN ( T2_PT_UNKNOWN ) ;
    t2b_prev_pt ( link_item, p ) ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_point ( T2_LINK_PT(link), 0.015, 10 ) ;
        getch();
        paint_point ( p, 0.02, 11 ) ;
        getch();
    }
    else
        DISPLAY-- ;
#endif
    RETURN ( ( other_region != NULL ) ? t2b_pt_pos_region ( other_region, p ) :
        t2b_pt_pos_loop ( T2_LINK_LOOP(mate), p ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2b_next_pt ( item0, p ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM item0 ;
PT2 p ;
{
    T2_LINK link0, link1 ;
    T2_EDGE edge0, edge1 ;
    DML_ITEM item1 ;
    T2_LOOP loop ;
    PARM parm0, parm1 ;

    link0 = DML_RECORD(item0) ;
    edge0 = T2_LINK_EDGE(link0) ;
    loop = T2_EDGE_LOOP(edge0) ;

    item1 = t2b_next_link_item ( item0 ) ;
/*  08-11-92 
    item1 = DML_NEXT(item0) ;
*/
    if ( item1 == NULL ) {
        if ( edge0 == t2c_last_edge ( loop ) ) {
            if ( IS_ZERO ( T2_LINK_T(link0) - T2_EDGE_T1(edge0) ) ) {
                if ( !T2_LOOP_CLOSED(loop) )
                    RETURN ( FALSE ) ;
                item1 = DML_FIRST ( T2_LOOP_LINK_LIST(loop) ) ;
                link1 = DML_RECORD(item1) ;
                edge1 = T2_LINK_EDGE(link1) ;
                if ( edge1 == t2c_first_edge ( loop ) &&
                    IS_ZERO ( T2_LINK_T(link1) - T2_EDGE_T0(edge1) ) ) {
                    item1 = DML_NEXT(item1) ;
                    link1 = DML_RECORD(item1) ;
                    if ( T2_LINK_EDGE(link1) == edge1 ) 
                        t2b_edge_pt ( edge1, T2_EDGE_PARM0(edge1), 
                            T2_LINK_CPARM(link1), p ) ;
                    else 
                        t2b_edge_pt ( edge1, T2_EDGE_PARM0(edge1), 
                            T2_EDGE_PARM1(edge1), p ) ;
                }
                else {
                    edge1 = t2c_first_edge ( loop ) ;
                    t2b_edge_pt ( edge1, T2_EDGE_PARM0(edge1), 
                        T2_EDGE_PARM1(edge1), p ) ;
                }
            }
            else
                t2b_edge_pt ( edge0, T2_LINK_CPARM(link0), 
                    T2_EDGE_PARM1(edge0), p ) ;
        }
        else if ( !IS_ZERO ( T2_LINK_T(link0) - T2_EDGE_T1(edge0) ) ) {
            t2b_edge_pt ( edge0, T2_LINK_CPARM(link0), 
                T2_EDGE_PARM1(edge0), p ) ;
        }
        else {
            edge1 = t2c_next_edge ( edge0 ) ;
            t2b_edge_pt ( edge1, T2_EDGE_PARM0(edge1), 
                T2_EDGE_PARM1(edge1), p ) ;
        }
    }

    else {
        link1 = DML_RECORD(item1) ;
        edge1 = T2_LINK_EDGE(link1) ;
        if ( IS_ZERO ( T2_LINK_T(link0) - T2_EDGE_T1(edge0) ) ) {
            edge0 = t2c_next_edge ( edge0 ) ;
            parm0 = T2_EDGE_PARM0(edge0) ;
        }
        else 
            parm0 = T2_LINK_CPARM(link0) ;
        if ( IS_ZERO ( T2_LINK_T(link1) - T2_EDGE_T0(edge1) ) ) {
            edge1 = t2c_prev_edge ( edge1 ) ;
            parm1 = T2_EDGE_PARM1(edge1) ;
        }
        else 
            parm1 = T2_LINK_CPARM(link1) ;
        if ( edge0 == edge1 ) 
            t2b_edge_pt ( edge0, parm0, parm1, p ) ;
        else
            t2c_ept1 ( edge0, p ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_ITEM t2b_prev_link_item ( item0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM item0 ;
{
    T2_LINK link0, link1 ;
    T2_EDGE edge0, edge1 ;
    DML_ITEM item1 ;

    link0 = DML_RECORD(item0) ;
    edge0 = T2_LINK_EDGE(link0) ;

    for ( item1 = DML_PREV(item0) ; item1 != NULL ; item1 = DML_PREV(item1) ) {
        link1 = DML_RECORD(item1) ;
        edge1 = T2_LINK_EDGE(link1) ;
        if ( edge0 == edge1 ) {
            if ( !IS_SMALL ( T2_LINK_T(link1) - T2_LINK_T(link0) ) )
                RETURN ( item1 ) ;
        }
        else {
            if ( !IS_SMALL ( T2_LINK_T(link0) - T2_EDGE_T0(edge0) ) ||
                 !IS_SMALL ( T2_LINK_T(link1) - T2_EDGE_T1(edge1) ) ||
                 ( edge1 != t2c_prev_edge ( edge0 ) ) ) 
                RETURN ( item1 ) ;
        }
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_ITEM t2b_next_link_item ( item0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM item0 ;
{
    T2_LINK link0, link1 ;
    T2_EDGE edge0, edge1 ;
    DML_ITEM item1 ;

    link0 = DML_RECORD(item0) ;
    edge0 = T2_LINK_EDGE(link0) ;

    DML_FOR_LOOP ( item0, item1 ) {
        link1 = DML_RECORD(item1) ;
        edge1 = T2_LINK_EDGE(link1) ;
        if ( edge0 == edge1 ) {
            if ( !IS_SMALL ( T2_LINK_T(link1) - T2_LINK_T(link0) ) )
                RETURN ( item1 ) ;
        }
        else {
            if ( !IS_SMALL ( T2_LINK_T(link0) - T2_EDGE_T1(edge0) ) ||
                 !IS_SMALL ( T2_LINK_T(link1) - T2_EDGE_T0(edge1) ) ||
                 ( edge1 != t2c_next_edge ( edge0 ) ) ) 
                RETURN ( item1 ) ;
        }
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_PT_POSITION t2b_next_pos ( link_item, other_region ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM link_item ;
T2_REGION other_region ;
{
    DML_ITEM item ;
    T2_PT_POSITION pos ;
    T2_LOOP loop ;
    DML_LIST link_list ;
    T2_LINK link ;

    for ( item = link_item ; item != NULL ; item = DML_NEXT(item) ) {
        pos = t2b_next_pos1 ( item, other_region ) ;
        if ( ( pos == T2_PT_INSIDE ) || ( pos == T2_PT_OUTSIDE ) )
            RETURN ( pos ) ;
    }

    link = DML_RECORD(link_item) ;
    loop = T2_LINK_LOOP(link) ;
    link_list = T2_LOOP_LINK_LIST(loop) ;

    for ( item = dml_first(link_list) ; item != link_item ; 
        item = DML_NEXT(item) ) {
        pos = t2b_next_pos1 ( item, other_region ) ;
        if ( ( pos == T2_PT_INSIDE ) || ( pos == T2_PT_OUTSIDE ) )
            RETURN ( pos ) ;
    }
    RETURN ( T2_PT_UNKNOWN ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_PT_POSITION t2b_next_pos1 ( link_item, other_region ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM link_item ;
T2_REGION other_region ;
{
    PT2 p ;
    T2_LINK link, mate ;

    link = DML_RECORD(link_item) ;
    mate = T2_LINK_MATE(link) ;
    if ( mate == NULL )
        RETURN ( T2_PT_UNKNOWN ) ;
    t2b_next_pt ( link_item, p ) ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_point ( T2_LINK_PT(link), 0.015, 10 ) ;
        getch();
        paint_point ( p, 0.02, 12 ) ;
        getch();
    }
    else
        DISPLAY-- ;
#endif
    RETURN ( ( other_region != NULL ) ? t2b_pt_pos_region ( other_region, p ) :
        t2b_pt_pos_loop ( T2_LINK_LOOP(mate), p ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_edge_pt ( edge, parm0, parm1, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM parm0, parm1 ;
PT2 pt ;
{
    PARM_S parm ;
    PARM_T(&parm) = 0.5 * ( PARM_T(parm0) + PARM_T(parm1) ) ;
    PARM_J(&parm) = ( PARM_J(parm0) + PARM_J(parm1) ) / 2 ;
    t2c_parm_adjust ( edge, &parm ) ;
    RETURN ( t2c_eval_pt ( edge, &parm, pt ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_link_decouple ( link ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LINK link ;
{
    T2_LINK mate ;

    mate = T2_LINK_MATE(link) ;
    if ( mate != NULL ) {
        T2_LINK_MATE(mate) = NULL ;
        T2_LINK_MATE(link) = NULL ;
    }
}

#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

