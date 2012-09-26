/* -Z -S -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2DL.C **********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <c2adefs.h>
#include <c2cdefs.h>
#include <c2ddefs.h>
#include <c2vdefs.h>
#include <c2vmcrs.h>
#include <dmldefs.h>
#include <t2cdefs.h>
#include <t2ddefs.h>
#include <t2dpriv.h>
#include <t2ipriv.h>
#include <t2link.h>
#include <t2attrd.h>
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

typedef struct yx_struct {
    PT2 yx_pt0 ;
    PT2 yx_pt1 ;
    T2_EDGE yx_edge ;
    T2_LINK yx_link ;
    PARM_S yx_parm ; 
} T2_YX_S, *T2_YX ;
#define T2_YX_PT0(X)   ((X)->yx_pt0)
#define T2_YX_PT1(X)   ((X)->yx_pt1)
#define T2_YX_PARM(X) (&((X)->yx_parm))
#define T2_YX_T(X)    (PARM_T(T2_YX_PARM(X)))
#define T2_YX_EDGE(X) ((X)->yx_edge)
#define T2_YX_LINK(X) ((X)->yx_link)
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif

STATIC DML_LIST t2d_region_y_extrs __(( T2_REGION, ATTR, BOOLEAN )) ;
STATIC void t2d_loop_y_extrs __(( T2_LOOP, ATTR, BOOLEAN, DML_LIST )) ;
STATIC void t2d_edge_y_extrs __(( T2_EDGE, DML_LIST )) ;
STATIC void t2d_vtx_y_extr __(( T2_EDGE, T2_EDGE, BOOLEAN, DML_LIST )) ;
STATIC BOOLEAN t2d_vtx_y_extr_hor __(( T2_EDGE, T2_EDGE, PT2, PT2, PT2 )) ;
STATIC BOOLEAN t2d_cnvx_extr __(( T2_EDGE, PARM, INT, PT2 )) ;
STATIC void t2d_append_yx __(( T2_EDGE, PARM, PT2, DML_LIST )) ;
STATIC void t2d_break_pass1 __(( T2_REGION, DML_LIST )) ;
STATIC void t2d_break_pass2 __(( DML_LIST, DML_LIST )) ;
STATIC T2_LOOP t2d_break_loop __(( DML_LIST )) ;
STATIC T2_LINK t2d_break_traverse __(( T2_LINK, DML_LIST, 
            BOOLEAN, DML_LIST )) ;
STATIC T2_EDGE t2d_connect_links __(( T2_LINK )) ;
STATIC void    t2d_remove_link __(( DML_LIST, T2_LINK )) ;
STATIC BOOLEAN t2d_break_sort_key __(( T2_YX, T2_YX )) ;
STATIC void    t2d_free_yx __(( T2_YX )) ;
STATIC void    t2d_sort_loop_links __(( T2_LOOP )) ;
STATIC BOOLEAN t2d_links_order __(( T2_LINK )) ;
STATIC BOOLEAN t2d_yx_valid __(( T2_REGION, T2_YX, T2_YX, T2_PARM )) ;
STATIC void t2d_mark_edges __(( T2_LOOP )) ;
STATIC BOOLEAN t2d_prev_pt __(( T2_EDGE, PT2 )) ;
STATIC BOOLEAN t2d_next_pt __(( T2_EDGE, PT2 )) ;

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2d_break_region ( region, r_attr, l_attr, loop_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ; 
ATTR r_attr, l_attr ;
DML_LIST loop_list ;
{
    RETURN ( t2d_break_region1 ( region, r_attr, l_attr, TRUE, loop_list ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2d_break_region1 ( region, r_attr, l_attr, 
            all_x, loop_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ; 
ATTR r_attr, l_attr ;
BOOLEAN all_x ;
DML_LIST loop_list ;
{
    DML_LIST temp_list ;
    T2_REGION strip ;
    PT2 origin ;
    DML_ITEM item, last ;
    T2_LOOP loop ;

    temp_list = t2d_break_region_right ( region, r_attr, all_x, NULL ) ;
    c2a_box_get_ctr ( T2_REGION_BOX(region), origin ) ;
    if ( loop_list == NULL ) 
        loop_list = dml_create_list() ;
    last = DML_LAST(loop_list) ;

    DML_WALK_LIST ( temp_list, item ) {
        loop = DML_RECORD(item) ;
        t2c_rotate_cs_loop ( loop, origin, -1.0, 0.0 ) ;
        strip = t2d_loop_to_region ( loop ) ;
        loop_list = t2d_break_region_right ( strip, l_attr, 
            /* all_x, */ TRUE, loop_list ) ;
        t2d_clear_region ( strip ) ;
    }
    dml_destroy_list ( temp_list, ( PF_ACTION ) t2d_free_loop ) ;
    temp_list = NULL ;
    last = ( last == NULL ) ? DML_FIRST(loop_list) : DML_NEXT(last) ;
    for ( item = last ; item != NULL ; item = DML_NEXT(item) ) {
        loop = DML_RECORD(item) ;
        t2c_rotate_cs_loop ( loop, origin, -1.0, 0.0 ) ;
        t2d_mark_edges ( loop ) ;
    }
    RETURN ( loop_list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2d_break_region_left ( region, attr, loop_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ; 
ATTR attr ;
DML_LIST loop_list ;
{
    PT2 origin ;
    DML_ITEM item, last ;

    c2a_box_get_ctr ( T2_REGION_BOX(region), origin ) ;
    if ( loop_list == NULL ) 
        loop_list = dml_create_list() ;
    last = DML_LAST(loop_list) ;

    t2c_rotate_cs_region ( region, origin, -1.0, 0.0 ) ;
    loop_list = t2d_break_region_right ( region, attr, TRUE, loop_list ) ;
    t2d_clear_region ( region ) ;
    last = ( last == NULL ) ? DML_FIRST(loop_list) : DML_NEXT(last) ;
    for ( item = last ; item != NULL ; item = DML_NEXT(item) ) 
        t2c_rotate_cs_loop ( DML_RECORD(item), origin, -1.0, 0.0 ) ;
    RETURN ( loop_list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2d_break_region_right ( region, attr, all_x, loop_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ; 
ATTR attr ;
BOOLEAN all_x ;
DML_LIST loop_list ;
{
    DML_LIST yx_list ;

#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    clear_screen () ;
    paint_region ( region, 12 ) ;
    getch ();
}
else
    DISPLAY-- ;
#endif

    yx_list = t2d_region_y_extrs ( region, attr, all_x ) ;
    if ( yx_list == NULL ) 
        RETURN ( NULL ) ;
    if ( loop_list == NULL ) 
        loop_list = dml_create_list ();
    t2d_break_pass1 ( region, yx_list ) ;
    if ( DML_LENGTH(yx_list) == 0 ) {
        dml_append_data ( loop_list, 
            t2d_copy_loop ( T2_REGION_EXT_LOOP(region), NULL, NULL ) ) ;
#ifdef CCDK_DEBUG
{
DML_ITEM item ;
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
DML_WALK_LIST ( loop_list, item ) {
    paint_loop ( DML_RECORD(item), 12 ) ;
    getch ();
}
}
else
    DISPLAY-- ;
}
#endif
        dml_free_list ( yx_list ) ;
        yx_list = NULL ;
        RETURN ( loop_list ) ;
    }
    t2d_break_pass2 ( yx_list, loop_list ) ;
    dml_destroy_list ( yx_list, ( PF_ACTION ) t2d_free_yx ) ;
    yx_list = NULL ;
    t2i_clear_links_region ( region ) ;
#ifdef CCDK_DEBUG
{
DML_ITEM item ;
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
DML_WALK_LIST ( loop_list, item ) {
    paint_loop ( DML_RECORD(item), 12 ) ;
    getch ();
}
}
else
    DISPLAY-- ;
}
#endif
    RETURN ( loop_list ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_LIST t2d_region_y_extrs ( region, attr, all_x )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ; 
ATTR attr ;
BOOLEAN all_x ;
{
    DML_LIST yx_list = dml_create_list ();
    DML_ITEM item ;
    T2_LOOP loop ;

    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) {
        loop = DML_RECORD(item) ;
        t2d_loop_y_extrs ( loop, attr, all_x, yx_list ) ;
    }
    dml_sort_list ( yx_list, ( PF_SORT ) t2d_break_sort_key ) ;
#ifdef CCDK_DEBUG
{
DML_ITEM item ;
T2_YX yx ;
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
DML_WALK_LIST ( yx_list, item ) {
    yx = DML_RECORD(item) ;
    paint_point ( T2_YX_PT0(yx), 0.03, 12 ) ;
    getch ();
}
}
else
    DISPLAY-- ;
}
#endif
    RETURN ( yx_list ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2d_free_yx ( yx ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_YX yx ;
{
    FREE ( yx ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2d_break_sort_key ( yx0, yx1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_YX yx0, yx1 ;
{
    if ( IS_SMALL ( T2_YX_PT0(yx0)[1] - T2_YX_PT0(yx1)[1] ) ) 
        RETURN ( T2_YX_PT0(yx0)[0] > T2_YX_PT0(yx1)[0] ) ;
    else 
        RETURN ( T2_YX_PT0(yx0)[1] > T2_YX_PT0(yx1)[1] ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2d_loop_y_extrs ( loop, attr, all_x, yx_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ; 
ATTR attr ;
BOOLEAN all_x;
DML_LIST yx_list ;
{
    DML_ITEM item, item1 ;
    T2_EDGE edge, edge1 ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        if ( ( attr == (ATTR)0 ) || ( T2_EDGE_ATTR(edge) & attr ) )
            t2d_edge_y_extrs ( edge, yx_list ) ;
    }

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        item1 = DML_NEXT(item) ;
        if ( item1 != NULL ) {
            edge1 = DML_RECORD(item1) ;
            if ( ( attr == (ATTR)0 ) || ( ( T2_EDGE_ATTR(edge) & attr ) && 
                 ( T2_EDGE_ATTR(edge1) & attr ) ) )
                t2d_vtx_y_extr ( edge, edge1, all_x, yx_list ) ;
        }
    }
    if ( T2_LOOP_CLOSED(loop) ) {
        edge = DML_LAST_RECORD(T2_LOOP_EDGE_LIST(loop)) ;
        edge1 = DML_FIRST_RECORD(T2_LOOP_EDGE_LIST(loop)) ;
        if ( ( attr == (ATTR)0 ) || ( ( T2_EDGE_ATTR(edge) & attr ) && 
            ( T2_EDGE_ATTR(edge1) & attr ) ) )
            t2d_vtx_y_extr ( edge, edge1, all_x, yx_list ) ;
    }
}


/*-------------------------------------------------------------------------*/
STATIC void t2d_edge_y_extrs ( edge, yx_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ; 
DML_LIST yx_list ;
{
    DML_LIST fn_list = dml_create_list() ;
    DML_ITEM item ;
    FN_EXTR_REC extr_rec ;
    PT2 pt ;

    c2c_coord_extrs ( T2_EDGE_CURVE(edge), 1, fn_list ) ;
    DML_WALK_LIST ( fn_list, item ) {
        extr_rec = ( FN_EXTR_REC ) dml_record ( item ) ;
        if ( t2d_cnvx_extr ( edge, FN_EXTR_REC_PARM(extr_rec), 
            FN_EXTR_REC_TYPE(extr_rec), pt ) ) 
            t2d_append_yx ( edge, FN_EXTR_REC_PARM(extr_rec), pt, yx_list ) ;
        FREE ( extr_rec ) ;
        extr_rec = NULL ;
    }
    dml_free_list ( fn_list ) ;
    fn_list = NULL ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2d_cnvx_extr ( edge, parm, type, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM parm ;
INT type ;
PT2 pt ;
{
    PT2 tan ;
/* Added 10-21-91 - for rough turning - */
    if ( IS_ZERO ( T2_EDGE_T0(edge) - PARM_T(parm) ) || 
         IS_ZERO ( T2_EDGE_T1(edge) - PARM_T(parm) ) )
        RETURN ( FALSE ) ;
    t2c_eval_pt_tan ( edge, parm, pt, tan ) ;

    if ( type == -1 ) /* min */
        RETURN ( tan[0] < 0.0 ) ;
    else if ( type == 1 ) /* max */
        RETURN ( tan[0] > 0.0 ) ;
    else 
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2d_vtx_y_extr ( edge0, edge1, all_x, yx_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge0, edge1 ;
BOOLEAN all_x ; 
DML_LIST yx_list ;
{
    PT2 p, tan0, tan1 ;

    t2c_ept_tan1 ( edge0, p, tan0 ) ;
    c2v_normalize ( tan0, tan0 ) ;
    t2c_etan0 ( edge1, tan1 ) ;
    c2v_normalize ( tan1, tan1 ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_point ( p, 0.03, 6 ) ;
    getch ();
}
else
    DISPLAY-- ;
#endif

    if ( ( ( tan0[1] >= -BBS_ZERO && tan1[1] <= BBS_ZERO ) ||
           ( tan1[1] >= -BBS_ZERO && tan0[1] <= BBS_ZERO ) ) &&
            ( C2V_CROSS ( tan0, tan1 ) <= BBS_ZERO ) && 
/* Added 10-21-91 */ 
/*
            ( all_x || ( !IS_SMALL(tan0[1]) || !IS_SMALL(tan1[1]) ) ) )
*/
            ( all_x || t2d_vtx_y_extr_hor ( edge0, edge1, p, tan0, tan1 ) ) )
        t2d_append_yx ( edge0, T2_EDGE_PARM1(edge0), p, yx_list ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2d_vtx_y_extr_hor ( edge0, edge1, p, tan0, tan1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge0, edge1 ; 
PT2 p, tan0, tan1 ;
{
    BOOLEAN b0, b1 ;
    if ( !IS_SMALL(tan0[1]) || !IS_SMALL(tan1[1]) )
        RETURN ( TRUE ) ;
    if ( ( tan0[0] < 0.0 ) && T2_EDGE_IS_LINE(edge0) )
        RETURN ( FALSE ) ;
    if ( ( tan0[0] > 0.0 ) && T2_EDGE_IS_LINE(edge1) )
        RETURN ( FALSE ) ;
    b0 = t2d_prev_pt ( edge0, p ) ;
    b1 = t2d_next_pt ( edge1, p ) ;
    RETURN ( b0 == b1 ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2d_append_yx ( edge, parm, pt, yx_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ; 
PARM parm ;
PT2 pt ;
DML_LIST yx_list ;
{
    T2_YX yx ;
    DML_ITEM item ;
    BOOLEAN same_loop = TRUE ;

    for ( item = dml_last(yx_list) ; same_loop && item != NULL ; 
        item=dml_prev(item) ) { 
        yx = DML_RECORD(item) ;
        same_loop = ( T2_EDGE_LOOP(T2_YX_EDGE(yx)) == T2_EDGE_LOOP(edge) ) ;
        if ( same_loop && C2V_IDENT_PTS ( pt, T2_YX_PT0(yx) ) ) 
            RETURN ;
    }

    yx = MALLOC (1,  T2_YX_S ) ;
    T2_YX_EDGE(yx) = edge ;
    COPY_PARM ( parm, T2_YX_PARM(yx) ) ;
    if ( pt == NULL ) 
        t2c_eval_pt ( edge, parm, T2_YX_PT0(yx) ) ;
    else 
        C2V_COPY ( pt, T2_YX_PT0(yx) ) ;
    dml_append_data ( yx_list, yx ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_point ( T2_YX_PT0(yx), 0.03, 5 ) ;
    getch ();
}
else
    DISPLAY-- ;
#endif
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2d_prev_pt ( edge0, p0 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge0 ; 
PT2 p0 ;
{
    T2_EDGE edge ;
    PT2 p ;
    INT n ;

    for ( edge = edge0, n = 0 ; ; edge = t2c_prev_edge ( edge ), n++ ) {
        if ( ( n != 0 ) && ( edge == edge0 ) ) 
            RETURN ( FALSE ) ;
        t2c_mid_pt ( edge, p ) ;
        if ( p[1] > p0[1] + BBS_TOL ) 
            RETURN ( TRUE ) ;
        else if ( p[1] < p0[1] - BBS_TOL ) 
            RETURN ( FALSE ) ;
    }
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2d_next_pt ( edge0, p0 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge0 ; 
PT2 p0 ;
{
    T2_EDGE edge ;
    PT2 p ;
    INT n ;

    for ( edge = edge0, n = 0 ; ; edge = t2c_next_edge ( edge ), n++ ) {
        if ( ( n != 0 ) && ( edge == edge0 ) ) 
            RETURN ( FALSE ) ;
        t2c_mid_pt ( edge, p ) ;
        if ( p[1] > p0[1] + BBS_TOL ) 
            RETURN ( TRUE ) ;
        else if ( p[1] < p0[1] - BBS_TOL ) 
            RETURN ( FALSE ) ;
    }
}


/*-------------------------------------------------------------------------*/
STATIC void t2d_break_pass1 ( region, yx_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
DML_LIST yx_list ;
{
    DML_ITEM item, item1 ;
    T2_YX yx, yx_prev ;
    T2_PARM_S int_parm ;
    T2_LINK link ;
    T2_EDGE edge1, edge2 ;

    yx_prev = NULL ;
    t2i_clear_links_region ( region ) ;
    for ( item=DML_FIRST(yx_list) ; item!=NULL ; item = item1 ) {
        item1=DML_NEXT(item) ; 
        yx = (T2_YX) DML_RECORD(item) ;
        if ( t2d_yx_valid ( region, yx, yx_prev, &int_parm ) ) {
            edge1 = T2_PARM_EDGE(&int_parm) ;
            edge2 = T2_YX_EDGE(yx) ;
            link = t2i_create_link ( 
                edge1, T2_PARM_CPARM(&int_parm), T2_LINK_XLE, T2_YX_PT1(yx), 
                edge2, T2_YX_PARM(yx), T2_LINK_XLE, T2_YX_PT0(yx), 
                TRUE, FALSE ) ;
            T2_YX_LINK(yx) = link ;
            yx_prev = yx ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_line ( T2_YX_PT0(yx), T2_YX_PT1(yx), 5 ) ;
    getch ();
}
else
    DISPLAY-- ;
#endif
        }
        else {
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_point ( T2_YX_PT0(yx), 0.02, 6 ) ;
    getch ();
}
else
    DISPLAY-- ;
#endif
            T2_YX_LINK(yx) = NULL ;
            FREE ( yx ) ;
            dml_remove_item ( yx_list, item ) ;
            item = NULL ;
        }
    }

    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) 
        t2d_sort_loop_links ( DML_RECORD(item) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2d_yx_valid ( region, yx, yx_prev, int_parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
T2_YX yx, yx_prev ;
T2_PARM int_parm ;
{
    PT2 pt ;
    T2_PT_POSITION pt_pos ;

    if ( !t2i_region_ray ( region, T2_YX_PT0(yx), 0.0, 
        T2_YX_PT1(yx), int_parm ) ) {
        if ( yx_prev != NULL && 
            IS_SMALL ( T2_YX_PT0(yx_prev)[1] - T2_YX_PT0(yx)[1] ) ) {
            T2_PARM_EDGE(int_parm) = T2_YX_EDGE(yx_prev) ;
            PARM_COPY ( T2_YX_PARM(yx_prev), T2_PARM_CPARM(int_parm) ) ;
            C2V_COPY ( T2_YX_PT0(yx_prev), T2_YX_PT1(yx) ) ;
        }
        else
            RETURN ( FALSE ) ;
    }
    if ( C2V_IDENT_PTS ( T2_YX_PT0(yx), T2_YX_PT1(yx) ) )
        RETURN ( FALSE ) ;

    if ( IS_ZERO ( T2_YX_T(yx) - T2_EDGE_T0 ( T2_YX_EDGE(yx) ) ) ||
         IS_ZERO ( T2_YX_T(yx) - T2_EDGE_T1 ( T2_YX_EDGE(yx) ) ) ) {
        C2V_MID_PT ( T2_YX_PT0(yx), T2_YX_PT1(yx), pt ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_point ( pt, 0.02, 7 ) ;
    getch ();
}
else
    DISPLAY-- ;
#endif
        pt_pos = t2c_pt_pos_region ( region, pt ) ;
        if ( pt_pos != T2_PT_INSIDE )
            RETURN ( FALSE ) ; 
    }
    if ( yx_prev != NULL && 
        C2V_IDENT_PTS ( T2_YX_PT1(yx_prev), T2_YX_PT1(yx) ) ) 
        RETURN ( FALSE ) ; 
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2d_sort_loop_links ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    DML_ITEM item ;
    T2_LINK link0, link1 ;

    if ( loop == NULL || T2_LOOP_LINK_LIST(loop) == NULL ) 
        RETURN ;
    DML_WALK_LIST ( T2_LOOP_LINK_LIST(loop), item ) {
        link0 = DML_RECORD(item) ;
        if ( DML_NEXT(item) == NULL ) 
            RETURN ;
        link1 = DML_RECORD(DML_NEXT(item)) ;
        if ( C2V_IDENT_PTS ( T2_LINK_PT(link0), T2_LINK_PT(link1) ) ) {
            if ( !t2d_links_order ( link0 ) ) {
                DML_RECORD(item) = link1 ;
                DML_RECORD(DML_NEXT(item)) = link0 ;
            }
        }
    }
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2d_links_order ( link ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LINK link ;
{
    T2_LINK mate ;
    PT2 tan ;
    t2c_eval_tan ( T2_LINK_EDGE(link), T2_LINK_CPARM(link), tan ) ;
    mate = T2_LINK_MATE(link) ;
    RETURN ( ( tan[0] > 0.0 ) == 
        ( T2_LINK_PT(mate)[0] - T2_LINK_PT(link)[0] < 0.0 ) ) ;
}

/*-------------------------------------------------------------------------*/
STATIC void t2d_break_pass2 ( yx_list, loop_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST yx_list ;
DML_LIST loop_list ;
{
    T2_LOOP loop ;

    while ( DML_LENGTH(yx_list) ) {
        loop = t2d_break_loop ( yx_list ) ;
        dml_append_data ( loop_list, loop ) ;
    }
}


/*-------------------------------------------------------------------------*/
STATIC T2_LOOP t2d_break_loop ( yx_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST yx_list ;
{
    DML_LIST edgelist = dml_create_list() ;
    T2_YX yx ;
    T2_LINK link0, link1 ;
    BOOLEAN yb_edge ;
    T2_LOOP loop ;

    yx = DML_FIRST_RECORD(yx_list) ;
    link0 = T2_YX_LINK ( yx ) ;
    link1 = link0 ;
    yb_edge = ( T2_LINK_STATUS(link0) == T2_LINK_XLE || 
        T2_LINK_STATUS(link0) == T2_LINK_XE ) ;
    do {
        link1 = t2d_break_traverse ( link1, yx_list, yb_edge, edgelist ) ;
    }
    while ( link1 != link0 ) ;
/* New code 02-19-91 */
    if ( T2_LINK_STATUS(link0) == T2_LINK_X ) {
        t2d_free_yx ( yx ) ;
        yx = NULL ;
        dml_remove_first ( yx_list ) ;
    }
/* New code 02-19-91 */
    loop = t2d_create_loop ( NULL, edgelist ) ;
    RETURN ( loop ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_LINK t2d_break_traverse ( link, yx_list, yb_edge, edgelist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LINK link ;
BOOLEAN yb_edge ;
DML_LIST yx_list, edgelist ;
{
    T2_EDGE edge, edge1, edge2 ;
    T2_LINK link1, link2 ;

    if ( yb_edge ) {
        link1 = T2_LINK_MATE(link) ;
        edge = t2d_connect_links ( link ) ;
        dml_append_data ( edgelist, edge ) ;
        link2 = t2i_next_link ( link1 ) ;
        edge1 = T2_LINK_EDGE(link1) ;
        edge2 = T2_LINK_EDGE(link2) ;
        t2d_append_trim ( edge1, T2_LINK_CPARM(link1), 
            edge2, T2_LINK_CPARM(link2), T2_ATTR_INIT, edgelist ) ;
        if ( T2_LINK_STATUS(link) == T2_LINK_XLE ) 
            T2_LINK_STATUS(link) = T2_LINK_XL ;
        else if ( T2_LINK_STATUS(link) == T2_LINK_XE ) 
            T2_LINK_STATUS(link) = T2_LINK_X ;
        if ( T2_LINK_STATUS(link1) == T2_LINK_XLE ) 
            T2_LINK_STATUS(link1) = T2_LINK_XE ;
        else if ( T2_LINK_STATUS(link1) == T2_LINK_XL ) 
            T2_LINK_STATUS(link1) = T2_LINK_X ;
    }

    else {
        link1 = t2i_next_link ( link ) ;
        edge1 = T2_LINK_EDGE(link) ;
        edge2 = T2_LINK_EDGE(link1) ;
        t2d_append_trim ( edge1, T2_LINK_CPARM(link), 
            edge2, T2_LINK_CPARM(link1), T2_ATTR_INIT, edgelist ) ;
        link2 = T2_LINK_MATE(link1) ;
        edge = t2d_connect_links ( link1 ) ;
        dml_append_data ( edgelist, edge ) ;

        if ( T2_LINK_STATUS(link) == T2_LINK_XLE ) 
            T2_LINK_STATUS(link) = T2_LINK_XE ;
        else if ( T2_LINK_STATUS(link) == T2_LINK_XL ) 
            T2_LINK_STATUS(link) = T2_LINK_X ;
        if ( T2_LINK_STATUS(link1) == T2_LINK_XLE ) 
            T2_LINK_STATUS(link1) = T2_LINK_XL ;
        else if ( T2_LINK_STATUS(link1) == T2_LINK_XE ) 
            T2_LINK_STATUS(link1) = T2_LINK_X ;
    }

    if ( T2_LINK_STATUS(link) == T2_LINK_X ) 
        t2d_remove_link ( yx_list, link ) ;
    if ( T2_LINK_STATUS(link1) == T2_LINK_X ) 
        t2d_remove_link ( yx_list, link1 ) ;

    RETURN ( link2 ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2d_remove_link ( yx_list, link ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST yx_list ;
T2_LINK link ;
{
    DML_ITEM item ;
    T2_YX yx ;
    yx = DML_FIRST_RECORD(yx_list) ;
    if ( T2_YX_LINK(yx) == link ) 
        RETURN ;
/* New code 02-19-91 */
    DML_WALK_LIST ( yx_list, item ) {
        yx = DML_RECORD(item) ;
        if ( T2_YX_LINK(yx) == link ) {
            t2d_free_yx ( yx ) ;
            yx = NULL ;
            dml_remove_item ( yx_list, item ) ;
            item = NULL ;
            RETURN ;
        }
    }
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2d_connect_links ( link0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LINK link0 ;
{
    T2_LINK link1 = T2_LINK_MATE(link0) ;
    T2_EDGE edge = t2d_create_edge ( NULL, 
        c2d_line ( T2_LINK_PT(link0), T2_LINK_PT(link1) ), 1 ) ;
    if ( T2_LINK_PT(link0)[0] > T2_LINK_PT(link1)[0] ) 
        T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_ATTR_HORLINE ; 
    else
        T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_ATTR_HORLINE ; 
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( edge, 13 ) ;
    getch ();
}
else
    DISPLAY-- ;
#endif
    RETURN ( edge ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2d_mark_edges ( loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    T2_EDGE edge ;
    DML_ITEM item ;
    PT2 tan_vec ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        if ( T2_EDGE_ATTR(edge) & T2_ATTR_HORLINE ) {
            t2c_etan0 ( edge, tan_vec ) ;
            if ( tan_vec[0] > 0.0 ) 
                T2_EDGE_ATTR(edge) = 
                    T2_EDGE_ATTR(edge) | T2_ATTR_BOTTOM ;
            else
                T2_EDGE_ATTR(edge) = 
                    T2_EDGE_ATTR(edge) | T2_ATTR_TOP ;
            T2_EDGE_ATTR(edge) = 
                T2_EDGE_ATTR(edge) & ~T2_ATTR_HORLINE ;
        }
    }
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

