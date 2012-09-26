/* -2 -3 */
/********************************* ALBR.C **********************************/
/***************** Processing Bernstein-Bezier polynomials *****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef   SPLINE
#include <albdefs.h>
#include <alpdefs.h>
#include <dmldefs.h>

STATIC  INT     root_iter __(( REAL* DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 , REAL DUMMY4 , REAL* DUMMY5 )) ;
STATIC  INT     root_mult __(( REAL* DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 , REAL DUMMY4 , REAL* DUMMY5 )) ;
STATIC  INT     find_roots __(( REAL* DUMMY0 , REAL* DUMMY1 , INT DUMMY2 , 
            REAL DUMMY3 , INT DUMMY4 , REAL DUMMY5 , INT DUMMY6 , 
            DML_LIST DUMMY7 )) ;
STATIC  BOOLEAN append_root_interval __(( DML_LIST DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 )) ;
STATIC  BOOLEAN append_root_rec __(( DML_LIST DUMMY0 , REAL DUMMY1 , 
            INT DUMMY2 )) ;
STATIC  void    check_zero_interval __(( REAL* DUMMY0 , INT DUMMY1 , 
            DML_ITEM DUMMY2 )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT alb_roots ( b, d, roots_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *b  ;
INT d ;
DML_LIST roots_list ;
{
    INT n, i, m0, m1 ;
    REAL t[2] ;
    INT mult[2] ;
    DML_ITEM item ;

    if ( d<=3 ) {

        if ( d==2 ) 
            n = alp_solve2 ( b[1]-b[0], b[0], t, mult ) ;
        else if ( d==3 ) 
            n = alp_solve_bez3 ( b[0], b[1], b[2], t, mult ) ;
        else
            n = 0 ;
        for ( i=0 ; i<n ; i++ ) {
            if ( t[i] >= - BBS_ZERO && t[i] <= 1.0 + BBS_ZERO ) 
                (void) append_root_rec ( roots_list, t[i], mult[i] ) ;
        }
        RETURN ( n ) ;
    }
    for ( m0 = 0 ; m0<d && IS_SMALL(b[m0]) ; m0++ ) ;
    for ( m1 = d-1 ; m1>=0 && IS_SMALL(b[m1]) ; m1-- ) ;
    m1 = d-m1-1 ;
    if ( m0 >= d ) {
        n = 1 ;
        (void) append_root_interval ( roots_list, 0.0, 1.0 ) ;
    }
    else {
        n = 0 ;
        if ( m0 && append_root_rec ( roots_list, 0.0, m0 ) )
            n++ ;
        n += find_roots ( b, b, d, 0.0, m0, 1.0, m1, roots_list ) ;
        if ( m1 && append_root_rec ( roots_list, 1.0, m1 ) )
            n++ ;

        DML_WALK_LIST ( roots_list, item ) 
            if ( AL_ROOT_REC_MULT((AL_ROOT_REC)DML_RECORD(item)) < 0 ) 
                check_zero_interval ( b, d, item ) ;

    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT alb_root_iter ( a, d, t_init, t_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *a      ;   /* Bezier coefficients */
INT     d       ;   /* Order */
REAL    t_init  ;   /* Initial parameter value */
REAL    *t_ptr  ;   /* Root */
{
    RETURN ( root_iter ( a, d, t_init, 0.0, 1.0, t_ptr ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC  INT find_roots ( a, b, d, t0, m0, t1, m1, roots_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *a  ;
REAL *b  ;
INT d ;
REAL t0, t1 ;
INT m0, m1 ;
DML_LIST roots_list ;
{
    INT n, nl, n0, nr, i, i_max=0, m ;
    REAL *bl, *br, t_init, t, delt, delt_max, u ;

    if ( m0 + m1 >= d ) {
        if ( append_root_interval ( roots_list, t0, t1 ) )
            RETURN ( 1 ) ;
        else 
            RETURN ( 0 ) ;
    }
    n = 0 ;
    delt_max = 0.0 ;
    for ( i=m0+1 ; i<d-m1 ; i++ ) {
        if ( ( b[i-1] < 0.0 && b[i] > 0.0 ) || 
             ( b[i-1] > 0.0 && b[i] < 0.0 ) ) {
            delt = fabs ( b[i] - b[i-1] ) ;
            if ( delt_max < delt ) {
                delt_max = delt ;
                i_max = i ;
            }
            n++ ;
        }
    }
    if ( n==0 ) 
        RETURN ( 0 ) ;
    
    t_init = t0 + ( t1 - t0 ) * ( (REAL)i_max - 0.5 ) / ( (REAL)(d-1) ) ;
    if ( n==1 || t1 - t0 > 1e-3 ) 
        m = root_iter ( a, d, t_init, t0, t1, &t ) ;
    else 
        m = root_mult ( a, d, t_init, t0, t1, &t ) ;
    if ( n==1 && m ) {
        if ( append_root_rec ( roots_list, t, m ) )
            RETURN ( 1 ) ;
        else 
            RETURN ( 0 ) ;
    }
    else if ( m == 0 )
        t = 0.5 * ( t0 + t1 ) ;
    u = ( t - t0 ) / ( t1 - t0 ) ;
    
    bl = CREATE ( 2*d-1, REAL ) ;
    if ( bl == NULL )
        RETURN ( -1 ) ;
    br = bl + (d-1) ;
    alb_brk0 ( b, d, 1, u, bl ) ;

    /* Find if the partition parameter happens to be a root */
/*
    if ( !m ) 
        for ( m = 0 ; m<d && IS_SMALL(br[m]) ; m++ ) ;
*/
    nl = find_roots ( a, bl, d, t0, m0, t, m, roots_list ) ;
    n0 = ( m && append_root_rec ( roots_list, t, m ) ? 1 : 0 ) ;
    nr = find_roots ( a, br, d, t, m, t1, m1, roots_list ) ;
    KILL ( bl ) ;
    RETURN ( nl + n0 + nr ) ;

}


/*-------------------------------------------------------------------------*/
STATIC INT root_iter ( a, d, t_init, t0, t1, t_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *a      ;   /* Bezier */
INT     d       ;   /* Order */
REAL    t_init  ;   /* Initial parameter value */
REAL    t0      ;   /* Left end of the interval */
REAL    t1      ;   /* Right end of the interval */
REAL    *t_ptr  ;   /* Root */
{
    REAL    dt ;
    REAL   x[2], x_prev=1e4*BBS_TOL ;
    INT     count ;

    *t_ptr = t_init ;
    dt = 1.0 ;

    for ( count=0 ; count<MAX_COUNT ; count++ ) {
        alb_eval ( a, d, 1, *t_ptr, 1, x ) ;
        if ( count>0 && fabs(x[0]) > fabs(x_prev) ) {
            if ( IS_SMALL(x_prev) ) {
                *t_ptr += dt ;
                RETURN ( 1 ) ;
            }
            else 
                RETURN ( 0 ) ;
        }
        x_prev = x[0] ;

        if ( IS_SMALL ( x[1] ) ) {
            if ( IS_SMALL ( x[0] ) ) 
                RETURN ( root_mult ( a, d, *t_ptr, t0, t1, t_ptr ) ) ;
            else 
                RETURN ( 0 ) ;
        }

        dt = - (x[0]) / (x[1]) ;
        *t_ptr += dt ;
        if ( *t_ptr > t1 || *t_ptr < t0 ) {
            *t_ptr = 0.5*(t0+t1) ;
            RETURN ( 0 ) ;
        }

        if ( IS_ZERO(dt) ) 
            RETURN ( 1 ) ;
    }
    RETURN ( 0 ) ;
}        


/*-------------------------------------------------------------------------*/
STATIC INT root_mult ( a, d, t_init, t0, t1, t_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *a      ;   /* Bezier */
INT     d       ;   /* Order */
REAL    t_init  ;   /* Initial parameter value */
REAL    t0      ;   /* Left end of the interval */
REAL    t1      ;   /* Right end of the interval */
REAL    *t_ptr  ;   /* Root */
{
    REAL    x[3], dt, h0, h1, r, r0 ;
    INT     count, mult = (-1) ;

    *t_ptr = t_init ;
    r0 = -1.0 ;
    dt = 1.0 ;

    for ( count=0 ; count<MAX_COUNT ; count++ ) {
        alb_eval ( a, d, 1, *t_ptr, 2, x ) ;
        if ( IS_ZERO(x[1]) || IS_ZERO(x[2]) ) {
            if ( IS_ZERO(x[0]) )
                RETURN ( mult ) ;
            else 
                RETURN ( -1 ) ;
        }
        h0 = (x[0]) / ( x[1]) ;
        h1 = (x[1]) / ( x[2]) ;
        r = 1.0 / ( 1.0 - h0/h1 ) ;
        if ( fabs(dt) <= 1e-8 && fabs(r-r0) >= 0.2 ) 
            RETURN ( IS_SMALL(x[0]) ? mult : 0 ) ;
        r0 = r ;
        mult = (INT) ( r + 0.5 ) ;
        dt = - h0 * r ;
        *t_ptr += dt ;
        if ( *t_ptr > t1 ) 
            *t_ptr = t1 ;
        else if ( *t_ptr < t0 ) 
            *t_ptr = t0 ;
        if ( IS_ZERO(dt) ) 
            RETURN ( mult ) ;
    }
    RETURN ( 0 ) ;
}        


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN append_root_rec ( rlist, tr, multr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST rlist ;
REAL tr ;
INT  multr ;
{
    AL_ROOT_REC root_rec ;

    if ( DML_LAST(rlist) != NULL ) { 
        root_rec = (AL_ROOT_REC)DML_LAST_RECORD(rlist) ;
        if ( AL_ROOT_REC_MULT(root_rec) == -1 && 
            IS_ZERO ( tr - AL_ROOT_REC_T1(root_rec) ) ) 
            RETURN ( FALSE ) ; /* A new record does not need to be created */
        if ( AL_ROOT_REC_MULT(root_rec) > 0 && 
            IS_ZERO ( tr - AL_ROOT_REC_T(root_rec) ) ) 
            RETURN ( FALSE ) ; /* A new record does not need to be created */
    }
    root_rec = MALLOC ( 1, AL_ROOT_REC_S ) ;
    AL_ROOT_REC_T(root_rec) = tr ; 
    AL_ROOT_REC_MULT(root_rec) = multr ;
    dml_append_data ( rlist, root_rec ) ; 
    RETURN ( TRUE ) ;   /* A new record has been created */
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN append_root_interval ( rlist, tr, t1r ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST rlist ;
REAL tr, t1r ;
{
    AL_ROOT_REC root_rec ;

    if ( DML_LAST(rlist) != NULL ) {
        root_rec = (AL_ROOT_REC)DML_LAST_RECORD(rlist) ;
        if ( ( AL_ROOT_REC_MULT(root_rec) > 0 && 
            IS_ZERO ( tr - AL_ROOT_REC_T(root_rec) ) ) || 
            ( AL_ROOT_REC_MULT(root_rec) == -1 && 
            IS_ZERO ( tr - AL_ROOT_REC_T1(root_rec) ) ) ) {

            AL_ROOT_REC_T1(root_rec) = t1r ; 
            AL_ROOT_REC_MULT(root_rec) = -1 ;
            RETURN ( FALSE ) ;   /* A new record has not been created */
        }
    }

    root_rec =  MALLOC ( 1, AL_ROOT_REC_S ) ;
    if ( root_rec == NULL )
        RETURN ( FALSE ) ;
    AL_ROOT_REC_T(root_rec) = tr ; 
    AL_ROOT_REC_T1(root_rec) = t1r ; 
    AL_ROOT_REC_MULT(root_rec) = -1 ;
    dml_append_data ( rlist, root_rec ) ; 
    RETURN ( TRUE ) ;   /* A new record has been created */
}


/*-------------------------------------------------------------------------*/
STATIC void check_zero_interval ( b, d, item ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *b  ;
INT  d ;
DML_ITEM item ;
{
    REAL t, x0, x1 ;
    if ( DML_PREV(item) == NULL ) 
        x0 = b[0] ;
    else {
        t = 0.5 * ( AL_ROOT_REC_T((AL_ROOT_REC)DML_RECORD(item)) + 
            ( AL_ROOT_REC_MULT((AL_ROOT_REC)DML_RECORD(DML_PREV(item))) > 0 ? 
                AL_ROOT_REC_T((AL_ROOT_REC)DML_RECORD(DML_PREV(item))) :
                AL_ROOT_REC_T1((AL_ROOT_REC)DML_RECORD(DML_PREV(item))) ) ) ;
        alb_eval ( b, d, 1, t, 0, &x0 ) ;
    }
    if ( DML_NEXT(item) == NULL ) 
        x0 = b[d-1] ;
    else {
        t = 0.5 * ( AL_ROOT_REC_T1((AL_ROOT_REC)DML_RECORD(item)) + 
                    AL_ROOT_REC_T((AL_ROOT_REC)DML_RECORD(DML_NEXT(item))) ) ;
        alb_eval ( b, d, 1, t, 0, &x1 ) ;
    }
    if ( ( x0 > 0.0 ) == ( x1 > 0.0 ) ) 
        AL_ROOT_REC_MULT((AL_ROOT_REC)DML_RECORD(item)) = -2 ;
}
#endif   /*SPLINE*/

