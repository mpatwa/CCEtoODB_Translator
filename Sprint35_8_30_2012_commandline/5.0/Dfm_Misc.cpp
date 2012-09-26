// $Header: /CAMCAD/5.0/Dfm_Misc.cpp 55    6/17/07 8:51p Kurt Van Ness $

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
#include "pcbutil.h"
#include "graph.h"
#include "CCEtoODB.h"
#include "CamCadDatabase.h"
#include "DcaGeomLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CCEtoODBView *apiView; // from API.CPP
extern int maxErrorsPerTest; // from DFM.CPP
extern char *FailureRanges[]; // from DRC.CPP

void DFM_CompShadow(CCEtoODBDoc *doc, FileStruct *file, 
      double marLeftMargin, double marRightMargin, double marTopMargin, double marBottomMargin,
      double crtLeftMargin, double crtRightMargin, double crtTopMargin, double crtBottomMargin);

/******************************************************************************
* DFM::TestpointsPerNet
*/
SHORT DFM::TestpointsPerNet(SHORT geometryNumber, SHORT minTestpoints, LPCTSTR algName)
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

   BlockStruct *flattenGeometry = doc->getBlockAt(geometryNumber);

   if (!flattenGeometry)
      return RC_HOLE_IN_ARRAY;

   FileStruct *file = doc->Find_File(flattenGeometry->getFileNumber());

	int algIndex = GetAlgorithmNameIndex(doc, algName);

   CMap <int, int, int, int> testpointsPerNetMap; // map ValueArray Index to Num of Testpoints

   POSITION netPos = file->getHeadNetPosition();
   while (netPos)
   {
      NetStruct *net = file->getNextNet(netPos);

      testpointsPerNetMap.SetAt(doc->RegisterValue(net->getNetName()), 0);
   }

   WORD netnameKeyword = doc->IsKeyWord(ATT_NETNAME, 0);

   POSITION dataPos = flattenGeometry->getHeadDataPosition();
   while (dataPos)
   {
      DataStruct *data = flattenGeometry->getNextData(dataPos);

      if (!data->getAttributes())
         continue;

      Attrib* attrib;

      if (data->getAttributes()->Lookup(netnameKeyword, attrib))
      {
         int tespointsPerNet;

         if (testpointsPerNetMap.Lookup(attrib->getStringValueIndex(), tespointsPerNet))
            testpointsPerNetMap.SetAt(attrib->getStringValueIndex(), tespointsPerNet + 1);
      }
   }

   CString failureRangeString = FailureRanges[0];

   POSITION mapPos = testpointsPerNetMap.GetStartPosition();
   while (mapPos)
   {
      int valueIndex, testpointsInNet;
      testpointsPerNetMap.GetNextAssoc(mapPos, valueIndex, testpointsInNet);

      if (testpointsInNet < minTestpoints)
      {
         CString netname = doc->getCamCadData().getAttributeValueDictionary().getAt(valueIndex);
         CString drcString;
         drcString.Format("%d Testpoints in Net [%s] (%d required)", testpointsInNet, netname, minTestpoints);
         DRCStruct *drc = CreateDRC(file, drcString, DRC_CLASS_NETS, 0, algIndex, 0);
         DRC_FillNets(drc, netname);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, failureRangeString.GetBuffer(0), SA_OVERWRITE, NULL);
         double dblMinTestpoints = minTestpoints;
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &dblMinTestpoints, SA_OVERWRITE, NULL);
         double dblTestpoints = testpointsInNet;
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), VT_UNIT_DOUBLE, &dblTestpoints, SA_OVERWRITE, NULL);
      }
   }

   return RC_SUCCESS;
}

/******************************************************************************
* DFM::ComponentShadow
*/
SHORT DFM::ComponentShadow(LONG filePosition, DOUBLE marLeftMargin, DOUBLE marRightMargin, DOUBLE marTopMargin, DOUBLE marBottomMargin, DOUBLE crtLeftMargin, DOUBLE crtRightMargin, DOUBLE crtTopMargin, DOUBLE crtBottomMargin)
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

   DFM_CompShadow(doc, file, 
         marLeftMargin, marRightMargin, marTopMargin, marBottomMargin, 
         crtLeftMargin, crtRightMargin, crtTopMargin, crtBottomMargin); 

   return RC_SUCCESS;
}

struct CompShadowStruct
{
   long entity;
   CString refdes;
   double xmin, xmax, ymin, ymax;
   BOOL placeBottom;
   double x, y;
   CAttributes* attribMap;

   // accessors
   long getEntityNumber() const            { return entity; }
   void setEntityNumber(long entityNumber) { entity = entityNumber; }

   CAttributes*& getAttributesRef()          { return attribMap; }
};
typedef CTypedPtrList<CPtrList, CompShadowStruct*> CCompShadowList;

void DFM_CompShadow(CCEtoODBDoc *doc, FileStruct *file, 
      double marLeftMargin, double marRightMargin, double marTopMargin, double marBottomMargin,
      double crtLeftMargin, double crtRightMargin, double crtTopMargin, double crtBottomMargin)
{
   CString algName = "Component Shadow";
   int algIndex = GetAlgorithmNameIndex(doc, algName);
   long errorCount = 0;
   BOOL TooManyErrors = FALSE;

   CCompShadowList list;

   // find comps and build list of extents
   POSITION pos = file->getBlock()->getHeadDataPosition();
   while (pos)
   {
      DataStruct *data = file->getBlock()->getNextData(pos);

      if (data->getDataType() != T_INSERT || data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;

      CompShadowStruct *cs = new CompShadowStruct;
      list.AddTail(cs);
      cs->setEntityNumber(data->getEntityNumber());
      cs->refdes = data->getInsert()->getRefname();
      cs->placeBottom = data->getInsert()->getPlacedBottom();
      cs->x = data->getInsert()->getOrigin().x;
      cs->y = data->getInsert()->getOrigin().y;
      cs->attribMap = data->getAttributes();
      BlockStruct *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
      block_extents(doc, &cs->xmin, &cs->xmax, &cs->ymin, &cs->ymax, &subblock->getDataList(), 
            data->getInsert()->getOrigin().x, data->getInsert()->getOrigin().y, data->getInsert()->getAngle(),
				data->getInsert()->getMirrorFlags(), data->getInsert()->getScale(), data->getLayerIndex(), FALSE);
   }

   // compare boxes
   POSITION pos1 = list.GetHeadPosition();
   while (pos1 && !TooManyErrors)
   {
      CompShadowStruct *cs1 = list.GetNext(pos1);

      POSITION pos2 = list.GetHeadPosition();
      while (pos2 && !TooManyErrors)
      {
         CompShadowStruct *cs2 = list.GetNext(pos2);

         if (cs1 == cs2)
            continue;
         
         // same surface
         if (cs1->placeBottom ^ cs2->placeBottom)
            continue;

         int failureRange;
         double x, y;
         CString layerName;
         CString failureRangeString;
         double shadow_xmin, shadow_xmax, shadow_ymin, shadow_ymax;

         // MARGINAL RANGE
         shadow_xmin = cs1->xmin - marLeftMargin;
         shadow_xmax = cs1->xmax + marRightMargin;
         shadow_ymin = cs1->ymin - marBottomMargin;
         shadow_ymax = cs1->ymax + marTopMargin;

         // see if boxes touch
         if (!(shadow_xmax < cs2->xmin || shadow_xmin > cs2->xmax || shadow_ymax < cs2->ymin || shadow_ymin > cs2->ymax))
         {
            failureRange = 1;
            layerName = algName + " Marginal";
            failureRangeString = "Marginal";

            // find a good point for the marker
            double xmin, ymin, xmax, ymax;
            xmin = max(shadow_xmin, cs2->xmin);
            xmax = min(shadow_xmax, cs2->xmax);
            ymin = max(shadow_ymin, cs2->ymin);
            ymax = min(shadow_ymax, cs2->ymax);
            x = (xmin + xmax) / 2;
            y = (ymin + ymax) / 2;

            // CRITICAL RANGE
            shadow_xmin = cs1->xmin - crtLeftMargin;
            shadow_xmax = cs1->xmax + crtRightMargin;
            shadow_ymin = cs1->ymin - crtBottomMargin;
            shadow_ymax = cs1->ymax + crtTopMargin;

            // see if boxes touch
            if (!(shadow_xmax < cs2->xmin || shadow_xmin > cs2->xmax || shadow_ymax < cs2->ymin || shadow_ymin > cs2->ymax))
            {
               failureRange = 0;
               failureRangeString = "Critical";
               layerName = algName + " Critical";

               // find a good point for the marker
               double xmin, ymin, xmax, ymax;
               xmin = max(shadow_xmin, cs2->xmin);
               xmax = min(shadow_xmax, cs2->xmax);
               ymin = max(shadow_ymin, cs2->ymin);
               ymax = min(shadow_ymax, cs2->ymax);
               x = (xmin + xmax) / 2;
               y = (ymin + ymax) / 2;
            }

            CString comment;
            comment.Format("%s -> %s", cs1->refdes, cs2->refdes);
            DRCStruct *drc = AddDRC(doc, file, x, y, comment, DRC_CLASS_MEASURE, failureRange, algIndex, DFT_ALG_COMPONENT_SHADOW);
            DRC_FillMeasure(drc, DRC_ALG_GENERIC, cs1->getEntityNumber(), DRC_ALG_GENERIC, cs2->getEntityNumber(), (DbUnit)cs1->x, (DbUnit)cs1->y, (DbUnit)cs2->x, (DbUnit)cs2->y);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, algName.GetBuffer(0), SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, failureRangeString.GetBuffer(0), SA_OVERWRITE, NULL);

            errorCount++;
            if (maxErrorsPerTest && errorCount > maxErrorsPerTest)
               TooManyErrors = TRUE;
         }
      }
   }

   pos = list.GetHeadPosition();
   while (pos)
   {
      CompShadowStruct *cs = list.GetNext(pos);
      delete cs;
   }
}

/******************************************************************************
* DFM::ExposedTestpointPadSize
*/
SHORT DFM::ExposedTestpointPadSize(SHORT geometryNumber, DOUBLE minSize, DOUBLE maxSize, LPCTSTR algName)
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

   BlockStruct *geometry = doc->getBlockAt(geometryNumber);

   if (!geometry)
      return RC_HOLE_IN_ARRAY;

   FileStruct *file = doc->Find_File(geometry->getFileNumber());
   int algIndex = GetAlgorithmNameIndex(doc, algName);
   Mat2x2 m;
   RotMat2(&m, 0);

   int decimals = GetDecimals(doc->getSettings().getPageUnits());

   POSITION dataPos = geometry->getHeadDataPosition();
   while (dataPos)
   {
      DataStruct *data = geometry->getNextData(dataPos);

      ExtentRect extents;
      PolyExtents(doc, data->getPolyList(), &extents, 1, 0, 0, 0, &m, FALSE);

      double width = extents.right - extents.left;
      double height = extents.top - extents.bottom;

      double smallSize = min(width, height);
      double bigSize = max(width, height);

      double x = (extents.right + extents.left) / 2;
      double y = (extents.top + extents.bottom) / 2;

      if (smallSize < minSize)
      {
         CString drcString;
         drcString.Format("Testpoint Size %.*lf < %.*lf", decimals, smallSize, decimals, minSize);

         DRCStruct *drc = AddDRC(doc, file, x, y, drcString, DRC_CLASS_SIMPLE, 0, algIndex, 0);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, FailureRanges[0], SA_OVERWRITE, NULL);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &minSize, SA_OVERWRITE, NULL);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), VT_UNIT_DOUBLE, &smallSize, SA_OVERWRITE, NULL);
      }

      if (bigSize > maxSize)
      {
         CString drcString;
         drcString.Format("Testpoint Size %.*lf > %.*lf", decimals, bigSize, decimals, maxSize);

         DRCStruct *drc = AddDRC(doc, file, x, y, drcString, DRC_CLASS_SIMPLE, 0, algIndex, 0);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, FailureRanges[0], SA_OVERWRITE, NULL);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &maxSize, SA_OVERWRITE, NULL);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), VT_UNIT_DOUBLE, &bigSize, SA_OVERWRITE, NULL);
      }
   }

   return RC_SUCCESS;
}

/******************************************************************************
* DFM::RouteTraceWidthMinimum
*/
SHORT DFM::RouteTraceWidthMinimum(SHORT geometryNumber, DOUBLE marginal, DOUBLE critical, LPCTSTR excludeNets, LPCTSTR algName)
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

   FileStruct *file = doc->Find_File(geometry->getFileNumber());

   int algIndex = GetAlgorithmNameIndex(doc, algName);
   int decimals = GetDecimals(doc->getSettings().getPageUnits());

   char *netnames = STRDUP(excludeNets);
   CStringList netnameList;

   CString tok = strtok(netnames, ", ");
   while (!tok.IsEmpty())
   {
      netnameList.AddTail(tok);
      tok = strtok(NULL, ", ");
   }

   free(netnames);

   WORD netnameKeyword = doc->IsKeyWord(ATT_NETNAME, 0);

   POSITION dataPos = geometry->getHeadDataPosition();
   while (dataPos)
   {
      DataStruct *data = geometry->getNextData(dataPos);

      if (data->getGraphicClass() != GR_CLASS_ETCH)
         continue;

      Attrib* attrib;

      if (data->getAttributes() && data->getAttributes()->Lookup(netnameKeyword, attrib))
      {
         if (netnameList.Find(attrib->getStringValue()))
            continue;
      }

      POSITION polyPos = data->getPolyList()->GetHeadPosition();
      while (polyPos)
      {
         CPoly *poly = data->getPolyList()->GetNext(polyPos);

         if (poly->isClosed() || poly->isFilled() || poly->isVoid() || poly->isHatchLine() || poly->isHidden() || poly->isThermalLine())
            continue;

         if (poly->getWidthIndex() < 0)
            continue;

         double width = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA();

         if (width < marginal)
         {
            double checkValue = marginal;
            int failureRange = 1;

            if (width < critical)
            {
               checkValue = critical;
               failureRange = 0;
            }

            CPnt *pnt = poly->getPntList().GetHead();
            double x = pnt->x, y = pnt->y;

            if (pnt->bulge < BULGE_THRESHOLD && poly->getPntList().GetCount() > 1)
            {
               POSITION pntPos = poly->getPntList().GetHeadPosition();
               poly->getPntList().GetNext(pntPos);
               CPnt *next = poly->getPntList().GetAt(pntPos);

               x = (pnt->x + next->x) / 2;
               y = (pnt->y + next->y) / 2;
            }

            CString drcString;
            drcString.Format("%.*lf < %.*lf", decimals, width, decimals, checkValue);

            DRCStruct *drc = AddDRC(doc, file, x, y, drcString, DRC_CLASS_SIMPLE, failureRange, algIndex, 0);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, FailureRanges[failureRange], SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &checkValue, SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), VT_UNIT_DOUBLE, &width, SA_OVERWRITE, NULL);
         }
      }
   }

   return RC_SUCCESS;
}

/******************************************************************************
* DFM::GenerateEtchBlockers
*/
SHORT DFM::GenerateEtchBlockers(DOUBLE maxPinPitch, SHORT* topLayerNumber, SHORT* bottomLayerNumber)
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

   doc->StoreDocForImporting();

   LayerStruct *topLayer = doc->AddNewLayer("EtchBlockerTop");
   *topLayerNumber = topLayer->getLayerIndex();

   LayerStruct *bottomLayer = doc->AddNewLayer("EtchBlockerBottom");
   *bottomLayerNumber = bottomLayer->getLayerIndex();

   topLayer->setMirroredLayerIndex(bottomLayer->getLayerIndex());
   bottomLayer->setMirroredLayerIndex(topLayer->getLayerIndex());

   WORD smdKeyword = doc->IsKeyWord(ATT_SMDSHAPE, 0);
   Attrib* attrib;
   double accuracy = get_accuracy(doc);

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *compGeom = doc->getBlockAt(i);

      if (!compGeom)
         continue;

      if (compGeom->getBlockType() != BLOCKTYPE_PCBCOMPONENT)
         continue;

      if (!compGeom->getAttributesRef() || !compGeom->getAttributesRef()->Lookup(smdKeyword, attrib))
         continue;

      double pinPitch = get_pinpitch(compGeom, accuracy, doc);

      if (pinPitch > maxPinPitch)
         continue;

      SetCurrentDataList(&compGeom->getDataList());

      POSITION pinPos1 = compGeom->getHeadDataPosition();
      while (pinPos1)
      {
         DataStruct *pin1 = compGeom->getNextData(pinPos1);

         if (pin1->getDataType() != T_INSERT || pin1->getInsert()->getInsertType() != INSERTTYPE_PIN)
            continue;

         BlockStruct *pin1Geom = doc->getBlockAt(pin1->getInsert()->getBlockNumber());

         double xmin, xmax, ymin, ymax;
         block_extents(doc, &xmin, &xmax, &ymin, &ymax, &pin1Geom->getDataList(), 0, 0, 0, 0, 1, -1, FALSE);

         double offset1 = 0.6 * min(xmax-xmin, ymax-ymin); // slightly more than half

         POSITION pinPos2 = pinPos1;
         while (pinPos2)
         {
            DataStruct *pin2 = compGeom->getNextData(pinPos2);

            if (pin2->getDataType() != T_INSERT || pin2->getInsert()->getInsertType() != INSERTTYPE_PIN)
               continue;

            double distance = Length(pin1->getInsert()->getOrigin().x, pin1->getInsert()->getOrigin().y,
					pin2->getInsert()->getOrigin().x, pin2->getInsert()->getOrigin().y);

            if (distance > pinPitch + SMALLNUMBER)
               continue;

            BlockStruct *pin2Geom = doc->getBlockAt(pin2->getInsert()->getBlockNumber());

            double xmin, xmax, ymin, ymax;
            block_extents(doc, &xmin, &xmax, &ymin, &ymax, &pin2Geom->getDataList(), 0, 0, 0, 0, 1, -1, FALSE);
            double offset2 = 0.6 * min(xmax-xmin, ymax-ymin); // slightly more than half

            double x1, y1, x2, y2; 
            FindPointOnLine(pin1->getInsert()->getOrigin().x, pin1->getInsert()->getOrigin().y, pin2->getInsert()->getOrigin().x, pin2->getInsert()->getOrigin().y, offset1, &x1, &y1);
            FindPointOnLine(pin2->getInsert()->getOrigin().x, pin2->getInsert()->getOrigin().y, pin1->getInsert()->getOrigin().x, pin1->getInsert()->getOrigin().y, offset2, &x2, &y2);

            Graph_Line(topLayer->getLayerIndex(), x1, y1, x2, y2, 0, 0, FALSE);
         }
      }
   }

   return RC_SUCCESS;
}

bool DFM_GerberThermalBarrelPlugCheck(CCEtoODBDoc& camCadDoc,FileStruct& ecadFile);

/******************************************************************************
* DFM::GerberThermalBarrelPlugCheck
*/
SHORT DFM::GerberThermalBarrelPlugCheck(LONG filePosition)
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

   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);

   if (!DFM_GerberThermalBarrelPlugCheck(*doc, *file))
      return RC_GENERAL_ERROR;

   return RC_SUCCESS;
}

bool DFM_Pin1OrientationCheck(CCEtoODBDoc& camCadDoc,BlockStruct& block, int surfaceMask,int componentTypeMask,int occuranceFence);

/******************************************************************************
* Pin1Orientation
*/
SHORT DFM::Pin1Orientation(LONG filePosition, SHORT top, SHORT bottom, SHORT thruHole, SHORT smd, LONG ignoreCount)
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

   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);

   int surface = 0;
   if (top)
      surface |= topSurface;
   if (bottom)
      surface |= bottomSurface;

   int compType = 0;
   if (thruHole)
      compType |= thruHoleMountComponent;
   if (smd)
      compType |= surfaceMountComponent;

   if (!DFM_Pin1OrientationCheck(*doc, *(file->getBlock()), surface, compType, ignoreCount))
      return RC_GENERAL_ERROR;

   return RC_SUCCESS;
}

bool DFM_TestPadDensityCheck(CCEtoODBDoc& camCadDoc, BlockStruct& pcbBlock, double gridSize, int criticalDensity, int marginalDensity, bool addTestPointDensityAttributeFlag);

/******************************************************************************
* TestPadDensityCheck
*/
SHORT DFM::TestPadDensityCheck(LONG filePosition, SHORT marginal, SHORT critical, DOUBLE gridSize, SHORT addTPDensityAttrib)
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

   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);

   if (!DFM_TestPadDensityCheck(*doc, *(file->getBlock()), gridSize, critical, marginal, addTPDensityAttrib != 0))
      return RC_GENERAL_ERROR;

   return RC_SUCCESS;
}

bool DFM_ComponentSurfaceCheck(CCEtoODBDoc& camCadDoc,BlockStruct& pcbBlock, int surfaceMask,const CString& attributeKeyword,const CString& attributeValue);

/******************************************************************************
* ComponentNotAllowedPerSurface
*/
SHORT DFM::ComponentNotAllowedPerSurface(LONG filePosition, SHORT top, SHORT bottom, LPCTSTR attribKeyword, LPCTSTR attribValue)
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

   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);

   int surface = 0;
   if (top)
      surface |= topSurface;
   if (bottom)
      surface |= bottomSurface;

   if (!DFM_ComponentSurfaceCheck(*doc, *(file->getBlock()), surface, attribKeyword, attribValue))
      return RC_GENERAL_ERROR;

   return RC_SUCCESS;
}

bool DFM_ComponentHeightCheck(CCEtoODBDoc& camCadDoc,BlockStruct& pcbBlock, 
      int surfaceMask,const CString& componentHeightAttributeKeyword, 
      double topCriticalHeight ,double topMarginalHeight, 
      double bottomCriticalHeight,double bottomMarginalHeight);

/******************************************************************************
* ComponentHeight
*/
SHORT DFM::ComponentHeight(LONG filePosition, SHORT top, SHORT bottom, LPCTSTR componentHeightAttributeKeyword, DOUBLE topMarginalHeight, DOUBLE topCriticalHeight, DOUBLE bottomMarginalHeight, DOUBLE bottomCriticalHeight)
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

   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);

   int surface = 0;
   if (top)
      surface |= topSurface;
   if (bottom)
      surface |= bottomSurface;

   if (!DFM_ComponentHeightCheck(*doc, *(file->getBlock()), surface, componentHeightAttributeKeyword, 
                  topCriticalHeight, topMarginalHeight, bottomCriticalHeight, bottomMarginalHeight))
      return RC_GENERAL_ERROR;

   return RC_SUCCESS;
}
