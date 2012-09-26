/* -S -Z -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2B.C ***********************************/ 
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
#include <t2xmax.h>
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
EXTERN BOOLEAN DIR ;
#endif
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC BOOLEAN  t2b_form_region __(( DML_LIST, DML_LIST, DML_LIST )) ;
STATIC DML_LIST t2b_form_region1 __(( DML_ITEM, DML_LIST, DML_LIST )) ;
STATIC DML_ITEM t2b_last_x_max __(( DML_LIST )) ;
STATIC BOOLEAN  t2b_remove_loops __(( DML_LIST, DML_LIST )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2b_create_regions ( looplist, regionlist ) 
/* This routine creates regions out of a bunch of oriented loops. Loops used 
for construction are removed from the looplist, so the remaining loops could 
not be used in the construction. The loops are supposed to have no 
intersections */
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist, regionlist ;
{
    DML_LIST x_max_list ;
    BOOLEAN done = FALSE ;

#ifdef CCDK_DEBUG
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        DML_ITEM item ;
        DML_WALK_LIST ( looplist, item ) {
            paint_loop ( DML_RECORD(item), 12 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
#endif
    if ( regionlist == NULL ) 
        regionlist = dml_create_list () ;
    x_max_list = t2d_create_x_max_list ( looplist ) ;
    while ( !done )
        done = t2b_remove_loops ( looplist, x_max_list ) ; 

    while ( DML_LENGTH(x_max_list) && DML_LENGTH(looplist) &&
        t2b_form_region ( looplist, x_max_list, regionlist ) ) ;
    dml_destroy_list ( x_max_list, dmm_free ) ;
//    x_max_list = NULL ;
    RETURN ( regionlist ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_remove_loops ( looplist, x_max_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist, x_max_list ;
{
    T2_LOOP loop0, loop ;
    T2_X_MAX x_max ;
    DML_ITEM item, item1 ;
    PT2 pt ;
    T2_PT_POSITION pt_pos ;

    if ( DML_LENGTH(x_max_list) == 0 ) 
        RETURN ( TRUE ) ;
    x_max = DML_FIRST_RECORD(x_max_list) ;
    if ( T2_X_MAX_ORTN(x_max) )
        RETURN ( TRUE ) ;
    loop0 = T2_X_MAX_LOOP(x_max) ;
    FREE ( x_max ) ;
    x_max = NULL ; 
    dml_remove_first ( x_max_list ) ; 

    for ( item = DML_FIRST(x_max_list) ; item != NULL ; item = item1 ) {
        item1 = DML_NEXT(item) ;
        x_max = DML_RECORD(item) ;
        loop = T2_X_MAX_LOOP(x_max) ;
        t2c_loop_pt ( loop, pt, NULL ) ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) 
    {
        paint_loop ( loop0, 12 ) ;
        getch();
        paint_loop ( loop, 13 ) ;
        getch();
        paint_point ( pt, 0.02, 11 ) ;
        getch();
    }
    else
        DISPLAY-- ;
#endif
        pt_pos = t2c_pt_pos_loop ( loop0, pt ) ; 
        if ( pt_pos == T2_PT_OUTSIDE || pt_pos == T2_PT_ON_BOUNDARY ) {
            t2d_free_loop ( loop ) ;
            dml_remove_data ( looplist, loop ) ;
            FREE ( x_max ) ;
//            x_max = NULL ; 
            dml_remove_item ( x_max_list, item ) ; 
        }
    }
    t2d_free_loop ( loop0 ) ;
    dml_remove_data ( looplist, loop0 ) ;
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2b_form_region ( inlooplist, x_max_list, regionlist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST inlooplist, x_max_list, regionlist ;
{
    DML_ITEM item ;
    DML_LIST looplist ;

    item = t2b_last_x_max ( x_max_list ) ;
    if ( item == NULL ) 
        RETURN ( FALSE ) ;
    looplist = t2b_form_region1 ( item, inlooplist, x_max_list ) ;
    if ( looplist != NULL && t2c_looplist_area ( looplist ) > BBS_TOL ) 
        dml_append_data ( regionlist, t2d_create_region ( looplist ) ) ;
    else 
        dml_destroy_list ( looplist, ( PF_ACTION ) t2d_free_loop ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_ITEM t2b_last_x_max ( x_max_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST x_max_list ;
{
    DML_ITEM item ;
    T2_X_MAX x_max ;

    for ( item = DML_LAST(x_max_list) ; item != NULL ; 
        item = DML_PREV(item) ) {
        x_max = DML_RECORD(item) ;
        if ( T2_X_MAX_ORTN(x_max) )
            RETURN ( item ) ;
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_LIST t2b_form_region1 ( item0, inlooplist, x_max_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM item0 ;
DML_LIST inlooplist, x_max_list ;
{
    T2_X_MAX x_max ; 
    DML_ITEM item, item1 ;
    DML_LIST looplist ;
    PT2 pt ;
    T2_PT_POSITION pt_pos ;
    T2_LOOP loop, loop0 ;

    looplist = dml_create_list () ;
    x_max = DML_RECORD(item0) ;
    loop0 = T2_X_MAX_LOOP(x_max) ;
    dml_append_data ( looplist, loop0 ) ;
    item1 = DML_NEXT(item0) ;
    FREE ( x_max ) ;
    x_max = NULL ;
    dml_remove_item ( x_max_list, item0 ) ;
    dml_remove_data ( inlooplist, loop0 ) ;

    for ( item = item1 ; item != NULL ; item = item1 ) {
        item1 = DML_NEXT(item) ;
        x_max = DML_RECORD(item) ;
        loop = T2_X_MAX_LOOP(x_max) ;
        t2c_loop_pt ( loop, pt, NULL ) ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) 
    {
        paint_loop ( loop0, 12 ) ;
        getch();
        paint_loop ( loop, 13 ) ;
        getch();
        paint_point ( pt, 0.02, 11 ) ;
        getch();
    }
    else
        DISPLAY-- ;
#endif
        pt_pos = t2c_pt_pos_loop ( loop0, pt ) ; 
        if ( pt_pos == T2_PT_INSIDE || pt_pos == T2_PT_ON_BOUNDARY ) {
            dml_append_data ( looplist, loop ) ;
            dml_remove_data ( inlooplist, loop ) ;
            FREE ( x_max ) ;
//            x_max = NULL ; 
            dml_remove_item ( x_max_list, item ) ; 
        }
    }
    RETURN ( looplist ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

