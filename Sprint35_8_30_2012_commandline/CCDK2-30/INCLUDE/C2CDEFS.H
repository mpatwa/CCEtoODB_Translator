/******************************** C2CDEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef     C2CDEFS
#define     C2CDEFS
#include    <c2defs.h>
#include    <dmdefs.h>

extern  BOOLEAN     c2c_eval __(( C2_CURVE DUMMY0 , PARM DUMMY1 , INT DUMMY2 ,
            PT2* DUMMY3 )) ;
extern  BOOLEAN     c2c_eval_pt __(( C2_CURVE DUMMY0 , PARM DUMMY1 ,
            PT2 DUMMY2 )) ;
extern  BOOLEAN     c2c_eval_tan __(( C2_CURVE DUMMY0 , PARM DUMMY1 ,
            PT2 DUMMY2 )) ;
extern  BOOLEAN     c2c_eval_pt_tan __(( C2_CURVE DUMMY0 , PARM DUMMY1 ,
            PT2 DUMMY2 , PT2 DUMMY3 )) ;
extern  REAL        c2c_eval_tan_angle __(( C2_CURVE DUMMY0 , PARM DUMMY1 )) ;
extern  BOOLEAN     c2c_ept0 __(( C2_CURVE DUMMY0 , PT2 DUMMY1 )) ;
extern  BOOLEAN     c2c_etan0 __(( C2_CURVE DUMMY0 , PT2 DUMMY1 )) ;
extern  BOOLEAN     c2c_ept_tan0 __(( C2_CURVE DUMMY0 , PT2 DUMMY1 ,
            PT2 DUMMY2 )) ;
extern  REAL        c2c_eval_angle0 __(( C2_CURVE DUMMY0 )) ;
extern  BOOLEAN     c2c_mid_pt __(( C2_CURVE DUMMY0 , PT2 DUMMY1 )) ;
extern  BOOLEAN     c2c_ept1 __(( C2_CURVE DUMMY0 , PT2 DUMMY1 )) ;
extern  BOOLEAN     c2c_etan1 __(( C2_CURVE DUMMY0 , PT2 DUMMY1 )) ;
extern  BOOLEAN     c2c_ept_tan1 __(( C2_CURVE DUMMY0 , PT2 DUMMY1 ,
            PT2 DUMMY2 )) ;
extern  REAL        c2c_eval_angle1 __(( C2_CURVE DUMMY0 )) ;
extern  BOOLEAN     c2c_closed __(( C2_CURVE DUMMY0 )) ;
extern  BOOLEAN     c2c_eval_offset __(( C2_CURVE DUMMY0 , PARM DUMMY1 ,
            INT DUMMY2 , REAL DUMMY3 , PT2* DUMMY4 )) ;
extern  BOOLEAN     c2c_eval_offset_pt __(( C2_CURVE DUMMY0 , PARM DUMMY1 ,
            REAL DUMMY2 , PT2 DUMMY3 )) ;
extern  C2_CURVE    c2c_copy __(( C2_CURVE DUMMY0 , C2_CURVE DUMMY1 )) ;
extern  BOOLEAN     c2c_box_curves __(( DML_LIST DUMMY0 , C2_BOX DUMMY1 )) ;
extern  BOOLEAN     c2c_box_curves_expand __(( DML_LIST DUMMY0 , 
            C2_BOX DUMMY1 )) ;
extern  BOOLEAN     c2c_curvature __(( C2_CURVE DUMMY0 , PARM DUMMY1 ,
            REAL* DUMMY2 )) ;
extern  INT         c2c_intersect __(( C2_CURVE DUMMY0 , C2_CURVE DUMMY1 ,
            DML_LIST DUMMY2 )) ;
extern  INT         c2c_self_intersect __(( C2_CURVE DUMMY0 , 
            DML_LIST DUMMY1 )) ;
extern  INT         c2c_intersect_ext __(( C2_CURVE DUMMY0 , C2_CURVE DUMMY1 ,
            DML_LIST DUMMY2 )) ;
extern  INT         c2c_intersect_line_or_arc __(( C2_CURVE DUMMY0 ,
            C2_CURVE DUMMY1 , REAL* DUMMY2 , REAL* DUMMY3 , PT2* DUMMY4 ,
            INT* DUMMY5 , BOOLEAN* DUMMY6 )) ;
extern  INT         c2c_intersect_line_or_arc_ext __(( C2_CURVE DUMMY0 ,
            C2_CURVE DUMMY1 , REAL* DUMMY2 , REAL* DUMMY3 , PT2* DUMMY4 ,
            INT* DUMMY5 , BOOLEAN* DUMMY6 )) ;
extern  BOOLEAN     c2c_project __(( C2_CURVE DUMMY0 , PT2 DUMMY1 ,
            PARM DUMMY2 , PT2 DUMMY3 )) ;
extern  BOOLEAN     c2c_project_ext __(( C2_CURVE DUMMY0 , PT2 DUMMY1 ,
            PARM DUMMY2 , PT2 DUMMY3 )) ;
extern  void        c2c_info_curve __(( C2_CURVE DUMMY0 , FILE* DUMMY1 )) ;
extern  void        c2c_info_int_rec __(( C2_INT_REC DUMMY0 , FILE* DUMMY1 )) ;
extern  BOOLEAN     c2c_select __(( C2_CURVE DUMMY0 , PT2 DUMMY1 ,
            REAL DUMMY2 , PARM DUMMY3 , REAL* DUMMY4 )) ;
extern  REAL        c2c_length __(( C2_CURVE DUMMY0 )) ;
extern  BOOLEAN     c2c_get_arc_data __(( C2_CURVE DUMMY0 , PT2 DUMMY1 ,
            REAL* DUMMY2 , REAL* DUMMY3 , REAL* DUMMY4 , INT* DUMMY5 )) ;
extern  BOOLEAN     c2c_get_arc_pts_d __(( C2_CURVE DUMMY0 , PT2 DUMMY1 ,
                PT2 DUMMY2 , REAL* DUMMY3 )) ;
extern  BOOLEAN     c2c_arc_angle __(( C2_CURVE DUMMY0 , PARM DUMMY1 ,
            REAL* DUMMY2 )) ;
extern  BOOLEAN     c2c_arc_parm __(( C2_CURVE DUMMY0 , REAL DUMMY1 ,
            PARM DUMMY2 )) ;
extern  BOOLEAN     c2c_get_arc_radius __(( C2_CURVE DUMMY0 , REAL* DUMMY1 )) ;
extern  BOOLEAN     c2c_get_arc_center __(( C2_CURVE DUMMY0 , PT2 DUMMY1 )) ;
extern  BOOLEAN     c2c_get_arc_start_angle __(( C2_CURVE DUMMY0 ,
            REAL* DUMMY1 )) ;
extern  BOOLEAN     c2c_get_arc_sweep __(( C2_CURVE DUMMY0 , REAL* DUMMY1 )) ;
extern  BOOLEAN     c2c_get_arc_direction __(( C2_CURVE DUMMY0 , 
            INT* DUMMY1 )) ;
extern  BOOLEAN     c2c_get_pcurve_data __(( C2_CURVE DUMMY0 , INT DUMMY1 ,
            BOOLEAN* DUMMY2 , PT2 DUMMY3 , PT2 DUMMY4 , REAL* DUMMY5 ,
            REAL* DUMMY6 , REAL* DUMMY7 , INT* DUMMY8 )) ;
extern  BOOLEAN     c2c_pcurve_segment_epts_d __(( C2_CURVE DUMMY0 , 
	    INT DUMMY1 , PT2 DUMMY2 , PT2 DUMMY3 , REAL* DUMMY4 )) ;
extern  INT         c2c_pcurve_n __(( C2_CURVE )) ;
extern  C2_CURVE    c2c_translate __(( C2_CURVE DUMMY0 , PT2 DUMMY1 )) ;
extern  C2_CURVE    c2c_rotate __(( C2_CURVE DUMMY0 , PT2 DUMMY1 ,
            REAL DUMMY2 )) ;
extern  C2_CURVE    c2c_rotate_cs __(( C2_CURVE DUMMY0 , PT2 DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 )) ;
extern  C2_CURVE    c2c_scale __(( C2_CURVE DUMMY0 , PT2 DUMMY1 , 
            REAL DUMMY2 )) ;
extern  C2_CURVE    c2c_mirror_line __(( C2_CURVE DUMMY0 , PT2*DUMMY1 )) ;
extern  C2_CURVE    c2c_mirror_dir __(( C2_CURVE DUMMY0 , PT2 DUMMY1 ,
            PT2 DUMMY2 )) ;
extern  C2_CURVE    c2c_transform __(( C2_CURVE DUMMY0 , 
            C2_TRANSFORM DUMMY1 )) ;
extern  BOOLEAN     c2c_approx __(( C2_CURVE DUMMY0 , PARM DUMMY1 ,
            PARM DUMMY2 , REAL DUMMY3 , BOOLEAN DUMMY4 , PT2* DUMMY5 ,
            PARM DUMMY6 , INT DUMMY7 , INT* DUMMY8 )) ;
extern  void        c2c_approx_init __(( C2_CURVE DUMMY0 , PARM DUMMY1 ,
            PARM DUMMY2 , BOOLEAN DUMMY3 )) ;
extern  C2_CURVE    c2c_trim0 __(( C2_CURVE DUMMY0 , PARM DUMMY1 )) ;
extern  C2_CURVE    c2c_trim1 __(( C2_CURVE DUMMY0 , PARM DUMMY1 )) ;
extern  C2_CURVE    c2c_trim0_verify __(( C2_CURVE DUMMY0 , PARM DUMMY1 )) ;
extern  C2_CURVE    c2c_trim1_verify __(( C2_CURVE DUMMY0 , PARM DUMMY1 )) ;
extern  C2_CURVE    c2c_trim __(( C2_CURVE DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 )) ;
extern  C2_CURVE    c2c_trim_t0 __(( C2_CURVE DUMMY0 , REAL DUMMY1 )) ;
extern  C2_CURVE    c2c_trim_t1 __(( C2_CURVE DUMMY0 , REAL DUMMY1 )) ;
extern  C2_CURVE    c2c_trim_t __(( C2_CURVE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 )) ;
extern  INT         c2c_coord_extrs __(( C2_CURVE DUMMY0 , INT DUMMY1 ,
            DML_LIST DUMMY2 )) ;
extern  BOOLEAN c2c_coord_extr __(( C2_CURVE DUMMY0 , INT DUMMY1 , 
            INT DUMMY2 , REAL *DUMMY3 )) ;
extern  void        c2c_display __(( C2_CURVE DUMMY0 , PARM DUMMY1 ,
            PARM DUMMY2 , REAL DUMMY3 , PT2 DUMMY4 , REAL DUMMY5 ,
             PF_DISPLAY DUMMY6 , ANY DUMMY7 )) ;
extern  BOOLEAN     c2c_fillet __(( C2_CURVE DUMMY0 , PARM DUMMY1 ,
            C2_CURVE DUMMY2 , PARM DUMMY3 , REAL DUMMY4 , PT2 DUMMY5 ,
            PT2 DUMMY6 , PT2 DUMMY7 )) ;
extern  BOOLEAN     c2c_fillet_corner __(( C2_CURVE DUMMY0 , C2_CURVE DUMMY1 ,
            REAL DUMMY2 , PARM DUMMY3 , PARM DUMMY4 , PT2 DUMMY5 ,
            PT2 DUMMY6 , PT2 DUMMY7 )) ;
extern  BOOLEAN     c2c_eval_equi_parm __(( C2_CURVE DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 , INT DUMMY3 , PT2* DUMMY4 , PT2 DUMMY5 , 
            PT2 DUMMY6 )) ; 
extern  DML_LIST c2c_appr_arc __(( C2_CURVE DUMMY0 , INT DUMMY1 , 
            REAL DUMMY2 , DML_LIST DUMMY3 , DML_LIST DUMMY4 )) ;
extern  DML_LIST c2c_appr_line __(( C2_CURVE DUMMY0 , INT DUMMY1 , 
            REAL DUMMY2 , DML_LIST DUMMY3 , DML_LIST DUMMY4 )) ;
extern  DML_LIST c2c_read_curves __(( STRING DUMMY0 , DML_LIST DUMMY1 )) ;
extern  C2_CURVE c2c_read_curve __(( STRING DUMMY0 )) ;
extern  DML_LIST c2c_get_curves __(( FILE* DUMMY0 , DML_LIST DUMMY1 )) ;
extern  C2_CURVE c2c_get_curve __(( FILE* DUMMY0 )) ;
extern  BOOLEAN  c2c_write_curves __(( STRING DUMMY0 , DML_LIST DUMMY1 )) ;
extern  BOOLEAN  c2c_write_curve __(( STRING DUMMY0 , C2_CURVE DUMMY1 )) ;
extern  BOOLEAN  c2c_put_curves __(( FILE* DUMMY0 , DML_LIST DUMMY1 )) ;
extern  BOOLEAN  c2c_put_curve __(( FILE* DUMMY0 , C2_CURVE DUMMY1 )) ;
extern  void c2c_divide __(( C2_CURVE DUMMY0 , INT DUMMY1 ,
             PARM DUMMY2 , PARM DUMMY3 , PT2* DUMMY4 , PARM DUMMY5 )) ;
extern  C2_CURVE c2c_reverse __(( C2_CURVE DUMMY0 )) ;
extern  DML_LIST c2c_sort_curve_list __(( DML_LIST DUMMY0 , PF_EVAL2 DUMMY1 , 
            PF_EVAL2 DUMMY2 , BOOLEAN DUMMY3 , DML_LIST DUMMY4 )) ;
extern  REAL c2c_gap_curves __(( DML_LIST DUMMY0 , PF_EVAL2 DUMMY1 , 
            PF_EVAL2 DUMMY2 , ANY* DUMMY3 , ANY* DUMMY4 )) ;
extern  INT c2c_t_pos __(( C2_CURVE DUMMY0 , REAL DUMMY1 )) ; 
extern  BOOLEAN c2c_t_inside __(( C2_CURVE DUMMY0 , REAL DUMMY1 )) ; 
extern  INT c2c_parm_pos __(( C2_CURVE DUMMY0 , PARM DUMMY1 )) ; 
extern  BOOLEAN c2c_parm_inside __(( C2_CURVE DUMMY0 , PARM DUMMY1 )) ; 
#ifdef SPLINE
extern  BOOLEAN     c2c_get_ellipse_data __(( C2_CURVE DUMMY0 , PT2 DUMMY1 ,
             REAL* DUMMY2 , REAL* DUMMY3 , REAL* DUMMY4 )) ;
extern  BOOLEAN  c2c_ellipse_to_nurb __(( C2_CURVE DUMMY0 )) ;
extern  BOOLEAN c2c_ellipse_pt_tan __(( PT2 DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , REAL DUMMY3 , REAL DUMMY4 , PT2 DUMMY5 , 
            PT2 DUMMY6 )) ;
#endif /*SPLINE*/
#endif /* C2CDEFS*/

