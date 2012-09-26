/* -2 -3 */
/********************************** ALSD.C **********************************/
/******************* Dimension independent splines *************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <bbsdefs.h>
#ifdef  SPLINE
#include <aladefs.h>
#include <alldefs.h>
#include <alsdefs.h>
STATIC  BOOLEAN alsd_interp_clsd_cl_ends __(( REAL* DUMMY0 , INT DUMMY1 , 
            INT DUMMY2 , REAL* DUMMY3 , REAL* DUMMY4 )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN als_interp_tan ( a, n, dim, knot, tan0, tan1, c ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *a     ;   /* a[n-2][dim] */
INT n ;
INT dim ;
REAL *knot ;   /* knot[n+4] */
REAL *tan0 ;   /* tan0[dim] */
REAL *tan1 ;   /* tan1[dim] */
REAL *c    ;   /* c[n][dim] */

{
    REAL *diag, *super_diag, *sub_diag, w0, w1, w, z, *a0, *a1, *a_last, 
        *c_last ;
    INT i,j ;
    BOOLEAN status ;

    diag = CREATE ( n-2, REAL ) ;
    super_diag = CREATE ( n-3, REAL ) ;
    sub_diag = CREATE ( n-3, REAL ) ;
    a_last = a + (n-3)*dim ;
    c_last = c + (n-1)*dim ;
    a0 = CREATE ( dim, REAL ) ;
    a1 = CREATE ( dim, REAL ) ;
    diag[0] = ( knot[5]-knot[3] ) / ( knot[5]-knot[2] ) ;
    super_diag[0] = 1.0 - diag[0] ;
    diag[n-3] = ( knot[n]-knot[n-2] ) / ( knot[n+1]-knot[n-2] ) ;
    sub_diag[n-4] = 1.0 - diag[n-3] ;
    w0 = ( knot[4]-knot[3] ) / 3.0 ;
    w1 = ( knot[n]-knot[n-1] ) / 3.0 ;
    for ( j=0 ; j<dim ; j++ ) {
        a0[j] = a[j] ;
        a[j] += w0 * tan0[j] ;
        a1[j] = a_last[j] ;
        a_last[j] -= w1 * tan1[j] ;
    }
    
    w = knot[4] - knot[3] ;
    w *= w ;
    for ( i=1 ; i<n-3 ; i++ ) {
        z = knot[i+4] - knot[i+2] ;
        super_diag[i] = w / ( z * (knot[i+5]-knot[i+2]) ) ;
        w = knot[i+4] - knot[i+3] ;
        w *= w ;
        sub_diag[i-1] = w / ( z * (knot[i+4]-knot[i+1]) ) ;
        diag[i] = 1.0 - sub_diag[i-1] - super_diag[i] ;
    }
    
    status = all_3diag ( diag, sub_diag, super_diag, a, n-2, dim, c+dim ) ;
    
    w0 = ( knot[3]-knot[2] ) / 3.0 ;
    w = ( knot[4]-knot[1] ) / ( knot[4]-knot[3] ) ;
    w1 = ( knot[n+1]-knot[n] ) / 3.0 ;
    z = ( knot[n+2]-knot[n-1] ) / ( knot[n]-knot[n-1] ) ;
    for ( j=0 ; j<dim ; j++ ) {
        c[j] = c[dim+j] + w * ( a0[j] - c[dim+j] - tan0[j] * w0 ) ;
        a[j] = a0[j] ;
        c_last[j] = c_last[j-dim] + 
            z * ( a1[j] - c_last[j-dim] + w1 * tan1[j] ) ;
        a_last[j] = a1[j] ;
    }
    
    KILL ( diag ) ;
    KILL ( super_diag ) ;
    KILL ( sub_diag ) ;
    KILL ( a0 ) ;
    KILL ( a1 ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN als_interp_clsd ( a, n, dim, knot, c ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *a      ;   /* a[n-3]; a[n-3]=a[0] */
INT n ;
INT dim ;
REAL *knot  ;   /* knot[n+4] */
REAL *c     ;   /* c[n]  */
{
    BOOLEAN open_end_0, open_end_1 ;
    REAL t0[3], t1[3] ;

    open_end_0 = ( knot[3]-knot[1] > BBS_ZERO ) ;
    open_end_1 = ( knot[n+2]-knot[n] > BBS_ZERO ) ;
    if ( open_end_0 ) {
        ala_copy ( knot+1, 3, t0 ) ;
        knot[1] = knot[2] = knot[3] ;
    }
    if ( open_end_1 ) {
        ala_copy ( knot+n, 3, t1 ) ;
        knot[n+1] = knot[n+2] = knot[n] ;
    }
    if ( !alsd_interp_clsd_cl_ends ( a, n, dim, knot, c ) )
        RETURN ( FALSE ) ;
    if ( open_end_0 ) {
        ala_copy ( t0, 3, knot+1 ) ;
        als_4ord_open_end ( c, c+dim, c+2*dim, knot[1], knot[2], knot[3], 
            knot[4], knot[5], dim, c, c+dim ) ;
    }
    if ( open_end_1 ) {
        ala_copy ( t1, 3, knot+n ) ;
        als_4ord_open_end ( c+(n-1)*dim, c+(n-2)*dim, c+(n-3)*dim, 
            knot[n+2], knot[n+1], knot[n], knot[n-1], knot[n-2], dim, 
            c+(n-1)*dim, c+(n-2)*dim ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN alsd_interp_clsd_cl_ends ( a, n, dim, knot, c ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *a      ;   /* a[n-3]; a[n-3]=a[0] */
INT n ;
INT dim ;
REAL *knot  ;   /* knot[n+4] */
REAL *c     ;   /* c[n]  */

{
    REAL *diag, *super_diag, *sub_diag, *a_last, u0, u1, v0, v1, v2, w, z ;
    INT i,j ;
    BOOLEAN status ;
        
    diag = CREATE ( n-3, REAL ) ;
    super_diag = CREATE ( n-3, REAL ) ;
    sub_diag = CREATE ( n-3, REAL ) ;
    a_last = CREATE ( dim, REAL ) ;

    /* Set up equations except the first and the last ones */
    w = knot[4] - knot[3] ;
    w *= w ;

    for ( i=1 ; i<n-4 ; i++ ) {
        z = knot[i+4] - knot[i+2] ;
        super_diag[i] = w / ( z * (knot[i+5]-knot[i+2]) ) ;
        w = knot[i+4] - knot[i+3] ;
        w *= w ;
        sub_diag[i-1] = w / ( z * (knot[i+4]-knot[i+1]) ) ;
        diag[i] = 1.0 - sub_diag[i-1] - super_diag[i] ;
    }
    
    /* Set up the first and the last equation */
    u0 = 1.0 / ( knot[4]-knot[3] ) ;
    u1 = 1.0 / ( knot[5]-knot[3] ) ;
    v0 = 1.0 / ( knot[n]-knot[n-1] ) ;
    v1 = 1.0 / ( knot[n]-knot[n-2] ) ;
    v2 = 1.0 / ( knot[n]-knot[n-3] ) ;
    w = (u0+v0) * (u0+v1) ;
    super_diag[0] = - u0*u1 / w ;
    sub_diag[n-4] = v0*v1 / w ;
    diag[0] = 1.0 - super_diag[0] - sub_diag[n-4] ;
    u0 /= v0 ;
    v1 /= v0 ;
    v2 /= v0 ;
    w = 1.0 - v1 ;
    w *= w ;    /* w = (1-v1)ý */
    sub_diag[n-5] = v1 * v2 ;
    diag[n-4] = v1 * ( 2.0 - v1 - v2 ) ;
    super_diag[n-4] = - u0 * w ;
    w *= ( u0+1.0 ) ;
    ala_copy ( a+(n-4)*dim, dim, a_last ) ;
    for ( j=0 ; j<dim ; j++ ) 
        a[(n-4)*dim+j] -= ( a[j]*w ) ;

    status = all_3diag_per ( diag, sub_diag, super_diag, a, n-3, dim, c+dim ) ;
    ala_copy ( a_last, dim, a+(n-4)*dim ) ;
    ala_copy ( a, dim, c ) ;
    ala_copy ( a, dim, c+(n-1)*dim ) ;
    for ( j=0 ; j<dim ; j++ ) 
        c[(n-2)*dim+j] = a[j] + u0 * ( a[j] - c[dim+j] ) ;
            
    KILL ( diag ) ;
    KILL ( super_diag ) ;
    KILL ( sub_diag ) ;
    KILL ( a_last ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void als_4ord_close_end ( c0, c1, c2, t1, t2, t3, t4, t5, 
            dim, x0, x1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *c0 , *c1 , *c2  ;
REAL t1, t2, t3, t4, t5 ;
INT dim ;
REAL *x0 , *x1  ;
{
    REAL *y ;
    REAL u, v ;
    INT j ;
    
    y = CREATE ( dim, REAL ) ;
    u = ( t4-t3 ) / ( t4-t1 ) ;
    v = 1.0 - u ;
    for ( j=0 ; j<dim ; j++ )
        y[j]  = u * c0[j] + v * c1[j] ;
    u = ( t5-t3 ) / ( t5-t2 ) ;
    v = 1.0 - u ;
    for ( j=0 ; j<dim ; j++ )
        x1[j]  = u * c1[j] + v * c2[j] ;
    u = ( t3-t2 ) / ( t4-t2 ) ;
    v = 1.0 - u ;
    for ( j=0 ; j<dim ; j++ )
        x0[j]  = u * x1[j] + v * y[j] ;
    KILL ( y ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void als_4ord_open_end ( x0, x1, x2, t1, t2, t3, t4, t5, 
            dim, c0, c1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *x0 , *x1 , *x2  ;
REAL t1, t2, t3, t4, t5 ;
INT dim ;
REAL *c0 , *c1  ;
{
    REAL *y ;
    REAL u, v ;
    INT j ;
    
    y = CREATE ( dim, REAL ) ;
    u = ( t4-t2 ) / ( t4-t3 ) ;
    v = 1.0 - u ;
    for ( j=0 ; j<dim ; j++ )
        y[j]  = u * x0[j] + v * x1[j] ;
    u = ( t5-t2 ) / ( t5-t3 ) ;
    v = 1.0 - u ;
    for ( j=0 ; j<dim ; j++ )
        c1[j]  = u * x1[j] + v * x2[j] ;
    u = ( t4-t1 ) / ( t4-t3 ) ;
    v = 1.0 - u ;
    for ( j=0 ; j<dim ; j++ )
        c0[j]  = u * y[j] + v * c1[j] ;
    KILL ( y ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void als_uniform_knots ( n, d, knot )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT n, d ;
REAL *knot  ;   /* knot[n+d] */
{
    INT i ;
    knot[0] = (REAL)(1-d) ;
    for ( i=1 ; i<n+d ; i++ )
        knot[i] = knot[i-1] + 1.0 ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void als_clsc_uni_knots ( n, d, knot )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT n, d ;
REAL *knot  ;   /* knot[n+d] */
{
    INT i ;
    for ( i=0 ; i<d ; i++ )
        knot[i] = 0.0 ;
    for ( i=d ; i<=n ; i++ )
        knot[i] = knot[i-1] + 1.0 ;
    for ( i=n+1 ; i<n+d ; i++ )
        knot[i] = knot[i-1] ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void als_ellipse_knots ( n, knot ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT n ;
REAL *knot ;
{
    knot[0] = knot[1] = knot[2] = 0.0 ;
    if ( n==3 ) 
        knot[3] = knot[4] = knot[5] = 1.0 ;
    else {
        knot[3] = knot[4] = 0.5 ;
        knot[5] = knot[6] = 1.0 ;
        if ( n==5 ) 
            knot[7] = 1.0 ;
        else { 
            knot[7] = knot[8] = 1.5 ;
            knot[9] = knot[10] = knot[11] = 2.0 ;
        }
    }
}

#endif  /*SPLINE*/


