/* -2 -3 */
/********************************** C2G1.C *********************************/
/****************** Routines for processing arc geometry *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2gdefs.h>
#include <c2rdefs.h>
#include <c2gmcrs.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2g_approx ( arc, t0, t1, gran, dir, 
        pt_buffer, parm_buffer, buf_size, index, end_t )   
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL t0, t1 ;
REAL gran ;
BOOLEAN dir ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    C2_ASEG_S arc1 ;
    if ( C2_ARC_ZERO_RAD(arc) ) {
        PT2 ctr ;
        if ( *index >= buf_size ) {
            *end_t = t0 ;
            RETURN ( FALSE ) ;
        }
        c2r_ctr ( C2_ARC_SEG(arc), ctr ) ;
        C2V_COPY ( ctr, pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) {
            PARM_T(parm_buffer+(*index)) = t0 ;
            PARM_J(parm_buffer+(*index)) = 0 ;
        }
        (*index)++ ;
        if ( *index >= buf_size ) {
            *end_t = t1 ;
            RETURN ( FALSE ) ;
        }
        C2V_COPY ( ctr, pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) {
            PARM_T(parm_buffer+(*index)) = t1 ;
            PARM_J(parm_buffer+(*index)) = t1<=1.0 ? 0 : 1 ;
        }
        (*index)++ ;
        RETURN ( TRUE ) ;
    }

    if ( dir ) {
        if ( t1 <= 1.0 + BBS_ZERO ) 
            RETURN ( c2r_approx ( C2_ARC_SEG(arc), t0, t1, 0, gran, dir, 
                pt_buffer, parm_buffer, buf_size, index, end_t ) ) ;
        else if ( t0 >= 1.0 - BBS_ZERO ) 
            RETURN ( c2r_approx ( c2r_complement ( C2_ARC_SEG(arc), &arc1 ), 
                t0-1.0, t1-1.0, 1, gran, dir, pt_buffer, parm_buffer, 
                buf_size, index, end_t ) ) ;
        else 
            RETURN ( c2r_approx ( C2_ARC_SEG(arc), t0, 1.0, 0, gran, dir, 
                pt_buffer, parm_buffer, buf_size, index, end_t ) && 
                c2r_approx ( c2r_complement ( C2_ARC_SEG(arc), &arc1 ), 
                0.0, t1-1.0, 1, gran, dir, pt_buffer, parm_buffer, 
                buf_size, index, end_t ) ) ;
    }

    else {
        if ( t0 <= 1.0 + BBS_ZERO ) 
            RETURN ( c2r_approx ( C2_ARC_SEG(arc), t1, t0, 0, gran, dir, 
                pt_buffer, parm_buffer, buf_size, index, end_t ) ) ;
        else if ( t1 >= 1.0 - BBS_ZERO ) 
            RETURN ( c2r_approx ( c2r_complement ( C2_ARC_SEG(arc), &arc1 ), 
                t1-1.0, t0-1.0, 1, gran, dir, pt_buffer, parm_buffer, 
                buf_size, index, end_t ) ) ;
        else 
            RETURN ( c2r_approx ( c2r_complement ( C2_ARC_SEG(arc), &arc1 ), 
                0.0, t0-1.0, 1, gran, dir, pt_buffer, parm_buffer, 
                buf_size, index, end_t ) && 
                c2r_approx ( C2_ARC_SEG(arc), t1, 1.0, 0, gran, dir, 
                pt_buffer, parm_buffer, buf_size, index, end_t ) ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2g_approx_zoomed ( arc, t0, t1, gran, x, y, w, 
        pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
REAL t0, t1 ;
REAL gran ;
REAL x, y, w ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    C2_BOX_S box ;
    C2_ASEG_S arc1 ;

    C2_MIN_X(&box) = x - w ;
    C2_MAX_X(&box) = x + w ;
    C2_MIN_Y(&box) = y - w ;
    C2_MAX_Y(&box) = y + w ;

    if ( C2_ARC_ZERO_RAD(arc) ) {
        PT2 ctr ;
        if ( *index >= buf_size ) {
            *end_t = t0 ;
            RETURN ( FALSE ) ;
        }
        C2V_COPY ( c2r_ctr ( C2_ARC_SEG(arc), ctr ), pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) {
            PARM_T(parm_buffer+(*index)) = t0 ;
            PARM_J(parm_buffer+(*index)) = 0 ;
        }
        (*index)++ ;
        if ( *index >= buf_size ) {
            *end_t = t1 ;
            RETURN ( FALSE ) ;
        }
        C2V_COPY ( ctr, pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) {
            PARM_T(parm_buffer+(*index)) = t1 ;
            PARM_J(parm_buffer+(*index)) = t1<=1.0 ? 0 : 1 ;
        }
        (*index)++ ;
        RETURN ( TRUE ) ;
    }

    if ( t1 <= 1.0 + BBS_ZERO ) 
        RETURN ( c2r_approx_zoomed ( C2_ARC_SEG(arc), t0, t1, 0, gran, &box, 
            w, pt_buffer, parm_buffer, buf_size, index, end_t ) ) ;
    else if ( t0 >= 1.0 - BBS_ZERO )  
        RETURN ( c2r_approx_zoomed ( c2r_complement ( C2_ARC_SEG(arc), &arc1 ), 
            t0-1.0, t1-1.0, 1, gran, &box, w, pt_buffer, parm_buffer, 
            buf_size, index, end_t ) ) ;
    else 
        RETURN ( c2r_approx_zoomed ( C2_ARC_SEG(arc), t0, 1.0, 0, gran, &box, 
            w, pt_buffer, parm_buffer, buf_size, index, end_t ) &&
            c2r_approx_zoomed ( c2r_complement ( C2_ARC_SEG(arc), &arc1 ), 
            0.0, t1-1.0, 1, gran, &box, w, pt_buffer, parm_buffer, 
            buf_size, index, end_t ) ) ;
}

