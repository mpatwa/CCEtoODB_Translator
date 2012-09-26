/* -2 -3 */
/********************************** C2P1.C *********************************/
/*************** Routines for processing polycurve geometry ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2pdefs.h>
#include <c2qdefs.h>
#include <c2pmcrs.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2p_approx ( pcurve, t0, t1, gran, dir, 
        pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
REAL t0, t1 ;
REAL gran ;
BOOLEAN dir ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    DML_ITEM item ;
    INT j, n ;
    C2_PC_BUFFER buffer ;

    if ( dir ) {
        j = 0 ;
        buffer = C2_PCURVE_BUFFER(pcurve) ; 
        if ( !c2q_approx ( buffer, t0, t1, j, gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, end_t ) )
            RETURN ( FALSE ) ;
        j += ( c2q_n ( buffer ) - 1 ) ;

        C2_WALK_PCURVE ( pcurve, item ) {
            buffer = (C2_PC_BUFFER)DML_RECORD(item) ;
            if ( !c2q_approx ( buffer, t0, t1, j, gran, dir, 
                pt_buffer, parm_buffer, buf_size, index, end_t ) )
                RETURN ( FALSE ) ;
            j += ( c2q_n ( buffer ) - 1 ) ;
        }
    }
    else {
        n = c2p_n ( pcurve ) ;
        t0 -= ( (REAL)n - 1.0 ) ;
        t1 -= ( (REAL)n - 1.0 ) ;
        j = n ;
        for ( item = dml_last ( C2_PCURVE_BLIST(pcurve) ) ; 
              item != NULL ; item = dml_prev ( item ) ) {
            buffer = (C2_PC_BUFFER)DML_RECORD(item) ;
            j -= ( c2q_n ( buffer ) - 1 ) ;
            if ( !c2q_approx ( DML_RECORD(item), t0, t1, j, gran, dir, 
                pt_buffer, parm_buffer, buf_size, index, end_t ) )
                RETURN ( FALSE ) ;
        }
        if ( !c2q_approx ( C2_PCURVE_BUFFER(pcurve), t0, t1, 0, gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, end_t ) )
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2p_approx_zoomed ( pcurve, t0, t1, gran, x, y, w, 
            pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
REAL t0, t1 ;
REAL gran ;
REAL x, y, w ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    DML_ITEM item ;
    INT j ;
    C2_PC_BUFFER buffer ;

    j = 0 ;
    buffer = C2_PCURVE_BUFFER(pcurve) ; 
    if ( !c2q_approx_zoomed ( buffer, t0, t1, j, gran, x, y, w, 
        pt_buffer, parm_buffer, buf_size, index, end_t ) )
        RETURN ( FALSE ) ;
    j += ( c2q_n ( buffer ) - 1 ) ;

    C2_WALK_PCURVE ( pcurve, item ) {
        buffer = (C2_PC_BUFFER)DML_RECORD(item) ;
        if ( !c2q_approx_zoomed ( buffer, t0, t1, j, gran, x, y, w, 
            pt_buffer, parm_buffer, buf_size, index, end_t ) )
            RETURN ( FALSE ) ;
        j += ( c2q_n ( buffer ) - 1 ) ;
    }
    RETURN ( TRUE ) ;
}

