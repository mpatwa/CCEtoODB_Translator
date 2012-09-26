/* -2 -3 */
/********************************** ALL.C **********************************/
/***************************** Linear algebra ******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <aladefs.h>
#include <alldefs.h>
#ifdef   SPLINE

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN all_3diag ( a, b, c, d, n, dim, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *a     ;   /* a[n] */
REAL *b     ;   /* b[n-1] */
REAL *c     ;   /* c[n-1] */
REAL *d     ;   /* d[n][dim] */
INT  n ;
INT  dim ;
REAL *x     ;   /* x[n][dim] */
{
    REAL *p, *q ;
    BOOLEAN status ;

    p = CREATE ( n, REAL ) ;
    q = CREATE ( n-1, REAL ) ;
    status = all_3diag_0 ( a, b, c, n, p, q ) ;
    if ( status ) 
        all_3diag_1 ( b, p, q, d, n, dim, x ) ;
    KILL ( p ) ;
    KILL ( q ) ;
    RETURN ( status ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN all_3diag_0 ( a, b, c, n, p, q ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *a     ;   /* a[n] */
REAL *b     ;   /* b[n-1] */
REAL *c     ;   /* c[n-1] */
INT  n ;
REAL *p     ;   /* p[n] */
REAL *q     ;   /* q[n-1] */
{
    INT i ;
    REAL u ;
    if ( IS_ZERO(a[0]) ) 
        RETURN ( FALSE ) ;
    p[0] = 1.0 / a[0] ;
    for ( i=1 ; i<n ; i++ ) {
        q[i-1] = - c[i-1] * p[i-1] ;
        u = a[i] + q[i-1] * b[i-1] ;
        if ( IS_ZERO(u) ) 
            RETURN ( FALSE ) ;
        p[i] = 1.0 / u ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void all_3diag_1 ( b, p, q, d, n, dim, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *b     ;   /* b[n-1] */
REAL *p     ;   /* p[n] */
REAL *q     ;   /* q[n-1] */
REAL *d     ;   /* d[n-1][dim] */
INT  n ;
INT  dim ;
REAL *x     ;   /* x[n][dim] */
{
    INT i, j, k ;


    for ( j=0 ; j<dim ; j++ )  
        x[j] = d[j] * p[0] ;

    for ( i=1, k=dim ; i<n ; i++, k+=dim )  /* k=i*dim */
        for ( j=dim+k-1 ; j>=k ; j-- )  
            x[j] = ( d[j] - b[i-1] * x[j-dim] ) * p[i] ;

    for ( i=n-2, k=(n-2)*dim ; i>= 0 ; i--, k-=dim ) 
        for ( j=dim+k-1 ; j>=k ; j-- )  
            x[j] += ( q[i] * x[dim+j] ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN all_3diag_per ( a, b, c, d, n, dim, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *a     ;   /* a[n] */
REAL *b     ;   /* b[n] */
REAL *c     ;   /* c[n] */
REAL *d     ;   /* d[n][dim] */
INT  n ;
INT  dim ;
REAL *x     ;   /* x[n][dim] */
{
    REAL *p, *q, *u, *v ;
    BOOLEAN status ;

    p = CREATE ( n, REAL ) ;
    q = CREATE ( n-1, REAL ) ;
    u = CREATE ( n, REAL ) ;
    v = CREATE ( n-1, REAL ) ;
    status = all_3diag_per_0 ( a, b, c, n, p, q, u, v ) &&
        all_3diag_per_1 ( b, p, q, u, v, d, n, dim, x ) ;
    KILL ( p ) ;
    KILL ( q ) ;
    KILL ( u ) ;
    KILL ( v ) ;
    RETURN ( status ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN all_3diag_per_0 ( a, b, c, n, p, q, u, v ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *a     ;   /* a[n] */
REAL *b     ;   /* b[n-1] */
REAL *c     ;   /* c[n-1] */
INT  n ;
REAL *p     ;   /* p[n] */
REAL *q     ;   /* q[n-1] */
REAL *u     ;   /* u[n] */
REAL *v     ;   /* v[n-1] */
{
    INT i ;
    REAL z, s ;

    if ( IS_ZERO(a[0]) ) 
        RETURN ( FALSE ) ;
    p[0] = 1.0 / a[0] ;

    if ( n==1 ) 
        u[0] = 0.0 ;
    else if ( n==2 ) {
        q[0] = - c[0] * p[0] ;
        z = a[1] + b[0] * q[0] ;
        if ( IS_ZERO(z) )
            RETURN ( FALSE ) ;
        p[1] = 1.0 / z ;
        q[0] = 0.0 ;
        u[0] = 0.0 ;
        v[0] = 0.0 ;
    }
    else {
        u[0] = c[n-1] ;
        v[0] = b[n-1] * p[0] ;
        s = u[0] * v[0] ;
        q[0] = -c[0] * p[0] ;
        for ( i=1 ; i<n-1 ; i++ ) {
            z = a[i] + q[i-1] * b[i-1] ;
            if ( IS_ZERO(z) ) 
                RETURN ( FALSE ) ;
            p[i] = 1.0 /z ;
            q[i] = -c[i] * p[i] ;
            u[i] = u[i-1] * q[i-1] ;
            v[i] = -b[i-1] * v[i-1] * p[i] ;
            s += u[i] * v[i] ;
        }
        z = a[n-1] + q[n-2] * b[n-2] ;
        if ( IS_ZERO(z) ) 
            RETURN ( FALSE ) ;
        p[n-1] = 1.0 / z ;
        u[n-1] = u[n-2] * q[n-2] - b[n-2] * v[n-2] - s ;
    }
    RETURN ( TRUE ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN all_3diag_per_1 ( b, p, q, u, v, d, n, dim, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *b     ;   /* b[n-1] */
REAL *p     ;   /* p[n] */
REAL *q     ;   /* q[n-1] */
REAL *u     ;   /* u[n] */
REAL *v     ;   /* v[n-1] */
REAL *d     ;   /* d[n-1][dim] */
INT  n ;
INT  dim ;
REAL *x     ;   /* x[n][dim] */
{
    INT i, j, k, m ;
    REAL *s, w, z ;


    s = CREATE ( dim, REAL ) ;
    for ( j=0 ; j<dim ; j++ ) {
        x[j] = d[j] * p[0] ; 
        s[j] = u[0] * x[j] ;
    }
    for ( i=1, k=dim ; i<n-1 ; i++, k+=dim ) {
        for ( j=0 ; j<dim ; j++ ) {
            x[k+j] = ( d[k+j] - b[i-1] * x[k-dim+j] ) * p[i] ;
            s[j] += u[i] * x[k+j] ;
        }
    }

    m = (n-1)*dim ;

    if ( n>1 ) {
        if ( IS_ZERO(p[n-1]) ) {
            KILL ( s ) ;
            RETURN ( FALSE ) ;
            }
        w = 1.0 / p[n-1] ;
        z = u[n-1] + w ;
        if ( IS_ZERO(z) ) {
            KILL ( s ) ;
            RETURN ( FALSE ) ;
        }
        for ( j=0 ; j<dim ; j++ ) 
            x[m+j] = ( d[m+j] - b[n-2] * x[m-dim+j] - s[j] ) / z ;
    }

    for ( i=n-2, k=m-dim /* = i * dim */ ; i>=0 ; i--, k-=dim )
        for ( j=0 ; j<dim ; j++ )
            x[k+j] += q[i] * x[k+dim+j] - v[i] * x[m+j] ;
    KILL ( s ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN all_3diag_per_2 ( a, b, c, p, q, d, n, dim, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *a     ;   /* a[n] */
REAL *b     ;   /* b[n] */
REAL *c     ;   /* c[n] */
REAL p ;
REAL q ;
REAL *d     ;   /* d[n][dim] */
INT  n ;
INT  dim ;
REAL *x     ;   /* x[n][dim] */
{
    REAL a0, b0, c0, a1, b1, c1, *d0, *d1, *d_last ;
    INT j ;
    BOOLEAN status ;
    /* a[0]*x[0]+c[0]*x[0]+p*x[n-2]+b[n-1]*x[n-1]=d[0] ;...;
       c[n-1]*x[0]+q*x[1]+b[n-2]*x[n-2]+a[n-1]*x[n-1]=d[n-1] */
    d0 = CREATE ( dim, REAL ) ;
    d1 = CREATE ( dim, REAL ) ;
    d_last = d + (n-1)*dim ;
 
    /* Save all the arrays whose elements will be changed */
    a0 = a[0] ;
    b0 = b[n-2] ;
    c0 = c[0] ;
    a1 = a[n-1] ;
    b1 = b[n-1] ;
    c1 = c[n-1] ;
    for ( j=0 ; j<dim ; j++ ) {
        d0[j] = d[j] ;
        d1[j] = d_last[j] ;
    }
    
    /* Recompute the coefficients */
    a[0] = a0 * b0 - c1 * p ;
    c[0] = c0 * b0 - q * p ;
    b[n-1] = b1 * b0 - a1 * p ;
    c[n-1] = c1 * c0 - a0 * q ;
    b[n-2] = c[0] ;
    a[n-1] = a1 * c0 - b1 * q ;
    for ( j=0 ; j<dim ; j++ ) {
        d[j] = d0[j] * b0 - d1[j] * p ;
        d_last[j] = d1[j] * c0 - d0[j] * q ;
    }
    
    status = all_3diag_per ( a, b, c, d, n, dim, x ) ;
    
    /* Restore all the arrays whose elements have been changed */
    a[0] = a0 ;
    b[n-2] = b0 ;
    c[0] = c0 ;
    a[n-1] = a1 ;
    b[n-1] = b1 ;
    c[n-1] = c1 ;
    for ( j=0 ; j<dim ; j++ ) {
        d[j] = d0[j] ;
        d_last[j] = d1[j] ;
    }
    KILL ( d0 ) ;
    KILL ( d1 ) ;
    RETURN ( status ) ;
}   


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN all_lin_sys ( a, b, n, dim, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *a     ;   /* a[n][n]; ATTENTION : This routine modifies elements 
                   of the matrix ; if you do not want the matrix 
                   to get changed, make a copy of it */

REAL *b     ;   /* b[n][dim] */
INT  n ;
INT  dim ;
REAL *x     ;   /* x[n][dim] */
{

    INT i, i_max, j, k, m, p, m1 ;
    REAL a_max, pivot, r, *x_i, *x_m ;


    ala_copy ( b, n*dim, x ) ;
    
    for ( i=0, p=0, x_i=x ; i<n ; i++, p+=(n+1), x_i+=dim ) { 
    /* p=i*(n+1) = i*n+i ; p is the position of the submatrix */
    /* x_i=x+i*dim is the i-th component */
        i_max = i ;
        a_max = fabs(a[p]) ;
        for ( m=i+1, m1=p+n ; m<n ; m++, m1+=n )    /* m1 = m*n+i */
            if ( fabs(a[m1]) > a_max ) {
                i_max = m ;
                a_max = fabs(a[m1]) ;
            }
        if ( a_max <= BBS_TOL )
            RETURN ( FALSE ) ;
        if ( i_max != i ) {
            ala_swap ( a+i_max*n+i, a+p, n-i ) ;
            ala_swap ( x+i_max*dim, x_i, dim ) ;
        }
    
        pivot = a[p] ;
        for ( m=1 ; m<n-i ; m++ ) 
            a[p+m] /= pivot ;
        for ( j=0 ; j<dim ; j++ ) 
            x_i[j] /= pivot ;
        a[p] = 1.0 ;
        for ( m=0, m1=i, x_m=x ; m<i ; m++, m1+=n, x_m+=dim ) { /* m1=m*n+i */
            r = a[m1] ;
            for ( j=0 ; j<dim ; j++ )
                x_m[j] -= ( r * x_i[j] ) ;
            for ( k=1 ; k<n-i ; k++ ) 
                a[m1+k] -= ( r * a[p+k] ) ;
            a[m1] = 0.0 ;
        }   
        for ( m=i+1, m1=(i+1)*n+i, x_m=x_i+dim ; m<n ; m++, m1+=n, x_m+= dim ) {
            r = a[m1] ;
            for ( j=0 ; j<dim ; j++ )
                x_m[j] -= ( r * x_i[j] ) ;
            for ( k=1 ; k<n-i ; k++ )
                a[m1+k] -= ( r * a[p+k] ) ;
            a[m1] = 0.0 ;
        } 
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void all_product_mtx ( a, b, m, n, p, c ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *a     ;   /* a[m][n] */
REAL *b     ;   /* b[n][p] */
INT   m     ;
INT   n     ;
INT   p     ;
REAL *c     ;   /* c[m][p] */
{
    INT i, j, k ;
    REAL *a_ptr, *b_ptr, *c_ptr ;

    for ( i=0, c_ptr = c, a_ptr=a ; i<m ; i++, c_ptr++, a_ptr+=m ) 
        for ( j=0 ; j<p ; j++, c_ptr++ ) {
            *c_ptr = 0.0 ;
            for ( k=0, b_ptr=b+j ; k<n ; k++, b_ptr+=n ) 
                *c_ptr += *(a_ptr+k) * (*b_ptr) ;
        }
}
#endif   /*SPLINE*/

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN all_3_by_3_lin_sys ( a, b, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL a[3][3]  ;
REAL b[3]     ;
REAL x[3]     ;
{
    REAL det ;

    det = a[0][0] * ( a[1][1] * a[2][2] - a[2][1] * a[1][2] ) + 
          a[1][0] * ( a[2][1] * a[0][2] - a[0][1] * a[2][2] ) + 
          a[2][0] * ( a[0][1] * a[1][2] - a[1][1] * a[0][2] ) ;

    if ( IS_SMALL(det) ) 
        RETURN ( FALSE ) ;

    x[0] = ( b[0] * ( a[1][1] * a[2][2] - a[2][1] * a[1][2] ) + 
           b[1] * ( a[2][1] * a[0][2] - a[0][1] * a[2][2] ) + 
           b[2] * ( a[0][1] * a[1][2] - a[1][1] * a[0][2] ) ) / det ;
    x[1] = ( a[0][0] * ( b[1] * a[2][2] - b[2] * a[1][2] ) + 
           a[1][0] * ( b[2] * a[0][2] - b[0] * a[2][2] ) + 
           a[2][0] * ( b[0] * a[1][2] - b[1] * a[0][2] ) ) / det ;
    x[2] = ( a[0][0] * ( a[1][1] * b[2] - a[2][1] * b[1] ) + 
           a[1][0] * ( a[2][1] * b[0] - a[0][1] * b[2] ) + 
           a[2][0] * ( a[0][1] * b[1] - a[1][1] * b[0] ) ) / det ;
    RETURN ( TRUE ) ;
}

