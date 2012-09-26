/* -R -Z -S -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/*********************************** T2I.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <dmldefs.h>
#include <c2cdefs.h>
#include <c2ddefs.h>
#include <c2vdefs.h>
#include <t2cdefs.h>
#include <t2cpriv.h>
#include <t2idefs.h>
#include <t2ipriv.h>
#include <t2link.h>
#include <c2mem.h>
#include <c2vmcrs.h>
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
EXTERN REAL OFFS_COEFF ;
#endif
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC DML_ITEM t2i_insert_link __(( T2_LOOP DUMMY0 , T2_LINK DUMMY1 , 
            BOOLEAN DUMMY2 , INT DUMMY3 )) ;
STATIC BOOLEAN t2i_last_edge_open_loop __(( T2_EDGE DUMMY0 )) ;
STATIC INT t2i_exclude_adjacent __(( DML_LIST DUMMY0 , INT DUMMY1 )) ;

/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT t2i_intersect_edge ( edge1, edge2, check_end1, 
            check_end2, intlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge1, edge2 ;
BOOLEAN check_end1, check_end2 ;
DML_LIST intlist ;
{
    DML_LIST list = dml_create_list() ;
    DML_ITEM item ;
    INT n = c2c_intersect ( T2_EDGE_CURVE(edge1), T2_EDGE_CURVE(edge2), list ) ;
    C2_INT_REC ci ;
    T2_INT_REC ti ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( edge1, 10 ) ;
    paint_edge ( edge2, 11 ) ;
    getch();
}
else
    DISPLAY-- ;
#endif
    DML_WALK_LIST ( list, item )
    {
        ci = ( C2_INT_REC ) dml_record ( item ) ;
        if ( check_end1 && ( C2_INT_REC_TYPE(ci) != -2 ) && 
            IS_ZERO ( T2_EDGE_T1(edge1) - C2_INT_REC_T1(ci) ) &&
            !t2i_last_edge_open_loop ( edge1 ) ) {
            c2d_free_int_rec ( ci ) ;
            n-- ;
        }
        else if ( check_end2 && ( C2_INT_REC_TYPE(ci) != -2 ) && 
            IS_ZERO ( T2_EDGE_T1(edge2) - C2_INT_REC_T2(ci) ) ) {
            c2d_free_int_rec ( ci ) ;
            n-- ;
        }
        else {
            ti = t2i_create_int_rec ( edge1, edge2, ci ) ;
            dml_append_data ( intlist, ( ANY ) ti ) ;
        }
    }
    dml_free_list ( list ) ;
    list = NULL ;
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2i_last_edge_open_loop ( edge )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
{
    T2_LOOP loop ;

    if ( edge == NULL ) 
        RETURN ( FALSE ) ;
    loop = T2_EDGE_LOOP(edge) ;
    if ( loop == NULL || T2_LOOP_CLOSED(loop) || 
        T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ( FALSE ) ;
    RETURN ( edge == DML_LAST_RECORD ( T2_LOOP_EDGE_LIST(loop) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_INT_REC t2i_create_int_rec ( edge1, edge2, ci )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge1, edge2 ;
C2_INT_REC ci ;
{
    T2_INT_REC ti = T2_ALLOC_INT_REC ;
    T2_INT_REC_EDGE1(ti) = edge1 ;
    T2_INT_REC_EDGE2(ti) = edge2 ;
    T2_INT_REC_CI(ti) = ci ;
    RETURN ( ti ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_INT_REC t2i_flip_int_rec ( ti )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_INT_REC ti ;
{
    T2_EDGE edge ;
    REAL t ;
    INT j ;

    edge = T2_INT_REC_EDGE1(ti) ;
    T2_INT_REC_EDGE1(ti) = T2_INT_REC_EDGE2(ti) ;
    T2_INT_REC_EDGE2(ti) = edge ;
    t = T2_INT_REC_T1(ti) ;
    T2_INT_REC_T1(ti) = T2_INT_REC_T2(ti) ;
    T2_INT_REC_T2(ti) = t ;
    j = T2_INT_REC_J1(ti) ;
    T2_INT_REC_J1(ti) = T2_INT_REC_J2(ti) ;
    T2_INT_REC_J2(ti) = j ;
    RETURN ( ti ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2i_free_int_rec ( ti )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_INT_REC ti ;
{
    C2_FREE_INT_REC(T2_INT_REC_CI(ti)) ;
    T2_INT_REC_CI(ti) = NULL ;
    T2_FREE_INT_REC(ti) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT t2i_intersect_loop ( loop1, loop2, intlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop1, loop2 ;
DML_LIST intlist ;
{
    DML_ITEM item1, item2 ;
    T2_EDGE edge1, edge2 ;
    INT m, n = 0 ;
#ifdef CCDK_DEBUG
    INT i1, i2 ;

    i1 = 0 ;
#endif
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop1), item1 )
    {
        edge1 = ( T2_EDGE ) dml_record ( item1 ) ;
#ifdef CCDK_DEBUG
        i1++ ;
        i2 = 0 ;
#endif

        DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop2), item2 )
        {
            edge2 = ( T2_EDGE ) dml_record ( item2 ) ;
#ifdef CCDK_DEBUG
            i2++ ;
#endif
            m = t2i_intersect_edge ( edge1, edge2, TRUE, TRUE, intlist ) ;
            n += m ;
        }
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT t2i_intersect_loop_self ( loop, intlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
DML_LIST intlist ;
{
    DML_ITEM item1, item2 ;
    T2_EDGE edge1, edge2 ;
    INT m, n = 0 ;
#ifdef CCDK_DEBUG
    INT i1, i2 ;
#endif

#ifdef CCDK_DEBUG
    i1 = 0 ;
#endif

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item1 )
    {
        edge1 = dml_record ( item1 ) ;
#ifdef CCDK_DEBUG
{
PT2 tan0, tan1 ;
REAL angle ;
t2c_etan1 ( edge1, tan0 ) ;
edge2 = dml_record ( dml_next ( item1 ) ) ;
if ( edge2 != NULL ) {
    t2c_etan0 ( edge2, tan1 ) ;
    angle = c2v_vecs_angle ( tan0, tan1 ) ;
    }
}
        i1++ ;
        i2 = i1 ;
#endif
        DML_FOR_LOOP ( dml_next ( item1 ), item2 ) {
            edge2 = dml_record ( item2 ) ;
#ifdef CCDK_DEBUG
            i2++ ;
#endif
            m = t2i_intersect_edge ( edge1, edge2, TRUE, TRUE, intlist ) ;
            if ( item2 == DML_NEXT(item1) ) 
                m = t2i_exclude_adjacent ( intlist, m ) ;
            n += m ;
        }
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT t2i_exclude_adjacent ( intlist, m ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST intlist ;
INT m ;
{
    DML_ITEM item, item1 ; 
    INT n, i ;
    T2_INT_REC ti ;
    T2_EDGE edge1, edge2 ;
    REAL zero = 10.0 * BBS_ZERO ;

    n = m ; 
    if ( m <= 0 ) 
        RETURN ( n ) ;
    for ( item = DML_LAST(intlist), i = 0 ; 
        item != NULL && i < m ; item = item1, i++ ) {

        item1 = DML_PREV(item) ; 
        ti = DML_RECORD(item) ;
        edge1 = T2_INT_REC_EDGE1(ti) ;
        edge2 = T2_INT_REC_EDGE2(ti) ;
        if ( fabs ( T2_INT_REC_T1(ti) - T2_EDGE_T1(edge1) ) <= zero &&
             fabs ( T2_INT_REC_T2(ti) - T2_EDGE_T0(edge2) ) <= zero ) {
            t2i_free_int_rec ( ti ) ;
            DML_RECORD(item) = NULL ;
            dml_remove_item ( intlist, item ) ;
            n-- ;
        }
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2i_clear_links_region ( region ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
{
    if ( region != NULL ) 
        dml_apply ( T2_REGION_LOOP_LIST(region), 
        			( PF_ACTION ) t2i_clear_links_loop ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2i_clear_links_loop ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    if ( loop != NULL && T2_LOOP_LINK_LIST(loop) != NULL ) {
        dml_destroy_list ( T2_LOOP_LINK_LIST(loop), 
        				   ( PF_ACTION ) t2i_free_link ) ; 
        T2_LOOP_LINK_LIST(loop) = NULL ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_LINK t2i_create_link ( edge1, cparm1, s1, pt1, 
            edge2, cparm2, s2, pt2, dir, loop_epts )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge1 ;
PARM cparm1 ;
INT s1 ;
PT2 pt1 ;
T2_EDGE edge2 ;
PARM cparm2 ;
INT s2 ;
PT2 pt2 ;
BOOLEAN dir, loop_epts ;
{
    T2_LINK link1 = T2_ALLOC_LINK, link2 = T2_ALLOC_LINK ;
    T2_LINK_EDGE(link1) = edge1 ;
    COPY_PARM(cparm1,T2_LINK_CPARM(link1)) ;
    T2_LINK_STATUS(link1) = s1 ;
    T2_LINK_MATE(link1) = link2 ;
    T2_LINK_EDGE(link2) = edge2 ;
    COPY_PARM(cparm2,T2_LINK_CPARM(link2)) ;
    T2_LINK_STATUS(link2) = s2 ;
    T2_LINK_MATE(link2) = link1 ;
    if ( pt1 == NULL ) 
        t2c_eval_pt ( edge1, cparm1, T2_LINK_PT(link1) ) ;
    else {
        C2V_COPY ( pt1, T2_LINK_PT(link1) ) ;
    }
    if ( pt2 == NULL ) 
        t2c_eval_pt ( edge2, cparm2, T2_LINK_PT(link2) ) ;
    else {
        C2V_COPY ( pt2, T2_LINK_PT(link2) ) ;
    }
    t2i_insert_link ( T2_EDGE_LOOP(edge1), link1, dir, 
        ( loop_epts ? 1 : 0 ) ) ;
    t2i_insert_link ( T2_EDGE_LOOP(edge2), link2, dir, 
        ( loop_epts ? 2 : 0 ) ) ;
#ifdef GDEBUG
    {
    PT2 p ;
    t2c_eval_pt ( edge1, cparm1, p ) ;
    dsp_edge ( edge1 ) ;
    dsp_edge ( edge2 ) ;
    dsp_point ( p ) ;
    dsp_blank_edge ( edge1 ) ;
    dsp_blank_edge ( edge2 ) ;
    dsp_blank_point ( p ) ;
    }
#endif
    RETURN ( link1 ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2i_delete_link ( link )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LINK link ;
{
    DML_ITEM item ;
    DML_LIST list ;
    T2_LINK mate = T2_LINK_MATE(link) ;

    list = T2_LOOP_LINK_LIST(T2_LINK_LOOP(link)) ;
    item = dml_find_data ( list, link ) ;
    T2_FREE_LINK ( link ) ;
    link = NULL ;
    dml_remove_item ( list, item ) ;
    item = NULL ;
    if ( mate != NULL ) {
        list = T2_LOOP_LINK_LIST(T2_LINK_LOOP(mate)) ;
        item = dml_find_data ( list, mate ) ;
        T2_FREE_LINK ( mate ) ;
        mate = NULL ;
        dml_remove_item ( list, item ) ;
        item = NULL ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2i_free_link ( link )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LINK link ;
{
    T2_FREE_LINK ( link ) ;
}

/*-------------------------------------------------------------------------*/
STATIC DML_ITEM t2i_insert_link ( loop, link, dir, pos ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_LINK link ;
BOOLEAN dir ;
INT pos ;
{
    DML_ITEM item ;
    T2_LINK link1 ;
    INT cmp ;

    if ( T2_LOOP_LINK_LIST(loop) == NULL ) {
        T2_LOOP_LINK_LIST(loop) = dml_create_list() ;
        RETURN ( dml_append_data ( T2_LOOP_LINK_LIST(loop), link ) ) ;
    }

    if ( pos == 1 ) 
        RETURN ( dml_insert ( T2_LOOP_LINK_LIST(loop), link, TRUE ) ) ;
    if ( pos == 2 ) 
        RETURN ( dml_insert ( T2_LOOP_LINK_LIST(loop), link, FALSE ) ) ;

    DML_WALK_LIST ( T2_LOOP_LINK_LIST(loop), item ) {
        link1 = ( T2_LINK ) dml_record ( item ) ;
        cmp = t2c_compare_cparms ( T2_LINK_EDGE(link), T2_LINK_CPARM(link), 
            T2_LINK_EDGE(link1), T2_LINK_CPARM(link1) ) ;
        if ( cmp == -1 )
            RETURN ( dml_insert_prior ( T2_LOOP_LINK_LIST(loop), 
                item, link ) ) ;
        else if ( cmp == 0 ) {
            if ( dir ) {
                if ( ( T2_LINK_STATUS(link) & T2_LINK_OUT_IN ) && 
                    ( T2_LINK_STATUS(link1) & T2_LINK_IN_OUT ) ) 
                    RETURN ( dml_insert_prior ( T2_LOOP_LINK_LIST(loop), 
                        item, link ) ) ;
            }
            else { 
                if ( ( ( T2_LINK_STATUS(link) & T2_LINK_UNDEF ) && 
                    !( T2_LINK_STATUS(link1) & T2_LINK_UNDEF ) ) ||
                    ( ( T2_LINK_STATUS(link) & T2_LINK_OUT_IN ) && 
                    ( T2_LINK_STATUS(link1) & T2_LINK_IN_OUT ) ) ) 
                    RETURN ( dml_insert_prior ( T2_LOOP_LINK_LIST(loop), 
                        item, link ) ) ;
            }
        }
    }
    RETURN ( dml_append_data ( T2_LOOP_LINK_LIST(loop), link ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_LINK t2i_next_link ( link ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LINK link ;
{
    T2_LOOP loop = T2_LINK_LOOP(link) ;
    DML_ITEM item ;

    if ( link == DML_LAST_RECORD(T2_LOOP_LINK_LIST(loop)) )
        RETURN ( T2_LOOP_CLOSED(loop) ? 
        DML_FIRST_RECORD(T2_LOOP_LINK_LIST(loop)) : NULL ) ;
    item = dml_find_data ( T2_LOOP_LINK_LIST(loop), link ) ;
    RETURN ( DML_RECORD(DML_NEXT(item)) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_LINK t2i_prev_link ( link ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LINK link ;
{
    T2_LOOP loop = T2_LINK_LOOP(link) ;
    DML_ITEM item ;

    if ( link == DML_FIRST_RECORD(T2_LOOP_LINK_LIST(loop)) )
        RETURN ( T2_LOOP_CLOSED(loop) ? 
            DML_LAST_RECORD(T2_LOOP_LINK_LIST(loop)) : NULL ) ;
    item = dml_find_data ( T2_LOOP_LINK_LIST(loop), link ) ;
    RETURN ( DML_RECORD(DML_PREV(item)) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT t2i_intersect_ray_region ( ray, region, 
            tan_int, start_pt, intlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE ray ;
T2_REGION region ;
BOOLEAN tan_int, start_pt ;
DML_LIST intlist ;
{
    INT n = 0 ;
    DML_ITEM item ;
    T2_LOOP loop ;
    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item )
    {
        loop = ( T2_LOOP ) dml_record ( item ) ;
        n += t2i_intersect_ray_loop ( ray, loop, tan_int, start_pt, 
            TRUE, intlist ) ;
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT t2i_intersect_ray_loop ( ray, loop, 
            tan_int, start_pt, ext_ray, intlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE ray ;
T2_LOOP loop ;
BOOLEAN tan_int, start_pt, ext_ray ;
DML_LIST intlist ;
{
    RETURN ( t2i_intersect_ray_edgelist ( ray, T2_LOOP_EDGE_LIST(loop), 
        tan_int, start_pt, ext_ray, intlist ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT t2i_intersect_ray_edgelist ( ray, edgelist, tan_int, 
            start_pt, ext_ray, intlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE ray ;
DML_LIST edgelist ;
BOOLEAN tan_int, start_pt, ext_ray ;
DML_LIST intlist ;
{
    INT n = 0 ;
    DML_ITEM item ;
    T2_EDGE edge ;
    DML_WALK_LIST ( edgelist, item ) {
        edge = dml_record ( item ) ;
        n += t2i_intersect_ray_edge ( ray, edge, 
            tan_int, start_pt, ext_ray, intlist ) ;
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT t2i_intersect_ray_edge ( ray, edge, tan_int, 
            start_pt, ext_ray, intlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE ray ;
T2_EDGE edge ;
BOOLEAN tan_int, start_pt, ext_ray ;
DML_LIST intlist ;
{
    DML_LIST list = dml_create_list() ;
    DML_ITEM item ;
    INT n ;
    C2_INT_REC ci ;
    T2_INT_REC ti ;
    BOOLEAN b0, b1, b2, b ;

    n = ext_ray ? c2c_intersect_ext ( ray, T2_EDGE_CURVE(edge), list ) :
        c2c_intersect ( ray, T2_EDGE_CURVE(edge), list ) ;

    if ( n < 0 ) {
        dml_destroy_list ( list, ( PF_ACTION ) c2d_free_int_rec ) ;
/* 03-01-92 */
        n = 0 ;
        ci = C2_ALLOC_INT_REC ;
        if ( t2c_ept0 ( edge, C2_INT_REC_PT(ci) ) &&
             c2c_project ( ray, C2_INT_REC_PT(ci), 
                C2_INT_REC_PARM1(ci), NULL ) &&
            ( C2_INT_REC_T1(ci) >= BBS_ZERO || 
            ( start_pt && C2_INT_REC_T1(ci) >= -BBS_ZERO ) ) ) {
            PARM_COPY ( T2_EDGE_PARM0(edge), C2_INT_REC_PARM2(ci) ) ;
            C2_INT_REC_NEAR_TAN(ci) = FALSE ;
            C2_INT_REC_DIST(ci) = 0.0 ;
            C2_INT_REC_TYPE(ci) = -2 ;
            n++ ;
            ti = t2i_create_int_rec ( NULL, edge, ci ) ;
            dml_append_data ( intlist, ti ) ;
        }
        else {
            C2_FREE_INT_REC(ci) ;
            ci = NULL ;
        }

        ci = C2_ALLOC_INT_REC ;
        if ( t2c_ept1 ( edge, C2_INT_REC_PT(ci) ) &&
             c2c_project ( ray, C2_INT_REC_PT(ci), 
                C2_INT_REC_PARM1(ci), NULL ) &&
            ( C2_INT_REC_T1(ci) >= BBS_ZERO || 
            ( start_pt && C2_INT_REC_T1(ci) >= -BBS_ZERO ) ) ) {
            PARM_COPY ( T2_EDGE_PARM1(edge), C2_INT_REC_PARM2(ci) ) ;
/*
            C2_INT_REC_TAN_STATUS(ci) = FALSE ;
*/
            C2_INT_REC_DIST(ci) = 0.0 ;
            C2_INT_REC_TYPE(ci) = -2 ;
            n++ ;
            ti = t2i_create_int_rec ( NULL, edge, ci ) ;
            dml_append_data ( intlist, ti ) ;
        }
        else {
            C2_FREE_INT_REC(ci) ;
            ci = NULL ;
        }

        RETURN ( n ) ;
/* 03-01-92 */
    }
    DML_WALK_LIST ( list, item ) {
        ci = ( C2_INT_REC ) dml_record ( item ) ;
        b0 = ( T2_EDGE_DIR(edge) == 1 ) ?
            ( C2_INT_REC_T2(ci) >= T2_EDGE_T0(edge) - BBS_ZERO )
                && ( C2_INT_REC_T2(ci) <= T2_EDGE_T1(edge) - BBS_ZERO ) :
            ( C2_INT_REC_T2(ci) <= T2_EDGE_T0(edge) + BBS_ZERO )
                && ( C2_INT_REC_T2(ci) >= T2_EDGE_T1(edge) + BBS_ZERO ) ;
        b1 = ( C2_INT_REC_T1(ci) > ( start_pt ? -BBS_ZERO : BBS_ZERO ) ) &&
            ( tan_int || ( C2_INT_REC_TYPE(ci) == 1 ) ) ;
        b2 = ( C2_INT_REC_T1(ci) >= -BBS_ZERO ) && 
            ( C2_INT_REC_TYPE(ci) == -2 ) ;

        b = b1 || b2 ;
        if ( b0 && b ) {
            ti = t2i_create_int_rec ( NULL, edge, ci ) ;
            dml_append_data ( intlist, ti ) ;
        }
        else {
            n-- ;
            C2_FREE_INT_REC(ci) ;
            ci = NULL ;
        }
    }
    dml_free_list ( list ) ;
    list = NULL ;
    RETURN ( n ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

