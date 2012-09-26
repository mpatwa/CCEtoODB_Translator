// $Header: /CAMCAD/4.6/read_wrt/TeradyneSpectrum_Out.cpp 27    5/11/07 12:01p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright ?1994-2003. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "teradynespectrum_out.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void TeradyneSpectrum_WriteFiles(const CString filename, CCEtoODBDoc* pDoc, FormatStruct* format)
{
	try
	{
		CTeradyneSpectrumWriter spectrum(pDoc, filename);

		// output all file info
		spectrum.WriteFiles(filename);
	}
	catch (CString exception)
	{
		ErrorMessage(exception, "Teradyne Spectrum Write", MB_OK);
	}
}


/******************************************************************************
/******************************************************************************
* CTeradyneSpectrumWriter::CTeradyneSpectrumWriter
*/
CTeradyneSpectrumWriter::CTeradyneSpectrumWriter(CCEtoODBDoc* doc, CString outfilename)
: CIplWriter(*doc)
, m_netNameMap(doc)
, m_sharedIplInitSucceeded(false)
{
	m_eCHFormat = CH_ChennelNumber;
	m_bExportVias = true;
	m_sensorNumber = 0;

	if (doc == NULL)
		return;

	pDoc = doc;

   CFilePath logFilePath(outfilename);
   logFilePath.setBaseFileName("TeradyneSpectrum");
   logFilePath.setExtension("log");

   m_iplLogFp = NULL;
   m_localLogFilename = logFilePath.getPath();
   if (getApp().m_readWriteJob.IsActivated())
   {
      // Readwrite job aka vPlan usage.
      m_iplLogFp = getApp().OpenOperationLogFile("TeradyneSpectrum.log", m_localLogFilename);
   }
   else
   {
      // Standard CCASM CAMCAD product.
      if ((m_iplLogFp = fopen(m_localLogFilename, "wt")) == NULL) // rewrite file
      {
         ErrorMessage("Can not open Logfile!", m_localLogFilename, MB_ICONEXCLAMATION | MB_OK);
      }
   }

	if (m_iplLogFp != NULL)
	{
      WriteStandardExportLogHeader(m_iplLogFp, "Teradyne Spectrum");
	}

   if (m_sharedIplInitSucceeded = gatherData())   // gatherData() will report failure, no need to do it again here
   {
      m_netNameMap.CollectNames(pFile->getNetList());
      m_netNameMap.WriteReport(m_iplLogFp);

      // read rules for IPL
      CString settingsFile( getApp().getExportSettingsFilePath("TeradyneSpectrum.out") );
      {
         CString msg;
         msg.Format("Teradyne Spectrum: Settings file [%s].\n\n", settingsFile);
         getApp().LogMessage(msg);
      }
      readSettingsFile(settingsFile);

      if (!channelNumberStats.DoRemapping())
      {
         //check to make sure all probe names are numeric
         BOOL abort = FALSE;
         POSITION pos = testProbes.GetHeadPosition();
         while (pos)
         {
            if (!IsProbeNameNumeric(((CIplTestProbe*)testProbes.GetNext(pos))->GetName()))
            {
               abort = TRUE;
               break;
            }
         }
         if (abort)
         {
            //abort export
            CString err = "Non numeric probe names were found. Export process aborted.";
            throw err;
         }
      }

      // Preprocessing of channel #'s to probes using rules
      assignChannelNumbers();

      // Add in Unplaced probes
      // These ignore the rules (spacing rules, etc). Unplaced probes are simply
      // numbered sequentially increasing, starting at highest placed probe channel plus 1.
      gatherAndAssignUnplacedProbes();
   }
}

CTeradyneSpectrumWriter::~CTeradyneSpectrumWriter()
{
}

/******************************************************************************
* CIplWriter::readSettingsFile
*/
int CTeradyneSpectrumWriter::readSettingsFile(CString filename)
{
	CFileException e;
	CFormatStdioFile file;
	CString line;

	if (!file.Open(filename, CFile::modeRead, &e))
	{
      // no settings file found
      CString tmp;
		char msg[255];
		if (e.m_cause != e.none && e.GetErrorMessage(msg, 255))
			tmp.Format("%s [%s]", msg, filename);
		else
			tmp.Format("File [%s] not found", filename);

      ErrorMessage(tmp, "Teradyne Spectrum Settings", MB_OK | MB_ICONHAND);
		if (m_iplLogFp != NULL)
         fprintf(m_iplLogFp, "%s\n", tmp);

      return e.m_cause;
   }

   while (file.ReadString(line))
   {
		int tokPos = 0;		
		CString tok = line.Tokenize(" \t\n", tokPos);
      if (tok.IsEmpty())
			continue;

      if (tok[0] == '.')
      {
         if (!STRCMPI(tok, ".INDUCTOR_VALUER"))
         {
				tok = line.Tokenize(" \t\n", tokPos);
				if (!tok.IsEmpty())
               this->m_inductorResValAttrName = tok;
         }
         else if (!STRCMPI(tok, ".INDUCTOR_TOLR"))
         {
				tok = line.Tokenize(" \t\n", tokPos);
				if (!tok.IsEmpty())
               this->m_inductorResTolAttrName = tok;
         }
         else if (!STRCMPI(tok, ".PREASSIGNED_CHANNEL"))
         {
				tok = line.Tokenize(" \t\n", tokPos);
				if (tok.IsEmpty())
					continue;
	         int channelNum = atoi(tok);

				tok = line.Tokenize(" \t\n", tokPos);
				if (tok.IsEmpty())
					continue;
	         CString net = tok;

				if (channelNum >= 0 && !net.IsEmpty())
					channelNumberStats.AddPreassignedChannel(net, channelNum);
			}
         else if (!STRCMPI(tok, ".RESERVED_CHANNELS"))
         {
				tok = line.Tokenize(" \t\n", tokPos);
				if (tok.IsEmpty())
					continue;
	         CString resChannels = tok;

				channelNumberStats.SetReservedChannels(resChannels);
         }
         else if (!STRCMPI(tok, ".CHANNEL_SPACING"))
         {
				EChannelSpacing channelSpacing = ChannelSpacing16;

				tok = line.Tokenize(" \t\n", tokPos);
				if (tok.IsEmpty())
					continue;
				if (tok == "CSPACE_32")
					channelSpacing = ChannelSpacing32;
            else if (tok == "CSPACE_1")
               channelSpacing = ChannelSpacing1;

				channelNumberStats.SetChannelSpacing(channelSpacing);
			}
			else if (!STRCMPI(tok,".APPLY_TESTPIN_REMAPPING"))
			{
				tok = line.Tokenize(" \t\n", tokPos);
				if (tok.IsEmpty())
					continue;

				if (!tok.CompareNoCase("Y"))
					channelNumberStats.SetRemappingFlag(true);
			}			
			else if (!STRCMPI(tok,".CMAP_FORMAT"))
			{
				tok = line.Tokenize(" \t\n", tokPos);
				if (tok.IsEmpty())
					continue;

				if (!tok.CompareNoCase("net"))
					m_eCHFormat = CH_NetName;
				else
					m_eCHFormat = CH_ChennelNumber;
			}
			else if (!STRCMPI(tok,".EXPORT_PROBED_VIAS"))
			{
				tok = line.Tokenize(" \t\n", tokPos);
				if (tok.IsEmpty())
					continue;

				if (!tok.CompareNoCase("N"))
					m_bExportVias = false;					 
			}
      }
   } // END while (file.ReadString(line)) ...

   file.Close();
	return 0;
}

int CTeradyneSpectrumWriter::writeIPL(CString filepath)
{
	CFormatStdioFile outFile;
	CFileException e;
	
	if (!outFile.Open(filepath + "IPL.DAT", CFile::modeCreate|CFile::modeWrite, &e))
		return e.m_cause;
	
	// display the header information
	{
		CTime time = CTime::GetCurrentTime();
		outFile.WriteString("/* --------------------------------------------------- */\n");
		outFile.WriteString("/* %-51s */\n", outFile.GetFileName().MakeUpper());
		outFile.WriteString("/* Created by CCE to ODB++ v.%-31s */\n", getApp().getVersionString());
		outFile.WriteString("/* %-51s */\n", time.Format("%A, %B %d, %Y"));
		outFile.WriteString("/* --------------------------------------------------- */\n");
	}

	// Board name
	outFile.WriteString("\nBOARD\tNAME=\"%s\";\n\n",
		pFile != NULL && pFile->getBlock() != NULL ? pFile->getBlock()->getName() : "");

	writeIplComponents(&outFile);
	if (m_bExportVias)
		writeIplVias(&outFile);

	outFile.Close();

	return 0;
}

/******************************************************************************
* CTeradyneSpectrumWriter::writeComponents
*/
void CTeradyneSpectrumWriter::writeIplComponents(CFormatStdioFile* file)
{
	CIplTestOrder iplTestOrder;
	POSITION orderPos;
	for (orderPos = iplTestOrder.GetHeadPosition(); orderPos != NULL;)
	{
		DeviceTypeTag nowServingDevType = iplTestOrder.GetNext(orderPos);

		// Collect components of "now serving" type into list sorted by refdes
		CList<CIplComponent*> sortedByRefdes;
		POSITION pos = m_compMap.GetStartPosition();
		while (pos)
		{
			CString key;
			CIplComponent *iplComp = NULL;

			m_compMap.GetNextAssoc(pos, key, iplComp);

			if (iplComp && iplComp->GetType() == nowServingDevType)
			{
				CIplSmartRefdes newsmRefdes(iplComp->GetName());
				POSITION sbrPos = NULL;
				for (sbrPos = sortedByRefdes.GetHeadPosition(); sbrPos != NULL;)
				{
					CIplComponent *curIplComp = sortedByRefdes.GetAt(sbrPos);
					CIplSmartRefdes cursmRefdes(curIplComp->GetName());
					if (newsmRefdes < cursmRefdes) break;
					sortedByRefdes.GetNext(sbrPos);
				}
				if (sbrPos)
					sortedByRefdes.InsertBefore(sbrPos, iplComp);
				else
					sortedByRefdes.AddTail(iplComp);
			}
		}

		// Output the parts from the sorted list
		for (POSITION sbrPos = sortedByRefdes.GetHeadPosition(); sbrPos != NULL;)
		{
			CIplComponent *iplComp = sortedByRefdes.GetNext(sbrPos);
			CString iplString = getIplString(iplComp);
			file->WriteString("%s\n", iplString);
		}
		sortedByRefdes.RemoveAll();
	}

}

/******************************************************************************
* getIPLString
*/
CString CTeradyneSpectrumWriter::getIplString(CIplComponent *iplComp)
{
	if (iplComp == NULL)
		return "";

	CString subClass = "";
	CString prtLine = "";
	CString catLine = "";
	CString pinLine = "";
	CString valLine = "";
	CString tolLine = "";
	CString valLineL = "";
	CString tolLineL = "";
	CString scnLine = "";

	DeviceTypeTag type = iplComp->GetType();

	// Get subclass
	if (type == deviceTypeConnector)
		subClass.Format("CONN%d", iplComp->GetPinCount());
	else if (type == deviceTypeTestPoint)
		subClass = "VIA";
	else if (type == deviceTypeCapacitor)
		subClass = "CAP";
	else if (type == deviceTypeInductor)
		subClass = "IND";
	else if (type == deviceTypePotentiometer)
		subClass = "POT";
	else if (type == deviceTypeResistor)
		subClass = "RES";
	else if (type == deviceTypeResistorArray)
	{
		// Related to case 1675
		// Mark requested this specific handling of Resistor Array (08/04/05)
		// I explained it was different than all the rest, and maybe all should be
		// changed to be consistent. He said no, just change Resistor Array.
		subClass = iplComp->GetSubClassAttribute();
		if (subClass.IsEmpty())
			subClass = "RP_DB";
	}
	else if (type == deviceTypeCapacitorPolarized || type == deviceTypeCapacitorTantalum)
		subClass = "PCAP";
	else if (type == deviceTypeDiode)
		subClass = "DIODE";
	else if (type == deviceTypeDiodeLed)
		subClass = "LED";
	else if (type == deviceTypeDiodeZener)
		subClass = "ZENER";
	else if (type == deviceTypeFuse || type == deviceTypeJumper)
		subClass = "JUMPER";
	else if (type == deviceTypeTransistor || type == deviceTypeTransistorNpn)
		subClass = "NPN";
	else if (type == deviceTypeTransistorPnp)
		subClass = "PNP";
	else if (type == deviceTypeTransistorFetNpn		|| type == deviceTypeTransistorFetPnp ||
				type == deviceTypeTransistorMosfetNpn	|| type == deviceTypeTransistorMosfetPnp)
		subClass = "FET";
	else
		subClass = iplComp->GetSubClass();


	// Part Number
	CString partNumber = iplComp->GetPartNumber();
	if (!partNumber.IsEmpty())
	{
		prtLine.Format("\tFAB_FIL = \"%s\",\n", partNumber);
	}


	// Cap Opens Scan
	if (type == deviceTypeCapacitor || type == deviceTypeCapacitorPolarized || type == deviceTypeCapacitorTantalum ||
		type == deviceTypeConnector || 
		type == deviceTypeIC || type == deviceTypeICDigital || type == deviceTypeICLinear)
	{
		CString value = iplComp->GetCapacitiveOpens();
		if (value.CompareNoCase("true") == 0)
		{
			iplComp->SetSensorNumber(m_sensorNumber++);
			
			switch (type)
			{
			case deviceTypeCapacitor:
			case deviceTypeCapacitorPolarized:
			case deviceTypeCapacitorTantalum:
				scnLine.Format("\tCAP_SCAN = YES,\n\tSENSOR = %d,\n", iplComp->GetSensorNumber());
				break;
			case deviceTypeConnector:
				scnLine.Format("\tFRAME_SCAN = YES,\n\tSENSOR = %d,\n", iplComp->GetSensorNumber());
				break;
			case deviceTypeIC:
			case deviceTypeICDigital:
			case deviceTypeICLinear:
				scnLine.Format("\tFRAME_SCAN = YES,\n\tSENSOR = %d,\n", iplComp->GetSensorNumber());
				break;
			}
		}
	}
   
   // Diode Opens Scan
   if (
      type == deviceTypeIC || 
      type == deviceTypeICDigital || 
	   type == deviceTypeICLinear
      )
   {
		CString value = iplComp->GetDiodeOpens();
		if (value.CompareNoCase("true") == 0)
		{
			//iplComp->SetSensorNumber(m_sensorNumber++);
         if (scnLine.IsEmpty())
         {
            scnLine.Format("\tDELTA_SCAN = YES,\n"); // no sensor number
         }
         else
         {
            CString tmp;
            tmp.Format("\tDELTA_SCAN = YES,\n"); // no sensor number
            scnLine += tmp;
         }
      }
   }


	// Category
	switch (type)
	{
	case deviceTypeIC:
		catLine = "\tCATEGORY = ANALOG,\n";
		break;
	case deviceTypeICDigital:
		catLine = "\tCATEGORY = DIGITAL,\n";
		break;
	case deviceTypeICLinear:
		catLine = "\tCATEGORY = LINEAR,\n";
		break;
	default:
		// No category
		break;
	}


	// Get pins and netnames
	switch (type)
	{
	case deviceTypeBattery:
	case deviceTypeCapacitorArray:
	case deviceTypeConnector:
	case deviceTypeCrystal:
	case deviceTypeDiodeArray:
	case deviceTypeFilter:
	case deviceTypeIC:
	case deviceTypeICDigital:
	case deviceTypeICLinear:
	case deviceTypeNoTest:
	case deviceTypeOscillator:
	case deviceTypePowerSupply:
	case deviceTypeRelay:
	case deviceTypeResistorArray:
	case deviceTypeSpeaker:
	case deviceTypeSwitch:
	case deviceTypeTestPoint:
	case deviceTypeTransformer:
	case deviceTypeTransistorArray:
	case deviceTypeTransistorScr:
	case deviceTypeTransistorTriac:
	case deviceTypeVoltageRegulator:
		{
			// PINS = [netname1, netname2, ... ]

			CString tmpNetNames = "";
			CString netNames = "";
			CIplPin *iplPin = iplComp->GetFirstPin();
			while (iplPin != NULL)
			{
				if (tmpNetNames.GetLength() + iplPin->GetNetName().GetLength() >= 90)
				{
					if (netNames.IsEmpty())
						netNames = tmpNetNames;
					else
						netNames.AppendFormat("\n\t\t%s", tmpNetNames);

					tmpNetNames = "";
				}

				tmpNetNames.AppendFormat("%s,", cleanNetname(iplPin->GetNetName()));
					
				iplPin = iplComp->GetNextPin();
			}

			if (netNames.IsEmpty())
				netNames = tmpNetNames;
			else
				netNames.AppendFormat("\n\t\t%s", tmpNetNames);

			// Remove the last comma
			if (!netNames.IsEmpty())
				netNames.SetAt(netNames.GetLength()-1, '\0');

			pinLine.Format("\tPINS = (%s),\n", netNames);
		}
		break;

	case deviceTypeCapacitor:
	case deviceTypeInductor:
	case deviceTypePotentiometer:
	case deviceTypeResistor:
	case deviceTypeFuse:
	case deviceTypeJumper:
		{
			// LEAD1 = [netname1]
			// LEAD2 = [netname2]

			CIplPin* pin1 = iplComp->GetFirstPin();
			CIplPin* pin2 = iplComp->GetNextPin();


			CString netName1 = (pin1!=NULL)?pin1->GetNetName():"";
			CString netName2 = (pin2!=NULL)?pin2->GetNetName():"";

			pinLine.AppendFormat("\tLEAD1 = %s,\n", cleanNetname(netName1));
			pinLine.AppendFormat("\tLEAD2 = %s,\n", cleanNetname(netName2));
		}
		break;

	case deviceTypeCapacitorPolarized:
	case deviceTypeCapacitorTantalum:
		{
			// MINUS = [Negative netname]
			// PLUS = [Positive netname]

			CIplPin* minusPin = NULL;
			CIplPin* plusPin = NULL;

			CIplPin *iplPin = iplComp->GetFirstPin();
			while (iplPin != NULL)
			{
				if (minusPin == NULL && iplPin->GetPinType() == pinTypeNegative)
					minusPin = iplPin;
				else if (plusPin == NULL && iplPin->GetPinType() == pinTypePositive)
					plusPin = iplPin;

				iplPin = iplComp->GetNextPin();
			}

			CString netName1 = (minusPin!=NULL)?minusPin->GetNetName():"";
			CString netName2 = (plusPin!=NULL)?plusPin->GetNetName():"";

			pinLine.AppendFormat("\tPLUS = %s,\n", cleanNetname(netName1));
			pinLine.AppendFormat("\tMINUS = %s,\n", cleanNetname(netName2));
		}
		break;

	case deviceTypeDiode:
	case deviceTypeDiodeLed:
	case deviceTypeDiodeZener:
		{
			// ANODE = [Annode netname]
			// CATHODE = [Cathode netname]

			CIplPin* anodePin = NULL;
			CIplPin* cathodePin = NULL;

			CIplPin *iplPin = iplComp->GetFirstPin();
			while (iplPin != NULL)
			{
				if (anodePin == NULL && iplPin->GetPinType() == pinTypeAnode)
					anodePin = iplPin;
				else if (cathodePin == NULL && iplPin->GetPinType() == pinTypeCathode)
					cathodePin = iplPin;

				iplPin = iplComp->GetNextPin();
			}

			CString netName1 = (anodePin!=NULL)?anodePin->GetNetName():"";
			CString netName2 = (cathodePin!=NULL)?cathodePin->GetNetName():"";

			pinLine.AppendFormat("\tANODE = %s,\n", cleanNetname(netName1));
			pinLine.AppendFormat("\tCATHODE = %s,\n", cleanNetname(netName2));
		}
		break;

	case deviceTypeTransistor:
	case deviceTypeTransistorNpn:
	case deviceTypeTransistorPnp:
		{
			// BASE = [Base netname]
			// COLLECTOR = [Collector netname];
			// EMITTER = [Emitter netname];

			CIplPin* basePin = NULL;
			CIplPin* collectorPin = NULL;
			CIplPin* emitterPin = NULL;

			CIplPin *iplPin = iplComp->GetFirstPin();
			while (iplPin != NULL)
			{
				if (basePin == NULL && iplPin->GetPinType() == pinTypeBase)
					basePin = iplPin;
				else if (collectorPin == NULL && iplPin->GetPinType() == pinTypeCollector)
					collectorPin = iplPin;
				else if (emitterPin == NULL && iplPin->GetPinType() == pinTypeEmitter)
					emitterPin = iplPin;

				iplPin = iplComp->GetNextPin();
			}

			CString netName1 = (basePin!=NULL)?basePin->GetNetName():"";
			CString netName2 = (collectorPin!=NULL)?collectorPin->GetNetName():"";
			CString netName3 = (emitterPin!=NULL)?emitterPin->GetNetName():"";

			pinLine.AppendFormat("\tBASE = %s,\n", cleanNetname(netName1));
			pinLine.AppendFormat("\tCOLLECTOR = %s,\n", cleanNetname(netName2));
			pinLine.AppendFormat("\tEMITTER = %s,\n", cleanNetname(netName3));
		}
		break;

	case deviceTypeTransistorFetNpn:
	case deviceTypeTransistorFetPnp:
	case deviceTypeTransistorMosfetNpn:
	case deviceTypeTransistorMosfetPnp:
		{
			// DRAIN = [Base netname]
			// GATE = [Collector netname];
			// SOURCE = [Emitter netname];

			CIplPin* drainPin = NULL;
			CIplPin* gatePin = NULL;
			CIplPin* sourcePin = NULL;

			CIplPin *iplPin = iplComp->GetFirstPin();
			while (iplPin != NULL)
			{
				if (drainPin == NULL && iplPin->GetPinType() == pinTypeDrain)
					drainPin = iplPin;
				else if (gatePin == NULL && iplPin->GetPinType() == pinTypeGate)
					gatePin = iplPin;
				else if (sourcePin == NULL && iplPin->GetPinType() == pinTypeSource)
					sourcePin = iplPin;

				iplPin = iplComp->GetNextPin();
			}

			CString netName1 = (drainPin!=NULL)?drainPin->GetNetName():"";
			CString netName2 = (gatePin!=NULL)?gatePin->GetNetName():"";
			CString netName3 = (sourcePin!=NULL)?sourcePin->GetNetName():"";

			pinLine.AppendFormat("\tDRAIN = %s,\n", cleanNetname(netName1));
			pinLine.AppendFormat("\tGATE = %s,\n", cleanNetname(netName2));
			pinLine.AppendFormat("\tSOURCE = %s,\n", cleanNetname(netName3));
		}
		break;

	default:
		return "";
	}


	// Get tolerance and value
	if (type == deviceTypeCapacitor	|| type == deviceTypeCapacitorPolarized	||
		 type == deviceTypeInductor	|| type == deviceTypePotentiometer			|| 
		 type == deviceTypeResistor	||	type == deviceTypeCapacitorTantalum		||
		 type == deviceTypeDiodeZener || type == deviceTypeResistorArray)
	{
      double resVal = -1.;
      double resTol = -1.;
      CString resUnit;
      if (type == deviceTypeInductor)
      {
         // This is the only one that used valLineL and tolLineL (only inductors use TOLL and VALUEL)
         tolLineL.Format("\tTOLL = %s,\n", iplComp->GetPlusTolerance());
         valLineL.Format("\tVALUEL = %s,\n", iplComp->GetValue());

         // Inductors optionally have resistance and tolerance, if present these end up
         // in valLine and tolLine (VALUE and TOL).
         // If value is present then try for tol, but don't do tol without value.

         if (!m_inductorResValAttrName.IsEmpty())
         {
            CString valstr = iplComp->GetAttribute(m_inductorResValAttrName);
            if (!valstr.IsEmpty())
            {
               // Some val is present, try for tol
               if (!m_inductorResTolAttrName.IsEmpty())
                  tolLine.Format("\tTOL = %s,\n", iplComp->GetPlusTolerance(m_inductorResTolAttrName));

               // Extract value for later use in resistance value formatting
			      ComponentValues compValue(valstr);
			      compValue.GetValue(resVal, resUnit);
            }
         }
      }
      else if (type == deviceTypeResistor || 
         type == deviceTypeResistorArray)  // Case 2217, treat res array like regular res
      {
			// As found, assumes value is initially ohms
			ComponentValues compValue(iplComp->GetValue());
			compValue.GetValue(resVal, resUnit);
         tolLine.Format("\tTOL = %s,\n", iplComp->GetPlusTolerance());
      }
      else
      {
         // Standard, use TOL and VALUE in tolLine and valLine
         tolLine.Format("\tTOL = %s,\n", iplComp->GetPlusTolerance());
         valLine.Format("\tVALUE = %s,\n", iplComp->GetValue());
      }

      // Special format handling of resistor value and tolerance
		if (resVal > -1.)
		{
			// Convert to K or M Ohms, if value is large enough to be appropriate
			int ohms = round(resVal);
			if (ohms < 1000) {
				valLine.Format("\tVALUE = %sO,\n", trimValue(resVal)); // dts0100547137 Write original value otherwise 1.9 turns to 2, 0.4 turns to 0
			}
			else if (ohms < 1000000)
			{
				double kohms = (double)ohms / 1000.0;
				valLine.Format("\tVALUE = %sK,\n", trimValue(kohms));
			}
			else
			{
				double mohms = (double)ohms / 1000000.0;
				valLine.Format("\tVALUE = %sM,\n", trimValue(mohms));
			}
		}
	}

		
	// Append the information to create the IPL string
	CString iplString = "";
	iplString.Format("%s\tID = %s,\n", cleanSubclass(subClass), cleanRefdes(iplComp->GetName()));

	if (!catLine.IsEmpty())		iplString += catLine;
	if (!prtLine.IsEmpty())		iplString += prtLine;
	if (!valLineL.IsEmpty())	iplString += valLineL;
	if (!tolLineL.IsEmpty())	iplString += tolLineL;
	if (!valLine.IsEmpty())		iplString += valLine;
	if (!tolLine.IsEmpty())		iplString += tolLine;
	if (!scnLine.IsEmpty())		iplString += scnLine;
	if (!pinLine.IsEmpty())		iplString += pinLine;
	

	// Only if of type NO_TEST
	if (type == deviceTypeNoTest || type == deviceTypeTestPoint)
		iplString += "\tSTATUS = DONOTRUN;\n";


	// replace the last "," with ";"
	iplString = iplString.Trim();
	iplString = iplString.Left(iplString.GetLength()-1) + ";\n";

	return iplString;
}

/******************************************************************************
*/
CString CTeradyneSpectrumWriter::trimValue(double val)
{
   // Get value in "up to" %0.3f format.
   // Trim of trailing zeros.
   // If that leaves it with trailing decimal then trim that too.

   CString trimmedVal;
   trimmedVal.Format("%0.3f", val);
   trimmedVal.TrimRight("0");
   trimmedVal.TrimRight(".");

   return trimmedVal;
}

/******************************************************************************
* CTeradyneSpectrumWriter::writeVias
*/
void CTeradyneSpectrumWriter::writeIplVias(CFormatStdioFile* file)
{
   CString *ignoredKey;
   CSpectrumWriterVia *via = NULL;
   for (m_sortedVias.GetFirstSorted(ignoredKey, via); via != NULL; m_sortedVias.GetNextSorted(ignoredKey, via)) 
	{
      file->WriteString("VIA\tID = %s,\n", cleanRefdes(via->getRefname()));
      file->WriteString("\tPINS = (%s),\n", cleanNetname(via->getNetname()));
		file->WriteString("\tSTATUS = DONOTRUN;\n\n");
   }
}

/******************************************************************************
* CTeradyneSpectrumWriter::writeNets
*/
int CTeradyneSpectrumWriter::writeNets(CString filepath)
{
	CFormatStdioFile outFile;
	CFileException e;
	
	if (!outFile.Open(filepath + "Nets.asc", CFile::modeCreate|CFile::modeWrite, &e))
		return e.m_cause;
	
	// display the header information
	{
		CTime time = CTime::GetCurrentTime();
		outFile.WriteString("; ---------------------------------------------------\n");
		outFile.WriteString("; Filename : %s\n", outFile.GetFileName());
		outFile.WriteString("; Created by CCE to ODB++ v.%s\n", getApp().getVersionString());
		outFile.WriteString("; %s\n", time.Format("%A, %B %d, %Y"));
		outFile.WriteString("; ---------------------------------------------------\n");
		outFile.WriteString(";\n");
		outFile.WriteString("; Description:\n");
		outFile.WriteString(";	#1 	= net number\n");
		outFile.WriteString(";	(S)	= net type (S=Signal, P = power/ground)\n");
		outFile.WriteString(";	A(1)	= net name\n");
		outFile.WriteString(";	P1.2	= a pin on the net\n");
		outFile.WriteString(";	U20.4	= a pin on the net\n");
		outFile.WriteString(";\n");
	}

	// output all the nets and their comp/pins
   CString *ignoredKey;
   CIplNet *iplNet = NULL;
   for (m_netMap.GetFirstSorted(ignoredKey, iplNet); iplNet != NULL; m_netMap.GetNextSorted(ignoredKey, iplNet)) 
	{
		outFile.WriteString("#%d (%s) %s\n", iplNet->GetNumber(),
			(iplNet->IsPowerNet() || iplNet->IsGroundNet())?"P":"S", cleanNetname(iplNet->GetName()));


		// get all the comp/pins to output
		NetStruct *net = iplNet->GetNet();
		POSITION cpPos = net->getHeadCompPinPosition();
		while (cpPos)
		{
			CompPinStruct *cp = net->getNextCompPin(cpPos);

         // Refnames may be altered due to use of subclass test elements.
         // Also more than one element may use same pin, so need to process in loop to get them all.
         // If no elements use this pin then is probably a "normal" comp, no test elements used, so use 
         // cp's refname.

         CIplComponent *iplComp = this->m_compMap.LookupElement(cp->getRefDes(), cp->getPinName(), 1);
         if (iplComp != NULL)
         {
            // Output all elements
            int elementN = 1;
            while (iplComp != NULL)
            {
               outFile.WriteString("%s.%s\n", cleanRefdes(iplComp->GetName()), cp->getPinName());
               iplComp = this->m_compMap.LookupElement(cp->getRefDes(), cp->getPinName(), ++elementN);
            }
         }
         else
         {
            // Output one normal pin
            if(!cp->IsDiePin(pDoc->getCamCadData()))
               outFile.WriteString("%s.%s\n", cleanRefdes(cp->getRefDes()), cp->getPinName());
            else if (m_iplLogFp != NULL)
               fprintf(m_iplLogFp, "%s - Net: %s CompPin %s - Skipped CompPin for Die component.\n",
               outFile.GetFileName(), net->getNetName(), cp->getPinRef());
         }
		}


		if (m_bExportVias)
		{
			// get all the vias to output
			CIplTestProbe* testProbe = iplNet->GetFirstTestProbe();
			while (testProbe != NULL)
			{
				if (testProbe->GetProbedItemType() == insertTypeVia || testProbe->GetProbedItemType() == insertTypeBondPad)
					outFile.WriteString("%s\n", cleanRefdes(testProbe->GetProbedItemRefName()));
	       
				testProbe = iplNet->GetNextTestProbe(testProbe);
			}
		}


		outFile.WriteString("\n");
	} // END while (pos) ...

	outFile.Close();

	return 0;
}

/******************************************************************************
* CTeradyneSpectrumWriter::writeParts
*/
int CTeradyneSpectrumWriter::writeParts(CString filepath)
{
	CFormatStdioFile outFile;
	CFileException e;
	
	if (!outFile.Open(filepath + "Parts.asc", CFile::modeCreate|CFile::modeWrite, &e))
		return e.m_cause;
	
	// display the header information
	{
		CTime time = CTime::GetCurrentTime();
		outFile.WriteString("; ---------------------------------------------------\n");
		outFile.WriteString("; Filename : %s\n", outFile.GetFileName());
		outFile.WriteString("; Created by CCE to ODB++ v.%s\n", getApp().getVersionString());
		outFile.WriteString("; %s\n", time.Format("%A, %B %d, %Y"));
		outFile.WriteString("; ---------------------------------------------------\n");
		outFile.WriteString(";\n");
		outFile.WriteString("; Filename : Parts.asc\n");
		outFile.WriteString(";\n");
		outFile.WriteString(";  Description:\n");
		outFile.WriteString(";	P1	- Reference designator\n");
		outFile.WriteString(";	1,900 0.075 - device origin location on board\n");
		outFile.WriteString(";	0	- device rotation, degrees, CCW\n");
		outFile.WriteString(";	E5	- device location on board grid\n");
		outFile.WriteString(";	(T) 	- device surface (T - top, B - bottom)\n");
		outFile.WriteString(";	'PN-conn50'	- device name\n");
		outFile.WriteString(";	'PINCONN50'	- a device pattern graphic name\n");
		outFile.WriteString(";\n");
		outFile.WriteString("Part          X             Y             Rot           Grid          T/B           'Device', 'Outline'\n");
	}


	writePartsComponents(&outFile);
	if (m_bExportVias)
		writePartsVias(&outFile);


	outFile.Close();

	return 0;
}

/******************************************************************************
*/
int CSpectrumWriterVia::AscendingRefnameSortFunc(const void *a, const void *b)
{
   CSpectrumWriterVia* compA = (CSpectrumWriterVia*)(((SElement*) a )->pObject->m_object);
   CSpectrumWriterVia* compB = (CSpectrumWriterVia*)(((SElement*) b )->pObject->m_object);

   return (compA->getSortableRefname().CompareNoCase(compB->getSortableRefname()));
}

/******************************************************************************
* CTeradyneSpectrumWriter::writePartsComponents
*/
void CTeradyneSpectrumWriter::writePartsComponents(CFormatStdioFile* file)
{
   CString *ignoredKey;
   CIplComponent *iplComp = NULL;
   for (m_compMap.GetFirstSorted(ignoredKey, iplComp); iplComp != NULL; m_compMap.GetNextSorted(ignoredKey, iplComp)) 
   {
      file->WriteString("%-13s %-13.3f %-13.3f %-13.3f %-13s (%s)%10s '%s', '%s'\n",
         cleanRefdes(iplComp->GetName()), 
         iplComp->GetXLocation(), iplComp->GetYLocation(), RadToDeg(iplComp->GetRotation()),
         iplComp->GetGridLocation(), iplComp->IsOnTop()?"T":"B", "", 
         cleanSubclass(iplComp->GetComponentDevice()), iplComp->GetBlockName());
   }
}

/******************************************************************************
*/
void CTeradyneSpectrumWriter::CollectSortedVias()
{
   m_sortedVias.empty();

	POSITION pos = m_netMap.GetStartPosition();
	while (pos)
	{
		CString key;
		CIplNet *iplNet = NULL;

		m_netMap.GetNextAssoc(pos, key, iplNet);
		if (iplNet == NULL)
			continue;

		CIplTestProbe* testProbe = iplNet->GetFirstTestProbe();
		while (testProbe != NULL)
		{
			if (testProbe->GetProbedItemType() == insertTypeVia || testProbe->GetProbedItemType() == insertTypeBondPad)
			{
            int channelNumber = testProbe->GetChannelNumber();
            if (testProbe->IsPowerInjection())
               channelNumber = 10000;

            CString srn = testProbe->GetProbedItemSortableRefName();
            CSpectrumWriterVia *spectreVia = new CSpectrumWriterVia(testProbe->GetProbedItemSortableRefName(), 
               testProbe->GetProbedItemRefName(), 
               testProbe->GetGridLocation(),
               testProbe->GetProbedItemXLocation(), testProbe->GetProbedItemYLocation(), testProbe->GetProbedItemRotationDegrees(), 
               testProbe->IsProbedItemOnTop(), testProbe->IsProbedItemSMD(),
               "VIA", testProbe->GetProbedItemBlockName(),
               testProbe->GetNetName(), channelNumber);

            m_sortedVias.SetAt(testProbe->GetProbedItemSortableRefName(), spectreVia);
         }
			testProbe = iplNet->GetNextTestProbe(testProbe);
		}
	}

   m_sortedVias.setSortFunction(&CSpectrumWriterVia::AscendingRefnameSortFunc);
   m_sortedVias.Sort();
}

/******************************************************************************
*/
void CTeradyneSpectrumWriter::writePartsVias(CFormatStdioFile* file)
{
   CString *ignoredKey;
   CSpectrumWriterVia *via = NULL;
   for (m_sortedVias.GetFirstSorted(ignoredKey, via); via != NULL; m_sortedVias.GetNextSorted(ignoredKey, via)) 
	{
		file->WriteString("%-13s %-13.3f %-13.3f %-13.3f %-13s (%s)%10s '%s', '%s'\n",
         cleanRefdes(via->getRefname()), via->getX(), via->getY(), via->getTheta(),
         via->getGridLocation(), via->isTopSide()?"T":"B", "", 
         cleanSubclass(via->getSubclass()), via->getBlockname());
	}
}

/******************************************************************************
* CTeradyneSpectrumWriter::writePins
*/
int CTeradyneSpectrumWriter::writePins(CString filepath)
{
	CFormatStdioFile outFile;
	CFileException e;
	
	if (!outFile.Open(filepath + "Pins.asc", CFile::modeCreate|CFile::modeWrite, &e))
		return e.m_cause;
	
	// display the header information
	{
		CTime time = CTime::GetCurrentTime();
		outFile.WriteString("; ---------------------------------------------------\n");
		outFile.WriteString("; Filename : %s\n", outFile.GetFileName());
		outFile.WriteString("; Created by CCE to ODB++ v.%s\n", getApp().getVersionString());
		outFile.WriteString("; %s\n", time.Format("%A, %B %d, %Y"));
		outFile.WriteString("; ---------------------------------------------------\n");
		outFile.WriteString(";\n");
		outFile.WriteString(";  Description:\n");
		outFile.WriteString(";     First line:\n");
		outFile.WriteString(";        <part> - reference designator\n");
		outFile.WriteString(";        <T/B> - surface location (T = top, B = bottom)\n");
		outFile.WriteString(";        <device> - device type\n");
		outFile.WriteString(";     Second and following lines:\n");
		outFile.WriteString(";         <pin> - pin number\n");
		outFile.WriteString(";         <name> - pin name\n");
		outFile.WriteString(";         <x> - location of pin on board\n");
		outFile.WriteString(";         <y>	- location of pin on board\n");
		outFile.WriteString(";         <layer> - 0 = through hole pin location, 1 = SMD top, 2 = SMD bottom\n");
		outFile.WriteString(";         <net> - name of net attached to the pin\n");
		outFile.WriteString(":         <nail> - channel number(s) assigned to net\n");
		outFile.WriteString(";                  32000 - alternated probe locations available (see Nails.asc)\n");
		outFile.WriteString(";\n");
		outFile.WriteString("Part          T/B  Device\n");
		outFile.WriteString("Pin           Name          X             Y             Layer         Net           Nail(s)\n");
	}

	writePinsComponents(&outFile);
	if (m_bExportVias)
		writePinsVias(&outFile);

	outFile.Close();

	return 1;
}

/******************************************************************************
* CTeradyneSpectrumWriter::writePinsComponents
*/
void CTeradyneSpectrumWriter::writePinsComponents(CFormatStdioFile* file)
{
   CString *ignoredKey;
   CIplComponent *iplComp = NULL;
   for (m_compMap.GetFirstSorted(ignoredKey, iplComp); iplComp != NULL; m_compMap.GetNextSorted(ignoredKey, iplComp)) 
	{
		file->WriteString("PART %-8s (%s)  '%s'\n", cleanRefdes(iplComp->GetName()), iplComp->IsOnTop()?"T":"B",
			iplComp->GetDescription());

		// write all the pins
		CIplPin *iplPin = iplComp->GetFirstPin();
		while (iplPin != NULL)
		{
			// calculate the location of the pin in the board
			CPoint2d pinLocation(iplPin->GetXLocation(), iplPin->GetYLocation());
			if (!iplComp->IsOnTop()) pinLocation.x *= -1;
			CTMatrix transform;
			transform.rotateRadians(iplComp->GetRotation());
			transform.translate(iplComp->GetXLocation(), iplComp->GetYLocation());
			transform.transform(pinLocation);

			// get all the channel numbers for the net this pin is on
			CIplSortedArray sortedArray;
			CIplNet *iplNet = m_netMap.Lookup(iplPin->GetNetName());
			if (iplNet != NULL)
			{
				CIplTestProbe *iplTP = iplNet->GetFirstTestProbe();
				while (iplTP != NULL)
				{
               if (iplTP->IsPowerInjection())
                  sortedArray.Add(10000);
               else
               {
                  if(!iplTP->IsProbedDiePin())
                     sortedArray.Add(iplTP->GetChannelNumber());
                  else if (m_iplLogFp != NULL)
                     fprintf(m_iplLogFp, "%s - Net: %s Probe $%s - Skipped Probe for Die component.\n", file->GetFileName(),
                        iplNet->GetName(), iplTP->GetName());
               }

					iplNet->GetNextTestProbe(iplTP);
				}
			}


			// get the layer paramter of the pin
			// 0 = through hole pin location, 1 = SMD top, 2 = SMD bottom
			int layer = iplComp->IsOnTop()?1:2;
			if (!iplPin->IsSMD()) layer = 0;

			file->WriteString("%-14d%-14s%-14.3f%-14.3f%-14d%-14s%s\n", iplPin->GetPinNumber(), iplPin->GetName(),
				pinLocation.x, pinLocation.y, layer, cleanNetname(iplPin->GetNetName()), sortedArray.GetDelimitedString(",", false));

			iplPin = iplComp->GetNextPin();
		}

		file->WriteString("\n");
	}
}

/******************************************************************************
* CTeradyneSpectrumWriter::writePinsVias
*/
void CTeradyneSpectrumWriter::writePinsVias(CFormatStdioFile* file)
{
   CString *ignoredKey;
   CSpectrumWriterVia *via = NULL;
   for (m_sortedVias.GetFirstSorted(ignoredKey, via); via != NULL; m_sortedVias.GetNextSorted(ignoredKey, via)) 
	{
		// get the layer paramter of the pin
		// 0 = through hole pin location, 1 = SMD top, 2 = SMD bottom
      int layer = via->isTopSide()?1:2;
      if (!via->isSMD()) layer = 0;

      file->WriteString("PART %-8s (%s)  '%s'\n", cleanRefdes(via->getRefname()), via->isTopSide()?"T":"B", "VIA");

      file->WriteString("%-14d%-14s%-14.3f%-14.3f%-14d%-14s%d\n\n", 
         1, "1", via->getX(), via->getY(), layer, 
         cleanNetname(via->getNetname()), via->getChannelNumber());

   }
}

/******************************************************************************
* CTeradyneSpectrumWriter::writeTstParts
*/
int CTeradyneSpectrumWriter::writeTstparts(CString filepath)
{
	CFormatStdioFile outFile;
	CFileException e;
	
	if (!outFile.Open(filepath + "Tstparts.asc", CFile::modeCreate|CFile::modeWrite, &e))
		return e.m_cause;
	
	// display the header information
	{
		CTime time = CTime::GetCurrentTime();
		outFile.WriteString("; ---------------------------------------------------\n");
		outFile.WriteString("; Filename : %s\n", outFile.GetFileName());
		outFile.WriteString("; Created by CCE to ODB++ v.%s\n", getApp().getVersionString());
		outFile.WriteString("; %s\n", time.Format("%A, %B %d, %Y"));
		outFile.WriteString("; ---------------------------------------------------\n");
		outFile.WriteString(";\n");
		outFile.WriteString("Part          Grid          T/B           'Device', 'Outline'\n");
	}

	writeTstpartsComponents(&outFile);
	if (m_bExportVias)
		writeTstpartsVias(&outFile);

	outFile.Close();

	return 0;
}

/******************************************************************************
* CTeradyneSpectrumWriter::writeTstPartsComponents
*/
void CTeradyneSpectrumWriter::writeTstpartsComponents(CFormatStdioFile* file)
{
   CString *ignoredKey;
   CIplComponent *iplComp = NULL;
   for (m_compMap.GetFirstSorted(ignoredKey, iplComp); iplComp != NULL; m_compMap.GetNextSorted(ignoredKey, iplComp)) 
	{
		if (iplComp->GetType() != deviceTypeNoTest)
      {
         file->WriteString("%-13s %-13s (%s)%10s '%s', '%s'\n", cleanRefdes(iplComp->GetName()), iplComp->GetGridLocation(),
   			iplComp->IsOnTop()?"T":"B", "", iplComp->GetComponentDevice(), iplComp->GetBlockName());
	   }
   }
}

/******************************************************************************
* CTeradyneSpectrumWriter::writeTstPartsVias
*/
void CTeradyneSpectrumWriter::writeTstpartsVias(CFormatStdioFile* file)
{
   CString *ignoredKey;
   CSpectrumWriterVia *via = NULL;
   for (m_sortedVias.GetFirstSorted(ignoredKey, via); via != NULL; m_sortedVias.GetNextSorted(ignoredKey, via)) 
	{
      file->WriteString("%-13s %-13s (%s)%10s '%s', '%s'\n", cleanRefdes(via->getRefname()), via->getGridLocation(),
         via->isTopSide()?"T":"B", "", "VIA", "VIA");
   }
}

/******************************************************************************
* CTeradyneSpectrumWriter::writeCMAP
*/
int CTeradyneSpectrumWriter::writeCMAP(CString filepath)
{
	CFormatStdioFile outFile;
	CFileException e;
	CMapSortedStringToOb<CNetChannel> channelNameMap;

	
	if (!outFile.Open(filepath + "CMAP.txt", CFile::modeCreate|CFile::modeWrite, &e))
		return e.m_cause;

	// display the header information
	{
		CTime time = CTime::GetCurrentTime();
		outFile.WriteString("/* --------------------------------------------------- */\n");
		outFile.WriteString("/* %-51s */\n", outFile.GetFileName().MakeUpper());
		outFile.WriteString("/* Created by CCE to ODB++ v.%-31s */\n", getApp().getVersionString());
		outFile.WriteString("/* %-51s */\n", time.Format("%A, %B %d, %Y"));
		outFile.WriteString("/* --------------------------------------------------- */\n");
	}

	outFile.WriteString("CHANNELMAP\n");

	if (channelNumberStats.GetCount() <= 5120)
		writeCMapNets(&outFile, channelNameMap);
	else
		writeCMapNets5210(&outFile, channelNameMap);

	writeCMapChannels(&outFile, channelNameMap);

	outFile.WriteString("END\n");
	outFile.Close();

	return 0;
}

/******************************************************************************
* CTeradyneSpectrumWriter::writeCMapNets
*/
void CTeradyneSpectrumWriter::writeCMapNets(CFormatStdioFile* file, CMapSortedStringToOb<CNetChannel> &channelNameMap)
{
	// Output the NET section
	// This function is called when there is less than 5210 channels and nets;
	// otherwise the fucntion writeCMapNets5210 is called

	file->WriteString("\tNET\n");

   bool first = true;
   CString *ignoredKey;
   CIplNet *iplNet = NULL;
   for (m_netMap.GetFirstSorted(ignoredKey, iplNet); iplNet != NULL; m_netMap.GetNextSorted(ignoredKey, iplNet)) 
	{
		int channelCount = 0;
		CString channelNameList, channelName, channelNumber;
		CIplTestProbe* testProbe = iplNet->GetFirstTestProbe();
		while (testProbe != NULL)
		{
         // skip probes on Die Pins
         bool isDiePin = iplNet->GetNet()->IsDiePin(testProbe->GetProbedItemRefName(), testProbe->GetProbedItemPinName());

         if (!testProbe->IsPowerInjection() && 
            !isDiePin)
         {
			   if (m_eCHFormat == CH_NetName)
			   {
				   if (iplNet->GetTestProbeCount() > 1)
					   channelName.Format("CH_%s_CH%d", cleanNetname(iplNet->GetName()), ++channelCount);
				   else               
					   channelName.Format("CH_%s", cleanNetname(iplNet->GetName()));
			   }
			   else
			   {
               if (testProbe->IsPlaced())
               {
                  channelName.Format("CH_%d", testProbe->GetChannelNumber());
               }
               else
               {
                  channelName.Format("CH_9999_%d", testProbe->GetChannelNumber());
               }
			   }

            if (testProbe->IsPlaced())
               channelNumber.Format("%d", testProbe->GetChannelNumber());
            else
               channelNumber = "9999";

            if (!channelNameList.IsEmpty())
               channelNameList += ", ";
			   channelNameList += channelName;
			   channelNameMap.SetAt(channelName, new CNetChannel(channelName, channelNumber));
         }

         if(isDiePin && m_iplLogFp != NULL)
         {
            fprintf(m_iplLogFp, "%s - Net: %s Probe %d - Skipped Probe for Die component.\n", file->GetFileName(),
                  iplNet->GetName(), testProbe->GetChannelNumber());
         }
			testProbe = iplNet->GetNextTestProbe(testProbe);
		}

		if (!channelNameList.IsEmpty())
		{
			if (!first)
            file->WriteString(",\n");  // end previous output line
         first = false;

         // no newline char here, it gets handled later
			file->WriteString("\t\t\"%s\" = (%s)", cleanNetname(iplNet->GetName()), channelNameList);	
      }
	}

   // If first is false then we had output, mark the end of the output with semicolon
   if (!first)
      file->WriteString(";\n");

}

/******************************************************************************
* CTeradyneSpectrumWriter::writeCMapNets5210
*/
void CTeradyneSpectrumWriter::writeCMapNets5210(CFormatStdioFile* file, CMapSortedStringToOb<CNetChannel> &channelNameMap)
{
	// Output the NET section
	// This function is called when there is more than 5210 channels and nets.
	// It will ONLY output those nets with placed probes

	int totalChannelCount = 0;
	file->WriteString("\tNET\n");

   bool first = true;
   CString *ignoredKey;
   CIplNet *iplNet = NULL;
   for (m_netMap.GetFirstSorted(ignoredKey, iplNet); iplNet != NULL; m_netMap.GetNextSorted(ignoredKey, iplNet)) 
	{
		int channelCount = 0;
		CString channelNameList, channelName, channelNumber;
		CMapStringToString tmpChannelNameMap;

		CIplTestProbe* testProbe = iplNet->GetFirstTestProbe();
		while (testProbe != NULL)
		{
			if (!testProbe->IsPlaced())
				continue;
         
         // skip probes on Die Pins
         if(iplNet->GetNet()->IsDiePin(testProbe->GetProbedItemRefName(), testProbe->GetProbedItemPinName()))
         {
            if (m_iplLogFp != NULL)
               fprintf(m_iplLogFp, "%s - Net: %s Probe %d - Skipped Probe for Die component.\n",
               file->GetFileName(), iplNet->GetName(), testProbe->GetChannelNumber());
            continue;
         }

			if (m_eCHFormat == CH_NetName)
			{
				if (iplNet->GetTestProbeCount() > 1)
					channelName.Format("CH_%s_CH%d", cleanNetname(iplNet->GetName()), ++channelCount);
				else               
					channelName.Format("CH_%s", cleanNetname(iplNet->GetName()));
         }
			else
			{
				channelName.Format("CH_%d", testProbe->GetChannelNumber());
			}

         channelNumber.Format("%d", testProbe->GetChannelNumber());
         if (!channelNameList.IsEmpty())
            channelNameList += ", ";
         channelNameList += channelName;
			channelNameMap.SetAt(channelName, new CNetChannel(channelName, channelNumber));

			testProbe = iplNet->GetNextTestProbe(testProbe);
			totalChannelCount++;
		}

		if (!channelNameList.IsEmpty() && totalChannelCount <= 5210)
		{
			if (!first)
            file->WriteString(",\n"); // end previous output line
         first = false;

			file->WriteString("\t\t\"%s\" = (%s)", cleanNetname(iplNet->GetName()), channelNameList);
		}
	}

   // If first is false then we had output, mark the end of the output.
   if (!first)
      file->WriteString(";\n");

}


/******************************************************************************
*/
int CNetChannel::AscendingNetNameSortFunc(const void *a, const void *b)
{
   CNetChannel* chA = (CNetChannel*)(((SElement*) a )->pObject->m_object);
   CNetChannel* chB = (CNetChannel*)(((SElement*) b )->pObject->m_object);

   return (chA->getNetName().CompareNoCase(chB->getNetName()));
}

/******************************************************************************
* CTeradyneSpectrumWriter::writeCMapChannels
*/
void CTeradyneSpectrumWriter::writeCMapChannels(CFormatStdioFile* file, CMapSortedStringToOb<CNetChannel> &channelNameMap)
{
   channelNameMap.setSortFunction(&CNetChannel::AscendingNetNameSortFunc);
   channelNameMap.Sort();

	// Output the "CHANNEL" section
	file->WriteString("\tCHANNEL\n");

   CString *channelName = NULL;
   CNetChannel *thisNetChan = NULL;
   CNetChannel *nextNetChan = NULL;
   channelNameMap.GetFirstSorted(channelName, thisNetChan);
   while ( thisNetChan != NULL )
	{
      channelNameMap.GetNextSorted(channelName, nextNetChan);

      file->WriteString("\t\t%s = (%s)%s\n", thisNetChan->getNetName(), thisNetChan->getChannelNumber(),
         nextNetChan != NULL ? "," : ";");

      thisNetChan = nextNetChan;
	}
}

/******************************************************************************
* CTeradyneSpectrumWriter::writeTestConnect
*/
int CTeradyneSpectrumWriter::writeTestConnect(CString filepath)
{
	CFormatStdioFile outFile;
	CFileException e;
	
	if (!outFile.Open(filepath + "Testconnect.txt", CFile::modeCreate|CFile::modeWrite, &e))
		return e.m_cause;

	outFile.Close();

	return 0;
}

/******************************************************************************
* CIplWriter::WriteFiles
*/
int CTeradyneSpectrumWriter::WriteFiles(CString filepath)
{
   if (m_sharedIplInitSucceeded)
   {
      // Sort components in ascending refname order, affects several output files.
      m_compMap.setSortFunction(&CIplComponentMap::AscendingIplCompRefnameSortFunc);
      m_compMap.Sort();

      // Gather vias into sorted list, affects several output files.
      CollectSortedVias();

      // Sort the net list, affects several output files.
      m_netMap.setSortFunction(&CIplNetMap::AscendingNetNameSortFunc);
      m_netMap.Sort();

      // write the ipl file
      if (writeIPL(filepath) != 0 && m_iplLogFp != NULL)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "IPL.DAT");
      }

      // write the cmap file
      if (writeCMAP(filepath) != 0 && m_iplLogFp != NULL)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "CMAP.txt");
      }

      // write the testconnect file
      if (writeTestConnect(filepath) != 0 && m_iplLogFp != NULL)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "TestConnect.txt");
      }

      // write the board outline file
      if (writeBoardOutline(filepath) != 0 && m_iplLogFp != NULL)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "BdOutline.asc");
      }

      // write the board file
      if (writeBoard(filepath) != 0 && m_iplLogFp != NULL)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "Board.asc");
      }

      // write the Ind xRef file
      if (writeIndXRef(filepath) != 0 && m_iplLogFp != NULL)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "IndXRef.asc");
      }

      // write the nails file
      if (writeNails(filepath) != 0 && m_iplLogFp != NULL)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "Nails.asc");
      }

      // write the name xRef file
      if (writeNameXRef(filepath) != 0 && m_iplLogFp != NULL)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "NameXRef.asc");
      }

      // write the net to nail file
      if (writeNet2Nail(filepath) != 0 && m_iplLogFp != NULL)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "Net2Nail.asc");
      }

      // write the nets file
      if (writeNets(filepath) != 0 && m_iplLogFp != NULL)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "Nets.asc");
      }

      // write the parts file
      if (writeParts(filepath) != 0 && m_iplLogFp != NULL)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "Parts.asc");
      }

      // write the pins file
      if (writePins(filepath) != 0 && m_iplLogFp != NULL)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "Pins.asc");
      }

      // write the test parts file
      if (writeTstparts(filepath) != 0 && m_iplLogFp != NULL)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "Tstparts.asc");
      }
   }

	return 0;
}

CString CTeradyneSpectrumWriter::cleanNetname(CString camcadName)
{
   CString cleanName;

   m_netNameMap.Lookup(camcadName, cleanName);
   
   if (cleanName.IsEmpty())
   {
      // ErrorMessage(¡§Net name mapping failure¡¨)
      cleanName = camcadName;  // Better than nothing
   }

   return cleanName;
}
CString CTeradyneSpectrumWriter::cleanSubclass(CString camcadName)
{
	// Based on page 10 of Spectrum_Input_List_Spec.doc

	CString cleanName = camcadName;

	cleanName.Replace(' ', '_');  // 32 becomes 95;     space becomes _	
	cleanName.Replace('"', '_');  // 34 becomes 95;     " becomes _
	cleanName.Replace('(', '<');  // 40 becomes 60;     ( becomes <
	cleanName.Replace(')', '>');  // 41 becomes 62;     ) becomes >
	cleanName.Replace(',', '_');  // 44 becomes 95;     ,  becomes _
	cleanName.Replace('/', '_');  // 47 becomes 95;     / becomes _
	cleanName.Replace(';', '_');  // 59 becomes 95;     ; becomes _
	cleanName.Replace('[', '<');  // 91 becomes 60;     [ becomes <
	cleanName.Replace(']', '>');  // 93 becomes 62;     ] becomes >

	return cleanName;
}

CString CTeradyneSpectrumWriter::cleanRefdes(CString camcadName)
{
	// Based on page 10 of Spectrum_Input_List_Spec.doc

	CString cleanName = camcadName;

	cleanName.Replace(' ', '_');  // 32 becomes 95;     space becomes _
	cleanName.Replace('!', '_');  // 33 becomes 95;     ! becomes _
	cleanName.Replace('"', '_');  // 34 becomes 95;     " becomes _
	cleanName.Replace('#', 'N');  // 35 becomes 78;     # becomes N
	cleanName.Replace('$', '_');  // 36 becomes 95;     $ becomes _
	cleanName.Replace('%', '_');  // 37 becomes 95;     % becomes _
	cleanName.Replace('&', '_');  // 38 becomes 95;     & becomes _
	cleanName.Replace('\'', '_'); // 39 becomes 95;     ' becomes _
	cleanName.Replace('(', '_');  // 40 becomes 95;     ( becomes _
	cleanName.Replace(')', '_');  // 41 becomes 95;     ) becomes _
	cleanName.Replace('*', '_');  // 42 becomes 95;     * becomes _
	cleanName.Replace('+', '_');  // 43 becomes 95;     + becomes _
	cleanName.Replace(',', '_');  // 44 becomes 95;     ,  becomes _
	cleanName.Replace('.', '_');  // 46 becomes 95;     . becomes _
	cleanName.Replace('/', '_');  // 47 becomes 95;     / becomes _
	cleanName.Replace(':', '_');  // 58 becomes 95;     : becomes _
	cleanName.Replace(';', '_');  // 59 becomes 95;     ; becomes _
	cleanName.Replace('<', '_');  // 60 becomes 95;     < becomes _
	cleanName.Replace('=', '_');  // 61 becomes 95;     = becomes _
	cleanName.Replace('>', '_');  // 62 becomes 95;     > becomes _
	cleanName.Replace('?', '_');  // 63 becomes 95;     ? becomes _
	cleanName.Replace('@', '_');  // 64 becomes 95;     @ becomes _
	cleanName.Replace('[', '_');  // 91 becomes 95;     [ becomes _
	cleanName.Replace('\\', '_'); // 92 becomes 95;     \ becomes _
	cleanName.Replace(']', '_');  // 93 becomes 95;     ] becomes _
	cleanName.Replace('^', '_');  // 94 becomes 95;     ^ becomes _
	cleanName.Replace('`', '_');  // 96 becomes 95;     ` becomes _
	cleanName.Replace('{', '_');  // 123 becomes 95;    { becomes _
	cleanName.Replace('|', '_');  // 124 becomes 95;    | becomes _
	cleanName.Replace('}', '_');  // 125 becomes 95;    } becomes _
	cleanName.Replace('~', 'N');  // 126 becomes 78;    ~ becomes N

	return cleanName;
}

/******************************************************************************
* class CCleanedNetNameMap
*/

CCleanedNetNameMap::CCleanedNetNameMap(CCEtoODBDoc* doc)
:m_pDoc(doc)
{

}

int CCleanedNetNameMap::CollectNames(CNetList& NetList)
{
   // The mapping is done in two passes.
   // The first pass collects names that do not change due to cleaning.
   // This is to favor the existing names, i.e. do not change a name that is already okay.
   // The "polishing" process may cause some name to change to match an existing name, and
   // so one or the other has to be treated to make it unique. It should be the one that
   // already must change due to polishing anyway. The one that is already okay should be
   // allowed to remain as-is.
   // Doing the whole mapping in one pass leaves the mapping to the chance order of nets in
   // the nestlist, a flip of the coin as to which gets changed.
   // So first collect all names that do not need to change, then map in around them the
   // ones that do need to change.

   CMapStringToString tmpMap; // Map of cleanedName back to original net name


   // Pass 1 - Collect names that do not need to change

   POSITION netPos = NetList.GetHeadPosition();

   while (netPos)
   {
      NetStruct *net = NetList.GetNext(netPos);

      CString originalName = net->getNetName();
      CString cleanedName = polishNetname(originalName);

      // Case Matters !
      if (cleanedName.Compare(originalName) == 0)
      {
         // Name did not change, it is a 1st pass keeper
         tmpMap.SetAt(cleanedName, originalName);
         this->SetAt(originalName, cleanedName);
         m_pDoc->SetUnknownAttrib(&net->getAttributesRef(),"SPECTRUM_NETNAME", cleanedName, SA_OVERWRITE, NULL);
      }
   }

   // Pass 2 - Treat names that change when polished

   netPos = NetList.GetHeadPosition();

   while (netPos)
   {
      NetStruct *net = NetList.GetNext(netPos);

      CString originalName = net->getNetName();
      CString baseCleanedName = polishNetname(originalName);

      // We already dealt above with names that did not change due to polishing,
      // So in this pass we only need to treat names that do change.
      // In fact, MUST only treat names that do change, otherwise we make "fake" collisions.

      if (baseCleanedName.Compare(originalName) != 0)
      {
         CString cleanedName( baseCleanedName );
         CString ignoredValue;
         int count = 1;
      
         // If cleaned name is alreay used (exists in map) then loop adding a counter
         // suffix until name is unique (not already in map)
         while (tmpMap.Lookup(cleanedName, ignoredValue) ) 
         {
            cleanedName.Format("%s_%d", baseCleanedName, count++);
         }

         tmpMap.SetAt(cleanedName, originalName);
         this->SetAt(originalName, cleanedName);
         
         m_pDoc->SetUnknownAttrib(&net->getAttributesRef(),"SPECTRUM_NETNAME", cleanedName, SA_OVERWRITE, NULL);
      }
   }

   return this->GetCount();
}

CString CCleanedNetNameMap:: polishNetname(CString camcadName)
{
      // Based on page 10 of Spectrum_Input_List_Spec.doc
 
      CString cleanName = camcadName;
 
      cleanName.Replace(' ', '_');  // 32 becomes 95;     space becomes _     
      cleanName.Replace('"', '_');  // 34 becomes 95;     " becomes _
      cleanName.Replace('%', '_');  // 37 becomes 95;     % becomes _
      cleanName.Replace('(', '<');  // 40 becomes 60;     ( becomes <
      cleanName.Replace(')', '>');  // 41 becomes 62;     ) becomes >
      cleanName.Replace(',', '_');  // 44 becomes 95;      ,  becomes _
      cleanName.Replace('-', '~');  // 45 becomes 126;    - becomes ~
      cleanName.Replace(';', '_');  // 59 becomes 95;     ; becomes _
      cleanName.Replace('=', '_');  // 61 becomes 95;     = becomes _
      cleanName.Replace('[', '<');  // 91 becomes 60;     [ becomes <
      cleanName.Replace(']', '>');  // 93 becomes 62;     ] becomes >
 
      cleanName.MakeUpper();
      return cleanName;
}

void CCleanedNetNameMap::WriteReport(FILE *logFp)
{
   if (logFp != NULL)
   {
      fprintf(logFp, "\n Net Name Mapping\n");
      fprintf(logFp, "%20s --> %s\n", "CAMCAD", "Spectrum");

      POSITION pos = this->GetStartPosition();
      while (pos != NULL)
      {
         CString camcadName, spectrumName;
         this->GetNextAssoc(pos, camcadName, spectrumName);
         fprintf(logFp, "%20s --> %s\n", camcadName, spectrumName);
      }

      fprintf(logFp, "\n");
   }
}

