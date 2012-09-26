/****************************** C2DDEFS.h **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef     C2DDEFS
#define     C2DDEFS
#include    <c2defs.h>
#include    <dmdefs.h>

extern  void        c2d_free_curve __(( C2_CURVE DUMMY0 )) ;
extern  C2_CURVE    c2d_line __(( PT2 DUMMY0 , PT2 DUMMY1 )) ;
extern  C2_CURVE    c2d_line_dir __(( PT2 DUMMY0 , PT2 DUMMY1 )) ;
extern  C2_CURVE    c2d_ray __(( PT2 DUMMY0 , REAL DUMMY1 )) ;
extern  C2_CURVE    c2d_arc __(( PT2 DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , INT DUMMY4 )) ;
extern  C2_CURVE    c2d_arc_3pts __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 )) ;
extern  C2_CURVE    c2d_arc_2pts_tan __(( PT2 DUMMY0 , PT2 DUMMY1 , 
            PT2 DUMMY2 )) ;
extern  C2_CURVE    c2d_arc_2pts_tan1 __(( PT2 DUMMY0 , PT2 DUMMY1 , 
            PT2 DUMMY2 )) ;
extern  C2_CURVE    c2d_arc_pts_bulge __(( PT2 DUMMY0 , PT2 DUMMY1 , 
            REAL DUMMY2 )) ;
extern  C2_CURVE    c2d_arc_ctr_2pts __(( PT2 DUMMY0 , PT2 DUMMY1 ,
            PT2 DUMMY2 , INT DUMMY3 )) ;
extern  C2_CURVE    c2d_arc_ctr_2tans __(( PT2 DUMMY0 , REAL DUMMY1 ,
            PT2 DUMMY2 , PT2 DUMMY3 )) ;
extern  C2_CURVE    c2d_arc_complement __(( C2_CURVE DUMMY0 )) ;
extern  C2_CURVE    c2d_circle __(( PT2 DUMMY0 , REAL DUMMY1 )) ;
extern  C2_CURVE    c2d_circle_ctr_pt __(( PT2 DUMMY0 , PT2 DUMMY1 )) ;
extern  C2_CURVE    c2d_circle_ctr_pt_dir __(( PT2 DUMMY0 , PT2 DUMMY1 , 
            BOOLEAN DUMMY3 )) ;
extern  C2_CURVE    c2d_pcurve_through __(( PT2* DUMMY0 , INT DUMMY1 )) ;
extern  C2_CURVE    c2d_pcurve_init_size __(( PT2 DUMMY0 , INT DUMMY1 )) ;
extern  C2_CURVE    c2d_pcurve_init __(( PT2 DUMMY0 )) ;
extern  C2_CURVE    c2d_pcurve_add_arc_2pts __(( C2_CURVE DUMMY0 ,
            PT2 DUMMY1 , PT2 DUMMY2 )) ;
extern  C2_CURVE    c2d_pcurve_add_arc_ctr_pt __(( C2_CURVE DUMMY0 ,
            PT2 DUMMY1 , PT2 DUMMY2 )) ;
extern  C2_CURVE    c2d_pcurve_add_arc_tan __(( C2_CURVE DUMMY0 , 
            PT2 DUMMY1 )) ;
extern  C2_CURVE    c2d_pcurve_add_arc_tan0 __(( C2_CURVE DUMMY0 , 
            PT2 DUMMY1 , PT2 DUMMY2 )) ;
extern  C2_CURVE    c2d_pcurve_add_line __(( C2_CURVE DUMMY0 , PT2 DUMMY1 )) ;
extern  C2_CURVE    c2d_pcurve_add_line_tan __(( C2_CURVE DUMMY0 ,
            PT2 DUMMY1 )) ;
extern  C2_CURVE    c2d_pcurve_remove_last __(( C2_CURVE DUMMY0 )) ;
extern  C2_CURVE    c2d_pcurve_close __(( C2_CURVE DUMMY0 )) ;
extern  C2_CURVE    c2d_pcurve_segment __(( C2_CURVE DUMMY0 , INT DUMMY1 )) ;
extern  DML_LIST    c2d_pcurve_smash __(( C2_CURVE DUMMY0 , 
            DML_LIST DUMMY1 )) ;
extern  C2_CURVE    c2d_pcurve_add_curve __(( C2_CURVE DUMMY0 , 
            C2_CURVE DUMMY1 , REAL DUMMY2 )) ;
extern  C2_CURVE    c2d_pcurve_curves __(( DML_LIST DUMMY0 , REAL DUMMY1 )) ;
extern  DML_LIST    c2d_pcurve_list __(( DML_LIST DUMMY0 , REAL DUMMY1 , 
            DML_LIST DUMMY2 )) ;
extern  C2_CURVE    c2d_copy __(( C2_CURVE DUMMY0 )) ;
extern  C2_CURVE    c2d_trim0 __(( C2_CURVE DUMMY0 , PARM DUMMY1 )) ;
extern  C2_CURVE    c2d_trim1 __(( C2_CURVE DUMMY0 , PARM DUMMY1 )) ;
extern  C2_CURVE    c2d_trim __(( C2_CURVE DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 )) ;
extern  C2_CURVE    c2d_trim_t0 __(( C2_CURVE DUMMY0 , REAL DUMMY1 )) ;
extern  C2_CURVE    c2d_trim_t1 __(( C2_CURVE DUMMY0 , REAL DUMMY1 )) ;
extern  C2_CURVE    c2d_trim_t __(( C2_CURVE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 )) ;
extern  INT         c2d_offset __(( C2_CURVE DUMMY0 , REAL DUMMY1 ,
            DML_LIST DUMMY2 )) ;
extern  INT         c2d_offset_through __(( C2_CURVE DUMMY0 , PT2 DUMMY1 ,
            DML_LIST DUMMY2 )) ;
extern  C2_CURVE    c2d_offset_curve __(( C2_CURVE DUMMY0 , REAL DUMMY1 )) ;
extern  C2_CURVE    c2d_offset_curve_through __(( C2_CURVE DUMMY0 , 
            PT2 DUMMY1 )) ;

#ifdef  SPLINE
extern  C2_CURVE    c2d_spline __(( PT2* DUMMY0 , INT DUMMY1 )) ;
extern  C2_CURVE    c2d_spline_knots __(( PT2* DUMMY0 , INT DUMMY1 ,
            C2_KNOT_OPTIONS DUMMY2 )) ;
extern  C2_CURVE    c2d_spline_tan __(( PT2* DUMMY0 , INT DUMMY1 ,
            C2_KNOT_OPTIONS DUMMY2 , PT2 DUMMY3 , C2_TAN_OPTIONS DUMMY4 ,
            PT2 DUMMY5 , C2_TAN_OPTIONS DUMMY6 )) ;
extern  C2_CURVE    c2d_spline_clsd __(( PT2* DUMMY0 , INT DUMMY1 ,
            C2_KNOT_OPTIONS DUMMY2 )) ;
extern  C2_CURVE    c2d_nurb __(( HPT2* DUMMY0 , INT DUMMY1 , 
            INT DUMMY2 , REAL* DUMMY3 )) ;
extern  C2_CURVE    c2d_nu_bspline __(( PT2* DUMMY0 , INT DUMMY1 , 
            INT DUMMY2 , REAL* DUMMY3 )) ;
extern  C2_CURVE    c2d_curve_to_spline __(( C2_CURVE DUMMY0 )) ;
extern  C2_CURVE    c2d_spline_approx __(( C2_CURVE DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 , INT DUMMY3 )) ;
extern  C2_CURVE    c2d_ellipse __(( PT2 DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 )) ;
extern  C2_CURVE    c2d_bezier_ctl_pts __(( PT2 DUMMY0 , PT2 DUMMY1 ,
            PT2 DUMMY2 , PT2 DUMMY3 )) ;
extern  C2_CURVE    c2d_bezier_dctl_hpts __(( HPT2* DUMMY0 , INT DUMMY1 )) ;
extern  C2_CURVE    c2d_bezier_2pts_2tans __(( PT2 DUMMY0 , PT2 DUMMY1 ,
            PT2 DUMMY2 , PT2 DUMMY3 )) ;
extern  C2_CURVE    c2d_bezier_through __(( PT2 DUMMY0 , PT2 DUMMY1 ,
            PT2 DUMMY2 , PT2 DUMMY3 )) ;
extern  C2_CURVE    c2d_bezier_through_opt __(( PT2 DUMMY0 , PT2 DUMMY1 ,
            PT2 DUMMY2 , PT2 DUMMY3 ,  C2_KNOT_OPTIONS DUMMY4 )) ;
extern  C2_CURVE    c2d_bezier_through_node __(( PT2 DUMMY0 , PT2 DUMMY1 ,
            PT2 DUMMY2 , PT2 DUMMY3 , REAL DUMMY4 ,  REAL DUMMY5 )) ;
extern  C2_CURVE c2d_spline_fit __(( C2_CURVE DUMMY0 , REAL DUMMY1 , 
            INT DUMMY2 )) ;

#endif  /* SPLINE */

extern  C2_CURVE    c2d_translate __(( C2_CURVE DUMMY0 , PT2 DUMMY1 )) ;
extern  C2_CURVE    c2d_rotate __(( C2_CURVE DUMMY0 , PT2 DUMMY1 ,
            REAL DUMMY2 )) ;
extern  C2_CURVE    c2d_rotate_cs __(( C2_CURVE DUMMY0 , PT2 DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 )) ;
extern  C2_CURVE    c2d_scale __(( C2_CURVE DUMMY0 , PT2 DUMMY1 , 
            REAL DUMMY2 )) ;
extern  C2_CURVE    c2d_mirror_line __(( C2_CURVE DUMMY0 , PT2* DUMMY1 )) ;
extern  C2_CURVE    c2d_mirror_dir __(( C2_CURVE DUMMY0 , PT2 DUMMY1 ,
            PT2 DUMMY2 )) ;
extern  C2_CURVE    c2d_transform __(( C2_CURVE DUMMY0 , 
            C2_TRANSFORM DUMMY1 )) ;
extern  C2_CURVE    c2d_polygon __(( PT2* DUMMY0 , INT DUMMY1 )) ;
extern  PT2*        c2d_point __(( REAL DUMMY0 , REAL DUMMY1 )) ;
extern  void        c2d_free_point __(( PT2* DUMMY0 )) ;
extern  C2_INT_REC  c2d_create_int_rec __(( void )) ;
extern  void        c2d_free_int_rec __(( C2_INT_REC DUMMY0 )) ;
extern  C2_CURVE    c2d_ellipse_to_nurb __(( C2_CURVE DUMMY0 )) ;
extern  C2_CURVE    c2d_ellipse_to_pcurve __(( PT2, REAL, REAL, REAL, 
            REAL, REAL )) ;

#endif /* C2DDEFS*/

