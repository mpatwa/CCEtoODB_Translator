/* -2 -3 -x */
/******************************* DXFA.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#include <dmldefs.h>
#include <dx0defs.h>
#include <dxfilm.h>
#include <dxapdefm.h>
#ifdef __ACAD11__

STATIC DX_APPDEF dx0_create_appdef __(( STRING )) ;
STATIC BOOLEAN dx0_append_code __(( DX_APPDEF, int )) ;

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_ITEM dxf_define_appl ( file, appl_name )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING appl_name ;
{
    DX_APPDEF appdef ;

    appdef = dx0_create_appdef ( appl_name ) ;
    if ( appdef == NULL ) 
        RETURN ( NULL ) ;
    if ( DX_FILE_APPDEF_LIST(file) == NULL ) 
        DX_FILE_APPDEF_LIST(file) = dml_create_list () ;
    if ( DX_FILE_APPDEF_LIST(file) == NULL ) 
        RETURN ( NULL ) ;
    RETURN ( dml_append_data ( DX_FILE_APPDEF_LIST(file), appdef ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DX_APPDEF dx0_create_appdef ( appl_name )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING appl_name ;
{
    DX_APPDEF appdef ;
    int* template, templ_size ;

    templ_size = DX_INIT_TEMPL_SIZE + 2 ;
    template = MALLOC ( templ_size, int ) ;
    if ( template == NULL )
        RETURN ( NULL ) ;
    appdef = DX_CREATE_APPDEF ;
    if ( appdef == NULL ) {
        FREE ( template ) ;
        RETURN ( NULL ) ;
    }
    strcpy ( DX_APPDEF_NAME(appdef), appl_name ) ;
    DX_APPDEF_TEMPLATE(appdef) = template ;
    DX_APPDEF_TEMPL_SIZE(appdef) = templ_size ;
    DX_APPDEF_CURR_SIZE(appdef) = 3 ;
    DX_APPDEF_TEMPLATE(appdef)[0] = DXF_MULT_CODE ;
    DX_APPDEF_TEMPLATE(appdef)[1] = 0 ;
    DX_APPDEF_TEMPLATE(appdef)[2] = DXF_END_CODE ;
    RETURN ( appdef ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_free_appdef ( appdef )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DX_APPDEF appdef ;
{
    FREE ( DX_APPDEF_TEMPLATE(appdef) ) ;
    DX_FREE_APPDEF ( appdef ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DX_APPDEF dx0_find_appdef ( file, appl_name )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING appl_name ;
{
    DML_ITEM item ;
    DX_APPDEF appdef ;

    if ( file == NULL || DX_FILE_APPDEF_LIST(file) == NULL ) 
        RETURN ( NULL ) ;
    DML_WALK_LIST ( DX_FILE_APPDEF_LIST(file), item ) {
        appdef = DML_RECORD(item) ;
        if ( !strcmp ( DX_APPDEF_NAME(appdef), appl_name ) )
            RETURN ( appdef ) ;
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_append_code ( file, appl_name, code )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING appl_name ;
int code ;
{
    DX_APPDEF appdef ;

    appdef = dx0_find_appdef ( file, appl_name ) ;
    if ( appdef == NULL ) 
        RETURN ( FALSE ) ;
    RETURN ( dx0_append_code ( appdef, code ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN dx0_append_code ( appdef, code )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DX_APPDEF appdef ;
int code ;
{
    INT i, m, n, s ;
    int k ;

    s = DX_APPDEF_CURR_SIZE(appdef) ;
    for ( n = 0, i = 2 ; ( i < s - 1 ) && ( n <= 2 ) ; i++ ) {
        if ( DX_APPDEF_TEMPLATE(appdef)[i] == code )
            n++ ;
    }
    if ( n > 1 ) 
        n = 0 ;

    if ( DX_APPDEF_CURR_SIZE(appdef) + n >= DX_APPDEF_TEMPL_SIZE(appdef) ) {
        DX_APPDEF_TEMPL_SIZE(appdef) += DX_INIT_TEMPL_SIZE ;
        k = (int)DX_APPDEF_TEMPL_SIZE(appdef) ;
        REALLOC ( DX_APPDEF_TEMPLATE(appdef), k, int ) ;
    }

    s = DX_APPDEF_CURR_SIZE(appdef) ;
    if ( n == 1 ) {
        m = DX_APPDEF_TEMPLATE(appdef)[1] ;
        for ( i = s ; i > m + 2 ; i-- )
            DX_APPDEF_TEMPLATE(appdef)[i] = DX_APPDEF_TEMPLATE(appdef)[i-1] ;
        DX_APPDEF_TEMPLATE(appdef)[1] += 1 ;
        DX_APPDEF_TEMPLATE(appdef)[m+2] = code ;
        DX_APPDEF_CURR_SIZE(appdef) += 1 ;
        s = DX_APPDEF_CURR_SIZE(appdef) ;
    }

    DX_APPDEF_TEMPLATE(appdef)[s] = DX_APPDEF_TEMPLATE(appdef)[s-1] ;
    DX_APPDEF_TEMPLATE(appdef)[s-1] = code ;
    DX_APPDEF_CURR_SIZE(appdef) += 1 ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC int* dxf_appl_template ( file, appl_name )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING appl_name ;
{
    DX_APPDEF appdef ;

    appdef = dx0_find_appdef ( file, appl_name ) ;
    if ( appdef == NULL ) 
        RETURN ( NULL ) ;
    RETURN ( DX_APPDEF_TEMPLATE(appdef) ) ;
}
#endif

