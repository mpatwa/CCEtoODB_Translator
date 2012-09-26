/* -2 -3 */
/********************************* VP2.C ***********************************/
/********************** Display viewports management ***********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2adefs.h>
#include <grrdefs.h>
#include <vp2defs.h>
#include <v2ddefs.h>
#include <v2dpriv.h>
#include <vpidefs.h>
#include <c2vmcrs.h>
#include <vpmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vp2_create ( x, y, w, h, px0, py0, px1, py1, 
            bd_color, bk_color ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL x, y, w, h ;
REAL px0, py0, px1, py1 ;
GR_COLOR bd_color, bk_color ;
{
    REAL view_ctr_pt[3], view_x_vec[3], view_y_vec[3] ;
    view_ctr_pt[0] = x ;
    view_ctr_pt[1] = y ;
    view_ctr_pt[2] = 0.0 ;
    view_x_vec[0] = 0.5*w ;
    view_x_vec[1] = 0.0 ;
    view_x_vec[2] = 0.0 ;
    view_y_vec[0] = 0.0 ;
    view_y_vec[1] = 0.5*h ;
    view_y_vec[2] = 0.0 ;
    RETURN ( vpi_create ( view_ctr_pt, view_x_vec, view_y_vec, 
        px0, py0, px1, py1, bd_color, bk_color ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL* vp2_get_ctr ( vp, ctr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
PT2 ctr ;
{
    C2V_COPY ( VP_VIEWPORT_VIEW_CTR_PT(vp), ctr ) ;
    RETURN ( ctr ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vp2_set_ctr ( vp, ctr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
PT2 ctr ;
{
    C2V_COPY ( ctr, VP_VIEWPORT_VIEW_CTR_PT(vp) ) ;
    RETURN ( vpi_setup ( vp ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC C2_BOX vp2_get_view_box ( vp, view_box )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
C2_BOX view_box ;
{
    PT2 ctr ;
    REAL w, h ;
    vp2_get_ctr ( vp, ctr ) ;
    vpi_get_wh ( vp, &w, &h ) ;
    RETURN ( c2a_box_set_ctr_wh ( view_box, ctr, w, h ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vp2_set_view_box ( vp, view_box )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
C2_BOX view_box ;
{
    RETURN ( vp2_fit ( vp, view_box, 1.0 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vp2_fit ( vp, box, border_factor ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
C2_BOX box ;
REAL border_factor ;
{
    REAL a ;
    VP_VIEWPORT_VIEW_CTR_PT(vp)[0] = 
        0.5 * ( C2_MIN_X(box) + C2_MAX_X(box) ) ;
    VP_VIEWPORT_VIEW_CTR_PT(vp)[1] = 
        0.5 * ( C2_MIN_Y(box) + C2_MAX_Y(box) ) ;
    VP_VIEWPORT_VIEW_CTR_PT(vp)[2] = 0.0 ;
    a = fabs ( C2_MAX_X(box) - C2_MIN_X(box) ) ;
    if ( a < VP_VIEWPORT_GRAN(vp) ) 
        a = VP_VIEWPORT_GRAN(vp) ;
    VP_VIEWPORT_VIEW_X_VEC(vp)[0] = 0.5 * a * border_factor ;
    VP_VIEWPORT_VIEW_X_VEC(vp)[1] = 0.0 ;
    VP_VIEWPORT_VIEW_X_VEC(vp)[2] = 0.0 ;
    VP_VIEWPORT_VIEW_Y_VEC(vp)[0] = 0.0 ;
    a = fabs ( C2_MAX_Y(box) - C2_MIN_Y(box) ) ;
    if ( a < VP_VIEWPORT_GRAN(vp) ) 
        a = VP_VIEWPORT_GRAN(vp) ;
    VP_VIEWPORT_VIEW_Y_VEC(vp)[1] = 0.5 * a * border_factor ;
    VP_VIEWPORT_VIEW_Y_VEC(vp)[2] = 0.0 ;
    RETURN ( vpi_setup ( vp ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT* vp2_point_to_pixel ( pt, vp, pxl ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt ;
VP_VIEWPORT vp ;
INT pxl[2] ;
{
    PT2 a ;
    v2d_pt_transform ( pt, vp, a ) ;
    RETURN ( grr_point_to_pixel ( a, pxl ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL* vp2_pixel_to_point ( pxl, vp, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT pxl[2] ;
VP_VIEWPORT vp ;
PT2 pt ;
{
    PT2 a ;

    grr_pixel_to_point ( pxl, a ) ;
    pt[0] = VP_VIEWPORT_INVERSE(vp)[0][0] * a[0] + 
        VP_VIEWPORT_INVERSE(vp)[0][1] * a[1] + VP_VIEWPORT_INVERSE(vp)[0][2] ; 
    pt[1] = VP_VIEWPORT_INVERSE(vp)[1][0] * a[0] + 
        VP_VIEWPORT_INVERSE(vp)[1][1] * a[1] + VP_VIEWPORT_INVERSE(vp)[1][2] ;
    RETURN ( pt ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vp2_append_transform ( vp, t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
C2_TRANSFORM t ;
{
    INT i, j ;
    VP_TRANSFORM vt ;
    vpi_get_transform ( vp, vt ) ;

    for ( i=0 ; i<2 ; i++ ) {
        for ( j=0 ; j<2 ; j++ ) 
            VP_VIEWPORT_TRANSFORM(vp)[i][j] = 
                vt[i][0]*t[0][j] + vt[i][1]*t[1][j] /*+ vt[i][2]*t[2][j] */;
        VP_VIEWPORT_TRANSFORM(vp)[i][2] = 0.0 ;
        VP_VIEWPORT_TRANSFORM(vp)[i][3] = vt[i][0]*t[0][2] + 
            vt[i][1]*t[1][2] + /*vt[i][2]*t[2][2] + */ vt[i][3] ;
    }
    RETURN ( vp ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vp2_shift ( vp, shift ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
PT2 shift ;
{
    VP_VIEWPORT_TRANSFORM(vp)[0][3] += 
        ( VP_VIEWPORT_TRANSFORM(vp)[0][0] * shift[0] + 
            VP_VIEWPORT_TRANSFORM(vp)[0][1] * shift[1] ) ;
    VP_VIEWPORT_TRANSFORM(vp)[1][3] += 
        ( VP_VIEWPORT_TRANSFORM(vp)[1][0] * shift[0] + 
            VP_VIEWPORT_TRANSFORM(vp)[1][1] * shift[1] ) ;
    RETURN ( vp ) ;
}

