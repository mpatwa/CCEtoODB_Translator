/* -Z __BBS_MILL__=1 */
/********************************* T2PX2.C *********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h>
#include <m2cdefs.h> 
#include <c2adefs.h> 
#include <c2vdefs.h> 
#include <t2cdefs.h> 
#include <t2cpriv.h> 
#include <t2ddefs.h>
#include <t2pdefs.h>
#include <t2attrd.h>
#include <c2vmcrs.h>
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
INT HORLINE1 = 0 ;
#endif
#if ( __BBS_MILL__>=1 )

STATIC  BOOLEAN t2p_edge_is_horline __(( T2_EDGE )) ;
STATIC  void t2p_loop_procsd __(( T2_LOOP )) ;
STATIC  T2_EDGE t2p_first_horline __(( T2_LOOP, T2_EDGE, T2_DIR, T2_EDGE* )) ;
STATIC  BOOLEAN t2p_check_horline1 __(( T2_LOOP, T2_EDGE, T2_DIR, 
            DML_LIST, PT2, T2_EDGE* )) ;
STATIC T2_EDGE t2p_xrough_pass0 __(( T2_LOOP, DML_LIST, PT2, PT2, INT, 
            REAL*, REAL*, BOOLEAN* )) ;
STATIC T2_EDGE t2p_xrough_pass1 __(( T2_LOOP, PT2, PT2, INT,
            REAL*, REAL*, BOOLEAN* )) ;
STATIC T2_EDGE t2p_xrough_pass2 __(( T2_LOOP, DML_LIST, PT2, PT2, 
            REAL, REAL )) ;
STATIC T2_EDGE t2p_xrough_pass3 __(( T2_LOOP, PT2, PT2, REAL, REAL )) ;
STATIC T2_EDGE t2p_rough_edge_test __(( PT2, T2_EDGE, T2_EDGE, PT2, PT2, 
            REAL, REAL, INT, INT )) ;
STATIC T2_LOOP t2p_project_looplist __(( PT2, DML_LIST, INT, T2_PARM )) ;
STATIC BOOLEAN t2p_project_loop __(( PT2, T2_LOOP, T2_PARM )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_mark_edge ( edge, item ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
DML_ITEM item ;
{
    T2_EDGE edge0, edge1, ref ;
    T2_LOOP loop ;
    DML_ITEM item0, item1 ;

    if ( edge == NULL ) 
        RETURN ;
    T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_ATTR_PROCESSED ;
    if ( item == NULL ) {
        edge0 = t2c_prev_edge ( edge ) ;
        edge1 = t2c_next_edge ( edge ) ;
    }
    else {
        loop = T2_EDGE_LOOP(edge) ;
        item0 = DML_PREV(item) ;
        if ( item0 == NULL ) 
            item0 = DML_LAST ( T2_LOOP_EDGE_LIST(loop) ) ;
        edge0 = DML_RECORD(item0) ;
        item1 = DML_NEXT(item) ;
        if ( item1 == NULL ) 
            item1 = DML_FIRST ( T2_LOOP_EDGE_LIST(loop) ) ;
        edge1 = DML_RECORD(item1) ;
    }

    if ( edge0 != NULL && t2p_hor_edge ( edge0 ) )
        T2_EDGE_ATTR(edge0) = T2_EDGE_ATTR(edge0) | T2_ATTR_PROCSD_1 ;
    if ( edge1 != NULL && t2p_hor_edge ( edge1 ) )
        T2_EDGE_ATTR(edge1) = T2_EDGE_ATTR(edge1) | T2_ATTR_PROCSD_0 ;

    ref = T2_EDGE_REF(edge) ;
    if ( ref == NULL ) 
        RETURN ;
    T2_EDGE_ATTR(ref) = T2_EDGE_ATTR(ref) | T2_ATTR_PROCESSED ;
    edge0 = t2c_prev_edge ( ref ) ;
    edge1 = t2c_next_edge ( ref ) ;
    if ( edge0 != NULL && t2p_hor_edge ( edge0 ) )
        T2_EDGE_ATTR(edge0) = T2_EDGE_ATTR(edge0) | T2_ATTR_PROCSD_1 ;
    if ( edge1 != NULL && t2p_hor_edge ( edge1 ) )
        T2_EDGE_ATTR(edge1) = T2_EDGE_ATTR(edge1) | T2_ATTR_PROCSD_0 ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_mark_loop ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    DML_ITEM item, item0, item1 ;
    T2_EDGE edge, edge0, edge1 ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL )
        RETURN ;
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        if ( T2_EDGE_ATTR(edge) & T2_ATTR_TOP ) 
            T2_LOOP_ATTR(loop) = T2_LOOP_ATTR(loop) | T2_ATTR_TOP ;
        if ( T2_EDGE_ATTR(edge) & T2_ATTR_BOTTOM ) 
            T2_LOOP_ATTR(loop) = T2_LOOP_ATTR(loop) | T2_ATTR_BOTTOM ;
        item0 = DML_PREV(item) ;
        if ( item0 == NULL ) 
            item0 = DML_LAST ( T2_LOOP_EDGE_LIST(loop) ) ;
        edge0 = dml_record ( item0 ) ;
        item1 = DML_NEXT(item) ;
        if ( item1 == NULL ) 
            item1 = DML_FIRST ( T2_LOOP_EDGE_LIST(loop) ) ;
        edge1 = dml_record ( item1 ) ;

        if ( T2_EDGE_ATTR(edge) & T2_ATTR_TOP ) {
            if ( T2_EDGE_ATTR(edge0) & T2_ATTR_TOP ) 
                t2p_mark_start ( edge0, 0 ) ;
            if ( T2_EDGE_ATTR(edge1) & T2_ATTR_TOP ) 
                t2p_mark_start ( edge1, 1 ) ;
        }
        else if ( T2_EDGE_ATTR(edge) & T2_ATTR_BOTTOM ) {
            if ( T2_EDGE_ATTR(edge0) & T2_ATTR_BOTTOM ) 
                t2p_mark_start ( edge0, 0 ) ;
            if ( T2_EDGE_ATTR(edge1) & T2_ATTR_BOTTOM ) 
                t2p_mark_start ( edge1, 1 ) ;
        }
        else if ( t2p_edge_is_horline ( edge ) ) {
            T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_ATTR_HORLINE ;
            T2_LOOP_ATTR(loop) = T2_LOOP_ATTR(loop) | T2_ATTR_HORLINE ;
            if ( edge0 != NULL ) {
                if ( ( T2_EDGE_ATTR(edge0) & T2_ATTR_TOP ) || 
                     ( T2_EDGE_ATTR(edge0) & T2_ATTR_BOTTOM ) )
                    t2p_mark_start ( edge0, 0 ) ;
                t2p_mark_start ( edge1, 1 ) ;
            }
            if ( edge1 != NULL ) {
                if ( ( T2_EDGE_ATTR(edge1) & T2_ATTR_TOP ) || 
                     ( T2_EDGE_ATTR(edge1) & T2_ATTR_BOTTOM ) )
                    t2p_mark_start ( edge1, 1 ) ;
            }
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2p_zigzag_first ( looplist ) 
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
        }
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2p_other_edge ( loop, edge0, dir0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_EDGE edge0 ;
T2_DIR dir0 ;
{
    T2_EDGE edge ;
    DML_ITEM item, item0 ;
    ATTR attr ;

    if ( loop == NULL && edge0 == NULL ) 
        RETURN ( NULL ) ;
    if ( loop == NULL ) 
        loop = T2_EDGE_LOOP(edge0) ;
    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ( NULL ) ;

    if ( T2_EDGE_ATTR(edge0) & T2_ATTR_TOP ) 
        attr = T2_ATTR_BOTTOM ;
    else if ( T2_EDGE_ATTR(edge0) & T2_ATTR_BOTTOM ) 
        attr = T2_ATTR_TOP ;
    else
        attr = T2_ATTR_INIT ;

    if ( dir0 == T2_DIR_CCW ) {
        item0 = dml_find_data ( T2_LOOP_EDGE_LIST(loop), edge0 ) ;
        for ( item = DML_NEXT(item0) ; item != NULL ; item = DML_NEXT(item) ) {
            edge = DML_RECORD(item) ;
            if ( T2_EDGE_ATTR(edge) & attr ) 
                RETURN ( edge ) ;
        }
        for ( item = DML_FIRST ( T2_LOOP_EDGE_LIST(loop) ) ; 
            item != item0 && item != NULL ; item = DML_NEXT(item) ) {
            edge = DML_RECORD(item) ;
            if ( T2_EDGE_ATTR(edge) & attr ) 
                RETURN ( edge ) ;
        }
    }

    else if ( dir0 == T2_DIR_CW ) {
        item0 = dml_find_data ( T2_LOOP_EDGE_LIST(loop), edge0 ) ;
        for ( item = DML_PREV(item0) ; item != NULL ; item = DML_PREV(item) ) {
            edge = DML_RECORD(item) ;
            if ( T2_EDGE_ATTR(edge) & attr ) 
                RETURN ( edge ) ;
        }
        for ( item = DML_LAST ( T2_LOOP_EDGE_LIST(loop) ) ; 
            item != item0 && item != NULL ; item = DML_PREV(item) ) {
            edge = DML_RECORD(item) ;
            if ( T2_EDGE_ATTR(edge) & attr ) 
                RETURN ( edge ) ;
        }
    }

    if ( edge0 != NULL ) {
        DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
            edge = DML_RECORD(item) ;
            if ( edge != edge0 && ( T2_EDGE_ATTR(edge) & attr ) )
                RETURN ( edge ) ;
        }
    }
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        if ( ( T2_EDGE_ATTR(edge) & T2_ATTR_BOTTOM ) ||
            ( T2_EDGE_ATTR(edge) & T2_ATTR_TOP ) ) {
            if ( edge != edge0 ) 
                RETURN ( edge ) ;
        }
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2p_edge_is_horline ( edge ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                 !!!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
{
    RETURN ( m2c_curve_is_horline ( T2_EDGE_CURVE(edge) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2p_hor_edge ( edge ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                 !!!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
{
    if ( edge == NULL ) 
        RETURN ( FALSE ) ;
    if ( T2_EDGE_ATTR(edge) & T2_ATTR_TOP ) 
        RETURN ( TRUE ) ;
    else if ( T2_EDGE_ATTR(edge) & T2_ATTR_BOTTOM ) 
        RETURN ( TRUE ) ;
    else if ( T2_EDGE_ATTR(edge) & T2_ATTR_HORLINE ) 
        RETURN ( TRUE ) ;
    else
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2p_loop_procsd ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                 !!!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    DML_ITEM item ;
    T2_EDGE edge ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        if ( T2_EDGE_ATTR(edge) & T2_ATTR_PROCESSED ) {
            if ( t2p_hor_edge ( edge ) ) 
                t2p_mark_edge ( edge, item ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2p_check_horline ( loop, edge, dir, zigzag_list, ept ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                 !!!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_EDGE edge ;
T2_DIR dir ;
DML_LIST zigzag_list ;
PT2 ept ;
{
    BOOLEAN done = FALSE ;
    T2_EDGE edge1 ;

    t2p_loop_procsd ( loop ) ;
    edge1 = NULL ;
    while ( !done )
        done = t2p_check_horline1 ( loop, edge, dir, zigzag_list, 
            ept, &edge1 ) ;
    RETURN ( edge1 ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2p_first_horline ( loop, edge0, dir, last_edge ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                 !!!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_EDGE edge0 ;
T2_DIR dir ;
T2_EDGE *last_edge ;
{
    T2_EDGE edge ;
    DML_ITEM item0, item ;

#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop_x ( loop ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
/*  YG 12-29-92 */
    *last_edge = ( edge0 == NULL ) ? t2c_first_edge ( loop ) : edge0 ;
    item0 = ( edge0 == NULL ) ? DML_FIRST ( T2_LOOP_EDGE_LIST(loop) ) :
        dml_find_data ( T2_LOOP_EDGE_LIST(loop), edge0 ) ;
    if ( dir == T2_DIR_CW ) {
        for ( item = item0 ; item != NULL ; item = DML_PREV(item) ) {
            edge = DML_RECORD(item) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( edge, 13 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
            if ( T2_EDGE_ATTR(edge) & T2_ATTR_PROCESSED ) {
                if ( t2p_hor_edge ( edge ) )
                    *last_edge = edge ;
            }
            else {
                if ( T2_EDGE_ATTR(edge) & T2_ATTR_HORLINE ) 
                    RETURN ( edge ) ;
/* 11-17-92 */
                else 
                    *last_edge = edge ;
/* 11-17-92 */
            }
        }
        for ( item = DML_LAST(T2_LOOP_EDGE_LIST(loop)) ; 
            item != NULL && item != item0 ; item = DML_PREV(item) ) {
            edge = DML_RECORD(item) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( edge, 13 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
            if ( T2_EDGE_ATTR(edge) & T2_ATTR_PROCESSED ) {
                if ( t2p_hor_edge ( edge ) )
                    *last_edge = edge ;
            }
            else {
                if ( T2_EDGE_ATTR(edge) & T2_ATTR_HORLINE ) 
                    RETURN ( edge ) ;
/* 11-17-92 */
                else 
                    *last_edge = edge ;
/* 11-17-92 */
            }
        }
    }
    else {
        for ( item = item0 ; item != NULL ; item = DML_NEXT(item) ) {
            edge = DML_RECORD(item) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( edge, 13 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
            if ( T2_EDGE_ATTR(edge) & T2_ATTR_PROCESSED ) {
                if ( t2p_hor_edge ( edge ) )
                    *last_edge = edge ;
            }
            else {
                if ( ( T2_EDGE_ATTR(edge) & T2_ATTR_HORLINE ) && 
                     ( T2_EDGE_ATTR(edge) & T2_ATTR_PART ) )
                    RETURN ( edge ) ;
/* 11-17-92 */
                else 
                    *last_edge = edge ;
/* 11-17-92 */
            }
        }
        for ( item = DML_FIRST(T2_LOOP_EDGE_LIST(loop)) ; 
            item != NULL && item != item0 ; item = DML_NEXT(item) ) {
            edge = DML_RECORD(item) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( edge, 13 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
            if ( T2_EDGE_ATTR(edge) & T2_ATTR_PROCESSED ) {
                if ( t2p_hor_edge ( edge ) )
                    *last_edge = edge ;
            }
            else {
                if ( ( T2_EDGE_ATTR(edge) & T2_ATTR_HORLINE ) && 
                     ( T2_EDGE_ATTR(edge) & T2_ATTR_PART ) )
                    RETURN ( edge ) ;
/* 11-17-92 */
                else 
                    *last_edge = edge ;
/* 11-17-92 */
            }
        }
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2p_check_horline1 ( loop, edge0, dir, zigzag_list, 
    ept, edge1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                 !!!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_EDGE edge0 ;
T2_DIR dir ;
DML_LIST zigzag_list ;
PT2 ept ;
T2_EDGE *edge1 ;
{
    T2_EDGE edge, last_edge, copy, edge2 ;
    PARM last_parm ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ( TRUE ) ;
#ifdef CCDK_DEBUG
HORLINE1++ ;
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop_x ( loop ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
    if ( !( T2_LOOP_ATTR(loop) & T2_ATTR_HORLINE ) )
        RETURN ( TRUE ) ;
    edge = t2p_first_horline ( loop, edge0, dir, &last_edge ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    if ( edge != NULL )
        paint_edge ( edge, 12 ) ;
    if ( last_edge != NULL )
        paint_edge ( last_edge, 13 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
    if ( edge == NULL ) 
        RETURN ( TRUE ) ;
    if ( last_edge == NULL ) 
        RETURN ( TRUE ) ;
    if ( T2_EDGE_ATTR(last_edge) & T2_ATTR_PROCESSED ) 
        last_parm = ( dir == T2_DIR_CCW ) ? 
            T2_EDGE_PARM1(last_edge) : T2_EDGE_PARM0(last_edge) ;
    else 
        last_parm = ( dir == T2_DIR_CCW ) ? 
            T2_EDGE_PARM0(last_edge) : T2_EDGE_PARM1(last_edge) ;

    *edge1 = t2p_append_chain_check ( last_edge, last_parm, edge, 
        ( dir == T2_DIR_CCW ) ? T2_EDGE_PARM1(edge) : T2_EDGE_PARM0(edge), 
        dir == T2_DIR_CCW, T2_ATTR_PROCESSED, (ATTR)0, ept, zigzag_list ) ;

    edge2 = ( dir == T2_DIR_CCW ) ? 
        t2c_next_edge ( edge ) : t2c_prev_edge ( edge ) ;
    if ( T2_EDGE_ATTR(edge2) & T2_ATTR_PROCESSED ) ;
    else if ( ( T2_EDGE_ATTR(edge2) & T2_ATTR_TOP ) || 
         ( T2_EDGE_ATTR(edge2) & T2_ATTR_BOTTOM ) ) {
        copy = t2d_copy_edge ( edge2 ) ;
        if ( dir != T2_DIR_CCW  )
            T2_EDGE_DIR(copy) = -T2_EDGE_DIR(copy) ;
        t2p_append_edge ( zigzag_list, ept, copy ) ;
        t2p_mark_edge ( edge2, NULL ) ;
        *edge1 = edge2 ;
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_DIR t2p_zigzag_dir0 ( edge0, dir, dir_last )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                 !!!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge0 ;
T2_DIR dir, dir_last ;
{
    BOOLEAN end0, end1 ;

    if ( T2_EDGE_ATTR(edge0) & T2_ATTR_PROCESSED ) {
        end0 = TRUE ;
        end1 = TRUE ;
    }
    else {
        end0 = ( T2_EDGE_ATTR(edge0) & T2_ATTR_PROCSD_0 ) != 0 ;
        end1 = ( T2_EDGE_ATTR(edge0) & T2_ATTR_PROCSD_1 ) != 0 ;
    }

    if ( end0 && !end1 ) 
        RETURN ( T2_DIR_CCW ) ;
    else if ( !end0 && end1 ) 
        RETURN ( T2_DIR_CW ) ;
    else if ( dir_last != T2_DIR_UNDEF && 
        ( T2_EDGE_ATTR(edge0) & T2_ATTR_FIRST ) ) 
        RETURN ( dir_last ) ;
    else if ( dir == T2_DIR_CCW ) 
        RETURN ( T2_DIR_CW ) ;
    else if ( dir == T2_DIR_CW ) 
        RETURN ( T2_DIR_CCW ) ;
    else { 
        end0 = ( T2_EDGE_ATTR(edge0) & T2_ATTR_START_0 ) != 0 ;
        end1 = ( T2_EDGE_ATTR(edge0) & T2_ATTR_START_1 ) != 0 ;
        if ( end0 && !end1 ) 
            RETURN ( T2_DIR_CW ) ;
        if ( !end0 && end1 ) 
            RETURN ( T2_DIR_CCW ) ;
        else 
            RETURN ( T2_DIR_UNDEF ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_DIR t2p_zigzag_dir1 ( edge1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                 !!!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge1 ;
{
    if ( edge1 == NULL || ( T2_EDGE_ATTR(edge1) & T2_ATTR_PROCESSED ) )
        RETURN ( T2_DIR_UNDEF ) ;
    else if ( T2_EDGE_ATTR(edge1) & T2_ATTR_START_0 ) 
        RETURN ( T2_DIR_CCW ) ;
    else if ( T2_EDGE_ATTR(edge1) & T2_ATTR_START_1 ) 
        RETURN ( T2_DIR_CW ) ;
    else 
        RETURN ( T2_DIR_UNDEF ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_mark_start ( edge, end )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                 !!!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
INT end ;
{
    T2_EDGE ref = T2_EDGE_REF(edge) ;

    if ( end == 0 ) {
        if ( T2_EDGE_ATTR(edge) & T2_ATTR_START_1 ) {
            T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) & ~T2_ATTR_START_1 ;
            if ( ref != NULL ) 
                T2_EDGE_ATTR(ref) = T2_EDGE_ATTR(ref) & ~T2_ATTR_START_0 ;
        }
        T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_ATTR_START_0 ;
        if ( ref != NULL ) 
            T2_EDGE_ATTR(ref) = T2_EDGE_ATTR(ref) | T2_ATTR_START_1 ;
    }
    else if ( end == 1 ) {
        if ( T2_EDGE_ATTR(edge) & T2_ATTR_START_0 ) {
            T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) & ~T2_ATTR_START_0 ;
            if ( ref != NULL ) 
                T2_EDGE_ATTR(ref) = T2_EDGE_ATTR(ref) & ~T2_ATTR_START_1 ;
        }
        T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_ATTR_START_1 ;
        if ( ref != NULL ) 
            T2_EDGE_ATTR(ref) = T2_EDGE_ATTR(ref) | T2_ATTR_START_0 ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_mark_refs ( looplist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                 !!!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
{
    DML_LIST ref0list = dml_create_list (), ref1list = dml_create_list ();
    DML_ITEM item, item0 ;
    T2_EDGE edge, edge0 ;
    T2_LOOP loop ;
    PT2 p0, p1, q0, q1 ;
    BOOLEAN end_loop ;

    DML_WALK_LIST ( looplist, item0 ) {
        loop = DML_RECORD(item0) ;
        DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
            edge = DML_RECORD(item) ;
            if ( ( T2_EDGE_ATTR(edge) & T2_ATTR_TOP ) ||
                 ( T2_EDGE_ATTR(edge) & T2_ATTR_BOTTOM ) ) {
                if ( T2_EDGE_REF(edge) == NULL ) 
                    dml_append_data ( ref0list, edge ) ;
                else 
                    dml_append_data ( ref1list, edge ) ;
            }
        }
    }

    while ( DML_LENGTH(ref0list) > 0 ) {
        item0 = DML_FIRST(ref0list) ;
        edge0 = DML_RECORD(item0) ;
        t2c_ept0 ( edge0, p0 ) ;
        t2c_ept1 ( edge0, p1 ) ;
        end_loop = FALSE ;

        for ( item = DML_NEXT(item0) ; item != NULL && !end_loop ; 
            item = DML_NEXT(item) ) {
            edge = DML_RECORD(item) ;
            t2c_ept0 ( edge, q0 ) ;
            t2c_ept1 ( edge, q1 ) ;
            if ( C2V_IDENT_PTS(p0,q1) && C2V_IDENT_PTS(p1,q0) ) {
                T2_EDGE_REF(edge0) = edge ;
                T2_EDGE_REF(edge) = edge0 ;
                dml_remove_item ( ref0list, item ) ;
                dml_remove_first ( ref0list ) ;
                end_loop = TRUE ;
            }
        }
        for ( item = DML_FIRST(ref1list) ; item != NULL && !end_loop ; 
            item = DML_NEXT(item) ) {
            edge = DML_RECORD(item) ;
            t2c_ept0 ( edge, q0 ) ;
            t2c_ept1 ( edge, q1 ) ;
            if ( C2V_IDENT_PTS(p0,q1) && C2V_IDENT_PTS(p1,q0) ) {
                T2_EDGE_REF(edge0) = edge ;
                T2_EDGE_REF(edge) = edge0 ;
                dml_remove_item ( ref1list, item ) ;
                dml_remove_first ( ref0list ) ;
                end_loop = TRUE ;
            }
        }
    }
    dml_free_list ( ref0list ) ;
    dml_free_list ( ref1list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2p_check_edge1 ( edge0, loop, ept ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge0 ;
T2_LOOP loop ;
PT2 ept ;
{
    T2_EDGE edge, edge1 = NULL ;
    REAL dist, dist_min = 0.0 ;
    DML_ITEM item ;
    ATTR attr ;
    PT2 p ;

    if ( edge0 == NULL ) 
        RETURN ( NULL ) ;
    if ( ( t2c_ept0 ( edge0, p ) && C2V_IDENT_PTS ( ept, p ) ) ||
         ( t2c_ept1 ( edge0, p ) && C2V_IDENT_PTS ( ept, p ) ) )
        RETURN ( edge0 ) ;
    if ( T2_EDGE_ATTR(edge0) & T2_ATTR_TOP ) 
        attr = T2_ATTR_TOP ;
    else if ( T2_EDGE_ATTR(edge0) & T2_ATTR_BOTTOM ) 
        attr = T2_ATTR_BOTTOM ;
    else
        RETURN ( edge0 ) ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        if ( ( T2_EDGE_ATTR(edge) & T2_ATTR_PROCESSED ) && 
             ( T2_EDGE_ATTR(edge) & attr ) ) {
            t2c_ept0 ( edge, p ) ;
            dist = C2V_DIST ( p, ept ) ;
            if ( edge1 == NULL || dist < dist_min ) {
                edge1 = edge ;
                dist_min = dist ;
            }
            t2c_ept1 ( edge, p ) ;
            dist = C2V_DIST ( p, ept ) ;
            if ( edge1 == NULL || dist < dist_min ) {
                edge1 = edge ;
                dist_min = dist ;
            }
        }
    }
    RETURN ( edge1 == NULL ? edge0 : edge1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2p_rough_edge ( region, looplist, pt, p ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
DML_LIST looplist ;
PT2 pt, p ;
{
    T2_EDGE edge ;
    BOOLEAN has_rough ;
    REAL x_min, x_max ;

    has_rough = FALSE ;
    edge = t2p_xrough_pass0 ( T2_REGION_EXT_LOOP(region), looplist, 
        pt, p, 1, NULL, NULL, &has_rough ) ;
    if ( edge != NULL ) {
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( edge, 12 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
        RETURN ( edge ) ;
    }
    if ( !has_rough )
        RETURN ( NULL ) ;
    edge = t2p_xrough_pass0 ( T2_REGION_EXT_LOOP(region), looplist, 
        pt, p, 0, &x_min, &x_max, &has_rough ) ;
    if ( edge != NULL ) {
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( edge, 12 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
        RETURN ( edge ) ;
    }

    edge = t2p_xrough_pass2 ( T2_REGION_EXT_LOOP(region), looplist, pt, 
        p, x_min, x_max ) ;
    if ( edge != NULL ) {
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( edge, 12 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
        RETURN ( edge ) ;
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2p_xrough_pass0 ( loop, looplist, pt, p, coord, 
            min_ptr, max_ptr, has_rough ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
DML_LIST looplist ;
PT2 pt, p ;
INT coord ;
REAL *min_ptr, *max_ptr ;
BOOLEAN *has_rough ;
{
    T2_EDGE edge ;
    T2_PARM_S parm ;
    T2_LOOP loop1 ;

    edge = t2p_xrough_pass1 ( loop, pt, p, coord, 
        min_ptr, max_ptr, has_rough ) ;
    if ( edge == NULL ) 
        RETURN ( NULL ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( edge, 13 ) ;
    paint_point ( p, 0.015, 14 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
    loop1 = t2p_project_looplist ( p, looplist, coord, &parm ) ;
    if ( loop1 == NULL ) 
        RETURN ( NULL ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( T2_PARM_EDGE(&parm), 13 ) ;
    paint_point ( p, 0.015, 14 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
    t2p_startpt_extr ( loop1, looplist, &parm ) ;
    RETURN ( T2_PARM_EDGE(&parm) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2p_xrough_pass1 ( loop, pt, p, coord, 
            min_ptr, max_ptr, has_rough ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt, p ;
INT coord ;
REAL *min_ptr, *max_ptr ;
BOOLEAN *has_rough ;
{
    T2_EDGE edge0, edge1, edge_min, edge_max ;
    DML_ITEM item0, item1 ;
    PT2 q, p_min, p_max ;
    REAL coord_min, coord_max ;
    INT status ;

    status = 0 ;
    edge_min = NULL ;
    edge_max = NULL ;
    t2c_break_loop_at_extrs ( loop, coord ) ;
    t2c_loop_ept0 ( loop, p_min ) ;
    coord_min = p_min[coord] ;
    coord_max = coord_min ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item0 ) {
        edge0 = DML_RECORD(item0) ;
        t2c_ept0 ( edge0, q ) ;
        if ( q[coord] < coord_min ) 
            coord_min = q[coord] ;
        if ( q[coord] > coord_max ) 
            coord_max = q[coord] ;
        if ( T2_EDGE_ATTR(edge0) & T2_ATTR_ROUGH ) {
            if ( has_rough != NULL )
                *has_rough = TRUE ;
        }
    }

    if ( min_ptr != NULL ) 
        *min_ptr = coord_min ;
    if ( max_ptr != NULL ) 
        *max_ptr = coord_max ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item0 ) {
        edge0 = DML_RECORD(item0) ;
        item1 = DML_NEXT(item0) ;
        if ( item1 == NULL ) 
            item1 = dml_first ( T2_LOOP_EDGE_LIST(loop) ) ;
        edge1 = DML_RECORD(item1) ;
        t2c_ept0 ( edge1, q ) ;
        if ( ( T2_EDGE_ATTR(edge0) & T2_ATTR_PART ) || 
             ( T2_EDGE_ATTR(edge1) & T2_ATTR_PART ) ) {
            if ( IS_SMALL ( q[coord] - coord_min ) )
                status = 1 ;
            else if ( IS_SMALL ( q[coord] - coord_max ) )
                status = -1 ;
        }
        if ( ( T2_EDGE_ATTR(edge0) & T2_ATTR_ROUGH ) &&
             ( T2_EDGE_ATTR(edge1) & T2_ATTR_ROUGH ) ) {
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( edge0, 13 ) ;
    paint_edge ( edge1, 14 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
#ifdef OLD_CODE
            if ( ( edge_min == NULL ) || 
                ( q[coord] < p_min[coord] - BBS_ZERO ) ||
                ( IS_SMALL(q[coord]-p_min[coord]) && pt != NULL && 
                    ( C2V_DIST(q,pt) < C2V_DIST(p_min,pt) ) ) ) {
                edge_min = edge1 ;
                C2V_COPY ( q, p_min ) ;
            }
            if ( ( edge_max == NULL ) || ( q[coord] > p_max[coord] ) ||
                ( IS_SMALL(q[coord]-p_max[coord]) && pt != NULL && 
                    ( C2V_DIST(q,pt) < C2V_DIST(p_max,pt) ) ) ) {
                edge_max = edge1 ;
                C2V_COPY ( q, p_max ) ;
            }
#endif
            if ( pt == NULL ) {
                if ( ( edge_min == NULL ) || 
                    ( q[coord] < p_min[coord] - BBS_ZERO ) ) {
                    edge_min = edge1 ;
                    C2V_COPY ( q, p_min ) ;
                }
                if ( ( edge_max == NULL ) || ( q[coord] > p_max[coord] ) ) {
                    edge_max = edge1 ;
                    C2V_COPY ( q, p_max ) ;
                }
            }
            else {
                if ( ( edge_min == NULL ) || 
                    ( C2V_DIST(q,pt) < C2V_DIST(p_min,pt) ) ) {
                    edge_min = edge1 ;
                    C2V_COPY ( q, p_min ) ;
                }
            }
        }
    }
    RETURN ( t2p_rough_edge_test ( p, edge_min, edge_max, 
        p_min, p_max, coord_min, coord_max, coord, status ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2p_xrough_pass2 ( loop, looplist, pt, p, x_min, x_max ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
DML_LIST looplist ;
PT2 pt, p ;
REAL x_min, x_max ;
{
    T2_EDGE edge ;
    T2_PARM_S parm ;
    T2_LOOP loop1 ;

    edge = t2p_xrough_pass3 ( loop, pt, p, x_min, x_max ) ;
    if ( edge == NULL ) 
        RETURN ( NULL ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( edge, 13 ) ;
    paint_point ( p, 0.015, 14 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
    loop1 = t2p_project_looplist ( p, looplist, 0, &parm ) ;
    if ( loop1 == NULL ) 
        RETURN ( NULL ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( T2_PARM_EDGE(&parm), 13 ) ;
    paint_point ( p, 0.015, 14 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
    t2p_startpt_extr ( loop1, looplist, &parm ) ;
    RETURN ( T2_PARM_EDGE(&parm) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2p_xrough_pass3 ( loop, pt, p, x_min, x_max ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt, p ;
REAL x_min, x_max ;
{
    T2_EDGE edge, edge_min, edge_max ;
    DML_ITEM item ;
    PT2 p0, p1 ;
    INT status ;
    REAL dist_min, dist_max ;

    status = 0 ;
    edge_min = NULL ;
    edge_max = NULL ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        if ( T2_EDGE_ATTR(edge) & T2_ATTR_ROUGH ) {
            t2c_ept0 ( edge, p0 ) ;
            t2c_ept1 ( edge, p1 ) ;
            if ( IS_SMALL ( p0[0] - x_max ) || IS_SMALL ( p1[0] - x_max ) ) {
                if ( edge_max != NULL ) 
                    edge_max = edge ;
            }
            if ( IS_SMALL ( p0[0] - x_min ) || IS_SMALL ( p1[0] - x_min ) ) {
                if ( edge_min != NULL ) 
                    edge_min = edge ;
            }
        }
    }

    if ( edge_max == NULL ) {
        if ( edge_min == NULL ) 
            status = 0 ;
        else 
            status = -1 ;
    }

    else {
        if ( edge_min == NULL ) 
            status = 1 ;
        else {
            if ( pt == NULL )
                status = 1 ;
            else {
                dist_min = fabs ( pt[0] - x_min ) ;
                dist_max = fabs ( pt[0] - x_max ) ;
                status = ( dist_min < dist_max ) ? -1 : 1 ;
            }
        }    
    }

    if ( status == -1 ) 
        edge = edge_min ;
    else if ( status == 1 ) 
        edge = edge_max ;
    else 
        RETURN ( NULL ) ;
    t2c_ept0 ( edge, p0 ) ;
    t2c_ept1 ( edge, p1 ) ;
    C2V_MID_PT ( p0, p1, p ) ;
    edge = t2c_loop_insert_vtx ( loop, p, p ) ;
    RETURN ( edge ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2p_rough_edge_test ( p, edge_min, edge_max, 
            p_min, p_max, coord_min, coord_max, coord, status0 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 p ;
T2_EDGE edge_min, edge_max ;
PT2 p_min, p_max ;
REAL coord_min, coord_max ;
INT coord, status0 ;
{
    INT status ;

    status = 0 ;
#ifdef OLD_CODE
    if ( edge_min != NULL ) {
        if ( edge_max != NULL ) {
            if ( IS_SMALL ( coord_min - p_min[coord] ) ) {
                if ( IS_SMALL ( coord_max - p_max[coord] ) ) {
                    if ( pt != NULL ) {
                        dist_min = C2V_DIST ( pt, p_min ) ;
                        dist_max = C2V_DIST ( pt, p_max ) ;
                        status = ( dist_min < dist_max ) ? -1 : 1 ;
                    }
                }
                else
                    status = -1 ;
            }
            else if ( IS_SMALL ( coord_max - p_max[coord] ) ) 
                status = 1 ;
            if ( ( status == 0 ) && ( pt != NULL ) ) {
                dist_min = C2V_DIST ( pt, p_min ) ;
                dist_max = C2V_DIST ( pt, p_max ) ;
                status = ( dist_min < dist_max ) ? -1 : 1 ;
            }
            else if ( status0 != 0 ) 
                status = status0 ;
            else 
                status = -1 ;
        }
        else 
            status = -1 ;
    }
    else if ( edge_max != NULL )
        status = 1 ;
#endif
    if ( edge_min != NULL ) {
        if ( edge_max != NULL ) {
            if ( IS_SMALL ( coord_min - p_min[coord] ) ) 
                status = -1 ;
            else if ( IS_SMALL ( coord_max - p_max[coord] ) ) 
                status = 1 ;
            else if ( status0 != 0 ) 
                status = status0 ;
            else 
                status = -1 ;
        }
        else 
            status = -1 ;
    }
    else if ( edge_max != NULL )
        status = 1 ;

    if ( status == 1 ) {
        C2V_COPY ( p_max, p ) ;
        RETURN ( edge_max ) ;
    }
    if ( status == -1 ) {
        C2V_COPY ( p_min, p ) ;
        RETURN ( edge_min ) ;
    }
    else
        RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_LOOP t2p_project_looplist ( p, looplist, coord, parm )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 p ;
DML_LIST looplist ;
INT coord ;
T2_PARM parm ;
{
    DML_ITEM item ;

    DML_WALK_LIST ( looplist, item ) {
        t2c_break_loop_at_extrs ( DML_RECORD(item), coord ) ;
        if ( t2p_project_loop ( p, DML_RECORD(item), parm ) )
            RETURN ( DML_RECORD(item) ) ;
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2p_project_loop ( p, loop, parm )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 p ;
T2_LOOP loop ;
T2_PARM parm ;
{
    DML_ITEM item ;
    T2_EDGE edge ;
    PT2 p0 ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ( FALSE ) ;
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        t2c_ept0 ( edge, p0 ) ;
        if ( C2V_IDENT_PTS ( p, p0 ) ) {
            T2_PARM_EDGE(parm) = edge ;
            PARM_COPY ( T2_EDGE_PARM0(edge), T2_PARM_CPARM(parm) ) ;
            RETURN ( TRUE ) ;
        }
    }
    RETURN ( FALSE ) ;
}
#endif /* __BBS_MILL__>=1 */

