/* -2 -3 */
/********************************* W2D.C ***********************************/
/******* Display of 2-dim geometry in the window manager environment *******/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <dmldefs.h>
#include <v2ddefs.h>
#include <vpidefs.h>
#include <w2ddefs.h>
#include <wmmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w2d_set_textposition ( pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt ;
{
    vpi_clip ( WMI_CURR_VIEWPORT ) ;
    v2d_set_textposition ( pt, WMI_CURR_VIEWPORT ) ;
    vpi_unclip ( WMI_CURR_VIEWPORT ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w2d_moveto ( pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt ;
{
    DML_ITEM item ;
    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v2d_moveto ( pt, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v2d_moveto ( pt, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w2d_lineto ( pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt ;
{
    DML_ITEM item ;
    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v2d_lineto ( pt, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v2d_lineto ( pt, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w2d_curve ( curve, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm0, parm1 ;
{
    DML_ITEM item ;
    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v2d_curve ( curve, parm0, parm1, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v2d_curve ( curve, parm0, parm1, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w2d_curvelist ( curvelist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curvelist ;
{
    DML_ITEM item ;
    DML_WALK_LIST ( curvelist, item ) 
        w2d_curve ( DML_RECORD(item), NULL, NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w2d_point ( pt, size ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt ;
REAL size ;
{
    DML_ITEM item ;
    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v2d_point ( pt, size, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v2d_point ( pt, size, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}

