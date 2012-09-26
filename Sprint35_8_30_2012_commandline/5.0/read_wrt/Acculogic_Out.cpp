

#include "stdafx.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "TypedContainer.h"
#include "CompValues.h"
#include "Net_Util.h"
#include "RwLib.h"
#include "Acculogic_Out.h"
#include "RwUiLib.h"
#include "pcbutil.h"

void Acculogic_WriteFile(const CString& filename, CCEtoODBDoc &doc, FormatStruct *format)
{
	FileStruct *file = doc.getFileList().GetOnlyShown(blockTypePcb);

	if (file == NULL)
	{
		ErrorMessage("Need single visible PCB to export.", "Acculogic FPT Write", MB_OK);
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
#ifdef REQUIRE_PLACED_PROBES
	if (!hasOnePlacedProbe)
	{
		ErrorMessage("Currently visible PCB has no placed probes, placed probes are required for this writer.\nRun probe placement before running Acculogic FPT Write.", "Acculogic ICT Write", MB_OK);
		return;
	}
#endif

   
   // Still here? Looks okay, try making Acculogic output

	try
	{
      CAcculogicICTWriter Acculogic(doc, *file, filename);
      Acculogic.WriteFile(filename);
      Acculogic.WriteUnsupportedToLog();

	}
	catch (CString exception)
	{
		ErrorMessage(exception, "Acculogic FPT Write", MB_OK);
	}
}

//**************************************************************************


CAcculogicICTWriter::CAcculogicICTWriter(CCEtoODBDoc &doc, FileStruct &file, CString outputFilename) : CObject()
	, m_doc(doc)
	, m_file(file)
   , m_maxPartID(0)
{
   // Open log file.
   m_logFile = getApp().OpenOperationLogFile("AcculogicFPTWrite.log", m_localLogFilename);
   if (m_logFile == NULL) // error message already issued, just return.
      return;
   WriteStandardExportLogHeader(m_logFile, "Acculogic FPT");

   m_time = CTime::GetCurrentTime();

}

CAcculogicICTWriter::~CAcculogicICTWriter()
{
   // Close log file only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!m_localLogFilename.IsEmpty())
   {
      fclose(m_logFile);
   }
}

double CAcculogicICTWriter::AccUnits(double camcadValue)
{
   return m_doc.convertPageUnitsTo(pageUnitsMilliMeters, camcadValue);
}

#ifdef KONRAD
void CAcculogicICTWriter::FillProbeMap()
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
#endif

bool CAcculogicICTWriter::WriteFile(CString filename)
{
	CStdioFileWriteFormat outFile(2048);
	CExtFileException err;
	if (!outFile.open(filename, &err))
	{
		CString errMsg;
		char exceptionMsg[255];
		if (err.m_cause != err.none && err.GetErrorMessage(exceptionMsg, 255))
		{
			errMsg.Format("Error writing file!!\n\nFile could not be opened due to the following reason:\n   %s", exceptionMsg);
			fprintf(m_logFile, "Error: Could not open file for write {%s}\n", exceptionMsg);
		}
		else
			errMsg = "Error writing file!!\n\nFile could not be opened for writing.";

		ErrorMessage(errMsg, "Acculogic FPT Write");

		return false;
	}

   bool retval = true;
   
   GatherComponents();
   GatherPins();
   SortComponents();
   GatherProbes();
   SortProbes();
   ValidateProbes();

   if (!WriteHeader(outFile))    retval = false;

   for (int dt = deviceTypeUnknown; dt < deviceTypeUndefined; dt++)
   {
      if (!WriteComponents(outFile, (DeviceTypeTag)dt))    retval = false;
   }

   WriteComponentPins(outFile);
   WriteProbes(outFile);
   WriteFiducials(outFile);
   WriteFrameScanTests(outFile);
   WriteDeltaScanTests(outFile);

   outFile.writef("E\n"); // File ermination record

	outFile.close();

   // Mark didn't like this, so it is out, but easily revived if minds change
#ifdef REPORT_ACCULOGIC_ERROR
   if (!retval)
   {
      ErrorMessage("Errors encountered while writing output, see log file for details.", "Acculogic FPT Write");
   }
#endif

	return retval;
}

bool CAcculogicICTWriter::WriteHeader(CStdioFileWriteFormat &outFile)
{
   double pcbSizeX = 0.0, pcbSizeY = 0.0;

   BlockStruct *pcbBlk = m_file.getBlock();

   // Use board outline extent for board size
   CAcculogicBoard accuBrd(m_doc, pcbBlk);
   CExtent outlineExt = accuBrd.getBoardOutlineExtents();
   if (outlineExt.isValid())
   {
      pcbSizeX = outlineExt.getXsize();
      pcbSizeY = outlineExt.getYsize();
   }
   // Might not be an outline, if not use graphic extent for board size
   if ((pcbSizeX == 0.0 || pcbSizeY == 0.0) && pcbBlk != NULL)
   {
      CExtent ext = pcbBlk->getExtent(m_doc.getCamCadData());
      pcbSizeX = ext.getXsize();
      pcbSizeY = ext.getYsize();
   }

   outFile.writef("S");                                  // Record type identifier (S = System)
   outFile.writef(",%.24s", m_file.getName());           // Board name
   outFile.writef(",%.10s", m_time.Format("%Y/%m/%d"));  // Date
   outFile.writef(",%d", m_maxPartID);                   // Max component ID (comp ID's are made up integers)
   outFile.writef(",%d", m_netIdMap.GetCount());         // Number of nets involved
   outFile.writef(",%d", 1);                             // Number of groups (boards), max 99 -- only supporting one board 
   outFile.writef(",%.2f", AccUnits(pcbSizeX));          // PCB Size X
   outFile.writef(",%.2f", AccUnits(pcbSizeY));          // PCB Size Y
   outFile.writef(",VER2.00");                           // File version
   outFile.writef("\n");

   return true;
}

void CAcculogicICTWriter::GatherComponents()
{
   m_componentList.RemoveAll();

   int id = 0;

	POSITION pos;
	for (pos = m_file.getBlock()->getHeadDataInsertPosition(); pos!=NULL; m_file.getBlock()->getNextDataInsert(pos))
	{
		DataStruct *data = m_file.getBlock()->getAtData(pos);

      if (data->isInsertType(insertTypePcbComponent))
      {
         CAcculogicComponent *kcomp = new CAcculogicComponent(&m_doc, data);

         if (kcomp->IsSupported())
         {
            kcomp->SetPartID(++id);
         }

         m_componentList.Add(kcomp);
      }
	}

   m_maxPartID = id;
}

bool CAcculogicICTWriter::WriteComponents(CStdioFileWriteFormat &outFile, DeviceTypeTag targetDeviceType)
{
   bool retval = true;

   // No need to loop if dev type is unkonwn or undefined
   if (targetDeviceType != deviceTypeUnknown && targetDeviceType != deviceTypeUndefined)
   {
      //CString targetDevTypeStr = deviceTypeTagToFriendlyString(targetDeviceType);
      //outFile.writef("//\n// %s\n", targetDevTypeStr);

      for (int i = 0; i < m_componentList.GetCount(); i++)
      {
         CAcculogicComponent *kcomp = m_componentList.GetAt(i);
         if (kcomp->GetDeviceTypeTag() == targetDeviceType && kcomp->IsSupported())
         {
            if (!WriteComponent(outFile, kcomp))
               retval = false;
         }
      }
   }

   return retval;
}

bool CAcculogicICTWriter::WriteComponent(CStdioFileWriteFormat &outFile, CAcculogicComponent *kcomp)
{
   bool retval = true; // assume success for a change

   // Failure to write comp dur to being unsupported type is considered normal 
   // filtering and not an error. Note of the unsupported item is written in
   // the log.

   if (kcomp != NULL && kcomp->IsSupported())
   {
      CString refname = kcomp->GetCompData()->getInsert()->getRefname();
      CString testvalue = kcomp->GetCompValue();
      CString testunits = kcomp->GetCompValueUnits();
      CString ptol = kcomp->GetPlusTolerance();
      CString ntol = kcomp->GetMinusTolerance();
      CString accdevtype = kcomp->GetAcculogicDevType();
      CString compside = kcomp->GetCompData()->getInsert()->getPlacedTop() ? "T" : "B";
      double rotation = kcomp->GetCompData()->getInsert()->getAngleDegrees();
      int partid = kcomp->GetPartID();
      CString partnumber = kcomp->GetPartnumberAttrib();
      CString gridloc = kcomp->GetGridLocationAttrib();

      if (gridloc.GetLength() > 2)
      {
         fprintf(m_logFile, "Grid is too fine. Acculogic grid designation is limited to two characters, can not write \"%s\"\n", gridloc);
      }

      // Probe side, based on standard rules
      CString probeside;
      CString mountTech = kcomp->GetMountTechAttrib();
      if (mountTech.CompareNoCase("SMD") == 0)
         probeside = kcomp->GetCompData()->getInsert()->getPlacedTop() ? "T" : "B";  // SMT, probe same side as comp
      else
         probeside = kcomp->GetCompData()->getInsert()->getPlacedTop() ? "B" : "T";  // TH, probe opposite side of comp

      CPoint2d centroid;
      kcomp->GetCompData()->getInsert()->getCentroidLocation(m_doc.getCamCadData(), centroid);


      outFile.writef("P");                   // Record type designator
      outFile.writef(",1");                  // Existence of component (1 = Exists, 0 = Non-exist)
      outFile.writef(",1");                  // Group number (aka board number)
      outFile.writef(",%s", accdevtype);     // EXSEL part type
      outFile.writef(",%d", partid);         // CAD Part Number (a made up integer, apparantly a Fabmaster internal value)
      outFile.writef(",%.24s", refname);     // Part name (aka Refname)
      outFile.writef(",%.2s", gridloc);      // Grid Location (?) e.g. A1
      outFile.writef(",%s", testvalue);      // Value
      outFile.writef(",%s", testunits);      // Units  (e.g. Uf)
      outFile.writef(",");  // Second value (? Not supported in camcad)
      outFile.writef(",");  // Units for second value (? Not supported in camcad)
      outFile.writef(",%s", ptol);           // Upper Tolerance (eg. 12%)
      outFile.writef(",%s", ntol);           // Lower Tolerance (eg. 10%)
      outFile.writef(",%.2f", AccUnits(centroid.x));  // Part Centroid X
      outFile.writef(",%.2f", AccUnits(centroid.y));  // Part Centroid Y
      outFile.writef(",%.1f", rotation);     // Rotation (-359.9 .. 359.9)
      outFile.writef(",%s",   compside);     // Component Board Side (T or B)
      outFile.writef(",%s",   probeside);    // Probe Board Side (T or B)
      outFile.writef(",%.24s", partnumber);  // Part Name (Fabmaster Device NAM attrib)
      outFile.writef("\n");
   }

   return retval;
}

CString CAcculogicPin::GetAcculogicPinName()
{
   // Selected pin types get pin names based on pin type.
   // The rest get the pin refname aka pin number.

   CString pinName = this->GetPinName();

   WORD deviceToPackagePinMapKW = m_doc->RegisterKeyWord(StandardAttributeTagToName(standardAttributeDeviceToPackagePinMap), 0, VT_STRING);
   Attrib* attrib = NULL;

   if (m_cp != NULL &&
       m_cp->getAttributesRef() != NULL &&
       m_cp->getAttributesRef()->Lookup(deviceToPackagePinMapKW, attrib))
   {
      CString pinType;

      if (attrib != NULL && attrib->getValueType() == VT_STRING)
      {
         pinType = attrib->getStringValue();
      } 

      if (!pinType.IsEmpty())
      {
         if (pinType.CompareNoCase("Base") == 0)
            pinName = "B";
         else if (pinType.CompareNoCase("Collector") == 0)
            pinName = "C";
         else if (pinType.CompareNoCase("Emitter") == 0)
            pinName = "E";
         else if (pinType.CompareNoCase("Anode") == 0)
            pinName = "A";
         else if (pinType.CompareNoCase("Cathode") == 0)
            pinName = "K"; // yes, K.  C is Collector
         else if (pinType.CompareNoCase("Gate") == 0)
            pinName = "G";
         else if (pinType.CompareNoCase("Drain") == 0)
            pinName = "D";
         else if (pinType.CompareNoCase("Source") == 0)
            pinName = "S";
         else if (pinType.CompareNoCase("Positive") == 0)
            pinName = "+";
         else if (pinType.CompareNoCase("Negative") == 0)
            pinName = "-";
         else if (pinType.CompareNoCase(ATT_VALUE_NO_CONNECT) == 0)
            pinName = pinType;
      }
   }

   return pinName;  
}

bool CAcculogicICTWriter::WriteComponentPins(CStdioFileWriteFormat &outFile)
{
   for (int i = 0; i < m_pinList.GetCount(); i++)
   {
      CAcculogicPin *pin = m_pinList.GetAt(i);

      int netId = 0;
      m_netIdMap.Lookup(pin->GetNetName(), netId);

      //CompPinStruct *cp = pin->GetCompPin();
      CString pinName = pin->GetAcculogicPinName();

      // Filter out no-connect pins, output the rest

      if (pinName.CompareNoCase(ATT_VALUE_NO_CONNECT) != 0)
      {
         outFile.writef("L");                                      // Record type designator
         outFile.writef(",%d", pin->GetParentComp()->GetPartID()); // Parent component ID
         outFile.writef(",%s", pinName);                           // Pin number aka refname, sometimes mapped by pin type to special name
         outFile.writef(",%d", netId);                             // Net number
         outFile.writef(",%.2f", AccUnits(pin->GetX()));           // X coord
         outFile.writef(",%.2f", AccUnits(pin->GetY()));           // Y coord
         outFile.writef("\n");
      }
   }

   return true;
}

void CAcculogicICTWriter::GatherPins()
{
   m_pinList.RemoveAll();
   m_netIdMap.RemoveAll();

   WORD pinMapKW = m_doc.RegisterKeyWord(ATT_DEVICETOPACKAGEPINMAP, 0, VT_STRING);
   int prevNetId = 0;

   for (int i = 0; i < m_componentList.GetCount(); i++)
   {
      CAcculogicComponent *kcomp = m_componentList.GetAt(i);
      if (kcomp->IsSupported())
      {
         CBasesVector cmpBV(kcomp->GetCompData()->getInsert()->getBasesVector());
			////if (m_pcbBasesVector != NULL)				
			////	cmpBV.transform(m_pcbBasesVector->getTransformationMatrix());
         CTMatrix compMatrix = cmpBV.getTransformationMatrix();


         BlockStruct *insertedBlock = kcomp->GetInsertedCompGeometryBlock();
         if (insertedBlock != NULL)
         {
            for (CDataListIterator dataList(*insertedBlock, insertTypePin); dataList.hasNext();)
            {
               DataStruct *data = dataList.getNext();
               //if (data->isInsertType(insertTypePin))
               {
						CBasesVector pinBV(data->getInsert()->getBasesVector());
						pinBV.transform(compMatrix);

						double pinX = pinBV.getX();
						double pinY = pinBV.getY();
						//double pinT = pinBV.getRotationDegrees();

                  CString pinname = data->getInsert()->getRefname();
                  CString refname = kcomp->GetCompData()->getInsert()->getRefname();

                  NetStruct *net = NULL;
                  CompPinStruct *cp = FindCompPin(&m_file, refname, pinname, &net);
                  CAcculogicPin *accPin = new CAcculogicPin(&m_doc, kcomp, pinname, pinX, pinY, net != NULL ? net->getNetName() : "", cp);

                  this->m_pinList.Add(accPin);

                  if (net != NULL)
                  {
                     int netId;
                     if (!m_netIdMap.Lookup(net->getNetName(), netId))
                     {
                        m_netIdMap.SetAt(net->getNetName(), ++prevNetId);
                     }
                  }

#ifdef KONRAD
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
                  }
#endif
               }
            }
         }
      }
   }
}

bool CAcculogicICTWriter::WriteFrameScanTests(CStdioFileWriteFormat &outFile)
{
   // aka CAP OPENS test

   for (int i = 0; i < m_componentList.GetCount(); i++)
   {
      CAcculogicComponent *kcomp = m_componentList.GetAt(i);
      CString capOpensVal = kcomp->GetCapacitiveOpensAttrib();
      if (kcomp->IsSupported() && capOpensVal.CompareNoCase("true") == 0)
      {
         CExtent extent = kcomp->GetInsertedCompGeometryBlock()->getExtent(m_doc.getCamCadData());

         outFile.writef("M"); // Record type identifier
         outFile.writef(",%d", kcomp->GetPartID());                                  // CAD part number
         outFile.writef(",%.2f", AccUnits(extent.getXsize()));                       // Package width (size in X)
         outFile.writef(",%.2f", AccUnits(extent.getYsize()));                       // Package length (size in Y)
         outFile.writef(",%.2f", AccUnits(kcomp->GetCompHeightAttrib()));            // Package height
         outFile.writef(",%.16s", kcomp->GetInsertedCompGeometryBlock()->getName()); // Shape identifier
         outFile.writef("\n");
      }
   }

   return true;
}

bool CAcculogicICTWriter::WriteDeltaScanTests(CStdioFileWriteFormat &outFile)
{
   // aka DIODE OPENS test

      for (int i = 0; i < m_componentList.GetCount(); i++)
   {
      CAcculogicComponent *kcomp = m_componentList.GetAt(i);
      CString diodeOpensVal = kcomp->GetDiodeOpensAttrib();
      if (kcomp->IsSupported() && diodeOpensVal.CompareNoCase("true") == 0)
      {
      }
   }

   return true;
}

void CAcculogicICTWriter::ValidateProbes()
{
   // Probe refnames must be numbers in Acculogic format.
   // Use camcad probe refnames if they are all numbers.
   // If even one is not purely a number, assign new numeric names to all.

   bool allIsWell = true; // optimist, prove otherwise

   for (int i = 0; i < m_probeList.GetCount() && allIsWell; i++)
   {
      CAcculogicProbe *accprobe = m_probeList.GetAt(i);
      CString refname = accprobe->GetRefname();

      for (int j = 0; j < refname.GetLength(); j++)
      {
         if (!isdigit(refname.GetAt(j)))
            allIsWell = false;
      }
   }

   if (!allIsWell)
   {
      fprintf(m_logFile, "\nAcculogic Probe RefNames must be numeric. A non-numeric probe RefName was found in the CAMCAD data.\n");
      fprintf(m_logFile, "All probes have been renamed (renumbered) to compensate.\n\n");

      for (int i = 0; i < m_probeList.GetCount(); i++)
      {
         CAcculogicProbe *accprobe = m_probeList.GetAt(i);
         accprobe->SetRefname( (i+1) );
      }
   }
}

void CAcculogicICTWriter::GatherProbes()
{
   // Only interested in probes that are placed and are not power injection resources.

   m_probeList.RemoveAll();

   WORD testResourceKW = (WORD)m_doc.getStandardAttributeKeywordIndex(standardAttributeTestResource);
   WORD placementKW = (WORD)m_doc.getStandardAttributeKeywordIndex(standardAttributeProbePlacement);

   BlockStruct *pcbBlock = m_file.getBlock();

   if (pcbBlock != NULL)
   {
      for (CDataListIterator dataList(*pcbBlock, insertTypeTestProbe); dataList.hasNext();)
      {
         DataStruct *data = dataList.getNext();
         bool skip = false;

         // Skip Power Injection resources
         Attrib* attrib = NULL;
         if (data->lookUpAttrib(testResourceKW, attrib))
         {
            CString value = get_attvalue_string(&m_doc, attrib);    
            if (value.CompareNoCase("Power Injection") == 0)
               skip = true;
         }
   
         // Skip if not placed
         if (data->lookUpAttrib(placementKW, attrib))
         {
            CString placementStr = get_attvalue_string(&m_doc, attrib);
            if (placementStr.CompareNoCase("Unplaced") == 0)
               skip = true;
         }


         if (!skip)
            m_probeList.Add( new CAcculogicProbe( &m_doc, data ) );
      }
   }
}

bool CAcculogicICTWriter::WriteProbes(CStdioFileWriteFormat &outFile)
{
   WORD netnameKW = m_doc.RegisterKeyWord(ATT_NETNAME, 0, VT_STRING);

   for (int i = 0; i < m_probeList.GetCount(); i++)
   {
      CAcculogicProbe *accprobe = m_probeList.GetAt(i);
      DataStruct *data = accprobe->GetProbeData();
      if (data != NULL)
      {
         InsertStruct *insert = data->getInsert();

         CString netname;
         Attrib *attrib = NULL;
         if (data->getAttributesRef() != NULL && data->getAttributesRef()->Lookup(netnameKW, attrib))
            netname = get_attvalue_string(&m_doc, attrib);

         int netID = 0;
         if (!netname.IsEmpty())
            m_netIdMap.Lookup(netname, netID);
         
         //Skip probes on Die Pins
         if(accprobe->IsProbedDiePin())
         {
            fprintf(m_logFile, "J - Net: %s Probe $%s - Skipped Probe for Die pin.\n", netname, accprobe->GetRefname());
         }
         else
         {
            outFile.writef("J");                           // Record type designator
            outFile.writef(",%d", netID);                  // Net number
            outFile.writef(",%s", netname);                // Net name
            outFile.writef(",%s", accprobe->GetRefname()); // Nail number (probe refname, but alpha chars not allowed, only digits)
            outFile.writef(",%.2f", AccUnits(insert->getOriginX()));  // X
            outFile.writef(",%.2f", AccUnits(insert->getOriginY()));  // Y
            outFile.writef(",%s", insert->getPlacedTop() ? "T" : "B" );// PCB Side T=top, B=bot, D=both
            outFile.writef(",9");                          // Priority 9=high .. 1=low, 0=inhibit
            outFile.writef("\n");
         }
      }
   }
   
   return true;
}

void CAcculogicICTWriter::GatherFiducials(bool topFlag, DataStruct **fid1, DataStruct **fid2, DataStruct **fid3, DataStruct **fid4)
{

   int fidcount = 0;
   *fid1 = *fid2 = *fid3 = *fid4 = NULL;

   BlockStruct *pcbBlock = m_file.getBlock();
   if (pcbBlock != NULL)
   {
      for (CDataListIterator dataList(*pcbBlock, insertTypeFiducial); dataList.hasNext();)
      {
         DataStruct *data = dataList.getNext();
         InsertStruct *insert = data->getInsert();

         // (topFlag != insert->getPlacedBottom()) would do, but it isn't so plain to understand
         if ((topFlag && insert->getPlacedTop()) ||
            (!topFlag && insert->getPlacedBottom()))
         {
            fidcount++;
            if (fidcount == 1)
               *fid1 = data;
            else if (fidcount == 2)
               *fid2 = data;
            else if (fidcount == 3)
               *fid3 = data;
            else if (fidcount == 4)
               *fid4 = data;
         }

         if (fidcount == 4)
            return;
      }
   }
}

CString CAcculogicICTWriter::GetFidX(DataStruct *fid)
{
   CString valStr;

   if (fid != NULL)
   {
      valStr.Format("%.2f", AccUnits(fid->getInsert()->getOriginX()));
   }
   return valStr;
}

CString CAcculogicICTWriter::GetFidY(DataStruct *fid)
{
   CString valStr;

   if (fid != NULL)
   {
      valStr.Format("%.2f", AccUnits(fid->getInsert()->getOriginY()));
   }
   return valStr;
}

bool CAcculogicICTWriter::WriteFiducials(CStdioFileWriteFormat &outFile)
{
   DataStruct *fid1 = NULL, *fid2 = NULL, *fid3 = NULL, *fid4 = NULL;

   // TOP
   GatherFiducials(true, &fid1, &fid2, &fid3, &fid4);

   // Need at least one fid to write the record
   if (fid1 != NULL)
   {
      outFile.writef("F");                                                 // Record type identifier
      outFile.writef(",FIDTOP01");                                         // Refname
      outFile.writef(",%.2f", AccUnits(fid1->getInsert()->getOriginX()));  // X
      outFile.writef(",%.2f", AccUnits(fid1->getInsert()->getOriginY()));  // Y
      outFile.writef(",%s", fid2 == NULL ? "" : "FIDTOP02");               // Refname
      outFile.writef(",%s", GetFidX(fid2));                                // X
      outFile.writef(",%s", GetFidY(fid2));                                // Y
      outFile.writef(",%s", fid3 == NULL ? "" : "FIDTOP03");               // Refname
      outFile.writef(",%s", GetFidX(fid3));                                // X
      outFile.writef(",%s", GetFidY(fid3));                                // Y
      outFile.writef(",%s", fid4 == NULL ? "" : "FIDTOP04");               // Refname
      outFile.writef(",%s", GetFidX(fid4));                                // X
      outFile.writef(",%s", GetFidY(fid4));                                // Y
      outFile.writef(",T");                                                // PCB Side
      outFile.writef("\n");

   }

   // BOTTOM
   GatherFiducials(false, &fid1, &fid2, &fid3, &fid4);

   // Need at least one fid to write the record
   if (fid1 != NULL)
   {
      outFile.writef("F");                                                 // Record type identifier
      outFile.writef(",FIDBTM01");                                         // Refname
      outFile.writef(",%.2f", AccUnits(fid1->getInsert()->getOriginX()));  // X
      outFile.writef(",%.2f", AccUnits(fid1->getInsert()->getOriginY()));  // Y
      outFile.writef(",%s", fid2 == NULL ? "" : "FIDBTM02");               // Refname
      outFile.writef(",%s", GetFidX(fid2));                                // X
      outFile.writef(",%s", GetFidY(fid2));                                // Y
      outFile.writef(",%s", fid3 == NULL ? "" : "FIDBTM03");               // Refname
      outFile.writef(",%s", GetFidX(fid3));                                // X
      outFile.writef(",%s", GetFidY(fid3));                                // Y
      outFile.writef(",%s", fid4 == NULL ? "" : "FIDBTM04");               // Refname
      outFile.writef(",%s", GetFidX(fid4));                                // X
      outFile.writef(",%s", GetFidY(fid4));                                // Y
      outFile.writef(",B");                                                // PCB Side
      outFile.writef("\n");

   }

   return true;
}

void CAcculogicICTWriter::WriteUnsupportedToLog()
{
   // Call this function only after WriteComponents().
   // The "is supported" flag is set during the write process.

   fprintf(m_logFile, "\n\n");

   for (int i = 0; i < m_componentList.GetCount(); i++)
   {
      CAcculogicComponent *kcomp = m_componentList.GetAt(i);
      if (!kcomp->IsSupported())
      {
         fprintf(m_logFile, "Unsupported device type:  %7s  %s\n",
            kcomp->GetCompData()->getInsert()->getRefname(),
            deviceTypeTagToFriendlyString(kcomp->GetDeviceTypeTag()));
      }
   }
}


static int AscendingRefnameSortFunc(const void *a, const void *b)
{
   CAcculogicComponent *ka = *((CAcculogicComponent**)(a));
   CAcculogicComponent *kb = *((CAcculogicComponent**)(b));

   return ka->GetSortKey().CompareNoCase(kb->GetSortKey());
}


void CAcculogicICTWriter::SortComponents()
{
   this->m_componentList.setSortFunction(AscendingRefnameSortFunc);
   this->m_componentList.sort();
}


static int AscendingProbeRefnameSortFunc(const void *a, const void *b)
{
   CAcculogicProbe *ka = *((CAcculogicProbe**)(a));
   CAcculogicProbe *kb = *((CAcculogicProbe**)(b));

   int aval = atoi(ka->GetRefname());
   int bval = atoi(kb->GetRefname());

   return (aval - bval);
}

void CAcculogicICTWriter::SortProbes()
{
   this->m_probeList.setSortFunction(AscendingProbeRefnameSortFunc);
   this->m_probeList.sort();
}

//****************************************************************************************
//****************************************************************************************


CAcculogicComponent::CAcculogicComponent(CCEtoODBDoc *doc, DataStruct *data)
: m_doc(doc)
, m_compData(data)
, m_deviceType(deviceTypeUnknown)
, m_supportedDevType(false)
{
   // This will get used a lot, don't want to look it up/calc it every time
   m_sortableRefname = data->getInsert()->getSortableRefDes();

   // This too
   Attrib *attr = NULL;
   WORD devTypeKW = m_doc->RegisterKeyWord(ATT_DEVICETYPE, 0, valueTypeString);
   if (data->lookUpAttrib(devTypeKW, attr))
      m_deviceType = stringToDeviceTypeTag( get_attvalue_string(m_doc, attr) );
}

BlockStruct *CAcculogicComponent::GetInsertedCompGeometryBlock()
{
   DataStruct *compdata = GetCompData();
   InsertStruct *compinsert = compdata->getInsert();
   int insertedBlockNum = compinsert->getBlockNumber();
   BlockStruct *insertedBlock = m_doc->getBlockAt(insertedBlockNum);

   return insertedBlock;
}

double CAcculogicComponent::GetCompHeightAttrib()
{

   double value = 0.0;
   Attrib* attrib = NULL;
   WORD kw = m_doc->RegisterKeyWord(ATT_COMPHEIGHT, 0, VT_UNIT_DOUBLE);

   if (m_compData->getAttributes() == NULL || !m_compData->getAttributes()->Lookup(kw, attrib) || attrib == NULL)
      value = 0.0;
   else
      value = attrib->getDoubleValue();

   return value;
}

CString CAcculogicComponent::GetCapacitiveOpensAttrib()
{

   CString value;
   Attrib* attrib = NULL;
   WORD kw = m_doc->RegisterKeyWord(ATT_CAP_OPENS, 0, VT_STRING);

   if (m_compData->getAttributes() == NULL || !m_compData->getAttributes()->Lookup(kw, attrib) || attrib == NULL)
      value = "";
   else
      value = attrib->getStringValue();

   return value;
}

CString CAcculogicComponent::GetDiodeOpensAttrib()
{

   CString value;
   Attrib* attrib = NULL;
   WORD kw = m_doc->RegisterKeyWord(ATT_DIODE_OPENS, 0, VT_STRING);

   if (m_compData->getAttributes() == NULL || !m_compData->getAttributes()->Lookup(kw, attrib) || attrib == NULL)
      value = "";
   else
      value = attrib->getStringValue();

   return value;
}

CString CAcculogicComponent::GetGridLocationAttrib()
{

   CString value;
   Attrib* attrib = NULL;
   WORD kw = m_doc->RegisterKeyWord(ATT_GRID_LOCATION, 0, VT_STRING);

   if (m_compData->getAttributes() == NULL || !m_compData->getAttributes()->Lookup(kw, attrib) || attrib == NULL)
      value = "";
   else
      value = attrib->getStringValue();

   return value;
}

CString CAcculogicComponent::GetMountTechAttrib()
{
   CString value;
   Attrib* attrib = NULL;
   WORD kw = m_doc->RegisterKeyWord(ATT_TECHNOLOGY, 0, VT_STRING);

   if (m_compData->getAttributes() == NULL || !m_compData->getAttributes()->Lookup(kw, attrib) || attrib == NULL)
      value = "";
   else
      value = attrib->getStringValue();

   return value;
}

CString CAcculogicComponent::GetPartnumberAttrib()
{
   CString value;
   Attrib* attrib = NULL;
   WORD kw = m_doc->RegisterKeyWord(ATT_PARTNUMBER, 0, VT_STRING);

   if (m_compData->getAttributes() == NULL || !m_compData->getAttributes()->Lookup(kw, attrib) || attrib == NULL)
      value = "";
   else
      value = attrib->getStringValue();

   return value;
}

CString CAcculogicComponent::GetSubclassAttrib()
{
   CString value;
   Attrib* attrib = NULL;
   WORD kw = m_doc->RegisterKeyWord(ATT_SUBCLASS, 0, VT_STRING);

   if (m_compData->getAttributes() == NULL || !m_compData->getAttributes()->Lookup(kw, attrib) || attrib == NULL)
      value = "";
   else
      value = attrib->getStringValue();

   return value;
}

CString CAcculogicComponent::GetValueAttrib()
{
   CString value;
   Attrib* attrib = NULL;
   WORD valueKW = m_doc->RegisterKeyWord(ATT_VALUE, 0, VT_STRING);

   if (m_compData->getAttributes() == NULL || !m_compData->getAttributes()->Lookup(valueKW, attrib) || attrib == NULL)
      value = "0";
   else
      value = attrib->getStringValue();

   return value;
}

CString CAcculogicComponent::GetPlusTolerance()
{
   CString plusTol = "";
   Attrib* attrib = NULL;
   WORD plusTolKW = m_doc->RegisterKeyWord(ATT_PLUSTOLERANCE, 0, VT_DOUBLE);

   if (m_compData->getAttributes() == NULL || !m_compData->getAttributes()->Lookup(plusTolKW, attrib) || attrib == NULL)
      plusTol = "0%";
   else
      plusTol.Format("%d%%", round(fabs(attrib->getDoubleValue())));

   return plusTol;
}

CString CAcculogicComponent::GetMinusTolerance()
{
   CString minusTol = "";
   Attrib* attrib = NULL;
   WORD minusTolKW = m_doc->RegisterKeyWord(ATT_MINUSTOLERANCE, 0, VT_DOUBLE);

   if (m_compData->getAttributes() == NULL || !m_compData->getAttributes()->Lookup(minusTolKW, attrib) || attrib == NULL)
      minusTol = GetPlusTolerance();  // if minus tolerance not specified make same as plus tolerance
   else
      minusTol.Format("%d%%", round(fabs(attrib->getDoubleValue())));

   return minusTol;
}

CString CAcculogicComponent::GetAcculogicDevType()
{
   switch (m_deviceType)
   {
   case deviceTypeBattery:
      return "BTT";

   case deviceTypeCapacitor:
   case deviceTypeCapacitorPolarized:
   case deviceTypeCapacitorTantalum:
      return "C";
   case deviceTypeCapacitorArray:
      return GetSubclassAttrib();

   case deviceTypeConnector:
      return "CN";
   case deviceTypeCrystal:
      return "X";

   case deviceTypeDiode:
      return "D";
   case deviceTypeDiodeLed:
      return "LED";
   case deviceTypeDiodeZener:
      return "DZ";
   case deviceTypeDiodeArray:
      return GetSubclassAttrib();

   case deviceTypeFilter:
      return GetSubclassAttrib();
   case deviceTypeFuse:
      return "F";

   case deviceTypeIC:
   case deviceTypeICDigital:
   case deviceTypeICLinear:
      return "IC";

   case deviceTypeInductor:
      return "L";
   case deviceTypeJumper:
      return "J";
   case deviceTypeNoTest:
      return "";

   case deviceTypeOpto:
      return "PC";
   case deviceTypeOscillator:
      return GetSubclassAttrib();
   case deviceTypePotentiometer:
      return "RV";
   case deviceTypePowerSupply:
      return GetSubclassAttrib();
   case deviceTypeRelay:
      return "RY";
   case deviceTypeResistor:
      return "R";
   case deviceTypeResistorArray:
      return GetSubclassAttrib();
   case deviceTypeSpeaker:
      return "BZ";
   case deviceTypeSwitch:
      return "SW";
   case deviceTypeTestPoint:
      return "TP";
   case deviceTypeTransformer:
      return "T";

   case deviceTypeTransistor:
      return "N";
   case deviceTypeTransistorArray:
      return GetSubclassAttrib();
   case deviceTypeTransistorFetNpn:
      return "NF";
   case deviceTypeTransistorFetPnp:
      return "PF";
   case deviceTypeTransistorMosfetNpn:
      return "NF";
   case deviceTypeTransistorMosfetPnp:
      return "PF";
   case deviceTypeTransistorNpn:
      return "N";
   case deviceTypeTransistorPnp:
      return "P";
   case deviceTypeTransistorScr:
      return "SCR";
   case deviceTypeTransistorTriac:
      return GetSubclassAttrib();

   case deviceTypeVoltageRegulator:
      return GetSubclassAttrib();

   }

   return "?";
}

bool CAcculogicComponent::IsSupported()
{
   // A device type is supported if it has a mapping to an Acculogic dev type
   
   CString accdt = GetAcculogicDevType();

   return (!accdt.IsEmpty() && accdt.Compare("?") != 0);
}

CString CAcculogicComponent::GetCompValue()
{
   CString attrValue = GetValueAttrib();
   if (attrValue.IsEmpty())
      attrValue = "0";

   ComponentValues cval;
   cval.SetValue(attrValue);

   // Value for Acculogic should not have units or metric prefix char
   // None of the existing ComponentValue methods will give us this scaled value
   // with the metric prefix tag. So do what GetPrintableString does to scale
   // the number.

   double val = cval.GetValue();
   MetricPrefixTag prefixtag;
   double scaledval = scientificNormalization(val, prefixtag);
   CString retval;
   retval.Format("%lg", scaledval);

   return retval;
}

CString CAcculogicComponent::GetCompValueUnits()
{
   CString attrValue = GetValueAttrib();
   if (attrValue.IsEmpty())
      attrValue = "0";

   ComponentValues cval;
   cval.SetValue(attrValue);

   MetricPrefixTag prefixTag = cval.getMetricPrefix();
   CString prefixStr = metricPrefixTagToAbbreviatedString(prefixTag);

   ValueUnitTag valUnitTag = cval.GetUnits();
   CString valUnitStr = valueUnitToAbbreviatedString(valUnitTag);
   // We want one char, truncate to change Ohm to O
   valUnitStr.Truncate(1);

   CString retval = prefixStr + valUnitStr;

   return retval;
}


CAcculogicBoard::CAcculogicBoard(CCEtoODBDoc &doc, BlockStruct *pcbblock)
: m_doc(doc)
, m_pcbBlock(pcbblock)
{
}

DataStruct *CAcculogicBoard::GetBoardOutline()
{
   // Patterned after CBoardInstance in panelization.cpp

   if (m_pcbBlock != NULL)
   {
      POSITION pos = m_pcbBlock->getDataList().GetHeadPosition();
      while (pos != NULL)
      {
         DataStruct *data = m_pcbBlock->getDataList().GetNext(pos);

         if (data->getDataType() == T_POLY && data->getGraphicClass() == GR_CLASS_BOARDOUTLINE)
         {
            return data;
         }
      }
   }

   return NULL;
}

CExtent CAcculogicBoard::getBoardOutlineExtents()
{
   // Patterened after CBoardInstance in panelization.cpp

   DataStruct *boardOutline = GetBoardOutline();

   if (boardOutline == NULL)
      return CExtent(0.0, 0.0, 0.0, 0.0);

   return boardOutline->getPolyList()->getExtent();
}

/******************************************************************************
* CAcculogicProbe
*/
CAcculogicProbe::CAcculogicProbe(CCEtoODBDoc *doc, DataStruct *probedata)
{
   m_data = probedata; 
   m_refname = m_data != NULL ? m_data->getInsert()->getRefname() : ""; 

   SetProbedDiePin(doc->getCamCadData(), probedata);
}

/******************************************************************************
* CAcculogicProbe::SetProbedDiePin
*/
void CAcculogicProbe::SetProbedDiePin(CCamCadData &cancadData, DataStruct* data)
{
   m_IsProbedDiePin = ::IsProbedDiePin(cancadData, data);
}