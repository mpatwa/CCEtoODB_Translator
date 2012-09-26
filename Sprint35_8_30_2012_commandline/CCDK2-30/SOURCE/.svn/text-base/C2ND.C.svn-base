/* -2 -3 */
/********************************** C2ND.C *********************************/
/********************************** Nurbs **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef  SPLINE
#include <c2ndefs.h>
#include <c2sdefs.h>
#include <c2nmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2n_approx ( nurb, start_parm, end_parm, parm0, parm1, 
        gran, dir, pt_buffer, parm_buffer, buf_size, index, draw_parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM start_parm, end_parm, parm0, parm1 ;
REAL gran ;
BOOLEAN dir ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
PARM draw_parm ;
{
    RETURN ( c2s_approx ( C2_NURB_CTPT(nurb), C2_NURB_N(nurb), 
        C2_NURB_D(nurb), C2_NURB_KNOT(nurb), C2_NURB_W(nurb), start_parm, 
        end_parm != NULL ? end_parm : ( dir ? parm1 : parm0 ), 
        gran, dir, pt_buffer, parm_buffer, buf_size, index, draw_parm ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2n_approx_zoomed ( nurb, start_parm, end_parm, parm1, 
        gran, x, y, w, pt_buffer, parm_buffer, buf_size, index, draw_parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM start_parm, end_parm, parm1 ;
REAL gran, x, y, w ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
PARM draw_parm ;
{
    RETURN ( c2s_approx_zoomed ( C2_NURB_CTPT(nurb), C2_NURB_N(nurb), 
        C2_NURB_D(nurb), C2_NURB_KNOT(nurb), C2_NURB_W(nurb), start_parm, 
        end_parm != NULL ? end_parm : parm1, 
        gran, x, y, w, pt_buffer, parm_buffer, buf_size, index, draw_parm ) ) ;
}
#endif  /*SPLINE*/

