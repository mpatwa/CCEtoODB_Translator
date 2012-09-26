/* -2 -3 */
/******************************* C2CIL.C ********************************/ 
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
#include <c2adefs.h>
#include <c2cdefs.h>
#include <c2ddefs.h>
#include <dmldefs.h>
#include <c2mem.h>
#include <c2vmcrs.h>

STATIC  INT inters_array_inside __(( C2_CURVE, C2_CURVE, REAL*, REAL*, 
                PT2*, INT*, INT )) ;
/*----------------------------------------------------------------------*/
BBS_PUBLIC INT c2c_intersect_line_or_arc ( curve1, curve2, t1, t2, int_pt, 
            type, near_tan )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve1, curve2 ;
INT *type ;
REAL *t1, *t2 ;
PT2 *int_pt ;
BOOLEAN *near_tan ;
{
    INT n ;

    if ( !c2a_box_overlap ( C2_CURVE_BOX(curve1), C2_CURVE_BOX(curve2) ) ) 
        RETURN ( 0 ) ;
    n = c2c_intersect_line_or_arc_ext ( curve1, curve2, 
        t1, t2, int_pt, type, near_tan ) ;
    RETURN ( n <= 0 ? n : 
        inters_array_inside ( curve1, curve2, t1, t2, int_pt, type, n ) ) ;
}


/*----------------------------------------------------------------------*/
STATIC INT inters_array_inside ( curve1, curve2, t1, t2, int_pt, type, m )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C2_CURVE curve1, curve2 ;
INT *type ;
REAL *t1, *t2 ;
PT2 *int_pt ;
INT m ;
{
    INT i, n ;

    n = 0 ;
    for ( i=0 ; i<m ; i++ ) {
        if ( C2_CURVE_T0(curve1) - BBS_ZERO <= t1[i] && 
             C2_CURVE_T1(curve1) + BBS_ZERO >= t1[i] && 
             C2_CURVE_T0(curve2) - BBS_ZERO <= t2[i] && 
             C2_CURVE_T1(curve2) + BBS_ZERO >= t2[i] ) {
            if ( i!=n ) {
                t1[n] = t1[i] ;
                t2[n] = t2[i] ;
                C2V_COPY ( int_pt[i], int_pt[n] ) ;
                type[n] = type[n] ;
            }
            n++ ;
        }
    }
    RETURN ( n ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC INT c2c_intersect_line_or_arc_ext ( curve1, curve2, t1, t2, 
            int_pt, type, near_tan )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve1, curve2 ;
INT *type ;
REAL *t1, *t2 ;
PT2 *int_pt ;
BOOLEAN *near_tan ;
{
    if ( curve1 == curve2 ) 
        RETURN ( -25 ) ;

    if ( C2_CURVE_IS_LINE(curve1) ) {
        if ( C2_CURVE_IS_LINE(curve2) ) 
            RETURN ( c2l_inters_ext ( C2_CURVE_LINE(curve1), 
                C2_CURVE_LINE(curve2), t1, t2, int_pt, type, near_tan ) ) ;
        else if ( C2_CURVE_IS_ARC(curve2) ) 
            RETURN ( c2g_int_line_arc_ext ( C2_CURVE_LINE(curve1), 
                C2_CURVE_ARC(curve2), t1, t2, int_pt, type, near_tan ) ) ;
        else 
            RETURN ( -4 ) ;
    }

    else if ( C2_CURVE_IS_ARC(curve1) ) {
        if ( C2_CURVE_IS_LINE(curve2) ) 
            RETURN ( c2g_int_line_arc_ext ( C2_CURVE_LINE(curve2), 
                C2_CURVE_ARC(curve1), t2, t1, int_pt, type, near_tan ) ) ;
        else if ( C2_CURVE_IS_ARC(curve2) ) 
            RETURN ( c2g_int_arc_arc_ext ( C2_CURVE_ARC(curve1), 
                C2_CURVE_ARC(curve2), t1, t2, int_pt, type, near_tan ) ) ;
        else 
            RETURN ( -4 ) ;
    }

    else 
        RETURN ( -4 ) ;
}

