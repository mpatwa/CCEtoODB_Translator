/******************************* T2BPRIV.H *********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!       (C) Copyright 1989 - 1995       Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef T2BPRIV
#define T2BPRIV
#include <t2defs.h>
#include <t2link.h>

extern  T2_FRAGMENT t2b_create_fragment __(( T2_PARM DUMMY0 , 
            T2_PARM DUMMY1 )) ;
extern  void    t2b_free_fragment __(( T2_FRAGMENT DUMMY0  )) ;
extern  void t2b_fragment_loop __(( T2_LOOP DUMMY0 , PF_T2B_VALID DUMMY1 , 
            DML_LIST DUMMY2 )) ;
extern  void t2b_split __(( T2_REGION DUMMY0 , T2_REGION DUMMY1 )) ; 
extern  DML_LIST t2b_traverse __(( T2_REGION DUMMY0 , T2_REGION DUMMY1 , 
            DML_LIST DUMMY2 )) ; 
extern  DML_LIST t2b_diff_loops __(( T2_LOOP DUMMY0 , DML_LIST DUMMY1 , 
            DML_LIST DUMMY2 )) ;
extern  DML_LIST t2b_diff_loop_rgns __(( T2_LOOP DUMMY0 , DML_LIST DUMMY1 , 
            DML_LIST DUMMY2 )) ;
extern  void t2b_mark_link_delete __(( T2_LINK DUMMY0 , BOOLEAN DUMMY1 )) ;
extern  void t2b_delete_links __(( T2_LOOP DUMMY0 , BOOLEAN DUMMY1 )) ; 
extern  INT t2b_intersect_loop __(( T2_LOOP DUMMY0 , T2_LOOP DUMMY1 , 
            INT DUMMY2 , BOOLEAN DUMMY3 , DML_LIST DUMMY4 )) ;
extern  DML_LIST t2b_assemble __(( DML_LIST DUMMY0 , DML_LIST DUMMY1 )) ; 
extern  T2_PT_POSITION t2b_pt_pos_loop __(( T2_LOOP DUMMY0 , PT2 DUMMY1 )) ;
extern  void t2b_intersect __(( T2_REGION DUMMY0 , T2_REGION DUMMY1 )) ;
extern  void t2b_intersect_loop_region __(( T2_LOOP, T2_REGION, DML_LIST )) ;
extern  void t2b_mark_link_transverse __(( T2_LINK DUMMY0 , BOOLEAN DUMMY1 )) ;
extern  void    t2b_create_link __(( T2_EDGE, PARM, T2_EDGE, PARM, INT, PT2 )) ;
extern  T2_PT_POSITION t2b_prev_pos __(( DML_ITEM DUMMY0 , T2_REGION DUMMY1 )) ;
extern  T2_PT_POSITION t2b_next_pos __(( DML_ITEM DUMMY0 , T2_REGION DUMMY1 )) ;
extern  void t2b_verify_inside __(( T2_REGION DUMMY0 , T2_REGION DUMMY1 )) ;
extern  BOOLEAN t2b_verify_pass13 __(( T2_LOOP DUMMY0 )) ;
extern  DML_LIST t2b_loops __(( DML_LIST DUMMY0 , DML_LIST DUMMY1 )) ;
extern  void t2b_loops_split __(( DML_LIST DUMMY0 )) ;
extern  void t2b_loop_link_epts __(( T2_LOOP DUMMY0 , T2_REGION DUMMY1 )) ;
extern  BOOLEAN t2b_verify_pass14 __(( T2_LOOP DUMMY0 )) ;
extern  T2_LINK t2b_loop_pt0 __(( T2_LOOP DUMMY0 , PT2 DUMMY1 )) ;
extern  T2_LINK t2b_loop_pt1 __(( T2_LOOP DUMMY0 , PT2 DUMMY1 )) ;
extern  void    t2b_link_decouple __(( T2_LINK DUMMY0 )) ;
extern  void    t2b_verify_pass21 __(( T2_LOOP DUMMY0 , T2_REGION DUMMY1 )) ; 
extern  BOOLEAN t2b_prev_pt __(( DML_ITEM DUMMY0 , PT2 DUMMY1 )) ;
extern  BOOLEAN t2b_next_pt __(( DML_ITEM DUMMY0 , PT2 DUMMY1 )) ;
extern  void     t2b_verify_pass31 __(( T2_LOOP DUMMY0 , T2_LOOP DUMMY1 , 
            REAL DUMMY2 , T2_OFFSET_DESC DUMMY3 )) ;
extern  DML_LIST t2b_offset_pass0 __(( T2_LOOP DUMMY0 , REAL DUMMY1 , 
            DML_LIST DUMMY2 , DML_LIST DUMMY3 )) ;
extern  DML_LIST t2b_offset_pass1 __(( T2_LOOP DUMMY0 , DML_LIST DUMMY1 , 
            REAL DUMMY2 , DML_LIST DUMMY3 , DML_LIST DUMMY4 )) ;
extern  DML_LIST t2b_offset_pass2 __(( DML_LIST DUMMY0 , REAL DUMMY1 , 
            T2_OFFSET_DESC DUMMY2 , DML_LIST DUMMY3 )) ;
extern  void     t2b_offset_pass3 __(( T2_LOOP DUMMY0 , REAL DUMMY1 , 
            T2_OFFSET_DESC DUMMY2 , DML_LIST DUMMY3 )) ;
extern  DML_LIST t2b_offset_pass4 __(( DML_LIST DUMMY0  DUMMY0 , 
            DML_LIST DUMMY1 DUMMY1 )) ;
extern  void     t2b_offset_link_epts __(( T2_LOOP DUMMY0 , REAL DUMMY1 , 
            T2_OFFSET_DESC DUMMY2 , T2_LOOP DUMMY3 )) ;
extern  void t2b_remove_open_loops __(( DML_LIST DUMMY0 , DML_ITEM DUMMY1 )) ;
extern  T2_OD   t2b_od_lookup_list __(( DML_LIST DUMMY0 , ANY DUMMY1 , 
            REAL DUMMY2 , REAL* DUMMY3 )) ;
extern  REAL t2b_od_dist_max __(( REAL DUMMY0 , T2_OFFSET_DESC DUMMY1 )) ;
extern  REAL t2b_od_dist_max_list __(( REAL DUMMY0 , DML_LIST DUMMY1 )) ;
extern  void t2b_od_edge_pts __(( T2_OFFSET_DESC DUMMY0 )) ;
extern  void t2b_od_edge_pts_list __(( DML_LIST DUMMY0 )) ;
extern  DML_LIST t2b_split_region_self_inters __(( T2_REGION DUMMY0 , 
            DML_LIST DUMMY1 )) ;
extern  DML_LIST t2b_split_loop_self_inters __(( T2_LOOP DUMMY0 , 
            DML_LIST DUMMY1 )) ;
extern  DML_LIST t2b_create_regions __(( DML_LIST DUMMY0 , DML_LIST DUMMY1 )) ;
extern  DML_LIST t2b_diff_pass1 __(( T2_REGION DUMMY0 , DML_LIST DUMMY1 , 
            DML_LIST DUMMY2 )) ;
extern  BOOLEAN  t2b_offset_pt_pos __(( T2_LOOP DUMMY0 , REAL DUMMY1 , 
            T2_OFFSET_DESC DUMMY2 , PT2 DUMMY3 )) ;
extern  DML_LIST t2b_offset_rgn_pass1 __(( DML_LIST DUMMY0 , BOOLEAN DUMMY1 , 
            DML_LIST DUMMY2 )) ;
extern  BOOLEAN  t2b_offset_edge __(( T2_EDGE DUMMY0 , REAL DUMMY1 , 
            T2_OFFSET_DESC DUMMY2 , DML_LIST  DUMMY3 )) ;
extern  void t2b_pierce_loop __(( T2_LOOP DUMMY0 , PT2 DUMMY1 , REAL DUMMY2 , 
            BOOLEAN DUMMY3 , DML_LIST DUMMY4 , DML_LIST DUMMY5 )) ;
extern  DML_ITEM t2b_clst_loop __(( DML_LIST DUMMY0 , PT2 DUMMY1 , 
            PT2 DUMMY2 )) ;
extern  T2_VTX_STATUS t2b_vertex_convexity __(( T2_EDGE DUMMY0 , 
            T2_EDGE DUMMY1 , PT2 DUMMY2 , PT2  DUMMY3 )) ;
extern  void t2b_intersect_looplist __(( DML_LIST DUMMY0 , BOOLEAN DUMMY1 , 
            BOOLEAN DUMMY2 )) ;
extern  BOOLEAN t2b_offset_verify __(( T2_LOOP DUMMY0 , T2_LOOP DUMMY1 , 
            REAL DUMMY2 , T2_OFFSET_DESC DUMMY3 )) ;
extern  DML_LIST t2b_offset_extloop __(( T2_REGION DUMMY0 , REAL DUMMY1 , 
            T2_OFFSET_DESC DUMMY2 , DML_LIST DUMMY3 )) ;
extern  DML_LIST t2b_offset_looplist __(( DML_LIST, REAL, 
            T2_OFFSET_DESC, DML_LIST )) ;
extern  DML_LIST t2b_offset_regionlist __(( DML_LIST, REAL, 
            T2_OFFSET_DESC, DML_LIST )) ;
extern  DML_LIST t2b_leftover __(( T2_REGION DUMMY0 , REAL DUMMY1 , 
            DML_LIST DUMMY2 )) ;
extern  DML_LIST t2b_recut_region __(( T2_REGION DUMMY0 , REAL DUMMY1 , 
            DML_LIST DUMMY2 )) ;
extern  DML_LIST t2b_recut_loop __(( T2_LOOP DUMMY0 , REAL DUMMY1 , 
            DML_LIST DUMMY2 )) ;
extern  T2_OD t2b_od3_lookup_list __(( DML_LIST DUMMY0 , ANY DUMMY1 , 
            REAL DUMMY2 , REAL* DUMMY3 )) ;
extern  REAL t2b_od3_dist_max_list __(( REAL DUMMY0 , DML_LIST DUMMY1  )) ;
extern  DML_ITEM t2b_append_od3 __(( ANY DUMMY0 , PT2 DUMMY1 , 
            REAL DUMMY2 , T2_OFFSET_DESC DUMMY3 )) ;
extern  DML_ITEM t2b_append_od3_list __(( ANY DUMMY0 , PT2 DUMMY1 , 
            REAL DUMMY2 , DML_LIST DUMMY3 )) ;
extern  T2_OFFSET_DESC t2b_offset2_set_mode __(( T2_OFFSET_DESC DUMMY0 , 
           T2_OFFSET_MODE DUMMY1 )) ;
extern  INT t2b_offset2_curve __(( C2_CURVE DUMMY0 , T2_OFFSET_DESC DUMMY1 , 
            INT DUMMY2 , DML_LIST DUMMY3 )) ;
extern  DML_LIST t2b_loop_outside_region_list __(( T2_LOOP DUMMY0 , 
            DML_LIST DUMMY2 , DML_LIST DUMMY3 )) ;
extern  DML_LIST t2b_loop_inside_loop __(( T2_LOOP DUMMY0 , 
            T2_LOOP DUMMY2 , DML_LIST DUMMY3 )) ;
extern  void t2b_link_status __(( DML_ITEM DUMMY0 , T2_REGION DUMMY1 )) ;
extern  void t2b_fragment_pt __(( T2_FRAGMENT, PT2 )) ;
extern  BOOLEAN t2b_fragment_test __(( T2_FRAGMENT DUMMY0 , 
            DML_LIST DUMMY1 )) ;
extern  INT t2b_link_force __(( T2_LINK DUMMY0 , T2_LINK DUMMY1 )) ;
extern  BOOLEAN t2b_links_same_pos __(( T2_LINK, T2_LINK )) ;
extern  void t2b_classify __(( T2_REGION, T2_REGION )) ;
extern  T2_PT_POSITION t2b_pt_pos_region __(( T2_REGION DUMMY0 , 
            PT2 DUMMY1 )) ;
#endif

