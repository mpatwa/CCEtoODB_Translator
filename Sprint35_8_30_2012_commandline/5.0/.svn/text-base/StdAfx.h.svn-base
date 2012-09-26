// $Header: /CAMCAD/5.0/StdAfx.h 34    5/22/07 6:33p Rick Faltersack $

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define WINVER 0x0502 // Change this to the appropriate value to target Windows 98 and Windows 2000 or later.

#define _WIN32_WINNT 0x502 // For NT 4.0, and not taking advantage of XP/2000 benefits.

#define _WIN32_WINDOWS 0x0510 // Change this to the appropriate value to target Windows Me or later.

//#define _CRTDBG_MAP_ALLOC //Turns on the Debug C RT

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#include <afxcmn.h>
#include <tipdlg.h>
#include <afxdlgs.h>
#include <afxdhtml.h>

////Should be last
//#include <CRTDBG.h>
//
//#ifdef _DEBUG
//#ifndef NEW_INLINE_WORKAROUND
//#define NEW_INLINE_WORKAROUND new (_NORMAL_BLOCK, __FILE__, __LINE__)
//#define new NEW_INLINE_WORKAROUND
//#endif
//#endif //_DEBUG

extern char *shapes[];
extern char *devicetypes[];

extern int MAX_COORD; // limit for range of logical page units because of MAX_INT
#define MAX_16BIT    16000
#define MAX_32BIT    20000000 // 2,147,483,647 / 100

extern int Platform; // Platform ID - VER_PLATFORM_WIN32s, ..WIN32_WINDOWS, ..WIN32_NT
#define WIN32S    0 // VER_PLATFORM_WIN32s             0   // Win32s on Windows 3.1. 
#define WIN9x     1 // VER_PLATFORM_WIN32_WINDOWS      1   // Windows 95, Windows 98, or Windows Me
#define WINNT     2 // VER_PLATFORM_WIN32_NT           2   // Windows NT 3.51, Windows NT 4.0, Windows 2000, or Whistler (XP)

extern int Product; 

#define Pi (3.141592653589793)
#define PiOver2 (Pi/2.)
#define PiOver4 (Pi/4.)
#define PiOver8 (Pi/8.)
#define TwoPi (Pi + Pi)
#define radiansToDegrees(a) ((a)*(180./Pi))
#define degreesToRadians(a) ((a)*(Pi/180.))

#define SqrtOf2        (1.4142135623730950488016887242097)
#define SqrtOf2Over2   (.70710678118654752440084436210485)
#define SqrtOf3        (1.7320508075688772935274463415059)
#define TwoOverSqrtOf3 (1.1547005383792515290182975610039)

// Version Specific
#define EnableGerberEducator
#define CamCadMajorVersion    4
#define CamCadMinorVersion    11

#define EnableDcaCamCadDocLegacyCode

//Compatiablity with VS2008
#define STRDUP                         _strdup
#define ECVT                           _ecvt
#define HYPOT                          _hypot
#define STRCMPI                        _strcmpi  
#define STRICMP                        _stricmp  
#define STRNICMP                       _strnicmp  
#define STRREV                         _strrev
#define STRSET                         _strset   
#define UNLINK                         _unlink   
#define ITOA                           _itoa 
#define STRUPR                         _strupr
#define STRLWR                         _strlwr

// For CCE encryption/decryption support.
#define LIBXML_STATIC
#define LIBXSLT_STATIC
#define XMLSEC_STATIC

// end STDAFX.H   
