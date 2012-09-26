/* -2 -3 */
/*********************************** C2BM.C ********************************/
/*********************** Two-dimensional Bezier curves *********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef  SPLINE
#include <aladefs.h>
#include <c2bdefs.h>
#include <bbsgauss.h>
#include <c2vmcrs.h>
#include <c2coned.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2b_mass_prop ( b, d, w, t0, t1, p, dim, tol, 
            prop_function, result ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2    *b  ;  /* Control points of the segment */
INT     d ;    /* Order of the segment */
REAL    w ;    /* Offset of the segment */
REAL    t0, t1 ;    /* Parameters */
INT     p ;
INT     dim ;
REAL    tol ;
PF_PROP2 prop_function ;
REAL    *result ;    /* result[dim] */
{

    INT i, m, k, j ;
    BOOLEAN success ;
    REAL h, u, t, *result_new, *result_temp ;
    PT2 *x ;
    HPT2 *bl, *br ;

    success = FALSE ;
    result_new = CREATE ( dim, REAL ) ;
    result_temp = CREATE ( dim, REAL ) ;
    x = CREATE ( p+1, PT2 ) ;
    h = t1 - t0 ;

    for ( m=0, k=0 ; m<8 && !success ; m++ ) {

        ala_set_zero ( result_new, dim ) ;

        for ( i=0 ; i<=m ; i++, k++ ) {

            t = 0.5 * ( 1.0 + NODE[k] ) ;
            if ( c2b_eval ( b, d, w, t, p, x ) ) {
                u = 1.0 ;
                for ( j=1 ; j<=p ; j++ ) {
                    u /= h ;
                    C2V_SCALE ( x[j], u, x[j] ) ;
                }
                (*prop_function) ( x, result_temp ) ;
                ala_addt ( result_new, result_temp, WEIGHT[k], 
                    dim, result_new ) ;
            }        

            t = 0.5 * ( 1.0 - NODE[k] ) ;
            if ( c2b_eval ( b, d, w, t, p, x ) ) {
                (*prop_function) ( x, result_temp ) ;
                u = 1.0 ;
                for ( j=1 ; j<=p ; j++ ) {
                    u /= h ;
                    C2V_SCALE ( x[j], u, x[j] ) ;
                }
                (*prop_function) ( x, result_temp ) ;
                ala_addt ( result_new, result_temp, WEIGHT[k], 
                    dim, result_new ) ;
            }        
        } 

        success = m && ala_diff_small ( result_new, result, dim, tol ) ;
        ala_copy ( result_new, dim, result ) ;
    }

    if ( !success ) {
        t = 0.5 * ( t0 + t1 ) ;
        bl = CREATE ( 2*d-1, HPT2 ) ;
        br = bl + (d-1) ;
        C2B_SBDV0 ( b, d, bl ) ;
        c2b_mass_prop ( bl, d, w, t0, t, p, dim, tol, prop_function, result ) ;
        c2b_mass_prop ( br, d, w, t, t1, p, dim, tol, prop_function, 
            result_new ) ;
        ala_add ( result, result_new, dim, result ) ;
        KILL ( bl ) ;
    }
    else 
        ala_scale ( result, 0.5*h, dim, result ) ;
    KILL ( result_new ) ;
    KILL ( result_temp ) ;
    KILL ( x ) ;
}
#endif  /*SPLINE*/

