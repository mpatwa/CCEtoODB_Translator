/* -2 -3 */
/********************************** C2Q.C **********************************/
/*************** Routines for processing polycurve geometry ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2qdefs.h>
#include <c2rdefs.h>
#include <dmldefs.h>
#include <c2qmcrs.h>
#include <c2rmcrs.h>
#include <c2vmcrs.h>
STATIC BOOLEAN c2q_ti __(( C2_PC_BUFFER, REAL*, INT* )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2q_eval ( buffer, t, p, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
REAL t ;
INT p ;
PT2 *x ;
{
    INT i ;

    if ( C2_PC_BUFFER_N(buffer) == 1 ) {
        C2V_COPY ( C2_PC_BUFFER_PT0(buffer), x[0] ) ;
        for ( i = 1 ; i <= p ; i++ ) {
            C2V_SET_ZERO ( x[i] ) ;
        }
        RETURN ( TRUE ) ;
    }
    RETURN ( c2q_ti ( buffer, &t, &i ) && 
        c2r_eval ( C2_PC_BUFFER_ARC(buffer,i), t-(REAL)i, p, x ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2q_n ( buffer ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
{
    RETURN ( C2_PC_BUFFER_N(buffer) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2q_size ( buffer ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
{
    RETURN ( C2_PC_BUFFER_SIZE(buffer) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2q_pt_tan ( buffer, t, pt, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
REAL t ;
PT2 pt, tan_vec ;
{
    INT i ;
    RETURN ( c2q_ti ( buffer, &t, &i ) && 
        c2r_pt_tan ( C2_PC_BUFFER_ARC(buffer,i), t-(REAL)i, 
            pt, tan_vec ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2q_segment ( buffer, i ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
INT *i ;
{
    if ( buffer == NULL || C2_PC_BUFFER_N(buffer) <= 0 )
        RETURN ( NULL ) ;
    if ( *i < 0 )
        RETURN ( NULL ) ;
    else if ( *i < (C2_PC_BUFFER_N(buffer)-1) )
        RETURN ( C2_PC_BUFFER_ARC(buffer,*i) ) ;
    else {
        *i -= ( C2_PC_BUFFER_N(buffer) - 1 ) ;
        RETURN ( NULL ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2q_first_segm ( buffer ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
{
    if ( buffer == NULL || C2_PC_BUFFER_N(buffer) <= 0 )
        RETURN ( NULL ) ;
    else 
        RETURN ( C2_PC_BUFFER_ARC(buffer,0) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2q_last_segm ( buffer ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
{
    if ( buffer == NULL || C2_PC_BUFFER_N(buffer) <= 1 )
        RETURN ( NULL ) ;
    else 
        RETURN ( C2_PC_BUFFER_ARC(buffer,C2_PC_BUFFER_N(buffer)-2) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2q_curvature ( buffer, t, crv ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
REAL t, *crv ;
{
    INT i ;
    if ( c2q_ti ( buffer, &t, &i ) ) {
        *crv = c2r_curvature ( C2_PC_BUFFER_ARC(buffer,i) ) ;
        RETURN ( TRUE ) ;
    }
    else
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2q_coord_extrs ( buffer, t0, t1, j0, coord, extr_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
REAL t0, t1 ;
INT j0, coord ;
DML_LIST extr_list ;
{
    REAL t[2], u ;
    INT i, j, m, n ;
    C2_EXTR_REC extr_rec ;
    PT2 pt ;

    n = 0 ;
    for ( i=0 ; i<C2_PC_BUFFER_N(buffer)-1 ; i++ ) {
        m = c2r_coord_extrs ( C2_PC_BUFFER_ARC(buffer,i), coord, t ) ;
        for ( j=0 ; j<m ; j++ ) {
            u = t[j] + (REAL)j0 + (REAL)i ;
            if ( u >= t0 - BBS_ZERO && u <= t1 + BBS_ZERO && 
                ( m==0 || !IS_SMALL(u - 
                C2_EXTR_REC_T((C2_EXTR_REC)DML_LAST_RECORD(extr_list)) ) ) ) {
                extr_rec = MALLOC ( 1, C2_EXTR_REC_S ) ;
                if ( extr_rec == NULL )
                    RETURN (-1) ;
                C2_EXTR_REC_T(extr_rec) = u ; 
                C2_EXTR_REC_J(extr_rec) = i + j0 ; 
                c2r_pt_tan ( C2_PC_BUFFER_ARC(buffer,i), t[j], pt, NULL ) ;
                C2_EXTR_REC_F(extr_rec) = pt[coord] ;
                C2_EXTR_REC_TYPE(extr_rec) = 1 ;
                dml_append_data ( extr_list, extr_rec ) ; 
                n++ ;
            }
        }
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2q_length ( buffer, t0, t1, j0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
REAL t0, t1 ;
INT j0 ;
{
    INT i0, i1, i ;
    REAL length ;
    C2_ASEG_S trim_arc ;

    t0 -= (REAL)j0 ;
    t1 -= (REAL)j0 ;
    i0 = (INT)t0 ;
    i1 = (INT)t1 ;
    t0 -= (REAL)i0 ;
    t1 -= (REAL)i1 ;
    length = 0.0 ;

    if ( i0==i1 ) 
        RETURN ( c2r_length ( c2r_trim ( C2_PC_BUFFER_ARC(buffer,i0), 
            t0, t1, &trim_arc ) ) ) ;

    c2r_trim ( C2_PC_BUFFER_ARC(buffer,i0), t0, 1.0, &trim_arc ) ;
    length += c2r_length ( c2r_trim ( C2_PC_BUFFER_ARC(buffer,i0), 
        t0, 1.0, &trim_arc ) ) ;
    for ( i=i0+1 ; i<i1 ; i++ )
        length += c2r_length ( C2_PC_BUFFER_ARC(buffer,i) ) ;
    if ( t1 > BBS_ZERO ) 
        length += c2r_length ( c2r_trim ( C2_PC_BUFFER_ARC(buffer,i1), 
            0.0, t1, &trim_arc ) ) ;
    RETURN ( length ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2q_select ( buffer, t0, t1, j0, pt, tol, 
            t_ptr, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
REAL t0, t1 ;
INT j0 ;
PT2 pt ;
REAL tol ;
REAL *t_ptr ;
REAL *dist_ptr ;
{
    INT i ;
    REAL dist, t ;
    BOOLEAN status = FALSE ;

    t0 -= (REAL)j0 ;
    t1 -= (REAL)j0 ;

    for ( i=0 ; i<C2_PC_BUFFER_N(buffer)-1 ; i++ ) {
        if ( c2r_select ( C2_PC_BUFFER_ARC(buffer,i), pt, tol, 
            &t, &dist ) ) {
            t += (REAL) ( i + j0 ) ;
            if ( ( t >= t0 - BBS_ZERO ) && ( t <= t1 + BBS_ZERO ) ) {
                if ( dist_ptr == NULL ) {
                    if ( t_ptr != NULL ) 
                        *t_ptr = t ;
                    RETURN ( TRUE ) ;
                }
                if ( !status || dist < *dist_ptr ) {
                    status = TRUE ;
                    if ( t_ptr != NULL ) 
                        *t_ptr = t ;
                    *dist_ptr = dist ;
                }
            }
        }
    }
    RETURN ( dist_ptr != NULL && *dist_ptr <= tol ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2q_project ( buffer, t0, t1, j0, pt, t_ptr, proj_pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
REAL t0, t1 ;
INT j0 ;
PT2 pt ;
REAL *t_ptr ;
PT2 proj_pt ;
{
    INT i ;
    BOOLEAN status=FALSE ;
    REAL dist, dist_min=0.0, t ;
    PT2 p ;

    t0 -= (REAL)j0 ;
    t1 -= (REAL)j0 ;
    for ( i=0 ; i<C2_PC_BUFFER_N(buffer)-1 ; i++ ) {
        if ( c2r_project ( C2_PC_BUFFER_ARC(buffer,i), pt, &t, p ) ) {
            t += (REAL)i ;
            if ( t0 - BBS_ZERO <= t && t <= t1 + BBS_ZERO ) {
                dist = C2V_DIST ( p, pt ) ;
                if ( !status || dist < dist_min ) {
                    dist_min = dist ;
                    *t_ptr = t + (REAL)j0 ;
                    status = TRUE ;
                    if ( proj_pt != NULL ) 
                        C2V_COPY ( p, proj_pt ) ;
                }
            }
        }
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2q_closed ( buffer )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
{
    RETURN ( C2V_IDENT_PTS ( C2_PC_BUFFER_PT(buffer,0), 
        C2_PC_BUFFER_PT(buffer,C2_PC_BUFFER_N(buffer)-1) ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c2q_ti ( buffer, t, i ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
REAL *t ;
INT *i ;
{
    *i = (INT)(*t-BBS_ZERO) ;
    if ( *i < 0 ) {
        *i = 0 ;
        *t = 0.0 ;
        RETURN ( TRUE ) ;
    }
    else if ( *i >= C2_PC_BUFFER_N(buffer) - 1 ) {
        *i = C2_PC_BUFFER_N(buffer) - 2 ;
        *t -= ( (REAL) C2_PC_BUFFER_N(buffer) - 1.0 ) ;
        RETURN ( *t <= 1.0 + BBS_ZERO ) ;
    }
    else 
        RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2q_ept0 ( buffer, p )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
PT2 p ;
{
    if ( buffer != NULL && C2_PC_BUFFER_N(buffer) > 0 ) {
        C2V_COPY ( C2_PC_BUFFER_PT0(buffer), p ) ;
        RETURN ( TRUE ) ;
    }
    else
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2q_ept1 ( buffer, p )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
PT2 p ;
{
    if ( buffer != NULL && C2_PC_BUFFER_N(buffer) > 0 ) {
        C2V_COPY ( C2_PC_BUFFER_PT1(buffer), p ) ;
        RETURN ( TRUE ) ;
    }
    else
        RETURN ( FALSE ) ;
}

