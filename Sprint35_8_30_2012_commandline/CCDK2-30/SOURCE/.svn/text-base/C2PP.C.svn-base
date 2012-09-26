/* -2 -3*/
/********************************** C2PP.C *********************************/
/****************** Routines for processing arc geometry *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2pdefs.h>
#include <c2qdefs.h>
#include <dmldefs.h>
#include <c2pmcrs.h>
#include <bbspriv.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2p_info ( pcurve, outfile ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
FILE *outfile ;
{
    DML_ITEM item ;

    bbs_print_tab ( outfile ) ;
    bbs_print_string ( outfile, "Points:\t\t\t\tBulge:" ) ;
    bbs_print_new_line ( outfile ) ;
    c2q_info ( C2_PCURVE_BUFFER(pcurve), outfile ) ;
    C2_WALK_PCURVE ( pcurve, item ) 
        c2q_info ( DML_RECORD(item), outfile ) ;
}

