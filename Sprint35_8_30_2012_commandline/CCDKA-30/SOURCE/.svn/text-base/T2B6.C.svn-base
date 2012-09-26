/* -S -Z -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2B6.C **********************************/
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <aladefs.h>
#include <dmldefs.h>
#include <t2bdefs.h>
#include <t2bpriv.h>
#include <t2cdefs.h>
#include <t2cpriv.h>
#include <t2ddefs.h>
#include <t2ipriv.h>
#include <t2link.h>
#include <t2attrd.h>
#include <c2vdefs.h>
#include <c2vmcrs.h>
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC void t2b_loop_verify __(( T2_LOOP )) ;
STATIC void t2b_loop_epts __(( T2_LOOP )) ;
STATIC BOOLEAN  t2b_valid_loops __(( T2_LINK, T2_LINK )) ;
STATIC void     t2b_offset_link_status __(( DML_ITEM, T2_LOOP, 
            REAL, T2_OFFSET_DESC )) ;
STATIC T2_PT_POSITION t2b_offset_prev_pos __(( DML_ITEM, T2_LOOP, REAL, 
            T2_OFFSET_DESC )) ;
STATIC T2_PT_POSITION t2b_offset_prev_pos1 __(( DML_ITEM, T2_LOOP, REAL, 
            T2_OFFSET_DESC )) ;
STATIC T2_PT_POSITION t2b_offset_next_pos __(( DML_ITEM, T2_LOOP, REAL, 
            T2_OFFSET_DESC )) ;
STATIC BOOLEAN  t2b_valid_offset __(( T2_LINK, T2_LINK )) ;
STATIC void     t2b_offset_link_ept0 __(( T2_LOOP, REAL, 
            T2_OFFSET_DESC, T2_LOOP )) ;
STATIC void     t2b_offset_link_ept1 __(( T2_LOOP, REAL, 
            T2_OFFSET_DESC, T2_LOOP )) ;
STATIC BOOLEAN t2b_offset_pt_dist __(( PT2, PT2, PT2, PT2, REAL, REAL )) ;
STATIC void     t2b_offset_assemble __(( DML_LIST, DML_ITEM, REAL, REAL )) ;
STATIC DML_LIST t2b_loops_traverse __(( DML_LIST DUMMY0 , DML_LIST DUMMY1 )) ;
STATIC void t2b_fragment_outside __(( DML_LIST DUMMY0 , DML_LIST DUMMY1 )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2b_loops ( inlist, outlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST inlist, outlist ;
{
    t2b_loops_split ( inlist ) ;
    outlist = t2b_loops_traverse ( inlist, outlist ) ;
    dml_apply ( inlist, ( PF_ACTION ) t2i_clear_links_loop ) ;
    RETURN ( outlist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_loops_split ( looplist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
{
    DML_ITEM item ;

    t2b_intersect_looplist ( looplist, FALSE, FALSE ) ;
    DML_WALK_LIST ( looplist, item ) {
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_loop ( DML_RECORD(item), 12 ) ;
        getch () ;
    }
    else
        DISPLAY-- ;
#endif
        t2b_loop_verify ( DML_RECORD(item) ) ;
    }
    DML_WALK_LIST ( looplist, item ) 
        t2b_loop_epts ( DML_RECORD(item) ) ;
    DML_WALK_LIST ( looplist, item ) 
        t2b_delete_links ( DML_RECORD(item), TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_loop_verify ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    BOOLEAN done ;

    done = FALSE ;
    while ( !done ) 
        done = t2b_verify_pass13 ( loop ) ;
    done = FALSE ;
    while ( !done ) 
        done = t2b_verify_pass14 ( loop ) ;

    t2b_verify_pass21 ( loop, NULL ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_loop_epts ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    T2_EDGE edge0, edge1 ;
    T2_LINK link0, link1 ;

    edge0 = t2c_first_edge ( loop ) ;
    edge1 = t2c_last_edge ( loop ) ;

    link0 = dml_first_record ( T2_LOOP_LINK_LIST(loop) ) ;
    link1 = dml_last_record ( T2_LOOP_LINK_LIST(loop) ) ;

    if ( ( link0 == NULL ) || ( T2_LINK_EDGE(link0) != edge0 ) ||
        !IS_ZERO ( T2_LINK_T(link0) - T2_EDGE_T0(edge0) ) ) 
        t2b_create_link ( edge0, T2_EDGE_PARM0(edge0), 
            NULL, NULL, T2_LINK_NEXT_IN, NULL ) ;
    else {
        t2b_link_decouple ( link0 ) ;
        T2_LINK_STATUS(link0) = T2_LINK_STATUS(link0) & ~T2_LINK_DELETE ;
        T2_LINK_STATUS(link0) = T2_LINK_STATUS(link0) | T2_LINK_NEXT_IN ;
    }

    if ( ( link1 == NULL ) || ( T2_LINK_EDGE(link1) != edge1 ) ||
        !IS_ZERO ( T2_LINK_T(link1) - T2_EDGE_T1(edge1) ) ) 
        t2b_create_link ( edge1, T2_EDGE_PARM1(edge1), 
            NULL, NULL, T2_LINK_PREV_IN, NULL ) ;
    else {
        t2b_link_decouple ( link1 ) ;
        T2_LINK_STATUS(link1) = T2_LINK_STATUS(link1) & ~T2_LINK_DELETE ;
        T2_LINK_STATUS(link1) = T2_LINK_STATUS(link1) | T2_LINK_PREV_IN ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_PT_POSITION t2b_pt_pos_loop ( loop, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt ;
{
    T2_PARM_S parm ;
    PT2 on_pt, tan, vec ;
    REAL cross ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_loop ( loop, 12 ) ;
        getch () ;
    }
    else
        DISPLAY-- ;
#endif

    if ( T2_LOOP_CLOSED(loop) ) 
        RETURN ( t2c_pt_pos_loop ( loop, pt ) ) ;

    else if ( t2c_project_loop ( loop, pt, TRUE, &parm, on_pt ) &&
        t2c_eval_tan ( T2_PARM_EDGE(&parm), T2_PARM_CPARM(&parm), tan ) ) {
        C2V_SUB ( pt, on_pt, vec ) ;
        if ( C2V_IS_SMALL(vec) )
            RETURN ( T2_PT_ON_BOUNDARY ) ;
        cross = C2V_CROSS ( vec, tan ) ;
        RETURN ( ( cross > 0.0 ) ? T2_PT_OUTSIDE : T2_PT_INSIDE ) ;
    }
#ifdef __BRLNDC__
	RETURN ( T2_PT_OUTSIDE ) ; /*This is merely to get rid of the warning */
#endif
#ifdef __WATCOM__
	RETURN ( T2_PT_OUTSIDE ) ; /*This is merely to get rid of the warning */
#endif
}


/*-------------------------------------------------------------------------*/
STATIC DML_LIST t2b_loops_traverse ( inlist, outlist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST inlist, outlist ;
{
    DML_ITEM item ;
    DML_LIST fragment_list ;

    fragment_list = dml_create_list () ;
    DML_WALK_LIST ( inlist, item ) 
        t2b_fragment_loop ( DML_RECORD(item), t2b_valid_loops, 
            fragment_list ) ;

    t2b_fragment_outside ( inlist, fragment_list ) ;

    outlist = t2b_assemble ( fragment_list, outlist ) ;
    dml_free_list ( fragment_list ) ;
    RETURN ( outlist ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_fragment_outside ( looplist, fragment_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist, fragment_list ;
{
    DML_ITEM item, item1 ;

    if ( fragment_list == NULL )
        RETURN ;

    for ( item = DML_FIRST(fragment_list) ; item != NULL ; item = item1 ) {
        item1 = DML_NEXT(item) ;
        if ( !t2b_fragment_test ( DML_RECORD(item), looplist ) ) {
            t2b_free_fragment ( DML_RECORD(item) ) ;
            dml_remove_item ( fragment_list, item ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_valid_loops ( link1, link2 ) 
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

    else if ( ( s1 & T2_LINK_ANTIDIR0 ) && ( s2 & T2_LINK_ANTIDIR1 ) )
        RETURN ( TRUE ) ;
    else if ( ( s1 & T2_LINK_ANTIDIR0 ) && ( s2 & T2_LINK_NEXT_IN ) )
        RETURN ( TRUE ) ;
    else if ( ( s1 & T2_LINK_PREV_IN ) && ( s2 & T2_LINK_ANTIDIR1 ) )
        RETURN ( TRUE ) ;

    else 
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_verify_pass31 ( loop0, loop, dist, desc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0, loop ;
REAL dist ;
T2_OFFSET_DESC desc ;
{
    DML_ITEM item ;

    if ( T2_LOOP_LINK_LIST(loop) == NULL ) 
        RETURN ;
    DML_WALK_LIST ( T2_LOOP_LINK_LIST(loop), item ) 
        t2b_offset_link_status ( item, loop0, dist, desc ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_offset_link_status ( link_item, loop0, dist, desc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM link_item ;
T2_LOOP loop0 ;
REAL dist ;
T2_OFFSET_DESC desc;
{
    T2_LINK link, mate ;
    T2_PT_POSITION prev_pos, next_pos ;
    T2_LOOP loop ;
    DML_LIST link_list ;

    link = DML_RECORD(link_item) ;
    if ( T2_LINK_STATUS(link) & T2_LINK_DELETE ) 
        RETURN ;
    loop = T2_LINK_LOOP(link) ;
    link_list = T2_LOOP_LINK_LIST(loop) ;

    if ( T2_LINK_STATUS(link) & T2_LINK_CODIR0 ) {
        mate = T2_LINK_MATE(link) ;
        T2_LINK_STATUS(mate) = T2_LINK_ANTIDIR0 ;
        RETURN ;
    }
    else if ( T2_LINK_STATUS(link) & T2_LINK_CODIR1 ) {
        mate = T2_LINK_MATE(link) ;
        T2_LINK_STATUS(mate) = T2_LINK_ANTIDIR1 ;
        RETURN ;
    }
    else if ( T2_LINK_STATUS(link) & T2_LINK_ANTIDIR0 ) {
        prev_pos = t2b_offset_prev_pos ( link_item, loop0, dist, desc ) ;
        RETURN ;
    }
    else if ( T2_LINK_STATUS(link) & T2_LINK_ANTIDIR1 ) {
        next_pos = t2b_offset_next_pos ( link_item, loop0, dist, desc ) ;
        RETURN ;
    }

    if ( T2_LOOP_CLOSED(loop) && DML_LENGTH(link_list) == 1 ) {
        t2b_mark_link_delete ( link, TRUE ) ;
        RETURN ;
    }
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_point ( T2_LINK_PT(link), 0.03, 9 ) ;
        getch () ;
    }
    else
        DISPLAY-- ;
#endif

    prev_pos = t2b_offset_prev_pos ( link_item, loop0, dist, desc ) ;
    next_pos = t2b_offset_next_pos ( link_item, loop0, dist, desc ) ;

    if ( ( prev_pos == T2_PT_INSIDE ) && ( next_pos == T2_PT_INSIDE ) )
        t2b_mark_link_delete ( link, TRUE ) ;
    else if ( ( prev_pos == T2_PT_OUTSIDE ) && ( next_pos == T2_PT_OUTSIDE ) )
        t2b_mark_link_delete ( link, TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_PT_POSITION t2b_offset_prev_pos ( link_item, loop0, dist, desc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM link_item ;
T2_LOOP loop0 ;
REAL dist ;
T2_OFFSET_DESC desc ;
{
    T2_LINK link ;
    T2_PT_POSITION prev_pos ;

    prev_pos = t2b_offset_prev_pos1 ( link_item, loop0, dist, desc ) ;
    link = DML_RECORD(link_item) ;
    if ( prev_pos == T2_PT_INSIDE ) {
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) & ~T2_LINK_UNDEF ;
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) | T2_LINK_PREV_IN ;
    }
    else if ( prev_pos == T2_PT_OUTSIDE ) {
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) & ~T2_LINK_UNDEF ;
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) | T2_LINK_PREV_OUT ;
    }
    RETURN ( prev_pos ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_PT_POSITION t2b_offset_prev_pos1 ( link_item, loop0, dist, desc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM link_item ;
T2_LOOP loop0 ;
REAL dist ;
T2_OFFSET_DESC desc ;
{
    PT2 p ;
    T2_LINK link ;
    T2_EDGE edge ;

    link = DML_RECORD(link_item) ;
    if ( T2_LINK_STATUS(link) & T2_LINK_PREV_IN ) 
        RETURN ( T2_PT_INSIDE ) ;
    else if ( T2_LINK_STATUS(link) & T2_LINK_PREV_OUT ) 
        RETURN ( T2_PT_OUTSIDE ) ;
    edge = T2_LINK_EDGE(link) ;
    if ( T2_EDGE_ATTR(edge) & T2_ATTR_FIRST ) 
        RETURN ( T2_PT_OUTSIDE ) ;
    else if ( IS_SMALL ( T2_EDGE_T0(edge) - T2_LINK_T(link) ) ) {
        edge = t2c_prev_edge ( edge ) ;
        if ( edge != NULL && ( T2_EDGE_ATTR(edge) & T2_ATTR_FIRST ) )
            RETURN ( T2_PT_OUTSIDE ) ;
    }
    else if ( IS_SMALL ( T2_EDGE_T1(edge) - T2_LINK_T(link) ) ) {
        edge = t2c_next_edge ( edge ) ;
        if ( edge != NULL && ( T2_EDGE_ATTR(edge) & T2_ATTR_FIRST ) )
            RETURN ( T2_PT_OUTSIDE ) ;
    }

    if ( t2b_prev_pt ( link_item, p ) )
        RETURN ( t2b_offset_pt_pos ( loop0, dist, desc, p ) ? 
            T2_PT_OUTSIDE : T2_PT_INSIDE ) ;
    else
        RETURN ( T2_PT_UNKNOWN ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_PT_POSITION t2b_offset_next_pos ( link_item, loop0, dist, desc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM link_item ;
T2_LOOP loop0 ;
REAL dist ;
T2_OFFSET_DESC desc ;
{
    PT2 p ;
    T2_LINK link ;
    T2_PT_POSITION next_pos ;

    link = DML_RECORD(link_item) ;
    if ( T2_LINK_STATUS(link) & T2_LINK_NEXT_IN ) 
        next_pos = T2_PT_INSIDE ;
    else if ( T2_LINK_STATUS(link) & T2_LINK_NEXT_OUT ) 
        next_pos = T2_PT_OUTSIDE ;
    else {
        if ( t2b_next_pt ( link_item, p ) )
            next_pos = t2b_offset_pt_pos ( loop0, dist, desc, p ) ? 
                T2_PT_OUTSIDE : T2_PT_INSIDE ;
        else
            next_pos = T2_PT_UNKNOWN ;
    }
    if ( next_pos == T2_PT_INSIDE ) {
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) & ~T2_LINK_UNDEF ;
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) | T2_LINK_NEXT_IN ;
    }
    else if ( next_pos == T2_PT_OUTSIDE ) {
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) & ~T2_LINK_UNDEF ;
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) | T2_LINK_NEXT_OUT ;
    }
    RETURN ( next_pos ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2b_offset_pt_pos ( loop, dist, desc, p0 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL dist ;
T2_OFFSET_DESC desc ;
PT2 p0 ;
{
    T2_PARM_S parm ;
    PT2 p1, p2, tan ;
    T2_EDGE edge ;
    REAL dist1, dist2 ;

    if ( !t2c_project_loop ( loop, p0, TRUE, &parm, NULL ) ) 
        RETURN ( TRUE ) ;
    edge = T2_PARM_EDGE(&parm) ;
    t2c_eval_pt_tan ( edge, T2_PARM_CPARM(&parm), p1, tan ) ;
    c2v_normalize ( tan, tan ) ;
    t2b_od_lookup ( desc, edge, dist, &dist1 ) ;
    C2V_OFFSET ( p1, tan, dist1, p2 ) ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_point ( p0, 0.02, 10 ) ;
        paint_point ( p1, 0.02, 11 ) ;
        paint_point ( p2, 0.02, 12 ) ;
        getch () ;
    }
    else
        DISPLAY-- ;
#endif
    if ( C2V_IDENT_PTS ( p0, p2 ) ) 
        RETURN ( TRUE ) ;
    if ( IS_SMALL ( T2_EDGE_T0(edge) - T2_PARM_T(&parm) ) ||
         IS_SMALL ( T2_EDGE_T1(edge) - T2_PARM_T(&parm) ) ) {
        dist2 = C2V_DIST ( p0, p1 ) ;
        RETURN ( IS_ZERO ( fabs(dist1) - dist2 ) ) ;
    }
    RETURN ( FALSE ) ;
}


#if 0

/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_offset_pt_dist ( p0, p1, tan0, tan1, dist0, dist1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 p0, p1, tan0, tan1 ;
REAL dist0, dist1 ;
{
    PT2 vec, offset0, offset1 ;
    REAL d, d_min, d_max ;

    C2V_SUB ( p0, p1, vec ) ;
    d = C2V_NORM ( vec ) ;
    ala_min_max ( fabs(dist0), fabs(dist1), &d_min, &d_max ) ;
    offset0[0] = tan0[1] ;
    offset0[1] = -tan0[0] ;
    offset1[0] = tan1[1] ;
    offset1[1] = -tan1[0] ;
    RETURN ( c2v_vec_between ( offset0, offset1, vec ) && 
        ( d_min <= d ) && ( d <= d_max ) ) ;
}

#endif // 0

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_offset_link_epts ( loop, dist, desc, loop1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL dist ;
T2_OFFSET_DESC desc ;
T2_LOOP loop1 ;
{
    t2b_offset_link_ept0 ( loop, dist, desc, loop1 ) ;
    t2b_offset_link_ept1 ( loop, dist, desc, loop1 ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_offset_link_ept0 ( loop, dist, desc, loop1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL dist ;
T2_OFFSET_DESC desc ;
T2_LOOP loop1 ;
{
    T2_EDGE edge ;
    PT2 pt ;
    T2_LINK link ;

    edge = t2c_first_edge_no_attr ( loop1, T2_ATTR_FIRST ) ;
    if ( edge == NULL ) 
        RETURN ;
    link = dml_first_record ( T2_LOOP_LINK_LIST(loop1) ) ;
    if ( ( link != NULL ) && ( T2_LINK_EDGE(link) == edge ) &&
        IS_ZERO ( T2_LINK_T(link) - T2_EDGE_T0(edge) ) )
        RETURN ;
    t2c_ept0 ( edge, pt ) ;
    if ( t2b_offset_pt_pos ( loop, dist, desc, pt ) )
        t2b_create_link ( edge, T2_EDGE_PARM0(edge), NULL, NULL, 
            T2_LINK_NEXT_OUT, pt ) ;
    else
        t2b_create_link ( edge, T2_EDGE_PARM0(edge), NULL, NULL, 
            T2_LINK_NEXT_IN, pt ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_offset_link_ept1 ( loop, dist, desc, loop1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL dist ;
T2_OFFSET_DESC desc ;
T2_LOOP loop1 ;
{
    T2_EDGE edge ;
    PT2 pt ;
    T2_LINK link ;

    edge = t2c_last_edge_no_attr ( loop1, T2_ATTR_FIRST ) ;
    if ( edge == NULL ) 
        RETURN ;
    link = dml_last_record ( T2_LOOP_LINK_LIST(loop1) ) ;
    if ( ( link != NULL ) && ( T2_LINK_EDGE(link) == edge ) &&
        IS_ZERO ( T2_LINK_T(link) - T2_EDGE_T1(edge) ) )
        RETURN ;
    t2c_ept1 ( edge, pt ) ;
    if ( t2b_offset_pt_pos ( loop, dist, desc, pt ) )
        t2b_create_link ( edge, T2_EDGE_PARM1(edge), NULL, NULL, 
            T2_LINK_PREV_OUT, pt ) ;
    else
        t2b_create_link ( edge, T2_EDGE_PARM1(edge), NULL, NULL, 
            T2_LINK_PREV_IN, pt ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2b_offset_pass4 ( inloop_list, outloop_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST inloop_list, outloop_list ;
{
    DML_LIST fragment_list ;
    DML_ITEM item, last ;
    REAL coeff ;

    fragment_list = dml_create_list () ;
    last = dml_last ( outloop_list ) ;
    DML_WALK_LIST ( inloop_list, item ) 
        t2b_fragment_loop ( DML_RECORD(item), t2b_valid_offset, 
            fragment_list ) ;
    outloop_list = t2b_assemble ( fragment_list, outloop_list ) ;
    coeff = 10.0 * t2c_get_gap_coeff () ;
    t2b_offset_assemble ( outloop_list, last, 
        coeff * BBS_TOL, coeff * BBS_TOL ) ;
    dml_free_list ( fragment_list ) ;

    RETURN ( outloop_list ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_offset_assemble ( looplist, item0, tol, ang_tol ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
DML_ITEM item0 ;
REAL tol, ang_tol ;
{
    DML_ITEM item, item1 ;
    T2_LOOP loop, loop1 ;
    PT2 p, tan, p1, tan1 ;
    REAL dist, cross ;

    if ( item0 == NULL ) 
        item0 = DML_FIRST(looplist) ;
    else
        item0 = DML_NEXT(item0) ;
    for ( item = item0 ; item != NULL ; item = item1 ) {
        item1 = DML_NEXT(item) ;
        if ( item1 == NULL )
            RETURN ;
        loop = DML_RECORD(item) ;
        loop1 = DML_RECORD(item1) ;
        if ( !T2_LOOP_CLOSED(loop) && !T2_LOOP_CLOSED(loop1) ) {
            t2c_loop_ept_tan1 ( loop, p, tan ) ;
            c2v_normalize ( tan, tan ) ;
            t2c_loop_ept_tan0 ( loop1, p1, tan1 ) ;
            c2v_normalize ( tan1, tan1 ) ;
            dist = C2V_DIST ( p, p1 ) ;
            cross = C2V_CROSS ( tan, tan1 ) ;
            if ( dist <= tol && fabs ( cross ) <= ang_tol ) {
                t2d_append_loop ( loop, loop1 ) ;
                t2d_free_loop ( loop1 ) ;
                dml_remove_item ( looplist, item1 ) ;
                item1 = item ;
            }
        }
    }
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_valid_offset ( link1, link2 ) 
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
    if ( ( s1 & T2_LINK_NEXT_OUT ) && ( s2 & T2_LINK_PREV_OUT ) )
        RETURN ( TRUE ) ;
    else if ( ( s1 & T2_LINK_NEXT_OUT ) && ( s2 & T2_LINK_CODIR0 ) )
        RETURN ( TRUE ) ;
    else if ( ( s1 & T2_LINK_CODIR1 ) && ( s2 & T2_LINK_PREV_OUT ) )
        RETURN ( TRUE ) ;
    else if ( ( s1 & T2_LINK_NEXT_OUT ) && ( s2 & T2_LINK_ANTIDIR0 ) )
        RETURN ( TRUE ) ;
    else if ( ( s1 & T2_LINK_ANTIDIR1 ) && ( s2 & T2_LINK_PREV_OUT ) )
        RETURN ( TRUE ) ;
    else if ( ( s1 & T2_LINK_CODIR0 ) && ( s2 & T2_LINK_CODIR1 ) )
        RETURN ( TRUE ) ;
/* 
    else if ( ( s1 & T2_LINK_ANTIDIR0 ) && ( s2 & T2_LINK_ANTIDIR1 ) )
        RETURN ( TRUE ) ;
*/
    else 
        RETURN ( FALSE ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

