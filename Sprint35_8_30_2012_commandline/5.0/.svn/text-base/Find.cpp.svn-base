// $Header: /CAMCAD/5.0/Find.cpp 27    3/09/07 2:35p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright ©  1994-99. All Rights Reserved.
*/

#include "StdAfx.h"
#include "Find.h" 

/******************************************************************************
* FindEntity
*/
//void *FindEntity(CCEtoODBDoc *doc, long entityNum, EEntityType &entityType)
//{
//   void *voidPtr;
//
//   if (voidPtr = FindDataEntity(doc, entityNum, NULL, NULL))
//   {
//      entityType = entityTypeData;
//      return voidPtr;
//   }
//
//   if (voidPtr = FindNetEntity(doc, entityNum, NULL))
//   {
//      entityType = entityTypeNet;
//      return voidPtr;
//   }
//
//   if (voidPtr = FindCompPinEntity(doc, entityNum, NULL, NULL))
//   {
//      entityType = entityTypeCompPin;
//      return voidPtr;
//   }
//
//   return NULL;
//}

/******************************************************************************
* FindDataEntity
*/
DataStruct *FindDataEntity(CCEtoODBDoc *doc, long entityNum, CDataList **DataList, BlockStruct **Block)
{
   if (!entityNum)   return NULL;

   for (int i=0; i < doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;      // holes in blocklist !

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);
         if (data->getEntityNumber() == entityNum)
         {
            if (DataList)
               *DataList = &block->getDataList();
            if (Block)
               *Block = block;
            return data;
         }
      }
   }

   return NULL;
}

/******************************************************************************
* FindNetEntity
*/
NetStruct *FindNetEntity(CCEtoODBDoc *doc, long entityNum, FileStruct **File)
{
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      POSITION netPos = file->getNetList().GetHeadPosition();
      while (netPos != NULL)
      {
         NetStruct *net = file->getNetList().GetNext(netPos);

         if (net->getEntityNumber() == entityNum)
         {
            if (File)
               *File = file;
            return net;
         }
      }
   }

   return NULL;
}

/******************************************************************************
* FindDrcEntity
*/
DRCStruct *FindDrcEntity(CCEtoODBDoc *doc, long entityNum, DRCStruct **Drc)
{
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      POSITION drcPos = file->getDRCList().GetHeadPosition();
      while (drcPos)
      {
         DRCStruct *drc = file->getDRCList().GetNext(drcPos);

         if (drc->getEntityNumber() == entityNum)
         {
            if (Drc)
               *Drc = drc;
            return drc;
         }
      }
   }

   return NULL;
}

/******************************************************************************
* FindCompPinEntity
*/
CompPinStruct *FindCompPinEntity(CCEtoODBDoc *doc, long entityNum, FileStruct **File, NetStruct **Net)
{
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      POSITION netPos = file->getNetList().GetHeadPosition();
      while (netPos != NULL)
      {
         NetStruct *net = file->getNetList().GetNext(netPos);

         POSITION cpPos = net->getHeadCompPinPosition();
         while (cpPos != NULL)
         {
            CompPinStruct *cp = net->getNextCompPin(cpPos);

            if (cp->getEntityNumber() == entityNum)
            {
               if (File)
                  *File = file;
               if (Net)
                  *Net = net;
               return cp;
            }
         }
      }
   }

   return NULL;
}

/******************************************************************************
* FindCompPin_ByName
*/
CompPinStruct *FindCompPin_ByName(CCEtoODBDoc *doc, CString compName, CString pinName, FileStruct **File, NetStruct **Net)
{
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      POSITION netPos = file->getNetList().GetHeadPosition();
      while (netPos)
      {
         NetStruct *net = file->getNetList().GetNext(netPos);

         POSITION cpPos = net->getHeadCompPinPosition();
         while (cpPos)
         {
            CompPinStruct *cp = net->getNextCompPin(cpPos);

            if (cp->getRefDes() == compName && cp->getPinName() == pinName)
            {
               if (File)
                  *File = file;
               if (Net)
                  *Net = net;
               return cp;
            }
         }
      }
   }

   return NULL;
}

/******************************************************************************
* FindLayer_ByName
*/
LayerStruct *FindLayer_ByName(CCEtoODBDoc *doc, CString layerName)
{
   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];

      if (!layer) 
         continue;

      if (layerName == layer->getName())
         return layer;
   }

   return NULL;
}






