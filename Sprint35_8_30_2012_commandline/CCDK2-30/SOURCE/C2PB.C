/* -2 -3 */
/********************************** C2PB.C *********************************/
/*************** Routines for processing polycurve geometry ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2pdefs.h>
#include <c2qdefs.h>
#include <c2rdefs.h>
#include <c2adefs.h>
#include <dmldefs.h>
#include <c2pmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_PCURVE c2p_create ( s, n ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT s, n ;
{
    C2_PCURVE pcurve ;

    pcurve = MALLOC ( 1, C2_PCURVE_S ) ;
    if ( pcurve==NULL ) 
        RETURN ( NULL ) ;

    c2q_initialize ( C2_PCURVE_BUFFER(pcurve), s, n ) ;
    C2_PCURVE_BLIST(pcurve) = NULL ;
    RETURN ( pcurve ) ;
}


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE void c2p_box ( pcurve, t0, t1, box ) 
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                          !!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C2_PCURVE pcurve ;
REAL t0, t1 ;
C2_BOX box ;
{
    BOOLEAN status ;
    DML_ITEM item ;
    C2_PC_BUFFER buffer ;
    INT j0 ;

    j0 = 0 ;
    buffer = C2_PCURVE_BUFFER(pcurve) ;
    status = c2q_box ( C2_PCURVE_BUFFER(pcurve), t0, t1, j0, box ) ;
    j0 += ( c2q_n ( buffer ) - 1 ) ;

    C2_WALK_PCURVE ( pcurve, item ) { 
        buffer = (C2_PC_BUFFER)DML_RECORD(item) ;
        if ( status ) 
            c2q_box_append ( buffer, t0, t1, j0, box ) ;
        else
            status = c2q_box ( buffer, t0, t1, j0, box ) ;
        j0 += ( c2q_n ( buffer ) - 1 ) ;
    }
}


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE void c2p_box_append ( pcurve, box ) 
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                          !!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C2_PCURVE pcurve ;
C2_BOX box ;
{
    C2_ASEG arc ;
    arc = c2p_last_segm ( pcurve ) ;
    c2r_box_append ( arc, 0.0, 1.0, box ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2p_free ( pcurve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
{
    if ( pcurve==NULL ) 
        RETURN ;
    c2q_clear ( C2_PCURVE_BUFFER(pcurve) ) ;
    dml_destroy_list ( C2_PCURVE_BLIST(pcurve), ( PF_ACTION ) c2q_free ) ;
    FREE ( pcurve ) ;
}

