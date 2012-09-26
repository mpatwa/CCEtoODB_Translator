/* -2 -3 -x */
/*********************************** DMB.C *********************************/
/******************************* DMB routines ******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmbdefs.h>
/*-------------------------------------------------------------------------*/
BBS_PRIVATE ANY dmb_create ( buf_size )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT buf_size ;
{
    ANY *buffer ;
    INT i ;
    if ( buf_size <= 0 )
        buffer = NULL ;
    else
        buffer = (ANY*)malloc ( buf_size * sizeof ( int* ) ) ;
    if ( buffer == NULL ) 
        RETURN ( NULL ) ;
    for ( i=0 ; i<buf_size ; i++ ) 
        buffer[i] = NULL ;
    RETURN ( (ANY)buffer ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dmb_destroy ( buffer, buf_size, clear )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY **buffer ;
INT buf_size ;
PF_ACTION clear ;   /* Clearing data records - freeing the internals */
{
    INT i ;
    if ( (*buffer) == NULL ) 
        RETURN ;
    for ( i = 0 ; i < buf_size ; i++ ) {
        if ( (*buffer)[i] != NULL ) {
            if ( clear != PF_NULL ) 
                (*clear) ( (*buffer)[i] ) ;
            FREE ( (*buffer)[i] ) ;
        }
    }
    free ( (*buffer) ) ;
    *buffer = NULL ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE ANY dmb_malloc ( buffer, buf_size, curr_no, size )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY **buffer ;
INT buf_size, *curr_no ;
size_t size ;
{
    INT i ;
    ANY curr_ptr ;

    if ( *curr_no == 0 ) 
        (*buffer) = (ANY*)dmb_create ( buf_size ) ;

    (*curr_no)++ ;
    for ( i=0 ; i<buf_size ; i++ ) {
        if ( (*buffer)[i] != NULL ) {
            curr_ptr = (*buffer)[i] ;
            (*buffer)[i] = NULL ;
            RETURN ( curr_ptr ) ;
        }
    }
#ifdef  MALLOC_CHECK
    RETURN ( bbs_malloc ( size ) ) ;
#else
    RETURN ( dmm_malloc ( size ) ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dmb_free ( object, buffer, buf_size, curr_no, clear )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY object ;
ANY **buffer ;
INT buf_size, *curr_no ;
PF_ACTION clear ;
{
    INT i ;

    if ( object == NULL ) 
        RETURN ;
    if ( clear != PF_NULL ) 
        (*clear) ( object ) ;
    (*curr_no)-- ;
    if ( *curr_no == 0 ) {
        dmb_destroy ( buffer, buf_size, clear ) ;
        FREE ( object ) ;
        RETURN ;
    }

    for ( i=0 ; i<buf_size ; i++ ) {
        if ( (*buffer)[i] == NULL ) {
            (*buffer)[i] = object ;
            RETURN ;
        }
    }
    FREE(object) ;
}

