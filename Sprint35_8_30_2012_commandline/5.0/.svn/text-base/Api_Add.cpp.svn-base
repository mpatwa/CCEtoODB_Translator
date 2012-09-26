// $Header: /CAMCAD/4.5/Api_Add.cpp 23    8/04/06 8:55p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/

#include "stdafx.h"
#include "API.h"
#include "ccview.h"
#include "graph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CCEtoODBView *apiView; // from API.CPP
extern BOOL API_AutoRedraw; // from API.CPP

/******************************************************************************
* CreateLayer
*/
short API::CreateLayer(LPCTSTR name, short FAR* layerNumber) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   LayerStruct *layer = doc->Add_Layer(name);
   *layerNumber = layer->getLayerIndex();

   return RC_SUCCESS;
}

/******************************************************************************
* CreateAperture 
*/
short API::CreateAperture(LPCTSTR name, short shape, double sizeA, double sizeB, double xOffset, double yOffset, 
      double rotation, short dcode, short FAR* widthIndex, short FAR* geometryNumber) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   doc->StoreDocForImporting();

   *widthIndex = Graph_Aperture(name, shape, sizeA, sizeB, xOffset, yOffset, DegToRad(rotation), dcode, 0L, FALSE, NULL);
   *geometryNumber = doc->getWidthTable()[*widthIndex]->getBlockNumber();

   return RC_SUCCESS;
}


/******************************************************************************
* TempAddInsert
*/
short API::TempAddInsert(long filePosition, short geometryNumber, LPCTSTR refName, short insertType,
      double x, double y, double rotation, short mirror, short layerNumber, 
      long FAR* dataPosition) 
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

   if (geometryNumber < 0 || geometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *block = doc->getBlockAt(geometryNumber);

   if (!block)
      return RC_ITEM_NOT_FOUND;

   doc->PrepareAddEntity(file);
   DataStruct *data = Graph_Block_Reference(block->getName(), refName, block->getFileNumber(), 
         x, y, DegToRad(rotation), mirror, 1.0, layerNumber, 0/*int global*/);
   data->getInsert()->setInsertType(insertType);

   *dataPosition = (long)file->getBlock()->getDataList().Find(data);

   if (API_AutoRedraw)
      doc->UpdateAllViews(NULL);

   return RC_SUCCESS;
}


/******************************************************************************
* AddInsert
*/
short API::AddInsert(short geometryNumber, short insertedGeometryNumber, LPCTSTR refName, short insertType, double x, double y, double rotation, short mirror, short layerNumber, long FAR* dataPosition) 
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

   if (insertedGeometryNumber < 0 || insertedGeometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;
   BlockStruct *insertedBlock = doc->getBlockAt(insertedGeometryNumber);

   if (!block || !insertedBlock)
      return RC_ITEM_NOT_FOUND;

   SetCurrentDataList(&block->getDataList());

   DataStruct *data = Graph_Block_Reference(insertedBlock->getName(), refName, insertedBlock->getFileNumber(), 
         x, y, DegToRad(rotation), mirror, 1.0, layerNumber, 0/*int global*/);
   data->getInsert()->setInsertType(insertType);

   *dataPosition = (long)block->getDataList().Find(data);

   // inherit block's attributes
   if (insertedBlock->getAttributesRef())
   {
      Attrib* attrib;
      WORD keyword;

      // loop through block's attributes
      POSITION attribPos = insertedBlock->getAttributesRef()->GetStartPosition();
      while (attribPos)
      {
         insertedBlock->getAttributesRef()->GetNextAssoc(attribPos, keyword, attrib);

         if (!data->getAttributesRef())
            data->getAttributesRef() = new CAttributes();

         // if insert does not have this attrib, add it
         if (!data->getAttributesRef()->Lookup(keyword, attrib)) 
         {
            Attrib *newAttrib = attrib->allocateCopy();
            newAttrib->setInherited(true);     // set the inherited flag
            data->getAttributesRef()->SetAt(keyword, newAttrib);
         }
      }
   }
   
   if (API_AutoRedraw)
      doc->UpdateAllViews(NULL);

   return RC_SUCCESS;
}

/******************************************************************************
* AddPolyEntity
*/
short API::AddPolyEntity(short geometryNumber, short layerNumber, long FAR* dataPosition) 
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

   SetCurrentDataList(&block->getDataList());

   DataStruct *data = Graph_PolyStruct(layerNumber, 0, 0);  

   *dataPosition = (long)block->getDataList().Find(data);

   if (API_AutoRedraw)
      doc->UpdateAllViews(NULL);

   return RC_SUCCESS;
}

/******************************************************************************
* AddPoly
*/
short API::AddPoly(short geometryNumber, long dataPosition, short widthIndex, long FAR* polyPosition) 
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

   POSITION pos = (POSITION)dataPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   DataStruct *data = block->getDataList().GetAt(pos);

   CPoly *poly = Graph_Poly(data, widthIndex, 0, 0, 0);

   *polyPosition = (long)data->getPolyList()->Find(poly);

   return RC_SUCCESS;
}

/******************************************************************************
* AddPolyPnt
*/
short API::AddPolyPnt(short geometryNumber, long dataPosition, long polyPosition, double x, double y, double bulge, long FAR* pntPosition) 
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

   POSITION pos = (POSITION)dataPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   DataStruct *data = block->getDataList().GetAt(pos);

   pos = (POSITION)polyPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   CPoly *poly = data->getPolyList()->GetAt(pos);

   CPnt *pnt = Graph_Vertex(x, y, bulge);

   *pntPosition = (long)poly->getPntList().Find(pnt);

   return RC_SUCCESS;
}


/******************************************************************************
* DeleteEntity
*/
short API::DeleteEntity(short geometryNumber, long dataPosition) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (geometryNumber < 0 || geometryNumber >= doc->getMaxBlockIndex())
      return -1;

   BlockStruct *block = doc->getBlockAt(geometryNumber);

   if (!block)
      return -1;

   // find DATA
   POSITION pos = (POSITION)dataPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   DataStruct *data = block->getDataList().GetAt(pos);

   RemoveOneEntityFromDataList(doc, &block->getDataList(), data);

   return RC_SUCCESS;
}
