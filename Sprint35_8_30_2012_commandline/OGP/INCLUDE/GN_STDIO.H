/*
$Workfile:   gn_stdio.h  $

$Header:   P:/pvcs/sscope9/include/gn_stdio.h_v   9.0   11 Mar 1996 11:22:16   softadmn  $

$Modtime:   18 Jan 1995 14:58:08  $

$Log:   P:/pvcs/sscope9/include/gn_stdio.h_v  $
 * 
 *    Rev 9.0   11 Mar 1996 11:22:16   softadmn
 * Initial revision.
 * 
 *    Rev 8.0   18 Jan 1995 14:58:08   softadmn
 * Initial revision.
*/

/* 
   gn_stdio.h -
   
   These define the general purpose defines

*/

#define PRE_PROCESS_MENU -1

#define isodd(x) (x & 1)
#define MAX(X,Y) ( ((X) > (Y)) ? (X) : (Y) )
#define MIN(X,Y) ( ((X) < (Y)) ? (X) : (Y) )

#define MINI_STRING 16
#define SHORT_STRING 32
#define MEDIUM_STRING 80
#define LONG_STRING 128
#define REAL_LONG_STRING 1000

/* File access defines ... access(fileName,CODE) */

#define EXIST_OK 00
#define WRITE_OK 02
#define READ_OK 04
#define RW_OK 06

#ifndef ON
#define ON 	1
#endif
#ifndef OFF
#define OFF 0
#endif

#define TRUE 1
#define FALSE 0

#define TEMP_DEFAULT "."
