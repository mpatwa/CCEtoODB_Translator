/***************************** T2CPRIV.H ********************************/
/*************************  2-D Topology ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!   (C) Copyright 1989 - 1995             Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef T2CPRIV
#define T2CPRIV
#include <t2defs.h>
#include <t2link.h>
extern  T2_EDGE t2c_first_edge_attr __(( T2_LOOP DUMMY0 , ATTR  DUMMY1 )) ;
extern  T2_EDGE t2c_last_edge_no_attr __(( T2_LOOP DUMMY0 , ATTR  DUMMY1 )) ;
extern  T2_EDGE t2c_first_edge_no_attr __(( T2_LOOP DUMMY0 , ATTR  DUMMY1 )) ;
extern DML_ITEM t2c_project_looplist __(( DML_LIST, PT2, BOOLEAN, 
            T2_PARM, PT2 )) ;
extern BOOLEAN t2c_loop_make_first_edge __(( T2_LOOP, T2_EDGE )) ;
extern BOOLEAN t2c_loop_make_first_by_pt __(( T2_LOOP, PT2 )) ;
extern REAL t2c_edge_area __(( T2_EDGE, PT2 )) ;
extern REAL t2c_looplist_area __(( DML_LIST DUMMY0 )) ;
extern BOOLEAN t2c_desc_region_loop __(( T2_REGION DUMMY0 , T2_LOOP DUMMY1 , 
            T2_LOOP* DUMMY2 , T2_EDGE* DUMMY3 )) ;
extern BOOLEAN t2c_desc_loop_loop __(( T2_LOOP DUMMY0 , T2_LOOP DUMMY1 , 
            T2_LOOP* DUMMY2 , T2_EDGE* DUMMY3 )) ;
extern BOOLEAN t2c_desc_edge_loop __(( T2_EDGE DUMMY0 , T2_LOOP DUMMY1 , 
            T2_EDGE* DUMMY2 )) ;
extern void    t2c_walk_region_init __(( T2_REGION_WALK )) ;
extern T2_EDGE t2c_walk_region __(( T2_REGION, T2_REGION_WALK )) ;
extern void t2c_remark_region __(( T2_REGION, ATTR, ATTR )) ; 
extern void t2c_remark_loop __(( T2_LOOP, ATTR, ATTR )) ; 
extern void t2c_remark_edgelist __(( DML_LIST, ATTR, ATTR )) ; 
extern void t2c_lremark_region __(( T2_REGION, ATTR, ATTR )) ; 
extern void t2c_lremark_loop __(( T2_LOOP, ATTR, ATTR )) ; 
extern void t2c_mark_region __(( T2_REGION, ATTR )) ; 
extern void t2c_mark_looplist __(( DML_LIST, ATTR )) ; 
extern void t2c_mark_loop __(( T2_LOOP, ATTR )) ; 
extern void t2c_mark_edgelist __(( DML_LIST, ATTR )) ; 
extern void t2c_mark_region_part __(( T2_REGION )) ; 
extern void t2c_mark_loop_part __(( T2_LOOP )) ; 
extern void t2c_mark_edgelist_part __(( DML_LIST )) ; 
extern void t2c_mark_region_rough __(( T2_REGION )) ; 
extern void t2c_mark_loop_rough __(( T2_LOOP )) ; 
extern void t2c_mark_edgelist_rough __(( DML_LIST )) ; 
extern void t2c_attr_init_region __(( T2_REGION )) ; 
extern void t2c_attr_init_loop __(( T2_LOOP )) ; 
extern void t2c_attr_init_edgelist __(( DML_LIST )) ; 
extern T2_EDGE t2c_edge_by_attr __(( T2_LOOP, ATTR )) ; 
extern T2_EDGE t2c_edge_by_attr_rgn __(( T2_REGION, ATTR )) ; 
extern void t2c_lmark_region __(( T2_REGION, ATTR )) ; 
extern void t2c_lmark_loop __(( T2_LOOP, ATTR )) ; 
extern void t2c_lattr_init_region __(( T2_REGION )) ; 
extern void t2c_lattr_init_loop __(( T2_LOOP )) ; 
extern void t2c_orphan_region __(( T2_REGION )) ; 
extern void t2c_orphan_loop __(( T2_LOOP )) ; 
extern void t2c_orphan_edge __(( T2_EDGE )) ; 
extern T2_EDGE t2c_child_in_loop __(( T2_EDGE, T2_LOOP )) ;
extern T2_EDGE t2c_child_in_region __(( T2_EDGE, T2_REGION )) ;
extern T2_EDGE t2c_child_in_looplist __(( T2_EDGE, DML_LIST )) ;
extern T2_EDGE t2c_child_in_regionlist __(( T2_EDGE, DML_LIST )) ;
extern BOOLEAN t2c_closed __(( T2_EDGE )) ;
extern void    t2c_parm_adjust __(( T2_EDGE, PARM )) ;
extern void t2c_info_edge_vtcs __(( T2_EDGE DUMMY0 , T2_EDGE DUMMY1 , 
            FILE* DUMMY2 )) ;
extern void t2c_break_region_at_extrs __(( T2_REGION, INT )) ;
extern void t2c_break_loop_at_extrs __(( T2_LOOP, INT )) ;
extern void t2c_break_loop_at_extrs_attr __(( T2_LOOP, INT, ATTR )) ;
extern void t2c_break_loop_at_tangents __(( T2_LOOP, REAL, ATTR )) ;
extern DML_LIST t2c_break_edge_at_extrs __(( T2_EDGE, INT )) ;
extern INT t2c_loop_vtx_extrs __(( T2_LOOP, INT, DML_LIST )) ;
extern  void    t2c_loop_extr_vtcs __(( T2_LOOP, INT, PT2, PT2 )) ;
extern  void    t2c_replace_by_line __(( T2_LOOP, T2_EDGE, PARM, 
            T2_EDGE, PARM )) ;
extern void t2c_order_looplist __(( DML_LIST DUMMY0 , PT2  DUMMY1 )) ;
extern void t2c_order_edgelist __(( DML_LIST DUMMY0 , PT2  DUMMY1 )) ;
extern void t2c_order_list __(( DML_LIST DUMMY0 , PT2 DUMMY1 , 
            PF_T2C_EVAL DUMMY2 , PF_T2C_EVAL  DUMMY3 )) ;
extern  REAL    t2c_get_gap_coeff __ (( void )) ;
extern  void    t2c_set_gap_coeff __ (( REAL  DUMMY0 )) ;
extern  REAL t2c_loop_x_max __(( T2_LOOP, T2_PARM )) ;
extern  INT t2c_loop_rearrange_mid __(( T2_LOOP, T2_EDGE, PT2 )) ;
extern  void t2c_trim_curve_by_loop_0 __(( C2_CURVE, T2_LOOP )) ;
extern  void t2c_trim_curve_by_loop_1 __(( C2_CURVE, T2_LOOP )) ;
extern  DML_ITEM t2c_insert_link __(( T2_LOOP, T2_LINK )) ; 
extern  BOOLEAN t2c_start_open_loop __(( T2_LOOP, PT2 )) ; 
extern  DML_LIST t2c_break_loop_by_attr __(( T2_LOOP )) ; 
extern  BOOLEAN t2c_connect_2loops __(( T2_LOOP, T2_LOOP, REAL, BOOLEAN, 
            PT2, PT2, T2_PARM, T2_PARM )) ;
extern  BOOLEAN t2c_loop_pt_attr __(( T2_LOOP, ATTR, PT2, T2_PARM )) ;
extern  BOOLEAN t2c_merge_hor_edges __(( T2_LOOP )) ;
extern  INT t2c_compare_links __(( T2_LINK DUMMY0 , T2_LINK DUMMY1 )) ;
#endif

