/****************************** DXDEFS.H ***********************************/ 
/*****************  Include file for DXF file manipulation  ****************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!       (C) Copyright 1989 - 1995       Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#ifndef     DXDEFS 
#define     DXDEFS 

#include <bbsdefs.h>
#include <dmdefs.h>
EXTERN      INT             DX_ENTS_NUMBER ;
typedef     long            DXF_ENAME[2] ;
#ifdef __UNIX__
typedef     INT             DX_INT_FILLER ;
#endif
#ifdef __MSC32__
typedef     INT             DX_INT_FILLER ;
#endif

#define     DXF_ENAME_INIT(N)   {(N)[0]= -1l;(N)[1]= -1l;}
#define     DXF_INIT_ENAME(N)   (((N)[0]== -1l)&&((N)[1]== -1l))
#define     DX_BINARY_SENTINEL  "AutoCAD Binary DXF\r\n\032"
#include    <dxstring.h>
#define     DX_DEFAULT_PRECISION        6
#define     DXF_HUGE                    1e+18
#define     DXF_HUGE_POS_WORD           "1.000000E+20"
#define     DXF_HUGE_NEG_WORD           "-1.000000E+20"

#define     DXF_BAD_CODE                -1001
#define     DXF_END_CODE                -1002
#define     DXF_ANY_CODE                -1003
#define     DXF_ARRAY_COUNT_CODE        -1004
#define     DXF_ARRAY_CODE              -1005
#define     DXF_DEF0_CODE               -1006
#define     DXF_DEF1_CODE               -1007
#define     DXF_NODEF_CODE              -1008
#define     DXF_PUT0_CODE               -1009
#define     DXF_STANDARD_CODE           -1010
#define     DXF_ENAME_CODE              -1011
#define     DXF_PUT210_CODE             -1012
#define     DXF_PUTADX_CODE             -1013
#define     DXF_DEFNEG1_CODE            -1014
#ifdef __ACAD11__
#define     DXF_MULT_CODE               -1015
#define     DXF_LIST_CODE               -1016
#endif
#define     DX_INT_FILLER_CODE          -1017 
#define     DXF_HNDL_ENABLED            -1018

#define     DX_HANDLE_SIZE      32
#define     DX_TXT_STRING_SIZE  512
#define     DX_BUFFER_SIZE      512
#define     DX_1000_SIZE        DX_STRING_SIZE
/*
#define     DX_COLOR_BYLAYER    0x7fff
*/
#define     DXF_COLOR_BYENT     0
#define     DXF_LTYPE_BYENT     0
#define     DXF_COLOR_BYLAYER   1
#define     DXF_LTYPE_BYLAYER   1

typedef     char  DX_BUFFER[DX_BUFFER_SIZE] ;
typedef     char  DX_TXT_STRING[DX_TXT_STRING_SIZE] ;
typedef     char  DX_NAME[20] ;

#define DXF_FILEPOS_ENUM { DXF_FIRST_ENTITY, DXF_FIRST_BLOCK, \
        DXF_FIRST_TABLE_ENTRY, DXF_RESTORE_POSITION, DXF_HEADER } 
#ifdef __cplusplus
enum DXF_FILEPOS_ENUM ;
typedef char DXF_FILEPOS_MODE ;
#else
typedef enum DXF_FILEPOS_ENUM DXF_FILEPOS_MODE ;
#endif

#include    <dxfild.h>
#include    <dxhdvrd.h>
/*
#include    <dxhdvrm.h>
*/
#include    <dxentx.h>
#include    <dxtblx.h>
#ifdef __ACAD11__
#include    <dxapdefd.h>
#include    <dxapdatd.h>
#endif      /* __ACAD11__ */
#endif     /*DXDEFS*/

