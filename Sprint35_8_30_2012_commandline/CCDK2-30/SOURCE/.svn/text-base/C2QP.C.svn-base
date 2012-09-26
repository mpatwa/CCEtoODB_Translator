/* -2 -3*/
/********************************** C2QP.C *********************************/
/****************** Routines for processing arc geometry *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2qdefs.h>
#include <c2apriv.h>
#include <bbspriv.h>
#include <c2qmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2q_info ( buffer, outfile ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
FILE *outfile ;
{
    INT i ;

    bbs_print_new_line ( outfile ) ;
    for ( i=0 ; i<C2_PC_BUFFER_N(buffer)-1 ; i++ ) {
        c2a_print_pt ( outfile, C2_PC_BUFFER_PT(buffer,i) ) ;
        bbs_print_tabs ( outfile, 2 ) ;
        bbs_print_real ( outfile, C2_PC_BUFFER_D(buffer,i) ) ;
        bbs_print_new_line ( outfile ) ;
    }
    c2a_print_pt ( outfile, C2_PC_BUFFER_PT1(buffer) ) ;
}

