/* -S __BBS_MILL__=3 */
/********************************* T2BS.C **********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <dmldefs.h>
#include <t2bpriv.h>
#include <t2ipriv.h>
#include <t2cdefs.h>
#include <t2ddefs.h>
#include <c2vmcrs.h>
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
EXTERN BOOLEAN DIR ;
#endif

#if ( __BBS_MILL__>=3 )
STATIC BOOLEAN t2b_lorl_valid __(( T2_LINK, T2_LINK )) ;
STATIC BOOLEAN t2b_lil_valid __(( T2_LINK, T2_LINK )) ;
STATIC BOOLEAN t2b_connect_open_loops __(( DML_LIST )) ;
STATIC void t2b_lorl_link_status __(( DML_ITEM, DML_LIST )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2b_loop_outside_region_list ( loop, region_list, 
            result )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
DML_LIST region_list, result ;
{
    DML_LIST intlist, fragment_list ;
    DML_ITEM item ;
    T2_EDGE edge ;
    BOOLEAN done ;
    PT2 pt ;
    T2_PT_POSITION pt_pos ;

#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_loop ( loop, 9 ) ;
        getch ();
        DML_WALK_LIST ( region_list, item ) {
            paint_region ( DML_RECORD(item), 11 ) ;
            getch ();
        }
    }
    else
        DISPLAY-- ;
#endif
    t2i_clear_links_loop ( loop ) ;
    edge = t2c_first_edge ( loop ) ;
    t2c_ept0 ( edge, pt ) ;
    pt_pos = t2c_pt_pos_region_union ( region_list, pt ) ;
    if ( pt_pos == T2_PT_OUTSIDE )
        t2b_create_link ( edge, T2_EDGE_PARM0(edge), NULL, NULL, 
            T2_LINK_NEXT_OUT, NULL ) ;
    else
        t2b_create_link ( edge, T2_EDGE_PARM0(edge), NULL, NULL, 
            T2_LINK_NEXT_IN, NULL ) ;
    edge = t2c_last_edge ( loop ) ;
    if ( !T2_LOOP_CLOSED(loop) ) {
        t2c_ept1 ( edge, pt ) ;
        pt_pos = t2c_pt_pos_region_union ( region_list, pt ) ;
    }
    if ( pt_pos == T2_PT_OUTSIDE )
        t2b_create_link ( edge, T2_EDGE_PARM1(edge), NULL, NULL, 
            T2_LINK_PREV_OUT, NULL ) ;
    else
        t2b_create_link ( edge, T2_EDGE_PARM1(edge), NULL, NULL, 
            T2_LINK_PREV_IN, NULL ) ;

    intlist = dml_create_list () ;

    DML_WALK_LIST ( region_list, item ) 
        t2b_intersect_loop_region ( loop, DML_RECORD(item), intlist ) ;
    dml_free_list ( intlist ) ;

    done = FALSE ;
    while ( !done )
        done = t2b_verify_pass13 ( loop ) ;

    done = FALSE ;
    while ( !done )
        done = t2b_verify_pass14 ( loop ) ;

    DML_WALK_LIST ( T2_LOOP_LINK_LIST(loop), item ) 
        t2b_lorl_link_status ( item, region_list ) ;

    fragment_list = dml_create_list () ;
    t2b_fragment_loop ( loop, t2b_lorl_valid, fragment_list ) ;
    result = t2b_assemble ( fragment_list, result ) ;
    done = FALSE ;
    while ( !done ) 
        done = t2b_connect_open_loops ( result ) ;
    dml_free_list ( fragment_list ) ;
    RETURN ( result ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_connect_open_loops ( looplist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
{
    DML_ITEM item0, item1 ;
    T2_LOOP loop0, loop1 ;
    PT2 p0, p1, q0, q1 ;

    DML_WALK_LIST ( looplist, item0 ) {
        loop0 = (T2_LOOP)DML_RECORD(item0) ;
        t2c_loop_ept0 ( loop0, p0 ) ;
        t2c_loop_ept1 ( loop0, p1 ) ;

        DML_FOR_LOOP ( DML_NEXT(item0), item1 ) {
            loop1 = (T2_LOOP)DML_RECORD(item1) ;
            t2c_loop_ept0 ( loop1, q0 ) ;
            t2c_loop_ept1 ( loop1, q1 ) ;

            if ( C2V_IDENT_PTS ( p0, q1 ) ) {
                t2d_append_loop ( loop1, loop0 ) ;
                t2d_free_loop ( loop0 ) ;
                dml_remove_item ( looplist, item0 ) ;
                RETURN ( FALSE ) ;
            }
            else if ( C2V_IDENT_PTS ( p1, q0 ) ) {
                t2d_append_loop ( loop0, loop1 ) ;
                t2d_free_loop ( loop1 ) ;
                dml_remove_item ( looplist, item1 ) ;
                RETURN ( FALSE ) ;
            }
        }
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_lorl_valid ( link1, link2 ) 
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
    else 
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_lorl_link_status ( link_item, region_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM link_item ;
DML_LIST region_list ;
{
    T2_LINK link ;
    T2_PT_POSITION prev_pos, next_pos ;
    T2_LOOP loop ;
    DML_LIST link_list ;
    PT2 pt ;

    link = DML_RECORD(link_item) ;
    if ( T2_LINK_STATUS(link) & T2_LINK_DELETE ) 
        RETURN ;
    loop = T2_LINK_LOOP(link) ;
    link_list = T2_LOOP_LINK_LIST(loop) ;

    if ( DML_LENGTH(link_list) == 1 ) {
        t2b_mark_link_delete ( link, TRUE ) ;
        RETURN ;
    }

    if ( T2_LINK_STATUS(link) & T2_LINK_PREV_IN ) 
        prev_pos = T2_PT_INSIDE ;
    else if ( T2_LINK_STATUS(link) & T2_LINK_PREV_OUT ) 
        prev_pos = T2_PT_OUTSIDE ;
    else {
        t2b_prev_pt ( link_item, pt ) ;
        prev_pos = t2c_pt_pos_region_union ( region_list, pt ) ;
    }
    if ( prev_pos == T2_PT_INSIDE ) {
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) & ~T2_LINK_UNDEF ;
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) | T2_LINK_PREV_IN ;
    }
    else if ( prev_pos == T2_PT_OUTSIDE ) {
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) & ~T2_LINK_UNDEF ;
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) | T2_LINK_PREV_OUT ;
    }

    if ( T2_LINK_STATUS(link) & T2_LINK_NEXT_IN ) 
        next_pos = T2_PT_INSIDE ;
    else if ( T2_LINK_STATUS(link) & T2_LINK_NEXT_OUT ) 
        next_pos = T2_PT_OUTSIDE ;
    else {
        t2b_next_pt ( link_item, pt ) ;
        next_pos = t2c_pt_pos_region_union ( region_list, pt ) ;
    }
    if ( next_pos == T2_PT_INSIDE ) {
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) & ~T2_LINK_UNDEF ;
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) | T2_LINK_NEXT_IN ;
    }
    else if ( next_pos == T2_PT_OUTSIDE ) {
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) & ~T2_LINK_UNDEF ;
        T2_LINK_STATUS(link) = T2_LINK_STATUS(link) | T2_LINK_NEXT_OUT ;
    }

    if ( ( prev_pos == T2_PT_INSIDE ) && ( next_pos == T2_PT_INSIDE ) )
        t2b_mark_link_delete ( link, TRUE ) ;
    else if ( ( prev_pos == T2_PT_OUTSIDE ) && ( next_pos == T2_PT_OUTSIDE ) )
        t2b_mark_link_delete ( link, TRUE ) ;
    else if ( ( prev_pos == T2_PT_ON_BOUNDARY ) && 
        ( next_pos == T2_PT_ON_BOUNDARY ) )
        t2b_mark_link_delete ( link, TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2b_loop_inside_loop ( loop0, loop1, result )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0, loop1 ;
DML_LIST result ;
{
    DML_LIST intlist, fragment_list ;
    DML_ITEM item ;
    T2_EDGE edge ;
    BOOLEAN done ;
    PT2 pt ;
    T2_PT_POSITION pt_pos ;

    t2i_clear_links_loop ( loop0 ) ;
    edge = t2c_first_edge ( loop0 ) ;
    t2c_ept0 ( edge, pt ) ;
    pt_pos = t2c_pt_pos_loop ( loop1, pt ) ;
    if ( pt_pos == T2_PT_OUTSIDE )
        t2b_create_link ( edge, T2_EDGE_PARM0(edge), NULL, NULL, 
            T2_LINK_NEXT_OUT, NULL ) ;
    else
        t2b_create_link ( edge, T2_EDGE_PARM0(edge), NULL, NULL, 
            T2_LINK_NEXT_IN, NULL ) ;
    edge = t2c_last_edge ( loop0 ) ;
    if ( !T2_LOOP_CLOSED(loop0) ) {
        t2c_ept1 ( edge, pt ) ;
        pt_pos = t2c_pt_pos_loop ( loop1, pt ) ;
    }
    if ( pt_pos == T2_PT_OUTSIDE )
        t2b_create_link ( edge, T2_EDGE_PARM1(edge), NULL, NULL, 
            T2_LINK_PREV_OUT, NULL ) ;
    else
        t2b_create_link ( edge, T2_EDGE_PARM1(edge), NULL, NULL, 
            T2_LINK_PREV_IN, NULL ) ;

    intlist = dml_create_list () ;
    t2b_intersect_loop ( loop0, loop1, 0, FALSE, intlist ) ;
    dml_free_list ( intlist ) ;

    done = FALSE ;
    while ( !done )
        done = t2b_verify_pass13 ( loop0 ) ;

    done = FALSE ;
    while ( !done )
        done = t2b_verify_pass14 ( loop0 ) ;

    DML_WALK_LIST ( T2_LOOP_LINK_LIST(loop0), item ) 
        t2b_link_status ( item, FALSE ) ;

    fragment_list = dml_create_list () ;
    t2b_fragment_loop ( loop0, t2b_lil_valid, fragment_list ) ;
    result = t2b_assemble ( fragment_list, result ) ;
    dml_free_list ( fragment_list ) ;
    RETURN ( result ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_lil_valid ( link1, link2 ) 
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
#endif /* ( __BBS_MILL__>=3 ) */

