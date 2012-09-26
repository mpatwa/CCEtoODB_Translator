// $Header: /CAMCAD/4.4/read_wrt/Men_out.h 10    3/21/04 3:44a Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#define  MAX_LAYERS        100
#define  MEN_SMALLNUMBER   0.0001      // accurricy is written with 1.4

// layer flags for connect layer table
#define  LAY_TOP           -1
#define  LAY_BOT           -2
#define  LAY_INNER         -3
#define  LAY_ALL           -4
#define  LAY_POWER         -5

typedef struct
{
   CString  name;
   int      stacking_num;
   CString  type;
   long     color;
   int      width; 
   int      fill; 
   int      patt;  
   int      trans;  
   int      path; 
   int      space; 
   int      style; 
   int      text;   
   int      pen; 
   int      hilite;
   int      protect; 
   int      select;

   void setName(const CString& Name) { name = Name; }
} MENLayerrepStruct;
typedef CTypedPtrArray<CPtrArray, MENLayerrepStruct*> LayerRepArray;

typedef struct
{
   CString  vianame;
} MENViaStruct;

typedef CTypedPtrArray<CPtrArray, MENViaStruct*> ViaArray;

typedef struct
{
   CString  padname;
   CString  newpadname;
   int      rotation;   // rotation in degree
} MENPadRotStruct;

typedef CTypedPtrArray<CPtrArray, MENPadRotStruct*> PadRotArray;

typedef struct
{
   double x1,y1,x2,y2;
}MENPinExtent;

typedef struct
{
   CString  padstackname;
   CString  usedpadname;   // use either original or UNIQUE name
   int      typ;           // 1, 2, 4
   int      used_as_via;
   int      top;
   int      inner;
   int      bot;
   CUIntArray  padElecLayerList;
   int      padElecLayerCount;
   UINT64   layermap;      // normal layer
   UINT64   mirrormap;     // mirrored layer (done for speed reasons once)
} MENPadStackStruct;

typedef CTypedPtrArray<CPtrArray, MENPadStackStruct*> PadStackArray;

typedef  CArray<Point2, Point2&> CPolyArray;
