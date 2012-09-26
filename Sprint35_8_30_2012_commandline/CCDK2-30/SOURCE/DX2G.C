/* -2 -3 */
/******************************* DX2G.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h>
#include <dx2defs.h>
#include <dx0defs.h>

STATIC DML_LIST dx2_get_points0 __(( DXF_FILE, DXF_FILTER, DML_LIST )) ;
STATIC DML_LIST dx2_get_curves0 __(( DXF_FILE, DXF_FILTER, DML_LIST )) ;

/*-------------------------------------------------------------------------*/
BBS_PUBLIC PT2* dx2_get_point ( file, filter ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_FILTER filter ;
{ 
    DXF_ENTITY entity ;
    PT2 *p ;
    DXF_FILTER_S flt ;

    if ( filter == NULL ) {
        filter = &flt ;
        DX_FILTER_TYPE(filter) = DXF_POINT ;
        dx0_strcpy ( DX_FILTER_LAYER(filter), NULL ) ;
        DX_FILTER_COLOR(filter) = DXF_ANY_COLOR ;
    }

    while ( TRUE ) {
        entity = dxf_get_entity ( file, filter ) ;
        if ( entity == NULL )
            RETURN ( NULL ) ;
        p = dx2_entity_to_point ( entity ) ;
        dxf_free_entity ( entity ) ;
        if ( p != NULL ) 
            RETURN ( p ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dx2_get_points ( file, blockname, filter, pointslist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING blockname ;
DXF_FILTER filter ;
DML_LIST pointslist ;
{ 
    if ( !dxf_set_filepos ( file, blockname, DXF_FIRST_ENTITY ) )
        RETURN ( NULL ) ;
    RETURN ( dx2_get_points0 ( file, filter, pointslist ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_LIST dx2_get_points0 ( file, filter, pointslist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_FILTER filter ;
DML_LIST pointslist ;
{ 
    PT2 *p ;

    if ( pointslist == NULL ) 
        pointslist = dml_create_list ();
    if ( pointslist == NULL ) 
        RETURN ( NULL ) ;

    for ( p = dx2_get_point ( file, filter ) ; p != NULL ;
        p = dx2_get_point ( file, filter ) ) {
        if ( dml_append_data ( pointslist, p ) == NULL )
            RETURN ( NULL ) ;
    }
    RETURN ( pointslist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dx2_read_points ( filename, blockname, 
        filter, pointslist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
STRING blockname ;
DXF_FILTER filter ;
DML_LIST pointslist ;
{
    DXF_FILE file ;

    file = dxf_fopen ( filename, DXF_READ ) ;
    if ( file == NULL ) 
        RETURN ( NULL ) ;
    if ( !dx0_setpos_block ( file, blockname ) )
        RETURN ( NULL ) ;
    pointslist = dx2_get_points0 ( file, filter, pointslist ) ;
    dxf_fclose ( file ) ;
    RETURN ( pointslist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE dx2_get_curve ( file, filter ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_FILTER filter ;
{ 
    DXF_ENTITY entity ;
    C2_CURVE curve ;
    DXF_FILTER_S flt ;

    if ( filter == NULL ) {
        filter = &flt ;
        DX_FILTER_TYPE(filter) = DXF_CURVE ;
        dx0_strcpy ( DX_FILTER_LAYER(filter), NULL ) ;
        DX_FILTER_COLOR(filter) = DXF_ANY_COLOR ;
    }

    while ( TRUE ) {
        entity = dxf_get_entity ( file, filter ) ;
        if ( entity == NULL ) 
            RETURN ( NULL ) ;
        curve = dx2_entity_to_curve ( entity ) ;
        dxf_free_entity ( entity ) ;
        if ( curve != NULL ) 
            RETURN ( curve ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dx2_get_curves ( file, blockname, filter, curveslist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING blockname ;
DXF_FILTER filter ;
DML_LIST curveslist ;
{ 
    if ( !dxf_set_filepos ( file, blockname, DXF_FIRST_ENTITY ) )
        RETURN ( NULL ) ;
    RETURN ( dx2_get_curves0 ( file, filter, curveslist ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_LIST dx2_get_curves0 ( file, filter, curveslist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_FILTER filter ;
DML_LIST curveslist ;
{ 
    C2_CURVE curve ;
    if ( curveslist == NULL ) 
        curveslist = dml_create_list ();
    if ( curveslist == NULL ) 
        RETURN ( NULL ) ;

    for ( curve = dx2_get_curve ( file, filter ) ; curve != NULL ;
        curve = dx2_get_curve ( file, filter ) ) {
        if ( dml_append_data ( curveslist, curve ) == NULL ) {
            RETURN ( NULL ) ;
        }
    }
    RETURN ( curveslist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dx2_read_curves ( filename, blockname, 
        filter, curveslist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
STRING blockname ;
DXF_FILTER filter ;
DML_LIST curveslist ;
{
    DXF_FILE file ;

    file = dxf_fopen ( filename, DXF_READ ) ;
    if ( file == NULL ) 
        RETURN ( NULL ) ;
    if ( !dx0_setpos_block ( file, blockname ) ) 
        RETURN ( NULL ) ;
    curveslist = dx2_get_curves0 ( file, filter, curveslist ) ;
    dxf_fclose ( file ) ;
    RETURN ( curveslist ) ;
}

