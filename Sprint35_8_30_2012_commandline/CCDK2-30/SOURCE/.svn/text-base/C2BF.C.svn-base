/* -2 -3 */
/*********************************** C2BF.C ********************************/
/*********************** Two-dimensional Bezier curves *********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <bbsdefs.h>
#ifdef  SPLINE
#include <c2bdefs.h>
#include <c2apriv.h>
#include <c2vdefs.h>
#include <c2vmcrs.h>
#include <c2coned.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2b_flat ( b, d, w ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2 *b  ;      /* Control points */
INT d ;         /* Order */
REAL w ;        /* Offset */
{
    PT2 ept0, ept1 ;
    INT conv=0 ;

    c2b_ept0 ( b, d, w, ept0 ) ;
    c2b_ept1 ( b, d, w, ept1 ) ;
    RETURN ( c2b_flat_epts ( b, d, ept0, ept1, &conv ) ) ;
}   


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2b_flat_epts ( b, d, ept0, ept1, conv ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2 *b  ;      /* Control points */
INT d ;         /* Order */
PT2 ept0, ept1 ;
INT *conv ;
{
    C2_CONE_S cone ;

    c2a_dcone ( b, d, conv, &cone ) ;
    RETURN ( c2a_flat_epts_cone ( ept0, ept1, conv, &cone ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2b_flat_non_rat ( b, d ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

PT2 *b  ;      /* Control points */
INT d ;         /* Order */
{
    PT2 ept_vec, vec ;
    REAL flatness, fl_i ;
    INT i ;

    C2V_SUB ( b[d-1], b[0], ept_vec ) ;
    if ( !c2v_normalize ( ept_vec, ept_vec ) ) 
        RETURN ( -1.0 ) ;

    flatness = 0.0 ;

    for ( i=1 ; i<d-1 ; i++ ) {
        C2V_SUB ( b[i], b[0], vec ) ;
        fl_i = C2V_CROSS ( vec, ept_vec ) ;
        if ( fl_i < 0.0 ) 
            fl_i = - fl_i ;
        if ( fl_i > flatness ) 
            flatness = fl_i ;
    }
    RETURN ( flatness ) ;
}
#endif /*SPLINE*/


