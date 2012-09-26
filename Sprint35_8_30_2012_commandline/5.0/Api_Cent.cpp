// $Header: /CAMCAD/4.6/Api_Cent.cpp 24    2/14/07 3:59p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "centroid.h"
#include "api.h"
#include "graph.h"

extern CCEtoODBView *apiView; // from API.CPP

/******************************************************************************
* API::SetCentroid
*/
short API::SetCentroid(short geometryNumber, float x, float y, float rotation, short overwrite)
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (geometryNumber < 0 || geometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *block = doc->getBlockAt(geometryNumber);

   if (!block)
      return RC_HOLE_IN_ARRAY;

   if (block->GetCentroidData() != NULL && !overwrite)
      return RC_ITEM_ALREADY_EXISTS;

   doc->SetCentroid(*block, x, y, rotation /*degrees*/);

   return RC_SUCCESS;
}

/******************************************************************************
* API::AutoGenerateCentroid
*/
short API::AutoGenerateCentroid(short geometryNumber, short method, short overwrite)
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (geometryNumber < 0 || geometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *block = doc->getBlockAt(geometryNumber);

   if (!block)
      return RC_HOLE_IN_ARRAY;

   DataStruct *centroidData = block->GetCentroidData();

   if (centroidData && !overwrite)
      return RC_ITEM_ALREADY_EXISTS;

   double x, y;
   if (!CalculateCentroid(doc, block, method, &x, &y))
      return RC_GENERAL_ERROR;

   doc->SetCentroid(*block, x, y, 0.0);

   return RC_SUCCESS;
}

/******************************************************************************
* API::GenerateCompCentroid
*/
short API::GenerateCompCentroid(long filePosition, short method)
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);

	if (GenerateComponentCentroid(doc, file, method) < 0)
		return RC_GENERAL_ERROR;

	return RC_SUCCESS;
}

/******************************************************************************
* API::GetCentroid
*/
short API::GetCentroid(short geometryNumber, float FAR *x, float FAR *y, float FAR *rotation)
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (geometryNumber < 0 || geometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *block = doc->getBlockAt(geometryNumber);

   if (!block)
      return RC_HOLE_IN_ARRAY;

   DataStruct *centroidData = centroid_exist_in_block(block);

   if (!centroidData)
      return RC_ITEM_NOT_FOUND;

   *x = centroidData->getInsert()->getOriginX();
   *y = centroidData->getInsert()->getOriginY();
   *rotation = (float)RadToDeg(centroidData->getInsert()->getAngle());

   return RC_SUCCESS;
}