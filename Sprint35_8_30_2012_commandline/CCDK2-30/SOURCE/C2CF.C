/* -2 -3 */
/******************************* C2CF.C *********************************/ 
/***************** Two-dimensional arcs *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#define     C2CF_MAX_STEP .2 
#define     C2CF_MAX_COUNT 20
#include <c2vdefs.h>
#include <c2cdefs.h>
#include <c2gdefs.h>
#include <c2vmcrs.h>

#ifdef DDEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif

STATIC  BOOLEAN tangent_circle_rad __(( ANY DUMMY0 , PARM DUMMY1 , 
            BOOLEAN DUMMY2 , ANY DUMMY3 , PARM DUMMY4 , BOOLEAN DUMMY5 , 
            REAL DUMMY6 , PT2 DUMMY7 , PARM DUMMY8 , PT2 DUMMY9 , 
            PARM DUMMY10 , PT2  DUMMY11 )) ;
STATIC  BOOLEAN  project_step __(( ANY DUMMY0 , PARM DUMMY1 , BOOLEAN DUMMY2 , 
            PT2 DUMMY3 , PT2  DUMMY4 [3] )) ;
STATIC  void     check_parm __(( C2_CURVE DUMMY0 , REAL*  DUMMY1 )) ;
STATIC REAL fillet_crtr __(( PT2 DUMMY0 , PT2 DUMMY1 [] , PT2 DUMMY2 [] )) ;
STATIC REAL fillet_crtr1 __(( PT2 DUMMY0 , PT2 DUMMY1 [] )) ;


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_fillet ( curve1, parm1, curve2, parm2, 
            rad, pt1, pt2, ctr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve1, curve2 ;
PARM parm1, parm2 ;
REAL rad ;
PT2 pt1, pt2, ctr ;
{
    RETURN ( tangent_circle_rad ( curve1, parm1, TRUE, 
        curve2, parm2, TRUE, rad, pt1, parm1, pt2, parm2, ctr ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_fillet_corner ( curve1, curve2, rad, 
            parm1, parm2, pt1, pt2, ctr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve1, curve2 ;
REAL rad ;
PARM parm1, parm2 ;
PT2 pt1, pt2, ctr ;
{
    PT2 p1_0, p1_1, p2_0, p2_1, p, tan1, tan2 ;
    REAL *p1, *p2, n1, n2, n, dist, dist_min, cross ;
    INT i1, i2 ;
    PARM_S parms1, parms2 ;

    if ( parm1==NULL ) 
        parm1 = &parms1 ;
    if ( parm2==NULL ) 
        parm2 = &parms2 ;

    if ( !c2c_ept0 ( curve1, p1_0 ) )
        RETURN ( FALSE ) ;
    if ( !c2c_ept1 ( curve1, p1_1 ) )
        RETURN ( FALSE ) ;
    if ( !c2c_ept0 ( curve2, p2_0 ) )
        RETURN ( FALSE ) ;
    if ( !c2c_ept1 ( curve2, p2_1 ) )
        RETURN ( FALSE ) ;
    dist = c2v_dist_squared ( p1_0, p2_0 ) ;
    dist_min = dist ;
    i1 = 0 ;
    i2 = 0 ;
    dist = c2v_dist_squared ( p1_1, p2_0 ) ;
    if ( dist < dist_min ) {
        i1 = 1 ;
        i2 = 0 ;
        dist_min = dist ;
    }
    dist = c2v_dist_squared ( p1_0, p2_1 ) ;
    if ( dist < dist_min ) {
        i1 = 0 ;
        i2 = 1 ;
        dist_min = dist ;
    }
    dist = c2v_dist_squared ( p1_1, p2_1 ) ;
    if ( dist < dist_min ) {
        i1 = 1 ;
        i2 = 1 ;
        dist_min = dist ;
    }
    if ( i1 == 0 ) {
        if ( !c2c_etan0 ( curve1, tan1 ) )
            RETURN ( FALSE ) ;
        p1 = p1_0 ;
        PARM_COPY ( C2_CURVE_PARM0(curve1), parm1 ) ;
    }
    else {
        if ( !c2c_etan1 ( curve1, tan1 ) )
            RETURN ( FALSE ) ;
        C2V_NEGATE ( tan1, tan1 ) ;
        p1 = p1_1 ;
        PARM_COPY ( C2_CURVE_PARM1(curve1), parm1 ) ;
    }
    if ( i2 == 0 ) {
        if ( !c2c_etan0 ( curve2, tan2 ) )
            RETURN ( FALSE ) ;
        p2 = p2_0 ;
        PARM_COPY ( C2_CURVE_PARM0(curve2), parm2 ) ;
    }
    else {
        if ( !c2c_etan1 ( curve2, tan2 ) )
            RETURN ( FALSE ) ;
        C2V_NEGATE ( tan2, tan2 ) ;
        p2 = p2_1 ;
        PARM_COPY ( C2_CURVE_PARM1(curve2), parm2 ) ;
    }
    C2V_SUB ( p2, p1, p ) ;
    n1 = C2V_NORM ( tan1 ) ;
    n2 = C2V_NORM ( tan2 ) ;
    cross = C2V_CROSS ( tan1, tan2 ) ;
    if ( fabs(cross) <= BBS_ZERO * n1 * n2 )
        RETURN ( FALSE ) ;
    n = rad * ( n1 * n2 - C2V_DOT ( tan1, tan2 ) ) ;
    if ( cross < 0.0 ) 
        n = -n ;
    if ( i1 == 0 ) 
        PARM_T(parm1) += ( C2V_CROSS ( p, tan2 ) + n / n1 ) / cross ;
    else 
        PARM_T(parm1) -= ( C2V_CROSS ( p, tan2 ) + n / n1 ) / cross ;
    if ( i2 == 0 ) 
        PARM_T(parm2) += ( C2V_CROSS ( p, tan1 ) + n / n2 ) / cross ;
    else 
        PARM_T(parm2) -= ( C2V_CROSS ( p, tan1 ) + n / n2 ) / cross ;
    RETURN ( c2c_fillet ( curve1, parm1, curve2, parm2, 
        rad, pt1, pt2, ctr ) ) ;
}


/*----------------------------------------------------------------------*/
STATIC BOOLEAN tangent_circle_rad ( curve1, parm1_init, curve1_id, 
        curve2, parm2_init, curve2_id, rad, pt1, parm1, pt2, parm2, ctr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY curve1, curve2 ;
PARM parm1_init, parm2_init ;
BOOLEAN curve1_id, curve2_id ;
REAL rad ;
PT2 pt1, pt2, ctr ;
PARM parm1, parm2 ;
{
    C2_CURVE crv1=NULL, crv2=NULL ;
    PT2 x1[3], x2[3], c1, c2, diff, diff1 ;
    INT count ;
    BOOLEAN status1, status2 ;
    PARM_S parms1, parms2 ;
    REAL a, b, c, q, s, u, d1, d2 ;

    if ( !curve1_id || !curve2_id ) 
        RETURN ( FALSE ) ;
    if ( parm1==NULL ) 
        parm1 = &parms1 ;
    if ( parm2==NULL ) 
        parm2 = &parms2 ;

    if ( curve1_id ) {
        PARM_COPY ( parm1_init, parm1 ) ; 
        c2c_eval ( (C2_CURVE)curve1, parm1, 2, x1 ) ;
        crv1 = (C2_CURVE)curve1 ;
    }
    else 
        c2v_copy ( curve1, x1[0] ) ;

    if ( curve2_id ) {
        PARM_COPY ( parm2_init, parm2 ) ; 
        c2c_eval ( (C2_CURVE)curve2, parm2, 2, x2 ) ;
        crv2 = (C2_CURVE)curve2 ;
    }
    else 
        c2v_copy ( curve2, x2[0] ) ;

    C2V_SUB ( x2[0], x1[0], diff ) ;

    if ( C2V_IS_SMALL(diff) ) { 

        if ( IS_ZERO ( PARM_T(parm1) - C2_CURVE_T1(crv1) ) ) {
            C2V_NEGATE ( x1[1], x1[1] ) ;
        }
        else if ( !IS_ZERO( PARM_T(parm1) - C2_CURVE_T0(crv1) ) ) 
            RETURN ( FALSE ) ;

        if ( IS_ZERO ( PARM_T(parm2) - C2_CURVE_T1(crv2) ) ) {
            C2V_NEGATE ( x2[1], x2[1] ) ;
        }
        else if ( !IS_ZERO ( PARM_T(parm2) - C2_CURVE_T0(crv2) ) ) 
            RETURN ( FALSE ) ;

        a = C2V_NORM ( x1[1] ) ;
        b = C2V_DOT ( x1[1], x2[1] ) ;
        c = C2V_NORM ( x2[1] ) ;
        q = ( a * c ) / b ;
        if ( IS_ZERO ( 1.0 - q ) ) 
            RETURN ( FALSE ) ;

        q = rad * sqrt ( ( 1.0 + q ) / ( 1.0 - q ) ) ;
        
        if ( IS_ZERO ( PARM_T(parm1) - C2_CURVE_T0(crv1) ) ) 
            PARM_T(parm1) += (q/a) ;
        else 
            PARM_T(parm1) -= (q/a) ;

        if ( IS_ZERO ( PARM_T(parm2) - C2_CURVE_T0(crv2) ) ) 
            PARM_T(parm2) += (q/a) ;
        else 
            PARM_T(parm2) -= (q/a) ;
        c2c_eval ( (C2_CURVE)curve1, parm1, 2, x1 ) ;
        c2c_eval ( (C2_CURVE)curve2, parm2, 2, x2 ) ;
    }

    for ( count = 0 ; count < C2CF_MAX_COUNT ; count++ ) {
        C2V_SUB ( x2[0], x1[0], diff ) ;
        diff1[0] = diff[0] / rad ;
        diff1[1] = diff[1] / rad ;
        if ( IS_ZERO(diff1[0]) && IS_ZERO(diff1[1]) )
            RETURN ( FALSE ) ;
        u = 1.0 / C2V_DOT ( diff1, diff1 ) - 0.25 ;

        if ( IS_ZERO(u) ) {
            status1 = c2g_2pts_tan_ctr ( x1[0], x1[1], x2[0], c1 ) ;
            status2 = c2g_2pts_tan_ctr ( x2[0], x2[1], x1[0], c2 ) ;
            if ( status1 ) {
                if ( status2 ) {
                    C2V_MID_PT ( c1, c2, ctr ) ;
                }
                else {
                    C2V_COPY ( c1, ctr ) ;
                }
            }
            else {
                if ( status2 ) {
                    C2V_COPY ( c2, ctr ) ;
                }
                else {
                    C2V_MID_PT ( x1[0], x2[0], ctr ) ;
                }
            }

            project_step ( curve1, parm1, curve1_id, ctr, x1 ) ;
            project_step ( curve2, parm2, curve2_id, ctr, x2 ) ;
            status1 = TRUE ;
            status2 = TRUE ;
            C2V_MID_PT ( x1[0], x2[0], ctr ) ;
        }
        else if ( u < 0.0 ) {
            c2v_normalize ( x1[1], c1 ) ;
            c2v_normalize ( x2[1], c2 ) ;
            c = C2V_DOT ( c1, c2 ) ;
            s = C2V_CROSS ( c1, c2 ) ;
            a = C2V_CROSS ( diff, c1 ) ;
            b = C2V_CROSS ( diff, c2 ) ;
            d1 = a > 0.0 ? - rad : rad ;
            d2 = b > 0.0 ? rad : - rad ;
            u = ( b + d1 * c - d2 ) / s ;
            ctr[0] = x1[0][0] + u * c1[0] - d1 * c1[1] ;
            ctr[1] = x1[0][1] + u * c1[1] + d1 * c1[0] ;
            status1 = project_step ( curve1, parm1, curve1_id, ctr, x1 ) ;
            status2 = project_step ( curve2, parm2, curve2_id, ctr, x2 ) ;
        }
        else {
            u = sqrt ( u ) ;
            C2V_MID_PT ( x1[0], x2[0], ctr ) ;
            C2V_OFFSET ( ctr, diff, u, c1 ) ;
            C2V_OFFSET ( ctr, diff, -u, c2 ) ;
            d1 = fillet_crtr ( c1, x1, x2 ) ;
            d2 = fillet_crtr ( c2, x1, x2 ) ;
            if ( d1 < d2 ) {
                C2V_COPY ( c1, ctr ) ;
            }
            else {
                C2V_COPY ( c2, ctr ) ;
            }
            status1 = project_step ( curve1, parm1, curve1_id, ctr, x1 ) ;
            status2 = project_step ( curve2, parm2, curve2_id, ctr, x2 ) ;
        }
#ifdef DDEBUG
/*
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
*/
    grr_set_color ( 11 ) ;
    w2d_point ( x1[0], .03 ) ;
    grr_set_color ( 12 ) ;
    w2d_point ( x2[0], .03 ) ;
    grr_set_color ( 13 ) ;
    w2d_point ( ctr, .03 ) ;
    getch();
/*
}
else
    DISPLAY-- ;
*/
#endif
        if ( status1 && status2 ) {
            if ( pt1 != NULL ) 
                C2V_COPY ( x1[0], pt1 ) ;
            if ( pt2 != NULL ) 
                C2V_COPY ( x2[0], pt2 ) ;
            RETURN ( TRUE ) ;
        }
    }
    RETURN ( FALSE ) ;
}


/*----------------------------------------------------------------------*/
STATIC REAL fillet_crtr ( c, x1, x2 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 c, x1[], x2[] ;
{
    REAL d1, d2, t, d ;

    d1 = fillet_crtr1 ( c, x1 ) ;
    d2 = fillet_crtr1 ( c, x2 ) ;
    if ( IS_SMALL(d1) && IS_SMALL(d2) )
        RETURN ( 0.0 ) ;
    else if ( d1 > d2 ) {
        t = d2 / d1 ;
        d = d1 * HYPOT ( 1.0, t ) ;
    }
    else {
        t = d1 / d2 ;
        d = d2 * HYPOT ( 1.0, t ) ;
    }
    RETURN ( d ) ;
/*
    RETURN ( ( d1 > d2 ) ? d1 : d2 ) ;
*/
}


/*----------------------------------------------------------------------*/
STATIC REAL fillet_crtr1 ( c, x ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 c, x[] ;
{
    PT2 v, n ;

    C2V_SUB ( c, x[0], v ) ;
    c2v_normalize ( x[1], n ) ;
#ifdef DDEBUG
{
    REAL dot ;
    dot = C2V_DOT ( v, x[1] ) ;
}
#endif
    RETURN ( fabs ( C2V_DOT ( v, x[1] ) ) ) ;
}
    
/*----------------------------------------------------------------------*/
STATIC BOOLEAN project_step ( curve, parm, curve_id, a, x ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

ANY curve ;
PARM parm ;
BOOLEAN curve_id ;
PT2 a, x[3] ;
{
    PT2 vec ;
    REAL t, dot ;
    BOOLEAN status ;

    if ( !curve_id ) 
        RETURN ( TRUE ) ;

    C2V_SUB ( x[0], a, vec ) ;
    dot = C2V_DOT ( vec, x[1] ) ;
    t = - dot / ( C2V_DOT ( x[1], x[1] ) + C2V_DOT ( vec, x[2] ) ) ; 
    dot /= ( C2V_NORML1 ( vec ) * C2V_NORML1 ( x[1] ) ) ;
    if ( t > C2CF_MAX_STEP )
        t = C2CF_MAX_STEP ;
    else if ( t < - C2CF_MAX_STEP )
        t = - C2CF_MAX_STEP ;
    t += PARM_T(parm) ;
    check_parm ( (C2_CURVE)curve, &t ) ;
    status = IS_CONVERGED(PARM_T(parm)-t) ;
    PARM_T(parm) = t ;
    c2c_eval ( (C2_CURVE)curve, parm, 2, x ) ;
    RETURN ( status && IS_ZERO(dot) ) ;    
}


/*----------------------------------------------------------------------*/
STATIC void check_parm ( curve, t_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL *t_ptr ;
{
    if ( *t_ptr < C2_CURVE_T0(curve) ) {
        if ( c2c_closed ( curve ) ) 
            *t_ptr += ( C2_CURVE_T1(curve) - C2_CURVE_T0(curve) ) ;
/*
        else
            *t_ptr = C2_CURVE_T0(curve) ;
*/
    }
    else if ( *t_ptr > C2_CURVE_T1(curve) ) {
        if ( c2c_closed ( curve ) ) 
            *t_ptr -= ( C2_CURVE_T1(curve) - C2_CURVE_T0(curve) ) ;
/*
        else
            *t_ptr = C2_CURVE_T1(curve) ;
*/
    }
}

