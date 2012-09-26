/* -S -Z -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2BO.C **********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <dmldefs.h>
#include <c2ddefs.h>
#include <m2cdefs.h>
#include <t2bdefs.h>
#include <t2bpriv.h>
#include <t2cdefs.h>
#include <t2cpriv.h>
#include <t2ddefs.h>
#include <c2vdefs.h>
#include <c2vmcrs.h>
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
GLOBAL T2B_OFFSET_EXTLOOP = 0 ;
GLOBAL T2B_OFFSET_LOOP = 0 ;
GLOBAL T2B_OFFSET_REGION = 0 ;
#endif
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC DML_LIST t2b_open_regions __(( DML_LIST, DML_LIST )) ;
STATIC void     t2b_offset_join __(( DML_LIST, BOOLEAN, REAL, 
            T2_OFFSET_DESC, DML_LIST )) ;
STATIC BOOLEAN  t2b_join_loops __(( T2_LOOP, T2_LOOP, REAL, REAL, 
            T2_OFFSET_DESC )) ;
STATIC T2_LOOP t2b_check_offset __(( DML_LIST, T2_LOOP, REAL )) ;

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2b_offset_region ( region, dist, desc, result )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
REAL dist ;
T2_OFFSET_DESC desc ;
DML_LIST result ;
{
    DML_LIST looplist ;
    T2_LOOP loop ;
    REAL distl ;
    DML_ITEM item ;

#ifdef CCDK_DEBUG
T2B_OFFSET_REGION++ ;
#endif
    looplist = dml_create_list () ;
    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) {
        loop = DML_RECORD(item) ;
        t2b_od_lookup ( desc, loop, dist, &distl ) ; 
        looplist = t2b_offset_loop ( DML_RECORD(item), 
            distl, desc, looplist ) ;
    }
#ifdef CCDK_DEBUG
{
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        DML_WALK_LIST ( looplist, item ) {
            paint_loop ( DML_RECORD(item), 9 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
}
#endif
    RETURN ( t2b_offset_rgn_pass1 ( looplist, 
        T2_LOOP_CLOSED(T2_REGION_EXT_LOOP(region)), result ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2b_offset_extloop ( region, dist, desc, result )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
REAL dist ;
T2_OFFSET_DESC desc ;
DML_LIST result ;
{
    T2_LOOP x_loop, loop, new ;
    DML_LIST looplist ;
    DML_ITEM item ;

#ifdef CCDK_DEBUG
T2B_OFFSET_EXTLOOP++ ;
#endif
    looplist = dml_create_list() ;
    item = DML_FIRST ( T2_REGION_LOOP_LIST(region) ) ;
    x_loop = DML_RECORD(item) ;
    t2b_offset_loop ( x_loop, dist, desc, looplist ) ;

    DML_FOR_LOOP ( dml_next(item), item ) {
        loop = DML_RECORD(item) ;
        new = t2d_copy_loop ( loop, NULL, NULL ) ;
        dml_append_data ( looplist, new ) ;
    }

    RETURN ( t2b_offset_rgn_pass1 ( looplist, 
        T2_LOOP_CLOSED(T2_REGION_EXT_LOOP(region)), result ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2b_offset_rgn_pass1 ( looplist, closed_only, result )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
BOOLEAN closed_only ;
DML_LIST result ;
{
    DML_LIST templist ;

    templist = t2b_loops ( looplist, NULL ) ;
    if ( closed_only ) 
        t2b_remove_open_loops ( templist, NULL ) ;
#ifdef CCDK_DEBUG
{
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        DML_WALK_LIST ( templist, item ) {
            paint_loop ( DML_RECORD(item), 10 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
}
#endif
    dml_destroy_list ( looplist, ( PF_ACTION ) t2d_free_loop ) ;
    result = t2b_open_regions ( templist, result ) ;
    result = t2b_create_regions ( templist, result ) ;
#ifdef CCDK_DEBUG
{
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL && dml_length(result) == 0 ) {
        DML_WALK_LIST ( result, item ) {
            paint_region ( DML_RECORD(item), 11 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
}
#endif
    dml_free_list ( templist ) ;
    RETURN ( result ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2b_offset_loop ( loop, dist, desc, offset_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL dist ;
T2_OFFSET_DESC desc ;
DML_LIST offset_list ;
{
    DML_LIST loop0list, dist1_list, loopdist_list, looplist, edgelist ;
    T2_LOOP loop1, parent ;
    DML_ITEM last, item, item1 ;
    REAL dist_max ;
    DML_LIST dlist ;

#ifdef CCDK_DEBUG
T2B_OFFSET_LOOP++ ;
#endif
    dlist = ( desc == NULL ) ? NULL : T2_OFFSET_DESC_DLIST(desc) ;

    t2d_adjust_epts_loop ( loop, t2c_get_gap_coeff () * BBS_TOL ) ;
#ifdef CCDK_DEBUG
if ( DIS_LEVEL == -2 )
    t2c_write_loop ( "loop.dat", loop ) ;
#endif
    if ( offset_list == NULL ) 
        offset_list = dml_create_list() ;
    dist_max = t2b_od_dist_max ( dist, desc ) ;
    if ( IS_SMALL(dist_max) ) {
        dml_append_data ( offset_list, t2d_copy_loop ( loop, NULL, NULL ) ) ;
        RETURN ( offset_list ) ;
    }

    dist1_list = ( dlist == NULL ) ? NULL : dml_create_list () ;

    if ( dist_max < 0.0 ) 
        t2c_reverse_loop ( loop ) ;
    edgelist = t2b_offset_pass0 ( loop, dist, dlist, dist1_list ) ;
    if ( edgelist == NULL ) 
        RETURN ( NULL ) ;

    loopdist_list = ( dlist == NULL ) ? NULL : dml_create_list () ;
    loop0list = t2b_offset_pass1 ( loop, edgelist, dist, dist1_list, 
        loopdist_list ) ;
    dml_destroy_list ( dist1_list, ( PF_ACTION ) t2b_free_od ) ;
    dist1_list = NULL ;
    if ( loop0list == NULL ) 
        RETURN ( NULL ) ;

#ifdef CCDK_DEBUG
{
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        DML_WALK_LIST ( loop0list, item ) {
            paint_loop ( (T2_LOOP)DML_RECORD(item), 10 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
}
#endif
    looplist = t2b_offset_pass2 ( loop0list, fabs(dist), 
        desc, loopdist_list ) ;
    if ( loopdist_list != NULL && dist_max > 0.0 )
        t2b_offset_join ( looplist, T2_LOOP_CLOSED(loop), dist, 
            desc, loopdist_list ) ;
#ifdef CCDK_DEBUG
{
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        DML_WALK_LIST ( looplist, item ) {
            paint_loop ( (T2_LOOP)DML_RECORD(item), 14 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
}
#endif
    dml_free_list ( loop0list ) ;
    loop0list = NULL ;
    dml_destroy_list ( loopdist_list, ( PF_ACTION ) t2b_free_od ) ;
    loopdist_list = NULL ;
    t2b_offset_pass3 ( loop, fabs(dist), desc, looplist ) ;
    last = DML_LAST(offset_list) ;
    t2b_offset_pass4 ( looplist, offset_list ) ;
    dml_destroy_list ( looplist, ( PF_ACTION ) t2d_free_loop ) ;
    looplist = NULL ;
    last = ( last == NULL ) ? DML_FIRST(offset_list) : DML_NEXT(last) ;
    parent = ( T2_LOOP_PARENT(loop) == NULL ) ? loop : T2_LOOP_PARENT(loop) ;
    if ( dist_max < 0.0 ) 
        t2c_reverse_loop ( loop ) ;

    for ( item = last ; item != NULL ; item = item1 ) {
        item1 = DML_NEXT(item) ;
        loop1 = DML_RECORD(item) ;
#ifdef CCDK_DEBUG
{
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_loop ( loop1, 11 ) ;
        getch();
    }
    else
        DISPLAY-- ;
}
#endif
        if ( t2b_offset_verify ( loop, loop1, dist, desc ) ) {
            t2d_adjust_epts_loop ( loop1, t2c_get_gap_coeff () * BBS_TOL ) ;
            T2_LOOP_PARENT(loop1) = parent ;
            T2_LOOP_ATTR(loop1) = T2_LOOP_ATTR(loop1) | T2_LOOP_ATTR(loop) ;
            if ( dist_max < 0.0 ) 
                t2c_reverse_loop ( loop1 ) ;
        }                                                     
        else {
            t2d_free_loop ( loop1 ) ;
            dml_remove_item ( offset_list, item ) ;
        }
    }
#ifdef CCDK_DEBUG
{
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        DML_WALK_LIST ( offset_list, item ) {
            paint_loop ( (T2_LOOP)DML_RECORD(item), 11 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
    if ( DIS_LEVEL == -2 && T2_LOOP_CLOSED(loop) ) {
        DML_WALK_LIST ( offset_list, item ) {
            if ( !T2_LOOP_CLOSED((T2_LOOP)DML_RECORD(item)) ) {
                t2c_write_loop ( "loop0.dat", loop ) ;
                t2c_write_looplist ( "loops.dat", offset_list ) ;
            }
        }
    }
}
#endif
    RETURN ( offset_list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2b_offset_looplist ( looplist, dist, desc, offset_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
REAL dist ;
T2_OFFSET_DESC desc ;
DML_LIST offset_list ;
{
    DML_ITEM item ;
    T2_LOOP loop ;

    if ( looplist == NULL ) 
        RETURN ( offset_list ) ;
    if ( offset_list == NULL ) 
        offset_list = dml_create_list () ;

    DML_WALK_LIST ( looplist, item ) {
        loop = (T2_LOOP)DML_RECORD(item) ;
        if ( t2b_offset_loop ( loop, dist, desc, offset_list ) == NULL ) 
            RETURN ( NULL ) ;
    }
    RETURN ( offset_list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2b_offset_regionlist ( region_list, dist, desc, 
            offset_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST region_list ;
REAL dist ;
T2_OFFSET_DESC desc ;
DML_LIST offset_list ;
{
    DML_ITEM item ;
    T2_REGION region ;

    if ( region_list == NULL ) 
        RETURN ( offset_list ) ;
    if ( offset_list == NULL ) 
        offset_list = dml_create_list () ;

    DML_WALK_LIST ( region_list, item ) {
        region = (T2_REGION)DML_RECORD(item) ;
        if ( t2b_offset_region ( region, dist, desc, offset_list ) == NULL ) 
            RETURN ( NULL ) ;
    }
    RETURN ( offset_list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2b_offset_verify ( loop0, loop, dist, desc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0, loop ;
REAL dist ;
T2_OFFSET_DESC desc ;
{
    PT2 pt ;

    if ( T2_LOOP_CLOSED(loop0) && !T2_LOOP_CLOSED(loop) ) 
        RETURN ( FALSE ) ;
    t2c_loop_ept0 ( loop, pt ) ;
    RETURN ( t2b_offset_pt_pos ( loop0, dist, desc, pt ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_remove_open_loops ( looplist, item0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
DML_ITEM item0 ;
{
    DML_ITEM item, item1 ;
    T2_LOOP loop ;

    if ( item0 == NULL ) 
        item0 = DML_FIRST(looplist) ;
    for ( item = item0 ; item != NULL ; item = item1 ) {
        item1 = DML_NEXT(item) ;
        loop = (T2_LOOP)DML_RECORD(item) ;
        if ( !T2_LOOP_CLOSED(loop) ) {
            t2d_free_loop ( loop ) ;
            dml_remove_item ( looplist, item ) ;
        }
    }
}

/*-------------------------------------------------------------------------*/
STATIC DML_LIST t2b_open_regions ( looplist, regionlist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist, regionlist ;
{
    DML_ITEM item, item1 ;
    T2_LOOP loop ;

    for ( item = DML_FIRST(looplist) ; item != NULL ; item = item1 ) {
        item1 = DML_NEXT(item) ;
        loop = DML_RECORD(item) ;
        if ( !T2_LOOP_CLOSED(loop) ) {
            if ( regionlist == NULL ) 
                regionlist = dml_create_list () ;
            dml_append_data ( regionlist, t2d_loop_to_region ( loop ) ) ;
            dml_remove_item ( looplist, item ) ;
        }
    }
    RETURN ( regionlist ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_offset_join ( looplist, closed, dist, desc, dist_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
BOOLEAN closed ;
REAL dist ;
T2_OFFSET_DESC desc ;
DML_LIST dist_list ;
{
    DML_ITEM item1, item2 ;
    T2_LOOP loop1, loop2 ;
    PT2 p1, p2 ;
    REAL dist1, dist2 ;

    loop1 = (T2_LOOP)DML_FIRST_RECORD(looplist) ;
    t2b_od_lookup_list ( dist_list, loop1, dist, &dist1 ) ;

    DML_WALK_LIST ( looplist, item1 ) {
        loop1 = (T2_LOOP)DML_RECORD(item1) ;
        item2 = DML_NEXT(item1) ;
        if ( item2 != NULL ) {
            loop2 = DML_RECORD(item2) ;
            t2b_od_lookup_list ( dist_list, loop2, dist, &dist2 ) ;
            t2b_join_loops ( loop1, loop2, dist1, dist2, desc ) ;
            dist1 = dist2 ;
        }
    }

    if ( closed && DML_LENGTH(looplist) > 1 ) {
        loop1 = (T2_LOOP)DML_LAST_RECORD(looplist) ;
        loop2 = (T2_LOOP)DML_FIRST_RECORD(looplist) ;
        t2b_od_lookup_list ( dist_list, loop2, dist, &dist2 ) ;
        t2b_join_loops ( loop1, loop2, dist1, dist2, desc ) ;
    }

    for ( item1 = DML_FIRST(looplist) ; item1 != NULL ; item1 = item2 ) {
        item2 = DML_NEXT(item1) ;
        if ( item2 != NULL ) {
            loop1 = (T2_LOOP)DML_RECORD(item1) ;
            loop2 = (T2_LOOP)DML_RECORD(item2) ;
            t2c_loop_ept1 ( loop1, p1 ) ;
            t2c_loop_ept0 ( loop2, p2 ) ;
            if ( C2V_IDENT_PTS ( p1, p2 ) ) {
                t2d_append_loop ( loop1, loop2 ) ;
                t2d_free_loop ( loop2 ) ;
                dml_remove_item ( looplist, item2 ) ;
                item2 = item1 ;
            }
        }
    }
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_join_loops ( loop1, loop2, dist1, dist2, desc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop1, loop2 ;
REAL dist1, dist2 ;
T2_OFFSET_DESC desc ;
{
    REAL dist, u1, u2, cross, dot ;
    PT2 p1, p2, q1, q2, tan1, tan2, a[4] ;
    C2_CURVE line, arc ;
    T2_EDGE edge ;
    INT i, n ;

#if ( __BBS_MILL__ < 2 || __BBS_TURN__ < 2 ) 
    desc = NULL ;
#endif

    if ( IS_SMALL(dist1-dist2) )
        RETURN ( FALSE ) ;
    dist = dist1 < dist2 ? dist1 : dist2 ;
    t2c_loop_ept_tan1 ( loop1, p1, tan1 ) ;
    t2c_loop_ept_tan0 ( loop2, p2, tan2 ) ;
    if ( C2V_IDENT_PTS ( p1, p2 ) )
        RETURN ( FALSE ) ;

    c2v_normalize ( tan1, tan1 ) ;
    c2v_normalize ( tan2, tan2 ) ;
    cross = C2V_CROSS ( tan1, tan2 ) ; 
    dot = C2V_DOT ( tan1, tan2 ) ; 
    u1 = ( ( dist - dist1 ) * dot + ( dist2 - dist ) ) / cross ;
    u2 = - ( ( dist - dist1 ) + ( dist - dist2 ) * dot ) / cross ;

    if ( u1 < - BBS_ZERO || u2 < -BBS_ZERO )
        RETURN ( FALSE ) ;

    C2V_ADDT ( p1, tan1, u1, q1 ) ;
    C2V_ADDT ( p2, tan2, -u2, q2 ) ;
    if ( u1 > BBS_ZERO ) {
        line = c2d_line ( p1, q1 ) ;
        edge = t2d_create_edge ( loop1, line, 1 ) ;
        t2d_append_edge ( loop1, edge, FALSE ) ;
    }

    arc = ( fabs(dist1) < fabs(dist2) ) ? c2d_arc_2pts_tan ( q1, tan1, q2 ) :
        c2d_arc_2pts_tan1 ( q1, q2, tan2 ) ;
    if ( arc != NULL ) {
        if ( ( desc == NULL ) || ( T2_OFFSET_DESC_MODE(desc) == T2_FILLET ) ) {
            edge = t2d_create_edge ( loop1, arc, 1 ) ;
            t2d_append_edge ( loop1, edge, FALSE ) ;
        }

        else {
            n = ( T2_OFFSET_DESC_MODE(desc) == T2_SHARP ) ? 3 : 4 ;
            n = m2c_arc_polygon ( arc, n, a ) ;
            c2d_free_curve ( arc ) ;
            for ( i = 1 ; i < n ; i++ ) {
                line = c2d_line ( a[i-1], a[i] ) ;
                edge = t2d_create_edge ( loop1, line, 1 ) ;
                t2d_append_edge ( loop1, edge, FALSE ) ;
            }
        }
    }

    if ( u2 > BBS_ZERO ) {
        line = c2d_line ( q2, p2 ) ;
        edge = t2d_create_edge ( loop2, line, 1 ) ;
        t2d_append_edge ( loop2, edge, TRUE ) ;
    }
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC INT t2b_offset_loop_array ( loop, step, n, desc, offset ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL step ;
INT n ;
T2_OFFSET_DESC desc ;
T2_LOOP *offset ;
{
    INT i, m ;
    BOOLEAN end_loop ;
    DML_LIST offset_list ;

    offset[0] = t2d_copy_loop ( loop, NULL, NULL ) ;

    m = n ;
    for ( i=1, end_loop = FALSE ; i<n && !end_loop ; i++ ) {
        offset_list = t2b_offset_loop ( offset[i-1], step, desc, NULL ) ;
        if ( offset_list == NULL || DML_LENGTH(offset_list) == 0 ) {
            end_loop = TRUE ;
            m = i ;
        }
        else if ( DML_LENGTH(offset_list) > 1 ) {
            offset[i] = t2b_check_offset ( offset_list, offset[i-1], step ) ;
            if ( offset[i] == NULL ) {
                end_loop = TRUE ;
                m = -i ;
            }
        }
        else {
            offset[i] = DML_FIRST_RECORD(offset_list) ;
            DML_FIRST_RECORD(offset_list) = NULL ;
        }
        dml_destroy_list ( offset_list, ( PF_ACTION ) t2d_free_loop ) ;
    }

    RETURN ( m ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC INT t2b_offset_loop_lists ( loop, step, n, desc, offset_list ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL step ;
INT n ;
T2_OFFSET_DESC desc ;
DML_LIST *offset_list ;
{
    INT i ;

    if ( offset_list[0] == NULL ) 
        offset_list[0] = dml_create_list () ;
    dml_append_data ( offset_list[0], t2d_copy_loop ( loop, NULL, NULL ) ) ;

    for ( i=1 ; i<n ; i++ ) {
        offset_list[i] = t2b_offset_looplist ( offset_list[i-1], 
            step, desc, offset_list[i] ) ;
        if ( dml_length ( offset_list[i] ) == 0 ) 
            RETURN ( i ) ;
    }
    RETURN ( n ) ;
}


/*----------------------------------------------------------------------*/
STATIC T2_LOOP t2b_check_offset ( offset_list, loop, dist ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST offset_list ;
T2_LOOP loop ;
REAL dist ;
{
    PT2 p0, p1, q0, q1, tan0, tan1, v0, v1 ;
    DML_ITEM item ;
    T2_LOOP offset ;

    if ( dist < 0.0 ) 
        dist = - dist ;
    t2c_loop_ept_tan0 ( loop, p0, tan0 ) ;
    t2c_loop_ept_tan1 ( loop, p1, tan1 ) ;
    DML_WALK_LIST ( offset_list, item ) {
        offset = DML_RECORD(item) ;
        t2c_loop_ept0 ( offset, q0 ) ;
        t2c_loop_ept1 ( offset, q1 ) ;
        C2V_SUB ( q0, p0, v0 ) ;
        C2V_SUB ( q1, p1, v1 ) ;

        if ( IS_SMALL ( C2V_NORM ( v0 ) - dist ) && 
             IS_SMALL ( C2V_NORM ( v1 ) - dist ) &&
             IS_SMALL ( C2V_DOT ( v0, tan0 ) ) && 
             IS_SMALL ( C2V_DOT ( v1, tan1 ) ) ) {
            DML_RECORD(item) = NULL ;
            dml_remove_item ( offset_list, item ) ;
            RETURN ( offset ) ;
        }
    }
    RETURN ( NULL ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2b_inflate_edge ( edge, dist ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
REAL dist ;
{
    C2_CURVE curve1, curve2, arc0, arc1 ;
    DML_LIST edgelist ;

    m2c_inflate ( T2_EDGE_CURVE(edge), dist, 
        &curve1, &curve2, &arc0, &arc1 ) ;
    edgelist = dml_create_list () ;
    dml_append_data ( edgelist, t2d_create_edge ( NULL, curve1, 1 ) ) ;
    dml_append_data ( edgelist, t2d_create_edge ( NULL, arc1, 1 ) ) ;
    dml_append_data ( edgelist, t2d_create_edge ( NULL, curve2, 1 ) ) ;
    dml_append_data ( edgelist, t2d_create_edge ( NULL, arc0, 1 ) ) ;
    RETURN ( t2d_create_loop ( NULL, edgelist ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2b_inflate_line ( pt0, pt1, dist ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1 ;
REAL dist ;
{
    C2_CURVE line, curve1, curve2, arc0, arc1 ;
    DML_LIST edgelist ;

    line = c2d_line ( pt0, pt1 ) ;
    m2c_inflate ( line, dist, &curve1, &curve2, &arc0, &arc1 ) ;
    edgelist = dml_create_list () ;
    dml_append_data ( edgelist, t2d_create_edge ( NULL, curve1, 1 ) ) ;
    dml_append_data ( edgelist, t2d_create_edge ( NULL, arc1, 1 ) ) ;
    dml_append_data ( edgelist, t2d_create_edge ( NULL, curve2, 1 ) ) ;
    dml_append_data ( edgelist, t2d_create_edge ( NULL, arc0, 1 ) ) ;
    c2d_free_curve ( line ) ;
    RETURN ( t2d_create_loop ( NULL, edgelist ) ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

