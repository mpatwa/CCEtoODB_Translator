/******************************* VPDEFS.H **********************************/
/************************ Viewport definitions *****************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef VPDEFS
#define VPDEFS

#include <grdefs.h>
#include <c2defs.h>

typedef     REAL            VP_TRANSFORM[2][4] ;
typedef     REAL            VP_INVERSE[3][3] ;

typedef     struct          vp_view {
            REAL            view_ctr_pt[3] ; 
            REAL            view_x_vec[3] ;
            REAL            view_y_vec[3] ;
            } VP_VIEW_S ;
typedef     VP_VIEW_S   *VP_VIEW ;

typedef     C2_BOX_S        VP_SCREEN_BOX_S ;
typedef     C2_BOX          VP_SCREEN_BOX ;

typedef     struct          vp_viewport {
            VP_TRANSFORM    transform ;
            VP_INVERSE      inverse ;
            REAL            granularity ;
            VP_VIEW_S       view ;
            VP_SCREEN_BOX_S screen_box ;
            GR_COLOR        bd_color ;
            GR_COLOR        bk_color ;
            INT             viewport_clipped ;
            REAL            prev_pt[3] ;
            } VP_VIEWPORT_S ;

typedef     VP_VIEWPORT_S *VP_VIEWPORT ;

#endif /* VPDEFS */

