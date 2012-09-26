/* -2 -3 -x */
/******************************* DXBG.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#include <dmldefs.h>
#include <dx0defs.h>
#include <dxetmpls.h>

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_BLOCK dxf_get_block ( file, blockname )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING blockname ;
{ 
    int code, *block_template ;
    DXF_BLOCK block ;

    if ( blockname != NULL ) {
        if ( !dx0_setpos_block ( file, blockname ) )
            RETURN ( NULL ) ;
    }
    else {
        if ( !dx0_find ( file, 0, "BLOCK", NULL ) )
            RETURN ( NULL ) ;
    }
#ifdef __ACAD11__
    block = dx0_alloc_block ( dx0_file_release ( file ) );
#else
    block = dx0_alloc_block ();
#endif
    if ( block == NULL ) 
        RETURN ( NULL ) ;
#ifdef __ACAD11__
    block_template = ( dx0_file_release ( file ) == DXF_ACAD_RELEASE ) ? 
        DXF_BLOCK_TEMPLATE_11 : DXF_BLOCK_TEMPLATE_10 ;
#else
    block_template = DXF_BLOCK_TEMPLATE ;
#endif
    while ( TRUE  ) {
        code = dx0_get_object ( file, (char*)DXF_BLOCK_HEADER(block), 
            block_template, NULL, NULL ) ;
        if ( code == DXF_BAD_CODE ) {
            dxf_free_block ( block ) ;
            RETURN ( NULL ) ;
        }
        else if ( code == 0 ) 
            RETURN ( block ) ;
    }
#ifndef __BRLNDC__
#ifndef __UNIX__
    RETURN ( NULL ) ;
#endif
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dxf_get_blocks ( file, blocklist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DML_LIST blocklist ;
{ 
    DXF_BLOCK block ;

    if ( !dxf_set_filepos ( file, NULL, DXF_FIRST_BLOCK ) )
        RETURN ( NULL ) ;
    if ( blocklist == NULL ) 
        blocklist = dml_create_list ();
    if ( blocklist == NULL ) 
        RETURN ( NULL ) ;

    for ( block = dxf_get_block ( file, NULL ) ; block != NULL ;
        block = dxf_get_block ( file, NULL ) ) {
        if ( dml_append_data ( blocklist, block ) == NULL )
            RETURN ( NULL ) ;
    }
    RETURN ( blocklist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dxf_read_blocks ( filename, blocklist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
DML_LIST blocklist ;
{
    DXF_FILE file ;

    file = dxf_fopen ( filename, DXF_READ ) ;
    if ( file == NULL ) 
        RETURN ( NULL ) ;
    blocklist = dxf_get_blocks ( file, blocklist ) ;
    dxf_fclose ( file ) ;
    RETURN ( blocklist ) ;
}

