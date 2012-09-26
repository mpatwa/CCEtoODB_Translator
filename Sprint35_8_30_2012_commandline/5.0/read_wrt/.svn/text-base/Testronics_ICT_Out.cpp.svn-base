
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
#include "Testronics_ICT_Out.h"
#include "InFile.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//**************************************************************************


void TestronicsICT_WriteFile(const CString& filename, CCEtoODBDoc &doc, FormatStruct *format)
{
	FileStruct *file = doc.getFileList().GetExclusivelyShown(blockTypePcb);

	if (file == NULL)
	{
      int pcbCount = 0;
      int panelCount = 0;
      int otherCount = 0;
      int totalVisible = doc.getFileList().GetVisibleCount(pcbCount, panelCount, otherCount);
      CString info;
      CString otherinfo; // mention only pcb and panel files unless there really are "other" files visible
      if (otherCount > 0)
         otherinfo.Format("\n\t%d Other File%s", otherCount, otherCount != 1 ? "s" : "");
      info.Format("\nCurrently visible:\n\t%d PCB File%s\n\t%d Panel File%s%s",
         pcbCount, pcbCount != 1 ? "s" : "", panelCount, panelCount != 1 ? "s" : "", otherinfo );
		ErrorMessage(info, "Testronics ICT Export requires a single visible PCB file.", MB_OK);
		return;
	}

#ifdef REQUIRE_PLACED_PROBES
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
		ErrorMessage("Currently visible PCB has no placed probes, placed probes are required for this writer.\nRun probe placement before running Testronics ICT Write.", "Testronics ICT Write", MB_OK);
		return;
	}
#endif

   // Still here? Looks okay, try making Testronics output

	try
	{
      CTestronicsICTWriter Testronics(doc, *file, filename);
      Testronics.GatherComponents();
      Testronics.SortComponents();
      Testronics.WriteFile(filename);
      Testronics.WriteUnsupportedToLog();

	}
	catch (CString exception)
	{
		ErrorMessage(exception, "Testronics ICT Write", MB_OK);
	}
}


//**************************************************************************


CTestronicsICTWriter::CTestronicsICTWriter(CCEtoODBDoc &doc, FileStruct &file, CString outputFilename)
   : m_doc(doc)
	, m_file(file)
   , m_krSettings(false)  // issue (do not silence) out file err msgs
{
   // Open log file.
   m_logFile = getApp().OpenOperationLogFile("TestronicsICTWrite.log", m_localLogFilename);
   if (m_logFile == NULL) // error message already issued, just return.
      return;
   WriteStandardExportLogHeader(m_logFile, "Testronics ICT");

   FillProbeMap();
   
}

CTestronicsICTWriter::~CTestronicsICTWriter()
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

void CTestronicsICTWriter::FillProbeMap()
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

bool CTestronicsICTWriter::WriteFile(CString filename)
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

		ErrorMessage(errMsg, "Testronics ICT Write");

		return false;
	}

   bool retval = true;

   WriteHeader(outFile);

   // Writes all dev types, ignores devtype param
   outFile.writef("#PARTS TABLE\n");
   outFile.writef("#RefDes, DeviceType, Value, Tolerance, Empty, PartNumber, Subclass\n");
   if (!WriteComponents(outFile, deviceTypeUndefined))  retval = false;
   outFile.writef("\n#PINS TABLE\n");
   outFile.writef("#RefDes, PinName, NetName, ProbeNumber\n");
   if (!WritePins(outFile))  retval = false;


	outFile.close();

   // Mark didn't like this, so it is out, but easily revived if minds change
#ifdef REPORT_Testronics_ERROR
   if (!retval)
   {
      ErrorMessage("Errors encountered while writing output, see log file for details.", "Testronics ICT Write");
   }
#endif

	return retval;
}

void CTestronicsICTWriter::WriteHeader(CStdioFileWriteFormat &outFile)
{
   CString me(outFile.getFilePath());

   CString timeStamp;
	CTime t;
	t = t.GetCurrentTime();
   timeStamp.Format("%s", t.Format("%A, %B %d, %Y at %H:%M:%S"));

   CString mainHeaderLine;
   mainHeaderLine.Format("CAMCAD Professional %s - Licensed / Output File - Testronics", getApp().getVersionString());

   CString filenameHeaderLine;
   filenameHeaderLine.Format("Filename : %s\n", me);

   int longestLine = max(mainHeaderLine.GetLength(), filenameHeaderLine.GetLength());

   outFile.writef("#\n");
   outFile.writef("# %s\n", mainHeaderLine);
   outFile.writef("# "); for (int i = 0; i < mainHeaderLine.GetLength(); i++) outFile.write("="); outFile.write("\n");
   outFile.writef("#\n");
   outFile.writef("# %s\n", filenameHeaderLine);
   outFile.writef("#\n");
   outFile.writef("# Generated : %s\n", timeStamp);
   outFile.writef("#\n");
   outFile.writef("# "); for (int i = 0; i < longestLine; i++) outFile.write("-"); outFile.write("\n");
   outFile.writef("#\n");
}

bool CTestronicsICTWriter::WritePins(CStdioFileWriteFormat &outFile)
{
   // Pin output is based on comp pins, therefore by extension based on nets

   bool retval = true;

   POSITION netpos = this->m_file.getNetList().GetHeadPosition();
   while (netpos != NULL)
   {
      NetStruct *net = this->m_file.getNetList().GetNext(netpos);

      if (net != NULL)
      {
         POSITION cppos = net->getHeadCompPinPosition();
         while (cppos != NULL)
         {
            CompPinStruct *cp = net->getNextCompPin(cppos);
            
            if (cp != NULL)
            {
               bool writePin = true;

               // Only need to check loaded flag if we are not exporting unloaded parts
               if (!this->getSettings()->getExportUnloadedParts())
               {
                  CString sortableRefname = cp->getSortableRefDes();
                  CTestronicsComponent *kcomp = NULL;
                  m_componentList.Lookup(sortableRefname, kcomp);
                  writePin = (kcomp != NULL) ? (kcomp->GetLoaded().CompareNoCase("TRUE") == 0): false; // default false because it was not in componentList
               }

               if (writePin)
               {
                  CString probename;  // no default, output field is blank if no probe
                  CString mappedProbe;
                  if (this->m_probeMap.Lookup(net->getNetName(), mappedProbe))
                     probename = mappedProbe;

                  // RefDes, Pin name, Net name, Probe number

                  outFile.writef("%s,%s,%s,%s\n",
                     cp->getRefDes(), cp->getPinName(), net->getNetName(), probename);
               }
            }
         }
      }
   }

   return retval;
}

bool CTestronicsICTWriter::WriteComponents(CStdioFileWriteFormat &outFile, DeviceTypeTag targetDeviceType)
{
   bool retval = true;

   ////CString targetDevTypeStr = deviceTypeTagToFriendlyString(targetDeviceType);
   ////outFile.writef("//\n// %s\n", targetDevTypeStr);

   CString *sortableRefname;
   CTestronicsComponent *kcomp;
   for (m_componentList.GetFirstSorted(sortableRefname, kcomp); kcomp != NULL; m_componentList.GetNextSorted(sortableRefname, kcomp)) 
   {
      //ignoring device type:  if (kcomp->GetDeviceTypeTag() == targetDeviceType )
      {
         if ( getSettings()->getExportUnloadedParts() == true || kcomp->GetLoaded().CompareNoCase("false") != 0 )  // Check the setting from file
         {
            //ignoring merged status, if check is disabled:  if ( kcomp->GetMergedStatus().CompareNoCase("Ignored") != 0 )	// Don't print out the component with Ignore merged status
            {
               if (!kcomp->Write(outFile, m_probeMap, m_logFile))
               {
                  retval = false;
               }
            }
         }
      }
   }

   return retval;
}

void CTestronicsICTWriter::WriteUnsupportedToLog()
{
   // Call this function only after WriteComponents().
   // The "is supported" flag is set during the write process.

   fprintf(m_logFile, "\n\n");

   CString *sortableRefname;
   CTestronicsComponent *kcomp;
   for (m_componentList.GetFirstSorted(sortableRefname, kcomp); kcomp != NULL; m_componentList.GetNextSorted(sortableRefname, kcomp)) 
   {
      if (!kcomp->IsSupported())
      {
         fprintf(m_logFile, "Unsupported device type:  %7s  %s\n",
            kcomp->GetCompData()->getInsert()->getRefname(),
            deviceTypeTagToFriendlyString(kcomp->GetDeviceTypeTag()));
      }
   }
}

void CTestronicsICTWriter::GatherComponents()
{
	POSITION pos;
	for (pos = m_file.getBlock()->getHeadDataInsertPosition(); pos!=NULL; m_file.getBlock()->getNextDataInsert(pos))
	{
		DataStruct *data = m_file.getBlock()->getAtData(pos);

      if (data->isInsertType(insertTypePcbComponent))
      {
         CTestronicsComponent *kcomp = new CTestronicsComponent(&m_doc, data);
         m_componentList.SetAt(kcomp->GetSortableRefname(), kcomp);
      }
	}
}

static int AscendingRefnameSortFunc(const void *a, const void *b)  //*rcf Make this part of writer class
{
   CTestronicsComponent *ka = (CTestronicsComponent*)(((SElement*) a )->pObject->m_object);
   CTestronicsComponent *kb = (CTestronicsComponent*)(((SElement*) b )->pObject->m_object);

   return ka->GetSortKey().CompareNoCase(kb->GetSortKey());
}


void CTestronicsICTWriter::SortComponents()
{
   this->m_componentList.setSortFunction(AscendingRefnameSortFunc);
   this->m_componentList.Sort();
}


//**************************************************************************

CTestronicsComponent::CTestronicsComponent(CCEtoODBDoc *doc, DataStruct *data) 
: CObject()
, m_doc(doc)
, m_compData(data)
, m_deviceType(deviceTypeUnknown)
, m_supportedDevType(false)
, m_krSettings(true) // silence out file err msgs
{
   // This will get used a lot, don't want to look it up/calc it every time
   m_sortableRefname = data->getInsert()->getSortableRefDes();

   // This too
   Attrib *attr = NULL;
   WORD devTypeKW = m_doc->RegisterKeyWord(ATT_DEVICETYPE, 0, valueTypeString);
   if (data->lookUpAttrib(devTypeKW, attr))
      m_deviceType = stringToDeviceTypeTag( get_attvalue_string(m_doc, attr) );
}

CString CTestronicsComponent::GetValue()
{
   CString value("0");
   Attrib* attrib = NULL;
   WORD valueKW = m_doc->RegisterKeyWord(ATT_VALUE, 0, VT_STRING);

   if ( m_compData->getAttributes() != NULL && m_compData->getAttributes()->Lookup(valueKW, attrib) )
      value = GetAttribValue(attrib);

   //// This is from Konrad, not Testronics, came along with cloning Konrad to get Testronics, keep a little while
   // Case dts0100423258 - Testronics refuses to recognize upper case K for Kilo, requires lower case.
   // Though this is just an "info" (comment) value, we'll adjust the K's here too.
   ////value.Replace('K', 'k');

   return value;
}

CString CTestronicsComponent::GetPlusTolerance()
{
#ifdef KONRAD_STYLE__USES_0
   CString plusTol("0");
   Attrib* attrib = NULL;
   WORD plusTolKW = m_doc->RegisterKeyWord(ATT_PLUSTOLERANCE, 0, VT_DOUBLE);

   if (m_compData->getAttributes() != NULL && m_compData->getAttributes()->Lookup(plusTolKW, attrib) )
      plusTol.Format("%d", round(fabs(attrib->getDoubleValue())));
#else
   // It looks like Testronics wants blank output instead of 0
   CString plusTol;
   Attrib* attrib = NULL;
   WORD plusTolKW = m_doc->RegisterKeyWord(ATT_PLUSTOLERANCE, 0, VT_DOUBLE);

   if (m_compData->getAttributes() != NULL && m_compData->getAttributes()->Lookup(plusTolKW, attrib) )
   {
      double tol = fabs(attrib->getDoubleValue());
      if (tol > 0.0)
      {
         plusTol.Format("%.2f", tol);
         plusTol.TrimRight("0");  // drop trailing zeros
         plusTol.TrimRight(".");  // drop decimal pt if it is at right now
      }
   }
#endif

   return plusTol;
}

CString CTestronicsComponent::GetMinusTolerance()
{
   CString minusTol("0");
   Attrib* attrib = NULL;
   WORD minusTolKW = m_doc->RegisterKeyWord(ATT_MINUSTOLERANCE, 0, VT_DOUBLE);

   if (m_compData->getAttributes() != NULL && m_compData->getAttributes()->Lookup(minusTolKW, attrib))
      minusTol.Format("%d", round(fabs(attrib->getDoubleValue())));

   return minusTol;
}

CString CTestronicsComponent::GetTestronicsDevType()
{
   switch (m_deviceType)
   {
   case deviceTypeBattery:
      return "U";
   case deviceTypeCapacitor:
      return "C";
   case deviceTypeCapacitorArray:
      return "U";
   case deviceTypeCapacitorPolarized:
      return "C";
   case deviceTypeCapacitorTantalum:
      return "C";
   case deviceTypeConnector:
      return "J";
   case deviceTypeCrystal:
      return "U";
   case deviceTypeDiode:
      return "D";
   case deviceTypeDiodeArray:
      return "U";
   case deviceTypeDiodeLed:
      return "D";
   case deviceTypeDiodeLedArray:
      return "U";
   case deviceTypeDiodeZener:
      return "D";
   case deviceTypeFilter:
      return "U";
   case deviceTypeFuse:
      return "E";
   case deviceTypeIC:
      return "U";
   case deviceTypeICDigital:
      return "U";
   case deviceTypeICLinear:
      return "U";
   case deviceTypeInductor:
      return "L";
   case deviceTypeJumper:
      return "E";
   case deviceTypeNoTest:
      return "U";
   case deviceTypeOscillator:
      return "U";
   case deviceTypePotentiometer:
      return "U";
   case deviceTypePowerSupply:
      return "U";
   case deviceTypeRelay:
      return "U";
   case deviceTypeResistor:
      return "R";
   case deviceTypeResistorArray:
      return "U";
   case deviceTypeSpeaker:
      return "U";
   case deviceTypeSwitch:
      return "U";
   case deviceTypeTestPoint:
      return "U";
   case deviceTypeTransformer:
      return "T";
   case deviceTypeTransistor:
      return "Q";
   case deviceTypeTransistorArray:
      return "U";
   case deviceTypeTransistorFetNpn:
      return "U";
   case deviceTypeTransistorFetPnp:
      return "U";
   case deviceTypeTransistorMosfetNpn:
      return "U";
   case deviceTypeTransistorMosfetPnp:
      return "U";
   case deviceTypeTransistorNpn:
      return "Q";
   case deviceTypeTransistorPnp:
      return "Q";
   case deviceTypeTransistorScr:
      return "U";
   case deviceTypeTransistorTriac:
      return "U";
   case deviceTypeVoltageRegulator:
      return "U";
   case deviceTypeOpto:
      return "U";

   case deviceTypeUnknown:
   case deviceTypeUndefined:
   default:
      return "?";
   }

   return "?";
}

CString CTestronicsComponent::GetNormalizedValue()
{
   CString attrValue = GetValue();
   if (attrValue.IsEmpty())
      attrValue = "0";

   ComponentValues cval;
   cval.SetValue(attrValue);
  
   CString retval;  // No default, If no value attrib or value is zero return emtpy string

   if (cval.GetValue() != 0.0)
   {
      retval = cval.GetPrintableString();
      retval.MakeUpper(); // Mark says make it upper case
   }

   return retval;
}

bool CTestronicsComponent::GetConnectedPin(int pinnum, CompPinStruct **retCompPin, NetStruct **retNet)
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

CString CTestronicsComponent::GetPartNumber()
{
   CString partnumber("NOPOP");
   Attrib* attrib = NULL;
   WORD partnumKW = m_doc->RegisterKeyWord(ATT_PARTNUMBER, 0, VT_STRING);

   if ( m_compData->getAttributes() != NULL && m_compData->getAttributes()->Lookup(partnumKW, attrib) )
      partnumber = GetAttribValue(attrib);

   return partnumber;
}

CString CTestronicsComponent::GetSubclass()
{
   CString subclass; // No default
   Attrib* attrib = NULL;
   WORD attKW = m_doc->RegisterKeyWord(ATT_SUBCLASS, 0, VT_STRING);

   if ( m_compData->getAttributes() != NULL && m_compData->getAttributes()->Lookup(attKW, attrib) )
      subclass = GetAttribValue(attrib);

   return subclass;
}

CString CTestronicsComponent::GetMergedStatus()
{
   CString mergedStatus;
   Attrib* attrib = NULL;
   WORD mergedStatusKW = m_doc->RegisterKeyWord("MergedStatus", 0, VT_STRING);
  
   if ( m_compData->getAttributes() != NULL && m_compData->getAttributes()->Lookup(mergedStatusKW, attrib) )
      mergedStatus = GetAttribValue(attrib);

   return mergedStatus;
}

CString CTestronicsComponent::GetMergedValue()
{
   CString mergedValue;
   Attrib* attrib = NULL;
   WORD mergedValueKW = m_doc->RegisterKeyWord("MergedValue", 0, VT_STRING);

   if ( m_compData->getAttributes() != NULL && m_compData->getAttributes()->Lookup(mergedValueKW, attrib) )
      mergedValue = GetAttribValue(attrib);

   return mergedValue;
}

CString CTestronicsComponent::GetLoaded()
{
   CString loadValue;
   Attrib* attrib = NULL;
   WORD loadValueKW = m_doc->RegisterKeyWord("Loaded", 0, VT_STRING);

   if ( m_compData->getAttributes() != NULL && m_compData->getAttributes()->Lookup(loadValueKW, attrib) )
      loadValue = GetAttribValue(attrib);

   return loadValue;
}


CString CTestronicsComponent::GetAttribValue ( Attrib *attrib )
{
	CString value;

	if ( attrib!= NULL )
	{
		value = attrib->getStringValue();
	}

	return value;
}

bool CTestronicsComponent::Write(CStdioFileWriteFormat &outFile, CMapStringToString &probeMap, FILE *logFile)
{
   bool retval = true; // assume success for a change
 
   CString refname = GetCompData()->getInsert()->getRefname();
   CString testvalue = GetNormalizedValue();
   CString partnumber = GetPartNumber();
   CString subclass = GetSubclass();
   CString ptol = GetPlusTolerance();

   CString devicetype = GetTestronicsDevType();

   // RefDes, DeviceType, Value, Tolerance, Empty, Partnumber, User-defined(Subclass)

   outFile.writef("%s,%s,%s,%s,%s,%s,%s\n",
      refname, devicetype, testvalue, ptol, "" /*empty*/, partnumber, subclass);


   // If it was writen, it is supported
   this->SetSupported(true);

   return retval;
}

/****************************************************************************/
/*
*/

CTestronicsSettings::CTestronicsSettings(bool silenceErrMsg)
{	   
   m_ExportUnloadedParts = false;
   m_WriteInaccessible = true;

   CString settingsFile( getApp().getExportSettingsFilePath("Testronics.out") );
   // This construct gets call alot in this export. Don't want to fill the log
   // with this settings file message.
   //{
   //   CString msg;
   //   msg.Format("Testronics: Settings file [%s].\n", settingsFile);
   //   getApp().LogMessage(msg);
   //}
   Load_TestronicsSettings(settingsFile, silenceErrMsg);
}

CTestronicsSettings::~CTestronicsSettings()
{
	
}

bool CTestronicsSettings::Load_TestronicsSettings(const CString fname, bool silenceErrMsgs)
{
   CInFile inFile;

   if (!inFile.open(fname))
   {
      if (!silenceErrMsgs)
      {
         CString tmp;
         tmp.Format("File [%s] not found", fname);
         ErrorMessage(tmp, "Testronics Settings", MB_OK | MB_ICONHAND);
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
