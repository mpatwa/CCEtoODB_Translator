// $Header: /CAMCAD/4.3/RbTypes.h 7     8/12/03 9:06p Kurt Van Ness $

/***************************************************************************
* RBTYPES.H
*
* (C) Copyright 1996 Rainbow Technologies, Inc. All rights reserved.
*
* Description - Defines types used in Rainbow code for Microsoft C/C++
*               compilers.
*
****************************************************************************/
#ifndef _RBTYPES_H
#define _RBTYPES_H

#ifndef _MSC_VER
#error "This include file is only for Microsoft C/C++ compilers!"
#endif
/***************************************************************************
* DEFINE THE TARGETED PLATFORM
****************************************************************************/
#if (!(defined(_WIN32_) || defined(WIN32) || defined(_WIN32)) && !defined(_WINDOWS))
#define _DOS_   1
#define _16BIT_ 1
#elif (defined(_WINDOWS) && !(defined(_WIN32_) || defined(_WIN32) || defined(WIN32)))
#define _WIN_   1
#define _16BIT_ 1
#elif (defined(_WIN32_) || defined(WIN32) || defined(_WIN32))
#define _WIN32_ 1
#define _32BIT_ 1
#endif
/***************************************************************************/

/****************************************************************************
* SETTINGS FOR MICROSOFT C
****************************************************************************/
#if  defined(_WIN32_)
#define RB_EXPAPI   __export
#define RB_ASM      __asm
#define RB_STDCALL  __stdcall
#define RB_FASTCALL __fastcall
#define RB_PASCAL
#define RB_CDECL    __cdecl
#define RB_FAR
#define RB_NEAR
#define RB_HUGE
#elif defined(_OS2_)
#define RB_EXPAPI
#define RB_ASM      _asm
#define RB_LOADDS   _loadds
#define RB_STDCALL
#define RB_FASTCALL _fastcall
#define RB_PASCAL   _pascal
#define RB_CDECL    _cdecl
#define RB_FAR      _far
#define RB_NEAR     _near
#define RB_HUGE     _huge
#elif defined(_WIN_)
#define RB_EXPAPI   _export
#define RB_ASM      _asm
#define RB_LOADDS   _loadds
#define RB_STDCALL
#define RB_FASTCALL _fastcall
#define RB_PASCAL   _pascal
#define RB_CDECL    _cdecl
#define RB_FAR      _far
#define RB_NEAR     _near
#define RB_HUGE     _huge
#elif defined(_DOS_)
#if (_MSC_VER <= 7)
#define RB_EXPAPI
#define RB_ASM      _asm
#define RB_LOADDS   _loadds
#define RB_STDCALL
#define RB_FASTCALL _fastcall
#define RB_PASCAL   _pascal
#define RB_CDECL    _cdecl
#define RB_FAR      _far
#define RB_NEAR     _near
#define RB_HUGE     _huge
#else
#define RB_EXPAPI
#define RB_ASM      __asm
#define RB_STDCALL  __stdcall
#define RB_FASTCALL __fastcall
#define RB_PASCAL
#define RB_CDECL    __cdecl
#define RB_FAR
#define RB_NEAR
#define RB_HUGE
#endif /* _MSC_VER */
#endif /* _WIN32_  */
/***************************************************************************/

/***************************************************************************
* DEFINE POINTER TYPES
****************************************************************************/
#if (defined(_DOS_) && defined(_16BIT_))
#define RB_PTR     RB_FAR *
#elif (defined(_DOS_) && defined(_32BIT_))
#define RB_PTR     *
#elif defined(_WIN_)
#define RB_PTR     RB_FAR *
#elif defined(_WIN32_)
#define RB_PTR     *
#elif defined(_OS2_)
#define RB_PTR     *
#endif
/***************************************************************************/
#define RB_IN
#define RB_OUT
#define RB_IO
#define RB_STRUCT typedef struct
#define RB_UNION  typedef union

#ifdef __cplusplus
#define RB_EXPORT extern "C"
#else
#define RB_EXPORT extern
#endif /* __cplusplus */
#define RB_LOCAL  static

/* define RNBO types */
typedef                void RB_VOID;
typedef                char RB_CHAR;
typedef          short int  RB_SHORT;
typedef          long  int  RB_LONG;
typedef unsigned       char RB_BOOLEAN;
typedef unsigned       char RB_BYTE;
typedef unsigned       char RB_UCHAR;
typedef unsigned short int  RB_USHORT;
typedef unsigned short int  RB_WORD;
typedef unsigned long  int  RB_ULONG;
typedef unsigned long  int  RB_DWORD;
typedef RB_VOID    RB_PTR   RBP_VOID;
typedef RB_CHAR    RB_PTR   RBP_CHAR;
typedef RB_SHORT   RB_PTR   RBP_SHORT;
typedef RB_LONG    RB_PTR   RBP_LONG;
typedef RB_BOOLEAN RB_PTR   RBP_BOOLEAN;
typedef RB_BYTE    RB_PTR   RBP_BYTE;
typedef RB_UCHAR   RB_PTR   RBP_UCHAR;
typedef RB_USHORT  RB_PTR   RBP_USHORT;
typedef RB_WORD    RB_PTR   RBP_WORD;
typedef RB_ULONG   RB_PTR   RBP_ULONG;
typedef RB_DWORD   RB_PTR   RBP_DWORD;

/* define macros */
#define RB_ROR(x)            ((x >> 1) | (x << ((sizeof(x)*8)-1)))
#define RB_ROL(x)            ((x << 1) | (x >> ((sizeof(x)*8)-1)))
#define RB_HI_BIT(w)         ((RB_WORD)((RB_WORD)(w) & 0x8000))
#define RB_LO_BIT(w)         ((RB_WORD)((RB_WORD)(w) & 0x0001))
#define RB_MAKE_WORD(lo,hi)  ((RB_WORD)(((RB_BYTE)(lo))  | (((RB_WORD)((RB_BYTE)(hi))) << 8)))
#define RB_MAKE_DWORD(lo,hi) ((RB_DWORD)(((RB_WORD)(lo)) | (((RB_DWORD)((RB_WORD)(hi))) << 16)))
#define RB_HI_WORD(l)        ((RB_WORD)(((RB_DWORD)(l) >> 16) & 0x0000FFFF))
#define RB_LO_WORD(l)        ((RB_WORD)((RB_DWORD)l & 0x0000FFFF))
#define RB_HI_BYTE(s)        ((RB_BYTE)(((RB_WORD)(s) >> 8) & 0x00FF))
#define RB_LO_BYTE(s)        ((RB_BYTE)((RB_WORD)s & 0x00FF))
#define RB_MIN(x,y)          ((x) < (y) ? (x) : (y))
#define RB_MAX(x,y)          ((x) > (y) ? (x) : (y))
#endif  /* _RBTYPES_H */
/* end of file */
