/* -2 -3 -x */
/******************************* DX0S.C *********************************/
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
BBS_PRIVATE BOOLEAN dx0_setpos_header ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ; 
{ 
    fpos_t pos0 ;

    if ( !dx0_getpos ( file, &pos0 ) )
        RETURN ( FALSE ) ;

    if ( DX_FILE_HEADER_POS_DEF(file) ) 
        RETURN ( dx0_setpos ( file, &(DX_FILE_HEADER_POS(file)) ) ) ;
    else {
        if ( !dx0_find ( file, 2, "HEADER", NULL ) ) {
            dx0_rewind ( file ) ;
            if ( !dx0_find ( file, 2, "HEADER", NULL ) ) {
                (void)dx0_setpos ( file, &pos0 ) ;
                RETURN ( FALSE ) ;
            }
        }
        if ( dx0_getpos ( file, &(DX_FILE_HEADER_POS(file)) ) ) {
            DX_FILE_SET_HEADER_POS(file) ;
            RETURN ( TRUE ) ;
        }
        else {
            (void)dx0_setpos ( file, &pos0 ) ;
            RETURN ( FALSE ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_setpos_tables ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ; 
{ 
    fpos_t pos0 ;

    if ( !dx0_getpos ( file, &pos0 ) )
        RETURN ( FALSE ) ;

    if ( DX_FILE_TABLES_POS_DEF(file) ) 
        RETURN ( dx0_setpos ( file, &(DX_FILE_TABLES_POS(file)) ) ) ;
    else {
        if ( !dx0_find_after ( file, 0, "SECTION", 2, "TABLES", NULL ) ) {
            dx0_rewind ( file ) ;
            if ( !dx0_find_after ( file, 0, "SECTION", 2, "TABLES", NULL ) ) {
                (void)dx0_setpos ( file, &pos0 ) ;
                RETURN ( FALSE ) ;
            }
        }
        if ( dx0_getpos ( file, &(DX_FILE_TABLES_POS(file)) ) ) {
            DX_FILE_SET_TABLES_POS(file) ;
            RETURN ( TRUE ) ;
        }
        else {
            (void)dx0_setpos ( file, &pos0 ) ;
            RETURN ( FALSE ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_setpos_blocks ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ; 
{ 
    fpos_t pos0 ;

    if ( !dx0_getpos ( file, &pos0 ) )
        RETURN ( FALSE ) ;

    if ( DX_FILE_BLOCKS_POS_DEF(file) ) 
        RETURN ( dx0_setpos ( file, &(DX_FILE_BLOCKS_POS(file)) ) ) ;
    else {
        if ( !dx0_find_after ( file, 0, "SECTION", 2, "BLOCKS", NULL ) ) {
            dx0_rewind ( file ) ;
            if ( !dx0_find_after ( file, 0, "SECTION", 2, "BLOCKS", NULL ) ) {
                (void)dx0_setpos ( file, &pos0 ) ;
                RETURN ( FALSE ) ;
            }
        }
        if ( dx0_getpos ( file, &(DX_FILE_BLOCKS_POS(file)) ) ) {
            DX_FILE_SET_BLOCKS_POS(file) ;
            RETURN ( TRUE ) ;
        }
        else {
            (void)dx0_setpos ( file, &pos0 ) ;
            RETURN ( FALSE ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_setpos_block ( file, blockname )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ; 
STRING blockname ;
{ 
    DML_ITEM item ;

    if ( blockname == NULL ) 
        RETURN ( dx0_setpos_entities ( file ) ) ;
    if ( DX_FILE_BLOCKPOS_LIST(file) == NULL && !dx0_block_list ( file ) )
        RETURN ( FALSE ) ;
    DML_WALK_LIST ( DX_FILE_BLOCKPOS_LIST(file), item ) {
        if ( strcmp ( DX_BLOCKPOS_NAME((DXF_BLOCKPOS)DML_RECORD(item)), 
            blockname ) == 0 )
            RETURN ( dx0_setpos ( file, 
                &DX_BLOCKPOS_FPOS((DXF_BLOCKPOS)DML_RECORD(item)) ) ) ;
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_setpos_entities ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ; 
{ 
    fpos_t pos0 ;
    BOOLEAN status ;

    status = dx0_getpos ( file, &pos0 ) ;
    if ( !status )
        RETURN ( FALSE ) ;
#ifdef OLD_CODE
    if ( !dx0_getpos ( file, &pos0 ) )
        RETURN ( FALSE ) ;
#endif
    if ( DX_FILE_ENTITIES_POS_DEF(file) ) 
        RETURN ( dx0_setpos ( file, &(DX_FILE_ENTITIES_POS(file)) ) ) ;
    else {
        if ( !dx0_find_after ( file, 0, "SECTION", 2, "ENTITIES", NULL ) ) {
            dx0_rewind ( file ) ;
            if ( !dx0_find_after ( file, 0, "SECTION", 2, 
                "ENTITIES", NULL ) ) {
                (void)dx0_setpos ( file, &pos0 ) ;
                RETURN ( FALSE ) ;
            }
        }
        if ( dx0_getpos ( file, &(DX_FILE_ENTITIES_POS(file)) ) ) {
            DX_FILE_SET_ENTITIES_POS(file) ;
            RETURN ( TRUE ) ;
        }
        else {
            (void)dx0_setpos ( file, &pos0 ) ;
            RETURN ( FALSE ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_block_list ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    DX_BUFFER string ;
    fpos_t pos ;
    DXF_BLOCKPOS blockpos ;

    if ( !dx0_getpos ( file, &pos ) )
        RETURN ( FALSE ) ;
    if ( !dx0_setpos_blocks ( file ) ) {
        (void)dx0_setpos ( file, &pos ) ;
        RETURN ( FALSE ) ;
    }
    DX_FILE_BLOCKPOS_LIST(file) = dml_create_list ();
    if ( DX_FILE_BLOCKPOS_LIST(file) == NULL ) {
        (void)dx0_setpos ( file, &pos ) ;
        RETURN ( FALSE ) ;
    }
    while ( TRUE ) {
        if ( !dx0_find ( file, 0, NULL, NULL ) || 
             !dx0_get_string ( file, string ) ) {
            (void)dx0_setpos ( file, &pos ) ;
            RETURN ( FALSE ) ;
        }
        if ( strcmp ( string, "BLOCK" ) == 0 ) {
            blockpos = DX_CREATE_BLOCKPOS ;
            if ( blockpos == NULL || 
                !dx0_getpos ( file, &DX_BLOCKPOS_FPOS(blockpos) ) ) {
                (void)dx0_setpos ( file, &pos ) ;
                RETURN ( FALSE ) ;
            }
            if ( !dx0_find ( file, 2, NULL, NULL ) ||
                !dx0_get_string ( file, DX_BLOCKPOS_NAME(blockpos) ) ) {
                (void)dx0_setpos ( file, &pos ) ;
                RETURN ( FALSE ) ;
            }
            if ( dml_append_data ( DX_FILE_BLOCKPOS_LIST(file), blockpos ) 
                == NULL ) {
                (void)dx0_setpos ( file, &pos ) ;
                RETURN ( FALSE ) ;
            }
        }
        else if ( strcmp ( string, "ENDSEC" ) == 0 || 
             strcmp ( string, "SECTION" ) == 0 )
            RETURN ( dx0_setpos ( file, &pos ) ) ;
    }
#ifndef __BRLNDC__
#ifndef __UNIX__
    RETURN ( FALSE ) ;
#endif
#endif
}

