// $Header: /CAMCAD/4.4/read_wrt/Mentor.h 9     3/08/04 2:03a Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#if !defined(__Mentor_h__)
#define __Mentor_h__

#pragma once

#define  MEN_UNKNOWN          0
#define  MEN_GEOM             1     
#define  MEN_COMP             2
#define  MEN_WIRE             3
#define  MEN_NETLIST          4
#define  MEN_TECHNOLOGY       5
#define  MEN_PACKAGE          6
#define  MEN_LAYER            7
#define  MEN_PINS             8
#define  MEN_TESTPOINT        9
#define  MEN_NEUT             10 // must be the last
#define  MEN_TOTAL            11

#define  MAX_TOK              127
#define  MAX_ATT              255

class CCEtoODBDoc;
class FileStruct;

int tst_mentorbrdfiletype(FILE *ifp);
int do_wires(CCEtoODBDoc *Doc, FileStruct *pcbFile, FILE *fLog, FILE *fWire, int via, int pageUnits);

#endif /*__Mentor_h__*/