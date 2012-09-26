/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SYSUTILS_H__
#define __SYSUTILS_H__

#ifndef NULL
# define NULL 0
#endif

#ifndef TRUE
# define TRUE 1
#endif

#ifndef FALSE
# define FALSE 0
#endif

#define SCL_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define SCL_MAX(a, b) (((a) > (b)) ? (a) : (b))

#ifndef ABS
# define ABS(x) ((x) < 0 ? -(x) : (x))
#endif

// SQRT2
#if !defined(_SQRT2)
# define _SQRT2   1.414213562373095048801688724209698078569671875376948
#endif

// PI
#if !defined(_PI)
# define _PI 3.14159265358979323846264338327950288419716939937510
#endif

#define COUNTOF(a) (sizeof(a)/sizeof(*a))

inline int iround(double x)
{
	return int(x > 0 ? x + 0.5 : x - 0.5);
}

#ifdef _DEBUG
# define UNUSED(x)
#else
# define UNUSED(x) x
#endif

typedef char * PTSTR;
typedef const char * PCTSTR;

#endif // __SYSUTILS_H__

