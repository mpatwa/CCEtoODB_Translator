
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/           

#include "stdafx.h"
#include "CCEtoODB.h"
#include "API.h"
#include "ccview.h"
#include "attrib.h"
#include "pcbutil.h"
#include "drc.h"
#include "centroid.h"
#include "extents.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CCEtoODBView *apiView; // from API.CPP
extern CMultiDocTemplate* pDocTemplate; // from CAMCAD.CPP

/*
Sub Main
   Dim CAMCAD
   Set CAMCAD = CreateObject("CAMCAD.Application")

   Dim FileArray As Variant
   CAMCAD.GetFileArray FileArray
   max = UBound (FileArray)
   
   Dim Name As String
   Dim Num as Integer
   Dim x As Single
   Dim y As Single
   Dim scale As Single
   Dim angle As Single
   Dim mirror As Integer
   Dim fileType As Integer
   Dim geometryNumber
      
   For i = 0 To max
      res = CAMCAD.GetFile ( FileArray(i), Name, Num, x, y, scale, angle, mirror, fileType, geometryNumber )
   Next i

End Sub
*/

/******************************************************************************
* GetDocumentArray
*/
short API::GetDocumentArray(VARIANT FAR* documentArray) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   int count = 0;

   POSITION pos = pDocTemplate->GetFirstDocPosition();
   while (pos != NULL)
   {
      pDocTemplate->GetNextDoc(pos);
      count++;
   }

   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(documentArray);
   documentArray->vt = VT_ARRAY | VT_I4;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I4, 0, count); 

   pos = pDocTemplate->GetFirstDocPosition();
   for (long i = 0; i < count; i++)
   {
      SafeArrayPutElement(psa, &i, &pos);
      pDocTemplate->GetNextDoc(pos);
   }

   documentArray->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetDocument
*/
short API::GetDocument(long documentPosition, BSTR FAR* name) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   POSITION pos = pDocTemplate->GetFirstDocPosition();
   while (pos != NULL)
   {
      if (pos == (POSITION)documentPosition)
      {
         CDocument *doc = pDocTemplate->GetNextDoc(pos);
         CString buf = doc->GetTitle();
         *name = buf.AllocSysString();
         return RC_SUCCESS;
      }
      pDocTemplate->GetNextDoc(pos);
   }

   return RC_ITEM_NOT_FOUND;
}

/******************************************************************************
* GetActiveDocument
*/
short API::GetActiveDocument(long FAR* documentPosition) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   *documentPosition = (long)doc->GetDocumentPosition();
   return RC_SUCCESS;
}


/******************************************************************************
* GetFileArray
*/
short API::GetFileArray(VARIANT FAR* fileArray) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   int count = doc->getFileList().GetCount();
   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(fileArray);
   fileArray->vt = VT_ARRAY | VT_I4;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I4, 0, count); 

   POSITION pos = doc->getFileList().GetHeadPosition();
   for (long i = 0; i < count; i++)
   {
      SafeArrayPutElement(psa, &i, &pos);
      doc->getFileList().GetNext(pos);
   }

   fileArray->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetFile
*/
short API::GetFile2(long filePosition, BSTR FAR* name, short FAR* number, BOOL FAR* On, float FAR* x, float FAR* y, 
      float FAR* scale, float FAR* angle, short FAR* mirror, short FAR* type, short FAR* geometryNumber) 
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

   *name   = file->getName().AllocSysString();
   *number = file->getFileNumber();
   *On     = file->isShown();
   *x      = (DbUnit)file->getInsertX();
   *y      = (DbUnit)file->getInsertY();
   *scale  = (DbUnit)file->getScale();
   *angle  = (DbUnit)RadToDeg(file->getRotation());
   *mirror = file->isMirrored();
   *type   = file->getBlockType();
   *geometryNumber = file->getBlock()->getBlockNumber();

   return RC_SUCCESS;
}

short API::GetFile(long filePosition, BSTR FAR* name, short FAR* number, float FAR* x, float FAR* y, 
                   float FAR* scale, float FAR* angle, short FAR* mirror, short FAR* type, short FAR* geometryNumber) 
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

   *name   = file->getName().AllocSysString();
   *number = file->getFileNumber();
   *x      = (DbUnit)file->getInsertX();
   *y      = (DbUnit)file->getInsertY();
   *scale  = (DbUnit)file->getScale();
   *angle  = (DbUnit)RadToDeg(file->getRotation());
   *mirror = file->isMirrored();
   *type   = file->getBlockType();
   *geometryNumber = file->getBlock()->getBlockNumber();

   return RC_SUCCESS;
}

/******************************************************************************
* GetFilePlaced
*/
short API::GetFilePlaced(long filePosition, BOOL FAR* placed) 
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

   *placed = !file->notPlacedYet();

   return RC_SUCCESS;
}
 
/******************************************************************************
* GetTypeArray
*/
short API::GetTypeArray(long filePosition, VARIANT FAR* typeArray) 
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

   int count = file->getTypeList().GetCount();
   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(typeArray);
   typeArray->vt = VT_ARRAY | VT_I4;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I4, 0, count); 

   pos = file->getTypeList().GetHeadPosition();
   for (long i = 0; i < count; i++)
   {
      SafeArrayPutElement(psa, &i, &pos);
      file->getTypeList().GetNext(pos);
   }

   typeArray->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetType
*/
short API::GetType(long filePosition, long typePosition, BSTR FAR* name, short FAR* geometryNumber) 
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

   *name = type->getName().AllocSysString();
   *geometryNumber = type->getBlockNumber();

   return RC_SUCCESS;
}

/******************************************************************************
* GetNetArray
*/
short API::GetNetArray(long filePosition, VARIANT FAR* netArray) 
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

   int count = file->getNetList().GetCount();
   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(netArray);
   netArray->vt = VT_ARRAY | VT_I4;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I4, 0, count); 

   pos = file->getNetList().GetHeadPosition();
   for (long i = 0; i < count; i++)
   {
      SafeArrayPutElement(psa, &i, &pos);
      file->getNetList().GetNext(pos);
   }

   netArray->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetNet
*/
short API::GetNet(long filePosition, long netPosition, BSTR FAR* netname) 
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

   *netname = net->getNetName().AllocSysString();

   return RC_SUCCESS;
}


/******************************************************************************
* GetCompPinArray
*/
short API::GetCompPinArray(long filePosition, long netPosition, VARIANT FAR* compPinArray) 
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

   int count = net->getCompPinCount();
   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(compPinArray);
   compPinArray->vt = VT_ARRAY | VT_I4;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I4, 0, count); 

   pos = net->getHeadCompPinPosition();
   for (long i = 0; i < count; i++)
   {
      SafeArrayPutElement(psa, &i, &pos);
      net->getNextCompPin(pos);
   }

   compPinArray->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetCompPin
*/
short API::GetCompPin(long filePosition, long netPosition, long compPinPosition, BSTR FAR* compName, BSTR FAR* pinName, 
                      float FAR* x, float FAR* y) 
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

   *compName = cp->getRefDes().AllocSysString();
   *pinName = cp->getPinName().AllocSysString();
   *x = (float)cp->getOriginX();
   *y = (float)cp->getOriginY();

   return RC_SUCCESS;
}


/******************************************************************************
* GetDRCArray
*/
short API::GetDRCArray(long filePosition, VARIANT FAR* drcArray) 
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

   int count = file->getDRCList().GetCount();
   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(drcArray);
   drcArray->vt = VT_ARRAY | VT_I4;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I4, 0, count); 

   pos = file->getDRCList().GetHeadPosition();
   for (long i = 0; i < count; i++)
   {
      SafeArrayPutElement(psa, &i, &pos);
      file->getDRCList().GetNext(pos);
   }

   drcArray->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetDRC
*/
short API::GetDRC2(long filePosition, long drcPosition, BSTR FAR* string, double FAR* x, double FAR* y, 
      short FAR* failureRange, short FAR* algorithmIndex, short FAR* algorithmType, long FAR* insertEntity, 
      BSTR FAR* comment, short FAR* drcClass) 
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

   pos = (POSITION)drcPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   DRCStruct *drc = file->getDRCList().GetAt(pos);

   *string = drc->getString().AllocSysString();
   *comment = drc->getComment().AllocSysString();
   *x = drc->getOrigin().x;
   *y = drc->getOrigin().y;
   *failureRange = drc->getFailureRange();
   *algorithmIndex = drc->getAlgorithmIndex();
   *algorithmType = drc->getAlgorithmType();
   *insertEntity = drc->getInsertEntityNumber();
   *drcClass = drc->getDrcClass();

   return RC_SUCCESS;
}

short API::GetDRC(long filePosition, long drcPosition, BSTR FAR* string, double FAR* x, double FAR* y, 
      short FAR* failureRange, short FAR* algorithmIndex, short FAR* algorithmType, BSTR FAR* comment, short FAR* drcClass) 
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

   pos = (POSITION)drcPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   DRCStruct *drc = file->getDRCList().GetAt(pos);

   *string = drc->getString().AllocSysString();
   *comment = drc->getComment().AllocSysString();
   *x = drc->getOrigin().x;
   *y = drc->getOrigin().y;
   *failureRange = drc->getFailureRange();
   *algorithmIndex = drc->getAlgorithmIndex();
   *algorithmType = drc->getAlgorithmType();
   *drcClass = drc->getDrcClass();

   return RC_SUCCESS;
}

/******************************************************************************
* GetDRCMeasure
*/
short API::GetDRCMeasure(long filePosition, long drcPosition, long FAR* entity1, short FAR* type1, double FAR* x1, double FAR* y1, long FAR* entity2, short FAR* type2, double FAR* x2, double FAR* y2) 
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

   pos = (POSITION)drcPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   DRCStruct *drc = file->getDRCList().GetAt(pos);

   if (drc->getDrcClass() != DRC_CLASS_MEASURE)
      return RC_WRONG_TYPE;

   DRC_MeasureStruct *meas = (DRC_MeasureStruct*)drc->getVoidPtr();

   *entity1 = meas->entity1;
   *type1 = meas->type1;
   *x1 = meas->x1;
   *y1 = meas->y1;
   *entity2 = meas->entity2;
   *type2 = meas->type2;
   *x2 = meas->x2;
   *y2 = meas->y2;

   return RC_SUCCESS;
}

/******************************************************************************
* GetDRCNets
*/
short API::GetDRCNets(long filePosition, long drcPosition, VARIANT FAR* netnameArray) 
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

   pos = (POSITION)drcPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   DRCStruct *drc = file->getDRCList().GetAt(pos);

   if (drc->getDrcClass() != DRC_CLASS_NETS)
      return RC_WRONG_TYPE;

   CStringList *list = (CStringList*)drc->getVoidPtr();

   if (!list->GetCount())
      return RC_EMPTY_ARRAY;

   VariantInit(netnameArray);
   netnameArray->vt = VT_ARRAY | VT_BSTR;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_BSTR, 0, list->GetCount()); 

   long i = 0;
   POSITION netnamePos = list->GetHeadPosition();
   while (netnamePos)
   {
      CString netname = list->GetNext(netnamePos);
      BSTR bstr = netname.AllocSysString();
      SafeArrayPutElement(psa, &i, bstr);
      SysFreeString(bstr);

      i++;
   }

   netnameArray->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetDRCAlgorithm
*/
short API::GetDRCAlgorithm(short algorithmNumber, BSTR FAR* algorithmName) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (algorithmNumber < 0 || algorithmNumber >= doc->getDfmAlgorithmArraySize())
      return RC_INDEX_OUT_OF_RANGE;

   *algorithmName = doc->getDfmAlgorithmNamesArray()[algorithmNumber].AllocSysString();

   return RC_SUCCESS;
}


/******************************************************************************
* GetGeometryArrayByType
*/
short API::GetGeometryArrayByType(short geometryType, VARIANT FAR* geometryArray) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   // count geometries of this type
   int count = 0;
	short n = 0;
   for (n=0; n<doc->getMaxBlockIndex(); n++)
   {
      BlockStruct *block = doc->getBlockAt(n);
      if (block == NULL)   continue;

      if (geometryType == -1 || block->getBlockType() == geometryType)
         count++;
   }
   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(geometryArray);
   geometryArray->vt = VT_ARRAY | VT_I2;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I2, 0, count); 

   long i = 0;
   for (n=0; n<doc->getMaxBlockIndex(); n++)
   {
      BlockStruct *block = doc->getBlockAt(n);
      if (block == NULL)   continue;
      if (geometryType == -1 || block->getBlockType() == geometryType)
      {
         SafeArrayPutElement(psa, &i, &n);
         i++;
      }
   }

   geometryArray->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetGeometry
*/
short API::GetGeometry(short geometryNumber, BSTR FAR* name, short FAR* geometryType, short FAR* geometryIsA) 
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

   *name = block->getName().AllocSysString();
   *geometryType = block->getBlockType();

   if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
      *geometryIsA = 1;
   else if (block->getFlags() & BL_TOOL || block->getFlags() & BL_BLOCK_TOOL)
      *geometryIsA = 2;
   else
      *geometryIsA = 0;

   return RC_SUCCESS;
}


/******************************************************************************
* GetDataArray
*/
short API::GetDataArray(short geometryNumber, VARIANT FAR* dataArray) 
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

   int count = block->getDataList().GetCount();
   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(dataArray);
   dataArray->vt = VT_ARRAY | VT_I4;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I4, 0, count); 

   POSITION pos = block->getDataList().GetHeadPosition();
   for (long i = 0; i < count; i++)
   {
      SafeArrayPutElement(psa, &i, &pos);
      block->getDataList().GetNext(pos);
   }

   dataArray->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetDataArrayByGraphicClass
*/
short API::GetDataArrayByGraphicClass(short geometryNumber, short graphicClass, VARIANT FAR* dataArray) 
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

   // count datas of this type
   int count = 0;
   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);
      if (data->getGraphicClass() == graphicClass)
         count++;
   }

   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(dataArray);
   dataArray->vt = VT_ARRAY | VT_I4;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I4, 0, count); 

   long i = 0;
   pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetAt(pos);
      if (data->getGraphicClass() == graphicClass)
      {
         SafeArrayPutElement(psa, &i, &pos);
         i++;
      }
      block->getDataList().GetNext(pos);
   }

   dataArray->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetDataArrayByType
*/
short API::GetDataArrayByType(short geometryNumber, short dataType, VARIANT FAR* dataArray) 
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

   // count datas of this type
   int count = 0;
   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);
      if (data->getDataType() == dataType)
         count++;
   }

   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(dataArray);
   dataArray->vt = VT_ARRAY | VT_I4;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I4, 0, count); 

   long i = 0;
   pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetAt(pos);
      if (data->getDataType() == dataType)
      {
         SafeArrayPutElement(psa, &i, &pos);
         i++;
      }
      block->getDataList().GetNext(pos);
   }

   dataArray->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetDataArrayByInsertType
*/
short API::GetDataArrayByInsertType(short geometryNumber, short insertType, VARIANT FAR* dataArray) 
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

   // count datas of this type
   int count = 0;
   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);
      if (data->getDataType() == T_INSERT && (insertType == -1 || data->getInsert()->getInsertType() == insertType))
         count++;
   }

   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(dataArray);
   dataArray->vt = VT_ARRAY | VT_I4;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I4, 0, count); 

   long i = 0;
   pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetAt(pos);
      if (data->getDataType() == T_INSERT && (insertType == -1 || data->getInsert()->getInsertType() == insertType))
      {
         SafeArrayPutElement(psa, &i, &pos);
         i++;
      }
      block->getDataList().GetNext(pos);
   }

   dataArray->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetData
*/
short API::GetData(short geometryNumber, long dataPosition, short FAR* dataIsA, short FAR* graphicClass, short FAR* layer) 
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

   *dataIsA = data->getDataType();
   *graphicClass = data->getGraphicClass();
   *layer = data->getLayerIndex();

   return RC_SUCCESS;
}

/******************************************************************************
* GetDataInsert
*/
short API::GetDataInsert(short geometryNumber, long dataPosition, short FAR* insertedGeometryNumber, 
                         float FAR* x, float FAR* y, float FAR* scale, float FAR* rotation, short FAR* mirror, 
                         short FAR* insertType, BSTR FAR* referenceName) 
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

   if (data->getDataType() != T_INSERT)
      return RC_WRONG_TYPE;

   *insertedGeometryNumber = data->getInsert()->getBlockNumber();
   *x = data->getInsert()->getOriginX();
   *y = data->getInsert()->getOriginY();
   *scale = data->getInsert()->getScale();
   *rotation = (DbUnit)RadToDeg(data->getInsert()->getAngle());
   *mirror = data->getInsert()->getMirrorFlags();
   *insertType = data->getInsert()->getInsertType();
   CString buf;
   if (data->getInsert()->getRefname())
      buf = data->getInsert()->getRefname();
   else 
      buf = "";
   *referenceName = buf.AllocSysString();

   return RC_SUCCESS;
}

/******************************************************************************
* GetDataPolyStruct
*/
short API::GetDataPolyStruct(short geometryNumber, long dataPosition, VARIANT FAR* polyArray) 
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

   if (data->getDataType() != T_POLY)
      return RC_WRONG_TYPE;

   int count = data->getPolyList()->GetCount();
   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(polyArray);
   polyArray->vt = VT_ARRAY | VT_I4;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I4, 0, count); 

   pos = data->getPolyList()->GetHeadPosition();
   for (long i = 0; i < count; i++)
   {
      SafeArrayPutElement(psa, &i, &pos);
      data->getPolyList()->GetNext(pos);
   }

   polyArray->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetDataPoly
*/
short API::GetDataPoly(short geometryNumber, long dataPosition, long polyPosition, 
                       short FAR* widthIndex, BOOL FAR* filled, BOOL FAR* closed, BOOL FAR* voidPoly, 
                       VARIANT FAR* pointArray) 
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

   if (data->getDataType() != T_POLY)
      return RC_WRONG_TYPE;

   // find POLY
   pos = (POSITION)polyPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   CPoly *poly = data->getPolyList()->GetAt(pos);

   *widthIndex = poly->getWidthIndex();
   *filled = poly->isFilled();
   *closed = poly->isClosed();
   *voidPoly = poly->isVoid();

   int count = poly->getPntList().GetCount();
   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(pointArray);
   pointArray->vt = VT_ARRAY | VT_I4;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I4, 0, count); 

   pos = poly->getPntList().GetHeadPosition();
   for (long i = 0; i < count; i++)
   {
      SafeArrayPutElement(psa, &i, &pos);
      poly->getPntList().GetNext(pos);
   }

   pointArray->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetDataPolyPoint
*/
short API::GetDataPolyPoint(short geometryNumber, long dataPosition, long polyPosition, long pointPosition, 
                            float FAR* x, float FAR* y, float FAR* bulgeNumber) 
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

   if (data->getDataType() != T_POLY)
      return RC_WRONG_TYPE;

   // find POLY
   pos = (POSITION)polyPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   CPoly *poly = data->getPolyList()->GetAt(pos);

   // find POINT
   pos = (POSITION)pointPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   CPnt *pnt = poly->getPntList().GetAt(pos);

   *x = pnt->x;
   *y = pnt->y;
   *bulgeNumber = pnt->bulge;

   return RC_SUCCESS;
}


/******************************************************************************
* GetLayerArrayByType
*/
short API::GetLayerArrayByType(short layerType, VARIANT FAR* layerArray) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   // count layers of this type
   int count = 0;
	short n=0;
   for (n=0; n<doc->getMaxLayerIndex(); n++)
   {
      LayerStruct *layer = doc->getLayerArray()[n];
      if (layer == NULL)   continue;
      if (layer->getLayerType() == layerType)
         count++;
   }
   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(layerArray);
   layerArray->vt = VT_ARRAY | VT_I2;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I2, 0, count); 

   long i = 0;
   for (n=0; n<doc->getMaxLayerIndex(); n++)
   {
      LayerStruct *layer = doc->getLayerArray()[n];
      if (layer == NULL)   continue;
      if (layer->getLayerType() == layerType)
      {
         SafeArrayPutElement(psa, &i, &n);
         i++;
      }
   }

   layerArray->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetLayerStackup
*/
short API::GetLayerStackup(short layerNumber, short FAR* electrical, short FAR* physical, short FAR* artwork) 
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

   *electrical = layer->getElectricalStackNumber();
   *physical = layer->getPhysicalStackNumber();
   *artwork = layer->getArtworkStackNumber();

   return RC_SUCCESS;
}

/******************************************************************************
* GetLayerVisible
*/
short API::GetLayerVisible(short layerNumber, BOOL FAR* visible) 
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

   *visible = layer->isVisible();

   return RC_SUCCESS;
}

/******************************************************************************
* GetLayerEditable
*/
short API::GetLayerEditable(short layerNumber, BOOL FAR* editable) 
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

   *editable = layer->isEditable();

   return RC_SUCCESS;
}

#include <comdef.h>
/******************************************************************************
* GetLayer
*/
short API::GetLayer3(short layerNumber, BSTR FAR* name, short FAR* layerType, short FAR* show, 
      short FAR* mirrorLayerNumber, BSTR FAR* comment, short FAR* red, short FAR* green, short FAR* blue, 
      short FAR* worldView) 
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

   *name = layer->getName().AllocSysString();
   *layerType = layer->getLayerType();
   *show = layer->isVisible();
   *mirrorLayerNumber = layer->getMirroredLayerIndex();
   *comment = layer->getComment().AllocSysString();
   *red = GetRValue(layer->getColor());
   *green = GetGValue(layer->getColor());
   *blue = GetBValue(layer->getColor());
   *worldView = layer->isWorldView();

   return RC_SUCCESS;
}

short API::GetLayer2(short layerNumber, BSTR FAR* name, short FAR* layerType, short FAR* show, 
      short FAR* mirrorLayerNumber, BSTR FAR* comment, short FAR* red, short FAR* green, short FAR* blue) 
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

   *name = layer->getName().AllocSysString();
   *layerType = layer->getLayerType();
   *show = layer->isVisible();
   *mirrorLayerNumber = layer->getMirroredLayerIndex();
   *comment = layer->getComment().AllocSysString();
   *red = GetRValue(layer->getColor());
   *green = GetGValue(layer->getColor());
   *blue = GetBValue(layer->getColor());

   return RC_SUCCESS;
}

short API::GetLayer(short layerNumber, BSTR FAR* name, short FAR* layerType, short FAR* show, BSTR FAR* comment) 
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

   *name = layer->getName().AllocSysString();
   *layerType = layer->getLayerType();
   *show = layer->isVisible();
   *comment = layer->getComment().AllocSysString();

   return RC_SUCCESS;
}

/******************************************************************************
* GetSelectedArray
*/
short API::GetSelectedArray(VARIANT FAR* selectedArray) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   int count = doc->SelectList.GetCount();
   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(selectedArray);
   selectedArray->vt = VT_ARRAY | VT_I4;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_I4, 0, count); 

   POSITION pos = doc->SelectList.GetHeadPosition();
   for (long i = 0; i < count; i++)
   {
      SafeArrayPutElement(psa, &i, &pos);
      doc->SelectList.GetNext(pos);
   }

   selectedArray->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetSelected
*/
short API::GetSelected(long selectedPosition, short FAR* geometryNumber, long FAR* dataPosition) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   POSITION pos = (POSITION)selectedPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   SelectStruct *s = doc->SelectList.GetAt(pos);

   if (doc->Find_File(s->filenum) != NULL)
      *geometryNumber = doc->Find_File(s->filenum)->getBlock()->getBlockNumber();
   *dataPosition = (long)s->getParentDataList()->Find(s->getData());

   return RC_SUCCESS;
}

/******************************************************************************
* GetSubSelectLevel
*/
short API::GetSubSelectLevel(short FAR* subSelectLevel) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   *subSelectLevel = doc->getSelectStack().getLevelIndex();

   return RC_SUCCESS;
}

/******************************************************************************
* GetSubSelect
*/
short API::GetSubSelect(short subSelectLevel, short FAR* geometryNumber, long FAR* dataPosition, 
      float FAR* insert_x, float FAR* insert_y, float FAR* scale, float FAR* rotation, short FAR* mirror, short FAR* layer) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   CSelectStack& selectStack = doc->getSelectStack();

   if (! selectStack.isValidLevelIndex(subSelectLevel))
   {
      return RC_INDEX_OUT_OF_RANGE;
   }

   SelectStruct *s = selectStack.getAt(subSelectLevel);
  
   if (subSelectLevel <= 0)
   { 
      if (doc->Find_File(s->filenum) == NULL)
         return RC_ITEM_NOT_FOUND;
      *geometryNumber = doc->Find_File(s->filenum)->getBlock()->getBlockNumber();      
   }
   else
   {
      *geometryNumber = selectStack.getAt(subSelectLevel - 1)->getData()->getInsert()->getBlockNumber();
   }

   *dataPosition = (long)s->getParentDataList()->Find(s->getData());
   *insert_x     = (DbUnit)s->insert_x;
   *insert_y     = (DbUnit)s->insert_y;
   *scale        = (DbUnit)s->scale;
   *rotation     = (DbUnit)s->rotation;
   *mirror       = s->mirror;
   *layer        = s->layer;

   return RC_SUCCESS;
}


/* Data Extraction ************************************************************/

#define DELIM (char)5

/******************************************************************************
* GetComponentData
*/
short API::GetComponentData(long filePosition, VARIANT FAR* array) 
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

   BlockStruct *block = file->getBlock();
   int count = 0;
   
   pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;

      count++;
   }

   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(array);
   array->vt = VT_ARRAY | VT_BSTR;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_BSTR, 0, count); 

   CString buf;
   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   long i = 0;
   BSTR bstr;
   pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;

      buf = "";

      if (data->getInsert()->getRefname())
         buf = data->getInsert()->getRefname();

      buf += DELIM;

      // surface
      if (data->getInsert()->getPlacedBottom())
         buf += "B";
      else
         buf += "T";

      buf += DELIM;

      // insertLayer
      CString layerNum;
      layerNum.Format("%d", data->getLayerIndex());
      buf += layerNum;

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
* GetNetCompPinData3
*/
short API::GetNetCompPinData3(long filePosition, VARIANT FAR* array)
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

   CString buf;
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

         buf = net->getNetName();
         buf += DELIM;
         buf += cp->getRefDes();
         buf += DELIM;
         buf += cp->getPinName();
         buf += DELIM;

         switch (cp->getVisible())
         {
         case VISIBLE_NONE:
            buf += "none";
            break;
         case VISIBLE_TOP:
            buf += "top";
            break;
         case VISIBLE_BOTTOM:
            buf += "bottom";
            break;
         case VISIBLE_BOTH:
            buf += "both";
            break;
         }
         buf += DELIM;

			CString entityNumber;
			entityNumber.Format("%d", net->getEntityNumber());
			buf += entityNumber;
         buf += DELIM;
			entityNumber.Format("%d", cp->getEntityNumber());
			buf += entityNumber;
         buf += DELIM;
			
         CString pntString;
         pntString.Format("%+.*lf,%+.*lf", decimals, cp->getOriginX(), decimals, cp->getOriginY());
         buf += pntString;

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

/******************************************************************************
* GetNetCompPinData
*/
short API::GetNetCompPinData2(long filePosition, VARIANT FAR* array) 
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

   CString buf;
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

         buf = net->getNetName();
         buf += DELIM;
         buf += cp->getRefDes();
         buf += DELIM;
         buf += cp->getPinName();
         buf += DELIM;

         switch (cp->getVisible())
         {
         case VISIBLE_NONE:
            buf += "none";
            break;
         case VISIBLE_TOP:
            buf += "top";
            break;
         case VISIBLE_BOTTOM:
            buf += "bottom";
            break;
         case VISIBLE_BOTH:
            buf += "both";
            break;
         }

         buf += DELIM;

         CString pntString;
         pntString.Format("%+.*lf,%+.*lf", decimals, cp->getOriginX(), decimals, cp->getOriginY());
         buf += pntString;

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

short API::GetNetCompPinData(long filePosition, VARIANT FAR* array) 
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

   CString buf;
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

         buf = net->getNetName();
         buf += DELIM;
         buf += cp->getRefDes();
         buf += DELIM;
         buf += cp->getPinName();
         buf += DELIM;

         switch (cp->getVisible())
         {
         case VISIBLE_NONE:
            buf += "none";
            break;
         case VISIBLE_TOP:
            buf += "top";
            break;
         case VISIBLE_BOTTOM:
            buf += "bottom";
            break;
         case VISIBLE_BOTH:
            buf += "both";
            break;
         }

         buf += DELIM;

         CString pntString;
         pntString.Format("%+.*lf,%+.*lf", decimals, cp->getOriginX(), decimals, cp->getOriginY());
         buf += pntString;

         bstr = buf.AllocSysString();
         SafeArrayPutElement(psa, &i, bstr);
         SysFreeString(bstr);

         i++;
      }
   }

   array->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetNetTraceData
*/
short API::GetNetTraceData(long filePosition, VARIANT FAR* array) 
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

   BlockStruct *block = file->getBlock();
   int count = 0;
   WORD keyword = doc->IsKeyWord(ATT_NETNAME, TRUE);
 
   pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (data->getDataType() != T_POLY)
         continue;

      if (data->getGraphicClass() != graphicClassEtch && data->getGraphicClass() != graphicClassSignal)
         continue;
      
      Attrib *attrib = NULL;
      if (data->lookUpAttrib(keyword, attrib))
         count++;
   }

   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(array);
   array->vt = VT_ARRAY | VT_BSTR;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_BSTR, 0, count); 

   CString buf;
   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   long i = 0;
   BSTR bstr;
   pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (data->getDataType() != T_POLY)
         continue;

      if (data->getGraphicClass() != graphicClassEtch && data->getGraphicClass() != graphicClassSignal)
         continue;

      Attrib *attrib = NULL;
      if (data->lookUpAttrib(keyword, attrib))
      {
         buf = attrib->getStringValue(); // NetName
         buf += DELIM;

         // surface | layerNum |
         if (data->getLayerIndex() < 0)
         {
            buf += "?";
            buf += DELIM;
            buf += "-1";
         }
         else
         {
            LayerStruct *layer = doc->getLayerArray()[data->getLayerIndex()];
            switch (layer->getLayerType())
            {
            case LAYTYPE_SIGNAL_TOP:
               buf += "T";
               break;
            case LAYTYPE_SIGNAL_BOT:
               buf += "B";
               break;
            case LAYTYPE_SIGNAL_INNER:
               buf += "I";
               break;
            default:
               buf += "?";
               break;
            }
            buf += DELIM;

            CString layerNum;
            layerNum.Format("%d", data->getLayerIndex());
            buf += layerNum;
         }

         POSITION polyPos = data->getPolyList()->GetHeadPosition();
         while (polyPos != NULL)
         {
            CPoly *poly = data->getPolyList()->GetNext(polyPos);

            POSITION pntPos = poly->getPntList().GetHeadPosition();
            while (pntPos != NULL)
            {
               CPnt *pnt = poly->getPntList().GetNext(pntPos);

               CString pntString;
               pntString.Format("%+.*lf,%+.*lf", decimals, pnt->x, decimals, pnt->y);

               buf += DELIM;
               buf += pntString;
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

/******************************************************************************
* GetNetTraceDataByNetname
*/
short API::GetNetTraceDataByNetname(long filePosition, LPCTSTR netName, VARIANT FAR* array) 
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

   BlockStruct *block = file->getBlock();
   int count = 0;
   WORD keyword = doc->IsKeyWord(ATT_NETNAME, TRUE);

   pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (data->getDataType() != T_POLY)
         continue;

      if (data->getGraphicClass() != graphicClassEtch && data->getGraphicClass() != graphicClassSignal)
         continue;

		Attrib *attrib = NULL;
      if (data->lookUpAttrib(keyword, attrib))
      {
         CString netname = attrib->getStringValue();
         if (!netname.CompareNoCase(netName))
            count++;
      }
   }

   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(array);
   array->vt = VT_ARRAY | VT_BSTR;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_BSTR, 0, count); 

   CString buf;
   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   long i = 0;
   BSTR bstr;
   pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (data->getDataType() != T_POLY)
         continue;

      if (data->getGraphicClass() != graphicClassEtch && data->getGraphicClass() != graphicClassSignal)
         continue;

      Attrib *attrib = NULL;
      if (data->lookUpAttrib(keyword, attrib))
      {
         CString netname = attrib->getStringValue();
         if (netname.CompareNoCase(netName))
            continue;

         buf = netname; // NetName
         buf += DELIM;

         // surface | layerNum |
         if (data->getLayerIndex() < 0)
         {
            buf += "?";
            buf += DELIM;
            buf += "-1";
         }
         else
         {
            LayerStruct *layer = doc->getLayerArray()[data->getLayerIndex()];
            switch (layer->getLayerType())
            {
            case LAYTYPE_SIGNAL_TOP:
               buf += "T";
               break;
            case LAYTYPE_SIGNAL_BOT:
               buf += "B";
               break;
            case LAYTYPE_SIGNAL_INNER:
               buf += "I";
               break;
            default:
               buf += "?";
               break;
            }
            buf += DELIM;

            CString layerNum;
            layerNum.Format("%d", data->getLayerIndex());
            buf += layerNum;
         }

         POSITION polyPos = data->getPolyList()->GetHeadPosition();
         while (polyPos != NULL)
         {
            CPoly *poly = data->getPolyList()->GetNext(polyPos);

            POSITION pntPos = poly->getPntList().GetHeadPosition();
            while (pntPos != NULL)
            {
               CPnt *pnt = poly->getPntList().GetNext(pntPos);

               CString pntString;
               pntString.Format("%+.*lf,%+.*lf", decimals, pnt->x, decimals, pnt->y);

               buf += DELIM;
               buf += pntString;
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

/******************************************************************************
* GetNetViaData
*/
short API::GetNetViaData(long filePosition, VARIANT FAR* array) 
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

   BlockStruct *block = file->getBlock();
   int count = 0;
   WORD keyword = doc->IsKeyWord(ATT_NETNAME, TRUE);
   Attrib* attrib;
   
   pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_VIA)
         continue;

      if (!data->getAttributesRef())
         continue;
      
      if (data->getAttributesRef()->Lookup(keyword, attrib))
      {
         CString netname = attrib->getStringValue();

         count++;
      }
   }

   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(array);
   array->vt = VT_ARRAY | VT_BSTR;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_BSTR, 0, count); 

   CString buf;
   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   long i = 0;
   BSTR bstr;
   pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_VIA)
         continue;

      if (!data->getAttributesRef())
         continue;
      
      if (data->getAttributesRef()->Lookup(keyword, attrib))
      {
         // Netname
         buf = attrib->getStringValue();
         buf += DELIM;

         // Refname
         buf += data->getInsert()->getRefname();
         buf += DELIM;

         // surface 
         if (data->getInsert()->getPlacedBottom())
            buf += "B";
         else
            buf += "T";

         buf += DELIM;

         // insertLayer
         CString layerNum;
         layerNum.Format("%d", data->getLayerIndex());
         buf += layerNum;

         buf += DELIM;
         CString pntString;
         pntString.Format("%+.*lf,%+.*lf", decimals, data->getInsert()->getOriginX(), decimals, data->getInsert()->getOriginY());
         buf += pntString;

         bstr = buf.AllocSysString();
         SafeArrayPutElement(psa, &i, bstr);
         SysFreeString(bstr);

         i++;
      }
   }

   array->parray = psa;

   return RC_SUCCESS;
}

/******************************************************************************
* GetPlacementData
*/
short API::GetPlacementData2(long filePosition, short insertType, VARIANT FAR* array) 
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

   BlockStruct *block = file->getBlock();
   int count = 0;
   
   pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (data->getDataType() == T_INSERT && (insertType == -1 || data->getInsert()->getInsertType() == insertType))
         count++;
   }

   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(array);
   array->vt = VT_ARRAY | VT_BSTR;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_BSTR, 0, count); 

   CString buf, buf2;
   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   long i = 0;
   BSTR bstr;
   pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      POSITION tempPos = pos;
      DataStruct *data = block->getDataList().GetNext(pos);

      if (!(data->getDataType() == T_INSERT && (insertType == -1 || data->getInsert()->getInsertType() == insertType)))
         continue;

      // insert type
      buf = insertTypeToDisplayString(data->getInsert()->getInsertType());
      buf += DELIM;

      // refname
      if (data->getInsert()->getRefname())
         buf += data->getInsert()->getRefname();
      buf += DELIM;

      // geomname
      BlockStruct *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
      if (subblock)
         buf += subblock->getName();
      buf += DELIM;

      // geomnum
      buf2.Format("%d", data->getInsert()->getBlockNumber());
      buf += buf2;
      buf += DELIM;

      // x, y
      buf2.Format("%+.*lf,%+.*lf", decimals, data->getInsert()->getOriginX(), decimals, data->getInsert()->getOriginY());
      buf += buf2;
      buf += DELIM;

      // rotation
      buf2.Format("%+.2lf", RadToDeg(data->getInsert()->getAngle()));
      buf += buf2;
      buf += DELIM;

      // scale
      buf2.Format("%.2lf", data->getInsert()->getScale());
      buf += buf2;
      buf += DELIM;

      // surface
      if (data->getInsert()->getPlacedBottom())
         buf += "B";
      else
         buf += "T";
      buf += DELIM;

      // insertLayer
      buf2.Format("%d", data->getLayerIndex());
      buf += buf2;
      buf += DELIM;

      // mirror
      if (data->getInsert()->getMirrorFlags() & MIRROR_FLIP)
         buf += "M";
      /*else
         buf += "";*/
      buf += DELIM;

      // entity
      buf2.Format("%d", data->getEntityNumber());
      buf += buf2;
      buf += DELIM;

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

short API::GetPlacementData(long filePosition, short insertType, VARIANT FAR* array) 
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

   BlockStruct *block = file->getBlock();
   int count = 0;
   
   pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (data->getDataType() == T_INSERT && (insertType == -1 || data->getInsert()->getInsertType() == insertType))
         count++;
   }

   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(array);
   array->vt = VT_ARRAY | VT_BSTR;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_BSTR, 0, count); 

   CString buf, buf2;
   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   long i = 0;
   BSTR bstr;
   pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (!(data->getDataType() == T_INSERT && (insertType == -1 || data->getInsert()->getInsertType() == insertType)))
         continue;

      // insert type
      buf = insertTypeToDisplayString(data->getInsert()->getInsertType());
      buf += DELIM;

      // refname
      if (data->getInsert()->getRefname())
         buf += data->getInsert()->getRefname();
      buf += DELIM;

      // geomname
      BlockStruct *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
      if (subblock)
         buf += subblock->getName();
      buf += DELIM;

      // geomnum
      buf2.Format("%d", data->getInsert()->getBlockNumber());
      buf += buf2;
      buf += DELIM;

      // x, y
      buf2.Format("%+.*lf,%+.*lf", decimals, data->getInsert()->getOriginX(), decimals, data->getInsert()->getOriginY());
      buf += buf2;
      buf += DELIM;

      // rotation
      buf2.Format("%+.2lf", RadToDeg(data->getInsert()->getAngle()));
      buf += buf2;
      buf += DELIM;

      // scale
      buf2.Format("%.2lf", data->getInsert()->getScale());
      buf += buf2;
      buf += DELIM;

      // surface
      if (data->getInsert()->getPlacedBottom())
         buf += "B";
      else
         buf += "T";
      buf += DELIM;

      // insertLayer
      buf2.Format("%d", data->getLayerIndex());
      buf += buf2;
      buf += DELIM;

      // mirror
      if (data->getInsert()->getMirrorFlags() & MIRROR_FLIP)
         buf += "M";
      /*else
         buf += "";*/

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
* GetGeometryData
*/
short API::GetGeometryData(short geometryType, VARIANT FAR* array) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   int count = 0;
	short n = 0;
   for (n=0; n<doc->getMaxBlockIndex(); n++)
   {
      BlockStruct *block = doc->getBlockAt(n);
      if (block == NULL)   continue;

      if (geometryType == -1 || block->getBlockType() == geometryType)
         count++;
   }
   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(array);
   array->vt = VT_ARRAY | VT_BSTR;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_BSTR, 0, count); 

   CString buf, buf2;
   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   long i = 0;
   BSTR bstr;
   for (n=0; n<doc->getMaxBlockIndex(); n++)
   {
      BlockStruct *block = doc->getBlockAt(n);

      if (!block || !(geometryType == -1 || block->getBlockType() == geometryType))
         continue;

      // geomnum
      buf.Format("%d", block->getBlockNumber());
      buf += DELIM;

      // name
      buf += block->getName();
      buf += DELIM;

      // filenum
      buf2.Format("%d", block->getFileNumber());
      buf += buf2;
      buf += DELIM;

      // extents (xmin, xmax, ymin, ymax)
      doc->validateBlockExtents(block);

      // print extents only if they are valid, but never for drill tool blocks
      if (block->extentIsValid() && block->getBlockType() != blockTypeDrillHole)
      {
         buf2.Format("%+.*lf,%+.*lf,%+.*lf,%+.*lf", decimals, block->getXmin(), decimals, block->getXmax(), decimals, block->getYmin(), decimals, block->getYmax());
         buf += buf2;
         buf += DELIM;
      }

      // pins output for all blocks except for drill hole
      if (block->getBlockType() != blockTypeDrillHole)
      {
         // # pins
         int pins = 0;
         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = block->getDataList().GetNext(dataPos);
            if (data->getDataType() == T_INSERT && data->getInsert()->getInsertType() == INSERTTYPE_PIN)
               pins++;
         }
         buf2.Format("%d", pins);
         buf += buf2;

         dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = block->getDataList().GetNext(dataPos);
            if (data->getDataType() == T_INSERT && data->getInsert()->getInsertType() == INSERTTYPE_PIN)
            {
               buf += DELIM;
               if (data->getInsert()->getRefname())
               {
                  buf2.Format("%s", data->getInsert()->getRefname());
                  buf += buf2;
               }

               buf += DELIM;
               buf2.Format("%+.*lf,%+.*lf", decimals, data->getInsert()->getOriginX(), decimals, data->getInsert()->getOriginY());
               buf += buf2;
            }
         }
      }

      // Drill Hole values
      if (block->getBlockType() == blockTypeDrillHole)
      {
         // Tool Type and Comment are constant blanks in Tool List, apparantly features that
         // were not fully implemented or not fully deleted, but unused so not output here.

         // ExportAs (aka TCode), Tool Size, Plated, Display, Geometry (if "complex" type tool display)

         int cmplxblknum = block->getToolBlockNumber();
         CString cmplxblkname;
         BlockStruct *cmplxblk = cmplxblknum != 0 ? doc->getBlockAt(cmplxblknum) : NULL;
         if (cmplxblk != NULL)
            cmplxblkname = cmplxblk->getName();
         

         buf2.Format("T%d%c%f%c%s%c%s%c%s",
            block->getTcode(), DELIM, 
            block->getToolSize(), DELIM,
            block->getToolHolePlated()? "Plated" : "Non-plated", DELIM,
            block->getToolDisplay() ? "Y" : "N", DELIM, 
            cmplxblkname);

         buf += DELIM;
         buf += buf2;
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
* GetNetData
*/
short API::GetNetData(long filePosition, VARIANT FAR* array) 
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

   int count = file->getNetList().GetCount();

   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(array);
   array->vt = VT_ARRAY | VT_BSTR;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_BSTR, 0, count); 

   long i=0;
   BSTR bstr;
   pos = file->getNetList().GetHeadPosition();
   while (pos != NULL)
   {
      NetStruct *net = file->getNetList().GetNext(pos);

      // insert type
      CString buf = net->getNetName();

      // attributes
      if (net->getAttributesRef())
      {
         for (POSITION attribPos = net->getAttributesRef()->GetStartPosition();attribPos != NULL;)
         {
            WORD keyword;
            Attrib* attrib;

            net->getAttributesRef()->GetNextAssoc(attribPos, keyword, attrib);

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
* GetCentroidData
*/
short API::GetCentroidData(long filePosition, short insertType, VARIANT FAR* array) 
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

   BlockStruct *block = file->getBlock();
   int count = 0;
   
   pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (data->getDataType() == T_INSERT && (insertType == -1 || data->getInsert()->getInsertType() == insertType))
         count++;
   }

   if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(array);
   array->vt = VT_ARRAY | VT_BSTR;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_BSTR, 0, count); 

   CString buf, buf2;
   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   long i = 0;
   BSTR bstr;
   pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (!(data->getDataType() == T_INSERT && (insertType == -1 || data->getInsert()->getInsertType() == insertType)))
         continue;

      // insert type
      buf = insertTypeToDisplayString(data->getInsert()->getInsertType());
      buf += DELIM;

      // refname
      if (data->getInsert()->getRefname())
         buf += data->getInsert()->getRefname();
      buf += DELIM;

      // geomname
      BlockStruct *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
      if (subblock)
         buf += subblock->getName();
      buf += DELIM;

      // geomnum
      buf2.Format("%d", data->getInsert()->getBlockNumber());
      buf += buf2;
      buf += DELIM;

      DataStruct *centroid = centroid_exist_in_block(subblock);
      if (centroid)
      {
         // x, y
         Point2 point1;
         point1.bulge = 0;
         // here update position
         point1.x = centroid->getInsert()->getOriginX();
         if (data->getInsert()->getMirrorFlags() & MIRROR_FLIP)
            point1.x = -point1.x;

         point1.y = centroid->getInsert()->getOriginY();

         Mat2x2 mm;
         RotMat2(&mm, data->getInsert()->getAngle());

         TransPoint2(&point1, 1, &mm, 0.0, 0.0);
   
         buf2.Format("%+.*lf,%+.*lf", 
               decimals, data->getInsert()->getOriginX() + point1.x, 
               decimals, data->getInsert()->getOriginY() + point1.y);
         buf += buf2;
         buf += DELIM;

         // rotation
         buf2.Format("%+.2lf", RadToDeg(data->getInsert()->getAngle() + centroid->getInsert()->getAngle()));
         buf += buf2;
         buf += DELIM;
      }  
      else
      {
         // x, y
         buf += DELIM;

         // rotation
         buf += DELIM;
      }

      // scale
      buf2.Format("%.2lf", data->getInsert()->getScale());
      buf += buf2;
      buf += DELIM;

      // surface
      if (data->getInsert()->getPlacedBottom())
         buf += "B";
      else
         buf += "T";
      buf += DELIM;

      // insertLayer
      buf2.Format("%d", data->getLayerIndex());
      buf += buf2;
      buf += DELIM;

      // mirror
      if (data->getInsert()->getMirrorFlags() & MIRROR_FLIP)
         buf += "M";
      /*else
         buf += "";*/
      
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

#include "dft.h"
#include "net_util.h"

short API::GetProbeData(long filePosition, VARIANT FAR* array)
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

   BlockStruct *block = file->getBlock();
   int count = 0;

	pos = block->getHeadDataInsertPosition();
	while (pos != NULL)
	{
		DataStruct *data = block->getNextDataInsert(pos);
		InsertStruct *insert = data->getInsert();

		if (insert->getInsertType() == INSERTTYPE_TEST_PROBE)
			count++;
	}

	if (!count)
      return RC_EMPTY_ARRAY;

   VariantInit(array);
   array->vt = VT_ARRAY | VT_BSTR;
   SAFEARRAY FAR* psa = SafeArrayCreateVector(VT_BSTR, 0, count+1); 

   CString buf, buf2;
   long i = 0;

	{
		// Entity Number
		buf = "Probe Entity Number";
		buf += DELIM;

		// refname
		buf += "Probe Reference Designator";
		buf += DELIM;

		// Netname
		buf += "Access Reference Designator";
		buf += DELIM;

		// Netname
		buf += "Netname";
		buf += DELIM;

		// x, y
		buf += "X Location, Y Location";
		buf += DELIM;

		// surface
		buf += "Surface";
		buf += DELIM;

		// SampleRefDes
		buf += "Sample RefDes-Pin";
		buf += DELIM;

		// Exposed Copper Diameter
		buf += "Exposed Copper Diameter";
		buf += DELIM;

		// Target Type
		buf += "Target Type";
		buf += DELIM;

		// TargetName-Pin
		buf += "TargetName-Pin";
		buf += DELIM;

		// Placed/Unplaced
		buf += "Placed/Unplaced";
		buf += DELIM;

		// Probe Type
		buf += "Probe Type";
		buf += DELIM;

		// Probe Size
		buf += "Probe Size";

		SafeArrayPutElement(psa, &i, buf.AllocSysString());
	}
	
	WORD netKw            = doc->RegisterKeyWord(ATT_NETNAME, 0, VT_STRING);
	WORD expCopperKw      = doc->RegisterKeyWord(DFT_ATT_EXPOSE_METAL_DIAMETER, 0, valueTypeUnitDouble);
	WORD targetTypeKw     = doc->RegisterKeyWord(DFT_ATT_TARGET_TYPE, 0, VT_STRING);
	WORD probePlacementKw = doc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, VT_STRING);
	WORD testResourceKw   = doc->RegisterKeyWord(ATT_TESTRESOURCE, 0, VT_STRING);
	WORD dataLinkKw       = doc->RegisterKeyWord(ATT_DDLINK, 0, VT_INTEGER);
	
   i = 1;
   int decimals = GetDecimals(doc->getSettings().getPageUnits());

   pos = block->getHeadDataInsertPosition();
   while (pos != NULL)
   {
      POSITION tempPos = pos;
      DataStruct *probeData = block->getNextDataInsert(pos);
		InsertStruct *probeInsert = probeData->getInsert();

		if (probeInsert->getInsertType() != INSERTTYPE_TEST_PROBE)
         continue;

		Attrib *attrib = NULL;

		// get access point
		DataStruct *accData = NULL;

		if (probeData->lookUpAttrib(dataLinkKw, attrib))
		{
			EEntityType findType = entityTypeUndefined;
			VOID *voidPtr = FindDataEntity(doc, attrib->getIntValue(), NULL, NULL);
			if (voidPtr)
				accData = (DataStruct*)voidPtr;
		}

		// get feature
      CEntity featureEntity;

		//EEntityType entityType = entityTypeUndefined;
		//VOID *voidPtr = NULL;
		if (accData != NULL && accData->lookUpAttrib(dataLinkKw, attrib))
      {
			//voidPtr = FindEntity(doc, attrib->getIntValue(), entityType);
         featureEntity = CEntity::findEntity(doc->getCamCadData(),attrib->getIntValue());
      }

		CString netName;

		if (probeData->lookUpAttrib(netKw, attrib))
			netName = get_attvalue_string(doc, attrib);

		// Entity Number
      buf2.Format("%d", probeData->getEntityNumber());
      buf = buf2;
		buf += DELIM;

		// Probe Refname
      if (!probeInsert->getRefname().IsEmpty())
         buf += probeInsert->getRefname();

		buf += DELIM;

		// Access Reference Designator
      if (accData != NULL && !accData->getInsert()->getRefname().IsEmpty())
         buf += accData->getInsert()->getRefname();

		buf += DELIM;

		// Netname
		buf += netName;
		buf += DELIM;

		// x, y
      buf2.Format("%+.*lf,%+.*lf", decimals, probeInsert->getOriginX(), decimals, probeInsert->getOriginY());
      buf += buf2;
		buf += DELIM;

		// surface
      if (probeInsert->getPlacedBottom())
         buf += "B";
      else
         buf += "T";

		buf += DELIM;

		// SampleRefDes
		//if (voidPtr != NULL && entityType == entityTypeCompPin)
      if (featureEntity.getEntityType() == entityTypeCompPin)
		{
         CompPinStruct* cp = featureEntity.getCompPin();
			buf += cp->getPinRef('-');
		}
		else
		{
			NetStruct *net = ::FindNet(file, netName);

			if (net != NULL && net->getCompPinCount() > 0)
			{
				CompPinStruct *cp = net->getHeadCompPin();
				buf += cp->getPinRef('-');
			}
		}

		buf += DELIM;

		// Exposed Copper Diameter
		if (accData != NULL && accData->lookUpAttrib(expCopperKw, attrib))
			buf += get_attvalue_string(doc, attrib);
		buf += DELIM;

		// Target Type
		if (accData != NULL && accData->lookUpAttrib(targetTypeKw, attrib))
			buf += get_attvalue_string(doc, attrib);
		buf += DELIM;

		// TargetName-Pin
		if (featureEntity.getEntityType() == entityTypeCompPin)
		{
         CompPinStruct* cp = featureEntity.getCompPin();
			buf += cp->getPinRef('-');
		}
		else if (featureEntity.getEntityType() == entityTypeData)
		{
			//DataStruct *insData = (DataStruct*)voidPtr;
         DataStruct* insData = featureEntity.getData();

         if (insData != NULL && insData->getDataType() == dataTypeInsert)
         {
				buf += insData->getInsert()->getRefname() + "-1";
         }
		}

		buf += DELIM;

		// Placed/Unplaced
		if (probeData->lookUpAttrib(probePlacementKw, attrib))
			buf += get_attvalue_string(doc, attrib);

		buf += DELIM;

		// Probe Type
		if (probeData->lookUpAttrib(testResourceKw, attrib))
			buf += get_attvalue_string(doc, attrib);

		buf += DELIM;
      
		BlockStruct *probeBlock = doc->getBlockAt(probeInsert->getBlockNumber());

		if (probeBlock != NULL)
		{
			CString blockName = probeBlock->getName();
			buf += blockName.Mid(0, blockName.ReverseFind('_'));
		}

		SafeArrayPutElement(psa, &i, buf.AllocSysString());

      i++;
   }

	V_ARRAY(array) = psa;

   return RC_SUCCESS;
}
short API::GetProjectPathAPI(BSTR FAR* ProjectPath) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();
   *ProjectPath = doc->GetProjectPath().AllocSysString();

   return RC_SUCCESS;
}

