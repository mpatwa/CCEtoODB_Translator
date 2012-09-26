/* -2 -3 -x */
/******************************* DXEC.C *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                      !!!!!!!!*/ 
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <string.h> 
#include <dmldefs.h>
#include <dxents.h>
#include <dx0defs.h>

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_set_entity_layer ( entity, layer ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
STRING layer ;
{
    dx0_strcpy ( DXF_ENTITY_LAYER(entity), layer ) ;
    if ( DXF_ENTITY_IS_INSERT(entity) )
        dml_apply_data ( DXF_INSERT_ATTRS(entity), 
            ( PF_ACTION_DATA ) dxf_set_entity_layer, layer ) ;
    else if ( DXF_ENTITY_IS_PLINE(entity) )
        dml_apply_data ( DXF_PLINE_VLIST(entity), 
            ( PF_ACTION_DATA ) dxf_set_entity_layer, layer ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_set_entity_handle ( entity, handle ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
STRING handle ;
{
    dx0_strcpy ( DXF_ENTITY_HANDLE(entity), handle ) ;
    if ( DXF_ENTITY_IS_INSERT(entity) )
        dml_apply_data ( DXF_INSERT_ATTRS(entity), 
            ( PF_ACTION_DATA ) dxf_set_entity_handle, handle ) ;
    else if ( DXF_ENTITY_IS_PLINE(entity) )
        dml_apply_data ( DXF_PLINE_VLIST(entity), 
            ( PF_ACTION_DATA ) dxf_set_entity_handle, handle ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_set_entity_ltype ( entity, ltype ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
STRING ltype ;
{
    dx0_strcpy ( DXF_ENTITY_LTYPE(entity), ltype ) ;
    DXF_ENTITY_LTYPE_STATUS(entity) = DXF_LTYPE_BYENT ;
    if ( DXF_ENTITY_IS_INSERT(entity) )
        dml_apply_data ( DXF_INSERT_ATTRS(entity), 
            ( PF_ACTION_DATA ) dxf_set_entity_ltype, ltype ) ;
    else if ( DXF_ENTITY_IS_PLINE(entity) )
        dml_apply_data ( DXF_PLINE_VLIST(entity), 
            ( PF_ACTION_DATA ) dxf_set_entity_ltype, ltype ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_set_entity_ltype_bylayer ( entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
{
    DXF_ENTITY_LTYPE_STATUS(entity) = DXF_LTYPE_BYLAYER  ;
    if ( DXF_ENTITY_IS_INSERT(entity) )
        dml_apply ( DXF_INSERT_ATTRS(entity), ( PF_ACTION ) dxf_set_entity_ltype_bylayer ) ;
    else if ( DXF_ENTITY_IS_PLINE(entity) )
        dml_apply ( DXF_PLINE_VLIST(entity), ( PF_ACTION ) dxf_set_entity_ltype_bylayer ) ;
}


#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_set_entity_elevation ( file, entity, elevation ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_ENTITY entity ;
REAL elevation ;
{
    int *template ;
    int code ;
    BOOLEAN s = TRUE ;

    if ( DXF_ENTITY_IS_POINT(entity) || DXF_ENTITY_IS_LINE(entity) ||
         DXF_ENTITY_IS_LINE3D(entity) || DXF_ENTITY_IS_3DFACE(entity) )
        RETURN ;

    template = dx0_entity_template ( DX_ENTITY_ID(entity), 
        dx0_file_release ( file ) ) ;
    if ( template == NULL ) 
        RETURN ;
    for ( code = 30 ; code < 37 && s ; code++ ) {
        if ( !DXF_ENTITY_IS_DIM(entity) || ( code == 31 ) || 
            ( code == 32 ) || ( code == 36 ) ) {
            s = dx0_set_template ( (char*)DX_ENTITY_ANY(entity), 
                template, code, 0, &elevation ) ;
        }
    }
}
#else
/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_set_entity_elevation ( entity, elevation ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
REAL elevation ;
{
    int *template = dx0_entity_template ( DX_ENTITY_ID(entity) ) ;
    int code ;
    BOOLEAN s = TRUE ;

    if ( DXF_ENTITY_IS_POINT(entity) || DXF_ENTITY_IS_LINE(entity) ||
         DXF_ENTITY_IS_LINE3D(entity) || DXF_ENTITY_IS_3DFACE(entity) )
        RETURN ;

    if ( template == NULL ) 
        RETURN ;
    for ( code = 30 ; code < 37 && s ; code++ ) {
        if ( !DXF_ENTITY_IS_DIM(entity) || ( code == 31 ) || 
            ( code == 32 ) || ( code == 36 ) ) {
            s = dx0_set_template ( (char*)DX_ENTITY_ANY(entity), 
                template, code, 0, &elevation ) ;
        }
    }
}
#endif


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_set_entity_thickness ( entity, thickness ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
REAL thickness ;
{
    DML_LIST list ;
    DML_ITEM item ;

    DXF_ENTITY_THICKNESS(entity) = thickness ;
    if ( DXF_ENTITY_IS_INSERT(entity) )
        list = DXF_INSERT_ATTRS(entity) ;
    else if ( DXF_ENTITY_IS_PLINE(entity) )
        list = DXF_PLINE_VLIST(entity) ;
    else 
        RETURN ;

    if ( list == NULL )
      RETURN ;
    DML_WALK_LIST ( list, item ) 
        dxf_set_entity_thickness ( DML_RECORD(item), thickness ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_set_entity_color ( entity, color ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
INT color ;
{
    DML_LIST list ;
    DML_ITEM item ;

    DXF_ENTITY_COLOR(entity) = color ;
    DXF_ENTITY_COLOR_STATUS(entity) = 
        ( color >= 0 ) ? DXF_COLOR_BYENT : DXF_COLOR_BYLAYER ;
    if ( DXF_ENTITY_IS_INSERT(entity) )
        list = DXF_INSERT_ATTRS(entity) ;
    else if ( DXF_ENTITY_IS_PLINE(entity) )
        list = DXF_PLINE_VLIST(entity) ;
    else 
        RETURN ;

    if ( list == NULL )
      RETURN ;
    DML_WALK_LIST ( list, item ) 
        dxf_set_entity_color ( DML_RECORD(item), color ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_set_entity_color_bylayer ( entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
{
    DXF_ENTITY_COLOR_STATUS(entity) = DXF_COLOR_BYLAYER ;
    if ( DXF_ENTITY_IS_INSERT(entity) )
        dml_apply ( DXF_INSERT_ATTRS(entity), ( PF_ACTION ) dxf_set_entity_color_bylayer ) ;
    else if ( DXF_ENTITY_IS_PLINE(entity) )
        dml_apply ( DXF_PLINE_VLIST(entity), ( PF_ACTION ) dxf_set_entity_color_bylayer ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_set_entity_extrusion ( entity, extrusion ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
REAL extrusion[3] ;
{
    dx0_copy_point ( extrusion, DXF_ENTITY_EXTRUSION(entity) );
    if ( DXF_ENTITY_IS_INSERT(entity) )
        dml_apply_data ( DXF_INSERT_ATTRS(entity), 
            ( PF_ACTION_DATA ) dxf_set_entity_extrusion, extrusion ) ;
    else if ( DXF_ENTITY_IS_PLINE(entity) )
        dml_apply_data ( DXF_PLINE_VLIST(entity), 
            ( PF_ACTION_DATA ) dxf_set_entity_extrusion, extrusion ) ;
}



/*-------------------------------------------------------------------------*/
BBS_PUBLIC STRING dxf_get_entity_layer ( entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
{
    RETURN ( DXF_ENTITY_LAYER(entity) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC STRING dxf_get_entity_handle ( entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
{
    RETURN ( DXF_ENTITY_HANDLE(entity) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC STRING dxf_get_entity_ltype ( entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
{
    RETURN ( ( DXF_ENTITY_LTYPE_STATUS(entity) == DXF_LTYPE_BYENT ) ? 
        DXF_ENTITY_LTYPE(entity) : NULL ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT dxf_get_entity_ltype_status ( entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
{
    RETURN ( DXF_ENTITY_LTYPE_STATUS(entity) ) ;
}

#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL dxf_get_entity_elevation ( file, entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_ENTITY entity ;
{
    int *template = dx0_entity_template ( DX_ENTITY_ID(entity), 
        dx0_file_release ( file ) ) ;
    REAL *r ;
    if ( template == NULL ) 
        RETURN ( 0.0 ) ;
    r = (REAL*)dx0_get_template ( (char*)DX_ENTITY_ANY(entity), 
        template, 30, 0 ) ;
    RETURN ( *r ) ;
}
#else

/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL dxf_get_entity_elevation ( entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
{
    int *template = dx0_entity_template ( DX_ENTITY_ID(entity) ) ;
    REAL *r ;
    if ( template == NULL ) 
        RETURN ( 0.0 ) ;
    r = dx0_get_template ( (char*)DX_ENTITY_ANY(entity), template, 30, 0 ) ;
    RETURN ( *r ) ;
}
#endif

/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL dxf_get_entity_thickness ( entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
{
    RETURN ( DXF_ENTITY_THICKNESS(entity) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT dxf_get_entity_color ( entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
{
    RETURN ( ( DXF_ENTITY_COLOR_STATUS(entity) == DXF_COLOR_BYENT ) ?
        DXF_ENTITY_COLOR(entity) : -1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT dxf_get_entity_color_status ( entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
{
    RETURN ( DXF_ENTITY_COLOR_STATUS(entity) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL *dxf_get_entity_extrusion ( entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
{
    RETURN ( DXF_ENTITY_EXTRUSION(entity) ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dxf_get_polyline_vlist ( entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
{
    RETURN ( DXF_PLINE_VLIST (entity) ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_set_polyline_vlist ( entity, vlist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
DML_LIST vlist;
{
    RETURN ( TRUE ) ;
}

#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_set_entity_descriptor ( file, entity, descriptor ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_ENTITY entity ;
DXF_ENTITY_DESC descriptor ;
{
    if ( entity == NULL || descriptor == NULL )
        RETURN ;

    DXF_ENTITY_LTYPE_STATUS(entity) = DXF_DESC_LTYPE_STATUS(descriptor) ;
    DXF_ENTITY_COLOR_STATUS(entity) = DXF_DESC_COLOR_STATUS(descriptor) ;
    dx0_strcpy ( DXF_ENTITY_LAYER(entity), DXF_DESC_LAYER(descriptor) ) ;
    dx0_strcpy ( DXF_ENTITY_HANDLE(entity), DXF_DESC_HANDLE(descriptor) ) ;
    dx0_strcpy ( DXF_ENTITY_LTYPE(entity), DXF_DESC_LTYPE(descriptor) ) ;
    dxf_set_entity_elevation ( file, entity, DXF_DESC_ELEVATION(descriptor) + 
        dxf_get_entity_elevation ( file, entity ) ) ;
    DXF_ENTITY_THICKNESS(entity) = DXF_DESC_THICKNESS(descriptor) ;
    DXF_ENTITY_COLOR(entity) = DXF_DESC_COLOR(descriptor) ;
    if ( !IS_SMALL(DXF_DESC_EXTRUSION(descriptor)[0]) || 
         !IS_SMALL(DXF_DESC_EXTRUSION(descriptor)[1]) || 
         !IS_SMALL(DXF_DESC_EXTRUSION(descriptor)[2]) ) {
        DXF_ENTITY_EXTRUSION(entity)[0] = DXF_DESC_EXTRUSION(descriptor)[0] ;
        DXF_ENTITY_EXTRUSION(entity)[1] = DXF_DESC_EXTRUSION(descriptor)[1] ;
        DXF_ENTITY_EXTRUSION(entity)[2] = DXF_DESC_EXTRUSION(descriptor)[2] ;
    }
}

#else
/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_set_entity_descriptor ( entity, descriptor ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
DXF_ENTITY_DESC descriptor ;
{
    if ( entity == NULL || descriptor == NULL )
        RETURN ;

    DXF_ENTITY_LTYPE_STATUS(entity) = DXF_DESC_LTYPE_STATUS(descriptor) ;
    DXF_ENTITY_COLOR_STATUS(entity) = DXF_DESC_COLOR_STATUS(descriptor) ;
    dx0_strcpy ( DXF_ENTITY_LAYER(entity), DXF_DESC_LAYER(descriptor) ) ;
    dx0_strcpy ( DXF_ENTITY_HANDLE(entity), DXF_DESC_HANDLE(descriptor) ) ;
    dx0_strcpy ( DXF_ENTITY_LTYPE(entity), DXF_DESC_LTYPE(descriptor) ) ;
    dxf_set_entity_elevation ( entity, DXF_DESC_ELEVATION(descriptor) + 
        dxf_get_entity_elevation ( entity ) ) ;
    DXF_ENTITY_THICKNESS(entity) = DXF_DESC_THICKNESS(descriptor) ;
    DXF_ENTITY_COLOR(entity) = DXF_DESC_COLOR(descriptor) ;
    if ( !IS_SMALL(DXF_DESC_EXTRUSION(descriptor)[0]) || 
         !IS_SMALL(DXF_DESC_EXTRUSION(descriptor)[1]) || 
         !IS_SMALL(DXF_DESC_EXTRUSION(descriptor)[2]) ) {
        DXF_ENTITY_EXTRUSION(entity)[0] = DXF_DESC_EXTRUSION(descriptor)[0] ;
        DXF_ENTITY_EXTRUSION(entity)[1] = DXF_DESC_EXTRUSION(descriptor)[1] ;
        DXF_ENTITY_EXTRUSION(entity)[2] = DXF_DESC_EXTRUSION(descriptor)[2] ;
    }
}
#endif
