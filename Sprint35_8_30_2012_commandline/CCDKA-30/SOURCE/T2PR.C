/* -Z -S __BBS_MILL__=1 */
/******************************* T2PR.C *********************************/ 
/******************************* Roughing *******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h> 
#include <c2ddefs.h> 
#include <c2dpriv.h> 
#include <t2bdefs.h> 
#include <t2bpriv.h> 
#include <t2cdefs.h> 
#include <t2cpriv.h> 
#include <t2ddefs.h> 
#include <t2dpriv.h> 
#include <t2idefs.h> 
#include <t2pdefs.h> 
#include <t2attrd.h> 
#include <c2vmcrs.h> 

#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif
#if ( __BBS_MILL__>=1 )

STATIC DML_LIST t2p_rough_loop __(( T2_LOOP, REAL, BOOLEAN, DML_LIST )) ;
STATIC DML_LIST t2p_rough_pass1 __(( T2_LOOP, REAL, BOOLEAN )) ;
STATIC DML_LIST t2p_rough_pass2 __(( DML_LIST, REAL, BOOLEAN )) ;
STATIC BOOLEAN t2p_rough_check_concave __(( T2_LOOP )) ;
STATIC BOOLEAN t2p_rough_check_intloop __(( T2_LOOP )) ;
STATIC T2_LOOP t2p_break_loop __(( T2_LOOP )) ;
STATIC DML_LIST t2p_offset_rough __(( T2_LOOP, REAL, BOOLEAN, DML_LIST )) ;

/*----------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2p_rough_region ( region, dist, end_arc, region_list ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
REAL dist ;
BOOLEAN end_arc ;
DML_LIST region_list ;
{
    DML_ITEM item ;
    DML_LIST looplist, temp_list ;

    if ( IS_SMALL(dist) ) {
        dml_append_data ( region_list, t2d_copy_region ( region ) ) ;
        RETURN ( region_list ) ;
    }

    temp_list = dml_create_list();
    if ( temp_list == NULL ) 
        RETURN ( NULL ) ;

    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) {
        temp_list = t2p_rough_loop ( DML_RECORD(item), dist, 
            end_arc, temp_list ) ;
        if ( temp_list == NULL )
            RETURN ( NULL ) ;
    }

    looplist = t2d_link_loops ( temp_list ) ;
    region_list = t2d_create_regions ( looplist, region_list ) ;
    dml_free_list ( temp_list ) ;
    dml_free_list ( looplist ) ;
    RETURN ( region_list ) ;
}


/*----------------------------------------------------------------------*/
STATIC DML_LIST t2p_rough_loop ( loop, dist, end_arc, offset_list ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL dist ;
BOOLEAN end_arc ;
DML_LIST offset_list ;
{
    DML_LIST looplist ;

    if ( offset_list == NULL ) 
        offset_list = t2p_rough_pass1 ( loop, dist, end_arc ) ;
    else {
        looplist = t2p_rough_pass1 ( loop, dist, end_arc ) ;
        dml_append_list ( offset_list, looplist ) ;
        dml_free_list ( looplist ) ;
    }
    RETURN ( offset_list ) ;
}


/*----------------------------------------------------------------------*/
STATIC DML_LIST t2p_rough_pass1 ( loop, dist, end_arc ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL dist ;
BOOLEAN end_arc ;
{
    BOOLEAN done = FALSE ;
    DML_LIST looplist, offset_list ;
    T2_LOOP loop1 ;
    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ( NULL ) ;

    if ( loop == T2_REGION_EXT_LOOP(T2_LOOP_REGION(loop)) ) {
        while ( !done ) 
            done = t2p_rough_check_concave ( loop ) ;
    }

    else
        t2p_rough_check_intloop ( loop ) ;

    looplist = dml_create_list ();
    for ( loop1 = t2p_break_loop ( loop ) ; loop1 != NULL ;
        loop1 = t2p_break_loop ( loop ) ) 
        dml_append_data ( looplist, loop1 ) ;
    offset_list = t2p_rough_pass2 ( looplist, dist, end_arc ) ;
    dml_free_list ( looplist ) ;
    RETURN ( offset_list ) ;
}


/*----------------------------------------------------------------------*/
STATIC T2_LOOP t2p_break_loop ( loop ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    T2_EDGE edge ;
    T2_LOOP loop1 ;
    DML_LIST edgelist ;
    ATTR attr ;
    BOOLEAN end_loop ;
    DML_ITEM item, item1 ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ||
        DML_LENGTH ( T2_LOOP_EDGE_LIST(loop) ) == 0 ) 
        RETURN ( NULL ) ;
    edgelist = dml_create_list();
    edge = DML_FIRST_RECORD ( T2_LOOP_EDGE_LIST(loop) ) ;
    attr = T2_EDGE_ATTR(edge) ;
    end_loop = FALSE ;

    for ( item = DML_FIRST ( T2_LOOP_EDGE_LIST(loop) ) ; 
        item != NULL && !end_loop ; item = item1 ) { 
        item1 = DML_NEXT ( item ) ;
        edge = DML_RECORD(item) ;
        if ( T2_EDGE_ATTR(edge) & attr ) {
            dml_append_data ( edgelist, edge ) ;
            DML_RECORD(item) = NULL ;
            dml_remove_item ( T2_LOOP_EDGE_LIST(loop), item ) ; 
        }
        else 
            end_loop = TRUE ;
    }

    loop1 = t2d_create_loop ( NULL, edgelist ) ;
    T2_LOOP_ATTR(loop1) = attr ;
    RETURN ( loop1 ) ;
}


/*----------------------------------------------------------------------*/
STATIC DML_LIST t2p_rough_pass2 ( looplist, dist, end_arc ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
REAL dist ;
BOOLEAN end_arc ;
{
    DML_LIST offset_list ;
    DML_ITEM item ;
    T2_LOOP loop ;

    if ( looplist == NULL || DML_LENGTH(looplist) == 0 ) 
        RETURN ( NULL ) ;

    offset_list = dml_create_list ();
    DML_WALK_LIST ( looplist, item ) {
        loop = DML_RECORD(item) ;
        if ( T2_LOOP_ATTR(loop) & T2_ATTR_PART ) 
            dml_append_data ( offset_list, loop ) ;
        else if ( T2_LOOP_ATTR(loop) & T2_ATTR_ROUGH ) {
            t2p_offset_rough ( loop, dist, end_arc, offset_list ) ;
            t2d_free_loop ( loop ) ;
        }
        else 
            dml_append_data ( offset_list, loop ) ;
    }
    RETURN ( offset_list ) ;
}


/*----------------------------------------------------------------------*/
STATIC DML_LIST t2p_offset_rough ( loop, dist, end_arc, looplist ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL dist ;
BOOLEAN end_arc ;
DML_LIST looplist ;
{
    T2_EDGE edge0, edge1, new ;
    T2_LOOP loop0, loop1 ;
    DML_ITEM item, last ;
    PT2 p0, p1, q0, q1 ;
    T2_OFFSET_DESC desc ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ||
        DML_LENGTH ( T2_LOOP_EDGE_LIST(loop) ) == 0 ) 
        RETURN ( NULL ) ;

    desc = t2b_offset_set_mode ( NULL, T2_SHARP ) ;

    last = DML_LAST(looplist) ;

    edge0 = DML_FIRST_RECORD ( T2_LOOP_EDGE_LIST(loop) ) ;
    edge1 = DML_LAST_RECORD ( T2_LOOP_EDGE_LIST(loop) ) ;
    t2c_ept0 ( edge0, p0 ) ;
    t2c_ept1 ( edge1, p1 ) ;
    t2b_offset_loop ( loop, dist, desc, looplist ) ;
    for ( item = ( last == NULL ) ? dml_first(looplist) : DML_NEXT(last) ; 
        item != NULL ; item = DML_NEXT(item) ) 
        t2c_mark_loop ( DML_RECORD(item), T2_ATTR_ROUGH ) ;
    if ( C2V_IDENT_PTS ( p0, p1 ) ) {
        t2b_free_offset_desc ( desc ) ;
        RETURN ( looplist ) ;
    }
    loop0 = ( last == NULL ) ? dml_first_record ( looplist ) : 
        dml_record ( dml_next ( last ) ) ;
    if ( loop0 == NULL ) {
        t2b_free_offset_desc ( desc ) ;
        RETURN ( looplist ) ;
    }
/* why would it have no edges ? */
    edge0 = DML_FIRST_RECORD ( T2_LOOP_EDGE_LIST(loop0) ) ;
    t2c_ept0 ( edge0, q0 ) ;
    new = end_arc ? 
        t2d_create_edge ( NULL, c2d_arc_pts_bulge ( p0, q0, 1.0 ), 1 ) :
        t2d_create_edge ( NULL, c2d_line ( p0, q0 ), 1 ) ;
    T2_EDGE_ATTR(new) = T2_EDGE_ATTR(new) | T2_ATTR_ROUGH ;
    dml_insert_after ( T2_LOOP_EDGE_LIST(loop0), NULL, new ) ;

    loop1 = dml_record ( dml_last ( looplist ) ) ;
    if ( loop1 == NULL ) {
        t2b_free_offset_desc ( desc ) ;
        RETURN ( looplist ) ;
    }
    edge1 = DML_LAST_RECORD ( T2_LOOP_EDGE_LIST(loop1) ) ;
    t2c_ept1 ( edge1, q1 ) ;
    new = end_arc ? 
        t2d_create_edge ( NULL, c2d_arc_pts_bulge ( q1, p1, 1.0 ), 1 ) :
        t2d_create_edge ( NULL, c2d_line ( q1, p1 ), 1 ) ;
    T2_EDGE_ATTR(new) = T2_EDGE_ATTR(new) | T2_ATTR_ROUGH ;
    dml_append_data ( T2_LOOP_EDGE_LIST(loop1), new ) ;
    t2b_free_offset_desc ( desc ) ;
    RETURN ( looplist ) ;
}


/*----------------------------------------------------------------------*/
STATIC BOOLEAN t2p_rough_check_concave ( loop ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    DML_ITEM item ;
    T2_EDGE edge0, edge1 ;
    T2_VTX_STATUS c ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ( TRUE ) ;

    item = DML_FIRST ( T2_LOOP_EDGE_LIST(loop) ) ;
    if ( item == NULL )
        RETURN ( TRUE ) ;
    edge1 = DML_RECORD(item) ;

    DML_FOR_LOOP ( DML_NEXT(item), item ) {
        edge0 = edge1 ;
        edge1 = DML_RECORD(item) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( edge0, 10 ) ;
    paint_edge ( edge1, 11 ) ;
    getch();
}
else
    DISPLAY-- ;
#endif
        if ( !( T2_EDGE_ATTR(edge0) & T2_EDGE_ATTR(edge1) ) ) {
            c = t2b_vertex_convexity ( edge0, edge1, NULL, NULL ) ;
            if ( c == T2_VTX_CONCAVE ) {
                T2_EDGE_ATTR(edge0) = T2_ATTR_PART ;
                T2_EDGE_ATTR(edge1) = T2_ATTR_PART ;
                RETURN ( FALSE ) ;
            }
        }
    }

    if ( T2_LOOP_CLOSED(loop) ) {
        edge0 = DML_LAST_RECORD ( T2_LOOP_EDGE_LIST(loop) ) ;
        edge1 = DML_FIRST_RECORD ( T2_LOOP_EDGE_LIST(loop) ) ;
        if ( T2_EDGE_ATTR(edge0) != T2_EDGE_ATTR(edge1) ) {
            c = t2b_vertex_convexity ( edge0, edge1, NULL, NULL ) ;
            if ( c == T2_VTX_CONCAVE ) {
                T2_EDGE_ATTR(edge0) = T2_ATTR_PART ;
                T2_EDGE_ATTR(edge1) = T2_ATTR_PART ;
                RETURN ( FALSE ) ;
            }
        }
    }
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
STATIC BOOLEAN t2p_rough_check_intloop ( loop ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
{
    DML_ITEM item ;
    T2_EDGE edge ;
    BOOLEAN rough, part ;

    rough = FALSE ;
    part = FALSE ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ( TRUE ) ;
    for ( item = DML_FIRST ( T2_LOOP_EDGE_LIST(loop) ) ; 
        item != NULL && ( !rough || !part ) ; item = DML_NEXT(item) ) {
        edge = DML_RECORD(item) ;
        if ( T2_EDGE_ATTR(edge) & T2_ATTR_ROUGH ) 
            rough = TRUE ;
        if ( T2_EDGE_ATTR(edge) & T2_ATTR_PART ) 
            part = TRUE ;
    }
    if ( part || !rough ) {
        DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) { 
            edge = DML_RECORD(item) ;
            T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) & ~T2_ATTR_ROUGH ;
            T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) & T2_ATTR_PART ;
        }
    }
    else {
        DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) { 
            edge = DML_RECORD(item) ;
            T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) & T2_ATTR_ROUGH ;
        }
    }
#ifdef __BRLNDC__
	RETURN ( FALSE ) ;
#endif
#ifdef __WATCOM__
	RETURN ( FALSE ) ;
#endif
}
#endif /* __BBS_MILL__>=1 */

