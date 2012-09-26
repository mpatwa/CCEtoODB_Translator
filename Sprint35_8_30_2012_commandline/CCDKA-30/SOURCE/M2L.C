/* -R __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************** M2L.C **********************************/
/******************** Routines for processing lines ************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <aladefs.h>
#include <c2ldefs.h>
#include <m2ldefs.h>
#include <c2vdefs.h>
#include <c2lmcrs.h>
#include <c2vmcrs.h>
STATIC INT m2l_intersect_coinc __(( C2_LINE, REAL, REAL, C2_LINE, REAL, REAL, 
    REAL*, REAL*, PT2*, INT* )) ;
STATIC BOOLEAN m2l_ident_lines __(( C2_LINE, C2_LINE )) ;
STATIC INT m2l_intersect_ident __(( C2_LINE, REAL, REAL, REAL, REAL, 
    REAL*, REAL*, PT2*, INT* )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2l_hor_ray_int_no ( line, t0, t1, pt, pt_on_line ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
REAL t0, t1 ;
PT2 pt ;
BOOLEAN pt_on_line ;
{
    PT2 ept0, ept1, *ept0_ptr, *ept1_ptr ;
    REAL t, x, x_minus=pt[0]-BBS_TOL, 
        y_minus=pt[1]-BBS_TOL, y_plus=pt[1]+BBS_TOL ;

    if ( IS_ZERO(t0) ) 
        ept0_ptr = (PT2*)C2_LINE_PT0(line) ;
    else {
        C2V_ADDU ( C2_LINE_PT0(line), C2_LINE_PT1(line), t0, ept0 ) ;
        ept0_ptr = (PT2*)ept0 ;
    }
    if ( IS_ZERO(t1-1.0) ) 
        ept1_ptr = (PT2*)C2_LINE_PT1(line) ;
    else {
        C2V_ADDU ( C2_LINE_PT0(line), C2_LINE_PT1(line), t1, ept1 ) ;
        ept1_ptr = (PT2*)ept1 ;
    }

    if ( C2V_IDENT_PTS ( pt, *ept0_ptr ) || C2V_IDENT_PTS ( pt, *ept1_ptr ) )
        RETURN ( pt_on_line ? M2_PT_ON_CURVE : 0 ) ;
    if ( (*ept0_ptr)[1] > y_plus && (*ept1_ptr)[1] > y_plus ) 
        RETURN ( 0 ) ;
    if ( (*ept0_ptr)[1] < y_minus && (*ept1_ptr)[1] < y_minus ) 
        RETURN ( 0 ) ;
    if ( (*ept0_ptr)[0] < x_minus && (*ept1_ptr)[0] < x_minus ) 
        RETURN ( 0 ) ;

    if ( IS_SMALL ( (*ept1_ptr)[1] - (*ept0_ptr)[1] ) ) {
        if ( (*ept0_ptr)[0] <= x_minus && (*ept1_ptr)[0] <= x_minus )
            RETURN ( 0 ) ;
        else if ( (*ept0_ptr)[0] <= x_minus || (*ept1_ptr)[0] <= x_minus )
            RETURN ( pt_on_line ? M2_PT_ON_CURVE : 0 ) ;
        else 
            RETURN ( M2_END_PT_ON_RAY ) ; 
    }

    t = ( pt[1] - C2_LINE_PT0(line)[1] ) / 
        ( C2_LINE_PT1(line)[1] - C2_LINE_PT0(line)[1] ) ;
    if ( IS_ZERO(t-t0) || IS_ZERO(t-t1) )
        RETURN ( M2_END_PT_ON_RAY ) ;
    else if ( t < t0 || t > t1 )
        RETURN ( 0 ) ;
    x = t * ( C2_LINE_PT1(line)[0] - C2_LINE_PT0(line)[0] ) +
        C2_LINE_PT0(line)[0] ;
    if ( x <= x_minus ) 
        RETURN ( 0 ) ;
    else if ( x >= pt[0] + BBS_TOL ) 
        RETURN ( 1 ) ;
    else 
        RETURN ( pt_on_line ? M2_PT_ON_CURVE : 0 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2l_ray_int_no ( line, t0, t1, pt, c, s, pt_on_line ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
REAL t0, t1 ;
PT2 pt ;
REAL c, s ;
BOOLEAN pt_on_line ;
{
    C2_LINE_S rot_line ;

    c2l_rotate_cs ( line, pt, c, - s, &rot_line ) ;
    RETURN ( m2l_hor_ray_int_no ( &rot_line, t0, t1, pt, pt_on_line ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2l_x_max ( line, t0, t1, x_max_ptr, t_max_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
REAL t0, t1 ;
REAL *x_max_ptr ;
REAL *t_max_ptr ;
{
    REAL x0, x1 ;

    x0 = IS_ZERO(t0) ? C2_LINE_PT0(line)[0] : 
        C2_LINE_PT0(line)[0] * ( 1.0-t0 ) + C2_LINE_PT1(line)[0] * t0 ;
    x1 = IS_ZERO(t1-1.0) ? C2_LINE_PT1(line)[0] : 
        C2_LINE_PT0(line)[0] * ( 1.0-t1 ) + C2_LINE_PT1(line)[0] * t1 ;
    if ( x0 < x1 ) {
        if ( x1 > *x_max_ptr ) {
            *x_max_ptr = x1 ;
            *t_max_ptr = t1 ;
            RETURN ( TRUE ) ;
        }
        else
            RETURN ( FALSE ) ;
    }
    else {
        if ( x0 > *x_max_ptr ) {
            *x_max_ptr = x0 ;
            *t_max_ptr = t0 ;
            RETURN ( TRUE ) ;
        }
        else
            RETURN ( FALSE ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL m2l_area ( line, t0, t1, origin )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
REAL t0, t1 ;
PT2 origin ;
{
    if ( origin == NULL ) 
        RETURN ( 0.5 * ( t1 - t0 ) * 
            C2V_CROSS ( C2_LINE_PT0(line), C2_LINE_PT1(line) ) ) ;
    else {
        PT2 a0, a1 ;
        C2V_SUB ( C2_LINE_PT0(line), origin, a0 ) ;
        C2V_SUB ( C2_LINE_PT1(line), origin, a1 ) ; 
        RETURN ( 0.5 * ( t1 - t0 ) * C2V_CROSS ( a0, a1 ) ) ;
    }
} 


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2l_tan_angle ( line, t0, t1, angle, t )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
REAL t0, t1, angle ;
REAL *t ;
{
    if ( ( C2_LINE_PT1(line)[1] - C2_LINE_PT0(line)[1] ) * cos ( angle ) -
         ( C2_LINE_PT1(line)[0] - C2_LINE_PT0(line)[0] ) * sin ( angle ) > 
         0.0 ) {
        t[0] = t0 ;
        t[1] = t1 ;
        RETURN ( 2 ) ;
    }
    else
        RETURN ( 0 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void m2l_set_ept0 ( line, pt, t0, t1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
PT2 pt ;
REAL t0, t1 ;
{
    PT2 p1 ;
    C2V_ADDU ( C2_LINE_PT0(line), C2_LINE_PT1(line), t1, p1 ) ;
    m2l_set ( line, pt, p1, t0, t1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void m2l_set_ept1 ( line, pt, t0, t1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
PT2 pt ;
REAL t0, t1 ;
{
    PT2 p0 ;
    C2V_ADDU ( C2_LINE_PT0(line), C2_LINE_PT1(line), t0, p0 ) ;
    m2l_set ( line, p0, pt, t0, t1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void m2l_set ( line, p0, p1, t0, t1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
PT2 p0, p1 ;
REAL t0, t1 ;
{
    REAL dt = t1 - t0 ;
    if ( IS_ZERO(dt) ) 
        RETURN ;
    C2_LINE_PT0(line)[0] = ( t1 * p0[0] - t0 * p1[0] ) / dt ;
    C2_LINE_PT0(line)[1] = ( t1 * p0[1] - t0 * p1[1] ) / dt ;
    C2_LINE_PT1(line)[0] = ( (1.0-t0) * p1[0] - (1.0-t1) * p0[0] ) / dt ;
    C2_LINE_PT1(line)[1] = ( (1.0-t0) * p1[1] - (1.0-t1) * p0[1] ) / dt ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL m2l_size ( line, t0, t1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
REAL t0, t1 ;
{
    RETURN ( c2l_length ( line, t0, t1 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2l_adjust_2lines ( line0, t0_0, t0_1, line1, t1_0, t1_1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line0, line1 ;
REAL t0_0, t0_1, t1_0, t1_1 ;
{
    INT type ;
    REAL t0, t1, t, cross ;
    PT2 a0, a1, b0, b1, q0, q1, c ;

    C2V_ADDU ( C2_LINE_PT0(line0), C2_LINE_PT1(line0), t0_0, a0 ) ;
    C2V_ADDU ( C2_LINE_PT0(line0), C2_LINE_PT1(line0), t0_1, a1 ) ;
    C2V_ADDU ( C2_LINE_PT0(line1), C2_LINE_PT1(line1), t1_0, b0 ) ;
    C2V_ADDU ( C2_LINE_PT0(line1), C2_LINE_PT1(line1), t1_1, b1 ) ;
    C2V_SUB ( a1, a0, q0 ) ;
    C2V_SUB ( b1, b0, q1 ) ;
    c2v_normalize ( q0, q0 ) ;
    c2v_normalize ( q1, q1 ) ;
    cross = C2V_CROSS ( q0, q1 ) ;

    if ( IS_ZERO(cross) && C2V_DOT(q0,q1) < 0.0 ) {
        C2V_MID_PT ( b0, a1, c ) ;
    }
    else if ( fabs(cross) <= 0.01 || c2l_inters_ext ( line0, line1, 
        &t0, &t1, (PT2*)c, &type, NULL ) != 1 ) {
        t0 = C2V_DIST ( a0, a1 ) ;
        t1 = C2V_DIST ( b0, b1 ) ;
        t = t0 / ( t0 + t1 ) ;
        C2V_ADDU ( a0, b1, t, c ) ;
    }
    m2l_set ( line0, a0, c, t0_0, t0_1 ) ;
    m2l_set ( line1, c, b1, t1_0, t1_1 ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2l_is_small ( line, t0, t1, tol ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
REAL t0, t1, tol ;
{
#ifdef CCDK_DEBUG
{
PT2 a0, a1 ;
C2V_ADDU ( C2_LINE_PT0(line), C2_LINE_PT1(line), t0, a0 ) ;
C2V_ADDU ( C2_LINE_PT0(line), C2_LINE_PT1(line), t1, a1 ) ;
}
#endif
    RETURN ( c2l_length ( line, t0, t1 ) <= tol ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2l_intersect ( line1, t10, t11, line2, t20, t21, 
            vtx_status, no_coinc, t1, t2, int_pt, type ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line1, line2 ;
REAL t10, t11, t20, t21 ;
BOOLEAN vtx_status, no_coinc ;
REAL *t1, *t2 ;
PT2 *int_pt ;
INT *type ;
{
    PT2 x, vec1, vec2 ;
    REAL d, d1, d2, tol ;
    REAL zero = /* vtx_status ? ( 100.0 * BBS_ZERO ) : */ BBS_ZERO ;

    if ( m2l_ident_lines ( line1, line2 ) ) 
        RETURN ( m2l_intersect_ident ( line1, t10, t11, t20, t21, 
            t1, t2, int_pt, type ) ) ;

    C2_LINE_DIR_VEC ( line1, vec1 ) ;
    C2_LINE_DIR_VEC ( line2, vec2 ) ;
    C2V_SUB ( C2_LINE_PT0(line2), C2_LINE_PT0(line1), x ) ;
    d = C2V_CROSS ( vec1, vec2 ) ;
    d1 = C2V_CROSS ( x, vec1 ) ;
    d2 = C2V_CROSS ( x, vec2 ) ;

    tol = BBS_TOL * C2V_NORM ( vec1 ) ;
    if ( !no_coinc && ( fabs(d) <= tol ) ) {
        if ( fabs(d1) <= tol ) 
            RETURN ( m2l_intersect_coinc ( line1, t10, t11, 
                line2, t20, t21, t1, t2, int_pt, type ) ) ;     
        else
            RETURN ( 0 ) ;  /* parallel but not coincident */
    }

    tol = BBS_TOL * C2V_NORM ( vec2 ) ;
    if ( !no_coinc && ( fabs(d) <= tol ) ) {
        if ( fabs(d2) <= tol ) 
            RETURN ( m2l_intersect_coinc ( line1, t10, t11, 
                line2, t20, t21, t1, t2, int_pt, type ) ) ;     
        else
            RETURN ( 0 ) ;  /* parallel but not coincident */
    }
    
    t1[0] = d2 / d ;
    t2[0] = d1 / d ;
    if ( ( t1[0] <= t10 - zero ) || ( t1[0] >= t11 + zero ) || 
         ( t2[0] <= t20 - zero ) || ( t2[0] >= t21 + zero ) )
        RETURN ( 0 ) ; 
    if ( int_pt != NULL ) 
        C2V_ADDT ( C2_LINE_PT0(line1), vec1, t1[0], int_pt[0] ) ;
    type[0] = 1 ;
    RETURN ( 1 ) ; 
}


/*-------------------------------------------------------------------------*/
STATIC INT m2l_intersect_coinc ( line1, t10, t11, line2, t20, t21, 
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
STATIC BOOLEAN m2l_ident_lines ( line1, line2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line1, line2 ;
{
    RETURN ( C2V_IDENT_PTS ( C2_LINE_PT0(line1), C2_LINE_PT0(line2) ) &&
        C2V_IDENT_PTS ( C2_LINE_PT1(line1), C2_LINE_PT1(line2) ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT m2l_intersect_ident ( line, t10, t11, t20, t21, 
    t1, t2, int_pt, type ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
REAL t10, t11, t20, t21 ;
REAL *t1, *t2 ;
PT2 *int_pt ;
INT *type ;
{
    REAL u10, u11, u20, u21, u0, u1 ;

    ala_min_max ( t10, t11, &u10, &u11 ) ;
    ala_min_max ( t20, t21, &u20, &u21 ) ;

    if ( IS_ZERO(u11-u20) ) {
        C2V_ADDU ( C2_LINE_PT0(line), C2_LINE_PT1(line), u20, int_pt[0] ) ;
        t1[0] = u11 ;
        t2[0] = u20 ;
        type[0] = -2 ;
        RETURN ( 1 ) ;
    }

    else if ( IS_ZERO(u10-u21) ) {
        C2V_ADDU ( C2_LINE_PT0(line), C2_LINE_PT1(line), u10, int_pt[0] ) ;
        t1[0] = u10 ;
        t2[0] = u21 ;
        type[0] = -2 ;
        RETURN ( 1 ) ;
    }

    else if ( u11 < u20 ) 
        RETURN ( 0 ) ;

    else if ( u21 < u10 ) 
        RETURN ( 0 ) ;

    else {
        u0 = ( u10 < u20 ) ? u20 : u10 ;
        u1 = ( u11 < u21 ) ? u11 : u21 ;
        C2V_ADDU ( C2_LINE_PT0(line), C2_LINE_PT1(line), u0, int_pt[0] ) ;
        t1[0] = u0 ;
        t2[0] = u0 ;
        type[0] = -2 ;
        C2V_ADDU ( C2_LINE_PT0(line), C2_LINE_PT1(line), u1, int_pt[1] ) ;
        t1[1] = u1 ;
        t2[1] = u1 ;
        type[1] = -2 ;
        RETURN ( 2 ) ;
    }
}


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE BOOLEAN m2l_furthest_pt ( line, pt, t0, t1, line_pt )
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C2_LINE line ;
PT2 pt ;
REAL t0, t1 ;
PT2 line_pt ;
{
    PT2 p0, p1 ;

    C2V_ADDU ( C2_LINE_PT0(line), C2_LINE_PT1(line), t0, p0 ) ;
    C2V_ADDU ( C2_LINE_PT0(line), C2_LINE_PT1(line), t1, p1 ) ;

    if ( C2V_DIST(pt,p0) < C2V_DIST(pt,p1) ) {
        C2V_COPY ( p1, line_pt ) ;
    }
    else {
        C2V_COPY ( p0, line_pt ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE BOOLEAN m2l_increment ( line, dir, t0, t1, w_ptr, pt, parm )
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C2_LINE line ;
INT dir ;
REAL t0, t1 ;
REAL *w_ptr ;
PT2 pt ;
PARM parm ;
{
    REAL dist, t, dt ;
    BOOLEAN status ;

    dist = C2V_DIST ( C2_LINE_PT0(line), C2_LINE_PT1(line) ) ;
    dt = (*w_ptr) / dist ;

    if ( dir == 1 ) {
        t = t0 + dt ;
        status = ( t <= t1 + BBS_ZERO ) ;
    }
    else {
        t = t0 - dt ;
        status = ( t >= t1 - BBS_ZERO ) ;
    }

    if ( status ) {
        *w_ptr = 0.0 ;
        if ( pt != NULL ) {
            C2V_ADDU ( C2_LINE_PT0(line), C2_LINE_PT1(line), t, pt ) ;
        }
        if ( parm != NULL ) {
            PARM_SETJ ( t, 1, parm ) ;
        }
    }
    else
        *w_ptr -= ( fabs ( t1 - t0 ) * dist ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN m2l_horline ( line ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                 !!!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
{
    RETURN ( IS_SMALL ( C2_LINE_PT0(line)[1] - C2_LINE_PT1(line)[1] ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE REAL m2l_along ( line, t0, l, dir )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
REAL t0, l ;
INT dir ;
{
    REAL u ;

    u = C2V_DIST ( C2_LINE_PT0(line), C2_LINE_PT1(line) ) ;
    if ( IS_SMALL(u) )
        u = 0.0 ;
    else 
        u = l / u ;
    RETURN ( ( dir == 1 ) ? ( t0 + u ) : ( t0 - u ) ) ;
}

