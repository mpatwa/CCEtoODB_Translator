/* -S -Z -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2B5.C **********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <dmldefs.h>
#include <c2ddefs.h>
#include <c2dpriv.h>
#include <c2vdefs.h>
#include <c2vmcrs.h>
#include <t2bdefs.h>
#include <t2bpriv.h>
#include <t2cdefs.h>
#include <t2ddefs.h>
#include <t2dpriv.h>
#include <t2attrd.h>
#include <t2linkm.h>
#include <t2odm.h>
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC BOOLEAN  t2b_offset_open_loop __(( REAL, DML_LIST, DML_LIST, 
            DML_LIST )) ;
STATIC T2_VTX_STATUS t2b_round_convex __(( T2_EDGE, T2_EDGE, 
            DML_ITEM, DML_LIST, BOOLEAN )) ;
STATIC BOOLEAN  t2b_edge_convexity __(( T2_EDGE, PARM, REAL*, INT* )) ;
STATIC T2_EDGE  t2b_zero_arc __(( T2_EDGE, T2_EDGE, PT2, PT2 )) ;
STATIC BOOLEAN  t2b_offset_edgelist __(( DML_LIST DUMMY0 , REAL DUMMY1 , 
            T2_OFFSET_DESC DUMMY2 , DML_LIST DUMMY3 )) ;
STATIC INT      t2b_offset_curve __(( C2_CURVE, REAL, T2_OFFSET_DESC, 
            DML_LIST )) ;
STATIC BOOLEAN t2b_offset_pass13 __(( T2_LOOP )) ;
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
EXTERN BOOLEAN DIR ;
GLOBAL INT T2B_O_PASS13 = 0 ;
#endif

/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2b_offset_pass0 ( loop, dist, dist_list, dist1_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL dist ;
DML_LIST dist_list, dist1_list ;
{
    DML_LIST edgelist = dml_create_list() ;
    T2_EDGE edge1, edge ;
    DML_ITEM item ;
    REAL dist1 ;
    T2_OD od ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = (T2_EDGE)DML_RECORD(item) ;
        edge1 = t2d_copy_edge ( edge ) ;
        od = t2b_od_lookup_list ( dist_list, edge, dist, &dist1 ) ;
        if ( od != NULL ) 
            t2b_append_od_list ( edge1, T2_OD_PT(od), dist1, dist1_list ) ;
        T2_EDGE_LOOP(edge1) = NULL ;
        if ( edge1 == NULL || dml_append_data ( edgelist, edge1 ) == NULL ) 
            RETURN ( NULL ) ;
    }

    if ( !t2c_loop_closed ( loop ) && 
        !t2b_offset_open_loop ( dist, dist_list, dist1_list, edgelist ) ) 
            RETURN ( NULL ) ;

    RETURN ( edgelist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2b_offset_pass1 ( loop, edgelist, dist, 
        dist1_list, loopdist_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
DML_LIST edgelist ;
REAL dist ;
DML_LIST dist1_list, loopdist_list ;
{
    DML_LIST looplist ;
    T2_EDGE edge1, edge2, edge, prev_edge ;
    T2_LOOP loop1 ;
    DML_ITEM item, prev_item ;
    REAL dist1, dist2 ;
#ifndef __BRLNDC__
    BOOLEAN diff_attrs ;
#endif
#ifdef CCDK_DEBUG
    INT i = 0 ;
#endif

    looplist = dml_create_list () ;
    loop1 = t2d_create_loop ( NULL, dml_create_list() ) ;
    if ( edgelist == NULL || loop1 == NULL ) 
        RETURN ( NULL ) ;
    T2_LOOP_PARENT(loop1) = ( T2_LOOP_PARENT(loop) == NULL ) ? 
        loop : T2_LOOP_PARENT(loop) ;
    item = dml_first ( edgelist ) ;
    if ( item == NULL )
        RETURN ( NULL ) ;
    edge1 = (T2_EDGE)DML_RECORD(item) ;
    if ( t2b_od_lookup_list ( dist1_list, edge1, dist, &dist1 ) != NULL ) 
        t2b_append_od_list ( loop1, NULL, dist1, loopdist_list ) ;
    T2_EDGE_LOOP(edge1) = loop1 ;
    if ( t2d_append_edge ( loop1, edge1, FALSE ) == NULL ) 
        RETURN ( NULL ) ;
    if ( dml_append_data ( looplist, loop1 ) == NULL ) 
        RETURN ( NULL ) ;

    DML_FOR_LOOP ( dml_next( item ), item ) {
        edge2 = (T2_EDGE)DML_RECORD(item) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    BOOLEAN dir = DIR ;
    DIR = TRUE ;
    paint_edge ( edge1, 10 ) ;
    getch();
    paint_edge ( edge2, 11 ) ;
    getch();
    DIR = dir ;
}
else
    DISPLAY-- ;
#endif
        t2b_od_lookup_list ( dist1_list, edge2, dist, &dist2 ) ;
        t2b_round_convex ( edge1, edge2, item, edgelist, 
            IS_SMALL(dist1-dist2) ) ;
#ifdef CCDK_DEBUG
    i++ ;
#endif
        diff_attrs = ( ( T2_EDGE_ATTR(edge1) & T2_ATTR_PART ) && 
              ( T2_EDGE_ATTR(edge2) & T2_ATTR_ROUGH ) ) ||
            ( ( T2_EDGE_ATTR(edge2) & T2_ATTR_PART ) && 
              ( T2_EDGE_ATTR(edge1) & T2_ATTR_ROUGH ) ) ;

        if ( ( T2_EDGE_VTX(edge1) == T2_VTX_CONCAVE ) || 
            !IS_SMALL(dist1-dist2) /* || diff_attrs */ ) {
/* new code 10-15-92 */
            prev_item = DML_PREV(item) ;
            prev_edge = (T2_EDGE) DML_RECORD ( prev_item ) ;
            if ( ( loop1 != NULL ) && 
                ( prev_edge != t2c_last_edge ( loop1 ) ) ) {
                t2d_free_edge ( prev_edge ) ;
                DML_RECORD(prev_item) = NULL ;
            }
/* new code 10-15-92 */
            loop1 = t2d_create_loop ( NULL, dml_create_list() ) ;
            T2_LOOP_PARENT(loop1) = loop ;
            if ( loop1 == NULL ) 
                RETURN ( NULL ) ;
            if ( dml_append_data ( looplist, loop1 ) == NULL ) 
                RETURN ( NULL ) ;
            if ( !IS_SMALL(dist-dist2) )
                t2b_append_od_list ( loop1, NULL, dist2, loopdist_list ) ;
        }

        else if ( T2_EDGE_VTX(edge1) == T2_VTX_CONVEX ) {
            edge = (T2_EDGE)DML_RECORD ( DML_PREV ( item ) ) ;
            T2_EDGE_LOOP(edge) = loop1 ;
            T2_EDGE_VTX(edge1) = T2_VTX_TANGENT ;
            if ( t2d_append_edge ( loop1, edge, FALSE ) == NULL ) 
                RETURN ( NULL ) ;
        }

        T2_EDGE_LOOP(edge2) = loop1 ;
        if ( t2d_append_edge ( loop1, edge2, FALSE ) == NULL ) 
            RETURN ( NULL ) ;
        edge1 = edge2 ;
        dist1 = dist2 ;
    }

    if ( T2_LOOP_CLOSED(loop) ) {
        edge2 = DML_FIRST_RECORD(edgelist) ;
        t2b_od_lookup_list ( dist1_list, edge2, dist, &dist2 ) ;
        t2b_round_convex ( edge1, edge2, item, edgelist, 
            IS_SMALL(dist1-dist2) ) ;
#ifdef CCDK_DEBUG
    i++ ;
#endif
        if ( T2_EDGE_VTX(edge1) == T2_VTX_CONVEX && IS_SMALL(dist1-dist2) ) {
            edge = DML_LAST_RECORD(edgelist) ;
            T2_EDGE_LOOP(edge) = loop1 ;
            if ( t2d_append_edge ( loop1, edge, FALSE ) == NULL ) 
                RETURN ( NULL ) ;
        }
    }

    dml_free_list ( edgelist ) ;
    edgelist = NULL ;
    RETURN ( looplist ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_offset_open_loop ( dist, dist_list, dist1_list, edgelist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL dist ;
DML_LIST dist_list, dist1_list ;
DML_LIST edgelist ;
{
    T2_EDGE edge1, edge ;
    PT2 pt, tan0, tan1 ;
    REAL dist1 ;
    C2_CURVE arc ;

    edge = DML_FIRST_RECORD(edgelist) ; 

    //if ( t2c_length ( edge, NULL, NULL ) >= fabs(dist) + BBS_TOL ) 
    {
        t2c_ept_tan0 ( edge, pt, tan1 ) ;
        C2V_NEGATE ( tan1, tan0 ) ;
        arc = c2d_zero_arc ( pt, tan0, tan1 ) ;

        edge1 = t2d_create_edge ( NULL, arc, 1 ) ;
        if ( arc == NULL || edge1 == NULL ) 
            RETURN ( FALSE ) ;
        T2_EDGE_VTX(edge1) = T2_VTX_TANGENT ;
        T2_EDGE_ATTR(edge1) = T2_EDGE_ATTR(edge1) | T2_ATTR_FIRST ;
        if ( dml_insert_after ( edgelist, NULL, edge1 ) == NULL ) 
            RETURN ( FALSE ) ;
        if ( t2b_od_lookup_list ( dist_list, edge, dist, &dist1 ) )
            t2b_append_od_list ( edge1, NULL, dist1, dist1_list ) ;
    }

    edge = DML_LAST_RECORD(edgelist) ;

    //if ( t2c_length ( edge, NULL, NULL ) >= fabs(dist) + BBS_TOL ) 
    {
        t2c_ept_tan1 ( edge, pt, tan0 ) ;
        C2V_NEGATE ( tan0, tan1 ) ;
        arc = c2d_zero_arc ( pt, tan0, tan1 ) ;
        edge1 = t2d_create_edge ( NULL, arc, 1 ) ;
        if ( arc == NULL || edge1 == NULL ) 
            RETURN ( FALSE ) ;
        T2_EDGE_VTX(edge) = T2_VTX_TANGENT ;
        T2_EDGE_ATTR(edge1) = T2_EDGE_ATTR(edge1) | T2_ATTR_FIRST ;
        if ( dml_append_data ( edgelist, edge1 ) == NULL ) 
            RETURN ( FALSE ) ;
        if ( t2b_od_lookup_list ( dist_list, edge, dist, &dist1 ) != NULL )
            t2b_append_od_list ( edge1, NULL, dist1, dist1_list ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_VTX_STATUS t2b_round_convex ( edge1, edge2, item, 
            edgelist, zero_arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge1, edge2 ;
DML_ITEM item ;
DML_LIST edgelist ;
BOOLEAN zero_arc ;
{
    PT2 tan1, tan2 ;
    T2_EDGE edge ;

    t2b_vertex_convexity ( edge1, edge2, tan1, tan2 ) ;

    if ( T2_EDGE_VTX(edge1) == T2_VTX_CONVEX && zero_arc ) {
        edge = t2b_zero_arc ( edge1, edge2, tan1, tan2 ) ;
        dml_insert_prior ( edgelist, item, edge ) ;
    }
    RETURN ( T2_EDGE_VTX(edge1) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2b_zero_arc ( edge1, edge2, tan1, tan2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge1, edge2 ;
PT2 tan1, tan2 ;
{
    T2_EDGE edge ;
    PT2 p1, p2, ctr ;

    t2c_ept1 ( edge1, p1 ) ;
    t2c_ept0 ( edge2, p2 ) ;
    C2V_MID_PT ( p1, p2, ctr ) ;
    edge = t2d_create_edge ( T2_EDGE_LOOP(edge1), 
        c2d_zero_arc ( ctr, tan1, tan2 ), 1 ) ;
    if ( ( T2_EDGE_ATTR(edge1) & T2_ATTR_PART ) && 
        ( T2_EDGE_ATTR(edge2) & T2_ATTR_PART ) )
        T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_ATTR_PART ;
/* yg 121792 */
    else if ( ( T2_EDGE_ATTR(edge1) & T2_ATTR_ROUGH ) || 
        ( T2_EDGE_ATTR(edge2) & T2_ATTR_ROUGH ) )
        T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_ATTR_ROUGH ;
/* yg 121792 */
    T2_EDGE_PARENT(edge) = NULL ;
    T2_EDGE_VTX(edge) = T2_VTX_TANGENT ;
    RETURN ( edge ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_VTX_STATUS t2b_vertex_convexity ( edge1, edge2, tan1, tan2 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge1, edge2 ;
PT2 tan1, tan2 ;
{
    REAL s, k1, k2 ;
    INT c1, c2 ;
    PT2 ttan1, ttan2 ;
#ifdef CCDK_DEBUG
    PT2 pt1, pt2, vec ;
    REAL s1, s2 ;
#endif
    REAL zero = BBS_ZERO ;

    /*
    if ( T2_EDGE_VTX(edge1) != T2_VTX_UNDEF ) 
        RETURN ( T2_EDGE_VTX(edge1) ) ;
    */
    if ( T2_EDGE_IS_ZERO_ARC(edge1) || T2_EDGE_IS_ZERO_ARC(edge2) ) {
        T2_EDGE_VTX(edge1) = T2_VTX_TANGENT ;
        RETURN ( T2_EDGE_VTX(edge1) ) ;
    }

    t2c_etan1 ( edge1, ttan1 ) ;
    t2c_etan0 ( edge2, ttan2 ) ;
    c2v_normalize ( ttan1, ttan1 ) ;
    c2v_normalize ( ttan2, ttan2 ) ;

#ifdef CCDK_DEBUG
    t2c_ept1 ( edge1, pt1 ) ;
    t2c_ept0 ( edge2, pt2 ) ;
    C2V_SUB ( pt2, pt1, vec ) ;
    s1 = C2V_CROSS ( ttan1, vec ) ;
    s2 = C2V_CROSS ( ttan2, vec ) ;
#endif

    s = C2V_CROSS ( ttan1, ttan2 ) ;
    if ( s > zero ) {

#ifdef CCDK_DEBUG
        if ( DIS_LEVEL == -4 ) {
            if ( ( T2_EDGE_VTX(edge1) != T2_VTX_CONVEX ) && 
                 ( T2_EDGE_VTX(edge1) != T2_VTX_UNDEF ) )
                getch () ;
        }
#endif
        T2_EDGE_VTX(edge1) = T2_VTX_CONVEX ;
    }

    else if ( fabs(s) < zero && C2V_DOT ( ttan1, ttan2 ) < 0.0 ) {
        t2b_edge_convexity ( edge1, T2_EDGE_PARM1(edge1), &k1, &c1 ) ;
        t2b_edge_convexity ( edge2, T2_EDGE_PARM0(edge2), &k2, &c2 ) ;
/* Second degree continuity but opposite tangents */
        if ( c1 == 0 && c2 == 0 && fabs(k1-k2) < zero ) 
            T2_EDGE_VTX(edge1) = T2_VTX_CONVEX ;

        if ( ( c1 == 1 ) || ( c1 == 0 ) ) {
            if ( ( c2 == 1 ) || ( c2 == 0 ) ) 
                T2_EDGE_VTX(edge1) = T2_VTX_CONCAVE ;
            else {
                if ( fabs ( k2 ) < fabs ( k1 ) )
                    T2_EDGE_VTX(edge1) = T2_VTX_CONCAVE ;
                else
                    T2_EDGE_VTX(edge1) = T2_VTX_CONVEX ;
            }
        }
        else {
            if ( ( c2 == -1 ) || ( c2 == 0 ) ) 
                T2_EDGE_VTX(edge1) = T2_VTX_CONVEX ;
            else {
                if ( fabs ( k2 ) > fabs ( k1 ) )
                    T2_EDGE_VTX(edge1) = T2_VTX_CONCAVE ;
                else
                    T2_EDGE_VTX(edge1) = T2_VTX_CONVEX ;
            }
        }
    }
    else if ( s < - zero ) {
#ifdef CCDK_DEBUG
        if ( DIS_LEVEL == -4 ) {
            if ( ( T2_EDGE_VTX(edge1) != T2_VTX_CONCAVE ) && 
                 ( T2_EDGE_VTX(edge1) != T2_VTX_UNDEF ) )
                getch () ;
        }
#endif
        T2_EDGE_VTX(edge1) = T2_VTX_CONCAVE ;
    }
    else {
#ifdef CCDK_DEBUG
        if ( DIS_LEVEL == -4 ) {
            if ( ( T2_EDGE_VTX(edge1) != T2_VTX_TANGENT ) && 
                 ( T2_EDGE_VTX(edge1) != T2_VTX_UNDEF ) )
                getch () ;
        }
#endif
        T2_EDGE_VTX(edge1) = T2_VTX_TANGENT ;
    }
    if ( tan1 != NULL ) {
        C2V_COPY ( ttan1, tan1 ) ;
    }
    if ( tan2 != NULL ) {
        C2V_COPY ( ttan2, tan2 ) ;
    }
    RETURN ( T2_EDGE_VTX(edge1) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_edge_convexity ( edge, parm, k, c )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM parm ;
REAL *k ;
INT *c ;
{
    t2c_curvature ( edge, parm, k ) ;

    if ( IS_SMALL(*k) )
        *c = 0 ;
    else if ( *k > 0.0 )
        *c = 1 ;
    else 
        *c = -1 ;

    if ( T2_EDGE_REVERSED(edge) )
    {
        *k = - *k ;
        *c = - *c ;
    }

    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2b_offset_pass2 ( inlist, dist, desc, dist_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST inlist ;
REAL dist ;
T2_OFFSET_DESC desc ;
DML_LIST dist_list ;
{
    DML_LIST looplist = dml_create_list(), edgelist, edge1list ;
    BOOLEAN status = TRUE ;
    DML_ITEM item, item1, item2 ;
    T2_LOOP loop1, loop2 ;
    REAL dist1 ;

    if ( looplist == NULL ) 
        RETURN ( NULL ) ;
    for ( item = DML_FIRST(inlist) ; item != NULL ; item = item2 ) {
        item2 = DML_NEXT(item) ;
        loop1 = (T2_LOOP)DML_RECORD(item) ;
        t2b_od_lookup_list ( dist_list, loop1, dist, &dist1 ) ;
        edgelist = dml_create_list() ;
        if ( edgelist == NULL ) 
            RETURN ( NULL ) ;
        status = t2b_offset_edgelist ( T2_LOOP_EDGE_LIST(loop1), 
            fabs(dist1), desc, edgelist ) && status ;

        dml_destroy_list ( T2_LOOP_EDGE_LIST(loop1), (PF_ACTION) t2d_free_edge ) ;
        T2_LOOP_EDGE_LIST(loop1) = NULL ;

        if ( status ) {
            T2_LOOP_EDGE_LIST(loop1) = edgelist ;
            DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop1), item1 ) {
                T2_EDGE_LOOP( ( T2_EDGE ) DML_RECORD(item1) ) = loop1 ;
            }
            t2c_loop_box ( loop1 ) ;
            if ( dml_append_data ( looplist, loop1 ) == NULL ) 
                RETURN ( NULL ) ;
        }

        else {
            while ( DML_LENGTH(edgelist) ) {
                edge1list = dml_create_list();
                if ( edge1list == NULL ) 
                    RETURN ( NULL ) ;
                t2d_sort_edgelist ( edgelist, BBS_TOL, edge1list ) ;
                if ( DML_LENGTH(edgelist) /* not the last one */ ) {
                    loop2 = t2d_create_loop ( NULL, edge1list ) ;
                    if ( loop2 == NULL ) 
                        RETURN ( NULL ) ;
                    t2d_close_extend ( loop2, 10.0 * BBS_TOL ) ;
                    T2_LOOP_PARENT(loop2) = loop1 ;
                    if ( dml_append_data ( looplist, loop2 ) == NULL ) 
                        RETURN ( NULL ) ;
                }
                else {
                    T2_LOOP_EDGE_LIST(loop1) = edge1list ;
                    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop1), item1 ) {
                        T2_EDGE_LOOP( (T2_EDGE)DML_RECORD(item1) ) = loop1 ;
                    }
                    t2c_loop_box ( loop1 ) ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_loop ( loop1, 12 ) ;
        getch();
    }
    else
        DISPLAY-- ;
#endif
                    t2c_loop_closed ( loop1 ) ;
                    t2d_close_extend ( loop1, 10.0 * BBS_TOL ) ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_loop ( loop1, 11 ) ;
        getch();
    }
    else
        DISPLAY-- ;
#endif
                    if ( dml_append_data ( looplist, loop1 ) == NULL ) 
                        RETURN ( NULL ) ;
                }
            }
            dml_free_list ( edgelist ) ;
            edgelist = NULL ;
        }
        if ( T2_LOOP_EDGE_LIST(loop1) == NULL || 
            DML_LENGTH(T2_LOOP_EDGE_LIST(loop1)) == 0 ) {
            t2d_free_loop ( loop1 ) ;
        }
    }
    RETURN ( looplist ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_offset_edgelist ( edgelist, dist, desc, offset_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST edgelist ;
REAL dist ;
T2_OFFSET_DESC desc ;
DML_LIST offset_list ;
{
    DML_ITEM item ;
    T2_EDGE edge ;
    BOOLEAN status = TRUE ;
#ifdef CCDK_DEBUG
    INT i = 0 ;
#endif
    DML_WALK_LIST ( edgelist, item ) {
        edge = (T2_EDGE)DML_RECORD(item) ;
#ifdef CCDK_DEBUG
    i++ ;
#endif
        if ( !t2b_offset_edge ( edge, dist, desc, offset_list ) ) {
            status = FALSE ;
            edge = dml_last_record ( offset_list ) ;
            if ( edge != NULL ) 
                T2_EDGE_VTX(edge) = T2_VTX_UNDEF ;
        }
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2b_offset_edge ( edge, dist, desc, offset_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
REAL dist ;
T2_OFFSET_DESC desc ;
DML_LIST offset_list ;
{
    DML_LIST curvelist = dml_create_list() ;
    DML_ITEM item ;
    INT l ;
    T2_EDGE edge1 ;
    C2_CURVE curve ;

    if ( T2_EDGE_REVERSED(edge) )
        dist = - dist ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_edge ( edge, 10 ) ;
        getch();
    }
    else
        DISPLAY-- ;
#endif
    l = t2b_offset_curve ( T2_EDGE_CURVE(edge), dist, desc, curvelist ) ;
    if ( l != 0 ) {
        DML_WALK_LIST ( curvelist, item ) {
            curve = (C2_CURVE)DML_RECORD(item) ;
            edge1 = t2d_create_edge ( NULL, curve, T2_EDGE_DIR(edge) ) ;
#ifdef CCDK_DEBUG
{
    REAL d, s;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_edge ( edge1, 11 ) ;
        getch();
    }
    else
        DISPLAY-- ;
}
#endif
            T2_EDGE_ATTR(edge1) = T2_EDGE_ATTR(edge1) | T2_EDGE_ATTR(edge) ;
            T2_EDGE_PARENT(edge1) = ( T2_EDGE_PARENT(edge) == NULL ) ? 
                edge : T2_EDGE_PARENT(edge) ;
            dml_append_data ( offset_list, edge1 ) ;
        }
    }
    if ( l == 1 ) 
        T2_EDGE_VTX(edge1) = T2_EDGE_VTX(edge) ;
    dml_free_list ( curvelist ) ;
    curvelist = NULL ;
    RETURN ( l == 1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT t2b_offset_curve ( curve, dist, desc, curvelist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL dist ;
T2_OFFSET_DESC desc ;
DML_LIST curvelist ;
{
    INT l ;

    l = c2d_offset ( curve, dist, curvelist ) ;

#if ( __BBS_MILL__ >= 2 || __BBS_TURN__ >= 2 ) 
    RETURN ( t2b_offset2_curve ( curve, desc, l, curvelist ) ) ;
#else
    desc = NULL ;
    RETURN ( l ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_offset_pass3 ( loop, dist, desc, looplist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL dist ;
T2_OFFSET_DESC desc ;
DML_LIST looplist ;
{
    DML_ITEM item1 ;
    T2_LOOP loop1 ;
    BOOLEAN done ;

    t2b_intersect_looplist ( looplist, TRUE, T2_LOOP_CLOSED(loop) ) ;

    DML_WALK_LIST ( looplist, item1 ) {
        done = FALSE ;
        loop1 = (T2_LOOP)DML_RECORD(item1) ;
        while ( !done ) 
/*
            done = t2b_verify_pass13 ( loop1 ) ;
*/
            done = t2b_offset_pass13 ( loop1 ) ;
    }

    DML_WALK_LIST ( looplist, item1 ) {
        loop1 = (T2_LOOP)DML_RECORD(item1) ;
        t2b_verify_pass31 ( loop, loop1, dist, desc ) ;
        t2b_offset_link_epts ( loop, dist, desc, loop1 ) ;
    }
    DML_WALK_LIST ( looplist, item1 ) {
        loop1 = (T2_LOOP)DML_RECORD(item1) ;
        t2b_delete_links ( loop1, FALSE ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2b_offset_pass13 ( loop ) 
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
T2B_O_PASS13++ ;
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


#ifdef OLD_CODE
/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_offset_pass13 ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    DML_ITEM item1, item2 ;
    T2_LINK link1, link2 ;
    BOOLEAN status ;
    INT delete ;
#ifdef CCDK_DEBUG
T2B_O_PASS13++ ;
#endif

    status = TRUE ;
    if ( T2_LOOP_LINK_LIST(loop) == NULL ) 
        RETURN ( status ) ;
    for ( item1 = DML_FIRST(T2_LOOP_LINK_LIST(loop)), 
        item2 = dml_next(item1) ; item2 != NULL && item1 != NULL ; 
        item1 = item2, item2 = DML_NEXT(item1) ) {
        link1 = DML_RECORD(item1) ;
        link2 = DML_RECORD(item2) ;

        if ( !( T2_LINK_STATUS(link1) & T2_LINK_DELETE ) &&
             !( T2_LINK_STATUS(link2) & T2_LINK_DELETE ) &&
             t2b_links_same_pos ( link1, link2 ) ) {
            delete = t2b_link_force ( link1, link2 ) ;
            if ( delete == 1 ) {
                status = FALSE ;
                t2b_mark_link_delete ( link1, /* TRUE */ FALSE ) ;
            }
            else if ( delete == 2 ) {
                status = FALSE ;
                t2b_mark_link_delete ( link2, /* TRUE */ FALSE ) ;
            }
        }
    }
/*
    if ( !status ) 
        t2b_delete_links ( loop, FALSE ) ;
*/
    RETURN ( status ) ;
}
#endif

#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

