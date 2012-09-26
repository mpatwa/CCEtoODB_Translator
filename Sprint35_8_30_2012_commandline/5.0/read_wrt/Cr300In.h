// $Header: /CAMCAD/4.4/read_wrt/Cr300In.h 8     3/31/04 3:33p Dean Admin $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

/* Define Section *********************************************************/

#define  TERMALL    "TERM_ALL"
#define  TERMTOP    "TERM_TOP"

#define  MDF_TYPE          1
#define  UDF_TYPE          2

#define  VER_LINE          1
#define  VER_SRF           2
#define  VER_ARC           3
#define  VER_NEGACONTOUR   4

#define  MAX_POLY          10000
#define  MAX_LINE          40000      // must be very big, because a command is read until ; (command end char).
#define  MAX_LAYERS        256
#define  MAX_TEXTTABLE     1000
#define  MAX_TURRET        1000
#define  MAX_VIATABLE      1000
#define  MAX_LAND          1000
#define  MAX_SMDPIN        1000

#define  PART_FIDUCIAL     1
#define  PART_FIDUCIALTOP  2
#define  PART_FIDUCIALBOT  3
#define  PART_TOOL         4

/* Structure Section *********************************************************/

typedef struct
{
   double d;
   int    toolindex;
}CR3000Drill;
typedef CArray<CR3000Drill, CR3000Drill&> CDrillArray;

typedef struct
{
   CString  attrib;
   CString  mapattrib;
}CR3000Attribmap;
typedef CTypedPtrArray<CPtrArray, CR3000Attribmap*> CAttribmapArray;

typedef struct
{
   CString  name;
   CString  originalname;
   double   x,y,rot;
   int      mirror;
}CR3000Comp;
typedef CTypedPtrArray<CPtrArray, CR3000Comp*> CCompArray;

typedef struct
{
   CString  termnoun;
   CString  pinname; 
   //int    pinnumber; // in new versions, this is a pinname
   double   x,y;
   int      defined;
   int      termlayer;
} CR3000Terminal;
typedef CTypedPtrArray<CPtrArray, CR3000Terminal*> CTerminalArray;

typedef struct
{
   int   wcode;   // aperture number, which pointers into the turret table
   char  flat;
   int   layer;
   int   cnt;
   Point2 *p;
} Smdpin;

typedef struct
{
   int   turret;
   int   layer;
} Land;

typedef struct
{
   int   number;
   double holediameter;
   int   comp_app;
   int   comp_app_unc;
   int   comp_app_therm;
   int   comp_app_clear;
   int   inner_app;
   int   inner_app_unc;
   int   inner_app_therm;
   int   inner_app_clear;
   int   solder_app;
   int   solder_app_unc;
   int   solder_app_therm;
   int   solder_app_clear;
   int   used;            // 1 is PAD 2 is Via
} Viatable;

typedef struct
{
   int   number;
   char  type;
   double outer;
   double inner;
} Turret;

typedef struct
{
   int      nr;
   double   height;
   double   width;
   double   space;
}Texttable;

typedef struct
{
   int      number;
   CString  comment;
}ArtLaySpec;

typedef struct
{
   int   number;
   int   wirelay;
   int   negative;   // positive, negative
}WireLaySpec;

typedef struct
{
   CString  name;
   int      smd;     // this is needed, if we have a to put in a dummy
                     // pad.
   int      number;
} Shape;

typedef struct
{
   char  name[80];
} NetName;

typedef struct
{
   double x,y;
   double rad;
   int    f;
}Poly_l;

typedef struct
{
   int   compNumber;    
   int   blockNumber;   
   BOOL  done;
}CompNumber;
typedef CArray<CompNumber, CompNumber&> CompNumberArray;

typedef struct
{
   CString  pinNumber;
   CString  pinName;
}PinName;
typedef CArray<PinName, PinName&> PinNameArray;

typedef struct
{
   CString  name;
   int      parttype;   // normal, fiducialtop, fiducialbot, fiducialboth
}Cr3000Part;
typedef CTypedPtrArray<CPtrArray, Cr3000Part*> CPartArray;


