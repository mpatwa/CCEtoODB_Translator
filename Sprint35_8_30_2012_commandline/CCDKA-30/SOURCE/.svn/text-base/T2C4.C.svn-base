/* -R -Z -S -L -T -P __BBS_MILL__=1 __BBS_TURN__=1 __BBS_PUNCH__=1 */
/******************************* T2C4.C *********************************/ 
/**************** Two-dimensional topology ******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h> 
#include <c2adefs.h>
#include <c2cdefs.h>
#include <c2ddefs.h>
#include <c2cpriv.h>
#include <c2vdefs.h>
#include <c2vmcrs.h>
#include <m2cdefs.h>
#include <t2ddefs.h>
#include <t2cdefs.h>
#include <t2cpriv.h>
#ifdef CCDK_DEBUG
EXTERN INT DIS_LEVEL ;
EXTERN INT DISPLAY ;
#endif
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC void t2c_divide_edge __(( T2_EDGE, PARM, PARM, REAL, 
            INT*, REAL*, PT2*, PARM, T2_PARM )) ;
STATIC T2_EDGE t2c_fillet_vtx __(( T2_EDGE, T2_EDGE, REAL )) ;
STATIC BOOLEAN t2c_connect_edge __(( T2_EDGE, T2_LOOP, REAL, BOOLEAN, 
            REAL, PT2, PT2, T2_PARM, T2_PARM )) ;
STATIC BOOLEAN t2c_increment __(( T2_EDGE, PARM, PARM, REAL*, PT2, PARM )) ;

/*----------------------------------------------------------------------*/
BBS_PUBLIC REAL t2c_length_edge ( edge, parm0, parm1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM parm0, parm1 ;
{
    REAL length ;

    if ( parm0 == NULL && parm1 == NULL ) 
        length = c2c_length ( T2_EDGE_CURVE(edge) ) ;
    else {
        PARM_S parms0, parms1 ;
        COPY_PARM ( T2_EDGE_PARM0(edge), &parms0 ) ;
        COPY_PARM ( T2_EDGE_PARM1(edge), &parms1 ) ;
        t2c_trim_edge ( edge, parm0, parm1 ) ;
        length = c2c_length ( T2_EDGE_CURVE(edge) ) ;
        t2c_trim_edge ( edge, &parms0, &parms1 ) ;
    }
    RETURN ( length ) ;
}                


/*----------------------------------------------------------------------*/
BBS_PUBLIC REAL t2c_length_loop ( loop, parm0, parm1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_PARM parm0, parm1 ;
{
    DML_ITEM item ;
    REAL length = 0.0 ;
    BOOLEAN status ;
    PARM cparm0, cparm1 ;
    T2_EDGE edge ;

    status = ( parm0 == NULL ) ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = (T2_EDGE)DML_RECORD(item) ;
        if ( !status ) 
            status = ( T2_PARM_EDGE(parm0) == edge ) ;
        if ( status ) {
            cparm0 = ( parm0 != NULL && T2_PARM_EDGE(parm0) == edge ) ?
                T2_PARM_CPARM(parm0) : NULL ;
            cparm1 = ( parm1 != NULL && T2_PARM_EDGE(parm1) == edge ) ?
                T2_PARM_CPARM(parm1) : NULL ;
            length += t2c_length_edge ( edge, cparm0, cparm1 ) ;
        }
        if ( parm1 != NULL && T2_PARM_EDGE(parm1) == edge )  
            RETURN ( length ) ;
    }
    RETURN ( length ) ;
}                


/*----------------------------------------------------------------------*/
BBS_PUBLIC void t2c_divide_loop ( loop, n, parm0, parm1, pt, parm )
/*----------------------------------------------------------------------*/
/****       The first and the last points are included, so n >= 2    ****/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
INT n ;
T2_PARM parm0, parm1 ;
PT2* pt ;
T2_PARM parm ;
{
    REAL w, length, l ;
    BOOLEAN status ;
    DML_ITEM item ;
    T2_EDGE edge ;
    INT i ;
    PARM cparm0, cparm1 ;

    length = t2c_length_loop ( loop, parm0, parm1 ) ;
    l = length / ( (REAL)(n-1) ) ;
    status = TRUE ;

    if ( pt != NULL ) {
        if ( parm0 == NULL ) 
            t2c_loop_ept0 ( loop, pt[0] ) ;
        else
            t2c_eval_pt ( T2_PARM_EDGE(parm0), T2_PARM_CPARM(parm0), pt[0] ) ;
        if ( parm1 == NULL ) 
            t2c_loop_ept1 ( loop, pt[n-1] ) ;
        else
            t2c_eval_pt ( T2_PARM_EDGE(parm1), T2_PARM_CPARM(parm1), 
                pt[n-1] ) ;
    }
    if ( parm != NULL ) {
        if ( parm0 == NULL ) {
            T2_PARM_EDGE(parm) = t2c_first_edge ( loop ) ;
            PARM_COPY ( T2_EDGE_PARM0(T2_PARM_EDGE(parm)), 
                T2_PARM_CPARM(parm) ) ;
        }
        else {
            T2_PARM_EDGE(parm) = T2_PARM_EDGE(parm0) ;
            PARM_COPY ( T2_PARM_CPARM(parm0), T2_PARM_CPARM(parm) ) ;
        }
        if ( parm1 == NULL ) {
            T2_PARM_EDGE(parm+n-1) = t2c_last_edge ( loop ) ;
            PARM_COPY ( T2_EDGE_PARM1(T2_PARM_EDGE(parm+n-1)), 
                T2_PARM_CPARM(parm+n-1) ) ;
        }
        else {
            T2_PARM_EDGE(parm+n-1) = T2_PARM_EDGE(parm1) ;
            PARM_COPY ( T2_PARM_CPARM(parm1), T2_PARM_CPARM(parm+n-1) ) ;
        }
    }

    i = 1 ;
    w = l ;
    status = ( parm0 == NULL ) ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = (T2_EDGE)DML_RECORD(item) ;
        if ( !status ) 
            status = ( T2_PARM_EDGE(parm0) == edge ) ;
        if ( status ) {
            cparm0 = ( parm0 != NULL && T2_PARM_EDGE(parm0) == edge ) ?
                T2_PARM_CPARM(parm0) : NULL ;
            cparm1 = ( parm1 != NULL && T2_PARM_EDGE(parm1) == edge ) ?
                T2_PARM_CPARM(parm1) : NULL ;
            t2c_divide_edge ( edge, cparm0, cparm1, l, &i, &w, 
                pt, NULL, parm ) ;
        }
        if ( parm1 != NULL && T2_PARM_EDGE(parm1) == edge )  
            RETURN ;
    }
}                


/*----------------------------------------------------------------------*/
BBS_PUBLIC void t2c_divide ( edge, n, parm0, parm1, pt, parm )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
INT n ;
PARM parm0, parm1 ;
PT2* pt ;
PARM parm ;
{
    REAL l, length, w ;
    INT i ;

    if ( edge == NULL )
        RETURN ;
    length = t2c_length_edge ( edge, parm0, parm1 ) ;
    l = length / ( (REAL)(n-1) ) ;
    i = 1 ;
    w = l ;

    if ( pt != NULL ) {
        if ( parm0 == NULL )
            t2c_ept0 ( edge, pt[0] ) ;
        else
            t2c_eval_pt ( edge, parm0, pt[0] ) ;
    }
    if ( parm != NULL ) {
        if ( parm0 == NULL ) {
            PARM_COPY ( T2_EDGE_PARM0(edge), parm ) ;
        }
        else {
            PARM_COPY ( parm0, parm ) ;
        }
    }

    if ( IS_ZERO ( length ) ) {
        for ( i = 1 ; i < n ; i++ ) {
            if ( pt != NULL ) {
                C2V_COPY ( pt[0], pt[i] ) ;
            }
            if ( parm != NULL ) {
                PARM_COPY ( parm, parm+i ) ;
            }
        }
        RETURN ;
    }
    t2c_divide_edge ( edge, parm0, parm1, l, &i, &w, pt, parm, NULL ) ;
}


/*----------------------------------------------------------------------*/
STATIC void t2c_divide_edge ( edge, parm0, parm1, l, 
            i_ptr, w_ptr, pt, parm, tparm )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM parm0, parm1 ;
REAL l ;
INT *i_ptr ;
REAL *w_ptr ;
PT2* pt ;
PARM parm ;
T2_PARM tparm ;
{
    PARM_S parms0, parms1 ;

    if ( parm0 == NULL ) {
        PARM_COPY ( T2_EDGE_PARM0(edge), &parms0 ) ;
    }
    else {
        PARM_COPY ( parm0, &parms0 ) ;
    }
#ifdef OLD_CODE
    while ( m2c_increment ( T2_EDGE_CURVE(edge), T2_EDGE_DIR(edge), &parms0, 
        parm1, w_ptr, ( pt == NULL ) ? NULL : pt[*i_ptr], &parms1 ) ) {
#endif
    while ( t2c_increment ( edge, &parms0, parm1, w_ptr, 
        ( pt == NULL ) ? NULL : pt[*i_ptr], &parms1 ) ) {
        PARM_COPY ( &parms1, &parms0 ) ;
        if ( parm != NULL ) {
            PARM_COPY ( &parms1, (parm+(*i_ptr)) ) ;
        }
        if ( tparm != NULL ) {
            PARM_COPY ( &parms1, T2_PARM_CPARM(tparm+(*i_ptr)) ) ;
            T2_PARM_EDGE(tparm+(*i_ptr)) = edge ;
        }
        (*i_ptr)++ ;
        *w_ptr = l ;
        if ( parm1 == NULL ) {
            if ( IS_ZERO ( T2_EDGE_T1(edge) - PARM_T(&parms1) ) )
                RETURN ;
        }
        else {
            if ( IS_ZERO ( PARM_T(parm1) - PARM_T(&parms1) ) )
                RETURN ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT t2c_compare_cparms ( edge1, cparm1, edge2, cparm2 )
/* Returns parm1 < parm2 ? -1 : 1 */
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge1 ;
PARM cparm1 ;
T2_EDGE edge2 ;
PARM cparm2 ;
{
    T2_LOOP loop = T2_EDGE_LOOP(edge1) ;
    DML_ITEM item = NULL ;
    if ( edge1 == edge2 )
    {
        if ( IS_ZERO ( fabs ( PARM_T(cparm1) - PARM_T(cparm2) ) ) ) 
            RETURN ( 0 ) ;
        else if ( T2_EDGE_DIR(edge1) == 1 ) 
            RETURN ( PARM_T(cparm1) < PARM_T(cparm2) ? -1 : 1 ) ;
        else
            RETURN ( PARM_T(cparm1) > PARM_T(cparm2) ? -1 : 1 ) ;
    }
    else
    {
        DML_FOR_LOOP ( dml_find_data ( T2_LOOP_EDGE_LIST(loop), 
            ( ANY ) edge1 ), item )
        {
            if ( ( T2_EDGE ) dml_record ( item ) == edge2 ) 
                break ;
        }
        RETURN ( item != NULL ? -1 : 1 ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT t2c_compare_tparms ( tparm1, tparm2 )
/* Returns parm1 < parm2 ? -1 : 1 */
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_PARM tparm1 ;
T2_PARM tparm2 ;
{
    RETURN ( t2c_compare_cparms ( T2_PARM_EDGE(tparm1), T2_PARM_CPARM(tparm1), 
        T2_PARM_EDGE(tparm2), T2_PARM_CPARM(tparm2) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2c_set_cparm ( edge, t, cparm )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
REAL t ;
PARM cparm ;
{
    PARM_T(cparm) = t ;
    PARM_J(cparm) = 0 ;
    t2c_parm_adjust ( edge, cparm ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2c_set_tparm ( edge, t, tparm )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
REAL t ;
T2_PARM tparm ;
{
    t2c_set_cparm ( edge, t, T2_PARM_CPARM(tparm) ) ;
    T2_PARM_EDGE(tparm) = edge ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void t2c_parm_adjust ( edge, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM parm ;
{
    c2c_parm_adjust ( T2_EDGE_CURVE(edge), parm ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC void t2c_box_loops ( loops, box ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST loops ;
C2_BOX box ;
{
    T2_LOOP loop ;
    DML_ITEM item ;

    if ( dml_length(loops) == 0 )
        RETURN ;
    loop = (T2_LOOP)DML_FIRST_RECORD(loops) ;
    c2a_box_copy ( T2_LOOP_BOX(loop), box ) ;
    DML_FOR_LOOP ( DML_SECOND(loops), item ) {
        loop = (T2_LOOP)DML_RECORD(item) ;
        c2a_box_append ( T2_LOOP_BOX(loop), box ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_fillet_loop ( loop, rad )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL rad ;
{
    DML_ITEM item0, item1 ;
    BOOLEAN status ;
    T2_EDGE fillet ;

    status = TRUE ;

    for ( item0 = dml_first ( T2_LOOP_EDGE_LIST(loop) ) ; 
        item0 != NULL ; item0 = item1 ) {
        item1 = DML_NEXT(item0) ;
        if ( item1 == NULL ) {
            if ( T2_LOOP_CLOSED(loop) ) 
                item1 = dml_first ( T2_LOOP_EDGE_LIST(loop) ) ; 
            else
                RETURN ( status ) ;
        }
        fillet = t2c_fillet_vtx ( DML_RECORD(item0), DML_RECORD(item1), rad ) ;
        if ( fillet == NULL ) 
            status = FALSE ;
        else 
            dml_insert_after ( T2_LOOP_EDGE_LIST(loop), item0, fillet ) ;
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2c_fillet_vtx ( edge0, edge1, rad )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge0, edge1 ;
REAL rad ;
{
    PARM_S parm0, parm1 ;
    PT2 pt0, pt1, ctr, tan0 ;
    C2_CURVE fillet ;

    if ( !c2c_fillet_corner ( T2_EDGE_CURVE(edge0), T2_EDGE_CURVE(edge1), 
        rad, &parm0, &parm1, pt0, pt1, ctr ) )
        RETURN ( NULL ) ;
    t2c_eval_tan ( edge0, &parm0, tan0 ) ;
    if ( !t2c_trim_edge_verify ( edge0, NULL, &parm0 ) )
        RETURN ( NULL ) ;
    if ( !t2c_trim_edge_verify ( edge1, &parm1, NULL ) )
        RETURN ( NULL ) ;
    fillet = c2d_arc_2pts_tan ( pt0, tan0, pt1 ) ;
    RETURN ( t2d_create_edge ( T2_EDGE_LOOP(edge0), fillet, 1 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_extend_loop ( loop, dist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL dist ;
{
    RETURN ( t2c_extend_loop0 ( loop, dist ) &&
        t2c_extend_loop1 ( loop, dist ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_extend_loop0 ( loop, dist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL dist ;
{
    PT2 pt, tan ;
    C2_CURVE line ;
    T2_EDGE edge ;

    if ( T2_LOOP_CLOSED(loop) )
        RETURN ( FALSE ) ;
    t2c_loop_ept_tan0 ( loop, pt, tan ) ;
    c2v_normalize ( tan, tan ) ;
    C2V_SCALE ( tan, dist, tan ) ;
    line = c2d_line_dir ( pt, tan ) ;
    edge = t2d_create_edge ( loop, line, -1 ) ;
    t2d_append_edge ( loop, edge, TRUE ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_extend_loop1 ( loop, dist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL dist ;
{
    PT2 pt, tan ;
    C2_CURVE line ;
    T2_EDGE edge ;

    if ( T2_LOOP_CLOSED(loop) )
        RETURN ( FALSE ) ;
    t2c_loop_ept_tan1 ( loop, pt, tan ) ;
    c2v_normalize ( tan, tan ) ;
    C2V_SCALE ( tan, dist, tan ) ;
    line = c2d_line_dir ( pt, tan ) ;
    edge = t2d_create_edge ( loop, line, 1 ) ;
    t2d_append_edge ( loop, edge, FALSE ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_connect_2loops ( loop0, loop1, dist, on_vtcs, 
            pt0, pt1, parm0, parm1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0, loop1 ;
REAL dist ;
BOOLEAN on_vtcs ;
PT2 pt0, pt1 ;
T2_PARM parm0, parm1 ;
{
    T2_EDGE edge ;
    REAL h ;
    DML_ITEM item ;

    for ( h = 0.5 ; h >= 0.01 ; h *= 0.33 ) {
        DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop0), item ) {
            edge = (T2_EDGE)DML_RECORD(item) ;
            if ( t2c_connect_edge ( edge, loop1, dist, on_vtcs, h, 
                pt0, pt1, parm0, parm1 ) )
                RETURN ( TRUE ) ;
        }
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2c_connect_edge ( edge, loop, dist, on_vtcs, h, 
            pt0, pt1, parm0, parm1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
T2_LOOP loop ;
REAL dist ;
BOOLEAN on_vtcs ;
REAL h ;
PT2 pt0, pt1 ;
T2_PARM parm0, parm1 ;
{
    REAL step ;
    BOOLEAN end_edge ;
    PT2 p0, p1 ;
    PARM_S parm ;

    step = h * ( T2_EDGE_T1(edge) - T2_EDGE_T0(edge) ) ;
    PARM_COPY ( T2_EDGE_PARM0(edge), &parm ) ;

    end_edge = FALSE ;
    if ( !on_vtcs ) {
        PARM_T(&parm) += step ;
        t2c_parm_adjust ( edge, &parm ) ;
        end_edge = ( T2_EDGE_DIR(edge) == 1 ) ?
            ( PARM_T(&parm) > T2_EDGE_T1(edge) + BBS_ZERO - step ) :
            ( PARM_T(&parm) < T2_EDGE_T1(edge) - BBS_ZERO - step ) ;
    }

    while ( !end_edge ) {
        t2c_eval_pt ( edge, &parm, p0 ) ;
        if ( t2c_project_loop ( loop, p0, on_vtcs, parm1, p1 )
            && ( C2V_DIST ( p0, p1 ) <= dist ) ) {
            if ( parm0 != NULL ) {
                T2_PARM_EDGE(parm0) = edge ;
                PARM_COPY ( &parm, T2_PARM_CPARM(parm0) ) ;
            }
            if ( pt0 != NULL ) {
                C2V_COPY ( p0, pt0 ) ;
            }
            if ( pt1 != NULL ) {
                C2V_COPY ( p1, pt1 ) ;
            }
            RETURN ( TRUE ) ;
        }
        PARM_T(&parm) += step ;
        t2c_parm_adjust ( edge, &parm ) ;
        if ( on_vtcs ) 
            end_edge = ( T2_EDGE_DIR(edge) == 1 ) ?
                ( PARM_T(&parm) > T2_EDGE_T1(edge) + BBS_ZERO ) :
                ( PARM_T(&parm) < T2_EDGE_T1(edge) - BBS_ZERO ) ;
        else
            end_edge = ( T2_EDGE_DIR(edge) == 1 ) ?
                ( PARM_T(&parm) > T2_EDGE_T1(edge) + BBS_ZERO - step ) :
                ( PARM_T(&parm) < T2_EDGE_T1(edge) - BBS_ZERO - step ) ;
    }
    RETURN ( FALSE ) ;
}

#ifdef NEW_CODE
/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2c_approx_chain ( loop, parm0, parm1, acc, parm_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_PARM parm0, parm1 ;
REAL acc ;
DML_LIST parm_list ;
{
    BOOLEAN inside ;
    PARM p0, p1 ;
    DML_ITEM item ;
    T2_EDGE edge ;

    if ( parm_list == NULL ) 
        parm_list = dml_create_list () ;

    inside = ( parm0 == NULL ) ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        if ( !inside ) 
            inside = ( edge == T2_PARM_EDGE(parm0) ) ;
        if ( inside ) {
            p0 = ( ( parm0 == NULL ) || ( edge != T2_PARM_EDGE(parm0) ) ) ?
                NULL : T2_PARM_CPARM(parm0) ;
            p1 = ( ( parm1 == NULL ) || ( edge != T2_PARM_EDGE(parm1) ) ) ?
                NULL : T2_PARM_CPARM(parm1) ;
            t2c_approx_edge ( edge, p0, p1, acc, parm_list ) ;
        }
        if ( p1 != NULL )
            RETURN ( parm_list ) ;
    }
    RETURN ( parm_list ) ;
}
#endif

/*----------------------------------------------------------------------*/
BBS_PUBLIC INT t2c_approx_n ( edge, parm0, parm1, acc )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM parm0, parm1 ;
REAL acc ;
{
    if ( edge == NULL )
        RETURN ( 0 ) ;
    else 
        RETURN ( ( T2_EDGE_DIR(edge) == 1 ) ? 
        m2c_approx_n ( T2_EDGE_CURVE(edge), parm0, parm1, acc ) :
        m2c_approx_n ( T2_EDGE_CURVE(edge), parm1, parm0, acc ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_along ( edge, parm0, parm1, l, dir, parm )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM parm0, parm1 ;
REAL l ;
INT dir ;
PARM parm ;
{
    RETURN ( ( T2_EDGE_DIR(edge) == 1 ) ? 
        m2c_along ( T2_EDGE_CURVE(edge), parm0, parm1, l, dir, parm ) :
        m2c_along ( T2_EDGE_CURVE(edge), parm0, parm1, l, -dir, parm ) ) ; 
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_along_loop ( loop, tparm0, tparm1, w, dir, tparm )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_PARM tparm0, tparm1 ;
REAL w ;
INT dir ;
T2_PARM tparm ;
{
    DML_ITEM item ;
    T2_EDGE edge ;
    PARM cparm0, cparm1 ;
    REAL l ;

    item = dml_find_data ( T2_LOOP_EDGE_LIST(loop), T2_PARM_EDGE(tparm0) ) ;

    while ( item != NULL ) {
        edge = DML_RECORD(item) ;
        cparm0 = ( edge == T2_PARM_EDGE(tparm0) ) ? 
            T2_PARM_CPARM(tparm0) : NULL ;
        cparm1 = ( edge == T2_PARM_EDGE(tparm1) ) ? 
            T2_PARM_CPARM(tparm1) : NULL ;
        l = ( dir == 1 ) ? t2c_length ( edge, cparm0, cparm1 ) :
            t2c_length ( edge, cparm1, cparm0 ) ;
        if ( w < l ) {
            t2c_along ( edge, cparm0, cparm1, w, dir, T2_PARM_CPARM(tparm) ) ;
            T2_PARM_EDGE(tparm) = edge ;
            RETURN ( TRUE ) ;
        }
        item = ( dir == 1 ) ? DML_NEXT(item) : DML_PREV(item) ;
        w -= l ;
    }
    RETURN ( FALSE ) ;
}


/*----------------------------------------------------------------------*/
STATIC BOOLEAN t2c_increment ( edge, parm0, parm1, w_ptr, pt, parm )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM parm0, parm1 ;
REAL *w_ptr ;
PT2 pt ;
PARM parm ;
{
    REAL l ;

    l = *w_ptr ;

    if ( t2c_along ( edge, parm0, parm1, l, 1, parm ) ) {
        if ( pt != NULL ) 
            t2c_eval_pt ( edge, parm, pt ) ;
        RETURN ( TRUE ) ;
    }
    else {
        *w_ptr -= t2c_length ( edge, parm0, parm1 ) ;
        if ( parm1 == NULL ) {
            PARM_COPY ( T2_EDGE_PARM1(edge), parm ) ;
        }
        else {
            PARM_COPY ( parm1, parm ) ;
        }
        RETURN ( FALSE ) ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_loop_is_circle ( loop, tol, ctr, rad_ptr )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL tol ;
PT2 ctr ;
REAL *rad_ptr ;
{
    BOOLEAN status ;
    DML_ITEM item ;
    T2_EDGE edge ;
    PT2 ctr1 ;

    status = FALSE ;
    if ( !T2_LOOP_CLOSED(loop) )
        RETURN ( FALSE ) ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item )
    {
        edge = DML_RECORD(item) ;
        if ( !T2_EDGE_IS_ARC(edge) )
            RETURN ( FALSE ) ;
        if ( status ) {
            if ( !c2c_get_arc_center ( T2_EDGE_CURVE(edge), ctr1 ) )
                RETURN ( FALSE ) ;
            else if ( C2V_DISTL1 ( ctr, ctr1 ) > tol )
                RETURN ( FALSE ) ;
        }
        else if ( !c2c_get_arc_data ( T2_EDGE_CURVE(edge), ctr, rad_ptr, 
            NULL, NULL, NULL ) )
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

