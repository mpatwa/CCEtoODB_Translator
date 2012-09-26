/* __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************** M2RX.C *********************************/
/****************** Routines for processing arc geometry *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2rdefs.h>
#include <dmldefs.h>
#include <m2ldefs.h>
#include <m2rdefs.h>
#include <m2xdefs.h>
#include <c2lmcrs.h>
#include <c2rmcrs.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2r_xhatch_inters ( arc, t0, t1, j, pt, h, c, s, 
            owner, intlist, m )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
REAL t0, t1 ;
INT j ;
PT2 pt ;
REAL h, c, s ;
ANY owner ; 
DML_LIST *intlist ;
INT m ;
{
    INT k0, k1, k, n=0 ;
    REAL r, rad, tol, t, u, v, p0, p1, q0, q1, q ;
    PT2 p, ctr, on_pt ;
    BOOLEAN status, project ;

    if ( c2r_ctr ( arc, ctr ) == NULL ) {
        C2_LINE_S line ;
        C2V_COPY ( C2_ASEG_PT0(arc), C2_LINE_PT0(&line) ) ;
        C2V_COPY ( C2_ASEG_PT1(arc), C2_LINE_PT1(&line) ) ;
        RETURN ( m2l_xhatch_inters ( &line, t0, t1, pt, h, c, s, 
            owner, intlist, m ) ) ;
    }

    rad = c2r_rad ( arc ) ;
    v = ( pt[0] - ctr[0] ) * s - ( pt[1] - ctr[1] ) * c ;
    k0 = (INT) floor ( ( v - rad ) / h ) ;
    k1 = (INT) ceil ( ( v + rad ) / h ) ;
    if ( k0 < 0 ) 
        k0 = 0 ;
    if ( k1 >= m ) 
        k1 = m-1 ;

    r = rad * rad ;
    tol = r * BBS_ZERO ;

    for ( k=k0 ; k<=k1 ; k++ ) {
        u = v - (REAL)k * h ;
        q = r - u * u ;

        if ( q >= - tol ) {

            status = ( q > tol ) ;
            q = status ? sqrt ( q ) : 0.0 ;
            p0 = ctr[0] + u * s ;
            p1 = ctr[1] - u * c ;
            q0 = q * c ;
            q1 = q * s ;
            p[0] = p0 + q0 ;
            p[1] = p1 + q1 ;
            project = c2r_project ( arc, p, &t, on_pt ) ;
            if ( status ) {
                if ( project && t0 - BBS_ZERO <= t && t <= t1 + BBS_ZERO ) {
                    if ( m2x_append ( intlist[k], p[0], p[1], t + (REAL)j, j,
                        owner, status && t0+BBS_ZERO<t && t<t1-BBS_ZERO ) 
                        == NULL )
                        RETURN ( -1 ) ;
                    else 
                        n++ ;
                }
            }
            else {
                if ( project && ( IS_ZERO(t-t0) || IS_ZERO(t-t1) ) ) {
                    if ( m2x_append ( intlist[k], p[0], p[1], t + (REAL)j, j, 
                        owner, status ) == NULL )
                        RETURN ( -1 ) ;
                    else 
                        n++ ;
                }
            }

            p[0] = p0 - q0 ;
            p[1] = p1 - q1 ;
            project = c2r_project ( arc, p, &t, on_pt ) ;
            if ( status ) {
                if ( project && t0 - BBS_ZERO <= t && t <= t1 + BBS_ZERO ) {
                    if ( m2x_append ( intlist[k], p[0], p[1], t + (REAL)j, 
                        j, owner, status && t0+BBS_ZERO<t && t<t1-BBS_ZERO ) 
                        == NULL )
                        RETURN ( -1 ) ;
                    else 
                        n++ ;
                }
            }
            else {
                if ( project && ( IS_ZERO(t-t0) || IS_ZERO(t-t1) ) ) {
                    if ( m2x_append ( intlist[k], p[0], p[1], t + (REAL)j, 
                        j, owner, status ) == NULL )
                        RETURN ( -1 ) ;
                else 
                    n++ ;
                }
            }
        }
    }
    RETURN ( n ) ;
}

