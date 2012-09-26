/* -2 -3 */
/********************************** C2GB.C *********************************/
/****************** Routines for processing arc geometry *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2gdefs.h>
#include <c2rdefs.h>
#include <c2adefs.h>
#include <c2gmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2g_box ( arc, t0, t1, box ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL t0, t1 ;
C2_BOX box ;
{
    PT2 ctr, p ;
    REAL rad ;

    if ( C2_ARC_ZERO_RAD(arc) ) 
        c2a_box_init_pt ( box, c2r_ctr ( C2_ARC_SEG(arc), ctr) ) ;
    else if ( t1 > 1.0 + BBS_ZERO ) {
        rad = c2r_rad ( C2_ARC_SEG(arc) ) ;
        if ( rad < 0.0 ) {      /* line */
            c2g_pt_tan ( arc, t0, p, NULL ) ;
            c2a_box_init_pt ( box, p ) ;
            c2g_pt_tan ( arc, t1, p, NULL ) ;
            c2a_box_append_pt ( box, p ) ;
        }
        else 
            c2a_box_set_ctr_wh ( box, c2r_ctr ( C2_ARC_SEG(arc), ctr), 
                2.0*rad, 2.0*rad ) ;
    }
    else
        c2r_box ( C2_ARC_SEG(arc), t0, t1, box ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2g_zero_rad ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
{
    RETURN ( C2_ARC_ZERO_RAD(arc) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2g_curvature ( arc, curv_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL* curv_ptr ;
{
    if ( C2_ARC_ZERO_RAD(arc) ) 
        RETURN ( FALSE ) ;
    else {
        *curv_ptr = c2r_curvature ( C2_ARC_SEG(arc) ) ;
        RETURN ( TRUE ) ;
    }
}

