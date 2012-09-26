/* -S __BBS_MILL__=3 */
/********************************* T2PS2.C *********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h>
#include <c2ddefs.h>
#include <c2vdefs.h>
#include <t2bdefs.h>
#include <t2bpriv.h>
#include <t2cdefs.h>
#include <t2ddefs.h>
#include <t2idefs.h>
#include <t2ipriv.h>
#include <t2pdefs.h>
#include <t2link.h>
#include <t2sprlm.h>
#include <c2vmcrs.h>

#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif
#if ( __BBS_MILL__>=3 )
STATIC BOOLEAN t2p_sharp_vtx __(( T2_EDGE, T2_EDGE, REAL, REAL )) ;
STATIC BOOLEAN t2p_sharp_vtx_tan __(( PT2, PT2, REAL, REAL )) ;
STATIC BOOLEAN t2p_spiral_extend_vtx __(( T2_EDGE, T2_EDGE, 
            REAL, REAL, T2_LOOP )) ;
STATIC BOOLEAN t2p_spiral_vtx_pt __(( T2_EDGE, T2_EDGE, PT2, PT2, PT2, PT2 )) ;
STATIC BOOLEAN t2p_inters_bisector __(( T2_EDGE, T2_EDGE, PT2, PT2, PT2 )) ;
STATIC void t2p_diff __(( DML_LIST, DML_LIST, T2_SPIRAL )) ;
STATIC void t2p_diff_pass1 __(( DML_LIST, T2_SPIRAL )) ;
STATIC void t2p_inter_loops __(( T2_REGION, DML_LIST, INT, T2_SPIRAL )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2p_sharp_loop ( loop, step, rad )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL step, rad ;
{
    DML_ITEM item0, item1 ;

#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -2 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop, 9 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item0 ) {
        item1 = DML_NEXT(item0) ;
        if ( item1 == NULL ) 
            item1 = DML_FIRST ( T2_LOOP_EDGE_LIST(loop) ) ;
        if ( t2p_sharp_vtx ( DML_RECORD(item0), DML_RECORD(item1), 
            step, rad ) ) 
            RETURN ( TRUE ) ;
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2p_sharp_vtx ( edge0, edge1, step, rad )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge0, edge1 ;
REAL step, rad ;
{
    PT2 tan0, tan1 ;

    t2c_etan1 ( edge0, tan0 ) ;
    t2c_etan0 ( edge1, tan1 ) ;
    RETURN ( t2p_sharp_vtx_tan ( tan0, tan1, step, rad ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2p_sharp_vtx_tan ( tan0, tan1, step, rad )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 tan0, tan1 ;
REAL step, rad ;
{
    REAL s ;

    c2v_normalize ( tan0, tan0 ) ;
    c2v_normalize ( tan1, tan1 ) ;
    s = C2V_CROSS ( tan0, tan1 ) ;
    if ( ( s <= - BBS_ZERO ) || 
        ( ( s < BBS_ZERO ) && ( C2V_DOT ( tan0, tan1 ) > 0.0 ) ) ) 
        RETURN ( FALSE ) ;
    s = sqrt ( 0.5 * ( 1.0 + C2V_DOT ( tan0, tan1 ) ) ) ; /* sin(0.5*angle) */
    if ( rad >= step / ( 1.0 + s ) ) 
        RETURN ( FALSE ) ;
    else 
        RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2p_spiral_extend ( loop, parent, step, rad )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop, parent ;
REAL step, rad ;
{
    DML_ITEM item0, item1 ;
    T2_EDGE edge0, edge1 ;
    DML_LIST offset_list ;
    T2_LOOP offset ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -2 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop, 9 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
    if ( parent == NULL ) 
        parent = T2_LOOP_PARENT(loop) ;
    if ( parent == NULL ) 
        RETURN ( FALSE ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -2 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( parent, 10 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif

    offset_list = t2b_offset_loop ( parent, -rad, NULL, NULL ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -2 || DISPLAY <= DIS_LEVEL ) {
    DML_ITEM item ;
    DML_WALK_LIST ( offset_list, item ) {
        paint_loop ( DML_RECORD(item), 11 ) ;
        getch () ;
    }
}
else
    DISPLAY-- ;
#endif
    if ( dml_length ( offset_list ) != 1 ) {
        dml_destroy_list ( offset_list, ( PF_ACTION ) t2d_free_loop ) ;
        RETURN ( FALSE ) ;
    }
    offset = DML_FIRST_RECORD(offset_list) ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item0 ) {
        item1 = DML_NEXT(item0) ;
        if ( item1 == NULL ) 
            item1 = DML_FIRST(T2_LOOP_EDGE_LIST(loop)) ;
        edge0 = (T2_EDGE)DML_RECORD(item0) ;
        edge1 = (T2_EDGE)DML_RECORD(item1) ;
        if ( !t2p_spiral_extend_vtx ( edge0, edge1, step, rad, offset ) ) {
            dml_destroy_list ( offset_list, ( PF_ACTION ) t2d_free_loop ) ;
            RETURN ( FALSE ) ;
        }
    }
    dml_destroy_list ( offset_list, ( PF_ACTION ) t2d_free_loop ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/ 
STATIC BOOLEAN t2p_spiral_extend_vtx ( edge0, edge1, step, rad, offset ) 
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
T2_EDGE edge0, edge1 ;
REAL step, rad ;
T2_LOOP offset ;
{
    REAL w ;
    PT2 p0, tan0, tan1, q0, q1, q ;
    T2_PARM_S parm0, parm1 ;

    t2c_ept_tan1 ( edge0, p0, tan0 ) ;
    t2c_etan0 ( edge1, tan1 ) ;
    if ( !t2p_sharp_vtx_tan ( tan0, tan1, step, rad ) )
        RETURN ( TRUE ) ;

    w = step - rad ;
    C2V_SET ( p0[0] + w * tan0[1], p0[1] - w * tan0[0], q0 ) ;
    C2V_SET ( p0[0] + w * tan1[1], p0[1] - w * tan1[0], q1 ) ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_loop ( offset, 9 ) ;
        getch ();
        paint_point ( q0, 0.02, 10 ) ;
        getch ();
        paint_point ( q1, 0.02, 11 ) ;
        getch ();
    }
    else
        DISPLAY-- ;
#endif
    if ( !t2c_project_loop ( offset, q0, TRUE, &parm0, q ) ||
        !C2V_IDENT_PTS(q0,q) )
        RETURN ( FALSE ) ;                           
    if ( !t2c_project_loop ( offset, q1, TRUE, &parm1, q ) ||
        !C2V_IDENT_PTS(q1,q) )
        RETURN ( FALSE ) ;
    if ( !t2p_spiral_vtx_pt ( T2_PARM_EDGE(&parm0), 
        T2_PARM_EDGE(&parm1), p0, tan0, tan1, T2_EDGE_SC_PT(edge0) ) ) {
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_edge ( edge0, 9 ) ;
        getch ();
        paint_edge ( edge1, 10 ) ;
        getch ();
        paint_edge ( T2_PARM_EDGE(&parm0), 11 ) ;
        getch ();
        paint_edge ( T2_PARM_EDGE(&parm1), 12 ) ;
        getch ();
    }
    else
        DISPLAY-- ;
    if ( DIS_LEVEL == -2 ) {
        T2_EDGE edge ;
        FILE *file = fopen ( "edges.dat", "w" ) ;
        t2c_put_edge ( file, edge0 ) ;
        t2c_put_edge ( file, edge1 ) ;
        for ( edge = T2_PARM_EDGE(&parm0) ; edge != NULL && file != NULL ; 
            edge = t2c_next_edge ( edge ) ) {
            t2c_put_edge ( file, edge ) ;
            if ( edge == T2_PARM_EDGE(&parm1) ) {
                fclose ( file ) ;
                file = NULL ;
            }
        }
    }
#endif
        RETURN ( FALSE ) ;
    }
    T2_EDGE_VTX(edge0) = T2_VTX_SHARP ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/ 
STATIC BOOLEAN t2p_spiral_vtx_pt ( edge0, edge1, pt, tan0, tan1, vtx_pt )
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
T2_EDGE edge0, edge1 ;
PT2 pt, tan0, tan1, vtx_pt ;
{
    T2_EDGE edge ;
    PT2 bisector ;

    edge = t2c_next_edge ( edge0 ) ;
    if ( edge == edge1 )
        RETURN ( t2c_ept1 ( edge0, vtx_pt ) ) ;
    C2V_SUB ( tan0, tan1, bisector ) ;
    RETURN ( t2p_inters_bisector ( edge0, edge1, pt, bisector, vtx_pt ) ) ;
}


/*-------------------------------------------------------------------------*/ 
STATIC BOOLEAN t2p_inters_bisector ( edge0, edge1, pt, bisector, vtx_pt )
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
T2_EDGE edge0, edge1 ;
PT2 pt, bisector, vtx_pt ;
{
    DML_ITEM item0, item ;
    REAL dist0, dist ;
    C2_CURVE ray ;
    DML_LIST intlist ;
    T2_EDGE edge ;
    T2_INT_REC ti ;

#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_edge ( edge0, 9 ) ;
        getch ();
        paint_edge ( edge1, 10 ) ;
        getch ();
        paint_loop ( T2_EDGE_LOOP(edge0), 11 ) ;
        getch ();
    }
    else
        DISPLAY-- ;
#endif
    ray = c2d_line_dir ( pt, bisector ) ;
    intlist = dml_create_list () ;
    for ( edge = t2c_next_edge ( edge0 ) ; edge != edge1 ; 
        edge = t2c_next_edge ( edge ) ) 
        t2i_intersect_ray_edge ( ray, edge, 
            FALSE, FALSE, FALSE, intlist ) ;

    item0 = NULL ;
    dist0 = 0.0 ;

    DML_WALK_LIST ( intlist, item ) {
        ti = (T2_INT_REC)DML_RECORD(item) ;
        dist = C2V_DIST ( pt, T2_INT_REC_PT(ti) ) ;
        if ( item0 == NULL || dist < dist0 ) {
            item0 = item ;
            dist0 = dist ;
        }
    }
    if ( item0 != NULL ) {
        ti = (T2_INT_REC)DML_RECORD(item0) ;
        C2V_COPY ( T2_INT_REC_PT(ti), vtx_pt ) ; 
    }
    dml_destroy_list ( intlist, ( PF_ACTION ) t2i_free_int_rec ) ;
    c2d_free_curve ( ray ) ;
    RETURN ( item0 != NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_leftover ( region0, region_list, rad, cut_mode, 
            spiral ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region0 ;
DML_LIST region_list ;
REAL rad ;
INT cut_mode ;
T2_SPIRAL spiral ;
{
    DML_ITEM item ;
    DML_LIST exter_list, inter_list ;
    T2_REGION region ;

#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_region ( region0, 9 ) ;
        getch ();
        DML_WALK_LIST ( region_list, item ) {
            paint_region ( DML_RECORD(item), 10 ) ;
            getch ();
        }
    }
    else
        DISPLAY-- ;
#endif
    inter_list = dml_create_list () ;

    DML_WALK_LIST ( region_list, item ) {
        region = (T2_REGION)DML_RECORD(item) ;
        t2b_offset_region ( region, rad, NULL, inter_list ) ;
    }
    exter_list = t2b_offset_extloop ( region0, -rad, NULL, NULL ) ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        DML_WALK_LIST ( inter_list, item ) {
            paint_region ( DML_RECORD(item), 11 ) ;
            getch ();
        }
        DML_WALK_LIST ( exter_list, item ) {
            paint_region ( DML_RECORD(item), 12 ) ;
            getch ();
        }
    }
    else
        DISPLAY-- ;
#endif
    t2p_diff ( exter_list, inter_list, spiral ) ;
    t2p_inter_loops ( region0, inter_list, cut_mode, spiral ) ;
    dml_destroy_list ( inter_list, ( PF_ACTION ) t2d_free_region ) ;
    dml_destroy_list ( exter_list, ( PF_ACTION ) t2d_free_region ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_leftover_1 ( region0, region_list, rad, cut_mode, 
            spiral ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region0 ;
DML_LIST region_list ;
REAL rad ;
INT cut_mode ;
T2_SPIRAL spiral ;
{
    DML_ITEM item ;
    DML_LIST inter_list ;
    T2_REGION region ;

    if ( DML_LENGTH ( T2_REGION_LOOP_LIST(region0) ) <= 1 ) 
        RETURN ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_region ( region0, 9 ) ;
        getch ();
        DML_WALK_LIST ( region_list, item ) {
            paint_region ( DML_RECORD(item), 10 ) ;
            getch ();
        }
    }
    else
        DISPLAY-- ;
#endif

    inter_list = dml_create_list () ;

    DML_WALK_LIST ( region_list, item ) {
        region = (T2_REGION)DML_RECORD(item) ;
        t2b_offset_region ( region, rad, NULL, inter_list ) ;
    }
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        DML_WALK_LIST ( inter_list, item ) {
            paint_region ( DML_RECORD(item), 11 ) ;
            getch ();
        }
    }
    else
        DISPLAY-- ;
#endif
    t2p_inter_loops ( region0, inter_list, cut_mode, spiral ) ;
    dml_destroy_list ( inter_list, ( PF_ACTION ) t2d_free_region ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2p_diff ( exter_list, inter_list, spiral ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST exter_list, inter_list ;
T2_SPIRAL spiral ;
{
    DML_ITEM item ;
    DML_LIST l_list ;

    l_list = dml_create_list () ;
    DML_WALK_LIST ( exter_list, item ) 
        t2b_diff_pass1 ( (T2_REGION)DML_RECORD(item), inter_list, l_list ) ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        DML_WALK_LIST ( l_list, item ) {
            paint_region ( DML_RECORD(item), 13 ) ;
            getch ();
        }
    }
    else
        DISPLAY-- ;
#endif
#ifdef OLD_CODE
    DML_WALK_LIST ( l_list, item ) {
        region = (T2_REGION)DML_RECORD(item) ;
        loop = T2_REGION_EXT_LOOP(region) ;
        sub_spiral = t2p_create_spiral () ;
        if ( sub_spiral == NULL ) 
            RETURN ;
        t2p_append_spiral_loop ( sub_spiral, loop ) ;
        t2p_append_spiral ( spiral, sub_spiral ) ;
    }

    dml_apply ( l_list, t2d_clear_region ) ;
    dml_free_list ( l_list ) ;
#endif
    t2p_diff_pass1 ( l_list, spiral ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2p_diff_pass1 ( l_list, spiral ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST l_list ;
T2_SPIRAL spiral ;
{
    DML_ITEM item ;
    T2_LOOP loop ;
    T2_REGION region ;
    T2_SPIRAL sub_spiral ;

    DML_WALK_LIST ( l_list, item ) {
        region = (T2_REGION)DML_RECORD(item) ;
        loop = T2_REGION_EXT_LOOP(region) ;
        sub_spiral = t2p_create_spiral () ;
        if ( sub_spiral == NULL ) 
            RETURN ;
        t2p_append_spiral_loop ( sub_spiral, loop ) ;
        t2p_append_spiral ( spiral, sub_spiral ) ;
    }

    dml_apply ( l_list, ( PF_ACTION ) t2d_clear_region ) ;
    dml_free_list ( l_list ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2p_inter_loops ( region, inter_list, cut_mode, spiral ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
DML_LIST inter_list ;
INT cut_mode ;
T2_SPIRAL spiral ;
{
    DML_ITEM item ;
    DML_LIST templist = NULL, looplist ;

#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_region ( region, 9 ) ;
        getch ();
        DML_WALK_LIST ( inter_list, item ) {
            paint_region ( DML_RECORD(item), 11 ) ;
            getch ();
        }
    }
    else
        DISPLAY-- ;
#endif
    looplist = T2_REGION_LOOP_LIST(region) ;

    for ( item = dml_second(looplist) ; item != NULL ; 
        item = DML_NEXT(item) ) {
            templist = t2b_loop_outside_region_list ( DML_RECORD(item), 
                    inter_list, templist ) ;
    }

    if ( templist != NULL ) {
        if ( cut_mode & 2 ) {   /* inside_only */
            DML_WALK_LIST ( templist, item ) {
                T2_SPIRAL_OPEN_LIST(spiral) = 
                    t2b_loop_inside_loop ( DML_RECORD(item), 
                        T2_REGION_EXT_LOOP(region), 
                        T2_SPIRAL_OPEN_LIST(spiral) ) ;
            }
            dml_destroy_list ( templist, ( PF_ACTION ) t2d_free_loop ) ;
        }
        else {
            if ( T2_SPIRAL_OPEN_LIST(spiral) == NULL ) 
                T2_SPIRAL_OPEN_LIST(spiral) = templist ;
            else {
                dml_append_list ( T2_SPIRAL_OPEN_LIST(spiral), templist ) ;
                dml_free_list ( templist ) ;
            }
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2p_offset ( region, step, rad, offset_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
REAL step, rad ;
DML_LIST offset_list ;
{
    REAL dist ;

    offset_list = t2b_offset_extloop ( region, -rad, NULL, offset_list ) ;
    if ( IS_SMALL(step) )
        RETURN ( offset_list ) ;
    else if ( dml_length ( offset_list ) >= 1 )
        RETURN ( offset_list ) ;
    else {
        for ( dist = 0.9*rad ; dist > 0.5 * rad ; dist *= 0.9 ) {
            dml_free_list ( offset_list ) ;
            offset_list = t2b_offset_extloop ( region, -dist, NULL, NULL ) ;
            if ( dml_length ( offset_list ) >= 1 )
                RETURN ( offset_list ) ;
        }
        RETURN ( NULL ) ;
    }
}
#endif /* __BBS_MILL__>=3 */

