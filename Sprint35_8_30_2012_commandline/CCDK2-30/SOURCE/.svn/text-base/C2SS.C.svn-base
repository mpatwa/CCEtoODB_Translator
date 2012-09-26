/* -2 -3 */
/********************************** C2S.C **********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#include <alsdefs.h>
#include <c2bdefs.h>
#include <c2sdefs.h>
#ifdef  SPLINE

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2s_select ( a, d, knot, w, parm0, parm1, pt, tol, 
        sel_parm, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *a  ;
INT     d ;
REAL    *knot  ;
REAL    w ;
PARM    parm0, parm1 ;
PT2     pt ;
REAL    tol ;
PARM    sel_parm ;
REAL    *dist_ptr ;
{
    BOOLEAN status, st_init ;
    INT j ;    
    HPT2 *b ;
    PARM_S parm ;
    REAL t, dist ;

    if ( sel_parm == NULL ) 
        sel_parm = &parm ;
    status = FALSE ;
    st_init = FALSE ;
    b = CREATE ( d, HPT2 ) ;
    j = PARM_J(parm0) ;
    C2S_CONV_BEZ ( a, d, knot, j, b ) ;

    if ( PARM_T(parm0) > knot[j] + BBS_ZERO ) 
        C2B_BRKR ( b, d, ( PARM_T(parm0) - knot[j] ) / ( knot[j+1] - knot[j] ), 
            b ) ;

    if ( PARM_J(parm1) == j ) {
        C2B_BRKL ( b, d, ( PARM_T(parm1) - PARM_T(parm0) ) / 
            ( knot[j+1] - PARM_T(parm0) ), b ) ;
        status = c2b_select ( b, d, w, pt, tol, PARM_T(parm0), 
            PARM_T(parm1), &(PARM_T(sel_parm)), dist_ptr ) ;
        if ( status ) {
            st_init = TRUE ;
            PARM_J(sel_parm) = j ;
        }
    }

    else if ( knot[j+1] >  PARM_T(parm0) + BBS_ZERO ) {
        status = c2b_select ( b, d, w, pt, tol, PARM_T(parm0), 
            knot[j+1], &(PARM_T(sel_parm)), dist_ptr ) ;
        if ( status ) {
            PARM_J(sel_parm) = j ;
            st_init = TRUE ;
        }

        for ( j=PARM_J(parm0)+1 ; 
            j<PARM_J(parm1) && ( !status || dist_ptr != NULL ) ; j++ ) {
            if ( knot[j+1]-knot[j] > BBS_ZERO ) {
                C2S_CONV_BEZ ( a, d, knot, j, b ) ;
                status = c2b_select ( b, d, w, pt, tol, knot[j], 
                    knot[j+1], &t, &dist ) ;
                if ( status ) {
                    if ( dist_ptr == NULL ) {
                        PARM_T(sel_parm) = t ;
                        PARM_J(sel_parm) = j ;
                    }
                    else if ( !st_init || dist < *dist_ptr ) {
                        st_init = TRUE ;
                        PARM_T(sel_parm) = t ;
                        PARM_J(sel_parm) = j ;
                        *dist_ptr = dist ;
                    }
                }
            }
        }
    
        if ( !status || dist_ptr != NULL ) {
            j = PARM_J(parm1) ;
            if ( PARM_T(parm1) - knot[j] > BBS_ZERO ) {
                C2S_CONV_BEZ ( a, d, knot, j, b ) ;
                C2B_BRKL ( b, d, ( PARM_T(parm1) - knot[j] ) / 
                    ( knot[j+1] - knot[j] ), b ) ;
                status = c2b_select ( b, d, w, pt, tol, knot[j],
                     PARM_T(parm1), &t, dist_ptr ) ;
                if ( status ) {
                    if ( dist_ptr == NULL ) {
                        PARM_T(sel_parm) = t ;
                        PARM_J(sel_parm) = j ;
                    }
                    else if ( !st_init || dist < *dist_ptr ) {
                        st_init = TRUE ;
                        PARM_T(sel_parm) = t ;
                        PARM_J(sel_parm) = j ;
                        *dist_ptr = dist ;
                    }
                }
            }
        }
    }
    KILL ( b ) ;
    RETURN ( st_init || status ) ;
}

#endif /*SPLINE*/


