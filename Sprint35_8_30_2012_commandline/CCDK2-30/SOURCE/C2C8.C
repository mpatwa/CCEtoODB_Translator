/* -2 -3*/
/******************************* C2C8.C *********************************/
/**************** Two-dimensional curves ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alsdefs.h>
#include <c2cdefs.h>
#include <c2cpriv.h>
#include <c2gdefs.h>
#include <c2ldefs.h>
#include <c2ndefs.h>
#include <c2vmcrs.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2c_trim0 ( curve, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm ;
{
    COPY_PARM ( parm, C2_CURVE_PARM0(curve) ) ;
    c2c_box ( curve ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2c_trim1 ( curve, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm ;
{
    COPY_PARM ( parm, C2_CURVE_PARM1(curve) ) ;
    c2c_box ( curve ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2c_trim0_verify ( curve, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm ;
{
    if ( PARM_T(parm) > C2_CURVE_T1(curve) )
        RETURN ( NULL ) ;
    COPY_PARM ( parm, C2_CURVE_PARM0(curve) ) ;
    c2c_box ( curve ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2c_trim1_verify ( curve, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm ;
{
    if ( PARM_T(parm) < C2_CURVE_T0(curve) )
        RETURN ( NULL ) ;
    COPY_PARM ( parm, C2_CURVE_PARM1(curve) ) ;
    c2c_box ( curve ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2c_trim ( curve, parm0, parm1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm0, parm1 ;
{
    if ( ( parm0 != NULL ) && ( parm1 != NULL ) && 
        ( PARM_T(parm0) > PARM_T(parm1) ) )
        RETURN ( NULL ) ;
    if ( parm0 != NULL ) 
        COPY_PARM ( parm0, C2_CURVE_PARM0(curve) ) ;
    if ( parm1 != NULL ) 
        COPY_PARM ( parm1, C2_CURVE_PARM1(curve) ) ;
    c2c_box ( curve ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2c_trim_t0 ( curve, t ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL t ;
{
    C2_CURVE_T0(curve) = t ;
    c2c_parm_adjust ( curve, C2_CURVE_PARM0(curve) ) ;
    c2c_box ( curve ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2c_trim_t1 ( curve, t ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL t ;
{
    C2_CURVE_T1(curve) = t ;
    c2c_parm_adjust ( curve, C2_CURVE_PARM1(curve) ) ;
    c2c_box ( curve ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2c_trim_t ( curve, t0, t1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL t0, t1 ;
{
    if ( t0 > t1 )
        RETURN ( NULL ) ;
    c2c_trim_t0 ( curve, t0 ) ;
    c2c_trim_t1 ( curve, t1 ) ;
    c2c_box ( curve ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE PARM c2c_parm_adjust ( curve, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm ;
{
    if ( C2_CURVE_IS_LINE(curve) ) 
        PARM_J(parm) = 1 ;
    else if ( C2_CURVE_IS_ARC(curve) || C2_CURVE_IS_PCURVE(curve) ) 
        PARM_J(parm) = (INT)PARM_T(parm) ;
#ifdef SPLINE
    else 
        c2n_parm_adjust ( C2_CURVE_NURB(curve), parm ) ;
#endif /*SPLINE*/
    RETURN ( parm ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2c_reverse ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
{
    REAL t ;

    if ( C2_CURVE_IS_LINE(curve) ) {
        c2l_reverse ( C2_CURVE_LINE(curve), C2_CURVE_LINE(curve) ) ;
        t = C2_CURVE_T0(curve) ;
        C2_CURVE_T0(curve) = 1.0 - C2_CURVE_T1(curve) ;
        C2_CURVE_T1(curve) = 1.0 - t ;
    }

    else if ( C2_CURVE_IS_ARC(curve) ) {
        c2g_reverse ( C2_CURVE_ARC(curve), C2_CURVE_ARC(curve) ) ;
        t = C2_CURVE_T0(curve) ;
        C2_CURVE_T0(curve) = 1.0 - C2_CURVE_T1(curve) ;
        C2_CURVE_T1(curve) = 1.0 - t ;
    }

#ifdef SPLINE
    else 
        c2n_reverse ( C2_CURVE_NURB(curve), 
            C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve) ) ;
#endif /*SPLINE*/
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC INT c2c_t_pos ( curve, t ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL t ;
{
    if ( t <= C2_CURVE_T0(curve) - BBS_ZERO ) 
        RETURN ( -1 ) ;
    else if ( t >= C2_CURVE_T1(curve) + BBS_ZERO ) 
        RETURN ( 1 ) ;
    else 
        RETURN ( 0 ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_t_inside ( curve, t ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL t ;
{
    RETURN ( c2c_t_pos ( curve, t ) == 0 ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC INT c2c_parm_pos ( curve, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm ;
{
    RETURN ( c2c_t_pos ( curve, PARM_T(parm) ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_parm_inside ( curve, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm ;
{
    RETURN ( c2c_t_inside ( curve, PARM_T(parm) ) ) ;
}

