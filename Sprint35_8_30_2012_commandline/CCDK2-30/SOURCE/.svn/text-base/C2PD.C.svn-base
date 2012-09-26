/* -2 -3 */
/********************************** C2PD.C *********************************/
/**************** Routines for processing polycurve geometry ***************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2pdefs.h>
#include <c2qdefs.h>
#include <dmldefs.h>
#include <c2pmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_PCURVE c2p_create_copy ( pcurve0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve0 ;
{
    C2_PCURVE pcurve ;
    C2_PC_BUFFER buffer0 ;

    buffer0 = C2_PCURVE_BUFFER(pcurve0) ;
    pcurve = c2p_create ( c2q_size ( buffer0 ), c2q_n ( buffer0 ) ) ;
    if ( pcurve == NULL ) 
        RETURN ( NULL ) ;
    c2q_copy ( buffer0, C2_PCURVE_BUFFER(pcurve) ) ;
    C2_PCURVE_BLIST(pcurve) = 
        dml_recreate ( C2_PCURVE_BLIST(pcurve0), (PF_ANY1)c2q_create_copy ) ; 
    RETURN ( pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PCURVE c2p_add ( pcurve, pt0, pt1, d, tol ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
PT2 pt0, pt1 ;
REAL d, tol ;
{
    RETURN ( c2q_add_2pts_d ( c2p_last_buffer(pcurve), pt0, pt1, d, tol ) 
        == NULL ? NULL : pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PCURVE c2p_add_arc_2pts ( pcurve, a0, a1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
PT2 a0, a1 ;
{
    RETURN ( c2q_add_arc_2pts ( c2p_last_buffer(pcurve), a0, a1 ) == NULL ? 
        NULL : pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PCURVE c2p_add_arc_ctr_pt ( pcurve, ctr, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
PT2 ctr, a ;
{
    RETURN ( c2q_add_arc_ctr_pt ( c2p_last_buffer(pcurve), ctr, a ) == NULL ?
        NULL : pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PCURVE c2p_add_arc_tan ( pcurve, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
PT2 a ;
{
    RETURN ( c2q_add_arc_tan ( c2p_last_buffer(pcurve), a ) == NULL ? 
        NULL : pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PCURVE c2p_add_arc_tan0 ( pcurve, a, tan0 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
PT2 a, tan0 ;
{
    RETURN ( c2q_add_arc_tan0 ( c2p_last_buffer(pcurve), a, tan0 ) == 
        NULL ? NULL : pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PCURVE c2p_add_line ( pcurve, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
PT2 a ;
{
    RETURN ( c2q_add_line ( c2p_last_buffer(pcurve), a ) == NULL ? 
        NULL : pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PCURVE c2p_add_line_tan ( pcurve, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
PT2 a ;
{
    RETURN ( c2q_add_line_tan ( c2p_last_buffer(pcurve), a ) == NULL ? 
        NULL : pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PCURVE c2p_remove_last ( pcurve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
{
    RETURN ( c2q_remove_last ( c2p_last_buffer(pcurve) ) == NULL ? 
        NULL : pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PCURVE c2p_close ( pcurve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
{
    PT2 p0 ;
    c2p_ept0 ( pcurve, p0 ) ;
    RETURN ( c2p_add_line ( pcurve, p0 ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PCURVE c2p_trim ( pcurve0, t0, t1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve0 ;
REAL t0, t1 ;
{
    DML_ITEM item ;
    C2_PCURVE pcurve1 = c2p_create ( 0, 0 ) ;
    C2_PC_BUFFER buffer0, buffer ;
    INT j0 ;

    j0 = 0 ;
    buffer0 = C2_PCURVE_BUFFER(pcurve0) ;
    c2q_trim ( buffer0, t0, t1, j0, C2_PCURVE_BUFFER(pcurve1) ) ; 
    j0 += ( c2q_n ( buffer0 ) - 1 ) ;

    C2_WALK_PCURVE ( pcurve0, item ) {
        buffer0 = (C2_PC_BUFFER)DML_RECORD(item) ;
        buffer = c2q_trim ( buffer0, t0, t1, j0, NULL ) ; 
        j0 += ( c2q_n ( buffer0 ) - 1 ) ;
        if ( buffer != NULL ) {
            if ( C2_PCURVE_BLIST(pcurve1) == NULL ) 
                C2_PCURVE_BLIST(pcurve1) = dml_create_list () ;
            dml_append_data ( C2_PCURVE_BLIST(pcurve1), buffer ) ; 
        }
    }
    RETURN ( pcurve1 ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PCURVE c2p_through ( a, n ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a      ;   /* a[n] */
INT n ;
{
    C2_PCURVE pcurve ;

    pcurve = c2p_create ( n, n ) ;
    if ( pcurve == NULL ) 
        RETURN ( NULL ) ;
    c2q_through ( a, n, C2_PCURVE_BUFFER(pcurve) ) ;
    RETURN ( pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2p_init ( pcurve, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
PT2 a ;
{
    C2_PC_BUFFER buffer ;
    buffer = c2p_first_buffer ( pcurve ) ;
    if ( buffer == NULL ) 
        RETURN ;
    c2q_init ( buffer, a ) ;
}

