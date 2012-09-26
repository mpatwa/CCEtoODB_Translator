/* -2 -3 -x */
/******************************* DXH0.C *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                      !!!!!!!!*/ 
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <string.h> 
#include <dx0hdr.h>

GLOBAL int DXF_TEMPLATE_1[]     = { 1, DXF_END_CODE } ;
GLOBAL int DXF_TEMPLATE_2[]     = { 2, DXF_END_CODE } ;
GLOBAL int DXF_TEMPLATE_5[]     = { DXF_HNDL_ENABLED, 5, DXF_END_CODE } ;
GLOBAL int DXF_TEMPLATE_6[]     = { 6, DXF_END_CODE } ;
GLOBAL int DXF_TEMPLATE_7[]     = { 7, DXF_END_CODE } ;
GLOBAL int DXF_TEMPLATE_8[]     = { 8, DXF_END_CODE } ;
GLOBAL int DXF_TEMPLATE_40[]    = { 40, DXF_END_CODE } ;
GLOBAL int DXF_TEMPLATE_50[]    = { 50, DXF_END_CODE } ;
GLOBAL int DXF_TEMPLATE_62[]    = { 62, DXF_END_CODE } ;
GLOBAL int DXF_TEMPLATE_70[]    = { 70, DXF_END_CODE } ;
GLOBAL int DXF_TEMPLATE_R2[]    = { 10, 20, DXF_END_CODE } ;
GLOBAL int DXF_TEMPLATE_R3[]    = { 10, 20, 30, DXF_END_CODE } ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE int *dx0_hdr_var_template ( var_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING var_name ;
{ 
    if ( strncmp ( var_name, "$A", 2 ) == 0 )
        RETURN ( dx0_hdr_var_template_a ( var_name ) ) ;
    else if ( strcmp ( var_name, "$BLIPMODE" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strncmp ( var_name, "$C", 2 ) == 0 )
        RETURN ( dx0_hdr_var_template_c ( var_name ) ) ;
    else if ( strncmp ( var_name, "$D", 2 ) == 0 )
        RETURN ( dx0_hdr_var_template_d ( var_name ) ) ;
    else if ( strcmp ( var_name, "$ELEVATION" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strncmp ( var_name, "$EXT", 4 ) == 0 )
        RETURN ( DXF_TEMPLATE_R3 ) ;
    else if ( strcmp ( var_name, "$FASTZOOM" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$FILLETRAD" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strcmp ( var_name, "$FILLMODE" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$FLATLAND" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$GRIDMODE" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$GRIDUNIT" ) == 0 )
        RETURN ( DXF_TEMPLATE_R2 ) ;
    else if ( strcmp ( var_name, "$HANDLING" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$HANDSEED" ) == 0 )
        RETURN ( DXF_TEMPLATE_5 ) ;
    else if ( strcmp ( var_name, "$INSBASE" ) == 0 )
        RETURN ( DXF_TEMPLATE_R3 ) ;
    else if ( strcmp ( var_name, "$LIMCHECK" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strncmp ( var_name, "$LIMM", 5 ) == 0 )
        RETURN ( DXF_TEMPLATE_R2 ) ;
    else if ( strcmp ( var_name, "$LTSCALE" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strncmp ( var_name, "$LU", 3 ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
#ifdef __ACAD11__
    else if ( strcmp ( var_name, "$MAXACTVP" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
#endif
    else if ( strcmp ( var_name, "$MENU" ) == 0 )
        RETURN ( DXF_TEMPLATE_1 ) ;
    else if ( strcmp ( var_name, "$MIRRTEXT" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strncmp ( var_name, "$O", 2 ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strncmp ( var_name, "$P", 2 ) == 0 )
        RETURN ( dx0_hdr_var_template_p ( var_name ) ) ;
    else if ( strcmp ( var_name, "$QTEXTMODE" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$REGENMODE" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strncmp ( var_name, "$S", 2 ) == 0 )
        RETURN ( dx0_hdr_var_template_s ( var_name ) ) ;
    else if ( strncmp ( var_name, "$TD", 3 ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strcmp ( var_name, "$TEXTSIZE" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strcmp ( var_name, "$TEXTSTYLE" ) == 0 )
        RETURN ( DXF_TEMPLATE_7 ) ;
    else if ( strcmp ( var_name, "$THICKNESS" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strcmp ( var_name, "$TRACEWID" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strcmp ( var_name, "$TILEMODE" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strncmp ( var_name, "$U", 2 ) == 0 )
        RETURN ( dx0_hdr_var_template_u ( var_name ) ) ;
    else if ( strcmp ( var_name, "$VIEWCTR" ) == 0 )
        RETURN ( DXF_TEMPLATE_R2 ) ;
    else if ( strcmp ( var_name, "$VIEWDIR" ) == 0 )
        RETURN ( DXF_TEMPLATE_R3 ) ;
    else if ( strcmp ( var_name, "$VIEWSIZE" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strcmp ( var_name, "$VISRETAIN" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$WORLDVIEW" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else 
        RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT dxf_hdr_var_sizeof ( var_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING var_name ;
{ 
    RETURN ( dx0_sizeof_template ( dx0_hdr_var_template ( var_name ) ) ) ;
}

