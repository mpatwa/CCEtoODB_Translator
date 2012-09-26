/* -2 -3 */
/******************************* C2CX.C *********************************/
/**************** Two-dimensional curves ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2gdefs.h>
#include <c2pdefs.h>
#include <c2ndefs.h>
#include <dmldefs.h>
#include <c2cdefs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2c_coord_extrs ( curve, coord, extr_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* This routine computes extremums of the specified coordinate of a curve */
C2_CURVE    curve ;
INT         coord ;
DML_LIST    extr_list ;
{
    if ( C2_CURVE_IS_LINE(curve) ) 
        RETURN ( 0 ) ;

    else if ( C2_CURVE_IS_ARC(curve) ) 
        RETURN ( c2g_coord_extrs ( C2_CURVE_ARC(curve), 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve), coord, extr_list ) ) ;
    else if ( C2_CURVE_IS_PCURVE(curve) ) 
        RETURN ( c2p_coord_extrs ( C2_CURVE_PCURVE(curve), 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve), coord, extr_list ) ) ;
    else 
#ifdef  SPLINE
        RETURN ( c2n_coord_extrs ( C2_CURVE_NURB(curve), 
            C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve), 
            coord, extr_list ) ) ;
#else
        RETURN ( 0 ) ;
#endif  /*SPLINE*/
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_coord_extr ( curve, coord, extr, value ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* This routine computes extremums of the specified coordinate of a curve */
C2_CURVE    curve ;
INT         coord, extr ;
REAL *value ;
{
    DML_LIST extr_list = dml_create_list ();
    DML_ITEM item ;
    BOOLEAN valid = FALSE ;
    PT2 p ;
    C2_EXTR_REC x_rec ;

    c2c_coord_extrs ( curve, coord, extr_list ) ;

    DML_WALK_LIST ( extr_list, item ) {
        x_rec = (C2_EXTR_REC)DML_RECORD(item) ;
        if ( ( C2_EXTR_REC_TYPE(x_rec) == extr ) && ( !valid ||
            ( ( extr > 0 ) == ( C2_EXTR_REC_F(x_rec) > *value ) ) ) ) {
            valid = TRUE ;
            *value = C2_EXTR_REC_F(x_rec) ;
        }
        FREE ( x_rec ) ;
        x_rec = NULL ;
        DML_RECORD(item) = NULL ;
    }
    c2c_ept0 ( curve, p ) ;
    if ( !valid || ( ( extr > 0 ) == ( p[coord] > *value ) ) ) {
        valid = TRUE ;
        *value = p[coord] ;
    }
    c2c_ept1 ( curve, p ) ;
    if ( !valid || ( ( extr > 0 ) == ( p[coord] > *value ) ) ) {
        valid = TRUE ;
        *value = p[coord] ;
    }
    dml_free_list ( extr_list ) ;
    RETURN ( valid ) ;
}

