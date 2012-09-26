// $Header: /CAMCAD/DcaLib/DcaApertureShape.cpp 3     3/09/07 5:15p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// TAKAYAFPTWRITE.CPP

#include "stdafx.h"
#include "CCEtoODB.h"
#include "DFT.h"
#include "DeviceType.h"
#include "Gauge.h"
#include "PcbUtil.h"
#include "Takaya_o.h"
#include "TakayaFPTWrite.h"
#include "RwUiLib.h"
#include "CompValues.h"

extern char* netstatus[];

static int defaultICDiodeTolerance = 25;
static TakayaFPTProbeOutputCollection m_topProbeOutputCollection;
static TakayaFPTProbeOutputCollection m_bottomProbeOutputCollection;
int m_noProbeAccessMarkCountTop;
int m_noProbeAccessMarkCountBot;
static bool uniqueProbeNames = false;

void WriteTakayaFPT(CCEtoODBDoc& camCadDoc, const CString pathName)
{
   TakayaFPTWrite takayaWriter(camCadDoc, pathName);
   takayaWriter.write();
}

TakayaFPTSurface intToSurfaceTag(const int surface)
{
   TakayaFPTSurface retval = surfaceUnknown;

   switch (surface)
   {
   case 0:  retval = surfaceTop;       break;
   case 1:  retval = surfaceBottom;    break;
   case 2:  retval = surfaceBoth;      break;
   default: retval = surfaceUnknown;   break;
   }

   return retval;
}

TakayaShortType shortTypeStringToTag(const CString shortTypeString)
{
   if (shortTypeString.CompareNoCase("ANALYSIS") == 0)      return shortTypeAnalysis;
   if (shortTypeString.CompareNoCase("PIN_TO_PIN") == 0)   return shortTypePinToPin;
   
   return shortTypeUnknown;
}

TakayaPinMapType pinMapTypeStringToTag(const CString pinMapTypeString)
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

void convertToTakayaUnits(double& value, double unitFactor)
{
   value = floor(value * unitFactor + 0.5);
}

//_____________________________________________________________________________
TakayaFPTTestAccess::TakayaFPTTestAccess(const long accessEntityNumber,const CString name, const CString netName, const CPoint2d xyOrigin,
                       const TakayaFPTSurface surface,const bool isOnIcGroundNet,const bool hasTakayaAccessAttrib,const int targetTypePriority,
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

TakayaFPTTestAccess::~TakayaFPTTestAccess()
{
}

//_____________________________________________________________________________
TakayaFPTTestAccessCollection::TakayaFPTTestAccessCollection()
: m_testAccessWithAttribute(true)
, m_testAccessWithPriority(true)
{
   m_testAccessWithAttribute.SetSize(0, 20);
   m_testAccessWithPriority.SetSize(0, 20);
   m_curTestAccessWithAttributeIndex = 0;
   m_curTestAccessWithPriorityIndex = 0;
   m_priorityArrayAlreadySorted = false;
}

TakayaFPTTestAccessCollection::~TakayaFPTTestAccessCollection()
{
   empty();
}

void TakayaFPTTestAccessCollection::empty()
{
   m_testAccessWithAttribute.empty();
   m_testAccessWithPriority.empty();
}

   
TakayaFPTTestAccess* TakayaFPTTestAccessCollection::GetFirstTestAccess()
{
   // iterator index marks the one to return next
   m_iteratorIndex = 0;

   return GetNextTestAccess();
}
TakayaFPTTestAccess* TakayaFPTTestAccessCollection::GetNextTestAccess()
{
   // If iteratorIndex is within range of listA then return from there.
   // Othewise subtract size of listA from index, and try listB.
   // If outside of range for both then return NULL.
   // Increment the iteratorIndex to pt to next to return if we do return one.
   // If returning NULL then leave iteratorIndex alone.

   TakayaFPTTestAccess *keeper = NULL;

   int countListA = m_testAccessWithAttribute.GetCount();
   if (m_iteratorIndex >= 0 && m_iteratorIndex < countListA)
   {
      keeper = m_testAccessWithAttribute.GetAt(m_iteratorIndex);
   }
   else
   {
      int countListB = m_testAccessWithPriority.GetCount();
      int offsetIteratorIndex = m_iteratorIndex - countListA;
      if (offsetIteratorIndex >= 0 && offsetIteratorIndex < countListB)
      {
         keeper = m_testAccessWithPriority.GetAt(offsetIteratorIndex);
      }
   }

   if (keeper != NULL)
      m_iteratorIndex++;

   return keeper;
}

TakayaFPTTestAccess* TakayaFPTTestAccessCollection::addTestAccess(const long entityNumber, const CString name, const CString netName, const CPoint2d xyOrigin,
                       const TakayaFPTSurface surface,const bool isOnIcGroundNet,const bool hasTakayaAccessAttrib,const int targetTypePriority,const long targetEntityNumber)
{
   TakayaFPTTestAccess* testAccess = new TakayaFPTTestAccess(entityNumber, name, netName, xyOrigin, surface, isOnIcGroundNet, hasTakayaAccessAttrib, targetTypePriority, targetEntityNumber);
   if (hasTakayaAccessAttrib)
      m_testAccessWithAttribute.SetAtGrow(m_testAccessWithAttribute.GetCount(), testAccess);
   else
      m_testAccessWithPriority.SetAtGrow(m_testAccessWithPriority.GetCount(), testAccess);

   return testAccess;
}

static int priorityCompareFunction(const void *arg1, const void* arg2)
{
   TakayaFPTTestAccess **testAccess1, **testAccess2;
   testAccess1 = (TakayaFPTTestAccess**)arg1;
   testAccess2 = (TakayaFPTTestAccess**)arg2;

   if ((*testAccess1)->getTargetTypePriority() < (*testAccess2)->getTargetTypePriority())
      return -1;
   else if ((*testAccess1)->getTargetTypePriority() > (*testAccess2)->getTargetTypePriority())
      return 1;
   else
      return 0;
}

TakayaFPTTestAccess* TakayaFPTTestAccessCollection::getTestAccess(FILE *logFp, const int maxHits, TakayaFPTTestAccess* testAccessUsed)
{
   int index = m_curTestAccessWithAttributeIndex;
   TakayaFPTTestAccess* testAccess =  NULL;
   while (index < m_testAccessWithAttribute.GetCount())
   {
      TakayaFPTTestAccess* tempTestAccess = m_testAccessWithAttribute.GetAt(index);
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
         qsort(m_testAccessWithPriority.GetData(), m_testAccessWithPriority.GetCount(), sizeof(TakayaFPTTestAccess*), priorityCompareFunction);
         m_priorityArrayAlreadySorted = true;
      }

      index = m_curTestAccessWithPriorityIndex;
      while (index < m_testAccessWithPriority.GetCount())
      {
         TakayaFPTTestAccess* tempTestAccess = m_testAccessWithPriority.GetAt(index);
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

TakayaFPTTestAccess* TakayaFPTTestAccessCollection::getTestAccessOnTarget(const long targetEntityNumber)
{
   for (int index = 0; index < m_testAccessWithAttribute.GetCount(); index++)
   {
      TakayaFPTTestAccess* testAccess = m_testAccessWithAttribute.GetAt(index);
      if (testAccess != NULL && testAccess->getTargetEntityNumber() == targetEntityNumber) 
      {
         return testAccess;
      }
   }

   for (int index = 0; index < m_testAccessWithPriority.GetCount(); index++)
   {
      TakayaFPTTestAccess* testAccess = m_testAccessWithPriority.GetAt(index);
      if (testAccess != NULL && testAccess->getTargetEntityNumber() == targetEntityNumber) 
      {
         return testAccess;
      }
   }

   return NULL;
}

TakayaFPTTestAccess* TakayaFPTTestAccessCollection::getTestAccess(FILE *logFp, const int maxHits)
{
   return getTestAccess(logFp, maxHits, NULL);
}

void TakayaFPTTestAccessCollection::getTwoTestAccess(FILE *logFp, const int maxHits, TakayaFPTTestAccess** testAccess1, TakayaFPTTestAccess** testAccess2)
{
   *testAccess1 = getTestAccess(logFp, maxHits, NULL);
   *testAccess2 = getTestAccess(logFp, maxHits, *testAccess1);
}

void TakayaFPTTestAccessCollection::writeLogFile(FILE *logFp)
{
   for (int indx = 0; indx < m_testAccessWithAttribute.GetCount(); indx++)
   {
      TakayaFPTTestAccess* fptaccess = m_testAccessWithAttribute.GetAt(indx);
      fprintf(logFp, "   ACCESS [%s]  HITS [%d]\n", fptaccess->getName(), fptaccess->getUsedCount());
   }

   for (int indx = 0; indx < m_testAccessWithPriority.GetCount(); indx++)
   {
      TakayaFPTTestAccess* fptaccess = m_testAccessWithPriority.GetAt(indx);
      fprintf(logFp, "   ACCESS [%s]  HITS [%d]\n", fptaccess->getName(), fptaccess->getUsedCount());
   }
}

//_____________________________________________________________________________
TakayaFPTNet::TakayaFPTNet(NetStruct& netStruct)
: m_netStruct(netStruct)
, m_name(netStruct.getNetName())
, m_netType(netTypeUnknown)
, m_isMaxHitExceeded(false)
, m_isIcGroundNet(false)
, m_powerRailTestedSurface(surfaceUnknown)
{
}

TakayaFPTNet::~TakayaFPTNet()
{
   m_topTestAccessCollection.empty();
   m_bottomTestAccessCollection.empty();
}

TakayaFPTTestAccess* TakayaFPTNet::addTestAccess(const long entityNumber, const CString name, const CPoint2d xyOrigin,
                       const TakayaFPTSurface surface,const bool hasTakayaAccessAttrib,const int targetTypePriority,const long targetEntityNumber)
{
   TakayaFPTTestAccess* testAccess = NULL;
   if (surface == surfaceTop)
      testAccess = m_topTestAccessCollection.addTestAccess(entityNumber, name, m_name, xyOrigin, surface, m_isIcGroundNet, hasTakayaAccessAttrib, targetTypePriority, targetEntityNumber);
   else if (surface == surfaceBottom)
      testAccess = m_bottomTestAccessCollection.addTestAccess(entityNumber, name, m_name, xyOrigin, surface, m_isIcGroundNet, hasTakayaAccessAttrib, targetTypePriority, targetEntityNumber);

   return testAccess;
}

TakayaFPTTestAccess* TakayaFPTNet::getTestAccess(FILE *logFp, const int maxHits, const TakayaFPTSurface testSurface)
{
   TakayaFPTTestAccess* testAccess = NULL;

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

TakayaFPTTestAccess* TakayaFPTNet::getTestAccessOnTarget(const long targetEntityNumber, const TakayaFPTSurface surface)
{
   TakayaFPTTestAccess* testAccess = NULL;

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

void TakayaFPTNet::getTwoTestAccess(FILE *logFp, const int maxHits, const TakayaFPTSurface testSurface, TakayaFPTTestAccess** testAccess1, TakayaFPTTestAccess** testAccess2)
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

void TakayaFPTNet::writeLogFile(FILE *logFp)
{
   fprintf(logFp, "NET [%s]\n", this->getName());

   this->m_topTestAccessCollection.writeLogFile(logFp);
   this->m_bottomTestAccessCollection.writeLogFile(logFp);
}

//_____________________________________________________________________________
TakayaFPTProbeOutput::TakayaFPTProbeOutput(const int probeNumber, const CString netName, const CPoint2d origin,const int testAccessEntityNumber)
{
   m_probeNumber = probeNumber;
   m_netName = netName;
   m_xyOrigin = origin;
   m_testAccessEntityNumber = testAccessEntityNumber;
}

TakayaFPTProbeOutput::~TakayaFPTProbeOutput()
{
}

//_____________________________________________________________________________
TakayaFPTProbeOutputCollection::TakayaFPTProbeOutputCollection()
: m_probeOutputArray(true)
{
   m_probeOutputArray.SetSize(0, 100);
}

TakayaFPTProbeOutputCollection::~TakayaFPTProbeOutputCollection()
{
   RemoveAll();
}

void TakayaFPTProbeOutputCollection::RemoveAll()
{
   m_probeOutputArray.empty();
   m_testAccessToProbeNumberMap.RemoveAll();
}

CString TakayaFPTProbeOutputCollection::getProbeNumberString(TakayaFPTTestAccess* testAccess, bool autoGenerate)
{
   if (testAccess != NULL)
      return getProbeNumberString(*testAccess, autoGenerate);

   return "N?";  // this is an error in output, but better than blank (giving no clue)
}

CString TakayaFPTProbeOutputCollection::getProbeNumberString(TakayaFPTTestAccess& testAccess, bool autoGenerate)
{
   // If autoGenerate == true then generate a probe at access point if none is already there.
   // If false then return existing probe name or empty string if none.

   CString probeNumberString;
   if ((!m_testAccessToProbeNumberMap.Lookup(testAccess.getName(), probeNumberString)) && autoGenerate)
   {  
      int probeNumber;
      if (uniqueProbeNames == true)
         probeNumber = m_bottomProbeOutputCollection.getCount() + m_topProbeOutputCollection.getCount() + 1;         
      else
         probeNumber = m_probeOutputArray.GetCount()+1;

      TakayaFPTProbeOutput* probeOutput = new TakayaFPTProbeOutput(probeNumber, testAccess.getNetName(), testAccess.getInsertOrigin(),testAccess.getAccessEntityNumber());
      m_probeOutputArray.SetAtGrow(m_probeOutputArray.GetCount(), probeOutput);

      probeNumberString.Format("N%d", probeNumber);
      m_testAccessToProbeNumberMap.SetAt(testAccess.getName(), probeNumberString);  // Access refname is not reliable, would maybe be beter off using entity#.
   }

   return probeNumberString;
}

int TakayaFPTProbeOutputCollection::getCount()
{
   return m_probeOutputArray.GetCount();
}

TakayaFPTProbeOutput* TakayaFPTProbeOutputCollection::getAt(const int index)
{
   TakayaFPTProbeOutput* probeOutput = NULL;
   if (index < m_probeOutputArray.GetCount())
      probeOutput = m_probeOutputArray.GetAt(index);

   return probeOutput;
}

void TakayaFPTProbeOutputCollection::SortByProbeNumber()
{
   m_probeOutputArray.setSortFunction(TakayaFPTProbeOutputCollection::AscendingProbeNumberFunc);
   m_probeOutputArray.sort();
}

int TakayaFPTProbeOutputCollection::AscendingProbeNumberFunc(const void *a, const void *b)
{
   TakayaFPTProbeOutput* probeOutput1 = *((TakayaFPTProbeOutput**)(a));
   TakayaFPTProbeOutput* probeOutput2 = *((TakayaFPTProbeOutput**)(b));
   return (probeOutput1->getProbeNumber() - probeOutput2->getProbeNumber());
}

//_____________________________________________________________________________
TakayaFPTNetMap::TakayaFPTNetMap()
: m_netNameMap(nextPrime2n(20), true)
, m_powerGroundNameMap(nextPrime2n(20))
{
   m_powerNetCount = 0;
   m_groundNetCount = 0;
}

TakayaFPTNetMap::~TakayaFPTNetMap()
{
   empty();
}

void TakayaFPTNetMap::empty()
{
   m_netNameMap.empty();
   m_powerGroundNameMap.RemoveAll();
}

TakayaFPTNet* TakayaFPTNetMap::addNet(NetStruct& netStruct)
{
   TakayaFPTNet* net = findNet(netStruct.getNetName());
   if (!netStruct.getNetName().IsEmpty() && net == NULL)
   {
      net = new TakayaFPTNet(netStruct);
      CString key = netStruct.getNetName();
      key.MakeLower();
      m_netNameMap.setAt(key, net);
   }

   return net;
}

TakayaFPTNet* TakayaFPTNetMap::findNet(const CString netName)
{
   TakayaFPTNet* net = NULL;
   if (!netName.IsEmpty())
   {
      CString key = netName;
      key.MakeLower();
      m_netNameMap.Lookup(key, net);
   }

   return net;
}

CString TakayaFPTNetMap::getPowerGroundName(TakayaFPTNet& net)
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

void TakayaFPTNetMap::generatePowerRailShortTest(FILE *logFp, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
{
   CMapStringToString probePairMap;

   for (POSITION pos1=m_netNameMap.GetStartPosition(); pos1!=NULL;)
   {
      CString key;
      TakayaFPTNet* net1 = NULL;
      m_netNameMap.GetNextAssoc(pos1, key, net1);

      // If a Power Rail short test is done on one surafce, then it can not be done on anotehr
      if (net1 != NULL && net1->getNetType() != netTypeUnknown && (net1->getPowerRailTestedSurface() == surfaceUnknown || net1->getPowerRailTestedSurface() == testSurface))
      {
         for (POSITION pos2=pos1; pos2!=NULL;)
         {
            TakayaFPTNet* net2 = NULL;
            m_netNameMap.GetNextAssoc(pos2, key, net2);
            if (net2 != NULL && net2->getNetType() != netTypeUnknown && (net2->getPowerRailTestedSurface() == surfaceUnknown || net2->getPowerRailTestedSurface() == testSurface))
            {
               TakayaFPTTestAccess* testAccess1 = net1->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
               TakayaFPTTestAccess* testAccess2 = net2->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
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

void TakayaFPTNetMap::writeLogFile(FILE *logFp)
{
   fprintf(logFp, "\n\nNet Access Location Hit Counts\n\n");

   POSITION netpos = m_netNameMap.GetStartPosition();
   while (netpos != NULL)
   {
      CString netname;
      TakayaFPTNet *fptnet;
      m_netNameMap.GetNextAssoc(netpos, netname, fptnet);

      if (fptnet != NULL)
         fptnet->writeLogFile(logFp);
   }
}

//_____________________________________________________________________________
TakayaFPTCompPin::TakayaFPTCompPin(CompPinStruct& compPinStruct, TakayaFPTNet& net)
: m_compPinStruct(compPinStruct)
, m_net(net)
{
   m_alreadyTested = false;
   m_testedSurface = surfaceUnknown;
   m_topTestAccess = NULL;
   m_bottomTestAccess = NULL;
}

TakayaFPTCompPin::~TakayaFPTCompPin()
{
   m_topTestAccess = NULL;
   m_bottomTestAccess = NULL;
}

TakayaFPTTestAccess* TakayaFPTCompPin::getTestAccess(FILE *logFp, const int maxHits, const TakayaFPTSurface testSurface)
{
   TakayaFPTTestAccess* testAccess = NULL;
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

TakayaFPTTestAccess* TakayaFPTCompPin::getTestAccessOnSelf(const TakayaFPTSurface testSurface)
{
   // If self has an access marker on surface, return it.
   // If not, return NULL, even if there is other access on same net.

   return m_net.getTestAccessOnTarget(this->getEntityNumber(), testSurface);
}

//_____________________________________________________________________________
TakayaFPTCompPinMap::TakayaFPTCompPinMap(bool isContainer)
: m_compPinMap(nextPrime2n(20), isContainer)
{
   m_alreadySorted = false;
}

TakayaFPTCompPinMap::~TakayaFPTCompPinMap()
{
   empty();
}

void TakayaFPTCompPinMap::empty()
{
   m_compPinMap.empty();
}

void TakayaFPTCompPinMap::addCompPin(TakayaFPTCompPin* compPin)
{
   if (compPin != NULL)
   {
      if (findCompPin(compPin->getCompPinName()) == NULL)
      {
         m_compPinMap.SetAt(compPin->getCompPinName(), compPin);
      }
   }
}

TakayaFPTCompPin* TakayaFPTCompPinMap::addCompPin(CompPinStruct& compPinStruct, TakayaFPTNet& net)
{
   TakayaFPTCompPin* compPin = findCompPin(compPinStruct.getPinRef());
   if (!compPinStruct.getPinRef().IsEmpty() && compPin == NULL)
   {
      compPin = new TakayaFPTCompPin(compPinStruct, net);
      m_compPinMap.SetAt(compPin->getCompPinName(), compPin);
   }

   return compPin;
}

TakayaFPTCompPin* TakayaFPTCompPinMap::findCompPin(const CString compPinName)
{
   TakayaFPTCompPin* compPin = NULL;
   CString key = compPinName;
   key.MakeLower();
   m_compPinMap.Lookup(key, compPin);

   return compPin;
}

POSITION TakayaFPTCompPinMap::getStartPosition() const
{ 
   return m_compPinMap.GetStartPosition(); 
}

TakayaFPTCompPin* TakayaFPTCompPinMap::getNext(POSITION& pos)     
{
   CString key;
   TakayaFPTCompPin* compPin = NULL;
   if (pos != NULL)
      m_compPinMap.GetNextAssoc(pos, key, compPin);

   return compPin;
}

void TakayaFPTCompPinMap::sort()
{
   m_compPinMap.Sort();
   m_alreadySorted = true;
}

TakayaFPTCompPin* TakayaFPTCompPinMap::getFirstSorted()
{
   TakayaFPTCompPin* compPin = NULL;
   CString* key = NULL;
   m_compPinMap.GetFirstSorted(key, compPin);

   return compPin;
}

TakayaFPTCompPin* TakayaFPTCompPinMap::getNextSorted()
{
   TakayaFPTCompPin* compPin = NULL;
   CString* key = NULL;
   m_compPinMap.GetNextSorted(key, compPin);

   return compPin;
}

//_____________________________________________________________________________
TakayaFPTComponent::TakayaFPTComponent(FileStruct& file, InsertStruct& insert)
: m_file(file)
, m_insert(insert)
, m_compPinMap(false)
{
   // Initialzie the following members
   m_name = insert.getRefname();
   m_takayaName = insert.getRefname();
   m_deviceType = deviceTypeUndefined;
   m_surface = insert.getGraphicMirrored()?surfaceBottom:surfaceTop;
   m_isIcOpenTest = false;
   m_ipOpenTestDone = false;
   m_isPinShortTestDone = false;
   m_isLoaded = true;
   m_isSmd = true;
   m_pinCount = 0;
   m_compHeightPageUnits = 0.;
   m_compHeightMM = 0.;
   m_writeSelectVisionTest = true;
   m_value = "*";
   m_comment = "*";
   m_gridLocation = "*";
}

TakayaFPTComponent::~TakayaFPTComponent()
{
   m_compPinMap.empty();
}

void TakayaFPTComponent::addCompPin(TakayaFPTCompPin* compPin)
{
   m_compPinMap.addCompPin(compPin);
}

TakayaFPTCompPin* TakayaFPTComponent::findCompPin(const CString pinName)
{
   for (POSITION pos=m_compPinMap.getStartPosition(); pos!=NULL;)
   {
      TakayaFPTCompPin* compPin = m_compPinMap.getNext(pos);
      if (compPin != NULL && compPin->getPinName().CompareNoCase(pinName) == 0)
         return compPin;
   }

   return NULL;
}

void TakayaFPTComponent::updateProperty(CCEtoODBDoc& doc, CCamCadData& camCadData, CAttributes* attributes, TakayaFPTSettings &takayaSettings)
{
   CString ppCommandAttribName( takayaSettings.getPPCommandAttribName() );
   CString commentAttribute( takayaSettings.getCommentAttribute() );
   CString icOpenDescription( takayaSettings.getIcOpenDescription() );
   bool isIcOpenDescriptionAttrib = takayaSettings.isIcOpenDescriptionAttrib();

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
         positiveTol = 25;
         negativeTol = 25;
      }
      else
      {
         positiveTol = 10;
         negativeTol = 10;
      }

      // Now get the actually positive tolerance from attribute
      if (attributes->Lookup(camCadData.getAttributeKeywordIndex(standardAttributePlusTolerance), attrib))
         positiveTol = attrib->getDoubleValue();
      else if (attributes->Lookup(camCadData.getAttributeKeywordIndex(standardAttributeTolerance), attrib))
         positiveTol = attrib->getDoubleValue();

      if (positiveTol < 1)
         positiveTol = 1;
       m_positiveTol.Format("%.0f", positiveTol);
   
      // Now get the actually negative tolerance from attribute
      if (attributes->Lookup(camCadData.getAttributeKeywordIndex(standardAttributeMinusTolerance), attrib))
         negativeTol = attrib->getDoubleValue();
      else if (attributes->Lookup(camCadData.getAttributeKeywordIndex(standardAttributeTolerance), attrib))
         negativeTol = attrib->getDoubleValue();

      if (negativeTol < 1)
         negativeTol = 1;
      m_negativeTol.Format("%.0f", negativeTol);

      if (attributes->Lookup(camCadData.getAttributeKeywordIndex(standardAttributeComponentHeight), attrib))
         m_compHeightPageUnits = attrib->getDoubleValue();
      m_compHeightMM = doc.convertPageUnitsTo(pageUnitsMilliMeters, m_compHeightPageUnits);

      camCadData.getAttributeStringValue(m_partNumber, *attributes, standardAttributePartNumber);
      camCadData.getAttributeStringValue(m_testStrategy, *attributes, standardAttributeTestStrategy);
      camCadData.getAttributeStringValue(m_subClass, *attributes, standardAttributeSubclass);
      m_aptModelName = m_subClass;

      camCadData.getAttributeStringValue(m_gridLocation, *attributes, standardAttributeGridLocation);
      if (m_gridLocation.IsEmpty())
         m_gridLocation = "*";

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
         m_value.Trim();
         double tmp = atof(m_value);

         // DR 792705 says output "0Ohm" for zero value resistors, not "*".
         // (Update, actually output "0O".)
         // This section of code is not resistor-specific, so make the conversion
         // to "*" specifically skip resistors, as the DR does not say change
         // any other kind of part.
         // Then DR 890862 comes along. This one says if value is blank make resistor
         // value "*". But when zero, leave as zero, do not turn to "*" like other parts.

         bool isResistor = (m_deviceType == deviceTypeResistor || m_deviceType == deviceTypeResistorArray);

         // If data string value is blank, for all device types, use "*".
         // If numeric value is zero and device is not resistor, use "*".
         if (m_value.IsEmpty() || (tmp == 0.0 && !isResistor))
            m_value = "*";
      }

      // Set element and modify other proper base on device type
      switch (m_deviceType)
      {
      case deviceTypeJumper:
         {
            m_element = "R";
			   m_value = "0O"; 
         }
         break;

      case deviceTypeFuse:
         {
             m_element = "R";
             m_value = "*";
         }
         break;

      case deviceTypeResistor:
      case deviceTypeResistorArray:
         {
            m_element = "R";

            // DR 890862 says stop doing this. Blank values go to "*" and stay "*". Actual zero stays zero.
            //if (m_value == "*")
            //   m_value = "0O"; // zero oh
            
            if (m_value != "*")
            {
               m_value.Replace("Ohms", "O"); // value has "Ohms" as suffix, then replace with "O"
               m_value.Replace("Ohm", "O");  // value has "Ohm" as suffix, then replace with "O"
               if (m_value.Right(1) != "O")    // value does not end in "O", then attach "O"
                  m_value += "O";
            }
            if (m_deviceType == deviceTypeResistor && m_mergedStatus.CompareNoCase("primary") == 0)
               m_takayaName.AppendFormat("*");
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
               m_value.Replace("FARADS", "F"); // value has "FARADS" as suffix, then replace with "F"
               m_value.Replace("FARAD", "F");  // value has "FARAD" as suffix, then replace with "F"
               if (m_value.Right(1) != "F")    // value does not end in "F", then attach "F"
                  m_value += "F";
            }
            if (m_deviceType != deviceTypeCapacitorArray && m_mergedStatus.CompareNoCase("primary") == 0)
               m_takayaName.AppendFormat("*");           
         }
         break;

      case deviceTypeInductor:      
         {
            m_element = "L";

            if (m_value != "*")
            {
               m_value.Replace("HENRY", "H");     // value has "HENRY" as suffix, then replace with "H"
               m_value.Replace("HERRIES", "H");   // value has "HERRIES" as suffix, then replace with "H"
               if (m_value.Right(1) != "H")       // value does not end with "H", then attach "H"
                  m_value += "H";
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
            // This will be set later at TakayaFPTComponentCollection::generateIcTest()
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

int TakayaFPTComponent::generateTestPinCount(FILE *logFp, const int maxHits, const TakayaFPTSurface testSurface)
{
   int unUsedPin = 0;
   int testAccessCount = 0; 

   m_compPinMap.sort();

   for (POSITION pos=m_compPinMap.getStartPosition(); pos!=NULL;)
   {
      TakayaFPTCompPin* compPin = m_compPinMap.getNext(pos);
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
TakayaFPTComponentCollection::TakayaFPTComponentCollection(CGTabTableList *tableList)
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

TakayaFPTComponentCollection::~TakayaFPTComponentCollection()
{
   empty();
}

void TakayaFPTComponentCollection::empty()
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

void TakayaFPTComponentCollection::applyTakayaOrigin(CPoint2d &location, bool topside)
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

void TakayaFPTComponentCollection::convertToTakayaUnits(CPoint2d& location, double unitFactor, bool topside)
{
   applyTakayaOrigin(location, topside);

   ::convertToTakayaUnits(location.x, unitFactor);
   ::convertToTakayaUnits(location.y, unitFactor);
}

void TakayaFPTComponentCollection::moveIcDeviceTypeToOtherComponent()
{
   while (m_icComponents.GetHeadPosition() != NULL)
   {
      TakayaFPTComponent* component = m_icComponents.RemoveHead();
      component->unsetDeviceType();
      m_otherComponents.AddTail(component);
   }
}

TakayaFPTComponent* TakayaFPTComponentCollection::addComponent(CCEtoODBDoc& doc, CCamCadData& camCadData, FileStruct& file, DataStruct& data, TakayaFPTSettings& settings, CTMatrix transformMatrix)
{
   InsertStruct* insert = data.getInsert();
   if (insert == NULL)
      return NULL;

   TakayaFPTComponent* component = findComponent(insert->getRefname());
   if (!insert->getRefname().IsEmpty() && component == NULL)
   {
      // Get origin at board level
      CPoint2d origin = insert->getOrigin2d();
      transformMatrix.transform(origin);
      convertToTakayaUnits(origin, settings.getUnitFactor(), insert->getPlacedTop());

      component = new TakayaFPTComponent(file, *insert);
      component->setInsertOrigin(origin);
      component->updateProperty(doc, camCadData, data.getAttributes(), settings);

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

TakayaFPTComponent* TakayaFPTComponentCollection::findComponent(const CString name)
{
   TakayaFPTComponent* component = NULL;
   CString key = name;
   key.MakeLower();
   m_componentMapByName.Lookup(name, component);
   return component;
}

void TakayaFPTComponentCollection::generateComponentTest(CCamCadData& camcadData, FILE *logFp, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
                                                         TakayaFPTNet* groundNet, TakayaIcDiodeCommandMap& icDiodeCommandMap, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
{
   m_visionTestCount = 0;

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

void TakayaFPTComponentCollection::generateResistorTest(FILE *logFp, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
{
   for (POSITION pos=m_resistorComponents.GetHeadPosition(); pos!=NULL;)
   {
      TakayaFPTComponent* component = m_resistorComponents.GetNext(pos);
      if (component != NULL)
      {
         // Output Kelvin test if component qualifies, otherwise output generic resistor test.
         if (!generateKelvinTest(logFp, *component, probeOutputCollection, outputArray, settings, testSurface))
            generateGenericTest(logFp, *component, adptModelMap, probeOutputCollection, outputArray, settings, testSurface);
      }
   }
}

void TakayaFPTComponentCollection::generateCapacitorTest(FILE *logFp, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
{
   for (POSITION pos=m_capacitorComponents.GetHeadPosition(); pos!=NULL;)
   {
      TakayaFPTComponent* component = m_capacitorComponents.GetNext(pos);
      if (component != NULL)
      { 
         generateGenericTest(logFp, *component, adptModelMap, probeOutputCollection, outputArray, settings, testSurface);
      }
   }
}

void TakayaFPTComponentCollection::generateDiodeTest(FILE *logFp, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
{
   for (POSITION pos=m_diodeComponents.GetHeadPosition(); pos!=NULL;)
   {
      TakayaFPTComponent* component = m_diodeComponents.GetNext(pos);
      if (component != NULL)
      {
         int testPinCount = component->generateTestPinCount(logFp, settings.getMaxHitCount(), testSurface);
         if (testPinCount > 0)
         {
            TakayaFPTAptModel* aptModel = adptModelMap.lookup(component->getAptModelName());
            if (aptModel != NULL)
            {
               // Generate test using aptModel
               aptModel->generateAptModelTest(logFp, *component, probeOutputCollection, outputArray, settings, testSurface);
            }
            else
            {
               TakayaFPTCompPin* anodePin = NULL;
               TakayaFPTCompPin* cathodePin = NULL;
         
               TakayaFPTCompPin* compPin = component->getFirstSorted();
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

            if(aptModel == NULL && !component->getAptModelName().IsEmpty())
               fprintf(logFp, "Subclass %s of component %s not found in Takaya model library\n", component->getAptModelName(), component->getName());            
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

void TakayaFPTComponentCollection::generateInductorTest(FILE *logFp, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
{
   for (POSITION pos=m_inductorComponents.GetHeadPosition(); pos!=NULL;)
   {
      TakayaFPTComponent* component = m_inductorComponents.GetNext(pos);
      if (component != NULL)
      { 
         generateGenericTest(logFp, *component, adptModelMap, probeOutputCollection, outputArray, settings, testSurface);
      }
   }
}

void TakayaFPTComponentCollection::generateIcTest(CCamCadData& camcadData, FILE *logFp, TakayaFPTAptModelMap& adptModelMap, 
                                                  TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
                                                  TakayaFPTNet& groundNet, TakayaIcDiodeCommandMap& icDiodeCommandMap, 
                                                  const bool isIcDiodeTest, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
{
   for (POSITION pos=m_icComponents.GetHeadPosition(); pos!=NULL;)
   {
      TakayaFPTComponent* component = m_icComponents.GetNext(pos);
      if (component != NULL)
      { 
         bool isIcOpensTest = component->isIcOpenTest();
         TakayaFPTAptModel* aptModel = NULL;
         CSubclass *sc = NULL;

         if (isIcOpensTest)
         {
            generateIcOpenTest(logFp, camcadData, groundNet, *component, probeOutputCollection, outputArray, settings, testSurface);
         }
         else
         {
            //NOT USED:  int testPinCount = component->generateTestPinCount(settings.getMaxHitCount(), testSurface);          
            aptModel = adptModelMap.lookup(component->getAptModelName());
            sc =  m_subclassList.findSubclass(component->getAptModelName());
            if (aptModel != NULL)
            {
               // Generate test using aptModel
               aptModel->generateAptModelTest(logFp, *component, probeOutputCollection, outputArray, settings, testSurface);
            }
            else if(sc != NULL && isValidDeviceTypeForMDBUse(component->getDeviceType()))            
            {
               generateOutputUsingMDB(logFp, *component, probeOutputCollection, outputArray, settings, testSurface, sc);            
            }

            if(aptModel == NULL && !component->getAptModelName().IsEmpty())
               fprintf(logFp, "Subclass %s of component %s not found in Takaya model library\n", component->getAptModelName(), component->getName());
            if(sc == NULL && isValidDeviceTypeForMDBUse(component->getDeviceType()))
               fprintf(logFp, "Subclass %s of component %s not found in Data Doctor element library\n", component->getAptModelName(), component->getName());
         }

         if ((aptModel == NULL) && (sc == NULL) && // apt model test was not used, AND subclass is not found in .mdb AND
             ((!isIcOpensTest) ||  // didn't do ic opens so go ahead with this stuff OR
              (isIcDiodeTest && settings.getAllowIcDiodeWithIcOpen())  // is diode test and we want it even if opens test was done
             )
            )
         {
            TakayaFPTCompPin* groundPin = NULL;
            CString element, prefix;

            if (isIcDiodeTest)
            {
               TakayaIcDiodeCommand* icDiodeCommand = icDiodeCommandMap.getGroundPin(component->getPartNumber());
               if (icDiodeCommand != NULL)
                  groundPin = component->findCompPin(icDiodeCommand->getPinName());

               element = "D";
               prefix = settings.getIcDiodePrefix();
            }
            else
            {
               element = "C";
               prefix = settings.getIcCapacitorPrefix();
            }

            TakayaFPTCompPin* compPin = component->getFirstSorted();
            while (compPin != NULL)
            {
               if (compPin->getTestedSurface() == surfaceUnknown || compPin->getTestedSurface() == testSurface)
               {
                  TakayaFPTTestAccess* testAccess = compPin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
                  if (testAccess != NULL)
                  {
                     // If Test Acces Point of comppin is already use for another Ic Capacitor test, then don't output a test for it.
                     // Unless the comppin is already test on the same surface, which mean that the same comppin was used as 
                     // a Ic Capacitor test and is now used again for Ic Diode test on the same surface.
                     bool testAccessUsedForIcTest = isIcDiodeTest?testAccess->getUsedForIcDiodeTest():testAccess->getUsedForIcCapacitorTest();

                     if (!testAccess->getIsOnIcGroundNet() && !testAccessUsedForIcTest)
                     {
                        // Get Ground Test Access Point
                        TakayaFPTTestAccess* groundTestAccess = NULL;
                        if (isIcDiodeTest && groundPin != NULL)
                           groundTestAccess = groundPin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);

                        if (groundTestAccess == NULL)
                           groundTestAccess = groundNet.getTestAccess(logFp, settings.getMaxHitCount(), testSurface);

                        if (groundTestAccess != NULL)
                        {
                           CString probeNumber1 = probeOutputCollection.getProbeNumberString(*testAccess);
                           CString probeNumber2 = probeOutputCollection.getProbeNumberString(*groundTestAccess);

                           CString output;
                           int positiveTol,negativeTol;
                           if(element.Compare("D"))
                           {
                              positiveTol = 25;
                              negativeTol = 25;
                           }
                           else
                           {
                              positiveTol = defaultICDiodeTolerance;
                              negativeTol = defaultICDiodeTolerance;
                           }
                           output.Format("%s %s%s %s-GP %s * %s %s %s @T %d %d", component->getCommentOperator(), prefix, component->getTakayaName(),
                              compPin->getPinName(), component->getComment(), element, probeNumber1, probeNumber2,positiveTol,negativeTol);
                           outputArray.Add(output);

                           isIcDiodeTest?testAccess->setUsedForIcDiodeTest(true):testAccess->setUsedForIcCapacitorTest(true);                              
                           compPin->setTestedSurface(testSurface);
                           compPin->setAlreadyTested(true);
                        }
                        else if (groundTestAccess == NULL)
                        {
                           fprintf(logFp, "Error:  No more available \"Ground Pin\" found to test IC [%s]\n", component->getName());
                           break;
                        }
                     }

                     
                  }
               }

               compPin = component->getNextSorted();
            }
         }

         //else
         //{
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

void TakayaFPTComponentCollection::generateTransistorTest(FILE *logFp, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
{
   for (POSITION pos=m_transitorComponents.GetHeadPosition(); pos!=NULL;)
   {
      TakayaFPTComponent* component = m_transitorComponents.GetNext(pos);
      if (component != NULL)
      {
         int testPinCount = component->generateTestPinCount(logFp, settings.getMaxHitCount(), testSurface);
         if (testPinCount > 0)
         {
            TakayaFPTAptModel* aptModel = adptModelMap.lookup(component->getAptModelName());
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
               TakayaFPTCompPin* anodePin = NULL;
               TakayaFPTCompPin* cathodePin = NULL;
               TakayaFPTCompPin* basePin = NULL;
               TakayaFPTCompPin* collectorPin = NULL;
               TakayaFPTCompPin* emitterPin = NULL;
               TakayaFPTCompPin* gatePin = NULL;
               TakayaFPTCompPin* sourcePin = NULL;
               TakayaFPTCompPin* drainPin = NULL;

               CString anodeProbeNum, cathodeProbeNum, baseProbeNum, collectorProbeNum, emitterProbeNum, gateProbeNum, sourceProbeNum, drainProbeNum;

               TakayaFPTCompPin* compPin = component->getFirstSorted();
               while (compPin != NULL)
               {
                  TakayaPinMapType pinMapType = pinMapTypeStringToTag(compPin->getPinMap());
                  TakayaFPTTestAccess* testAccess = compPin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
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
                  if (anodePin!=NULL && cathodePin!=NULL && collectorPin!=NULL && emitterPin!=NULL)
                  {
                     CString output;
                     CString partNumber = component->getPartNumber().IsEmpty()?"*":component->getPartNumber();

                     output.Format("%s@PC %s %s * %s %s %s %s", component->getCommentOperator(), component->getTakayaName(), partNumber,
                        anodeProbeNum, cathodeProbeNum, emitterProbeNum, collectorProbeNum);
                     outputArray.Add(output);

                     output.Format("%s@CE", component->getCommentOperator());
                     outputArray.Add(output);

                     output.Format("%s%s C-E %s * R %s %s @K OP @K JP", component->getCommentOperator(), component->getTakayaName(), partNumber,
                        collectorProbeNum, emitterProbeNum);
                     outputArray.Add(output);

                     output.Format("%s%s A-K %s * D %s %s @T 25 25 @K JP", component->getCommentOperator(), component->getTakayaName(), partNumber,
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
                     component->SetWriteSelectVisionTest(false);
                  }
               }
               else
               {

                  CString output, comment;
                  if (component->getDeviceType() == deviceTypeTransistorPnp)
                  {
                     comment = component->getComment()=="*"?"PNP":component->getComment();
                     output.Format("%s@Q2 %s %s * @T 10 10", component->getCommentOperator(), component->getTakayaName(), comment);
                  }
                  else if (component->getDeviceType() == deviceTypeTransistor || component->getDeviceType() == deviceTypeTransistorNpn)
                  {
                     comment = component->getComment()=="*"?"NPN":component->getComment();
                     output.Format("%s@Q2 %s %s * @T 10 10", component->getCommentOperator(), component->getTakayaName(), comment);
                  }
                  else if (component->getDeviceType() == deviceTypeTransistorFetNpn || component->getDeviceType() == deviceTypeTransistorMosfetNpn)
                  {
                     comment = component->getComment()=="*"?"FET_N":component->getComment();
                     output.Format("%s@F %s %s * @MM FETEN", component->getCommentOperator(), component->getTakayaName(), comment);
                  }
                  else if (component->getDeviceType() == deviceTypeTransistorFetPnp || component->getDeviceType() == deviceTypeTransistorMosfetPnp)
                  {
                     comment = component->getComment()=="*"?"FET_P":component->getComment();
                     output.Format("%s@F %s %s * @MM FETDP", component->getCommentOperator(), component->getTakayaName(), comment);
                  }

                  if (basePin != NULL && collectorPin != NULL && emitterPin != NULL)
                  {
                     outputArray.Add(output);               

                     output.Format("%s %s", component->getCommentOperator(), baseProbeNum);
                     outputArray.Add(output);

                     output.Format("%s %s", component->getCommentOperator(), collectorProbeNum);
                     outputArray.Add(output);

                     output.Format("%s %s", component->getCommentOperator(), emitterProbeNum);
                     outputArray.Add(output);

                     output.Format("%s@CE", component->getCommentOperator());
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
                     outputArray.Add(output);

                     output.Format("%s %s", component->getCommentOperator(), gateProbeNum);
                     outputArray.Add(output);

                     output.Format("%s %s", component->getCommentOperator(), drainProbeNum);
                     outputArray.Add(output);

                     output.Format("%s %s", component->getCommentOperator(), sourceProbeNum);
                     outputArray.Add(output);

                     output.Format("%s@CE", component->getCommentOperator());
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

void TakayaFPTComponentCollection::generateCapacitorPolarizedTest(CCamCadData& camcadData, FILE *logFp, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTNet* groundNet, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
{
   for (POSITION pos=m_capacitorPolarizedComponents.GetHeadPosition(); pos!=NULL;)
   {
      TakayaFPTComponent* component = m_capacitorPolarizedComponents.GetNext(pos);
      if (component != NULL)
      { 
         if (component->isIcOpenTest() && groundNet != NULL)
            generateIcOpenTest(logFp, camcadData, *groundNet, *component, probeOutputCollection, outputArray, settings, testSurface);
         else
            generateGenericTest(logFp, *component, adptModelMap, probeOutputCollection, outputArray, settings, testSurface);
      }
   }
}

void TakayaFPTComponentCollection::generateCapacitorTantalumTest(CCamCadData& camcadData, FILE *logFp, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTNet* groundNet, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
{
   for (POSITION pos=m_capacitorTantalumComponents.GetHeadPosition(); pos!=NULL;)
   {
      TakayaFPTComponent* component = m_capacitorTantalumComponents.GetNext(pos);
      if (component != NULL)
      { 
         if (component->isIcOpenTest() && groundNet != NULL)
            generateIcOpenTest(logFp, camcadData, *groundNet, *component, probeOutputCollection, outputArray, settings, testSurface);
         else
            generateGenericTest(logFp, *component, adptModelMap, probeOutputCollection, outputArray, settings, testSurface);
      }
   }
}

void TakayaFPTComponentCollection::generateIcOpenTest(FILE *logFp, CCamCadData& camcadData, 
                                                      TakayaFPTNet& groundNet, TakayaFPTComponent& component, 
                                                      TakayaFPTProbeOutputCollection& probeOutputCollection, 
                                                      CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
{
   // Get Pin Location that is sensorDiameter from DFT (or RealPart package) outline

   if (component.getSurface() != testSurface || component.getIcOpenTestDone())
      return;

   // Get the pcb component geometry.
	BlockStruct* geomBlock = camcadData.getBlockAt(component.getInsert().getBlockNumber());

   // Get the RealPart Package block. This may or may not exist, is okay if it does not.
   BlockStruct *realPartPkgBlock = GetRealPartPackageBlockForComponent(camcadData, &component);

   double sensorDia = settings.getCapOpensSensorDiameter(camcadData.getPageUnits());

	ICOpenTestGeom* icOpenTestGeom = m_icOpenTestGeomMap.GetICOpenTestGeom(camcadData, geomBlock, realPartPkgBlock, sensorDia);
	if (icOpenTestGeom != NULL)
   {
      if (icOpenTestGeom->HasShrinkError())
      {
         fprintf(logFp, "Sensor diameter too large for \"%s\", using component centroid.\n",
            component.getName());
      }

	   CPinLocationMap* pinLocationMap = icOpenTestGeom->CreatePinLocationMapForInsert(&component.getInsert(), testSurface);
	   if (pinLocationMap != NULL)
      {
         int testPinCount = component.generateTestPinCount(logFp, settings.getMaxHitCount(), testSurface);

         if (testPinCount > 0)
         {
            TakayaFPTTestAccess* groundTestAccess1 = NULL;
            TakayaFPTTestAccess* groundTestAccess2 = NULL;
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

               CString output;
               output.Format("@A G1:%s G2:%s", groundProbe1, groundProbe2);
               outputArray.Add(output);

               // DR 777288 says put a threshold on using individual pin locations for this test. If
               // component body is less than 8mm then output component centroid as pin location.
               // We originally took that to mean if both X and Y dims are less than 8mm.
               // That was rejected with note "if either are less than 8mm then use centroid". So be it.
               double thresholdInPageUnits = camcadData.convertToPageUnits(pageUnitsMilliMeters, 8.0);
               CExtent *packageExtent = m_packageOutlineExtentMap.getPackageOutlineExtent(camcadData, &component);
               double pkgSizeX = packageExtent?packageExtent->getXsize():0.;
               double pkgSizeY = packageExtent?packageExtent->getYsize():0.;
               bool useCompCentroidForPinLoc = (pkgSizeX < thresholdInPageUnits) || (pkgSizeY < thresholdInPageUnits);

               // If the shrink operation failed then use the comp centroid. We already logged a message above,
               // here just switch modes if needed.
               if (icOpenTestGeom->HasShrinkError())
                  useCompCentroidForPinLoc = true;

               TakayaFPTCompPin* compPin = component.getFirstSorted();
               while (compPin != NULL)
               {
                  TakayaFPTTestAccess* testAccess = compPin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
                  if (testAccess != NULL)
                  {

		               CPoint2d pinLoc;
		               CPoint2d* tmpPinLoc = NULL;

                     // If comp extent was big enough then try for individual pin locations from map.
                     if (!useCompCentroidForPinLoc)
                     {
                        pinLocationMap->Lookup(compPin->getPinName(), tmpPinLoc);
                        if (tmpPinLoc != NULL)
                           pinLoc = *tmpPinLoc;
                     }

                     // If not found in map, or comp size is forcing use of centroid, look for centroid.
                     // If centroid isn't there either then use component insert location.
                     if (tmpPinLoc == NULL || useCompCentroidForPinLoc)
                     {
                        if (!component.getInsert().getCentroidLocation(camcadData, pinLoc))
                           pinLoc = component.getInsert().getOrigin2d();

                        // DR 847843 - Need to mirror X for bottom side.
                        if (component.getSurface() == surfaceBottom)
                           pinLoc.x = -pinLoc.x;
                     }

                     convertToTakayaUnits(pinLoc, settings.getUnitFactor(), component.getSurface() == surfaceTop);

                     CString probeNumber = probeOutputCollection.getProbeNumberString(*testAccess);

						   if (probeNumber.CompareNoCase(groundProbe1) != 0 && probeNumber.CompareNoCase(groundProbe2) != 0 )
						   {
                        CString value = compPin->getPinName() + "Pin";

							   // Only output test if the probe number is not the same as the one in @A G1:N G2:N
                        output.Format("%s %s %s %s %s %s %.0f %.0f @PL H 0 0 1 0 @T 100 30", component.getCommentOperator(), component.getTakayaName(), 
                              value, component.getIcOpenDescription(), component.getLocation(), probeNumber, pinLoc.x, pinLoc.y);
                        outputArray.Add(output);
						   }
                  }

                  compPin = component.getNextSorted();
               }

               output.Format("@CE");
               outputArray.Add(output);

               component.setIcOpenTestDone(true);
            }
         }
      }
   }
}

void TakayaFPTComponentCollection::generateUnsupportedDeviceTypeTest(FILE *logFp, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
{
   for (POSITION pos=m_otherComponents.GetHeadPosition(); pos!=NULL;)
   {
      TakayaFPTComponent* component = m_otherComponents.GetNext(pos);
      if (component != NULL)
      {
         TakayaFPTAptModel* aptModel = adptModelMap.lookup(component->getAptModelName());
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

bool TakayaFPTComponentCollection::generateKelvinTest(FILE *logFp, TakayaFPTComponent& component, TakayaFPTProbeOutputCollection& probeOutputCollection,
                                                      CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
{
   // Only do Kelvin test if resistor value is non-zero and is less than Kelvin threshold.

   double kelvinThreshold = settings.getKelvinThreshold();
   CString compValueStr( component.getValue() );
   ComponentValues compValueManager(compValueStr);
   double compValue = compValueManager.GetValue();

   if (compValue > 0. && compValue < kelvinThreshold)
   {
      int testPinCount = component.generateTestPinCount(logFp, settings.getMaxHitCount(), testSurface);

      if (testPinCount == 2)
      {
         TakayaFPTCompPin* compPin1 = component.getFirstSorted();
         TakayaFPTCompPin* compPin2 = component.getNextSorted();

         if (compPin1 != NULL && compPin2 != NULL)
         {
            // Abort mission if not the correct surface.
            if (compPin1->getTestedSurface() != surfaceUnknown && compPin1->getTestedSurface() != testSurface &&
                compPin2->getTestedSurface() != surfaceUnknown && compPin2->getTestedSurface() != testSurface )
                return false;

            // Need four probes for Kelvin test, two for each pin. Try to get them.
            TakayaFPTNet& net1 = compPin1->getNet();
            TakayaFPTNet& net2 = compPin2->getNet();

            TakayaFPTTestAccess* testAccess1A = NULL;
            TakayaFPTTestAccess* testAccess1B = NULL;
            TakayaFPTTestAccess* testAccess2A = NULL;
            TakayaFPTTestAccess* testAccess2B = NULL;

            net1.getTwoTestAccess(logFp, settings.getMaxHitCount(), testSurface, &testAccess1A, &testAccess1B);
            // Save time, need four probes or its no go, so get two probes for net2 only if net1 had two probes.
            if (testAccess1A != NULL && testAccess1B != NULL)
               net2.getTwoTestAccess(logFp, settings.getMaxHitCount(), testSurface, &testAccess2A, &testAccess2B);

            // Only need to check 2A and 2B here, as they will not be set unless 1A and 1B were both set first.
            if (testAccess2A != NULL && testAccess2B != NULL)
            {
               // Got it all, output the Kelvin test.

               CString probe1A = probeOutputCollection.getProbeNumberString(*testAccess1A);
               CString probe1B = probeOutputCollection.getProbeNumberString(*testAccess1B);
               CString probe2A = probeOutputCollection.getProbeNumberString(*testAccess2A);
               CString probe2B = probeOutputCollection.getProbeNumberString(*testAccess2B);

               CString option;
               option.Format("@T %s %s", component.getPositiveTol(), component.getNegativeTol());

               CString output;
               output.Format("%s @KL %s %s %s %s %s %s %s %s %s", component.getCommentOperator(), component.getTakayaName(), component.getValue(),
                  component.getComment(), component.getLocation(), probe1A, probe1B, probe2A, probe2B, option);

               outputArray.Add(output);

               // Set comppin to being tested
               compPin1->setTestedSurface(testSurface);
               compPin1->setAlreadyTested(true);
               compPin2->setTestedSurface(testSurface);
               compPin2->setAlreadyTested(true);


               return true; // Kelvin test has been output.
            }
         }

         //if (generateOutput(logFp, component, compPin1, compPin2, probeOutputCollection, outputArray, settings, testSurface))
         //   component.SetWriteSelectVisionTest(false);
      }
   }

   return false; // Did not do a Kelvin test
}


void TakayaFPTComponentCollection::generateGenericTest(FILE *logFp, TakayaFPTComponent& component, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection,
      CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
{
   int testPinCount = component.generateTestPinCount(logFp, settings.getMaxHitCount(), testSurface);

   if (testPinCount > 0)
   {
      TakayaFPTAptModel* aptModel = adptModelMap.lookup(component.getAptModelName());
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
            TakayaFPTCompPin* compPin1 = component.getFirstSorted();
            TakayaFPTCompPin* compPin2 = component.getNextSorted();

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

bool TakayaFPTComponentCollection::generateOutput(FILE *logFp, TakayaFPTComponent& component, TakayaFPTCompPin* compPin1, TakayaFPTCompPin* compPin2, TakayaFPTProbeOutputCollection& probeOutputCollection,
      CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
{
   if (compPin1 == NULL || compPin2 == NULL)
      return false;

   if (compPin1->getTestedSurface() != surfaceUnknown && compPin1->getTestedSurface() != testSurface &&
       compPin2->getTestedSurface() != surfaceUnknown && compPin2->getTestedSurface() != testSurface )
       return false;

   bool retval = false;
   TakayaFPTTestAccess* testAccess1 = compPin1->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
   TakayaFPTTestAccess* testAccess2 = compPin2->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);

   if (testAccess1 != NULL && testAccess2 != NULL && testAccess1 != testAccess2)
   {
      CString probeNumber1 = probeOutputCollection.getProbeNumberString(*testAccess1);
      CString probeNumber2 = probeOutputCollection.getProbeNumberString(*testAccess2);

      CString option;

      // 13 Apr 2012 No DR, by phone with Mark, the 792705 fix is changed to now only apply to resistors and jumpers.
      // Was catching diodes, most likely because of "*" value, but that was not verified.

      DeviceTypeTag devType = component.getDeviceType();
      bool isSpecialZeroValuePart =  (devType == deviceTypeJumper) || (devType == deviceTypeResistor) || (devType == deviceTypeResistorArray);

      CString compValue( component.getValue() );
      // DR 890862 says no longer output @K E @KR stuff for "*" value resistors.
      bool isSpecialZeroValueValue = (compValue.Compare("0O") == 0) || (compValue.Compare("0Ohm") == 0);

      if (isSpecialZeroValuePart && isSpecialZeroValueValue)
      {
         // DR 792705 - Zero value resistor gets this, not sure what it means.
         option.Format("@K E @MR 3.0O");
      }
      else
      {
         // Non-zero value resistors get Tolerance, as was  the case for all components before DR 792705
         option.Format("@T %s %s", component.getPositiveTol(), component.getNegativeTol());
      }


      if(devType == deviceTypeDiodeZener && option.Find("@MM") < 0)
          option.AppendFormat(" @MM DCZD");

      CString output;
      output.Format("%s %s %s %s %s %s %s %s %s", component.getCommentOperator(), component.getTakayaName(), component.getValue(),
         component.getComment(), component.getLocation(), component.getElement(), probeNumber1, probeNumber2, option);

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

bool TakayaFPTComponentCollection::isValidDeviceTypeForMDBUse(const DeviceTypeTag deviceType)
{
   return (deviceType == deviceTypeCapacitorArray
      || deviceType == deviceTypeDiodeArray
      || deviceType == deviceTypeFilter
      || deviceType == deviceTypeICLinear
      || deviceType == deviceTypeResistorArray            
      || deviceType == deviceTypeTransistorArray);
}

bool TakayaFPTComponentCollection::isValidTwoPinDeviceType(const DeviceTypeTag deviceType)
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

bool TakayaFPTComponentCollection::isValidFourPinDeviceType(const DeviceTypeTag deviceType)
{
   return (deviceType == deviceTypeOpto);
}

bool TakayaFPTComponentCollection::isValidThreePinDeviceType(const DeviceTypeTag deviceType)
{
   return (deviceType == deviceTypeTransistor
      || deviceType == deviceTypeTransistorFetNpn
      || deviceType == deviceTypeTransistorFetPnp
      || deviceType == deviceTypeTransistorMosfetNpn
      || deviceType == deviceTypeTransistorMosfetPnp
      || deviceType == deviceTypeTransistorNpn
      || deviceType == deviceTypeTransistorPnp);
}

void TakayaFPTComponentCollection::generateOutputUsingMDB(FILE *logFp, TakayaFPTComponent& component,
     TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
     TakayaFPTSettings& settings, const TakayaFPTSurface testSurface, CSubclass *sc)
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

void TakayaFPTComponentCollection::generateFourPinOutputUsingMDB(FILE *logFp, TakayaFPTComponent& component,
     TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
     TakayaFPTSettings& settings, const TakayaFPTSurface testSurface, CSubclassElement *el)
{ 
   TakayaFPTCompPin* anodePin = NULL;
   TakayaFPTCompPin* cathodePin = NULL;
   TakayaFPTCompPin* collectorPin = NULL;
   TakayaFPTCompPin* emitterPin = NULL;

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

      TakayaFPTTestAccess* anodePinTestAccess = anodePin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
      TakayaFPTTestAccess* cathodePinTestAccess = cathodePin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
      TakayaFPTTestAccess* collectorPinTestAccess = collectorPin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
      TakayaFPTTestAccess* emitterPinTestAccess = emitterPin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);

      CString anodeProbeNum( probeOutputCollection.getProbeNumberString(anodePinTestAccess) );
      CString cathodeProbeNum( probeOutputCollection.getProbeNumberString(cathodePinTestAccess) );
      CString collectorProbeNum( probeOutputCollection.getProbeNumberString(collectorPinTestAccess) );
      CString emitterProbeNum( probeOutputCollection.getProbeNumberString(emitterPinTestAccess) );
         
      CString output;
      CString partNumber = component.getPartNumber().IsEmpty()?"*":component.getPartNumber();

      output.Format("%s@PC %s %s * %s %s %s %s", component.getCommentOperator(), component.getTakayaName(), partNumber,
         anodeProbeNum, cathodeProbeNum, collectorProbeNum, emitterProbeNum);
      outputArray.Add(output);

      output.Format("%s@CE", component.getCommentOperator());
      outputArray.Add(output);

      output.Format("%s%s C-E %s * R %s %s @K OP @K JP", component.getCommentOperator(), component.getTakayaName(), partNumber,
         collectorProbeNum, emitterProbeNum);
      outputArray.Add(output);

      output.Format("%s%s A-K %s * D %s %s @T 25 25 @K JP", component.getCommentOperator(), component.getTakayaName(), partNumber,
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

void TakayaFPTComponentCollection::generateThreePinOutputUsingMDB(FILE *logFp, TakayaFPTComponent& component,
     TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
     TakayaFPTSettings& settings, const TakayaFPTSurface testSurface, CSubclassElement *el)
{
   if(el->getDeviceType() == deviceTypeTransistor 
      || el->getDeviceType() == deviceTypeTransistor
      || el->getDeviceType() == deviceTypeTransistor)
   {
      //For deviceTypes deviceTypeTransistor, deviceTypeTransistorNpn and deviceTypeTransistorPnp
      TakayaFPTCompPin* basePin = NULL;
      TakayaFPTCompPin* emitterPin = NULL;
      TakayaFPTCompPin* collectorPin = NULL;
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
         
         TakayaFPTTestAccess* basePinTestAccess = basePin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
         TakayaFPTTestAccess* emitterPinTestAccess = emitterPin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
         TakayaFPTTestAccess* collectorPinTestAccess = collectorPin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);

         if(basePinTestAccess != NULL && emitterPinTestAccess != NULL && collectorPinTestAccess != NULL)
         {
            baseProbeNum = probeOutputCollection.getProbeNumberString(*basePinTestAccess);
            emitterProbeNum = probeOutputCollection.getProbeNumberString(*emitterPinTestAccess);
            collectorProbeNum = probeOutputCollection.getProbeNumberString(*collectorPinTestAccess);

            CString output, comment;
            if (component.getDeviceType() == deviceTypeTransistorPnp)
            {
               comment = component.getComment()=="*"?"PNP":component.getComment();
               output.Format("%s@Q2 %s %s * @T 10 10", component.getCommentOperator(), component.getTakayaName(), comment);
            }
            else if (component.getDeviceType() == deviceTypeTransistor || component.getDeviceType() == deviceTypeTransistorNpn)
            {
               comment = component.getComment()=="*"?"NPN":component.getComment();
               output.Format("%s@Q2 %s %s * @T 10 10", component.getCommentOperator(), component.getTakayaName(), comment);
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
      TakayaFPTCompPin* drainPin = NULL;
      TakayaFPTCompPin* gatePin = NULL;
      TakayaFPTCompPin* sourcePin = NULL;
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
         
         TakayaFPTTestAccess* drainPinTestAccess = drainPin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
         TakayaFPTTestAccess* gatePinTestAccess = gatePin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
         TakayaFPTTestAccess* sourcePinTestAccess = sourcePin->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);

         if(drainPinTestAccess != NULL && gatePinTestAccess != NULL && sourcePinTestAccess != NULL)
         {
            drainProbeNum = probeOutputCollection.getProbeNumberString(*drainPinTestAccess);
            gateProbeNum = probeOutputCollection.getProbeNumberString(*gatePinTestAccess);
            sourceProbeNum = probeOutputCollection.getProbeNumberString(*sourcePinTestAccess);
            CString output, comment;

            if (component.getDeviceType() == deviceTypeTransistorFetNpn || component.getDeviceType() == deviceTypeTransistorMosfetNpn)
            {
               comment = component.getComment()=="*"?"FET_N":component.getComment();
               output.Format("%s@F %s %s * @MM FETEN", component.getCommentOperator(), component.getTakayaName(), comment);
            }
            else if (component.getDeviceType() == deviceTypeTransistorFetPnp || component.getDeviceType() == deviceTypeTransistorMosfetPnp)
            {
               comment = component.getComment()=="*"?"FET_P":component.getComment();
               output.Format("%s@F %s %s * @MM FETDP", component.getCommentOperator(), component.getTakayaName(), comment);
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

void TakayaFPTComponentCollection::generateTwoPinOutputUsingMDB(FILE *logFp, TakayaFPTComponent& component,
     TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
     TakayaFPTSettings& settings, const TakayaFPTSurface testSurface, CSubclassElement *el)
{
   CString value = component.getValue();
   CString ntol = component.getNegativeTol();
   CString ptol = component.getPositiveTol();
   CString refName = component.getName();
   TakayaFPTCompPin* compPin1 = component.findCompPin(el->getPin1Name());
   TakayaFPTCompPin* compPin2 = component.findCompPin(el->getPin2Name());
   if (compPin1 != NULL && compPin2 != NULL)
   {
      if (compPin1->getTestedSurface() != surfaceUnknown && compPin1->getTestedSurface() != testSurface &&
         compPin2->getTestedSurface() != surfaceUnknown && compPin2->getTestedSurface() != testSurface )
         return;

      TakayaFPTTestAccess* testAccess1 = compPin1->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
      TakayaFPTTestAccess* testAccess2 = compPin2->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);

      if (testAccess1 != NULL && testAccess2 != NULL && testAccess1 != testAccess2)
      {
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
            break;
         case deviceTypeDiode:
         case deviceTypeDiodeLed:
         case deviceTypeDiodeZener:
            element = "D";
            break;
         case deviceTypeFuse:
         case deviceTypeResistor:
         case deviceTypeJumper:
            element = "R";
            break;
         case deviceTypeInductor:
            element = "L";
            break;
         }


         output.Format("%s %s %s %s-%s/%s-%s * %s %s %s @T %s %s", component.getCommentOperator(), refName, value, refName, el->getPin1Name(), refName, el->getPin2Name(), element, probeNumber1, probeNumber2, ptol, ntol);
         outputArray.Add(output);

         // Set comppin to being tested
         compPin1->setTestedSurface(testSurface);
         compPin1->setAlreadyTested(true);
         compPin2->setTestedSurface(testSurface);
         compPin2->setAlreadyTested(true);
      }
   }
}

bool TakayaFPTComponentCollection::generateVisionOutput(FILE *logFp, TakayaFPTComponent& component, 
      CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
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

      CString output;
      output.Format(" %s %s %s %s %s %.0f %.0f", visionCommand, component.getTakayaName(), value, component.getComment(),
         component.getLocation(), fptorigin.x, fptorigin.y);

      outputArray.Add(output);
      m_visionTestCount++;

      retval = true;
   }
   else
   {
         // Need to write vision but didn't get writen then log it in the log file
         fprintf(logFp, "Comp [%s] not written in Takaya output file.\n", component.getName());
   }

   return retval;
}

void TakayaFPTComponentCollection::generatePinToPinShortTest(CCamCadData& camcadData, FILE *logFp, CTypedPtrListContainer<TakayaFPTComponent*>& components, TakayaFPTProbeOutputCollection& probeOutputCollection,
   CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
{
   for (POSITION pos=components.GetHeadPosition(); pos!=NULL;)
   {
      TakayaFPTComponent* component = components.GetNext(pos);
      if (component != NULL && !component->getPinShortTestDone() && settings.lookupPinToPinDevice(component->getDeviceType()))
      {
         BlockStruct* block = camcadData.getBlockAt(component->getInsert().getBlockNumber());
         if (block == NULL)
            continue;

         int testPinCount = component->generateTestPinCount(logFp, settings.getMaxHitCount(), testSurface);
         TakayaFPTPinToPinAnalyzer* pinToPinAnalyzer = m_pinToPinAnalyzerMap.getPinToPinAnalyzer(*block, settings.getPinToPinDistance());
         if (pinToPinAnalyzer != NULL && testPinCount > 0)
         {  
            TakayaFPTPinToPinShort* pinToPinShort = pinToPinAnalyzer->getFirstSorted();
            while (pinToPinShort != NULL)
            {
               TakayaFPTCompPin* compPin1 = component->findCompPin(pinToPinShort->getPinName1());
               TakayaFPTCompPin* compPin2 = component->findCompPin(pinToPinShort->getPinName2());

               if (compPin1 != NULL && compPin2 != NULL)
               {
                  TakayaFPTTestAccess* testAccess1 = compPin1->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
                  TakayaFPTTestAccess* testAccess2 = compPin2->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
         
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

                           CString output;
                           output.Format(" %s P%s-%s %s * R %s %s @K OP", component->getTakayaName(), 
                                 pinToPinShort->getPinName1(), pinToPinShort->getPinName2(),
                                 component->getPartNumber().IsEmpty()?"*":component->getPartNumber(),
                                 probeNumber1, probeNumber2);
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

void TakayaFPTComponentCollection::generateComponentHighFlyZoneTest(FileStruct& file, CCamCadData& camcadData, FILE *logFp, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
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

void TakayaFPTComponentCollection::generateHighFlyZoneTest(FileStruct& file, CCamCadData& camcadData, FILE *logFp, CTypedPtrListContainer<TakayaFPTComponent*>& components, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
{
   for (POSITION pos=components.GetHeadPosition(); pos!=NULL;)
   {
      TakayaFPTComponent* component = components.GetNext(pos);
      if (component != NULL && component->getSurface() == testSurface && component->getCompHeight() > settings.getHighFlyZone())
      {
#ifdef deadcode // old dup code way
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
#endif

         CExtent* packageExtent = m_packageOutlineExtentMap.getPackageOutlineExtent(camcadData, component);
         
         if (packageExtent != NULL)
         {
            //packageExtent->transform(component->getInsert().getTMatrix());

            double xmax = packageExtent->getXmax();
            double xmin = packageExtent->getXmin();
            double ymax = packageExtent->getYmax();
            double ymin = packageExtent->getYmin();
            double compHeight = component->getCompHeight();

            ::convertToTakayaUnits(xmax, settings.getUnitFactor());
            ::convertToTakayaUnits(xmin, settings.getUnitFactor());
            ::convertToTakayaUnits(ymax, settings.getUnitFactor());
            ::convertToTakayaUnits(ymin, settings.getUnitFactor());

            ::convertToTakayaUnits(compHeight, settings.getUnitFactor());

            CPoint2d fptorigin = component->getInsertOrigin();
            CString output;
            output.Format("@HN3 %s %.0f %.0f %.0f %.0f %.0f %.0f %.0f", component->getTakayaName(),
                  fptorigin.x, fptorigin.y, xmax, xmin, ymax, ymin, compHeight);
            outputArray.Add(output);
         }
      }
   }

}

void TakayaFPTComponentCollection::generateComponentRepairFile(FileStruct & file, CCamCadData& camCadData, CStdioFileWriteFormat& repairFile, const CTMatrix pcbMatrix, 
                                                               TakayaFPTNetMap& netMap, TakayaFPTProbeOutputCollection& topProbeOutputCollection, TakayaFPTProbeOutputCollection& bottomProbeOutputCollection,
                                                               TakayaFPTSettings& settings, const double pageUnitFactor, const double takayaUnitFactor)
{
   writeRepairFileHeader(repairFile);

   writeRepairFileSection(file, camCadData, repairFile, pcbMatrix, m_resistorComponents, topProbeOutputCollection, bottomProbeOutputCollection, settings, pageUnitFactor, takayaUnitFactor);
   writeRepairFileSection(file, camCadData, repairFile, pcbMatrix, m_capacitorComponents, topProbeOutputCollection, bottomProbeOutputCollection, settings, pageUnitFactor, takayaUnitFactor);
   writeRepairFileSection(file, camCadData, repairFile, pcbMatrix, m_diodeComponents, topProbeOutputCollection, bottomProbeOutputCollection, settings, pageUnitFactor, takayaUnitFactor);
   writeRepairFileSection(file, camCadData, repairFile, pcbMatrix, m_inductorComponents, topProbeOutputCollection, bottomProbeOutputCollection, settings, pageUnitFactor, takayaUnitFactor);
   writeRepairFileSection(file, camCadData, repairFile, pcbMatrix, m_icComponents, topProbeOutputCollection, bottomProbeOutputCollection, settings, pageUnitFactor, takayaUnitFactor);
   writeRepairFileSection(file, camCadData, repairFile, pcbMatrix, m_transitorComponents, topProbeOutputCollection, bottomProbeOutputCollection, settings, pageUnitFactor, takayaUnitFactor);
   writeRepairFileSection(file, camCadData, repairFile, pcbMatrix, m_capacitorPolarizedComponents, topProbeOutputCollection, bottomProbeOutputCollection, settings, pageUnitFactor, takayaUnitFactor);
   writeRepairFileSection(file, camCadData, repairFile, pcbMatrix, m_capacitorTantalumComponents, topProbeOutputCollection, bottomProbeOutputCollection, settings, pageUnitFactor, takayaUnitFactor);
   writeRepairFileSection(file, camCadData, repairFile, pcbMatrix, m_otherComponents, topProbeOutputCollection, bottomProbeOutputCollection, settings, pageUnitFactor, takayaUnitFactor);
   writeRepairFileSection(file, camCadData, repairFile, pcbMatrix, m_testPoints, topProbeOutputCollection, bottomProbeOutputCollection, settings, pageUnitFactor, takayaUnitFactor);

   writeRepairFileVias   (file, camCadData, repairFile, pcbMatrix, netMap,       topProbeOutputCollection, bottomProbeOutputCollection, settings, pageUnitFactor, takayaUnitFactor);
}

void TakayaFPTComponentCollection::writeRepairFileSection(FileStruct& file, CCamCadData& camCadData, CStdioFileWriteFormat& repairFile, const CTMatrix pcbMatrix, 
                                                          CTypedPtrListContainer<TakayaFPTComponent*>& components, TakayaFPTProbeOutputCollection& topProbeOutputCollection, 
                                                          TakayaFPTProbeOutputCollection& bottomProbeOutputCollection, 
                                                          TakayaFPTSettings& settings, const double pageUnitFactor, const double takayaUnitFactor)
{
   CTMatrix pageFactorMatrix;
   pageFactorMatrix.scale(pageUnitFactor);  // Converts ccz page units to repair file units

   // takayaUnitFactor converts page units to takaya units, what we want is to convert takaya units to repair file units.
   CTMatrix takayaFactorMatrix;
   takayaFactorMatrix.scale( pageUnitFactor / takayaUnitFactor );

   for (POSITION pos=components.GetHeadPosition(); pos!=NULL;)
   {
      TakayaFPTComponent* tkyaComponent = components.GetNext(pos);
      if (tkyaComponent != NULL)
      {
#ifdef deadcode // clunk
         //First find a real part with the same refDes, if not found then use pcb component.
         BlockStruct* block = NULL;
         CExtent* packageExtent = NULL;
         block = GetRealPartPackageBlockForComponent(file, camCadData, tkyaComponent);
         if(NULL != block)
            packageExtent = m_packageOutlineExtentMap.getPackageOutlineExtent(camCadData, *block, true);
         
         if(NULL == packageExtent)
         {
            block = NULL;
            block = camCadData.getBlockAt(tkyaComponent->getInsert().getBlockNumber());
            if (block != NULL)
               packageExtent = m_packageOutlineExtentMap.getPackageOutlineExtent(camCadData, *block, false);
         }
#endif
         CExtent* packageExtent = m_packageOutlineExtentMap.getPackageOutlineExtent(camCadData, tkyaComponent);

         // packageExtent as returned above is owned by a map, no need to delete it.
         // If we make a new one here, though, we need to delete it or leak mem.
         bool localTempExtent = false;
         if (packageExtent == NULL)
         {
            packageExtent = new CExtent(0.0, 0.0, 0.0, 0.0);
            localTempExtent = true;
         }

         CPoint2d centroid;
         tkyaComponent->getInsert().getCentroidLocation(camCadData, centroid);
         centroid.transform(pageFactorMatrix);

         TakayaFPTCompPin* tkyaCompPin = tkyaComponent->getFirstSorted();
         while (tkyaCompPin != NULL)
         {
            TakayaFPTTestAccess *topTestAccess = tkyaCompPin->getTestAccessOnSelf(surfaceTop);
            TakayaFPTTestAccess *botTestAccess = tkyaCompPin->getTestAccessOnSelf(surfaceBottom);

            CString topProbeRefname, botProbeRefname;
            CPoint2d topProbeOrigin, botProbeOrigin;

            if (topTestAccess != NULL)
            {
               topProbeRefname = m_topProbeOutputCollection.getProbeNumberString(*topTestAccess, false/*NoAutoGenerate*/);
               topProbeOrigin = topTestAccess->getInsertOrigin();  // Already trasformed, do not apply factorMatrix or pcbMatrix
               topProbeOrigin.transform(takayaFactorMatrix);
            }

            if (botTestAccess != NULL)
            {
               botProbeRefname = m_bottomProbeOutputCollection.getProbeNumberString(*botTestAccess, false/*NoAutoGenerate*/);
               botProbeOrigin = botTestAccess->getInsertOrigin(); // Already trasformed, do not apply factorMatrix or pcbMatrix
               botProbeOrigin.transform(takayaFactorMatrix);
            }
           


            CPoint2d pinOrigin = tkyaCompPin->getInsertOrigin();
            CPoint2d pinOriginWithOffset(pinOrigin);
            getAccessMarkerXYOffset(camCadData, file.getBlock(), tkyaCompPin->getEntityNumber(), pinOriginWithOffset);

            pinOrigin.transform(pageFactorMatrix);
            pinOrigin.transform(pcbMatrix);
            pinOriginWithOffset.transform(pageFactorMatrix);
            pinOriginWithOffset.transform(pcbMatrix);

            writeRepairFileCompRecord(repairFile, tkyaComponent, tkyaCompPin, centroid, pinOrigin, pinOriginWithOffset, packageExtent,
               topProbeRefname, topProbeOrigin, botProbeRefname, botProbeOrigin);

            tkyaCompPin = tkyaComponent->getNextSorted();
         }

         if (localTempExtent)
            delete packageExtent;
      }
   }
}

//---------------------------------------------------------------------------------

void TakayaFPTComponentCollection::writeRepairFileVias(FileStruct& file, CCamCadData& camcadData, CStdioFileWriteFormat& repairFile, const CTMatrix pcbMatrix, 
                                                       TakayaFPTNetMap& netMap, TakayaFPTProbeOutputCollection& topProbeOutputCollection, TakayaFPTProbeOutputCollection& bottomProbeOutputCollection,
                                                       TakayaFPTSettings& settings, const double pageUnitFactor, const double takayaUnitFactor)
{
   // Vias get their own brute force repair file dumper because all the existing Takaya "component list"
   // implementation is founded on list that require refdes set, and vias more often than not lack refdes.
   // Vias are not part of the rest of Takaya tester output files, only here in the repair file.
   // So just dump them. 

   CTMatrix pageFactorMatrix;
   pageFactorMatrix.scale(pageUnitFactor);  // Converts ccz page units to repair file units

   // takayaUnitFactor converts page units to takaya units, what we want is to convert takaya units to repair file units.
   CTMatrix takayaFactorMatrix;
   takayaFactorMatrix.scale( pageUnitFactor / takayaUnitFactor );

   // Dump all vias to repair file
   BlockStruct *fileBlock = file.getBlock();
   if (fileBlock != NULL)
   {
      POSITION dataPos = fileBlock->getHeadDataInsertPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = fileBlock->getNextDataInsert(dataPos);
         if (data->isInsertType(insertTypeVia))
         {
            InsertStruct *viaInsert = data->getInsert();

            // Extent for actual comps is based preferebly on RealPart outline, secondarily on
            // component outline. Vias don't have these, just use a zero extent.
            CExtent packageExtent(0.,0.,0.,0.);

            CPoint2d centroid;
            viaInsert->getCentroidLocation(camcadData, centroid);
            centroid.transform(pageFactorMatrix);

            CString netName;
            camcadData.getAttributeStringValue(netName, *data->getAttributes(), standardAttributeNetName);
            TakayaFPTNet *tkyaNet = NULL;
            if (!netName.IsEmpty())
               tkyaNet = netMap.findNet(netName);

            if (tkyaNet != NULL)
            {
               TakayaFPTTestAccess *topTestAccess = tkyaNet->getTestAccessOnTarget(data->getEntityNumber(), surfaceTop);
               TakayaFPTTestAccess *botTestAccess = tkyaNet->getTestAccessOnTarget(data->getEntityNumber(), surfaceBottom);

               CString topProbeRefname, botProbeRefname;
               CPoint2d topProbeOrigin, botProbeOrigin;

               if (topTestAccess != NULL)
               {
                  topProbeRefname = m_topProbeOutputCollection.getProbeNumberString(*topTestAccess, false/*NoAutoGenerate*/);
                  topProbeOrigin = topTestAccess->getInsertOrigin();  // Already trasformed, do not apply factorMatrix or pcbMatrix
                  topProbeOrigin.transform(takayaFactorMatrix);
               }

               if (botTestAccess != NULL)
               {
                  botProbeRefname = m_bottomProbeOutputCollection.getProbeNumberString(*botTestAccess, false/*NoAutoGenerate*/);
                  botProbeOrigin = botTestAccess->getInsertOrigin(); // Already trasformed, do not apply factorMatrix or pcbMatrix
                  botProbeOrigin.transform(takayaFactorMatrix);
               }

               CPoint2d pinOrigin = viaInsert->getOrigin2d();
               CPoint2d pinOriginWithOffset(pinOrigin);
               getAccessMarkerXYOffset(camcadData, file.getBlock(), data->getEntityNumber(), pinOriginWithOffset);

               pinOrigin.transform(pageFactorMatrix);
               pinOrigin.transform(pcbMatrix);
               pinOriginWithOffset.transform(pageFactorMatrix);
               pinOriginWithOffset.transform(pcbMatrix);

               CString refname(viaInsert->getRefname());

               writeRepairFileRecord(repairFile, 
                  refname.IsEmpty()?"X":refname, "X", netName, viaInsert->getPlacedTop()?surfaceTop:surfaceBottom, 
                  "X"/*partnumber*/, true/*loaded*/, 0.0/*height*/, "X"/*PP command*/, 
                  centroid, 0.0, "VIA", "X" /*value*/, 
                  &packageExtent, pinOrigin, pinOriginWithOffset,
                  topProbeRefname, topProbeOrigin, botProbeRefname, botProbeOrigin);

            }
         }
      }
   }
}

//---------------------------------------------------------------------------------

void TakayaFPTComponentCollection::writeRepairFileCompRecord(CStdioFileWriteFormat& repairFile, TakayaFPTComponent *takayaComponent, TakayaFPTCompPin *takayaCompPin, 
                                                         CPoint2d centroid, CPoint2d pinOrigin, CPoint2d pinOriginWithOffset, CExtent *packageExtent,
                                                         CString topProbeName, CPoint2d topProbeOrigin, CString botProbeName, CPoint2d botProbeOrigin)
{
   CString smdThru;
   smdThru.Format("%s%s", takayaComponent->getInsert().getInsertType() == insertTypeTestPoint?"TP_":"", takayaComponent->isSmd()?"SMD":"THT");

   writeRepairFileRecord(repairFile,
      takayaComponent->getTakayaName(), takayaCompPin->getPinName(), takayaCompPin->getNetName(), takayaComponent->getSurface(),
      takayaComponent->getPartNumber(), takayaComponent->isLoaded(), takayaComponent->getCompHeightMM(), takayaComponent->getPPCommand(),
      centroid, takayaComponent->getInsert().getAngleDegrees(), smdThru, takayaComponent->getValue(),
      packageExtent, pinOrigin, pinOriginWithOffset, topProbeName,topProbeOrigin, botProbeName, botProbeOrigin);
}

//---------------------------------------------------------------------------------

void TakayaFPTComponentCollection::writeRepairFileRecord(CStdioFileWriteFormat& repairFile,
                                                         CString compName, CString pinName, CString netName, TakayaFPTSurface surface,
                                                         CString partnumber, bool isLoaded, double compHeight, CString ppCommand,
                                                         CPoint2d centroid, double rotationDegrees, CString smdThru, CString value,
                                                         CExtent *packageExtent, CPoint2d pinOrigin,  CPoint2d pinOriginWithOffset,
                                                         CString topProbeName, CPoint2d topProbeOrigin, CString botProbeName, CPoint2d botProbeOrigin)
{
   repairFile.writef("%s;",    compName);
   repairFile.writef("%s;",    pinName);
   repairFile.writef("%s;",    surface==surfaceTop?"T":"B");
   repairFile.writef("%s;",    partnumber);
   repairFile.writef("%d;",    isLoaded?1:0);

   // Hi Rick   (19 April 2011)
   // Can you tweak the COMPHEIGHT field in the Takaya repair file. Round it up and make it a whole number. It is always written in MM.
   // Thanks
   // Mark
   // [Rick] Ok.
   // So don't do this anymore:    repairFile.writef("%0.1f;", compHeight);
   repairFile.writef("%d;", (int)(ceil(compHeight)));

   repairFile.writef("%s;",    ppCommand);
   repairFile.writef("%0.5f\t%0.5f;", centroid.x, centroid.y);

   repairFile.writef("%0.3f;", packageExtent->getXmin());
   repairFile.writef("%0.3f;", packageExtent->getYmax()); 
   repairFile.writef("%0.3f;", packageExtent->getXmax());
   repairFile.writef("%0.3f;", packageExtent->getYmin());

   repairFile.writef("%0.5f\t%0.5f;", pinOrigin.x, pinOrigin.y);
   repairFile.writef("%0.5f\t%0.5f;", pinOriginWithOffset.x, pinOriginWithOffset.y); // with board offset
   
   CString topProbeXY("X\tX"), botProbeXY("X\tX"); // defaults for when probe does not exist
   if (!topProbeName.IsEmpty())
      topProbeXY.Format("%0.3f\t%0.3f", topProbeOrigin.x, topProbeOrigin.y);
   if (!botProbeName.IsEmpty())
      botProbeXY.Format("%0.3f\t%0.3f", botProbeOrigin.x, botProbeOrigin.y);

   repairFile.writef("%s;",    netName);
   repairFile.writef("%s;",    topProbeName.IsEmpty() ? "X" : topProbeName);
   repairFile.writef("%s;",    topProbeXY);
   repairFile.writef("%s;",    botProbeName.IsEmpty() ? "X" : botProbeName);
   repairFile.writef("%s;",    botProbeXY);

   repairFile.writef("%s;",    smdThru);
   repairFile.writef("%s;",    value);
   repairFile.writef("%.2lf;", normalizeDegrees(rotationDegrees));
   repairFile.writef("\n");
}

//---------------------------------------------------------------------------------

void TakayaFPTComponentCollection::writeRepairFileHeader(CStdioFileWriteFormat& repairFile)
{
   repairFile.writef( ":CompName;" );
   repairFile.writef( "PinName;" );
   repairFile.writef( "ComponentSurface;" );
   repairFile.writef( "PartNumber;" );
   repairFile.writef( "Loaded;" );

   repairFile.writef( "CompHeight;" );
   repairFile.writef( "PP;" );
   repairFile.writef( "CentroidX\tCentroidY;" );

   repairFile.writef( "PkgExtentXmin;" );
   repairFile.writef( "PkgExtentYmax;" );
   repairFile.writef( "PkgExtentXmax;" );
   repairFile.writef( "PkgExtentYmin;" );
   
   repairFile.writef( "PinOriginX\tPinOriginY;" );
   repairFile.writef( "PinOriginWithBrdOffsetX\tPinOriginWithBrdOffsetY;" );

   repairFile.writef( "NetName;" );
   repairFile.writef( "TopAccessNail;" );
   repairFile.writef( "TopAccessNailX\tTopAccessNailY;" );
   repairFile.writef( "BotAccessNail;" );
   repairFile.writef( "BotAccessNailX\tBotAccessNailY;" );

   repairFile.writef( "SMD/THT;" );
   repairFile.writef( "Value;" );
   repairFile.writef( "Rotation;\n" );
}

/******************************************************************************
* FindAccessDataForEntityNum
*/
DataStruct *TakayaFPTComponentCollection::FindAccessDataForEntityNum(CCamCadData& camCadData,  BlockStruct *pcbFileBlock, long entityNumber)
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

void TakayaFPTComponentCollection::getAccessMarkerXYOffset(CCamCadData& camCadData, BlockStruct *pcbFileBlock, int entityNumber, CPoint2d &pinOriginWithOffset)
{
   DataStruct *accessData = FindAccessDataForEntityNum(camCadData, pcbFileBlock, entityNumber);

   if(accessData && accessData->getInsert())
      pinOriginWithOffset = accessData->getInsert()->getOrigin2d();      
}

BlockStruct* TakayaFPTComponentCollection::GetRealPartPackageBlockForComponent(CCamCadData& camCadData,TakayaFPTComponent* component)
{
   FileStruct& file = component->getFile();
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

void TakayaFPTComponentCollection::writeLogFile(FILE *logFp)
{
   for (POSITION pos=m_resistorComponents.GetHeadPosition(); pos!=NULL;)
   {
      TakayaFPTComponent* component = m_resistorComponents.GetNext(pos);
      if (component != NULL)
      {
         TakayaFPTCompPin* compPin = component->getFirstSorted();
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
      TakayaFPTComponent* component = m_capacitorComponents.GetNext(pos);
      if (component != NULL)
      {
         TakayaFPTCompPin* compPin = component->getFirstSorted();
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
      TakayaFPTComponent* component = m_diodeComponents.GetNext(pos);
      if (component != NULL)
      {
         TakayaFPTCompPin* compPin = component->getFirstSorted();
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
      TakayaFPTComponent* component = m_inductorComponents.GetNext(pos);
      if (component != NULL)
      {
         TakayaFPTCompPin* compPin = component->getFirstSorted();
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
      TakayaFPTComponent* component = m_icComponents.GetNext(pos);
      if (component != NULL)
      {
         TakayaFPTCompPin* compPin = component->getFirstSorted();
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
      TakayaFPTComponent* component = m_transitorComponents.GetNext(pos);
      if (component != NULL)
      {
         TakayaFPTCompPin* compPin = component->getFirstSorted();
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
      TakayaFPTComponent* component = m_capacitorPolarizedComponents.GetNext(pos);
      if (component != NULL)
      {
         TakayaFPTCompPin* compPin = component->getFirstSorted();
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
      TakayaFPTComponent* component = m_capacitorTantalumComponents.GetNext(pos);
      if (component != NULL)
      {
         TakayaFPTCompPin* compPin = component->getFirstSorted();
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
      TakayaFPTComponent* component = m_otherComponents.GetNext(pos);
      if (component != NULL)
      {
         TakayaFPTCompPin* compPin = component->getFirstSorted();
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
TakayaIcDiodeCommandMap::TakayaIcDiodeCommandMap()
   : m_icDiodeCommandMap(nextPrime2n(20),true)
{
}

TakayaIcDiodeCommandMap::~TakayaIcDiodeCommandMap()
{
   empty();
}

void TakayaIcDiodeCommandMap::addCommand(const CString partNumber, const CString pinName, const bool swap)
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

TakayaIcDiodeCommand* TakayaIcDiodeCommandMap::getGroundPin(const CString partNumber)
{
   TakayaIcDiodeCommand* icDiodeCommand = NULL;
   CString upperCasePartNumber = partNumber;
   upperCasePartNumber.MakeUpper();   
   m_icDiodeCommandMap.Lookup(upperCasePartNumber, icDiodeCommand);

   return icDiodeCommand;
}

//_____________________________________________________________________________
TakayaFPTAptModel::TakayaFPTAptModel(const CString name)
: m_aptTestList(true)
, m_sName(name)
{
}

TakayaFPTAptModel::~TakayaFPTAptModel()
{
   m_aptTestList.empty();
}

TakayaFPTAptTest* TakayaFPTAptModel::createTest()
{
   TakayaFPTAptTest* aptTest = new TakayaFPTAptTest();
   m_aptTestList.AddTail(aptTest);
   
   return aptTest;
}

bool TakayaFPTAptModel::generateAptModelTest(FILE *logFp, TakayaFPTComponent& component, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface)
{
   bool retval = false;

   for (POSITION pos=m_aptTestList.GetHeadPosition(); pos!=NULL;)
   {
      TakayaFPTAptTest* aptTest = m_aptTestList.GetNext(pos);
      if(aptTest != NULL)
      {
		   // get the value, comment, location, element, option from aptTet if they are there
		   // otherwise, use whatever that is passed it

         CString refname = aptTest->GetRefdes();
         refname.Replace(TK_REF_REPLACEMENT, component.getTakayaName());

         CString value = component.getValue();
         if (!aptTest->GetValue().IsEmpty())
            value = aptTest->GetValue();

         CString comment = component.getValue();
         if (!aptTest->GetComment().IsEmpty())
         {
            comment = aptTest->GetComment();

            if (comment.CompareNoCase(TK_PART_REPLACEMENT) == 0)
               comment = component.getPartNumber();
            else
			      comment.Replace(TK_REF_REPLACEMENT, component.getTakayaName());
         }

         CString location = component.getLocation();
         if (!aptTest->GetLocation().IsEmpty())
            location = aptTest->GetLocation();

         CString element = component.getElement();
         if (!aptTest->GetElement().IsEmpty())
            element = aptTest->GetElement();

         CString option;
         if (!aptTest->GetOption().IsEmpty())
         {
            option = aptTest->GetOption();
            option.Replace(TK_PTOL_REPLACEMENT, component.getPositiveTol());
            option.Replace(TK_NTOL_REPLACEMENT, component.getNegativeTol());
         }

         if (aptTest->GetDeviceType() == deviceTypeUnknown)
         {
            TakayaFPTCompPin* compPin1 = component.findCompPin(aptTest->GetPinNum1());
            TakayaFPTCompPin* compPin2 = component.findCompPin(aptTest->GetPinNum2());
            if (compPin1 != NULL && compPin2 != NULL)
            {
               if (compPin1->getTestedSurface() != surfaceUnknown && compPin1->getTestedSurface() != testSurface &&
                  compPin2->getTestedSurface() != surfaceUnknown && compPin2->getTestedSurface() != testSurface )
                  continue;

               TakayaFPTTestAccess* testAccess1 = compPin1->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
               TakayaFPTTestAccess* testAccess2 = compPin2->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);

               if (testAccess1 != NULL && testAccess2 != NULL && testAccess1 != testAccess2)
               {
                  CString probeNumber1 = probeOutputCollection.getProbeNumberString(*testAccess1);
                  CString probeNumber2 = probeOutputCollection.getProbeNumberString(*testAccess2);

                  CString output;
                  output.Format("%s %s %s %s %s %s %s %s %s", component.getCommentOperator(), refname, value, comment, location, element, probeNumber1, probeNumber2, option);
                  outputArray.Add(output);

                  // Set comppin to being tested
                  compPin1->setTestedSurface(testSurface);
                  compPin1->setAlreadyTested(true);
                  compPin2->setTestedSurface(testSurface);
                  compPin2->setAlreadyTested(true);
                  retval = true;
               }
            }
         }
         else if (aptTest->GetDeviceType() == deviceTypeOpto)
         {
            TakayaFPTCompPin* compPin1 = component.findCompPin(aptTest->GetPinNum1());
            TakayaFPTCompPin* compPin2 = component.findCompPin(aptTest->GetPinNum2());
            TakayaFPTCompPin* compPin3 = component.findCompPin(aptTest->GetPinNum3());
            TakayaFPTCompPin* compPin4 = component.findCompPin(aptTest->GetPinNum4());

            if (compPin1 != NULL && compPin2 != NULL && compPin3 !=  NULL && compPin4 != NULL)
            {
               if ((compPin1->getTestedSurface() != surfaceUnknown && compPin1->getTestedSurface() != testSurface) ||
                   (compPin2->getTestedSurface() != surfaceUnknown && compPin2->getTestedSurface() != testSurface) ||
                   (compPin3->getTestedSurface() != surfaceUnknown && compPin3->getTestedSurface() != testSurface) ||
                   (compPin4->getTestedSurface() != surfaceUnknown && compPin4->getTestedSurface() != testSurface))
                  continue;

               TakayaFPTTestAccess* testAccess1 = compPin1->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
               TakayaFPTTestAccess* testAccess2 = compPin2->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
               TakayaFPTTestAccess* testAccess3 = compPin3->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
               TakayaFPTTestAccess* testAccess4 = compPin4->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);

               if (testAccess1 != NULL && testAccess2 != NULL && testAccess3 != NULL && testAccess4 != NULL)
               {
                  CString anodeProbeNum = probeOutputCollection.getProbeNumberString(*testAccess1);
                  CString cathodeProbeNum = probeOutputCollection.getProbeNumberString(*testAccess2);
                  CString collectorProbeNum = probeOutputCollection.getProbeNumberString(*testAccess3);
                  CString emitterProbeNum = probeOutputCollection.getProbeNumberString(*testAccess4);

                  CString output;
                  output.Format("%s@PC %s %s * %s %s %s %s", component.getCommentOperator(), refname, comment,
                     anodeProbeNum, cathodeProbeNum, emitterProbeNum, collectorProbeNum);
                  outputArray.Add(output);

                  output.Format("%s@CE", component.getCommentOperator());
                  outputArray.Add(output);

                  output.Format("%s%s C-E %s * R %s %s @K OP @K JP", component.getCommentOperator(), refname, comment,
                     collectorProbeNum, emitterProbeNum);
                  outputArray.Add(output);
                  
                  output.Format("%s%s A-K %s * D %s %s @T 25 25 @K JP", component.getCommentOperator(), refname, comment,
                     anodeProbeNum, cathodeProbeNum);
                  outputArray.Add(output);

                  // Set comppin to being tested
                  compPin1->setTestedSurface(testSurface);
                  compPin1->setAlreadyTested(true);
                  compPin2->setTestedSurface(testSurface);
                  compPin2->setAlreadyTested(true);
                  compPin3->setTestedSurface(testSurface);
                  compPin3->setAlreadyTested(true);
                  compPin4->setTestedSurface(testSurface);
                  compPin4->setAlreadyTested(true);
                  retval = true;
               }
            }
         }
         else if (aptTest->GetDeviceType() == deviceTypeTransistor || aptTest->GetDeviceType() == deviceTypeTransistorFetNpn)
         {
            TakayaFPTCompPin* compPin1 = component.findCompPin(aptTest->GetPinNum1());
            TakayaFPTCompPin* compPin2 = component.findCompPin(aptTest->GetPinNum2());
            TakayaFPTCompPin* compPin3 = component.findCompPin(aptTest->GetPinNum3());

            if (compPin1 != NULL && compPin2 != NULL && compPin3 !=  NULL)
            {
               if ((compPin1->getTestedSurface() != surfaceUnknown && compPin1->getTestedSurface() != testSurface) ||
                   (compPin2->getTestedSurface() != surfaceUnknown && compPin2->getTestedSurface() != testSurface) ||
                   (compPin3->getTestedSurface() != surfaceUnknown && compPin3->getTestedSurface() != testSurface))
                  continue;

               TakayaFPTTestAccess* testAccess1 = compPin1->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
               TakayaFPTTestAccess* testAccess2 = compPin2->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
               TakayaFPTTestAccess* testAccess3 = compPin3->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);

               if (testAccess1 != NULL && testAccess2 != NULL && testAccess3 != NULL)
               {
                  CString probeNumber1 = probeOutputCollection.getProbeNumberString(*testAccess1);
                  CString probeNumber2 = probeOutputCollection.getProbeNumberString(*testAccess2);
                  CString probeNumber3 = probeOutputCollection.getProbeNumberString(*testAccess3);

                  CString output;
                  if (aptTest->GetDeviceType() == deviceTypeTransistor)
                  {
                     comment = comment=="*"?"NPN":comment;
                     output.Format("%s@Q2 %s %s * %s", component.getCommentOperator(), refname, comment, option);
                  }
                  else if (aptTest->GetDeviceType() == deviceTypeTransistorFetNpn)
                  {
                     comment = comment=="*"?"FET_N":comment;
                     output.Format("%s@F %s %s * %s", component.getCommentOperator(), refname, comment, option);
                  }

                  outputArray.Add(output);               

                  output.Format("%s %s", component.getCommentOperator(), probeNumber1);
                  outputArray.Add(output);

                  output.Format("%s %s", component.getCommentOperator(), probeNumber2);
                  outputArray.Add(output);

                  output.Format("%s %s", component.getCommentOperator(), probeNumber3);
                  outputArray.Add(output);

                  output.Format("%s@CE", component.getCommentOperator());
                  outputArray.Add(output);

                  // Set comppin to being tested
                  compPin1->setTestedSurface(testSurface);
                  compPin1->setAlreadyTested(true);
                  compPin2->setTestedSurface(testSurface);
                  compPin2->setAlreadyTested(true);
                  compPin3->setTestedSurface(testSurface);
                  compPin3->setAlreadyTested(true);
                  retval = true;
               }
            }
         }
         else if (aptTest->GetDeviceType() == deviceTypeRelay)
         {
            TakayaFPTCompPin* compPin1 = component.findCompPin(aptTest->GetPinNum1());
            TakayaFPTCompPin* compPin2 = component.findCompPin(aptTest->GetPinNum2());
            TakayaFPTCompPin* compPin3 = component.findCompPin(aptTest->GetPinNum3());
            TakayaFPTCompPin* compPin4 = component.findCompPin(aptTest->GetPinNum4());

            if (compPin1 != NULL && compPin2 != NULL && compPin3 !=  NULL && compPin4 != NULL)
            {
               if ((compPin1->getTestedSurface() != surfaceUnknown && compPin1->getTestedSurface() != testSurface) ||
                   (compPin2->getTestedSurface() != surfaceUnknown && compPin2->getTestedSurface() != testSurface) ||
                   (compPin3->getTestedSurface() != surfaceUnknown && compPin3->getTestedSurface() != testSurface) ||
                   (compPin4->getTestedSurface() != surfaceUnknown && compPin4->getTestedSurface() != testSurface))
                  continue;

               TakayaFPTTestAccess* testAccess1 = compPin1->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
               TakayaFPTTestAccess* testAccess2 = compPin2->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
               TakayaFPTTestAccess* testAccess3 = compPin3->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);
               TakayaFPTTestAccess* testAccess4 = compPin4->getTestAccess(logFp, settings.getMaxHitCount(), testSurface);

               if (testAccess1 != NULL && testAccess2 != NULL && testAccess3 != NULL && testAccess4 != NULL)
               {
                  CString probeNumber1 = probeOutputCollection.getProbeNumberString(*testAccess1);
                  CString probeNumber2 = probeOutputCollection.getProbeNumberString(*testAccess2);
                  CString probeNumber3 = probeOutputCollection.getProbeNumberString(*testAccess3);
                  CString probeNumber4 = probeOutputCollection.getProbeNumberString(*testAccess4);

                  CString output;
                  output.Format("%s %s %s %s %s %s %s @P+:%s @P-:%s %s", component.getCommentOperator(), refname, value, comment, element, probeNumber1, probeNumber2, probeNumber3, probeNumber4, option);
                  outputArray.Add(output);
                  // Set comppin to being tested
                  compPin1->setTestedSurface(testSurface);
                  compPin1->setAlreadyTested(true);
                  compPin2->setTestedSurface(testSurface);
                  compPin2->setAlreadyTested(true);
                  compPin3->setTestedSurface(testSurface);
                  compPin3->setAlreadyTested(true);
                  compPin4->setTestedSurface(testSurface);
                  compPin4->setAlreadyTested(true);
                  retval = true;
               }
            }
         }         
      }
   }

   return retval;
}

//_____________________________________________________________________________
TakayaFPTAptModelMap::TakayaFPTAptModelMap()
: m_aptModelMap(nextPrime2n(20), true)
{
}

TakayaFPTAptModelMap::~TakayaFPTAptModelMap()
{
   empty();
}

void TakayaFPTAptModelMap::empty()
{
   m_aptModelMap.empty();
}

TakayaFPTAptModel* TakayaFPTAptModelMap::add(const CString aptModelName)
{
   TakayaFPTAptModel* aptModel = lookup(aptModelName);
	if (aptModel == NULL)
	{
		// Create and add aptModel to map
		aptModel = new TakayaFPTAptModel(aptModelName);
		m_aptModelMap.SetAt(aptModelName, aptModel);
	}

   return aptModel;
}

TakayaFPTAptModel* TakayaFPTAptModelMap::lookup(const CString aptModelName)
{
   TakayaFPTAptModel* aptModel = NULL;
   m_aptModelMap.Lookup(aptModelName, aptModel);
   return aptModel;
}

//_____________________________________________________________________________
TakayaFPTPinToPinAnalyzer::TakayaFPTPinToPinAnalyzer()
: m_pinToPinMap(nextPrime2n(20), true)
{
   m_pinToPinMap.empty();
}

TakayaFPTPinToPinAnalyzer::~TakayaFPTPinToPinAnalyzer()
{
   m_pinToPinMap.empty();
}

void TakayaFPTPinToPinAnalyzer::analysisPinToPinShort(BlockStruct& block, const double distance)
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
               TakayaFPTPinToPinShort* pinToPinShort = new TakayaFPTPinToPinShort(pinInsert1->getRefname(), closestPinInsert->getRefname());
               m_pinToPinMap.SetAt(pinName1, pinToPinShort);
            }
            else
            {
               TakayaFPTPinToPinShort* pinToPinShort = new TakayaFPTPinToPinShort(closestPinInsert->getRefname(), pinInsert1->getRefname());
               m_pinToPinMap.SetAt(pinName2, pinToPinShort);
            }
         }        
      }
   }

   m_pinToPinMap.Sort();
}

TakayaFPTPinToPinShort* TakayaFPTPinToPinAnalyzer::getFirstSorted()
{
   TakayaFPTPinToPinShort* pinToPinShort = NULL;
   CString* key = NULL;
   m_pinToPinMap.GetFirstSorted(key, pinToPinShort);

   return pinToPinShort;
}

TakayaFPTPinToPinShort* TakayaFPTPinToPinAnalyzer::getNextSorted()
{
   TakayaFPTPinToPinShort* pinToPinShort = NULL;
   CString* key = NULL;
   m_pinToPinMap.GetNextSorted(key, pinToPinShort);

   return pinToPinShort;
}

//_____________________________________________________________________________
TakayaFPTPinToPinAnalyzerMap::TakayaFPTPinToPinAnalyzerMap()
: m_pcbComponentPinToPinAnalyzerMap(nextPrime2n(20), true)
{
   m_pcbComponentPinToPinAnalyzerMap.empty();
}

TakayaFPTPinToPinAnalyzerMap::~TakayaFPTPinToPinAnalyzerMap()
{
   m_pcbComponentPinToPinAnalyzerMap.empty();
}

TakayaFPTPinToPinAnalyzer* TakayaFPTPinToPinAnalyzerMap::getPinToPinAnalyzer(BlockStruct& block, const double distance)
{
   TakayaFPTPinToPinAnalyzer* pinToPinAnalyzer = NULL;
   if (!m_pcbComponentPinToPinAnalyzerMap.Lookup(block.getBlockNumber(), pinToPinAnalyzer))
   {
      pinToPinAnalyzer = new TakayaFPTPinToPinAnalyzer();
      pinToPinAnalyzer->analysisPinToPinShort(block, distance);
      m_pcbComponentPinToPinAnalyzerMap.SetAt(block.getBlockNumber(), pinToPinAnalyzer);
   }

   return pinToPinAnalyzer;
}

//_____________________________________________________________________________
TakayaFPTPackageOutlineExtentMap::TakayaFPTPackageOutlineExtentMap()
: m_blockPackageExtentMap(nextPrime2n(20), true)
{
}

TakayaFPTPackageOutlineExtentMap::~TakayaFPTPackageOutlineExtentMap()
{
   m_blockPackageExtentMap.empty();
}

CExtent* TakayaFPTPackageOutlineExtentMap::getPackageOutlineExtent(CCamCadData& camCadData, TakayaFPTComponent* tkyaComponent)
{
   // This is the one to call from the app, with component as param.
   //First try for a real part with the same refDes, if not found then use pcb component.

   CExtent* packageExtent = NULL;

   // Try RealPart
   BlockStruct* block = GetRealPartPackageBlockForComponent(camCadData, tkyaComponent);
   if (NULL != block)
      packageExtent = this->getPackageOutlineExtent(camCadData, *block, true);

   // If RealPart didn't work out, try PCB Component.
   if (NULL == packageExtent)
   {
      block = camCadData.getBlockAt(tkyaComponent->getInsert().getBlockNumber());
      if (block != NULL)
         packageExtent = this->getPackageOutlineExtent(camCadData, *block, false);
   }

   return packageExtent;
}

BlockStruct* TakayaFPTPackageOutlineExtentMap::GetRealPartPackageBlockForComponent(CCamCadData& camCadData, TakayaFPTComponent* component)
{
   // We have two of these funcs now, this one and one over in the component collection.
   // We could toss this one, and use the one in comp collection to cache the RP pkgBlock ptr in the component, then don't need to look it up more than once.

   FileStruct& file = component->getFile();
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

CExtent* TakayaFPTPackageOutlineExtentMap::getPackageOutlineExtent(CCamCadData& camCadData, BlockStruct& block, bool realPart)
{
   // This is a worker bee, called from the one above. You probably don't want to use this directly
   // unless it matters whether the extent comes from RealPart versus PCB Comp.

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
TakayaFPTPcbFile::TakayaFPTPcbFile(FileStruct& fileStruct)
: m_fileStruct(fileStruct)
{
   m_name.Empty();
   m_blockNumber = 0;
   m_xyOrigin.x = 0.0;
   m_xyOrigin.y = 0.0;
   m_rotationRadian = 0;   
   m_isMirrored = false;
}

TakayaFPTPcbFile::~TakayaFPTPcbFile()
{
}

//_____________________________________________________________________________
TakayaFPTSettings::TakayaFPTSettings()
{
   reset();
}

TakayaFPTSettings::~TakayaFPTSettings()
{

}

void TakayaFPTSettings::reset()
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

   m_testLibraryMode = false; // EXPORE_MORE
   m_testLibraryFileName.Empty();   // APT_MODEL_FILE_NAME
   m_takayaLibraryDirectory.Empty();

	m_pinToPinDeviceTypeMap.RemoveAll();
   m_pinToPinDistance = -1;

   m_exportIcCapacitor = true;
   m_exportIcDiode = true;
   m_isIcOpenDescriptionAttrib = false;
   m_icOpenDescription.Empty();
   m_icDiodePrefix.Empty();
   m_icCapacitorPrefix.Empty();
   m_commentAttribute.Empty();
   m_PPCommandAttribName.Empty();

   m_kelvinThreshold = 0.0;  // Value of 0 turns off Kelvin feature.

   m_capOpensSensorDiameterMM = TK_DEFAULT_OUTLINE_SHRINK_DISTANCE;  // In millimeters !

   m_exportAllAccessPoints = false; // false is equivalent to standard behavior before this option existed.

   m_originModeTag = cczCadOrigin;
}

void TakayaFPTSettings::addPinToPinDevice(const CString deviceType)
{
   CString deviceTypeKey = deviceType;
   deviceTypeKey.MakeUpper();

   m_pinToPinDeviceTypeMap.SetAt(deviceTypeKey, deviceType);
}

bool TakayaFPTSettings::lookupPinToPinDevice(const DeviceTypeTag deviceType)
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

CString TakayaFPTSettings::getTestLibraryFullPathName() const
{
   CString retval;

   if (m_takayaLibraryDirectory.IsEmpty())
   {
      retval = getApp().getSystemSettingsFilePath(m_testLibraryFileName);
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
TakayaFPTWrite::TakayaFPTWrite(CCEtoODBDoc& camCadDoc, const CString pathName)
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

   m_unitFactor = m_camCadDoc.convertPageUnitsTo(pageUnitsMilliMeters, 1000.);

   m_noProbeAccessMarkCountTop = 0;
   m_noProbeAccessMarkCountBot = 0;

   // This is to force the open and writing of the header early.
   getLogFile();
}

TakayaFPTWrite::~TakayaFPTWrite()
{
   closeLogFile();
   closeTopFile();
   closeBottomFile();
   empty();
}

void TakayaFPTWrite::empty()
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

   m_topProbeOutputCollection.RemoveAll();
   m_bottomProbeOutputCollection.RemoveAll();

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

CProgressDlg& TakayaFPTWrite::getProgressDlg()
{
   if (m_progressDlg == NULL)
   {
      m_progressDlg = new CProgressDlg("Takaya Output", false);
      m_progressDlg->Create(AfxGetMainWnd());
   }

   return *m_progressDlg;
}

FILE * TakayaFPTWrite::getLogFile()
{
   if (m_logFp == NULL)
   {
      m_logFp = getApp().OpenOperationLogFile("takaya.log", m_localLogFilename);
      WriteStandardExportLogHeader(m_logFp, "Takaya");
   }

   return m_logFp;
}

void TakayaFPTWrite::closeLogFile()
{
   // Close log file only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!m_localLogFilename.IsEmpty() && m_logFp != NULL)
   {
      fclose(m_logFp);
      m_logFp = NULL;
   }
}

CStdioFileWriteFormat& TakayaFPTWrite::getTopFile()
{
   if (m_topOutputFile == NULL)
   {
      CString fileName = m_pathName;
	   if (fileName.Find(".ca",0) != -1)
         fileName.Insert(fileName.Find(".ca",0), "_top");
	   else
		   fileName += "_top";
      
      m_topOutputFile = new CStdioFileWriteFormat();
      m_topOutputFile->open(fileName);
   }

   return *m_topOutputFile;
}

void TakayaFPTWrite::closeTopFile()
{
   if (m_topOutputFile != NULL)
   {
      m_topOutputFile->close();

      delete m_topOutputFile;
      m_topOutputFile = NULL;
   }
}

CStdioFileWriteFormat& TakayaFPTWrite::getBottomFile()
{
   if (m_bottomOutputFile == NULL)
   {
      CString fileName = m_pathName;
	   if (fileName.Find(".ca",0) != -1)
         fileName.Insert(fileName.Find(".ca",0), "_bot");
	   else
		   fileName += "_bot";

      m_bottomOutputFile = new CStdioFileWriteFormat();
      m_bottomOutputFile->open(fileName);
   }

   return *m_bottomOutputFile;
}

void TakayaFPTWrite::closeBottomFile()
{
   if (m_bottomOutputFile != NULL)
   {
      m_bottomOutputFile->close();

      delete m_bottomOutputFile;
      m_bottomOutputFile = NULL;
   }
}

void TakayaFPTWrite::write()
{
   if (getPcbDesignList())
   {
      loadSettings();
      loadModelTest();

      if (!m_settings.getTopPopulated())
         fprintf(getLogFile(), "TAKAYA Test report does not include BOTTOM side placed components !\n\n");
      if (!m_settings.getBottomPopulated())
         fprintf(getLogFile(), "TAKAYA Test report does not include TOP side placed components !\n\n");

      // The dialog inits rhe BrdRefPnts, so we want a dialog even if it turns
      // out we do not go Modal with it.
      TakayaBoardPointSettingsDlg dlg(m_camCadData, m_settings.getOriginMode());

      if (getApp().m_readWriteJob.IsActivated())
      {
         // vPlan DFT style, no popups allowed. Default the settings as needed.
         // If we wanted to change any settings from defaults in dlg then here is where to do it. None today.
      }
      else
      {
         // Classic interactive CAMCAD.
         if (dlg.DoModal() != IDOK)  // SilentRunning aka readWriteJob->activated is already checked.
            return;
      }

	   m_topBrdRefPntCommand = dlg.GetTopBrdRefPntCommand();
	   m_topAuxRefPntCommand = dlg.GetTopAuxRefPntCommand();
	   m_botBrdRefPntCommand = dlg.GetBotBrdRefPntCommand();
	   m_botAuxRefPntCommand = dlg.GetBotAuxRefPntCommand();

      m_topOutputOrigin = dlg.GetTopBrdOrigin();
      m_botOutputOrigin = dlg.GetBotBrdOrigin();

      this->m_componentCollection.SetTakayaOrigin(m_topOutputOrigin, m_botOutputOrigin);

      getProgressDlg().ShowWindow(SW_SHOW);
      getProgressDlg().SetStatus("Preparing Data for Takaya output...");
      for (POSITION pos=m_pcbDesignList.GetHeadPosition(); pos!=NULL;)
      {
         TakayaFPTPcbFile* pcbFile = m_pcbDesignList.GetNext(pos);
         if (pcbFile != NULL)
         {
            generate_PINLOC(&m_camCadDoc, &pcbFile->getFileStruct(), FALSE);

            if (loadTakayaData(pcbFile->getFileStruct()))
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
                  m_settings.setExportIcCapacitor(dlg.GetICCapacitance()==TRUE);
                  m_settings.setExportIcDiode(dlg.GetICDiode()==TRUE);
               }

               //----------------------------
               // Generate test for components
               if (m_settings.getTestSurface() == surfaceBoth)
               {
                  if (m_settings.getPreferenceSurface() == surfaceTop)
                  {
                     //First generate for top and then for bottom
                     // Export access points that are used
                     m_componentCollection.generateComponentTest(m_camCadData, getLogFile(), m_aptModelMap, m_topProbeOutputCollection, m_topOutputArray, m_groundNet, m_icDiodeCommandMap, m_settings, surfaceTop);
                     generateProbablyShortTest(pcbFile->getFileStruct(), getLogFile(), m_topProbeOutputCollection, m_topOutputArray, surfaceTop);
                     m_netMap.generatePowerRailShortTest(getLogFile(), m_topProbeOutputCollection, m_topOutputArray, m_settings, surfaceTop);
                     m_componentCollection.generateComponentHighFlyZoneTest(pcbFile->getFileStruct(), m_camCadData, getLogFile(), m_topProbeOutputCollection, m_topOutputArray, m_settings, surfaceTop);
                     
                     // If exporting all access pts then add probes to output collection for unused access pts.
                     if (m_settings.getExportAllAccessPoints())
                        m_noProbeAccessMarkCountTop = createProbesForUnusedAccessMarks(surfaceTop);

                     // Export access points that are used
                     m_componentCollection.generateComponentTest(m_camCadData, getLogFile(), m_aptModelMap, m_bottomProbeOutputCollection, m_bottomOutputArray, m_groundNet, m_icDiodeCommandMap, m_settings, surfaceBottom);
                     generateProbablyShortTest(pcbFile->getFileStruct(), getLogFile(), m_bottomProbeOutputCollection, m_bottomOutputArray, surfaceBottom);
                     m_netMap.generatePowerRailShortTest(getLogFile(), m_bottomProbeOutputCollection, m_bottomOutputArray, m_settings, surfaceBottom);
                     m_componentCollection.generateComponentHighFlyZoneTest(pcbFile->getFileStruct(), m_camCadData, getLogFile(), m_bottomProbeOutputCollection, m_bottomOutputArray, m_settings, surfaceBottom);
                                      
                     // If exporting all access pts then add probes to output collection for unused access pts.
                     if (m_settings.getExportAllAccessPoints())
                        m_noProbeAccessMarkCountBot = createProbesForUnusedAccessMarks(surfaceBottom); 
                  }
                  else
                  {
                     //First generate for bottom and then for top
                     // Export access points that are used
                     m_componentCollection.generateComponentTest(m_camCadData, getLogFile(), m_aptModelMap, m_bottomProbeOutputCollection, m_bottomOutputArray, m_groundNet, m_icDiodeCommandMap, m_settings, surfaceBottom);
                     generateProbablyShortTest(pcbFile->getFileStruct(), getLogFile(), m_bottomProbeOutputCollection, m_bottomOutputArray, surfaceBottom);
                     m_netMap.generatePowerRailShortTest(getLogFile(), m_bottomProbeOutputCollection, m_bottomOutputArray, m_settings, surfaceBottom);
                     m_componentCollection.generateComponentHighFlyZoneTest(pcbFile->getFileStruct(), m_camCadData, getLogFile(), m_bottomProbeOutputCollection, m_bottomOutputArray, m_settings, surfaceBottom);

                     // If exporting all access pts then add probes to output collection for unused access pts.
                     if (m_settings.getExportAllAccessPoints())
                        m_noProbeAccessMarkCountBot = createProbesForUnusedAccessMarks(surfaceBottom);

                     // Export access points that are used
                     m_componentCollection.generateComponentTest(m_camCadData, getLogFile(), m_aptModelMap, m_topProbeOutputCollection, m_topOutputArray, m_groundNet, m_icDiodeCommandMap, m_settings, surfaceTop);
                     generateProbablyShortTest(pcbFile->getFileStruct(), getLogFile(), m_topProbeOutputCollection, m_topOutputArray, surfaceTop);
                     m_netMap.generatePowerRailShortTest(getLogFile(), m_topProbeOutputCollection, m_topOutputArray, m_settings, surfaceTop);
                     m_componentCollection.generateComponentHighFlyZoneTest(pcbFile->getFileStruct(), m_camCadData, getLogFile(), m_topProbeOutputCollection, m_topOutputArray, m_settings, surfaceTop);                  
                  
                     // If exporting all access pts then add probes to output collection for unused access pts.
                     if (m_settings.getExportAllAccessPoints())
                        m_noProbeAccessMarkCountTop = createProbesForUnusedAccessMarks(surfaceTop);
                  }              
               }
               else if (m_settings.getTestSurface() == surfaceTop)
               {
                  // Export access points that are used
                  m_componentCollection.generateComponentTest(m_camCadData, getLogFile(), m_aptModelMap, m_topProbeOutputCollection, m_topOutputArray, m_groundNet, m_icDiodeCommandMap, m_settings, surfaceTop);
                  generateProbablyShortTest(pcbFile->getFileStruct(), getLogFile(), m_topProbeOutputCollection, m_topOutputArray, surfaceTop);
                  m_netMap.generatePowerRailShortTest(getLogFile(), m_topProbeOutputCollection, m_topOutputArray, m_settings, surfaceTop);
                  m_componentCollection.generateComponentHighFlyZoneTest(pcbFile->getFileStruct(), m_camCadData, getLogFile(), m_topProbeOutputCollection, m_topOutputArray, m_settings, surfaceTop);
              
                  // If exporting all access pts then add probes to output collection for unused access pts.
                  if (m_settings.getExportAllAccessPoints())
                     m_noProbeAccessMarkCountTop = createProbesForUnusedAccessMarks(surfaceTop);
               }
               else if (m_settings.getTestSurface() == surfaceBottom)
               {
                  // Export access points that are used
                  m_componentCollection.generateComponentTest(m_camCadData, getLogFile(), m_aptModelMap, m_bottomProbeOutputCollection, m_bottomOutputArray, m_groundNet, m_icDiodeCommandMap, m_settings, surfaceBottom);
                  generateProbablyShortTest(pcbFile->getFileStruct(), getLogFile(), m_bottomProbeOutputCollection, m_bottomOutputArray, surfaceBottom);
                  m_netMap.generatePowerRailShortTest(getLogFile(), m_bottomProbeOutputCollection, m_bottomOutputArray, m_settings, surfaceBottom);
                  m_componentCollection.generateComponentHighFlyZoneTest(pcbFile->getFileStruct(), m_camCadData, getLogFile(), m_bottomProbeOutputCollection, m_bottomOutputArray, m_settings, surfaceBottom);

                  // If exporting all access pts then add probes to output collection for unused access pts.
                  if (m_settings.getExportAllAccessPoints())
                     m_noProbeAccessMarkCountBot = createProbesForUnusedAccessMarks(surfaceBottom);
               }

               // Output Repair File
               CString repairFileName = m_pathName;
	            int index = repairFileName.ReverseFind('.');
               if (index > -1)
                  repairFileName.Delete(index, repairFileName.GetLength() - index);
               repairFileName.Append(".asc");

               CStdioFileWriteFormat* repairFile = new CStdioFileWriteFormat();
               repairFile->open(repairFileName);

               double mmUnitFactor =  m_camCadDoc.convertPageUnitsTo(pageUnitsMilliMeters, 1);
               repairFile->writef(":BottomBoardReferenceX\tBottomBoardReferenceY;BottomAuxilliaryReferenceX\tBottomAuxilliaryReferenceY;");
               repairFile->writef("TopBoardReferenceX\tTopBoardReferenceY;TopAuxilliaryReferenceX\tTopAuxilliaryReferenceY;\n");
               repairFile->writef("%0.5f\t%0.5f;", dlg.GetBotBrdRefPnt().x*mmUnitFactor, dlg.GetBotBrdRefPnt().y*mmUnitFactor);
               repairFile->writef("%0.5f\t%0.5f;", dlg.GetBotAuxRefPnt().x*mmUnitFactor, dlg.GetBotAuxRefPnt().y*mmUnitFactor);
               repairFile->writef("%0.5f\t%0.5f;", dlg.GetTopBrdRefPnt().x*mmUnitFactor, dlg.GetTopBrdRefPnt().y*mmUnitFactor);
               repairFile->writef("%0.5f\t%0.5f;\n", dlg.GetTopAuxRefPnt().x*mmUnitFactor, dlg.GetTopAuxRefPnt().y*mmUnitFactor);
               repairFile->writef(":\n");

               m_componentCollection.generateComponentRepairFile(pcbFile->getFileStruct(), m_camCadData, *repairFile, pcbFile->getMatrix(), 
                  this->m_netMap, m_topProbeOutputCollection, m_bottomProbeOutputCollection, m_settings, mmUnitFactor, this->m_unitFactor);

               repairFile->close();
            }
            else
            {
               ErrorMessage("No TestAccess point found!\nYou must run TestAccess Analysis first and rerun the Takaya output.",
					      "No Test Access Point found!");

               m_settings.setTopPopulated(false);
               m_settings.setBottomPopulated(false);
               break;
            }
            
            m_componentCollection.writeLogFile(getLogFile());
            m_netMap.writeLogFile(getLogFile());
         }
      }

      if (m_settings.getTopPopulated())
      {
         CString topFileName = m_pathName;
	      if (topFileName.Find(".ca",0) != -1)
            topFileName.Insert(topFileName.Find(".ca",0), "_top");
	      else
		      topFileName += "_top";

         getProgressDlg().SetStatus("Outputing " + topFileName);
         writeTakayaOutput(topFileName, m_topOutputArray, m_topProbeOutputCollection, surfaceTop);
      }

      if (m_settings.getBottomPopulated())
      {
         CString bottomFileName = m_pathName;
	      if (bottomFileName.Find(".ca",0) != -1)
            bottomFileName.Insert(bottomFileName.Find(".ca",0), "_bot");
	      else
		      bottomFileName += "_bot";
       
         getProgressDlg().SetStatus("Outputing " + bottomFileName);
         writeTakayaOutput(bottomFileName, m_bottomOutputArray, m_bottomProbeOutputCollection, surfaceBottom);
      }
   }

   //closeTopFile();
   //closeBottomFile();
   //closeLogFile();
}

bool TakayaFPTWrite::getPcbDesignList()
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

               TakayaFPTPcbFile* takayaPcbFile = new TakayaFPTPcbFile(*pcbFile);
               takayaPcbFile->setName(pcbInsert->getRefname());
               takayaPcbFile->setBlockNumber(pcbInsert->getBlockNumber());
               takayaPcbFile->setMatrix(pcbInsert->getTMatrix());
               takayaPcbFile->setInsertOrigin(pcbInsert->getOrigin2d());
               takayaPcbFile->setRotationRadian(pcbInsert->getAngleRadians());
               takayaPcbFile->setMirrored(pcbInsert->getGraphicMirrored());

               m_pcbDesignList.AddTail(takayaPcbFile);
            }
         }
         else if (file->getBlockType() == blockTypePcb)
         {
            TakayaFPTPcbFile* takayaPcbFile = new TakayaFPTPcbFile(*file);
            takayaPcbFile->setName(file->getName());
            takayaPcbFile->setBlockNumber(file->getBlock()->getBlockNumber());
            takayaPcbFile->setInsertOrigin(CPoint2d(file->getInsertX(), file->getInsertY()));
            takayaPcbFile->setRotationRadian(file->getRotationRadians());
            takayaPcbFile->setMirrored(file->isMirrored());

            m_pcbDesignList.AddTail(takayaPcbFile);
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
   //         TakayaFPTPcbFile* takayaPcbFile = new TakayaFPTPcbFile();
   //         takayaPcbFile->setName(pcbInsert->getRefname());
   //         takayaPcbFile->setBlockNumber(pcbInsert->getBlockNumber());
   //         takayaPcbFile->setOrigin(pcbInsert->getOrigin2d());
   //         takayaPcbFile->setRotationRadian(pcbInsert->getAngleRadians());
   //         takayaPcbFile->setMirrored(pcbInsert->getGraphicMirrored());

   //         m_pcbDesignList.AddTail(takayaPcbFile);
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
   //         TakayaFPTPcbFile* takayaPcbFile = new TakayaFPTPcbFile();
   //         takayaPcbFile->setName(file->getName());
   //         takayaPcbFile->setBlockNumber(file->getBlock()->getBlockNumber());
   //         takayaPcbFile->setOrigin(CPoint2d(file->getInsertX(), file->getInsertY()));
   //         takayaPcbFile->setRotationRadian(file->getRotationRadians());
   //         takayaPcbFile->setMirrored(file->isMirrored());

   //         m_pcbDesignList.AddTail(takayaPcbFile);
   //      }
   //   }
   //}

   return m_pcbDesignList.GetCount()>0;
}

void TakayaFPTWrite::loadSettings()
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

   CString settingsFile( getApp().getExportSettingsFilePath("takaya.out") );
   {
      CString msg;
      msg.Format("Takaya: Settings file [%s].\n\n", settingsFile);
      getApp().LogMessage(msg);
   }
   
   if ((fp = fopen(settingsFile, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", settingsFile);
      ErrorMessage(tmp, "TAKAYA Settings", MB_OK | MB_ICONHAND);
      return;
   }   

   while (fgets(line, 255, fp))
   {
      if ((lp = strtok(line, " \t\n")) == NULL)
         continue;

      if (lp[0] == '.')
      {
         if (!STRICMP(lp, ".EXPORT_ALL_ACCESS_POINTS"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
            
            m_settings.setExportAllAccessPoints( (toupper(lp[0]) == 'Y') );
         }
         else if (!STRICMP(lp, ".ORIGIN_MODE"))
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
			else if (!STRICMP(lp, ".KELVIN"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
            m_settings.setKelvinThreshold(atof(lp));
			}
			else if (!STRICMP(lp, ".SHORTS"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
            m_settings.setShortType(shortTypeStringToTag(lp));
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

void TakayaFPTWrite::loadModelTest()
{
   m_aptModelMap.empty();

   FILE *fp;
   char line[255];
   char *lp;

   if ((fp = fopen(m_settings.getTestLibraryFullPathName(), "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("The file %s is not found at the path specified by the takaya.out file command \"TAKAYA_LIBRARY\".", m_settings.getTestLibraryFullPathName());
      ErrorMessage(tmp, "TAKAYA.Out File Warning ", MB_OK | MB_ICONHAND);
      return;
   }

	TakayaFPTAptModel* aptModel = NULL;
   while (fgets(line, 255, fp))
   {
      if ((lp = strtok(line, " =\t\n")) == NULL)
         continue;

		if (lp[0] == ';')	// indicate a comment
			continue;

		if (!STRICMP(lp, "MODEL"))
		{
			if ((lp = strtok(NULL, " \"\t\n")) == NULL)
				continue;
			CString modelName = lp;

			aptModel = m_aptModelMap.add(modelName);
		}
		else if (!STRICMP(lp, ":EOD"))
		{
			// Reset aptModel
			aptModel = NULL;
		}
		else if (lp[0] == '<')
		{
			if (aptModel == NULL)
				continue;

			// Anything input that starts with "<" means it requires value from CAMCAD for that parament.
			// Therefore, set the parament to empty first and if the imput doesn't starts with "<"
			// then assign the input to the parament; otherwise, parament will get assign later on
			CString refdes = "";
			CString value = "";
			CString comment = "";
			CString location = "";
			CString element = "";
			CString option = "";
         CStringArray pinNums;

         // Fields 1 to 5 are always the same form.
         // Starting at Field 6 the Probe may be specified in a variety of forms.
         // After the Probes are "options", just toss 'em all into one options bucket.

			// Field 1 - Indicate a new test string
			if (lp[0] != '<')									// refdes
				refdes = lp;
         else
            refdes = TK_REF_REPLACEMENT;

         // Field 2
			if ((lp = strtok(NULL, " \t")) == NULL)	// value
				continue;
			if (lp[0] != '<')
				value = lp;

         // Field 3
			if ((lp = strtok(NULL, " \t")) == NULL)	// comment
				continue;
         comment = lp;

         // Field 4
         if ((lp = strtok(NULL, " \t")) == NULL)   // location
               continue;
         if (lp[0] != '<') // Why are we not saving the macro replacement for these?
            location = lp;

         // Field 5
         if ((lp = strtok(NULL, " \t")) == NULL)   // element - don't need to check, always take as it is
               continue;
         element = lp[0];  // aka Test Code


         // Pins aka Probes is a tricky one.
         // May be in variety of forms, e.g.
         // <REF> <VAL> RELAY24V IO/V R <PROBE:1> <PROBE:2> @P+:<PROBE:3> @P-:<PROBE:4> @IV 24 @K SH
         // <REF> <VAL> <REF>-1/<REF>-8 * R <PROBE:1,8> @T <TOL> <NTOL>;
         //
         // Having <PROBE:1,8> should work the same as having both <PROBE:1> <PROBE:8>

         CSupString processingStr;

         while (((lp = strtok(NULL, " \t\n")) != NULL))   // pin numbers string
         {
            processingStr = lp;
            processingStr.MakeUpper();
            if (processingStr.Find("PROBE") > -1)  // Is pin designating field, one or more pins follow
            {
               CStringArray params;
               processingStr.ParseQuote(params, "<:,>");
               for (int iparam = 0; iparam < params.GetCount(); iparam++)
               {
                  CString thisparam( params.GetAt(iparam) );
                  if (!thisparam.IsEmpty() && thisparam.Compare("PROBE") != 0)  // ignore the keyword, rest are pin nums
                  {
                     pinNums.Add(params.GetAt(iparam));
                  }
               }
            }
            else
            {
               // This one isn't a probe so stop loop. Leave lp as it is so next step starts
               // with current buffer.
               break;
            }
         }

         // Processing the probes left what ever comes after probe in lp, not yet processed.
         // Put the current lp and what ever is on rest of line in option.
         while (lp != NULL)
         {
            option += " " + CString(lp);
            lp = strtok(NULL, " \t\n");
         }


         // Format comment string
         // Remove the set of "<" and ">" and everything in between and replace with TK_REF_REPLACEMENT
         CString tmpComment = comment;
         comment = "";
         while (true)
         {
            int leftBracket = tmpComment.Find('<');
            int rightBracket = tmpComment.Find('>');

            if (leftBracket > -1 && rightBracket > -1)
            {
               comment.AppendFormat("%s%s", tmpComment.Left(leftBracket), TK_REF_REPLACEMENT);
               tmpComment.Delete(0, rightBracket + 1);
            }
            else
            {
               comment.AppendFormat("%s", tmpComment);
               break;
            }
         }


         // Format option string
         // Remove the 1st and 2nd set of "<" and ">" and everything in between and
         // replace with TK_PTOL_REPLACEMENT, and TK_NTOL_REPLACEMENT
         option.Trim();
         option.Replace("<TOL>", TK_PTOL_REPLACEMENT);
         option.Replace("<NTOL>", TK_NTOL_REPLACEMENT);


         // Create a new test and add it to the model
         TakayaFPTAptTest* aptTest = aptModel->createTest();
         aptTest->SetDeviceType(deviceTypeUnknown);
         aptTest->SetRefdes(refdes);
         aptTest->SetValue(value);
         aptTest->SetCommment(comment);
         aptTest->SetLocation(location);
         aptTest->SetElement(element);
         aptTest->SetPinNum1(pinNums.GetCount() > 0 ? pinNums.GetAt(0) : "");
         aptTest->SetPinNum2(pinNums.GetCount() > 1 ? pinNums.GetAt(1) : "");
         aptTest->SetPinNum3(pinNums.GetCount() > 2 ? pinNums.GetAt(2) : "");
         aptTest->SetPinNum4(pinNums.GetCount() > 3 ? pinNums.GetAt(3) : "");
         aptTest->SetOption(option);
		}
		else if (lp[0] == '@')
		{
			if (aptModel == NULL)
				continue;

			// Anything input that starts with "<" means it requires value from CAMCAD for that parament.
			// Therefore, set the parament to empty first and if the imput doesn't starts with "<"
			// then assign the input to the parament; otherwise, parament will get assign later on

         int deviceType = deviceTypeUnknown;
         if (!STRICMP(lp, "@PC"))
            deviceType = deviceTypeOpto;
         else if (!STRICMP(lp, "@Q2"))
            deviceType = deviceTypeTransistor;
         else if (!STRICMP(lp, "@F"))
            deviceType = deviceTypeTransistorFetNpn;

			CString refdes, location, option, pinNumString;
         CString comment = TK_PART_REPLACEMENT;

			if ((lp = strtok(NULL, " \t")) == NULL)	// refdes
            continue;
			refdes = lp;

			if ((lp = strtok(NULL, " \t")) == NULL)	// value
				continue;
			if (lp[0] != '<')
				comment = lp;

			if ((lp = strtok(NULL, " \t")) == NULL)	// location
				continue;
			if (lp[0] != '<')
				location = lp;

			if ((lp = strtok(NULL, " \t")) == NULL)	// pin numbers string
				continue;
			pinNumString = lp;

			if ((lp = strtok(NULL, "\n")) != NULL)	// option
         {
   			option = lp;
            option.Trim();
         }

			// Format refdes string
			// Remove the set of "<" and ">" and everything in between and replace with TK_REF_REPLACEMENT
			CString tmpRef = refdes;
			refdes = "";
			while (true)
			{
				int leftBracket = tmpRef.Find('<');
				int rightBracket = tmpRef.Find('>');
				
				if (leftBracket > -1 && rightBracket > -1)
				{
					refdes.AppendFormat("%s%s", tmpRef.Left(leftBracket), TK_REF_REPLACEMENT);
					tmpRef.Delete(0, rightBracket + 1);
				}
				else
				{
					refdes.AppendFormat("%s", tmpRef);
					break;
				}
			}
			
			// Format option string
			// Remove the 1st and 2nd set of "<" and ">" and everything in between and
			// replace with TK_PTOL_REPLACEMENT, and TK_NTOL_REPLACEMENT
			option.Replace("<TOL>", TK_PTOL_REPLACEMENT);
			option.Replace("<NTOL>", TK_NTOL_REPLACEMENT);

			CString pinNum1, pinNum2, pinNum3, pinNum4;
			if ((lp = strtok(pinNumString.GetBuffer(0), ":")) == NULL)
				continue;
			if ((lp = strtok(NULL, ">")) == NULL)
				continue;

         CStringArray params;
         CSupString pinNumSupString(lp);
         pinNumSupString.Parse(params, ",");

         if (params.GetCount() > 0)
            pinNum1 = params.GetAt(0);
         
         if (params.GetCount() > 1)
            pinNum2 = params.GetAt(1);

         if (params.GetCount() > 2)
            pinNum3 = params.GetAt(2);

         if (params.GetCount() > 3)
            pinNum4 = params.GetAt(3);

			// Create a new test and add it to the model
			TakayaFPTAptTest* aptTest = aptModel->createTest();
         aptTest->SetDeviceType(deviceType);
			aptTest->SetRefdes(refdes);
			aptTest->SetValue("");
			aptTest->SetCommment(comment);
			aptTest->SetLocation(location);
			aptTest->SetElement("");
			aptTest->SetPinNum1(pinNum1);
			aptTest->SetPinNum2(pinNum2);
			aptTest->SetPinNum3(pinNum3);
			aptTest->SetPinNum4(pinNum4);
			aptTest->SetOption(option);
		}
	}
	aptModel = NULL;	// to avoid aptModel being destroy by the destructor when it goes out of scope

	fclose(fp);
	return ;
}

bool TakayaFPTWrite::loadTakayaData(FileStruct& pcbFile)
{
   bool retval = false;
   CTMatrix matrix = pcbFile.getTMatrix();

   // Need to empty they before loading new data
   m_netMap.empty();
   m_compPinMap.empty();
   m_componentCollection.empty();
   m_groundNet = NULL;

   // This map is for temperary use to hold maps of compPins by component name
   CTypedMapStringToPtrContainer<TakayaFPTCompPinMap*> compPinByCompNameMap(nextPrime2n(20), true);

   for (POSITION pos=pcbFile.getHeadNetPosition(); pos!=NULL;)
   {
      NetStruct* netStruct = pcbFile.getNextNet(pos);
      //dts0100596922  Process all nets, both used and unused
      if (netStruct != NULL)
      {
         TakayaFPTNet* net = m_netMap.addNet(*netStruct);
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

         // Create TakayaFPTCompPinMap
         for (POSITION compPinPos=netStruct->getHeadCompPinPosition(); compPinPos!=NULL;)
         {
            CompPinStruct* compPinStruct = netStruct->getNextCompPin(compPinPos);
            if (compPinStruct != NULL)
            {
               TakayaFPTCompPin* compPin =  m_compPinMap.addCompPin(*compPinStruct, *net);
               if (compPin != NULL)
               {
                  if (compPinStruct->getAttributes() !=  NULL)
                  {
                     CString pinMap;
                     m_camCadData.getAttributeStringValue(pinMap, *compPinStruct->getAttributes(), standardAttributeDeviceToPackagePinMap);
                     compPin->setPinMap(pinMap);
                  }

                  TakayaFPTCompPinMap* compPinMap = NULL;
                  if (!compPinByCompNameMap.Lookup(compPin->getCompName(), compPinMap))
                  {
                     compPinMap = new TakayaFPTCompPinMap(false);
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
         TakayaFPTComponent* component = m_componentCollection.addComponent(m_camCadDoc, m_camCadData, pcbFile, *data, m_settings, matrix);
         if (component != NULL)
         {
            // Add TakayaFPTCompPin
            TakayaFPTCompPinMap* compPinMap = NULL;
            if (compPinByCompNameMap.Lookup(component->getName(), compPinMap))
            {
               for(POSITION pos=compPinMap->getStartPosition(); pos!=NULL;)
               {
                  TakayaFPTCompPin* compPin = compPinMap->getNext(pos);
                  component->addCompPin(compPin);
               }
            }
         }
      }
      else if (insert->getInsertType() == insertTypeTestAccessPoint && data->getAttributes() != NULL)
      {
         // Only create TakayaFPTTestAccess if test access point has NETNAME attribute
         CString stringValue;          
         if (m_camCadData.getAttributeStringValue(stringValue, *data->getAttributes(), standardAttributeNetName))
         {
            TakayaFPTNet* net = m_netMap.findNet(stringValue);
            if (net != NULL)
            {
               // Get origin at board level
               CPoint2d testAccessOrigin = insert->getOrigin2d();
               matrix.transform(testAccessOrigin);               
               this->m_componentCollection.convertToTakayaUnits(testAccessOrigin, m_unitFactor, insert->getPlacedTop());

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

               // Create TakayaFPTTestAccess
               TakayaFPTTestAccess* testAccess = net->addTestAccess(data->getEntityNumber(), insert->getRefname(), testAccessOrigin, 
                     insert->getGraphicMirrored()?surfaceBottom:surfaceTop, hasTakayaAccessAttrib, targetTypePriority, targetEntityNumber);
            }
         }

         // set to TRUE as long as at least one Test Access Point is found
         retval = true;
      }
   }

   return retval;
}

void TakayaFPTWrite::writeTakayaOutput(const CString fileName, CStringArray& outputArray, TakayaFPTProbeOutputCollection& probeOutputCollection, const TakayaFPTSurface testSurface)
{
   CStdioFileWriteFormat* file = new CStdioFileWriteFormat();
   file->open(fileName);

   // Write header
   CTime curTime;
	curTime = curTime.GetCurrentTime();
   file->writef("// Generated by %s on %s\n", getApp().getCamCadSubtitle(), curTime.Format("%A, %B %d, %Y at %H:%M:%S"));

   // Write board names
   CString boardNames;
   for (POSITION pos=m_pcbDesignList.GetHeadPosition(); pos!=NULL;)
   {
      TakayaFPTPcbFile* pcbFile = m_pcbDesignList.GetNext(pos);
      if (pcbFile != NULL)
      {
         boardNames.AppendFormat("%s ", pcbFile->getName());
      }
   }
   file->writef("// Board Name: %s\n\n", boardNames);
   file->writef("@M %s\n", boardNames);

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

   // Probes and Access Marks
   if(testSurface == surfaceTop)
      DeleteAllProbes(&m_camCadDoc, m_camCadData.getFileList().GetOnlyShown(blockTypePcb));
   m_camCadDoc.PrepareAddEntity(m_camCadData.getFileList().GetOnlyShown(blockTypePcb));

   writeProbeAccessPoints(file, probeOutputCollection, testSurface);

   // All done.
   file->close();
}

void TakayaFPTWrite::writeProbeAccessPoints(CStdioFileWriteFormat* file, TakayaFPTProbeOutputCollection& probeOutputCollection, const TakayaFPTSurface testSurface)
{
   // Always write accesspoints that have probes, based on probeOutputCollection.

   // Mark sectino start
   file->write("@N\n");

   int probeOutputCount = probeOutputCollection.getCount();
   int highestProbeNumber = -1;

   probeOutputCollection.SortByProbeNumber();
   for (int i= 0; i<probeOutputCollection.getCount(); i++)
   {
      TakayaFPTProbeOutput* probeOutput = probeOutputCollection.getAt(i);
      if (probeOutput != NULL)
      {
         CPoint2d fptorigin = probeOutput->getInsertOrigin();
         if (testSurface == surfaceBottom)
            fptorigin.x = -fptorigin.x;

         if (probeOutput->getProbeNumber() > highestProbeNumber)
            highestProbeNumber = probeOutput->getProbeNumber();

         file->writef("%d: %.0f, %.0f, %s\n", probeOutput->getProbeNumber(), fptorigin.x, fptorigin.y, probeOutput->getNetName());

         updateProbeInCCZ(probeOutput);
      }
   }

#ifdef DEADCODE
   //rcf don't need this now because the probes are all in the output collection

   // Optionally write access points with no probe
   if (m_settings.getExportAllAccessPoints())
   {
      for (POSITION pos=m_netMap.GetStartPosition(); pos!=NULL;)
      {
         CString key;
         TakayaFPTNet* fptNet = NULL;
         m_netMap.GetNextAssoc(pos, key, fptNet);
         if (fptNet != NULL)
         {
            CString netName( fptNet->getName() );

            TakayaFPTTestAccessCollection &accessCollection = fptNet->getTestAccessCollection(testSurface);
            TakayaFPTTestAccess *fptTestAccess = accessCollection.GetFirstTestAccess();
            while (fptTestAccess != NULL)
            {
               // Process current one
               TakayaFPTSurface testAccessSurface = fptTestAccess->getSurface();
               if (testAccessSurface == testSurface)
               {
                  // If not probed then count it.
                  if (fptTestAccess->getUsedCount() < 1)
                  {
                     CPoint2d fptorigin = fptTestAccess->getInsertOrigin();
                     file->writef("%d: %.0f, %.0f, %s\n", ++highestProbeNumber, fptorigin.x, fptorigin.y, netName);
                  }
               }

               // Get next one
               fptTestAccess = accessCollection.GetNextTestAccess();
            }
            
         }
      }
   }
#endif

   // Mark section end
   file->write("@CE\n");
}

int TakayaFPTWrite::createProbesForUnusedAccessMarks(const TakayaFPTSurface testSurface)
{
   // Returns number of unused access markers found.
   // Creates a probe on each.

   int unusedAccessCount = 0;

   for (POSITION pos=m_netMap.GetStartPosition(); pos!=NULL;)
   {
      CString key;
      TakayaFPTNet* fptNet = NULL;
      m_netMap.GetNextAssoc(pos, key, fptNet);
      if (fptNet != NULL)
      {
         CString netName( fptNet->getName() );

         TakayaFPTTestAccessCollection &accessCollection = fptNet->getTestAccessCollection(testSurface);
         TakayaFPTTestAccess *fptTestAccess = accessCollection.GetFirstTestAccess();
         while (fptTestAccess != NULL)
         {
            // Process current one
            TakayaFPTSurface testAccessSurface = fptTestAccess->getSurface();
            if (testAccessSurface == testSurface)
            {
               // If not probed then write it/count it.
               if (fptTestAccess->getUsedCount() < 1)
               {
                  unusedAccessCount++;

                  // Getting a probe number for it causes it to get put in the output list.
                 if (testSurface == surfaceTop)
                    m_topProbeOutputCollection.getProbeNumberString(fptTestAccess);
                 else
                    m_bottomProbeOutputCollection.getProbeNumberString(fptTestAccess);
               }
            }

            // Get next one
            fptTestAccess = accessCollection.GetNextTestAccess();
         }
      }
   }

   return unusedAccessCount;
}

DataStruct *TakayaFPTProbeOutput::getTestAccessData(CCamCadData &camCadData, int accessEntityNumber)
{
   DataStruct* testAccess = NULL;

   CEntity entity = CEntity::findEntity(camCadData, accessEntityNumber, entityTypeData);
   if (entity.getEntityType() == entityTypeData)
   {
      testAccess = entity.getData();

      // If it is not a test access mark insert then reject it.
      if (testAccess != NULL && !testAccess->isInsertType(insertTypeTestAccessPoint))
         testAccess = NULL;
   }

   return testAccess;
}


void TakayaFPTWrite::updateProbeInCCZ(TakayaFPTProbeOutput* probeOutput)
{
   if (probeOutput != NULL)
   {
      // Update probes on CamcadDoc
      CEntity entity = CEntity::findEntity(m_camCadDoc.getCamCadData(), probeOutput->getTestAccessEntityNumber(), entityTypeData);
      if (entity.getEntityType() == entityTypeData)
      {

         DataStruct* testAccess = entity.getData();
         if (testAccess->getInsert() != NULL && testAccess->getInsert()->getInsertType() == insertTypeTestAccessPoint)
         {
            Attrib* attrib = NULL;
            double testAccessFeatureSize = 0.0;
            if (attrib = is_attvalue(&m_camCadDoc, testAccess->getAttributes(), DFT_ATT_EXPOSE_METAL_DIAMETER, 0))
               testAccessFeatureSize = attrib->getDoubleValue();

            CString probeBlockName;
            probeBlockName.Format("%0.3f_%s", testAccessFeatureSize, (testAccess->getInsert()->getPlacedBottom() == 0)?"Top":"Bottom");
            BlockStruct* probeBlock = probeBlock = CreateTestProbeGeometry(&m_camCadDoc, probeBlockName, testAccessFeatureSize, probeBlockName, testAccessFeatureSize);

            CString probeName, probeRefname;
            probeName.Format("N%d_%s", probeOutput->getProbeNumber(), (testAccess->getInsert()->getPlacedBottom() == 0)?"Top":"Bottom");
            probeRefname.Format("N%d", probeOutput->getProbeNumber());
            DataStruct *probeData = Graph_Block_Reference(probeBlock->getName(), probeName, probeBlock->getFileNumber(), 
               testAccess->getInsert()->getOriginX(), testAccess->getInsert()->getOriginY(), 0, (testAccess->getInsert()->getPlacedBottom() == 0)?FALSE:TRUE, 1.0, -1, FALSE);
            probeData->getInsert()->setInsertType(insertTypeTestProbe);
            probeData->setHidden(false);

            //*rcf Would be more efficient to move these to TakayaFPTWrite class, to get once and cache
            int probePlacementKw = m_camCadDoc.getStandardAttributeKeywordIndex(standardAttributeProbePlacement);
            int dataLinkKw = m_camCadDoc.getStandardAttributeKeywordIndex(standardAttributeDataLink);
            int netNameKw = m_camCadDoc.getStandardAttributeKeywordIndex(standardAttributeNetName);
            int refnameKW = m_camCadDoc.getStandardAttributeKeywordIndex(standardAttributeRefName);

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
}

void TakayaFPTWrite::generateProbablyShortTest(FileStruct& fileStruct, FILE *logFp, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, const TakayaFPTSurface testSurface)
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

         TakayaFPTNet* net1 = m_netMap.findNet(netName1);
         TakayaFPTNet* net2 = m_netMap.findNet(netName2);

         CString netPair;
         if (net1 != NULL && net2 != NULL && net1 != net2 &&
             !m_probablyShortNetPairMap.Lookup(netName1+netName2, netPair) &&
             !m_probablyShortNetPairMap.Lookup(netName2+netName1, netPair))
         {
            TakayaFPTTestAccess* testAccess1 = net1->getTestAccess(logFp, m_settings.getMaxHitCount(), testSurface);
            TakayaFPTTestAccess* testAccess2 = net2->getTestAccess(logFp, m_settings.getMaxHitCount(), testSurface);

            if (testAccess1 != NULL && testAccess2 != NULL)
            {
               if (compPinRef1.IsEmpty())
                  compPinRef1 = getCompPinRefForProbableShort(net1->getNetStruct());
               if (compPinRef2.IsEmpty())
                  compPinRef2 = getCompPinRefForProbableShort(net2->getNetStruct());

               CString probeNumber1 = probeOutputCollection.getProbeNumberString(*testAccess1);
               CString probeNumber2 = probeOutputCollection.getProbeNumberString(*testAccess2);

               CString output;
               output.Format(" %s %s * * R %s %s @K OP", compPinRef1, compPinRef2, probeNumber1, probeNumber2);
               outputArray.Add(output);

               m_probablyShortNetPairMap.SetAt(netName1+netName2, netName1+netName2);
               m_probablyShortNetPairMap.SetAt(netName2+netName1, netName2+netName1);
            }
         }       
      }
   }
}

CString TakayaFPTWrite::getCompPinRefForProbableShort(NetStruct& netStruct)
{
   CString compPinRef;

   for (POSITION pos=netStruct.getHeadCompPinPosition(); pos!=NULL;)
   {
      CompPinStruct* compPin = netStruct.getNextCompPin(pos);
      if (compPin != NULL)
      {
         TakayaFPTComponent* component = m_componentCollection.findComponent(compPin->getRefDes());
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


