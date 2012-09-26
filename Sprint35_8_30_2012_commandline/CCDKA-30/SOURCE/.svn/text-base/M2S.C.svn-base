/* __BBS_MILL__=3 __BBS_TURN__=3 */
/********************************** M2S.C **********************************/
/****************** Routines for processing arc geometry *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <c2bdefs.h>
#include <c2sdefs.h>
#include <c2apriv.h>
#include <c2ddefs.h>
#include <c2vmcrs.h>
#include <dmldefs.h>
#include <m2bdefs.h>
#include <m2sdefs.h>

STATIC void m2s_area_function __(( PT2*, REAL* )) ;
STATIC void m2s_mass_ctr_function __(( PT2*, REAL* )) ;
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2s_x_max ( a, d, knot, w, parm0, parm1, 
            x_max_ptr, x_max_parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
INT d ;
REAL *knot ;
REAL w ;
PARM parm0, parm1 ; 
REAL *x_max_ptr ;
PARM x_max_parm ;
{
    BOOLEAN status ;
    INT j ;    
    HPT2 *b ;
    REAL x_max0, x_max, t ;    
    PARM_S parm ;

    status = FALSE ;
    b = CREATE ( d, HPT2 ) ;
    j = PARM_J(parm0) ;
    C2S_CONV_BEZ ( a, d, knot, j, b ) ;
    if ( PARM_T(parm0) > knot[j] + BBS_ZERO ) 
        C2B_BRKR ( b, d, ( PARM_T(parm0) - knot[j] ) / ( knot[j+1] - knot[j] ), 
            b ) ;

    if ( PARM_J(parm1) == j ) {
        C2B_BRKL ( b, d, ( PARM_T(parm1) - PARM_T(parm0) ) / 
            ( knot[j+1] - PARM_T(parm0) ), b ) ;
        status = m2b_x_max ( b, d, w, &x_max, &t ) ;
        if ( status ) {
            PARM_T(&parm) = PARM_T(parm0) + 
                ( PARM_T(parm1) - PARM_T(parm0) ) * t ;
            PARM_J(&parm) = j ;
        }
    }

    else if ( knot[j+1] >  PARM_T(parm0) + BBS_ZERO ) {
        status = m2b_x_max ( b, d, w, &x_max, &t ) ;
        if ( status ) {
            PARM_T(&parm) = PARM_T(parm0) + 
                ( knot[j+1] - PARM_T(parm0) ) * t ;
            PARM_J(&parm) = j ;
        }
      
        for ( j=PARM_J(parm0) + 1 ; j < PARM_J(parm1) ; j++ ) {
            if ( knot[j+1]-knot[j] > BBS_ZERO ) {
                C2S_CONV_BEZ ( a, d, knot, j, b ) ;
                if ( !status ) {
                    status = m2b_x_max ( b, d, w, &x_max, &t ) ;
                    if ( status ) {
                        PARM_T(&parm) = knot[j] + 
                            ( knot[j+1] - knot[j] ) * t ;
                        PARM_J(&parm) = j ;
                    }
                }
                else if ( m2b_x_max ( b, d, w, &x_max0, &t ) 
                    && ( x_max0 > x_max ) ) {
                    x_max = x_max0 ;
                    PARM_T(&parm) = knot[j] + 
                        ( knot[j+1] - knot[j] ) * t ;
                    PARM_J(&parm) = j ;
                }
            }
        }
    
        j = PARM_J(parm1) ;
        if ( knot[j+1]-knot[j] > BBS_ZERO ) {
            C2S_CONV_BEZ ( a, d, knot, j, b ) ;
            if ( PARM_T(parm1) > knot[j] + BBS_ZERO ) {
                if ( PARM_T(parm1) < knot[j+1] - BBS_ZERO )
                    C2B_BRKL ( b, d, ( PARM_T(parm1) - knot[j] ) / 
                        ( knot[j+1] - knot[j] ), b ) ;
                if ( !status ) {
                    status = m2b_x_max ( b, d, w, &x_max, &t ) ;
                    if ( status ) {
                        PARM_T(&parm) = knot[j] + 
                            ( PARM_T(parm1) - knot[j] ) * t ;
                        PARM_J(&parm) = j ;
                    }
                }
                else if ( m2b_x_max ( b, d, w, &x_max0, &t ) 
                    && ( x_max0 > x_max ) ) {
                    x_max = x_max0 ;
                    PARM_T(&parm) = knot[j] + 
                        ( PARM_T(parm1) - knot[j] ) * t ;
                    PARM_J(&parm) = j ;
                }
            }
        }
    }

    if ( x_max_ptr != NULL ) {
        if ( (*x_max_ptr) < x_max ) {
            *x_max_ptr = x_max ;
            if ( x_max_parm != NULL ) 
                PARM_COPY ( &parm, x_max_parm ) ;
        }
    }
    else if ( x_max_parm != NULL ) 
        PARM_COPY ( &parm, x_max_parm ) ;

    KILL ( b ) ;
    RETURN ( status ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2s_hor_ray_int_no ( a, n, d, knot, w, 
            parm0, parm1, pt, pt_on_curve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
INT n, d ;
REAL *knot ;
REAL w ;
PARM parm0, parm1 ; 
PT2 pt ;
BOOLEAN pt_on_curve ;
{
    DML_LIST inters_list = dml_create_list () ;
    INT inters_no ;
    DML_ITEM item ;
    C2_INT_REC ci ;

    inters_no = c2s_inters_hor_line ( a, n, d, knot, w, parm0, parm1, pt, 
        1, inters_list ) ;
    DML_WALK_LIST ( inters_list, item ) {
        ci = DML_RECORD(item) ;
        if ( IS_ZERO(C2_INT_REC_T1(ci)-PARM_T(parm0)) ||
             IS_ZERO(C2_INT_REC_T1(ci)-PARM_T(parm1)) ) {
            dml_destroy_list ( inters_list, c2d_free_int_rec ) ;
            RETURN ( M2_END_PT_ON_RAY ) ;
        }
        if ( C2V_IS_SMALL(C2_INT_REC_PT(ci)) && pt_on_curve ) {
            dml_destroy_list ( inters_list, c2d_free_int_rec ) ;
            RETURN ( M2_PT_ON_CURVE ) ;
        }
    }
    dml_destroy_list ( inters_list, c2d_free_int_rec ) ;
    RETURN ( inters_no ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2s_ray_int_no ( a, n, d, knot, w, parm0, parm1, 
            pt, c, s, pt_on_curve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
INT n, d ;
REAL *knot ;
REAL w ;
PARM parm0, parm1 ; 
PT2 pt ;
REAL c, s ;
BOOLEAN pt_on_curve ;
{
    INT m ;

    c2a_rotate_hpt_cs ( a, n, pt, c, -s, a ) ;
    m = m2s_hor_ray_int_no ( a, n, d, knot, w, parm0, parm1, 
        pt, pt_on_curve ) ;
    c2a_rotate_hpt_cs ( a, n, pt, c, s, a ) ;
    RETURN ( m ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE REAL m2s_area ( a, n, d, knot, w, parm0, parm1, origin ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
INT n, d ;
REAL *knot ;
REAL w ;
PARM parm0, parm1 ; 
PT2 origin ;
{
    REAL area ;
    c2a_sub ( a, n, origin, a ) ;
    c2s_mass_prop ( a, d, knot, w, parm0, parm1, 1, 1, 10.0*BBS_TOL, 
        m2s_area_function, &area ) ;
    C2V_NEGATE ( origin, origin ) ;
    c2a_sub ( a, n, origin, a ) ;
    C2V_NEGATE ( origin, origin ) ;
    RETURN ( area ) ;
}


/*----------------------------------------------------------------------*/
STATIC void m2s_area_function ( x, result )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *x ;
REAL *result ;
{
    *result = 0.5 * C2V_CROSS ( x[0], x[1] ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2s_area_mass_ctr ( a, d, knot, w, parm0, parm1, ctr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
INT d ;
REAL *knot ;
REAL w ;
PARM parm0, parm1 ; 
PT2 ctr ;
{

    REAL c[3] ;
    c2s_mass_prop ( a, d, knot, w, parm0, parm1, 1, 3, 10.0*BBS_TOL, 
        m2s_mass_ctr_function, c ) ;
    if ( IS_SMALL(c[2]) ) 
        RETURN ( FALSE ) ;
    c[2] *= 1.5 ;
    ctr[0] = c[0] / c[2] ;
    ctr[1] = c[1] / c[2] ;
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
STATIC void m2s_mass_ctr_function ( x, result )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *x ;
REAL *result ;
{
    result[2] = 0.5 * C2V_CROSS ( x[0], x[1] ) ;
    result[0] = x[0][0] * result[2] ;
    result[1] = x[0][1] * result[2] ;
}

#endif /*SPLINE*/

