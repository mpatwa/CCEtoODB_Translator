// $Header: /CAMCAD/5.0/Dfm_Dist.cpp 63    6/17/07 8:51p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "dfm_dist.h"
#include "measure.h"
#include "drc.h"
#include "data.h"
#include "attrib.h"
#include "find.h"
#include "extents.h"
#include "outline.h" 
#include "bb_lib.h"
#include "polylib.h"
#include "union.h"
#include "dfm.h"
#include "crypt.h"
#include "api.h"
#include "net_util.h"
#include "CCEtoODB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Building Block includes
// needed for BuildingBlock software
extern "C"
{
#include <dmldefs.h>
#include <t2cdefs.h>
#include <c2cdefs.h>
}

extern CCEtoODBView *apiView; // from API.CPP
extern int maxErrorsPerTest; // from DFM.CPP

static BOOL DFM_DistanceCheck(CCEtoODBDoc *doc, CDataList *datalist1, CDataList *datalist2, FileStruct *file, 
      double marginalDist, double criticalDist, 
      BOOL SkipSameNets, BOOL OneDRCPerNetPair, BOOL SkipSameComps, CString netname1, CString netname2, 
      CString algName, int algType, BOOL CanTouch);

static void DFM_PolyExtents(CCEtoODBDoc *doc, CPolyList *polylist, ExtentRect *extents, double scale, int mirror,
      double insert_x, double insert_y, Mat2x2 *m, int use_width);

/******************************************************************************
* DFM::DistanceCheck
*/
SHORT DFM::DistanceCheck(SHORT geometryNumber1, SHORT geometryNumber2, DOUBLE marginalDistance, DOUBLE criticalDistance,
      LPCTSTR algName, SHORT algType, SHORT CanTouch, SHORT SkipSameNets, SHORT OneDRCPerNetPair)
{
   return DistanceCheck2(geometryNumber1, geometryNumber2, marginalDistance, criticalDistance,
      algName, algType, CanTouch, SkipSameNets, OneDRCPerNetPair, FALSE);
}

/******************************************************************************
* DFM::DistanceCheck2
*/
SHORT DFM::DistanceCheck2(SHORT geometryNumber1, SHORT geometryNumber2, DOUBLE marginalDistance, DOUBLE criticalDistance,
      LPCTSTR algName, SHORT algType, SHORT CanTouch, SHORT SkipSameNets, SHORT OneDRCPerNetPair, SHORT SkipSameComponents)
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

   if (geometryNumber1 < 0 || geometryNumber1 >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *block1 = doc->getBlockAt(geometryNumber1);

   if (!block1)
      return RC_HOLE_IN_ARRAY;

   if (geometryNumber2 < 0 || geometryNumber2 >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *block2 = doc->getBlockAt(geometryNumber2);

   if (!block2)
      return RC_HOLE_IN_ARRAY;

   FileStruct *file = doc->Find_File(block1->getFileNumber());

   CWaitCursor wait;

   BOOL TooManyErrors = DFM_DistanceCheck(doc, &block1->getDataList(), &block2->getDataList(), file, marginalDistance, criticalDistance, 
      SkipSameNets, OneDRCPerNetPair, SkipSameComponents, "", "", 
      algName, algType, CanTouch);

   if (TooManyErrors)
   {
      int algIndex = GetAlgorithmNameIndex(doc, algName);
      DRCStruct *drc = CreateDRC(file, "Max Error Count Hit", DRC_CLASS_NO_MARKER, 1, algIndex, 0);
      return RC_TOO_MANY_ERRORS;
   }

   return RC_SUCCESS;
}

/******************************************************************************
* DFM::DistanceCheckDiffNets
*/
SHORT DFM::DistanceCheckDiffNets(SHORT geometryNumber, DOUBLE marginalDistance, DOUBLE criticalDistance, 
      LPCTSTR algName, SHORT algType, SHORT CanTouch)
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

   BlockStruct *block = doc->getBlockAt(geometryNumber);

   if (!block)
      return RC_HOLE_IN_ARRAY;

   FileStruct *file = doc->Find_File(block->getFileNumber());

   CWaitCursor wait;

   BOOL TooManyErrors = DFM_DistanceCheck(doc, &block->getDataList(), &block->getDataList(), file, marginalDistance, criticalDistance, 
      TRUE, TRUE, FALSE, "", "", 
      algName, algType, CanTouch);

   if (TooManyErrors)
   {
      int algIndex = GetAlgorithmNameIndex(doc, algName);
      DRCStruct *drc = CreateDRC(file, "Max Error Count Hit", DRC_CLASS_NO_MARKER, 1, algIndex, 0);
      return RC_TOO_MANY_ERRORS;
   }

   return RC_SUCCESS;
}

/******************************************************************************
* DFM::DistanceCheckTwoNets
*/
SHORT DFM::DistanceCheckTwoNets(SHORT geometryNumber, LPCTSTR netname1, LPCTSTR netname2, 
      DOUBLE marginalDistance, DOUBLE criticalDistance, LPCTSTR algName, SHORT algType, SHORT CanTouch)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDfm)) // && !get_license(LIC_VISION_DFM)) 
   {
      ErrorAccess("You do not have a License for DFM!");
      return RC_NO_LICENSE;
   }*/

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (geometryNumber < 0 || geometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *block = doc->getBlockAt(geometryNumber);

   if (!block)
      return RC_HOLE_IN_ARRAY;

   FileStruct *file = doc->Find_File(block->getFileNumber());

   CWaitCursor wait;

   BOOL TooManyErrors = DFM_DistanceCheck(doc, &block->getDataList(), &block->getDataList(), file, marginalDistance, criticalDistance, 
      TRUE, TRUE, FALSE, netname1, netname2, 
      algName, algType, CanTouch);

   if (TooManyErrors)
   {
      int algIndex = GetAlgorithmNameIndex(doc, algName);
      DRCStruct *drc = CreateDRC(file, "Max Error Count Hit", DRC_CLASS_NO_MARKER, 1, algIndex, 0);
      return RC_TOO_MANY_ERRORS;
   }

   return RC_SUCCESS;
}

/******************************************************************************
* DFM_DistanceCheck
*/
BOOL DFM_DistanceCheck(CCEtoODBDoc *doc, CDataList *datalist1, CDataList *datalist2, FileStruct *file, 
      double marginalDist, double criticalDist, 
      BOOL SkipSameNets, BOOL OneDRCPerNetPair, BOOL SkipSameComps, CString netname1, CString netname2, 
      CString algName, int algType, BOOL CanTouch)
{
   DTransform xform;
   Point2 result1, result2;
   Mat2x2 m;
   RotMat2(&m, 0);
   int algIndex = GetAlgorithmNameIndex(doc, algName);
   CDiffNetsList diffNetsList;
   long errorCount = 0;
   BOOL TooManyErrors = FALSE;


   // extents rect map
   CExtentsMap extentsMap1;
   POSITION pos = datalist1->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = datalist1->GetNext(pos);		
		if (data == NULL || data->getDataType() != T_POLY)
         continue;

      ExtentRect extents;
      DFM_PolyExtents(doc, data->getPolyList(), &extents, 1, 0, 0, 0, &m, TRUE);

      extentsMap1.SetAt(data, extents);
   }

   CExtentsMap extentsMap2;
   pos = datalist2->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = datalist2->GetNext(pos);
		if (data == NULL || data->getDataType() != T_POLY)
         continue;

      ExtentRect extents;
      DFM_PolyExtents(doc, data->getPolyList(), &extents, 1, 0, 0, 0, &m, TRUE);

      extentsMap2.SetAt(data, extents);
   }


   double actualMarginalDist = marginalDist;
   double actualCriticalDist = criticalDist;

   if (actualCriticalDist > actualMarginalDist)
      actualMarginalDist = actualCriticalDist;

   WORD netnameKeyword = doc->IsKeyWord(ATT_NETNAME, 0);
   WORD refnameKeyword = doc->IsKeyWord(ATT_REFNAME, 0);
   WORD compRefnameKeyword = doc->IsKeyWord(ATT_DFMCOMP_REFNAME, 0);
   Attrib* attrib;
   int net1, net2;
   int comp1, comp2;
   CString compName1, compName2;

	CViolationsMapList violations;
   CMapStringToString violationsMap;

   POSITION pos1 = datalist1->GetHeadPosition();
   while (pos1 && !TooManyErrors)
   {
      DataStruct* data1 = datalist1->GetNext(pos1);

      if (data1 == NULL || data1->getDataType() != dataTypePoly)
         continue;

      // SkipSameNets?
      if (SkipSameNets && data1->getAttributes() && data1->getAttributes()->Lookup(netnameKeyword, attrib))
      {        
         net1 = attrib->getStringValueIndex();

         CString netname = doc->getCamCadData().getAttributeValueDictionary().getAt(net1);

         if (!netname1.IsEmpty() && !netname2.IsEmpty() && netname != netname1 && netname != netname2)
            continue;
      }

      // SkipSameComps?
      if (SkipSameComps && data1->getAttributes() && data1->getAttributes()->Lookup(compRefnameKeyword, attrib))
      {        
         comp1 = attrib->getStringValueIndex();

         CString compname = doc->getCamCadData().getAttributeValueDictionary().getAt(comp1);

         compName1 = compname;
      }

      ExtentRect extents1;

      if (!extentsMap1.Lookup(data1, extents1))
         continue;

      POSITION pos2 = datalist2->GetHeadPosition();

      if (datalist2 == datalist1) // do not start at head
         pos2 = pos1;

      while (pos2 && !TooManyErrors)
      {
         DataStruct *data2 = datalist2->GetNext(pos2);

         if (data2 == NULL || data2->getDataType() != dataTypePoly)
            continue;

         // MUST check for same entity number because if a padstack with more than one electric pads
			// in the stack will end up having several flatten poly with the same entity number of the insert.
			// This fixed case #1641 and other cases
			// the two lines of code below were commented out to fix case 1824
		   //if (data2->getEntityNumber() == data1->getEntityNumber())
		 		//continue;


         ExtentRect extents2;
         if (!extentsMap2.Lookup(data2, extents2))      
            continue;

         // Box Check
         if (extents1.left > extents2.right + actualMarginalDist || extents2.left > extents1.right + actualMarginalDist || 
               extents1.bottom > extents2.top + actualMarginalDist || extents2.bottom > extents1.top + actualMarginalDist)
            continue;

         // SkipSameNets?
         if (SkipSameNets && data2->getAttributes() && data2->getAttributes()->Lookup(netnameKeyword, attrib))
         {
            net2 = attrib->getStringValueIndex();

            if (net1 == net2)
               continue;

            CString netname = doc->getCamCadData().getAttributeValueDictionary().getAt(net2);

            if (!netname1.IsEmpty() && !netname2.IsEmpty() && netname != netname1 && netname != netname2)
               continue;
         }

         // SkipSameComps?
         if (SkipSameComps && data2->getAttributes() && data2->getAttributes()->Lookup(compRefnameKeyword, attrib))
         {
            //if (!data2->getAttributesRef())
            // continue;
            //if (!data2->getAttributesRef()->Lookup(compRefnameKeyword, voidPtr))
            // continue;
            comp2 = attrib->getStringValueIndex();

            if (comp1 == comp2)
               continue;

            CString compname = doc->getCamCadData().getAttributeValueDictionary().getAt(comp2);

            if (!compName1.IsEmpty() && compname == compName1)
               continue;

            compName2 = compname;
         }



			// make sure we don't create a violation with an etch and a component that the etch is connected to
			// make sure we have an etch and a component and get the netname attribute and refname attribute, respectively
			DataStruct *etchData = NULL, *compData = NULL;
			Attrib *etchAttrib = NULL, *compAttrib = NULL;

			if ((data1->getDataType() == dataTypePoly && data1->getGraphicClass() == GR_CLASS_ETCH && data1->getAttributes() && data1->getAttributes()->Lookup(netnameKeyword, etchAttrib)) || 
				(data2->getDataType() == dataTypePoly && data2->getGraphicClass() == GR_CLASS_ETCH && data2->getAttributes() && data2->getAttributes()->Lookup(netnameKeyword, etchAttrib)))
				etchData = (data1->getAttributes() && data1->getAttributes()->Lookup(netnameKeyword, etchAttrib))?data1:data2;

			if ((data1->getAttributes() && data1->getAttributes()->Lookup(compRefnameKeyword, compAttrib)) || (data2->getAttributes() && data2->getAttributes()->Lookup(compRefnameKeyword, compAttrib)))
				compData = (data1->getAttributes() && data1->getAttributes()->Lookup(compRefnameKeyword, compAttrib))?data1:data2;

			// make sure we have a net and comp to work with and their attributes
			if (etchData != NULL && etchAttrib != NULL && compData != NULL && compAttrib != NULL )
			{
				CString etchNetName = get_attvalue_string(doc, etchAttrib), compRefName = get_attvalue_string(doc, compAttrib);
				NetStruct *etchNet = FindNet(doc->getFileList().GetFirstShown(blockTypePcb), etchNetName);

				if (etchNet != NULL)
				{
					bool netHasComp = false;
					POSITION cpPos = etchNet->getHeadCompPinPosition();
					while (cpPos && !netHasComp)
					{
						CompPinStruct *cp = etchNet->getNextCompPin(cpPos);

						if (!cp->getRefDes().CompareNoCase(compRefName))
							netHasComp = true;;
					}

					if (netHasComp)
						continue;
				}

			}

         // Measure
         double distance = FindClosestPoints(doc, data1, &xform, FALSE, data2, &xform, FALSE, &result1, &result2);

         // CanTouch?
         if (distance < SMALLNUMBER && CanTouch) 
            continue;

			// check if a violation had occurred between these two entities, if so move on to the next entity
			//if (data1->getEntityNumber() != data2->getEntityNumber())
			//	if (violations.hasBeenViolated(data1->getEntityNumber(), data2->getEntityNumber()))
			//		continue;

         CString ref1, ref2;
         Attrib *attrib;
         if (attrib = get_attvalue(data1->getAttributes(), refnameKeyword))
            ref1 = attrib->getStringValue();
         else
            ref1.Format("%ld", data1->getEntityNumber());

         if (attrib = get_attvalue(data2->getAttributes(), refnameKeyword))
            ref2 = attrib->getStringValue();
         else
            ref2.Format("%ld", data2->getEntityNumber());

         CString ref1ToRef2 = ref1 + "<->" + ref2;
         CString ref2ToRef1 = ref2 + "<->" + ref1;  
         CString value;
         if (ref1.CompareNoCase(ref2) == 0 || violationsMap.Lookup(ref1ToRef2, value) || violationsMap.Lookup(ref2ToRef1, value))
            continue;
         
         violationsMap.SetAt(ref1ToRef2, ref1ToRef2);
         violationsMap.SetAt(ref2ToRef1, ref2ToRef1);

         if (distance < actualMarginalDist + SMALLNUMBER)
         {
            int failureRange = 1;

            if (distance < actualCriticalDist + SMALLNUMBER)
               failureRange = 0;

            double checkValue;
            char *failureRangeString;

            switch (failureRange)
            {
            case 0:
               checkValue = actualCriticalDist;
               failureRangeString = "Critical";
               break;

            case 1:
               checkValue = actualMarginalDist;
               failureRangeString = "Marginal";
               break;
            };

            if (SkipSameNets)
            {
               DiffNetsStruct *dn = new DiffNetsStruct;
               diffNetsList.AddTail(dn);
               dn->net1 = net1;
               dn->net2 = net2;
               dn->entity1 = data1->getEntityNumber();
               dn->entity2 = data2->getEntityNumber();
               dn->distance = distance;
               dn->x1 = result1.x;
               dn->y1 = result1.y;
               dn->x2 = result2.x;
               dn->y2 = result2.y;
               dn->failureRange = failureRange;
               dn->failureRangeString = failureRangeString;
               dn->checkValue = checkValue;
            }

            //CString ref1, ref2;
            //Attrib *attrib;

            ////WORD refnameKeyword = doc->IsKeyWord(ATT_REFNAME, 0);
            //if (attrib = get_attvalue(data1->getAttributes(), refnameKeyword))
            //   ref1 = attrib->getStringValue();
            //else
            //   ref1.Format("%ld", data1->getEntityNumber());

            //if (attrib = get_attvalue(data2->getAttributes(), refnameKeyword))
            //   ref2 = attrib->getStringValue();
            //else
            //   ref2.Format("%ld", data2->getEntityNumber());

            CString string;
            if (!ref1.IsEmpty() && !ref2.IsEmpty())
            {
               string += ref1;
               string += "<->";
               string += ref2;
            }
            else if (!ref1.IsEmpty())
            {
               string += ref1;
            }
            else if (!ref2.IsEmpty())
            {
               string += ref2;
            }

            CString buf;
            buf.Format(" => %.*lf ", GetDecimals(doc->getSettings().getPageUnits()), distance);
            string += buf;

            // Don't report errors here if check for nets
            if (!SkipSameNets)
            {
               DRCStruct *drc = AddDRC(doc, file, (result1.x + result2.x) / 2,  (result1.y + result2.y) / 2, string, 
                     DRC_CLASS_MEASURE, failureRange, algIndex, algType);

					errorCount++;   // Cases 1767.  error counter should be incremented only when a drc is added.
               DRC_FillMeasure(drc, DRC_ALG_GENERIC, data1->getEntityNumber(), DRC_ALG_GENERIC, data2->getEntityNumber(), (DbUnit)result1.x, (DbUnit)result1.y, (DbUnit)result2.x, (DbUnit)result2.y);
               doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, algName.GetBuffer(0), SA_OVERWRITE, NULL);
               doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, failureRangeString, SA_OVERWRITE, NULL);
               doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &checkValue, SA_OVERWRITE, NULL);
               doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), VT_UNIT_DOUBLE, &distance, SA_OVERWRITE, NULL);

               if (!ref1.IsEmpty())
                  doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_REF1, 0), VT_STRING, ref1.GetBuffer(0), SA_OVERWRITE, NULL);

               if (!ref2.IsEmpty())
                  doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_REF2, 0), VT_STRING, ref2.GetBuffer(0), SA_OVERWRITE, NULL);
            }
            
				//violations.AddViolation(data1->getEntityNumber(), data2->getEntityNumber());

            if (maxErrorsPerTest && errorCount >= maxErrorsPerTest)
               TooManyErrors = TRUE;
         } 
      }
   }

   if (OneDRCPerNetPair && !diffNetsList.IsEmpty() && (netname1.IsEmpty() || netname2.IsEmpty()))
   {
      // remove duplicate net pairs
      POSITION pos1 = diffNetsList.GetHeadPosition();
      while (pos1)
      {
         DiffNetsStruct *dn1 = diffNetsList.GetAt(pos1);
         POSITION pos2 = pos1;
         diffNetsList.GetNext(pos2);

         while (pos2)
         {
            DiffNetsStruct *dn2 = diffNetsList.GetAt(pos2);
   
            if ( (dn1->net1 == dn2->net1 && dn1->net2 == dn2->net2) || (dn1->net1 == dn2->net2 && dn1->net2 == dn2->net1) )
            {
               if (dn1->distance <= dn2->distance)
               {
                  POSITION tempPos = pos2;
                  diffNetsList.GetNext(pos2);
                  diffNetsList.RemoveAt(tempPos);
                  delete dn2;
               }
               else
               {
                  pos2 = NULL;
                  POSITION tempPos = pos1;
                  diffNetsList.GetNext(pos1);
                  diffNetsList.RemoveAt(tempPos);
                  delete dn1;

                  dn1 = diffNetsList.GetAt(pos1);
                  pos2 = pos1;
                  diffNetsList.GetNext(pos2);
               }
            }
            else
               diffNetsList.GetNext(pos2);
         }

         diffNetsList.GetNext(pos1);
      }


      // make DRCs
      CString newAlgName = algName;
      newAlgName += "_NetNames";
      int newAlgIndex = GetAlgorithmNameIndex(doc, newAlgName);

      POSITION pos = diffNetsList.GetHeadPosition();
      while (pos)
      {
         DiffNetsStruct *dn = diffNetsList.GetNext(pos);

         CString comment;
         comment.Format("%s <-> %s => %.*lf", doc->getCamCadData().getAttributeValueDictionary().getAt(dn->net1), 
                                              doc->getCamCadData().getAttributeValueDictionary().getAt(dn->net2), GetDecimals(doc->getSettings().getPageUnits()), dn->distance);
         DRCStruct *drc = AddDRC(doc, file, (dn->x1 + dn->x2) / 2,  (dn->y1 + dn->y2) / 2, comment, DRC_CLASS_MEASURE, dn->failureRange, newAlgIndex, algType);
         DRC_FillMeasure(drc, DRC_ALG_GENERIC, dn->entity1, DRC_ALG_GENERIC, dn->entity2, (DbUnit)dn->x1, (DbUnit)dn->y1, (DbUnit)dn->x2, (DbUnit)dn->y2);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, newAlgName.GetBuffer(0), SA_OVERWRITE, NULL);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, dn->failureRangeString.GetBuffer(0), SA_OVERWRITE, NULL);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &dn->checkValue, SA_OVERWRITE, NULL);
         doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), VT_UNIT_DOUBLE, &dn->distance, SA_OVERWRITE, NULL);

         delete dn;
      }

      diffNetsList.RemoveAll();
   }

   while (!diffNetsList.IsEmpty())
   {
      DiffNetsStruct *dn = diffNetsList.RemoveHead();
      delete dn;
   }

   return TooManyErrors;
}

#include "float.h"
/*******************************************************************************
* DFM_PolyExtents 
*/
void DFM_PolyExtents(CCEtoODBDoc *doc, CPolyList *polylist, ExtentRect *extents, double scale, int mirror,
      double insert_x, double insert_y, Mat2x2 *m, int use_width)
{
   POSITION polyPos, pntPos;
   CPoly *poly;
   CPnt *pnt;
   Point2 vertex, last;
   double bulge;
   double width;

   extents->left = extents->bottom = FLT_MAX;
   extents->right = extents->top = -FLT_MAX;

   polyPos = polylist->GetHeadPosition();
   while (polyPos != NULL)
   {
      poly = polylist->GetNext(polyPos);
      if (use_width && poly->getWidthIndex() >= 0)
         width = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() / 2 * scale;
      else
         width = 0;

      pntPos = poly->getPntList().GetHeadPosition();
      if (pntPos != NULL)
      {
         pnt = poly->getPntList().GetNext(pntPos);
         vertex.x = pnt->x * scale;  vertex.y = pnt->y * scale;
         if (mirror) vertex.x = -vertex.x;
         TransPoint2(&vertex, 1, m, insert_x, insert_y);

         bulge = pnt->bulge;

         extents->left = ( (vertex.x-width <= extents->left) ? vertex.x-width : extents->left);
         extents->right = ( (vertex.x+width >= extents->right) ? vertex.x+width : extents->right);
         extents->top = ( (vertex.y+width >= extents->top) ? vertex.y+width : extents->top);
         extents->bottom = ( (vertex.y-width <= extents->bottom) ? vertex.y-width : extents->bottom);

         while (pntPos != NULL)
         {
            pnt = poly->getPntList().GetNext(pntPos);

            last.x = vertex.x;  last.y = vertex.y;

            vertex.x = pnt->x * scale;  vertex.y = pnt->y * scale;
            if (mirror) vertex.x = -vertex.x;
            TransPoint2(&vertex, 1, m, insert_x, insert_y);

            extents->left = ( (vertex.x-width <= extents->left) ? vertex.x-width : extents->left);
            extents->right = ( (vertex.x+width >= extents->right) ? vertex.x+width : extents->right);
            extents->top = ( (vertex.y+width >= extents->top) ? vertex.y+width : extents->top);
            extents->bottom = ( (vertex.y-width <= extents->bottom) ? vertex.y-width : extents->bottom);

            if (fabs(bulge) > SMALLNUMBER) // bulge
            {
               double da, sa, r, cx, cy;
               da = atan(bulge) * 4;
               ArcPoint2Angle(last.x, last.y, vertex.x, vertex.y, da, &cx, &cy, &r, &sa);

               // if an extent point of circle is on correct side of line between last & vertex,
               // then it should be considered in the arc extents
               BOOL Undefined = fabs(last.x - vertex.x) < SMALLNUMBER;

               if (Undefined)
               {
                  if ((last.y > vertex.y) ^ (da < 0)) // if Left of line between last & vertex 
                  {
                     // left side of circle
                     extents->left = min(extents->left, cx-r-width);

                     // top and bottom of circle
                     if (cx < vertex.x)
                     {
                        extents->top = max(extents->top, cy+r+width);
                        extents->bottom = min(extents->bottom, cy-r-width);
                     }
                  }
                  else // if Right of line between last & vertex 
                  {
                     // right side of circle
                     extents->right = max(extents->right, cx+r+width);

                     // top and bottom of circle
                     if (cx > vertex.x)
                     {
                        extents->top = max(extents->top, cy+r+width);
                        extents->bottom = min(extents->bottom, cy-r-width);
                     }
                  }
               }
               else // defined slope of line between last & vertex
               {
                  BOOL Below; // consider points Above or Below line between last & vertex 
                  double m, b;

                  m = (last.y - vertex.y) / (last.x - vertex.x); // m = dy/dx
                  b = vertex.y - m * vertex.x; // y - mx = b

                  Below = (last.x < vertex.x) ^ (da < 0);

                  // left side of circle
                  if ((cy > m * (cx-r) + b) ^ Below)
                     extents->left = min(extents->left, cx-r-width);

                  // right side of circle
                  if ((cy > m * (cx+r) + b) ^ Below)
                     extents->right = max(extents->right, cx+r+width);

                  // top of circle
                  if ((cy+r > m * cx + b) ^ Below)
                     extents->top = max(extents->top, cy+r+width);

                  // bottom of circle
                  if ((cy-r > m * cx + b) ^ Below)
                     extents->bottom = min(extents->bottom, cy-r-width);
               }
            }
            bulge = pnt->bulge;
         }
      }
   }

   extents->top = dbunitround(extents->top);
   extents->bottom = dbunitround(extents->bottom);
   extents->left = dbunitround(extents->left);
   extents->right = dbunitround(extents->right);

   //if (fabs(extents->top) < SMALLNUMBER)      extents->top = 0;
   //if (fabs(extents->bottom) < SMALLNUMBER)   extents->bottom = 0;
   //if (fabs(extents->left) < SMALLNUMBER)     extents->left = 0;
   //if (fabs(extents->right) < SMALLNUMBER) extents->right = 0;
}

bool CViolationsMapList::hasBeenViolated(long entityNum1, long entityNum2)
{
	CWordArray *wordArray = NULL;

	if (!Lookup((WORD)entityNum1, wordArray))
		return false;

	if (wordArray == NULL)
		return false;

	for (int wordIndex=0; wordIndex<wordArray->GetCount(); wordIndex++)
	{
		WORD violatedEntityNum = wordArray->GetAt(wordIndex);
		if (violatedEntityNum == (WORD)entityNum2)
			return true;
	}

	return false;
}

void CViolationsMapList::AddViolation(long entityNum1, long entityNum2)
{
	CWordArray *wordArray1 = NULL, *wordArray2 = NULL;

	// look up the word array for entity 1
	if (!Lookup((WORD)entityNum1, wordArray1))
	{
		// since none was found, create a new word array
		wordArray1 = new CWordArray();
		SetAt((WORD)entityNum1, wordArray1);
	}

	// look up the word array for entity 2
	if (!Lookup((WORD)entityNum2, wordArray2))
	{
		// since none was found, create a new word array
		wordArray2 = new CWordArray();
		SetAt((WORD)entityNum2, wordArray2);
	}


	// look in the word array for a matching entity
	bool foundMatch = false;
	for (int wordIndex=0; wordIndex<wordArray1->GetCount() && !foundMatch; wordIndex++)
	{
		WORD violatedEntityNum = wordArray1->GetAt(wordIndex);
		if (violatedEntityNum == (WORD)entityNum2)
			foundMatch = true;
	}
	// if none was found, add the match
	if (!foundMatch)
		wordArray1->Add((WORD)entityNum2);

	// look in the word array for a matching entity
	foundMatch = false;
	for (int wordIndex=0; wordIndex<wordArray2->GetCount() && !foundMatch; wordIndex++)
	{
		WORD violatedEntityNum = wordArray2->GetAt(wordIndex);
		if (violatedEntityNum == (WORD)entityNum1)
			foundMatch = true;
	}
	// if none was found, add the match
	if (!foundMatch)
		wordArray2->Add((WORD)entityNum1);
}

