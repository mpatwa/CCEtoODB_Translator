/* -2 -3 -x */
/*********************************** DML1.C *********************************/
/******************************* DML routines ******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <dmldefs.h>

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dml_create_array_to_list ( x, n, size )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY x ;
INT n, size ;
{   
    DML_LIST list = dml_create_list() ;
    dml_copy_array_to_list ( x, n, size, list ) ;
    return ( list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dml_copy_array_to_list ( x, n, size, list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY x ;
INT n, size ;
DML_LIST list ;
{   
    STRING y ;
    INT i ;
    for ( i=0, y=(STRING)x ; i<n ; i++, y+=size )
        dml_append_data ( list, y ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC ANY dml_create_list_to_array ( list, size )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
INT size ;
{   
    ANY x ;
    x = (ANY)MALLOC ( DML_LENGTH(list)*size, char ) ;
    dml_copy_list_to_array ( list, x, size ) ;
    return ( x ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dml_copy_list_to_array ( list, x, size )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
ANY x ;
INT size ;
{   
    DML_ITEM item ;
    INT i ;
    STRING y, x_char ;

    x_char = (STRING)x ;
    DML_WALK_LIST ( list, item ) {
        y = (STRING)DML_RECORD ( item ) ;
        for ( i=0 ; i<size ; i++, y++, x_char++ )
            *x_char = *y ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_ITEM dml_nth_item ( list, n, n0, item0 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
INT n, n0 ;
DML_ITEM item0 ;
{   
    DML_ITEM item ;
    INT i, d[3], m, l ;

    l = dml_length ( list ) ;
    if ( l == 0 )
        RETURN ( NULL ) ;

    if ( n < 0 || n >= l )
        RETURN ( NULL ) ;

    d[0] = n ;
    d[1] = l - 1 - n ;
    i = ( d[0] < d[1] ) ? 0 : 1 ;

    if ( item0 != NULL )
    {
        d[2] = abs ( n - n0 ) ;
        if ( d[2] < d[i] ) 
            i = 2 ;
    }
    if ( i == 0 )
    {
        n0 = 0 ;
        item0 = DML_FIRST(list) ;
    }
    else if ( i == 1 )
    {
        n0 = l - 1 ;
        item0 = DML_LAST(list) ;
    }
    m = n - n0 ;

    if ( m > 0 )
    {
        for ( item = item0, i = 0 ; i < m ; item = DML_NEXT(item), i++ ) ;
        RETURN ( item ) ;
    }
    else 
    {
        for ( item = item0, i = 0 ; i < -m ; item = DML_PREV(item), i++ ) ;
        RETURN ( item ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_ITEM* dml_array_of_items ( list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST list ;
{   
    DML_ITEM item, *array ;
    INT i, l ;

    l = dml_length ( list ) ;
    if ( l == 0 ) 
        RETURN ( NULL ) ;
    array = MALLOC ( l, DML_ITEM ) ;
    if ( array == NULL )
        RETURN ( NULL ) ;

    i = 0 ;
    DML_WALK_LIST ( list, item )
    {
        array[i] = item ;
        i++ ;
    }
    RETURN ( array ) ;
}

