/* -2 -3 */
/****************************** DX2P10S.C *******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dxents.h>
#include <dx2defs.h>
#include <dx2priv.h>
#ifndef __ACAD11__
#include <alsdefs.h>
#include <c2ddefs.h>
#include <c2ndefs.h>
#include <c2sdefs.h>
#include <dmldefs.h>
#include <dx0defs.h>
#include <c2hmcrs.h>
#include <c2nmcrs.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx2_vertex_to_entity ( pt, bulge, flags ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt ;
REAL bulge ;
INT flags ;
{ 
    DXF_ENTITY vertex ;

    vertex = dx0_alloc_entity ( DXF_VERTEX ) ;
    if ( vertex == NULL ) 
        RETURN ( NULL ) ;
    C2V_COPY ( pt, DXF_VERTEX_PT(vertex) ) ;
    DXF_VERTEX_BULGE(vertex) = bulge ;
    DXF_VERTEX_FLAGS(vertex) = flags ;
    RETURN ( vertex ) ;
}

#ifdef SPLINE
/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx2_spline_to_entity ( spline, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB spline ;
PARM parm0, parm1 ;
{
    DXF_ENTITY pline ;
    INT i, m, n ;
    DXF_ENTITY vertex ;
    REAL *knot, h ;
    PARM_S parm ;
    PT2 pt ; 
    int k ;
    HPT2 *ctl_pt ;
    BOOLEAN closed ;

    if ( ( parm0 != NULL && c2n_trimmed0 ( spline, parm0 ) ) ||
         ( parm1 != NULL && c2n_trimmed1 ( spline, parm1 ) ) ) {

        C2_NURB tspline = c2n_trim ( spline, parm0, parm1 ) ;
        pline = dx2_spline_to_entity ( tspline, NULL, NULL ) ;
        c2n_free_nurb ( tspline ) ;
        RETURN ( pline ) ;
    }

    pline = dx0_alloc_entity ( DXF_POLYLINE ) ;
    if ( C2_NURB_D(spline) == 3 ) {
        DXF_PLINE_FLAGS(pline) |= DXF_PLINE_SPL_FIT ;
        DXF_PLINE_TYPE(pline) = DXF_PLINE_QUADR ;
    }
    else if ( C2_NURB_D(spline) == 4 ) {
        DXF_PLINE_FLAGS(pline) |= DXF_PLINE_SPL_FIT ;
        DXF_PLINE_TYPE(pline) = DXF_PLINE_CUBIC ;
    }
    else if ( C2_NURB_D(spline) != 2 ) 
        RETURN ( NULL ) ;
    if ( pline == NULL ) 
        RETURN ( NULL ) ;

    n = C2_NURB_N(spline) ;
    k = (int) ( n + C2_NURB_D(spline) ) ;
    knot = MALLOC ( k, REAL) ;
    als_clsc_uni_knots ( n, C2_NURB_D(spline), knot ) ;

    closed = c2n_closed ( spline ) ;

    if ( closed ) {
        DXF_PLINE_FLAGS(pline) |= DXF_PLINE_CLSD ;
        n-- ;
        ctl_pt = c2n_clsd_ctlpts ( spline ) ;
    }
    else
        ctl_pt = C2_NURB_CTPT(spline) ;

    DXF_PLINE_VLIST(pline) = dml_create_list();
    if ( DXF_PLINE_VLIST(pline) == NULL )
        RETURN ( NULL ) ;

    C2H_HPT_PT ( ctl_pt[0], pt ) ;
    vertex = dx2_vertex_to_entity ( pt, 0.0, DXF_VERTEX_SPL_FR ) ;
    if ( vertex == NULL ) 
        RETURN ( NULL ) ;
    if ( dml_append_data ( DXF_PLINE_VLIST(pline), vertex ) == NULL )
        RETURN ( NULL ) ;

    h = 1.0 / (REAL)DXF_SPLINESEGS ;
    m = DXF_SPLINESEGS * ( n - C2_NURB_D(spline) + 1 ) ;
    PARM_J(&parm) = 0 ;

    for ( i=0, PARM_T(&parm) = 0.0 ; i <= m ; i++, PARM_T(&parm) += h ) {
        c2s_pt_tan ( ctl_pt, n, C2_NURB_D(spline), knot, 0.0, 
            &parm, pt, NULL ) ;
        vertex = dx2_vertex_to_entity ( pt, 0.0, DXF_VERTEX_SPL_FIT ) ;
        if ( vertex == NULL ) 
            RETURN ( NULL ) ;
        if ( dml_append_data ( DXF_PLINE_VLIST(pline), vertex ) == NULL )
            RETURN ( NULL ) ;
    }

    for ( i=1 ; i<n ; i++ ) {
        C2H_HPT_PT ( ctl_pt[i], pt ) ;
        vertex = dx2_vertex_to_entity ( pt, 0.0, DXF_VERTEX_SPL_FR ) ;
        if ( vertex == NULL ) 
            RETURN ( NULL ) ;
        if ( dml_append_data ( DXF_PLINE_VLIST(pline), vertex ) == NULL )
            RETURN ( NULL ) ;
    }
    if ( closed )
        FREE ( ctl_pt ) ;
    FREE ( knot ) ;
    RETURN ( pline ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx2_ellnurb_to_entity ( ellipse, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB ellipse ;
PARM parm0, parm1 ;
{
    PT2 ctr ;
    REAL major_axis, minor_axis, angle, t0, t1 ;

    if ( !c2n_get_ellipse_data ( ellipse, ctr, &major_axis, 
        &minor_axis, &angle ) )
        RETURN ( NULL ) ;

    if ( ( parm0 == NULL ) || IS_ZERO ( PARM_T(parm0) ) )
        t0 = 0.0 ;
    else
        t0 = c2n_ellipse_t ( ellipse, parm0 ) ;
    if ( ( parm1 == NULL ) || IS_ZERO ( PARM_T(parm1) - 2.0 ) )
        t1 = TWO_PI ;
    else
        t1 = c2n_ellipse_t ( ellipse, parm1 ) ;

    RETURN ( dx2_ellipse_to_entity ( ctr, major_axis, minor_axis, angle, 
        t0, t1 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx2_ellipse_to_entity ( ctr, major_axis, minor_axis, 
            angle, t0, t1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr ;
REAL major_axis, minor_axis, angle, t0, t1 ;
{
    C2_CURVE pcurve ;
    DXF_ENTITY entity ;

    pcurve = c2d_ellipse_to_pcurve ( ctr, major_axis, minor_axis, 
        angle, t0, t1 ) ;
    entity = dx2_curve_to_entity ( pcurve ) ;
    DXF_PLINE_FLAGS(entity) = DXF_PLINE_FLAGS(entity) & ~DXF_PLINE_FIT ;
    c2d_free_curve ( pcurve ) ;
    RETURN ( entity ) ;
}
#endif /*SPLINE*/
#endif

