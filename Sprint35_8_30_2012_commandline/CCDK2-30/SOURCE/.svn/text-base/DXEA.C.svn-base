/* -2 -3 -x */
/******************************* DXEA.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#include <dx0defs.h>
#include <dxapdatm.h>
#include <dmldefs.h>

#ifdef __ACAD11__
STATIC DML_ITEM dx0_filter_find_appl __(( DXF_FILTER, STRING )) ;
STATIC DML_ITEM dx0_entity_find_appdat __(( DXF_ENTITY, STRING )) ;

/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_set_entity_field ( entity, appl_name, 
            code, index, value ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
STRING appl_name ;
int code, index ;
ANY value ;
{
    DX_APPDAT appdat ;
    DML_ITEM item ;

    item = dx0_entity_find_appdat ( entity, appl_name ) ;
    if ( item == NULL ) 
        RETURN ( FALSE ) ;
    appdat = DML_RECORD(item) ;
    RETURN ( dx0_set_template ( DX_APPDAT_DATA(appdat), 
        DX_APPDAT_TEMPLATE(appdat), code, index, value ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC ANY dxf_get_entity_field ( entity, appl_name, code, index ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
STRING appl_name ;
int code, index ;
{
    DX_APPDAT appdat ;
    DML_ITEM item ;

    item = dx0_entity_find_appdat ( entity, appl_name ) ;
    if ( item == NULL ) 
        RETURN ( NULL ) ;
    appdat = DML_RECORD(item) ;
    RETURN ( dx0_get_template ( DX_APPDAT_DATA(appdat), 
        DX_APPDAT_TEMPLATE(appdat), code, index ) ) ;

}


/*-------------------------------------------------------------------------*/
STATIC DML_ITEM dx0_entity_find_appdat ( entity, appl_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
STRING appl_name ;
{
    DML_ITEM item ;
    DX_APPDAT appdat ;

    if ( DX_ENTITY_APPDAT_LIST(entity) == NULL )
        RETURN ( NULL ) ;

    DML_WALK_LIST ( DX_ENTITY_APPDAT_LIST(entity), item ) {
        appdat = DML_RECORD(item) ;
        if ( !strcmp ( dx0_appdat_name ( appdat ), appl_name ) ) 
            RETURN ( item ) ;
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DX_APPDAT dx0_create_appdat ( file, appl_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING appl_name ;
{
    int* template ;

    template = dxf_appl_template ( file, appl_name ) ;
    if ( template == NULL ) 
        RETURN ( NULL ) ;
    RETURN ( dx0_create_appdat_template ( template ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DX_APPDAT dx0_create_appdat_template ( template ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
int* template ;
{
    DX_APPDAT appdat ;

    if ( template == NULL ) 
        RETURN ( NULL ) ;
    appdat = DX_CREATE_APPDAT ;
    DX_APPDAT_TEMPLATE(appdat) = template ;
    DX_APPDAT_DATA(appdat) = dx0_create_record ( template ) ;
    RETURN ( appdat ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_free_appdat ( appdat ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DX_APPDAT appdat ;
{
    dx0_free_record ( DX_APPDAT_DATA(appdat), DX_APPDAT_TEMPLATE(appdat) ) ;
    DX_FREE_APPDAT ( appdat ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE STRING dx0_appdat_name ( appdat ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DX_APPDAT appdat ;
{
    RETURN ( (STRING)DX_APPDAT_DATA(appdat) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_filter_append_appl ( filter, appl_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILTER filter ;
STRING appl_name ;
{
    if ( appl_name == NULL ) {
        dxf_filter_append_all_appl ( filter ) ;
        RETURN ;
    }
    if ( DX_FILTER_APPL_LIST(filter) == NULL )
        DX_FILTER_APPL_LIST(filter) = dml_create_list () ;
    if ( dx0_filter_find_appl ( filter, appl_name ) == NULL )
        dml_append_data ( DX_FILTER_APPL_LIST(filter), strdup ( appl_name ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_ITEM dx0_filter_find_appl ( filter, appl_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILTER filter ;
STRING appl_name ;
{
    DML_ITEM item ;
    STRING name ;

    if ( DX_FILTER_APPL_LIST(filter) == NULL )
        RETURN ( NULL ) ;

    DML_WALK_LIST ( DX_FILTER_APPL_LIST(filter), item ) {
        name = DML_RECORD(item) ;
        if ( !strcmp ( appl_name, name ) ) 
            RETURN ( item ) ;
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_filter_append_all_appl ( filter ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILTER filter ;
{
    DX_FILTER_ALL_APPLS(filter) = TRUE ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_filter_cancel_all_appl ( filter ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILTER filter ;
{
    DX_FILTER_ALL_APPLS(filter) = FALSE ;
    dml_destroy_list ( DX_FILTER_APPL_LIST(filter), dmm_free ) ;
    DX_FILTER_APPL_LIST(filter) = NULL ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_filter_cancel_appl ( filter, appl_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILTER filter ;
STRING appl_name ;
{
    DML_ITEM item ;
    STRING name ;

    if ( appl_name == NULL ) {
        dxf_filter_cancel_all_appl ( filter ) ;
        RETURN ;
    }
    item = dx0_filter_find_appl ( filter, appl_name ) ;
    if ( item != NULL ) {
        name = DML_RECORD(item) ;
        dmm_free ( name ) ;
        dml_remove_item ( DX_FILTER_APPL_LIST(filter), item ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_appdat_filter ( appdat_list, filter )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST appdat_list ;
DXF_FILTER filter ;
{
    DML_ITEM item, item1 ;
    DX_APPDAT appdat ;

    if ( ( appdat_list == NULL ) || ( filter == NULL ) ||
            DX_FILTER_ALL_APPLS(filter) )
        RETURN ;

    for ( item = dml_first(appdat_list) ; item != NULL ; item = item1 ) {
        item1 = DML_NEXT(item) ;
        appdat = DML_RECORD(item) ;
        if ( dx0_filter_find_appl ( filter, dx0_appdat_name ( appdat ) ) 
            == NULL ) {
            dx0_free_appdat ( appdat ) ;
            dml_remove_item ( appdat_list, item ) ;
        }
    }
}
#endif

