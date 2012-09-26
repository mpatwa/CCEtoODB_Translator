/* -2 -3 */
/********************************** C2N.C **********************************/
/********************************** Nurbs **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef  SPLINE
#include <aladefs.h>
#include <alsdefs.h>
#include <c2apriv.h>
#include <c2ndefs.h>
#include <c2nmcrs.h>
#include <c2vmcrs.h>
#include <c2cxtrm.h>
#include <c2mem.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_NURB c2n_create_nurb ( n, d ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT n, d ;
{
    C2_NURB nurb ;

    nurb = C2_ALLOC_NURB ;
    if ( nurb == NULL ) 
        RETURN ( NULL ) ;
    C2_NURB_N(nurb) = n ;
    C2_NURB_D(nurb) = d ;
    C2_NURB_CTPT_PTR(nurb) = MALLOC ( n, HPT2 ) ;
    if ( C2_NURB_CTPT_PTR(nurb) == NULL ) 
        RETURN ( NULL ) ;
    C2_NURB_KNOT_PTR(nurb) = MALLOC ( n+d, REAL ) ;
    if ( C2_NURB_KNOT(nurb) == NULL ) {
        FREE ( C2_NURB_CTPT(nurb) ) ;
        RETURN ( NULL ) ;
    }
    C2_NURB_W(nurb) = 0.0 ;
    C2_NURB_CURV_EXTR_PTR(nurb) = NULL ;
    C2_NURB_CURV_EXTR_NO(nurb) = -1 ;   /* init. value */
    RETURN ( nurb ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2n_set_ctpts ( a, nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a ;
C2_NURB nurb ;
{
    c2a_pt_hpt ( a, C2_NURB_N(nurb), C2_NURB_CTPT(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2n_set_cthpts ( a, nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
C2_NURB nurb ;
{
    ala_copy ( (REAL*)a, 3*C2_NURB_N(nurb), (REAL*)C2_NURB_CTPT(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2n_set_knots ( knot, nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *knot ;
C2_NURB nurb ;
{
    if ( knot == NULL ) 
        c2n_set_uniform_knots ( nurb ) ;
    else
        ala_copy ( knot, C2_NURB_N(nurb) + C2_NURB_D(nurb), 
            C2_NURB_KNOT(nurb) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2n_set_uniform_knots ( nurb ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
{
    als_uniform_knots ( C2_NURB_N(nurb), C2_NURB_D(nurb), 
        C2_NURB_KNOT(nurb) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2n_set_clsc_uni_knots ( nurb ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
{
    als_clsc_uni_knots ( C2_NURB_N(nurb), C2_NURB_D(nurb), 
        C2_NURB_KNOT(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_NURB c2n_create_copy_nurb ( nurb0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb0 ;
{
    C2_NURB nurb ;

    nurb = c2n_create_nurb ( C2_NURB_N(nurb0), C2_NURB_D(nurb0) ) ;
    if ( nurb == NULL ) 
        RETURN ( NULL ) ;
    ala_copy ( (REAL*)C2_NURB_CTPT(nurb0), 3*C2_NURB_N(nurb), 
        (REAL*)C2_NURB_CTPT(nurb) ) ;
    ala_copy ( C2_NURB_KNOT(nurb0), C2_NURB_N(nurb)+C2_NURB_D(nurb), 
        C2_NURB_KNOT(nurb) ) ;
    c2n_copy_extr_rec ( nurb0, nurb ) ;
    C2_NURB_W(nurb) = C2_NURB_W(nurb0) ;
    RETURN ( nurb ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2n_copy_extr_rec ( nurb0, nurb1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb0, nurb1 ;
{
    C2_CURV_EXTR extr_rec0, extr_rec1 ;
    INT i ;

    if ( C2_NURB_CURV_EXTR_NO(nurb0) >= 0 ) {
        C2_NURB_CURV_EXTR_NO(nurb1) = C2_NURB_CURV_EXTR_NO(nurb0) ;
        C2_NURB_CURV_EXTR_PTR(nurb1) = 
            MALLOC ( C2_NURB_CURV_EXTR_NO(nurb0), C2_CURV_EXTR_S ) ;

        extr_rec0 = C2_NURB_CURV_EXTR(nurb0) ;
        extr_rec1 = C2_NURB_CURV_EXTR(nurb1) ;

        for ( i = 0 ; i < C2_NURB_CURV_EXTR_NO(nurb0) ; i++ ) {
            C2_COPY_CURV_EXTR ( extr_rec0+i, extr_rec1+i ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2n_free_nurb ( nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
{
    FREE ( C2_NURB_CTPT(nurb) ) ;
    FREE ( C2_NURB_KNOT(nurb) ) ;
    FREE ( C2_NURB_CURV_EXTR(nurb) ) ;
    C2_FREE_NURB ( nurb ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2n_closed ( nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
{
    RETURN ( C2V_IDENT_PTS ( C2_NURB_CTPT(nurb)[0], 
        C2_NURB_CTPT(nurb)[C2_NURB_N(nurb)-1] ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_NURB c2n_trim ( nurb0, parm0, parm1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb0 ;
PARM parm0, parm1 ;
{
    INT j0, j1, n, d=C2_NURB_D(nurb0) ;
    C2_NURB nurb ;

    if ( PARM_T(parm1) <= PARM_T(parm0) + BBS_ZERO )
        RETURN ( NULL ) ;
    j0 = ( parm0 == NULL ) ? d-1 : PARM_J(parm0) ;
    j1 = ( parm1 == NULL ) ? C2_NURB_N(nurb0)-1 : PARM_J(parm1) ;
    if ( IS_ZERO ( C2_NURB_KNOT(nurb0)[j1] - PARM_T(parm1) ) )
        j1-- ;
    n = j1 - j0 + d ;
    nurb = c2n_create_nurb ( n, d ) ;
    if ( nurb == NULL ) 
        RETURN ( nurb ) ;
    als_trim ( (REAL*)C2_NURB_CTPT(nurb0), d, C2_NURB_KNOT(nurb0), 3, 
        parm0, parm1, (REAL*)C2_NURB_CTPT(nurb), n, C2_NURB_KNOT(nurb) ) ;
    RETURN ( nurb ) ;
}
#endif  /*SPLINE*/

