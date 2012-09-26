/* -2 -3 -x */
/*********************************** DML.C *********************************/
/******************************* DML routines ******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h>
GLOBAL      DML_LIST        DML_LIST_BUFFER[DML_LIST_BUF_SIZE] ;
GLOBAL      DML_ITEM        DML_ITEM_BUFFER[DML_ITEM_BUF_SIZE] ;
STATIC void free_list __(( DML_LIST )) ;
GLOBAL      INT             DML_LISTS_NUMBER=0 ;
GLOBAL      INT             DML_ITEMS_NUMBER=0 ;
/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_ITEM dml_first ( list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list;
{
    RETURN ( list==NULL ? NULL : list->first_item ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_ITEM dml_second ( list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list;
{
    RETURN ( dml_next ( dml_first ( list ) ) ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC ANY dml_first_record ( list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list;
{
    RETURN ( dml_record ( dml_first ( list ) ) ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_ITEM dml_last ( list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list;
{
    RETURN ( list==NULL ? NULL : list->last_item ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC ANY dml_last_record ( list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list;
{
    RETURN ( dml_record ( dml_last ( list ) ) ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT dml_length ( list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list;
{
    RETURN ( list==NULL ? 0 : list->length ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_ITEM dml_next ( item )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM item;
{
    RETURN ( item==NULL ? NULL : item->next_item ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_ITEM dml_prev ( item )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM item;
{
    RETURN ( item==NULL ? NULL : item->prev_item ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_ITEM dml_cycl_prev ( item, list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM item ;
DML_LIST list ;
{
    RETURN ( ( DML_PREV(item) == NULL ) ? DML_LAST(list) : DML_PREV(item) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_ITEM dml_cycl_next ( item, list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM item ;
DML_LIST list ;
{
    RETURN ( ( DML_NEXT(item) == NULL ) ? DML_FIRST(list) : DML_NEXT(item) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC ANY   dml_record ( item )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM item;
{
    RETURN ( item==NULL ? NULL : item->record ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dml_make_first ( list, item )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list;
DML_ITEM item ;
{
    DML_ITEM prev, next ;

    if ( list == NULL || item == NULL || item == DML_FIRST(list) )
        RETURN ;
    prev = DML_PREV(item) ;
    next = DML_NEXT(item) ;
    DML_LINK(prev,next) ;
    if ( next == NULL ) 
        DML_LAST(list) = prev ;
    DML_NEXT(item) = DML_FIRST(list) ;
    DML_PREV(item) = NULL ;
    DML_PREV(DML_FIRST(list)) = item ;
    DML_FIRST(list) = item ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dml_make_last ( list, item ) 
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                          !!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
DML_LIST list; 
DML_ITEM item ; 
{ 
    DML_ITEM prev, next ;

    if ( list == NULL || item == NULL || item == DML_LAST(list) )
        RETURN ;
    prev = DML_PREV(item) ;
    next = DML_NEXT(item) ;
    DML_LINK(prev,next) ;
    if ( prev == NULL ) 
        DML_FIRST(list) = next ;

    DML_NEXT(item) = NULL ;
    DML_PREV(item) = DML_LAST(list) ;
    DML_NEXT(DML_LAST(list)) = item ;
    DML_LAST(list) = item ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dml_create_list ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    DML_LIST list ;
    DML_LISTS_NUMBER++ ;
    list = DML_ALLOC_LIST ;
    if ( list != NULL ) 
        DML_EMPTY_LIST(list) ;
    RETURN(list) ;
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_ITEM dml_create_item ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    DML_ITEM item ;
    item = DML_ALLOC_ITEM ;
    DML_ITEMS_NUMBER++ ;
    RETURN(item) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dml_clear_list ( list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;     /* List to be cleared, can be NULL */
{
    DML_ITEM item = dml_first(list) , next ;

    while ( item != NULL ) {
        next = DML_NEXT(item) ;
        dml_free_item ( item ) ;
        item = next ;
    }
    DML_EMPTY_LIST(list) ;
    RETURN ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dml_destroy_list ( list, data_destructor )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;     /* List to be destroyed, can be NULL */
PF_ACTION data_destructor ;   /* Destructor of data records */
{
    if ( list == NULL ) 
        RETURN ;
    dml_apply ( list, data_destructor ) ;
    dml_free_list ( list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dml_free_list ( list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;     /* List to be freed, can be NULL */
{
    if ( list==NULL ) 
        RETURN ;
    dml_clear_list(list) ;
    free_list ( list ) ;
    DML_LISTS_NUMBER-- ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dml_flush ( )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    INT i ;

    for ( i=0 ; i<DML_LIST_BUF_SIZE ; i++ ) {
        if ( DML_LIST_BUFFER[i] != NULL ) {
            dml_clear_list ( DML_LIST_BUFFER[i] ) ;
            DML_FREE_LIST ( DML_LIST_BUFFER[i] ) ;
            DML_LIST_BUFFER[i] = NULL ;
        }
    }
    for ( i=0 ; i<DML_ITEM_BUF_SIZE ; i++ ) {
        DML_FREE_ITEM ( DML_ITEM_BUFFER[i] ) ;
        DML_ITEM_BUFFER[i] = NULL ;
    }
    DML_LISTS_NUMBER = 0 ;
}


/*-------------------------------------------------------------------------*/
STATIC void free_list ( list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;     /* List to be freed, can be NULL */
{
    DML_FREE_LIST(list) ;
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dml_free_item ( item )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM item ;     /* Item to be freed, can be NULL */
{
    if ( item == NULL )
        RETURN ;
    DML_ITEMS_NUMBER-- ;
    DML_FREE_ITEM(item) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_ITEM dml_insert_after ( list, prev_item, data )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;     /* The list to insert data onto */
DML_ITEM prev_item ;    /* The item to insert data after */
ANY data ;          /* The data record to be inserted */
{
    DML_ITEM new_item = dml_create_item() ;
    DML_ITEM next_item ;

    if ( new_item == NULL ) 
        RETURN ( NULL ) ;
    if ( prev_item == NULL ) {     /* Make it first item of the list */
        next_item = DML_FIRST(list) ;
        DML_FIRST(list) = new_item ;
    }
    else {
        next_item = DML_NEXT(prev_item) ;
        DML_NEXT(prev_item) = new_item ;
    }

    if ( DML_LAST(list) == prev_item ) 
        DML_LAST(list) = new_item ;
    DML_LINK(new_item,next_item) ;
    DML_PREV(new_item) = prev_item ;
    DML_RECORD(new_item) = data ;
    DML_LENGTH(list)++ ;
    RETURN ( new_item ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_ITEM dml_insert_prior ( list, next_item, data )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;         /* The list to insert data onto */
DML_ITEM next_item ;    /* The item to insert data before */
ANY data ;              /* The data record to be inserted */
{
    DML_ITEM new_item ;
    DML_ITEM prev_item ;

    if ( next_item == NULL )    /* Last item of the list */
        RETURN ( dml_append_data ( list, data ) ) ;

    new_item = dml_create_item() ;
    if ( new_item == NULL ) 
        RETURN ( NULL ) ;

    prev_item = DML_PREV(next_item) ;
    if ( DML_FIRST(list) == next_item )
        DML_FIRST(list) = new_item ;
    DML_LINK(prev_item,new_item) ;
    DML_LINK(new_item,next_item) ;
    DML_RECORD(new_item) = data ;
    DML_LENGTH(list)++ ;
    RETURN ( new_item ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_ITEM dml_insert ( list, data, pos_first )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;         /* The list to insert data onto */
ANY data ;              /* The data record to be inserted */
BOOLEAN pos_first ;     /* Position as first (or last) element on the list */
{
    RETURN ( pos_first ? dml_insert_after ( list, NULL, data ) :
        dml_insert_prior ( list, NULL, data ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_ITEM dml_append_data ( list, data ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;     /* The list to append data onto */
ANY data ;          /* The data record to be appended */
{
    DML_ITEM new_item = dml_create_item() ;
    DML_ITEM prev_item = DML_LAST(list) ;

    if ( new_item == NULL ) 
        RETURN ( NULL ) ;
    if ( DML_FIRST(list) == NULL ) {
        DML_FIRST(list) = new_item ;
        DML_PREV(new_item) = NULL ;
    }
    else
        DML_LINK(prev_item,new_item) ;
    
    DML_LAST(list) = new_item ;
    DML_NEXT(new_item) = NULL ;
    DML_RECORD(new_item) = data ;
    DML_LENGTH(list)++ ;
    RETURN ( new_item ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dml_remove_item ( list, item ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
DML_ITEM item ;
{
    DML_ITEM prev, next ;

    if ( list == NULL || item == NULL )
        RETURN ;
    if ( item == DML_FIRST(list) )
        dml_remove_first ( list ) ;
    else if ( item == DML_LAST(list) )
        dml_remove_last ( list ) ;
    else 
    {
        prev = DML_PREV(item) ;
        next = DML_NEXT(item) ;
        DML_LINK(prev,next) ;
        DML_LENGTH(list)-- ;
        dml_free_item ( item ) ;
    }
}       


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dml_remove_data ( list, data ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
ANY data ;
{
    DML_ITEM item ;

    item = dml_find_data ( list, data ) ;
    dml_remove_item ( list, item ) ;
    RETURN ( item != NULL ) ;
}       


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dml_kill_data ( list, data, data_destructor ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
ANY data ;
PF_ACTION data_destructor ;   /* Destructor of data records */
{
    DML_ITEM item ;

    item = dml_find_data ( list, data ) ;
    (*data_destructor) ( data ) ;
    dml_remove_item ( list, item ) ;
    RETURN ( item != NULL ) ;
}       


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dml_remove_last ( list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
{
    DML_ITEM last = DML_LAST(list) ;

    if ( list == NULL || DML_LENGTH(list) == 0 )
        RETURN ;
    if ( DML_FIRST(list) == last ) 
        DML_FIRST(list) = NULL ;
    DML_LAST(list) = DML_PREV ( last ) ; 
    if ( DML_LAST(list) != NULL ) 
        DML_NEXT ( DML_LAST(list) ) = NULL ;
    DML_LENGTH(list)-- ;
    dml_free_item ( last ) ;
}       


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dml_remove_first ( list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
{
    DML_ITEM first = DML_FIRST(list) ;

    if ( list == NULL || DML_LENGTH(list) == 0 )
        RETURN ;
    if ( DML_LAST(list) == first ) 
        DML_LAST(list) = NULL ;
    DML_FIRST(list) = DML_NEXT ( first ) ; 
    if ( DML_FIRST(list) != NULL ) 
        DML_PREV ( DML_FIRST(list) ) = NULL ;
    DML_LENGTH(list)-- ;
    dml_free_item ( first ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dml_apply ( list, action )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
PF_ACTION action ;
{   
    DML_ITEM item ;

    if ( list==NULL ) 
        RETURN ;
    DML_WALK_LIST ( list, item ) 
        (*action) ( DML_RECORD(item) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dml_apply_test ( list, test )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
PF_TEST test ;
{   
    DML_ITEM item ;

    if ( list==NULL ) 
        RETURN ( TRUE ) ;
    DML_WALK_LIST ( list, item ) {
        if ( ! (*test) ( DML_RECORD(item) ) )
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dml_apply_data ( list, action, data )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
PF_ACTION_DATA action ;
ANY data ;
{   
    DML_ITEM item ;

    if ( list==NULL ) 
        RETURN ;
    DML_WALK_LIST ( list, item ) 
        (*action) ( DML_RECORD(item), data ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_ITEM dml_find_data ( list, data )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
ANY data ;
{   
    DML_ITEM item ;

    DML_WALK_LIST ( list, item ) 
        if ( DML_RECORD(item) == data ) 
            RETURN ( item ) ;
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dml_find_data2 ( list, data0, data1, item0, item1 )
/* Finds *item0 and *item1 and returns ( *item0 preceeds *item1 ) */
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
ANY data0, data1 ;
DML_ITEM *item0, *item1 ;
{   
    DML_ITEM item ;
    INT order ;

    order = 0 ;
    DML_WALK_LIST ( list, item ) {
        if ( DML_RECORD(item) == data0 ) {
            *item0 = item ;
            if ( order == 0 ) 
                order = 1 ;
        }
        if ( DML_RECORD(item) == data1 ) {
            *item1 = item ;
            if ( order == 0 ) 
                order = -1 ;
        }
    }

    RETURN ( ( order == 1 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dml_filter ( list, filter, data, filterlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
PF_TEST_DATA filter ;
ANY data ;
DML_LIST filterlist ;
{   
    DML_ITEM item ;

    if ( filterlist == NULL ) 
        filterlist = dml_create_list () ;

    DML_WALK_LIST ( list, item ) {
        if ( (*filter) ( DML_RECORD(item), data ) ) 
            dml_append_data ( filterlist, DML_RECORD(item) ) ;
    }
    if ( DML_LENGTH(filterlist) == 0 ) {
        dml_free_list ( filterlist ) ;
        RETURN ( NULL ) ;
    }
    else
        RETURN ( filterlist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dml_copy_list ( list0, list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list0, list ;
{   
    DML_ITEM item ;

    if ( list0 == NULL ) 
        RETURN ( NULL ) ;
    if ( list == NULL ) 
        list = dml_create_list () ;

    DML_WALK_LIST ( list0, item )  
        dml_append_data ( list, DML_RECORD(item) ) ;
    RETURN ( list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dml_append_list ( list1, list2 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list1, list2 ;
{
    if ( list2 == NULL || DML_LENGTH(list2) == 0 )
        RETURN ( list1 ) ; 
    if ( DML_LENGTH(list1) == 0 ) 
        DML_FIRST(list1) = DML_FIRST(list2) ;
    else     
        DML_LINK ( DML_LAST(list1), DML_FIRST(list2) ) ;

    DML_LAST(list1) = DML_LAST(list2) ;
    DML_LENGTH(list1) += DML_LENGTH(list2) ;
    DML_EMPTY_LIST ( list2 ) ;
    RETURN ( list1 ) ; 
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dml_append_copy ( list1, list2 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list1, list2 ;
{
    DML_ITEM item ;

    if ( list2 == NULL )
        RETURN ( list1 ) ; 

    DML_WALK_LIST ( list2, item ) 
        dml_append_data ( list1, DML_RECORD(item) ) ;
    RETURN ( list1 ) ; 
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dml_swap ( item1, item2 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_ITEM item1, item2 ;
{
    ANY record ;
    record = DML_RECORD(item1) ;
    DML_RECORD(item1) = DML_RECORD(item2) ;
    DML_RECORD(item2) = record ;
}

