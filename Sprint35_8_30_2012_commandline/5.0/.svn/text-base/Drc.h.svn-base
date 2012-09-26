// $Header: /CAMCAD/5.0/Drc.h 27    3/19/07 4:30a Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once

#include "dbutil.h"
#include "DcaDrc.h"

class CAttributes;
class CWriteFormat;
class CCamCadFileWriteProgress;

// Algorithm type -- updated in drc.cpp AlgTypes[]
#define  DRC_ALG_GENERIC                        0
#define  DRC_ALG_PROBABLESHORT_PINtoPIN         1  // there will be other pin to pin distance checks.
                                                   // each entity number if from the COMP_PIN list
#define  DRC_ALG_PROBABLESHORT_PINtoFEATURE     2  // the first is from the COMP_PIN list, the second from the Data
#define  DRC_ALG_PROBABLESHORT_FEATUREtoFEATURE 3  // each entity is from the data

#define  DFT_ALG_NET_WITHOUT_PROBE              4
#define  DFT_ALG_NET_WITHOUT_ACCESS             5
#define  DFT_ALG_COMPONENT_SHADOW               6
#define  DFT_ALG_NET_COLLISION                  7
#define  DFT_ALG_NET_ADJACENCY                  8

#define  DRC_ALG_SPACING_COMPtoCOMP             9
#define  DRC_ALG_SPACING_COMPtoBOARDOUTLINE     10

#define  MAX_DRC_ALG                            11 

/******************************************************************************
* DRC_MeasureStruct
*/

#define  DRC_TYPE_DATALIST    0
#define  DRC_TYPE_COMPPIN     1

class CCEtoODBDoc;
class BlockStruct;
class FileStruct;
class DataStruct;

int GetAlgorithmNameIndex(CCEtoODBDoc *doc, const char *algName);

BlockStruct* GenerateDRCMarker(CCEtoODBDoc *doc);

DRCStruct *CreateDRC(FileStruct *file, const char *string, int drcClass, int failureRange, int algIndex, int algType);

DRCStruct *AddDRC(CCEtoODBDoc *doc, FileStruct *file, 
      double x, double y, const char *string, int drcClass, int failureRange, int algIndex, int algType);

DRCStruct *AddDRCAndMarker(CCEtoODBDoc *doc, FileStruct *file, 
      double x, double y, const char *string, int drcClass, int failureRange, int algIndex, int algType, 
      const char *layerName, DataStruct **insert);

void DRC_FillMeasure(DRCStruct *drc, int type1, long entity1, int type2, long entity2, DbUnit x1, DbUnit y1, DbUnit x2, DbUnit y2);
void DRC_FillNets(DRCStruct *drc, CString netname);

void ClearDRCList(CCEtoODBDoc *doc);
void ClearDRCListForAlogorithm(CCEtoODBDoc *doc,int algorithmIndex);

int RemoveOneDRC(CCEtoODBDoc *doc, DRCStruct *drc, FileStruct *file);
int RemoveOneDRC(CCEtoODBDoc *doc,POSITION drcPos, FileStruct *file);

void ShowAllDRCs(CCEtoODBDoc *doc, BOOL Show);
