/* -2 -3 */
/********************************** C2PI.C *********************************/
/*************** Routines for processing polycurve geometry ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2pdefs.h>
#include <c2qdefs.h>
#include <dmldefs.h>
#include <c2mem.h>
#include <c2pmcrs.h>
STATIC INT c2p_inters_buffer __(( C2_PCURVE, C2_PC_BUFFER, INT*, DML_LIST )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2p_intersect ( pcurve1, pcurve2, int_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve1, pcurve2 ;
DML_LIST int_list ;
{
    INT n, k ;
    DML_ITEM item ;

    k = 0 ;
    n = 0 ;
    n += c2p_inters_buffer ( pcurve1, C2_PCURVE_BUFFER(pcurve2), &k, 
        int_list ) ;
    C2_WALK_PCURVE ( pcurve2, item ) 
        n += c2p_inters_buffer ( pcurve1, DML_RECORD(item), &k, int_list ) ;
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT c2p_inters_buffer ( pcurve, buffer, k, int_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
C2_PC_BUFFER buffer ;
INT *k ;
DML_LIST int_list ;
{
    INT n, k1 ;
    DML_ITEM item ;

    k1 = 0 ;
    n = 0 ;
    n += c2q_intersect ( C2_PCURVE_BUFFER(pcurve), &k1, buffer, k, int_list ) ;
    C2_WALK_PCURVE ( pcurve, item ) 
        n += c2q_intersect ( DML_RECORD(item), &k1, buffer, k, int_list ) ;
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2p_self_inters ( pcurve, int_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
DML_LIST int_list ;
{
    RETURN ( c2p_intersect ( pcurve, pcurve, int_list ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2p_inters_line ( pcurve, line, int_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
C2_LINE line ;
DML_LIST int_list ;
{
    INT n, k ;
    DML_ITEM item ;

    k = 0 ;
    n = 0 ;
    n += c2q_inters_line ( C2_PCURVE_BUFFER(pcurve), &k, line, int_list ) ;
    C2_WALK_PCURVE ( pcurve, item ) 
        n += c2q_inters_line ( DML_RECORD(item), &k, line, int_list ) ;
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2p_inters_arc ( pcurve, arc, full_circle, int_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
C2_ARC arc ;
BOOLEAN full_circle ;
DML_LIST int_list ;
{
    INT n, k ;
    DML_ITEM item ;

    k = 0 ;
    n = 0 ;
    n += c2q_inters_arc ( C2_PCURVE_BUFFER(pcurve), &k, arc, 
        full_circle, int_list ) ;
    C2_WALK_PCURVE ( pcurve, item ) 
        n += c2q_inters_arc ( DML_RECORD(item), &k, arc, 
            full_circle, int_list ) ;
    RETURN ( n ) ;
}

