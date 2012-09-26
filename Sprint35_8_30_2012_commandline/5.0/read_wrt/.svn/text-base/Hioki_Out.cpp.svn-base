/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2011. All Rights Reserved.
*/

// HiokiWRITE.CPP

#include "stdafx.h"
#include "CCEtoODB.h"
#include "DFT.h"
#include "DeviceType.h"
#include "Gauge.h"
#include "PcbUtil.h"
#include "Hioki_Out.h"
#include "RwUiLib.h"
#include "CompValues.h"

extern char* netstatus[];

static int defaultICDiodeTolerance = 25;
static HiokiProbeOutputCollection m_topProbeOutputCollection;
static HiokiProbeOutputCollection m_bottomProbeOutputCollection;
static bool uniqueProbeNames = false;


//*rcf Come up with something better for this
static int globalStepNo = 0;

//-----------------------------------------------------------------------------

void Hioki_WriteFiles(CString filename, CCEtoODBDoc &doc, FormatStruct *format)
{
   HiokiWriter writer(doc, filename);
   writer.write();
}

//-----------------------------------------------------------------------------

HiokiSurface intToSurfaceTag(const int surface)
{
   HiokiSurface retval = surfaceUnknown;

   switch (surface)
   {
   case 0:  retval = surfaceTop;       break;
   case 1:  retval = surfaceBottom;    break;
   case 2:  retval = surfaceBoth;      break;
   default: retval = surfaceUnknown;   break;
   }

   return retval;
}

HiokiShortType shortTypeStringToTag(const CString shortTypeString)
{
   if (shortTypeString.CompareNoCase("ANALYSIS") == 0)      return shortTypeAnalysis;
   if (shortTypeString.CompareNoCase("PIN_TO_PIN") == 0)    return shortTypePinToPin;
   
   return shortTypeUnknown;
}

HiokiPinMapType pinMapTypeStringToTag(const CString pinMapTypeString)
{
   if (pinMapTypeString.CompareNoCase("Anode") == 0)           return pinMapTypeAnode;
   if (pinMapTypeString.CompareNoCase("Cathode") == 0)         return pinMapTypeCathode;
   if (pinMapTypeString.CompareNoCase("Base") == 0)            return pinMapTypeBase;
   if (pinMapTypeString.CompareNoCase("Collector") == 0)       return pinMapTypeCollector;
   if (pinMapTypeString.CompareNoCase("Emitter") == 0)         return pinMapTypeEmitter;
   if (pinMapTypeString.CompareNoCase("Gate") == 0)            return pinMapTypeGate;
   if (pinMapTypeString.CompareNoCase("Source") == 0)          return pinMapTypeSource;
   if (pinMapTypeString.CompareNoCase("Drain") == 0)           return pinMapTypeDrain;   

   return pinMapTypeUnknown;
}

static void convertToHiokiUnits(double& value, double unitFactor)
{
   value = floor(value * unitFactor + 0.5);
}

//_____________________________________________________________________________
HiokiTestAccess::HiokiTestAccess(const long accessEntityNumber,const CString name, const CString netName, const CPoint2d xyOrigin,
                       const HiokiSurface surface,const bool isOnIcGroundNet,const bool hasTakayaAccessAttrib,const int targetTypePriority,
                       const long targetEntityNumber)
: m_accessEntityNumber(accessEntityNumber)
, m_targetEntityNumber(targetEntityNumber)
, m_name(name)
, m_netName(netName)
, m_errorMessage("")
, m_xyOrigin(xyOrigin)
, m_surface(surface)
, m_isOnIcGroundNet(isOnIcGroundNet)
, m_hasTakayaAccessAttrib(hasTakayaAccessAttrib)
, m_usedForIcCapacitorTest(false)
, m_usedForIcDiodeTest(false)
, m_targetTypePriority(targetTypePriority)
, m_usedCount(0)
{
}

HiokiTestAccess::~HiokiTestAccess()
{
}

//_____________________________________________________________________________
HiokiTestAccessCollection::HiokiTestAccessCollection()
: m_testAccessWithAttribute(true)
, m_testAccessWithPriority(true)
{
   m_testAccessWithAttribute.SetSize(0, 20);
   m_testAccessWithPriority.SetSize(0, 20);
   m_curTestAccessWithAttributeIndex = 0;
   m_curTestAccessWithPriorityIndex = 0;
   m_priorityArrayAlreadySorted = false;
}

HiokiTestAccessCollection::~HiokiTestAccessCollection()
{
   empty();
}

void HiokiTestAccessCollection::empty()
{
   m_testAccessWithAttribute.empty();
   m_testAccessWithPriority.empty();
}

HiokiTestAccess* HiokiTestAccessCollection::addTestAccess(const long entityNumber, const CString name, const CString netName, const CPoint2d xyOrigin,
                       const HiokiSurface surface,const bool isOnIcGroundNet,const bool hasTakayaAccessAttrib,const int targetTypePriority,const long targetEntityNumber)
{
   HiokiTestAccess* testAccess = new HiokiTestAccess(entityNumber, name, netName, xyOrigin, surface, isOnIcGroundNet, hasTakayaAccessAttrib, targetTypePriority, targetEntityNumber);
   if (hasTakayaAccessAttrib)
      m_testAccessWithAttribute.SetAtGrow(m_testAccessWithAttribute.GetCount(), testAccess);
   else
      m_testAccessWithPriority.SetAtGrow(m_testAccessWithPriority.GetCount(), testAccess);

   return testAccess;
}

static int priorityCompareFunction(const void *arg1, const void* arg2)
{
   HiokiTestAccess **testAccess1, **testAccess2;
   testAccess1 = (HiokiTestAccess**)arg1;
   testAccess2 = (HiokiTestAccess**)arg2;

   if ((*testAccess1)->getTargetTypePriority() < (*testAccess2)->getTargetTypePriority())
      return -1;
   else if ((*testAccess1)->getTargetTypePriority() > (*testAccess2)->getTargetTypePriority())
      return 1;
   else
      return 0;
}

HiokiTestAccess* HiokiTestAccessCollection::getTestAccess(FILE *logFp, const int maxHits, HiokiTestAccess* testAccessUsed)
{
   int index = m_curTestAccessWithAttributeIndex;
   HiokiTestAccess* testAccess =  NULL;
   while (index < m_testAccessWithAttribute.GetCount())
   {
      HiokiTestAccess* tempTestAccess = m_testAccessWithAttribute.GetAt(index);
      if (tempTestAccess != testAccessUsed && tempTestAccess->getUsedCount() < maxHits) 
      {
         testAccess = tempTestAccess;
         testAccess->increaseUsedCount();

         if (testAccess->getUsedCount() == maxHits)
         {
            fprintf(logFp, "HIT LIMIT REACHED on access point [%s] on net [%s]\n", testAccess->getName(), testAccess->getNetName());
         }
         break;
      }
      else
      {
         index++;
      }
   }

   if (testAccessUsed == NULL)// && index < m_testAccessWithAttribute.GetCount())
   {
      m_curTestAccessWithAttributeIndex = index;
   }

   // No Test Access with TAKAYA_ACCESS attribute so get Test Access by priority 
   if (testAccess == NULL)
   {
      // Sort priority array base on Target Type priority
      // Only sort it once if it is not already sorted
      if (!m_priorityArrayAlreadySorted && m_testAccessWithPriority.GetCount() > 1)
      {
         qsort(m_testAccessWithPriority.GetData(), m_testAccessWithPriority.GetCount(), sizeof(HiokiTestAccess*), priorityCompareFunction);
         m_priorityArrayAlreadySorted = true;
      }

      index = m_curTestAccessWithPriorityIndex;
      while (index < m_testAccessWithPriority.GetCount())
      {
         HiokiTestAccess* tempTestAccess = m_testAccessWithPriority.GetAt(index);
         if (tempTestAccess != testAccessUsed && tempTestAccess->getUsedCount() < maxHits) 
         {
            testAccess = tempTestAccess;
            testAccess->increaseUsedCount();

            if (testAccess->getUsedCount() == maxHits)
            {
               fprintf(logFp, "HIT LIMIT REACHED on access point [%s] on net [%s]\n", testAccess->getName(), testAccess->getNetName());
            }
            break;
         }
         else
         {
            index++;
         }
      }

      if (testAccessUsed == NULL)// && index < m_testAccessWithPriority.GetCount())
      {
         m_curTestAccessWithPriorityIndex = index;
      }
   }

   return testAccess;
}

HiokiTestAccess* HiokiTestAccessCollection::getTestAccessOnTarget(const long targetEntityNumber)
{
   for (int index = 0; index < m_testAccessWithAttribute.GetCount(); index++)
   {
      HiokiTestAccess* testAccess = m_testAccessWithAttribute.GetAt(index);
      if (testAccess != NULL && testAccess->getTargetEntityNumber() == targetEntityNumber) 
      {
         return testAccess;
      }
   }

   for (int index = 0; index < m_testAccessWithPriority.GetCount(); index++)
   {
      HiokiTestAccess* testAccess = m_testAccessWithPriority.GetAt(index);
      if (testAccess != NULL && testAccess->getTargetEntityNumber() == targetEntityNumber) 
      {
         return testAccess;
      }
   }

   return NULL;
}

HiokiTestAccess* HiokiTestAccessCollection::getTestAccess(FILE *logFp, const int maxHits)
{
   return getTestAccess(logFp, maxHits, NULL);
}

void HiokiTestAccessCollection::getTwoTestAccess(FILE *logFp, const int maxHits, HiokiTestAccess** testAccess1, HiokiTestAccess** testAccess2)
{
   *testAccess1 = getTestAccess(logFp, maxHits, NULL);
   *testAccess2 = getTestAccess(logFp, maxHits, *testAccess1);
}

void HiokiTestAccessCollection::writeLogFile(FILE *logFp)
{
   for (int indx = 0; indx < m_testAccessWithAttribute.GetCount(); indx++)
   {
      HiokiTestAccess* fptaccess = m_testAccessWithAttribute.GetAt(indx);
      fprintf(logFp, "   ACCESS [%s]  HITS [%d]\n", fptaccess->getName(), fptaccess->getUsedCount());
   }

   for (int indx = 0; indx < m_testAccessWithPriority.GetCount(); indx++)
   {
      HiokiTestAccess* fptaccess = m_testAccessWithPriority.GetAt(indx);
      fprintf(logFp, "   ACCESS [%s]  HITS [%d]\n", fptaccess->getName(), fptaccess->getUsedCount());
   }
}

//_____________________________________________________________________________
HiokiNet::HiokiNet(NetStruct& netStruct)
: m_netStruct(netStruct)
, m_name(netStruct.getNetName())
, m_netType(netTypeUnknown)
, m_isMaxHitExceeded(false)
, m_isIcGroundNet(false)
, m_powerRailTestedSurface(surfaceUnknown)
{
}

HiokiNet::~HiokiNet()
{
   m_topTestAccessCollection.empty();
   m_bottomTestAccessCollection.empty();
}

HiokiTestAccess* HiokiNet::addTestAccess(const long entityNumber, const CString name, const CPoint2d xyOrigin,
                       const HiokiSurface surface,const bool hasTakayaAccessAttrib,const int targetTypePriority,const long targetEntityNumber)
{
   HiokiTestAccess* testAccess = NULL;
   if (surface == surfaceTop)
      testAccess = m_topTestAccessCollection.addTestAccess(entityNumber, name, m_name, xyOrigin, surface, m_isIcGroundNet, hasTakayaAccessAttrib, targetTypePriority, targetEntityNumber);
   else if (surface == surfaceBottom)
      testAccess = m_bottomTestAccessCollection.addTestAccess(entityNumber, name, m_name, xyOrigin, surface, m_isIcGroundNet, hasTakayaAccessAttrib, targetTypePriority, targetEntityNumber);

   return testAccess;
}

HiokiTestAccess* HiokiNet::getTestAccess(FILE *logFp, const int maxHits, const HiokiSurface testSurface)
{
   HiokiTestAccess* testAccess = NULL;

   if (testSurface == surfaceTop)
   {
      testAccess = m_topTestAccessCollection.getTestAccess(logFp, maxHits);
   }
   else if (testSurface == surfaceBottom)
   {
      testAccess = m_bottomTestAccessCollection.getTestAccess(logFp, maxHits);
   }

   return testAccess;
}

HiokiTestAccess* HiokiNet::getTestAccessOnTarget(const long targetEntityNumber, const HiokiSurface surface)
{
   HiokiTestAccess* testAccess = NULL;

   if (surface == surfaceTop)
   {
      testAccess = m_topTestAccessCollection.getTestAccessOnTarget(targetEntityNumber);
   }
   else if (surface == surfaceBottom)
   {
      testAccess = m_bottomTestAccessCollection.getTestAccessOnTarget(targetEntityNumber);
   }

   return testAccess;
}

void HiokiNet::getTwoTestAccess(FILE *logFp, const int maxHits, const HiokiSurface testSurface, HiokiTestAccess** testAccess1, HiokiTestAccess** testAccess2)
{
   if (testSurface == surfaceTop)
   {
      m_topTestAccessCollection.getTwoTestAccess(logFp, maxHits, testAccess1, testAccess2);
   }
   else if (testSurface == surfaceBottom)
   {
      m_bottomTestAccessCollection.getTwoTestAccess(logFp, maxHits, testAccess1, testAccess2);
   }
}

void HiokiNet::writeLogFile(FILE *logFp)
{
   fprintf(logFp, "NET [%s]\n", this->getName());

   this->m_topTestAccessCollection.writeLogFile(logFp);
   this->m_bottomTestAccessCollection.writeLogFile(logFp);
}

//_____________________________________________________________________________
HiokiProbeOutput::HiokiProbeOutput(const int probeNumber, const CString netName, const CPoint2d origin,const int testAccessEntityNumber)
{
   m_probeNumber = probeNumber;
   m_netName = netName;
   m_xyOrigin = origin;
   m_testAccessEntityNumber = testAccessEntityNumber;
}

HiokiProbeOutput::~HiokiProbeOutput()
{
}

//_____________________________________________________________________________
HiokiProbeOutputCollection::HiokiProbeOutputCollection()
: m_probeOutputArray(true)
{
   m_probeOutputArray.SetSize(0, 100);
}

HiokiProbeOutputCollection::~HiokiProbeOutputCollection()
{
   m_probeOutputArray.empty();
   m_testAccessToProbeNumberMap.RemoveAll();
}

CString HiokiProbeOutputCollection::getProbeNumberString(HiokiTestAccess* testAccess, bool autoGenerate)
{
   if (testAccess != NULL)
      return getProbeNumberString(*testAccess, autoGenerate);

   return "N?";  // this is an error in output, but better than blank (giving no clue)
}

CString HiokiProbeOutputCollection::getProbeNumberString(HiokiTestAccess& testAccess, bool autoGenerate)
{
   // If autoGenerate == true then generate a probe at access point if none is already there.
   // If false then return existing probe name or empty string if none.

   CString probeNumberString;
   if ((!m_testAccessToProbeNumberMap.Lookup(testAccess.getName(), probeNumberString)) && autoGenerate)
   {  
      int probeNumber;
      if(uniqueProbeNames == true)
      {
         probeNumber = m_bottomProbeOutputCollection.getCount() + m_topProbeOutputCollection.getCount() + 1;         
      }
      else
         probeNumber = m_probeOutputArray.GetCount()+1;

      HiokiProbeOutput* probeOutput = new HiokiProbeOutput(probeNumber, testAccess.getNetName(), testAccess.getInsertOrigin(),testAccess.getAccessEntityNumber());
      m_probeOutputArray.SetAtGrow(m_probeOutputArray.GetCount(), probeOutput);

      probeNumberString.Format("%d", probeNumber);
      m_testAccessToProbeNumberMap.SetAt(testAccess.getName(), probeNumberString);
   }

   return probeNumberString;
}

int HiokiProbeOutputCollection::getCount()
{
   return m_probeOutputArray.GetCount();
}

HiokiProbeOutput* HiokiProbeOutputCollection::getAt(const int index)
{
   HiokiProbeOutput* probeOutput = NULL;
   if (index < m_probeOutputArray.GetCount())
      probeOutput = m_probeOutputArray.GetAt(index);

   return probeOutput;
}

void HiokiProbeOutputCollection::SortByProbeNumber()
{
   m_probeOutputArray.setSortFunction(HiokiProbeOutputCollection::AscendingProbeNumberFunc);
   m_probeOutputArray.sort();
}

int HiokiProbeOutputCollection::AscendingProbeNumberFunc(const void *a, const void *b)
{
   HiokiProbeOutput* probeOutput1 = *((HiokiProbeOutput**)(a));
   HiokiProbeOutput* probeOutput2 = *((HiokiProbeOutput**)(b));
   return (probeOutput1->getProbeNumber() - probeOutput2->getProbeNumber());
}

//_____________________________________________________________________________
HiokiNetMap::HiokiNetMap()
: m_netMap(nextPrime2n(20), true)
, m_powerGroundNameMap(nextPrime2n(20))
{
   m_powerNetCount = 0;
   m_groundNetCount = 0;
}

HiokiNetMap::~HiokiNetMap()
{
   empty();
}

void HiokiNetMap::empty()
{
   m_netMap.empty();
   m_powerGroundNameMap.RemoveAll();
}

HiokiNet* HiokiNetMap::addNet(NetStruct& netStruct)
{
   HiokiNet* net = findNet(netStruct.getNetName());
   if (!netStruct.getNetName().IsEmpty() && net == NULL)
   {
      net = new HiokiNet(netStruct);
      CString key = netStruct.getNetName();
      key.MakeLower();
      m_netMap.setAt(key, net);
   }

   return net;
}

HiokiNet* HiokiNetMap::findNet(const CString netName)
{
   HiokiNet* net = NULL;
   if (!netName.IsEmpty())
   {
      CString key = netName;
      key.MakeLower();
      m_netMap.Lookup(key, net);
   }

   return net;
}

CString HiokiNetMap::getPowerGroundName(HiokiNet& net)
{
   CString powerGroundName;
   if (!m_powerGroundNameMap.Lookup(net.getName(), powerGroundName))
   {
      if (net.getNetType() == netTypePower)
         powerGroundName.Format("P%d", ++m_powerNetCount);
      else
         powerGroundName.Format("G%d", ++m_groundNetCount);

      m_powerGroundNameMap.SetAt(net.getName(), powerGroundName);
   }

   return powerGroundName;
}

void HiokiNetMap::generatePowerRailShortTest(FILE *logFp, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface)
{
   CMapStringToString probePairMap;

   for (POSITION pos1=m_netMap.GetStartPosition(); pos1!=NULL;)
   {
      CString key;
      HiokiNet* net1 = NULL;
      m_netMap.GetNextAssoc(pos1, key, net1);

      // If a Power Rail short test is done on one surafce, then it can not be done on anotehr
      if (net1 != NULL && net1->getNetType() != netTypeUnknown && (net1->getPowerRailTestedSurface() == surfaceUnknown || net1->getPowerRailTestedSurface() == testSurface))
      {
         for (POSITION pos2=pos1; pos2!=NULL;)
         {
            HiokiNet* net2 = NULL;
            m_netMap.GetNextAssoc(pos2, key, net2);
            if (net2 != NULL && net2->getNetType() != netTypeUnknown && (net2->getPowerRailTestedSurface() == surfaceUnknown || net2->getPowerRailTestedSurface() == testSurface))
            {
               HiokiTestAccess* testAccess1 = net1->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
               HiokiTestAccess* testAccess2 = net2->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
               if (testAccess1 != NULL && testAccess2 != NULL)
               {
                  CString probeNumber1 = probeOutputCollection.getProbeNumberString(*testAccess1);
                  CString probeNumber2 = probeOutputCollection.getProbeNumberString(*testAccess2);

                  CString powerGroundName1 = getPowerGroundName(*net1);
                  CString powerGroundName2 = getPowerGroundName(*net2);

                  CString probePair;                       
                  if (probePairMap.Lookup(probeNumber1+probeNumber2, probePair) ||
                      probePairMap.Lookup(probeNumber2+probeNumber1, probePair))
                  {
                     fprintf(logFp,  "Duplicate Power Rail shorts test not generated %s P%s-%s\n",
                           powerGroundName1+powerGroundName2, net1->getName(), net2->getName());
                  }
                  else
                  {
                     probePairMap.SetAt(probeNumber1+probeNumber2, probeNumber1+probeNumber2);
                     probePairMap.SetAt(probeNumber2+probeNumber1, probeNumber2+probeNumber1);

                     CString output;
                     output.Format(" %s P-Auto %s-%s * R %s %s @K OP", powerGroundName1+powerGroundName2, net1->getName(), net2->getName(), probeNumber1, probeNumber2);
                     outputArray.Add(output);
                  }

                  net1->setPowerRailTestedSurface(testSurface);
                  net2->setPowerRailTestedSurface(testSurface);
               }
            }
         }
      }
   }
}

void HiokiNetMap::writeLogFile(FILE *logFp)
{
   fprintf(logFp, "\n\nNet Access Location Hit Counts\n\n");

   POSITION netpos = m_netMap.GetStartPosition();
   while (netpos != NULL)
   {
      CString netname;
      HiokiNet *fptnet;
      m_netMap.GetNextAssoc(netpos, netname, fptnet);

      if (fptnet != NULL)
         fptnet->writeLogFile(logFp);
   }
}

//_____________________________________________________________________________
HiokiCompPin::HiokiCompPin(CompPinStruct& compPinStruct, HiokiNet& net)
: m_compPinStruct(compPinStruct)
, m_net(net)
{
   m_alreadyTested = false;
   m_testedSurface = surfaceUnknown;
   m_topTestAccess = NULL;
   m_bottomTestAccess = NULL;
}

HiokiCompPin::~HiokiCompPin()
{
   m_topTestAccess = NULL;
   m_bottomTestAccess = NULL;
}

HiokiTestAccess* HiokiCompPin::getTestAccess(FILE *logFp, const int maxHits, const HiokiSurface testSurface)
{
   HiokiTestAccess* testAccess = NULL;
   if (testSurface == surfaceTop)
   {
      if (m_topTestAccess == NULL)
         m_topTestAccess = m_net.getTestAccess(logFp, maxHits, testSurface);

      testAccess = m_topTestAccess;
   }
   else if (testSurface == surfaceBottom)
   {
      if (m_bottomTestAccess == NULL)
         m_bottomTestAccess = m_net.getTestAccess(logFp, maxHits,testSurface);

      testAccess = m_bottomTestAccess;
   }

   return testAccess;
}

HiokiTestAccess* HiokiCompPin::getTestAccessOnSelf(const HiokiSurface testSurface)
{
   // If self has an access marker on surface, return it.
   // If not, return NULL, even if there is other access on same net.

   return m_net.getTestAccessOnTarget(this->getEntityNumber(), testSurface);
}

//_____________________________________________________________________________
HiokiCompPinMap::HiokiCompPinMap(bool isContainer)
: m_compPinMap(nextPrime2n(20), isContainer)
{
   m_alreadySorted = false;
}

HiokiCompPinMap::~HiokiCompPinMap()
{
   empty();
}

void HiokiCompPinMap::empty()
{
   m_compPinMap.empty();
}

void HiokiCompPinMap::addCompPin(HiokiCompPin* compPin)
{
   if (compPin != NULL)
   {
      if (findCompPin(compPin->getCompPinName()) == NULL)
      {
         m_compPinMap.SetAt(compPin->getCompPinName(), compPin);
      }
   }
}

HiokiCompPin* HiokiCompPinMap::addCompPin(CompPinStruct& compPinStruct, HiokiNet& net)
{
   HiokiCompPin* compPin = findCompPin(compPinStruct.getPinRef());
   if (!compPinStruct.getPinRef().IsEmpty() && compPin == NULL)
   {
      compPin = new HiokiCompPin(compPinStruct, net);
      m_compPinMap.SetAt(compPin->getCompPinName(), compPin);
   }

   return compPin;
}

HiokiCompPin* HiokiCompPinMap::findCompPin(const CString compPinName)
{
   HiokiCompPin* compPin = NULL;
   CString key = compPinName;
   key.MakeLower();
   m_compPinMap.Lookup(key, compPin);

   return compPin;
}

POSITION HiokiCompPinMap::getStartPosition() const
{ 
   return m_compPinMap.GetStartPosition(); 
}

HiokiCompPin* HiokiCompPinMap::getNext(POSITION& pos)     
{
   CString key;
   HiokiCompPin* compPin = NULL;
   if (pos != NULL)
      m_compPinMap.GetNextAssoc(pos, key, compPin);

   return compPin;
}

void HiokiCompPinMap::sort()
{
   m_compPinMap.Sort();
   m_alreadySorted = true;
}

HiokiCompPin* HiokiCompPinMap::getFirstSorted()
{
   HiokiCompPin* compPin = NULL;
   CString* key = NULL;
   m_compPinMap.GetFirstSorted(key, compPin);

   return compPin;
}

HiokiCompPin* HiokiCompPinMap::getNextSorted()
{
   HiokiCompPin* compPin = NULL;
   CString* key = NULL;
   m_compPinMap.GetNextSorted(key, compPin);

   return compPin;
}

//_____________________________________________________________________________
HiokiComponent::HiokiComponent(FileStruct& file, InsertStruct& insert)
: m_file(file)
, m_insert(insert)
, m_compPinMap(false)
{
   // Initialzie the following members
   m_name = insert.getRefname();
   m_hiokiName = insert.getRefname();
   m_deviceType = deviceTypeUndefined;
   m_surface = insert.getGraphicMirrored()?surfaceBottom:surfaceTop;
   m_isIcOpenTest = false;
   m_ipOpenTestDone = false;
   m_isPinShortTestDone = false;
   m_isLoaded = true;
   m_isSmd = true;
   m_pinCount = 0;
   m_compHeight = 0;
   m_writeSelectVisionTest = true;
   m_value = "*";
   m_comment = "*";
   m_gridLocation = "*";
}

HiokiComponent::~HiokiComponent()
{
   m_compPinMap.empty();
}

void HiokiComponent::addCompPin(HiokiCompPin* compPin)
{
   m_compPinMap.addCompPin(compPin);
}

HiokiCompPin* HiokiComponent::findCompPin(const CString pinName)
{
   for (POSITION pos=m_compPinMap.getStartPosition(); pos!=NULL;)
   {
      HiokiCompPin* compPin = m_compPinMap.getNext(pos);
      if (compPin != NULL && compPin->getPinName().CompareNoCase(pinName) == 0)
         return compPin;
   }

   return NULL;
}

void HiokiComponent::lookupAttributes(CCamCadData& camCadData, CAttributes* attributes, HiokiSettings &settings)
{
   CString ppCommandAttribName( settings.getPPCommandAttribName() );
   CString commentAttribute( settings.getCommentAttribute() );
   CString icOpenDescription( settings.getIcOpenDescription() );
   bool isIcOpenDescriptionAttrib = settings.isIcOpenDescriptionAttrib();

   BlockStruct* insertBlock = camCadData.getBlockAt(m_insert.getBlockNumber());
   if (insertBlock != NULL)
      m_pinCount = insertBlock->getPinCount();

   if (attributes != NULL)
   {
      CString attribValue;
      CAttribute* attrib = NULL;

      // Get device type
      if (camCadData.getAttributeStringValue(attribValue, *attributes, standardAttributeDeviceType))
         m_deviceType = stringToDeviceTypeTag(attribValue);

      // Initalize tollerance base on device type
      double positiveTol, negativeTol;
      if (m_deviceType == deviceTypeDiode || m_deviceType == deviceTypeDiodeLed || m_deviceType == deviceTypeDiodeZener ||
         m_deviceType == deviceTypeIC || m_deviceType == deviceTypeICDigital || m_deviceType == deviceTypeICLinear)
      {
         positiveTol = 25.;
         negativeTol = 25.;
      }
      else
      {
         positiveTol = 10.;
         negativeTol = 10.;
      }

      // Now get the actually positive tolerance from attribute
      if (attributes->Lookup(camCadData.getAttributeKeywordIndex(standardAttributePlusTolerance), attrib))
         positiveTol = attrib->getDoubleValue();
      else if (attributes->Lookup(camCadData.getAttributeKeywordIndex(standardAttributeTolerance), attrib))
         positiveTol = attrib->getDoubleValue();

      if (positiveTol < 1)
         positiveTol = 1;
       m_positiveTol.Format("%.1f", positiveTol);
   
      // Now get the actually negative tolerance from attribute
      if (attributes->Lookup(camCadData.getAttributeKeywordIndex(standardAttributeMinusTolerance), attrib))
         negativeTol = attrib->getDoubleValue();
      else if (attributes->Lookup(camCadData.getAttributeKeywordIndex(standardAttributeTolerance), attrib))
         negativeTol = attrib->getDoubleValue();

      if (negativeTol < 1)
         negativeTol = 1;
      m_negativeTol.Format("%.1f", -negativeTol);  // Is positive val in data, make negative for output.

      // Height
      if (attributes->Lookup(camCadData.getAttributeKeywordIndex(standardAttributeComponentHeight), attrib))
         m_compHeight = attrib->getDoubleValue();

      camCadData.getAttributeStringValue(m_partNumber, *attributes, standardAttributePartNumber);
      camCadData.getAttributeStringValue(m_testStrategy, *attributes, standardAttributeTestStrategy);
      camCadData.getAttributeStringValue(m_subClass, *attributes, standardAttributeSubclass);
      m_aptModelName = m_subClass;

      camCadData.getAttributeStringValue(m_gridLocation, *attributes, standardAttributeGridLocation);
      if (m_gridLocation.IsEmpty())
         m_gridLocation = DEFAULT_GRID_LOCATION;

      if (!ppCommandAttribName.IsEmpty())
      {
      if (attributes->Lookup(camCadData.getAttributeKeywordIndex(ppCommandAttribName), attrib))
         m_ppCommand = camCadData.getAttributeStringValue(*attrib);
      }

      if (attributes->Lookup(camCadData.getAttributeKeywordIndex(commentAttribute), attrib))
         m_comment = camCadData.getAttributeStringValue(*attrib); 
      if (attributes->Lookup(camCadData.getAttributeKeywordIndex("MergedStatus"), attrib))
         m_mergedStatus = camCadData.getAttributeStringValue(*attrib); 
      if (attributes->Lookup(camCadData.getAttributeKeywordIndex("MergedValue"), attrib))
         m_mergedValue = camCadData.getAttributeStringValue(*attrib); 
      if (attributes->Lookup(camCadData.getAttributeKeywordIndex(ATT_CAP_OPENS), attrib))
      {
         attribValue = camCadData.getAttributeStringValue(*attrib); 
         if (attribValue.CompareNoCase("true") == 0)
         {
            m_isIcOpenTest = true;
            m_icOpenDescription = icOpenDescription;

            if (isIcOpenDescriptionAttrib)
            {
               if (attributes->Lookup(camCadData.getAttributeKeywordIndex(icOpenDescription), attrib))
                  m_icOpenDescription = camCadData.getAttributeStringValue(*attrib);
            }

            if (m_icOpenDescription.IsEmpty())
               m_icOpenDescription = "ICOP";
         }
      }

      CString loadedValue;
      camCadData.getAttributeStringValue(loadedValue, *attributes, standardAttributeLoaded);
      m_isLoaded = (loadedValue.CompareNoCase("FALSE") != 0);

      CString technologyValue;
      camCadData.getAttributeStringValue(technologyValue, *attributes, standardAttributeTechnology);
      m_isSmd = (technologyValue.CompareNoCase("SMD") == 0);

      // Get value and format value units
      camCadData.getAttributeStringValue(m_value, *attributes, standardAttributeValue);
      {
         if (m_mergedStatus.CompareNoCase("Primary") == 0)
            m_value = m_mergedValue;
         m_value.Replace(" ", "");      

         // Extract the number from the value string
         CString tmpValue; 
         for (int i=0; i<m_value.GetLength(); i++)
         {
            CString tmp = m_value.GetAt(i);
            
            if (is_number(tmp) || tmp == ".")
            {
               tmpValue.Append(tmp);
            }
            //To get the units modifier, e.g. the K in 1.5KOhms, okay if it gets units instead (e.g. O or H).
            else
            {
               tmpValue.Append(tmp);
               break;
            }           
         }
         m_value = tmpValue;
         double tmp = atof(m_value);
         if(tmp == 0.0)
            m_value = DEFAULT_COMP_VALUE;
      }

      // Set element and modify other proper base on device type
      switch (m_deviceType)
      {
      case deviceTypeJumper:
         {
            m_element = "R";
			   m_value = ""; // Takaya: m_value = "0O";
         }
         break;

      case deviceTypeFuse:
         {
             m_element = "R";
             m_value = ""; // Takaya: m_value = "*";
         }
         break;

      case deviceTypeResistor:
      case deviceTypeResistorArray:
         {
            m_element = "R";
            
            if (m_value != "*")
            {
               //*rcf Takaya stuff, probably don't want for Hioki
               //m_value.Replace("Ohms", "O"); // value has "Ohms" as suffix, then replace with "O"
               //m_value.Replace("Ohm", "O");  // value has "Ohm" as suffix, then replace with "O"
               //if (m_value.Right(1) != "O")    // value does not end in "O", then attach "O"
               //   m_value += "O";
            }
            if (m_deviceType == deviceTypeResistor && m_mergedStatus.CompareNoCase("primary") == 0)
               m_hiokiName.AppendFormat("*");
         }
         break;

      case deviceTypeCapacitor:     
      case deviceTypeCapacitorArray:
      case deviceTypeCapacitorPolarized:
      case deviceTypeCapacitorTantalum:
         {
            m_element = "C";           

            if (m_value != "*")
            {
               //*rcf Takaya stuff, not Hioki?
               //m_value.Replace("FARADS", "F"); // value has "FARADS" as suffix, then replace with "F"
               //m_value.Replace("FARAD", "F");  // value has "FARAD" as suffix, then replace with "F"
               //if (m_value.Right(1) != "F")    // value does not end in "F", then attach "F"
               //   m_value += "F";
            }
            if (m_deviceType != deviceTypeCapacitorArray && m_mergedStatus.CompareNoCase("primary") == 0)
               m_hiokiName.AppendFormat("*");           
         }
         break;

      case deviceTypeInductor:      
         {
            m_element = "L";

            if (m_value != "*")
            {
               //*rcf Takaya stuff
               //m_value.Replace("HENRY", "H");     // value has "HENRY" as suffix, then replace with "H"
               //m_value.Replace("HERRIES", "H");   // value has "HERRIES" as suffix, then replace with "H"
               //if (m_value.Right(1) != "H")       // value does not end with "H", then attach "H"
               //   m_value += "H";
            }

            // Due to case dts0100374680, this is commmented out
		      //if (m_mergedStatus.CompareNoCase("primary") == 0)
		      //	element = "L*";        
         }
         break;

      case deviceTypeIC: 
      case deviceTypeICDigital:
      case deviceTypeICLinear:
         {
            // This will be set later at HiokiComponentCollection::generateIcTest()
            // base on whether it is a IcCapacitor test.  If it is then set to "C"
            // else set to "D" 
            m_element = "";            
         }
         break;

      case deviceTypeDiode:
      case deviceTypeDiodeLed:
      case deviceTypeDiodeZener:
      case deviceTypeDiodeArray:
         {
            m_element = "D"; 

            if (m_deviceType != deviceTypeDiodeZener)
               m_value = "*";
         }
         break;

      case deviceTypeTransistor:
      case deviceTypeTransistorNpn:
      case deviceTypeTransistorPnp:
      case deviceTypeTransistorFetNpn:
      case deviceTypeTransistorMosfetNpn:
      case deviceTypeTransistorFetPnp:
      case deviceTypeTransistorMosfetPnp:
      case deviceTypeTransistorArray:
      case deviceTypeOpto:
         {
            m_element = "D";           
         }
         break;

      default:                           
         {                          
         }
         break; 
      }
      
      if (m_mergedStatus.CompareNoCase("ignored") == 0 || (!m_testStrategy.IsEmpty() && m_testStrategy.Find(ATT_VALUE_TEST_FPT) < 0))
         m_commentOperator = "//";
   }
}

CString HiokiComponent::getHiokiValue()
{
   ComponentValues compVal;
   compVal.SetValue( this->getValue() );
   double sciVal = compVal.getScientificValue();
   CString valStr;
   valStr.Format("%.3E", sciVal);
   return valStr;
}

int HiokiComponent::generateTestPinCount(FILE *logFp, const int maxHits, const HiokiSurface testSurface)
{
   int unUsedPin = 0;
   int testAccessCount = 0; 

   m_compPinMap.sort();

   for (POSITION pos=m_compPinMap.getStartPosition(); pos!=NULL;)
   {
      HiokiCompPin* compPin = m_compPinMap.getNext(pos);
      if (compPin->getTestedSurface() == surfaceUnknown)
      {
         unUsedPin++;
      }

      if (compPin->getTestAccess(logFp, maxHits, testSurface) != NULL)
      {
         testAccessCount++;
      }
   }

   if (unUsedPin <= 0 )    //*rcf  This looks odd to me, I doubt it is correct, but don't have proof at the moment
      testAccessCount = 0;

   return testAccessCount;
}

//_____________________________________________________________________________
HiokiComponentCollection::HiokiComponentCollection(CGTabTableList *tableList)
: m_componentMapByName(nextPrime2n(20), true)
, m_resistorComponents(false)
, m_capacitorComponents(false)
, m_diodeComponents(false)
, m_inductorComponents(false)
, m_icComponents(false)
, m_transitorComponents(false)
, m_capacitorPolarizedComponents(false)
, m_capacitorTantalumComponents(false)
, m_otherComponents(false)
, m_testPoints(false)
{
   m_subclassList.Load(tableList);
}

HiokiComponentCollection::~HiokiComponentCollection()
{
   empty();
}

void HiokiComponentCollection::empty()
{
   m_componentMapByName.empty();
   m_resistorComponents.empty();
   m_capacitorComponents.empty();
   m_inductorComponents.empty();
   m_icComponents.empty();
   m_diodeComponents.empty();
   m_transitorComponents.empty();
   m_capacitorPolarizedComponents.empty();
   m_capacitorTantalumComponents.empty();
   m_otherComponents.empty();
   m_testPoints.empty();
   m_pinToPinProbePairMap.RemoveAll();
}

void HiokiComponentCollection::applyTakayaOrigin(CPoint2d &location, bool topside)
{
   if (topside)
   {
      location.x -= m_topOutputOrigin.x;
      location.y -= m_topOutputOrigin.y;
   }
   else
   {
      location.x -= m_botOutputOrigin.x;
      location.y -= m_botOutputOrigin.y;
   }
}

void HiokiComponentCollection::convertToHiokiUnits(CPoint2d& location, double unitFactor, bool topside)
{
   applyTakayaOrigin(location, topside);

   ::convertToHiokiUnits(location.x, unitFactor);
   ::convertToHiokiUnits(location.y, unitFactor);
}

void HiokiComponentCollection::moveIcDeviceTypeToOtherComponent()
{
   while (m_icComponents.GetHeadPosition() != NULL)
   {
      POSITION pos = m_icComponents.GetHeadPosition();
      HiokiComponent* component = m_icComponents.GetAt(pos);
      m_icComponents.RemoveAt(pos);

      // Add to m_otherComponents after removing it from IC component array
      if (component != NULL)
      {
         component->unsetDeviceType();
         m_otherComponents.AddTail(component);
      }
   }

   m_otherComponents.empty();
}

HiokiComponent* HiokiComponentCollection::addComponent(CCamCadData& camCadData, FileStruct& file, DataStruct& data, HiokiSettings& settings, CTMatrix transformMatrix)
{
   InsertStruct* insert = data.getInsert();
   if (insert == NULL)
      return NULL;

   HiokiComponent* component = findComponent(insert->getRefname());
   if (!insert->getRefname().IsEmpty() && component == NULL)
   {
      // Get origin at board level
      CPoint2d origin = insert->getOrigin2d();
      transformMatrix.transform(origin);
      convertToHiokiUnits(origin, settings.getUnitFactor(), insert->getPlacedTop());

      component = new HiokiComponent(file, *insert);
      component->setInsertOrigin(origin);
      component->lookupAttributes(camCadData, data.getAttributes(), settings);

      CString key = component->getName();
      key.MakeLower();
      m_componentMapByName.setAt(key, component);

      if (insert->getInsertType() == insertTypeTestPoint)
      {
         this->m_testPoints.AddTail(component);
      }
      else
      {
         switch (component->getDeviceType())
         {
         case deviceTypeResistor:
         case deviceTypeJumper:
         case deviceTypeFuse:
         case deviceTypeResistorArray:
            {
               m_resistorComponents.AddTail(component); 
               break;
            }
         case deviceTypeCapacitor:     
         case deviceTypeCapacitorArray:
            {
               m_capacitorComponents.AddTail(component);           
               break;
            }
         case deviceTypeInductor:      
            {
               m_inductorComponents.AddTail(component);           
               break;
            }
         case deviceTypeIC: 
         case deviceTypeICDigital:
         case deviceTypeICLinear:
            {
               m_icComponents.AddTail(component);           
               break;
            }
         case deviceTypeDiode:
         case deviceTypeDiodeLed:
         case deviceTypeDiodeZener:
         case deviceTypeDiodeArray:
            {
               m_diodeComponents.AddTail(component);           
               break;
            }
         case deviceTypeTransistor:
         case deviceTypeTransistorNpn:
         case deviceTypeTransistorPnp:
         case deviceTypeTransistorFetNpn:
         case deviceTypeTransistorMosfetNpn:
         case deviceTypeTransistorFetPnp:
         case deviceTypeTransistorMosfetPnp:
         case deviceTypeTransistorArray:
         case deviceTypeOpto:
            {
               m_transitorComponents.AddTail(component);           
               break;
            }
         case deviceTypeCapacitorPolarized:
            {
               m_capacitorPolarizedComponents.AddTail(component);           
               break;
            }
         case deviceTypeCapacitorTantalum: 
            {
               m_capacitorTantalumComponents.AddTail(component);           
               break;
            }
         default:                           
            {
               m_otherComponents.AddTail(component);              
               break;                
            }
         }
      }
   }

   return component;
}

HiokiComponent* HiokiComponentCollection::findComponent(const CString name)
{
   HiokiComponent* component = NULL;
   CString key = name;
   key.MakeLower();
   m_componentMapByName.Lookup(name, component);
   return component;
}

void HiokiComponentCollection::generateComponentTest(CCamCadData& camcadData, FILE *logFp, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
                                                         HiokiNet* groundNet, HiokiIcDiodeCommandMap& icDiodeCommandMap, HiokiSettings& settings, const HiokiSurface testSurface)
{
   m_visionTestCount = 0;
   globalStepNo = 1;  // reset cos file output record counter

   generateResistorTest(logFp, adptModelMap, probeOutputCollection, outputArray, settings, testSurface);
   generateCapacitorTest(logFp, adptModelMap, probeOutputCollection, outputArray, settings, testSurface);
   generateDiodeTest(logFp, adptModelMap, probeOutputCollection, outputArray, settings, testSurface);
   generateInductorTest(logFp, adptModelMap, probeOutputCollection, outputArray, settings, testSurface);
   if (groundNet != NULL)
   {
      if (settings.getExportIcCapacitor())
         generateIcTest(camcadData, logFp, adptModelMap, probeOutputCollection, outputArray, *groundNet, icDiodeCommandMap, false, settings, testSurface); // element = "C" and value = "40PF" if ic_capacitor test, else it is "D", and takayaRefName has prefix base on if test type
      if (settings.getExportIcDiode())
         generateIcTest(camcadData, logFp, adptModelMap, probeOutputCollection, outputArray, *groundNet, icDiodeCommandMap, true, settings, testSurface); // element = "C" and value = "40PF" if ic_capacitor test, else it is "D", and takayaRefName has prefix base on if test type
   }
   generateTransistorTest(logFp, adptModelMap, probeOutputCollection, outputArray, settings, testSurface);
   generateCapacitorPolarizedTest(camcadData, logFp, adptModelMap, probeOutputCollection, outputArray, groundNet, settings, testSurface);
   generateCapacitorTantalumTest(camcadData, logFp, adptModelMap, probeOutputCollection, outputArray, groundNet, settings, testSurface);
   generateUnsupportedDeviceTypeTest(logFp, adptModelMap, probeOutputCollection, outputArray, settings, testSurface);

   if (settings.getShortType() == shortTypePinToPin)
   {
      generatePinToPinShortTest(camcadData, logFp, m_resistorComponents, probeOutputCollection, outputArray, settings, testSurface);
      generatePinToPinShortTest(camcadData, logFp, m_capacitorComponents, probeOutputCollection, outputArray, settings, testSurface);
      generatePinToPinShortTest(camcadData, logFp, m_diodeComponents, probeOutputCollection, outputArray, settings, testSurface);
      generatePinToPinShortTest(camcadData, logFp, m_inductorComponents, probeOutputCollection, outputArray, settings, testSurface);
      generatePinToPinShortTest(camcadData, logFp, m_icComponents, probeOutputCollection, outputArray, settings, testSurface);
      generatePinToPinShortTest(camcadData, logFp, m_transitorComponents, probeOutputCollection, outputArray, settings, testSurface);
      generatePinToPinShortTest(camcadData, logFp, m_capacitorPolarizedComponents, probeOutputCollection, outputArray, settings, testSurface);
      generatePinToPinShortTest(camcadData, logFp, m_capacitorTantalumComponents, probeOutputCollection, outputArray, settings, testSurface);
      generatePinToPinShortTest(camcadData, logFp, m_otherComponents, probeOutputCollection, outputArray, settings, testSurface);
   }
}

void HiokiComponentCollection::generateResistorTest(FILE *logFp, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface)
{
   for (POSITION pos=m_resistorComponents.GetHeadPosition(); pos!=NULL;)
   {
      HiokiComponent* component = m_resistorComponents.GetNext(pos);
      if (component != NULL)
      { 
         generateGenericTest(logFp, *component, adptModelMap, probeOutputCollection, outputArray, settings, testSurface);
      }
   }
}

void HiokiComponentCollection::generateCapacitorTest(FILE *logFp, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface)
{
   for (POSITION pos=m_capacitorComponents.GetHeadPosition(); pos!=NULL;)
   {
      HiokiComponent* component = m_capacitorComponents.GetNext(pos);
      if (component != NULL)
      { 
         generateGenericTest(logFp, *component, adptModelMap, probeOutputCollection, outputArray, settings, testSurface);
      }
   }
}

void HiokiComponentCollection::generateDiodeTest(FILE *logFp, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface)
{
   for (POSITION pos=m_diodeComponents.GetHeadPosition(); pos!=NULL;)
   {
      HiokiComponent* component = m_diodeComponents.GetNext(pos);
      if (component != NULL)
      {
         int testPinCount = component->generateTestPinCount(logFp, settings.getMaxHitCount(), testSurface);
         if (testPinCount > 0)
         {
            HiokiAptModel* aptModel = adptModelMap.lookup(component->getAptModelName());
            if (aptModel != NULL)
            {
               // Generate test using aptModel
               aptModel->generateAptModelTest(logFp, *component, probeOutputCollection, outputArray, settings, testSurface);
            }
            else
            {
               HiokiCompPin* anodePin = NULL;
               HiokiCompPin* cathodePin = NULL;
         
               HiokiCompPin* compPin = component->getFirstSorted();
               while (compPin != NULL)
               {
                  if (compPin->getPinMap().CompareNoCase(PIN_MAP_ANODE) == 0)
                     anodePin = compPin;
                  else if (compPin->getPinMap().CompareNoCase(PIN_MAP_CATHODE) == 0)
                     cathodePin = compPin;
                        
                  compPin = component->getNextSorted();
               }

               if (generateOutput(logFp, *component, anodePin, cathodePin, probeOutputCollection, outputArray, settings, testSurface))
                  component->SetWriteSelectVisionTest(false);
            }
         }

         // If Paralleled test generate but has been commented out, then write out a vision test
         if (component->getCommentOperator() == "//" && component->getMergedStatus().CompareNoCase("Ignored") == 0)
            component->SetWriteSelectVisionTest(true);

         // Determine if a vision test is needed
         if (settings.getAllComponentVisionTest() || (settings.getSelectVisionTest() && component->GetWriteSelectVisionTest()))
         {
            generateVisionOutput(logFp, *component, outputArray, settings, testSurface);
         }
      }
   }
}

void HiokiComponentCollection::generateInductorTest(FILE *logFp, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface)
{
   for (POSITION pos=m_inductorComponents.GetHeadPosition(); pos!=NULL;)
   {
      HiokiComponent* component = m_inductorComponents.GetNext(pos);
      if (component != NULL)
      { 
         generateGenericTest(logFp, *component, adptModelMap, probeOutputCollection, outputArray, settings, testSurface);
      }
   }
}

void HiokiComponentCollection::generateIcTest(CCamCadData& camcadData, FILE *logFp, HiokiAptModelMap& adptModelMap, 
                                                  HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
                                                  HiokiNet& groundNet, HiokiIcDiodeCommandMap& icDiodeCommandMap, 
                                                  const bool isIcDiodeTest, HiokiSettings& settings, const HiokiSurface testSurface)
{
   // No IC test in Hioki.
}

void HiokiComponentCollection::generateTransistorTest(FILE *logFp, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface)
{
   for (POSITION pos=m_transitorComponents.GetHeadPosition(); pos!=NULL;)
   {
      HiokiComponent* component = m_transitorComponents.GetNext(pos);
      if (component != NULL)
      {
         int testPinCount = component->generateTestPinCount(logFp, settings.getMaxHitCount(), testSurface);
         if (testPinCount > 0)
         {
            HiokiAptModel* aptModel = adptModelMap.lookup(component->getAptModelName());
            CSubclass *sc = m_subclassList.findSubclass(component->getAptModelName());
            if (aptModel != NULL)
            {
               // Generate test using aptModel
               aptModel->generateAptModelTest(logFp, *component, probeOutputCollection, outputArray, settings, testSurface);
            }
            else if(sc != NULL && isValidDeviceTypeForMDBUse(component->getDeviceType()))            
            {
               generateOutputUsingMDB(logFp, *component, probeOutputCollection, outputArray, settings, testSurface, sc);            
            }
            else
            {
               HiokiCompPin* anodePin = NULL;
               HiokiCompPin* cathodePin = NULL;
               HiokiCompPin* basePin = NULL;
               HiokiCompPin* collectorPin = NULL;
               HiokiCompPin* emitterPin = NULL;
               HiokiCompPin* gatePin = NULL;
               HiokiCompPin* sourcePin = NULL;
               HiokiCompPin* drainPin = NULL;

               CString anodeProbeNum, cathodeProbeNum, baseProbeNum, collectorProbeNum, emitterProbeNum, gateProbeNum, sourceProbeNum, drainProbeNum;

               HiokiCompPin* compPin = component->getFirstSorted();
               while (compPin != NULL)
               {
                  HiokiPinMapType pinMapType = pinMapTypeStringToTag(compPin->getPinMap());
                  HiokiTestAccess* testAccess = compPin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
                  if (testAccess != NULL && (compPin->getTestedSurface() == surfaceUnknown || compPin->getTestedSurface() == testSurface))
                  {
                     CString probeNumber = probeOutputCollection.getProbeNumberString(*testAccess);

                     switch (pinMapType)
                     {
                     case pinMapTypeAnode:
                        {
                           anodePin = compPin;
                           anodeProbeNum = probeNumber;
                        }
                        break;
                     case pinMapTypeCathode:
                        {
                           cathodePin = compPin;
                           cathodeProbeNum = probeNumber;
                        }
                        break;
                     case pinMapTypeBase:
                        {
                           basePin = compPin;
                           baseProbeNum = probeNumber;
                        }
                        break;
                     case pinMapTypeCollector:  
                        {
                           collectorPin = compPin;
                           collectorProbeNum = probeNumber;
                        }
                        break;
                     case pinMapTypeEmitter: 
                        {
                           emitterPin = compPin;
                           emitterProbeNum = probeNumber;
                        }
                        break;
                     case pinMapTypeGate:       
                        {
                           gatePin = compPin;
                           gateProbeNum = probeNumber;
                        }
                        break;
                     case pinMapTypeSource:
                        {
                           sourcePin = compPin;
                           sourceProbeNum = probeNumber;
                        }
                        break;
                     case pinMapTypeDrain: 
                        {
                           drainPin = compPin;
                           drainProbeNum = probeNumber;     
                        }
                        break;
                     default:                                                    
                        break;
                     }
                  }

                  compPin = component->getNextSorted();
               }

               if(component->getDeviceType() == deviceTypeOpto)
               {
                  //*rcf Looks like no Opto support in Hioki, probably need to disable this section, or convert tests?
                  if (anodePin!=NULL && cathodePin!=NULL && collectorPin!=NULL && emitterPin!=NULL)
                  {
                     CString annotation;
                     CString output;
                     CString partNumber = component->getPartNumber().IsEmpty()?"*":component->getPartNumber();

                     if (settings.getAnnotated())
                     {
                        //output.Format("TAKAYA:%s@PC %s %s * %s %s %s %s", component->getCommentOperator(), component->getHiokiName(), partNumber,
                        //   anodeProbeNum, cathodeProbeNum, emitterProbeNum, collectorProbeNum);
                        //outputArray.Add(output);

                        //output.Format("TAKAYA:%s@CE", component->getCommentOperator());
                        //outputArray.Add(output);

                        //output.Format("TAKAYA:%s%s C-E %s * R %s %s @K OP @K JP", component->getCommentOperator(), component->getHiokiName(), partNumber,
                        //   collectorProbeNum, emitterProbeNum);
                        //outputArray.Add(output);

                        //output.Format("TAKAYA:%s%s A-K %s * D %s %s @T 25 25 @K JP", component->getCommentOperator(), component->getHiokiName(), partNumber,
                        //   anodeProbeNum, cathodeProbeNum);

                        annotation.Format(" ! Transistor  RefDes[%s]  AnodeProbe[%s] CathodeProbe[%s]",
                           component->getHiokiName(), anodeProbeNum, cathodeProbeNum);

                        //outputArray.Add(output);
                     }

                     //*rcf new Hioki
                     HiokiCosRecord cosRec(component->getHiokiName(), component, anodeProbeNum, cathodeProbeNum);
                     cosRec.GetFormattedRecord(globalStepNo++, output);
                     output += annotation;
                     outputArray.Add(output);

                     anodePin->setTestedSurface(testSurface);
                     anodePin->setAlreadyTested(true);
                     cathodePin->setTestedSurface(testSurface);
                     cathodePin->setAlreadyTested(true);
                     collectorPin->setTestedSurface(testSurface);
                     collectorPin->setAlreadyTested(true);
                     emitterPin->setTestedSurface(testSurface);
                     emitterPin->setAlreadyTested(true);
                     component->SetWriteSelectVisionTest(false);
                  }
               }
               else
               {

                  CString output, comment;
                  if (component->getDeviceType() == deviceTypeTransistorPnp)
                  {
                     comment = component->getComment()=="*"?"PNP":component->getComment();
                     output.Format("TAKAYA:%s@Q2 %s %s * @T 10 10", component->getCommentOperator(), component->getHiokiName(), comment);
                  }
                  else if (component->getDeviceType() == deviceTypeTransistor || component->getDeviceType() == deviceTypeTransistorNpn)
                  {
                     comment = component->getComment()=="*"?"NPN":component->getComment();
                     output.Format("TAKAYA:%s@Q2 %s %s * @T 10 10", component->getCommentOperator(), component->getHiokiName(), comment);
                  }
                  else if (component->getDeviceType() == deviceTypeTransistorFetNpn || component->getDeviceType() == deviceTypeTransistorMosfetNpn)
                  {
                     comment = component->getComment()=="*"?"FET_N":component->getComment();
                     output.Format("TAKAYA:%s@F %s %s * @MM FETEN", component->getCommentOperator(), component->getHiokiName(), comment);
                  }
                  else if (component->getDeviceType() == deviceTypeTransistorFetPnp || component->getDeviceType() == deviceTypeTransistorMosfetPnp)
                  {
                     comment = component->getComment()=="*"?"FET_P":component->getComment();
                     output.Format("TAKAYA:%s@F %s %s * @MM FETDP", component->getCommentOperator(), component->getHiokiName(), comment);
                  }

                  if (basePin != NULL && collectorPin != NULL && emitterPin != NULL)
                  {
                     CString annotation;
                     if (settings.getAnnotated())
                     {
                        //outputArray.Add(output);  // This is from what  is formatted above

                       // output.Format("TAKAYA:%s %s", component->getCommentOperator(), baseProbeNum);
                       // outputArray.Add(output);

                        //output.Format("TAKAYA:%s %s", component->getCommentOperator(), collectorProbeNum);
                        //outputArray.Add(output);

                        //output.Format("TAKAYA:%s %s", component->getCommentOperator(), emitterProbeNum);
                        //outputArray.Add(output);

                        //output.Format("TAKAYA:%s@CE", component->getCommentOperator());
                        annotation.Format(" ! Transistor  RefDes[%s] BaseProbe[%s] EmitterProbe[%s]",
                           component->getHiokiName(), baseProbeNum, emitterProbeNum);

                        //outputArray.Add(output);
                     }

                     //*rcf new Hioki
                     HiokiCosRecord cosRec(component->getHiokiName(), component, HIOKI_TEST_TRANSISTOR, baseProbeNum, emitterProbeNum);
                     cosRec.GetFormattedRecord(globalStepNo++, output);
                     output += annotation;
                     outputArray.Add(output);

                     basePin->setTestedSurface(testSurface);
                     basePin->setAlreadyTested(true);
                     collectorPin->setTestedSurface(testSurface);
                     collectorPin->setAlreadyTested(true);
                     emitterPin->setTestedSurface(testSurface);
                     emitterPin->setAlreadyTested(true);

                     component->SetWriteSelectVisionTest(false);
                  }
                  else if (gatePin != NULL && sourcePin != NULL && drainPin != NULL)
                  {
                     CString annotation;
                     if (settings.getAnnotated())
                     {
                        //outputArray.Add(output);

                        //output.Format("TAKAYA:%s %s", component->getCommentOperator(), gateProbeNum);
                        //outputArray.Add(output);

                        //output.Format("TAKAYA:%s %s", component->getCommentOperator(), drainProbeNum);
                        outputArray.Add(output);

                        //output.Format("TAKAYA:%s %s", component->getCommentOperator(), sourceProbeNum);
                        //outputArray.Add(output);

                        //output.Format("TAKAYA:%s@CE", component->getCommentOperator());

                        annotation.Format(" ! Transistor  RefDes[%s] GateProbe[%s] DrainProbe[%s]",
                           component->getHiokiName(), gateProbeNum, drainProbeNum);

                        //outputArray.Add(output);
                     }

                     //*rcf new Hioki
                     HiokiCosRecord cosRec(component->getHiokiName(), component, HIOKI_TEST_TRANSISTOR, gateProbeNum, drainProbeNum);
                     cosRec.GetFormattedRecord(globalStepNo++, output);
                     output += annotation;
                     outputArray.Add(output);

                     gatePin->setTestedSurface(testSurface);
                     gatePin->setAlreadyTested(true);
                     sourcePin->setTestedSurface(testSurface);
                     sourcePin->setAlreadyTested(true);
                     drainPin->setTestedSurface(testSurface);
                     drainPin->setAlreadyTested(true);

                     component->SetWriteSelectVisionTest(false);
                  }
               }
            }

            if(aptModel == NULL && !component->getAptModelName().IsEmpty())
               fprintf(logFp, "Subclass %s of component %s not found in Takaya model library\n", component->getAptModelName(), component->getName());
            if(sc == NULL && isValidDeviceTypeForMDBUse(component->getDeviceType()))
               fprintf(logFp, "Subclass %s of component %s not found in Data Doctor element library\n", component->getAptModelName(), component->getName());
         }

         // If Paralleled test generate but has been commented out, then write out a vision test
         if (component->getCommentOperator() == "//" && component->getMergedStatus().CompareNoCase("Ignored") == 0)
            component->SetWriteSelectVisionTest(true);

         // Determine if a vision test is needed
         if (settings.getAllComponentVisionTest() || (settings.getSelectVisionTest() && component->GetWriteSelectVisionTest()))
         {
            generateVisionOutput(logFp, *component, outputArray, settings, testSurface);
         }
      }
   }
}

void HiokiComponentCollection::generateCapacitorPolarizedTest(CCamCadData& camcadData, FILE *logFp, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiNet* groundNet, HiokiSettings& settings, const HiokiSurface testSurface)
{
   for (POSITION pos=m_capacitorPolarizedComponents.GetHeadPosition(); pos!=NULL;)
   {
      HiokiComponent* component = m_capacitorPolarizedComponents.GetNext(pos);
      if (component != NULL)
      { 
         if (component->isIcOpenTest() && groundNet != NULL)
            generateIcOpenTest(logFp, camcadData, *groundNet, *component, probeOutputCollection, outputArray, settings, testSurface);
         else
            generateGenericTest(logFp, *component, adptModelMap, probeOutputCollection, outputArray, settings, testSurface);
      }
   }
}

void HiokiComponentCollection::generateCapacitorTantalumTest(CCamCadData& camcadData, FILE *logFp, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiNet* groundNet, HiokiSettings& settings, const HiokiSurface testSurface)
{
   for (POSITION pos=m_capacitorTantalumComponents.GetHeadPosition(); pos!=NULL;)
   {
      HiokiComponent* component = m_capacitorTantalumComponents.GetNext(pos);
      if (component != NULL)
      { 
         if (component->isIcOpenTest() && groundNet != NULL)
            generateIcOpenTest(logFp, camcadData, *groundNet, *component, probeOutputCollection, outputArray, settings, testSurface);
         else
            generateGenericTest(logFp, *component, adptModelMap, probeOutputCollection, outputArray, settings, testSurface);
      }
   }
}

void HiokiComponentCollection::generateIcOpenTest(FILE *logFp, CCamCadData& camcadData, 
                                                      HiokiNet& groundNet, HiokiComponent& component, 
                                                      HiokiProbeOutputCollection& probeOutputCollection, 
                                                      CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface)
{
   // Get Pin Location that is sensorDiameter from DFT (or RealPart package) outline

   if (component.getSurface() != testSurface || component.getIcOpenTestDone())
      return;

   // Get the pcb component geometry.
	BlockStruct* geomBlock = camcadData.getBlockAt(component.getInsert().getBlockNumber());

   // Get the RealPart Package block. This may or may not exist, is okay if it does not.
   BlockStruct *realPartPkgBlock = GetRealPartPackageBlockForComponent(component.getFile(), camcadData, &component);

   double sensorDia = settings.getCapOpensSensorDiameter(camcadData.getPageUnits());

	ICOpenTestGeom* icOpenTestGeom = m_icOpenTestGeomMap.GetICOpenTestGeom(camcadData, geomBlock, realPartPkgBlock, sensorDia);
	if (icOpenTestGeom != NULL)
   {
      if (icOpenTestGeom->HasShrinkError())
      {
         fprintf(logFp, "Outline processing shrink error for \"%s\". Sensor Diameter out of applicable range for this outline.\n",
            component.getName());
      }

	   CPinLocationMap* pinLocationMap = icOpenTestGeom->CreatePinLocationMapForInsert(&component.getInsert(), testSurface);
	   if (pinLocationMap != NULL)
      {
         int testPinCount = component.generateTestPinCount(logFp, settings.getMaxHitCount(), testSurface);

         if (testPinCount > 0)
         {
            HiokiTestAccess* groundTestAccess1 = NULL;
            HiokiTestAccess* groundTestAccess2 = NULL;
            groundNet.getTwoTestAccess(logFp, settings.getMaxHitCount(), testSurface, &groundTestAccess1, &groundTestAccess2);

            if (groundTestAccess1 == NULL || groundTestAccess2 == NULL)
            {
               // Error, Opens test requires two access pts on ground net.
               fprintf(logFp, "Error, Insufficient access points on ground net on %s surface, at least two are required for Opens test for component \"%s\".\n",
                  testSurface == testSurfaceTop ? "TOP" : "BOTTOM", component.getName());
            }
            else
            {
               CString groundProbe1 = probeOutputCollection.getProbeNumberString(*groundTestAccess1);
               CString groundProbe2 = probeOutputCollection.getProbeNumberString(*groundTestAccess2);

               CString annotation;
               CString output;

               HiokiCompPin* compPin = component.getFirstSorted();
               while (compPin != NULL)
               {
                  HiokiTestAccess* testAccess = compPin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
                  if (testAccess != NULL)
                  {
		               CPoint2d pinLoc;
		               CPoint2d* tmpPinLoc = NULL;
		               pinLocationMap->Lookup(compPin->getPinName(), tmpPinLoc);
		               if (tmpPinLoc != NULL)
			               pinLoc = *tmpPinLoc;
		               else if (!component.getInsert().getCentroidLocation(camcadData, pinLoc))
			               pinLoc = component.getInsert().getOrigin2d();

                     convertToHiokiUnits(pinLoc, settings.getUnitFactor(), component.getSurface() == surfaceTop);

                     CString probeNumber = probeOutputCollection.getProbeNumberString(*testAccess);

						   if (probeNumber.CompareNoCase(groundProbe1) != 0 && probeNumber.CompareNoCase(groundProbe2) != 0 )
						   {
                        if (settings.getAnnotated())
                        {
                           CString value = compPin->getPinName() + "Pin";

							      // Only output test if the probe number is not the same as the one in @A G1:N G2:N
                           output.Format("%s %s %s %s %s %s %.0f %.0f @PL H 0 0 1 0 @T 100 30", component.getCommentOperator(), component.getHiokiName(), 
                                 value, component.getIcOpenDescription(), component.getLocation(), probeNumber, pinLoc.x, pinLoc.y);
                           outputArray.Add(output);
                        }
                        //*rcf BUG No hioki output here yet
						   }
                  }

                  compPin = component.getNextSorted();
               }

               if (settings.getAnnotated())
               {
                  //output.Format("@CE");
                  //outputArray.Add(output);
               }

               component.setIcOpenTestDone(true);
            }
         }
      }
   }
}

void HiokiComponentCollection::generateUnsupportedDeviceTypeTest(FILE *logFp, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface)
{
   for (POSITION pos=m_otherComponents.GetHeadPosition(); pos!=NULL;)
   {
      HiokiComponent* component = m_otherComponents.GetNext(pos);
      if (component != NULL)
      {
         HiokiAptModel* aptModel = adptModelMap.lookup(component->getAptModelName());
         if (aptModel != NULL)
         {
            int testPinCount = component->generateTestPinCount(logFp, settings.getMaxHitCount(), testSurface);
            if (testPinCount > 0)
            {
               // Generate test using aptModel
               aptModel->generateAptModelTest(logFp, *component, probeOutputCollection, outputArray, settings, testSurface);
            }
         }
         if(aptModel == NULL && !component->getAptModelName().IsEmpty())
               fprintf(logFp, "Subclass %s of component %s not found in Takaya model library\n", component->getAptModelName(), component->getName());            

          // If Paralleled test generate but has been commented out, then write out a vision test
         if (component->getCommentOperator() == "//" && component->getMergedStatus().CompareNoCase("Ignored") == 0)
            component->SetWriteSelectVisionTest(true);

         // Determine if a vision test is needed
         if (settings.getAllComponentVisionTest() || (settings.getSelectVisionTest() && component->GetWriteSelectVisionTest()))
         {
            generateVisionOutput(logFp, *component, outputArray, settings, testSurface);
         }
      }
   }
}

void HiokiComponentCollection::generateGenericTest(FILE *logFp, HiokiComponent& component, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection,
      CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface)
{
   int testPinCount = component.generateTestPinCount(logFp, settings.getMaxHitCount(), testSurface);

   if (testPinCount > 0)
   {
      HiokiAptModel* aptModel = adptModelMap.lookup(component.getAptModelName());
      if (aptModel != NULL)
      {
         // Generate test using aptModel
         aptModel->generateAptModelTest(logFp, component, probeOutputCollection, outputArray, settings, testSurface);
      }
      else
      {
         CSubclass *sc = m_subclassList.findSubclass(component.getAptModelName());
         if(sc != NULL && isValidDeviceTypeForMDBUse(component.getDeviceType()))            
         {
            generateOutputUsingMDB(logFp, component, probeOutputCollection, outputArray, settings, testSurface, sc);            
         }
         else if (testPinCount == 2)
         {
            HiokiCompPin* compPin1 = component.getFirstSorted();
            HiokiCompPin* compPin2 = component.getNextSorted();

            if (generateOutput(logFp, component, compPin1, compPin2, probeOutputCollection, outputArray, settings, testSurface))
               component.SetWriteSelectVisionTest(false);
         }

         if(aptModel == NULL && !component.getAptModelName().IsEmpty())
            fprintf(logFp, "Subclass %s of component %s not found in Takaya model library\n", component.getAptModelName(), component.getName());
         if(sc == NULL && isValidDeviceTypeForMDBUse(component.getDeviceType()))
            fprintf(logFp, "Subclass %s of component %s not found in Data Doctor element library\n", component.getAptModelName(), component.getName());
      }
   }

   if ((component.getDeviceType() == deviceTypeCapacitorPolarized && settings.getPolarizedCapVisionTest()) ||
       (component.getDeviceType() == deviceTypeCapacitorTantalum  && settings.getTantalumCapVisionTest()))
       component.SetWriteSelectVisionTest(true);

   // If Paralleled test generate but has been commented out, then write out a vision test
   if (component.getCommentOperator() == "//" && component.getMergedStatus().CompareNoCase("Ignored") == 0)
      component.SetWriteSelectVisionTest(true);

   // Determine if a vision test is needed
   if (settings.getAllComponentVisionTest() || (settings.getSelectVisionTest() && component.GetWriteSelectVisionTest()))
   {
      generateVisionOutput(logFp, component, outputArray, settings, testSurface);
   }
}

bool HiokiComponentCollection::generateOutput(FILE *logFp, HiokiComponent& component, HiokiCompPin* compPin1, HiokiCompPin* compPin2, HiokiProbeOutputCollection& probeOutputCollection,
      CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface)
{
   if (compPin1 == NULL || compPin2 == NULL)
      return false;

   if (compPin1->getTestedSurface() != surfaceUnknown && compPin1->getTestedSurface() != testSurface &&
       compPin2->getTestedSurface() != surfaceUnknown && compPin2->getTestedSurface() != testSurface )
       return false;

   bool retval = false;
   HiokiTestAccess* testAccess1 = compPin1!=NULL?compPin1->getTestAccess(logFp, settings.getMaxHitCount(), testSurface):NULL;
   HiokiTestAccess* testAccess2 = compPin2!=NULL?compPin2->getTestAccess(logFp, settings.getMaxHitCount(), testSurface):NULL;

   if (testAccess1 != NULL && testAccess2 != NULL && testAccess1 != testAccess2)
   {
      CString probeNumber1 = probeOutputCollection.getProbeNumberString(*testAccess1);
      CString probeNumber2 = probeOutputCollection.getProbeNumberString(*testAccess2);

      CString annotation;
      CString output;

      if (settings.getAnnotated())
      {
         //CString option;
         //option.Format("@T %s %s", component.getPositiveTol(), component.getNegativeTol());
         //if(component.getDeviceType() == deviceTypeDiodeZener && option.Find("@MM") < 0)
         //   option.AppendFormat(" @MM DCZD");

         //output.Format("TAKAYA:%s %s %s %s %s %s %s %s %s", component.getCommentOperator(), component.getHiokiName(), component.getValue(),
         //   component.getComment(), component.getLocation(), component.getElement(), probeNumber1, probeNumber2, option);

         annotation.Format(" ! Test  RefDes[%s] Probe1[%s] Probe2[%s]",
            component.getHiokiName(), probeNumber1, probeNumber2);

         //outputArray.Add(output);
      }

      //*rcf new Hioki
      HiokiCosRecord cosRec(component.getHiokiName(), &component, GetHiokiTestCode(component.getDeviceType()), probeNumber1, probeNumber2);
      cosRec.GetFormattedRecord(globalStepNo++, output);
      output += annotation;
      outputArray.Add(output);

      // Set comppin to being tested
      compPin1->setTestedSurface(testSurface);
      compPin1->setAlreadyTested(true);
      compPin2->setTestedSurface(testSurface);
      compPin2->setAlreadyTested(true);
      retval = true;
   }

   return retval;
}

bool HiokiComponentCollection::isValidDeviceTypeForMDBUse(const DeviceTypeTag deviceType)
{
   return (deviceType == deviceTypeCapacitorArray
      || deviceType == deviceTypeDiodeArray
      || deviceType == deviceTypeFilter
      || deviceType == deviceTypeICLinear
      || deviceType == deviceTypeResistorArray            
      || deviceType == deviceTypeTransistorArray);
}

bool HiokiComponentCollection::isValidTwoPinDeviceType(const DeviceTypeTag deviceType)
{
   return (deviceType == deviceTypeCapacitor
         || deviceType == deviceTypeCapacitorPolarized
         || deviceType == deviceTypeCapacitorTantalum
         || deviceType == deviceTypeDiode
         || deviceType == deviceTypeDiodeLed
         || deviceType == deviceTypeDiodeZener
         || deviceType == deviceTypeFuse
         || deviceType == deviceTypeInductor
         || deviceType == deviceTypeJumper
         || deviceType == deviceTypeResistor);
}

bool HiokiComponentCollection::isValidFourPinDeviceType(const DeviceTypeTag deviceType)
{
   return (deviceType == deviceTypeOpto);
}

bool HiokiComponentCollection::isValidThreePinDeviceType(const DeviceTypeTag deviceType)
{
   return (deviceType == deviceTypeTransistor
      || deviceType == deviceTypeTransistorFetNpn
      || deviceType == deviceTypeTransistorFetPnp
      || deviceType == deviceTypeTransistorMosfetNpn
      || deviceType == deviceTypeTransistorMosfetPnp
      || deviceType == deviceTypeTransistorNpn
      || deviceType == deviceTypeTransistorPnp);
}

void HiokiComponentCollection::generateOutputUsingMDB(FILE *logFp, HiokiComponent& component,
     HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
     HiokiSettings& settings, const HiokiSurface testSurface, CSubclass *sc)
{
   
   for(int i = 0; i < sc->getNumElements(); i++)
   {
      CSubclassElement *el = sc->getElementAt(i);
      if (el != NULL)
      {
         if (isValidTwoPinDeviceType(el->getDeviceType()))
            generateTwoPinOutputUsingMDB(logFp, component, probeOutputCollection, outputArray, 
            settings,testSurface,el);
         else if (isValidThreePinDeviceType(el->getDeviceType()))
            generateThreePinOutputUsingMDB(logFp, component, probeOutputCollection, outputArray, 
            settings,testSurface,el);
         else if (isValidFourPinDeviceType(el->getDeviceType()))
            generateFourPinOutputUsingMDB(logFp, component, probeOutputCollection, outputArray, 
            settings,testSurface,el);
      }
   }      
}

void HiokiComponentCollection::generateFourPinOutputUsingMDB(FILE *logFp, HiokiComponent& component,
     HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
     HiokiSettings& settings, const HiokiSurface testSurface, CSubclassElement *el)
{ 
   HiokiCompPin* anodePin = NULL;
   HiokiCompPin* cathodePin = NULL;
   HiokiCompPin* collectorPin = NULL;
   HiokiCompPin* emitterPin = NULL;

   anodePin = component.findCompPin(el->getPin1Name());
   cathodePin = component.findCompPin(el->getPin2Name());
   emitterPin = component.findCompPin(el->getPin3Name());
   collectorPin = component.findCompPin(el->getPin4Name());
   if (anodePin!=NULL && cathodePin!=NULL && collectorPin!=NULL && emitterPin!=NULL)
   {
      if(anodePin->getTestedSurface() != surfaceUnknown && anodePin->getTestedSurface() != testSurface &&
         cathodePin->getTestedSurface() != surfaceUnknown && cathodePin->getTestedSurface() != testSurface &&
         collectorPin->getTestedSurface() != surfaceUnknown && collectorPin->getTestedSurface() != testSurface &&
         emitterPin->getTestedSurface() != surfaceUnknown && emitterPin->getTestedSurface() != testSurface)
         return;

      HiokiTestAccess* anodePinTestAccess = anodePin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
      HiokiTestAccess* cathodePinTestAccess = cathodePin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
      HiokiTestAccess* collectorPinTestAccess = collectorPin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
      HiokiTestAccess* emitterPinTestAccess = emitterPin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);

      CString anodeProbeNum( probeOutputCollection.getProbeNumberString(anodePinTestAccess) );
      CString cathodeProbeNum( probeOutputCollection.getProbeNumberString(cathodePinTestAccess) );
      CString collectorProbeNum( probeOutputCollection.getProbeNumberString(collectorPinTestAccess) );
      CString emitterProbeNum( probeOutputCollection.getProbeNumberString(emitterPinTestAccess) );
         
      CString output;
      CString partNumber = component.getPartNumber().IsEmpty()?"*":component.getPartNumber();

      output.Format("%s@PC %s %s * %s %s %s %s", component.getCommentOperator(), component.getHiokiName(), partNumber,
         anodeProbeNum, cathodeProbeNum, collectorProbeNum, emitterProbeNum);
      outputArray.Add(output);

      output.Format("%s@CE", component.getCommentOperator());
      outputArray.Add(output);

      output.Format("%s%s C-E %s * R %s %s @K OP @K JP", component.getCommentOperator(), component.getHiokiName(), partNumber,
         collectorProbeNum, emitterProbeNum);
      outputArray.Add(output);

      output.Format("%s%s A-K %s * D %s %s @T 25 25 @K JP", component.getCommentOperator(), component.getHiokiName(), partNumber,
         anodeProbeNum, cathodeProbeNum);
      outputArray.Add(output);

      anodePin->setTestedSurface(testSurface);
      anodePin->setAlreadyTested(true);
      cathodePin->setTestedSurface(testSurface);
      cathodePin->setAlreadyTested(true);
      collectorPin->setTestedSurface(testSurface);
      collectorPin->setAlreadyTested(true);
      emitterPin->setTestedSurface(testSurface);
      emitterPin->setAlreadyTested(true);
   }
}

void HiokiComponentCollection::generateThreePinOutputUsingMDB(FILE *logFp, HiokiComponent& component,
     HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
     HiokiSettings& settings, const HiokiSurface testSurface, CSubclassElement *el)
{
   if(el->getDeviceType() == deviceTypeTransistor 
      || el->getDeviceType() == deviceTypeTransistor
      || el->getDeviceType() == deviceTypeTransistor)
   {
      //For deviceTypes deviceTypeTransistor, deviceTypeTransistorNpn and deviceTypeTransistorPnp
      HiokiCompPin* basePin = NULL;
      HiokiCompPin* emitterPin = NULL;
      HiokiCompPin* collectorPin = NULL;
      CString baseProbeNum, emitterProbeNum, collectorProbeNum;
      basePin = component.findCompPin(el->getPin1Name());
      emitterPin = component.findCompPin(el->getPin2Name());
      collectorPin = component.findCompPin(el->getPin3Name());
      if(basePin != NULL && emitterPin != NULL && collectorPin != NULL)
      {
         if(basePin->getTestedSurface() != surfaceUnknown && basePin->getTestedSurface() != testSurface &&
            emitterPin->getTestedSurface() != surfaceUnknown && emitterPin->getTestedSurface() != testSurface &&
            collectorPin->getTestedSurface() != surfaceUnknown && collectorPin->getTestedSurface() != testSurface)
            return;
         
         HiokiTestAccess* basePinTestAccess = basePin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
         HiokiTestAccess* emitterPinTestAccess = emitterPin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
         HiokiTestAccess* collectorPinTestAccess = collectorPin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);

         if(basePinTestAccess != NULL && emitterPinTestAccess != NULL && collectorPinTestAccess != NULL)
         {
            baseProbeNum = probeOutputCollection.getProbeNumberString(*basePinTestAccess);
            emitterProbeNum = probeOutputCollection.getProbeNumberString(*emitterPinTestAccess);
            collectorProbeNum = probeOutputCollection.getProbeNumberString(*collectorPinTestAccess);

            CString output, comment;
            if (component.getDeviceType() == deviceTypeTransistorPnp)
            {
               comment = component.getComment()=="*"?"PNP":component.getComment();
               output.Format("%s@Q2 %s %s * @T 10 10", component.getCommentOperator(), component.getHiokiName(), comment);
            }
            else if (component.getDeviceType() == deviceTypeTransistor || component.getDeviceType() == deviceTypeTransistorNpn)
            {
               comment = component.getComment()=="*"?"NPN":component.getComment();
               output.Format("%s@Q2 %s %s * @T 10 10", component.getCommentOperator(), component.getHiokiName(), comment);
            }
            outputArray.Add(output);               

            output.Format("%s %s", component.getCommentOperator(), baseProbeNum);
            outputArray.Add(output);

            output.Format("%s %s", component.getCommentOperator(), collectorProbeNum);
            outputArray.Add(output);
            
            output.Format("%s %s", component.getCommentOperator(), emitterProbeNum);
            outputArray.Add(output);
            
            output.Format("%s@CE", component.getCommentOperator());
            outputArray.Add(output);

            basePin->setTestedSurface(testSurface);
            basePin->setAlreadyTested(true);
            collectorPin->setTestedSurface(testSurface);
            collectorPin->setAlreadyTested(true);
            emitterPin->setTestedSurface(testSurface);            
            emitterPin->setAlreadyTested(true);
         }
      }
   }
   else
   {
      //For deviceTypes deviceTypeTransistorFetNpn, deviceTypeTransistorFetPnp
      // deviceTypeTransistorMosfetNpn and deviceTypeTransistorMosfetPnp
      HiokiCompPin* drainPin = NULL;
      HiokiCompPin* gatePin = NULL;
      HiokiCompPin* sourcePin = NULL;
      CString drainProbeNum, gateProbeNum, sourceProbeNum;
      drainPin = component.findCompPin(el->getPin1Name());
      gatePin = component.findCompPin(el->getPin2Name());
      sourcePin = component.findCompPin(el->getPin3Name());
      if(drainPin != NULL && gatePin != NULL && sourcePin != NULL)
      {
         if(drainPin->getTestedSurface() != surfaceUnknown && drainPin->getTestedSurface() != testSurface &&
            gatePin->getTestedSurface() != surfaceUnknown && gatePin->getTestedSurface() != testSurface &&
            sourcePin->getTestedSurface() != surfaceUnknown && sourcePin->getTestedSurface() != testSurface)
            return;
         
         HiokiTestAccess* drainPinTestAccess = drainPin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
         HiokiTestAccess* gatePinTestAccess = gatePin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
         HiokiTestAccess* sourcePinTestAccess = sourcePin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);

         if(drainPinTestAccess != NULL && gatePinTestAccess != NULL && sourcePinTestAccess != NULL)
         {
            drainProbeNum = probeOutputCollection.getProbeNumberString(*drainPinTestAccess);
            gateProbeNum = probeOutputCollection.getProbeNumberString(*gatePinTestAccess);
            sourceProbeNum = probeOutputCollection.getProbeNumberString(*sourcePinTestAccess);
            CString output, comment;

            if (component.getDeviceType() == deviceTypeTransistorFetNpn || component.getDeviceType() == deviceTypeTransistorMosfetNpn)
            {
               comment = component.getComment()=="*"?"FET_N":component.getComment();
               output.Format("%s@F %s %s * @MM FETEN", component.getCommentOperator(), component.getHiokiName(), comment);
            }
            else if (component.getDeviceType() == deviceTypeTransistorFetPnp || component.getDeviceType() == deviceTypeTransistorMosfetPnp)
            {
               comment = component.getComment()=="*"?"FET_P":component.getComment();
               output.Format("%s@F %s %s * @MM FETDP", component.getCommentOperator(), component.getHiokiName(), comment);
            }

            outputArray.Add(output);

            output.Format("%s %s", component.getCommentOperator(), gateProbeNum);
            outputArray.Add(output);

            output.Format("%s %s", component.getCommentOperator(), drainProbeNum);
            outputArray.Add(output);

            output.Format("%s %s", component.getCommentOperator(), sourceProbeNum);
            outputArray.Add(output);

            output.Format("%s@CE", component.getCommentOperator());
            outputArray.Add(output);

            gatePin->setTestedSurface(testSurface);
            gatePin->setAlreadyTested(true);
            sourcePin->setTestedSurface(testSurface);
            sourcePin->setAlreadyTested(true);
            drainPin->setTestedSurface(testSurface);
            drainPin->setAlreadyTested(true);
         }
      }
   }
}

void HiokiComponentCollection::generateTwoPinOutputUsingMDB(FILE *logFp, HiokiComponent& component,
     HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
     HiokiSettings& settings, const HiokiSurface testSurface, CSubclassElement *el)
{
   CString value = component.getValue();
   CString ntol = component.getNegativeTol();
   CString ptol = component.getPositiveTol();
   CString refName = component.getName();
   HiokiCompPin* compPin1 = component.findCompPin(el->getPin1Name());
   HiokiCompPin* compPin2 = component.findCompPin(el->getPin2Name());
   if (compPin1 != NULL && compPin2 != NULL)
   {
      if (compPin1->getTestedSurface() != surfaceUnknown && compPin1->getTestedSurface() != testSurface &&
         compPin2->getTestedSurface() != surfaceUnknown && compPin2->getTestedSurface() != testSurface )
         return;

      HiokiTestAccess* testAccess1 = compPin1->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
      HiokiTestAccess* testAccess2 = compPin2->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);

      if (testAccess1 != NULL && testAccess2 != NULL && testAccess1 != testAccess2)
      {
         char testType = HIOKI_TEST_NONE;
         CString probeNumber1 = probeOutputCollection.getProbeNumberString(*testAccess1);
         CString probeNumber2 = probeOutputCollection.getProbeNumberString(*testAccess2);

         CString output;
         CString element;

         if (!el->getValue().IsEmpty())
            value = el->getValue();
         if (!el->getPlusTol().IsEmpty())
            ptol = el->getPlusTol();
         if (!el->getMinusTol().IsEmpty())
            ntol = el->getMinusTol();

         switch (el->getDeviceType())
         {
         case deviceTypeCapacitor:
         case deviceTypeCapacitorPolarized:
         case deviceTypeCapacitorTantalum:
            element = "C";
            testType = HIOKI_TEST_CAPACITOR;
            break;
         case deviceTypeDiode:
         case deviceTypeDiodeLed:
            element = "D";
            testType = HIOKI_TEST_DIODE;
            break;
         case deviceTypeDiodeZener:
            element = "D";
            testType = HIOKI_TEST_ZENER;
            break;
         case deviceTypeFuse:
         case deviceTypeResistor:
         case deviceTypeJumper:
            element = "R";
            testType = HIOKI_TEST_RESISTOR;
            break;
         case deviceTypeInductor:
            element = "L";
            testType = HIOKI_TEST_INDUCTOR;
            break;
         }


         CString annotation;
         if (settings.getAnnotated())
         {
            //output.Format("TAKAYA:%s %s %s %s-%s/%s-%s * %s %s %s @T %s %s", component.getCommentOperator(), refName, value, refName, el->getPin1Name(), refName, el->getPin2Name(), element, probeNumber1, probeNumber2, ptol, ntol);
            
            annotation.Format(" ! TwoPin SubclassElement RefDes[%s] TestType[%c] Probe1[%s] Probe2[%s]",
               component.getHiokiName(), testType, probeNumber1, probeNumber2);

            //outputArray.Add(output);
         }

         //*rcf new Hioki
         HiokiCosRecord cosRec(component.getHiokiName(), &component, testType, probeNumber1, probeNumber2);
         cosRec.GetFormattedRecord(globalStepNo++, output);
         output += annotation;
         outputArray.Add(output);

         // Set comppin to being tested
         compPin1->setTestedSurface(testSurface);
         compPin1->setAlreadyTested(true);
         compPin2->setTestedSurface(testSurface);
         compPin2->setAlreadyTested(true);
      }
   }
}

bool HiokiComponentCollection::generateVisionOutput(FILE *logFp, HiokiComponent& component, 
      CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface)
{
   bool retval = false;

   if (m_visionTestCount < settings.getMaxVisionTestCount())
   {
      if(testSurface != component.getSurface())
         return retval;
		CString visionCommand = "";
		if (component.getDeviceType() == deviceTypeCapacitorPolarized)
			visionCommand = "@V1";
		else if (component.getDeviceType() == deviceTypeCapacitorTantalum)
			visionCommand = "@V2";
		else if (component.getDeviceType() == deviceTypeIC)
			visionCommand = "@V3";
		else
			visionCommand = "@V5";

		CString value = "*"; // Mark said to always output "*" as value for all vision test

      CPoint2d fptorigin = component.getInsertOrigin();
      if (testSurface == surfaceBottom)
         fptorigin.x = -fptorigin.x;

      CString annotation;
      CString output;

      if (settings.getAnnotated())
      {
         annotation.Format(" ! Vision Test  RefDes[%s]",
            component.getHiokiName());
      }

      //*rcf new Hioki
      HiokiCosRecord cosRec(component.getHiokiName(), &component, HIOKI_TEST_VISION, "", "");
      cosRec.GetFormattedRecord(globalStepNo++, output);
      output += annotation;
      outputArray.Add(output);

      m_visionTestCount++;

      retval = true;
   }
   else
   {
         // Need to write vision but didn't get writen so log it in the log file
         fprintf(logFp, "Comp [%s] not written in Hioki output file.\n", component.getName());
   }

   return retval;
}

void HiokiComponentCollection::generatePinToPinShortTest(CCamCadData& camcadData, FILE *logFp, CTypedPtrListContainer<HiokiComponent*>& components, HiokiProbeOutputCollection& probeOutputCollection,
   CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface)
{
   for (POSITION pos=components.GetHeadPosition(); pos!=NULL;)
   {
      HiokiComponent* component = components.GetNext(pos);
      if (component != NULL && !component->getPinShortTestDone() && settings.lookupPinToPinDevice(component->getDeviceType()))
      {
         BlockStruct* block = camcadData.getBlockAt(component->getInsert().getBlockNumber());
         if (block == NULL)
            continue;

         int testPinCount = component->generateTestPinCount(logFp, settings.getMaxHitCount(), testSurface);
         HiokiPinToPinAnalyzer* pinToPinAnalyzer = m_pinToPinAnalyzerMap.getPinToPinAnalyzer(*block, settings.getPinToPinDistance());
         if (pinToPinAnalyzer != NULL && testPinCount > 0)
         {  
            HiokiPinToPinShort* pinToPinShort = pinToPinAnalyzer->getFirstSorted();
            while (pinToPinShort != NULL)
            {
               HiokiCompPin* compPin1 = component->findCompPin(pinToPinShort->getPinName1());
               HiokiCompPin* compPin2 = component->findCompPin(pinToPinShort->getPinName2());

               if (compPin1 != NULL && compPin2 != NULL)
               {
                  HiokiTestAccess* testAccess1 = compPin1->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
                  HiokiTestAccess* testAccess2 = compPin2->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
         
                  if (testAccess1 != NULL && testAccess2 != NULL)
                  {
                     CString probeNumber1 = probeOutputCollection.getProbeNumberString(*testAccess1);
                     CString probeNumber2 = probeOutputCollection.getProbeNumberString(*testAccess2);

                     if (probeNumber1 == probeNumber2)
                     {
                        fprintf(logFp, "Shorts test not generated %s P%s-%s pins on the same net\n",
                              component->getName(), pinToPinShort->getPinName1(), pinToPinShort->getPinName2());
                     }
                     else
                     {
                        CString probePair;                       
                        if (m_pinToPinProbePairMap.Lookup(probeNumber1+probeNumber2, probePair) ||
                            m_pinToPinProbePairMap.Lookup(probeNumber2+probeNumber1, probePair))
                        {
                           fprintf(logFp,  "Duplicate shorts test not generated %s P%s-%s\n",
                                 component->getName(), pinToPinShort->getPinName1(), pinToPinShort->getPinName2());
                        }
                        else
                        {
                           m_pinToPinProbePairMap.SetAt(probeNumber1+probeNumber2, probeNumber1+probeNumber2);
                           m_pinToPinProbePairMap.SetAt(probeNumber2+probeNumber1, probeNumber2+probeNumber1);


                           CString annotation;
                           CString output;
                           if (settings.getAnnotated())
                           {
                              annotation.Format(" ! Pin-to-pin Short  RefDes[%s] Pin1[%s] Pin2[%s], Probe1[%s] Probe2[%s]",
                                 component->getHiokiName(), pinToPinShort->getPinName1(), pinToPinShort->getPinName2(), probeNumber1, probeNumber2);
                           }

                           CString modifiedName;
                           modifiedName.Format("%s P%s-%s", component->getHiokiName(), pinToPinShort->getPinName1(), pinToPinShort->getPinName2()); 
                           HiokiCosRecord cosRec(modifiedName, component, HIOKI_TEST_SHORT, probeNumber1, probeNumber2);
                           cosRec.GetFormattedRecord(globalStepNo++, output);
                           output += annotation;
                           outputArray.Add(output);
                        }
                     }
                  }

               }

               pinToPinShort = pinToPinAnalyzer->getNextSorted();
            }

            component->setPinShortTestDone(true);
         }
      }
   }
}

void HiokiComponentCollection::generateComponentHighFlyZoneTest(FileStruct& file, CCamCadData& camcadData, FILE *logFp, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface)
{
   if (settings.getHighFlyZone() > 0)
   {
      generateHighFlyZoneTest(file, camcadData, logFp, m_resistorComponents, probeOutputCollection,           outputArray, settings, testSurface);
      generateHighFlyZoneTest(file, camcadData, logFp, m_capacitorComponents, probeOutputCollection,          outputArray, settings, testSurface);
      generateHighFlyZoneTest(file, camcadData, logFp, m_diodeComponents, probeOutputCollection,              outputArray, settings, testSurface);
      generateHighFlyZoneTest(file, camcadData, logFp, m_inductorComponents, probeOutputCollection,           outputArray, settings, testSurface);
      generateHighFlyZoneTest(file, camcadData, logFp, m_icComponents, probeOutputCollection,                 outputArray, settings, testSurface);
      generateHighFlyZoneTest(file, camcadData, logFp, m_transitorComponents, probeOutputCollection,          outputArray, settings, testSurface);
      generateHighFlyZoneTest(file, camcadData, logFp, m_capacitorPolarizedComponents, probeOutputCollection, outputArray, settings, testSurface);
      generateHighFlyZoneTest(file, camcadData, logFp, m_capacitorTantalumComponents, probeOutputCollection,  outputArray, settings, testSurface);
      generateHighFlyZoneTest(file, camcadData, logFp, m_otherComponents, probeOutputCollection,              outputArray, settings, testSurface);
   }
}

void HiokiComponentCollection::generateHighFlyZoneTest(FileStruct& file, CCamCadData& camcadData, FILE *logFp, CTypedPtrListContainer<HiokiComponent*>& components, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface)
{
   //*rcf Keep code for now, but no apparant Hioki equivalent for this test.
   return;

   for (POSITION pos=components.GetHeadPosition(); pos!=NULL;)
   {
      HiokiComponent* component = components.GetNext(pos);
      if (component != NULL && component->getSurface() == testSurface && component->getCompHeight() > settings.getHighFlyZone())
      {
         //First find a real part with the same refDes, if not found then use pcb component.
         BlockStruct* block = NULL;
         CExtent* packageExtent = NULL;
         block = GetRealPartPackageBlockForComponent(file, camcadData, component);
         if(NULL != block)
            packageExtent = m_packageOutlineExtentMap.getPackageOutlineExtent(camcadData, *block, true);
         
         if(NULL == packageExtent)
         {
            block = NULL;
            block = camcadData.getBlockAt(component->getInsert().getBlockNumber());
            if (block != NULL)
               packageExtent = m_packageOutlineExtentMap.getPackageOutlineExtent(camcadData, *block, false);
            else continue;
         }
         
         if (packageExtent != NULL)
         {
            //packageExtent->transform(component->getInsert().getTMatrix());

            double xmax = packageExtent->getXmax();
            double xmin = packageExtent->getXmin();
            double ymax = packageExtent->getYmax();
            double ymin = packageExtent->getYmin();
            double compHeight = component->getCompHeight();

            ::convertToHiokiUnits(xmax, settings.getUnitFactor());
            ::convertToHiokiUnits(xmin, settings.getUnitFactor());
            ::convertToHiokiUnits(ymax, settings.getUnitFactor());
            ::convertToHiokiUnits(ymin, settings.getUnitFactor());

            ::convertToHiokiUnits(compHeight, settings.getUnitFactor());

            CPoint2d fptorigin = component->getInsertOrigin();
            CString output;
            output.Format("@HN3 %s %.0f %.0f %.0f %.0f %.0f %.0f %.0f", component->getHiokiName(),
                  fptorigin.x, fptorigin.y, xmax, xmin, ymax, ymin, compHeight);
            outputArray.Add(output);
            //*rcf No apparant Hioki equivalent
         }
      }
   }

}

/******************************************************************************
* FindAccessDataForEntityNum
*/
DataStruct *HiokiComponentCollection::FindAccessDataForEntityNum(CCamCadData& camCadData,  BlockStruct *pcbFileBlock, long entityNumber)
{
   if (pcbFileBlock != NULL)
   {
      POSITION dataPos = pcbFileBlock->getHeadDataInsertPosition();
      while (dataPos)
      {
         DataStruct* accessData = pcbFileBlock->getNextDataInsert(dataPos);

         if (accessData != NULL && accessData->isInsertType(insertTypeTestAccessPoint))
         {
            CAttribute *attrib = accessData->getAttributesRef()->lookupAttribute(camCadData.getAttributeKeywordIndex(ATT_DDLINK));
            
            if (attrib != NULL && attrib->getIntValue() == entityNumber)
            {
               return accessData;
            }
         }
      }
   }

   return NULL;
}

void HiokiComponentCollection::getAccessMarkerXYOffset(CCamCadData& camCadData, BlockStruct *pcbFileBlock, int entityNumber, CPoint2d &pinOriginWithOffset)
{
   DataStruct *accessData = FindAccessDataForEntityNum(camCadData, pcbFileBlock, entityNumber);

   if(accessData && accessData->getInsert())
      pinOriginWithOffset = accessData->getInsert()->getOrigin2d();      
}

BlockStruct* HiokiComponentCollection::GetRealPartPackageBlockForComponent(FileStruct& file, CCamCadData& camCadData,HiokiComponent* component)
{
   BlockStruct* pkgBlock = NULL;
   POSITION pos = file.getBlock()->getDataList().GetHeadPosition();
   while (NULL != pos)
   {
      DataStruct* data = NULL;
      data = file.getBlock()->getDataList().GetNext(pos);
      if((data != NULL)
         && (data->getDataType() == dataTypeInsert)
         && (data->getInsert() != NULL)
         && (data->getInsert()->getInsertType() == insertTypeRealPart)
         && (!data->getInsert()->getRefname().CompareNoCase(component->getName())))
      {
         //Found the corresponding real part.
         BlockStruct *realpartBlock = camCadData.getBlockAt(data->getInsert()->getBlockNumber());
         if (realpartBlock != NULL)
         {
            for (CDataListIterator dataListIterator(*realpartBlock); dataListIterator.hasNext();)
            {
               DataStruct* realPartData = dataListIterator.getNext();

               if (realPartData->isInsertType(insertTypePackage))
               {
                  pkgBlock = camCadData.getBlockAt(realPartData->getInsert()->getBlockNumber());
                  if(NULL != pkgBlock)
                     return pkgBlock;
               }
            }
         }
      }
   }
   return pkgBlock;
}

void HiokiComponentCollection::writeLogFile(FILE *logFp)
{
   for (POSITION pos=m_resistorComponents.GetHeadPosition(); pos!=NULL;)
   {
      HiokiComponent* component = m_resistorComponents.GetNext(pos);
      if (component != NULL)
      {
         HiokiCompPin* compPin = component->getFirstSorted();
         while (compPin != NULL)
         {
            if(false == compPin->getAlreadyTested())
               fprintf(logFp, "Comp [%s] Pin [%s] not electrically tested\n",
               component->getName(),compPin->getPinName());
            compPin = component->getNextSorted();
         }
      }
   }
   for (POSITION pos=m_capacitorComponents.GetHeadPosition(); pos!=NULL;)
   {
      HiokiComponent* component = m_capacitorComponents.GetNext(pos);
      if (component != NULL)
      {
         HiokiCompPin* compPin = component->getFirstSorted();
         while (compPin != NULL)
         {
            if(false == compPin->getAlreadyTested())
               fprintf(logFp, "Comp [%s] Pin [%s] not electrically tested\n",
               component->getName(),compPin->getPinName());
            compPin = component->getNextSorted();
         }
      }
   }
   for (POSITION pos=m_diodeComponents.GetHeadPosition(); pos!=NULL;)
   {
      HiokiComponent* component = m_diodeComponents.GetNext(pos);
      if (component != NULL)
      {
         HiokiCompPin* compPin = component->getFirstSorted();
         while (compPin != NULL)
         {
            if(false == compPin->getAlreadyTested())
               fprintf(logFp, "Comp [%s] Pin [%s] not electrically tested\n",
               component->getName(),compPin->getPinName());
            compPin = component->getNextSorted();
         }
      }
   }
   for (POSITION pos=m_inductorComponents.GetHeadPosition(); pos!=NULL;)
   {
      HiokiComponent* component = m_inductorComponents.GetNext(pos);
      if (component != NULL)
      {
         HiokiCompPin* compPin = component->getFirstSorted();
         while (compPin != NULL)
         {
            if(false == compPin->getAlreadyTested())
               fprintf(logFp, "Comp [%s] Pin [%s] not electrically tested\n",
               component->getName(),compPin->getPinName());
            compPin = component->getNextSorted();
         }
      }
   }
   for (POSITION pos=m_icComponents.GetHeadPosition(); pos!=NULL;)
   {
      HiokiComponent* component = m_icComponents.GetNext(pos);
      if (component != NULL)
      {
         HiokiCompPin* compPin = component->getFirstSorted();
         while (compPin != NULL)
         {
            if(false == compPin->getAlreadyTested())
               fprintf(logFp, "Comp [%s] Pin [%s] not electrically tested\n",
               component->getName(),compPin->getPinName());
            compPin = component->getNextSorted();
         }
      }
   }
   for (POSITION pos=m_transitorComponents.GetHeadPosition(); pos!=NULL;)
   {
      HiokiComponent* component = m_transitorComponents.GetNext(pos);
      if (component != NULL)
      {
         HiokiCompPin* compPin = component->getFirstSorted();
         while (compPin != NULL)
         {
            if(false == compPin->getAlreadyTested())
               fprintf(logFp, "Comp [%s] Pin [%s] not electrically tested\n",
               component->getName(),compPin->getPinName());
            compPin = component->getNextSorted();
         }
      }
   }
   for (POSITION pos=m_capacitorPolarizedComponents.GetHeadPosition(); pos!=NULL;)
   {
      HiokiComponent* component = m_capacitorPolarizedComponents.GetNext(pos);
      if (component != NULL)
      {
         HiokiCompPin* compPin = component->getFirstSorted();
         while (compPin != NULL)
         {
            if(false == compPin->getAlreadyTested())
               fprintf(logFp, "Comp [%s] Pin [%s] not electrically tested\n",
               component->getName(),compPin->getPinName());
            compPin = component->getNextSorted();
         }
      }
   }
   for (POSITION pos=m_capacitorTantalumComponents.GetHeadPosition(); pos!=NULL;)
   {
      HiokiComponent* component = m_capacitorTantalumComponents.GetNext(pos);
      if (component != NULL)
      {
         HiokiCompPin* compPin = component->getFirstSorted();
         while (compPin != NULL)
         {
            if(false == compPin->getAlreadyTested())
               fprintf(logFp, "Comp [%s] Pin [%s] not electrically tested\n",
               component->getName(),compPin->getPinName());
            compPin = component->getNextSorted();
         }
      }
   }
   for (POSITION pos=m_otherComponents.GetHeadPosition(); pos!=NULL;)
   {
      HiokiComponent* component = m_otherComponents.GetNext(pos);
      if (component != NULL)
      {
         HiokiCompPin* compPin = component->getFirstSorted();
         while (compPin != NULL)
         {
            if(false == compPin->getAlreadyTested())
               fprintf(logFp, "Comp [%s] Pin [%s] not electrically tested\n",
               component->getName(),compPin->getPinName());
            compPin = component->getNextSorted();
         }
      }
   }
}

//_____________________________________________________________________________
HiokiIcDiodeCommandMap::HiokiIcDiodeCommandMap()
   : m_icDiodeCommandMap(nextPrime2n(20),true)
{
}

HiokiIcDiodeCommandMap::~HiokiIcDiodeCommandMap()
{
   empty();
}

void HiokiIcDiodeCommandMap::addCommand(const CString partNumber, const CString pinName, const bool swap)
{
   TakayaIcDiodeCommand* icDiodeCommand = NULL;
   CString upperCasePartNumber = partNumber;
   upperCasePartNumber.MakeUpper();
   if (m_icDiodeCommandMap.Lookup(upperCasePartNumber, icDiodeCommand))
   {
      delete icDiodeCommand;
      icDiodeCommand = NULL;
   }

   icDiodeCommand = new TakayaIcDiodeCommand(upperCasePartNumber, pinName, swap);
   m_icDiodeCommandMap.SetAt(upperCasePartNumber, icDiodeCommand);
}

TakayaIcDiodeCommand* HiokiIcDiodeCommandMap::getGroundPin(const CString partNumber)
{
   TakayaIcDiodeCommand* icDiodeCommand = NULL;
   CString upperCasePartNumber = partNumber;
   upperCasePartNumber.MakeUpper();   
   m_icDiodeCommandMap.Lookup(upperCasePartNumber, icDiodeCommand);

   return icDiodeCommand;
}

//_____________________________________________________________________________
HiokiAptModel::HiokiAptModel(const CString name)
: m_aptTestList(true)
, m_sName(name)
{
}

HiokiAptModel::~HiokiAptModel()
{
   m_aptTestList.empty();
}

HiokiAptTest* HiokiAptModel::createTest()
{
   HiokiAptTest* aptTest = new HiokiAptTest();
   m_aptTestList.AddTail(aptTest);
   
   return aptTest;
}

bool HiokiAptModel::generateAptModelTest(FILE *logFp, HiokiComponent& component, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface)
{
   bool retval = false;

   return retval;
}

//_____________________________________________________________________________
HiokiAptModelMap::HiokiAptModelMap()
: m_aptModelMap(nextPrime2n(20), true)
{
}

HiokiAptModelMap::~HiokiAptModelMap()
{
   empty();
}

void HiokiAptModelMap::empty()
{
   m_aptModelMap.empty();
}

HiokiAptModel* HiokiAptModelMap::add(const CString aptModelName)
{
   HiokiAptModel* aptModel = lookup(aptModelName);
	if (aptModel == NULL)
	{
		// Create and add aptModel to map
		aptModel = new HiokiAptModel(aptModelName);
		m_aptModelMap.SetAt(aptModelName, aptModel);
	}

   return aptModel;
}

HiokiAptModel* HiokiAptModelMap::lookup(const CString aptModelName)
{
   HiokiAptModel* aptModel = NULL;
   m_aptModelMap.Lookup(aptModelName, aptModel);
   return aptModel;
}

//_____________________________________________________________________________
HiokiPinToPinAnalyzer::HiokiPinToPinAnalyzer()
: m_pinToPinMap(nextPrime2n(20), true)
{
   m_pinToPinMap.empty();
}

HiokiPinToPinAnalyzer::~HiokiPinToPinAnalyzer()
{
   m_pinToPinMap.empty();
}

void HiokiPinToPinAnalyzer::analysisPinToPinShort(BlockStruct& block, const double distance)
{
   if (distance < 0)
      return;

   for (POSITION pos1=block.getHeadDataInsertPosition(); pos1!= NULL;)
   {
      InsertStruct* pinInsert1 = block.getNextDataInsert(pos1)->getInsert();
      if (pinInsert1 != NULL && pinInsert1->getInsertType() == insertTypePin)
      {
         InsertStruct* closestPinInsert = NULL; 
         double shortestDistance = DBL_MAX;

         for(POSITION pos2=pos1; pos2!=NULL;)
         {
            InsertStruct* pinInsert2 = block.getNextDataInsert(pos2)->getInsert();
            if (pinInsert2 != NULL && pinInsert2->getInsertType() == insertTypePin)
            {
				   double pinDistance = pinInsert1->getOrigin2d().distance(pinInsert2->getOrigin2d());
				   if (pinDistance < shortestDistance)
				   {
					   shortestDistance = pinDistance;
					   closestPinInsert = pinInsert2;
				   }
            }
         }

         if (shortestDistance < distance)
         {
            CString pinName1 = pinInsert1->getSortableRefDes();
            CString pinName2 = closestPinInsert->getSortableRefDes();

            if (pinName1.CompareNoCase(pinName2) < 0)
            {
               HiokiPinToPinShort* pinToPinShort = new HiokiPinToPinShort(pinInsert1->getRefname(), closestPinInsert->getRefname());
               m_pinToPinMap.SetAt(pinName1, pinToPinShort);
            }
            else
            {
               HiokiPinToPinShort* pinToPinShort = new HiokiPinToPinShort(closestPinInsert->getRefname(), pinInsert1->getRefname());
               m_pinToPinMap.SetAt(pinName2, pinToPinShort);
            }
         }        
      }
   }

   m_pinToPinMap.Sort();
}

HiokiPinToPinShort* HiokiPinToPinAnalyzer::getFirstSorted()
{
   HiokiPinToPinShort* pinToPinShort = NULL;
   CString* key = NULL;
   m_pinToPinMap.GetFirstSorted(key, pinToPinShort);

   return pinToPinShort;
}

HiokiPinToPinShort* HiokiPinToPinAnalyzer::getNextSorted()
{
   HiokiPinToPinShort* pinToPinShort = NULL;
   CString* key = NULL;
   m_pinToPinMap.GetNextSorted(key, pinToPinShort);

   return pinToPinShort;
}

//_____________________________________________________________________________
HiokiPinToPinAnalyzerMap::HiokiPinToPinAnalyzerMap()
: m_pcbComponentPinToPinAnalyzerMap(nextPrime2n(20), true)
{
   m_pcbComponentPinToPinAnalyzerMap.empty();
}

HiokiPinToPinAnalyzerMap::~HiokiPinToPinAnalyzerMap()
{
   m_pcbComponentPinToPinAnalyzerMap.empty();
}

HiokiPinToPinAnalyzer* HiokiPinToPinAnalyzerMap::getPinToPinAnalyzer(BlockStruct& block, const double distance)
{
   HiokiPinToPinAnalyzer* pinToPinAnalyzer = NULL;
   if (!m_pcbComponentPinToPinAnalyzerMap.Lookup(block.getBlockNumber(), pinToPinAnalyzer))
   {
      pinToPinAnalyzer = new HiokiPinToPinAnalyzer();
      pinToPinAnalyzer->analysisPinToPinShort(block, distance);
      m_pcbComponentPinToPinAnalyzerMap.SetAt(block.getBlockNumber(), pinToPinAnalyzer);
   }

   return pinToPinAnalyzer;
}

//_____________________________________________________________________________
HiokiPackageOutlineExtentMap::HiokiPackageOutlineExtentMap()
: m_blockPackageExtentMap(nextPrime2n(20), true)
{
}

HiokiPackageOutlineExtentMap::~HiokiPackageOutlineExtentMap()
{
   m_blockPackageExtentMap.empty();
}

CExtent* HiokiPackageOutlineExtentMap::getPackageOutlineExtent(CCamCadData& camCadData, BlockStruct& block, bool realPart)
{
   CExtent* extent =  NULL;
   GraphicClassTag graphicsClass;
   if(realPart)
      graphicsClass = graphicClassPackageBody;
   else
      graphicsClass = graphicClassPackageOutline;
   if (!m_blockPackageExtentMap.Lookup(block.getBlockNumber(), extent))
   {
      for (CDataListIterator dataList(block, dataTypePoly); dataList.hasNext();)
      {
         DataStruct* polyData = dataList.getNext();
         if (polyData->getGraphicClass() == graphicsClass)
         {
            extent = new CExtent(polyData->getPolyList()->getExtent(camCadData));            
            break;
         }
      }

      m_blockPackageExtentMap.SetAt(block.getBlockNumber(), extent);
   }

   return extent;
}

//_____________________________________________________________________________
HiokiPcbFile::HiokiPcbFile(FileStruct& fileStruct)
: m_fileStruct(fileStruct)
{
   m_name.Empty();
   m_blockNumber = 0;
   m_xyOrigin.x = 0.0;
   m_xyOrigin.y = 0.0;
   m_rotationRadian = 0;   
   m_isMirrored = false;
}

HiokiPcbFile::~HiokiPcbFile()
{
}

//_____________________________________________________________________________
HiokiSettings::HiokiSettings()
{
   reset();
}

HiokiSettings::~HiokiSettings()
{

}

void HiokiSettings::reset()
{
   m_testSurface = surfaceTop;
   m_preferenceSurface = surfaceTop;
   m_shortType = shortTypeUnknown;
   m_topPopulated = true;
   m_bottomPopulated = true;
   m_polarizedCapVisionTest = false;
   m_tantalumCapVisionTest = false;
   m_allComponentVisionTest = false;
   m_selectVisionTest = false;
   m_maxVisionTestCount = 240;
   m_maxHitCount = 50;
   m_highFlyZone = 0;

   m_annotateOutput = false;

   m_testLibraryMode = false; // EXPORE_MORE
   m_testLibraryFileName.Empty();   // APT_MODEL_FILE_NAME
   m_takayaLibraryDirectory.Empty();

	m_pinToPinDeviceTypeMap.RemoveAll();
   m_pinToPinDistance = -1;

   m_exportIcCapacitor = false;
   m_exportIcDiode = false;
   m_isIcOpenDescriptionAttrib = false;
   m_icOpenDescription.Empty();
   m_icDiodePrefix.Empty();
   m_icCapacitorPrefix.Empty();
   m_commentAttribute.Empty();
   m_PPCommandAttribName.Empty();

   m_capOpensSensorDiameterMM = TK_DEFAULT_OUTLINE_SHRINK_DISTANCE;  // In millimeters !

   m_originModeTag = cczCadOrigin;
}

void HiokiSettings::addPinToPinDevice(const CString deviceType)
{
   CString deviceTypeKey = deviceType;
   deviceTypeKey.MakeUpper();

   m_pinToPinDeviceTypeMap.SetAt(deviceTypeKey, deviceType);
}

bool HiokiSettings::lookupPinToPinDevice(const DeviceTypeTag deviceType)
{
   bool deviceFound = false;
   
   CString value;   
   CString deviceTypeKey =  deviceTypeTagToFriendlyString(deviceType);
   deviceTypeKey.MakeUpper();
   deviceFound = m_pinToPinDeviceTypeMap.Lookup(deviceTypeKey, value)?true:false;

   if (!deviceFound)
   {
      deviceTypeKey = deviceTypeTagToValueString(deviceType);
      deviceTypeKey.MakeUpper();
      deviceFound = m_pinToPinDeviceTypeMap.Lookup(deviceTypeKey, value)?true:false;
   }

   return deviceFound;
}

CString HiokiSettings::getTestLibraryFullPathName() const
{
   CString retval;

   if (m_takayaLibraryDirectory.IsEmpty())
   {
      retval = getApp().getExportSettingsFilePath(m_testLibraryFileName);
   }
   else
   {
      if (m_takayaLibraryDirectory.Right(1) != "\\")
         retval = m_takayaLibraryDirectory + "\\" + m_testLibraryFileName;
      else
         retval = m_takayaLibraryDirectory + m_testLibraryFileName;
   }

   return retval;
}


//_____________________________________________________________________________
HiokiWriter::HiokiWriter(CCEtoODBDoc& camCadDoc, const CString pathName)
: m_camCadDoc(camCadDoc)
, m_camCadData(camCadDoc.getCamCadData())
, m_pcbDesignList(true)
, m_compPinMap(true)
, m_pathName(pathName)
, m_hasGroundNet(false)
, m_probablyShortNetPairMap(nextPrime2n(20))
, m_topOutputOrigin(0.,0.)
, m_botOutputOrigin(0.,0.)
, m_componentCollection(&camCadDoc.getCamCadData().getTableList())
{
   m_progressDlg = NULL;
   m_logFp = NULL;
   m_topOutputFile = NULL;
   m_bottomOutputFile = NULL;
   empty();

   m_topOutputArray.SetSize(0, 100);
   m_bottomOutputArray.SetSize(0, 100);

   m_unitFactor = m_camCadDoc.convertPageUnitsTo(pageUnitsMilliMeters, 1.);
}

HiokiWriter::~HiokiWriter()
{
   closeLogFile();
   empty();
}

void HiokiWriter::empty()
{
   m_pcbDesignList.empty();
   m_aptModelMap.empty();
   m_icDiodeCommandMap.empty();
   m_netMap.empty();
   m_compPinMap.empty();
   m_componentCollection.empty();
   m_topOutputArray.RemoveAll();
   m_bottomOutputArray.RemoveAll();
   m_probablyShortNetPairMap.RemoveAll();

   m_groundNet = NULL;

   m_topBrdRefPntCommand.Empty();
   m_topAuxRefPntCommand.Empty();
   m_botBrdRefPntCommand.Empty();
   m_botAuxRefPntCommand.Empty();

   if (m_progressDlg != NULL && m_progressDlg->DestroyWindow())
   {
      delete m_progressDlg;
      m_progressDlg = NULL;
   }
}

CProgressDlg& HiokiWriter::getProgressDlg()
{
   if (m_progressDlg == NULL)
   {
      m_progressDlg = new CProgressDlg("Hioki Export", false);
      m_progressDlg->Create(AfxGetMainWnd());
   }

   return *m_progressDlg;
}

FILE* HiokiWriter::getLogFile()
{
   if (m_logFp == NULL)
   {
      m_logFp = getApp().OpenOperationLogFile("hioki.log", m_localLogFilename);
      WriteStandardExportLogHeader(m_logFp, "Hioki");
   }

   return m_logFp;
}

void HiokiWriter::closeLogFile()
{
   if (!m_localLogFilename.IsEmpty() && m_logFp != NULL)
   {
      fclose(m_logFp);
      m_logFp = NULL;
   }
}


void HiokiWriter::write()
{
   if (getPcbDesignList())
   {
      loadSettings();

      if (!m_settings.getTopPopulated())
         fprintf(getLogFile(), "HIOKI Test report does not include BOTTOM side placed components !\n\n");
      if (!m_settings.getBottomPopulated())
         fprintf(getLogFile(), "HIOKI Test report does not include TOP side placed components !\n\n");

      this->m_componentCollection.SetTakayaOrigin(m_topOutputOrigin, m_botOutputOrigin);

      getProgressDlg().ShowWindow(SW_SHOW);
      getProgressDlg().SetStatus("Preparing Data for Hioki output...");
      for (POSITION pos=m_pcbDesignList.GetHeadPosition(); pos!=NULL;)
      {
         HiokiPcbFile* pcbFile = m_pcbDesignList.GetNext(pos);
         if (pcbFile != NULL)
         {
            generate_PINLOC(&m_camCadDoc, &pcbFile->getFileStruct(), FALSE);

            if (loadHiokiData(pcbFile->getFileStruct()))
            {
               if (!m_hasGroundNet)
               {
                  m_settings.setExportIcCapacitor(false);
                  m_settings.setExportIcDiode(false);
            
                  m_componentCollection.moveIcDeviceTypeToOtherComponent();
               }
               else
               {
                  // Always get the setting specified by users if there is GROUND net
#ifdef USE_DIALOG
                  m_settings.setExportIcCapacitor(dlg.GetICCapacitance()==TRUE);
                  m_settings.setExportIcDiode(dlg.GetICDiode()==TRUE);
#else
                  m_settings.setExportIcCapacitor(false);
                  m_settings.setExportIcDiode(false);
#endif
               }

               //----------------------------
               // Generate test for components
               if (m_settings.getTestSurface() == surfaceBoth)
               {
                  if (m_settings.getPreferenceSurface() == surfaceTop)
                  {
                     //First generate for top and then for bottom
                     m_componentCollection.generateComponentTest(m_camCadData, getLogFile(), m_aptModelMap, m_topProbeOutputCollection, m_topOutputArray, m_groundNet, m_icDiodeCommandMap, m_settings, surfaceTop);
                     generateProbablyShortTest(pcbFile->getFileStruct(), getLogFile(), m_topProbeOutputCollection, m_topOutputArray, surfaceTop);
                     m_netMap.generatePowerRailShortTest(getLogFile(), m_topProbeOutputCollection, m_topOutputArray, m_settings, surfaceTop);
                     m_componentCollection.generateComponentHighFlyZoneTest(pcbFile->getFileStruct(), m_camCadData, getLogFile(), m_topProbeOutputCollection, m_topOutputArray, m_settings, surfaceTop);
                     
                     m_componentCollection.generateComponentTest(m_camCadData, getLogFile(), m_aptModelMap, m_bottomProbeOutputCollection, m_bottomOutputArray, m_groundNet, m_icDiodeCommandMap, m_settings, surfaceBottom);
                     generateProbablyShortTest(pcbFile->getFileStruct(), getLogFile(), m_bottomProbeOutputCollection, m_bottomOutputArray, surfaceBottom);
                     m_netMap.generatePowerRailShortTest(getLogFile(), m_bottomProbeOutputCollection, m_bottomOutputArray, m_settings, surfaceBottom);
                     m_componentCollection.generateComponentHighFlyZoneTest(pcbFile->getFileStruct(), m_camCadData, getLogFile(), m_bottomProbeOutputCollection, m_bottomOutputArray, m_settings, surfaceBottom);
                  }
                  else
                  {
                     //First generate for bottom and then for top
                     m_componentCollection.generateComponentTest(m_camCadData, getLogFile(), m_aptModelMap, m_bottomProbeOutputCollection, m_bottomOutputArray, m_groundNet, m_icDiodeCommandMap, m_settings, surfaceBottom);
                     generateProbablyShortTest(pcbFile->getFileStruct(), getLogFile(), m_bottomProbeOutputCollection, m_bottomOutputArray, surfaceBottom);
                     m_netMap.generatePowerRailShortTest(getLogFile(), m_bottomProbeOutputCollection, m_bottomOutputArray, m_settings, surfaceBottom);
                     m_componentCollection.generateComponentHighFlyZoneTest(pcbFile->getFileStruct(), m_camCadData, getLogFile(), m_bottomProbeOutputCollection, m_bottomOutputArray, m_settings, surfaceBottom);

                     m_componentCollection.generateComponentTest(m_camCadData, getLogFile(), m_aptModelMap, m_topProbeOutputCollection, m_topOutputArray, m_groundNet, m_icDiodeCommandMap, m_settings, surfaceTop);
                     generateProbablyShortTest(pcbFile->getFileStruct(), getLogFile(), m_topProbeOutputCollection, m_topOutputArray, surfaceTop);
                     m_netMap.generatePowerRailShortTest(getLogFile(), m_topProbeOutputCollection, m_topOutputArray, m_settings, surfaceTop);
                     m_componentCollection.generateComponentHighFlyZoneTest(pcbFile->getFileStruct(), m_camCadData, getLogFile(), m_topProbeOutputCollection, m_topOutputArray, m_settings, surfaceTop);                  
                  }              
               }
               else if (m_settings.getTestSurface() == surfaceTop)
               {
                  m_componentCollection.generateComponentTest(m_camCadData, getLogFile(), m_aptModelMap, m_topProbeOutputCollection, m_topOutputArray, m_groundNet, m_icDiodeCommandMap, m_settings, surfaceTop);
                  generateProbablyShortTest(pcbFile->getFileStruct(), getLogFile(), m_topProbeOutputCollection, m_topOutputArray, surfaceTop);
                  m_netMap.generatePowerRailShortTest(getLogFile(), m_topProbeOutputCollection, m_topOutputArray, m_settings, surfaceTop);
                  m_componentCollection.generateComponentHighFlyZoneTest(pcbFile->getFileStruct(), m_camCadData, getLogFile(), m_topProbeOutputCollection, m_topOutputArray, m_settings, surfaceTop);
               }
               else if (m_settings.getTestSurface() == surfaceBottom)
               {
                  m_componentCollection.generateComponentTest(m_camCadData, getLogFile(), m_aptModelMap, m_bottomProbeOutputCollection, m_bottomOutputArray, m_groundNet, m_icDiodeCommandMap, m_settings, surfaceBottom);
                  generateProbablyShortTest(pcbFile->getFileStruct(), getLogFile(), m_bottomProbeOutputCollection, m_bottomOutputArray, surfaceBottom);
                  m_netMap.generatePowerRailShortTest(getLogFile(), m_bottomProbeOutputCollection, m_bottomOutputArray, m_settings, surfaceBottom);
                  m_componentCollection.generateComponentHighFlyZoneTest(pcbFile->getFileStruct(), m_camCadData, getLogFile(), m_bottomProbeOutputCollection, m_bottomOutputArray, m_settings, surfaceBottom);
               }

            }
            else
            {
               ErrorMessage("No TestAccess point found!\nYou must run TestAccess Analysis first and rerun the Hioki output.",
					      "No Test Access Point found!");

               m_settings.setTopPopulated(false);
               m_settings.setBottomPopulated(false);
               break;
            }
            
            m_componentCollection.writeLogFile(getLogFile());
            m_netMap.writeLogFile(getLogFile());
         }
      }


      // Prep output file base name.
      // Get base filename, remove existing _top or _bot, if there is one.
      CFilePath outputFilePath(m_pathName);
      CString baseFilename( outputFilePath.getBaseFileName() );
      if (baseFilename.Right(4).CompareNoCase("_top") == 0  || baseFilename.Right(4).CompareNoCase("_bot") == 0)
      {
         baseFilename.Truncate( baseFilename.GetLength() - 4 );  // Drop _top or _bot suffix.
      }

      if (m_settings.getTopPopulated())
      {
         CString topBaseFilename(baseFilename + "_top");

         outputFilePath.setBaseFileName(topBaseFilename);
         outputFilePath.setExtension("cos");
         CString cosFilename = outputFilePath.getPath();

         outputFilePath.setExtension("ptp");
         CString ptpFilename( outputFilePath.getPath() );

         getProgressDlg().SetStatus("Outputing " + cosFilename);
         writeHiokiOutput(cosFilename, ptpFilename, m_topOutputArray, m_topProbeOutputCollection, surfaceTop);
      }

      if (m_settings.getBottomPopulated())
      {
         CString botBaseFilename(baseFilename + "_bot");

         outputFilePath.setBaseFileName(botBaseFilename);
         outputFilePath.setExtension("cos");
         CString cosFilename = outputFilePath.getPath();

         outputFilePath.setExtension("ptp");
         CString ptpFilename( outputFilePath.getPath() );
       
         getProgressDlg().SetStatus("Outputing " + cosFilename);
         writeHiokiOutput(cosFilename, ptpFilename, m_bottomOutputArray, m_bottomProbeOutputCollection, surfaceBottom);
      }
   }

}

bool HiokiWriter::getPcbDesignList()
{
   // Loop through all file in file list
   for (POSITION pos=m_camCadData.getFileList().GetHeadPosition(); pos!=NULL;)
   {
      FileStruct* file = m_camCadData.getFileList().GetNext(pos);
      if (file != NULL && file->getBlock() != NULL && file->isShown())
      {
         if (file->getBlockType() == blockTypePanel)
         {
            for (CDataListIterator dataList(*file->getBlock(), insertTypePcb); dataList.hasNext();)
            {
               InsertStruct* pcbInsert = dataList.getNext()->getInsert();
               FileStruct* pcbFile = m_camCadData.getFileList().FindByBlockNumber(pcbInsert->getBlockNumber());

               HiokiPcbFile* hiokiPcbFile = new HiokiPcbFile(*pcbFile);
               hiokiPcbFile->setName(pcbInsert->getRefname());
               hiokiPcbFile->setBlockNumber(pcbInsert->getBlockNumber());
               hiokiPcbFile->setMatrix(pcbInsert->getTMatrix());
               hiokiPcbFile->setInsertOrigin(pcbInsert->getOrigin2d());
               hiokiPcbFile->setRotationRadian(pcbInsert->getAngleRadians());
               hiokiPcbFile->setMirrored(pcbInsert->getGraphicMirrored());

               m_pcbDesignList.AddTail(hiokiPcbFile);
            }
         }
         else if (file->getBlockType() == blockTypePcb)
         {
            HiokiPcbFile* hiokiPcbFile = new HiokiPcbFile(*file);
            hiokiPcbFile->setName(file->getName());
            hiokiPcbFile->setBlockNumber(file->getBlock()->getBlockNumber());
            hiokiPcbFile->setInsertOrigin(CPoint2d(file->getInsertX(), file->getInsertY()));
            hiokiPcbFile->setRotationRadian(file->getRotationRadians());
            hiokiPcbFile->setMirrored(file->isMirrored());

            m_pcbDesignList.AddTail(hiokiPcbFile);
         }
      }
   }

   //FileStruct* file = m_camCadData.getSingleVisiblePanel();
   //if (file != NULL && file->getBlock() != NULL)
   //{
   //   // Panel exist so get PCB design on panel
   //      for (CDataListIterator dataList(*file->getBlock(), insertTypePcb); dataList.hasNext();)
   //      {
   //         InsertStruct* pcbInsert = dataList.getNext()->getInsert();
   //         HiokiPcbFile* hiokiPcbFile = new HiokiPcbFile();
   //         hiokiPcbFile->setName(pcbInsert->getRefname());
   //         hiokiPcbFile->setBlockNumber(pcbInsert->getBlockNumber());
   //         hiokiPcbFile->setOrigin(pcbInsert->getOrigin2d());
   //         hiokiPcbFile->setRotationRadian(pcbInsert->getAngleRadians());
   //         hiokiPcbFile->setMirrored(pcbInsert->getGraphicMirrored());

   //         m_pcbDesignList.AddTail(hiokiPcbFile);
   //      }
   //}
   //else
   //{
   //   // No pnael exist, look for PCB design in file list
   //   for (POSITION pos=m_camCadData.getFileList().GetHeadPosition(); pos!=NULL;)
   //   {
   //      file = m_camCadData.getFileList().GetNext(pos);
   //      if (file != NULL && file->getBlock() != NULL && file->isShown() && file->getBlockType() == blockTypePcb)
   //      {
   //         HiokiPcbFile* hiokiPcbFile = new HiokiPcbFile();
   //         hiokiPcbFile->setName(file->getName());
   //         hiokiPcbFile->setBlockNumber(file->getBlock()->getBlockNumber());
   //         hiokiPcbFile->setOrigin(CPoint2d(file->getInsertX(), file->getInsertY()));
   //         hiokiPcbFile->setRotationRadian(file->getRotationRadians());
   //         hiokiPcbFile->setMirrored(file->isMirrored());

   //         m_pcbDesignList.AddTail(hiokiPcbFile);
   //      }
   //   }
   //}

   return m_pcbDesignList.GetCount()>0;
}

void HiokiWriter::loadSettings()
{
   m_settings.reset();
   m_settings.setUnitFactor(m_unitFactor);
   defaultICDiodeTolerance = 25;
   uniqueProbeNames = false;
   FileStruct* pcbFile = m_camCadData.getFileList().GetOnlyShown(blockTypePcb);
   if (pcbFile != NULL)
   {
      /*CDFTSolution* dftSolution =  m_camCadDoc.GetCurrentDFTSolution(*pcbFile);
      if (dftSolution != NULL && dftSolution->GetTestPlan() != NULL)
      {         
         m_settings.setTestSurface(intToSurfaceTag(dftSolution->GetTestPlan()->GetSurface()));
         m_settings.setPreferenceSurface(intToSurfaceTag(dftSolution->GetTestPlan()->GetProbeSidePreference()));
      }*/
   }


   FILE *fp;
   char line[255];
   char *lp;

   CString settingsFilename( getApp().getExportSettingsFilePath("hioki.out") );
   {
      CString msg;
      msg.Format("Hioki Export: Settings file [%s].\n\n", settingsFilename);
      getApp().LogMessage(msg);
   }

   if ((fp = fopen(settingsFilename, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found.", settingsFilename);
      ErrorMessage(tmp, "Hioki Settings", MB_OK | MB_ICONHAND);
      return;
   }   

   while (fgets(line, 255, fp))
   {
      if ((lp = strtok(line, " \t\n")) == NULL)
         continue;

      if (lp[0] == '.')
      {
         if (!STRICMP(lp, ".ORIGIN_MODE"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
				CString val(lp);
            if (val.CompareNoCase("REFERENCE") == 0)
               m_settings.setOriginMode(relativeToReferencePoint); // Option
            else
               m_settings.setOriginMode(cczCadOrigin); // Default
         }
         else if (!STRICMP(lp, ".BOTTOM_POPULATED"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
				if (toupper(lp[0]) == 'N')
					m_settings.setBottomPopulated(false);
         }
         else if (!STRICMP(lp, ".TOP_POPULATED"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
				if (toupper(lp[0]) == 'N')
					m_settings.setTopPopulated(false);
         }
         else if (!STRICMP(lp, ".MAX_HIT_COUNT"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
				m_settings.setMaxHitCount(atoi(lp));
         }
         else if (!STRICMP(lp, ".POLARIZED_CAP_VISION"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
				if (toupper(lp[0]) == 'Y')
					m_settings.setPolarizedCapVisionTest(true);
         }
         else if (!STRICMP(lp, ".TANTALUM_CAP_VISION"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
				if (toupper(lp[0]) == 'Y')
					m_settings.setTantalumCapVisionTest(true);
         }
         else if (!STRICMP(lp, ".ALL_COMPONENT_VISION"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
				if (toupper(lp[0]) == 'Y')
					m_settings.setAllComponentVisionTest(true);
         }
         else if (!STRICMP(lp, ".SELECT_VISION"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
				if (toupper(lp[0]) == 'Y')
					m_settings.setSelectVisionTest(true);
         }
			else if (!STRICMP(lp, ".MODE"))
			{
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
				if (atoi(lp) == 2)
					m_settings.setTestLibraryMode(true);
			}
			else if (!STRICMP(lp, ".TEST_LIB"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
				m_settings.setTestLibraryFileName(lp);
			}
			else if (!STRICMP(lp, ".PP_COMMAND"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
            m_settings.setPPCommandAttribName(lp);
			}
			else if (!STRICMP(lp, ".SHORTS"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
            m_settings.setShortType(shortTypeStringToTag(lp));
			}
			else if (!STRICMP(lp, ".ANNOTATE_OUTPUT"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
            if (toupper(lp[0]) == 'Y')
               m_settings.setAnnotated(true);
            else
               m_settings.setAnnotated(false);
			}
			else if (!STRICMP(lp, ".PIN_TO_PIN_TYPE"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
            m_settings.addPinToPinDevice(lp);
			}
			else if (!STRICMP(lp, ".PIN_TO_PIN_DISTANCE"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
				m_settings.setPinToPinDistance(atof(lp));
			}		
			else if (!STRICMP(lp, ".MAX_VISION_COUNT"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
				m_settings.setMaxVisionTestCount(atoi(lp));
			}		
			else if (!STRICMP(lp, ".IC_DIODE"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
            CString partNumber = lp;

            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
            CString pinName = lp;

            bool swap = false;
            if ((lp = strtok(NULL, " \"\t\n")) != NULL)
               swap = STRCMPI(lp, "TRUE")==0?true:false;

            m_icDiodeCommandMap.addCommand(partNumber, pinName, swap);
         }
			else if (!STRICMP(lp, ".IC_DIODE_PREFIX"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
            m_settings.setIcDiodePrefix(lp);
         }
			else if (!STRICMP(lp, ".IC_CAP_PREFIX"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
            m_settings.setIcCapacitorPrefix(lp);
         }
         else if (!STRICMP(lp, ".TAKAYA_LIBRARY"))
         {
            if ((lp = strtok(NULL, "\"\t\n")) == NULL)
               continue;
            m_settings.setTakayaLibraryDirectory(lp);
         }
         else if (!STRICMP(lp, ".IC_OPEN_DESC"))
         {
            if ((lp = strtok(NULL, "\"\t\n")) == NULL)
               continue;
            CString icOpenDescription = lp;

            if (icOpenDescription.Left(1) == "<" && icOpenDescription.Right(1) == ">")
            {
               // If the string is inside a pair of <>, then it is the name of an attribute
               icOpenDescription = icOpenDescription.Mid(1, icOpenDescription.GetLength()-2);
               m_settings.setIsIcOpenDescriptionAttrib(true);
            }

            m_settings.setIcOpenDescription(icOpenDescription);
         }
         else if (!STRICMP(lp, ".COMMENT"))
         {
            if ((lp = strtok(NULL, "\"\t\n")) == NULL)
               continue;
            m_settings.setCommentAttribute(lp);
         }
         else if (!STRICMP(lp, ".HIGH_FLY_ZONE"))
         {
            if ((lp = strtok(NULL, "\"\t\n")) == NULL)
               continue;
            if (atof(lp) > 0)
               m_settings.setHighFlyZone(atof(lp));
         }
         ///add new command to include an option for sensor diameter
         else if (!STRICMP(lp, ".CAP_OPENS_SENSOR_DIAMETER"))
         {
            if ((lp = strtok(NULL, " =\t\n")) == NULL)
               continue;

            double capOpensSensorDiameterMM = atof(lp);
            m_settings.setCapOpensSensorDiameter(capOpensSensorDiameterMM);   // Save as found, in millimeters                    
         }
         ///add new command to include IC diode test when IC open test is created
         else if (!STRICMP(lp, ".ALLOW_IC_DIODE_WITH_IC_OPEN"))
         {
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
            
            m_settings.setAllowIcDiodeWithIcOpen(toupper(lp[0]) == 'Y' || toupper(lp[0]) == 'T');
            
         }
         else if (!STRICMP(lp, ".IC_DIODE_TOL"))
         {
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
            defaultICDiodeTolerance = round((atof(lp)));            
         }
         else if (!STRICMP(lp, ".UNIQUE_PROBE_NAMES"))
         {
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
            uniqueProbeNames = toupper(lp[0]) == 'Y' || toupper(lp[0]) == 'T';          
         }
		}
   }

   fclose(fp);
}

bool HiokiWriter::loadHiokiData(FileStruct& pcbFile)
{
   bool retval = false;
   CTMatrix matrix = pcbFile.getTMatrix();

   // Need to empty they before loading new data
   m_netMap.empty();
   m_compPinMap.empty();
   m_componentCollection.empty();
   m_groundNet = NULL;

   // This map is for temperary use to hold maps of compPins by component name
   CTypedMapStringToPtrContainer<HiokiCompPinMap*> compPinByCompNameMap(nextPrime2n(20), true);

   for (POSITION pos=pcbFile.getHeadNetPosition(); pos!=NULL;)
   {
      NetStruct* netStruct = pcbFile.getNextNet(pos);
      //dts0100596922  Process all nets, both used and unused
      if (netStruct != NULL)
      {
         HiokiNet* net = m_netMap.addNet(*netStruct);
         if (net != NULL && netStruct->getAttributes() != NULL)
         {
            CString value;
            if (m_camCadData.getAttributeStringValue(value, *netStruct->getAttributes(), standardAttributeTestNetStatus))
            {
               net->setIcGroundNet(value.CompareNoCase(netstatus[NETSTATUS_GROUND])==0);
               m_hasGroundNet = true;

               if (m_groundNet == NULL)
                  m_groundNet = net;
            }

            if (m_camCadData.getAttributeStringValue(value, *netStruct->getAttributes(), standardAttributeNetType))
            {
               if (value.CompareNoCase("POWER") == 0)
                  net->setNetType(netTypePower);
               else if (value.CompareNoCase("GROUND") == 0)
                  net->setNetType(netTypeGround);
            }
         }

         // Create HiokiCompPinMap
         for (POSITION compPinPos=netStruct->getHeadCompPinPosition(); compPinPos!=NULL;)
         {
            CompPinStruct* compPinStruct = netStruct->getNextCompPin(compPinPos);
            if (compPinStruct != NULL)
            {
               HiokiCompPin* compPin =  m_compPinMap.addCompPin(*compPinStruct, *net);
               if (compPin != NULL)
               {
                  if (compPinStruct->getAttributes() !=  NULL)
                  {
                     CString pinMap;
                     m_camCadData.getAttributeStringValue(pinMap, *compPinStruct->getAttributes(), standardAttributeDeviceToPackagePinMap);
                     compPin->setPinMap(pinMap);
                  }

                  HiokiCompPinMap* compPinMap = NULL;
                  if (!compPinByCompNameMap.Lookup(compPin->getCompName(), compPinMap))
                  {
                     compPinMap = new HiokiCompPinMap(false);
                     compPinByCompNameMap.SetAt(compPin->getCompName(), compPinMap);
                  }
                  compPinMap->addCompPin(compPin);
               }
            }
         }
      }
   }

   if (!m_hasGroundNet)
   {
      fprintf(getLogFile(), "No Net identified with Attribute [%s] Value [%s] Status for IC testing.\n", 
               ATT_TEST_NET_STATUS, netstatus[NETSTATUS_GROUND]);
   }

   for (CDataListIterator dataList(*pcbFile.getBlock(), dataTypeInsert); dataList.hasNext();)
   {
      DataStruct* data = dataList.getNext();
      InsertStruct* insert = data->getInsert();
      if (insert->getInsertType() == insertTypePcbComponent || insert->getInsertType() == insertTypeTestPoint)
      {
         HiokiComponent* component = m_componentCollection.addComponent(m_camCadData, pcbFile, *data, m_settings, matrix);
         if (component != NULL)
         {
            // Add HiokiCompPin
            HiokiCompPinMap* compPinMap = NULL;
            if (compPinByCompNameMap.Lookup(component->getName(), compPinMap))
            {
               for(POSITION pos=compPinMap->getStartPosition(); pos!=NULL;)
               {
                  HiokiCompPin* compPin = compPinMap->getNext(pos);
                  component->addCompPin(compPin);
               }
            }
         }
      }
      else if (insert->getInsertType() == insertTypeTestAccessPoint && data->getAttributes() != NULL)
      {
         // Only create HiokiTestAccess if test access point has NETNAME attribute
         CString stringValue;          
         if (m_camCadData.getAttributeStringValue(stringValue, *data->getAttributes(), standardAttributeNetName))
         {
            HiokiNet* net = m_netMap.findNet(stringValue);
            if (net != NULL)
            {
               // Get origin at board level
               CPoint2d testAccessOrigin = insert->getOrigin2d();
               matrix.transform(testAccessOrigin);               
               this->m_componentCollection.convertToHiokiUnits(testAccessOrigin, m_unitFactor, insert->getPlacedTop());

			      // Find out if the feature linked to the access point has TAKAYA_ACCESS attribute
               bool hasTakayaAccessAttrib = false;
               CAttribute* attrib = NULL;
               int targetEntityNumber = -1;
               if (data->getAttributes()->Lookup(m_camCadData.getAttributeKeywordIndex(standardAttributeDataLink), attrib))
               {
                  targetEntityNumber = m_camCadData.getAttributeIntegerValue(*attrib);
                  CEntity entity = CEntity::findEntity(m_camCadData, targetEntityNumber);
                  if (entity.getEntityType() == entityTypeData && entity.getData()->getAttributes() != NULL)
                  {
                     if (m_camCadData.getAttributeStringValue(stringValue, *entity.getData()->getAttributes(), standardAttributeTakayaAccess))
                        hasTakayaAccessAttrib = true;
                  }
                  else if (entity.getEntityType() == entityTypeCompPin && entity.getCompPin()->getAttributes() != NULL)
                  {
                     if (m_camCadData.getAttributeStringValue(stringValue, *entity.getCompPin()->getAttributes(), standardAttributeTakayaAccess))
                        hasTakayaAccessAttrib = true;
                  }
               }

               // Get target type priority
               int targetTypePriority  = INT_MAX-1;
               if (m_camCadData.getAttributeStringValue(stringValue, *data->getAttributes(), standardAttributeTargetType))
               {
				      CStringArray targetTypeArray;
				      CSupString targetTypeSupString(stringValue);
				      targetTypeSupString.Parse(targetTypeArray, ",");

				      for (int i=0; i<targetTypeArray.GetCount(); i++)
				      {
					      CString targetType = targetTypeArray.GetAt(i);
					      CDFTTarget* dftTarget = NULL;
					      int index = INT_MAX-1;

                     /*CDFTSolution *curDftSoln = m_camCadDoc.GetCurrentDFTSolution(pcbFile);
                     if (curDftSoln != NULL && curDftSoln->GetTestPlan() != NULL)
                     {
                        if (insert->getPlacedBottom())
                           dftTarget = curDftSoln->GetTestPlan()->GetTargetPriority().Find_BotTargets(targetType, index);
                        else
                           dftTarget = curDftSoln->GetTestPlan()->GetTargetPriority().Find_TopTargets(targetType, index);
                     }*/

					      if (dftTarget != NULL && index < targetTypePriority)
						      targetTypePriority = index;
				      }
               }

               // Create HiokiTestAccess
               HiokiTestAccess* testAccess = net->addTestAccess(data->getEntityNumber(), insert->getRefname(), testAccessOrigin, 
                     insert->getGraphicMirrored()?surfaceBottom:surfaceTop, hasTakayaAccessAttrib, targetTypePriority, targetEntityNumber);
            }
         }

         // set to TRUE as long as at least one Test Access Point is found
         retval = true;
      }
   }

   return retval;
}

void HiokiWriter::writeHiokiOutput(const CString fileName, const CString ptpFilename, CStringArray& outputArray, HiokiProbeOutputCollection& probeOutputCollection, const HiokiSurface testSurface)
{
   //*rcf TOO LATE !!! globalStepNo = 1;

   CStdioFileWriteFormat* file = new CStdioFileWriteFormat();
   file->open(fileName);

   // Write header
   CTime curTime;
	curTime = curTime.GetCurrentTime();
   if (m_settings.getAnnotated())
   {
      file->writef("! Generated by %s on %s\n", getApp().getCamCadSubtitle(), curTime.Format("%A, %B %d, %Y at %H:%M:%S"));
   }

   // Write board names
   CString boardNames;
   for (POSITION pos=m_pcbDesignList.GetHeadPosition(); pos!=NULL;)
   {
      HiokiPcbFile* pcbFile = m_pcbDesignList.GetNext(pos);
      if (pcbFile != NULL)
      {
         boardNames.AppendFormat("%s ", pcbFile->getName());
      }
   }
   
   // Write board reference command
   if (testSurface == surfaceTop)
   {
      // Use reference pt if origin mode is CAD; if origin mode is Reference then these would be 0,0 so not needed.
      if (m_settings.getOriginMode() == cczCadOrigin && !m_topBrdRefPntCommand.IsEmpty() && m_topBrdRefPntCommand.CompareNoCase(""))
			file->writef("%s\n", m_topBrdRefPntCommand);

		if (!m_topAuxRefPntCommand.IsEmpty() && m_topAuxRefPntCommand.CompareNoCase(""))
			file->writef("%s\n", m_topAuxRefPntCommand);
   }
   else
   {
		// Use reference pt if origin mode is CAD; if origin mode is Reference then these would be 0,0 so not needed.
      if (m_settings.getOriginMode() == cczCadOrigin && !m_botBrdRefPntCommand.IsEmpty() && m_botBrdRefPntCommand.CompareNoCase(""))
			file->writef("%s\n", m_botBrdRefPntCommand);

		if (!m_botAuxRefPntCommand.IsEmpty() && m_botAuxRefPntCommand.CompareNoCase(""))
			file->writef("%s\n", m_botAuxRefPntCommand);
   }

   // Write tests
   for (int i=0; i<outputArray.GetCount(); i++)
   {
      CString output = outputArray.GetAt(i);
      file->writef("%s\n", output);
   }

   // Write probes
   CStdioFileWriteFormat* ptpfile = new CStdioFileWriteFormat();
   ptpfile->open(ptpFilename);
   writeHiokiProbes(ptpfile, probeOutputCollection, testSurface);
   ptpfile->close();

   file->close();
}

void HiokiWriter::writeHiokiProbes(CStdioFileWriteFormat* file, HiokiProbeOutputCollection& probeOutputCollection, const HiokiSurface testSurface)
{
   if(testSurface == surfaceTop)
      DeleteAllProbes(&m_camCadDoc, m_camCadData.getFileList().GetOnlyShown(blockTypePcb));
   m_camCadDoc.PrepareAddEntity(m_camCadData.getFileList().GetOnlyShown(blockTypePcb));
   int probePlacementKw = m_camCadDoc.getStandardAttributeKeywordIndex(standardAttributeProbePlacement);
   int dataLinkKw = m_camCadDoc.getStandardAttributeKeywordIndex(standardAttributeDataLink);
   int netNameKw = m_camCadDoc.getStandardAttributeKeywordIndex(standardAttributeNetName);
   int refnameKW = m_camCadDoc.getStandardAttributeKeywordIndex(standardAttributeRefName);

   probeOutputCollection.SortByProbeNumber();
   for (int i= 0; i<probeOutputCollection.getCount(); i++)
   {
      HiokiProbeOutput* probeOutput = probeOutputCollection.getAt(i);
      if (probeOutput != NULL)
      {
         CPoint2d fptorigin = probeOutput->getInsertOrigin();
         if (testSurface == surfaceBottom)
            fptorigin.x = -fptorigin.x;

         if (m_settings.getAnnotated())
         {
            //file->writef("TAKAYA:%d: %.0f, %.0f, %s\n", probeOutput->getProbeNumber(), fptorigin.x, fptorigin.y, probeOutput->getNetName());
         }

         // Hioki
         file->writef("WPTP,%4d,%.3f,%.3f,%.3f\n",
            probeOutput->getProbeNumber(), fptorigin.x, fptorigin.y, 0.0); 

         // Update probes on CamcadDoc
         CEntity entity = CEntity::findEntity(m_camCadDoc.getCamCadData(), probeOutput->getTestAccessEntityNumber(), entityTypeData);
         if (entity.getEntityType() != entityTypeData)
            continue;

         DataStruct* testAccess = entity.getData();
         if (testAccess->getInsert() == NULL || testAccess->getInsert()->getInsertType() != insertTypeTestAccessPoint)
            continue;

         Attrib* attrib = NULL;
         double testAccessFeatureSize = 0.0;
         if (attrib = is_attvalue(&m_camCadDoc, testAccess->getAttributes(), DFT_ATT_EXPOSE_METAL_DIAMETER, 0))
            testAccessFeatureSize = attrib->getDoubleValue();

         CString probeBlockName;
         probeBlockName.Format("%0.3f_%s", testAccessFeatureSize, (testAccess->getInsert()->getPlacedBottom() == 0)?"Top":"Bottom");
         BlockStruct* probeBlock = probeBlock = CreateTestProbeGeometry(&m_camCadDoc, probeBlockName, testAccessFeatureSize, probeBlockName, testAccessFeatureSize);

         CString probeName, probeRefname;
         probeName.Format("%d_%s", probeOutput->getProbeNumber(), (testAccess->getInsert()->getPlacedBottom() == 0)?"Top":"Bottom");
         probeRefname.Format("%d", probeOutput->getProbeNumber());
         DataStruct *probeData = Graph_Block_Reference(probeBlock->getName(), probeName, probeBlock->getFileNumber(), 
            testAccess->getInsert()->getOriginX(), testAccess->getInsert()->getOriginY(), 0, (testAccess->getInsert()->getPlacedBottom() == 0)?FALSE:TRUE, 1.0, -1, FALSE);
         probeData->getInsert()->setInsertType(insertTypeTestProbe);
         probeData->setHidden(false);

         void* voidPtr = (void*)"Placed";
         int entityNumber = probeOutput->getTestAccessEntityNumber();
         probeData->setAttrib(m_camCadDoc.getCamCadData(), probePlacementKw, valueTypeString, voidPtr, attributeUpdateOverwrite, NULL);
         probeData->setAttrib(m_camCadDoc.getCamCadData(), dataLinkKw, valueTypeInteger, (void*)&entityNumber, attributeUpdateOverwrite, NULL);
         probeData->setAttrib(m_camCadDoc.getCamCadData(), netNameKw, valueTypeString, probeOutput->getNetName().GetBuffer(0), attributeUpdateOverwrite, NULL);

         // Add probe number as REFNAME attribute
         CreateTestProbeRefnameAttr(&m_camCadDoc, probeData, probeName, testAccessFeatureSize);
      }
   }
}

void HiokiWriter::generateProbablyShortTest(FileStruct& fileStruct, FILE *logFp, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, const HiokiSurface testSurface)
{
   for (POSITION pos=fileStruct.getDRCList().GetHeadPosition(); pos!=NULL;)
   {
      DRCStruct* drc = fileStruct.getDRCList().GetNext(pos);
      if (drc != NULL)
      {
         DRC_MeasureStruct* drcMeasure = (DRC_MeasureStruct*)(drc->getVoidPtr());

         CString netName1, netName2, compPinRef1, compPinRef2;

         if (drc->getAlgorithmType() == DRC_ALG_PROBABLESHORT_PINtoPIN)
         {
            NetStruct* netStruct = NULL;
            CompPinStruct* comppin = CEntity::findCompPinEntity(m_camCadData, drcMeasure->entity1, NULL, &netStruct);
            if (netStruct != NULL && comppin != NULL)
            {
               netName1 = netStruct->getNetName();
               compPinRef1 = comppin->getPinRef('-');
            }

            comppin = CEntity::findCompPinEntity(m_camCadData, drcMeasure->entity2, NULL, &netStruct);            
            if (netStruct != NULL && comppin != NULL)
            {
               netName2 = netStruct->getNetName();
               compPinRef2 = comppin->getPinRef('-');
            }
         }
         else if (drc->getAlgorithmType() == DRC_ALG_PROBABLESHORT_PINtoFEATURE)
         {
            NetStruct* netStruct = NULL;
            CompPinStruct* comppin = CEntity::findCompPinEntity(m_camCadData, drcMeasure->entity1, NULL, &netStruct);
            if (netStruct != NULL && comppin != NULL)
            {
               netName1 = netStruct->getNetName();
               compPinRef1 = comppin->getPinRef('-');
            }

            DataStruct* data = CEntity::findDataEntity(m_camCadData, drcMeasure->entity2);
            if (data != NULL && data->getAttributes() != NULL)
               m_camCadData.getAttributeStringValue(netName2, *data->getAttributes(), standardAttributeNetName);
         }
         else if (drc->getAlgorithmType() == DRC_ALG_PROBABLESHORT_FEATUREtoFEATURE)
         {
            DataStruct* data = CEntity::findDataEntity(m_camCadData, drcMeasure->entity1);
            if (data != NULL && data->getAttributes() != NULL)
               m_camCadData.getAttributeStringValue(netName1, *data->getAttributes(), standardAttributeNetName);

            data = CEntity::findDataEntity(m_camCadData, drcMeasure->entity2);
            if (data != NULL && data->getAttributes() != NULL)
               m_camCadData.getAttributeStringValue(netName2, *data->getAttributes(), standardAttributeNetName);
         }

         HiokiNet* net1 = m_netMap.findNet(netName1);
         HiokiNet* net2 = m_netMap.findNet(netName2);

         CString netPair;
         if (net1 != NULL && net2 != NULL && net1 != net2 &&
             !m_probablyShortNetPairMap.Lookup(netName1+netName2, netPair) &&
             !m_probablyShortNetPairMap.Lookup(netName2+netName1, netPair))
         {
            HiokiTestAccess* testAccess1 = net1->getTestAccess(logFp, m_settings.getMaxHitCount(), testSurface);
            HiokiTestAccess* testAccess2 = net2->getTestAccess(logFp, m_settings.getMaxHitCount(), testSurface);

            if (testAccess1 != NULL && testAccess2 != NULL)
            {
               if (compPinRef1.IsEmpty())
                  compPinRef1 = getCompPinRefForProbableShort(net1->getNetStruct());
               if (compPinRef2.IsEmpty())
                  compPinRef2 = getCompPinRefForProbableShort(net2->getNetStruct());

               CString probeNumber1 = probeOutputCollection.getProbeNumberString(*testAccess1);
               CString probeNumber2 = probeOutputCollection.getProbeNumberString(*testAccess2);

               CString output;

               // For Hioki
               CString modifiedName;
               modifiedName.Format("%s %s", compPinRef1, compPinRef2); 

               CString annotation;
               if (m_settings.getAnnotated())
               {
                  annotation.Format(" ! Probably-short Test ModifiedRef[%s] CompPin1[%s] CompPin2[%s] Probe1[%s] Probe2[%s]",
                     modifiedName, compPinRef1, compPinRef2, probeNumber1, probeNumber2);
               }

               HiokiCosRecord cosRec(modifiedName, NULL, HIOKI_TEST_SHORT, probeNumber1, probeNumber2);
               cosRec.GetFormattedRecord(globalStepNo++, output);
               output += annotation;
               outputArray.Add(output);

               m_probablyShortNetPairMap.SetAt(netName1+netName2, netName1+netName2);
               m_probablyShortNetPairMap.SetAt(netName2+netName1, netName2+netName1);
            }
         }       
      }
   }
}

CString HiokiWriter::getCompPinRefForProbableShort(NetStruct& netStruct)
{
   CString compPinRef;

   for (POSITION pos=netStruct.getHeadCompPinPosition(); pos!=NULL;)
   {
      CompPinStruct* compPin = netStruct.getNextCompPin(pos);
      if (compPin != NULL)
      {
         HiokiComponent* component = m_componentCollection.findComponent(compPin->getRefDes());
         DeviceTypeTag deviceType = component==NULL?deviceTypeUndefined:component->getDeviceType();

         if (deviceType == deviceTypeConnector)
         {
            compPinRef = compPin->getPinRef('-');
            break;
         }
         else if (compPinRef.IsEmpty() || deviceType == deviceTypeIC || deviceType || deviceTypeICDigital || deviceType == deviceTypeICLinear)
         {
            compPinRef = compPin->getPinRef('-');
         }
      }
   }

   return compPinRef;
}

//-------------------------------------------------------------------------

#define HIOKI_TEST_NONE       ' '
#define HIOKI_TEST_RESISTOR   'R'
#define HIOKI_TEST_INDUCTOR   'L'
#define HIOKI_TEST_CAPACITOR  'C'
#define HIOKI_TEST_ZENER      'Z'
#define HIOKI_TEST_TRANSISTOR 'Q'
#define HIOKI_TEST_DIODE      'D'
#define HIOKI_TEST_VOLTAGE    'V'
#define HIOKI_TEST_OPEN       'O'
#define HIOKI_TEST_SHORT      'S'
#define HIOKI_TEST_DISCHARGE  'd'
#define HIOKI_TEST_VISION     'v'
#define HIOKI_TEST_4LEADRES   'r'

char GetHiokiTestCode(DeviceTypeTag devtype)
{
   switch (devtype)
   {
   case deviceTypeResistor:              return HIOKI_TEST_RESISTOR;
   case deviceTypeInductor:              return HIOKI_TEST_INDUCTOR;
   case deviceTypeCapacitor:             return HIOKI_TEST_CAPACITOR;
   case deviceTypeCapacitorPolarized:    return HIOKI_TEST_CAPACITOR;
   case deviceTypeCapacitorTantalum:     return HIOKI_TEST_CAPACITOR;
   case deviceTypeDiodeZener:            return HIOKI_TEST_ZENER;
   case deviceTypeTransistor:            return HIOKI_TEST_TRANSISTOR;
	case deviceTypeTransistorFetNpn:      return HIOKI_TEST_TRANSISTOR;
	case deviceTypeTransistorFetPnp:      return HIOKI_TEST_TRANSISTOR;
	case deviceTypeTransistorMosfetNpn:   return HIOKI_TEST_TRANSISTOR;
	case deviceTypeTransistorMosfetPnp:   return HIOKI_TEST_TRANSISTOR;
	case deviceTypeTransistorNpn:         return HIOKI_TEST_TRANSISTOR;
	case deviceTypeTransistorPnp:         return HIOKI_TEST_TRANSISTOR;
	case deviceTypeTransistorScr:         return HIOKI_TEST_TRANSISTOR;
	case deviceTypeTransistorTriac:       return HIOKI_TEST_TRANSISTOR;
   case deviceTypeDiode:                 return HIOKI_TEST_DIODE;
   }

   return HIOKI_TEST_NONE;
}

//-------------------------------------------------------------------------

//HiokiCosRecord::HiokiCosRecord()
//{
//}

HiokiCosRecord::HiokiCosRecord(CString refDes, HiokiComponent *hiokiComp)
: m_refdes(refDes)
, m_testType(HIOKI_TEST_NONE)
, m_hiokiComp(hiokiComp)
{
}

HiokiCosRecord::HiokiCosRecord(CString refDes, HiokiComponent *hiokiComp, CString HProbeNum, CString LProbeNum)
: m_refdes(refDes)
, m_testType(HIOKI_TEST_NONE)
, m_hiokiComp(hiokiComp)
, m_HProbeNum(HProbeNum)
, m_LProbeNum(LProbeNum)
{
}

HiokiCosRecord::HiokiCosRecord(CString refDes, HiokiComponent *hiokiComp, char testType, CString HProbeNum, CString LProbeNum)
: m_refdes(refDes)
, m_testType(testType)
, m_hiokiComp(hiokiComp)
, m_HProbeNum(HProbeNum)
, m_LProbeNum(LProbeNum)
{
}


void HiokiCosRecord::GetFormattedRecord(int stepNo, CString &output)
{
   // Format a COS file record.

   CString partPosition( m_hiokiComp != NULL ? m_hiokiComp->getLocation() : "" );

   CString measureMode("00"); //*rcf get real measure mode
   CString measureRange("00"); //*rcf get real measure range

   CString actualValue( m_hiokiComp != NULL ? m_hiokiComp->getHiokiValue() : "");
   CString standardValue(actualValue);

   CString maxVal(m_hiokiComp != NULL ? m_hiokiComp->getPositiveTol() : "0.0");
   CString minVal(m_hiokiComp != NULL ? m_hiokiComp->getNegativeTol() : "0.0");

   // Note no ending \n. The output loop writing the stored recs will provide the newline.

   // Field Number:  1    2     3     4    5    6     7     8     9     10      11     12     13
   output.Format("%4.4s,%5d,%12.12s,%2.2s,%c,%2.2s,%2.2s,%4.4s,%4.4s,%10.10s,%10.10s,%5.5s [%%],%5.5s [%%]",
      "WCOS",        //  1. Header
      stepNo,        //  2. Step Number
      m_refdes,      //  3. Parts Name
      partPosition,  //  4. Part Position
      m_testType,    //  5. Device Name  aka Test Type
      measureMode,   //  6. Measure Mode
      measureRange,  //  7. Measure Range
      m_HProbeNum,   //  8. H Pattern No
      m_LProbeNum,   //  9. L Pattern No
      actualValue,   // 10. Actual Value
      standardValue, // 11. Standard value
      maxVal,        // 12. Maximum value (plus tolerance)
      minVal         // 13. Minimum value (minus tolerance)
      );
}

