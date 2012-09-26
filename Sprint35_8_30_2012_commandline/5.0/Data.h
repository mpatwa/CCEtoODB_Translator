// $Header: /CAMCAD/5.0/Data.h 167   6/15/07 7:39p Kurt Van Ness $

/*****************************************************************************/
/*  
    Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/ 

#if !defined(__Data_h__)
#define __Data_h__

#pragma once

#include <afxtempl.h>
#include "dbutil.h"
#include "Extent.h"
#include "TypedContainer.h"
#include "TMState.h"
#include "Dca.h"
#include "DcaData.h"
#include "DcaHorizontalPosition.h"
#include "DcaVerticalPosition.h"
#include "DcaDataType.h"
#include "DcaInsert.h"
#include "DcaPoly.h"
#include "DcaText.h"
#include "DcaBlob.h"

//#if defined(__CcDoc_h__)
//#error CcDoc.h included in __FILE__
//#endif

class CWriteFormat;
class CAttributes;
class CCamCadFileWriteProgress;
//class Attrib;
class CPolygon;
class Point2;

// TYPES 
//  - used in data structure
#define T_DRAW          0  
#define T_INSERT        1  
#define T_POLY          2
#define T_TEXT          3
#define T_TOOL          4  // this is no longer used !
#define T_POINT         5
#define T_BLOB          6
#define T_POLY3D        7

class CTMatrix;
class CPoint2dList;
class CBasesVector;
class LayerStruct;

int propagateLayer(int parentLayer,int childLayer);

#if !defined(EnableDcaCamCadDocLegacyCode)
//_____________________________________________________________________________
class ExtentRect 
{
public:
   double left, top, right, bottom;

   ExtentRect();
   ExtentRect& operator=(const CExtent& extent);
   void reset();

   bool isCorrupt();
   bool isValid();
};
#endif

#endif
