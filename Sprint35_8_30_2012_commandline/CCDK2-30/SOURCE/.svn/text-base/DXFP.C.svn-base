/* -2 -3 -x */
/******************************* DXFP.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#include <dmldefs.h>
#include <dx0defs.h>
#include <dxfilm.h>
#include <dxpos.h>

/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_open_header ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    if ( DX_FILE_HEADER_OPEN(file) || DX_FILE_TABLES_OPEN(file) ||
        DX_FILE_BLOCKS_OPEN(file) || DX_FILE_ENTITIES_OPEN(file) ||
        DX_FILE_HEADER_CLOSED(file) || DX_FILE_TABLES_CLOSED(file) ||
        DX_FILE_BLOCKS_CLOSED(file) || DX_FILE_ENTITIES_CLOSED(file) )
        RETURN ( FALSE ) ;
    DX_FILE_SET_HEADER_OPEN(file) ;
    RETURN ( dx0_put_string_group ( file, 0, "SECTION" ) &&
             dx0_put_string_group ( file, 2, "HEADER" ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_open_tables ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    if ( !DX_FILE_HEADER_OPEN(file) && !dxf_open_header ( file ) ) 
        RETURN ( FALSE ) ;
    if ( !DX_FILE_HEADER_CLOSED(file) && !dxf_close_header ( file ) ) 
        RETURN ( FALSE ) ;
    if ( DX_FILE_TABLES_OPEN(file) || DX_FILE_BLOCKS_OPEN(file) || 
        DX_FILE_ENTITIES_OPEN(file) || DX_FILE_TABLES_CLOSED(file) ||
        DX_FILE_BLOCKS_CLOSED(file) || DX_FILE_ENTITIES_CLOSED(file) )
        RETURN ( FALSE ) ;
    DX_FILE_SET_TABLES_OPEN(file) ;
    RETURN ( dx0_put_string_group ( file, 0, "SECTION" ) &&
             dx0_put_string_group ( file, 2, "TABLES" ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_open_blocks ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
DXF_FILE file ; 
{ 
    if ( !DX_FILE_TABLES_OPEN(file) && !dxf_open_tables ( file ) ) 
        RETURN ( FALSE ) ;
    if ( !DX_FILE_TABLES_CLOSED(file) && !dxf_close_tables ( file ) ) 
        RETURN ( FALSE ) ;
    if ( DX_FILE_BLOCKS_OPEN(file) || DX_FILE_ENTITIES_OPEN(file) ||
        DX_FILE_BLOCKS_CLOSED(file) || DX_FILE_ENTITIES_CLOSED(file) )
        RETURN ( FALSE ) ;
    DX_FILE_SET_BLOCKS_OPEN(file) ;
    RETURN ( dx0_put_string_group ( file, 0, "SECTION" ) &&
             dx0_put_string_group ( file, 2, "BLOCKS" ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_open_block ( file, name, layer, flags, base_pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
DXF_FILE file ; 
STRING name, layer ;
INT flags ;
REAL base_pt[3] ;
{
    DXF_BLOCKPOS blockpos ;

    if ( !DX_FILE_BLOCKS_OPEN(file) && !dxf_open_blocks ( file ) ) 
        RETURN ( FALSE ) ;
    if ( DX_FILE_BLOCKS_CLOSED(file) || DX_FILE_ENTITIES_OPEN(file) || 
        DX_FILE_ENTITIES_CLOSED(file) )
        RETURN ( FALSE ) ;
    if ( DX_FILE_BLOCKPOS_LIST(file) == NULL ) 
        DX_FILE_BLOCKPOS_LIST(file) = dml_create_list();
    if ( DX_FILE_BLOCKPOS_LIST(file) == NULL ) 
        RETURN ( FALSE ) ;
    blockpos = DX_CREATE_BLOCKPOS ;
    if ( blockpos == NULL ) 
        RETURN ( FALSE ) ;
    dx0_strcpy ( DX_BLOCKPOS_NAME(blockpos), name ) ;
    if ( !dx0_getpos ( file, &DX_BLOCKPOS_FPOS(blockpos) ) )
        RETURN ( FALSE ) ;
    if ( dml_append_data ( DX_FILE_BLOCKPOS_LIST(file), blockpos ) == NULL )
        RETURN ( FALSE ) ;
    RETURN ( dx0_put_string_group ( file, 0, "BLOCK" ) && 
        dx0_put_string_group ( file, 8, layer ) && 
        dx0_put_string_group ( file, 2, name ) && 
        dx0_put_int_group ( file, 70, flags ) && 
        dx0_put_real_group ( file, 10, base_pt[0] ) && 
        dx0_put_real_group ( file, 20, base_pt[1] ) && 
        dx0_put_real_group ( file, 30, base_pt[2] ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_open_entities ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    if ( !DX_FILE_BLOCKS_OPEN(file) && !dxf_open_blocks ( file ) ) 
        RETURN ( FALSE ) ;
    if ( !DX_FILE_BLOCKS_CLOSED(file) && !dxf_close_blocks ( file ) ) 
        RETURN ( FALSE ) ;
    if ( DX_FILE_ENTITIES_OPEN(file) || DX_FILE_ENTITIES_CLOSED(file) )
        RETURN ( FALSE ) ;
    DX_FILE_SET_ENTITIES_OPEN(file) ;
    RETURN ( dx0_put_string_group ( file, 0, "SECTION" ) &&
             dx0_put_string_group ( file, 2, "ENTITIES" ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_close_header ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    if ( !DX_FILE_HEADER_OPEN(file) || DX_FILE_TABLES_OPEN(file) || 
        DX_FILE_BLOCKS_OPEN(file) || DX_FILE_ENTITIES_OPEN(file) || 
        DX_FILE_HEADER_CLOSED(file) || DX_FILE_TABLES_CLOSED(file) || 
        DX_FILE_BLOCKS_CLOSED(file) || DX_FILE_ENTITIES_CLOSED(file) )
        RETURN ( FALSE ) ;
    DX_FILE_SET_HEADER_CLOSED(file) ;
    RETURN ( dx0_put_string_group ( file, 0, "ENDSEC" ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_close_tables ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    if ( !DX_FILE_TABLES_OPEN(file) || DX_FILE_BLOCKS_OPEN(file) || 
        DX_FILE_ENTITIES_OPEN(file) || DX_FILE_TABLES_CLOSED(file) || 
        DX_FILE_BLOCKS_CLOSED(file) || DX_FILE_ENTITIES_CLOSED(file) )
        RETURN ( FALSE ) ;
    if ( DX_FILE_LAST_OPEN(file) && !dxf_close_table ( file ) ) 
        RETURN ( FALSE ) ;
    DX_FILE_SET_TABLES_CLOSED(file) ;
    RETURN ( dx0_put_string_group ( file, 0, "ENDSEC" ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_close_table ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    if ( !DX_FILE_TABLES_OPEN(file) || DX_FILE_BLOCKS_OPEN(file) || 
        DX_FILE_ENTITIES_OPEN(file) || DX_FILE_TABLES_CLOSED(file) || 
        DX_FILE_BLOCKS_CLOSED(file) || DX_FILE_ENTITIES_CLOSED(file) ||
        DX_FILE_LAST_CLOSED(file) ) 
        RETURN ( FALSE ) ;
    DX_FILE_SET_LAST_CLOSED(file) ;
    RETURN ( dx0_put_string_group ( file, 0, "ENDTAB" ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_close_blocks ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
DXF_FILE file ; 
{ 

    if ( !DX_FILE_BLOCKS_OPEN(file) || DX_FILE_ENTITIES_OPEN(file) || 
        DX_FILE_BLOCKS_CLOSED(file) || DX_FILE_ENTITIES_CLOSED(file) )
        RETURN ( FALSE ) ;
    if ( DX_FILE_LAST_OPEN(file) && !dxf_close_block ( file ) ) 
        RETURN ( FALSE ) ;
    DX_FILE_SET_BLOCKS_CLOSED(file) ;
    RETURN ( dx0_put_string_group ( file, 0, "ENDSEC" ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_close_entities ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    if ( !DX_FILE_ENTITIES_OPEN(file) || DX_FILE_ENTITIES_CLOSED(file) )
        RETURN ( FALSE ) ;
    if ( DX_FILE_SEQ_OPEN(file) )
        dx0_put_seqend ( file ) ;
    DX_FILE_SET_ENTITIES_CLOSED(file) ;
    RETURN ( dx0_put_string_group ( file, 0, "ENDSEC" ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_close_block ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    if ( !DX_FILE_BLOCKS_OPEN(file) || DX_FILE_ENTITIES_OPEN(file) || 
        DX_FILE_BLOCKS_CLOSED(file) || DX_FILE_ENTITIES_CLOSED(file) ) 
        RETURN ( FALSE ) ;
    if ( DX_FILE_SEQ_OPEN(file) )
        dx0_put_seqend ( file ) ;
    DX_FILE_SET_LAST_CLOSED(file) ;
    RETURN ( dx0_put_string_group ( file, 0, "ENDBLK" ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_comment ( file, comment )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING comment ;
{
    RETURN ( dx0_put_string_group ( file, 999, comment ) ) ;
}

