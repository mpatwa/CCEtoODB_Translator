/* -2 -3 */
/********************************** C2RZ.C *********************************/
/******************** Routines for processing arcs *************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alrdefs.h>
#include <c2ldefs.h>
#include <c2rdefs.h>
#include <c2vdefs.h>
#include <c2lmcrs.h>
#include <c2rmcrs.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2r_divide ( arc, n, t0, t1, pt, parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
INT n ;
REAL t0, t1 ;
PT2* pt ;
PARM parm ;
{
    PT2 ctr, p0, p1, v0, v1 ;
    REAL c, s, t, sweep, angle, step ;
    INT i ;

    if ( c2r_ctr ( arc, ctr ) == NULL ) {
        C2_LINE_S line ;
        C2V_COPY ( C2_ASEG_PT0(arc), C2_LINE_PT0(&line) ) ;
        C2V_COPY ( C2_ASEG_PT1(arc), C2_LINE_PT1(&line) ) ;
        c2l_divide ( &line, n, t0, t1, pt, parm ) ;
    }

    else {
        c2r_pt_tan ( arc, t0, p0, NULL ) ;
        c2r_pt_tan ( arc, t1, p1, NULL ) ;
        C2V_SUB ( p0, ctr, v0 ) ;
        C2V_SUB ( p1, ctr, v1 ) ;
        sweep = c2v_vecs_angle ( v0, v1 ) ;
        step = sweep / ( (REAL)(n-1) ) ;

        C2V_SET ( -v0[1], v0[0], v1 ) ;
        if ( C2_ASEG_D(arc) < 0.0 ) {
            C2V_NEGATE ( v1, v1 ) ;
        }

        for ( i = 0, angle = 0.0 ; i < n ; i++, angle += step ) {
            if ( pt != NULL ) {
                c = cos ( angle ) ;
                s = sin ( angle ) ;
                C2V_ADDC ( ctr, v0, c, v1, s, pt[i] ) ;
            }

            if ( parm != NULL ) {
                t = alr_parm ( angle, C2_ASEG_D(arc) ) ;
                t = alr_trim_parm ( C2_ASEG_D(arc), t, t0, t1 ) ;
                SET_PARM ( t, parm+i ) ;
                alr_parm_set ( parm+i ) ;
            }
        }
    }
}

