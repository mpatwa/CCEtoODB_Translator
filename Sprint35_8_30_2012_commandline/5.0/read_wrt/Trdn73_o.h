// $Header: /CAMCAD/4.3/read_wrt/Trdn73_o.h 7     8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#include <afxtempl.h>

typedef struct
{
   CString  refName;
   CString  geomName;
   int      geomNum;
   int      bottom;
   double   x, y;
   double   rotation;
   int      doPins;
} TEComp;
typedef CTypedPtrArray<CPtrArray, TEComp*> TECompArray;

typedef struct
{
   CString  geomName;
   double   centroid_x, centroid_y;
   double   centroid_rotation;
} TEGeom;
typedef CTypedPtrArray<CPtrArray, TEGeom*> TEGeomArray;

typedef struct
{
   CString  refName;
   CString  geomName;
   CString  fiducialRef;
   double   x, y;
   int      layerFlag;     // 1 = top, 2 bottom, 3 all
} TEPanelFiducial;
typedef CTypedPtrArray<CPtrArray, TEPanelFiducial*> TEPanelFiducialArray;

typedef struct
{
   CString  refName;
   CString  geomName;
   int      geomNum;
   double   x, y;
   double   rotation;
   int      mirror;
} TEPanelBoard;
typedef CTypedPtrArray<CPtrArray, TEPanelBoard*> TEPanelBoardArray;
