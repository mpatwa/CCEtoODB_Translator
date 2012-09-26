/* -L __BBS_MILL__=3 */
/********************************* T2BL.C **********************************/ 
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
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
EXTERN BOOLEAN DIR ;
#endif

#if ( __BBS_MILL__>=3 )
/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2b_leftover ( region, dist, result )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
REAL dist ;
DML_LIST result ;
{
    DML_LIST fillet_list ;
    DML_ITEM item, item1 ;

    if ( result == NULL ) 
        result = dml_create_list () ;
    fillet_list = t2b_fillet_region ( region, dist, NULL ) ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    DML_WALK_LIST ( fillet_list, item ) {
        paint_region ( DML_RECORD(item), 5 ) ;
        getch();
    }
}
else
    DISPLAY-- ;
if ( DIS_LEVEL == -2 ) 
    t2c_write_region ( "fillet.dat", DML_FIRST_RECORD(fillet_list) ) ;
#endif
    DML_WALK_LIST ( fillet_list, item ) 
        t2c_mark_region_rough ( DML_RECORD(item) ) ;
    t2c_mark_region_part ( region ) ;
    result = t2b_diff_pass1 ( region, fillet_list, result ) ;
    for ( item = DML_FIRST(result) ; item != NULL ; item = item1 ) {
        item1 = DML_NEXT(item) ;
        if ( t2c_region_area ( DML_RECORD(item) ) <= fabs(dist) * BBS_TOL ) {
            t2d_free_region ( DML_RECORD(item) ) ;
            dml_remove_item ( result, item ) ;
        }
    }
    dml_destroy_list ( fillet_list, t2d_free_region ) ;
    RETURN ( result ) ;
}
#endif /* ( __BBS_MILL__>=3 ) */

