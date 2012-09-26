/* -2 -3 */
/********************************** C2PW.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alwdefs.h>
#include <c2pdefs.h>
#include <c2qdefs.h>
#include <c2pmcrs.h>
#include <dmldefs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_PCURVE c2p_get ( file, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE *file ;
PARM parm0, parm1 ;
{
    INT n ;
    C2_PCURVE pcurve ;

    if ( !alw_get_int ( file, &n ) )
        RETURN ( NULL ) ;
    pcurve = c2p_create ( n, n ) ;
    if ( c2q_get ( file, C2_PCURVE_BUFFER(pcurve), n ) ||
        !alw_get_parm ( file, parm0 ) || !alw_get_parm ( file, parm1 ) ) {
        c2p_free ( pcurve ) ;
        RETURN ( NULL ) ;
    }
    RETURN ( pcurve ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2p_put ( file, pcurve, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE *file ;
C2_PCURVE pcurve ;
PARM parm0, parm1 ;
{
    INT n ;
    DML_ITEM item ;
    C2_PC_BUFFER buffer ;
    BOOLEAN status ;

    n = c2p_n ( pcurve ) ;

    if ( !alw_put_int ( file, n, NULL ) ) 
        RETURN ( FALSE ) ;
    buffer = C2_PCURVE_BUFFER(pcurve) ;
    status = ( c2q_n ( buffer ) > 0 ) ;
    if ( !c2q_put ( file, buffer, FALSE ) ) 
        RETURN ( FALSE ) ;

    C2_WALK_PCURVE ( pcurve, item ) {
        buffer = (C2_PC_BUFFER)DML_RECORD(item) ;
        if ( !c2q_put ( file, buffer, status ) ) 
            RETURN ( FALSE ) ;
        if ( !status ) 
            status = ( c2q_n ( buffer ) > 0 ) ;
    }
    RETURN ( alw_put_parm ( file, parm0, NULL ) && 
        alw_put_parm ( file, parm1, NULL ) ) ;
}

