/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/
// This is a part of the PowerSYS System C++ library.
// Copyright (C) 1996 PADS Software Inc.
// All rights reserved.

// Name:		ST_def.h
// Date:		10.20.86
// Author:		Alexander Zabolotnikov	
// Description:	several definitions

#ifndef __SAS_DEF_H__
#define __SAS_DEF_H__

class SAS_Seg;
class XEvent;
class IterPolyCross;
class BndSeg;
class BarHdr;
class stProc;
class glAreaData;

const unsigned long SDB_NO_SCAN		= 0x00000000;
const unsigned long SDB_LIGHT_SCAN	= 0x00000001;
const unsigned long SDB_FULL_SCAN	= 0x00000002;
const unsigned long INVALID_SCAN	= 0x00000010;

const unsigned int SCAN_TYP_HARD	=	0x00000001;
const unsigned int SCAN_GEO_LIST	=	0x00000002;
const unsigned int SCAN_SLD_LIST	=	0x00000004;
const unsigned int SCAN_CLP_MASK	=	0x00000008;
const unsigned int SCAN_NO_CROSS	=	0x00000010;
const unsigned int SCAN_STT_SLD		=	0x00000020;
const unsigned int SCAN_LNK_HOLE	=	0x00000040;
const unsigned int SCAN_GRP_HOLE	=	0x00000080;

const unsigned int SCAN_DB_LIGHT	=	(SCAN_GEO_LIST | SCAN_STT_SLD | SCAN_CLP_MASK);
const unsigned int SCAN_DB_HARD		=	(SCAN_DB_LIGHT | SCAN_TYP_HARD);
const unsigned int SCAN_DB_ALG		=	(SCAN_DB_HARD  | SCAN_NO_CROSS);
const unsigned int SCAN_DEF_TYPE	=	0x00000000;		// light

#define STEP_ONE 1
#define STEP_TWO 2

// SAS_SegType
#define	CIR_T 0
#define	VSG_T 1
#define	SEG_T 2
#define	ARC_T 4

#define INS_EVENT	0  // <--
#define DEL_EVENT	1	// -->
#define CRS_EVENT	2	//  X
#define REP_EVENT	3	// -*-
#define CRS_REPL	4
#define NON_EVENT	8

#define MAX_XIND_ORD	64000
#define MAX_POLY_OFF	8190
#define DELTA_SIZE 1024
#define GEN_SNET_ZONE 1	// upg generation mode

#endif	// __SAS_DEF_H__

