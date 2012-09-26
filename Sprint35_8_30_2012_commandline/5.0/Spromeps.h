// $Header: /CAMCAD/4.3/Spromeps.h 7     8/12/03 9:06p Kurt Van Ness $

/***************************************************************************
* SPROMEPS.H
*
* (C) Copyright 1989-1993 Rainbow Technologies, Inc. All rights reserved.
*
* Description - SuperPro Multiple Entry Points Header file.
*
* Purpose     - This module provides a method for performing SuperPro API
*               commands so you do not have to deal with command packets.
*               It provides a function for each API command.                                                    * 
*
****************************************************************************/
#ifndef _SPROMEPS_H
#define _SPROMEPS_H

#include "rbtypes.h"                   /* for Rainbow typedefs       */
#include "rbstatus.h"                  /* for Rainbow status codes   */
#include "rbcommon.h"                  /* for common typedefs        */

#define SPRO_APIPACKET_SIZE            RB_DEFAULT_PACKET_SIZE
#define SPRO_MAX_QUERY_SIZE            56 /* in bytes         */

typedef RB_DWORD RB_SPRO_APIPACKET[SPRO_APIPACKET_SIZE/sizeof(RB_DWORD)];
typedef RB_WORD  SP_STATUS;
typedef RBP_VOID RBP_SPRO_APIPACKET;

/* provided for packward compatibility for OS/2 spromeps.h */
#undef  RNBO_SPRO_API
#define RNBO_SPRO_APIPACKET     RB_SPRO_APIPACKET
#define RNBO_SPRO_APIPACKET_PTR RBP_SPRO_APIPACKET
#define RNBO_SPRO_API           SP_API
#define RNBO_USHORT_PTR         RBP_WORD
#define RNBO_UCHAR_PTR          RBP_BYTE
#define RNBO_QUERY_PTR          RBP_BYTE
#define RNBO_ULONG_PTR          RBP_DWORD

/* Define the extern routines */
#if (defined(_DOS_) && defined(_16BIT_))
#if defined(__TINY__)
RB_EXPORT SP_STATUS RB_CDECL RNBOsproCfgLibParams( RBP_SPRO_APIPACKET  thePacket, RBP_SPRO_LIB_PARAMS params );
RB_EXPORT SP_STATUS RB_CDECL RNBOsproFormatPacket( RBP_SPRO_APIPACKET thePacket, RB_WORD thePacketSize );
RB_EXPORT SP_STATUS RB_CDECL RNBOsproInitialize( RBP_SPRO_APIPACKET packet );
RB_EXPORT SP_STATUS RB_CDECL RNBOsproFindFirstUnit( RBP_SPRO_APIPACKET packet,
                                                    RB_WORD            devleoperID );
RB_EXPORT SP_STATUS RB_CDECL RNBOsproFindNextUnit( RBP_SPRO_APIPACKET packet );

RB_EXPORT SP_STATUS RB_CDECL RNBOsproRead(  RBP_SPRO_APIPACKET packet,
                                            RB_WORD            address,
                                           RBP_WORD           data );
RB_EXPORT SP_STATUS RB_CDECL RNBOsproExtendedRead(  RBP_SPRO_APIPACKET packet,
                                                    RB_WORD            address,
                                                   RBP_WORD           data,
                                                   RBP_BYTE           accessCode );
RB_EXPORT SP_STATUS RB_CDECL RNBOsproWrite( RBP_SPRO_APIPACKET packet,
                                            RB_WORD            writePassword,
                                            RB_WORD            address,
                                            RB_WORD            data,
                                            RB_BYTE            accessCode );
RB_EXPORT SP_STATUS RB_CDECL RNBOsproOverwrite( RBP_SPRO_APIPACKET packet,
                                                RB_WORD            writePassword,
                                                RB_WORD            overwritePassword1,
                                                RB_WORD            overwritePassword2,
                                                RB_WORD            address,
                                                RB_WORD            data,
                                                RB_BYTE            accessCode );
RB_EXPORT SP_STATUS RB_CDECL RNBOsproDecrement( RBP_SPRO_APIPACKET packet,
                                                RB_WORD            writePassword,
                                                RB_WORD            address );
RB_EXPORT SP_STATUS RB_CDECL RNBOsproActivate( RBP_SPRO_APIPACKET packet,
                                               RB_WORD            writePassword,
                                               RB_WORD            activatePassword1,
                                               RB_WORD            activatePassword2,
                                               RB_WORD            address );
RB_EXPORT SP_STATUS RB_CDECL RNBOsproQuery(  RBP_SPRO_APIPACKET packet,
                                             RB_WORD            address,
                                             RBP_VOID           queryData,
                                            RBP_VOID           response,
                                            RBP_DWORD          response32,
                                             RB_WORD            length );
RB_EXPORT RB_WORD RB_CDECL RNBOsproGetFullStatus( RBP_SPRO_APIPACKET thePacket );

RB_EXPORT SP_STATUS RB_CDECL RNBOsproGetVersion(  RBP_SPRO_APIPACKET thePacket,
                                                 RBP_BYTE           majVer,
                                                 RBP_BYTE           minVer,
                                                 RBP_BYTE           rev,
                                                 RBP_BYTE           osDrvrType );


RB_EXPORT SP_STATUS RB_CDECL RNBOsproGetUnitInfo(  RBP_SPRO_APIPACKET thePacket,
                                                  RBP_UNIT_INFO      unitInfo );
RB_EXPORT SP_STATUS RB_CDECL RNBOsproSetUnitInfo( RBP_SPRO_APIPACKET thePacket,
                                                  RBP_UNIT_INFO      unitInfo );
#else
RB_EXPORT SP_STATUS RB_FAR RB_CDECL RNBOsproCfgLibParams( RBP_SPRO_APIPACKET  thePacket,
                                                          RBP_SPRO_LIB_PARAMS params );
RB_EXPORT SP_STATUS RB_FAR RB_CDECL RNBOsproFormatPacket( RBP_SPRO_APIPACKET thePacket,
                                                           RB_WORD            thePacketSize );
RB_EXPORT SP_STATUS RB_FAR RB_CDECL RNBOsproInitialize( RBP_SPRO_APIPACKET packet );

RB_EXPORT SP_STATUS RB_FAR RB_CDECL RNBOsproFindFirstUnit( RBP_SPRO_APIPACKET packet,
                                                           RB_WORD            devleoperID );
RB_EXPORT SP_STATUS RB_FAR RB_CDECL RNBOsproFindNextUnit( RBP_SPRO_APIPACKET packet );

RB_EXPORT SP_STATUS RB_FAR RB_CDECL RNBOsproRead(  RBP_SPRO_APIPACKET packet,
                                                   RB_WORD            address,
                                                  RBP_WORD           data );
RB_EXPORT SP_STATUS RB_FAR RB_CDECL RNBOsproExtendedRead(  RBP_SPRO_APIPACKET packet,
                                                           RB_WORD            address,
                                                          RBP_WORD           data,
                                                          RBP_BYTE           accessCode );
RB_EXPORT SP_STATUS RB_FAR RB_CDECL RNBOsproWrite( RBP_SPRO_APIPACKET packet,
                                                   RB_WORD            writePassword,
                                                   RB_WORD            address,
                                                   RB_WORD            data,
                                                   RB_BYTE            accessCode );
RB_EXPORT SP_STATUS RB_FAR RB_CDECL RNBOsproOverwrite( RBP_SPRO_APIPACKET packet,
                                                       RB_WORD            writePassword,
                                                       RB_WORD            overwritePassword1,
                                                       RB_WORD            overwritePassword2,
                                                       RB_WORD            address,
                                                       RB_WORD            data,
                                                       RB_BYTE            accessCode );
RB_EXPORT SP_STATUS RB_FAR RB_CDECL RNBOsproDecrement( RBP_SPRO_APIPACKET packet,
                                                       RB_WORD            writePassword,
                                                       RB_WORD            address );
RB_EXPORT SP_STATUS RB_FAR RB_CDECL RNBOsproActivate( RBP_SPRO_APIPACKET packet,
                                                      RB_WORD            writePassword,
                                                      RB_WORD            activatePassword1,
                                                      RB_WORD            activatePassword2,
                                                      RB_WORD            address );
RB_EXPORT SP_STATUS RB_FAR RB_CDECL RNBOsproQuery(  RBP_SPRO_APIPACKET packet,
                                                    RB_WORD            address,
                                                    RBP_VOID           queryData,
                                                   RBP_VOID           response,
                                                   RBP_DWORD          response32,
                                                    RB_WORD            length );
RB_EXPORT RB_WORD RB_FAR RB_CDECL RNBOsproGetFullStatus( RBP_SPRO_APIPACKET thePacket );

RB_EXPORT SP_STATUS RB_FAR RB_CDECL RNBOsproGetVersion(  RBP_SPRO_APIPACKET thePacket,
                                                        RBP_BYTE           majVer,
                                                        RBP_BYTE           minVer,
                                                        RBP_BYTE           rev,
                                                        RBP_BYTE           osDrvrType );
RB_EXPORT SP_STATUS RB_FAR RB_CDECL RNBOsproGetUnitInfo(  RBP_SPRO_APIPACKET thePacket,
                                                         RBP_UNIT_INFO      unitInfo );
RB_EXPORT SP_STATUS RB_FAR RB_CDECL RNBOsproSetUnitInfo( RBP_SPRO_APIPACKET thePacket,
                                                         RBP_UNIT_INFO      unitInfo );
#endif
#elif (defined(_DOS_) && defined(_32BIT_))
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproCfgLibParams( RBP_SPRO_APIPACKET  thePacket,
                                                     RBP_SPRO_LIB_PARAMS params );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproFormatPacket( RBP_SPRO_APIPACKET thePacket,
                                                      RB_WORD            thePacketSize );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproInitialize( RBP_SPRO_APIPACKET packet );

RB_EXPORT SP_STATUS RB_STDCALL RNBOsproFindFirstUnit( RBP_SPRO_APIPACKET packet,
                                                      RB_WORD            devleoperID );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproFindNextUnit( RBP_SPRO_APIPACKET packet );

RB_EXPORT SP_STATUS RB_STDCALL RNBOsproRead(  RBP_SPRO_APIPACKET packet,
                                              RB_WORD            address,
                                             RBP_WORD           data );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproExtendedRead(  RBP_SPRO_APIPACKET packet,
                                                      RB_WORD            address,
                                                     RBP_WORD           data,
                                                     RBP_BYTE           accessCode );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproWrite( RBP_SPRO_APIPACKET packet,
                                              RB_WORD            writePassword,
                                              RB_WORD            address,
                                              RB_WORD            data,
                                              RB_BYTE            accessCode );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproOverwrite( RBP_SPRO_APIPACKET packet,
                                                  RB_WORD            writePassword,
                                                  RB_WORD            overwritePassword1,
                                                  RB_WORD            overwritePassword2,
                                                  RB_WORD            address,
                                                  RB_WORD            data,
                                                  RB_BYTE            accessCode );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproDecrement( RBP_SPRO_APIPACKET packet,
                                                  RB_WORD            writePassword,
                                                  RB_WORD            address );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproActivate( RBP_SPRO_APIPACKET packet,
                                                 RB_WORD            writePassword,
                                                 RB_WORD            activatePassword1,
                                                 RB_WORD            activatePassword2,
                                                 RB_WORD            address );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproQuery(  RBP_SPRO_APIPACKET packet,
                                               RB_WORD            address,
                                               RBP_VOID           queryData,
                                              RBP_VOID           response,
                                              RBP_DWORD          response32,
                                               RB_WORD            length );
RB_EXPORT RB_WORD RB_STDCALL RNBOsproGetFullStatus( RBP_SPRO_APIPACKET thePacket );

RB_EXPORT SP_STATUS RB_STDCALL RNBOsproGetVersion(  RBP_SPRO_APIPACKET thePacket,
                                                   RBP_BYTE           majVer,
                                                   RBP_BYTE           minVer,
                                                   RBP_BYTE           rev,
                                                   RBP_BYTE           osDrvrType );


RB_EXPORT SP_STATUS RB_STDCALL RNBOsproGetUnitInfo(  RBP_SPRO_APIPACKET thePacket,
                                                    RBP_UNIT_INFO      unitInfo );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproSetUnitInfo( RBP_SPRO_APIPACKET thePacket,
                                                    RBP_UNIT_INFO      unitInfo );
#elif (defined(_WIN_) && defined(_16BIT_))
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproCfgLibParams( RBP_SPRO_APIPACKET  thePacket,
                                                          RBP_SPRO_LIB_PARAMS params );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproFormatPacket( RBP_SPRO_APIPACKET thePacket,
                                                           RB_WORD            thePacketSize );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproInitialize( RBP_SPRO_APIPACKET packet );

RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproFindFirstUnit( RBP_SPRO_APIPACKET packet,
                                                           RB_WORD            devleoperID );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproFindNextUnit( RBP_SPRO_APIPACKET packet );

RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproRead(  RBP_SPRO_APIPACKET packet,
                                                   RB_WORD            address,
                                                  RBP_WORD           data );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproExtendedRead(  RBP_SPRO_APIPACKET packet,
                                                           RB_WORD            address,
                                                          RBP_WORD           data,
                                                          RBP_BYTE           accessCode );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproWrite( RBP_SPRO_APIPACKET packet,
                                                   RB_WORD            writePassword,
                                                   RB_WORD            address,
                                                   RB_WORD            data,
                                                   RB_BYTE            accessCode );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproOverwrite( RBP_SPRO_APIPACKET packet,
                                                       RB_WORD            writePassword,
                                                       RB_WORD            overwritePassword1,
                                                       RB_WORD            overwritePassword2,
                                                       RB_WORD            address,
                                                       RB_WORD            data,
                                                       RB_BYTE            accessCode );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproDecrement( RBP_SPRO_APIPACKET packet,
                                                       RB_WORD            writePassword,
                                                       RB_WORD            address );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproActivate( RBP_SPRO_APIPACKET packet,
                                                      RB_WORD            writePassword,
                                                      RB_WORD            activatePassword1,
                                                      RB_WORD            activatePassword2,
                                                      RB_WORD            address );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproQuery(  RBP_SPRO_APIPACKET packet,
                                                    RB_WORD            address,
                                                    RBP_VOID           queryData,
                                                   RBP_VOID           response,
                                                   RBP_DWORD          response32,
                                                    RB_WORD            length );
RB_EXPORT RB_WORD RB_FAR RB_PASCAL RNBOsproGetFullStatus( RBP_SPRO_APIPACKET thePacket );

RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproGetVersion(  RBP_SPRO_APIPACKET thePacket,
                                                        RBP_BYTE           majVer,
                                                        RBP_BYTE           minVer,
                                                        RBP_BYTE           rev,
                                                        RBP_BYTE           osDrvrType );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproGetUnitInfo(  RBP_SPRO_APIPACKET thePacket,
                                                         RBP_UNIT_INFO      unitInfo );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproSetUnitInfo( RBP_SPRO_APIPACKET thePacket,
                                                         RBP_UNIT_INFO      unitInfo );
#elif defined(_WIN32_)
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproCfgLibParams( RBP_SPRO_APIPACKET  thePacket,
                                                     RBP_SPRO_LIB_PARAMS params );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproFormatPacket( RBP_SPRO_APIPACKET thePacket,
                                                      RB_WORD            thePacketSize );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproInitialize( RBP_SPRO_APIPACKET packet );

RB_EXPORT SP_STATUS RB_STDCALL RNBOsproFindFirstUnit( RBP_SPRO_APIPACKET packet,
                                                      RB_WORD            devleoperID );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproFindNextUnit( RBP_SPRO_APIPACKET packet );

RB_EXPORT SP_STATUS RB_STDCALL RNBOsproRead(  RBP_SPRO_APIPACKET packet,
                                              RB_WORD            address,
                                             RBP_WORD           data );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproExtendedRead(  RBP_SPRO_APIPACKET packet,
                                                      RB_WORD            address,
                                                     RBP_WORD           data,
                                                     RBP_BYTE           accessCode );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproWrite( RBP_SPRO_APIPACKET packet,
                                              RB_WORD            writePassword,
                                              RB_WORD            address,
                                              RB_WORD            data,
                                              RB_BYTE            accessCode );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproOverwrite( RBP_SPRO_APIPACKET packet,
                                                  RB_WORD            writePassword,
                                                  RB_WORD            overwritePassword1,
                                                  RB_WORD            overwritePassword2,
                                                  RB_WORD            address,
                                                  RB_WORD            data,
                                                  RB_BYTE            accessCode );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproDecrement( RBP_SPRO_APIPACKET packet,
                                                  RB_WORD            writePassword,
                                                  RB_WORD            address );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproActivate( RBP_SPRO_APIPACKET packet,
                                                 RB_WORD            writePassword,
                                                 RB_WORD            activatePassword1,
                                                 RB_WORD            activatePassword2,
                                                 RB_WORD            address );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproQuery(  RBP_SPRO_APIPACKET packet,
                                               RB_WORD            address,
                                               RBP_VOID           queryData,
                                              RBP_VOID           response,
                                              RBP_DWORD          response32,
                                               RB_WORD            length );
RB_EXPORT RB_WORD RB_STDCALL RNBOsproGetFullStatus( RBP_SPRO_APIPACKET thePacket );

RB_EXPORT SP_STATUS RB_STDCALL RNBOsproGetVersion(  RBP_SPRO_APIPACKET thePacket,
                                                   RBP_BYTE           majVer,
                                                   RBP_BYTE           minVer,
                                                   RBP_BYTE           rev,
                                                   RBP_BYTE           osDrvrType );


RB_EXPORT SP_STATUS RB_STDCALL RNBOsproGetUnitInfo(  RBP_SPRO_APIPACKET thePacket,
                                                    RBP_UNIT_INFO      unitInfo );
RB_EXPORT SP_STATUS RB_STDCALL RNBOsproSetUnitInfo( RBP_SPRO_APIPACKET thePacket,
                                                    RBP_UNIT_INFO      unitInfo );
#elif (defined(_OS2_) && defined(_16BIT_))
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproCfgLibParams( RBP_SPRO_APIPACKET  thePacket,
                                                          RBP_SPRO_LIB_PARAMS params );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproFormatPacket( RBP_SPRO_APIPACKET thePacket,
                                                           RB_WORD            thePacketSize );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproInitialize( RBP_SPRO_APIPACKET packet );

RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproFindFirstUnit( RBP_SPRO_APIPACKET packet,
                                                           RB_WORD            devleoperID );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproFindNextUnit( RBP_SPRO_APIPACKET packet );

RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproRead(  RBP_SPRO_APIPACKET packet,
                                                   RB_WORD            address,
                                                  RBP_WORD           data );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproExtendedRead(  RBP_SPRO_APIPACKET packet,
                                                           RB_WORD            address,
                                                          RBP_WORD           data,
                                                          RBP_BYTE           accessCode );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproWrite( RBP_SPRO_APIPACKET packet,
                                                   RB_WORD            writePassword,
                                                   RB_WORD            address,
                                                   RB_WORD            data,
                                                   RB_BYTE            accessCode );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproOverwrite( RBP_SPRO_APIPACKET packet,
                                                       RB_WORD            writePassword,
                                                       RB_WORD            overwritePassword1,
                                                       RB_WORD            overwritePassword2,
                                                       RB_WORD            address,
                                                       RB_WORD            data,
                                                       RB_BYTE            accessCode );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproDecrement( RBP_SPRO_APIPACKET packet,
                                                       RB_WORD            writePassword,
                                                       RB_WORD            address );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproActivate( RBP_SPRO_APIPACKET packet,
                                                      RB_WORD            writePassword,
                                                      RB_WORD            activatePassword1,
                                                      RB_WORD            activatePassword2,
                                                      RB_WORD            address );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproQuery(  RBP_SPRO_APIPACKET packet,
                                                    RB_WORD            address,
                                                    RBP_VOID           queryData,
                                                   RBP_VOID           response,
                                                   RBP_DWORD          response32,
                                                    RB_WORD            length );
RB_EXPORT RB_WORD RB_FAR RB_PASCAL RNBOsproGetFullStatus( RBP_SPRO_APIPACKET thePacket );

RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproGetVersion(  RBP_SPRO_APIPACKET thePacket,
                                                        RBP_BYTE           majVer,
                                                        RBP_BYTE           minVer,
                                                        RBP_BYTE           rev,
                                                        RBP_BYTE           osDrvrType );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproGetUnitInfo(  RBP_SPRO_APIPACKET thePacket,
                                                         RBP_UNIT_INFO      unitInfo );
RB_EXPORT SP_STATUS RB_FAR RB_PASCAL RNBOsproSetUnitInfo( RBP_SPRO_APIPACKET thePacket,
                                                         RBP_UNIT_INFO      unitInfo );
#elif (defined(_OS2_) && defined(_32BIT_))
RB_EXPORT SP_STATUS RB_SYSCALL RNBOsproCfgLibParams( RBP_SPRO_APIPACKET  thePacket,
                                                     RBP_SPRO_LIB_PARAMS params );
RB_EXPORT SP_STATUS RB_SYSCALL RNBOsproFormatPacket( RBP_SPRO_APIPACKET thePacket,
                                                      RB_WORD            thePacketSize );
RB_EXPORT SP_STATUS RB_SYSCALL RNBOsproInitialize( RBP_SPRO_APIPACKET packet );

RB_EXPORT SP_STATUS RB_SYSCALL RNBOsproFindFirstUnit( RBP_SPRO_APIPACKET packet,
                                                      RB_WORD            devleoperID );
RB_EXPORT SP_STATUS RB_SYSCALL RNBOsproFindNextUnit( RBP_SPRO_APIPACKET packet );

RB_EXPORT SP_STATUS RB_SYSCALL RNBOsproRead(  RBP_SPRO_APIPACKET packet,
                                              RB_WORD            address,
                                             RBP_WORD           data );
RB_EXPORT SP_STATUS RB_SYSCALL RNBOsproExtendedRead(  RBP_SPRO_APIPACKET packet,
                                                      RB_WORD            address,
                                                     RBP_WORD           data,
                                                     RBP_BYTE           accessCode );
RB_EXPORT SP_STATUS RB_SYSCALL RNBOsproWrite( RBP_SPRO_APIPACKET packet,
                                              RB_WORD            writePassword,
                                              RB_WORD            address,
                                              RB_WORD            data,
                                              RB_BYTE            accessCode );
RB_EXPORT SP_STATUS RB_SYSCALL RNBOsproOverwrite( RBP_SPRO_APIPACKET packet,
                                                  RB_WORD            writePassword,
                                                  RB_WORD            overwritePassword1,
                                                  RB_WORD            overwritePassword2,
                                                  RB_WORD            address,
                                                  RB_WORD            data,
                                                  RB_BYTE            accessCode );
RB_EXPORT SP_STATUS RB_SYSCALL RNBOsproDecrement( RBP_SPRO_APIPACKET packet,
                                                  RB_WORD            writePassword,
                                                  RB_WORD            address );
RB_EXPORT SP_STATUS RB_SYSCALL RNBOsproActivate( RBP_SPRO_APIPACKET packet,
                                                 RB_WORD            writePassword,
                                                 RB_WORD            activatePassword1,
                                                 RB_WORD            activatePassword2,
                                                 RB_WORD            address );
RB_EXPORT SP_STATUS RB_SYSCALL RNBOsproQuery(  RBP_SPRO_APIPACKET packet,
                                               RB_WORD            address,
                                               RBP_VOID           queryData,
                                              RBP_VOID           response,
                                              RBP_DWORD          response32,
                                               RB_WORD            length );
RB_EXPORT RB_WORD RB_SYSCALL RNBOsproGetFullStatus( RBP_SPRO_APIPACKET thePacket );

RB_EXPORT SP_STATUS RB_SYSCALL RNBOsproGetVersion(  RBP_SPRO_APIPACKET thePacket,
                                                   RBP_BYTE           majVer,
                                                   RBP_BYTE           minVer,
                                                   RBP_BYTE           rev,
                                                   RBP_BYTE           osDrvrType );


RB_EXPORT SP_STATUS RB_SYSCALL RNBOsproGetUnitInfo(  RBP_SPRO_APIPACKET thePacket,
                                                    RBP_UNIT_INFO      unitInfo );
RB_EXPORT SP_STATUS RB_SYSCALL RNBOsproSetUnitInfo( RBP_SPRO_APIPACKET thePacket,
                                                    RBP_UNIT_INFO      unitInfo );
#endif
#endif                                       /* _SPROMEPS_H                */
/* end of file */
