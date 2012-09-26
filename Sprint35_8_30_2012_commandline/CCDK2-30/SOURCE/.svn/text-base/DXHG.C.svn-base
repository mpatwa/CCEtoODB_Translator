/* -2 -3 -x */
/******************************* DXHG.C *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                      !!!!!!!!*/ 
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <string.h> 
#include <dx0defs.h>
#include <dxfilm.h>
#include <dxpos.h>

/*-------------------------------------------------------------------------*/
BBS_PUBLIC ANY dxf_get_hdr_var ( file, var_name, data ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING var_name ;
ANY data ;
{ 
    int *template = dx0_hdr_var_template ( var_name ) ;

    if ( template == NULL ) 
        RETURN ( NULL ) ;
    dx0_rewind ( file ) ;
    if ( !dx0_find ( file, 9, var_name, NULL ) )
        RETURN ( NULL ) ;
    (void)dx0_get_object ( file, (char*)data, template, NULL, NULL ) ;
    RETURN ( data ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_read_hdr_var ( file, var_name, hdr_var ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING var_name ;
DXF_HDR_VAR hdr_var ;
{ 
    fpos_t curr_pos ;
    int *template ;
    int code ;

#ifdef OLD_CODE
    if ( !dx0_find ( file, 9, NULL, &curr_pos ) ||
         !dx0_get_string ( file, var_name ) )
        RETURN ( FALSE ) ;
#endif

    if ( !dx0_getpos ( file, &curr_pos ) )
        RETURN ( FALSE ) ;
    code = dx0_get_code ( file ) ;
    if ( code != 9 )
    {
        (void)dx0_setpos ( file, &curr_pos ) ;
        RETURN ( FALSE ) ;
    }
    if ( !dx0_get_string ( file, var_name ) )
        RETURN ( FALSE ) ;
    template = dx0_hdr_var_template ( var_name ) ;

    if ( template == NULL ) 
        RETURN ( FALSE ) ;
    (void)dx0_get_object ( file, (char*)hdr_var, template, NULL, NULL ) ;
    RETURN ( TRUE ) ;
}

