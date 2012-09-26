/* -2 -3 */
/******************************* C2C1.C *********************************/
/**************** Two-dimensional curves ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2cdefs.h>
#include <c2vmcrs.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN     c2c_closed ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
{
    PT2 ept0, ept1 ;
    if ( IS_ZERO ( C2_CURVE_T1(curve) - C2_CURVE_T0(curve) ) )
        RETURN ( TRUE ) ;   /* degenerate curve */
    else if ( C2_CURVE_IS_LINE(curve) )
        RETURN ( FALSE ) ;
    else if ( C2_CURVE_IS_ARC(curve) )
        RETURN ( C2_CURVE_T0(curve) <= BBS_ZERO &&
            C2_CURVE_T1(curve) >= 2.0 - BBS_ZERO ) ;
    else if ( C2_CURVE_IS_PCURVE(curve) )
        RETURN ( c2c_ept0 ( curve, ept0 ) && c2c_ept1 ( curve, ept1 )
            && C2V_IDENT_PTS ( ept0, ept1 ) ) ;
    else
#ifdef  SPLINE
        RETURN ( c2c_ept0 ( curve, ept0 ) && c2c_ept1 ( curve, ept1 )
            && C2V_IDENT_PTS ( ept0, ept1 ) ) ;
#else  /*SPLINE*/
        RETURN ( FALSE ) ;
#endif  /*SPLINE*/
}

