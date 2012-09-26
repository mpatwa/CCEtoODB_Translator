/* -2 -3 */
/*********************************** C2BB.C ********************************/
/*********************** Two-dimensional Bezier curves *********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef  SPLINE
#include <c2bdefs.h>
#include <c2coned.h>

STATIC  BOOLEAN c2bb_pt_in_box_coord __(( HPT2* DUMMY0 , INT DUMMY1 , 
            REAL DUMMY2 , REAL DUMMY3 , INT DUMMY4 )) ;
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2b_pt_in_box ( b, d, w, pt, tol ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *b  ;   /* Control points of the segment */
INT     d ;     /* Order of the segment */
REAL    w ;     /* Offset of the segment */
PT2     pt ;    /* Point */
REAL    tol ;   /* Offset of the segment */
{
    tol += w ;
    RETURN ( c2bb_pt_in_box_coord ( b, d, pt[0], tol, 0 ) && 
             c2bb_pt_in_box_coord ( b, d, pt[1], tol, 1 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2b_overlap_box ( b, d, w, box_x, box_y, box_w, box_h, 
            tol ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *b  ;   /* Control points of the segment */
INT     d ;     /* Order of the segment */
REAL    w ;     /* Offset of the segment */
REAL    box_x, box_y, box_w, box_h ;
REAL    tol ;   /* Offset of the segment */
{
    tol += w ;
    RETURN ( c2bb_pt_in_box_coord ( b, d, box_x, tol+box_w, 0 ) && 
             c2bb_pt_in_box_coord ( b, d, box_y, tol+box_h, 1 ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c2bb_pt_in_box_coord ( b, d, x, tol, j ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *b  ;   /* Control points of the segment */
INT     d ;     /* Order of the segment */
REAL    x ;     /* Point */
REAL    tol ;   /* Offset of the segment */
INT     j ;
{
    INT i ;
    REAL a_min, a_max, b_min, b_max, b_value ;

    a_min = x - tol ;
    a_max = x + tol ;
    if ( b[0][2] <= BBS_ZERO ) 
        RETURN ( TRUE ) ;
    b_min = b[0][j] / b[0][2] ;
    b_max = b_min ;
    if ( b_min <= a_max && a_min <= b_max ) 
        RETURN ( TRUE ) ;
    for ( i=1 ; i<d ; i++ ) {
        if ( b[i][2] <= BBS_ZERO ) 
            RETURN ( TRUE ) ;
        b_value = b[i][j] / b[i][2] ;
        if ( b_value < b_min ) 
            b_min = b_value ;
        else if ( b_value > b_max ) 
            b_max = b_value ;
        if ( b_min <= a_max && a_min <= b_max ) 
            RETURN ( TRUE ) ;
    }
    RETURN ( FALSE ) ;
}
#endif /*SPLINE*/

