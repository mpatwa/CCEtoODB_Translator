// $Header: /CAMCAD/5.0/Api_Attr.cpp 44    6/17/07 8:49p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-97. All Rights Reserved.
*/           

#include "stdafx.h"
#include "CCEtoODB.h"
#include "API.h"
#include "ccdoc.h"
#include "attrib.h"

extern CCEtoODBView *apiView; // from API.CPP

static CAttributes** currentAttribMap;
static CCEtoODBDoc *currentDoc;

/******************************************************************************
* SetTypeCurrentAttrib
*/
short API::SetTypeCurrentAttrib(long filePosition, long typePosition) 
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

   pos = (POSITION)typePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   TypeStruct *type = file->getTypeList().GetAt(pos);

   currentAttribMap = &type->getAttributesRef();
   currentDoc = doc;

   return RC_SUCCESS;
}

/******************************************************************************
* SetNetCurrentAttrib
*/
short API::SetNetCurrentAttrib(long filePosition, long netPosition)
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

   currentAttribMap = &net->getAttributesRef();
   currentDoc = doc;

   return RC_SUCCESS;
}

/******************************************************************************
* SetCompPinCurrentAttrib
*/
short API::SetCompPinCurrentAttrib(long filePosition, long netPosition, long compPinPosition) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   // find FILE
   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);

   // find NET
   pos = (POSITION)netPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   NetStruct *net = file->getNetList().GetAt(pos);

   // find COMPPIN
   pos = (POSITION)compPinPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   CompPinStruct *cp = net->getCompPinList().getAt(pos);

   currentAttribMap = &cp->getAttributesRef();
   currentDoc = doc;

   return RC_SUCCESS;
}

/******************************************************************************
* SetGeometryCurrentAttrib
*/
short API::SetGeometryCurrentAttrib(short geometryNumber) 
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

   currentAttribMap = &block->getAttributesRef();
   currentDoc = doc;

   return RC_SUCCESS;
}

/******************************************************************************
* SetDataCurrentAttrib
*/
short API::SetDataCurrentAttrib(short geometryNumber, long dataPosition) 
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

   currentAttribMap = &data->getAttributesRef();
   currentDoc = doc;

   return RC_SUCCESS;
}

/******************************************************************************
* SetSelectedDataCurrentAttrib
*/
short API::SetSelectedDataCurrentAttrib() 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (doc->nothingIsSelected())
      return RC_NOTHING_SELECTED;

   SelectStruct *s = doc->getSelectStack().getAt(0);
   currentAttribMap = &s->getData()->getAttributesRef();
   currentDoc = doc;

   return RC_SUCCESS;
}

/******************************************************************************
* SetLayerCurrentAttrib
*/
short API::SetLayerCurrentAttrib(short layerNumber) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (layerNumber < 0 || layerNumber >= doc->getMaxLayerIndex())
      return -1;

   LayerStruct *layer = doc->getLayerArray()[layerNumber];

   if (!layer)
      return -1;

   currentAttribMap = &layer->getAttributesRef();
   currentDoc = doc;

   return RC_SUCCESS;
}

/******************************************************************************
* GetKeyword
*/
short API::GetKeyword(short keyword, BSTR FAR* InKeyword, BSTR FAR* CCKeyword, 
                      BSTR FAR* OutKeyword, short FAR* type, int FAR* group) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (keyword < 0 || keyword >= doc->getKeyWordArray().GetCount())
      return RC_INDEX_OUT_OF_RANGE;

	const KeyWordStruct *kw = doc->getKeyWordArray()[keyword];
   *InKeyword = kw->getInKeyword().AllocSysString();
   *CCKeyword = kw->getCCKeyword().AllocSysString();
   *OutKeyword = kw->getOutKeyword().AllocSysString();
   *type = kw->getValueType();
   *group = kw->getGroup();

   return RC_SUCCESS;
}

/******************************************************************************
* GetKeywordIndex
*/
short API::GetKeywordIndex(LPCTSTR keywordString, short FAR* keyword) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   int result = doc->IsKeyWord(keywordString, 0);

   if (result == -1)
   {
      *keyword = -1;
      return RC_NO_ATTRIBS;
   }

   *keyword = result;

   return RC_SUCCESS;
}

/******************************************************************************
* CreateKeyword
*/
short API::CreateKeyword(LPCTSTR name, short valueType, short group, short FAR* keyword) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   *keyword = doc->RegisterKeyWord(name, 0, valueType);

   if (*keyword == -1)
      return RC_NO_ATTRIBS;

   doc->getKeyWordArray().SetGroup(*keyword,intToAttribGroupTag(group));

   return RC_SUCCESS;
}

/******************************************************************************
* GetAttribKeywordArray
*/
short API::GetAttribKeywordArray(VARIANT FAR* keywordArray) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (!*currentAttribMap)
      return RC_NO_ATTRIBS;

   int count = (*currentAttribMap)->GetCount();
   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(keywordArray);
   keywordArray->vt = VT_ARRAY | VT_I2;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I2, 0, count); 

   WORD keyword;
   Attrib* attrib;

   POSITION pos = (*currentAttribMap)->GetStartPosition();

   for (long i = 0; i < count; i++)
   {
      (*currentAttribMap)->GetNextAssoc(pos, keyword, attrib);
      SafeArrayPutElement(psa, &i, &keyword);
   }

   keywordArray->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetAttribValue
*/
short API::GetAttribValue(short keyword, BSTR FAR* value) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (doc != currentDoc)
      return RC_NO_ACTIVE_DOC;

   if (!*currentAttribMap)
      return RC_NO_ATTRIBS;

   Attrib* attrib;
   CString buf;

   if (!(*currentAttribMap)->Lookup(keyword, attrib))
      return RC_NO_ATTRIBS;

   switch (attrib->getValueType())
   {
      case VT_STRING:
      case VT_EMAIL_ADDRESS:
      case VT_WEB_ADDRESS:
         buf = attrib->getStringValue();
         break;
      case VT_UNIT_DOUBLE:
         {
            int decimals = GetDecimals(doc->getSettings().getPageUnits());
            buf.Format("%+.*lf", decimals, attrib->getDoubleValue());
         }
         break;
      case VT_DOUBLE:
         buf.Format("%+.2lf", attrib->getDoubleValue());
         break;   
      case VT_INTEGER:
         buf.Format("%+d", attrib->getIntValue());
         break;
      case VT_NONE:
         buf = "";
         break;
   }

   *value = buf.AllocSysString();

   return RC_SUCCESS;
}

/******************************************************************************
* SetAttribValue
*/
short API::SetAttribValue(short keyword, LPCTSTR value, short overwriteMethod) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (doc != currentDoc)
      return RC_NO_ACTIVE_DOC;

   doc->SetUnknownAttrib(currentAttribMap, doc->getKeyWordArray()[keyword]->cc, value, overwriteMethod, NULL);

   return RC_SUCCESS;
}

/******************************************************************************
* RemoveAttrib
*/
short API::RemoveAttrib(short keyword) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (doc != currentDoc)
      return RC_NO_ACTIVE_DOC;

   ::RemoveAttrib(keyword, currentAttribMap);

   return RC_SUCCESS;
}

/******************************************************************************
* API::RemoveAttrib
*/
short API::DeleteOccuranceOfKeyword(short keyword) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (doc == NULL)
      return RC_NO_ACTIVE_DOC;

   doc->DeleteOccurancesOfKeyword(keyword);

   return RC_SUCCESS;
}

/******************************************************************************
* SetAttribValueOnSelected
*/
short API::SetAttribValueOnSelected(short keyword, LPCTSTR value, short overwriteMethod) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (doc->SelectList.IsEmpty())
      return RC_NOTHING_SELECTED;

   POSITION pos = doc->SelectList.GetHeadPosition();
   while (pos != NULL)
   {
      SelectStruct *s = doc->SelectList.GetNext(pos);

      doc->SetUnknownAttrib(&s->getData()->getAttributesRef(), doc->getKeyWordArray()[keyword]->cc, value, overwriteMethod, NULL);
   }

   return RC_SUCCESS;
}

/******************************************************************************
* SetAttribVisible
*/
short API::SetAttribVisible(short keyword, BOOL visible, double x, double y, double height, double width, 
      double angle, short layer, short penWidthIndex, BOOL proportional, BOOL neverMirror) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (doc != currentDoc)
      return RC_NO_ACTIVE_DOC;

   if (!*currentAttribMap)
      return RC_NO_ATTRIBS;

   Attrib* attrib;
   CString buf;

   if (!(*currentAttribMap)->Lookup(keyword, attrib))
      return RC_NO_ATTRIBS;

   attrib->setCoordinate(x,y);
   attrib->setRotationRadians(angle);
   attrib->setHeight(height);
   attrib->setWidth(width);
   attrib->setProportionalSpacing(proportional);
   attrib->setPenWidthIndex(penWidthIndex);
   attrib->setMirrorDisabled(neverMirror);
   attrib->setVisible(visible);
   attrib->setFlags(0);
   attrib->setLayerIndex(layer);
   attrib->setInherited(false);
   attrib->setHorizontalPosition(horizontalPositionLeft);
   attrib->setVerticalPosition(verticalPositionBaseline);

   return RC_SUCCESS;
}

/******************************************************************************
* SetAttribByRefDes
*/
short API::SetAttribByRefDes(LPCTSTR keyword, LPCTSTR value, short method, LPCTSTR refDes) 
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
         DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

         if (data->getDataType() == T_INSERT && data->getInsert()->getRefname() && !STRICMP(data->getInsert()->getRefname(), refDes))
            return doc->SetUnknownAttrib(&data->getAttributesRef(), keyword, value, method, NULL);
      }
   }

   return RC_ITEM_NOT_FOUND;
}

