/***************************** C2BDEFS.H ***********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef C2BDEFS
#define C2BDEFS
#ifdef SPLINE
#include <c2defs.h>
#include <dmdefs.h>
#include <albdefs.h>
#include <c2coned.h>

#define C2BI_ANGLE_TOL      1e-5

#define C2B_BRK0(A,D,T,B)       alb_brk0((REAL*)A,D,3,T,(REAL*)B)
#define C2B_BRKL(A,D,T,B)       alb_brkl((REAL*)A,D,3,T,(REAL*)B)
#define C2B_BRKR(A,D,T,B)       alb_brkr((REAL*)A,D,3,T,(REAL*)B)
#define C2B_SBDV0(A,D,B)        alb_sbdv0((REAL*)A,D,3,(REAL*)B)
#define C2B_TRIM(A,D,T0,T1,B)   alb_trim((REAL*)A,D,3,T0,T1,(REAL*)B)

extern  BOOLEAN c2b_eval __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , INT DUMMY4 , PT2* DUMMY5 )) ;
extern  BOOLEAN c2b_eval_pt __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , PT2 DUMMY4 )) ;
extern  BOOLEAN c2b_eval_array __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            INT DUMMY3 , PT2* DUMMY4 )) ;
extern  void    c2b_dot __(( HPT2* DUMMY0 , INT DUMMY1 , HPT2* DUMMY2 ,
            INT DUMMY3 , HREAL* DUMMY4 )) ;
extern  void    c2b_cross __(( HPT2* DUMMY0 , INT DUMMY1 , HPT2* DUMMY2 ,
            INT DUMMY3 , HREAL* DUMMY4 )) ;
extern  void    c2b_dot_nonrat __(( PT2* DUMMY0 , INT DUMMY1 , PT2* DUMMY2 ,
            INT DUMMY3 , REAL* DUMMY4 )) ;
extern  void    c2b_deriv __(( HPT2* DUMMY0 , INT DUMMY1 , PT2* DUMMY2 )) ;
extern  void    c2b_cross_deriv __(( PT2* DUMMY0 , INT DUMMY1 , 
            REAL* DUMMY2 )) ;
extern  BOOLEAN c2b_project __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT2 DUMMY3 , REAL* DUMMY4 , PT2 DUMMY5 , REAL* DUMMY6 )) ;
extern  BOOLEAN c2b_ept0 __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT2 DUMMY3 )) ;
extern  BOOLEAN c2b_etan0 __(( HPT2* DUMMY0 , INT DUMMY1 , PT2 DUMMY2 )) ;
extern  BOOLEAN c2b_unit_normal0 __(( HPT2* DUMMY0 , INT DUMMY1 , 
            PT2 DUMMY2 )) ;
extern  BOOLEAN c2b_ept_tan0 __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT2 DUMMY3 , PT2 DUMMY4 )) ;
extern  BOOLEAN c2b_ept1 __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT2 DUMMY3 )) ;
extern  BOOLEAN c2b_etan1 __(( HPT2* DUMMY0 , INT DUMMY1 , PT2 DUMMY2 )) ;
extern  BOOLEAN c2b_unit_normal1 __(( HPT2* DUMMY0 , INT DUMMY1 , 
            PT2 DUMMY2 )) ;
extern  BOOLEAN c2b_ept_tan1 __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT2 DUMMY3 , PT2 DUMMY4 )) ;
extern  void    c2b_brk0 __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            HPT2* DUMMY3 )) ;
extern  void    c2b_brkl __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            HPT2* DUMMY3 )) ;
extern  void    c2b_brkr __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            HPT2* DUMMY3 )) ;
extern  void    c2b_sbdv0 __(( HPT2* DUMMY0 , INT DUMMY1 , HPT2* DUMMY2 )) ;
extern  void    c2b_trim __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 , HPT2* DUMMY4 )) ;
extern  REAL    c2b_flat __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 )) ;
extern  REAL    c2b_flat_epts __(( HPT2* DUMMY0 , INT DUMMY1 , PT2 DUMMY2 ,
            PT2 DUMMY3 , INT* DUMMY4 )) ;
extern  REAL    c2b_flat_non_rat __(( PT2* DUMMY0 , INT DUMMY1 )) ;
extern  BOOLEAN c2b_curvature __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL* DUMMY4 )) ;
extern  BOOLEAN c2b_select __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT2 DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , REAL DUMMY6 ,
             REAL* DUMMY7 , REAL* DUMMY8 )) ;
extern  BOOLEAN c2b_pt_in_box __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT2 DUMMY3 , REAL DUMMY4 )) ;
extern  BOOLEAN c2b_overlap_box __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 ,  REAL DUMMY4 , REAL DUMMY5 , REAL DUMMY6 ,
            REAL DUMMY7 )) ;
extern  BOOLEAN c2b_project_init __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT2 DUMMY3 , REAL* DUMMY4 )) ;
extern  BOOLEAN c2b_project_iter __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT2 DUMMY3 , REAL* DUMMY4 )) ;
extern  BOOLEAN c2b_int_coord_line_init __(( HPT2* DUMMY0 , INT DUMMY1 ,
            REAL DUMMY2 , INT DUMMY3 , REAL DUMMY4 , REAL* DUMMY5 )) ;
extern  BOOLEAN c2b_int_coord_line_iter __(( HPT2* DUMMY0 , INT DUMMY1 ,
            REAL DUMMY2 , INT DUMMY3 , REAL DUMMY4 , REAL* DUMMY5 )) ;
extern  INT     c2b_inters_coord_line __(( HPT2* DUMMY0 , INT DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 , REAL DUMMY4 , INT DUMMY5 ,
            REAL DUMMY6 , INT DUMMY7 , INT DUMMY8 , DML_LIST DUMMY9 )) ;
extern  INT     c2b_coord_extr_init __(( HPT2* DUMMY0 , INT DUMMY1 ,
            REAL DUMMY2 , INT DUMMY3 , INT DUMMY4 , REAL DUMMY5 , 
            REAL* DUMMY6 )) ;
extern  BOOLEAN c2b_inters_iter __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT2 DUMMY3 , PT2 DUMMY4 , REAL DUMMY5 ,  HPT2* DUMMY6 ,
            INT DUMMY7 , REAL DUMMY8 , PT2 DUMMY9 , PT2 DUMMY10 ,
            REAL DUMMY11 , REAL* DUMMY12 , REAL* DUMMY13 , REAL* DUMMY14 ,
            PT2 DUMMY15 )) ;
extern  BOOLEAN c2b_inters_init __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , HPT2* DUMMY5 , INT DUMMY6 ,
             REAL DUMMY7 , REAL DUMMY8 , REAL DUMMY9 , REAL* DUMMY10 ,
            REAL* DUMMY11 )) ;
extern  INT     c2b_inters_tan __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , INT DUMMY5 ,  INT DUMMY6 ,
            REAL DUMMY7 , C2_CONE DUMMY8 , PT2 DUMMY9 , PT2 DUMMY10 ,
            HPT2* DUMMY11 , INT DUMMY12 , REAL DUMMY13 , REAL DUMMY14 ,
            REAL DUMMY15 , INT DUMMY16 , INT DUMMY17 , REAL DUMMY18 ,
            C2_CONE DUMMY19 , PT2 DUMMY20 , PT2 DUMMY21 , DML_LIST DUMMY22 )) ;
extern  BOOLEAN c2b_sbdv_bez __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , INT DUMMY5 , PT2 DUMMY6 ,
            PT2 DUMMY7 ,  HPT2** DUMMY8 , HPT2** DUMMY9 , REAL* DUMMY10 ,
            INT* DUMMY11 , INT* DUMMY12 , PT2 DUMMY13 , REAL* DUMMY14 ,
            REAL* DUMMY15 , C2_CONE DUMMY16 , C2_CONE DUMMY17 )) ;
extern  BOOLEAN c2b_curv_extrs __(( HPT2* DUMMY0 , INT DUMMY1 , 
            DML_LIST DUMMY2 )) ;
extern  BOOLEAN c2b_curv_extrs_rat __(( HPT2* DUMMY0 , INT DUMMY1 , 
            REAL DUMMY2 , REAL DUMMY3 , INT DUMMY4 , INT* DUMMY5 , 
            DML_LIST DUMMY6 )) ;
extern  BOOLEAN c2b_curv_extrs_nonrat __(( HPT2* DUMMY0 , INT DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 , INT DUMMY4 ,  INT* DUMMY5 ,
            DML_LIST DUMMY6 )) ;
extern  INT     c2b_intersect __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , INT DUMMY5 ,  HPT2* DUMMY6 ,
            INT DUMMY7 , REAL DUMMY8 , REAL DUMMY9 , REAL DUMMY10 ,
            INT DUMMY11 , DML_LIST DUMMY12 )) ;
extern  INT     c2b_self_intersect __(( HPT2* DUMMY0 , INT DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 , REAL DUMMY4 , INT DUMMY5 ,
             DML_LIST DUMMY6 )) ;
extern  BOOLEAN c2b_approx __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , INT DUMMY5 , REAL DUMMY6 ,
            C2_BOX DUMMY7 , BOOLEAN DUMMY8 , BOOLEAN DUMMY9 , BOOLEAN DUMMY10 ,
            PT2* DUMMY11 , PARM DUMMY12 , INT DUMMY13 , INT* DUMMY14 ,
            REAL* DUMMY15 )) ;
extern  INT         c2b_append_inters_rec __(( DML_LIST DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 , INT DUMMY4 ,  REAL DUMMY5 ,
            REAL DUMMY6 , REAL DUMMY7 , INT DUMMY8 , PT2 DUMMY9 ,
            REAL DUMMY10 , INT DUMMY11 )) ;
extern  void        c2b_mass_prop __(( HPT2* DUMMY0 , INT DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 , REAL DUMMY4 , INT DUMMY5 ,
            INT DUMMY6 ,  REAL DUMMY7 , PF_PROP2 DUMMY8 , REAL* DUMMY9 )) ;
extern  void    c2b_appr_arc __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , INT DUMMY5 , INT DUMMY6 ,
            REAL DUMMY7 , DML_LIST DUMMY8 , DML_LIST DUMMY9 )) ;
extern  REAL    c2b_arc_deviation __(( HPT2* DUMMY0 , INT DUMMY1 , 
            REAL DUMMY2 , PT2 DUMMY3 , REAL DUMMY4 )) ;
extern  INT     c2b_mult0 __(( HPT2* DUMMY0 , INT DUMMY1 , INT DUMMY2 )) ;
extern  INT     c2b_mult1 __(( HPT2* DUMMY0 , INT DUMMY1 , INT DUMMY2 )) ;
extern  BOOLEAN c2b_max_dist __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            HPT2* DUMMY3 , INT DUMMY4 , REAL DUMMY5 , REAL DUMMY6 , 
            INT DUMMY7 , REAL* DUMMY8 , REAL* DUMMY9 )) ;
extern  INT     c2b_coinc __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            HPT2* DUMMY3 , INT DUMMY4 , REAL DUMMY5 )) ;
#endif /* SPLINE */
#endif /* C2BDEFS */

