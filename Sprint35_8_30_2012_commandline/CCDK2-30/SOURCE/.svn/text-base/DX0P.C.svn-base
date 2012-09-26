/* -2 -3 -x*/
/******************************* DX0P.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h>
#include <dx0defs.h>
#include <dxfilm.h>
#include <dxcode.h>
#ifdef DEBUG
#include <dxdimd.h>
#endif
#include <dxltpd.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_code ( file, code ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
int code ;
{
    short c ;

    if ( DX_FILE_ASCII(file) ) {
        if ( code < 10 ) 
            RETURN ( fprintf ( DX_FILE_HANDLE(file), "  %d\n", code ) != 0 ) ;
        else if ( code < 100 ) 
            RETURN ( fprintf ( DX_FILE_HANDLE(file), " %d\n", code ) != 0 ) ;
        else 
            RETURN ( fprintf ( DX_FILE_HANDLE(file), "%d\n", code ) != 0 ) ;
    }
    else if ( DX_FILE_BINARY(file) ) {
        c = (short) code ;
        RETURN ( fwrite ( &c, 1, 1, DX_FILE_HANDLE(file) ) ) ;
    }
    else 
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_int ( file, i ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT i ;
{
    if ( DX_FILE_ASCII(file) ) {
        if ( i >= 0 && i <= 9 )
            RETURN ( fprintf ( DX_FILE_HANDLE(file), "     %d\n", i ) != 0 ) ;
        else if ( ( i >= 10 && i <= 99 ) || ( i >= -9 && i <= -1 ) )
            RETURN ( fprintf ( DX_FILE_HANDLE(file), "    %d\n", i ) != 0 ) ;
        else if ( ( i >= 100 && i <= 999 ) || ( i >= -99 && i <= -10 ) )
            RETURN ( fprintf ( DX_FILE_HANDLE(file), "   %d\n", i ) != 0 ) ;
        else if ( ( i >= 1000 && i <= 9999 ) || ( i >= -999 && i <= -100 ) )
            RETURN ( fprintf ( DX_FILE_HANDLE(file), "  %d\n", i ) != 0 ) ;
        else if ( ( i >= 10000 && (long)i <= 99999 ) || 
            ( i >= -9999 && i <= -1000 ) )
            RETURN ( fprintf ( DX_FILE_HANDLE(file), " %d\n", i ) != 0 ) ;
        else 
            RETURN ( fprintf ( DX_FILE_HANDLE(file), "%d\n", i ) != 0 ) ;
    }
/*
    if ( DX_FILE_ASCII(file) ) 
        RETURN ( fprintf ( DX_FILE_HANDLE(file), "%6.d\n", i ) != 0 ) ;
*/
    else if ( DX_FILE_BINARY(file) ) 
        RETURN ( fwrite ( &i, 2, 1, DX_FILE_HANDLE(file) ) ) ;
    else 
        RETURN ( FALSE ) ;
}

#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_long ( file, i ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
long i ;
{
    if ( DX_FILE_ASCII(file) ) 
        RETURN ( fprintf ( DX_FILE_HANDLE(file), "%6.d\n", i ) != 0 ) ;
    else if ( DX_FILE_BINARY(file) ) 
        RETURN ( fwrite ( &i, 2, 1, DX_FILE_HANDLE(file) ) ) ;
    else 
        RETURN ( FALSE ) ;
}
#endif


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_real ( file, p, r ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT p ;
REAL r ;
{
    REAL a ;
    INT s ;
    long c ;
    if ( DX_FILE_ASCII(file) ) {
        if ( r >= DXF_HUGE ) 
            RETURN ( fprintf ( DX_FILE_HANDLE(file), "%s\n", 
                DXF_HUGE_POS_WORD ) != 0 ) ;
        else if ( r <= -DXF_HUGE ) 
            RETURN ( fprintf ( DX_FILE_HANDLE(file), "%s\n", 
                DXF_HUGE_NEG_WORD ) != 0 ) ;
        if ( p <= 0 ) 
            p = DX_FILE_PRECISION(file) ;
        for ( s=1, a=10.0*r ; s<p ; s++, a*=10.0 ) {
            c = (long)a ; 
            if ( fabs ( (double)c-(double)a ) <= BBS_ZERO ) 
                RETURN ( fprintf ( DX_FILE_HANDLE(file), "%.*lf\n", 
                    s, r ) != 0 ) ;
        }
        RETURN ( fprintf ( DX_FILE_HANDLE(file), "%.*lf\n", p, r ) != 0 ) ;
    }
    else if ( DX_FILE_BINARY(file) ) {
        if ( r >= DXF_HUGE ) 
            RETURN ( dx0_put_string ( file, DXF_HUGE_POS_WORD ) ) ;
        else if ( r <= -DXF_HUGE ) 
            RETURN ( dx0_put_string ( file, DXF_HUGE_NEG_WORD ) ) ;
        else 
            RETURN ( fwrite ( &r, 8, 1, DX_FILE_HANDLE(file) ) )  ;
    }
    else 
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_string ( file, string ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING string ;
{
    INT i, n ;

    if ( DX_FILE_ASCII(file) ) 
        RETURN ( fprintf ( DX_FILE_HANDLE(file), "%s\n", string ) != 0 ) ;

    else if ( DX_FILE_BINARY(file) ) {
        for ( i=0 ; i<DX_BUFFER_SIZE ; i++ ) {
            n = fwrite ( string+i, 1, 1, DX_FILE_HANDLE(file) ) ;
            if ( n <= 0 ) 
                RETURN ( FALSE ) ;
            if ( string[i] == '\0' ) 
                RETURN ( TRUE ) ;
        }
        RETURN ( FALSE ) ;
    }
    else 
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_int_group ( file, code, i ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
int code ;
INT i ;
{
    RETURN ( dx0_put_code ( file, code ) && dx0_put_int ( file, i ) ) ;
}

#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_long_group ( file, code, i ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
int code ;
long i ;
{
    RETURN ( dx0_put_code ( file, code ) && dx0_put_long ( file, i ) ) ;
}
#endif


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_real_group ( file, code, r ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
int code ;
REAL r ;
{
    INT s ;
    s = ( code > 200 && code < 1000 ) ? 16 : 0 ;
    RETURN ( dx0_put_code ( file, code ) && dx0_put_real ( file, s, r ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_string_group ( file, code, string ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
int code ;
STRING string ;
{
    RETURN ( string==NULL || ( dx0_put_code ( file, code ) && 
        dx0_put_string ( file, string ) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_any_group ( file, code, data ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
int code ;
ANY data ;
{
    if ( DX_CODE_STRING(code) ) 
        RETURN ( dx0_put_string_group ( file, code, (STRING)data ) ) ;
    if ( DX_CODE_REAL(code) ) 
        RETURN ( dx0_put_real_group ( file, code, *((REAL*)data) ) ) ;
#ifdef __ACAD11__
    if ( code == 1071 ) 
        RETURN ( dx0_put_long_group ( file, code, *((long*)data) ) ) ;
#endif
    if ( DX_CODE_INT(code) ) 
        RETURN ( dx0_put_int_group ( file, code, *((INT*)data) ) ) ;
    else
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_int_mode ( file, code, mode, i ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
int code, mode ;
INT i ;
{
    RETURN ( ( mode == DXF_DEF0_CODE && i == 0 ) || 
             ( mode == DXF_DEF1_CODE && i == 1 ) || 
             ( mode == DXF_DEFNEG1_CODE && i == -1 ) || 
/* NEW_CODE 11-11-91 */
             ( DX_CODE_COLOR(code) && i == DXF_ANY_COLOR ) ||
/* NEW_CODE 11-11-91 */
             ( dx0_put_code ( file, code ) && dx0_put_int ( file, i ) ) ) ;
}

#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_long_mode ( file, code, mode, i ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
int code, mode ;
long i ;
{
    RETURN ( ( mode == DXF_DEF0_CODE && i == 0 ) || 
             ( mode == DXF_DEF1_CODE && i == 1 ) || 
             ( mode == DXF_DEFNEG1_CODE && i == -1 ) || 
             ( dx0_put_code ( file, code ) && dx0_put_long ( file, i ) ) ) ;
}
#endif


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_real_mode ( file, code, mode, r ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
int code, mode ;
REAL r ;
{
    INT s ;
    s = ( code > 200 && code < 1000 ) ? 16 : 0 ;

    RETURN ( ( mode == DXF_DEF0_CODE && IS_SMALL(r) ) || 
             ( mode == DXF_DEF1_CODE && IS_SMALL(r-1.0) ) || 
             ( mode == DXF_DEFNEG1_CODE && IS_SMALL(r+1.0) ) || 
             ( mode == DXF_ARRAY_CODE ) || ( dx0_put_code ( file, code ) && 
                dx0_put_real ( file, s, r ) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_string_mode ( file, code, mode, string ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
int code, mode ;
STRING string ;
{
    if ( ( mode != DXF_HNDL_ENABLED ) && ( code == 5 || code == 1005 ) && 
        !DX_FILE_ENT_HNDL_ENABLED(file) )
        RETURN ( TRUE ) ;
    if ( ( string == NULL || string[0] == '\0') && mode != DXF_NODEF_CODE ) {
        if ( mode == DXF_PUT0_CODE ) 
            RETURN ( dx0_put_string_group ( file, code, "0" ) ) ;
        else if ( mode == DXF_STANDARD_CODE ) 
            RETURN ( dx0_put_string_group ( file, code, "STANDARD" ) ) ;
        else
            RETURN ( TRUE ) ;
    }
    else
        RETURN ( dx0_put_string_group ( file, code, string ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_any_mode ( file, code, mode, data ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
int code, mode ;
ANY data ;
{
    if ( DX_CODE_STRING(code) ) 
        RETURN ( dx0_put_string_mode ( file, code, mode, (STRING)data ) ) ;
    if ( DX_CODE_REAL(code) ) 
        RETURN ( dx0_put_real_mode ( file, code, mode, *((REAL*)data) ) ) ;
#ifdef __ACAD11__
    if ( code == 1071 ) 
        RETURN ( dx0_put_long_mode ( file, code, mode, *((long*)data) ) ) ;
#endif
    if ( DX_CODE_INT(code) ) 
        RETURN ( dx0_put_int_mode ( file, code, mode, *((INT*)data) ) ) ;
    else
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_object ( file, object, template, put_6, put_62 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
ANY object ;         
int* template ;
BOOLEAN put_6, put_62 ;
{ 
    int *t, mode = DXF_NODEF_CODE ;
    INT i, count=0, size ;
    STRING data, array ;
    REAL *r ;
#ifdef __ACAD11__
    DML_ITEM item ;
#endif

#ifdef DEBUG
DX_DIM  dim = (DX_DIM)object ;
#endif

#ifndef __BRLNDC__
DX_LTYPE ltype = (DX_LTYPE)object ;
#endif

#ifdef __ACAD11__
    if ( template != NULL && *template == DXF_MULT_CODE ) 
        template += ( template[1] + 2 ) ;
#endif
    for ( t=template, data=(STRING)object ; *t != DXF_END_CODE ; t++ ) {
      {
#ifndef __BRLNDC__
	REAL *rr ;
	INT *ii ;
	rr = (REAL*)data ;
	ii = (INT*)data ;
#endif
      }

        if ( DX_CODE_MODE(*t) ) {
	  if ( *t == DX_INT_FILLER_CODE )
	    {
	      size = dx0_sizeof ( *t ) ;
	      data += size ;
	    }
            else if ( *t != DXF_PUT210_CODE ) 
                mode = *t ;
        }
#ifdef __ACAD11__
        else if ( mode == DXF_LIST_CODE ) {
            if ( *(DML_LIST*)data != NULL ) {
                DML_WALK_LIST ( *(DML_LIST*)data, item ) {
                    if ( !dx0_put_any_mode ( file, *t, 
                        DXF_NODEF_CODE, DML_RECORD(item) ) )
                        RETURN ( FALSE ) ;
                }
            }
	    size = dx0_sizeof ( mode ) ;
            data += size ;
        }
#endif
        else {
            if ( mode == DXF_ARRAY_COUNT_CODE ) 
                count = *(INT*)data ;
            else if ( mode == DXF_ARRAY_CODE ) {
                array = *(STRING*)data ;
                for ( i=0 ; i<count ; i++ ) {
                    if ( !dx0_put_any_group ( file, *t, array ) )
                        RETURN ( FALSE ) ;
		    size = dx0_sizeof ( *t ) ;
                    array += size ;
                }
            }
            if ( *t==210 ) {
                r = (REAL*)data ;
                if ( !IS_SMALL(r[0]) || !IS_SMALL(r[1]) || !IS_SMALL(r[2]) )
                    mode = DXF_NODEF_CODE ;
            }
            if ( ( ( *t != 6 ) || put_6 ) && ( ( *t != 62 ) || put_62 ) 
                && !dx0_put_any_mode ( file, *t, mode, data ) )
/*
            if ( ( *t == 6 ) && !put_6 ) ;
            else if ( ( *t == 62 ) && !put_62 ) ;
            else if ( !dx0_put_any_mode ( file, *t, mode, data ) )
*/
                RETURN ( FALSE ) ;
	    size = dx0_sizeof ( *t ) ;
            data += size;
        }
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_seqend ( file ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{ 
    DX_FILE_SET_SEQ_CLOSED(file) ;
    RETURN ( dx0_put_string_group ( file, 0, "SEQEND" ) &&
        dx0_put_string_mode ( file, 8, DXF_PUT0_CODE, 
            DX_FILE_ENT_LAYER(file) ) ) ;
}

