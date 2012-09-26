/***********************************************************
 *
 *  Module: gen.h
 *
 *  Description:
 *
 *    This module includes general declarations used by any project
 *    which uses the general libraries.
 *
 *  Creation Date:	18 Oct 93
 *  Author:		Ofer Shofman
 ***********************************************************/

#ifndef GEN_DEF
#define GEN_DEF

#ifdef WINDOWS_NV
#ifndef SHOW_ALL_WARNINGS
#pragma warning( disable : 4101 4018 4761 4244 4305 4520)
#endif
#pragma warning( error : 4020 4002 4700 4013)
#endif

#include <stdio.h>
#include <fcntl.h>

#ifndef WINDOWS_NV
#include <unistd.h>
#endif
#include <sys/stat.h>

#ifdef WINDOWS_NV
#define stat _stat
#endif

#include <sys/types.h>
#ifdef WINDOWS_NV
#include <time.h>
#endif
#ifndef WINDOWS_NV
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>

#if defined(SOLARIS_2) && !defined(__GNUC__)
/* definitions for some semi-standard mathematical routines */

#undef erf
#undef erfc
#undef hypot
#undef isnan
#undef j0
#undef j1
#undef jn
#undef lgamma
#undef y0
#undef y1
#undef yn
#undef acosh
#undef asinh
#undef atanh
#undef cbrt
#undef logb
#undef scalb
#undef expm1
#undef log1p
#undef rint
#undef significand
#undef copysign

#ifndef __P
#define __P(a) a
#endif

extern double	erf __P((double));
extern double	erfc __P((double));
extern double	hypot __P((double, double));
extern int	isnan __P((double));
extern double	j0 __P((double));
extern double	j1 __P((double));
extern double	jn __P((int, double));
extern double	lgamma __P((double));
extern double	y0 __P((double));
extern double	y1 __P((double));
extern double	yn __P((int, double));
extern double	acosh __P((double));
extern double	asinh __P((double));
extern double	atanh __P((double));
extern double	cbrt __P((double));
extern double	logb __P((double));
extern double	scalb __P((double, int));
extern double	expm1 __P((double));
extern double	log1p __P((double));
extern double	rint __P((double));
extern double	copysign __P((double, double));

#endif

#ifndef WINDOWS_NV
#include <dirent.h>
#endif
#if defined(SOLARIS_1) || defined(SOLARIS_2) || defined(LINUX)
#include <values.h>
#endif
#ifndef NO_DATE_TIME
#include <datetime.h>
#endif
#include <errno.h>

#if defined(AIX4) || defined (LINUX)
#include <time.h>
#include <sys/select.h>
#endif

#include <gen_nls.h>
#include <gen_unix.h>

#ifndef WINDOWS_NV
extern int errno;
extern char *strerror (int);
#endif

/* 4.May.2005 - ALB: #if defined(XM_INC) && !defined(LINUX) */
#if defined(XM_INC)
#include <gui_xdefs.h>
#endif

#ifdef AIX4
#define USHORT_BF  ushort
#define UCHAR_BF   uchar
#else
#define USHORT_BF  ushort
#define UCHAR_BF   uchar
typedef unsigned char  uchar;
#if ! defined(SOLARIS_2) && ! defined(LINUX)
typedef unsigned long  ulong;
#endif
#endif

#if defined(LINUX)
/* typedef unsigned long  ulong; defined in SOLARIS_2 */
#define CHEND16(x) ((((uint16_t)(x) << 8) + ((uint16_t)(x) >> 8)) & 0xffff)
#define CHEND32(x) (((uint32_t)(x)<<24) + (((uint32_t)(x)<<8) & 0xff0000) + \
                   (((uint32_t)(x)>>8) & 0xff00) + ((uint32_t)(x)>>24))
#define ENDIAN16(x) (x) = CHEND16(x)
#define ENDIAN32(x) (x) = CHEND32(x)
#define ENDIAN64(x) { \
   int32_t *_j = (int32_t *)(&(x));\
   int32_t _t;\
   ENDIAN32(_j[0]); ENDIAN32(_j[1]);\
   _t = _j[0]; _j[0] = _j[1]; _j[1] = _t; }
#define ENDIAN_INDEX(x) (((x) >= 0 && (x) <= 0x7fff) ? (x) : CHEND32(x))

#define ENDIAN_TIMEVAL(tv, num) {\
   int32_t _itv;\
   ASSERT(L_PTR(tv) && (num) >= 1 && (num) < 1000, ;);\
   for (_itv = 0; _itv < (num); _itv++) {\
      ENDIAN32((tv)[_itv].tv_sec);\
      ENDIAN32((tv)[_itv].tv_usec);\
   }}
#define log2(x) (log(x)/M_LN2)
#endif /* LINUX */

#ifdef WINDOWS_NV
#undef E_OK
#include <float.h>
typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned long  ulong;
#define CHEND16(x) ((((ushort)(x) << 8) + ((ushort)(x) >> 8)) & 0xffff)
#define CHEND32(x) (((ulong)(x)<<24) + (((ulong)(x)<<8) & 0xff0000) + \
                    (((ulong)(x)>>8) & 0xff00) + ((ulong)(x)>>24))
#define ENDIAN16(x) (x) = CHEND16(x)
#define ENDIAN32(x) (x) = CHEND32(x)
#define ENDIAN64(x) { \
   long *_j = (long *)(&(x));\
   long _t;\
   ENDIAN32(_j[0]); ENDIAN32(_j[1]);\
   _t = _j[0]; _j[0] = _j[1]; _j[1] = _t; }
#define ENDIAN_INDEX(x) (((x) >= 0 && (x) <= 0x7fff) ? (x) : CHEND32(x))

#define ENDIAN_TIMEVAL(tv, num) {\
   int     _itv;\
   ASSERT(L_PTR(tv) && (num) >= 1 && (num) < 1000, ;);\
   for (_itv = 0; _itv < (num); _itv++) {\
      ENDIAN32((tv)[_itv].tv_sec);\
      ENDIAN32((tv)[_itv].tv_usec);\
   }}
#define MAXFLOAT FLT_MAX
#define log2(x) (log(x)/M_LN2)
#define DLLEXP(x) __declspec(dllexport) x
#endif

#if !defined(WINDOWS_NV)
  #define DLLEXP(x) x
#endif

#if ! defined(WINDOWS_NV) && ! defined(LINUX)
#define CHEND16(x) (x)
#define CHEND32(x) (x)
#define ENDIAN16(x)
#define ENDIAN32(x)
#define ENDIAN64(x)
#define ENDIAN_TIMEVAL(tv, num)
#endif

#ifndef isascii
#define isascii(_c)   ( (unsigned)(_c) < 0x80 )
#endif

typedef unsigned char  boolean;

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

#define LENGTH(x) (sizeof((x))/sizeof((x)[0]))
#define OFFSET_OF(type,field)	((int)(&((type *)0)->field))

#ifndef MAX  /* In sys/param.h */
#define MAX(x,y)  ((x)>(y)?(x):(y))
#endif
#ifndef MIN
#define MIN(x,y)  ((x)<(y)?(x):(y))
#endif
#define SGN(x) (x==0 ? 0 : (x > 0 ? 1 : -1))
#define ABS(x)   ((x)>0 ? (x) : -(x))
#define INT(x) ( (x)>=0 ? (int)((x)+0.5) : (int)((x)-0.5) )
#define SETINT(x,expr) { double _exp = (expr); x = INT(_exp); }
#if !defined(SOLARIS_1) && !defined(SOLARIS_2) && !defined(LINUX)
#ifdef MAXINT
#undef MAXINT
#endif
#define MAXINT 0x7fffffff

#ifdef MAXSHORT
#undef MAXSHORT
#endif
#define MAXSHORT 0x7fff
#endif /* !defined(SOLARIS_1) && !defined(SOLARIS_2) && !defined(LINUX) */

#if !defined(LINUX)
#define MININT (-MAXINT)
#ifdef MINSHORT
#undef MINSHORT
#endif
#ifdef MINSHORT
#undef MINSHORT
#endif
#define MINSHORT (-MAXSHORT)
#endif /* LINUX */

/* the 'do { } while (0)' loop is designed to allow 'if () MACRO; else ...'
   without it the result is the syntax error 'if () { ... }; else ...'
*/

#define SWAP(a,b,type) do { \
    type _tmp_var_ = a; \
    a = b; \
    b = _tmp_var_; } while (0)

#define NUMCMP(n1,n2,tol) (ABS((n1)-(n2)) <= tol)

/* qsort comparison function. dir = 1 descending (highest first); */
#define Q_NUMCMP(n1,n2,dir,final) { \
  if ((n1)<(n2)) return  (dir); \
  if ((n1)>(n2)) return -(dir); \
  if (final) return  0; }


#define IS_BETWEEN(min,n,max)	(((min) <= (n)) && ((n) <= (max)))
#define IS_BOUNDED(n,max)	((n) >= 0 && (n) < (max))
#define IS_INRANGE(min,n,max)	(min <= max ? IS_BETWEEN(min,n,max) : !IS_BETWEEN(max,n,min))

/* It looks like the CPP Compiler doesn't like the previous */
/* name of that structure, which was 'exception', therefore */
/* it was changed. ZW & VG                                  */

#ifdef __cplusplus
#define our_exception __math_exception
#else
#define our_exception exception
#endif

#if defined(SOLARIS_2) && !defined(__GNUC__)
struct our_exception {
	int    type;
	char  *name;
	double arg1;
	double arg2;
	double retval;
};
#endif

typedef int (*qsort_fn) (const void *, const void *);


/* MACROs to make gen__i18n names shorter */
/******************************************/
#define MAX_CHAR_SIZE 4

typedef char  wchar[MAX_CHAR_SIZE];

#define MBLEN(s)                 ((s)==NULL ? 0 : gen__i18n_mbLen (s))
#define MBSTRLEN(s)              ((s)==NULL ? 0 : gen__i18n_mbStrLenChars (s))  /* String len in chars   */
#define MBINCR(ptr)              gen__i18n_mbCharNext(ptr)                    /* like p++ but in chars */
#define MBDECR(start, ptr)       gen__i18n_mbCharPrev(start,ptr)              /* like p-- but in chars */
#define MBNINCR(ptr, n)          gen__i18n_mbNcharNext(ptr, n)                /* like p++ but in chars n times */
#define MBNDECR(start, ptr, n)   gen__i18n_mbNcharPrev(start,ptr, n)          /* like p-- but in chars n times */
#define MBSTRNCPY(dst, src, n)   gen__i18n_mbStrNcpyChars(dst, src, n)        /* strncpy in chars      */
#define MBSTRNCAT(dst, src, n)   gen__i18n_mbStrNcatChars(dst, src, n)        /* strncat in chars      */
#define MBSTRNCMP(s1, s2, n)     gen__i18n_mbStrNcmpChars(s1, s2, n)          /* strncmp in chars      */
#define MBSTRNCASECMP(s1, s2, n) gen__i18n_mbStrNicmpChars(s1, s2, n)         /* strncasecmp in chars      */
                                                                              /* set the charcter at n     */
		                                                              /* Not working if c is kanji */
#define MBSET(s,n,c)             {if ((s)!=NULL) *(gen__i18n_mbNcharNext(s,n)) = (c);}


/* The following MACROs should not affect :
  gen_str.c, gen_i18n.c & gen_lic.c
  ******************************************/

#if !defined(GEN_STR_C) && !defined(GEN_I18N_C) && !defined(GEN_LIC_C)

#ifdef isalpha
#undef isalpha
#endif
#define isalpha(p1)		gen__str_isalpha(p1)

#ifdef isalnum
#undef isalnum
#endif
#define isalnum(p1)		gen__str_isalnum(p1)

#ifdef iscntrl
#undef iscntrl
#endif
#define iscntrl(p1)		gen__str_iscntrl(p1)

#ifdef isprint
#undef isprint
#endif
#define isprint(p1)		gen__str_isprint(p1)

#ifdef isgraph
#undef isgraph
#endif
#define isgraph(p1)		gen__str_isgraph(p1)

#ifdef isspace
#undef isspace
#endif
#define isspace(p1)		gen__str_isspace(p1)

#ifdef isdigit
#undef isdigit
#endif
#define isdigit(p1)		gen__str_isdigit(p1)

#ifdef ispunct
#undef ispunct
#endif
#define ispunct(p1)		gen__str_ispunct(p1)

#ifdef isupper
#undef isupper
#endif
#define isupper(p1)		gen__str_isupper(p1)

#ifdef islower
#undef islower
#endif
#define islower(p1)		gen__str_islower(p1)

#ifdef strncat
#undef strncat
#endif
#define strncat(p1, p2, p3)	gen__str_strncat(p1, p2, p3)

#ifdef strchr
#undef strchr
#endif
#define strchr(p1, p2)		gen__str_strchr(p1, p2)

#ifdef strcmp
#undef strcmp
#endif
#define strcmp(p1, p2)		gen__str_strcmp(p1, p2)

#ifdef strcspn
#undef strcspn
#endif
#define strcspn(p1, p2)		gen__str_strcspn(p1, p2)

#ifdef strncmp
#undef strncmp
#endif
#define strncmp(p1, p2, p3)	gen__str_strncmp(p1, p2, p3)

#ifdef strncpy
#undef strncpy
#endif
#define strncpy(p1, p2, p3)	gen__str_strncpy(p1, p2, p3)

#ifdef strpbrk
#undef strpbrk
#endif
#define strpbrk(p1, p2)		gen__str_strpbrk(p1, p2)

#ifdef strspn
#undef strspn
#endif
#define strspn(p1, p2)		gen__str_strspn(p1, p2)

#ifdef strstr
#undef strstr
#endif
#define strstr(p1, p2)		gen__str_strstr(p1, p2)

#ifdef toupper
#undef toupper
#endif
#define toupper(p1)		(isascii(p1) ? gen__str_toupper2(p1) : (p1))

#ifdef tolower
#undef tolower
#endif
#define tolower(p1)		(isascii(p1) ? gen__str_tolower2(p1) : (p1))

#ifdef strrchr
#undef strrchr
#endif
#define strrchr(p1, p2)		gen__str_strrchr(p1, p2)

#ifdef strtok
#undef strtok
#endif
#define strtok(p1, p2)		gen__str_strtok(p1, p2)

#ifdef strncasecmp
#undef strncasecmp
#endif
#define strncasecmp(p1, p2, p3)	gen__str_strncasecmp(p1, p2, p3)

#endif


#ifdef WINDOWS_NV

#ifndef strcasecmp
#define strcasecmp(_a, _b) ( gen__str_cmp_nc (_a, _b) )
#endif

#ifndef strncasecmp
#define strncasecmp(_a, _b, _c) ( gen__str_ncmp_nc (_a, _b, _c) )
#endif

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif
#ifndef M_SQRT2
#define M_SQRT2	1.41421356237309504880
#endif
#ifndef M_LN2
#define M_LN2	0.69314718055994530942
#endif

#endif


#define PIE M_PI
#ifndef M_PI_2
#define	M_PI_2		1.57079632679489661923
#endif
#ifndef M_SQRT1_2
#define	M_SQRT1_2	0.70710678118654752440
#endif
#define DEG(x) (180.*(x)/PIE)
#define RAD(x) (PIE*(x)/180.)

#if defined(SOLARIS_2) || defined(AIX4)
#define log2(x) log(x)/log(2)
#endif

#define E_OK	0

#define L_PTR(p) ((unsigned int)(p) > 1000)
#define SET_PTR(p,val) { if (L_PTR(p)) *p = val; }

#define WILD_CHAR	'*'
#define WILD_STR	"*"
#define NEG_CHAR	'!'
#define NEG_STR		"!"
#define SEP_CHAR	';'
#define SEP_STR		";"
#define DELIM_CHAR	','
#define DELIM_STR	","
#define QUOTE_CHAR	'\\'
#define QUOTE_STR	"\\"
#define SPACE_CHAR	' '
#define SPACE_STR	" "
#define SQL_DELIM_CHAR	'|'
#define SQL_DELIM_STR	"|"

#define LF_STR		"\n"

#define GEN_UNITS_INCH     0
#define GEN_UNITS_MM       1
#define GEN_UNITS_MIL      2
#define GEN_UNITS_MICRON   3
#define GEN_UNITS_SQR_INCH 4
#define GEN_UNITS_SQR_MM   5
#define GEN_UNITS_NONE     6

#define MM(x)   ((x) * 25.4)
#define INCH(x) ((x) / 25.4)

#define PIXEL2INCH(x)	((x)   /10160000.)
#define INCH2PIXEL(x)	INT((x)*10160000.)
#define PIXEL2MM(x)	PIXEL2INCH(MM(x))
#define MM2PIXEL(x)	INCH2PIXEL(INCH(x))

#define STR(x)	#x

#define KB	1024
#define BYTE2K(x) ( ((x) - 1)/KB + 1)

#define LOG_MSG_NONE	  "NONE"
#define LOG_MSG_INTERR	  "INTERNAL"
#define LOG_MSG_WARN      "WARNING"

#undef  ASSERT
#define ASSERT(x,action) \
   { if (!(x)) { \
      gen__log_write (LOG_MSG_INTERR,"(%s) at %s %d\n", STR(x), __FILE__, __LINE__); \
      gen__err_sound(); action;} }

#define QUIET_ASSERT(x,action) \
   { if (!(x)) { \
      gen__log_disable_stdout(); \
      gen__log_write (LOG_MSG_INTERR,"(%s) at %s %d\n", STR(x), __FILE__, __LINE__); \
      gen__log_enable_stdout(); \
      action;} }

#define QUITE_ASSERT(x,action) QUIET_ASSERT(x,action)

/* Used in program as substitute to avoid confusing get_pt */
#define _LINE_SYMBOL	__LINE__
#define _FILE_SYMBOL	__FILE__

#define GEN_FOPEN(file,mode)  gen__fs_fopen((file),(mode), __FILE__, __LINE__)
#define GEN_FCLOSE(file)      gen__fs_fclose((file), __FILE__, __LINE__); (file) = NULL;
#define COND_GEN_FCLOSE(file) { if (file) GEN_FCLOSE(file); }

#define SYSERR(func) { \
   gen__log_write (LOG_MSG_INTERR, "System %s call error : %s at %s %d", \
		   func, gen__err_unix(), __FILE__, __LINE__); }

#define STATUS_CHECK { if (status!=E_OK) {status_check(__FILE__, __LINE__); return(status);} }
#define STATUS_RET   { if (status!=E_OK) {status_check(__FILE__, __LINE__); return;} }
#define STATUS_ACT(action) \
	{ if (status!=E_OK) {status_check(__FILE__, __LINE__); action; return(status);} }
#define STATUS_ERR(_status) \
	{ if (_status!=E_OK) {status_check(__FILE__, __LINE__); return(_status); } }

#define STATUS_CHECK_DEBUG(time,str) \
	 { if (status!=E_OK) {status_check(__FILE__, __LINE__); return(status);} \
	   else printf("%s %d %s %s\n", _FILE_SYMBOL, _LINE_SYMBOL, (time ? gen__str_cur_date(1) : ""), str); }

#define STRINIT(s)     { if (s) (s)[0]='\0'; }
#define STRNL(s)       ((s)?(s):"")
#define STRLEN(s)      ((s)?(int)strlen(s):0)
#define STREMPTY(s)    ((s) ? (s)[0]=='\0' : TRUE)
#define STRCMP(s1, s2) strcmp(STRNL(s1), STRNL(s2))

#define STRCPY_PRT(s1,s2,n) \
   { ASSERT((n) <= STRLEN(s2), ;);  \
     gen__str_ncpy (s1,s2,n); }

      /* nili 8/97 : STRNCPY is not the proper thing to use if you want to copy
	 part of a string . it requires that the length of the source will be
	 equal or less than the number of characters you want to copy .
	 but since there are over than 300 places in the genesis that calls
	 that func i will not change that define (all the places probably copy
	 all the source )- i added the above STRCPY_PRT . enjoy ! */

/* same as nili's comment above but for BYTENCPY */
#define BYTENCPY_PRT(s1,s2,n) \
   do { ASSERT((n) <= STRLEN(s2), ;);  \
	{ boolean orig_lang = gen__str_set_eng (); \
	  gen__str_ncpy (s1,s2,n); \
	  gen__str_set_lang(orig_lang); }} while (0)

#define STRNCPY(s1,s2,n) \
   do { ASSERT(STRLEN(s2) <= (n), ;);  \
	gen__str_ncpy (s1,s2,n); } while (0)

#define STRNCPY2(s1,s2) STRNCPY(s1,s2,sizeof(s1)-1)
/*KE - new macros for saving time in foreign languages */
#define BYTENCPY(s1,s2,n) \
   do { ASSERT(STRLEN(s2) <= (n), ;);  \
       {boolean orig_lang = gen__str_set_eng (); \
	gen__str_ncpy (s1,s2,n); \
        gen__str_set_lang(orig_lang); }} while (0)

#define BYTENCPY2(s1,s2) BYTENCPY(s1,s2,sizeof(s1)-1)

#define BYTENCAT(s1,s2,n)  \
   do { boolean orig_lang = gen__str_set_eng (); \
	gen__str_ncat (s1,s2,n); \
        gen__str_set_lang(orig_lang); } while (0)

#define BYTENCAT2(s1,s2)  BYTENCAT(s1,s2,sizeof(s1)-1)
#define BYTECMPNC(s1,s2)  gen__str_eq_nc_not_multibyte(s1,s2)
#define BYTECMP(s1,s2)	  gen__str_cmp_not_multibyte(s1,s2)
#define BYTESTRSTR(s1,s2) gen__str_strstr_not_multibyte(s1,s2)

      /* if you want to copy only part of s2 - dont use  STRNCAT. it will not work.
	 use strncat */
#define STRNCAT(s1,s2,n)  gen__str_ncat ((s1),(s2),(n))

#define STRNCAT2(s1,s2)	  STRNCAT(s1,s2,sizeof(s1)-1)

#define EXTEND_STRNCAT(s1,s2,n)		\
   { int _len1 = STRLEN (s1);		\
     int _len2 = STRLEN (s2);		\
     ASSERT (_len1 <= (n), ;);		\
     if (_len2 >= n - _len1) {		\
       n += MAX(n, _len2);		\
       s1 = REALLOC(s1, (n + 1));	\
       ASSERT (L_PTR (s1), ;);		\
     }					\
     ASSERT(_len2 <= (n) - _len1, ;);	\
     gen__str_ncat ((s1),(s2),(n));	\
   }

#define VSPRINTF(str,arguments,args) \
   { int len = vsprintf(str, arguments, args); \
     ASSERT(len < sizeof(str), ;); }

#define SKIP_BLANKS(p) { while (*p == ' ') p++; }

#define gen__str_dup(s) gen__str_dup_fn((s), __FILE__, __LINE__)

#define DUPSTR(src,dst) \
   { dst = gen__str_dup_fn((src), __FILE__, __LINE__); \
     ASSERT((dst) != NULL, return (E_GEN_STR_MEM)); }

#define DUPSTR_NOCHK(src,dst) \
   { dst = gen__str_dup_fn((src), __FILE__, __LINE__); \
     ASSERT((dst) != NULL, ;) }


#define CKSUMSTR(str,sum) \
   { char *p = str; for (sum = 0; *p != '\0'; sum += *p++); }

#define FREE_LIST(_list,_num_list) { \
   gen__str_list_text_free (_list,_num_list);  \
   (_list) = NULL; (_num_list) = 0; }

#define MEMSET(_str)    memset(&(_str),  0, sizeof(_str))
#define MEMSETP(_str_p) memset((_str_p), 0, sizeof(*(_str_p)))
#define MEMCPY(p1,p2)	memcpy(p1, p2, sizeof(p1));

#define LEN_PATH	512
#define LEN_NAME	64
#define LEN_DESC	1000
#define LEN_LONGNAME	1000
#define ENT_NAME_CHARS "abcdefghijklmnopqrstuvwxyz0123456789+-_."
#define LEN_SIGNATURE	5000

typedef struct {
   char  *name;
   char  *val;
} LibArg;

#define LibSetArg(args,aname,aval,num) { args[num].name = (aname); args[num].val = (char *) (aval); }

#define LIB_ARGS            static LibArg _LibArgs[100]; static int _LibArgsNum
#define LIBRESET            {_LibArgsNum = 0;}
#define LIBSET(name, val)   {LibSetArg (_LibArgs,name,val,_LibArgsNum);_LibArgsNum++;}

typedef struct {
   int   xc;
   int   yc;
   int   xoff;
   int   yoff;
   int   angle;
   int   mirror;
   int   polarity;
   double ang, sang, cang;    /* Real angle (rad) (valid when angle = -1) */
                              /*  plus the sine and cosine of the angle */
} trans_struc;

#define ZERO_TRANS(t) ((t)->xc    == 0 && (t)->yc     == 0 && \
                       (t)->xoff  == 0 && (t)->yoff   == 0 && \
                       (t)->angle == 0 && (t)->mirror == 0 && \
                       (t)->polarity == 0)

#define DEG2TRANS(deg,t)  mat__trans_set_angle2(t,deg,0)
#define TRANS2DEG(t) \
	(((t)->angle < 0) ? DEG((t)->ang) : ((t)->angle * 90.0))
#define TRANS2RAD(t) \
	(((t)->angle < 0) ? ((t)->ang) : RAD((t)->angle * 90.0))

#define SET_TRANS(t,pc,poff,angle,_mirror) { MEMSETP(t); \
	(t)->xc   = (pc)->x;   (t)->yc   = (pc)->y;   \
	(t)->xoff = (poff)->x; (t)->yoff = (poff)->y; \
	DEG2TRANS(angle, t);   (t)->mirror = _mirror; }

typedef struct {
   int x_min, y_min;
   int x_max, y_max;
} limits_struc;

#define SET_LIMITS(l,_x_min,_y_min,_x_max,_y_max) \
    { (l)->x_min = _x_min; (l)->y_min = _y_min; \
      (l)->x_max = _x_max; (l)->y_max = _y_max; }

#define SET_LIMITS_P(l,p,q)     SET_LIMITS(l, MIN((p)->x, (q)->x), MIN((p)->y, (q)->y), \
					      MAX((p)->x, (q)->x), MAX((p)->y, (q)->y))
#define SET_LIMITS_D(l,p,dx,dy) SET_LIMITS(l, (p)->x-(dx), (p)->y-(dy), (p)->x+(dx), (p)->y+(dy))

#define LIMITS_INIT(l) SET_LIMITS(l,MAXINT,MAXINT,MININT,MININT)

#define IS_DEF_LIMITS(l) ((l)->x_min != MAXINT)

#define LIM_LL(l,p)	{ (p)->x = (l)->x_min; \
			  (p)->y = (l)->y_min; }
#define LIM_UR(l,p)	{ (p)->x = (l)->x_max; \
			  (p)->y = (l)->y_max; }
#define LIM_DX(l)	((l)->x_max - (l)->x_min)
#define LIM_DY(l)	((l)->y_max - (l)->y_min)
#define LIM_DM(l,w,h)	{ (w) = LIM_DX(l); \
			  (h) = LIM_DY(l); }
#define LIM_AREA(l)	((double)LIM_DX(l) * (double)LIM_DY(l))

#define LIM_CX(l)	((l)->x_max + (l)->x_min)/2
#define LIM_CY(l)	((l)->y_max + (l)->y_min)/2
#define LIM_CP(l,p)	{ (p)->x = LIM_CX(l); \
			  (p)->y = LIM_CY(l); }

/* check if two given limits that are increased by epsilon
   on both dimention overlap */
#define LIMITS_OVERLAP_EPSILON(l1,l2,eps) \
    (((l1)->x_min - (eps) <= (l2)->x_max) &&  \
     ((l1)->x_max + (eps) >= (l2)->x_min) &&  \
     ((l1)->y_min - (eps) <= (l2)->y_max) &&  \
     ((l1)->y_max + (eps) >= (l2)->y_min))

#define LIMITS_OVERLAP(l1,l2) LIMITS_OVERLAP_EPSILON(l1,l2,0)

#define LIMITS_IN_LIMITS_EPSILON(l1,l2,eps) \
    (((l1)->x_min + (eps) >= (l2)->x_min) && \
     ((l1)->x_max - (eps) <= (l2)->x_max) && \
     ((l1)->y_min + (eps) >= (l2)->y_min) && \
     ((l1)->y_max - (eps) <= (l2)->y_max))

#define LIMITS_IN_LIMITS(l1,l2)	LIMITS_IN_LIMITS_EPSILON(l1,l2,0)

#define LIMITS_NOT_INTERSECT(l1,l2) \
    (((l1)->x_max < (l2)->x_min || (l1)->x_min > (l2)->x_max) || \
     ((l1)->y_max < (l2)->y_min || (l1)->y_min > (l2)->y_max ))

#define LIMITS_INTERSECT(l1,l2) (!LIMITS_NOT_INTERSECT(l1,l2))

#define LIMITS_EQUAL_EPSILON(l1,l2,eps) \
    (LIMITS_IN_LIMITS_EPSILON(l1,l2,eps) && LIMITS_IN_LIMITS_EPSILON(l2,l1,eps))


/* We faced problems in English enviroment when
   running into japanese character. Therfore _tolower
   and _toupper are canceled since 11/12/96 - ERAN   */

#ifdef _tolower
#undef _tolower
#endif
#define _tolower(c) tolower(c)

#ifdef _toupper
#undef _toupper
#endif
#define _toupper(c) toupper(c)


#if defined(AIX4)
#undef NULL
#if defined(__cplusplus) || defined(c_plusplus)
#define NULL 0
#else
#define NULL ((void *) 0)
#endif
#endif

#if defined(SOLARIS_1)
#undef NULL
#define NULL ((void *) 0)
#endif

typedef struct {
   int x, y;
} point_struc;

#define PNT_DX(p1,p2)	  ((p1)->x - (p2)->x)
#define PNT_DY(p1,p2)	  ((p1)->y - (p2)->y)
#define PNT_DM(p1,p2,w,h) { (w) = PNT_DX(p1,p2); \
			    (h) = PNT_DY(p1,p2); }
#define PNT_CX(p1,p2)	  ((p1)->x + (p2)->x)/2
#define PNT_CY(p1,p2)	  ((p1)->y + (p2)->y)/2
#define PNT_CP(p1,p2,p)	  { (p)->x = PNT_CX(p1,p2); \
			    (p)->y = PNT_CY(p1,p2); }

#define PNTLIM(p1,p2,l)	  SET_LIMITS(l, MIN((p1)->x,(p2)->x), MIN((p1)->y,(p2)->y), \
					MAX((p1)->x,(p2)->x), MAX((p1)->y,(p2)->y));
#define PNTLEN(p1,p2,d)	  { double _w,_h; PNT_DM(p1,p2,_w,_h); d = sqrt(_w*_w + _h*_h); }
#define PNTANG(p1,p2,a)	  { double _w,_h; PNT_DM(p1,p2,_w,_h); a = (_w||_h ? atan2(_h,_w) : 0); }

#define PNTCMP_TOL(p1,p2,tol)	mat__util_pntcmp_tol(p1,p2,tol)
#define PNTCMP(p1,p2)		PNTCMP_TOL(p1,p2,0)

#define PNTADD(p,p1,p2)	  { (p)->x = (p1)->x + (p2)->x; \
			    (p)->y = (p1)->y + (p2)->y; }
#define PNTSUB(p,p1,p2)	  { (p)->x = (p1)->x - (p2)->x; \
			    (p)->y = (p1)->y - (p2)->y; }

/* This structure is used by gen libraries which return list of indices
 * to avoid repeated allocation of data when performance counts
 */
typedef struct {
   int *ind;
   int  num, alloc;
   int  add_alloc;
   int  magic;
} gen_ind_list_struc;

#ifdef WINDOWS_NV
#define SPRINTF_S(_str, ...) sprintf_s(_str, sizeof(_str), ##__VA_ARGS__ )
#else
#define SPRINTF_S(_str, ...) sprintf(_str, ##__VA_ARGS__ )
#endif

#ifdef WINDOWS_NV
#define VSPRINTF_S(_str, ...) vsprintf_s(_str, sizeof(_str), ##__VA_ARGS__ )
#else
#define VSPRINTF_S(_str, ...) vsprintf(_str, ##__VA_ARGS__ )
#endif

#ifdef WINDOWS_NV
#define SSCANF_S(_str, _fmt, ...) sscanf_s(_str, _fmt, ##__VA_ARGS__ )
#else
#define SSCANF_S(_str, _fmt, ...) sscanf(_str, _fmt, ##__VA_ARGS__ )
#endif

#define IND_LIST_MAGIC 12121212

#define GEN_INIT_IND_LIST(_resp,_flag,_alloc,_add) \
   { if (!(_flag)) {  \
      status = gen__sort_ind_list_construct \
          (&(_resp), (_alloc), (_add)); \
      STATUS_CHECK; \
      (_flag) = TRUE; \
   } else { \
       status = gen__sort_ind_list_clear(&(_resp)); STATUS_CHECK; }}

/* used to reduce fragmentation and extra allocations by joining objects
   into one list, much like gen_str_sprintf_struc is used for strings */
typedef struct {
  int ix, num;
} obj_ref_struc;

#include <gen_mem.h>

#endif
