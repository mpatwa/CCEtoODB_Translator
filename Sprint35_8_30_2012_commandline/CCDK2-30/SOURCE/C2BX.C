/* -2 -3 */
/*********************************** C2BX.C ********************************/
/*********************** Two-dimensional Bezier curves *********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef  SPLINE
#include <aladefs.h>
#include <albdefs.h>
#include <c2bdefs.h>
#include <c2edefs.h>
#include <c2ndefs.h>
#include <c2vmcrs.h>
#include <c2coned.h>
#include <c2cxtrd.h>
#include <c2cxtrm.h>
STATIC  BOOLEAN curv_extr_append __(( HPT2*, INT, REAL, REAL, REAL, INT, 
                    DML_LIST )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2b_curvature ( a, d, w, t, curv_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2 *a  ;
INT d ;
REAL w ;
REAL t ;
REAL *curv_ptr ;
{
    HPT2 b[3] ;
    REAL c ;

    alb_eval ( (REAL*)a, d, 3, t, 2, (REAL*)b ) ;   /* evaluate only hom.*/
    if ( !c2e_curvature ( b, curv_ptr ) )           /* coordinates */
        RETURN ( FALSE ) ;
    if ( IS_SMALL(w) ) 
        RETURN ( TRUE ) ;
    /* curvw = curv0 / ( 1 + w * curv0 ) */
    c = 1.0 + w * *curv_ptr ;
    if ( IS_SMALL ( c ) ) 
        RETURN ( FALSE ) ;
    *curv_ptr /= c ;
    RETURN ( TRUE ) ;
}    

    
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2b_curv_extrs ( a, d, curv_extr_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a  ;
INT d ;
DML_LIST curv_extr_list ;
{
    INT s=0 ;
    RETURN ( ( BOOLEAN ) c2b_curv_extrs_rat ( a, d, 0.0, 1.0, 0, &s, curv_extr_list ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2b_curv_extrs_rat ( a, d, t0, t1, j, s_ptr, curv_extr_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a  ;
INT d ;
REAL t0, t1 ;
INT j ;
INT *s_ptr ;
DML_LIST curv_extr_list ;
{
    INT n, i, du, dz, dp, dq, dv ;
    PT2 *z ;
    REAL *u, *v, *w, *u1, *v1, *w1, *p0, *p1, *q0, *q1 ;
    REAL s0, s1 ;
    DML_LIST roots_list ;
    DML_ITEM r_item ;

    dz = 2*d-3 ;
    z = CREATE ( dz, PT2 ) ;
    c2b_deriv ( a, d, z ) ;     /* Derivative */

    w = CREATE ( d, REAL ) ;
    for ( i=0 ; i<d ; i++ )
        w[i] = a[i][2] ;        /* Denominator */

    du = 2 * dz - 1 ;
    u = CREATE ( du, REAL ) ;
    c2b_dot_nonrat ( z, dz, z, dz, u ) ;   /* u = z dot z */

    dv = 2*dz - 3 ;
    v = CREATE ( dv, REAL ) ;
    c2b_cross_deriv ( z, dz, v ) ;          /* v = z cross z' */
    
    KILL ( z ) ;

    dp = dv + d - 1 ;
    dq = dp + du - 2 ;
    
    p0 = CREATE ( dp, REAL ) ;
    alb_mult ( v, dv, w, d, p0 ) ;          /* p0 = vùw */
    u1 = CREATE ( du-1, REAL ) ;
    ala_diff ( u, du, u1 ) ;                /* u1 = u'/(du-1) */
    q0 = CREATE ( dq, REAL ) ;
    alb_mult ( u1, du-1, p0, dp, q0 ) ;     /* q0 = u'ùvùw/(du-1) */
    KILL ( u1 ) ;
    
    dp = dv + d - 2 ;
    p1 = CREATE ( dp, REAL ) ;
    v1 = CREATE ( dv-1, REAL ) ;

    ala_diff ( v, dv, v1 ) ;                /* v1 = v'/(dv-1) */
    alb_mult ( v1, dv-1, w, d, p0 ) ;       /* p0 = v'ùw/(dv-1) */
    KILL ( v1 ) ;
    w1 = CREATE ( d-1, REAL ) ;
    ala_diff ( w, d, w1 ) ;                 /* w1 = w'/(d-1) */
    KILL ( w ) ;

    alb_mult ( v, dv, w1, d-1, p1 ) ;       /* p1 = vùw'/(d-1) */
    KILL ( v ) ;
    KILL ( w1 ) ;
    
    s0 = (REAL)(dv-1) ; 
    s1 = 2.0 * (REAL)(d-1) ;
    for ( i=0 ; i<dp ; i++ )
        p0[i] = s0 * p0[i] + s1 * p1[i] ;   
        /* p0 = (dv-1)v'w/(dv-1) + 2ù(d-1)vùw'/(d-1) = v'w + 2vw' */

    KILL ( p1 ) ;
    
    q1 = CREATE ( dq, REAL ) ;
    alb_mult ( p0, dp, u, du, q1 ) ;        /* q1 = ( v'w+2vw' ) * u */

    KILL ( p0 ) ;
    KILL ( u ) ;
    
    s0 = 1.5 * (REAL) (du-1) ;

    for ( i=0 ; i < dq ; i++ )
        q1[i] -= s0 * q0[i] ;   /* q1 = ( v'w+2vw' ) * u - 1.5 u'ùvùw */
    
    KILL ( q0 ) ;
    roots_list = dml_create_list ();
    n = alb_roots ( q1, dq, roots_list ) ;
    
    if ( ( *s_ptr<0 && q1[0]>0.0 ) || ( *s_ptr>0 && q1[0]<0.0 ) ) {
        if ( curv_extr_append ( a, d, 0.0, t0, t1, j, curv_extr_list ) )
            n++ ;
    }

    if ( q1[dq-1] > 0.0 ) 
        *s_ptr = 1 ;
    else if ( q1[dq-1] < 0.0 ) 
        *s_ptr = -1 ;
    else
        *s_ptr = 0 ;
    KILL ( q1 ) ;

    DML_WALK_LIST ( roots_list, r_item ) {
        if ( !curv_extr_append ( a, d, 
            AL_ROOT_REC_T((AL_ROOT_REC)DML_RECORD(r_item)),
            t0, t1, j, curv_extr_list ) ) 
            n-- ;
    }
    DML_WALK_LIST ( roots_list, r_item ) 
        FREE ( DML_RECORD(r_item) ) ;
    dml_free_list ( roots_list ) ;
    RETURN ( (BOOLEAN) n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2b_curv_extrs_nonrat ( a, d, t0, t1, j, 
            s_ptr, curv_extr_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a  ;
INT d ;
REAL t0, t1 ;
INT j ;
INT *s_ptr ;
DML_LIST curv_extr_list ;
{
    INT n, i, du, dz, dq, dv ;
    PT2 *z ;
    REAL *u, *v, *u1, *v1, *q0, *q1 ;
    REAL s ;
    DML_LIST roots_list ;
    DML_ITEM r_item ;

    dz = d-1 ;
    z = CREATE ( dz, PT2 ) ;
    for ( i=0 ; i<d-1 ; i++ )      /* Derivative */
        C2V_SUB ( a[i+1], a[i], z[i] ) ;

    du = 2 * dz - 1 ;
    u = CREATE ( du, REAL ) ;
    c2b_dot_nonrat ( z, dz, z, dz, u ) ;   /* u = z dot z */
    
    dv = 2*dz - 3 ;
    v = CREATE ( dv, REAL ) ;
    c2b_cross_deriv ( z, dz, v ) ;          /* v = z cross z' */
    
    KILL ( z ) ;

    /********* v' ù u - 1.5 v ù u' = 0 *************/

    dq = dv + du - 2 ;

    u1 = CREATE ( du-1, REAL ) ;
    ala_diff ( u, du, u1 ) ;                /* u1 = u'/(du-1) */
    q0 = CREATE ( dq, REAL ) ;
    alb_mult ( u1, du-1, v, dv, q0 ) ;     /* q0 = u'ùv/(du-1) */
    KILL ( u1 ) ;

    v1 = CREATE ( dv-1, REAL ) ;
    ala_diff ( v, dv, v1 ) ;                /* v1 = v'/(dv-1) */
    q1 = CREATE ( dq, REAL ) ;
    alb_mult ( u, du, v1, dv-1, q1 ) ;     /* q1 = uùv'/(du-1) */
    KILL ( v1 ) ;
    KILL ( u ) ;
    KILL ( v ) ;
    
    s = 1.5 * (REAL) (du-1) / (REAL)(dv-1) ; 

    for ( i=0 ; i < dq ; i++ )
        q1[i] -= s * q0[i] ;   /* q1 = v'* u - 1.5 u'ùv */
    
    KILL ( q0 ) ;
    roots_list = dml_create_list ();
    n = alb_roots ( q1, dq, roots_list ) ;
    
    if ( ( *s_ptr<0 && q1[0]>0.0 ) || ( *s_ptr>0 && q1[0]<0.0 ) ) {
        if ( curv_extr_append ( a, d, 0.0, t0, t1, j, curv_extr_list ) )
            n++ ;
    }

    if ( q1[dq-1] > 0.0 ) 
        *s_ptr = 1 ;
    else if ( q1[dq-1] < 0.0 )
        *s_ptr = -1 ;
    else
        *s_ptr = 0 ;
    KILL ( q1 ) ;

    DML_WALK_LIST ( roots_list, r_item ) {
        if ( !curv_extr_append ( a, d, 
            AL_ROOT_REC_T((AL_ROOT_REC)DML_RECORD(r_item)), 
            t0, t1, j, curv_extr_list ) ) 
            n-- ;
    }
    DML_WALK_LIST ( roots_list, r_item ) 
        FREE ( DML_RECORD(r_item) ) ;
    dml_free_list ( roots_list ) ;
    RETURN ( ( BOOLEAN ) n ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN curv_extr_append ( a, d, u, t0, t1, j, curv_extr_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a  ;
INT d ;
REAL u, t0, t1 ;
INT j ;
DML_LIST curv_extr_list ;
{
    REAL t, curvature ;
    C2_CURV_EXTR curv_extr_record ;

    t = t0 + ( t1 - t0 ) * u ;
    if ( ( DML_LAST(curv_extr_list) == NULL ||
        !IS_ZERO ( t - 
            C2_CURV_EXTR_T((C2_CURV_EXTR)DML_LAST_RECORD(curv_extr_list)) )) 
            && c2b_curvature ( a, d, 0.0, u, &curvature ) ) {
        curv_extr_record = c2n_create_curv_extr ( t, j, curvature ) ;
    dml_append_data ( curv_extr_list, curv_extr_record ) ;
        RETURN ( TRUE ) ;
    }
    RETURN ( FALSE ) ;
}
#endif  /*SPLINE*/

