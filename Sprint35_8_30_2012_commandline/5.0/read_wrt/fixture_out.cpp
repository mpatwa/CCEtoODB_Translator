// $Header: /CAMCAD/5.0/read_wrt/fixture_out.cpp 51    5/22/07 1:45a Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "stdafx.h"
#include "fixture_out.h"
#include "ccdoc.h"
#include "float.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static bool hasMissingTestPlan(FileStruct *file, CCEtoODBDoc *doc);

void Fixture_WriteFiles(CString filepath, CCEtoODBDoc &doc, FormatStruct *format, int page_units)
{
   FileStruct *file = doc.getFileList().GetOnlyShown(blockTypePcb);

   if (file == NULL)
      file = doc.getFileList().GetOnlyShown(blockTypePanel);

   if (file == NULL)
   {
      ErrorMessage("Only a single file shown that is of type PCB can be exported", "Fixture File Write", MB_OK);
      //ErrorMessage("Only a single file shown that is of type PCB or PCB Panel can be exported", "Fixture File Write", MB_OK);
      return;
   }

   if (hasMissingTestPlan(file, &doc))
   {
      if (file->getBlockType() == blockTypePcb)
         ErrorMessage("There is no Test Plan in the currently shown file.\nNo information to write to the fixture file.\nOperation terminated.", "Fixture File Write", MB_OK);
      else
         ErrorMessage("One or more PCBs in the panel have no Test Plan.\nAll PCBs must have a Test Plan in order to create a panel fixture file.\nOperation terminated.", "Fixture File Write", MB_OK); 

      return;
   }


   try
   {
      CFixtureFileWriter fixFile(file, &doc);

      // output all file info
      fixFile.WriteFiles(filepath);
   }
   catch (CString exception)
   {
      ErrorMessage(exception, "Fixture File Write", MB_OK);
   }
}

static bool hasMissingTestPlan(FileStruct *file, CCEtoODBDoc *doc)
{
   if (file->getBlockType() == blockTypePanel)
   {
      BlockStruct *panelBlock = file->getBlock();
      if (panelBlock == NULL)
         return true; // No panel block, then no test plan either

      for (POSITION boardPos=panelBlock->getHeadDataInsertPosition(); boardPos!=NULL; panelBlock->getNextDataInsert(boardPos))
      {
         DataStruct *boardData = panelBlock->getAtData(boardPos);
         InsertStruct *boardInsert = boardData->getInsert();

         if (boardInsert->getInsertType() == insertTypePcb)
         {
            FileStruct *boardFile = doc->getFileList().FindByBlockNumber(boardInsert->getBlockNumber());
            if (boardFile != NULL)
            {
               /*if (doc->GetCurrentDFTSolution(*boardFile) == NULL ||
                  doc->GetCurrentDFTSolution(*boardFile)->GetTestPlan() == NULL)
               {
                  return true;
               }*/
            }
         }
      }
   }
   else if (file->getBlockType() == blockTypePcb)
   {
      /*if (doc->GetCurrentDFTSolution(*file) == NULL ||
         doc->GetCurrentDFTSolution(*file)->GetTestPlan() == NULL)
      {
         return true;
      }*/
   }
   else
   {
      // Not appropriate file type, call it a failure
      return true;
   }

   // If didn't find missing testplan above, then all must be present
   return false;
}

/******************************************************************************
* CFixtureProbe
*/
CFixtureProbe::CFixtureProbe(CCEtoODBDoc *doc, FileStruct *file, DataStruct *testProbe, CTMatrix mat, int boardNumber)
{
   m_pDoc = NULL;
   m_pTestProbeData = m_pTestAccessPointData = NULL;

   m_eResourceType = testResourceTypeTest;

   m_iBoardNumber = boardNumber;
   m_sName = "NO_PROBE";
   m_bPlaced = false;
   m_sNetName = "NO_NET";
   m_sTemplateName = "NO TEMPLATE";
   m_eResourceType = testResourceTypeTest;
   m_sTesterInterface = "-1";
   m_iSize = 0;
   m_dDrillSize = 0.0;
   m_dExposedMetalDiameter = 0.0;
   m_sTechnology = "UNKNOWN";
   m_sProbeMessage = "";
   m_dExposedMetalDiameter = 0.0;

   if (doc == NULL || testProbe == NULL)
      return;

   if (testProbe->getDataType() != T_INSERT)
      return;

   if (testProbe->getInsert()->getInsertType() != insertTypeTestProbe)
      return;

   EEntityType eType = entityTypeUndefined;
   Attrib *attrib = NULL;
   //VOID *voidPtr = NULL;
   int linkEntity = 0;

   m_pDoc = doc;
   m_pTestProbeData = testProbe;
   m_sName = m_pTestProbeData->getInsert()->getRefname();
   m_ptLocation = m_pTestProbeData->getInsert()->getOrigin2d();
   mat.transform(m_ptLocation);

   WORD ppKW = m_pDoc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, valueTypeString);

   if (m_pTestProbeData->lookUpAttrib(ppKW, attrib))
      m_bPlaced = ((CString)get_attvalue_string(m_pDoc, attrib) == "Placed");
   
   // get the net we are probing
   WORD netKW = m_pDoc->RegisterKeyWord(ATT_NETNAME, 0, valueTypeString);

   if (m_pTestProbeData->lookUpAttrib(netKW, attrib))
      m_sNetName = get_attvalue_string(m_pDoc, attrib);

   // get the entity of the test access point the test probe is linked to
   WORD linkKW = m_pDoc->RegisterKeyWord(ATT_DDLINK, 0, valueTypeInteger);

   if (!m_pTestProbeData->lookUpAttrib(linkKW, attrib))
      return;

   //if ((voidPtr = FindEntity(m_pDoc, attrib->getIntValue(), eType)) != NULL && eType == entityTypeData)
   //   m_pTestAccessPointData = (DataStruct*)voidPtr;
   CEntity accessEntity = CEntity::findEntity(m_pDoc->getCamCadData(), attrib->getIntValue());

   if (accessEntity.getEntityType() == entityTypeData)
   {
      m_pTestAccessPointData = accessEntity.getData();
   }

   if (m_pTestAccessPointData == NULL || !m_pTestAccessPointData->lookUpAttrib(linkKW, attrib))
      return;

   m_probedEntity = CEntity::findEntity(m_pDoc->getCamCadData(), attrib->getIntValue());

   // get exposed metal diameter
   WORD expMetalKW = m_pDoc->RegisterKeyWord(DFT_ATT_EXPOSE_METAL_DIAMETER, 0, valueTypeUnitDouble);

   if (m_pTestAccessPointData->lookUpAttrib(expMetalKW, attrib))
      m_dExposedMetalDiameter = attrib->getDoubleValue();

   // get the resource type
   WORD testResKW = m_pDoc->RegisterKeyWord(ATT_TESTRESOURCE, 0, valueTypeString);

   if (m_pTestProbeData->lookUpAttrib(testResKW, attrib))
   {
      CString testValue = get_attvalue_string(m_pDoc, attrib);

      if (testValue == "Test")
         m_eResourceType = testResourceTypeTest;
      else if (testValue == "Power Injection")
         m_eResourceType = testResourceTypePowerInjection;
   }

   // get the tester interface id
   WORD testInterfaceKW = m_pDoc->RegisterKeyWord(ATT_TESTERINTERFACE, 0, valueTypeString);

   if (m_pTestProbeData->lookUpAttrib(testInterfaceKW, attrib))
      m_sTesterInterface = get_attvalue_string(m_pDoc, attrib);

   // get the probe size
   // This is not reliable. It depends on probe name in Probes tab of Probe Placement
	// being a "size" based name
   BlockStruct *insBlock = m_pDoc->getBlockAt(m_pTestProbeData->getInsert()->getBlockNumber());
   m_iSize = atoi(insBlock->getName());

   // get the probe drill size
   POSITION pos = insBlock->getHeadDataInsertPosition();
   while (pos)
   {
      DataStruct *drillData = insBlock->getNextDataInsert(pos);
      BlockStruct *drillBlock = m_pDoc->getBlockAt(drillData->getInsert()->getBlockNumber());

      if (drillBlock!=NULL && drillBlock->getBlockType() == blockTypeDrillHole)
      {
         m_dDrillSize = drillBlock->getToolSize();
         break;
      }
   }

   // get the feature technology
   WORD techKW = m_pDoc->RegisterKeyWord(ATT_TECHNOLOGY, 0, valueTypeString);

   //if (m_eProbedType == entityTypeCompPin)
   if (m_probedEntity.getEntityType() == entityTypeCompPin)
   {
      BlockStruct *fileBlock = file->getBlock();

      POSITION dataPos = fileBlock->getHeadDataInsertPosition();
      while (dataPos)
      {
         DataStruct *data = fileBlock->getNextDataInsert(dataPos);
         InsertStruct *insert = data->getInsert();

         //if (insert->getInsertType() != insertTypePcbComponent)
           // continue;

         //if (insert->getRefname() != m_probedEntity.getCompPin()->getRefDes())
           // continue;
			if (insert->getInsertType() == insertTypePcbComponent && insert->getRefname() == m_probedEntity.getCompPin()->getRefDes())
			{
				if (data->lookUpAttrib(techKW, attrib))
					m_sTechnology = get_attvalue_string(m_pDoc, attrib);
			
				break;
			}
      }
   }
   //else if (m_eProbedType == entityTypeData)
   else if (m_probedEntity.getEntityType() == entityTypeData)
   {
      if (m_probedEntity.getData()!=NULL && m_probedEntity.getData()->lookUpAttrib(techKW, attrib))
         m_sTechnology = get_attvalue_string(m_pDoc, attrib);
   }

   // get probe style
   WORD probeStyleKW = (WORD)m_pDoc->getStandardAttributeKeywordIndex(standardAttributeProbeStyle); 

   if (m_pTestProbeData->lookUpAttrib(probeStyleKW, attrib))
      m_sProbeStyle = get_attvalue_string(m_pDoc, attrib);

   //get the probe message
   Attrib* attribProbeMessage = is_attvalue(m_pDoc, testProbe->getAttributeMap(), "PROBE_MESSAGE", 0);

   if (attribProbeMessage)
   {
      m_sProbeMessage = get_attvalue_string(m_pDoc, attribProbeMessage);
   }

   // Probe template name
   int indx = insBlock->getName().Find("_Bot");
   if (indx < 0)
      indx = insBlock->getName().Find("_Top");
	if (indx < 0 && !m_sProbeStyle.IsEmpty())
	{
		CString probeStyleWithSeparator;
		probeStyleWithSeparator.Format("_%s", m_sProbeStyle);
		indx = insBlock->getName().Find(probeStyleWithSeparator);
	}

   if (indx > -1)
      m_sTemplateName = insBlock->getName().Mid(0, indx);
   else
      m_sTemplateName = insBlock->getName();

}

bool CFixtureProbe::IsOnTop() const
{
   if (m_pTestProbeData != NULL)
   {
      if (m_pTestProbeData->getInsert()->getLayerMirrored())
         return false;
      else
         return true;
   }
   else
      return true;
}

CString CFixtureProbe::GetCompName() const
{
   //if (m_eProbedType != entityTypeCompPin || m_pProbedItem == NULL)
   //   return "";

   //return ((CompPinStruct*)m_pProbedItem)->getRefDes();

   CString retval;
   
   if (m_probedEntity.getEntityType() == entityTypeCompPin)
   {
      retval = m_probedEntity.getCompPin()->getRefDes();
   }

   return retval;
}

CString CFixtureProbe::GetPinName() const
{
   //if (m_eProbedType != entityTypeCompPin || m_pProbedItem == NULL)
   //   return "";

   //return ((CompPinStruct*)m_pProbedItem)->getPinName();

   CString retval;
   
   if (m_probedEntity.getEntityType() == entityTypeCompPin)
   {
      retval = m_probedEntity.getCompPin()->getPinName();
   }

   return retval;
}

CString CFixtureProbe::GetRefName() const
{
   CString retval;
   
   if (m_probedEntity.getEntityType() == entityTypeData)
   {
      if (m_probedEntity.getData()!= NULL && m_probedEntity.getData()->getDataType() == dataTypeInsert)
      {
         retval = m_probedEntity.getData()->getInsert()->getRefname();
      }		
   }
   else if (m_probedEntity.getEntityType() == entityTypeCompPin)
   {
      retval = m_probedEntity.getCompPin()->getPinRef('-');
   }

   return retval;
}

CString CFixtureProbe::GetResourceTypeString() const
{
   switch (m_eResourceType)
   {
   case testResourceTypeTest:
      return "Test";
   case testResourceTypePowerInjection:
      return "Power Injection";
   }

   return "";
}

//_____________________________________________________________________________
CProbe::CProbe(CCEtoODBDoc *doc, FileStruct *file, DataStruct *testProbe, CTMatrix mat)
: CFixtureProbe(doc, file, testProbe, mat)
{
   m_dFeatureDrillSize = 0.0;

   WORD ttKw = doc->RegisterKeyWord(ATT_DFT_TARGETTYPE, 0, valueTypeString);
   Attrib *attrib = NULL;
   if (m_pTestAccessPointData!=NULL && m_pTestAccessPointData->lookUpAttrib(ttKw, attrib))
      m_sTargetType = get_attvalue_string(doc, attrib);


   BlockStruct *probeBlock = doc->getBlockAt(testProbe->getInsert()->getBlockNumber());

   BlockStruct *insBlock = NULL;

	if (m_probedEntity.getEntityType() == entityTypeCompPin)
	{
		insBlock = doc->getBlockAt(m_probedEntity.getCompPin()->getPadstackBlockNumber());
	}
	else if (m_probedEntity.getEntityType() == entityTypeData)
	{
		if (m_probedEntity.getData()!=NULL && m_probedEntity.getData()->getDataType() == dataTypeInsert)
		{
			insBlock = doc->getBlockAt(m_probedEntity.getData()->getInsert()->getBlockNumber());
		}
	}
	// else other entity types not do not qualify


	if (insBlock != NULL)
	{
		// get the probe drill size
		POSITION pos = insBlock->getHeadDataInsertPosition();
		while (pos)
		{
			DataStruct *drillData = insBlock->getNextDataInsert(pos);
			BlockStruct *drillBlock = m_pDoc->getBlockAt(drillData->getInsert()->getBlockNumber());

			if (drillBlock != NULL && drillBlock->getBlockType() == blockTypeDrillHole)
			{
				m_dFeatureDrillSize = drillBlock->getToolSize();
				break;
			}
		}
	}
}   

/******************************************************************************
* CFixtureAccLoc
*/
void CFixtureAccLoc::DumpToFile(CFormatStdioFile &file, int indent)
{
   CString accLoc;
   CString featureType("Undefined");

   //if (GetFeatureType() == entityTypeCompPin)
   if (getFeatureEntity().getEntityType() == entityTypeCompPin)
   {
      //CompPinStruct *cp = (CompPinStruct*)GetFeature();
      CompPinStruct* cp = getFeatureEntity().getCompPin();
      accLoc.Format("CompPin %s [%d] at 0x%08x", cp->getPinRef(), cp->getEntityNumber(), this);
      featureType = "CompPin";
   }
   //else if (GetFeatureType() == entityTypeData)
   else if (getFeatureEntity().getEntityType() == entityTypeData)
   {
      //DataStruct *data = (DataStruct*)GetFeature();
      DataStruct* data = getFeatureEntity().getData();
		if (data !=NULL)
			accLoc.Format("Via %s [%d] at 0x%08x", data->getInsert()->getRefname(), data->getEntityNumber(), this);
		featureType = "Data (Via)";
   }

   file.WriteString("%*s::Accessable Location on %s::\n", indent, " ", accLoc);

   indent += 3;

   CString buf;
   //file.WriteString("%*sm_eFeatureType = %s\n", indent, " ", (GetFeatureType()==entityTypeCompPin)?"CompPin":"Data (Via)");
   file.WriteString("%*sm_eFeatureType = %s\n", indent, " ", featureType);
   file.WriteString("%*sm_pTA = %s\n", indent, " ", (GetTAInsert()==NULL)?"None":GetTAInsert()->getInsert()->getRefname());
   file.WriteString("%*sm_ptLocation = (%0.3f, %0.3f)\n", indent, " ", GetLocation().x, GetLocation().y);
   file.WriteString("%*sm_sTargetType = %s\n", indent, " ", GetTargetType());
   file.WriteString("%*sm_sNetName = %s\n", indent, " ", GetNetName());
   file.WriteString("%*sm_eSurface = %s\n", indent, " ", (GetSurface()==testSurfaceTop)?"Top":"Bottom");

   switch (GetFeatureCondition())
   {
   case featureConditionProbeable:
      buf = "Probable";
      break;
   case featureConditionForced:
      buf = "Forced";
      break;
   case featureConditionNoProbe:
      buf = "No Probe";
      break;
   }

   file.WriteString("%*sm_eFeatureCondition = %s\n", indent, " ", buf);
   file.WriteString("%*sm_pProbe = 0x%08x\n", indent, " ", GetPlaceableProbe());
   file.WriteString("%*sm_pDoubleWiredProbe = 0x%08x\n", indent, " ", GetDoubleWiredProbe());
   file.WriteString("%*sm_bForFixtureFile = %s\n", indent, " ", m_bForFixtureFile?"True":"False");

   indent -= 3;
}

/******************************************************************************
* CFixtureNet
*/
CPPAccessibleLocation *CFixtureNet::AddTail_AccessibleLocations(DataStruct *testAccessData)
{
   CFixtureAccLoc *accessibleLocation = new CFixtureAccLoc(m_pDoc, testAccessData);
   m_accessibleLocations.AddTail(accessibleLocation);

   return accessibleLocation;
}



/******************************************************************************
* CPilotDrillSolution
*/
void CPilotDrillSolution::DumpToFile(CFormatStdioFile &file, int indent)
{
   CProbePlacementSolution::DumpToFile(file, indent);
}

void CPilotDrillSolution::addAllNets()
{
   RemoveAll_Nets();

   //////////////////////////////////////////////////
   // Gather nets
   POSITION pos = m_pFile->getNetList().GetHeadPosition();
   while (pos)
   {
      NetStruct *net = m_pFile->getNetList().GetNext(pos);

      CPPNet *ppNet = NULL;
      if (m_ppNets.Lookup(net->getNetName(), ppNet))
         continue;

      m_ppNets.SetAt(net->getNetName(), new CFixtureNet(m_pDoc, net->getNetName()));
   }
}

void CPilotDrillSolution::gatherAndMatchProbes()
{
   //////////////////////////////////////////////////
   // Gather accessible locations with probes
   BlockStruct *fileBlock = m_pFile->getBlock();
   if (fileBlock != NULL)
   {
      POSITION pos = fileBlock->getHeadDataInsertPosition();
      while (pos)
      {
         DataStruct *data = fileBlock->getNextDataInsert(pos);
         InsertStruct *insert = data->getInsert();

         if (insert->getInsertType() != insertTypeTestProbe)
            continue;

         long entityLink = 0;
         Attrib *attrib = NULL;
         WORD ddKw = m_pDoc->RegisterKeyWord(ATT_DDLINK, 0, valueTypeInteger);
         if (data->lookUpAttrib(ddKw, attrib))
            entityLink = attrib->getIntValue();

         CString netName;
         WORD netKw = m_pDoc->RegisterKeyWord(ATT_NETNAME, 0, valueTypeString);
         if (data->lookUpAttrib(netKw , attrib))
            netName = get_attvalue_string(m_pDoc, attrib);

         ETestResourceType testRes = testResourceTypeTest;
         WORD testResKw = m_pDoc->RegisterKeyWord(ATT_TESTRESOURCE, 0, valueTypeString);
         if (data->lookUpAttrib(testResKw , attrib))
            testRes = ((CString)"Test" == get_attvalue_string(m_pDoc, attrib))?testResourceTypeTest:testResourceTypePowerInjection;

         CPPProbe *probe = AddTail_Probes(netName, testRes);
         probe->SetProbeRefname(insert->getRefname());

         ETestSurface probeSurface = testSurfaceUnset;
         POSITION accPos = GetHeadPosition_AccessibleLocations(netName);
         while (accPos)
         {
            CPPAccessibleLocation *accLoc = GetNext_AccessibleLocations(netName, accPos);

            if (accLoc->GetTAInsert()->getEntityNumber() != entityLink)
               continue;

            probeSurface = accLoc->GetSurface();
            if (!accLoc->IsProbed())
            {
               accLoc->PlaceProbe(probe);
               ((CFixtureAccLoc*)accLoc)->SetForFixtureFlag(false);
               break;
            }
            else if (!accLoc->IsDoubleWired())
            {
               accLoc->PlaceDoubleWiredProbe(probe);
               ((CFixtureAccLoc*)accLoc)->SetForFixtureFlag(false);
               break;
            }
         }

         // Get the probe size
         BlockStruct *insBlock = m_pDoc->getBlockAt(insert->getBlockNumber());
         CString m_sTemplateName;
         int indx = insBlock->getName().Find("_Bot");
         if (indx < 0)
            indx = insBlock->getName().Find("_Top");
         if (indx > -1)
            m_sTemplateName = insBlock->getName().Mid(0, indx);
         else
            m_sTemplateName = insBlock->getName();
   
         if (probeSurface == testSurfaceTop)
         {
            POSITION probePos = m_pTestPlan->GetProbes().GetHeadPosition_TopProbes();
            while (probePos)
            {
               CDFTProbeTemplate *pTemplate = m_pTestPlan->GetProbes().GetNext_TopProbes(probePos);

               if (pTemplate->GetName() ==  m_sTemplateName)
               {
                  probe->SetProbeTemplate(pTemplate);
                  break;
               }
            } // END while (probePos) ...
         }
         else if (probeSurface == testSurfaceBottom)
         {
            POSITION probePos = m_pTestPlan->GetProbes().GetHeadPosition_BotProbes();
            while (probePos)
            {
               CDFTProbeTemplate *pTemplate = m_pTestPlan->GetProbes().GetNext_BotProbes(probePos);

               if (pTemplate->GetName() ==  m_sTemplateName)
               {
                  probe->SetProbeTemplate(pTemplate);
                  break;
               }
            } // END while (probePos) ...
         }
      } // END while (pos) ...
   } // END if (fileBlock != NULL) ...
}

void CPilotDrillSolution::createAndPlaceProbes()
{
   POSITION pos = m_ppNets.GetStartPosition();
   while (pos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(pos, netName, ppNet);

      if (ppNet->GetNoProbeFlag())
         continue;

      POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations();
      while (accPos)
      {
         CFixtureAccLoc *accLoc = (CFixtureAccLoc*)ppNet->GetNext_AccessibleLocations(accPos);

         if (!accLoc->IsForFixtureFile())
            continue;

         if (accLoc->IsProbed() || accLoc->IsDoubleWired())
            continue;

         accLoc->PlaceProbe(AddTail_Probes(netName, testResourceTypeTest));
      }
   }
}

int CPilotDrillSolution::maximizeFixtureProbeSizes()
{
   int removeCount = 0;

   POSITION netPos = m_ppNets.GetStartPosition();
   while (netPos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(netPos, netName, ppNet);

      if (ppNet->GetNoProbeFlag())
         continue;

      POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations();
      while (accPos)
      {
         CFixtureAccLoc *accLoc = (CFixtureAccLoc*)ppNet->GetNext_AccessibleLocations(accPos);

         if (accLoc->GetSurface() != m_pTestPlan->GetProbeSide() && m_pTestPlan->GetProbeSide() != testSurfaceBoth)
            continue;
         if (!accLoc->IsProbed())
            continue;
         if (!accLoc->IsForFixtureFile())
            continue;

         POSITION dummyPos = NULL;
         CDFTProbeTemplate *largestProbeTemplate = NULL;
         if (accLoc->GetSurface() == testSurfaceTop)
            largestProbeTemplate = m_pTestPlan->GetProbes().GetLargest_UsedTopProbes(dummyPos);
         else if (accLoc->GetSurface() == testSurfaceBottom)
            largestProbeTemplate = m_pTestPlan->GetProbes().GetLargest_UsedBotProbes(dummyPos);

         double testDiameter = 0.0;
         if (largestProbeTemplate != NULL)
            testDiameter = largestProbeTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());

         CAccessibleLocationList foundList;
         int probeDensity = getProbeDensity(accLoc->GetLocation(), testDiameter, accLoc->GetSurface(), foundList);

         // if there is nothing in the way
         if (probeDensity <= 1)
         {
            CDFTProbeTemplate *pTemplate = getOptimalProbeSize(accLoc);
            CPPProbe *probe = accLoc->GetPlaceableProbe();
            probe->SetProbeTemplate(pTemplate);
            continue;
         }

         //if (accLoc->GetPlaceableProbe()->HasProbeTemplate())
         // continue;

         POSITION tempPos;
         CDFTProbeTemplate *accTemplate = NULL;
         if (accLoc->GetPlaceableProbe()->HasProbeTemplate())
            accTemplate = m_pTestPlan->GetProbes().Find_Probe(accLoc->GetPlaceableProbe()->GetProbeTemplateName(), tempPos, accLoc->GetSurface());

         // of those found, look for the closest probe
         CFixtureAccLoc *closestAccLoc = NULL;
         CDFTProbeTemplate *closestAccTemplate = NULL;
         double closestDistance = DBL_MAX;
         POSITION foundPos = foundList.GetHeadPosition();
         while (foundPos)
         {
            CQfeAccessibleLocation *qfeLoc = foundList.GetNext(foundPos);
            CFixtureAccLoc *qfeAccLoc = (CFixtureAccLoc*)qfeLoc->GetAccessibleLocation();

            if (qfeAccLoc->GetPlaceableProbe() == NULL)
               continue;

            if (accLoc == qfeAccLoc)
               continue;

            double distance = accLoc->GetLocation().distance(qfeAccLoc->GetLocation());
            if (distance < closestDistance)
            {
               closestAccLoc = qfeAccLoc;
               closestDistance = distance;

               if (closestAccLoc->GetPlaceableProbe()->HasProbeTemplate())
                  closestAccTemplate = m_pTestPlan->GetProbes().Find_Probe(closestAccLoc->GetPlaceableProbe()->GetProbeTemplateName(), tempPos, accLoc->GetSurface());
               else
                  closestAccTemplate = NULL;
            }
            else if (distance - closestDistance < SMALLNUMBER && qfeAccLoc->GetPlaceableProbe()->HasProbeTemplate())
            {
               CDFTProbeTemplate *qfeAccTemplate = NULL;
               qfeAccTemplate = m_pTestPlan->GetProbes().Find_Probe(qfeAccLoc->GetPlaceableProbe()->GetProbeTemplateName(), tempPos, accLoc->GetSurface());

               // if the distances are basically the same, favor the access point that has the largest probe template assigned
               if (closestAccTemplate != NULL && qfeAccTemplate != NULL && closestAccTemplate->GetDiameter() < qfeAccTemplate->GetDiameter())
               {
                  closestAccLoc = qfeAccLoc;
                  closestDistance = distance;
                  closestAccTemplate = qfeAccTemplate;
               }
               else if (qfeAccTemplate != NULL)
               {
                  closestAccLoc = qfeAccLoc;
                  closestDistance = distance;
                  closestAccTemplate = qfeAccTemplate;
               }
            }
         }

         if (closestAccLoc != NULL)
         {
            CDFTProbeTemplate *temp1 = accTemplate, *temp2 = closestAccTemplate;

            // if there are probe templates on both of them, make sure they don't violate each other
            if (temp1 != NULL && temp2 != NULL)
            {
               // if there is a violation, remove the template off the current probe
               double diameter1 = temp1->GetDiameter()* Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits()),
                     diameter2 = temp2->GetDiameter()* Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
               if ((diameter1+diameter2)/2 > closestDistance)
                  temp1 = NULL;
            }

            // TODO: Locking - Fix maximizing of probe sized to work with Retaining Probe Sizes
            getOptimalProbeCombination(accLoc, closestAccLoc, temp1, temp2);
            if ((m_pTestPlan->RetainProbeSizes() && accLoc->GetPlaceableProbe()->DidExist()) || !m_pTestPlan->RetainProbeSizes())
            {
               if (accLoc->GetPlaceableProbe()->HasProbeTemplate() && temp1 != NULL)
               {
                  if (temp1->GetDiameter() < accTemplate->GetDiameter())
                     accLoc->GetPlaceableProbe()->SetProbeTemplate(temp1);
               }
               else
                  accLoc->GetPlaceableProbe()->SetProbeTemplate(temp1);
            }

            if ((m_pTestPlan->RetainProbeSizes() && closestAccLoc->GetPlaceableProbe()->DidExist()) || !m_pTestPlan->RetainProbeSizes())
            {
               if (closestAccLoc->GetPlaceableProbe()->HasProbeTemplate() && temp2 != NULL)
               {
                  if (temp2->GetDiameter() < closestAccTemplate->GetDiameter())
                     closestAccLoc->GetPlaceableProbe()->SetProbeTemplate(temp2);
               }
               else
                  closestAccLoc->GetPlaceableProbe()->SetProbeTemplate(temp2);
            }

            // if no available probe combination is possible, we may need to remove one so another may fit
            // remove the one with the highest density, or if equal, the current probe
            if (temp1 == NULL && temp2 == NULL)
            {
               // start with the density of the current one
               CPPAccessibleLocation *highestDensityAccLoc = NULL;
               int highestDensity = 0;

               foundPos = foundList.GetHeadPosition();
               while (foundPos)
               {
                  CQfeAccessibleLocation *qfeLoc = foundList.GetNext(foundPos);
                  CPPAccessibleLocation *qfeAccLoc = qfeLoc->GetAccessibleLocation();

                  if (qfeAccLoc->GetPlaceableProbe() == NULL)
                     continue;

                  if (accLoc == qfeAccLoc)
                     continue;
   
                  CAccessibleLocationList tempFoundList;
                  int density = getProbeDensity(qfeAccLoc->GetLocation(), testDiameter, qfeAccLoc->GetSurface(), tempFoundList);
                  if (density > highestDensity && ((CFixtureAccLoc*)qfeAccLoc)->IsForFixtureFile())
                  {
                     highestDensityAccLoc = qfeAccLoc;
                     highestDensity = density;
                  }
               } // END while (foundPos) ...

               //  Remove the probe that has the highest density.  If the density of the current probe is the highest, even if
               // equal to any other, then it is deleted.  Otherwise delete the highest density probe.
               if (highestDensity > probeDensity)
                  highestDensityAccLoc->RemoveProbe();
               else
                  accLoc->RemoveProbe();

               removeCount++;
            } // END if (temp1 == NULL && temp2 == NULL) ...
            // if one of the probe combination has no valid template and the other is an original probe (not for fixture),
            // we need to remove that probe
            else if ((temp1 == NULL && !closestAccLoc->IsForFixtureFile()) || (temp2 == NULL && !accLoc->IsForFixtureFile()))
            {
               if (temp1 == NULL)
                  accLoc->RemoveProbe();
               else if (temp2 == NULL)
                  closestAccLoc->RemoveProbe();
            } // END if ((temp1 == NULL && !closestAccLoc->IsForFixtureFile()) || ...
         }
      } // END while (accPos) ...
   } // END while (netPos) ...

   return removeCount;


   //POSITION netPos = m_ppNets.GetStartPosition();
   //while (netPos)
   //{
   // CString netName;
   // CPPNet *ppNet = NULL;
   // m_ppNets.GetNextAssoc(netPos, netName, ppNet);

   // if (ppNet->GetNoProbeFlag())
   //    continue;

   // POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations();
   // while (accPos)
   // {
   //    CFixtureAccLoc *accLoc = (CFixtureAccLoc*)ppNet->GetNext_AccessibleLocations(accPos);

   //    if (accLoc->GetSurface() != m_pTestPlan->GetProbeSide() && m_pTestPlan->GetProbeSide() != testSurfaceBoth)
   //       continue;
   //    if (!accLoc->IsProbed())
   //       continue;
   //    if (!accLoc->IsForFixtureFile())
   //       continue;

   //    POSITION dummyPos = NULL;
   //    CDFTProbeTemplate *largestProbeTemplate = NULL;
   //    if (accLoc->GetSurface() == testSurfaceTop)
   //       largestProbeTemplate = m_pTestPlan->GetProbes().GetLargest_UsedTopProbes(dummyPos);
   //    else if (accLoc->GetSurface() == testSurfaceBottom)
   //       largestProbeTemplate = m_pTestPlan->GetProbes().GetLargest_UsedBotProbes(dummyPos);

   //    double testDiameter = (double)largestProbeTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());

   //    CAccessibleLocationList foundList;
   //    int probeDensity = getProbeDensity(accLoc->GetLocation(), testDiameter, accLoc->GetSurface(), foundList);

   //    // if there is nothing in the way
   //    if (probeDensity <= 1)
   //    {
   //       CDFTProbeTemplate *pTemplate = getOptimalProbeSize(accLoc);
   //       CPPProbe *probe = accLoc->GetPlaceableProbe();
   //       probe->SetProbeTemplate(pTemplate);
   //       continue;
   //    }

   //    if (accLoc->GetPlaceableProbe()->HasProbeTemplate())
   //       continue;

   //    // of those found, look for the closest probe
   //    CQfeAccessibleLocation *closestQfeLoc = NULL;
   //    double distance = DBL_MAX, closestDistance = DBL_MAX;
   //    POSITION foundPos = foundList.GetHeadPosition();
   //    while (foundPos)
   //    {
   //       CQfeAccessibleLocation *qfeLoc = foundList.GetNext(foundPos);

   //       if (qfeLoc->GetAccessibleLocation()->GetPlaceableProbe() == NULL)
   //          continue;

   //       CFixtureAccLoc *qfeAccLoc = (CFixtureAccLoc*)qfeLoc->GetAccessibleLocation();
   //       if (accLoc == qfeAccLoc)
   //          continue;

   //       distance = accLoc->GetLocation().distance(qfeLoc->GetAccessibleLocation()->GetLocation());
   //       if (distance < closestDistance && qfeAccLoc->IsForFixtureFile())
   //       {
   //          closestQfeLoc = qfeLoc;
   //          closestDistance = distance;
   //       }
   //    }

   //    if (closestQfeLoc != NULL)
   //    {
   //       CDFTProbeTemplate *temp1 = NULL, *temp2 = NULL;
   //       getOptimalProbeCombination(accLoc, closestQfeLoc->GetAccessibleLocation(), temp1, temp2);
   //       if (!accLoc->GetPlaceableProbe()->HasProbeTemplate())
   //          accLoc->GetPlaceableProbe()->SetProbeTemplate(temp1);
   //       if (!closestQfeLoc->GetAccessibleLocation()->GetPlaceableProbe()->HasProbeTemplate())
   //          closestQfeLoc->GetAccessibleLocation()->GetPlaceableProbe()->SetProbeTemplate(temp2);
   //    }
   // } // END while (accPos) ...
   //} // END while (netPos) ...

   //return 0;
}

int CPilotDrillSolution::removeFixtureTemplatesFromProbes()
{
   int removeCount = 0;

   POSITION netPos = m_ppNets.GetStartPosition();
   while (netPos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(netPos, netName, ppNet);

      if (ppNet->GetNoProbeFlag())
         continue;

      POSITION probePos = ppNet->GetHeadPosition_Probes();
      while (probePos)
      {
         CPPProbe *probe = ppNet->GetNext_Probes(probePos);
         CFixtureAccLoc *accLoc = (CFixtureAccLoc*)probe->GetAccessibleLocation();

         if (accLoc != NULL && probe->HasProbeTemplate() && accLoc->IsForFixtureFile())
         {
            probe->SetProbeTemplate(NULL);
            removeCount++;
         }
      }
   }

   return removeCount;
}

int CPilotDrillSolution::fixViolatingFixtureProbeTemplates()
{
   int violationCount = 0;

   POSITION netPos = m_ppNets.GetStartPosition();
   while (netPos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(netPos, netName, ppNet);

      if (ppNet->GetNoProbeFlag())
         continue;

      POSITION probePos = ppNet->GetHeadPosition_Probes();
      while (probePos)
      {
         CPPProbe *probe = ppNet->GetNext_Probes(probePos);
         CFixtureAccLoc *accLoc = (CFixtureAccLoc*)probe->GetAccessibleLocation();
         POSITION dummyPos = NULL;

         if (accLoc == NULL)
            continue;

         if (probe->HasProbeTemplate())
         {
            CDFTProbeTemplate *largestProbeTemplate = NULL;
            if (accLoc->GetSurface() == testSurfaceTop)
               largestProbeTemplate = m_pTestPlan->GetProbes().GetLargest_UsedTopProbes(dummyPos);
            else if (accLoc->GetSurface() == testSurfaceBottom)
               largestProbeTemplate = m_pTestPlan->GetProbes().GetLargest_UsedBotProbes(dummyPos);

            double testDiameter = 0.0;
            if (largestProbeTemplate != NULL)
               testDiameter = largestProbeTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());

            CAccessibleLocationList foundList;
            int probeDensity = getProbeDensity(accLoc->GetLocation(), testDiameter, accLoc->GetSurface(), foundList);

            // if there is nothing in the way
            if (probeDensity <= 1)
               continue;
            CDFTProbeTemplate *probeTemplate = m_pTestPlan->GetProbes().Find_Probe(probe->GetProbeTemplateName(), dummyPos, accLoc->GetSurface());

            // see if this access location violates any around it
            POSITION foundPos = foundList.GetHeadPosition();
            while (foundPos && probeTemplate)
            {
               CQfeAccessibleLocation *qfeLoc = foundList.GetNext(foundPos);
               CFixtureAccLoc *qfeAccLoc = (CFixtureAccLoc*)qfeLoc->GetAccessibleLocation();

               if (qfeAccLoc->GetPlaceableProbe() == NULL)
                  continue;

               if (!qfeAccLoc->GetPlaceableProbe()->HasProbeTemplate())
                  continue;

               if (accLoc == qfeAccLoc)
                  continue;
					
               POSITION dummyPos = NULL;
               CDFTProbeTemplate *qfeProbeTemplate = m_pTestPlan->GetProbes().Find_Probe(qfeAccLoc->GetPlaceableProbe()->GetProbeTemplateName(), dummyPos, accLoc->GetSurface());
					
               double distance = accLoc->GetLocation().distance(qfeAccLoc->GetLocation());              
					double probeRadius = probeTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits()) / 2.0;
               double qfeRadius = 0.0;

					if (qfeProbeTemplate != NULL)
						qfeRadius = qfeProbeTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits()) / 2.0;
               
					if (distance < (probeRadius + qfeRadius))
               {
                  CDFTProbeTemplate *temp1 = accLoc->IsForFixtureFile()?NULL:probeTemplate,
                                    *temp2 = qfeAccLoc->IsForFixtureFile()?NULL:qfeProbeTemplate;

                  getOptimalProbeCombination(accLoc, qfeAccLoc, temp1, temp2);
                  
                  if (accLoc->GetPlaceableProbe() != NULL)
                  {
                     if (accLoc->GetPlaceableProbe()->HasProbeTemplate() && temp1 != NULL)
                     {
                        if (temp1->GetDiameter() < probeTemplate->GetDiameter())
                        {
                           accLoc->GetPlaceableProbe()->SetProbeTemplate(temp1);
                           probeTemplate = temp1;
                        }
                     }
                     else
                     {
                        accLoc->GetPlaceableProbe()->SetProbeTemplate(temp1);
                        probeTemplate = temp1;
                     }
                  }

                  if (qfeAccLoc->GetPlaceableProbe() != NULL)
                  {
                     if (qfeAccLoc->GetPlaceableProbe()->HasProbeTemplate() && temp2 != NULL)
                     {
                        if (qfeProbeTemplate != NULL && temp2->GetDiameter() < qfeProbeTemplate->GetDiameter())
                           qfeAccLoc->GetPlaceableProbe()->SetProbeTemplate(temp2);
                     }
                     else
                     {
                        qfeAccLoc->GetPlaceableProbe()->SetProbeTemplate(temp2);
                     }
                  }

                  // Case 1496 - The real problem
                  // If violation involves probes that were both already there, i.e. not
                  // generated "for fixture file", then we can't fix it here, and we don't
                  // really care, as far as adding "pilotdrills" goes. Therefore, only
                  // count the violation if at least one of the entities is "for fixture file".
                  if (accLoc->IsForFixtureFile() || qfeAccLoc->IsForFixtureFile())
                     violationCount++;
               } // END if (distance < (probeRadius + qfeRadius)) ...
            } // END while (foundPos) ...

            if (probeTemplate == NULL && accLoc->IsForFixtureFile())
            {
               accLoc->RemoveProbe();
               break;
            }

            //if (qfeProbeTemplate == NULL

         } // END if (probe->HasProbeTemplate()) ...
         else
            accLoc->RemoveProbe();
      } // END while (probePos) ...
   } // END while (netPos) ...

   return violationCount;
}

int CPilotDrillSolution::RunAnalysis()
{
   haveTopProbes = false;
   haveBotProbes = false;

   // make sure there are probe templates to use
   CDFTProbeTemplates &Templates = m_pTestPlan->GetProbes();
   POSITION pos = Templates.GetHeadPosition_TopProbes();
   while (pos && !haveTopProbes)
   {
      CDFTProbeTemplate *pTemplate = Templates.GetNext_TopProbes(pos);
      if (pTemplate->GetUseFlag())
         haveTopProbes = true;
   }

   pos = Templates.GetHeadPosition_BotProbes();
   while (pos && !haveBotProbes)
   {
      CDFTProbeTemplate *pTemplate = Templates.GetNext_BotProbes(pos);
      if (pTemplate->GetUseFlag())
         haveBotProbes = true;
   }

   if (!haveTopProbes && !haveBotProbes)
   {
      m_sErrMsg = "There were no probe templates to use.";
      return -1;
   }

   m_probeNamer.Reset(m_pTestPlan->GetProbeStartNumber(), 1, 1);

   gatherAccesibleLocations();
   calculateNeededProbes();
   gatherAndMatchProbes();
   createAndPlaceProbes();

   // Case 1496, add loop counter to ensure no infinite
   int loopcount = 0;
   while (loopcount < 10 && maximizeFixtureProbeSizes() > 0)
   {
      fixViolatingFixtureProbeTemplates();
      removeFixtureTemplatesFromProbes();
      loopcount++;
   }
   loopcount = 0;
   while (loopcount < 10 && fixViolatingFixtureProbeTemplates() > 0)
   {
      maximizeFixtureProbeSizes();
      loopcount++;
   }

   return 0;
}

void CPilotDrillSolution::WriteDrills(CFormatStdioFile &file, PageUnitsTag outputPageUnit, int outputPrecision, bool top, CTMatrix mat)
{
   POSITION pos = m_ppNets.GetStartPosition();
   while (pos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(pos, netName, ppNet);

      POSITION accLocPos = ppNet->GetHeadPosition_AccessibleLocations();
      while (accLocPos)
      {
         CFixtureAccLoc *accLoc = (CFixtureAccLoc*)ppNet->GetNext_AccessibleLocations(accLocPos);

         if (top && accLoc->GetSurface() == testSurfaceBottom)
            continue;
         if (!top && accLoc->GetSurface() == testSurfaceTop)
            continue;
         if (!accLoc->IsForFixtureFile())
            continue;
         if (!accLoc->IsProbed())
            continue;

         CPPProbe *probe = accLoc->GetPlaceableProbe();

         if (!probe->HasProbeTemplate())
            continue;

         CString refName;

         //if (accLoc->GetFeatureType() == entityTypeData)
         // refName = ((DataStruct*)accLoc->GetFeature())->getInsert()->getRefname();
         //else if (accLoc->GetFeatureType() == entityTypeCompPin)
         // refName = ((CompPinStruct*)accLoc->GetFeature())->getPinRef('-');

         if (accLoc->getFeatureEntity().getEntityType() == entityTypeData)
            refName = accLoc->getFeatureEntity().getData()->getInsert()->getRefname();
         else if (accLoc->getFeatureEntity().getEntityType() == entityTypeCompPin)
            refName = accLoc->getFeatureEntity().getCompPin()->getPinRef('-');

         CPoint2d location = accLoc->GetLocation();
         mat.transform(location);

         file.WriteString("X%.*fY%.*f\t; PILOTDRILL %s %s %s\n",
            outputPrecision, location.x * Units_Factor(m_pDoc->getSettings().getPageUnits(), outputPageUnit),
            outputPrecision, location.y * Units_Factor(m_pDoc->getSettings().getPageUnits(), outputPageUnit),
            probe->GetProbeTemplateName(), refName, netName);
      }
   }
}

/******************************************************************************
* CPilotDrillSolutionMap::CPilotDrillSolutionMap
*/
CPilotDrillSolutionMap::CPilotDrillSolutionMap(CCEtoODBDoc *doc, FileStruct *file)
{
   if (doc == NULL || file == NULL)
      return;

   if (file->getBlockType() == blockTypePanel)
   {
      // create a pilot drill solution for every board in the panel with a board file
      BlockStruct *panelBlock = file->getBlock();
      if (panelBlock == NULL)
         return;

      for (POSITION boardPos=panelBlock->getHeadDataInsertPosition(); boardPos!=NULL; panelBlock->getNextDataInsert(boardPos))
      {
         DataStruct *boardData = panelBlock->getAtData(boardPos);
         InsertStruct *boardInsert = boardData->getInsert();

         if (boardInsert->getInsertType() != insertTypePcb)
            continue;

         FileStruct *boardFile = doc->getFileList().FindByBlockNumber(boardInsert->getBlockNumber());
         if (boardFile == NULL)
            continue;

         /*if (doc->GetCurrentDFTSolution(*boardFile) == NULL)
            continue;*/

         CPilotDrillSolution *drillSolution = NULL;
         if (Lookup(boardFile->getBlock()->getName(), drillSolution))
            continue;

         /*drillSolution = new CPilotDrillSolution(doc, doc->GetCurrentDFTSolution(*boardFile)->GetTestPlan(), boardFile);
         SetAt(boardFile->getBlock()->getName(), drillSolution);*/
      }
   }
   else if (file->getBlockType() == blockTypePcb)
   {
      /*CPilotDrillSolution *drillSolution = new CPilotDrillSolution(doc, doc->GetCurrentDFTSolution(*file)->GetTestPlan(), file);
      SetAt(file->getBlock()->getName(), drillSolution);*/
   }
}



/******************************************************************************
* CFixtureFileWriter
*/
CFixtureFileWriter::CFixtureFileWriter(FileStruct *file, CCEtoODBDoc *doc)
   : m_PilotDrillSolutions(doc, file)
{
   m_pDoc = doc;
   m_pFile = file;
   m_eOutputPageUnit = pageUnitsInches;
   m_iOutputPrecision = 3;
}

CFixtureFileWriter::~CFixtureFileWriter()
{
};

int CFixtureFileWriter::loadSettings(CString filename)
{
   CFileException e;
   CFormatStdioFile file;
   CString line;
   CString tok;
   int tokPos = 0;

   if (!file.Open(filename, CFile::modeRead, &e))
   {
      // no settings file found
      CString tmp;
      char msg[255];
		if (e.m_cause != e.none && e.GetErrorMessage(msg, 255))
         tmp.Format("%s [%s]", msg, filename);
      else
         tmp.Format("File [%s] not found", filename);
      ErrorMessage(tmp,"Fixture File Settings", MB_OK | MB_ICONHAND);

      fprintf(m_logFile, "%s\n", tmp);

      return e.m_cause;
   }

   while (file.ReadString(line))
   {
      tokPos = 0;
      
      tok = line.Tokenize(" \t\n", tokPos);
      if (tok.IsEmpty())  continue;

      if (tok[0] == '.')
      {
         // here do it.
         if (!STRCMPI(tok,".OUTPUTPAGEUNIT"))
         {
            CString pageUnit = line.Tokenize(" \t\n", tokPos);

            if (pageUnit == "INCHES")
               m_eOutputPageUnit = pageUnitsInches;
            else if (pageUnit == "MM")
               m_eOutputPageUnit = pageUnitsMilliMeters;
            else if (pageUnit == "MILS")
               m_eOutputPageUnit = pageUnitsMils;
         }
         else
         if (!STRCMPI(tok,".OUTPUTPRECISION"))
         {
            tok = line.Tokenize(" \t\n", tokPos);
            if (tok.IsEmpty()) continue;
            m_iOutputPrecision = atoi(tok);
         }        
      }
   } // END while (file.ReadString(line)) ...

   file.Close();
   return 0;
}

int CFixtureFileWriter::gatherProbes()
{
   CTMatrix mat;

   if (m_pFile->getBlockType() == blockTypePcb)
   {
      mat.initMatrix();
      for (POSITION pos=m_pFile->getBlock()->getHeadDataInsertPosition(); pos!=NULL; m_pFile->getBlock()->getNextDataInsert(pos))
      {
         DataStruct *data = m_pFile->getBlock()->getAtData(pos);
         InsertStruct *insert = data->getInsert();

         if (insert->getInsertType() != insertTypeTestProbe)
            continue;

         m_probeList.AddTail(new CFixtureProbe(m_pDoc, m_pFile, data, mat, 0));
      }
   }
   else if (m_pFile->getBlockType() == blockTypePanel)
   {
      int boardNumber = 0; // probe number offset calc requires starting this counter at zero
         
      for (POSITION pcbPos=m_pFile->getBlock()->getHeadDataInsertPosition(); pcbPos!=NULL; m_pFile->getBlock()->getNextDataInsert(pcbPos))
      {
         DataStruct *pcbData = m_pFile->getBlock()->getAtData(pcbPos);
         InsertStruct *pcbInsert = pcbData->getInsert();

         FileStruct *pcbFile = m_pDoc->getFileList().FindByBlockNumber(pcbInsert->getBlockNumber());
         if (pcbFile == NULL)
            continue;

         BlockStruct *pcbBlock = pcbFile->getBlock();
         if (pcbBlock == NULL)
            continue;

         mat.initMatrix();
         mat.scale(pcbInsert->getGraphicMirrored()?-pcbInsert->getScale():pcbInsert->getScale(), pcbInsert->getScale());
         mat.rotateRadians(pcbInsert->getAngle());
         mat.translate(pcbInsert->getOrigin2d());
         for (POSITION dataPos=pcbBlock->getHeadDataInsertPosition(); dataPos!=NULL; pcbBlock->getNextDataInsert(dataPos))
         {
            DataStruct *data = pcbBlock->getAtData(dataPos);
            InsertStruct *insert = data->getInsert();

            if (insert->getInsertType() == insertTypeTestProbe)
            {
               CFixtureProbe *probe = new CFixtureProbe(m_pDoc, pcbFile, data, mat, boardNumber);
               m_probeList.AddTail(probe);
            }
         }
         boardNumber++;
      }
   }

   return 0;
}

int CFixtureFileWriter::findHighestProbeNumber()
{
   int highestProbeNumber = 0;

   POSITION pos = m_probeList.GetHeadPosition();
   while (pos)
   {
      CFixtureProbe *probe = m_probeList.GetNext(pos);
      int thisProbeNumber = atoi(probe->GetName());
      if (thisProbeNumber > highestProbeNumber)
         highestProbeNumber = thisProbeNumber;
   }
   return highestProbeNumber;
}

double CFixtureFileWriter::getDrillHoleSize(DataStruct *data)
{
   if (data->getDataType() != dataTypeInsert)
      return 0.0;

   InsertStruct *insert = data->getInsert();
   BlockStruct *block = m_pDoc->getBlockAt(insert->getBlockNumber());
   if (block!=NULL)
	{
		if (block->isDrillHole())		
			return block->getToolSize();

		POSITION subPos = block->getHeadDataInsertPosition();
		while (subPos)
		{
			DataStruct *subData = block->getNextDataInsert(subPos);
			InsertStruct *subInsert = subData->getInsert();
			BlockStruct *subBlock = m_pDoc->getBlockAt(subInsert->getBlockNumber());
			
			if (subInsert->getInsertType() == insertTypePin)
				return getDrillHoleSize(subData);

			if (subBlock !=NULL && subInsert->getInsertType() != insertTypeDrillHole && subInsert->getInsertType() != insertTypeDrillTool && !subBlock->isDrillHole())
				continue;

			return getDrillHoleSize(subData); 		
		}
	}
   return 0.0;
}

int CFixtureFileWriter::writeDrillData(CFormatStdioFile &file, bool top)
{
   CMapStringToPtr uniqueProbes;

   file.WriteString("; Drill Data\n");
   file.WriteString(";\n");
   file.WriteString("; Probe Size\n");
   file.WriteString("; PX\tPY\t;PROBEDRILL Probe_Name\tTarget_name\tNetname\n");

   POSITION pos = m_probeList.GetHeadPosition();
   while (pos)
   {
      CFixtureProbe *probe = m_probeList.GetNext(pos);
      
      //if (top != probe->IsOnTop())
        // continue;
      //if (!probe->IsPlaced())
        // continue;
		if (top == probe->IsOnTop() && probe->IsPlaced())
		{	
			CString id;
			VOID *dummy = NULL;
			id.Format("X%.*fY%.*f", m_iOutputPrecision, probe->GetLocation().x * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit),
				m_iOutputPrecision, probe->GetLocation().y * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit));
			if (uniqueProbes.Lookup(id, dummy))
				continue;

			file.WriteString("X%.*fY%.*f\t; PROBEDRILL %s %s %s\n",
				m_iOutputPrecision, probe->GetLocation().x * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit),
				m_iOutputPrecision, probe->GetLocation().y * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit),
				probe->GetTemplateName(), probe->GetRefName(), probe->GetNetName());

			uniqueProbes.SetAt(id, NULL);
		}
   }

   if (m_pFile->getBlockType() == blockTypePanel)
   {
      for (pos=m_pFile->getBlock()->getHeadDataInsertPosition(); pos!=NULL; m_pFile->getBlock()->getNextDataInsert(pos))
      {
         DataStruct *data = m_pFile->getBlock()->getAtData(pos);
         InsertStruct *insert = data->getInsert();

         if (insert->getInsertType() != insertTypePcb)
            continue;

         CPilotDrillSolution *drillSolution = NULL;
         BlockStruct *block = m_pDoc->getBlockAt(insert->getBlockNumber());
         if (!m_PilotDrillSolutions.Lookup(block->getName(), drillSolution))
            continue;

#ifdef _DEBUG
         file.WriteString("\n; Pilot drills for %s using %s\n", insert->getRefname(), block->getName());
#endif
         CTMatrix mat;
         mat.scale(insert->getGraphicMirrored()?-insert->getScale():insert->getScale(), insert->getScale());
         mat.rotateRadians(insert->getAngle());
         mat.translate(insert->getOrigin2d());
         drillSolution->WriteDrills(file, m_eOutputPageUnit, m_iOutputPrecision, top, mat);
      }
   }
   else if (m_pFile->getBlockType() == blockTypePcb)
   {
      CString pcbBlockName = m_pFile->getBlock()->getName();
      CPilotDrillSolution *drillSolution = NULL;
      if (m_PilotDrillSolutions.Lookup(pcbBlockName, drillSolution))
         drillSolution->WriteDrills(file, m_eOutputPageUnit, m_iOutputPrecision, top);
   }

   return 0;
}

int CFixtureFileWriter::writeRecepticleSize(CFormatStdioFile &file, bool top)
{
   CMapStringToPtr uniqueProbes;
   CUnits curUnit(m_pDoc->getSettings().getPageUnits());

   file.WriteString(";\n");
   file.WriteString("; Receptacle Size\n");
   file.WriteString(";\n");
   file.WriteString("; PX\tPY\tProbe_Size\tExposed_metal_dia\tDrill_dia\tTechnology\tComponent_Via\tProbe_style\n");

   POSITION pos = m_probeList.GetHeadPosition();
   while (pos)
   {
      CFixtureProbe *probe = m_probeList.GetNext(pos);
      
      if (top != probe->IsOnTop())
         continue;
      if (!probe->IsPlaced())
         continue;

      CString id;
      VOID *dummy = NULL;
      id.Format("X%.*fY%.*f",
         m_iOutputPrecision, probe->GetLocation().x * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit),
         m_iOutputPrecision, probe->GetLocation().y * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit));
      if (uniqueProbes.Lookup(id, dummy))
         continue;

      CString probeStyle = probe->GetProbeStyle();
      file.WriteString("%.*f\t%.*f\t%d\t%.*f\t%.*f\t%s\t%s\t%s\n",
         m_iOutputPrecision, probe->GetLocation().x * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit),
         m_iOutputPrecision, probe->GetLocation().y * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit),
         probe->GetSize(),
         m_iOutputPrecision, probe->GetExposedMetalDiameter() * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit),
         m_iOutputPrecision, probe->GetDrillSize() * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit),
         probe->GetTechnology(), probe->GetRefName(), probeStyle);

      uniqueProbes.SetAt(id, NULL);
   }

   return 0;
}

int CFixtureFileWriter::writeTestResource(CFormatStdioFile &file, bool top)
{
   file.WriteString(";\n");
   file.WriteString("; Test Resource Wiring\n");
   file.WriteString(";\n");
   file.WriteString("; Probe_name\tPX\tPY\tTester_Interface_no\tTarget_name\tNetname\n");

   POSITION pos = m_probeList.GetHeadPosition();
   while (pos)
   {
      CFixtureProbe *probe = m_probeList.GetNext(pos);
      
      if (top != probe->IsOnTop())
         continue;
      if (!probe->IsPlaced())
         continue;

      if (probe->GetResourceType() != testResourceTypeTest)
         continue;

       
      file.WriteString("%s\t%.*f\t%.*f\t%s\t%s\t%s\t%s\n",
         probe->GetName(),
         m_iOutputPrecision, probe->GetLocation().x * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit),
         m_iOutputPrecision, probe->GetLocation().y * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit),
         probe->GetTesterInterface(),
         probe->GetRefName(), probe->GetNetName(), probe->GetProbeMessage());
   }

   return 0;
}

int CFixtureFileWriter::writePowerInjection(CFormatStdioFile &file, bool top)
{
   file.WriteString(";\n");
   file.WriteString("; Power Injection Wiring\n");
   file.WriteString(";\n");
   file.WriteString("; Probe_name\tPX\tPY\tTester_Interface_no\tTarget_name\tNetname\n");

   POSITION pos = m_probeList.GetHeadPosition();
   while (pos)
   {
      CFixtureProbe *probe = m_probeList.GetNext(pos);
      
      if (top != probe->IsOnTop())
         continue;
      if (!probe->IsPlaced())
         continue;

      if (probe->GetResourceType() != testResourceTypePowerInjection)
         continue;

      file.WriteString("%s\t%.*f\t%.*f\t%d\t%s\t%s\t%s\n",
         probe->GetName(),
         m_iOutputPrecision, probe->GetLocation().x * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit),
         m_iOutputPrecision, probe->GetLocation().y * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit),
         probe->GetTesterInterface(),
         probe->GetRefName(), probe->GetNetName(), probe->GetProbeMessage());
   }

   return 0;
}

int CFixtureFileWriter::writeToolingHoles(CFormatStdioFile &file, bool top)
{
   int toolCount = 0;

   file.WriteString(";\n");
   file.WriteString("; Tooling Holes\n");
   file.WriteString(";\n");
   file.WriteString("; Hole_name\tHX\tHY\tHole_Dia\n");

   BlockStruct *fileBlock = m_pFile->getBlock();
   POSITION pos = fileBlock->getHeadDataInsertPosition();
   while (pos)
   {
      DataStruct *holeData = fileBlock->getNextDataInsert(pos);
      InsertStruct *holeInsert = holeData->getInsert();
      
      if (holeInsert->getInsertType() != insertTypeDrillHole && holeInsert->getInsertType() != insertTypeDrillTool)
         continue;

      CString holeName = holeInsert->getRefname();
      if (holeName.IsEmpty())
         holeName.Format("TOOL_%d", toolCount++);

      double toolSize = getDrillHoleSize(holeData) * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit);
      double xLoc = holeInsert->getOriginX() * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit);
      double yLoc = holeInsert->getOriginY() * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit);
      file.WriteString("%s\t%.*f\t%.*f\t%.*f\n",
         holeName,
         m_iOutputPrecision, xLoc,
         m_iOutputPrecision, yLoc,
         m_iOutputPrecision, toolSize);
   }

   return 0;
}

int CFixtureFileWriter::writeCapcitiveOpenComps(CFormatStdioFile &file, bool top)
{
   file.WriteString(";\n");
   file.WriteString("; Capacitive Opens\n");
   file.WriteString(";\n");
   file.WriteString("; RefDes\tCX\tCY\tHeight\tLength\n");

   BlockStruct *fileBlock = m_pFile->getBlock();
   POSITION pos = fileBlock->getHeadDataInsertPosition();
   while (pos)
   {
      DataStruct *insData = fileBlock->getNextDataInsert(pos);
      InsertStruct *insInsert = insData->getInsert();
      
      if (insInsert->getInsertType() != insertTypePcbComponent)
         continue;

      if (insInsert->getInsertType() != insertTypePcbComponent)
			continue;
      if (top != !insInsert->getPlacedBottom())
			continue;
					
      Attrib *attrib = NULL;
      WORD capOpenKW = m_pDoc->RegisterKeyWord(ATT_CAP_OPENS, 0, valueTypeString);
      if (!insData->lookUpAttrib(capOpenKW, attrib) || (CString)get_attvalue_string(m_pDoc, attrib) == "False")
         continue;

      BlockStruct *insertBlock = m_pDoc->getBlockAt(insInsert->getBlockNumber());	
      if (insertBlock != NULL)
      {      
         DataStruct *centroid = insertBlock->GetCentroidData();
		   CExtent extent = insertBlock->getExtent();

         CPoint2d center(extent.getCenter());		
         if (centroid != NULL)
            center = centroid->getInsert()->getOrigin2d();

         insInsert->getTMatrix().transform(center);

         file.WriteString("%s\t%.*f\t%.*f\t%.*f\t%.*f\n",
            insInsert->getRefname(),
            m_iOutputPrecision, center.x * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit),
            m_iOutputPrecision, center.y * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit),
            m_iOutputPrecision, extent.getYsize() * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit),
            m_iOutputPrecision, extent.getXsize() * Units_Factor(m_pDoc->getSettings().getPageUnits(), m_eOutputPageUnit));
      }
   }

   return 0;
}

int CFixtureFileWriter::dumpToFile(CString filename)
{
   CFileException e;
   CFormatStdioFile file;

   if (!file.Open(filename, CFile::modeCreate|CFile::modeWrite, &e))
      return -1;

   file.WriteString("Filename: %s\n", filename);
   CTime t;
   t = t.GetCurrentTime();
   file.WriteString("%s\n", t.Format("Created date :%A, %B %d, %Y at %H:%M:%S"));
   file.WriteString("\n");

   POSITION pos = m_probeList.GetHeadPosition();
   while (pos)
   {
      CFixtureProbe *fProbe = m_probeList.GetNext(pos);

      file.WriteString("--------------------------------------\n");
      file.WriteString("ProbeName : %s\n", fProbe->GetName());
      file.WriteString("   Placed : %s\n", fProbe->IsPlaced()?"True":"False");
      file.WriteString("   NetName : %s\n", fProbe->GetNetName());
      file.WriteString("   Surface : %s\n", fProbe->IsOnTop()?"Top":"Bottom");
      file.WriteString("   xLocation : %.3f\n", fProbe->GetLocation().x);
      file.WriteString("   yLocation : %.3f\n", fProbe->GetLocation().y);
      file.WriteString("   RefName : %s\n", fProbe->GetRefName());
      file.WriteString("   Resource Type : %s\n", fProbe->GetResourceTypeString());
      file.WriteString("   Size : %.3f\n", fProbe->GetSize());
      file.WriteString("   Drill Size : %.3f\n", fProbe->GetDrillSize());
      file.WriteString("   Technology : %s\n", fProbe->GetTechnology());
      file.WriteString("   Exposed Metal : %.3f\n", fProbe->GetExposedMetalDiameter());
   }

   pos = m_PilotDrillSolutions.GetStartPosition();
   while (pos)
   {
      CString pcbBlockName;
      CPilotDrillSolution *drillSolution = NULL;
      m_PilotDrillSolutions.GetNextAssoc(pos, pcbBlockName, drillSolution);

      file.WriteString("--------------------------------------\n");
      file.WriteString("Pilot Drill Solution for %s\n", pcbBlockName);
      drillSolution->DumpToFile(file, 3);
   }

   file.Close();

   return 0;
}

int CFixtureFileWriter::writeFile(CString filename, bool top)
{
   CFileException e;
   CFormatStdioFile file;

   if (!file.Open(filename, CFile::modeCreate|CFile::modeWrite, &e))
   {
      // no settings file found
      CString tmp;
      char msg[255];

		if (e.m_cause != e.none && e.GetErrorMessage(msg, 255))
         tmp.Format("%s [%s]", msg, filename);
      else
			tmp.Format("Could not open file for writing: [%s]", filename);
      ErrorMessage(tmp, "Fixture File Write", MB_OK | MB_ICONHAND);

      fprintf(m_logFile, "%s\n", tmp);

      return e.m_cause;
   }

   file.WriteString("; Filename: %s\n", filename);
   CTime t;
   t = t.GetCurrentTime();
   file.WriteString("; %s\n", t.Format("Created date :%A, %B %d, %Y at %H:%M:%S"));
   file.WriteString("\n");
   file.WriteString("\n");

   writeDrillData(file, top);
   file.WriteString("\n");

   writeRecepticleSize(file, top);
   file.WriteString("\n");

   writeTestResource(file, top);
   file.WriteString("\n");

   writePowerInjection(file, top);
   file.WriteString("\n");

   writeToolingHoles(file, top);
   file.WriteString("\n");

   writeCapcitiveOpenComps(file, top);

   file.Close();

   return 0;
}

void CFixtureFileWriter::applyProbeNumberOffset(int probeNumberOffset)
{
   POSITION pos = m_probeList.GetHeadPosition();
   while (pos)
   {
      CFixtureProbe *probe = m_probeList.GetNext(pos);
      if (probe != NULL)
      {
         int originalProbeNumber = atoi(probe->GetName());
         int newProbeNumber = originalProbeNumber + (probeNumberOffset * probe->GetBoardNumber());
         CString probeNumberStr;
         probeNumberStr.Format("%d", newProbeNumber);
         probe->SetName(probeNumberStr);
      }
   }
}

int CFixtureFileWriter::WriteFiles(CString filepath)
{
   if (m_pDoc == NULL || m_pFile == NULL)
      return -1;

   bool display_error = false;
   CString line;
   CString tok;
   int tokPos = 0;
   CTime t;

   // Open log file.
   CString localLogFilename;
   m_logFile = getApp().OpenOperationLogFile("fixture.log", localLogFilename);
   if (m_logFile == NULL) // error message already issued, just return.
      return -1;
   WriteStandardExportLogHeader(m_logFile, "Fixture File");

   CString settingsFile( getApp().getExportSettingsFilePath("fixture.out") );
   {
      CString msg;
      msg.Format("Fixture File: Settings file [%s].\n", settingsFile);
      getApp().LogMessage(msg);
   }
   loadSettings(settingsFile);

   gatherProbes();

   // Probe number offset, only for panel mode fixture output
   if (m_pFile->getBlockType() == blockTypePanel)
   {
      int probeNumberOffset = findHighestProbeNumber();
      CFixtureOutProbeOffsetDlg dlg(probeNumberOffset);
      if (dlg.DoModal() != IDOK)
         return 0;
      probeNumberOffset = dlg.getProbeNumberOffset();
      applyProbeNumberOffset(probeNumberOffset);
   }

   POSITION pos=m_PilotDrillSolutions.GetStartPosition();
   while (pos)
   {
      CString pcbBlockName;
      CPilotDrillSolution *drillSolution = NULL;
      m_PilotDrillSolutions.GetNextAssoc(pos, pcbBlockName, drillSolution);

      drillSolution->RunAnalysis();
   }
   dumpToFile(filepath + "\\fixture.dbg");

	CFilePath outfile(filepath);
	CFilePath cczfile(m_pFile->getName());

	outfile.setBaseFileName(cczfile.getBaseFileName());

	outfile.setExtension("top");
	writeFile(outfile.getPath(), true);

	outfile.setExtension("bot");
	writeFile(outfile.getPath(), false);

   t = t.GetCurrentTime();
   fprintf(m_logFile, "%s\n", t.Format("Done :%A, %B %d, %Y at %H:%M:%S"));
   fprintf(m_logFile, "Logfile closed\n");

   // Close log file only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!localLogFilename.IsEmpty())
   {
      fclose(m_logFile);

      // We want this to happen only for local log files, not when
      // system log file is in play.
      if (display_error)
         Logreader(localLogFilename);
   }

   return 0;
}


// CFixtureOutProbeOffsetDlg dialog

IMPLEMENT_DYNAMIC(CFixtureOutProbeOffsetDlg, CDialog)
CFixtureOutProbeOffsetDlg::CFixtureOutProbeOffsetDlg(int defaultOffset, CWnd* pParent /*=NULL*/)
   : CDialog(CFixtureOutProbeOffsetDlg::IDD, pParent)
   , m_probeNumberOffset(defaultOffset)
   , m_minimumOffset(defaultOffset)
{
}

CFixtureOutProbeOffsetDlg::~CFixtureOutProbeOffsetDlg()
{
}

void CFixtureOutProbeOffsetDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_PROBE_OFFSET, m_probeNumberOffset);
   DDV_MinMaxInt(pDX, m_probeNumberOffset, m_minimumOffset, 9000000);
}


BEGIN_MESSAGE_MAP(CFixtureOutProbeOffsetDlg, CDialog)
END_MESSAGE_MAP()


// CFixtureOutProbeOffsetDlg message handlers
