/******************************* BBSGRAPH.H ********************************/
/************************* Display macros **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*************** These macros emulate C Graphics routines ******************/

#ifndef BBSGRAPH
#define BBSGRAPH
#define QGLGRAPH

#ifdef  __MCRGRAPH__
#include <graph.h>
#endif
#ifdef  __TURBOC__
#ifndef _Windows
#include <graphics.h>
#else
#define __WINDOWS__
#endif // _Windows
#endif
#ifdef  __GFX__
#ifdef  PI
#define BBS_PI  PI
#undef  PI
#endif
#include <gfx.h>
#ifndef __HIGHC24__
#include <gfxg_pro.h>
#endif
#ifdef  PI
#undef  PI
#define PI  BBS_PI
#endif
#endif  /*GFX*/

#endif  /*BBSGRAPH*/

