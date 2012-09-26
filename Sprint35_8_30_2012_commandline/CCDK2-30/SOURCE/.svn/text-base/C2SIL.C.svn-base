/* -2 -3 */
/********************************** C2SIL.C ********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef  SPLINE
#include <alsdefs.h>
#include <c2bdefs.h>
#include <c2sdefs.h>
#include <c2apriv.h>
#include <dmldefs.h>
#include <c2lmcrs.h>
#include <c2vmcrs.h>
#include <c2hmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2s_inters_line ( a, n, d, knot, w, parm0, parm1, 
            line, inters_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *a ;  /* Control points */
INT     n, d ;    /* Order */
REAL    *knot; /* Knots */
REAL    w ;    /* Offset */
PARM    parm0, parm1 ;    /* Start and end parameters */
C2_LINE line ;    /* Line */
DML_LIST inters_list ;
{
    INT inters_no, i ;
    PT2 dir, tpt ; 
    DML_ITEM item ;
    REAL c, s, dir_norm ;
    C2_INT_REC ci ;

    C2_LINE_DIR_VEC ( line, dir ) ;
    dir_norm = C2V_NORM ( dir ) ;
    if ( IS_SMALL(dir_norm) ) 
        RETURN ( 0 ) ;
    c = dir[0] / dir_norm ;
    s = dir[1] / dir_norm ;

    c2a_rotate_hpt_cs ( a, n, C2_LINE_PT0(line), c, -s, a ) ;
    inters_no = c2s_inters_hor_line ( a, n, d, knot, w, parm0, parm1, 
        C2_LINE_PT0(line), 0, inters_list ) ;
    c2a_rotate_hpt_cs ( a, n, C2_LINE_PT0(line), c, s, a ) ;

    for ( i=0, item = DML_LAST(inters_list) ; i<inters_no ; 
        i++, item=DML_PREV(item) ) {
        ci = (C2_INT_REC)DML_RECORD(item) ;
        C2V_SUB ( C2_INT_REC_PT(ci), C2_LINE_PT0(line), C2_INT_REC_PT(ci) ) ; 
        C2V_ROTATE_VEC_CS ( C2_INT_REC_PT(ci), c, s, tpt ) ;
        C2V_ADD ( C2_LINE_PT0(line), tpt, C2_INT_REC_PT(ci) ) ; 
        C2_INT_REC_T2(ci) /= dir_norm ;
    }
    RETURN ( inters_no ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2s_inters_hor_line ( a, n, d, knot, w, parm0, parm1, pt, 
        dir, inters_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *a ;  /* Control points */
INT     n, d ;    /* Order */
REAL    *knot; /* Knots */
REAL    w ;    /* Offset */
PARM    parm0, parm1 ;    /* Start and end parameters */
PT2 pt ;
INT dir ;
DML_LIST inters_list ;
{
    INT inters_no, i ;
    DML_ITEM item ;
    C2_INT_REC ci ;

    c2a_sub ( a, n, pt, a ) ;
    inters_no = c2s_inters_x_axis ( a, d, knot, w, 
            parm0, parm1, dir, inters_list ) ;
    C2V_NEGATE ( pt, pt ) ;
    c2a_sub ( a, n, pt, a ) ;
    C2V_NEGATE ( pt, pt ) ;

    for ( i=0, item = DML_LAST(inters_list) ; i<inters_no ; 
        i++, item=DML_PREV(item) ) {
        ci = (C2_INT_REC)DML_RECORD(item) ;
        C2V_ADD ( C2_INT_REC_PT(ci), pt, C2_INT_REC_PT(ci) ) ; 
    }
    RETURN ( inters_no ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2s_inters_x_axis ( a, d, knot, w, parm0, parm1, dir, 
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
PARM    parm0, parm1 ;    /* Start and end parameters */
INT dir ;
DML_LIST inters_list ;
{
    INT inters_no, j ;
    HPT2 *b ;

    inters_no = 0 ;
    b = CREATE ( d, HPT2 ) ;
    j = PARM_J(parm0) ;
    C2S_CONV_BEZ ( a, d, knot, j, b ) ;

    if ( PARM_T(parm0) > knot[j] + BBS_ZERO ) 
        C2B_BRKR ( b, d, ( PARM_T(parm0) - knot[j] ) / 
            ( knot[j+1] - knot[j] ), b ) ;

    if ( PARM_J(parm1) == j ) {
        C2B_BRKL ( b, d, ( PARM_T(parm1) - PARM_T(parm0) ) / 
            ( knot[j+1] - PARM_T(parm0) ), b ) ;
        inters_no = c2b_inters_coord_line ( b, d, w, 
            PARM_T(parm0), PARM_T(parm1), j, 0.0, 1, dir, inters_list ) ;
    }

    else if ( knot[j+1] >  PARM_T(parm0) + BBS_ZERO ) { 
        inters_no = c2b_inters_coord_line ( b, d, w, 
            PARM_T(parm0), knot[j+1], j, 0.0, 1, dir, inters_list ) ;

        for ( j=PARM_J(parm0)+1 ; j<PARM_J(parm1) ; j++ ) {
            if ( knot[j+1]-knot[j] > BBS_ZERO ) {
                C2S_CONV_BEZ ( a, d, knot, j, b ) ;
                inters_no += c2b_inters_coord_line ( b, d, w,
                    knot[j], knot[j+1], j, 0.0, 1, dir, inters_list ) ;
            }
        }

        j = PARM_J(parm1) ;

        if ( PARM_T(parm1) - knot[j] > BBS_ZERO ) {
            C2S_CONV_BEZ ( a, d, knot, j, b ) ;
            if ( PARM_T(parm1) < knot[j+1] - BBS_ZERO )
                C2B_BRKL ( b, d, ( PARM_T(parm1) - knot[j] ) / 
                    ( knot[j+1] - knot[j] ), b ) ;
            inters_no += c2b_inters_coord_line ( b, d, w, 
                knot[j], PARM_T(parm1), j, 0.0, 1, dir, inters_list ) ;
        }
    }
    KILL ( b ) ;
    RETURN ( inters_no ) ;
}


#endif /*SPLINE*/

