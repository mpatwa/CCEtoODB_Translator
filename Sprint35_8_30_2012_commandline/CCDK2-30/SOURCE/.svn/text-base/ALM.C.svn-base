/* -2 -3 */
/********************************** ALM.C **********************************/
/********************** Processing matrices ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <aladefs.h>
#include <almdefs.h>

STATIC void alm_eigen_sort __(( INT, REAL*, REAL* )) ;
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN alm_min_quad_form ( mtx, m, n, min_vector, min_value ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *mtx   ;   /* mtx[n][m] */ 
INT     m, n ;
REAL    *min_vector   ; /* min_vector[m] */ 
REAL    *min_value   ; 
{ 
    REAL *eigen_vector, *eigen_value ;

    eigen_vector = CREATE ( n*n, REAL ) ;
    eigen_value = CREATE ( n, REAL ) ;
    alm_eigen ( mtx, m, n, eigen_vector, eigen_value ) ;
    if ( min_vector != NULL ) 
        ala_copy ( eigen_vector, n, min_vector ) ;
    if ( min_value != NULL )
        *min_value = eigen_value[0] ;

    KILL ( eigen_vector ) ;
    KILL ( eigen_value ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void alm_eigen ( mtx, m, n, eigen_vector, eigen_value ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *mtx   ;   /* mtx[m][n] */ 
INT     m, n   ;
REAL    *eigen_vector   ; /* eigen_vector[n][n] */ 
REAL    *eigen_value   ; /* eigen_value[n] */ 
{ 
    REAL *mtx1 ;

    mtx1 = CREATE ( n*n, REAL ) ;
    alm_mult_self_conj ( mtx, m, n, mtx1 ) ;
    alm_eigen_sq_mtx ( mtx1, n, eigen_vector, eigen_value ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void alm_mult_self_conj ( mtx0, m, n, mtx ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *mtx0 ;   /* mtx0[m][n] */ 
INT     m, n ;   
REAL    *mtx ; /* mtx[n][n] */
{ 
    INT i, j, k ;

    for ( i = 0 ; i < n ; i++ ) {
        for ( j = 0 ; j < n ; j++ ) {
            mtx[i*n+j] = 0.0 ;
            for ( k = 0 ; k < m ; k++ ) 
                mtx[i*n+j] += mtx0[k*n+i] * mtx0[k*n+j] ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void alm_mult ( mtx0, mtx1, n0, n1, n2, mtx ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *mtx0, *mtx1   ;   /* mtx0[n0][n1], mtx1[n1][n2] */ 
INT     n0, n1, n2     ;   
REAL    *mtx ; /* mtx[n0][n2] */
{ 
    INT i, j, k ;

    for ( i = 0 ; i < n0 ; i++ ) {
        for ( j = 0 ; j < n0 ; j++ ) {
            mtx[i*n2+j] = 0.0 ;
            for ( k = 0 ; k < n1 ; k++ ) 
                mtx[i*n2+j] += mtx0[i*n1+k] * mtx1[k*n2+j] ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void alm_eigen_sq_mtx ( mtx, n, eigen_vector, eigen_value ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *mtx   ;   /* mtx[n][n] */ 
INT     n       ;  
REAL    *eigen_vector   ; /* eigen_vector[n][n] */ 
REAL    *eigen_value   ; /* eigen_value[n] */ 
{ 
    INT i, j=0, k, l, count, max_count ;
    REAL a, b, c, d, s, mtx_max, tol, *vec1, *vec2 ;

    vec1 = CREATE ( n, REAL ) ;
    vec2 = CREATE ( n, REAL ) ;

    mtx_max = 0.0 ;

    for ( i = 0 ; i < n ; i++ ) {
        for ( j = 0 ; j < n ; j++ ) {
            if ( fabs(mtx[i*n+j]) > mtx_max )
                mtx_max = fabs(mtx[i*n+j]) ;
            eigen_vector[i*n+j] = ( i == j ) ? 1.0 : 0.0 ;
        }
    }

    tol = BBS_ZERO * mtx_max ;
    s = 1.0 ;
    max_count = MAX_COUNT * n ;

    for ( count = 0 ; count < max_count && fabs(s) > tol ; count++ ) {
        mtx_max = 0.0 ;
        for ( k = 0 ; k < n ; k++ ) {
            for ( l = k+1 ; l < n ; l++ ) {
                if ( fabs(mtx[k*n+l]) > mtx_max ) {
                    mtx_max = fabs(mtx[k*n+l]) ;
                    i = k ;
                    j = l ;
                }
            }
        }

        if ( mtx_max < tol )
            s = 1.0 ;
        else {
            b = mtx[i*(n+1)] - mtx[j*(n+1)] ;
            d = mtx[i*n+j] ;
            a = fabs(b) / ( 2.0 * sqrt ( 4.0 * d * d + b * b ) ) ;
            c = sqrt ( 0.5 + a ) - 1.0 ;
            s = sqrt ( 0.5 - a ) ;

            if ( ( b < 0.0 ) == d > 0.0 )
                s = -s ;
            for ( k = 0 ; k < n ; k++ ) {
                vec1[k] = mtx[k*n+i] * c + mtx[k*n+j] * s ;
                vec2[k] = mtx[k*n+j] * c - mtx[k*n+i] * s ;
            }
            for ( k = 0 ; k < n ; k++ ) {
                mtx[k*n+i] += vec1[k] ;
                mtx[k*n+j] += vec2[k] ;
            }

            for ( k = 0 ; k < n ; k++ ) {
                vec1[k] = mtx[i*n+k] * c + mtx[j*n+k] * s ;
                vec2[k] = mtx[j*n+k] * c - mtx[i*n+k] * s ;
            }
            for ( k = 0 ; k < n ; k++ ) {
                mtx[i*n+k] += vec1[k] ;
                mtx[j*n+k] += vec2[k] ;
            }

            for ( k = 0 ; k < n ; k++ ) {
                vec1[k] = eigen_vector[i*n+k] * c + eigen_vector[j*n+k] * s ;
                vec2[k] = eigen_vector[j*n+k] * c - eigen_vector[i*n+k] * s ;
            }
            for ( k = 0 ; k < n ; k++ ) {
                eigen_vector[i*n+k] += vec1[k] ;
                eigen_vector[j*n+k] += vec2[k] ;
            }
        }
    }

    for ( i = 0 ; i < n ; i++ ) 
        eigen_value[i] = mtx[i*n+i] ;
    KILL ( vec1 ) ;
    KILL ( vec2 ) ;
    alm_eigen_sort ( n, eigen_vector, eigen_value ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void alm_eigen_sort ( n, eigen_vector, eigen_value ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT     n       ;
REAL    *eigen_vector   ; /* eigen_vector[n][n] */ 
REAL    *eigen_value   ; /* eigen_value[n] */ 
{ 
    INT i, k ;
    BOOLEAN sorted = FALSE ;
    REAL r ;

    for ( k = 0 ; k < n && !sorted ; k++ ) {
        sorted = TRUE ;
        for ( i = 1 ; i < n ; i++ ) {
            if ( eigen_value[i] < eigen_value[i-1] ) {
                r = eigen_value[i] ;
                eigen_value[i] = eigen_value[i-1] ;
                eigen_value[i-1] = r ;
                ala_swap ( eigen_vector+i*n, eigen_vector+(i-1)*n, n ) ;
            }
            sorted = FALSE ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void alm_conj ( mtx, m, n ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *mtx ; /* mtx[m][n] */ 
INT     m, n ;   
{ 
    INT i, j ;
    REAL r ;
/*
    mtx[i][j] <--> mtx[j][i] 
*/
    for ( i = 0 ; i < m ; i++ ) {
        for ( j = 0 ; j < n ; j++ ) {
            r = mtx[i*n+j] ;
            mtx[i*n+j] = mtx[j*n+i] ;
            mtx[j*n+i] = r ;
        }
    }
}

