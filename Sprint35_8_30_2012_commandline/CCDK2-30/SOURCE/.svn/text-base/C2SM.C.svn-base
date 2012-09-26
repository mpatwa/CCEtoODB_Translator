/* -2 -3 */
/********************************** C2SM.C *********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <bbsdefs.h>
#ifdef  SPLINE
#include <aladefs.h>
#include <c2bdefs.h>
#include <c2sdefs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2s_mass_prop ( a, d, knot, w, parm0, parm1, p, dim, tol, 
        prop_function, result ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *a  ;
INT     d ;
REAL    *knot  ;
REAL    w ;
PARM    parm0, parm1 ;
INT     p ;
INT     dim ;
REAL    tol ;
PF_PROP2 prop_function ;
REAL    *result ;    /* result[dim] */
{
    REAL *bez_result ;
    INT j ;    
    HPT2 *b ;

    ala_set_zero ( result, dim ) ;
    bez_result = CREATE ( dim, REAL ) ;
    b = CREATE ( d, HPT2 ) ;
    j = PARM_J(parm0) ;
    C2S_CONV_BEZ ( a, d, knot, j, b ) ;

    if ( PARM_T(parm0) > knot[j] + BBS_ZERO ) 
        C2B_BRKR ( b, d, ( PARM_T(parm0) - knot[j] ) / 
            ( knot[j+1] - knot[j] ), b ) ;
    if ( PARM_J(parm1) == j ) {
        C2B_BRKL ( b, d, ( PARM_T(parm1) - PARM_T(parm0) ) / 
            ( knot[j+1] - PARM_T(parm0) ), b ) ;
        c2b_mass_prop ( b, d, w, PARM_T(parm0), PARM_T(parm1), p, dim, tol, 
            prop_function, result ) ;
    }
    else if ( knot[j+1] >  PARM_T(parm0) + BBS_ZERO ) 
        c2b_mass_prop ( b, d, w, PARM_T(parm0), knot[j+1], p, dim, tol, 
            prop_function, result ) ;
      
    for ( j=PARM_J(parm0)+1 ; j<PARM_J(parm1) ; j++ ) {
        if ( knot[j+1]-knot[j] > BBS_ZERO ) {
            C2S_CONV_BEZ ( a, d, knot, j, b ) ;
            c2b_mass_prop ( b, d, w, knot[j], knot[j+1], p, dim, tol, 
                prop_function, bez_result ) ;
            ala_add ( result, bez_result, dim, result ) ;
        }
    }
    
    j = PARM_J(parm1) ;
    if ( PARM_T(parm1) - knot[j] > BBS_ZERO ) {
        C2S_CONV_BEZ ( a, d, knot, j, b ) ;
        if ( PARM_T(parm1) < knot[j+1] - BBS_ZERO ) 
            C2B_BRKL ( b, d, ( PARM_T(parm1) - knot[j] ) / 
                ( knot[j+1] - knot[j] ), b ) ;
        c2b_mass_prop ( b, d, w, knot[j], PARM_T(parm1), p, dim, tol, 
            prop_function, bez_result ) ;
        ala_add ( result, bez_result, dim, result ) ;
    }
    KILL ( b ) ;
    KILL ( bez_result ) ;
}    
#endif  /*SPLINE*/

