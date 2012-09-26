/* -2 -3 */
/******************************* DX2G2.C ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2pdefs.h>
#include <c2qdefs.h>
#include <c2ndefs.h>
#include <c2dpriv.h>
#include <c2ddefs.h>
#include <dmldefs.h>
#include <dxents.h>
#include <dx2defs.h>
#include <dx2priv.h>
#include <c2hmcrs.h>
#include <c2nmcrs.h>
#include <c2pmcrs.h>
#include <c2qmcrs.h>
#include <c2vmcrs.h>

STATIC C2_CURVE dx2_pline_to_pcurve __(( DXF_ENTITY )) ;
STATIC C2_PC_BUFFER dx2_pline_to_pc_buffer __(( DML_ITEM*, INT*, 
            C2_PC_BUFFER, DXF_ENTITY )) ;
STATIC C2_CURVE dx2_pface_append_vtx __(( DML_ITEM*, INT, C2_CURVE )) ;
#ifdef SPLINE
STATIC C2_CURVE dx2_pline_to_spline __(( DXF_ENTITY )) ;
STATIC void dx2_pline_to_nurb __(( DXF_ENTITY, C2_NURB, BOOLEAN )) ;
#endif /*SPLINE*/
/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_CURVE dx2_pline_to_curve ( pline ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY pline ;
{
    if ( DXF_PLINE_TYPE(pline) == DXF_PLINE_NSMOOTH )
        RETURN ( dx2_pline_to_pcurve ( pline ) ) ;
    else
#ifdef SPLINE
        RETURN ( dx2_pline_to_spline ( pline ) ) ;
#else
        RETURN ( NULL ) ;
#endif /*SPLINE*/
}


/*-------------------------------------------------------------------------*/
STATIC  C2_CURVE dx2_pline_to_pcurve ( pline ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY pline ;
{
    BOOLEAN closed ;
    C2_CURVE curve ;
    C2_PCURVE pcurve ;
    DML_ITEM item ;
    INT n ;
    DXF_ENTITY last_vtx ;
    C2_PC_BUFFER buffer ;

    closed = (BOOLEAN) ( DXF_PLINE_FLAGS(pline) & 1 ) ;
    n = DML_LENGTH ( DXF_PLINE_VLIST(pline) ) ; 
    if ( closed ) {
        n++ ;
        last_vtx = (DXF_ENTITY)dml_first_record ( DXF_PLINE_VLIST(pline) ) ;
    }
    else
        last_vtx = (DXF_ENTITY)dml_last_record ( DXF_PLINE_VLIST(pline) ) ;

    curve = c2d_pcurve_frame ( 0, 0 ) ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    C2_CURVE_T0(curve) = 0.0 ;
    C2_CURVE_J0(curve) = 0 ;
    C2_CURVE_T1(curve) = (REAL)(n-1) ;
    C2_CURVE_J1(curve) = n-2 ;
    pcurve = C2_CURVE_PCURVE(curve) ;
    buffer = C2_PCURVE_BUFFER(pcurve) ;
    item = DML_FIRST ( DXF_PLINE_VLIST(pline) ) ;
    n-- ;
    dx2_pline_to_pc_buffer ( &item, &n, C2_PCURVE_BUFFER(pcurve), last_vtx ) ;
    while ( item != NULL && n > 0 ) {
        buffer = dx2_pline_to_pc_buffer ( &item, &n, NULL, last_vtx ) ;
        if ( buffer != NULL ) {
            if ( C2_PCURVE_BLIST(pcurve) == NULL ) 
                C2_PCURVE_BLIST(pcurve) = dml_create_list () ;
            dml_append_data ( C2_PCURVE_BLIST(pcurve), buffer ) ;
        }
    }
    c2p_box ( pcurve, C2_CURVE_T0(curve), 
        C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
#ifdef DEBUG
{
    PT2 p1, p2 ;
    c2p_pt_tan ( pcurve, 0.5, p1, NULL ) ;
    c2p_pt_tan ( pcurve, 1.5, p2, NULL ) ;
}
#endif
    RETURN ( curve ) ;
}


/*-------------------------------------------------------------------------*/
STATIC C2_PC_BUFFER dx2_pline_to_pc_buffer ( item_ptr, n_ptr, buffer, vtx0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM *item_ptr ;
INT *n_ptr ;
C2_PC_BUFFER buffer ;
DXF_ENTITY vtx0 ;
{
    DXF_ENTITY vertex ;
    INT i, n ;

    if ( *n_ptr <= C2_PC_BUFFER_MAXSIZE ) {
        n = *n_ptr ;
        if ( vtx0 != NULL ) 
            n++ ;
    }
    else {
        n = C2_PC_BUFFER_MAXSIZE ;
        vtx0 = NULL ;
    }
    if ( n <= 0 ) 
        RETURN ( NULL ) ;
    buffer = c2q_create ( buffer, n ) ;
    for ( i=0 ; i < n-1 ; i++, *item_ptr = DML_NEXT(*item_ptr) ) {
        vertex = (DXF_ENTITY)DML_RECORD(*item_ptr) ;
        C2V_COPY ( DXF_VERTEX_PT(vertex), C2_PC_BUFFER_PT(buffer,i) ) ;
        C2_PC_BUFFER_D(buffer,i) = DXF_VERTEX_BULGE(vertex) ;
        C2_PC_BUFFER_N(buffer)++ ;
        (*n_ptr)-- ;
    }
    if ( vtx0 == NULL ) {
        vertex = (DXF_ENTITY)DML_RECORD(*item_ptr) ;
        C2V_COPY ( DXF_VERTEX_PT(vertex), C2_PC_BUFFER_PT(buffer,i) ) ;
    }
    else {
        C2V_COPY ( DXF_VERTEX_PT(vtx0), C2_PC_BUFFER_PT(buffer,i) ) ;
        *item_ptr = NULL ;
    }
    C2_PC_BUFFER_N(buffer)++ ;
    RETURN ( buffer ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dx2_pface_to_curves ( pface, curves_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY pface ;
DML_LIST curves_list ;
{ 
    C2_CURVE curve ;
    DML_ITEM item, *item_array ;
    DXF_ENTITY vertex ;
    INT vtx_pface, vtx_3dmesh ;

    if ( pface == NULL ) 
        RETURN ( NULL ) ;
    if ( !DXF_ENTITY_IS_PLINE(pface) )
        RETURN ( NULL ) ;
    if ( !( DXF_PLINE_FLAGS(pface) & DXF_PLINE_IS_PFACE ) )
        RETURN ( NULL ) ;
    if ( curves_list == NULL )
        curves_list = dml_create_list () ;

    item_array = dml_array_of_items ( DXF_PLINE_VLIST(pface) ) ;
    curve = NULL ;

    DML_WALK_LIST ( DXF_PLINE_VLIST(pface), item ) {
        vertex = (DXF_ENTITY)DML_RECORD(item) ;
        vtx_pface = ( DXF_VERTEX_FLAGS(vertex) & DXF_VERTEX_3D_PFACE ) ;
        vtx_3dmesh = ( DXF_VERTEX_FLAGS(vertex) & DXF_VERTEX_3D_MESH ) ;
        if ( ( vtx_pface != 0 ) && ( vtx_3dmesh == 0 ) ) 
        {
            if ( DXF_VERTEX_I_71(vertex) != -1 )           /* new curve */
            {
                if ( curve != NULL )
                {
                    c2d_pcurve_close ( curve ) ;
                    dml_append_data ( curves_list, curve ) ;
                    curve = NULL ;
                }
                curve = dx2_pface_append_vtx ( item_array, 
                    DXF_VERTEX_I_71(vertex), curve ) ;
            }
            curve = dx2_pface_append_vtx ( item_array, 
                DXF_VERTEX_I_72(vertex), curve ) ;
            curve = dx2_pface_append_vtx ( item_array, 
                DXF_VERTEX_I_73(vertex), curve ) ;
            curve = dx2_pface_append_vtx ( item_array, 
                DXF_VERTEX_I_74(vertex), curve ) ;
        }
    }
    if ( curve != NULL )
    {
        c2d_pcurve_close ( curve ) ;
        dml_append_data ( curves_list, curve ) ;
        curve = NULL ;
    }
    FREE ( item_array ) ;
    RETURN ( curves_list ) ;
}

/*-------------------------------------------------------------------------*/
STATIC C2_CURVE dx2_pface_append_vtx ( item_array, i, curve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM *item_array ;
INT i ;
C2_CURVE curve ;
{ 
    DML_ITEM item ;
    DXF_ENTITY vtx ;

    if ( i <= 0 )
        RETURN (  curve ) ;
    item = item_array[i-1] ;
    vtx = DML_RECORD(item) ;

    if ( curve == NULL )
        curve = c2d_pcurve_init ( DXF_VERTEX_PT(vtx) ) ;
    else
        curve = c2d_pcurve_add_line ( curve, DXF_VERTEX_PT(vtx) ) ;
    RETURN ( curve ) ;
}


#ifdef SPLINE
/*-------------------------------------------------------------------------*/
STATIC void dx2_pline_to_nurb ( pline, nurb, closed ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY pline ;
C2_NURB nurb ;
BOOLEAN closed ;
{
    DML_ITEM item ;
    INT n ;
    DXF_ENTITY vertex ;

    n = 0 ;
    DML_WALK_LIST ( DXF_PLINE_VLIST(pline), item ) {
        vertex = (DXF_ENTITY)DML_RECORD(item) ;
        if ( DXF_VERTEX_FLAGS(vertex) & DXF_VERTEX_SPL_FR ) {
            C2V_COPY ( DXF_VERTEX_PT(vertex), C2_NURB_CTPT(nurb)[n] ) ;
            C2_NURB_CTPT(nurb)[n][2] = 1.0 ;
            n++ ;
        }
    }

    if ( closed ) 
        c2n_convert_clsd ( nurb ) ;
}


/*-------------------------------------------------------------------------*/
STATIC  C2_CURVE dx2_pline_to_spline ( pline ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY pline ;
{
    BOOLEAN closed ;
    C2_CURVE spline ;
    DML_ITEM item ;
    INT d, n ;
    DXF_ENTITY vertex ;

    closed = (BOOLEAN) ( DXF_PLINE_FLAGS(pline) & 1 ) ;
    if ( DXF_PLINE_TYPE(pline) == DXF_PLINE_QUADR ) 
        d = 3 ;
    else if ( DXF_PLINE_TYPE(pline) == DXF_PLINE_CUBIC ) 
        d = 4 ;
    else 
        RETURN ( NULL ) ;

    n = closed ? d - 1 : 0 ;
    DML_WALK_LIST ( DXF_PLINE_VLIST(pline), item ) {
        vertex = (DXF_ENTITY)DML_RECORD(item) ;
        if ( DXF_VERTEX_FLAGS(vertex) & DXF_VERTEX_SPL_FR ) 
            n++ ;
    }

    spline = c2d_spline_frame ( n, d, C2_CLSC_UNI ) ;
    if ( spline == NULL ) 
        RETURN ( NULL ) ;

    dx2_pline_to_nurb ( pline, C2_CURVE_NURB(spline), closed ) ;
    c2n_box ( C2_CURVE_NURB(spline), C2_CURVE_BOX(spline) ) ;
    RETURN ( spline ) ;
}
#endif /*SPLINE*/

