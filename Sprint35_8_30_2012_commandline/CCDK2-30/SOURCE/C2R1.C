/* -2 -3 */
/********************************** C2R1.C *********************************/
/***************** Routines for displaying r-arc geometry ******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alrdefs.h>
#include <c2rdefs.h>
#include <c2rmcrs.h>
#include <c2vmcrs.h>

STATIC  BOOLEAN c2r_approx1 __(( C2_ASEG, REAL, REAL, INT, REAL, C2_BOX, 
            REAL, REAL, PT2*, PARM, INT, INT*, REAL* )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2r_approx ( arc, t0, t1, j, gran, dir, 
        pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
REAL t0, t1 ;
REAL gran ;
INT j ;
BOOLEAN dir ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    INT index0, i, n ;
    REAL u, sweep, angle, c, s ;
    PT2 ctr, vec, vec1 ;

    if ( *index >= buf_size ) {
        *end_t = t0 + (REAL)j ;
        RETURN ( FALSE ) ;
    }

    if ( !IS_SMALL(t0) || !IS_SMALL(t1-1.0) ) {
        C2_ASEG_S trim_arc ;
        BOOLEAN status ;

        index0 = *index ;
        status = c2r_approx ( c2r_trim ( arc, t0, t1, &trim_arc ), 0.0, 1.0, 
            j, gran, dir, pt_buffer, parm_buffer, buf_size, index, end_t ) ;
        *end_t = alr_trim_parm ( C2_ASEG_D(arc), *end_t - (REAL)j, t0, t1 ) 
            + (REAL)j ;
        if ( parm_buffer != NULL ) {
            for ( i=index0 ; i<*index ; i++ ) {
                PARM_T(parm_buffer+i) = alr_trim_parm ( C2_ASEG_D(arc), 
                    PARM_T(parm_buffer+i) - (REAL)j, t0, t1 ) + (REAL)j ;
                PARM_J(parm_buffer+i) = j ;
            }
        }
        RETURN ( status ) ;
    }

    sweep = alr_sweep ( C2_ASEG_D(arc) ) ;
    u = C2V_DIST ( C2_ASEG_PT0(arc), C2_ASEG_PT1(arc) ) ;
    if ( IS_SMALL(u) ) 
        n = 1 ;
    else if ( fabs(C2_ASEG_D(arc)) * u <= 0.5 * gran ) 
        n = (INT) ( sqrt ( 0.5 * fabs(sweep) * 
            ( 1.0 + C2_ASEG_D(arc)*C2_ASEG_D(arc) ) * u / gran ) + 1.0 ) ;
    else {
        u = 2.0 * fabs(C2_ASEG_D(arc)) * gran / 
            ( u * ( 1.0 + C2_ASEG_D(arc) * C2_ASEG_D(arc) ) ) ;
        u = sqrt ( u ) ;
        u = u > 1.0 ? HALF_PI : asin ( u ) ;
        n = (INT) ( 0.5 * fabs(sweep) / u + 1.0 ) ;
    }
    if ( dir ) {
        C2V_COPY ( C2_ASEG_PT0(arc), pt_buffer[*index] ) ;
    }
    else {
        C2V_COPY ( C2_ASEG_PT1(arc), pt_buffer[*index] ) ;
    }
    if ( parm_buffer != NULL ) {
        PARM_T(parm_buffer+(*index)) = dir ? (REAL)j : (REAL)(j+1) ;
        PARM_J(parm_buffer+(*index)) = j ;
    }
    (*index)++ ;

    if ( *index >= buf_size ) {
        *end_t = dir ? (REAL)j : (REAL)(j+1) ;
        RETURN ( FALSE ) ;
    }
    
    if ( n > 1 ) {
/*      n /= 2 ; */
        angle = sweep / ( (REAL)n ) ; 
        c = cos ( angle ) ;
        s = sin ( angle ) ;
    STACK_AVAIL ;
        c2r_ctr ( arc, ctr ) ;
        if ( dir ) {
            C2V_SUB ( C2_ASEG_PT0(arc), ctr, vec ) ;
        }
        else {
            C2V_SUB ( C2_ASEG_PT1(arc), ctr, vec ) ;
            s = -s ;
        }

        for ( i=1 ; i<n ; i++ ) {

            C2V_COPY ( vec, vec1 ) ;
            C2V_ROTATE_VEC_CS ( vec1, c, s, vec ) ;
            C2V_ADD ( vec, ctr, pt_buffer[*index] ) ;
            if ( parm_buffer != NULL ) {
                if ( dir ) 
                    PARM_T(parm_buffer+(*index)) = (REAL)j + 
                        alr_parm ( (REAL)i * angle, C2_ASEG_D(arc) ) ;
                else
                    PARM_T(parm_buffer+(*index)) = (REAL)(j+1) - 
                        alr_parm ( (REAL)i * angle, C2_ASEG_D(arc) ) ;
                PARM_J(parm_buffer+(*index)) = j ;
            }
            (*index)++ ;
            if ( *index == buf_size ) {
                *end_t = (REAL)j + ( dir ? 
                    alr_parm ( (REAL)i*angle, C2_ASEG_D(arc) ) :
                    alr_parm ( sweep-(REAL)i*angle, C2_ASEG_D(arc) ) ) ;
                RETURN ( FALSE ) ;
            }
        }
    }

    if ( dir ) {
        C2V_COPY ( C2_ASEG_PT1(arc), pt_buffer[*index] ) ;
    }
    else {
        C2V_COPY ( C2_ASEG_PT0(arc), pt_buffer[*index] ) ;
    }
    if ( parm_buffer != NULL ) {
        PARM_T(parm_buffer+(*index)) = dir ? (REAL)(j+1) : (REAL)j ;
        PARM_J(parm_buffer+(*index)) = j ; 
    }
    (*index)++ ;
    *end_t = dir ? 1.0 + (REAL)j : (REAL)j ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2r_approx_zoomed ( arc, t0, t1, j, gran, box, w, 
        pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ; 
REAL t0, t1 ;
REAL gran ;
INT j ;
C2_BOX box ;
REAL w ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    REAL flatness ;
    BOOLEAN status ;

    if ( !IS_SMALL(t0) || !IS_SMALL(t1-1.0) ) {
        C2_ASEG_S trim_arc ;
        INT i, index0 ;
 
        index0 = *index ;
        status = c2r_approx_zoomed ( c2r_trim ( arc, t0, t1, &trim_arc ), 
            0.0, 1.0, j, gran, box, w, pt_buffer, parm_buffer, buf_size, 
            index, end_t ) ;
        *end_t = alr_trim_parm ( C2_ASEG_D(arc), *end_t - (REAL)j, t0, t1 ) 
            + (REAL)j ;

        if ( parm_buffer != NULL ) {
            for ( i=index0 ; i<*index ; i++ ) {
                PARM_T(parm_buffer+i) = alr_trim_parm ( C2_ASEG_D(arc), 
                    PARM_T(parm_buffer+i) - (REAL)j, t0, t1 ) + (REAL)j ;
                PARM_J(parm_buffer+i) = j ;
            }
        }
        RETURN ( status ) ;
    }

    flatness = c2r_flatness ( arc ) ;
    status = c2r_approx1 ( arc, t0, t1, j, gran, box, flatness, 10.0*w, 
        pt_buffer, parm_buffer, buf_size, index, end_t ) ;
    *end_t += (REAL)j ;
    RETURN ( status ) ;
}

/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c2r_approx1 ( arc, t0, t1, j, gran, box, flatness, size, 
        pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
REAL t0, t1 ;
INT j ;
REAL gran ;
C2_BOX box ;
REAL flatness, size ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    C2_ASEG_S arc0, arc1 ;
    REAL t, fl ;

    if ( ( C2_ASEG_PT0(arc)[0] < C2_MIN_X(box) - flatness && 
           C2_ASEG_PT1(arc)[0] < C2_MIN_X(box) - flatness ) || 
         ( C2_ASEG_PT0(arc)[0] > C2_MAX_X(box) + flatness && 
           C2_ASEG_PT1(arc)[0] > C2_MAX_X(box) + flatness ) || 
         ( C2_ASEG_PT0(arc)[1] < C2_MIN_Y(box) - flatness && 
           C2_ASEG_PT1(arc)[1] < C2_MIN_Y(box) - flatness ) || 
         ( C2_ASEG_PT0(arc)[1] > C2_MAX_Y(box) + flatness && 
           C2_ASEG_PT1(arc)[1] > C2_MAX_Y(box) + flatness ) ) {
        *end_t = t1 ;
        RETURN ( FALSE ) ;
    }
    if ( *index == 0 ) {
        C2V_COPY ( C2_ASEG_PT0(arc), pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) 
            SET_PARMJ ( t0, j, parm_buffer +(*index) ) ;
        (*index)++ ;
    }

    if ( flatness <= gran && 
        C2V_DISTL1 ( C2_ASEG_PT0(arc), C2_ASEG_PT1(arc) ) <= size ) {
        C2V_COPY ( C2_ASEG_PT1(arc), pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) 
            SET_PARMJ ( t1, j, parm_buffer +(*index) ) ;
        (*index)++ ;
        *end_t = t1 ;
        RETURN ( *index < buf_size ) ;
/*
        if ( *index >= buf_size ) {
            *end_t = t1 ;
            RETURN ( FALSE ) ;
        }
        RETURN ( TRUE ) ;
*/
    }

    c2r_sbdv ( arc, &arc0, &arc1 ) ;
    t = 0.5 * ( t0 + t1 ) ;
    fl = c2r_flatness ( &arc0 ) ;
    RETURN ( c2r_approx1 ( &arc0, t0, t, j, gran, box, fl, size, 
                pt_buffer, parm_buffer, buf_size, index, end_t ) &&
             c2r_approx1 ( &arc1, t, t1, j, gran, box, fl, size, 
                pt_buffer, parm_buffer, buf_size, index, end_t ) ) ;
}

