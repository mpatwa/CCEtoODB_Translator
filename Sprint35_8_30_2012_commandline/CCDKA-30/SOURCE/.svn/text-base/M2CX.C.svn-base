/* __BBS_MILL__=1 __BBS_TURN__=1 */
/******************************* M2CX.C *********************************/
/**************** Two-dimensional curves ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <m2gdefs.h> 
#include <m2ldefs.h> 
#ifdef NEW_CODE
#include <m2pdefs.h> 
#endif
#include <m2ndefs.h> 
#include <m2cdefs.h> 

/*----------------------------------------------------------------------*/
BBS_PRIVATE INT m2c_xhatch_inters ( curve, pt, h, c, s, owner, intlist, m ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 pt ;
REAL h, c, s ;
ANY owner ;
DML_LIST *intlist ;
INT m ;
{
    if ( C2_CURVE_IS_LINE(curve) ) 
        RETURN ( m2l_xhatch_inters ( C2_CURVE_LINE(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), pt, h, c, s, owner, intlist, m ) ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        RETURN ( m2g_xhatch_inters ( C2_CURVE_ARC(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), pt, h, c, s, owner, intlist, m ) ) ;
#ifdef NEW_CODE
    else if ( C2_CURVE_IS_PCURVE(curve) ) 
        RETURN ( m2p_xhatch_inters ( C2_CURVE_PCURVE(curve), 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve), pt, 
            h, c, s, owner, intlist, m ) ) ;
#endif
    else 
#ifdef  SPLINE
        RETURN ( m2n_xhatch_inters ( C2_CURVE_NURB(curve), 
            C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve), 
            pt, h, c, s, owner, intlist, m ) ) ;
#else
        RETURN ( -1 ) ;
#endif  /* SPLINE */
}

