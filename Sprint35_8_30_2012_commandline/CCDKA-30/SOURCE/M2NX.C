/* __BBS_MILL__=3 __BBS_TURN__=3 */
/********************************** M2NX.C *********************************/
/****************** Routines for processing nurb geometry ******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <c2nmcrs.h>
#include <m2ndefs.h>
#include <m2sdefs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2n_xhatch_inters ( nurb, parm0, parm1, 
            pt, h, c, s, owner, intlist, m ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm0, parm1 ; 
PT2 pt ;
REAL h, c, s ;
ANY owner ;
DML_LIST *intlist ;
INT m ;
{
    RETURN ( m2s_xhatch_inters ( C2_NURB_CTPT(nurb), C2_NURB_N(nurb), 
        C2_NURB_D(nurb), C2_NURB_KNOT(nurb), C2_NURB_W(nurb), 
        parm0, parm1, pt, h, c, s, owner, intlist, m ) ) ;
}
#endif /*SPLINE*/

