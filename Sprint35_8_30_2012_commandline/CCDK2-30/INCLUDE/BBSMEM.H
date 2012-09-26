/********************************  BBSMEM.H ********************************/
/**************** include file for BBS memory checking *********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     BBSMEM
#define     BBSMEM
#define     QGLMEM
#include    <bbsdefs.h>

/* 
#define     MALLOC_CHECK
#define     STACK_CHECK 
#define     CREATE_CHECK 
*/ 

#ifdef      MALLOC_CHECK
EXTERN      INT         MALLOC_COUNT ;
EXTERN      long        MALLOC_SIZE ;
EXTERN      INT         FREE_COUNT ;
EXTERN      ANY         MALLOC_PTR[] ;
#endif 

#ifdef      CREATE_CHECK
EXTERN      INT         CREATE_COUNT ;
EXTERN      INT         KILL_COUNT ;
EXTERN      ANY         CREATE_PTR[] ;
#endif 

extern  ANY     bbs_malloc __(( INT DUMMY0 )) ;
extern  void    bbs_free __(( ANY DUMMY0 )) ;
extern  ANY     bbs_create __(( INT DUMMY0 )) ;
extern  void    bbs_kill __(( ANY DUMMY0 )) ;
extern  void    bbs_print_malloc __(( void )) ;
extern  void    bbs_print_create __(( void )) ;
#endif /* BBSMEM*/


