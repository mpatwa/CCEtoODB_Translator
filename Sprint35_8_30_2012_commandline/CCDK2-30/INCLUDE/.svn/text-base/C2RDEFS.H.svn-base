/******************************** C2RDEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef     C2RDEFS
#define     C2RDEFS
#include <c2defs.h>

#define C2R_QR(A,Q,R)   {Q[0]=C2_ASEG_PT1(A)[0]-C2_ASEG_PT0(A)[0];\
Q[1]=C2_ASEG_PT1(A)[1]-C2_ASEG_PT0(A)[1];R[0]= -Q[1];R[1]=Q[0];}
#define C2R_UV(A,U,V)   \
{V=2.0*C2_ASEG_D(A)/(1.0+C2_ASEG_D(A)*C2_ASEG_D(A));U=1.0-C2_ASEG_D(A)*V;}

extern  BOOLEAN c2r_eval __(( C2_ASEG DUMMY0 , REAL DUMMY1 , INT DUMMY2 ,
            PT2* DUMMY3 )) ;
extern  BOOLEAN c2r_pt_tan __(( C2_ASEG DUMMY0 , REAL DUMMY1 , PT2 DUMMY2 ,
            PT2 DUMMY3 )) ;
extern  void    c2r_tan0 __(( C2_ASEG DUMMY0 , PT2 DUMMY1 )) ;
extern  void    c2r_tan1 __(( C2_ASEG DUMMY0 , PT2 DUMMY1 )) ;
extern  REAL    c2r_curvature __(( C2_ASEG DUMMY0 )) ;
extern  void    c2r_box __(( C2_ASEG DUMMY0 , REAL DUMMY1 , REAL DUMMY2 , 
            C2_BOX DUMMY3 )) ;
extern  void    c2r_box_append __(( C2_ASEG DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , C2_BOX DUMMY3 )) ;
extern  INT     c2r_coord_extrs __(( C2_ASEG DUMMY0 , INT DUMMY1 ,
            REAL* DUMMY2 )) ;
extern  REAL    c2r_length __(( C2_ASEG DUMMY0 )) ;
extern  C2_ASEG c2r_reverse __(( C2_ASEG DUMMY0 , C2_ASEG DUMMY1 )) ;
extern  C2_ASEG c2r_complement __(( C2_ASEG DUMMY0 , C2_ASEG DUMMY1 )) ;
extern  BOOLEAN c2r_select __(( C2_ASEG DUMMY0 , PT2 DUMMY1 , REAL DUMMY2 ,
            REAL* DUMMY3 , REAL* DUMMY4 )) ;
extern  BOOLEAN c2r_project __(( C2_ASEG DUMMY0 , PT2 DUMMY3 , 
            REAL* DUMMY4 , PT2 DUMMY5 )) ;
extern  BOOLEAN c2r_get_data __(( C2_ASEG DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 ,
            REAL* DUMMY3 , REAL* DUMMY4 , REAL* DUMMY5 , INT* DUMMY6 )) ;
extern  REAL    *c2r_ctr __(( C2_ASEG DUMMY0 , PT2 DUMMY1 )) ;
extern  REAL    c2r_rad __(( C2_ASEG DUMMY0 )) ;
extern  REAL    c2r_sweep __(( C2_ASEG DUMMY0 , REAL DUMMY1 , REAL DUMMY2 )) ;
extern  REAL    c2r_angle_to_t __(( C2_ASEG DUMMY0 , REAL DUMMY1 )) ;
extern  REAL    c2r_t_to_angle __(( C2_ASEG DUMMY0 , REAL DUMMY1 )) ;
extern  INT     c2r_dir __(( C2_ASEG DUMMY0 )) ;
extern  REAL    c2r_flatness __(( C2_ASEG DUMMY0 )) ;
extern  void    c2r_sbdv __(( C2_ASEG DUMMY0 , C2_ASEG DUMMY1 , 
            C2_ASEG DUMMY2 )) ;
extern  C2_ASEG c2r_trim __(( C2_ASEG DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            C2_ASEG DUMMY3 )) ;
extern  C2_ASEG c2r_untrim __(( C2_ASEG DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            C2_ASEG DUMMY3 )) ;
extern  INT     c2r_intersect __(( C2_ASEG DUMMY0 , C2_ASEG DUMMY1 ,
            REAL* DUMMY2 , REAL* DUMMY3 , PT2* DUMMY4 , INT* DUMMY5 )) ;
extern  INT     c2r_inters_line __(( C2_ASEG DUMMY0 , C2_LINE DUMMY1 ,
            REAL* DUMMY2 , REAL* DUMMY3 , PT2* DUMMY4 , INT* DUMMY5 )) ;
extern  BOOLEAN c2r_offset __(( C2_ASEG DUMMY0 , REAL DUMMY1 , 
            C2_ASEG DUMMY2 )) ;
extern  BOOLEAN c2r_offset_zero_arc __(( C2_ASEG DUMMY0 , REAL DUMMY1 , 
            C2_ASEG DUMMY2 )) ;
extern  C2_ASEG c2r_arc __(( PT2 DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , INT DUMMY4 , C2_ASEG DUMMY5 )) ;
extern  C2_ASEG c2r_3pts __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 ,
            C2_ASEG DUMMY3 )) ;
extern  C2_ASEG c2r_2pts_tan __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 ,
            C2_ASEG DUMMY3 )) ;
extern  C2_ASEG c2r_2pts_ctr __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 ,
            INT DUMMY3 , C2_ASEG DUMMY4 )) ;
extern  C2_ASEG c2r_arc_pts_bulge __(( PT2 DUMMY0 , PT2 DUMMY1 , 
            REAL DUMMY2 , C2_ASEG DUMMY3 )) ;
extern  C2_ASEG c2r_2pts_2tans __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 ,
            PT2 DUMMY3 , C2_ASEG DUMMY4 )) ;
extern  C2_ASEG c2r_ctr_2tans __(( PT2 DUMMY0 , REAL DUMMY1 , PT2 DUMMY2 ,
            PT2 DUMMY3 , C2_ASEG DUMMY4 )) ;
extern  C2_ASEG c2r_zero_rad __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 ,
            PT2 DUMMY3 , REAL DUMMY4 , C2_ASEG DUMMY5 )) ;
extern  C2_ASEG c2r_circle __(( PT2 DUMMY0 , REAL DUMMY1 , C2_ASEG DUMMY2 )) ;
extern  C2_ASEG c2r_circle_ctr_pt __(( PT2 DUMMY0 , PT2 DUMMY1 , 
            C2_ASEG DUMMY2 )) ;
extern  C2_ASEG c2r_line __(( PT2 DUMMY0 , PT2 DUMMY1 , C2_ASEG DUMMY2 )) ;
extern  C2_ASEG c2r_copy __(( C2_ASEG DUMMY0 , C2_ASEG DUMMY1 )) ;
extern  C2_ASEG c2r_inflate __(( C2_ASEG DUMMY0 , REAL DUMMY1 , C2_ASEG DUMMY2 )) ;
extern  BOOLEAN c2r_approx __(( C2_ASEG DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            INT DUMMY3 , REAL DUMMY4 , BOOLEAN DUMMY5 ,  PT2* DUMMY6 ,
            PARM DUMMY7 , INT DUMMY8 , INT* DUMMY9 , REAL* DUMMY10 )) ;
extern  BOOLEAN c2r_approx_zoomed __(( C2_ASEG DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , INT DUMMY3 , REAL DUMMY4 , C2_BOX DUMMY5 ,
            REAL DUMMY6 , PT2* DUMMY7 , PARM DUMMY8 , INT DUMMY9 ,
            INT* DUMMY10 , REAL* DUMMY11 )) ;
extern  REAL*   c2r_d_vec __(( C2_ASEG DUMMY0 , PT2 DUMMY1 )) ;
extern  BOOLEAN c2r_2pts_tan_ctr __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 , 
            PT2 DUMMY3 )) ;
extern  INT c2r_intersect_coinc __(( C2_ASEG DUMMY0 , C2_ASEG DUMMY1 , 
            REAL* DUMMY2 , REAL* DUMMY3 , PT2* DUMMY4 , INT* DUMMY5 )) ; 
extern  void c2r_divide __(( C2_ASEG DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 , PT2* DUMMY4 , PARM DUMMY5 )) ;
extern  void c2r_get_epts_d __(( C2_ASEG DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 , 
            REAL* DUMMY3 )) ;
extern  REAL c2r_get_d __(( C2_ASEG DUMMY0 )) ;
extern  void c2r_negate_d __(( C2_ASEG DUMMY0 )) ;
extern  void c2r_translate __(( C2_ASEG DUMMY0 , PT2 DUMMY1 , 
            C2_ASEG DUMMY2 )) ;
extern  void c2r_scale __(( C2_ASEG DUMMY0 , PT2 DUMMY1 , REAL DUMMY2 ,
            C2_ASEG DUMMY3 )) ;
extern  void c2r_rotate_cs __(( C2_ASEG DUMMY0 , PT2 DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , C2_ASEG DUMMY4 )) ;
extern  void    c2r_mirror __(( C2_ASEG DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 ,
            C2_ASEG DUMMY3 )) ;
extern  BOOLEAN c2r_transform __(( C2_ASEG DUMMY0 , C2_TRANSFORM DUMMY1 ,
            C2_ASEG DUMMY2 )) ;
extern  void    c2r_bez __(( C2_ASEG DUMMY0 , HPT2* DUMMY1 )) ;
extern  void    c2r_bez_mid_pt __(( C2_ASEG DUMMY0 , HPT2 DUMMY1 )) ;
extern  BOOLEAN  c2r_get_pts_d __(( C2_ASEG DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , PT2 DUMMY3 , PT2 DUMMY4 , REAL* DUMMY5 )) ;
#ifdef SPLINE
extern  INT     c2r_bezs __(( C2_ASEG DUMMY0 , BOOLEAN DUMMY1 , 
            HPT2* DUMMY2 )) ;
#endif
#endif /* C2RDEFS*/


