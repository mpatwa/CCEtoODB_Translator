// $Header: /CAMCAD/4.3/read_wrt/ViewDrIn.h 7     8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once

/* Defines Section ***************************************************************************/

#define VIEWDRAWERR  "viewdraw.log"

#define  MAX_JOINT   2000   
#define  MAX_LINE    255

/* Struct Section ***************************************************************************/

typedef  struct
{
   CString  name;       // name.version
   CString  libname;
   CString  sheetname;
} VIEWInsert;
typedef CTypedPtrArray<CPtrArray, VIEWInsert*> CInsertArray;

typedef  struct
{
   int      netnr;
   CString  netname;
} VIEWNetName;
typedef CTypedPtrArray<CPtrArray, VIEWNetName*> CNetNameArray;

typedef  struct
{
   CString  path;
   CString  libname;
} VIEWLibpath;
typedef CTypedPtrArray<CPtrArray, VIEWLibpath*> CLibpathArray;

typedef struct
{
   double   x,y;
   int      typ;
}Joint;
