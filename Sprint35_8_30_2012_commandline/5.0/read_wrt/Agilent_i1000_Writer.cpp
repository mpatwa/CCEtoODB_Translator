// $Header: /CAMCAD/4.6/read_wrt/Agilent_i1000_Writer.cpp 31    6/06/07 6:59p Rick Faltersack $



// Agilent i1000 Writer

#include "stdafx.h"
#include "CCEtoODB.h"
#include "Port.h"
#include "PolyLib.h"
#include "Find.h"
#include "Net_Util.h"
#include "CompValues.h"
#include "SettingsFile.h"
#include "Agilent_i1000_Writer.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// For vectoring out arcs
#define MIN_ARC_ANGLE_DEGREES  5.0


void Agilent_i1000_WriteFiles     (CString     filename, CCEtoODBDoc &doc, FormatStruct *format)
{

   FileStruct *pcbFile = doc.getFileList().GetOnlyShown(blockTypePcb);

   if (pcbFile == NULL)
   {
      ErrorMessage("A single PCB File must be visible for Agilent i1000 Export", "Agilent i1000 Export");
   }
   else
   {
      CAgilentI1000Writer i1000Writer(&doc, pcbFile);
      i1000Writer.WriteFiles(filename);
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////

CAgI1000Part::CAgI1000Part(CCEtoODBDoc *doc, FileStruct *pcbFile, DataStruct *camcadPartData)
: m_camcadPartData(camcadPartData)
, m_doc(doc)
, m_pcbFile(pcbFile)
, m_anodePin(NULL)
, m_cathodePin(NULL)
, m_basePin(NULL)
, m_collectorPin(NULL)
, m_emitterPin(NULL)
, m_anodeNet(NULL)
, m_cathodeNet(NULL)
, m_baseNet(NULL)
, m_collectorNet(NULL)
, m_emitterNet(NULL)
{
   m_deviceType = deviceTypeUnknown;
   if (doc != NULL && pcbFile != NULL && m_camcadPartData != NULL)
   {
      Attrib *attr = NULL;
      WORD devTypeKW = doc->getStandardAttributeKeywordIndex(standardAttributeDeviceType);
      if (m_camcadPartData->lookUpAttrib(devTypeKW, attr))
         m_deviceType = stringToDeviceTypeTag( get_attvalue_string(doc, attr) );

      switch (m_deviceType)
      {
      case deviceTypeTransistor:
      case deviceTypeTransistorNpn:
      case deviceTypeTransistorPnp:
         m_basePin = FindCompPinAndNet(pinTypeBase, m_baseNet);
         m_collectorPin = FindCompPinAndNet(pinTypeCollector, m_collectorNet);
         m_emitterPin = FindCompPinAndNet(pinTypeEmitter, m_emitterNet);
         break;

      default:
         // For parts like resistor we treat pin 1 as anode pin and pin 2 as cathode pin.
         // For diodes and such, they are what they say they are.
         m_anodePin =   FindCompPinAndNet(pinTypeAnode,   m_anodeNet);
         m_cathodePin = FindCompPinAndNet(pinTypeCathode, m_cathodeNet);
         break;
      }
      

      m_pinMap.CollectPins(m_doc, m_pcbFile, m_camcadPartData);
   }
}

//-----------------------------------------------------------------------------------------

CompPinStruct *CAgI1000Part::FindCompPinAndNet(PinTypeTag desiredPinType, NetStruct *&net)
{
   net = NULL;

   CompPinStruct *cpPin1 = NULL;
   CompPinStruct *cpPin2 = NULL;
   CompPinStruct *cpFirstPin = NULL;
   CompPinStruct *cpSecondPin = NULL;

   NetStruct *cpPin1Net = NULL;
   NetStruct *cpPin2Net = NULL;
   NetStruct *cpFirstPinNet = NULL;
   NetStruct *cpSecondPinNet = NULL;

   BlockStruct *insertedBlk = m_doc->getBlockAt(m_camcadPartData->getInsert()->getBlockNumber());

   if (insertedBlk != NULL)
   {
      for (POSITION pos = insertedBlk->getHeadDataInsertPosition(); pos != NULL; insertedBlk->getNextDataInsert(pos))
      {
         DataStruct *pinData = insertedBlk->getAtData(pos);
         if (pinData->isInsertType(insertTypePin))
         {
            CString pinName = pinData->getInsert()->getRefname();

		      NetStruct *cpnet = NULL;
            CompPinStruct *cp = NULL;
 
            if ((cp = FindCompPin(m_pcbFile, m_camcadPartData->getInsert()->getRefname(), pinName, &cpnet)) != NULL)
            {
               // Gather info for possible default settings later.
               if (pinName.Compare("1") == 0)
               {
                  cpPin1 = cp;
                  cpPin1Net = cpnet;
               }
               if (pinName.Compare("2") == 0)
               {
                  cpPin2 = cp;
                  cpPin2Net = cpnet;
               }

               if (cpFirstPin == NULL)
               {
                  cpFirstPin = cp;
                  cpFirstPinNet = cpnet;
               }
               else if (cpSecondPin == NULL)
               {
                  cpSecondPin = cp;
                  cpSecondPinNet = cpnet;
               }


               WORD pinMapKW = m_doc->getStandardAttributeKeywordIndex(standardAttributeDeviceToPackagePinMap);
			      Attrib *attrib = NULL;
			      if (cp->getAttributesRef() != NULL && cp->getAttributesRef()->Lookup(pinMapKW, attrib))
			      {
				      CString pinTypeStr = get_attvalue_string(m_doc, attrib);
				      PinTypeTag thisPinType = stringToPinTypeTag(pinTypeStr);

                  if (thisPinType == desiredPinType)
                  {
                     net = cpnet;
                     return cp;
                  }
			      }
            }
         }
      }

      // Default, only applies for anode and cathode.
      // No pin of desired type found, return a default.
      // E.g. Resistors don't have anodes, if asking for an anode on Resistor we want pin 1.
      if (desiredPinType == pinTypeAnode)
      {
         if (cpPin1 != NULL)
         {
            net = cpPin1Net;
            return cpPin1;
         }
         if (cpFirstPin != NULL)
         {
            net = cpFirstPinNet;
            return cpFirstPin;
         }
      }
      if (desiredPinType == pinTypeCathode)
      {
         if (cpPin2 != NULL)
         {
            net = cpPin2Net;
            return cpPin2;
         }
         if (cpSecondPin != NULL)
         {
            net = cpSecondPinNet;
            return cpSecondPin;
         }
      }
   }

   return NULL;
}

//-----------------------------------------------------------------------------------------

CompPinStruct *CAgI1000Part::GetCompPin(PinTypeTag pinType)
{
   switch (pinType)
   {
   case pinTypeAnode:
      return m_anodePin;
   case pinTypeCathode:
      return m_cathodePin;
   case pinTypeBase:
      return m_basePin;
   case pinTypeCollector:
      return m_collectorPin;
   case pinTypeEmitter:
      return m_emitterPin;
   }
   
   return NULL;
}

NetStruct *CAgI1000Part::GetPinNet(PinTypeTag pinType)
{
   switch (pinType)
   {
   case pinTypeAnode:
      return m_anodeNet;
   case pinTypeCathode:
      return m_cathodeNet;
   case pinTypeBase:
      return m_baseNet;
   case pinTypeCollector:
      return m_collectorNet;
   case pinTypeEmitter:
      return m_emitterNet;
   }
   
   return NULL;
}

//-----------------------------------------------------------------------------------------

CString CAgI1000Part::GetGridLocation()
{

   CString value;
   Attrib* attrib = NULL;
   WORD kw = m_doc->getStandardAttributeKeywordIndex(standardAttributeGridLocation);

   if (m_camcadPartData->lookUpAttrib(kw, attrib))
      value = attrib->getStringValue();

   return value;
}

CString CAgI1000Part::GetValueAttrib()
{
   CString value;
   Attrib* attrib = NULL;
   WORD kw = m_doc->getStandardAttributeKeywordIndex(standardAttributeValue);

   if (m_camcadPartData->lookUpAttrib(kw, attrib))
      value = attrib->getStringValue();

   return value;
}

CString CAgI1000Part::GetValue()
{
   CString val = GetValueAttrib();

   if (val.IsEmpty())
      val = "0";

   switch (this->m_deviceType)
   {
   case deviceTypeResistor:
      val += "o"; // add little o for ohms, to match customer's example
      break;

   case deviceTypeTransistor:
   case deviceTypeTransistorNpn:
   case deviceTypeTransistorPnp:
      if (val.Compare("0") == 0)
         val = "0.7V";
      break;

   case deviceTypeJumper:
      {
         ComponentValues compValue(val);
         double numericVal = compValue.GetValue();
         if (numericVal < 10.0)
            val = "1JP";
         else if (numericVal < 20.0)
            val = "2JP";
         else if (numericVal < 40.0)
            val = "3JP";
         else
            val = "4JP";
      }
      break;
   }

   return val;
}

CString CAgI1000Part::GetPlusToleranceAttrib()
{
   CString plusTol("0%");
   Attrib* attrib = NULL;
   WORD kw = m_doc->getStandardAttributeKeywordIndex(standardAttributePlusTolerance);

   if (m_camcadPartData->lookUpAttrib(kw, attrib))
      plusTol.Format("%d%%", round(fabs(attrib->getDoubleValue())));

   return plusTol;
}

CString CAgI1000Part::GetPlusTolerance()
{
   if (this->GetDeviceType() == deviceTypeJumper)
      return "F";

   CString val = GetPlusToleranceAttrib();

   switch (this->m_deviceType)
   {
   case deviceTypeTransistor:
   case deviceTypeTransistorNpn:
   case deviceTypeTransistorPnp:
      if (val.Compare("0%") == 0)
         val = "10%";
      break;
   }

   return val;
}

CString CAgI1000Part::GetMinusToleranceAttrib()
{
   CString minusTol = GetPlusTolerance();
   Attrib* attrib = NULL;
   WORD kw = m_doc->getStandardAttributeKeywordIndex(standardAttributeMinusTolerance);

   if (m_camcadPartData->lookUpAttrib(kw, attrib))
      minusTol.Format("%d%%", round(fabs(attrib->getDoubleValue())));

   switch (this->m_deviceType)
   {
   case deviceTypeTransistor:
   case deviceTypeTransistorNpn:
   case deviceTypeTransistorPnp:
      if (minusTol.Compare("0%") == 0)
         minusTol = "10%";
      break;
   }

   return minusTol;
}

CString CAgI1000Part::GetMinusTolerance()
{
   if (this->GetDeviceType() == deviceTypeJumper)
      return "F";

   return GetMinusToleranceAttrib();
}

CString CAgI1000Part::GetCapacitiveOpensAttrib()
{

   CString value;
   Attrib* attrib = NULL;
   WORD kw = m_doc->RegisterKeyWord(ATT_CAP_OPENS, 0, VT_STRING);
   // There is not Cap Opens standard attrib keyword index defined, so it is being done
   // the old way, above. Development is in 4.6 for this exporter, so the common code
   // is not going to be expanded at this time for Cap Opens. In 5.0 code this "new"
   // standard attrib should be added. It is used in Data Doctor, so I think that qualifies
   // it as "standard enough".
   ///WORD kw = m_doc->getStandardAttributeKeywordIndex(standardAttributeCapacitiveOpens);

   if (m_camcadPartData->lookUpAttrib(kw, attrib))
      value = attrib->getStringValue();

   return value;
}

bool CAgI1000Part::TestCapacitiveOpens()
{
   CString val = GetCapacitiveOpensAttrib();

   return (val.CompareNoCase("true") == 0);

}

CString CAgI1000Part::GetDescription(CString usersDescAttribName)
{
   CString kwName(ATT_PARTNUMBER);  // default

   if (!usersDescAttribName.IsEmpty())
      kwName = usersDescAttribName;

   CString desc = "";

   Attrib *attrib = NULL;
   int kw = m_doc->IsKeyWord(kwName, 0);
   if (kw >= 0 && this->GetCamcadPartData()->lookUpAttrib(kw, attrib))
   {
      if (attrib != NULL)
      {
         desc = get_attvalue_string(m_doc, attrib);
      }
   }

   return desc;
}


bool CAgI1000Part::IsSMD()
{
   CString val;
   WORD kw = m_doc->getStandardAttributeKeywordIndex( standardAttributeTechnology );

   Attrib *attrib = NULL;
            
   if (this->GetCamcadPartData()->lookUpAttrib(kw, attrib))
   {
      val = attrib->getStringValue();
   }

   return (val.CompareNoCase("SMD") == 0);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CAgI1000PartMap::CollectParts(CCEtoODBDoc *doc, FileStruct *pcbFile)
{
   this->empty();

   // Collect the parts
   if (doc != NULL && pcbFile != NULL && pcbFile->getBlock() != NULL)
   {
	   POSITION pos = pcbFile->getBlock()->getHeadDataInsertPosition();
	   while (pos != NULL)
      {
         DataStruct *datum = pcbFile->getBlock()->getNextDataInsert(pos);

         if (datum->isInsertType(insertTypePcbComponent)) // || datum->isInsertType(insertTypeTestPoint))
         {
            CString refname = datum->getInsert()->getRefname();
            CString uniqueRefname = refname;
            int count = 1;
            LPCTSTR key;
            while (this->LookupKey(uniqueRefname, key))
            {
               uniqueRefname.Format("%s_%d", refname, count++);
            }

            CAgI1000Part *part = new CAgI1000Part(doc, pcbFile, datum);
            this->SetAt(uniqueRefname, part);
         }
      }
   }

   // Sort the parts by ascending refname
   this->setSortFunction(&CAgI1000PartMap::AscendingRefnameSortFunc);
   this->Sort();

}

//-----------------------------------------------------------------------------------------

int CAgI1000PartMap::AscendingRefnameSortFunc(const void *a, const void *b)
{
   CAgI1000Part* itemA = (CAgI1000Part*)(((SElement*) a )->pObject->m_object);
   CAgI1000Part* itemB = (CAgI1000Part*)(((SElement*) b )->pObject->m_object);

   InsertStruct *insA = itemA->GetCamcadPartData()->getInsert();
   InsertStruct *insB = itemB->GetCamcadPartData()->getInsert();

   return (insA->getSortableRefDes().CompareNoCase(insB->getSortableRefDes()));
}

/////////////////////////////////////////////////////////////////////////////////////////////

bool CAgI1000Via::IsSMD()
{
   CString val;
   WORD kw = m_doc->getStandardAttributeKeywordIndex( standardAttributeTechnology );

   Attrib *attrib = NULL;
            
   if (this->GetCamcadViaData()->lookUpAttrib(kw, attrib))
   {
      val = attrib->getStringValue();
   }

   return (val.CompareNoCase("SMD") == 0);
}

//-----------------------------------------------------------------------------------------

void CAgI1000ViaMap::CollectVias(CCEtoODBDoc *doc, FileStruct *pcbFile, CAgI1000ProbeMap &probeMap)
{
   this->empty();

   CString *ignoredKey;
   CAgI1000Probe*probe;

   int viaCount = 0;

   for (probeMap.GetFirstSorted(ignoredKey, probe); probe != NULL; probeMap.GetNextSorted(ignoredKey, probe))
   {
      //Add bondpads
      if ((probe->GetProbedItemInsertType() == insertTypeVia || probe->GetProbedItemInsertType() == insertTypeBondPad)
         && probe->GetProbedItemData() != NULL)
      {
         viaCount++;
         
         CAgI1000Via *via = new CAgI1000Via(doc, pcbFile, probe->GetProbedItemData(), probe);

         CString viaKey;
         viaKey.Format("Via %d", viaCount);
         this->SetAt(viaKey, via);
      }
   }

   this->setSortFunction(&CAgI1000ViaMap::AscendingRefnameSortFunc);
   this->Sort();
}

//-----------------------------------------------------------------------------------------

int CAgI1000ViaMap::AscendingRefnameSortFunc(const void *a, const void *b)
{
   CAgI1000Via* itemA = (CAgI1000Via*)(((SElement*) a )->pObject->m_object);
   CAgI1000Via* itemB = (CAgI1000Via*)(((SElement*) b )->pObject->m_object);

   InsertStruct *insA = itemA->GetCamcadViaData()->getInsert();
   InsertStruct *insB = itemB->GetCamcadViaData()->getInsert();

   return (insA->getSortableRefDes().CompareNoCase(insB->getSortableRefDes()));
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

CAgI1000Probe::CAgI1000Probe(CCEtoODBDoc *doc, FileStruct *pcbFile, DataStruct *camcadPartData)
: m_camcadProbeData(camcadPartData)
, m_doc(doc)
, m_pcbFile(pcbFile)
, m_accessMarkerData(NULL)
, m_placed(false)
{

   if (m_doc != NULL && m_camcadProbeData != NULL)
   {
      // Get Access Marker
      int linkKW = m_doc->getStandardAttributeKeywordIndex(standardAttributeDataLink);

      Attrib *attrib;
      if (m_camcadProbeData->lookUpAttrib(linkKW, attrib))
      {
         CEntity accessEntity = CEntity::findEntity(m_doc->getCamCadData(), attrib->getIntValue());

         if (accessEntity.getEntityType() == entityTypeData)
         {
            m_accessMarkerData = accessEntity.getData();
         }
      }

      if (m_accessMarkerData != NULL && m_accessMarkerData->lookUpAttrib(linkKW, attrib))
      {
         m_probedEntity = CEntity::findEntity(m_doc->getCamCadData(), attrib->getIntValue());
      }

      // Determine if probe is placed
      WORD placementKw = m_doc->getStandardAttributeKeywordIndex(standardAttributeProbePlacement);
      Attrib *attr = NULL;
      if (m_camcadProbeData->lookUpAttrib(placementKw, attr))
      {
         CString placementVal = attr->getStringValue();
         m_placed = (placementVal.CompareNoCase("Placed") == 0);
      }
   }

}

void CAgI1000Probe::SetProbeNumber(int num)
{
   // Request came in to back fill assigned number to cc data.

   this->m_probeNumber = num;

   CString refname;
   refname.Format("%d", num);

   this->GetCamcadProbeData()->getInsert()->setRefname(refname);
}

CString CAgI1000Probe::GetProbedItemRefName()
{
   CString retval;

   if ((m_probedEntity.getEntityType() == entityTypeData) &&
      (m_probedEntity.getData()->getDataType() == dataTypeInsert))
   {
      retval = m_probedEntity.getData()->getInsert()->getRefname();
   }
   else if (m_probedEntity.getEntityType() == entityTypeCompPin)
   {
      retval = m_probedEntity.getCompPin()->getRefDes();
   }

   return retval;
}

CString CAgI1000Probe::GetProbedItemPinName()
{
   if (m_probedEntity.getEntityType() == entityTypeCompPin)
   {
      return m_probedEntity.getCompPin()->getPinName();
   }

   return "";
}

InsertTypeTag CAgI1000Probe::GetProbedItemInsertType() 
{
   if ((m_probedEntity.getEntityType() == entityTypeData) &&
      (m_probedEntity.getData()->getDataType() == dataTypeInsert))
   {
      return m_probedEntity.getData()->getInsert()->getInsertType();
   }

   return insertTypeUnknown;
}

CString CAgI1000Probe::GetGridLocation()
{

   CString value;
   Attrib* attrib = NULL;
   WORD kw = m_doc->getStandardAttributeKeywordIndex(standardAttributeGridLocation);

   if (m_camcadProbeData->lookUpAttrib(kw, attrib))
      value = attrib->getStringValue();

   return value;
}

CString CAgI1000Probe::GetNetNameAttrib(DataStruct *data)
{
   CString netname;
   WORD netnameKw = m_doc->getStandardAttributeKeywordIndex(standardAttributeNetName);

   Attrib *attrib = NULL;
            
   if (data->lookUpAttrib(netnameKw, attrib))
   {
      netname = attrib->getStringValue();
   }

   return netname;
}

CString CAgI1000Probe::GetNetNameAttrib(CompPinStruct *compPin)
{
   CString netname;
   WORD netnameKw = m_doc->getStandardAttributeKeywordIndex(standardAttributeNetName);

   Attrib *attrib = NULL;
            
   if (compPin->lookUpAttrib(netnameKw, attrib))
   {
      netname = attrib->getStringValue();
   }

   return netname;
}

CString CAgI1000Probe::GetProbedNetName()
{
   // Multi-level search for attrib netname.
   // Look on probe itself first, if not set look on access marker, if not set look on probed target.

   if (m_probedNetName.IsEmpty() && m_camcadProbeData != NULL)
   {
      WORD netNameKW = m_doc->getStandardAttributeKeywordIndex(standardAttributeNetName);
      WORD dlinkKW = m_doc->getStandardAttributeKeywordIndex(standardAttributeDataLink);

      m_probedNetName = this->GetNetNameAttrib( m_camcadProbeData );

      if (m_probedNetName.IsEmpty())
      {
         Attrib *a;
         if (m_camcadProbeData->lookUpAttrib(dlinkKW, a))
         {
            // Get access marker
            CEntity probedEntity = CEntity::findEntity(m_doc->getCamCadData(), a->getIntValue());

            if (probedEntity.getEntityType() == entityTypeData)
            {
               DataStruct* ta = probedEntity.getData();

               m_probedNetName = this->GetNetNameAttrib( ta );

               if (m_probedNetName.IsEmpty())
               {
                  // Get probed target
                  if (ta->lookUpAttrib(dlinkKW, a))
                  {
                     CEntity accessEntity = CEntity::findEntity(m_doc->getCamCadData(),a->getIntValue());

                     if (accessEntity.getEntityType() == entityTypeData)
                     {
                        DataStruct* feature = accessEntity.getData();
                        m_probedNetName = this->GetNetNameAttrib( feature );
                     }
                     else if (accessEntity.getEntityType() == entityTypeCompPin)
                     {
                        CompPinStruct *compPin = accessEntity.getCompPin();
                        m_probedNetName = this->GetNetNameAttrib( compPin );
                     }
                  }
               }
            }
         }
      }
   }

	return m_probedNetName;
}

CString CAgI1000Probe::GetProbedNetType()
{
   if (m_probedNetType.IsEmpty())
   {
      m_probedNetType = "SIGNAL"; // default

      NetStruct *net = FindNet(this->m_pcbFile, this->GetProbedNetName());

      if (net != NULL)
      {
         WORD netTypeKw = m_doc->getStandardAttributeKeywordIndex(standardAttributeNetType);
         Attrib *attrib = NULL;

         if (net->lookUpAttrib(netTypeKw, attrib))
         {
            CString camcadNetType = attrib->getStringValue();

            if (camcadNetType.CompareNoCase("POWER") == 0)
               m_probedNetType = "POWER";

            if (camcadNetType.CompareNoCase("GROUND") == 0)
               m_probedNetType = "GROUND";
         }
      }
   }

   return m_probedNetType;
}

/******************************************************************************
* CAgI1000Probe::IsProbedDiePin
*/
bool CAgI1000Probe::IsProbedDiePin()
{
   if (m_probedEntity.getEntityType() == entityTypeCompPin)
   {
      return m_probedEntity.getCompPin()->IsDiePin(m_doc->getCamCadData());
   }
   return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////

int CAgI1000ProbeMap::CollectProbes(CCEtoODBDoc *doc, FileStruct *pcbFile, bool collectPlaced, int startingProbeNumber)
{
   // Probe map is keyed by net name that is probed.
   // Multiple probes per net get encoded netnames to keep keys unique.

   this->empty();

   WORD placementKw = doc->getStandardAttributeKeywordIndex(standardAttributeProbePlacement);

   // Collect the probes
   if (doc != NULL && pcbFile != NULL && pcbFile->getBlock() != NULL)
   {
	   POSITION pos = pcbFile->getBlock()->getHeadDataInsertPosition();
	   while (pos != NULL)
      {
         DataStruct *datum = pcbFile->getBlock()->getNextDataInsert(pos);

         if (datum->isInsertType(insertTypeTestProbe))
         {
            bool placed = false;

            Attrib *attr = NULL;
            if (datum->lookUpAttrib(placementKw, attr))
            {
               CString placementVal = attr->getStringValue();
               placed = (placementVal.CompareNoCase("Placed") == 0);
            }

            if (placed == collectPlaced)
            {
               CAgI1000Probe *probe = new CAgI1000Probe(doc, pcbFile, datum);

               CString netname = probe->GetProbedNetName();

               if (!netname.IsEmpty())
               {
                  // First placed probe goes in map with plain netname as key.
                  // 2nd to nth and unplaced go in with special encoding to keep key unique.
                  // Use LookupNthProbe(..) to look them up.

                  CString refname = datum->getInsert()->getRefname();
                  int count = 1;
                  CString uniqueNetname = this->EncodeNetname(netname, count);

                  LPCTSTR key;
                  while (this->LookupKey(uniqueNetname, key))
                  {
                     uniqueNetname = this->EncodeNetname(netname, ++count);
                  }

                  this->SetAt(uniqueNetname, probe);
               }
               else
               {
                  delete probe;
               }
            }

         }
      }
   }

   int highestProbeNumber = AssignProbeNumbers(startingProbeNumber);

   // Sort the probes by ascending refname
   this->setSortFunction(&CAgI1000ProbeMap::AscendingRefnameSortFunc);
   this->Sort();

   // Original flavor tried to use original ccz probe numbers, but
   // renumbered in "validate" if any were alpha, cuz alpha is not allowed.
   // Subsequently, change request came in to number probes in a certain
   // fashion, see AssignProbeNumbers().
   // This is here for posterity, jic we want to go back.
   //ValidateProbeNumbers();

   return highestProbeNumber;
}

//-----------------------------------------------------------------------------------------

CString CAgI1000ProbeMap::EncodeNetname(CString netname, int probeNumber)
{
   // 1st probe gets no encoding, use net name as is.
   // 2nd to Nth get encoding, so all can live in same map.
   // The probeNumber is the ordinal number of the probe, not the probe refname or assigned number.
   // E.g. if a probe has three probes they are 1, 2, and 3 here, regardless of actual probe names or numbers.

   CString encoded( netname );

   if (probeNumber > 1)
      encoded.Format("%s--!!(Probe: %d)!!", netname, probeNumber);

   return encoded;
}

//-----------------------------------------------------------------------------------------

CAgI1000Probe *CAgI1000ProbeMap::LookupNthProbe(CString netname, int n)
{
   CString key = this->EncodeNetname(netname, n);

   CAgI1000Probe *probe = NULL;

   this->Lookup(key, probe);

   return probe;
}

//-----------------------------------------------------------------------------------------

int CAgI1000ProbeMap::AscendingRefnameSortFunc(const void *a, const void *b)
{
   CAgI1000Probe* itemA = (CAgI1000Probe*)(((SElement*) a )->pObject->m_object);
   CAgI1000Probe* itemB = (CAgI1000Probe*)(((SElement*) b )->pObject->m_object);

   InsertStruct *insA = itemA->GetCamcadProbeData()->getInsert();
   InsertStruct *insB = itemB->GetCamcadProbeData()->getInsert();

   return (insA->getSortableRefDes().CompareNoCase(insB->getSortableRefDes()));
}

//-----------------------------------------------------------------------------------------

CString CAgI1000ProbeMap::GetProbeListForNet(CString netname, CString separator)
{
   CString probes;

   if (separator.IsEmpty())
      separator = ",";

   CAgI1000Probe *probe = NULL;
   int probeCounter = 1;
   while ((probe = this->LookupNthProbe(netname, probeCounter++)) != NULL)
   {
      //Skip probes on Die pins
      if(!probe->IsProbedDiePin())
      {
         if (!probes.IsEmpty())
            probes += separator;
         CString probeNumStr;
         probeNumStr.Format("%d", probe->GetProbeNumber());
         probes += probeNumStr;
      }
      else if (m_logFp != NULL)
      {
         fprintf(m_logFp, "CAgI1000ProbeMap - Net: %s Probe $%d CompPin %s.%s - Skipped Probe for Die pin.\n",
            probe->GetProbedNetName(), probe->GetProbeNumber(), probe->GetProbedItemRefName(), probe->GetProbedItemPinName());
      }
   }

   return probes;
}

//-----------------------------------------------------------------------------------------

int CAgI1000ProbeMap::AssignProbeNumbers(int startingNumber)
{
   // Probes get renumbered like so...
   // Lowest probe numbers go to Ground probes.               (Pass 0)
   // Next come Power probes.                                 (Pass 1)
   // Then all probes that are not Ground or Power.           (Pass 2)

   // New probe numbers get pushed back into ccz data.

   // Only placed probes are in the map.
   // Assign numbers to those first, then process unplaced probes.

   int nextProbeNumber = startingNumber;

   for (int pass = 0; pass < 3; pass++)
   {
      CString *ignoredKey;
      CAgI1000Probe *probe = NULL;
      for (this->GetFirstSorted(ignoredKey, probe); probe != NULL; this->GetNextSorted(ignoredKey, probe))
      {
         CString nettype = probe->GetProbedNetType();
         
         if (
            (pass == 0 && nettype.CompareNoCase("GROUND") == 0) ||
            (pass == 1 && nettype.CompareNoCase("POWER") == 0)  ||
            (pass == 2 && nettype.CompareNoCase("SIGNAL") == 0)
            )
         {
            probe->SetProbeNumber(nextProbeNumber++);
         }

      }
   }

   return nextProbeNumber - 1;
}

//-----------------------------------------------------------------------------------------

void CAgI1000ProbeMap::ValidateProbeNumbers()
{
   // Output format apparantly requires strictly numeric probe numbers.
   // Check collected probes, if all refnames are strictly numeric then use them as
   // probe numbers. If any are alphanumeric, then renumber all.

   bool renumberRequired = false;

   CString *ignoredKey;
   CAgI1000Probe *probe = NULL;
   for (this->GetFirstSorted(ignoredKey, probe); probe != NULL && !renumberRequired; this->GetNextSorted(ignoredKey, probe))
   {
      CString refname = probe->GetCamcadProbeData()->getInsert()->getRefname();
      int pnum = atoi(refname);
      CString newRefname;
      newRefname.Format("%d", pnum);
      if (refname.Compare(newRefname) != 0)
      {
         renumberRequired = true;
      }
      else
      {
         probe->SetProbeNumber(pnum);
      }
   }

   if (renumberRequired)
   {
      int pnum = 1;
      for (this->GetFirstSorted(ignoredKey, probe); probe != NULL; this->GetNextSorted(ignoredKey, probe))
      {
         probe->SetProbeNumber(pnum++);
      }
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////

CAgI1000CompDataTest::CAgI1000CompDataTest(DeviceTypeTag devType, CString probeFlag, CString refName, CString gridLoc, 
                                           CString value, CString plusTol, CString minusTol, 
                                           CString testMode, CString pin1probe, CString pin2probe)
: m_devtype(devType)
, m_probeflag(probeFlag)
, m_refname(refName)
, m_gridlocation(gridLoc)
, m_value(value)
, m_plustol(plusTol)
, m_minustol(minusTol)
, m_testmode(testMode)
, m_pin1probe(pin1probe)
, m_pin2probe(pin2probe)
{
}

CAgI1000CompDataTest::CAgI1000CompDataTest(DeviceTypeTag devType, CString probeFlag, CString refName, CString gridLoc, 
                                           CString value, CString plusTol, CString minusTol, 
                                           CString testMode, int pin1probe, int pin2probe)
: m_devtype(devType)
, m_probeflag(probeFlag)
, m_refname(refName)
, m_gridlocation(gridLoc)
, m_value(value)
, m_plustol(plusTol)
, m_minustol(minusTol)
, m_testmode(testMode)
{
   m_pin1probe.Format("%d", pin1probe);
   m_pin2probe.Format("%d", pin2probe);
}

/////////////////////////////////////////////////////////////////////////////////////////////

CAgilentI1000WriterSettings::CAgilentI1000WriterSettings()
: m_pinSpacingThreshold(0.0)
, m_pinSpacingThresholdUnits(pageUnitsInches)
, m_outputUnits(pageUnitsInches)
, m_outputDecimalPlaces(3)
, m_exportTestJet(false)
{
}

void CAgilentI1000WriterSettings::Read()
{
   CFilePath settingsFilePath(getApp().getExportSettingsFilePath("Agilent_i1000.out"));
   
   {
      CString msg;
      msg.Format("Agilent i1000 Export: Settings file [%s].\n\n", settingsFilePath.getPath());
      getApp().LogMessage(msg);
   }

   if (fileExists(settingsFilePath.getPath()))
   {
      CSettingsFile settingsFile;

      if (!settingsFile.open( settingsFilePath.getPath() ))
      {
         CString msg;
         msg.Format("Could not open settings file '%s'\n", settingsFilePath.getPath());
         ErrorMessage(msg, "Agilent i1000 Export");
      }
      else
      {
         while (settingsFile.getNextCommandLine())
         {
            if      (settingsFile.isCommand(".PIN_TO_PIN_DISTANCE"   ,2))  
            {
               m_pinSpacingThreshold = settingsFile.getDoubleParam(1);
               if (settingsFile.getParamCount() > 2)
               {
                  CString units = settingsFile.getParam(2);
                  if (units.CompareNoCase("inch") == 0)
                     m_pinSpacingThresholdUnits = pageUnitsInches;
                  else if (units.CompareNoCase("mm") == 0 || units.CompareNoCase("millimeters") == 0)
                     m_pinSpacingThresholdUnits = pageUnitsMilliMeters;
               }
            }
            else if  (settingsFile.isCommand(".OUTPUT_UNITS"         ,2))  
            {
               CString units = settingsFile.getParam(1);
               if (units.CompareNoCase("inch") == 0)
                  m_outputUnits = pageUnitsInches;
               else if (units.CompareNoCase("mm") == 0 || units.CompareNoCase("millimeters") == 0)
                  m_outputUnits = pageUnitsMilliMeters;

               if (settingsFile.getParamCount() > 2)
               {
                  int places = settingsFile.getIntParam(2);
                  if (places > 0 && places < 8)
                     m_outputDecimalPlaces = places;
               }
            }
            else if (settingsFile.isCommand(".PIN_TO_PIN_TYPE"       ,2))  AddPinShortTestType( settingsFile.getParam(1) );
            else if (settingsFile.isCommand(".DESCRIPTION"           ,2))  m_descriptionAttribName = settingsFile.getParam(1);
            else if (settingsFile.isCommand(".EXPORT_TESTJET"        ,2))  settingsFile.parseYesNoParam(1, m_exportTestJet, false);
         }
      }
   }
}

void CAgilentI1000WriterSettings::AddPinShortTestType(CString devTypeStr)
{
   DeviceTypeTag devType = stringToDeviceTypeTag(devTypeStr);

   if (devType != deviceTypeUndefined)
      this->m_deviceTypeMap.SetAt((WORD)devType, NULL);
}

/////////////////////////////////////////////////////////////////////////////////////////////

CAgilentI1000Writer::CAgilentI1000Writer(CCEtoODBDoc *doc, FileStruct *pcbFile)
: m_doc(doc)
, m_pcbFile(pcbFile)
, m_placedProbeMap(m_logFp)
, m_unplacedProbeMap(m_logFp)
{
   m_settings.Read();
}

bool CAgilentI1000Writer::GetLogFile(CString outputfolderpath, CString logname)
{
   // Retro, for ordinary interactive CAMCAD, log file goes to same folder as output.
   CString logPath = GetLogfilePath(logname);
   if (!outputfolderpath.IsEmpty())
   {
      CFilePath logFilePath(outputfolderpath);
      logFilePath.setBaseFileName("AgilentI1000");
      logFilePath.setExtension("log");
      logPath = logFilePath.getPath();
   }

   m_localLogFilename = logPath; // This would be the standard interactive CAMCAD log.
   if (getApp().m_readWriteJob.IsActivated())
   {
      // Is readwrite job aka vPlan mode.
      m_logFp = getApp().OpenOperationLogFile("3070.log", m_localLogFilename);
      if (m_logFp == NULL) // error message already issued, just return.
         return false;
   }
   else
   {
      // Standard CCASM CAMCAD product.
      if ((m_logFp = fopen(m_localLogFilename, "wt")) == NULL) // rewrite file
      {
         ErrorMessage("Can not open Logfile!", m_localLogFilename, MB_ICONEXCLAMATION | MB_OK);
         return false;
      }
   }

   if (m_logFp != NULL)
   {
      WriteStandardExportLogHeader(m_logFp, "Agilent i1000");
      return true;
   }

   return false;
}

void CAgilentI1000Writer::WriteFiles(CString outputfolderpath)
{
   CString outfileBasename(m_pcbFile->getName());
   outfileBasename.Replace(".", "_");

   GetLogFile(outputfolderpath, "AgilentI1000.log");
   InitData();

   CFilePath outputFilepath(outputfolderpath);
   outputFilepath.setBaseFileName(outfileBasename);

   outputFilepath.setExtension("atd");
   CStdioFileWriteFormat atdFile;
   if (atdFile.open(outputFilepath.getPath()))
   {
      WriteATDFile(atdFile);
      atdFile.close();
   }

   outputFilepath.setExtension("nal");
   CStdioFileWriteFormat nailFile;
   if (nailFile.open(outputFilepath.getPath()))
   {
      WriteNailsFile(nailFile);
      nailFile.close();
   }

   outputFilepath.setExtension("pin");
   CStdioFileWriteFormat pinFile;
   if (pinFile.open(outputFilepath.getPath()))
   {
      WritePinsFile(pinFile);
      pinFile.close();
   }

   // Close log file only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!m_localLogFilename.IsEmpty())
   {
      fclose(m_logFp);
   }
}

void CAgilentI1000Writer::InitData()
{
   m_partMap.CollectParts(m_doc, m_pcbFile);
   int highestPlacedProbeNumber = m_placedProbeMap.CollectProbes(m_doc, m_pcbFile, true, 1);
   m_unplacedProbeMap.CollectProbes(m_doc, m_pcbFile, false, (highestPlacedProbeNumber + 1));
   m_viaMap.CollectVias(m_doc, m_pcbFile, m_placedProbeMap); // only collects vias that have probe directly placed on them

   CollectATDFileCompDataTests(deviceTypeResistor);
   CollectATDFileCompDataTests(deviceTypeCapacitor);
   CollectATDFileCompDataTests(deviceTypeInductor);
   CollectATDFileCompDataTests(deviceTypeDiode);
   CollectATDFileCompDataTests(deviceTypeDiodeZener);
   CollectATDFileCompDataTests(deviceTypeTransistor);
   CollectATDFileCompDataTests(deviceTypeJumper);

   CollectATDFileCompDataPinShortTests();

}

CString CAgilentI1000Writer::GetComponentTypeSectionName(DeviceTypeTag devType)
{
   switch (devType)
   {
   case deviceTypeResistor:
      return "R";

   case deviceTypeCapacitor:
   case deviceTypeCapacitorPolarized:
   case deviceTypeCapacitorTantalum:
      return "C";

   case deviceTypeInductor:
      return "L";

   case deviceTypeDiode:
   case deviceTypeDiodeLed:
      return "D";

   case deviceTypeDiodeZener:
      return "V";

   case deviceTypeTransistor:
   case deviceTypeTransistorNpn:
   case deviceTypeTransistorPnp:
      return "Q";

   case deviceTypeJumper:
      return "JP";


   default:
      break;
   }

   return "????"; // a flag to indicate device type is not supported
}


CString CAgilentI1000Writer::GetComponentTestMode(DeviceTypeTag devType)
{
   switch (devType)
   {
   case deviceTypeResistor:
      return "CC";

   case deviceTypeCapacitor:
   case deviceTypeCapacitorPolarized:
   case deviceTypeCapacitorTantalum:
      return "0";

   case deviceTypeInductor:
      return "0";

   case deviceTypeDiode:
   case deviceTypeDiodeLed:
      return "LV";

   case deviceTypeDiodeZener:
      return "LV";

   case deviceTypeTransistor:
   case deviceTypeTransistorNpn:
   case deviceTypeTransistorPnp:
      return "0";

   case deviceTypeJumper:
      return "JP";


   default:
      break;
   }

   return "????"; // a flag to indicate device type is not supported
}

void CAgilentI1000Writer::GenerateATDFileCompDataTest(CAgI1000Part *part, PinTypeTag pin1Type, PinTypeTag pin2Type, bool putPinsInRefname)
{
   if (part != NULL)
   {
      // In this context pin 1 and pin 2 are just two pins, not necessarily refnames 1 and 2.
      // Output has pin 1 in "left" column, pin 2 in "right" column.
            
      CompPinStruct *pin1 = part->GetCompPin(pin1Type);
      CompPinStruct *pin2 = part->GetCompPin(pin2Type);

      NetStruct *pin1Net = part->GetPinNet(pin1Type);
      NetStruct *pin2Net = part->GetPinNet(pin2Type);

      CAgI1000Probe *pin1Probe = NULL;
      CAgI1000Probe *pin2Probe = NULL;
      if (pin1Net != NULL)
         pin1Probe = m_placedProbeMap.LookupFirstProbe( pin1Net->getNetName() );
      if (pin2Net != NULL)
         pin2Probe = m_placedProbeMap.LookupFirstProbe( pin2Net->getNetName() );

      CString refname;
      if (putPinsInRefname)
      {
         refname.Format("%s-%s-%s",
            part->GetCamcadPartData()->getInsert()->getRefname(),
            (pin1 != NULL ? pin1->getPinName() : "0"),
            (pin2 != NULL ? pin2->getPinName() : "0"));
      }
      else
      {
         refname = part->GetCamcadPartData()->getInsert()->getRefname();
      }

      CAgI1000CompDataTest *compTest = new CAgI1000CompDataTest(
         part->GetDeviceType(),
         ((pin1Probe != NULL && pin2Probe != NULL) ? "@" : "P"),
         refname,
         part->GetGridLocation(),
         part->GetValue(),
         part->GetPlusTolerance(),
         part->GetMinusTolerance(),
         GetComponentTestMode(part->GetDeviceType()),
         (pin1Probe != NULL ? pin1Probe->GetProbeNumber() : 0),
         (pin2Probe != NULL ? pin2Probe->GetProbeNumber() : 0));

      this->m_testList.AddTail(compTest);
   }
}

void CAgilentI1000Writer::GenerateATDFileCompDataPinShortTest(CAgI1000Part *part, InsertStruct *pinAInsert, InsertStruct *pinBInsert)
{
   if (part != NULL && pinAInsert != NULL && pinBInsert != NULL)
   {
      // Order so "lower" pin is first
      InsertStruct *pin1 = pinAInsert;
      InsertStruct *pin2 = pinBInsert;
      if (pinAInsert->getSortableRefDes() > pinBInsert->getSortableRefDes())
      {
         pin1 = pinBInsert;
         pin2 = pinAInsert;
      }

      NetStruct *pin1Net = NULL;
      CompPinStruct *cp1 = NULL;
      cp1 = FindCompPin(m_pcbFile, part->GetCamcadPartData()->getInsert()->getRefname(), pin1->getRefname(), &pin1Net);

      NetStruct *pin2Net = NULL;
      CompPinStruct *cp2 = NULL;
      cp2 = FindCompPin(m_pcbFile, part->GetCamcadPartData()->getInsert()->getRefname(), pin2->getRefname(), &pin2Net);

      CAgI1000Probe *pin1Probe = NULL;
      if (pin1Net != NULL)
         pin1Probe = m_placedProbeMap.LookupFirstProbe(pin1Net->getNetName());

      CAgI1000Probe *pin2Probe = NULL;
      if (pin2Net != NULL)
         pin2Probe = m_placedProbeMap.LookupFirstProbe(pin2Net->getNetName());

      CString refname;
      refname.Format("%s-%s-%s",
         part->GetCamcadPartData()->getInsert()->getRefname(),
         pin1->getRefname(),
         pin2->getRefname());

      // Test pin short if pins are not on same net
      if (pin1Net == NULL || pin1Net != pin2Net)
      {
         CAgI1000CompDataTest *compTest = new CAgI1000CompDataTest(
            deviceTypeJumper, // Pin shorts are tested as Jumpers, regardless of actual component device type
            ((pin1Probe != NULL && pin2Probe != NULL) ? "@" : "P"),
            refname,
            part->GetGridLocation(),
            "4JP",
            "F",
            "F",
            "JP",
            (pin1Probe != NULL ? pin1Probe->GetProbeNumber() : 0),
            (pin2Probe != NULL ? pin2Probe->GetProbeNumber() : 0));

         this->m_testList.AddTail(compTest);
      }
   }
}

void CAgilentI1000Writer::CollectATDFileCompDataTests(DeviceTypeTag devTypeToCollect)
{
   CString currentSectionName( GetComponentTypeSectionName(devTypeToCollect) );

   CString *ignoredKey;
   CAgI1000Part *part = NULL;
   for (m_partMap.GetFirstSorted(ignoredKey, part); part != NULL; m_partMap.GetNextSorted(ignoredKey, part)) 
	{
      // Collect part if section name matches devTypeToCollect. This allows a single call to write a
      // group, which keeps the section sorted by refname, as opposed to three separate calls for each specific
      // transistor type, which would make three sets of ordering in the block.
      // e.g. all three Transistor types written in one call.

      CString partSectionName( GetComponentTypeSectionName(part->GetDeviceType()) );

      if (partSectionName.Compare(currentSectionName) == 0)
      {
         if (partSectionName.Compare("Q") == 0)
         {
            // Transistors - Generates multiple tests.
            // Base to collector, Collector to emitter, Emitter to base
            GenerateATDFileCompDataTest(part, pinTypeBase, pinTypeCollector, true);
            GenerateATDFileCompDataTest(part, pinTypeCollector, pinTypeEmitter, true);
            GenerateATDFileCompDataTest(part, pinTypeEmitter, pinTypeBase, true);
         }
         else
         {
            // "Regular" part - One test for two pins
            // If pins not typed (e.g. resistor) pin 1 is treated as anode and pin 2 is cathode
            GenerateATDFileCompDataTest(part, pinTypeAnode, pinTypeCathode, false);
         }
      }
   }
}

void CAgilentI1000Writer::CollectATDFileCompDataPinShortTests()
{
   // Based on code found in Takaya.cpp.
   // Requirements Spec says make tests like Takaya does.

   if (this->m_settings.GetPinSpacingThreshold(m_doc->getPageUnits()) > 0.0)
   {
      CString *ignoredKey;
      CAgI1000Part *part = NULL;
      for (m_partMap.GetFirstSorted(ignoredKey, part); part != NULL; m_partMap.GetNextSorted(ignoredKey, part)) 
      {
         if (this->m_settings.IsPinShortTestType(part->GetDeviceType()))
         {
            BlockStruct *compBlk = m_doc->getBlockAt( part->GetCamcadPartData()->getInsert()->getBlockNumber() );

            if (compBlk != NULL)
            {
               CTypedPtrArrayContainer<InsertStruct*> pinArray1(50, false); // NOT a container
               CTypedPtrArrayContainer<InsertStruct*> pinArray2(50, false); // NOT a container

               for (CDataListIterator dataList(*compBlk, insertTypePin); dataList.hasNext();)
               {
                  InsertStruct* pinInsert = dataList.getNext()->getInsert();
                  pinArray1.SetAtGrow(pinArray1.GetCount(), pinInsert);
                  pinArray2.SetAtGrow(pinArray2.GetCount(), pinInsert);
               }

               for (int index1 = 0; index1 < pinArray1.GetCount(); index1++)
               {
                  InsertStruct* pinInsert1 = pinArray1.GetAt(index1);
                  InsertStruct* closestPinInsert = NULL;

                  double shortestDistance = DBL_MAX;
                  for (int index2=index1+1; index2<pinArray2.GetCount(); index2++)
                  {
                     InsertStruct* pinInsert2 = pinArray2.GetAt(index2);

                     if (pinInsert1 != NULL && pinInsert2 != NULL)
                     {
                        double distance = pinInsert1->getOrigin2d().distance(pinInsert2->getOrigin2d());
                        if (distance < shortestDistance)
                        {
                           shortestDistance = distance;
                           closestPinInsert = pinInsert2;
                        }
                     }
                  }

                  // Add pin to pin short
                  if (shortestDistance < m_settings.GetPinSpacingThreshold(m_doc->getPageUnits()) && pinInsert1 != NULL && closestPinInsert != NULL)
                  {
                     GenerateATDFileCompDataPinShortTest(part, pinInsert1, closestPinInsert);
                  }
               }
            }
         }
      }
   }
}

void CAgilentI1000Writer::WriteATDFile(CStdioFileWriteFormat& atdFile)
{
   WriteATDFileComponentDataTests(atdFile);
   
   if (m_settings.GetExportTestJetFlag())
      WriteATDFileCapOpensTests(atdFile);
}

void CAgilentI1000Writer::WriteATDFileComponentDataTests(CStdioFileWriteFormat& atdFile)
{
  atdFile.writef("{>>>>%s>>>>}\n", "Component Data");

   WriteATDFileCompDataTests(atdFile, deviceTypeResistor);
   WriteATDFileCompDataTests(atdFile, deviceTypeCapacitor);
   WriteATDFileCompDataTests(atdFile, deviceTypeInductor);
   WriteATDFileCompDataTests(atdFile, deviceTypeDiode);
   WriteATDFileCompDataTests(atdFile, deviceTypeDiodeZener);
   WriteATDFileCompDataTests(atdFile, deviceTypeTransistor);
   WriteATDFileCompDataTests(atdFile, deviceTypeJumper);

   atdFile.writef("{<<<<%s<<<<}\n", "ComponentData");
}

void CAgilentI1000Writer::WriteATDFileCompDataTests(CStdioFileWriteFormat& atdFile, DeviceTypeTag devTypeToWrite)
{
   CString currentSectionName( GetComponentTypeSectionName(devTypeToWrite) );

   atdFile.writef("{>>>>%s>>>>}\n", currentSectionName);

   int recId = 1;
   POSITION testpos = this->m_testList.GetHeadPosition();
   while (testpos != NULL)
   {
      CAgI1000CompDataTest *test = this->m_testList.GetNext(testpos);

      // Output part if section name matches devTypeToWrite. This allows a single call to write a
      // group, which keeps the section sorted by refname, as opposed to three separate calls for each specific
      // transistor type, which would make three sets of ordering in the block.
      // e.g. all three Transistor types written in one call.

      CString partSectionName( GetComponentTypeSectionName(test->GetDeviceType()) );

      if (partSectionName.Compare(currentSectionName) == 0)
         WriteATDFileCompDataTest(atdFile, recId++, test);
   }

   atdFile.writef("{<<<<%s<<<<}\n", currentSectionName);
}

void CAgilentI1000Writer::WriteATDFileCompDataTest(CStdioFileWriteFormat& atdFile, int recId, CAgI1000CompDataTest *test)
{
   // All the data we actually write are within field range 1 to 14.
   // There are a couple constant fields within that range.

   atdFile.writef("%d;", recId);
   atdFile.writef("%s;", test->GetProbeFlag());
   atdFile.writef("%s;", test->GetRefName());
   atdFile.writef("%s;", test->GetGridLocation());
   atdFile.writef("0;"); // Actual value - we write constant 0
   atdFile.writef("%s;", test->GetValue());
   atdFile.writef("%s;", test->GetPlusTolerance());
   atdFile.writef("%s;", test->GetMinusTolerance());
   atdFile.writef("%s;", test->GetTestMode());
   atdFile.writef("1;"); // Range - constant value 1
   atdFile.writef("0;"); // Delay - constant value 0
   atdFile.writef("0;"); // Average - constant value 0
   atdFile.writef("%s;", test->GetPin1Probe());
   atdFile.writef("%s;", test->GetPin2Probe());

   // There are 19 additional fields in a record, all constants.

   atdFile.writef("0;"); // EA - constant value 0
   atdFile.writef("0;"); // EB - constant value 0
   atdFile.writef("0;"); // DAV - constant value 0
   atdFile.writef("0;"); // EDAV - constant value 0
   atdFile.writef("0;"); // G1 - constant value 0
   atdFile.writef("0;"); // 2 - constant value 0
   atdFile.writef("0;"); // 3 - constant value 0
   atdFile.writef("0;"); // 4 - constant value 0
   atdFile.writef("0;"); // 5 - constant value 0
   atdFile.writef("0;"); // 6 - constant value 0
   atdFile.writef("0;"); // 7 - constant value 0
   atdFile.writef("0;"); // 8 - constant value 0
   atdFile.writef("0;"); // 9 - constant value 0
   atdFile.writef("0;"); // 10 - constant value 0
   atdFile.writef("0;"); // Offset - constant value 0
   atdFile.writef("0;"); // 0 - constant value 0
   atdFile.writef("0;"); // Retry - constant value 0
   atdFile.writef("F;"); // Save - constant value F
   atdFile.writef("0;"); // Condition Time - constant value 0

   atdFile.write("\n");
}



void CAgilentI1000Writer::WriteATDFileCapOpensTests(CStdioFileWriteFormat& atdFile)
{
   CString currentSectionName( "IC" );

   atdFile.writef("{>>>>%s>>>>}\n", currentSectionName);

   CString *ignoredKey;
   CAgI1000Part *part = NULL;
   for (m_partMap.GetFirstSorted(ignoredKey, part); part != NULL; m_partMap.GetNextSorted(ignoredKey, part)) 
	{
      DeviceTypeTag devType = part->GetDeviceType();

      if (devType == deviceTypeIC ||
         devType == deviceTypeICDigital ||
         devType == deviceTypeICLinear)
      {

         if (part->TestCapacitiveOpens())
         {
            WriteATDFileCapOpensTest(atdFile, part);
         }
      }
   }

   atdFile.writef("{<<<<%s<<<<}\n", currentSectionName);
}


void CAgilentI1000Writer::WriteATDFileCapOpensTest(CStdioFileWriteFormat& atdFile, CAgI1000Part *part)
{
   CString name = part->GetCamcadPartData()->getInsert()->getRefname();

   atdFile.writef("{>>>>1:%s>>>>}\n", name);

   CString gridLoc( part->GetGridLocation() );

   atdFile.writef("%s;%s;%s;%s;%s;\n", "0", "F", "F", gridLoc.IsEmpty() ? "@" : gridLoc, "@");


   atdFile.writef("{>>>>%s>>>>}\n", "PIN");

   // 20 pins per line
   CString *ignoredKey;
   CAgI1000Pin *pin = NULL;
   int count = 0;
   for (part->GetPinMap().GetFirstSorted(ignoredKey, pin); pin != NULL; part->GetPinMap().GetNextSorted(ignoredKey, pin)) 
	{
      CAgI1000Probe *probe = NULL;
      if (pin->GetNet() != NULL)
         probe = m_placedProbeMap.LookupFirstProbe(pin->GetNet()->getNetName());

      if (count > 0 && (count % 20) == 0)
         atdFile.write("\n");

      atdFile.writef("%d;", (probe != NULL ? probe->GetProbeNumber() : 0));
      count++;
   }

   if (count > 0)
      atdFile.write("\n");

   atdFile.writef("{<<<<%s<<<<}\n", "PIN");

   atdFile.writef("{<<<<1:%s<<<<}\n", name);
}

/////////////////////////////////////////////////////////////////////////////////////////////

CAgI1000Pin::CAgI1000Pin(CCEtoODBDoc *doc, FileStruct *pcbFile, DataStruct *camcadPartData, DataStruct *camcadPinData)
: m_doc(doc)
, m_pcbFile(pcbFile)
, m_camcadPartData(camcadPartData)
, m_camcadPinData(camcadPinData)
, m_comppin(NULL)
, m_net(NULL)
{
   if (m_camcadPartData != NULL && m_camcadPinData != NULL)
      m_comppin = FindCompPin(m_pcbFile, m_camcadPartData->getInsert()->getRefname(), m_camcadPinData->getInsert()->getRefname(), &m_net);
   int jj = 0;//*rcf
}

void CAgI1000PinMap::CollectPins(CCEtoODBDoc *doc, FileStruct *pcbFile, DataStruct *camcadPartData)
{
   this->empty();

   // Collect the parts
   if (doc != NULL && pcbFile != NULL && camcadPartData != NULL && camcadPartData->isInsert())
   {
      BlockStruct *insertedBlk = doc->getBlockAt( camcadPartData->getInsert()->getBlockNumber() );
      if (insertedBlk != NULL)
      {
	      POSITION pos = insertedBlk->getHeadDataInsertPosition();
	      while (pos != NULL)
         {
            DataStruct *datum = insertedBlk->getNextDataInsert(pos);

            if (datum->isInsertType(insertTypePin))
            {
               CString refname = datum->getInsert()->getSortableRefDes();
               LPCTSTR key;
               if (this->LookupKey(refname, key))
               {
                  //*rcf error - dup pin refname
               }
               else
               {
                  CAgI1000Pin *pin = new CAgI1000Pin(doc, pcbFile, camcadPartData, datum);
                  this->SetAt(refname, pin);
               }
            }
         }
      }

      // Sort the parts by ascending refname
      this->setSortFunction(&CAgI1000PinMap::AscendingRefnameSortFunc);
      this->Sort();
   }
}

//-----------------------------------------------------------------------------------------

int CAgI1000PinMap::AscendingRefnameSortFunc(const void *a, const void *b)
{
   CAgI1000Pin* itemA = (CAgI1000Pin*)(((SElement*) a )->pObject->m_object);
   CAgI1000Pin* itemB = (CAgI1000Pin*)(((SElement*) b )->pObject->m_object);

   InsertStruct *insA = itemA->GetCamcadPinData()->getInsert();
   InsertStruct *insB = itemB->GetCamcadPinData()->getInsert();

   return (insA->getSortableRefDes().CompareNoCase(insB->getSortableRefDes()));
}


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

void CAgilentI1000Writer::WriteNailsFile(CStdioFileWriteFormat& nailsFile)
{
   // The PRD said output nails.asc and pins.asc like Spectrum.
   // HOwever, Spectrum has a whole bunch of rules that assign "channel numbers", and I was
   // told that part of Spectrum does not apply. So some of the channel number related
   // code here is disabled, but kept for reference for awhile, during alpha/beta test.

   // For this file the nets have a net number.
   // As of this writing, none of the other i1000 outputs use the net number.
   // So we just determine it locally here, and toss it when done.
   CMapStringToInt netNumberMap;

   CString filename( nailsFile.getFilePath() );
   CFilePath filepath(filename);

   CTime time = CTime::GetCurrentTime();

   nailsFile.writef("; ---------------------------------------------------\n");
   nailsFile.writef("; Filename : %s\n", filepath.getFileName());
   nailsFile.writef("; Created by CCE to ODB++ v.%s\n", getApp().getVersionString());
   nailsFile.writef("; %s\n", time.Format("%A, %B %d, %Y"));
   nailsFile.writef("; ---------------------------------------------------\n");
   nailsFile.writef(";\n");
   nailsFile.writef("; Descripion:\n");
   nailsFile.writef(";  $0 - channel number\n");
   nailsFile.writef(";  <x> <y> - probe location\n");
   nailsFile.writef(";  <type> - Barrel size code, constant 1\n");
   //nailsFile.writef(";  <type> - Barrel size code, map to the 50, 75, 100 mil diameter\n");
   //nailsFile.writef(";        probe fixture hole diameter. Provide this note in a log file.\n");
   //nailsFile.writef(";       \"Nails.asc: Type code maps to Barrel Size\n");
   //nailsFile.writef(";        Type 1, Size 100\"\n");
   nailsFile.writef(";  <grid> - nail grid location if set, \"@\" if not set\n");
   nailsFile.writef(";  <T/B> - probe surface (B = bottom, T = top)\n");
   nailsFile.writef(";  <net> - net number\n");
   nailsFile.writef(";  <net name> - net name\n");
   nailsFile.writef(";  <virtual> - 'Virtual' is constant V\n"); // case 2025 changed this from unused to constant "V"
   nailsFile.writef(";  <pin/via> - probe logical location (PIN - device pin U5.14,\n");
   nailsFile.writef(";        VIA = Via named via123)\n");
   nailsFile.writef(";\n");
   //nailsFile.writef(";  $32000   = an optional probable location on the net that was not\n");
   //nailsFile.writef(";        needed because other locations were used. (The test\n");
   //nailsFile.writef(";        engineer could select one of these locations as an\n");
   //nailsFile.writef(";        alternate or additional probe location depending\n");
   //nailsFile.writef(";        on test requirements.) This section is listed last.\n");
   //nailsFile.writef(";\n");
   nailsFile.writef(";\n");
   nailsFile.writef("Nail          X             Y             Type          Grid          T/B           Net           Net Name      Virtual       Pin/Via\n");

   int decimalPlaces = this->m_settings.GetOutputDecimalPlaces();
   PageUnitsTag docPageUnits = m_doc->getPageUnits();

   CString *ignoredKey;
   CAgI1000Probe *probe = NULL;
   for (m_placedProbeMap.GetFirstSorted(ignoredKey, probe); probe != NULL; m_placedProbeMap.GetNextSorted(ignoredKey, probe))
   {
      // if (probe->IsPlaced())  As of this writing only probes in probe map ARE placed
      {
         //CIplNet *iplNet = m_netMap.Lookup(iplTP->GetNetName());

         int channelNumber = probe->GetProbeNumber();
         //int channelNumber = iplTP->GetChannelNumber();
         //if (iplTP->IsPowerInjection())
         //   channelNumber = 10000;
         //else if (iplTP->GetChannelNumber() == CHNL_UNASSIGNED)
         //   channelNumber = 32000;

         // (Spectrum/IPL) case 2025 asks for constant V in this field
         CString virtualVal("V"); 

         // get the Pin/Via section
         CString pinOrVia;
         if (entityTypeCompPin == probe->GetProbedItemEntityType())
            pinOrVia.Format("PIN %s.%s", probe->GetProbedItemRefName(), probe->GetProbedItemPinName());
         else if (entityTypeData == probe->GetProbedItemEntityType())
         {
            CString viaName = probe->GetProbedItemRefName();
            pinOrVia.Format("VIA %s", viaName.IsEmpty() ? "." : viaName);
         }

         // net number - start at 1
         int netnumber = 0;
         if (!netNumberMap.Lookup(probe->GetProbedNetName(), netnumber))
         {
            netnumber = netNumberMap.GetCount() + 1;
            netNumberMap.SetAt(probe->GetProbedNetName(), netnumber);
         }
         
         CString gridLoc( probe->GetGridLocation() );
         if(!probe->IsProbedDiePin())
         {
            nailsFile.writef("$%-12d %-13.*f %-13.*f %-13d %-13s (%s)%10s #%-12d %-13s %-13s %s\n",
               channelNumber, 
               decimalPlaces, m_settings.GetOutputValue(probe->GetCamcadProbeData()->getInsert()->getOriginX(), docPageUnits), 
               decimalPlaces, m_settings.GetOutputValue(probe->GetCamcadProbeData()->getInsert()->getOriginY(), docPageUnits),
               1, 
               gridLoc.IsEmpty() ? "@" : gridLoc,
               probe->GetCamcadProbeData()->getInsert()->getPlacedTop() ? "T" : "B", 
               "", 
               netnumber, 
               probe->GetProbedNetName(), 
               virtualVal, 
               pinOrVia);
         }
         else // skipped probes on Die Pins
         {
            if (m_logFp != NULL)
            {
               fprintf(m_logFp, "%s - Net: %s Probe $%d CompPin %s.%s - Skipped Probe for Die pin.\n",
                  filepath.getFileName(), probe->GetProbedNetName(), channelNumber, 
                  probe->GetProbedItemRefName(), probe->GetProbedItemPinName());
            }
         }
      }
   }

}

////////////////////////////////////////////////////////////////////////////////////////////////

void CAgilentI1000Writer::WritePinsFile(CStdioFileWriteFormat& pinsFile)
{
   WritePinsFileHeader(pinsFile);
   WritePinsFileComponents(pinsFile);
   WritePinsFileVias(pinsFile);
}

void CAgilentI1000Writer::WritePinsFileHeader(CStdioFileWriteFormat& pinsFile)
{
   // Pattern for this lifted from Spectrum Writer

   CTime time = CTime::GetCurrentTime();

   CString filename( pinsFile.getFilePath() );
   CFilePath filepath(filename);

   pinsFile.writef("; ---------------------------------------------------\n");
   pinsFile.writef("; Filename : %s\n", filepath.getFileName() );
   pinsFile.writef("; Created by CCE to ODB++ v.%s\n", getApp().getVersionString());
   pinsFile.writef("; %s\n", time.Format("%A, %B %d, %Y"));
   pinsFile.writef("; ---------------------------------------------------\n");
   pinsFile.writef(";\n");
   pinsFile.writef(";  Description:\n");
   pinsFile.writef(";     First line:\n");
   pinsFile.writef(";        <part> - reference designator\n");
   pinsFile.writef(";        <T/B> - surface location (T = top, B = bottom)\n");
   pinsFile.writef(";        <device> - device type\n");
   pinsFile.writef(";     Second and following lines:\n");
   pinsFile.writef(";         <pin> - pin number\n");
   pinsFile.writef(";         <name> - pin name\n");
   pinsFile.writef(";         <x> - location of pin on board\n");
   pinsFile.writef(";         <y>	- location of pin on board\n");
   pinsFile.writef(";         <layer> - 0 = through hole pin location, 1 = SMD top, 2 = SMD bottom\n");
   pinsFile.writef(";         <net> - name of net attached to the pin\n");
   pinsFile.writef(":         <nail> - channel number(s) assigned to net\n");
   //pinsFile.writef(";                  32000 - alternated probe locations available (see Nails.asc)\n");
   pinsFile.writef(";\n");
   pinsFile.writef("Part          T/B  Device\n");
   pinsFile.writef("Pin           Name          X             Y             Layer         Net           Nail(s)\n");
}

void CAgilentI1000Writer::WritePinsFileComponents(CStdioFileWriteFormat& pinsFile)
{
   // Pattern for this lifted from Spectrum Writer

   int decimalPlaces = this->m_settings.GetOutputDecimalPlaces();
   PageUnitsTag docPageUnits = m_doc->getPageUnits();

   CString *ignoredKey;
   CAgI1000Part *part = NULL;
   for (m_partMap.GetFirstSorted(ignoredKey, part); part != NULL; m_partMap.GetNextSorted(ignoredKey, part)) 
	{
      pinsFile.writef("PART %-8s (%s)  '%s'\n", 
         part->GetCamcadPartData()->getInsert()->getRefname(), 
         part->GetCamcadPartData()->getInsert()->getPlacedTop() ? "T" : "B",
         part->GetDescription( m_settings.GetDescriptionAttribName() ));

      // write all the pins
      CString *ignoredKey;
      CAgI1000Pin *pin = NULL;
      int count = 0;
      for (part->GetPinMap().GetFirstSorted(ignoredKey, pin); pin != NULL; part->GetPinMap().GetNextSorted(ignoredKey, pin)) 
      {
         // calculate the location of the pin in the board
         CPoint2d pinLocation(pin->GetCamcadPinData()->getInsert()->getOriginX(), pin->GetCamcadPinData()->getInsert()->getOriginY());
         if (part->GetCamcadPartData()->getInsert()->getPlacedBottom()) pinLocation.x *= -1.;
         CTMatrix transform;
         transform.rotateRadians(part->GetCamcadPartData()->getInsert()->getAngleRadians());
         transform.translate(part->GetCamcadPartData()->getInsert()->getOriginX(), part->GetCamcadPartData()->getInsert()->getOriginY());
			transform.transform(pinLocation);

			// get all the placed probe numbers for the net this pin is on
         CString nails = m_placedProbeMap.GetProbeListForNet(pin->GetNet()->getNetName(), ",");

			// get the layer paramter of the pin
			// 0 = through hole pin location, 1 = SMD top, 2 = SMD bottom
         int layer = part->GetCamcadPartData()->getInsert()->getPlacedTop() ? 1 : 2;
         if (!part->IsSMD()) layer = 0;

			pinsFile.writef("%-14d%-14s%-14.*f%-14.*f%-14d%-14s%s\n", 
            ++count, // pin number
            pin->GetCamcadPinData()->getInsert()->getRefname(),
            decimalPlaces, m_settings.GetOutputValue(pinLocation.x, docPageUnits), 
            decimalPlaces, m_settings.GetOutputValue(pinLocation.y, docPageUnits), 
            layer, pin->GetNet()->getNetName(), nails);
		}


      pinsFile.writef("\n");
	}
}

void CAgilentI1000Writer::WritePinsFileVias(CStdioFileWriteFormat& pinsFile)
{
   int decimalPlaces = this->m_settings.GetOutputDecimalPlaces();
   PageUnitsTag docPageUnits = m_doc->getPageUnits();

   CString *ignoredKey;
   CAgI1000Via *via = NULL;

   for (m_viaMap.GetFirstSorted(ignoredKey, via); via != NULL; m_viaMap.GetNextSorted(ignoredKey, via)) 
	{
		// get the layer paramter of the pin
		// 0 = through hole pin location, 1 = SMD top, 2 = SMD bottom
      int layer = via->GetCamcadViaData()->getInsert()->getPlacedTop() ? 1 : 2;
      if (!via->IsSMD()) layer = 0;

      pinsFile.writef("PART %-8s (%s)  '%s'\n", 
         via->GetCamcadViaData()->getInsert()->getRefname(),
         via->GetCamcadViaData()->getInsert()->getPlacedTop() ? "T" : "B",
         "VIA");

      pinsFile.writef("%-14d%-14s%-14.*f%-14.*f%-14d%-14s%d\n\n", 
         1, "1", 
         decimalPlaces, m_settings.GetOutputValue(via->GetCamcadViaData()->getInsert()->getOriginX(), docPageUnits), 
         decimalPlaces, m_settings.GetOutputValue(via->GetCamcadViaData()->getInsert()->getOriginY(), docPageUnits), 
         layer, 
         via->GetProbe()->GetProbedNetName(), 
         via->GetProbe()->GetProbeNumber());
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////