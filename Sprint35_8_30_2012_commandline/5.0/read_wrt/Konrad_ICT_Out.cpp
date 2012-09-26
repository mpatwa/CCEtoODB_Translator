
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright ?1994-2003. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "TypedContainer.h"
#include "CompValues.h"
#include "Net_Util.h"
#include "RwLib.h"
#include "Element.h"
#include "Konrad_ICT_Out.h"
#include "InFile.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//**************************************************************************


void KonradICT_WriteFile(const CString& filename, CCEtoODBDoc &doc, FormatStruct *format)
{
	FileStruct *file = doc.getFileList().GetOnlyShown(blockTypePcb);

	if (file == NULL)
	{
		ErrorMessage("Need single visible PCB to export.", "Konrad ICT Write", MB_OK);
		return;
	}

   // No point in creating output if there are no probes placed
   bool hasOnePlacedProbe = false;
   WORD placementKW = doc.RegisterKeyWord(ATT_PROBEPLACEMENT, 0, valueTypeString);
	POSITION pos;
	for (pos = file->getBlock()->getHeadDataInsertPosition(); pos!=NULL && !hasOnePlacedProbe; file->getBlock()->getNextDataInsert(pos))
	{
		DataStruct *data = file->getBlock()->getAtData(pos);
      if (data->isInsertType(insertTypeTestProbe))
      {
         Attrib *attr = NULL;      
         if (data->lookUpAttrib(placementKW, attr))
         {
            CString probename = data->getInsert()->getRefname();
            CString placementStr = get_attvalue_string(&doc, attr);
            if (!probename.IsEmpty() && placementStr.CompareNoCase("Placed") == 0)
            {
               hasOnePlacedProbe = true;
            }
         }
      }
   }
	if (!hasOnePlacedProbe)
	{
		ErrorMessage("Currently visible PCB has no placed probes, placed probes are required for this writer.\nRun probe placement before running Konrad ICT Write.", "Konrad ICT Write", MB_OK);
		return;
	}

   // Still here? Looks okay, try making Konrad output

	try
	{
      CKonradICTWriter konrad(doc, *file, filename);
      konrad.GatherComponents();
      konrad.SortComponents();
      konrad.WriteFile(filename);
      konrad.WriteUnsupportedToLog();

	}
	catch (CString exception)
	{
		ErrorMessage(exception, "Konrad ICT Write", MB_OK);
	}
}


//**************************************************************************


CKonradICTWriter::CKonradICTWriter(CCEtoODBDoc &doc, FileStruct &file, CString outputFilename) : CObject()
	, m_doc(doc)
	, m_file(file)
{
   // Open log file.
   m_logFile = getApp().OpenOperationLogFile("KonradICTWrite.log", m_localLogFilename);
   if (m_logFile == NULL) // error message already issued, just return.
      return;
   WriteStandardExportLogHeader(m_logFile, "Konrad ICT");

   FillProbeMap();

   m_subclassList.Load(&doc.getCamCadData().getTableList());
   
}

CKonradICTWriter::~CKonradICTWriter()
{
   // Close log file only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!m_localLogFilename.IsEmpty())
   {
      fclose(m_logFile);
   }

   m_probeMap.RemoveAll();
}

void CKonradICTWriter::FillProbeMap()
{
   // Fill probe map, maps net name to a placed probe name
   WORD placementKW = m_doc.RegisterKeyWord(ATT_PROBEPLACEMENT, 0, valueTypeString);
   WORD netnameKW = m_doc.RegisterKeyWord(ATT_NETNAME, 0, valueTypeString);
	POSITION pos;
	for (pos = m_file.getBlock()->getHeadDataInsertPosition(); pos!=NULL; m_file.getBlock()->getNextDataInsert(pos))
	{
		DataStruct *data = m_file.getBlock()->getAtData(pos);
      if (data->isInsertType(insertTypeTestProbe))
      {
         Attrib *attr = NULL;      
         if (data->lookUpAttrib(placementKW, attr))
         {
            CString probename = data->getInsert()->getRefname();
            CString placementStr = get_attvalue_string(&m_doc, attr);
            if (!probename.IsEmpty() && placementStr.CompareNoCase("Placed") == 0)
            {
               if (data->lookUpAttrib(netnameKW, attr))
               {
                  CString netname = get_attvalue_string(&m_doc, attr);
                  CString existingProbe;
                  if (!m_probeMap.Lookup(netname, existingProbe))
                  {
                     m_probeMap.SetAt(netname, probename);
                  }
               }
            }
         }
      }
   }
}

bool CKonradICTWriter::WriteFile(CString filename)
{
	CStdioFileWriteFormat outFile(2048);
	CExtFileException err;
	if (!outFile.open(filename, &err))
	{
		CString errMsg;
		char exceptionMsg[255];
		if (err.m_cause != err.none && err.GetErrorMessage(exceptionMsg, 255))
		{
			errMsg.Format("Error writing file!!\n\nFile could not be open due to the following reason:\n   %s", exceptionMsg);
			fprintf(m_logFile, "Error: Could not open file for write {%s}\n", exceptionMsg);
		}
		else
			errMsg = "Error writing file!!\n\nFile could not be opened for writing.";

		ErrorMessage(errMsg, "Konrad ICT Write");

		return false;
	}

   CStringArray skippedCompMsgs;

   bool retval = true;
   if (!WriteComponents(outFile, deviceTypeResistor, skippedCompMsgs))             retval = false;
   if (!WriteComponents(outFile, deviceTypeCapacitor, skippedCompMsgs))            retval = false;
   if (!WriteComponents(outFile, deviceTypeCapacitorPolarized, skippedCompMsgs))   retval = false;
   if (!WriteComponents(outFile, deviceTypeCapacitorTantalum, skippedCompMsgs))    retval = false;
   if (!WriteComponents(outFile, deviceTypeInductor, skippedCompMsgs))             retval = false;
   if (!WriteComponents(outFile, deviceTypeDiode, skippedCompMsgs))                retval = false;
   if (!WriteComponents(outFile, deviceTypeDiodeLed, skippedCompMsgs))             retval = false;
   if (!WriteComponents(outFile, deviceTypeDiodeZener, skippedCompMsgs))           retval = false;

	outFile.close();

   // Write skipped list to log.
   // It's done this way to group the messages, and keep them out of block of other messages produced for items during output.
   if (skippedCompMsgs.GetCount() > 0)
   {
      fprintf(m_logFile, "\n\n");
      for (int msgI = 0; msgI < skippedCompMsgs.GetCount(); msgI++)
      {
         fprintf(m_logFile, skippedCompMsgs.GetAt(msgI));
      }
   }

   // Mark didn't like this, so it is out, but easily revived if minds change
#ifdef REPORT_KONRAD_ERROR
   if (!retval)
   {
      ErrorMessage("Errors encountered while writing output, see log file for details.", "Konrad ICT Write");
   }
#endif

	return retval;
}

bool CKonradICTWriter::WriteComponents(CStdioFileWriteFormat &outFile, DeviceTypeTag targetDeviceType, CStringArray &skippedCompMsgs)
{
   bool retval = true;

   CString targetDevTypeStr = deviceTypeTagToFriendlyString(targetDeviceType);
   outFile.writef("//\n// %s\n", targetDevTypeStr);

   for (int i = 0; i < m_componentList.GetCount(); i++)
   {
      CKonradComponent *kcomp = m_componentList.GetAt(i);

      CKonradTestElementList &elementList = kcomp->GetTestElementList(m_subclassList);
      
      for (int elementI = 0; elementI < elementList.GetCount(); elementI++)
      {
         CKonradTestElement *testElement = elementList.GetAt(elementI);

         if (testElement != NULL && testElement->GetDeviceTypeTag() == targetDeviceType )
         {
            if ( getSettings()->getExportUnloadedParts() == true || kcomp->GetLoaded().CompareNoCase("false") != 0 )  // Check the setting from file
            {
               if ( kcomp->GetMergedStatus().CompareNoCase("Ignored") != 0 )	// Don't print out the component with Ignore merged status
               {
                  if (!testElement->Write(outFile, m_probeMap, m_logFile))
                     retval = false;
               }
               else
               {
                  CString msg;
                  msg.Format("Component skipped due to Merged Status (Ignored):  %7s  %s\n",
                     kcomp->GetRefname(),
                     deviceTypeTagToFriendlyString(kcomp->GetDeviceTypeTag()));
                  skippedCompMsgs.Add(msg);
                  kcomp->SetSupported(true); // White lie, don't want this in Unsupported List report
               }
            }
            else
            {
               CString msg;
               msg.Format("Component skipped due to Loaded (False):  %7s  %s\n",
                  kcomp->GetRefname(),
                  deviceTypeTagToFriendlyString(kcomp->GetDeviceTypeTag()));
               skippedCompMsgs.Add(msg);
               kcomp->SetSupported(true); // White lie, don't want this in Unsupported List report
            }
         }
      }
   }


   return retval;
}

void CKonradICTWriter::WriteUnsupportedToLog()
{
   // Call this function only after WriteComponents().
   // The "is supported" flag is set during the write process.

   fprintf(m_logFile, "\n\n");

   for (int i = 0; i < m_componentList.GetCount(); i++)
   {
      CKonradComponent *kcomp = m_componentList.GetAt(i);
      if (!kcomp->IsSupported())
      {
         fprintf(m_logFile, "Unsupported device type:  %7s  %s\n",
            kcomp->GetRefname(),
            deviceTypeTagToFriendlyString(kcomp->GetDeviceTypeTag()));
      }
   }
}

void CKonradICTWriter::GatherComponents()
{
	POSITION pos;
	for (pos = m_file.getBlock()->getHeadDataInsertPosition(); pos!=NULL; m_file.getBlock()->getNextDataInsert(pos))
	{
		DataStruct *data = m_file.getBlock()->getAtData(pos);

      if (data->isInsertType(insertTypePcbComponent))
      {
         CKonradComponent *kcomp = new CKonradComponent(&m_doc, data);
         m_componentList.Add(kcomp);
      }
	}
}

static int AscendingRefnameSortFunc(const void *a, const void *b)
{
   CKonradComponent *ka = *((CKonradComponent**)(a));
   CKonradComponent *kb = *((CKonradComponent**)(b));

   return ka->GetSortKey().CompareNoCase(kb->GetSortKey());
}


void CKonradICTWriter::SortComponents()
{

   this->m_componentList.setSortFunction(AscendingRefnameSortFunc);
   this->m_componentList.sort();
}


//**************************************************************************

CKonradComponent::CKonradComponent(CCEtoODBDoc *doc, DataStruct *data)
: m_doc(doc)
, m_compData(data)
, m_deviceType(deviceTypeUnknown)
, m_supportedDevType(false)
, m_krSettings(false)
{
   // This will get used a lot, don't want to look it up/calc it every time
   m_sortableRefname = data->getInsert()->getSortableRefDes();

   // This too
   Attrib *attr = NULL;
   WORD devTypeKW = m_doc->RegisterKeyWord(ATT_DEVICETYPE, 0, valueTypeString);
   if (data->lookUpAttrib(devTypeKW, attr))
      m_deviceType = stringToDeviceTypeTag( get_attvalue_string(m_doc, attr) );
}

CString CKonradComponent::GetValue()
{
   CString value("0");
   Attrib* attrib = NULL;
   WORD valueKW = m_doc->RegisterKeyWord(ATT_VALUE, 0, VT_STRING);

   if ( m_compData->getAttributes() != NULL && m_compData->getAttributes()->Lookup(valueKW, attrib) )
      value = GetAttribValue(attrib);

   // Case dts0100423258 - Konrad refuses to recognize upper case K for Kilo, requires lower case.
   // Though this is just an "info" (comment) value, we'll adjust the K's here too.
   value.Replace('K', 'k');

   return value;
}

CString CKonradComponent::GetPlusTolerance()
{
   CString plusTol("0%");
   Attrib* attrib = NULL;
   WORD plusTolKW = m_doc->RegisterKeyWord(ATT_PLUSTOLERANCE, 0, VT_DOUBLE);

   if (m_compData->getAttributes() != NULL && m_compData->getAttributes()->Lookup(plusTolKW, attrib) )
      plusTol.Format("%d%%", round(fabs(attrib->getDoubleValue())));

   return plusTol;
}

CString CKonradComponent::GetMinusTolerance()
{
   CString minusTol("0%");
   Attrib* attrib = NULL;
   WORD minusTolKW = m_doc->RegisterKeyWord(ATT_MINUSTOLERANCE, 0, VT_DOUBLE);

   if (m_compData->getAttributes() != NULL && m_compData->getAttributes()->Lookup(minusTolKW, attrib))
      minusTol.Format("%d%%", round(fabs(attrib->getDoubleValue())));

   return minusTol;
}

CString CKonradComponent::GetKonradDevType(DeviceTypeTag devtypetag)
{
   if (devtypetag == deviceTypeUndefined)
      devtypetag = m_deviceType;

   switch (devtypetag)
   {
   case deviceTypeCapacitor:
   case deviceTypeCapacitorPolarized:
   case deviceTypeCapacitorTantalum:
      return "C";
   case deviceTypeDiode:
      return "D";
   case deviceTypeDiodeLed:
      return "D";
   case deviceTypeDiodeZener:
      return "Z";
   case deviceTypeInductor:
      return "L";
   case deviceTypeResistor:
      return "R";
   }

   return "?";
}

CString CKonradComponent::GetNormalizedValue()
{
   CString attrValue = GetValue();
   if (attrValue.IsEmpty())
      attrValue = "0";

   ComponentValues cval;
   cval.SetValue(attrValue);
  
#ifdef USE_RAW_VALUES
   // Original scheme wrote out value without metric factor, e.g. 1000 instead of 1K.
   // This is correct and works, but not customer's preference, see case 2223.
   CString retval = fpfmtExactPrecision(cval.GetValue(), 3);
#else
   // Value for Konrad should not have units, e.g. 100p is correct, 100pF is incorrect. See Konrad Spec.
   cval.SetUnits(valueUnitUndefined); // This stops getPrintableString() from appending units.
   CString retval = cval.GetPrintableString();

   // Case dts0100423258 - Konrad refuses to recognize upper case K for Kilo, requires lower case
   retval.Replace('K', 'k');
#endif

   return retval;
}

bool CKonradComponent::GetCompPin(CString pinRefname, CompPinStruct **retCompPin, NetStruct **retNet)
{
   FileStruct *file = m_doc->getFileList().GetOnlyShown(blockTypePcb);

   *retNet = NULL;
   *retCompPin = FindCompPin(file, this->GetRefname(), pinRefname, retNet);

   return (*retCompPin != NULL);
}

bool CKonradComponent::GetConnectedPin(int pinnum, CompPinStruct **retCompPin, NetStruct **retNet)
{
   // Get Refname of pin.
   // If Pinnum is 1 then take the first pin encountered.
   // If Pinnum is 2 then the second pin encountered.

   *retCompPin = NULL;
   *retNet = NULL;

   WORD pinMapKW = m_doc->RegisterKeyWord(ATT_DEVICETOPACKAGEPINMAP, 0, VT_STRING);

   FileStruct *file = m_doc->getFileList().GetOnlyShown(blockTypePcb);

   DataStruct *compdata = GetCompData();
   InsertStruct *compinsert = compdata->getInsert();
   int insertedBlockNum = compinsert->getBlockNumber();
   BlockStruct *insertedBlock = m_doc->getBlockAt(insertedBlockNum);

   if (insertedBlock != NULL)
   {
      //int pincount = 0;
      int qualifiedPinCount = 0;
      POSITION pos = insertedBlock->getHeadDataInsertPosition();
      while (pos != NULL)
      {
         DataStruct *data = insertedBlock->getNextDataInsert(pos);
         if (data->isInsertType(insertTypePin))
         {
            //pincount++;
            CString pinname = data->getInsert()->getRefname();
            CString refname = compinsert->getRefname();

            NetStruct *net = NULL;
            CompPinStruct *cp = FindCompPin(file, refname, pinname, &net);
            if (cp != NULL)
            {
               // It is No Connect only if it specifically says so
               bool isNoConnect = false;
               Attrib *attrib = NULL;
               if (cp->getAttributesRef() && cp->getAttributesRef()->Lookup(pinMapKW, attrib))
               {
                  CString pinTypeStr = get_attvalue_string(m_doc, attrib);
                  //PinTypeTag pinType = stringToPinTypeTag(pinTypeStr);  // Too bad for me, no pinTypeNoConnect is supported
                  if (pinTypeStr.CompareNoCase(ATT_VALUE_NO_CONNECT) == 0)
                     isNoConnect = true;
               }

               if (!isNoConnect)
               {
                  qualifiedPinCount++;  // this pin qualifies as possible keeper

                  if (qualifiedPinCount == pinnum) // then it is the one we want
                  {
                     *retCompPin = cp;
                     *retNet = net;
                     return true;
                  }
               }
            }
         }
      }
   }

   return false; // connected pin not found
}

CString CKonradComponent::GetPartNumber()
{
   CString partnumber("NOPOP");
   Attrib* attrib = NULL;
   WORD partnumKW = m_doc->RegisterKeyWord(ATT_PARTNUMBER, 0, VT_STRING);

   if ( m_compData->getAttributes() != NULL && m_compData->getAttributes()->Lookup(partnumKW, attrib) )
      partnumber = GetAttribValue(attrib);

   return partnumber;
}

CString CKonradComponent::GetMergedStatus()
{
   CString mergedStatus;
   Attrib* attrib = NULL;
   WORD mergedStatusKW = m_doc->RegisterKeyWord("MergedStatus", 0, VT_STRING);
  
   if ( m_compData->getAttributes() != NULL && m_compData->getAttributes()->Lookup(mergedStatusKW, attrib) )
      mergedStatus = GetAttribValue(attrib);

   return mergedStatus;
}

CString CKonradComponent::GetMergedValue()
{
   CString mergedValue;
   Attrib* attrib = NULL;
   WORD mergedValueKW = m_doc->RegisterKeyWord("MergedValue", 0, VT_STRING);

   if ( m_compData->getAttributes() != NULL && m_compData->getAttributes()->Lookup(mergedValueKW, attrib) )
      mergedValue = GetAttribValue(attrib);

   return mergedValue;
}

CString CKonradComponent::GetSubclass()
{
   CString val;
   Attrib* attrib = NULL;
   WORD kw = m_doc->RegisterKeyWord(ATT_SUBCLASS, 0, VT_STRING);

   if ( m_compData->getAttributes() != NULL && m_compData->getAttributes()->Lookup(kw, attrib) )
      val = GetAttribValue(attrib);

   return val;
}

CString CKonradComponent::GetLoaded()
{
   CString loadValue;
   Attrib* attrib = NULL;
   WORD loadValueKW = m_doc->RegisterKeyWord(ATT_LOADED, 0, VT_STRING);

   if ( m_compData->getAttributes() != NULL && m_compData->getAttributes()->Lookup(loadValueKW, attrib) )
      loadValue = GetAttribValue(attrib);

   return loadValue;
}


CString CKonradComponent::GetAttribValue ( Attrib *attrib )
{
	CString value;

	if ( attrib!= NULL )
	{
		value = attrib->getStringValue();
	}

	return value;
}

CKonradTestElementList &CKonradComponent::GetTestElementList(CSubclassList &subclassList)
{
   // Fill out test element list once, it will not change for duration of export.

   if (m_testElementList.GetCount() < 1)
   {
      CString subclassName( this->GetSubclass() );
      CSubclass *sc = subclassList.findSubclass(subclassName);
      if (sc == NULL || sc->getNumElements() < 1)
      {
         // No subclass definition, default to standard element based on component
         CKonradTestElement *testelement = new CKonradTestElement(*this);
         m_testElementList.Add(testelement);
      }
      else
      {
         for (int i = 0; i < sc->getNumElements(); i++)
         {
            CSubclassElement *el = sc->getElementAt(i);
            if (el != NULL)
            {
               CKonradTestElement *testelement = new CKonradTestElement(*this, el);
               m_testElementList.Add(testelement);
            }
         }
      }
   }

   return this->m_testElementList;
}

/****************************************************************************/
/*
*/

CKonradTestElement::CKonradTestElement(CKonradComponent &kcomp, CSubclassElement *el)
{
   // Note that Konrad only tests 2-pin parts, so we can ignore 
   // possibility of 3rd..nth pin in test element.

   m_scElement = el;

   m_kcomp = &kcomp;

   m_devtype = kcomp.GetDeviceTypeTag();
   m_konradDevtype = kcomp.GetKonradDevType();

   m_refname = kcomp.GetRefname();
   m_infovalue = kcomp.GetValue();
   m_testvalue = kcomp.GetNormalizedValue();
   m_partnumber = kcomp.GetPartNumber();
   m_ptol = kcomp.GetPlusTolerance();
   m_ntol = kcomp.GetMinusTolerance();
   m_mergedvalue = kcomp.GetMergedValue();
   m_mergedStatus = kcomp.GetMergedStatus();
   m_loadedValue = kcomp.GetLoaded();

   if (el != NULL)
   {
      // Subclass Element overrides a few of the values (some optional) and determines pin refnames
      m_devtype = el->getDeviceType();
      m_konradDevtype = kcomp.GetKonradDevType(m_devtype);

      CString pin1name( el->getPin1Name() );
      CString pin2name( el->getPin2Name() );
      if (!el->getName().IsEmpty())
         m_refname.Format("%s_%s", kcomp.GetRefname(), el->getName());
      else
         m_refname.Format("%s_%s_%s", kcomp.GetRefname(), pin1name, pin2name);

      kcomp.GetCompPin(pin1name, &m_pin1cp, &m_pin1net);
      kcomp.GetCompPin(pin2name, &m_pin2cp, &m_pin2net);

      if (!el->getValue().IsEmpty())
         m_testvalue = el->getValue();

      if (!el->getPlusTol().IsEmpty())
         m_ptol = el->getPlusTol();

      if (!el->getMinusTol().IsEmpty())
         m_ntol = el->getMinusTol();
   }
   else
   {
      // Standard comp handler, pin refnames don't matter, just find 1st and 2nd pins
      kcomp.GetConnectedPin(1, &m_pin1cp, &m_pin1net);
      kcomp.GetConnectedPin(2, &m_pin2cp, &m_pin2net);
   }
}

/****************************************************************************/
/*
*/

bool CKonradTestElement::Write(CStdioFileWriteFormat &outFile, CMapStringToString &probeMap, FILE* logFile)
{
   bool retval = true; // assume success for a change
 
   CString pin1netname;
   CString pin2netname;

   if (m_pin1cp == NULL && m_pin2cp == NULL)
   {
      fprintf(logFile, "%s %s has no connected pins.\n", IsGeneratedElement() ? "Element" : "Component", m_refname);
   }
   else
   {
      if (m_pin1cp == NULL)
      {
         if (this->IsGeneratedElement())
            fprintf(logFile, "No CompPin found for component %s pin %s.\n", this->GetKComp()->GetRefname(), this->GetSubclassElement()->getPin1Name());
         else
            fprintf(logFile, "No CompPin found for component %s 1st connected pin.\n", this->GetKComp()->GetRefname());
      }
      else
      {
         if (m_pin1net != NULL)
            pin1netname = m_pin1net->getNetName();
         else
            fprintf(logFile, "CompPin %s does not have a net.\n", m_pin1cp->getPinRef());
      }

      if (m_pin2cp == NULL)
      {
         if (this->IsGeneratedElement())
            fprintf(logFile, "No CompPin found for component %s pin %s.\n", this->GetKComp()->GetRefname(), this->GetSubclassElement()->getPin2Name());
         else if (m_pin1cp == NULL)
            fprintf(logFile, "No CompPin found for component %s 2nd connected pin.\n", this->GetKComp()->GetRefname());
         else
            fprintf(logFile, "Component %s has only one connected pin.\n", m_refname);
      }
      else
      {
         if (m_pin2net != NULL)
            pin2netname = m_pin2net->getNetName();
         else
            fprintf(logFile, "CompPin %s does not have a net.\n", m_pin2cp->getPinRef());
      }
   }


   
   CString pin1probeName("?");
   CString pin2probeName("?");

   // Find appropriate probes, squawk if none found, but output anyway
   CString mappedProbe;
   if (probeMap.Lookup(pin1netname, mappedProbe))
      pin1probeName = mappedProbe;
   if (probeMap.Lookup(pin2netname, mappedProbe))
      pin2probeName = mappedProbe;

   // Report on probe only if there is a compPin and a net. If no compPin or net
   // then we already reported a problem with the part.
   if (m_pin1cp != NULL && m_pin1net != NULL && pin1probeName.Compare("?") == 0)
   {
      retval = false;
      fprintf(logFile, "%-9s %s, net %s for first connected pin (%s) has no placed probe.\n", 
         IsGeneratedElement()?"Element":"Component", m_refname, pin1netname, m_pin1cp->getPinName());
   }

   if (m_pin2cp != NULL && m_pin2net != NULL && pin2probeName.Compare("?") == 0)
   {
      retval = false;
      fprintf(logFile, "%-9s %s, net %s for second connected pin (%s) has no placed probe.\n", 
         IsGeneratedElement()?"Element":"Component", m_refname, pin2netname, m_pin2cp->getPinName());
   }

   if ( !m_kcomp->getSettings()->getWriteInaccessible() && (pin1probeName.Compare("?") == 0 || pin2probeName.Compare("?") == 0))
      return retval;

   // We would like to write out the pins since the WRITE_INACCESSIBLE_TESTS is true.
   if (pin1probeName.Compare("?") == 0)
      pin1probeName = "0";
   if (pin2probeName.Compare("?") == 0)
      pin2probeName = "0";

   // Value overrides for two kinds of parts
   if (this->m_devtype == deviceTypeDiode)
   {
      m_infovalue = "Diode";
      if (atof(m_testvalue) == 0.0)
         m_testvalue = "700m";
   }

   if (this->m_devtype == deviceTypeDiodeLed)
   {
      m_infovalue = "Diode_LED";
      if (atof(m_testvalue) == 0.0)
         m_testvalue = "2";
   }
  
   outFile.writef("%s,", m_konradDevtype);

   // Check if part number exists
   if ( m_partnumber.CompareNoCase("NOPOP") != 0 )
      outFile.writef("UUT_1_%s_%s,", m_refname, m_partnumber);
   else
	  outFile.writef("UUT_1_%s,", m_refname);

   // If the merged status is "Primary", we would print out its merged value
   if ( m_mergedStatus.CompareNoCase("Primary") == 0 )
	  outFile.writef("%s;", m_mergedvalue);
   else
      outFile.writef("%s;", m_infovalue);
   
   if (m_ptol.Right(1).Compare("%") != 0)
      m_ptol += "%";
   if (m_ntol.Right(1).Compare("%") != 0)
      m_ntol += "%";

   outFile.writef("%s,", m_testvalue);
   outFile.writef("%s,", m_ntol);
   outFile.writef("%s;", m_ptol);
   outFile.writef("%s,", pin1probeName);
   outFile.writef("%s;\n", pin2probeName);

   // If it was writen, it is supported
   if (m_kcomp != NULL)
      m_kcomp->SetSupported(true);

   return retval;
}

/****************************************************************************/
/*
*/

CKonradSettings::CKonradSettings(bool reportFileNotFound)
: m_reportFileNotFoundError(reportFileNotFound)
{	   
   m_ExportUnloadedParts = false;
   m_WriteInaccessible = true;

   CString settingsFile( getApp().getExportSettingsFilePath("Konrad.out") );
   // This construct gets call alot in this export. Don't want to fill the log
   // with this settings file message.
   //{
   //   CString msg;
   //   msg.Format("Konrad: Settings file [%s].\n", settingsFile);
   //   getApp().LogMessage(msg);
   //}
   Load_Konradsettings(settingsFile);
}

CKonradSettings::~CKonradSettings()
{
	
}

bool CKonradSettings::Load_Konradsettings(const CString fname)
{
   CInFile inFile;

   if (!inFile.open(fname))
   {
      if (this->m_reportFileNotFoundError)
      {
         CString tmp;
         tmp.Format("File [%s] not found", fname);
         ErrorMessage( tmp, "Konrad Settings", MB_OK | MB_ICONHAND);
      }
      return false;
   }
   while (inFile.getNextCommandLine())
   {
	  if (inFile.isCommand(".EXPORT_UNLOADED_PARTS",2)) 
      {	 
		 inFile.parseYesNoParam(1,m_ExportUnloadedParts,false);   
      }
	  if (inFile.isCommand(".WRITE_INACCESSIBLE_TESTS",2)) 
      {	 
		 inFile.parseYesNoParam(1,m_WriteInaccessible,true);   
      }
   }
   return true;
}
