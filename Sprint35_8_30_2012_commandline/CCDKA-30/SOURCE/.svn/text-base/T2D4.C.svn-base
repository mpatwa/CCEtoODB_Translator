/* -R -Z -S -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2D4.C **********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <c2vmcrs.h> 
#include <dmldefs.h> 
#include <c2ddefs.h> 
#include <t2cdefs.h> 
#include <t2ddefs.h> 
#include <t2dpriv.h> 

#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC DML_LIST t2d_link_chains __(( DML_LIST )) ;
STATIC void t2d_append_to_chain __(( T2_LOOP, PT2, PT2, DML_LIST )) ;
STATIC T2_LOOP t2d_connect_chain __(( DML_LIST )) ;
STATIC DML_ITEM t2d_clst_curve __(( DML_LIST, PT2, PT2, PT2, PT2, 
        REAL*, INT* )) ;

/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_REGION t2d_create_box ( corner, side_x, side_y, angle ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 corner ;
REAL side_x, side_y, angle ;
{
    PT2 pt[5] ;
    INT i ;
    REAL c, s ;

    c = cos ( angle ) ;
    s = sin ( angle ) ;
    if ( corner == NULL ) 
    {
        for ( i=0 ; i<5 ; i++ )
            C2V_SET_ZERO ( pt[i] ) ;
    }
    else 
    {
        for ( i=0 ; i<5 ; i++ )
            C2V_COPY ( corner, pt[i] ) ;
    }
    pt[1][0] += c * side_x ;
    pt[1][1] += s * side_y ;
    pt[3][0] -= s * side_x ;
    pt[3][1] += c * side_y ;
    pt[2][0] = pt[1][0] + pt[3][0] - pt[0][0] ;
    pt[2][1] = pt[1][1] + pt[3][1] - pt[0][1] ;

    RETURN ( t2d_create_polyline ( pt, 4 ) ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_REGION t2d_create_polyline ( a, n )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 a[] ;
INT n ;
{
    INT i ;
    DML_LIST curvelist = dml_create_list();
    C2_CURVE curve ;
    T2_REGION polyline ;

    for ( i=1 ; i<n ; i++ ) {
        curve = c2d_line ( a[i-1], a[i] ) ;
        dml_append_data ( curvelist, curve ) ;
    }
    curve = c2d_line ( a[n-1], a[0] ) ;
    dml_append_data ( curvelist, curve ) ;
    polyline = t2d_build_region ( curvelist, NULL, BBS_TOL ) ;
    dml_free_list ( curvelist ) ;
    RETURN ( polyline ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_REGION t2d_create_circle ( ctr, rad ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr ;
REAL rad ;
{
    DML_LIST curvelist = dml_create_list();
    C2_CURVE curve ;
    T2_REGION circle ;
    curve = c2d_circle ( ctr, rad ) ;
    dml_append_data ( curvelist, curve ) ;
    circle = t2d_build_region ( curvelist, NULL, BBS_TOL ) ;
    dml_free_list ( curvelist ) ;
    RETURN ( circle ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2d_create_box_loop ( corner, side_x, side_y, angle ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 corner ;
REAL side_x, side_y, angle ;
{
    PT2 pt[5] ;
    INT i ;
    REAL c, s ;

    c = cos ( angle ) ;
    s = sin ( angle ) ;
    for ( i=0 ; i<5 ; i++ )
        C2V_COPY ( corner, pt[i] ) ;
    pt[1][0] += c * side_x ;
    pt[1][1] += s * side_y ;
    pt[3][0] -= s * side_x ;
    pt[3][1] += c * side_y ;
    pt[2][0] = pt[1][0] + pt[3][0] - pt[0][0] ;
    pt[2][1] = pt[1][1] + pt[3][1] - pt[0][1] ;

    RETURN ( t2d_create_polyline_loop ( pt, 4 ) ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2d_create_polyline_loop ( a, n )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 a[] ;
INT n ;
{
    INT i ;
    DML_LIST curvelist = dml_create_list();
    C2_CURVE curve ;
    T2_LOOP polyline ;

    for ( i=1 ; i<n ; i++ ) {
        curve = c2d_line ( a[i-1], a[i] ) ;
        dml_append_data ( curvelist, curve ) ;
    }
    curve = c2d_line ( a[n-1], a[0] ) ;
    dml_append_data ( curvelist, curve ) ;
    polyline = t2d_build_loop ( NULL, curvelist, NULL, BBS_TOL ) ;
    dml_free_list ( curvelist ) ;
    RETURN ( polyline ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2d_create_circle_loop ( ctr, rad ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr ;
REAL rad ;
{
    DML_LIST curvelist = dml_create_list();
    C2_CURVE curve ;
    T2_LOOP circle ;
    curve = c2d_circle ( ctr, rad ) ;
    dml_append_data ( curvelist, curve ) ;
    circle = t2d_build_loop ( NULL, curvelist, NULL, BBS_TOL ) ;
    dml_free_list ( curvelist ) ;
    RETURN ( circle ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2d_link_loops ( inlist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST inlist ;
{
    DML_LIST chain_list = t2d_link_chains ( inlist ) ;
    DML_ITEM item ;
    T2_LOOP loop ;
    DML_LIST outlist = dml_create_list();

    DML_WALK_LIST ( chain_list, item ) {    /* lists of closed loop chains */
        loop = t2d_connect_chain ( DML_RECORD(item) ) ;
        dml_append_data ( outlist, loop ) ;
    }

    dml_destroy_list ( chain_list, ( PF_ACTION ) dml_free_list ) ;

    RETURN ( outlist ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_LOOP t2d_connect_chain ( chain ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST chain ;
{
    DML_LIST edgelist = dml_create_list();
    DML_ITEM item ;
    T2_LOOP loop ;

    DML_WALK_LIST ( chain, item ) {
        loop = DML_RECORD(item) ;
        dml_append_list ( edgelist, T2_LOOP_EDGE_LIST(loop) ) ;
        t2d_free_loop ( loop ) ;
    }
    RETURN ( t2d_create_loop ( NULL, edgelist ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_LIST t2d_link_chains ( looplist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
{
    DML_LIST chainlist = dml_create_list();
    DML_ITEM item ;
    T2_LOOP loop ;
    PT2 p0, p1 ;

    DML_WALK_LIST ( looplist, item ) {
        loop = DML_RECORD(item) ;
        t2c_loop_ept0 ( loop, p0 ) ;
        t2c_loop_ept1 ( loop, p1 ) ;
        t2d_append_to_chain ( loop, p0, p1, chainlist ) ;
    }
    RETURN ( chainlist ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2d_append_to_chain ( loop, p0, p1, chainlist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 p0, p1 ;
DML_LIST chainlist ;
{
    DML_LIST chain ;
    PT2 q ;
    T2_LOOP loop0 ;
    DML_ITEM item ;

    DML_WALK_LIST ( chainlist, item ) {
        chain = DML_RECORD(item) ;
        loop0 = DML_FIRST_RECORD(chain) ;
        t2c_loop_ept0 ( loop0, q ) ;
        if ( C2V_IDENT_PTS ( p1, q ) ) {
            dml_insert_after ( chain, NULL, loop ) ;
            RETURN ;
        }
        if ( C2V_IDENT_PTS ( p0, q ) ) {
            t2c_reverse_loop ( loop ) ;
            dml_insert_after ( chain, NULL, loop ) ;
            RETURN ;
        }
        loop0 = DML_LAST_RECORD(chain) ;
        t2c_loop_ept1 ( loop0, q ) ;
        if ( C2V_IDENT_PTS ( p0, q ) ) {
            dml_append_data ( chain, loop ) ;
            RETURN ;
        }
        if ( C2V_IDENT_PTS ( p1, q ) ) {
            t2c_reverse_loop ( loop ) ;
            dml_append_data ( chain, loop ) ;
            RETURN ;
        }
    }
    chain = dml_create_list ();
    dml_append_data ( chain, loop ) ;
    dml_append_data ( chainlist, chain ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2d_fillet_loop ( loop, rad )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL rad ;
{
    T2_LOOP copy ;

    copy = t2d_copy_loop ( loop, NULL, NULL ) ;
    if ( t2c_fillet_loop ( copy, rad ) )
        RETURN ( copy ) ;
    else {
        t2d_free_loop ( copy ) ;
        RETURN ( NULL ) ;
    }
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */


