// $Header: /CAMCAD/4.6/Api_Set.cpp 29    2/14/07 3:59p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-97. All Rights Reserved.
*/           

#include "stdafx.h"
#include "CCEtoODB.h"
#include "API.h"
#include "ccdoc.h"

extern CCEtoODBView *apiView; // from API.CPP
extern BOOL API_AutoRedraw; // from API.CPP

//void UpdateLayers(CCEtoODBDoc *doc);

/******************************************************************************
* SetLayerType
*/
short API::SetLayerType(short layerNumber, short layerType) 
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

   if (!layer)
      return RC_HOLE_IN_ARRAY;

   layer->setLayerType(layerType);

   return RC_SUCCESS;
}

/******************************************************************************
* SetLayerShow
*     OBSOLETE
*/
short API::SetLayerShow(short layerNumber, short show) 
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

   if (!layer)
      return RC_HOLE_IN_ARRAY;

   layer->setVisible(show);
   layer->setEditable(show);
   //UpdateLayers(doc);

   return RC_SUCCESS;
}

/******************************************************************************
* SetLayerVisible
*/
short API::SetLayerVisible(short layerNumber, BOOL visible) 
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

   if (!layer)
      return RC_HOLE_IN_ARRAY;

   layer->setVisible(visible);
   //UpdateLayers(doc);

   return RC_SUCCESS;
}

/******************************************************************************
* SetLayerEditable
*/
short API::SetLayerEditable(short layerNumber, BOOL editable) 
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

   if (!layer)
      return RC_HOLE_IN_ARRAY;

   layer->setEditable(editable);
   //UpdateLayers(doc);

   return RC_SUCCESS;
}

/******************************************************************************
* SetLayerWorldView
*/
short API::SetLayerWorldView(short layerNumber, BOOL show) 
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

   if (!layer)
      return RC_HOLE_IN_ARRAY;

   layer->setWorldView(show);

   return RC_SUCCESS;
}

/******************************************************************************
* SetLayerMirror
*/
short API::SetLayerMirror(short layerNumber, short mirrorLayerNumber) 
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

   if (!layer)
      return RC_HOLE_IN_ARRAY;

   // clear mirror of layer that was mirrored to this
   doc->getLayerArray()[layer->getMirroredLayerIndex()]->setMirroredLayerIndex( layer->getMirroredLayerIndex());

   // clear mirror of layer that new mirror was mirrored to
   int newMirrorWasMirroredTo = doc->getLayerArray()[mirrorLayerNumber]->getMirroredLayerIndex();
   doc->getLayerArray()[newMirrorWasMirroredTo]->setMirroredLayerIndex( newMirrorWasMirroredTo);

   // new mirror is now mirrored with this
   layer->setMirroredLayerIndex( mirrorLayerNumber);
   doc->getLayerArray()[mirrorLayerNumber]->setMirroredLayerIndex( layer->getLayerIndex());

   return RC_SUCCESS;
}

/******************************************************************************
* SetLayerColor
*/
short API::SetLayerColor(short layerNumber, short red, short green, short blue) 
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

   if (!layer)
      return RC_HOLE_IN_ARRAY;

   layer->setColor( RGB(red, green, blue));

   return RC_SUCCESS;
}

/******************************************************************************
* SetFileShow
*/
short API::SetFileShow(long filePosition, BOOL show) 
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

   file->setShow(show);

   if (API_AutoRedraw)
   {
      doc->UpdateAllViews(NULL);
      doc->OnFitPageToImage();
   }

   return RC_SUCCESS;
}

/******************************************************************************
* SetFileLocation
*/
short API::SetFileLocation(long filePosition, double x, double y) 
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

   file->setInsertX((DbUnit)x);
   file->setInsertY((DbUnit)y);

   if (API_AutoRedraw)
   {
      doc->UpdateAllViews(NULL);
      doc->OnFitPageToImage();
   }

   return RC_SUCCESS;
}

/******************************************************************************
* SetFileRotation
*/
short API::SetFileRotation(long filePosition, double rotation) 
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

   file->setRotation((DbUnit)DegToRad(rotation));

   if (API_AutoRedraw)
   {
      doc->UpdateAllViews(NULL);
      doc->OnFitPageToImage();
   }

   return RC_SUCCESS;
}

/******************************************************************************
* SetFileMirror
*/
short API::SetFileMirror(long filePosition, BOOL mirror) 
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

   file->setMirrored(mirror);

   if (API_AutoRedraw)
   {
      doc->UpdateAllViews(NULL);
      doc->OnFitPageToImage();
   }

   return RC_SUCCESS;
}

/******************************************************************************
* SetFileGeometry
*/
short API::SetFileGeometry(long filePosition, short geometryNumber) 
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

   if (geometryNumber < 0)
      file->setBlock(file->getOriginalBlock());
   else
   {
      BlockStruct *block = doc->Find_Block_by_Num(geometryNumber);

      if (!block)
         return RC_ITEM_NOT_FOUND;

      file->setBlock(block);
   }

   if (API_AutoRedraw)
   {
      doc->UpdateAllViews(NULL);
      doc->OnFitPageToImage();
   }

   return RC_SUCCESS;
}


/******************************************************************************
* SetGeometryName
*/
short API::SetGeometryName(short geometryNumber, LPCTSTR name) 
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

   block->setName(name);

   return RC_SUCCESS;
}

/******************************************************************************
* SetDataInsertInserttype
*/
short API::SetDataInsertInserttype(short geometryNumber, long dataPosition, short inserttype) 
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
      return RC_GENERAL_ERROR;

   // find DATA
   POSITION pos = (POSITION)dataPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   DataStruct *data = block->getDataList().GetAt(pos);

   if (data->getDataType() != T_INSERT)
      return RC_WRONG_TYPE;

   data->getInsert()->setInsertType(inserttype);

   return RC_SUCCESS;
}

/******************************************************************************
* SetDataPolyFilled
*/
short API::SetDataPolyFilled(short geometryNumber, long dataPosition, long polyPosition, short filled) 
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
      return RC_GENERAL_ERROR;

   // find DATA
   POSITION pos = (POSITION)dataPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   DataStruct *data = block->getDataList().GetAt(pos);

   // find POLY
   pos = (POSITION)polyPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   CPoly *poly = data->getPolyList()->GetAt(pos);

   poly->setFilled(filled);

   return RC_SUCCESS;
}

/******************************************************************************
* SetDataPolyClosed
*/
short API::SetDataPolyClosed(short geometryNumber, long dataPosition, long polyPosition, short closed) 
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
      return RC_GENERAL_ERROR;

   // find DATA
   POSITION pos = (POSITION)dataPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   DataStruct *data = block->getDataList().GetAt(pos);

   // find POLY
   pos = (POSITION)polyPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   CPoly *poly = data->getPolyList()->GetAt(pos);

   poly->setClosed(closed);

   return RC_SUCCESS;
}

/******************************************************************************
* SetDataPolyVoid
*/
short API::SetDataPolyVoid(short geometryNumber, long dataPosition, long polyPosition, short voidPoly) 
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
      return RC_GENERAL_ERROR;

   // find DATA
   POSITION pos = (POSITION)dataPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   DataStruct *data = block->getDataList().GetAt(pos);

   // find POLY
   pos = (POSITION)polyPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   CPoly *poly = data->getPolyList()->GetAt(pos);

   poly->setVoid(voidPoly);

   return RC_SUCCESS;
}

/******************************************************************************
* SetDataPolyWidthIndex
*/
short API::SetDataPolyWidthIndex(short geometryNumber, long dataPosition, long polyPosition, short widthIndex) 
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
      return RC_GENERAL_ERROR;

   // find DATA
   POSITION pos = (POSITION)dataPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   DataStruct *data = block->getDataList().GetAt(pos);

   // find POLY
   pos = (POSITION)polyPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   CPoly *poly = data->getPolyList()->GetAt(pos);

   poly->setWidthIndex(widthIndex);

   return RC_SUCCESS;
}

/******************************************************************************
* SetDataGraphicClass
*/
short API::SetDataGraphicClass(short geometryNumber, long dataPosition, short graphicClass) 
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
      return RC_GENERAL_ERROR;

   // find DATA
   POSITION pos = (POSITION)dataPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   DataStruct *data = block->getDataList().GetAt(pos);

   if (data->getDataType() == T_INSERT)
      return RC_WRONG_TYPE;

   data->setGraphicClass(graphicClass);

   return RC_SUCCESS;
}
