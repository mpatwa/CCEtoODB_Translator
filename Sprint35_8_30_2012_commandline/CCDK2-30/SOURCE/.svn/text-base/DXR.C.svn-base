/* -2 -3 -x */
/******************************* DXR.C **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef      __MCRSFTC__
#include <io.h>
#endif

#ifdef __BRLNDC__
#include <io.h>
#endif

#include <stdio.h>
#include <dxrdefs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE long dxr_ftell ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
{
    RETURN ( ftell ( file ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE long dxr_filelength ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
{ 
#ifdef      __MCRSFTC__
    RETURN ( _filelength ( _fileno ( file ) ) ) ;
#endif

#ifdef      __TURBOC__
    RETURN ( filelength ( fileno ( file ) ) ) ;
#endif

#ifdef      __HIGHC__
    RETURN ( filelength ( fileno ( file ) ) ) ;
#endif

#ifdef      __WATCOM__
    RETURN ( filelength ( fileno ( file ) ) ) ;
#define     __PROTOTYPES__
#endif

#ifdef      __NDP__
    RETURN ( 1 ) ;
#endif

#ifdef      __MPW__
    RETURN ( filelength ( fileno ( file ) ) ) ;
#endif

#ifdef      __UNIX__
    RETURN ( 1 ) ;
#endif
}

