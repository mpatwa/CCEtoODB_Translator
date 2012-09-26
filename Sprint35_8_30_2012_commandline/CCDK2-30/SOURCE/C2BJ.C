/* -2 -3 */
/*********************************** C2BJ.C ********************************/
/*********************** Two-dimensional Bezier curves *********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <bbsdefs.h>
#ifdef  SPLINE
#include <c2bdefs.h>
#include <c2apriv.h>
#include <dmldefs.h>
#include <fnbdefs.h>
#include <c2vmcrs.h>
#include <c2coned.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2b_project ( b, d, w, pt, t_ptr, ppt, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2    *b  ;
INT     d ;
REAL    w ;
PT2     pt ;
REAL    *t_ptr, *dist_ptr ;
PT2     ppt ;
{
    HPT2 *a  ;
    PT2 x[3]  ;
    HREAL *c ;
    REAL z, dist0, dist = 0.0 ;
    BOOLEAN status ;
    DML_LIST xlist ;
    DML_ITEM item ;
    C2_EXTR_REC xr ;

    a = CREATE ( d, HPT2 ) ;
    c2a_sub ( b, d, pt, a ) ;
    c = CREATE ( 2*d-1, HREAL ) ;
    c2b_dot ( a, d, a, d, c ) ;
    
    if ( IS_ZERO(w) ) {
        status = fnb_extr ( c, 2*d-1, FALSE /*min*/, t_ptr, &dist0 ) ;
        if ( status ) {
            if ( IS_ZERO(*t_ptr) && c2b_ept_tan0 ( b, d, w, x[0], x[1] ) ) {
                C2V_SUB ( x[0], pt, x[0] ) ;
                z = C2V_DOT ( x[0], x[1] ) ;
                status = ( fabs(z) <= BBS_ZERO * C2V_NORML1 ( x[1] ) ) ;
            }
            else if ( IS_ZERO(*t_ptr-1.0) && 
                c2b_ept_tan1 ( b, d, w, x[0], x[1] ) ) {
                C2V_SUB ( x[0], pt, x[0] ) ;
                z = C2V_DOT ( x[0], x[1] ) ;
                status = ( fabs(z) <= BBS_ZERO * C2V_NORML1 ( x[1] ) ) ;
            }
        }
    }

    else {  /* Non-zero offset */

        xlist = dml_create_list () ;
        fnb_extrs ( c, 2*d-1, 0.0, 1.0, 0, xlist ) ;
        status = FALSE ;
        DML_WALK_LIST ( xlist, item ) {
            xr = (C2_EXTR_REC)DML_RECORD(item) ;
            c2b_eval ( b, d, w, C2_EXTR_REC_T(xr), 2, (PT2*)x ) ;
            C2V_SUB ( x[0], pt, x[0] ) ;
            z = C2V_DOT ( x[0], x[2] ) + C2V_DOT ( x[1], x[1] ) ;
            if ( z > 0.0 ) {
                dist0 = C2V_DOT ( x[0], x[0] ) ;
                if ( !status || ( dist0 < dist ) ) {
                    dist = dist0 ;
                    status = TRUE ;
                    *t_ptr = C2_EXTR_REC_T(xr) ;
                }
            }
        }
        dml_destroy_list ( xlist, dmm_free ) ;
    }
    if ( status ) {
        if ( dist_ptr != NULL )
            *dist_ptr = ( dist0 > 0.0 ? sqrt ( dist0 ) : 0.0 ) ;
        if ( ppt != NULL ) { /* evaluate the point */
            status = c2b_eval ( b, d, w, *t_ptr, 0, (PT2*)ppt ) ;
        }
    }
    KILL ( a ) ;
    KILL ( c ) ;
    RETURN ( status ) ;
}
#endif /*SPLINE*/

