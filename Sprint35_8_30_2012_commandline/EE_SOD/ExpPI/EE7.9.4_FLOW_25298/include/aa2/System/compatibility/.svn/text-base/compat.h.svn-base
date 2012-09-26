#ifndef _COMPAT_H_
#define _COMPAT_H_

#ifdef UNIX

#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h> 
#include <string.h>

#ifndef MAINWIN

// Macroses:
#define _MAX_PATH		PATH_MAX
#define _MAX_EXT		PATH_MAX
#define APIENTRY		


// Types:
#define __int64			long long
#define BOOL			bool
typedef char			TCHAR;
typedef unsigned char	BYTE;
typedef const TCHAR *	LPCTSTR;

#endif  // MAINWIN

// Variables:
#define _environ		environ

// Functions:
#define _access			access
#define _atoi64			atoll
#define _getcwd			getcwd
#define _mkdir			mkdir
#define _vsprintf		vsprintf
#define _vsnprintf		vsnprintf
#define _vsnwprintf     vsnprintf
#ifndef MAINWIN
#define stricmp			strcasecmp
#define strnicmp		strncasecmp
#endif

char * itoa(int, char *, int);
char * _i64toa(__int64, char *, int);

char * strupr(char *);
char * strlwr(char *);

int mkdir(const char *);

// Analog to Windows function. Gets the filename of the currently running executable.
// Copies an absolute filename of the currently running executable to the array 'buf'
// of size 'size'. The first argument is dummy an has to be NULL.
// The function returns 'size' on success or zero otherwise.
int GetModuleFileName(void *dummy, char *buf, size_t buf_size);

extern "C" {
  #if (defined(__hpux) ||  (defined (MAINWIN) && defined (sunos5)))
    // stupid hack.  Thanks Mainsoft
       int      wmemcmp(const wchar_t *, const wchar_t *, size_t);
       wchar_t* wmemcpy(wchar_t *, const wchar_t *, size_t);
       wchar_t* wmemmove(wchar_t *, const wchar_t *, size_t);
       wchar_t* wmemset(wchar_t *, wchar_t, size_t);
//       void *wmemchr (const wchar_t *, wchar_t, size_t);
  #endif

 #if defined (linux) || defined (sunos5)
     // already defined
 #else
     wchar_t *wmemchr (const wchar_t *, wchar_t, size_t);
 #endif
};

#endif // UNIX

#endif // _COMPAT_H_

