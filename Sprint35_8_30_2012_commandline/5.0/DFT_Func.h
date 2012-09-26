// $Header: /CAMCAD/5.0/DFT_Func.h 12    3/19/07 4:30a Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once

#include <afxtempl.h>
#include "ccdoc.h"

int DFT_TestProbeSequence(CCEtoODBDoc *doc, FileStruct *file, int algorithm, 
                          int startingPoint, const char *prefix, double bandwidth, 
                          int startNumber, int nameIncrement);

int DFT_CreateGeomDFxOutline(CCEtoODBDoc *doc, BlockStruct *block,int algorithm, int overwrite);
