/******************************* T2DPRIV.H *********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!       (C) Copyright 1989 - 1995       Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef T2DPRIV
#define T2DPRIV

#include <bbsdefs.h>
#include <dmdefs.h>
#include <c2defs.h>
#include <t2defs.h>
extern void t2d_set_edge_buf_size __(( INT  )) ;
extern DML_LIST t2d_create_regions_ort __(( DML_LIST, DML_LIST, BOOLEAN )) ;
extern DML_LIST t2d_create_x_max_list __(( DML_LIST )) ;
extern DML_LIST t2d_link_loops __(( DML_LIST )) ;
extern void     t2d_set_ept0 __(( T2_EDGE, PT2 )) ;
extern void     t2d_set_ept1 __(( T2_EDGE, PT2 )) ;
extern DML_LIST t2d_close_hor_loop __(( T2_LOOP, REAL* )) ;
extern T2_LOOP  t2d_open_loop __(( T2_LOOP )) ;
extern T2_LOOP t2d_close_loop __(( T2_LOOP )) ;
extern BOOLEAN t2d_extend_edges __(( T2_EDGE, T2_EDGE, PT2, PT2, REAL )) ;
extern BOOLEAN t2d_close_extend __(( T2_LOOP, REAL )) ;
extern DML_LIST t2d_break_region1 __(( T2_REGION, ATTR, ATTR, 
            BOOLEAN, DML_LIST )) ;
extern DML_LIST t2d_break_region_right __(( T2_REGION, ATTR, 
            BOOLEAN, DML_LIST )) ;
extern DML_LIST t2d_break_region_left __(( T2_REGION, ATTR, DML_LIST )) ;
extern void    t2d_open_leadin __(( T2_LOOP, T2_LEAD_MODE, 
            REAL, REAL, BOOLEAN )) ;
extern void    t2d_open_leadout __(( T2_LOOP, T2_LEAD_MODE, 
            REAL, REAL, BOOLEAN )) ;
extern T2_LOOP t2d_leadin_loop __(( T2_LOOP, PT2, PT2, PT2, BOOLEAN, REAL )) ;
extern T2_LOOP t2d_leadout_loop __(( T2_LOOP, PT2, PT2, PT2, BOOLEAN, REAL )) ;
extern T2_EDGE t2d_translate_edge __(( T2_EDGE, PT2 )) ;
extern void    t2d_loop_leads __(( T2_LOOP, T2_LEAD_MODE, REAL, REAL, 
            T2_EDGE, T2_EDGE, INT, PT2 )) ;
extern BOOLEAN t2d_sort_edgelist __(( DML_LIST, REAL, DML_LIST )) ;
extern REAL     t2d_adjust_epts __(( T2_EDGE, T2_EDGE )) ;
extern BOOLEAN t2d_append_trim __(( T2_EDGE, PARM, T2_EDGE, PARM, 
            ATTR, DML_LIST )) ;
extern C2_CURVE t2d_loop_curve __(( T2_LOOP, DML_ITEM, REAL )) ;
#endif

