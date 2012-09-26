/* -2 -3 */
/******************************* FNA.C *************************************/
/*********** fna routines - arrays of homogeneous numbers ******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#include <fnadefs.h>
#ifdef   SPLINE

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN fna_extr ( b, d, dim, j, f_max, i_max, f_min, i_min )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL   *b      ;   /* Array of homogeneous points; in fact *b [dim+1] */
INT     d       ;   /* Order */
INT     dim     ;   /* Coordinate */
INT     j       ;   /* Coordinate */
REAL    *f_max  ;   /* Max. number */
INT     *i_max  ;   /* Index of the max. number */
REAL    *f_min  ;   /* Min. number */
INT     *i_min  ;   /* Index of the min. number */
{
    INT i, status ;
    REAL f, f_prev ;
    BOOLEAN interior_extr = FALSE ;

    if ( b[dim] <= BBS_ZERO ) 
        RETURN ( FALSE ) ;
    *f_max = *f_min = b[j]/b[dim] ;
    if ( i_max != NULL ) 
        *i_max = 0 ;
    if ( i_min != NULL ) 
        *i_min = 0 ;

    if ( b[dim+1+dim] <= BBS_ZERO ) 
        RETURN ( FALSE ) ;
    f = b[dim+1+j]/b[dim+1+dim] ;
    if ( f > *f_max + BBS_TOL ) {
        if ( i_max != NULL ) 
            *i_max = 1 ;
        *f_max = f ;
        status = 1 ;
    }
    else if ( f < *f_min - BBS_TOL ) {
        if ( i_min != NULL ) 
            *i_min = 1 ;
        *f_min = f ;
        status = -1 ;
    }
    else 
        status = 0 ;

    for ( i=2 ; i<d-1 ; i++ ) {
        f_prev = f ;
        if ( b[i*(dim+1)+dim] < BBS_ZERO ) 
            RETURN ( FALSE ) ;
        f = b[i*(dim+1)+j]/b[i*(dim+1)+dim] ;
        interior_extr = interior_extr || 
            ( status == -1 && f > f_prev + BBS_TOL ) || 
            ( status == 1 && f < f_prev - BBS_TOL ) ;
        if ( f > *f_max ) {
            if ( i_max != NULL ) 
                *i_max = i ;
            *f_max = f ;
            status = 1 ;
        }
        else if ( f < *f_min ) {
            if ( i_min != NULL ) 
                *i_min = i ;
            *f_min = f ;
            status = -1 ;
        }
    }

    f_prev = f ;
    if ( b[(d-1)*(dim+1)+dim] < BBS_ZERO ) 
        RETURN ( FALSE ) ;
    f = b[(d-1)*(dim+1)+j]/b[(d-1)*(dim+1)+dim] ;
    interior_extr = interior_extr || 
        ( status == -1 && f > f_prev + BBS_TOL ) ||
        ( status == 1 && f < f_prev - BBS_TOL ) ;
    if ( f >= *f_max - BBS_TOL ) {
        if ( i_max != NULL ) 
            *i_max = d-1 ;
        *f_max = f ;
    }
    else if ( f <= *f_min + BBS_TOL ) {
        if ( i_min != NULL ) 
            *i_min = d-1 ;
        *f_min = f ;
    }
    if ( i_min != NULL && i_max != NULL &&
        ( ( *i_min == 0 && *i_max == d-1 ) || 
          ( *i_max == 0 && *i_min == d-1 ) ) )
        interior_extr = FALSE ;
    RETURN ( interior_extr ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN fna_max ( b, d, f_max, i_max )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HREAL   *b      ;   /* Array of homogeneous numbers */
INT     d       ;   /* Order */
REAL    *f_max  ;   /* Max. number */
INT     *i_max  ;   /* Index of the max. number */
{
    INT i ;
    REAL f ;

    if ( b[0][1] < BBS_ZERO ) 
        RETURN ( FALSE ) ;
    *f_max = b[0][0]/b[0][1] ;
    *i_max = 0 ;
    for ( i=1 ; i<d-1 ; i++ ) {
        if ( b[i][1] < BBS_ZERO ) 
            RETURN ( FALSE ) ;
        f = b[i][0]/b[i][1] ;
        if ( f > *f_max ) {
            *i_max = i ;
            *f_max = f ;
        }
    }
    if ( b[d-1][1] < BBS_ZERO ) 
        RETURN ( FALSE ) ;
    f = b[d-1][0]/b[d-1][1] ;
    if ( f >= *f_max ) {
        *i_max = d-1 ;
        *f_max = f ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BOOLEAN fna_min ( b, d, f_min, i_min )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HREAL   *b      ;   /* Array of homogeneous numbers */
INT     d       ;   /* Order */
REAL    *f_min  ;   /* Min. number */
INT     *i_min  ;   /* Index of the min. number */
{
    INT i ;
    REAL f ;

    if ( b[0][1] < BBS_ZERO ) 
        RETURN ( FALSE ) ;
    *f_min = b[0][0]/b[0][1] ;
    *i_min = 0 ;

    for ( i=1 ; i<d ; i++ ) {
        if ( b[i][1] < BBS_ZERO ) 
            RETURN ( FALSE ) ;
        f = b[i][0]/b[i][1] ;
        if ( f < *f_min ) {
            *i_min = i ;
            *f_min = f ;
        }
    }
    if ( b[d-1][1] < BBS_ZERO ) 
        RETURN ( FALSE ) ;
    f = b[d-1][0]/b[d-1][1] ;
    if ( f <= *f_min ) {
        *i_min = d-1 ;
        *f_min = f ;
    }
    RETURN ( TRUE ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void fna_copy ( a, d, b )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HREAL   *a, *b  ;   /* Arrays of homogeneous numbers */
INT     d       ;   /* Order */
{
    INT i ;
    for ( i=0 ; i<d ; i++ ) {
        b[i][0] = a[i][0] ;
        b[i][1] = a[i][1] ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT fna_monotonic ( a, d, dim, j )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL   *a ;    /* Array of homogeneous numbers */
INT     d  ;    /* Order */
INT     dim  ;  /* Dimension */
INT     j  ;    /* Coordinate index */
{
    INT mon, i, k=0 ;
    REAL diff ;

    mon = 0 ;
    for ( i=1; i<d && !mon ; i++ ) {
        diff = a[i*(dim+1)+j]*a[(i-1)*(dim+1)+dim] - 
            a[(i-1)*(dim+1)+j]*a[i*(dim+1)+dim] ;
        if ( diff > BBS_TOL ) {
            mon = 1 ;
            k = i ;
        }
        else if ( diff < -BBS_TOL ) {
            mon = -1 ;
            k = i ;
        }
    }
    
    for ( i=k+1; i<d ; i++ ) {
        diff = a[i*(dim+1)+j]*a[(i-1)*(dim+1)+dim] - 
               a[(i-1)*(dim+1)+j]*a[i*(dim+1)+dim] ;
        if ( diff > BBS_TOL ) {
            if ( mon == -1 ) 
                RETURN ( 0 ) ;
        }
        else if ( diff < -BBS_TOL ) {
            if ( mon == 1 ) 
                RETURN ( 0 ) ;
        }
    }
    RETURN ( mon ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN fna_bounds ( a, d, dim, j, a_mon, low_bnd, low_i, 
            up_bnd, up_i )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *a ;
INT d;
INT dim  ;  /* Dimension */
INT j  ;    /* Coordinate index */
INT a_mon ;
REAL *low_bnd ;
INT *low_i ;
REAL *up_bnd ;
INT *up_i ;
{
    REAL z ;
    INT i ;

    if ( a[dim] <= BBS_ZERO || a[d*(dim+1)-1] <= BBS_ZERO )
        RETURN ( FALSE ) ;

    if ( a_mon == 1 ) {
        *low_bnd = a[j] / a[dim] ;
        *up_bnd = a[(d-1)*(dim+1)+j] / a[d*(dim+1)-1] ;
        *low_i = 0 ;
        *up_i = d-1 ;
    }
    else if ( a_mon == -1 ) {
        *low_bnd = a[(d-1)*(dim+1)+j] / a[d*(dim+1)-1] ;
        *up_bnd = a[j] / a[dim] ;
        *low_i = d-1 ;
        *up_i = 0 ;
    }
    else {
        *low_bnd = *up_bnd = a[j] / a[dim] ;
        *low_i = *up_i = 0 ;
    
        for ( i=1 ; i<d ; i++ ) {
            if ( a[i*(dim+1)+dim] <= BBS_ZERO ) {
                if ( a[i*(dim+1)+j] > 0.0 )
                    *up_i = i ;
                else 
                    *low_i = i ;
                RETURN ( FALSE ) ;
            }
            z = a[i*(dim+1)+j] / a[i*(dim+1)+dim] ;
            if ( z < *low_bnd ) {
                *low_bnd = z ;
                *low_i = i ;
            }
            else if ( z > *up_bnd ) {
                *up_bnd = z ;
                *up_i = i ;
            }
        }
    }
    RETURN ( TRUE ) ;
}
#endif   /*SPLINE*/

