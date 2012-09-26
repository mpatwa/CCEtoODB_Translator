/* -2 -3 */
/********************************** C2G.C **********************************/
/****************** Routines for processing arc geometry *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2gdefs.h>
#include <c2rdefs.h>
#include <dmldefs.h>
#include <c2gmcrs.h>
#include <c2vmcrs.h>

STATIC INT coord_extr __(( C2_ARC, PT2, REAL, REAL, INT, INT, DML_LIST )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2g_eval ( arc, t, p, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL t ;
INT p ;
PT2 *x ;
{
    if ( C2_ARC_ZERO_RAD(arc) ) {
        INT i ;
        c2r_ctr ( C2_ARC_SEG(arc), x[0] ) ;
        for ( i=1 ; i<=p ; i++ ) 
            C2V_SET_ZERO ( x[i] ) ;
    }

    else {
        if ( t <= 0.0 ) 
            t = 0.0 ;
        if ( t <= 1.0 ) 
            c2r_eval ( C2_ARC_SEG(arc), t, p, x ) ;
        else {
            C2_ASEG_S arc1 ;
            if ( t >= 2.0 ) 
                    t = 2.0 ;
            c2r_eval ( c2r_complement ( C2_ARC_SEG(arc), &arc1 ), 
                t - 1.0, p, x ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2g_pt_tan ( arc, t, pt, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* Evaluate non-normalized tangent vector */
C2_ARC arc ;
REAL t ;
PT2 pt, tan_vec ;
{
    if ( t <= 0.0 ) 
        t = 0.0 ;
    if ( t <= 1.0 ) 
        c2r_pt_tan ( C2_ARC_SEG(arc), t, pt, tan_vec ) ;
    else {
        C2_ASEG_S arc1 ;

        if ( t >= 2.0 ) 
            t = 2.0 ;
        c2r_pt_tan ( c2r_complement ( C2_ARC_SEG(arc), &arc1 ), 
            t - 1.0, pt, tan_vec ) ;
    }

    if ( C2_ARC_ZERO_RAD(arc) && pt != NULL ) 
        c2r_ctr ( C2_ARC_SEG(arc), pt ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2g_coord_extrs ( arc, t0, t1, coord, extr_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC  arc ;
REAL    t0, t1 ;
INT     coord ;
DML_LIST extr_list ;
{
    PT2 ctr, pt ;
    REAL rad ;
    INT extr_no ;

    rad = c2r_rad ( C2_ARC_SEG(arc) ) ;
    if ( rad < 0.0 ) 
        RETURN ( 0 ) ;
    c2r_ctr ( C2_ARC_SEG(arc), ctr ) ;
    C2V_COPY ( ctr, pt ) ;
    pt[coord] += rad ;
    extr_no = coord_extr ( arc, pt, t0, t1, coord, 1, extr_list ) ;

    C2V_COPY ( ctr, pt ) ;
    pt[coord] -= rad ;
    extr_no += coord_extr ( arc, pt, t0, t1, coord, -1, extr_list ) ;
    RETURN ( extr_no ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT coord_extr ( arc, pt, t0, t1, coord, type, extr_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC  arc ;
PT2     pt ;
REAL    t0, t1 ;
INT     coord, type ;
DML_LIST extr_list ;
{
    REAL t ;
    C2_EXTR_REC extr_rec ;

    if ( c2g_project ( arc, t0, t1, pt, &t, NULL ) ) {
        extr_rec = MALLOC ( 1, C2_EXTR_REC_S ) ;
        if ( extr_rec == NULL )
            RETURN (-1) ;
        C2_EXTR_REC_T(extr_rec) = t ; 
        C2_EXTR_REC_J(extr_rec) = ( t <= 1.0 ? 0 : 1 ) ; 
        C2_EXTR_REC_F(extr_rec) = pt[coord] ;
        C2_EXTR_REC_TYPE(extr_rec) = type ;
        dml_append_data ( extr_list, extr_rec ) ; 
        RETURN ( 1 ) ;
    }
    else
        RETURN ( 0 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2g_length ( arc, t0, t1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL t0, t1 ;
{
    C2_ASEG_S arc1 ;

    if ( C2_ARC_ZERO_RAD(arc) )
        RETURN ( 0.0 ) ;
    if ( IS_SMALL(t0) && IS_SMALL(t1-1.0) ) 
        RETURN ( c2r_length ( C2_ARC_SEG(arc) ) ) ;
    else if ( IS_SMALL(t0) && IS_SMALL(t1-2.0) ) {
        RETURN ( c2r_length ( C2_ARC_SEG(arc) ) + 
            c2r_length ( c2r_complement ( C2_ARC_SEG(arc), &arc1 ) ) ) ;
    }
    else if ( t1 < 1.0 ) 
        RETURN ( c2r_length ( c2r_trim ( C2_ARC_SEG(arc), 
            t0, t1, &arc1 ) ) ) ;
    else if ( t0 > 1.0 ) 
        RETURN ( c2r_length ( c2r_trim ( 
            c2r_complement ( C2_ARC_SEG(arc), &arc1 ), 
            t0-1.0, t1-1.0, &arc1 ) ) ) ;
    else 
        RETURN ( 
            c2r_length ( c2r_trim ( C2_ARC_SEG(arc), t0, 1.0, &arc1 ) ) +
            c2r_length ( c2r_trim ( c2r_complement ( C2_ARC_SEG(arc), &arc1 ), 
                0.0, t1-1.0, &arc1 ) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2g_select ( arc, t0, t1, pt, tol, t_ptr, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL t0, t1 ;
PT2 pt ;
REAL tol ;
REAL *t_ptr ;
REAL *dist_ptr ;
{
    C2_ASEG_S arc1 ;
    REAL t ;

    if ( t_ptr == NULL ) 
        t_ptr = &t ;
    if ( C2_ARC_ZERO_RAD(arc) ) {
        PT2 ctr ;
        *t_ptr = 0.5 ;
        if ( C2V_DISTL1 ( c2r_ctr ( C2_ARC_SEG(arc), ctr ), pt ) > tol ) 
            RETURN ( FALSE ) ;
        if ( dist_ptr != NULL ) 
            *dist_ptr = C2V_DIST ( c2r_ctr ( C2_ARC_SEG(arc), ctr ), pt ) ;
        RETURN ( TRUE ) ;
    }
    if ( c2r_select ( C2_ARC_SEG(arc), pt, tol, t_ptr, dist_ptr ) && 
        t0 - BBS_ZERO <= *t_ptr && *t_ptr <= t1 + BBS_ZERO ) 
        RETURN ( TRUE ) ;
    if ( t1 <= 1.0 + BBS_ZERO )
        RETURN ( FALSE ) ;
    if ( !c2r_select ( c2r_complement ( C2_ARC_SEG(arc), &arc1 ), 
        pt, tol, t_ptr, dist_ptr ) )
        RETURN ( FALSE ) ;
    *t_ptr += 1.0 ;
    RETURN ( t0 - BBS_ZERO <= *t_ptr && *t_ptr <= t1 + BBS_ZERO ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2g_project ( arc, t0, t1, pt, t_ptr, on_pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
PT2 pt ;
REAL t0, t1 ;
REAL *t_ptr ;
PT2 on_pt ;
{
    C2_ASEG_S arc1 ;
    REAL dist0, dist1, u0, u1 ;
    PT2 p0, p1, c ; 
    BOOLEAN status0, status1 ;

    if ( t1 <= 1.0 ) {
        if ( c2r_project ( C2_ARC_SEG(arc), pt, t_ptr, on_pt ) )
            RETURN ( ( t0-BBS_ZERO <= *t_ptr ) && ( *t_ptr <= t1+BBS_ZERO ) ) ;
        else {
            c2r_ctr ( C2_ARC_SEG(arc), c ) ;
            if ( C2V_IDENT_PTS(c,pt) ) {
                *t_ptr = 0.5 * ( t0 + t1 ) ;
                if ( on_pt != NULL ) 
                    c2r_pt_tan ( C2_ARC_SEG(arc), *t_ptr, on_pt, NULL ) ;
                RETURN ( TRUE ) ;
            }
            else
                RETURN ( FALSE ) ;
        }
    }
    status0 = t0 < 1.0 && c2r_project ( C2_ARC_SEG(arc), pt, &u0, p0 ) 
        && t0-BBS_ZERO <= u0 && u0 <= t1+BBS_ZERO ;
    status1 = c2r_project ( c2r_complement ( C2_ARC_SEG(arc), &arc1 ), 
        pt, &u1, p1 ) && t0-BBS_ZERO <= u1+1.0 && u1+1.0 <= t1+BBS_ZERO && 
        u1 <= 1.0 - BBS_ZERO ;
    if ( status0 ) {
        if ( status1 ) {
            dist0 = C2V_DIST ( p0, pt ) ; 
            dist1 = C2V_DIST ( p1, pt ) ; 
            if ( dist0 <= dist1 ) {
                *t_ptr = u0 ;
                if ( on_pt != NULL ) 
                    C2V_COPY ( p0, on_pt ) ;
            }
            else {
                *t_ptr = u1+1.0 ;
                if ( on_pt != NULL ) 
                    C2V_COPY ( p1, on_pt ) ;
            }
            RETURN ( TRUE ) ;
        }
        else {
            *t_ptr = u0 ;
            if ( on_pt != NULL ) 
                C2V_COPY ( p0, on_pt ) ;
            RETURN ( TRUE ) ;
        }
    }
    else {
        if ( status1 ) {
            *t_ptr = u1+1.0 ;
            if ( on_pt != NULL ) 
                C2V_COPY ( p1, on_pt ) ;
            RETURN ( TRUE ) ;
        }
        else {
            c2r_ctr ( C2_ARC_SEG(arc), c ) ;
            if ( C2V_IDENT_PTS(c,pt) ) {
                *t_ptr = 0.5 * ( t0 + t1 ) ;
                if ( on_pt != NULL ) 
                    c2r_pt_tan ( C2_ARC_SEG(arc), *t_ptr, on_pt, NULL ) ;
                RETURN ( TRUE ) ;
            }
            else
                RETURN ( FALSE ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ARC c2g_complement ( arc0, arc1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc0, arc1 ;
{
    if ( c2r_complement ( C2_ARC_SEG(arc0), C2_ARC_SEG(arc1) ) == NULL ) 
        RETURN ( NULL ) ;
    C2_ARC_ZERO_RAD(arc1) = C2_ARC_ZERO_RAD(arc0) ;
    RETURN ( arc1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ARC c2g_reverse ( arc0, arc1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc0, arc1 ;
{
    if ( c2r_reverse ( C2_ARC_SEG(arc0), C2_ARC_SEG(arc1) ) == NULL ) 
        RETURN ( NULL ) ;
    C2_ARC_ZERO_RAD(arc1) = C2_ARC_ZERO_RAD(arc0) ;
    RETURN ( arc1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2g_negate_d ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
{
    c2r_negate_d ( C2_ARC_SEG(arc) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2g_rad ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
{
    RETURN ( C2_ARC_ZERO_RAD(arc) ? 0.0 : c2r_rad ( C2_ARC_SEG(arc) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2g_start_angle ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
{
    REAL sa;

    c2r_get_data ( C2_ARC_SEG(arc), NULL, NULL, NULL, &sa, NULL, NULL );

    RETURN ( sa );
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2g_sweep ( arc, t0, t1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL t0, t1 ;
{
    RETURN ( C2_ARC_ZERO_RAD(arc) ? 0.0 : 
        c2r_sweep ( C2_ARC_SEG(arc), t0, t1 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2g_t_to_angle ( arc, t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL t ;
{
    RETURN ( C2_ARC_ZERO_RAD(arc) ? 0.0 : 
        c2r_t_to_angle ( C2_ARC_SEG(arc), t ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2g_angle_to_t ( arc, angle ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL angle ;
{
    RETURN ( C2_ARC_ZERO_RAD(arc) ? 0.0 : 
        c2r_angle_to_t ( C2_ARC_SEG(arc), angle ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2g_dir ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
{
    RETURN ( C2_ARC_ZERO_RAD(arc) ? 0 : c2r_dir ( C2_ARC_SEG(arc) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL* c2g_ctr ( arc, ctr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
PT2 ctr ;
{
    RETURN ( c2r_ctr ( C2_ARC_SEG(arc), ctr ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2g_2pts_tan_ctr ( pt0, tan0, pt1, ctr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1, tan0, ctr ;
{
    RETURN ( c2r_2pts_tan_ctr ( pt0, tan0, pt1, ctr ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE REAL c2g_get_d ( arc )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
{
    RETURN ( C2_ARC_ZERO_RAD(arc) ? 0.0 : c2r_get_d ( C2_ARC_SEG(arc) ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2g_get_pts_d ( arc, t0, t1, pt0, pt1, d_ptr )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL t0, t1 ;
PT2 pt0, pt1 ;
REAL* d_ptr ;
{
    RETURN ( !C2_ARC_ZERO_RAD(arc) && 
        c2r_get_pts_d ( C2_ARC_SEG(arc), t0, t1, pt0, pt1, d_ptr ) ) ;
}

#ifdef SPLINE
/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2g_bezs ( arc, ext_arc, bez ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
BOOLEAN ext_arc ;
HPT2 *bez ;
{
    RETURN ( C2_ARC_ZERO_RAD(arc) ? 0 : 
        c2r_bezs ( C2_ARC_SEG(arc), ext_arc, bez ) ) ;
}
#endif /*SPLINE*/

