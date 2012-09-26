 /* -S -Z -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2B4.C **********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <c2vmcrs.h>
#include <dmldefs.h>
#include <t2bdefs.h>
#include <t2bpriv.h>
#include <t2cdefs.h>
#include <t2cpriv.h>
#include <t2ddefs.h>
#include <t2dpriv.h>
#include <t2attrd.h>
#include <t2link.h>
#include <t2frgmm.h>
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC BOOLEAN t2b_valid_boolean1 __(( T2_LINK, T2_LINK )) ; 
STATIC BOOLEAN t2b_valid_boolean2 __(( T2_LINK, T2_LINK )) ; 
STATIC T2_LOOP t2b_loop_out __(( DML_LIST )) ;
STATIC DML_ITEM t2b_next_fragment __(( DML_LIST, PT2, REAL, REAL )) ;
STATIC void t2b_append_fragment __(( T2_FRAGMENT, DML_LIST )) ;
STATIC void t2b_fragment __(( T2_REGION DUMMY0 , PF_T2B_VALID DUMMY1 , 
            DML_LIST DUMMY2 )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2b_traverse ( region1, region2, outloop_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region1, region2 ;
DML_LIST outloop_list ;
{
    DML_LIST fragment_list ;

    fragment_list = dml_create_list () ;
    t2b_fragment ( region1, t2b_valid_boolean1, fragment_list ) ;
    t2b_fragment ( region2, t2b_valid_boolean2, fragment_list ) ;

    outloop_list = t2b_assemble ( fragment_list, outloop_list ) ;
    dml_free_list ( fragment_list ) ;
    RETURN ( outloop_list ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_fragment ( region, valid, fragment_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
PF_T2B_VALID valid ;
DML_LIST fragment_list ;
{
    DML_ITEM item ;

    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) 
        t2b_fragment_loop ( DML_RECORD(item), valid, fragment_list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_fragment_loop ( loop, valid, fragment_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PF_T2B_VALID valid ;
DML_LIST fragment_list ;
{
    DML_ITEM item0, item1 ;
    T2_LINK link0, link1 ;
    T2_FRAGMENT fragment ;

    if ( T2_LOOP_LINK_LIST(loop) == NULL ) 
        RETURN ;
    t2b_delete_links ( loop, FALSE ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop, 11 ) ;
    getch();

}
else
    DISPLAY-- ;
#endif

    DML_WALK_LIST ( T2_LOOP_LINK_LIST(loop), item0 ) {
        item1 = DML_NEXT(item0) ;
        if ( item1 == NULL ) 
            RETURN ;
        link0 = DML_RECORD(item0) ;
        link1 = DML_RECORD(item1) ;
#ifdef CCDK_DEBUG
        DISPLAY++ ;
        if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
            paint_chain ( loop, T2_LINK_EDGE(link0), T2_LINK_CPARM(link0), 
                T2_LINK_EDGE(link1), T2_LINK_CPARM(link1) ) ;
            paint_point ( T2_LINK_PT(link0), 0.01, 13 ) ;
            paint_point ( T2_LINK_PT(link1), 0.01, 14 ) ;
            getch();
        }
        else
            DISPLAY-- ;
#endif
        if ( (*valid) ( link0, link1 ) ) {
            fragment = t2b_create_fragment ( T2_LINK_PARM(link0), 
                T2_LINK_PARM(link1) ) ;
            dml_append_data ( fragment_list, fragment ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_valid_boolean1 ( link1, link2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LINK link1, link2 ;
{
    INT s1, s2 ;

    if ( T2_LINK_EDGE(link1) == T2_LINK_EDGE(link2) &&
        IS_ZERO ( T2_LINK_T(link1) - T2_LINK_T(link2) ) )
        RETURN ( FALSE ) ;

    s1 = T2_LINK_STATUS(link1) ;
    s2 = T2_LINK_STATUS(link2) ;

    if ( ( s1 & T2_LINK_NEXT_IN ) && ( s2 & T2_LINK_PREV_IN ) )
        RETURN ( TRUE ) ;
    else if ( ( s1 & T2_LINK_CODIR0 ) && ( s2 & T2_LINK_CODIR1 ) )
        RETURN ( TRUE ) ;
    else 
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_valid_boolean2 ( link1, link2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LINK link1, link2 ;
{
    INT s1, s2 ;

    if ( T2_LINK_EDGE(link1) == T2_LINK_EDGE(link2) &&
        IS_ZERO ( T2_LINK_T(link1) - T2_LINK_T(link2) ) )
        RETURN ( FALSE ) ;
    s1 = T2_LINK_STATUS(link1) ;
    s2 = T2_LINK_STATUS(link2) ;

    if ( ( s1 & T2_LINK_NEXT_IN ) && ( s2 & T2_LINK_PREV_IN ) )
        RETURN ( TRUE ) ;
    else 
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2b_assemble ( fragment_list, outloop_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST fragment_list ;
DML_LIST outloop_list ;
{
    T2_LOOP loop ;
#ifdef CCDK_DEBUG
    PT2 pt, tan ;
#endif
    if ( outloop_list == NULL ) 
        outloop_list = dml_create_list () ;
    while ( DML_LENGTH(fragment_list) ) {
        loop = t2b_loop_out ( fragment_list ) ;
        if ( loop == NULL ) 
            RETURN ( outloop_list ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop, 14 ) ;
    getch();
    paint_loop ( loop, 13 ) ;
}
else
    DISPLAY-- ;
#endif
        if ( T2_LOOP_CLOSED(loop) && IS_SMALL ( t2c_loop_area ( loop ) ) )
            t2d_free_loop ( loop ) ;
        else
            dml_append_data ( outloop_list, loop ) ;
    }
    RETURN ( outloop_list ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_LOOP t2b_loop_out ( fragment_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST fragment_list ;
{
    PT2 p0, p1 ;
    DML_LIST edgelist = dml_create_list () ;
    DML_ITEM item ;
    T2_FRAGMENT fragment ;
    REAL tol, tol1 ;
    T2_LOOP loop ;

    tol = t2c_get_gap_coeff () * BBS_TOL ;
    tol1 = 0.1 * BBS_TOL ;
    item = DML_FIRST(fragment_list) ;
    fragment = DML_RECORD(item) ;
    C2V_COPY ( T2_FRAGMENT_PT0(fragment), p0 ) ;

    while ( item != NULL ) {
        fragment = DML_RECORD(item) ;
        t2b_append_fragment ( fragment, edgelist ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edgelist ( edgelist, 12 ) ;
    getch();
    paint_edgelist ( edgelist, 11 ) ;
}
else
    DISPLAY-- ;
#endif
        C2V_COPY ( T2_FRAGMENT_PT1(fragment), p1 ) ;
        t2b_free_fragment ( fragment ) ;
        dml_remove_item ( fragment_list, item ) ;
        if ( DML_LENGTH(edgelist) && ( C2V_DIST ( p0, p1 ) <= tol ) ) {
            loop = t2d_create_loop ( NULL, edgelist ) ;
            T2_LOOP_CLOSED(loop) = TRUE ;
            RETURN ( loop ) ;
        }
        item = t2b_next_fragment ( fragment_list, p1, tol, tol1 ) ;
    }
    if ( dml_length(edgelist) == 0 ) {
        dml_free_list ( edgelist ) ;
        RETURN ( NULL ) ;
    }
    else
        RETURN ( t2d_create_loop ( NULL, edgelist ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_ITEM t2b_next_fragment ( fragment_list, pt, tol, tol1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST fragment_list ;
PT2 pt ;
REAL tol, tol1 ;
{
    DML_ITEM item, item0 ;
    REAL dist, dist0 ;
    T2_FRAGMENT fragment ;

    dist0 = 0.0 ;
    item0 = NULL ;

    DML_WALK_LIST ( fragment_list, item ) {
        fragment = DML_RECORD(item) ;
        dist = C2V_DIST ( pt, T2_FRAGMENT_PT0(fragment) ) ;
        if ( dist < tol1 ) 
            RETURN ( item ) ;
        if ( item0 == NULL || dist < dist0 ) {
            dist0 = dist ;
            item0 = item ;
        }
    }
    RETURN ( dist0 <= tol ? item0 : NULL ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_append_fragment ( fragment, edgelist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_FRAGMENT fragment ;
DML_LIST edgelist ;
{
    t2d_append_trim ( 
        T2_FRAGMENT_EDGE0(fragment), T2_FRAGMENT_CPARM0(fragment), 
        T2_FRAGMENT_EDGE1(fragment), T2_FRAGMENT_CPARM1(fragment), 
        T2_ATTR_INIT, edgelist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_FRAGMENT t2b_create_fragment ( parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_PARM parm0, parm1 ;
{
    T2_FRAGMENT fragment = dmm_malloc ( sizeof ( T2_FRAGMENT_S ) ) ;
    if ( fragment != NULL ) {
        T2_COPY_PARM ( parm0, T2_FRAGMENT_PARM0(fragment) ) ;
        T2_COPY_PARM ( parm1, T2_FRAGMENT_PARM1(fragment) ) ;
    }
    t2c_eval_pt ( T2_PARM_EDGE(parm0), T2_PARM_CPARM(parm0), 
        T2_FRAGMENT_PT0(fragment) ) ;
    t2c_eval_pt ( T2_PARM_EDGE(parm1), T2_PARM_CPARM(parm1), 
        T2_FRAGMENT_PT1(fragment) ) ;
    RETURN ( fragment ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_free_fragment ( fragment ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_FRAGMENT fragment ;
{
    dmm_free ( fragment ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_fragment_pt ( fragment, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_FRAGMENT fragment ;
PT2 pt ;
{
    PARM_S cparm ;
    T2_EDGE edge0, edge1, edge ;

    edge0 = T2_FRAGMENT_EDGE0(fragment) ;
    edge1 = T2_FRAGMENT_EDGE1(fragment) ;

    if ( edge0 == edge1 ) {
        PARM_T(&cparm) = 
            0.5 * ( T2_FRAGMENT_T0(fragment) + T2_FRAGMENT_T1(fragment) ) ;
        PARM_J(&cparm) = 
            ( T2_FRAGMENT_J0(fragment) + T2_FRAGMENT_J1(fragment) ) / 2 ;
        edge = edge0 ;
    }
    else if ( fabs ( T2_EDGE_T1(edge0) - T2_FRAGMENT_T0(fragment) ) >
        fabs ( T2_EDGE_T0(edge1) - T2_FRAGMENT_T1(fragment) ) ) {
        PARM_T(&cparm) = 
            0.5 * ( T2_FRAGMENT_T0(fragment) + T2_EDGE_T1(edge0) ) ;
        PARM_J(&cparm) = 
            ( T2_FRAGMENT_J0(fragment) + T2_EDGE_J1(edge0) ) / 2 ;
        edge = edge0 ;
    }
    else {
        PARM_T(&cparm) = 
            0.5 * ( T2_FRAGMENT_T1(fragment) + T2_EDGE_T0(edge1) ) ;
        PARM_J(&cparm) = 
            ( T2_FRAGMENT_J1(fragment) + T2_EDGE_J0(edge1) ) / 2 ;
        edge = edge1 ;
    }

    t2c_parm_adjust ( edge, &cparm ) ;
    t2c_eval_pt ( edge, &cparm, pt ) ;
#ifdef DDEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_chain ( T2_FRAGMENT_LOOP(fragment), 
            T2_FRAGMENT_EDGE0(fragment), T2_FRAGMENT_CPARM0(fragment), 
            T2_FRAGMENT_EDGE1(fragment), T2_FRAGMENT_CPARM1(fragment) ) ;
        paint_point ( pt, 0.02, 5 ) ;
        getch();
    }
    else
        DISPLAY-- ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2b_fragment_test ( fragment, looplist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_FRAGMENT fragment ;
DML_LIST looplist ;
{
    DML_ITEM item ;
    PT2 pt ;
    T2_PT_POSITION pt_pos ;
    T2_LOOP loop ;

    t2b_fragment_pt ( fragment, pt ) ;

    DML_WALK_LIST ( looplist, item ) {
        loop = (T2_LOOP)DML_RECORD(item) ;
#ifdef DDEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_loop ( DML_RECORD(item), 12 ) ;
        getch () ;
    }
    else
        DISPLAY-- ;
#endif
        if ( ( loop != T2_FRAGMENT_LOOP(fragment) ) && 
            ( t2c_orientation ( loop ) == T2_DIR_CW ) ) {
            pt_pos = t2c_pt_pos_loop ( loop, pt ) ;
            if ( pt_pos == T2_PT_OUTSIDE ) {
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_chain ( T2_FRAGMENT_LOOP(fragment), 
            T2_FRAGMENT_EDGE0(fragment), T2_FRAGMENT_CPARM0(fragment), 
            T2_FRAGMENT_EDGE1(fragment), T2_FRAGMENT_CPARM1(fragment) ) ;
        paint_loop ( loop, 4 ) ;
        paint_point ( pt, 0.02, 5 ) ;
        getch();
    }
    else
        DISPLAY-- ;
#endif
                RETURN ( FALSE ) ;
            }
        }
    }
    RETURN ( TRUE ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

