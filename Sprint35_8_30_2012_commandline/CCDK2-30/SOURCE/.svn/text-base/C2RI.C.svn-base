/* -2 -3 */
/********************************** C2R.C **********************************/
/***************** Routines for processing r-arc geometry *****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <alpdefs.h>
#include <c2ldefs.h>
#include <c2rdefs.h>
#include <c2vdefs.h>
#include <c2lmcrs.h>
#include <c2rmcrs.h>
#include <c2vmcrs.h>
STATIC INT c2r_intersect1 __(( C2_ASEG, C2_ASEG, PT2, REAL*, REAL*, 
            PT2*, INT* )) ;
STATIC BOOLEAN c2r_new_inters __(( REAL*, REAL*, INT, INT )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2r_intersect ( arc1, arc2, t1, t2, pt, type ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc1, arc2 ;
REAL *t1 , *t2  ;
PT2 *pt  ;
INT *type  ;
{
    PT2 b1, b2 ;

    C2V_SUB ( C2_ASEG_PT1(arc1), C2_ASEG_PT0(arc1), b1 ) ;
    C2V_SUB ( C2_ASEG_PT1(arc2), C2_ASEG_PT0(arc2), b2 ) ;

    if ( C2V_NORML1 ( b1 ) > C2V_NORML1 ( b2 ) ) 
        RETURN ( c2r_intersect1 ( arc1, arc2, b2, t1, t2, pt, type ) ) ;
    else 
        RETURN ( c2r_intersect1 ( arc2, arc1, b1, t2, t1, pt, type ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT c2r_intersect1 ( arc1, arc2, b, t1, t2, pt, type ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc1, arc2 ;
PT2 b ;
REAL *t1 , *t2  ;
PT2 *pt  ;
INT *type  ;
{
    PT2 a0, a1, vec1, vec2, p ;
    REAL z[3], u1, v1, u2, v2, n0, n1, c, s, c0, c1, s0, s1, t[2], tau ;
    INT i, m, n, mult[2] ;
    BOOLEAN project ;

    C2R_UV ( arc1, u1, v1 ) ;
    C2R_UV ( arc2, u2, v2 ) ;

    C2V_SUB ( C2_ASEG_PT0(arc1), C2_ASEG_PT0(arc2), a0 ) ;
    C2V_SUB ( C2_ASEG_PT1(arc1), C2_ASEG_PT0(arc2), a1 ) ;

    tau = HYPOT ( C2V_DISTL1 ( C2_ASEG_PT1(arc1), C2_ASEG_PT0(arc1) ),
          C2V_NORML1 ( b ) ) ;
    if ( IS_SMALL(tau) ) 
        RETURN ( 0 ) ;      /* Check if they coinside ?? */
    tau = 1.0 / tau ;
    C2V_SCALE ( a0, tau, a0 ) ;
    C2V_SCALE ( a1, tau, a1 ) ;
    C2V_SCALE ( b, tau, b ) ;

    n0 = C2V_DOT ( a0, a0 ) ;
    c = C2V_DOT ( a0, a1 ) ;
    s = C2V_CROSS ( a0, a1 ) ;
    n1 = C2V_DOT ( a1, a1 ) ;
    c0 = C2V_DOT ( a0, b ) ;
    s0 = C2V_CROSS ( a0, b ) ;
    c1 = C2V_DOT ( a1, b ) ;
    s1 = C2V_CROSS ( a1, b ) ;

    z[0] = v2 * ( n0 - c0 ) + u2 * s0 ;
    z[1] = v2 * ( 2.0 * ( c * u1 + s * v1 ) - u1 * ( c0 + c1 ) +
        v1 * ( s1 - s0 ) ) + u2 * ( u1 * ( s0 + s1 ) + v1 * ( c1 - c0 ) ) ;
    z[2] = v2 * ( n1 - c1 ) + u2 * s1 ;

    m = alp_solve_bez3 ( z[0], 0.5*z[1], z[2], t, mult ) ;
    if ( m < 0 ) {
        if ( c2r_ctr ( arc1, a0 ) != NULL && c2r_ctr ( arc2, a1 ) != NULL ) {
            u1 = c2r_rad ( arc1 ) ;
            u2 = c2r_rad ( arc2 ) ;
            RETURN ( C2V_IDENT_PTS(a0,a1) && IS_SMALL(u1-u2) ? -1 : 0 ) ;
        }
        else {
            C2V_SUB ( C2_ASEG_PT1(arc1), C2_ASEG_PT0(arc1), vec1 ) ;
            C2V_SUB ( C2_ASEG_PT1(arc2), C2_ASEG_PT0(arc2), vec2 ) ;
            s = C2V_CROSS ( vec1, vec2 ) ;
            u1 = C2V_CROSS ( a0, vec1 ) ;
            u2 = C2V_CROSS ( a0, vec2 ) ;
            tau = BBS_TOL * C2V_NORM ( vec1 ) ;
            if ( ( fabs(s) <= tau ) ) 
                RETURN ( ( fabs(u1) <= tau ) ? -1 : 0 ) ;
            tau = BBS_TOL * C2V_NORM ( vec2 ) ;
            if ( ( fabs(s) <= tau ) ) 
                RETURN ( ( fabs(u2) <= tau ) ? -1 : 0 ) ;
            else
                RETURN ( 0 ) ;
      }
    }
    n = 0 ;

    for ( i=0 ; i<m ; i++ ) {
        if ( t[i] >= 0.0 && t[i] <= 1.0 ) {
            c2r_pt_tan ( arc1, t[i], p, NULL ) ;
            project = c2r_project ( arc2, p, &tau, NULL ) ;
/* yg added 120692 
            if ( !project && ( mult[i] == 2 ) ) {
                if ( ( tau >= 1.0 ) && ( tau <= 1.0 + sqrt ( BBS_ZERO ) ) ) {
                    tau = 1.0 ;
                    project = TRUE ;
                }
                else if ( tau >= 2.0 - sqrt ( BBS_ZERO ) ) {
                    tau = 0.0 ;
                    project = TRUE ;
                }
            }
 yg added 120692 */
            if ( project ) {
                t1[n] = t[i] ;
                t2[n] = tau ;
                C2V_COPY ( p, pt[n] ) ;
                type[n] = mult[i] ;
                n++ ;
            }
        }
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2r_inters_line ( arc, line, t1, t2, pt, type ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
C2_LINE line ;
REAL *t1 , *t2  ;
PT2 *pt  ;
INT *type  ;
{
    PT2 b0, b1, a, b ;
    REAL t[2], z[3], u, v ;
    INT i, m, n, mult[2] ;

    C2R_UV ( arc, u, v ) ;
    C2V_SUB ( C2_ASEG_PT0(arc), C2_LINE_PT0(line), b0 ) ;
    C2V_SUB ( C2_ASEG_PT1(arc), C2_LINE_PT0(line), b1 ) ;
    C2V_SUB ( C2_ASEG_PT1(arc), C2_ASEG_PT0(arc), a ) ;
    C2V_SUB ( C2_LINE_PT1(line), C2_LINE_PT0(line), b ) ;
/* 09-22-91 */
    c2v_normalize_l1 ( b, b ) ;
/* 09-22-91 */

    z[0] = C2V_CROSS ( b0, b ) ;
    z[2] = C2V_CROSS ( b1, b ) ;
    z[1] = u * ( z[0] + z[2] ) + v * C2V_DOT ( a, b ) ;
    m = alp_solve_bez3 ( z[0], 0.5*z[1], z[2], t, mult ) ;
    n = 0 ;

    if ( m == - 1 ) {
        C2_LINE_S arc_line ;

        C2V_COPY ( C2_ASEG_PT0(arc), C2_LINE_PT0(&arc_line) ) ;
        C2V_COPY ( C2_ASEG_PT1(arc), C2_LINE_PT1(&arc_line) ) ;
        RETURN ( c2l_intersect_coinc ( &arc_line, 0.0, 1.0, line, 0.0, 1.0, 
            t1, t2, pt, type ) ) ;
    }

    for ( i=0 ; i<m ; i++ ) {
        if ( t[i] >= -BBS_ZERO && t[i] <= 1.0 + BBS_ZERO ) {
            c2r_pt_tan ( arc, t[i], a, NULL ) ;
            t1[n] = t[i] ;
            t2[n] = c2l_project ( line, a, NULL ) ;
            C2V_COPY ( a, pt[n] ) ;
            type[n] = mult[i] ;
            n++ ;
        }
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2r_intersect_coinc ( arc1, arc2, t1, t2, int_pt, type ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc1, arc2 ;
REAL *t1, *t2 ;
PT2 *int_pt ;
INT *type ;
{
    REAL t ;
    INT n = 0 ;
    PT2 proj_pt ;

    if ( c2r_project ( arc2, C2_ASEG_PT0(arc1), &t, proj_pt ) && 
        t >= - BBS_ZERO && t <= 1.0 + BBS_ZERO ) {
        t1[n] = 0.0 ;
        t2[n] = t ;
        C2V_COPY ( C2_ASEG_PT0(arc1), int_pt[n] ) ;
        type[n] = ( !IS_ZERO(t) && !IS_ZERO(t-1.0) ) ? -2 : -1 ;
        if ( c2r_new_inters ( t1, t2, type[n], n ) )
            n++ ;
    }

    if ( c2r_project ( arc2, C2_ASEG_PT1(arc1), &t, proj_pt ) && 
        t >= - BBS_ZERO && t <= 1.0 + BBS_ZERO ) {
        t1[n] = 1.0 ;
        t2[n] = t ;
        type[n] = ( !IS_ZERO(t) && !IS_ZERO(t-1.0) ) ? -2 : -1 ;
        C2V_COPY ( C2_ASEG_PT1(arc1), int_pt[n] ) ;
        if ( c2r_new_inters ( t1, t2, type[n], n ) )
            n++ ;
    }

    if ( n == 2 ) {
        if ( type[0] == -1 && type[1] == -1 && IS_ZERO(t1[0]) && 
            IS_ZERO(t2[0]) && IS_ZERO(t1[1]-1.0) && IS_ZERO(t2[1]) ) {
            t2[1] = 1.0 ;
            type[0] = type[1] = -2 ;
        }
        RETURN ( c2l_coinc_order ( t1, t2, int_pt, type, n ) ) ;
    }

    if ( c2r_project ( arc1, C2_ASEG_PT0(arc2), &t, proj_pt ) && 
        t >= - BBS_ZERO && t <= 1.0 + BBS_ZERO ) {
        t1[n] = t ;
        t2[n] = 0.0 ;
        C2V_COPY ( C2_ASEG_PT0(arc2), int_pt[n] ) ;
        type[n] = ( !IS_ZERO(t) && !IS_ZERO(t-1.0) ) ? -2 : -1 ;
        if ( c2r_new_inters ( t1, t2, type[n], n ) )
            n++ ;
    }

    if ( c2r_project ( arc1, C2_ASEG_PT1(arc2), &t, proj_pt ) && 
        t >= - BBS_ZERO && t <= 1.0 + BBS_ZERO ) {
        t1[n] = t ;
        t2[n] = 1.0 ;
        C2V_COPY ( C2_ASEG_PT1(arc2), int_pt[n] ) ;
        type[n] = ( !IS_ZERO(t) && !IS_ZERO(t-1.0) ) ? -2 : -1 ;
        if ( c2r_new_inters ( t1, t2, type[n], n ) )
            n++ ;
    }
    if ( n == 2 && type[0] == -1 && type[1] == -1 && IS_ZERO(t1[0]) 
        && IS_ZERO(t2[0]) && IS_ZERO(t1[1]-1.0) && IS_ZERO(t2[1]-1.0) ) 
        type[0] = type[1] = -2 ;
    RETURN ( c2l_coinc_order ( t1, t2, int_pt, type, n ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c2r_new_inters ( t1, t2, type, n ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *t1, *t2 ;
INT type, n ;
{
    INT i ;
    if ( type != -1 ) 
        RETURN ( TRUE ) ;
    for ( i=0 ; i<n ; i++ ) {
        if ( ( IS_ZERO(t1[i]-t1[n]) || IS_ZERO(fabs(t1[i]-t1[n])-2.0) ) && 
             ( IS_ZERO(t2[i]-t2[n]) || IS_ZERO(fabs(t2[i]-t2[n])-2.0) ) ) 
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}

