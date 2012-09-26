// $Header: /CAMCAD/DcaLib/DcaBbLib.h 1     3/09/07 5:10p Kurt Van Ness $

#if !defined(__DcaBbLib_h__)
#define __DcaBbLib_h__

#pragma once


#include "DcaData.h"

// Building Block includes
// needed for BuildingBlock software
extern "C"
{
#undef __
#define __(x) x

#include "t2defs.h"
}

C2_CURVE BB_CreateCurve(double x1, double y1, double x2, double y2, double bulge);

C2_CURVE BB_CreatePCurve(CPntList *pntList);

DML_LIST BB_CreateCurveList(CPntList *pntList);

T2_REGION BB_CreateRegion(CPntList *pntList);

void BB_free_region(ANY region);

CString BB_DebugPCurve(C2_CURVE pcurve);

CPntList *BuildUnion(CPolyList *partList);
CPntList *BuildUnion(CPntList *pntList1, CPntList *pntList2);

DML_LIST BB_Union(CPolyList *polyList);
DML_LIST BB_Union(CPntList *pntList1, CPntList *pntList2);

CPntList *BB_ExtractResults(DML_LIST rgnList);

// end BB_LIB.H

#endif