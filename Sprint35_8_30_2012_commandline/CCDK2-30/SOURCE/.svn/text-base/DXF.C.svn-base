/* -2 -3 -x */
/******************************* DXF.C **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#include <stdio.h>
#include <dmldefs.h>
#include <dx0defs.h>
#include <dxrdefs.h>
#include <dxfilm.h>
#include <dxpos.h>

STATIC void dx0_fopen_init __(( DXF_FILE )) ;
STATIC void dx0_free_unrec __(( fpos_t* )) ;

/*-------------------------------------------------------------------------*/
STATIC void dx0_fopen_init ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    DX_FILE_PRECISION(file) = DX_DEFAULT_PRECISION ; 
#ifdef __ACAD11__
    DX_FILE_RELEASE(file) = DXF_ACAD_RELEASE ; 
    DX_FILE_APPDEF_LIST(file) = NULL ;
#endif
    DX_FILE_FLAGS(file) = 0 ;
    DX_FILE_HEADER_POS(file)  = 0 ;
    DX_FILE_TABLES_POS(file)  = 0 ;
    DX_FILE_BLOCKS_POS(file)  = 0 ;
    DX_FILE_ENTITIES_POS(file)= 0 ;
    DX_FILE_BLOCKPOS_LIST(file) = NULL ;
    DX_FILE_SPLINESEGS(file) = DXF_SPLINESEGS ;
    DX_FILE_LENGTH(file) = 0 ; 
    DX_FILE_UNREC_LIST(file) = NULL ;
    DX_FILE_STATUS(file) = DXF_FILE_INIT ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_FILE dxf_fopen ( filename, mode )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ; 
DXF_FILE_MODE mode ;
{ 
    DXF_FILE file ;
    char sentinel[22];     /* Binary DXF file sentinel buffer */

    file = DX_CREATE_FILE ;
    if ( file == NULL ) 
        RETURN ( NULL ) ;
    DX_FILE_TYPE(file) = mode ; 
    if ( mode & DXF_READ ) {
        DX_FILE_HANDLE(file) = fopen ( filename, "rb" ) ; 
        if ( DX_FILE_HANDLE(file) == NULL ) {
            DX_FREE_FILE ( file ) ;
            RETURN ( NULL ) ;
        }
        if ( ( fread ( sentinel, 1, 22, DX_FILE_HANDLE(file) ) != 22 ) ||
            strncmp ( sentinel, DX_BINARY_SENTINEL, 22 ) ) {
            rewind ( DX_FILE_HANDLE(file) ) ;
            DX_FILE_TYPE(file) |= DXF_ASCII ;
        }
        else 
            DX_FILE_TYPE(file) |= DXF_BINARY ;
        dx0_fopen_init ( file ) ;
        RETURN ( file ) ;
    }

    else if ( mode & DXF_WRITE ) {
        if ( mode & DXF_ASCII ) 
            DX_FILE_HANDLE(file) = fopen ( filename, "w" ) ; 
        else if ( mode & DXF_BINARY ) {
            DX_FILE_HANDLE(file) = fopen ( filename, "wb" ) ; 
            fwrite ( DX_BINARY_SENTINEL, 1, 22, DX_FILE_HANDLE(file) ) ;
        }
        if ( DX_FILE_HANDLE(file) == NULL ) {
            DX_FREE_FILE ( file ) ;
            RETURN ( NULL ) ;
        }
        dx0_fopen_init ( file ) ;
        RETURN ( file ) ;
    }
    else 
        RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_fclose ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    INT status ;

    if ( file == NULL ) 
        RETURN ( TRUE ) ;
    if ( DX_FILE_TYPE(file) & DXF_WRITE ) {
        if ( !DX_FILE_HEADER_OPEN(file) && !dxf_open_header ( file ) )
            RETURN ( FALSE ) ; 
        if ( !DX_FILE_HEADER_CLOSED(file) && !dxf_close_header ( file ) )
            RETURN ( FALSE ) ; 
        if ( !DX_FILE_TABLES_OPEN(file) && !dxf_open_tables ( file ) )
            RETURN ( FALSE ) ; 
        if ( !DX_FILE_TABLES_CLOSED(file) && !dxf_close_tables ( file ) )
            RETURN ( FALSE ) ; 
        if ( !DX_FILE_BLOCKS_OPEN(file) && !dxf_open_blocks ( file ) )
            RETURN ( FALSE ) ; 
        if ( !DX_FILE_BLOCKS_CLOSED(file) && !dxf_close_blocks ( file ) )
            RETURN ( FALSE ) ; 
        if ( !DX_FILE_ENTITIES_OPEN(file) && !dxf_open_entities ( file ) )
            RETURN ( FALSE ) ; 
        if ( !DX_FILE_ENTITIES_CLOSED(file) && !dxf_close_entities ( file ) )
            RETURN ( FALSE ) ; 
        if ( !dx0_put_string_group ( file, 0, "EOF" ) )
            RETURN ( FALSE ) ;
    }
    status = fclose ( DX_FILE_HANDLE(file) ) ;
    dml_destroy_list ( DX_FILE_BLOCKPOS_LIST(file), 
    			       ( PF_ACTION ) dx0_free_blockpos ) ;
#ifdef __ACAD11__
    dml_destroy_list ( DX_FILE_APPDEF_LIST(file), 
    				   ( PF_ACTION ) dx0_free_appdef ) ;
#endif
    DX_FILE_BLOCKPOS_LIST(file) = NULL ;
    dml_destroy_list ( DX_FILE_UNREC_LIST(file), 
    				   ( PF_ACTION ) dx0_free_unrec ) ;
    DX_FREE_FILE ( file ) ;
    RETURN ( status==0 ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void dx0_free_unrec ( unrec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
fpos_t *unrec ;
{
    dmm_free ( unrec ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT dxf_get_precision ( file ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    RETURN ( DX_FILE_PRECISION(file) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_set_precision ( file, precision ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT precision ;
{
    DX_FILE_PRECISION(file) = precision ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT dxf_get_splinesegs ( file ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    RETURN ( DX_FILE_SPLINESEGS(file) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_set_splinesegs ( file, splinesegs ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT splinesegs ;
{
    DX_FILE_SPLINESEGS(file) = splinesegs ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC long dxf_ftell ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    RETURN ( dxr_ftell ( DX_FILE_HANDLE(file) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC long dxf_filelength ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{ 
    if ( file == NULL ) 
        RETURN ( 0 ) ;
    if ( DX_FILE_LENGTH(file) == 0 )
        DX_FILE_LENGTH(file) = dxr_filelength ( DX_FILE_HANDLE(file) ) ;
    RETURN ( DX_FILE_LENGTH(file) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL dxf_current_pos ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    long pos, length ;

    pos = dxf_ftell ( file ) ;
    length = dxf_filelength ( file ) ;
    RETURN ( (REAL)pos / (REAL)length ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC char *dxf_get_filepos ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    fpos_t pos ;
    RETURN ( dx0_getpos ( file, &pos ) ? (char*)pos : NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_set_filepos ( file, def_ptr, access_mode )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
ANY def_ptr ;
DXF_FILEPOS_MODE access_mode ;
{ 
    fpos_t pos ;

    switch ( access_mode ) {

        case DXF_HEADER :
            RETURN ( dx0_setpos_header ( file ) ) ;
        case DXF_FIRST_ENTITY :
            RETURN ( def_ptr == NULL ? dx0_setpos_entities ( file ) :
            dx0_setpos_block ( file, def_ptr ) ) ;
        case DXF_FIRST_BLOCK :
            RETURN ( dx0_setpos_blocks ( file ) ) ;
        case DXF_FIRST_TABLE_ENTRY :
            RETURN ( dx0_setpos_tables ( file ) && 
                dx0_find ( file, 2, def_ptr, NULL ) && 
                dx0_find ( file, 70, NULL, NULL ) ) ;
        case DXF_RESTORE_POSITION :
            pos = (fpos_t)def_ptr ;
            RETURN ( dx0_setpos ( file, &pos ) ) ;
        default :
            RETURN ( FALSE ) ;
    }
}

#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT dxf_get_file_release ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    RETURN ( dx0_file_release ( file ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_set_file_release ( file, release )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT release ;
{
    if ( release == 10 || release == DXF_ACAD_RELEASE ) 
        DX_FILE_RELEASE(file) = release ;
}
#endif  /* ACAD11 */

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dxf_unrec_list ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    RETURN ( DX_FILE_UNREC_LIST(file) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT dxf_read_unrec ( file, pos, r )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
fpos_t pos ;
ANY r ;
{
    fpos_t pos0 ;
    int code ;

    if ( !dx0_getpos ( file, &pos0 ) )
        RETURN ( DXF_BAD_CODE ) ;
    if ( !dx0_setpos ( file, &pos ) ) {
        (void)dx0_setpos ( file, &pos0 ) ;
        RETURN ( DXF_BAD_CODE ) ;
    }
    code = dx0_get_code ( file ) ; 
    if ( code < 0 || !dx0_get_any ( file, code, r ) ) {
        (void)dx0_setpos ( file, &pos0 ) ;
        RETURN ( DXF_BAD_CODE ) ;
    }
    if ( !dx0_setpos ( file, &pos0 ) )
        RETURN ( DXF_BAD_CODE ) ;
    RETURN ( code ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_FILE_STATUS dxf_file_status ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{ 
    RETURN ( DX_FILE_STATUS(file) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_vtcs_disable ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{ 
    DX_FILE_SET_VTCS_DISABLE(file) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_vtcs_enable ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{ 
    DX_FILE_SET_VTCS_DISABLE(file) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_attrs_disable ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{ 
    DX_FILE_SET_ATTRS_DISABLE(file) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_attrs_enable ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{ 
    DX_FILE_SET_ATTRS_DISABLE(file) ;
}

