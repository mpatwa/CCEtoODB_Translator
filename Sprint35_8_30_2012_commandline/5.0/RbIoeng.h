/****************************************************************************
* RBIOENG.H
*
* (C) Copyright 1997 Rainbow Technologies, Inc. All rights reserved.
*
*     This software contains proprietary information which shall not
*     be reproduced or transferred to other documents and shall not
*     be disclosed to others or used for manufacturing or any other
*     purpose without prior written permission of Rainbow Technologies,
*     Inc.
*
* Description : Include file for RBIOENG.LIBs for C/C++ compilers.
*
* Revision History:
*
* $Version: $
* $Archive: /Rainbow Script Engine/MISC/SAMPLES/X86/SX/WIN32/rbioeng.h $
* $Revision: 10 $
* $Header: /Rainbow Script Engine/MISC/SAMPLES/X86/SX/WIN32/rbioeng.h 10    8/25/97 9:10a Bgrove $
* $Author: Bgrove $
* $Date: 8/25/97 9:10a $
*
* $History: rbioeng.h $
* 
* *****************  Version 10  *****************
* User: Bgrove       Date: 8/25/97    Time: 9:10a
* Updated in $/Rainbow Script Engine/MISC/SAMPLES/X86/SX/WIN32
* 
* *****************  Version 9  *****************
* User: Bgrove       Date: 8/25/97    Time: 9:10a
* Updated in $/Rainbow Script Engine/MISC/SAMPLES/X86/SX/WIN16
* 
* *****************  Version 8  *****************
* User: Bgrove       Date: 8/25/97    Time: 9:10a
* Updated in $/Rainbow Script Engine/MISC/SAMPLES/X86/SX/DOSRM
* 
* *****************  Version 7  *****************
* User: Bgrove       Date: 8/25/97    Time: 9:10a
* Updated in $/Rainbow Script Engine/MISC/SAMPLES/X86/SX/DOS32X
* 
* *****************  Version 6  *****************
* User: Bgrove       Date: 8/25/97    Time: 9:10a
* Updated in $/Rainbow Script Engine/MISC/SAMPLES/X86/SX/DOS16X
* 
* *****************  Version 5  *****************
* User: Bgrove       Date: 8/25/97    Time: 9:07a
* Updated in $/Rainbow Script Engine/MISC/CLECHO
* 
* *****************  Version 4  *****************
* User: Bgrove       Date: 8/25/97    Time: 9:01a
* Updated in $/Rainbow Script Engine/CLIENT/RBIOENG
* 
* *****************  Version 3  *****************
* User: Bgrove       Date: 8/20/97    Time: 2:21p
* Updated in $/Rainbow Script Engine/CLIENT/RBIOENG
* Fixed paths
* 
* *****************  Version 2  *****************
* User: Bgrove       Date: 8/19/97    Time: 9:34a
* Updated in $/Rainbow Script Engine/CLIENT/RBIOENG
* Fixed file dependences
*
****************************************************************************/
#ifndef _RBIOENG_H
#define _RBIOENG_H

#include "rbtypes.h"

#define RBIO_PACKET_SIZE                             64 /* size in bytes */

/* I/O command types */
#define RBIO_INPUT                                   1 /* get input data           */
#define RBIO_OUTPUT                                  2 /* set outpt data           */
#define RBIO_OUTPUT_STR                              3 /* output string            */

/* Command values 0 - 99 are reserved by the Script Engine */
#define RBIO_NULL                                    0UL
#define RBIO_QUIT                                    1UL
#define RBIO_SERVER_NOT_INSTALLED                    2UL
#define RBIO_SYNC_ERR                                3UL
#define RBIO_TEST_RBIOCMD                            4UL
#define RBIO_TEST_RBIOI8                             5UL
#define RBIO_TEST_RBIOI16                            6UL
#define RBIO_TEST_RBIOI32                            7UL
#define RBIO_TEST_RBIOBUF                            8UL

/* SentinelSuperPro Command Values 100 - 199 */
#define RBIO_SX_FORMAT_PKT                         100UL
#define RBIO_SX_INITIALIZE                         101UL
#define RBIO_SX_FIND_FIRST_UNIT                    102UL
#define RBIO_SX_FIND_NEXT_UNIT                     103UL
#define RBIO_SX_READ                               104UL
#define RBIO_SX_EXTENDED_READ                      105UL
#define RBIO_SX_WRITE                              106UL
#define RBIO_SX_OVERWRITE                          107UL
#define RBIO_SX_DECREMENT                          108UL
#define RBIO_SX_ACTIVATE_ALGO                      109UL
#define RBIO_SX_QUERY                              110UL
#define RBIO_SX_GET_VERSION                        111UL
#define RBIO_SX_GET_FULL_STATUS                    112UL
#define RBIO_SX_GET_UNIT_INFO                      113UL
#define RBIO_SX_SET_UNIT_INFO                      114UL
#define RBIO_SX_CFG_LIB_PARAM_MACH_TYPE            115UL
#define RBIO_SX_CFG_LIB_PARAM_DELAY                116UL
#define RBIO_SX_CFG_LIB_PARAM_MASK_INTS            117UL
#define RBIO_SX_CFG_LIB_PARAM_ROUTER_FLAGS         118UL
#define RBIO_SX_CFG_LIB_PARAM_OS_PARAMS            119UL
#define RBIO_SX_CFG_LIB_PARAM_PORT_PARAMS          120UL
#define RBIO_SX_GET_CRYPT_INFO                     121UL

/* NetSentinel Command Values 200 - 299 */
#define RBIO_NS_OPEN_FIRST_KEY                     200UL
#define RBIO_NS_OPEN_NEXT_KEY                      201UL
#define RBIO_NS_CLOSE_KEY                          202UL
#define RBIO_NS_QUERY_KEY                          203UL
#define RBIO_NS_GET_STATS                          204UL
#define RBIO_NS_LOCK_KEY                           205UL
#define RBIO_NS_UNLOCK_KEY                         206UL
#define RBIO_NS_CANCEL_LOCK_KEY                    207UL
#define RBIO_NS_SET_INFO_KEY                       208UL
#define RBIO_NS_GET_INFO_KEY                       209UL
#define RBIO_NS_SET_BLOCKING_MODE                  210UL
#define RBIO_NS_GET_SERVER_INFO                    211UL
#define RBIO_NS_ACCESS_SUB_LICENSE                 212UL
#define RBIO_NS_SET_RNBO_LIB_PARAM_HW_PPORTS       213UL
#define RBIO_NS_SET_RNBO_LIB_PARAM_SPEC_PPORTS     214UL
#define RBIO_NS_SET_RNBO_LIB_PARAM_DEPT_NAME       215UL
#define RBIO_NS_SET_RNBO_LIB_PARAM_MAX_SERVERS     216UL
#define RBIO_NS_SET_RNBO_LIB_PARAM_PROTOCOLS       217UL
#define RBIO_NS_SET_RNBO_LIB_PARAM_NETNAME         218UL
#define RBIO_NS_SET_RNBO_LIB_PARAM_IPXSPX_TIMEOUT  219UL

/* SentinelPro Command Values 300 - 399 */
#define RBIO_SP_FORMAT_PKT                         300UL
#define RBIO_SP_INITIALIZE                         301UL
#define RBIO_SP_QUERY                              302UL
#define RBIO_SP_GET_VERSION                        303UL
#define RBIO_SP_GET_FULL_STATUS                    304UL
#define RBIO_SP_GET_UNIT_INFO                      305UL
#define RBIO_SP_SET_UNIT_INFO                      306UL
#define RBIO_SP_CFG_LIB_PARAM_MACH_TYPE            307UL
#define RBIO_SP_CFG_LIB_PARAM_DELAY                308UL
#define RBIO_SP_CFG_LIB_PARAM_MASK_INTS            309UL
#define RBIO_SP_CFG_LIB_PARAM_ROUTER_FLAGS         310UL
#define RBIO_SP_CFG_LIB_PARAM_OS_PARAMS            311UL
#define RBIO_SP_CFG_LIB_PARAM_PORT_PARAMS          312UL

/* SentinelScribe Command Values 400 - 499 */
#define RBIO_SK_FORMAT_PKT                         400UL
#define RBIO_SK_INITIALIZE                         401UL
#define RBIO_SK_SET_FAMILY                         402UL
#define RBIO_SK_SET_PORT                           403UL
#define RBIO_SK_ENABLE                             404UL
#define RBIO_SK_READ                               405UL
#define RBIO_SK_WRITE                              406UL
#define RBIO_SK_COMMAND                            407UL
#define RBIO_SK_GET_VERSION                        408UL
#define RBIO_SK_GET_FULL_STATUS                    409UL
#define RBIO_SK_GET_UNIT_INFO                      410UL
#define RBIO_SK_SET_UNIT_INFO                      411UL
#define RBIO_SK_CFG_LIB_PARAM_MACH_TYPE            412UL
#define RBIO_SK_CFG_LIB_PARAM_DELAY                413UL
#define RBIO_SK_CFG_LIB_PARAM_MASK_INTS            414UL
#define RBIO_SK_CFG_LIB_PARAM_ROUTER_FLAGS         415UL
#define RBIO_SK_CFG_LIB_PARAM_OS_PARAMS            416UL
#define RBIO_SK_CFG_LIB_PARAM_PORT_PARAMS          417UL

/* NetSentinel-C Programming API Command Values 500 - 599 */
#define RBIO_NK_FORMAT_PKT                         500UL
#define RBIO_NK_INITIALIZE                         501UL
#define RBIO_NK_SET_FAMILY                         502UL
#define RBIO_NK_SET_PORT                           503UL
#define RBIO_NK_ENABLE                             504UL
#define RBIO_NK_READ                               505UL
#define RBIO_NK_WRITE                              506UL
#define RBIO_NK_COMMAND                            507UL
#define RBIO_NK_GET_VERSION                        508UL
#define RBIO_NK_GET_FULL_STATUS                    509UL
#define RBIO_NK_GET_UNIT_INFO                      510UL
#define RBIO_NK_SET_UNIT_INFO                      511UL
#define RBIO_NK_CFG_LIB_PARAM_MACH_TYPE            512UL
#define RBIO_NK_CFG_LIB_PARAM_DELAY                513UL
#define RBIO_NK_CFG_LIB_PARAM_MANK_INTS            514UL
#define RBIO_NK_CFG_LIB_PARAM_ROUTER_FLAGS         515UL
#define RBIO_NK_CFG_LIB_PARAM_OS_PARAMS            516UL
#define RBIO_NK_CFG_LIB_PARAM_PORT_PARAMS          517UL

/* Sentinel-Cplus Command Values 600 - 699 */
#define RBIO_SC_FORMAT_PKT                         600UL
#define RBIO_SC_INITIALIZE                         601UL
#define RBIO_SC_READ                               602UL
#define RBIO_SC_GET_VERSION                        603UL
#define RBIO_SC_GET_FULL_STATUS                    604UL
#define RBIO_SC_GET_UNIT_INFO                      605UL
#define RBIO_SC_SET_UNIT_INFO                      606UL
#define RBIO_SC_CFG_LIB_PARAM_MACH_TYPE            607UL
#define RBIO_SC_CFG_LIB_PARAM_DELAY                608UL
#define RBIO_SC_CFG_LIB_PARAM_MASK_INTS            609UL
#define RBIO_SC_CFG_LIB_PARAM_ROUTER_FLAGS         610UL
#define RBIO_SC_CFG_LIB_PARAM_OS_PARAMS            611UL
#define RBIO_SC_CFG_LIB_PARAM_PORT_PARAMS          612UL

/* SentinelLm     Command Values  700 -  899 */
/* SentinelEve3   Command Values  900 -  999 */
/* NetSentinelMac Command Values 1000 - 1099 */

#if (defined(_DOS_) && defined(_16BIT_))
#if defined(__TINY__)
RB_EXPORT RB_VOID RB_CDECL RBIOINIT( RBP_VOID packet, RBP_WORD size );
RB_EXPORT RB_VOID RB_CDECL RBIOEXIT( RBP_VOID packet );
RB_EXPORT RB_VOID RB_CDECL RBIOI8(   RBP_VOID packet, RBP_WORD ioCmd, RBP_BYTE  data );
RB_EXPORT RB_VOID RB_CDECL RBIOI16(  RBP_VOID packet, RBP_WORD ioCmd, RBP_WORD  data );
RB_EXPORT RB_VOID RB_CDECL RBIOI32(  RBP_VOID packet, RBP_WORD ioCmd, RBP_DWORD data );
RB_EXPORT RB_VOID RB_CDECL RBIOBUF(  RBP_VOID packet, RBP_WORD ioCmd, RBP_VOID  buffer, RBP_WORD len );
RB_EXPORT RB_VOID RB_CDECL RBIOCMD(  RBP_VOID packet, RBP_DWORD cmd );
#else
RB_EXPORT RB_VOID RB_FAR RB_CDECL RBIOINIT( RBP_VOID packet, RBP_WORD size );
RB_EXPORT RB_VOID RB_FAR RB_CDECL RBIOEXIT( RBP_VOID packet );
RB_EXPORT RB_VOID RB_FAR RB_CDECL RBIOI8(   RBP_VOID packet, RBP_WORD ioCmd, RBP_BYTE  data );
RB_EXPORT RB_VOID RB_FAR RB_CDECL RBIOI16(  RBP_VOID packet, RBP_WORD ioCmd, RBP_WORD  data );
RB_EXPORT RB_VOID RB_FAR RB_CDECL RBIOI32(  RBP_VOID packet, RBP_WORD ioCmd, RBP_DWORD data );
RB_EXPORT RB_VOID RB_FAR RB_CDECL RBIOBUF(  RBP_VOID packet, RBP_WORD ioCmd, RBP_VOID  buffer, RBP_WORD len );
RB_EXPORT RB_VOID RB_FAR RB_CDECL RBIOCMD(  RBP_VOID packet, RBP_DWORD cmd );
#endif
#elif (defined(_DOS_) && defined(_32BIT_))
RB_EXPORT RB_VOID RB_CDECL RBIOINIT( RBP_VOID packet, RBP_WORD size );
RB_EXPORT RB_VOID RB_CDECL RBIOEXIT( RBP_VOID packet );
RB_EXPORT RB_VOID RB_CDECL RBIOI8(   RBP_VOID packet, RBP_WORD ioCmd, RBP_BYTE  data );
RB_EXPORT RB_VOID RB_CDECL RBIOI16(  RBP_VOID packet, RBP_WORD ioCmd, RBP_WORD  data );
RB_EXPORT RB_VOID RB_CDECL RBIOI32(  RBP_VOID packet, RBP_WORD ioCmd, RBP_DWORD data );
RB_EXPORT RB_VOID RB_CDECL RBIOBUF(  RBP_VOID packet, RBP_WORD ioCmd, RBP_VOID  buffer, RBP_WORD len );
RB_EXPORT RB_VOID RB_CDECL RBIOCMD(  RBP_VOID packet, RBP_DWORD cmd );
#elif (defined(_WIN_) && defined(_16BIT_))
RB_EXPORT RB_VOID RB_FAR RB_PASCAL RBIOINIT( RBP_VOID packet, RBP_WORD size );
RB_EXPORT RB_VOID RB_FAR RB_PASCAL RBIOEXIT( RBP_VOID packet );
RB_EXPORT RB_VOID RB_FAR RB_PASCAL RBIOI8(   RBP_VOID packet, RBP_WORD ioCmd, RBP_BYTE  data );
RB_EXPORT RB_VOID RB_FAR RB_PASCAL RBIOI16(  RBP_VOID packet, RBP_WORD ioCmd, RBP_WORD  data );
RB_EXPORT RB_VOID RB_FAR RB_PASCAL RBIOI32(  RBP_VOID packet, RBP_WORD ioCmd, RBP_DWORD data );
RB_EXPORT RB_VOID RB_FAR RB_PASCAL RBIOBUF(  RBP_VOID packet, RBP_WORD ioCmd, RBP_VOID  buffer, RBP_WORD len );
RB_EXPORT RB_VOID RB_FAR RB_PASCAL RBIOCMD(  RBP_VOID packet, RBP_DWORD cmd );
#elif (defined(_WIN32_))
RB_EXPORT RB_VOID RB_STDCALL RBIOINIT( RBP_VOID packet, RBP_WORD size );
RB_EXPORT RB_VOID RB_STDCALL RBIOEXIT( RBP_VOID packet );
RB_EXPORT RB_VOID RB_STDCALL RBIOI8(   RBP_VOID packet, RBP_WORD ioCmd, RBP_BYTE  data );
RB_EXPORT RB_VOID RB_STDCALL RBIOI16(  RBP_VOID packet, RBP_WORD ioCmd, RBP_WORD  data );
RB_EXPORT RB_VOID RB_STDCALL RBIOI32(  RBP_VOID packet, RBP_WORD ioCmd, RBP_DWORD data );
RB_EXPORT RB_VOID RB_STDCALL RBIOBUF(  RBP_VOID packet, RBP_WORD ioCmd, RBP_VOID  buffer, RBP_WORD len );
RB_EXPORT RB_VOID RB_STDCALL RBIOCMD(  RBP_VOID packet, RBP_DWORD cmd );
#endif

#endif  /* _RBIOENG_H */
/* end of file */


