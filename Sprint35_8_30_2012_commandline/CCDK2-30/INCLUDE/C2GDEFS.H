/******************************** C2GDEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     C2GDEFS
#define     C2GDEFS
#include <c2defs.h>
#include <dmdefs.h>

extern  void    c2g_set_buf_size __(( INT DUMMY0 )) ;
extern  C2_ARC  c2g_alloc __(( void )) ;
extern  void    c2g_free __(( C2_ARC DUMMY0 )) ;
extern  C2_ARC  c2g_create_copy __(( C2_ARC DUMMY0 )) ;
extern  C2_ARC  c2g_copy __(( C2_ARC DUMMY0 , C2_ARC DUMMY1 )) ;
extern  BOOLEAN c2g_offset __(( C2_ARC DUMMY0 , REAL DUMMY1 , C2_ARC DUMMY2 )) ;
extern  void    c2g_eval __(( C2_ARC DUMMY0 , REAL DUMMY1 , INT DUMMY2 ,
            PT2* DUMMY3 )) ;
extern  void    c2g_pt_tan __(( C2_ARC DUMMY0 , REAL DUMMY1 , PT2 DUMMY2 ,
            PT2 DUMMY3 )) ;
extern  REAL    c2g_rad __(( C2_ARC DUMMY0 )) ;
extern  REAL    c2g_start_angle __(( C2_ARC DUMMY0 )) ;
extern  REAL    c2g_sweep __(( C2_ARC DUMMY0 , REAL DUMMY1 , REAL DUMMY2 )) ;
extern  REAL    c2g_t_to_angle __(( C2_ARC DUMMY0 , REAL DUMMY1 )) ;
extern  REAL    c2g_angle_to_t __(( C2_ARC DUMMY0 , REAL DUMMY1 )) ;
extern  INT     c2g_dir __(( C2_ARC DUMMY0 )) ;
extern  REAL*   c2g_ctr __(( C2_ARC DUMMY0 , PT2 DUMMY1 )) ;
#ifdef SPLINE
extern  INT     c2g_bezs __(( C2_ARC DUMMY0 , BOOLEAN DUMMY1 , HPT2* DUMMY2 )) ;
#endif
extern  void    c2g_info __(( C2_ARC DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            FILE* DUMMY3 )) ;
extern  void    c2g_box __(( C2_ARC DUMMY0 , REAL DUMMY1 , REAL DUMMY2 , 
            C2_BOX DUMMY3 )) ;
extern  INT     c2g_coord_extrs __(( C2_ARC DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , INT DUMMY3 , DML_LIST DUMMY4 )) ;
extern  REAL    c2g_arc __(( PT2 DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , INT DUMMY4 , C2_ARC DUMMY5 )) ;
extern  C2_ARC  c2g_create_arc __(( PT2 DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , INT DUMMY4 , REAL* DUMMY5 )) ;
extern  void    c2g_circle __(( PT2 DUMMY0 , REAL DUMMY1 , C2_ARC DUMMY2 )) ;
extern  C2_ARC  c2g_create_circle __(( PT2 DUMMY0 , REAL DUMMY1 )) ;
extern  void    c2g_circle_ctr_pt __(( PT2 DUMMY0 , PT2 DUMMY1 , 
            C2_ARC DUMMY2 )) ;
extern  C2_ARC  c2g_create_circle_ctr_pt __(( PT2 DUMMY0 , PT2 DUMMY1 )) ;
extern  BOOLEAN c2g_3pts __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 ,
            C2_ARC DUMMY3 , REAL* DUMMY4 )) ;
extern  C2_ARC  c2g_create_3pts __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 )) ;
extern  BOOLEAN c2g_ctr_2pts __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 ,
            INT DUMMY3 , C2_ARC DUMMY4 , REAL* DUMMY5 )) ;
extern  C2_ARC  c2g_create_ctr_2pts __(( PT2 DUMMY0 , PT2 DUMMY1 ,
            PT2 DUMMY2 , INT DUMMY3 )) ;
extern  C2_ARC  c2g_create_ctr_2tans __(( PT2 DUMMY0 , REAL DUMMY1 , 
            PT2 DUMMY2 , PT2 DUMMY3 )) ;
extern  void c2g_arc_pts_bulge __(( PT2 DUMMY0 , PT2 DUMMY1 , 
            REAL DUMMY2 , C2_ARC DUMMY3 )) ;
extern  C2_ARC  c2g_create_zero_rad __(( PT2 DUMMY0 , PT2 DUMMY1 , 
            PT2 DUMMY2 , PT2 DUMMY3 , REAL DUMMY4 )) ;
extern  C2_ARC  c2g_2pts_tan __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 )) ;
extern  C2_ARC  c2g_2pts_tan1 __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 )) ;
extern  C2_ARC  c2g_2pts_2tans __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 ,
            PT2 DUMMY3 )) ;
extern  BOOLEAN c2g_2pts_tan_ctr __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 , 
            PT2 DUMMY3 )) ;
extern  REAL    c2g_length __(( C2_ARC DUMMY0 , REAL DUMMY1 , REAL DUMMY2 )) ;
extern  BOOLEAN c2g_approx __(( C2_ARC DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , BOOLEAN DUMMY4 , PT2* DUMMY5 , PARM DUMMY6 ,
             INT DUMMY7 , INT* DUMMY8 , REAL* DUMMY9 )) ;
extern  BOOLEAN c2g_approx_zoomed __(( C2_ARC DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 , REAL DUMMY4 , REAL DUMMY5 ,
             REAL DUMMY6 , PT2* DUMMY7 , PARM DUMMY8 , INT DUMMY9 ,
            INT* DUMMY10 , REAL* DUMMY11 )) ;
extern  BOOLEAN c2g_select __(( C2_ARC DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            PT2 DUMMY3 , REAL DUMMY4 , REAL* DUMMY5 , REAL* DUMMY6 )) ;
extern  BOOLEAN c2g_project __(( C2_ARC DUMMY0 , REAL DUMMY1 , REAL DUMMY2 , 
            PT2 DUMMY3 , REAL* DUMMY4 , PT2 DUMMY5 )) ;
extern  INT     c2g_int_line_arc_ext __(( C2_LINE DUMMY0 , C2_ARC DUMMY1 ,
            REAL* DUMMY2 , REAL* DUMMY3 ,  PT2* DUMMY4 , INT* DUMMY5 , 
            BOOLEAN* DUMMY6 )) ;
extern  INT     c2g_int_line_arc __(( C2_LINE DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , C2_ARC DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , 
            REAL* DUMMY6 , REAL* DUMMY7 ,  PT2* DUMMY8 , INT* DUMMY9 )) ;
extern  INT     c2g_int_arc_arc_ext __(( C2_ARC DUMMY0 , C2_ARC DUMMY1 ,
            REAL* DUMMY2 , REAL* DUMMY3 , PT2* DUMMY4 , INT* DUMMY5 ,
            BOOLEAN* DUMMY6 )) ;
extern  INT     c2g_int_arc_arc __(( C2_ARC DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , C2_ARC DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , 
            REAL* DUMMY6 , REAL* DUMMY7 , PT2* DUMMY8 , INT* DUMMY9 )) ;
extern  INT c2g_intersect_coinc __(( C2_ARC DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , C2_ARC DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , 
            REAL* DUMMY6 , REAL* DUMMY7 , PT2* DUMMY8 , INT* DUMMY9 )) ;
extern  void c2g_translate __(( C2_ARC DUMMY0 , PT2 DUMMY1 , 
            C2_ARC DUMMY2 )) ;
extern  void c2g_scale __(( C2_ARC DUMMY0 , PT2 DUMMY1 , REAL DUMMY2 ,
            C2_ARC DUMMY3 )) ;
extern  void c2g_rotate_cs __(( C2_ARC DUMMY0 , PT2 DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , C2_ARC DUMMY4 )) ;
extern  void    c2g_mirror __(( C2_ARC DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 ,
            C2_ARC DUMMY3 )) ;
extern  BOOLEAN c2g_transform __(( C2_ARC DUMMY0 , C2_TRANSFORM DUMMY1 ,
            C2_ARC DUMMY2 )) ;
extern  C2_ARC c2g_complement __(( C2_ARC DUMMY0 , C2_ARC DUMMY1 )) ;
extern  C2_ARC c2g_reverse __(( C2_ARC DUMMY0 , C2_ARC DUMMY1 )) ;
extern  C2_ARC c2g_get __(( FILE* DUMMY0 , PARM DUMMY1 , PARM DUMMY2 )) ;
extern  BOOLEAN c2g_put __(( FILE* DUMMY0 , C2_ARC DUMMY1 , PARM DUMMY2 , 
            PARM DUMMY3 )) ;
extern  void c2g_divide __(( C2_ARC DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 , PT2* DUMMY4 , PARM DUMMY5 )) ;
extern  void c2g_negate_d __(( C2_ARC DUMMY0 )) ;
extern  BOOLEAN c2g_zero_rad __(( C2_ARC DUMMY0 )) ;
extern  BOOLEAN c2g_curvature __(( C2_ARC DUMMY0 , REAL* DUMMY1 )) ;
extern  REAL c2g_get_d __(( C2_ARC DUMMY0 )) ;
extern  BOOLEAN  c2g_get_pts_d __(( C2_ARC DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , PT2 DUMMY3 , PT2 DUMMY4 , REAL* DUMMY5 )) ;

#endif /* C2GDEFS*/

