
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "xform.h"
#include "graph.h"
#include "apertur2.h"
#include "net_util.h"
#include "dfm.h"
#include "crypt.h"
#include "api.h"
#include "CCEtoODB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CCEtoODBView *apiView; // from API.CPP

void FlattenBlock(CCEtoODBDoc *doc, BlockStruct *block, FileStruct *file, int flattenLayer, CDataList *datalist,
      BOOL TopLevelOnly, int graphicClass, DTransform *xform, int insertLayer, CString refdes, CString netname, CAttributes* attribMap);

void FlattenBlockByInserttype(CCEtoODBDoc *doc, BlockStruct *block, FileStruct *file, int flattenLayer, CDataList *datalist,
      DTransform *xform, int insertLayer, int inserttype, short surface, 
      BOOL FlatteningByAttrib, WORD attribKeyword, CString attribValue, int comparisonType,
      BOOL InsertPointOnly, 
      long entityNumberOfFoundInsert, 
      CString refdes, CString netname, short inCur_surface, CAttributes* attribMap);

BOOL AttribMatch(CCEtoODBDoc *doc, CAttributes* map, int keyword, CString attribValueString, int comparisonType);

/******************************************************************************
* DFM::CreateFlattenGeometry
*/
SHORT DFM::CreateFlattenGeometry(LONG filePosition, SHORT* geometryNumber)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDfm)) //&& !get_license(LIC_VISION_DFM) ) 
   {
      ErrorAccess("You do not have a License for DFM!");
      return RC_NO_LICENSE;
   }*/

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);

   doc->StoreDocForImporting();

   BlockStruct *block;
   CString blockname;
   blockname.Format("%s_%d", file->getName(), doc->getMaxBlockIndex());
   block = Graph_Block_Exists(doc, blockname, file->getFileNumber());
   if (block)
      return RC_GENERAL_ERROR;

   block = doc->Add_Blockname(blockname, file->getFileNumber(), BL_FILE, FALSE); 
   SetCurrentDataList(&block->getDataList());

   doc->RegisterKeyWord(ATT_DFMCOMP_REFNAME, 0, VT_STRING);

   *geometryNumber = block->getBlockNumber();

   return RC_SUCCESS;
}

/******************************************************************************
* DFM::Flatten
*/
SHORT DFM::Flatten(SHORT geometryNumber, SHORT layerNumber, SHORT TopLevelOnly)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDfm)) // && !get_license(LIC_VISION_DFM) ) 
   {
      ErrorAccess("You do not have a License for DFM!");
      return RC_NO_LICENSE;
   }*/

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (geometryNumber < 0 || geometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *flattenGeometry = doc->getBlockAt(geometryNumber);

   if (!flattenGeometry)
      return RC_HOLE_IN_ARRAY;

   FileStruct *file = doc->Find_File(flattenGeometry->getFileNumber());

   DTransform xform;
	FlattenBlock(doc, file->getOriginalBlock(), file, layerNumber, &flattenGeometry->getDataList(), TopLevelOnly, -1, &xform, -1, "", "", NULL);

   return RC_SUCCESS;
}

/******************************************************************************
* DFM::FlattenByGraphicClass
*/
SHORT DFM::FlattenByGraphicClass(SHORT geometryNumber, SHORT layerNumber, SHORT graphicClass, SHORT TopLevelOnly)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDfm)) // && !get_license(LIC_VISION_DFM) ) 
   {
      ErrorAccess("You do not have a License for DFM!");
      return RC_NO_LICENSE;
   }*/

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (geometryNumber < 0 || geometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *flattenGeometry = doc->getBlockAt(geometryNumber);

   if (!flattenGeometry)
      return RC_HOLE_IN_ARRAY;

   FileStruct *file = doc->Find_File(flattenGeometry->getFileNumber());

   DTransform xform;
   FlattenBlock(doc, file->getOriginalBlock(), file, layerNumber, &flattenGeometry->getDataList(), TopLevelOnly, graphicClass, &xform, -1, "", "", NULL);

   return RC_SUCCESS;
}

/******************************************************************************
* DFM::FlattenByInserttype
*/
SHORT DFM::FlattenByInserttype(SHORT geometryNumber, SHORT layerNumber, SHORT inserttype, SHORT InsertPointOnly, SHORT surface)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDfm)) // && !get_license(LIC_VISION_DFM) ) 
   {
      ErrorAccess("You do not have a License for DFM!");
      return RC_NO_LICENSE;
   }*/

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (geometryNumber < 0 || geometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *flattenGeometry = doc->getBlockAt(geometryNumber);

   if (!flattenGeometry)
      return RC_HOLE_IN_ARRAY;

   FileStruct *file = doc->Find_File(flattenGeometry->getFileNumber());

   // prep block tag for optimization
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (!block)
         continue;
      block->setMarked(1);
   }

   DTransform xform;
   FlattenBlockByInserttype(doc, file->getOriginalBlock(), file, layerNumber, &flattenGeometry->getDataList(), &xform, -1, 
         inserttype, surface, FALSE, 0, "", 0, InsertPointOnly, -1, "", "", 0, NULL);

   return RC_SUCCESS;
}

/******************************************************************************
* DFM::FlattenByAttrib
*/
SHORT DFM::FlattenByAttrib(SHORT geometryNumber, SHORT layerNumber,
		LPCTSTR attribKeyword, LPCTSTR attribValue, SHORT comparisonType, 
      SHORT inserttype, SHORT InsertPointOnly, SHORT surface)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDfm)) // && !get_license(LIC_VISION_DFM) ) 
   {
      ErrorAccess("You do not have a License for DFM!");
      return RC_NO_LICENSE;
   }*/

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (geometryNumber < 0 || geometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *flattenGeometry = doc->getBlockAt(geometryNumber);

   if (!flattenGeometry)
      return RC_HOLE_IN_ARRAY;

   FileStruct *file = doc->Find_File(flattenGeometry->getFileNumber());

   WORD keyword = doc->IsKeyWord(attribKeyword, 0);

   DTransform xform;

   // prep block tag for optimization
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (!block)
         continue;
      block->setMarked(1);
   }

   if (keyword == -1)
   {
      switch (comparisonType)
      {
      case 4:
      case 6: // required to not have the keyword, but we already know nothing has the keyword, so we do a basic flatten by inserttype
         FlattenBlockByInserttype(doc, file->getOriginalBlock(), file, layerNumber, &flattenGeometry->getDataList(), &xform, -1, inserttype, surface,
               FALSE, 0, "", 0, 
               InsertPointOnly, -1, "", "", 0, NULL);
         return RC_SUCCESS;

      default: // keyword required, but we know nothing has it because the keyword does not exist
         return RC_SUCCESS;
      }
   }

   FlattenBlockByInserttype(doc, file->getOriginalBlock(), file, layerNumber, &flattenGeometry->getDataList(), &xform, -1, inserttype, surface, 
         TRUE, keyword, attribValue, comparisonType, 
         InsertPointOnly, -1, "", "", 0, NULL);

   return RC_SUCCESS;
}

/******************************************************************************
* FlattenBlock
*/
void FlattenBlock(CCEtoODBDoc *doc, BlockStruct *block, FileStruct *file, int flattenLayer, CDataList *datalist,
      BOOL TopLevelOnly, int graphicClass, DTransform *xform, int insertLayer, CString refdes, CString netname, CAttributes* attribMap)
{
   CWaitCursor wait;

   WORD netnameKeyword = doc->IsKeyWord(ATT_NETNAME, 0);
   WORD refnameKeyword = doc->IsKeyWord(ATT_REFNAME, 0);
   WORD compRefnameKeyword = doc->IsKeyWord(ATT_DFMCOMP_REFNAME, 0);

   POSITION pos = block->getHeadDataPosition();
   while (pos)
   {
      DataStruct *data = block->getNextData(pos);
      
		BlockStruct *subblock = NULL;
      if (data->getDataType() == T_INSERT)
      {
         if (TopLevelOnly)
            continue;
      
         subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
      }
      else if (graphicClass > -1 && data->getGraphicClass() != graphicClass)
         continue;

      // Figure Layer
      int layerNumber = data->getLayerIndex();
      if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
         layerNumber = insertLayer;

      if (data->getDataType() != T_INSERT || 
            ((subblock->getFlags() & BL_TOOL || subblock->getFlags() & BL_BLOCK_TOOL) && !subblock->getToolDisplay()) || 
            ((subblock->getFlags() & BL_APERTURE || subblock->getFlags() & BL_BLOCK_APERTURE) && subblock->getShape() != T_COMPLEX))
      {
         if (layerNumber < 0)
            continue;

         if (xform->mirror & MIRROR_LAYERS)
         {
            LayerStruct *layer = doc->getLayerAt(doc->getLayerAt(layerNumber)->getMirroredLayerIndex());
            if (layer->getNeverMirror())
               continue;
            layerNumber = layer->getLayerIndex();
         }
         else 
         {
            LayerStruct *layer = doc->getLayerAt(layerNumber);
            if (layer->getMirrorOnly())
               continue;
            layerNumber = layer->getLayerIndex();
         }

         if (flattenLayer != -1 && layerNumber != flattenLayer)
            continue;
      }

      switch (data->getDataType())
      {
      case T_POLY:
         {
            DataStruct *newData = CopyTransposeEntity(data, xform->x, xform->y, xform->rotation, xform->mirror, xform->scale, insertLayer, FALSE,data->getEntityNumber());
            //newData->setEntityNumber(data->getEntityNumber());
            datalist->AddTail(newData);

				// Case 1181: make sure we use a zero line width so we are measuring from the lines center
				if (graphicClass == graphicClassBoardOutline)
					newData->getPolyList()->setWidthIndex(0);

				doc->CopyAttribs(&newData->getAttributesRef(), attribMap);
            doc->CopyAttribs(&newData->getAttributesRef(), data->getAttributesRef());

            if (!netname.IsEmpty())
               doc->SetAttrib(&newData->getAttributesRef(), netnameKeyword, VT_STRING, (void*)(LPCTSTR)netname, SA_OVERWRITE, NULL);
            if (!refdes.IsEmpty())
               doc->SetAttrib(&newData->getAttributesRef(), refnameKeyword, VT_STRING, (void*)(LPCTSTR)refdes, SA_OVERWRITE, NULL);
         }
         break;

      case T_INSERT:
         {
            // calculate point
            Point2 point2;
            point2.x = data->getInsert()->getOrigin().x;
            point2.y = data->getInsert()->getOrigin().y;
            point2.bulge = 0;
            xform->TransformPoint(&point2);

            BOOL 
               DrawAp = FALSE, 
               DrawSubblock = FALSE,
               IsTool = (subblock->getFlags() & BL_TOOL) || (subblock->getFlags() & BL_BLOCK_TOOL),
               IsAp = (subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE);
            double 
               _x = point2.x,
               _y = point2.y,
               _rot = xform->rotation,
               _scale = xform->scale;
            int 
               _mirror = xform->mirror;
            int 
               _layer = layerNumber;

            // TOOL
            if (IsTool)
            {
               subblock = doc->Find_Block_by_Num(subblock->getToolBlockNumber());

               if ((subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE))
                  DrawAp = TRUE;
               else
                  DrawSubblock = TRUE;
            } // end TOOL

            // APERTURE
            if (IsAp || DrawAp)
            {
               // COMPLEX APERTURE
               if (subblock->getShape() == T_COMPLEX)
               {
                  // subblock is complex aperture insert (only looking at block's values ex. sizeA & rotation)
                  //_x = ;
                  //_y = ;
                  _rot += subblock->getRotation();

                  // now change subblock to aperture definition block (looking at block definition ex. polys & circles)
                  subblock = doc->Find_Block_by_Num((int)(subblock->getSizeA()));
                  DrawSubblock = TRUE;
               }

               // NORMAL APERTURE
               else
               {
                  if (subblock->getShape() == T_UNDEFINED)
                  {
                  }
                  else
                  {
                     if (graphicClass > -1)
                        continue;

                     CPolyList *polylist = ApertureToPolyTranspose(subblock, data->getInsert(), 
                           xform->x, xform->y, xform->rotation, xform->scale, xform->mirror);
                     if (polylist)
                     {
                        DataStruct *newData = Graph_PolyStruct(layerNumber, 0, FALSE,data->getEntityNumber());
                        //newData->setEntityNumber(data->getEntityNumber());
                        delete newData->getPolyList();

                        doc->CopyAttribs(&newData->getAttributesRef(), attribMap);
                        doc->CopyAttribs(&newData->getAttributesRef(), data->getAttributes());
                        newData->getPolyList() = polylist;

                        if (!netname.IsEmpty())
                           doc->SetAttrib(&newData->getAttributesRef(), netnameKeyword, VT_STRING, (void*)(LPCTSTR)netname, SA_OVERWRITE, NULL);
                        if (!refdes.IsEmpty())
                           doc->SetAttrib(&newData->getAttributesRef(), refnameKeyword, VT_STRING, (void*)(LPCTSTR)refdes, SA_OVERWRITE, NULL);
                     }
                  }
               } // end NORMAL APERTURE
            } // end APERTURE

            // GRAPHIC BLOCK
            if (!IsAp && !IsTool) 
            {
               DrawSubblock = TRUE;

               if (xform->mirror & MIRROR_FLIP)
                  _rot = xform->rotation - data->getInsert()->getAngle();
               else
                  _rot = xform->rotation + data->getInsert()->getAngle();

               _mirror = xform->mirror ^ data->getInsert()->getMirrorFlags();
               _scale = xform->scale * data->getInsert()->getScale();
            }


            // Draw Block
            if (DrawSubblock)
            {
               DTransform subXForm(_x, _y, _scale, _rot, _mirror);

               CString newNetname = netname;
               Attrib* attrib = NULL;

               if (data->getAttributes() && data->getAttributes()->Lookup(netnameKeyword, attrib))
               {
                  newNetname = attrib->getStringValue();
               }

               if (data->getInsert()->getInsertType() == INSERTTYPE_PIN)
               {
                  NetStruct *net = NULL;

                  if (FindCompPin(file, refdes, data->getInsert()->getRefname(), &net))
                     newNetname = net->getNetName();
               }

               CString newRefdes;

               if (!refdes.IsEmpty())
               {
                  newRefdes = refdes;
                  newRefdes += ",";
               }

               newRefdes += data->getInsert()->getRefname();

               CAttributes newAttribMap;
               CAttributes* newAttribMapPtr = &newAttribMap;
               doc->CopyAttribs(&newAttribMapPtr, attribMap);
               doc->CopyAttribs(&newAttribMapPtr, data->getAttributes());

               if ((data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT || data->getInsert()->getInsertType() == INSERTTYPE_TOOLING ||
                    data->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL) &&
                    strlen(data->getInsert()->getRefname()))
                  doc->SetAttrib(&newAttribMapPtr, compRefnameKeyword, VT_STRING, (void*)(LPCTSTR)data->getInsert()->getRefname(), SA_OVERWRITE, NULL);

               FlattenBlock(doc, subblock, file, flattenLayer, datalist, TopLevelOnly, graphicClass, &subXForm, layerNumber, newRefdes, newNetname, &newAttribMap);

               newAttribMap.empty();
            } 
         }

         break;
      }
   }
}

/******************************************************************************
* FlattenBlockByInserttype
*/
void FlattenBlockByInserttype(CCEtoODBDoc *doc, BlockStruct *block, FileStruct *file, int flattenLayer, CDataList *datalist,
      DTransform *xform, int insertLayer, int inserttype, short surface, 
      BOOL FlatteningByAttrib, WORD attribKeyword, CString attribValue, int comparisonType,
      BOOL InsertPointOnly, 
      long entityNumberOfFoundInsert, 
      CString refdes, CString netname, short inCur_surface, CAttributes* attribMap)
{
   CWaitCursor wait;

   WORD netnameKeyword = doc->IsKeyWord(ATT_NETNAME, 0);
   WORD refnameKeyword = doc->IsKeyWord(ATT_REFNAME, 0);
   WORD compRefnameKeyword = doc->IsKeyWord(ATT_DFMCOMP_REFNAME, 0);

   POSITION pos = block->getHeadDataPosition();
   while (pos)
   {
      DataStruct *data = block->getNextData(pos);

      BlockStruct *subblock;
      if (data->getDataType() == T_INSERT)
         subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

		// if no insert has been found, make sure we're checking the correct surface
		int cur_surface = 0;
		if (entityNumberOfFoundInsert == -1 && data->getDataType() == T_INSERT)
		{
			// top && top = top
			// bottom && bottom = top
			cur_surface = (
								(!(data->getInsert()->getMirrorFlags() & MIRROR_ALL) && !(inCur_surface & MIRROR_ALL)) ||
								((data->getInsert()->getMirrorFlags() & MIRROR_ALL) && (inCur_surface & MIRROR_ALL))
								)?0:MIRROR_ALL;

			// if top mounted and not looking for top
			if (!(cur_surface & MIRROR_ALL) && !(surface & 0x0001))
				continue;
			// if bottom mounted and looking for bottom
			if ((cur_surface & MIRROR_ALL) && !(surface & 0x0002))
				continue;
		}

		// Figure Layer
      int layerNumber = data->getLayerIndex();
      if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
         layerNumber = insertLayer;

      if (data->getDataType() != T_INSERT || 
            ((subblock->getFlags() & BL_TOOL || subblock->getFlags() & BL_BLOCK_TOOL) && !subblock->getToolDisplay()) || 
            ((subblock->getFlags() & BL_APERTURE || subblock->getFlags() & BL_BLOCK_APERTURE) && subblock->getShape() != T_COMPLEX))
      {
         if (entityNumberOfFoundInsert < 0)
            continue;

         if (layerNumber < 0)
            continue;

         LayerStruct *layer = doc->getLayerAt(layerNumber);
         LayerStruct *mirrorLayer = doc->getLayerAt(layer->getMirroredLayerIndex());
         if (flattenLayer == -1 || layer->getLayerIndex() == flattenLayer || mirrorLayer->getLayerIndex() == flattenLayer)
            block->setMarked(1);

         if (xform->mirror & MIRROR_LAYERS)
         {
            if (mirrorLayer->getNeverMirror())
               continue;
            layerNumber = mirrorLayer->getLayerIndex();
         }
         else 
         {
            if (layer->getMirrorOnly())
               continue;
            layerNumber = layer->getLayerIndex();
         }

         if (flattenLayer != -1 && layerNumber != flattenLayer)
            continue;

         if (InsertPointOnly)
         {
            DataStruct *newData = Graph_PolyStruct(layerNumber, 0, FALSE,entityNumberOfFoundInsert);
            //newData->setEntityNumber(entityNumberOfFoundInsert);
            Graph_Poly(newData, 0, FALSE, FALSE, FALSE);
            Graph_Vertex(xform->x, xform->y, 0);

            doc->CopyAttribs(&newData->getAttributesRef(), attribMap);
            doc->CopyAttribs(&newData->getAttributesRef(), data->getAttributes());

            if (!netname.IsEmpty())
               doc->SetAttrib(&newData->getAttributesRef(), netnameKeyword, VT_STRING, (void*)(LPCTSTR)netname, SA_OVERWRITE, NULL);
            if (!refdes.IsEmpty())
               doc->SetAttrib(&newData->getAttributesRef(), refnameKeyword, VT_STRING, (void*)(LPCTSTR)refdes, SA_OVERWRITE, NULL);

            block->setMarked(1);
            continue;
         }
      }

      switch (data->getDataType())
      {
      case T_POLY:
         {
            DataStruct *newData = CopyTransposeEntity(data, xform->x, xform->y, xform->rotation, xform->mirror, xform->scale, insertLayer, FALSE,entityNumberOfFoundInsert);
            //newData->setEntityNumber(entityNumberOfFoundInsert);
            datalist->AddTail(newData);

            doc->CopyAttribs(&newData->getAttributesRef(), attribMap);
            doc->CopyAttribs(&newData->getAttributesRef(), data->getAttributes());

            if (!netname.IsEmpty())
               doc->SetAttrib(&newData->getAttributesRef(), netnameKeyword, VT_STRING, (void*)(LPCTSTR)netname, SA_OVERWRITE, NULL);
            if (!refdes.IsEmpty())
               doc->SetAttrib(&newData->getAttributesRef(), refnameKeyword, VT_STRING, (void*)(LPCTSTR)refdes, SA_OVERWRITE, NULL);

            block->setMarked(1);
         }
         break;

      case T_INSERT:
         {
            // calculate point
            Point2 point2;
            point2.x = data->getInsert()->getOrigin().x;
            point2.y = data->getInsert()->getOrigin().y;
            point2.bulge = 0;
            xform->TransformPoint(&point2);

            BOOL 
               DrawAp = FALSE, 
               DrawSubblock = FALSE,
               IsTool = (subblock->getFlags() & BL_TOOL) || (subblock->getFlags() & BL_BLOCK_TOOL),
               IsAp = (subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE);
            double 
               _x = point2.x,
               _y = point2.y,
               _rot = xform->rotation,
               _scale = xform->scale;
            int 
               _mirror = xform->mirror;
            int 
               _layer = layerNumber;

            // TOOL
            if (IsTool)
            {
               subblock = doc->Find_Block_by_Num(subblock->getToolBlockNumber());
                      
               if ((subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE))
                  DrawAp = TRUE;
               else
                  DrawSubblock = TRUE;
            } // end TOOL

            // APERTURE
            if (IsAp || DrawAp)
            {
               // COMPLEX APERTURE
               if (subblock->getShape() == T_COMPLEX)
               {
                  // subblock is complex aperture insert (only looking at block's values ex. sizeA & rotation)
                  //_x = ;
                  //_y = ;
                  _rot += subblock->getRotation();

                  // now change subblock to aperture definition block (looking at block definition ex. polys & circles)
                  subblock = doc->Find_Block_by_Num((int)(subblock->getSizeA()));
                  DrawSubblock = TRUE;
               }

               // NORMAL APERTURE
               else
               {
                  if (subblock->getShape() != T_UNDEFINED)
                  {
                     CPolyList *polylist = ApertureToPolyTranspose(subblock, data->getInsert(), 
                           xform->x, xform->y, xform->rotation, xform->scale, xform->mirror);
                     if (polylist)
                     {
                        DataStruct *newData = Graph_PolyStruct(layerNumber, 0, FALSE,entityNumberOfFoundInsert);
                        //newData->setEntityNumber(entityNumberOfFoundInsert);
                        delete newData->getPolyList();
                        newData->getPolyList() = polylist;

                        doc->CopyAttribs(&newData->getAttributesRef(), attribMap);
                        doc->CopyAttribs(&newData->getAttributesRef(), data->getAttributes());

                        if (!netname.IsEmpty())
                           doc->SetAttrib(&newData->getAttributesRef(), netnameKeyword, VT_STRING, (void*)(LPCTSTR)netname, SA_OVERWRITE, NULL);
                        if (!refdes.IsEmpty())
                           doc->SetAttrib(&newData->getAttributesRef(), refnameKeyword, VT_STRING, (void*)(LPCTSTR)refdes, SA_OVERWRITE, NULL);

                        block->setMarked(1);
                     }
                  }
               } // end NORMAL APERTURE
            } // end APERTURE

            // GRAPHIC BLOCK
            if (!IsAp && !IsTool) 
            {
               DrawSubblock = TRUE;

               if (xform->mirror & MIRROR_FLIP)
                  _rot = xform->rotation - data->getInsert()->getAngle();
               else
                  _rot = xform->rotation + data->getInsert()->getAngle();

               _mirror = xform->mirror ^ data->getInsert()->getMirrorFlags();
               _scale = xform->scale * data->getInsert()->getScale();
            }


            // Draw Block
            if (DrawSubblock)
            {
               if (!subblock->getMarked() && !FlatteningByAttrib)
					{
						// only continue if no top level floating layers
						bool dataOnFloatingLayersFound = false;
						POSITION subDataPos = subblock->getHeadDataPosition();
						while (subDataPos)
						{
							DataStruct *subData = subblock->getNextData(subDataPos);
							LayerStruct *subDataLayer = doc->getLayerAt(subData->getLayerIndex());
							
							if (subDataLayer && subDataLayer->getFloating())
							{
								dataOnFloatingLayersFound = true;
								break;
							}
						}

						if (!dataOnFloatingLayersFound)
							continue;
					}

               DTransform subXForm(_x, _y, _scale, _rot, _mirror);

               CString newNetname = netname;
               Attrib* attrib;

               if (data->getAttributes() && data->getAttributes()->Lookup(netnameKeyword, attrib))
               {
                  newNetname = attrib->getStringValue();
               }

               int newEntityNumberOfFoundInsert = entityNumberOfFoundInsert;

               if (data->getInsert()->getInsertType() == INSERTTYPE_PIN)
               {
                  NetStruct *net;
                  CompPinStruct *compPin = FindCompPin(file, refdes, data->getInsert()->getRefname(), &net);
                  if (compPin)
                  {
                     newNetname = net->getNetName();

                     if (inserttype == -1 || data->getInsert()->getInsertType() == inserttype)
                     {
                        if (!FlatteningByAttrib)
                           newEntityNumberOfFoundInsert = compPin->getEntityNumber();
                        else if (newEntityNumberOfFoundInsert >= 0)
                           newEntityNumberOfFoundInsert = compPin->getEntityNumber();
                        else if (AttribMatch(doc, compPin->getAttributesRef(), attribKeyword, attribValue, comparisonType))
                           newEntityNumberOfFoundInsert = compPin->getEntityNumber();
                     }
                  }
               }

               if (newEntityNumberOfFoundInsert == -1 && (inserttype == -1 || data->getInsert()->getInsertType() == inserttype) )
               {
						if (data->getInsert()->getInsertType() == insertTypeRealPart)
						{
							BlockStruct *rpBlock = doc->getBlockAt(data->getInsert()->getBlockNumber());
							DataStruct *pkgData = NULL;
							if (rpBlock != NULL && (pkgData = rpBlock->GetPackage()))
							{
								if (!FlatteningByAttrib || 
										(FlatteningByAttrib && 
											(AttribMatch(doc, pkgData->getAttributes(), attribKeyword, attribValue, comparisonType) || // this item
											AttribMatch(doc, attribMap, attribKeyword, attribValue, comparisonType)) // items above this
										)
									)
									newEntityNumberOfFoundInsert = data->getEntityNumber();
							}
						}
						else
						{
							if (!FlatteningByAttrib || 
									(FlatteningByAttrib && 
										(AttribMatch(doc, data->getAttributes(), attribKeyword, attribValue, comparisonType) || // this item
										AttribMatch(doc, attribMap, attribKeyword, attribValue, comparisonType)) // items above this
									)
								)
								newEntityNumberOfFoundInsert = data->getEntityNumber();
						}
               }


               CString newRefdes;
               if (!refdes.IsEmpty())
               {
                  newRefdes = refdes;
                  newRefdes += ",";
               }
               newRefdes += data->getInsert()->getRefname();

               CAttributes newAttribMap;
               CAttributes* newAttribMapPtr = &newAttribMap;
               doc->CopyAttribs(&newAttribMapPtr, attribMap);
               doc->CopyAttribs(&newAttribMapPtr, data->getAttributes());

               if ((data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT || data->getInsert()->getInsertType() == INSERTTYPE_TOOLING ||
                    data->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL || data->getInsert()->getInsertType() == insertTypeRealPart) &&
                    strlen(data->getInsert()->getRefname()))
                  doc->SetAttrib(&newAttribMapPtr, compRefnameKeyword, VT_STRING, (void*)(LPCTSTR)data->getInsert()->getRefname(), SA_OVERWRITE, NULL);

               subblock->setMarked(0);

               FlattenBlockByInserttype(doc, subblock, file, flattenLayer, datalist, &subXForm, layerNumber, inserttype, surface, 
                     FlatteningByAttrib, attribKeyword, attribValue, comparisonType, 
                     InsertPointOnly, 
                     newEntityNumberOfFoundInsert, newRefdes, newNetname, cur_surface, &newAttribMap);

					// Case 1125 - we need to reset the mark if this insert wasn't found to need flattening
					if (newEntityNumberOfFoundInsert == -1)
						subblock->setMarked(1);

               if (subblock->getMarked())
                  block->setMarked(1);

               newAttribMap.empty();
            } 
         }
         break;
      }
   }
}

/******************************************************************************
* AttribMatch
   | Has KW | value
-2 |  yes   |  <=
-1 |  yes   |  <
0  |  yes   |  ==
1  |  yes   |  >
2  |  yes   |  >=
3  |  yes   |  !=
4  |        |  !=  // does not have kw/value pair
5  |  yes   | don't care  // has kw
6  |  no    | don't care  // does not have kw
*/
BOOL AttribMatch(CCEtoODBDoc *doc, CAttributes* map, int keyword, CString attribValueString, int comparisonType)
{
   Attrib* attrib;

   if (!map || !map->Lookup(keyword, attrib)) // does not have kw
      return comparisonType == 4 || comparisonType == 6;

   if (comparisonType == 5)
      return TRUE;

   if (comparisonType == 6)
      return FALSE;

   double value = 0;

   switch (attrib->getValueType())
   {
   case VT_NONE:
      return TRUE;

   case VT_STRING:
      {
         switch (comparisonType)
         {
            case 0: // value == attribValue
               return (!attribValueString.CompareNoCase(attrib->getStringValue()));

/*          case 1: // value >= attribValue
               return value > attribValue - SMALLNUMBER;

            case 2: // value > attribValue
               return value > attribValue + SMALLNUMBER;

            case -1: // value <= attribValue
               return value < attribValue + SMALLNUMBER;

            case -2: // value < attribValue
               return value < attribValue - SMALLNUMBER;*/

            case 3: // value != attribValue
            case 4: // does not have kw/value pair
               return (attribValueString.CompareNoCase(attrib->getStringValue()));

            default:
               return TRUE;
         }
      }

   case VT_INTEGER:
      value = attrib->getIntValue();
      break;

   case VT_DOUBLE:
   case VT_UNIT_DOUBLE:
      value = attrib->getDoubleValue();
      break;
   }

   double attribValue = atof(attribValueString);
   
   switch (comparisonType)
   {
      case 0: // value == attribValue
         return fabs(value - attribValue) < SMALLNUMBER;

      case 1: // value >= attribValue
         return value > attribValue - SMALLNUMBER;

      case 2: // value > attribValue
         return value > attribValue + SMALLNUMBER;

      case -1: // value <= attribValue
         return value < attribValue + SMALLNUMBER;

      case -2: // value < attribValue
         return value < attribValue - SMALLNUMBER;

      case 3: // value != attribValue
      case 4: // does not have kw/value pair
         return fabs(value - attribValue) > SMALLNUMBER;

      default:
         return TRUE;
   }
}

/******************************************************************************
* DFM::FilterGeometry
*/
SHORT DFM::FilterGeometry(SHORT geometryNumber, LPCTSTR keyword, LPCTSTR value, SHORT comparisonType)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDfm)) 
   {
      ErrorAccess("You do not have a License for DFM!");
      return RC_NO_LICENSE;
   }*/

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (geometryNumber < 0 || geometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *geometry = doc->getBlockAt(geometryNumber);

   if (!geometry)
      return RC_HOLE_IN_ARRAY;

   WORD keywordIndex = doc->IsKeyWord(keyword, 0);

   POSITION dataPos = geometry->getHeadDataPosition();
   while (dataPos)
   {
      DataStruct *data = geometry->getNextData(dataPos);

      if (AttribMatch(doc, data->getAttributes(), keywordIndex, value, comparisonType))
         RemoveOneEntityFromDataList(doc, &geometry->getDataList(), data);
   }

   return RC_SUCCESS;
}
