/* -L __BBS_MILL__=3 __BBS_TURN__=3 */
/********************************* T2BF.C **********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <dmldefs.h>
#include <t2bdefs.h>
#include <t2cdefs.h>
#include <t2ddefs.h>
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
EXTERN BOOLEAN DIR ;
#endif
#if ( __BBS_MILL__>=3 || __BBS_TURN__>=3 )

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2b_fillet_region ( region, rad, fillet_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
REAL rad ;
DML_LIST fillet_list ;
{
    DML_LIST offset_list, region_list ;
    DML_ITEM item ;

    offset_list = t2b_offset_region ( region, -rad, NULL, NULL ) ;
#ifdef CCDK_DEBUG
{
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        DML_WALK_LIST ( offset_list, item ) {
            paint_region ( DML_RECORD(item), 9 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
    if ( DIS_LEVEL == -2 ) 
        t2c_write_region ( "offset.dat", DML_FIRST_RECORD(offset_list) ) ;
}
#endif
    region_list = dml_create_list () ;
    DML_WALK_LIST ( offset_list, item ) 
        region_list = t2b_offset_region ( DML_RECORD(item), rad, 
            NULL, region_list ) ;
#ifdef CCDK_DEBUG
{
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        DML_WALK_LIST ( region_list, item ) {
            paint_region ( DML_RECORD(item), 10 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
}
#endif
    fillet_list = t2b_union_l ( region_list, fillet_list ) ;
    dml_destroy_list ( offset_list, ( PF_ACTION ) t2d_free_region ) ;
    dml_destroy_list ( region_list, ( PF_ACTION ) t2d_free_region ) ;
    RETURN ( fillet_list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2b_fillet_loop ( loop, rad, fillet_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
REAL rad ;
DML_LIST fillet_list ;
{
    DML_LIST offset_list ;
    DML_ITEM item ;

    if ( fillet_list == NULL ) 
        fillet_list = dml_create_list();
    offset_list = t2b_offset_loop ( loop, -rad, NULL, NULL ) ;
#ifdef CCDK_DEBUG
{
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        DML_WALK_LIST ( offset_list, item ) {
            paint_loop ( (T2_LOOP)DML_RECORD(item), 9 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
}
#endif
    if ( offset_list != NULL ) {
        DML_WALK_LIST ( offset_list, item ) 
            t2b_offset_loop ( (T2_LOOP)DML_RECORD(item), rad, 
                NULL, fillet_list ) ;
    }
#ifdef CCDK_DEBUG
{
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        DML_WALK_LIST ( fillet_list, item ) {
            paint_loop ( (T2_LOOP)DML_RECORD(item), 10 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
}
#endif
    dml_destroy_list ( offset_list, ( PF_ACTION ) t2d_free_loop ) ;
    RETURN ( fillet_list ) ;
}
#endif /* __BBS_MILL__>=3 || __BBS_TURN__>=3 */

