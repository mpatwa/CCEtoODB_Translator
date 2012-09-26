/* -2 -3 */
/******************************* C2DR.C *********************************/
/********** Two-dimensional polygon construction routines ***************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <bbsdefs.h>
#ifdef SPLINE
#include <aladefs.h>
#include <c2ndefs.h>
#include <c2ddefs.h>
#include <c2dpriv.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_polygon ( a, n ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a      ;   /* a[n] */
INT n ;
{
    RETURN ( c2d_polygon_knots ( a, n, NULL ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_CURVE c2d_polygon_knots ( a, n, knot ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a      ;   /* a[n] */
INT n ;
REAL *knot  ;
{
    C2_CURVE curve ;

    curve = c2d_curve() ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;

    C2_CURVE_NURB(curve) = c2n_create_nurb ( n, 2 ) ;
    if ( C2_CURVE_NURB(curve) == NULL ) {
        c2d_free_curve ( curve ) ;
        RETURN ( NULL ) ;
    }

    C2_CURVE_TYPE(curve) = C2_NURB_TYPE ;
    c2n_set_ctpts ( a, C2_CURVE_NURB(curve) ) ;
    c2n_set_knots ( knot, C2_CURVE_NURB(curve) ) ;
    c2n_init_parms ( C2_CURVE_NURB(curve), C2_CURVE_PARM0(curve), 
        C2_CURVE_PARM1(curve) ) ;
    c2n_box ( C2_CURVE_NURB(curve), C2_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;

}
#endif /* SPLINE */

