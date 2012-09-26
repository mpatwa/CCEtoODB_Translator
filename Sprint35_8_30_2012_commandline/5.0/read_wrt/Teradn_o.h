// $Header: /CAMCAD/4.3/read_wrt/Teradn_o.h 6     8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

typedef struct
{
   CString  name;       // list of components in the datalist
   CString  geomname;
   int      bottom;
   double   x, y;
   double   rotation;
} TEComp;
typedef CTypedPtrArray<CPtrArray, TEComp*> TECompArray;

typedef struct
{
   CString  name;       // list of pcb blocks in the datalist
   double   centroid_x, centroid_y;
   double   centroid_rotation;
} TEGeom;
typedef CTypedPtrArray<CPtrArray, TEGeom*> TEGeomArray;

typedef struct
{
   CString  name;       // list of pcb blocks in the datalist
   CString  geomName;
   CString  fiducialRef;
   double   x, y;
   int      layerFlag;     // 1 = top, 2 bottom, 3 all
} TEPanelFiducial;
typedef CTypedPtrArray<CPtrArray, TEPanelFiducial*> TEPanelFiducialArray;

typedef struct
{
   CString  name;       // list of pcb blocks in the datalist
   CString  geomName;
   CString  rejectRef;
   double   x, y;
   int      layerFlag;     // 1 = top, 2 bottom, 3 all
} TEPanelReject;
typedef CTypedPtrArray<CPtrArray, TEPanelReject*> TEPanelRejectArray;

typedef struct
{
   CString  refname;
   CString  geomName;         // list of pcb blocks in the datalist
   int      geomNum;
   double   board_x, board_y;
   double   board_rotation;
   int      board_mirror;
   BOOL     Unique;           // mirrored boards on a panel makes it unique
} TEPanelBoard;
typedef CTypedPtrArray<CPtrArray, TEPanelBoard*> TEPanelBoardArray;

typedef CArray<Point2, Point2&> CPolyArray;
