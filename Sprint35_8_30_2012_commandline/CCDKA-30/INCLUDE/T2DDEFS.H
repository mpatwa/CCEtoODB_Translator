/******************************* T2DDEFS.H *********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!       (C) Copyright 1989 - 1995       Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef T2DDEFS
#define T2DDEFS

#include <bbsdefs.h>
#include <dmdefs.h>
#include <c2defs.h>
#include <t2defs.h>

#define t2d_reverse_region      t2c_reverse_region 
#define t2d_reverse_loop        t2c_reverse_loop
#define t2d_reverse_edgelist    t2c_reverse_edgelist 

extern T2_REGION t2d_create_region __(( DML_LIST )) ;
extern T2_REGION t2d_loop_to_region __(( T2_LOOP )) ;
extern void t2d_free_region __(( T2_REGION )) ;
extern void t2d_clear_region __(( T2_REGION )) ;
extern T2_LOOP t2d_create_loop __(( T2_REGION, DML_LIST )) ;
extern T2_LOOP t2d_append_edgelist __(( T2_LOOP, DML_LIST )) ;
extern T2_LOOP t2d_append_loop __(( T2_LOOP, T2_LOOP )) ;
extern void t2d_free_loop __(( T2_LOOP )) ;
extern void t2d_clear_loop __(( T2_LOOP )) ;
extern T2_EDGE t2d_create_edge __(( T2_LOOP, C2_CURVE, INT )) ;
extern void t2d_free_edge __(( T2_EDGE )) ;
extern DML_LIST t2d_create_edgelist __(( DML_LIST, REAL )) ;
extern T2_REGION t2d_copy_region __(( T2_REGION )) ;
extern T2_LOOP t2d_copy_loop __(( T2_LOOP, T2_EDGE, T2_EDGE )) ;
extern T2_EDGE t2d_copy_edge __(( T2_EDGE )) ;
extern void t2d_trim_loop __(( T2_LOOP, T2_EDGE, PARM, T2_EDGE, PARM )) ;
extern T2_EDGE t2d_trim_edge __(( T2_EDGE, PARM, PARM )) ;
extern T2_EDGE t2d_trim_ext_edge __(( T2_EDGE, PARM, PARM )) ;
extern T2_REGION t2d_build_region __(( DML_LIST, C2_CURVE, REAL )) ;
extern DML_LIST t2d_build_regions __(( DML_LIST, C2_CURVE, REAL, DML_LIST )) ;
extern T2_LOOP t2d_build_loop __(( T2_REGION, DML_LIST, C2_CURVE, REAL )) ;
extern DML_LIST t2d_build_looplist __(( T2_REGION, DML_LIST, 
            C2_CURVE, BOOLEAN, REAL )) ;
extern T2_REGION t2d_create_box __(( PT2, REAL, REAL, REAL )) ;
extern T2_REGION t2d_create_polyline __(( PT2*, INT )) ;
extern T2_REGION t2d_create_circle __(( PT2, REAL )) ;
extern T2_LOOP t2d_create_box_loop __(( PT2, REAL, REAL, REAL )) ;
extern T2_LOOP t2d_create_polyline_loop __(( PT2*, INT )) ;
extern T2_LOOP t2d_create_circle_loop __(( PT2, REAL )) ;
extern BOOLEAN t2d_append_curvelist __(( T2_LOOP, DML_LIST, C2_CURVE, REAL )) ;
extern T2_EDGE t2d_append_curve __(( T2_LOOP, C2_CURVE, REAL )) ;
extern DML_LIST t2d_create_regions __(( DML_LIST, DML_LIST )) ;
extern void t2d_decomp_loop __(( T2_LOOP, DML_LIST )) ;
extern void t2d_decomp_region __(( T2_REGION, DML_LIST )) ;
extern DML_LIST t2d_break_region __(( T2_REGION, ATTR, ATTR, DML_LIST )) ;
extern DML_ITEM t2d_append_edge __(( T2_LOOP, T2_EDGE, BOOLEAN )) ;
extern DML_ITEM t2d_insert_edge __(( T2_LOOP, DML_ITEM, T2_EDGE, BOOLEAN )) ;
extern T2_REGION t2d_rotate_region __(( T2_REGION, PT2, REAL )) ;
extern REAL     t2d_adjust_epts_region __(( T2_REGION, REAL )) ;
extern REAL     t2d_adjust_epts_loop __(( T2_LOOP, REAL )) ;
extern T2_LOOP t2d_loop_rearrange __(( T2_LOOP, PT2, T2_PARM )) ;
extern T2_LOOP t2d_loop_rearrange_pt __(( T2_LOOP, PT2, PT2 )) ;
extern T2_LOOP t2d_loop_rearrange_parm __(( T2_LOOP, T2_PARM )) ;
extern T2_LOOP t2d_loop_pcurve __(( T2_REGION, C2_CURVE )) ;
extern T2_LOOP t2d_fillet_loop __(( T2_LOOP, REAL )) ;
extern void t2d_append_segment __(( T2_REGION DUMMY1, PT2 DUMMY2, PT2 DUMMY3 ));

#endif

