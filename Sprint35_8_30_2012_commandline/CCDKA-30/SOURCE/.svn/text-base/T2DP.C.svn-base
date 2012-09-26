/* -Z -S -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2DP.C **********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2cdefs.h>
#include <c2ddefs.h>
#include <c2vdefs.h>
#include <dmldefs.h>
#include <t2bdefs.h>
#include <t2cdefs.h>
#include <t2cpriv.h>
#include <t2ddefs.h>
#include <t2dpriv.h>
#include <t2idefs.h>
#include <c2vmcrs.h>
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC void t2d_clsd_leadin __(( T2_LOOP, T2_LEAD_MODE, REAL, REAL, 
            T2_EDGE, T2_EDGE, PT2 )) ;
STATIC void t2d_clsd_leadout __(( T2_LOOP, T2_LEAD_MODE, REAL, T2_EDGE )) ;
STATIC C2_CURVE t2d_leadin_tangent __(( T2_LOOP, T2_EDGE, PARM, REAL )) ;
STATIC C2_CURVE t2d_leadout_tangent __(( T2_LOOP, REAL )) ;
STATIC BOOLEAN t2d_arc_pts_in __(( REAL, T2_EDGE, T2_EDGE, PT2, 
            PT2, PT2, PT2, PARM )) ;
STATIC BOOLEAN t2d_normal_pts_in __(( REAL, T2_EDGE, T2_EDGE, PT2, 
            PT2, PT2, PARM )) ;
STATIC BOOLEAN t2d_arc_pts_out __(( REAL, T2_LOOP, T2_EDGE, PT2, PT2, PT2 )) ;
STATIC BOOLEAN t2d_normal_pts_out __(( REAL, T2_LOOP, T2_EDGE, PT2, PT2 )) ;
STATIC void t2d_ext_open_leadin __(( T2_LOOP, REAL )) ;
STATIC void t2d_ext_open_leadout __(( T2_LOOP, REAL )) ;

/*----------------------------------------------------------------------*/
BBS_PRIVATE void t2d_loop_leads ( loop, lead_mode, lead_rad, lead_size, 
            edge0, lead_edge, dir, start_pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_LEAD_MODE lead_mode ;
REAL lead_rad, lead_size ;
T2_EDGE edge0, lead_edge ;
INT dir ;
PT2 start_pt ;
{

#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_loop ( loop, 9 ) ;
        getch();
        if ( edge0 != NULL ) {
            paint_edge ( edge0, 10 ) ;
            getch();
        }
        if ( lead_edge != NULL ) {
            paint_edge ( lead_edge, 11 ) ;
            getch();
            paint_loop ( T2_EDGE_LOOP(lead_edge), 12 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
#endif

    if ( T2_LOOP_CLOSED(loop) && edge0 != NULL ) {
        t2d_clsd_leadin ( loop, lead_mode, lead_rad, lead_size, 
                edge0, lead_edge, start_pt ) ;
        t2d_clsd_leadout ( loop, lead_mode, lead_rad, lead_edge ) ;
    }
    else {
        t2d_open_leadin ( loop, lead_mode, lead_rad, lead_size, dir ) ;
        t2d_open_leadout ( loop, lead_mode, lead_rad, lead_size, dir ) ;
    }
}


/*----------------------------------------------------------------------*/
STATIC void t2d_clsd_leadin ( loop, lead_mode, lead_rad, lead_size, 
            edge0, lead_edge, start_pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_LEAD_MODE lead_mode ;
REAL lead_rad, lead_size ;
T2_EDGE edge0, lead_edge ;
PT2 start_pt ;
{
    T2_PARM_S tparm ;
    C2_CURVE lead = NULL ;
    T2_EDGE lead_ext = NULL ;
    PT2 p0, p1, tan ;
    T2_EDGE edge ;
    PARM_S parm ;
    REAL norm, dt ;

    if ( lead_mode == T2_LEAD_NONE ) 
        RETURN ;

#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_loop ( loop, 9 ) ;
        getch();
        if ( edge0 != NULL ) {
            paint_edge ( edge0, 10 ) ;
            getch();
        }
        if ( lead_edge != NULL ) {
            paint_edge ( lead_edge, 11 ) ;
            getch();
            paint_loop ( T2_EDGE_LOOP(lead_edge), 12 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
#endif

    if ( lead_mode == T2_LEAD_ARC || lead_mode == T2_LEAD_EXT_ARC ) {
        if ( t2d_arc_pts_in ( lead_rad, edge0, lead_edge, start_pt, 
            p0, p1, tan, T2_PARM_CPARM(&tparm) ) )
            lead = c2d_arc_2pts_tan1 ( p0, p1, tan ) ;
        else {
            if ( lead_mode == T2_LEAD_ARC ) 
                lead_mode = T2_LEAD_NORMAL ;
            else if ( lead_mode == T2_LEAD_EXT_ARC ) 
                lead_mode = T2_LEAD_EXT_NORMAL ;
        }
    }

    else if ( lead_mode == T2_LEAD_NORMAL || lead_mode == T2_LEAD_EXT_NORMAL ) {
        if ( t2d_normal_pts_in ( lead_rad, edge0, lead_edge, start_pt, 
            p0, p1, T2_PARM_CPARM(&tparm) ) )
            lead = c2d_line ( p0, p1 ) ;
    }

    else if ( lead_mode == T2_LEAD_TANGENT ) {
        if ( edge0 == NULL ) 
            edge0 = t2c_first_edge ( loop ) ;
        t2c_mid_parm ( edge0, T2_PARM_CPARM(&tparm) ) ;
        lead = t2d_leadin_tangent ( loop, edge0, 
            T2_PARM_CPARM(&tparm), lead_rad ) ;
    }

#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_curve ( lead, 13 ) ;
        getch();
    }
    else
        DISPLAY-- ;
#endif
    T2_PARM_EDGE(&tparm) = edge0 ;
    t2c_loop_rearrange_parm ( loop, &tparm ) ;

    if ( lead_mode == T2_LEAD_EXT_ARC || lead_mode == T2_LEAD_EXT_NORMAL ) {
        edge = t2c_first_edge ( loop ) ;
        t2c_etan0 ( edge, tan ) ;
        norm = C2V_NORM ( tan ) ;
        dt = ( IS_SMALL ( norm ) ) ? 0.0 : ( lead_size / norm ) ;
        if ( !IS_SMALL ( dt ) ) {
            PARM_COPY ( T2_EDGE_PARM0(edge), &parm ) ;
            if ( T2_EDGE_DIR(edge) == 1 ) {
                PARM_T(&parm) += dt ;
                if ( PARM_T(&parm) > T2_EDGE_T1(edge) ) 
                    PARM_T(&parm) = T2_EDGE_T1(edge) ;
            }
            else {
                PARM_T(&parm) -= dt ;
                if ( PARM_T(&parm) < T2_EDGE_T1(edge) ) 
                    PARM_T(&parm) = T2_EDGE_T1(edge) ;
            }
            t2c_parm_adjust ( edge, &parm ) ;
            lead_ext = t2d_trim_edge ( edge, NULL, &parm ) ;
            if ( lead_ext != NULL ) 
                t2d_append_edge ( loop, lead_ext, FALSE ) ;
        }
    }

    if ( lead != NULL ) {
        edge = t2d_create_edge ( loop, lead, 1 ) ;
        t2d_append_edge ( loop, edge, TRUE ) ;
    }
}


/*----------------------------------------------------------------------*/
STATIC C2_CURVE t2d_leadin_tangent ( loop, edge0, parm, lead_rad ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_EDGE edge0 ;
PARM parm ;
REAL lead_rad ;
{
    PT2 pt0, pt1, tan ;
    C2_CURVE line ;
    DML_LIST int_list ;
    T2_INT_REC ti, ti0 ;
    DML_ITEM item ;

    t2c_eval_pt_tan ( edge0, parm, pt1, tan ) ;
    if ( !c2v_normalize ( tan, tan ) ) 
        RETURN ( NULL ) ;
    C2V_ADDT ( pt1, tan, -lead_rad, pt0 ) ;
    line = c2d_line ( pt0, pt1 ) ;
    int_list = dml_create_list () ;
    t2i_loop_curve ( loop, line, FALSE, int_list ) ;
    ti0 = NULL ;

    DML_WALK_LIST ( int_list, item ) {
        ti = (T2_INT_REC)DML_RECORD(item) ;
        if ( ( T2_INT_REC_TYPE(ti) >= 0 ) && 
            ( T2_INT_REC_T2(ti) < 1.0 - BBS_ZERO ) ) {
            if ( ( ti0 == NULL ) || 
                ( T2_INT_REC_T2(ti) > T2_INT_REC_T2(ti0) ) )
                ti0 = ti ;
        }
    }

    if ( ti0 != NULL ) {
        if ( T2_INT_REC_T2(ti0) < 1.0 - BBS_ZERO )
            c2c_trim0 ( line, T2_INT_REC_PARM2(ti0) ) ;
        else {
            c2d_free_curve ( line ) ;
            line = NULL ;
        }
    }

    dml_destroy_list ( int_list, ( PF_ACTION ) t2i_free_int_rec ) ;
    RETURN ( line ) ;
}


/*----------------------------------------------------------------------*/
STATIC void t2d_clsd_leadout ( loop, lead_mode, lead_rad, lead_edge ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_LEAD_MODE lead_mode ;
REAL lead_rad ;
T2_EDGE lead_edge ;
{
    C2_CURVE lead = NULL ;
    PT2 p0, p1, tan ;
    T2_EDGE edge ;

    if ( lead_mode == T2_LEAD_NONE ) 
        RETURN ;

    if ( lead_mode == T2_LEAD_ARC || lead_mode == T2_LEAD_EXT_ARC ) {
        if ( t2d_arc_pts_out ( lead_rad, loop, lead_edge, p0, p1, tan ) )
            lead = c2d_arc_2pts_tan ( p0, tan, p1 ) ;
        else {
            if ( lead_mode == T2_LEAD_ARC ) 
                lead_mode = T2_LEAD_NORMAL ;
            else if ( lead_mode == T2_LEAD_EXT_ARC ) 
                lead_mode = T2_LEAD_EXT_NORMAL ;
        }
    }

    else if ( lead_mode == T2_LEAD_NORMAL || lead_mode == T2_LEAD_EXT_NORMAL ) {
        if ( t2d_normal_pts_out ( lead_rad, loop, lead_edge, p0, p1 ) )
            lead = c2d_line ( p0, p1 ) ;
    }

    else if ( lead_mode == T2_LEAD_TANGENT ) 
        lead = t2d_leadout_tangent ( loop, lead_rad ) ;

    if ( lead != NULL ) {
        edge = t2d_create_edge ( loop, lead, 1 ) ;
        t2d_append_edge ( loop, edge, FALSE ) ;
    }
}


/*----------------------------------------------------------------------*/
STATIC C2_CURVE t2d_leadout_tangent ( loop, lead_rad ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL lead_rad ;
{
    PT2 pt0, pt1, tan ;
    C2_CURVE line ;
    DML_LIST int_list ;
    T2_INT_REC ti, ti0 ;
    DML_ITEM item ;

    t2c_loop_ept_tan1 ( loop, pt0, tan ) ;
    if ( !c2v_normalize ( tan, tan ) ) 
        RETURN ( NULL ) ;
    C2V_ADDT ( pt0, tan, lead_rad, pt1 ) ;
    line = c2d_line ( pt0, pt1 ) ;
    int_list = dml_create_list () ;
    t2i_loop_curve ( loop, line, FALSE, int_list ) ;
    ti0 = NULL ;

    DML_WALK_LIST ( int_list, item ) {
        ti = (T2_INT_REC)DML_RECORD(item) ;
        if ( ( T2_INT_REC_TYPE(ti) >= 0 ) && 
            ( T2_INT_REC_T2(ti) > BBS_ZERO ) ) {
            if ( ( ti0 == NULL ) || 
                ( T2_INT_REC_T2(ti) < T2_INT_REC_T2(ti0) ) )
                ti0 = ti ;
        }
    }

    if ( ti0 != NULL ) {
        if ( T2_INT_REC_T2(ti0) > BBS_ZERO )
            c2c_trim0 ( line, T2_INT_REC_PARM2(ti0) ) ;
        else {
            c2d_free_curve ( line ) ;
            line = NULL ;
        }
    }

    dml_destroy_list ( int_list, ( PF_ACTION ) t2i_free_int_rec ) ;
    RETURN ( line ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void t2d_open_leadin ( loop, lead_mode, lead_rad, lead_size, dir ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_LEAD_MODE lead_mode ;
REAL lead_rad, lead_size ;
BOOLEAN dir ;
{
    PT2 p, tan, q, normal ;
    C2_CURVE lead ;
    T2_EDGE edge ;

    if ( lead_mode == T2_LEAD_EXT_ARC ) {
        t2d_ext_open_leadin ( loop, lead_size ) ;
        lead_mode = T2_LEAD_ARC ;
    }
    else if ( lead_mode == T2_LEAD_EXT_NORMAL ) {
        t2d_ext_open_leadin ( loop, lead_size ) ;
        lead_mode = T2_LEAD_NORMAL ;
    }

    lead = NULL ;
    t2c_loop_ept_tan0 ( loop, p, tan ) ;
    c2v_normalize ( tan, tan ) ;

    C2V_SCALE ( tan, fabs(lead_rad), tan ) ;
    if ( lead_mode == T2_LEAD_TANGENT ) {
        C2V_SUB ( p, tan, q ) ;
        lead = c2d_line ( q, p ) ;
    }
    else if ( lead_mode == T2_LEAD_ARC || lead_mode == T2_LEAD_NORMAL ) {
        C2V_NORMAL ( tan, normal ) ;
        if ( dir == -1 ) 
            C2V_NEGATE ( normal, normal ) ;
        if ( lead_rad < 0.0 ) 
            C2V_NEGATE ( normal, normal ) ;
        C2V_ADD ( p, normal, q ) ;
        if ( lead_mode == T2_LEAD_ARC ) {
            C2V_SUB ( q, tan, q ) ;
            lead = c2d_arc_2pts_tan1 ( q, p, tan ) ;
        }
        else if ( lead_mode == T2_LEAD_NORMAL ) 
            lead = c2d_line ( q, p ) ;
    }

    if ( lead != NULL ) {
        t2c_trim_curve_by_loop_0 ( lead, loop ) ;
        edge = t2d_create_edge ( loop, lead, 1 ) ;
        t2d_append_edge ( loop, edge, TRUE ) ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void t2d_open_leadout ( loop, lead_mode, 
            lead_rad, lead_size, dir ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_LEAD_MODE lead_mode ;
REAL lead_rad, lead_size ;
BOOLEAN dir ;
{
    PT2 p, tan, q, normal ;
    C2_CURVE lead ;
    T2_EDGE edge ;

    if ( lead_mode == T2_LEAD_EXT_ARC ) {
        t2d_ext_open_leadout ( loop, lead_size ) ;
        lead_mode = T2_LEAD_ARC ;
    }
    else if ( lead_mode == T2_LEAD_EXT_NORMAL ) {
        t2d_ext_open_leadout ( loop, lead_size ) ;
        lead_mode = T2_LEAD_NORMAL ;
    }

    lead = NULL ;
    t2c_loop_ept_tan1 ( loop, p, tan ) ;
    c2v_normalize ( tan, tan ) ;
    C2V_SCALE ( tan, fabs(lead_rad), tan ) ;
    if ( lead_mode == T2_LEAD_TANGENT ) {
        C2V_ADD ( p, tan, q ) ;
        lead = c2d_line ( p, q ) ;
    }
    else if ( lead_mode == T2_LEAD_ARC || lead_mode == T2_LEAD_NORMAL ) {
        C2V_NORMAL ( tan, normal ) ;
        if ( dir == -1 ) 
            C2V_NEGATE ( normal, normal ) ;
        if ( lead_rad < 0.0 ) 
            C2V_NEGATE ( normal, normal ) ;
        C2V_ADD ( p, normal, q ) ;
        if ( lead_mode == T2_LEAD_ARC ) {
            C2V_ADD ( q, tan, q ) ;
            lead = c2d_arc_2pts_tan ( p, tan, q ) ;
        }
        else if ( lead_mode == T2_LEAD_NORMAL ) 
            lead = c2d_line ( p, q ) ;
    }

    if ( lead != NULL ) {
        t2c_trim_curve_by_loop_1 ( lead, loop ) ;
        edge = t2d_create_edge ( loop, lead, 1 ) ;
        t2d_append_edge ( loop, edge, FALSE ) ;
    }
}


/*----------------------------------------------------------------------*/
STATIC void t2d_ext_open_leadin ( loop, lead_size ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL lead_size ;
{
    C2_CURVE curve ;
    PT2 pt, tan ;

    t2c_loop_ept_tan0 ( loop, pt, tan ) ;
    c2v_normalize ( tan, tan ) ;
    C2V_SCALE ( tan, -lead_size, tan ) ;
    curve = c2d_line_dir ( pt, tan ) ;
    t2d_append_edge ( loop, t2d_create_edge ( loop, curve, -1 ) , TRUE ) ;
}


/*----------------------------------------------------------------------*/
STATIC void t2d_ext_open_leadout ( loop, lead_size ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL lead_size ;
{
    C2_CURVE curve ;
    PT2 pt, tan ;

    t2c_loop_ept_tan1 ( loop, pt, tan ) ;
    c2v_normalize ( tan, tan ) ;
    C2V_SCALE ( tan, lead_size, tan ) ;
    curve = c2d_line_dir ( pt, tan ) ;
    t2d_append_edge ( loop, t2d_create_edge ( loop, curve, 1 ), FALSE ) ;
}


/*----------------------------------------------------------------------*/
STATIC BOOLEAN t2d_arc_pts_in ( lead_rad, edge, lead_edge, start_pt, 
    p0, p1, tan, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL lead_rad ;
T2_EDGE lead_edge, edge ;
PT2 start_pt ;
PT2 p0, tan, p1 ;
PARM parm ;
{
    C2_CURVE circle, ray ;
    DML_LIST int_list ;
    DML_ITEM item ;
    REAL t0, t1 ;
    BOOLEAN status ;
    C2_INT_REC ci ;
    PT2 tan0, c, normal ;
    PARM_S parm0 ;

    int_list = dml_create_list();
    if ( start_pt != NULL ) {
        t2c_project ( lead_edge, start_pt, &parm0, p0 ) ;
        t2c_eval_tan ( lead_edge, &parm0, tan0 ) ;
    }
    else {
        t2c_mid_pt_tan ( lead_edge, p0, tan0 ) ;
        t2c_mid_parm ( lead_edge, &parm0 ) ;
    }
    t0 = C2V_NORM ( tan0 ) ;
    if ( !IS_SMALL(t0) ) 
        PARM_T(&parm0) -= lead_rad / t0 ;
    t2c_eval_pt ( lead_edge, &parm0, p0 ) ;

    circle = c2d_circle ( p0, lead_rad ) ;
    c2c_intersect ( T2_EDGE_CURVE(lead_edge), circle, int_list ) ;
    status = FALSE ;
    DML_WALK_LIST ( int_list, item ) {
        ci = DML_RECORD(item) ;
        if ( ( T2_EDGE_DIR(lead_edge) == 1 ) == 
            ( C2_INT_REC_T1(ci) > PARM_T(&parm0) ) && !status ) {
            PARM_COPY ( C2_INT_REC_PARM1(ci), parm ) ;
            status = TRUE ;
            C2V_COPY ( C2_INT_REC_PT(ci), c ) ;
        }
    }
    c2d_free_curve ( circle ) ;
    dml_destroy_list ( int_list, ( PF_ACTION ) c2d_free_int_rec ) ;
    int_list = NULL ;
    if ( !status )
        RETURN ( FALSE ) ;

    int_list = dml_create_list();
    t2c_eval_tan ( lead_edge, parm, tan ) ;
    c2v_normalize ( tan, tan ) ;
    C2V_SCALE ( tan, lead_rad, tan ) ;
    normal[0] = tan[1] ;
    normal[1] = -tan[0] ;
    ray = c2d_line_dir ( c, normal ) ;
    c2c_intersect_ext ( T2_EDGE_CURVE(edge), ray, int_list ) ;
    status = FALSE ;
    if ( T2_EDGE_T0(edge) < T2_EDGE_T1(edge) ) {
        PARM_T(&parm0) = T2_EDGE_T0(edge) ;
        t1 = T2_EDGE_T1(edge) ;
    }
    else {
        t1 = T2_EDGE_T0(edge) ;
        PARM_T(&parm0) = T2_EDGE_T1(edge) ;
    }
    for ( item = DML_FIRST(int_list) ; item != NULL && !status ; 
        item = DML_NEXT(item) ) {
        ci = DML_RECORD(item) ;
        if ( ( C2_INT_REC_T2(ci) > 0.0 ) && 
            ( PARM_T(&parm0)-BBS_ZERO <= C2_INT_REC_T1(ci) ) &&
            ( t1+BBS_ZERO >= C2_INT_REC_T1(ci) ) ) {
            C2V_COPY ( C2_INT_REC_PT(ci), p1 ) ;
            status = ( IS_SMALL ( C2V_DIST ( c, p1 ) - lead_rad ) ) ;
            PARM_COPY ( C2_INT_REC_PARM1(ci), parm ) ;
        }
    }
    c2d_free_curve ( ray ) ;
    dml_destroy_list ( int_list, ( PF_ACTION ) c2d_free_int_rec ) ;
    int_list = NULL ;
    RETURN ( status ) ;
}


/*----------------------------------------------------------------------*/
STATIC BOOLEAN t2d_normal_pts_in ( lead_rad, edge, lead_edge, start_pt, 
            p0, p1, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL lead_rad ;
T2_EDGE lead_edge, edge ;
PT2 start_pt ;
PT2 p0, p1 ;
PARM parm ;
{
    C2_CURVE ray ;
    DML_LIST int_list ;
    DML_ITEM item ;
    BOOLEAN status ;
    C2_INT_REC ci ;
    PT2 tan, normal ;

    if ( start_pt ) 
        t2c_project ( lead_edge, start_pt, parm, p0 ) ;
    else 
        t2c_mid_parm ( lead_edge, parm ) ;

    int_list = dml_create_list();
    t2c_eval_pt_tan ( lead_edge, parm, p0, tan ) ;
    c2v_normalize ( tan, tan ) ;
    C2V_SCALE ( tan, lead_rad, tan ) ;
    normal[0] = tan[1] ;
    normal[1] = -tan[0] ;
    ray = c2d_line_dir ( p0, normal ) ;
    c2c_intersect_ext ( T2_EDGE_CURVE(edge), ray, int_list ) ;
    status = FALSE ;
    for ( item = DML_FIRST(int_list) ; item != NULL && !status ; 
        item = DML_NEXT(item) ) {
        ci = DML_RECORD(item) ;
        if ( ( C2_INT_REC_T2(ci) > 0.0 ) && 
            ( T2_EDGE_T_MIN(edge)-BBS_ZERO <= C2_INT_REC_T1(ci) ) &&
            ( T2_EDGE_T_MAX(edge)+BBS_ZERO >= C2_INT_REC_T1(ci) ) ) {
            C2V_COPY ( C2_INT_REC_PT(ci), p1 ) ;
            status = ( IS_SMALL ( C2V_DIST ( p0, p1 ) - lead_rad ) ) ;
            PARM_COPY ( C2_INT_REC_PARM1(ci), parm ) ;
        }
    }
    c2d_free_curve ( ray ) ;
    dml_destroy_list ( int_list, ( PF_ACTION ) c2d_free_int_rec ) ;
    int_list = NULL ;
    RETURN ( status ) ;
}


/*----------------------------------------------------------------------*/
STATIC BOOLEAN t2d_arc_pts_out ( lead_rad, loop, lead_edge, p0, p1, tan ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL lead_rad ;
T2_LOOP loop ;
T2_EDGE lead_edge ;
PT2 p0, tan, p1 ;
{
    C2_CURVE circle ;
    DML_LIST int_list ;
    DML_ITEM item ;
    BOOLEAN status ;
    T2_INT_REC ti ;
    PT2 c, normal, vec ;

    int_list = dml_create_list();
    t2c_loop_ept_tan1 ( loop, p0, tan ) ;
    c2v_normalize ( tan, tan ) ;
    C2V_SCALE ( tan, lead_rad, tan ) ;
    normal[0] = -tan[1] ;
    normal[1] = tan[0] ;
    C2V_ADD ( p0, normal, c ) ;
    circle = c2d_circle ( c, lead_rad ) ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_curve ( circle, 13 ) ;
        getch();
    }
    else
        DISPLAY-- ;
#endif
    if ( T2_EDGE_LOOP(lead_edge) == NULL ) 
        t2i_edge_curve ( lead_edge, circle, FALSE, int_list ) ;
    else
        t2i_loop_curve ( T2_EDGE_LOOP(lead_edge), circle, FALSE, int_list ) ;

    status = FALSE ;
    DML_WALK_LIST ( int_list, item ) {
        ti = DML_RECORD(item) ;
        C2V_SUB ( T2_INT_REC_PT(ti), c, vec ) ;
        if ( C2V_DOT ( tan, vec ) > 0.0 ) {
            status = TRUE ;
            C2V_COPY ( T2_INT_REC_PT(ti), p1 ) ;
        }
    }
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_point ( p1, 0.15, 14 ) ;
        getch();
    }
    else
        DISPLAY-- ;
#endif
    c2d_free_curve ( circle ) ;
    dml_destroy_list ( int_list, ( PF_ACTION ) t2i_free_int_rec ) ;
    int_list = NULL ;
    RETURN ( status ) ;
}


/*----------------------------------------------------------------------*/
STATIC BOOLEAN t2d_normal_pts_out ( lead_rad, loop, lead_edge, p0, p1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL lead_rad ;
T2_LOOP loop ;
T2_EDGE lead_edge ;
PT2 p0, p1 ;
{
    BOOLEAN status ;
    PT2 tan, normal, on_pt ;

    t2c_loop_ept_tan1 ( loop, p0, tan ) ;
    c2v_normalize ( tan, tan ) ;
    C2V_SCALE ( tan, lead_rad, tan ) ;
    normal[0] = tan[1] ;
    normal[1] = -tan[0] ;
    C2V_SUB ( p0, normal, p1 ) ;

    status = t2c_project ( lead_edge, p1, NULL, on_pt ) && 
        IS_SMALL ( C2V_DIST ( p1, on_pt ) ) ;

    RETURN ( status ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

