/***********************************************************
 *
 *  Module: gen_unix.h
 *
 *  Description:
 *
 *     This module includes declarations used by the
 *     gen_unix.c module.
 *
 *  Creation Date:     8 Aug 00
 *  Wrriten by:        Moshe Levy
 ***********************************************************/

#include <stdio.h>

#ifndef GEN_UNIX_DEF
#define GEN_UNIX_DEF

#define E_GEN_UNIX_INTERNAL      14501
#define E_GEN_UNIX_DIR_NEXIST    14502
#define E_GEN_UNIX_HOSTNAME      14503
#define GEN_UNIX_DIR_NEXIST     -1
#define GEN_UNIX_IS_NOT_DIR     -2
#define GEN_UNIX_FILE_NEXISTS	 0
#define GEN_UNIX_FILE_IS_FILE	 1
#define GEN_UNIX_FILE_IS_DIR	 2
#define GEN_UNIX_FILE_IS_OTHER	 3 

#ifdef WINDOWS_NV
#include <sys/stat.h>
#include <windows.h>
#endif

typedef struct {
   char		name[100];
   short	id ;
   short	level ;
   short	parent_id ;
   short	num_children ;
   char		is_dir ;
   int		size ;
   long 	last_access ;
   long 	last_modify ;
}gen_unix_parse_dir_struc ;

typedef struct {
   int			pid ;
   int			fd_read ;
   int			fd_write ;
   FILE			*fp_read ;
   FILE			*fp_write ;
#ifdef WINDOWS_NV
   HANDLE               hPipe ;
#endif  
} gen_unix_pipe_struc ;


#ifdef WINDOWS_NV

#include <windows.h>

#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)

#ifndef SYS_NMLN
#define SYS_NMLN    257
#endif

struct utsname {
    char sysname[SYS_NMLN];
    char nodename[SYS_NMLN];
    char release[SYS_NMLN];
    char version[SYS_NMLN];
    char machine[SYS_NMLN];
};

#define dirent direct
typedef struct direct 
{
    long d_ino;                /* inode number (not used by MS-DOS)  */
    long d_namlen;             /* Name length  */
    char d_name[257];          /* File name  */
} _DIRECT;

typedef struct _dir_struc
{
    char   *start;             /* Starting position */
    char   *curr;              /* Current position */
    long   size;               /* Allocated size of string table */
    long   nfiles;             /* Number of filenames in table */
    struct direct dirstr;      /* Directory structure to return */
    void*  handle;             /* System handle */
    char   *end;               /* Position after last filename */
    char   *orig_start;             /* Starting position */
    char   *orig_curr;              /* Current position */
    long   orig_size;               /* Allocated size of string table */
    long   orig_nfiles;             /* Number of filenames in table */
    struct direct orig_dirstr;      /* Directory structure to return */
    char   *orig_end;               /* Position after last filename */
    char   orig_scanname[MAX_PATH+3];
} DIR;

#define set_socktype        gen__unix_set_socktype
#define start_sockets       gen__unix_start_sockets
#define uname               gen__unix_uname
#define opendir             gen__unix_opendir
#define readdir             gen__unix_readdir
#define closedir            gen__unix_closedir

/*
#ifndef WINDOWS_NV
typedef DWORD	pid_t ;
#endif
*/
/*
#define  getpid GetCurrentProcessID
*/

/* Define Windows File acces codes   */
/* Since Windows does not support    */
/* Unix X_OK (executable), we set it */
/* to F_OK (existance)               */

#define F_OK 0
#define X_OK 0
#define W_OK 2
#define R_OK 4


#else   /* ( If not WINDOWS_NV ) */

#include <sys/types.h>
#include <sys/utsname.h>
#include <pwd.h>
#include <dirent.h>

#endif /* WINDOWS_NV */


#endif /* GEN_UNIX_DEF */
