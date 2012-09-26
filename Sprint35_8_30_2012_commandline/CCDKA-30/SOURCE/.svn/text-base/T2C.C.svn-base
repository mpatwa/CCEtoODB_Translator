/* -R -Z -S -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/******************************* T2C.C **********************************/ 
/**************** Two-dimensional topology ******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <dmldefs.h> 
#include <c2cdefs.h> 
#include <c2cpriv.h> 
#include <c2vdefs.h> 
#include <m2cdefs.h>
#include <t2cdefs.h>
#include <t2cpriv.h>
#include <c2vmcrs.h> 
GLOBAL  REAL        T2_GAP_COEFF = 100.0 ;
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_eval_pt ( edge, parm, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM parm ;
PT2 pt ;
{
    RETURN ( edge != NULL && T2_EDGE_CURVE(edge) != NULL && 
        c2c_eval ( T2_EDGE_CURVE(edge), parm, 0, (PT2*)pt ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_eval_tan ( edge, parm, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM parm ;
PT2 tan_vec ;
{
    BOOLEAN status = edge != NULL && T2_EDGE_CURVE(edge) != NULL && 
        c2c_eval_tan ( T2_EDGE_CURVE(edge), parm, tan_vec ) ;
    if ( status && T2_EDGE_REVERSED(edge) )
        C2V_NEGATE ( tan_vec, tan_vec ) ;
    RETURN ( status ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_eval_pt_tan ( edge, parm, pt, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM parm ;
PT2 pt ;
PT2 tan_vec ;
{
    BOOLEAN status = edge != NULL && T2_EDGE_CURVE(edge) != NULL && 
        c2c_eval_pt_tan ( T2_EDGE_CURVE(edge), parm, pt, tan_vec ) ;
    if ( (T2_EDGE_DIR(edge)!=1) && status && ( tan_vec != NULL ) )
        C2V_NEGATE ( tan_vec, tan_vec ) ;
    RETURN ( status ) ;
}   


/*----------------------------------------------------------------------*/
BBS_PUBLIC REAL t2c_eval_tan_angle ( edge, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM parm ;
{
    PT2 tan_vec ;
    REAL angle ;
    if ( t2c_eval_tan ( edge, parm, tan_vec ) ) {
        c2v_normalize ( tan_vec, tan_vec ) ;
        angle = c2v_atan2 ( tan_vec ) ;
        if ( angle >= TWO_PI - BBS_ZERO ) 
            angle = 0.0 ;
        RETURN ( angle ) ;
    }
    RETURN ( 0.0 ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_eval_offset ( edge, parm, offset, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM parm ;
REAL offset ;
PT2 pt ;
{
    if ( T2_EDGE_DIR(edge) == -1 ) 
        offset = - offset ;
    RETURN ( c2c_eval_offset_pt ( T2_EDGE_CURVE(edge), parm, offset, pt ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_ept0 ( edge, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 pt ;
{
    RETURN ( edge != NULL && T2_EDGE_CURVE(edge) != NULL && 
        c2c_eval_pt ( T2_EDGE_CURVE(edge), T2_EDGE_PARM0(edge), pt ) ) ; 
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_etan0 ( edge, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 tan_vec ;
{
    BOOLEAN status ;
    status = edge != NULL && T2_EDGE_CURVE(edge) != NULL && 
        c2c_eval_tan ( T2_EDGE_CURVE(edge), T2_EDGE_PARM0(edge), tan_vec ) ;
    if ( (T2_EDGE_DIR(edge)!=1) && status )
        C2V_NEGATE ( tan_vec, tan_vec ) ;
    RETURN ( status ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN  t2c_ept_tan0 ( edge, pt, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 pt ;
PT2 tan_vec ;
{
    BOOLEAN status ;
    status = edge != NULL && T2_EDGE_CURVE(edge) != NULL && 
        c2c_eval_pt_tan ( T2_EDGE_CURVE(edge), T2_EDGE_PARM0(edge), 
            pt, tan_vec ) ;
    if ( (T2_EDGE_DIR(edge)!=1) && status && ( tan_vec != NULL ) )
        C2V_NEGATE ( tan_vec, tan_vec ) ;
    RETURN ( status ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_ept_offset0 ( edge, offset, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
REAL offset ;
PT2 pt ;
{
    RETURN ( t2c_eval_offset ( edge, T2_EDGE_PARM0(edge), offset, pt ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC REAL t2c_eval_angle0 ( edge ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
{
    RETURN ( t2c_eval_tan_angle ( edge, T2_EDGE_PARM0(edge) ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_mid_pt ( edge, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 pt ;
{
    RETURN ( edge != NULL && T2_EDGE_CURVE(edge) != NULL && 
        c2c_midpt_dir ( T2_EDGE_CURVE(edge), 1, pt, NULL ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_mid_pt_parm ( edge, pt, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 pt ;
T2_PARM parm ;
{
    if ( edge == NULL || T2_EDGE_CURVE(edge) == NULL )
        RETURN ( FALSE ) ;
    if ( parm == NULL ) 
        RETURN ( c2c_midpt_dir ( T2_EDGE_CURVE(edge), 1, pt, NULL ) ) ;
    else {
        T2_PARM_EDGE(parm) = edge ;
        c2c_mid_parm ( T2_EDGE_CURVE(edge), T2_PARM_CPARM(parm) ) ;
        RETURN ( pt == NULL || 
            c2c_eval_pt ( T2_EDGE_CURVE(edge), T2_PARM_CPARM(parm), pt ) ) ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_mid_pt_tan ( edge, pt, tan ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 pt, tan ;
{
    RETURN ( edge != NULL && T2_EDGE_CURVE(edge) != NULL && 
        c2c_midpt_dir ( T2_EDGE_CURVE(edge), T2_EDGE_DIR(edge), 
        pt, tan ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC void t2c_mid_parm ( edge, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM parm ;
{
    c2c_mid_parm ( T2_EDGE_CURVE(edge), parm ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_ept1 ( edge, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 pt ;
{
    RETURN ( edge != NULL && T2_EDGE_CURVE(edge) != NULL && 
        c2c_eval_pt ( T2_EDGE_CURVE(edge), T2_EDGE_PARM1(edge), pt ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_etan1 ( edge, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 tan_vec ;
{
    BOOLEAN status ;
    status = edge != NULL && T2_EDGE_CURVE(edge) != NULL && 
        c2c_eval_tan ( T2_EDGE_CURVE(edge), T2_EDGE_PARM1(edge), tan_vec ) ;
    if ( (T2_EDGE_DIR(edge)!=1) && status )
        C2V_NEGATE ( tan_vec, tan_vec ) ;
    RETURN ( status ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_ept_tan1 ( edge, pt, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 pt ;
PT2 tan_vec ;
{
    BOOLEAN status ;
    status = edge != NULL && T2_EDGE_CURVE(edge) != NULL && 
        c2c_eval_pt_tan ( T2_EDGE_CURVE(edge), T2_EDGE_PARM1(edge), 
            pt, tan_vec ) ;
    if ( (T2_EDGE_DIR(edge)!=1) && status && ( tan_vec != NULL ) )
        C2V_NEGATE ( tan_vec, tan_vec ) ;
    RETURN ( status ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC REAL t2c_eval_angle1 ( edge ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
{
    RETURN ( t2c_eval_tan_angle ( edge, T2_EDGE_PARM1(edge) ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_ept_offset1 ( edge, offset, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
REAL offset ;
PT2 pt ;
{
    RETURN ( t2c_eval_offset ( edge, T2_EDGE_PARM1(edge), offset, pt ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_vec ( edge, vec ) /* ept1-ept0 */
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 vec ;
{
    PT2 p0, p1 ;
    if ( !t2c_ept0 ( edge, p0 ) )
        RETURN ( FALSE ) ;
    if ( !t2c_ept1 ( edge, p1 ) )
        RETURN ( FALSE ) ;
    C2V_SUB ( p1, p0, vec ) ;
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_closed ( edge ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
{
    RETURN ( edge != NULL && T2_EDGE_CURVE(edge) != NULL && 
        c2c_closed ( T2_EDGE_CURVE(edge) ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_loop_ept0 ( loop, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt ;
{
    RETURN ( loop != NULL && T2_LOOP_EDGE_LIST(loop) != NULL && 
        DML_LENGTH(T2_LOOP_EDGE_LIST(loop)) != 0 &&
        t2c_ept0 ( t2c_first_edge ( loop ), pt ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_loop_ept_tan0 ( loop, pt, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt, tan_vec ;
{
    RETURN ( loop != NULL && T2_LOOP_EDGE_LIST(loop) != NULL && 
        DML_LENGTH(T2_LOOP_EDGE_LIST(loop)) != 0 &&
        t2c_ept_tan0 ( t2c_first_edge ( loop ), pt, tan_vec ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_loop_offset0 ( loop, offset, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL offset ;
PT2 pt ;
{
    RETURN ( loop != NULL && T2_LOOP_EDGE_LIST(loop) != NULL && 
        DML_LENGTH(T2_LOOP_EDGE_LIST(loop)) != 0 &&
        t2c_ept_offset0 ( t2c_first_edge ( loop ), offset, pt ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_loop_ept1 ( loop, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt ;
{
    RETURN ( loop != NULL && T2_LOOP_EDGE_LIST(loop) != NULL && 
        DML_LENGTH(T2_LOOP_EDGE_LIST(loop)) != 0 &&
        t2c_ept1 ( dml_last_record ( T2_LOOP_EDGE_LIST(loop) ), pt ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_loop_ept_tan1 ( loop, pt, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt, tan_vec ;
{
    RETURN ( loop != NULL && T2_LOOP_EDGE_LIST(loop) != NULL && 
        DML_LENGTH(T2_LOOP_EDGE_LIST(loop)) != 0 &&
        t2c_ept_tan1 ( dml_last_record ( T2_LOOP_EDGE_LIST(loop) ), 
            pt, tan_vec ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_loop_offset1 ( loop, offset, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL offset ;
PT2 pt ;
{
    RETURN ( loop != NULL && T2_LOOP_EDGE_LIST(loop) != NULL && 
        DML_LENGTH(T2_LOOP_EDGE_LIST(loop)) != 0 &&
        t2c_ept_offset1 ( dml_last_record ( T2_LOOP_EDGE_LIST(loop) ), 
            offset, pt ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_loop_closed ( loop ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    PT2 p0, p1 ;
    if ( !t2c_loop_ept0 ( loop, p0 ) ) 
        RETURN ( FALSE ) ;
    if ( !t2c_loop_ept1 ( loop, p1 ) )
        RETURN ( FALSE ) ;
    T2_LOOP_CLOSED(loop) = C2V_IDENT_PTS ( p0, p1 ) ;
    RETURN ( T2_LOOP_CLOSED(loop) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_loop_ept_smooth ( loop ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    PT2 t0, t1 ;

    RETURN ( T2_LOOP_CLOSED(loop) && 
        t2c_loop_ept_tan0 ( loop, NULL, t0 ) && 
        t2c_loop_ept_tan1 ( loop, NULL, t1 ) && 
        c2v_vecs_parallel ( t0, t1 ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_loop_smooth_at ( parm ) 
/*----------------------------------------------------------------------*/
T2_PARM parm ;
{
    PT2 t0, t1 ;
    T2_EDGE edge ;

    if ( IS_ZERO ( T2_PARM_T(parm) - T2_EDGE_T0(T2_PARM_EDGE(parm)) ) ) {
        edge = t2c_prev_edge ( T2_PARM_EDGE(parm) ) ;
        RETURN ( edge != NULL && t2c_etan0 ( T2_PARM_EDGE(parm), t0 ) && 
            t2c_etan1 ( edge, t1 ) && c2v_vecs_parallel ( t0, t1 ) ) ;
    }
    if ( IS_ZERO ( T2_PARM_T(parm) - T2_EDGE_T1(T2_PARM_EDGE(parm)) ) ) {
        edge = t2c_next_edge ( T2_PARM_EDGE(parm) ) ;
        RETURN ( edge != NULL && t2c_etan1 ( T2_PARM_EDGE(parm), t0 ) && 
            t2c_etan0 ( edge, t1 ) && c2v_vecs_parallel ( t0, t1 ) ) ;
    }
    else
        RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT t2c_tan_angle ( edge, angle, parm )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
REAL angle ;
PARM parm ;
{
    RETURN ( m2c_tan_angle ( T2_EDGE_CURVE(edge), angle, parm ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_EDGE t2c_prev_edge ( edge )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
{
    T2_LOOP loop = T2_EDGE_LOOP(edge) ;
    DML_ITEM item ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ( NULL ) ;
/* Added 08-11-91 */
    if ( edge == DML_FIRST_RECORD ( T2_LOOP_EDGE_LIST(loop) ) ) 
        RETURN ( t2c_loop_closed ( loop ) ? 
            DML_LAST_RECORD ( T2_LOOP_EDGE_LIST(loop) ) : NULL ) ;
/* Added 08-11-91 */
    item = dml_find_data ( T2_LOOP_EDGE_LIST(loop), edge ) ;
    if ( item == NULL ) 
        RETURN ( NULL ) ;
    RETURN ( dml_record ( dml_prev ( item ) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_EDGE t2c_next_edge ( edge )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
{
    T2_LOOP loop = T2_EDGE_LOOP(edge) ;
    DML_ITEM item ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ( NULL ) ;
/* Added 08-11-91 */
    if ( edge == DML_LAST_RECORD ( T2_LOOP_EDGE_LIST(loop) ) ) 
        RETURN ( t2c_loop_closed ( loop ) ? 
            DML_FIRST_RECORD ( T2_LOOP_EDGE_LIST(loop) ) : NULL ) ;
/* Added 08-11-91 */
    item = dml_find_data ( T2_LOOP_EDGE_LIST(loop), edge ) ;
    if ( item == NULL ) 
        RETURN ( NULL ) ;
    RETURN ( dml_record ( dml_next ( item ) ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_loop_pt ( loop, p, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 p ;
T2_PARM parm ;
{
    DML_ITEM item ;
    T2_EDGE edge ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ||
        DML_LENGTH ( T2_LOOP_EDGE_LIST(loop) ) == 0 ) 
        RETURN ( FALSE ) ;
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        if ( t2c_mid_pt_parm ( edge, p, parm ) )
            RETURN ( TRUE ) ;
    }
    if ( parm != NULL ) {
        T2_PARM_EDGE(parm) = t2c_first_edge ( loop ) ;
        PARM_COPY ( T2_EDGE_PARM0(T2_PARM_EDGE(parm)), T2_PARM_CPARM(parm) ) ;
    }
    RETURN ( t2c_loop_ept0 ( loop, p ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_loop_pt_attr ( loop, attr, p, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
ATTR attr ;
PT2 p ;
T2_PARM parm ;
{
    DML_ITEM item ;
    T2_EDGE edge ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ||
        DML_LENGTH ( T2_LOOP_EDGE_LIST(loop) ) == 0 ) 
        RETURN ( FALSE ) ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        if ( ( T2_EDGE_ATTR(edge) & attr ) && 
            t2c_mid_pt_parm ( edge, p, parm ) )
            RETURN ( TRUE ) ;
    }
    RETURN ( FALSE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_loop_pt_closest ( loop, p0, pt, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 p0, pt ;
T2_PARM parm ;
{
    DML_ITEM item ;
    T2_EDGE edge ;
    REAL dist, dist0 ;
    T2_PARM_S parm0, parm1 ;
    PT2 p1 ;

    if ( parm == NULL ) 
        parm = &parm1 ;
    T2_PARM_EDGE(parm) = NULL ;
    dist0 = 0.0 ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ||
        DML_LENGTH ( T2_LOOP_EDGE_LIST(loop) ) == 0 ) 
        RETURN ( FALSE ) ;
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        if ( t2c_mid_pt_parm ( edge, p1, &parm0 ) ) {
            dist = C2V_DIST ( p1, p0 ) ;
            if ( ( T2_PARM_EDGE(parm) == NULL ) || ( dist < dist0 ) ) {
                dist0 = dist ;
                T2_COPY_PARM ( &parm0, parm ) ;
                if ( pt != NULL ) {
                    C2V_COPY ( p1, pt ) ;
                }
            }
        }
    }
    RETURN ( ( T2_PARM_EDGE(parm) != NULL ) || t2c_loop_ept0 ( loop, pt ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT t2c_offset_dir ( loop, bias_pt, side )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 bias_pt ;
T2_SIDE side ;
{
    PT2 on_pt, tangent, normal ;
    REAL cross ;
    T2_PARM_S parm ;

    if ( side == T2_SIDE_RIGHT )
        RETURN ( 1 ) ;
    else if ( side == T2_SIDE_LEFT )
        RETURN ( -1 ) ;
    else if ( ( side == T2_SIDE_BIAS_PT ) && ( bias_pt != NULL ) &&
        t2c_project_loop ( loop, bias_pt, TRUE, &parm, on_pt ) ) {
        t2c_eval_tan ( T2_PARM_EDGE(&parm), T2_PARM_CPARM(&parm), tangent ) ;
        C2V_SUB ( bias_pt, on_pt, normal ) ;
        cross = C2V_CROSS ( normal, tangent ) ;
        if ( IS_SMALL(cross) ) 
            RETURN ( 0 ) ;
        else if ( cross > 0.0 ) 
            RETURN ( 1 ) ;
        else 
            RETURN ( -1 ) ;
    }
    else 
        RETURN ( 0 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2c_trim_edge ( edge, parm0, parm1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM parm0, parm1 ;
{
    if ( T2_EDGE_DIR(edge)==1 ) {
        if ( parm0 != NULL )
            c2c_trim0 ( T2_EDGE_CURVE(edge), parm0 ) ;
        if ( parm1 != NULL )
            c2c_trim1 ( T2_EDGE_CURVE(edge), parm1 ) ;
    }
    else {
        if ( parm0 != NULL )
            c2c_trim1 ( T2_EDGE_CURVE(edge), parm0 ) ;
        if ( parm1 != NULL )
            c2c_trim0 ( T2_EDGE_CURVE(edge), parm1 ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_trim_edge_verify ( edge, parm0, parm1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM parm0, parm1 ;
{
    BOOLEAN status = TRUE ;

    if ( T2_EDGE_DIR(edge) == 1 ) {
        if ( parm0 != NULL ) {
            if ( c2c_trim0_verify ( T2_EDGE_CURVE(edge), parm0 ) == NULL )
                status = FALSE ;
        }
        if ( parm1 != NULL ) {
            if ( c2c_trim1_verify ( T2_EDGE_CURVE(edge), parm1 ) == NULL )
                status = FALSE ;
        }
    }
    else {
        if ( parm0 != NULL ) {
            if ( c2c_trim1_verify ( T2_EDGE_CURVE(edge), parm0 ) == NULL )
                status = FALSE ;
        }
        if ( parm1 != NULL ) {
            if ( c2c_trim0_verify ( T2_EDGE_CURVE(edge), parm1 ) == NULL )
                status = FALSE ;
        }
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL t2c_edge_size ( edge )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
{
    RETURN ( m2c_size ( T2_EDGE_CURVE(edge) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_EDGE t2c_biggest_edge ( loop )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    DML_ITEM item ;
    T2_EDGE big_edge = NULL, edge ;
    REAL size, max_size=0.0 ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ( NULL ) ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        size = m2c_size ( T2_EDGE_CURVE(edge) ) ;
        if ( size > max_size ) {
            size = max_size ;
            big_edge = edge ;
        }
    }
    RETURN ( big_edge ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_EDGE t2c_first_edge ( loop )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    RETURN ( loop == NULL ? NULL : 
        dml_first_record ( T2_LOOP_EDGE_LIST(loop) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_EDGE t2c_last_edge ( loop )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    RETURN ( loop == NULL ? NULL : 
        dml_last_record ( T2_LOOP_EDGE_LIST(loop) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2c_first_edge_attr ( loop, attr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
ATTR attr ;
{
    DML_ITEM item ;
    T2_EDGE edge ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ( NULL ) ;
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        if ( T2_EDGE_ATTR(edge) & attr )
            RETURN ( edge ) ;
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2c_first_edge_no_attr ( loop, attr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
ATTR attr ;
{
    DML_ITEM item ;
    T2_EDGE edge ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ( NULL ) ;
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        if ( !( T2_EDGE_ATTR(edge) & attr ) )
            RETURN ( edge ) ;
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2c_last_edge_no_attr ( loop, attr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
ATTR attr ;
{
    DML_ITEM item ;
    T2_EDGE edge ;
    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ( NULL ) ;
    for ( item = DML_LAST ( T2_LOOP_EDGE_LIST(loop) ) ; 
        item != NULL ; item = DML_PREV(item) ) {
        edge = DML_RECORD(item) ;
        if ( !( T2_EDGE_ATTR(edge) & attr ) )
            RETURN ( edge ) ;
    }
    RETURN ( NULL ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE REAL t2c_get_gap_coeff __(( void )) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    RETURN ( T2_GAP_COEFF ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void t2c_set_gap_coeff ( gap_coeff ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL gap_coeff ;
{
    T2_GAP_COEFF = gap_coeff ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */
