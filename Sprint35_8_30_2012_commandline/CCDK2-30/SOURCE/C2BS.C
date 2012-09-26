/* -2 -3 */
/*********************************** C2BS.C ********************************/
/*********************** Two-dimensional Bezier curves *********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <bbsdefs.h>
#ifdef  SPLINE
#include <c2bdefs.h>
#include <c2apriv.h>
#include <c2vdefs.h>
#include <c2vmcrs.h>
#include <fnbdefs.h>
#include <c2coned.h>

STATIC  BOOLEAN c2bs_select __(( HPT2*, INT, REAL, PT2, REAL, REAL, REAL, 
                    INT, PT2, PT2, INT, REAL*, REAL* )) ;
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2b_select ( b, d, w, pt, tol, t0, t1, t_ptr, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *b  ;  /* Control points of the segment */
INT     d ;    /* Order of the segment */
REAL    w ;    /* Offset of the segment */
PT2     pt ;
REAL    tol ;
REAL    t0, t1 ;
REAL    *t_ptr ;
REAL    *dist_ptr ;
{
    INT conv ;
    PT2 ept0, ept1 ;
    REAL t ;

    conv = 0 ;
    RETURN ( c2b_ept0 ( b, d, w, ept0 ) && c2b_ept1 ( b, d, w, ept1 ) && 
        c2bs_select ( b, d, w, pt, tol, t0, t1, conv, ept0, ept1, 0, 
        t_ptr==NULL ? &t : t_ptr, dist_ptr ) ) ;
}

/*-------------------------------------------------------------------------*/
STATIC  BOOLEAN c2bs_select ( b, d, w, pt, tol, t0, t1, conv, 
    ept0, ept1, level, t_ptr, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2    *b  ;  /* Control points of the segment */
INT     d ;    /* Order of the segment */
REAL    w ;    /* Offset of the segment */
PT2     pt ;
REAL    tol ;
REAL    t0, t1 ;
INT     conv ;
PT2     ept0, ept1 ;
INT     level ;    /* Level of recursion */
REAL    *t_ptr ;
REAL    *dist_ptr ;
{
    INT convl, convr ;
    C2_CONE_S cone ;
    PT2 ppt ;
    REAL t ;
    HPT2 *bl, *br ;
    BOOLEAN status ;

    c2a_dcone ( b, d, &conv, &cone ) ;

    if ( conv >= -1 && conv <= 1 ) {
        if ( !c2a_parlgrm_w_inside ( ept0, ept1, &cone, pt, tol ) )
            RETURN ( FALSE ) ;

        if ( c2b_project_iter ( b, d, w, pt, t_ptr ) || 
            ( *t_ptr>=-BBS_ZERO && *t_ptr<=1.0+BBS_ZERO ) ) {
            if ( c2b_eval ( b, d, w, *t_ptr, 0, (PT2*)ppt ) ) {
                if ( C2V_DISTL1 ( pt, ppt ) <= tol ) {
                    *t_ptr = t0 + ( t1 - t0 ) * *t_ptr ;
                    if ( dist_ptr != NULL ) 
                        *dist_ptr = C2V_DIST ( pt, ppt ) ;
                    RETURN ( TRUE ) ;
                }
                else if ( level > 3 ) 
                    RETURN ( FALSE ) ;
            }

            if ( *t_ptr > 0.9 ) 
                *t_ptr = 0.9 ;
            if ( *t_ptr < 0.1 ) 
                *t_ptr = 0.1 ;
        }
        else {
            if ( level > 3 ) 
                RETURN ( FALSE ) ;
            *t_ptr = 0.5 ;
        }
    }

    else
        *t_ptr = 0.5 ;

    bl = CREATE ( 2*d-1, HPT2 ) ;
    if ( bl == NULL )
        RETURN ( FALSE ) ;
    br = bl + (d-1) ;
    C2B_BRK0 ( b, d, *t_ptr, bl ) ;
    t = t0 + ( t1 - t0 ) * *t_ptr ;
    c2b_ept1 ( bl, d, w, ppt ) ;
    convl = conv ;
    convr = conv ;
    status = c2bs_select ( bl, d, w, pt, tol, t0, t, convl, 
                ept0, ppt, level+1, t_ptr, dist_ptr ) 
        || c2bs_select ( br, d, w, pt, tol, t, t1, convr, 
                ppt, ept1, level+1, t_ptr, dist_ptr ) ;
    KILL ( bl ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2b_project_iter ( b, d, w, pt, t_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2    *b  ;
INT     d ;
REAL    w ;
PT2     pt ;
REAL    *t_ptr ;
{
    INT count ;
    REAL delta, r, r0, r1 ;
    PT2 v, x[3]  ;

    if ( !c2b_project_init ( b, d, w, pt, t_ptr ) )
        RETURN ( FALSE ) ;
    for ( count=0 ; count<10 ; count++ ) {
        if ( !c2b_eval ( b, d, 0.0, *t_ptr, 2, x ) )
            RETURN ( FALSE ) ;
        C2V_SUB ( x[0], pt, v ) ;
        r0 = C2V_DOT ( x[1], x[1] ) ;
        r1 = C2V_DOT ( v, x[2] ) ;
        r = r0 + r1 ;
        if ( IS_SMALL(r) ) {
            r = r0 ;
            if ( IS_SMALL(r) ) 
                RETURN ( FALSE ) ;
        }
        delta = - C2V_DOT ( v, x[1] ) / r ;
        *t_ptr += delta ;
        if ( *t_ptr > 1.0 ) {
            if ( count>0 && ( *t_ptr > 1.2 ) ) 
                RETURN ( FALSE ) ;
            *t_ptr = 1.0 ;
        }
        if ( *t_ptr < 0.0 ) {
            if ( count>0 && ( *t_ptr < -0.2 ) ) 
                RETURN ( FALSE ) ;
            *t_ptr = 0.0 ;
        }
        if ( IS_ZERO(delta) ) 
            RETURN ( TRUE ) ;
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2b_project_init ( b, d, w, pt, t_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2    *b  ;
INT     d ;
REAL    w ;
PT2     pt ;
REAL    *t_ptr ;
{
    PT2 pt0, pt1 ;
    REAL u, a0, a1 ;

    if ( !c2b_ept0 ( b, d, w, pt0 ) )
        RETURN ( FALSE ) ;
    if ( !c2b_ept1 ( b, d, w, pt1 ) )
        RETURN ( FALSE ) ;
    u = c2v_project_line ( pt0, pt1, pt, NULL ) ;
    a0 = b[0][2] * ( 1.0 - u ) ;
    a1 = b[d-1][2] * u ;
    *t_ptr = a1 / ( a0 + a1 ) ;
    RETURN ( TRUE ) ;
}
#endif /*SPLINE*/

