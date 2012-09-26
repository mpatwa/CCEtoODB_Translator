/******************************* VPMCRS.H **********************************/
/************************ Viewport definitions *****************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef VPMCRS
#define VPMCRS

#define     VP_VIEW_CTR_PT(V)           ((V)->view_ctr_pt)
#define     VP_VIEW_X_VEC(V)            ((V)->view_x_vec)
#define     VP_VIEW_Y_VEC(V)            ((V)->view_y_vec)

#define     VP_VIEWPORT_TRANSFORM(W)    ((W)->transform)
#define     VP_VIEWPORT_INVERSE(W)      ((W)->inverse)
#define     VP_VIEWPORT_GRAN(W)         ((W)->granularity)
#define     VP_VIEWPORT_VIEW(W)         (&((W)->view))
#define     VP_VIEWPORT_VIEW_CTR_PT(W)  (VP_VIEW_CTR_PT(VP_VIEWPORT_VIEW(W)))
#define     VP_VIEWPORT_VIEW_X_VEC(W)   (VP_VIEW_X_VEC(VP_VIEWPORT_VIEW(W)))
#define     VP_VIEWPORT_VIEW_Y_VEC(W)   (VP_VIEW_Y_VEC(VP_VIEWPORT_VIEW(W)))
#define     VP_VIEWPORT_SCREEN_BOX(W)   (&((W)->screen_box))
#define     VP_VIEWPORT_MIN_PT(W)       (C2_MIN_PT(VP_VIEWPORT_SCREEN_BOX(W)))
#define     VP_VIEWPORT_MAX_PT(W)       (C2_MAX_PT(VP_VIEWPORT_SCREEN_BOX(W)))
#define     VP_VIEWPORT_PX0(W)          (C2_MIN_X(VP_VIEWPORT_SCREEN_BOX(W)))
#define     VP_VIEWPORT_PY0(W)          (C2_MIN_Y(VP_VIEWPORT_SCREEN_BOX(W)))
#define     VP_VIEWPORT_PX1(W)          (C2_MAX_X(VP_VIEWPORT_SCREEN_BOX(W)))
#define     VP_VIEWPORT_PY1(W)          (C2_MAX_Y(VP_VIEWPORT_SCREEN_BOX(W)))
#define     VP_VIEWPORT_BD_COLOR(W)     ((W)->bd_color)
#define     VP_VIEWPORT_BK_COLOR(W)     ((W)->bk_color)
#define     VP_VIEWPORT_CLIPPED(W)      ((W)->viewport_clipped)
#define     VP_VIEWPORT_PREV_PT(VP)     ((VP)->prev_pt)
#define     VP_VIEWPORT_PREV_X(VP)      ((VP)->prev_pt[0])
#define     VP_VIEWPORT_PREV_Y(VP)      ((VP)->prev_pt[1])
#define     VP_VIEWPORT_PREV_Z(VP)      ((VP)->prev_pt[2])

#endif /* VPMCRS */

