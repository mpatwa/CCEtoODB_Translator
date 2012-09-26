/* -2 -3 */
/********************************** C2GI.C *********************************/
/****************** Routines for processing arc geometry *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alpdefs.h>
#include <c2gdefs.h>
#include <c2ldefs.h>
#include <c2rdefs.h>
#include <c2vdefs.h>
#include <c2vmcrs.h>
#include <c2gmcrs.h>
#include <c2lmcrs.h>
#ifdef DEBUG
#include <c2vdefs.h>
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif

STATIC BOOLEAN c2g_new_inters __(( REAL*, REAL*, INT, INT )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2g_int_line_arc_ext ( line, arc, t1, t2, pt, type, near_tan )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
C2_ARC arc ;
REAL *t1, *t2 ;
PT2 *pt ;
INT *type ;
BOOLEAN *near_tan ;
{
    C2_ASEG_S carc ;
    INT i, j, m, n ;
    PT2 p[2], vec, lvec, q, ctr ;
    REAL u, dist, diff, u1[2], u2[2], rad ;
    INT x[2] ;
    BOOLEAN status ;

#ifdef DDEBUG
{
PT2 tan, tan0, tan1 ;
REAL angle ;
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_curve ( line, 9 ) ;
    paint_curve ( arc, 10 ) ;
    getch();
}
else
    DISPLAY-- ;
C2_LINE_DIR_VEC ( line, tan ) ;
c2g_pt_tan ( arc, 0.0, NULL, tan0 ) ;
c2g_pt_tan ( arc, 1.0, NULL, tan1 ) ;
angle = c2v_vecs_angle ( tan, tan0 ) ;
angle = c2v_vecs_angle ( tan, tan1 ) ;
}
#endif

    if ( c2r_ctr ( C2_ARC_SEG(arc), ctr ) != NULL ) {
        rad = c2r_rad ( C2_ARC_SEG(arc) ) ;
        u = c2l_project ( line, ctr, q ) ;
        C2V_SUB ( C2_LINE_PT1(line), C2_LINE_PT0(line), lvec ) ;
        C2V_SUB ( q, ctr, vec ) ;
        dist = C2V_NORM ( vec ) ;
        diff = rad - dist ;
        if ( diff < -BBS_TOL ) {
#ifdef DDEBUG
    printf ( "diff = %.16lf\n", diff ) ;
#endif
            RETURN ( 0 ) ;
        }
#ifdef DEBUG
        u = diff * ( rad + dist ) / C2V_DOT ( lvec, lvec ) ;
        u = ( u > 0.0 ) ? sqrt(u) : 0.0 ;
#endif
        if ( diff > 0.0 ) 
            u = sqrt ( rad * rad - dist * dist ) / C2V_NORM ( lvec ) ;
        else
            u = 0.0 ;
        C2V_ADDT ( q, lvec, u, pt[0] ) ;
        C2V_ADDT ( q, lvec, -u, pt[1] ) ;
        dist = C2V_DIST ( pt[0], pt[1] ) ;
#ifdef DDEBUG
    printf ( "diff = %.16lf\t dist = %.16lf\n", diff, dist ) ;
#endif
        if ( dist <= BBS_TOL ) {
            n = 1 ;
            C2V_MID_PT ( pt[0], pt[1], pt[0] ) ;
            type[0] = 2 ;
            if ( near_tan != NULL ) 
                near_tan[0] = FALSE ;
        }
        else {
            n = 2 ;
            type[0] = 1 ;
            type[1] = 1 ;
            if ( near_tan != NULL ) {
                near_tan[0] = ( diff < BBS_TOL ) ;
                near_tan[1] = near_tan[0] ;
            }
        }
        for ( i = 0 ; i < n ; i++ ) {
            t1[i] = c2l_project ( line, pt[i], NULL ) ;
            c2g_project ( arc, 0.0, 2.0, pt[i], t2+i, NULL ) ;
        }
        RETURN ( n ) ;
    }

    n = c2r_inters_line ( C2_ARC_SEG(arc), line, t2, t1, pt, type ) ;
    if ( ( ( n == 0 ) || ( n == 1 && type[0] < 2 ) ) && 
        c2r_complement ( C2_ARC_SEG(arc), &carc ) != NULL ) {
/*
    if ( n < 2 && c2r_complement ( C2_ARC_SEG(arc), &carc ) != NULL ) {
*/
        m = c2r_inters_line ( &carc, line, u2, u1, p, x ) ;
        for ( i=0 ; i<m ; i++ ) {
            if ( u2[i] > BBS_ZERO && u2[i] < 1.0 - BBS_ZERO ) {
                status = TRUE ;
                for ( j=0 ; j<n-1 && status ; j++ ) {
                    if ( IS_ZERO ( t1[j] - u1[i] ) || 
                        IS_ZERO ( t2[j] - u2[i] - 1.0 ) ) 
                        status = FALSE ;
                }
                if ( status ) {
                    t1[n] = u1[i] ;
                    t2[n] = u2[i] + 1.0 ;
                    C2V_COPY ( p[i], pt[n] ) ;
                    type[n] = x[i] ;
                    n++ ;
                }
            }
        }
    }
    RETURN ( n ) ;
}   


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2g_int_line_arc ( line, t10, t11, arc, t20, t21, t1, t2, 
            pt, type ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
REAL t10, t11 ;
C2_ARC arc ;
REAL t20, t21 ;
REAL *t1, *t2 ;
PT2 *pt ;
INT *type ;
{
    INT i, m, n ;
#ifdef DDEBUG
PT2 tan, tan0, tan1 ;
REAL angle ;
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_curve ( line, 9 ) ;
    paint_curve ( arc, 10 ) ;
    getch();
}
else
    DISPLAY-- ;
C2_LINE_DIR_VEC ( line, tan ) ;
c2g_pt_tan ( arc, 0.0, NULL, tan0 ) ;
c2g_pt_tan ( arc, 1.0, NULL, tan1 ) ;
angle = c2v_vecs_angle ( tan, tan0 ) ;
angle = c2v_vecs_angle ( tan, tan1 ) ;
#endif
    if ( t20 >= -BBS_ZERO && t21 <= 1.0 + BBS_ZERO ) 
        m = c2r_inters_line ( C2_ARC_SEG(arc), line, t2, t1, pt, type ) ;
    else
        m = c2g_int_line_arc_ext ( line, arc, t1, t2, pt, type, NULL ) ;

    n = 0 ;
    for ( i = 0 ; i < m ; i++ ) {
        if ( t1[i] >= t10 - BBS_ZERO && t1[i] <= t11 + BBS_ZERO &&
             t2[i] >= t20 - BBS_ZERO && t2[i] <= t21 + BBS_ZERO ) {
            if ( i != n ) {
                t1[n] = t1[i] ;
                t2[n] = t2[i] ;
                C2V_COPY ( pt[i], pt[n] ) ;
                type[n] = type[i] ;
            }
            n++ ;
        }
    }
    RETURN ( n ) ;
}   

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2g_int_arc_arc_ext ( arc1, arc2, t1, t2, pt, type, near_tan ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc1, arc2 ;
REAL *t1, *t2 ;
PT2 *pt ;
INT *type ;
BOOLEAN *near_tan ;
{
    C2_ASEG_S carc1, carc2 ;
    BOOLEAN s1, s2 ;
    INT i, m, n = 0 ;
    PT2 ctr1, ctr2, vec, vec1, a, p[2] ;
    REAL rada;
    REAL radb ;
    REAL q1, q2, u, v, u1[2], u2[2] ;
    INT x[2] ;

    if ( !C2_ARC_ZERO_RAD(arc1) && !C2_ARC_ZERO_RAD(arc2) && 
        c2r_ctr ( C2_ARC_SEG(arc1), ctr1 ) != NULL && 
        c2r_ctr ( C2_ARC_SEG(arc2), ctr2 ) != NULL ) {

        rada = c2r_rad ( C2_ARC_SEG(arc1) ) ;
        radb = c2r_rad ( C2_ARC_SEG(arc2) ) ;
        C2V_SUB ( ctr2, ctr1, vec ) ;
        if ( C2V_IS_SMALL ( vec ) ) 
            RETURN ( IS_SMALL(rada-radb) ? -1 : 0 ) ;
        vec1[0] = vec[0] / rada ;
        vec1[1] = vec[1] / rada ;
        q1 = 1.0 / C2V_DOT ( vec1, vec1 ) ; 
        vec1[0] = vec[0] / radb ;
        vec1[1] = vec[1] / radb ;
        q2 = 1.0 / C2V_DOT ( vec1, vec1 ) ; 
        u = 0.5 * ( 1 + q1 - q2 ) ;
        v = q1 - u * u ; 
        v = ( v > 0.0 ) ? sqrt ( v ) : 0.0 ;

        n = 0 ; 
        C2V_ADDT ( ctr1, vec, u, vec1 ) ; 

        a[0] = vec1[0] - v * vec[1] ;
        a[1] = vec1[1] + v * vec[0] ;
        if ( c2g_project ( arc1, 0.0, 2.0, a, t1+n, p[0] ) &&
             c2g_project ( arc2, 0.0, 2.0, a, t2+n, p[1] ) &&
            C2V_IDENT_PTS ( p[0], p[1] ) ) {
            C2V_MID_PT ( p[0], p[1], pt[n] ) ;
            type[n] = 1 ;
            n++ ;
        }

        v = -v ;
        a[0] = vec1[0] - v * vec[1] ;
        a[1] = vec1[1] + v * vec[0] ;
        if ( c2g_project ( arc1, 0.0, 2.0, a, t1+n, p[0] ) &&
             c2g_project ( arc2, 0.0, 2.0, a, t2+n, p[1] ) &&
            C2V_IDENT_PTS ( p[0], p[1] ) ) {
            C2V_MID_PT ( p[0], p[1], pt[n] ) ;
            type[n] = 1 ;
            n++ ;
        }
        if ( n == 2 && C2V_IDENT_PTS ( pt[0], pt[1] ) ) {
            n = 1 ;
            type[0] = 2 ;
            if ( near_tan != NULL ) 
                near_tan[0] = FALSE ;
            C2V_MID_PT ( pt[0], pt[1], pt[0] ) ;
            if ( t1[1] < 0.5 && t1[0] > 1.5 ) 
                t1[0] = t1[1] ;
            else if ( t1[0] >= 0.5 || t1[1] <= 1.5 ) 
                t1[0] = 0.5 * ( t1[0] + t1[1] ) ;
            if ( t2[1] < 0.5 && t2[0] > 1.5 ) 
                t2[0] = t2[1] ;
            else if ( t2[0] >= 0.5 || t2[1] <= 1.5 ) 
                t2[0] = 0.5 * ( t2[0] + t2[1] ) ;
        }
        if ( near_tan != NULL && n == 2 ) {
            u = C2V_NORM ( vec ) ; 
            if ( IS_SMALL ( u - rada - radb ) || 
                 IS_SMALL ( u - rada + radb ) || 
                 IS_SMALL ( u + rada - radb ) ) {
                near_tan[0] = TRUE ;
                near_tan[1] = TRUE ;
            }
            else {
                near_tan[0] = FALSE ;
                near_tan[1] = FALSE ;
            }
        }
        RETURN ( n ) ;
    }

    s1 = ( c2r_complement ( C2_ARC_SEG(arc1), &carc1 ) != NULL ) ;
    s2 = ( c2r_complement ( C2_ARC_SEG(arc2), &carc2 ) != NULL ) ;

    n = c2r_intersect ( C2_ARC_SEG(arc1), C2_ARC_SEG(arc2), 
        t1, t2, pt, type ) ;

    if ( n < 0 ) 
        RETURN ( n ) ;

    m = s1 ? c2r_intersect ( &carc1, C2_ARC_SEG(arc2), u1, u2, p, x ) : 0 ;
    for ( i=0 ; i<m ; i++ ) {
        if ( u1[i] > BBS_ZERO && u1[i] < 1.0 - BBS_ZERO ) {
            t1[n] = u1[i] + 1.0 ;
            t2[n] = u2[i] ;
            C2V_COPY ( p[i], pt[n] ) ;
            type[n] = x[i] ;
            n++ ;
        }
    }

    m = s1 ? c2r_intersect ( C2_ARC_SEG(arc1), &carc2, u1, u2, p, x ) : 0 ;
    for ( i=0 ; i<m ; i++ ) {
        if ( u2[i] > BBS_ZERO && u2[i] < 1.0 - BBS_ZERO ) {
            t1[n] = u1[i] ;
            t2[n] = u2[i] + 1.0 ;
            C2V_COPY ( p[i], pt[n] ) ;
            type[n] = x[i] ;
            n++ ;
        }
    }

    m = s1 && s2 ? c2r_intersect ( &carc1, &carc2, u1, u2, p, x ) : 0 ;
    for ( i=0 ; i<m ; i++ ) {
        if ( u1[i] > BBS_ZERO && u1[i] < 1.0 - BBS_ZERO && 
             u2[i] > BBS_ZERO && u2[i] < 1.0 - BBS_ZERO ) {
            t1[n] = u1[i] + 1.0 ;
            t2[n] = u2[i] + 1.0 ;
            C2V_COPY ( p[i], pt[n] ) ;
            type[n] = x[i] ;
            n++ ;
        }
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2g_int_arc_arc ( arc1, t10, t11, arc2, t20, t21, t1, t2, 
            pt, type ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc1, arc2 ;
REAL t10, t11, t20, t21 ;
REAL *t1, *t2 ;
PT2 *pt ;
INT *type ;
{
    INT i, m, n ;

    if ( t20 >= -BBS_ZERO && t21 <= 1.0 + BBS_ZERO ) 
        m = c2r_intersect ( C2_ARC_SEG(arc1), C2_ARC_SEG(arc2), 
            t1, t2, pt, type ) ;
    else
        m = c2g_int_arc_arc_ext ( arc1, arc2, t1, t2, pt, type, NULL ) ;

    n = 0 ;
    for ( i = 0 ; i < m ; i++ ) {
        if ( t1[i] >= t10 - BBS_ZERO && t1[i] <= t11 + BBS_ZERO &&
             t2[i] >= t20 - BBS_ZERO && t2[i] <= t21 + BBS_ZERO ) {
            if ( i != n ) {
                t1[n] = t1[i] ;
                t2[n] = t2[i] ;
                C2V_COPY ( pt[i], pt[n] ) ;
                type[n] = type[i] ;
            }
            n++ ;
        }
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2g_intersect_coinc ( arc1, t10, t11, arc2, t20, t21, 
    t1, t2, int_pt, type ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc1, arc2 ;
REAL t10, t11, t20, t21 ;
REAL *t1, *t2 ;
PT2 *int_pt ;
INT *type ;
{
    REAL t ;
    INT n = 0 ;
    PT2 proj_pt ;
    BOOLEAN /* status, */ ext, ext1, ext2 ;

    ext1 = IS_ZERO(t10) && IS_ZERO(t11-2.0) ;
    ext2 = IS_ZERO(t20) && IS_ZERO(t21-2.0) ;
    ext = ext1 || ext2 ;

/***************
    n = 2 ;
    t1[0] = t2[0] = 0.0 ;
    t1[1] = t2[1] = 2.0 ;
    type[0] = type[1] = -2 ;
    RETURN ( n ) ;
***************/

    c2g_pt_tan ( arc1, t10, int_pt[n], NULL ) ;
/*
    status = c2g_project ( arc2, t20, t21, int_pt[n], &t, proj_pt ) ;
    if ( status && t >= t20 && t <= t21 ) {
*/
    if ( c2g_project ( arc2, t20, t21, int_pt[n], &t, proj_pt ) ) {
        t1[n] = t10 ;
        t2[n] = t ;
        type[n] = ( ext || ( !IS_ZERO(t-t20) && !IS_ZERO(t-t21) ) ) ? -2 : -1 ;
        if ( c2g_new_inters ( t1, t2, type[n], n ) )
            n++ ;
    }

    c2g_pt_tan ( arc1, t11, int_pt[n], NULL ) ;
/*
    status = c2g_project ( arc2, t20, t21, int_pt[n], &t, proj_pt ) ;
    if ( status && t >= t20 && t <= t21 ) {
*/
    if ( c2g_project ( arc2, t20, t21, int_pt[n], &t, proj_pt ) ) {
        t1[n] = t11 ;
        t2[n] = t ;
        type[n] = ( ext || ( !IS_ZERO(t-t20) && !IS_ZERO(t-t21) )) ? -2 : -1 ;
        if ( c2g_new_inters ( t1, t2, type[n], n ) )
            n++ ;
    }

    if ( ext1 && ext2 && n == 2 ) {
        if ( type[0] == -1 && type[1] == -1 && IS_ZERO(t1[0]-t10) && 
            IS_ZERO(t2[0]-t20) && IS_ZERO(t1[1]-t11) && IS_ZERO(t2[1]-t20) ) {
            t2[1] = t21 ;
            type[0] = type[1] = -2 ;
        }
        RETURN ( c2l_coinc_order ( t1, t2, int_pt, type, n ) ) ;
    }

    c2g_pt_tan ( arc2, t20, int_pt[n], NULL ) ;
/*
    status = c2g_project ( arc1, t10, t11, int_pt[n], &t, proj_pt ) ;
    if ( status && t >= t10 && t <= t11 ) {
*/
    if ( c2g_project ( arc1, t10, t11, int_pt[n], &t, proj_pt ) ) {
        t1[n] = t ;
        t2[n] = t20 ;
        type[n] = ( ext || ( !IS_ZERO(t-t10) && !IS_ZERO(t-t11) )) ? -2 : -1 ;
        if ( c2g_new_inters ( t1, t2, type[n], n ) )
            n++ ;
    }

    c2g_pt_tan ( arc2, t21, int_pt[n], NULL ) ;
/*
    status = c2g_project ( arc1, t10, t11, int_pt[n], &t, proj_pt ) ;
    if ( status && t >= t10 && t <= t11 ) {
*/
    if ( c2g_project ( arc1, t10, t11, int_pt[n], &t, proj_pt ) ) {
        t1[n] = t ;
        t2[n] = t21 ;
        type[n] = ( ext || ( !IS_ZERO(t-t10) && !IS_ZERO(t-t11) )) ? -2 : -1 ;
        if ( c2g_new_inters ( t1, t2, type[n], n ) )
            n++ ;
    }
    if ( n == 2 && type[0] == -1 && type[1] == -1 && IS_ZERO(t1[0]-t10) 
        && IS_ZERO(t2[0]-t20) && IS_ZERO(t1[1]-t11) && IS_ZERO(t2[1]-t21) ) 
        type[0] = type[1] = -2 ;
    RETURN ( c2l_coinc_order ( t1, t2, int_pt, type, n ) ) ;
}

/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c2g_new_inters ( t1, t2, type, n ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *t1, *t2 ;
INT type, n ;
{
    INT i ;
    if ( type != -1 ) 
        RETURN ( TRUE ) ;
    for ( i=0 ; i<n ; i++ ) {
        if ( ( IS_ZERO(t1[i]-t1[n]) || IS_ZERO(fabs(t1[i]-t1[n])-2.0) ) && 
             ( IS_ZERO(t2[i]-t2[n]) || IS_ZERO(fabs(t2[i]-t2[n])-2.0) ) ) 
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}

