/* -2 -3 */
/*********************************** C2BP.C ********************************/
/*********************** Two-dimensional Bezier curves *********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <bbsdefs.h>
#ifdef  SPLINE
#include <albdefs.h>
#include <c2bdefs.h>
#include <c2adefs.h>
#include <c2apriv.h>
#include <c2vmcrs.h>
#include <c2coned.h>

STATIC  BOOLEAN approx1 __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 , REAL DUMMY4 , INT DUMMY5 , REAL DUMMY6 , 
            C2_BOX DUMMY7 , REAL DUMMY8 , BOOLEAN DUMMY9 , PT2 DUMMY10 , 
            PT2* DUMMY11 , PARM DUMMY12 , INT DUMMY13 , INT* DUMMY14 , 
            REAL*  DUMMY15 )) ;
STATIC  BOOLEAN approx2 __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 , REAL DUMMY4 , INT DUMMY5 , REAL DUMMY6 , 
            C2_BOX DUMMY7 , REAL DUMMY8 , BOOLEAN DUMMY9 , PT2 DUMMY10 , 
            PT2 DUMMY11 , INT DUMMY12 , REAL DUMMY13 , PT2* DUMMY14 , 
            PARM DUMMY15 , INT DUMMY16 , INT* DUMMY17 , REAL*  DUMMY18 )) ; 
STATIC  BOOLEAN approx_non_rat __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 , INT DUMMY4 , REAL DUMMY5 , BOOLEAN DUMMY6 , 
            BOOLEAN DUMMY7 , PT2* DUMMY8 , PARM DUMMY9 , INT DUMMY10 , 
            INT* DUMMY11 , REAL*  DUMMY12 )) ;
STATIC  BOOLEAN approx1_non_rat __(( PT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 , INT DUMMY4 , REAL DUMMY5 , BOOLEAN DUMMY6 ,
            PT2* DUMMY7 , PARM DUMMY8 , INT DUMMY9 , INT* DUMMY10 , 
            REAL* DUMMY11 )) ;
STATIC  BOOLEAN approx2_non_rat __(( PT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 , INT DUMMY4 , REAL DUMMY5 , BOOLEAN DUMMY6 , 
            REAL DUMMY7 , PT2* DUMMY8 , PARM DUMMY9 , INT DUMMY10 , 
            INT* DUMMY11 , REAL*  DUMMY12 )) ;
STATIC  BOOLEAN approx3_non_rat __(( PT2* DUMMY0 , REAL DUMMY1 , REAL DUMMY2 , 
            INT DUMMY3 , REAL DUMMY4 , BOOLEAN DUMMY5 , PT2* DUMMY6 , 
            PARM DUMMY7 , INT DUMMY8 , INT* DUMMY9 , REAL* DUMMY10 )) ;
STATIC  BOOLEAN approx4_non_rat __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 , 
            PT2 DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , INT DUMMY6 , 
            REAL DUMMY7 , BOOLEAN DUMMY8 , PT2* DUMMY9 , PARM DUMMY10 , 
            INT DUMMY11 , INT* DUMMY12 , REAL*  DUMMY13 )) ;
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2b_approx ( b, d, w, t0, t1, j, gran, box, dir, 
        non_rational, ept_bit, pt_buffer, parm_buffer, buf_size, index, t_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *b ;  /* Control points of the segment */
INT     d ;    /* Order of the segment */
REAL    w ;    /* Offset of the segment */
REAL    t0, t1 ;    
INT     j ;    
REAL    gran ;
C2_BOX  box ;
BOOLEAN dir, non_rational, ept_bit ;
PT2     *pt_buffer ;
PARM    parm_buffer ;
INT     buf_size, *index ;
REAL    *t_ptr ;
{
    PT2 ept ;

    if ( box == NULL && non_rational ) /* offset = 0.0 also */
        RETURN ( approx_non_rat ( b, d, t0, t1, j, gran, dir, ept_bit, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) ) ;

    if ( ept_bit && *index >= buf_size ) {
        *t_ptr = t0 ;
        RETURN ( FALSE ) ;
    }

    if ( ( dir ? c2b_ept0 ( b, d, w, ept ) : c2b_ept1 ( b, d, w, ept ) ) && 
        ept_bit ) {
        C2V_COPY ( ept, pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) 
            SET_PARMJ ( t0, j, parm_buffer +(*index) ) ;
        (*index)++ ;
    }
    if ( *index >= buf_size ) {
        *t_ptr = t0 ;
        RETURN ( FALSE ) ;
    }
    RETURN ( approx1 ( b, d, w, t0, t1, j, gran, box, 10.0*c2a_box_size(box), 
        dir, ept, pt_buffer, parm_buffer, buf_size, index, t_ptr ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN approx1 ( b, d, w, t0, t1, j, gran, box, size, dir, ept0, 
        pt_buffer, parm_buffer, buf_size, index, t_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *b ;
INT d ;
REAL w ;
REAL t0, t1 ;
INT j ;
REAL gran ;
C2_BOX box ;
REAL size ;
BOOLEAN dir ;
PT2 ept0 ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *t_ptr ;
{
    INT conv=0 ;
    PT2 ept1 ;
    REAL flatness ;

    if ( !( dir ? c2b_ept1 ( b, d, w, ept1 ) : c2b_ept0 ( b, d, w, ept1 ) ) )
        RETURN ( FALSE ) ;
    flatness = c2b_flat_epts ( b, d, ept0, ept1, &conv ) ;
    RETURN ( approx2 ( b, d, w, t0, t1, j, gran, box, size, dir, ept0, ept1, 
        conv, flatness, pt_buffer, parm_buffer, buf_size, index, t_ptr ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN approx2 ( b, d, w, t0, t1, j, gran, box, size, dir, ept0, ept1, 
            conv, flatness, pt_buffer, parm_buffer, buf_size, index, t_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *b ;
INT d ;
REAL w ;
REAL t0, t1 ;
INT j ;
REAL gran ;
C2_BOX box ;
REAL size ;
BOOLEAN dir ;
PT2 ept0, ept1 ;
INT conv ;
REAL flatness ;
PT2 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *t_ptr ;
{
    HPT2 *bl, *br ;
    INT convl, convr ;
    PT2 mid_pt ;
    REAL mid_t, fll, flr ;
    BOOLEAN status ; 

    if ( box != NULL && flatness >= 0.0 ) {

        if ( ( ept0[0] < C2_MIN_X(box) - flatness && 
               ept1[0] < C2_MIN_X(box) - flatness ) || 
             ( ept0[0] > C2_MAX_X(box) + flatness && 
               ept1[0] > C2_MAX_X(box) + flatness ) || 
             ( ept0[1] < C2_MIN_Y(box) - flatness && 
               ept1[1] < C2_MIN_Y(box) - flatness ) || 
             ( ept0[1] > C2_MAX_Y(box) + flatness && 
               ept1[1] > C2_MAX_Y(box) + flatness ) ) {

            *t_ptr = t1 ;
            RETURN ( FALSE ) ;
        }
    }

    if ( flatness >= 0.0 && flatness <= gran && ( IS_SMALL(size) || 
        C2V_DISTL1 ( ept0, ept1 ) <= size ) ) {
        C2V_COPY ( ept1, pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) 
            SET_PARMJ ( t1, j, parm_buffer +(*index) ) ;
        (*index)++ ;
        if ( *index >= buf_size ) {
            *t_ptr = t1 ;
            RETURN ( FALSE ) ;
        }
        RETURN ( TRUE ) ;
    }

    bl = CREATE ( 2*d-1, HPT2 ) ;
    if ( bl == NULL ) 
        RETURN ( FALSE ) ;
    br = bl + d - 1 ;
    C2B_SBDV0 ( b, d, bl ) ;
    mid_t = 0.5 * ( t0 + t1 ) ;
    if ( !c2b_ept1 ( bl, d, w, mid_pt ) )
        RETURN ( FALSE ) ;

    convl = conv ;
    convr = conv ;
/*
    if ( flatness >= 8.0 * gran ) {
        fll = flr = 0.25 * flatness ;
    }
    else {
*/
        if ( dir ) {
            fll = c2b_flat_epts ( bl, d, ept0, mid_pt, &convl ) ;
            flr = c2b_flat_epts ( br, d, mid_pt, ept1, &convr ) ;
        }
        else {
            fll = c2b_flat_epts ( bl, d, ept1, mid_pt, &convl ) ;
            flr = c2b_flat_epts ( br, d, mid_pt, ept0, &convr ) ;
        }
/*
    }
*/
    if ( dir ) 
        status = 
            approx2 ( bl, d, w, t0, mid_t, j, gran, box, size, dir, 
                ept0, mid_pt, convl, fll, pt_buffer, parm_buffer, 
                buf_size, index, t_ptr ) &&
            approx2 ( br, d, w, mid_t, t1, j, gran, box, size, dir, 
                mid_pt, ept1, convr, flr, pt_buffer, parm_buffer, 
                buf_size, index, t_ptr ) ;
    else
        status = 
            approx2 ( br, d, w, t0, mid_t, j, gran, box, size, dir, 
                ept0, mid_pt, convr, flr, pt_buffer, parm_buffer, 
                buf_size, index, t_ptr ) &&
            approx2 ( bl, d, w, mid_t, t1, j, gran, box, size, dir, 
                mid_pt, ept1, convl, fll, pt_buffer, parm_buffer, 
                buf_size, index, t_ptr ) ;

    KILL ( bl ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN approx_non_rat ( b, d, t0, t1, j, gran, dir, ept_bit, 
    pt_buffer, parm_buffer, buf_size, index, t_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2    *b ;  /* Control points of the segment ; w=0*/
INT     d ;
REAL    t0, t1 ;
INT     j ;
REAL    gran ;
BOOLEAN dir, ept_bit ;
PT2     *pt_buffer ;
PARM    parm_buffer ;
INT     buf_size, *index ;
REAL    *t_ptr ;
{
    PT2 *c ;
    INT i ;
    BOOLEAN status ;

    if ( *index >= buf_size ) {
        *t_ptr = t0 ;
        RETURN ( FALSE ) ;
    }

    c = CREATE ( d, PT2 ) ;
    if ( c == NULL ) 
        RETURN ( FALSE ) ;
    for ( i=0 ; i<d ; i++ ) 
        C2V_COPY ( b[i], c[i] ) ;

    if ( ept_bit ) {
        if ( dir ) {
            C2V_COPY ( c[0], pt_buffer[*index] ) ;
        }
        else {
            C2V_COPY ( c[d-1], pt_buffer[*index] ) ;
        }
        if ( parm_buffer != NULL ) 
            SET_PARMJ ( t0, j, parm_buffer + (*index) ) ;
        (*index)++ ;
    }

    if ( *index >= buf_size ) {
        *t_ptr = t0 ;
        RETURN ( FALSE ) ;
    }
    status = approx1_non_rat ( c, d, t0, t1, j, gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) ;
    KILL ( c ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN approx1_non_rat ( b, d, t0, t1, j, gran, dir, pt_buffer, 
                            parm_buffer, buf_size, index, t_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2     *b ;
INT     d ;
REAL    t0, t1 ;
INT     j ;
REAL    gran ;
BOOLEAN dir ;
PT2     *pt_buffer ;
PARM    parm_buffer ;
INT     buf_size, *index ;
REAL    *t_ptr ;
{
    REAL flatness ;

    if ( d==4 ) 
        RETURN ( approx3_non_rat ( b, t0, t1, j, gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) ) ;

    flatness = c2b_flat_non_rat ( b, d ) ;
    RETURN ( approx2_non_rat ( b, d, t0, t1, j, gran, dir, flatness, 
        pt_buffer, parm_buffer, buf_size, index, t_ptr ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN approx2_non_rat ( b, d, t0, t1, j, gran, dir, flatness, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2     *b ;
INT     d ;
REAL    t0, t1 ;
INT     j ;
REAL    gran ;
BOOLEAN dir ;
REAL    flatness ;
PT2     *pt_buffer ;
PARM    parm_buffer ;
INT buf_size, *index ;
REAL *t_ptr ;
{
    PT2 *bl, *br ;
    REAL mid_t, fll, flr ;
    BOOLEAN status ;

    if ( flatness >= 0.0 && flatness <= gran ) {
        if ( dir ) {
            C2V_COPY ( b[d-1], pt_buffer[*index] ) ;
        }
        else {
            C2V_COPY ( b[0], pt_buffer[*index] ) ;
        }
        if ( parm_buffer != NULL ) 
            SET_PARMJ ( t1, j, parm_buffer + (*index) ) ;
        (*index)++ ;
        if ( *index >= buf_size ) {
            *t_ptr = t1 ;
            RETURN ( FALSE ) ;
        }
        RETURN ( TRUE ) ;
    }

    bl = CREATE ( 2*d-1, PT2 ) ;
    if ( bl == NULL ) 
        RETURN ( FALSE ) ;
    br = bl + d - 1 ;
    alb_sbdv0 ( (REAL*)b, d, 2, (REAL*)bl ) ;
    mid_t = 0.5 * ( t0 + t1 ) ;

    if ( flatness >= 8.0 * gran ) {
        fll = flr = 0.25 * flatness ;
    }
    else {
        fll = c2b_flat_non_rat ( bl, d ) ;
        flr = c2b_flat_non_rat ( br, d ) ;
    }

    status = dir ? 
          ( approx2_non_rat ( bl, d, t0, mid_t, j, gran, dir, fll, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) &&
            approx2_non_rat ( br, d, mid_t, t1, j, gran, dir, flr, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) ) :
          ( approx2_non_rat ( br, d, t0, mid_t, j, gran, dir, flr, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) &&
            approx2_non_rat ( bl, d, mid_t, t1, j, gran, dir, fll, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) ) ;

    KILL ( bl ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN approx3_non_rat ( b, t0, t1, j, gran, dir, 
        pt_buffer, parm_buffer, buf_size, index, t_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

PT2    *b ;  /* Control points of the segment ; w=0*/
REAL    t0, t1 ;
INT     j ;
REAL    gran ;
BOOLEAN dir ;
PT2     *pt_buffer ;
PARM    parm_buffer ;
INT     buf_size, *index ;
REAL    *t_ptr ;
{
    PT2 c0, c1, c2, c3 ;

    C2V_COPY ( b[0], c0 ) ;
    C2V_COPY ( b[3], c1 ) ;
    c2[0] = b[2][0] - 2.0*b[1][0] + b[0][0] ;
    c2[1] = b[2][1] - 2.0*b[1][1] + b[0][1] ;
    c3[0] = b[3][0] - 2.0*b[2][0] + b[1][0] ;
    c3[1] = b[3][1] - 2.0*b[2][1] + b[1][1] ;

    RETURN ( approx4_non_rat ( c0, c1, c2, c3, t0, t1, j, gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN approx4_non_rat ( b0, b1, c0, c1, t0, t1, j, gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2     b0, b1, c0, c1 ;
REAL    t0, t1 ;
INT     j ;
REAL    gran ;
BOOLEAN dir ;
PT2     *pt_buffer ;
PARM    parm_buffer ;
INT     buf_size, *index ;
REAL    *t_ptr ;
{
    PT2 bm, cl, cm, cr ;
    REAL mid_t ;

    if ( fabs(c0[0]) <= gran && fabs(c0[1]) <= gran && 
         fabs(c1[0]) <= gran && fabs(c1[1]) <= gran ) {

        if ( dir ) {
            C2V_COPY ( b1, pt_buffer[*index] ) ;
        }
        else {
            C2V_COPY ( b0, pt_buffer[*index] ) ;
        }
        if ( parm_buffer != NULL ) 
            SET_PARMJ ( t1, j, parm_buffer + (*index) ) ;
        (*index)++ ;
        if ( *index >= buf_size ) {
            *t_ptr = t1 ;
            RETURN ( FALSE ) ;
        }
        RETURN ( TRUE ) ;
    }

    C2V_SCALE ( c0, 0.25, cl ) ;
    C2V_SCALE ( c1, 0.25, cr ) ;
    C2V_MID_PT ( cl, cr, cm ) ;
    C2V_MID_PT ( b0, b1, bm ) ;
    C2V_ADDT ( bm, cm, -3.0, bm ) ;
    mid_t = 0.5 * ( t0 + t1 ) ;
    RETURN ( dir ? 
        ( approx4_non_rat ( b0, bm, cl, cm, t0, mid_t, j, gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) &&
            approx4_non_rat ( bm, b1, cm, cr, mid_t, t1, j, gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) ) :
        ( approx4_non_rat ( bm, b1, cm, cr, t0, mid_t, j, gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) &&
            approx4_non_rat ( b0, bm, cl, cm, mid_t, t1, j, gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) ) ) ;
}

#endif  /*SPLINE*/

