/* -2 -3 */
/********************************** C2Q1.C *********************************/
/*************** Routines for processing polycurve geometry ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2qdefs.h>
#include <c2rdefs.h>
#include <c2qmcrs.h>
#include <c2rmcrs.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2q_approx ( buffer, t0, t1, j0, gran, dir, 
        pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
REAL t0, t1 ;
INT j0 ;
REAL gran ;
BOOLEAN dir ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    INT i0, i1, i ;
    REAL u0, u1 ;

    if ( C2_PC_BUFFER_N(buffer) <= 0 ) 
        RETURN ( TRUE ) ;
    if ( t0 >= (REAL)(j0+C2_PC_BUFFER_N(buffer)-1) || t1 <= (REAL)j0 )
        RETURN ( TRUE ) ;
    u0 = ( t0 > (REAL)j0 ) ? ( t0 - (REAL)j0 ) : 0.0 ;
    u1 = ( t1 < (REAL)(j0+C2_PC_BUFFER_N(buffer)-1) ) ? 
        ( t1 - (REAL)j0 ) : (REAL)(C2_PC_BUFFER_N(buffer)-1) ;
    i0 = (INT)u0 ;
    i1 = (INT)u1 ;

    if ( C2_PC_BUFFER_N(buffer) == 1 ) {
        if ( *index >= buf_size ) {
            *end_t = t0 ;
            RETURN ( FALSE ) ;
        }
        C2V_COPY ( C2_PC_BUFFER_PT(buffer,0), pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) 
            PARM_SETJ ( t0, i0+j0, parm_buffer+(*index) ) ;
        (*index)++ ;
        *end_t = t0 ;
        RETURN ( TRUE ) ;
    }

    u0 -= (REAL)i0 ;
    u1 -= (REAL)i1 ;

    if ( dir ) {
        if ( i0==i1 ) 
            RETURN ( c2r_approx ( C2_PC_BUFFER_ARC(buffer,i0), u0, u1, i0+j0, 
                gran, dir, pt_buffer, parm_buffer, buf_size, index, end_t ) ) ;
        if ( i0 > i1 ) 
            RETURN ( TRUE ) ;
        if ( !c2r_approx ( C2_PC_BUFFER_ARC(buffer,i0), u0, 1.0, i0+j0, gran, 
            dir, pt_buffer, parm_buffer, buf_size, index, end_t ) )
            RETURN ( FALSE ) ;

        for ( i=i0+1 ; i<i1 ; i++ ) {
            (*index)-- ;
            if ( !c2r_approx ( C2_PC_BUFFER_ARC(buffer,i), 0.0, 1.0, i+j0, 
                gran, dir, pt_buffer, parm_buffer, buf_size, index, end_t ) )
                RETURN ( FALSE ) ;
        }

        if ( u1 > BBS_ZERO ) {
            (*index)-- ;
            if ( !c2r_approx ( C2_PC_BUFFER_ARC(buffer,i1), 0.0, u1, i1+j0, 
                gran, dir, pt_buffer, parm_buffer, buf_size, index, end_t ) )
                RETURN ( FALSE ) ;
        }
    }

    else {
        if ( i0==i1+1 ) 
            RETURN ( c2r_approx ( C2_PC_BUFFER_ARC(buffer,i1), u1, 
                u0<BBS_ZERO?1.0:u0, i1+j0, gran, dir, 
                pt_buffer, parm_buffer, buf_size, index, end_t ) ) ;
        if ( i0 < i1 ) 
            RETURN ( TRUE ) ;
        if ( !c2r_approx ( C2_PC_BUFFER_ARC(buffer,i0-1), u0, 1.0, i0-1+j0, 
            gran, dir, pt_buffer, parm_buffer, buf_size, index, end_t ) )
            RETURN ( FALSE ) ;

        for ( i=i0-2 ; i>i1 ; i-- ) {
            (*index)-- ;
            if ( !c2r_approx ( C2_PC_BUFFER_ARC(buffer,i), 0.0, 1.0, i+j0, 
                gran, dir, pt_buffer, parm_buffer, buf_size, index, end_t ) )
                RETURN ( FALSE ) ;
        }

        if ( u1 < 1.0-BBS_ZERO ) {
            (*index)-- ;
            if ( !c2r_approx ( C2_PC_BUFFER_ARC(buffer,i1), u1, 1.0, i1+j0, 
                gran, dir, pt_buffer, parm_buffer, buf_size, index, end_t ) )
                RETURN ( FALSE ) ;
        }
    }

    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2q_approx_zoomed ( buffer, t0, t1, j0, gran, x, y, w, 
            pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
REAL t0, t1 ;
INT j0 ;
REAL gran ;
REAL x, y, w ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    C2_BOX_S box ;
    INT i0, i1, i ;
    REAL u0, u1 ;

    if ( C2_PC_BUFFER_N(buffer) <= 0 ) 
        RETURN ( TRUE ) ;
    if ( t0 >= (REAL)(j0+C2_PC_BUFFER_N(buffer)-1) || t1 <= (REAL)j0 )
        RETURN ( TRUE ) ;
    u0 = ( t0 > (REAL)j0 ) ? ( t0 - (REAL)j0 ) : 0.0 ;
    u1 = ( t1 < (REAL)(j0+C2_PC_BUFFER_N(buffer)-1) ) ? 
        ( t1 - (REAL)j0 ) : (REAL)(C2_PC_BUFFER_N(buffer)-1) ;
    i0 = (INT)u0 ;
    i1 = (INT)u1 ;

    C2_MIN_X(&box) = x - w ;
    C2_MAX_X(&box) = x + w ;
    C2_MIN_Y(&box) = y - w ;
    C2_MAX_Y(&box) = y + w ;

    if ( C2_PC_BUFFER_N(buffer) == 1 ) {
        if ( *index >= buf_size ) {
            *end_t = t0 ;
            RETURN ( FALSE ) ;
        }
        C2V_COPY ( C2_PC_BUFFER_PT(buffer,0), pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) 
            PARM_SETJ ( t0, i0+j0, parm_buffer+(*index) ) ;
        (*index)++ ;
        *end_t = t0 ;
        RETURN ( TRUE ) ;
    }

    u0 -= (REAL)i0 ;
    u1 -= (REAL)i1 ;

    if ( i0==i1 ) 
        RETURN ( c2r_approx_zoomed ( C2_PC_BUFFER_ARC(buffer,i0), u0, u1, 
            i0+j0, gran, &box, w, pt_buffer, 
            parm_buffer, buf_size, index, end_t ) ) ;

    if ( !c2r_approx_zoomed ( C2_PC_BUFFER_ARC(buffer,i0), u0, 1.0, i0+j0, 
        gran, &box, w, pt_buffer, parm_buffer, buf_size, index, end_t ) )
        RETURN ( FALSE ) ;

    for ( i=i0+1 ; i<i1 ; i++ ) {
        if ( !c2r_approx_zoomed ( C2_PC_BUFFER_ARC(buffer,i), 0.0, 1.0, i+j0, 
            gran, &box, w, pt_buffer, parm_buffer, buf_size, index, end_t ) )
            RETURN ( FALSE ) ;
    }

    if ( u1 > BBS_ZERO ) {
        if ( !c2r_approx_zoomed ( C2_PC_BUFFER_ARC(buffer,i1), 0.0, u1, i1+j0, 
            gran, &box, w, pt_buffer, parm_buffer, buf_size, index, end_t ) )
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}

