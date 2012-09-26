/* -2 -3 */
/********************************** C2S.C **********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <bbsdefs.h>
#include <alsdefs.h>
#include <c2sdefs.h>
#include <c2edefs.h>
#include <c2vmcrs.h>
#include <c2hmcrs.h>

STATIC HPT2* c2s_clsd_ctlpts_4 __(( HPT2* DUMMY0 , INT DUMMY1 , 
            REAL* DUMMY2 )) ;
STATIC HPT2* c2s_clsd_ctlpts_3 __(( HPT2* DUMMY0 , INT DUMMY1 ,
            REAL* DUMMY2 )) ;
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2s_eval ( c, n, d, knot, w, parm, p, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *c  ;
INT n ;
INT d ;
REAL *knot  ;
REAL w ;
PARM parm ;
INT p ;
PT2 *x  ;
{
    HPT2 *a ;
    PT2 *a1 ;
    BOOLEAN status ;

    if ( !als_parm_adjust ( n, d, knot, parm ) )
        RETURN ( FALSE ) ;
    if ( IS_ZERO(w) ) { /* no offset */
        a = CREATE ( p+1, HPT2 ) ;
        als_eval ( (REAL*)(c+PARM_J(parm)-d+1), d,
            knot+PARM_J(parm)-d+1, 3, PARM_T(parm), p, (REAL*)a ) ;
        status = c2e_ratio ( a, p, x ) ;
        KILL ( a ) ;
    }
    else {  /* offset */
        a = CREATE ( p+2, HPT2 ) ;
        a1 = CREATE ( p+2, PT2 ) ;
        als_eval ( (REAL*)(c+PARM_J(parm)-d+1), d,
            knot+PARM_J(parm)-d+1, 3, PARM_T(parm), p+1, (REAL*)a ) ;
        status = c2e_ratio ( a, p+1, a1 ) && c2e_offset ( a1, w, p, x ) ;
        KILL ( a ) ;
        KILL ( a1 ) ;
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2s_pt_tan ( c, n, d, knot, w, parm, pt, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *c  ;
INT n ;
INT d ;
REAL *knot  ;
REAL w ;
PARM parm ;
PT2 pt, tan_vec ;
{
    HPT2 x1, x[2] ;
    REAL r ;

    if ( !als_parm_adjust ( n, d, knot, parm ) )
        RETURN ( FALSE ) ;
    if ( tan_vec == NULL && IS_SMALL(w) ) 
        als_eval ( (REAL*)(c+PARM_J(parm)-d+1), d, 
            knot+PARM_J(parm)-d+1, 3, PARM_T(parm), 0, (REAL*)x ) ;
    else 
        als_eval ( (REAL*)(c+PARM_J(parm)-d+1), d, 
            knot+PARM_J(parm)-d+1, 3, PARM_T(parm), 1, (REAL*)x ) ;
    if ( tan_vec != NULL ) 
        C2H_DIFF ( x[1], x[0], x1 ) ;
    if ( pt != NULL ) {
        if ( HPT2_INF(x[0]) ) 
            RETURN ( FALSE ) ;
        pt[0] = x[0][0] / x[0][2] ;
        pt[1] = x[0][1] / x[0][2] ;

        if ( !IS_SMALL(w) ) {
            r = C2V_NORM ( x1 ) ;
            if ( IS_SMALL(r) ) 
                RETURN ( FALSE ) ;
            r = w / r ;
            C2V_OFFSET ( pt, x1, r, pt ) ;
        }
    }
    if ( tan_vec != NULL ) 
        C2V_COPY ( x1, tan_vec ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2s_coinc ( c1, n1, d1, knot1, w1, c2, n2, d2, knot2, w2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *c1, *c2 ;
INT n1, n2 ;
INT d1, d2 ;
REAL *knot1, *knot2  ;
REAL w1, w2 ;
{
    INT i ;

    if ( n1 != n2 || d1 != d2 )
        RETURN ( FALSE ) ;
    if ( !IS_SMALL ( w1 - w2 ) )
        RETURN ( FALSE ) ;

    for ( i = 0 ; i < n1+d1 ; i++ ) {
        if ( !IS_SMALL ( knot1[i] - knot2[i] ) )
            RETURN ( FALSE ) ;
    }

    for ( i = 0 ; i < n1 ; i++ ) {
        if ( !IS_SMALL ( c1[i][0] - c2[i][0] ) )
            RETURN ( FALSE ) ;
        if ( !IS_SMALL ( c1[i][1] - c2[i][1] ) )
            RETURN ( FALSE ) ;
        if ( !IS_SMALL ( c1[i][2] - c2[i][2] ) )
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE HPT2* c2s_clsd_ctlpts ( a, n, d, knot ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
INT n ;
INT d ;
REAL *knot ;
{
    if ( d == 4 )
        RETURN ( c2s_clsd_ctlpts_4 ( a, n, knot ) ) ;
    else if ( d == 3 )
        RETURN ( c2s_clsd_ctlpts_3 ( a, n, knot ) ) ;
    else
        RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
STATIC HPT2* c2s_clsd_ctlpts_4 ( a, n, knot ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
INT n ;
REAL *knot ;
{
    HPT2 *c, b[4] ;
    INT j ;

    c = MALLOC ( n-3, HPT2 ) ;
    if ( c == NULL ) 
        RETURN ( NULL ) ;

    for ( j = 3 ; j < n ; j++ )
    {
        C2S_CONV_BEZ ( a, 4, knot, j, b ) ;
        c[j-3][0] = 6.0 * b[0][0] - 7.0 * b[1][0] + 2.0 * b[2][0] ;
        c[j-3][1] = 6.0 * b[0][1] - 7.0 * b[1][1] + 2.0 * b[2][1] ;
        c[j-3][2] = 1.0 ;
    }
    RETURN ( c ) ;
}


/*-------------------------------------------------------------------------*/
STATIC HPT2* c2s_clsd_ctlpts_3 ( a, n, knot ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
INT n ;
REAL *knot ;
{
    HPT2 *c, b[3] ;
    INT j ;

    c = MALLOC ( n-2, HPT2 ) ;
    if ( c == NULL ) 
        RETURN ( NULL ) ;

    for ( j = 2 ; j < n ; j++ )
    {
        C2S_CONV_BEZ ( a, 3, knot, j, b ) ;
        c[j-2][0] = 2.0 * b[0][0] - b[1][0] ;
        c[j-2][1] = 2.0 * b[0][1] - b[1][1] ;
        c[j-2][2] = 1.0 ;
    }
    RETURN ( c ) ;
}
#endif  /*SPLINE*/

