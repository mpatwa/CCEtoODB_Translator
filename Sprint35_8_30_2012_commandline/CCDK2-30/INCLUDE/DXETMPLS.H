/****************************** DXETMPLS.H **********************************/ 
/*****************  Include file for DXF file manipulation  ****************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!       (C) Copyright 1989 - 1995       Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#ifndef     DXETMPLS 
#define     DXETMPLS 

#define DX_DESC_TEMPLATE DXF_PUT0_CODE, 8, 5, DXF_DEF0_CODE, \
6, 38, 39, 62, 210, 220, 230, DXF_NODEF_CODE

#ifdef  __ACAD11__
#include <dxetmpl1.h>
#else
#include <dxetmpl0.h>
#endif  /* __ACAD11__ */
#endif

