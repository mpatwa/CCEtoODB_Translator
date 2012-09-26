/* -R __BBS_MILL__=3 __BBS_TURN__=3 */
/********************************** M2N.C **********************************/
/****************** Routines for processing nurb geometry ******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <c2nmcrs.h>
#include <m2ndefs.h>
#include <m2sdefs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2n_x_max ( nurb, parm0, parm1, x_max_ptr, x_max_parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm0, parm1 ; 
REAL *x_max_ptr ;
PARM x_max_parm ;
{
    RETURN ( m2s_x_max ( C2_NURB_CTPT(nurb), C2_NURB_D(nurb), 
        C2_NURB_KNOT(nurb), C2_NURB_W(nurb), parm0, parm1, 
        x_max_ptr, x_max_parm ) ) ;

}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2n_hor_ray_int_no ( nurb, parm0, parm1, pt, pt_on_curve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm0, parm1 ; 
PT2 pt ;
BOOLEAN pt_on_curve ;
{
    RETURN ( m2s_hor_ray_int_no ( C2_NURB_CTPT(nurb), C2_NURB_N(nurb), 
            C2_NURB_D(nurb), C2_NURB_KNOT(nurb), C2_NURB_W(nurb), 
            parm0, parm1, pt, pt_on_curve ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2n_ray_int_no ( nurb, parm0, parm1, pt, 
            c, s, pt_on_curve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm0, parm1 ; 
PT2 pt ;
REAL c, s ;
BOOLEAN pt_on_curve ;
{
    RETURN ( m2s_ray_int_no ( C2_NURB_CTPT(nurb), C2_NURB_N(nurb), 
            C2_NURB_D(nurb), C2_NURB_KNOT(nurb), C2_NURB_W(nurb), 
            parm0, parm1, pt, c, s, pt_on_curve ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL m2n_area ( nurb, parm0, parm1, origin ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm0, parm1 ; 
PT2 origin ;
{
    RETURN ( m2s_area ( C2_NURB_CTPT(nurb), C2_NURB_N(nurb), 
            C2_NURB_D(nurb), C2_NURB_KNOT(nurb), C2_NURB_W(nurb), 
            parm0, parm1, origin ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2n_area_mass_ctr ( nurb, parm0, parm1, ctr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm0, parm1 ; 
PT2 ctr ;
{
    RETURN ( m2s_area_mass_ctr ( C2_NURB_CTPT(nurb), C2_NURB_D(nurb), 
        C2_NURB_KNOT(nurb), C2_NURB_W(nurb), parm0, parm1, ctr ) ) ;
}
#endif /*SPLINE*/

