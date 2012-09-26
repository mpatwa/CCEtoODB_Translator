/* -S -Z -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2B7.C **********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <t2bpriv.h>
#include <t2cdefs.h>
#include <c2vmcrs.h>
#include <t2link.h>
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )
STATIC BOOLEAN t2b_parm_midpt __(( T2_PARM, T2_PARM, PT2, T2_PARM )) ;
STATIC void t2b_classify_region __(( T2_REGION, T2_REGION )) ;
STATIC void t2b_classify_loop __(( T2_LOOP, T2_REGION )) ;
STATIC void t2b_classify_fragment __(( T2_LINK, T2_LINK, T2_REGION )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_classify ( region1, region2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region1, region2 ;
{
    t2b_classify_region ( region1, region2 ) ;
    t2b_classify_region ( region2, region1 ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_classify_region ( region, other_region ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region, other_region ;
{
    DML_ITEM item ;

    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) 
        t2b_classify_loop ( DML_RECORD(item), other_region ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_classify_loop ( loop, other_region ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_REGION other_region ;
{
    DML_ITEM item, item1 ;

    t2b_loop_link_epts ( loop, other_region ) ;
    if ( T2_LOOP_LINK_LIST(loop) == NULL ) 
        RETURN ;
    DML_WALK_LIST ( T2_LOOP_LINK_LIST(loop), item ) {
        item1 = DML_NEXT(item) ;
        if ( item1 == NULL )
            RETURN ;
        t2b_classify_fragment ( DML_RECORD(item), DML_RECORD(item1), 
            other_region ) ;
    }
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_classify_fragment ( link0, link1, other_region )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LINK link0, link1 ;
T2_REGION other_region ;
{
    T2_PARM_S parm0, parm1 ;
    PT2 pt, tan0, tan1 ;
    T2_PT_POSITION pos ;

#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_point ( T2_LINK_PT(link0), 0.01, 13 ) ;
        getch();
        paint_point ( T2_LINK_PT(link1), 0.01, 14 ) ;
        getch();
    }
    else
        DISPLAY-- ;
#endif

    if ( ( T2_LINK_STATUS(link0) & T2_LINK_NEXT_IN ) &&
         ( T2_LINK_STATUS(link1) & T2_LINK_PREV_IN ) )
        RETURN ;
    if ( ( T2_LINK_STATUS(link0) & T2_LINK_NEXT_OUT ) &&
         ( T2_LINK_STATUS(link1) & T2_LINK_PREV_OUT ) )
        RETURN ;
    if ( ( T2_LINK_STATUS(link0) & T2_LINK_CODIR0 ) && 
         ( T2_LINK_STATUS(link1) & T2_LINK_CODIR1 ) ) 
        RETURN ;
    if ( ( T2_LINK_STATUS(link0) & T2_LINK_ANTIDIR0 ) &&
         ( T2_LINK_STATUS(link1) & T2_LINK_ANTIDIR1 ) ) 
        RETURN ;
    
    if ( !t2b_parm_midpt ( T2_LINK_PARM(link0), T2_LINK_PARM(link1), 
        pt, &parm0 ) )
        RETURN ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_point ( pt, 0.01, 12 ) ;
        getch();
    }
    else
        DISPLAY-- ;
#endif

    pos = t2b_pt_pos_region ( other_region, pt ) ;
    if ( pos == T2_PT_INSIDE ) {
        T2_LINK_STATUS(link0) = T2_LINK_STATUS(link0) | T2_LINK_NEXT_IN ;
        T2_LINK_STATUS(link1) = T2_LINK_STATUS(link1) | T2_LINK_PREV_IN ;
    }
    else if ( pos == T2_PT_OUTSIDE ) {
        T2_LINK_STATUS(link0) = T2_LINK_STATUS(link0) | T2_LINK_NEXT_OUT ;
        T2_LINK_STATUS(link1) = T2_LINK_STATUS(link1) | T2_LINK_PREV_OUT ;
    }
    else if ( pos == T2_PT_ON_BOUNDARY ) {
        t2c_project_region ( other_region, pt, FALSE, &parm1, NULL ) ;
        t2c_eval_tan ( T2_PARM_EDGE(&parm0), T2_PARM_CPARM(&parm0), tan0 ) ;
        t2c_eval_tan ( T2_PARM_EDGE(&parm1), T2_PARM_CPARM(&parm1), tan1 ) ;

        if ( C2V_DOT ( tan0, tan1 ) > 0.0 ) {
            T2_LINK_STATUS(link0) = T2_LINK_STATUS(link0) | T2_LINK_CODIR0 ;
            T2_LINK_STATUS(link1) = T2_LINK_STATUS(link1) | T2_LINK_CODIR1 ;
        }
        else {
            T2_LINK_STATUS(link0) = T2_LINK_STATUS(link0) | T2_LINK_ANTIDIR0 ;
            T2_LINK_STATUS(link1) = T2_LINK_STATUS(link1) | T2_LINK_ANTIDIR1 ;
        }
    }
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_parm_midpt ( parm0, parm1, pt, parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_PARM parm0, parm1 ;
PT2 pt ;
T2_PARM parm ;
{
    T2_EDGE edge0, edge1, edge ;
    REAL t0, t1 ;

    edge0 = T2_PARM_EDGE(parm0) ;
    edge1 = T2_PARM_EDGE(parm1) ;

    if ( edge0 == edge1 ) {
        if ( IS_ZERO ( T2_PARM_T(parm0) - T2_PARM_T(parm1) ) )
            RETURN ( FALSE ) ;
        T2_PARM_EDGE(parm) = edge0 ;
        T2_PARM_T(parm) = 0.5 * ( T2_PARM_T(parm0) + T2_PARM_T(parm1) ) ;
        T2_PARM_J(parm) = ( T2_PARM_J(parm0) + T2_PARM_J(parm1) ) / 2 ;
    }
    else {
        t0 = fabs ( T2_PARM_T(parm0) - T2_EDGE_T1(edge0) ) ;
        t1 = fabs ( T2_PARM_T(parm1) - T2_EDGE_T0(edge1) ) ;
        if ( IS_ZERO(t0) && IS_ZERO(t1) ) {
            edge = t2c_next_edge ( edge0 ) ;
            if ( edge == edge1 ) 
                RETURN ( FALSE ) ;
            else 
                t2c_mid_pt_parm ( edge, pt, parm ) ;
        }

        else if ( t0 > t1 ) {
            T2_PARM_EDGE(parm) = edge0 ;
            T2_PARM_T(parm) = 0.5 * ( T2_PARM_T(parm0) + T2_EDGE_T1(edge0) ) ;
            T2_PARM_J(parm) = ( T2_PARM_J(parm0) + T2_EDGE_J1(edge0) ) / 2 ;
        }

        else {
            T2_PARM_EDGE(parm) = edge1 ;
            T2_PARM_T(parm) = 0.5 * ( T2_PARM_T(parm1) + T2_EDGE_T0(edge1) ) ;
            T2_PARM_J(parm) = ( T2_PARM_J(parm1) + T2_EDGE_J0(edge1) ) / 2 ;
        }
    }
    RETURN ( t2c_eval_pt ( T2_PARM_EDGE(parm), T2_PARM_CPARM(parm), pt ) ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */


