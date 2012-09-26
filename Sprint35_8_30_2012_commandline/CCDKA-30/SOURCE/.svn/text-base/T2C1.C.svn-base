/* -R -Z -S -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/******************************* T2C1.C *********************************/ 
/**************** Two-dimensional topology ******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h> 
#include <fndefs.h>
#include <c2adefs.h> 
#include <c2cdefs.h> 
#include <c2ddefs.h> 
#include <c2vdefs.h> 
#include <c2vmcrs.h> 
#include <m2cdefs.h>
#include <t2cdefs.h>
#include <t2cpriv.h>
#include <t2ddefs.h>
#include <t2attrd.h>
#include <t2xrecm.h>
#ifdef CCDK_DEBUG
EXTERN INT DIS_LEVEL ;
EXTERN INT DISPLAY ;
#endif
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC BOOLEAN t2c_sort_key1 __(( FN_EXTR_REC, FN_EXTR_REC )) ; 
STATIC DML_ITEM t2c_order_next __(( DML_LIST, PT2, PF_T2C_EVAL )) ;
STATIC BOOLEAN t2c_project_loop_test __(( T2_EDGE, PT2, BOOLEAN, T2_PARM, PT2, 
            PARM, REAL*, REAL*, BOOLEAN )) ;
STATIC BOOLEAN t2c_project_test_edge __(( T2_EDGE, PT2, T2_PARM, PT2, 
            PARM, REAL*, REAL*, BOOLEAN )) ;
STATIC BOOLEAN t2c_project_test_vtx __(( T2_EDGE, PT2, T2_PARM, PT2, 
            REAL*, REAL*, BOOLEAN )) ;
STATIC BOOLEAN t2c_project_test_vtx1 __(( T2_EDGE, PARM, PT2, T2_PARM, PT2, 
            REAL*, REAL*, BOOLEAN )) ;

/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_curvature ( edge, parm, curv_ptr )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                       !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

T2_EDGE edge ;
PARM parm ;
REAL *curv_ptr ;
{
    RETURN ( c2c_curvature ( T2_EDGE_CURVE(edge), parm, curv_ptr ) );
}            


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_project ( edge, pt, parm, on_pt )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 pt ;
PARM parm ;
PT2 on_pt ;
{
    RETURN ( c2c_project ( T2_EDGE_CURVE(edge), pt, parm, on_pt ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_project_loop ( loop, pt, on_vtcs, parm, on_pt )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt ;
BOOLEAN on_vtcs ;
T2_PARM parm ;
PT2 on_pt ;
{
    BOOLEAN valid_dist_min=FALSE ;
    REAL dist_min=0.0, dot_min=0.0 ;
    PT2 p1 ;
    PARM_S cparm ;
    DML_ITEM item ;
    T2_EDGE edge ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        valid_dist_min = t2c_project_loop_test ( edge, pt, on_vtcs, 
            parm, p1, &cparm, &dist_min, &dot_min, valid_dist_min ) ;
    }
    if ( valid_dist_min && on_pt != NULL ) {
        C2V_COPY ( p1, on_pt ) ;
    }
    RETURN ( valid_dist_min ) ;
}


/*----------------------------------------------------------------------*/
STATIC BOOLEAN t2c_project_loop_test ( edge, pt, on_vtcs, 
            parm, on_pt, cparm, dist_min, dot_min, valid_dist_min )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 pt ;
BOOLEAN on_vtcs ;
T2_PARM parm ;
PT2 on_pt ;
PARM cparm ;
REAL *dist_min, *dot_min ;
BOOLEAN valid_dist_min ;
{
    valid_dist_min = t2c_project_test_edge ( edge, pt, parm, on_pt, cparm, 
        dist_min, dot_min, valid_dist_min ) ;
    if ( on_vtcs ) 
        valid_dist_min = t2c_project_test_vtx ( edge, pt, parm, on_pt,  
            dist_min, dot_min, valid_dist_min ) ;
    RETURN ( valid_dist_min ) ;
}


/*----------------------------------------------------------------------*/
STATIC BOOLEAN t2c_project_test_edge ( edge, pt, tparm, on_pt, 
            cparm, dist_min, dot_min, valid_dist_min )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 pt ;
T2_PARM tparm ;
PT2 on_pt ;
PARM cparm ;
REAL *dist_min, *dot_min ;
BOOLEAN valid_dist_min ;
{
    REAL dist, dot ;
    PT2 p, tan, vec ;

    if ( edge != NULL && t2c_project ( edge, pt, cparm, p ) ) {
        dist = C2V_DIST ( p, pt ) ;
        if ( !valid_dist_min || ( dist <= *dist_min + BBS_TOL ) ) {
            t2c_eval_tan ( edge, cparm, tan ) ;
            c2v_normalize ( tan, tan ) ;
            C2V_SUB ( pt, p, vec ) ;
            dot = C2V_DOT ( vec, tan ) ;
            if ( !valid_dist_min || ( dist < *dist_min - BBS_TOL ) || 
                ( fabs(dot) < fabs(*dot_min) ) ) {
                *dist_min = dist ;
                *dot_min = dot ;
                if ( tparm != NULL ) {
                    PARM_COPY ( cparm, T2_PARM_CPARM(tparm) ) ;
                    T2_PARM_EDGE(tparm) = edge ;
                }
                C2V_COPY ( p, on_pt ) ;
                valid_dist_min = TRUE ;
            }
        }
    }

    RETURN ( valid_dist_min ) ;
}


/*----------------------------------------------------------------------*/
STATIC BOOLEAN t2c_project_test_vtx ( edge, pt, tparm, on_pt, 
            dist_min, dot_min, valid_dist_min )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 pt ;
T2_PARM tparm ;
PT2 on_pt ;
REAL *dist_min, *dot_min ;
BOOLEAN valid_dist_min ;
{
    if ( edge != NULL ) {
        valid_dist_min = t2c_project_test_vtx1 ( edge, T2_EDGE_PARM0(edge), 
            pt, tparm, on_pt, dist_min, dot_min, valid_dist_min ) ;
        valid_dist_min = t2c_project_test_vtx1 ( edge, T2_EDGE_PARM1(edge), 
            pt, tparm, on_pt, dist_min, dot_min, valid_dist_min ) ;
    }
    RETURN ( valid_dist_min ) ;
}


/*----------------------------------------------------------------------*/
STATIC BOOLEAN t2c_project_test_vtx1 ( edge, edge_parm, pt, tparm, on_pt, 
            dist_min, dot_min, valid_dist_min )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM edge_parm ;
PT2 pt ;
T2_PARM tparm ;
PT2 on_pt ;
REAL *dist_min, *dot_min ;
BOOLEAN valid_dist_min ;
{
    REAL dist, dot ;
    PT2 p, tan, vec ;

    t2c_eval_pt_tan ( edge, edge_parm, p, tan ) ;
    dist = C2V_DIST ( p, pt ) ;
    if ( !valid_dist_min || ( dist <= *dist_min + BBS_TOL ) ) {
        c2v_normalize ( tan, tan ) ;
        C2V_SUB ( pt, p, vec ) ;
        dot = C2V_DOT ( vec, tan ) ;
        if ( !valid_dist_min || ( dist < *dist_min - BBS_TOL ) || 
            ( fabs(dot) < fabs(*dot_min) ) ) {
            *dist_min = dist ;
            *dot_min = dot ;
            if ( tparm != NULL ) {
                PARM_COPY ( edge_parm, T2_PARM_CPARM(tparm) ) ;
                T2_PARM_EDGE(tparm) = edge ;
            }
            C2V_COPY ( p, on_pt ) ;
            valid_dist_min = TRUE ;
        }
    }
    RETURN ( valid_dist_min ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_project_chain ( loop, parm0, parm1, pt, on_vtcs, 
            parm, on_pt )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_PARM parm0, parm1 ;
PT2 pt ;
BOOLEAN on_vtcs ;
T2_PARM parm ;
PT2 on_pt ;
{
    BOOLEAN end_chain, new_edge, valid_dist_min=FALSE ;
    REAL dist_min=0.0, dot_min=0.0 ;
    PT2 p1 ;
    PARM_S cparm ;
    DML_ITEM item, item0, item1 ;
    T2_EDGE edge ;

    dml_find_data2 ( T2_LOOP_EDGE_LIST(loop), T2_PARM_EDGE(parm0), 
        T2_PARM_EDGE(parm1), &item0, &item1 ) ;

    end_chain = FALSE ;
    if ( item0 == NULL ) 
        item0 = DML_FIRST(T2_LOOP_EDGE_LIST(loop)) ; 
    if ( item1 == NULL ) 
        item1 = DML_LAST(T2_LOOP_EDGE_LIST(loop)) ; 

    for ( item = item0 ; !end_chain ; 
        item = dml_cycl_next ( item, T2_LOOP_EDGE_LIST(loop) ) ) {

        new_edge = ( edge == T2_PARM_EDGE(parm0) ) || 
            ( edge == T2_PARM_EDGE(parm1) ) ;
        edge = DML_RECORD(item) ;
        if ( edge == T2_PARM_EDGE(parm1) ) {
            if ( edge == T2_PARM_EDGE(parm0) ) 
                edge = t2d_trim_edge ( edge, T2_PARM_CPARM(parm0), 
                    T2_PARM_CPARM(parm1) ) ;
            else
                edge = t2d_trim_edge ( edge, NULL, T2_PARM_CPARM(parm1) ) ;
            new_edge = TRUE ;
            end_chain = TRUE ;
        }
        else if ( edge == T2_PARM_EDGE(parm1) ) {
            edge = t2d_trim_edge ( edge, T2_PARM_CPARM(parm0), NULL ) ;
            new_edge = TRUE ;
        }

        valid_dist_min = t2c_project_test_edge ( edge, pt, parm, p1, 
            &cparm, &dist_min, &dot_min, valid_dist_min ) ;
        if ( on_vtcs ) 
            valid_dist_min = t2c_project_test_vtx ( edge, pt, parm, p1, 
                &dist_min, &dot_min, valid_dist_min ) ;
        if ( new_edge ) {
            T2_PARM_EDGE(parm) = DML_RECORD(item) ;
            t2d_free_edge ( edge ) ;
        }
    }
    if ( valid_dist_min && on_pt != NULL ) {
        C2V_COPY ( p1, on_pt ) ;
    }
    RETURN ( valid_dist_min ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_project_attr ( edge, pt, attr, parm, on_pt )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 pt ;
ATTR attr ;
PARM parm ;
PT2 on_pt ;
{
    if ( T2_EDGE_ATTR(edge) & attr )
        RETURN ( c2c_project ( T2_EDGE_CURVE(edge), pt, parm, on_pt ) ) ;
    else
        RETURN ( FALSE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2c_project_loop_attr ( loop, pt, attr, on_vtcs, 
            parm, on_pt )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt ;
ATTR attr ;
BOOLEAN on_vtcs ;
T2_PARM parm ;
PT2 on_pt ;
{
    BOOLEAN valid_dist_min=FALSE ;
    REAL dist_min=0.0, dot_min=0.0 ;
    PT2 p1 ;
    PARM_S cparm ;
    DML_ITEM item ;
    T2_EDGE edge ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        if ( T2_EDGE_ATTR(edge) & attr ) {
            valid_dist_min = t2c_project_loop_test ( edge, pt, on_vtcs, 
                parm, p1, &cparm, &dist_min, &dot_min, valid_dist_min ) ;
        }
    }
    if ( valid_dist_min && on_pt != NULL ) {
        C2V_COPY ( p1, on_pt ) ;
    }
    RETURN ( valid_dist_min ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC REAL t2c_dist_pt_loop ( loop, pt )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt ;
{
    PT2 on_pt ;

    if ( loop == NULL || !t2c_project_loop ( loop, pt, TRUE, NULL, on_pt ) )
        RETURN ( -1.0 ) ;
    else
        RETURN ( C2V_DIST ( on_pt, pt ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2c_project_region ( region, pt, on_vtcs, parm, on_pt )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
PT2 pt ;
BOOLEAN on_vtcs ;
T2_PARM parm ;
PT2 on_pt ;
{
    RETURN ( dml_record ( t2c_project_looplist 
        ( T2_REGION_LOOP_LIST(region), pt, on_vtcs, parm, on_pt ) ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE DML_ITEM t2c_project_looplist ( looplist, pt, 
        on_vtcs, parm, on_pt )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
PT2 pt ;
BOOLEAN on_vtcs ;
T2_PARM parm ;
PT2 on_pt ;
{
    REAL dist, dist_min=0.0 ;
    PT2 p ;
    T2_PARM_S tparm ;
    DML_ITEM item, item_min=NULL ;
    T2_LOOP loop ;

    DML_WALK_LIST ( looplist, item ) {
        loop = DML_RECORD(item) ;
        if ( t2c_project_loop ( loop, pt, on_vtcs, &tparm, p ) ) {
            dist = C2V_DIST ( p, pt ) ;
            if ( item_min == NULL || dist < dist_min ) {
                dist_min = dist ;
                item_min = item ;
                if ( parm != NULL ) {
                    PARM_COPY ( T2_PARM_CPARM(&tparm), T2_PARM_CPARM(parm) ) ;
                    T2_PARM_EDGE(parm) = T2_PARM_EDGE(&tparm) ;
                }
                if ( on_pt != NULL ) 
                    C2V_COPY ( p, on_pt ) ;
            }
        }
    }
    RETURN ( item_min ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE REAL t2c_edge_area ( edge, origin ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 origin ;
{
    RETURN ( (T2_EDGE_DIR(edge)==1) ? m2c_area ( T2_EDGE_CURVE(edge), origin ) 
        : - m2c_area ( T2_EDGE_CURVE(edge), origin ) ) ;
}                


/*----------------------------------------------------------------------*/
BBS_PUBLIC REAL t2c_loop_area ( loop ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    DML_ITEM item ;
    REAL area = 0.0 ;
    PT2 origin ;

    c2a_box_get_ctr ( T2_LOOP_BOX(loop), origin ) ;
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) 
        area += t2c_edge_area ( DML_RECORD(item), origin ) ;
    RETURN ( area ) ;
}                


/*----------------------------------------------------------------------*/
BBS_PRIVATE REAL t2c_looplist_area ( looplist ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
{
    DML_ITEM item ;
    REAL area = 0.0 ;
    DML_WALK_LIST ( looplist, item ) 
        area += t2c_loop_area ( DML_RECORD(item) ) ;
    RETURN ( area ) ;
}                


/*----------------------------------------------------------------------*/
BBS_PUBLIC REAL t2c_region_area ( region ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
{
    RETURN ( t2c_looplist_area ( T2_REGION_LOOP_LIST(region) ) ) ;
}                


/*----------------------------------------------------------------------*/
BBS_PRIVATE void t2c_break_region_at_extrs ( region, coord )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
INT coord ;
{
    DML_ITEM item ;
    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item )
        t2c_break_loop_at_extrs ( DML_RECORD(item), coord ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void t2c_break_loop_at_extrs ( loop, coord )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
INT coord ;
{
    t2c_break_loop_at_extrs_attr ( loop, coord, T2_ATTR_INIT ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void t2c_break_loop_at_tangents ( loop, angle, attr )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL angle ;
ATTR attr ;
{
    PT2 origin ;

    c2a_box_get_ctr ( T2_LOOP_BOX(loop), origin ) ;
    t2c_rotate_loop ( loop, origin, -angle ) ;
    t2c_break_loop_at_extrs_attr ( loop, 1, attr ) ;
    t2c_rotate_loop ( loop, origin, angle ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2c_break_edge_at_extrs ( edge, coord )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
INT coord ;
{
    DML_ITEM item, item1 ;
    T2_EDGE edge1 ;
    DML_LIST list = dml_create_list(), edgelist = NULL;
    INT n ;
    PARM parm0 = NULL, parm1 ;
    FN_EXTR_REC extr_rec ;

    n = c2c_coord_extrs ( T2_EDGE_CURVE(edge), coord, list ) ;
    if ( n > 0 )
    {
        for ( item = dml_first ( list ) ;
            item != NULL ;
            item = item1 )
        {
            item1 = dml_next ( item ) ;
            extr_rec = ( FN_EXTR_REC ) DML_RECORD ( item ) ;
            if ( IS_SMALL ( FN_EXTR_REC_T(extr_rec) - T2_EDGE_T0(edge) ) ||
                IS_SMALL ( FN_EXTR_REC_T(extr_rec) - T2_EDGE_T1(edge) ) ) {
                FREE ( extr_rec ) ;
                extr_rec = NULL ;
                dml_remove_item ( list, item ) ;
                item = NULL ;
                n-- ;
            }
        }
    }

    if ( n > 0 )
    {
        dml_sort_list ( list, ( PF_SORT ) t2c_sort_key1 ) ;

        edgelist = dml_create_list() ;

        DML_WALK_LIST ( list, item )
        {
            extr_rec = ( FN_EXTR_REC ) dml_record ( item ) ;
            parm1 = FN_EXTR_REC_PARM(extr_rec) ;
            edge1 = t2d_trim_edge ( edge, parm0, parm1 ) ;
            if ( edge1 != NULL ) 
                dml_append_data ( edgelist, edge1 ) ;
            parm0 = parm1 ;
        }

        parm1 = NULL ;
        edge1 = t2d_trim_edge ( edge, parm0, parm1 ) ;
        if ( edge1 != NULL ) 
            dml_append_data ( edgelist, edge1 ) ;

        DML_WALK_LIST ( list, item ) {
            extr_rec = ( FN_EXTR_REC ) DML_RECORD ( item ) ;
            FREE ( extr_rec ) ;
            extr_rec = NULL ;
        }
    }

    dml_free_list ( list ) ;
    list = NULL ;
    RETURN ( edgelist ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void t2c_break_loop_at_extrs_attr ( loop, coord, attr )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
INT coord ;
ATTR attr ;
{
    DML_ITEM item, item1, item2 ;
    T2_EDGE edge, edge0 ;
    DML_LIST edgelist ;

    for ( item = dml_first ( T2_LOOP_EDGE_LIST(loop) ) ;
        item != NULL ; item = item1 ) {
        item1 = dml_next ( item ) ;
        edge0 = dml_record ( item ) ;
        if ( attr == T2_ATTR_INIT || ( T2_EDGE_ATTR(edge0) & attr ) ) {
            edgelist = t2c_break_edge_at_extrs ( edge0, coord ) ;
            if ( edgelist != NULL ) {
                DML_WALK_LIST ( edgelist, item2 ) {
                    edge = dml_record ( item2 ) ;
                    dml_insert_prior ( T2_LOOP_EDGE_LIST(loop), item, edge ) ;
                }
                dml_remove_item ( T2_LOOP_EDGE_LIST(loop), item ) ;
                item = NULL ;
                t2d_free_edge ( edge0 ) ;
                edge0 = NULL ;
                dml_free_list ( edgelist ) ;
                edgelist = NULL ;
            }
        }
    }
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE INT t2c_loop_vtx_extrs ( loop, coord, list )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
INT coord ;
DML_LIST list ;
{
    PT2 p, p1 ;
    INT dir, dir1 ;
    T2_EDGE edge, edge1 ;
    DML_ITEM item ;
    T2_EXTR_REC extr_rec ;

    item = dml_first ( T2_LOOP_EDGE_LIST(loop) ) ;
    edge = dml_record ( item ) ;
    t2c_ept0 ( edge, p ) ;
    t2c_ept1 ( edge, p1 ) ;
    if ( p[coord] < p1[coord] )
        dir = 1 ;
    else
        dir = -1 ;
    edge1 = edge ;
    DML_FOR_LOOP ( dml_next ( item ), item ) 
    {
        edge = dml_record ( item ) ;
        C2V_COPY ( p1, p ) ;
        t2c_ept1 ( edge, p1 ) ;
        if ( p[coord] < p1[coord] )
            dir1 = 1 ;
        else
            dir1 = -1 ;
        if ( dir != dir1 )
        {
            extr_rec = T2_ALLOC_EXTR_REC ;
            T2_EXTR_REC_EDGE(extr_rec) = edge1 ;
            T2_EXTR_REC_POSITION(extr_rec) = 1 ;
            T2_EXTR_REC_T(extr_rec) = T2_EDGE_T1(edge1) ;
            if ( dir == 1 )
                T2_EXTR_REC_TYPE(extr_rec) = 1 ;
            else
                T2_EXTR_REC_TYPE(extr_rec) = - 1 ;

            dml_append_data ( list, ( ANY ) extr_rec ) ;
            dir = dir1 ;
        }
        edge1 = edge ;
    }
    RETURN ( dml_length ( list ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void t2c_loop_extr_vtcs ( loop, coord, p_min, p_max )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
INT coord ;
PT2 p_min, p_max ;
{
    DML_ITEM item ;
    T2_EDGE edge ;
    PT2 p ;

    if ( ( loop == NULL ) || ( T2_LOOP_EDGE_LIST(loop) == NULL ) )
        RETURN ;
    t2c_loop_ept0 ( loop, p ) ;
    if ( p_min != NULL ) 
        C2V_COPY ( p, p_min ) ;
    if ( p_max != NULL ) 
        C2V_COPY ( p, p_max ) ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        t2c_ept0 ( edge, p ) ;
        if ( ( p_min != NULL ) && ( p[coord] < p_min[coord] ) )
            C2V_COPY ( p, p_min ) ;
        if ( ( p_max != NULL ) && ( p[coord] > p_max[coord] ) )
            C2V_COPY ( p, p_max ) ;
    }
}


/*----------------------------------------------------------------------*/
STATIC BOOLEAN t2c_sort_key1 ( extr_rec1, extr_rec2 )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FN_EXTR_REC extr_rec1, extr_rec2 ;
{
    RETURN ( FN_EXTR_REC_T(extr_rec1) < FN_EXTR_REC_T(extr_rec2) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC void t2c_trim_loop ( loop, edge0, parm0, edge1, parm1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_EDGE edge0, edge1 ;
PARM parm0, parm1 ;
{
    T2_PARM_S parm ;
    if ( T2_LOOP_CLOSED(loop) ) {
        T2_PARM_EDGE(&parm) = edge0 ;
        PARM_COPY ( T2_EDGE_PARM0(edge0), T2_PARM_CPARM(&parm) ) ; 
        t2c_loop_rearrange_parm ( loop, &parm ) ;
    }
    t2d_trim_loop ( loop, edge0, parm0, edge1, parm1 ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void t2c_replace_by_line ( loop, edge0, parm0, edge1, parm1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_EDGE edge0, edge1 ;
PARM parm0, parm1 ;
{
    DML_ITEM item, item0, item1, item2 ;
    BOOLEAN dir ;
    PT2 p0, p1 ;
    T2_EDGE edge ;

    item0 = NULL ;
    item1 = NULL ;
    t2c_eval_pt ( edge0, parm0, p0 ) ;
    t2c_eval_pt ( edge1, parm1, p1 ) ;
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        if ( edge == edge0 ) {
            item0 = item ;
            if ( item1 == NULL ) 
                dir = TRUE ;
            else {
                dir = FALSE ;
                break ;
            }
        }
        else if ( edge == edge1 ) {
            item1 = item ;
            if ( item0 != NULL ) 
                break ;
        }
    }
    if ( edge0 == edge1 ) {
        if ( ( T2_EDGE_DIR(edge0) == 1 ) == 
             ( PARM_T(parm0) < PARM_T(parm1) ) ) {
            edge = t2d_trim_edge ( edge0, parm1, NULL ) ;
            T2_EDGE_LOOP(edge) = loop ;
            t2c_trim_edge ( edge0, NULL, parm0 ) ;
            if ( edge != NULL ) 
                dml_insert_after ( T2_LOOP_EDGE_LIST(loop), item0, edge ) ;
            if ( !C2V_IDENT_PTS(p0,p1) ) {
                edge = t2d_create_edge ( loop, c2d_line ( p0, p1 ), 1 ) ;
                T2_EDGE_ATTR(edge) = 
                    T2_EDGE_ATTR(edge) | T2_EDGE_ATTR(edge0) ;
                dml_insert_after ( T2_LOOP_EDGE_LIST(loop), item0, edge ) ;
            }
            if ( IS_ZERO(T2_EDGE_T1(edge0)-T2_EDGE_T0(edge0)) ) {
                t2d_free_edge ( edge0 ) ;
                dml_remove_item ( T2_LOOP_EDGE_LIST(loop), item0 ) ; 
            }
        }
        else {
            for ( item = DML_FIRST(T2_LOOP_EDGE_LIST(loop)) ; 
                item != NULL ; item = item1 ) {
                item1 = DML_NEXT(item) ;
                if ( item != item0 ) {
                    t2d_free_edge ( DML_RECORD(item) ) ;
                    dml_remove_item ( T2_LOOP_EDGE_LIST(loop), item ) ; 
                }
            }
            t2c_trim_edge ( edge0, parm1, parm0 ) ;
            if ( !C2V_IDENT_PTS(p0,p1) ) {
                edge = t2d_create_edge ( loop, c2d_line ( p0, p1 ), 1 ) ;
                T2_EDGE_ATTR(edge) = 
                    T2_EDGE_ATTR(edge) | T2_EDGE_ATTR(edge0) ;
                dml_insert_after ( T2_LOOP_EDGE_LIST(loop), item0, edge ) ;
            }
        }
    }

    else if ( dir ) {
        t2c_trim_edge ( edge0, NULL, parm0 ) ;
        for ( item = DML_NEXT(item0) ; item != NULL && item != item1 ; 
            item = item2 ) {
            item2 = DML_NEXT(item) ;
            t2d_free_edge ( DML_RECORD(item) ) ;
            dml_remove_item ( T2_LOOP_EDGE_LIST(loop), item ) ; 
        }
        t2c_trim_edge ( edge1, parm1, NULL ) ;
        if ( !C2V_IDENT_PTS(p0,p1) ) {
            edge = t2d_create_edge ( loop, c2d_line ( p0, p1 ), 1 ) ;
            T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_EDGE_ATTR(edge0) ;
            dml_insert_after ( T2_LOOP_EDGE_LIST(loop), item0, edge ) ;
        }
        if ( IS_ZERO(T2_EDGE_T1(edge0)-T2_EDGE_T0(edge0)) ) {
            t2d_free_edge ( edge0 ) ;
            dml_remove_item ( T2_LOOP_EDGE_LIST(loop), item0 ) ; 
        }
        if ( IS_ZERO(T2_EDGE_T1(edge1)-T2_EDGE_T0(edge1)) ) {
            t2d_free_edge ( edge1 ) ;
            dml_remove_item ( T2_LOOP_EDGE_LIST(loop), item1 ) ; 
        }
    }

    else {
        for ( item = DML_FIRST(T2_LOOP_EDGE_LIST(loop)) ; 
            item != item1 ; item = item2 ) {
            item2 = DML_NEXT(item) ;
            t2d_free_edge ( DML_RECORD(item) ) ;
            dml_remove_item ( T2_LOOP_EDGE_LIST(loop), item ) ; 
        }
        for ( item = DML_NEXT(item0) ; item != NULL ; item = item2 ) {
            item2 = DML_NEXT(item) ;
            t2d_free_edge ( DML_RECORD(item) ) ;
            dml_remove_item ( T2_LOOP_EDGE_LIST(loop), item ) ; 
        }
        t2c_trim_edge ( edge1, parm1, NULL ) ;
        if ( !C2V_IDENT_PTS(p0,p1) ) {
            edge = t2d_create_edge ( loop, c2d_line ( p0, p1 ), 1 ) ;
            T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_EDGE_ATTR(edge0) ;
            dml_insert_after ( T2_LOOP_EDGE_LIST(loop), item0, edge ) ;
        }
        t2c_trim_edge ( edge0, NULL, parm0 ) ;
        if ( IS_ZERO(T2_EDGE_T1(edge0)-T2_EDGE_T0(edge0)) ) {
            t2d_free_edge ( edge0 ) ;
            dml_remove_item ( T2_LOOP_EDGE_LIST(loop), item0 ) ; 
        }
        if ( IS_ZERO(T2_EDGE_T1(edge1)-T2_EDGE_T0(edge1)) ) {
            t2d_free_edge ( edge1 ) ;
            dml_remove_item ( T2_LOOP_EDGE_LIST(loop), item1 ) ; 
        }
    }
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void t2c_order_looplist ( looplist, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
PT2 pt ;
{
    t2c_order_list ( looplist, pt, t2c_loop_ept0, t2c_loop_ept1 ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void t2c_order_edgelist ( edgelist, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST edgelist ;
PT2 pt ;
{
    t2c_order_list ( edgelist, pt, t2c_ept0, t2c_ept1 ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void t2c_order_list ( list, pt, eval_pt0, eval_pt1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
PT2 pt ;
PF_T2C_EVAL eval_pt0, eval_pt1 ;
{
    DML_LIST templist ;
    DML_ITEM item ;
    PT2 p ;

    templist = dml_create_list () ;
    for ( item = t2c_order_next ( list, pt, eval_pt0 ) ;
          item != NULL ;
          item = t2c_order_next ( list, p, eval_pt0 ) ) {
        eval_pt1 ( DML_RECORD(item), p ) ;
        dml_append_data ( templist, DML_RECORD(item) ) ;
        dml_remove_item ( list, item ) ;
    }
    dml_append_list ( list, templist ) ;
    dml_free_list ( templist ) ;
}


/*----------------------------------------------------------------------*/
STATIC DML_ITEM t2c_order_next ( list, pt, eval_pt0 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
PT2 pt ;
PF_T2C_EVAL eval_pt0 ;
{
    DML_ITEM item0, item ;
    REAL dist, dist0 ;
    PT2 p ;

    if ( pt == NULL ) 
        RETURN ( DML_FIRST(list) ) ;
    item0 = NULL ;
    dist0 = 0.0 ;

    DML_WALK_LIST ( list, item ) {
        eval_pt0 ( DML_RECORD(item), p ) ;
        dist = C2V_DIST ( p, pt ) ;
        if ( ( item0 == NULL ) || ( dist < dist0 ) ) {
            dist0 = dist ;
            item0 = item ;
        }
    }
    RETURN ( item0 ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

