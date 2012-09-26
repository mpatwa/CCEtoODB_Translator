/*
   err_hand.h -
   
   This file defines the interface for the ERROR HANDLER. This will
   enable the file name and line number to be automatic. The structures
   for the error tables are listed below.

*/

/* OGP Includes */
#include "err_tabs.h"

/* Prototype definitions for compiler warnings */
void errHandler(char *file_name,
               int line_number,
               char *func_name,
               int error_table,
               int error_number,
	       char *extra_str,
               int extra_value);

/* Macro to get atomatically the file name, and line number */
#define errPost(func_name,error_table,error_number,extra_str,extra_value) errHandler(__FILE__,__LINE__,func_name,error_table,error_number,extra_str,extra_value)

/* This is the "NULL" value for extra_value */
#define NO_VAL -999

/* For return on NO error */
#define NO_ERROR 0

/* For general use as "Just-Got-An-Error" */
#define GOT_ERROR -1

/* This is the same value as FALSE in wndx.h */
#define IS_WNDX_ERROR 0 

/* ERROR table format ... The real tables are int err_tabs.c */ 
typedef struct errorTabEntry {
   char *error_number;
   char *error_msg;
   int error_level;
} errorTabEntry;

/* MAIN error table structure defined in err_tabs.c */
typedef struct {
   char *table_name;
   errorTabEntry *table;
} errorTable;

