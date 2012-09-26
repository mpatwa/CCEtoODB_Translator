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
BBS_PRIVATE void c2g_info ( arc, t0, t1, outfile ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
FILE *outfile ;
REAL t0, t1 ;
{
    PT2 ctr, pt ;
    REAL rad, sa, sw, ea ;
    INT dir ;

    if ( IS_ZERO ( c2r_get_d ( C2_ARC_SEG(arc) ) ) ) {
        bbs_print_string ( outfile, "The arc is degenerate" ) ;
    }
    else {
        c2g_ctr ( arc, ctr );
        rad = c2g_rad ( arc );
        sa = c2g_start_angle ( arc );
        sw = c2g_sweep ( arc, t0, t1 );
        dir = c2g_dir ( arc );
//        c2r_get_data ( C2_ARC_SEG(arc), ctr, NULL, &rad, &sa, &sw, &dir );
//        c2r_ctr ( C2_ARC_SEG(arc), ctr ) ;
//        rad = C2_ARC_ZERO_RAD(arc) ? 0.0 : c2r_rad ( C2_ARC_SEG(arc) ) ;
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
        bbs_print_real ( outfile, sa / PI_OVER_180 ) ;
        bbs_print_new_line ( outfile ) ;
        bbs_print_string ( outfile, "Sweep" ) ;
        bbs_print_tabs ( outfile, 3 ) ;
        bbs_print_real ( outfile, sw ) ;
        bbs_print_real ( outfile, sw / PI_OVER_180 ) ;
        bbs_print_new_line ( outfile ) ;
        ea = sa + sw;
        bbs_print_string ( outfile, "End Angle" ) ;
        bbs_print_tabs ( outfile, 3 ) ;
        bbs_print_real ( outfile, ea ) ;
        bbs_print_real ( outfile, ea / PI_OVER_180 ) ;
        bbs_print_new_line ( outfile ) ;
        bbs_print_string ( outfile, "Direction" ) ;
        bbs_print_tabs ( outfile, 3 ) ;
        bbs_print_int ( outfile, dir ) ;
    }

    bbs_print_new_line ( outfile ) ;
    bbs_print_string ( outfile, "Start point" ) ;
    bbs_print_tabs ( outfile, 2 ) ;
    c2g_pt_tan ( arc, t0, pt, NULL ) ;
    c2a_print_pt ( outfile, pt ) ;
    bbs_print_new_line ( outfile ) ;
    bbs_print_string ( outfile, "End point" ) ;
    bbs_print_tabs ( outfile, 2 ) ;
    c2g_pt_tan ( arc, t1, pt, NULL ) ;
    c2a_print_pt ( outfile, pt ) ;
}

