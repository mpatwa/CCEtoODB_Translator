/****************************** DXENTX.H ***********************************/
/*****************  Include file for DXF file manipulation  ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!       (C) Copyright 1989 - 1995       Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     DXENTX 
#define     DXENTX 

#include <bbsdefs.h>
#include <dmdefs.h>

#include    <dxdesd.h>
#include    <dxdesm.h>
#include    <dxhdrd.h>
#include    <dxhdrm.h>

#include    <dxblpd.h>
#include    <dxblpm.h>
#include    <dxblcd.h>
#include    <dxblcm.h>

#ifdef DX_ENT_ON
#include    <dxentxd.h>
#endif      /* DX_ENT_ON */
#include    <dxentid.h>
#include    <dxentd.h>
#include    <dxfltd.h>
#include    <dxfltm.h>
#define     DXF_ANY_COLOR   (INT)15
#include    <dxentm.h>
#ifdef DX_ENT_ON
#include    <dxentxm.h>
#endif  /* DX_ENT_ON */
EXTERN      DXF_ENTITY      DX_ENT_BUFFER[] ;
#endif

