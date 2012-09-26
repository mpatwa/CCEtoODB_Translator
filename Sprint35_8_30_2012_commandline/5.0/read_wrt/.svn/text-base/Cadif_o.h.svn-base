// $Header: /CAMCAD/4.3/read_wrt/Cadif_o.h 8     8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once

int CADIF_LoadTechnology(const char *filename);

#define  TECHNOLOGY_NAME   "CADSTAR"
#define  FONT_INDEX        0

// layer flags for connect layer table
#define  LAY_TOP           -1
#define  LAY_BOT           -2
#define  LAY_INNER         -3
#define  LAY_ALL           -4

typedef struct
{
   CString  filename;
   int      signalcnt;                 // 1, 2, 4
   int      powercnt;
} CADIFTechnologyStruct;

typedef CTypedPtrArray<CPtrArray, CADIFTechnologyStruct*> TechnologyArray;

typedef struct
{
   CString  padstackname;
   int      typ;                 // 1, 2, 4
   int      used_as_via;
   int      pcnum;
   UINT64   layermap;      // normal layer
   UINT64   mirrormap;     // mirrored layer (done for speed reasons once)
} CADIFPadStackStruct;
typedef CTypedPtrArray<CPtrArray, CADIFPadStackStruct*> PadStackArray;

typedef struct
{
   int   bnum;
   int   rot;
   int   degreeAngle;
} CADIFPadShapeStruct;

typedef CTypedPtrArray<CPtrArray, CADIFPadShapeStruct*> PadShapeArray;

typedef struct
{
   int   bnum;
   int   rot;
} CADIFPadSymStruct;

typedef CTypedPtrArray<CPtrArray, CADIFPadSymStruct*> PadSymArray;

typedef struct
{
   CString  padname;
   CString  newpadname;
   int      bnum;
   int      pcnum;      // PC%d number
   int      rotation;   // rotation in degree
} CADIFPadCodeRotStruct;

typedef CTypedPtrArray<CPtrArray, CADIFPadCodeRotStruct*> PadCodeRotArray;

typedef struct
{
   int      layerindex;
   CString  layername;
   CString  layertype;           // redac cadif fixed - see layer_usage[]
   CString  originallayertype;   // enhanced for camcad recognition.
   CString  swaplayer;
   CString  material;
   double   thickness;
   CString  embedding;
   CString  routingbias;
   int      stacknum;
} CADIFLayerrepStruct;
typedef CTypedPtrArray<CPtrArray, CADIFLayerrepStruct*> LayerRepArray;

typedef struct
{
   CString  vianame;
   int      totalviacnt;
} CADIFViaStruct;

typedef CTypedPtrArray<CPtrArray, CADIFViaStruct*> ViaArray;

typedef struct
{
   int   index;
} CADIFEndPointStruct;

typedef CTypedPtrArray<CPtrArray, CADIFEndPointStruct*> EndPointArray;

typedef struct
{
   long  x,y;
   int   layer;
} CADIFJunctionStruct;

typedef CTypedPtrArray<CPtrArray, CADIFJunctionStruct*> JunctionArray;

typedef struct
{
   CString  compname;
   int      compindex;
   CString  pinname;
   int      pinindex;
} CADIFCompPinStruct;

typedef CTypedPtrArray<CPtrArray, CADIFCompPinStruct*> CompPinArray;

typedef struct
{
   CString  netname;
} CADIFNetStruct;

typedef CTypedPtrArray<CPtrArray, CADIFNetStruct*> NetArray;

typedef struct
{
   CString  name;
} CADIFCompStruct;

typedef CTypedPtrArray<CPtrArray, CADIFCompStruct*> CompArray;

typedef struct
{
   CString  name;
   CString  cadiflayer; // L1, L50 etc...
   int      block_num;
   int      layertype;
   int      electricalstacknumber;
   int      cclayer;
   double   rotation;
} CADIFPadLayerStruct;

typedef CTypedPtrArray<CPtrArray, CADIFPadLayerStruct*> PadLayerArray;

typedef struct
{
   int      mirror;
   double   height;
   double   charwidth;
   double   rotation;
} CADIFTextStruct;

typedef CTypedPtrArray<CPtrArray, CADIFTextStruct*> TextArray;

static   char* layer_usage[] = 
{
   "ASSY_DWG",
   "BOTTOM_RESIST",
   "CLEARANCE",
   "COVERGLAZE",
   "DOCUMENT",
   "DRILL_DWG",
   "DRILL_ID",
   "ELECTRICAL",
   "LAMINATE",
   "NO_DIEL",
   "NO_TRACKS",
   "NO_VIAS",
   "OTHER_USAGE",
   "PLACEMENT",
   "POWER_PLANE",
   "PROFILING",
   "PROHIBITED",
   "RESERVED",
   "RESISTOR",
   "SHEET_DIEL",
   "SILKSCREEN",
   "SPLIT_PLANE",
   "THERM_BOX",
   "TOP_RESIST",
   "UNLAYERED",
   "WIREBOND",
   "XOVER_DIEL",
};
#define  SIZ_LAYER_USAGE   (sizeof(layer_usage) / sizeof(char *))




