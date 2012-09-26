/* -L __BBS_MILL__=3 */
/********************************* T2BR.C **********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <dmldefs.h>
#include <t2bdefs.h>
#include <t2bpriv.h>
#include <t2cdefs.h>
#include <t2cpriv.h>
#include <t2ddefs.h>
#include <t2dpriv.h>
#include <t2idefs.h>
#include <t2attrd.h>
#include <c2ddefs.h>
#include <c2vdefs.h>
#include <c2vmcrs.h>

#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
EXTERN BOOLEAN DIR ;
#endif

#if ( __BBS_MILL__>=3 )

STATIC DML_LIST t2b_recut_offset __(( DML_LIST, REAL )) ;
STATIC DML_LIST t2b_recut_connect __(( DML_LIST, DML_LIST )) ;
STATIC BOOLEAN t2b_recut_connect_2loops __(( T2_LOOP, T2_LOOP, 
            BOOLEAN, BOOLEAN )) ;
STATIC BOOLEAN t2b_recut_inters_ray __(( T2_LOOP, PT2, PT2, INT, 
            PT2, T2_PARM )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2b_recut_region ( region, rad, region_list )
/* Attention : this routine can be applied only to regions resulting 
from a call to t2b_leftover */
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
REAL rad ;
DML_LIST region_list ;
{
    DML_ITEM item, last ;
    DML_LIST looplist ;
    T2_REGION region1 ;

    if ( region_list == NULL ) 
        region_list = dml_create_list();
    if ( region_list == NULL ) 
        RETURN ( NULL ) ;
    looplist = dml_create_list();
    if ( looplist == NULL ) 
        RETURN ( NULL ) ;

    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) {
        if ( t2b_recut_loop ( DML_RECORD(item), rad, looplist ) 
            == NULL )
            RETURN ( NULL ) ;
    }

    last = DML_LAST(region_list) ;
    region_list = t2b_offset_rgn_pass1 ( looplist, TRUE, region_list ) ;
    last = ( last == NULL ) ? DML_FIRST(region_list) : DML_NEXT(last) ;
    for ( item = last ; item != NULL ; item = DML_NEXT(item) ) {
        region1 = (T2_REGION)DML_RECORD(item) ;
        T2_REGION_PARENT(region1) = region ;
    }
#ifdef __BRLNDC__
	RETURN ( NULL ) ;
#endif
#ifdef __WATCOM__
	RETURN ( NULL ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2b_recut_loop ( loop, rad, result )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL rad ;
DML_LIST result ;
{
    DML_LIST loop0list, looplist ;
    T2_LOOP loop1, parent ;
    DML_ITEM last, item, item1 ;

#ifdef CCDK_DEBUG
if ( DIS_LEVEL == -2 )
    t2c_write_loop ( "loop.dat", loop ) ;
#endif
    if ( result == NULL ) 
        result = dml_create_list() ;

    loop0list = t2c_break_loop_by_attr ( loop ) ;
#ifdef CCDK_DEBUG
{
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        DML_WALK_LIST ( loop0list, item ) {
            paint_loop ( (T2_LOOP)DML_RECORD(item), 10 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
}
#endif
    looplist = t2b_recut_offset ( loop0list, rad ) ;
    dml_destroy_list ( loop0list, ( PF_ACTION ) t2d_free_loop ) ;
    loop0list = NULL ;

    last = DML_LAST(result) ;
    result = t2b_recut_connect ( looplist, result ) ;
    dml_destroy_list ( looplist, ( PF_ACTION ) t2d_free_loop ) ;
    looplist = NULL ;
    last = ( last == NULL ) ? DML_FIRST(result) : DML_NEXT(last) ;
    parent = ( T2_LOOP_PARENT(loop) == NULL ) ? loop : T2_LOOP_PARENT(loop) ;

    for ( item = last ; item != NULL ; item = item1 ) {
        item1 = DML_NEXT(item) ;
        loop1 = DML_RECORD(item) ;
        t2d_adjust_epts_loop ( loop1, t2c_get_gap_coeff () * BBS_TOL ) ;
        T2_LOOP_PARENT(loop1) = parent ;
        T2_LOOP_ATTR(loop1) = T2_LOOP_ATTR(loop1) | T2_LOOP_ATTR(loop) ;
    }
#ifdef CCDK_DEBUG
{
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        DML_WALK_LIST ( result, item ) {
            paint_loop ( (T2_LOOP)DML_RECORD(item), 11 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
    if ( DIS_LEVEL == -2 && T2_LOOP_CLOSED(loop) ) {
        DML_WALK_LIST ( result, item ) {
            if ( !T2_LOOP_CLOSED((T2_LOOP)DML_RECORD(item)) ) {
                t2c_write_loop ( "loop0.dat", loop ) ;
                t2c_write_looplist ( "loops.dat", result ) ;
            }
        }
    }
}
#endif
    RETURN ( result ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_LIST t2b_recut_offset ( loop0list, rad )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST loop0list ;
REAL rad ;
{
    DML_LIST looplist ;
    DML_ITEM item ;
    T2_LOOP loop ;

    looplist = dml_create_list () ;

    DML_WALK_LIST ( loop0list, item ) {
        loop = (T2_LOOP)DML_RECORD(item) ;
        if ( T2_LOOP_ATTR(loop) & T2_ATTR_PART )
            t2b_offset_loop ( loop, -rad, NULL, looplist ) ;
        else if ( T2_LOOP_ATTR(loop) & T2_ATTR_ROUGH )
            t2b_offset_loop ( loop, rad, NULL, looplist ) ;
    }
    RETURN ( looplist ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_LIST t2b_recut_connect ( looplist, result )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
DML_LIST result ;
{
    T2_LOOP loop0, loop ;
    DML_ITEM item, last ;
    BOOLEAN first_part ;

    if ( ( looplist == NULL ) || ( DML_LENGTH(looplist) == 0 ) ) 
        RETURN ( result ) ;
    if ( result == NULL )
        result = dml_create_list () ;
    last = DML_LAST(result) ;

    loop0 = NULL ;
    loop = DML_FIRST_RECORD(looplist) ;
    first_part = ( T2_LOOP_ATTR(loop) & T2_ATTR_PART ) ;

    DML_WALK_LIST ( looplist, item ) { 
        loop = DML_RECORD(item) ;
        if ( !t2b_recut_connect_2loops ( loop0, loop, TRUE, TRUE ) ) {
            loop0 = t2d_copy_loop ( loop, NULL, NULL ) ;
            dml_append_data ( result, loop0 ) ;
            T2_LOOP_ATTR(loop0) = T2_LOOP_ATTR(loop0) & ~T2_ATTR_PART ;
            T2_LOOP_ATTR(loop0) = T2_LOOP_ATTR(loop0) & ~T2_ATTR_ROUGH ;
        }
    }
    last = ( last == NULL ) ? DML_FIRST(result) : DML_NEXT(last) ;
    t2b_recut_connect_2loops ( loop0, DML_RECORD(last), FALSE, first_part ) ;
    RETURN ( result ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_recut_connect_2loops ( loop0, loop1, append, first_part )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0, loop1 ;
BOOLEAN append, first_part ;
{
    PT2 pt0, pt1, tan0, tan1, int_pt ;
    BOOLEAN status, part1, trim0, trim1 ;
    C2_CURVE curve ;
    T2_EDGE edge ;
    T2_LOOP loop ;
    T2_PARM_S int_parm ;

    if ( loop0 == NULL ) 
        RETURN ( FALSE ) ;
    t2c_loop_ept_tan1 ( loop0, pt0, tan0 ) ;
    t2c_loop_ept_tan0 ( loop1, pt1, tan1 ) ;
    if ( append ) 
        part1 = ( T2_LOOP_ATTR(loop1) & T2_ATTR_PART ) ;
    else
        part1 = first_part ;
#ifdef CCDK_DEBUG
{
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_loop ( loop0, 11 ) ;
        getch();
        paint_loop ( loop1, 12 ) ;
        getch();
    }
    else
        DISPLAY-- ;
}
#endif

    if ( C2V_IDENT_PTS ( pt0, pt1 ) ) {
        status = TRUE ;
        trim0 = FALSE ;
        trim1 = FALSE ;
    }
    else {
        if ( part1 ) {
            if ( t2b_recut_inters_ray ( loop0, pt1, tan1, -1, 
                int_pt, &int_parm ) ) {
                curve = c2d_line ( int_pt, pt1 ) ;
                trim0 = TRUE ;
            }
            else {
                curve = c2d_arc_2pts_tan1 ( pt0, pt1, tan1 ) ;
                trim0 = FALSE ;
            }
            trim1 = FALSE ;
        }

        else {
            if ( t2b_recut_inters_ray ( loop1, pt0, tan0, 1, 
                int_pt, &int_parm ) ) {
                curve = c2d_line ( pt0, int_pt ) ;
                trim1 = TRUE ;
            }
            else {
                curve = c2d_arc_2pts_tan ( pt0, tan0, pt1 ) ;
                trim1 = FALSE ;
            }
            trim0 = FALSE ;
        }
        status = ( curve != NULL ) ;
        if ( status ) {
            if ( trim0 )
                t2c_trim_loop ( loop0, NULL, NULL, 
                    T2_PARM_EDGE(&int_parm), T2_PARM_CPARM(&int_parm) ) ;
            edge = t2d_create_edge ( loop0, curve, 1) ;
            t2d_append_edge ( loop0, edge, FALSE ) ;
#ifdef CCDK_DEBUG
{
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_edge ( edge, 13 ) ;
        getch();
    }
    else
        DISPLAY-- ;
}    
#endif
        }
    }

    if ( status ) {
        if ( append ) {
            if ( trim1 ) {
                loop = t2d_copy_loop ( loop1, T2_PARM_EDGE(&int_parm), NULL ) ;
                edge = t2c_first_edge ( loop ) ;
                t2c_trim_edge ( edge, T2_PARM_CPARM(&int_parm), NULL ) ;
            }
            else
                loop = t2d_copy_loop ( loop1, NULL, NULL ) ;
            t2d_append_loop ( loop0, loop ) ;
            t2d_free_loop ( loop ) ;
        }
        else if ( trim1 ) 
            t2c_trim_loop ( loop1, T2_PARM_EDGE(&int_parm), 
                T2_PARM_CPARM(&int_parm), NULL, NULL ) ;
    }

#ifdef CCDK_DEBUG
{
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_loop ( loop0, 14 ) ;
        getch();
    }
    else
        DISPLAY-- ;
}
#endif
    RETURN ( status ) ;
}    


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_recut_inters_ray ( loop, pt, tan, dir, 
            int_pt, int_parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt, tan, int_pt ;
INT dir ;
T2_PARM int_parm ;
{
    C2_CURVE ray ;
    DML_LIST ti_list = dml_create_list();
    DML_ITEM item ;
    T2_INT_REC ti0, ti ;
    BOOLEAN status ;

    if ( dir == -1 ) {
        C2V_NEGATE ( tan, tan ) ;
    }
    ray = c2d_line_dir ( pt, tan ) ;
    t2i_intersect_ray_loop ( ray, loop, TRUE, FALSE, TRUE, ti_list ) ;

    ti0 = NULL ;

    DML_WALK_LIST ( ti_list, item ) {
        ti = (T2_INT_REC)DML_RECORD(item) ;
        if ( T2_INT_REC_T1(ti) > BBS_ZERO ) {
            if ( ( ti0 == NULL ) || 
                ( T2_INT_REC_T1(ti) < T2_INT_REC_T1(ti0) ) ) 
                ti0 = ti ;
        }
    }

    if ( ti0 == NULL ) 
        status = FALSE ;
    else {
        C2V_COPY ( T2_INT_REC_PT(ti0), int_pt ) ;
        T2_PARM_EDGE(int_parm) = T2_INT_REC_EDGE2(ti) ;
        PARM_COPY ( T2_INT_REC_PARM2(ti), T2_PARM_CPARM(int_parm) ) ;
        status = TRUE ;
    }
    dml_destroy_list ( ti_list, ( PF_ACTION ) t2i_free_int_rec ) ;
    RETURN ( status ) ;
}
#endif /* ( __BBS_MILL__>=3 ) */

