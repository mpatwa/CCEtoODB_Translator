// $Header: /CAMCAD/5.0/Api_Sch.cpp 38    6/17/07 8:49p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "API.h"
#include "ccview.h"
#include "attrib.h"
#include "Sch_Lib.h"

extern CCEtoODBView *apiView; // from API.CPP

#define DELIM (char)5

/******************************************************************************
* SetSchematicNetCrossProbe
*/
short API::SetSchematicNetCrossProbe(BOOL On) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBApp *app = (CCEtoODBApp*)AfxGetApp();

   if (!app->getSchematicLinkController().isSchematicLinkInSession())
      return RC_NOT_ALLOWED;

   app->getSchematicLinkController().setNetCrossProbeOn(On==TRUE);

   return RC_SUCCESS;
}

/******************************************************************************
* SetSchematicComponentCrossProbe
*/
short API::SetSchematicComponentCrossProbe(BOOL On) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBApp *app = (CCEtoODBApp*)AfxGetApp();
   if (!app->schLinkState)
      return RC_NOT_ALLOWED;

   if (!app->getSchematicLinkController().isSchematicLinkInSession())
      return RC_NOT_ALLOWED;

   app->getSchematicLinkController().setComponentCrossProbeOn(On==TRUE);

   return RC_SUCCESS;
}

/******************************************************************************
* SetSchematicSheet
*/
short API::SetSchematicSheet(long filePosition, short geometryNumber) 
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
      return RC_HOLE_IN_ARRAY;

   file->setBlock(block);

   return RC_SUCCESS;
}

/******************************************************************************
* GetSchematicViewArray
*/
short API::GetSchematicViewArray(VARIANT FAR* fileArray) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   int count = 0;
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (file->getBlockType() == BLOCKTYPE_SHEET)
         count++;
   }

   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(fileArray);
   fileArray->vt = VT_ARRAY | VT_I4;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I4, 0, count); 

   long i = 0;
   filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      POSITION tempPos = filePos;
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (file->getBlockType() == BLOCKTYPE_SHEET)
      {
         SafeArrayPutElement(psa, &i, &tempPos);
         i++;
      }
   }

   fileArray->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetSchematicSheetArray
*/
short API::GetSchematicSheetArray(long filePosition, VARIANT FAR* sheetArray) 
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

   int count = 0;
	long i=0;
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block && block->getBlockType() == BLOCKTYPE_SHEET && block->getFileNumber() == file->getFileNumber())
         count++;
   }

   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(sheetArray);
   sheetArray->vt = VT_ARRAY | VT_I4;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I4, 0, count); 

   long index = 0;
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block && block->getBlockType() == BLOCKTYPE_SHEET && block->getFileNumber() == file->getFileNumber())
      {
         SafeArrayPutElement(psa, &index, &i);
         index++;
      }
   }

   sheetArray->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetPlacementData
*/
short API::GetLogicSymbolPlacementData(short geometryNumber, VARIANT FAR* array) 
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

   int count = 0;
   
   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      if (data->getDataType() == T_INSERT && data->getInsert()->getInsertType() == INSERTTYPE_SYMBOL)
         count++;
   }

   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(array);
   array->vt = VT_ARRAY | VT_BSTR;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_BSTR, 0, count); 

   int keyword = doc->IsKeyWord(SCH_ATT_DESIGNATOR, 0);
   CString buf2;
   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   long i = 0;
   BSTR bstr;
   dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      POSITION tempPos = dataPos;
      DataStruct *data = block->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT || data->getInsert()->getInsertType() != INSERTTYPE_SYMBOL)
         continue;

      CString buf = "";

      // instance name
      Attrib* attrib;

      if (keyword >= 0 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         buf += attrib->getStringValue();
      }

      buf += DELIM;

      // refname
      if (data->getInsert()->getRefname())
         buf += data->getInsert()->getRefname();

      buf += DELIM;

      // geomname
      BlockStruct *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      if (subblock)
      {
         CString geomName = subblock->getName();
         int index = geomName.ReverseFind(DELIM);

         if (index >= 0)
            buf += geomName.Right(geomName.GetLength() - index - 1);
         else
            buf += geomName;
      }

      buf += DELIM;

      // geomnum
      buf2.Format("%d", data->getInsert()->getBlockNumber());
      buf += buf2;
      buf += DELIM;

/*    // x, y
      buf2.Format("%+.*lf,%+.*lf", decimals, data->getInsert()->getOriginX(), decimals, data->getInsert()->getOriginY());
      buf += buf2;
      buf += DELIM;

      // rotation
      buf2.Format("%+.2lf", RadToDeg(data->getInsert()->getAngle()));
      buf += buf2;
      buf += DELIM;

      // entity
      buf2.Format("%d", data->getEntityNumber());
      buf += buf2;
      buf += DELIM; */

      // position
      buf2.Format("%lu", tempPos);
      buf += buf2;
      buf += DELIM;

      // attributes
      if (data->getAttributesRef())
      {
         for (POSITION attribPos = data->getAttributesRef()->GetStartPosition();attribPos != NULL;)
         {
            WORD keyword;
            Attrib* attrib;

            data->getAttributesRef()->GetNextAssoc(attribPos, keyword, attrib);

            buf += DELIM;
      
            buf += doc->getKeyWordArray()[keyword]->cc;

            buf += DELIM;
            
            CString buf2;

            switch (attrib->getValueType())
            {
            case VT_NONE:
               break;
            case VT_INTEGER:
               buf2.Format("%d", attrib->getIntValue());
               buf += buf2;
               break;
            case VT_DOUBLE:
            case VT_UNIT_DOUBLE:
               buf2.Format("%lf", attrib->getDoubleValue());
               buf += buf2;
               break;
            case VT_STRING:
            case VT_EMAIL_ADDRESS:
            case VT_WEB_ADDRESS:
               {
                  if (!attrib->getStringValue().IsEmpty())
                     buf += attrib->getStringValue();
               }
               break;
            }
         }
      }
      
      bstr = buf.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);

      i++;
   }

   array->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetSchematicNetData
*/
short API::GetSchematicNetData(long filePosition, VARIANT FAR* array) 
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

   int count = 0;
   
   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      NetStruct *net = file->getNetList().GetNext(netPos);

      POSITION cpPos = net->getHeadCompPinPosition();
      while (cpPos != NULL)
      {
         CompPinStruct *cp = net->getNextCompPin(cpPos);
         count++;
      }
   }

   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(array);
   array->vt = VT_ARRAY | VT_BSTR;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_BSTR, 0, count); 

   int compKey = doc->RegisterKeyWord(SCH_ATT_COMPDESIGNATOR, 0, VT_STRING);
   int pinKey = doc->RegisterKeyWord(SCH_ATT_PINDESIGNATOR, 0, VT_STRING);
   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   long i = 0;
   BSTR bstr;
   netPos = file->getNetList().GetHeadPosition();

   while (netPos != NULL)
   {
      NetStruct *net = file->getNetList().GetNext(netPos);

      POSITION cpPos = net->getHeadCompPinPosition();
      while (cpPos != NULL)
      {
         CompPinStruct *cp = net->getNextCompPin(cpPos);

         CString buf = net->getNetName();
         buf += DELIM;
         buf += cp->getRefDes();
         buf += DELIM;
         buf += cp->getPinName();
         buf += DELIM;

         // comp designator
         Attrib* attrib;

         if (compKey >= 0 && cp->getAttributesRef() && cp->getAttributesRef()->Lookup(compKey, attrib))
         {
            buf += attrib->getStringValue();
         }

         buf += DELIM;

         // pin designator
         if (pinKey >= 0 && cp->getAttributesRef() && cp->getAttributesRef()->Lookup(pinKey, attrib))
         {
            buf += attrib->getStringValue();
         }

         buf += DELIM;

         // attributes
         if (cp->getAttributesRef())
         {
            for (POSITION attribPos = cp->getAttributesRef()->GetStartPosition();attribPos != NULL;)
            {
               WORD keyword;
               Attrib* attrib;

               cp->getAttributesRef()->GetNextAssoc(attribPos, keyword, attrib);

               buf += DELIM;
         
               buf += doc->getKeyWordArray()[keyword]->cc;

               buf += DELIM;
               
               CString buf2;

               switch (attrib->getValueType())
               {
               case VT_NONE:
                  break;
               case VT_INTEGER:
                  buf2.Format("%d", attrib->getIntValue());
                  buf += buf2;
                  break;
               case VT_DOUBLE:
               case VT_UNIT_DOUBLE:
                  buf2.Format("%lf", attrib->getDoubleValue());
                  buf += buf2;
                  break;
               case VT_STRING:
               case VT_EMAIL_ADDRESS:
               case VT_WEB_ADDRESS:
                  {
                     if (!attrib->getStringValue().IsEmpty())
                        buf += attrib->getStringValue();
                  }
                  break;
               }
            }
         }

         bstr = buf.AllocSysString();
         SafeArrayPutElement(psa, &i, bstr);
         SysFreeString(bstr);

         i++;         
      }
   }

   array->parray = psa;

   return RC_SUCCESS;
}