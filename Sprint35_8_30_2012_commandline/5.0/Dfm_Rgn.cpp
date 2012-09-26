
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "dfm.h"
#include "crypt.h"
#include "api.h"
#include "ccdoc.h"
#include "graph.h"
#include "rgn.h"
#include "region.h"
#include "polylib.h"
#include "drc.h"
#include "extents.h"
#include "EntityNumber.h"
#include "CCEtoODB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CCEtoODBView *apiView; // from API.CPP
extern int maxErrorsPerTest; // from DFM.CPP
extern char *FailureRanges[]; // from DRC.CPP

struct SMUStruct
{
   DataStruct *data;
   Region *region;
	bool polyIsCircle;
};
typedef CTypedPtrList<CPtrList, SMUStruct*> CSMUList;

struct SMStruct
{
   ExtentRect extents;
   Region *region;
};
typedef CTypedPtrList<CPtrList, SMStruct*> CSMList;

/******************************************************************************
* DFM::ProcessSolderMask
*/
SHORT DFM::ProcessSolderMask(SHORT soldermaskGeometryNumber, SHORT copperGeometryNumber)
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
   if (copperGeometryNumber < 0 || copperGeometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *soldermaskGeometry = doc->getBlockAt(soldermaskGeometryNumber);
   BlockStruct *copperGeometry = doc->getBlockAt(copperGeometryNumber);

   if (!soldermaskGeometry)
      return RC_HOLE_IN_ARRAY;
   if (!copperGeometry)
      return RC_HOLE_IN_ARRAY;

   CWaitCursor wait;


   FileStruct *file = doc->Find_File(copperGeometry->getFileNumber());
   doc->PrepareAddEntity(file);

   Mat2x2 m;
   RotMat2(&m, 0);

   float scaleFactor = (float)(1000.0 * Units_Factor(doc->getSettings().getPageUnits(), UNIT_INCHES));


   // build soldermask region list and soldermask extents list

   CSMList smList;

   POSITION smPos = soldermaskGeometry->getDataList().GetHeadPosition();
   while (smPos) // for each soldermask
   {
      DataStruct *smData = soldermaskGeometry->getDataList().GetNext(smPos);

      SMStruct *sm = new SMStruct;

      PolyExtents(doc, smData->getPolyList(), &sm->extents, 1, 0, 0, 0, &m, 0);
      sm->region = RegionFromPolylist(doc, smData->getPolyList(), scaleFactor);

      smList.AddTail(sm);
   }


   // start at the end so we can add new intersection polys to the tail and not loop through them
   POSITION copperPos = copperGeometry->getDataList().GetTailPosition();
   while (copperPos) // for each copper data
   {
      POSITION tempCopperPos = copperPos;
      DataStruct *copperData = copperGeometry->getDataList().GetPrev(copperPos);
      
      
      ExtentRect copperExtents;
      PolyExtents(doc, copperData->getPolyList(), &copperExtents, 1, 0, 0, 0, &m, 0);


      Region *copperRegion = NULL;
      Region *solderableRegion = new Region();


      POSITION smPos = smList.GetHeadPosition();
      while (smPos) // for each soldermask
      {
         SMStruct *sm = smList.GetNext(smPos);

         if (sm->extents.left > copperExtents.right || copperExtents.left > sm->extents.right || 
               sm->extents.bottom > copperExtents.top || copperExtents.bottom > sm->extents.top)
            continue;  


         // calculate region and intersect with all soldermask pieces and take the union of the intersections

         if (!copperRegion) // only create the region once
            copperRegion = RegionFromPolylist(doc, copperData->getPolyList(), scaleFactor);

         Region *intersectRegion = IntersectRegions(copperRegion, sm->region);
         if (intersectRegion)
         {
            solderableRegion->CombineWith(intersectRegion);
            delete intersectRegion;

            if (AreRegionsEqual(solderableRegion, copperRegion)) // copper completely covered -> don't need to check anymore soldermask
               break;
         }
      }

      if (!copperRegion)
         RemoveOneEntityFromDataList(doc, &copperGeometry->getDataList(), copperData, tempCopperPos); // if we didn't create it, it doesn't touch soldermask
      else
      {
         if (solderableRegion->IsEmpty()) // was within extents of some soldermask, but no common area with soldermask
            RemoveOneEntityFromDataList(doc, &copperGeometry->getDataList(), copperData, tempCopperPos); // if we didn't create it, it doesn't touch soldermask
         else if (!AreRegionsEqual(solderableRegion, copperRegion)) // copper not completely covered -> replace copper entity with solderableRegion
         {
            CPolyList *newPolylist = solderableRegion->GetPolys(scaleFactor);
            if (newPolylist)
            {
               FreePolyList(copperData->getPolyList());
               copperData->getPolyList() = newPolylist;
            }
            else // no solderable region
               RemoveOneEntityFromDataList(doc, &copperGeometry->getDataList(), copperData, tempCopperPos); // empty solderable region, remove entire copper
         }

         delete copperRegion; // only free if we created it
      }

      delete solderableRegion;
   }


   // free soldermask rgns
   smPos = smList.GetHeadPosition();
   while (smPos) // for each soldermask
   {
      SMStruct *sm = smList.GetNext(smPos);
      delete sm->region;
      delete sm;
   }

   return RC_SUCCESS;
}

/******************************************************************************
* DFM::ViaUnderPart
*/
SHORT DFM::ViaUnderPart(SHORT viaGeometryNumber, SHORT compGeometryNumber, LPCTSTR algName)
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

   if (viaGeometryNumber < 0 || viaGeometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;
   if (compGeometryNumber < 0 || compGeometryNumber >= doc->getMaxBlockIndex())
      return RC_INDEX_OUT_OF_RANGE;

   BlockStruct *viaGeometry = doc->getBlockAt(viaGeometryNumber);
   BlockStruct *compGeometry = doc->getBlockAt(compGeometryNumber);

   if (!viaGeometry)
      return RC_HOLE_IN_ARRAY;
   if (!compGeometry)
      return RC_HOLE_IN_ARRAY;

   CWaitCursor wait;


   FileStruct *file = doc->Find_File(compGeometry->getFileNumber());
   doc->PrepareAddEntity(file);
   int algIndex = GetAlgorithmNameIndex(doc, algName);

   Mat2x2 m;
   RotMat2(&m, 0);

   float scaleFactor = (float)(200.0 * Units_Factor(doc->getSettings().getPageUnits(), UNIT_INCHES));
   WORD refnameKeyword = doc->IsKeyWord(ATT_REFNAME, 0);

   CObList viaRegionList;

   POSITION viaPos = viaGeometry->getDataList().GetHeadPosition();
   while (viaPos)
   {
      DataStruct *viaData = viaGeometry->getDataList().GetNext(viaPos);

      Region *viaRegion = RegionFromPolylist(doc, viaData->getPolyList(), scaleFactor);
      viaRegionList.AddTail((CObject*)viaRegion);
   }


   long errorCount = 0;
   BOOL TooManyErrors = FALSE;

   // start at the end so we can add new intersection polys to the tail and not loop through them
   POSITION compPos = compGeometry->getDataList().GetTailPosition();
   while (compPos && !TooManyErrors)
   {
      DataStruct *compData = compGeometry->getDataList().GetPrev(compPos);

      Region *compRegion = RegionFromPolylist(doc, compData->getPolyList(), scaleFactor);

      viaPos = viaRegionList.GetHeadPosition();
      while (viaPos && !TooManyErrors)
      {
         Region *viaRegion = (Region*)viaRegionList.GetNext(viaPos);

         if (DoRegionsIntersect(compRegion, viaRegion))
         {
            CString refDes = "Unknown";
            Attrib* attrib;

            if (compData->getAttributesRef() && compData->getAttributesRef()->Lookup(refnameKeyword, attrib))
               refDes = attrib->getStringValue();

            CString drcString;
            drcString.Format("Via under %s", refDes);

            int failureRange = 1;
            Region *intersectRegion = IntersectRegions(compRegion, viaRegion);

            if (intersectRegion)
            {
               if (AreRegionsEqual(viaRegion, intersectRegion))
                  failureRange = 0;

               delete intersectRegion;
            }

            ExtentRect viaExtents;
            viaRegion->GetExtents(&viaExtents, scaleFactor);

            double x = (viaExtents.left + viaExtents.right) / 2;
            double y = (viaExtents.bottom + viaExtents.top) / 2;

            DRCStruct *drc = AddDRC(doc, file, x, y, drcString, DRC_CLASS_SIMPLE, failureRange, algIndex, 0);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_TESTNAME, 0), VT_STRING, (void*)algName, SA_OVERWRITE, NULL);
            doc->SetAttrib(&drc->getAttributesRef(), doc->IsKeyWord(ATT_DFM_FAILURE_RANGE, 0), VT_STRING, FailureRanges[failureRange], SA_OVERWRITE, NULL);

            errorCount++;

            if (maxErrorsPerTest && errorCount >= maxErrorsPerTest)
               TooManyErrors = TRUE;
         }
      }

      delete compRegion;
   }

   viaPos = viaRegionList.GetHeadPosition();
   while (viaPos)
   {
      Region *viaRegion = (Region*)viaRegionList.GetNext(viaPos);

      delete viaRegion;
   }

   if (TooManyErrors)
   {
      DRCStruct *drc = CreateDRC(file, "Max Error Count Hit", DRC_CLASS_NO_MARKER, 1, algIndex, 0);
      return RC_TOO_MANY_ERRORS;
   }

   return RC_SUCCESS;
}

SHORT DFM::CombinePolies(SHORT geometryNumber)
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

   BlockStruct *block = doc->getBlockAt(geometryNumber);

   if (!block)
      return RC_HOLE_IN_ARRAY;

   CWaitCursor wait;

   FileStruct *file = doc->Find_File(block->getFileNumber());
   doc->PrepareAddEntity(file);

   Mat2x2 m;
   RotMat2(&m, 0);

   float scaleFactor = (float)(1000.0 * Units_Factor(doc->getSettings().getPageUnits(), UNIT_INCHES));

   
   // build region list from polies
   CSMUList rgnList;
   POSITION polyPos = block->getDataList().GetHeadPosition();
   while (polyPos) // for each poly
   {
      SMUStruct *smu = new SMUStruct;
		smu->polyIsCircle = false;

      smu->data = block->getDataList().GetNext(polyPos);
		CPolyList* polyList = smu->data->getPolyList();

		if (polyList->GetCount() == 1)
		{
			CPoly* poly = polyList->GetHead();
			if (!poly->isFilled() || !poly->isClosed())
				continue;

			double cX, cY, radius;
			smu->polyIsCircle = PolyIsCircle(poly, &cX, &cY, &radius)==TRUE?true:false;
		}

      smu->region = RegionFromPolylist(doc, polyList, scaleFactor);
      rgnList.AddTail(smu);
   }


   // build union list of regions
   BOOL regionTouched;
   CSMUList resultRgnList;

   // Always start at the head
   POSITION smuPos = rgnList.GetHeadPosition();
   while (smuPos)
   {
      regionTouched = FALSE;
      POSITION smuPos2 = smuPos;

      // Get the first current region
      SMUStruct *smu = rgnList.GetNext(smuPos2);
      while (smuPos2)
      {
         // Get the second current region
         SMUStruct *smu2 = rgnList.GetAt(smuPos2);

         // Check if the regions touch
         if (DoRegionsTouch(smu->region, smu2->region))
         {
            // if they touch, remove both regions from the list, union them and add it to the end
            regionTouched = TRUE;

            rgnList.RemoveAt(smuPos);
            rgnList.RemoveAt(smuPos2);

            Region *resultRgn = UnionRegions(smu->region, smu2->region);

            // remove smaller region and its data, and replace the larger region with the unioned region
            if (smu->region > smu2->region)
            {
               delete smu2->region;
               delete smu2->data;
               smu2->data = NULL;

               delete smu->region;
               smu->region = resultRgn;
					smu->polyIsCircle = false;
               rgnList.AddTail(smu);
            }
            else
            {
               delete smu->region;
               delete smu->data;
               smu->data = NULL;

               delete smu2->region;
               smu2->region = resultRgn;
					smu2->polyIsCircle = false;
               rgnList.AddTail(smu2);
            }

            // start from the beginning of the list again
            break;
         }

         // move to the next region
         rgnList.GetNext(smuPos2);
      }

      if (!regionTouched)
      {
         // if the first current region didn't touch any other region then add it to the result region list
         // and remove it from the current region list
         resultRgnList.AddTail(smu);
         rgnList.RemoveAt(smuPos);
      }

      // Always start at the head
      smuPos = rgnList.GetHeadPosition();
   }
   
   // remove all polies in the current block
   block->getDataList().RemoveAll();
   Graph_Block_On(GBO_OVERWRITE, block->getName(), block->getFileNumber(), block->getFlags());

   smuPos = resultRgnList.GetHeadPosition();
   while (smuPos)
   {
      SMUStruct *smu = resultRgnList.GetNext(smuPos);

		if (!smu->polyIsCircle)
		{
			CPolyList *newPolylist = smu->region->GetPolys(scaleFactor);
			if (newPolylist)
			{
				FreePolyList(smu->data->getPolyList());
				smu->data->getPolyList() = newPolylist;
			}
		}

		AddEntity(smu->data); // entity number should already have been assigned when smu->data was constructed - knv - 20070629
      //smu->data->setEntityNumber(CEntityNumber::allocate());
   }

   // remove all unioned regions
   smuPos = resultRgnList.GetHeadPosition();
   while (smuPos != NULL)
   {
      SMUStruct *smu = resultRgnList.GetNext(smuPos);
      delete smu->region;
      delete smu;
   }
   resultRgnList.RemoveAll();

   return 0;
}
