/* -2 -3 -x */
/******************************* DX0T.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>

#include <dxents.h>

#include <dx0defs.h>
#include <dmldefs.h>
#include <dxcode.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_clear_any ( code, record ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
int code ;
STRING record ;
{ 
    long *name ;
    if ( DX_CODE_STRING(code) ) 
        dx0_strcpy ( record, NULL ) ;
    else if ( DX_CODE_REAL(code) ) 
        *((REAL*)record) = 0.0 ;
    else if ( DX_CODE_COLOR(code) ) 
        *((INT*)record) = /* DX_COLOR_BYLAYER */ DXF_ANY_COLOR ;
    else if ( DX_CODE_INT(code) ) 
        *((INT*)record) = 0 ;
    else if ( code == DXF_ENAME_CODE ) {
        name = (long*)record ;
        name[0] = name[1] = -1 ;
    }
#ifdef __ACAD11__
    else if ( code == DXF_ANY_CODE || code == DXF_LIST_CODE ) 
        *((STRING*)record) = NULL ;
#else
    else if ( code == DXF_ANY_CODE ) 
        *((STRING*)record) = NULL ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_set1_any ( code, record ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
int code ;
ANY record ;
{ 
    if ( DX_CODE_REAL(code) ) 
        *((REAL*)record) = 1.0 ;
    else if ( DX_CODE_INT(code) ) 
        *((INT*)record) = 1 ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_setneg1_any ( code, record ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
int code ;
ANY record ;
{ 
    if ( DX_CODE_REAL(code) ) 
        *((REAL*)record) = -1.0 ;
    else if ( DX_CODE_INT(code) ) 
        *((INT*)record) = -1 ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE int dx0_sizeof_template ( template ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
int* template ;
{ 
    int s=0, i, mode=DXF_NODEF_CODE ;

    if ( template == NULL ) 
        RETURN ( -1 ) ;
#ifdef __ACAD11__
    if ( template[0] == DXF_MULT_CODE ) 
        template += ( template[1] + 2 ) ;
#endif
    for ( i=0 ; template[i] != DXF_END_CODE ; i++ ) {
#ifdef __ACAD11__
        if ( mode != DXF_LIST_CODE )
            s += dx0_sizeof ( template[i] ) ; 
        if ( template[i] == DXF_LIST_CODE )
            mode = DXF_LIST_CODE ;
        else if ( template[i] < 0 && template[i] != DX_INT_FILLER_CODE )
            mode = DXF_NODEF_CODE ;
#else
        s += dx0_sizeof ( template[i] ) ;
#endif
    }
    RETURN ( s ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_init_template ( record, template ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY record ;
int* template ;
{ 
    int *t, size, total_size = 0;
#ifndef __BRLNDC__
    int offset ;
#endif
    BOOLEAN zero_mode=TRUE, neg1_mode=FALSE, array_mode=FALSE, 
        list_mode=FALSE ;
    STRING data ;
#ifdef __ACAD11__
#ifdef DEBUG
    DX_VIEWPORT vp = (DX_VIEWPORT)record ;
#endif

#ifndef __BRLNDC__
    DX_POLYLINE pline = (DX_POLYLINE)record ;
#endif

    if ( template[0] == DXF_MULT_CODE ) 
        template += ( template[1] + 2 ) ;
#endif
    for ( t=template, data=(STRING)record ; *t != DXF_END_CODE ; t++ ) {
        if ( *t >= 0 ) { 
            if ( array_mode ) {
                dx0_clear_any ( DXF_ANY_CODE, data ) ;
                array_mode = FALSE ;
                size = dx0_sizeof ( DXF_ANY_CODE ) ;
                data += size ;
                total_size += size ;
#ifndef __BRLNDC__
		offset = data - (char*)record ;
#endif
            }
            else if ( !list_mode ) {
                if ( zero_mode ) 
                    dx0_clear_any ( *t, data ) ;
                else if ( neg1_mode ) 
                    dx0_setneg1_any ( *t, data ) ;
                else
                    dx0_set1_any ( *t, data ) ;
                size = dx0_sizeof ( *t ) ;
                data += size ;
                total_size += size ;
#ifndef __BRLNDC__
		offset = data - (char*)record ;
#endif
            }
        }
        else if ( *t == DXF_ARRAY_COUNT_CODE || *t == DXF_DEF0_CODE || 
            *t == DXF_NODEF_CODE ) {
            zero_mode = TRUE ;
            list_mode = FALSE ;
        }
        else if ( *t == DXF_ARRAY_CODE ) {
            zero_mode = TRUE ;
            array_mode = TRUE ;
            list_mode = FALSE ;
        }
        else if ( *t == DXF_DEF1_CODE ) {
            zero_mode = FALSE ;
            list_mode = FALSE ;
        }
        else if ( *t == DXF_DEFNEG1_CODE ) {
            zero_mode = FALSE ;
            neg1_mode = TRUE ;
            list_mode = FALSE ;
        }
        else if ( *t == DXF_ENAME_CODE || *t == DXF_ANY_CODE ) { 
            dx0_clear_any ( *t, data ) ;
            size = dx0_sizeof ( *t ) ;
            data += size ;
	    total_size += size ;
#ifndef __BRLNDC__
	    offset = data - (char*)record ;
#endif
            list_mode = FALSE ;
        }
#ifdef __ACAD11__
        else if ( *t == DXF_LIST_CODE ) { 
            dx0_clear_any ( *t, data ) ;
            size = dx0_sizeof ( *t ) ;
            data += size ;
	    total_size += size ;
#ifndef __BRLNDC__
	   		offset = data - (char*)record ;
#endif
            list_mode = TRUE ;
        }
	else if ( *t == DX_INT_FILLER_CODE ) {
            size = dx0_sizeof ( *t ) ;
            data += size ;
	    total_size += size ;
#ifndef __BRLNDC__
	    offset = data - (char*)record ;
#endif
	}
#endif
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_set_template ( record, template, code, index, value ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY record ;
int* template ;
int code, index ;
ANY value ;
{ 
    int *t, count ;
    STRING data ;

    count = 0 ;
#ifdef __ACAD11__
    if ( template[0] == DXF_MULT_CODE ) 
        template += ( template[1] + 2 ) ;
#endif
    for ( t=template, data=(STRING)record ; *t != DXF_END_CODE ; t++ ) {
        if ( *t == code ) { 
            if ( count == index ) {
                if ( DX_CODE_STRING(code) ) 
                    strncpy ( data, value, dx0_sizeof(code) ) ;
                else if ( DX_CODE_REAL(code) ) 
                    *((REAL*)data) = *((REAL*)value) ;
                else if ( DX_CODE_INT(code) ) 
                    *((INT*)data) = *((INT*)value) ;
                RETURN ( TRUE ) ;
            }
            count++ ;
        }
        data += dx0_sizeof ( *t ) ;
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE ANY dx0_get_template ( record, template, code, index ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY record ;
int* template ;
int code, index ;
{ 
    int *t, count ;
    STRING data ;

    count = 0 ;
#ifdef __ACAD11__
    if ( template[0] == DXF_MULT_CODE ) 
        template += ( template[1] + 2 ) ;
#endif
    for ( t=template, data=(STRING)record ; *t != DXF_END_CODE ; t++ ) {
        if ( *t == code ) {
            if ( count == index ) 
                RETURN ( (ANY)data ) ;
            count++ ;
        }
        data += dx0_sizeof ( *t ) ;
    }
    RETURN ( NULL ) ;
}


#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PRIVATE ANY dx0_create_record ( template ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
int* template ;
{ 
    int size ;
    ANY record ;

    size = dx0_sizeof_template ( template ) ;
    record = MALLOC ( size, char ) ;
    dx0_init_template ( record, template ) ;
    RETURN ( record ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_free_record ( record, template ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY record ;
int* template ;
{ 
    int *t ;
    BOOLEAN array_mode=FALSE, list_mode=FALSE ;
    STRING data ;
#ifdef DEBUG
    DX_VIEWPORT vp = (DX_VIEWPORT)record ;
#endif
    if ( template[0] == DXF_MULT_CODE ) 
        template += ( template[1] + 2 ) ;

    for ( t=template, data=(STRING)record ; *t != DXF_END_CODE ; t++ ) {
        if ( *t >= 0 ) { 
            if ( array_mode ) {
                dmm_free ( data ) ;
                array_mode = FALSE ;
                data += dx0_sizeof ( DXF_ANY_CODE ) ;
            }
            else if ( list_mode ) {
                dml_destroy_list ( (DML_LIST)data, dmm_free ) ;
                list_mode = FALSE ;
                data += dx0_sizeof ( DXF_ANY_CODE ) ;
            }
            else
                data += dx0_sizeof ( *t ) ;
        }
        else {
            if ( *t == DXF_ARRAY_CODE ) 
                array_mode = TRUE ;
            else if ( *t == DXF_LIST_CODE ) 
                list_mode = TRUE ;
            data += dx0_sizeof ( *t ) ;
        }
    }
    FREE ( record ) ;
}
#endif

