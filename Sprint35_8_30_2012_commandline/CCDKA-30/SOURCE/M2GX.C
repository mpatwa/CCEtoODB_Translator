/* __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************** M2GX.C *********************************/
/****************** Routines for processing arc geometry *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h>
#include <c2rdefs.h>
#include <m2gdefs.h>
#include <m2rdefs.h>
#include <m2xdefs.h>
#include <c2gmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2g_xhatch_inters ( arc, t0, t1, pt, h, c, s, 
            owner, intlist, m )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL t0, t1 ;
PT2 pt ;
REAL h, c, s ;
ANY owner ;
DML_LIST *intlist ;
INT m ;
{
    C2_ASEG_S carc ;
    PT2 ctr ;
    REAL u, v ;
    INT k0 ;

    if ( C2_ARC_ZERO_RAD(arc) ) {
        c2r_ctr ( C2_ARC_SEG(arc), ctr ) ;

        v = ( pt[0] - ctr[0] ) * s - ( pt[1] - ctr[1] ) * c ;
        k0 = (INT) floor ( ( v ) / h ) ;
        u = v - (REAL)k0 * h ;
        if ( !IS_ZERO(u) ) 
            RETURN ( 0 ) ;
        RETURN ( ( m2x_append ( intlist[k0], ctr[0], ctr[1], 0.0, 0, 
            owner, 0 ) == NULL ) ? -1 : 1 ) ;
    }

    if ( t1 <= 1.0 + BBS_ZERO ) 
        RETURN ( m2r_xhatch_inters ( C2_ARC_SEG(arc), 
            t0, t1, 0, pt, h, c, s, owner, intlist, m ) ) ;
    else if ( t0 >= 1.0 - BBS_ZERO ) 
        RETURN ( m2r_xhatch_inters ( c2r_complement ( C2_ARC_SEG(arc), 
            &carc ), t0-1.0, t1-1.0, 1, pt, h, c, s, owner, intlist, m ) ) ;
    else 
        RETURN ( m2r_xhatch_inters ( C2_ARC_SEG(arc), 
            t0, 1.0, 0, pt, h, c, s, owner, intlist, m ) + 
            m2r_xhatch_inters ( c2r_complement ( C2_ARC_SEG(arc), &carc ), 
            0.0, t1-1.0, 1, pt, h, c, s, owner, intlist, m ) ) ;
}

