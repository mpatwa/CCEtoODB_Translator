// $Header: /CAMCAD/DcaLib/DcaProbePlacement.cpp 4     6/29/07 12:22p Lynn Phung $

#include "StdAfx.h"
#include "DcaProbePlacement.h"

//_____________________________________________________________________________
CPPAccessibleLocation::CPPAccessibleLocation(CCamCadData& camCadData, DataStruct *testAccessData)
: m_pDoc(NULL)
//, m_eFeatureType(entityTypeUndefined)
//, m_pFeature(NULL)
, m_pTA(NULL)
, m_ptLocation(0, 0)
, m_eSurface(testSurfaceUnset)
, m_eFeatureCondition(featureConditionProbeable)
, m_pProbe(NULL)
, m_pDoubleWiredProbe(NULL)
, m_distToClosestPart(0.0)
{
   m_pDoc = doc;
   m_pTA = testAccessData;
   //m_bPlaced = true;

   InsertStruct *taInsert = testAccessData->getInsert();

   // get the location of the access point
   m_ptLocation = taInsert->getOrigin2d();

   // get the feature and feature type
   Attrib *dataLinkAttrib = NULL;
   long featureEntityNumber = 0;
   WORD dataLinkKw = m_pDoc->RegisterKeyWord(ATT_DDLINK, 0, valueTypeInteger);
   if (testAccessData->getAttributes() && testAccessData->getAttributes()->Lookup(dataLinkKw, dataLinkAttrib))
   {
      //EEntityType entityType;

      featureEntityNumber = dataLinkAttrib->getIntValue();
      m_featureEntity = CEntity::findEntity(*m_pDoc,featureEntityNumber);
      //m_pFeature = FindEntity(m_pDoc, featureEntityNumber, entityType);
      //m_eFeatureType = entityType;
   }

   // get the target type attribute
   Attrib *targetAttrib = NULL;
   WORD targetTypeKw = m_pDoc->RegisterKeyWord(ATT_DFT_TARGETTYPE, 0, valueTypeString);
   if (testAccessData->getAttributes() && testAccessData->getAttributes()->Lookup(targetTypeKw, targetAttrib))
      m_sTargetType = get_attvalue_string(m_pDoc, targetAttrib);

   // get the target type attribute
   Attrib *netNameAttrib = NULL;
   WORD netNameKw = m_pDoc->RegisterKeyWord(ATT_NETNAME, 0, valueTypeString);
   if (testAccessData->getAttributes() && testAccessData->getAttributes()->Lookup(netNameKw, netNameAttrib))
      m_sNetName = get_attvalue_string(m_pDoc, netNameAttrib);

   // get the surface of the feature
   bool onTop = false;
   if (!taInsert->getLayerMirrored())
      m_eSurface = testSurfaceTop;
   else
      m_eSurface = testSurfaceBottom;
}

CPPAccessibleLocation::CPPAccessibleLocation(CCamCadData& camCadData,const CEntity& featureEntity, CPoint2d location, CString targetType, CString netName, ETestSurface surface)
{
   m_pDoc = &camCadDoc;
   //m_eFeatureType = featureType;
   //m_pFeature = feature;
   m_featureEntity = featureEntity;
   m_ptLocation = location;
   m_sTargetType = targetType;
   m_sNetName = netName;
   m_eSurface = surface;
   m_pTA = NULL;
   m_distToClosestPart = 0.0;

   m_eFeatureCondition = featureConditionProbeable;
   m_pProbe = NULL;
   m_pDoubleWiredProbe = NULL;

   //m_bPlaced = false;
}

CPPAccessibleLocation::CPPAccessibleLocation(CPPAccessibleLocation &other)
{
   *this = other;
}

CPPAccessibleLocation::~CPPAccessibleLocation()
{
}

CPPAccessibleLocation& CPPAccessibleLocation::operator=(const CPPAccessibleLocation &other)
{
   if (&other != this)
   {
      m_pDoc              = other.m_pDoc;
    //m_eFeatureType      = other.m_eFeatureType;
    //m_pFeature          = other.m_pFeature;
      m_featureEntity     = other.m_featureEntity;
    //m_bPlaced           = other.m_bPlaced;
      m_pTA               = other.m_pTA;
    //m_FeatureExtents    = other.m_FeatureExtents;
      m_ptLocation        = other.m_ptLocation;
      m_sTargetType       = other.m_sTargetType;
      m_sNetName          = other.m_sNetName;
      m_eSurface          = other.m_eSurface;
      m_distToClosestPart = other.m_distToClosestPart;

      m_eFeatureCondition = other.m_eFeatureCondition;
      m_pProbe            = other.m_pProbe;
      m_pDoubleWiredProbe = other.m_pDoubleWiredProbe;
   }

   return *this;
}

void CPPAccessibleLocation::SetFeatureCondition(EFeatureCondition condition)
{
   if (condition < featureConditionMIN && condition >= featureConditionMAX)
      return;

   m_eFeatureCondition = condition;
}

void CPPAccessibleLocation::PlaceProbe(CPPProbe *probe)
{
   m_pProbe = probe;
   probe->SetAccessibleLocationPtr(this);
}

CPPProbe *CPPAccessibleLocation::RemoveProbe()
{
   CPPProbe *probe = m_pProbe;

   // Case 1997, crashed, not always a probe on an accessible location
   if (probe != NULL)
      probe->RemoveAccessibleLocationPtr();

   m_pProbe = NULL;

   return probe;
}

void CPPAccessibleLocation::PlaceDoubleWiredProbe(CPPProbe *probe)
{
   m_pDoubleWiredProbe = probe;
   probe->SetAccessibleLocationPtr(this);
}

CPPProbe *CPPAccessibleLocation::RemoveDoubleWiredProbe()
{
   CPPProbe *probe;
   probe = m_pDoubleWiredProbe;
   probe->RemoveAccessibleLocationPtr();

   m_pDoubleWiredProbe = NULL;

   return probe;
}

void CPPAccessibleLocation::DumpToFile(CFormatStdioFile &file, int indent)
{
   CString accLoc;
   CString featureType("Undefined");

   //if (m_eFeatureType == entityTypeCompPin)
   if (m_featureEntity.getEntityType() == entityTypeCompPin)
   {
      //CompPinStruct *cp = (CompPinStruct*)m_pFeature;
      CompPinStruct* cp = m_featureEntity.getCompPin();
      accLoc.Format("CompPin %s [%d] at 0x%08x", cp->getPinRef(), cp->getEntityNumber(), this);
      featureType = "CompPin";
   }
   //else if (m_eFeatureType == entityTypeData)
   else if (m_featureEntity.getEntityType() == entityTypeData)
   {
      //DataStruct *data = (DataStruct*)m_pFeature;
      DataStruct* data = m_featureEntity.getData();
      accLoc.Format("Via %s [%d] at 0x%08x", data->getInsert()->getRefname(), data->getEntityNumber(), this);
      featureType = "Data (Via)";
   }

   file.WriteString("%*s::Accessable Location on %s::\n", indent, " ", accLoc);

   indent += 3;

   CString buf;
   //file.WriteString("%*sm_eFeatureType = %s\n", indent, " ", (m_eFeatureType==entityTypeCompPin)?"CompPin":"Data (Via)");
   file.WriteString("%*sm_eFeatureType = %s\n", indent, " ",featureType);
   file.WriteString("%*sm_pTA = %s\n", indent, " ", (m_pTA==NULL)?"None":m_pTA->getInsert()->getRefname());
   file.WriteString("%*sm_ptLocation = (%0.3f, %0.3f)\n", indent, " ", m_ptLocation.x, m_ptLocation.y);
   file.WriteString("%*sm_sTargetType = %s\n", indent, " ", m_sTargetType);
   file.WriteString("%*sm_sNetName = %s\n", indent, " ", m_sNetName);
   file.WriteString("%*sm_eSurface = %s\n", indent, " ", (m_eSurface==testSurfaceTop)?"Top":"Bottom");

   switch (m_eFeatureCondition)
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
   file.WriteString("%*sm_pProbe = 0x%08x\n", indent, " ", m_pProbe);
   file.WriteString("%*sm_pDoubleWiredProbe = 0x%08x\n", indent, " ", m_pDoubleWiredProbe);

   indent -= 3;
}

//_____________________________________________________________________________
CPPProbe::CPPProbe(CCamCadData& camCadData, ETestResourceType resType, CString netName)
   : m_pDoc(NULL)
   , m_pAccessibleLocation(NULL)
   //, m_lProbeNumber(-1)
   , m_bPlaced(false)
   //, m_pProbe(NULL)
   , m_sProbeTemplateName("")
   , m_bExisting(false)
{
   m_pDoc = doc;
   m_eResourceType = resType;
   m_sNetName = netName;
}

CPPProbe::CPPProbe(CPPProbe &probe)
{
   m_pDoc = probe.m_pDoc;
   m_eResourceType = probe.m_eResourceType;
   m_sNetName = probe.m_sNetName;
   m_pAccessibleLocation = probe.m_pAccessibleLocation;
   m_sProbeRefname = probe.m_sProbeRefname;
   m_bPlaced = probe.m_bPlaced;
   m_sProbeTemplateName = probe.m_sProbeTemplateName;
   //m_pProbe = probe.m_pProbe;
   m_bExisting = probe.m_bExisting;
}

CPPProbe::~CPPProbe()
{
}

CPPProbe& CPPProbe::operator=(const CPPProbe &probe)
{
   if (&probe != this)
   {
      m_pDoc = probe.m_pDoc;
      m_eResourceType = probe.m_eResourceType;
      m_sNetName = probe.m_sNetName;
      m_pAccessibleLocation = probe.m_pAccessibleLocation;
      m_sProbeRefname = probe.m_sProbeRefname;
      m_bPlaced = probe.m_bPlaced;
      m_sProbeTemplateName = probe.m_sProbeTemplateName;
      //m_pProbe = probe.m_pProbe;
      m_bExisting = probe.m_bExisting;
   }

   return *this;
}

void CPPProbe::SetAccessibleLocationPtr(CPPAccessibleLocation *aLocation)
{
   m_pAccessibleLocation = aLocation;
   m_bPlaced = true;
}

CPPAccessibleLocation *CPPProbe::RemoveAccessibleLocationPtr()
{
   CPPAccessibleLocation *accLoc = m_pAccessibleLocation;

   m_bPlaced = false;
   m_pAccessibleLocation = NULL;
   
   return accLoc;
}

void CPPProbe::SetProbeTemplate(CDFTProbeTemplate *probeTemplate)
{
   if (probeTemplate == NULL)
      m_sProbeTemplateName.Empty();
   else
      m_sProbeTemplateName = probeTemplate->GetName();
}

DataStruct *CPPProbe::AddProbeUnplacedToBoard(FileStruct *file, CTestPlan &testPlan)
{
   CDFTProbeTemplate *pTemplate = testPlan.GetProbes().GetTail_TopProbes();

   WORD testResKW = m_pDoc->RegisterKeyWord(ATT_TESTRESOURCE, 0, VT_STRING);
   WORD probePlacementKW = m_pDoc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, VT_STRING);
   WORD netNameKW = m_pDoc->IsKeyWord(ATT_NETNAME, 0);
   WORD refnameKW = m_pDoc->RegisterKeyWord(ATT_REFNAME, 0, VT_STRING);

   double diameter = pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
   double drillsize = pTemplate->GetDrillSize() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
   double textsize = pTemplate->GetTextSize() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
   BlockStruct *probeBlock = CreateTestProbeGeometry(m_pDoc, pTemplate->GetName(), diameter, pTemplate->GetName(), drillsize);

   Graph_Block_On(file->getBlock());

   int mirror = 0;   // no mirroring

   //*rcf CString probeNumber;
   //*rcf probeNumber.Format("%d", m_lProbeNumber);
   DataStruct *probe = Graph_Block_Reference(probeBlock->getName(), this->GetProbeRefname(), -1, 0.0, 0.0, 0.0, mirror, 1.0, -1, 0);
   probe->getInsert()->setInsertType(insertTypeTestProbe);

   Graph_Block_Off();

   CString testResourceType;
   switch (m_eResourceType)
   {
   case testResourceTypeTest:
      testResourceType = "Test";
      probe->setAttrib(m_pDoc, testResKW, VT_STRING, testResourceType.GetBuffer(0), SA_OVERWRITE, NULL);
      probe->setAttrib(m_pDoc, probePlacementKW, VT_STRING, "Unplaced", SA_OVERWRITE, NULL);
      break;
   case testResourceTypePowerInjection:
      testResourceType = "Power Injection";
      probe->setAttrib(m_pDoc, testResKW, VT_STRING, testResourceType.GetBuffer(0), SA_OVERWRITE, NULL);
      probe->setAttrib(m_pDoc, probePlacementKW, VT_STRING, "Unplaced", SA_OVERWRITE, NULL);
      {
         CString probeName;
         POSITION pos = NULL;
         CDFTProbeableNet *pNet = testPlan.GetNetConditions().Find_ProbeableNet(m_sNetName, pos);
         switch (pNet->GetNetType())
         {
         case probeableNetTypePower:
            probeName = (CString)"P" + probe->getInsert()->getRefname();
            probe->getInsert()->setRefname(strdup(probeName));
            break;
         case probeableNetTypeGround:
            probeName = (CString)"G" + probe->getInsert()->getRefname();
            probe->getInsert()->setRefname(strdup(probeName));
            break;
         }
      }
      break;
   }
   probe->setAttrib(m_pDoc, netNameKW, VT_STRING, m_sNetName.GetBuffer(0), SA_OVERWRITE, NULL);

   CreateTestProbeRefnameAttr(m_pDoc, probe, probe->getInsert()->getRefname(), drillsize, textsize, testPlan.GetNeverMirrorRefname());

   probe->setHidden(true);

   return probe;
}

void CPPProbe::DumpToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*s::Probe on %s at 0x%08x::\n", indent, " ", m_sNetName, this);

   indent += 3;
   file.WriteString("%*sm_eResourceType = %s Resource\n", indent, " ", (m_eResourceType==testResourceTypeTest)?"Test":"Power Injection");
   file.WriteString("%*sm_sNetName = %s\n", indent, " ", m_sNetName);
   file.WriteString("%*sm_pAccessibleLocation = 0x%08x\n", indent, " ", m_pAccessibleLocation);
   file.WriteString("%*sm_lProbeNumber = %s\n", indent, " ", m_sProbeRefname);
   file.WriteString("%*sm_bPlaced = %s\n", indent, " ", m_bPlaced?"True":"False");
   file.WriteString("%*sm_pProbe = %s\n", indent, " ", m_sProbeTemplateName.IsEmpty()?"Null":m_sProbeTemplateName);
   file.WriteString("%*sm_bExisting = %s\n", indent, " ", m_bExisting?"True":"False");
   indent -= 3;
}

void CPPProbe::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<Probe Name=\"%s\"", this->GetProbeRefname());
   switch (m_eResourceType)
   {
   case testResourceTypeTest:
      writeFormat.writef(" ResType=\"Test\"");
      break;
   case testResourceTypePowerInjection:
      writeFormat.writef(" ResType=\"Power Injection\"");
      break;
   }
   writeFormat.writef(" Placed=\"%s\"", m_bPlaced?"True":"False");

   if (m_bPlaced && !m_sProbeTemplateName.IsEmpty())
   {
      writeFormat.writef(" Diameter=\"%s\"", m_sProbeTemplateName);
      writeFormat.writef(" Type=\"%s\"", "Unknown");
      writeFormat.writef(" TipSize=\"%s\"", "Unknown");
      writeFormat.writef(" AccessName=\"%s\"", SwapSpecialCharacters(m_pAccessibleLocation->GetTAInsert()->getInsert()->getRefname()));
      writeFormat.writef(" Forced=\"%s\"", (m_pAccessibleLocation->GetFeatureCondition() == featureConditionForced)?"True":"False");

      if (m_pAccessibleLocation->GetFeatureCondition() == featureConditionForced)
      {
         writeFormat.writef(" ForcedFeature=\"%s\"", SwapSpecialCharacters(m_pAccessibleLocation->getFeatureEntity().getCompPin()->getPinRef()));
      }
   }

   writeFormat.writef("/>\n");
}

//_____________________________________________________________________________
void CPPProbeList::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   POSITION pos = GetHeadPosition();
   while (pos)
   {
      CPPProbe *probe = GetNext(pos);

      probe->WriteXML(writeFormat, progress);
   }
}

//_____________________________________________________________________________
CPPNet::CPPNet(CCamCadData& camCadData, CString netName)
{
   m_pDoc = doc;
   m_sNetName = netName;
   m_bNoProbe = false;
   m_eNetType = probeableNetTypeSignal;
   m_iTestResourcesNeeded = 0;
   m_iPwrInjResourcesNeeded = 0;
   m_pNet = NULL;
}

CPPNet::CPPNet(CPPNet &ppNet)
{
   m_pDoc = ppNet.m_pDoc;
   m_sNetName = ppNet.m_sNetName;
   m_bNoProbe = ppNet.m_bNoProbe;
   m_iTestResourcesNeeded = ppNet.m_iTestResourcesNeeded;
   m_iPwrInjResourcesNeeded = ppNet.m_iPwrInjResourcesNeeded;
   m_pNet = ppNet.m_pNet;

   POSITION pos = ppNet.m_accessibleLocations.GetHeadPosition();
   while (pos)
   {
      CPPAccessibleLocation *accessibleLocation = new CPPAccessibleLocation(*ppNet.m_accessibleLocations.GetNext(pos));
      m_accessibleLocations.AddTail(accessibleLocation);
   }

   pos = ppNet.m_probes.GetHeadPosition();
   while (pos)
   {
      CPPProbe *probe = new CPPProbe(*ppNet.m_probes.GetNext(pos));
      m_probes.AddTail(probe);
   }
}

CPPNet::~CPPNet()
{
   RemoveAll_Probes();
   RemoveAll_AccessibleLocations();
}

CPPNet& CPPNet::operator=(const CPPNet &ppNet)
{
   if (&ppNet == this)
   {
      m_pDoc = ppNet.m_pDoc;
      m_sNetName = ppNet.m_sNetName;
      m_bNoProbe = ppNet.m_bNoProbe;
      m_iTestResourcesNeeded = ppNet.m_iTestResourcesNeeded;
      m_iPwrInjResourcesNeeded = ppNet.m_iPwrInjResourcesNeeded;
      m_pNet = ppNet.m_pNet;

      POSITION pos = ppNet.m_accessibleLocations.GetHeadPosition();
      while (pos)
      {
         CPPAccessibleLocation *accessibleLocation = new CPPAccessibleLocation(*ppNet.m_accessibleLocations.GetNext(pos));
         m_accessibleLocations.AddTail(accessibleLocation);
      }

      pos = ppNet.m_probes.GetHeadPosition();
      while (pos)
      {
         CPPProbe *probe = new CPPProbe(*ppNet.m_probes.GetNext(pos));
         m_probes.AddTail(probe);
      }
   }

   return *this;
}

POSITION CPPNet::GetHeadPosition_AccessibleLocations(EEntityType featureType, CString targetType, CString netName, ETestSurface surface) const
{
   POSITION pos;

   for (pos = m_accessibleLocations.GetHeadPosition();pos != NULL;)
   {
      CPPAccessibleLocation* pAccLoc = m_accessibleLocations.GetAt(pos);

      if ((featureType < entityTypeMIN || pAccLoc->getFeatureEntity().getEntityType() == featureType) &&
         (targetType.IsEmpty() || pAccLoc->GetTargetType().Find(targetType) > 0) &&
         (netName.IsEmpty() || pAccLoc->GetNetName() == netName) &&
         (surface < testSurfaceMIN || pAccLoc->GetSurface() == surface))
         break;

      m_accessibleLocations.GetNext(pos);
   }

   return pos;
}

POSITION CPPNet::GetTailPosition_AccessibleLocations(EEntityType featureType, CString targetType, CString netName, ETestSurface surface) const
{
   POSITION pos;

   for (pos = m_accessibleLocations.GetTailPosition();pos != NULL;)
   {
      CPPAccessibleLocation* pAccLoc = m_accessibleLocations.GetAt(pos);

      if ((featureType < entityTypeMIN || pAccLoc->getFeatureEntity().getEntityType() == featureType) &&
         (targetType.IsEmpty() || pAccLoc->GetTargetType().Find(targetType) > 0) &&
         (netName.IsEmpty() || pAccLoc->GetNetName() == netName) &&
         (surface < testSurfaceMIN || pAccLoc->GetSurface() == surface))
         break;

      m_accessibleLocations.GetPrev(pos);
   }

   return pos;
}

CPPAccessibleLocation *CPPNet::GetNext_AccessibleLocations(POSITION &pos, EEntityType featureType, CString targetType, CString netName, ETestSurface surface) const
{
   CPPAccessibleLocation *pAccLoc = m_accessibleLocations.GetNext(pos);

   while (pos)
   {
      CPPAccessibleLocation *pNextAccLoc = m_accessibleLocations.GetAt(pos);

      if ((featureType < entityTypeMIN || pNextAccLoc->getFeatureEntity().getEntityType() == featureType) &&
         (targetType.IsEmpty() || pNextAccLoc->GetTargetType().Find(targetType) > 0) &&
         (netName.IsEmpty() || pNextAccLoc->GetNetName() == netName) &&
         (surface < testSurfaceMIN || pNextAccLoc->GetSurface() == surface))
         break;

      m_accessibleLocations.GetNext(pos);
   }

   return pAccLoc;
}

int CPPNet::GetCount_TestProbes() const
{
   int probeCount = 0;
   for (POSITION pos=m_probes.GetHeadPosition(); pos!=NULL; m_probes.GetNext(pos))
   {
      CPPProbe *probe = m_probes.GetAt(pos);
      
      if (probe->GetResourceType() == testResourceTypeTest)
         probeCount++;
   }

   return probeCount;
}

int CPPNet::GetCount_PowerInjectionProbes() const
{
   int probeCount = 0;
   for (POSITION pos=m_probes.GetHeadPosition(); pos!=NULL; m_probes.GetNext(pos))
   {
      CPPProbe *probe = m_probes.GetAt(pos);
      
      if (probe->GetResourceType() == testResourceTypePowerInjection)
         probeCount++;
   }

   return probeCount;
}

bool CPPNet::HasUnplacedProbe()
{
   for (POSITION pos=m_probes.GetHeadPosition(); pos!=NULL; m_probes.GetNext(pos))
   {
      CPPProbe *probe = m_probes.GetAt(pos);
      if (!probe->IsPlaced())
         return true;      // found an unplaced probe
   }

   // Did not find any unplaced probes
   return false;
}

CPPProbe *CPPNet::AddHead_Probes(ETestResourceType resType, CString netName, CString probeRefname)
{
   CPPProbe *probe = new CPPProbe(m_pDoc, resType, netName);
   probe->SetProbeRefname(probeRefname);
   m_probes.AddHead(probe);

   return probe;
}

CPPProbe *CPPNet::AddTail_Probes(ETestResourceType resType, CString netName, CString probeRefname)
{
   CPPProbe *probe = new CPPProbe(m_pDoc, resType, netName);
   probe->SetProbeRefname(probeRefname);
   m_probes.AddTail(probe);

   return probe;
}

void CPPNet::RemoveAt_Probes(POSITION pos)
{
   CPPProbe *probe = m_probes.GetAt(pos);
   
   m_probes.RemoveAt(pos);
   delete probe;
}

void CPPNet::RemoveAll_Probes()
{
   POSITION pos = m_probes.GetHeadPosition();
   while (pos)
   {
      CPPProbe *probe = m_probes.GetNext(pos);
      delete probe;
   }
   m_probes.RemoveAll();
}

CPPAccessibleLocation *CPPNet::AddHead_AccessibleLocations(DataStruct *testAccessData)
{
   CPPAccessibleLocation *accessibleLocation = new CPPAccessibleLocation(m_pDoc, testAccessData);
   m_accessibleLocations.AddHead(accessibleLocation);

   return accessibleLocation;
}

CPPAccessibleLocation *CPPNet::AddTail_AccessibleLocations(DataStruct *testAccessData)
{
   CPPAccessibleLocation *accessibleLocation = new CPPAccessibleLocation(m_pDoc, testAccessData);
   m_accessibleLocations.AddTail(accessibleLocation);
   return accessibleLocation;
}

CPPAccessibleLocation *CPPNet::AddTail_AccessibleLocations(const CEntity& entity, CPoint2d location, CString targetType, CString netName, ETestSurface surface)
{
   CPPAccessibleLocation *accessibleLocation = new CPPAccessibleLocation(*m_pDoc, entity, location, targetType, netName, surface);
   m_accessibleLocations.AddTail(accessibleLocation);

   return accessibleLocation;
}

void CPPNet::RemoveAt_AccessibleLocations(POSITION pos)
{
   CPPAccessibleLocation *accessibleLocation = m_accessibleLocations.GetAt(pos);
   
   m_accessibleLocations.RemoveAt(pos);
   delete accessibleLocation;

   // TODO: m_LocationTree.remove( ... (???)
}

void CPPNet::RemoveAll_AccessibleLocations()
{
   POSITION pos = m_accessibleLocations.GetHeadPosition();
   while (pos)
   {
      CPPAccessibleLocation *accessibleLocation = m_accessibleLocations.GetNext(pos);
      delete accessibleLocation;
   }
   m_accessibleLocations.RemoveAll();
}

void CPPNet::DumpToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*s::Net %s::\n", indent, " ", m_sNetName);
   indent += 3;

   file.WriteString("%*s::Probes::\n", indent, " ");
   indent += 3;
   POSITION pos = m_probes.GetHeadPosition();
   while (pos)
   {
      CPPProbe *probe = m_probes.GetNext(pos);
      probe->DumpToFile(file, indent);
   }
   indent -= 3;

   file.WriteString("%*s::Accessable Locations::\n", indent, " ");
   indent += 3;
   pos = m_accessibleLocations.GetHeadPosition();
   while (pos)
   {
      CPPAccessibleLocation *accessibleLocation = m_accessibleLocations.GetNext(pos);
      accessibleLocation->DumpToFile(file, indent);
   }
   indent -= 3;

   switch (m_eNetType)
   {
   case probeableNetTypePower:
      file.WriteString("%*sm_eNetType = %s\n", indent, " ", "Power");
      break;
   case probeableNetTypeSignal:
      file.WriteString("%*sm_eNetType = %s\n", indent, " ", "Signal");
      break;
   case probeableNetTypeGround:
      file.WriteString("%*sm_eNetType = %s\n", indent, " ", "Ground");
      break;
   }
   file.WriteString("%*sm_iTestResourcesNeeded = %d\n", indent, " ", m_iTestResourcesNeeded);
   file.WriteString("%*sm_iPwrInjResourcesNeeded = %d\n", indent, " ", m_iPwrInjResourcesNeeded);

   indent -= 3;
}

void CPPNet::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<ProbeNet Name=\"%s\">\n", SwapSpecialCharacters(m_sNetName));

   writeFormat.pushHeader("  ");
   m_probes.WriteXML(writeFormat, progress);
   writeFormat.popHeader();

   writeFormat.writef("</ProbeNet>\n", m_sNetName);
}

//_____________________________________________________________________________
void CPPNetMap::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   POSITION pos = GetStartPosition();
   while (pos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      GetNextAssoc(pos, netName, ppNet);

      ppNet->WriteXML(writeFormat, progress);
   }
}

int CPPNetMap::AscendingNetNameSortFunc(const void *a, const void *b)
{
   CPPNet *ppNetA = (CPPNet*)(((SElement*) a )->pObject->m_object);
   CPPNet *ppNetB = (CPPNet*)(((SElement*) b )->pObject->m_object);

   
   return (ppNetA->GetNet()->getNetName().CompareNoCase(ppNetB->GetNet()->getNetName()));
}

//_____________________________________________________________________________
CQfeAccessibleLocation::CQfeAccessibleLocation()
: m_pAccLocation(NULL)
{
}

CQfeAccessibleLocation::CQfeAccessibleLocation(CPPAccessibleLocation *accessiblelocation)
: m_pAccLocation(accessiblelocation)
{
}

CQfeAccessibleLocation::~CQfeAccessibleLocation()
{
   m_pAccLocation = NULL;
}

CPoint2d CQfeAccessibleLocation::getOrigin() const
{
   return m_pAccLocation->GetLocation();
}

CExtent CQfeAccessibleLocation::getExtent() const
{
   return CExtent(0, 0, 0, 0);
}

CString CQfeAccessibleLocation::getInfoString() const
{
   return "";
}

bool CQfeAccessibleLocation::isExtentIntersecting(const CExtent& extent)
{
   return (psInside == extent.pointState(m_pAccLocation->GetLocation()));
}

bool CQfeAccessibleLocation::isPointWithinDistance(const CPoint2d& point,double distance)
{
   return (m_pAccLocation->GetLocation().distance(point) <= distance);
}

bool CQfeAccessibleLocation::isInViolation(CObject2d& otherObject)
{
   return FALSE;
}

int CQfeAccessibleLocation::getObjectType() const
{
   return 0;
}

CPPAccessibleLocation *GetAccessibleLocation()
{
   return m_pAccLocation;
}

//_____________________________________________________________________________
CAccessibleLocationList::CAccessibleLocationList(int nBlockSize=200)
: CTypedPtrList<CQfeList,CQfeAccessibleLocation*>(nBlockSize)
{
}

//_____________________________________________________________________________
CAccessibleLocationTree::CAccessibleLocationTree()
: CQfeExtentLimitedContainer(CSize2d(0., 0.), false, true)
{
}

CAccessibleLocationTree::~CAccessibleLocationTree()
{
}

int CAccessibleLocationTree::search(const CExtent& extent,CAccessibleLocationList& foundList)
{
   return CQfeExtentLimitedContainer::search(extent,foundList);
}

void CAccessibleLocationTree::setAt(CQfeAccessibleLocation* accessiblelocation)
{
   CQfeExtentLimitedContainer::setAt(accessiblelocation);
}

//_____________________________________________________________________________
CProbeCount::CProbeCount()
{
   m_bUsedTop = false;
   m_bUsedBot = false;
   m_eSurface = testSurfaceMIN;
   m_iTestResourceCountTop = 0;
   m_iPwrInjResourceCountTop = 0;
   m_iTestResourceCountBot = 0;
   m_iPwrInjResourceCountBot = 0;
}

CProbeCount::CProbeCount(CString probeName, ETestSurface surface, bool used)
{
   m_sName = probeName;
   m_eSurface = surface;
   m_iTestResourceCountTop = 0;
   m_iPwrInjResourceCountTop = 0;
   m_iTestResourceCountBot = 0;
   m_iPwrInjResourceCountBot = 0;

   m_bUsedTop = false;
   m_bUsedBot = false;
   if (surface == testSurfaceTop)
      m_bUsedTop = used;
   else if (surface == testSurfaceBottom)
      m_bUsedBot = used;
}

CProbeCount::CProbeCount(CProbeCount &probeCount)
{
   m_bUsedTop = probeCount.m_bUsedTop;
   m_bUsedBot = probeCount.m_bUsedBot;
   m_sName = probeCount.m_sName;
   m_eSurface = probeCount.m_eSurface;
   m_iTestResourceCountTop = probeCount.m_iTestResourceCountTop;
   m_iPwrInjResourceCountTop = probeCount.m_iPwrInjResourceCountTop;
   m_iTestResourceCountBot = probeCount.m_iTestResourceCountBot;
   m_iPwrInjResourceCountBot = probeCount.m_iPwrInjResourceCountBot;
}

//_____________________________________________________________________________
CPolyListItem::CPolyListItem(CPolyListItem &listItem)
{
   m_pPolyList = listItem.m_pPolyList;
   m_eSurface = listItem.m_eSurface;
}

CPolyListItem::CPolyListItem(CPolyList *polyList, ETestSurface surface)
{
   m_pPolyList = *polyList;
   m_eSurface = surface;
}

CPolyListItem::~CPolyListItem()
{
   //POSITION pos = m_pPolyList.GetHeadPosition();
   //while (pos)
   //{
   //   delete m_pPolyList.GetNext(pos);
   //}
   //m_pPolyList.RemoveAll();
}

//_____________________________________________________________________________
CPPProbeResult::CPPProbeResult(ETestResourceType resType, CString netName, CString accessName, CString templateName, CString probeRefname)
{
   m_eResourceType = resType;
   m_sNetName = netName;
   m_sAccessName = accessName;
   m_sProbeTemplateName = templateName;
   m_sProbeRefname = probeRefname;
}

CPPProbeResult::CPPProbeResult(const CPPProbeResult& other)
{
   m_eResourceType = other.m_eResourceType;
   m_sNetName = other.m_sNetName;
   m_sAccessName = other.m_sAccessName;
   m_sProbeTemplateName = other.m_sProbeTemplateName;
   m_sProbeRefname = other.m_sProbeRefname;
}

CPPProbeResult::~CPPProbeResult()
{
}

//_____________________________________________________________________________
CPPNetResult::CPPNetResult(CString netName)
{
   m_sNetName = netName;
   m_bNoProbe = true;
   m_probeResultList.empty();
}

CPPNetResult::CPPNetResult(const CPPNetResult& other)
{
   m_sNetName = other.m_sNetName;
   m_bNoProbe = other.m_bNoProbe;

   POSITION pos = other.m_probeResultList.GetHeadPosition();
   while (pos != NULL)
   {
      CPPProbeResult* probeResult = new CPPProbeResult(*other.m_probeResultList.GetNext(pos));
      if (probeResult == NULL)
         continue;
      
      m_probeResultList.AddTail(probeResult);
   }
}

CPPNetResult::~CPPNetResult()
{
   m_probeResultList.empty();
}

CPPProbeResult* CPPNetResult::AddProbeResult(ETestResourceType resType, CString netName, CString accessName, CString templateName, CString probeRefname)
{
   CPPProbeResult* probeResult = new CPPProbeResult(resType, netName, accessName, templateName, probeRefname);
   m_probeResultList.AddTail(probeResult);

   return probeResult;
}

void CPPNetResult::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<ProbeNet Name=\"%s\" NoProbe=\"%s\">\n", SwapSpecialCharacters(m_sNetName), m_bNoProbe?"True":"False");
   writeFormat.pushHeader("  ");
   
   POSITION pos = m_probeResultList.GetHeadPosition();
   while (pos != NULL)
   {
      CPPProbeResult* probeResult = m_probeResultList.GetNext(pos);
      if (probeResult == NULL)
         continue;

      writeFormat.writef("<Probe Number=\"%s\" TestResource=\"%s\" AccessName=\"%s\" TemplateName=\"%s\"/>\n",
                         probeResult->GetProbeRefname(),
                         (probeResult->GetResourceType()==testResourceTypePowerInjection)?"Power Injection":"Test",
                         SwapSpecialCharacters(probeResult->GetAccessName()),
                         SwapSpecialCharacters(probeResult->GetProbeTemplateName()));
   }

   writeFormat.popHeader();
   writeFormat.writef("</ProbeNet>\n");
}

int CPPNetResult::LoadXML(CXMLNode* node)
{
   if (node == NULL)
      return -1;

   CString nodeName;
   if (!node->GetName(nodeName) || nodeName != "ProbeNet")
      return 0;

   CString name;
   if (!node->GetAttrValue("Name", name))
      return 0;
   m_sNetName = name;

   CString buf;
   if (node->GetAttrValue("NoProbe", buf))
   {
      if (buf == "True")
         m_bNoProbe = true;
      else
         m_bNoProbe = false;
   }

   CXMLNode *subNode = NULL;
   node->ResetChildList();
   while (subNode = node->NextChild())
   {
      CString nodeName;
      if (!subNode->GetName(nodeName) || nodeName != "Probe")
         continue;

      CString buf;
      if (!subNode->GetAttrValue("Number", buf))
         continue;
      CString probeRefname = buf;

      if (!subNode->GetAttrValue("TestResource", buf))
         continue;
      ETestResourceType resType = (buf=="Power Injection")?testResourceTypePowerInjection:testResourceTypeTest;

      if (!subNode->GetAttrValue("AccessName", buf))
         continue;
      CString accessName = buf;

      if (!subNode->GetAttrValue("TemplateName", buf))
         continue;
      CString templateName = buf;

      CPPProbeResult* probeResult = new CPPProbeResult(resType, m_sNetName, accessName, templateName, probeRefname);
      m_probeResultList.AddTail(probeResult);

      delete subNode;
   }

   return 0;
}

//_____________________________________________________________________________
CProbePlacementSolution::CProbePlacementSolution(CCamCadData& camCadData, CTestPlan *testPlan, FileStruct *file)
{
   m_pDoc = doc;
   m_pTestPlan = testPlan;
   m_probeNamer.Reset();
   m_pProgressDlg = NULL;

   m_pFile = file;

   if (file == NULL)
      m_pFile = m_pDoc->getFileList().GetOnlyShown(blockTypePcb);

   gatherPackageOutlines();
}

CProbePlacementSolution::CProbePlacementSolution(CProbePlacementSolution &ppSolution, CTestPlan *testPlan)
{
   m_pDoc = ppSolution.m_pDoc;
   m_pFile = ppSolution.m_pFile;
   m_pTestPlan = testPlan;
   m_probeNamer = ppSolution.m_probeNamer;
   m_pProgressDlg = NULL;

   POSITION pos = m_ppNets.GetStartPosition();
   while (pos)
   {
      CString netName;
      CPPNet *pNet = NULL;

      ppSolution.m_ppNets.GetNextAssoc(pos, netName, pNet);
      m_ppNets.SetAt(netName, new CPPNet(*pNet));
   }

   pos = ppSolution.m_polyList.GetHeadPosition();
   while (pos)
   {
      CPolyListItem &pListItem = ppSolution.m_polyList.GetNext(pos);

      m_polyList.AddTail(pListItem);
   }

   pos = ppSolution.m_netResultList.GetHeadPosition();
   while (pos != NULL)
   {
      CPPNetResult* netResult = ppSolution.m_netResultList.GetNext(pos);
      if (netResult == NULL)
         continue;

      CPPNetResult* newNetResult = new CPPNetResult(*netResult);
      m_netResultList.AddTail(newNetResult);
   }
}

CProbePlacementSolution::~CProbePlacementSolution()
{
   RemoveAll_Nets();

   destroyProgress();
}

void CProbePlacementSolution::calculateNeededProbes()
{
   CDFTNetConditions &netConditions = m_pTestPlan->GetNetConditions();

   // start with the net conditions
   POSITION pos = netConditions.GetHeadPosition_ProbeableNets();
   while (pos)
   {
      CDFTProbeableNet *pNet = netConditions.GetNext_ProbeableNets(pos);
      CPPNet *ppNet = NULL;

      if (!m_ppNets.Lookup(pNet->GetName(), ppNet))
      {
         ppNet = new CPPNet(m_pDoc, pNet->GetName());
         m_ppNets.SetAt(pNet->GetName(), ppNet);
      }

      ppNet->SetNetType(pNet->GetNetType());
      if (pNet->GetNoProbe())
      {
         ppNet->SetNoProbeFlag(true);
         ppNet->SetTestResourcesNeeded(0);
         ppNet->SetPowerInjectionResourcesNeeded(0);
         continue;
      }
      ppNet->SetTestResourcesNeeded((pNet->GetTestResourcesPerNet()<1)?1:pNet->GetTestResourcesPerNet());
      ppNet->SetPowerInjectionResourcesNeeded(pNet->GetPowerInjectionResourcesPerNet());
   }

   // evalute the TESTNETPROBE attribute on each net
   WORD testNetProbesKW = m_pDoc->IsKeyWord(ATT_TEST_NET_PROBES, 0);
   if (testNetProbesKW >= 0)
   {
      pos = m_pFile->getNetList().GetHeadPosition();
      while (pos)
      {
         NetStruct *pNet = m_pFile->getNetList().GetNext(pos);
         CPPNet *ppNet = NULL;

         if (!m_ppNets.Lookup(pNet->getNetName(), ppNet))
         {
            ppNet = new CPPNet(m_pDoc, pNet->getNetName());
            m_ppNets.SetAt(pNet->getNetName(), ppNet);
         }
         ppNet->SetNet(pNet);

         // if set to not probe, we have not test needs
         if (ppNet->GetNoProbeFlag())
            continue;

         Attrib *attrib = NULL;
         if (!(pNet->getAttributes() && pNet->getAttributes()->Lookup(testNetProbesKW, attrib)))
            continue;

         if (ppNet->GetTestResourcesNeeded() < attrib->getIntValue())
            ppNet->SetTestResourcesNeeded(attrib->getIntValue());
      }
   }

   // calculate power injection needs
   int powerUsage = m_pTestPlan->GetPowerInjectionUsage();
   int powerValue = m_pTestPlan->GetPowerInjectionValue();

   if (powerUsage != 0) // No power injection
   {
      int newPowerInjectionValue = 0;
      
      if (powerUsage == 1)    // Probes per power rail
         newPowerInjectionValue = powerValue;
      else if (powerUsage == 3)     // Probes per Number of Nets per Power Rail
      {
         // default to one so we don't divide by zero
         if (powerValue == 0)
            powerValue = 1;
         newPowerInjectionValue = m_pFile->getNetCount() / powerValue;
      }

      POSITION pos = m_ppNets.GetStartPosition();
      while (pos)
      {
         CString netName;
         CPPNet *ppNet = NULL;
         m_ppNets.GetNextAssoc(pos, netName, ppNet);

         if (ppNet->GetNetType() != probeableNetTypePower  && ppNet->GetNetType() != probeableNetTypeGround)
            continue;

         if (ppNet->GetNoProbeFlag())
            continue;

         if (powerUsage == 2)    // Probes per Number of Connections per Power Rail
         {
            NetStruct *pNet = ppNet->GetNet();
            int connectionCount = pNet->getCompPinCount();

            // default to one so we don't divide by zero
            if (powerValue == 0)
               powerValue = 1;
            newPowerInjectionValue = connectionCount / powerValue;
         }

         if (ppNet->GetPowerInjectionResourcesNeeded() < newPowerInjectionValue)
            ppNet->SetPowerInjectionResourcesNeeded(newPowerInjectionValue);
      }
   }
}

void CProbePlacementSolution::gatherPackageOutlines()
{
   if (m_pFile != NULL)
   {
      POSITION pos = m_pFile->getBlock()->getHeadDataInsertPosition();
      while (pos)
      {
         DataStruct *data = m_pFile->getBlock()->getNextDataInsert(pos);

         if (data->getInsert()->getInsertType() != insertTypePcbComponent)
            continue;

         BlockStruct *subBlock = m_pDoc->getBlockAt(data->getInsert()->getBlockNumber());

         POSITION subPos = subBlock->getHeadDataPosition();
         while (subPos)
         {
            DataStruct *subData = subBlock->getNextData(subPos);

            if (subData->getDataType() == dataTypePoly && subData->getGraphicClass() == graphicClassPackageOutline)
            {
               CTMatrix mat(m_pFile->getTMatrix());
               mat.translateCtm(data->getInsert()->getOrigin2d());
               mat.rotateRadiansCtm(data->getInsert()->getAngle());
               mat.scaleCtm(data->getInsert()->getMirrored() ? -data->getInsert()->getScale() : data->getInsert()->getScale(),data->getInsert()->getScale());

               ETestSurface outlineSurface = data->getInsert()->getLayerMirrored()?testSurfaceBottom:testSurfaceTop;                
               CPolyListItem polyListItem = CPolyListItem(subData->getPolyList(), outlineSurface);
               polyListItem.Transform(mat);
               m_polyList.AddTail(polyListItem);
               break;
            }
         }
      }
   }
}

void CProbePlacementSolution::applyNoProbeFeatures(CDFTPreconditions &Preconditions)
{
   POSITION noProbePos = Preconditions.GetHeadPosition_NoProbedFeatures();
   while (noProbePos)
   {
      CDFTFeature *pFeature = Preconditions.GetNext_NoProbedFeatures(noProbePos);
      CString featureComp = pFeature->GetComponent(), featurePin = pFeature->GetPin();
      bool featureFound = false;

      // see if there is a comp/pin access point for this feature
      POSITION netPos = m_ppNets.GetStartPosition();
      while (netPos && !featureFound)
      {
         CString netName;
         CPPNet *ppNet = NULL;
         m_ppNets.GetNextAssoc(netPos, netName, ppNet);

         POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations();
         while (accPos)
         {
            CPPAccessibleLocation *pAccLoc = ppNet->GetNext_AccessibleLocations(accPos);

            if (pAccLoc->getFeatureEntity().getEntityType() != entityTypeCompPin)
               continue;

            CompPinStruct* cp = pAccLoc->getFeatureEntity().getCompPin();

            CString comp = cp->getRefDes();
            CString pin = cp->getPinName();

            if (featureComp == comp && featurePin == pin)
            {
               pAccLoc->SetFeatureCondition(featureConditionNoProbe);
               featureFound = true;
               break;
            }
         }
      }
   }
}

void CProbePlacementSolution::createForcedAccessLocations(CDFTPreconditions &Preconditions)
{
   POSITION forcedProbePos = Preconditions.GetHeadPosition_ForcedFeatures();
   while (forcedProbePos)
   {
      CDFTFeature *pFeature = Preconditions.GetNext_ForcedFeatures(forcedProbePos);
      CString featureComp = pFeature->GetComponent(), featurePin = pFeature->GetPin();
      CPPAccessibleLocation *pFoundAccLoc = NULL;
      CPPNet *ppFoundNet = NULL;

      // see if there is a comp/pin access point for this feature
      POSITION netPos = m_ppNets.GetStartPosition();
      while (netPos && ppFoundNet == NULL)
      {
         CString netName;
         CPPNet *ppNet = NULL;
         m_ppNets.GetNextAssoc(netPos, netName, ppNet);

         POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations(entityTypeCompPin);
         while (accPos && pFoundAccLoc == NULL)
         {
            CPPAccessibleLocation *accLoc = ppNet->GetNext_AccessibleLocations(accPos, entityTypeCompPin);
            //CompPinStruct *cp = (CompPinStruct*)accLoc->GetFeature();
            CompPinStruct* cp = accLoc->getFeatureEntity().getCompPin();

            CString comp = cp->getRefDes();
            CString pin = cp->getPinName();
            if (featureComp != comp || featurePin != pin)
               continue;

            pFoundAccLoc = accLoc;
            ppFoundNet = ppNet;
         }
      } // END while (netPos && !featureFound) ...

      // if there is no access location, then we need to create it (as unplaced)
      if (pFoundAccLoc == NULL)
      {
         CPoint2d pinLoc;
         BlockStruct *block = m_pFile->getBlock();
         POSITION dataPos = block->getHeadDataInsertPosition();
         while (dataPos)   
         {
            DataStruct *pcbData = block->getNextDataInsert(dataPos);
            if (pcbData->getInsert()->getInsertType() != insertTypePcbComponent)
               continue;

            if (pcbData->getInsert()->getRefname() != featureComp)
               continue;

            BlockStruct *subBlock = m_pDoc->getBlockAt(pcbData->getInsert()->getBlockNumber());
            POSITION subDataPos = subBlock->getHeadDataInsertPosition();
            while (subDataPos)
            {
               DataStruct *pinData = subBlock->getNextDataInsert(subDataPos);
               if (pinData->getInsert()->getInsertType() != insertTypePin)
                  continue;

               if (pinData->getInsert()->getRefname() != featurePin)
                  continue;

               Point2 point2(pinData->getInsert()->getOrigin().x, pinData->getInsert()->getOrigin().y);
               DTransform xform(pcbData->getInsert()->getOrigin2d().x, pcbData->getInsert()->getOrigin2d().y,
                  1, pcbData->getInsert()->getAngle(), pcbData->getInsert()->getMirrorFlags());
               xform.TransformPoint(&point2);
               pinLoc.x = point2.x;
               pinLoc.y = point2.y;
            }
         }

         NetStruct *net = NULL;
         CompPinStruct *cp = FindCompPin(m_pFile, featureComp, featurePin, &net);
         if (cp == NULL || net == NULL)
            continue;

         // Create accessible location where the pointer to the test access point is NULL.  This is how we know it doesn't
         // already exist from Access Analysis and needs to be created when placing the probe down.
         pFoundAccLoc = AddTail_AccessibleLocations(net->getNetName(), CEntity(cp), pinLoc, "", testSurfaceTop);
      }

      if (pFoundAccLoc != NULL)
         pFoundAccLoc->SetFeatureCondition(featureConditionForced);
   }
}

void CProbePlacementSolution::applyForcedFeatures(CDFTPreconditions &Preconditions)
{
   createForcedAccessLocations(Preconditions);

   POSITION forcedProbePos = Preconditions.GetHeadPosition_ForcedFeatures();
   while (forcedProbePos)
   {
      CDFTFeature *pFeature = Preconditions.GetNext_ForcedFeatures(forcedProbePos);
      CString featureComp = pFeature->GetComponent();
      CString featurePin = pFeature->GetPin();
      CPPAccessibleLocation *pFoundAccLoc = NULL;
      CPPNet *ppFoundNet = NULL;

      if (!featureComp.IsEmpty() && !featurePin.IsEmpty())
      {
         // see if there is a comp/pin access point for this feature
         POSITION netPos = m_ppNets.GetStartPosition();
         while (netPos && ppFoundNet == NULL)
         {
            CString netName;
            CPPNet *ppNet = NULL;
            m_ppNets.GetNextAssoc(netPos, netName, ppNet);

            POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations(entityTypeCompPin);
            while (accPos && pFoundAccLoc == NULL)
            {
               CPPAccessibleLocation *accLoc = ppNet->GetNext_AccessibleLocations(accPos, entityTypeCompPin);
               CompPinStruct* cp = accLoc->getFeatureEntity().getCompPin();

               CString comp = cp->getRefDes();
               CString pin = cp->getPinName();
               if (featureComp == comp && featurePin == pin)
               {
                  pFoundAccLoc = accLoc;
                  pFoundAccLoc->SetFeatureCondition(featureConditionForced);
                  ppFoundNet = ppNet;
               }
            }
         } // END while (netPos && ppFoundNet == NULL) ...


         if (pFoundAccLoc != NULL)
         {
            // determine what probe (template) we can place at this point
            int cost = 0;
            CDFTProbeTemplate *probeTemplate = NULL;
            if (pFoundAccLoc->GetSurface() == testSurfaceTop)
               probeTemplate = m_pTestPlan->GetProbes().GetTail_TopProbes();
            else
               probeTemplate = m_pTestPlan->GetProbes().GetTail_BotProbes();

            // Find a probe on the specified net and place it
            CPPProbe *pProbe = NULL;
            POSITION probePos = ppFoundNet->GetHeadPosition_Probes();
            while (probePos && pProbe == NULL)
            {
               CPPProbe *probe = ppFoundNet->GetNext_Probes(probePos);

               if (probe->IsPlaced())
                  continue;

               if (probe->GetNetName() != pFoundAccLoc->GetNetName())
                  continue;

               pProbe = probe;
               pFoundAccLoc->PlaceProbe(pProbe);
               pProbe->SetProbeTemplate(probeTemplate);
            }

            // if the probe doesn't exist, we need to create it
            if (pProbe == NULL)
            {
               CString probeRefname = m_probeNamer.GetNextProbeRefname(ppFoundNet, testResourceTypeTest);

               CPPProbe *pProbe = ppFoundNet->AddTail_Probes(testResourceTypeTest, pFoundAccLoc->GetNetName(), probeRefname);

               pFoundAccLoc->PlaceProbe(pProbe);
               pProbe->SetProbeTemplate(probeTemplate);
            }
         }
      }
   } // END while (forcedProbePos) ...
}

void CProbePlacementSolution::applyPreconditions(CDFTPreconditions &Preconditions)
{
   // use preconditions only when we aren't forcing probes on all access markers
   if (!m_pTestPlan->GetPlaceOnAllAccessible())
   {
      // For each feature specified to not be probed, set any accessible location
      // at a specific feature to never be probed.  We don't need to worry about
      // any features that don't have accessible locations because they will not
      // be probed anyway, unless they are forced.  Forced probes take precedence
      // and will create an accessible location if needed.
      applyNoProbeFeatures(Preconditions);
      applyForcedFeatures(Preconditions);
   }
}

void CProbePlacementSolution::addAllNets()
{
   RemoveAll_Nets();

   if (m_pProgressDlg != NULL)
      m_pProgressDlg->SetStatus("Gathering nets...");

   //////////////////////////////////////////////////
   // Gather nets
   POSITION pos = m_pFile->getNetList().GetHeadPosition();
   while (pos)
   {
      NetStruct *net = m_pFile->getNetList().GetNext(pos);

      CPPNet *ppNet = NULL;
      if (!m_ppNets.Lookup(net->getNetName(), ppNet))
      {
         m_ppNets.SetAt(net->getNetName(), new CPPNet(m_pDoc, net->getNetName()));
      }
   }

   if (m_pProgressDlg != NULL)
      m_pProgressDlg->StepIt();
}

void CProbePlacementSolution::gatherAccesibleLocations()
{
   addAllNets();

   if (m_pProgressDlg != NULL)
      m_pProgressDlg->SetStatus("Gathering accessible locations...");

   //////////////////////////////////////////////////
   // Gather accessible locations
   BlockStruct *fileBlock = m_pFile->getBlock();
   if (fileBlock != NULL)
   {
      POSITION pos = fileBlock->getHeadDataInsertPosition();
      while (pos)
      {
         DataStruct *data = fileBlock->getNextDataInsert(pos);
         if (data->isInsertType(insertTypeTestAccessPoint))
         {
            AddTail_AccessibleLocations(data);
         }
      }
   }

   if (m_pProgressDlg != NULL)
      m_pProgressDlg->StepIt();
}

void CProbePlacementSolution::createProbes()
{
   RemoveAll_Probes();
   POSITION pos = m_ppNets.GetStartPosition();
   while (pos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(pos, netName, ppNet);

      // use the number of specified resources
      if (!m_pTestPlan->GetPlaceOnAllAccessible())
      {
         if (ppNet->GetNoProbeFlag())
            continue;

         for (int i=0; i<ppNet->GetTestResourcesNeeded(); i++)//*rcf need to adjust this so power gets added as power ?
            AddTail_Probes(netName, testResourceTypeTest);

         for (int i=0; i<ppNet->GetPowerInjectionResourcesNeeded(); i++)
            AddTail_Probes(netName, testResourceTypePowerInjection);
      }
      else
      {
         // we want to force a probe on all access markers, so we need to make as
         // many probes as there are markers
         for (int i=0; i<ppNet->GetCount_AccessibleLocations(); i++)
            AddTail_Probes(netName, testResourceTypeTest);
      }
   }
}

void CProbePlacementSolution::createAdditionalProbes()
{
   POSITION pos = m_ppNets.GetStartPosition();
   while (pos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(pos, netName, ppNet);

      // use the number of psecified resources
      if (!m_pTestPlan->GetPlaceOnAllAccessible())
      {
         if (ppNet->GetNoProbeFlag())
            continue;

         for (int i=ppNet->GetCount_TestProbes(); i<ppNet->GetTestResourcesNeeded(); i++)
            AddTail_Probes(netName, testResourceTypeTest);

         for (int i=ppNet->GetCount_PowerInjectionProbes(); i<ppNet->GetPowerInjectionResourcesNeeded(); i++)
            AddTail_Probes(netName, testResourceTypePowerInjection);
      }
      else
      {
         // we want to force a probe on all access markers, so we need to make as
         // many probes as there are markers (minus the ones that already have exist)
         for (int i=ppNet->GetCount_TestProbes(); i<ppNet->GetCount_AccessibleLocations(); i++)
            AddTail_Probes(netName, testResourceTypeTest);
      }
   }
}

void CProbePlacementSolution::gatherAndMatchProbes()
{
   int normalProbeNum = m_pTestPlan->GetProbeStartNumber();
   int powerProbeNum = 1;
   int groundProbeNum = 1;

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
         if (m_pTestPlan->RetainProbeNames())
         {
            probe->SetProbeRefname( insert->getRefname() );
         }
         else
         {
            bool power = false;
            bool ground = false;
            CPPNet *ppNet = NULL;
            if (!m_ppNets.Lookup(netName, ppNet))
            {
               if (ppNet->IsPowerNet())
                  power = true;
               if (ppNet->IsGroundNet())
                  ground = true;
            }
            CString refname;
            if (power)
               refname.Format("P%d", powerProbeNum++);
            else if (ground)
               refname.Format("G%d", groundProbeNum++);
            else
               refname.Format("%d", normalProbeNum++);    

            probe->SetProbeRefname(refname);
         }

         // Only "Placed" probes should be marked as existing
         bool placed = false;
         WORD testProbePlacedKw = m_pDoc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, valueTypeString);
         if (data->lookUpAttrib(testProbePlacedKw , attrib))
            placed = ((CString)"Placed" == ((CString)get_attvalue_string(m_pDoc, attrib)));

         probe->SetAsExisting(placed);

         ETestSurface probeSurface = testSurfaceUnset;
         if (m_pTestPlan->GetUseExistingProbes() && m_pTestPlan->RetainProbePositions())
         {
            // Get the access point the probe is on
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
                  //((CFixtureAccLoc*)accLoc)->SetForFixtureFlag(false);
                  break;
               }
               else if (!accLoc->IsDoubleWired())
               {
                  accLoc->PlaceDoubleWiredProbe(probe);
                  //((CFixtureAccLoc*)accLoc)->SetForFixtureFlag(false);
                  break;
               }
            }
         }
         else
            probeSurface = insert->getMirrored()?testSurfaceBottom:testSurfaceTop;

         if (m_pTestPlan->GetUseExistingProbes() && m_pTestPlan->RetainProbeSizes() && entityLink > 0)
         {
            // Get the probe size
            BlockStruct *insBlock = m_pDoc->getBlockAt(insert->getBlockNumber());
            CString m_sTemplateName = insBlock->getName().Mid(0, insBlock->getName().Find("_"));
            if (probeSurface == testSurfaceTop)
            {
               POSITION probePos = NULL;
               CDFTProbeTemplate *pTemplate = m_pTestPlan->GetProbes().Find_TopProbes(m_sTemplateName, probePos);
               if (pTemplate != NULL)
                  probe->SetProbeTemplate(pTemplate);
               //POSITION probePos = m_pTestPlan->GetProbes().GetHeadPosition_TopProbes();
               //while (probePos)
               //{
               // CDFTProbeTemplate *pTemplate = m_pTestPlan->GetProbes().GetNext_TopProbes(probePos);

               // if (pTemplate->GetName() ==  m_sTemplateName)
               // {
               //    probe->SetProbeTemplate(pTemplate);
               //    break;
               // }
               //} // END while (probePos) ...
            }
            else if (probeSurface == testSurfaceBottom)
            {
               POSITION probePos = NULL;
               CDFTProbeTemplate *pTemplate = m_pTestPlan->GetProbes().Find_BotProbes(m_sTemplateName, probePos);
               if (pTemplate != NULL)
                  probe->SetProbeTemplate(pTemplate);
               //POSITION probePos = m_pTestPlan->GetProbes().GetHeadPosition_BotProbes();
               //while (probePos)
               //{
               // CDFTProbeTemplate *pTemplate = m_pTestPlan->GetProbes().GetNext_BotProbes(probePos);

               // if (pTemplate->GetName() ==  m_sTemplateName)
               // {
               //    probe->SetProbeTemplate(pTemplate);
               //    break;
               // }
               //} // END while (probePos) ...
            }
         }
      } // END while (pos) ...
   } // END if (fileBlock != NULL) ...
}

void CProbePlacementSolution::gatherPlacedProbes()
{
   RemoveAll_Probes();
   WORD netNameKW = m_pDoc->RegisterKeyWord(ATT_NETNAME, 0, valueTypeString);
   WORD testResKW = m_pDoc->RegisterKeyWord(ATT_TESTRESOURCE, 0, valueTypeString);
   WORD dataLinkKW = m_pDoc->RegisterKeyWord(ATT_DDLINK, 0, valueTypeInteger);

   if (m_pProgressDlg != NULL)
      m_pProgressDlg->SetStatus("Gathering Probes...");

   //////////////////////////////////////////////////
   // Gather accessible locations
   BlockStruct *fileBlock = m_pFile->getBlock();
   if (fileBlock != NULL)
   {
      POSITION pos = fileBlock->getHeadDataInsertPosition();
      while (pos)
      {
         DataStruct *data = fileBlock->getNextDataInsert(pos);
         InsertStruct *insert = data->getInsert();

         // make sure we have a probe
         if (insert->getInsertType() != insertTypeTestProbe)
            continue;

         // get the netname
         Attrib *attrib = NULL;
         if (!data->lookUpAttrib(netNameKW, attrib))
            continue;
         CString netname = get_attvalue_string(m_pDoc, attrib);

         // get the resource type
         if (!data->lookUpAttrib(testResKW, attrib))
            continue;
         CString testRes = get_attvalue_string(m_pDoc, attrib);;
         ETestResourceType resource = testResourceTypeTest;
         if (testRes == "Power Injection")
            resource = testResourceTypePowerInjection;

         // get the probe data link to the access marker
         if (!data->lookUpAttrib(dataLinkKW, attrib))
            continue;
         int datalink = attrib->getIntValue();

         // find the accessible location the probe is linked to
         CPPNet *ppNet = NULL;
         if (!m_ppNets.Lookup(netname, ppNet))
            continue;
         for (POSITION netPos = ppNet->GetHeadPosition_AccessibleLocations(); netPos!=NULL; ppNet->GetNext_AccessibleLocations(netPos))
         {
            CPPAccessibleLocation *pAccLoc = ppNet->GetAt_AccessibleLocations(netPos);
            if (pAccLoc->GetTAInsert()->getEntityNumber() == datalink)
            {
               CPPProbe *pProbe = AddTail_Probes(netname, resource);
               pProbe->SetProbeRefname(insert->getRefname());
               pAccLoc->PlaceProbe(pProbe);
               break;
            }
         }

      }
   }

   if (m_pProgressDlg != NULL)
      m_pProgressDlg->StepIt();
}

CPPAccessibleLocation* CProbePlacementSolution::getAccessLocationByTargetPriority(CPPNet *ppNet, ETestSurface favored)
{
   // the lower the (non-negative) value the higher the priority
   // the highest priority is 0

   int highestPriority = -1; 
   CPPAccessibleLocation* accLocWithHighestPriority = NULL;

   POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations();
   while (accPos != NULL)
   {
      CPPAccessibleLocation *accLoc = ppNet->GetNext_AccessibleLocations(accPos);
      if ( 
         (accLoc != NULL) &&
         (accLoc->GetFeatureCondition() == featureConditionProbeable) &&
         (accLoc->GetPlaceableProbe() == NULL) &&
         (accLoc->GetSurface() == favored)
         )
      {
         int targetPriority = getTargetTypePriority(accLoc);
         if (targetPriority >= 0)
         {
            if (highestPriority == -1 || highestPriority > targetPriority)
            {
               highestPriority = targetPriority;
               accLocWithHighestPriority = accLoc;
            }
         }
      }

   }

   return accLocWithHighestPriority;
}

CPPAccessibleLocation *CProbePlacementSolution::getAvailableAccessLocationOnSurface(CPPNet *ppNet, ETestSurface favored)
{
   POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations();
   while (accPos)
   {
      CPPAccessibleLocation *accLoc = ppNet->GetNext_AccessibleLocations(accPos);

      if (
         (accLoc->GetFeatureCondition() == featureConditionProbeable) &&
         (accLoc->GetPlaceableProbe() == NULL) &&
         (isValidTargetType(accLoc)) &&
         (accLoc->GetSurface() == favored)
         )
      {
         return accLoc;
      }
   }

   return NULL;
}

bool CProbePlacementSolution::anyProbeUsesComponentOutline()
{
   CDFTProbeTemplates &pTemplates = m_pTestPlan->GetProbes();

   if (m_pTestPlan->GetProbeSide() == testSurfaceTop || m_pTestPlan->GetProbeSide() == testSurfaceBoth)
   {
      POSITION templatePos = pTemplates.GetHeadPosition_TopProbes();
      while (templatePos)
      {
         CDFTProbeTemplate *pTemplate = pTemplates.GetNext_TopProbes(templatePos);
         if (pTemplate->GetUseFlag())
         {
            if (pTemplate->GetUseComponentOutline())
            {
               return true;
            }
         }
      }
   }

   if (m_pTestPlan->GetProbeSide() == testSurfaceBottom || m_pTestPlan->GetProbeSide() == testSurfaceBoth)
   {
      POSITION templatePos = pTemplates.GetHeadPosition_BotProbes();
      while (templatePos)
      {
         CDFTProbeTemplate *pTemplate = pTemplates.GetNext_BotProbes(templatePos);
         if (pTemplate->GetUseFlag())
         {
            if (pTemplate->GetUseComponentOutline())
            {
               return true;
            }
         }
      }
   }

   return false;
}

void CProbePlacementSolution::removeExcessUnplacedProbes()
{
	// Remove unplaced probes for nets whose total probe count
	// exceeds the resource requirement.
	// E.g. if a net has one placed and one unplaced probe, and resource
	// requirement is one, then the unplaced probe should be eliminated.

   POSITION pos = m_ppNets.GetStartPosition();
   while (pos != NULL)
   {
      CString netName;
      CPPNet* ppNet = NULL;
      m_ppNets.GetNextAssoc(pos, netName, ppNet);

      if (ppNet == NULL || ppNet->GetNoProbeFlag())
         continue;

		int resourceNeeded = ppNet->GetTestResourcesNeeded();
		
		// Count resources present
		int resourcePresent = 0;	// placed probes
		int totalPresent = 0;		// all probes
      POSITION probePos = ppNet->GetHeadPosition_Probes();
      while (probePos != NULL)
      {
         CPPProbe* probe = ppNet->GetNext_Probes(probePos);
         if (probe != NULL)
			{
				totalPresent++;
				if (probe->GetAccessibleLocation() != NULL)
					resourcePresent++;
			}
		}

		if (totalPresent > resourceNeeded)
		{
			// Remove excess unplaced probes until total resource count is
			// equal to resources needed.
			// If probe is placed, leave it, even if there is an excess.
			// E.g. this might happen in a "retain existing probes" situation,
			// when resouce count was reduced. Keep it anyway.

			bool startover = true;
			while (totalPresent > resourceNeeded && startover)
			{
				startover = false;
				probePos = ppNet->GetHeadPosition_Probes();
				while (probePos != NULL && !startover)
				{
					CPPProbe* probe = ppNet->GetAt_Probes(probePos);
					if (probe != NULL)
					{
						if (!probe->IsPlaced() || probe->GetAccessibleLocation() == NULL)
						{
							totalPresent--;
							ppNet->RemoveAt_Probes(probePos);
							startover = true;
						}
						else
						{
							ppNet->GetNext_Probes(probePos);
						}
					}
				}
			}

		}
	}
}

void CProbePlacementSolution::placeProbes()
{
   POSITION pos = NULL;

   if (anyProbeUsesComponentOutline())
   {
      int startpercent = 0;
      if (m_pProgressDlg != NULL)
         startpercent = atoi(m_pProgressDlg->GetPercent());
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->SetStatus("Determining access point/component proximity...");

      resetAccessPointDistances();

      if (m_pProgressDlg != NULL)
         m_pProgressDlg->StepIt();
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->SetPos(startpercent);
   }

   if (m_pProgressDlg != NULL)
      m_pProgressDlg->SetStatus("Calculating needed probes...");
   calculateNeededProbes();
   if (m_pProgressDlg != NULL)
      m_pProgressDlg->StepIt();

   if (m_pTestPlan->GetUseExistingProbes())
   {
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->SetStatus("Gather existing probes...");
      gatherAndMatchProbes();
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->StepIt();

      if (m_pTestPlan->GetProbeUse() == probeUsageAddAdditional)
      {
         if (m_pProgressDlg != NULL)
            m_pProgressDlg->SetStatus("Adding additional probes...");
         createAdditionalProbes();
         if (m_pProgressDlg != NULL)
            m_pProgressDlg->StepIt();
      }
      else
      {
         if (m_pProgressDlg != NULL)
            m_pProgressDlg->StepIt();
      }

      if (!m_pTestPlan->RetainProbePositions() || m_pTestPlan->GetProbeUse() == probeUsageAddAdditional)
      {
         if (m_pProgressDlg != NULL)
            m_pProgressDlg->SetStatus("Placing additional probes...");
         placeProbesOnAccessPoints();
         if (m_pProgressDlg != NULL)
            m_pProgressDlg->StepIt();
      }
      else
      {
         if (m_pProgressDlg != NULL)
            m_pProgressDlg->StepIt();
      }
   }
   else
   {
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->SetStatus("Creating probes...");
      createProbes();
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->StepIt();

      if (m_pProgressDlg != NULL)
         m_pProgressDlg->SetStatus("Applying preconditions...");
      applyPreconditions(m_pTestPlan->GetPreconditions());
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->StepIt();

      if (m_pProgressDlg != NULL)
         m_pProgressDlg->SetStatus("Placing probes...");
      placeProbesOnAccessPoints();
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->StepIt();
   }

   // optimize top probe locations
   if (!m_pTestPlan->GetUseExistingProbes() ||
      (m_pTestPlan->GetUseExistingProbes() && (m_pTestPlan->GetProbeUse() == probeUsageAddAdditional || !m_pTestPlan->RetainProbePositions())))
   {
      if (m_pTestPlan->GetProbeSide() == testSurfaceBoth)
      {
         // use surface priority if using both surfaces
         ETestSurface sidePref = m_pTestPlan->GetProbeSidePreference();
         ETestSurface otherSide = (sidePref==testSurfaceTop) ? testSurfaceBottom : testSurfaceTop;
         CString sidePrefName =   (sidePref == testSurfaceTop) ? "top" : "bottom";
         CString otherSideName = (otherSide == testSurfaceTop) ? "top" : "bottom";
         CString msg;

         msg.Format("Optimizing %s probe locations...", sidePrefName);
         if (m_pProgressDlg != NULL) m_pProgressDlg->SetStatus(msg);
         optimizeProbeLocationsOnSide(sidePref);

         msg.Format("Optimizing %s probe locations...", otherSideName);
         if (m_pProgressDlg != NULL) m_pProgressDlg->SetStatus(msg);
         optimizeProbeLocationsOnSide(otherSide);
      }
      else
      {
         // Only one side is active

         // Optimize top if active
         if (m_pTestPlan->CanProbeTopSide())
         {

            if (m_pProgressDlg != NULL)
               m_pProgressDlg->SetStatus("Optimizing top probe locations...");

            optimizeProbeLocationsOnSide(testSurfaceTop);
         }
         if (m_pProgressDlg != NULL)
            m_pProgressDlg->StepIt();

         // Optimize bottom if active
         if (m_pTestPlan->CanProbeBotSide())
         {
            if (m_pProgressDlg != NULL)
               m_pProgressDlg->SetStatus("Optimizing bottom probe locations...");

            optimizeProbeLocationsOnSide(testSurfaceBottom);
         }

         if (m_pProgressDlg != NULL)
            m_pProgressDlg->StepIt();
      }
   }
   else
   {
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->StepIt();
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->StepIt();
   }


   if (m_pProgressDlg != NULL)
      m_pProgressDlg->SetStatus("Maximizing probe sizes...");

   if (m_pTestPlan->GetTesterType() == DFT_TESTER_FIXTURE)
   {
      int loopCount = 0;
		int removedCount = 0;
      int violationCount = 0;
      // Use loopCount to stop infinite loop.
      // Case 2022 - Force loop to happen at least once or we get undetected/unfixed probe violations.
      while (loopCount < 10 && (((removedCount = maximizeProbeSizes()) > 0) || (loopCount < 1)))
      {
         violationCount = fixViolatingProbeTemplates();
         removeTemplatesFromProbes();
         loopCount++;
      }

		// Do the following only if there are still violations,
		// I.e. don't try to fix what ain't broke
		// Cases have shown that doing so can actually make the solution worse (e.g. case 1729)
		if (removedCount > 0 || violationCount > 0)
		{
			loopCount = 0;
			while (loopCount < 10 && fixViolatingProbeTemplates() > 0)
			{
				maximizeProbeSizes();
				loopCount++;
			}
		}

      resurrectNonViolatingUnplacedProbes();

      lastChanceMaximizeProbeSize(testSurfaceTop);
      lastChanceMaximizeProbeSize(testSurfaceBottom);

      if (m_pProgressDlg != NULL)
         m_pProgressDlg->StepIt();

      if (m_pProgressDlg != NULL)
         m_pProgressDlg->SetStatus("Processing double wiring...");
      doubleWireRemainingProbes();
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->StepIt();
   }
   else
   {
      setSmallestProbeSizes();
      if (m_pProgressDlg != NULL)
      {
         // Update the progress bar for maximizing probe sizes and processing double wiring
         m_pProgressDlg->StepIt();
         m_pProgressDlg->StepIt();
      }
   }
}

void CProbePlacementSolution::optimizeProbeLocationsOnSide(ETestSurface surface)
{
   // surface should be testSurfaceTop or testSurfaceBottom.
   // testSurfaceBoth is not appropriate here.

   if ((surface == testSurfaceTop && m_pTestPlan->CanProbeTopSide()) ||
      (surface == testSurfaceBottom && m_pTestPlan->CanProbeBotSide()))
   {
      bool firstTry = true;

      POSITION pos = NULL;
      if (surface == testSurfaceTop)
         pos = m_pTestPlan->GetProbes().GetTailPosition_UsedTopProbes();
      else
         pos = m_pTestPlan->GetProbes().GetTailPosition_UsedBotProbes();

      while (pos)
      {
         CDFTProbeTemplate *pTemplate = NULL;
         if (surface == testSurfaceTop)
            pTemplate = m_pTestPlan->GetProbes().GetPrev_UsedTopProbes(pos);
         else
            pTemplate = m_pTestPlan->GetProbes().GetPrev_UsedBotProbes(pos);

         if (pTemplate)
         {
            double probeDiameter = pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            double minDistToComp = 0;
            if (pTemplate->GetUseComponentOutline())
               minDistToComp = pTemplate->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            while (optimizeProbeLocations(probeDiameter, minDistToComp, firstTry) > 0) ;
            firstTry = false;
         }
      }
   }
}

void CProbePlacementSolution::placeProbesOnAccessPoints()
{
   // place all probes on access points
   POSITION pos = m_ppNets.GetStartPosition();
   while (pos != NULL)
   {
      CString netName;
      CPPNet* ppNet = NULL;
      m_ppNets.GetNextAssoc(pos, netName, ppNet);

      if (ppNet == NULL || ppNet->GetNoProbeFlag())
         continue;

      POSITION probePos = ppNet->GetHeadPosition_Probes();
      while (probePos != NULL)
      {
         CPPProbe* probe = ppNet->GetNext_Probes(probePos);

         // move on if already on an access point
         if (probe == NULL || probe->GetAccessibleLocation() != NULL)
            continue;
         
         CPPAccessibleLocation *accLoc = NULL;
         if (m_pTestPlan->GetProbeSide() == testSurfaceBoth)
         {
            // use surface priority if using both surfaces when initally placing probes (placeProbes)
            ETestSurface sidePref = m_pTestPlan->GetProbeSidePreference();
            ETestSurface otherSide = (sidePref==testSurfaceTop)?testSurfaceBottom:testSurfaceTop;

            if (sidePref == testSurfaceTop && haveTopProbes || sidePref == testSurfaceBottom && haveBotProbes)
               accLoc = getAccessLocationByTargetPriority(ppNet, sidePref);
            if (accLoc == NULL && (otherSide == testSurfaceTop && haveTopProbes || otherSide == testSurfaceBottom && haveBotProbes))
               accLoc = getAccessLocationByTargetPriority(ppNet, otherSide);
         }
         else
         {
            if (m_pTestPlan->GetProbeSide() == testSurfaceTop && haveTopProbes ||
               m_pTestPlan->GetProbeSide() == testSurfaceBottom && haveBotProbes)
               accLoc = getAccessLocationByTargetPriority(ppNet, m_pTestPlan->GetProbeSide());
         }

         // if there are not more accessible locations, then we are done with this net
         if (accLoc != NULL)
         {
            accLoc->PlaceProbe(probe);
#ifdef HELP_SEE_ALL
            // BAD Thing to do
            // This causes a bug in the  probe placement optimization, but it is useful
            // for enabling the viewing of all potential probes in the result. Without
            // this, some probes will not get a template, and will not make it to the
            // final stage. Although it creates a bug, it is useful for some debugging
            // purposes, and that is all.
            if (!probe->HasProbeTemplate())
            {
               CDFTProbeTemplate *probeTemplate = NULL;
               if (accLoc->GetSurface() == testSurfaceTop)
                  probeTemplate = m_pTestPlan->GetProbes().GetTail_TopProbes();
               else
                  probeTemplate = m_pTestPlan->GetProbes().GetTail_BotProbes();
               probe->SetProbeTemplate(probeTemplate);
            }
#endif
         }
      }
   }
}

int CProbePlacementSolution::getProbeDensity(CPoint2d point, double actualdiameter, ETestSurface surface, CAccessibleLocationList &foundList)
{
   int densityCount = 0;

   // Roundoff and direct float comparison is hindering density check.
   // Overlap "on the line" (ie boundary) is okay. Just subtract off
   // some fuzz from the size, to account for "close floats".
   double extentSize = actualdiameter - SMALLNUMBER;

   // It seems odd that this is using diameter rather than radius.
   // For the time being, we'll leave it, presuming it is somehow correct
   // for the algorithm. But it is suspect.
   CExtent extent(-extentSize, -extentSize, extentSize, extentSize);

   extent.translate(point.x, point.y);
   int count = 0;

   if (surface == testSurfaceTop)
      count = m_LocationTreeTop.search(extent, foundList);
   else if (surface == testSurfaceBottom)
      count = m_LocationTreeBot.search(extent, foundList);

   POSITION pos = foundList.GetHeadPosition();
   while (pos)
   {
      POSITION curPos = pos;
      CQfeAccessibleLocation *qfeAccLoc = foundList.GetNext(pos);

      CPPAccessibleLocation *accLoc = qfeAccLoc->GetAccessibleLocation();
      if (accLoc->GetPlaceableProbe() == NULL)
      {
         // No probe here, not an item for further consideration.
         foundList.RemoveAt(curPos);
      }
      else if (!qfeAccLoc->isPointWithinDistance(point, extentSize))
      {
         // Weed out false positive.
         // Rectangular extent is not great for round objects, it gives false positives.
         // But rectangular extents are convenient to use (above) due to facilities available
         // in camcad. If we're here, then item is not really within distance, remove it from 
         // found list.

         foundList.RemoveAt(curPos);
      }
      else
      {
         // It's really there, count it.
         densityCount++;
      }
   }

   return densityCount;
}

int CProbePlacementSolution::getActualProbeDensity(CPoint2d probe1_point, double probe1_diameter, ETestSurface surface, CAccessibleLocationList &foundList)
{
   int densityCount = 0;

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
         CPPProbe *probe2 = ppNet->GetNext_Probes(probePos);

         if (probe2 && probe2->IsPlaced())
         {
            CString probe2_templateName = probe2->GetProbeTemplateName();
            
            CPPAccessibleLocation *accLoc = probe2->GetAccessibleLocation();

            if (accLoc && accLoc->GetSurface() == surface &&
               accLoc->GetPlaceableProbe() != NULL)
            {
               CDFTProbeTemplates &pTemplates = m_pTestPlan->GetProbes();

               POSITION dummyPos = NULL;
               CDFTProbeTemplate *pTemplate = pTemplates.Find_Probe(probe2->GetProbeTemplateName(), dummyPos, accLoc->GetSurface());

               if (pTemplate)
               {
                  double probe2_diameter = pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());

                  double sumOfRadii = (probe1_diameter + probe2_diameter) / 2.0;
                  double distance = probe1_point.distance(probe2->GetAccessibleLocation()->GetLocation());

                  // If "on the line" (distance == sumOfRadii) they are  considered
                  // non-overlapping, don't count them. Count only true overlaps.
                  if (distance < sumOfRadii) 
                  {
                     densityCount++;
                     foundList.AddTail(new CQfeAccessibleLocation(accLoc));
                  }
                  
               }
            }
         }
      }
   }

   return densityCount;
}


int CProbePlacementSolution::getTargetTypePriority(CPPAccessibleLocation *accLoc)
{
   int priority = -1;
   int index = 0;
   CDFTTargetPriority &tPriority = m_pTestPlan->GetTargetPriority();

   int strIndex = 0;
   CString targetType = accLoc->GetTargetType();
   CString curTargetName = targetType.Tokenize(", ", strIndex);
   while (!curTargetName.IsEmpty())
   {
      if (accLoc->GetSurface() == testSurfaceTop)
      {
         CDFTTarget* target = tPriority.Find_TopTargets(curTargetName, index);
         if (target && target->GetEnabledFlag() && (index <= priority || priority == -1))
            priority = index;
      }
      else if (accLoc->GetSurface() == testSurfaceBottom)
      {
         CDFTTarget* target = tPriority.Find_BotTargets(curTargetName, index);
         if (target && target->GetEnabledFlag() && (index <= priority || priority == -1))
            priority = index;
      }

      curTargetName = targetType.Tokenize(", ", strIndex);
   }

   return priority;
}

bool CProbePlacementSolution::isValidTargetType(CPPAccessibleLocation *accLoc)
{
   bool foundTargetType = false;

   int index = 0;
   CDFTTarget *target = NULL;
   CDFTTargetPriority &tPriority = m_pTestPlan->GetTargetPriority();

   CString targetType = accLoc->GetTargetType();
   int strIndex = 0;
   CString curTargetName = targetType.Tokenize(", ", strIndex);
   while (!curTargetName.IsEmpty())
   {
      if (accLoc->GetSurface() == testSurfaceTop)
      {
         target = tPriority.Find_TopTargets(curTargetName, index);
         if (target && target->GetEnabledFlag())
            return true;
      }
      else if (accLoc->GetSurface() == testSurfaceBottom)
      {
         target = tPriority.Find_BotTargets(curTargetName, index);
         if (target && target->GetEnabledFlag())
            return true;
      }

      curTargetName = targetType.Tokenize(", ", strIndex);
   }

   return false;
}

bool CProbePlacementSolution::isTooCloseToComponentOutline(CPPAccessibleLocation *accLoc, double minDistToOutline)
{
#ifdef ORIGINAL
   double shortestDistanceToComp = DBL_MAX;
   CPoint2d location = accLoc->GetLocation();
   ETestSurface surface = accLoc->GetSurface();

   POSITION pos = m_polyList.GetHeadPosition();
   while (pos)
   {
      CPolyListItem &listItem = m_polyList.GetNext(pos);
      
      if (listItem.GetSurface() != surface)
         continue;

      if (listItem.IsPointInside(location))
      {
         shortestDistanceToComp = 0.0;
         break;
      }

      double dist = listItem.DistanceTo(location);
      if (dist < shortestDistanceToComp)
         shortestDistanceToComp = dist;
   }

   return (shortestDistanceToComp < minDistToOutline);
#else
   return (accLoc->GetClosestDistance() < minDistToOutline);
#endif
}

double CProbePlacementSolution::findDistanceToClosestComponentOutline(CPPAccessibleLocation *accLoc)
{
   double shortestDistanceToComp = DBL_MAX;
   CPoint2d location = accLoc->GetLocation();
   ETestSurface surface = accLoc->GetSurface();

   POSITION pos = m_polyList.GetHeadPosition();
   while (pos)
   {
      CPolyListItem &listItem = m_polyList.GetNext(pos);
      
      if (listItem.GetSurface() != surface)
         continue;

      if (listItem.IsPointInside(location))
      {
         shortestDistanceToComp = 0.0;
         break;
      }

      double dist = listItem.DistanceTo(location);
      if (dist < shortestDistanceToComp)
         shortestDistanceToComp = dist;
   }

   return (shortestDistanceToComp);
}

void CProbePlacementSolution::DrawOutlines()
{
   POSITION pos = m_pDoc->GetFirstViewPosition();
   CCAMCADView *view = (CCAMCADView*)m_pDoc->GetNextView(pos);
   CClientDC dc(view);
   view->OnPrepareDC(&dc);

   CPen topPen(PS_SOLID, 0, RGB(255, 255, 255));
   CPen botPen(PS_SOLID, 0, RGB(255, 0, 0));

   CGdiObject *oldBrush = dc.SelectStockObject(HOLLOW_BRUSH);
   pos = m_polyList.GetHeadPosition();
   while (pos)
   {
      CPolyListItem &listItem = m_polyList.GetNext(pos);

      HGDIOBJ oldPen = dc.SelectObject((listItem.GetSurface()==testSurfaceTop)?topPen:botPen);

      POSITION polyPos = listItem.GetPolyList().GetHeadPosition();
      while (polyPos)
      {
         CPoly *poly = listItem.GetPolyList().GetNext(polyPos);
         POSITION pntPos = poly->getPntList().GetHeadPosition();
         CPnt *pnt = poly->getPntList().GetNext(pntPos);
         dc.MoveTo((int)(pnt->x * view->getScaleFactor()), (int)(pnt->y * view->getScaleFactor()));
         while (pntPos)
         {
            pnt = poly->getPntList().GetNext(pntPos);
            dc.LineTo((int)(pnt->x * view->getScaleFactor()), (int)(pnt->y * view->getScaleFactor()));
         }
      }

      dc.SelectObject(oldPen);
   }
   dc.SelectObject(oldBrush);
}

void CProbePlacementSolution::resetAccessPointDistances()
{
   int netcount =  m_ppNets.GetCount();
   int netindx = 0;

   if (m_pProgressDlg != NULL)
   {
      m_pProgressDlg->SetPos(0);
      m_pProgressDlg->SetRange32((long)0, (long)100);
   }

   POSITION netPos = m_ppNets.GetStartPosition();
   while (netPos)
   {        
      CString netName;
      CPPNet *ppNet = NULL;
      netindx++;
      m_ppNets.GetNextAssoc(netPos, netName, ppNet);

      int percentdone = Round(100.0 * ((double)netindx / (double)netcount));
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->SetPos(min(percentdone + 1, 99));

      if (ppNet->GetNoProbeFlag())
         continue;


      POSITION sourceAccPos = ppNet->GetHeadPosition_AccessibleLocations();
      while (sourceAccPos)
      {
         CPPAccessibleLocation *sourceAccLoc = ppNet->GetNext_AccessibleLocations(sourceAccPos);
         if (m_pTestPlan->GetProbeSide() == testSurfaceBoth || sourceAccLoc->GetSurface() == m_pTestPlan->GetProbeSide())
         {
            sourceAccLoc->SetClosestDistance(findDistanceToClosestComponentOutline(sourceAccLoc));
         }
      }
   }

   if (m_pProgressDlg != NULL)
   {
      m_pProgressDlg->SetRange32((long)0, (long)(1 + 1 + 8));
      m_pProgressDlg->SetPos(1);
   }
}

int CProbePlacementSolution::optimizeProbeLocations(double diameter, double minDistToOutline, bool removeProbeIfNeeded)
{
   int moveCount = 0;
   POSITION netPos = m_ppNets.GetStartPosition();
   while (netPos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(netPos, netName, ppNet);

      if (ppNet->GetNoProbeFlag())
         continue;

      POSITION sourceAccPos = ppNet->GetHeadPosition_AccessibleLocations();
      while (sourceAccPos)
      {
         POSITION curSourceAccPos = sourceAccPos;
         CPPAccessibleLocation *sourceAccLoc = ppNet->GetNext_AccessibleLocations(sourceAccPos);

         if (sourceAccLoc->GetSurface() != m_pTestPlan->GetProbeSide() && m_pTestPlan->GetProbeSide() != testSurfaceBoth)
            continue;
         if (sourceAccLoc->GetPlaceableProbe() == NULL)
            continue;
         if (m_pTestPlan->GetUseExistingProbes() && m_pTestPlan->RetainProbePositions() && sourceAccLoc->GetPlaceableProbe()->DidExist())
            continue;
         if (sourceAccLoc->GetFeatureCondition() == featureConditionForced)
            continue;

         bool sourceTooCloseToComponent = minDistToOutline > 0.0 && isTooCloseToComponentOutline(sourceAccLoc, minDistToOutline);

         CAccessibleLocationList foundList;
         int sourceDensity = getProbeDensity(sourceAccLoc->GetLocation(), diameter, sourceAccLoc->GetSurface(), foundList);

         POSITION destAccPos = ppNet->GetHeadPosition_AccessibleLocations();
         while (destAccPos)
         {
            if (curSourceAccPos == destAccPos)
            {
               ppNet->GetNext_AccessibleLocations(destAccPos);
               continue;
            }

            CPPAccessibleLocation *destAccLoc = ppNet->GetNext_AccessibleLocations(destAccPos);

            if (destAccLoc->GetSurface() != m_pTestPlan->GetProbeSide() && m_pTestPlan->GetProbeSide() != testSurfaceBoth)
               continue;
            if (destAccLoc->GetSurface() == testSurfaceTop && !haveTopProbes ||
               destAccLoc->GetSurface() == testSurfaceBottom && !haveBotProbes)
               continue;
            if (destAccLoc->GetPlaceableProbe() != NULL)
               continue;
            if (minDistToOutline > 0.0 && isTooCloseToComponentOutline(destAccLoc, minDistToOutline))
               continue;
            if (!isValidTargetType(destAccLoc))
               continue;
            if (destAccLoc->GetFeatureCondition() != featureConditionProbeable)
               continue;
            
            // Side preference takes precedence over probe size, over everything.
            // But only when doing probes on "both" sides.
            // If source probe is on preferred side and dest probe is not, do not
            // move the probe, unless the source probe is too close to a component.
            if (m_pTestPlan->GetProbeSide() == testSurfaceBoth)
            {
               ETestSurface sidePref = m_pTestPlan->GetProbeSidePreference();
               if (!sourceTooCloseToComponent &&
                  sourceAccLoc->GetSurface() == sidePref &&
                  destAccLoc->GetSurface() != sidePref)
                  continue;
            }

            CAccessibleLocationList foundList;
            int destDensity = getProbeDensity(destAccLoc->GetLocation(), diameter, destAccLoc->GetSurface(), foundList);

            if (sourceDensity - 1 > destDensity)
            {
               // move the probe
               CPPProbe *probe = sourceAccLoc->RemoveProbe();
               destAccLoc->PlaceProbe(probe);

               moveCount++;
               break;
            }
         }

         // remove the probe if we have to and if 
         if (sourceTooCloseToComponent && sourceAccLoc->GetPlaceableProbe() != NULL && removeProbeIfNeeded)
         {
            sourceAccLoc->RemoveProbe();
            moveCount++;
         }
      }
   }

   return moveCount;
}

CDFTProbeTemplate *CProbePlacementSolution::getOptimalProbeSize(CPPAccessibleLocation *accLoc)
{
   ETestSurface surface = accLoc->GetSurface();
   POSITION pTempPos = NULL;

   CDFTProbeTemplates &pTemplates = m_pTestPlan->GetProbes();
   if (surface == testSurfaceTop && m_pTestPlan->CanProbeTopSide())
   {
      POSITION templatePos = pTemplates.GetHeadPosition_TopProbes();
      while (templatePos)
      {
         CDFTProbeTemplate *pTemplate = pTemplates.GetNext_TopProbes(templatePos);
         if (!pTemplate->GetUseFlag())
            continue;

         double probeSize = pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
         if (pTemplate->GetUseComponentOutline())
         {
            double minCompDist = pTemplate->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            if (isTooCloseToComponentOutline(accLoc, minCompDist))
               continue;
         }

         // return this as the current probe template
         return pTemplate;
      }
   }
   else if (surface == testSurfaceBottom && m_pTestPlan->CanProbeBotSide())
   {
      POSITION templatePos = pTemplates.GetHeadPosition_BotProbes();
      while (templatePos)
      {
         CDFTProbeTemplate *pTemplate = pTemplates.GetNext_BotProbes(templatePos);
         if (!pTemplate->GetUseFlag())
            continue;

         double probeSize = pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
         if (pTemplate->GetUseComponentOutline())
         {
            double minCompDist = pTemplate->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            if (isTooCloseToComponentOutline(accLoc, minCompDist))
               continue;
         }

         // return this as the current probe template
         return pTemplate;
      }
   }

   return NULL;
}

void CProbePlacementSolution::getOptimalProbeCombination(CPPAccessibleLocation *accLoc1, CPPAccessibleLocation *accLoc2, CDFTProbeTemplate *&template1, CDFTProbeTemplate *&template2)
{
   ETestSurface surface = accLoc1->GetSurface();
   POSITION pTempPos = NULL;

	// In case 1729 data, we get a better result if we always let this function select
	// both templates, but doing so essentially disables a lot of logic in the
	// "maximize" call. This needs more study.
   // (Later...) In case 2029 we also get better result (the required result to close the
   // case) if we let this function pick both templates all the time.
   // It is not clear if this harms anything. Results seem to look good. Time will tell.
	template1 = template2 = NULL;

   if (template1 != NULL && template2 != NULL)
      return;

   // NOTE: possible optimization to precalculate the cost of every combinations of probes
   int curTotalCost = 0;

   CDFTProbeTemplates &pTemplates = m_pTestPlan->GetProbes();
   if (surface == testSurfaceTop && m_pTestPlan->CanProbeTopSide())
   {
      if (template1 == NULL && template2 == NULL)
      {
         POSITION templatePos1 = pTemplates.GetHeadPosition_TopProbes();
         while (templatePos1)
         {
            POSITION curTemplatePos = templatePos1;
            CDFTProbeTemplate *probeTemplate1 = pTemplates.GetNext_TopProbes(templatePos1);
            if (!probeTemplate1->GetUseFlag())
               continue;

            double probeSize1 = probeTemplate1->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            if (probeTemplate1->GetUseComponentOutline())
            {
               double minCompDist = probeTemplate1->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
               if (isTooCloseToComponentOutline(accLoc1, minCompDist))
                  continue;
            }

            POSITION templatePos2 = curTemplatePos;
            while (templatePos2)
            {
               CDFTProbeTemplate *probeTemplate2 = pTemplates.GetNext_TopProbes(templatePos2);
               if (!probeTemplate2->GetUseFlag())
                  continue;

               double probeSize2 = probeTemplate2->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
               if (probeTemplate2->GetUseComponentOutline())
               {
                  double minCompDist = probeTemplate2->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
                  if (isTooCloseToComponentOutline(accLoc2, minCompDist))
                     continue;
               }

               int cost = probeTemplate1->GetCost() + probeTemplate2->GetCost();
               double minDistance = (probeSize1 + probeSize2) / 2;

               // set this as the current if the current cost is less than the last set
               if (minDistance < accLoc1->GetLocation().distance(accLoc2->GetLocation()) &&
                  (template1 == NULL || template2 == NULL  || curTotalCost > cost))
               {
                  template1 = probeTemplate1;
                  template2 = probeTemplate2;
                  curTotalCost = cost;
               }
            }
         }
      }
      else if (template1 == NULL && template2 != NULL)
      {
         CDFTProbeTemplate *probeTemplate2 = template2;

         POSITION templatePos = pTemplates.GetHeadPosition_TopProbes();
         while (templatePos)
         {
            CDFTProbeTemplate *probeTemplate1 = pTemplates.GetNext_TopProbes(templatePos);
            if (!probeTemplate1->GetUseFlag())
               continue;

            double probeSize1 = probeTemplate1->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            double probeSize2 = probeTemplate2->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            if (probeTemplate2->GetUseComponentOutline())
            {
               double minCompDist = probeTemplate2->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
               if (isTooCloseToComponentOutline(accLoc2, minCompDist))
                  continue;
            }

            int cost = probeTemplate1->GetCost() + probeTemplate2->GetCost();
            double minDistance = (probeSize1 + probeSize2) / 2;

            // set this as the current if the current cost is less than the last set
            if (minDistance < accLoc1->GetLocation().distance(accLoc2->GetLocation()) &&
               (template1 == NULL || curTotalCost > cost))
            {
               template1 = probeTemplate1;
               curTotalCost = cost;
            }
         }
      }
      else if (template1 != NULL && template2 == NULL)
      {
         CDFTProbeTemplate *probeTemplate1 = template1;

         POSITION templatePos = pTemplates.GetHeadPosition_TopProbes();
         while (templatePos)
         {
            CDFTProbeTemplate *probeTemplate2 = pTemplates.GetNext_TopProbes(templatePos);
            if (!probeTemplate2->GetUseFlag())
               continue;

            double probeSize1 = probeTemplate1->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            double probeSize2 = probeTemplate2->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            if (probeTemplate1->GetUseComponentOutline())
            {
               double minCompDist = probeTemplate1->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
               if (isTooCloseToComponentOutline(accLoc1, minCompDist))
                  continue;
            }

            int cost = probeTemplate1->GetCost() + probeTemplate2->GetCost();
            double minDistance = (probeSize1 + probeSize2) / 2;

            // set this as the current if the current cost is less than the last set
            if (minDistance < accLoc1->GetLocation().distance(accLoc2->GetLocation()) &&
               (template2 == NULL || curTotalCost > cost))
            {
               template2 = probeTemplate2;
               curTotalCost = cost;
            }
         }
      }
   }
   else if (surface == testSurfaceBottom && m_pTestPlan->CanProbeBotSide())
   {
      if (template1 == NULL && template2 == NULL)
      {
         POSITION templatePos1 = pTemplates.GetHeadPosition_BotProbes();
         while (templatePos1)
         {
            POSITION curTemplatePos = templatePos1;
            CDFTProbeTemplate *probeTemplate1 = pTemplates.GetNext_BotProbes(templatePos1);
            if (!probeTemplate1->GetUseFlag())
               continue;

            double probeSize1 = probeTemplate1->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            if (probeTemplate1->GetUseComponentOutline())
            {
               double minCompDist = probeTemplate1->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
               if (isTooCloseToComponentOutline(accLoc1, minCompDist))
                  continue;
            }

            POSITION templatePos2 = curTemplatePos;
            while (templatePos2)
            {
               CDFTProbeTemplate *probeTemplate2 = pTemplates.GetNext_BotProbes(templatePos2);
               if (!probeTemplate2->GetUseFlag())
                  continue;

               double probeSize2 = probeTemplate2->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
               if (probeTemplate2->GetUseComponentOutline())
               {
                  double minCompDist = probeTemplate2->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
                  if (isTooCloseToComponentOutline(accLoc2, minCompDist))
                     continue;
               }

               int cost = probeTemplate1->GetCost() + probeTemplate2->GetCost();
               double minDistance = (probeSize1 + probeSize2) / 2;

               // set this as the current if the current cost is less than the last set
               if (minDistance < accLoc1->GetLocation().distance(accLoc2->GetLocation()) &&
                  (template1 == NULL || template2 == NULL || curTotalCost > cost))
               {
                  template1 = probeTemplate1;
                  template2 = probeTemplate2;
                  curTotalCost = cost;
               }
            }
         }
      }
      else if (template1 == NULL && template2 != NULL)
      {
         CDFTProbeTemplate *probeTemplate2 = template2;

         POSITION templatePos = pTemplates.GetHeadPosition_BotProbes();
         while (templatePos)
         {
            CDFTProbeTemplate *probeTemplate1 = pTemplates.GetNext_BotProbes(templatePos);
            if (!probeTemplate1->GetUseFlag())
               continue;

            double probeSize1 = probeTemplate1->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            double probeSize2 = probeTemplate2->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            if (probeTemplate2->GetUseComponentOutline())
            {
               double minCompDist = probeTemplate2->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
               if (isTooCloseToComponentOutline(accLoc2, minCompDist))
                  continue;
            }

            int cost = probeTemplate1->GetCost() + probeTemplate2->GetCost();
            double minDistance = (probeSize1 + probeSize2) / 2;

            // set this as the current if the current cost is less than the last set
            if (minDistance < accLoc1->GetLocation().distance(accLoc2->GetLocation()) &&
               (template1 == NULL || curTotalCost > cost))
            {
               template1 = probeTemplate1;
               curTotalCost = cost;
            }
         }
      }
      else if (template1 != NULL && template2 == NULL)
      {
         CDFTProbeTemplate *probeTemplate1 = template1;

         POSITION templatePos = pTemplates.GetHeadPosition_BotProbes();
         while (templatePos)
         {
            CDFTProbeTemplate *probeTemplate2 = pTemplates.GetNext_BotProbes(templatePos);
            if (!probeTemplate2->GetUseFlag())
               continue;

            double probeSize1 = probeTemplate1->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            double probeSize2 = probeTemplate2->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            if (probeTemplate1->GetUseComponentOutline())
            {
               double minCompDist = probeTemplate1->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
               if (isTooCloseToComponentOutline(accLoc1, minCompDist))
                  continue;
            }

            int cost = probeTemplate1->GetCost() + probeTemplate2->GetCost();
            double minDistance = (probeSize1 + probeSize2) / 2;

            // set this as the current if the current cost is less than the last set
            if (minDistance < accLoc1->GetLocation().distance(accLoc2->GetLocation()) &&
               (template2 == NULL || curTotalCost > cost))
            {
               template2 = probeTemplate2;
               curTotalCost = cost;
            }
         }
      }
   }
}

int CProbePlacementSolution::resurrectNonViolatingUnplacedProbes()
{
   int resurrectionCount = 0;

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

         if (probe && !probe->IsPlaced())
         {
            bool beenPlaced = false;

            POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations();
            while (accPos && !beenPlaced)
            {
               CPPAccessibleLocation *accLoc = ppNet->GetNext_AccessibleLocations(accPos);

               if (accLoc->GetSurface() != m_pTestPlan->GetProbeSide() && m_pTestPlan->GetProbeSide() != testSurfaceBoth)
                  continue;

               if (accLoc->GetPlaceableProbe() == NULL &&
                  accLoc->GetFeatureCondition() != featureConditionNoProbe)
               {

                  POSITION dummyPos = NULL;
                  CDFTProbeTemplate *pTemplate = NULL;

                  // Try the templates from largest to smallest

                  ETestSurface surface = accLoc->GetSurface();
                  CDFTProbeTemplates &pTemplates = m_pTestPlan->GetProbes();

                  POSITION templatePos = NULL;

                  if (surface == testSurfaceTop && m_pTestPlan->CanProbeTopSide())
                     templatePos = pTemplates.GetHeadPosition_TopProbes();
                  else if (surface == testSurfaceBottom && m_pTestPlan->CanProbeBotSide())
                     templatePos = pTemplates.GetHeadPosition_BotProbes();

#ifdef TRY_LARGEST_TO_SMALLEST
                  while (templatePos && !beenPlaced)
#endif
                  {
                     CDFTProbeTemplate *pTemplate = NULL;

#ifdef TRY_LARGEST_TO_SMALLEST
                     if (surface == testSurfaceTop)
                        pTemplate = pTemplates.GetNext_TopProbes(templatePos);
                     else if (surface == testSurfaceBottom)
                        pTemplate = pTemplates.GetNext_BotProbes(templatePos);
#else
                     // just try smallest
                     if (surface == testSurfaceTop)
                        pTemplate = pTemplates.GetSmallest_UsedTopProbes(templatePos);
                     else if (surface == testSurfaceBottom)
                        pTemplate = pTemplates.GetSmallest_UsedBotProbes(templatePos);
                     
#endif


                     if (pTemplate && pTemplate->GetUseFlag())
                     {
                        double probeDiameter = pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());

                        bool componentViolation = false;

                        if (pTemplate->GetUseComponentOutline())
                        {
                           double minDistToOutline = pTemplate->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
                           componentViolation = isTooCloseToComponentOutline(accLoc, minDistToOutline);
                        }

                        if (!componentViolation)
                        {           
                           CAccessibleLocationList foundList;
                           int probeDensity = getActualProbeDensity(accLoc->GetLocation(), probeDiameter, accLoc->GetSurface(), foundList);

                           if (probeDensity > 0)
                           {
                              // Minimize size of all violators
                              // (Last chance maximize will bring them back up if possible.)
                              POSITION foundpos = foundList.GetHeadPosition();
                              while (foundpos)
                              {
                                 CQfeAccessibleLocation *qfeAccLoc = foundList.GetNext(foundpos);

                                 CPPAccessibleLocation *neighborAccLoc = qfeAccLoc->GetAccessibleLocation();
                                 if (neighborAccLoc->GetPlaceableProbe() != NULL)
                                 {
                                    CPPProbe *nprobe = neighborAccLoc->GetPlaceableProbe();
                                    nprobe->SetProbeTemplate(pTemplate);
                                 }
                              }
                           }

                           // Now try again
                           probeDensity = getActualProbeDensity(accLoc->GetLocation(), probeDiameter, accLoc->GetSurface(), foundList);

                           if (probeDensity == 0)
                           {
                              accLoc->PlaceProbe(probe);
                              probe->SetProbeTemplate(pTemplate);
                              beenPlaced = true;
                              resurrectionCount++;
                           }

                        }

                     }
                  }
               }
            }
         }
      }
   }
   return resurrectionCount;
}

int CProbePlacementSolution::lastChanceMaximizeProbeSize(ETestSurface surface)
{
   // We want to try to evenly grow the probe sizes. That's why the
   // probe template loop is on the outside, rather than the inside.
   // It it is better to grow two probes to 75 than to grow one to
   // 100 leaving the other at 50.

   int changeCount = 0;

   CDFTProbeTemplates &pTemplates = m_pTestPlan->GetProbes();

   POSITION templatePos = NULL;

   // The check for side enabled in test plan is removed because Forced Probes may be
   // on a side that is "not enabled", but we still want to maximize them.
   // All we lose is a little time looping just to find there are no probes on a given side.
   // See test case dts0100380281.
   if (surface == testSurfaceTop) //  && m_pTestPlan->CanProbeTopSide())
      templatePos = pTemplates.GetTailPosition_TopProbes();
   else if (surface == testSurfaceBottom ) //  && m_pTestPlan->CanProbeBotSide())
      templatePos = pTemplates.GetTailPosition_BotProbes();

   while (templatePos)
   {
      CDFTProbeTemplate *pTemplate = NULL;


      if (surface == testSurfaceTop)
         pTemplate = pTemplates.GetPrev_TopProbes(templatePos);
      else if (surface == testSurfaceBottom)
         pTemplate = pTemplates.GetPrev_BotProbes(templatePos);

      if (pTemplate && pTemplate->GetUseFlag())
      {

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

               if (probe && probe->IsPlaced())
               {
                  CString probe_templateName = probe->GetProbeTemplateName();

                  CPPAccessibleLocation *accLoc = probe->GetAccessibleLocation();

                  if (accLoc && accLoc->GetSurface() == surface &&
                     accLoc->GetPlaceableProbe() != NULL)
                  {
                     POSITION dummyPos = NULL;
                     CDFTProbeTemplate *pExistingTemplate = pTemplates.Find_Probe(probe->GetProbeTemplateName(), dummyPos, accLoc->GetSurface());

							double existing_probe_diameter = 0.0;
                     if (pExistingTemplate)
                        existing_probe_diameter = pExistingTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
                        
							double new_probe_diameter = pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
                        
                     if (new_probe_diameter > existing_probe_diameter)
                     {
                        CAccessibleLocationList dummyList;
                        int density = getActualProbeDensity(accLoc->GetLocation(), new_probe_diameter, surface, dummyList);
                        if (density <= 1) {
                           probe->SetProbeTemplate(pTemplate);
                           changeCount++;
                        }
                     }
                  }
               }
            }
         }
      }
   }

   return changeCount;
}



int CProbePlacementSolution::maximizeProbeSizes()
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
         CPPAccessibleLocation *accLoc = ppNet->GetNext_AccessibleLocations(accPos);

         if (accLoc->GetSurface() != m_pTestPlan->GetProbeSide() && m_pTestPlan->GetProbeSide() != testSurfaceBoth)
            continue;
         if (accLoc->GetPlaceableProbe() == NULL)
            continue;
         if (m_pTestPlan->RetainProbeSizes() && accLoc->GetPlaceableProbe()->DidExist())
            continue;

         POSITION dummyPos = NULL;
         CDFTProbeTemplate *largestProbeTemplate = NULL;
         if (accLoc->GetSurface() == testSurfaceTop)
            largestProbeTemplate = m_pTestPlan->GetProbes().GetLargest_UsedTopProbes(dummyPos);
         else if (accLoc->GetSurface() == testSurfaceBottom)
            largestProbeTemplate = m_pTestPlan->GetProbes().GetLargest_UsedBotProbes(dummyPos);

         double testDiameter = largestProbeTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());

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
         CPPAccessibleLocation *closestAccLoc = NULL;
         CDFTProbeTemplate *closestAccTemplate = NULL;
         double closestDistance = DBL_MAX;
         POSITION foundPos = foundList.GetHeadPosition();
         while (foundPos)
         {
            CQfeAccessibleLocation *qfeLoc = foundList.GetNext(foundPos);
            CPPAccessibleLocation *qfeAccLoc = qfeLoc->GetAccessibleLocation();

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
            if ((m_pTestPlan->RetainProbeSizes() && !accLoc->GetPlaceableProbe()->DidExist()) || !m_pTestPlan->RetainProbeSizes())
            {
               if (accLoc->GetPlaceableProbe()->HasProbeTemplate() && temp1 != NULL)
               {
                  if (temp1->GetDiameter() < accTemplate->GetDiameter())
                     accLoc->GetPlaceableProbe()->SetProbeTemplate(temp1);
               }
               else
                  accLoc->GetPlaceableProbe()->SetProbeTemplate(temp1);
            }

            if ((m_pTestPlan->RetainProbeSizes() && !closestAccLoc->GetPlaceableProbe()->DidExist()) || !m_pTestPlan->RetainProbeSizes())
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
                  
                  if (density > highestDensity && !(m_pTestPlan->RetainProbeSizes() && qfeAccLoc->GetPlaceableProbe()->DidExist()))
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
            else if ((temp1 == NULL && m_pTestPlan->RetainProbeSizes() && closestAccLoc->GetPlaceableProbe()->DidExist()) ||
                     (temp2 == NULL && m_pTestPlan->RetainProbeSizes() && accLoc->GetPlaceableProbe()->DidExist()))
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
}

int CProbePlacementSolution::fixViolatingProbeTemplates()
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
         CPPAccessibleLocation *accLoc = probe->GetAccessibleLocation();
         POSITION dummyPos = NULL;

         if (accLoc == NULL)
            continue;

         if (probe->HasProbeTemplate())
         {
            POSITION ptPos = NULL;
            CDFTProbeTemplate *largestProbeTemplate = NULL;
            if (accLoc->GetSurface() == testSurfaceTop)
               largestProbeTemplate = m_pTestPlan->GetProbes().GetLargest_UsedTopProbes(dummyPos);
            else if (accLoc->GetSurface() == testSurfaceBottom)
               largestProbeTemplate = m_pTestPlan->GetProbes().GetLargest_UsedBotProbes(dummyPos);
            double testDiameter = largestProbeTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());

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
               CPPAccessibleLocation *qfeAccLoc = qfeLoc->GetAccessibleLocation();

               if (qfeAccLoc->GetPlaceableProbe() == NULL)
                  continue;

               if (!qfeAccLoc->GetPlaceableProbe()->HasProbeTemplate())
                  continue;

               if (accLoc == qfeAccLoc)
                  continue;

               POSITION dummyPos2 = NULL;
               CDFTProbeTemplate *qfeProbeTemplate = m_pTestPlan->GetProbes().Find_Probe(qfeAccLoc->GetPlaceableProbe()->GetProbeTemplateName(), dummyPos2, accLoc->GetSurface());

               double distance = accLoc->GetLocation().distance(qfeAccLoc->GetLocation());
               double probeRadius = probeTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits()) / 2.0;
               double qfeRadius = qfeProbeTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits()) / 2.0;

               if (distance < (probeRadius + qfeRadius))
               {
                  CDFTProbeTemplate *temp1 = (m_pTestPlan->RetainProbeSizes() && accLoc->GetPlaceableProbe()->DidExist())?probeTemplate:NULL,
                                    *temp2 = (m_pTestPlan->RetainProbeSizes() && qfeAccLoc->GetPlaceableProbe()->DidExist())?qfeProbeTemplate:NULL;

                  getOptimalProbeCombination(accLoc, qfeAccLoc, temp1, temp2);
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

                  if (qfeAccLoc->GetPlaceableProbe()->HasProbeTemplate() && temp2 != NULL)
                  {
                     if (temp2->GetDiameter() < qfeProbeTemplate->GetDiameter())
                        qfeAccLoc->GetPlaceableProbe()->SetProbeTemplate(temp2);
                  }
                  else
                     qfeAccLoc->GetPlaceableProbe()->SetProbeTemplate(temp2);

                  violationCount++;
               } // END if (distance < (probeRadius + qfeRadius)) ...
            } // END while (foundPos) ...

            if (probeTemplate == NULL && !(m_pTestPlan->RetainProbeSizes() && accLoc->GetPlaceableProbe()->DidExist()))
            {
               accLoc->RemoveProbe();
               break;
            }
         } // END if (probe->HasProbeTemplate()) ...
         else
         {
            accLoc->RemoveProbe();
         }
      } // END while (probePos) ...
   } // END while (netPos) ...

   return violationCount;
}

int CProbePlacementSolution::removeTemplatesFromProbes()
{
   // Leave templates on forced probes, remove the rest.

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
         CPPAccessibleLocation *accLoc = probe->GetAccessibleLocation();

         if (accLoc != NULL && accLoc->GetFeatureCondition() != featureConditionForced && probe->HasProbeTemplate() &&
            !(m_pTestPlan->RetainProbeSizes() && accLoc->GetPlaceableProbe()->DidExist()) )
         {
            probe->SetProbeTemplate(NULL);
            removeCount++;
         }
      }
   }

   return removeCount;
}

int CProbePlacementSolution::removeProbesWithNoTemplate()
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
         CPPAccessibleLocation *accLoc = probe->GetAccessibleLocation();

         if (accLoc != NULL && !probe->HasProbeTemplate())
         {
            accLoc->RemoveProbe();
            removeCount++;
         }
      }
   }

   return removeCount;
}

void CProbePlacementSolution::setSmallestProbeSizes()
{
   POSITION pos = NULL;
   CDFTProbeTemplate *pTemplateTop = m_pTestPlan->GetProbes().GetTail_TopProbes();
   CDFTProbeTemplate *pTemplateBot = m_pTestPlan->GetProbes().GetTail_BotProbes();

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
         CPPAccessibleLocation *accLoc = probe->GetAccessibleLocation();

         if (accLoc != NULL)
         {
            if (accLoc->GetSurface() == testSurfaceTop || accLoc->GetSurface() == testSurfaceBoth)
               probe->SetProbeTemplate(pTemplateTop);
            else if (accLoc->GetSurface() == testSurfaceBottom)
               probe->SetProbeTemplate(pTemplateBot);
         }
      }
   }
}

void CProbePlacementSolution::doubleWireRemainingProbes()
{
   if (!m_pTestPlan->GetAllowDoubleWiring())
      return;

   POSITION pos = m_ppNets.GetStartPosition();
   while (pos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(pos, netName, ppNet);

      if (ppNet->GetNoProbeFlag())
         continue;

      POSITION probePos = ppNet->GetHeadPosition_Probes();
      while (probePos)
      {
         CPPProbe *probe = ppNet->GetNext_Probes(probePos);

         if (probe->GetAccessibleLocation() != NULL)
            continue;

         POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations();
         while (accPos)
         {
            CPPAccessibleLocation *accLoc = ppNet->GetNext_AccessibleLocations(accPos);
            CPPProbe *placedProbe = accLoc->GetPlaceableProbe();

            if (placedProbe == NULL)
               continue;

            if (!accLoc->IsDoubleWired())
            {
               POSITION pTempPos = NULL;
               accLoc->PlaceDoubleWiredProbe(probe);
               probe->SetProbeTemplate(m_pTestPlan->GetProbes().Find_Probe(placedProbe->GetProbeTemplateName(), pTempPos, accLoc->GetSurface()));
               break;
            }
         }
      }
   } // END while (pos) ...
}

CProbePlacementSolution& CProbePlacementSolution::operator=(const CProbePlacementSolution &ppSolution)
{
   if (&ppSolution != this)
   {
      m_pDoc = ppSolution.m_pDoc;
      m_pTestPlan = ppSolution.m_pTestPlan;
      m_pFile = ppSolution.m_pFile;
      
      RemoveAll_Nets();
      POSITION pos = ppSolution.m_ppNets.GetStartPosition();
      while (pos)
      {
         CString netName;
         CPPNet *ppNet = NULL;
         ppSolution.m_ppNets.GetNextAssoc(pos, netName, ppNet);
         
         m_ppNets.SetAt(netName, new CPPNet(*ppNet));
      }
      
      m_polyList.RemoveAll();
      pos = ppSolution.m_polyList.GetHeadPosition();
      while (pos)
      {
         m_polyList.AddTail(ppSolution.m_polyList.GetNext(pos));
      }

      // TODO: copy net map (???)
      // TODO: copy Qfe extents lookup (???)
   }

   return *this;
}

POSITION CProbePlacementSolution::GetHeadPosition_Probes(CString netName) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetHeadPosition_Probes();
}

POSITION CProbePlacementSolution::GetTailPosition_Probes(CString netName) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetTailPosition_Probes();
}

CPPProbe *CProbePlacementSolution::GetHead_Probes(CString netName) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetHead_Probes();
}

CPPProbe *CProbePlacementSolution::GetTail_Probes(CString netName) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetTail_Probes();
}

CPPProbe *CProbePlacementSolution::GetNext_Probes(CString netName, POSITION &pos) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetNext_Probes(pos);
}

CPPProbe *CProbePlacementSolution::GetPrev_Probes(CString netName, POSITION &pos) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetPrev_Probes(pos);
}

POSITION CProbePlacementSolution::GetHeadPosition_AccessibleLocations(CString netName) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetHeadPosition_AccessibleLocations();
}

POSITION CProbePlacementSolution::GetTailPosition_AccessibleLocations(CString netName) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetTailPosition_AccessibleLocations();
}

CPPAccessibleLocation *CProbePlacementSolution::GetHead_AccessibleLocations(CString netName) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetHead_AccessibleLocations();
}

CPPAccessibleLocation *CProbePlacementSolution::GetTail_AccessibleLocations(CString netName) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetTail_AccessibleLocations();
}

CPPAccessibleLocation *CProbePlacementSolution::GetPrev_AccessibleLocations(CString netName, POSITION &pos) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetPrev_AccessibleLocations(pos);
}

CPPAccessibleLocation *CProbePlacementSolution::GetNext_AccessibleLocations(CString netName, POSITION &pos, EEntityType featureType, CString targetType, ETestSurface surface) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;

   CPPAccessibleLocation *pAccLoc = ppNet->GetNext_AccessibleLocations(pos);

   while (pos)
   {
      CPPAccessibleLocation *pNextAccLoc = ppNet->GetAt_AccessibleLocations(pos);

      if ((featureType < entityTypeMIN || pNextAccLoc->getFeatureEntity().getEntityType() == featureType) &&
         (targetType.IsEmpty() || pNextAccLoc->GetTargetType().Find(targetType) > 0) &&
         (netName.IsEmpty() || pNextAccLoc->GetNetName() == netName) &&
         (surface < testSurfaceMIN || pNextAccLoc->GetSurface() == surface))
         break;

      ppNet->GetNext_AccessibleLocations(pos);
   }

   return pAccLoc;
}

CPPProbe *CProbePlacementSolution::AddHead_Probes(CString netName, ETestResourceType resType, CString probeRefname)
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
   {
      ppNet = new CPPNet(m_pDoc, netName);
      m_ppNets.SetAt(netName, ppNet);
   }
   
   if (probeRefname.IsEmpty())
      probeRefname = m_probeNamer.GetNextProbeRefname(ppNet, resType);

   return ppNet->AddHead_Probes(resType, netName, probeRefname);
}

CPPProbe *CProbePlacementSolution::AddTail_Probes(CString netName, ETestResourceType resType, CString probeRefname)
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
   {
      ppNet = new CPPNet(m_pDoc, netName);
      m_ppNets.SetAt(netName, ppNet);
   }
   
   if (probeRefname.IsEmpty())
      probeRefname = m_probeNamer.GetNextProbeRefname(ppNet, resType);

   return ppNet->AddTail_Probes(resType, netName, probeRefname);
}

void CProbePlacementSolution::RemoveAll_Probes(CString netName)
{
   if (netName.IsEmpty())
   {
      // remove all probes
      POSITION netPos = m_ppNets.GetStartPosition();
      while (netPos)
      {
         CString netName;
         CPPNet *ppNet = NULL;
         m_ppNets.GetNextAssoc(netPos, netName, ppNet);

         ppNet->RemoveAll_Probes();
         //if (ppNet->GetCount_AccessibleLocations() == 0)
         //{
         // delete ppNet;
         // m_ppNets.RemoveKey(netName);
         //}
      }
   }
   else
   {
      // remove all probes under a net
      CPPNet *ppNet = NULL;
      if (!m_ppNets.Lookup(netName, ppNet))
         return;

      ppNet->RemoveAll_Probes();
      //if (ppNet->GetCount_AccessibleLocations() == 0)
      //{
      // delete ppNet;
      // m_ppNets.RemoveKey(netName);
      //}
   }
}

CPPAccessibleLocation *CProbePlacementSolution::AddHead_AccessibleLocations(DataStruct *testAccessData)
{
   // get the net name
   CString netName;
   Attrib *attrib = NULL;
   WORD netNameKW = m_pDoc->IsKeyWord(ATT_NETNAME, 0);
   if (!testAccessData->getAttributes()->Lookup(netNameKW, attrib))
      return NULL;
   netName = get_attvalue_string(m_pDoc, attrib);

   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
   {
      ppNet = new CPPNet(m_pDoc, netName);
      m_ppNets.SetAt(netName, ppNet);
   }

   CPPAccessibleLocation *accessibleLocation = ppNet->AddHead_AccessibleLocations(testAccessData);

   if (accessibleLocation->GetSurface() == testSurfaceTop)
      m_LocationTreeTop.setAt(new CQfeAccessibleLocation(accessibleLocation));
   else if (accessibleLocation->GetSurface() == testSurfaceBottom)
      m_LocationTreeBot.setAt(new CQfeAccessibleLocation(accessibleLocation));

   return accessibleLocation;
}

CPPAccessibleLocation *CProbePlacementSolution::AddTail_AccessibleLocations(DataStruct *testAccessData)
{
   // get the net name
   CString netName;
   Attrib *attrib = NULL;
   WORD netNameKW = m_pDoc->IsKeyWord(ATT_NETNAME, 0);
   if (!testAccessData->getAttributes()->Lookup(netNameKW, attrib))
      return NULL;
   netName = get_attvalue_string(m_pDoc, attrib);

   //// make sure the net exists in the netlist
   //if (!FindNet(m_pFile, netName))
   // return NULL;

   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
   {
      return NULL;
      //ppNet = new CPPNet(m_pDoc, netName);
      //m_ppNets.SetAt(netName, ppNet);
   }

   CPPAccessibleLocation *accessibleLocation = ppNet->AddTail_AccessibleLocations(testAccessData);

   if (accessibleLocation->GetSurface() == testSurfaceTop)
      m_LocationTreeTop.setAt(new CQfeAccessibleLocation(accessibleLocation));
   else if (accessibleLocation->GetSurface() == testSurfaceBottom)
      m_LocationTreeBot.setAt(new CQfeAccessibleLocation(accessibleLocation));

   return accessibleLocation;
}

CPPAccessibleLocation *CProbePlacementSolution::AddTail_AccessibleLocations(CString netName,const CEntity& featureEntity, CPoint2d location, CString targetType, ETestSurface surface)
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
   {
      return NULL;
      //ppNet = new CPPNet(m_pDoc, netName);
      //m_ppNets.SetAt(netName, ppNet);
   }

   CPPAccessibleLocation *accessibleLocation = ppNet->AddTail_AccessibleLocations(featureEntity, location, targetType, netName, surface);

   if (accessibleLocation->GetSurface() == testSurfaceTop)
      m_LocationTreeTop.setAt(new CQfeAccessibleLocation(accessibleLocation));
   else if (accessibleLocation->GetSurface() == testSurfaceBottom)
      m_LocationTreeBot.setAt(new CQfeAccessibleLocation(accessibleLocation));

   return accessibleLocation;
}

void CProbePlacementSolution::RemoveAll_AccessibleLocations(CString netName)
{
   if (netName.IsEmpty())
   {
      // remove all probes
      POSITION netPos = m_ppNets.GetStartPosition();
      while (netPos)
      {
         CString netName;
         CPPNet *ppNet = NULL;
         m_ppNets.GetNextAssoc(netPos, netName, ppNet);

         ppNet->RemoveAll_AccessibleLocations();
         //if (ppNet->GetCount_Probes() == 0)
         //{
         // delete ppNet;
         // m_ppNets.RemoveKey(netName);
         //}
      }
   }
   else
   {
      // remove all probes under a net
      CPPNet *ppNet = NULL;
      if (!m_ppNets.Lookup(netName, ppNet))
         return;

      ppNet->RemoveAll_AccessibleLocations();
      //if (ppNet->GetCount_Probes() == 0)
      //{
      // delete ppNet;
      // m_ppNets.RemoveKey(netName);
      //}
   }

   m_LocationTreeTop.deleteAll();
   m_LocationTreeBot.deleteAll();
}

void CProbePlacementSolution::RemoveAll_Nets()
{
   // remove all probes
   POSITION netPos = m_ppNets.GetStartPosition();
   while (netPos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(netPos, netName, ppNet);

      delete ppNet;
   }

   m_ppNets.RemoveAll();
   m_LocationTreeTop.deleteAll();
   m_LocationTreeBot.deleteAll();
}

void CProbePlacementSolution::ShowProgress()
{
   if (m_pProgressDlg == NULL)
   {
      m_pProgressDlg = new CProgressDlg("Probe Placement");
      m_pProgressDlg->Create(AfxGetMainWnd());
      m_pProgressDlg->SetStep(1);
   }

   m_pProgressDlg->SetPos(0);
   m_pProgressDlg->ShowWindow(SW_SHOW);
}

void CProbePlacementSolution::HideProgress()
{
   if (m_pProgressDlg == NULL)
      return;

   m_pProgressDlg->ShowWindow(SW_HIDE);
}

void CProbePlacementSolution::destroyProgress()
{
   if (m_pProgressDlg == NULL)
      return;

   m_pProgressDlg->ShowWindow(SW_HIDE);
   if (m_pProgressDlg->DestroyWindow())
   {
      delete m_pProgressDlg;
      m_pProgressDlg = NULL;
   }
}

int CProbePlacementSolution::RunProbePlacement()
{
   if (m_pFile == NULL)
   {
      m_sErrMsg = "Current file pointer is NULL.";
      return -1;
   }

   // Reset probe placement solution
   m_ppNets.empty();
   m_sErrMsg.Empty();
   m_LocationTreeTop.deleteAll();
   m_LocationTreeBot.deleteAll();

   haveTopProbes = !m_pTestPlan->CanProbeTopSide();
   haveBotProbes = !m_pTestPlan->CanProbeBotSide();

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

   long progressMin = 0, progressMax = 0;
   progressMax = 1 + 1 + 8;
   if (m_pProgressDlg != NULL)
      m_pProgressDlg->SetRange32(progressMin, progressMax);

   gatherAccesibleLocations();
   placeProbes();
	removeExcessUnplacedProbes();

   return 0;
}

long CProbePlacementSolution::PlaceProbesOnBoard()
{
   long placedProbeCount = 0;
   CMapStringToPtr accessPointerMap;
   long accPointCount = 1;
   m_netResultList.empty();

   POSITION netPos = m_ppNets.GetStartPosition();
   while (netPos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(netPos, netName, ppNet);

      // Creat net result
      CPPNetResult* netResult = new CPPNetResult(netName);
      m_netResultList.AddTail(netResult);

      POSITION probePos = ppNet->GetHeadPosition_Probes();
      while (probePos)
      {
         CPPProbe *probe = ppNet->GetNext_Probes(probePos);

         if (!probe->IsPlaced())
         {
            // Save unplaced proberesult
            CPPProbeResult* probeResult = netResult->AddProbeResult(probe->GetResourceType(), probe->GetNetName(), "", probe->GetProbeTemplateName(), probe->GetProbeRefname());

            probe->AddProbeUnplacedToBoard(m_pFile, *m_pTestPlan);
            continue;
         }

         CPPAccessibleLocation *accLoc = probe->GetAccessibleLocation();

         if (accLoc == NULL)
            continue;

         // create an access point if one didn't exist (only for forced probes)
         CString accPointName;

         if (accLoc->GetTAInsert() == NULL)
         {
            // we need to create a test access point
            accPointName.Format("$$FORCEDACCESS_%d", accPointCount++);
            DataStruct *accData = PlaceTestAccessPoint(m_pDoc, m_pFile->getBlock(), accPointName, accLoc->GetLocation().x, accLoc->GetLocation().y,
               accLoc->GetSurface(), accLoc->GetNetName(), "SMD", accLoc->getFeatureEntity().getCompPin()->getEntityNumber(), 0.0,
               m_pDoc->getSettings().getPageUnits());
            accLoc->SetTAInsert(accData);
         }
         else
         {
            accPointName = accLoc->GetTAInsert()->getInsert()->getRefname();
         }

         // Save probe result
         netResult->SetNoProbe(false);
         CPPProbeResult* probeResult = netResult->AddProbeResult(probe->GetResourceType(), probe->GetNetName(), accPointName, probe->GetProbeTemplateName(), probe->GetProbeRefname());

         // Map access name to access point
         accessPointerMap.SetAt(accPointName, accLoc->GetTAInsert());         
      }
   }

   placedProbeCount = PlaceProbesResultOnBoard(accessPointerMap);
   accessPointerMap.RemoveAll();

   return placedProbeCount;
}

long CProbePlacementSolution::PlaceProbesResultOnBoard(CMapStringToPtr& accessPointMap)
{
   WORD dLinkKW = m_pDoc->IsKeyWord(ATT_DDLINK, 0);
   WORD testResKW = m_pDoc->RegisterKeyWord(ATT_TESTRESOURCE, 0, VT_STRING);
   WORD probePlacementKW = m_pDoc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, VT_STRING);
   WORD netNameKW = m_pDoc->IsKeyWord(ATT_NETNAME, 0);
   WORD refnameKW = m_pDoc->RegisterKeyWord(ATT_REFNAME, 0, VT_STRING);
   long placedProbeCount = 0;

   Graph_Block_On(m_pFile->getBlock());

   POSITION netResultPos = m_netResultList.GetHeadPosition();
   while (netResultPos != NULL)
   {
      CPPNetResult* netResult = m_netResultList.GetNext(netResultPos);
      if (netResult == NULL)
         continue;

      if (netResult->IsNoProbe())
      {
         // add a drc marker if the net doesn't have any placed probes
         drc_net_without_probe(m_pDoc, m_pFile, netResult->GetNetName());
         continue;
      }

      POSITION probeResultPos = netResult->GetProbeHeadPosition();
      while (probeResultPos != NULL)
      {
         CPPProbeResult* probeResult = netResult->GetProbeNext(probeResultPos);
         if (probeResult == NULL || probeResult->GetAccessName().IsEmpty())
            continue;

         DataStruct* accData = NULL;
         if (!accessPointMap.Lookup(probeResult->GetAccessName(), (void*&)accData) || accData == NULL)
            continue;

         InsertStruct* accInsert = accData->getInsert();
         if (accInsert == NULL)
            continue;

         int mirror = accInsert->getMirrorFlags();
         POSITION probeTemplatePos = NULL;
         CDFTProbeTemplate *pTemplate =  m_pTestPlan->GetProbes().Find_Probe(probeResult->GetProbeTemplateName(),
               probeTemplatePos, mirror?testSurfaceBottom:testSurfaceTop);
         if (pTemplate == NULL && m_pTestPlan->GetTesterType() == DFT_TESTER_FIXTURE)
            continue;


         // Create probe geometry
         CString probeName = pTemplate->GetName() + ((mirror == 0)?"_Top":"_Bot");
         double diameter = pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
         double drillsize = pTemplate->GetDrillSize() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
         double textsize = pTemplate->GetTextSize() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
         BlockStruct* probeBlock = CreateTestProbeGeometry(m_pDoc, probeName, diameter, probeName, drillsize);

         // Insert the probe 
         DataStruct *probe = Graph_Block_Reference(probeBlock->getName(), probeResult->GetProbeRefname(), -1, 
               accInsert->getOriginX(), accInsert->getOriginY(), 0.0, mirror, 1.0, -1, 0);
         probe->getInsert()->setInsertType(insertTypeTestProbe);
         placedProbeCount++;

         // Determine resource type
         CString testResourceType;
         switch (probeResult->GetResourceType())
         {
         case testResourceTypeTest:
            testResourceType = "Test";
            break;
         case testResourceTypePowerInjection:
            testResourceType = "Power Injection";
#ifdef DEADCODE
            {
               CString probeName;
               CDFTProbeableNet *pNet = m_pTestPlan->GetNetConditions().Find_ProbeableNet(probeResult->GetNetName(), probeTemplatePos);
               switch (pNet->GetNetType())
               {
                  //*rcf  KILL ALL THIS, IT IS OBSOLETE
               case probeableNetTypePower:
                  probeName = (CString)"P" + probe->getInsert()->getRefname();
                  probe->getInsert()->setRefname(strdup(probeName));
                  break;
               case probeableNetTypeGround:
                  probeName = (CString)"G" + probe->getInsert()->getRefname();
                  probe->getInsert()->setRefname(strdup(probeName));
                  break;
               }
            }
#endif
            break;
         }

         // Set attributes
         long datalink = accData->getEntityNumber();
         probe->setAttrib(m_pDoc, dLinkKW, VT_INTEGER, (void*)&datalink, SA_OVERWRITE, NULL);
         probe->setAttrib(m_pDoc, testResKW, VT_STRING, testResourceType.GetBuffer(0), SA_OVERWRITE, NULL);
         probe->setAttrib(m_pDoc, probePlacementKW, VT_STRING, "Placed", SA_OVERWRITE, NULL);
         probe->setAttrib(m_pDoc, netNameKW, VT_STRING, probeResult->GetNetName().GetBuffer(0), SA_OVERWRITE, NULL);

         CreateTestProbeRefnameAttr(m_pDoc, probe, probe->getInsert()->getRefname(), drillsize, textsize, m_pTestPlan->GetNeverMirrorRefname());

      }
   }

   Graph_Block_Off();

   return placedProbeCount;
}

int CProbePlacementSolution::ValidateAndPlaceProbes()
{
   haveTopProbes = !m_pTestPlan->CanProbeTopSide();
   haveBotProbes = !m_pTestPlan->CanProbeBotSide();

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

   RemoveAll_Nets();
   
   m_probeNamer.Reset(m_pTestPlan->GetProbeStartNumber(), 1, 1);

   long progressMin = 0, progressMax = 0;
   progressMax = 1 + 1 + 8;
   if (m_pProgressDlg != NULL)
      m_pProgressDlg->SetRange32(progressMin, progressMax);

   gatherAccesibleLocations();
   gatherPlacedProbes();
   maximizeProbeSizes();
   while (fixViolatingProbeTemplates() > 0)
      ;

   return 0;
}

void CProbePlacementSolution::GetProbeCounts(int &topPlaced, int &botPlaced, int &totUnplaced, int &forcedInactiveSide, CProbeCountArray &countArray,
                                             int &netsInvolved, int &netsFullyProbed, int &netsPartiallyProbed, int &netsNotProbed)
{
   topPlaced = botPlaced = totUnplaced = forcedInactiveSide = 0;
   netsInvolved = netsFullyProbed = netsPartiallyProbed = netsNotProbed = 0;
   countArray.RemoveAll();

   // get all the probe names
   POSITION pos = m_pTestPlan->GetProbes().GetHeadPosition_TopProbes();
   while (pos)
   {
      CDFTProbeTemplate *pTemplate = m_pTestPlan->GetProbes().GetNext_TopProbes(pos);
      countArray.Add(CProbeCount(pTemplate->GetName(), testSurfaceTop, m_pTestPlan->CanProbeTopSide() && pTemplate->GetUseFlag()));
   }
   pos = m_pTestPlan->GetProbes().GetHeadPosition_BotProbes();
   while (pos)
   {
      CDFTProbeTemplate *pTemplate = m_pTestPlan->GetProbes().GetNext_BotProbes(pos);

      bool foundTemplate = false;
      for (int i=0; i<countArray.GetCount(); i++)
      {
         CProbeCount &probeCount = countArray.GetAt(i);
         if (probeCount.GetProbeName() == pTemplate->GetName())
         {
            probeCount.SetProbeSurface(testSurfaceBoth);
            probeCount.SetBottomUsedFlag(m_pTestPlan->CanProbeBotSide() && pTemplate->GetUseFlag());
            foundTemplate = true;
            break;
         }
      }

      if (!foundTemplate)
         countArray.Add(CProbeCount(pTemplate->GetName(), testSurfaceBottom, m_pTestPlan->CanProbeBotSide() && pTemplate->GetUseFlag()));
   }

   pos = m_ppNets.GetStartPosition();
   while (pos)
   {
      bool hasPlacedProbe = false;
      bool hasUnplacedProbe = false;
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(pos, netName, ppNet);

      POSITION probePos = ppNet->GetHeadPosition_Probes();
      
      if (probePos != NULL)
         netsInvolved++;

      while (probePos)
      {
         CPPProbe *probe = ppNet->GetNext_Probes(probePos);
         CPPAccessibleLocation *accLoc = probe->GetAccessibleLocation();
         if (accLoc == NULL)
         {
            hasUnplacedProbe = true;
            totUnplaced++;
         }
         else
         {
            hasPlacedProbe = true;
            if (accLoc->GetSurface() == testSurfaceTop)
               topPlaced++;
            else if (accLoc->GetSurface() == testSurfaceBottom)
               botPlaced++;

            if (accLoc->GetFeatureCondition() == featureConditionForced)
            {
               if ((accLoc->GetSurface() == testSurfaceTop && !m_pTestPlan->CanProbeTopSide()) ||
                  (accLoc->GetSurface() == testSurfaceBottom && !m_pTestPlan->CanProbeBotSide()))
               {
                  forcedInactiveSide++;
               }
            }

            POSITION pTempPos = NULL;
            CDFTProbeTemplate *pTemplate = m_pTestPlan->GetProbes().Find_Probe(probe->GetProbeTemplateName(), pTempPos, accLoc->GetSurface());
            if (pTemplate == NULL)
               continue;

            for (int i=0; i<countArray.GetCount(); i++)
            {
               CProbeCount &probeCount = countArray.GetAt(i);
               if (probeCount.GetProbeName() == pTemplate->GetName())
               {
                  if (probe->GetResourceType() == testResourceTypeTest)
                  {
                     if (accLoc->GetSurface() == testSurfaceTop)
                     {
                        probeCount.IncrementTopTestResources();
                        if (accLoc->IsDoubleWired())
                           probeCount.IncrementTopTestResources();
                     }
                     else if (accLoc->GetSurface() == testSurfaceBottom)
                     {
                        probeCount.IncrementBottomTestResources();
                        if (accLoc->IsDoubleWired())
                           probeCount.IncrementBottomTestResources();
                     }
                  }
                  else if (probe->GetResourceType() == testResourceTypePowerInjection)
                  {
                     if (accLoc->GetSurface() == testSurfaceTop)
                     {
                        probeCount.IncrememtTopPowerInjectionResources();
                        if (accLoc->IsDoubleWired())
                           probeCount.IncrememtTopPowerInjectionResources();
                     }
                     else if (accLoc->GetSurface() == testSurfaceBottom)
                     {
                        probeCount.IncrememtBottomPowerInjectionResources();
                        if (accLoc->IsDoubleWired())
                           probeCount.IncrememtBottomPowerInjectionResources();
                     }
                  }

                  break;
               }
            }
         }
      }

      if (hasPlacedProbe && hasUnplacedProbe) // has both placed and unplaced
         netsPartiallyProbed++;
      else if (hasPlacedProbe) // has placed and no unplaced
         netsFullyProbed++;
      else if (hasUnplacedProbe)
         netsNotProbed++;
      // else is not a participating net
   }
}

void CProbePlacementSolution::ReservePreexistingProbeRefnames()
{
   if (m_pTestPlan->GetUseExistingProbes() && m_pTestPlan->RetainProbeNames())
   {
      POSITION pos = m_ppNets.GetStartPosition();
      while (pos)
      {
         CString netName;
         CPPNet *ppNet = NULL;
         m_ppNets.GetNextAssoc(pos, netName, ppNet);

         if (!ppNet->GetNoProbeFlag())
         {
            POSITION probePos = ppNet->GetHeadPosition_Probes();
            while (probePos)
            {
               CPPProbe *probe = ppNet->GetNext_Probes(probePos);

               if (probe->DidExist())
                  m_probeNamer.Strike(probe->GetProbeRefname());
            }
         }
      }
   }
}

void CProbePlacementSolution::RenumberProbes()
{
   // Initial naming of probes is done before it is known what probes will end up placed
   // and what will end up unplaced. Mark asked for probes to be numbered in continuous
   // blocks, all placed probes first, then unplaced probes. So here we renumber aka rename
   // the probes. Watch out not to renumber any "use existing - retain probe names" probes.

   // Four passes, states are:
   // 1 - Number placed probes that are not Power Injection
   // 2 - Number unplaced probes that are not Power Injection
   // 3 - Number placed power injection probes
   // 4 - Number unplaced power injection probes

   m_probeNamer.Reset(m_pTestPlan->GetProbeStartNumber(), 1, 1);
   ReservePreexistingProbeRefnames();

   // Sort the nets first, this will result in probes assigned low to high on nets
   // in ascending alpha order, a double whammy ordering for the list box.
   m_ppNets.setSortFunction(&CPPNetMap::AscendingNetNameSortFunc);
   m_ppNets.Sort();

   for (int state = 1; state <= 4; state++)
   {
      CString *netName = NULL;
      CPPNet *ppNet = NULL;
      for (m_ppNets.GetFirstSorted(netName, ppNet); ppNet != NULL; m_ppNets.GetNextSorted(netName, ppNet))
      {
         if (!ppNet->GetNoProbeFlag())
         {
            POSITION probePos = ppNet->GetHeadPosition_Probes();
            while (probePos)
            {
               CPPProbe *probe = ppNet->GetNext_Probes(probePos);

               if (probe->DidExist() && m_pTestPlan->GetUseExistingProbes() && m_pTestPlan->RetainProbeNames())
               {
                  // Pre-existing probe, leave name alone
               }
               else
               {
                  // Rename

                  switch (state)
                  {
                  case 1:   // Placed probes that are Not power injection
                     {
                        if (probe->IsPlaced() && probe->GetResourceType() == testResourceTypeTest)
                           probe->SetProbeRefname(m_probeNamer.GetNextProbeRefname(ppNet, probe->GetResourceType()));
                     }
                     break;
                  case 2:   // Unplaced probes that are Not power injection
                     {
                        if (!probe->IsPlaced() && probe->GetResourceType() == testResourceTypeTest)
                           probe->SetProbeRefname(m_probeNamer.GetNextProbeRefname(ppNet, probe->GetResourceType()));
                     }
                     break;
                  case 3:   // Placed probes that Are power injection
                     {
                        if (probe->IsPlaced() && probe->GetResourceType() == testResourceTypePowerInjection)
                           probe->SetProbeRefname(m_probeNamer.GetNextProbeRefname(ppNet, probe->GetResourceType()));
                     }
                     break;
                  case 4:   // Unplaced probes that Are power injection
                     {
                        if (!probe->IsPlaced() && probe->GetResourceType() == testResourceTypePowerInjection)
                           probe->SetProbeRefname(m_probeNamer.GetNextProbeRefname(ppNet, probe->GetResourceType()));
                     }
                     break;
                  }
               }
            }
         }
      }
   }
}

bool CProbePlacementSolution::CreateReport(CString Filename, CAccessAnalysisSolution *aaSolution)
{
   CFormatStdioFile file;
   CFileException err;

   if (!file.Open(Filename, CFile::modeCreate|CFile::modeWrite, &err))
      return false;

   int indent = 0;
   CTime t = t.GetCurrentTime();
   file.WriteString("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   file.WriteString("+ Probe Placement Report\n");
   file.WriteString("+\n");
   file.WriteString("+ Created by %s - %s\n", getApp().getCamCadTitle(), COPYRIGHT);
   file.WriteString("+ Version     : %s\n", getApp().getVersionString());
   file.WriteString("+ Date & Time : %s\n", t.Format("%A, %B %d, %Y at %H:%M:%S"));
   file.WriteString("+ PCB File    : %s\n", m_pFile->getName());
   file.WriteString("+ Units       : %s\n", GetUnitName(m_pDoc->getSettings().getPageUnits()));
   file.WriteString("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   file.WriteString("\n\n");

   file.WriteString("=====================================================================================\n");
   file.WriteString("= TEST PLAN SETTINGS\n");
   file.WriteString("=====================================================================================\n");
   m_pTestPlan->WriteReport(file, 3);
   file.WriteString("\n\n");

   int topPlaced = 0, botPlaced = 0;
   int totUnplaced = 0;
   int forcedInactiveSide = 0;
   int netsInvolved = 0, netsFullyProbed = 0, netsPartiallyProbed = 0, netsNotProbed = 0;
   CProbeCountArray countArray;
   GetProbeCounts(topPlaced, botPlaced, totUnplaced, forcedInactiveSide, countArray, netsInvolved, netsFullyProbed, netsPartiallyProbed, netsNotProbed);

   CExtent ext = m_pFile->getBlock()->getExtent();

   if (!ext.isValid())
   {
      m_pDoc->CalcBlockExtents(m_pFile->getBlock());
      ext = m_pFile->getBlock()->getExtent();
   }

   file.WriteString("=====================================================================================\n");
   file.WriteString("= BOARD STATISTICS\n");
   file.WriteString("=====================================================================================\n");
   file.WriteString("Board Size : %.3f x %.3f\n", ext.getXsize(), ext.getYsize());
   file.WriteString("Probes Placed : %d\n", topPlaced + botPlaced);
   file.WriteString("Probes Unplaced : %d\n", totUnplaced);
   if (forcedInactiveSide > 0)
      file.WriteString("Probes Forced to Inactive Side : %d\n", forcedInactiveSide);
   file.WriteString("Nets not probed : %d (%.2f%%)\n", netsNotProbed,              ((double)netsNotProbed / (double)netsInvolved) * 100.);
   file.WriteString("Nets partially probed : %d (%.2f%%)\n", netsPartiallyProbed,  ((double)netsPartiallyProbed / (double)netsInvolved) * 100.);
   file.WriteString("Nets completely probed : %d (%.2f%%)\n", netsFullyProbed,     ((double)netsFullyProbed / (double)netsInvolved) * 100.);
   file.WriteString("Nets not analyzed : %d\n", m_ppNets.GetCount() - netsInvolved);
   file.WriteString("\n");
   CString boundingLine = "-----------------------";
   CString probeLine    = "|Probes               |";
   CString divideLine   = "|                     |";
   CString divideLine2  = "|                     |";
   CString divideLine3  = "|---------------------|";
   for (int i=0; i<countArray.GetCount(); i++)
   {
      CProbeCount &probeCount = countArray.GetAt(i);

      CString buf;
      buf.Format("    %-s%*s|", probeCount.GetProbeName(), 7-probeCount.GetProbeName().GetLength(), " ");

      boundingLine += "------------";
      probeLine    += buf;
      divideLine   += "-----------|";
      divideLine2  += " TR  | PIJ |";
      divideLine3  += "-----------|";
   }
   file.WriteString("%s\n", boundingLine);
   file.WriteString("%s\n", probeLine);
   file.WriteString("%s\n", divideLine);
   file.WriteString("%s\n", divideLine2);
   file.WriteString("%s\n", divideLine3);

   // top results
   file.WriteString("|Top Probes Placed    | ");
   for (int i=0; i<countArray.GetCount(); i++)
   {
      CProbeCount &probeCount = countArray.GetAt(i);

      if (probeCount.GetTopUsedFlag())
      {
         CString buf;
         buf.Format("%d", probeCount.GetTopTestResourceCount());
         file.WriteString("%-s%*s| ", buf, 4-buf.GetLength(), " ");

         buf.Format("%d", probeCount.GetTopPowerInjectionResourceCount());
         file.WriteString("%-s%*s| ", buf, 4-buf.GetLength(), " ");
      }
      else
         file.WriteString("-   | -   | ");
   }
   file.WriteString("\n");

   // bottom results
   file.WriteString("|Bottom Probes Placed | ");
   for (int i=0; i<countArray.GetCount(); i++)
   {
      CProbeCount &probeCount = countArray.GetAt(i);

      if (probeCount.GetBottomUsedFlag())
      {
         CString buf;
         buf.Format("%d", probeCount.GetBottomTestResourceCount());
         file.WriteString("%-s%*s| ", buf, 4-buf.GetLength(), " ");

         buf.Format("%d", probeCount.GetBottomPowerInjectionResourceCount());
         file.WriteString("%-s%*s| ", buf, 4-buf.GetLength(), " ");
      }
      else
         file.WriteString("-   | -   | ");
   }
   file.WriteString("\n");
   file.WriteString("%s\n", boundingLine);
   file.WriteString("\n\n");

   // This will get the Unplaced Probe report section output in
   // ascending net name order.
   m_ppNets.setSortFunction(&CPPNetMap::AscendingNetNameSortFunc);
   m_ppNets.Sort();

   WriteUnplacedProbePlacement(file, aaSolution, includeMultiPinNet);
   WriteUnplacedProbePlacement(file, aaSolution, includeSinglePinNet);
   WriteUnplacedProbePlacement(file, aaSolution, includeUnconnecedNet);

   WritePlacedProbePlacement(file, aaSolution, includeMultiPinNet);
   WritePlacedProbePlacement(file, aaSolution, includeSinglePinNet);
   WritePlacedProbePlacement(file, aaSolution, includeUnconnecedNet);

   file.Close();

   return true;
}

void CProbePlacementSolution::WriteNetAccessInfo(CFormatStdioFile &file, CAANetAccess *netAcc)
{
   if (netAcc != NULL)
   {
      POSITION netAccPos = netAcc->GetHeadPosition_AccessibleLocations();
      while (netAccPos)
      {
         CAAAccessLocation *accLoc = netAcc->GetNext_AccessibleLocations(netAccPos);

         if (m_pTestPlan->GetPPWriteIgnoreSurface() == false && m_pTestPlan->GetProbeSide() != testSurfaceBoth &&
            m_pTestPlan->GetProbeSide() != accLoc->GetAccessSurface())
            continue;

         if (!accLoc->GetAccessible())
         {
            CString violation = accLoc->GetErrorMessage();

            CString featureName;
            CPoint2d location;

            if (accLoc->getFeatureEntity().getEntityType() == entityTypeCompPin)
            {
               featureName = accLoc->getFeatureEntity().getCompPin()->getPinRef('-');
               location    = accLoc->getFeatureEntity().getCompPin()->getOrigin();
            }
            else if (accLoc->getFeatureEntity().getEntityType() == entityTypeData)
            {
               featureName = accLoc->getFeatureEntity().getData()->getInsert()->getRefname();
               location    = accLoc->getFeatureEntity().getData()->getInsert()->getOrigin2d();
            }

            file.WriteString("%-15s %-8s %-8.3f %-8.3f %-s\n", featureName, (accLoc->GetAccessSurface()==testSurfaceTop)?"T":"B",
               location.x, location.y, violation);
         }
      }
   }
}

void CProbePlacementSolution::WriteNetAccessInfo(CFormatStdioFile &file, CPPNet *ppNet)
{
   if (ppNet != NULL)
   {
      POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations();
      while (accPos)
      {
         CPPAccessibleLocation *accLoc = ppNet->GetNext_AccessibleLocations(accPos);

         if (m_pTestPlan->GetPPWriteIgnoreSurface() == false && m_pTestPlan->GetProbeSide() != testSurfaceBoth &&
            m_pTestPlan->GetProbeSide() != accLoc->GetSurface())
            continue;

         CString info;
         if (accLoc->GetPlaceableProbe() != NULL)
         {
            CPPProbe *secondProbe = accLoc->GetDoubleWiredProbe();
            info.Format("Placed Probe%s:  %s%s%s", 
               secondProbe != NULL ? "s" : "",   // plural in label to match number of probes
               accLoc->GetPlaceableProbe()->GetProbeRefname(),
               secondProbe != NULL ? ", " : "",  // separator between probe numbers
               secondProbe != NULL ? secondProbe->GetProbeRefname() : "");
         }
         else if (m_pTestPlan->GetProbeSide() != testSurfaceBoth && accLoc->GetSurface() != m_pTestPlan->GetProbeSide())
            info = "Surface not evaluated";
         else if (ppNet->GetNoProbeFlag())
            info = "Net set to No Probe";
         else if (accLoc->GetFeatureCondition() == featureConditionNoProbe)
            info = "Feature set to No Probe";
         else if (!isValidTargetType(accLoc))
            info = "Excluded target type";
         else if (accLoc->IsProbed() && !accLoc->IsDoubleWired() && !m_pTestPlan->GetAllowDoubleWiring())
            info = "Double wiring option not allowed";
         else
            info = "Probe-to-probe spacing violation";

         CString featureName;
         if (accLoc->getFeatureEntity().getEntityType() == entityTypeCompPin)
            featureName = accLoc->getFeatureEntity().getCompPin()->getPinRef('-');
         else if (accLoc->getFeatureEntity().getEntityType() == entityTypeData)
            featureName = accLoc->getFeatureEntity().getData()->getInsert()->getRefname();

         file.WriteString("%-15s %-8s %-8.3f %-8.3f %-s\n", featureName, (accLoc->GetSurface()==testSurfaceTop)?"T":"B",
            accLoc->GetLocation().x, accLoc->GetLocation().y, info);

      }
   }
}

void CProbePlacementSolution::WriteUnplacedProbePlacement(CFormatStdioFile &file, CAccessAnalysisSolution *aaSolution, EIncludedNetType netType)
{
   CString netTypeStr = "";
   if (netType == includeMultiPinNet) 
      netTypeStr = "Multiple Pin Nets";
   else if (netType == includeSinglePinNet)
      netTypeStr = "Single Pin Nets";
   else if (netType == includeUnconnecedNet)
      netTypeStr = "Unconnected Nets";

   file.WriteString("=====================================================================================\n");
   file.WriteString("= UNPLACED PROBES: %s\n", netTypeStr);
   file.WriteString("=====================================================================================\n");

   if (aaSolution == NULL)
      return;

   CString *netname = NULL;
   CPPNet *ppNet = NULL;
   for (m_ppNets.GetFirstSorted(netname, ppNet); ppNet != NULL; m_ppNets.GetNextSorted(netname, ppNet))
   {

      CAANetAccess *netAcc = aaSolution->GetNetAccess(*netname);
      if (netAcc != NULL && netAcc->GetIncludeNetType() == netType)
      {
         if (ppNet->HasUnplacedProbe())
         {
            // Net Name
            file.WriteString("NET: %s\n", *netname);

            // Unplaced probes numbers
            CString unplacedProbeNumbers;
            bool plural = false;
            POSITION probePos = ppNet->GetHeadPosition_Probes();
            while (probePos)
            {
               CPPProbe *probe = ppNet->GetNext_Probes(probePos);
               if (!probe->IsPlaced())
               {
                  if (!unplacedProbeNumbers.IsEmpty())
                  {
                     unplacedProbeNumbers += ", ";
                     plural = true;
                  }
                  unplacedProbeNumbers += probe->GetProbeRefname();
               }
            }
            file.WriteString("Unplaced probe%s: %s\n", plural ? "s" : "", unplacedProbeNumbers);

            // Net access stats
            WriteNetAccessInfo(file, netAcc);
            WriteNetAccessInfo(file, ppNet);

            file.WriteString("\n");

         }
      }
   }
   file.WriteString("\n\n");
}

void CProbePlacementSolution::WritePlacedProbePlacement(CFormatStdioFile &file, CAccessAnalysisSolution *aaSolution, EIncludedNetType netType)
{
   CString netTypeStr = "";

   if (netType == includeMultiPinNet) 
      netTypeStr = "Multiple Pin Nets";
   else if (netType == includeSinglePinNet)
      netTypeStr = "Single Pin Nets";
   else if (netType == includeUnconnecedNet)
      netTypeStr = "Unconnected Nets";

   file.WriteString("=====================================================================================\n");
   file.WriteString("= PLACED PROBES: %s\n", netTypeStr);
   file.WriteString("=====================================================================================\n");

   if (aaSolution == NULL)
      return;

   POSITION pos = m_ppNets.GetStartPosition();
   while (pos)
   {
      CString netname;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(pos, netname, ppNet);

      CAANetAccess *netAcc = aaSolution->GetNetAccess(netname);

      if (netAcc == NULL || netAcc->GetIncludeNetType() != netType)
         continue;

      POSITION probePos = ppNet->GetHeadPosition_Probes();
      while (probePos)
      {
         CPPProbe *probe = ppNet->GetNext_Probes(probePos);

         if (probe->IsPlaced())
         {
            CString buf;
            CPPAccessibleLocation *accLoc = probe->GetAccessibleLocation();

            if (accLoc->getFeatureEntity().getEntityType() == entityTypeData)
            {
               DataStruct* data = accLoc->getFeatureEntity().getData();

               if (data->getDataType() == dataTypeInsert)
               {
                  buf = data->getInsert()->getRefname();
               }
               else
               {
                  buf = dataStructTypeToString(data->getDataType());
               }
            }
            else if (accLoc->getFeatureEntity().getEntityType() == entityTypeCompPin)
            {
               buf = accLoc->getFeatureEntity().getCompPin()->getPinRef('-');
            }

            file.WriteString("%-8s %-8s %-8.3f %-8.3f %-8s %-15s %-8s%s\n",
               probe->GetProbeRefname(), probe->GetProbeTemplateName(),
               accLoc->GetLocation().x, accLoc->GetLocation().y,
               buf, probe->GetNetName(), 
               (accLoc->GetSurface() == testSurfaceTop)?"Top":"Bottom",
               (accLoc->GetFeatureCondition() == featureConditionForced)?" Forced":"");
         }
      }
   }

   file.WriteString("\n\n");
}

void CProbePlacementSolution::DumpToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*s::PPNets::\n", indent, " ");
   indent += 3;
   POSITION pos = m_ppNets.GetStartPosition();
   while (pos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(pos, netName, ppNet);

      ppNet->DumpToFile(file, indent);
   }
   indent -= 3;

   CHtmlFileWriteFormat htmlLogFile(1024);
   CString path = file.GetFilePath();
   if (htmlLogFile.open(path.Mid(0, path.ReverseFind('.')) + "_metrics.htm"))
   {
      m_LocationTreeTop.printMetricsReport(htmlLogFile);
      m_LocationTreeBot.printMetricsReport(htmlLogFile);
   }
   htmlLogFile.close();
}

void CProbePlacementSolution::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<PPSolution>\n");
   writeFormat.pushHeader("  ");

   POSITION pos = m_netResultList.GetHeadPosition();
   while (pos)
   {
      CPPNetResult* netResult = m_netResultList.GetNext(pos);
      if (netResult == NULL)
         continue;

      netResult->WriteXML(writeFormat, progress);
   }

   writeFormat.popHeader();
   writeFormat.writef("</PPSolution>\n");
}

int CProbePlacementSolution::LoadXML(CXMLNode *node)
{
   if (node == NULL)
      return -1;

   CXMLNode *subNode = NULL;
   node->ResetChildList();
   while (subNode = node->NextChild())
   {
      CString nodeName;
      if (!subNode->GetName(nodeName) || nodeName != "ProbeNet")
         continue;

      CString netName;
      if (!subNode->GetAttrValue("Name", netName))
         continue;

      CPPNetResult *netResult = new CPPNetResult(netName);
      m_netResultList.AddTail(netResult);

      netResult->LoadXML(subNode);

      delete subNode;
   }

   return 0;
}

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________

//_____________________________________________________________________________
