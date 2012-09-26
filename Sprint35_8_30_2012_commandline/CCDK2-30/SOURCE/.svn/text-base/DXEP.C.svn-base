/* -2 -3 -x */
/******************************* DXEP.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#include <dxents.h>
#include <dx0defs.h>
#include <dxfilm.h>
#include <dmldefs.h>

STATIC BOOLEAN dxf_put_polyline_vtcs __(( DXF_FILE, DXF_ENTITY )) ;
STATIC BOOLEAN dxf_put_insert_attrs __(( DXF_FILE, DXF_ENTITY )) ;
STATIC BOOLEAN dxf_check_block __(( DXF_FILE, STRING )) ;

/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_entity ( file, entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_ENTITY entity ;
{ 
    BOOLEAN put_6, put_62 ;
#ifdef __ACAD11__
    int *template = dx0_entity_template ( DX_ENTITY_ID(entity), 
        dx0_file_release ( file ) ) ;
#else
    int *template = dx0_entity_template ( DX_ENTITY_ID(entity) ) ;
#endif
    if ( !DX_FILE_BLOCKS_OPEN(file) && !DX_FILE_ENTITIES_OPEN(file) ) {
        if ( !dxf_open_entities ( file ) )
            RETURN ( FALSE ) ;
    }
    if ( DX_FILE_BLOCKS_CLOSED(file) && 
        ( !DX_FILE_ENTITIES_OPEN(file) || DX_FILE_ENTITIES_CLOSED(file) ) )
        RETURN ( FALSE ) ;
    if ( template == NULL || entity == NULL ) 
        RETURN ( FALSE ) ;
    if ( DXF_ENTITY_IS_INSERT(entity) && 
        !dxf_check_block ( file, DXF_INSERT_BLOCK(entity) ) )
        RETURN ( FALSE ) ;
    if ( DX_FILE_SEQ_OPEN(file) && !DXF_ENTITY_IS_VERTEX(entity) && 
        !DXF_ENTITY_IS_ATTRIB(entity) )
        dx0_put_seqend ( file ) ;
    if ( !dx0_put_string_group ( file, 0, 
        dx0_entity_name ( DX_ENTITY_ID(entity) ) ) )
        RETURN ( FALSE ) ;
    put_6 = ( DXF_ENTITY_LTYPE_STATUS(entity) == DXF_LTYPE_BYENT ) ;
    put_62 = ( DXF_ENTITY_COLOR_STATUS(entity) == DXF_COLOR_BYENT ) ;
    if ( !dx0_put_object ( file, DX_ENTITY_ANY(entity), template, 
        put_6, put_62 ) )
        RETURN ( FALSE ) ;
#ifdef __ACAD11__
    dx0_put_appl_data ( file, entity ) ;
#endif
    dx0_strcpy ( DX_FILE_ENT_LAYER(file), DXF_ENTITY_LAYER(entity) ) ;
    if ( DXF_ENTITY_IS_PLINE(entity) ) 
        RETURN ( dxf_put_polyline_vtcs ( file, entity ) ) ;
    else if ( DXF_ENTITY_IS_INSERT(entity) ) 
        RETURN ( dxf_put_insert_attrs ( file, entity ) ) ;
    if ( DXF_ENTITY_IS_VERTEX(entity) || DXF_ENTITY_IS_ATTRIB(entity) ) 
        DX_FILE_SET_SEQ_OPEN(file) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN dxf_put_polyline_vtcs ( file, polyline ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_ENTITY polyline ;
{ 
    DML_ITEM item ;
    if ( dml_length ( DXF_PLINE_VLIST(polyline) ) == 0 ) 
        RETURN ( TRUE ) ;
    DML_WALK_LIST ( DXF_PLINE_VLIST(polyline), item ) {
        if ( !dxf_put_entity ( file, DML_RECORD(item) ) )
            RETURN ( FALSE ) ;
    }
    RETURN ( dx0_put_seqend ( file ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN dxf_check_block ( file, blockname ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING blockname ;
{ 
    DML_ITEM item ;
    DML_LIST blist = DX_FILE_BLOCKPOS_LIST(file) ;
    DXF_BLOCKPOS bpos ;

    if ( DX_FILE_BLOCKS_OPEN(file) && !DX_FILE_BLOCKS_CLOSED(file) ) 
        RETURN ( TRUE ) ;

    if ( blist == NULL ) 
        RETURN ( FALSE ) ;
    DML_WALK_LIST ( blist, item ) {
        bpos = (DXF_BLOCKPOS)DML_RECORD(item) ;
        if ( ( strcmp ( DX_BLOCKPOS_NAME(bpos), blockname ) == 0 ) )
            RETURN ( TRUE ) ;
    }
    RETURN ( FALSE ) ;
/*
    if ( blist == NULL ) 
        RETURN ( FALSE ) ;
    for ( item = DML_FIRST(blist) ; item != NULL && !found ; 
        item = DML_NEXT(item) ) {
        bpos = (DXF_BLOCKPOS)DML_RECORD(item) ;
        found = ( strcmp ( DX_BLOCKPOS_NAME(bpos), blockname ) == 0 ) ;
    }
    if ( !found ) 
        RETURN ( FALSE ) ;
    if ( DX_FILE_BLOCKS_OPEN(file) && !DX_FILE_BLOCKS_CLOSED(file) ) {
        bpos = (DXF_BLOCKPOS)DML_LAST_RECORD(blist) ;
        if ( strcmp ( DX_BLOCKPOS_NAME(bpos), blockname ) == 0 ) 
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
*/
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN dxf_put_insert_attrs ( file, insert ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_ENTITY insert ;
{ 
    DML_ITEM item ;

    if ( dml_length ( DXF_INSERT_ATTRS(insert) ) == 0 ) 
        RETURN ( TRUE ) ;
    DML_WALK_LIST ( DXF_INSERT_ATTRS(insert), item ) {
        if ( !dxf_put_entity ( file, DML_RECORD(item) ) )
            RETURN ( FALSE ) ;
    }
    RETURN ( dx0_put_seqend ( file ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_entities ( file, entitieslist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DML_LIST entitieslist ;
{ 
    DML_ITEM item ;
    BOOLEAN status = TRUE ;

    DML_WALK_LIST ( entitieslist, item ) {
        if ( !dxf_put_entity ( file, DML_RECORD(item) ) )
            status = FALSE ;
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_write_entities ( filename, file_type, entitieslist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
DXF_FILE_MODE file_type ;
DML_LIST entitieslist ;
{ 
    DXF_FILE file = dxf_fopen ( filename, DXF_WRITE | file_type ) ;
    BOOLEAN status = TRUE ;

    if ( file == NULL )
        RETURN ( FALSE ) ;

    if ( !dxf_open_entities ( file ) )
        RETURN ( FALSE ) ;
    if ( !dxf_put_entities ( file, entitieslist ) )
        status = FALSE ;
    if ( !dxf_close_entities ( file ) )
        status = FALSE ;
    if ( !dxf_fclose ( file ) )
        status = FALSE ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_line ( file, pt0, pt1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL pt0[3], pt1[3] ;
{
    DXF_ENTITY entity ;
    BOOLEAN status ;
#ifdef __ACAD11__
    entity = dxf_create_line ( file, pt0, pt1 ) ;
#else
    entity = dxf_create_line ( pt0, pt1 ) ;
#endif
    status = ( entity != NULL ) && dxf_put_entity ( file, entity ) ;
    dxf_free_entity ( entity ) ;
    RETURN ( status ) ;
}
 

/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_point ( file, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL pt[3] ;
{
    DXF_ENTITY entity ;
    BOOLEAN status ;
#ifdef __ACAD11__
    entity = dxf_create_point ( file, pt ) ;
#else
    entity = dxf_create_point ( pt ) ;
#endif
    status = ( entity != NULL ) && dxf_put_entity ( file, entity ) ;
    dxf_free_entity ( entity ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_circle ( file, ctr, rad ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL ctr[3], rad ;
{
    DXF_ENTITY entity ;
    BOOLEAN status ;
#ifdef __ACAD11__
    entity = dxf_create_circle ( file, ctr, rad ) ;
#else
    entity = dxf_create_circle ( ctr, rad ) ;
#endif
    status = ( entity != NULL ) && dxf_put_entity ( file, entity ) ;
    dxf_free_entity ( entity ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_arc ( file, ctr, rad, ang0, ang1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL ctr[3], rad, ang0, ang1 ;
{
    DXF_ENTITY entity ;
    BOOLEAN status ;
#ifdef __ACAD11__
    entity = dxf_create_arc ( file, ctr, rad, ang0, ang1 ) ;
#else
    entity = dxf_create_arc ( ctr, rad, ang0, ang1 ) ;
#endif
    status = ( entity != NULL ) && dxf_put_entity ( file, entity ) ;
    dxf_free_entity ( entity ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_trace ( file, pt0, pt1, pt2, pt3 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL pt0[3], pt1[3], pt2[3], pt3[3] ;
{
    DXF_ENTITY entity ;
    BOOLEAN status ;
#ifdef __ACAD11__
    entity = dxf_create_trace ( file, pt0, pt1, pt2, pt3 ) ;
#else
    entity = dxf_create_trace ( pt0, pt1, pt2, pt3 ) ;
#endif
    status = ( entity != NULL ) && dxf_put_entity ( file, entity ) ;
    dxf_free_entity ( entity ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_solid ( file, pt0, pt1, pt2, pt3 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL pt0[3], pt1[3], pt2[3], pt3[3] ;
{
    DXF_ENTITY entity ;
    BOOLEAN status ;
#ifdef __ACAD11__
    entity = dxf_create_solid ( file, pt0, pt1, pt2, pt3 ) ;
#else
    entity = dxf_create_solid ( pt0, pt1, pt2, pt3 ) ;
#endif
    status = ( entity != NULL ) && dxf_put_entity ( file, entity ) ;
    dxf_free_entity ( entity ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_text ( file, insert_pt, height, text, 
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
    DXF_ENTITY entity ;
    BOOLEAN status ;
#ifdef __ACAD11__
    entity = dxf_create_text ( file, insert_pt, height, text, 
            ang, scale, style_name ) ;
#else
    entity = dxf_create_text ( insert_pt, height, text, 
        ang, scale, style_name ) ;
#endif
    status = ( entity != NULL ) && dxf_put_entity ( file, entity ) ;
    dxf_free_entity ( entity ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_shape ( file, insert_pt, size, name, 
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
    DXF_ENTITY entity ;
    BOOLEAN status ;
#ifdef __ACAD11__
    entity = dxf_create_shape ( file, insert_pt, size, name, ang, scale ) ;
#else
    entity = dxf_create_shape ( insert_pt, size, name, ang, scale ) ;
#endif
    status = ( entity != NULL ) && dxf_put_entity ( file, entity ) ;
    dxf_free_entity ( entity ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_insert ( file, block, insert_pt, scale, ang, 
        cols_cnt, rows_cnt, col_spc, row_spc, attrib_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING block ;
REAL insert_pt[3], scale, ang, col_spc, row_spc ;
INT cols_cnt, rows_cnt ;
DML_LIST attrib_list ;
{
    DXF_ENTITY entity ;
    BOOLEAN status ;
#ifdef __ACAD11__
    entity = dxf_create_insert ( file, block, insert_pt, scale, ang, 
        cols_cnt, rows_cnt, col_spc, row_spc ) ;
#else
    entity = dxf_create_insert ( block, insert_pt, scale, ang, 
        cols_cnt, rows_cnt, col_spc, row_spc ) ;
#endif
    DXF_INSERT_ATTRS(entity) = attrib_list ;
    status = ( entity != NULL ) && dxf_put_entity ( file, entity ) ;
    dxf_free_entity ( entity ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_attdef ( file, text_pt, height, text, 
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
    DXF_ENTITY entity ;
    BOOLEAN status ;
#ifdef __ACAD11__
    entity = dxf_create_attdef ( file, text_pt, height, text, 
        prompt, tag, flags, len, ang, scale, style_name ) ;
#else
    entity = dxf_create_attdef ( text_pt, height, text, 
        prompt, tag, flags, len, ang, scale, style_name ) ;
#endif
    status = ( entity != NULL ) && dxf_put_entity ( file, entity ) ;
    dxf_free_entity ( entity ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_polyline ( file, flags, type, vtx_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT flags, type ;
DML_LIST vtx_list ;
{
    DXF_ENTITY entity ;
    BOOLEAN status ;
#ifdef __ACAD11__
    entity = dxf_create_polyline ( file, flags, type ) ;
#else
    entity = dxf_create_polyline ( flags, type ) ;
#endif
    DXF_PLINE_VLIST(entity) = vtx_list ;
    status = ( entity != NULL ) && dxf_put_entity ( file, entity ) ;
    dxf_free_entity ( entity ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_line3d ( file, pt0, pt1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL pt0[3], pt1[3] ;
{
    RETURN ( dxf_put_line ( file, pt0, pt1 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_3dface ( file, pt0, pt1, pt2, pt3, flags ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL pt0[3], pt1[3], pt2[3], pt3[3] ;
INT flags ;
{
    DXF_ENTITY entity ;
    BOOLEAN status ;
#ifdef __ACAD11__
    entity = dxf_create_3dface ( file, pt0, pt1, pt2, pt3, flags ) ;
#else
    entity = dxf_create_3dface ( pt0, pt1, pt2, pt3, flags ) ;
#endif
    status = ( entity != NULL ) && dxf_put_entity ( file, entity ) ;
    dxf_free_entity ( entity ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_dimension ( file, name, def_pt, mid_pt, 
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
    DXF_ENTITY entity ;
    BOOLEAN status ;
#ifdef __ACAD11__
    entity = dxf_create_dimension ( file, name, def_pt, mid_pt, insert_pt, 
        type, text, def_pt3, def_pt4, def_pt5, def_pt6, ldr_len, ang ) ;
#else
    entity = dxf_create_dimension ( name, def_pt, mid_pt, insert_pt, 
        type, text, def_pt3, def_pt4, def_pt5, def_pt6, ldr_len, ang ) ;
#endif
    status = ( entity != NULL ) && dxf_put_entity ( file, entity ) ;
    dxf_free_entity ( entity ) ;
    RETURN ( status ) ;
}


#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_viewport ( file, ctr_pt, width, height, 
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
    DXF_ENTITY entity ;
    BOOLEAN s ;
    entity = dxf_create_viewport ( file, ctr_pt, width, height, 
        id, status, target_pt, dir_vec ) ;
    s = ( entity != NULL ) && dxf_put_entity ( file, entity ) ;
    dxf_free_entity ( entity ) ;
    RETURN ( s ) ;
}
#endif

