/* -2 -3 */
/********************************* V2D1.C **********************************/
/***************** Display of 2-dim geometry in a viewport *****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2cdefs.h>
#include <c2cpriv.h>
#include <c2ddefs.h>
#include <c2ndefs.h>
#include <c2vdefs.h>
#include <grrdefs.h>
#include <v2ddefs.h>
#include <v2dpriv.h>
#include <vpidefs.h>
#include <vp2defs.h>
#include <c2vmcrs.h>
#include <vpmcrs.h>
STATIC REAL v2d_r __(( REAL, REAL, REAL, REAL )) ;

/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN v2d_select ( curve, pt, viewport, sel_parm, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 pt ;
VP_VIEWPORT viewport ;
PARM sel_parm ;
REAL *dist_ptr ;
{
    RETURN ( c2c_select ( curve, pt, vpi_get_select_width ( viewport ), 
        sel_parm, dist_ptr ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void v2d_poly ( a, n, viewport ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a  ;
INT n ;
VP_VIEWPORT viewport ;
{
    PT2 p ;
    INT i ;

    if ( n > 1 ) {
        V2D_PT_TRANSFORM ( a[0], viewport, p ) ;
        grr_moveto ( p ) ;
        for ( i=1 ; i<n ; i++ ) {
            V2D_PT_TRANSFORM ( a[i], viewport, p ) ;
            grr_lineto ( p ) ;
        }
    }
}            


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void v2d_curve_dir ( curve, dir, viewport ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
INT dir ;
VP_VIEWPORT viewport ;
{
    PT2 pt, tan_vec ;

    if ( c2c_midpt_dir ( curve, dir, pt, tan_vec ) )
        v2d_arrow ( pt, tan_vec, 0.0125, 0.005, 0.008, viewport ) ;
}


#ifdef SPLINE
/*-------------------------------------------------------------------------*/
BBS_PRIVATE void v2d_hpoly ( a, n, viewport ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a  ;
INT n ;
VP_VIEWPORT viewport ;
{
    PT2 p0, p ;
    INT i, i0=0 ;

    for ( i=0 ; i<n && a[i][0] <= BBS_ZERO ; i++ ) 
        i0 = i+1 ;
    if ( i0 >= n ) 
        RETURN ;

    p0[0] = a[i0][0] / a[i0][2] ;
    p0[1] = a[i0][1] / a[i0][2] ;
    V2D_PT_TRANSFORM ( p0, viewport, p ) ;
    grr_moveto ( p ) ;

    for ( i=i0+1 ; i<n ; i++ ) {
        if ( a[i][2] > BBS_ZERO ) {
            p0[0] = a[i][0] / a[i][2] ;
            p0[1] = a[i][1] / a[i][2] ;
            V2D_PT_TRANSFORM ( p0, viewport, p ) ;
            grr_lineto ( p ) ;
        }
    }
}            


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void v2d_ctl_poly ( curve, viewport ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
VP_VIEWPORT viewport ;
{
    HPT2 *c ;
    INT n ;

    if ( C2_CURVE_IS_SPLINE(curve) ) {
        c = c2n_get_ctpt ( C2_CURVE_NURB(curve) ) ;
        n = c2n_get_n ( C2_CURVE_NURB(curve) ) ;
        v2d_hpoly ( c, n, viewport ) ;
    }
}
#endif /*SPLINE*/

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void v2d_curve_box ( curve, viewport ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
VP_VIEWPORT viewport ;
{
    v2d_box ( C2_CURVE_BOX(curve), viewport ) ; 
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void v2d_box ( box, viewport ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_BOX box ;
VP_VIEWPORT viewport ;
{
    PT2 p0, p1 ;

    V2D_PT_TRANSFORM ( C2_MIN_PT(box), viewport, p0 ) ;
    V2D_PT_TRANSFORM ( C2_MAX_PT(box), viewport, p1 ) ;
    grr_box ( p0, p1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void v2d_line ( pt0, pt1, viewport ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1 ;
VP_VIEWPORT viewport ;
{
    v2d_moveto ( pt0, viewport ) ;
    v2d_lineto ( pt1, viewport ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void v2d_inf_line ( pt, angle, vp ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt ;
REAL angle ;
VP_VIEWPORT vp ;
{
    PT2 p0, p1 ;
    INT clipped ;

    clipped = VP_VIEWPORT_CLIPPED(vp) ;
    if ( !clipped )
        vpi_clip ( vp ) ;
    v2d_define_inf_line ( pt, angle, p0, p1, vp ) ;
    v2d_line ( p0, p1, vp ) ;
    if ( !clipped ) 
        vpi_unclip ( vp ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void v2d_define_inf_line ( pt, angle, p0, p1, vp ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt ;
REAL angle ;
PT2 p0, p1 ;
VP_VIEWPORT vp ;
{
    REAL a1, b1, a2, b2, c, s, r0, r1 ;

    c = cos ( angle ) ;
    s = sin ( angle ) ;
    if ( s < 0.0 && c  <= 1.0 - BBS_ZERO ) {
        s = -s ;
        c = -c ;
    }

    a1 = VP_VIEWPORT_TRANSFORM(vp)[0][0] * c + 
         VP_VIEWPORT_TRANSFORM(vp)[0][1] * s ;
    a2 = VP_VIEWPORT_TRANSFORM(vp)[1][0] * c + 
         VP_VIEWPORT_TRANSFORM(vp)[1][1] * s ;
    if ( pt == NULL ) {
        b1 = VP_VIEWPORT_TRANSFORM(vp)[0][3] ;
        b2 = VP_VIEWPORT_TRANSFORM(vp)[1][3] ;
    }
    else {
        b1 = VP_VIEWPORT_TRANSFORM(vp)[0][0] * pt[0] + 
             VP_VIEWPORT_TRANSFORM(vp)[0][1] * pt[1] + 
             VP_VIEWPORT_TRANSFORM(vp)[0][3] ;
        b2 = VP_VIEWPORT_TRANSFORM(vp)[1][0] * pt[0] + 
             VP_VIEWPORT_TRANSFORM(vp)[1][1] * pt[1] + 
             VP_VIEWPORT_TRANSFORM(vp)[1][3] ;
    }
    r0 = v2d_r ( a1, b1 + 1.0, a2, b2 + 1.0 ) ;
    r1 = v2d_r ( a1, 2.0 - b1, a2, 2.0 - b2 ) ;

    if ( pt == NULL ) {
        C2V_SET ( - r0*c, - r0*s, p0 ) ;
        C2V_SET ( r1*c, r1*s, p1 ) ;
    }
    else {
        C2V_SET ( pt[0] - r0*c, pt[0] - r0*s, p0 ) ;
        C2V_SET ( pt[0] + r1*c, pt[0] + r1*s, p1 ) ;
    }
}


/*-------------------------------------------------------------------------*/
STATIC REAL v2d_r ( a1, b1, a2, b2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL a1, b1, a2, b2 ;
{
    REAL c1, c2 ;
/*    r * a1 > b1 ; r * a2 > b2 ; */
    if ( IS_SMALL(a1) ) {
        if ( IS_SMALL(a2) ) 
            RETURN ( 0.0 ) ;
        else
            RETURN ( b2 /a2 ) ;
    }
    else {
        if ( IS_SMALL(a2) ) 
            RETURN ( b1 / a1 ) ;
        else {
            c1 = b1 / a1 ;
            c2 = b2 / a2 ;
            RETURN ( c1 < c2 ? c2 : c1 ) ;
        }
    }
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void v2d_circle ( ctr, rad, viewport ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr ;
REAL rad ;
VP_VIEWPORT viewport ;
{
    C2_CURVE circle ;
    circle = c2d_circle ( ctr, rad ) ;
    v2d_curve ( circle, NULL, NULL, viewport ) ;
    c2d_free_curve ( circle ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void v2d_cross ( pt, a, viewport ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt ;
REAL a ;
VP_VIEWPORT viewport ;
{
    PT2 p0, p1 ;
    C2V_SET ( pt[0]-a, pt[1], p0 ) ;
    C2V_SET ( pt[0]+a, pt[1], p1 ) ;
    v2d_line ( p0, p1, viewport ) ;
    C2V_SET ( pt[0], pt[1]-a, p0 ) ;
    C2V_SET ( pt[0], pt[1]+a, p1 ) ;
    v2d_line ( p0, p1, viewport ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void v2d_arrow ( pt, vec, a, b, h, viewport ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt, vec ;
REAL a, b, h ;
VP_VIEWPORT viewport ;
{
    PT2 p, v ;

    C2V_ADD ( pt, vec, p ) ;
    V2D_PT_TRANSFORM ( p, viewport, v ) ;
    V2D_PT_TRANSFORM ( pt, viewport, p ) ;
    C2V_SUB ( v, p, v ) ;
    c2v_normalize ( v, v ) ;
    grr_arrow ( p, v, a, b, h ) ;    
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN v2d_pt_inside ( viewport, a ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT viewport ;
PT2 a ;
{
    PT2 b ;

    V2D_PT_TRANSFORM ( a, viewport, b ) ;
    RETURN ( vpi_pt_inside ( viewport, b ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void v2d_curve_shift ( curve, shift, vp ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 shift ;
VP_VIEWPORT vp ;
{
    vp2_shift ( vp, shift ) ;
    v2d_curve ( curve, NULL, NULL, vp ) ;
    C2V_NEGATE ( shift, shift ) ;
    vp2_shift ( vp, shift ) ;
    C2V_NEGATE ( shift, shift ) ;
}

