/* -2 -3 */
/******************************* C2CD.C *********************************/
/**************** Two-dimensional curves ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alrdefs.h>
#include <c2gdefs.h>
#include <c2ldefs.h>
#include <c2pdefs.h>
#include <c2ndefs.h>
#include <c2adefs.h>
#include <c2cdefs.h>
#include <c2cpriv.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_approx ( curve, parm1, draw_parm, gran, dir, 
        pt_buffer, parm_buffer, buf_size, index ) 
/*----------------------------------------------------------------------*/
C2_CURVE curve ;
PARM parm1 ;
PARM draw_parm ;
REAL gran ;
BOOLEAN dir ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
{
    BOOLEAN end_bit ;
    PARM_S start_parm ;

    *index = 0 ;
    COPY_PARM ( draw_parm, &start_parm ) ;

    if ( C2_CURVE_IS_LINE(curve) ) {
        end_bit = c2l_approx ( C2_CURVE_LINE(curve), PARM_T(&start_parm), 
            parm1 == NULL ? ( dir ? C2_CURVE_T1(curve) : C2_CURVE_T0(curve) ) 
            : PARM_T(parm1), 
            pt_buffer, parm_buffer, buf_size, index, &PARM_T(draw_parm) ) ;
        PARM_J(draw_parm) = 1 ;
    }

    else if ( C2_CURVE_IS_ARC(curve) ) {
        end_bit = c2g_approx ( C2_CURVE_ARC(curve), PARM_T(&start_parm), 
            parm1 == NULL ? ( dir ? C2_CURVE_T1(curve) : C2_CURVE_T0(curve) ) 
            : PARM_T(parm1), gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, &(PARM_T(draw_parm)) ) ;
        alr_parm_set ( draw_parm ) ;
    }

    else if ( C2_CURVE_IS_PCURVE(curve) ) {
        end_bit = c2p_approx ( C2_CURVE_PCURVE(curve), PARM_T(&start_parm), 
            parm1 == NULL ? ( dir ? C2_CURVE_T1(curve) : C2_CURVE_T0(curve) ) 
            : PARM_T(parm1), gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, &(PARM_T(draw_parm)) ) ;
        PARM_J(draw_parm) = (INT)PARM_T(draw_parm) ;
    }
    else if ( C2_CURVE_IS_ZERO_ARC(curve) ) 
        end_bit = TRUE ;
#ifdef  SPLINE
    else 
        end_bit = c2n_approx ( C2_CURVE_NURB(curve), &start_parm, parm1, 
            C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve), gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, draw_parm ) ;
#endif  /*SPLINE*/

    if ( !end_bit ) {
        if ( dir ) 
            end_bit = ( PARM_T(draw_parm) >= 
        ( parm1 == NULL ? C2_CURVE_T1(curve) : PARM_T(parm1) ) - BBS_ZERO ) ;
        else
            end_bit = ( PARM_T(draw_parm) <= 
        ( parm1 == NULL ? C2_CURVE_T0(curve) : PARM_T(parm1) ) + BBS_ZERO ) ;
    }
    RETURN ( end_bit ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2c_approx_zoomed ( curve, parm1, draw_parm, gran, 
        x, y, w, pt_buffer, parm_buffer, buf_size, index ) 
/*----------------------------------------------------------------------*/
C2_CURVE curve ;
PARM parm1 ;
PARM draw_parm ;
REAL gran ;
REAL x, y, w ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
{
    BOOLEAN end_bit ;
    PARM_S start_parm ;

    *index = 0 ;
    COPY_PARM ( draw_parm, &start_parm ) ;

    if ( C2_CURVE_IS_LINE(curve) ) {
        end_bit = c2l_approx_zoomed ( C2_CURVE_LINE(curve), 
            PARM_T(&start_parm), 
            parm1 == NULL ? C2_CURVE_T1(curve) : PARM_T(parm1), x, y, w, 
            pt_buffer, parm_buffer, buf_size, index, &PARM_T(draw_parm) ) ;
        PARM_J(draw_parm) = 1 ;
    }

    else if ( C2_CURVE_IS_ARC(curve) ) {
        end_bit = c2g_approx_zoomed ( C2_CURVE_ARC(curve), 
            PARM_T(&start_parm), 
            parm1 == NULL ? C2_CURVE_T1(curve) : PARM_T(parm1), 
            gran, x, y, w, pt_buffer, parm_buffer, buf_size, 
            index, &(PARM_T(draw_parm)) ) ;
        alr_parm_set ( draw_parm ) ;
    }

    else if ( C2_CURVE_IS_PCURVE(curve) ) {
        end_bit = c2p_approx_zoomed ( C2_CURVE_PCURVE(curve), 
            PARM_T(&start_parm), 
            parm1 == NULL ? C2_CURVE_T1(curve) : PARM_T(parm1), 
            gran, x, y, w, pt_buffer, parm_buffer, buf_size, 
            index, &(PARM_T(draw_parm)) ) ;
        PARM_J(draw_parm) = (INT)PARM_T(draw_parm) ;
    }
    else if ( C2_CURVE_IS_ZERO_ARC(curve) ) 
        end_bit = TRUE ;
#ifdef  SPLINE
    else 
        end_bit = c2n_approx_zoomed ( C2_CURVE_NURB(curve), &start_parm, 
            parm1, C2_CURVE_PARM1(curve), gran, x, y, w, 
            pt_buffer, parm_buffer, buf_size, index, draw_parm ) ;
#endif  /*SPLINE*/

    if ( !end_bit && PARM_T(draw_parm) >= 
        ( parm1 == NULL ? C2_CURVE_T1(curve) : PARM_T(parm1) ) - BBS_ZERO ) 
        end_bit = TRUE ;
    RETURN ( end_bit ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC void c2c_approx_init ( curve, parm0, draw_parm, dir ) 
/*----------------------------------------------------------------------*/
C2_CURVE curve ;
PARM parm0 ;
PARM draw_parm ;
BOOLEAN dir ;
{
    COPY_PARM ( parm0 != NULL ? parm0 : 
        ( dir ? C2_CURVE_PARM0(curve) : C2_CURVE_PARM1(curve) ), draw_parm ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void c2c_display ( curve, parm0, parm1, gran, view_ctr, w, 
    display, display_data ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm0, parm1 ;
PT2 view_ctr ;
REAL w, gran ;
PF_DISPLAY display ;
ANY display_data ;
{
    PT2 pt_buffer[C2_DISPLAY_BUF_SIZE] ;
    PARM_S curve_parm ;
    INT index ;
    BOOLEAN zoomed, end_bit ;

    zoomed = w < 0.2 * c2a_box_size ( C2_CURVE_BOX(curve) ) ;
    c2c_approx_init ( curve, parm0, &curve_parm, TRUE ) ;

    do {
        end_bit = zoomed ? 
            c2c_approx_zoomed ( curve, parm1, &curve_parm, 
                gran, view_ctr[0], view_ctr[1], w, 
                pt_buffer, NULL, C2_DISPLAY_BUF_SIZE, &index ) :
            c2c_approx ( curve, parm1, &curve_parm, gran, TRUE, 
                pt_buffer, NULL, C2_DISPLAY_BUF_SIZE, &index ) ;
        if ( display != PF_NULL ) 
            (*display) ( pt_buffer, index, display_data ) ;
    } while ( !end_bit ) ;
}

