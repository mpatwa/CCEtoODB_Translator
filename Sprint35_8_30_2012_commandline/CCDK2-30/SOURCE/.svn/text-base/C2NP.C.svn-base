/* -2 -3 */
/********************************** C2NP.C *********************************/
/********************************** Nurbs **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef  SPLINE
#include <c2gdefs.h>
#include <c2ndefs.h>
#include <c2sdefs.h>
#include <c2gmcrs.h>
#include <c2lmcrs.h>
#include <c2nmcrs.h>
#include <c2pmcrs.h>
#include <c2qmcrs.h>
#include <c2vmcrs.h>
#include <dmldefs.h>
STATIC INT c2n_pc_buf_dn __(( C2_PC_BUFFER )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_NURB c2n_pcurve ( pcurve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
{
    INT n ;
    C2_NURB nurb ;
    DML_ITEM item ;

    n = c2n_pc_buf_dn ( C2_PCURVE_BUFFER(pcurve) ) ;
    C2_WALK_PCURVE ( pcurve, item ) 
        n += c2n_pc_buf_dn ( DML_RECORD(item) ) ;

    n = 2 * n - 1 ;
    nurb = c2n_create_nurb ( n, 3 ) ;
    c2s_pcurve ( pcurve, C2_NURB_CTPT_PTR(nurb), C2_NURB_KNOT_PTR(nurb) ) ;
    RETURN ( nurb ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT c2n_pc_buf_dn ( buffer ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
{
    INT i, n = C2_PC_BUFFER_N(buffer) ;

    for ( i=0 ; i<C2_PC_BUFFER_N(buffer) ; i++ ) {
        if ( fabs(C2_PC_BUFFER_D(buffer,i)) >= 1.0 - BBS_ZERO ) 
            n++ ;
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_NURB c2n_line ( line ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
{
    C2_NURB nurb = c2n_create_nurb ( 2, 2 ) ;
    if ( nurb == NULL ) 
        RETURN ( NULL ) ;
    C2V_COPY ( C2_LINE_PT0(line), C2_NURB_CTPT(nurb)[0] ) ;
    C2_NURB_CTPT(nurb)[0][2] = 1.0 ;
    C2V_COPY ( C2_LINE_PT1(line), C2_NURB_CTPT(nurb)[1] ) ;
    C2_NURB_CTPT(nurb)[1][2] = 1.0 ;
    C2_NURB_KNOT(nurb)[0] = C2_NURB_KNOT(nurb)[1] = 0.0 ; 
    C2_NURB_KNOT(nurb)[2] = C2_NURB_KNOT(nurb)[3] = 1.0 ; 
    RETURN ( nurb ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_NURB c2n_arc ( arc, full_circle ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
BOOLEAN full_circle ;
{
    INT n ;
    C2_NURB nurb ;

    n = full_circle ? 9 : 
        ( fabs ( c2g_get_d ( arc ) ) < 1.0-BBS_ZERO ? 3 : 5 ) ;

    nurb = c2n_create_nurb ( n, 3 ) ;
    if ( nurb == NULL ) 
        RETURN ( NULL ) ;
    (void)c2g_bezs ( arc, full_circle, C2_NURB_CTPT(nurb) ) ;

    C2_NURB_KNOT(nurb)[0] = C2_NURB_KNOT(nurb)[1] = 
        C2_NURB_KNOT(nurb)[2] = 0.0 ;
    if ( n==3 ) 
        C2_NURB_KNOT(nurb)[3] = C2_NURB_KNOT(nurb)[4] = 
            C2_NURB_KNOT(nurb)[5] = 1.0 ;
    else {
        C2_NURB_KNOT(nurb)[3] = C2_NURB_KNOT(nurb)[4] = 0.5 ;
        C2_NURB_KNOT(nurb)[5] = C2_NURB_KNOT(nurb)[6] = 1.0 ;
        if ( n==5 ) 
            C2_NURB_KNOT(nurb)[7] = 1.0 ;
        else { 
            C2_NURB_KNOT(nurb)[7] = C2_NURB_KNOT(nurb)[8] = 1.5 ;
            C2_NURB_KNOT(nurb)[9] = C2_NURB_KNOT(nurb)[10] = 
                C2_NURB_KNOT(nurb)[11] = 2.0 ;
        }
    }
    RETURN ( nurb ) ;
}


/*--------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2n_get_ellipse_data ( nurb, ctr, 
            major_axis, minor_axis, angle ) 
/*--------------------------------------------------------------------*/
C2_NURB nurb ;
PT2 ctr ;
REAL *major_axis, *minor_axis, *angle ;
{
    RETURN ( c2s_get_ellipse_data ( C2_NURB_CTPT(nurb), 
            ctr, major_axis, minor_axis, angle ) ) ;
}
#endif  /*SPLINE*/

