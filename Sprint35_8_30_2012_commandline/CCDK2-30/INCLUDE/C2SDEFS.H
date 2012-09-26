/******************************** C2SDEFS.C ********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifdef      SPLINE
#ifndef     C2SDEFS
#define     C2SDEFS
#include    <c2defs.h>
#include    <dmdefs.h>
#include    <alsdefs.h>

#define C2S_CONV_BEZ(A,D,KNOT,J,B) \
als_conv_bez((REAL*)((A)+(J)-(D)+1),(D),(KNOT)+(J)-(D)+1,3,(REAL*)(B))

extern  BOOLEAN c2s_interp_knots __(( PT2* DUMMY0 , INT DUMMY1 , 
            C2_KNOT_OPTIONS DUMMY2 , REAL* DUMMY3 )) ;
extern  BOOLEAN c2s_clsd_interp_knots __(( PT2* DUMMY0 , INT DUMMY1 ,
            C2_KNOT_OPTIONS DUMMY2 , REAL* DUMMY3 )) ;
extern  BOOLEAN c2s_interp_tan __(( PT2* DUMMY0 , INT DUMMY1 , 
            C2_KNOT_OPTIONS DUMMY2 , PT2 DUMMY3 , C2_TAN_OPTIONS DUMMY4 , 
            PT2 DUMMY5 , C2_TAN_OPTIONS DUMMY6 , PT2* DUMMY7 , 
            REAL* DUMMY8 )) ;
extern  BOOLEAN c2s_interp_clsd __(( PT2* DUMMY0 , INT DUMMY1 , 
            C2_KNOT_OPTIONS DUMMY2 , PT2* DUMMY3 , REAL* DUMMY4 )) ;
extern  void    c2s_pcurve __(( C2_PCURVE DUMMY0 , HPT2* DUMMY1 , 
            REAL* DUMMY2 )) ;
extern  BOOLEAN c2s_eval __(( HPT2* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 , REAL DUMMY4 ,  PARM DUMMY5 , INT DUMMY6 ,
            PT2* DUMMY7 )) ;
extern  BOOLEAN c2s_pt_tan __(( HPT2* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 , REAL DUMMY4 , PARM DUMMY5 , PT2 DUMMY6 ,
            PT2 DUMMY7 )) ;
extern  BOOLEAN c2s_project __(( HPT2* DUMMY0 , INT DUMMY1 , REAL* DUMMY2 ,
            REAL DUMMY3 , PARM DUMMY4 , PARM DUMMY5 ,  PT2 DUMMY6 ,
            PARM DUMMY7 , PT2 DUMMY8 )) ;
extern  BOOLEAN c2s_select __(( HPT2* DUMMY0 , INT DUMMY1 , REAL* DUMMY2 ,
            REAL DUMMY3 , PARM DUMMY4 , PARM DUMMY5 ,  PT2 DUMMY6 ,
            REAL DUMMY7 , PARM DUMMY8 , REAL* DUMMY9 )) ;
extern  void    c2s_mass_prop __(( HPT2* DUMMY0 , INT DUMMY1 , REAL* DUMMY2 ,
            REAL DUMMY3 , PARM DUMMY4 , PARM DUMMY5 ,  INT DUMMY6 ,
            INT DUMMY7 , REAL DUMMY8 , PF_PROP2 DUMMY9 , REAL* DUMMY10 )) ;
extern  BOOLEAN c2s_pt_at_dist __(( HPT2* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 , REAL DUMMY4 , PARM DUMMY5 ,  PARM DUMMY6 ,
            PARM DUMMY7 , REAL DUMMY8 , PARM DUMMY9 , REAL* DUMMY10 )) ;
extern  void    c2s_break __(( HPT2* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 , REAL DUMMY4 , PARM DUMMY5 ,  PARM DUMMY6 ,
            INT DUMMY7 , PARM* DUMMY8 , PT2* DUMMY9 )) ;
#ifdef NEW_CODE
extern  BOOLEAN c2s_inters_iter __(( HPT2* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 , REAL DUMMY4 , PARM DUMMY5 ,  HPT2* DUMMY6 ,
            INT DUMMY7 , INT DUMMY8 , REAL* DUMMY9 , REAL DUMMY10 ,
            PARM DUMMY11 ,  REAL DUMMY12 , PARM DUMMY13 , PARM DUMMY14 ,
            PT2 DUMMY15 )) ;
#endif
extern  BOOLEAN c2s_curv_w_root __(( HPT2* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 , REAL DUMMY4 , REAL DUMMY5 ,  PARM DUMMY6 ,
            PARM DUMMY7 , PARM DUMMY8 )) ;
extern  BOOLEAN c2s_curvature __(( HPT2* DUMMY0 , INT DUMMY1 , REAL* DUMMY2 ,
            REAL DUMMY3 , PARM DUMMY4 , REAL* DUMMY5 )) ;
extern  INT     c2s_curv_extrs __(( HPT2* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 , DML_LIST DUMMY4 )) ;
extern  INT     c2s_inters_line __(( HPT2* DUMMY0 , INT DUMMY1 , INT DUMMY2 , 
            REAL* DUMMY3 , REAL DUMMY4 ,  PARM DUMMY5 , PARM DUMMY6 , 
            C2_LINE DUMMY7 , DML_LIST DUMMY8 )) ;
extern  INT     c2s_inters_hor_line __(( HPT2* DUMMY0 , INT DUMMY1 , 
            INT DUMMY2 , REAL* DUMMY3 , REAL DUMMY4 ,  PARM DUMMY5 , 
            PARM DUMMY6 , PT2 DUMMY7 , INT DUMMY8 , DML_LIST DUMMY9 )) ;
extern  INT     c2s_inters_x_axis __(( HPT2* DUMMY0 , INT DUMMY1 , 
            REAL* DUMMY2 , REAL DUMMY3 ,  PARM DUMMY4 , PARM DUMMY5 , 
            INT DUMMY6 , DML_LIST DUMMY7 )) ;
extern  INT     c2s_inters_arc __(( HPT2* DUMMY0 , INT DUMMY1 , REAL* DUMMY2 ,
            REAL DUMMY3 ,  PARM DUMMY4 , PARM DUMMY5 , C2_ARC DUMMY6 ,
            DML_LIST DUMMY7 )) ;
extern  INT     c2s_inters_splines __(( HPT2* DUMMY0 , INT DUMMY1 ,
            INT DUMMY2 , REAL* DUMMY3 , REAL DUMMY4 ,  PARM DUMMY5 ,
            PARM DUMMY6 , HPT2* DUMMY7 , INT DUMMY8 , INT DUMMY9 ,
            REAL* DUMMY10 , REAL DUMMY11 ,  PARM DUMMY12 , PARM DUMMY13 ,
            DML_LIST DUMMY14 )) ;
extern  INT     c2s_self_inters __(( HPT2* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 , REAL DUMMY4 ,  PARM DUMMY5 , PARM DUMMY6 ,
            DML_LIST DUMMY7 )) ;
extern  BOOLEAN c2s_approx __(( HPT2* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 , REAL DUMMY4 , PARM DUMMY5 , PARM DUMMY6 ,
             REAL DUMMY7 , BOOLEAN DUMMY8 , PT2* DUMMY9 , PARM DUMMY10 ,
            INT DUMMY11 , INT* DUMMY12 , PARM DUMMY13 )) ;
extern  BOOLEAN c2s_approx_zoomed __(( HPT2* DUMMY0 , INT DUMMY1 ,
            INT DUMMY2 , REAL* DUMMY3 , REAL DUMMY4 , PARM DUMMY5 ,
            PARM DUMMY6 ,  REAL DUMMY7 , REAL DUMMY8 , REAL DUMMY9 ,
            REAL DUMMY10 , PT2* DUMMY11 , PARM DUMMY12 , INT DUMMY13 ,
            INT* DUMMY14 , PARM DUMMY15 )) ;
extern  void    c2s_info __(( HPT2* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 , REAL DUMMY4 , FILE* DUMMY5 )) ;
extern  void    c2s_info_ellipse __(( HPT2* DUMMY0 , REAL DUMMY1 ,
            FILE* DUMMY2 )) ;
extern  INT     c2s_coord_extrs __(( HPT2* DUMMY0 , INT DUMMY1 , REAL* DUMMY2 ,
            PARM DUMMY3 , PARM DUMMY4 , INT DUMMY5 ,  DML_LIST DUMMY6 )) ;
extern  INT     c2s_offset __(( HPT2* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 , REAL DUMMY4 , INT DUMMY5 , C2_CURV_EXTR DUMMY6 , 
            PARM DUMMY7 , PARM DUMMY8 ,REAL DUMMY9 , DML_LIST DUMMY10 )) ;
extern  INT     c2s_offset_polygon __(( HPT2* DUMMY0 , INT DUMMY1 ,
            REAL DUMMY2 , PT2* DUMMY3 )) ;
extern  BOOLEAN c2s_get_ellipse_data __(( HPT2* DUMMY0 , PT2 DUMMY1 , 
            REAL* DUMMY2 , REAL* DUMMY3 , REAL* DUMMY4 )) ;
extern  DML_LIST c2s_appr_arc __(( HPT2* DUMMY0 , INT DUMMY1 , INT DUMMY2 , 
            REAL* DUMMY3 , REAL DUMMY4 , C2_CURV_EXTR DUMMY5 , INT DUMMY6 ,
            PARM DUMMY7 , PARM DUMMY8 , INT DUMMY9 , REAL DUMMY10 , 
            DML_LIST DUMMY11 , DML_LIST DUMMY12 )) ;
extern  BOOLEAN c2s_get __(( FILE* DUMMY0 , HPT2* DUMMY1 , INT DUMMY2 , 
            INT DUMMY3 , REAL* DUMMY4 , REAL* DUMMY5 )) ;
extern  BOOLEAN  c2s_put __(( FILE* DUMMY0 , HPT2* DUMMY1 , INT DUMMY2 , 
            INT DUMMY3 , REAL* DUMMY4 , REAL DUMMY5 )) ;
extern  void    c2s_ellipse __(( PT2 DUMMY0 , REAL DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 , HPT2* DUMMY4 , REAL* DUMMY5 )) ;
extern  BOOLEAN c2s_coinc __(( HPT2* DUMMY0 , INT DUMMY1 , INT DUMMY2 , 
            REAL* DUMMY3 , REAL DUMMY4 , HPT2* DUMMY5 , INT DUMMY6 , 
            INT DUMMY7 , REAL* DUMMY8 , REAL DUMMY9 )) ;
extern  void    c2s_reverse __(( HPT2* DUMMY0 , INT DUMMY1 , INT DUMMY2 , 
            REAL* DUMMY3 , PARM DUMMY4 , PARM DUMMY5 )) ;
extern  HPT2*   c2s_clsd_ctlpts __(( HPT2* DUMMY0 , INT DUMMY1 , 
            INT DUMMY2 , REAL* DUMMY3 )) ;
#endif  /* C2SDEFS */
#endif  /* SPLINE */

