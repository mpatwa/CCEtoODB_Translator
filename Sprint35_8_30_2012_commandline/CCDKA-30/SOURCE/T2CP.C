/* -R -Z -S -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/******************************* T2CP.C *********************************/ 
/**************** Two-dimensional topology ******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2apriv.h> 
#include <bbspriv.h> 
#include <c2vdefs.h> 
#include <c2vmcrs.h> 
#include <dmldefs.h> 
#include <t2cdefs.h>
#include <t2cpriv.h>
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

/*----------------------------------------------------------------------*/
BBS_PUBLIC void t2c_info_region ( region, outfile )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
FILE *outfile ;
{
    DML_ITEM item ;
    if ( region == NULL || T2_REGION_LOOP_LIST(region) == NULL ) 
        RETURN ;
    bbs_print_new_line ( outfile ) ;
    bbs_print_string ( outfile, "REGION has " ) ;
    bbs_print_int ( outfile, DML_LENGTH ( T2_REGION_LOOP_LIST(region) ) ) ;
    bbs_print_string ( outfile, "loops" ) ;
    bbs_print_new_line ( outfile ) ;

    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region) , item ) 
        t2c_info_loop ( DML_RECORD(item), outfile ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC void t2c_info_loop ( loop, outfile )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
FILE *outfile ;
{
    DML_ITEM item ;
    if ( loop == NULL || T2_LOOP_EDGE_LIST(loop) == NULL ) 
        RETURN ;
    bbs_print_new_line ( outfile ) ;
    if ( T2_LOOP_CLOSED(loop) ) 
        bbs_print_string ( outfile, "LOOP is closed" ) ;
    else
        bbs_print_string ( outfile, "LOOP is open" ) ;
    bbs_print_new_line ( outfile ) ;
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop) , item ) 
        t2c_info_edge ( DML_RECORD(item), outfile ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2c_info_loop_vtcs ( loop, outfile )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
FILE *outfile ;
{
    DML_ITEM item, item1 ;
    INT i = 0 ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        item1 = DML_NEXT(item) ;
        if ( item1 == NULL && T2_LOOP_CLOSED(loop) ) 
            item1 = DML_FIRST(T2_LOOP_EDGE_LIST(loop)) ;
        if ( item1 != NULL ) {
            i++ ;
            bbs_print_new_line ( outfile ) ;
            bbs_print_int ( outfile, i ) ;
            t2c_info_edge_vtcs ( DML_RECORD(item), 
                DML_RECORD(item1), outfile ) ;
        }
    }
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC void t2c_info_edge ( edge, outfile )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
FILE *outfile ;
{
    PT2 p0, p1 ;

    t2c_ept0 ( edge, p0 ) ;
    bbs_print_string ( outfile, "Start point : " ) ;
    c2a_print_pt ( outfile, p0 ) ;
    bbs_print_new_line ( outfile ) ;
    t2c_ept1 ( edge, p1 ) ;
    bbs_print_string ( outfile, "End point   : " ) ;
    c2a_print_pt ( outfile, p1 ) ;
    bbs_print_new_line ( outfile ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void t2c_info_edge_vtcs ( edge0, edge1, outfile )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge0, edge1 ;
FILE *outfile ;
{
    PT2 p0, p1, diff, t0, t1 ;

    bbs_print_new_line ( outfile ) ;
    t2c_ept_tan1 ( edge0, p0, t0 ) ;
    t2c_ept_tan0 ( edge1, p1, t1 ) ;
    C2V_SUB ( p1, p0, diff ) ;
    bbs_print_string ( outfile, "Diff   : " ) ;
    c2a_print_pt ( outfile, diff ) ;
    bbs_print_new_line ( outfile ) ;
    bbs_print_string ( outfile, "Angle  : " ) ;
    bbs_print_real ( outfile, c2v_vecs_angle ( t0, t1 ) ) ;
    bbs_print_new_line ( outfile ) ;
    c2v_normalize ( t0, t0 ) ;
    c2v_normalize ( t1, t1 ) ;
    bbs_print_string ( outfile, "Dot    : " ) ;
    bbs_print_real ( outfile, C2V_DOT ( t0, t1 ) ) ;
    bbs_print_new_line ( outfile ) ;
    bbs_print_string ( outfile, "Cross  : " ) ;
    bbs_print_real ( outfile, C2V_CROSS ( t0, t1 ) ) ;
    bbs_print_new_line ( outfile ) ;
    bbs_print_string ( outfile, "Dot0   : " ) ;
    bbs_print_real ( outfile, C2V_DOT ( t0, diff ) ) ;
    bbs_print_new_line ( outfile ) ;
    bbs_print_string ( outfile, "Cross0 : " ) ;
    bbs_print_real ( outfile, C2V_CROSS ( t0, diff ) ) ;
    bbs_print_new_line ( outfile ) ;
    bbs_print_string ( outfile, "Dot1   : " ) ;
    bbs_print_real ( outfile, C2V_DOT ( t1, diff ) ) ;
    bbs_print_new_line ( outfile ) ;
    bbs_print_string ( outfile, "Cross1 : " ) ;
    bbs_print_real ( outfile, C2V_CROSS ( t1, diff ) ) ;
    bbs_print_new_line ( outfile ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

