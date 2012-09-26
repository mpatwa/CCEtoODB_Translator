/* -2 -3 -x */
/******************************* DXEGA.C ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#include <dmldefs.h>
#include <dx0defs.h>
#include <dxapdatm.h>
#include <dxapdefm.h>
#include <dxfilm.h>
#include <dxpos.h>
#ifdef __ACAD11__

STATIC DX_APPDAT dx0_get_appdat __(( DXF_FILE )) ;
STATIC DX_APPDAT dx0_get_appdat0 __(( DXF_FILE, DX_APPDEF )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_get_appl_data ( file, entity )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_ENTITY entity ;
{
    DX_APPDAT appdat ;

    if ( DX_ENTITY_APPDAT_LIST(entity) == NULL )
        DX_ENTITY_APPDAT_LIST(entity) = dml_create_list () ;
    for ( appdat = dx0_get_appdat ( file ) ; appdat != NULL ;
          appdat = dx0_get_appdat ( file ) ) {
        dml_append_data ( DX_ENTITY_APPDAT_LIST(entity), appdat ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DX_APPDAT dx0_get_appdat ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    fpos_t pos ;
    DX_STRING appl_name ;
    DX_APPDEF appdef ;
    int code ;

    while ( TRUE ) {
        if ( !dx0_getpos ( file, &pos ) ) 
            RETURN ( NULL ) ;
        code = dx0_get_code ( file ) ;
        if ( code == 1001 ) {
            dx0_get_string ( file, appl_name ) ;
            appdef = dx0_find_appdef ( file, appl_name ) ;
            if ( appdef != NULL ) {
                dx0_setpos ( file, &pos ) ;
                RETURN ( dx0_get_appdat0 ( file, appdef ) ) ; 
            }
        }
        else if ( code == 0 ) {
            dx0_setpos ( file, &pos ) ;
            RETURN ( NULL ) ;
        }
        else
            dx0_get_void ( file, code ) ;
    }
}


/*-------------------------------------------------------------------------*/
STATIC DX_APPDAT dx0_get_appdat0 ( file, appdef )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DX_APPDEF appdef ;
{
    DX_APPDAT appdat ;

    appdat = dx0_create_appdat_template ( DX_APPDEF_TEMPLATE(appdef) ) ;
    dx0_get_object ( file, DX_APPDAT_DATA(appdat), 
        DX_APPDAT_TEMPLATE(appdat), ( BOOLEAN* ) FALSE, ( BOOLEAN* ) FALSE ) ;
    RETURN ( appdat ) ;
}
#endif

