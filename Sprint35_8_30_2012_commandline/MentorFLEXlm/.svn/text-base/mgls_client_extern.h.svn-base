/*
** HEADER
**    mgls_client_extern.h -- This file contains the external declaration
**                            of the MGLS interfaces.
**
** COPYRIGHT
**
**    Copyright 1992-2012 Mentor Graphics Corporation
**    All Rights Reserved.
**
** THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY
** INFORMATION WHICH IS THE PROPERTY OF MENTOR
** GRAPHICS CORPORATION OR ITS LICENSORS AND IS
** SUBJECT TO LICENSE TERMS.
**
** WRITTEN BY:
**   Mahmood TahanPesar
**
** DESCRIPTION
**    This file contains the external declaration of MGLS interfaces used
**    by clients.
**
** USER GUIDE
**
*/

#ifndef INCLUDED_MGLS_CLIENT_EXTERN
#define INCLUDED_MGLS_CLIENT_EXTERN

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)

#include "mgls_licensing.h"
#include "mgls_client_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

   extern struct MGLS_ERRNO_STRUCT mgls_errstrct;
   extern long mgls_errno;
   extern long core_Mheap_alloc( int );
   extern long core_Mheap_allocv( int, char * );
   extern long core_Mheap_allocv_attrs( int, const char *, const char *, int );
   extern long core_Mheap_stat( long );
   extern long core_Mheap_free( long );
   extern long core_Mheap_clean( void );
   extern long core_Mheap_set( int, MGLS_ATTR_VALUE );
   extern long core_Mheap_get( int, MGLS_ATTR_VALUE * );
   extern long core_Mheap_valid( int );
   extern long core_Mheap_validv( int, char * );
   extern long core_Mheap_validv_attrs( int, const char *, const char * );
   extern long core_Mheap_getpid(const char *, int *);
   extern long core_Mheap_errs( char ** );
   extern long core_Mheap_release( void );
   extern long core_Mheap_which( int, int * );
   extern long core_Mheap_retry( void );
   extern long core_Mheap_licinfo( struct MGLS_LICINFO * );
   extern long core_Mheap_contains( int, int *, int ** );
   extern long core_Mheap_lookup( int, char * );
   extern long core_Mheap_exists( int );
   extern long core_Mheap_list(int *, int **);
   extern long core_Mheap_member( int, int *, int ** );
   extern long core_Mheap_ferrs( size_t *, size_t *, struct mgls_FLEX_err ** );
   extern long core_Mheap_allocq( int, const char *, int );
   extern long core_Mheap_freeq( long, int );
   extern long core_Mheap_lost( int *, struct mgls_lost_features *[] );
   extern long core_Mheap_msg( long transaction_id, wchar_t *msg_buf );
   extern long core_Mheap_count( int, const char *, long );

#ifdef __cplusplus
}
#endif

#else

#ifdef __cplusplus

#ifndef __signal_h
#include <signal.h>
#endif

extern "C" {

    extern long mgls_errno;
    extern long core_Mheap_alloc(int application_id);
    extern long core_Mheap_allocv(int app_id, char *app_date);
    extern long core_Mheap_allocv_attrs( int, const char *, const char *, int );
    extern long core_Mheap_stat(long transaction_id);
    extern long core_Mheap_free(long transaction_id);
    extern long core_Mheap_clean();
    extern long core_Mheap_set( int which, MGLS_ATTR_VALUE new_value);
    extern long core_Mheap_get( int which, MGLS_ATTR_VALUE *current_value);
    extern long core_Mheap_valid(int application_id);
    extern long core_Mheap_validv( int application_id, char *app_version );
    extern long core_Mheap_validv_attrs( int, const char *, const char * );
    extern long core_Mheap_list(int *num_of_ids, int **id_list);
    extern long core_Mheap_getpid(const char *fname, int *pid);
    extern long core_Mheap_which(int transaction_id, int *actual_id);
    extern long core_Mheap_debug(char *fname, int option);
    extern long core_Mheap_errs(char **error_string);
    extern long Mh_sigchld( void (*disp)(), void (**prev_disp)() );
    extern long Mh_set_sigchld( void (*disp)(), sigset_t m_sa_mask,
                                int *m_sa_flags, void (**prev_disp)());
    extern long core_Mheap_retry();
    extern long core_Mheap_release();
    extern long core_Mheap_licinfo( struct MGLS_LICINFO *license_info );
    extern long core_Mheap_contains( int application_id, int *num_of_ids, int **id_list);
    extern long core_Mheap_lookup(int application_id, char *fname);
    extern long core_Mheap_exists(int application_id);
    extern long core_Mheap_member( int application_id, int *num_of_ids, int **id_list);
    extern long core_Mheap_ferrs(size_t *num_ferrs, size_t *size_of_ferrs_list, struct mgls_FLEX_err *ferrs_list[]);
    extern long core_Mheap_allocq(int application_id, const char *app_date, int quantity);
    extern long core_Mheap_freeq( long transaction_id, int quantity );
    extern long core_Mheap_lost( int *num_features, struct mgls_lost_features *lost_features[] );
    extern long core_Mheap_msg( long transaction_id, wchar_t *msg_buf );
    extern long core_Mheap_count( int application_id, const char *app_date, long flags );
}

#else

extern long mgls_errno;

    extern long core_Mheap_alloc(   /* int application_id */ );
    extern long core_Mheap_allocv(  /* int application_id, char *app_date */ );
    extern long core_Mheap_allocv_attrs( /*  int app_id, const char *app_date, const char *display, int linger_interval  */ );
    extern long core_Mheap_stat(    /*long transaction_id*/ );
    extern long core_Mheap_free(    /*long transaction_id*/ );
    extern long core_Mheap_clean(   );
    extern long core_Mheap_set(     /*int which, MGLS_ATTR_VALUE new_value*/ );
    extern long core_Mheap_get(     /*int which, MGLS_ATTR_VALUE *current_value*/ );
    extern long core_Mheap_valid(   /*int application_id*/ );
    extern long core_Mheap_validv(  /*int application_id, char *app_version*/ );
    extern long core_Mheap_validv_attrs( /*  int application_id, const char *app_version, const char *display*/ );
    extern long core_Mheap_list(    /*int *num_of_ids, int **id_list*/ );
    extern long core_Mheap_getpid(  /*const char *fname, int *pid*/ );
    extern long core_Mheap_which(   /*int transaction_id, int *actual_id*/ );
    extern long core_Mheap_debug(   /*char *fname, int option*/ );
    extern long core_Mheap_errs(    /*char **error_string*/ );
    extern long Mh_sigchld(         /*void (*disp)(), void (*prev_disp)()*/ );
    extern long Mh_set_sigchld(     /*void (*disp)(), sigset_t m_sa_mask,*/
                                    /*int *m_sa_flags, void (*prev_disp)()*/ );
    extern long core_Mheap_retry();
    extern long core_Mheap_release();
    extern long core_Mheap_licinfo( /*struct MGLS_LICINFO *license_info*/ );
    extern long core_Mheap_lookup(	/*int application_id, char *fname*/ );
    extern long core_Mheap_contains(/*int application_id, int *num_of_ids, int **id_list*/);
    extern long core_Mheap_exists(  /*int application_id*/);
    extern long core_Mheap_member(/*int application_id, int *num_of_ids, int **id_list*/);
    extern long core_Mheap_ferrs(/* size_t *num_ferrs, size_t *size_of_ferrs_list, struct mgls_FLEX_err *ferrs_list[] */);
    extern long core_Mheap_allocq(/* int application_id, const char *app_date, int quantity */);
    extern long core_Mheap_freeq(/* long transaction_id, int quantity  */);
    extern long core_Mheap_lost(/* int *num_features, struct mgls_lost_features *lost_features[] */);
    extern long core_Mheap_msg( /*long transaction_id, wchar_t *msg_buf*/ );
    extern long core_Mheap_count( /* int application_id, const char *app_date, long flags */);

#endif

#endif

#endif
