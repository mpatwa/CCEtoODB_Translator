/* -2 -3 */
/****************************** DX2P10.C ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dxents.h>
#include <dx2defs.h>
#include <dx2priv.h>
#ifndef __ACAD11__
#include <c2cdefs.h>
#include <c2pdefs.h>
#include <dmldefs.h>
#include <dx0defs.h>
#include <c2pmcrs.h>
#include <c2qmcrs.h>
#include <c2vmcrs.h>

STATIC DXF_ENTITY dx2_line_to_entity __(( C2_CURVE )) ;
STATIC DXF_ENTITY dx2_arc_to_entity __(( C2_CURVE )) ;
STATIC DXF_ENTITY dx2_pcurve_to_entity __(( C2_PCURVE, REAL, REAL )) ;
STATIC DXF_ENTITY dx2_pc_buffer_to_entity __(( C2_PC_BUFFER, DXF_ENTITY )) ;

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dx2_point_to_entity ( pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt ;
{ 
    DXF_ENTITY entity ;
    entity = dx0_alloc_entity ( DXF_POINT ) ;
    if ( entity == NULL ) 
        RETURN ( NULL ) ;
    C2V_COPY ( pt, DXF_POINT_PT(entity) ) ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dx2_curve_to_entity ( curve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
{ 
    if ( C2_CURVE_IS_LINE(curve) )
        RETURN ( dx2_line_to_entity ( curve ) ) ;
    else if ( C2_CURVE_IS_ARC(curve) )
        RETURN ( dx2_arc_to_entity ( curve ) ) ;
    else if ( C2_CURVE_IS_PCURVE(curve) ) 
        RETURN ( dx2_pcurve_to_entity ( C2_CURVE_PCURVE(curve), 
            C2_CURVE_T0(curve), C2_CURVE_T1(curve) ) ) ;
#ifdef SPLINE
    else if ( C2_CURVE_IS_ELLIPSE(curve) )
        RETURN ( dx2_ellnurb_to_entity ( C2_CURVE_NURB(curve), 
            C2_CURVE_PARM0(curve),  C2_CURVE_PARM1(curve) ) ) ;
    else if ( C2_CURVE_IS_SPLINE(curve) || C2_CURVE_IS_BEZIER(curve) )
        RETURN ( dx2_spline_to_entity ( C2_CURVE_NURB(curve), 
            C2_CURVE_PARM0(curve),  C2_CURVE_PARM1(curve) ) ) ;
#endif
    else
        RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DXF_ENTITY dx2_line_to_entity ( line ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE line ;
{ 
    DXF_ENTITY entity ;

    entity = dx0_alloc_entity ( DXF_LINE ) ;
    if ( entity == NULL ) 
        RETURN ( NULL ) ;
    c2c_ept0 ( line, DXF_LINE_PT0(entity) ) ;
    c2c_ept1 ( line, DXF_LINE_PT1(entity) ) ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DXF_ENTITY dx2_arc_to_entity ( curve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
{ 
    DXF_ENTITY entity ;
    PT2 ctr ;
    REAL rad, ang0, sweep ;
    INT dir ;

    if ( !c2c_get_arc_data ( curve, ctr, &rad, &ang0, &sweep, &dir ) ) {
        entity = dx0_alloc_entity ( DXF_LINE ) ;
        if ( entity == NULL ) 
            RETURN ( NULL ) ;
        c2c_ept0 ( curve, DXF_LINE_PT0(entity) ) ;
        c2c_ept1 ( curve, DXF_LINE_PT1(entity) ) ;
        RETURN ( entity ) ;
    }
    if ( sweep <= TWO_PI - BBS_ZERO ) {
        entity = dx0_alloc_entity ( DXF_ARC ) ;
        if ( entity == NULL ) 
            RETURN ( NULL ) ;
        C2V_COPY ( ctr, DXF_ARC_CTR(entity) ) ;
        DXF_ARC_RAD(entity) = rad ;
        if ( dir == 1 ) {
            DXF_ARC_ANG0(entity) = ang0 / PI_OVER_180 ;
            DXF_ARC_ANG1(entity) = ( ang0 + sweep ) / PI_OVER_180 ;
        }
        else {
            DXF_ARC_ANG0(entity) = ( ang0 - sweep ) / PI_OVER_180 ;
            DXF_ARC_ANG1(entity) = ang0 / PI_OVER_180 ;
        }
    }
    else {
        entity = dx0_alloc_entity ( DXF_CIRCLE ) ;
        if ( entity == NULL ) 
            RETURN ( NULL ) ;
        C2V_COPY ( ctr, DXF_CIRCLE_CTR(entity) ) ;
        DXF_CIRCLE_RAD(entity) = rad ;
    }
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DXF_ENTITY dx2_pcurve_to_entity ( pcurve, t0, t1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve ;
REAL t0, t1 ;
{ 
    DXF_ENTITY pline ;
    BOOLEAN closed ;
    DXF_ENTITY vertex ;
    PT2 p ;
    DML_ITEM item ;

    if ( t0 > BBS_ZERO || t1 < (REAL) c2p_n(pcurve) - 1.0 - BBS_ZERO ) {
        C2_PCURVE tcurve = c2p_trim ( pcurve, t0, t1 ) ;
        pline = dx2_pcurve_to_entity ( tcurve, 
            0.0, (REAL) c2p_n(tcurve) - 1.0 ) ;
        c2p_free ( tcurve ) ;
        RETURN ( pline ) ;
    }
    pline = dx0_alloc_entity ( DXF_POLYLINE ) ;
    if ( pline == NULL ) 
        RETURN ( NULL ) ;

    closed = c2p_closed ( pcurve ) ;
    if ( closed ) 
        DXF_PLINE_FLAGS(pline) |= DXF_PLINE_CLSD ;

    DXF_PLINE_VLIST(pline) = dml_create_list();
    if ( DXF_PLINE_VLIST(pline) == NULL )
        RETURN ( NULL ) ;
    dx2_pc_buffer_to_entity ( C2_PCURVE_BUFFER(pcurve), pline ) ;
    C2_WALK_PCURVE ( pcurve, item ) 
        dx2_pc_buffer_to_entity ( DML_RECORD(item), pline ) ;

    if ( !closed && c2p_ept1 ( pcurve, p ) ) {
        vertex = dx2_vertex_to_entity ( p, 0.0, 0 ) ;
        if ( vertex == NULL ) 
            RETURN ( NULL ) ;
        if ( dml_append_data ( DXF_PLINE_VLIST(pline), vertex ) == NULL )
            RETURN ( NULL ) ;
    }
    RETURN ( pline ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DXF_ENTITY dx2_pc_buffer_to_entity ( buffer, pline ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
DXF_ENTITY pline ;
{ 
    INT i ;
    DXF_ENTITY vertex ;

    for ( i=0 ; i<C2_PC_BUFFER_N(buffer)-1 ; i++ ) {
        if ( !IS_SMALL(C2_PC_BUFFER_D(buffer,i)) )
            DXF_PLINE_FLAGS(pline) |= DXF_PLINE_FIT ; /*Curve-fit vertices*/
        vertex = dx2_vertex_to_entity ( C2_PC_BUFFER_PT(buffer,i), 
            C2_PC_BUFFER_D(buffer,i), 0 ) ;
        if ( vertex == NULL ) 
            RETURN ( NULL ) ;
        if ( dml_append_data ( DXF_PLINE_VLIST(pline), vertex ) == NULL )
            RETURN ( NULL ) ;
    }
    RETURN ( pline ) ;
}
#endif

