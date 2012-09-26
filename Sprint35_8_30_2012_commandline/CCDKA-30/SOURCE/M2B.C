/* __BBS_MILL__=3 __BBS_TURN__=3 */
/********************************** M2B.C **********************************/
/***************** Routines for processing Bezier geometry *****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <c2bdefs.h>
#include <c2adefs.h>
#include <c2vmcrs.h>
#include <dmldefs.h>
#include <fnbdefs.h>
#include <m2bdefs.h>
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2b_x_max ( b, d, w, x_max_ptr, t_max_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *b ;
INT d ;
REAL w ;
REAL *x_max_ptr, *t_max_ptr ;
{
    PT2 x  ;
    HREAL *c ;
    BOOLEAN status ;
    DML_LIST xlist ;
    DML_ITEM item ;
    FN_EXTR_REC xr ;
    INT i ;

#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_bez ( b, d, w, 12 ) ;
        getch ();
    }
    else
        DISPLAY-- ;
#endif
    c = CREATE ( d, HREAL ) ;
    for ( i=0 ; i<d ; i++ ) {
        c[i][0] = b[i][0] ;
        c[i][1] = b[i][2] ;
    }
    
    if ( IS_ZERO(w) ) 
        status = fnb_extr ( c, d, TRUE /*max*/, t_max_ptr, x_max_ptr ) ;
    else {  /* Non-zero offset */
        xlist = dml_create_list () ;
        fnb_extrs ( c, d, 0.0, 1.0, 0, xlist ) ;
    
        status = FALSE ;
        DML_WALK_LIST ( xlist, item ) {
            xr = DML_RECORD(item) ;
            c2b_eval_pt ( b, d, w, C2_EXTR_REC_T(xr), x ) ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_point ( x, 0.02, 13 ) ;
        getch ();
    }
    else
        DISPLAY-- ;
#endif
            if ( !status || x[0] > *x_max_ptr ) {
                status = TRUE ;
                *x_max_ptr = x[0] ;
                *t_max_ptr = C2_EXTR_REC_T(xr) ;
            }
        }   

        DML_WALK_LIST ( xlist, item ) 
            FREE ( DML_RECORD(item) ) ;
        dml_free_list ( xlist ) ;
    }
    KILL ( c ) ;
    RETURN ( status ) ;
}
#endif /*SPLINE */

