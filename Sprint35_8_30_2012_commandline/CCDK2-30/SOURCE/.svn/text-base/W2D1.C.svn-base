/* -2 -3 */
/********************************* W2D1.C **********************************/
/******* Display of 2-dim geometry in the window manager environment *******/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h>
#include <c2ddefs.h>
#include <v2ddefs.h>
#include <v2dpriv.h>
#include <vpidefs.h>
#include <w2ddefs.h>
#include <w2dpriv.h>
#include <c2vmcrs.h>
#include <wmmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w2d_poly ( a, n ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a  ;
INT n ;
{
    DML_ITEM item ;

    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v2d_poly ( a, n, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v2d_poly ( a, n, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w2d_curve_dir ( curve, dir ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
INT dir ;
{
    DML_ITEM item ;
    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v2d_curve_dir ( curve, dir, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v2d_curve_dir ( curve, dir, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}


#ifdef SPLINE

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void w2d_hpoly ( a, n ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a  ;
INT n ;
{
    DML_ITEM item ;
    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v2d_hpoly ( a, n, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v2d_hpoly ( a, n, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}            


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void w2d_ctl_poly ( curve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
{
    DML_ITEM item ;
    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v2d_ctl_poly ( curve, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v2d_ctl_poly ( curve, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}            
#endif /*SPLINE*/

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w2d_curve_box ( curve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
{
    w2d_box ( C2_CURVE_BOX(curve) ) ; 
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w2d_box ( box ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_BOX box ;
{
    DML_ITEM item ;
    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v2d_box ( box, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v2d_box ( box, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w2d_line ( pt0, pt1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1 ;
{
    w2d_moveto ( pt0 ) ;
    w2d_lineto ( pt1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w2d_inf_line ( pt, angle ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt ;
REAL angle ;
{
    DML_ITEM item ;

    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v2d_inf_line ( pt, angle, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v2d_inf_line ( pt, angle, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w2d_circle ( ctr, rad ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr ;
REAL rad ;
{
    C2_CURVE circle ;
    circle = c2d_circle ( ctr, rad ) ;
    w2d_curve ( circle, NULL, NULL ) ;
    c2d_free_curve ( circle ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w2d_cross ( pt, a ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt ;
REAL a ;
{
    PT2 p0, p1 ;
    C2V_SET ( pt[0]-a, pt[1], p0 ) ;
    C2V_SET ( pt[0]+a, pt[1], p1 ) ;
    w2d_line ( p0, p1 ) ;
    C2V_SET ( pt[0], pt[1]-a, p0 ) ;
    C2V_SET ( pt[0], pt[1]+a, p1 ) ;
    w2d_line ( p0, p1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w2d_arrow ( pt, vec, a, b, h ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt, vec ;
REAL a, b, h ;
{
    DML_ITEM item ;
    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v2d_arrow ( pt, vec, a, b, h, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v2d_arrow ( pt, vec, a, b, h, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT w2d_select ( curve, pt, sel_parm, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 pt ;
PARM sel_parm ;
REAL *dist_ptr ;
{
    DML_ITEM item ;
    for ( item=DML_LAST(WMI_VIEWPORT_LIST) ; item!=NULL ; 
        item=DML_PREV(item) ) 
        if ( v2d_select ( curve, pt, DML_RECORD(item), sel_parm, dist_ptr ) )
            RETURN ( (VP_VIEWPORT)DML_RECORD(item) ) ;
    RETURN ( NULL ) ;
}


