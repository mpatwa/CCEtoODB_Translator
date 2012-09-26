/* -2 -3 */
/******************************* FNB.C *************************************/
/*********** fnb routines - rational Bernstein-Bezier polynomials **********/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <albdefs.h>
#include <dmldefs.h>
#include <fnadefs.h>
#include <fnbdefs.h>
#ifdef  SPLINE
STATIC  BOOLEAN extr_find __(( HREAL* DUMMY0 , INT DUMMY1 , HREAL* DUMMY2 , 
            BOOLEAN DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , REAL* DUMMY6 , 
            REAL*  DUMMY7 )) ;
STATIC  void    extrs __(( HREAL* DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 , INT DUMMY4 , DML_LIST  DUMMY5 )) ;
STATIC  void    append_extr_rec __(( DML_LIST DUMMY0 , REAL DUMMY1 , 
            INT DUMMY2 , REAL DUMMY3 , INT  DUMMY4 )) ;

#define GET_F(B,F,S)    {\
if ( (B)[1] > BBS_ZERO ) (F) = (B)[0] / (B)[1] ;\
else { (F) = 0.0 ; (S) *= 2 ; }}
        
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN fnb_extr ( b, d, find_max, t_ptr, f_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HREAL   *b      ;   /* Array of Bernstein-Bezier coefficients */
INT     d       ;   /* Order */
BOOLEAN find_max;   /* TRUE, if max, FALSE if min */
REAL    *t_ptr  ;   /* Extremum parameter value */
REAL    *f_ptr  ;   /* Extremal polynomial value */
{
    INT status ;
    REAL f0, f1, f ;

    f0 = b[0][0]/b[0][1] ;
    f1 = b[d-1][0]/b[d-1][1] ;
    if ( find_max == ( f0 > f1 ) ) {
        *t_ptr = 0.0 ;
        f = f0 ;
    }
    else {
        *t_ptr = 1.0 ;
        f = f1 ;
    }
    status = extr_find ( b, d, b, find_max, 0.0, 1.0, t_ptr, &f ) ;
    if ( f_ptr != NULL )
        *f_ptr = f ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN extr_find ( b, d, a, find_max, u, v, t_ptr, f_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HREAL   *b      ;   /* Array of Bernstein-Bezier coefficients */
INT     d       ;   /* Order */
HREAL   *a      ;   /* Evaluation matrix */
BOOLEAN find_max;   /* TRUE, if max, FALSE if min */
REAL    u, v    ;   /* Lower and upper bounds of the interval */
REAL    *t_ptr  ;   /* Extremum parameter value */
REAL    *f_ptr  ;   /* Extremal polynomial value */
{
    INT i_extr ;
    REAL f_extr, h, t, t0 ;
    HREAL *bl, *br ;
    BOOLEAN status ;

    if ( find_max ) {
        fna_max ( b, d, &f_extr, &i_extr ) ;
        if ( f_extr <= *f_ptr + BBS_TOL ) 
            RETURN ( TRUE ) ;
    }
    else {
        fna_min ( b, d, &f_extr, &i_extr ) ;
        if ( f_extr >= *f_ptr - BBS_TOL ) 
            RETURN ( TRUE ) ;
    }

    h =  ( v - u ) / ( (REAL) (d-1) ) ;
    t0 = u + h * (REAL) i_extr ;
    if ( !fnb_extr_init ( a, d, t0, &t, NULL ) ) 
        t = t0 ;
    if ( t <= u + h )
        t = u + h ;
    else if ( t >= v - h )
        t = v - h ;
    t0 = ( t - u ) / ( v - u ) ;
    bl = CREATE ( 2*d-1, HREAL ) ;
    if ( bl == NULL )
        RETURN ( FALSE ) ;
    br = bl + d-1 ;
    alb_brk0 ( (REAL*)b, d, 2, t0, (REAL*)bl ) ;
    f_extr = br[0][0] / br[0][1] ;
    if ( find_max == ( f_extr > *f_ptr ) ) {
        *f_ptr = f_extr ;
        *t_ptr = t ;
    }
    status = extr_find ( bl, d, a, find_max, u, t, t_ptr, f_ptr ) && 
             extr_find ( br, d, a, find_max, t, v, t_ptr, f_ptr ) ;
    KILL ( bl ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN fnb_extr_init ( bez, d, t0, t_ptr, f_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HREAL   *bez      ;   /* Bezier */
INT     d       ;   /* Order */
REAL    t0      ;   /* Initial parameter value */
REAL    *t_ptr  ;   /* Extremum parameter value */
REAL    *f_ptr  ;   /* Extremal polynomial value, may be NULL */
{
    REAL    dt, a, b ;
    HREAL   x[3] ;
    INT     count ;

    *t_ptr = t0 ;
    for ( count=0 ; count<MAX_COUNT ; count++ ) {
        alb_eval ( (REAL*)bez, d, 2, *t_ptr, 2, (REAL*)x ) ;
        a = x[1][0]*x[0][1] - x[0][0]*x[1][1] ;
        b = x[2][0]*x[0][1] - x[0][0]*x[2][1] ;
        dt = - a / b ;
        *t_ptr += dt ;
        if ( *t_ptr > 1.0 ) {
            *t_ptr = 1.0 ;
            RETURN ( FALSE ) ;
        }
        if ( *t_ptr < 0.0 ) {
            *t_ptr = 0.0 ;
            RETURN ( FALSE ) ;
        }
        if ( IS_CONVERGED(dt) ) {
            if ( f_ptr != NULL ) {
                alb_eval ( (REAL*)bez, d, 2, *t_ptr, 0, (REAL*)x ) ;
                *f_ptr = x[0][0]/x[0][1] ;
            }
            RETURN ( TRUE ) ;
        }
    }
    RETURN ( FALSE ) ;
}        


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void fnb_extrs ( b, d, t0, t1, j, extr_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HREAL   *b      ;   /* Array of Bernstein-Bezier coefficients */
INT     d       ;   /* Order */
REAL    t0, t1  ;
INT     j       ;
DML_LIST extr_list ;    /* List of the extremum records */
{
    REAL f ;
    INT i, status ;

    status = 0 ;
    for ( i=1 ; i<d && !status ; i++ ) {
        f = b[i][0] * b[0][1] - b[0][0] * b[i][1] ;
        if ( f > BBS_ZERO ) 
            status = 1 ; 
        else if ( f < - BBS_ZERO ) 
            status = - 1 ; 
    }
    GET_F ( b[0], f, status ) ;
    
    if ( !status ) {        /* flat piece */
        append_extr_rec ( extr_list, t0, j, f, status ) ;
        append_extr_rec ( extr_list, t1, j, f, status ) ;
        RETURN ;
    }
    else if ( fabs ( b[1][0] * b[0][1] - b[0][0] * b[1][1] ) <= 
        BBS_ZERO * b[0][1] * b[1][1] ) 
        append_extr_rec ( extr_list, t0, j, f, status ) ;
    
    extrs ( b, d, t0, t1, j, extr_list ) ;
    
    status = 0 ;
    for ( i=d-2 ; i>=0 && !status ; i-- ) {
        f = b[d-1][0] * b[i][1] - b[i][0] * b[d-1][1] ;
        if ( f > BBS_ZERO ) 
            status = 1 ; 
        else if ( f < - BBS_ZERO ) 
            status = - 1 ; 
    }
    GET_F ( b[d-1], f, status ) ;

    if ( fabs ( b[d-2][0] * b[d-1][1] - b[d-1][0] * b[d-2][1] ) <= 
        BBS_ZERO * b[d-1][1] * b[d-2][1] ) 
        append_extr_rec ( extr_list, t1, j, f, status ) ;
}

/*-------------------------------------------------------------------------*/
STATIC void extrs ( b, d, u, v, j, extr_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HREAL   *b      ;   /* Array of Bernstein-Bezier coefficients ( local ) */
INT     d       ;   /* Order */
REAL    u, v    ;   /* Bounds of the interval */ 
INT     j       ;   /* Index */
DML_LIST extr_list ;    /* List of the extremum records */
{
    REAL f_min, f_max, f_extr, t0, t ;
    INT i_extr, i_min, i_max, extr_type ;
    BOOLEAN status ;
    HREAL *bl, *br ;

    if ( !fna_extr ( (REAL*)b, d, 1, 0, &f_max, &i_max, &f_min, &i_min ) )
        RETURN ;    /* No extremums over the interval */
    
    if ( v - u <= BBS_TOL || f_max - f_min <= BBS_TOL ) { 
            /* constant or small intl */
        f_extr = b[0][0] / b[0][1] ;
        append_extr_rec ( extr_list, u, j, f_extr, 0 ) ;
        append_extr_rec ( extr_list, v, j, f_extr, 0 ) ;
        RETURN ;
    }
        
    i_extr = abs ( d-1-2*i_min ) < abs ( d-1-2*i_max) ? i_min : i_max ;
    /* An extremal index closest to the middle of the interval */
    t0 = ( (REAL) i_extr ) / ( (REAL) (d-1) ) ;
    status = fnb_extr_init ( b, d, t0, &t, &f_extr ) ;

    if ( t <= BBS_ZERO || t >= 1.0 - BBS_ZERO ) {
        if ( !status ) 
            t = 0.5 ;
        status = FALSE ;
    }
    bl = CREATE ( 2*d-1, HREAL ) ;
    if ( bl == NULL )
        RETURN ;
    br = bl + d-1 ;
    alb_brk0 ( (REAL*)b, d, 2, t, (REAL*)bl ) ;

    t = u + t * ( v - u ) ;

    extrs ( bl, d, u, t, j, extr_list );
        
    if ( status ) {
        extr_type = ( i_min == i_extr ) ? -1 : 1 ;
        append_extr_rec ( extr_list, t, j, f_extr, extr_type ) ;
    }
    extrs ( br, d, t, v, j, extr_list );
    KILL ( bl ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void append_extr_rec ( xlist, tx, jx, fx, xtype ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST xlist ;
REAL tx ;
INT  jx ;
REAL fx ;
INT  xtype ;
{
    FN_EXTR_REC extr_rec ;
    if ( DML_LAST(xlist) == NULL || 
        FN_EXTR_REC_T ( (FN_EXTR_REC)DML_LAST_RECORD(xlist) ) < tx-BBS_ZERO ) {
        extr_rec = MALLOC ( 1, FN_EXTR_REC_S ) ;
        if ( extr_rec == NULL ) 
            RETURN ;
        FN_EXTR_REC_T(extr_rec) = tx ; 
        FN_EXTR_REC_J(extr_rec) = jx ;
        FN_EXTR_REC_F(extr_rec) = fx ;
        FN_EXTR_REC_TYPE(extr_rec) = xtype ;
        dml_append_data ( xlist, extr_rec ) ; 
    }
    else if ( !xtype ) 
        FN_EXTR_REC_TYPE ( (FN_EXTR_REC)DML_LAST_RECORD(xlist) ) = xtype ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT fnb_coord_break_parm ( b, d, dim, j, f_min, f_max, t_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *b ;
INT     d ;
INT     dim ;
INT     j ;
REAL    *f_min, *f_max ;
REAL    *t_ptr ;
{
    INT i_min, i_max, mon ;
    
    mon = fna_monotonic ( b, d, dim, j ) ;
    if ( !fna_bounds ( b, d, dim, j, mon, f_min, &i_min, f_max, &i_max ) ) {
        *t_ptr = 0.5 ;
        RETURN ( -2 ) ;
    }

    if ( mon ) 
        RETURN ( mon  ) ; /* monotonic */

    /* Find extremum which is closer to the middle of the interval */

    if ( abs(2*i_min-d+1) < abs(2*i_max-d+1) ) 
        *t_ptr = (REAL)i_min / ( (REAL)(d-1) ) ;
    else 
        *t_ptr = (REAL)i_max / ( (REAL)(d-1) ) ;
    if ( *t_ptr < 0.1 ) 
        *t_ptr = 0.1 ; 
    else if ( *t_ptr > 0.9 ) 
        *t_ptr = 0.9 ; 
    RETURN ( mon  ) ; /* monotonic */
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN fnb_select ( b, d, tol, u, v, t_ptr, fmin_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HREAL*  b ;
INT     d ;
REAL    u, v, tol, *t_ptr, *fmin_ptr ;
{
    REAL f_min, t, t0 ;
    INT i_min ;
    HREAL *bl, *br ;

    fna_min ( b, d, &f_min, &i_min ) ;
    if ( f_min > tol ) 
        RETURN ( FALSE ) ;
    if ( i_min==0 ) {
        if ( t_ptr != NULL ) 
            *t_ptr = u ;
        if ( fmin_ptr != NULL ) 
            *fmin_ptr = b[0][0]/b[0][1] ;
        RETURN ( TRUE ) ;
    }
    else if ( i_min==d-1 ) {
        if ( t_ptr != NULL ) 
            *t_ptr = v ;
        if ( fmin_ptr != NULL ) 
            *fmin_ptr = b[d-1][0]/b[d-1][1] ;
        RETURN ( TRUE ) ;
    }
    t0 = (REAL)i_min / (REAL)(d-1) ;
    if ( !fnb_extr_init ( b, d, t0, &t, &f_min ) ) 
        t = t0 ;
    else if ( f_min <= tol ) {
        if ( t_ptr != NULL ) 
            *t_ptr = u + t * ( v - u ) ;
        if ( fmin_ptr != NULL ) 
            *fmin_ptr = f_min ;
        RETURN ( TRUE ) ;
    }
    bl = CREATE ( 2*d-1, HREAL ) ;
    if ( bl == NULL )
        RETURN ( FALSE ) ;
    br = bl + d-1 ;
    alb_brk0 ( (REAL*)b, d, 2, t, (REAL*)bl ) ;
    t = u + t * ( v - u ) ;
    RETURN ( fnb_select ( bl, d, tol, u, t, t_ptr, fmin_ptr ) || 
        fnb_select ( br, d, tol, t, v, t_ptr, fmin_ptr ) ) ;
}

#endif  /*SPLINE*/

