/* -2 -3 */
/******************************* C2CE1.C ********************************/
/**************** Two-dimensional curves ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2pdefs.h>
#include <c2gdefs.h>
#include <c2ndefs.h>
#include <c2cdefs.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_curvature ( curve, parm, curv_ptr )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                       !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm ;
REAL *curv_ptr ;
{
    if ( C2_CURVE_IS_LINE(curve) ) {
        *curv_ptr = 0.0 ;
        RETURN ( TRUE ) ;
    }
    else if ( C2_CURVE_IS_ARC(curve) ) 
        RETURN ( c2g_curvature ( C2_CURVE_ARC(curve), curv_ptr ) ) ;
    else if ( C2_CURVE_IS_PCURVE(curve) ) {
        *curv_ptr = c2p_curvature ( C2_CURVE_PCURVE(curve), PARM_T(parm) ) ;
        RETURN ( TRUE ) ;
    }
    else
#ifdef  SPLINE
        RETURN ( c2n_curvature ( C2_CURVE_NURB(curve), parm, curv_ptr ) ) ;
#else
        RETURN ( FALSE ) ;
#endif /* SPLINE */
}

#ifdef NEW_CODE
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2c_curv_root ( curve, offset, parm0, parm1, root_parm )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL offset ;
PARM parm0, parm1, root_parm ;
{
    if ( C2_CURVE_IS_LINE(curve) || C2_CURVE_IS_ARC(curve) || 
        C2_CURVE_IS_PCURVE(curve) )
        RETURN ( FALSE ) ;
    else
#ifdef SPLINE
    RETURN ( c2s_curv_w_root ( C2_CURVE_CTPT(curve), C2_CURVE_N(curve),
            C2_CURVE_D(curve), C2_CURVE_KNOT(curve), C2_CURVE_W(curve),
            offset, parm0, parm1, root_parm ) ) ;
#else        
        RETURN ( FALSE ) ;
#endif  /*SPLINE*/
}
#endif  /*NEW_CODE*/

