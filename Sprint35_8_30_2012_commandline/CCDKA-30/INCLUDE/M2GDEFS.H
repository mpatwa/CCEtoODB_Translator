/******************************** M2GDEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef     M2GDEFS
#define     M2GDEFS
#include <bbsdefs.h>
#include <dmdefs.h>
#include <c2defs.h>
#include <m2defs.h>

extern  INT     m2g_hor_ray_int_no __(( C2_ARC DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , PT2 DUMMY3 , BOOLEAN DUMMY4 )) ;
extern  INT     m2g_ray_int_no __(( C2_ARC DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
             PT2 DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , BOOLEAN DUMMY6 )) ;
extern  BOOLEAN m2g_x_max __(( C2_ARC DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL* DUMMY3 , REAL* DUMMY4 )) ;
extern  INT     m2g_xhatch_inters __(( C2_ARC DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , PT2 DUMMY3 ,  REAL DUMMY4 , REAL DUMMY5 ,
            REAL DUMMY6 , ANY DUMMY7 , DML_LIST* DUMMY8 , INT DUMMY9 )) ;
extern  REAL    m2g_area __(( C2_ARC DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , PT2 DUMMY3 )) ;
extern  INT     m2g_tan_angle __(( C2_ARC DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL* DUMMY4 )) ;
extern  void    m2g_set_ept0 __(( C2_ARC DUMMY0 , PT2 DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 )) ;
extern  void    m2g_set_ept1 __(( C2_ARC DUMMY0 , PT2 DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 )) ;
extern  REAL m2g_size __(( C2_ARC DUMMY0 , REAL DUMMY1 , REAL DUMMY2 )) ;
extern  BOOLEAN m2g_adjust_line_arc __(( C2_LINE DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , INT DUMMY3 , C2_ARC DUMMY4 , REAL DUMMY5 , 
            REAL DUMMY6, INT DUMMY7 )) ;
extern  BOOLEAN m2g_adjust_arc_line __(( C2_ARC DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , INT DUMMY3 , C2_LINE DUMMY4 , REAL DUMMY5 ,
            REAL DUMMY6, INT DUMMY7 )) ;
extern  BOOLEAN m2g_adjust_2arcs __(( C2_ARC DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , INT DUMMY3 , C2_ARC DUMMY4 , REAL DUMMY5 , 
            REAL DUMMY6, INT DUMMY7 )) ;
extern  BOOLEAN m2g_is_small __(( C2_ARC DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , REAL DUMMY3 )) ;
extern  INT     m2g_polygon __(( C2_ARC DUMMY0 , INT DUMMY1 , PT2* DUMMY2 )) ;
extern  INT     m2g_int_line_arc __(( C2_LINE DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , C2_ARC DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , 
            BOOLEAN DUMMY6 , REAL* DUMMY7 , REAL* DUMMY8 , PT2* DUMMY9 ,
            INT* DUMMY10 )) ;
extern  INT m2g_int_arc_arc __(( C2_ARC DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , C2_ARC DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , 
            BOOLEAN DUMMY6 , BOOLEAN DUMMY7 , REAL* DUMMY8 , REAL* DUMMY9 , 
            PT2* DUMMY10 , INT* DUMMY11 )) ;
extern  BOOLEAN m2g_furthest_pt __(( C2_ARC DUMMY0 , PT2 DUMMY1 , 
            REAL DUMMY2 , REAL DUMMY3 , PT2 DUMMY4 )) ;
extern  BOOLEAN m2g_increment __(( C2_ARC DUMMY0 , INT DUMMY1 , 
            REAL DUMMY2 , REAL DUMMY3 , REAL* DUMMY4 , 
            PT2 DUMMY5 , PARM DUMMY6 )) ;
extern  INT m2g_xpt_line_arc __(( C2_LINE DUMMY0 , C2_ARC DUMMY1 , 
            REAL* DUMMY2 , REAL* DUMMY3 )) ;
extern  INT m2g_xpt __(( C2_ARC DUMMY0 , C2_ARC DUMMY1 , 
            REAL* DUMMY2 , REAL* DUMMY3 )) ;
extern  INT m2g_xpt_vtx __(( C2_ARC DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 , 
            PT2 DUMMY3 , REAL* DUMMY4 )) ;
extern  INT m2g_approx_n __(( C2_ARC DUMMY0 , REAL DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 )) ;
extern  BOOLEAN m2g_along __(( C2_ARC DUMMY0 , REAL DUMMY1 , REAL DUMMY2 , 
            INT DUMMY3 , REAL* DUMMY4 )) ;
#endif /* M2GDEFS*/

