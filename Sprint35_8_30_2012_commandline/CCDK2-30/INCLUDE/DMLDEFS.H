/***************************** DMLDEFS.H ***********************************/
/***************** Include file for list management component **************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef DMLDEFS
#define DMLDEFS
#include <dmdefs.h>

/****** Declarations of routines *********/
extern  DML_ITEM    dml_first __(( DML_LIST DUMMY0 ));
extern  DML_ITEM    dml_second __(( DML_LIST DUMMY0 ));
extern  ANY         dml_first_record __(( DML_LIST DUMMY0 ));
extern  DML_ITEM    dml_last __(( DML_LIST DUMMY0 ));
extern  ANY         dml_last_record __(( DML_LIST DUMMY0 ));
extern  INT         dml_length __(( DML_LIST DUMMY0 ));
extern  DML_ITEM    dml_prev __(( DML_ITEM DUMMY0 ));
extern  DML_ITEM    dml_next __(( DML_ITEM DUMMY0 ));
extern  DML_ITEM    dml_cycl_prev __(( DML_ITEM DUMMY0 , DML_LIST DUMMY1 ));
extern  DML_ITEM    dml_cycl_next __(( DML_ITEM DUMMY0 , DML_LIST DUMMY1 ));
extern  ANY         dml_record __(( DML_ITEM DUMMY0 ));
extern  void        dml_make_first __(( DML_LIST DUMMY0 , DML_ITEM DUMMY1 )) ;
extern  void        dml_make_last __(( DML_LIST DUMMY0 , DML_ITEM DUMMY1 )) ;
extern  DML_LIST    dml_create_list __(( void ));
extern  DML_LIST    dml_create_copy __(( DML_LIST DUMMY0 )) ;
extern  DML_LIST    dml_recreate __(( DML_LIST DUMMY0 , PF_ANY1 DUMMY1 )) ;
extern  DML_LIST    dml_create_array_to_list __(( ANY DUMMY0 , INT DUMMY1 , 
            INT DUMMY2 ));
extern  void        dml_copy_array_to_list __(( ANY DUMMY0 , INT DUMMY1 , 
            INT DUMMY2 , DML_LIST DUMMY3 ));
extern  ANY         dml_create_list_to_array __(( DML_LIST DUMMY0 , 
            INT DUMMY1 ));
extern  void        dml_copy_list_to_array __(( DML_LIST DUMMY0 , ANY DUMMY1 , 
            INT DUMMY2 ));
extern  void        dml_free_list __(( DML_LIST DUMMY0 ));
extern  void        dml_flush __(( void )) ;
extern  DML_ITEM    dml_create_item __(( void ));
extern  void        dml_free_item __(( DML_ITEM DUMMY0 ));
extern  DML_ITEM    dml_insert_after __(( DML_LIST DUMMY0 , DML_ITEM DUMMY1 , 
            ANY DUMMY2 ));
extern  DML_ITEM    dml_insert_prior __(( DML_LIST DUMMY0 , DML_ITEM DUMMY1 , 
            ANY DUMMY2 ));
extern  DML_ITEM    dml_insert __(( DML_LIST DUMMY0 , ANY DUMMY1 , 
            BOOLEAN DUMMY2 ));
extern  DML_ITEM    dml_append_data __(( DML_LIST DUMMY0 , ANY DUMMY1 ));
extern  void        dml_remove_item __(( DML_LIST DUMMY0 , DML_ITEM DUMMY1 ));
extern  BOOLEAN     dml_remove_data __(( DML_LIST DUMMY0 , ANY DUMMY1 )) ;
extern  BOOLEAN     dml_kill_data __(( DML_LIST DUMMY0 , ANY DUMMY1 , 
            PF_ACTION DUMMY2 )) ;
extern  void        dml_remove_first __(( DML_LIST DUMMY0 ));
extern  void        dml_remove_last __(( DML_LIST DUMMY0 ));
extern  void        dml_remove_chain __(( DML_LIST DUMMY0 , DML_ITEM DUMMY1 , 
        DML_ITEM DUMMY2 ));
extern  DML_LIST    dml_append_chain __(( DML_LIST DUMMY0 , DML_ITEM DUMMY1 , 
        DML_ITEM DUMMY2 , DML_LIST DUMMY3 ));
extern  void        dml_destroy_chain __(( DML_LIST DUMMY0 , DML_ITEM DUMMY1 , 
        DML_ITEM DUMMY2, PF_ACTION DUMMY3  ));
extern  void        dml_clear_list __(( DML_LIST DUMMY0 ));
extern  DML_LIST    dml_append_list __(( DML_LIST DUMMY0 , DML_LIST DUMMY1 ));
extern  DML_LIST    dml_append_copy __(( DML_LIST DUMMY0 , DML_LIST DUMMY1 ));
extern  DML_LIST    dml_reverse_list __(( DML_LIST DUMMY0 ));
extern  DML_LIST    dml_rearrange __(( DML_LIST DUMMY0 , DML_ITEM DUMMY1 ));
extern  DML_LIST    dml_merge_lists __(( DML_LIST DUMMY0 , DML_LIST DUMMY1 , 
            PF_SORT DUMMY2 ));
extern  DML_LIST    dml_sort_list __(( DML_LIST DUMMY0 , PF_SORT DUMMY1 ));
extern  DML_LIST    dml_sort_list_data __(( DML_LIST DUMMY0 , 
            PF_SORT_DATA DUMMY1 , ANY DUMMY2 )) ;
extern  DML_ITEM dml_insert_by_key __(( DML_LIST DUMMY0 , 
            ANY DUMMY1 , PF_SORT DUMMY2 )) ;
extern  DML_ITEM    dml_find_data __(( DML_LIST DUMMY0 , ANY DUMMY1 )) ; 
extern  DML_LIST    dml_filter __(( DML_LIST DUMMY0 , PF_TEST_DATA DUMMY1 , 
            ANY DUMMY2 , DML_LIST DUMMY3 )) ; 
extern  void        dml_destroy_list __(( DML_LIST DUMMY0 , 
            PF_ACTION DUMMY1 )); 
extern  void        dml_apply __(( DML_LIST DUMMY0 , PF_ACTION DUMMY1 )) ; 
extern  BOOLEAN     dml_apply_test __(( DML_LIST DUMMY0 , PF_TEST DUMMY1 )) ; 
extern  void        dml_apply_data __(( DML_LIST DUMMY0 , 
            PF_ACTION_DATA DUMMY1 , ANY DUMMY2 )) ; 
extern  DML_ITEM    dml_find_test __(( DML_LIST DUMMY0 , PF_TEST DUMMY1 )) ;
extern  void dml_swap __(( DML_ITEM DUMMY0 , DML_ITEM DUMMY1 )) ;
extern  DML_LIST dml_order_list __(( DML_LIST DUMMY0 , DML_ITEM DUMMY1 , 
            PF_COMP DUMMY2 , ANY DUMMY3 )) ;
extern  DML_LIST dml_copy_list __(( DML_LIST DUMMY0 , DML_LIST DUMMY1 )) ;
extern  BOOLEAN dml_find_data2 __(( DML_LIST DUMMY0 , ANY DUMMY1 , ANY DUMMY2 , 
            DML_ITEM* DUMMY3 , DML_ITEM* DUMMY4 )) ;
extern  DML_ITEM dml_nth_item __(( DML_LIST DUMMY0 , INT DUMMY1 , INT DUMMY2 , 
            DML_ITEM DUMMY3 )) ;
extern  DML_ITEM* dml_array_of_items __(( DML_LIST DUMMY0 )) ;

#endif  /* DMLDEFS */ 


