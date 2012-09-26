/* -R -Z -S -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2D.C ***********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2adefs.h>
#include <c2vdefs.h>
#include <c2vmcrs.h>
#include <c2cdefs.h>
#include <c2ddefs.h>
#include <dmldefs.h>
#include <m2cdefs.h>
#include <t2cdefs.h>
#include <t2ddefs.h>
#include <t2dpriv.h>
#include <t2ipriv.h>
#include <t2attrd.h>
#include <dmbdefs.h>

GLOBAL      INT             T2_EDGE_BUF_SIZE=0 ;
GLOBAL      INT             T2_EDGES_NUMBER=0 ;
GLOBAL      T2_EDGE         *T2_EDGE_BUFFER ;
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC T2_EDGE t2d_alloc_edge __(( void )) ;
STATIC void t2d_clear_edge __(( T2_EDGE )) ;

/*----------------------------------------------------------------------*/
BBS_PRIVATE void t2d_set_edge_buf_size ( n ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT n ;
{
    T2_EDGE_BUF_SIZE = n ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_REGION t2d_create_region ( looplist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
{
    DML_ITEM item ;
    T2_LOOP loop ;
    T2_REGION region = T2_ALLOC_REGION ;

    if ( region == NULL ) 
        RETURN ( NULL ) ;
    T2_REGION_LOOP_LIST(region) = looplist ;
    T2_REGION_ATTR(region) = T2_ATTR_INIT ;
    T2_REGION_PARENT(region) = NULL ;
    if ( looplist != NULL ) {
        DML_WALK_LIST ( looplist, item ) {
            loop = ( T2_LOOP ) dml_record ( item ) ;
            T2_LOOP_REGION(loop) = region ;
        }
    }
    RETURN ( region ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_REGION t2d_loop_to_region ( loop )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    T2_REGION region ;

    region = t2d_create_region ( dml_create_list() ) ;
    T2_LOOP_REGION(loop) = region ;
    RETURN ( dml_append_data ( T2_REGION_LOOP_LIST(region), loop ) == NULL ?
        NULL : region ) ;

}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2d_free_region ( region )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
{
    if ( region != NULL )
    {
        if ( T2_REGION_LOOP_LIST(region) != NULL ) {
            dml_destroy_list ( T2_REGION_LOOP_LIST(region),
                (PF_ACTION) t2d_free_loop ) ;
            T2_REGION_LOOP_LIST(region) = NULL ;
        }
        T2_FREE_REGION ( region ) ;
    }
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2d_clear_region ( region )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
{
    if ( region != NULL ) {
        dml_free_list ( T2_REGION_LOOP_LIST(region) ) ;
        T2_REGION_LOOP_LIST(region) = NULL ;
        T2_FREE_REGION ( region ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2d_create_loop ( region, edgelist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
DML_LIST edgelist ;
{
    T2_LOOP loop = T2_ALLOC_LOOP ;
    DML_ITEM item ;
    T2_EDGE edge ;
    PT2 p0, p1 ;

    if ( loop == NULL ) 
        RETURN ( NULL ) ;
    T2_LOOP_REGION(loop) = region ;
    T2_LOOP_EDGE_LIST(loop) = edgelist ;
    T2_LOOP_ATTR(loop) = T2_ATTR_INIT ;
    T2_LOOP_PARENT(loop) = NULL ;
    T2_LOOP_CLOSED(loop) = FALSE ;
    T2_LOOP_LINK_LIST(loop) = NULL ;
    if ( edgelist != NULL && dml_length ( edgelist ) > 0 )
    {
        edge = dml_first_record ( edgelist ) ;
        t2c_ept0 ( edge, p0 ) ;
        edge = dml_last_record ( edgelist ) ;
        t2c_ept1 ( edge, p1 ) ;
        T2_LOOP_CLOSED(loop) = C2V_IDENT_PTS ( p0, p1 ) ;
        c2a_box_init_pt ( T2_LOOP_BOX(loop), p0 ) ;
        DML_WALK_LIST ( edgelist, item ) {
            edge = dml_record ( item ) ;
            T2_EDGE_LOOP(edge) = loop ;
            c2a_box_append ( T2_LOOP_BOX(loop), T2_EDGE_BOX(edge) ) ;
        }
    }
    RETURN ( loop ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2d_append_edgelist ( loop, edgelist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
DML_LIST edgelist ;
{
    DML_ITEM item ;
    T2_EDGE edge ;
    PT2 p ;

    if ( T2_LOOP_EDGE_LIST(loop) == NULL ) {
        T2_LOOP_EDGE_LIST(loop) = dml_create_list ();
        t2c_ept0 ( dml_first_record ( edgelist ), p ) ;
        c2a_box_init_pt ( T2_LOOP_BOX(loop), p ) ;
    }
    DML_WALK_LIST ( edgelist, item ) {
        edge = (T2_EDGE)DML_RECORD(item) ;
        T2_EDGE_LOOP(edge) = loop ;
        c2a_box_append ( T2_LOOP_BOX(loop), T2_EDGE_BOX(edge) ) ;
    }
    dml_append_list ( T2_LOOP_EDGE_LIST(loop), edgelist ) ; 
    T2_LOOP_CLOSED(loop) = t2c_loop_closed ( loop ) ;
    RETURN ( loop ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2d_append_loop ( loop, loop1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop, loop1 ;
{
    DML_ITEM item ;
    T2_EDGE edge, edge1 ;

    edge = t2c_last_edge ( loop ) ;
    edge1 = t2c_first_edge ( loop1 ) ;
    if ( edge != NULL && edge1 != NULL ) 
        t2d_adjust_epts ( edge, edge1 ) ;
    if ( T2_LOOP_EDGE_LIST(loop) == NULL ) {
        if ( edge1 == NULL ) 
            RETURN ( loop ) ;
        T2_LOOP_EDGE_LIST(loop) = dml_create_list ();
        c2a_box_copy ( T2_EDGE_BOX(edge1), T2_LOOP_BOX(loop) ) ;
    }
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop1), item ) {
        edge = (T2_EDGE)DML_RECORD(item) ;
        T2_EDGE_LOOP(edge) = loop ;
    }
    dml_append_list ( T2_LOOP_EDGE_LIST(loop), T2_LOOP_EDGE_LIST(loop1) ) ; 
    if ( dml_length ( T2_LOOP_EDGE_LIST(loop1) ) > 0 ) 
        c2a_box_union ( T2_LOOP_BOX(loop), T2_LOOP_BOX(loop1), 
            T2_LOOP_BOX(loop) ) ;
    T2_LOOP_CLOSED(loop) = t2c_loop_closed ( loop ) ;
    RETURN ( loop ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_REGION t2d_copy_region ( region )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
{
    T2_LOOP loop ;
    DML_ITEM item ;
    DML_LIST looplist = dml_create_list();
    T2_REGION copy ;

    if ( looplist == NULL ) 
        RETURN ( NULL ) ;
    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) {
        loop = t2d_copy_loop ( DML_RECORD(item), NULL, NULL ) ;
        if ( loop == NULL )
            RETURN ( NULL ) ;
        if ( dml_append_data ( looplist, loop ) == NULL ) 
            RETURN ( NULL ) ;
    }
    copy = t2d_create_region ( looplist ) ;
    T2_REGION_ATTR(copy) = T2_REGION_ATTR(region) ; 
    T2_REGION_PARENT(copy) = T2_REGION_PARENT(region) == NULL ? 
        region : T2_REGION_PARENT(region) ; 
    RETURN ( copy ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2d_copy_loop ( loop, edge0, edge1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_EDGE edge0, edge1 ;
{
    T2_LOOP loop1 ;
    DML_ITEM item, item0, item1 ;
    DML_LIST edgelist = dml_create_list() ;
    T2_EDGE new ;

    if ( edgelist == NULL ) 
        RETURN ( NULL ) ;
    item0 = ( edge0 != NULL ? dml_find_data ( T2_LOOP_EDGE_LIST(loop), 
        ( ANY ) edge0 ) : dml_first ( T2_LOOP_EDGE_LIST(loop) ) ) ;
    item1 = ( edge1 != NULL ? dml_find_data ( T2_LOOP_EDGE_LIST(loop), 
        edge1 ) : NULL ) ;
    item1 = ( item1 != NULL ? dml_next ( item1 ) : NULL ) ;
    for ( item = item0; item!=item1 && item!=NULL ; item = dml_next(item) )
    {
        new = t2d_copy_edge ( dml_record ( item ) ) ;
        if ( new == NULL ) 
            RETURN ( NULL ) ;
        if ( dml_append_data ( edgelist, new ) == NULL )
            RETURN ( NULL ) ;
    }
    loop1 = t2d_create_loop ( /*T2_LOOP_REGION(loop)*/ NULL, edgelist ) ;
    if ( loop1 == NULL ) 
        RETURN ( NULL ) ;
    T2_LOOP_ATTR(loop1) = T2_LOOP_ATTR(loop) ;
    T2_LOOP_PARENT(loop1) = T2_LOOP_PARENT(loop) == NULL ? 
        loop : T2_LOOP_PARENT(loop) ;
    RETURN ( loop1 ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2d_trim_loop ( loop, edge0, parm0, edge1, parm1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_EDGE edge0 ;
PARM parm0 ;
T2_EDGE edge1 ;
PARM parm1 ;
{
    T2_EDGE edge ;
    DML_LIST list = T2_LOOP_EDGE_LIST(loop) ;
    DML_ITEM item, item1, item2 ;
    PARM_S parms0, parms1 ;

    if ( edge0 == NULL ) 
        edge0 = t2c_first_edge ( loop ) ;
    if ( parm0 == NULL ) {
        parm0 = &parms0 ;
        PARM_COPY ( T2_EDGE_PARM0(edge0), parm0 ) ;
    }
    if ( edge1 == NULL ) 
        edge1 = t2c_last_edge ( loop ) ;
    if ( parm1 == NULL ) {
        parm1 = &parms1 ;
        PARM_COPY ( T2_EDGE_PARM1(edge1), parm1 ) ;
    }

    item1 = dml_find_data ( list, edge0 ) ;
    for ( item = dml_first ( list ) ; item != item1 ; item = item2 ) 
    {
        item2 = dml_next ( item ) ;
        edge = ( T2_EDGE ) dml_record ( item ) ;
        t2d_free_edge ( edge ) ;
        edge = NULL ;
        dml_remove_item ( list, item ) ;
        item = NULL ;
    }
    t2c_trim_edge ( edge0, parm0, NULL ) ;
    if ( ( edge0 != edge1 ) && 
        IS_ZERO ( T2_EDGE_T1(edge0) - T2_EDGE_T0(edge0) ) ) {
        t2d_free_edge ( edge0 ) ;
        dml_remove_data ( list, edge0 ) ;
        edge0 = NULL ;
    }

    item1 = dml_find_data ( list, edge1 ) ;
    if ( item1 == NULL ) {
        t2c_loop_closed ( loop ) ;
        RETURN ;
    }
    t2c_trim_edge ( edge1, NULL, parm1 ) ;

    for ( item = dml_next ( item1 ) ; item != NULL ; item = item2 ) 
    {
        item2 = dml_next ( item ) ;
        edge = ( T2_EDGE ) dml_record ( item ) ;
        t2d_free_edge ( edge ) ;
        edge = NULL ;
        dml_remove_item ( list, item ) ;
        item = NULL ;
    }

    if ( IS_ZERO ( T2_EDGE_T1(edge1) - T2_EDGE_T0(edge1) ) ) {
        t2d_free_edge ( edge1 ) ;
        dml_remove_data ( list, edge1 ) ;
        edge1 = NULL ;
    }
    t2c_loop_closed ( loop ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2d_free_loop ( loop )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    if ( loop != NULL ) 
    {
        if ( T2_LOOP_EDGE_LIST(loop) != NULL ) {
            dml_destroy_list ( T2_LOOP_EDGE_LIST(loop), 
                ( PF_ACTION ) t2d_free_edge ) ;
            T2_LOOP_EDGE_LIST(loop) = NULL ;
        }
        if ( T2_LOOP_LINK_LIST(loop) != NULL ) {
            dml_destroy_list ( T2_LOOP_LINK_LIST(loop), 
            				   ( PF_ACTION )t2i_free_link ) ;
            T2_LOOP_LINK_LIST(loop) = NULL ;
        }
        T2_FREE_LOOP ( loop ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2d_clear_loop ( loop )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    if ( loop != NULL ) {
        dml_free_list ( T2_LOOP_EDGE_LIST(loop) ) ;
        T2_LOOP_EDGE_LIST(loop) = NULL ;
        T2_FREE_LOOP ( loop ) ;
        if ( T2_LOOP_LINK_LIST(loop) != NULL ) {
            dml_destroy_list ( T2_LOOP_LINK_LIST(loop), 
            				   ( PF_ACTION ) t2i_free_link ) ;
            T2_LOOP_LINK_LIST(loop) = NULL ;
        }
    }
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2d_alloc_edge ( )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    RETURN ( dmb_malloc ( (ANY**)&T2_EDGE_BUFFER, T2_EDGE_BUF_SIZE, 
        &T2_EDGES_NUMBER, sizeof(T2_EDGE_S) ) ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_EDGE t2d_create_edge ( loop, curve, dir )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
C2_CURVE curve ;
INT dir ;
{
    T2_EDGE edge = t2d_alloc_edge () ;
    if ( edge == NULL ) 
        RETURN ( NULL ) ;
    T2_EDGE_LOOP(edge) = loop ;
    T2_EDGE_CURVE(edge) = curve ;
    T2_EDGE_DIR(edge) = dir ;
    T2_EDGE_ATTR(edge) = T2_ATTR_INIT ;
    T2_EDGE_PARENT(edge) = NULL ;
    T2_EDGE_REF(edge) = NULL ;
    T2_EDGE_VTX(edge) = T2_VTX_UNDEF ;
    C2V_SET_ZERO ( T2_EDGE_SC_PT(edge) ) ;
    RETURN ( edge ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_EDGE t2d_copy_edge ( edge )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
{
    C2_CURVE curve ;
    T2_EDGE new ;

    if ( T2_EDGE_CURVE(edge) == NULL ) 
        curve = NULL ;
    else {
        curve = c2d_copy ( T2_EDGE_CURVE(edge) ) ;
        if ( curve == NULL ) 
            RETURN ( NULL ) ;
    }
    new = t2d_create_edge ( T2_EDGE_LOOP(edge), curve, T2_EDGE_DIR(edge) ) ;
    if ( new == NULL ) 
        RETURN ( NULL ) ;
    T2_EDGE_ATTR(new) = T2_EDGE_ATTR(edge) ;
    T2_EDGE_REF(new) = NULL ;
    T2_EDGE_PARENT(new) = ( T2_EDGE_PARENT(edge) == NULL ) ? 
        edge : T2_EDGE_PARENT(edge) ;
    T2_EDGE_VTX(new) = T2_EDGE_VTX(edge) ;
    C2V_COPY ( T2_EDGE_SC_PT(edge), T2_EDGE_SC_PT(new) ) ;
    RETURN ( new ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_EDGE t2d_trim_edge ( edge, parm0, parm1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM parm0, parm1 ;
{
    T2_EDGE new = t2d_copy_edge ( edge ) ;
    if ( new != NULL ) 
        t2c_trim_edge ( new, parm0, parm1 ) ;
    if ( IS_ZERO ( T2_EDGE_T1(new) - T2_EDGE_T0(new) ) ) {
        t2d_free_edge ( new ) ;
        new = NULL ;
    }
    RETURN ( new ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_EDGE t2d_trim_ext_edge ( edge, parm0, parm1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PARM parm0, parm1 ;
{
    T2_EDGE new ;
    PARM_S parm ;

    if ( !T2_EDGE_IS_ARC(edge) ) {
        t2c_trim_edge ( edge, parm0, parm1 ) ;
        RETURN ( edge ) ;
    }
    else if ( parm0 != NULL && PARM_T(parm0) > T2_EDGE_T_MAX(edge) ) {
        PARM_T(&parm) = 2.0 ;
        PARM_J(&parm) = 2 ;
        new = t2d_trim_edge ( edge, parm0, &parm ) ;
        PARM_T(&parm) = 0.0 ;
        PARM_J(&parm) = 0 ;
        t2c_trim_edge ( edge, &parm, parm1 ) ;
        RETURN ( new ) ;
    }
    else if ( parm1 != NULL && PARM_T(parm1) < T2_EDGE_T_MIN(edge) ) {
        PARM_T(&parm) = 2.0 ;
        PARM_J(&parm) = 2 ;
        new = t2d_trim_edge ( edge, parm0, &parm ) ;
        PARM_T(&parm) = 0.0 ;
        PARM_J(&parm) = 0 ;
        t2c_trim_edge ( edge, &parm, parm1 ) ;
        RETURN ( new ) ;
    }
    else {
        t2c_trim_edge ( edge, parm0, parm1 ) ;
        RETURN ( edge ) ;
    }
}
        

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2d_free_edge ( edge )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
{
    dmb_free ( edge, (ANY**)&T2_EDGE_BUFFER, T2_EDGE_BUF_SIZE, 
        &T2_EDGES_NUMBER, ( PF_ACTION ) t2d_clear_edge ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2d_clear_edge ( edge )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
{
    if ( T2_EDGE_CURVE(edge) != NULL ) {
        c2d_free_curve ( T2_EDGE_CURVE(edge) ) ;
        T2_EDGE_CURVE(edge) = NULL ;
        T2_EDGE_LOOP(edge) = NULL ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2d_loop_rearrange ( loop, pt, parm )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt ;
T2_PARM parm ;
{
    RETURN ( parm != NULL ? t2d_loop_rearrange_parm ( loop, parm ) :
        t2d_loop_rearrange_pt ( loop, pt, NULL ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2d_loop_rearrange_pt ( loop, pt, loop_pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt, loop_pt ;
{
    T2_LOOP copy ;

    copy = t2d_copy_loop ( loop, NULL, NULL ) ;
    if ( copy == NULL ) 
        RETURN ( NULL ) ;
    if ( pt == NULL || t2c_loop_rearrange_pt ( copy, pt, TRUE, loop_pt ) )
        RETURN ( copy ) ;
    else {
        t2d_free_loop ( copy ) ;
        RETURN ( NULL ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2d_loop_rearrange_parm ( loop, parm )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_PARM parm ;
{
    T2_LOOP copy ;

    copy = t2d_copy_loop ( loop, NULL, NULL ) ;
    if ( copy == NULL ) 
        RETURN ( NULL ) ;

    if ( t2c_loop_rearrange_parm ( copy, parm ) )
        RETURN ( copy ) ;
    else {
        t2d_free_loop ( copy ) ;
        RETURN ( NULL ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_CURVE t2d_loop_curve ( loop, item, ang_tol )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* This routine computes first contiguous curve, if it is a line or an arc,
starting at a specified edge; an edge is specified by its item ; 
if item == NULL, the first edge is specified */
T2_LOOP loop ;
DML_ITEM item ;
REAL ang_tol ;
{
    PT2 p0, p1, p2, c0, c1, tan0, tan1 ;
    BOOLEAN first_line, new_curve ;
    T2_EDGE edge0, edge1 ;
    DML_ITEM item0, item1 ;
    REAL angle ;

    if ( item == NULL ) 
        item = dml_first ( T2_LOOP_EDGE_LIST(loop) ) ;

    edge0 = dml_record ( item ) ;

    t2c_ept0 ( edge0, p0 ) ;
    first_line = T2_EDGE_IS_LINE(edge0) ;
    if ( T2_EDGE_IS_ARC(edge0) ) 
        t2c_mid_pt ( edge0, p1 ) ;
    else if ( !first_line ) 
        RETURN ( c2d_copy ( T2_EDGE_CURVE(edge0) ) ) ;
    t2c_ept1 ( edge0, p2 ) ;

    for ( item0 = item ; ; item0 = dml_next ( item0 ) ) {
        item1 = DML_NEXT(item0) ;
        if ( item1 == NULL ) 
            item1 = dml_first ( T2_LOOP_EDGE_LIST(loop) ) ;
        edge0 = DML_RECORD(item0) ;
        edge1 = DML_RECORD(item1) ;

        t2c_etan1 ( edge0, tan0 ) ;
        t2c_etan0 ( edge1, tan1 ) ;
        angle = c2v_vecs_angle ( tan0, tan1 ) ;
        new_curve = TRUE ;
        if ( fabs(angle-PI) <= ang_tol ) {
            if ( first_line && T2_EDGE_IS_LINE(edge1) ) 
                new_curve = FALSE ;
            else if ( T2_EDGE_IS_ARC(edge0) && T2_EDGE_IS_ARC(edge1) ) {
                if ( c2c_get_arc_center ( T2_EDGE_CURVE(edge0), c0 ) &&
                     c2c_get_arc_center ( T2_EDGE_CURVE(edge1), c1 ) &&
                     C2V_IDENT_PTS ( c0, c1 ) ) 
                new_curve = FALSE ;
            }
        }
        if ( new_curve ) {
            if ( first_line )
                RETURN ( c2d_line ( p0, p2 ) ) ;
            else if ( C2V_IDENT_PTS ( p0, p2 ) )
                RETURN ( c2d_circle_ctr_pt ( c0, p0 ) ) ;
            else 
                RETURN ( c2d_arc_3pts ( p0, p1, p2 ) ) ;
        }
        else 
            t2c_ept1 ( edge1, p2 ) ;
        if ( item1 == item ) 
            RETURN ( NULL ) ;
    }
#if 0
    RETURN ( NULL ) ;
#endif
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

