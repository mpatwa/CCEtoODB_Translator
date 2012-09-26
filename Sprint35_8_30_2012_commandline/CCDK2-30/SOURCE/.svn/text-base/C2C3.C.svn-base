/* -2 -3 */
/********************************* C2C3.C **********************************/
/*************************  Sorting lists of curves ************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h>
#include <c2cdefs.h>
#include <c2vmcrs.h>
STATIC DML_LIST c2c_sort_gap __(( DML_LIST, PF_EVAL2, PF_EVAL2, BOOLEAN, 
            DML_LIST, REAL*, ANY*, ANY* )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST c2c_sort_curve_list ( inlist, eval_ept0, eval_ept1, 
            dir, outlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST inlist ;
PF_EVAL2 eval_ept0, eval_ept1 ;
BOOLEAN dir ;
DML_LIST outlist ;
{
    RETURN ( c2c_sort_gap ( inlist, eval_ept0, eval_ept1, 
            dir, outlist, NULL, NULL, NULL ) ) ; 
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2c_gap_curves ( geomlist, eval_ept0, eval_ept1, 
        geom0_ptr, geom1_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST geomlist ;
PF_EVAL2 eval_ept0, eval_ept1 ;
ANY *geom0_ptr, *geom1_ptr ; 
{ 
    DML_LIST templist = dml_create_list () ;
    REAL gap ;

    templist = dml_create_copy ( geomlist ) ;
    templist = c2c_sort_gap ( templist, eval_ept0, eval_ept1, 
        FALSE, templist, &gap, geom0_ptr, geom1_ptr ) ;
    dml_free_list ( templist ) ;
    RETURN ( gap ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_LIST c2c_sort_gap ( inlist, eval_ept0, eval_ept1, 
            dir, outlist, gap_ptr, geom0_ptr, geom1_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST inlist ;
PF_EVAL2 eval_ept0, eval_ept1 ;
BOOLEAN dir ;
DML_LIST outlist ;
REAL *gap_ptr ;
ANY *geom0_ptr, *geom1_ptr ;
{
    DML_ITEM item, item0 ;
    PT2 q0, q1, r0, r1, p0, p1 ;
    REAL dist, dist0=0.0, gap ;
    BOOLEAN pos_first ;
    ANY geom, prev_geom ;
    DML_LIST templist ;

    if ( outlist == NULL ) 
        outlist = dml_create_list () ;
    gap = 0.0 ;
    if ( geom0_ptr != NULL ) 
        *geom0_ptr = NULL ;
    if ( geom1_ptr != NULL ) 
        *geom1_ptr = NULL ;

    if ( inlist == outlist ) {
        templist = dml_create_list () ;
        dml_append_list ( templist, inlist ) ;
    }
    else 
        templist = inlist ;

    item = dml_first ( templist ) ;
    prev_geom = NULL ;

    if ( item != NULL ) {
        geom = dml_record ( item ) ;
        eval_ept0 ( geom, p0 ) ;
        eval_ept1 ( geom, p1 ) ;
        dml_append_data ( outlist, geom ) ;
        dml_remove_item ( templist, item ) ;
        item = NULL ;
    }

    while ( dml_length ( templist ) > 0 ) {
        prev_geom = geom ;
        item0 = NULL ;
        DML_WALK_LIST ( templist, item ) {
            geom = dml_record ( item ) ;
            eval_ept0 ( geom, r0 ) ;
            eval_ept1 ( geom, r1 ) ;
            dist = C2V_DIST ( p1, r0 ) ;
            if ( ( item0 == NULL ) || ( dist < dist0 ) ) {
                pos_first = FALSE ;
                dist0 = dist ;
                item0 = item ;
                C2V_COPY ( r1, q1 ) ;
            }
            dist = C2V_DIST ( p0, r1 ) ;
            if ( ( item0 == NULL ) || ( dist < dist0 ) ) {
                pos_first = TRUE ;
                dist0 = dist ;
                item0 = item ;
                C2V_COPY ( r0, q0 ) ;
            }

            if ( !dir ) {
                dist = C2V_DIST ( p1, r1 ) ;
                if ( ( item0 == NULL ) || ( dist < dist0 ) ) {
                    pos_first = FALSE ;
                    dist0 = dist ;
                    item0 = item ;
                    C2V_COPY ( r0, q1 ) ;
                }
                dist = C2V_DIST ( p0, r0 ) ;
                if ( ( item0 == NULL ) || ( dist < dist0 ) ) {
                    pos_first = TRUE ;
                    dist0 = dist ;
                    item0 = item ;
                    C2V_COPY ( r1, q0 ) ;
                }
            }
        }

        if ( gap < dist0 ) {
            gap = dist0 ;
            if ( geom0_ptr != NULL )
                *geom0_ptr = prev_geom ;
            if ( geom1_ptr != NULL )
                *geom1_ptr = geom ;
        }

        geom = dml_record ( item0 ) ;
        dml_insert ( outlist, geom, pos_first ) ;

        if ( pos_first ) {
            C2V_COPY ( q0, p0 ) ;
        }
        else {
            C2V_COPY ( q1, p1 ) ;
        }
        dml_remove_item ( templist, item0 ) ;
    }

    if ( inlist == outlist ) 
        dml_free_list ( templist ) ;
    if ( gap_ptr != NULL )
        *gap_ptr = gap ;

    RETURN ( outlist ) ;
}

