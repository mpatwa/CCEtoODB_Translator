/* -2 -3 */
/*********************************** C2BC.C ********************************/
/*************** Intersection of two-dimensional Bezier curves *************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef  SPLINE
#include <c2bdefs.h>
#include <c2coned.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2b_inters_iter ( b1, d1, w1, ept01, ept11, parm_tol1, 
    b2, d2, w2, ept02, ept12, parm_tol2, t1_ptr, t2_ptr, dist_ptr, intr_pt )  
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2    *b1 ;  /* Control points of the first segment */
INT     d1 ;    /* Order of the first segment */
REAL    w1 ;    /* Offset of the first segment */
PT2     ept01, ept11 ;
REAL    parm_tol1 ;    /* Parametric tolerance for the first segment */
HPT2    *b2 ;  /* Control points of the second segment */
INT     d2 ;    /* Order of the second segment */
REAL    w2 ;    /* Offset of the second segment */
PT2     ept02, ept12 ;
REAL    parm_tol2 ;    /* Parametric tolerance for the second segment */
REAL *t1_ptr ;   /* Parameter of the intersection point */
REAL *t2_ptr ;   /* Parameter of the intersection point */
REAL *dist_ptr ;
PT2  intr_pt ;  /* Intersection point */
{
    REAL cross, t1_init, t2_init ;
    PT2 v, v1, v2 ;

    C2V_SUB ( ept02, ept01, v ) ;
    C2V_SUB ( ept11, ept01, v1 ) ;
    C2V_SUB ( ept12, ept02, v2 ) ;
    cross = C2V_CROSS ( v1, v2 ) ;
    t1_init = C2V_CROSS ( v, v2 ) / cross ;
    t2_init = C2V_CROSS ( v, v1 ) / cross ;
    
    if ( t1_init < -0.1 || t1_init > 1.1 ) 
        RETURN ( FALSE ) ;
    if ( t2_init < -0.1 || t2_init > 1.1 ) 
        RETURN ( FALSE ) ;
    if ( c2b_inters_init ( b1, d1, w1, t1_init, parm_tol1, 
            b2, d2, w2, t2_init, parm_tol2, t1_ptr, t2_ptr ) ) {
        c2b_eval_pt ( b1, d1, w1, *t1_ptr, v1 ) ;
        c2b_eval_pt ( b2, d2, w2, *t2_ptr, v2 ) ;
        *dist_ptr = C2V_DISTL1 ( v1, v2 ) ;
        if ( *dist_ptr <= BBS_TOL ) {
            C2V_MID_PT ( v1, v2, intr_pt ) ;
            RETURN ( TRUE ) ;
        }
        else 
            RETURN ( FALSE ) ;
    }
    else 
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2b_inters_init ( b1, d1, w1, t1_init, parm_tol1, 
        b2, d2, w2, t2_init, parm_tol2, t1_ptr, t2_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2    *b1 ;  /* Control points of the first segment */
INT     d1 ;    /* Order of the first segment */
REAL    w1 ;    /* Offset of the first segment */
REAL    t1_init;/* Initial parameter along the first segment */
REAL    parm_tol1 ;
HPT2    *b2 ;  /* Control points of the second segment */
INT     d2 ;    /* Order of the second segment */
REAL    w2 ;    /* Offset of the second segment */
REAL    t2_init;/* Initial parameter along the second segment */
REAL    parm_tol2 ;
REAL *t1_ptr;   /* Parameter of the intersection point */
REAL *t2_ptr;   /* Parameter of the intersection point */
{
    INT count ;
    PT2 x1[2], x2[2], x ;
    REAL u1, u2, v1, v2, cross ;
    BOOLEAN status ;

    *t1_ptr = t1_init ;
    *t2_ptr = t2_init ;

    for ( count=0 ; count <= MAX_COUNT ; count++ ) {
        c2b_eval ( b1, d1, w1, *t1_ptr, 1, x1 ) ;
        c2b_eval ( b2, d2, w2, *t2_ptr, 1, x2 ) ;
        cross = C2V_CROSS ( x1[1], x2[1] ) ;
        v1 = C2V_NORML1(x1[1]) ;
        v2 = C2V_NORML1(x2[1]) ;
        C2V_SUB ( x2[0], x1[0], x ) ;
        if ( IS_SMALL(v1) || IS_SMALL(v2) || 
            ( fabs(cross) <= BBS_ZERO * v1 * v2 ) )
            RETURN ( FALSE ) ;
        u1 = (*t1_ptr) + C2V_CROSS ( x, x2[1] ) / cross ;
        u2 = (*t2_ptr) + C2V_CROSS ( x, x1[1] ) / cross ;
        if ( u1 < 0.0 ) {
            u1 = 0.5 * (*t1_ptr) ;
            if ( count >= 2 ) 
                RETURN ( FALSE ) ;
        }
        else if ( u1 > 1.0 ) {
            u1 = 0.5 * ( (*t1_ptr) + 1.0 ) ;
            if ( count >= 2 ) 
                RETURN ( FALSE ) ;
        }
        if ( u2 < 0.0 ) {
            u2 = 0.5 * (*t2_ptr) ;
            if ( count >= 2 ) 
                RETURN ( FALSE ) ;
        }
        else if ( u2 > 1.0 ) {
            u2 = 0.5 * ( (*t2_ptr) + 1.0 ) ;
            if ( count >= 2 ) 
                RETURN ( FALSE ) ;
        }
        
        status = ( fabs ( u1 - (*t1_ptr) ) < parm_tol1 && 
                   fabs ( u2 - (*t2_ptr) ) < parm_tol2 ) ;
        *t1_ptr = u1 ;
        *t2_ptr = u2 ;
        if ( status ) 
            RETURN ( TRUE ) ;
    }
    RETURN ( FALSE ) ;
}    

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2b_int_coord_line_init ( b, d, w, j, x, t_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2    *b  ;
INT     d ;
REAL    w ;
INT     j ;
REAL    x ;
REAL    *t_ptr ;
{
    PT2 pt0, pt1 ;

    if ( !c2b_ept0 ( b, d, w, pt0 ) )
        RETURN ( FALSE ) ;
    if ( !c2b_ept1 ( b, d, w, pt1 ) )
        RETURN ( FALSE ) ;
    *t_ptr = ( x - pt0[j] ) / ( pt1[j] - pt0[j] ) ;
    if ( *t_ptr < 0.1 ) 
        *t_ptr = 0.1 ;
    else if ( *t_ptr > 0.9 ) 
        *t_ptr = 0.9 ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2b_int_coord_line_iter ( b, d, w, j, x, t_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2    *b  ;
INT     d ;
REAL    w ;
INT     j ;
REAL    x ;
REAL    *t_ptr ;
{
    INT count ;
    REAL delta, t0 ;
    PT2 v[2]  ;

    if ( !c2b_int_coord_line_init ( b, d, w, j, x, t_ptr ) ) {
        *t_ptr = 0.5 ;
        RETURN ( FALSE ) ;
    }
    t0 = *t_ptr ;
    for ( count=0 ; count<10 ; count++ ) {
        if ( !c2b_eval ( b, d, w, *t_ptr, 1, v ) ) {
            *t_ptr = t0 ;
            RETURN ( FALSE ) ;
        }
        if ( IS_ZERO(v[1][j]) ) {
            *t_ptr = t0 ;
            RETURN ( FALSE ) ;
        }

        delta = - ( v[0][j] - x ) / v[1][j] ;
        *t_ptr += delta ;
        if ( *t_ptr > 1.0 ) {
            if ( count>0 && ( *t_ptr > 1.2 ) ) {
                *t_ptr = t0 ;
                RETURN ( FALSE ) ;
            }
            *t_ptr = 1.0 ;
        }
        if ( *t_ptr < 0.0 ) {
            if ( count>0 && ( *t_ptr < -0.2 ) ) {
                *t_ptr = t0 ;
                RETURN ( FALSE ) ;
            }
            *t_ptr = 0.0 ;
        }
        if ( IS_ZERO(delta) ) 
            RETURN ( TRUE ) ;
    }
    *t_ptr = t0 ;
    RETURN ( FALSE ) ;
}
#endif  /*SPLINE*/ 

