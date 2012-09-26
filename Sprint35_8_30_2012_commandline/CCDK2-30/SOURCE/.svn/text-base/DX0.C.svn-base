/* -2 -3 -x */
/******************************* DX0.C **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#include <dmldefs.h>
#include <dx0defs.h>
#include <dxfilm.h>
#include <dxcode.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE int dx0_sizeof ( code ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
int code ;
{ 
    if ( code == 5 ) 
        RETURN ( DX_HANDLE_SIZE ) ;
    else if ( code == DX_INT_FILLER_CODE )
#ifdef __UNIX__
        RETURN ( sizeof(DX_INT_FILLER) ) ;
#else
#ifdef __MSC32__
        RETURN ( sizeof(DX_INT_FILLER) ) ;
#else
        RETURN ( 0 ) ;
#endif
#endif
    else if ( code == 1 ) 
        RETURN ( DX_TXT_STRING_SIZE ) ;
#ifdef __ACAD11__
    else if ( code == 1005 ) 
        RETURN ( DX_HANDLE_SIZE ) ;
    else if ( code == 1004 ) 
        RETURN ( DX_TXT_STRING_SIZE ) ;
    else if ( code == 1000 ) 
        RETURN ( DX_1000_SIZE ) ;
    else if ( code == 1071 ) 
        RETURN ( sizeof(long) ) ;
#endif
    else if ( DX_CODE_STRING(code) ) 
        RETURN ( sizeof (DX_STRING) ) ;
    else if ( DX_CODE_REAL(code) ) 
        RETURN ( sizeof ( REAL) ) ;
    else if ( DX_CODE_INT(code) ) 
        RETURN ( sizeof ( INT) ) ;
    else if ( code == DXF_ANY_CODE ) 
        RETURN ( sizeof ( ANY ) ) ;
    else if ( code == DXF_ENAME_CODE ) 
        RETURN ( sizeof ( DXF_ENAME ) ) ;
#ifdef __ACAD11__
    else if ( code == DXF_LIST_CODE ) 
        RETURN ( sizeof ( DML_LIST ) ) ;
#endif
    else 
        RETURN ( 0 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL *dx0_copy_point ( p0, p1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL p0[3], p1[3] ;
{ 
    if ( p1 != NULL ) {
        if ( p0 == NULL ) {
            p1[0] = p1[1] = p1[2] = 0.0 ;
        }
        else {
            p1[0] = p0[0] ;
            p1[1] = p0[1] ;
            p1[2] = p0[2] ;
        }
    }
    RETURN ( p1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_rewind ( file ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{ 
    char sentinel[22];     /* Binary DXF file sentinel buffer */

    rewind ( DX_FILE_HANDLE(file) ) ;
    if ( DX_FILE_BINARY(file) ) 
        fread ( sentinel, 1, 22, DX_FILE_HANDLE(file) ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE STRING dx0_strcpy ( string1, string2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING string1, string2 ;
{ 
    RETURN ( strcpy ( string1, string2 == NULL ? "\0" : string2 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_unrec ( file, pos )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
fpos_t pos ;
{ 
    fpos_t *pos1 ;
    if ( file == NULL ) 
        RETURN ;
    if ( DX_FILE_UNREC_LIST(file) == NULL ) 
        DX_FILE_UNREC_LIST(file) = dml_create_list () ;
    pos1 = MALLOC ( 1, fpos_t ) ;
    *pos1 = pos ;
    dml_append_data ( DX_FILE_UNREC_LIST(file), pos1 ) ; 
}


#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT dx0_file_release ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{ 
    RETURN ( ( file == NULL ) ? 
        (INT)DXF_ACAD_RELEASE : DX_FILE_RELEASE(file) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_FILE_STATUS dx0_set_file_status ( file, s )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT s ;
{ 
    if ( file == NULL ) 
        RETURN ( 0 ) ;
    DX_FILE_STATUS(file) = DX_FILE_STATUS(file) | s ;
    RETURN ( DX_FILE_STATUS(file) ) ;
}
#endif


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_arb_axis ( z, x, y ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL z[3], x[3], y[3] ;
{ 
    REAL norm ;

    if ( IS_ZERO(z[0]) && IS_ZERO(z[1]) && IS_ZERO(z[2]) ) 
        z[2] = 1.0 ;        /* If z undefined, its WCS */
    if ( fabs(z[0]) < 0.015625 && fabs(z[1]) < 0.015625 ) {
        x[0] = z[2] ;
        x[1] = 0.0 ;
        x[2] = -z[0] ;
    }
    else {
        x[0] = -z[1] ;
        x[1] = z[0] ;
        x[2] = 0.0 ;
    }

    norm = sqrt ( x[0]*x[0] + x[1]*x[1] + x[2]*x[2] ) ;
    x[0] = x[0] / norm ;
    x[1] = x[1] / norm ;
    x[2] = x[2] / norm ;
    y[0] = z[1] * x[2] - z[2] * x[1] ;
    y[1] = z[2] * x[0] - z[0] * x[2] ;
    y[2] = z[0] * x[1] - z[1] * x[0] ;
}

