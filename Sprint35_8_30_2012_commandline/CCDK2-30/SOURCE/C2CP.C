/* -2 -3 */
/******************************* C2CP.C *********************************/
/**************** Printing two-dimensional curves ***********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2gdefs.h>
#include <c2ldefs.h>
#include <c2pdefs.h>
#include <c2ndefs.h>
#include <c2apriv.h>
#include <c2cdefs.h>
#include <bbspriv.h>

BBS_PRIVATE void c2c_info_arc __(( C2_CURVE arc, FILE *outfile )) ;

/*----------------------------------------------------------------------*/
BBS_PUBLIC void c2c_info_curve ( curve, outfile ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
FILE *outfile ;
{
    bbs_print_new_line ( outfile ) ;
    if ( C2_CURVE_IS_LINE(curve) ) 
        bbs_print_string ( outfile, "The curve is a 2d line\n" ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) {
        if ( C2_CURVE_T0(curve) <= BBS_ZERO && 
            C2_CURVE_T1(curve) >= 2.0 - BBS_ZERO )
            bbs_print_string ( outfile, "The curve is a 2d circle\n" ) ;
        else
            bbs_print_string ( outfile, "The curve is a 2d arc\n" ) ;
    }
    else if ( C2_CURVE_IS_PCURVE(curve) ) 
        bbs_print_string ( outfile, "The curve is a 2d pcurve\n" ) ;
#ifdef  SPLINE
    else if ( C2_CURVE_IS_SPLINE(curve) ) 
        bbs_print_string ( outfile, "The curve is a 2d spline\n" ) ;
    else if ( C2_CURVE_IS_BEZIER(curve) ) 
        bbs_print_string ( outfile, "The curve is a 2d Bezier spline\n" ) ;
    else if ( C2_CURVE_IS_ELLIPSE(curve) ) 
        bbs_print_string ( outfile, "The curve is a 2d ellipse\n" ) ;
    else 
        bbs_print_string ( outfile, "The curve's type is unknown\n" ) ;
#endif  /*SPLINE*/

    bbs_print_string ( outfile, "The start parameter of the curve is" ) ;
    bbs_print_tab ( outfile ) ;
    bbs_print_real ( outfile, C2_CURVE_T0(curve) ) ;
    bbs_print_new_line ( outfile ) ;
    bbs_print_string ( outfile, "The  end  parameter of the curve is" ) ;
    bbs_print_tab ( outfile ) ;
    bbs_print_real ( outfile, C2_CURVE_T1(curve) ) ;
    bbs_print_new_line ( outfile ) ;
    if ( C2_CURVE_IS_LINE(curve) ) 
        c2l_info ( C2_CURVE_LINE(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), outfile ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        c2c_info_arc ( curve, outfile ) ;
    else if ( C2_CURVE_IS_PCURVE(curve) ) 
        c2p_info ( C2_CURVE_PCURVE(curve), outfile ) ;
#ifdef  SPLINE
    else if ( C2_CURVE_IS_ELLIPSE(curve) ) 
        c2n_info_ellipse ( C2_CURVE_NURB(curve), outfile ) ;
    else if ( C2_CURVE_IS_SPLINE(curve) || C2_CURVE_IS_BEZIER(curve) ) 
        c2n_info ( C2_CURVE_NURB(curve), outfile ) ;
#endif  /*SPLINE*/
    bbs_print_new_line ( outfile ) ;
}    

/*----------------------------------------------------------------------*/
BBS_PUBLIC void c2c_info_int_rec ( int_rec, outfile ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C2_INT_REC int_rec ;
FILE *outfile ;
{
    bbs_print_new_line ( outfile ) ;
    if ( C2_INT_REC_TYPE(int_rec) < 0 ) 
        bbs_print_string ( outfile, "Coincident segments\n" ) ;
    bbs_print_string ( outfile, "t1 = " ) ;
    bbs_print_real ( outfile, C2_INT_REC_T1(int_rec) ) ;
    bbs_print_tab ( outfile ) ;
    bbs_print_string ( outfile, "j1 = " ) ;
    bbs_print_int ( outfile, C2_INT_REC_J1(int_rec) ) ;
    bbs_print_tab ( outfile ) ;
    bbs_print_string ( outfile, "t2 = " ) ;
    bbs_print_real ( outfile, C2_INT_REC_T2(int_rec) ) ;
    bbs_print_tab ( outfile ) ;
    bbs_print_string ( outfile, "j2 = " ) ;
    bbs_print_int ( outfile, C2_INT_REC_J2(int_rec) ) ;
    bbs_print_new_line ( outfile ) ;
    bbs_print_string ( outfile, "pt\t" ) ;
    c2a_print_pt ( outfile, C2_INT_REC_PT(int_rec) ) ;
    bbs_print_string ( outfile, "\t, dist = \t" ) ;
    bbs_print_real ( outfile, C2_INT_REC_DIST(int_rec) ) ;

    bbs_print_new_line ( outfile ) ;
    if ( C2_INT_REC_TYPE(int_rec) > 0 ) {
        if ( C2_INT_REC_TRANS(int_rec) )
            bbs_print_string ( outfile, "Transversal intersection\n" ) ;
        if ( C2_INT_REC_TANGENT(int_rec) )
            bbs_print_string ( outfile, "Tangential intersection\n" ) ;
    }
}    

/* -2 -3 */
/********************************** C2GP.C *********************************/
/****************** Routines for processing arc geometry *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2gdefs.h>
#include <c2rdefs.h>
#include <c2apriv.h>
#include <c2gmcrs.h>
#include <bbspriv.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2c_info_arc ( arc, outfile ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE arc ;
FILE *outfile ;
{
    PT2 ctr, pt ;
    REAL rad, sa, sw, ea ;
    INT dir ;

    c2c_get_arc_data ( arc, ctr, &rad, &sa, &sw, &dir );

    bbs_print_string ( outfile, "Center" ) ;
    bbs_print_tabs ( outfile, 3 ) ;
    c2a_print_pt ( outfile, ctr ) ;
    bbs_print_new_line ( outfile ) ;

    bbs_print_string ( outfile, "Radius" ) ;
    bbs_print_tabs ( outfile, 3 ) ;
    bbs_print_real ( outfile, rad ) ;
    bbs_print_new_line ( outfile ) ;

    bbs_print_string ( outfile, "Start angle" ) ;
    bbs_print_tabs ( outfile, 3 ) ;
    bbs_print_real ( outfile, sa ) ;
    bbs_print_tabs ( outfile, 1 );
    bbs_print_real ( outfile, sa / PI_OVER_180 ) ;
    bbs_print_new_line ( outfile ) ;

    bbs_print_string ( outfile, "Sweep" ) ;
    bbs_print_tabs ( outfile, 3 ) ;
    bbs_print_real ( outfile, sw ) ;
    bbs_print_tabs ( outfile, 1 );
    bbs_print_real ( outfile, sw / PI_OVER_180 ) ;
    bbs_print_new_line ( outfile ) ;

    if ( dir == 1 )
        ea = sa + sw;
    else
        ea = sa - sw;

    bbs_print_string ( outfile, "End Angle" ) ;
    bbs_print_tabs ( outfile, 3 ) ;
    bbs_print_real ( outfile, ea ) ;
    bbs_print_tabs ( outfile, 1 );
    bbs_print_real ( outfile, ea / PI_OVER_180 ) ;
    bbs_print_new_line ( outfile ) ;

    bbs_print_string ( outfile, "Direction" ) ;
    bbs_print_tabs ( outfile, 3 ) ;
    bbs_print_int ( outfile, dir ) ;
    bbs_print_new_line ( outfile ) ;

    bbs_print_string ( outfile, "Start point" ) ;
    bbs_print_tabs ( outfile, 2 ) ;
    c2c_ept0 ( arc, pt );
    c2a_print_pt ( outfile, pt ) ;
    bbs_print_new_line ( outfile ) ;

    bbs_print_string ( outfile, "End point" ) ;
    bbs_print_tabs ( outfile, 2 ) ;
    c2c_ept1 ( arc, pt );
    c2a_print_pt ( outfile, pt ) ;
}


