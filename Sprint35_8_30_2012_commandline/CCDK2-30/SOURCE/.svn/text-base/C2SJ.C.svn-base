/* -2 -3 */
/********************************** C2SJ.C *********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <bbsdefs.h>
#ifdef  SPLINE
#include <c2bdefs.h>
#include <c2sdefs.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2s_project ( a, d, knot, w, parm0, parm1, 
            pt, pr_parm, proj_pt ) 
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
PARM    pr_parm ;
PT2     proj_pt ;
{
    BOOLEAN status ;
    INT j ;    
    HPT2 *b ;
    HPT2 curr_pt ;
    REAL dist0, dist, t ;    
    PT2     ppt ;

    status = FALSE ;
    b = CREATE ( d, HPT2 ) ;
    j = PARM_J(parm0) ;
    C2S_CONV_BEZ ( a, d, knot, j, b ) ;
    if ( PARM_T(parm0) > knot[j] + BBS_ZERO ) 
        C2B_BRKR ( b, d, ( PARM_T(parm0) - knot[j] ) / ( knot[j+1] - knot[j] ), 
            b ) ;

    if ( PARM_J(parm1) == j ) {
        C2B_BRKL ( b, d, ( PARM_T(parm1) - PARM_T(parm0) ) / 
            ( knot[j+1] - PARM_T(parm0) ), b ) ;
        status = c2b_project ( b, d, w, pt, &t, ppt, &dist ) ;
        if ( status ) {
            PARM_T(pr_parm) = PARM_T(parm0) + 
                ( PARM_T(parm1) - PARM_T(parm0) ) * t ;
            PARM_J(pr_parm) = j ;
            if ( proj_pt != NULL ) 
                C2V_COPY ( ppt, proj_pt ) ;
        }
    }

    else if ( knot[j+1] >  PARM_T(parm0) + BBS_ZERO ) {
        status = c2b_project ( b, d, w, pt, &t, ppt, &dist ) ;
        if ( status ) {
            PARM_T(pr_parm) = PARM_T(parm0) + 
                ( knot[j+1] - PARM_T(parm0) ) * t ;
            PARM_J(pr_parm) = j ;
            if ( proj_pt != NULL ) 
                C2V_COPY ( ppt, proj_pt ) ;
        }
    }  

    for ( j=PARM_J(parm0) + 1 ; j < PARM_J(parm1) ; j++ ) {
        if ( knot[j+1]-knot[j] > BBS_ZERO ) {
            C2S_CONV_BEZ ( a, d, knot, j, b ) ;
            if ( !status ) {
                status = c2b_project ( b, d, w, pt, &t, ppt, &dist ) ;
                if ( status ) {
                    PARM_T(pr_parm) = knot[j] + 
                        ( knot[j+1] - knot[j] ) * t ;
                    PARM_J(pr_parm) = j ;
                    if ( proj_pt != NULL ) 
                        C2V_COPY ( ppt, proj_pt ) ;
                }
            }
            else if ( c2b_project ( b, d, w, pt, &t, curr_pt, &dist0 ) 
                && dist0 < dist ) {
                C2V_COPY ( curr_pt, ppt ) ;
                dist = dist0 ;
                PARM_T(pr_parm) = knot[j] + 
                    ( knot[j+1] - knot[j] ) * t ;
                PARM_J(pr_parm) = j ;
                if ( proj_pt != NULL ) 
                    C2V_COPY ( ppt, proj_pt ) ;
            }
        }
    }

    j = PARM_J(parm1) ;
    if ( knot[j+1]-knot[j] > BBS_ZERO ) {
        C2S_CONV_BEZ ( a, d, knot, j, b ) ;
        if ( PARM_T(parm1) > knot[j] + BBS_ZERO ) {
            if ( PARM_T(parm1) < knot[j+1] - BBS_ZERO )
                C2B_BRKL ( b, d, ( PARM_T(parm1) - knot[j] ) / 
                    ( knot[j+1] - knot[j] ), b ) ;
            if ( !status ) {
                status = c2b_project ( b, d, w, pt, &t, ppt, &dist ) ;
                if ( status ) {
                    PARM_T(pr_parm) = knot[j] + 
                        ( PARM_T(parm1) - knot[j] ) * t ;
                    PARM_J(pr_parm) = j ;
                    if ( proj_pt != NULL ) 
                        C2V_COPY ( ppt, proj_pt ) ;
                }
            }
            else if ( c2b_project ( b, d, w, pt, &t, curr_pt, &dist0 ) 
                && dist0 < dist ) {
                C2V_COPY ( curr_pt, ppt ) ;
                dist = dist0 ;
                PARM_T(pr_parm) = knot[j] + 
                    ( PARM_T(parm1) - knot[j] ) * t ;
                PARM_J(pr_parm) = j ;
                if ( proj_pt != NULL ) 
                    C2V_COPY ( ppt, proj_pt ) ;
            }
        }
    }
    KILL ( b ) ;
    RETURN ( status ) ;
}    
#endif /*SPLINE*/

