/* -S -Z -L -T __BBS_MILL__=3 __BBS_TURN__=3 */
/********************************* T2BD3.C **********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <c2vmcrs.h>
#include <c2tdefs.h>
#include <dmldefs.h>
#include <t2bdefs.h>
#include <t2bpriv.h>
#include <t2cdefs.h>
#include <t2odm.h>
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif
#if ( __BBS_MILL__>=3 || __BBS_TURN__>=3 )

/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_OD t2b_od3_lookup_list ( distlist, owner, dist0, dist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST distlist ; 
ANY owner ;
REAL dist0, *dist ;
{
    T2_OD od ;
    DML_ITEM item ;

    if ( distlist == NULL ) {
        *dist = dist0 ;
        RETURN ( NULL ) ;
    }

    DML_WALK_LIST ( distlist, item ) {
        od = (T2_OD)DML_RECORD(item) ;
        if ( T2_OD_OWNER(od) == owner ) {
            if ( dist != NULL ) {
                if ( IS_SMALL(dist0) ) 
                    *dist = T2_OD_DIST(od) ;
                else if ( dist0 > 0.0 ) 
                    *dist = fabs(T2_OD_DIST(od)) ;
                else 
                    *dist = -fabs(T2_OD_DIST(od)) ;
            }
            RETURN ( od ) ;
        }
    }
    *dist = dist0 ;
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL t2b_od3_dist_max_list ( dist, distlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL dist ;
DML_LIST distlist ;
{
    REAL max_dist ;
    T2_OD od ;
    DML_ITEM item ;

    max_dist = dist ;
    if ( distlist == NULL ) 
        RETURN ( max_dist ) ;

    DML_WALK_LIST ( distlist, item ) {
        od = (T2_OD)DML_RECORD(item) ;
        if ( fabs(max_dist) < fabs(T2_OD_DIST(od)) )
            max_dist = T2_OD_DIST(od) ;
    }
    if ( dist > BBS_TOL )
        max_dist = fabs(max_dist) ;
    else if ( dist < - BBS_TOL )
        max_dist = - fabs(max_dist) ;
    RETURN ( max_dist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_ITEM t2b_append_od3 ( owner, pt, dist, desc )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY owner ;
PT2 pt ;
REAL dist;
T2_OFFSET_DESC desc ;
{
    if ( desc == NULL )
        RETURN ( NULL ) ;
    if ( T2_OFFSET_DESC_DLIST(desc) == NULL ) 
        T2_OFFSET_DESC_DLIST(desc) = dml_create_list () ;
    if ( T2_OFFSET_DESC_DLIST(desc) == NULL ) 
        RETURN ( NULL ) ;
    RETURN ( t2b_append_od3_list ( owner, pt, dist, 
        T2_OFFSET_DESC_DLIST(desc) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_ITEM t2b_append_od3_list ( owner, pt, dist, distlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY owner ;
PT2 pt ;
REAL dist;
DML_LIST distlist ;
{
    RETURN ( ( distlist == NULL ) ? NULL : 
        dml_append_data ( distlist, t2b_create_od ( owner, pt, dist ) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_od_select_loop ( loop, desc, tol )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_OFFSET_DESC desc ;
REAL tol ;
{
    if ( desc != NULL ) 
        t2b_od_select_loop_list ( loop, T2_OFFSET_DESC_DLIST(desc), tol ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_od_select_loop_list ( loop, dist_list, tol )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
DML_LIST dist_list ;
REAL tol ;
{
    T2_PARM_S parm ;
    DML_ITEM item ;
    T2_OD od ;
    PT2 pt ;

#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_loop ( loop, 9 ) ;
        getch () ;
    }
    else
        DISPLAY-- ;
#endif

    if ( dist_list == NULL ) 
        RETURN ;
    DML_WALK_LIST ( dist_list, item ) {
        od = (T2_OD)DML_RECORD(item) ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_point ( T2_OD_PT(od), 0.02, 10 ) ;
        getch () ;
    }
    else
        DISPLAY-- ;
#endif
        if ( T2_OD_OWNER(od) == NULL && 
            t2c_project_loop ( loop, T2_OD_PT(od), TRUE, &parm, pt ) && 
            ( C2V_DISTL1 ( T2_OD_PT(od), pt ) <= tol ) ) {
            T2_OD_OWNER(od) = T2_PARM_EDGE(&parm) ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_edge ( T2_OD_OWNER(od), 11 ) ;
        getch () ;
    }
    else
        DISPLAY-- ;
#endif
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_od_edge_pts ( desc )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_OFFSET_DESC desc ;
{
    if ( desc != NULL ) 
        t2b_od_edge_pts_list ( T2_OFFSET_DESC_DLIST(desc) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2b_od_edge_pts_list ( dist_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST dist_list ;
{
    DML_ITEM item ;
    T2_OD od ;

    if ( dist_list == NULL ) 
        RETURN ;
    DML_WALK_LIST ( dist_list, item ) {
        od = DML_RECORD(item) ;
        if ( T2_OD_OWNER(od) != NULL ) 
            t2c_mid_pt ( T2_OD_OWNER(od), T2_OD_PT(od) ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2b_xform_od_list ( dist_list, xform )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST dist_list ;
C2_XFORM xform ;
{
    DML_ITEM item ;
    T2_OD od ;

    if ( dist_list == NULL ) 
        RETURN ;

    DML_WALK_LIST ( dist_list, item ) {
        od = (T2_OD)DML_RECORD(item) ;
        c2t_eval_pt ( T2_OD_PT(od), xform, T2_OD_PT(od) ) ;
    }
}
#endif /* __BBS_MILL__>=3 || __BBS_TURN__>=3 */

