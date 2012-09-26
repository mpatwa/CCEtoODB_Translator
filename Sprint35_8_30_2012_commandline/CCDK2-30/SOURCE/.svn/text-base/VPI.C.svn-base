/* -2 -3 */
/********************************* VPI.C ***********************************/
/********************** Display viewports management ***********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <aladefs.h>
#include <c2adefs.h>
#include <grldefs.h>
#include <grrdefs.h>
#include <vpidefs.h>
#include <vpmcrs.h>

#define     NORM(A)     (sqrt((A)[0]*(A)[0]+(A)[1]*(A)[1]+(A)[2]*(A)[2]))
#define     DOT(A,B)    ((A)[0]*(B)[0]+(A)[1]*(B)[1]+(A)[2]*(B)[2])
STATIC  BOOLEAN vpi_transform __(( VP_VIEW, C2_BOX, VP_TRANSFORM, 
            VP_INVERSE )) ;
STATIC  REAL    vpi_gran __(( VP_TRANSFORM )) ;

/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vpi_create ( view_ctr_pt, view_x_vec, view_y_vec, 
    px0, py0, px1, py1, bd_color, bk_color ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL view_ctr_pt[3], view_x_vec[3], view_y_vec[3], px0, py0, px1, py1 ;
GR_COLOR bd_color, bk_color ;
{
    VP_VIEWPORT vp ;
    INT i ;

    vp = MALLOC ( 1, VP_VIEWPORT_S ) ;
    if ( vp==NULL ) 
        RETURN ( NULL ) ;

    for ( i=0 ; i<3 ; i++ ) {
        VP_VIEWPORT_VIEW_CTR_PT(vp)[i] = view_ctr_pt[i] ;
        VP_VIEWPORT_VIEW_X_VEC(vp)[i]  = view_x_vec[i] ;
        VP_VIEWPORT_VIEW_Y_VEC(vp)[i]  = view_y_vec[i] ;
    }
    VP_VIEWPORT_PX0(vp) = px0 ;
    VP_VIEWPORT_PY0(vp) = py0 ;
    VP_VIEWPORT_PX1(vp) = px1 ;
    VP_VIEWPORT_PY1(vp) = py1 ;
    VP_VIEWPORT_BD_COLOR(vp) = bd_color ;
    VP_VIEWPORT_BK_COLOR(vp) = bk_color ;
    VP_VIEWPORT_CLIPPED(vp) = 0 ;
    RETURN ( vpi_setup ( vp ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE VP_VIEWPORT vpi_setup ( vp ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
{
    REAL a ;

    if ( VP_VIEWPORT_PX0(vp) > VP_VIEWPORT_PX1(vp) ) {
        a = VP_VIEWPORT_PX0(vp) ;
        VP_VIEWPORT_PX0(vp) = VP_VIEWPORT_PX1(vp) ;
        VP_VIEWPORT_PX1(vp) = a ;
    }
    if ( VP_VIEWPORT_PY0(vp) > VP_VIEWPORT_PY1(vp) ) {
        a = VP_VIEWPORT_PY0(vp) ;
        VP_VIEWPORT_PY0(vp) = VP_VIEWPORT_PY1(vp) ;
        VP_VIEWPORT_PY1(vp) = a ;
    }

    VP_VIEWPORT_PREV_X(vp) = VP_VIEWPORT_VIEW_CTR_PT(vp)[0] ;
    VP_VIEWPORT_PREV_Y(vp) = VP_VIEWPORT_VIEW_CTR_PT(vp)[1] ;
    VP_VIEWPORT_PREV_Z(vp) = VP_VIEWPORT_VIEW_CTR_PT(vp)[2] ;
    if ( !vpi_transform ( 
        VP_VIEWPORT_VIEW(vp), VP_VIEWPORT_SCREEN_BOX(vp), 
        VP_VIEWPORT_TRANSFORM(vp), VP_VIEWPORT_INVERSE(vp) ) )
        RETURN ( NULL ) ;
    VP_VIEWPORT_GRAN(vp) = vpi_gran ( VP_VIEWPORT_TRANSFORM(vp) ) ;
    RETURN ( vp ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void vpi_free ( vp ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
{
    FREE ( vp ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vpi_clip ( vp ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
{
    if ( !VP_VIEWPORT_CLIPPED(vp) )
    { 
        grr_clip_inside ( VP_VIEWPORT_MIN_PT(vp), VP_VIEWPORT_MAX_PT(vp) ) ;
    	VP_VIEWPORT_CLIPPED(vp)++ ;
	}

    RETURN ( vp ) ;
} 


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vpi_unclip ( vp ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
{
    if ( VP_VIEWPORT_CLIPPED(vp) != 0 )
	{
    	grl_unclip();
    	VP_VIEWPORT_CLIPPED(vp)-- ;
	}

    RETURN ( vp ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void vpi_show ( vp ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
{
    vpi_fill ( vp ) ;
    vpi_border ( vp ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void vpi_fill ( vp ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
{
    GR_COLOR prev_color=grl_get_color() ;
	GR_PATT prev_patt=grl_get_patt();
    grl_set_color ( VP_VIEWPORT_BK_COLOR(vp) ) ;
    grl_set_patt ( GR_SOLID_PATT ) ;
    grr_fill_box ( VP_VIEWPORT_MIN_PT(vp), VP_VIEWPORT_MAX_PT(vp) ) ;
    grl_set_color ( prev_color ) ;
	grl_set_patt ( prev_patt );
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void vpi_border ( vp ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
{
    GR_COLOR prev_color=grl_get_color() ;
	GR_PATT prev_patt=grl_get_patt();
    grl_set_color ( VP_VIEWPORT_BD_COLOR(vp) ) ;
    grl_set_patt ( GR_SOLID_PATT ) ;
    grr_box ( VP_VIEWPORT_MIN_PT(vp), VP_VIEWPORT_MAX_PT(vp) ) ;
    grl_set_color ( prev_color ) ;
	grl_set_patt ( prev_patt );
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void vpi_unshow ( vp, scr_color ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
GR_COLOR scr_color ;
{
    GR_COLOR prev_color=grl_get_color() ;
    vpi_unclip ( vp ) ;
    grl_set_color ( scr_color ) ;
    grr_fill_box ( VP_VIEWPORT_MIN_PT(vp), VP_VIEWPORT_MAX_PT(vp) ) ;
    grl_set_color ( prev_color ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN vpi_transform ( view, screen_box, transform, inverse ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEW view ;
C2_BOX screen_box ;
VP_TRANSFORM transform ;
VP_INVERSE inverse ;
{
    REAL dx, dy, rx, ry, mx, my, cx, cy, xx, xy, yy, q ; 
    INT i ;

    mx = grr_get_max_x() ;
    my = grr_get_max_y() ;
    if ( IS_SMALL(mx) || IS_SMALL(my) ) 
        RETURN ( FALSE ) ;
    dx = 0.5 * ( C2_MAX_X(screen_box) - C2_MIN_X(screen_box) ) - 2.0 / mx ;
    dy = 0.5 * ( C2_MAX_Y(screen_box) - C2_MIN_Y(screen_box) ) - 2.0 / my ;
    xx = DOT ( VP_VIEW_X_VEC(view), VP_VIEW_X_VEC(view) ) ;
    xy = DOT ( VP_VIEW_X_VEC(view), VP_VIEW_Y_VEC(view) ) ;
    yy = DOT ( VP_VIEW_Y_VEC(view), VP_VIEW_Y_VEC(view) ) ;

    mx = sqrt ( xx ) / dx ;
    my = sqrt ( yy ) * grl_get_screen_ratio () / dy ; 

    if ( mx > my ) {
        my /= mx ;
        mx = 1.0 ;
    }
    else {
        mx /= my ;
        my = 1.0 ;
    }
    q = xx * yy - xy * xy ;
    cx = DOT ( VP_VIEW_CTR_PT(view), VP_VIEW_X_VEC(view) ) ;
    cy = DOT ( VP_VIEW_CTR_PT(view), VP_VIEW_Y_VEC(view) ) ;
    for ( i=0 ; i<3 ; i++ ) {
        transform[0][i] = mx * dx * ( VP_VIEW_X_VEC(view)[i] * yy - 
            VP_VIEW_Y_VEC(view)[i] * xy ) / q ;
        transform[1][i] = -my * dy * ( VP_VIEW_Y_VEC(view)[i] * xx - 
            VP_VIEW_X_VEC(view)[i] * xy ) / q ;
    }
    transform[0][3] = 0.5 * ( C2_MAX_X(screen_box) - C2_MIN_X(screen_box) ) - 
        dx * mx * ( cx * yy - cy * xy ) / q ;
    transform[1][3] = 0.5 * ( C2_MAX_Y(screen_box) - C2_MIN_Y(screen_box) ) + 
        my * dy * ( cy * xx - cx * xy ) / q ; 

    xx = 0.5 * ( C2_MIN_X(screen_box) + C2_MAX_X(screen_box) ) ;
    yy = 0.5 * ( C2_MIN_Y(screen_box) + C2_MAX_Y(screen_box) ) ;

    rx = mx * dx ;
    ry = - my * dy ;

    for ( i=0 ; i<3 ; i++ ) {
        inverse[i][0] = VP_VIEW_X_VEC(view)[i] / rx ;
        inverse[i][1] = VP_VIEW_Y_VEC(view)[i] / ry ;
        inverse[i][2] = VP_VIEW_CTR_PT(view)[i] - 
            xx * inverse[i][0] - yy * inverse[i][1] ;
    }

    VP_VIEW_X_VEC(view)[0] /= mx ;
    VP_VIEW_X_VEC(view)[1] /= mx ;
    VP_VIEW_X_VEC(view)[2] /= mx ;
    VP_VIEW_Y_VEC(view)[0] /= my ;
    VP_VIEW_Y_VEC(view)[1] /= my ;
    VP_VIEW_Y_VEC(view)[2] /= my ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC REAL vpi_gran ( transform ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_TRANSFORM transform ;
{
    REAL t0, t1, x, t ;

    t0 = fabs(transform[0][0]) + fabs(transform[0][1]) + 
            fabs(transform[0][2]);
    t1 = fabs(transform[1][0]) + fabs(transform[1][1]) + 
            fabs(transform[1][2]);
    t = t0 + t1 ;
    x = grr_get_max_x() + grr_get_max_y() ;
    RETURN ( 1.0 / ( x * t ) ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void vpi_get_wh ( vp, w, h )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
REAL *w, *h ;
{
    if ( w!=NULL ) 
        *w = 2.0 * NORM ( VP_VIEWPORT_VIEW_X_VEC(vp) ) ;
    if ( h!=NULL ) 
        *h = 2.0 * NORM ( VP_VIEWPORT_VIEW_Y_VEC(vp) ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*   vpi_get_transform ( vp, t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
VP_TRANSFORM t ;
{
    RETURN ( ala_copy ( (REAL*)VP_VIEWPORT_TRANSFORM(vp), 
        sizeof(VP_TRANSFORM)/sizeof(REAL), (REAL*)t ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL   vpi_get_gran ( vp ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
{
    RETURN ( VP_VIEWPORT_GRAN(vp) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN vpi_pt_inside ( vp, a ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
PT2 a ;
{
    RETURN ( c2a_pt_inside ( VP_VIEWPORT_SCREEN_BOX(vp), a, BBS_ZERO ) ) ;
}

