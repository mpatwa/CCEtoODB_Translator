// $Header: /CAMCAD/4.3/Dfm_Dist.h 14    12/04/03 6:12p Alvin $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once

#include "file.h"
#include "ccdoc.h"

void DFM_CompShadow(CCEtoODBDoc *doc, FileStruct *file, 
      double marLeftMargin, double marRightMargin, double marTopMargin, double marBottomMargin,
      double crtLeftMargin, double crtRightMargin, double crtTopMargin, double crtBottomMargin);

struct DiffNetsStruct
{
   int net1, net2;
   long entity1, entity2;
   double x1, y1, x2, y2;
   double distance;
   int failureRange;
   CString failureRangeString;
   double checkValue;
};
typedef CTypedPtrList<CPtrList, DiffNetsStruct*> CDiffNetsList;

typedef CMap <void*, void*, ExtentRect, ExtentRect&> CExtentsMap;

class CViolationsMapList : CTypedMapWordToObContainer<CWordArray*>
{
public:
	CViolationsMapList():CTypedMapWordToObContainer<CWordArray*>() {};
	~CViolationsMapList() {};

	bool hasBeenViolated(long entityNum1, long entityNum2);
	void AddViolation(long entityNum1, long entityNum2);
};