/* -2 -3 */
/********************************** C2SP.C *********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef  SPLINE
#include <c2bdefs.h>
#include <c2sdefs.h>
#include <c2apriv.h>

STATIC BOOLEAN spl_approx __(( HPT2*, INT, INT, REAL*, REAL, PARM, PARM, REAL, 
        C2_BOX, BOOLEAN, BOOLEAN, PT2*, PARM, INT, INT*, PARM )) ;


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2s_approx ( a, n, d, knot, offset, parm0, parm1, 
            gran, dir, pt_buffer, parm_buffer, buf_size, index, end_parm )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
INT n, d ;
REAL *knot ;
REAL offset ;
PARM parm0, parm1 ;
REAL gran ;
BOOLEAN dir ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
PARM end_parm ;
{

    BOOLEAN non_rational ;

    non_rational = ( IS_ZERO(offset) && c2a_non_rational ( a, n ) ) ;

    RETURN ( spl_approx ( a, n, d, knot, offset, parm0, parm1, gran, NULL, 
     dir, non_rational, pt_buffer, parm_buffer, buf_size, index, end_parm ) ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2s_approx_zoomed ( a, n, d, knot, offset, parm0, parm1, 
        gran, x, y, w, pt_buffer, parm_buffer, buf_size, index, end_parm )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
INT n, d ;
REAL *knot ;
REAL offset ;
PARM parm0, parm1 ;
REAL gran ;
REAL x, y, w ; 
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
PARM end_parm ;
{
    C2_BOX_S box ;

    C2_MIN_X(&box) = x - 0.5 * w ;
    C2_MAX_X(&box) = C2_MIN_X(&box) + w ;
    C2_MIN_Y(&box) = y - 0.5 * w ;
    C2_MAX_Y(&box) = C2_MIN_Y(&box) + w ;

    RETURN ( spl_approx ( a, n, d, knot, offset, parm0, parm1, gran, &box, 
        TRUE, FALSE, pt_buffer, parm_buffer, buf_size, index, end_parm ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN spl_approx ( a, n, d, knot, w, parm0, parm1, gran, box, 
        dir, non_rational, pt_buffer, parm_buffer, buf_size, index, end_parm )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
INT n, d ;
REAL *knot ;
REAL w ;
PARM parm0, parm1 ;
REAL gran ;
C2_BOX box ;
BOOLEAN dir ;
BOOLEAN non_rational ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
PARM end_parm ;
{
    BOOLEAN status ;
    INT j0, j ;    
    HPT2 *b ;
    REAL t ;

    b = CREATE ( d, HPT2 ) ;

    if ( dir ) {
        j0 = PARM_J(parm0) ;
        while ( j0 < PARM_J(parm1) && PARM_T(parm0) >= knot[j0+1] - BBS_ZERO ) 
            j0++ ;
        C2S_CONV_BEZ ( a, d, knot, j0, b ) ;
        if ( PARM_T(parm0) > knot[j0] + BBS_ZERO ) 
            C2B_BRKR ( b, d, ( PARM_T(parm0) - knot[j0] ) / 
                ( knot[j0+1] - knot[j0] ), b ) ;

        if ( PARM_J(parm1) == j0 ) {
            if ( knot[j0+1] > PARM_T(parm0) + BBS_ZERO ) 
                C2B_BRKL ( b, d, ( PARM_T(parm1) - PARM_T(parm0) ) / 
                    ( knot[j0+1] - PARM_T(parm0) ), b ) ;

            status = c2b_approx ( b, d, w, PARM_T(parm0), PARM_T(parm1), j0, 
                gran, box, dir, non_rational, TRUE, pt_buffer, parm_buffer, 
                buf_size, index, &t ) ;

            if ( !status ) 
                SET_PARMJ ( t, j0, end_parm ) ;
        }

        else {
            status = c2b_approx ( b, d, w, PARM_T(parm0), knot[j0+1], j0, 
                gran, box, dir, non_rational, TRUE, pt_buffer, parm_buffer, 
                buf_size, index, &t ) ;
            if ( !status ) 
                SET_PARMJ ( t, j0, end_parm ) ;
      
            for ( j=j0+1 ; j<PARM_J(parm1) && status ; j++ ) {
                if ( knot[j+1]-knot[j] > BBS_ZERO ) {
                    C2S_CONV_BEZ ( a, d, knot, j, b ) ;
/*
                    (*index)-- ;
*/
                    status = c2b_approx ( b, d, w, knot[j], knot[j+1], j, 
                        gran, box, dir, non_rational, box!=NULL, 
                        pt_buffer, parm_buffer, buf_size, index, &t ) ;
                    if ( !status ) 
                        SET_PARMJ ( t, j, end_parm ) ;
                }
            }
    
            j = PARM_J(parm1) ;
            if ( status && knot[j+1]-knot[j] > BBS_ZERO ) {
                C2S_CONV_BEZ ( a, d, knot, j, b ) ;
                if ( PARM_T(parm1) > knot[j] + BBS_ZERO ) {
                    if ( PARM_T(parm1) < knot[j+1] - BBS_ZERO ) 
                        C2B_BRKL ( b, d, ( PARM_T(parm1) - knot[j] ) / 
                            ( knot[j+1] - knot[j] ), b ) ;
/*
                    (*index)-- ;
*/
                    status = c2b_approx ( b, d, w, knot[j], PARM_T(parm1), j, 
                        gran, box, dir, non_rational, box!=NULL, 
                        pt_buffer, parm_buffer, buf_size, index, &t ) || 
                        ( t >= PARM_T(parm1) ) ;
                    if ( !status ) 
                        SET_PARMJ ( t, j, end_parm ) ;
                }
            }
        }
    }

    else {
        j0 = PARM_J(parm0) ;
        while ( PARM_T(parm0) <= knot[j0] + BBS_ZERO ) 
            j0-- ;
        C2S_CONV_BEZ ( a, d, knot, j0, b ) ;
        if ( PARM_T(parm0) < knot[j0+1] - BBS_ZERO ) 
            C2B_BRKL ( b, d, ( PARM_T(parm0) - knot[j0] ) / 
                ( knot[j0+1] - knot[j0] ), b ) ;

        if ( PARM_J(parm1) == j0 ) {
            C2B_BRKR ( b, d, ( PARM_T(parm1) - knot[j0] ) / 
                ( PARM_T(parm0) - knot[j0] ), b ) ;

            status = c2b_approx ( b, d, w, PARM_T(parm0), PARM_T(parm1), j0, 
                gran, box, dir, non_rational, TRUE, pt_buffer, parm_buffer, 
                buf_size, index, &t ) ;

            if ( !status ) 
                SET_PARMJ ( t, j0, end_parm ) ;
        }

        else {
            status = c2b_approx ( b, d, w, PARM_T(parm0), knot[j0], j0, 
                gran, box, dir, non_rational, TRUE, pt_buffer, parm_buffer, 
                buf_size, index, &t ) ;
            if ( !status ) 
                SET_PARMJ ( t, j0, end_parm ) ;
      
            for ( j=j0-1 ; j>PARM_J(parm1) && status ; j-- ) {
                if ( knot[j+1]-knot[j] > BBS_ZERO ) {
                    C2S_CONV_BEZ ( a, d, knot, j, b ) ;
/*
                    (*index)-- ;
*/
                    status = c2b_approx ( b, d, w, knot[j+1], knot[j], j, 
                        gran, box, dir, non_rational, box!=NULL, 
                        pt_buffer, parm_buffer, buf_size, index, &t ) ;
                    if ( !status ) 
                        SET_PARMJ ( t, j, end_parm ) ;
                }
            }
    
            j = PARM_J(parm1) ;
            if ( status && knot[j+1]-knot[j] > BBS_ZERO ) {
                C2S_CONV_BEZ ( a, d, knot, j, b ) ;
                if ( PARM_T(parm1) < knot[j+1] - BBS_ZERO ) {
                    if ( PARM_T(parm1) > knot[j] + BBS_ZERO ) 
                        C2B_BRKR ( b, d, ( PARM_T(parm1) - knot[j] ) / 
                            ( knot[j+1] - knot[j] ), b ) ;
/*
                    (*index)-- ;
*/
                    status = c2b_approx ( b, d, w, knot[j+1], PARM_T(parm1),
                        j, gran, box, dir, non_rational, box!=NULL, 
                        pt_buffer, parm_buffer, buf_size, index, &t ) || 
                        ( t <= PARM_T(parm1) ) ;
                    if ( !status ) 
                        SET_PARMJ ( t, j, end_parm ) ;
                }
            }
        }
    }

    KILL ( b ) ;
    RETURN ( status ) ;
}    
#endif  /*SPLINE*/

