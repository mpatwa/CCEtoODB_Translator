/*************************** C2NDEFS.h **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!   (C) Copyright 1989 - 1995             Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifdef      SPLINE
#ifndef     C2NDEFS
#define     C2NDEFS
#include    <c2defs.h>

extern  C2_NURB     c2n_create_nurb __(( INT DUMMY0 , INT DUMMY1 )) ;
extern  void        c2n_set_ctpts __(( PT2* DUMMY0 , C2_NURB DUMMY1 )) ;
extern  void        c2n_set_cthpts __(( HPT2* DUMMY0 , C2_NURB DUMMY1 )) ;
extern  void        c2n_set_knots __(( REAL* DUMMY0 , C2_NURB DUMMY1 )) ;
extern  void        c2n_set_uniform_knots __(( C2_NURB DUMMY0 )) ;
extern  void        c2n_set_clsc_uni_knots __(( C2_NURB DUMMY0 )) ;
extern  void        c2n_init_parms __(( C2_NURB DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 )) ;
extern  C2_NURB     c2n_create_copy_nurb __(( C2_NURB DUMMY0 )) ;
extern  C2_NURB     c2n_trim __(( C2_NURB DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 )) ;
extern  void        c2n_free_nurb __(( C2_NURB DUMMY0 )) ;
extern  C2_CURV_EXTR c2n_create_curv_extr __(( REAL DUMMY0 , INT DUMMY1 ,
            REAL DUMMY2 )) ;
extern  void        c2n_free_curv_extr __(( C2_CURV_EXTR DUMMY0 )) ;
extern  INT         c2n_setup_curv_extr __(( C2_NURB DUMMY0 )) ;
extern  C2_NURB     c2n_line __(( C2_LINE DUMMY0 )) ;
extern  C2_NURB     c2n_arc __(( C2_ARC DUMMY0 , BOOLEAN DUMMY1 )) ;
extern  C2_NURB     c2n_pcurve __(( C2_PCURVE DUMMY0 )) ;
extern  BOOLEAN     c2n_closed __(( C2_NURB DUMMY0 )) ;
extern  void        c2n_copy_extr_rec __(( C2_NURB DUMMY0 , C2_NURB DUMMY2 )) ;
extern  void        c2n_box __(( C2_NURB DUMMY0 , C2_BOX DUMMY1 )) ;
extern  void        c2n_translate __(( C2_NURB DUMMY0 , PT2 DUMMY1 )) ; 
extern  void        c2n_scale __(( C2_NURB DUMMY0 , PT2 DUMMY1 , 
            REAL DUMMY2 )) ; 
extern  void        c2n_mirror __(( C2_NURB DUMMY0 , PT2 DUMMY1 , 
            PT2 DUMMY2 )) ; 
extern  void        c2n_rotate_cs __(( C2_NURB DUMMY0 , PT2 DUMMY2 , 
            REAL DUMMY2 , REAL DUMMY3 )) ; 
extern  BOOLEAN     c2n_transform __(( C2_NURB DUMMY0 , 
            C2_TRANSFORM DUMMY1 )) ; 
extern  HPT2*       c2n_get_ctpt __(( C2_NURB DUMMY0 )) ;
extern  INT         c2n_get_n __(( C2_NURB DUMMY0 )) ;
extern  REAL*       c2n_get_knot __(( C2_NURB DUMMY0 )) ;
extern  INT         c2n_get_d __(( C2_NURB DUMMY0 )) ;
extern  REAL        c2n_get_w __(( C2_NURB DUMMY0 )) ;
extern  void        c2n_set_w __(( C2_NURB DUMMY0 , REAL DUMMY1 )) ;
extern  BOOLEAN     c2n_parm_adjust __(( C2_NURB DUMMY0 , PARM DUMMY1 )) ;
extern  void        c2n_reverse __(( C2_NURB DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 )) ;
extern  C2_NURB     c2n_bezier_ctl_pts __(( PT2 DUMMY0 , PT2 DUMMY1 , 
            PT2 DUMMY2 , PT2 DUMMY3 )) ;
extern  C2_NURB     c2n_bezier_dctl_hpts __(( HPT2* DUMMY0 , INT DUMMY1 )) ;
extern  INT         c2n_offset __(( C2_NURB DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 , REAL DUMMY3 , DML_LIST DUMMY4 )) ;
extern  BOOLEAN c2n_trimmed __(( C2_NURB DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 )) ;
extern  BOOLEAN c2n_trimmed0 __(( C2_NURB DUMMY0 , PARM DUMMY1 )) ;
extern  BOOLEAN c2n_trimmed1 __(( C2_NURB DUMMY0 , PARM DUMMY1 )) ;
extern  BOOLEAN c2n_interp_tan __(( PT2* DUMMY0 , INT DUMMY1 , 
            C2_KNOT_OPTIONS DUMMY2 , PT2 DUMMY3 , C2_TAN_OPTIONS DUMMY4 , 
            PT2 DUMMY5 , C2_TAN_OPTIONS DUMMY6 , C2_NURB DUMMY7 )) ;
extern  BOOLEAN c2n_interp_clsd __(( PT2* DUMMY0 , INT DUMMY1 , 
            C2_KNOT_OPTIONS DUMMY2 , C2_NURB DUMMY3 )) ;
extern  C2_NURB c2n_get __(( FILE* DUMMY0 , PARM DUMMY1 , PARM DUMMY2 )) ;
extern  BOOLEAN c2n_put __(( FILE* DUMMY0 , C2_NURB DUMMY1 , PARM DUMMY2 , 
            PARM DUMMY3 )) ;
extern  BOOLEAN c2n_get_ellipse_data __(( C2_NURB DUMMY0 , PT2 DUMMY1 , 
            REAL* DUMMY2 , REAL* DUMMY3 , REAL* DUMMY4 )) ;
BBS_PRIVATE REAL c2n_ellipse_t __(( C2_NURB DUMMY0 , PARM DUMMY1 )) ;
extern  BOOLEAN c2n_approx __(( C2_NURB DUMMY0 , PARM DUMMY1 , PARM DUMMY2 ,
            PARM DUMMY3 , PARM DUMMY4 , REAL DUMMY5 , BOOLEAN DUMMY6 , 
            PT2* DUMMY7 , PARM DUMMY8 , INT DUMMY9 , INT* DUMMY10 , 
            PARM DUMMY11 )) ;
extern  BOOLEAN c2n_approx_zoomed __(( C2_NURB DUMMY0 , PARM DUMMY1 ,
            PARM DUMMY2 ,  PARM DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , 
            REAL DUMMY6 , REAL DUMMY7 , PT2* DUMMY8 , PARM DUMMY9 , 
            INT DUMMY10 , INT* DUMMY11 , PARM DUMMY12 )) ;
extern  BOOLEAN c2n_eval __(( C2_NURB DUMMY0 , PARM DUMMY1 ,
            INT DUMMY2 ,  PT2* DUMMY3 )) ;
extern  BOOLEAN c2n_pt_tan __(( C2_NURB DUMMY0 , PARM DUMMY1 ,
            PT2 DUMMY2 , PT2 DUMMY3 )) ;
extern  BOOLEAN c2n_curvature __(( C2_NURB DUMMY0 , PARM DUMMY1 ,
            REAL* DUMMY2 )) ;
extern  INT     c2n_self_inters __(( C2_NURB DUMMY0 , PARM DUMMY1 ,
            PARM DUMMY2 , DML_LIST DUMMY3 )) ;
extern  INT     c2n_inters_line __(( C2_NURB DUMMY0 , PARM DUMMY1 ,
            PARM DUMMY2 , C2_LINE DUMMY3 , DML_LIST DUMMY4 )) ;
extern  INT     c2n_inters_arc __(( C2_NURB DUMMY0 , PARM DUMMY1 ,
            PARM DUMMY2 , C2_ARC DUMMY3 , DML_LIST DUMMY4 )) ;
extern  INT c2n_inters_splines __(( C2_NURB DUMMY0 , PARM DUMMY1 ,
            PARM DUMMY2 , C2_NURB DUMMY3 , PARM DUMMY4 , PARM DUMMY5 , 
            DML_LIST DUMMY6 )) ;
extern  void c2n_compare_knots __(( C2_NURB DUMMY0 , REAL DUMMY1 , 
            REAL* DUMMY2 , INT DUMMY3 )) ;
extern  BOOLEAN c2n_project __(( C2_NURB DUMMY0 , PARM DUMMY1 ,
            PARM DUMMY2 , PT2 DUMMY3 , PARM DUMMY4 , PT2 DUMMY5 )) ;
extern  void    c2n_mass_prop __(( C2_NURB DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 ,  INT DUMMY3 , INT DUMMY4 , REAL DUMMY5 , 
            PF_PROP2 DUMMY6 , REAL* DUMMY7 )) ;
extern  void c2n_info __(( C2_NURB DUMMY0 , FILE* DUMMY1 )) ;
extern  void c2n_info_ellipse __(( C2_NURB DUMMY0 , FILE* DUMMY1 )) ;
extern  BOOLEAN c2n_select __(( C2_NURB DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 , PT2 DUMMY3 , REAL DUMMY4 , PARM DUMMY5 , 
            REAL* DUMMY6 )) ;
extern  INT c2n_coord_extrs __(( C2_NURB DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 , INT DUMMY3 , DML_LIST DUMMY4 )) ;
extern  DML_LIST c2n_appr_arc __(( C2_NURB DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 , INT DUMMY3 , REAL DUMMY4 , DML_LIST DUMMY5 , 
            DML_LIST DUMMY5 )) ;
extern  C2_NURB c2n_ellipse __(( PT2 DUMMY0 , REAL DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 )) ;
extern  HPT2* c2n_clsd_ctlpts __(( C2_NURB DUMMY0 )) ;
extern  void c2n_convert_clsd __(( C2_NURB DUMMY0 )) ;
#endif /* C2NDEFS*/ 
#endif /* SPLINE */

