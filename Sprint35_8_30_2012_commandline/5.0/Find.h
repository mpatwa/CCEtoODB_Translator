// $Header: /CAMCAD/5.0/Find.h 21    3/09/07 8:41p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright ©  1994-99. All Rights Reserved.
*/

#if !defined(__Find_h__)
#define __Find_h__

#pragma once

#include "ccdoc.h"
#include "drc.h"  
#include "DcaEntity.h"

enum EEntityType;

CString entityTypeToString(EEntityType tagValue);

// ENTITYNUM
//void *FindEntity(CCEtoODBDoc *doc, long entityNum, EEntityType &entityType);

DataStruct *FindDataEntity(CCEtoODBDoc *doc, long entityNum, CDataList **DataList = NULL, BlockStruct **Block = NULL);

NetStruct *FindNetEntity(CCEtoODBDoc *doc, long entityNum, FileStruct **File);

CompPinStruct *FindCompPinEntity(CCEtoODBDoc *doc, long entityNum, FileStruct **File, NetStruct **Net);

DRCStruct *FindDrcEntity(CCEtoODBDoc *doc, long entityNum, DRCStruct **Drc);

// NAME
LayerStruct *FindLayer_ByName(CCEtoODBDoc *doc, CString layerName);

CompPinStruct *FindCompPin_ByName(CCEtoODBDoc *doc, CString compName, CString pinName, FileStruct **File, NetStruct **Net);

#endif
// end find.h