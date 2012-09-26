/* -2 -3 */
/******************************* C2CT.C *********************************/ 
/**************** Two-dimensional curves ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2gdefs.h>
#include <c2ldefs.h>
#include <c2ndefs.h>
#include <c2pdefs.h>
#include <c2tdefs.h>
#include <c2vdefs.h>
#include <c2cdefs.h>
#include <c2vmcrs.h>
#include <c2mem.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2c_translate ( curve, shift ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 shift ;
{
    if ( C2_CURVE_IS_LINE(curve) ) {
        c2l_translate ( C2_CURVE_LINE(curve), shift, C2_CURVE_LINE(curve) ) ;
        c2l_box ( C2_CURVE_LINE(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
    }
    else if ( C2_CURVE_IS_ARC(curve) ) {
        c2g_translate ( C2_CURVE_ARC(curve), shift, C2_CURVE_ARC(curve) ) ;
        c2g_box ( C2_CURVE_ARC(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
    }
    else if ( C2_CURVE_IS_PCURVE(curve) ) {
        c2p_translate ( C2_CURVE_PCURVE(curve), shift ) ;
        c2p_box ( C2_CURVE_PCURVE(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
    }
#ifdef  SPLINE
    else {
        c2n_translate ( C2_CURVE_NURB(curve), shift ) ;
        c2n_box ( C2_CURVE_NURB(curve), C2_CURVE_BOX(curve) ) ;
    }
#endif  /* SPLINE */
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2c_rotate ( curve, origin, angle ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 origin ;
REAL angle ;
{
    REAL c, s ;

    c = cos ( angle ) ;
    s = sin ( angle ) ;
    c2c_rotate_cs ( curve, origin, c, s ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2c_rotate_cs ( curve, origin, c, s ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 origin ;
REAL c, s ;
{
    if ( C2_CURVE_IS_LINE(curve) ) {
        c2l_rotate_cs ( C2_CURVE_LINE(curve), origin, 
            c, s, C2_CURVE_LINE(curve) ) ;
        c2l_box ( C2_CURVE_LINE(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
    }

    else if ( C2_CURVE_IS_ARC(curve) ) {
        c2g_rotate_cs ( C2_CURVE_ARC(curve), origin, 
            c, s, C2_CURVE_ARC(curve) ) ;
        c2g_box ( C2_CURVE_ARC(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
    }
    else if ( C2_CURVE_IS_PCURVE(curve) ) {
        c2p_rotate_cs ( C2_CURVE_PCURVE(curve), origin, 
                c, s, C2_CURVE_PCURVE(curve) ) ;
        c2p_box ( C2_CURVE_PCURVE(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
    }

#ifdef  SPLINE
    else {
        c2n_rotate_cs ( C2_CURVE_NURB(curve), origin, c, s ) ;
        c2n_box ( C2_CURVE_NURB(curve), C2_CURVE_BOX(curve) ) ;
    }
#endif  /* SPLINE */
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2c_scale ( curve, origin, factor ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 origin ;
REAL factor ;
{
    PT2 a ;

    a[0] = ( 1.0 - factor ) * origin[0] ;
    a[1] = ( 1.0 - factor ) * origin[1] ;

    if ( C2_CURVE_IS_LINE(curve) ) {
        c2l_scale ( C2_CURVE_LINE(curve), a, factor, C2_CURVE_LINE(curve) ) ;
        c2l_box ( C2_CURVE_LINE(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
    }
    else if ( C2_CURVE_IS_ARC(curve) ) {
        c2g_scale ( C2_CURVE_ARC(curve), a, factor, C2_CURVE_ARC(curve) ) ;
        c2g_box ( C2_CURVE_ARC(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
    }
    else if ( C2_CURVE_IS_PCURVE(curve) ) {
        c2p_scale ( C2_CURVE_PCURVE(curve), origin, factor ) ;
        c2p_box ( C2_CURVE_PCURVE(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
    }
#ifdef  SPLINE
    else {
        c2n_scale ( C2_CURVE_NURB(curve), a, factor ) ;
        c2n_box ( C2_CURVE_NURB(curve), C2_CURVE_BOX(curve) ) ;
    }
#endif  /* SPLINE */
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2c_mirror_line ( curve, line ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 line[2] ;
{
    PT2 origin, direction ;

    C2V_COPY ( line[0], origin ) ;
    direction[0] = line[1][1] - line[0][1] ;
    direction[1] = line[0][0] - line[1][0] ;
    c2v_normalize ( direction, direction ) ;
    c2c_mirror_dir ( curve, origin, direction ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2c_mirror_dir ( curve, origin, normal ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 origin, normal ;
{
    if ( C2_CURVE_IS_LINE(curve) ) {
        c2l_mirror ( C2_CURVE_LINE(curve), origin, normal, 
            C2_CURVE_LINE(curve) ) ;
        c2l_box ( C2_CURVE_LINE(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
    }

    else if ( C2_CURVE_IS_ARC(curve) ) {
        c2g_mirror ( C2_CURVE_ARC(curve), origin, normal, 
            C2_CURVE_ARC(curve) ) ;
        c2g_box ( C2_CURVE_ARC(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
    }
    else if ( C2_CURVE_IS_PCURVE(curve) ) {
        c2p_mirror_dir ( C2_CURVE_PCURVE(curve), origin, normal ) ;
        c2p_box ( C2_CURVE_PCURVE(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
    }
#ifdef  SPLINE
    else {
        c2n_mirror ( C2_CURVE_NURB(curve), origin, normal ) ;
        c2n_box ( C2_CURVE_NURB(curve), C2_CURVE_BOX(curve) ) ;
    }
#endif  /* SPLINE */
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2c_transform ( curve, t ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
C2_TRANSFORM t ;
{
#ifdef  SPLINE
    C2_NURB nurb ;
#endif /*SPLINE*/
    if ( C2_CURVE_IS_LINE(curve) ) {
        c2l_transform ( C2_CURVE_LINE(curve), t, C2_CURVE_LINE(curve) ) ;
        c2l_box ( C2_CURVE_LINE(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
    }
    else if ( C2_CURVE_IS_ARC(curve) ) {
        if ( c2g_transform ( C2_CURVE_ARC(curve), t, C2_CURVE_ARC(curve) ) ) {
            c2g_box ( C2_CURVE_ARC(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
            RETURN ( curve ) ;
        }
#ifdef SPLINE
        else {
            nurb = c2n_arc ( C2_CURVE_ARC(curve), C2_CURVE_IS_EXT_ARC(curve) ) ;
            if ( nurb == NULL ) 
                RETURN ( NULL ) ;
            c2n_transform ( nurb, t ) ;
            C2_FREE_ARC ( C2_CURVE_ARC(curve) ) ;
            C2_CURVE_NURB(curve) = nurb ;
            C2_CURVE_TYPE(curve) = C2_ELLIPSE_TYPE ;
            c2n_box ( nurb, C2_CURVE_BOX(curve) ) ;
            C2_CURVE_J0(curve) = 2 ;
            C2_CURVE_J1(curve) = c2n_get_n ( nurb ) - 1 ;
            RETURN ( curve ) ;
        }
#else
        else 
            RETURN ( NULL ) ;
#endif /*SPLINE*/ 
    }
    else if ( C2_CURVE_IS_PCURVE(curve) ) {
        c2p_transform ( C2_CURVE_PCURVE(curve), t, C2_CURVE_PCURVE(curve) ) ;
        c2p_box ( C2_CURVE_PCURVE(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
    }
#ifdef  SPLINE
    else {
        if ( !c2n_transform ( C2_CURVE_NURB(curve), t ) )
            RETURN ( NULL ) ;
        c2n_box ( C2_CURVE_NURB(curve), C2_CURVE_BOX(curve) ) ;
    }
#endif  /* SPLINE */
    RETURN ( curve ) ;
}

