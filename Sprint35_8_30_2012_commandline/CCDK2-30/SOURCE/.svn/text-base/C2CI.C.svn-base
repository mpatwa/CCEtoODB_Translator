/* -2 -3 */
/******************************* C2CI.C *********************************/ 
/**************** Two-dimensional curves ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alrdefs.h>
#include <c2gdefs.h>
#include <c2ldefs.h>
#include <c2pdefs.h>
#include <c2ndefs.h>
#include <c2adefs.h>
#include <c2cdefs.h>
#include <c2cpriv.h>
#include <c2ddefs.h>
#include <dmldefs.h>
#include <c2mem.h>
#include <c2vmcrs.h>

#ifdef DDEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif
STATIC  INT     c2c_coinc_lines __(( C2_CURVE DUMMY0 , C2_CURVE DUMMY1 , 
            DML_LIST  DUMMY2 )) ;
STATIC  INT     c2c_coinc_arcs __(( C2_CURVE DUMMY0 , C2_CURVE DUMMY1 , 
            DML_LIST  DUMMY2 )) ;
STATIC  INT     c2c_remove_end_inters __(( PARM DUMMY0 , INT DUMMY1 , 
            DML_LIST DUMMY2 , INT  DUMMY3 )) ;
STATIC  void    c2c_inters_rec_swap __(( DML_LIST DUMMY0 , INT  DUMMY1 )) ;
#ifdef  SPLINE
STATIC  void    c2c_compare_knots __(( C2_CURVE DUMMY0 , C2_CURVE DUMMY1 , 
            DML_LIST DUMMY2 , INT  DUMMY3 )) ;
STATIC  void    c2c_compare_knots_curve __(( C2_CURVE DUMMY0 , REAL DUMMY1 , 
            REAL* DUMMY2 , INT  DUMMY3 )) ;
#endif  /* SPLINE */

/*----------------------------------------------------------------------*/
BBS_PUBLIC INT c2c_intersect ( curve1, curve2, inters_list )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C2_CURVE curve1, curve2 ;
DML_LIST inters_list ;
{
    INT i, n, m ;
    DML_ITEM item, prev_item ;
    C2_INT_REC int_rec ;

    STACK_AVAIL ;
    if ( curve1 == curve2 ) 
        RETURN ( c2c_self_intersect ( curve1, inters_list ) ) ;
#ifdef DDEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_curve ( curve1, 5 ) ;
    paint_box ( C2_CURVE_BOX(curve1), 13 ) ;
    paint_curve ( curve2, 6 ) ;
    paint_box ( C2_CURVE_BOX(curve2), 14 ) ;
    getch();
}
else
    DISPLAY-- ;
#endif

    if ( !c2a_box_overlap ( C2_CURVE_BOX(curve1), C2_CURVE_BOX(curve2) ) ) 
        RETURN ( 0 ) ;

    m = c2c_intersect_ext ( curve1, curve2, inters_list ) ;

    n = m ;

    if ( n == -1 ) {
        if ( C2_CURVE_IS_LINE(curve1) && C2_CURVE_IS_LINE(curve2) ) 
            RETURN ( c2c_coinc_lines ( curve1, curve2, inters_list ) ) ;
        else if ( C2_CURVE_IS_ARC(curve1) && C2_CURVE_IS_ARC(curve2) ) 
            RETURN ( c2c_coinc_arcs ( curve1, curve2, inters_list ) ) ;
    }

    item = DML_LAST(inters_list) ;

    for ( i=0 ; i<m ; i++ ) {
        prev_item = DML_PREV(item) ;

        int_rec = (C2_INT_REC) DML_RECORD(item) ;

        if ( C2_INT_REC_T1(int_rec) < C2_CURVE_T0(curve1) - BBS_ZERO || 
             C2_INT_REC_T1(int_rec) > C2_CURVE_T1(curve1) + BBS_ZERO || 
             C2_INT_REC_T2(int_rec) < C2_CURVE_T0(curve2) - BBS_ZERO || 
             C2_INT_REC_T2(int_rec) > C2_CURVE_T1(curve2) + BBS_ZERO ) {
            C2_FREE_INT_REC(int_rec) ;
            dml_remove_item ( inters_list, item ) ;
            n-- ;
        }
        item = prev_item ;
    }
    RETURN ( n ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC INT c2c_self_intersect ( curve, inters_list )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
DML_LIST inters_list ;
{
    INT n ;

    if ( C2_CURVE_IS_LINE(curve) ) 
        RETURN ( 0 ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        RETURN ( 0 ) ;
    else if ( C2_CURVE_IS_PCURVE(curve) ) {
        n = c2p_self_inters ( C2_CURVE_PCURVE(curve), inters_list ) ;
        if ( c2c_closed ( curve ) )
            n -= c2c_remove_end_inters ( C2_CURVE_PARM1(curve), 
                1, inters_list, n ) ;
        RETURN ( n ) ;
    }
#ifdef  SPLINE
    else if ( C2_CURVE_IS_SPLINE(curve) ) {
        n = c2n_self_inters ( C2_CURVE_NURB(curve), C2_CURVE_PARM0(curve), 
            C2_CURVE_PARM1(curve), inters_list ) ;
        if ( c2c_closed ( curve ) )
            n -= c2c_remove_end_inters ( C2_CURVE_PARM1(curve), 
                1, inters_list, n ) ;
        c2c_compare_knots ( curve, curve, inters_list, n ) ;
        RETURN ( n ) ;
    }
#endif  /* SPLINE */
    else 
        RETURN ( 0 ) ;
}


/*----------------------------------------------------------------------*/
STATIC INT c2c_coinc_lines ( curve1, curve2, inters_list )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C2_CURVE curve1, curve2 ;
DML_LIST inters_list ;
{
    REAL t1[2], t2[2] ;
    PT2 int_pt[2] ;
    INT i, m, n, type[2] ;
    PARM_S parm1, parm2 ;

    n = c2l_intersect_coinc ( 
        C2_CURVE_LINE(curve1), C2_CURVE_T0(curve1), C2_CURVE_T1(curve1), 
        C2_CURVE_LINE(curve2), C2_CURVE_T0(curve2), C2_CURVE_T1(curve2), 
        t1, t2, int_pt, type ) ;

    m = 0 ;
    for ( i=0 ; i< n ; i++ ) {
        PARM_J(&parm1) = 1 ;
        PARM_T(&parm1) = t1[i] ;
        PARM_J(&parm2) = 1 ;
        PARM_T(&parm2) = t2[i] ;
        if ( c2c_append_int_rec ( inters_list, &parm1, &parm2, 
            int_pt[i], 0.0, type[i], FALSE ) ) 
            m++ ;
    }
    RETURN ( m ) ;
}


/*----------------------------------------------------------------------*/
STATIC INT c2c_coinc_arcs ( curve1, curve2, inters_list )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C2_CURVE curve1, curve2 ;
DML_LIST inters_list ;
{
    REAL t1[4], t2[4] ;
    PT2 int_pt[4] ;
    INT i, m, n, type[4] ;
    PARM_S parm1, parm2 ;

    n = c2g_intersect_coinc ( 
        C2_CURVE_ARC(curve1), C2_CURVE_T0(curve1), C2_CURVE_T1(curve1), 
        C2_CURVE_ARC(curve2), C2_CURVE_T0(curve2), C2_CURVE_T1(curve2), 
        t1, t2, int_pt, type ) ;

    m = 0 ;
    for ( i=0 ; i< n ; i++ ) {
        PARM_J(&parm1) = 1 ;
        PARM_T(&parm1) = t1[i] ;
        PARM_J(&parm2) = 1 ;
        PARM_T(&parm2) = t2[i] ;
        if ( c2c_append_int_rec ( inters_list, &parm1, &parm2, 
            int_pt[i], 0.0, type[i], FALSE ) )
            m++ ;
    }
    RETURN ( m ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC INT c2c_intersect_ext ( curve1, curve2, inters_list )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve1, curve2 ;
DML_LIST inters_list ;
{
    PARM_S parm1, parm2 ;
    INT i, n = 0, m ;
    INT type[4] ;
    BOOLEAN near_tan[4] ;
    REAL t1[4], t2[4], t ;
    PT2 int_pt[4], pt ;
    DML_LIST local_list ;
#ifdef  SPLINE
    C2_CURVE spline ;
#endif  /* SPLINE */

    STACK_AVAIL ;
    if ( curve1 == curve2 ) 
        RETURN ( c2c_self_intersect ( curve1, inters_list ) ) ;

    local_list = dml_create_list() ;

    if ( C2_CURVE_IS_LINE(curve1) ) {

        if ( C2_CURVE_IS_LINE(curve2) ) 
            n = c2l_inters_ext ( C2_CURVE_LINE(curve1), 
                    C2_CURVE_LINE(curve2), t1, t2, int_pt, type, near_tan ) ;
        else if ( C2_CURVE_IS_ARC(curve2) ) 
            n = c2g_int_line_arc_ext ( C2_CURVE_LINE(curve1), 
                C2_CURVE_ARC(curve2), t1, t2, int_pt, type, near_tan ) ;
        else if ( C2_CURVE_IS_PCURVE(curve2) ) {
            n = c2p_inters_line ( C2_CURVE_PCURVE(curve2), 
                C2_CURVE_LINE(curve1), local_list ) ;
            c2c_inters_rec_swap ( local_list, n ) ;
            if ( c2c_closed ( curve2 ) ) 
                 n -= c2c_remove_end_inters ( C2_CURVE_PARM1(curve2), 
                     2, local_list, n ) ;
        }
#ifdef  SPLINE

        else {
            n = c2n_inters_line ( C2_CURVE_NURB(curve2), 
                C2_CURVE_PARM0(curve2), C2_CURVE_PARM1(curve2), 
                C2_CURVE_LINE(curve1), local_list ) ;
            c2c_inters_rec_swap ( local_list, n ) ;
            if ( c2c_closed ( curve2 ) ) 
                 n -= c2c_remove_end_inters ( C2_CURVE_PARM1(curve2), 
                     2, local_list, n ) ;
            c2c_compare_knots ( curve1, curve2, local_list, n ) ;
        }
#endif  /* SPLINE */
    }

    else if ( C2_CURVE_IS_ARC(curve1) ) {
        if ( C2_CURVE_IS_LINE(curve2) ) {
            n = c2g_int_line_arc_ext ( C2_CURVE_LINE(curve2), 
                C2_CURVE_ARC(curve1), t2, t1, int_pt, type, near_tan ) ;
        }

        else if ( C2_CURVE_IS_ARC(curve2) ) 
            n = c2g_int_arc_arc_ext ( C2_CURVE_ARC(curve1), 
                C2_CURVE_ARC(curve2), t1, t2, int_pt, type, near_tan ) ;

        else if ( C2_CURVE_IS_PCURVE(curve2) ) {
            n = c2p_inters_arc ( C2_CURVE_PCURVE(curve2), 
                C2_CURVE_ARC(curve1), TRUE, local_list ) ;
            c2c_inters_rec_swap ( local_list, n ) ;
            if ( c2c_closed ( curve2 ) ) 
                 n -= c2c_remove_end_inters ( C2_CURVE_PARM1(curve2), 
                     2, local_list, n ) ;
        }

#ifdef  SPLINE

        else {
            n = c2n_inters_arc ( C2_CURVE_NURB(curve2), 
                C2_CURVE_PARM0(curve2), C2_CURVE_PARM1(curve2), 
                C2_CURVE_ARC(curve1), local_list ) ;
            c2c_inters_rec_swap ( local_list, n ) ;
            PARM_T(&parm1) = 4.0 ;
            alr_parm_set ( &parm1 ) ;
            n -= c2c_remove_end_inters ( &parm1, 1, local_list, n ) ;
            if ( c2c_closed ( curve2 ) )
                n -= c2c_remove_end_inters ( C2_CURVE_PARM1(curve2), 
                    2, local_list, n ) ;
            c2c_compare_knots ( curve1, curve2, local_list, n ) ;
        }
#endif  /* SPLINE */
    }

    else if ( C2_CURVE_IS_PCURVE(curve1) ) {

        if ( C2_CURVE_IS_LINE(curve2) ) {
            n = c2p_inters_line ( C2_CURVE_PCURVE(curve1), 
                C2_CURVE_LINE(curve2), local_list ) ;
        }

        else if ( C2_CURVE_IS_ARC(curve2) ) 
            n = c2p_inters_arc ( C2_CURVE_PCURVE(curve1), 
                C2_CURVE_ARC(curve2), TRUE, local_list ) ;

        else if ( C2_CURVE_IS_PCURVE(curve2) ) {
            n = c2p_intersect ( C2_CURVE_PCURVE(curve1), 
                C2_CURVE_PCURVE(curve2), local_list ) ;
            if ( c2c_closed ( curve1 ) ) 
                 n -= c2c_remove_end_inters ( C2_CURVE_PARM1(curve1), 
                     1, local_list, n ) ;
            if ( c2c_closed ( curve2 ) ) 
                 n -= c2c_remove_end_inters ( C2_CURVE_PARM1(curve2), 
                     2, local_list, n ) ;
        }

#ifdef  SPLINE

        else {
            spline = c2d_curve_to_spline ( curve1 ) ;
            n = c2n_inters_splines ( C2_CURVE_NURB(spline), 
                C2_CURVE_PARM0(spline), C2_CURVE_PARM1(spline), 
                C2_CURVE_NURB(curve2), C2_CURVE_PARM0(curve2), 
                C2_CURVE_PARM1(curve2), local_list ) ;
            if ( c2c_closed ( spline ) )
                n -= c2c_remove_end_inters ( C2_CURVE_PARM1(spline), 
                    1, local_list, n ) ;
            if ( c2c_closed ( curve2 ) )
                n -= c2c_remove_end_inters ( C2_CURVE_PARM1(curve2), 
                    2, local_list, n ) ;
            c2c_compare_knots ( spline, curve2, local_list, n ) ;
            c2d_free_curve ( spline ) ;
        }
#endif  /* SPLINE */
    }

#ifdef  SPLINE

    else {
        if ( C2_CURVE_IS_LINE(curve2) ) {
            n = c2n_inters_line ( C2_CURVE_NURB(curve1), 
                C2_CURVE_PARM0(curve1), C2_CURVE_PARM1(curve1), 
                C2_CURVE_LINE(curve2), local_list ) ;
            if ( c2c_closed ( curve1 ) ) 
                n -= c2c_remove_end_inters ( C2_CURVE_PARM1(curve1), 
                    1, local_list, n ) ;
            c2c_compare_knots ( curve1, curve2, local_list, n ) ;
        }
        else if ( C2_CURVE_IS_ARC(curve2) ) {
            n = c2n_inters_arc ( C2_CURVE_NURB(curve1), 
                C2_CURVE_PARM0(curve1), C2_CURVE_PARM1(curve1), 
                C2_CURVE_ARC(curve2), local_list ) ;
            if ( c2c_closed ( curve1 ) )
                n -= c2c_remove_end_inters ( C2_CURVE_PARM1(curve1), 
                    1, local_list, n ) ;
            PARM_T(&parm1) = 4.0 ;
            alr_parm_set ( &parm1 ) ;
            n -= c2c_remove_end_inters ( &parm1, 2, local_list, n ) ;
            c2c_compare_knots ( curve1, curve2, local_list, n ) ;
        }

        else if ( C2_CURVE_IS_PCURVE(curve2) ) {
            spline = c2d_curve_to_spline ( curve2 ) ;
            n = c2n_inters_splines ( C2_CURVE_NURB(curve1), 
                C2_CURVE_PARM0(curve1), C2_CURVE_PARM1(curve1), 
                C2_CURVE_NURB(spline), C2_CURVE_PARM0(spline), 
                C2_CURVE_PARM1(spline), local_list ) ;
            if ( c2c_closed ( curve1 ) )
                n -= c2c_remove_end_inters ( C2_CURVE_PARM1(curve1), 
                    1, local_list, n ) ;
            if ( c2c_closed ( spline ) )
                n -= c2c_remove_end_inters ( C2_CURVE_PARM1(spline), 
                    2, local_list, n ) ;
            c2c_compare_knots ( curve1, spline, local_list, n ) ;
            c2d_free_curve ( spline ) ;
        }

        else {
            n = c2n_inters_splines ( C2_CURVE_NURB(curve1), 
                C2_CURVE_PARM0(curve1), C2_CURVE_PARM1(curve1), 
                C2_CURVE_NURB(curve2), C2_CURVE_PARM0(curve2), 
                C2_CURVE_PARM1(curve2), local_list ) ;
            if ( c2c_closed ( curve1 ) )
                n -= c2c_remove_end_inters ( C2_CURVE_PARM1(curve1), 
                    1, local_list, n ) ;
            if ( c2c_closed ( curve2 ) )
                n -= c2c_remove_end_inters ( C2_CURVE_PARM1(curve2), 
                    2, local_list, n ) ;
            c2c_compare_knots ( curve1, curve2, local_list, n ) ;
        }
    }
#endif  /* SPLINE */
    if ( ( C2_CURVE_IS_LINE(curve1) || C2_CURVE_IS_ARC(curve1) ) &&
         ( C2_CURVE_IS_LINE(curve2) || C2_CURVE_IS_ARC(curve2) ) ) {
        if ( n < 0 ) {
            dml_free_list ( local_list ) ;
            RETURN ( n ) ;
        }

        m = 0 ;
        if ( C2_CURVE_IS_ARC(curve1) ) {
            if ( ( C2_CURVE_T0(curve1) > BBS_ZERO ) &&
                ( C2_CURVE_T1(curve1) >= 2.0 - BBS_ZERO ) ) {
                if ( n >= 0 && IS_SMALL(t1[0]) )
                    t1[0] = 2.0 ;
                if ( n >= 1 && IS_SMALL(t1[1]) )
                    t1[1] = 2.0 ;
            }
            if ( ( n == 2 ) && ( t1[0] > t1[1] ) ) {
                t = t1[0] ;
                t1[0] = t1[1] ;
                t1[1] = t ;
                t = t2[0] ;
                t2[0] = t2[1] ;
                t2[1] = t ;
                C2V_COPY ( int_pt[0], pt ) ;
                C2V_COPY ( int_pt[1], int_pt[0] ) ;
                C2V_COPY ( pt, int_pt[1] ) ;
                m = type[0] ;
                type[1] = type[0] ;
                type[1] = m ;
                m = near_tan[0] ;
                near_tan[1] = near_tan[0] ;
                near_tan[1] = m ;
/* jak 4/21/94 */
                m = 0;
            }
        }
        if ( C2_CURVE_IS_ARC(curve2) ) {
            if ( ( C2_CURVE_T0(curve2) > BBS_ZERO ) &&
                ( C2_CURVE_T1(curve2) >= 2.0 - BBS_ZERO ) ) {
                if ( n >= 0 && IS_SMALL(t2[0]) )
                    t2[0] = 2.0 ;
                if ( n >= 1 && IS_SMALL(t2[1]) )
                    t2[1] = 2.0 ;
            }
        }

        for ( i=0 ; i<n ; i++ ) {
            PARM_T(&parm1) = t1[i] ;
            if ( C2_CURVE_IS_LINE(curve1) ) 
                PARM_J(&parm1) = 1 ;
            else if ( C2_CURVE_IS_ARC(curve1) ) 
                alr_parm_set ( &parm1 ) ;

            PARM_T(&parm2) = t2[i] ;
            if ( C2_CURVE_IS_LINE(curve2) ) 
                PARM_J(&parm2) = 1 ;
            else if ( C2_CURVE_IS_ARC(curve2) ) 
                alr_parm_set ( &parm2 ) ;
            if ( c2c_append_int_rec ( local_list, &parm1, &parm2, int_pt[i], 
                0.0, type[i], near_tan[i] ) )
                m++ ;
        }
        n = m ;
    }
#ifdef DDEBUG
{
    DML_ITEM item ;
    C2_INT_REC ci ;
    PT2 p1, p2 ;
    REAL delta1, delta2 ;

    DML_WALK_LIST ( local_list, item ) {
        ci = DML_RECORD(item) ;
        c2c_eval_pt ( curve1, C2_INT_REC_PARM1(ci), p1 ) ;
        c2c_eval_pt ( curve2, C2_INT_REC_PARM2(ci), p2 ) ;
        delta1 = C2V_DISTL1 ( C2_INT_REC_PT(ci), p1 ) ;
        delta2 = C2V_DISTL1 ( C2_INT_REC_PT(ci), p2 ) ;
    }
}
#endif
    dml_append_list ( inters_list, local_list ) ;
    dml_free_list ( local_list ) ;
    RETURN ( n ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2c_append_int_rec ( inters_list, parm1, parm2, 
            int_pt, dist, type, near_tan ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST inters_list ;
PARM parm1, parm2 ;
PT2 int_pt ;
REAL dist ;
INT type ;
BOOLEAN near_tan ;
{
    C2_INT_REC int_rec, old_rec ;
    REAL tol ;
    INT m ;
    DML_ITEM item ;
    for ( item=DML_LAST(inters_list) ; item!=NULL ; item=DML_PREV(item) ) {

        old_rec = (C2_INT_REC)(DML_RECORD(item)) ;
        m = MAX ( C2_INT_REC_TYPE(old_rec), type ) ;
        tol = BBS_ZERO ;
        if ( m == 2 ) 
            tol = sqrt ( tol ) ;
        else if ( m > 2 ) 
            tol = pow ( tol, 1.0 / ( (REAL)m ) ) ;

        if ( fabs ( C2_INT_REC_T1(old_rec) - PARM_T(parm1) ) <= tol &&
            fabs ( C2_INT_REC_T2(old_rec) - PARM_T(parm2) ) <= tol ) {

            if ( ( type < 0 && C2_INT_REC_TYPE(old_rec) > 0 ) ||
                ( dist < C2_INT_REC_DIST(old_rec) ) ) {
                C2_INT_REC_T1(old_rec) = PARM_T(parm1) ;
                C2_INT_REC_T2(old_rec) = PARM_T(parm2) ;
                C2_INT_REC_J1(old_rec) = PARM_J(parm1) ;
                C2_INT_REC_J2(old_rec) = PARM_J(parm2) ;
                C2V_COPY ( int_pt, C2_INT_REC_PT(old_rec) ) ;
                C2_INT_REC_DIST(old_rec) = dist ;
                C2_INT_REC_TYPE(old_rec) = type ;
            }
            RETURN ( FALSE ) ;
        }
    }
    
    int_rec = C2_ALLOC_INT_REC ;
    if ( int_rec == NULL ) 
        RETURN ( FALSE ) ;
    C2_INT_REC_T1(int_rec) = PARM_T(parm1) ;
    C2_INT_REC_J1(int_rec) = PARM_J(parm1) ;
    C2_INT_REC_T2(int_rec) = PARM_T(parm2) ;
    C2_INT_REC_J2(int_rec) = PARM_J(parm2) ;
    C2V_COPY ( int_pt, C2_INT_REC_PT(int_rec) ) ;
    C2_INT_REC_DIST(int_rec) = dist ;
    C2_INT_REC_TYPE(int_rec) = type ;
    C2_INT_REC_NEAR_TAN(int_rec) = near_tan ;
    RETURN ( dml_append_data ( inters_list, int_rec ) != NULL ) ;
}    


/*----------------------------------------------------------------------*/
STATIC void c2c_inters_rec_swap ( inters_list, m ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST inters_list ;
INT m ;
{
    INT i, j ;
    REAL t ;
    DML_ITEM item ;
    C2_INT_REC int_rec ;

    for ( i=0, item = DML_LAST(inters_list) ; i<m ; 
        i++, item=DML_PREV(item) ) {
        int_rec = (C2_INT_REC)DML_RECORD(item) ;
        t = C2_INT_REC_T1(int_rec) ;
        j = C2_INT_REC_J1(int_rec) ;
        C2_INT_REC_T1(int_rec) = C2_INT_REC_T2(int_rec) ;
        C2_INT_REC_J1(int_rec) = C2_INT_REC_J2(int_rec) ;
        C2_INT_REC_T2(int_rec) = t ;
        C2_INT_REC_J2(int_rec) = j ;
    }
}    


/*------------------------------------------------------------------------*/
STATIC INT c2c_remove_end_inters ( end_parm, index, inters_list, inters_no ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PARM end_parm ;
INT index ;
DML_LIST inters_list ;
INT inters_no ;
{
    INT m, i ;
    DML_ITEM item, prev_item ;
    C2_INT_REC int_rec ;

    m = 0 ;
    item = DML_LAST(inters_list) ;

    if ( index==1 ) {
        for ( i=0 ; i<inters_no ; i++ ) {
            int_rec = (C2_INT_REC)DML_RECORD(item) ;
            prev_item = DML_PREV(item) ;
            if ( ( C2_INT_REC_J1(int_rec) == PARM_J(end_parm) ) &&
                 ( C2_INT_REC_TYPE(int_rec) > 0 ) &&
                 IS_ZERO ( C2_INT_REC_T1(int_rec) - PARM_T(end_parm) ) ) {
                C2_FREE_INT_REC(int_rec) ;
                DML_RECORD(item) = NULL ;
                dml_remove_item ( inters_list, item ) ;
                m++ ;
            }
            item = prev_item ;
        }
    }

    else if ( index==2 ) {
        for ( i=0 ; i<inters_no ; i++ ) {
            int_rec = (C2_INT_REC)DML_RECORD(item) ;
            prev_item = DML_PREV(item) ;
            if ( ( C2_INT_REC_J2(int_rec) == PARM_J(end_parm) ) &&
                 ( C2_INT_REC_TYPE(int_rec) > 0 ) &&
                 IS_ZERO ( C2_INT_REC_T2(int_rec) - PARM_T(end_parm) ) ) {
                C2_FREE_INT_REC(int_rec) ;
                DML_RECORD(item) = NULL ;
                dml_remove_item ( inters_list, item ) ;
                m++ ;
            }
            item = prev_item ;
        }
    }
    RETURN ( m ) ;
}


#ifdef SPLINE
/*------------------------------------------------------------------------*/
STATIC void c2c_compare_knots ( curve1, curve2, inters_list, inters_no ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve1, curve2 ;
DML_LIST inters_list ;
INT inters_no ;
{
    INT i ;
    DML_ITEM item ;
    REAL tol ;
    C2_INT_REC int_rec ;

    for ( i=0, item = DML_LAST(inters_list) ; i<inters_no ; 
        i++, item=DML_PREV(item) ) {

        int_rec = (C2_INT_REC)DML_RECORD(item) ;
        tol = BBS_ZERO ;
        if ( C2_INT_REC_TYPE(int_rec) == 2 ) 
            tol = sqrt ( BBS_ZERO ) ;
        else if ( C2_INT_REC_TYPE(int_rec) > 2 ) 
            tol = pow ( tol, 1.0/( (REAL)C2_INT_REC_TYPE(int_rec) ) );

        c2c_compare_knots_curve ( curve1, tol, &C2_INT_REC_T1(int_rec), 
            C2_INT_REC_J1(int_rec) ) ;
        c2c_compare_knots_curve ( curve2, tol, &C2_INT_REC_T2(int_rec), 
            C2_INT_REC_J2(int_rec) ) ;
    }
}    


/*------------------------------------------------------------------------*/
STATIC void c2c_compare_knots_curve ( curve, tol, t_ptr, j ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL tol ;
REAL *t_ptr ;
INT j ;
{
    INT i ;
    if ( C2_CURVE_IS_LINE(curve) ) {
        if ( fabs(*t_ptr) <= tol ) 
            *t_ptr = 0.0 ;
        else if ( fabs(*t_ptr-1.0) <= tol ) 
            *t_ptr = 1.0 ;
    }

    else if ( C2_CURVE_IS_ARC(curve) ) {
        if ( fabs(*t_ptr) <= tol ) 
            *t_ptr = 0.0 ;
        else if ( fabs(*t_ptr-0.5) <= tol ) 
            *t_ptr = 0.5 ;
        else if ( fabs(*t_ptr-1.0) <= tol ) 
            *t_ptr = 1.0 ;
        else if ( fabs(*t_ptr-1.5) <= tol ) 
            *t_ptr = 1.5 ;
    }

    else if ( C2_CURVE_IS_PCURVE(curve) ) {
        i = (INT)(*t_ptr+tol) ;
        if ( fabs(*t_ptr-(REAL)i) <= tol ) 
            *t_ptr = (REAL)i ;
    }

    else 
        c2n_compare_knots ( C2_CURVE_NURB(curve), tol, t_ptr, j ) ;
}    
#endif /*SPLINE*/

