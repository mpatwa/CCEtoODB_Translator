// $Header: /CAMCAD/4.5/RbStatus.h 10    11/01/05 2:38p Rick Faltersack $

/***************************************************************************
* RBSTATUS.H
*
* (C) Copyright 1997 Rainbow Technologies, Inc. All rights reserved.
*
*     This software contains proprietary information which shall not
*     be reproduced or transferred to other documents and shall not
*     be disclosed to others or used for manufacturing or any other
*     purpose without prior written permission of Rainbow Technologies,
*     Inc.
*
* Description: Defines Rainbow status codes.
*
* Revision History:
*
* $Version: $
* $Archive: /CAMCAD/4.5/RbStatus.h $
* $Revision: 10 $
* $Header: /CAMCAD/4.5/RbStatus.h 10    11/01/05 2:38p Rick Faltersack $
* $Author: Rick Faltersack $
* $Date: 11/01/05 2:38p $
*
* $History: RbStatus.h $
 * 
 * *****************  Version 10  *****************
 * User: Rick Faltersack Date: 11/01/05   Time: 2:38p
 * Updated in $/CAMCAD/4.5
 * Version = "4.5.1294";	// 11/01/05 - No case - AdipltIn.cpp,
 * CadpltIn.cpp, GenCadIn.cpp - Fixed 6 messages containing "unkown"
 * (unknown) - rcf
 * 	
 * 
 * *****************  Version 7  *****************
 * User: Kurt Van Ness Date: 8/12/03    Time: 9:06p
 * Updated in $/CAMCAD/4.3
* 
* *****************  Version 6  *****************
* User: Bgrove       Date: 8/25/97    Time: 9:10a
* Updated in $/Rainbow Script Engine/MISC/SAMPLES/X86/SX/WIN32
* 
* *****************  Version 5  *****************
* User: Bgrove       Date: 8/25/97    Time: 9:10a
* Updated in $/Rainbow Script Engine/MISC/SAMPLES/X86/SX/WIN16
* 
* *****************  Version 4  *****************
* User: Bgrove       Date: 8/25/97    Time: 9:10a
* Updated in $/Rainbow Script Engine/MISC/SAMPLES/X86/SX/DOSRM
* 
* *****************  Version 3  *****************
* User: Bgrove       Date: 8/25/97    Time: 9:10a
* Updated in $/Rainbow Script Engine/MISC/SAMPLES/X86/SX/DOS32X
* 
* *****************  Version 2  *****************
* User: Bgrove       Date: 8/25/97    Time: 9:10a
* Updated in $/Rainbow Script Engine/MISC/SAMPLES/X86/SX/DOS16X
 * 
 * *****************  Version 1  *****************
 * User: Bgrove       Date: 8/18/97    Time: 4:48p
 * Created in $/Rainbow Common Project Builds/INCLUDE
 * Rainbow Status Codes
*
*
****************************************************************************/
#ifndef _RBSTATUS_H
#define _RBSTATUS_H

/* status base codes */
#define DRVR_DISP_STATUS_BASE           0x0100 /* drvr dispatch   */
#define NT_PAR_IO_STATUS_BASE           0x0200 /* NT I/O handler  */
#define ROUTER_STATUS_BASE              0x0300 /* router          */
#define SPRO_STATUS_BASE                0x0400 /* SPRO drvr       */
#define PRO_STATUS_BASE                 0x0500 /* PRO  drvr       */
#define WIN_PAR_IO_STATUS_BASE          0x0600 /* WIN I/O handler */
#define CPLUS_STATUS_BASE               0x0700 /* C+   drvr       */
#define OS2_PAR_IO_STATUS_BASE          0x0800 /* OS2 I/O handler */
#define DOS_PAR_IO_STATUS_BASE          0x0900 /* DOS I/O handler */
#define VDD_PAR_IO_STATUS_BASE          0x0A00 /* VDD I/O handler */
#define SPRO_CL_STATUS_BASE             0x0B00 /* SPRO client lib */
#define PRO_CL_STATUS_BASE              0x0C00 /* PRO  client lib */
#define CPLUS_CL_STATUS_BASE            0x0D00 /* C+ client lib   */
#define SCRIBE_STATUS_BASE              0x0E00 /* SCRIBE drvr     */
#define SCRIBE_CL_STATUS_BASE           0x0F00 /* SCRIBE client   */
#define NC_STATUS_BASE                  0x1000 /* NETC drvr       */
#define NPRO_STATUS_BASE                0x1100 /* NETPRO drvr     */
#define NC_CL_STATUS_BASE               0x1200 /* NETC client lib */
#define NPRO_CL_STATUS_BASE             0x1300 /* NETPRO client   */
#define MPTRAD_STATUS_BASE              0x1400 /* MPHAR drvr      */
#define MPTRAD_CL_STATUS_BASE           0x1500 /* MPHAR client lib*/
#define DUBLIN_STATUS_BASE              0x1600 /* DUBLIN drvr     */
#define DUBLIN_CL_STATUS_BASE           0x1700 /* DUBLIN client   */
#define TIO_PAR_IO_STATUS_BASE          0x1800 /* TIO I/O handler */
#define REGMOD_PAR_IO_STATUS_BASE       0x1900 /* REGMOD I/O      */
#define INT2F_PAR_IO_STATUS_BASE        0x1A00 /* INT2F I/O       */
#define NCEDIT_CL_STATUS_BASE           0x1B00 /* NCEDIT client   */
#define WIN95_PAR_IO_STATUS_BASE        0x1C00 /* W95 I/O handler */

/* status codes */
#define RB_SUCCESS                      0
#define RB_INVALID_FUNCTION_CODE        1
#define RB_INVALID_PACKET               2
#define RB_UNIT_NOT_FOUND               3
#define RB_ACCESS_DENIED                4
#define RB_INVALID_MEMORY_ADDRESS       5
#define RB_INVALID_ACCESS_CODE          6
#define RB_PORT_IS_BUSY                 7
#define RB_WRITE_NOT_READY              8
#define RB_NO_PORT_INSTALLED            9
#define RB_ALREADY_ZERO                 10
#define RB_FAIL_ON_DRIVER_OPEN          11
#define RB_DRIVER_NOT_INSTALLED         12
#define RB_COMMUNICATIONS_ERROR         13
#define RB_OS_CHANGED                   14
#define RB_PACKET_TOO_SMALL             15
#define RB_INVALID_PARAMETER            16
#define RB_MEM_ACCESS_ERROR             17
#define RB_VERSION_NOT_SUPPORTED        18
#define RB_OS_ENV_NOT_SUPPORTED         19
#define RB_QUERY_TOO_LONG               20
#define RB_INVALID_COMMAND              21
#define RB_INVALID_PRODUCT              22
#define RB_INVALID_PORT_NUM             23
#define RB_INVALID_CONTROLLER           24
#define RB_OS_ENV_UNDEFINED             25
#define RB_FAIL_ON_DRIVER_READ          26
#define RB_FAIL_ON_DRIVER_WRITE         27
#define RB_FAIL_ON_DRIVER_IOCTL         28
#define RB_MEM_ALIGNMENT_ERROR          29
#define RB_DRIVER_IS_BUSY               30
#define RB_PORT_ALLOCATION_FAILURE      31
#define RB_PORT_RELEASE_FAILURE         32
#define RB_DUPLICATE                    33
#define RB_EVENT_INTERRUPTED            34
#define RB_EVENT_SIGNALED               35
#define RB_EVENT_TIMEOUT                36
#define RB_SLEEP_FAILED                 37
#define RB_SEM_REQ_TIMEOUT              38
#define RB_AQUIRE_PORT_TIMEOUT          39
#define RB_DEVICE_TIMEOUT               40
#define RB_PORT_CONT_HDLR_NOT_INSTALLED 41
#define RB_SIGNAL_NOT_SUPPORTED         42
#define RB_SEM_INIT_ERROR               43
#define RB_UNKNOWN_MACHINE              44
#define RB_SYS_API_ERROR                45
#define RB_UNIT_IS_BUSY                 46
#define RB_INVALID_PORT_TYPE            47
#define RB_INVALID_MACH_TYPE            48
#define RB_INVALID_IRQ_MASK             49
#define RB_INVALID_CONT_METHOD          50
#define RB_INVALID_PORT_FLAGS           51
#define RB_INVALID_LOG_PORT_CFG         52
#define RB_INVALID_OS_TYPE              53
#define RB_INVALID_LOG_PORT_NUM         54
#define RB_INVALID_CMD_CODE             55
#define RB_INVALID_ROUTER_FLGS          56
#define RB_INIT_NOT_CALLED              57
#define RB_DRVR_TYPE_NOT_SUPPORTED      58
#define RB_FAIL_ON_DRIVER_COMM          59
#define RB_STATUS_API_UNAVAILABLE       60
#define RB_WRITE_LOW_POWER_ERROR        61
#define RB_WRITE_MAY_BE_CORRUPTED       62
#define RB_DEVICE_ACCESS_FAILED         63
#define RB_FORMAT_NOT_CALLED            64
#define RB_KEY_CORRUPTED                65
#define RB_INVALID_STATUS               255

#define RB_STATUS_CODE_MASK             0x00FF
#define RB_STATUS_TYPE_MASK             0xFF00

#define SET_RB_STATUS(base,status) (RB_STATUS)(base | status)

#define RB_STATUS_ROUTER_CRITICAL_ERR(theStatus) \
    (((theStatus) & RB_STATUS_CODE_MASK) == RB_DRIVER_NOT_INSTALLED    || \
     ((theStatus) & RB_STATUS_CODE_MASK) == RB_VERSION_NOT_SUPPORTED   || \
     ((theStatus) & RB_STATUS_CODE_MASK) == RB_DRVR_TYPE_NOT_SUPPORTED || \
     ((theStatus) & RB_STATUS_CODE_MASK) == RB_FAIL_ON_DRIVER_COMM     || \
     ((theStatus) & RB_STATUS_CODE_MASK) == RB_OS_ENV_NOT_SUPPORTED)

#define RB_STATUS_COMM_ERR(theStatus) \
   (((theStatus) & RB_STATUS_CODE_MASK) == RB_COMMUNICATIONS_ERROR)

#define RB_STATUS_SUCCESS(theStatus) \
   (((theStatus) & RB_STATUS_CODE_MASK) == RB_SUCCESS)

/* product specific status codes */

/**  Pro API error codes.  **/
#define SENTPRO_SUCCESS                      RB_SUCCESS
#define SENTPRO_INVALID_FUNCTION             RB_INVALID_FUNCTION_CODE
#define SENTPRO_INVALID_PACKET               RB_INVALID_PACKET
#define SENTPRO_UNIT_NOT_FOUND               RB_UNIT_NOT_FOUND
#define SENTPRO_ACCESS_DENIED                RB_ACCESS_DENIED
#define SENTPRO_PORT_IS_BUSY                 RB_PORT_IS_BUSY
#define SENTPRO_NO_PORT_FOUND                RB_NO_PORT_INSTALLED
#define SENTPRO_DRIVER_OPEN_ERROR            RB_FAIL_ON_DRIVER_OPEN
#define SENTPRO_DRIVER_NOT_INSTALLED         RB_DRIVER_NOT_INSTALLED
#define SENTPRO_IO_COMMUNICATIONS_ERROR      RB_COMMUNICATIONS_ERROR
#define SENTPRO_PACKET_TOO_SMALL             RB_PACKET_TOO_SMALL
#define SENTPRO_INVALID_PARAMETER            RB_INVALID_PARAMETER
#define SENTPRO_MEM_ACCESS_ERROR             RB_MEM_ACCESS_ERROR
#define SENTPRO_VERSION_NOT_SUPPORTED        RB_VERSION_NOT_SUPPORTED
#define SENTPRO_OS_NOT_SUPPORTED             RB_OS_ENV_NOT_SUPPORTED
#define SENTPRO_QUERY_TOO_LONG               RB_QUERY_TOO_LONG
#define SENTPRO_INVALID_COMMAND              RB_INVALID_COMMAND
#define SENTPRO_MEM_ALIGNMENT_ERROR          RB_MEM_ALIGNMENT_ERROR
#define SENTPRO_DRIVER_IS_BUSY               RB_DRIVER_IS_BUSY
#define SENTPRO_PORT_ALLOCATION_FAILURE      RB_PORT_ALLOCATION_FAILURE
#define SENTPRO_PORT_RELEASE_FAILURE         RB_PORT_RELEASE_FAILURE
#define SENTPRO_ACQUIRE_PORT_TIMEOUT         RB_AQUIRE_PORT_TIMEOUT
#define SENTPRO_SIGNAL_NOT_SUPPORTED         RB_SIGNAL_NOT_SUPPORTED
#define SENTPRO_UNKNOWN_MACHINE              RB_UNKNOWN_MACHINE
#define SENTPRO_SYS_API_ERROR                RB_SYS_API_ERROR
#define SENTPRO_INVALID_PORT_TYPE            RB_INVALID_PORT_TYPE
#define SENTPRO_INVALID_MACH_TYPE            RB_INVALID_MACH_TYPE
#define SENTPRO_INVALID_IRQ_MASK             RB_INVALID_IRQ_MASK
#define SENTPRO_INVALID_CONT_METHOD          RB_INVALID_CONT_METHOD
#define SENTPRO_INVALID_PORT_FLAGS           RB_INVALID_PORT_FLAGS
#define SENTPRO_INVALID_LOG_PORT_CFG         RB_INVALID_LOG_PORT_CFG
#define SENTPRO_INVALID_OS_TYPE              RB_INVALID_OS_TYPE
#define SENTPRO_INVALID_LOG_PORT_NUM         RB_INVALID_LOG_PORT_NUM
#define SENTPRO_INVALID_ROUTER_FLGS          RB_INVALID_ROUTER_FLGS
#define SENTPRO_INIT_NOT_CALLED              RB_INIT_NOT_CALLED
#define SENTPRO_DRVR_TYPE_NOT_SUPPORTED      RB_DRVR_TYPE_NOT_SUPPORTED
#define SENTPRO_FAIL_ON_DRIVER_COMM          RB_FAIL_ON_DRIVER_COMM

/**  C-Plus API error codes.  **/
#define SENTC_SUCCESS                      RB_SUCCESS
#define SENTC_INVALID_FUNCTION             RB_INVALID_FUNCTION_CODE
#define SENTC_INVALID_PACKET               RB_INVALID_PACKET
#define SENTC_UNIT_NOT_FOUND               RB_UNIT_NOT_FOUND
#define SENTC_PORT_IS_BUSY                 RB_PORT_IS_BUSY
#define SENTC_NO_PORT_FOUND                RB_WRITE_NOT_READY
#define SENTC_DRIVER_OPEN_ERROR            RB_FAIL_ON_DRIVER_OPEN
#define SENTC_DRIVER_NOT_INSTALLED         RB_DRIVER_NOT_INSTALLED
#define SENTC_IO_COMMUNICATIONS_ERROR      RB_COMMUNICATIONS_ERROR
#define SENTC_PACKET_TOO_SMALL             RB_PACKET_TOO_SMALL
#define SENTC_INVALID_PARAMETER            RB_INVALID_PARAMETER
#define SENTC_VERSION_NOT_SUPPORTED        RB_VERSION_NOT_SUPPORTED
#define SENTC_OS_NOT_SUPPORTED             RB_OS_ENV_NOT_SUPPORTED
#define SENTC_INVALID_COMMAND              RB_INVALID_COMMAND
#define SENTC_MEM_ALIGNMENT_ERROR          RB_MEM_ALIGNMENT_ERROR
#define SENTC_DRIVER_IS_BUSY               RB_DRIVER_IS_BUSY
#define SENTC_PORT_ALLOCATION_FAILURE      RB_PORT_ALLOCATION_FAILURE
#define SENTC_PORT_RELEASE_FAILURE         RB_PORT_RELEASE_FAILURE
#define SENTC_ACQUIRE_PORT_TIMEOUT         RB_ACQUIRE_PORT_TIMEOUT
#define SENTC_SIGNAL_NOT_SUPPORTED         RB_SIGNAL_NOT_SUPPORTED
#define SENTC_UNKNOWN_MACHINE              RB_UNKNOWN_MACHINE
#define SENTC_SYS_API_ERROR                RB_SYS_API_ERROR
#define SENTC_INVALID_MACH_TYPE            RB_INVALID_PORT_TYPE
#define SENTC_INVALID_IRQ_MASK             RB_INVALID_IRQ_MASK
#define SENTC_INVALID_CONT_METHOD          RB_INVALID_CONT_METHOD
#define SENTC_INVALID_PORT_FLAGS           RB_INVALID_PORT_FLAGS
#define SENTC_INVALID_LOG_PORT_CFG         RB_INVALID_LOG_PORT_CFG
#define SENTC_INVALID_OS_TYPE              RB_INVALID_OS_TYPE
#define SENTC_INVALID_LOG_PORT_NUM         RB_INVALID_LOG_PORT_NUM
#define SENTC_INVALID_ROUTER_FLGS          RB_INVALID_ROUTER_FLGS
#define SENTC_INIT_NOT_CALLED              RB_INIT_NOT_CALLED
#define SENTC_DRVR_TYPE_NOT_SUPPORTED      RB_DRVR_TYPE_NOT_SUPPORTED
#define SENTC_FAIL_ON_DRIVER_COMM          RB_FAIL_ON_DRIVER_COMM

/**  SuperPro API error codes.  **/
#define SP_SUCCESS                      RB_SUCCESS
#define SP_INVALID_FUNCTION_CODE        RB_INVALID_FUNCTION_CODE
#define SP_INVALID_PACKET               RB_INVALID_PACKET
#define SP_UNIT_NOT_FOUND               RB_UNIT_NOT_FOUND
#define SP_ACCESS_DENIED                RB_ACCESS_DENIED
#define SP_INVALID_MEMORY_ADDRESS       RB_INVALID_MEMORY_ADDRESS
#define SP_INVALID_ACCESS_CODE          RB_INVALID_ACCESS_CODE
#define SP_PORT_IS_BUSY                 RB_PORT_IS_BUSY
#define SP_WRITE_NOT_READY              RB_WRITE_NOT_READY
#define SP_NO_PORT_FOUND                RB_NO_PORT_FOUND
#define SP_ALREADY_ZERO                 RB_ALREADY_ZERO
#define SP_DRIVER_OPEN_ERROR            RB_FAIL_ON_DRIVER_OPEN
#define SP_DRIVER_NOT_INSTALLED         RB_DRIVER_NOT_INSTALLED
#define SP_IO_COMMUNICATIONS_ERROR      RB_COMMUNICATIONS_ERROR
#define SP_PACKET_TOO_SMALL             RB_PACKET_TOO_SMALL
#define SP_INVALID_PARAMETER            RB_INVALID_PARAMETER
#define SP_MEM_ACCESS_ERROR             RB_MEM_ACCESS_ERROR
#define SP_VERSION_NOT_SUPPORTED        RB_VERSION_NOT_SUPPORTED
#define SP_OS_NOT_SUPPORTED             RB_OS_ENV_NOT_SUPPORTED
#define SP_QUERY_TOO_LONG               RB_QUERY_TOO_LONG
#define SP_INVALID_COMMAND              RB_INVALID_COMMAND
#define SP_MEM_ALIGNMENT_ERROR          RB_MEM_ALIGNMENT_ERROR
#define SP_DRIVER_IS_BUSY               RB_DRIVER_IS_BUSY
#define SP_PORT_ALLOCATION_FAILURE      RB_PORT_ALLOCATION_FAILURE
#define SP_PORT_RELEASE_FAILURE         RB_PORT_RELEASE_FAILURE
#define SP_ACQUIRE_PORT_TIMEOUT         RB_AQUIRE_PORT_TIMEOUT
#define SP_SIGNAL_NOT_SUPPORTED         RB_SIGNAL_NOT_SUPPORTED
#define SP_UNKNOWN_MACHINE              RB_UNKNOWN_MACHINE
#define SP_SYS_API_ERROR                RB_SYS_API_ERROR
#define SP_UNIT_IS_BUSY                 RB_UNIT_IS_BUSY
#define SP_INVALID_PORT_TYPE            RB_INVALID_PORT_TYPE
#define SP_INVALID_MACH_TYPE            RB_INVALID_MACH_TYPE
#define SP_INVALID_IRQ_MASK             RB_INVALID_IRQ_MASK
#define SP_INVALID_CONT_METHOD          RB_INVALID_CONT_METHOD
#define SP_INVALID_PORT_FLAGS           RB_INVALID_PORT_FLAGS
#define SP_INVALID_LOG_PORT_CFG         RB_INVALID_LOG_PORT_CFG
#define SP_INVALID_OS_TYPE              RB_INVALID_OS_TYPE
#define SP_INVALID_LOG_PORT_NUM         RB_INVALID_LOG_PORT_NUM
#define SP_INVALID_ROUTER_FLGS          RB_INVALID_ROUTER_FLGS
#define SP_INIT_NOT_CALLED              RB_INIT_NOT_CALLED
#define SP_DRVR_TYPE_NOT_SUPPORTED      RB_DRVR_TYPE_NOT_SUPPORTED
#define SP_FAIL_ON_DRIVER_COMM          RB_FAIL_ON_DRIVER_COMM

/**  SCRIBE API error codes.  **/
#define SK_SUCCESS                      RB_SUCCESS
#define SK_INVALID_PACKET               RB_INVALID_PACKET
#define SK_UNIT_NOT_FOUND               RB_UNIT_NOT_FOUND
#define SK_ACCESS_DENIED                RB_ACCESS_DENIED
#define SK_PORT_IS_BUSY                 RB_INVALID_MEMORY_ADDRESS
#define SK_NO_PORT_FOUND                RB_NO_PORT_INSTALLED
#define SK_DRIVER_OPEN_ERROR            RB_FAIL_ON_DRIVER_OPEN
#define SK_DRIVER_NOT_INSTALLED         RB_DRIVER_NOT_INSTALLED
#define SK_IO_COMMUNICATIONS_ERROR      RB_COMMUNICATIONS_ERROR
#define SK_PACKET_TOO_SMALL             RB_PACKET_TOO_SMALL
#define SK_INVALID_PARAMETER            RB_INVALID_PARAMETER
#define SK_VERSION_NOT_SUPPORTED        RB_VERSION_NOT_SUPPORTED
#define SK_OS_NOT_SUPPORTED             RB_OS_ENV_NOT_SUPPORTED
#define SK_INVALID_COMMAND              RB_INVALID_COMMAND
#define SK_MEM_ALIGNMENT_ERROR          RB_MEM_ALIGNMENT_ERROR
#define SK_DRIVER_IS_BUSY               RB_DRIVER_IS_BUSY
#define SK_PORT_ALLOCATION_FAILURE      RB_PORT_ALLOCATION_FAILURE
#define SK_PORT_RELEASE_FAILURE         RB_PORT_RELEASE_FAILURE
#define SK_ACQUIRE_PORT_TIMEOUT         RB_AQUIRE_PORT_TIMEOUT
#define SK_SIGNAL_NOT_SUPPORTED         RB_SIGNAL_NOT_SUPPORTED
#define SK_UNKNOWN_MACHINE              RB_UNKNOWN_MACHINE
#define SK_SYS_API_ERROR                RB_SYS_API_ERROR
#define SK_INVALID_PORT_TYPE            RB_INVALID_PORT_TYPE
#define SK_INVALID_MACH_TYPE            RB_INVALID_MACH_TYPE
#define SK_INVALID_IRQ_MASK             RB_INVALID_IRQ_MASK
#define SK_INVALID_CONT_METHOD          RB_INVALID_CONT_METHOD
#define SK_INVALID_PORT_FLAGS           RB_INVALID_PORT_FLAGS
#define SK_INVALID_LOG_PORT_CFG         RB_INVALID_LOG_PORT_CFG
#define SK_INVALID_OS_TYPE              RB_INVALID_OS_TYPE
#define SK_INVALID_LOG_PORT_NUM         RB_INVALID_LOG_PORT_NUM
#define SK_INVALID_ROUTER_FLGS          RB_INVALID_ROUTER_FLGS
#define SK_INIT_NOT_CALLED              RB_INIT_NOT_CALLED
#define SK_DRVR_TYPE_NOT_SUPPORTED      RB_DRVR_TYPE_NOT_SUPPORTED
#define SK_FAIL_ON_DRIVER_COMM          RB_FAIL_ON_DRIVER_COMM

/* SCRIBE SK_COMMAND status codes */
#define SKCMD_FAILURE                   -1
#define SKCMD_SYS_API_ERR             -800  /* System API Error occured */
#define SKCMD_DRIVER_OPEN_ERR         -801  /* Failed on opening sys drvr */
#define SKCMD_DRIVER_COMM_ERR         -802  /* comm fail with driver */
#define SKCMD_UNKNOWN_MACH            -803  /* unknown machine type */
#define SKCMD_SIGNAL_NOT_SUPPORTED    -804  /* BUSY line not supported */
#define SKCMD_ACQUIRE_PORT_TIMEOUT    -805  /* Acquire port time-out */
#define SKCMD_PORT_RELEASE_FAIL       -806  /* Release the port failed */
#define SKCMD_PORT_ALLOC_FAIL         -807  /* the driver failed to allocate the port */
#define SKCMD_DRVR_IS_BUSY            -808  /* the sys driver is busy */
#define SKCMD_IO_COMM_ERR             -809  /* I/O comm err with key */
#define SKCMD_FAILED_ACCESS_PORT      -895
#define SKCMD_PORT_IS_BUSY            -896
#define SKCMD_CORRUPTED_DRVR          -897  /* Data invalid             */
#define SKCMD_PORT_NOT_FOUND          -899
#define SKCMD_INVALID_REQUEST         -993  /* requested command is invalid */
#define SKCMD_NO_KEY_DETECTED         -994
#define SKCMD_INVALID_PARAMETER       -995  /* requested command has invalid parameter */
#define SKCMD_ACCESS_DENIED           -997  /* requested command requires a higher
                                               permission level */
#define SKCMD_VERSION_NOT_SUPPORTED   -998  /* Update system driver     */
#define SKCMD_DRVR_NOT_INSTALLED      -999  /* system driver is not installed */

#endif
/* end of file */
