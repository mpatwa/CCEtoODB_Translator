/* -2 -3 */
/********************************** ALW.C **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifndef __NDP__
#ifndef __UNIX__
#include <stdlib.h>
#endif
#endif
#include <alwdefs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN alw_get_string ( file, string, size )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE *file ;
STRING string ;
INT size ; 
{
    INT i ;

    if ( fgets ( string, size, file ) == NULL )
        RETURN ( FALSE ) ;
    for ( i=0 ; i<size ; i++ ) {
        if ( string[i] == '\r' || string[i] == '\n' || string[i] == '\t' 
            || string[i] == '\0' || string[i] == ' ' ) {
            string[i] = '\0' ; 
            RETURN ( TRUE ) ;
        }
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN alw_get_int ( file, int_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE *file ;
INT *int_ptr ; 
{
    char  buffer[128] ;

    if ( fgets ( buffer, 127, file ) == NULL )
        RETURN ( FALSE ) ;
    if ( int_ptr != NULL ) 
        *int_ptr = atoi ( buffer ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN alw_get_real ( file, real_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
REAL *real_ptr ;
{
    char buffer[128] ;

    if ( fgets ( buffer, 127, file ) == NULL )
        RETURN ( FALSE ) ;
    if ( real_ptr != NULL ) 
        *real_ptr = (REAL) atof ( buffer ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN alw_get_array ( file, array, n )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
REAL *array ;
INT n ;
{
    INT i ;
    for ( i=0 ; i<n ; i++ ) {
        if ( !alw_get_real ( file, array + i ) )
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN alw_get_parm ( file, parm )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
PARM parm ;
{
    RETURN ( alw_get_real ( file, &PARM_T(parm) ) &&
        alw_get_int ( file, &PARM_J(parm) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN alw_put_string ( file, string, comment ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
STRING string, comment ;
{
    if ( file == NULL ) 
        file = stdout ;
    if ( comment == NULL ) 
        RETURN ( fprintf ( file, "%s\n", string ) != 0 ) ;
    else
        RETURN ( fprintf ( file, "%s %s\n", string, comment ) != 0 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN alw_put_int ( file, i, comment ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE *file ;
INT i ;
STRING comment ;
{
    if ( file == NULL ) 
        file = stdout ;
    if ( comment == NULL )
        RETURN ( fprintf ( file, "%d\n", i ) != 0 ) ;
    else
        RETURN ( fprintf ( file, "%d %s\n", i, comment ) != 0 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN alw_put_real ( file, r, comment ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE *file ;
REAL r ;
STRING comment ;
{
    if ( file == NULL ) 
        file = stdout ;
    if ( comment == NULL ) 
        RETURN ( fprintf ( file, "%.18lf\n", r ) != 0 ) ;
    else
        RETURN ( fprintf ( file, "%.18lf %s\n", r, comment ) != 0 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN alw_put_array ( file, array, n, comment )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
REAL *array ;
INT n ;
STRING comment ;
{
    INT i ;
    for ( i=0 ; i<n ; i++ ) {
        if ( !alw_put_real ( file, array[i], comment ) )
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN alw_put_parm ( file, parm, comment )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
PARM parm ;
STRING comment ;
{
    RETURN ( alw_put_real ( file, PARM_T(parm), comment ) &&
        alw_put_int ( file, PARM_J(parm), comment ) ) ;
}

