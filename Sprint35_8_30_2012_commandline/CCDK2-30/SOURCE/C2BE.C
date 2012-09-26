/* -2 -3 */
/********************************** C2BE.C *********************************/
/***************** Two-dimensional Bezier curves evaluations ***************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef  SPLINE
#include <albdefs.h>
#include <c2bdefs.h>
#include <c2edefs.h>
#include <c2vdefs.h>
#include <c2vmcrs.h>
#include <c2hmcrs.h>
#include <c2coned.h>
STATIC void c2b_eval_cubic __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 , 
            PT2 DUMMY3 , REAL DUMMY4 , INT DUMMY5 , PT2* DUMMY6 )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2b_eval ( b, d, w, t, p, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2 *b  ;  /* b[d] array of control points */
INT d ;     /* order */
REAL w ;    /* offset distance */
REAL t ;    /* evaluation parameter */
INT p ;     /* order of the highest derivative */
PT2 *x  ;   /* output array of derivatives */
{
    HPT2 *a ;
    PT2 *a1 ;
    BOOLEAN status ;

    if ( IS_SMALL(w) ) { /* no offset */
        if ( d==4 && IS_SMALL( b[0][2] - 1.0 ) && IS_SMALL( b[1][2] - 1.0 ) 
            && IS_SMALL( b[2][2] - 1.0 ) && IS_SMALL( b[3][2] - 1.0 ) ) {
            c2b_eval_cubic ( b[0], b[1], b[2], b[3], t, p, x ) ;
            status = TRUE ;
        }
        else {
            a = CREATE ( p+1, HPT2 ) ;
            alb_eval ( (REAL*)b, d, 3, t, p, (REAL*)a ) ;
            status = c2e_ratio ( a, p, x ) ;
            KILL ( a ) ;
        }
    }   
    else {  /* offset */
        a = CREATE ( p+2, HPT2 ) ;
        a1 = CREATE ( p+2, PT2 ) ;
        alb_eval ( (REAL*)b, d, 3, t, p+1, (REAL*)a ) ;
        status = c2e_ratio ( a, p+1, a1 ) && c2e_offset ( a1, w, p, x ) ;
        KILL ( a ) ;
        KILL ( a1 ) ;
    }   
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void c2b_eval_cubic ( b0, b1, b2, b3, t, p, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 b0, b1, b2, b3 ;  /* b[d] array of control points */
REAL t ;    /* evaluation parameter */
INT p ;     /* order of the highest derivative */
PT2 *x  ;   /* output array of derivatives */
{
    PT2 c0, c1, c2, a0, a1 ;
    REAL t1 = 1.0 - t ;
    INT i ;

    C2V_ADDW ( b0, t1, b1, t, c0 ) ;
    C2V_ADDW ( b1, t1, b2, t, c1 ) ;
    C2V_ADDW ( b2, t1, b3, t, c2 ) ;

    C2V_ADDW ( c0, t1, c1, t, a0 ) ;
    C2V_ADDW ( c1, t1, c2, t, a1 ) ;

    C2V_ADDW ( a0, t1, a1, t, x[0] ) ;

    if ( p >= 1 ) {
        C2V_SUB ( a1, a0, x[1] ) ;
        C2V_SCALE ( x[1], 3.0, x[1] ) ;
    }

    if ( p >= 2 ) {
        C2V_SUB ( c2, c1, c2 ) ;
        C2V_SUB ( c1, c0, c1 ) ;
        C2V_SUB ( c2, c1, x[2] ) ;
        C2V_SCALE ( x[2], 6.0, x[2] ) ;
    }

    if ( p >= 3 ) {
        x[3][0] = 6.0 * ( b3[0] - b0[0] - 3.0 * ( b2[0] - b1[0] ) ) ;
        x[3][1] = 6.0 * ( b3[1] - b0[1] - 3.0 * ( b2[1] - b1[1] ) ) ;
    }
    for ( i=4 ; i<=p ; i++ ) 
        C2V_SET_ZERO ( x[i] ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2b_eval_pt ( b, d, w, t, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *b  ;  /* b[d] array of control points */
INT d ;     /* order */
REAL w ;    /* offset distance */
REAL t ;    /* evaluation parameter */
PT2 x ;   /* output array of derivatives */
{
    RETURN ( c2b_eval ( b, d, w, t, 0, (PT2*)x ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2b_ept0 ( b, d, w, pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2 *b  ;      /* Control points */
INT d ;         /* Order */
REAL w ;        /* Offset */
PT2 pt ;
{
    REAL p ;
    PT2 x ;

    if ( b[0][2]<=BBS_ZERO ) 
        RETURN ( FALSE ) ;
    pt[0] = b[0][0]/b[0][2] ;
    pt[1] = b[0][1]/b[0][2] ;
    if ( IS_ZERO(w) ) 
        RETURN ( TRUE ) ;
    if ( !c2b_etan0 ( b, d, x ) ) 
        RETURN ( FALSE ) ;
    p = w / C2V_NORM ( x ) ;
    C2V_OFFSET ( pt, x, p, pt ) ;
    RETURN ( TRUE ) ;
}   


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN     c2b_etan0 ( b, d, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2 *b  ;      /* Control points */
INT d ;         /* Order */
PT2 tan_vec ;
{
    INT i ;

    for ( i=1 ; i<d ; i++ ) { 
        C2H_DIFF ( b[i], b[0], tan_vec ) ; 
        if ( !C2V_IS_SMALL(tan_vec) ) 
            RETURN ( TRUE ) ; 
    } 
    RETURN ( FALSE ) ;
} 


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN     c2b_unit_normal0 ( b, d, unit_normal ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2 *b  ;      /* Control points */
INT d ;         /* Order */
PT2 unit_normal ;
{
    PT2 tan_vec ;
    INT i ;

    for ( i=1 ; i<d ; i++ ) { 
        C2H_DIFF ( b[i], b[0], tan_vec ) ; 
        if ( !C2V_IS_SMALL(tan_vec) ) { 
            if ( !c2v_normalize ( tan_vec, tan_vec ) ) 
                RETURN ( FALSE ) ;
            C2V_NORMAL ( tan_vec, unit_normal ) ;
            RETURN ( TRUE ) ;
        }
    } 
    C2V_SET_ZERO ( unit_normal ) ;
    RETURN ( FALSE ) ;
} 


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN     c2b_ept_tan0 ( b, d, w, pt, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2 *b  ;      /* Control points */
INT d ;         /* Order */
REAL w ;        /* Offset */
PT2 pt ;
PT2 tan_vec ;
{
    REAL p ;

    if ( !c2b_etan0 ( b, d, tan_vec ) ) 
        RETURN ( FALSE ) ;
    if ( b[0][2]<=BBS_ZERO ) 
        RETURN ( FALSE ) ;
    pt[0] = b[0][0]/b[0][2] ;
    pt[1] = b[0][1]/b[0][2] ;
    if ( IS_SMALL(w) ) 
        RETURN ( TRUE ) ;
    p = w / C2V_NORM ( tan_vec ) ;
    C2V_OFFSET ( pt, tan_vec, p, pt ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN     c2b_ept1 ( b, d, w, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2 *b  ;      /* Control points */
INT d ;         /* Order */
REAL w ;        /* Offset */
PT2 pt ;
{
    REAL p ;
    PT2 x ;

    if ( b[d-1][2]<=BBS_ZERO ) 
        RETURN ( FALSE ) ;
    pt[0] = b[d-1][0]/b[d-1][2] ;
    pt[1] = b[d-1][1]/b[d-1][2] ;
    if ( IS_SMALL(w) ) 
        RETURN ( TRUE ) ;
    if ( !c2b_etan1 ( b, d, x ) ) 
        RETURN ( FALSE ) ;
    p = w / C2V_NORM ( x ) ;
    C2V_OFFSET ( pt, x, p, pt ) ;
    RETURN ( TRUE ) ;
}   


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN     c2b_etan1 ( b, d, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2 *b  ;      /* Control points */
INT d ;         /* Order */
PT2 tan_vec ;
{
    INT i ;

    for ( i=d-2 ; i>=0 ; i-- ) { 
        C2H_DIFF ( b[d-1], b[i], tan_vec ) ; 
        if ( !C2V_IS_SMALL(tan_vec) ) 
            RETURN ( TRUE ) ; 
    } 
    RETURN ( FALSE ) ;
} 


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN     c2b_unit_normal1 ( b, d, unit_normal ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2 *b  ;      /* Control points */
INT d ;         /* Order */
PT2 unit_normal ;
{
    PT2 tan_vec ;
    INT i ;

    for ( i=d-2 ; i>=0 ; i-- ) { 
        C2H_DIFF ( b[d-1], b[i], tan_vec ) ; 
        if ( !C2V_IS_SMALL(tan_vec) ) { 
            if ( !c2v_normalize ( tan_vec, tan_vec ) ) 
                RETURN ( FALSE ) ;
            C2V_NORMAL ( tan_vec, unit_normal ) ;
            RETURN ( TRUE ) ;
        }
    } 
    C2V_SET_ZERO ( unit_normal ) ;
    RETURN ( FALSE ) ;
} 


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN     c2b_ept_tan1 ( b, d, w, pt, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2 *b  ;      /* Control points */
INT d ;         /* Order */
REAL w ;        /* Offset */
PT2 pt ;
PT2 tan_vec ;
{
    REAL p ;

    if ( !c2b_etan1 ( b, d, tan_vec ) ) 
        RETURN ( FALSE ) ;
    if ( b[d-1][2]<=BBS_ZERO ) 
        RETURN ( FALSE ) ;
    pt[0] = b[d-1][0]/b[d-1][2] ;
    pt[1] = b[d-1][1]/b[d-1][2] ;
    if ( IS_SMALL(w) ) 
        RETURN ( TRUE ) ;
    p = w / C2V_NORM ( tan_vec ) ;
    C2V_OFFSET ( pt, tan_vec, p, pt ) ;
    RETURN ( TRUE ) ;
}
#endif  /*SPLINE*/

