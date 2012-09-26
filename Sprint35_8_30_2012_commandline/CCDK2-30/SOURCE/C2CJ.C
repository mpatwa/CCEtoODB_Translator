/* -2 -3 */
/******************************* C2CJ.C *********************************/
/**************** Two-dimensional curves ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alrdefs.h>
#include <c2gdefs.h>
#include <c2ldefs.h>
#include <c2pdefs.h>
#include <c2ndefs.h>
#include <c2cdefs.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_project ( curve, pt, parm, proj_pt )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 pt ;
PARM parm ;
PT2 proj_pt ;
{
    PARM_S parm1 ;
    if ( parm == NULL ) 
        parm = &parm1 ;
    if ( C2_CURVE_IS_LINE(curve) ) {
        PARM_T(parm) = c2l_project ( C2_CURVE_LINE(curve), pt, proj_pt ) ;
        PARM_J(parm) = 1 ;
        RETURN ( PARM_T(parm) >= C2_CURVE_T0(curve) - BBS_ZERO && 
                 PARM_T(parm) <= C2_CURVE_T1(curve) + BBS_ZERO ) ;
/*  02-02-92 
        RETURN ( PARM_T(parm) >= C2_CURVE_T0(curve) && 
                 PARM_T(parm) <= C2_CURVE_T1(curve) ) ;
*/
    }

    else if ( C2_CURVE_IS_ARC(curve) ) {
        if ( !c2g_project ( C2_CURVE_ARC(curve), C2_CURVE_T0(curve),  
            C2_CURVE_T1(curve),  pt, &(PARM_T(parm)), proj_pt ) )
            RETURN ( FALSE ) ;
        alr_parm_set ( parm ) ;
        RETURN ( PARM_T(parm) >= C2_CURVE_T0(curve) && 
                 PARM_T(parm) <= C2_CURVE_T1(curve) ) ;
    }

    else if ( C2_CURVE_IS_PCURVE(curve) ) {
        if ( !c2p_project ( C2_CURVE_PCURVE(curve), C2_CURVE_T0(curve), 
                 C2_CURVE_T1(curve), pt, &(PARM_T(parm)), proj_pt ) )
            RETURN ( FALSE ) ;
        PARM_J(parm) = (INT)PARM_T(parm) ;
        RETURN ( TRUE ) ;
    }
    
    else 
#ifdef  SPLINE
        RETURN ( c2n_project ( C2_CURVE_NURB(curve), C2_CURVE_PARM0(curve), 
            C2_CURVE_PARM1(curve), pt, parm, proj_pt ) ) ;
#else  /*SPLINE*/
        RETURN ( FALSE ) ;
#endif  /*SPLINE*/
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_project_ext ( curve, pt, parm, proj_pt )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 pt ;
PARM parm ;
PT2 proj_pt ;
{
    PARM_S parm1 ;
    if ( parm == NULL ) 
        parm = &parm1 ;
    if ( C2_CURVE_IS_LINE(curve) ) {
        PARM_T(parm) = c2l_project ( C2_CURVE_LINE(curve), pt, proj_pt ) ;
        PARM_J(parm) = 1 ;
        RETURN ( TRUE ) ;
    }

    else if ( C2_CURVE_IS_ARC(curve) ) {
        if ( !c2g_project ( C2_CURVE_ARC(curve), C2_CURVE_T0(curve),  
            C2_CURVE_T1(curve),  pt, &(PARM_T(parm)), proj_pt ) )
            RETURN ( FALSE ) ;
        alr_parm_set ( parm ) ;
        RETURN ( TRUE ) ;
    }

    else 
        RETURN ( c2c_project ( curve, pt, parm, proj_pt ) ) ;
}

