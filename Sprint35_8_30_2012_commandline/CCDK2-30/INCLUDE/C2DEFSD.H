/******************************* C2DEFSD.H *********************************/
/*************** Include file for 2d definitions and macros ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     C2DEFSD
#define     C2DEFSD
#include    <c2cxtrd.h>
#ifdef SPLINE
typedef     struct          c2_nurb {
            HPT2            *ctl_pt ;
            REAL            *knot ;
            INT             index ;
            C2_CURV_EXTR    curv_extr_array ;
            REAL            offset ;
            INT             nctpts ;
            INT             order ;
            INT             curv_extr_no ;
            } C2_NURB_S ;
typedef     C2_NURB_S    *C2_NURB ;
#endif    /*SPLINE*/

typedef     struct          c2_line {
            PT2             pt[2] ;
            } C2_LINE_S ;
typedef     C2_LINE_S    *C2_LINE ;

EXTERN  INT                 C2_LINE_BUF_SIZE ;
EXTERN  INT                 C2_LINES_NUMBER ;
EXTERN  C2_LINE             *C2_LINE_BUFFER ;

typedef     struct          c2_aseg {
            PT2             pt0 ;
            REAL            d ;
            PT2             pt1 ;
            } C2_ASEG_S ;
typedef     C2_ASEG_S      *C2_ASEG ;

typedef     struct          c2_arc {
            C2_ASEG_S       arcseg ;
            BOOLEAN         zero_rad ;
            } C2_ARC_S ;
typedef     C2_ARC_S        *C2_ARC ;

typedef     struct          c2_zero_arc {
            PT2             ctr ;
            PT2             tan0 ;
            PT2             tan1 ;
            } C2_ZERO_ARC_S ;
typedef     C2_ZERO_ARC_S   *C2_ZERO_ARC ;

EXTERN  INT                 C2_ARC_BUF_SIZE ;
EXTERN  INT                 C2_ARCS_NUMBER ;
EXTERN  C2_ARC              *C2_ARC_BUFFER ;

#define     C2_PC_BUFFER_MAXSIZE     100
typedef     struct          c2_pc_buffer {
            REAL            *a ;
            INT             n ;
            INT             a_size ;
            } C2_PC_BUFFER_S ;
typedef     C2_PC_BUFFER_S     *C2_PC_BUFFER ;

typedef     struct          c2_pcurve {
            C2_PC_BUFFER_S  buffer ;
            DML_LIST        buffer_list ;
            } C2_PCURVE_S ;
typedef     C2_PCURVE_S     *C2_PCURVE ;
#endif

