/* -2 -3 */
/********************************** C2NS.C *********************************/
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
#include <c2adefs.h>
#include <c2apriv.h>
#include <c2ndefs.h>
#include <c2sdefs.h>
#include <c2tdefs.h>
#include <c2hmcrs.h>
#include <c2nmcrs.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2n_box ( nurb, box ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
C2_BOX box ;
{
    c2a_box_hpoly ( C2_NURB_CTPT(nurb), C2_NURB_N(nurb), box ) ;
    c2a_box_inflate ( box, fabs(C2_NURB_W(nurb)) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2n_translate ( nurb, shift ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PT2 shift ;
{
    INT i ;
    for ( i=0 ; i<C2_NURB_N(nurb) ; i++ ) {
        C2_NURB_CTPT(nurb)[i][0] += shift[0] * C2_NURB_CTPT(nurb)[i][2] ;
        C2_NURB_CTPT(nurb)[i][1] += shift[1] * C2_NURB_CTPT(nurb)[i][2] ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2n_scale ( nurb, a, factor ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PT2 a ;
REAL factor ;
{
    INT i ;
    for ( i = 0 ; i < C2_NURB_N(nurb) ; i++ ) {
        C2V_ADDW ( a, C2_NURB_CTPT(nurb)[i][2], C2_NURB_CTPT(nurb)[i], 
            factor, C2_NURB_CTPT(nurb)[i] ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2n_mirror ( nurb, origin, normal ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PT2 origin, normal ;
{
    c2a_mirror_hpt ( C2_NURB_CTPT(nurb), C2_NURB_N(nurb), 
        origin, normal, C2_NURB_CTPT(nurb) ) ;
    C2_NURB_W(nurb) = - C2_NURB_W(nurb) ; 
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2n_rotate_cs ( nurb, origin, c, s ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PT2 origin ;
REAL c, s ;
{
    c2a_rotate_hpt_cs ( C2_NURB_CTPT(nurb), C2_NURB_N(nurb), 
        origin, c, s, C2_NURB_CTPT(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2n_transform ( nurb, t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
C2_TRANSFORM t ;
{
    if ( !IS_SMALL(C2_NURB_W(nurb)) ) {
        if ( !c2t_orthogonal(t) ) 
            RETURN ( FALSE ) ;
        else 
            C2_NURB_W(nurb) *= c2t_det ( t ) ;
    }
    c2a_transform_hpt ( C2_NURB_CTPT(nurb), C2_NURB_N(nurb), 
                t, C2_NURB_CTPT(nurb) ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE HPT2* c2n_get_ctpt ( nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
{
    RETURN ( C2_NURB_CTPT(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2n_get_n ( nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
{
    RETURN ( C2_NURB_N(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL* c2n_get_knot ( nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
{
    RETURN ( C2_NURB_KNOT(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2n_get_d ( nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
{
    RETURN ( C2_NURB_D(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2n_get_w ( nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
{
    RETURN ( C2_NURB_W(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2n_set_w ( nurb, w ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
REAL w ;
{
    C2_NURB_W(nurb) = w ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2n_parm_adjust ( nurb, parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm ;
{
    RETURN ( als_parm_adjust ( C2_NURB_N(nurb), C2_NURB_D(nurb), 
        C2_NURB_KNOT(nurb), parm ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2n_reverse ( nurb, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm0, parm1 ;
{
    c2s_reverse ( C2_NURB_CTPT(nurb), C2_NURB_N(nurb), 
        C2_NURB_D(nurb), C2_NURB_KNOT(nurb), parm0, parm1 ) ;
    C2_NURB_W(nurb) = - C2_NURB_W(nurb) ;
    FREE ( C2_NURB_CURV_EXTR_PTR(nurb) ) ;
    C2_NURB_CURV_EXTR_PTR(nurb) = NULL ;
    C2_NURB_CURV_EXTR_NO(nurb) = -1 ;   /* init. value */
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_NURB c2n_bezier_ctl_pts ( b0, b1, b2, b3 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 b0, b1, b2, b3 ;
{
    C2_NURB nurb ;

    nurb = c2n_create_nurb ( 4, 4 ) ;
    if ( nurb == NULL ) 
        RETURN ( NULL ) ;

    C2V_COPY ( b0, C2_NURB_CTPT(nurb)[0] ) ;
    C2_NURB_CTPT(nurb)[0][2] = 1.0 ;
    C2V_COPY ( b1, C2_NURB_CTPT(nurb)[1] ) ;
    C2_NURB_CTPT(nurb)[1][2] = 1.0 ;
    C2V_COPY ( b2, C2_NURB_CTPT(nurb)[2] ) ;
    C2_NURB_CTPT(nurb)[2][2] = 1.0 ;
    C2V_COPY ( b3, C2_NURB_CTPT(nurb)[3] ) ;
    C2_NURB_CTPT(nurb)[3][2] = 1.0 ;
    
    C2_NURB_KNOT(nurb)[0] = C2_NURB_KNOT(nurb)[1] = 
        C2_NURB_KNOT(nurb)[2] = C2_NURB_KNOT(nurb)[3] = 0.0 ;
    C2_NURB_KNOT(nurb)[4] = C2_NURB_KNOT(nurb)[5] = 
        C2_NURB_KNOT(nurb)[6] = C2_NURB_KNOT(nurb)[7] = 1.0 ;
    C2_NURB_W(nurb) = 0.0 ;
    RETURN ( nurb ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_NURB c2n_bezier_dctl_hpts ( b, d ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *b ;
INT d ;
{
    C2_NURB nurb ;
    INT i ;

    nurb = c2n_create_nurb ( d, d ) ;
    if ( nurb == NULL ) 
        RETURN ( NULL ) ;

    ala_copy ( (REAL*)b, 3*d, (REAL*)C2_NURB_CTPT(nurb) ) ;
    
    for ( i=0 ; i<d ; i++ ) {
        C2_NURB_KNOT(nurb)[i] = 0.0 ;
        C2_NURB_KNOT(nurb)[i+d] = 1.0 ;
    }
    C2_NURB_W(nurb) = 0.0 ;
    RETURN ( nurb ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE INT c2n_offset ( nurb, parm0, parm1, offset, trim_list ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm0, parm1 ;
REAL offset ;
DML_LIST trim_list ;
{
    RETURN ( c2s_offset ( C2_NURB_CTPT(nurb), C2_NURB_N(nurb), 
        C2_NURB_D(nurb), C2_NURB_KNOT(nurb), C2_NURB_W(nurb), 
        C2_NURB_CURV_EXTR_NO(nurb), C2_NURB_CURV_EXTR(nurb), 
        parm0, parm1, offset, trim_list ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2n_init_parms ( nurb, parm0, parm1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm0, parm1 ;
{
    INT n, d ;

    n = C2_NURB_N(nurb) ;
    d = C2_NURB_D(nurb) ;
    if ( parm0 != NULL ) {
        PARM_T(parm0) = C2_NURB_KNOT(nurb)[d-1] ;
        PARM_J(parm0) = d-1 ;
    }
    if ( parm1 != NULL ) {
        PARM_T(parm1) = C2_NURB_KNOT(nurb)[n] ;
        PARM_J(parm1) = n-1 ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2n_trimmed ( nurb, parm0, parm1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm0, parm1 ;
{
    RETURN ( c2n_trimmed0 ( nurb, parm0 ) && c2n_trimmed1 ( nurb, parm1 ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2n_trimmed0 ( nurb, parm0 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm0 ;
{
    RETURN ( PARM_T(parm0) > 
        C2_NURB_KNOT(nurb)[C2_NURB_D(nurb)-1] + BBS_ZERO ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2n_trimmed1 ( nurb, parm1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm1 ;
{
    RETURN ( PARM_T(parm1) < 
            C2_NURB_KNOT(nurb)[C2_NURB_N(nurb)] - BBS_ZERO ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2n_interp_tan ( a, n, knot_options, 
                tan0, tan0_options, tan1, tan1_options, nurb ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a      ;   /* a[n-2] */
INT n ;
C2_KNOT_OPTIONS knot_options ;
PT2 tan0    ;
C2_TAN_OPTIONS tan0_options ;
PT2 tan1    ;
C2_TAN_OPTIONS tan1_options ;
C2_NURB nurb ;
{
    PT2 *b ;

    b = CREATE ( n+2, PT2 ) ;
    if ( b == NULL ) 
        RETURN ( FALSE ) ;

    if ( !c2s_interp_tan ( a, n+2, knot_options, tan0, tan0_options, 
            tan1, tan1_options, b, C2_NURB_KNOT(nurb) ) ) 
        RETURN ( FALSE ) ;
    c2n_set_ctpts ( b, nurb ) ;
    KILL ( b ) ;
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2n_interp_clsd ( a, n, knot_options, nurb ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a      ;   /* a[n-3] */
INT n ;
C2_KNOT_OPTIONS knot_options ;
C2_NURB nurb ;
{
    PT2 *b ;

    b = CREATE ( n+3, PT2 ) ;
    if ( b == NULL ) 
        RETURN ( FALSE ) ;

    if ( !c2s_interp_clsd ( a, n+3, knot_options, b, 
        C2_NURB_KNOT(nurb) ) ) 
        RETURN ( FALSE ) ;
    c2n_set_ctpts ( b, nurb ) ;
    KILL ( b ) ;
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2n_eval ( nurb, parm, p, x ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm ;
INT p ;
PT2 *x ;
{
    RETURN ( c2s_eval ( C2_NURB_CTPT(nurb), C2_NURB_N(nurb), C2_NURB_D(nurb), 
        C2_NURB_KNOT(nurb), C2_NURB_W(nurb), parm, p, x ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2n_pt_tan ( nurb, parm, pt, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm ;
PT2 pt ;
PT2 tan_vec ;
{
    RETURN ( c2s_pt_tan ( C2_NURB_CTPT(nurb), C2_NURB_N(nurb), 
        C2_NURB_D(nurb), C2_NURB_KNOT(nurb), C2_NURB_W(nurb), 
        parm, pt, tan_vec ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2n_curvature ( nurb, parm, curv_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm ;
REAL* curv_ptr ;
{
    RETURN ( c2s_curvature ( C2_NURB_CTPT(nurb), C2_NURB_D(nurb),
        C2_NURB_KNOT(nurb), C2_NURB_W(nurb), parm, curv_ptr ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2n_project ( nurb, parm0, parm1, pt, parm, proj_pt )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm0, parm1 ;
PT2 pt ;
PARM parm ;
PT2 proj_pt ;
{
    RETURN ( c2s_project ( C2_NURB_CTPT(nurb), C2_NURB_D(nurb), 
        C2_NURB_KNOT(nurb), C2_NURB_W(nurb), parm0, parm1, 
        pt, parm, proj_pt ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2n_mass_prop ( nurb, parm0, parm1, p, dim, tol, 
        prop_function, result ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM    parm0, parm1 ;
INT     p ;
INT     dim ;
REAL    tol ;
PF_PROP2 prop_function ;
REAL    *result ;    /* result[dim] */
{
    c2s_mass_prop ( C2_NURB_CTPT(nurb), C2_NURB_D(nurb), 
        C2_NURB_KNOT(nurb), C2_NURB_W(nurb), parm0, parm1, p, dim, 
        tol, prop_function, result ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2n_select ( nurb, parm0, parm1, 
            pt, tol, sel_parm, dist_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm0, parm1 ;
PT2 pt ;
REAL tol ;
PARM sel_parm ;
REAL *dist_ptr ;
{
    RETURN ( c2s_select ( C2_NURB_CTPT(nurb), C2_NURB_D(nurb), 
            C2_NURB_KNOT(nurb), C2_NURB_W(nurb), parm0, parm1, 
            pt, tol, sel_parm, dist_ptr ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2n_coord_extrs ( nurb, parm0, parm1, coord, extr_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB    nurb ;
PARM parm0, parm1 ;
INT         coord ;
DML_LIST    extr_list ;
{
    RETURN ( c2s_coord_extrs ( C2_NURB_CTPT(nurb), C2_NURB_D(nurb), 
            C2_NURB_KNOT(nurb), parm0, parm1, coord, extr_list ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_NURB c2n_ellipse ( ctr, major_axis, minor_axis, angle ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr ;
REAL major_axis, minor_axis, angle ;
{
    C2_NURB nurb ;

    nurb = c2n_create_nurb ( 9, 3 ) ;
    if ( nurb == NULL ) 
        RETURN ( NULL ) ;
    c2s_ellipse ( ctr, major_axis, minor_axis, angle, 
        C2_NURB_CTPT(nurb), C2_NURB_KNOT(nurb) ) ;
    RETURN ( nurb ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE REAL c2n_ellipse_t ( ellipse, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB ellipse ;
PARM parm ;
{
    PT2 ctr, pt, vec ;
    REAL major_axis, minor_axis, angle, cosa, sina, cost, sint ;

    c2n_get_ellipse_data ( ellipse, ctr, &major_axis, &minor_axis, &angle ) ;
    c2n_pt_tan ( ellipse, parm, pt, NULL ) ;
    C2V_SUB ( pt, ctr, vec ) ;
    cosa = (REAL)cos ( (double)angle ) ;
    sina = (REAL)sin ( (double)angle ) ;
    cost = ( vec[0] * cosa + vec[1] * sina ) / major_axis ;
    sint = ( vec[1] * cosa - vec[0] * sina ) / minor_axis ;
    RETURN ( bbs_atan2 ( sint, cost ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE HPT2* c2n_clsd_ctlpts ( nurb ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
{
    RETURN ( c2s_clsd_ctlpts ( C2_NURB_CTPT(nurb), C2_NURB_N(nurb), 
        C2_NURB_D(nurb), C2_NURB_KNOT(nurb) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2n_convert_clsd ( nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
{
    INT i, d, n ;
    PARM_S parm0, parm1 ;

    n = C2_NURB_N(nurb) ;
    d = C2_NURB_D(nurb) ;

    for ( i = 0 ; i < d - 1 ; i++ ) 
    {
        C2H_COPY ( C2_NURB_CTPT(nurb)[i], C2_NURB_CTPT(nurb)[n-d+1+i] ) ;
    }
    for ( i = d - 2 ; i >= 0 ; i-- ) 
    {
        C2_NURB_KNOT(nurb)[i] = C2_NURB_KNOT(nurb)[i+1] - 1.0 ;
    }
    for ( i = n + 1 ; i < n + d ; i++ ) 
    {
        C2_NURB_KNOT(nurb)[i] = C2_NURB_KNOT(nurb)[i-1] + 1.0 ;
    }

    PARM_SETJ ( C2_NURB_KNOT(nurb)[d-1], d-1, &parm0 ) ;
    PARM_SETJ ( C2_NURB_KNOT(nurb)[n], n, &parm1 ) ;
    als_trim ( (REAL*)C2_NURB_CTPT(nurb), d, C2_NURB_KNOT(nurb), 3, 
        &parm0, &parm1, (REAL*)C2_NURB_CTPT(nurb), n, C2_NURB_KNOT(nurb) ) ;
}
#endif  /*SPLINE*/

