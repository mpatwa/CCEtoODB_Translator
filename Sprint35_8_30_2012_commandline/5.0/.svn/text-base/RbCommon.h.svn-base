// $Header: /CAMCAD/4.3/RbCommon.h 7     8/12/03 9:06p Kurt Van Ness $

/***************************************************************************
* RBCOMMON.H
*
* (C) Copyright 1997 Rainbow Technologies, Inc. All rights reserved.
*
*     This software contains proprietary information which shall not
*     be reproduced or transferred to other documents and shall not
*     be disclosed to others or used for manufacturing or any other
*     purpose without prior written permission of Rainbow Technologies,
*     Inc.
*
* Description     : Common types and defines used among all product's APIs.
*
* Revision History:
*
* $Version: $
* $Archive: /CAMCAD/4.3/RbCommon.h $
* $Revision: 7 $
* $Header: /CAMCAD/4.3/RbCommon.h 7     8/12/03 9:06p Kurt Van Ness $
* $Author: Kurt Van Ness $
* $Date: 8/12/03 9:06p $
*
* $History: RbCommon.h $
 * 
 * *****************  Version 7  *****************
 * User: Kurt Van Ness Date: 8/12/03    Time: 9:06p
 * Updated in $/CAMCAD/4.3
* 
* *****************  Version 2  *****************
* User: Bgrove       Date: 8/25/97    Time: 9:10a
* Updated in $/Rainbow Script Engine/MISC/SAMPLES/X86/SX/WIN32
 * 
 * *****************  Version 1  *****************
 * User: Bgrove       Date: 8/18/97    Time: 4:48p
 * Created in $/Rainbow Common Project Builds/INCLUDE
 * Rainbow Status Codes
*
****************************************************************************/
#ifndef _RBCOMMON_H
#define _RBCOMMON_H

/* define packet and buffer sizes */
#define RB_DEFAULT_PACKET_SIZE       1028   /* 1024 + sizeof DWORD        */
#define RB_MTRAD_DEFAULT_PACKET_SIZE 2564   /* 2560 + sizeof DWORD        */
#define RB_MTRAD_MAX_READ_SIZE       1024   /* in bytes                   */

/* CFG LIB PARAMS API DEFINES */

/* machine types */
#define RB_MIN_MACH_TYPE          0
#define RB_AUTODETECT_MACHINE     0          /* Autodetect machine type    */
#define RB_IBM_MACHINE            1          /* defines IBM type hw        */
#define RB_NEC_MACHINE            2          /* defines NEC PC-9800 hw     */
#define RB_FMR_MACHINE            3          /* defines Fujitsu FMR hw     */
#define RB_MAX_MACH_TYPE          3

/* OS types */
#define RB_MIN_OS_TYPE            0
#define RB_AUTODETECT_OS_TYPE     0          /* Autodetect OS type         */
#define RB_OS_DOS                 1          /* DOS operating system       */
#define RB_OS_RSRV1               2          /* reserved                   */
#define RB_OS_RSRV2               3          /* reserved                   */
#define RB_OS_WIN3x               4          /* Windows 3.x operating env  */
#define RB_OS_WINNT               5          /* Windows NT operating system*/
#define RB_OS_OS2                 6          /* OS/2 operating system      */
#define RB_OS_WIN95               7          /* Windows 95 operating system*/
#define RB_OS_WIN32s              8          /* Windows WIN32s env         */
#define RB_OS_NW                  9          /* Netware operating system   */
#define RB_OS_QNX                 10
#define RB_MAX_OS_TYPE            9

/* Driver types */
#define RB_DOSRM_LOCAL_DRVR       1          /* DOS Real Mode local driver */
#define RB_WIN3x_LOCAL_DRVR       2          /* Windows 3.x local driver   */
#define RB_WIN32s_LOCAL_DRVR      3          /* Win32s local driver        */
#define RB_WIN3x_SYS_DRVR         4          /* Windows 3.x system driver  */
#define RB_WINNT_SYS_DRVR         5          /* Windows NT system driver   */
#define RB_OS2_SYS_DRVR           6          /* OS/2 system driver         */
#define RB_WIN95_SYS_DRVR         7          /* Windows 95 system driver   */
#define RB_NW_LOCAL_DRVR          8          /* Netware local driver       */
#define RB_QNX_LOCAL_DRVR         9          /* QNX local driver           */

/* Router Flags */
#define RB_ROUTER_USE_LOCAL_DRVR  0x0001     /* use linked in driver       */
#define RB_ROUTER_USE_SYS_DRVR    0x0002     /* use system driver          */
#define RB_ROUTER_AUTODETECT_DRVR (RB_ROUTER_USE_LOCAL_DRVR | \
                                   RB_ROUTER_USE_SYS_DRVR)      
#define RB_MAX_ROUTER_FLAGS       (RB_ROUTER_USE_LOCAL_DRVR | \
                                   RB_ROUTER_USE_SYS_DRVR)

/* Port Params flags */
#define RB_FIRST_LOG_PORT         0          /* first logical port         */
#define RB_LAST_LOG_PORT          3          /* last logical port          */
#define RB_VALIDATE_PORT          0x00000001L/* I/O validate port exsitence*/
#define RB_CONT_HNDLR_INSTALLED   0x00000002L/* OUT    system contention   */
#define RB_USER_DEFINED_PORT      0x00000004L/* OUT    user defined port   */
#define RB_FLAGS_DRVR_DEFINED     0x80000000L/* I/O driver defined flags   */
#define RB_RSRV_PORT_FLAGS        0x7FFFFFF8L/* reserved                   */
#define RB_DEFAULT_PORT_FLAGS     (RB_VALIDATE_PORT | RB_FLAGS_DRVR_DEFINED)
#define RB_USE_AUTOTIMING         0

/* Port types */
#define RB_MIN_PORT_TYPE          0
#define RB_AUTODETECT_PORT_TYPE   0          /* IN   autodetect port type  */
#define RB_NEC_PORT_TYPE          1          /* I/O NEC-PC9800 series port */
#define RB_FMR_PORT_TYPE          2          /* I/O Fujitus FMR series port*/
#define RB_PS2_PORT_TYPE          3          /* I/O IBM/AT/PS2 series port */
#define RB_PS2_DMA_PORT_TYPE      4          /* I/O IBM PS2 DMA series port*/
#define RB_MAX_PORT_TYPE          4

/* Contention Methods (bit mask) */
#define RB_CONT_METH_SYS_ALLOC    0x00000001L/* I/O System port allocation */
#define RB_CONT_METH_NT_RIRQL     0x00000002L/* OUT    NT Raise IRQ level  */
#define RB_CONT_METH_SYS_INT      0x00000004L/* I/O Disable System Ints    */
#define RB_CONT_METH_MASK_INT     0x00000008L/* I/O Mask ints at PIC       */
#define RB_CONT_METH_WIN_CS       0x00000010L/* I/O Windows Critical Sect  */
#define RB_CONT_METH_POLL_HW      0x00000020L/* I/O H/W polling of port    */
#define RB_CONT_METH_RBW          0x00000040L/* I/O Read Before Write      */
#define RB_CONT_METH_DRVR_DEFINED 0x80000000L/* Contention defined by drvr.*/

/* Interrupts to mask (bit mask) */
#define RB_IRQ_MASK_LPT1      0x0001         /* mask LPT1  interrupt       */
#define RB_IRQ_MASK_LPT2      0x0002         /* mask LPT2  interrupt       */
#define RB_IRQ_MASK_TIMER     0x0004         /* mask TIMER interrupt       */
#define RB_IRQ_MAX_MASK       (RB_IRQ_MASK_LPT1 | \
                               RB_IRQ_MASK_LPT2 | \
                               RB_IRQ_MASK_TIMER)
#define RB_IRQ_MASK_DEF       (RB_IRQ_MASK_LPT1 | \
                               RB_IRQ_MASK_TIMER)

/* Define default retry counts and intervals */
#define RB_PORT_CONT_RETRY_CNT_DEF 100       /* 100 retries for port cont  */
#define RB_PORT_CONT_RETRY_INT_DEF 300       /* 300 ms retry interval      */
#define RB_DEV_RETRY_CNT_DEF       100       /* 100 retries for device     */

/* Define the cmd field values for RB_SPRO_LIB_PARAMS  */
#define RB_SET_LIB_PARAMS_CMD      0x0001    /* Set library parameters     */
#define RB_GET_LIB_PARAMS_CMD      0x0002    /* Get library parameters     */

/* define the func field values for RB_SPRO_LIB_PARAMS */
#define RB_MACHINE_TYPE_FUNC       0x0001    /* Set/Get Machine type       */
#define RB_DELAY_FUNC              0x0002    /* Set/Get Delay value        */
#define RB_MASK_INTS_FUNC          0x0003    /* Set/Get Mask interrupts    */
#define RB_ROUTER_FLAGS_FUNC       0x0004    /* Set/Get Router flags       */
#define RB_OS_PARAMS_FUNC          0x0005    /* Set/Get O/S parameters     */
#define RB_PORT_PARAMS_FUNC        0x0006    /* Set/Get Port Parameters    */

RB_STRUCT _RB_OS_PARAMS {
RB_IO  RB_WORD osType;                       /* type of Operating System   */
RB_OUT RB_WORD osVer;                        /* version of Operating System*/
} RB_OS_PARAMS;
typedef RB_OS_PARAMS  RB_PTR RBP_OS_PARAMS;
typedef RB_OS_PARAMS  RB_SP_OS_PARAMS;
typedef RB_OS_PARAMS  RB_PRO_OS_PARAMS;
typedef RB_OS_PARAMS  RB_CPLUS_OS_PARAMS;
typedef RB_OS_PARAMS  RB_SCRIBE_OS_PARAMS;
typedef RB_OS_PARAMS  RB_NCEDIT_OS_PARAMS;
typedef RB_OS_PARAMS  RB_NC_OS_PARAMS;
typedef RB_OS_PARAMS  RB_NPRO_OS_PARAMS;
typedef RBP_OS_PARAMS RBP_SP_OS_PARAMS;
typedef RBP_OS_PARAMS RBP_PRO_OS_PARAMS;
typedef RBP_OS_PARAMS RBP_CPLUS_OS_PARAMS;
typedef RBP_OS_PARAMS RBP_SCRIBE_OS_PARAMS;
typedef RBP_OS_PARAMS RBP_NCEDIT_OS_PARAMS;
typedef RBP_OS_PARAMS RBP_NC_OS_PARAMS;
typedef RBP_OS_PARAMS RBP_NPRO_OS_PARAMS;

RB_STRUCT _RB_PORT_PARAMS {
RB_IO  RB_WORD  logPortNum;                  /* logical port number         */
RB_IO  RB_WORD  sysPortNum;                  /* system  port number         */
RB_IO  RB_WORD  portType;                    /* port type                   */
RB_IO  RB_WORD  phyAddr;                     /* physcial address            */
RB_OUT RB_WORD  mappedAddr;                  /* map address                 */
RB_IO  RB_WORD  deviceRetryCnt;              /* device retry count          */
RB_IO  RB_WORD  contentionRetryCnt;          /* port contention retry count */
RB_IO  RB_WORD  padding1;
RB_IO  RB_DWORD contentionMethod;            /* port contention method      */
RB_IO  RB_DWORD contentionRetryInterval;     /* port contention retry int   */
RB_IO  RB_DWORD flags1;                      /* port flags                  */
} RB_PORT_PARAMS;
typedef RB_PORT_PARAMS RB_PTR RBP_PORT_PARAMS;
typedef RB_PORT_PARAMS  RB_SP_PORT_PARAMS;
typedef RB_PORT_PARAMS  RB_PRO_PORT_PARAMS;
typedef RB_PORT_PARAMS  RB_CPLUS_PORT_PARAMS;
typedef RB_PORT_PARAMS  RB_SCRIBE_PORT_PARAMS;
typedef RB_PORT_PARAMS  RB_NCEDIT_PORT_PARAMS;
typedef RB_PORT_PARAMS  RB_NC_PORT_PARAMS;
typedef RB_PORT_PARAMS  RB_NPRO_PORT_PARAMS;
typedef RBP_PORT_PARAMS RBP_SP_PORT_PARAMS;
typedef RBP_PORT_PARAMS RBP_PRO_PORT_PARAMS;
typedef RBP_PORT_PARAMS RBP_CPLUS_PORT_PARAMS;
typedef RBP_PORT_PARAMS RBP_SCRIBE_PORT_PARAMS;
typedef RBP_PORT_PARAMS RBP_NCEDIT_PORT_PARAMS;
typedef RBP_PORT_PARAMS RBP_NC_PORT_PARAMS;
typedef RBP_PORT_PARAMS RBP_NPRO_PORT_PARAMS;

RB_UNION  _RB_CFG_PARAMS {
RB_IO RB_WORD        machineType;      /* machine type: IBM, NEC, or FMR   */
RB_IO RB_WORD        delay;            /* number of loops for 2us delay    */
RB_IO RB_WORD        maskInterrupts;   /* interrupts to mask               */
RB_IO RB_WORD        routerFlags;      /* request routing flags            */
RB_IO RB_OS_PARAMS   osParams;      /* OS parameters                    */
RB_IO RB_PORT_PARAMS portParams;    /* port parameters                  */
} RB_CFG_PARAMS;
typedef RB_CFG_PARAMS RB_PTR RBP_CFG_PARAMS;
typedef RB_CFG_PARAMS  RB_SP_CFG_PARAMS;
typedef RB_CFG_PARAMS  RB_PRO_CFG_PARAMS;
typedef RB_CFG_PARAMS  RB_CPLUS_CFG_PARAMS;
typedef RB_CFG_PARAMS  RB_SCRIBE_CFG_PARAMS;
typedef RB_CFG_PARAMS  RB_NCEDIT_CFG_PARAMS;
typedef RB_CFG_PARAMS  RB_NC_CFG_PARAMS;
typedef RB_CFG_PARAMS  RB_NPRO_CFG_PARAMS;
typedef RBP_CFG_PARAMS RBP_SP_CFG_PARAMS;
typedef RBP_CFG_PARAMS RBP_PRO_CFG_PARAMS;
typedef RBP_CFG_PARAMS RBP_CPLUS_CFG_PARAMS;
typedef RBP_CFG_PARAMS RBP_SCRIBE_CFG_PARAMS;
typedef RBP_CFG_PARAMS RBP_NCEDIT_CFG_PARAMS;
typedef RBP_CFG_PARAMS RBP_NC_CFG_PARAMS;
typedef RBP_CFG_PARAMS RBP_NPRO_CFG_PARAMS;

RB_STRUCT _RB_LIB_PARAMS {
RB_IN RB_WORD   cmd;                   /* command - set/get parameters     */
RB_IN RB_WORD   func;                  /* function to set/get              */
RB_IO RB_CFG_PARAMS params;
} RB_LIB_PARAMS;
typedef RB_LIB_PARAMS RB_PTR RBP_LIB_PARAMS;
typedef RB_LIB_PARAMS  RB_SPRO_LIB_PARAMS;
typedef RB_LIB_PARAMS  RB_PRO_LIB_PARAMS;
typedef RB_LIB_PARAMS  RB_CPLUS_LIB_PARAMS;
typedef RB_LIB_PARAMS  RB_SCRIBE_LIB_PARAMS;
typedef RB_LIB_PARAMS  RB_NCEDIT_LIB_PARAMS;
typedef RB_LIB_PARAMS  RB_NC_LIB_PARAMS;
typedef RB_LIB_PARAMS  RB_NPRO_LIB_PARAMS;
typedef RBP_LIB_PARAMS RBP_SPRO_LIB_PARAMS;
typedef RBP_LIB_PARAMS RBP_PRO_LIB_PARAMS;
typedef RBP_LIB_PARAMS RBP_CPLUS_LIB_PARAMS;
typedef RBP_LIB_PARAMS RBP_SCRIBE_LIB_PARAMS;
typedef RBP_LIB_PARAMS RBP_NCEDIT_LIB_PARAMS;
typedef RBP_LIB_PARAMS RBP_NC_LIB_PARAMS;
typedef RBP_LIB_PARAMS RBP_NPRO_LIB_PARAMS;

/* END CFG LIB PARAMS API DEFINES */

/* define a unit info structure */
typedef struct _RB_UNIT_INFO {
RB_WORD developerID;
RB_WORD serialNum;
RB_WORD port;
RB_BYTE reserved[18];
} RB_UNIT_INFO;
typedef RB_UNIT_INFO RB_PTR RBP_UNIT_INFO;

#endif
/* end of file */
