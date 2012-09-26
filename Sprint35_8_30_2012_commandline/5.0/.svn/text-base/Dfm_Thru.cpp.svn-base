// $Header: /CAMCAD/4.6/Dfm_Thru.cpp 38    1/15/07 5:08p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "dfm.h"
#include "lic.h"
#include "crypt.h"
#include "api.h"
#include "ccview.h"
#include "drc.h"
#include "extents.h"
#include "CCEtoODB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CCEtoODBView *apiView; // from API.CPP
extern char *FailureRanges[]; // from DRC.CPP

/******************************************************************************
* DFM::ViaPadSize
*/
SHORT DFM::ViaPadSize(LONG filePosition, DOUBLE topMinSize, DOUBLE topMaxSize, DOUBLE bottomMinSize, DOUBLE bottomMaxSize, 
      DOUBLE ignoreHoleSize, LPCTSTR algName)
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

   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);

   int algIndex = GetAlgorithmNameIndex(doc, algName);
   int decimals = GetDecimals(doc->getSettings().getPageUnits());

   CMapWordToPtr map;

   POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT || data->getInsert()->getInsertType() != INSERTTYPE_VIA)
         continue;

      void *voidPtr;
      if (!map.Lookup(data->getInsert()->getBlockNumber(), voidPtr))
         map.SetAt(data->getInsert()->getBlockNumber(), data);
   }

   POSITION mapPos = map.GetStartPosition();
   while (mapPos)
   {
      WORD geomNum;
      void *voidPtr;
      map.GetNextAssoc(mapPos, geomNum, voidPtr);
      DataStruct *viaInsert = (DataStruct*)voidPtr;

      BlockStruct *viaGeom = doc->getBlockAt(geomNum);

      DataStruct *topInsert = NULL, *bottomInsert = NULL, *drillInsert = NULL;

      POSITION dataPos = viaGeom->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = viaGeom->getDataList().GetNext(dataPos);

         if (data->getDataType() != T_INSERT)
            continue;

         switch (doc->getLayerArray()[data->getLayerIndex()]->getLayerType())
         {
         case LAYTYPE_PAD_TOP:
         case LAYTYPE_SIGNAL_TOP:
            topInsert = data;
            break;
         case LAYTYPE_PAD_BOTTOM:
         case LAYTYPE_SIGNAL_BOT:
            bottomInsert = data;
            break;
         case LAYTYPE_DRILL:
            drillInsert = data;
            break;
         }        
      }

      if (!drillInsert)
         continue;

      BlockStruct *drillGeom = doc->getBlockAt(drillInsert->getInsert()->getBlockNumber());
      if (drillGeom->getToolSize() < ignoreHoleSize)
         continue;

      if (topInsert)
      {
         BlockStruct *topPad = doc->getBlockAt(topInsert->getInsert()->getBlockNumber());

         if (topPad->getSizeA() < topMinSize)
         {
            CString drcString;
            drcString.Format("Via Top Pad Size %.*lf < %.*lf", decimals, topPad->getSizeA(), decimals, topMinSize);

            DRCStruct *drc = AddDRC(doc, file, viaInsert->getInsert()->getOriginX(), viaInsert->getInsert()->getOriginY(), drcString, DRC_CLASS_SIMPLE, 0, algIndex, 0);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, FailureRanges[0], SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &topMinSize, SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), (float)topPad->getSizeA(), SA_OVERWRITE, NULL);
         }

         if (topPad->getSizeA() > topMaxSize)
         {
            CString drcString;
            drcString.Format("Via Top Pad Size %.*lf > %.*lf", decimals, topPad->getSizeA(), decimals, topMaxSize);

            DRCStruct *drc = AddDRC(doc, file, viaInsert->getInsert()->getOriginX(), viaInsert->getInsert()->getOriginY(), drcString, DRC_CLASS_SIMPLE, 0, algIndex, 0);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, FailureRanges[0], SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &topMaxSize, SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), (float)topPad->getSizeA(), SA_OVERWRITE, NULL);
         }
      }

      if (bottomInsert)
      {
         BlockStruct *bottomPad = doc->getBlockAt(bottomInsert->getInsert()->getBlockNumber());

         if (bottomPad->getSizeA() < bottomMinSize)
         {
            CString drcString;
            drcString.Format("Via Bottom Pad Size %.*lf < %.*lf", decimals, bottomPad->getSizeA(), decimals, bottomMinSize);

            DRCStruct *drc = AddDRC(doc, file, viaInsert->getInsert()->getOriginX(), viaInsert->getInsert()->getOriginY(), drcString, DRC_CLASS_SIMPLE, 0, algIndex, 0);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, FailureRanges[0], SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &bottomMinSize, SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), (float)bottomPad->getSizeA(), SA_OVERWRITE, NULL);
         }

         if (bottomPad->getSizeA() > bottomMaxSize)
         {
            CString drcString;
            drcString.Format("Via Bottom Pad Size %.*lf > %.*lf", decimals, bottomPad->getSizeA(), decimals, bottomMaxSize);

            DRCStruct *drc = AddDRC(doc, file, viaInsert->getInsert()->getOriginX(), viaInsert->getInsert()->getOriginY(), drcString, DRC_CLASS_SIMPLE, 0, algIndex, 0);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, FailureRanges[0], SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &bottomMaxSize, SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), (float)bottomPad->getSizeA(), SA_OVERWRITE, NULL);
         }
      }
   }

   return RC_SUCCESS;
}

/******************************************************************************
* DFM::ViaHoleSize
*/
SHORT DFM::ViaHoleSize(LONG filePosition, DOUBLE minSize, DOUBLE maxSize, DOUBLE ignoreHoleSize, LPCTSTR algName)
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

   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);

   int algIndex = GetAlgorithmNameIndex(doc, algName);
   int decimals = GetDecimals(doc->getSettings().getPageUnits());

   CMapWordToPtr map;

   POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT || data->getInsert()->getInsertType() != INSERTTYPE_VIA)
         continue;

      void *voidPtr;
      if (!map.Lookup(data->getInsert()->getBlockNumber(), voidPtr))
         map.SetAt(data->getInsert()->getBlockNumber(), data);
   }

   POSITION mapPos = map.GetStartPosition();
   while (mapPos)
   {
      WORD geomNum;
      void *voidPtr;
      map.GetNextAssoc(mapPos, geomNum, voidPtr);
      DataStruct *viaInsert = (DataStruct*)voidPtr;

      BlockStruct *viaGeom = doc->getBlockAt(geomNum);

      DataStruct *drillInsert = NULL;

      POSITION dataPos = viaGeom->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = viaGeom->getDataList().GetNext(dataPos);

         if (data->getDataType() != T_INSERT)
            continue;

         if (doc->getLayerArray()[data->getLayerIndex()]->getLayerType() == LAYTYPE_DRILL)
         {
            drillInsert = data;
            break;
         }        
      }

      if (!drillInsert)
         continue;
      
      if (drillInsert)
      {
         BlockStruct *drillGeom = doc->getBlockAt(drillInsert->getInsert()->getBlockNumber());
         if (drillGeom->getToolSize() < ignoreHoleSize)
            continue;

         if (drillGeom->getToolSize() < minSize)
         {
            CString drcString;
            drcString.Format("Via Hole Size %.*lf < %.*lf", decimals, drillGeom->getToolSize(), decimals, minSize);

            DRCStruct *drc = AddDRC(doc, file, viaInsert->getInsert()->getOriginX(), viaInsert->getInsert()->getOriginY(), drcString, DRC_CLASS_SIMPLE, 0, algIndex, 0);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, FailureRanges[0], SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &minSize, SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), (float)drillGeom->getToolSize(), SA_OVERWRITE, NULL);
         }

         if (drillGeom->getToolSize() > maxSize)
         {
            CString drcString;
            drcString.Format("Via Hole Size %.*lf > %.*lf", decimals, drillGeom->getToolSize(), decimals, maxSize);

            DRCStruct *drc = AddDRC(doc, file, viaInsert->getInsert()->getOriginX(), viaInsert->getInsert()->getOriginY(), drcString, DRC_CLASS_SIMPLE, 0, algIndex, 0);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, FailureRanges[0], SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &maxSize, SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), (float)drillGeom->getToolSize(), SA_OVERWRITE, NULL);
         }
      }
   }

   return RC_SUCCESS;
}

/******************************************************************************
* DFM::BoardThicknessToDrillDiameter
*/
SHORT DFM::BoardThicknessToDrillDiameter(LONG filePosition, DOUBLE maxRatio, LPCTSTR algName)
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

   POSITION pos = (POSITION)filePosition;

   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;

   FileStruct *file = doc->getFileList().GetAt(pos);

   WORD keyword = doc->IsKeyWord(BOARD_THICKNESS, 0);
   Attrib* attrib;

   if (!file->getBlock()->getAttributesRef() || !file->getBlock()->getAttributesRef()->Lookup(keyword, attrib))
      return RC_NO_ATTRIBS;

   double boardThickness = attrib->getDoubleValue();

   int algIndex = GetAlgorithmNameIndex(doc, algName);
   int decimals = GetDecimals(doc->getSettings().getPageUnits());

   for (int i=0; i<doc->getNextWidthIndex(); i++)
   {
      BlockStruct *block = doc->getWidthTable()[i];

      if (!block)
         continue;

      if (!(block->getFlags() & BL_TOOL) && !(block->getFlags() & BL_BLOCK_TOOL))
         continue;

      double ratio = boardThickness / block->getToolSize();

      if (ratio > maxRatio)
      {
         CString drcString;
         drcString.Format("%s size %.*lf / %.*lf = %.*lf > %.*lf", block->getName(), decimals, block->getToolSize(), decimals, boardThickness, decimals, ratio, decimals, maxRatio);
         DRCStruct *drc = CreateDRC(file, drcString, DRC_CLASS_NO_MARKER, 0, algIndex, 0);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, FailureRanges[0], SA_OVERWRITE, NULL);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &maxRatio, SA_OVERWRITE, NULL);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), VT_UNIT_DOUBLE, &ratio, SA_OVERWRITE, NULL);
      }
   }

   return RC_SUCCESS;
}

/******************************************************************************
* Support for Annular Ring test
*/

void DFM::AnnularRingPadDrillCheck(CCEtoODBDoc *doc, FileStruct *file, CString padDescription, DataStruct *compData, DataStruct *pinData, DataStruct *padData, double x, double y, double drillSize, double minAllowable, LPCTSTR algName)
{
	if (compData != NULL && pinData != NULL && padData != NULL)
	{
		BlockStruct *pad = doc->getBlockAt(padData->getInsert()->getBlockNumber());

		if (pad != NULL)
		{
			int algIndex = GetAlgorithmNameIndex(doc, algName);
			int decimals = GetDecimals(doc->getSettings().getPageUnits());

			// Case 1844 - Ring size is based on radius
			double ringSize = (pad->getSizeA() - drillSize) / 2.0;

			if (ringSize < minAllowable)
			{
				CString drcString;
				drcString.Format("%s-%s %s Ring Size %.*lf < %.*lf", 
					compData->getInsert()->getRefname(), pinData->getInsert()->getRefname(),
					padDescription,
					decimals, ringSize, decimals, minAllowable);

				DRCStruct *drc = AddDRC(doc, file, x, y, drcString, DRC_CLASS_SIMPLE, 0, algIndex, 0);
				doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
				doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, FailureRanges[0], SA_OVERWRITE, NULL);
				doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &minAllowable, SA_OVERWRITE, NULL);
				doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), VT_UNIT_DOUBLE, &ringSize, SA_OVERWRITE, NULL);
			}
		}
	}
}

void DFM::AnnularRingReportAbsentPad(CCEtoODBDoc *doc, FileStruct *file, CString padDescription, DataStruct *compData, DataStruct *pinData, DataStruct *padData, double x, double y, double drillSize, double minAllowable, LPCTSTR algName)
{
	if (compData != NULL && pinData != NULL)
	{
		int algIndex = GetAlgorithmNameIndex(doc, algName);
		int decimals = GetDecimals(doc->getSettings().getPageUnits());

		double ringSize = 0.0;

		CString drcString;
		drcString.Format("%s-%s %s Absent", 
			compData->getInsert()->getRefname(), pinData->getInsert()->getRefname(),
			padDescription);

		DRCStruct *drc = AddDRC(doc, file, x, y, drcString, DRC_CLASS_SIMPLE, 0, algIndex, 0);
		doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
		doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, FailureRanges[0], SA_OVERWRITE, NULL);
		doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &minAllowable, SA_OVERWRITE, NULL);
		doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), VT_UNIT_DOUBLE, &ringSize, SA_OVERWRITE, NULL);
	}
}

/******************************************************************************
* DFM::AnnularRing
*/
SHORT DFM::AnnularRing(LONG filePosition, DOUBLE padTop, DOUBLE padBottom, DOUBLE soldTop, DOUBLE soldBottom, LPCTSTR algName)
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

   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);

   int algIndex = GetAlgorithmNameIndex(doc, algName);
   int decimals = GetDecimals(doc->getSettings().getPageUnits());


   POSITION compDataPos = file->getBlock()->getDataList().GetHeadPosition();
   while (compDataPos)
   {
      DataStruct *compInsert = file->getBlock()->getDataList().GetNext(compDataPos);

      if (compInsert->getDataType() != dataTypeInsert || compInsert->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;
   
      BlockStruct *compGeom = doc->getBlockAt(compInsert->getInsert()->getBlockNumber());
      Mat2x2 m;
      RotMat2(&m, compInsert->getInsert()->getAngle());

      CMapWordToPtr map;

      POSITION pinDataPos = compGeom->getDataList().GetHeadPosition();
      while (pinDataPos)
      {
         DataStruct *pinInsert = compGeom->getDataList().GetNext(pinDataPos);

         if (pinInsert->getDataType() != T_INSERT || pinInsert->getInsert()->getInsertType() != INSERTTYPE_PIN)
            continue;

         void *voidPtr;
         if (!map.Lookup(pinInsert->getInsert()->getBlockNumber(), voidPtr))
            map.SetAt(pinInsert->getInsert()->getBlockNumber(), pinInsert);
      }

      POSITION mapPos = map.GetStartPosition();
      while (mapPos)
      {
         WORD geomNum;
         void *voidPtr;
         map.GetNextAssoc(mapPos, geomNum, voidPtr);
         DataStruct *pinInsert = (DataStruct*)voidPtr;

         BlockStruct *pinGeom = doc->getBlockAt(geomNum);

         DataStruct *topPadInsert = NULL, *bottomPadInsert = NULL, *topSoldInsert = NULL, *bottomSoldInsert = NULL, *drillInsert = NULL;

         POSITION padDataPos = pinGeom->getDataList().GetHeadPosition();
         while (padDataPos)
         {
            DataStruct *padData = pinGeom->getDataList().GetNext(padDataPos);

            if (padData->getDataType() != T_INSERT)
               continue;

            switch (doc->getLayerArray()[padData->getLayerIndex()]->getLayerType())
            {
            case LAYTYPE_PAD_TOP:
               topPadInsert = padData;
               break;
            case LAYTYPE_PAD_BOTTOM:
               bottomPadInsert = padData;
               break;
            case LAYTYPE_MASK_TOP:
               topSoldInsert = padData;
               break;
            case LAYTYPE_MASK_BOTTOM:
               bottomSoldInsert = padData;
               break;
            case LAYTYPE_DRILL:
               drillInsert = padData;
               break;
            }        
         }

         if (!drillInsert)
            continue;

         BlockStruct *drillGeom = doc->getBlockAt(drillInsert->getInsert()->getBlockNumber());
         double drillSize = drillGeom->getToolSize();

         Point2 point2;
         point2.x = pinInsert->getPoint()->x;
         point2.y = pinInsert->getPoint()->y;
         if (compInsert->getInsert()->getMirrorFlags() & MIRROR_FLIP)
            point2.x = -point2.x;
         TransPoint2(&point2, 1, &m, compInsert->getInsert()->getOriginX(), compInsert->getInsert()->getOriginY());


			if (topPadInsert)
				AnnularRingPadDrillCheck(doc, file, "Top Pad", compInsert, pinInsert, topPadInsert, point2.x, point2.y, drillSize, padTop, algName);
			else
				AnnularRingReportAbsentPad(doc, file, "Top Pad", compInsert, pinInsert, topPadInsert, point2.x, point2.y, drillSize, padTop, algName);


			if (bottomPadInsert)
				AnnularRingPadDrillCheck(doc, file, "Bottom Pad", compInsert, pinInsert, bottomPadInsert, point2.x, point2.y, drillSize, padBottom, algName);
			else
				AnnularRingReportAbsentPad(doc, file, "Bottom Pad", compInsert, pinInsert, bottomPadInsert, point2.x, point2.y, drillSize, padBottom, algName);


			if (topSoldInsert)
				AnnularRingPadDrillCheck(doc, file, "Top Soldermask", compInsert, pinInsert, topSoldInsert, point2.x, point2.y, drillSize, soldTop, algName);
			else
				AnnularRingReportAbsentPad(doc, file, "Top Soldermask", compInsert, pinInsert, topSoldInsert, point2.x, point2.y, drillSize, soldTop, algName);


			if (bottomSoldInsert)
				AnnularRingPadDrillCheck(doc, file, "Bottom Soldermask", compInsert, pinInsert, bottomSoldInsert, point2.x, point2.y, drillSize, soldBottom, algName);
			else
				AnnularRingReportAbsentPad(doc, file, "Bottom Soldermask", compInsert, pinInsert, bottomSoldInsert, point2.x, point2.y, drillSize, soldBottom, algName);

      }
   }

   return RC_SUCCESS;
}
