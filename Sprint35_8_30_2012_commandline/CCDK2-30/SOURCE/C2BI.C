/* -2 -3 */
/*********************************** C2BI.C ********************************/
/*************** Intersection of two-dimensional Bezier curves *************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef  SPLINE
#include <c2bdefs.h>
#include <c2ddefs.h>
#include <dmldefs.h>
#include <c2vmcrs.h>
#include <c2hmcrs.h>
#include <c2mem.h>
#include <c2apriv.h>
#include <c2coned.h>

STATIC  INT c2bi_inters_bez __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 , REAL DUMMY4 , INT DUMMY5 , INT DUMMY6 , 
            REAL DUMMY7 , C2_CONE DUMMY8 , PT2 DUMMY9 , PT2 DUMMY10 , 
            HPT2* DUMMY11 , INT DUMMY12 , REAL DUMMY13 , REAL DUMMY14 , 
            REAL DUMMY15 , INT DUMMY16 , INT DUMMY17 , REAL DUMMY18 , 
            C2_CONE DUMMY19 , PT2 DUMMY20 , PT2 DUMMY21 , DML_LIST DUMMY22 )) ;
STATIC  INT c2bi_self_inters_bez __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 , REAL DUMMY4 , INT DUMMY5 , INT DUMMY6 , PT2 DUMMY7 , 
            PT2 DUMMY8 , DML_LIST  DUMMY9 )) ;
#ifdef DEBUG
GLOBAL C2B_APPEND = 0 ;
#endif

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2b_intersect ( b1, d1, w1, t01, t11, j1, 
            b2, d2, w2, t02, t12, j2, inters_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2    *b1 ;  /* Control points of the first segment */
INT     d1 ;    /* Order of the first segment */
REAL    w1 ;    /* Offset of the first segment */
REAL    t01, t11 ;
INT     j1 ;
HPT2    *b2 ;  /* Control points of the second segment */
INT     d2 ;    /* Order of the second segment */
REAL    w2 ;    /* Offset of the second segment */
REAL    t02, t12 ;
INT j2 ;
DML_LIST inters_list ;    /* List of intersection records */
{
    INT coinc, m, conv1, conv2 ;
    REAL dist, fl1, fl2 ;
    C2_CONE_S cone1, cone2 ;
    PT2 pt, ept01, ept11, ept02, ept12 ;

    STACK_AVAIL ;

    coinc = c2b_coinc ( b1, d1, w1, b2, d2, w2 ) ;
    if ( coinc != 0 ) {
        m = 0 ;
        c2b_ept0 ( b1, d1, w1, ept01 ) ;
        c2b_ept1 ( b1, d1, w1, ept11 ) ;
        c2b_ept0 ( b2, d2, w2, ept02 ) ;
        c2b_ept1 ( b2, d2, w2, ept12 ) ;
        if ( coinc == 1 ) {
            dist = C2V_DISTL1 ( ept01, ept02 ) ;
            if ( IS_SMALL(dist) ) {
                C2V_MID_PT ( ept01, ept02, pt ) ;
                m += c2b_append_inters_rec ( inters_list, 0.0, t01, 
                    t11, j1, 0.0, t02, t12, j2, pt, dist, -2 ) ;
            }
            dist = C2V_DISTL1 ( ept11, ept12 ) ;
            if ( IS_SMALL(dist) ) {
                C2V_MID_PT ( ept11, ept12, pt ) ;
                m += c2b_append_inters_rec ( inters_list, 1.0, t01, 
                    t11, j1, 1.0, t02, t12, j2, pt, dist, -2 ) ;
            }
        }
        else if ( coinc == -1 ) {
            dist = C2V_DISTL1 ( ept01, ept12 ) ;
            if ( IS_SMALL(dist) ) {
                C2V_MID_PT ( ept01, ept12, pt ) ;
                m += c2b_append_inters_rec ( inters_list, 0.0, t01, 
                    t11, j1, 1.0, t02, t12, j2, pt, dist, -2 ) ;
            }
            dist = C2V_DISTL1 ( ept11, ept02 ) ;
            if ( IS_SMALL(dist) ) {
                C2V_MID_PT ( ept11, ept02, pt ) ;
                m += c2b_append_inters_rec ( inters_list, 1.0, t01, 
                    t11, j1, 0.0, t02, t12, j2, pt, dist, -2 ) ;
            }
        }
        RETURN ( m ) ;
    }

    conv1 = 0 ;
    c2a_dcone ( b1, d1, &conv1, &cone1 ) ;
    if ( !c2b_ept0 ( b1, d1, w1, ept01 ) || !c2b_ept1 ( b1, d1, w1, ept11 ) )
        RETURN ( 0 ) ;
    fl1 = c2a_flat_epts_cone ( ept01, ept11, &conv1, &cone1 ) ;
    conv2 = 0 ;
    c2a_dcone ( b2, d2, &conv2, &cone2 ) ;
    if ( !c2b_ept0 ( b2, d2, w2, ept02 ) || !c2b_ept1 ( b2, d2, w2, ept12 ) )
        RETURN ( 0 ) ;
    fl2 = c2a_flat_epts_cone ( ept02, ept12, &conv2, &cone2 ) ;
    
    RETURN ( c2bi_inters_bez ( 
        b1, d1, w1, t01, t11, j1, conv1, fl1, &cone1, ept01, ept11, 
        b2, d2, w2, t02, t12, j2, conv2, fl2, &cone2, ept02, ept12, 
        inters_list ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2b_self_intersect ( b, d, w, t0, t1, j, inters_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2    *b ;  /* Control points of the first segment */
INT     d ;    /* Order of the first segment */
REAL    w ;    /* Offset of the first segment */
REAL    t0, t1 ;
INT     j ;
DML_LIST inters_list ;    /* List of self intersection records */
{
    INT conv ;
    C2_CONE_S cone ;
    PT2 ept0, ept1 ;

    conv = 0 ;
    c2a_dcone ( b, d, &conv, &cone ) ;
    if ( !c2b_ept0 ( b, d, w, ept0 ) || !c2b_ept1 ( b, d, w, ept1 ) )
        RETURN ( 0 ) ;
    RETURN ( c2bi_self_inters_bez ( b, d, w, t0, t1, j, conv, ept0, ept1, 
        inters_list ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT c2bi_inters_bez ( 
        b1, d1, w1, t01, t11, j1, conv1, fl1, cone1, ept01, ept11, 
        b2, d2, w2, t02, t12, j2, conv2, fl2, cone2, ept02, ept12, 
        inters_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *b1 ;  /* Control points of the first segment */
INT     d1 ;    /* Order of the first segment */
REAL    w1 ;    /* Offset of the first segment */
REAL    t01, t11 ;
INT j1 ;
INT conv1 ;
REAL fl1 ;
C2_CONE cone1 ;
PT2 ept01, ept11 ;
HPT2    *b2 ;  /* Control points of the second segment */
INT     d2 ;    /* Order of the second segment */
REAL    w2 ;    /* Offset of the second segment */
REAL    t02, t12 ;
INT j2 ;
INT conv2 ;
REAL fl2 ;
C2_CONE cone2 ;
PT2 ept02, ept12 ;
DML_LIST inters_list ;    /* List of intersection records */
{
    INT status ;
    PT2 pt ;
    REAL t1, t2, t ;
    REAL fll, flr ;
    HPT2 *bl, *br ;
    INT convl, convr ;
    C2_CONE_S conel, coner ;
    INT n ;
    REAL dist ;
    BOOLEAN overlap ;

    STACK_AVAIL ;
    if ( !c2a_parlgrm_overlap ( ept01, ept11, conv1, cone1, 
            ept02, ept12, conv2, cone2 ) )
        RETURN ( 0 ) ;  /* No intersections */
    overlap = ( abs(conv1) <= 1 ) && ( abs(conv2) <= 1 ) && 
        !c2a_cone_overlap ( cone1, cone2 ) ; 
                /* No more than one intersection */
    if ( overlap && c2b_inters_iter ( b1, d1, w1, ept01, ept11, 
        BBS_ZERO/( t11-t01 ), b2, d2, w2, ept02, ept12, BBS_ZERO/( t12-t02 ), 
        &t1, &t2, &dist, pt ) ) {
#ifdef DEBUG
{
PT2 p1 , p2 ;
c2b_eval_pt ( b1, d1, w1, t1, p1 ) ;
c2b_eval_pt ( b2, d2, w2, t2, p2 ) ;
}
#endif
            RETURN ( c2b_append_inters_rec ( inters_list, t1, t01, t11, j1, 
                t2, t02, t12, j2, pt, dist, 1 ) ) ;
    }
    if ( fl1 < 0.0 ) 
        status = 1 ;
    else if ( fl2 < 0.0 ) 
        status = 2 ;
    else if ( fl2 < fl1 ) 
        status = ( fl1 < sqrt(BBS_TOL) ) ? 0 : 1 ;
    else 
        status = ( fl2 < sqrt(BBS_TOL) ) ? 0 : 2 ;

    if ( status==0 ) 
        RETURN ( c2b_inters_tan ( 
            b1, d1, w1, t01, t11, j1, conv1, fl1, cone1, ept01, ept11, 
            b2, d2, w2, t02, t12, j2, conv2, fl2, cone2, ept02, ept12, 
            inters_list ) ) ;

    if ( status==1 ) {
        if ( !c2b_sbdv_bez ( b1, d1, w1, t01, t11, conv1, ept01, ept11, 
            &bl, &br, &t, &convl, &convr, pt, &fll, &flr, &conel, &coner ) )
            RETURN ( 0 ) ;
        n = c2bi_inters_bez ( 
            bl, d1, w1, t01, t, j1, convl, fll, &conel, ept01, pt, 
            b2, d2, w2, t02, t12, j2, conv2, fl2, cone2, ept02, ept12, 
            inters_list ) +
            c2bi_inters_bez ( 
            br, d1, w1, t, t11, j1, convr, flr, &coner, pt, ept11, 
            b2, d2, w2, t02, t12, j2, conv2, fl2, cone2, ept02, ept12, 
            inters_list ) ;
    }

    else {
        if ( !c2b_sbdv_bez ( b2, d2, w2, t02, t12, conv2, ept02, ept12, 
            &bl, &br, &t, &convl, &convr, pt, &fll, &flr, &conel, &coner ) )
            RETURN ( 0 ) ;
        n = c2bi_inters_bez ( 
            b1, d1, w1, t01, t11, j1, conv1, fl1, cone1, ept01, ept11, 
            bl, d2, w2, t02, t, j2, convl, fll, &conel, ept02, pt, 
            inters_list ) +
            c2bi_inters_bez ( 
            b1, d1, w1, t01, t11, j1, conv1, fl1, cone1, ept01, ept11, 
            br, d2, w2, t, t12, j2, convr, flr, &coner, pt, ept12, 
            inters_list ) ;
    }
    FREE ( bl ) ;
    RETURN ( n ) ;

}


/*-------------------------------------------------------------------------*/
STATIC INT c2bi_self_inters_bez ( b, d, w, t0, t1, j, conv, 
    ept0, ept1, inters_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2    *b ;  /* Control points of the first segment */
INT     d ;    /* Order of the first segment */
REAL    w ;    /* Offset of the first segment */
REAL    t0, t1 ;
INT j ;
INT conv ;
PT2 ept0, ept1 ;
DML_LIST inters_list ;    /* List of intersection records */
{
    PT2 pt, diff ;
    REAL t ;
    REAL fll, flr ;
    HPT2 *bl, *br ;
    INT convl, convr ;
    C2_CONE_S conel, coner ;
    INT inters_no, i, n ;
    DML_ITEM item, prev_item ;
    C2_INT_REC ci ;

    if ( conv>= -1 && conv <= 1 ) 
        RETURN ( 0 ) ;

    if ( !c2b_sbdv_bez ( b, d, w, t0, t1, conv, ept0, ept1, 
            &bl, &br, &t, &convl, &convr, pt, &fll, &flr, &conel, &coner ) )
        RETURN ( 0 ) ;

    t = 0.5 * ( t0 + t1 ) ;
    inters_no = c2bi_self_inters_bez ( bl, d, w, t0, t, j, convl, ept0, pt, 
        inters_list ) ;

    C2H_DIFF ( br[1], br[0], diff ) ;
    if ( !c2a_parlgrm_separated ( ept0, pt, convl, &conel, 
        pt, ept1, convr, &coner, diff ) ) {
        n = c2bi_inters_bez ( bl, d, w, t0, t, j, convl, fll, &conel, ept0, pt, 
            br, d, w, t, t1, j, convr, flr, &coner, pt, ept1, inters_list ) ;
        inters_no += n ;
        for ( i=0, item=DML_LAST(inters_list) ; i<n && item != NULL ; i++ ) {
            ci = (C2_INT_REC)DML_RECORD(item) ;
            prev_item = DML_PREV(item) ;
            if ( IS_SMALL ( C2_INT_REC_T2(ci) - C2_INT_REC_T1(ci) ) ) {
                dml_remove_item ( inters_list, item ) ;
                inters_no--;
                item = prev_item ;
            }
        }
    }
    inters_no += c2bi_self_inters_bez ( br, d, w, t, t1, j, convr, pt, ept1, 
        inters_list ) ;
    FREE ( bl ) ;
    RETURN ( inters_no ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2b_sbdv_bez ( b, d, w, t0, t1, conv, ept0, ept1, 
    bl, br, t, convl, convr, mpt, fll, flr, conel, coner )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *b ;
INT     d ;
REAL    w ;
REAL t0, t1 ;
INT conv ;
PT2 ept0, ept1 ;
HPT2 **bl, **br ;
REAL *t ;
INT *convl, *convr ;
PT2 mpt ;
REAL *fll, *flr ;
C2_CONE conel, coner ;
{
    *bl = MALLOC ( 2*d-1, HPT2 ) ;
    if ( *bl == NULL )
        RETURN ( FALSE ) ;
    *br = *bl + d - 1 ;
    C2B_SBDV0 ( b, d, *bl ) ;
    *convl = conv ;
    *convr = conv ;
    c2a_dcone ( *bl, d, convl, conel ) ;
    c2a_dcone ( *br, d, convr, coner ) ;
    *t = 0.5 * ( t0 + t1 ) ;
    if ( !c2b_ept1 ( *bl, d, w, mpt ) )
        RETURN ( FALSE  ) ;
    *fll = c2a_flat_epts_cone ( ept0, mpt, convl, conel ) ;
    *flr = c2a_flat_epts_cone ( mpt, ept1, convr, coner ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2b_append_inters_rec ( inters_list, t1, t01, t11, j1, 
        t2, t02, t12, j2, pt, dist, type ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST inters_list ;
REAL t1, t01, t11 ;
INT j1 ;
REAL t2, t02, t12 ;
INT j2 ;
PT2 pt ;
REAL dist ;
INT type ;
{
    C2_INT_REC int_rec, old_rec ;
    REAL tol ;
    INT m ;
    DML_ITEM item, item1 ;

#ifdef DEBUG
C2B_APPEND++ ;
#endif
    t1 = t01 + ( t11 - t01 ) * t1 ;
    t2 = t02 + ( t12 - t02 ) * t2 ;

    for ( item = DML_LAST(inters_list) ; item!=NULL ; item = item1 ) {
        item1 = DML_PREV(item) ;
        old_rec = (C2_INT_REC)(DML_RECORD(item)) ;
        m = MAX ( abs(C2_INT_REC_TYPE(old_rec)), abs(type) ) ;
        tol = BBS_ZERO ;
        if ( m == 2 ) 
            tol = sqrt ( tol ) ;
        else if ( m > 2 ) 
            tol = pow ( tol, 1.0 / ( (REAL)m ) ) ;

        if ( fabs ( C2_INT_REC_T1(old_rec) - t1 ) <= tol &&
            fabs ( C2_INT_REC_T2(old_rec) - t2 ) <= tol ) {

            if ( ( type < 0 && C2_INT_REC_TYPE(old_rec) > 0 ) ||
                ( dist < C2_INT_REC_DIST(old_rec) ) ) {
                C2_INT_REC_T1(old_rec) = t1 ;
                C2_INT_REC_T2(old_rec) = t2 ;
                C2_INT_REC_J1(old_rec) = j1 ;
                C2_INT_REC_J2(old_rec) = j2 ;
                C2V_COPY ( pt, C2_INT_REC_PT(old_rec) ) ;
                C2_INT_REC_DIST(old_rec) = dist ;
                C2_INT_REC_TYPE(old_rec) = type ;
                RETURN ( 0 ) ;
            }
            else if ( type < 0 && C2_INT_REC_TYPE(old_rec) == type ) {
                c2d_free_int_rec ( old_rec ) ;
                dml_remove_item ( inters_list, item ) ;
                RETURN ( -1 ) ;
            }
            else if ( type > 0 && C2_INT_REC_TYPE(old_rec) > 0 ) {
                if ( dist < C2_INT_REC_DIST(old_rec) ) {
                    C2_INT_REC_T1(old_rec) = t1 ;
                    C2_INT_REC_T2(old_rec) = t2 ;
                    C2_INT_REC_J1(old_rec) = j1 ;
                    C2_INT_REC_J2(old_rec) = j2 ;
                    C2V_COPY ( pt, C2_INT_REC_PT(old_rec) ) ;
                    C2_INT_REC_DIST(old_rec) = dist ;
                    C2_INT_REC_TYPE(old_rec) = type ;
                }
                RETURN ( 0 ) ;
            }
        }
    }
    
    int_rec = C2_ALLOC_INT_REC ;

    C2_INT_REC_T1(int_rec) = t1 ;
    C2_INT_REC_J1(int_rec) = j1 ;
    C2_INT_REC_T2(int_rec) = t2 ;
    C2_INT_REC_J2(int_rec) = j2 ;
    C2V_COPY ( pt, C2_INT_REC_PT(int_rec) ) ;
    C2_INT_REC_DIST(int_rec) = dist ;
    C2_INT_REC_TYPE(int_rec) = type ;
    C2_INT_REC_NEAR_TAN(int_rec) = FALSE ;
    dml_append_data ( inters_list, int_rec ) ;
    RETURN ( 1 ) ;
}    
#endif  /*SPLINE*/

