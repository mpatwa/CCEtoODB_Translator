/* -2 -3 */
/********************************** ALB.C **********************************/
/***************** Processing Bernstein-Bezier polynomials *****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef  SPLINE
#include <aladefs.h>
#include <albdefs.h>

STATIC void    alb_eval0 __(( REAL* DUMMY0 , INT DUMMY1 , INT DUMMY2 , 
        REAL DUMMY3 , INT DUMMY4 )) ;

/*-------------------------------------------------------------------------*/
STATIC void    alb_eval0 ( mtx, d, dim, t, p ) 
                /***************************************/
                /*** This routine works only for p<d ***/
                /***************************************/
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *mtx    ;   /* mtx[d][d][dim] ; mtx[0][i]=a[i] */
INT     d       ;   /* order of the Bezier */
INT     dim     ;   /* dimension of the Bezier */
REAL    t       ;   /* parameter value */ 
INT     p       ;   /* Evaluation order */
/* mtx[d-k-1][0][j] = is the output  *************************************/
{ 
    REAL t1, w, *row0, *row1 ;
    INT k, i, j, m, m_max, dd=d*dim ; 
    
    t1 = 1.0 - t ;
    for ( i=1, row0=mtx, row1=row0+dd, m_max=dd-2*dim ; i<d ; 
          i++, row0=row1, row1+=dd, m_max-=dim )
        for ( m=m_max /* =(d-i-1)*dim */ ; m >= 0 ; m-=dim )
            for ( j=0 ; j<dim ; j++ )
                row1[m+j] = t1*row0[m+j] + t*row0[m+dim+j] ;

    /* row0 = mtx+d*(d-1)*dim after the loop */
    for ( k=1 ; k<=p ; k++ ) 
        for ( i=k, row1=mtx+dd*(d-k-1), m_max=0 ; i<=p ; 
            i++, row1=row1-dd, m_max+=dim ) {
            for ( m=0 ; m<=m_max /* =(i-k)*dim */ ; m+=dim )
                for ( j=0 ; j<dim ; j++ )
                    row1[m+j] = row1[m+dim+j] - row1[m+j] ;
        }
    w = 1.0 ;
    for ( k=1, row1=row0-dd ; k<=p ; k++, row1=row1-dd ) {
        w *= ( (REAL) ( d-k ) ) ;
        for ( j=0 ; j<dim ; j++ )
            row1[j] *= w ; 
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void    alb_eval ( b, d, dim, t, p, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *b    ;   /* b[d][dim] ; mtx[0][i]=b[i] */
INT     d       ;   /* order of the Bezier */
INT     dim     ;   /* dimension of the Bezier */
REAL    t       ;   /* parameter value */ 
INT     p       ;   /* Evaluation order */
REAL    *x      ;   /* x[p+1][dim] - Returned polynomial values */
/* mtx[d-k-1][0][j] = is the output  *************************************/
{ 
    REAL *mtx ;
    INT p1, i, j, dd=d*dim ; 
    
    mtx = CREATE ( d*dd, REAL ) ;
    for ( i=0 ; i<dd ; i++ )
        mtx[i] = b[i] ;

    p1 = p<d ? p : d-1 ;
    alb_eval0 ( mtx, d, dim, t, p1 ) ;

    for ( i=0 ; i<=p1 ; i++ )
        for ( j=0 ; j<dim ; j++ )
            x[i*dim+j] = mtx[(d-i-1)*dd+j] ;
    for ( i=d ; i<=p ; i++ )
        for ( j=0 ; j<dim ; j++ )
            x[i*dim+j] = 0.0 ;
    KILL(mtx) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void    alb_sbdv0 ( a, d, dim, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

REAL   *a  ; 
INT d   ;   /* order of the Bezier */
INT dim ;   /* dimension of the Bezier */
REAL    *b  ;   /* b[2*d-1] - subdivision */
{ 
    INT k, i, j, i1, dd=2*dim ; 
    REAL *a_ptr, *b_ptr ;
    
    for ( a_ptr=a, b_ptr=b, i=0 ; i<d ; a_ptr+=dim, b_ptr+=dd, i++ )
        for ( j=0 ; j<dim ; j++ )
            b_ptr[j] = a_ptr[j] ;
    for ( k=1, a_ptr=b+dim, i1=2*d-3 ; k<d ; k++, i1--, a_ptr+=dim )
        for ( i=k, b_ptr=a_ptr ; i<=i1 ; i+=2, b_ptr+=dd )
            for ( j=0 ; j<dim ; j++ )
                b_ptr[j] = 0.5 * ( b_ptr[j-dim]+b_ptr[j+dim] ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void alb_sbdvl ( a, d, dim, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *a  ; 
INT     d   ;   /* order of the Bezier */
INT     dim ;   /* dimension of the Bezier */
REAL    *b  ;   /* b[d] - subdivision */
{ 
    INT dd=d*dim, k, i ; 
    
    for ( i=0 ; i<dd ; i++ )
        b[i] = a[i] ;
    for ( k=1 ; k<d ; k++ )
        for (i=dd-1 ; i>=k*dim ; i-- )
            b[i] = 0.5 * ( b[i-dim] + b[i] ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void alb_sbdvr ( a, d, dim, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *a  ; 
INT     d   ;   /* order of the Bezier */
INT     dim ;   /* dimension of the Bezier */
REAL    *b  ;   /* b[d] - subdivision */
{ 
    INT dd=d*dim, k, i ; 
    
    for ( i=0 ; i<dd ; i++ )
        b[i] = a[i] ;
    for ( k=1 ; k<d ; k++ )
        for (i=0 ; i<dd-k*dim ; i++ )
            b[i] = 0.5 * ( b[i] + b[i+dim] ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void alb_brk0 ( a, d, dim, t, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *a  ; 
INT     d   ;   /* order of the Bezier */
INT     dim ;   /* dimension of the Bezier */
REAL    t   ;   /* break parameter */
REAL    *b  ;   /* b[2*d-1] - subdivision */
{ 
    INT k, i, j, i1, dd=2*dim ; 
    REAL s=1.0-t, *a_ptr, *b_ptr ;
    
    for ( a_ptr=a, b_ptr=b, i=0 ; i<d ; a_ptr+=dim, b_ptr+=dd, i++ )
        for ( j=0 ; j<dim ; j++ )
            b_ptr[j] = a_ptr[j] ;
    for ( k=1, a_ptr=b+dim, i1=2*d-3 ; k<d ; k++, i1--, a_ptr+=dim )
        for ( i=k, b_ptr=a_ptr ; i<=i1 ; i+=2, b_ptr+=dd )
            for ( j=0 ; j<dim ; j++ )
                b_ptr[j] = s * b_ptr[j-dim] + t * b_ptr[j+dim] ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void alb_brkl ( a, d, dim, t, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *a  ; 
INT     d   ;   /* order of the Bezier */
INT     dim ;   /* dimension of the Bezier */
REAL    t   ;   /* break parameter */
REAL    *b  ;   /* b[d] - subdivision */
{ 
    INT dd=d*dim, k, i ; 
    REAL s=1.0-t ;
    
    for ( i=0 ; i<dd ; i++ )
        b[i] = a[i] ;
    for ( k=1 ; k<d ; k++ )
        for (i=dd-1 ; i>=k*dim ; i-- )
            b[i] = s * b[i-dim] + t * b[i] ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void alb_brkr ( a, d, dim, t, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *a  ; 
INT     d   ;   /* order of the Bezier */
INT     dim ;   /* dimension of the Bezier */
REAL    t   ;   /* break parameter */
REAL    *b  ;   /* b[d] - subdivision */
{ 
    INT dd=d*dim, k, i ; 
    REAL s=1.0-t ;
    
    for ( i=0 ; i<dd ; i++ )
        b[i] = a[i] ;
    for ( k=1 ; k<d ; k++ )
        for (i=0 ; i<dd-k*dim ; i++ )
            b[i] = s * b[i] + t * b[i+dim] ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void alb_trim ( a, d, dim, t0, t1, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL    *a  ; 
INT     d   ;   /* order of the Bezier */
INT     dim ;   /* dimension of the Bezier */
REAL    t0, t1 ;   /* break parameter */
REAL    *b  ;   /* b[d] - subdivision */
{ 
    alb_brkr ( a, d, dim, t0, b ) ;
    alb_brkl ( a, d, dim, (t1-t0)/(1.0-t0), b ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void alb_mult ( b1, d1, b2, d2, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

REAL *b1 , *b2  ;
REAL *b  ;
INT d1, d2 ;
{
    INT j1, j2, j, d=d1+d2-1 ;
    REAL *c, *c1, *c2, *c0 ;

    c = CREATE ( d*d, REAL ) ;
    ala_binom ( d, c ) ;
    c1 = c + d * (d1-1) ;
    c2 = c + d * (d2-1) ;
    c0 = c + d * (d-1) ;

    for ( j=0 ; j<d ; j++ ) 
        b[j] = 0.0 ;
    
    for ( j1=0 ; j1<d1 ; j1++ ) {
        for ( j2=0 ; j2<d2 ; j2++ ) {
            b[j1+j2] += 
                ( ( c1[j1] * c2[j2] / c0[j1+j2] ) * ( b1[j1] * b2[j2] ) ) ;
        }
    }
    KILL ( c ) ;
}

#endif  /*SPLINE*/

