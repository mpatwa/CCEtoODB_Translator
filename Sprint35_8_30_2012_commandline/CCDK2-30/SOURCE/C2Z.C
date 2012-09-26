/* -2 -3 */
/*********************************** C2Z.C *********************************/
/*********************** Two-dimensional zero arc curves *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2rdefs.h>
#include <c2vdefs.h>
#include <c2zdefs.h>
#include <c2gmcrs.h>
#include <c2zmcrs.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2z_free ( zero_arc )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ZERO_ARC zero_arc ;     /* Zero_arc to be freed, can be NULL */
{
    FREE ( zero_arc ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ZERO_ARC c2z_create ( ctr, tan0, tan1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr, tan0, tan1 ;
{
    C2_ZERO_ARC zero_arc ;

    zero_arc = MALLOC ( 1, C2_ZERO_ARC_S ) ;
    if ( zero_arc == NULL ) 
        RETURN ( NULL ) ;
    C2V_COPY ( ctr, C2_ZERO_ARC_CTR(zero_arc) ) ;
    C2V_COPY ( tan0, C2_ZERO_ARC_TAN0(zero_arc) ) ;
    C2V_COPY ( tan1, C2_ZERO_ARC_TAN1(zero_arc) ) ;
    RETURN ( zero_arc ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2z_offset ( arc0, w, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ZERO_ARC arc0 ;
REAL w ;
C2_ARC arc ;
{
    if ( IS_SMALL(w) )
        RETURN ( FALSE ) ;
    C2_ARC_ZERO_RAD(arc) = ( c2r_ctr_2tans ( C2_ZERO_ARC_CTR(arc0), w, 
        C2_ZERO_ARC_TAN0(arc0), C2_ZERO_ARC_TAN1(arc0), 
        C2_ARC_SEG(arc) ) == NULL ) ;
    RETURN ( !C2_ARC_ZERO_RAD(arc) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2z_eval ( arc, t, p, x )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ZERO_ARC arc ;
REAL t ;
INT p ;
PT2 *x ;
{
    INT i ;

    if ( p >= 0 ) 
        C2V_COPY ( C2_ZERO_ARC_CTR(arc), x[0] ) ;
    if ( p >= 1 ) {
        if ( IS_ZERO(t) ) {
            C2V_COPY ( C2_ZERO_ARC_TAN0(arc), x[1] ) ;
        }
        else if ( IS_ZERO(t-1.0) ) {
            C2V_COPY ( C2_ZERO_ARC_TAN1(arc), x[1] ) ;
        }
        else 
            RETURN ( FALSE ) ;
    }
    for ( i = 2 ; i <= p ; i++ ) {
        C2V_SET_ZERO ( x[i] ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2z_pt_tan ( arc, t, pt, tan ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ZERO_ARC arc ;
REAL t ;
PT2 pt, tan ;
{
    if ( pt != NULL ) 
        C2V_COPY ( C2_ZERO_ARC_CTR(arc), pt ) ;
    if ( tan != NULL ) {
        if ( IS_ZERO(t) ) {
            C2V_COPY ( C2_ZERO_ARC_TAN0(arc), tan ) ;
        }
        else if ( IS_ZERO(t-1.0) ) {
            C2V_COPY ( C2_ZERO_ARC_TAN1(arc), tan ) ;
        }
        else 
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}

