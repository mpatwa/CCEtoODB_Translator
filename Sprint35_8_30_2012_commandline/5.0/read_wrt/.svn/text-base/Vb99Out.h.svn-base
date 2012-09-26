// $Header: /CAMCAD/4.4/read_wrt/Vb99Out.h 10    5/07/04 3:43p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once

/* Defines Section ***************************************************************************/

#define  CLASS_UNKNOWN              0
#define  CLASS_BOARD_GEOMETRY       1
#define  CLASS_PACKAGE_GEOMETRY     2
#define  CLASS_ETCH                 3
#define  CLASS_DRAWING_FORMAT       4
#define  CLASS_MANUFACTURING        5
#define  CLASS_COMPONENT_VALUE      6
#define  CLASS_DEVICE_TYPE          7
#define  CLASS_REF_DES              8
#define  CLASS_TOLERANCE            9
#define  CLASS_USER_PART_NUMBER     10
#define  CLASS_PACKAGE_KEEPIN       11
#define  CLASS_PACKAGE_KEEPOUT      12
#define  CLASS_ROUTE_KEEPIN         13
#define  CLASS_ROUTE_KEEPOUT        14
#define  CLASS_VIA_KEEPOUT          15
#define  CLASS_ANTI_ETCH            16
#define  CLASS_PIN                  17
#define  CLASS_VIA_CLASS            18
#define  CLASS_MAX_CLASSES          19

// layer flags for connect layer table
#define  LAY_TOP           (-1)
#define  LAY_BOT           (-2)
#define  LAY_INNER         (-3)
#define  LAY_ALL           (-4)

/* Struct Section ***************************************************************************/

typedef struct
{
   CString  TOP_PAD;
   CString  INTERNAL_PAD;
   CString  BOTTOM_PAD;
   CString  CLEARANCE_PAD;
   CString  THERMAL_PAD;
   CString  TOP_SOLDERMASK_PAD;
   CString  BOTTOM_SOLDERMASK_PAD;
   CString  HOLE_NAME;
   double   HOLE_X, HOLE_Y;
   CString  TOP_SOLDERPASTE_PAD;
   CString  BOTTOM_SOLDERPASTE_PAD;
   // temporary storage
   CString  ALL, OUTER;
   CString  SOLDERMASKALL;
   CString  SOLDERPASTEALL;
}VB99Padforms;

typedef struct
{
   int      bnum;
   CString  padname;
   CString  newpadname;
   int      rotation;   // rotation in degree
} VB99PadRotStruct;

typedef CTypedPtrArray<CPtrArray, VB99PadRotStruct*> PadRotArray;

typedef struct
{
   CString  padname;
   CString  vianame;						// via name must be different than a padname
	CString	compPadname;				// comppin padstack name
	CString	fidPadname;					// fiducial padstack name
	CString	mountingPadname;			// mounting hole padstack name
   bool     used_as_via;				// padstack is used as a via definition
   bool     used_as_comp;				// padstack is used as a PIN definition
	bool		used_as_fiducial;			// padstack is used as a fiducial definition
	bool		used_as_mountingHole;	// padstack is used as a mounting hole definition
   bool     smd;							// padstacks only on bottom are treated as SMD with an OPP_SIDE pin_option
   bool     oppside;
   double   drill;
   int      blocknum;
   int      startlayernumber;
   int      endlayernumber;
}VB99Padstack;
typedef CTypedPtrArray<CPtrArray, VB99Padstack*> VB99PadstackArray;

typedef struct
{
   CString  netname;
   POSITION pos;
}VB99Routes;
typedef CTypedPtrArray<CPtrArray, VB99Routes*> VB99RoutesArray;

typedef struct
{
   CString  cellname;
}VB99Cell;
typedef CTypedPtrArray<CPtrArray, VB99Cell*> VB99CellArray;

typedef struct
{
   CString  compname;
   CString  devicename;    
   CString  geomname;
   int      devicefile_written;
}CompList;
typedef CTypedPtrArray<CPtrArray, CompList*> CComplistArray;

static   CComplistArray complistarray;    // this is the device - mapping
static   int         complistcnt;

static   VB99CellArray  celllistarray;    // this is the device - mapping
static   int         celllistcnt;

typedef struct
{
   int      insertnum;
   CString  pinname;
}VB99CompPinInst;
typedef CTypedPtrArray<CPtrArray, VB99CompPinInst*> CompPinInstArray;

class VB99ObstructData : public CObject
{
public:
   Point2 *points;
   int cnt;
   bool polyFilled;
   bool closed;
   bool voidout;
   double lineWidth;
   bool circle;
   CString layerName;

   VB99ObstructData();
   VB99ObstructData(VB99ObstructData &other);
   ~VB99ObstructData();
};

static CTypedObArrayContainer<VB99ObstructData*> obstructsArray;
