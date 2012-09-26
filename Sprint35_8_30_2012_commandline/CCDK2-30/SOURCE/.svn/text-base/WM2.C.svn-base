/* -2 -3 */
/********************************* WM2.C ***********************************/
/******************************* Window Manager ****************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <dmldefs.h>
#include <vp2defs.h>
#include <wm2defs.h>
#include <wmmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT wm2_create ( x, y, w, h, px0, py0, px1, py1, 
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
    VP_VIEWPORT viewport ; 
    viewport = vp2_create ( x, y, w, h, px0, py0, px1, py1, 
        bd_color, bk_color ) ;
    dml_append_data ( WMI_VIEWPORT_LIST, viewport ) ;
    RETURN ( viewport ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT wm2_fit ( box, border_factor ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_BOX box ;
REAL border_factor ;
{
    RETURN ( vp2_fit ( WMI_CURR_VIEWPORT, box, border_factor ) ) ;
}

