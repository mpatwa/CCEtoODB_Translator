/* -2 -3 -x */
/******************************* DXEPA.C ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dx0defs.h>
#include <dxapdatm.h>
#include <dmldefs.h>

#ifdef __ACAD11__

STATIC BOOLEAN dx0_put_appdat __(( DXF_FILE, DX_APPDAT )) ;
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_appl_data ( file, entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_ENTITY entity ;
{ 
    DML_ITEM item ;
    BOOLEAN status ;

    if ( DX_ENTITY_APPDAT_LIST(entity) == NULL ) 
        RETURN ( TRUE ) ;

    status = TRUE ;
    if ( DX_ENTITY_APPDAT_LIST(entity) != NULL ) {
        DML_WALK_LIST ( DX_ENTITY_APPDAT_LIST(entity), item ) {
            if ( !dx0_put_appdat ( file, DML_RECORD(item) ) )
                status = FALSE ;
        }
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN dx0_put_appdat ( file, appdat ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DX_APPDAT appdat ;
{ 
    RETURN ( dx0_put_object ( file, DX_APPDAT_DATA(appdat), 
        DX_APPDAT_TEMPLATE(appdat), FALSE, FALSE ) ) ;
}
#endif

