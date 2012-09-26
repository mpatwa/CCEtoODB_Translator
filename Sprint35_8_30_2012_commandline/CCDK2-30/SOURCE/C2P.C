/* -2 -3 */
/********************************** C2P.C **********************************/
/*************** Routines for processing polycurve geometry ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2pdefs.h>
#include <c2qdefs.h>
#include <c2rdefs.h>
#include <dmldefs.h>
#include <c2pmcrs.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2p_eval ( pcurve, t, p, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
REAL t ;
INT p ;
PT2 *x ;
{
    BOOLEAN status = TRUE ;
    DML_ITEM item ;

    if ( t < 0.0 ) {
        t = 0.0 ;
        status = FALSE ;
    }
    if ( c2q_eval ( C2_PCURVE_BUFFER(pcurve), t, p, x ) )
        RETURN ( status ) ;
    C2_WALK_PCURVE ( pcurve, item ) { 
        if ( c2q_eval ( DML_RECORD(item), t, p, x ) )
            RETURN ( TRUE ) ;
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2p_n ( pcurve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
{
    INT n ;
    DML_ITEM item ;

    n = c2q_n ( C2_PCURVE_BUFFER(pcurve) ) - 1 ;
    C2_WALK_PCURVE ( pcurve, item ) 
        n += ( c2q_n ( DML_RECORD(item) ) - 1 ) ;
    n++ ;
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2p_segment ( pcurve, i ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
INT i ;
{
    DML_ITEM item ;
    C2_ASEG arc ;

    if ( i < 0 ) 
        RETURN ( NULL ) ;
    arc = c2q_segment ( C2_PCURVE_BUFFER(pcurve), &i ) ;
    if ( arc != NULL ) 
        RETURN ( arc ) ;
    C2_WALK_PCURVE ( pcurve, item ) {
        arc = c2q_segment ( DML_RECORD(item), &i ) ;
        if ( arc != NULL ) 
            RETURN ( arc ) ;
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2p_segment_epts_d ( pcurve, i, ept0, ept1, d_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
INT i ;
PT2 ept0, ept1 ;
REAL *d_ptr ;
{
    C2_ASEG arc = c2p_segment ( pcurve, i ) ;
    if ( arc == NULL ) 
        RETURN ( FALSE ) ;
    else {
        c2r_get_epts_d ( arc, ept0, ept1, d_ptr ) ;
        RETURN ( TRUE ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2p_first_segm ( pcurve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
{
    C2_ASEG arc ;

    arc = c2q_first_segm ( C2_PCURVE_BUFFER(pcurve) ) ;
    RETURN ( arc != NULL ? arc :
        c2q_first_segm ( dml_first_record ( C2_PCURVE_BLIST(pcurve) ) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c2p_last_segm ( pcurve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
{
    C2_ASEG arc ;
    arc = c2q_last_segm ( dml_last_record ( C2_PCURVE_BLIST(pcurve) ) ) ;
    RETURN ( arc != NULL ? arc : c2q_last_segm ( C2_PCURVE_BUFFER(pcurve) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_PC_BUFFER c2p_first_buffer ( pcurve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
{
    C2_PC_BUFFER buffer ;
    buffer = C2_PCURVE_BUFFER(pcurve) ;
    if ( c2q_n ( buffer ) > 0 ) 
        RETURN ( buffer ) ;
    else
        RETURN ( (C2_PC_BUFFER)dml_first_record ( C2_PCURVE_BLIST(pcurve) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_PC_BUFFER c2p_last_buffer ( pcurve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
{
    C2_PC_BUFFER buffer ;
    buffer = (C2_PC_BUFFER)dml_last_record ( C2_PCURVE_BLIST(pcurve) ) ;
    if ( buffer != NULL && c2q_n ( buffer ) > 0 ) 
        RETURN ( buffer ) ;
    buffer = C2_PCURVE_BUFFER(pcurve) ;
    RETURN ( c2q_n ( buffer ) > 0 ? buffer : NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2p_pt_tan ( pcurve, t, pt, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
REAL t ;
PT2 pt, tan_vec ;
{
    BOOLEAN status = TRUE ;
    DML_ITEM item ;

    if ( t < 0.0 ) {
        t = 0.0 ;
        status = FALSE ;
    }
    if ( c2q_pt_tan ( C2_PCURVE_BUFFER(pcurve), t, pt, tan_vec ) )
        RETURN ( status ) ;
    C2_WALK_PCURVE ( pcurve, item ) { 
        if ( c2q_pt_tan ( DML_RECORD(item), t, pt, tan_vec ) )
            RETURN ( TRUE ) ;
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2p_curvature ( pcurve, t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
REAL t ;
{
    DML_ITEM item ;
    REAL crv ;

    if ( t < 0.0 ) 
        t = 0.0 ;
    if ( c2q_curvature ( C2_PCURVE_BUFFER(pcurve), t, &crv ) )
        RETURN ( crv ) ;
    C2_WALK_PCURVE ( pcurve, item ) { 
        if ( c2q_curvature ( DML_RECORD(item), t, &crv ) )
            RETURN ( crv ) ;
    }
    RETURN ( 0.0 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2p_coord_extrs ( pcurve, t0, t1, coord, extr_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
REAL t0, t1 ;
INT     coord ;
DML_LIST extr_list ;
{
    INT j0, m ;
    DML_ITEM item ;
    C2_PC_BUFFER buffer ;

    j0 = 0 ;
    buffer = C2_PCURVE_BUFFER(pcurve) ;
    m = c2q_coord_extrs ( buffer, t0, t1, j0, coord, extr_list ) ;
    j0 += ( c2q_n ( buffer ) - 1 ) ;

    C2_WALK_PCURVE ( pcurve, item ) {
        buffer = (C2_PC_BUFFER)DML_RECORD(item) ;
        m += c2q_coord_extrs ( buffer, t0, t1, j0, coord, extr_list ) ;
        j0 += ( c2q_n ( buffer ) - 1 ) ;
    }
    RETURN ( m ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2p_length ( pcurve, t0, t1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
REAL t0, t1 ;
{
    REAL length ;
    DML_ITEM item ;
    C2_PC_BUFFER buffer ;
    INT j0 ;

    j0 = 0 ;
    buffer = C2_PCURVE_BUFFER(pcurve) ;
    length = c2q_length ( buffer, t0, t1, j0 ) ;
    j0 += ( c2q_n ( buffer ) - 1 ) ;

    C2_WALK_PCURVE ( pcurve, item ) {
        buffer = (C2_PC_BUFFER)DML_RECORD(item) ;
        length += c2q_length ( buffer, t0, t1, j0 ) ;
        j0 += ( c2q_n ( buffer ) - 1 ) ;
    }
    RETURN ( length ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2p_select ( pcurve, t0, t1, pt, tol, t_ptr, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
REAL t0, t1 ;
PT2 pt ;
REAL tol ;
REAL *t_ptr ;
REAL *dist_ptr ;
{
    DML_ITEM item ;
    C2_PC_BUFFER buffer ;
    INT j0 ;

    j0 = 0 ;
    buffer = C2_PCURVE_BUFFER(pcurve) ;
    if ( c2q_select ( buffer, t0, t1, j0, pt, tol, t_ptr, dist_ptr ) )
        RETURN ( TRUE ) ;
    j0 += ( c2q_n ( buffer ) - 1 ) ;

    C2_WALK_PCURVE ( pcurve, item ) {
        buffer = (C2_PC_BUFFER)DML_RECORD(item) ;
        if ( c2q_select ( buffer, t0, t1, j0, pt, tol, t_ptr, dist_ptr ) )
            RETURN ( TRUE ) ;
        j0 += ( c2q_n ( buffer ) - 1 ) ;
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2p_project ( pcurve, t0, t1, pt, t_ptr, proj_pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
REAL t0, t1 ;
PT2 pt ;
REAL *t_ptr ;
PT2 proj_pt ;
{
    DML_ITEM item ;
    C2_PC_BUFFER buffer ;
    INT j0 ;

    j0 = 0 ;
    buffer = C2_PCURVE_BUFFER(pcurve) ;
    if ( c2q_project ( buffer, t0, t1, j0, pt, t_ptr, proj_pt ) )
        RETURN ( TRUE ) ;
    j0 += ( c2q_n ( buffer ) - 1 ) ;

    C2_WALK_PCURVE ( pcurve, item ) {
        buffer = (C2_PC_BUFFER)DML_RECORD(item) ;
        if ( c2q_project ( buffer, t0, t1, j0, pt, t_ptr, proj_pt ) )
            RETURN ( TRUE ) ;
        j0 += ( c2q_n ( buffer ) - 1 ) ;
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2p_closed ( pcurve )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
{
    PT2 p0, p1 ;

    RETURN ( c2p_ept0 ( pcurve, p0 ) && c2p_ept1 ( pcurve, p1 ) && 
        C2V_IDENT_PTS ( p0, p1 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2p_ept0 ( pcurve, p )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
PT2 p ;
{
    DML_ITEM item ;

    if ( c2q_ept0 ( C2_PCURVE_BUFFER(pcurve), p ) ) 
        RETURN ( TRUE ) ;
    C2_WALK_PCURVE ( pcurve, item ) { 
        if ( c2q_ept0 ( DML_RECORD(item), p ) ) 
            RETURN ( TRUE ) ;
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2p_ept1 ( pcurve, p )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
PT2 p ;
{
    DML_ITEM item ;

    for ( item = dml_last ( C2_PCURVE_BLIST(pcurve) ) ; 
        item != NULL ; item = dml_prev ( item ) ) {
        if ( c2q_ept1 ( DML_RECORD(item), p ) ) 
            RETURN ( TRUE ) ;
    }
    if ( c2q_ept1 ( C2_PCURVE_BUFFER(pcurve), p ) ) 
        RETURN ( TRUE ) ;
    RETURN ( FALSE ) ;
}


/*--------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2p_get_data ( pcurve, i, ctr, ept1, rad_ptr, 
    st_angle_ptr, sweep_ptr, dir_ptr ) 
/*--------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
INT i ;
PT2 ctr, ept1 ;
REAL *rad_ptr, *st_angle_ptr, *sweep_ptr ;
INT *dir_ptr ;
{
    RETURN ( c2r_get_data ( c2p_segment ( pcurve, i ), 
        ctr, ept1, rad_ptr, st_angle_ptr, sweep_ptr, dir_ptr ) ) ;
}

