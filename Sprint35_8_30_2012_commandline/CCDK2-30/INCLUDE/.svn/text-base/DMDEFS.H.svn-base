/****************************** DMDEFS.H ***********************************/
/***************** Include file for data management component **************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef DMDEFS
#define DMDEFS
#include <bbsdefs.h>

/********* Item and List definitions ********/

typedef struct  BBS_item {
        struct  BBS_item *prev_item;
        struct  BBS_item *next_item;
        ANY           record;
        } DML_ITEM_S ;
typedef     DML_ITEM_S *DML_ITEM ;

typedef struct BBS_list {
        DML_ITEM first_item;
        DML_ITEM last_item;
        INT      length;
        } DML_LIST_S ;
typedef     DML_LIST_S *DML_LIST ;


/************ Field access macros ************/
/*
#ifdef __UNIX__
#define DML_FIRST(L)        (dml_first(L))
#define DML_SECOND(L)       (dml_second(L))
#define DML_LAST(L)         (dml_last(L))
#define DML_LENGTH(L)       (dml_length(L))

#define DML_PREV(I)         (dml_prev(I))
#define DML_NEXT(I)         (dml_next(I))
#define DML_RECORD(I)       (dml_record(I))

#define DML_FIRST_RECORD(L) (dml_first_record(L))
#define DML_LAST_RECORD(L)  (dml_last_record(L))
#else
*/
#define DML_FIRST(L)        ((L)->first_item)
#define DML_SECOND(L)       (DML_NEXT(DML_FIRST(L)))
#define DML_LAST(L)         ((L)->last_item)
#define DML_LENGTH(L)       ((L)->length)

#define DML_PREV(I)         ((I)->prev_item)
#define DML_NEXT(I)         ((I)->next_item)
#define DML_RECORD(I)       ((I)->record)

#define DML_FIRST_RECORD(L) DML_RECORD(DML_FIRST(L))
#define DML_LAST_RECORD(L)  DML_RECORD(DML_LAST(L))
/*
#endif
*/
#include <dmlmem.h>
#ifndef MMALLOC_CHECK
#define DML_ITEM_BUF_SIZE   100
#define DML_LIST_BUF_SIZE   20
EXTERN  INT                 DML_LISTS_NUMBER ;
EXTERN  DML_LIST            DML_LIST_BUFFER[] ;
EXTERN  DML_ITEM            DML_ITEM_BUFFER[] ;
#endif

#define DML_LINK(P,N)       { if (P!=NULL) DML_NEXT(P)=N; \
                            if (N!=NULL) DML_PREV(N)=P;}
#define DML_EMPTY_LIST(L)   { DML_FIRST(L) = NULL ; \
                            DML_LAST(L) = NULL ; DML_LENGTH(L) = 0 ; }
/* Macros for traversing */

#define DML_FOR_LOOP(IT0,IT)    for ( IT=IT0 ; IT!=NULL ; IT=dml_next(IT) ) 

#define DML_WALK_LIST(LIST,IT)       \
for ( IT=dml_first(LIST) ; IT!=NULL ; IT=dml_next(IT) ) 


#endif  /* DMDEFS */ 

