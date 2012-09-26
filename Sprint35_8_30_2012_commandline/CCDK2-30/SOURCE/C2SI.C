/* -2 -3 */
/********************************** C2SI.C *********************************/
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
#include <c2gdefs.h>
#include <c2sdefs.h>
#include <c2adefs.h>
#include <c2apriv.h>
#include <dmldefs.h>
#include <c2vmcrs.h>
#include <c2hmcrs.h>

STATIC  INT  inters_splines __(( 
        HPT2*, INT, INT, REAL*, REAL, PARM, PARM, INT, C2_BOX, 
        HPT2*, INT, INT, REAL*, REAL, PARM, PARM, INT, C2_BOX, 
        REAL, DML_LIST )) ;
STATIC INT self_inters __(( HPT2*, INT, INT, REAL*, REAL, PARM, PARM, INT, 
            DML_LIST )) ;
STATIC  void conv_bez __(( HPT2*, INT, REAL*, PARM, PARM, HPT2*, REAL*, 
            REAL* )) ;

#ifdef NEW_CODE
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2s_inters_iter ( a1, n1, d1, knot1, w1, parm1_init, 
        a2, n2, d2, knot2, w2, parm2_init, tol, parm1, parm2, intr_pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2    *a1  ;  /* Control points of the first spline */
INT     n1 ;    /* Number of control points of the first spline */
INT     d1 ;    /* Order of the first spline */
REAL    *knot1; /* Knots of the first spline */
REAL    w1 ;    /* Offset distance of the first spline */
PARM    parm1_init;/* Initial parameter along the first spline */
HPT2    *a2  ;  /* Control points of the second spline */
INT     n2 ;    /* Number of control points of the second spline */
INT     d2 ;    /* Order of the second spline */
REAL    *knot2; /* Knots of the second spline */
REAL    w2 ;    /* Offset distance of the second spline */
PARM    parm2_init; /* Initial parameter along the second spline */
REAL    tol ;   /* Geometric tolerance */
PARM    parm1;  /* Parameter of the intersection point */
PARM    parm2;  /* Parameter of the intersection point */
PT2  intr_pt ;  /* Intersection point */
{

    INT count ;
    PT2 x1[2], x2[2], x ;
    REAL u1, u2, cross ;

    C2_COPY_PARM ( parm1_init, parm1 ) ;
    C2_COPY_PARM ( parm2_init, parm2 ) ;
    for ( count=0 ; ; count++ ) {
        if ( count>=MAX_COUNT )
            RETURN ( FALSE ) ;
    
        if ( c2s_eval ( a1, n1, d1, knot1, w1, parm1, 1, x1 ) ) 
            RETURN ( FALSE ) ;
        if ( c2s_eval ( a2, n2, d2, knot2, w2, parm2, 1, x2 ) ) 
            RETURN ( FALSE ) ;
        cross = C2V_CROSS ( x1[1], x2[1] ) ;

        if ( IS_ZERO(cross) ) 
            RETURN ( FALSE ) ;
        C2V_SUB ( x2[0], x1[0], x ) ;
        u1 = PARM_T(parm1) + C2V_CROSS ( x, x2[1] ) / cross ;
        u2 = PARM_T(parm2) + C2V_CROSS ( x, x1[1] ) / cross ;
        if ( u1 < knot1[d1-1] || u1 > knot1[n1] || 
             u2 < knot2[d2-1] || u2 > knot2[n2] ) 
            RETURN ( FALSE ) ;

        if ( IS_SMALL ( u1 - PARM_T(parm1) ) && 
             IS_SMALL ( u2 - PARM_T(parm2) ) ) {
            if ( c2s_eval ( a1, n1, d1, knot1, w1, parm1, 0, x1 ) )
                RETURN ( FALSE ) ;
            if ( c2s_eval ( a2, n2, d2, knot2, w2, parm2, 0, x2 ) )
                RETURN ( FALSE ) ;
            if ( fabs(x1[0][0]-x2[0][0]) <= tol && 
                fabs(x1[0][1]-x2[0][1]) <= tol ) {
                if ( intr_pt != NULL )
                    C2V_MID_PT ( x1[0], x2[0], intr_pt ) ;
                PARM_T(parm1) = u1 ;
                PARM_T(parm2) = u2 ;
                RETURN ( TRUE ) ;
            }
            else 
                RETURN ( FALSE ) ;
        }
        PARM_T(parm1) = u1 ;
        PARM_T(parm2) = u2 ;
    }
}    
#endif /*NEW_CODE*/

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2s_inters_splines ( a1, n1, d1, knot1, w1, parm10, parm11, 
            a2, n2, d2, knot2, w2, parm20, parm21, inters_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *a1, *a2 ;  /* Control points */
INT     n1, n2 ;    /* Number of control points */
INT     d1, d2 ;    /* Order */
REAL    *knot1, *knot2 ; /* Knots */
REAL    w1, w2 ;    /* Offset */
PARM parm10, parm11, parm20, parm21 ;    /* Start and end parameters */
DML_LIST inters_list ;
{
    C2_BOX_S box1, box2 ;
    REAL w ;

    STACK_AVAIL ;
    c2a_box_hpoly ( a1, n1, &box1 ) ;
    c2a_box_hpoly ( a2, n2, &box2 ) ;
    w = fabs(w1) + fabs(w2) + BBS_TOL ;
    RETURN ( inters_splines ( 
        a1, n1, d1, knot1, w1, parm10, parm11, d1-1, &box1, 
        a2, n2, d2, knot2, w2, parm20, parm21, d2-1, &box2, 
        w, inters_list ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT inters_splines ( a1, k1, d1, knot1, w1, parm10, parm11, j1, box1, 
    a2, k2, d2, knot2, w2, parm20, parm21, j2, box2, w, inters_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *a1, *a2 ;  /* Control points */
INT     k1, k2 ;    /* Number of control points */
INT     d1, d2 ;    /* Order */
REAL    *knot1, *knot2 ; /* Knots */
REAL    w1, w2 ;    /* Offset */
PARM parm10, parm11, parm20, parm21 ;    /* Start and end parameters */
INT     j1, j2 ;    /* Order */
C2_BOX  box1, box2 ;
REAL    w ;
DML_LIST inters_list ;
{
    INT inters_no, i, k ;
    HPT2 *b1, *b2 ;
    REAL u1, v1, u2, v2 ;
    C2_BOX_S sbox1, sbox2 ;

    STACK_AVAIL ;
    if ( PARM_T(parm11) < knot1[d1-1] ) 
        RETURN ( 0 ) ;
    if ( PARM_T(parm10) > knot1[k1] ) 
        RETURN ( 0 ) ;
    if ( PARM_T(parm21) < knot2[d2-1] ) 
        RETURN ( 0 ) ;
    if ( PARM_T(parm20) > knot2[k2] ) 
        RETURN ( 0 ) ;
    if ( !c2a_box_w_overlap ( box1, box2, w ) ) 
        RETURN ( 0 ) ;

    if ( k1 == d1 && k2 == d2 ) {

        if ( knot1[d1]-knot1[d1-1] > BBS_ZERO && 
            knot2[d2]-knot2[d2-1] > BBS_ZERO ) {
            b1 = CREATE ( d1, HPT2 ) ;
            b2 = CREATE ( d2, HPT2 ) ;
            conv_bez ( a1, d1, knot1, parm10, parm11, b1, &u1, &v1 ) ;
            conv_bez ( a2, d2, knot2, parm20, parm21, b2, &u2, &v2 ) ;

            inters_no = c2b_intersect ( b1, d1, w1, u1, v1, j1, 
                b2, d2, w2, u2, v2, j2, inters_list ) ;
            KILL ( b1 ) ;
            KILL ( b2 ) ;
        }
        else 
            inters_no = 0 ;
    }

    else {

        if ( k1 - d1 >= k2 - d2 ) {
            k = ( k1 + d1 - 1 ) / 2 ;
            i = k - d1 + 1 ;
            c2a_box_hpoly ( a1, k, &sbox1 ) ;
            inters_no = inters_splines ( 
                a1, k, d1, knot1, w1, parm10, parm11, j1, &sbox1, 
                a2, k2, d2, knot2, w2, parm20, parm21, j2, box2, 
                w, inters_list ) ;
            c2a_box_hpoly ( a1+i, k1-i, &sbox2 ) ;
            inters_no += inters_splines ( a1+i, k1-i, d1, knot1+i, w1, 
                parm10, parm11, j1+i, &sbox2, 
                a2, k2, d2, knot2, w2, parm20, parm21, j2, box2, 
                w, inters_list ) ;
        }

        else {
            k = ( k2 + d2 - 1 ) / 2 ;
            i = k - d2 + 1 ;
            c2a_box_hpoly ( a2, k, &sbox1 ) ;
            inters_no = inters_splines ( 
                a1, k1, d1, knot1, w1, parm10, parm11, j1, box1,
                a2, k, d2, knot2, w2, parm20, parm21, j2, &sbox1, 
                w, inters_list ) ;
            c2a_box_hpoly ( a2+i, k2-i, &sbox2 ) ;
            inters_no += inters_splines (
                a1, k1, d1, knot1, w1, parm10, parm11, j1, box1,
                a2+i, k2-i, d2, knot2+i, w2,
                parm20, parm21, j2+i, &sbox2, w, inters_list ) ;
        }
    }
    RETURN ( inters_no ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2s_self_inters ( a, n, d, knot, w, parm0, parm1, 
            inters_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *a ;  /* Control points */
INT     n ;    /* Number of control points */
INT     d ;    /* Order */
REAL    *knot ; /* Knots */
REAL    w ;    /* Offset */
PARM    parm0, parm1 ;    /* Start and end parameters */
DML_LIST inters_list ;
{
    RETURN ( self_inters ( a, n, d, knot, w, parm0, parm1, d-1, 
        inters_list ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT self_inters ( a, k, d, knot, w, parm0, parm1, j, inters_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *a ;  /* Control points */
INT     k ;    /* Number of control points */
INT     d ;    /* Order */
REAL    *knot ; /* Knots */
REAL    w ;    /* Offset */
PARM    parm0, parm1 ;    /* Start and end parameters */
INT     j ; 
DML_LIST inters_list ;
{
    INT i, l, m, n ;
    C2_CONE_S dcone ;
    INT conv ; 
    C2_BOX_S box1, box2 ;
    HPT2 *b ;
    REAL u, v ;
    INT inters_no ;
    DML_ITEM item, prev_item ;
    C2_INT_REC ci ;

    if ( PARM_T(parm1) < knot[d-1] ) 
        RETURN ( 0 ) ;
    if ( PARM_T(parm0) > knot[k] ) 
        RETURN ( 0 ) ;

    conv = 0 ;
    c2a_dcone ( a, k, &conv, &dcone ) ;
    if ( conv >= -1 && conv <= 1 ) 
        RETURN ( 0 ) ;

    else if ( k == d ) {
        if ( knot[d]-knot[d-1] > BBS_ZERO ) {
            b = CREATE ( d, HPT2 ) ;
            conv_bez ( a, d, knot, parm0, parm1, b, &u, &v ) ;
            inters_no = c2b_self_intersect ( b, d, w, u, v, j, inters_list ) ;
            KILL ( b ) ;
        }
        else 
            inters_no = 0 ;
    }

    else {
        m = ( k + d - 1 ) / 2 ;
        i = m - d + 1 ;
        inters_no = self_inters ( a, m, d, knot, w, parm0, parm1, 
            j, inters_list ) ;

        c2a_box_hpoly ( a, k, &box1 ) ;
        c2a_box_hpoly ( a+i, k-i, &box2 ) ;
        n = inters_splines ( a, m, d, knot, w, parm0, parm1, j, &box1, 
            a+i, k-i, d, knot+i, w, parm0, parm1, j+i, &box2, 
            2.0*w+BBS_TOL, inters_list ) ;

        inters_no += n ;

        for ( l=0, item=DML_LAST(inters_list) ; l<n && item != NULL ; l++ ) {
            ci = (C2_INT_REC)DML_RECORD(item) ;
            prev_item = DML_PREV(item) ;
            if ( IS_SMALL ( C2_INT_REC_T2(ci) - C2_INT_REC_T1(ci) ) ) {
                dml_remove_item ( inters_list, item ) ;
                inters_no--;
                item = prev_item ;
            }
        }
        inters_no += self_inters ( a+i, k-i, d, knot+i, w, 
            parm0, parm1, j+i, inters_list ) ;
    }

    RETURN ( inters_no ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void conv_bez ( a, d, knot, parm0, parm1, b, u_ptr, v_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *a ;  /* B-spline control points */
INT     d ;    /* Order */
REAL    *knot ; /* Knots */
PARM parm0, parm1 ;    /* Start and end parameters */
HPT2    *b ;  /* Bezier control points */
REAL *u_ptr, *v_ptr ;
{
    als_conv_bez ( (REAL*)a, d, knot, 3, (REAL*)b ) ;

    if ( PARM_T(parm0) > knot[d-1] + BBS_ZERO ) {
        C2B_BRKR ( b, d, ( PARM_T(parm0) - knot[d-1] ) / 
            ( knot[d] - knot[d-1] ), b ) ;
        *u_ptr = PARM_T(parm0) ;
    }
    else
        *u_ptr = knot[d-1] ;

    if ( PARM_T(parm1) < knot[d] - BBS_ZERO ) {
        C2B_BRKL ( b, d, ( PARM_T(parm1) - *u_ptr ) / 
            ( knot[d] - *u_ptr ), b ) ;
        *v_ptr = PARM_T(parm1) ;
    }
    else
        *v_ptr = knot[d] ;
}
#endif  /*SPLINE*/


