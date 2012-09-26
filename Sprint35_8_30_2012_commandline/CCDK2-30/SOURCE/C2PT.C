/* -2 -3 */
/********************************** C2PT.C *********************************/
/*************** Routines for processing polycurve geometry ****************/
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
BBS_PRIVATE void c2p_rotate_cs ( pcurve0, origin, c, s, pcurve1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve0, pcurve1 ;
PT2 origin ;
REAL c, s ;
{
    DML_ITEM item0, item1 ;

    c2q_rotate_cs ( C2_PCURVE_BUFFER(pcurve0), origin, c, s, 
        C2_PCURVE_BUFFER(pcurve0) ) ;
    for ( item0 = dml_first ( C2_PCURVE_BLIST(pcurve0) ), 
          item1 = dml_first ( C2_PCURVE_BLIST(pcurve1) ) ;
          item0 != NULL && item1 != NULL ;
          item0 = dml_next ( item0 ) , item1 = dml_next ( item1 ) ) 
        c2q_rotate_cs ( DML_RECORD(item0), origin, c, s, DML_RECORD(item0) ) ;
}



/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2p_transform ( pcurve0, t, pcurve1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve0, pcurve1 ;
C2_TRANSFORM t ;
{
    DML_ITEM item0, item1 ;

    c2q_transform ( C2_PCURVE_BUFFER(pcurve0), t, C2_PCURVE_BUFFER(pcurve0) ) ;
    for ( item0 = dml_first ( C2_PCURVE_BLIST(pcurve0) ), 
          item1 = dml_first ( C2_PCURVE_BLIST(pcurve1) ) ;
          item0 != NULL && item1 != NULL ;
          item0 = dml_next ( item0 ) , item1 = dml_next ( item1 ) ) 
        c2q_transform ( DML_RECORD(item0), t, DML_RECORD(item0) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2p_translate ( pcurve, shift ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
PT2 shift ;
{
    DML_ITEM item ;

    c2q_translate ( C2_PCURVE_BUFFER(pcurve), shift ) ;
    C2_WALK_PCURVE ( pcurve, item )  
        c2q_translate ( DML_RECORD(item), shift ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2p_scale ( pcurve, origin, factor ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
PT2 origin ;
REAL factor ;
{
    DML_ITEM item ;

    c2q_scale ( C2_PCURVE_BUFFER(pcurve), origin, factor ) ;
    C2_WALK_PCURVE ( pcurve, item )  
        c2q_scale ( DML_RECORD(item), origin, factor ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2p_mirror_dir ( pcurve, origin, normal ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
PT2 origin, normal ;
{
    DML_ITEM item ;

    c2q_mirror_dir ( C2_PCURVE_BUFFER(pcurve), origin, normal ) ;
    C2_WALK_PCURVE ( pcurve, item )  
        c2q_mirror_dir ( DML_RECORD(item), origin, normal ) ;
}

