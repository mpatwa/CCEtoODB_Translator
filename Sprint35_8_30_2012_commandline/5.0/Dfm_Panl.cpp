// $Header: /CAMCAD/5.0/Dfm_Panl.cpp 52    6/17/07 8:51p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "DFM.h"
#include "crypt.h"
#include "API.h"
#include "ccview.h"
#include "drc.h"
#include "float.h"
#include "extents.h"
#include "xform.h"
#include "CCEtoODB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CCEtoODBView *apiView; // from API.CPP
extern char *FailureRanges[]; // from DRC.CPP

static CString failureRangeString = FailureRanges[0];

/******************************************************************************
* GetOutlineExtents
*/
short GetOutlineExtents(CCEtoODBDoc *doc, FileStruct *file, int algIndex, const char *algName, ExtentRect *fileExtents)
{
   Mat2x2 m;
   RotMat2(&m, 0);

   fileExtents->left = fileExtents->bottom = FLT_MAX;
   fileExtents->right = fileExtents->top = -FLT_MAX;

   BOOL OutlineFound = FALSE;

   POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_POLY)
         continue;

      if (file->getBlockType() == BLOCKTYPE_PANEL && data->getGraphicClass() != GR_CLASS_PANELOUTLINE)
         continue;

      if (file->getBlockType() == BLOCKTYPE_PCB && data->getGraphicClass() != GR_CLASS_BOARDOUTLINE)
         continue;

      OutlineFound = TRUE;

      ExtentRect polyExtents;
      PolyExtents(doc, data->getPolyList(), &polyExtents, 1, 0, 0, 0, &m, FALSE);

      if (polyExtents.left < fileExtents->left)
         fileExtents->left = polyExtents.left;
      if (polyExtents.right > fileExtents->right)
         fileExtents->right = polyExtents.right;
      if (polyExtents.bottom < fileExtents->bottom)
         fileExtents->bottom = polyExtents.bottom;
      if (polyExtents.top > fileExtents->top)
         fileExtents->top = polyExtents.top;
   }

   if (!OutlineFound)
   {
      CString drcString;
      drcString = "TEST FAILED - No Outline Found";
      DRCStruct *drc = CreateDRC(file, drcString, DRC_CLASS_NO_MARKER, 0, algIndex, DRC_ALG_GENERIC);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, failureRangeString.GetBuffer(0), SA_OVERWRITE, NULL);

      return RC_DFM_TEST_FAILED;
   }

   return RC_SUCCESS;
}

/******************************************************************************
* DFM::PanelDimensions
*/
SHORT DFM::PanelDimensions(LONG filePosition, DOUBLE minWidth, DOUBLE maxWidth, DOUBLE minLength, DOUBLE maxLength, LPCTSTR algName)
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
   FileStruct *panelFile = doc->getFileList().GetAt(pos);

   int algIndex = GetAlgorithmNameIndex(doc, algName);

   ExtentRect panelExtents;

   short res = GetOutlineExtents(doc, panelFile, algIndex, algName, &panelExtents);
   if (res != RC_SUCCESS)
      return res;

   double panelLength = panelExtents.right - panelExtents.left;
   double panelWidth = panelExtents.top - panelExtents.bottom;

   int direction = GetDirectionOfTravel(doc, panelFile->getBlock());
   if (direction == 3 || direction == 4)
   {
      double temp = panelWidth;
      panelWidth = panelLength;
      panelLength = temp;
   }

   int decimals = GetDecimals(doc->getSettings().getPageUnits());

   if (panelWidth < minWidth)
   {
      CString drcString;
      drcString.Format("Panel Width %.*lf < %.*lf", decimals, panelWidth, decimals, minWidth);
      DRCStruct *drc = CreateDRC(panelFile, drcString, DRC_CLASS_NO_MARKER, 0, algIndex, 0);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, failureRangeString.GetBuffer(0), SA_OVERWRITE, NULL);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &minWidth, SA_OVERWRITE, NULL);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), VT_UNIT_DOUBLE, &panelWidth, SA_OVERWRITE, NULL);
   }

   if (panelWidth > maxWidth)
   {
      CString drcString;
      drcString.Format("Panel Width %.*lf > %.*lf", decimals, panelWidth, decimals, maxWidth);
      DRCStruct *drc = CreateDRC(panelFile, drcString, DRC_CLASS_NO_MARKER, 0, algIndex, 0);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, failureRangeString.GetBuffer(0), SA_OVERWRITE, NULL);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &maxWidth, SA_OVERWRITE, NULL);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), VT_UNIT_DOUBLE, &panelWidth, SA_OVERWRITE, NULL);
   }

   if (panelLength < minLength)
   {
      CString drcString;
      drcString.Format("Panel Length %.*lf < %.*lf", decimals, panelLength, decimals, minLength);
      DRCStruct *drc = CreateDRC(panelFile, drcString, DRC_CLASS_NO_MARKER, 0, algIndex, 0);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, failureRangeString.GetBuffer(0), SA_OVERWRITE, NULL);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &minLength, SA_OVERWRITE, NULL);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), VT_UNIT_DOUBLE, &panelLength, SA_OVERWRITE, NULL);
   }

   if (panelLength > maxLength)
   {
      CString drcString;
      drcString.Format("Panel Length %.*lf > %.*lf", decimals, panelLength, decimals, maxLength);
      DRCStruct *drc = CreateDRC(panelFile, drcString, DRC_CLASS_NO_MARKER, 0, algIndex, 0);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, failureRangeString.GetBuffer(0), SA_OVERWRITE, NULL);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &maxLength, SA_OVERWRITE, NULL);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), VT_UNIT_DOUBLE, &panelLength, SA_OVERWRITE, NULL);
   }

   return RC_SUCCESS;
}

/******************************************************************************
* DFM::PanelLengthToWidthRatio
*/
SHORT DFM::PanelLengthToWidthRatio(LONG filePosition, DOUBLE minRatio, LPCTSTR algName)
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
   FileStruct *panelFile = doc->getFileList().GetAt(pos);

   int algIndex = GetAlgorithmNameIndex(doc, algName);

   ExtentRect panelExtents;

   short res = GetOutlineExtents(doc, panelFile, algIndex, algName, &panelExtents);
   if (res != RC_SUCCESS)
      return res;


   double panelLength = panelExtents.right - panelExtents.left;
   double panelWidth = panelExtents.top - panelExtents.bottom;

   int direction = GetDirectionOfTravel(doc, panelFile->getBlock());
   if (direction == 3 || direction == 4)
   {
      double temp = panelWidth;
      panelWidth = panelLength;
      panelLength = temp;
   }

   double ratio = panelLength / panelWidth;

   int decimals = GetDecimals(doc->getSettings().getPageUnits());

   if (ratio < minRatio)
   {
      CString drcString;
      drcString.Format("Panel Length %.*lf / Panel Width %.*lf = %.2lf < %.2lf", 
            decimals, panelLength, decimals, panelWidth, ratio, minRatio);
      DRCStruct *drc = CreateDRC(panelFile, drcString, DRC_CLASS_NO_MARKER, 0, algIndex, 0);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, failureRangeString.GetBuffer(0), SA_OVERWRITE, NULL);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &minRatio, SA_OVERWRITE, NULL);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), VT_UNIT_DOUBLE, &ratio, SA_OVERWRITE, NULL);
   }

   return RC_SUCCESS;
}

/******************************************************************************
* DFM::ItemsInCorners
*/
SHORT DFM::ItemsInCorners(SHORT geometryNumber, DOUBLE minRail, DOUBLE maxRail, DOUBLE minNonRail, DOUBLE maxNonRail, SHORT minCorners, LPCTSTR algName, SHORT surface)
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

   ExtentRect outlineExtents;

   short res = GetOutlineExtents(doc, file, algIndex, algName, &outlineExtents);
   if (res != RC_SUCCESS)
      return res;

   double minTopBot = minRail;
   double maxTopBot = maxRail;
   double minLeftRight = minNonRail;
   double maxLeftRight = maxNonRail;

   int direction = GetDirectionOfTravel(doc, file->getBlock());
   if (direction == 3 || direction == 4)
   {
      minTopBot = minNonRail;
      maxTopBot = maxNonRail;
      minLeftRight = minRail;
      maxLeftRight = maxRail;
   }

   ExtentRect tlExtents;
   tlExtents.left = outlineExtents.left + minLeftRight;
   tlExtents.right = outlineExtents.left + maxLeftRight;
   tlExtents.top = outlineExtents.top - minTopBot;
   tlExtents.bottom = outlineExtents.top - maxTopBot;

   ExtentRect trExtents;
   trExtents.left = outlineExtents.right - maxLeftRight;
   trExtents.right = outlineExtents.right - minLeftRight;
   trExtents.top = outlineExtents.top - minTopBot;
   trExtents.bottom = outlineExtents.top - maxTopBot;

	ExtentRect blExtents;
   blExtents.left = outlineExtents.left + minLeftRight;
   blExtents.right = outlineExtents.left + maxLeftRight;
   blExtents.top = outlineExtents.bottom + maxTopBot;
   blExtents.bottom = outlineExtents.bottom + minTopBot;

	ExtentRect brExtents;
   brExtents.left = outlineExtents.right - maxLeftRight;
   brExtents.right = outlineExtents.right - minLeftRight;
   brExtents.top = outlineExtents.bottom + maxTopBot;
   brExtents.bottom = outlineExtents.bottom + minTopBot;

   BOOL tl = FALSE, tr = FALSE, bl = FALSE, br = FALSE;

   POSITION dataPos = flattenGeometry->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = flattenGeometry->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_POLY)
         continue;

      CPnt *insertPnt = ((CPoly*)data->getPolyList()->GetHead())->getPntList().GetHead();

      if (!tl && insertPnt->x > tlExtents.left - SMALLNUMBER && insertPnt->x < tlExtents.right + SMALLNUMBER && insertPnt->y > tlExtents.bottom - SMALLNUMBER && insertPnt->y < tlExtents.top + SMALLNUMBER)
         tl = TRUE;

      if (!tr && insertPnt->x > trExtents.left - SMALLNUMBER && insertPnt->x < trExtents.right + SMALLNUMBER && insertPnt->y > trExtents.bottom - SMALLNUMBER && insertPnt->y < trExtents.top + SMALLNUMBER)
         tr = TRUE;

      if (!bl && insertPnt->x > blExtents.left - SMALLNUMBER && insertPnt->x < blExtents.right + SMALLNUMBER && insertPnt->y > blExtents.bottom - SMALLNUMBER && insertPnt->y < blExtents.top + SMALLNUMBER)
         bl = TRUE;

      if (!br && insertPnt->x > brExtents.left - SMALLNUMBER && insertPnt->x < brExtents.right + SMALLNUMBER && insertPnt->y > brExtents.bottom - SMALLNUMBER && insertPnt->y < brExtents.top + SMALLNUMBER)
         br = TRUE;
   }

   int corners = tl + tr + bl + br;

   if (corners < minCorners)
   {
      CString surfaceString;
      switch (surface)
      {
      case 0:
         surfaceString = "";
         break;
      case 1:
         surfaceString = "on Top ";
         break;
      case 2:
         surfaceString = "on Bottom ";
         break;
      }
      CString drcString;
      drcString.Format("Items in %d Corners %s(%d Corners required)", corners, surfaceString, minCorners);
      DRCStruct *drc = CreateDRC(file, drcString, DRC_CLASS_NO_MARKER, 0, algIndex, 0);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, failureRangeString.GetBuffer(0), SA_OVERWRITE, NULL);
      double dblMinCorners = minCorners;
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &dblMinCorners, SA_OVERWRITE, NULL);
      double dblCorners = corners;
      doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), VT_UNIT_DOUBLE, &dblCorners, SA_OVERWRITE, NULL);
   }

   return RC_SUCCESS;
}

/******************************************************************************
* DFM::ClearanceToPanelEdge
*/
SHORT DFM::ClearanceToPanelEdge(SHORT geometryNumber, DOUBLE minRail, DOUBLE minNonRail, LPCTSTR algName)
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

   BlockStruct *flattenGeometry = doc->getBlockAt(geometryNumber);

   if (!flattenGeometry)
      return RC_HOLE_IN_ARRAY;

   FileStruct *panelFile = doc->Find_File(flattenGeometry->getFileNumber());

   CWaitCursor wait;

   int algIndex = GetAlgorithmNameIndex(doc, algName);
   WORD refnameKeyword = doc->IsKeyWord(ATT_REFNAME, 0);

   ExtentRect panelExtents;

   short res = GetOutlineExtents(doc, panelFile, algIndex, algName, &panelExtents);
   if (res != RC_SUCCESS)
      return res;

   double minTopBot = minRail;
   double minLeftRight = minNonRail;

   int direction = GetDirectionOfTravel(doc, panelFile->getBlock());
   if (direction == 3 || direction == 4)
   {
      minTopBot = minNonRail;
      minLeftRight = minRail;
   }

   Mat2x2 m;
   RotMat2(&m, 0);
   POSITION dataPos = flattenGeometry->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = flattenGeometry->getDataList().GetNext(dataPos);

      ExtentRect polyExtents;
      PolyExtents(doc, data->getPolyList(), &polyExtents, 1, 0, 0, 0, &m, FALSE);

      double testDist, actualDist;
      BOOL Failure = FALSE;
      double panelX, panelY, polyX, polyY;

      if (polyExtents.left < panelExtents.left + minLeftRight)
      {
         testDist = minLeftRight;
         actualDist = polyExtents.left - panelExtents.left;
         Failure = TRUE;
         panelX = panelExtents.left;
         polyX = polyExtents.left;
         panelY = polyY = (polyExtents.top + polyExtents.bottom) / 2;
      }
      else if (polyExtents.right > panelExtents.right - minLeftRight)
      {
         testDist = minLeftRight;
         actualDist = -polyExtents.right + panelExtents.right;
         Failure = TRUE;
         panelX = panelExtents.right;
         polyX = polyExtents.right;
         panelY = polyY = (polyExtents.top + polyExtents.bottom) / 2;
      }
      else if (polyExtents.top > panelExtents.top - minTopBot)
      {
         testDist = minTopBot;
         actualDist = -polyExtents.top + panelExtents.top;
         Failure = TRUE;
         panelY = panelExtents.top;
         polyY = polyExtents.top;
         panelX = polyX = (polyExtents.left + polyExtents.right) / 2;
      }
      else if (polyExtents.bottom < panelExtents.bottom + minTopBot)
      {
         testDist = minTopBot;
         actualDist = polyExtents.bottom - panelExtents.bottom;
         Failure = TRUE;
         panelY = panelExtents.bottom;
         polyY = polyExtents.bottom;
         panelX = polyX = (polyExtents.left + polyExtents.right) / 2;
      }

      if (Failure)
      {
         CString refDes = "Unknown";
         Attrib* attrib;

         if (data->getAttributesRef() && data->getAttributesRef()->Lookup(refnameKeyword, attrib))
            refDes = attrib->getStringValue();

         CString drcString;
         drcString.Format("%s in keepout", refDes);

         int algIndex = GetAlgorithmNameIndex(doc, algName);

         DRCStruct *drc = AddDRC(doc, panelFile, (polyX + panelX) / 2, (polyY + panelY) / 2, drcString, DRC_CLASS_MEASURE, 0, algIndex, 0);
         DRC_FillMeasure(drc, DRC_ALG_GENERIC, data->getEntityNumber(), DRC_ALG_GENERIC, -1, (DbUnit)polyX, (DbUnit)polyY, (DbUnit)panelX, (DbUnit)panelY);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, failureRangeString.GetBuffer(0), SA_OVERWRITE, NULL);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &testDist, SA_OVERWRITE, NULL);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), VT_UNIT_DOUBLE, &actualDist, SA_OVERWRITE, NULL);
      }
   }

   return RC_SUCCESS;
}

/******************************************************************************
* DFM::ComponentFlushWithOutline
*/
SHORT DFM::ComponentFlushWithOutline(SHORT geometryNumber, DOUBLE maxDist, LPCTSTR algName)
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

   BlockStruct *flattenGeometry = doc->getBlockAt(geometryNumber);

   if (!flattenGeometry)
      return RC_HOLE_IN_ARRAY;

   FileStruct *file = doc->Find_File(flattenGeometry->getFileNumber());

   CWaitCursor wait;

   int algIndex = GetAlgorithmNameIndex(doc, algName);
   WORD refnameKeyword = doc->IsKeyWord(ATT_REFNAME, 0);

   ExtentRect panelExtents;

   short res = GetOutlineExtents(doc, file, algIndex, algName, &panelExtents);
   if (res != RC_SUCCESS)
      return res;

   Mat2x2 m;
   RotMat2(&m, 0);
   POSITION dataPos = flattenGeometry->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = flattenGeometry->getDataList().GetNext(dataPos);

      ExtentRect polyExtents;
      PolyExtents(doc, data->getPolyList(), &polyExtents, 1, 0, 0, 0, &m, FALSE);

      if (polyExtents.left > panelExtents.left + maxDist && polyExtents.right < panelExtents.right - maxDist && 
            polyExtents.top < panelExtents.top - maxDist && polyExtents.bottom > panelExtents.bottom + maxDist)
      {
         CString refDes = "Unknown";
         Attrib* attrib;

         if (data->getAttributesRef() && data->getAttributesRef()->Lookup(refnameKeyword, attrib))
            refDes = attrib->getStringValue();

         CString drcString;
         drcString.Format("%s not flush with an edge", refDes);

         int algIndex = GetAlgorithmNameIndex(doc, algName);

         double x = (polyExtents.left + polyExtents.right) / 2;
         double y = (polyExtents.bottom + polyExtents.top) / 2;

         DRCStruct *drc = AddDRC(doc, file, x, y, drcString, DRC_CLASS_SIMPLE, 0, algIndex, 0);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, failureRangeString.GetBuffer(0), SA_OVERWRITE, NULL);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &maxDist, SA_OVERWRITE, NULL);
      }
   }

   return RC_SUCCESS;
}
