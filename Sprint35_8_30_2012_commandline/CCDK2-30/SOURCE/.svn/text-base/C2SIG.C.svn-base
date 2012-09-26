/* -2 -3 */
/********************************** C2SIG.C ********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef  SPLINE
#include <c2bdefs.h>
#include <c2gdefs.h>
#include <c2sdefs.h>
#include <c2adefs.h>
#include <c2apriv.h>
#include <dmldefs.h>
#ifdef DEBUG
#include <c2vmcrs.h>
#endif
STATIC INT inters_arc __(( HPT2*, INT, REAL, REAL, REAL, INT, HPT2*, 
            DML_LIST )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2s_inters_arc ( a, d, knot, w, parm0, parm1, arc, 
            inters_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *a ;  /* Control points */
INT     d ;    /* Order */
REAL    *knot; /* Knots */
REAL    w ;    /* Offset */
PARM parm0, parm1 ;    /* Start and end parameters */
C2_ARC  arc ;    /* Arc */
DML_LIST inters_list ;
{
    HPT2 *b, arc_bez[9] ;
    C2_BOX_S box, arc_box ;
    INT j, inters_no ;

    c2g_box ( arc, 0.0, 2.0, &arc_box ) ;
    inters_no = 0 ;
    b = CREATE ( d, HPT2 ) ;
    c2g_bezs ( arc, TRUE, arc_bez ) ;

    j = PARM_J(parm0) ;
    c2a_box_hpoly ( a+j-d+1, d, &box ) ;

    if ( c2a_box_w_overlap ( &box, &arc_box, fabs(w) ) ) {
        C2S_CONV_BEZ ( a, d, knot, j, b ) ;

        if ( PARM_T(parm0) > knot[j] + BBS_ZERO ) 
            C2B_BRKR ( b, d, ( PARM_T(parm0) - knot[j] ) / 
                ( knot[j+1] - knot[j] ), b ) ;

        if ( PARM_J(parm1) == j ) {
            C2B_BRKL ( b, d, ( PARM_T(parm1) - PARM_T(parm0) ) / 
                ( knot[j+1] - PARM_T(parm0) ), b ) ;
            inters_no += inters_arc ( b, d, w, PARM_T(parm0),
                PARM_T(parm1), j, arc_bez, inters_list ) ;
        }
/* J.Durand bug */
        else if ( knot[j+1] >  PARM_T(parm0) + BBS_ZERO ) 
            inters_no = inters_arc ( b, d, w, 
                PARM_T(parm0), knot[j+1], j, arc_bez, inters_list ) ;
    }

    for ( j=PARM_J(parm0)+1 ; j<PARM_J(parm1) ; j++ ) {

        if ( knot[j+1]-knot[j] > BBS_ZERO ) {
            c2a_box_hpoly ( a+j-d+1, d, &box ) ;

            if ( c2a_box_w_overlap ( &box, &arc_box, fabs(w) ) ) {
                C2S_CONV_BEZ ( a, d, knot, j, b ) ;
                inters_no += inters_arc ( b, d, w, knot[j], knot[j+1], 
                    j, arc_bez, inters_list ) ;
            }
        }
    }

    j = PARM_J(parm1) ;

    if ( j > PARM_J(parm0) && PARM_T(parm1) - knot[j] > BBS_ZERO ) {
        c2a_box_hpoly ( a+j-d+1, d, &box ) ;

        if ( c2a_box_w_overlap ( &box, &arc_box, fabs(w) ) ) {
            C2S_CONV_BEZ ( a, d, knot, j, b ) ;
            if ( PARM_T(parm1) < knot[j+1] - BBS_ZERO ) 
                C2B_BRKL ( b, d, ( PARM_T(parm1) - knot[j] ) / 
                    ( knot[j+1] - knot[j] ), b ) ;
            inters_no += inters_arc ( b, d, w, knot[j], 
                PARM_T(parm1), j, arc_bez, inters_list ) ;
        }
    }
    KILL ( b ) ;
#ifdef DEBUG
{
    DML_ITEM item ;
    C2_INT_REC ci ;
    PT2 p1, p2, p3 ;
    REAL delta1, delta2, delta3 ;

    DML_WALK_LIST ( inters_list, item ) {
        ci = DML_RECORD(item) ;
        c2s_eval ( a, PARM_J(parm1), d, knot, w, C2_INT_REC_PARM1(ci), 
            0, (PT2*)p1 ) ;
        c2g_pt_tan ( arc, C2_INT_REC_T2(ci), p2, NULL ) ;
        c2b_eval ( arc_bez, 3, 0.0, C2_INT_REC_T2(ci), 0, (PT2*)p3 ) ;
        delta1 = C2V_DISTL1 ( C2_INT_REC_PT(ci), p1 ) ;
        delta2 = C2V_DISTL1 ( C2_INT_REC_PT(ci), p2 ) ;
        delta3 = C2V_DISTL1 ( C2_INT_REC_PT(ci), p3 ) ;
    }
}
#endif
    RETURN ( inters_no ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT inters_arc ( b, d, w, t0, t1, j, arc_bez, inters_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *b, *arc_bez ;  /* Control points */
INT     j, d ;    /* Order of  a spline */
REAL    w ;    /* Offset */
REAL    t0, t1 ;
DML_LIST inters_list ;
{
    INT inters_no, i ;

    inters_no = 0 ;

    for ( i=0 ; i<4 ; i++ ) 
        inters_no += c2b_intersect ( b, d, w, t0, t1, j, 
            arc_bez+2*i, 3, 0.0, 0.5*(REAL)(i), 0.5*(REAL)(i+1), i, 
            inters_list ) ;
    RETURN ( inters_no ) ;
}
#endif /*SPLINE*/

