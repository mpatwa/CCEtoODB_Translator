/* -S -Z -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2B3.C **********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989-1996        Building Block Software   !!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <m2cdefs.h>
#include <c2adefs.h>
#include <c2cdefs.h>
#include <c2vdefs.h>
#include <t2bdefs.h>
#include <t2bpriv.h>
#include <t2cdefs.h>
#include <t2cpriv.h>
#include <t2ddefs.h>
#include <t2dpriv.h>
#include <t2ipriv.h>
#include <t2link.h>
#include <c2vmcrs.h>
#include <c2mem.h>
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
EXTERN BOOLEAN DIR ;
#endif
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC INT t2b_intersect_edge __(( T2_EDGE, T2_EDGE, INT, BOOLEAN, 
            DML_LIST )) ;
STATIC BOOLEAN t2b_create_links __(( T2_EDGE, T2_EDGE, C2_INT_REC, INT, 
            BOOLEAN, INT*, BOOLEAN )) ;
STATIC T2_LOOP t2b_split_loop __(( T2_LOOP DUMMY0 )) ;
STATIC void t2b_delete_coinc_int_links __(( T2_LOOP DUMMY0 )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_intersect ( region1, region2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989-1996        Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region1, region2 ;
{
    DML_LIST intlist ;
    DML_ITEM item1, item2 ;

    intlist = dml_create_list () ;
    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region1), item1 ) 
        DML_WALK_LIST ( T2_REGION_LOOP_LIST(region2), item2 ) 
            t2b_intersect_loop ( DML_RECORD(item1), DML_RECORD(item2), 
                0, FALSE, intlist ) ;
    dml_free_list ( intlist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_intersect_loop_region ( loop, region, intlist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989-1996        Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_REGION region ;
DML_LIST intlist ;
{
    DML_ITEM item ;

    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) 
        t2b_intersect_loop ( loop, DML_RECORD(item), 0, FALSE, intlist ) ;
    dml_clear_list ( intlist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT t2b_intersect_loop ( loop1, loop2, adjacent, 
            inf_line2, intlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989-1996        Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop1, loop2 ;
INT adjacent ;
BOOLEAN inf_line2 ;
DML_LIST intlist ;
{
    DML_ITEM item0, item1, item2 ;
    INT m, n = 0 ;
    T2_EDGE edge1, edge2 ;
#ifdef CCDK_DEBUG
    INT i1, i2 ;

    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_loop ( loop1, 10 ) ;
        paint_loop ( loop2, 11 ) ;
        getch();
    }
    else
        DISPLAY-- ;
    i1 = 0 ;
#endif

    if ( !c2a_box_w_overlap ( T2_LOOP_BOX(loop1), T2_LOOP_BOX(loop2), 
        t2c_get_gap_coeff () * BBS_TOL ) ) 
        RETURN ( 0 ) ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop1), item1 ) {

#ifdef CCDK_DEBUG
        i1++ ;
        i2 = 0 ;
#endif
        edge1 = (T2_EDGE)DML_RECORD(item1) ;
        item0 = ( loop1 == loop2 ) ? 
            item1 : DML_FIRST(T2_LOOP_EDGE_LIST(loop2)) ; 
        DML_FOR_LOOP ( item0, item2 ) {
            edge2 = (T2_EDGE)DML_RECORD(item2) ;
#ifdef CCDK_DEBUG
            i2++ ;
/*
            if ( loop1 == loop2 )
                adjacent = ( i2 == i1 + 1 ) ? 4 : 0 ;
*/
            if ( DIS_LEVEL == -4 && i2 >= 136 && i2 <= 139 ) {
                paint_edge ( edge1, 10 ) ;
                paint_edge ( edge2, 11 ) ;
                getch () ;
            }
#endif
            m = t2b_intersect_edge ( edge1, edge2, adjacent, 
                inf_line2, intlist ) ;
            n += m ;
        }
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT t2b_intersect_edge ( edge1, edge2, adjacent, inf_line2, intlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989-1996        Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge1, edge2 ;
INT adjacent ;
BOOLEAN inf_line2 ;
DML_LIST intlist ;
{
    DML_ITEM item ;
    INT n ;
    C2_INT_REC ci ;
    BOOLEAN vtx_status = FALSE, no_coinc = FALSE, coinc = FALSE ;

#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    BOOLEAN dir = DIR ;
    DIR = TRUE ;
    paint_edge ( edge1, 10 ) ;
    paint_edge ( edge2, 11 ) ;
    getch();
    DIR = dir ;
}
else
    DISPLAY-- ;
if ( DIS_LEVEL == -4 ) {
    c2c_write_curve ( "edge1.dat", T2_EDGE_CURVE(edge1) ) ;
    c2c_write_curve ( "edge2.dat", T2_EDGE_CURVE(edge2) ) ;
}
{
    PT2 p1, p2, tan1, tan2 ;
    REAL cross, dot ;
    t2c_ept_tan1 ( edge1, p1, tan1 ) ;
    t2c_ept_tan0 ( edge2, p2, tan2 ) ;
    c2v_normalize ( tan1, tan1 ) ;
    c2v_normalize ( tan2, tan2 ) ;
    cross = C2V_CROSS ( tan1, tan2 ) ; 
    dot = C2V_DOT ( tan1, tan2 ) ; 
    if ( adjacent & 2 ) {
        t2c_ept_tan0 ( edge1, p1, tan1 ) ;
        t2c_ept_tan1 ( edge2, p2, tan2 ) ;
        c2v_normalize ( tan1, tan1 ) ;
        c2v_normalize ( tan2, tan2 ) ;
        cross = C2V_CROSS ( tan1, tan2 ) ; 
        dot = C2V_DOT ( tan1, tan2 ) ; 
    }
}
#endif

    if ( ( adjacent & 1 ) && edge1 == t2c_last_edge ( T2_EDGE_LOOP(edge1) ) &&
        edge2 == t2c_first_edge ( T2_EDGE_LOOP(edge2) ) ) {
        vtx_status = TRUE ;
        if ( T2_EDGE_VTX(edge1) == T2_VTX_CONCAVE )
            no_coinc = TRUE ;
    }
    if ( ( adjacent & 2 ) && edge1 == t2c_first_edge ( T2_EDGE_LOOP(edge1) ) 
        && ( edge2 == t2c_last_edge ( T2_EDGE_LOOP(edge2) ) ) ) {
        vtx_status = TRUE ;
        if ( T2_EDGE_VTX(edge2) == T2_VTX_CONCAVE )
            no_coinc = TRUE ;
    }
    if ( inf_line2 ) 
        n = c2c_intersect_ext ( T2_EDGE_CURVE(edge1), T2_EDGE_CURVE(edge2), 
            intlist ) ;
    else
        n = m2c_intersect ( T2_EDGE_CURVE(edge1), T2_EDGE_CURVE(edge2), 
            vtx_status, no_coinc, intlist ) ;

    DML_WALK_LIST ( intlist, item ) {
        ci = DML_RECORD(item) ;
        coinc = t2b_create_links ( edge1, edge2, ci, adjacent, inf_line2, 
            &n, coinc ) ;
        C2_FREE_INT_REC(ci) ;
    }
    dml_clear_list ( intlist ) ;
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_create_links ( edge1, edge2, ci, adjacent, inf_line2, 
            n_ptr, prev_coinc )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989-1996        Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge1, edge2 ;
C2_INT_REC ci ;
INT adjacent ;
BOOLEAN inf_line2 ;
INT *n_ptr ;
BOOLEAN prev_coinc ;
{
    INT s ;
    PT2 tan1, tan2 ;
    REAL cross ;
    BOOLEAN coinc, e10, e11, e20, e21 ;

    if ( T2_EDGE_LOOP(edge1) == T2_EDGE_LOOP(edge2) ) {
        if ( IS_ZERO ( C2_INT_REC_T1(ci) - T2_EDGE_T1(edge1) ) &&
             IS_ZERO ( C2_INT_REC_T2(ci) - T2_EDGE_T0(edge2) ) &&
                edge2 == t2c_next_edge ( edge1 ) ) {
            (*n_ptr)-- ;
            RETURN ( FALSE ) ;
        }
/* YG added the following on 11-29-92 
        else if ( ( C2_INT_REC_TYPE(ci) >= 2 ) &&
            fabs ( C2_INT_REC_T1(ci) - T2_EDGE_T1(edge1) ) <= 50.0*BBS_ZERO &&
            fabs ( C2_INT_REC_T2(ci) - T2_EDGE_T0(edge2) ) <= 50.0*BBS_ZERO &&
            edge2 == t2c_next_edge ( edge1 ) ) {
            (*n_ptr)-- ;
            RETURN ( FALSE ) ;
        }
*/
        if ( IS_ZERO ( C2_INT_REC_T1(ci) - T2_EDGE_T0(edge1) ) &&
             IS_ZERO ( C2_INT_REC_T2(ci) - T2_EDGE_T1(edge2) ) &&
                edge2 == t2c_prev_edge ( edge1 ) ) {
            (*n_ptr)-- ;
            RETURN ( FALSE ) ;
        }
    }

    if ( !inf_line2 && 
        ( C2_INT_REC_T2(ci) < - T2_EDGE_T_MIN(edge2) - BBS_ZERO ) ) {
        (*n_ptr)-- ;
        RETURN ( FALSE ) ;
    }

    e10 = IS_ZERO ( C2_INT_REC_T1(ci) - T2_EDGE_T0(edge1) ) ;
    e11 = IS_ZERO ( C2_INT_REC_T1(ci) - T2_EDGE_T1(edge1) ) ;
    e20 = IS_ZERO ( C2_INT_REC_T2(ci) - T2_EDGE_T0(edge2) ) ;
    e21 = IS_ZERO ( C2_INT_REC_T2(ci) - T2_EDGE_T1(edge2) ) ;

    if ( C2_INT_REC_TYPE(ci) > 0 ) {

        if ( e10 || e11 || e20 || e21 ) {
            if ( ( adjacent & 1 ) && ( e11 || e20 ) &&
                edge1 == t2c_last_edge ( T2_EDGE_LOOP(edge1) ) &&
                edge2 == t2c_first_edge ( T2_EDGE_LOOP(edge2) ) )
                s = T2_LINK_PREV_OUT ;
            else if ( ( adjacent & 2 ) && ( e10 || e21 ) &&
                edge1 == t2c_first_edge ( T2_EDGE_LOOP(edge1) ) &&
                edge2 == t2c_last_edge ( T2_EDGE_LOOP(edge2) ) )
                s = T2_LINK_NEXT_OUT ;
            else
                s = T2_LINK_UNDEF ;
        }
        else {
            if ( C2_INT_REC_TYPE(ci) == 1 && !C2_INT_REC_NEAR_TAN(ci) && 
                 t2c_eval_tan ( edge1, C2_INT_REC_PARM1(ci), tan1 ) &&
                 t2c_eval_tan ( edge2, C2_INT_REC_PARM2(ci), tan2 ) &&
                 c2v_normalize_l1 ( tan1, tan1 ) && 
                 c2v_normalize_l1 ( tan2, tan2 ) ) {
                cross = C2V_CROSS ( tan1, tan2 ) ;
                if ( cross > BBS_ZERO ) 
                    s = T2_LINK_NEXT_OUT ;
                else if ( cross < -BBS_ZERO ) 
                    s = T2_LINK_NEXT_IN ;
                else 
                    s = T2_LINK_UNDEF ;
            }
            else 
                s = T2_LINK_UNDEF ;
        }
        coinc = FALSE ;
    }

    else if ( C2_INT_REC_TYPE(ci) == -1 ) {
        s = T2_LINK_UNDEF ;
        coinc = FALSE ;
    }

    else { 
        if ( ( adjacent & 1 ) && ( e11 || e20 ) &&
            edge1 == t2c_last_edge ( T2_EDGE_LOOP(edge1) ) &&
            edge2 == t2c_first_edge ( T2_EDGE_LOOP(edge2) ) )
            s = T2_LINK_NEXT_IN ;
        else if ( ( adjacent & 2 ) && ( e10 || e21 ) &&
            edge1 == t2c_first_edge ( T2_EDGE_LOOP(edge1) ) &&
            edge2 == t2c_last_edge ( T2_EDGE_LOOP(edge2) ) )
            s = T2_LINK_PREV_IN ;

        else {
            t2c_eval_tan ( edge1, C2_INT_REC_PARM1(ci), tan1 ) ;
            t2c_eval_tan ( edge2, C2_INT_REC_PARM2(ci), tan2 ) ;
            coinc = !prev_coinc ;
            if ( T2_EDGE_DIR(edge1) == -1 ) 
                prev_coinc = !prev_coinc ;

            if ( C2V_DOT ( tan1, tan2 ) > 0.0 ) 
                s = prev_coinc ? T2_LINK_CODIR1 : T2_LINK_CODIR0 ; 
            else 
                s = prev_coinc ? T2_LINK_ANTIDIR1 : T2_LINK_ANTIDIR0 ; 
        }
    }
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( edge1, 10 ) ;
    paint_edge ( edge2, 11 ) ;
    paint_point ( C2_INT_REC_PT(ci), 0.02, 12 ) ;
    getch();
}
else
    DISPLAY-- ;
#endif
    t2b_create_link ( edge1, C2_INT_REC_PARM1(ci), 
        edge2, C2_INT_REC_PARM2(ci), s, C2_INT_REC_PT(ci) ) ;
    RETURN ( coinc ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_create_link ( edge1, parm1, edge2, parm2, s, pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989-1996        Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge1 ;
PARM parm1 ;
T2_EDGE edge2 ;
PARM parm2 ;
INT s ;
PT2 pt ;
{
    T2_LINK link, mate ;
    link = T2_ALLOC_LINK ;
    T2_LINK_EDGE(link) = edge1 ;
    COPY_PARM ( parm1, T2_LINK_CPARM(link) ) ;
    T2_LINK_STATUS(link) = 0 ;

    if ( edge2 != NULL ) {
        mate = T2_ALLOC_LINK ;
        T2_LINK_EDGE(mate) = edge2 ;
        COPY_PARM ( parm2, T2_LINK_CPARM(mate) ) ;
        T2_LINK_STATUS(mate) = 0 ;
    }
    else 
        mate = NULL ;

    T2_LINK_MATE(link) = mate ;
    if ( pt == NULL ) 
        t2c_eval_pt ( edge1, parm1, T2_LINK_PT(link) ) ;
    else {
        C2V_COPY ( pt, T2_LINK_PT(link) ) ;
    }

    if ( s == T2_LINK_UNDEF ) {
        T2_LINK_STATUS(link) = T2_LINK_UNDEF ;
        if ( mate != NULL ) 
            T2_LINK_STATUS(mate) = T2_LINK_UNDEF ;
    }
    else if ( s == T2_LINK_PREV_IN ) 
        t2b_mark_link_transverse ( link, TRUE ) ;
    else if ( s == T2_LINK_PREV_OUT ) 
        t2b_mark_link_transverse ( link, FALSE ) ;
    else if ( s == T2_LINK_NEXT_IN ) 
        t2b_mark_link_transverse ( link, FALSE ) ;
    else if ( s == T2_LINK_NEXT_OUT ) 
        t2b_mark_link_transverse ( link, TRUE ) ;
    else if ( s == T2_LINK_CODIR0 ) {
        T2_LINK_STATUS(link) = T2_LINK_CODIR0 ;
        if ( mate != NULL ) 
            T2_LINK_STATUS(mate) = T2_LINK_CODIR0 ;
    }
    else if ( s == T2_LINK_CODIR1 ) {
        T2_LINK_STATUS(link) = T2_LINK_CODIR1 ;
        if ( mate != NULL ) 
            T2_LINK_STATUS(mate) = T2_LINK_CODIR1 ;
    }
    else if ( s == T2_LINK_ANTIDIR0 ) {
        T2_LINK_STATUS(link) = T2_LINK_ANTIDIR0 ;
        if ( mate != NULL ) 
            T2_LINK_STATUS(mate) = T2_LINK_ANTIDIR1 ;
    }
    else if ( s == T2_LINK_ANTIDIR1 ) {
        T2_LINK_STATUS(link) = T2_LINK_ANTIDIR1 ;
        if ( mate != NULL ) 
            T2_LINK_STATUS(mate) = T2_LINK_ANTIDIR0 ;
    }

    t2c_insert_link ( T2_EDGE_LOOP(edge1), link ) ;

    if ( mate != NULL ) {
        T2_LINK_MATE(mate) = link ;
        if ( pt == NULL ) 
            t2c_eval_pt ( edge2, parm2, T2_LINK_PT(mate) ) ;
        else {
            C2V_COPY ( pt, T2_LINK_PT(mate) ) ;
        }
        t2c_insert_link ( T2_EDGE_LOOP(edge2), mate ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2b_split_loop_self_inters ( loop0, split_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989-1996        Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0 ;
DML_LIST split_list ;
{
    DML_LIST intlist = dml_create_list ();
    T2_LOOP loop ;

    if ( split_list == NULL ) 
        split_list = dml_create_list () ;

    t2b_intersect_loop ( loop0, loop0, 0, FALSE, intlist ) ;
    t2b_loop_link_epts ( loop0, NULL ) ;
    t2b_delete_coinc_int_links ( loop0 ) ;

    for ( loop = t2b_split_loop ( loop0 ) ; loop != NULL ;
          loop = t2b_split_loop ( loop0 ) ) 
        dml_append_data ( split_list, loop ) ;
    dml_free_list ( intlist ) ;
    RETURN ( split_list ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_delete_coinc_int_links ( loop )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989-1996        Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    DML_ITEM item ;
    INT s ;
    T2_LINK link, link0 ;

    if ( T2_LOOP_LINK_LIST(loop) == NULL ) 
        RETURN ;
    DML_WALK_LIST ( T2_LOOP_LINK_LIST(loop), item ) {
        link = (T2_LINK)DML_RECORD(item) ;
        s = T2_LINK_STATUS(link) ;
        if ( ( s & T2_LINK_CODIR0 ) || ( s & T2_LINK_CODIR1 ) || 
             ( s & T2_LINK_ANTIDIR0 ) || ( s & T2_LINK_ANTIDIR1 ) )
            t2b_mark_link_delete ( link, TRUE ) ;
    }

    link0 = NULL ;

    DML_WALK_LIST ( T2_LOOP_LINK_LIST(loop), item ) {
        link = (T2_LINK)DML_RECORD(item) ;
        s = T2_LINK_STATUS(link) ;
        if ( ( s & T2_LINK_CODIR0 ) || ( s & T2_LINK_CODIR1 ) || 
             ( s & T2_LINK_ANTIDIR0 ) || ( s & T2_LINK_ANTIDIR1 ) )
            link0 = link ;
        else if ( link0 != NULL ) {
            if ( C2V_IDENT_PTS ( T2_LINK_PT(link), T2_LINK_PT(link0) ) ) {
                if ( T2_LINK_STATUS(link0) & T2_LINK_DELETE ) 
                    t2b_mark_link_delete ( link, TRUE ) ;
            }
            else
                link0 = NULL ;
        }
    }

    link0 = NULL ;

    for ( item = DML_LAST(T2_LOOP_LINK_LIST(loop)) ; 
        item != NULL ; item = DML_PREV(item) ) {
        link = (T2_LINK)DML_RECORD(item) ;
        s = T2_LINK_STATUS(link) ;
        if ( ( s & T2_LINK_CODIR0 ) || ( s & T2_LINK_CODIR1 ) || 
             ( s & T2_LINK_ANTIDIR0 ) || ( s & T2_LINK_ANTIDIR1 ) )
            link0 = link ;
        else if ( link0 != NULL ) {
            if ( C2V_IDENT_PTS ( T2_LINK_PT(link), T2_LINK_PT(link0) ) ) {
                if ( T2_LINK_STATUS(link0) & T2_LINK_DELETE ) 
                    t2b_mark_link_delete ( link, TRUE ) ;
            }
            else
                link0 = NULL ;
        }
    }

    t2b_delete_links ( loop, TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_LOOP t2b_split_loop ( loop0 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989-1996        Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0 ;
{
    T2_LINK link0, link1, link2 ;
    DML_LIST edgelist ;

    if ( T2_LOOP_LINK_LIST(loop0) == NULL || 
        DML_LENGTH(T2_LOOP_LINK_LIST(loop0)) == 0 ) 
        RETURN ( NULL ) ;

    edgelist = dml_create_list () ;
    link0 = DML_FIRST_RECORD ( T2_LOOP_LINK_LIST(loop0) ) ;
    link1 = link0 ;

    while ( TRUE ) {
        link2 = t2i_next_link ( link1 ) ;
        if ( link2 == NULL ) 
            RETURN ( NULL ) ;
        t2d_append_trim ( T2_LINK_EDGE(link1), T2_LINK_CPARM(link1), 
            T2_LINK_EDGE(link2), T2_LINK_CPARM(link2), (ATTR)0, edgelist ) ;
        link1 = T2_LINK_MATE(link2) ;
        if ( link1 == link0 ) {
            t2i_delete_link ( link1 ) ;
            RETURN ( t2d_create_loop ( T2_LOOP_REGION(loop0), edgelist ) ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_intersect_looplist ( looplist, inters_self, 
            first_last_adjacent )                   
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989-1996        Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
BOOLEAN inters_self, first_last_adjacent ;
{
    DML_LIST intlist ;
    DML_ITEM item0, item1, init_item ;
    T2_LOOP loop0, loop1 ;
    INT adjacent ;
    PT2 p0, p1 ;

    intlist = dml_create_list () ;
    DML_WALK_LIST ( looplist, item0 ) { 
        loop0 = (T2_LOOP)DML_RECORD(item0) ;
        init_item = inters_self ? item0 : dml_next(item0) ;
        DML_FOR_LOOP ( init_item, item1 ) {
            loop1 = (T2_LOOP)DML_RECORD(item1) ;
            adjacent = 0 ;
#ifdef OLD_CODE
            if ( item1 == DML_NEXT(item0) && t2c_loop_ept1 ( loop0, p0 ) && 
                t2c_loop_ept0 ( loop1, p1 ) && C2V_IDENT_PTS ( p0, p1 ) )
                    adjacent += 1 ;
#endif
            if ( item1 == DML_NEXT(item0) )
                    adjacent += 1 ;
/*
            if ( item0 == DML_FIRST(looplist) && 
                item1 == DML_LAST(looplist) && item0 != item1 && 
                t2c_loop_ept0 ( loop0, p0 ) && t2c_loop_ept1 ( loop1, p1 ) && 
                ( C2V_DIST ( p0, p1 ) <= 100.0 * BBS_TOL ) )
                    adjacent += 2 ;
*/
            if ( item0 == DML_FIRST(looplist) && 
                item1 == DML_LAST(looplist) && item0 != item1 ) {
                if ( first_last_adjacent )
                    adjacent += 2 ;
                else if ( t2c_loop_ept0 ( loop0, p0 ) && 
                    t2c_loop_ept1 ( loop1, p1 ) ) {
                    if ( C2V_IDENT_PTS ( p0, p1 ) ) 
                        adjacent += 2 ;
                }
            }

            t2b_intersect_loop ( loop0, loop1, adjacent, FALSE, intlist ) ;
        }
    }

    dml_free_list ( intlist ) ;
    intlist = NULL ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

