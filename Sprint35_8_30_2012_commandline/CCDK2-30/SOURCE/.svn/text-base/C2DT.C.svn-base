/* -2 -3 */
/******************************* C2D.C **********************************/
/********** Two-dimensional curve construction routines *****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <c2cdefs.h>
#include <c2ddefs.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_translate ( curve0, shift ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve0 ;
PT2 shift ;
{
    C2_CURVE curve ;

    curve = c2d_copy ( curve0 ) ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    c2c_translate ( curve, shift ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_rotate ( curve0, origin, angle ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve0 ;
PT2 origin ;
REAL angle ;
{
    C2_CURVE curve ;
    curve = c2d_copy ( curve0 ) ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    c2c_rotate ( curve, origin, angle ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_rotate_cs ( curve0, origin, c, s ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve0 ;
PT2 origin ;
REAL c, s ;
{
    C2_CURVE curve ;
    curve = c2d_copy ( curve0 ) ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    c2c_rotate_cs ( curve, origin, c, s ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_scale ( curve0, origin, factor ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve0 ;
REAL factor ;
PT2 origin ;
{
    C2_CURVE curve ;
    curve = c2d_copy ( curve0 ) ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    c2c_scale ( curve, origin, factor ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_mirror_line ( curve0, line ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve0 ;
PT2 line[2] ;
{
    C2_CURVE curve ;
    curve = c2d_copy ( curve0 ) ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    c2c_mirror_line ( curve, line ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_mirror_dir ( curve0, origin, direction ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve0 ;
PT2 origin, direction ;
{
    C2_CURVE curve ;
    curve = c2d_copy ( curve0 ) ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    c2c_mirror_dir ( curve, origin, direction ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_transform ( curve0, t ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve0 ;
C2_TRANSFORM t ;
{
    C2_CURVE curve ;
    curve = c2d_copy ( curve0 ) ;
    if ( c2c_transform ( curve, t ) == NULL ) {
        c2d_free_curve ( curve ) ;
#ifdef SPLINE
        if ( C2_CURVE_IS_ARC(curve) || C2_CURVE_IS_PCURVE(curve) ) {
            curve = c2d_curve_to_spline ( curve0 ) ;
            if ( curve == NULL ) 
                RETURN ( NULL ) ;
            if ( c2c_transform ( curve, t ) == NULL ) {
                c2d_free_curve ( curve ) ;
                RETURN ( NULL ) ;
            }
            else 
                RETURN ( curve ) ;
        }
        else
            RETURN ( NULL ) ;
#else
        RETURN ( NULL ) ;
#endif
    }
    else 
        RETURN ( curve ) ;
}

