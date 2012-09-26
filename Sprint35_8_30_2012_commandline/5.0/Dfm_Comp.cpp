// $Header: /CAMCAD/5.0/Dfm_Comp.cpp 41    6/17/07 8:51p Kurt Van Ness $

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
#include "xform.h"
#include "CCEtoODB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CCEtoODBView *apiView; // from API.CPP
extern char *FailureRanges[]; // from DRC.CPP


static void CheckAllowableOrientation(CCEtoODBDoc *doc, FileStruct *file, BlockStruct *boardGeom, double insert_x, double insert_y, double rotation, int mirror, 
      BOOL top, BOOL horizontal, CString algName, CString prefix);

/******************************************************************************
* DFM::ThruHolePadShapes
*/
SHORT DFM::ThruHolePadShapes(LONG filePosition, SHORT topAll, SHORT bottomAll, SHORT topOne, SHORT bottomOne, LPCTSTR algName)
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

   POSITION compPos = file->getBlock()->getDataList().GetHeadPosition();
   while (compPos)
   {
      DataStruct *compInsert = file->getBlock()->getDataList().GetNext(compPos);

      if (compInsert->getDataType() != dataTypeInsert || compInsert->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;

      BlockStruct *compGeometry = doc->getBlockAt(compInsert->getInsert()->getBlockNumber());
      double insert_x = compInsert->getInsert()->getOriginX();
      double insert_y = compInsert->getInsert()->getOriginY();
      double insert_rot = compInsert->getInsert()->getAngle();
      BOOL insert_mirror = compInsert->getInsert()->getMirrorFlags() & MIRROR_FLIP;

      POSITION pinPos = compGeometry->getDataList().GetHeadPosition();
      while (pinPos)
      {
         DataStruct *pinInsert = compGeometry->getDataList().GetNext(pinPos);

         if (pinInsert->getDataType() != T_INSERT || pinInsert->getInsert()->getInsertType() != INSERTTYPE_PIN)
            continue;

         Point2 point2;
         Mat2x2 m;
         RotMat2(&m, insert_rot);
         point2.x = pinInsert->getInsert()->getOriginX();
         if (insert_mirror)
            point2.x = -point2.x;
         point2.y = pinInsert->getInsert()->getOriginY();
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         BOOL pinOne = !strcmp(pinInsert->getInsert()->getRefname(), "1");

         BlockStruct *pinGeometry = doc->getBlockAt(pinInsert->getInsert()->getBlockNumber());

         BlockStruct *topPad = NULL, *bottomPad = NULL, *drillGeom = NULL;

         POSITION padPos = pinGeometry->getDataList().GetHeadPosition();
         while (padPos)
         {
            DataStruct *padInsert = pinGeometry->getDataList().GetNext(padPos);

            if (padInsert->getDataType() != dataTypeInsert)
               continue;

            switch (doc->getLayerArray()[padInsert->getLayerIndex()]->getLayerType())
            {
            case LAYTYPE_PAD_TOP:
               topPad = doc->getBlockAt(padInsert->getInsert()->getBlockNumber());
               break;
            case LAYTYPE_PAD_BOTTOM:
               bottomPad = doc->getBlockAt(padInsert->getInsert()->getBlockNumber());
               break;
            case LAYTYPE_DRILL:
               drillGeom = doc->getBlockAt(padInsert->getInsert()->getBlockNumber());
               break;
            }
         }

         if (!drillGeom)
            continue;

         if (insert_mirror)
         {
            BlockStruct *temp = topPad;
            topPad = bottomPad;
            bottomPad = temp;          
         }

         if (topPad && !(topPad->getShape() == topAll || (pinOne && topPad->getShape() == topOne)) )
         {
            CString drcString;
            drcString.Format("%s-%s Top Pad is a %s", compInsert->getInsert()->getRefname(), pinInsert->getInsert()->getRefname(), shapes[topPad->getShape()]);

            DRCStruct *drc = AddDRC(doc, file, point2.x, point2.y, drcString, DRC_CLASS_SIMPLE, 0, algIndex, 0);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, FailureRanges[0], SA_OVERWRITE, NULL);
         }

         if (bottomPad && !(bottomPad->getShape() == bottomAll || (pinOne && bottomPad->getShape() == bottomOne)) )
         {
            CString drcString;
            drcString.Format("%s-%s Bottom Pad is a %s", compInsert->getInsert()->getRefname(), pinInsert->getInsert()->getRefname(), shapes[bottomPad->getShape()]);

            DRCStruct *drc = AddDRC(doc, file, point2.x, point2.y, drcString, DRC_CLASS_SIMPLE, 0, algIndex, 0);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, FailureRanges[0], SA_OVERWRITE, NULL);
         }
      }
   }

   return RC_SUCCESS;
}

/******************************************************************************
* DFM::AllowableComponentOrientation
*/
SHORT DFM::AllowableComponentOrientation(LONG filePosition, SHORT top, SHORT horizontal, LPCTSTR algName)
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


   int direction = GetDirectionOfTravel(doc, file->getBlock());

   if (direction == 3 || direction == 4)
      horizontal = !horizontal;

   switch (file->getBlockType())
   {
   case BLOCKTYPE_PCB:
      CheckAllowableOrientation(doc, file, file->getBlock(), 0, 0, 0, 0, top, horizontal, algName, "");
      break;

   case BLOCKTYPE_PANEL:
      {
         POSITION pcbPos = file->getBlock()->getDataList().GetHeadPosition();
         while (pcbPos)
         {
            DataStruct *pcbInsert = file->getBlock()->getDataList().GetNext(pcbPos);

            if (pcbInsert->getDataType() != dataTypeInsert)
               continue;

            if (pcbInsert->getInsert()->getInsertType() != INSERTTYPE_PCB)
               continue;

            BOOL tempTop = top;
            if (pcbInsert->getInsert()->getMirrorFlags() & MIRROR_FLIP)
               tempTop = !tempTop;

            CString prefix;
            prefix.Format("%s,", pcbInsert->getInsert()->getRefname());

            CheckAllowableOrientation(doc, file, doc->getBlockAt(pcbInsert->getInsert()->getBlockNumber()), 
                  pcbInsert->getInsert()->getOriginX(), pcbInsert->getInsert()->getOriginY(), pcbInsert->getInsert()->getAngle(), pcbInsert->getInsert()->getMirrorFlags(), 
                  tempTop, horizontal, algName, prefix);
         }
      }
      break;

   default:
      return RC_WRONG_TYPE;
   }

   return RC_SUCCESS;
}

/******************************************************************************
* CheckAllowableOrientation
*/
static void CheckAllowableOrientation(CCEtoODBDoc *doc, FileStruct *file, BlockStruct *boardGeom, double insert_x, double insert_y, double rotation, int mirror, 
      BOOL top, BOOL horizontal, CString algName, CString prefix)
{
   int algIndex = GetAlgorithmNameIndex(doc, algName);

   WORD SMDKeyword = doc->IsKeyWord(ATT_SMDSHAPE, 0);
   Attrib* attrib;

   POSITION compPos = boardGeom->getDataList().GetHeadPosition();
   while (compPos)
   {
      DataStruct *compInsert = boardGeom->getDataList().GetNext(compPos);

      if (compInsert->getDataType() != dataTypeInsert || compInsert->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;

      BOOL isSMD = compInsert->getAttributesRef() && compInsert->getAttributesRef()->Lookup(SMDKeyword, attrib);

      if (top)
      {
         if (compInsert->getInsert()->getMirrorFlags() & MIRROR_FLIP || isSMD)
            continue;
      }
      else
      {
         if (!(compInsert->getInsert()->getMirrorFlags() & MIRROR_FLIP) || !isSMD)
            continue;
      }

      int compRotation;

      if (mirror)
         compRotation = round(RadToDeg(compInsert->getInsert()->getAngle() - rotation));
      else
         compRotation = round(RadToDeg(compInsert->getInsert()->getAngle() + rotation));
         
      int tempRotation = compRotation;

      if (!horizontal)
         tempRotation -= 90;
      
      if (!(tempRotation % 180))
         continue;

		// Case 1129 - Need to normalize the rotation for reporting purposes
		compRotation = (int)normalizeDegrees((double)compRotation);

		CString drcString;
      drcString.Format("%s%s is at %d degrees", prefix, compInsert->getInsert()->getRefname(), compRotation);

      Point2 point;
      point.x = compInsert->getInsert()->getOriginX();
      point.y = compInsert->getInsert()->getOriginY();

      DTransform xform(insert_x, insert_y, 1, rotation, mirror);
      xform.TransformPoint(&point);


      DRCStruct *drc = AddDRC(doc, file, point.x, point.y, drcString, DRC_CLASS_SIMPLE, 0, algIndex, 0);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName.GetBuffer(0), SA_OVERWRITE, NULL);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, FailureRanges[0], SA_OVERWRITE, NULL);
   }
}

/******************************************************************************
* DFM::SMDSoldermaskClearance
*/
SHORT DFM::SMDSoldermaskClearance(SHORT pinGeometryNumber, SHORT soldermaskGeometryNumber, DOUBLE minXMargin, DOUBLE minYMargin, 
      LPCTSTR algName)
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

   if (soldermaskGeometryNumber < 0 || soldermaskGeometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;
   if (pinGeometryNumber < 0 || pinGeometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *soldermaskGeometry = doc->getBlockAt(soldermaskGeometryNumber);
   BlockStruct *pinGeometry = doc->getBlockAt(pinGeometryNumber);

   if (!soldermaskGeometry)
      return RC_HOLE_IN_ARRAY;
   if (!pinGeometry)
      return RC_HOLE_IN_ARRAY;

   FileStruct *file = doc->Find_File(pinGeometry->getFileNumber());
   int algIndex = GetAlgorithmNameIndex(doc, algName);
   WORD refnameKeyword = doc->IsKeyWord(ATT_REFNAME, 0);
   Mat2x2 m;
   RotMat2(&m, 0);

   CString copperPinRef,maskPinRef;
   Attrib* attribute;
   ExtentRect pinExtents,maskExtents;
   
   for (POSITION pinPos = pinGeometry->getDataList().GetTailPosition();pinPos != NULL;) // for each copper data
   {
      DataStruct *pinData = pinGeometry->getDataList().GetPrev(pinPos);

      if (pinData->getAttributesRef() != NULL && pinData->getAttributesRef()->Lookup(refnameKeyword,attribute))
      {
         copperPinRef = attribute->getStringValue();
      }
      else
      {
         copperPinRef.Empty();
      }

      PolyExtents(doc, pinData->getPolyList(), &pinExtents, 1, 0, 0, 0, &m, FALSE); 

      bool covered = false;
      
      for (POSITION maskPos = soldermaskGeometry->getDataList().GetHeadPosition();maskPos != NULL;) // for each soldermask
      {
         DataStruct *maskData = soldermaskGeometry->getDataList().GetNext(maskPos);

         if (pinData->getAttributesRef() != NULL && pinData->getAttributesRef()->Lookup(refnameKeyword,attribute))
         {
            maskPinRef = attribute->getStringValue();
         }
         else
         {
            maskPinRef.Empty();
         }

         PolyExtents(doc, maskData->getPolyList(), &maskExtents, 1, 0, 0, 0, &m, FALSE);

         if (maskExtents.right  > pinExtents.right  + minXMargin && 
             maskExtents.left   < pinExtents.left   - minXMargin && 
             maskExtents.top    > pinExtents.top    + minYMargin && 
             maskExtents.bottom < pinExtents.bottom - minYMargin)
         {
            covered = true;
            break;
         }
      }

      if (!covered)
      {
         CString refDes = "Unknown";
         Attrib* attrib;

         if (pinData->getAttributesRef() && pinData->getAttributesRef()->Lookup(refnameKeyword, attrib))
            refDes = attrib->getStringValue();

         CString drcString;
         drcString.Format("%s not sufficiently covered by Soldermask", refDes);

         double x = (pinExtents.left + pinExtents.right) / 2;
         double y = (pinExtents.bottom + pinExtents.top) / 2;

         DRCStruct *drc = AddDRC(doc, file, x, y, drcString, DRC_CLASS_SIMPLE, 0, algIndex, 0);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, FailureRanges[0], SA_OVERWRITE, NULL);
      }
   }

   return RC_SUCCESS;
}
