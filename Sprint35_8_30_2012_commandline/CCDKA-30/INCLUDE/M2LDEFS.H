/******************************** M2LDEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!   (C) Copyright 1989 - 1995             Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef     M2LDEFS
#define     M2LDEFS
#include    <bbsdefs.h>
#include    <dmdefs.h>
#include    <c2defs.h>
#include    <m2defs.h>

extern  INT     m2l_hor_ray_int_no __(( C2_LINE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , PT2 DUMMY3 , BOOLEAN DUMMY4 )) ;
extern  INT     m2l_ray_int_no __(( C2_LINE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , PT2 DUMMY3 ,  REAL DUMMY4 , REAL DUMMY5 ,
            BOOLEAN DUMMY6 )) ;
extern  BOOLEAN m2l_x_max __(( C2_LINE DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL* DUMMY3 , REAL* DUMMY4 )) ;
extern  INT     m2l_xhatch_inters __(( C2_LINE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , PT2 DUMMY3 ,  REAL DUMMY4 , REAL DUMMY5 ,
            REAL DUMMY6 , ANY DUMMY7 , DML_LIST* DUMMY8 , INT DUMMY9 )) ;
extern  REAL    m2l_area __(( C2_LINE DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , PT2 DUMMY3 )) ;
extern  INT     m2l_tan_angle __(( C2_LINE DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , REAL DUMMY3 , REAL* DUMMY4 )) ;
extern  void    m2l_set_ept0 __(( C2_LINE DUMMY0 , PT2 DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 )) ;
extern  void    m2l_set_ept1 __(( C2_LINE DUMMY0 , PT2 DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 )) ;
extern  void    m2l_set __(( C2_LINE DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 )) ;
extern  REAL m2l_size __(( C2_LINE DUMMY0 , REAL DUMMY1 , REAL DUMMY2 )) ;
extern BOOLEAN m2l_join_lines __(( C2_LINE DUMMY0 , REAL DUMMY1 , REAL DUMMY2, 
            C2_LINE DUMMY3 , REAL DUMMY4 , REAL DUMMY5 )) ;
extern  BOOLEAN m2l_adjust_2lines __(( C2_LINE DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , C2_LINE DUMMY3 , REAL DUMMY4 , REAL DUMMY5 )) ;
extern  BOOLEAN m2l_is_small __(( C2_LINE DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , REAL DUMMY3 )) ;
extern  INT     m2l_intersect __(( C2_LINE DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , C2_LINE DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , 
            BOOLEAN DUMMY6 , BOOLEAN DUMMY7 , REAL* DUMMY8 , REAL* DUMMY9 , 
            PT2* DUMMY10 , INT* DUMMY11 )) ;
extern  BOOLEAN m2l_furthest_pt __(( C2_LINE DUMMY0 , PT2 DUMMY1 , 
            REAL DUMMY2 , REAL DUMMY3 , PT2 DUMMY4 )) ;
extern  BOOLEAN m2l_increment __(( C2_LINE DUMMY0 , INT DUMMY1 , 
            REAL DUMMY2 , REAL DUMMY3 , REAL* DUMMY4 , 
            PT2 DUMMY5 , PARM DUMMY6 )) ;
extern  BOOLEAN m2l_horline __(( C2_LINE DUMMY0 )) ;
extern  INT     m2l_xpt_vtx __(( C2_LINE DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 , 
            PT2 DUMMY3 , REAL* DUMMY4 )) ;
extern  REAL    m2l_along __(( C2_LINE DUMMY0 , REAL DUMMY1 , REAL DUMMY2 , 
            INT DUMMY3 )) ;
#endif /* M2LDEFS*/

