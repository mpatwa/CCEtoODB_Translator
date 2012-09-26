/* -2 -3 -x */
/******************************* DXED.C *********************************/ 
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
BBS_PUBLIC DXF_ENTITY_DESC dxf_create_descriptor ( layer, ltype, color, 
            elevation, extrusion ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING layer, ltype ;
INT color ;
REAL elevation, extrusion[3] ;
{
    DXF_ENTITY_DESC descriptor = DX_CREATE_DESC ;
    if ( descriptor == NULL )
        RETURN ( NULL ) ;
    DXF_DESC_LTYPE_STATUS(descriptor) = 
        ( ltype == NULL ) ? DXF_LTYPE_BYLAYER : DXF_LTYPE_BYENT ;
    DXF_DESC_COLOR_STATUS(descriptor) = 
        ( color < 0 ) ? DXF_COLOR_BYLAYER : DXF_COLOR_BYENT ;
    dx0_strcpy ( DXF_DESC_LAYER(descriptor), layer ) ;
    dx0_strcpy ( DXF_DESC_HANDLE(descriptor), NULL ) ;
    dx0_strcpy ( DXF_DESC_LTYPE(descriptor), ltype ) ;
    DXF_DESC_THICKNESS(descriptor) = 0.0 ; 
    DXF_DESC_ELEVATION(descriptor) = elevation ; 
    DXF_DESC_COLOR(descriptor) = color ;
    dx0_copy_point ( extrusion, DXF_DESC_EXTRUSION(descriptor) ) ;
    RETURN ( descriptor ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_free_descriptor ( descriptor ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY_DESC descriptor ;
{
    DX_FREE_DESC ( descriptor ) ;
}

#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_line ( file, pt0, pt1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL pt0[3], pt1[3] ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_LINE, file ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( pt0, DXF_LINE_PT0(entity) ) ;
    dx0_copy_point ( pt1, DXF_LINE_PT1(entity) ) ;
    RETURN ( entity ) ;
}
 

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_point ( file, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL pt[3] ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_POINT, file ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( pt, DXF_POINT_PT(entity) ) ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_circle ( file, ctr, rad ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL ctr[3], rad ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_CIRCLE, file ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( ctr, DXF_CIRCLE_CTR(entity) ) ;
    DXF_CIRCLE_RAD(entity) = rad ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_arc ( file, ctr, rad, ang0, ang1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL ctr[3], rad, ang0, ang1 ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_ARC, file ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( ctr, DXF_ARC_CTR(entity) ) ;
    DXF_ARC_RAD(entity) = rad ;
    DXF_ARC_ANG0(entity) = ang0 ;
    DXF_ARC_ANG1(entity) = ang1 ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_trace ( file, pt0, pt1, pt2, pt3 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL pt0[3], pt1[3], pt2[3], pt3[3] ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_TRACE, file ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( pt0, DXF_TRACE_PT0(entity) ) ;
    dx0_copy_point ( pt1, DXF_TRACE_PT1(entity) ) ;
    dx0_copy_point ( pt2, DXF_TRACE_PT2(entity) ) ;
    dx0_copy_point ( pt3, DXF_TRACE_PT3(entity) ) ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_solid ( file, pt0, pt1, pt2, pt3 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL pt0[3], pt1[3], pt2[3], pt3[3] ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_SOLID, file ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    if ( pt3 == NULL ) 
        pt3 = pt2 ;
    dx0_copy_point ( pt0, DXF_SOLID_PT0(entity) ) ;
    dx0_copy_point ( pt1, DXF_SOLID_PT1(entity) ) ;
    dx0_copy_point ( pt2, DXF_SOLID_PT2(entity) ) ;
    dx0_copy_point ( pt3, DXF_SOLID_PT3(entity) ) ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_text ( file, insert_pt, height, text, 
            ang, scale, style_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
DXF_FILE file ;
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL insert_pt[3], height, ang, scale ;
STRING text, style_name ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_TEXT, file ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( insert_pt, DXF_TEXT_INS_PT(entity) ) ;
    DXF_TEXT_HEIGHT(entity) = height ;
    DXF_TEXT_ROT_ANG(entity) = ang ;
    DXF_TEXT_X_SCALE(entity) = scale ;
    dx0_strcpy ( DXF_TEXT_VALUE(entity), text ) ;
    strcpy ( DXF_TEXT_STYLE(entity), 
        style_name == NULL || style_name[0]=='\0' ? "STANDARD" : style_name ) ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_shape ( file, insert_pt, size, name, 
        ang, scale ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL insert_pt[3], size, ang, scale ;
STRING name ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_SHAPE, file ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( insert_pt, DXF_SHAPE_INSERT_PT(entity) ) ;
    DXF_SHAPE_SIZE(entity) = size ;
    DXF_SHAPE_ROT_ANG(entity) = ang ;
    DXF_SHAPE_X_SCALE(entity) = scale ;
    dx0_strcpy ( DXF_SHAPE_NAME(entity), name ) ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_insert ( file, block, insert_pt, scale, ang, 
        cols_cnt, rows_cnt, col_spc, row_spc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING block ;
REAL insert_pt[3], scale, ang, col_spc, row_spc ;
INT cols_cnt, rows_cnt ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_INSERT, file ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( insert_pt, DXF_INSERT_PT(entity) ) ;
    DXF_INSERT_ROT_ANG(entity) = ang ;
    DXF_INSERT_X_SCALE(entity) = scale ;
    DXF_INSERT_Y_SCALE(entity) = scale ;
    DXF_INSERT_Z_SCALE(entity) = scale ;
    dx0_strcpy ( DXF_INSERT_BLOCK(entity), block ) ;
    DXF_INSERT_FLAGS(entity) = 0 ;
    DXF_INSERT_COLS_CNT(entity) = cols_cnt ;
    DXF_INSERT_ROWS_CNT(entity) = rows_cnt ;
    DXF_INSERT_COL_SPC(entity) = col_spc ;
    DXF_INSERT_ROW_SPC(entity) = row_spc ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_insert_add_attrib ( file, insert, text_pt, height, 
    text, tag, flags, len, ang, scale, style_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_ENTITY insert ;
STRING text, tag, style_name ;
REAL text_pt[3], height, ang, scale ;
INT flags, len ;
{
    DXF_ENTITY attrib ;
    if ( DXF_INSERT_ATTRS(insert) == NULL )
        DXF_INSERT_ATTRS(insert) = dml_create_list ();
    if ( DXF_INSERT_ATTRS(insert) == NULL )
        RETURN ( NULL ) ;

    attrib = dx0_create_attrib ( file, text_pt, height, text, tag, flags, 
        len, ang, scale, style_name ) ;

    if ( attrib == NULL || 
        dml_append_data ( DXF_INSERT_ATTRS(insert), attrib ) == NULL )
        RETURN ( NULL ) ;
    DXF_INSERT_FLAGS(insert) = 1 ;
    RETURN ( insert ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_attdef ( file, text_pt, height, text, 
        prompt, tag, flags, len, ang, scale, style_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING text, prompt, tag, style_name ;
REAL text_pt[3], height, ang, scale ;
INT flags, len ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_ATTDEF, file ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( text_pt, DXF_ATTDEF_PT(entity) ) ;
    DXF_ATTDEF_HEIGHT(entity) = height ;
    DXF_ATTDEF_ROT_ANG(entity) = ang ;
    DXF_ATTDEF_X_SCALE(entity) = scale ;
    dx0_strcpy ( DXF_ATTDEF_VALUE(entity), text ) ;
    dx0_strcpy ( DXF_ATTDEF_PROMPT(entity), prompt ) ;
    dx0_strcpy ( DXF_ATTDEF_TAG(entity), tag ) ;
    strcpy ( DXF_ATTDEF_STYLE(entity), 
        style_name == NULL || style_name[0]=='\0' ? "STANDARD" : style_name ) ;

    DXF_ATTDEF_AFLAGS(entity) = flags ;
    DXF_ATTDEF_FLD_LEN(entity) = len ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx0_create_attrib ( file, text_pt, height, text, tag, 
        flags, len, ang, scale, style_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING text, tag, style_name ;
REAL text_pt[3], height, ang, scale ;
INT flags, len ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_ATTRIB, file ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( text_pt, DXF_ATTRIB_PT(entity) ) ;
    DXF_ATTRIB_HEIGHT(entity) = height ;
    DXF_ATTRIB_ROT_ANG(entity) = ang ;
    DXF_ATTRIB_X_SCALE(entity) = scale ;
    dx0_strcpy ( DXF_ATTRIB_VALUE(entity), text ) ;
    dx0_strcpy ( DXF_ATTRIB_TAG(entity), tag ) ;
    strcpy ( DXF_ATTRIB_STYLE(entity), 
        style_name == NULL || style_name[0]=='\0' ? "STANDARD" : style_name ) ;

    DXF_ATTRIB_AFLAGS(entity) = flags ;
    DXF_ATTRIB_FLD_LEN(entity) = len ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx0_create_vertex ( file, pt, bulge, flags )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL pt[3], bulge ;
INT flags ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_VERTEX, file ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( pt, DXF_VERTEX_PT(entity) ) ;
    DXF_VERTEX_BULGE(entity) = bulge ;
    DXF_VERTEX_FLAGS(entity) = flags ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_polyline ( file, flags, type )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT flags, type ;
{
    DXF_ENTITY entity = 
        dx0_alloc_entity ( DXF_POLYLINE, file ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    DXF_PLINE_FLAGS(entity) = flags ;
    DXF_PLINE_TYPE(entity) = type ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_polyline_add_vertex ( file, polyline, pt, 
        bulge, flags )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_ENTITY polyline ;
REAL pt[3], bulge ;
INT flags ;
{
#ifndef __BRLNDC__
    DXF_ENTITY vtx0 = dml_first_record ( DXF_PLINE_VLIST(polyline) ) ;
#endif

    DXF_ENTITY vertex = dx0_create_vertex ( file, pt, bulge, flags ) ;
    if ( vertex == NULL )
        RETURN ( NULL ) ;
    if ( DXF_PLINE_VLIST(polyline) == NULL )
        DXF_PLINE_VLIST(polyline) = dml_create_list();
    if ( DXF_PLINE_VLIST(polyline) == NULL )
        RETURN ( NULL ) ;
    if ( dml_append_data ( DXF_PLINE_VLIST(polyline), vertex ) ==
        NULL )
        RETURN ( NULL ) ;
    DXF_PLINE_VFLAG(polyline) = 1 ;
    if ( flags & DXF_VERTEX_FIT ) 
        DXF_PLINE_FLAGS(polyline) |= DXF_PLINE_FIT ;
    if ( flags & DXF_VERTEX_SPL_FIT ) 
        DXF_PLINE_FLAGS(polyline) |= DXF_PLINE_SPL_FIT ;
    RETURN ( vertex ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_line3d ( file, pt0, pt1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL pt0[3], pt1[3] ;
{
    RETURN ( dxf_create_line ( file, pt0, pt1 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_3dface ( file, pt0, pt1, pt2, pt3, flags ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL pt0[3], pt1[3], pt2[3], pt3[3] ;
INT flags ;
{
    DXF_ENTITY entity = 
        dx0_alloc_entity ( DXF_3DFACE, file ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    if ( pt3 == NULL )
        pt3 = pt2 ;
    dx0_copy_point ( pt0, DXF_3DFACE_PT0(entity) ) ;
    dx0_copy_point ( pt1, DXF_3DFACE_PT1(entity) ) ;
    dx0_copy_point ( pt2, DXF_3DFACE_PT2(entity) ) ;
    dx0_copy_point ( pt3, DXF_3DFACE_PT3(entity) ) ;
    DXF_3DFACE_FLAGS(entity) = flags ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_dimension ( file, name, def_pt, mid_pt, 
    insert_pt, type, text, def_pt3, def_pt4, def_pt5, def_pt6, ldr_len, ang ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING name, text ;
REAL def_pt[3], mid_pt[3], insert_pt[3], def_pt3[3], def_pt4[3], def_pt5[3], 
        def_pt6[3], ldr_len, ang ;
INT type ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_DIM, file ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_strcpy ( DXF_DIM_NAME(entity), name ) ;
    dx0_strcpy ( DXF_DIM_TEXT(entity), text ) ;

    dx0_copy_point ( def_pt, DXF_DIM_DEF_PT(entity) ) ;
    dx0_copy_point ( mid_pt, DXF_DIM_MID_PT(entity) ) ;
    dx0_copy_point ( insert_pt, DXF_DIM_INSERT_PT(entity) ) ;
    dx0_copy_point ( def_pt3, DXF_DIM_DEF_PT3(entity) ) ;
    dx0_copy_point ( def_pt4, DXF_DIM_DEF_PT4(entity) ) ;
    dx0_copy_point ( def_pt5, DXF_DIM_DEF_PT5(entity) ) ;
    dx0_copy_point ( def_pt6, DXF_DIM_DEF_PT6(entity) ) ;
    DXF_DIM_LDR_LEN(entity) = ldr_len ;
    DXF_DIM_ANG(entity) = ang ;
    DXF_DIM_TYPE(entity) = type ;
    RETURN ( entity ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_viewport ( file, ctr_pt, width, height, 
        id, status, target_pt, dir_vec )  
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL ctr_pt[3] ;
REAL width, height ;
INT id, status ;
REAL target_pt[3], dir_vec[3] ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_VIEWPORT, file ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;

    dx0_copy_point ( ctr_pt, DXF_VIEWPORT_CTR_PT(entity) ) ;
    DXF_VIEWPORT_WIDTH(entity) = width ;
    DXF_VIEWPORT_HEIGHT(entity) = height ;
    DXF_VIEWPORT_ID(entity) = id ;
    DXF_VIEWPORT_STATUS(entity) = status ;
    dx0_copy_point ( target_pt, DXF_VIEWPORT_TARGET_PT(entity) ) ;
    dx0_copy_point ( dir_vec, DXF_VIEWPORT_DIR_VEC(entity) ) ;
    dx0_strcpy ( DXF_VIEWPORT_PAPER_SPACE(entity), "ACAD" ) ;
    dx0_strcpy ( DXF_VIEWPORT_DATA_BEGIN(entity), "MVIEW" ) ;
    dx0_strcpy ( DXF_VIEWPORT_WND_DATA(entity), "{" ) ;
    DXF_VIEWPORT_VERSION(entity) = 16 ;
    dx0_strcpy ( DXF_VIEWPORT_FROZEN_BEGIN(entity), "{" ) ;
    DXF_VIEWPORT_LAYER_LIST(entity) = NULL ;
    dx0_strcpy ( DXF_VIEWPORT_FROZEN_END(entity), "}" ) ;
    dx0_strcpy ( DXF_VIEWPORT_DATA_END(entity), "}" ) ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_viewport_append_layer ( viewport, layer ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY viewport ;
STRING layer ;
{
    if ( !DXF_ENTITY_IS_VIEWPORT(viewport) )
        RETURN ( NULL ) ;
    if ( DXF_VIEWPORT_LAYER_LIST(viewport) == NULL )
        DXF_VIEWPORT_LAYER_LIST(viewport) = dml_create_list () ;
    dml_append_data ( DXF_VIEWPORT_LAYER_LIST(viewport), strdup(layer) ) ; 
    RETURN ( viewport ) ;
}

#else
/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_line ( pt0, pt1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL pt0[3], pt1[3] ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_LINE ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( pt0, DXF_LINE_PT0(entity) ) ;
    dx0_copy_point ( pt1, DXF_LINE_PT1(entity) ) ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_point ( pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL pt[3] ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_POINT ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( pt, DXF_POINT_PT(entity) ) ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_circle ( ctr, rad ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL ctr[3], rad ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_CIRCLE ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( ctr, DXF_CIRCLE_CTR(entity) ) ;
    DXF_CIRCLE_RAD(entity) = rad ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_arc ( ctr, rad, ang0, ang1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL ctr[3], rad, ang0, ang1 ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_ARC ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( ctr, DXF_ARC_CTR(entity) ) ;
    DXF_ARC_RAD(entity) = rad ;
    DXF_ARC_ANG0(entity) = ang0 ;
    DXF_ARC_ANG1(entity) = ang1 ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_trace ( pt0, pt1, pt2, pt3 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL pt0[3], pt1[3], pt2[3], pt3[3] ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_TRACE ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( pt0, DXF_TRACE_PT0(entity) ) ;
    dx0_copy_point ( pt1, DXF_TRACE_PT1(entity) ) ;
    dx0_copy_point ( pt2, DXF_TRACE_PT2(entity) ) ;
    dx0_copy_point ( pt3, DXF_TRACE_PT3(entity) ) ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_solid ( pt0, pt1, pt2, pt3 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL pt0[3], pt1[3], pt2[3], pt3[3] ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_SOLID ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    if ( pt3 == NULL ) 
        pt3 = pt2 ;
    dx0_copy_point ( pt0, DXF_SOLID_PT0(entity) ) ;
    dx0_copy_point ( pt1, DXF_SOLID_PT1(entity) ) ;
    dx0_copy_point ( pt2, DXF_SOLID_PT2(entity) ) ;
    dx0_copy_point ( pt3, DXF_SOLID_PT3(entity) ) ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_text ( insert_pt, height, text, ang, scale, 
        style_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL insert_pt[3], height, ang, scale ;
STRING text, style_name ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_TEXT ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( insert_pt, DXF_TEXT_INS_PT(entity) ) ;
    DXF_TEXT_HEIGHT(entity) = height ;
    DXF_TEXT_ROT_ANG(entity) = ang ;
    DXF_TEXT_X_SCALE(entity) = scale ;
    dx0_strcpy ( DXF_TEXT_VALUE(entity), text ) ;
    strcpy ( DXF_TEXT_STYLE(entity), 
        style_name == NULL || style_name[0]=='\0' ? "STANDARD" : style_name ) ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_shape ( insert_pt, size, name, ang, scale ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL insert_pt[3], size, ang, scale ;
STRING name ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_SHAPE ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( insert_pt, DXF_SHAPE_INSERT_PT(entity) ) ;
    DXF_SHAPE_SIZE(entity) = size ;
    DXF_SHAPE_ROT_ANG(entity) = ang ;
    DXF_SHAPE_X_SCALE(entity) = scale ;
    dx0_strcpy ( DXF_SHAPE_NAME(entity), name ) ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_insert ( block, insert_pt, scale, ang, 
        cols_cnt, rows_cnt, col_spc, row_spc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING block ;
REAL insert_pt[3], scale, ang, col_spc, row_spc ;
INT cols_cnt, rows_cnt ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_INSERT ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( insert_pt, DXF_INSERT_PT(entity) ) ;
    DXF_INSERT_ROT_ANG(entity) = ang ;
    DXF_INSERT_X_SCALE(entity) = scale ;
    DXF_INSERT_Y_SCALE(entity) = scale ;
    DXF_INSERT_Z_SCALE(entity) = scale ;
    dx0_strcpy ( DXF_INSERT_BLOCK(entity), block ) ;
    DXF_INSERT_FLAGS(entity) = 0 ;
    DXF_INSERT_COLS_CNT(entity) = cols_cnt ;
    DXF_INSERT_ROWS_CNT(entity) = rows_cnt ;
    DXF_INSERT_COL_SPC(entity) = col_spc ;
    DXF_INSERT_ROW_SPC(entity) = row_spc ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_insert_add_attrib ( insert, text_pt, height, 
    text, tag, flags, len, ang, scale, style_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY insert ;
STRING text, tag, style_name ;
REAL text_pt[3], height, ang, scale ;
INT flags, len ;
{
    DXF_ENTITY attrib ;
    if ( DXF_INSERT_ATTRS(insert) == NULL )
        DXF_INSERT_ATTRS(insert) = dml_create_list ();
    if ( DXF_INSERT_ATTRS(insert) == NULL )
        RETURN ( NULL ) ;

    attrib = dx0_create_attrib ( text_pt, height, text, tag, flags, 
        len, ang, scale, style_name ) ;

    if ( attrib == NULL || 
        dml_append_data ( DXF_INSERT_ATTRS(insert), attrib ) == NULL )
        RETURN ( NULL ) ;
    DXF_INSERT_FLAGS(insert) = 1 ;
    RETURN ( insert ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_attdef ( text_pt, height, text, prompt, tag, 
        flags, len, ang, scale, style_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING text, prompt, tag, style_name ;
REAL text_pt[3], height, ang, scale ;
INT flags, len ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_ATTDEF ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( text_pt, DXF_ATTDEF_PT(entity) ) ;
    DXF_ATTDEF_HEIGHT(entity) = height ;
    DXF_ATTDEF_ROT_ANG(entity) = ang ;
    DXF_ATTDEF_X_SCALE(entity) = scale ;
    dx0_strcpy ( DXF_ATTDEF_VALUE(entity), text ) ;
    dx0_strcpy ( DXF_ATTDEF_PROMPT(entity), prompt ) ;
    dx0_strcpy ( DXF_ATTDEF_TAG(entity), tag ) ;
    strcpy ( DXF_ATTDEF_STYLE(entity), 
        style_name == NULL || style_name[0]=='\0' ? "STANDARD" : style_name ) ;

    DXF_ATTDEF_AFLAGS(entity) = flags ;
    DXF_ATTDEF_FLD_LEN(entity) = len ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx0_create_attrib ( text_pt, height, text, tag, flags, 
        len, ang, scale, style_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING text, tag, style_name ;
REAL text_pt[3], height, ang, scale ;
INT flags, len ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_ATTRIB ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( text_pt, DXF_ATTRIB_PT(entity) ) ;
    DXF_ATTRIB_HEIGHT(entity) = height ;
    DXF_ATTRIB_ROT_ANG(entity) = ang ;
    DXF_ATTRIB_X_SCALE(entity) = scale ;
    dx0_strcpy ( DXF_ATTRIB_VALUE(entity), text ) ;
    dx0_strcpy ( DXF_ATTRIB_TAG(entity), tag ) ;
    strcpy ( DXF_ATTRIB_STYLE(entity), 
        style_name == NULL || style_name[0]=='\0' ? "STANDARD" : style_name ) ;

    DXF_ATTRIB_AFLAGS(entity) = flags ;
    DXF_ATTRIB_FLD_LEN(entity) = len ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx0_create_vertex ( pt, bulge, flags )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL pt[3], bulge ;
INT flags ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_VERTEX ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_copy_point ( pt, DXF_VERTEX_PT(entity) ) ;
    DXF_VERTEX_BULGE(entity) = bulge ;
    DXF_VERTEX_FLAGS(entity) = flags ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_polyline ( flags, type )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT flags, type ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_POLYLINE ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    DXF_PLINE_FLAGS(entity) = flags ;
    DXF_PLINE_TYPE(entity) = type ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_polyline_add_vertex ( polyline, pt, bulge, flags )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY polyline ;
REAL pt[3], bulge ;
INT flags ;
{
    DXF_ENTITY vertex = dx0_create_vertex ( pt, bulge, flags ) ;
    if ( vertex == NULL )
        RETURN ( NULL ) ;
    if ( DXF_PLINE_VLIST(polyline) == NULL )
        DXF_PLINE_VLIST(polyline) = dml_create_list();
    if ( DXF_PLINE_VLIST(polyline) == NULL )
        RETURN ( NULL ) ;
    if ( dml_append_data ( DXF_PLINE_VLIST(polyline), vertex ) ==
        NULL )
        RETURN ( NULL ) ;
    DXF_PLINE_VFLAG(polyline) = 1 ;
    if ( flags & DXF_VERTEX_FIT ) 
        DXF_PLINE_FLAGS(polyline) |= DXF_PLINE_FIT ;
    if ( flags & DXF_VERTEX_SPL_FIT ) 
        DXF_PLINE_FLAGS(polyline) |= DXF_PLINE_SPL_FIT ;
    RETURN ( vertex ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_line3d ( pt0, pt1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL pt0[3], pt1[3] ;
{
    RETURN ( dxf_create_line ( pt0, pt1 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_3dface ( pt0, pt1, pt2, pt3, flags ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL pt0[3], pt1[3], pt2[3], pt3[3] ;
INT flags ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_3DFACE ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    if ( pt3 == NULL )
        pt3 = pt2 ;
    dx0_copy_point ( pt0, DXF_3DFACE_PT0(entity) ) ;
    dx0_copy_point ( pt1, DXF_3DFACE_PT1(entity) ) ;
    dx0_copy_point ( pt2, DXF_3DFACE_PT2(entity) ) ;
    dx0_copy_point ( pt3, DXF_3DFACE_PT3(entity) ) ;
    DXF_3DFACE_FLAGS(entity) = flags ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_create_dimension ( name, def_pt, mid_pt, insert_pt, 
    type, text, def_pt3, def_pt4, def_pt5, def_pt6, ldr_len, ang ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING name, text ;
REAL def_pt[3], mid_pt[3], insert_pt[3], def_pt3[3], def_pt4[3], def_pt5[3], 
        def_pt6[3], ldr_len, ang ;
INT type ;
{
    DXF_ENTITY entity = dx0_alloc_entity ( DXF_DIM ) ;
    if ( entity == NULL )
        RETURN ( NULL ) ;
    dx0_strcpy ( DXF_DIM_NAME(entity), name ) ;
    dx0_strcpy ( DXF_DIM_TEXT(entity), text ) ;

    dx0_copy_point ( def_pt, DXF_DIM_DEF_PT(entity) ) ;
    dx0_copy_point ( mid_pt, DXF_DIM_MID_PT(entity) ) ;
    dx0_copy_point ( insert_pt, DXF_DIM_INSERT_PT(entity) ) ;
    dx0_copy_point ( def_pt3, DXF_DIM_DEF_PT3(entity) ) ;
    dx0_copy_point ( def_pt4, DXF_DIM_DEF_PT4(entity) ) ;
    dx0_copy_point ( def_pt5, DXF_DIM_DEF_PT5(entity) ) ;
    dx0_copy_point ( def_pt6, DXF_DIM_DEF_PT6(entity) ) ;
    DXF_DIM_LDR_LEN(entity) = ldr_len ;
    DXF_DIM_ANG(entity) = ang ;
    DXF_DIM_TYPE(entity) = type ;
    RETURN ( entity ) ;
}
#endif

