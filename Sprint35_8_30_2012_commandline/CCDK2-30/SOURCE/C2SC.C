/* -2 -3 */
/********************************** C2SC.C *********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef  SPLINE
#include <albdefs.h>
#include <alsdefs.h>
#include <c2bdefs.h>
#include <c2apriv.h>
#include <c2edefs.h>
#include <c2sdefs.h>
#include <fnbdefs.h>
#include <c2vmcrs.h>

STATIC  BOOLEAN c2s_curv_w_root_iter __(( HPT2* DUMMY0 , INT DUMMY1 ,
            INT DUMMY2 , REAL* DUMMY3 , REAL DUMMY4 ,  REAL DUMMY5 ,
            PARM DUMMY6 , PARM DUMMY7 , PARM DUMMY8 , PARM DUMMY9 )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2s_curv_extrs ( a, n, d, knot, curv_extrs_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* This routine computes extremums of the curvature of a b-spline curve */
HPT2    *a  ;
INT     n ;
INT     d ;
REAL    *knot  ;
DML_LIST curv_extrs_list ;
{
    INT m, j, s ;
    HPT2 *a_ptr, *b ;
    BOOLEAN non_rational ;

    non_rational = c2a_non_rational ( a, n ) ;
    b = CREATE ( d, HPT2 ) ;
    if ( b== NULL )
        RETURN ( -1 ) ;
    m = 0 ;
    s = 0 ;

    for ( j=d-1, a_ptr=a ; j<n ; j++, a_ptr++ ) {
        if ( knot[j+1]-knot[j] > BBS_ZERO ) {

            C2S_CONV_BEZ ( a, d, knot, j, b ) ;
            if ( non_rational ) 
                m += c2b_curv_extrs_nonrat ( b, d, knot[j], knot[j+1], j, 
                    &s, curv_extrs_list ) ; 
            else 
                m += c2b_curv_extrs_rat ( b, d, knot[j], knot[j+1], j, 
                    &s, curv_extrs_list ) ; 
        }
    }
    KILL ( b ) ;
    RETURN ( m ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c2s_curv_w_root_iter ( a, n, d, knot, w, offset, parm0, parm1, 
    parm_init, parm )        
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* This routine computes a root of the equation 1 + kùoffset = 0 , which is **/
/* where the curvature of the offset curve changes sign */
HPT2    *a  ;  /* Control points of the spline */
INT     n ;    /* Number of control points of the spline */
INT     d ;    /* Order of the spline */
REAL    *knot; /* Knots of the spline */
REAL    w ;    /* Offset of the initial spline */
REAL    offset ;    /* Offset of the spline */
PARM    parm0, parm1 ;
PARM    parm_init;/* Initial parameter along the spline */
PARM    parm;  /* Parameter of the point with a given curvature */
{
    PT2 x[4] ;
    REAL w1, delta, kappa, kappa_w, kappa1, kappa0=0.0,
        dot_11, dot_12, norm_cube, cross_12, cross_13 ;
    INT count ;

    w1 = offset + w ;       /* Combined offset */
    COPY_PARM ( parm_init, parm ) ;
    
    delta = 1.0 ;
    for ( count=0 ; count<MAX_COUNT && ( fabs(delta) > 1e-7 ) ; count++ ) {
    
        if ( !c2s_eval ( a, n, d, knot, 0.0, parm, 3, x ) )
            RETURN ( FALSE ) ;
        dot_11 = C2V_DOT ( x[1], x[1] ) ;
        if ( IS_SMALL(dot_11 ) ) 
            RETURN ( FALSE ) ;
        dot_12 = C2V_DOT ( x[1], x[2] ) ;
        norm_cube = dot_11 * sqrt ( dot_11 ) ;
        cross_12 = C2V_CROSS ( x[1], x[2] ) ;
        cross_13 = C2V_CROSS ( x[1], x[3] ) ;
        kappa = cross_12 / norm_cube ;
        kappa_w = 1.0 + w1 * kappa ;
        if ( ( count > 0 ) && ( fabs(kappa_w) > fabs(kappa0) ) ) {
            COPY_PARM ( parm_init, parm ) ;
            RETURN ( FALSE ) ;
        }
        kappa0 = kappa_w ;
        kappa1 = cross_13 / norm_cube - 3.0 * dot_12 * kappa / dot_11 ;
    
        if ( IS_SMALL(kappa1) ) 
            RETURN ( FALSE ) ;
        if ( IS_SMALL(w1) )
            delta = - kappa / kappa1 ;
        else 
            delta = - kappa_w / ( w1 * kappa1 ) ;
        PARM_T(parm) += delta ;
        
        if ( PARM_T(parm) < PARM_T(parm0) ) {
            if ( count==0 ) {
                COPY_PARM ( parm_init, parm ) ;
                RETURN ( FALSE ) ;
            }
            else if ( count <= 2 ) 
                PARM_T(parm) = 0.5 * ( PARM_T(parm0) + PARM_T(parm) - delta ) ;
            else {
                PARM_T(parm) = 0.5 * ( PARM_T(parm0) + PARM_T(parm1) ) ;
                RETURN ( FALSE ) ;
            }
        }   
        else if ( PARM_T(parm) > PARM_T(parm1) ) {
            if ( count==0 ) {
                COPY_PARM ( parm_init, parm ) ;
                RETURN ( FALSE ) ;
            }
            else if ( count <= 2 ) 
                PARM_T(parm) = 0.5 * ( PARM_T(parm1) + PARM_T(parm) - delta ) ;
            else {
                PARM_T(parm) = 0.5 * ( PARM_T(parm0) + PARM_T(parm1) ) ;
                RETURN ( FALSE ) ;
            }
        }   
    }

    if ( fabs(delta) > 1e-7 ) 
        RETURN ( FALSE ) ;
    while ( c2s_curvature ( a, d, knot, w, parm, &kappa ) && 
        1.0 + offset * kappa < 0.0 ) {
        if ( kappa1 * offset > 0.0 ) 
            PARM_T(parm) += fabs ( delta ) ;
        else
            PARM_T(parm) -= fabs ( delta ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2s_curvature ( a, d, knot, w, parm, curv_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *a  ;  /* Control points of the spline */
INT     d ;    /* Order of the spline */
REAL    *knot; /* Knots of the spline */
REAL    w ;    /* Offset distance */
PARM    parm ;
REAL    *curv_ptr ;
{
    HPT2 x[3] ;
    REAL c ;

    als_eval ( (REAL*)(a+PARM_J(parm)-d+1), d, knot+PARM_J(parm)-d+1, 3, 
        PARM_T(parm), 2, (REAL*)x ) ;
    if ( !c2e_curvature ( x, curv_ptr ) )           /* coordinates */
        RETURN ( FALSE ) ;
    if ( IS_SMALL(w) ) 
        RETURN ( TRUE ) ;

    /* curvw = curv0 / ( 1 + w * curv0 ) */
    c = 1.0 + w * *curv_ptr ;
    if ( IS_SMALL ( c ) ) 
        RETURN ( FALSE ) ;
    *curv_ptr /= c ;
    RETURN ( TRUE ) ;
}    
    

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2s_curv_w_root ( a, n, d, knot, w, offset, 
            parm0, parm1, root_parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/********* this routine computes roots of 1+offsetùcurvature(w) ) **********/
HPT2    *a  ;  /* Control points of the spline */
INT     n ;    /* Number of control points of the spline */
INT     d ;    /* Order of the spline */
REAL    *knot; /* Knots of the spline */
REAL    w ;    /* Offset of the original spline */
REAL    offset ;    /* Offset of the spline */
PARM    parm0, parm1, root_parm ;
{
    PARM_S parm ;
    REAL curvature, curvature0 ;
#ifdef DEBUG
    REAL curvature1 ;
#endif

    PARM_T(&parm) = 0.5 * ( PARM_T(parm0) + PARM_T(parm1) ) ;
    PARM_J(&parm) = ( PARM_J(parm0) + PARM_J(parm1) ) / 2 ;
    als_parm_adjust ( n, d, knot, &parm ) ;
    if ( c2s_curv_w_root_iter ( a, n, d, knot, w, offset, parm0, parm1, 
            &parm, root_parm ) ) 
        RETURN ( TRUE ) ;

    /* If root has not been found */

    COPY_PARM ( root_parm, &parm ) ;
#ifdef DEBUG
    c2s_curvature ( a, d, knot, w, parm1, &curvature1 ) ;
    curvature1 = 1.0 + offset * curvature1 ;
#endif

    if ( !c2s_curvature ( a, d, knot, w, &parm, &curvature ) || 
         !c2s_curvature ( a, d, knot, w, parm0, &curvature0 ) ) 
        RETURN ( c2s_curv_w_root ( a, n, d, knot, w, offset, 
            parm0, &parm, root_parm ) || c2s_curv_w_root ( a, n, d, knot, 
            w, offset, &parm, parm1, root_parm ) ) ;

    curvature = 1.0 + offset * curvature ;
    curvature0 = 1.0 + offset * curvature0 ;

    if ( (curvature>0.0) == (curvature0>0.0) ) 
        RETURN ( c2s_curv_w_root ( a, n, d, knot, w, offset, 
            &parm, parm1, root_parm ) ) ;/* no root between parm0 & parm */
    else 
        RETURN ( c2s_curv_w_root ( a, n, d, knot, w, offset, 
            parm0, &parm, root_parm ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2s_coord_extrs ( a, d, knot, parm0, parm1, coord, extr_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* This routine computes extremums of the specified coordinate 
of a b-spline curve */
HPT2    *a ;
INT     d ;
REAL    *knot ;
PARM    parm0, parm1 ;    /* Start and end parameters */
INT     coord ; /* 0 or 1 */
DML_LIST extr_list ;
{
    INT extr_no, n, i, j ;
    HREAL *b, *c ;

    n = DML_LENGTH(extr_list) ;

    b = CREATE ( d, HREAL ) ;
    if ( b== NULL )
        RETURN ( -1 ) ;
    c = CREATE ( d, HREAL ) ;
    if ( c== NULL )
        RETURN ( -1 ) ;

    for ( i = 0, j = PARM_J(parm0)-d+1 ; i < d ; j++, i++ ) {
        c[i][0] = a[j][coord] ;
        c[i][1] = a[j][2] ;
    }

    j = PARM_J(parm0) ;
    als_conv_bez ( (REAL*)c, d, knot+j-d+1, 2, (REAL*)b ) ;

    if ( PARM_T(parm0) > knot[j] + BBS_ZERO ) 
        alb_brkr ( (REAL*)b, d, 2, ( PARM_T(parm0) - knot[j] ) / 
            ( knot[j+1] - knot[j] ), (REAL*)b ) ;

    if ( PARM_J(parm1) == j ) {
        alb_brkl ( (REAL*)b, d, 2, ( PARM_T(parm1) - PARM_T(parm0) ) / 
            ( knot[j+1] - PARM_T(parm0) ), (REAL*)b ) ;
        fnb_extrs ( b, d, PARM_T(parm0), PARM_T(parm1), j, extr_list ) ;
    }

    else if ( knot[j+1] >  PARM_T(parm0) + BBS_ZERO ) { 
        fnb_extrs ( b, d, PARM_T(parm0), knot[j+1], j, extr_list ) ;

        for ( j=PARM_J(parm0)+1 ; j<PARM_J(parm1) ; j++ ) {

            for ( i=1 ; i<d ; i++ ) {
                c[i-1][0] = c[i][0] ;
                c[i-1][1] = c[i][1] ;
            }
            c[d-1][0] = a[j][coord] ;
            c[d-1][1] = a[j][2] ;

            if ( knot[j+1]-knot[j] > BBS_ZERO ) {
                als_conv_bez ( (REAL*)c, d, knot+j-d+1, 2, (REAL*)b ) ;
                fnb_extrs ( b, d, knot[j], knot[j+1], j, extr_list ) ;
            }
        }

        j = PARM_J(parm1) ;

        if ( PARM_T(parm1) - knot[j] > BBS_ZERO ) {
            for ( i=1 ; i<d ; i++ ) {
                c[i-1][0] = c[i][0] ;
                c[i-1][1] = c[i][1] ;
            }
            c[d-1][0] = a[j][coord] ;
            c[d-1][1] = a[j][2] ;
            als_conv_bez ( (REAL*)c, d, knot+j-d+1, 2, (REAL*)b ) ;

            if ( PARM_T(parm1) < knot[j+1] - BBS_ZERO )
                alb_brkl ( (REAL*)b, d, 2, ( PARM_T(parm1) - knot[j] ) / 
                    ( knot[j+1] - knot[j] ), (REAL*)b ) ;
            fnb_extrs ( b, d, knot[j], PARM_T(parm1), j, extr_list ) ;
        }
    }
    KILL ( b ) ;
    KILL ( c ) ;

    extr_no = DML_LENGTH(extr_list) - n ;
    RETURN ( extr_no ) ;
}
#endif  /*SPLINE*/

