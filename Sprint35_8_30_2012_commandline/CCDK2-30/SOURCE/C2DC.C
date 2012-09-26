/* -2 -3 */
/******************************* C2DC.C *********************************/
/********** Two-dimensional curve construction routines *****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <c2cdefs.h>
#include <c2ddefs.h>
/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_trim0 ( curve, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm ;
{
    C2_CURVE trim_curve = c2d_copy ( curve ) ;
    if ( trim_curve == NULL ) 
        RETURN ( NULL ) ;
    c2c_trim0 ( trim_curve, parm ) ;
    RETURN ( trim_curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_trim1 ( curve, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm ;
{
    C2_CURVE trim_curve = c2d_copy ( curve ) ;
    if ( trim_curve == NULL ) 
        RETURN ( NULL ) ;
    c2c_trim1 ( trim_curve, parm ) ;
    RETURN ( trim_curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_trim ( curve, parm0, parm1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm0, parm1 ;
{
    C2_CURVE trim_curve ;
    REAL t0, t1 ;

    t0 = ( parm0 == NULL ) ? C2_CURVE_T0(curve) : PARM_T(parm0) ;
    t1 = ( parm1 == NULL ) ? C2_CURVE_T1(curve) : PARM_T(parm1) ;

    if ( IS_SMALL ( t0 - t1 ) )
        RETURN ( NULL ) ;

    trim_curve = c2d_copy ( curve ) ;
    if ( trim_curve == NULL ) 
        RETURN ( NULL ) ;
    if ( parm0 != NULL ) 
        c2c_trim0 ( trim_curve, parm0 ) ;
    if ( parm1 != NULL ) 
        c2c_trim1 ( trim_curve, parm1 ) ;
    RETURN ( trim_curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_trim_t0 ( curve, t0 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL t0 ;
{
    C2_CURVE trim_curve = c2d_copy ( curve ) ;
    if ( trim_curve == NULL ) 
        RETURN ( NULL ) ;
    c2c_trim_t0 ( trim_curve, t0 ) ;
    RETURN ( trim_curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_trim_t1 ( curve, t1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL t1 ;
{
    C2_CURVE trim_curve = c2d_copy ( curve ) ;
    if ( trim_curve == NULL ) 
        RETURN ( NULL ) ;
    c2c_trim_t1 ( trim_curve, t1 ) ;
    RETURN ( trim_curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_trim_t ( curve, t0, t1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL t0, t1 ;
{
    C2_CURVE trim_curve = c2d_copy ( curve ) ;
    if ( trim_curve == NULL ) 
        RETURN ( NULL ) ;
    c2c_trim_t ( trim_curve, t0, t1 ) ;
    RETURN ( trim_curve ) ;
}

