/******************************* VP2DEFS.H *********************************/
/*************************** View prototypes *******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!   (C) Copyright 1989 - 1995             Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef VP2DEFS
#define VP2DEFS

#include <vpdefs.h>
#include <c2defs.h>

extern  VP_VIEWPORT vp2_create __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 ,  REAL DUMMY4 , REAL DUMMY5 , REAL DUMMY6 ,
            REAL DUMMY7 , GR_COLOR DUMMY8 , GR_COLOR DUMMY9 )) ;
extern  REAL*       vp2_get_ctr __(( VP_VIEWPORT DUMMY0 , PT2 DUMMY1 )) ;
extern  VP_VIEWPORT vp2_set_ctr __(( VP_VIEWPORT DUMMY0 , PT2 DUMMY1 )) ;
extern  C2_BOX      vp2_get_view_box __(( VP_VIEWPORT DUMMY0 , 
            C2_BOX DUMMY1 )) ;
extern  VP_VIEWPORT vp2_set_view_box __(( VP_VIEWPORT DUMMY0 , 
            C2_BOX DUMMY1 )) ;
extern  VP_VIEWPORT vp2_fit __(( VP_VIEWPORT DUMMY0 , C2_BOX DUMMY1 ,
            REAL DUMMY2 )) ;
extern  INT*        vp2_point_to_pixel __(( PT2 DUMMY0 , VP_VIEWPORT DUMMY1 ,
            INT DUMMY2 [2] )) ;
extern  REAL*       vp2_pixel_to_point __(( INT DUMMY0 [2], 
            VP_VIEWPORT DUMMY1 , PT2 DUMMY2 )) ;
extern  VP_VIEWPORT vp2_append_transform __(( VP_VIEWPORT DUMMY0 ,
            C2_TRANSFORM DUMMY1 )) ;
extern  VP_VIEWPORT vp2_shift __(( VP_VIEWPORT DUMMY0 , PT2 DUMMY1 )) ;
#endif /* VP2DEFS */


