/* -2 -3 */
/********************************** C2LD.C *********************************/
/******************** Routines for processing lines ************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2ldefs.h>
#include <c2lmcrs.h>
#include <c2vmcrs.h>

STATIC BOOLEAN clip_line __(( PT2, PT2, REAL, REAL, REAL, REAL, REAL, 
            REAL, REAL, PT2*, PARM, INT, INT*, REAL* )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2l_approx ( line, t0, t1, pt_buffer, 
            parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
REAL t0, t1 ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    if ( *index >= buf_size ) {
        *end_t = t0 ;
        RETURN ( FALSE ) ;
    }

    if ( IS_ZERO(t0) ) {
        C2V_COPY ( C2_LINE_PT0(line), pt_buffer[*index] ) ;
    }
    else {
        C2V_ADDU ( C2_LINE_PT0(line), C2_LINE_PT1(line), t0, 
            pt_buffer[*index] ) ;
    }

    if ( parm_buffer != NULL ) {
        PARM_T(parm_buffer+(*index)) = t0 ;
        PARM_J(parm_buffer+(*index)) = 1 ;
    }
    (*index)++ ;

    if ( *index >= buf_size ) {
        *end_t = t1 ;
        RETURN ( FALSE ) ;
    }

    if ( IS_ZERO(t1-1.0) ) {
        C2V_COPY ( C2_LINE_PT1(line), pt_buffer[*index] ) ;
    }
    else {
        C2V_ADDU ( C2_LINE_PT0(line), C2_LINE_PT1(line), t1, 
        pt_buffer[*index] ) ;
    }
    if ( parm_buffer != NULL ) {
        PARM_T(parm_buffer+(*index)) = t1 ;
        PARM_J(parm_buffer+(*index)) = 1 ;
    }
    (*index)++ ;

    *end_t = t1 ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2l_approx_zoomed ( line, t0, t1, x, y, w, 
        pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
REAL t0, t1 ;
REAL x, y, w ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    REAL x_min, x_max, y_min, y_max ;
    PT2 pt0, pt1 ;

    if ( *index >= buf_size ) {
        *end_t = t0 ;
        RETURN ( FALSE ) ;
    }

    x_min = x - 0.5*w ;
    x_max = x_min + w ;
    y_min = y - 0.5*w ;
    y_max = y_min + w ;

    if ( IS_ZERO(t0) ) {
        C2V_COPY ( C2_LINE_PT0(line), pt0 ) ;
    }
    else {
        C2V_ADDU ( C2_LINE_PT0(line), C2_LINE_PT1(line), t0, pt0 ) ;
    }

    if ( IS_ZERO(t1-1.0) ) {
        C2V_COPY ( C2_LINE_PT1(line), pt1 ) ;
    }
    else {
        C2V_ADDU ( C2_LINE_PT1(line), C2_LINE_PT1(line), t1, pt1 ) ;
    }

    RETURN ( clip_line ( pt0, pt1, t0, t1, x_min, x_max, y_min, y_max, 
        10.0*w, pt_buffer, parm_buffer, buf_size, index, end_t ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN clip_line ( pt0, pt1, t0, t1, x_min, x_max, y_min, y_max, 
        size, pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1 ;
REAL t0, t1 ;
REAL x_min, x_max, y_min, y_max, size ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    PT2 pt ;
    REAL t ;

    if ( ( pt0[0] < x_min && pt1[0] < x_min ) || 
         ( pt0[0] > x_max && pt1[0] > x_max ) || 
         ( pt0[1] < y_min && pt1[1] < y_min ) ||
         ( pt0[1] > y_max && pt1[1] > y_max ) )
        RETURN ( TRUE ) ;

    if ( C2V_DISTL1 ( pt0, pt1 ) <= size ) {

        if ( *index >= buf_size ) {
            *end_t = t0 ;
            RETURN ( FALSE ) ;
        }

        C2V_COPY ( pt0, pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) {
            PARM_T(parm_buffer+(*index)) = t0 ;
            PARM_J(parm_buffer+(*index)) = 1 ;
        }
        (*index)++ ;

        if ( *index >= buf_size ) {
            *end_t = t1 ;
            RETURN ( FALSE ) ;
        }

        C2V_COPY ( pt1, pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) {
            PARM_T(parm_buffer+(*index)) = t1 ;
            PARM_J(parm_buffer+(*index)) = 1 ;
        }
        (*index)++ ;
        *end_t = t1 ;
        RETURN ( TRUE ) ;
    }

    else {
        t = 0.5 * ( t0 + t1 ) ;
        C2V_MID_PT ( pt0, pt1, pt ) ;
        RETURN ( clip_line ( pt0, pt, t0, t, x_min, x_max, y_min, y_max, 
            size, pt_buffer, parm_buffer, buf_size, index, end_t ) && 
            clip_line ( pt, pt1, t, t1, x_min, x_max, y_min, y_max, 
            size, pt_buffer, parm_buffer, buf_size, index, end_t ) ) ;
    }
}

