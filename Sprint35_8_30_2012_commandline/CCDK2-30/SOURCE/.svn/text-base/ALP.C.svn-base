/* -2 -3 */
/********************************** ALP.C **********************************/
/** Polynomial equation solvers for up to the fifth order (fourth degree) **/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alpdefs.h>

#define  CUB_ROOT(V) { if ( IS_ZERO(V) ) V = 0.0 ; \
            else if ( V > 0.0 ) V = exp ( log (V) / 3.0 ) ; \
            else V = - exp ( log ( -V ) / 3.0 ) ; }

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT alp_solve2 ( a0, a1, t, mult ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/******************  input **************/
REAL    a0, a1  ;   /* a0*t + a1 = 0 */
/****************** output **************/
REAL    *t  ;       /* root(s) */
INT     *mult   ;   /* multiplicity of the root(s) */
{ 
    if ( mult != NULL ) 
        mult[0] = 0 ;
    if ( IS_SMALL(a0) )
        RETURN ( IS_SMALL (a1) ? -1 : 0 );

    if ( mult != NULL ) 
        mult[0] = 1 ; 
    t[0] = -a1 / a0 ; 
    RETURN ( 1 ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT alp_solve3 ( a0, a1, a2, t, mult ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/******************  input **************/
REAL a0, a1, a2 ;   /* a0ùtý+a1ùt+a2 = 0 */
/****************** output **************/
REAL    *t  ;   /* root(s) */
INT *mult   ;   /* multiplicity of the root(s) */
{ 
    REAL a, q, r ;
    
    mult[0] = 0 ;   /* initialization */
    mult[1] = 0 ;

    a = fabs(a0) + fabs(a1) + fabs(a2) ; 
    if ( IS_SMALL(a) )
        RETURN ( -1 ) ;
    a0 /= a ;
    a1 /= a ;
    a2 /= a ;

    if ( IS_ZERO(a0) )  /* Degenerates into linear equation */
        RETURN ( alp_solve2 ( a1, a2, t, mult ) ) ;

    if ( IS_ZERO(a1) )  {

        if ( IS_ZERO(a2) )  {
            mult[0] = 2 ; 
            t[0] = 0.0 ; 
            RETURN ( 1 ) ;
        }

        else if ( (a2>0.0) != (a0>0.0) ) {
            q = sqrt ( - a2 / a0 ) ;
            mult[0] = 1 ;
            mult[1] = 1 ;
            t[0] = - q ; 
            t[1] = q ; 
            RETURN ( 2 ) ;
        }
        else
            RETURN ( 0 ) ;
    }

    r = 0.5 * a1 ; 
    q = r * r - a0 * a2 ; 
    if ( fabs(q) < 0.5 * fabs(a0) * BBS_ZERO ) {
        mult[0] = 2 ; 
        t[0] = - r / a0 ; 
        RETURN ( 1 ) ;
    }

    else if ( q < 0.0 ) 
        RETURN ( 0 ) ;

    /* q > 0 */
    q = sqrt ( q ) ;
    mult[0] = 1 ;
    mult[1] = 1 ;
    /* The smaller root comes first */
    if ( a0 > 0.0 ) {
        t[0] = ( -r - q ) / a0 ; 
        t[1] = ( -r + q ) / a0 ; 
    }
    else {
        t[0] = ( -r + q ) / a0 ; 
        t[1] = ( -r - q ) / a0 ; 
    }
    RETURN ( 2 ) ; 
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT alp_quad_eqn ( a0, a1, a2, t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/******************  input **************/
REAL a0, a1, a2 ;   /* a0ùtý+a1ùt+a2 = 0 */
/****************** output **************/
REAL    *t  ;   /* root(s) */
{ 
    REAL a, q, r ;

    a = fabs(a0) + fabs(a1) + fabs(a2) ; 
    if ( IS_SMALL(a) )
        RETURN ( -1 ) ;
    if ( IS_SMALL(a0) )  /* Degenerates into linear equation */
        RETURN ( alp_solve2 ( a1, a2, t, NULL ) ) ;

    r = 0.5 * a1 ; 
    q = r * r - a0 * a2 ; 
    q = q > 0.0 ? sqrt ( q ) : 0.0 ;
    if ( a0 > 0.0 ) {
        t[0] = ( -r - q ) / a0 ; 
        t[1] = ( -r + q ) / a0 ; 
    }
    else {
        t[0] = ( -r + q ) / a0 ; 
        t[1] = ( -r - q ) / a0 ; 
    }
    RETURN ( 2 ) ; 
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT alp_ratio_extrs ( a0, a1, a2, b0, b1, b2, t, mult ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/******************  input **************/
REAL a0, a1, a2, b0, b1, b2 ;   /* ( a0ùtý+a1ùt+a2 )/( b0ùtý+b1ùt+b2 ) */
/****************** output **************/
REAL    *t  ;   /* root(s) */
INT *mult   ;   /* multiplicity of the root(s) */
{
    REAL a, b, c0, c1, c2 ;
    a = fabs(a0) + fabs(a1) + fabs(a2) ; 
    a0 /= a ;
    a1 /= a ;
    a2 /= a ;
    b = fabs(b0) + fabs(b1) + fabs(b2) ; 
    b0 /= b ;
    b1 /= b ;
    b2 /= b ;
    c0 = a1 * b0 - a0 * b1 ;
    c1 = 2.0 * ( a2 * b0 - a0 * b2 ) ;
    c2 = a2 * b1 - a1 * b2 ;
    RETURN ( alp_solve3 ( c0, c1, c2, t, mult ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN alp_ratio_min ( a0, a1, a2, b0, b1, b2, t_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/******************  input **************/
REAL a0, a1, a2, b0, b1, b2 ;   /* ( a0ùtý+a1ùt+a2 )/( b0ùtý+b1ùt+b2 ) */
/****************** output **************/
REAL    *t_ptr ;
{
    REAL f, f_min = 0.0, t[2] ;
    INT i, n, mult[2] ;
    BOOLEAN status = FALSE ;

    n = alp_ratio_extrs ( a0, a1, a2, b0, b1, b2, t, mult ) ;
    for ( i=0 ; i<n ; i++ ) {
        if ( t[i]>=-BBS_ZERO && t[i]<=1.0+BBS_ZERO ) {
            f = ( (a0*t[i]+a1)*t[i]+a2 ) / ( (b0*t[i]+b1)*t[i]+b2 ) ;
            if ( !status || f < f_min ) {
                f_min = f ;
                *t_ptr = t[i] ;
                status = TRUE ;
            }
        }
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT alp_solve_bez3 ( b0, b1, b2, t, mult ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/******************  input **************/
REAL b0, b1, b2 ;   /* b0ù(1-t)ý+2úb1ùtú(1-t)+b2útý = 0 */
/****************** output **************/
REAL    *t  ;   /* root(s) */
INT *mult   ;   /* multiplicity of the root(s) */
{ 
    REAL b, q, zero = 10.0 * BBS_ZERO ;
    
    mult[0] = 0 ;   /* initialization */
    mult[1] = 0 ;

    b = fabs(b0) + fabs(b1) + fabs(b2) ; 
    if ( IS_SMALL(b) )
        RETURN ( -1 ) ;
    b0 /= b ;
    b1 /= b ;
    b2 /= b ;

    if ( fabs(b0) <= zero ) {
        if ( fabs(b1) <= zero ) {
            mult[0] = 2 ; 
            t[0] = 0.0 ; 
            RETURN ( 1 ) ;
        }
        else {
            b = 2.0 * b1 - b2 ;
            if ( fabs(b) <= zero ) {
                mult[0] = 1 ; 
                t[0] = 0.0 ; 
                RETURN ( 1 ) ;
            }
            else { 
                mult[0] = 1 ; 
                mult[1] = 1 ; 
                q = 2.0 * b1 / b ; 
                if ( q > 0.0 ) {
                    t[0] = 0.0 ; 
                    t[1] = q ; 
                }
                else {
                    t[0] = q ; 
                    t[1] = 0.0 ; 
                }
                RETURN ( 2 ) ;
            }
        }
    }

    else if ( fabs(b2) <= zero ) {
        if ( fabs(b1) <= zero ) {
            mult[0] = 2 ; 
            t[0] = 1.0 ; 
            RETURN ( 1 ) ;
        }
        else {
            b = b0 - 2.0 * b1 ;
            if ( fabs(b) <= zero ) {
                mult[0] = 1 ; 
                t[0] = 1.0 ; 
                RETURN ( 1 ) ;
            }
            else { 
                mult[0] = 1 ; 
                mult[1] = 1 ; 
                q = b0 / b ; 
                if ( q > 1.0 ) {
                    t[0] = 1.0 ; 
                    t[1] = q ; 
                }
                else {
                    t[0] = q ; 
                    t[1] = 1.0 ; 
                }
                RETURN ( 2 ) ;
            }
        }
    }
    else
        RETURN ( alp_solve3 ( b2-2.0*b1+b0, 2.0*(b1-b0), b0, t, mult ) ) ;
}


#ifdef  SPLINE
#ifdef NEW_CODE
/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT alp_solve4 ( a0, a1, a2, a3, t, mult ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/


/******************  input **************/
REAL a0, a1, a2, a3 ;   /* a0*t**3 + a1*t**2 + a2*t + a3 = 0 */
/****************** output **************/
REAL    *t  ;   /* root(s) */
INT *mult   ;   /* multiplicity of the root(s) */
{ 
    REAL q, r, s, c1, c2, c3, c1_sq, v, theta, v1, v2 ;

    mult[0] = 0 ;
    mult[1] = 0 ;
    mult[2] = 0 ;

    if ( IS_SMALL (a0) ) 
        RETURN ( alp_solve3 ( a1, a2, a3, t, mult ) ) ;
    
    c1 = a1 / ( 3.0 * a0 ) ; 
    c2 = a2 / a0 ; 
    c3 = a3 / a0 ; 
    c1_sq = c1 * c1 ; 
    r = c2 / 3.0 - c1_sq ; 
    q = c1 * ( c1_sq - 0.5*c2 ) + 0.5*c3 ;
    s = r*r*r + q*q ;
    
    if ( fabs(s) <= BBS_ZERO*BBS_ZERO ) { /* At least two equal roots */

        if ( IS_ZERO(r) ) { /* Three equal roots */
            mult[0] = 3 ;
            t[0] = - c1 ; 
            RETURN ( 1 ) ;
        }

        t[1] = ( ( c1 * c2 / 3.0 - c3 ) * 0.5 ) / r ;
        t[0] = -2.0 * t[1] - 3.0 * c1 ;
        if ( t[0] < t[1] ) 
            {
            mult[0] = 1 ;
            mult[1] = 2 ;
            }
        else
            {
            AL_SWAP ( t[0], t[1] ) ;
            mult[0] = 2 ;
            mult[1] = 1 ;
            }
        RETURN ( 2 ) ;
    }

    else if ( s > 0.0 ) {   /* one root */
        s = sqrt ( s ) ;
        v1 = - q +  s ;
        v2 = - q - s ;
        CUB_ROOT ( v1 ) ;
        CUB_ROOT ( v2 ) ;
        t[0] = v1 + v2 - c1 ;
        mult[0] = 1 ;
        RETURN ( 1 ) ;
    }

            /* s < 0.0 */
    v = 2.0 * sqrt ( - r ) ;
    theta = acos ( 2.0 * q / ( v * r ) ) / 3.0 ;
    v1 = cos ( theta ) ; 
    v2 = SQRT_3 * sin ( theta ) ; 
    t[0] = v * v1 - c1 ;
    t[1] = 0.5 * v * ( v2 - v1 ) - c1 ;
    t[2] = 0.5 * v * ( -v2 - v1 ) - c1 ; 
    if ( t[0] > t[1] ) 
        AL_SWAP ( t[0], t[1] ) ; 
    if ( t[1] > t[2] ) 
        AL_SWAP ( t[1], t[2] ) ; 
    if ( t[0] > t[1] ) 
        AL_SWAP ( t[0], t[1] ) ; 
    mult[0] = mult[1] = mult[2] = 1 ;
    RETURN ( 3 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT alp_solve5 ( a0, a1, a2, a3, a4, t, mult ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/


/******************  input **************/
REAL a0, a1, a2, a3, a4 ;   /* a0*t**4 + a1*t**3 + a2*t**2 + a3*t + a4 = 0 */
/****************** output **************/
REAL    *t  ;   /* root(s) */
INT *mult   ;   /* multiplicity of the root(s) */
{ 

    REAL c0, c1, c2, c3, m, n, t1[2], t2[2] ;
    INT mult1[2], mult2[2], i1, i2, i, m0, m1, m2 ;
    
    mult[0] = 0 ;
    mult[1] = 0 ;
    mult[2] = 0 ;
    mult[3] = 0 ;
    
    if ( IS_SMALL(a0) ) 
        RETURN ( alp_solve4 ( a1, a2, a3, a4, t, mult ) ) ;
    
    /* Compute coefficients of the cubic equation */
    c0 = a0 * a0 ;
    c1 = - a2 * a0 ;
    c2 = a1 * a3 - 4.0 * a4 * a0 ;
    c3 = a4 * ( 4.0 * a2 - a1 * a1 / a0 ) - a3 * a3 ; 
    
    m1 = alp_solve4 ( c0, c1, c2, c3, t, mult ) ;
    if ( m1 <= 0 ) 
        RETURN ( m1 ) ; 

    m = a1 * a1 / ( 4.0 * a0 * a0 ) - a2 / a0 + t[0] ;
    if ( m > 0.0 ) 
        m = sqrt ( m ) ; 
    else if ( m >= - BBS_ZERO*BBS_ZERO ) 
        m = 0.0 ;
    else
        RETURN ( 0 ) ; 

    n = 0.25 * t[0] * t[0] - a4 / a0 ; 
    if ( n > 0.0 ) 
        n = sqrt ( n ) ; 
    else if ( n >= - BBS_ZERO ) 
        n = 0.0 ; 
    else 
        RETURN ( 0 ) ;

    if ( ( a1 * t[0] - 2.0 * a3 ) * a0 > 0.0 ) { 
        if ( ( m >= 0.0 ) != ( n >= 0.0 ) ) 
            n = - n ;
    }
    else { 
        if ( ( m >= 0.0 ) == ( n >= 0.0 ) )
            n = - n ;
    }
    
    c0 = 1.0 ;
    c1 = 0.5 * a1 / a0 - m ;
    c2 = 0.5 * t[0] - n ;
    
    m1 = alp_solve3 ( c0, c1, c2, t1, mult1 ) ;

    c1 += ( 2.0 * m ) ;
    c2 += ( 2.0 * n ) ;
    
    m2 = alp_solve3 ( c0, c1, c2, t2, mult2 ) ;
    
    m0 = 0 ;
    i1 = 0 ;
    i2 = 0 ; 
    
    while ( ( i1 < m1 ) && ( i2 < m2 ) ) {
        if ( IS_ZERO ( t1[i1] - t2[i2] ) ) {
            t[m0] = t1[i1] ;
            mult[m0] = mult1[i1] + mult2[i2] ;
            i1++ ;
            i2++ ;
        }
        else if ( t1[i1] < t2[i2] ) {
            t[m0] = t1[i1] ;
            mult[m0] = mult1[i1] ;
            i1++ ;
        }
        else {
            t[m0] = t2[i2] ;
            mult[m0] = mult2[i2] ;
            i2++ ;
        }
        m0++ ;
    }
    
    if ( i1 >= m1 ) { /* no more roots of the first equation */
        for ( i=i2 ; i < m2 ; i++ ) {
            t[m0] = t2[i] ;
            mult[m0] = mult2[i] ;
            m0++ ;
        }
    }
    else if ( i2 >= m2 ) { /* no more roots of the second equation */
        for ( i=i1 ; i < m1 ; i++ ) {
            t[m0] = t1[i] ;
            mult[m0] = mult1[i] ;
            m0++ ;
        }
    }
    RETURN ( m0 ) ;
}
#endif  /*NEW_CODE*/
#endif  /*SPLINE*/

