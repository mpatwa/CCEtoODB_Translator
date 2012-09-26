/* -2 -3 */
/********************************** C2SR.C *********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#include <c2sdefs.h>
#include <c2hmcrs.h>
#ifdef  SPLINE

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2s_reverse ( c, n, d, knot, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *c  ;
INT n ;
INT d ;
REAL *knot  ;
PARM parm0, parm1 ;
{
    INT i0, i1 ;
    HPT2 a ;
    REAL tt, t ;
    INT j ;

    for ( i0 = 0, i1 = n-1 ; i0 < i1 ; i0++, i1-- ) {
        C2H_COPY ( c[i0], a ) ;
        C2H_COPY ( c[i1], c[i0] ) ;
        C2H_COPY ( a, c[i1] ) ;
    }

    tt = knot[0] + knot[n+d-1] ;
    for ( i0 = 0, i1 = n+d-1 ; i0 <= i1 ; i0++, i1-- ) {
        t = knot[i0] ;
        knot[i0] = tt - knot[i1] ;
        knot[i1] = tt - t ;
    }
    t = PARM_T(parm0) ;
    PARM_T(parm0) = tt - PARM_T(parm1) ;
    PARM_T(parm1) = tt - t ;
    j = PARM_J(parm0) ;
    PARM_J(parm0) = n - PARM_J(parm1) ;
    PARM_J(parm1) = n - j ;
}
#endif  /*SPLINE*/

