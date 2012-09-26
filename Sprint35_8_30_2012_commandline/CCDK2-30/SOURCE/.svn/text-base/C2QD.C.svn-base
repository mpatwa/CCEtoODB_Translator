/* -2 -3 */
/********************************** C2QD.C *********************************/
/**************** Routines for processing polycurve geometry ***************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2qdefs.h>
#include <c2rdefs.h>
#include <c2vdefs.h>
#include <c2qmcrs.h>
#include <c2rmcrs.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_PC_BUFFER c2q_check_size ( buffer, add_size ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
INT add_size ;
{
    if ( C2_PC_BUFFER_N(buffer) + add_size <= C2_PC_BUFFER_SIZE(buffer) )
        RETURN ( buffer ) ;
    C2_PC_BUFFER_SIZE(buffer) = C2_PC_BUFFER_N(buffer) + add_size + 10 ;
    REALLOC ( C2_PC_BUFFER_A(buffer), 3*C2_PC_BUFFER_SIZE(buffer)-1, 
        REAL ) ;
    if ( C2_PC_BUFFER_A(buffer)==NULL ) {
        FREE ( buffer ) ;
        RETURN ( NULL ) ;
    }
    RETURN ( buffer ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PC_BUFFER c2q_add ( buffer, pt, d ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
PT2 pt ;
REAL d ;
{
    INT n = C2_PC_BUFFER_N(buffer) ;

    if ( c2q_check_size ( buffer, 1 ) == NULL )
        RETURN ( NULL ) ;
    C2V_COPY ( pt, C2_PC_BUFFER_PT(buffer,n) ) ;
    C2_PC_BUFFER_D(buffer,n-1) = d ;
    C2_PC_BUFFER_N(buffer) += 1 ;
    RETURN ( buffer ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PC_BUFFER c2q_add_2pts_d ( buffer, pt0, pt1, d, tol ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
PT2 pt0, pt1 ;
REAL d, tol ;
{
    PT2 p, q ;

    if ( C2_PC_BUFFER_N(buffer) == 2 ) {
        c2q_ept0 ( buffer, p ) ;
        if ( ( C2V_DIST ( p, pt0 ) <= tol ) || 
             ( C2V_DIST ( p, pt1 ) <= tol ) ) {
            C2V_COPY ( C2_PC_BUFFER_PT(buffer,1), q ) ;
            C2V_COPY ( C2_PC_BUFFER_PT(buffer,0), C2_PC_BUFFER_PT(buffer,1) ) ;
            C2V_COPY ( q, C2_PC_BUFFER_PT(buffer,0) ) ;
            C2_PC_BUFFER_D(buffer,0) = - C2_PC_BUFFER_D(buffer,0) ;
        }
    }

    c2q_ept1 ( buffer, p ) ;
    if ( C2V_DIST ( p, pt0 ) <= tol ) 
        RETURN ( c2q_add ( buffer, pt1, d ) ) ;
    else if ( C2V_DIST ( p, pt1 ) <= tol ) 
        RETURN ( c2q_add ( buffer, pt0, -d ) ) ;
    else 
        RETURN ( NULL ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PC_BUFFER c2q_add_arc ( buffer, arc ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
C2_ASEG arc ;
{
    INT n = C2_PC_BUFFER_N(buffer) ;
    if ( arc == NULL ) 
        RETURN ( buffer ) ;
    if ( c2q_check_size ( buffer, 1 ) == NULL )
        RETURN ( NULL ) ;
    C2V_COPY ( C2_ASEG_PT1(arc), C2_PC_BUFFER_PT(buffer,n) ) ;
    C2_PC_BUFFER_D(buffer,n-1) = C2_ASEG_D(arc) ;
    C2_PC_BUFFER_N(buffer) += 1 ;
    RETURN ( buffer ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PC_BUFFER c2q_add_arc_dir ( buffer, arc, dir ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
C2_ASEG arc ;
INT dir ;
{
    INT n = C2_PC_BUFFER_N(buffer) ;
    if ( arc == NULL ) 
        RETURN ( buffer ) ;
    if ( c2q_check_size ( buffer, 1 ) == NULL )
        RETURN ( NULL ) ;
    if ( dir == 1 ) {
        C2V_COPY ( C2_ASEG_PT1(arc), C2_PC_BUFFER_PT(buffer,n) ) ;
        C2_PC_BUFFER_D(buffer,n-1) = C2_ASEG_D(arc) ;
    }
    else if ( dir == 1 ) {
        C2V_COPY ( C2_ASEG_PT0(arc), C2_PC_BUFFER_PT(buffer,n) ) ;
        C2_PC_BUFFER_D(buffer,n-1) = - C2_ASEG_D(arc) ;
    }
    C2_PC_BUFFER_N(buffer) += 1 ;
    RETURN ( buffer ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PC_BUFFER c2q_add_arc_2pts ( buffer, a0, a1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
PT2 a0, a1 ;
{
    C2_ASEG_S arc ;
    RETURN ( c2q_add_arc ( buffer, 
        c2r_3pts ( C2_PC_BUFFER_PT1(buffer), a0, a1, &arc ) ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PC_BUFFER c2q_add_arc_ctr_pt ( buffer, ctr, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
PT2 ctr, a ;
{
    C2_ASEG_S arc1, arc2 ;
    PT2 tan, tan1, tan2 ;

    c2r_2pts_ctr ( C2_PC_BUFFER_PT1(buffer), a, ctr, 1, &arc1 ) ;
    c2r_2pts_ctr ( C2_PC_BUFFER_PT1(buffer), a, ctr, 1, &arc2 ) ;
    c2r_tan1 ( &arc1, tan1 ) ;
    c2v_normalize ( tan1, tan1 ) ;
    c2r_tan0 ( &arc2, tan2 ) ;
    c2v_normalize ( tan2, tan2 ) ;
    c2r_tan1 ( C2_PC_BUFFER_ARC1(buffer), tan ) ;
    if ( C2V_DOT ( tan1, tan ) > C2V_DOT ( tan2, tan ) )
        RETURN ( c2q_add_arc ( buffer, &arc1 ) ) ;
    else
        RETURN ( c2q_add_arc ( buffer, &arc2 ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PC_BUFFER c2q_add_arc_tan ( buffer, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
PT2 a ;
{
    C2_ASEG_S arc ;
    PT2 tan ;
    INT n = C2_PC_BUFFER_N(buffer) ;

    c2r_tan1 ( C2_PC_BUFFER_ARC(buffer,n-2), tan ) ;
    RETURN ( c2q_add_arc ( buffer, 
        c2r_2pts_tan ( C2_PC_BUFFER_PT1(buffer), tan, a, &arc ) ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PC_BUFFER c2q_add_arc_tan0 ( buffer, a, tan0 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
PT2 a, tan0 ;
{
    C2_ASEG_S arc ;
#ifndef __WATCOM__
#ifndef __BRLNDC__
    C2_PC_BUFFER_N(buffer) ;
#endif
#endif

    RETURN ( c2q_add_arc ( buffer, 
        c2r_2pts_tan ( C2_PC_BUFFER_PT1(buffer), tan0, a, &arc ) ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PC_BUFFER c2q_add_line ( buffer, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
PT2 a ;
{
    RETURN ( c2q_add ( buffer, a, 0.0 ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PC_BUFFER c2q_add_line_tan ( buffer, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
PT2 a ;
{
    PT2 p, p1, vec, tan ;
    INT n = C2_PC_BUFFER_N(buffer) ;
    REAL t ;

    c2q_ept1 ( buffer, p1 ) ;
    c2r_tan1 ( C2_PC_BUFFER_ARC(buffer,n-2), tan ) ;
    C2V_SUB ( a, p1, vec ) ;
    t = C2V_DOT ( vec, tan ) / C2V_DOT ( tan, tan ) ;
    C2V_ADDT ( p1, tan, t, p ) ;
    RETURN ( c2q_add_line ( buffer, p ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PC_BUFFER c2q_remove_last ( buffer ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
{
    C2_PC_BUFFER_N(buffer)-- ;
    RETURN ( C2_PC_BUFFER_N(buffer) > 0 ? buffer : NULL ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PC_BUFFER c2q_close ( buffer ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
{
    PT2 p0 ;
    c2q_ept0 ( buffer, p0 ) ;
    RETURN ( c2q_add_line ( buffer, p0 ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PC_BUFFER c2q_trim ( buffer0, t0, t1, j0, buffer1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer0, buffer1 ;
REAL t0, t1 ;
INT j0 ;
{
    INT i0, i1, n, i ;

    if ( buffer0 == NULL || C2_PC_BUFFER_N(buffer0) <= 0 ) 
        RETURN ( buffer1 ) ;
    t0 -= (REAL)j0 ;
    t1 -= (REAL)j0 ;
    if ( t0 < 0.0 ) 
        t0 = 0.0 ;
    if ( t1 > (REAL) ( C2_PC_BUFFER_N(buffer0) - 1 ) )
        t1 = (REAL) ( C2_PC_BUFFER_N(buffer0) - 1 ) ;
    if ( t0 >= (REAL) ( C2_PC_BUFFER_N(buffer0) - 1 ) || t1 <= 0.0 )
        RETURN ( NULL ) ;

    i0 = (INT)(t0+BBS_ZERO) ;
    i1 = (INT)(t1-BBS_ZERO) ;
    t0 -= (REAL)i0 ;
    t1 -= (REAL)i1 ;
    n = i1 - i0 + 2 ;
    buffer1 = c2q_create ( buffer1, n ) ;
    C2_PC_BUFFER_N(buffer1) = n ;
    if ( i0 == i1 ) 
        c2r_trim ( C2_PC_BUFFER_ARC(buffer0,i0), t0, t1, 
            C2_PC_BUFFER_ARC(buffer1,0) ) ;
    else {
        c2r_trim ( C2_PC_BUFFER_ARC(buffer0,i0), t0, 1.0, 
            C2_PC_BUFFER_ARC0(buffer1) ) ;
        for ( i=1 ; i<n-2 ; i++ ) {
            C2V_COPY ( C2_PC_BUFFER_PT(buffer0,i0+i), 
                C2_PC_BUFFER_PT(buffer1,i) ) ;
            C2_PC_BUFFER_D(buffer1,i) = C2_PC_BUFFER_D(buffer0,i0+i) ;
        }
        c2r_trim ( C2_PC_BUFFER_ARC(buffer0,i1), 0.0, t1, 
            C2_PC_BUFFER_ARC1(buffer1) ) ;
    }
    RETURN ( buffer1 ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_PC_BUFFER c2q_through ( a, n, buffer ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *a      ;   /* a[n] */
INT n ;
C2_PC_BUFFER buffer ;
{
    INT i ;

    for ( i=0 ; i<n-1 ; i++ ) {
        C2V_COPY ( a[i], C2_PC_BUFFER_PT(buffer,i) ) ;
        C2_PC_BUFFER_D(buffer,i) = 0.0 ;
    }
    C2V_COPY ( a[n-1], C2_PC_BUFFER_PT(buffer,n-1) ) ;
    RETURN ( buffer ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2q_init ( buffer, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
PT2 a ;
{
    C2V_COPY ( a, C2_PC_BUFFER_PT0(buffer) ) ;
}

