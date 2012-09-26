/* -Z __BBS_MILL__=2 */
/********************************* T2PU.C **********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2adefs.h>
#include <c2ddefs.h>
#include <dmldefs.h>
#include <t2cdefs.h>
#include <t2cpriv.h>
#include <t2ddefs.h> 
#include <t2pdefs.h>
#include <t2xdefs.h>
#include <t2attrd.h>
#include <c2vdefs.h>
#include <c2vmcrs.h>
#if ( __BBS_MILL__>=2 )

STATIC DML_LIST t2p_unidir_pass1 __(( REAL, PT2, DML_LIST )) ;
STATIC T2_EDGE t2p_unidir_loop __(( DML_LIST, REAL, T2_EDGE, PT2, DML_LIST )) ;
STATIC T2_EDGE t2p_unidir_loop_1 __(( DML_LIST, REAL, T2_EDGE, 
            PT2, DML_LIST )) ;
STATIC T2_EDGE t2p_unidir_loop_2 __(( DML_LIST, REAL, T2_EDGE, 
            PT2, DML_LIST )) ;
STATIC T2_EDGE t2p_unidir_loop_3 __(( DML_LIST, REAL, T2_EDGE, 
            PT2, DML_LIST )) ;
STATIC void t2p_unidir_step_bndr0 __(( T2_XHATCH, INT, PT2, DML_LIST )) ;
STATIC void t2p_unidir_step_bndr1 __(( T2_XHATCH, INT, PT2, DML_LIST )) ;
STATIC BOOLEAN t2p_unidir_next1 __(( T2_EDGE, T2_EDGE*, PT2, REAL*, INT )) ;
STATIC T2_EDGE t2p_unidir_first __(( DML_LIST )) ;
STATIC T2_EDGE t2p_unidir_first_edge __(( T2_LOOP )) ;
STATIC void t2p_unidir_step0 __(( T2_XHATCH, T2_EDGE, PT2, DML_LIST )) ;
STATIC void t2p_unidir_step __(( T2_XHATCH, INT, PT2, DML_LIST )) ;
STATIC T2_EDGE t2p_unidir_step1 __(( T2_XHATCH, T2_EDGE, T2_EDGE, 
            BOOLEAN, PT2, DML_LIST )) ;
STATIC void t2p_unidir_step_line __(( T2_XHATCH, INT, PT2, DML_LIST )) ;
STATIC T2_EDGE t2p_unidir_next __(( DML_LIST, T2_EDGE, PT2 )) ;
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
EXTERN BOOLEAN DIR ;
#endif

/*----------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2p_unidir ( region, step, angle, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
REAL step, angle ;
PT2 pt ;
{
    PT2 pt1, origin ;
    DML_LIST unidir_list, looplist ;
    DML_ITEM item ;

    if ( !IS_SMALL(angle) ) {
        c2a_box_get_ctr ( T2_REGION_BOX(region), origin ) ;
        t2c_rotate_region ( region, origin, -angle ) ;
        if ( pt != NULL ) 
            c2v_rotate_pt ( pt, origin, -angle, pt1 ) ;
    }
    else if ( pt != NULL ) 
        C2V_COPY ( pt, pt1 ) ;

    looplist = t2d_break_region ( region, (ATTR)0, (ATTR)0, NULL ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
paint_region ( region, 10 ) ;
DML_WALK_LIST ( looplist, item ) {
    paint_loop ( DML_RECORD(item), 11 ) ;
    getch ();
}
}
else
    DISPLAY-- ;
#endif
    if ( looplist == NULL ) { 
        if ( !IS_SMALL(angle) ) 
            t2c_rotate_region ( region, origin, angle ) ;
        RETURN ( NULL ) ;
    }

    unidir_list = t2p_unidir_pass1 ( step, pt==NULL?NULL:pt1, looplist ) ;
    if ( !IS_SMALL(angle) ) {
        t2c_rotate_region ( region, origin, angle ) ;
        if ( unidir_list != NULL ) {
            DML_WALK_LIST ( unidir_list, item ) 
                t2c_rotate_loop ( DML_RECORD(item), origin, angle ) ;
        }
    }

    dml_destroy_list ( looplist, ( PF_ACTION ) t2d_free_loop ) ;
#ifdef CCDK_DEBUG
{
DML_ITEM item ;
BOOLEAN dir = DIR ;
DISPLAY++ ;
DIR = TRUE ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    DML_WALK_LIST ( unidir_list, item ) {
        paint_loop ( DML_RECORD(item), 12 ) ;
        getch ();
    }
}
else
    DISPLAY-- ;
DIR = dir ;
}
#endif
    RETURN ( unidir_list ) ;
}


/*----------------------------------------------------------------------*/
STATIC DML_LIST t2p_unidir_pass1 ( step, pt, looplist ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL step ;
PT2 pt ;
DML_LIST looplist ;
{
    T2_EDGE edge ;
    PT2 ept, p ;
    DML_LIST unidir_list ;
    T2_PT_POSITION pos ;

    t2p_mark_refs ( looplist ) ;
    dml_apply ( looplist, ( PF_ACTION ) t2p_mark_loop ) ;
    pos = t2p_startpt ( looplist, pt, p ) ;
    if ( pos == T2_PT_UNKNOWN ) 
        RETURN ( NULL ) ;

    unidir_list = dml_create_list() ;
    if ( pos == T2_PT_OUTSIDE ) {
        edge = t2d_create_edge ( NULL, c2d_line ( pt, p ), 1 ) ;
        t2p_append_edge ( unidir_list, ept, edge ) ;
    }
    t2p_mark_refs ( looplist ) ;

    edge = t2p_unidir_first ( looplist ) ;
    while ( edge != NULL ) 
        edge = t2p_unidir_loop ( looplist, step, edge, ept, unidir_list ) ; 

    RETURN ( unidir_list ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2p_unidir_loop ( looplist, h, edge0, ept, unidir_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
REAL h ;
T2_EDGE edge0 ;
PT2 ept ;
DML_LIST unidir_list ;
{
    T2_LOOP loop = T2_EDGE_LOOP(edge0) ;
    T2_LOOP_ATTR(loop) = T2_LOOP_ATTR(loop) | T2_ATTR_PROCESSED ;

#ifdef CCDK_DEBUG
{
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop, 12 ) ;
    getch ();
}
else
    DISPLAY-- ;
}
#endif
    if ( T2_LOOP_ATTR(loop) & T2_ATTR_TOP ) {
        if ( T2_LOOP_ATTR(loop) & T2_ATTR_BOTTOM ) 
            RETURN ( t2p_unidir_loop_1 ( looplist, 
                h, edge0, ept, unidir_list ) ) ;
        else
            RETURN ( t2p_unidir_loop_2 ( looplist, 
                h, edge0, ept, unidir_list ) ) ;
    }
    else {
        if ( T2_LOOP_ATTR(loop) & T2_ATTR_BOTTOM ) 
            RETURN ( t2p_unidir_loop_2 ( looplist, 
                h, edge0, ept, unidir_list ) ) ;
        else
            RETURN ( t2p_unidir_loop_3 ( looplist, 
                h, edge0, ept, unidir_list ) ) ;
    }
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2p_unidir_loop_1 ( looplist, h, edge0, ept, unidir_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
REAL h ;
T2_EDGE edge0 ;
PT2 ept ;
DML_LIST unidir_list ;
{
    INT i ;
    T2_XHATCH xh ;
    BOOLEAN up, end_on_edge ;
    T2_LOOP loop ;
    T2_EDGE edge, edge1 ;

    loop = T2_EDGE_LOOP(edge0) ;
    edge1 = t2p_other_edge ( loop, edge0, T2_DIR_UNDEF ) ;

    xh = t2p_strip ( loop, edge0, edge1, h, -1, &up, &end_on_edge ) ;
    if ( xh == NULL ) 
        RETURN ( NULL ) ;
    if ( T2_EDGE_ATTR(edge0) & T2_ATTR_FIRST ) {
        edge = t2c_next_edge ( edge0 ) ;
        if ( T2_EDGE_ATTR(edge) & T2_ATTR_FIRST ) {
            if ( T2_EDGE_ATTR(edge) & T2_ATTR_PROCESSED ) ;
            else
                edge0 = edge ;
        }
    }

    t2p_unidir_step0 ( xh, edge0, ept, unidir_list ) ;

    for ( i=0 ; i<T2_XHATCH_N(xh)-2 ; i+=2 ) 
        t2p_unidir_step ( xh, i, ept, unidir_list ) ;

    edge1 = t2p_unidir_step1 ( xh, edge0, edge1, end_on_edge, 
        ept, unidir_list ) ;
    t2x_free ( xh ) ;
    xh = NULL ;
    edge = t2p_check_horline ( loop, edge1, T2_DIR_CCW, unidir_list, ept ) ;
    if ( edge != NULL ) 
        edge1 = edge ;
    else 
        edge1 = t2p_check_edge1 ( edge1, loop, ept ) ;
    RETURN ( t2p_unidir_next ( looplist, edge1, ept ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2p_unidir_loop_2 ( looplist, h, edge0, ept, unidir_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
REAL h ;
T2_EDGE edge0 ;
PT2 ept ;
DML_LIST unidir_list ;
{
    INT i ;
    T2_XHATCH xh ;
    BOOLEAN up, end_on_edge ;
    T2_LOOP loop ;
    T2_EDGE edge, edge1 ;

    loop = T2_EDGE_LOOP(edge0) ;
    up = ( T2_LOOP_ATTR(loop) & T2_ATTR_TOP ) != 0 ;

    if ( T2_EDGE_ATTR(edge0) & T2_ATTR_FIRST ) {
        edge = t2c_next_edge ( edge0 ) ;
        if ( T2_EDGE_ATTR(edge) & T2_ATTR_FIRST ) {
            if ( T2_EDGE_ATTR(edge) & T2_ATTR_PROCESSED ) ;
            else
                edge0 = edge ;
        }
    }

    xh = t2p_strip ( loop, edge0, NULL, h, -1, &up, &end_on_edge ) ;
    if ( xh == NULL ) 
        RETURN ( NULL ) ;

    edge1 = t2p_other_edge ( loop, edge0, T2_DIR_UNDEF ) ;

    t2p_unidir_step0 ( xh, edge0, ept, unidir_list ) ;

    for ( i=0 ; i<T2_XHATCH_N(xh)-2 ; i+=2 ) 
        t2p_unidir_step ( xh, i, ept, unidir_list ) ;

    edge = t2p_unidir_step1 ( xh, edge0, edge1, 
        end_on_edge, ept, unidir_list ) ;
    if ( edge != NULL ) 
        edge1 = edge ;
    t2x_free ( xh ) ;
    xh = NULL ;

    edge = t2c_prev_edge ( edge0 ) ;
    edge = t2p_check_horline ( loop, edge1, T2_DIR_CCW, unidir_list, ept ) ;
    if ( edge != NULL ) 
        edge1 = edge ;
    else 
        edge1 = t2p_check_edge1 ( edge1, loop, ept ) ;
    RETURN ( t2p_unidir_next ( looplist, edge1, ept ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2p_unidir_loop_3 ( looplist, h, edge0, ept, unidir_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
REAL h ;
T2_EDGE edge0 ;
PT2 ept ;
DML_LIST unidir_list ;
{
    INT i ;
    T2_XHATCH xh ;
    BOOLEAN up, end_on_edge ;
    T2_LOOP loop ;
    T2_EDGE edge, edge1 ;
    PT2 tan0, tan1 ;

    loop = T2_EDGE_LOOP(edge0) ;
    edge1 = t2p_other_edge ( loop, edge0, T2_DIR_UNDEF ) ;
    t2c_loop_ept_tan0 ( loop, NULL, tan0 ) ;
    t2c_loop_ept_tan1 ( loop, NULL, tan1 ) ;
    c2v_normalize ( tan0, tan0 ) ;
    c2v_normalize ( tan1, tan1 ) ;
    if ( IS_ZERO(tan0[1]) )
        up = tan0[0] > 0.0 ;
    else 
        up = tan1[0] > 0.0 ;
    xh = t2p_strip ( loop, edge0, edge1, h, -1, &up, &end_on_edge ) ;
    if ( xh == NULL ) 
        RETURN ( NULL ) ;
    t2p_unidir_step0 ( xh, edge0, ept, unidir_list ) ;

    for ( i=0 ; i<T2_XHATCH_N(xh)-2 ; i+=2 ) 
        t2p_unidir_step ( xh, i, ept, unidir_list ) ;

    edge1 = t2p_unidir_step1 ( xh, edge0, edge1, end_on_edge, 
        ept, unidir_list ) ;
    t2x_free ( xh ) ;
    xh = NULL ;
    edge = t2p_check_horline ( loop, edge1, T2_DIR_CCW, unidir_list, ept ) ;
    if ( edge != NULL ) 
        edge1 = edge ;
    RETURN ( t2p_unidir_next ( looplist, edge1, ept ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2p_unidir_step0 ( xh, edge0, ept, unidir_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_XHATCH xh ;
T2_EDGE edge0 ;
PT2 ept ;
DML_LIST unidir_list ;
{
    T2_EDGE edge ;
    PT2 vec ;
    if ( t2p_hor_edge ( edge0 ) ) {
        if ( T2_EDGE_ATTR(edge0) & T2_ATTR_PROCESSED ) ;
        else {
            edge = t2d_copy_edge ( edge0 ) ;
            if ( t2c_vec ( edge, vec ) && vec[0] < - BBS_ZERO ) 
                T2_EDGE_DIR(edge) = - T2_EDGE_DIR(edge) ;
            t2p_append_edge ( unidir_list, ept, edge ) ;
            t2p_mark_edge ( edge0, NULL ) ;
        }
        if ( T2_XHATCH_N(xh) > 0 ) {
            t2p_append_chain ( edge0, T2_EDGE_PARM1(edge0), 
                (T2_EDGE)(T2_XHATCH_OWNER(xh)[1]), T2_XHATCH_PARM(xh)+1, 
                TRUE, T2_ATTR_PROCESSED, ept, unidir_list ) ;
            t2p_unidir_step_line ( xh, 0, ept, unidir_list ) ;
            t2p_append_chain ( 
                (T2_EDGE)(T2_XHATCH_OWNER(xh)[0]), T2_XHATCH_PARM(xh), 
                edge0, T2_EDGE_PARM0(edge0), 
                TRUE, T2_ATTR_PROCESSED, ept, unidir_list ) ;
        }
    }
    else {
        if ( T2_XHATCH_N(xh) > 0 ) {
            t2p_append_chain ( edge0, T2_EDGE_PARM0(edge0), 
                (T2_EDGE)(T2_XHATCH_OWNER(xh)[1]), T2_XHATCH_PARM(xh)+1, 
                TRUE, T2_ATTR_PROCESSED, ept, unidir_list ) ;
            t2p_unidir_step_line ( xh, 0, ept, unidir_list ) ;
            t2p_append_chain ( (T2_EDGE)(T2_XHATCH_OWNER(xh)[0]), 
                T2_XHATCH_PARM(xh), edge0, T2_EDGE_PARM0(edge0), 
                TRUE, T2_ATTR_PROCESSED, ept, unidir_list ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2p_unidir_step1 ( xh, edge0, edge1, 
            end_on_edge, ept, unidir_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_XHATCH xh ;
T2_EDGE edge0, edge1 ;
BOOLEAN end_on_edge ;
PT2 ept ;
DML_LIST unidir_list ;
{
    INT j ;
    ATTR attr ;
    T2_EDGE edge = NULL ;

    attr = T2_ATTR_INIT ;
    if ( edge1 != NULL ) {
        if ( T2_EDGE_ATTR(edge1) & T2_ATTR_TOP ) 
            attr = T2_ATTR_TOP ;
        else if ( T2_EDGE_ATTR(edge1) & T2_ATTR_BOTTOM ) 
            attr = T2_ATTR_BOTTOM ;
    }

    j = T2_XHATCH_N(xh)-1 ; 

    if ( j < 0 ) {
        if ( edge1 != NULL ) {
            if ( !end_on_edge || 
                ( T2_EDGE_ATTR(edge1) & T2_ATTR_PROCESSED ) ) 
                edge = t2p_append_chain_check ( edge0, T2_EDGE_PARM1(edge0), 
/*
                    edge1, T2_EDGE_PARM0(edge1), TRUE, 
*/
                    edge0, T2_EDGE_PARM0(edge0), TRUE, 
                    T2_ATTR_PROCESSED, attr, ept, unidir_list ) ;
            else if ( T2_EDGE_ATTR(edge0) & T2_ATTR_PROCESSED ) 
                edge = t2p_append_chain_check ( edge0, T2_EDGE_PARM1(edge0), 
/*
                    edge1, T2_EDGE_PARM1(edge1), 
*/
                    edge0, T2_EDGE_PARM0(edge0), 
                    TRUE, T2_ATTR_PROCESSED, attr, ept, unidir_list ) ;
            else 
                edge = t2p_append_chain_check ( edge0, T2_EDGE_PARM1(edge0), 
                    edge1, T2_EDGE_PARM1(edge1), 
                    TRUE, T2_ATTR_PROCESSED, attr, ept, unidir_list ) ;
        }
        else 
            t2p_append_chain ( edge0, T2_EDGE_PARM1(edge0), edge0, 
                T2_EDGE_PARM0(edge0), TRUE, T2_ATTR_PROCESSED, 
                ept, unidir_list ) ;
    }
    else if ( end_on_edge && ( T2_EDGE_ATTR(edge1) & T2_ATTR_PROCESSED ) ) {
        edge = t2p_append_chain_check ( (T2_EDGE)(T2_XHATCH_OWNER(xh)[j]), 
            T2_XHATCH_PARM(xh)+j, edge1, T2_EDGE_PARM0(edge1), TRUE,
            T2_ATTR_PROCESSED, attr, ept, unidir_list ) ;
        if ( edge == NULL ) 
            edge = t2p_append_chain_check ( edge1, T2_EDGE_PARM1(edge1), 
                (T2_EDGE)(T2_XHATCH_OWNER(xh)[j-1]), T2_XHATCH_PARM(xh)+j-1, 
                TRUE, T2_ATTR_PROCESSED, attr, ept, unidir_list ) ;
    }
    else 
        edge = t2p_append_chain_check ( (T2_EDGE)(T2_XHATCH_OWNER(xh)[j]), 
            T2_XHATCH_PARM(xh)+j, (T2_EDGE)(T2_XHATCH_OWNER(xh)[j-1]), 
            T2_XHATCH_PARM(xh)+j-1, TRUE, T2_ATTR_PROCESSED, attr, 
            ept, unidir_list ) ;
    RETURN ( edge == NULL ? edge1 : edge ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2p_unidir_step ( xh, i, ept, unidir_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_XHATCH xh ;
INT i ;
PT2 ept ;
DML_LIST unidir_list ;
{
    t2p_unidir_step_bndr0 ( xh, i+2, ept, unidir_list ) ;
    t2p_unidir_step_line ( xh, i+2, ept, unidir_list ) ;
    t2p_unidir_step_bndr1 ( xh, i+2, ept, unidir_list ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2p_unidir_step_line ( xh, i, ept, unidir_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_XHATCH xh ;
INT i ;
PT2 ept ;
DML_LIST unidir_list ;
{
    T2_EDGE edge ;
    C2_CURVE curve ;

    if ( i < 0 || i > T2_XHATCH_N(xh) - 1 )
        RETURN ;
    curve = c2d_line ( T2_XHATCH_PT(xh)[i+1], T2_XHATCH_PT(xh)[i] ) ;
    edge = t2d_create_edge ( NULL, curve, 1 ) ;
    t2p_append_edge ( unidir_list, ept, edge ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2p_unidir_step_bndr0 ( xh, i, ept, unidir_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_XHATCH xh ;
INT i ;
PT2 ept ;
DML_LIST unidir_list ;
{
    INT j0, j1 ;

    j0 = i-1 ;
    j1 = i+1 ;
    t2p_append_chain ( 
        (T2_EDGE)(T2_XHATCH_OWNER(xh)[j0]), T2_XHATCH_PARM(xh)+j0, 
        (T2_EDGE)(T2_XHATCH_OWNER(xh)[j1]), T2_XHATCH_PARM(xh)+j1, 
        TRUE, T2_ATTR_PROCESSED, ept, unidir_list ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2p_unidir_step_bndr1 ( xh, i, ept, unidir_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_XHATCH xh ;
INT i ;
PT2 ept ;
DML_LIST unidir_list ;
{
    INT j0, j1 ;

    j0 = i ;
    j1 = i-2 ;
    t2p_append_chain ( 
        (T2_EDGE)(T2_XHATCH_OWNER(xh)[j0]), T2_XHATCH_PARM(xh)+j0, 
        (T2_EDGE)(T2_XHATCH_OWNER(xh)[j1]), T2_XHATCH_PARM(xh)+j1, 
        TRUE, T2_ATTR_PROCESSED, ept, unidir_list ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2p_unidir_next ( looplist, edge0, ept ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
T2_EDGE edge0 ;
PT2 ept ;
{
    DML_ITEM item1, item ;
    T2_EDGE edge1, edge ;
    T2_LOOP loop, loop1 ;
    REAL dist_min ;
    INT end_index ;

    if ( edge0 != NULL ) {
        edge = T2_EDGE_REF(edge0) ;
        loop = T2_EDGE_LOOP(edge) ;
        if ( !(T2_LOOP_ATTR(loop) & T2_ATTR_PROCESSED ) )
            RETURN ( edge ) ;
    }

    edge1 = NULL ;
    DML_WALK_LIST ( looplist, item ) {
        loop = DML_RECORD(item) ;
        if ( !( T2_LOOP_ATTR(loop) & T2_ATTR_PROCESSED ) ) {
            DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item1 ) {
                edge = DML_RECORD(item1) ;
                if ( T2_EDGE_ATTR(edge) & T2_ATTR_PROCESSED ) {
                    if ( t2p_unidir_next1 ( edge, &edge1, ept, &dist_min, 0 ) )
                        end_index = 0 ;
                    if ( t2p_unidir_next1 ( edge, &edge1, ept, &dist_min, 1 ) )
                        end_index = 1 ;
                }
                if ( T2_EDGE_ATTR(edge) & T2_ATTR_PROCSD_0 ) 
                    if ( t2p_unidir_next1 ( edge, &edge1, ept, &dist_min, 0 ) )
                        end_index = 0 ;
                if ( T2_EDGE_ATTR(edge) & T2_ATTR_PROCSD_1 ) 
                    if ( t2p_unidir_next1 ( edge, &edge1, ept, &dist_min, 1 ) )
                        end_index = 1 ;
            }
        }
    }
    if ( edge1 != NULL ) {
        if ( end_index == 0 ) 
            T2_EDGE_ATTR(edge1) = T2_EDGE_ATTR(edge1) | T2_ATTR_START_0 ;
        if ( end_index == 1 ) 
            T2_EDGE_ATTR(edge1) = T2_EDGE_ATTR(edge1) | T2_ATTR_START_1 ;
    }
    if ( edge1 != NULL ) 
        RETURN ( edge1 ) ;

    DML_WALK_LIST ( looplist, item ) {
        loop = DML_RECORD(item) ;
        if ( !( T2_LOOP_ATTR(loop) & T2_ATTR_PROCESSED ) ) {
            DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item1 ) {
                edge = DML_RECORD(item1) ;
                edge1 = T2_EDGE_REF(edge) ;
                if ( edge1 != NULL ) {
                    loop1 = T2_EDGE_LOOP(edge1) ;
                    if ( T2_LOOP_ATTR(loop1) & T2_ATTR_PROCESSED ) {
                        if ( T2_EDGE_ATTR(edge1) & T2_ATTR_PROCSD_0 )
                            T2_EDGE_ATTR(edge) = 
                                T2_EDGE_ATTR(edge) | T2_ATTR_PROCSD_1 ;
                        if ( T2_EDGE_ATTR(edge1) & T2_ATTR_PROCSD_1 )
                            T2_EDGE_ATTR(edge) = 
                                T2_EDGE_ATTR(edge) | T2_ATTR_PROCSD_0 ;
#ifdef CCDK_DEBUG
if ( DISPLAY ) {
paint_loop ( loop, 12 ) ;
getch ();
paint_loop ( loop1, 13 ) ;
getch ();
}
#endif
                        RETURN ( edge ) ;
                    }
                }
            }
        }
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2p_unidir_next1 ( edge, edge0, ept, dist_min, index ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge, *edge0 ;
PT2 ept ;
REAL *dist_min ;
INT index ;
{
    PT2 p ;
    REAL dist ;

    if ( index == 0 ) 
        t2c_ept0 ( edge, p ) ;
    else if ( index == 1 ) 
        t2c_ept1 ( edge, p ) ;
    dist = C2V_DIST ( ept, p ) ;
    if ( *edge0 == NULL || dist < *dist_min ) {
        *edge0 = edge ;
        *dist_min = dist ;
        RETURN ( TRUE ) ;
    }
    else
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2p_unidir_first ( looplist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
{
    DML_ITEM item ;
    T2_LOOP loop ;
    T2_EDGE edge ;

    DML_WALK_LIST ( looplist, item ) {
        loop = DML_RECORD(item) ;
        if ( T2_LOOP_ATTR(loop) & T2_ATTR_PROCESSED ) ;
        else {
            edge = t2c_edge_by_attr ( loop, T2_ATTR_FIRST ) ;
            if ( edge != NULL ) 
                RETURN ( edge ) ;
            edge = t2p_unidir_first_edge ( loop ) ;
            if ( edge != NULL ) 
                RETURN ( edge ) ;
        }
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2p_unidir_first_edge ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    PT2 p ;
    REAL y_max = 0.0 ;
    DML_ITEM item, item0 ;

    t2c_break_loop_at_extrs ( loop, 1 ) ;
    item0 = NULL ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        if ( t2c_ept0 ( DML_RECORD(item), p ) && 
            ( ( item0 == NULL ) || ( p[1] > y_max ) ) ) {
            item0 = item ;
            y_max = p[1] ;
        }
    }
    RETURN ( dml_record(item0) ) ;
}
#endif /* __BBS_MILL__>=2 */

