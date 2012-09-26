/* -2 -3 */
/********************************** C2LI.C *********************************/
/******************** Routines for processing lines ************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <aldefs.h>
#include <c2ldefs.h>
#include <c2lmcrs.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2l_inters_ext ( line1, line2, t1, t2, 
            int_pt, type, near_tan ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line1, line2 ;
REAL *t1, *t2 ;
PT2 *int_pt ;
INT *type ;
BOOLEAN *near_tan ;
{
    PT2 vec, vec1, vec2 ;
    REAL d ;

    C2_LINE_DIR_VEC ( line1, vec1 ) ;
    C2_LINE_DIR_VEC ( line2, vec2 ) ;
    C2V_SUB ( C2_LINE_PT0(line2), C2_LINE_PT0(line1), vec ) ;
    d = C2V_CROSS ( vec1, vec2 ) ;
    if ( near_tan != NULL )
        near_tan[0] = 0 ;
    if ( fabs ( d ) <= BBS_ZERO * C2V_NORML1 ( vec1 ) * C2V_NORML1 ( vec2 ) ) {
        /* The lines are almost paralel */
        RETURN ( fabs ( C2V_CROSS(vec, vec1) ) > BBS_TOL * C2V_NORML1(vec1) ?
            0 /* parallel but not coincident */: -1 ) ;
    }
    
    t1[0] = C2V_CROSS ( vec, vec2 ) / d ;
    t2[0] = C2V_CROSS ( vec, vec1 ) / d ;
    if ( int_pt != NULL ) 
        C2V_ADDT ( C2_LINE_PT0(line1), vec1, t1[0], int_pt[0] ) ;
    type[0] = 1 ;
    RETURN ( 1 ) ; 
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2l_intersect ( line1, t10, t11, line2, t20, t21, t1, t2, 
            int_pt, type ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line1, line2 ;
REAL t10, t11, t20, t21 ;
REAL *t1, *t2 ;
PT2 *int_pt ;
INT *type ;
{
    INT n ;

    n = c2l_inters_ext ( line1, line2, t1, t2, int_pt, type, NULL ) ;
    if ( n == 1 && ( t1[0] < t10 - BBS_ZERO || t1[0] > t11 + BBS_ZERO || 
        t2[0] < t20 - BBS_ZERO || t2[0] > t21 + BBS_ZERO ) )
        n = 0 ;
    RETURN ( n ) ; 
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2l_intersect_coinc ( line1, t10, t11, line2, t20, t21, 
    t1, t2, int_pt, type ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line1, line2 ;
REAL t10, t11, t20, t21 ;
REAL *t1, *t2 ;
PT2 *int_pt ;
INT *type ;
{
    REAL t ;
    INT n = 0 ;
    PT2 p10, p11, p20, p21, proj_pt ;   /* Only for debugging purposes */
    BOOLEAN same_dir ;

    C2V_ADDU ( C2_LINE_PT0(line1), C2_LINE_PT1(line1), t10, p10 ) ;
    C2V_ADDU ( C2_LINE_PT0(line1), C2_LINE_PT1(line1), t11, p11 ) ;
    C2V_ADDU ( C2_LINE_PT0(line2), C2_LINE_PT1(line2), t20, p20 ) ;
    C2V_ADDU ( C2_LINE_PT0(line2), C2_LINE_PT1(line2), t21, p21 ) ;
    same_dir = ( fabs(p11[0]-p10[0]) > fabs(p11[1]-p10[1]) ) ?
        ( p11[0] > p10[0] ) == ( p21[0] > p20[0] ) :
        ( p11[1] > p10[1] ) == ( p21[1] > p20[1] ) ;

    if ( C2V_IDENT_PTS ( p10, p11 ) ) {
        if ( C2V_IDENT_PTS ( p20, p21 ) ) {

            if ( C2V_IDENT_PTS ( p10, p20 ) ) {
                t1[n] = t10 ;
                t2[n] = t20 ;
                type[n] = -2 ;
                RETURN ( 1 ) ;
            }
            else 
                RETURN ( 0 ) ;
        }

        else {
            C2V_COPY ( p10, int_pt[n] ) ;
            t = c2l_project ( line2, int_pt[n], proj_pt ) ;
            if ( t >= t20 && t <= t21 ) {
                t1[n] = t10 ;
                t2[n] = t ;
                type[n] = -2 ;
                RETURN ( 1 ) ;
            }
            else 
                RETURN ( 0 ) ;
        }
    }

    else {
        if ( C2V_IDENT_PTS ( p20, p21 ) ) {
            C2V_COPY ( p20, int_pt[n] ) ;
            t = c2l_project ( line1, int_pt[n], proj_pt ) ;
            if ( t >= t10 && t <= t11 ) {
                t1[n] = t ;
                t2[n] = t20 ;
                type[n] = -2 ;
                RETURN ( 1 ) ;
            }
            else 
                RETURN ( 0 ) ;
        }
    }

    t = c2l_project ( line2, p10, proj_pt ) ;
    if ( t >= t20 && t <= t21 ) {
        C2V_COPY ( p10, int_pt[n] ) ;
        t1[n] = t10 ;
        t2[n] = t ;
        type[n] = -2 ;
        n++ ;
    }

    t = c2l_project ( line2, p11, proj_pt ) ;
    if ( t >= t20 && t <= t21 ) {
        C2V_COPY ( p11, int_pt[n] ) ;
        t1[n] = t11 ;
        t2[n] = t ;
        type[n] = -2 ;
        n++ ;
    }

    if ( n == 2 ) 
        RETURN ( c2l_coinc_order ( t1, t2, int_pt, type, n ) ) ;
    t = c2l_project ( line1, p20, proj_pt ) ;
    if ( t >= t10 && t <= t11 ) {
        C2V_COPY ( p20, int_pt[n] ) ;
        t1[n] = t ;
        t2[n] = t20 ;
        type[n] = -2 ;
        n++ ;
        if ( n == 2 ) {
            if ( IS_ZERO(t1[0]-t1[1]) ) {
                if ( same_dir ? IS_ZERO(t1[0]-t11) : IS_ZERO(t1[0]-t10) ) {
                    type[0] = -1 ;
                    RETURN ( 1 ) ;
                }
                else 
                    n-- ;
            }
        }
    }

    if ( n == 2 ) 
        RETURN ( c2l_coinc_order ( t1, t2, int_pt, type, n ) ) ;
    t = c2l_project ( line1, p21, proj_pt ) ;
    if ( t >= t10 && t <= t11 ) {
        C2V_COPY ( p21, int_pt[n] ) ;
        t1[n] = t ;
        t2[n] = t21 ;
        type[n] = -2 ;
        n++ ;
        if ( n == 2 ) {
            if ( IS_ZERO(t1[0]-t1[1]) ) {
                if ( same_dir ? IS_ZERO(t1[0]-t10) : IS_ZERO(t1[0]-t11) ) {
                    type[0] = -1 ;
                    RETURN ( 1 ) ;
                }
                else 
                    n-- ;
            }
        }
    }
    if ( n == 1 && same_dir ) 
        type[0] = -1 ;
    RETURN ( c2l_coinc_order ( t1, t2, int_pt, type, n ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2l_coinc_order ( t1, t2, int_pt, type, n ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *t1, *t2 ;
PT2 *int_pt ;
INT *type ;
INT n ;
{
    REAL t ;
    PT2 p ;
    INT i, m ;
    BOOLEAN status ;

    status = FALSE ;

    while ( !status ) {
        status = TRUE ;
        for ( i = 1 ; i < n ; i++ ) {
            if ( t1[i-1] > t1[i] ) {
                t = t1[i-1] ;
                t1[i-1] = t1[i] ;
                t1[i] = t ;
                t = t2[i-1] ;
                t2[i-1] = t2[i] ;
                t2[i] = t ;
                C2V_COPY ( int_pt[i-1], p ) ;
                C2V_COPY ( int_pt[i], int_pt[i-1] ) ;
                C2V_COPY ( p, int_pt[i] ) ;
                m = type[i-1] ;
                type[i-1] = type[i] ;
                type[i] = m ;
                status = FALSE ;
            }
        }
    }
    RETURN ( n ) ;
}
