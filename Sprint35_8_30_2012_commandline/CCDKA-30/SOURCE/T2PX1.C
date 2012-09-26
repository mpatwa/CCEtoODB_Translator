/* -Z __BBS_MILL__=1 */
/********************************* T2PX1.C *********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2ddefs.h>
#include <c2tdefs.h>
#include <dmldefs.h>
#include <t2cdefs.h> 
#include <t2ddefs.h>
#include <t2pdefs.h>
#include <t2xdefs.h>
#include <t2attrd.h>
#include <c2vmcrs.h>
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif
#if ( __BBS_MILL__>=1 )

STATIC void t2p_zigzag_step_loop __(( T2_XHATCH, INT, T2_DIR, 
            PT2, DML_LIST )) ;
STATIC BOOLEAN t2p_zigzag_next1 __(( T2_EDGE, T2_EDGE*, PT2, REAL*, INT )) ;
STATIC void t2p_zigzag_append_loop __(( DML_LIST, PT2, T2_LOOP, C2_XFORM )) ;
STATIC void t2p_zigzag_xform_init __(( INT, C2_XFORM )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_zigzag_step0 ( xh, edge0, dir0, ept, zigzag_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_XHATCH xh ;
T2_EDGE edge0 ;
T2_DIR dir0 ;
PT2 ept ;
DML_LIST zigzag_list ;
{
    DML_ITEM item, item0, item1 ;
    T2_EDGE edge ;
    DML_LIST edgelist ;
    T2_LOOP zigzag ;

    if ( edge0 == NULL ) {
        if ( T2_XHATCH_N(xh) > 0 ) 
            t2p_append_chain ( 
                (T2_EDGE)(T2_XHATCH_OWNER(xh)[1]), T2_XHATCH_PARM(xh)+1, 
                (T2_EDGE)(T2_XHATCH_OWNER(xh)[0]), T2_XHATCH_PARM(xh), 
                FALSE, T2_ATTR_PROCESSED, ept, zigzag_list ) ;

        zigzag = DML_LAST_RECORD(zigzag_list) ;
        edgelist = T2_LOOP_EDGE_LIST(zigzag) ;
        for ( item = DML_FIRST(edgelist), item0 = NULL ; 
            item != NULL && item0 == NULL ; item = DML_NEXT(item) ) {
            edge = DML_RECORD(item) ;
            if ( T2_EDGE_ATTR(edge) & T2_ATTR_HORLINE )
                item0 = item ;
        }
        if ( item0 != NULL ) {
            for ( item = DML_FIRST(edgelist) ; item != item0 ; item = item1 ) {
                item1 = DML_NEXT(item) ;
                t2d_free_edge ( DML_RECORD(item) ) ;
                dml_remove_item ( edgelist, item ) ;
                item = NULL ;
            }
        }
    }

    else if ( t2p_hor_edge ( edge0 ) ) {

        if ( T2_EDGE_ATTR(edge0) & T2_ATTR_PROCESSED ) ;
        else {
            edge = t2d_copy_edge ( edge0 ) ;
            if ( dir0 == T2_DIR_CW ) 
                T2_EDGE_DIR(edge) = -T2_EDGE_DIR(edge) ;
            t2p_append_edge ( zigzag_list, ept, edge ) ;
            t2p_mark_edge ( edge0, NULL ) ;
        }
        if ( T2_XHATCH_N(xh) > 0 ) {
            if ( dir0 == T2_DIR_CCW ) 
                t2p_append_chain ( edge0, T2_EDGE_PARM1(edge0),
                    (T2_EDGE)(T2_XHATCH_OWNER(xh)[1]), 
                    T2_XHATCH_PARM(xh)+1, TRUE, T2_ATTR_PROCESSED, 
                    ept, zigzag_list ) ;
            else
                t2p_append_chain ( edge0, T2_EDGE_PARM0(edge0),
                    (T2_EDGE)(T2_XHATCH_OWNER(xh)[0]), T2_XHATCH_PARM(xh),
                    FALSE, T2_ATTR_PROCESSED, ept, zigzag_list ) ;
        }
    }
    else {
        if ( T2_XHATCH_N(xh) > 0 ) {
            if ( dir0 == T2_DIR_CCW ) 
                t2p_append_chain ( edge0, T2_EDGE_PARM0(edge0), 
                    (T2_EDGE)(T2_XHATCH_OWNER(xh)[1]), T2_XHATCH_PARM(xh)+1, 
                    TRUE, T2_ATTR_PROCESSED, ept, zigzag_list ) ;
            else
                t2p_append_chain ( edge0, T2_EDGE_PARM0(edge0), 
                    (T2_EDGE)(T2_XHATCH_OWNER(xh)[0]), T2_XHATCH_PARM(xh), 
                    FALSE, T2_ATTR_PROCESSED, ept, zigzag_list ) ;
        }
        else {
            edge = t2d_copy_edge ( edge0 ) ;
            if ( dir0 == T2_DIR_CW ) 
                T2_EDGE_DIR(edge) = -T2_EDGE_DIR(edge) ;
            t2p_append_edge ( zigzag_list, ept, edge ) ;
            t2p_mark_edge ( edge0, NULL ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2p_zigzag_step1 ( xh, edge0, edge1, dir, 
            end_on_edge, ept, zigzag_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_XHATCH xh ;
T2_EDGE edge0, edge1 ;
T2_DIR dir ;
BOOLEAN end_on_edge ;
PT2 ept ;
DML_LIST zigzag_list ;
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

    if ( dir == T2_DIR_CCW ) {
        t2p_zigzag_step_line ( xh, T2_XHATCH_N(xh)-2, 
            end_on_edge ? T2_DIR_CCW : T2_DIR_CW, ept, zigzag_list ) ;
        j = T2_XHATCH_N(xh)-1 ; 
    }
    else {
        t2p_zigzag_step_line ( xh, T2_XHATCH_N(xh)-2, 
            end_on_edge ? T2_DIR_CW : T2_DIR_CCW, ept, zigzag_list ) ;
        j = T2_XHATCH_N(xh)-2 ; 
    }

    if ( j < 0 ) {
        if ( edge1 != NULL ) {
            if ( !end_on_edge || 
                ( T2_EDGE_ATTR(edge1) & T2_ATTR_PROCESSED ) ) {
                if ( dir == T2_DIR_CCW ) 
                    edge = t2p_append_chain_check ( 
                        edge0, T2_EDGE_PARM1(edge0), 
                        edge1, T2_EDGE_PARM0(edge1), TRUE, 
                        T2_ATTR_PROCESSED, attr, ept, zigzag_list ) ;
                else
                    edge = t2p_append_chain_check ( 
                        edge0, T2_EDGE_PARM0(edge0), 
                        edge1, T2_EDGE_PARM1(edge1), FALSE, 
                        T2_ATTR_PROCESSED, attr, ept, zigzag_list ) ;
            }
            else if ( T2_EDGE_ATTR(edge0) & T2_ATTR_PROCESSED ) {
                if ( dir == T2_DIR_CCW ) 
                    edge = t2p_append_chain_check ( 
                        edge0, T2_EDGE_PARM1(edge0), 
                        edge1, T2_EDGE_PARM1(edge1), 
                        TRUE, T2_ATTR_PROCESSED, attr, ept, zigzag_list ) ;
                else
                    edge = t2p_append_chain_check ( 
                        edge0, T2_EDGE_PARM0(edge0), 
                        edge1, T2_EDGE_PARM0(edge1), FALSE, 
                        T2_ATTR_PROCESSED, attr, ept, zigzag_list ) ;
            }
            else {
                if ( dir == T2_DIR_CCW ) 
                    edge = t2p_append_chain_check ( 
                        edge0, T2_EDGE_PARM1(edge0), 
                        edge1, T2_EDGE_PARM1(edge1), 
                        TRUE, T2_ATTR_PROCESSED, attr, ept, zigzag_list ) ;
                else
                    edge = t2p_append_chain_check ( 
                        edge0, T2_EDGE_PARM0(edge0), 
                        edge1, T2_EDGE_PARM0(edge1), FALSE, 
                        T2_ATTR_PROCESSED, attr, ept, zigzag_list ) ;
            }
        }
        else {
            if ( dir == T2_DIR_CCW ) 
                t2p_append_chain ( edge0, T2_EDGE_PARM1(edge0), 
                    edge0, T2_EDGE_PARM0(edge0), TRUE, 
                    T2_ATTR_PROCESSED, ept, zigzag_list ) ;
            else
                t2p_append_chain ( edge0, T2_EDGE_PARM0(edge0), 
                    edge0, T2_EDGE_PARM1(edge0), FALSE, 
                    T2_ATTR_PROCESSED, ept, zigzag_list ) ;
        }
    }

    else if ( end_on_edge ) {
        if ( T2_EDGE_ATTR(edge1) & T2_ATTR_PROCESSED ) {
            if ( dir == T2_DIR_CCW ) 
                edge = t2p_append_chain_check ( 
                    (T2_EDGE)(T2_XHATCH_OWNER(xh)[j]), T2_XHATCH_PARM(xh)+j, 
                    edge1, T2_EDGE_PARM0(edge1), TRUE,
                    T2_ATTR_PROCESSED, attr, ept, zigzag_list ) ;
            else
                edge = t2p_append_chain_check ( 
                    (T2_EDGE)(T2_XHATCH_OWNER(xh)[j]), T2_XHATCH_PARM(xh)+j, 
                    edge1, T2_EDGE_PARM1(edge1), FALSE,
                    T2_ATTR_PROCESSED, attr, ept, zigzag_list ) ;
        }
        else {
            if ( dir == T2_DIR_CCW ) 
                edge = t2p_append_chain_check ( 
                    (T2_EDGE)(T2_XHATCH_OWNER(xh)[j]), T2_XHATCH_PARM(xh)+j, 
                    edge1, T2_EDGE_PARM1(edge1), TRUE,
                    T2_ATTR_PROCESSED, attr, ept, zigzag_list ) ;
            else
                edge = t2p_append_chain_check ( 
                    (T2_EDGE)(T2_XHATCH_OWNER(xh)[j]), T2_XHATCH_PARM(xh)+j, 
                    edge1, T2_EDGE_PARM0(edge1), FALSE, 
                    T2_ATTR_PROCESSED, attr, ept, zigzag_list ) ;
            t2p_mark_edge ( edge1, NULL ) ;
        }
    }
    else 
        if ( dir == T2_DIR_CCW )
            edge = t2p_append_chain_check ( 
                (T2_EDGE)(T2_XHATCH_OWNER(xh)[j-1]), T2_XHATCH_PARM(xh)+j-1,
                (T2_EDGE)(T2_XHATCH_OWNER(xh)[j]), T2_XHATCH_PARM(xh)+j, 
                FALSE, T2_ATTR_PROCESSED, attr, ept, zigzag_list ) ;
        else
            edge = t2p_append_chain_check ( 
                (T2_EDGE)(T2_XHATCH_OWNER(xh)[j+1]), T2_XHATCH_PARM(xh)+j+1, 
                (T2_EDGE)(T2_XHATCH_OWNER(xh)[j]), T2_XHATCH_PARM(xh)+j, 
                TRUE, T2_ATTR_PROCESSED, attr, ept, zigzag_list ) ;
    RETURN ( edge == NULL ? edge1 : edge ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_zigzag_step ( xh, i, dir, ept, zigzag_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_XHATCH xh ;
INT i ;
T2_DIR dir ;
PT2 ept ;
DML_LIST zigzag_list ;
{
    t2p_zigzag_step_line ( xh, i, dir, ept, zigzag_list ) ;
    t2p_zigzag_step_loop ( xh, i, dir, ept, zigzag_list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_zigzag_step_line ( xh, i, dir, ept, zigzag_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_XHATCH xh ;
INT i ;
T2_DIR dir ;
PT2 ept ;
DML_LIST zigzag_list ;
{
    T2_EDGE edge ;
    C2_CURVE curve ;

    if ( i < 0 || i > T2_XHATCH_N(xh) - 1 )
        RETURN ;
    curve = c2d_line ( T2_XHATCH_PT(xh)[i], T2_XHATCH_PT(xh)[i+1] ) ;
    edge = t2d_create_edge ( NULL, curve, dir==T2_DIR_CCW ? 1 : -1 ) ;
    t2p_append_edge ( zigzag_list, ept, edge ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2p_zigzag_step_loop ( xh, i, dir, ept, zigzag_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_XHATCH xh ;
INT i ;
T2_DIR dir ;
PT2 ept ;
DML_LIST zigzag_list ;
{
    INT j0, j1 ;

    if ( dir==T2_DIR_CCW ) {
        j0 = i+1 ;
        j1 = i+3 ;
    }
    else {
        j0 = i ;
        j1 = i+2 ;
    }
    t2p_append_chain ( 
        (T2_EDGE)(T2_XHATCH_OWNER(xh)[j0]), T2_XHATCH_PARM(xh)+j0, 
        (T2_EDGE)(T2_XHATCH_OWNER(xh)[j1]), T2_XHATCH_PARM(xh)+j1, 
        dir==T2_DIR_CCW, T2_ATTR_PROCESSED, ept, zigzag_list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2p_append_chain_check ( edge0, parm0, edge1, parm1, dir, 
            attr, check_attr, ept, zigzag_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge0, edge1 ;
PARM parm0, parm1 ;
BOOLEAN dir ;
ATTR attr, check_attr ;
PT2 ept ;
DML_LIST zigzag_list ;
{
    DML_LIST edgelist ;
    BOOLEAN cont_loop ;
    T2_EDGE child, parent, edge, edge2, result ;
    DML_ITEM item ;

    result = NULL ;
    edgelist = dml_create_list() ;

    cont_loop = t2p_append_trim_dir ( edge0, parm0, edge1, parm1, 
        dir, attr, edgelist ) ;
    edge = dml_last_record(edgelist) ;
    if ( edge != NULL ) {
        if ( !t2p_hor_edge ( edge ) ) 
            T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_ATTR_LAST ;
    }
    if ( check_attr != T2_ATTR_INIT ) {
        edge2 = t2c_prev_edge ( edge0 ) ;
        parent = NULL ;
        child = NULL ;
        for ( edge = edge1 ; edge != edge2 && child == NULL ; 
            edge = t2c_prev_edge ( edge ) ) {
            if ( T2_EDGE_ATTR(edge) & check_attr ) {
                parent = ( T2_EDGE_PARENT(edge) == NULL ) ? 
                    edge : T2_EDGE_PARENT(edge) ;
                child = edge ;
            }
        }
        result = NULL ;
        for ( item = DML_LAST(edgelist) ; item != NULL && result == NULL ; 
            item = DML_PREV(item) ) {
            edge = DML_RECORD(item) ;
            if ( T2_EDGE_PARENT(edge) == parent ) 
                result = child ;
        }
    }

    t2p_append_chain1 ( cont_loop, ept, edgelist, zigzag_list ) ;
    RETURN ( result ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_XHATCH t2p_strip ( loop, edge0, edge1, h, even, 
            up, end_on_edge ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_EDGE edge0, edge1 ;
REAL h ;
INT even ;      /* 1 - even, 0 - odd, -1 - does not matter */
BOOLEAN *up, *end_on_edge ;
{
    PT2 p0, p1, tan ;
    INT m ; 
    T2_XHATCH xhatch ;
    REAL diff ;
    BOOLEAN e0, e1 ;

#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    if ( edge0 != NULL )
        paint_edge ( edge0, 13 ) ;
    if ( edge1 != NULL )
        paint_edge ( edge1, 14 ) ;
    getch ();
}
else
    DISPLAY-- ;
#endif
    xhatch = NULL ;
    *end_on_edge = TRUE ;

    e0 = edge0 != NULL && t2p_hor_edge ( edge0 ) ;
    e1 = edge1 != NULL && t2p_hor_edge ( edge1 ) ;

    if ( e0 && e1 ) {
        t2c_ept0 ( edge0, p0 ) ;
        t2c_ept0 ( edge1, p1 ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_point ( p0, 0.02, 10 ) ;
    paint_point ( p1, 0.02, 11 ) ; 
    getch ();
}
else
    DISPLAY-- ;
#endif
        diff = fabs ( p0[1] - p1[1] ) ;
        if ( !IS_SMALL(diff) ) {
            m = (INT) ( diff / h + 1.0 - BBS_ZERO ) ;
            if ( m == 0 ) 
                m = 1 ;
            if ( even == 1 ) {
                if ( !(m%2) ) 
                    m++ ;
            }
            else if ( even == 0 ) {
                if ( m%2 ) 
                    m++ ;
            }
            h = diff / (REAL)m ;
            xhatch = t2x_loop ( loop, p0, h, 0.0 ) ;
            *up = ( p0[1] < p1[1] ) ;
        }
    }
    if ( xhatch == NULL ) {
        if ( e0 ) {
            t2c_ept0 ( edge0, p0 ) ;
            xhatch = t2x_loop ( loop, p0, h, 0.0 ) ;
            *end_on_edge = FALSE ;
            if ( T2_EDGE_ATTR(edge0) & T2_ATTR_BOTTOM ) 
                *up = TRUE ;
            else if ( T2_EDGE_ATTR(edge0) & T2_ATTR_TOP ) 
                *up = FALSE ;
            else if ( T2_EDGE_ATTR(edge0) & T2_ATTR_HORLINE ) 
                *up = t2c_etan0 ( edge0, tan ) && tan[0] > 0.0 ;

        }
        else if ( e1 ) {
            t2c_ept1 ( edge1, p1 ) ;
            xhatch = t2x_loop ( loop, p1, h, 0.0 ) ;
            if ( T2_EDGE_ATTR(edge1) & T2_ATTR_TOP ) 
                *up = TRUE ;
            else if ( T2_EDGE_ATTR(edge1) & T2_ATTR_BOTTOM ) 
                *up = FALSE ;
        }
        else {
            *end_on_edge = FALSE ;
            xhatch = t2x_loop ( loop, NULL, h, 0.0 ) ;
        }
    }
    if ( !(*up) ) 
        t2x_reverse ( xhatch ) ;
    RETURN ( xhatch ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2p_zigzag_next ( looplist, edge0, ept, dir ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
T2_EDGE edge0 ;
PT2 ept ;
T2_DIR *dir ;
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

#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    if ( edge0 != NULL ) {
        paint_edge ( edge0, 9 ) ;
        getch ();
    }
}
else
    DISPLAY-- ;
#endif
    edge1 = NULL ;
    *dir = T2_DIR_UNDEF ;

    DML_WALK_LIST ( looplist, item ) {
        loop = DML_RECORD(item) ;
        if ( !( T2_LOOP_ATTR(loop) & T2_ATTR_PROCESSED ) ) {
            DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item1 ) {
                edge = DML_RECORD(item1) ;
                if ( T2_EDGE_ATTR(edge) & T2_ATTR_PROCESSED ) {
                    if ( t2p_zigzag_next1 ( edge, &edge1, ept, &dist_min, 0 ) )
                        end_index = 0 ;
                    if ( t2p_zigzag_next1 ( edge, &edge1, ept, &dist_min, 1 ) )
                        end_index = 1 ;
                }
                if ( T2_EDGE_ATTR(edge) & T2_ATTR_PROCSD_0 ) 
                    if ( t2p_zigzag_next1 ( edge, &edge1, ept, &dist_min, 0 ) )
                        end_index = 0 ;
                if ( T2_EDGE_ATTR(edge) & T2_ATTR_PROCSD_1 ) 
                    if ( t2p_zigzag_next1 ( edge, &edge1, ept, &dist_min, 1 ) )
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
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop, 12 ) ;
    getch ();
    paint_loop ( loop1, 13 ) ;
    getch ();
}
else
    DISPLAY-- ;
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
STATIC BOOLEAN t2p_zigzag_next1 ( edge, edge0, ept, dist_min, index ) 
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
STATIC void t2p_zigzag_xform_init ( j, xform ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT j ;
C2_XFORM xform ;
{
    c2t_init ( xform ) ;
    if ( ( j == 1 ) /* || ( j == 2 ) */ ) 
        xform[0][0] = -1.0 ;
    if ( /* ( j == 2 ) || */ ( j == 3 ) ) 
        xform[1][1] = -1.0 ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_zigzag_xform_loop ( loop, j ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
INT j ;
{
    C2_XFORM xform ;
    t2p_zigzag_xform_init ( j, xform ) ;
    t2c_transform_loop ( loop, xform ) ;
    if ( ( j == 1 ) || ( j == 3 ) ) 
        t2c_reverse_loop ( loop ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_zigzag_append ( zigzag_list, ept, temp_list, j ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST zigzag_list ;
PT2 ept ;
DML_LIST temp_list ;
INT j ;
{
    C2_XFORM xform ;
    DML_ITEM item ;
    T2_LOOP loop ;

    t2p_zigzag_xform_init ( j, xform ) ;

    if ( ( j == 0 ) || ( j == 2 ) ) {
        DML_WALK_LIST ( temp_list, item ) {
            loop = DML_RECORD(item) ;
            t2p_zigzag_append_loop ( zigzag_list, ept, loop, xform ) ;
        }
    }
    else if ( ( j == 1 ) || ( j == 3 ) ) {
        for ( item = DML_LAST(temp_list) ; item != NULL ; 
            item = DML_PREV(item) ) {
            loop = DML_RECORD(item) ;
            t2p_zigzag_append_loop ( zigzag_list, ept, loop, xform ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
STATIC void t2p_zigzag_append_loop ( zigzag_list, ept, loop, xform ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST zigzag_list ;
PT2 ept ;
T2_LOOP loop ;
C2_XFORM xform ;
{
    T2_EDGE edge ;
    DML_ITEM item ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        t2c_transform_edge ( edge, xform ) ;
        t2p_append_edge ( zigzag_list, ept, edge ) ;
    }
    t2d_clear_loop ( loop ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_zigzag_rough_init ( rad, pt, p, zigzag_list, ept ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL rad ;
PT2 pt, p ;
DML_LIST zigzag_list ;
PT2 ept ;
{
    if ( pt == NULL ) {
        t2p_append_line ( p[0]+rad, p[1], p[0], p[1], zigzag_list, ept ) ;
    }
    else {
        t2p_append_line ( pt[0], pt[1], pt[0], p[1], zigzag_list, ept ) ;
        t2p_append_line ( pt[0], p[1], p[0], p[1], zigzag_list, ept ) ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void t2p_append_line ( x0, y0, x1, y1, zigzag_list, ept ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL x0, y0, x1, y1 ;
DML_LIST zigzag_list ;
PT2 ept ;
{
    PT2 p0, p1 ;

    C2V_SET ( x0, y0, p0 ) ;
    C2V_SET ( x1, y1, p1 ) ;
    if ( !C2V_IDENT_PTS ( p0, p1 ) ) 
        t2p_append_edge ( zigzag_list, ept, 
            t2d_create_edge ( NULL, c2d_line ( p0, p1 ), 1 ) ) ;
}
#endif /* __BBS_MILL__>=1 */

