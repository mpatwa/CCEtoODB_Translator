/* -S __BBS_MILL__=1 */
/********************************* T2PS1.C *********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h>
#include <t2cdefs.h>
#include <t2cpriv.h>
#include <t2pdefs.h>
#include <t2attrd.h>
#include <t2sprlm.h>
#include <c2vmcrs.h>

#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif
#if ( __BBS_MILL__>=1 )

STATIC void t2p_spiral_connect_out1 __(( T2_LOOP, T2_LOOP, REAL, BOOLEAN, 
            PT2, PT2 )) ;
STATIC BOOLEAN t2p_spiral_connect_out2 __(( T2_LOOP, T2_LOOP, REAL, 
            PT2, PT2, T2_PARM )) ;
STATIC BOOLEAN t2p_spiral_connect_out3 __(( T2_LOOP, REAL, PT2, PT2 )) ;
STATIC BOOLEAN t2p_spiral_connect_out4 __(( T2_LOOP, REAL, PT2 )) ;
STATIC void t2p_spiral_connect_in1 __(( T2_SPIRAL, REAL, PT2, BOOLEAN )) ; 
STATIC BOOLEAN t2p_spiral_sort_key __(( T2_SPIRAL, T2_SPIRAL, PT2 )) ; 
STATIC REAL    t2p_spiral_order_key __(( T2_SPIRAL, T2_SPIRAL )) ; 
STATIC BOOLEAN t2p_loop_rearrange __(( T2_LOOP, PT2, BOOLEAN, REAL, PT2 )) ;
STATIC BOOLEAN t2p_loop_rearrange1 __(( T2_LOOP, REAL, PT2 )) ;
STATIC void    t2p_spiral_connect_split __(( T2_LOOP, T2_LOOP, PT2, PT2 )) ;
STATIC T2_LOOP t2p_spiral_next_loop __(( T2_LOOP, DML_ITEM* )) ;
STATIC T2_EDGE t2p_loop_rearrange2 __(( T2_LOOP, REAL, INT, PARM )) ;
STATIC T2_SPIRAL t2p_spiral_last __(( T2_SPIRAL )) ;
#ifdef CCDK_DEBUG
INT CONNECT_OUT = 0 ;
INT CONNECT_OUT1 = 0 ;
INT CONNECT_IN = 0 ;
INT CONNECT_IN1 = 0 ;
#endif
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2p_spiral_connect_out ( loop, step, p1, in_out ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ; 
REAL step ;
PT2 p1 ;
BOOLEAN in_out ;
{
    T2_LOOP loop0, loop1 ;
    PT2 p0 ;
    BOOLEAN p1_valid ;
#ifdef CCDK_DEBUG
    INT connect_out ;
    CONNECT_OUT++ ;
    connect_out = CONNECT_OUT ;
#endif
    p1_valid = FALSE ;

    for ( loop0 = loop ; loop0 != NULL ; loop0 = loop1 ) {
        t2c_loop_ept0 ( loop0, p0 ) ;
        loop1 = T2_LOOP_PARENT(loop0) ;
        if ( loop1 == NULL ) {
            C2V_COPY ( p0, p1 ) ;
            RETURN ( TRUE ) ;
        }
        T2_LOOP_ATTR(loop1) = T2_LOOP_ATTR(loop1) | T2_ATTR_PROCSD_0 ;
        t2p_spiral_connect_out1 ( loop0, loop1, step, in_out, p0, p1 ) ;
        p1_valid = TRUE ; 
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop0, 9 ) ;
    paint_loop ( loop1, 10 ) ;
    paint_point ( p0, .02, 11 ) ;
    paint_point ( p1, .02, 12 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
        if ( T2_LOOP_SPIRAL(loop0) != T2_LOOP_SPIRAL(loop1) ) {
            C2V_COPY ( p1, T2_SPIRAL_PT(T2_LOOP_SPIRAL(loop0)) ) ;
        }
        C2V_COPY ( p1, p0 ) ;
    }
    RETURN ( p1_valid ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2p_spiral_connect_out1 ( loop0, loop1, step, in_out, p0, p1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0, loop1 ; 
REAL step ;
BOOLEAN in_out ;
PT2 p0, p1 ;
{
    T2_PARM_S parm ;
#ifdef CCDK_DEBUG
    BOOLEAN status ;
    REAL dist ;
    PT2 p2 ;
    INT connect_out1 ;
    CONNECT_OUT1++ ;
    connect_out1 = CONNECT_OUT1 ;
#endif

#ifdef CCDK_DEBUG
    status = 
#endif 
    t2c_project_loop ( loop1, p0, TRUE, &parm, p1 ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop0, 9 ) ;
    paint_loop ( loop1, 10 ) ;
    paint_point ( p0, .02, 11 ) ;
    paint_point ( p1, .02, 12 ) ;
    getch () ;
}
else
    DISPLAY-- ;
    dist = C2V_DIST ( p0, p1 ) ;
#endif
    if ( C2V_DIST ( p0, p1 ) > step + BBS_TOL ) {
        if ( in_out ) 
            t2p_spiral_connect_out2 ( loop0, loop1, step, p0, p1, &parm ) ;
        else
            t2p_spiral_connect_out3 ( loop0, step, p0, p1 ) ;
        t2p_spiral_connect_split ( loop0, loop1, p0, p1 ) ;
        if ( !in_out ) 
            t2c_loop_rearrange_pt ( loop0, p1, TRUE, NULL ) ;
#ifdef CCDK_DEBUG
if ( !in_out ) {
status = t2c_project_loop ( loop0, p1, TRUE, NULL, p2 ) ;
dist = C2V_DIST ( p1, p2 ) ;
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop0, 9 ) ;
    paint_point ( p2, .02, 12 ) ;
    getch () ;
}
else
    DISPLAY-- ;
}
#endif
    }
    t2c_loop_rearrange_parm ( loop1, &parm ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop1, 10 ) ;
    t2c_loop_ept0 ( loop1, p2 ) ;
    paint_point ( p2, .02, 13 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2p_spiral_connect_out2 ( loop0, loop1, step, p0, p1, parm0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0, loop1 ; 
REAL step ;
PT2 p0, p1 ;
T2_PARM parm0 ;
{
    PT2 q0, q1 ;
    DML_ITEM item ;
    BOOLEAN status ;
    REAL dist, dist0 ; 
    T2_PARM_S parm ;
#ifdef CCDK_DEBUG
    PT2 r ; 
    REAL d ; 
#endif

    dist = 0.0 ;
    status = FALSE ;
    if ( t2c_project_loop ( loop0, p1, TRUE, NULL, q0 ) && 
        t2c_project_loop ( loop1, q0, TRUE, &parm, q1 ) && 
        C2V_DIST ( q0, q1 ) <= step + BBS_TOL ) {
        C2V_COPY ( q1, p1 ) ;
        dist0 = dist ;
        T2_COPY_PARM ( &parm, parm0 ) ;
        status = TRUE ;
    }

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop0), item ) {
        t2c_mid_pt ( DML_RECORD(item), q0 ) ;
        dist = C2V_DIST ( q0, p0 ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop0, 9 ) ;
    paint_loop ( loop1, 10 ) ;
    paint_point ( q0, .02, 11 ) ;
    paint_point ( p0, .02, 12 ) ;
    if ( t2c_project_loop ( loop1, q0, TRUE, &parm, r ) )
        paint_point ( r, .02, 13 ) ;
    d = C2V_DIST ( q0, r ) ;
    paint_line ( q0, r, 13 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
        if ( ( !status || dist < dist0 ) && 
            t2c_project_loop ( loop1, q0, TRUE, &parm, q1 ) && 
            C2V_DIST ( q0, q1 ) <= step + BBS_TOL ) {
            C2V_COPY ( q1, p1 ) ;
            dist0 = dist ;
            T2_COPY_PARM ( &parm, parm0 ) ;
            status = TRUE ;
        }
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2p_spiral_connect_out3 ( loop0, step, p0, p1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0 ; 
REAL step ;
PT2 p0, p1 ;
{
    PT2 q0, q1 ;
    DML_ITEM item ;
    T2_PARM_S parm ;
    T2_SPIRAL spiral ;
    T2_LOOP loop ;

    if ( !t2c_project_loop ( loop0, p1, FALSE, NULL, q0 ) ||
        C2V_DIST ( q0, p1 ) > step + BBS_TOL ) 
        RETURN ( FALSE ) ;

    if ( !t2p_spiral_connect_out4 ( loop0, step, q0 ) ) 
        RETURN ( FALSE ) ;

    C2V_COPY ( q0, p0 ) ;
    spiral = T2_LOOP_SPIRAL(loop0) ;
    item = NULL ;
    for ( loop = t2p_spiral_next_loop ( loop0, &item ) ; loop != NULL ;
          loop = t2p_spiral_next_loop ( loop, &item ) ) {

        t2c_project_loop ( loop, q0, FALSE, &parm, q1 ) ;
        if ( T2_LOOP_SPIRAL(loop) != spiral ) {
            spiral = T2_LOOP_SPIRAL(loop) ;
            C2V_COPY ( q0, T2_SPIRAL_PT(spiral) ) ;
        }
        t2c_loop_rearrange_parm ( loop, &parm ) ;
        C2V_COPY ( q1, q0 ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_LOOP t2p_spiral_next_loop ( loop, item_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ; 
DML_ITEM *item_ptr ;
{
    T2_SPIRAL spiral ;

    if ( *item_ptr == NULL ) {
        spiral = T2_LOOP_SPIRAL(loop) ;
        *item_ptr = dml_find_data ( T2_SPIRAL_LOOP_LIST(spiral), loop ) ;
    }
    if ( DML_NEXT(*item_ptr) == NULL ) {
        spiral = T2_LOOP_SPIRAL(loop) ;
        if ( dml_length ( T2_SPIRAL_LIST(spiral) ) == 0 ) 
            RETURN ( NULL ) ;
        spiral = dml_first_record ( T2_SPIRAL_LIST(spiral) ) ;
        *item_ptr = dml_first ( T2_SPIRAL_LOOP_LIST(spiral) ) ;
    }
    else 
        *item_ptr = DML_NEXT(*item_ptr) ;
    RETURN ( dml_record ( *item_ptr ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2p_spiral_connect_out4 ( loop0, step, p ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0 ; 
REAL step ;
PT2 p ;
{
    DML_ITEM item ;
    T2_LOOP loop ;
    PT2 p0, p1 ;

    C2V_COPY ( p, p0 ) ;
    item = NULL ;
    for ( loop = t2p_spiral_next_loop ( loop0, &item ) ; loop != NULL ;
          loop = t2p_spiral_next_loop ( loop, &item ) ) {
        if ( !t2c_project_loop ( loop, p0, FALSE, NULL, p1 ) )
            RETURN ( FALSE ) ;
        if ( C2V_DIST ( p0, p1 ) > step + BBS_TOL ) 
            RETURN ( FALSE ) ;
        C2V_COPY ( p1, p0 ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2p_spiral_connect_split ( loop0, loop1, p0, p1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0, loop1 ;
PT2 p0, p1 ;
{
    T2_SPIRAL spiral, new0, new2 ;
    DML_ITEM item0, item1 ; 

    if ( T2_LOOP_SPIRAL(loop0) != T2_LOOP_SPIRAL(loop1) )
        RETURN ;

    spiral = T2_LOOP_SPIRAL(loop0) ;

    item0 = dml_find_data ( T2_SPIRAL_LOOP_LIST(spiral), loop0 ) ; 
    item1 = DML_NEXT(item0) ;

    if ( item1 != NULL ) {
        new2 = t2p_create_spiral () ;
        T2_SPIRAL_LOOP_LIST(new2) = 
            dml_append_chain ( T2_SPIRAL_LOOP_LIST(spiral), 
                item1, NULL, NULL ) ;
        if ( dml_length ( T2_SPIRAL_LOOP_LIST(new2) ) == 0 ) {
            dml_free_list ( T2_SPIRAL_LOOP_LIST(new2) ) ;
            T2_SPIRAL_LOOP_LIST(new2) = NULL ;
        }
        T2_SPIRAL_LIST(new2) = T2_SPIRAL_LIST(spiral) ;
        C2V_COPY ( p0, T2_SPIRAL_PT(new2) ) ;

        new0 = t2p_create_spiral () ;
        T2_SPIRAL_LOOP_LIST(new0) = dml_create_list () ;
        dml_append_data ( T2_SPIRAL_LOOP_LIST(new0), loop0 ) ;
        dml_remove_last ( T2_SPIRAL_LOOP_LIST(spiral) ) ;
        T2_SPIRAL_LIST(new0) = dml_create_list () ;
        dml_append_data ( T2_SPIRAL_LIST(new0), new2 ) ;
        C2V_COPY ( p1, T2_SPIRAL_PT(new0) ) ;

        if ( dml_length ( T2_SPIRAL_LOOP_LIST(spiral) ) == 0 ) {
            dml_free_list ( T2_SPIRAL_LOOP_LIST(spiral) ) ;
            T2_SPIRAL_LOOP_LIST(spiral) = NULL ;
        }
        T2_SPIRAL_LIST(spiral) = dml_create_list () ;
        dml_append_data ( T2_SPIRAL_LIST(spiral), new0 ) ;
        t2p_spiral_set_owner ( new0 ) ;
        t2p_spiral_set_owner ( new2 ) ;
        T2_SPIRAL_PARENT(new0) = spiral ;
    }
    else {
        new0 = t2p_create_spiral () ;
        T2_SPIRAL_LOOP_LIST(new0) = dml_create_list () ;
        dml_append_data ( T2_SPIRAL_LOOP_LIST(new0), loop0 ) ;
        dml_remove_last ( T2_SPIRAL_LOOP_LIST(spiral) ) ;
        T2_SPIRAL_LIST(new0) = T2_SPIRAL_LIST(spiral) ;
        C2V_COPY ( p1, T2_SPIRAL_PT(new0) ) ;
        if ( dml_length ( T2_SPIRAL_LOOP_LIST(spiral) ) == 0 ) {
            dml_free_list ( T2_SPIRAL_LOOP_LIST(spiral) ) ;
            T2_SPIRAL_LOOP_LIST(spiral) = NULL ;
        }
        T2_SPIRAL_LIST(spiral) = dml_create_list () ;
        dml_append_data ( T2_SPIRAL_LIST(spiral), new0 ) ;
        t2p_spiral_set_owner ( new0 ) ;
        T2_SPIRAL_PARENT(new0) = spiral ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2p_spiral_connect_in ( spiral, dist, p0, p0_valid ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral ;
REAL dist ;
PT2 p0 ;
BOOLEAN p0_valid ;
{
    DML_ITEM item ;
    T2_SPIRAL spiral0, spiral1 ;
    T2_LOOP loop ;
    PT2 p ;

#ifdef CCDK_DEBUG
    INT connect_in ;
    CONNECT_IN++ ;
    connect_in = CONNECT_IN ;
#endif
    if ( dml_length ( T2_SPIRAL_LIST(spiral) ) == 0 ) {
        t2p_spiral_connect_in1 ( spiral, dist, p0, p0_valid ) ;
        RETURN ( FALSE ) ;
    }

    DML_WALK_LIST ( T2_SPIRAL_LIST(spiral), item ) 
        p0_valid = t2p_spiral_connect_in ( DML_RECORD(item), 
            dist, p0, p0_valid ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( dml_first_record(T2_SPIRAL_LOOP_LIST(spiral)), 9 ) ;
    paint_loop ( dml_last_record(T2_SPIRAL_LOOP_LIST(spiral)), 9 ) ;
    if ( p0_valid )
        paint_point ( p0, .02, 9 ) ;
    getch () ;
    DML_WALK_LIST ( T2_SPIRAL_LIST(spiral), item ) {
        spiral0 = DML_RECORD(item) ;
        paint_loop ( dml_first_record(T2_SPIRAL_LOOP_LIST(spiral0)), 10 ) ;
        paint_point ( T2_SPIRAL_PT(spiral0), .02, 10 ) ;
        getch () ;
    }
}
else
    DISPLAY-- ;
#endif
    loop = dml_last_record ( T2_SPIRAL_LOOP_LIST(spiral) ) ;
    if ( loop != NULL ) 
        t2c_loop_ept0 ( loop, p ) ;
    else 
        C2V_COPY ( T2_SPIRAL_PT(spiral), p ) ;
    dml_sort_list_data ( T2_SPIRAL_LIST(spiral), ( PF_SORT_DATA ) t2p_spiral_sort_key, p ) ;
    spiral0 = DML_FIRST_RECORD ( T2_SPIRAL_LIST(spiral) ) ; 
    t2p_spiral_connect_in1 ( spiral, dist, T2_SPIRAL_PT(spiral0), TRUE ) ;
    spiral1 = t2p_spiral_last ( spiral0 ) ;
    dml_order_list ( T2_SPIRAL_LIST(spiral), 
    	dml_second(T2_SPIRAL_LIST(spiral)), ( PF_COMP )t2p_spiral_order_key, ( spiral1 )) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_SPIRAL t2p_spiral_last ( spiral ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral ;
{
    T2_SPIRAL spiral0 ;
    spiral0 = dml_last_record ( T2_SPIRAL_LIST(spiral) ) ;
    RETURN ( ( spiral0 == NULL ) ? spiral : t2p_spiral_last ( spiral0 ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2p_spiral_connect_in1 ( spiral, dist, p0, p0_valid ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral ;
REAL dist ;
PT2 p0 ;
BOOLEAN p0_valid ;
{
    PT2 p, p1 ; 
    DML_ITEM item, item1 ;
    T2_LOOP loop, prev_loop ;
    T2_SPIRAL new ;
    T2_PARM_S parm, prev_parm ;

#ifdef CCDK_DEBUG
    INT connect_in1 ;
    CONNECT_IN1++ ;
    connect_in1 = CONNECT_IN1 ;
#endif
    item = DML_LAST ( T2_SPIRAL_LOOP_LIST(spiral) ) ;
    loop = DML_RECORD(item) ;

#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop, 9 ) ;
    if ( p0_valid )
        paint_point ( p0, .02, 9 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
    if ( T2_LOOP_ATTR(loop) & T2_ATTR_PROCSD_0 ) 
        RETURN ;
    t2p_loop_rearrange ( loop, p0, p0_valid, dist, p ) ;
    T2_LOOP_ATTR(loop) = T2_LOOP_ATTR(loop) | T2_ATTR_PROCSD_0 ;

    for ( item = DML_PREV(item) ; item != NULL ; item = DML_PREV(item) ) {
        loop = DML_RECORD(item) ;
        T2_LOOP_ATTR(loop) = T2_LOOP_ATTR(loop) | T2_ATTR_PROCSD_0 ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop, 12 ) ;
    paint_point ( p, .02, 10 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
        t2c_loop_rearrange_pt ( loop, p, FALSE, p1 ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_point ( p1, .02, 11 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
        if ( C2V_DIST ( p, p1 ) > dist + BBS_TOL ) {
            new = t2p_create_spiral () ;
            T2_SPIRAL_LIST(new) = T2_SPIRAL_LIST(spiral) ;
            T2_SPIRAL_LIST(spiral) = dml_create_list () ;
            T2_SPIRAL_OPEN_LIST(new) = T2_SPIRAL_OPEN_LIST(spiral) ;
            T2_SPIRAL_OPEN_LIST(spiral) = NULL ;
            dml_append_data ( T2_SPIRAL_LIST(spiral), new ) ; 
            T2_SPIRAL_PARENT(new) = spiral ;
            C2V_COPY ( p, T2_SPIRAL_PT(new) ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_point ( T2_SPIRAL_PT(new), .02, 13 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
            T2_SPIRAL_LOOP_LIST(new) = dml_create_list () ;
            for ( item1 = dml_last ( T2_SPIRAL_LOOP_LIST(spiral) ) ; 
                DML_PREV(item1) != item && DML_PREV(item1) != item ; 
                item1 = dml_last ( T2_SPIRAL_LOOP_LIST(spiral) ) ) {
                dml_insert ( T2_SPIRAL_LOOP_LIST(new), 
                    DML_RECORD(item1), TRUE ) ;
                dml_remove_last ( T2_SPIRAL_LOOP_LIST(spiral) ) ;
            }

            prev_loop = dml_last_record ( T2_SPIRAL_LOOP_LIST(spiral) ) ;
            t2c_connect_2loops ( prev_loop, loop, dist, FALSE, 
                NULL, p1, &prev_parm, &parm ) ;
            t2c_loop_rearrange_parm ( prev_loop, &prev_parm ) ;
            t2c_loop_rearrange_parm ( loop, &parm ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop, 11 ) ;
    paint_point ( p1, .02, 13 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
        }
        C2V_COPY ( p1, p ) ;
    }
    loop = T2_LOOP_PARENT(loop) ;
    if ( loop != NULL ) {
        t2c_project_loop ( loop, p, FALSE, NULL, T2_SPIRAL_PT(spiral) ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop, 11 ) ;
    paint_point ( T2_SPIRAL_PT(spiral), .02, 13 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
        if ( C2V_DIST ( p, T2_SPIRAL_PT(spiral) ) > dist + BBS_TOL ) {
            new = t2p_create_spiral () ;
            T2_SPIRAL_LIST(new) = T2_SPIRAL_LIST(spiral) ;
            T2_SPIRAL_LIST(spiral) = dml_create_list () ;
            dml_append_data ( T2_SPIRAL_LIST(spiral), new ) ; 
            T2_SPIRAL_PARENT(new) = spiral ;
            C2V_COPY ( p, T2_SPIRAL_PT(new) ) ;
            T2_SPIRAL_LOOP_LIST(new) = T2_SPIRAL_LOOP_LIST(spiral) ;
            T2_SPIRAL_LOOP_LIST(spiral) = dml_create_list () ;
            T2_SPIRAL_OPEN_LIST(new) = T2_SPIRAL_OPEN_LIST(spiral) ;
            T2_SPIRAL_OPEN_LIST(spiral) = NULL ;
            dml_append_data ( T2_SPIRAL_LOOP_LIST(spiral), 
                dml_first_record ( T2_SPIRAL_LOOP_LIST(new) ) ) ;
            dml_remove_first ( T2_SPIRAL_LOOP_LIST(new) ) ;

            prev_loop = dml_first_record ( T2_SPIRAL_LOOP_LIST(spiral) ) ;
            t2c_connect_2loops ( prev_loop, loop, dist, FALSE, 
                NULL, p1, &prev_parm, &parm ) ;
            t2c_loop_rearrange_parm ( prev_loop, &prev_parm ) ;
            t2c_loop_rearrange_parm ( loop, &parm ) ;
            C2V_COPY ( p1, T2_SPIRAL_PT(spiral) ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop, 11 ) ;
    paint_point ( p1, .02, 13 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
        }    
    }
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2p_spiral_sort_key ( spiral1, spiral2, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral1, spiral2 ;
PT2 pt ;
{
    REAL dist1, dist2 ;

    dist1 = C2V_DIST ( pt, T2_SPIRAL_PT(spiral1) ) ;
    dist2 = C2V_DIST ( pt, T2_SPIRAL_PT(spiral2) ) ;
    RETURN ( dist1 < dist2 ) ;
}


/*-------------------------------------------------------------------------*/
STATIC REAL t2p_spiral_order_key ( spiral1, spiral2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral1, spiral2 ;
{
    RETURN ( C2V_DIST ( T2_SPIRAL_PT(spiral1), T2_SPIRAL_PT(spiral2) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_spiral_set_owner ( spiral ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_SPIRAL spiral ;
{
    DML_ITEM item ;
    T2_LOOP loop ;
    T2_SPIRAL spiral0 ;

    if ( spiral == NULL )
        RETURN ;
    if ( T2_SPIRAL_LOOP_LIST(spiral) != NULL ) {
        DML_WALK_LIST ( T2_SPIRAL_LOOP_LIST(spiral), item ) {
            loop = DML_RECORD(item) ;
            T2_LOOP_SPIRAL(loop) = spiral ;
        }
    }
    if ( T2_SPIRAL_LIST(spiral) != NULL ) {
        DML_WALK_LIST ( T2_SPIRAL_LIST(spiral), item ) {
            spiral0 = DML_RECORD(item) ;
            T2_SPIRAL_PARENT(spiral0) = spiral ;
        }
    }
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2p_loop_rearrange ( loop, pt, pt_valid, step, loop_pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt, loop_pt ;
BOOLEAN pt_valid ;
REAL step ;
{
    T2_PARM_S parm ;
    PT2 p1, p ; 
    BOOLEAN status ;
    REAL dist, dist0, dist1 ;
    DML_ITEM item ;
    T2_EDGE edge, edge1 ;

    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ||
        DML_LENGTH ( T2_LOOP_EDGE_LIST(loop) ) == 0 )  
        RETURN ( FALSE ) ;

    status = pt_valid &&
        t2c_project_loop_attr ( loop, pt, /* T2_ATTR_EXTERIOR, */ 
            T2_ATTR_ROUGH, FALSE, &parm, p1 ) && t2c_loop_smooth_at ( &parm ) ;
    if ( status ) {
        dist1 = C2V_DIST ( p1, pt ) ;
        if ( dist1 <= step ) {
            if ( t2c_loop_rearrange_parm ( loop, &parm ) ) {
                if ( loop_pt != NULL ) {
                    C2V_COPY ( p1, loop_pt ) ;
                }
                RETURN ( TRUE ) ;
            }
        }
    }

    edge = NULL ;
    dist0 = 0.0 ;

#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_loop ( loop, 10 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) { 
        edge1 = DML_RECORD(item) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( edge1, 11 ) ;
    getch () ;
}
else
    DISPLAY-- ;
#endif
        if ( T2_EDGE_ATTR(edge1) & /* T2_ATTR_EXTERIOR */ T2_ATTR_ROUGH ) {
            t2c_mid_pt ( edge1, p ) ;
            if ( pt_valid ) 
                dist = C2V_DIST ( p, pt ) ;
            if ( ( edge == NULL ) || !pt_valid || ( dist < dist0 ) ) {
                dist0 = dist ;
                edge = edge1 ;
            }
        }
    }

    if ( status && dist1 < dist0 && t2c_loop_rearrange_parm ( loop, &parm ) ) {
        if ( loop_pt != NULL ) {
            C2V_COPY ( p1, loop_pt ) ;
        }
        RETURN ( TRUE ) ;
    }

    if ( edge == NULL ) 
        RETURN ( t2p_loop_rearrange1 ( loop, step, loop_pt ) ) ;
    T2_PARM_EDGE(&parm) = edge ;
    T2_PARM_T(&parm) = 0.5 * ( T2_EDGE_T0(edge) + T2_EDGE_T1(edge) ) ;
    t2c_parm_adjust ( edge, T2_PARM_CPARM(&parm) ) ;
    status = t2c_loop_rearrange_parm ( loop, &parm ) ;
    if ( status && loop_pt != NULL ) 
        t2c_loop_ept0 ( loop, loop_pt ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2p_loop_rearrange1 ( loop, step, loop_pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 loop_pt ;
REAL step ;
{
    T2_EDGE edge ;
    INT n ;
    T2_PARM_S parm ;

    if ( T2_LOOP_PARENT(loop) == NULL ) 
        RETURN ( FALSE ) ;
    for ( n = 1 ; n < 10 ; n++ ) {
        edge = t2p_loop_rearrange2 ( loop, step, n, T2_PARM_CPARM(&parm) ) ;
        if ( edge != NULL ) {
            T2_PARM_EDGE(&parm) = edge ;
            if ( t2c_loop_rearrange_parm ( loop, &parm ) ) {
                if ( loop_pt != NULL ) 
                    t2c_loop_ept0 ( loop, loop_pt ) ;
                RETURN ( TRUE ) ;
            }
        }
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC T2_EDGE t2p_loop_rearrange2 ( loop, step, n, parm )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL step ;
INT n ;
PARM parm ;
{
    DML_ITEM item ;
    T2_EDGE edge ;
    PT2 p0, p1 ;
    REAL h ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        edge = DML_RECORD(item) ;
        h = ( T2_EDGE_T_MAX(edge) - T2_EDGE_T_MIN(edge) ) / ( (REAL)(n+1) ) ;
        for ( PARM_T(parm) = T2_EDGE_T_MIN(edge) + h ; 
              PARM_T(parm) < T2_EDGE_T_MAX(edge) - BBS_ZERO ; 
              PARM_T(parm) += h ) {
            t2c_parm_adjust ( edge, parm ) ;
            if ( t2c_eval_pt ( edge, parm, p0 ) &&
                t2c_project_loop ( T2_LOOP_PARENT(loop), p0, FALSE, NULL, p1 )
                && ( C2V_DIST ( p0, p1 ) <= step + BBS_TOL ) ) 
                RETURN ( edge ) ;
        }
    }
#ifdef __BRLNDC__
	RETURN ( edge ) ;
#endif
#ifdef __WATCOM__
	RETURN ( edge ) ;
#endif
}
#endif /* __BBS_MILL__>=1 */

