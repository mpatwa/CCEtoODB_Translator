// $Header: /CAMCAD/4.6/Extents.h 21    2/14/07 4:02p Kurt Van Ness $

/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/

#if !defined(__Extents_h__)
#define __Extents_h__

#pragma once

#include "ccdoc.h"

// returns TRUE is an extens was calculated, a FALSE if not like in a TOOL or APERTURE
bool block_extents(CCEtoODBDoc *doc,double *lxmin,double *lxmax,
              double *lymin,double *lymax,    // Pointers of extention
              CDataList *DataList,            // Start pointer of data
              double insert_x,double insert_y,
              double rotation,int mirror,
              double scale,
              int insertLayer,BOOL onlyVisible);

bool UseInsertExtents(double *lxmin,double *lxmax,double *lymin,double *lymax,BlockStruct *block,
      double insert_x,double insert_y,double rotation,int mirror,double scale);
CExtent UseInsertExtents(BlockStruct *block,
      double insert_x,double insert_y,double rotation,int mirror,double scale);

CExtent TextBox(CCEtoODBDoc* doc,const char* text,
      Point2* point,double width,double height,double rot,int mirror,BOOL proportional);

//bool TextExtents(CCEtoODBDoc *doc,DataStruct *np,ExtentRect *extents,double scale,int mirror,
//      double insert_x,double insert_y,double rotation);

// use_width is TRUE, when the polywidth is part of the extens, FALSE if the line/arc center should be used.
bool PolyExtents(CCEtoODBDoc *doc,CPolyList *polylist,ExtentRect *extents,double scale,int mirror,
      double insert_x,double insert_y,Mat2x2 *m,int use_width);
CExtent PolyExtents(CCEtoODBDoc *doc,CPolyList *polylist,double scale,int mirror,
      double insert_x,double insert_y,Mat2x2 *m,int use_width);

bool PolyPntExtents(CPntList *pntList,ExtentRect *extents,double scale,int mirror,
      double insert_x,double insert_y,Mat2x2 *m,double width);
bool PolyPntExtents(CPntList *pntList,CExtent& extent,double scale,int mirror,
      double insert_x,double insert_y,Mat2x2 *m,double width);

CExtent VisibleAttribsExtents(CCEtoODBDoc *doc,CAttributes* map,Point2* pnt,
      double block_scale,double block_rot,int block_mir);

CExtent VisibleAttribExtents(CCEtoODBDoc* doc,Attrib* attrib,Point2* pnt,
      double block_scale,double block_rot,int block_mir);

#endif

// end EXTENTS.H