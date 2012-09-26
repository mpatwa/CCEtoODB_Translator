
/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/           

#include "stdafx.h"
#include "ccdoc.h"
#include "attrib.h"
#include "polylib.h"
#include "mem.h"
#include "drc.h"

/******************************************************************************
* FreeAttribs()
*
* DESCRIPTION  : Empties AttribMap
*/
//void FreeAttribs(CAttributes* map)
//{
//   if (map != NULL)
//   {
//      map->empty();
//   }
//}

/******************************************************************************
* FreeNetList()
*
* DESCRIPTION  : Empties NetList
*/
//void FreeNetList(FileStruct *file)
//{
//   // free NetList
//   POSITION netPos = file->getNetList().GetHeadPosition();
//   while (netPos != NULL)
//   {
//      NetStruct *net = file->getNetList().GetNext(netPos);
//      FreeNet(net);
//   }
//
//   file->getNetList().RemoveAll();
//}

/******************************************************************************
* FreeNet()
*
* DESCRIPTION  : Frees memory for a net (net must be removed from NetList)
*/
//void FreeNet(NetStruct *net)
//{
//   if (net->getAttributesRef())
//   {
//      delete net->getAttributesRef();
//      net->getAttributesRef() = NULL;
//   }
//
//   // free CompPinList
//   POSITION compPinPos = net->getHeadCompPinPosition();
//   while (compPinPos != NULL)
//   {
//      CompPinStruct *compPin = net->getNextCompPin(compPinPos);
//      FreeCompPin(compPin);
//   }
//
//   delete net;
//	net = NULL;
//}

/******************************************************************************
* FreeCompPin()
*
* DESCRIPTION  : Frees memory for a compPin (compPin must be removed from Net)
*/
//void FreeCompPin(CompPinStruct *compPin)
//{
//   if (compPin->getAttributesRef())
//   {
//      delete compPin->getAttributesRef();
//      compPin->getAttributesRef() = NULL;
//   }
//
//   delete compPin;
//	compPin = NULL;
//}

/******************************************************************************
* FreeData
*
* DESCRIPTION  : Frees memory for a data (must be removed from Block
*/
//void FreeData(DataStruct*& data)
//{
   // check also in ccdoc.cpp (FreeBlock())
   //if (data->getDataType() == T_TEXT)
   //{
   //   free(data->getText()->getText());
   //   delete data->getText();
   //   data->getText() = NULL;
   //}
   //if (data->getDataType() == T_INSERT)
   //{
   //   if (data->getInsert()->refname)
   //      free(data->getInsert()->refname);
   //   delete data->getInsert();
   //   data->getInsert() = NULL;
   //}
   //if (data->getDataType() == T_POLY)
   //   FreePolyList(data->getPolyList());
   //if (data->getAttributesRef())
   //{
   //   FreeAttribs(data->getAttributesRef());
   //   delete data->getAttributesRef();
   //}
//   delete data;
//   data = NULL;
//}

/******************************************************************************
* CCAMCADDoc::FreeFile()
*
* DESCRIPTION  : Frees memory for a file (file must be removed from FileList)
*/
//void CCAMCADDoc::FreeFile(FileStruct *file, bool freeBlocks)
//{
//   FreeNetList(file);
//
//   // free TypeList
//   POSITION typePos = file->getTypeList().GetHeadPosition();
//   while (typePos != NULL)
//   {
//      TypeStruct *type = file->getTypeList().GetNext(typePos);
//      if (type->getAttributesRef())
//      {
//         delete type->getAttributesRef();
//         type->getAttributesRef() = NULL;
//      }
//
//      // free CompPinList
//      POSITION typePinPos = type->getTypePinList().GetHeadPosition();
//      while (typePinPos != NULL)
//      {
//         TypePinStruct *typePin = type->getTypePinList().GetNext(typePinPos);
//         if (typePin->getAttributesRef())
//         {
//            delete typePin->getAttributesRef();
//            typePin->getAttributesRef() = NULL;
//         }
//         delete typePin;
//      }
//      type->getTypePinList().RemoveAll();
//
//      delete type;
//   }
//   file->getTypeList().RemoveAll();
//
//   // free DRCList
//   POSITION drcPos = file->getDRCList().GetHeadPosition();
//   while (drcPos != NULL)
//   {
//      DRCStruct *drc = file->getDRCList().GetNext(drcPos);
//      if (drc->getAttributesRef())
//      {
//         delete drc->getAttributesRef();
//         drc->getAttributesRef() = NULL;
//      }
//      switch (drc->drcClass)
//      {
//      case DRC_CLASS_SIMPLE:
//      case DRC_CLASS_NO_MARKER:
//         break;
//      case DRC_CLASS_MEASURE:
//         if (drc->voidPtr)
//            delete (DRC_MeasureStruct*)drc->voidPtr;
//         break;
//      case DRC_CLASS_NETS:
//         if (drc->voidPtr)
//            delete (CStringList*)drc->voidPtr;
//         break;
//      };
//      delete drc;
//   }
//   file->getDRCList().RemoveAll();
//
//	if (freeBlocks)
//	{
//		// Free all blocks in this file
//		for (int i=0; i<getMaxBlockIndex(); i++)
//		{
//			BlockStruct *block = getBlockAt(i);
//			if (block == NULL)
//				continue;
//			if (block->getFileNumber() != file->getFileNumber())
//				continue;
//
//			FreeBlock(block);
//		}
//	}
//
//   delete file;
//}

/******************************************************************************
* CCAMCADDoc::FreeBlock()
*
* DESCRIPTION  : Frees memory for a block (block must be removed from BlockList)
*/
void CCAMCADDoc::FreeBlock(BlockStruct *block)
{
   //DataStruct* data;

   //delete block->getAttributesRef();
   //block->getAttributesRef() = NULL;

   //// free DataList  
   //for (POSITION dataPos = block->getDataList().GetHeadPosition();dataPos != NULL;)
   //{
   //   data = block->getDataList().GetNext(dataPos);

   //   delete data;
   //}

   //block->getDataList().RemoveAll();

   //getBlockArray().deleteBlock(block);

   delete block;
}

/******************************************************************************
* CCAMCADDoc::FreeLayer()
*
* DESCRIPTION  : Frees memory for a layer (layer must be removed from LayerArray)
*/
//void CCAMCADDoc::FreeLayer(LayerStruct*& layer)
//{
//   if (layer != NULL)
//   {
//      if (layer->getAttributesRef())
//      {
//         delete layer->getAttributesRef();
//         layer->getAttributesRef() = NULL;
//      }
//
//      delete layer;
//   }
//
//   layer = NULL;
//}

