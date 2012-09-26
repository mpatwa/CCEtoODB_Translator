/* -2 -3 -x */
/******************************* DXEG.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#include <dmldefs.h>
#include <dxents.h>
#include <dx0defs.h>
#include <dxfilm.h>
#include <dxpos.h>

STATIC BOOLEAN dx0_pline_vtcs __(( DXF_FILE, DXF_ENTITY )) ;
STATIC BOOLEAN dx0_insert_attrs __(( DXF_FILE, DXF_ENTITY )) ;
STATIC DXF_ENTITY dx0_get_entity1 __(( DXF_FILE, BOOLEAN* )) ;
STATIC BOOLEAN dx0_get_entity2 __(( DXF_FILE, DXF_ENTITY )) ;
STATIC DXF_ENTITY dx0_get_entity0 __(( DXF_FILE, BOOLEAN* )) ;
STATIC DML_LIST dx0_get_entities __(( DXF_FILE, DXF_FILTER, DML_LIST )) ;
STATIC BOOLEAN dx0_entity_end_name __(( DXF_FILE, STRING )) ;

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_get_entity ( file, filter ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_FILTER filter ;
{ 
    DXF_ENTITY entity ;
    BOOLEAN status ;

    while ( TRUE ) {
        entity = dx0_get_entity0 ( file, &status ) ;
        if ( !status ) 
            RETURN ( NULL ) ;
        if ( ( entity != NULL ) && dx0_entity_filter ( entity, filter ) ) {
            dx0_strcpy ( DX_FILE_ENT_LAYER(file), DXF_ENTITY_LAYER(entity) ) ;
            RETURN ( entity ) ;
        }
        else 
            dxf_free_entity ( entity ) ;
    }
#ifndef __BRLNDC__
#ifndef __UNIX__
    RETURN ( NULL ) ;
#endif
#endif
}


/*-------------------------------------------------------------------------*/
STATIC DXF_ENTITY dx0_get_entity0 ( file, status ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
BOOLEAN *status ;
{ 
    DXF_ENTITY entity ;
    BOOLEAN s1 ;
    entity = dx0_get_entity1 ( file, &s1 ) ;
    *status = dx0_get_entity2 ( file, entity ) && s1 ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DXF_ENTITY dx0_get_entity1 ( file, status ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* 06/04/91 ANN; Initialize w0 & w1 of polyline & vertex as -1. */
/* 08/21/91 ANN; Initialize Color of all entity_headers as 256 */

DXF_FILE file ;
BOOLEAN *status ;
{ 
    DXF_ENTITY entity ;
    DX_NAME name ;
    DXF_ENTITY_ID entity_id ;
    fpos_t curr_pos ;

    if ( !dx0_find ( file, 0, NULL, &curr_pos ) ||
         !dx0_get_string ( file, name ) )
        RETURN ( NULL ) ;
    *status = !dx0_entity_end_name ( file, name ) ;
    entity_id = dx0_entity_id ( name ) ;
#ifdef __ACAD11__
    entity = dx0_alloc_entity ( entity_id, file ) ;
#else
    entity = dx0_alloc_entity ( entity_id ) ;
#endif

    /**** Change some initializations ****/
    if (entity)
        entity->entity_data.entity_line.line_entity_header.color = 256;
 
    if (entity_id == DXF_POLYLINE)
    {
        entity->entity_data.entity_pline.pline_w0 = -1.;
        entity->entity_data.entity_pline.pline_w1 = -1.;
    }
    else if (entity_id == DXF_VERTEX)
    {
        entity->entity_data.entity_vertex.vertex_w0 = -1.;
        entity->entity_data.entity_vertex.vertex_w1 = -1.;
    }
 
    if ( entity != NULL ) {
        if ( DX_FILE_VTCS_DISABLED(file) && DXF_ENTITY_IS_VERTEX(entity) ) 
            DX_FILE_SET_SEQ_OPEN(file) ;
        if ( DX_FILE_ATTRS_DISABLED(file) && DXF_ENTITY_IS_ATTRIB(entity) ) 
            DX_FILE_SET_SEQ_OPEN(file) ;
    }
    if ( (*status) && ( entity == NULL ) )
        dx0_unrec ( file, curr_pos ) ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN dx0_entity_end_name ( file, name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING name ;
{ 
    if ( DX_FILE_SEQ_OPEN(file) ) {
        if ( !strcmp ( name, "SEQEND" ) ) {
            DX_FILE_SET_SEQ_CLOSED(file) ;
            RETURN ( FALSE ) ;
        }
        else 
            RETURN ( !strcmp ( name, "ENDBLK" ) || 
                !strcmp ( name, "ENDSEC" ) ) ;
    }
    else
        RETURN ( !strcmp ( name, "ENDBLK" ) || 
            !strcmp ( name, "ENDSEC" ) || !strcmp ( name, "SEQEND" ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN dx0_get_entity2 ( file, entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_ENTITY entity ;
{ 
    int code, *template ;
    BOOLEAN mod_6, mod_62 ;

#ifdef  __ACAD11__
    template = ( entity == NULL ) ? NULL : 
        dx0_entity_template ( DX_ENTITY_ID(entity), 
            dx0_file_release ( file ) ) ;
#else
    template = ( entity == NULL ) ? NULL : 
        dx0_entity_template ( DX_ENTITY_ID(entity) ) ;
#endif
    code = dx0_get_object ( file, ( entity==NULL ) ? NULL : 
        (char*)DX_ENTITY_ANY(entity), template, &mod_6, &mod_62 ) ;
    if ( entity != NULL ) {
        DXF_ENTITY_LTYPE_STATUS(entity) = 
            mod_6 ? DXF_LTYPE_BYENT : DXF_LTYPE_BYLAYER ;
        DXF_ENTITY_COLOR_STATUS(entity) = 
            mod_62 ? DXF_COLOR_BYENT : DXF_COLOR_BYLAYER ;
    }
    if ( code == DXF_BAD_CODE ) {
        dxf_free_entity ( entity ) ;
        RETURN ( FALSE ) ;
    }
    else if ( code == 0 ) {
        if ( entity == NULL ) 
            RETURN ( TRUE ) ;
        else if ( DXF_ENTITY_IS_PLINE(entity) ) {
            if ( !dx0_pline_vtcs ( file, entity ) ) {
                dxf_free_entity ( entity ) ;
                RETURN ( FALSE ) ;
            }
        }
        else if ( DXF_ENTITY_IS_INSERT(entity) ) {
            if ( !dx0_insert_attrs ( file, entity ) ) {
                dxf_free_entity ( entity ) ;
                RETURN ( FALSE ) ;
            }
        }
        else if ( DXF_ENTITY_IS_TEXT(entity) ) {
            if ( strlen ( DXF_TEXT_STYLE(entity) ) == 0 )
                strcpy ( DXF_TEXT_STYLE(entity), "STANDARD" ) ;
        }
        else if ( DXF_ENTITY_IS_ATTDEF(entity) ) {
            if ( strlen ( DXF_ATTDEF_STYLE(entity) ) == 0 )
                strcpy ( DXF_ATTDEF_STYLE(entity), "STANDARD" ) ;
        }
        else if ( DXF_ENTITY_IS_ATTRIB(entity) ) {
            if ( strlen ( DXF_ATTRIB_STYLE(entity) ) == 0 )
                strcpy ( DXF_ATTRIB_STYLE(entity), "STANDARD" ) ;
        }
        RETURN ( TRUE ) ;
    }
#ifdef  __ACAD11__
    else if ( code == 1001 ) 
        RETURN ( dx0_get_appl_data ( file, entity ) ) ;
#endif
    else
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN dx0_pline_vtcs ( file, polyline ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_ENTITY polyline ;
{ 
    DXF_ENTITY vertex ;
    BOOLEAN status ;

    if ( DX_FILE_VTCS_DISABLED(file) )
        RETURN ( TRUE ) ;
    DXF_PLINE_VLIST(polyline) = dml_create_list();
    if ( DXF_PLINE_VLIST(polyline) == NULL ) 
        RETURN ( FALSE ) ;
    while ( TRUE ) {
        vertex = dx0_get_entity1 ( file, &status ) ;
        if ( !status || ( vertex == NULL ) )
            RETURN ( TRUE ) ;
        if ( !DXF_ENTITY_IS_VERTEX(vertex) ) 
            RETURN ( FALSE ) ;
        if ( !dx0_get_entity2 ( file, vertex ) ) 
            RETURN ( FALSE ) ;
        if ( dml_append_data ( DXF_PLINE_VLIST(polyline), vertex ) == NULL )
            RETURN ( FALSE ) ;
    }
#ifndef __BRLNDC__
#ifndef __UNIX__
    RETURN ( FALSE ) ;
#endif
#endif
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN dx0_insert_attrs ( file, insert ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_ENTITY insert ;
{ 
    DXF_ENTITY attrib ;
    fpos_t pos ;
    DX_NAME name ;
    BOOLEAN status ;

    if ( DX_FILE_ATTRS_DISABLED(file) )
        RETURN ( TRUE ) ;
    if ( !dx0_getpos ( file, &pos ) )
        RETURN ( FALSE ) ;
    if ( dx0_get_code ( file ) != 0 || !dx0_get_string ( file, name ) )
        RETURN ( dx0_setpos ( file, &pos ) ) ;
    if ( strcmp ( name, DXF_ATTRIB_NAME ) != 0 )
        RETURN ( dx0_setpos ( file, &pos ) ) ;

    if ( !dx0_setpos ( file, &pos ) ) 
        RETURN ( FALSE ) ;
    DXF_INSERT_ATTRS(insert) = dml_create_list();
    if ( DXF_INSERT_ATTRS(insert) == NULL ) 
        RETURN ( FALSE ) ;
    while ( TRUE ) {
        attrib = dx0_get_entity0 ( file, &status ) ;
        if ( ( attrib == NULL ) || !status ) 
            RETURN ( TRUE ) ;
        if ( !DXF_ENTITY_IS_ATTRIB(attrib) ) 
            RETURN ( FALSE ) ;
        if ( dml_append_data ( DXF_INSERT_ATTRS(insert), attrib ) == NULL )
            RETURN ( FALSE ) ;
    }
#ifndef __BRLNDC__
#ifndef __UNIX__
    RETURN ( FALSE ) ;
#endif
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dxf_get_entities ( file, blockname, filter, entitylist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING blockname ;
DXF_FILTER filter ;
DML_LIST entitylist ;
{
    if ( !dxf_set_filepos ( file, blockname, DXF_FIRST_ENTITY ) )
        RETURN ( NULL ) ;
    RETURN ( dx0_get_entities ( file, filter, entitylist ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_LIST dx0_get_entities ( file, filter, entitylist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_FILTER filter ;
DML_LIST entitylist ;
{
    DXF_ENTITY entity ;

    if ( entitylist == NULL ) 
        entitylist = dml_create_list ();
    if ( entitylist == NULL ) 
        RETURN ( NULL ) ;

    for ( entity = dxf_get_entity ( file, filter ) ; entity != NULL ;
        entity = dxf_get_entity ( file, filter ) ) {
        if ( dml_append_data ( entitylist, entity ) == NULL )
            RETURN ( NULL ) ;
    }
    RETURN ( entitylist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dxf_read_entities ( filename, blockname, filter, 
            entitylist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
STRING blockname ;
DXF_FILTER filter ;
DML_LIST entitylist ;
{
    DXF_FILE file ;

    file = dxf_fopen ( filename, DXF_READ ) ;
    if ( file == NULL ) 
        RETURN ( NULL ) ;
    if ( !dx0_setpos_block ( file, blockname ) )
        RETURN ( NULL ) ;
    entitylist = dx0_get_entities ( file, filter, entitylist ) ;
    dxf_fclose ( file ) ;
    RETURN ( entitylist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_FILTER dxf_create_filter ( type, layer, color ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY_ID type ;
STRING layer ;
INT color ;
{
    DXF_FILTER filter = DX_CREATE_FILTER ;
    if ( filter == NULL ) 
        RETURN ( NULL ) ;
    DX_FILTER_TYPE(filter) = type ;
    dxf_filter_set_layer ( filter, layer ) ;
    DX_FILTER_COLOR(filter) = color ;
#ifdef __ACAD11__
    DX_FILTER_APPL_LIST(filter) = NULL ;
    DX_FILTER_ALL_APPLS(filter) = FALSE ;
#endif
    RETURN ( filter ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_filter_set_type ( filter, type ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILTER filter ;
DXF_ENTITY_ID type ;
{
    DX_FILTER_TYPE(filter) = type ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_filter_add_type ( filter, type ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILTER filter ;
DXF_ENTITY_ID type ;
{
    DX_FILTER_TYPE(filter) |= type ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_filter_set_layer ( filter, layer ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILTER filter ;
STRING layer ;
{
    dx0_strcpy ( DX_FILTER_LAYER(filter), layer ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_filter_set_color ( filter, color ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILTER filter ;
INT color ;
{
    DX_FILTER_COLOR(filter) = color ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_free_filter ( filter ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILTER filter ;
{
#ifdef __ACAD11__
    dml_destroy_list ( DX_FILTER_APPL_LIST(filter), dmm_free ) ;
#endif
    DX_FREE_FILTER(filter) ;
}

