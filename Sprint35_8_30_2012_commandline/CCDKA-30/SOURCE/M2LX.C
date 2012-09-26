/* __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************** M2LX.C *********************************/
/******************** Routines for processing lines ************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <m2ldefs.h>
#include <m2xdefs.h>
#include <c2lmcrs.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2l_xhatch_inters ( line, t0, t1, pt, h, c, s, 
            owner, intlist, m )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
REAL t0, t1 ;
PT2 pt ;
REAL h, c, s ;
ANY owner ;
DML_LIST *intlist ;
INT m ;
{
    INT k, k0, k1 ;
    PT2 d ;
    REAL u, v, t, dt, dx, dy, x, y, tt, x0, y0 ;
    BOOLEAN status ;

    C2_LINE_DIR_VEC ( line, d ) ;

    u = d[1] * c - d[0] * s ;
    v = ( C2_LINE_PT0(line)[1] - pt[1] ) * c - 
        ( C2_LINE_PT0(line)[0] - pt[0] ) * s ;

    if ( IS_SMALL(u) ) 
        RETURN ( 0 ) ;
    if ( u > 0.0 ) {
        k0 = (INT) floor ( ( t0 * u + v ) / h + 1.0 - BBS_ZERO ) ;
        k1 = (INT) floor ( ( t1 * u + v ) / h + BBS_ZERO ) ;
    }
    else {
        k0 = (INT) floor ( ( t1 * u + v ) / h + 1.0 - BBS_ZERO ) ;
        k1 = (INT) floor ( ( t0 * u + v ) / h + BBS_ZERO ) ;
    }

    if ( k0 < 0 ) 
        k0 = 0 ;
    if ( k1 >= m ) 
        k1 = m-1 ;
    dt = h / u ;
    tt = ( k0 * h - v ) / u ;
    x0 = C2_LINE_PT0(line)[0] + tt*d[0] ;
    y0 = C2_LINE_PT0(line)[1] + tt*d[1] ;
    dx = d[0] * dt ;
    dy = d[1] * dt ;
    for ( k = k0, t = tt, x = x0, y = y0 ; k<=k1 ; k++, t+=dt, x+=dx, y+=dy ) {
        status = !IS_ZERO(t-t0) && !IS_ZERO(t-t1) ;
        if ( m2x_append ( intlist[k], x, y, t, 1, owner, status ) == NULL )
            RETURN ( -1 ) ;
    }
    k = k1 - k0 + 1 ;
    if ( k < 0 ) 
        k = 0 ;
    RETURN ( k ) ;
}


