/* -2 -3 -x */
/******************************* DXE0.C *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                      !!!!!!!!*/ 
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <string.h> 
#include <dmldefs.h>
#include <dxents.h>
#include <dx0defs.h>
#include <dxetmpls.h>

GLOBAL      INT             DX_ENTS_NUMBER=0 ;
GLOBAL      DXF_ENTITY      DX_ENT_BUFFER[DX_BUF_SIZE] ;

#ifdef  __ACAD11__
GLOBAL int DXF_LINE_TEMPLATE_10[]     = { DX_LINE_TEMPLATE_10 } ;
GLOBAL int DXF_POINT_TEMPLATE_10[]    = { DX_POINT_TEMPLATE_10 } ;
GLOBAL int DXF_CIRCLE_TEMPLATE_10[]   = { DX_CIRCLE_TEMPLATE_10 } ;
GLOBAL int DXF_ARC_TEMPLATE_10[]      = { DX_ARC_TEMPLATE_10 } ;
GLOBAL int DXF_TRACE_TEMPLATE_10[]    = { DX_TRACE_TEMPLATE_10 } ;
GLOBAL int DXF_SOLID_TEMPLATE_10[]    = { DX_SOLID_TEMPLATE_10 } ;
GLOBAL int DXF_TEXT_TEMPLATE_10[]     = { DX_TEXT_TEMPLATE_10 } ;
GLOBAL int DXF_SHAPE_TEMPLATE_10[]    = { DX_SHAPE_TEMPLATE_10 } ;
GLOBAL int DXF_INSERT_TEMPLATE_10[]   = { DX_INSERT_TEMPLATE_10 } ;
GLOBAL int DXF_ATTDEF_TEMPLATE_10[]   = { DX_ATTDEF_TEMPLATE_10 } ;
GLOBAL int DXF_ATTRIB_TEMPLATE_10[]   = { DX_ATTRIB_TEMPLATE_10 } ;
GLOBAL int DXF_VERTEX_TEMPLATE_10[]   = { DX_VERTEX_TEMPLATE_10 } ;
GLOBAL int DXF_PLINE_TEMPLATE_10[]    = { DX_POLYLINE_TEMPLATE_10 } ;
GLOBAL int DXF_LINE3D_TEMPLATE_10[]   = { DX_LINE3D_TEMPLATE_10 } ;
GLOBAL int DXF_3DFACE_TEMPLATE_10[]   = { DX_3DFACE_TEMPLATE_10 } ;
GLOBAL int DXF_DIM_TEMPLATE_10[]      = { DX_DIM_TEMPLATE_10 } ;

GLOBAL int DXF_LINE_TEMPLATE_11[]     = { DX_LINE_TEMPLATE_11 } ;
GLOBAL int DXF_POINT_TEMPLATE_11[]    = { DX_POINT_TEMPLATE_11 } ;
GLOBAL int DXF_CIRCLE_TEMPLATE_11[]   = { DX_CIRCLE_TEMPLATE_11 } ;
GLOBAL int DXF_ARC_TEMPLATE_11[]      = { DX_ARC_TEMPLATE_11 } ;
GLOBAL int DXF_TRACE_TEMPLATE_11[]    = { DX_TRACE_TEMPLATE_11 } ;
GLOBAL int DXF_SOLID_TEMPLATE_11[]    = { DX_SOLID_TEMPLATE_11 } ;
GLOBAL int DXF_TEXT_TEMPLATE_11[]     = { DX_TEXT_TEMPLATE_11 } ;
GLOBAL int DXF_SHAPE_TEMPLATE_11[]    = { DX_SHAPE_TEMPLATE_11 } ;
GLOBAL int DXF_INSERT_TEMPLATE_11[]   = { DX_INSERT_TEMPLATE_11 } ;
GLOBAL int DXF_ATTDEF_TEMPLATE_11[]   = { DX_ATTDEF_TEMPLATE_11 } ;
GLOBAL int DXF_ATTRIB_TEMPLATE_11[]   = { DX_ATTRIB_TEMPLATE_11 } ;
GLOBAL int DXF_VERTEX_TEMPLATE_11[]   = { DX_VERTEX_TEMPLATE_11 } ;
GLOBAL int DXF_PLINE_TEMPLATE_11[]    = { DX_POLYLINE_TEMPLATE_11 } ;
GLOBAL int DXF_LINE3D_TEMPLATE_11[]   = { DX_LINE3D_TEMPLATE_11 } ;
GLOBAL int DXF_3DFACE_TEMPLATE_11[]   = { DX_3DFACE_TEMPLATE_11 } ;
GLOBAL int DXF_DIM_TEMPLATE_11[]      = { DX_DIM_TEMPLATE_11 } ;
GLOBAL int DXF_VIEWPORT_TEMPLATE[]    = { DX_VIEWPORT_TEMPLATE } ;

#else

GLOBAL int DXF_DESC_TEMPLATE[]     = { DX_DESC_TEMPLATE } ;
GLOBAL int DXF_LINE_TEMPLATE[]     = { DX_LINE_TEMPLATE } ;
GLOBAL int DXF_POINT_TEMPLATE[]    = { DX_POINT_TEMPLATE } ;
GLOBAL int DXF_CIRCLE_TEMPLATE[]   = { DX_CIRCLE_TEMPLATE } ;
GLOBAL int DXF_ARC_TEMPLATE[]      = { DX_ARC_TEMPLATE } ;
GLOBAL int DXF_TRACE_TEMPLATE[]    = { DX_TRACE_TEMPLATE } ;
GLOBAL int DXF_SOLID_TEMPLATE[]    = { DX_SOLID_TEMPLATE } ;
GLOBAL int DXF_TEXT_TEMPLATE[]     = { DX_TEXT_TEMPLATE } ;
GLOBAL int DXF_SHAPE_TEMPLATE[]    = { DX_SHAPE_TEMPLATE } ;
GLOBAL int DXF_INSERT_TEMPLATE[]   = { DX_INSERT_TEMPLATE } ;
GLOBAL int DXF_ATTDEF_TEMPLATE[]   = { DX_ATTDEF_TEMPLATE } ;
GLOBAL int DXF_ATTRIB_TEMPLATE[]   = { DX_ATTRIB_TEMPLATE } ;
GLOBAL int DXF_VERTEX_TEMPLATE[]   = { DX_VERTEX_TEMPLATE } ;
GLOBAL int DXF_PLINE_TEMPLATE[]    = { DX_POLYLINE_TEMPLATE } ;
GLOBAL int DXF_LINE3D_TEMPLATE[]   = { DX_LINE3D_TEMPLATE } ;
GLOBAL int DXF_3DFACE_TEMPLATE[]   = { DX_3DFACE_TEMPLATE } ;
GLOBAL int DXF_DIM_TEMPLATE[]      = { DX_DIM_TEMPLATE } ;

#endif

GLOBAL char    DXF_LINE_WORD[]     = { "LINE" } ;
GLOBAL char    DXF_POINT_WORD[]    = { "POINT" } ;
GLOBAL char    DXF_CIRCLE_WORD[]   = { "CIRCLE" } ;
GLOBAL char    DXF_ARC_WORD[]      = { "ARC" } ;
GLOBAL char    DXF_TRACE_WORD[]    = { "TRACE" } ; 
GLOBAL char    DXF_SOLID_WORD[]    = { "SOLID" } ;
GLOBAL char    DXF_TEXT_WORD[]     = { "TEXT" } ;
GLOBAL char    DXF_SHAPE_WORD[]    = { "SHAPE" } ;
GLOBAL char    DXF_INSERT_WORD[]   = { "INSERT" } ;
GLOBAL char    DXF_ATTDEF_WORD[]   = { "ATTDEF" } ;
GLOBAL char    DXF_ATTRIB_WORD[]   = { "ATTRIB" } ;
GLOBAL char    DXF_VERTEX_WORD[]   = { "VERTEX" } ; 
GLOBAL char    DXF_POLYLINE_WORD[] = { "POLYLINE" } ; 
GLOBAL char    DXF_LINE3D_WORD[]   = { "3DLINE" } ; 
GLOBAL char    DXF_3DFACE_WORD[]   = { "3DFACE" } ; 
GLOBAL char    DXF_DIM_WORD[]      = { "DIMENSION" } ; 
#ifdef __ACAD11__
GLOBAL char    DXF_VIEWPORT_WORD[] = { "VIEWPORT" } ;
#endif
GLOBAL char    DXF_NULL_WORD[]     = { "\0" } ;

#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx0_alloc_entity ( entity_id, file ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY_ID entity_id ;
DXF_FILE file ;
{ 
    INT i, release = dx0_file_release ( file ) ;
    int s, *template ;
    DXF_ENTITY entity ;

    if ( entity_id == DXF_NONE_ID ) {
        dx0_set_file_status ( file, DXF_ENTITY_UNKNOWN ) ;
        RETURN ( NULL ) ;
    }

    template = dx0_entity_template ( entity_id, release ) ;
    if ( template == NULL ) {
        dx0_set_file_status ( file, DXF_ENTITY_UNKNOWN ) ;
        RETURN ( NULL ) ;
    }
    s = sizeof(DXF_ENTITY_ID) + dx0_sizeof_template ( template ) + 
        sizeof(DXF_ENAME) + sizeof(DML_LIST) + 2 * sizeof(INT) ;
    entity = (DXF_ENTITY) MALLOC ( s, char ) ;
    i = sizeof ( DXF_ENTITY );
    if ( entity == NULL ) {
        dx0_set_file_status ( file, DXF_MALLOC_FAILURE ) ;
        RETURN ( NULL ) ;
    }
    DXF_ENAME_INIT(DX_ENTITY_ENAME(entity)) ;
    DX_ENTITY_APPDAT_LIST(entity) = NULL ;
    DX_ENTITY_ID(entity) = entity_id ;
    DXF_ENTITY_LTYPE_STATUS(entity) = DXF_LTYPE_BYLAYER ;
    DXF_ENTITY_COLOR_STATUS(entity) = DXF_COLOR_BYLAYER ;
    dx0_init_template ( DX_ENTITY_ANY(entity), template ) ;
    if ( entity_id == DXF_POLYLINE ) 
        DXF_PLINE_VFLAG(entity) = 1 ;
    if ( entity_id == DXF_VIEWPORT )
        DXF_VIEWPORT_LAYER_LIST(entity) = NULL;

    RETURN ( entity ) ;
}

#else
/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx0_alloc_entity ( entity_id ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY_ID entity_id ;
{ 
    INT i ;
    int s, *template ;
    DXF_ENTITY entity ;

    if ( entity_id == DXF_NONE_ID ) 
        RETURN ( NULL ) ;

    template = dx0_entity_template ( entity_id ) ;
    if ( template == NULL ) 
        RETURN ( NULL ) ;
    s = sizeof(DXF_ENTITY_ID) + dx0_sizeof_template ( template ) + 
        sizeof(DXF_ENAME) + 2 * sizeof(INT) ;
    entity = (DXF_ENTITY) MALLOC ( s, char ) ;
    if ( entity == NULL ) 
        RETURN ( NULL ) ;
    DXF_ENAME_INIT(DX_ENTITY_ENAME(entity)) ;
    DX_ENTITY_ID(entity) = entity_id ;
    DXF_ENTITY_LTYPE_STATUS(entity) = DXF_LTYPE_BYLAYER ;
    DXF_ENTITY_COLOR_STATUS(entity) = DXF_COLOR_BYLAYER ;
    dx0_init_template ( DX_ENTITY_ANY(entity), template ) ;
    if ( entity_id == DXF_POLYLINE ) 
        DXF_PLINE_VFLAG(entity) = 1 ;
    RETURN ( entity ) ;
}
#endif

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_free_entity ( entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
{ 
    INT i ;
    if ( entity == NULL ) 
        RETURN ;
    if ( DXF_ENTITY_IS_INSERT(entity) ) {
        dml_destroy_list ( DXF_INSERT_ATTRS(entity), 
        				   ( PF_ACTION ) dxf_free_entity ) ;
        DXF_INSERT_ATTRS(entity) = NULL ;
    }
    else if ( DXF_ENTITY_IS_PLINE(entity) ) {
        dml_destroy_list ( DXF_PLINE_VLIST(entity), 
        				   ( PF_ACTION ) dxf_free_entity ) ;
        DXF_PLINE_VLIST(entity) = NULL ; 
    }
#ifdef __ACAD11__
    else if ( DXF_ENTITY_IS_VIEWPORT(entity) ) {
        dml_destroy_list ( DXF_VIEWPORT_LAYER_LIST(entity), 
        				   ( PF_ACTION ) dmm_free ) ;
        DXF_VIEWPORT_LAYER_LIST(entity) = NULL ; 
    }
    dml_destroy_list ( DX_ENTITY_APPDAT_LIST(entity), 
    				   ( PF_ACTION ) dx0_free_appdat ) ;
#endif

    dx0_free_entity ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_free_entity ( entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
{ 
    if ( entity == NULL ) 
        RETURN ;
    if ( DXF_ENTITY_IS_INSERT(entity) ) 
            dml_destroy_list ( DXF_INSERT_ATTRS(entity), 
            				   ( PF_ACTION ) dxf_free_entity ) ;
    else if ( DXF_ENTITY_IS_PLINE(entity) ) 
        dml_destroy_list ( DXF_PLINE_VLIST(entity), 
        				   ( PF_ACTION ) dxf_free_entity ) ;
#ifdef __ACAD11__
    else if ( DXF_ENTITY_IS_VIEWPORT(entity) ) 
        dml_destroy_list ( DXF_VIEWPORT_LAYER_LIST(entity), dmm_free ) ;
#endif
    FREE ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY_ID dx0_entity_id ( name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING name ;
{ 
    if ( strcmp ( name, DXF_POINT_WORD ) == 0 ) 
        RETURN ( DXF_POINT ) ;
    else if ( strcmp ( name, DXF_LINE_WORD ) == 0 ) 
        RETURN ( DXF_LINE ) ;
    else if ( strcmp ( name, DXF_CIRCLE_WORD ) == 0 ) 
        RETURN ( DXF_CIRCLE ) ;
    else if ( strcmp ( name, DXF_ARC_WORD ) == 0 ) 
        RETURN ( DXF_ARC ) ;
    else if ( strcmp ( name, DXF_TRACE_WORD ) == 0 ) 
        RETURN ( DXF_TRACE ) ;
    else if ( strcmp ( name, DXF_SOLID_WORD ) == 0 ) 
        RETURN ( DXF_SOLID ) ;
    else if ( strcmp ( name, DXF_TEXT_WORD ) == 0 ) 
        RETURN ( DXF_TEXT ) ;
    else if ( strcmp ( name, DXF_SHAPE_WORD ) == 0 ) 
        RETURN ( DXF_SHAPE ) ;
    else if ( strcmp ( name, DXF_INSERT_WORD ) == 0 ) 
        RETURN ( DXF_INSERT ) ;
    else if ( strcmp ( name, DXF_ATTDEF_WORD ) == 0 ) 
        RETURN ( DXF_ATTDEF ) ;
    else if ( strcmp ( name, DXF_ATTRIB_WORD ) == 0 ) 
        RETURN ( DXF_ATTRIB ) ;
    else if ( strcmp ( name, DXF_VERTEX_WORD ) == 0 ) 
        RETURN ( DXF_VERTEX ) ;
    else if ( strcmp ( name, DXF_POLYLINE_WORD ) == 0 ) 
        RETURN ( DXF_POLYLINE ) ;
    else if ( strcmp ( name, DXF_LINE3D_WORD ) == 0 ) 
        RETURN ( DXF_LINE3D ) ;
    else if ( strcmp ( name, DXF_3DFACE_WORD ) == 0 ) 
        RETURN ( DXF_3DFACE ) ;
    else if ( strcmp ( name, DXF_DIM_WORD ) == 0 ) 
        RETURN ( DXF_DIM ) ;
#ifdef __ACAD11__
    else if ( strcmp ( name, DXF_VIEWPORT_WORD ) == 0 ) 
        RETURN ( DXF_VIEWPORT ) ;
#endif
    else 
        RETURN ( DXF_NONE_ID ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE STRING dx0_entity_name ( entity_id ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY_ID entity_id ;
{ 
    if ( entity_id == DXF_LINE ) 
        RETURN ( DXF_LINE_WORD ) ;
    else if ( entity_id == DXF_POINT ) 
        RETURN ( DXF_POINT_WORD ) ;
    else if ( entity_id == DXF_CIRCLE ) 
        RETURN ( DXF_CIRCLE_WORD ) ;
    else if ( entity_id == DXF_ARC ) 
        RETURN ( DXF_ARC_WORD ) ;
    else if ( entity_id == DXF_TRACE ) 
        RETURN ( DXF_TRACE_WORD ) ;
    else if ( entity_id == DXF_SOLID ) 
        RETURN ( DXF_SOLID_WORD ) ;
    else if ( entity_id == DXF_TEXT ) 
        RETURN ( DXF_TEXT_WORD ) ;
    else if ( entity_id == DXF_SHAPE ) 
        RETURN ( DXF_SHAPE_WORD ) ;
    else if ( entity_id == DXF_INSERT ) 
        RETURN ( DXF_INSERT_WORD ) ;
    else if ( entity_id == DXF_ATTDEF ) 
        RETURN ( DXF_ATTDEF_WORD ) ;
    else if ( entity_id == DXF_ATTRIB ) 
        RETURN ( DXF_ATTRIB_WORD ) ;
    else if ( entity_id == DXF_VERTEX ) 
        RETURN ( DXF_VERTEX_WORD ) ;
    else if ( entity_id == DXF_POLYLINE ) 
        RETURN ( DXF_POLYLINE_WORD ) ;
    else if ( entity_id == DXF_LINE3D ) 
        RETURN ( DXF_LINE3D_WORD ) ;
    else if ( entity_id == DXF_3DFACE ) 
        RETURN ( DXF_3DFACE_WORD ) ;
    else if ( entity_id == DXF_DIM ) 
        RETURN ( DXF_DIM_WORD ) ;
#ifdef __ACAD11__
    else if ( entity_id == DXF_VIEWPORT ) 
        RETURN ( DXF_VIEWPORT_WORD ) ;
#endif
    else
        RETURN ( NULL ) ;
}


#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PRIVATE int *dx0_entity_template ( entity_id, release ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY_ID entity_id ;
INT release ;
{ 
    if ( entity_id == DXF_LINE ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_LINE_TEMPLATE_11 : DXF_LINE_TEMPLATE_10 ) ;
    else if ( entity_id == DXF_POINT ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_POINT_TEMPLATE_11 : DXF_POINT_TEMPLATE_10 ) ;
    else if ( entity_id == DXF_CIRCLE ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_CIRCLE_TEMPLATE_11 : DXF_CIRCLE_TEMPLATE_10 ) ;
    else if ( entity_id == DXF_ARC ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_ARC_TEMPLATE_11 : DXF_ARC_TEMPLATE_10 ) ;
    else if ( entity_id == DXF_TRACE ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_TRACE_TEMPLATE_11 : DXF_TRACE_TEMPLATE_10 ) ;
    else if ( entity_id == DXF_SOLID ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_SOLID_TEMPLATE_11 : DXF_SOLID_TEMPLATE_10 ) ;
    else if ( entity_id == DXF_TEXT ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_TEXT_TEMPLATE_11 : DXF_TEXT_TEMPLATE_10 ) ;
    else if ( entity_id == DXF_SHAPE ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_SHAPE_TEMPLATE_11 : DXF_SHAPE_TEMPLATE_10 ) ;
    else if ( entity_id == DXF_INSERT ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_INSERT_TEMPLATE_11 : DXF_INSERT_TEMPLATE_10 ) ;
    else if ( entity_id == DXF_ATTDEF ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_ATTDEF_TEMPLATE_11 : DXF_ATTDEF_TEMPLATE_10 ) ;
    else if ( entity_id == DXF_ATTRIB ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_ATTRIB_TEMPLATE_11 : DXF_ATTRIB_TEMPLATE_10 ) ;
    else if ( entity_id == DXF_VERTEX ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_VERTEX_TEMPLATE_11 : DXF_VERTEX_TEMPLATE_10 ) ;
    else if ( entity_id == DXF_POLYLINE ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_PLINE_TEMPLATE_11 : DXF_PLINE_TEMPLATE_10 ) ;
    else if ( entity_id == DXF_LINE3D ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_LINE3D_TEMPLATE_11 : DXF_LINE3D_TEMPLATE_10 ) ;
    else if ( entity_id == DXF_3DFACE ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_3DFACE_TEMPLATE_11 : DXF_3DFACE_TEMPLATE_10 ) ;
    else if ( entity_id == DXF_DIM ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_DIM_TEMPLATE_11 : DXF_DIM_TEMPLATE_10 ) ;
    else if ( entity_id == DXF_VIEWPORT ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_VIEWPORT_TEMPLATE : NULL ) ;
    else
        RETURN ( NULL ) ;
}
#else
/*-------------------------------------------------------------------------*/
BBS_PRIVATE int *dx0_entity_template ( entity_id ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY_ID entity_id ;
{ 
    if ( entity_id == DXF_LINE ) 
        RETURN ( DXF_LINE_TEMPLATE ) ;
    else if ( entity_id == DXF_POINT ) 
        RETURN ( DXF_POINT_TEMPLATE ) ;
    else if ( entity_id == DXF_CIRCLE ) 
        RETURN ( DXF_CIRCLE_TEMPLATE ) ;
    else if ( entity_id == DXF_ARC ) 
        RETURN ( DXF_ARC_TEMPLATE ) ;
    else if ( entity_id == DXF_TRACE ) 
        RETURN ( DXF_TRACE_TEMPLATE ) ;
    else if ( entity_id == DXF_SOLID ) 
        RETURN ( DXF_SOLID_TEMPLATE ) ;
    else if ( entity_id == DXF_TEXT ) 
        RETURN ( DXF_TEXT_TEMPLATE ) ;
    else if ( entity_id == DXF_SHAPE ) 
        RETURN ( DXF_SHAPE_TEMPLATE ) ;
    else if ( entity_id == DXF_INSERT ) 
        RETURN ( DXF_INSERT_TEMPLATE ) ;
    else if ( entity_id == DXF_ATTDEF ) 
        RETURN ( DXF_ATTDEF_TEMPLATE ) ;
    else if ( entity_id == DXF_ATTRIB ) 
        RETURN ( DXF_ATTRIB_TEMPLATE ) ;
    else if ( entity_id == DXF_VERTEX ) 
        RETURN ( DXF_VERTEX_TEMPLATE ) ;
    else if ( entity_id == DXF_POLYLINE ) 
        RETURN ( DXF_PLINE_TEMPLATE ) ;
    else if ( entity_id == DXF_LINE3D ) 
        RETURN ( DXF_LINE3D_TEMPLATE ) ;
    else if ( entity_id == DXF_3DFACE ) 
        RETURN ( DXF_3DFACE_TEMPLATE ) ;
    else if ( entity_id == DXF_DIM ) 
        RETURN ( DXF_DIM_TEMPLATE ) ;
    else
        RETURN ( NULL ) ;
}
#endif

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_entity_filter ( entity, filter ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
DXF_FILTER filter ;
{ 
    BOOLEAN status ;
    status = ( filter == NULL ) || 
        ( dx0_entity_id_filter ( DX_ENTITY_ID(entity), filter ) && 
          dx0_layer_filter ( DXF_ENTITY_LAYER(entity), filter ) && 
          dx0_color_filter ( DXF_ENTITY_COLOR(entity), filter ) ) ;
#ifdef __ACAD11__
    dx0_appdat_filter ( DX_ENTITY_APPDAT_LIST(entity), filter ) ;
#endif
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_entity_id_filter ( entity_id, filter ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY_ID entity_id ;
DXF_FILTER filter ;
{ 
    RETURN ( ( filter == NULL ) || ( entity_id & DX_FILTER_TYPE(filter) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_layer_filter ( layer, filter ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING layer ;
DXF_FILTER filter ;
{ 
    RETURN ( filter==NULL || DX_FILTER_LAYER(filter)[0]=='\0' || 
        strcmp ( layer, DX_FILTER_LAYER(filter) ) == 0 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_color_filter ( color, filter ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT color ;
DXF_FILTER filter ;
{ 
    RETURN ( filter==NULL || DX_FILTER_COLOR(filter) == DXF_ANY_COLOR 
        || color == DX_FILTER_COLOR(filter) ) ;
}

