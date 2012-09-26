// $Header: /CAMCAD/4.3/read_wrt/Hp5DxIn.h 10    8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#include <afxtempl.h>
#include "file.h"

#define  MAX_LINE                2000  /* Max line length.           */
#define  TEXT_HEIGHTFACTOR       (1.0/1.4)   // if a text is .12 is HP3070 ASCII, it is really only
                                       // 0.1 in graphic
#define  TEXT_WIDTHFACTOR        (0.55)   

#define  FID_ATT                 1
#define  SMD_ATT                 2
#define  DEVICE_ATT              3

#define  HP5DXERR    "5dx.log"

typedef struct 
{
   int      shapeIndex;
   CString  pinname;
   CString  padstack;
   double   x, y;
   BOOL     SMD;
} HP5DXShapePin;
typedef CTypedPtrArray<CPtrArray, HP5DXShapePin*> HP5DXShapePinArray;

typedef struct 
{
   int      packageIndex;
   CString  pinname;
   CString  apertureName;
   double   pitch;      // not pinpitch
   CString  family;     // pin family
   double   x, y;
   BOOL     HasRotation;
   double   rotation;
} HP5DXPackagePin;
typedef CTypedPtrArray<CPtrArray, HP5DXPackagePin*> HP5DXPackagePinArray;

typedef struct 
{
   CString topName;
   CString botName;
   CString topHash;
   CString botHash;
   FileStruct *boardFile;
   double boardWidth;
   double boardHeight;

   int topRot;
   int botRot;

   BOOL SingleSided;

   BOOL Found;
} HP5DXBoardDef;
typedef CTypedPtrArray<CPtrArray, HP5DXBoardDef*> HP5DXBoardDefArray;

typedef struct 
{
   int boardIndex;
   DataStruct *pcbInsert;
   BOOL BottomSideOnly;
} HP5DXBoardInsert;
typedef CTypedPtrArray<CPtrArray, HP5DXBoardInsert*> HP5DXBoardInsertArray;

