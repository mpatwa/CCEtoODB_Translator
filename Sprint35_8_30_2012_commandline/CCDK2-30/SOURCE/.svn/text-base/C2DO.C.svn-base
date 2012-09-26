/* -2 -3 */
/******************************* C2DO.C *********************************/
/************ Two-dimensional curve offset routines *********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2gdefs.h>
#include <c2ldefs.h>
#include <c2ndefs.h>
#include <c2pdefs.h>
#include <c2sdefs.h>
#include <c2apriv.h>
#include <c2cdefs.h>
#include <c2ddefs.h>
#include <c2dpriv.h>
#include <c2vdefs.h>
#include <c2zdefs.h>
#include <dmldefs.h>
#include <c2mem.h>
#include <c2vmcrs.h>

STATIC INT offset_pcurve __(( C2_CURVE DUMMY0 , REAL DUMMY1 , 
            DML_LIST DUMMY2 )) ;
#ifdef  SPLINE
STATIC  INT     offset_spline __(( C2_CURVE DUMMY0 , REAL DUMMY1 , 
            DML_LIST DUMMY2 )) ;
STATIC  INT     offset_polygon __(( C2_CURVE DUMMY0 , REAL DUMMY1 , 
            DML_LIST  DUMMY2 )) ;
STATIC  void    trim_offset_spline __(( C2_CURVE DUMMY0 , REAL DUMMY1 , 
            DML_LIST DUMMY2 , C2_CURVE* DUMMY3 , PARM DUMMY4 , PARM DUMMY5 , 
            INT* DUMMY6 )) ;
STATIC  void    copy_curve_header __(( C2_CURVE DUMMY0 , 
            C2_CURVE  DUMMY1 )) ;
#endif  /* SPLINE */
STATIC  BOOLEAN offset_curve_construct __(( C2_CURVE DUMMY0 , REAL DUMMY1 , 
            C2_CURVE  DUMMY2 )) ;

/*----------------------------------------------------------------------*/
BBS_PUBLIC INT c2d_offset ( curve, offset, offset_list ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C2_CURVE curve ;
REAL offset ;
DML_LIST offset_list ;
{
    C2_CURVE offset_curve ;

    if ( C2_CURVE_IS_LINE(curve) || C2_CURVE_IS_ARC(curve) || 
        C2_CURVE_IS_ZERO_ARC(curve) ) {
        offset_curve = c2d_offset_curve ( curve, offset ) ;
        if ( offset_curve == NULL )
            RETURN ( 0 ) ;
        dml_append_data ( offset_list, offset_curve ) ;
        RETURN ( 1 ) ;
    }
    else if ( C2_CURVE_IS_PCURVE(curve) ) 
        RETURN ( offset_pcurve ( curve, offset, offset_list ) ) ;
    else
#ifdef SPLINE
        RETURN ( offset_spline ( curve, offset, offset_list ) ) ;
#else
        RETURN ( 0 ) ;
#endif /* SPLINE */
}

#ifdef  SPLINE
/*----------------------------------------------------------------------*/
STATIC INT offset_spline ( curve, offset, offset_list ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL offset ;
DML_LIST offset_list ;
{
    INT m ;
    DML_LIST trim_list ;
    DML_ITEM item ;
    PARM_S *trim_parm ;
    C2_CURVE offset_curve ;

    if ( c2n_get_d ( C2_CURVE_NURB(curve) ) == 2 ) 
        RETURN ( offset_polygon ( curve, offset, offset_list ) ) ;

    (void) c2n_setup_curv_extr ( C2_CURVE_NURB(curve) ) ;

    trim_list = dml_create_list();

    (void) c2n_offset ( C2_CURVE_NURB(curve), C2_CURVE_PARM0(curve), 
        C2_CURVE_PARM1(curve), offset, trim_list ) ;

    m = 0 ;
    DML_WALK_LIST ( trim_list, item ) {
        trim_parm = (PARM_S*)DML_RECORD(item) ;
        trim_offset_spline ( curve, offset, offset_list, 
            &offset_curve, &(trim_parm[0]), &(trim_parm[1]), &m ) ;
        FREE ( trim_parm ) ;
    }
    dml_free_list ( trim_list ) ;

    RETURN ( m ) ;
}


/*----------------------------------------------------------------------*/
STATIC INT offset_polygon ( curve, offset, offset_list ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL offset ;
DML_LIST offset_list ;
{
    C2_CURVE offset_curve ;
    INT m, n, k ;
    PT2 *offset_pt, p, q ;
    HPT2 *ctpt0, *ctpt ;
    REAL *knot0, *knot ;
    PT2 pt0 ;
    BOOLEAN closed ;
    C2_NURB nurb ;

    nurb = C2_CURVE_NURB ( curve ) ;
    ctpt0 = c2n_get_ctpt ( nurb ) ;
    knot0 = c2n_get_knot ( nurb ) ;
    ctpt = ctpt0 ;
    knot = knot0 ;
    n = c2n_get_n ( nurb ) ;
    k = 0 ;
    offset_pt = CREATE ( n, PT2 ) ;
    closed = C2V_IDENT_PTS ( ctpt[0], ctpt[n-1] ) ;

    if ( closed ) {
        C2V_SUB ( ctpt[n-1], ctpt[n-2], p ) ;
        c2v_normalize ( p, p ) ;
        C2V_SUB ( ctpt[1], ctpt[0], q ) ;
        c2v_normalize ( q, q ) ;
        closed = c2a_offset ( ctpt[0], p, q, offset, pt0 ) ;
    }

    while ( ctpt < ctpt0 + n ) {
        m = c2s_offset_polygon ( ctpt, n, offset, offset_pt ) ;
        if ( closed && ctpt == ctpt0 ) 
            C2V_COPY ( pt0, offset_pt[0] ) ;
        if ( closed && ctpt+m == ctpt0 + n ) 
            C2V_COPY ( pt0, offset_pt[m-1] ) ;
        if ( m > 0 ) {
            offset_curve = c2d_polygon_knots ( offset_pt, m, knot ) ;
            if ( offset_curve == NULL )
                RETURN ( 0 ) ;
            dml_append_data ( offset_list, offset_curve ) ;
            knot += m ;
            ctpt += m ;
            n -= m ;
        }
        else {
            knot++ ;
            ctpt++ ;
            n-- ;
        }
        k++ ;
    }
    KILL ( offset_pt ) ;
    RETURN ( k ) ;
}


/*----------------------------------------------------------------------*/
STATIC void trim_offset_spline ( curve0, offset, offset_list, 
                offset_curve_ptr, parml, parmr, m_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C2_CURVE curve0 ;
REAL offset ;
DML_LIST offset_list ;
C2_CURVE *offset_curve_ptr ;
PARM parml, parmr ;
INT *m_ptr ;
{

    if ( *m_ptr > 0 ) 
        *offset_curve_ptr = c2d_copy ( *offset_curve_ptr ) ;
    else 
        *offset_curve_ptr = c2d_offset_curve ( curve0, offset ) ;
    c2c_trim ( *offset_curve_ptr, parml, parmr ) ;
    c2n_copy_extr_rec ( C2_CURVE_NURB(curve0), 
        C2_CURVE_NURB(*offset_curve_ptr) ) ;
    dml_append_data ( offset_list, (*offset_curve_ptr) ) ;
    (*m_ptr)++ ;
}

#endif  /* SPLINE */

/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_offset_curve ( curve0, offset ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve0 ;
REAL offset ;
{
    if ( C2_CURVE_IS_PCURVE(curve0) ) {
        DML_LIST offset_list = dml_create_list() ;
        if ( offset_list == NULL ) 
            RETURN ( NULL ) ;
        if ( c2d_offset ( curve0, offset, offset_list ) == 1 ) 
            RETURN ( (C2_CURVE)DML_FIRST_RECORD(offset_list) ) ;
        else {
            dml_destroy_list ( offset_list, ( PF_ACTION ) c2d_free_curve ) ;
            RETURN ( NULL ) ;
        }
    }
    else {
        C2_CURVE curve = c2d_alloc_curve () ;
        if ( curve != NULL ) {
            if ( !offset_curve_construct ( curve0, offset, curve ) ) {
                c2d_free_curve ( curve ) ;
                curve = NULL ;
            }
        }
        RETURN ( curve ) ;
    }
}

/*----------------------------------------------------------------------*/
STATIC BOOLEAN offset_curve_construct ( curve0, offset, curve )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve0, curve ;
REAL offset ;
{
#ifdef  SPLINE
    REAL s ;
#endif /*SPLINE*/
    if ( C2_CURVE_IS_LINE(curve0) ) {
        c2c_copy ( curve0, curve ) ;
        c2l_offset ( C2_CURVE_LINE(curve), offset, C2_CURVE_LINE(curve) ) ;
        c2l_box ( C2_CURVE_LINE(curve), C2_CURVE_T0(curve),
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
        RETURN ( TRUE ) ;
    }

    else if ( C2_CURVE_IS_ARC(curve0) ) {
        c2c_copy ( curve0, curve ) ;
        if ( c2g_offset ( C2_CURVE_ARC(curve0), offset,
            C2_CURVE_ARC(curve) ) ) {
            c2g_box ( C2_CURVE_ARC(curve), C2_CURVE_T0(curve),
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
            RETURN ( !c2g_zero_rad ( C2_CURVE_ARC(curve) ) ) ;
        }
        else
            RETURN ( FALSE ) ;
    }
    else if ( C2_CURVE_IS_ZERO_ARC(curve0) ) {
        C2_CURVE_TYPE(curve) = C2_ARC_TYPE ;
        C2_CURVE_ARC(curve) = c2g_alloc () ;
        C2_CURVE_T0(curve) = 0.0 ;
        C2_CURVE_J0(curve) = 0 ;
        C2_CURVE_T1(curve) = 1.0 ;
        C2_CURVE_J1(curve) = 0 ;
        if ( c2z_offset ( C2_CURVE_ZERO_ARC(curve0), offset, 
            C2_CURVE_ARC(curve) ) ) {
            c2g_box ( C2_CURVE_ARC(curve), C2_CURVE_T0(curve),
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
            RETURN ( !c2g_zero_rad ( C2_CURVE_ARC(curve) ) ) ;
        }
        else
            RETURN ( FALSE ) ;
    }
    else
#ifdef  SPLINE
    {
        C2_NURB nurb0, nurb ;

        copy_curve_header ( curve0, curve ) ;
        nurb0 = C2_CURVE_NURB(curve0) ;
        nurb = C2_CURVE_NURB(curve) ;
        c2n_set_w ( nurb, c2n_get_w ( nurb0 ) + offset ) ;

        s = fabs(offset) ;
        C2_CURVE_X_MIN(curve) = C2_CURVE_X_MIN(curve0) - s ;
        C2_CURVE_X_MAX(curve) = C2_CURVE_X_MAX(curve0) + s ;
        C2_CURVE_Y_MIN(curve) = C2_CURVE_Y_MIN(curve0) - s ;
        C2_CURVE_Y_MAX(curve) = C2_CURVE_Y_MAX(curve0) + s ;
        RETURN ( TRUE ) ;
    }
#else
        RETURN ( FALSE ) ;
#endif /* SPLINE */
}


#ifdef  SPLINE
/*----------------------------------------------------------------------*/
STATIC void copy_curve_header ( curve0, curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve0, curve ;
{
    C2_CURVE_NURB(curve) = c2n_create_copy_nurb ( C2_CURVE_NURB(curve0) ) ;
    C2_CURVE_TYPE(curve) = C2_CURVE_TYPE(curve0) ;
    C2_CURVE_T0(curve) = C2_CURVE_T0(curve0) ;
    C2_CURVE_J0(curve) = C2_CURVE_J0(curve0) ;
    C2_CURVE_T1(curve) = C2_CURVE_T1(curve0) ;
    C2_CURVE_J1(curve) = C2_CURVE_J1(curve0) ;
}
#endif /* SPLINE */
/*-----------------------------------------------------------------------*/
STATIC INT offset_pcurve ( curve, offset, offset_list ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
REAL offset ;
DML_LIST offset_list ;
{
    C2_CURVE offset_curve ;
    DML_ITEM item ;
    DML_LIST temp_list ;
    INT n, n1 ;
    C2_PCURVE pcurve ;

    if ( IS_SMALL(offset) ) {
        offset_curve = c2d_copy ( curve ) ;
        if ( offset_curve == NULL ) 
            RETURN ( 0 ) ;
        RETURN ( dml_append_data ( offset_list, offset_curve ) == NULL ? 
            0 : 1 ) ;
    }

    temp_list = c2p_offset ( C2_CURVE_PCURVE(curve), offset ) ;
    n = DML_LENGTH ( temp_list ) ;

    DML_WALK_LIST ( temp_list, item ) {
        pcurve = (C2_PCURVE)DML_RECORD(item) ;
        offset_curve = c2d_curve ( ) ;
        if ( offset_curve == NULL ) 
            RETURN ( 0 ) ;
        C2_CURVE_TYPE(offset_curve) = C2_PCURVE_TYPE ;
        C2_CURVE_PCURVE(offset_curve) = pcurve ;
        n1 = c2p_n ( pcurve ) ;
        C2_CURVE_T1(offset_curve) = (REAL)(n1-1) ;
        C2_CURVE_J1(offset_curve) = n1 - 2 ;
        c2p_box ( C2_CURVE_PCURVE(offset_curve), C2_CURVE_T0(offset_curve), 
            C2_CURVE_T1(offset_curve), C2_CURVE_BOX(offset_curve) ) ;

        if ( dml_append_data ( offset_list, offset_curve ) == NULL )
            RETURN ( n ) ;
        n++ ;
    }
    dml_free_list ( temp_list ) ;
    RETURN ( n ) ;
}

