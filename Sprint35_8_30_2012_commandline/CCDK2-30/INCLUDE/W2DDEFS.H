/******************************* W2DDEFS.H *********************************/
/*************************** View prototypes *******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef W2DDEFS
#define W2DDEFS

#include <c2defs.h>
#include <wmdefs.h>

extern  void    w2d_set_textposition __(( PT2 DUMMY0 )) ;
extern  void    w2d_moveto __(( PT2 DUMMY0 )) ;
extern  void    w2d_lineto __(( PT2 DUMMY0 )) ;
extern  void    w2d_curve __(( C2_CURVE DUMMY0 , PARM DUMMY1 , PARM DUMMY2 )) ;
extern  void    w2d_curvelist __(( DML_LIST DUMMY0 )) ;
extern  void    w2d_point __(( PT2 DUMMY0 , REAL DUMMY1 )) ;
extern  VP_VIEWPORT w2d_select __(( C2_CURVE DUMMY0 , PT2 DUMMY1 ,
            PARM DUMMY2 , REAL* DUMMY3 )) ;
extern  void    w2d_poly __(( PT2* DUMMY0 , INT DUMMY1 )) ;
extern  void    w2d_curve_dir __(( C2_CURVE DUMMY0 , INT DUMMY1 )) ;
extern  void    w2d_curve_box __(( C2_CURVE DUMMY0 )) ;
extern  void    w2d_box __(( C2_BOX DUMMY0 )) ;
extern  void    w2d_line __(( PT2 DUMMY0 , PT2 DUMMY1 )) ;
extern  void    w2d_inf_line __(( PT2 DUMMY0 , REAL DUMMY1 )) ;
extern  void    w2d_circle __(( PT2 DUMMY0 , REAL DUMMY1 )) ;
extern  void    w2d_cross __(( PT2 DUMMY0 , REAL DUMMY1 )) ;
extern  void    w2d_arrow __(( PT2 DUMMY0 , PT2 DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 )) ;
#endif /* W2DDEFS */


