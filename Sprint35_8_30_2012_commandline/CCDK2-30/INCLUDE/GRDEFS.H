/******************************* GRDEFS.H **********************************/
/************************ Graphics definitions *****************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef GRDEFS
#define GRDEFS

#include <bbsdefs.h>
#include <bbsgraph.h>


/********************************* DATA TYPES ********************************/
#ifdef  __MCRGRAPH__
typedef     short int       GR_COLOR ;
typedef     unsigned short  GR_PATT ;
typedef     short int       GR_PIX_COORD ;
#endif

#ifdef  __WINDOWS__
typedef     short int       GR_COLOR ;
typedef     unsigned short  GR_PATT ;
typedef     short int       GR_PIX_COORD ;
#endif

#ifdef  __TURBOC__
#ifndef __WINDOWS__
typedef     int             GR_COLOR ;
typedef     unsigned int    GR_PATT ;
typedef     int             GR_PIX_COORD ;
#endif // __WINDOWS__
#endif

#ifdef  __GFX__
typedef     int             GR_COLOR ;
typedef     unsigned int    GR_PATT ;
typedef     int             GR_PIX_COORD ;
#endif

#ifdef  __UNIX__
typedef     int             GR_COLOR ;
typedef     unsigned int    GR_PATT ;
typedef     int             GR_PIX_COORD ;
#endif

/******************************* COLORS *************************************/
#define  GR_BLACK          0
#define  GR_BLUE           1
#define  GR_GREEN          2
#define  GR_CYAN           3
#define  GR_RED            4
#define  GR_MAGENTA        5
#define  GR_BROWN          6
#define  GR_LIGHTGRAY      7
#define  GR_DARKGRAY       8
#define  GR_LIGHTBLUE      9
#define  GR_LIGHTGREEN     10
#define  GR_LIGHTCYAN      11
#define  GR_LIGHTRED       12
#define  GR_LIGHTMAGENTA   13
#define  GR_LIGHTYELLOW    14
#define  GR_YELLOW         GR_LIGHTYELLOW
#define  GR_WHITE          15

/****************************** PATTERNS ************************************/
#define GR_SOLID_PATT   0xffff
#define GR_DASHED_PATT  0xc3c3

EXTERN      REAL        GR_SCREEN_RATIO ;
/********************************* CONSTANTS ********************************/
#define GR_MARGIN           1
#define GR_INT_ROUNDOFF     0.5

typedef     GR_PIX_COORD    GR_PIXEL[2] ;

/***************************** VIDEMODES ************************************/
enum video_mode { GR_NOVMODE, GR_DETECT, GR_VGA16, GR_EGA, GR_VGA2, GR_EGA0, 
        GR_HRES16, GR_HERC, GR_HRES0 } ;
#endif /* GRDEFS */

