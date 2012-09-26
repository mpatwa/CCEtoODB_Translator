/* -2 -3 -x */
/********************************** DML2.C *********************************/
/******************************* DML routines ******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <dmldefs.h>
STATIC DML_ITEM dml_find_min __(( DML_LIST, DML_ITEM, PF_COMP )) ;
STATIC void     dml_order_list_init __(( DML_LIST, DML_ITEM, PF_COMP, ANY )) ;

#define SORT(A,B)   (*sort_key) ( DML_RECORD(A), DML_RECORD(B) )

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dml_reverse_list ( list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
{
    DML_ITEM item, next ;
    
    if ( list == NULL || DML_LENGTH(list) <= 1 ) 
        RETURN ( list ) ;

    for ( item = DML_FIRST(list) ; item != NULL; item = next ) { 
        next = DML_NEXT(item) ;
        DML_NEXT(item) = DML_PREV(item) ;
        DML_PREV(item) = next ;
    }
    item = DML_LAST(list) ;
    DML_LAST(list) = DML_FIRST(list) ;
    DML_FIRST(list) = item ;
    RETURN ( list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dml_rearrange ( list, item0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
DML_ITEM item0 ;
{
    DML_ITEM item1 ;

    if ( item0 == NULL ) 
        RETURN ( NULL ) ;
    item1 = DML_PREV ( item0 ) ;
    if ( item1 == NULL ) 
        RETURN ( list ) ;
    
    DML_PREV(item0) = NULL ;
    DML_NEXT(item1) = NULL ;
    DML_NEXT(DML_LAST(list)) = DML_FIRST(list) ;
    DML_PREV(DML_FIRST(list)) = DML_LAST(list) ;
    DML_FIRST(list) = item0 ;
    DML_LAST(list) = item1 ;
    RETURN ( list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dml_sort_list ( list, sort_key ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
PF_SORT sort_key ;
{

    DML_ITEM item ;
    ANY temp ;
    INT i ;
    BOOLEAN sorted = FALSE ;

    for ( i = 1 ; i < DML_LENGTH(list) && !sorted ; i++ ) {
        sorted = TRUE ;
        for ( item = DML_FIRST(list) ; item != DML_LAST(list) ; 
            item=DML_NEXT(item) ) {
            if ( !(*sort_key) ( DML_RECORD(item), 
                DML_RECORD(DML_NEXT(item)) ) ) {
                sorted = FALSE ;
                temp = DML_RECORD(DML_NEXT(item)) ;
                DML_RECORD(DML_NEXT(item)) = DML_RECORD(item) ;
                DML_RECORD(item) = temp ;
            }
        } 
    }
    RETURN ( list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_ITEM dml_insert_by_key ( list, data, sort_key ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
ANY data ;
PF_SORT sort_key ;
{
    DML_ITEM item, item0, item1 ;
    ANY data0, data1 ;
    BOOLEAN s0, s1 ;

    DML_WALK_LIST ( list, item ) {
        item0 = DML_PREV(item) ;
        item1 = DML_NEXT(item) ;
        data0 = dml_record ( item0 ) ;
        data1 = dml_record ( item1 ) ;
        s0 = ( ( data0 == NULL ) || (*sort_key) ( data0, data ) ) ;
        s1 = ( ( data1 == NULL ) || (*sort_key) ( data, data1 ) ) ;
        if ( s0 && s1 ) 
            RETURN ( dml_insert_after ( list, item, data ) ) ;
    }
    RETURN ( dml_append_data ( list, data ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dml_sort_list_data ( list, sort_key, sort_data ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
PF_SORT_DATA sort_key ;
ANY sort_data ;
{
    DML_ITEM item ;
    ANY temp ;
    INT i ;
    BOOLEAN sorted = FALSE ;

    for ( i = 1 ; i < DML_LENGTH(list) && !sorted ; i++ ) {
        sorted = TRUE ;
        for ( item = DML_FIRST(list) ; item != DML_LAST(list) ; 
            item=DML_NEXT(item) ) {
            if ( !(*sort_key) ( DML_RECORD(item), 
                DML_RECORD(DML_NEXT(item)), sort_data ) ) {
                sorted = FALSE ;
                temp = DML_RECORD(DML_NEXT(item)) ;
                DML_RECORD(DML_NEXT(item)) = DML_RECORD(item) ;
                DML_RECORD(item) = temp ;
            }
        } 
    }
    RETURN ( list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dml_order_list ( list, item0, comp_key, init_data ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
DML_ITEM item0 ;
PF_COMP comp_key ;
ANY init_data ;
{
    DML_ITEM item1, item2 ;

    dml_order_list_init ( list, item0, comp_key, init_data ) ;
    if ( dml_length(list) <= 2 ) 
        RETURN ( list ) ;
/*
    for ( item1 = DML_FIRST(list) ; item1 != NULL ; item1 = DML_NEXT(item1) ) {
*/
    for ( item1 = item0 ; item1 != NULL ; item1 = DML_NEXT(item1) ) {
        item2 = dml_find_min ( list, item1, comp_key ) ;
        if ( item2 != NULL )
            dml_swap ( item2, DML_NEXT(item1) ) ;
    }
    RETURN ( list ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void dml_order_list_init ( list, item0, comp_key, init_data )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
DML_ITEM item0 ;
PF_COMP comp_key ;
ANY init_data ;
{
    DML_ITEM item, item1 ;
    REAL comp, comp1 ;

    if ( init_data == NULL ) 
        RETURN ;
    if ( item0 == NULL ) 
        item0 = DML_FIRST(list) ;

    item = NULL ;
    comp = 0.0 ;
    for ( item1 = item0 ; item1 != NULL ; item1 = DML_NEXT(item1) ) {
        comp1 = (*comp_key) ( init_data, DML_RECORD(item1) ) ;
        if ( ( item == NULL ) || ( comp1 < comp ) ) {
            item = item1 ;
            comp = comp1 ;
        }
    }
    if ( item != NULL ) 
        dml_swap ( item0, item ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DML_ITEM dml_find_min ( list, item0, comp_key ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
DML_ITEM item0 ;
PF_COMP comp_key ;
{
    DML_ITEM item, item1 ;
    REAL comp, comp1 ;

    if ( item0 == NULL ) 
        item0 = DML_FIRST(list) ;

    item = NULL ;
    comp = 0.0 ;
    for ( item1 = DML_NEXT(item0) ; item1 != NULL ; item1 = DML_NEXT(item1) ) {
        comp1 = (*comp_key) ( DML_RECORD(item0), DML_RECORD(item1) ) ;
        if ( ( item == NULL ) || ( comp1 < comp ) ) {
            item = item1 ;
            comp = comp1 ;
        }
    }
    RETURN ( item ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dml_create_copy ( inlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST inlist ;
{   
    DML_LIST outlist = dml_create_list() ;
    DML_ITEM item ;

    DML_WALK_LIST ( inlist, item ) 
        dml_append_data ( outlist, DML_RECORD(item) ) ;

    RETURN ( outlist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dml_recreate ( inlist, copy )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST inlist ;
PF_ANY1 copy ;
{   
    DML_LIST outlist ;
    DML_ITEM item ;
    if ( inlist == NULL ) 
        RETURN ( NULL ) ;

    outlist = dml_create_list() ;
    DML_WALK_LIST ( inlist, item ) 
        dml_append_data ( outlist, copy ( DML_RECORD(item) ) ) ;

    RETURN ( outlist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dml_merge_lists ( list1, list2, sort_key ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list1, list2 ;
PF_SORT sort_key ;
{
    DML_ITEM item1=DML_FIRST(list1), item2=DML_FIRST(list2), 
        prev2=NULL, next2=NULL ;

    if ( SORT ( item2, item1 ) )
        DML_FIRST(list1) = item2 ;
    
    while ( item1!=NULL && item2!=NULL )
    {
        if ( SORT ( item2, item1 ) )
        {   /* item1-1<prev2<item2<item1<next2 */
            prev2 = item2 ;
            next2 = DML_NEXT(item2) ;
            while ( next2!=NULL && SORT ( next2, item1 ) )
            {
                item2 = next2 ;
                next2 = DML_NEXT(next2) ;
            }
            DML_LINK ( DML_PREV(item1), prev2 ) ;
            DML_LINK ( item2, item1 ) ;
            item2 = next2 ;
        }
        else
            item1=DML_NEXT(item1) ;
    }
    
    if ( item2!=NULL )
    {
        DML_LINK ( DML_LAST(list1), item2 ) ;
        DML_LAST(list1) = DML_LAST(list2) ;            
    }
    DML_LENGTH(list1) += DML_LENGTH(list2) ;
    RETURN ( list1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dml_remove_chain ( list, item0, item1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
DML_ITEM item0, item1 ;
{
    DML_ITEM item, next_item ;
    if ( item0 == NULL ) 
        item0 = DML_FIRST(list) ;
    for ( item = item0 ; item != NULL && item != item1 ; item = next_item ) {
        next_item = DML_NEXT(item) ;
        dml_remove_item ( list, item ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dml_destroy_chain ( list, item0, item1, data_destructor ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
DML_ITEM item0, item1 ;
PF_ACTION data_destructor ;   /* Destructor of data records */
{
    DML_ITEM item, next_item ;
    if ( item0 == NULL ) 
        item0 = DML_FIRST(list) ;
    for ( item = item0 ; item != NULL && item != item1 ; item = next_item ) {
        next_item = DML_NEXT(item) ;
        (*data_destructor) ( DML_RECORD(item) ) ;
        dml_remove_item ( list, item ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dml_append_chain ( list0, item0, item1, list1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list0 ;
DML_ITEM item0, item1 ;
DML_LIST list1 ;
{
    DML_ITEM item, next_item ;

    if ( list1 == NULL ) 
        list1 = dml_create_list () ;
    if ( item0 == NULL ) 
        item0 = DML_FIRST(list0) ;
    for ( item = item0 ; item != NULL && item != item1 ; item = next_item ) {
        next_item = DML_NEXT(item) ;
        dml_append_data ( list1, DML_RECORD(item) ) ;
        dml_remove_item ( list0, item ) ;
    }
    RETURN ( list1 ) ;
}

