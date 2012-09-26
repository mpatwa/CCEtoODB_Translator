/***************************** T2CDEFS.H ********************************/
/*************************  2-D Topology ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!   (C) Copyright 1989 - 1995             Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef T2CDEFS
#define T2CDEFS

#include <bbsdefs.h>
#include <c2defs.h>
#include <t2defs.h>

#define t2c_length t2c_length_edge 

extern BOOLEAN t2c_eval_pt __(( T2_EDGE, PARM, PT2 )) ;
extern BOOLEAN t2c_eval_tan __(( T2_EDGE, PARM, PT2 )) ;
extern BOOLEAN t2c_eval_pt_tan __(( T2_EDGE, PARM, PT2, PT2 )) ;
extern REAL    t2c_eval_tan_angle __(( T2_EDGE, PARM )) ;
extern BOOLEAN t2c_eval_offset __(( T2_EDGE, PARM, REAL, PT2 )) ;
extern BOOLEAN t2c_ept0 __(( T2_EDGE, PT2 )) ;
extern BOOLEAN t2c_etan0 __(( T2_EDGE, PT2 )) ;
extern BOOLEAN  t2c_ept_tan0 __(( T2_EDGE, PT2, PT2 )) ;
extern REAL    t2c_eval_angle0 __(( T2_EDGE )) ;
extern BOOLEAN t2c_ept_offset0 __(( T2_EDGE, REAL, PT2 )) ;
extern BOOLEAN t2c_mid_pt __(( T2_EDGE, PT2 )) ;
extern BOOLEAN t2c_mid_pt_tan __(( T2_EDGE, PT2, PT2 )) ;
extern void    t2c_mid_parm __(( T2_EDGE, PARM )) ;
extern BOOLEAN t2c_mid_pt_parm __(( T2_EDGE, PT2, T2_PARM )) ;
extern BOOLEAN t2c_loop_pt __(( T2_LOOP, PT2, T2_PARM )) ;
extern BOOLEAN t2c_loop_pt_closest __(( T2_LOOP, PT2, PT2, T2_PARM )) ;
extern BOOLEAN t2c_ept1 __(( T2_EDGE, PT2 )) ;
extern BOOLEAN t2c_etan1 __(( T2_EDGE, PT2 )) ;
extern BOOLEAN  t2c_ept_tan1 __(( T2_EDGE, PT2, PT2 )) ;
extern REAL    t2c_eval_angle1 __(( T2_EDGE )) ;
extern BOOLEAN t2c_ept_offset1 __(( T2_EDGE, REAL, PT2 )) ;
extern BOOLEAN t2c_vec __(( T2_EDGE, PT2 )) ;
extern BOOLEAN t2c_loop_closed __(( T2_LOOP )) ;
extern BOOLEAN t2c_loop_ept_smooth __(( T2_LOOP )) ;
extern BOOLEAN t2c_loop_smooth_at __(( T2_PARM )) ;
extern BOOLEAN t2c_curvature __(( T2_EDGE edge, PARM parm, REAL *curv_ptr )) ;
extern  INT    t2c_offset_dir __(( T2_LOOP, PT2, T2_SIDE )) ;
extern BOOLEAN t2c_project __(( T2_EDGE, PT2, PARM, PT2  )) ;
extern BOOLEAN t2c_project_loop __(( T2_LOOP, PT2, BOOLEAN, T2_PARM, PT2 )) ;
extern BOOLEAN t2c_project_chain __(( T2_LOOP, T2_PARM, T2_PARM, PT2, 
            BOOLEAN, T2_PARM, PT2 )) ;
extern REAL    t2c_dist_pt_loop __(( T2_LOOP, PT2 )) ;
extern T2_LOOP t2c_project_region __(( T2_REGION, PT2, BOOLEAN, 
            T2_PARM, PT2 )) ;
extern BOOLEAN t2c_project_attr __(( T2_EDGE, PT2, ATTR, PARM, PT2  )) ;
extern BOOLEAN t2c_project_loop_attr __(( T2_LOOP, PT2, ATTR, BOOLEAN, 
            T2_PARM, PT2 )) ;
extern T2_REGION t2c_translate_region __(( T2_REGION, PT2 )) ;
extern T2_LOOP t2c_translate_loop __(( T2_LOOP, PT2 )) ;
extern T2_EDGE t2c_translate_edge __(( T2_EDGE, PT2 )) ;
extern T2_LOOP t2c_mirror_dir_loop __(( T2_LOOP, PT2, PT2 )) ;
extern T2_EDGE t2c_mirror_dir_edge __(( T2_EDGE, PT2, PT2 )) ;
extern T2_REGION t2c_rotate_region __(( T2_REGION, PT2, REAL )) ;
extern T2_REGION t2c_rotate_cs_region __(( T2_REGION, PT2, REAL, REAL )) ;
extern void     t2c_rotate_looplist __(( DML_LIST, PT2, REAL )) ;
extern T2_LOOP t2c_rotate_loop __(( T2_LOOP, PT2, REAL )) ;
extern T2_LOOP t2c_rotate_cs_loop __(( T2_LOOP, PT2, REAL, REAL )) ;
extern BOOLEAN t2c_select __(( T2_EDGE, PT2, REAL, PARM )) ;
extern REAL t2c_length_edge __(( T2_EDGE DUMMY0 , 
            PARM DUMMY1 , PARM DUMMY2 )) ;
extern REAL t2c_length_loop __(( T2_LOOP DUMMY0 ,
            T2_PARM DUMMY1 , T2_PARM DUMMY2 )) ;
extern T2_DIR t2c_orientation __(( T2_LOOP )) ;
extern void   t2c_orient_loop __(( T2_LOOP , BOOLEAN )) ;
extern T2_REGION t2c_copy_region __(( T2_REGION )) ;
extern REAL t2c_loop_area __(( T2_LOOP DUMMY0 )) ;
extern REAL t2c_edge_size __(( T2_EDGE DUMMY0 )) ;
extern REAL t2c_region_area __(( T2_REGION DUMMY0 )) ;
extern BOOLEAN t2c_loop_ept0 __(( T2_LOOP, PT2 )) ;
extern BOOLEAN t2c_loop_ept_tan0 __(( T2_LOOP, PT2, PT2 )) ;
extern BOOLEAN t2c_loop_offset0 __(( T2_LOOP, REAL, PT2 )) ;
extern BOOLEAN t2c_loop_ept1 __(( T2_LOOP, PT2 )) ;
extern BOOLEAN t2c_loop_ept_tan1 __(( T2_LOOP, PT2, PT2 )) ;
extern BOOLEAN t2c_loop_offset1 __(( T2_LOOP, REAL, PT2 )) ;
extern INT t2c_tan_angle __(( T2_EDGE, REAL, PARM )) ;
/*****************/
extern T2_PT_POSITION t2c_pt_pos_region __(( T2_REGION, PT2 )) ;
extern T2_PT_POSITION t2c_pt_pos_region_union __(( DML_LIST, PT2 )) ;
extern T2_PT_POSITION t2c_pt_pos_loop __(( T2_LOOP, PT2 )) ;
extern T2_PT_POSITION t2c_pt_pos_looplist __(( DML_LIST, PT2, DML_ITEM* )) ;
extern T2_PT_POSITION t2c_pt_pos_loops_common __(( DML_LIST, PT2 )) ;
extern void t2c_loop_box __(( T2_LOOP )) ;
extern INT t2c_loop_rearrange_pt __(( T2_LOOP, PT2, BOOLEAN, PT2 )) ;
extern INT t2c_loop_rearrange_parm __(( T2_LOOP, T2_PARM )) ;
extern T2_EDGE t2c_loop_insert_vtx __(( T2_LOOP, PT2, PT2 )) ;
extern T2_EDGE t2c_loop_insert_vtx_parm __(( T2_LOOP, T2_PARM )) ;
extern T2_EDGE t2c_prev_edge __(( T2_EDGE DUMMY0 )) ;
extern T2_EDGE t2c_next_edge __(( T2_EDGE DUMMY0 )) ;
extern void t2c_info_region __(( T2_REGION DUMMY0 , FILE* DUMMY1 )) ;
extern void t2c_info_loop __(( T2_LOOP DUMMY0 , FILE* DUMMY1 )) ;
extern void t2c_info_edge __(( T2_EDGE DUMMY0 , FILE* DUMMY1 )) ;
extern void t2c_info_loop_vtcs __(( T2_LOOP DUMMY0 , FILE* DUMMY1 )) ;
extern  BOOLEAN t2c_pt_in_region __(( T2_REGION, PT2 )) ;
extern  BOOLEAN t2c_pt_in_loop __(( T2_LOOP, PT2 )) ;
extern  T2_LOOP t2c_loops_pos __(( T2_LOOP, T2_LOOP )) ;
extern  void t2c_trim_edge __(( T2_EDGE, PARM, PARM )) ;
extern  BOOLEAN t2c_trim_edge_verify __(( T2_EDGE, PARM, PARM )) ;
extern  BOOLEAN t2c_loop_coord_extr __(( T2_LOOP, INT, INT, REAL* )) ;
extern  BOOLEAN t2c_loop_coord_extr_attr __(( T2_LOOP, INT, INT, 
            ATTR, REAL* )) ;
extern  BOOLEAN t2c_edge_coord_extr __(( T2_EDGE, INT, INT, REAL* )) ;
extern  void t2c_close_loop __(( T2_LOOP )) ;
extern  void t2c_join_lines_region __(( T2_REGION )) ; 
extern  void t2c_join_lines_loop __(( T2_LOOP )) ;
extern  BOOLEAN t2c_join_lines_edges __(( T2_EDGE, T2_EDGE )) ;
extern  T2_EDGE t2c_biggest_edge __(( T2_LOOP )) ;
extern  T2_EDGE t2c_first_edge __(( T2_LOOP )) ;
extern  T2_EDGE t2c_last_edge __(( T2_LOOP )) ;
extern  void    t2c_trim_loop __(( T2_LOOP, T2_EDGE, PARM, T2_EDGE, PARM )) ;
extern  BOOLEAN t2c_write_region __(( STRING, T2_REGION )) ;
extern  BOOLEAN t2c_write_looplist __(( STRING, DML_LIST )) ;
extern  BOOLEAN t2c_put_looplist __(( FILE*, DML_LIST )) ;
extern  BOOLEAN t2c_write_loop __(( STRING, T2_LOOP )) ; 
extern  BOOLEAN t2c_put_loop __(( FILE*, T2_LOOP )) ; 
extern  BOOLEAN t2c_write_edgelist __(( STRING, DML_LIST )) ; 
extern  BOOLEAN t2c_put_edgelist __(( FILE*, DML_LIST )) ; 
extern  BOOLEAN t2c_write_edge __(( STRING, T2_EDGE )) ; 
extern  BOOLEAN t2c_put_edge __(( FILE*, T2_EDGE )) ; 
extern  BOOLEAN t2c_write_region_attr __(( STRING, T2_REGION, ATTR )) ;
extern  BOOLEAN t2c_write_looplist_attr __(( STRING, DML_LIST, ATTR )) ;
extern  BOOLEAN t2c_put_looplist_attr __(( FILE*, DML_LIST, ATTR )) ;
extern  BOOLEAN t2c_write_loop_attr __(( STRING, T2_LOOP, ATTR )) ; 
extern  BOOLEAN t2c_put_loop_attr __(( FILE*, T2_LOOP, ATTR )) ; 
extern  BOOLEAN t2c_write_edgelist_attr __(( STRING, DML_LIST, ATTR )) ; 
extern  BOOLEAN t2c_put_edgelist_attr __(( FILE*, DML_LIST, ATTR )) ; 
extern  BOOLEAN t2c_put_edge_attr __(( FILE*, T2_EDGE, ATTR )) ; 
extern  T2_EDGE t2c_get_edge __(( FILE* )) ; 
extern  DML_LIST t2c_get_edgelist __(( FILE* )) ; 
extern  T2_LOOP t2c_get_loop __(( FILE* )) ; 
extern  DML_LIST t2c_get_looplist __(( FILE* )) ; 
extern  T2_LOOP t2c_read_loop __(( STRING )) ; 
extern  T2_EDGE t2c_read_edge __(( STRING )) ; 
extern  DML_LIST t2c_read_looplist __(( STRING )) ; 
extern  T2_REGION t2c_read_region __(( STRING )) ; 

extern  void    t2c_reverse_region __(( T2_REGION  DUMMY0 )) ;
extern  void    t2c_reverse_loop __(( T2_LOOP  DUMMY0 )) ;
extern  void    t2c_reverse_edgelist __(( DML_LIST  DUMMY0 )) ;
extern  T2_EDGE t2c_transform_edge __(( T2_EDGE DUMMY0 , 
            C2_TRANSFORM  DUMMY1 )) ;
extern  T2_LOOP t2c_transform_loop __(( T2_LOOP DUMMY0 , 
            C2_TRANSFORM  DUMMY1 )) ;
extern  T2_REGION t2c_transform_region __(( T2_REGION DUMMY0 , 
            C2_TRANSFORM  DUMMY1 )) ;
extern  REAL t2c_loop_min_angle __(( T2_LOOP DUMMY0 , T2_EDGE* DUMMY1 )) ;
extern  void t2c_divide_loop __(( T2_LOOP DUMMY0 , INT DUMMY1 , 
            T2_PARM DUMMY2 , T2_PARM DUMMY3 , PT2* DUMMY4 , 
            T2_PARM  DUMMY5 )) ;
extern  void t2c_divide __(( T2_EDGE DUMMY0 , INT DUMMY1 , 
            PARM DUMMY2 , PARM DUMMY3 , PT2* DUMMY4 , PARM  DUMMY5 )) ;
extern  REAL t2c_region_min_angle __(( T2_REGION DUMMY0 , T2_EDGE* DUMMY1 )) ;
extern INT t2c_compare_cparms __(( T2_EDGE DUMMY0 , PARM DUMMY1 , 
            T2_EDGE DUMMY2 , PARM DUMMY3 )) ;
extern INT t2c_compare_tparms __(( T2_PARM DUMMY0 , T2_PARM DUMMY1 )) ;
extern void t2c_set_cparm __(( T2_EDGE DUMMY0 , REAL DUMMY1 , PARM DUMMY2 )) ;
extern void t2c_set_tparm __(( T2_EDGE DUMMY0 , REAL DUMMY1 , 
            T2_PARM DUMMY2 )) ;
extern void t2c_box_loops __(( DML_LIST DUMMY0 , C2_BOX DUMMY1 )) ;
extern BOOLEAN t2c_fillet_loop __(( T2_LOOP DUMMY0 , REAL DUMMY1 )) ;
extern BOOLEAN t2c_extend_loop __(( T2_LOOP DUMMY0 , REAL DUMMY1 )) ;
extern BOOLEAN t2c_extend_loop0 __(( T2_LOOP DUMMY0 , REAL DUMMY1 )) ;
extern BOOLEAN t2c_extend_loop1 __(( T2_LOOP DUMMY0 , REAL DUMMY1 )) ;
extern DML_LIST t2c_approx_chain __(( T2_LOOP DUMMY0 , T2_PARM DUMMY1 , 
            T2_PARM DUMMY2 , REAL DUMMY3 , DML_LIST DUMMY4 )) ;
extern BOOLEAN t2c_along __(( T2_EDGE DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 , REAL DUMMY3 , INT DUMMY4 , PARM DUMMY5 )) ;
extern BOOLEAN t2c_along_loop __(( T2_LOOP DUMMY0 , T2_PARM DUMMY1 , 
            T2_PARM DUMMY2 , REAL DUMMY3 , INT DUMMY4 , T2_PARM DUMMY5 )) ;
extern INT t2c_approx_n __(( T2_EDGE DUMMY0 , PARM DUMMY1 , PARM DUMMY2 , 
            REAL DUMMY3 )) ;
extern BOOLEAN t2c_match_loops __(( T2_LOOP DUMMY0 , T2_LOOP DUMMY1 , 
            REAL DUMMY2 , REAL DUMMY3 , C2_XFORM DUMMY4 )) ;
extern BOOLEAN t2c_loop_is_circle __(( T2_LOOP DUMMY0 , REAL DUMMY1 , 
            PT2 DUMMY2 , REAL* DUMMY3 )) ;

/*### jak */

extern void t2c_loop_heal_lines __(( T2_LOOP DUMMY0 ));

extern void t2c_loop_rearrange_to_nontan __(( T2_LOOP DUMMY0 ));

extern void t2c_loop_change_convex_corners __(( T2_LOOP DUMMY0, 
    REAL DUMMY1, T2_OFFSET_DESC DUMMY2 ));

extern BBS_PUBLIC INT t2c_local_extrema_region __(( T2_REGION region,
    INT coord, DML_LIST list ));

extern BBS_PUBLIC INT t2c_local_extrema_loop __(( T2_LOOP loop, 
    INT coord, DML_LIST list ));

extern BBS_PUBLIC void t2c_print_loc_extr __(( 
    T2_LOC_EXTR_REC extr_rec ));

extern BBS_PUBLIC INT t2c_loop_edge_pos __(( 
    T2_LOOP loop, T2_EDGE edge ));

extern BBS_PUBLIC T2_EDGE t2c_loop_edge_by_pos __(( 
    T2_LOOP loop, INT pos ));

/*### jak */

#endif

