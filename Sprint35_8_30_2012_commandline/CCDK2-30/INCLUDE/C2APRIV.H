/******************************** C2APRIV.H *****************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!   (C) Copyright 1989 - 1995             Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef C2APRIV
#define C2APRIV
#include <c2defs.h>
#include <c2coned.h>
extern  void    c2a_print_pt __(( FILE* DUMMY0 , PT2 DUMMY1 )) ;
extern  void    c2a_get_pt __(( PT2 DUMMY0 )) ;
#ifdef  SPLINE
extern  BOOLEAN c2a_non_rational __(( HPT2* DUMMY0 , INT DUMMY1 )) ;
extern  C2_BOX  c2a_box_hpoly __(( HPT2* DUMMY0 , INT DUMMY1 , 
            C2_BOX DUMMY2 )) ;
extern  REAL    c2a_flatness __(( PT2 DUMMY0 , PT2 DUMMY1 , C2_CONE DUMMY2 )) ;
extern  REAL    c2a_flat_epts_cone __(( PT2 DUMMY0 , PT2 DUMMY1 , INT* DUMMY2 ,
            C2_CONE DUMMY3 )) ;
extern  void    c2a_dcone __(( HPT2* DUMMY0 , INT DUMMY1 , INT* DUMMY2 ,
            C2_CONE DUMMY3 )) ;
extern  BOOLEAN c2a_dir_vec_cone __(( C2_CONE DUMMY0 , PT2 DUMMY1 )) ;
extern  BOOLEAN c2a_cone_w_inside __(( C2_CONE DUMMY0 , HPT2 DUMMY1 ,
            REAL DUMMY2 )) ;
extern  BOOLEAN c2a_cone_overlap __(( C2_CONE DUMMY0 , C2_CONE DUMMY1 )) ;
extern  BOOLEAN c2a_parlgrm_w_inside __(( PT2 DUMMY0 , PT2 DUMMY1 ,
            C2_CONE DUMMY2 , PT2 DUMMY3 , REAL DUMMY4 )) ;
extern  BOOLEAN c2a_parlgrm_overlap __(( PT2 DUMMY0 , PT2 DUMMY1 ,
            INT DUMMY2 , C2_CONE DUMMY3 ,  PT2 DUMMY4 , PT2 DUMMY5 ,
            INT DUMMY6 , C2_CONE DUMMY7 )) ;
extern  BOOLEAN c2a_parlgrm_separated __(( PT2 DUMMY0 , PT2 DUMMY1 ,
            INT DUMMY2 , C2_CONE DUMMY3 ,  PT2 DUMMY4 , PT2 DUMMY5 ,
            INT DUMMY6 , C2_CONE DUMMY7 , PT2 DUMMY8 )) ;
extern  BOOLEAN c2a_pt_in_polygon __(( PT2* DUMMY0 , INT DUMMY1 , 
            PT2 DUMMY2 )) ;
extern  BOOLEAN c2a_pt_in_triangle __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 ,
            PT2 DUMMY3 )) ;
extern  BOOLEAN c2a_inters_lines __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 ,
            PT2 DUMMY3 )) ;
extern  void    c2a_sub __(( HPT2* DUMMY0 , INT DUMMY1 , PT2 DUMMY2 ,
            HPT2* DUMMY3 )) ;
extern  void    c2a_pt_hpt __(( PT2* DUMMY0 , INT DUMMY1 , HPT2* DUMMY2 )) ;
extern  void    c2a_rotate_hpt_cs __(( HPT2* DUMMY0 , INT DUMMY1 ,
            PT2 DUMMY2 , REAL DUMMY3 , REAL DUMMY4 , HPT2* DUMMY5 )) ;
extern  void    c2a_mirror_hpt __(( HPT2* DUMMY0 , INT DUMMY1 , PT2 DUMMY2 ,
            PT2 DUMMY3 , HPT2* DUMMY4 )) ;
extern  BOOLEAN c2a_offset __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 ,
            REAL DUMMY3 , PT2 DUMMY4 )) ;
extern  void    c2a_print_hpt __(( FILE* DUMMY0 , HPT2 DUMMY1 )) ;
extern  void    c2a_transform_hpt __(( HPT2* DUMMY0 , INT DUMMY1 ,
            C2_TRANSFORM DUMMY2 , HPT2* DUMMY3 )) ;
#endif  /* SPLINE */
#endif  /* C2APRIV */

