/* -R -Z -S -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2D5.C ***********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989-1994        Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h>
#include <c2ddefs.h>
#include <c2vdefs.h>
#include <t2ddefs.h>

void t2d_append_segment ( T2_REGION region, PT2 p0, PT2 p1 )
{
    T2_LOOP loop;
    DML_ITEM item;
    T2_EDGE edge;
    C2_CURVE curve;

    if ( dml_length ( T2_REGION_LOOP_LIST(region) ) == 0 )
    {
        loop = t2d_create_loop ( region, dml_create_list() );
        dml_append_data ( T2_REGION_LOOP_LIST(region), loop );
    }

    item = dml_first ( T2_REGION_LOOP_LIST(region) );
    loop = dml_record ( item );

    if ( c2v_dist ( p0, p1 ) > 10.0 * bbs_get_tol() )
    {
        curve = c2d_line ( p0, p1 );
        edge = t2d_create_edge ( loop, curve, 1 );
        dml_append_data ( T2_LOOP_EDGE_LIST(loop), edge );
    }
}

