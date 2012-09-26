/* -2 -3 */
/********************************** ALS.C **********************************/
/******************* Dimension independent splines *************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#include <aladefs.h>
#include <alpdefs.h>
#include <alsdefs.h>
#ifdef  SPLINE
STATIC void als_trim0 __(( INT, INT, PARM, REAL*, REAL*, REAL* )) ;
STATIC void als_trim1 __(( INT, INT, PARM, REAL*, INT, REAL*, REAL* )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN als_parm_adjust ( n, d, knot, parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT n ;
INT  d;
REAL *knot ;
PARM parm ;
{

    if ( PARM_T(parm) < knot[d-1] || PARM_T(parm) > knot[n] ) 
        RETURN ( FALSE ) ;
    if ( PARM_J(parm) < d-1 ) 
        PARM_J(parm) = d-1 ;
/*
    if ( PARM_J(parm) > n ) 
        PARM_J(parm) = n ;
*/
    if ( PARM_J(parm) >= n ) 
        PARM_J(parm) = n - 1 ;
    while ( ( PARM_J(parm) < n-1 ) && 
            ( PARM_T(parm) >= knot[PARM_J(parm)+1] ) )
        (PARM_J(parm))++ ;
    while ( ( PARM_J(parm) >= d ) &&
            ( PARM_T(parm) < knot[PARM_J(parm)] ) )
        (PARM_J(parm))-- ;
    
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void als_insert_knot ( a, d, knot, dim, u, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *a      ;  /* a[d][dim] - the old control points */
INT d   ;       /* order */
REAL *knot ;   /* knot0[2*d] */
INT dim ;
REAL    u   ;   /* new knot , knot[d-1] ó u ó knot[d] */
REAL *b     ;   /* b[d+1][dim] */
{
    INT i, j ;
    REAL u0, u1, *b_i, *a_i, *a_i_1 ;

    for ( j=0 ; j<dim ; j++ )
        b[j] = a[j] ;
    for ( i=1, b_i=b+dim, a_i=a+dim, a_i_1=a ; i<d ; 
        i++, b_i+=dim, a_i_1=a_i, a_i+=dim ) {
        u0 = ( u - knot[i] ) / ( knot[i+d-1] - knot[i] ) ;
        u1 = 1.0 - u0 ;
        for ( j=0 ; j<dim ; j++ )
            b_i[j] = u0 * a_i[j] + u1 * a_i_1[j] ;
    }
    b_i = b+d*dim ;
    a_i = a+(d-1)*dim ;

    for ( j=0 ; j<dim ; j++ )
        b_i[j] = a_i[j] ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void als_insert_knot0 ( a, d, knot, dim, c ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *a  ;      /* c[d][dim] */
INT  d ;
REAL *knot  ;   /* knot[2*d] */
INT  dim ;
REAL *c  ;      /* x[d][dim] */
{
/********************
    Insert new knot = t[d-1] so that the total multiplicity 
    of the new knot is d-1 **********/

    INT i, j, k, m ;
    REAL *c0, *c1, u, v, knot_d_1=knot[d-1] ;

    for ( m=d-2 ; m>=0 && knot[m] >= knot_d_1-BBS_ZERO ; m-- ) ;
    if ( c!= a ) 
        ala_copy ( a, d*dim, c ) ;
    for ( k=1 ; k<=m ; k++ ) {
        for ( i=0, c0=c, c1=c+dim ; i<=m-k ; i++, c0=c1, c1+=dim ) {
            /* c0=c+i*dim, c1 = c+(i+1)*dim */
            u = ( knot_d_1 - knot[i+k] ) / ( knot[i+d] - knot[i+k] ) ;
            v = 1.0 - u ;
            for ( j=0 ; j<dim ; j++ ) 
                c0[j] = u * c1[j] + v*c0[j] ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void als_insert_knot1 ( a, d, knot, dim, c ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *a  ;      /* c[d][dim] */
INT  d ;
REAL *knot  ;   /* knot[2*d] */
INT  dim ;
REAL *c  ;      /* x[d][dim] */
{
/********************
    Insert new knot = t[d] so that the total multiplicity 
    of the new knot is d-1 **********/

    INT i, j, k, m ;
    REAL *c0, *c1, u, v, knot_d_1=knot[d-1], knot_d=knot[d], 
        w=knot_d-knot_d_1 ;

    for ( m=d-2 ; m>=0 && knot[2*d-m-1] <= knot_d+BBS_ZERO ; m-- ) ;
    if ( c!= a ) 
        ala_copy ( a, d*dim, c ) ;
    for ( k=0 ; k<m ; k++ ) {
        for ( i=d-1, c0=c+i*dim, c1=c0-dim ; i>=d+k-m ; 
                i--, c0=c1, c1-=dim ) {
            /* c0=c+i*dim, c1 = c+(i-1)*dim */
            u = w / ( knot[i+d-1-k] - knot_d_1 ) ;
            v = 1.0 - u ;
            for ( j=0 ; j<dim ; j++ ) 
                c0[j] = u * c0[j] + v*c1[j] ;
        }
    }
}
    
/*-------------------------------------------------------------------------*/
BBS_PRIVATE void als_eval0 ( mtx, d, knot, dim, t, p ) 
/*-------------------------------------------------------------------------*/
                /***************************************/
                /*** This routine works only for p<d ***/
                /***************************************/
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *mtx    ;   /* mtx[d][d][dim] ; mtx[0][i]=a[i] */
INT     d       ;   /* order of the spline */
REAL    *knot   ;   /* knot[2*d-1] */
INT     dim     ;   /* dimension of the spline */
REAL    t       ;   /* parameter value, t[d-1]<=t<=t[d], t[d-1]<t[d] */ 
INT     p       ;   /* Evaluation order */
/* mtx[d-k-1][0][j] = is the output  *************************************/
{ 
    REAL u, v, w, *row0, *row1 ;
    INT k, i, i_dim, j, m, d_dim=d*dim ; 

    for ( k=1, row0=mtx, row1=mtx+d_dim ; k<d ; k++, row0=row1, row1+=d_dim )
        /* row0 = &(mtx[k-1][0][0]) = mtx+(k-1)*d_dim, 
           row1 = &(mtx[k][0][0]) = mtx+k*d_dim */
        for ( i=0, i_dim=0 ; i<d-k ; i++, i_dim+=dim ) { /* i_dim = i*dim */
            w = knot[i+d]-knot[i+k] ;
            u = IS_ZERO(w) ? 0.5 : ( knot[i+d]-t ) / w ;
            v = 1.0 - u ;
            for ( j=0 ; j<dim ; j++ ) 
                row1[i_dim+j] = 
                u * row0[i_dim+j] + v * row0[i_dim+dim+j] ;
        }

    for ( m=1 ; m<=p ; m++ )
        for ( k=d-m-1, row1=mtx+k*d_dim ; k>=d-p-1 ; k--, row1-=d_dim )
        /* row1 = &(mtx[k][0][0]) = mtx+k*d_dim */

            for ( i= 0, i_dim=0 ; i<d-k-m ; i++, i_dim+=dim ) { 
                    /* i_dim = i*dim */
                w = knot[i+d]-knot[i+k+m] ;
                u = IS_ZERO(w) ? 1.0 : 1.0 / ( knot[i+d]-knot[i+k+m] ) ;
                for ( j=0 ; j<dim ; j++ ) 
                    row1[i_dim+j] = 
                        u * ( row1[i_dim+dim+j] - row1[i_dim+j] ) ;
            }

    u = 1.0 ;
    /* row0 = &(mtx[d-k-1][0][0]) = mtx+(d-k-1)*d_dim */
    for ( k=1, row0=mtx+(d-2)*d_dim ; k<=p ; k++, row0-=d_dim ) {
        u *= ( (REAL) ( d-k ) ) ;
        for ( j=0 ; j<dim ; j++ )
            row0[j] *= u ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void    als_eval ( c, d, knot, dim, t, p, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *c    ;   /* c[d][dim] ; mtx[0][i]=c[i] */
INT     d       ;   /* order of the spline */
REAL    *knot   ;   /* knot[2*d-1] */
INT     dim     ;   /* dimension of the spline */
REAL    t       ;   /* parameter value */ 
INT     p       ;   /* Evaluation order */
REAL    *x      ;   /* x[p+1][dim] - Returned spline values */
/**** mtx[d-k-1][0][j] = x[k][j] is the output  ****************************/
{ 
    REAL *mtx ;
    INT p1, i, j, d_dim=d*dim ; 
    
    mtx = CREATE ( d*d_dim, REAL ) ;
    for ( i=0 ; i<d_dim ; i++ )
        mtx[i] = c[i] ;
    p1 = p<d ? p : d-1 ; /* p1<d */
    als_eval0 ( mtx, d, knot, dim, t, p1 ) ;
    for ( i=0 ; i<=p1 ; i++ )
        for ( j=0 ; j<dim ; j++ )
            x[i*dim+j] = mtx[(d-i-1)*d_dim+j] ;
    for ( i=d ; i<=p ; i++ )
        for ( j=0 ; j<dim ; j++ )
            x[i*dim+j] = 0.0 ;
    KILL(mtx) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void als_conv_bez ( a, d, knot, dim, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *a  ;      /* a[d][dim] - b-spline control points */
INT  d ;
REAL *knot  ;   /* knot[2*d] */
INT  dim ;
REAL *b  ;      /* b[d][dim] - bezier control points */
{
    als_insert_knot0 ( a, d, knot, dim, b ) ;
    als_insert_knot1 ( b, d, knot, dim, b ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void als_trim ( a0, d, knot0, dim, parm0, parm1, a1, n, knot1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL* a0 ;
INT d ;
REAL* knot0 ;
INT dim ;
PARM parm0, parm1 ;
REAL* a1 ;
INT n ;
REAL* knot1 ;
{
    INT j0 ;
    REAL *b ;

    j0 = ( parm0 == NULL ) ? d-1 : PARM_J(parm0) ;

    ala_copy ( a0+dim*(j0-d+1), dim*n, a1 ) ;
    ala_copy ( knot0+j0-d+1, n+d, knot1 ) ;
    b = CREATE ( dim*(d+1), REAL ) ;
    if ( parm0 != NULL ) 
        als_trim0 ( d, dim, parm0, a1, knot1, b ) ;
    if ( parm1 != NULL ) 
        als_trim1 ( d, dim, parm1, a1, n, knot1, b ) ;
    KILL ( b ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void als_trim0 ( d, dim, parm0, a, knot, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT d ;
INT dim ;
PARM parm0 ;
REAL* a ;
REAL* knot ;
REAL* b ;
{
    INT i ;
    als_insert_knot ( a, d, knot, dim, PARM_T(parm0), b ) ; 
    ala_copy ( b+dim, dim*d, a ) ;
    for ( i=0 ; i<d-1 ; i++ ) 
        knot[i] = knot[i+1] ;
    knot[d-1] = PARM_T(parm0) ;
    als_insert_knot0 ( a, d, knot, dim, a ) ; 
    for ( i=0 ; i<d ; i++ ) 
        knot[i] = knot[d-1] ;
}


/*-------------------------------------------------------------------------*/
STATIC void als_trim1 ( d, dim, parm1, a, n, knot, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT d ;
INT dim ;
PARM parm1 ;
REAL* a ;
INT n ;
REAL* knot ;
REAL* b ;
{
    INT i ;

    while ( knot[n+d-1] > PARM_T(parm1) + BBS_ZERO ) { 
        als_insert_knot ( a+dim*(n-d), d, knot+n-d, dim, PARM_T(parm1), b ) ;
        ala_copy ( b, dim*d, a+dim*(n-d) ) ; 
        for ( i=d-2 ; i>=0 ; i-- ) 
            knot[n+i+1] = knot[n+i] ;
        knot[n] = PARM_T(parm1) ;
    }
}
#endif  /*SPLINE*/


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN als_ellipse_break_t ( t0, t1, r, t_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL t0, t1, r, *t_ptr ;
{
    REAL x, y, cos2x, cosy, q, t[2], c0, c1, a0, a1, a2 ;
    INT mult[2], m ;

    x = t0 + t1 ;
    y = 0.5 * ( t1 - t0 ) ;
    q = 2.0 / ( 1.0 + r * r ) - 1.0 ;
    cos2x = (REAL)cos ( (double)x ) ;
    cosy = (REAL)cos ( (double)y ) ;
    a0 = q * cosy ;
    a1 = - 1.0 - q * cos2x ;
    a2 = cosy * ( cos2x + q * ( 1.0 - cosy * cosy ) ) ;
    m = alp_solve3 ( a0, a1, a2, t, mult ) ;

    c0 = (REAL)cos ( (double)(x + y) ) ;
    c1 = (REAL)cos ( (double)(x - y) ) ;

    if ( ( m == 2 ) && 
        ( ( t[0] <= c0 - BBS_ZERO ) || ( t[0] >= c1 + BBS_ZERO ) ) )
    {
        m-- ;
        t[0] = t[1] ;
    }
    if ( ( m > 0 ) && 
        ( ( t[m-1] <= c0 - BBS_ZERO ) || ( t[m-1] >= c1 + BBS_ZERO ) ) )
        m-- ;
    if ( m == 0 ) 
        RETURN ( FALSE ) ;
    if ( m == 2 ) 
    {
        if ( fabs ( (double)(t[1] - cos2x) ) < fabs ( (double)(t[0] - cos2x) ) )
            t[0] = t[1] ;
        m-- ;
    }
    *t_ptr = (REAL)acos ( (double)t[0] ) - 0.5 * x ;
    RETURN ( TRUE ) ;
}


