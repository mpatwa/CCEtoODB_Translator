/* -2 -3 */
/********************************* V2DF.C **********************************/
/***************** Display of 2-dim geometry in a viewport *****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2cdefs.h>
#include <c2vdefs.h>
#include <v2ddefs.h>
#include <v2dpriv.h>
#include <c2vmcrs.h>
#include <vpmcrs.h>

STATIC void v2d_half_circle __(( PT2, PT2, INT, REAL, VP_VIEWPORT )) ;
STATIC void v2d_normal __(( PT2, PT2, REAL, VP_VIEWPORT )) ;
STATIC void v2d_fat_pass1 __(( C2_CURVE, INT, REAL, VP_VIEWPORT )) ;
STATIC void v2d_fat_line __(( C2_CURVE, INT, REAL, VP_VIEWPORT )) ;

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void v2d_fat_curve ( curve, dir, rad, vp ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
INT dir ;
REAL rad ;
VP_VIEWPORT vp ;
{
    PT2 pt0, pt1, tan0, tan1 ;

    if ( !c2c_ept_tan0 ( curve, pt0, tan0 ) )
        RETURN ;
    if ( !c2c_ept_tan1 ( curve, pt1, tan1 ) )
        RETURN ;
    if ( !c2v_normalize ( tan0, tan0 ) )
        RETURN ;
    if ( !c2v_normalize ( tan1, tan1 ) )
        RETURN ;

    if ( dir == 1 ) 
        v2d_half_circle ( pt0, tan0, 0, rad, vp ) ;
    else {
        C2V_NEGATE ( tan1, tan1 ) ;
        v2d_half_circle ( pt1, tan1, 0, rad, vp ) ;
    }

    v2d_fat_pass1 ( curve, dir, rad, vp ) ;

    if ( dir == 1 ) 
        v2d_half_circle ( pt1, tan1, 1, rad, vp ) ;
    else {
        C2V_NEGATE ( tan0, tan0 ) ;
        v2d_half_circle ( pt0, tan0, 1, rad, vp ) ;
    }
}


/*-------------------------------------------------------------------------*/
STATIC void v2d_fat_pass1 ( curve, dir, rad, vp ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
INT dir ;
REAL rad ;
VP_VIEWPORT vp ;
{
    REAL h, l, cross ;
    PT2 x[3], tan ;
    PARM_S parm ;
    BOOLEAN draw_next ;

    if ( C2_CURVE_IS_LINE(curve) ) {
        v2d_fat_line ( curve, dir, rad, vp ) ;
        RETURN ;
    }

    if ( dir == 1 ) {
        PARM_COPY ( C2_CURVE_PARM0(curve), &parm ) ;
    }
    else
        PARM_COPY ( C2_CURVE_PARM1(curve), &parm ) ;

    draw_next = TRUE ;

    while ( draw_next ) {
        c2c_eval ( curve, &parm, 2, x ) ;
        c2v_normalize ( x[1], tan ) ;
        v2d_normal ( x[0], tan, rad, vp ) ;
        l = C2V_NORM ( x[1] ) ;
        if ( IS_SMALL(l) )
            draw_next = FALSE ;
        else {
            cross = C2V_CROSS ( x[1], x[2] ) ;
            h = VP_VIEWPORT_GRAN(vp) / ( l + rad * fabs(cross) / ( l * l ) ) ;
            if ( dir == 1 ) {
                PARM_T(&parm) += h ;
                draw_next = ( PARM_T(&parm) <= C2_CURVE_T1(curve) ) ;
            }
            else if ( dir == -1 ) {
                PARM_T(&parm) -= h ;
                draw_next = ( PARM_T(&parm) >= C2_CURVE_T0(curve) ) ;
            }
        }
    }
}


/*-------------------------------------------------------------------------*/
STATIC void v2d_fat_line ( curve, dir, rad, vp ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
INT dir ;
REAL rad ;
VP_VIEWPORT vp ;
{
    REAL h, l ;
    PT2 p0, p1, q0, q1, vec, normal ;
    INT i, n ;

    if ( dir == 1 ) {
        c2c_ept0 ( curve, p0 ) ;
        c2c_ept1 ( curve, p1 ) ;
    }
    else {
        c2c_ept0 ( curve, p1 ) ;
        c2c_ept1 ( curve, p0 ) ;
    }

    C2V_SUB ( p1, p0, vec ) ;
    l = C2V_NORM ( vec ) ;
    h = l / VP_VIEWPORT_GRAN(vp) ;
    n = (INT) ( h + 1.0 ) ;
    h = 1.0 / ( ( REAL)n ) ;
    normal[0] = vec[1] / l ;
    normal[1] = - vec[0] / l ;
    C2V_ADDT ( p0, normal, rad, q0 ) ;
    C2V_ADDT ( p0, normal, -rad, q1 ) ;
    C2V_SCALE ( vec, h, vec ) ;

    for ( i = 0 ; i <= n ; i++ ) {
        v2d_line ( q0, q1, vp ) ;
        C2V_ADD ( q0, vec, q0 ) ;
        C2V_ADD ( q1, vec, q1 ) ;
    }
}


/*-------------------------------------------------------------------------*/
STATIC void v2d_half_circle ( ctr, axis, end, rad, vp ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr, axis ;
INT end ;
REAL rad ;
VP_VIEWPORT vp ;
{
    REAL h, w ;
    PT2 q ;

    if ( end == 0 ) {
        for ( h = rad ; h >= 0.0 ; h-= VP_VIEWPORT_GRAN(vp) ) {
            w = sqrt ( rad * rad - h * h ) ;
            C2V_ADDT ( ctr, axis, -h, q ) ;
            v2d_normal ( q, axis, w, vp ) ;
        }
        v2d_normal ( ctr, axis, rad, vp ) ;
    }

    else if ( end == 1 ) {
        for ( h = 0.0 ; h <= rad ; h+= VP_VIEWPORT_GRAN(vp) ) {
            w = sqrt ( rad * rad - h * h ) ;
            C2V_ADDT ( ctr, axis, h, q ) ;
            v2d_normal ( q, axis, w, vp ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
STATIC void v2d_normal ( pt, axis, w, vp ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt, axis ;
REAL w ;
VP_VIEWPORT vp ;
{
    PT2 p0, p1, r ;

    C2V_SET ( w * axis[1], - w * axis[0], r ) ;
    C2V_SUB ( pt, r, p0 ) ;
    C2V_ADD ( pt, r, p1 ) ;
    v2d_line ( p0, p1, vp ) ;
}
