/* -2 -3 */
/*********************************** C2E.C *********************************/
/*********************** 2-d curve evaluation routines *********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <aladefs.h>
#include <c2edefs.h>
#include <c2vmcrs.h>
#include <c2hmcrs.h>
STATIC BOOLEAN c2e_norm_deriv __(( PT2*, REAL*, INT, REAL* )) ;
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2e_ratio ( a, p, b )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *a  ;   /* a[p+1] - Array of hom. coordinates of curve and its 
                    derivatives of order ó p */
INT     p ;     /* Order of the derivatives */
PT2     *b  ;   /* b[p+1] - Array of eucl. coordinates of curve and its 
                    derivatives of order ó p */
{
    INT i, j, q=p+1 ;
    REAL *c, w, *c_ptr ;

    if ( IS_ZERO( a[0][2] ) )
        RETURN ( FALSE ) ;

    if ( p==0 ) {
        b[0][0] = a[0][0] / a[0][2] ;
        b[0][1] = a[0][1] / a[0][2] ;
        RETURN ( TRUE ) ;
    }
    else if ( p==1 ) {
        b[0][0] = a[0][0] / a[0][2] ;
        b[0][1] = a[0][1] / a[0][2] ;
        b[1][0] = ( a[1][0] - b[0][0] * a[1][2] ) / a[0][2] ;
        b[1][1] = ( a[1][1] - b[0][1] * a[1][2] ) / a[0][2] ;
        RETURN ( TRUE ) ;
    }

    c = CREATE ( q*q, REAL ) ;
    ala_binom ( q, c ) ;
    for ( i=0, c_ptr=c ; i<=p ; i++, c_ptr+=q ) {
        b[i][0] = a[i][0] ;
        b[i][1] = a[i][1] ;
        for ( j=0 ; j<i ; j++ ) {
            w = c_ptr[j] * a[i-j][2] ;
            b[i][0] -= w * b[j][0] ;
            b[i][1] -= w * b[j][1] ;
        }
        b[i][0] /= (a[0][2]) ;
        b[i][1] /= (a[0][2]) ;
    }
    KILL ( c ) ;
    RETURN ( TRUE ) ;
}        

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2e_polynomial2 ( b, t, p, f ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 b[3], *f ;
REAL t ;
INT p ; /* p>= 2 */
{
    INT i, j ;
    for ( j=0 ; j<3 ; j++ ) {
        f[0][j] = ( b[2][j] * t + b[1][j] ) * t + b[0][j] ;
        f[1][j] = 2.0 * b[2][j] * t + b[1][j] ;
        f[2][j] = 2.0 * b[2][j] ;

        for ( i=3 ; i<=p ; i++ )
            f[i][j] = 0.0 ;
    }
}

        
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2e_offset ( x, w, p, y )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2    *x  ;    /* x[p+2] - Array of eucl. coordinates of curve and its 
                    derivatives of order ó p+1 */
REAL    w ;     /* Offset distance */
INT     p ;     /* Order of the derivatives */
PT2     *y  ;   /* y[p+1] - Array of eucl. coordinates of offset curve 
                    and its derivatives of order ó p */
{
/* y = x + w * normal */
    INT i ;
    if ( !c2e_normal ( x, p, y ) ) 
        RETURN ( FALSE ) ;
    for ( i=0 ; i <= p ; i++ ) {
        C2V_ADDT ( x[i], y[i], w, y[i] ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2e_normal ( a, p, b )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2    *a  ;    /* a[p+2] - Array of eucl. coordinates of curve and its 
                    derivatives of order ó p+1 */
INT     p ;     /* Order of the derivatives */
PT2     *b  ;   /* b[p+1] - Array of eucl. coordinates of offset curve 
                    and its derivatives of order ó p */
{
    INT i, k, q=p+1 ;
    REAL r, u, *z, *c ;
    
    z = CREATE ( q, REAL ) ;
    c = CREATE ( q*q, REAL ) ;
    ala_binom ( q, c ) ;

    if ( !c2e_norm_deriv ( a+1, c, p, z ) ) {
        KILL ( z ) ;
        KILL ( c ) ;
        RETURN ( FALSE ) ;
    }

    for ( k=0 ; k<=p ; k++ ) {
        C2V_SET_ZERO ( b[k] ) ;
        C2V_OFFSET ( b[k], a[k+1], 1.0, b[k] ) ;
        for ( i=0 ; i<k ; i++ ) {
            u = c[k*q+i] * z[k-i] ;
            b[k][0] -= ( u * b[i][0] ) ;
            b[k][1] -= ( u * b[i][1] ) ;
        }
        r = 1.0 / C2V_NORM ( a[1] ) ;
        b[k][0] *= r ;
        b[k][1] *= r ;
    }

    KILL ( z ) ;
    KILL ( c ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c2e_norm_deriv ( x, c, p, z )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2    *x  ;    /* x[p+1] - Array of eucl. coordinates of curve and its 
                    derivatives of order ó p */
REAL    *c  ;   
INT     p ;     /* Order of the derivatives */
REAL    *z  ;   /* z[p+1] - Array of norm(a) and 
                    its derivatives of order ó p */
{
    INT i, k, m, q=p+1 ;
    REAL r ;
    BOOLEAN even_k ;
    
    r = C2V_NORM ( x[0] ) ;
    if ( IS_ZERO(r) )       /* The first derivative is zero */
         RETURN ( FALSE ) ;
    
    z[0] = r ;   /* norm(a') */
    r = 1.0 / r ;

    for ( k=1, m=1, even_k = FALSE ; k<=p ; k++ ) {
        z[k] = C2V_DOT ( x[0], x[k] ) ;
        for (i=1 ; i<m ; i++ )
            z[k] += ( c[k*q+i] * ( C2V_DOT ( x[i], x[k-i] ) - z[i]*z[k-i] ) ) ;
    
        if ( even_k ) {
            z[k] += ( 0.5 * c[k*q+m] * 
                ( C2V_DOT ( x[m], x[m] ) - z[m] * z[m] ) ) ;
            m++ ;
            even_k = FALSE ;
        }
        else 
            even_k = TRUE ;
        z[k] *= r ;
    }
    RETURN ( TRUE ) ;
}


#ifdef  SPLINE
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2e_curvature ( a, curv_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 a[3] ;  /* homogeneous representations of point, */
             /* first & second derivatives */
REAL *curv_ptr ;
{

    PT2 x1, x2 ;
    REAL n, cross ;

    C2H_DIFF ( a[1], a[0], x1 ) ;
    C2H_DIFF ( a[2], a[0], x2 ) ;
    n = C2V_NORM ( x1 ) ;
    if ( n <= BBS_ZERO ) 
        RETURN ( FALSE ) ;
    cross = C2V_CROSS ( x1, x2 ) ;
    *curv_ptr = ( a[0][2] * a[0][2] * cross ) / ( n*n*n ) ;
    RETURN ( TRUE ) ;
}

#endif  /*SPLINE*/

