/* -S __BBS_MILL__=1 */
/********************************* T2PS.C **********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h>
#include <t2cdefs.h>
#include <t2cpriv.h>
#include <t2ddefs.h>
#include <t2bpriv.h>
#include <t2pdefs.h>
#include <t2attrd.h>
#include <t2sprlm.h>
#include <c2vmcrs.h>
#if ( __BBS_MILL__>=1 )

#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
GLOBAL INT T2P_SPIRAL0 = 0 ;
GLOBAL INT T2P_SPIRAL1 = 0 ;
REAL   T2_SPIRAL_INIT_PT = 1.0 ;
#endif
STATIC BOOLEAN t2p_new_spiral __(( T2_SPIRAL, DML_LIST, 
            REAL, REAL, INT )) ; 
STATIC BOOLEAN t2p_spiral1 __(( T2_REGION, REAL, REAL, INT, T2_SPIRAL )) ; 
STATIC void t2p_spiral_split __(( T2_SPIRAL, REAL, PT2 )) ; 
STATIC void t2p_spiral_connect __(( T2_SPIRAL, REAL, PT2, BOOLEAN )) ; 
STATIC T2_SPIRAL t2p_spiral_interior_init __(( T2_SPIRAL )) ;
STATIC T2_SPIRAL t2p_spiral_pt_pos __(( T2_SPIRAL, PT2 )) ; 
STATIC DML_ITEM t2p_spiral_loop_pt __(( T2_SPIRAL, REAL, PT2 )) ; 
STATIC BOOLEAN t2p_loop_rearrange_mid __(( T2_LOOP, PT2 )) ;
STATIC T2_LOOP t2p_spiral_ext_loop __(( T2_SPIRAL )) ; 

/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_SPIRAL t2p_spiral ( region, step, rad, pt, in_out, cut_mode ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ; 
REAL step, rad ;
PT2 pt ;
BOOLEAN in_out ;
INT cut_mode ;
{
    T2_SPIRAL spiral ;
    T2_REGION copy ;

    copy = t2d_copy_region ( region ) ;
/*
    t2c_mark_loop ( T2_REGION_EXT_LOOP(copy), T2_ATTR_EXTERIOR ) ;
*/
    spiral = t2p_spiral0 ( copy, step, rad, cut_mode ) ;

    t2d_free_region ( copy ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_spiral ( spiral, 9, FALSE ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
    if ( pt != NULL ) 
        t2p_spiral_split ( spiral, step, pt ) ;
    t2p_mark_spiral ( spiral, T2_ATTR_INIT ) ;
    t2p_spiral_connect ( spiral, step, pt, in_out ) ;
    RETURN ( spiral ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_SPIRAL t2p_spiral0 ( region, step, rad, cut_mode ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ; 
REAL step, rad ;
INT cut_mode ;
{
    T2_SPIRAL spiral ;
    T2_LOOP loop0 ;

#ifdef CCDK_DEBUG
    INT t2p_spiral0_c = T2P_SPIRAL0 ;
    char filename[12] ;
    T2P_SPIRAL0++ ;
    if ( DIS_LEVEL == -2 ) {
        sprintf ( filename, "spr0%d.dat", t2p_spiral0_c ) ; 
        t2c_write_region ( filename, region ) ;
    }
#endif

    loop0 = t2d_copy_loop ( T2_REGION_EXT_LOOP(region), NULL, NULL ) ;
    if ( loop0 == NULL ) 
        RETURN ( NULL ) ;

    spiral = t2p_create_spiral () ;
    t2p_append_spiral_loop ( spiral, loop0 ) ;
    if ( !t2p_spiral1 ( region, step, rad, cut_mode, spiral ) ) {
        t2p_free_spiral ( spiral ) ;
        RETURN ( NULL ) ;
    }
    RETURN ( spiral ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2p_spiral1 ( region, step, rad, cut_mode, spiral ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ; 
REAL step, rad ;
INT cut_mode ;
T2_SPIRAL spiral ;
{
    T2_SPIRAL sub_spiral ;
    DML_LIST offset_list ;
    DML_ITEM item, item1 ;
    T2_REGION offset ;
    BOOLEAN status, aux_offset ;
    T2_LOOP loop0 ;

#ifdef CCDK_DEBUG
    INT t2p_spiral1_c = T2P_SPIRAL1 ;
    char filename[12] ;
    INT ii ;
    T2P_SPIRAL1++ ;
    DISPLAY++ ;

    if ( DIS_LEVEL == -1 || DIS_LEVEL == -3 || DISPLAY <= DIS_LEVEL ) {
        paint_region ( region, 5 ) ;
        if ( DIS_LEVEL != -3 )
            getch () ;
        paint_region ( region, 11 ) ;
    }
    else
        DISPLAY-- ;
    if ( DIS_LEVEL == -2 ) {
        sprintf ( filename, "spr1%d.dat", t2p_spiral1_c ) ; 
        t2c_write_region ( filename, region ) ;
    }
#endif
    offset_list = t2b_offset_extloop ( region, -step, NULL, NULL ) ;
//#if ( __BBS_MILL__ >= 3 )
#if ( __BBS_MILL__ > 3 )
    if ( dml_length ( offset_list ) == 0 && !IS_SMALL(rad) && ( rad < step ) )
        offset_list = t2p_offset ( region, 0.0, rad, offset_list ) ;
#endif  /* __BBS_MILL__ > 3 */

#ifdef CCDK_DEBUG
{
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DIS_LEVEL == -3 || DISPLAY <= DIS_LEVEL ) {
        DML_WALK_LIST ( offset_list, item ) {
            paint_region ( DML_RECORD(item), 6 ) ;
            if ( DIS_LEVEL != -3 )
                getch () ;
            paint_region ( DML_RECORD(item), 12 ) ;
        }
    }
    else
        DISPLAY-- ;
    if ( DIS_LEVEL == -2 ) {
        ii = 0 ;
        DML_WALK_LIST ( offset_list, item ) {
            sprintf ( filename, "spr1%d%d.dat", t2p_spiral1_c, ii ) ; 
            t2c_write_region ( filename, DML_RECORD(item) ) ;
            ii++ ;
        }
    }
}
#endif

    if ( offset_list == NULL ) 
        RETURN ( TRUE ) ;

    aux_offset = FALSE ;

    for ( item = DML_FIRST(offset_list) ; item != NULL ; item = item1 ) {
        item1 = DML_NEXT(item) ;
        offset = DML_RECORD(item) ;
        loop0 = T2_REGION_EXT_LOOP(offset) ;
        if ( !T2_LOOP_CLOSED(loop0) ) {
            t2d_free_region ( offset ) ;
            dml_remove_item ( offset_list, item ) ;
        }
#if (__BBS_MILL__>=3)
        if ( cut_mode && !aux_offset )
            aux_offset = t2p_sharp_loop ( loop0, step, rad ) ;
#endif /* __BBS_MILL__>=3 */
    }

    if ( DML_LENGTH(offset_list) == 0 ) {
        if ( DML_LENGTH ( T2_REGION_LOOP_LIST(region) ) == 2 )  
            t2p_append_spiral_loop ( spiral, t2d_copy_loop ( 
                DML_LAST_RECORD(T2_REGION_LOOP_LIST(region)), NULL, NULL ) ) ;
        else if ( DML_LENGTH ( T2_REGION_LOOP_LIST(region) ) > 2 )  {
            DML_FOR_LOOP ( DML_SECOND(T2_REGION_LOOP_LIST(region)), item ) {
                sub_spiral = t2p_create_spiral () ;
                t2p_append_spiral_loop ( sub_spiral, 
                    t2d_copy_loop ( DML_RECORD(item), NULL, NULL ) ) ;
                dml_append_data ( T2_SPIRAL_LIST(spiral), sub_spiral ) ;
            }
        }
        dml_free_list ( offset_list ) ;
        RETURN ( TRUE ) ;
    }

    else if ( DML_LENGTH(offset_list) == 1 ) {
        offset = DML_FIRST_RECORD(offset_list) ;
#if (__BBS_MILL__>=3)
        if ( aux_offset && !t2p_spiral_extend ( T2_REGION_EXT_LOOP(offset), 
            T2_REGION_EXT_LOOP(region), step, rad ) ) {
            dml_destroy_list ( offset_list, ( PF_ACTION ) t2d_free_region ) ;
            offset_list = t2p_offset ( region, step, rad, NULL ) ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DIS_LEVEL == -3 || DISPLAY <= DIS_LEVEL ) {
        paint_regionlist ( offset_list, 5 ) ;
        if ( DIS_LEVEL != -3 )
            getch () ;
        paint_region ( region, 11 ) ;
    }
    else
        DISPLAY-- ;
#endif
            aux_offset = FALSE ;
        }
#endif
        if ( DML_LENGTH(offset_list) == 1 ) {
#if (__BBS_MILL__>=3)
            if ( cut_mode ) 
                t2p_leftover_1 ( region, offset_list, rad, cut_mode, spiral ) ;
#endif /* __BBS_MILL__>=3 */
            offset = DML_FIRST_RECORD(offset_list) ;
            t2p_append_spiral_loop ( spiral, T2_REGION_EXT_LOOP(offset) ) ;
            dml_free_list ( offset_list ) ;
            status = t2p_spiral1 ( offset, step, rad, cut_mode, spiral ) ;

            DML_FOR_LOOP ( dml_second(T2_REGION_LOOP_LIST(offset)), item )
                t2d_free_loop ( DML_RECORD(item) ) ;
            t2d_clear_region ( offset ) ;
            offset = NULL ;
            RETURN ( status ) ;
        }
    }

#if (__BBS_MILL__>=3)
    if ( cut_mode ) 
        t2p_leftover ( region, offset_list, rad, cut_mode, spiral ) ;
#endif /* __BBS_MILL__>=3 */

    if ( !t2p_new_spiral ( spiral, offset_list, step, rad, cut_mode ) ) 
        RETURN ( FALSE ) ;
    dml_free_list ( offset_list ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2p_spiral_split ( spiral, step, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral ;
REAL step ;
PT2 pt ;
{
    T2_SPIRAL spiral0, spiral1 ;
    DML_ITEM item ;

    spiral0 = t2p_spiral_pt_pos ( spiral, pt ) ;
    if ( spiral0 == NULL ) 
        RETURN ;
    item = t2p_spiral_loop_pt ( spiral0, step, pt ) ;
    if ( item != NULL ) {
        spiral1 = t2p_create_spiral () ;
        T2_SPIRAL_LOOP_LIST(spiral1) = 
            dml_append_chain ( T2_SPIRAL_LOOP_LIST(spiral0), 
                item, NULL, NULL ) ;
        T2_SPIRAL_LIST(spiral1) = T2_SPIRAL_LIST(spiral0) ;
        T2_SPIRAL_LIST(spiral0) = dml_create_list () ;
        dml_append_data ( T2_SPIRAL_LIST(spiral0), spiral1 ) ;
        T2_SPIRAL_PARENT(spiral1) = spiral0 ;
        t2p_spiral_set_owner ( spiral1 ) ;
    }
    T2_SPIRAL_LOOP_INIT(spiral) = 
        dml_last_record ( T2_SPIRAL_LOOP_LIST(spiral0) ) ; 
}


/*-------------------------------------------------------------------------*/
STATIC T2_SPIRAL t2p_spiral_pt_pos ( spiral, pt ) 
/* the smallest spiral surrounding the point */
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral ;
PT2 pt ;
{
    T2_PT_POSITION pos ;
    T2_SPIRAL spiral1 ;
    DML_ITEM item ;
    T2_LOOP loop ;

    if ( spiral == NULL ) 
        RETURN ( NULL ) ;

    if ( T2_SPIRAL_LOOP_LIST(spiral) != NULL ) {
        loop = t2p_spiral_ext_loop ( spiral ) ;
        if ( loop == NULL ) 
            RETURN ( NULL ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop, 9 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
        pos = t2c_pt_pos_loop ( loop, pt ) ;
        if ( pos == T2_PT_OUTSIDE ) 
            RETURN ( NULL ) ;
        else if ( pos == T2_PT_ON_BOUNDARY ) 
            RETURN ( spiral ) ;
    }

    if ( T2_SPIRAL_LIST(spiral) == NULL ) 
        RETURN ( spiral ) ;

    DML_WALK_LIST ( T2_SPIRAL_LIST(spiral), item ) {
        spiral1 = t2p_spiral_pt_pos ( DML_RECORD(item), pt ) ;
        if ( spiral1 != NULL ) 
            RETURN ( spiral1 ) ;
    }
    RETURN ( spiral ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_LOOP t2p_spiral_ext_loop ( spiral ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral ;
{
    T2_LOOP loop ;
    DML_ITEM item ;

    if ( spiral == NULL )
        RETURN ( NULL ) ;
    else if ( T2_SPIRAL_LOOP_LIST(spiral) != NULL ) 
        RETURN ( DML_FIRST_RECORD ( T2_SPIRAL_LOOP_LIST(spiral) ) ) ;
    DML_WALK_LIST ( T2_SPIRAL_LIST(spiral), item ) {
        loop = t2p_spiral_ext_loop ( DML_RECORD(item) ) ;
        if ( loop != NULL ) 
            RETURN ( loop ) ;
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_ITEM t2p_spiral_loop_pt ( spiral, step, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral ;
REAL step ;
PT2 pt ;
{
    DML_ITEM item, item1 ;
    T2_LOOP loop ;
    T2_PT_POSITION pos = T2_PT_UNKNOWN ;
    PT2 p1 ;

    if ( ( spiral == NULL ) || ( T2_SPIRAL_LOOP_LIST(spiral) == NULL ) )
        RETURN ( NULL ) ;
    DML_WALK_LIST ( T2_SPIRAL_LOOP_LIST(spiral), item ) {
        loop = DML_RECORD(item) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop, 10 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
        pos = t2c_pt_pos_loop ( loop, pt ) ;
        if ( pos == T2_PT_OUTSIDE ) 
            RETURN ( item ) ;
        else if ( pos == T2_PT_ON_BOUNDARY ) {
            item1 = DML_NEXT(item) ;
            if ( ( DML_PREV(item) != NULL ) && ( item1 != NULL ) && 
                ( !t2c_project_loop ( DML_RECORD(item1), pt, FALSE, NULL, p1 ) 
                || C2V_DIST ( pt, p1 ) > step + BBS_TOL ) ) 
            RETURN ( item ) ;
        }
    }
/*
    if ( ( pos == T2_PT_ON_BOUNDARY ) && 
        ( dml_length ( T2_SPIRAL_LIST(spiral) ) > 0 ) ) 
        RETURN ( DML_LAST(T2_SPIRAL_LOOP_LIST(spiral)) ) ; 
    else
*/
        RETURN ( NULL ) ; 
}


/*-------------------------------------------------------------------------*/
STATIC void t2p_spiral_connect ( spiral, step, pt, in_out ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral ;
REAL step ;
PT2 pt ;
BOOLEAN in_out ;
{
    T2_SPIRAL spiral0 ;
    T2_LOOP loop0 ;
    PT2 p0 ;
    BOOLEAN p0_status ;

#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_spiral ( spiral, 9, FALSE ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
    if ( T2_SPIRAL_LOOP_INIT(spiral) != NULL ) {
        loop0 = T2_SPIRAL_LOOP_INIT(spiral) ;
        if ( ( ( pt == NULL ) || 
            !t2c_loop_rearrange_pt ( loop0, pt, TRUE, NULL ) ) && 
            !t2c_loop_ept_smooth ( loop0 ) )
            t2p_loop_rearrange_mid ( loop0, pt ) ;
    }
    else {
        spiral0 = t2p_spiral_interior_init ( spiral ) ;
        loop0 = ( spiral0 == NULL ) ? NULL : 
            dml_last_record ( T2_SPIRAL_LOOP_LIST ( spiral0 ) ) ;
    }

    if ( loop0 != NULL ) 
        T2_LOOP_ATTR(loop0) = T2_LOOP_ATTR(loop0) | T2_ATTR_PROCSD_0 ;
    p0_status = t2p_spiral_connect_out ( loop0, 
        step, p0, ( pt != NULL ) || in_out ) ;
    t2p_spiral_connect_in ( spiral, step, p0, p0_status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_SPIRAL t2p_spiral_interior ( spiral ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral ;
{
    T2_SPIRAL spiral0 ;
    spiral0 = dml_first_record ( T2_SPIRAL_LIST(spiral) ) ;
    RETURN ( ( spiral0 == NULL ) ? 
        spiral : t2p_spiral_interior ( spiral0 ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_SPIRAL t2p_spiral_interior_init ( spiral ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral ;
{
    T2_SPIRAL spiral0 ;
    DML_ITEM item ;
    T2_LOOP loop0 ;

    if ( T2_SPIRAL_LIST(spiral) != NULL ) {
        DML_WALK_LIST ( T2_SPIRAL_LIST(spiral), item ) {
            spiral0 = t2p_spiral_interior_init ( DML_RECORD(item) ) ;
            if ( spiral0 != NULL ) {
                dml_make_first ( T2_SPIRAL_LIST(spiral), item ) ;
                RETURN ( spiral0 ) ;
            }
        }
        RETURN ( NULL ) ;
    }
    loop0 = dml_last_record ( T2_SPIRAL_LOOP_LIST(spiral) ) ;
    RETURN ( t2p_loop_rearrange_mid ( loop0, NULL ) ? spiral : NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_SPIRAL t2p_create_spiral __(( void )) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    T2_SPIRAL spiral = MALLOC ( 1, T2_SPIRAL_S ) ;
    if ( spiral != NULL ) {
        T2_SPIRAL_LOOP_LIST(spiral) = NULL ;
        T2_SPIRAL_OPEN_LIST(spiral) = NULL ;
        T2_SPIRAL_LIST(spiral) = NULL ;
        T2_SPIRAL_PARENT(spiral) = NULL ;
        T2_SPIRAL_LOOP_INIT(spiral) = NULL ;
        C2V_SET_ZERO ( T2_SPIRAL_PT(spiral) ) ;
        T2_SPIRAL_EDGE0(spiral) = NULL ;
        T2_SPIRAL_EDGE1(spiral) = NULL ;
        T2_SPIRAL_T0(spiral) = 0.0 ;
        T2_SPIRAL_T1(spiral) = 0.0 ;
        T2_SPIRAL_J0(spiral) = 0 ;
        T2_SPIRAL_J1(spiral) = 0 ;
    }
    RETURN ( spiral ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_free_spiral ( spiral ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral ;
{
    if ( spiral != NULL ) {
        dml_destroy_list ( T2_SPIRAL_LIST(spiral), 
        				   ( PF_ACTION ) t2p_free_spiral ) ;
        dml_destroy_list ( T2_SPIRAL_LOOP_LIST(spiral), 
        				   ( PF_ACTION ) t2d_free_loop ) ;
        dml_destroy_list ( T2_SPIRAL_OPEN_LIST(spiral), 
        				   ( PF_ACTION ) t2d_free_loop ) ;
        FREE ( spiral ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_clear_spiral ( spiral ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral ;
{
    if ( spiral != NULL ) {
        dml_apply ( T2_SPIRAL_LIST(spiral), ( PF_ACTION ) t2p_clear_spiral ) ;
        dml_free_list ( T2_SPIRAL_LIST(spiral) ) ;
        T2_SPIRAL_LIST(spiral) = NULL ;
        dml_free_list ( T2_SPIRAL_LOOP_LIST(spiral) ) ;
        T2_SPIRAL_LOOP_LIST(spiral) = NULL ;
        dml_free_list ( T2_SPIRAL_OPEN_LIST(spiral) ) ;
        T2_SPIRAL_OPEN_LIST(spiral) = NULL ;
        FREE ( spiral ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_append_spiral_loop ( spiral, loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral ;
T2_LOOP loop ;
{
    T2_LOOP loop0 ;

    if ( T2_SPIRAL_LOOP_LIST(spiral) == NULL ) 
        T2_SPIRAL_LOOP_LIST(spiral) = dml_create_list () ;
    loop0 = dml_last_record ( T2_SPIRAL_LOOP_LIST(spiral) ) ;
    dml_append_data ( T2_SPIRAL_LOOP_LIST(spiral), loop ) ;
    T2_LOOP_PARENT(loop) = loop0 ;
    T2_LOOP_SPIRAL(loop) = spiral ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2p_new_spiral ( spiral, region_list, step, rad, cut_mode ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral ;
DML_LIST region_list ;
REAL step, rad ;
INT cut_mode ;
{
    DML_ITEM item ;
    T2_REGION region ;
    T2_SPIRAL sub_spiral ;

    DML_WALK_LIST ( region_list, item ) {
        region = DML_RECORD(item) ;
        sub_spiral = t2p_spiral0 ( region, step, rad, cut_mode ) ;
        if ( sub_spiral == NULL ) 
            RETURN ( FALSE ) ;
        t2p_append_spiral ( spiral, sub_spiral ) ;
        t2d_free_region ( region ) ;
        region = NULL ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_append_spiral ( spiral, sub_spiral ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral, sub_spiral ;
{
    T2_LOOP loop0, loop1 ;

    if ( T2_SPIRAL_LIST(spiral) == NULL ) 
        T2_SPIRAL_LIST(spiral) = dml_create_list();
    if ( T2_SPIRAL_LIST(spiral) == NULL ) 
        RETURN ;
    loop0 = dml_last_record ( T2_SPIRAL_LOOP_LIST(spiral) ) ;
    T2_SPIRAL_PARENT(sub_spiral) = spiral ;
    loop1 = dml_first_record ( T2_SPIRAL_LOOP_LIST(sub_spiral) ) ;
    T2_LOOP_PARENT(loop1) = loop0 ;
    dml_append_data ( T2_SPIRAL_LIST(spiral), sub_spiral ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_mark_spiral ( spiral, s ) 
/*-------------------------------------------------------------------------*/
/* changes edges status values from s0 to s1 */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral ;
ATTR s ;
{
    DML_ITEM item ;
    T2_LOOP loop ;

    if ( spiral == NULL )
        RETURN ; 
    if ( T2_SPIRAL_LOOP_LIST(spiral) != NULL ) { 
        DML_WALK_LIST ( T2_SPIRAL_LOOP_LIST(spiral), item ) {
            loop = DML_RECORD(item) ;
            if ( s == T2_ATTR_INIT )
                T2_LOOP_ATTR(loop) = s ;
            else
                T2_LOOP_ATTR(loop) = T2_LOOP_ATTR(loop) | s ;
        }
    }
    if ( T2_SPIRAL_LIST(spiral) != NULL ) { 
        DML_WALK_LIST ( T2_SPIRAL_LIST(spiral), item ) {
            t2p_mark_spiral ( DML_RECORD(item), s ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2p_spiral_int_pt ( spiral, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral ;
PT2 pt ;
{
    T2_SPIRAL spiral0 ;
    T2_LOOP loop ; 

    if ( T2_SPIRAL_LOOP_INIT(spiral) != NULL ) 
        RETURN ( t2c_loop_ept0 ( T2_SPIRAL_LOOP_INIT(spiral), pt ) ) ;
    else {
        spiral0 = t2p_spiral_interior ( spiral ) ;
        if ( spiral0 == NULL ) 
            RETURN ( FALSE ) ;
        loop = dml_last_record ( T2_SPIRAL_LOOP_LIST(spiral0) ) ;
        RETURN ( t2c_loop_ept0 ( loop, pt ) ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2p_spiral_ext_pt ( spiral, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral ;
PT2 pt ;
{
    T2_LOOP loop ; 

    loop = dml_first_record ( T2_SPIRAL_LOOP_LIST(spiral) ) ;
    RETURN ( t2c_loop_ept0 ( loop, pt ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2p_loop_rearrange_mid ( loop, pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt ;
{
    T2_PARM_S parm ;
    REAL dist, dist0 = 0.0 ;
    PT2 p ;
    DML_ITEM item ;
    T2_EDGE edge, edge1 ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ||
        DML_LENGTH ( T2_LOOP_EDGE_LIST(loop) ) == 0 ) 
        RETURN ( FALSE ) ;
    if ( pt == NULL ) {
        edge = NULL ;
        for ( item = DML_FIRST(T2_LOOP_EDGE_LIST(loop)) ; 
            item != NULL && edge == NULL ; item = DML_NEXT(item) ) {
            edge1 = DML_RECORD(item) ;
            if ( ( T2_EDGE_ATTR(edge1) & T2_ATTR_ROUGH ) /* &&
                 ( T2_EDGE_ATTR(edge1) & T2_ATTR_EXTERIOR ) */ )
                edge = edge1 ;
        }
/*
        for ( item = DML_FIRST(T2_LOOP_EDGE_LIST(loop)) ; 
            item != NULL && edge == NULL ; item = DML_NEXT(item) ) {
            edge1 = DML_RECORD(item) ;
            if ( T2_EDGE_ATTR(edge1) & T2_ATTR_EXTERIOR ) 
                edge = edge1 ;
        }
*/
    }
    else {
        DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) { 
            edge1 = DML_RECORD(item) ;
            if ( ( T2_EDGE_ATTR(edge1) & T2_ATTR_ROUGH ) /* &&
                 ( T2_EDGE_ATTR(edge1) & T2_ATTR_EXTERIOR ) */ ) {
                t2c_mid_pt ( edge1, p ) ;
                dist = C2V_DIST ( p, pt ) ;
                if ( ( edge == NULL ) || ( dist < dist0 ) ) {
                    dist0 = dist ;
                    edge = edge1 ;
                }
            }
/*
            if ( T2_EDGE_ATTR(edge1) & T2_ATTR_EXTERIOR ) {
                t2c_mid_pt ( edge1, p ) ;
                dist = C2V_DIST ( p, pt ) ;
                if ( ( edge == NULL ) || ( dist < dist0 ) ) {
                    dist0 = dist ;
                    edge = edge1 ;
                }
            }
*/
        }
    }
    if ( edge == NULL ) 
        RETURN ( FALSE ) ;
    T2_PARM_EDGE(&parm) = edge ;
    T2_PARM_T(&parm) = 0.5 * ( T2_EDGE_T0(edge) + T2_EDGE_T1(edge) ) ;
    t2c_parm_adjust ( edge, T2_PARM_CPARM(&parm) ) ;
    RETURN ( t2c_loop_rearrange_parm ( loop, &parm ) ) ;
}
#endif /* __BBS_MILL__>=1 */

