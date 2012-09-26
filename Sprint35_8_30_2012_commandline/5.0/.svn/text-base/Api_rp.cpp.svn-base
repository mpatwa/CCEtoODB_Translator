
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/           

#include "stdafx.h"
#include "API.h"
#include "CCEtoODB.h"
#include "ccview.h"
#include "realpart.h"
#include "crypt.h"
#include "drc.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
#endif

extern CCEtoODBView *apiView; // from API.CPP
extern BOOL API_AutoRedraw; // from API.CPP

#define ATT_REALPART_DRC_REFDES "RealPart DRC RefDes"

/******************************************************************************
* GetRealPartData
*/
short API::GetRealPartData(BSTR FAR* data)
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   CRealPartWrite rpWriter(doc);
   rpWriter.GatherRPData();

   ULONGLONG buflen = rpWriter.GetLength();
   if (buflen == 0)
      return RC_ITEM_NOT_FOUND;

   CString finalXMLString = (CString)rpWriter.Detach();
   finalXMLString.GetBufferSetLength((int)buflen);

   *data = finalXMLString.AllocSysString();

   return RC_SUCCESS;
}

#include "xml_read.h"
/******************************************************************************
* SetRealPartData
*
* - reader.parse() in XML_READ.CPP
*   - XML() fills kwvalMap
*	   - XMLRealPartContent Class in XMLREALPARTCONTENT.CPP 
        and derived from XMLContent in XML_CONT.CPP
*/
short API::SetRealPartData(LPCTSTR data) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductRealPartPackage) && !getApp().getCamcadLicense().isLicensed(camcadProductRealPartDevice))
		return RC_NO_LICENSE;*/

	if (apiView == NULL)
		return RC_NO_ACTIVE_DOC;

	CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
	bool lastUseDialogs = pApp->getUseDialogsFlag();
	pApp->setUseDialogsFlag(false);

	CCEtoODBDoc *doc = apiView->GetDocument();

   RealPartXMLReader reader(*doc);
	FileStatusTag retval = reader.open(data);

	if (retval == statusSucceeded)
	{
		//reader.setDoc(doc);

		retval = reader.parse();
	}

	pApp->setUseDialogsFlag(lastUseDialogs);

	return RC_SUCCESS;
}

/******************************************************************************
* API::AddRealPartDrcMarker()
*/
short API::AddRealPartDrcMarker(long filePosition, LPCTSTR refDes, LPCTSTR packageName, LPCTSTR algorithmName, float x, float y, short failureRange, short priority, LPCTSTR comment, long FAR* entityNumber)
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

   int algIndex = GetAlgorithmNameIndex(doc, algorithmName);

	CString layerName = "RealPart Critical DRC";
	if (failureRange == 1)
		layerName = "RealPart Marginal DRC";

	CString failureMessage = packageName;
	failureMessage.Append(" -> ");
	failureMessage.Append(refDes);

   DataStruct *data;
   DRCStruct *drc = AddDRCAndMarker(doc, file, x, y, failureMessage, DRC_CLASS_SIMPLE, failureRange, algIndex, DRC_ALG_GENERIC, layerName, &data);
	drc->setComment(comment);
	drc->setPriority(priority);
	doc->SetAttrib(&drc->getAttributesRef(), doc->RegisterKeyWord(ATT_REALPART_DRC_REFDES, 0, VT_STRING), VT_STRING, (void*)refDes, SA_OVERWRITE, NULL);

   *entityNumber = drc->getEntityNumber();

   if (API_AutoRedraw)
      doc->UpdateAllViews(NULL);

   return RC_SUCCESS;
}

/******************************************************************************
* API::RemoveRealPartDrcMarker()
*/
short API::RemoveRealPartDrcMarker(long filePosition, LPCTSTR refDes)
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;


	// doc
   CCEtoODBDoc *doc = apiView->GetDocument();


	// file
   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);


	// attrib
	Attrib* attrib;
	WORD keyword = doc->IsKeyWord(ATT_REALPART_DRC_REFDES, 0);

	if (keyword < 0)
		return RC_ITEM_NOT_FOUND;

	POSITION drcPos = file->getDRCList().GetHeadPosition();
	while (drcPos != NULL)
   {
		POSITION tempPos = drcPos;
      DRCStruct *drc = file->getDRCList().GetNext(drcPos);

		if (drc->getAttributesRef())
		{
			if (drc->getAttributesRef()->Lookup(keyword, attrib))
			{
				CString attribRefDes = attrib->getStringValue();

				if (!strcmp(refDes, attribRefDes))
				{
					RemoveOneDRC(doc, tempPos, file);
					break;
				}
			}
		}
	}

   if (API_AutoRedraw)
      doc->UpdateAllViews(NULL);

   return RC_SUCCESS;
}

