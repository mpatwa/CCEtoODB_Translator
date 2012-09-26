// $Header: /CAMCAD/4.6/Api_Del.cpp 20    2/14/07 3:59p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "api.h"
#include "ccdoc.h"
#include "graph.h"
#include "lyrmanip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CCEtoODBView *apiView; // from API.CPP

/******************************************************************************
* DeleteGeometry
*  Hopefully no other blocks reference this block
*/
short API::DeleteGeometry(short geometryNumber) 
{
   CCEtoODBDoc *doc = apiView->GetDocument();

   if (geometryNumber < 0 || geometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *block = doc->getBlockAt(geometryNumber);

   if (!block)
      return RC_GENERAL_ERROR;

   doc->FreeBlock(block);

   //doc->getBlockAt(geometryNumber) = NULL;

   return RC_SUCCESS;
}

/******************************************************************************
* DeleteByLayer
*/
short API::DeleteByLayer(short layerNumber, BOOL RemoveLayerFromLayerList) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (layerNumber < 0 || layerNumber >= doc->getMaxLayerIndex())
      return RC_INDEX_OUT_OF_RANGE;

   LayerStruct *layer = doc->getLayerArray()[layerNumber];

   DeleteLayer(doc, layer, RemoveLayerFromLayerList);

   return RC_SUCCESS;
}

/******************************************************************************
* DeleteByInserttype
*/
short API::DeleteByInserttype(short geometryNumber, short inserttype) 
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
      return RC_ITEM_NOT_FOUND;

   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() == inserttype)
         RemoveOneEntityFromDataList(doc, &block->getDataList(), data);
   }

   return RC_SUCCESS;
}

/******************************************************************************
* DeleteByGraphicClass
*/
short API::DeleteByGraphicClass(short geometryNumber, short graphicClass) 
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
      return RC_ITEM_NOT_FOUND;

   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      if (data->getDataType() == T_INSERT)
         continue;

      if (data->getGraphicClass() == graphicClass)
         RemoveOneEntityFromDataList(doc, &block->getDataList(), data);
   }

   return RC_SUCCESS;
}

void delete_visible_root_graphic(CCEtoODBDoc *doc,bool componentsFlag,bool graphicsFlag);

/******************************************************************************
* DeleteGenericGraphic
*/
short API::DeleteGenericGraphic(BOOL components, BOOL graphics)
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   delete_visible_root_graphic(doc, components != 0, graphics != 0);

   return RC_SUCCESS;
}

void delete_traces(CCEtoODBDoc *doc, int traces, int vias);
/******************************************************************************
* DeleteTraceVias
*/
short API::DeleteTracesVias(short traces, short vias)
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   delete_traces(doc, traces, vias);

   return RC_SUCCESS;
}

