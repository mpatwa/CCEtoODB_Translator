/* -2 -3 */
/*********************************** C2BL.C ********************************/
/******* Intersection of a two-dimensional Bezier curve and a line *********/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <bbsdefs.h>
#ifdef  SPLINE
#include <c2bdefs.h>
#include <fnbdefs.h>
#include <c2coned.h>

STATIC INT  inters_coord_line __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 , REAL DUMMY4 , INT DUMMY5 , REAL DUMMY6 , 
            INT DUMMY7 , INT DUMMY8 , DML_LIST DUMMY9 )) ;
STATIC INT  inters_coord_line_mult __(( HPT2* DUMMY0 , INT DUMMY1 , 
            REAL DUMMY2 , INT DUMMY3 , REAL DUMMY4 , REAL* DUMMY5 )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2b_inters_coord_line ( a, d, w, t0, t1, index, 
            z, j, dir, inters_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
INT d ;
REAL w ;
REAL t0, t1 ;
INT index ;
REAL z ;
INT j ;
INT dir ;
DML_LIST inters_list ;
{
    RETURN ( inters_coord_line ( a, d, w, t0, t1, index, 
        z, j, dir, inters_list ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT inters_coord_line ( b, d, w, t0, t1, index, z, j, dir, inters_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *b ;
INT d ;
REAL w ;
REAL t0, t1 ;
INT index ;
REAL z ;
INT j ;
INT dir ;
DML_LIST inters_list ;
{
    INT mon, m, n, i ;
    REAL f, f_min, f_max, t ;
    PT2 pt0, pt1 ;
    HPT2 *bl, *br ;

    mon = fnb_coord_break_parm ( (REAL*)b, d, 2, j, &f_min, &f_max, &t ) ;

    if ( ( f_min > z + fabs(w) + BBS_TOL ) || 
        ( f_max < z - fabs(w) - BBS_TOL ) ) 
        RETURN ( 0 ) ;

    if ( mon == 1 || mon == -1 ) {

        if ( c2b_ept0 ( b, d, w, pt0 ) && IS_SMALL ( pt0[j] - z ) ) {
        /* ept0 is a root ; find its multiplicity */
            m = 1 ; 
            f = b[0][j] / b[0][2] ;
            for ( i=1 ; i<d ; i++ ) {
                if ( IS_SMALL ( b[i][j] / b[i][2] - f ) )
                    m++ ;
                else
                    break ;
            }
            if ( ( dir==1 && pt0[1-j] < - BBS_TOL ) || 
                    ( dir==-1 && pt0[1-j] > BBS_TOL ) )
                RETURN ( 0 ) ;
            RETURN ( c2b_append_inters_rec ( inters_list, 0.0, t0, t1, index, 
                    pt0[1-j], 0.0, 1.0, 1, pt0, fabs(pt0[j]-z), m ) ) ;
        }

        if ( c2b_ept1 ( b, d, w, pt1 ) && IS_SMALL ( pt1[j] - z ) ) {
        /* ept1 is a root ; find its multiplicity */
            m = 1 ; 
            f = b[d-1][j] / b[d-1][2] ;
            for ( i=d-2 ; i>=0 ; i-- ) {
                if ( IS_SMALL ( b[i][j] / b[i][2] - f ) )
                    m++ ;
                else
                    break ;
            }
            if ( ( dir==1 && pt1[1-j] < - BBS_TOL ) || 
                    ( dir==-1 && pt1[1-j] > BBS_TOL ) )
                RETURN ( 0 ) ;
            RETURN ( c2b_append_inters_rec ( inters_list, 1.0, t0, t1, index, 
                    pt1[1-j], 0.0, 1.0, 1, pt1, fabs(pt1[j]-z), m ) ) ;
        }

        if ( ( mon==1 && ( pt0[j] > BBS_TOL || pt1[j] < - BBS_TOL ) ) || 
            ( mon==-1 && ( pt0[j] < -BBS_TOL || pt1[j] > BBS_TOL ) ) )
                RETURN ( 0 ) ;

        if ( c2b_int_coord_line_iter ( b, d, w, j, z, &t ) ) {
            c2b_eval ( b, d, w, t, 0, (PT2*)pt0 ) ;
            if ( ( dir==1 && pt0[1-j] < - BBS_TOL ) || 
                    ( dir==-1 && pt0[1-j] > BBS_TOL ) )
                RETURN ( 0 ) ;
            RETURN ( c2b_append_inters_rec ( inters_list, t, t0, t1, index, 
                    pt0[1-j], 0.0, 1.0, 1, pt0, fabs(pt0[j]-z), 1 ) ) ;
        }
    }

    if ( mon == -2 || fabs ( f_max - f_min ) > sqrt ( BBS_TOL ) ) {

        bl = CREATE ( 2*d-1, HPT2 ) ;
        br = bl + d - 1 ;
        C2B_BRK0 ( b, d, t, bl ) ;
        t = t0 * ( 1.0 - t ) + t1 * t ;
        n = inters_coord_line ( bl, d, w, t0, t, index, 
                z, j, dir, inters_list ) + 
            inters_coord_line ( br, d, w, t, t1, index, 
                z, j, dir, inters_list ) ;
        KILL ( bl ) ;
        RETURN ( n ) ;
    }

    else {
        if ( c2b_ept0 ( b, d, w, pt0 ) && c2b_ept1 ( b, d, w, pt1 ) && 
            IS_SMALL ( pt0[j] - z ) ) { 
            if ( ( dir==1 && pt0[1-j] < - BBS_TOL && pt1[1-j] < - BBS_TOL ) || 
                 ( dir==-1 && pt0[1-j] > BBS_TOL && pt1[1-j] > BBS_TOL ) )
                RETURN ( 0 ) ;
            RETURN ( c2b_append_inters_rec ( inters_list, 0.0, t0, t1, index, 
                    pt0[1-j], 0.0, 1.0, 1, pt0, fabs(pt0[j]-z), -1 ) +
                 c2b_append_inters_rec ( inters_list, 0.0, t0, t1, index, 
                    pt0[1-j], 0.0, 1.0, 1, pt0, fabs(pt0[j]-z), -1 ) ) ;
        }
        else {
            n = inters_coord_line_mult ( b, d, w, j, z, &t ) ;
            if ( n <= 0 )
                RETURN ( 0 ) ;
            c2b_eval ( b, d, w, t, 0, (PT2*)pt0 ) ;
            if ( ( dir==1 && pt0[1-j] < - BBS_TOL ) || 
                 ( dir==-1 && pt0[1-j] > BBS_TOL ) )
                RETURN ( 0 ) ;

            RETURN ( c2b_append_inters_rec ( inters_list, t, t0, t1, index, 
                    pt0[1-j], 0.0, 1.0, 1, pt0, fabs(pt0[j]-z), n ) ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
STATIC INT inters_coord_line_mult ( b, d, w, j, z, t_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *b      ;   
INT     d       ;   
REAL    w       ;   
INT     j       ;   
REAL    z       ;   
REAL    *t_ptr  ;   /* Root */
{
    REAL    dt, h0, h1, r, r0, t_best=0.0, x_best=0.0 ;
    PT2     x[3] ;
    INT     count, mult=0 ;

    *t_ptr = 0.5 ;
    r0 = -1.0 ;
    dt = 1.0 ;

    for ( count=0 ; count<MAX_COUNT ; count++ ) {
        c2b_eval ( b, d, w, *t_ptr, 2, x ) ;
        if ( count==0 || fabs(x[0][j]-z) <= fabs(x_best-z) ) {
            t_best = *t_ptr ;
            x_best = x[0][j] ;
        }

        if ( IS_SMALL(x[1][j]) || IS_SMALL(x[2][j]) ) {
            if ( IS_SMALL(x_best-z) ) {
                *t_ptr = t_best ;
                RETURN ( mult ) ;
            }
            else 
                RETURN ( -1 ) ;
        }
        h0 = ( x[0][j] - z ) / x[1][j] ;
        h1 = x[1][j] / x[2][j] ;
        r = h1 / ( h1 - h0 ) ;
        if ( fabs(dt) <= 1e-8 && fabs(r-r0) >= 0.2 ) {
            if ( IS_SMALL(x_best-z) ) {
                *t_ptr = t_best ;
                RETURN ( mult ) ;
            }
            else 
                RETURN ( 0 ) ;
        }
        r0 = r ;
        if ( count==0 ) 
            mult = (INT) ( r + 0.5 ) ;
        dt = - h0 * r ;
        *t_ptr += dt ;
        if ( *t_ptr > 1.0 ) 
            *t_ptr = 1.0 ;
        else if ( *t_ptr < 0.0 ) 
            *t_ptr = 0.0 ;
        if ( IS_ZERO(dt) ) 
            RETURN ( mult ) ;
    }
    RETURN ( 0 ) ;
}        
#endif  /*SPLINE*/

