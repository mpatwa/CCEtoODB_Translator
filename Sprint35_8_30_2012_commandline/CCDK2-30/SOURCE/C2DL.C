/* -2 -3 */
/******************************* C2DL.C *********************************/
/********** Two-dimensional line construction routines ******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2ldefs.h>
#include <c2ddefs.h>
#include <c2dpriv.h>
#include <bbsdefs.h>
#include <c2vmcrs.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_line ( ept0, ept1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ept0, ept1 ;
{
    C2_CURVE curve ;

    curve = c2d_curve() ;
    if ( curve != NULL ) {
        C2_CURVE_TYPE(curve) = C2_LINE_TYPE ;
        C2_CURVE_T0(curve) = 0.0 ;
        C2_CURVE_J0(curve) = 1 ;
        C2_CURVE_T1(curve) = 1.0 ;
        C2_CURVE_J1(curve) = 2 ;

        C2_CURVE_LINE(curve) = c2l_create_2pts ( ept0, ept1 ) ;
        c2l_box ( C2_CURVE_LINE(curve), 0.0, 1.0, C2_CURVE_BOX(curve) ) ;
    }
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_line_dir ( ept0, dir_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

PT2 ept0, dir_vec ;
{
    PT2 ept1 ;

    C2V_ADD ( ept0, dir_vec, ept1 ) ;
    RETURN ( c2d_line ( ept0, ept1 ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_ray ( origin, angle ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 origin ;
REAL angle ;
{
    PT2 dir_vec ;
    REAL w_size = bbs_get_world_size () ;

    dir_vec[0] = cos ( angle ) * w_size ;
    dir_vec[1] = sin ( angle ) * w_size ;
    RETURN ( c2d_line_dir ( origin, dir_vec ) ) ;
}

