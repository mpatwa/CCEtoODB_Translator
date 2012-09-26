/* -2 -3 -x */
/******************************* DXH0.C *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                      !!!!!!!!*/ 
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <string.h> 
#include <dx0hdr.h>

EXTERN int DXF_TEMPLATE_1[]  ;
EXTERN int DXF_TEMPLATE_2[]  ;
EXTERN int DXF_TEMPLATE_5[]  ;
EXTERN int DXF_TEMPLATE_6[]  ;
EXTERN int DXF_TEMPLATE_7[]  ;
EXTERN int DXF_TEMPLATE_8[]  ;
EXTERN int DXF_TEMPLATE_40[] ;
EXTERN int DXF_TEMPLATE_50[] ;
EXTERN int DXF_TEMPLATE_62[] ;
EXTERN int DXF_TEMPLATE_70[] ;
EXTERN int DXF_TEMPLATE_R2[] ;
EXTERN int DXF_TEMPLATE_R3[] ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE int *dx0_hdr_var_template_a ( var_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING var_name ;
{ 
    if ( strcmp ( var_name, "$ACADVER" ) == 0 )
        RETURN ( DXF_TEMPLATE_1 ) ;
    else if ( strcmp ( var_name, "$ANGBASE" ) == 0 )
        RETURN ( DXF_TEMPLATE_50 ) ;
    else if ( strcmp ( var_name, "$ANGDIR" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strncmp ( var_name, "$ATT", 4 ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strncmp ( var_name, "$AU", 3 ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$AXISMODE" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$AXISUNIT" ) == 0 )
        RETURN ( DXF_TEMPLATE_R2 ) ;
    else
        RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE int *dx0_hdr_var_template_c ( var_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING var_name ;
{ 
    if ( strcmp ( var_name, "$CECOLOR" ) == 0 )
        RETURN ( DXF_TEMPLATE_62 ) ;
    else if ( strcmp ( var_name, "$CELTYPE" ) == 0 )
        RETURN ( DXF_TEMPLATE_6 ) ;
    else if ( strncmp ( var_name, "$CHAM", 5 ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strcmp ( var_name, "$CLAYER" ) == 0 )
        RETURN ( DXF_TEMPLATE_8 ) ;
    else if ( strcmp ( var_name, "$COORDS" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else
        RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE int *dx0_hdr_var_template_d ( var_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING var_name ;
{ 
    if ( strcmp ( var_name, "$DIMALT" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$DIMALTD" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$DIMALTF" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strcmp ( var_name, "$DIMAPOST" ) == 0 )
        RETURN ( DXF_TEMPLATE_1 ) ;
    else if ( strcmp ( var_name, "$DIMASO" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$DIMASZ" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strcmp ( var_name, "$DIMBLK" ) == 0 )
/*        RETURN ( DXF_TEMPLATE_2 ) ; bug in the Autocad spec */
        RETURN ( DXF_TEMPLATE_1 ) ;
    else if ( strncmp ( var_name, "$DIMBLK", 7 ) == 0 )
        RETURN ( DXF_TEMPLATE_1 ) ;
    else if ( strcmp ( var_name, "$DIMCEN" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strncmp ( var_name, "$DIMCL", 6 ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strncmp ( var_name, "$DIMDL", 6 ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strncmp ( var_name, "$DIMEX", 6 ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
#ifdef __ACAD11__
    else if ( strncmp ( var_name, "$DIMGAP", 6 ) == 0 )
         RETURN ( DXF_TEMPLATE_40 ) ;
#endif
    else if ( strcmp ( var_name, "$DIMLFAC" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strcmp ( var_name, "$DIMLIM" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$DIMPOST" ) == 0 )
        RETURN ( DXF_TEMPLATE_1 ) ;
    else if ( strcmp ( var_name, "$DIMRND" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strcmp ( var_name, "$DIMSAH" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$DIMSCALE" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strcmp ( var_name, "$DIMSTYLE" ) == 0 )
        RETURN ( DXF_TEMPLATE_2 ) ;
    else if ( strncmp ( var_name, "$DIMS", 5 ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$DIMTAD" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
#ifdef __ACAD11__
    else if ( strcmp ( var_name, "$DIMTFAC" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
#endif
    else if ( strcmp ( var_name, "$DIMTIH" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$DIMTIX" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$DIMTM" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strncmp ( var_name, "$DIMTO", 6 ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$DIMTP" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strcmp ( var_name, "$DIMTSZ" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strcmp ( var_name, "$DIMTVP" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strcmp ( var_name, "$DIMTXT" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strcmp ( var_name, "$DIMZIN" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$DRAGMODE" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else
        RETURN ( NULL ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE int *dx0_hdr_var_template_p ( var_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING var_name ;
{ 
    if ( strcmp ( var_name, "$PDMODE" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$PDSIZE" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
#ifdef __ACAD11__
    else if ( strcmp ( var_name, "$PELEVATION" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strcmp ( var_name, "$PEXTMAX" ) == 0 )
        RETURN ( DXF_TEMPLATE_R3 ) ;
    else if ( strcmp ( var_name, "$PEXTMIN" ) == 0 )
        RETURN ( DXF_TEMPLATE_R3 ) ;
    else if ( strcmp ( var_name, "$PINSBASE" ) == 0 )
        RETURN ( DXF_TEMPLATE_R3 ) ;
    else if ( strcmp ( var_name, "$PLIMCHECK" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$PLIMMAX" ) == 0 )
        RETURN ( DXF_TEMPLATE_R2 ) ;
    else if ( strcmp ( var_name, "$PLIMMIN" ) == 0 )
        RETURN ( DXF_TEMPLATE_R2 ) ;
#endif
    else if ( strcmp ( var_name, "$PLINEWID" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
#ifdef __ACAD11__
    else if ( strcmp ( var_name, "$PUCSNAME" ) == 0 )
        RETURN ( DXF_TEMPLATE_2 ) ;
    else if ( strcmp ( var_name, "$PUCSORG" ) == 0 )
        RETURN ( DXF_TEMPLATE_R3 ) ;
    else if ( strcmp ( var_name, "$PUCSXDIR" ) == 0 )
        RETURN ( DXF_TEMPLATE_R3 ) ;
    else if ( strcmp ( var_name, "$PUCSYDIR" ) == 0 )
        RETURN ( DXF_TEMPLATE_R3 ) ;
#endif
    else
        RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE int *dx0_hdr_var_template_s ( var_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING var_name ;
{ 
#ifdef __ACAD11__
    if ( strcmp ( var_name, "$SHADEDGE" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$SHADEDIF" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else 
#endif
    if ( strcmp ( var_name, "$SKETCHINC" ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strcmp ( var_name, "$SKPOLY" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strncmp ( var_name, "$SPL", 4 ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$SNAPANG" ) == 0 )
        RETURN ( DXF_TEMPLATE_50 ) ;
    else if ( strcmp ( var_name, "$SNAPBASE" ) == 0 )
        RETURN ( DXF_TEMPLATE_R2 ) ;
    else if ( strcmp ( var_name, "$SNAPISOPAIR" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$SNAPMODE" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$SNAPSTYLE" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strcmp ( var_name, "$SNAPUNIT" ) == 0 )
        RETURN ( DXF_TEMPLATE_R2 ) ;
    else if ( strncmp ( var_name, "$SURF", 5 ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else
        RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE int *dx0_hdr_var_template_u ( var_name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING var_name ;
{ 
    if ( strcmp ( var_name, "$UCSNAME" ) == 0 )
        RETURN ( DXF_TEMPLATE_2 ) ;
    else if ( strncmp ( var_name, "$UCS", 4 ) == 0 )
        RETURN ( DXF_TEMPLATE_R3 ) ;
#ifdef __ACAD11__
    else if ( strncmp ( var_name, "$UNITMODE", 4 ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
#endif
    else if ( strncmp ( var_name, "$USERI", 6 ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else if ( strncmp ( var_name, "$USERR", 6 ) == 0 )
        RETURN ( DXF_TEMPLATE_40 ) ;
    else if ( strcmp ( var_name, "$USRTIMER" ) == 0 )
        RETURN ( DXF_TEMPLATE_70 ) ;
    else
        RETURN ( NULL ) ;
}
