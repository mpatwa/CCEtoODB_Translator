// $Header: /CAMCAD/5.0/Api_Find.cpp 27    6/17/07 8:49p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-97. All Rights Reserved.
*/           

#include "stdafx.h"
#include <math.h>
#include "CCEtoODB.h"
#include "API.h"
#include "ccdoc.h"
#include "attrib.h"
#include "drc.h"

extern CCEtoODBView *apiView; // from API.CPP

/******************************************************************************
* FindDataByRefname
*/
short API::FindDataByRefname(LPCTSTR refname, short FAR* geometryNumber, long FAR* dataPosition) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden() || file->notPlacedYet() || file->getBlock() == NULL)
         continue;

      POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = file->getBlock()->getDataList().GetAt(dataPos);

         if (data->getDataType() == T_INSERT && data->getInsert()->getRefname() && !STRICMP(refname, data->getInsert()->getRefname()))
         {
            *geometryNumber = file->getBlock()->getBlockNumber();
            *dataPosition = (long)dataPos;
            return RC_SUCCESS;
         }

         file->getBlock()->getDataList().GetNext(dataPos);
      }
   }

   return RC_ITEM_NOT_FOUND;
}

/******************************************************************************
* FindLayer
*/
short API::FindLayer(LPCTSTR layerName, short FAR* layerNumber) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   for (int i=0; i < doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];
      if (layer == NULL)   continue;
      if (!layer->getName().Compare(layerName))
      {
         *layerNumber = i;
         return RC_SUCCESS;
      }
   }

   return RC_ITEM_NOT_FOUND;
}

/******************************************************************************
* FindVia
*/
short API::FindVia(LPCTSTR NetName, double x, double y, double delta, short FAR* geometryNumber, long FAR* dataPosition) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   Attrib* attrib;
   WORD keyword;

   if ((keyword = doc->IsKeyWord(ATT_NETNAME, 0)) < 0)
      return RC_GENERAL_ERROR;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block != NULL)
      {
         POSITION pos = block->getDataList().GetHeadPosition();
         while (pos != NULL)
         {
            DataStruct *data = block->getDataList().GetAt(pos);

            if (data->getDataType() == T_INSERT)
            {
               if ( (data->getInsert()->getInsertType() == INSERTTYPE_VIA) && 
                     (fabs(data->getInsert()->getOriginX() - x) < delta) && (fabs(data->getInsert()->getOriginY() - y) < delta) )
               {  
                  if ( (data->getAttributesRef()) && (!data->getAttributesRef()->Lookup(keyword, attrib)) )
                  {
                     CString netname = attrib->getStringValue();

                     if (!netname.CompareNoCase(NetName))
                     {  
                        *geometryNumber = block->getBlockNumber();
                        *dataPosition = (long)pos;

                        return RC_SUCCESS;
                     }        
                  }
               }
            }

            block->getDataList().GetNext(pos);
         }
      }
   }

   return RC_ITEM_NOT_FOUND;
}

/******************************************************************************
* FindDRC
*/
short API::FindDRC(long entityNumber, long FAR* filePosition, long FAR* drcPosition) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = doc->getFileList().GetAt(filePos);

      POSITION drcPos = file->getDRCList().GetHeadPosition();
      while (drcPos != NULL)
      {
         DRCStruct *drc = file->getDRCList().GetAt(drcPos);

         if (drc->getEntityNumber() == entityNumber)
         {
            *filePosition = (long)filePos;
            *drcPosition = (long)drcPos;
            return RC_SUCCESS;
         }

         file->getDRCList().GetNext(drcPos);
      }

      doc->getFileList().GetNext(filePos);
   }

   return RC_ITEM_NOT_FOUND;     
}

/******************************************************************************
* FindNet
*/
short API::FindNet(long filePosition, LPCTSTR netname, long FAR* netPosition) 
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

   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      NetStruct *net = file->getNetList().GetAt(netPos);

      if (!net->getNetName().Compare(netname))
      {
         *netPosition = (long)netPos;
         return RC_SUCCESS;
      }

      file->getNetList().GetNext(netPos);
   }

   return RC_ITEM_NOT_FOUND;
}

/******************************************************************************
* FindCompPin
*/
short API::FindCompPin(long filePosition, long netPosition, LPCTSTR comp, LPCTSTR pin, long FAR* cpPosition) 
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

   pos = (POSITION)netPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   NetStruct *net = file->getNetList().GetAt(pos);

   POSITION cpPos = net->getHeadCompPinPosition();
   while (cpPos)
   {
      CompPinStruct *cp = net->getCompPinList().getAt(cpPos);

      if (!cp->getRefDes().Compare(comp) && !cp->getPinName().Compare(pin))
      {
         *cpPosition = (long)cpPos;
         return RC_SUCCESS;
      }

      net->getNextCompPin(cpPos);
   }

   return RC_ITEM_NOT_FOUND;
}

/******************************************************************************
* FindData
*/
short API::FindData(long entityNumber, short FAR* geometryNumber, long FAR* dataPosition) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block != NULL)
      {
      
         POSITION pos = block->getDataList().GetHeadPosition();
         while (pos != NULL)
         {
            DataStruct *data = block->getDataList().GetAt(pos);

            if (data->getEntityNumber() == entityNumber)
            {
               *geometryNumber = block->getBlockNumber();
               *dataPosition = (long)pos;
               return RC_SUCCESS;
            }

            block->getDataList().GetNext(pos);
         }
      }
   }

   return RC_ITEM_NOT_FOUND;
}
