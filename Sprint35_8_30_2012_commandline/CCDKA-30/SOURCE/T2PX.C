/* -Z __BBS_MILL__=1 */
/********************************* T2PX.C **********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2adefs.h>
#include <c2ddefs.h>
#include <dmldefs.h>
#include <t2xdefs.h>
#include <t2cdefs.h>
#include <t2cpriv.h>
#include <t2ddefs.h> 
#include <t2dpriv.h> 
#include <t2idefs.h>
#include <t2pdefs.h>
#include <t2attrd.h>
#include <c2vdefs.h>
#include <c2vmcrs.h>

#if ( __BBS_MILL__>=1 )

STATIC DML_LIST t2p_zigzag_pass1 __(( REAL, REAL, PT2, DML_LIST, 
            T2_EDGE, PT2 )) ;
STATIC DML_LIST t2p_zigzag_pass2 __(( DML_LIST )) ;
STATIC BOOLEAN t2p_zigzag_pass3 __(( DML_LIST )) ;
STATIC void t2p_startpt_inside __(( T2_LOOP, DML_LIST, PT2, 
            PT2, T2_PARM, PT2, T2_PARM, BOOLEAN )) ;
STATIC T2_LOOP t2p_startpt_outside __(( DML_LIST, PT2, PT2 )) ;
STATIC void t2p_startpt_bndry __(( T2_LOOP, DML_LIST, T2_PARM, PT2 )) ;
STATIC void t2p_startpt_bndry1 __(( T2_LOOP, DML_LIST, T2_PARM, PT2 )) ;
STATIC T2_LOOP t2p_startpt1 __(( DML_LIST, PT2, T2_PT_POSITION* )) ;
STATIC T2_EDGE t2p_zigzag_loop __(( DML_LIST, REAL, T2_EDGE, 
            PT2, T2_DIR*, T2_DIR*, DML_LIST )) ;
STATIC T2_EDGE t2p_zigzag_loop_1 __(( DML_LIST, REAL, T2_EDGE, 
            PT2, T2_DIR*, T2_DIR*, DML_LIST )) ;
STATIC T2_EDGE t2p_zigzag_loop_2 __(( DML_LIST, REAL, T2_EDGE, 
            PT2, T2_DIR*, T2_DIR*, DML_LIST )) ;
STATIC T2_EDGE t2p_zigzag_loop_3 __(( DML_LIST, REAL, T2_EDGE, 
            PT2, T2_DIR*, T2_DIR*, DML_LIST )) ;
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif

/*----------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2p_zigzag ( region, step, rad, angle, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
REAL step, rad, angle ;
PT2 pt ;
{
    PT2 pt1, origin ;
    DML_LIST zigzag_list, looplist ;
    DML_ITEM item ;
    T2_EDGE rough_edge ;
    PT2 p ;

    t2c_join_lines_region ( region ) ;
    if ( !IS_SMALL(angle) ) {
        c2a_box_get_ctr ( T2_REGION_BOX(region), origin ) ;
        t2c_rotate_region ( region, origin, -angle ) ;
        if ( pt != NULL ) 
            c2v_rotate_pt ( pt, origin, -angle, pt1 ) ;
    }
    else if ( pt != NULL ) 
        C2V_COPY ( pt, pt1 ) ;
#ifdef CCDK_DEBUG
if ( DIS_LEVEL == -2 ) {
    t2c_write_region ( "zigrot.dat", region ) ;
    t2c_write_region_attr ( "zigrp.dat", region, T2_ATTR_PART ) ;
    t2c_write_region_attr ( "zigrr.dat", region, T2_ATTR_ROUGH ) ;
}
#endif

    looplist = t2d_break_region ( region, (ATTR)0, (ATTR)0, NULL ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_region ( region, 10 ) ;
    paint_looplist ( looplist, 11 ) ;
}
else
    DISPLAY-- ;
#endif
    if ( looplist == NULL ) { 
        if ( !IS_SMALL(angle) ) 
            t2c_rotate_region ( region, origin, angle ) ;
        RETURN ( NULL ) ;
    }

    rough_edge = t2p_rough_edge ( region, looplist, pt, p ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    if ( rough_edge != NULL ) 
        paint_edge ( rough_edge, 12 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
    zigzag_list = t2p_zigzag_pass1 ( step, rad, pt==NULL?NULL:pt1, 
        looplist, rough_edge, p ) ;
    if ( !IS_SMALL(angle) ) {
        t2c_rotate_region ( region, origin, angle ) ;
        if ( zigzag_list != NULL ) {
            DML_WALK_LIST ( zigzag_list, item ) 
                t2c_rotate_loop ( DML_RECORD(item), origin, angle ) ;
        }
    }

    dml_destroy_list ( looplist, ( PF_ACTION ) t2d_free_loop ) ;
    RETURN ( t2p_zigzag_pass2 ( zigzag_list ) ) ;
}


/*----------------------------------------------------------------------*/
STATIC DML_LIST t2p_zigzag_pass1 ( step, rad, pt, looplist, rough_edge, p ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL step, rad ;
PT2 pt ;
DML_LIST looplist ;
T2_EDGE rough_edge ;
PT2 p ;
{
    T2_EDGE edge ;
    T2_DIR dir = T2_DIR_UNDEF, dir_last = T2_DIR_UNDEF ;
    PT2 ept ;
    DML_LIST zigzag_list ;
    T2_PT_POSITION pos = T2_PT_UNKNOWN ;

    t2p_mark_refs ( looplist ) ;
    dml_apply ( looplist, ( PF_ACTION ) t2p_mark_loop ) ;
    zigzag_list = dml_create_list() ;

    if ( pt == NULL ) {
        if ( rough_edge == NULL ) {
            pos = t2p_startpt ( looplist, pt, p ) ;
            if ( pos == T2_PT_UNKNOWN )
                RETURN ( NULL ) ;
        }
        else {
            pt = p ;
        }
    }
    else {
        if ( rough_edge == NULL ) {
            pos = t2p_startpt ( looplist, pt, p ) ;
            if ( pos == T2_PT_OUTSIDE ) 
                t2p_append_line ( pt[0], pt[1], p[0], p[1], 
                    zigzag_list, ept ) ;
        }
        else {
            t2p_zigzag_rough_init ( rad, pt, p, zigzag_list, ept ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( rough_edge, 12 ) ;
    paint_line ( pt, p, 13 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
        }
    }

    t2p_mark_refs ( looplist ) ;

    edge = t2p_zigzag_first ( looplist ) ;
    while ( edge != NULL ) 
        edge = t2p_zigzag_loop ( looplist, step, edge, 
            ept, &dir, &dir_last, zigzag_list ) ; 

    RETURN ( zigzag_list ) ;
}


/*----------------------------------------------------------------------*/
STATIC DML_LIST t2p_zigzag_pass2 ( zigzag_list ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST zigzag_list ;
{
    BOOLEAN done = FALSE ;
    while ( !done ) 
        done = t2p_zigzag_pass3 ( zigzag_list ) ;
    RETURN ( zigzag_list ) ;
}


/*----------------------------------------------------------------------*/
STATIC BOOLEAN t2p_zigzag_pass3 ( zigzag_list ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST zigzag_list ;
{
    DML_ITEM item0, item1 ;
    T2_LOOP zigzag0, zigzag1 ;
    PT2 p, q ;
    BOOLEAN done = TRUE ;

    if ( zigzag_list == NULL ) 
        RETURN ( TRUE ) ;
    DML_WALK_LIST ( zigzag_list, item0 ) {
        zigzag0 = DML_RECORD(item0) ;
        t2c_loop_ept1 ( zigzag0, p ) ;
        for ( item1=DML_NEXT(item0) ; item1!=NULL ; item1=DML_NEXT(item1) ) {
            zigzag1 = DML_RECORD(item1) ;
            if ( t2c_loop_ept0 ( zigzag1, q ) && C2V_IDENT_PTS ( p, q ) ) 
                done = FALSE ;
            else {
                if ( t2c_loop_ept1 ( zigzag1, q ) && C2V_IDENT_PTS ( p, q ) ) {
                    t2c_reverse_loop ( zigzag1 ) ;
                    done = FALSE ;
                }
            }
            if ( !done ) {
                t2d_append_loop ( zigzag0, zigzag1 ) ;
                dml_remove_item ( zigzag_list, item1 ) ;
                t2d_free_loop ( zigzag1 ) ;
                RETURN ( FALSE ) ;
            }
        }
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_PT_POSITION t2p_startpt ( looplist, pt, p0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
PT2 pt, p0 ;
{
    T2_LOOP loop0 ;
    PT2 a, pt0, pt1 ;
    T2_PT_POSITION pos ;
    T2_PARM_S parm0, parm1 ;

    if ( looplist == NULL || DML_LENGTH(looplist) == 0 ) 
        RETURN ( T2_PT_UNKNOWN ) ;
    if ( pt != NULL ) {
        loop0 = t2p_startpt1 ( looplist, pt, &pos ) ;
        C2V_COPY ( pt, a ) ;
    }
    else { 
        loop0 = DML_FIRST_RECORD(looplist) ;
        t2c_pt_in_loop ( loop0, a ) ;
        pos = T2_PT_INSIDE ;
    }
    if ( loop0 == NULL && pos == T2_PT_OUTSIDE ) {
        loop0 = t2p_startpt_outside ( looplist, a, p0 ) ;
        RETURN ( loop0 == NULL ? T2_PT_UNKNOWN : T2_PT_OUTSIDE ) ;
    }

    if ( pos == T2_PT_INSIDE && t2i_loop_ray ( loop0, a, 0.0, pt0, &parm0 ) 
        && t2i_loop_ray ( loop0, a, PI, pt1, &parm1 ) ) {
        t2p_startpt_inside ( loop0, looplist, a, pt0, &parm0, 
            pt1, &parm1, TRUE ) ;
        RETURN ( pos ) ;
    }

    else if ( pos == T2_PT_ON_BOUNDARY ) {
        if ( !t2c_project_loop ( loop0, a, TRUE, &parm0, NULL ) )
            RETURN ( T2_PT_UNKNOWN ) ;
        t2p_startpt_bndry ( loop0, looplist, &parm0, a ) ;
        RETURN ( T2_PT_ON_BOUNDARY ) ;
    }
    RETURN ( T2_PT_UNKNOWN ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2p_startpt_inside ( loop0, looplist, pt, pt0, parm0, 
    pt1, parm1, tan_pos ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0 ;
DML_LIST looplist ;
PT2 pt, pt0, pt1 ;
T2_PARM parm0, parm1 ;
BOOLEAN tan_pos ;
{
    T2_LOOP loop1, loop2 ;
    T2_EDGE edge10, edge11, edge20, edge21 ;
    C2_CURVE line ;
    DML_LIST edgelist ;
    BOOLEAN one_edge ;

    dml_remove_data ( looplist, loop0 ) ;
    one_edge = ( pt == NULL ) || 
        C2V_IDENT_PTS ( pt0, pt ) || C2V_IDENT_PTS ( pt1, pt ) ;

    edgelist = dml_create_list();
    line = one_edge ? c2d_line ( pt1, pt0 ) : c2d_line ( pt, pt0 ) ;
    edge10 = t2d_create_edge ( NULL, line, 1 ) ;
    T2_EDGE_ATTR(edge10) = T2_EDGE_ATTR(edge10) | T2_ATTR_BOTTOM ; 
    T2_EDGE_ATTR(edge10) = T2_EDGE_ATTR(edge10) | T2_ATTR_FIRST ; 
    dml_append_data ( edgelist, edge10 ) ;
    t2d_append_trim ( T2_PARM_EDGE(parm0), T2_PARM_CPARM(parm0), 
        T2_PARM_EDGE(parm1), T2_PARM_CPARM(parm1), T2_ATTR_INIT, edgelist ) ;
    if ( one_edge ) 
        edge11 = NULL ;
    else {
        line = c2d_line ( pt1, pt ) ;
        edge11 = t2d_create_edge ( NULL, line, 1 ) ;
        T2_EDGE_ATTR(edge11) = T2_EDGE_ATTR(edge11) | T2_ATTR_BOTTOM ; 
        T2_EDGE_ATTR(edge11) = T2_EDGE_ATTR(edge11) | T2_ATTR_FIRST ; 
        dml_append_data ( edgelist, edge11 ) ;
    }
    loop1 = t2d_create_loop ( NULL, edgelist ) ;
    T2_LOOP_ATTR(loop1) = T2_LOOP_ATTR(loop0) | T2_ATTR_BOTTOM ;
    T2_LOOP_ATTR(loop1) = T2_LOOP_ATTR(loop1) | T2_ATTR_FIRST ;
    T2_LOOP_PARENT(loop1) = loop0 ;

    edgelist = dml_create_list();
    line = one_edge ? c2d_line ( pt0, pt1 ) : c2d_line ( pt, pt1 ) ;
    edge20 = t2d_create_edge ( NULL, line, 1 ) ;
    T2_EDGE_ATTR(edge20) = T2_EDGE_ATTR(edge20) | T2_ATTR_TOP ; 
    T2_EDGE_ATTR(edge20) = T2_EDGE_ATTR(edge20) | T2_ATTR_FIRST ; 
    dml_append_data ( edgelist, edge20 ) ;
    t2d_append_trim ( T2_PARM_EDGE(parm1), T2_PARM_CPARM(parm1), 
        T2_PARM_EDGE(parm0), T2_PARM_CPARM(parm0), T2_ATTR_INIT, edgelist ) ;
    if ( one_edge ) 
        edge21 = NULL ;
    else {
        line = c2d_line ( pt0, pt ) ;
        edge21 = t2d_create_edge ( NULL, line, 1 ) ;
        T2_EDGE_ATTR(edge21) = T2_EDGE_ATTR(edge21) | T2_ATTR_TOP ; 
        T2_EDGE_ATTR(edge21) = T2_EDGE_ATTR(edge21) | T2_ATTR_FIRST ; 
        dml_append_data ( edgelist, edge21 ) ;
    }
    loop2 = t2d_create_loop ( NULL, edgelist ) ;
    T2_LOOP_ATTR(loop2) = T2_LOOP_ATTR(loop0) ;
    T2_LOOP_ATTR(loop2) = T2_LOOP_ATTR(loop0) | T2_ATTR_TOP ;
    T2_LOOP_ATTR(loop2) = T2_LOOP_ATTR(loop2) | T2_ATTR_FIRST ;
    T2_LOOP_PARENT(loop2) = loop0 ;

    if ( one_edge ) {
        T2_EDGE_REF(edge10) = edge20 ;
        T2_EDGE_REF(edge20) = edge10 ;
        if ( tan_pos ) 
            t2p_mark_edge_pr1 ( edge10 ) ;
        else 
            t2p_mark_edge_pr0 ( edge10 ) ;
    }
    else {
        T2_EDGE_REF(edge10) = edge21 ;
        T2_EDGE_REF(edge21) = edge10 ;
        T2_EDGE_REF(edge11) = edge20 ;
        T2_EDGE_REF(edge20) = edge11 ;
        t2p_mark_edge_pr0 ( edge10 ) ;
        t2p_mark_edge_pr1 ( edge11 ) ;
    }
    dml_insert ( looplist, loop1, TRUE ) ;
    dml_insert ( looplist, loop2, FALSE ) ;
    t2d_free_loop ( loop0 ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2p_startpt_bndry ( loop0, looplist, parm0, a ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0 ;
DML_LIST looplist ;
T2_PARM parm0 ;
PT2 a ;
{
    T2_EDGE edge0, edge1 ;
    T2_PARM_S parm1 ;
    PT2 tan0, tan1, tan, pt1 ; 

    edge0 = T2_PARM_EDGE(parm0) ;

    if ( IS_ZERO ( T2_PARM_T(parm0) - T2_EDGE_T0(edge0) ) ) {
        edge1 = t2c_prev_edge ( edge0 ) ;
        if ( edge1 != NULL ) {
            if ( t2p_hor_edge ( edge1 ) ||
                ( t2c_etan0 ( edge0, tan0 ) && c2v_normalize ( tan0, tan0 ) &&
                  t2c_etan1 ( edge1, tan1 ) && c2v_normalize ( tan1, tan1 ) &&
                    ( fabs(tan1[1]) < fabs(tan0[1]) ) ) ) {
                T2_PARM_EDGE(parm0) = edge1 ;
                PARM_COPY ( T2_EDGE_PARM1(edge1), T2_PARM_CPARM(parm0) ) ;
            }
        }
    }

    else if ( IS_ZERO ( T2_PARM_T(parm0) - T2_EDGE_T1(edge0) ) ) {
        edge1 = t2c_next_edge ( edge0 ) ;
        if ( edge1 != NULL ) {
            if ( t2p_hor_edge ( edge1 ) ||
                ( t2c_etan1 ( edge0, tan0 ) && c2v_normalize ( tan0, tan0 ) &&
                  t2c_etan0 ( edge1, tan1 ) && c2v_normalize ( tan1, tan1 ) &&
                    ( fabs(tan1[1]) < fabs(tan0[1]) ) ) ) {
                T2_PARM_EDGE(parm0) = edge1 ;
                PARM_COPY ( T2_EDGE_PARM0(edge1), T2_PARM_CPARM(parm0) ) ;
            }
        }
    }
    if ( ( T2_EDGE_ATTR ( T2_PARM_EDGE(parm0) ) & T2_ATTR_TOP ) ||
         ( T2_EDGE_ATTR ( T2_PARM_EDGE(parm0) ) & T2_ATTR_BOTTOM ) )
        t2p_startpt_bndry1 ( loop0, looplist, parm0, a ) ;
    if ( t2c_eval_tan ( T2_PARM_EDGE(parm0), T2_PARM_CPARM(parm0), tan ) 
        && c2v_normalize ( tan, tan ) ) {
        if ( tan[1] > BBS_ZERO ) {
            if ( t2i_loop_ray ( loop0, a, PI, pt1, &parm1 ) ) 
                t2p_startpt_inside ( loop0, looplist, NULL, a, parm0, 
                    pt1, &parm1, TRUE ) ;
            else
                t2p_startpt_extr ( loop0, looplist, parm0 ) ;
        }
        else if ( tan[1] < -BBS_ZERO ) {
            if ( t2i_loop_ray ( loop0, a, 0.0, pt1, &parm1 ) )
                t2p_startpt_inside ( loop0, looplist, NULL, pt1, &parm1, 
                    a, parm0, FALSE ) ;
            else
                t2p_startpt_extr ( loop0, looplist, parm0 ) ;
        }
        else 
            t2p_startpt_extr ( loop0, looplist, parm0 ) ;
    }
}


/*-------------------------------------------------------------------------*/
STATIC void t2p_startpt_bndry1 ( loop0, looplist, parm, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0 ;
DML_LIST looplist ;
T2_PARM parm ;
PT2 pt ;
{
    T2_LOOP loop1 ;
    DML_ITEM item ;
    T2_EDGE edge0, edge1, new0, new1 ;
    INT parm_end ;

    edge0 = T2_PARM_EDGE(parm) ;
    if ( IS_ZERO ( T2_PARM_T(parm) - T2_EDGE_T0(edge0) ) ) 
        parm_end = 1 ;
    else if ( IS_ZERO ( T2_PARM_T(parm) - T2_EDGE_T1(edge0) ) ) 
        parm_end = 2 ;
    else 
        parm_end = 0 ;

    edge1 = T2_EDGE_REF(edge0) ;
    loop1 = T2_EDGE_LOOP ( edge1 ) ;
    t2c_loop_rearrange_parm ( loop0, parm ) ;
    t2c_loop_rearrange_pt ( loop1, pt, TRUE, NULL ) ;
    if ( parm_end == 1 ) {
        T2_EDGE_ATTR(edge0) = T2_EDGE_ATTR(edge0) | T2_ATTR_FIRST ;
        T2_EDGE_ATTR(edge1) = T2_EDGE_ATTR(edge1) | T2_ATTR_FIRST ;
        t2p_mark_edge_pr0 ( edge0 ) ;
    }
    else if ( parm_end == 2 ) {
        T2_EDGE_ATTR(edge0) = T2_EDGE_ATTR(edge0) | T2_ATTR_FIRST ;
        T2_EDGE_ATTR(edge1) = T2_EDGE_ATTR(edge1) | T2_ATTR_FIRST ;
        t2p_mark_edge_pr1 ( edge0 ) ;
    }

    else {
        new0 = DML_FIRST_RECORD ( T2_LOOP_EDGE_LIST(loop0) ) ;
        new1 = DML_LAST_RECORD ( T2_LOOP_EDGE_LIST(loop1) ) ;
        T2_EDGE_ATTR(new0) = T2_EDGE_ATTR(edge0) | T2_ATTR_FIRST ;
        T2_EDGE_ATTR(new1) = T2_EDGE_ATTR(edge1) | T2_ATTR_FIRST ;
        T2_EDGE_REF(new0) = new1 ;
        T2_EDGE_REF(new1) = new0 ;
        t2p_mark_edge_pr0 ( new0 ) ;

        new0 = DML_LAST_RECORD ( T2_LOOP_EDGE_LIST(loop0) ) ;
        new1 = DML_FIRST_RECORD ( T2_LOOP_EDGE_LIST(loop1) ) ;
        T2_EDGE_ATTR(new0) = T2_EDGE_ATTR(edge0) | T2_ATTR_FIRST ;
        T2_EDGE_ATTR(new1) = T2_EDGE_ATTR(edge1) | T2_ATTR_FIRST ;
        T2_EDGE_REF(new0) = new1 ;
        T2_EDGE_REF(new1) = new0 ;
        t2p_mark_edge_pr1 ( new0 ) ;
    }
    item = dml_find_data ( looplist, loop0 ) ;
    dml_make_first ( looplist, item ) ;
    item = dml_find_data ( looplist, loop1 ) ;
    dml_make_last ( looplist, item ) ;
    T2_LOOP_ATTR(loop0) = T2_LOOP_ATTR(loop0) | T2_ATTR_FIRST ;
    T2_LOOP_ATTR(loop1) = T2_LOOP_ATTR(loop1) | T2_ATTR_FIRST ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_startpt_extr ( loop0, looplist, parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0 ;
DML_LIST looplist ;
T2_PARM parm ;
{
    T2_EDGE edge ;
    BOOLEAN end1 ;

    edge = T2_PARM_EDGE(parm) ;
    end1 = IS_ZERO ( T2_EDGE_T1(edge) - T2_PARM_T(parm) ) ;
    t2c_loop_rearrange_parm ( loop0, parm ) ;
    edge = DML_FIRST_RECORD ( T2_LOOP_EDGE_LIST(loop0) ) ;
    T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_ATTR_FIRST ;
    t2p_mark_edge_pr0 ( edge ) ;
    edge = DML_LAST_RECORD ( T2_LOOP_EDGE_LIST(loop0) ) ;
    T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_ATTR_FIRST ;
    t2p_mark_edge_pr1 ( edge ) ;
    T2_LOOP_ATTR(loop0) = T2_LOOP_ATTR(loop0) | T2_ATTR_FIRST ;

    if ( end1 ) 
        dml_make_first ( T2_LOOP_EDGE_LIST(loop0), 
            DML_LAST ( T2_LOOP_EDGE_LIST(loop0) ) ) ;
    dml_make_first ( looplist, dml_find_data ( looplist, loop0 ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_LOOP t2p_startpt1 ( looplist, pt, pos ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
PT2 pt ;
T2_PT_POSITION *pos ;
{
    DML_ITEM item ;
    T2_LOOP loop ;

    if ( pt == NULL || looplist == NULL ) 
        RETURN ( NULL ) ;
    DML_WALK_LIST ( looplist, item ) {
        loop = DML_RECORD(item) ;
        *pos = t2c_pt_pos_loop ( loop, pt ) ;
        if ( *pos == T2_PT_INSIDE || *pos == T2_PT_ON_BOUNDARY ) 
            RETURN ( loop ) ;
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_LOOP t2p_startpt_outside ( looplist, pt, p0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
PT2 pt, p0 ;
{
    DML_ITEM item ;
    T2_LOOP loop ;
    T2_PARM_S parm ;
    C2_CURVE infline = c2d_ray ( pt, 0.0 ) ;
    DML_LIST intlist = dml_create_list() ;
    REAL dist, dist_r = 0.0, dist_l = 0.0 ;
    T2_INT_REC ti, ti_l = NULL, ti_r = NULL ;
    T2_EDGE edge ;

    DML_WALK_LIST ( looplist, item ) {
        if ( t2c_edge_by_attr ( DML_RECORD(item), T2_ATTR_ROUGH ) != NULL )
            RETURN ( NULL ) ;
    }

    DML_WALK_LIST ( looplist, item ) {
        loop = DML_RECORD(item) ;
        t2i_loop_curve ( loop, infline, TRUE, intlist ) ;
    }

    DML_WALK_LIST ( intlist, item ) {
        ti = DML_RECORD(item) ;
        dist = T2_INT_REC_PT(ti)[0] - pt[0] ;
        if ( dist > 0.0 && ( ti_r == NULL || dist < dist_r ) ) {
            dist_r = dist ;
            ti_r = ti ;
        }
        if ( dist < 0.0 && ( ti_l == NULL || dist > dist_l ) ) {
            dist_l = dist ;
            ti_l = ti ;
        }
    }
    c2d_free_curve ( infline ) ;

    if ( ti_r != NULL ) {
        edge = T2_INT_REC_EDGE1(ti_r) ;
        if ( !( T2_EDGE_ATTR(edge) & T2_ATTR_ROUGH ) ) 
            ti_r = NULL ;
    }
    if ( ti_l != NULL ) {
        edge = T2_INT_REC_EDGE1(ti_l) ;
        if ( !( T2_EDGE_ATTR(edge) & T2_ATTR_ROUGH ) ) 
            ti_l = NULL ;
    }

    if ( ti_r == NULL ) 
        ti = ti_l ;
    else if ( ti_l == NULL ) 
        ti = ti_r ;
    else 
        ti = ( pt[0] - T2_INT_REC_PT(ti_l)[0] < 
            T2_INT_REC_PT(ti_r)[0] - pt[0] ) ? ti_l : ti_r ;

    if ( ti == NULL ) 
        loop = NULL ;
    else {
        C2V_COPY ( T2_INT_REC_PT(ti), p0 ) ;
        T2_PARM_EDGE(&parm) = T2_INT_REC_EDGE1(ti) ;
        PARM_COPY ( T2_INT_REC_PARM1(ti), T2_PARM_CPARM(&parm) ) ;
        loop = T2_EDGE_LOOP ( T2_PARM_EDGE(&parm) ) ;
        t2p_startpt_bndry ( loop, looplist, &parm, p0 ) ;
    }
    dml_destroy_list ( intlist, ( PF_ACTION ) t2i_free_int_rec ) ;
    RETURN ( loop ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2p_zigzag_loop ( looplist, h, edge0, ept, 
            dir, dir_last, zigzag_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
REAL h ;
T2_EDGE edge0 ;
PT2 ept ;
T2_DIR *dir, *dir_last ;
DML_LIST zigzag_list ;
{
    T2_EDGE edge1 ;

    T2_LOOP loop = T2_EDGE_LOOP(edge0) ;
    T2_LOOP_ATTR(loop) = T2_LOOP_ATTR(loop) | T2_ATTR_PROCESSED ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_loop ( loop, 12 ) ;
        getch ();
        paint_edge ( edge0, 11 ) ;
        getch ();
    }
    else
        DISPLAY-- ;
    if ( DIS_LEVEL == -3 ) {
        paint_loop_x ( loop ) ;
        t2c_write_loop ( "loop.dat", loop ) ;
        t2c_write_loop_attr ( "looppa.dat", loop, T2_ATTR_PART ) ;
        t2c_write_loop_attr ( "loopro.dat", loop, T2_ATTR_ROUGH ) ;
        getch ();
    }
#endif
    if ( T2_LOOP_ATTR(loop) & T2_ATTR_TOP ) {
        if ( T2_LOOP_ATTR(loop) & T2_ATTR_BOTTOM ) 
            edge1 = t2p_zigzag_loop_1 ( looplist, 
                h, edge0, ept, dir, dir_last, zigzag_list ) ;
        else
            edge1 = t2p_zigzag_loop_2 ( looplist, 
                h, edge0, ept, dir, dir_last, zigzag_list ) ;
    }
    else {
        if ( T2_LOOP_ATTR(loop) & T2_ATTR_BOTTOM ) 
            edge1 = t2p_zigzag_loop_2 ( looplist, 
                h, edge0, ept, dir, dir_last, zigzag_list ) ;
        else
            edge1 = t2p_zigzag_loop_3 ( looplist, 
                h, edge0, ept, dir, dir_last, zigzag_list ) ;
    }

    RETURN ( edge1 ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2p_zigzag_loop_1 ( looplist, h, edge0, ept, 
            dir, dir_last, zigzag_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
REAL h ;
T2_EDGE edge0 ;
PT2 ept ;
T2_DIR *dir, *dir_last ;
DML_LIST zigzag_list ;
{
    INT i, even ;
    T2_XHATCH xh ;
    BOOLEAN up, end_on_edge ;
    T2_DIR dir0 = *dir, dir1 = T2_DIR_UNDEF ;
    T2_LOOP loop ;
    T2_EDGE edge, edge1 ;

    loop = T2_EDGE_LOOP(edge0) ;
    dir0 = t2p_zigzag_dir0 ( edge0, *dir, *dir_last ) ;
    edge1 = t2p_other_edge ( loop, edge0, dir0 ) ;
    if ( *dir_last != T2_DIR_UNDEF && 
           ( T2_EDGE_ATTR(edge1) & T2_ATTR_FIRST ) ) 
        dir1 = ( *dir_last == T2_DIR_CCW ) ? T2_DIR_CW : T2_DIR_CCW ;
    else
        dir1 = t2p_zigzag_dir1 ( edge1 ) ;

    if ( dir0 == T2_DIR_UNDEF || dir1 == T2_DIR_UNDEF ) 
        even = -1 ;
    else 
        even = ( dir0 == dir1 ) ? 1 : 0 ;

    xh = t2p_strip ( loop, edge0, edge1, h, even, &up, &end_on_edge ) ;
    if ( xh == NULL ) 
        RETURN ( NULL ) ;
    if ( dir0 == T2_DIR_UNDEF ) { 
        if ( dir1 == T2_DIR_CCW ) 
            dir0 = T2_XHATCH_N(xh)%4 ? T2_DIR_CW : T2_DIR_CCW ;
        else
            dir0 = T2_XHATCH_N(xh)%4 ? T2_DIR_CCW : T2_DIR_CW ;
    }

    if ( *dir_last == T2_DIR_UNDEF && 
        ( T2_LOOP_ATTR(loop) & T2_ATTR_FIRST ) )
        *dir_last = dir0 ;
    if ( T2_EDGE_ATTR(edge0) & T2_ATTR_FIRST ) {
        edge = ( dir0 == T2_DIR_CCW ) ? 
            t2c_next_edge ( edge0 ) : t2c_prev_edge ( edge0 ) ;
        if ( T2_EDGE_ATTR(edge) & T2_ATTR_FIRST ) {
            if ( T2_EDGE_ATTR(edge) & T2_ATTR_PROCESSED ) ;
            else
                edge0 = edge ;
        }
    }

    t2p_zigzag_step0 ( xh, edge0, dir0, ept, zigzag_list ) ;

    for ( i=0 ; i<T2_XHATCH_N(xh)-2 ; i+=2 ) {
        dir0 = ( dir0 == T2_DIR_CCW ) ? T2_DIR_CW : T2_DIR_CCW ;
        t2p_zigzag_step ( xh, i, dir0, ept, zigzag_list ) ;
    }

    if ( dir1 == T2_DIR_UNDEF ) 
        dir1 = ( dir0 == T2_DIR_CCW ) ? T2_DIR_CW : T2_DIR_CCW ;
    edge1 = t2p_zigzag_step1 ( xh, edge0, edge1, dir1, end_on_edge, 
        ept, zigzag_list ) ;
    t2x_free ( xh ) ;
    xh = NULL ;
    *dir = end_on_edge ? dir1 : T2_DIR_UNDEF ;
    edge = t2p_check_horline ( loop, edge1, dir1, zigzag_list, ept ) ;
    if ( edge != NULL ) 
        edge1 = edge ;
    else 
        edge1 = t2p_check_edge1 ( edge1, loop, ept ) ;
    RETURN ( t2p_zigzag_next ( looplist, edge1, ept, dir ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2p_zigzag_loop_2 ( looplist, h, edge0, ept, 
            dir, dir_last, zigzag_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
REAL h ;
T2_EDGE edge0 ;
PT2 ept ;
T2_DIR *dir, *dir_last ;
DML_LIST zigzag_list ;
{
    INT i ;
    T2_XHATCH xh ;
    BOOLEAN up, end_on_edge ;
    T2_DIR dir0 = *dir, dir00 ;
    T2_LOOP loop ;
    T2_EDGE edge, edge1 ;

    loop = T2_EDGE_LOOP(edge0) ;
    dir0 = t2p_zigzag_dir0 ( edge0, *dir, *dir_last ) ;
    if ( dir0 == T2_DIR_UNDEF )
        dir0 = T2_DIR_CCW ;    /* arbitrary choice */
    dir00 = dir0 ;
    up = ( T2_LOOP_ATTR(loop) & T2_ATTR_TOP ) != 0 ;

    if ( T2_EDGE_ATTR(edge0) & T2_ATTR_FIRST ) {
        edge = ( dir0 == T2_DIR_CCW ) ? 
            t2c_next_edge ( edge0 ) : t2c_prev_edge ( edge0 ) ;
        if ( T2_EDGE_ATTR(edge) & T2_ATTR_FIRST ) {
            if ( T2_EDGE_ATTR(edge) & T2_ATTR_PROCESSED ) ;
            else
                edge0 = edge ;
        }
    }

    xh = t2p_strip ( loop, edge0, NULL, h, -1, &up, &end_on_edge ) ;
    if ( xh == NULL ) 
        RETURN ( NULL ) ;

    edge1 = t2p_other_edge ( loop, edge0, dir0 ) ;

    if ( *dir_last == T2_DIR_UNDEF && 
        ( T2_LOOP_ATTR(loop) & T2_ATTR_FIRST ) )
        *dir_last = dir0 ;

    t2p_zigzag_step0 ( xh, edge0, dir0, ept, zigzag_list ) ;

    for ( i=0 ; i<T2_XHATCH_N(xh)-2 ; i+=2 ) {
        dir0 = ( dir0 == T2_DIR_CCW ) ? T2_DIR_CW : T2_DIR_CCW ;
        t2p_zigzag_step ( xh, i, dir0, ept, zigzag_list ) ;
    }

    edge = t2p_zigzag_step1 ( xh, edge0, edge1, dir0, 
        end_on_edge, ept, zigzag_list ) ;
    if ( edge != NULL ) 
        edge1 = edge ;
    t2x_free ( xh ) ;
    xh = NULL ;
    *dir = T2_DIR_UNDEF ;

    edge = ( dir00 == T2_DIR_CCW ) ? 
        t2c_prev_edge ( edge0 ) : t2c_next_edge ( edge0 ) ;
    if ( ! ( T2_EDGE_ATTR(edge) & T2_ATTR_PROCESSED ) && 
        t2p_hor_edge ( edge ) ) 
        dir0 = ( dir00 == T2_DIR_CCW ) ? T2_DIR_CW : T2_DIR_CCW ;
    else if ( edge1 == NULL || !t2p_hor_edge ( edge1 ) )
        dir0 = ( dir0 == T2_DIR_CCW ) ? T2_DIR_CW : T2_DIR_CCW ;
    edge = t2p_check_horline ( loop, edge1, dir0, zigzag_list, ept ) ;
    if ( edge != NULL ) 
        edge1 = edge ;
    else 
        edge1 = t2p_check_edge1 ( edge1, loop, ept ) ;
    RETURN ( t2p_zigzag_next ( looplist, edge1, ept, dir ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2p_zigzag_loop_3 ( looplist, h, edge0, ept, 
            dir, dir_last, zigzag_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
REAL h ;
T2_EDGE edge0 ;
PT2 ept ;
T2_DIR *dir, *dir_last ;
DML_LIST zigzag_list ;
{
    INT i ;
    T2_XHATCH xh ;
    BOOLEAN up, end_on_edge ;
    T2_DIR dir0 = *dir ;
    T2_LOOP loop ;
    T2_EDGE edge, edge1 ;
    PT2 tan0, tan1 ;

    loop = T2_EDGE_LOOP(edge0) ;
    edge1 = t2p_other_edge ( loop, edge0, T2_DIR_UNDEF ) ;

    dir0 = t2p_zigzag_dir0 ( edge0, *dir, *dir_last ) ;
    t2c_loop_ept_tan0 ( loop, NULL, tan0 ) ;
    t2c_loop_ept_tan1 ( loop, NULL, tan1 ) ;
    c2v_normalize ( tan0, tan0 ) ;
    c2v_normalize ( tan1, tan1 ) ;
    if ( ( tan0[1] > BBS_ZERO ) && ( tan1[1] < - BBS_ZERO ) )
        up = TRUE ;
    else if ( ( tan0[1] < - BBS_ZERO ) && ( tan1[1] > BBS_ZERO ) )
        up = FALSE ;
    else if ( IS_ZERO(tan0[1]) )
        up = ( tan0[0] > 0.0 ) ;
    else 
        up = ( tan1[0] > 0.0 ) ;

    xh = t2p_strip ( loop, edge0, edge1, h, -1, &up, &end_on_edge ) ;
    if ( xh == NULL ) 
        RETURN ( NULL ) ;
    if ( *dir_last == T2_DIR_UNDEF && 
        ( T2_LOOP_ATTR(loop) & T2_ATTR_FIRST ) )
        *dir_last = dir0 ;
    if ( dir0 == T2_DIR_CW && ( T2_EDGE_ATTR(edge0) & T2_ATTR_FIRST ) ) {
        edge = t2c_prev_edge ( edge0 ) ;
        if ( T2_EDGE_ATTR(edge) & T2_ATTR_FIRST ) 
            edge0 = edge ;
    }
    if ( dir0 == T2_DIR_UNDEF ) 
        dir0 = T2_DIR_CCW ;
    t2p_zigzag_step0 ( xh, edge0, dir0, ept, zigzag_list ) ;

    for ( i=0 ; i<T2_XHATCH_N(xh)-2 ; i+=2 ) {
        dir0 = ( dir0 == T2_DIR_CCW ) ? T2_DIR_CW : T2_DIR_CCW ;
        t2p_zigzag_step ( xh, i, dir0, ept, zigzag_list ) ;
    }

    edge1 = t2p_zigzag_step1 ( xh, edge0, edge1, dir0, end_on_edge, 
        ept, zigzag_list ) ;
    t2x_free ( xh ) ;
    xh = NULL ;
    *dir = T2_DIR_UNDEF ;
    edge = t2p_check_horline ( loop, edge1, dir0, zigzag_list, ept ) ;
    if ( edge != NULL ) 
        edge1 = edge ;
    RETURN ( t2p_zigzag_next ( looplist, edge1, ept, dir ) ) ;
}
#endif /* __BBS_MILL__>=1 */

