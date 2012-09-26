/* -2 -3 */
/******************************* C2D2.C *********************************/
/********** Two-dimensional curve construction routines *****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2cdefs.h>
#include <c2dpriv.h>
#include <c2ddefs.h>
#include <c2mem.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_copy ( curve0 )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C2_CURVE curve0 ;
{
    C2_CURVE curve ;
    curve = c2d_alloc_curve () ;
    if ( curve != NULL ) 
        c2c_copy ( curve0, curve ) ;
    if ( C2_CURVE_LINE(curve) == NULL ) {
        c2d_free_curve ( curve ) ;
        curve = NULL ;
    }
    RETURN ( curve ) ;
}

