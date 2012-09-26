/******************************* T2PDEFS.H *********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!       (C) Copyright 1989 - 1995       Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef T2PDEFS
#define T2PDEFS

#include <bbsdefs.h>
#include <dmdefs.h>
#include <m2defs.h>
#include <t2defs.h>

extern DML_LIST  t2p_transverse __(( T2_REGION, REAL, REAL, PT2 )) ;
extern T2_SPIRAL t2p_spiral __(( T2_REGION, REAL, REAL, PT2, BOOLEAN, INT )) ;
extern T2_SPIRAL t2p_spiral0 __(( T2_REGION, REAL, REAL, INT )) ; 
extern T2_SPIRAL t2p_create_spiral __(( void )) ;
extern void      t2p_free_spiral __(( T2_SPIRAL )) ;
extern DML_LIST t2p_rough_region __(( T2_REGION, REAL, BOOLEAN, DML_LIST )) ;
extern DML_LIST t2p_zigzag __(( T2_REGION, REAL, REAL, REAL, PT2 )) ;
extern DML_LIST t2p_unidir __(( T2_REGION, REAL, REAL, PT2 )) ;
extern void t2p_append_edge __(( DML_LIST, PT2, T2_EDGE )) ;
extern void t2p_append_chain __(( T2_EDGE, PARM, T2_EDGE, PARM, 
            BOOLEAN, ATTR, PT2, DML_LIST )) ;
extern BOOLEAN t2p_append_trim_dir __(( T2_EDGE, PARM, T2_EDGE, PARM, 
            BOOLEAN, ATTR, DML_LIST )) ;
extern void t2p_append_chain1 __(( BOOLEAN, PT2, DML_LIST, DML_LIST )) ;
extern void t2p_mark_edge_pr0 __(( T2_EDGE )) ;
extern void t2p_mark_edge_pr1 __(( T2_EDGE )) ;
extern void      t2p_clear_spiral __(( T2_SPIRAL )) ;
extern T2_SPIRAL t2p_exp_spiral __(( T2_REGION, PT2, REAL )) ;
extern T2_SPIRAL t2p_exp_decompose __(( T2_REGION, PT2, REAL )) ;
extern T2_LOOP   t2p_back_angle __(( T2_LOOP, REAL )) ;
extern T2_LOOP  t2p_translate __(( T2_LOOP, REAL, REAL )) ;
extern BOOLEAN  t2p_append_line_list __(( DML_LIST, PT2, PT2, ATTR )) ;
extern void t2p_append_line __(( REAL, REAL, REAL, REAL, DML_LIST, PT2 )) ;
extern BOOLEAN  t2p_spiral_int_pt __(( T2_SPIRAL, PT2 )) ;
extern BOOLEAN  t2p_spiral_ext_pt __(( T2_SPIRAL, PT2 )) ;
extern void     t2p_mark_spiral __(( T2_SPIRAL, ATTR )) ;
extern DML_ITEM t2p_last_hor_edge __(( T2_LOOP )) ;
extern T2_EDGE t2p_zigzag_first __(( DML_LIST )) ;
extern void t2p_zigzag_step0 __(( T2_XHATCH, T2_EDGE, T2_DIR, 
            PT2, DML_LIST )) ;
extern void t2p_zigzag_step __(( T2_XHATCH, INT, T2_DIR, PT2, DML_LIST )) ;
extern T2_EDGE t2p_zigzag_step1 __(( T2_XHATCH, T2_EDGE, T2_EDGE, 
            T2_DIR, BOOLEAN, PT2, DML_LIST )) ;
extern void t2p_zigzag_step_line __(( T2_XHATCH, INT, T2_DIR, PT2, DML_LIST )) ;
extern T2_DIR t2p_zigzag_dir0 __(( T2_EDGE, T2_DIR, T2_DIR )) ;
extern T2_DIR t2p_zigzag_dir1 __(( T2_EDGE )) ; 
extern T2_EDGE t2p_zigzag_next __(( DML_LIST, T2_EDGE, PT2, T2_DIR* )) ;
extern void t2p_zigzag_xform_loop __(( T2_LOOP, INT )) ;
extern void t2p_zigzag_append __(( DML_LIST, PT2, DML_LIST, INT )) ;
extern void t2p_zigzag_rough_init __(( REAL, PT2, PT2, DML_LIST, PT2 )) ;
extern T2_EDGE t2p_other_edge __(( T2_LOOP, T2_EDGE, T2_DIR )) ;
extern T2_XHATCH t2p_strip __(( T2_LOOP, T2_EDGE, T2_EDGE, REAL, INT, 
            BOOLEAN*, BOOLEAN* )) ;
extern void t2p_mark_procsd __(( DML_LIST )) ;
extern void t2p_mark_loop __(( T2_LOOP )) ;
extern T2_EDGE t2p_check_horline __(( T2_LOOP, T2_EDGE, T2_DIR, 
            DML_LIST, PT2 )) ;
extern void t2p_mark_start __(( T2_EDGE, INT )) ;
extern T2_PT_POSITION t2p_startpt __(( DML_LIST, PT2, PT2 )) ;
extern void t2p_mark_refs __(( DML_LIST )) ;
extern BOOLEAN t2p_hor_edge __(( T2_EDGE )) ;
extern T2_EDGE t2p_append_chain_check __(( T2_EDGE, PARM, T2_EDGE, PARM, 
            BOOLEAN, ATTR, ATTR, PT2, DML_LIST )) ;
extern void t2p_mark_edge __(( T2_EDGE, DML_ITEM )) ;
extern T2_EDGE t2p_check_edge1 __(( T2_EDGE, T2_LOOP, PT2 )) ;
extern T2_SPIRAL t2p_spiral_interior __(( T2_SPIRAL )) ; 
extern void     t2p_spiral_set_owner __(( T2_SPIRAL )) ; 
extern BOOLEAN t2p_spiral_connect_out __(( T2_LOOP, REAL, PT2, BOOLEAN )) ; 
extern BOOLEAN t2p_spiral_connect_in __(( T2_SPIRAL, REAL, PT2, BOOLEAN )) ; 
extern BOOLEAN t2p_sharp_loop __(( T2_LOOP, REAL, REAL )) ;
extern BOOLEAN t2p_spiral_extend __(( T2_LOOP, T2_LOOP, REAL, REAL )) ;
extern void t2p_leftover __(( T2_REGION, DML_LIST, REAL, INT, T2_SPIRAL )) ;
extern void t2p_leftover_1 __(( T2_REGION, DML_LIST, REAL, INT, T2_SPIRAL )) ;
extern void t2p_append_spiral_loop __(( T2_SPIRAL, T2_LOOP )) ; 
extern void t2p_append_spiral __(( T2_SPIRAL, T2_SPIRAL )) ; 
extern DML_LIST t2p_offset __(( T2_REGION, REAL, REAL, DML_LIST )) ;
extern T2_EDGE t2p_rough_edge __(( T2_REGION, DML_LIST, PT2, PT2 )) ; 
extern void t2p_startpt_extr __(( T2_LOOP, DML_LIST, T2_PARM )) ;
#endif 

