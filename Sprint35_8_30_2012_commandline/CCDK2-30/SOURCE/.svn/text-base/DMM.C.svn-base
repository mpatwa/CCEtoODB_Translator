/* -2 -3 -x */
/*********************************** DMM.C *********************************/
/******************************* DMM routines ******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsmem.h>
#include <dmmdefs.h>
#ifdef DEBUG
#include <conio.h>
#endif

#ifdef  MALLOC_CHECK
#include <conio.h>
#define MALLOC_BUFSIZE  16000
GLOBAL  INT         MALLOC_COUNT=0 ;
GLOBAL  long        MALLOC_SIZE=0 ;
GLOBAL  INT         FREE_COUNT=0 ;
GLOBAL  ANY         MALLOC_PTR[MALLOC_BUFSIZE] ;
#endif

#ifdef  CREATE_CHECK
#include <conio.h>
GLOBAL  INT         CREATE_COUNT=0 ;
GLOBAL  INT         KILL_COUNT=0 ;
GLOBAL  ANY         CREATE_PTR[1000] ;
#endif

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void *dmm_malloc ( size )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
size_t size ;
{
#ifdef DEBUG
    ANY ptr ;
    if ( size == 0 ) {
        printf ( "zero size\n" ) ;
        getch();
    }
#ifdef      MALLOC_CHECK
    ptr = bbs_malloc ( size ) ;
#else
    ptr = malloc ( size ) ;
#endif
    if ( ptr == NULL ) {
        printf ( "malloc failure\n" ) ;
        getch();
    }
    if ( size == 0 ) 
        size = 32 ;
    return ( ptr ) ;
#else
    if ( size == 0 ) 
        size = 32 ;
#ifdef      MALLOC_CHECK
    return ( bbs_malloc ( size ) ) ;
#else
    return ( malloc ( size ) ) ;
#endif
#endif
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void *dmm_realloc ( mem, size )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
void *mem ;
size_t size ;
{
    return ( realloc ( mem, size ) ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dmm_free ( mem )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
void *mem ;
{
#ifdef      MALLOC_CHECK
    if ( mem != NULL ) 
        bbs_free ( mem ) ;
#else
    if ( mem != NULL ) 
        free ( mem ) ;
#endif
}


#ifdef MALLOC_CHECK
/*------------------------------------------------------------------------*/
BBS_PRIVATE ANY bbs_malloc ( n ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT n ;
{
    ANY ptr ;
    ptr = malloc ( n ) ;
    if ( ptr == NULL ) {
        printf ( "malloc failure\n" ) ;
        getch();
    }
    MALLOC_PTR[MALLOC_COUNT] = ptr ;
    MALLOC_COUNT++ ;
    if ( MALLOC_COUNT >= MALLOC_BUFSIZE-1 ) {
        printf ( "MALLOC_COUNT = %d\n", MALLOC_COUNT ) ;
        getch();
    }
    MALLOC_SIZE +=n ;
    RETURN ( ptr ) ;
}


/*------------------------------------------------------------------------*/
BBS_PRIVATE void bbs_free ( a ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY a ;
{
    int i ;
    FREE_COUNT++ ;
    for ( i=0 ; i<MALLOC_COUNT ; i++ ) {
        if ( MALLOC_PTR[i] == a ) {
            MALLOC_PTR[i] = NULL ;
            free ( a ) ;
            RETURN ;
        }
    }
    printf ( "%p cannot be freed\n", a ) ;
    getch();
}
#endif


/*------------------------------------------------------------------------*/
BBS_PRIVATE void bbs_print_malloc () 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef MALLOC_CHECK
    INT i ;
    printf ( "MALLOC_COUNT=%d\tMALLOC_SIZE=%ld\tFREE_COUNT=%d\n", 
        MALLOC_COUNT, MALLOC_SIZE, FREE_COUNT ) ;
    for ( i=0 ; i<MALLOC_COUNT ; i++ ) {
        if ( MALLOC_PTR[i] != NULL ) {
            printf ( "%d\n", i ) ;
            getch();
        }
    }
#endif
}


#ifdef CREATE_CHECK
/*------------------------------------------------------------------------*/
BBS_PRIVATE ANY bbs_create ( n ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT n ;
{
    ANY ptr ;
    ptr = malloc ( n ) ;
    CREATE_PTR[CREATE_COUNT] = ptr ;
    CREATE_COUNT++ ;
    RETURN ( ptr ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void bbs_kill ( a ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY a ;
{
    int i ;

    if ( a == NULL )
        RETURN ;

    KILL_COUNT++ ;
    for ( i=0 ; i<CREATE_COUNT ; i++ ) {
        if ( CREATE_PTR[i] == a ) {
            CREATE_PTR[i] = NULL ;
            free ( a ) ;
            RETURN ;
        }
    }
    printf ( "%p cannot be killed\n", a ) ;
    getch();
}


/*------------------------------------------------------------------------*/
BBS_PRIVATE void bbs_print_create () 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    INT i ;
    printf ( "CREATE_COUNT=%d\tKILL_COUNT=%d\n", CREATE_COUNT, KILL_COUNT ) ;
    for ( i=0 ; i<CREATE_COUNT ; i++ ) {
        if ( CREATE_PTR[i] != NULL ) 
            printf ( "%d\n", i ) ;
    }
}
#endif


