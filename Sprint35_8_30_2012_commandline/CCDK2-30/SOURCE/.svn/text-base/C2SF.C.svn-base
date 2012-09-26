/* -2 -3 */
/******************************* C2SF.C *********************************/
/**************** Printing two-dimensional splines **********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <bbsdefs.h>
#ifdef SPLINE
#include <c2sdefs.h>
#include <c2apriv.h>
#include <bbspriv.h>

/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2s_info ( a, n, d, knot, w, outfile ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
INT n, d ;
REAL *knot ;
REAL w ;
FILE *outfile ;
{
    INT i ;
    bbs_print_string ( outfile, "The number of control points is\t\t" ) ;
    bbs_print_int ( outfile, n ) ;
    bbs_print_new_line ( outfile ) ;
    bbs_print_string ( outfile, "The order of the spline is\t\t" ) ;
    bbs_print_int ( outfile, d ) ;
    bbs_print_new_line ( outfile ) ;
    if ( w > BBS_ZERO ) {
        bbs_print_string ( outfile, "Offset of the spline is\t\t" ) ;
        bbs_print_tabs ( outfile, 3 ) ;
        bbs_print_real ( outfile, w ) ;
        bbs_print_new_line ( outfile ) ;
    }
    bbs_print_tab ( outfile ) ;
    bbs_print_string ( outfile, "Control points:\t\t" ) ;
    bbs_print_new_line ( outfile ) ;
    for ( i=0 ; i<n ; i++ ) {
        c2a_print_hpt ( outfile, a[i] ) ;
        bbs_print_new_line ( outfile ) ;
    }
    bbs_print_tab ( outfile ) ;
    bbs_print_string ( outfile, "Knots:\t\t" ) ;
    bbs_print_new_line ( outfile ) ;
    for ( i=0 ; i<n+d ; i++ ) {
        bbs_print_real ( outfile, knot[i] ) ;
        bbs_print_new_line ( outfile ) ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2s_info_ellipse ( a, w, outfile ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
REAL w ;
FILE *outfile ;
{
    REAL major_axis, minor_axis, angle ;
    PT2 ctr ;

    if ( !c2s_get_ellipse_data ( a, ctr, &major_axis, &minor_axis, &angle ) ) {
        bbs_print_string ( outfile, "Computation failed\n" ) ;
        RETURN ;
    }
    bbs_print_string ( outfile, "Center" ) ;
    bbs_print_tabs ( outfile, 3 ) ;
    c2a_print_pt ( outfile, ctr ) ;
    bbs_print_new_line ( outfile ) ;
    bbs_print_string ( outfile, "Major axis" ) ;
    bbs_print_tabs ( outfile, 4 ) ;
    bbs_print_real ( outfile, major_axis ) ;
    bbs_print_new_line ( outfile ) ;
    bbs_print_string ( outfile, "Minor axis" ) ;
    bbs_print_tabs ( outfile, 4 ) ;
    bbs_print_real ( outfile, minor_axis ) ;
    bbs_print_new_line ( outfile ) ;
    bbs_print_string ( outfile, "Angle" ) ;
    bbs_print_tabs ( outfile, 5 ) ;
    bbs_print_real ( outfile, angle ) ;
    if ( w > BBS_ZERO ) {
        bbs_print_string ( outfile, "Offset of the spline is\t\t" ) ;
        bbs_print_real ( outfile, w ) ;
        bbs_print_tabs ( outfile, 3 ) ;
        bbs_print_new_line ( outfile ) ;
    }
}
#endif /*SPLINE*/

