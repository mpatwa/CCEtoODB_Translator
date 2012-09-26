/* -2 -3 */
/****************************** DX2P11S.C *******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2ddefs.h>
#include <c2ndefs.h>
#include <c2sdefs.h>
#include <dxents.h>
#include <dx2defs.h>
#include <dx2priv.h>
#ifdef __ACAD11__
#include <alsdefs.h>
#include <dmldefs.h>
#include <dx0defs.h>
#include <c2hmcrs.h>
#include <c2nmcrs.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx2_vertex_to_entity ( file, pt, bulge, flags ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
PT2 pt ;
REAL bulge ;
INT flags ;
{ 
    DXF_ENTITY vertex ;

    vertex = dx0_alloc_entity ( DXF_VERTEX, file ) ;
    if ( vertex == NULL ) 
        RETURN ( NULL ) ;
    C2V_COPY ( pt, DXF_VERTEX_PT(vertex) ) ;
    DXF_VERTEX_BULGE(vertex) = bulge ;
    DXF_VERTEX_FLAGS(vertex) = flags ;
    RETURN ( vertex ) ;
}


#ifdef SPLINE
/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx2_spline_to_entity ( file, spline, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
C2_NURB spline ;
PARM parm0, parm1 ;
{
    DXF_ENTITY pline ;
    INT i, m, n, splinesegs ;
    int k ;
    DXF_ENTITY vertex ;
    REAL *knot, h ;
    PARM_S parm ;
    PT2 pt ; 
    HPT2 *ctl_pt ;
    BOOLEAN closed ;

    if ( ( parm0 != NULL && c2n_trimmed0 ( spline, parm0 ) ) || 
         ( parm1 != NULL && c2n_trimmed1 ( spline, parm1 ) ) ) {

        C2_NURB tspline = c2n_trim ( spline, parm0, parm1 ) ;
        pline = dx2_spline_to_entity ( file, tspline, NULL, NULL ) ;
        c2n_free_nurb ( tspline ) ;
        RETURN ( pline ) ;
    }

    pline = dx0_alloc_entity ( DXF_POLYLINE, file ) ;
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
        n -= ( C2_NURB_D(spline) - 1 ) ;
        ctl_pt = c2n_clsd_ctlpts ( spline ) ;
        if ( ctl_pt == NULL ) 
            ctl_pt = C2_NURB_CTPT(spline) ;
    }
    else
        ctl_pt = C2_NURB_CTPT(spline) ;

    DXF_PLINE_VLIST(pline) = dml_create_list();
    if ( DXF_PLINE_VLIST(pline) == NULL )
        RETURN ( NULL ) ;

    C2H_HPT_PT ( ctl_pt[0], pt ) ;
    vertex = dx2_vertex_to_entity ( file, pt, 0.0, DXF_VERTEX_SPL_FR ) ;
    if ( vertex == NULL ) 
        RETURN ( NULL ) ;
    if ( dml_append_data ( DXF_PLINE_VLIST(pline), vertex ) == NULL )
        RETURN ( NULL ) ;

    splinesegs = dxf_get_splinesegs ( file ) ;
    h = 1.0 / (REAL)splinesegs ;
    m = splinesegs * ( C2_NURB_N(spline) - C2_NURB_D(spline) + 1 ) ;
    PARM_J(&parm) = 0 ;
    if ( closed ) 
        m-- ;

    for ( i=0, PARM_T(&parm) = 0.0 ; i <= m ; i++, PARM_T(&parm) += h ) {
        c2s_pt_tan ( C2_NURB_CTPT(spline), C2_NURB_N(spline), 
            C2_NURB_D(spline), knot, 0.0, &parm, pt, NULL ) ;
        vertex = dx2_vertex_to_entity ( file, pt, 0.0, DXF_VERTEX_SPL_FIT ) ;
        if ( vertex == NULL ) 
            RETURN ( NULL ) ;
        if ( dml_append_data ( DXF_PLINE_VLIST(pline), vertex ) == NULL )
            RETURN ( NULL ) ;
    }

    for ( i=1 ; i<n ; i++ ) {
        C2H_HPT_PT ( ctl_pt[i], pt ) ;
        vertex = dx2_vertex_to_entity ( file, pt, 0.0, DXF_VERTEX_SPL_FR ) ;
        if ( vertex == NULL ) 
            RETURN ( NULL ) ;
        if ( dml_append_data ( DXF_PLINE_VLIST(pline), vertex ) == NULL )
            RETURN ( NULL ) ;
    }
    if ( closed )
    {
        FREE ( ctl_pt ) ;
        ctl_pt = NULL ;
    }
    FREE ( knot ) ;
    RETURN ( pline ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx2_ellnurb_to_entity ( file, ellipse, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
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

    RETURN ( dx2_ellipse_to_entity ( file, ctr, major_axis, minor_axis, angle, 
        t0, t1 ) ) ;                       
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx2_ellipse_to_entity ( file, ctr, 
        major_axis, minor_axis, angle, t0, t1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
PT2 ctr ;
REAL major_axis, minor_axis, angle, t0, t1 ;
{
    C2_CURVE pcurve ;
    DXF_ENTITY entity ;

    pcurve = c2d_ellipse_to_pcurve ( ctr, major_axis, minor_axis, 
        angle, t0, t1 ) ;
    entity = dx2_curve_to_entity ( file, pcurve ) ;
    DXF_PLINE_FLAGS(entity) = DXF_PLINE_FLAGS(entity) & ~DXF_PLINE_FIT ;
    c2d_free_curve ( pcurve ) ;
    RETURN ( entity ) ;
}
#endif /*SPLINE*/
#endif /*__ACAD11__*/

