/***********************************************************
 *
 *  Module: gen_fs.h
 *
 *  Description:
 *
 *    This module includes declarations used by the gen_file.c module.
 *
 *  Creation Date: 	18 Oct 93
 *  Author:		Ofer Shofman
 ***********************************************************/

#ifndef  GEN_FS_DEF
#define GEN_FS_DEF

/* gen_str - error codes (range : 2001 - 2999) */

#define E_GEN_FS_INTERNAL      2001
#define E_GEN_FS_STATFS        2002
#define E_GEN_FS_FILE_NEXIST   2003
#define E_GEN_FS_DIR_NEXIST    2004
#define E_GEN_FS_MKDIR         2005
#define E_GEN_FS_OPENDIR       2006
#define E_GEN_FS_OPEN_INP_FILE 2007
#define E_GEN_FS_OPEN_OUT_FILE 2008
#define E_GEN_FS_READ_FILE     2009
#define E_GEN_FS_WRITE_FILE    2010
#define E_GEN_FS_RENAME_FILE   2011
#define E_GEN_FS_DELETE_FILE   2012
#define E_GEN_FS_DELETE_DIR    2013
#define E_GEN_FS_CREATE_FILE   2014
#define E_GEN_FS_COMPRESS      2015
#define E_GEN_FS_FP_TABLE      2016
#define E_GEN_FS_ILL_DIR       2017
#define E_GEN_FS_TEMPNAME      2018
#define E_GEN_FS_MEM           2019
#define E_GEN_FS_ILL_FILE      2020
#define E_GEN_FS_FILE_NWRIT    2021

#define GEN_FS_DEL_DIR        0
#define GEN_FS_DEL_DIR_CONT   1

#define GEN_FS_DIRLIST_ALL    0
#define GEN_FS_DIRLIST_FILES  1
#define GEN_FS_DIRLIST_DIRS   2
#define GEN_FS_DIRLIST_FILES_NOT_HIDDEN  3

#define GEN_FS_FILE_NEXISTS	0
#define GEN_FS_FILE_IS_FILE	1
#define GEN_FS_FILE_IS_DIR	2
#define GEN_FS_FILE_IS_OTHER	3

#define GEN_FS_TIME_ACC       0
#define GEN_FS_TIME_MOD       1
#define GEN_FS_TIME_CNG       2

#define Gen_fsNgzipFilePath        "Gen_fsNgzipFilePath"
#define Gen_fsNgunzipFilePath      "Gen_fsNgunzipFilePath"
#define Gen_fsNunzipFilePath       "Gen_fsNunzipFilePath"
#define Gen_fsNtarFilePath         "Gen_fsNtarFilePath"
#define Gen_fsNcompressFilePath    "Gen_fsNcompressFilePath"
#define Gen_fsNuncompressFilePath  "Gen_fsNuncompressFilePath"


typedef struct {
   char		name[100];
   short	id ;
   short	level ;
   short	parent_id ;
   short	num_children ;
   boolean	is_dir ;
   int		size ;
   time_t	last_access ;
   time_t	last_modify ;
}gen_fs_parse_dir_struc ;

typedef enum {
  TAR_MODE_DIR=0, TAR_MODE_SUBDIRS
} tar_mode_enum ; 

typedef enum {
  TAR_OPT_TAR=0, TAR_OPT_UNTAR, TAR_OPT_UNZIP 
} tar_opt_enum ; 

#ifdef WINDOWS_NV

typedef long off_t;

#endif

#endif


