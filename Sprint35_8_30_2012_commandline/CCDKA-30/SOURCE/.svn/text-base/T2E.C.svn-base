/* -Z -S -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2E.C ***********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2adefs.h>
#include <c2vmcrs.h>
#include <dmldefs.h>
#include <m2cdefs.h>
#include <t2cdefs.h>
#include <t2ddefs.h>
#include <t2edefs.h>
#include <t2epriv.h>
#include <t2attrd.h>
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC DML_LIST t2e_build_looplist __(( T2_REGION, DML_LIST, REAL )) ;
STATIC T2_EDGE  t2e_append_edge_epts __(( T2_LOOP, T2_EDGE, PT2, PT2, 
            PT2, PT2, INT, REAL )) ;
STATIC T2_EDGE  t2e_append_edge_at __(( T2_LOOP, T2_EDGE, BOOLEAN, BOOLEAN )) ;
STATIC void     t2e_rearrange __(( DML_LIST )) ;
STATIC DML_ITEM t2e_clst_edge __(( DML_LIST, PT2, PT2, PT2, PT2, 
            INT*, REAL* )) ;
/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2e_build_regions ( curvelist1, s1, curvelist2, s2, 
            tol, regionlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curvelist1, curvelist2, regionlist ;
INT s1, s2 ;
REAL tol ;
{
    DML_LIST looplist, outlist, edgelist ;
    DML_ITEM item ;

    edgelist = dml_create_list ();
    if ( edgelist == NULL ) 
        RETURN ( NULL ) ;
    edgelist = t2e_append_curvelist ( curvelist1, s1, edgelist ) ;
    edgelist = t2e_append_curvelist ( curvelist2, s2, edgelist ) ;

    looplist = t2e_build_looplist ( NULL, edgelist, tol ) ;
    if ( looplist == NULL ) 
        RETURN ( NULL ) ;
    DML_WALK_LIST ( looplist, item ) 
        t2c_orient_loop ( DML_RECORD(item), TRUE ) ;
    outlist = t2d_create_regions ( looplist, regionlist ) ;
    t2e_decomp_edgelist ( edgelist, curvelist1, s1, curvelist2, s2 ) ; 
    dml_free_list ( edgelist ) ;
    dml_free_list ( looplist ) ;
    RETURN ( outlist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2e_build_loops ( curvelist1, s1, curvelist2, s2, 
            tol, looplist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curvelist1, curvelist2, looplist ;
INT s1, s2 ;
REAL tol ;
{
    DML_LIST l_list, edgelist ;
    DML_ITEM item ;

    edgelist = dml_create_list ();
    if ( edgelist == NULL ) 
        RETURN ( NULL ) ;
    edgelist = t2e_append_curvelist ( curvelist1, s1, edgelist ) ;
    edgelist = t2e_append_curvelist ( curvelist2, s2, edgelist ) ;

    if ( looplist == NULL ) {
        looplist = t2e_build_looplist ( NULL, edgelist, tol ) ;
        if ( looplist == NULL ) 
            RETURN ( NULL ) ;
        DML_WALK_LIST ( looplist, item ) 
            t2c_orient_loop ( DML_RECORD(item), TRUE ) ;
#ifdef CCDK_DEBUG
{
DML_ITEM item ;
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    DML_WALK_LIST ( looplist, item ) {
        paint_loop ( DML_RECORD(item), 10 ) ;
        getch ();
    }
}
else
    DISPLAY-- ;
}
#endif
    }
    else {
        l_list = t2e_build_looplist ( NULL, edgelist, tol ) ;
        if ( l_list == NULL ) 
            RETURN ( NULL ) ;
        DML_WALK_LIST ( l_list, item ) 
            t2c_orient_loop ( DML_RECORD(item), TRUE ) ;
#ifdef CCDK_DEBUG
{
DML_ITEM item ;
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    DML_WALK_LIST ( l_list, item ) {
        paint_loop ( DML_RECORD(item), 12 ) ;
        getch ();
    }
}
else
    DISPLAY-- ;
}
#endif
        dml_append_list ( looplist, l_list ) ;
        dml_free_list ( l_list ) ;
    }
    t2e_decomp_edgelist ( edgelist, curvelist1, s1, curvelist2, s2 ) ; 
    dml_free_list ( edgelist ) ;
    RETURN ( looplist ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_LIST t2e_build_looplist ( region, edgelist, tol )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
DML_LIST edgelist ;
REAL tol ;
{
    DML_LIST looplist = dml_create_list(), openlist = NULL ;
    T2_LOOP loop ;
    DML_ITEM item ;

    if ( looplist == NULL ) 
        RETURN ( NULL ) ;
    while ( DML_LENGTH(edgelist) ) {
        loop = t2e_build_loop ( region, edgelist, tol ) ; 
        if ( loop != NULL ) {
            if ( T2_LOOP_CLOSED(loop) ) {
                 dml_append_data ( looplist, loop ) ;
            }
            else {
                if ( openlist == NULL ) 
                    openlist = dml_create_list() ;
                dml_append_data ( openlist, loop ) ;
            }
        }
    }
    /* Find the enclosing loop and make it the first one on the list */
    if ( openlist != NULL ) {
        DML_WALK_LIST ( openlist, item ) {
            loop = (T2_LOOP)DML_RECORD(item) ;
            dml_append_list ( edgelist, T2_LOOP_EDGE_LIST(loop) ) ;
            t2d_free_loop ( loop ) ;
        }
        dml_free_list ( openlist ) ;
        openlist = NULL ;
    }
    RETURN ( looplist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2e_decomp_edgelist ( edgelist, curvelist1, s1, 
        curvelist2, s2 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST edgelist, curvelist1, curvelist2 ;
INT s1, s2 ;
{
    DML_ITEM item ;
    T2_EDGE edge ;
    DML_WALK_LIST ( edgelist, item ) {
        edge = (T2_EDGE)DML_RECORD(item) ;
        if ( T2_EDGE_ATTR(edge) & s1 ) 
            dml_append_data ( curvelist1, T2_EDGE_CURVE(edge) ) ;
        else if ( T2_EDGE_ATTR(edge) & s2 ) 
            dml_append_data ( curvelist2, T2_EDGE_CURVE(edge) ) ;
        T2_EDGE_CURVE(edge) = NULL ;
        t2d_free_edge ( edge ) ;
        edge = NULL ;
        DML_RECORD(item) = NULL ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2e_build_loop ( region, edgelist, tol )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
DML_LIST edgelist ;
REAL tol ;
{
    T2_LOOP loop = t2d_create_loop ( region, dml_create_list() ) ;
    if ( loop != NULL ) 
        t2e_append_edgelist ( loop, edgelist, tol ) ;
#ifdef CCDK_DEBUG
{
DML_ITEM item ;
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        paint_edge ( DML_RECORD(item), 12 ) ;
        getch () ;
    }
}
else
    DISPLAY-- ;
}
#endif
    t2e_rearrange ( T2_LOOP_EDGE_LIST(loop) ) ;
#ifdef CCDK_DEBUG
{
DML_ITEM item ;
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        paint_edge ( DML_RECORD(item), 13 ) ;
        getch () ;
    }
}
else
    DISPLAY-- ;
}
    t2c_loop_closed ( loop ) ;
#endif
    (void) t2d_adjust_epts_loop ( loop, tol ) ;
    t2c_loop_closed ( loop ) ;
#ifdef CCDK_DEBUG
{
DML_ITEM item ;
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        paint_edge ( DML_RECORD(item), 10 ) ;
        getch () ;
    }
}
else
    DISPLAY-- ;
}
#endif
    RETURN ( loop ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2e_rearrange ( edgelist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST edgelist ;
{
    T2_EDGE edge0, edge1 ;
    DML_ITEM item, item0, item1 ;

    if ( edgelist == NULL || DML_LENGTH(edgelist) <= 1 ) 
        RETURN ;

    edge0 = DML_FIRST_RECORD(edgelist) ;
    edge1 = DML_LAST_RECORD(edgelist) ;
    if ( !( T2_EDGE_ATTR(edge0) & T2_EDGE_ATTR(edge1) ) )
        RETURN ;

    item = NULL ;
    for ( item0 = DML_FIRST(edgelist), item1 = DML_NEXT(item0) ;
        item1 != NULL && item == NULL ; 
        item0 = item1, item1 = DML_NEXT(item1) ) {
        edge0 = DML_RECORD(item0) ;
        edge1 = DML_RECORD(item1) ;
        if ( !( T2_EDGE_ATTR(edge0) & T2_EDGE_ATTR(edge1) ) )
            item = item1 ;
    }
    dml_rearrange ( edgelist, item ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2e_append_edgelist ( loop, edgelist, tol ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
DML_LIST edgelist ;
REAL tol ;
{
    PT2 p0, p1, q0, q1 ;
    T2_EDGE edge ;
    DML_ITEM item ;
    INT pos ;
    REAL dist ;

    if ( DML_LENGTH(T2_LOOP_EDGE_LIST(loop)) ) {
        if ( !t2c_loop_ept0 ( loop, p0 ) || !t2c_loop_ept1 ( loop, p1 ) )
            RETURN ( FALSE ) ;
    }

    if ( t2e_append_edge_epts ( loop, DML_FIRST_RECORD(edgelist), 
        p0, p1, NULL, NULL, -1, tol ) == NULL ) 
        RETURN ( FALSE ) ;
    dml_remove_first ( edgelist ) ;

    while ( /* C2V_DISTL1(p0,p1) > tol && */ DML_LENGTH(edgelist) ) { 
        item = t2e_clst_edge ( edgelist, p0, p1, q0, q1, &pos, &dist ) ;
        if ( dist < C2V_DIST ( p0, p1 ) ) {
            edge = t2e_append_edge_epts ( loop, DML_RECORD(item), 
                p0, p1, q0, q1, pos, tol ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop, 9 ) ;
    if ( edge != NULL ) 
        paint_edge ( edge, 10 ) ;
    getch ();
}
else
    DISPLAY-- ;
#endif
            dml_remove_item ( edgelist, item ) ;
            item = NULL ;
            if ( edge == NULL ) 
                break ;
        }
        else 
            RETURN ( TRUE ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_ITEM t2e_clst_edge ( edgelist, p0, p1, q0, q1, pos, dist_min ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST edgelist ;
PT2 p0, p1, q0, q1 ;
INT *pos ;
REAL *dist_min ;
{
    DML_ITEM item, item0 ;
    T2_EDGE edge ;
    PT2 r0, r1 ;
    REAL d, dist ;
    INT pos0 ;

    *dist_min = 0.0 ;

    if ( edgelist == NULL || DML_LENGTH(edgelist) == 0 ) 
        RETURN ( NULL ) ;
    item0 = NULL ;
    DML_WALK_LIST ( edgelist, item ) {
        edge = DML_RECORD(item) ;
        t2c_ept0 ( edge, r0 ) ;
        t2c_ept1 ( edge, r1 ) ;
        dist = C2V_DIST ( p1, r0 ) ;
        pos0 = 10 ;
        d = C2V_DIST ( p1, r1 ) ;
        if ( d < dist ) {
            dist = d ;
            pos0 = 11 ;
        }
        d = C2V_DIST ( p0, r0 ) ;
        if ( d < dist ) {
            dist = d ;
            pos0 = 0 ;
        }
        d = C2V_DIST ( p0, r1 ) ;
        if ( d < dist ) {
            dist = d ;
            pos0 = 1 ;
        }
        if ( item0 == NULL || dist < (*dist_min) ) {
            item0 = item ;
            *dist_min = dist ;
            C2V_COPY ( r0, q0 ) ;
            C2V_COPY ( r1, q1 ) ;
            *pos = pos0 ;
        }
    }
    RETURN ( item0 ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2e_append_edge_epts ( loop, edge, p0, p1, q0, q1, pos, tol ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_EDGE edge ;
PT2 p0, p1, q0, q1 ;    /* p0 and p1 are the endpoints of the loop */
INT pos;
REAL tol ;
{
    PT2 ept0, ept1 ;

    if ( tol < BBS_TOL ) 
        tol = BBS_TOL ;

    if ( q0 == NULL ) 
        t2c_ept0 ( edge, ept0 ) ;
    else
        C2V_COPY ( q0, ept0 ) ;
    if ( q1 == NULL ) 
        t2c_ept1 ( edge, ept1 ) ;
    else
        C2V_COPY ( q1, ept1 ) ;

    if ( !DML_LENGTH(T2_LOOP_EDGE_LIST(loop)) ) {
        C2V_COPY ( ept0, p0 ) ;
        C2V_COPY ( ept1, p1 ) ;
        edge = t2e_append_edge_at ( loop, edge, TRUE, FALSE ) ;
    }
/* YG 120392
    else if ( ( pos < 0 || pos == 10 ) && C2V_DIST ( p1, ept0 ) < tol ) {
        C2V_COPY ( ept1, p1 ) ;
        edge = t2e_append_edge_at ( loop, edge, TRUE, TRUE ) ;
    }
    else if ( ( pos < 0 || pos == 11 ) && C2V_DIST ( p1, ept1 ) < tol ) {
        C2V_COPY ( ept0, p1 ) ;
        edge = t2e_append_edge_at ( loop, edge, FALSE, TRUE ) ;
    }
    else if ( ( pos < 0 || pos == 0 ) && C2V_DIST ( p0, ept0 ) < tol ) {
        C2V_COPY ( ept1, p0 ) ;
        edge = t2e_append_edge_at ( loop, edge, FALSE, FALSE ) ;
    }
    else if ( ( pos < 0 || pos == 1 ) && C2V_DIST ( p0, ept1 ) < tol ) {
        C2V_COPY ( ept0, p0 ) ;
        edge = t2e_append_edge_at ( loop, edge, TRUE, FALSE ) ;
    }
*/
    else if ( pos == 10 ) {
        C2V_COPY ( ept1, p1 ) ;
        edge = t2e_append_edge_at ( loop, edge, TRUE, TRUE ) ;
    }
    else if ( pos == 11 ) {
        C2V_COPY ( ept0, p1 ) ;
        edge = t2e_append_edge_at ( loop, edge, FALSE, TRUE ) ;
    }
    else if ( pos == 0 ) {
        C2V_COPY ( ept1, p0 ) ;
        edge = t2e_append_edge_at ( loop, edge, FALSE, FALSE ) ;
    }
    else if ( pos == 1 ) {
        C2V_COPY ( ept0, p0 ) ;
        edge = t2e_append_edge_at ( loop, edge, TRUE, FALSE ) ;
    }
    else
        RETURN ( NULL ) ;
/*
    if ( edge != NULL ) 
        T2_LOOP_CLOSED(loop) = ( C2V_DIST ( p0, p1 ) < tol ) ;
*/
    RETURN ( edge ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2e_append_edge_at ( loop, edge, dir, after ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_EDGE edge ;
BOOLEAN dir ;
BOOLEAN after ;
{
    if ( edge == NULL ) 
        RETURN ( NULL ) ;
    if ( dir != 1 ) 
        T2_EDGE_DIR(edge) = -T2_EDGE_DIR(edge) ;
    if ( DML_LENGTH(T2_LOOP_EDGE_LIST(loop)) == 0 ) 
        c2a_box_copy ( T2_EDGE_BOX(edge), T2_LOOP_BOX(loop) ) ;
    else
        c2a_box_append ( T2_LOOP_BOX(loop), T2_EDGE_BOX(edge) ) ;

    if ( after )
        dml_insert_prior ( T2_LOOP_EDGE_LIST(loop), NULL, edge ) ;
    else
        dml_insert_after ( T2_LOOP_EDGE_LIST(loop), NULL, edge ) ;
    T2_EDGE_LOOP(edge) = loop ;
/*
    after means insert after the last item == prior NULL, and vice versa */

    RETURN ( edge ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2e_append_curvelist ( curvelist, s, edgelist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curvelist, edgelist ;
ATTR s ;
{
    DML_ITEM item, item1 ;
    C2_CURVE curve ;
    T2_EDGE edge ;

    if ( curvelist == NULL ) 
        RETURN ( edgelist ) ;
    if ( edgelist == NULL ) 
        edgelist = dml_create_list();
    if ( edgelist == NULL ) 
        RETURN ( NULL ) ;
    m2c_break_curves ( curvelist ) ;
    for ( item = DML_FIRST(curvelist) ; item != NULL ; item = item1 ) {
        item1 = DML_NEXT(item) ;
        curve = DML_RECORD(item) ;
        edge = t2d_create_edge ( NULL, curve, 1 ) ;
        if ( edge == NULL || dml_append_data ( edgelist, edge ) == NULL ) 
            RETURN ( NULL ) ;
        T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | s ;
        DML_RECORD(item) = NULL ;
        dml_remove_item ( curvelist, item ) ;
    }
    RETURN ( edgelist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2e_split_loop ( loop, part, rough ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop, *rough, *part ;
{
    DML_LIST partlist, roughlist ;
    DML_ITEM item ;
    T2_EDGE edge ;

    if ( part != NULL ) 
        *part = NULL ;
    if ( rough != NULL ) 
        *rough = NULL ;
    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ;

    partlist = ( part == NULL ) ? NULL : dml_create_list () ;
    roughlist = ( rough == NULL ) ? NULL : dml_create_list () ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        if ( T2_EDGE_ATTR(edge) & T2_ATTR_ROUGH ) {
            if ( roughlist != NULL ) 
                dml_append_data ( roughlist, t2d_copy_edge ( edge ) ) ;
        }
        else {
            if ( partlist != NULL ) 
                dml_append_data ( partlist, t2d_copy_edge ( edge ) ) ;
        }
    }
    if ( partlist != NULL && DML_LENGTH(partlist) ) {
        *part = t2d_create_loop ( NULL, dml_create_list() ) ;
        t2e_append_edgelist ( *part, partlist, 10.0 * BBS_TOL ) ;
        T2_LOOP_PARENT(*part) = loop ; 
        T2_LOOP_ATTR(*part) = T2_ATTR_PART ;
    }
    if ( roughlist != NULL && DML_LENGTH(roughlist) ) {
        *rough = t2d_create_loop ( NULL, dml_create_list() ) ;
        t2e_append_edgelist ( *rough, roughlist, 10.0 * BBS_TOL ) ;
        T2_LOOP_PARENT(*rough) = loop ; 
        T2_LOOP_ATTR(*rough) = T2_ATTR_ROUGH ;
    }
    dml_free_list ( partlist ) ;
    dml_free_list ( roughlist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2e_join_loops ( loop0, loop1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0, loop1 ;
{
    T2_LOOP loop ;

    loop = t2d_create_loop ( NULL, dml_create_list () ) ;
    if ( loop0 != NULL ) 
        t2d_append_edgelist ( loop, T2_LOOP_EDGE_LIST(loop0) ) ;
    if ( loop1 != NULL ) 
        t2d_append_edgelist ( loop, T2_LOOP_EDGE_LIST(loop1) ) ;
    RETURN ( loop ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

