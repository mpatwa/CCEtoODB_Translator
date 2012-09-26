/* -2 -3 */
/******************************* C2PO.C *********************************/ 
/*************** Two-dimensional spline offset routines *****************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                      !!!!!!!!*/ 
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <c2pdefs.h>
#include <c2qdefs.h>
#include <c2pmcrs.h>
STATIC INT c2p_offset_buffer __(( C2_PC_BUFFER, REAL, DML_LIST )) ;

/*-----------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST c2p_offset ( pcurve, offset ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
REAL offset ;
{
    DML_LIST offset_list = dml_create_list () ;
    DML_ITEM item ;

    c2p_offset_buffer ( C2_PCURVE_BUFFER(pcurve), offset, offset_list ) ;
    C2_WALK_PCURVE ( pcurve, item ) 
        c2p_offset_buffer ( DML_RECORD(item), offset, offset_list ) ;
    RETURN ( offset_list ) ;
}


/*-----------------------------------------------------------------------*/
STATIC INT c2p_offset_buffer ( buffer, offset, offset_list ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
REAL offset ;
DML_LIST offset_list ;
{
    C2_PCURVE offset_curve ;
    INT size, k, n ;

    k = 0 ;
    n = 0 ;
    while ( k < c2q_n ( buffer ) ) {
        size = 3 * ( c2q_n ( buffer ) - k ) / 2 ;
        offset_curve = c2p_create ( size, 1 ) ;
        if ( offset_curve == NULL ) 
            RETURN ( 0 ) ;
        k += c2q_offset ( buffer, k, offset, C2_PCURVE_BUFFER(offset_curve) ) ;
        if ( dml_append_data ( offset_list, offset_curve ) == NULL )
            RETURN ( n ) ;
        n++ ;
    }
    RETURN ( n ) ;
}

