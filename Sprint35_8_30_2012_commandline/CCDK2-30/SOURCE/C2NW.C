/* -2 -3 */
/********************************** C2NW.C *********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef  SPLINE
#include <alwdefs.h>
#include <c2ndefs.h>
#include <c2sdefs.h>
#include <c2nmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_NURB c2n_get ( file, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE *file ;
PARM parm0, parm1 ;
{
    INT n, d ;
    C2_NURB nurb ;

    if ( !alw_get_int ( file, &n ) || !alw_get_int ( file, &d ) )
        RETURN ( NULL ) ;
    nurb = c2n_create_nurb ( n, d ) ;
    if ( !c2s_get ( file, C2_NURB_CTPT_PTR(nurb), n, d, 
        C2_NURB_KNOT_PTR(nurb), &C2_NURB_W(nurb) ) ||
        !alw_get_parm ( file, parm0 ) || !alw_get_parm ( file, parm1 ) ) {
        c2n_free_nurb ( nurb ) ;
        RETURN ( NULL ) ;
    }
    RETURN ( nurb ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2n_put ( file, nurb, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE *file ;
C2_NURB nurb ;
PARM parm0, parm1 ;
{
    RETURN ( c2s_put ( file, C2_NURB_CTPT(nurb), C2_NURB_N(nurb), 
        C2_NURB_D(nurb), C2_NURB_KNOT(nurb), C2_NURB_W(nurb) ) &&
        alw_put_parm ( file, parm0, NULL ) && 
        alw_put_parm ( file, parm1, NULL ) ) ;
}

#endif  /*SPLINE*/

