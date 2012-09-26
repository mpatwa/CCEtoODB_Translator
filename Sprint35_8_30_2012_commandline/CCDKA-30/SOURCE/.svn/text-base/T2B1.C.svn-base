/* -S -Z -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2B1.C **********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <c2vdefs.h>
#include <t2bdefs.h>
#include <t2bpriv.h>
#include <t2cdefs.h>
#include <c2vmcrs.h>
#include <t2link.h>
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
GLOBAL INT T2B_PASS13 = 0 ;
#endif
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC void t2b_verify_links __(( T2_REGION DUMMY0 , T2_REGION DUMMY1 )) ; 
STATIC void    t2b_verify_pass1 __(( T2_REGION DUMMY0 )) ;
STATIC BOOLEAN t2b_verify_pass11 __(( T2_REGION DUMMY0 )) ;
STATIC BOOLEAN t2b_verify_pass12 __(( T2_REGION DUMMY0 )) ;
STATIC void t2b_verify_pass2 __(( T2_REGION DUMMY0 , T2_REGION DUMMY1 )) ; 
STATIC void t2b_verify_pass20 __(( T2_REGION DUMMY0 , T2_REGION DUMMY1 )) ; 
STATIC INT t2b_link_force1 __(( T2_LINK DUMMY0 )) ;
STATIC void t2b_delete_link __(( DML_LIST DUMMY0 , DML_ITEM DUMMY1 , 
            BOOLEAN DUMMY2 )) ;
STATIC void t2b_mark_link_status __(( T2_LINK, INT, INT )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_split ( region1, region2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region1, region2 ;
{
    t2b_intersect ( region1, region2 ) ;
    t2b_verify_links ( region1, region2 ) ;
    t2b_verify_inside ( region1, region2 ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_verify_links ( region1, region2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region1, region2 ;
{
    t2b_verify_pass1 ( region1 ) ;
    t2b_verify_pass1 ( region2 ) ;
    t2b_verify_pass2 ( region1, region2 ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_verify_pass1 ( region1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region1 ;
{
    BOOLEAN done ;

    done = FALSE ;
    while ( !done ) 
        done = t2b_verify_pass11 ( region1 ) ;

    done = FALSE ;
    while ( !done ) 
        done = t2b_verify_pass12 ( region1 ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_verify_pass11 ( region1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region1 ;
{
    DML_ITEM item ;
    BOOLEAN status, done = TRUE ;
    T2_LOOP loop ;

    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region1), item ) {
        loop = (T2_LOOP)DML_RECORD(item) ;
        status = t2b_verify_pass13 ( loop ) ;
        if ( !status )
            done = FALSE ;
    }
    RETURN ( done ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2b_verify_pass13 ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    DML_ITEM item0, item1 ;
    T2_LINK link0, link1 ;
    BOOLEAN status ;
    INT delete ;
#ifdef CCDK_DEBUG
T2B_PASS13++ ;
#endif

    status = TRUE ;
    if ( T2_LOOP_LINK_LIST(loop) == NULL ) 
        RETURN ( status ) ;
    for ( item0 = DML_FIRST(T2_LOOP_LINK_LIST(loop)), 
        item1 = dml_next(item0) ; item1 != NULL && item0 != NULL ; 
        item0 = item1, item1 = DML_NEXT(item0) ) {
        link0 = DML_RECORD(item0) ;
        link1 = DML_RECORD(item1) ;

        if ( t2b_links_same_pos ( link0, link1 ) ) {
#ifdef CCDK_DEBUG
        DISPLAY++ ;
        if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
            paint_point ( T2_LINK_PT(link0), 0.01, 13 ) ;
            getch();
            paint_point ( T2_LINK_PT(link0), 0.01, 14 ) ;
        }
        else
            DISPLAY-- ;
#endif
            delete = t2b_link_force ( link0, link1 ) ;
            if ( delete == 1 ) {
                status = FALSE ;
                t2b_mark_link_delete ( link0, /* TRUE */ FALSE ) ;
            }
            else if ( delete == 2 ) {
                status = FALSE ;
                t2b_mark_link_delete ( link1, /* TRUE */ FALSE ) ;
            }
        }
    }
    if ( !status ) 
        t2b_delete_links ( loop, FALSE ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_verify_pass12 ( region1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region1 ;
{
    DML_ITEM item ;
    BOOLEAN status, done = TRUE ;
    T2_LOOP loop ;

    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region1), item ) {
        loop = (T2_LOOP)DML_RECORD(item) ;
        status = t2b_verify_pass14 ( loop ) ;
        if ( !status )
            done = FALSE ;
    }
    RETURN ( done ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2b_verify_pass14 ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    DML_ITEM item0, item1 ;
    T2_LINK link0, link1 ;
    BOOLEAN status ;

    status = TRUE ;
    if ( T2_LOOP_LINK_LIST(loop) == NULL ) 
        RETURN ( status ) ;
    for ( item0 = DML_FIRST(T2_LOOP_LINK_LIST(loop)), 
        item1 = dml_next(item0) ; item1 != NULL && item0 != NULL ; 
        item0 = item1, item1 = DML_NEXT(item0) ) {
        link0 = DML_RECORD(item0) ;
        link1 = DML_RECORD(item1) ;

        if ( t2b_links_same_pos ( link0, link1 ) ) {
#ifdef CCDK_DEBUG
        DISPLAY++ ;
        if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
            paint_point ( T2_LINK_PT(link0), 0.01, 13 ) ;
            getch();
            paint_point ( T2_LINK_PT(link0), 0.01, 14 ) ;
        }
        else
            DISPLAY-- ;
#endif
            if ( ( ( T2_LINK_STATUS(link0) & T2_LINK_CODIR1 ) && 
                   ( T2_LINK_STATUS(link1) & T2_LINK_CODIR0 ) ) ||
                 ( ( T2_LINK_STATUS(link0) & T2_LINK_ANTIDIR1 ) && 
                   ( T2_LINK_STATUS(link1) & T2_LINK_ANTIDIR0 ) ) ) {
                status = FALSE ;
                t2b_mark_link_delete ( link0, TRUE ) ;
                t2b_mark_link_delete ( link1, TRUE ) ;
            }
        }
    }
    if ( !status ) 
        t2b_delete_links ( loop, TRUE ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2b_links_same_pos ( link0, link1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LINK link0, link1 ;
{
    T2_EDGE edge1 = T2_LINK_EDGE(link0), edge2 = T2_LINK_EDGE(link1) ;

    if ( edge1 == edge2 && IS_SMALL ( T2_LINK_T(link0) - T2_LINK_T(link1) ) )
        RETURN ( TRUE ) ;
    else if ( IS_ZERO ( T2_LINK_T(link0) - T2_EDGE_T1(edge1) ) &&
         IS_ZERO ( T2_LINK_T(link1) - T2_EDGE_T0(edge2) ) &&
         ( t2c_next_edge ( edge1 ) == edge2 ) ) 
        RETURN ( TRUE ) ;
    else
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_delete_link ( link_list, item, del_mate ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST link_list ;
DML_ITEM item ;
BOOLEAN del_mate ;
{
    T2_LINK link, mate ;
    T2_LOOP mate_loop ;

    link = DML_RECORD(item) ;
    mate = T2_LINK_MATE(link) ;
    T2_FREE_LINK ( link ) ;
//    link = NULL ;
    dml_remove_item ( link_list, item ) ;
    if ( mate != NULL ) {
        T2_LINK_MATE(mate) = NULL ;
        if ( del_mate ) {
            mate_loop = T2_LINK_LOOP(mate) ;
            dml_remove_data ( T2_LOOP_LINK_LIST(mate_loop), mate ) ;
            T2_FREE_LINK ( mate ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_mark_link_delete ( link, del_mate ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LINK link ;
BOOLEAN del_mate ;
{
    if ( link != NULL ) {
        if ( del_mate )
            t2b_mark_link_status ( link, T2_LINK_DELETE, T2_LINK_DELETE ) ;
        else
            t2b_mark_link_status ( link, T2_LINK_DELETE, 0 ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_delete_links ( loop, del_mate ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
BOOLEAN del_mate ;
{
    DML_ITEM item, item1 ;
    T2_LINK link ;

    if ( T2_LOOP_LINK_LIST(loop) == NULL ) 
        RETURN ;
    if ( del_mate ) {
        DML_WALK_LIST ( T2_LOOP_LINK_LIST(loop), item ) {
            link = (T2_LINK)DML_RECORD(item) ;
            if ( T2_LINK_STATUS(link) & T2_LINK_DELETE ) 
                t2b_mark_link_delete ( T2_LINK_MATE(link), TRUE ) ;
        }
    }

    for ( item = DML_FIRST(T2_LOOP_LINK_LIST(loop)) ; 
        item != NULL ; item = item1 ) {
        item1 = DML_NEXT(item) ;
        link = DML_RECORD(item) ;
        if ( ( item1 != NULL ) && ( T2_LINK_MATE(link) != NULL ) && 
            ( DML_RECORD(item1) == T2_LINK_MATE(link) ) ) 
            item1 = DML_NEXT(item1) ;
        if ( T2_LINK_STATUS(link) & T2_LINK_DELETE ) 
            t2b_delete_link ( T2_LOOP_LINK_LIST(loop), item, del_mate ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT t2b_link_force ( link0, link1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LINK link0, link1 ;
{
    if ( T2_LINK_STATUS(link0) & T2_LINK_DELETE ) 
        RETURN ( 1 ) ;
    else if ( T2_LINK_STATUS(link1) & T2_LINK_DELETE ) 
        RETURN ( 2 ) ;
    else if ( T2_LINK_STATUS(link0) & T2_LINK_UNDEF ) {
        if ( T2_LINK_STATUS(link1) & T2_LINK_UNDEF ) 
            RETURN ( t2b_link_force1 ( link0 ) ) ;
        else
            RETURN ( 1 ) ;
    }
    else if ( T2_LINK_STATUS(link1) & T2_LINK_UNDEF ) 
        RETURN ( 2 ) ;
    else
        RETURN ( 0 ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT t2b_link_force1 ( link1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LINK link1 ;
{
    T2_EDGE edge, edge1 ;
    T2_LINK mate1 ;
    T2_LOOP loop, loop1 ;
    BOOLEAN mate_first, mate_last ;

    edge = T2_LINK_EDGE(link1) ;
    loop = T2_EDGE_LOOP(edge) ;
    mate1 = T2_LINK_MATE(link1) ;
    if ( mate1 == NULL ) {
        mate_first = TRUE ;
        mate_last = TRUE ;
    }
    else {
        edge1 = T2_LINK_EDGE(mate1) ;
        loop1 = T2_EDGE_LOOP(edge1) ;
        mate_first = ( edge1 == t2c_first_edge ( loop1 ) ) ;
        mate_last = ( edge1 == t2c_last_edge ( loop1 ) ) ;
    }

    if ( edge == DML_FIRST_RECORD(T2_LOOP_EDGE_LIST(loop)) && mate_last ) 
            RETURN ( 1 ) ;
    else if ( edge == DML_LAST_RECORD(T2_LOOP_EDGE_LIST(loop)) && mate_first ) 
            RETURN ( 1 ) ;
    else
        RETURN ( 2 ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_verify_pass2 ( region1, region2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region1, region2 ;
{
/*
    t2b_verify_pass20 ( region1, region2 ) ;
    t2b_verify_pass20 ( region2, region1 ) ;
*/
    t2b_classify ( region1, region2 ) ;
}


/*-------------------------------------------------------------------------*/
#if 0

STATIC void t2b_verify_pass20 ( region, other_region ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region, other_region ;
{
    DML_ITEM item ;

    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) 
        t2b_verify_pass21 ( DML_RECORD(item), other_region ) ;
}

#endif // 0

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_verify_pass21 ( loop, other_region ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_REGION other_region ;
{
    DML_ITEM item ;

    if ( T2_LOOP_LINK_LIST(loop) == NULL ) 
        RETURN ;
    DML_WALK_LIST ( T2_LOOP_LINK_LIST(loop), item ) 
        t2b_link_status ( item, other_region ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_link_status ( link_item, other_region ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM link_item ;
T2_REGION other_region ;
{
    T2_LINK link ;
    T2_PT_POSITION prev_pos, next_pos ;
    T2_LOOP loop ;
    DML_LIST link_list ;

    link = DML_RECORD(link_item) ;
    if ( T2_LINK_STATUS(link) & T2_LINK_DELETE ) 
        RETURN ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_point ( T2_LINK_PT(link), 0.01, 13 ) ;
        getch();
        paint_point ( T2_LINK_PT(link), 0.01, 14 ) ;
    }
    else
        DISPLAY-- ;
#endif
    loop = T2_LINK_LOOP(link) ;
    link_list = T2_LOOP_LINK_LIST(loop) ;

    if ( DML_LENGTH(link_list) == 1 ) {
        t2b_mark_link_delete ( link, TRUE ) ;
        RETURN ;
    }

    if ( T2_LINK_STATUS(link) & T2_LINK_PREV_IN ) 
        prev_pos = T2_PT_INSIDE ;
    else if ( T2_LINK_STATUS(link) & T2_LINK_PREV_OUT ) 
        prev_pos = T2_PT_OUTSIDE ;
    else 
        prev_pos = t2b_prev_pos ( link_item, other_region ) ;
    if ( prev_pos == T2_PT_INSIDE ) {
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) & ~T2_LINK_UNDEF ;
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) | T2_LINK_PREV_IN ;
    }
    else if ( prev_pos == T2_PT_OUTSIDE ) {
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) & ~T2_LINK_UNDEF ;
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) | T2_LINK_PREV_OUT ;
    }

    if ( T2_LINK_STATUS(link) & T2_LINK_NEXT_IN ) 
        next_pos = T2_PT_INSIDE ;
    else if ( T2_LINK_STATUS(link) & T2_LINK_NEXT_OUT ) 
        next_pos = T2_PT_OUTSIDE ;
    else 
        next_pos = t2b_next_pos ( link_item, other_region ) ;
    if ( next_pos == T2_PT_INSIDE ) {
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) & ~T2_LINK_UNDEF ;
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) | T2_LINK_NEXT_IN ;
    }
    else if ( next_pos == T2_PT_OUTSIDE ) {
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) & ~T2_LINK_UNDEF ;
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) | T2_LINK_NEXT_OUT ;
    }

    if ( ( prev_pos == T2_PT_INSIDE ) && ( next_pos == T2_PT_INSIDE ) )
        t2b_mark_link_delete ( link, /* TRUE */ FALSE ) ;
    else if ( ( prev_pos == T2_PT_OUTSIDE ) && ( next_pos == T2_PT_OUTSIDE ) )
        t2b_mark_link_delete ( link, /* TRUE */ FALSE ) ;
/* Added 07-01-92 */
    else if ( ( prev_pos == T2_PT_ON_BOUNDARY ) && 
        ( next_pos == T2_PT_ON_BOUNDARY ) )
        t2b_mark_link_delete ( link, /* TRUE */ FALSE ) ;
/* Added 07-01-92 */
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_mark_link_status ( link, s0, s1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LINK link ;
INT s0, s1 ;
{
    T2_LINK mate = T2_LINK_MATE(link) ;

    T2_LINK_STATUS(link) = T2_LINK_STATUS(link) & ~T2_LINK_UNDEF ; 
    T2_LINK_STATUS(link) = T2_LINK_STATUS(link) | s0 ; 
    if ( mate != NULL && s1 != 0 ) {
        T2_LINK_STATUS(mate) = T2_LINK_STATUS(mate) & ~T2_LINK_UNDEF ; 
        T2_LINK_STATUS(mate) = T2_LINK_STATUS(mate) | s1 ; 
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_mark_link_transverse ( link, in_out ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LINK link ;
BOOLEAN in_out ;
{
    if ( in_out ) {
        t2b_mark_link_status ( link, T2_LINK_PREV_IN, T2_LINK_PREV_OUT ) ;
        t2b_mark_link_status ( link, T2_LINK_NEXT_OUT, T2_LINK_NEXT_IN ) ;
    }
    else {
        t2b_mark_link_status ( link, T2_LINK_PREV_OUT, T2_LINK_PREV_IN ) ;
        t2b_mark_link_status ( link, T2_LINK_NEXT_IN, T2_LINK_NEXT_OUT ) ;
    }
}

#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

