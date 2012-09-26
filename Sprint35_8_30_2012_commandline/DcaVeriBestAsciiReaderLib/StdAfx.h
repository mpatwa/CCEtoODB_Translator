// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components

// TODO: reference additional headers your program requires here

#define Pi (3.141592653589793)
#define PiOver2 (Pi/2.)
#define PiOver4 (Pi/4.)
#define PiOver8 (Pi/8.)
#define TwoPi (Pi + Pi)
#define radiansToDegrees(a) ((a)*(180./Pi))
#define degreesToRadians(a) ((a)*(Pi/180.))

//Compatiablity with VS2008
#define STRDUP                         _strdup
#define ECVT                           _ecvt
#define HYPOT                          _hypot
#define STRCMPI                        _strcmpi  
#define STRICMP                        _stricmp  
#define STRNICMP                       _strnicmp  
#define STRREV                         _strrev