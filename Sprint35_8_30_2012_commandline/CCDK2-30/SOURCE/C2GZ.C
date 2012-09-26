/* -2 -3 */
/********************************** C2GZ.C *********************************/
/******************** Routines for processing arcs *************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2gdefs.h>
#include <c2rdefs.h>
#include <c2gmcrs.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2g_divide ( arc, n, t0, t1, pt, parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
INT n ;
REAL t0, t1 ;
PT2* pt ;
PARM parm ;
{
    REAL h, t ;
    INT i ;

    if ( C2_ARC_ZERO_RAD(arc) ) {
        PT2 ctr ;
        c2r_ctr ( C2_ARC_SEG(arc), ctr ) ;
        h = ( t1 - t0 ) / ( (REAL)(n-1) ) ;

        for ( i = 0, t = t0 ; i < n ; i++, t+=h ) {
            if ( pt != NULL ) {
                C2V_COPY ( ctr, pt[i] ) ;
            }
            if ( parm != NULL ) {
                PARM_SETJ ( t, 0, parm+i ) ;
            }
        }
    }

    else
        c2r_divide ( C2_ARC_SEG(arc), n, t0, t1, pt, parm ) ;
}
