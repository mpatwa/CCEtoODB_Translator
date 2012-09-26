/* -2 -3 */
/********************************** C2SD.C *********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <alsdefs.h>
#include <c2bdefs.h>
#include <c2rdefs.h>
#include <c2pdefs.h>
#include <c2sdefs.h>
#include <c2vmcrs.h>
#include <c2hmcrs.h>

STATIC  BOOLEAN c2sd_tan_vec_parab __(( PT2, PT2, PT2, REAL, REAL, REAL, 
            PT2 )) ;
STATIC  void    c2sd_tan_vec_magnitude __(( PT2, PT2, PT2, PT2 )) ;
STATIC  BOOLEAN c2sd_start_tangent __(( PT2*, INT, REAL*, PT2, C2_TAN_OPTIONS, 
            PT2 )) ;
STATIC  BOOLEAN c2sd_end_tangent __(( PT2*, INT, REAL*, PT2, C2_TAN_OPTIONS, 
            PT2 )) ;
STATIC  INT     c2s_arc __(( C2_ASEG, HPT2* )) ;

/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c2sd_tan_vec_parab ( pt0, pt1, pt2, t0, t1, t2, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1, pt2 ; /* interpolating points */
REAL t0, t1, t2 ;   /* knots */
PT2 tan_vec    ;    /* computed tangent vector */
{
    REAL h10, h21, h20, u, v ;
    
    h10 = t1 - t0 ;
    h21 = t2 - t1 ;
    if ( ( IS_ZERO(h10) ) || ( IS_ZERO(h21) ) )
        RETURN ( FALSE ) ;
    h20 = t2 - t0 ;
    u = h20 / ( h10 * h21 ) ;
    v = h10 / ( h20 * h21 ) ;
    tan_vec[0] = ( pt1[0] - pt0[0] ) * u - ( pt2[0] - pt0[0] ) * v ;
    tan_vec[1] = ( pt1[1] - pt0[1] ) * u - ( pt2[1] - pt0[1] ) * v ;
    RETURN ( TRUE ) ;
}    


/*-------------------------------------------------------------------------*/
STATIC void c2sd_tan_vec_magnitude ( pt0, pt1, dir_vec, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1 ; /* interpolating points */
PT2 dir_vec    ;    /* input tangent direction vector */
PT2 tan_vec    ;    /* output tangent direction vector */
{
    REAL t ;
    PT2 diff ;

    C2V_SUB ( pt1, pt0, diff ) ;
    t = C2V_DOT ( diff, dir_vec ) / C2V_DOT ( dir_vec, dir_vec ) ;
    C2V_SCALE ( dir_vec, t, tan_vec ) ;
}    


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c2sd_start_tangent ( a, n, knot, tan0, tan0_options, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a      ;   /* a[n-2] */
INT n ;
REAL *knot  ;   /* knot[n+4] */
PT2 tan0    ;   /* input (if any) atngent vector */
C2_TAN_OPTIONS tan0_options ;
PT2 tan_vec ;
{
    if ( tan0_options == C2_NDEF_TAN ) {
        if ( n==4 ) {
            C2V_SUB ( a[1], a[0], tan_vec ) ;
            RETURN ( TRUE ) ;
        }
        else 
            RETURN ( c2sd_tan_vec_parab ( a[0], a[1], a[2], 
                    knot[3], knot[4], knot[5], tan_vec ) ) ;
    }
    else if ( tan0_options == C2_DEF_TAN ) {
        c2sd_tan_vec_magnitude ( a[0], a[1], tan0, tan_vec ) ;
        RETURN ( TRUE ) ;
    }
    else if ( tan0_options == C2_DEF_TAN_MAGN ) {
        C2V_COPY ( tan0, tan_vec ) ;
        RETURN ( TRUE ) ;
    }
    else 
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c2sd_end_tangent ( a, n, knot, tan1, tan1_options, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a      ;   /* a[n-2] */
INT n ;
REAL *knot  ;   /* knot[n+4] */
PT2 tan1    ;   /* input (if any) atngent vector */
C2_TAN_OPTIONS tan1_options ;
PT2 tan_vec ;
{
    if ( tan1_options == C2_NDEF_TAN ) {
        if ( n==4 ) {
            C2V_SUB ( a[1], a[0], tan_vec ) ;
            RETURN ( TRUE ) ;
        }
        else 
            RETURN ( c2sd_tan_vec_parab ( a[n-3], a[n-4], a[n-5], 
                    knot[n], knot[n-1], knot[n-2], tan_vec ) ) ;
    }
    else if ( tan1_options == C2_DEF_TAN ) {
        c2sd_tan_vec_magnitude ( a[n-4], a[n-3], tan1, tan_vec ) ;
        RETURN ( TRUE ) ;
    }
    else if ( tan1_options == C2_DEF_TAN_MAGN ) {
        C2V_COPY ( tan1, tan_vec ) ;
        RETURN ( TRUE ) ;
    }
    else 
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2s_interp_knots ( a, n, knot_options, knot )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a      ;   /* a[n-2] */
INT n ;
C2_KNOT_OPTIONS knot_options ;
REAL *knot  ;   /* knot[n+4] */
{

    REAL c ;
    INT i ;
    PT2 vec ;

    if ( knot_options == C2_DEFAULT ) {
        knot[0] = knot[1] = knot[2] = knot[3] = 0.0 ;
        for ( i=4 ; i<=n ; i++ ) {
            C2V_SUB ( a[i-3], a[i-4], vec ) ;
            c = C2V_NORM ( vec ) ;
            if ( c <= BBS_ZERO )
                RETURN ( FALSE ) ;
            knot[i] = knot[i-1] + sqrt ( c ) ;
        }
        c = (REAL)(n-3) / knot[n] ;
        for ( i=4 ; i<=n ; i++ )
            knot[i] *= c ;
        knot[n+1] = knot[n+2] = knot[n+3] = knot[n] ;
        RETURN ( TRUE ) ;
    }
    else if ( knot_options == C2_UNIFORM ) {
        als_uniform_knots ( n, 4, knot ) ;
        RETURN ( TRUE ) ;
    }
    else if ( knot_options == C2_CLSC_UNI ) {
        als_clsc_uni_knots ( n, 4, knot ) ;
        RETURN ( TRUE ) ;
    }
    else if ( knot_options == C2_NONUNI ) {
        knot[0] = knot[1] = knot[2] = knot[3] = 0.0 ;
        for ( i=4 ; i<=n ; i++ ) {
            C2V_SUB ( a[i-3], a[i-4], vec ) ;
            c = C2V_NORM ( vec ) ;
            if ( c <= BBS_ZERO )
                RETURN ( FALSE ) ;
            knot[i] = knot[i-1] + c ;
        }
        c = (REAL)(n-3) / knot[n] ;
        for ( i=4 ; i<=n ; i++ )
            knot[i] *= c ;
        knot[n+1] = knot[n+2] = knot[n+3] = knot[n] ;
        RETURN ( TRUE ) ;
    }
    else if ( knot_options == C2_GIVEN ) 
        RETURN ( TRUE ) ;
    else
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2s_clsd_interp_knots ( a, n, knot_options, knot ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a      ;   /* a[n-3] */
INT n ;
C2_KNOT_OPTIONS knot_options ;
REAL *knot  ;   /* knot[n+4] */
{

    REAL c ;
    INT i ;
    PT2 vec ;

    if ( knot_options == C2_DEFAULT ) {
        knot[0] = knot[1] = knot[2] = knot[3] = 0.0 ;
        for ( i=4 ; i<n ; i++ ) {
            C2V_SUB ( a[i-3], a[i-4], vec ) ;
            c = C2V_NORM ( vec ) ;
            if ( c <= BBS_ZERO ) 
                RETURN ( FALSE ) ;
            knot[i] = knot[i-1] + sqrt ( c ) ;
        }
        C2V_SUB ( a[0], a[n-4], vec ) ;
        c = C2V_NORM ( vec ) ;
        if ( c <= BBS_ZERO ) 
            RETURN ( FALSE ) ;
        knot[n] = knot[n-1] + sqrt ( c ) ;
        
        c = (REAL)(n-3) / knot[n] ;
        for ( i=4 ; i<=n ; i++ ) 
            knot[i] *= c ;
        knot[n+1] = knot[n+2] = knot[n+3] = knot[n] ;
        RETURN ( TRUE ) ;
    }
    else if ( knot_options == C2_UNIFORM ) {
        knot[0] = -3.0 ;
        for ( i=1 ; i<n+4 ; i++ ) 
            knot[i] = knot[i-1] + 1.0 ;
        RETURN ( TRUE ) ;
    }   
    else if ( knot_options == C2_CLSC_UNI ) {
        knot[0] = knot[1] = knot[2] = knot[3] = 0.0 ;
        for ( i=4 ; i<=n ; i++ ) 
            knot[i] = knot[i-1] + 1.0 ;
        knot[n+1] = knot[n+2] = knot[n+3] = knot[n] ;
        RETURN ( TRUE ) ;
    }
    else if ( knot_options == C2_NONUNI ) {
        knot[0] = knot[1] = knot[2] = knot[3] = 0.0 ;
        for ( i=4 ; i<n ; i++ ) {
            C2V_SUB ( a[i-3], a[i-4], vec ) ;
            c = C2V_NORM ( vec ) ;
            if ( c <= BBS_ZERO )
                RETURN ( FALSE ) ;
            knot[i] = knot[i-1] + c ;
        }
        C2V_SUB ( a[0], a[n-4], vec ) ;
        c = C2V_NORM ( vec ) ;
        if ( c <= BBS_ZERO )
            RETURN ( FALSE ) ;
        knot[n] = knot[n-1] + c ;

        c = (REAL)(n-3) / knot[n] ;
        for ( i=4 ; i<=n ; i++ )
            knot[i] *= c ;
        knot[n+1] = knot[n+2] = knot[n+3] = knot[n] ;
        RETURN ( TRUE ) ;
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2s_interp_tan ( a, n, knot_options,
        tan0, tan0_options, tan1, tan1_options, c, knot ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a      ;   /* a[n-2] */
INT n ;
C2_KNOT_OPTIONS knot_options ;
PT2 tan0    ;
C2_TAN_OPTIONS tan0_options ;
PT2 tan1    ;
C2_TAN_OPTIONS tan1_options ;
PT2 *c     ;   /* c[n]  */
REAL *knot  ;   /* knot[n+4] */

{
    PT2 tan0_vec, tan1_vec ;

    RETURN ( c2s_interp_knots ( a, n, knot_options, knot ) &&
             c2sd_start_tangent ( a, n, knot, tan0, tan0_options, tan0_vec ) &&
             c2sd_end_tangent ( a, n, knot, tan1, tan1_options, tan1_vec ) &&
             als_interp_tan ( (REAL*)a, n, 2, knot, 
                    tan0_vec, tan1_vec, (REAL*)c ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2s_interp_clsd ( a, n, knot_options, c, knot ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a      ;   /* a[n-2] */
INT n ;
C2_KNOT_OPTIONS knot_options ;
PT2 *c     ;    /* c[n]  */
REAL *knot  ;   /* knot[n+4] */
{
    RETURN ( c2s_clsd_interp_knots ( a, n, knot_options, knot ) &&
             als_interp_clsd ( (REAL*)a, n, 2, knot, (REAL*)c ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2s_pcurve ( pcurve, a, knot ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
HPT2 *a      ;   /* a[2*n+1] */
REAL *knot  ;   /* knot[2*n+4] */
{
    INT i, j, k ;
    C2_ASEG arc ;

    knot[0] = 0.0 ;

    for ( i=0, j=0, arc = c2p_segment ( pcurve, 0 ) ; arc != NULL ; 
        i++, arc = c2p_segment ( pcurve, i ) ) {
        k = c2s_arc ( arc, a+j ) ;
        if ( k == 3 ) {
            knot[j+1] = knot[j+2] = (REAL)i ;
            j += 2 ;
        }
        else {
            knot[j+1] = knot[j+2] = (REAL)i ;
            knot[j+3] = knot[j+4] = (REAL)i+0.5 ;
            j += 4 ;
        }
    }
    knot[j+1] = knot[j+2] = knot[j+3] = (REAL)i ;
}


/*-------------------------------------------------------------------------*/
STATIC INT c2s_arc ( arc, a ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
HPT2 *a      ;   /* a[2*n+1] */
{
    HPT2 bez0[3] ;

    if ( fabs ( c2r_get_d ( arc ) ) <= 1.0 - BBS_ZERO ) {
        c2r_bez ( arc, a ) ;
        RETURN ( 3 ) ;
    }
    else {
        c2r_bez ( arc, bez0 ) ;
        C2B_SBDV0 ( bez0, 3, a ) ;
        RETURN ( 5 ) ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2s_ellipse ( ctr, major_axis, minor_axis, angle, a, knot ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr ;
REAL major_axis, minor_axis, angle ;
HPT2 *a ;
REAL *knot ;
{
    INT i ;
    PT2 p ;
    REAL c, s ;
    HPT2 b[3] ;

    C2H_SET ( major_axis, 0.0, 1.0, b[0] ) ;
    C2H_SET ( 0.0, minor_axis, 0.0, b[1] ) ;
    C2H_SET ( -major_axis, 0.0, 1.0, b[2] ) ;
    C2B_SBDV0 ( b, 3, a ) ;

    for ( i=4 ; i<9 ; i++ ) {
        C2V_NEGATE ( a[i-4], a[i] ) ;
        a[i][2] = a[i-4][2] ;
    }

    c = cos ( angle ) ;
    s = sin ( angle ) ;

    for ( i=0 ; i<9 ; i++ ) {
        C2V_ROTATE_VEC_CS ( a[i], c, s, p ) ;
        C2V_ADDT ( p, ctr, a[i][2], a[i] ) ;
    }
    als_ellipse_knots ( 12, knot ) ;
}

#endif  /*SPLINE*/

