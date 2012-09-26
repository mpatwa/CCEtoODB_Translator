// $Header: /CAMCAD/5.0/read_wrt/ScorpionOut.cpp 10    6/17/07 9:00p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "stdafx.h"
#include "scorpionout.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "RwUiLib.h"
#include "dcaentity.h"
#include "pcbutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


int cmpCSCOPin(const void* elem1,const void* elem2)
{
	const CSCOPin *p1,*p2;

	p1 = (CSCOPin*)((CPObject*)((SElement*)elem1)->pObject)->m_object;
	p2 = (CSCOPin*)((CPObject*)((SElement*)elem2)->pObject)->m_object;

	int retval = p1->GetSortKey().Compare(p2->GetSortKey());

	return retval;
}

CString GetScorpionDeviceType(CString ccDeviceType)
{
	ccDeviceType.MakeUpper();
	if (ccDeviceType == "BATTERY")
		return "BATTERY";
	else
	if (ccDeviceType == "CAPACITOR")
		return "CAP";
	else
	if (ccDeviceType == "CAPACITOR_POLARIZED" || ccDeviceType == "CAPACITOR_TANTALUM")
		return "PCAP";
	else
	if (ccDeviceType == "Connector")
		return "CONNEC";
	else
	if (ccDeviceType == "DIODE")
		return "DIODE";
	else
	if (ccDeviceType == "DIODE_LED")
		return "LED";
	else
	if (ccDeviceType == "DIODE_ZENER")
		return "ZDIODE";
	else
	if (ccDeviceType == "FUSE")
		return "FUSE";
	else
	if (ccDeviceType == "IC" || ccDeviceType == "IC_DIGITAL" ||
		ccDeviceType == "IC_LINEAR")
		return "IC";
	else
	if (ccDeviceType == "INDUCTOR")
		return "IND";
	else
	if (ccDeviceType == "POTENTIOMETER")
		return "VRES";
	else
	if (ccDeviceType == "RELAY")
		return "RELAY";
	else
	if (ccDeviceType == "RESISTOR")
		return "RES";
	else
	if (ccDeviceType == "TRANSISTOR" || ccDeviceType == "TRANSISTOR_NPN" ||
		ccDeviceType == "TRANSISTOR_PNP")
	return "TRAN";
	else
	if (ccDeviceType == "TRANSISTOR_FET_NPN" || ccDeviceType == "TRANSISTOR_FET_PNP" ||
		ccDeviceType == "TRANSISTOR_MOSFET_NPN" || ccDeviceType == "TRANSISTOR_MOSFET_PNP")
		return "FET";
	else
	if (ccDeviceType == "TRANSISTOR_TRIAC")
		return "TRIAC";
	else
	if (ccDeviceType == "CAPACITOR_ARRAY" || ccDeviceType == "CRYSTAL" ||
		ccDeviceType == "DIODE_ARRAY" || ccDeviceType == "FILTER" ||
		ccDeviceType == "NO_TEST" || ccDeviceType == "OSCILLATOR" ||
		ccDeviceType == "POWER_SUPPLY" || ccDeviceType == "RESISTOR_ARRAY" ||
		ccDeviceType == "SPEAKER" || ccDeviceType == "SWITCH" ||
		ccDeviceType == "TEST_POINT" || ccDeviceType == "TRANSFORMER" ||
		ccDeviceType == "TRANSISTOR_ARRAY" || ccDeviceType == "TRANSISTOR_SCR" ||
		ccDeviceType == "VOLTAGE_REGULATOR")
		return "OTHER";
	else
		return "";
}

/******************************************************************************
* Scorpion_WriteFile
*/
void Scorpion_WriteFile(CString filename, CCEtoODBDoc &doc, FormatStruct *format)
{
	FileStruct *file = doc.getFileList().GetOnlyShown(blockTypePcb);

	if (file == NULL)
	{
		ErrorMessage("No current design choosen to be written.", "Scorpion Writer");
		return;
	}

	ScorpionWriter scoWriter(&doc);
	scoWriter.Write(filename);
}



/******************************************************************************
* CSCOPin::CSCOPin
*/
CSCOPin::CSCOPin(DataStruct &data, CCEtoODBDoc &doc)
{
	InsertStruct *insert = data.getInsert();
	
	m_pData = &data;
	m_sName = insert->getRefname();
	m_ptLocation = insert->getOrigin2d();
	m_sSortKey = generateSortKey(m_sName);
	// m_sType will be retrieved later in gatherComponentInfo because it is found on the comp/pin in the net list

	Attrib *attrib = NULL;
	int keyword = doc.RegisterKeyWord(ATT_COMPPINNR, 0, valueTypeInteger);
	if (data.lookUpAttrib(keyword, attrib))
		m_iNumber = attrib->getIntValue();
	else
		m_iNumber = atoi(m_sName);

}

CSCOPin::CSCOPin(const CSCOPin &other)
{
	if (&other != this)
	{
		m_iNumber = other.m_iNumber;
		m_sName = other.m_sName;
		m_sType = other.m_sType;
		m_ptLocation = other.m_ptLocation;
	}
}

CSCOPin::~CSCOPin()
{
}

CString CSCOPin::generateSortKey(CString name)
{
	// create sort name
	CString temp, buf;
	bool alpha = true, lastAlpha = true;

	name.MakeUpper();
	for (int i=0; i<name.GetLength(); i++)
	{
		if (isalpha(name[i]))
			alpha = true;
		else if (isdigit(name[i]))
			alpha = false;
		else
			continue;

		if (i == 0)
		{
			lastAlpha = alpha;
			temp = name[i];
			continue;
		}

		if (lastAlpha != alpha)
		{
			if (lastAlpha)
			{
				// add string
				int len = temp.GetLength();
				temp = temp + (CString)"AAAAAAAAAAAAAAA";
				temp.Delete(15, len);
				buf += temp;
			}
			else
			{
				// add number
				temp = (CString)"000000000000000" + temp;
				temp.Delete(0, temp.GetLength() - 15);
				buf += temp;
			}

			temp.Empty();
		}
		temp += name[i];
		lastAlpha = alpha;
	}

	if (lastAlpha)
	{
		// add string
		int len = temp.GetLength();
		temp = temp + (CString)"AAAAAAAAAAAAAAA";
		temp.Delete(15, len);
		buf += temp;
	}
	else
	{
		// add number
		temp = (CString)"000000000000000" + temp;
		temp.Delete(0, temp.GetLength() - 15);
		buf += temp;
	}

	return buf;
}

CString CSCOPin::GetTypeScorpion(CString deviceType) const
{
	CString type;
	CString upperType = m_sType;
	CString upperDevice = deviceType;

	upperDevice.MakeUpper();
	upperType.MakeUpper();
	if (upperType == "POSITIVE")
		type = "P";
	else
	if (upperType == "NEGATIVE")
		type = "M";
	else
	if (upperType == "NO CONNECT")
		type = "NC";
	else
	if (upperType == "ANODE")
		type = "A";
	else
	if (upperType == "CATHODE")
		type = "C";
	else
	if (upperType == "WIPER")
		type = "T";
	else
	if (upperType == "1")
		type = "U";
	else
	if (upperType == "2")
		type = "L";
	else
	if (upperType == "BASE")
		type = "B";
	else
	if (upperType == "COLLECTOR")
		type = "C";
	else
	if (upperType == "EMITTER")
		type = "E";
	else
	if (upperType == "GATE")
		type = "G";
	else
	if (upperType == "DRAIN")
	{
		type = "D";
		if (upperDevice == "TRANSISTOR_TRIAC")
			type = "MT2";
	}
	else
	if (upperType == "SOURCE")
	{
		type = "S";
		if (upperDevice == "TRANSISTOR_TRIAC")
			type = "MT1";
	}

	return type;
}

void CSCOPin::Write(CWriteFormat &file, CString deviceType)
{
	CString type = GetTypeScorpion(deviceType);

	if (!type.IsEmpty())
		file.writef("PIN (P(%d) PN(\"%s\") T(%s))\n", m_iNumber, m_sName, type);
	else
		file.writef("PIN (P(%d) PN(\"%s\"))\n", m_iNumber, m_sName);
}




/******************************************************************************
* CSCOComponent::CSCOComponent
*/
CSCOComponent::CSCOComponent(DataStruct &data, CCEtoODBDoc &doc)
{
	InsertStruct *insert = data.getInsert();
	m_dTolerance = 0.;

	m_pData = &data;
	m_sRefDes = insert->getRefname();
	m_ptLocation = insert->getOrigin2d();
	m_bOnTop = !insert->getPlacedBottom();
	m_mpPins.setSortFunction(cmpCSCOPin);
	
	Attrib *attrib = NULL;
	int keyword = doc.RegisterKeyWord(ATT_PARTNUMBER, 0, valueTypeString);
	if (data.lookUpAttrib(keyword, attrib))
		m_sPartNumber = get_attvalue_string(&doc, attrib);

	keyword = doc.RegisterKeyWord(ATT_DEVICETYPE, 0, valueTypeString);
	if (data.lookUpAttrib(keyword, attrib))
		m_sDeviceType = get_attvalue_string(&doc, attrib);

	keyword = doc.RegisterKeyWord(ATT_SUBCLASS, 0, valueTypeString);
	if (data.lookUpAttrib(keyword, attrib))
		m_sSubClass = get_attvalue_string(&doc, attrib);

	keyword = doc.RegisterKeyWord(ATT_VALUE, 0, valueTypeString);
	if (data.lookUpAttrib(keyword, attrib))
		m_cvValue.SetValue(get_attvalue_string(&doc, attrib));

	keyword = doc.RegisterKeyWord(ATT_TOLERANCE, 0, valueTypeDouble);
	if (data.lookUpAttrib(keyword, attrib))
		m_dTolerance = attrib->getDoubleValue();

	BlockStruct *compBlock = doc.getBlockAt(insert->getBlockNumber());
	if (compBlock != NULL)
	{
		POSITION pos = compBlock->getHeadDataInsertPosition();
		while (pos)
		{
			DataStruct *pinData = compBlock->getNextDataInsert(pos);
			InsertStruct *pinInsert = pinData->getInsert();

			if (pinInsert->getInsertType() != insertTypePin)
				continue;

			m_mpPins.SetAt(pinInsert->getRefname(), new CSCOPin(*pinData, doc));
		}
	}
}

CSCOComponent::CSCOComponent(const CSCOComponent &other)
{
	if (&other != this)
	{
		m_pData = other.m_pData;
		m_sRefDes = other.m_sRefDes;
		m_sPartNumber = other.m_sPartNumber;
		m_ptLocation = other.m_ptLocation;
		m_sDeviceType = other.m_sDeviceType;
		m_cvValue = other.m_cvValue;
		m_dTolerance = other.m_dTolerance;
		m_bOnTop = other.m_bOnTop;
		
		POSITION pos = other.m_mpPins.GetStartPosition();
		while (pos)
		{
			CString key;
			CSCOPin *pin = NULL;
			other.m_mpPins.GetNextAssoc(pos, key, pin);

			CSCOPin *newPin = new CSCOPin(*pin);
			m_mpPins.SetAt(key, newPin);
		}
		m_mpPins.Sort();
	}
}

CSCOComponent::~CSCOComponent()
{
}

void CSCOComponent::writeStock(CWriteFormat &file)
{
	if (!m_sPartNumber.IsEmpty())
		file.writef("STOCK (\"%s\")\n", m_sPartNumber);
}

void CSCOComponent::writeLocation(CWriteFormat &file, double unitConversion)
{
	file.writef("POS (%.2f %.2f) SIDE (%s)\n", m_ptLocation.x * unitConversion, m_ptLocation.y * unitConversion, m_bOnTop?"C":"S");
}

void CSCOComponent::writeValue(CWriteFormat &file)
{
	if (m_cvValue.IsValid())
	{
		double value;
		MetricPrefixTag tag;
		m_cvValue.GetSmallestWholeNumber(value, tag);
		file.writef("VAL (%.0f%s)\n", value, metricPrefixTagToAbbreviatedString(tag));
	}
}

void CSCOComponent::writeTolerance(CWriteFormat &file)
{
	if (m_dTolerance > 0.)
		file.writef("TOL (%.2f)\n", m_dTolerance);
}

void CSCOComponent::writeComment(CWriteFormat &file)
{
	CString deviceType = m_sDeviceType;
	deviceType.MakeUpper();

	if ((deviceType == "CAPACITOR_ARRAY" || deviceType == "CRYSTAL" || deviceType == "DIODE_ARRAY" || deviceType == "FILTER" ||
		deviceType == "NO_TEST" || deviceType == "OSCILLATOR" || deviceType == "POWER_SUPPLY" || deviceType == "RESISTOR_ARRAY" ||
		deviceType == "SPEAKER" || deviceType == "SWITCH" || deviceType == "TEST_POINT" || deviceType == "TRANSFORMER" ||
		deviceType == "TRANSISTOR_ARRAY" || deviceType == "TRANSISTOR_SCR" || deviceType == "VOLTAGE_REGULATOR") &&
		!m_sSubClass.IsEmpty())
		file.writef("COMMENT (\"%s\")\n", m_sSubClass);
}

void CSCOComponent::writeFamily(CWriteFormat &file)
{
	CString deviceType = m_sDeviceType;
	deviceType.MakeUpper();

	if ((deviceType == "IC" || deviceType == "IC_DIGITAL" || deviceType == "IC_LINEAR") && !m_sSubClass.IsEmpty())
		file.writef("FAMILY (\"%s\")\n", m_sSubClass);
}

void CSCOComponent::writePinTotal(CWriteFormat &file)
{
	CString deviceType = m_sDeviceType;
	deviceType.MakeUpper();

	if (deviceType == "IC" || deviceType == "IC_DIGITAL" || deviceType == "IC_LINEAR")
		file.writef("PIN_TOTAL (%d)\n", m_mpPins.GetCount());
}

void CSCOComponent::writeType(CWriteFormat &file)
{
	CString deviceType = m_sDeviceType;
	deviceType.MakeUpper();

	CString type;
	if (deviceType == "TRANSISTOR" || deviceType == "TRANSISTOR_NPN")
		type = "NPN";
	else
	if (deviceType == "TRANSISTOR_PNP")
		type = "PNP";
	else
	if (deviceType == "TRANSISTOR_FET_NPN")
		type = "J_N";
	else
	if (deviceType == "TRANSISTOR_FET_PNP")
		type = "J_P";
	else
	if (deviceType == "TRANSISTOR_MOSFET_NPN")
		type = "MOS_N_D";
	else
	if (deviceType == "TRANSISTOR_MOSFET_PNP")
		type = "MOS_P_D";
	else
		return;

	file.writef("TYPE (%s)\n", type);
}

void CSCOComponent::writePinList(CWriteFormat &file)
{
	file.writef("PINLIST\n");
	file.writef("(\n");
	file.pushHeader("   ");

	m_mpPins.Sort();

	CString *key = NULL;
	CSCOPin *pin = NULL;
	m_mpPins.GetFirstSorted(key, pin);
	while (pin != NULL)
	{
		pin->Write(file, m_sDeviceType);

		m_mpPins.GetNextSorted(key, pin);
	}

	file.popHeader();
	file.writef(")\n");
}

CString CSCOComponent::GetTypeScorpion() const
{
	return GetScorpionDeviceType(m_sDeviceType);
}

CString CSCOComponent::GetValueEng() const
{
	CString value;
	return value;
}

bool CSCOComponent::LookUpPin(CString pinName, CSCOPin *&pin) const
{
	return m_mpPins.Lookup(pinName, pin)?true:false;
}

void CSCOComponent::Write(CWriteFormat &file, bool useLocations, double unitConversion)
{
	CString type = GetTypeScorpion();
	if (type.IsEmpty())
		return;

	file.writef("%s\n", type);
	file.writef("(\n");
	file.pushHeader("   ");

	writeStock(file);
	file.writef("NAME (\"%s\")\n", m_sRefDes);
	if (useLocations)
		writeLocation(file, unitConversion);
	writeValue(file);
	writeTolerance(file);
	writeComment(file);
	writeFamily(file);
	writePinTotal(file);
	writeType(file);
	writePinList(file);

	// end device
	file.popHeader();
	file.writef(")\n");
}



/******************************************************************************
* ScorpionWriter::ScorpionWriter
*/
ScorpionWriter::ScorpionWriter(CCEtoODBDoc *ccDoc)
{
	m_pDoc = ccDoc;
	m_pFile = NULL;
	m_pgOutputPageUnit = pageUnitsMils;
   m_logFp = NULL;
}

ScorpionWriter::~ScorpionWriter()
{
}

double ScorpionWriter::convertToOutputUnits(double value) const
{
	return value * Units_Factor((int)m_pDoc->getSettings().getPageUnits(), (int)m_pgOutputPageUnit);
}

/******************************************************************************
* ScorpionWriter::GetLogFile
*/
bool ScorpionWriter::GetLogFile(CString outputfolderpath, CString logname)
{
   // Retro, for ordinary interactive CAMCAD, log file goes to same folder as output.
   CString logPath = GetLogfilePath(logname);
   if (!outputfolderpath.IsEmpty())
   {
      CFilePath logFilePath(outputfolderpath);
      logFilePath.setBaseFileName("Scorpion");
      logFilePath.setExtension("log");
      logPath = logFilePath.getPath();
   }

   m_localLogFilename = logPath; // This would be the standard interactive CAMCAD log.
   if (getApp().m_readWriteJob.IsActivated())
   {
      // Is readwrite job aka vPlan mode.
      m_logFp = getApp().OpenOperationLogFile("Scorpion.log", m_localLogFilename);
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
      WriteStandardExportLogHeader(m_logFp, "Scorpion");
      return true;
   }

   return false;
}

bool ScorpionWriter::gatherComponentInfo()
{
	// gather all the components
	POSITION pos = m_pFile->getBlock()->getHeadDataInsertPosition();
	while (pos)
	{
		DataStruct *data = m_pFile->getBlock()->getNextDataInsert(pos);
		InsertStruct *insert = data->getInsert();

		if (insert->getInsertType() != insertTypePcbComponent)
			continue;

		m_mpComponents.SetAt(insert->getRefname(), new CSCOComponent(*data, *m_pDoc));
	}

	// get all the pin types and update the CSCOPins
	int keyword = m_pDoc->RegisterKeyWord(ATT_DEVICETOPACKAGEPINMAP, 0, valueTypeString);
	pos = m_pFile->getHeadNetPosition();
	while (pos)
	{
		NetStruct *net = m_pFile->getNextNet(pos);

		POSITION cpPos = net->getHeadCompPinPosition();
		while (cpPos)
		{
			CompPinStruct *cp = net->getNextCompPin(cpPos);

			Attrib *attrib = NULL;

			// get the type attribute
			if (!cp->lookUpAttrib(keyword, attrib))
				continue;
				
			CString type = get_attvalue_string(m_pDoc, attrib);

			// find the component
			CSCOComponent *comp = NULL;
			if (!m_mpComponents.Lookup(cp->getRefDes(), comp))
         {
            //Skip Die Pins
            if(cp->IsDiePin(m_pDoc->getCamCadData()) && m_logFp != NULL)
            {
               fprintf(m_logFp, "Net: %s CompPin %s - Skipped CompPin for Die component.\n", net->getNetName(), 
                  cp->getPinRef());
            }
				continue;
         }
			// find the pin
			CSCOPin *pin = NULL;
			if (!comp->LookUpPin(cp->getPinName(), pin))
 				continue;
 
         pin->SetType(type);
		}
	}

	return true;
}

bool ScorpionWriter::writeComponentTypes(CWriteFormat &file) const
{
	file.writef("COMPONENT_TYPES\n");
	file.writef("(\n");
	file.pushHeader("   ");

	file.writef("TYPELIST\n");
	file.writef("(\n");
	file.pushHeader("   ");

	POSITION pos = m_mpComponents.GetStartPosition();
	while (pos)
	{
		CString refname;
		CSCOComponent *comp = NULL;
		m_mpComponents.GetNextAssoc(pos, refname, comp);

		comp->Write(file);
	}

	// end TYPELIST
	file.popHeader();
	file.writef(")\n");

	// end COMPONENT_TYPES
	file.popHeader();
	file.writef(")\n");

	return true;
}

bool ScorpionWriter::writePbaDescription(CWriteFormat &file) const
{
	file.writef("PBA_DESCRIPTION\n");
	file.writef("(\n");
	file.pushHeader("   ");

	writeEnvironment(file);
	writeCoordinateSystem(file);
	writeBoardStructure(file);
	writeSurface(file);
	writeFiducials(file);
	writeConnections(file);
	writeFixture(file);
	writeComponents(file);

	// end PBA_DESCRIPTION
	file.popHeader();
	file.writef(")\n");

	return true;
}

bool ScorpionWriter::writeEnvironment(CWriteFormat &file) const
{
	file.writef("ENVIRONMENT\n");
	file.writef("(\n");
	file.pushHeader("   ");

	CString unitStr;
	if (m_pgOutputPageUnit == pageUnitsMils)
		unitStr = "IMPERIAL";
	else if (m_pgOutputPageUnit == pageUnitsMilliMeters)
		unitStr = "METRIC";

	file.writef("POSITION (UNITS(%s))\n", unitStr);

	// end ENVIRONMENT
	file.popHeader();
	file.writef(")\n");

	return true;
}

bool ScorpionWriter::writeBoardStructure(CWriteFormat &file) const
{
	file.writef("BOARD_STRUCTURE\n");
	file.writef("(\n");
	file.pushHeader("   ");

	writeBoardPanels(file);

	// end BOARD_STRUCTURE
	file.popHeader();
	file.writef(")\n");

	return true;
}

bool ScorpionWriter::writeBoardPanels(CWriteFormat &file) const
{
	file.writef("PANELLIST\n");
	file.writef("(\n");
	file.pushHeader("   ");

	file.writef("NAME (\"%s\")\n", m_pFile->getBlock()->getName());

	// end PANELLIST
	file.popHeader();
	file.writef(")\n");

	return true;
}

bool ScorpionWriter::writeFiducials(CWriteFormat &file) const
{
	bool foundFiducial = false;

	POSITION pos = m_pFile->getBlock()->getHeadDataInsertPosition();
	while (pos && !foundFiducial)
	{
		DataStruct *fidData = m_pFile->getBlock()->getNextDataInsert(pos);
		InsertStruct *fidInsert = fidData->getInsert();

		foundFiducial = (fidInsert->getInsertType() == insertTypeFiducial);
	}

	if (!foundFiducial)
		return true;

	file.writef("FIDUCIALS\n");
	file.writef("(\n");
	file.pushHeader("   ");

	writeFiducialPanels(file);

	// end FIDUCIALS
	file.popHeader();
	file.writef(")\n");

	return true;
}

bool ScorpionWriter::writeFiducialPanels(CWriteFormat &file) const
{
	file.writef("PANELLIST\n");
	file.writef("(\n");
	file.pushHeader("   ");

	writeFiducialPanel(file);

	// end PANELLIST
	file.popHeader();
	file.writef(")\n");

	return true;
}

bool ScorpionWriter::writeFiducialPanel(CWriteFormat &file) const
{
	file.writef("PANEL\n");
	file.writef("(\n");
	file.pushHeader("   ");

	file.writef("BOARD_PANEL (\"%s\")\n", m_pFile->getBlock()->getName());
	file.writef("SIDE (%s)\n", m_pFile->isMirrored()?"S":"C");
	writeFiducialList(file);

	// end PANEL
	file.popHeader();
	file.writef(")\n");

	return true;
}

bool ScorpionWriter::writeFiducialList(CWriteFormat &file) const
{
	file.writef("FIDUCIALLIST\n");
	file.writef("(\n");
	file.pushHeader("   ");

	POSITION pos = m_pFile->getBlock()->getHeadDataInsertPosition();
	while (pos)
	{
		DataStruct *fidData = m_pFile->getBlock()->getNextDataInsert(pos);
		InsertStruct *fidInsert = fidData->getInsert();

		if (fidInsert->getInsertType() != insertTypeFiducial)
			continue;

		writeFiducial(*fidData, file);
	}

	// end FIDUCIALLIST
	file.popHeader();
	file.writef(")\n");

	return true;
}

bool ScorpionWriter::writeFiducial(DataStruct &data, CWriteFormat &file) const
{
	InsertStruct *insert = data.getInsert();

	file.writef("FID (\"%s\") POS (%.2f %.2f) TYPE (GLOBAL)\n", insert->getRefname(), convertToOutputUnits(insert->getOriginX()), convertToOutputUnits(insert->getOriginY()));

	return true;
}

bool ScorpionWriter::writeConnections(CWriteFormat &file) const
{
	file.writef("CONNECTIONS\n");
	file.writef("(\n");
	file.pushHeader("   ");

	POSITION pos = m_pFile->getHeadNetPosition();
	while (pos)
	{
		NetStruct *net = m_pFile->getNextNet(pos);
      if(!net->IsSingleDiePinNet())
		   writeNode(*net, file);
      else if (m_logFp != NULL)
         fprintf(m_logFp, "CONNECTIONS - Net: %s - Skipped SinglePin Net for Die component.\n",net->getNetName());
	}

	// end CONNECTIONS
	file.popHeader();
	file.writef(")\n");

	return true;
}

bool ScorpionWriter::writeNode(NetStruct &net, CWriteFormat &file) const
{
	int kw = m_pDoc->RegisterKeyWord(ATT_NET_TYPE, 0, valueTypeString);
	Attrib *attrib = NULL;

	file.writef("NODE\n");
	file.writef("(\n");
	file.pushHeader("   ");

	file.writef("NAME (\"%s\")\n", net.getNetName());

	CString type;
	if (net.lookUpAttrib(kw, attrib))
		type = get_attvalue_string(m_pDoc, attrib);

	if (type == "Power")
		file.writef("TYPE (VCC)\n");
	else if (type == "Ground")
		file.writef("TYPE (GND)\n");

	writePinList(net, file);

	// end NODE
	file.popHeader();
	file.writef(")\n");

	return true;
}

bool ScorpionWriter::writePinList(NetStruct &net, CWriteFormat &file) const
{
	file.writef("PINLIST\n");
	file.writef("(\n");
	file.pushHeader("   ");

	POSITION pos = net.getHeadCompPinPosition();
	while (pos)
	{
		CompPinStruct *cp = net.getNextCompPin(pos);

      if(!cp->IsDiePin(m_pDoc->getCamCadData()))
		   file.writef("PIN (D(\"%s\") PN(%s))\n", cp->getRefDes(), cp->getPinName());
      else if (m_logFp != NULL)
         fprintf(m_logFp, "PINLIST - Net: %s CompPin %s- Skipped CompPin for Die component.\n", net.getNetName(), 
            cp->getPinRef());
	}

	// end PINLIST
	file.popHeader();
	file.writef(")\n");

	return true;
}

bool ScorpionWriter::writeFixture(CWriteFormat &file) const
{
	file.writef("FIXTURE\n");
	file.writef("(\n");
	file.pushHeader("   ");

	writeFixturePanels(file);

	// end FIXTURE
	file.popHeader();
	file.writef(")\n");

	return true;
}

bool ScorpionWriter::writeFixturePanels(CWriteFormat &file) const
{
	file.writef("PANELLIST\n");
	file.writef("(\n");
	file.pushHeader("   ");

	writeFixturePanel(file);

	// end PANELLIST
	file.popHeader();
	file.writef(")\n");

	return true;
}

bool ScorpionWriter::writeFixturePanel(CWriteFormat &file) const
{
	file.writef("PANEL\n");
	file.writef("(\n");
	file.pushHeader("   ");

	file.writef("NAME (\"%s\")\n", m_pFile->getName());
	file.writef("BOARD_PANEL (\"%s\")\n", m_pFile->getBlock()->getName());
	file.writef("TYPE (\"FIXED_NAILS\")\n");
	writeNails(file);

	// end PANEL
	file.popHeader();
	file.writef(")\n");

	return true;
}

bool ScorpionWriter::writeNails(CWriteFormat &file) const
{
	file.writef("NAILLIST\n");
	file.writef("(\n");
	file.pushHeader("   ");
   int netnameKW = m_pDoc->getStandardAttributeKeywordIndex(standardAttributeNetName);

	POSITION pos = m_pFile->getBlock()->getHeadDataInsertPosition();
	while (pos)
	{
		DataStruct *probeData = m_pFile->getBlock()->getNextDataInsert(pos);
		InsertStruct *probeInsert = probeData->getInsert();

		if (probeInsert->getInsertType() != insertTypeTestProbe)
			continue;

      if(!IsProbedDiePin(m_pDoc->getCamCadData(), probeData))
		   writeNail(*probeData, file);
      else
      {
         Attrib* attrib = NULL;
         CString NetName;
	      if (probeData->lookUpAttrib(netnameKW, attrib))
            NetName = get_attvalue_string(m_pDoc, attrib);

         if (m_logFp != NULL)
            fprintf(m_logFp, "NAILLIST - Net: %s Probe %s - Skipped Probe for Die pin.\n", NetName, probeInsert->getRefname());
      }
	}

	// end NAILLIST
	file.popHeader();
	file.writef(")\n");

	return true;
}

bool ScorpionWriter::writeNail(DataStruct &probe, CWriteFormat &file) const
{
   // Skip Power Injection resources
   bool skip = false;
   WORD testSourceKW = (WORD)m_pDoc->getStandardAttributeKeywordIndex(standardAttributeTestResource);  
   Attrib* attrib = NULL;
   if (probe.getAttributes() != NULL && probe.getAttributes()->Lookup(testSourceKW, attrib) && attrib != NULL)
   {
      CString value = attrib->getStringValue();
      if (value.CompareNoCase("Power Injection") == 0)
         return false;
   }

   // Proceed with Test Resources
	InsertStruct *insert = probe.getInsert();

	file.writef("NAIL\n");
	file.writef("(\n");
	file.pushHeader("   ");

	file.writef("N (%d)\n", atoi(insert->getRefname()));

	bool placed = false;
	int kw = m_pDoc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, valueTypeString);
	if (probe.lookUpAttrib(kw, attrib))
		placed = (!strcmp("Placed", get_attvalue_string(m_pDoc, attrib)));

	if (placed)
	{
		file.writef("POS (%.2f %.2f)\n", convertToOutputUnits(insert->getOriginX()), convertToOutputUnits(insert->getOriginY()));

		BlockStruct *probeBlock = m_pDoc->getBlockAt(insert->getBlockNumber());
		if (probeBlock != NULL)
		{
			CString probeName = probeBlock->getName();
			file.writef("QLTY (%s)\n", probeName.Mid(0, probeName.Find("_")));
		}
	}

	kw = m_pDoc->RegisterKeyWord(ATT_NETNAME, 0, valueTypeString);
	if (probe.lookUpAttrib(kw, attrib))
		file.writef("NODE (\"%s\")\n", get_attvalue_string(m_pDoc, attrib));

	// end NAIL
	file.popHeader();
	file.writef(")\n");

	return true;
}

bool ScorpionWriter::writeComponents(CWriteFormat &file) const
{
	file.writef("COMPONENTS\n");
	file.writef("(\n");
	file.pushHeader("   ");

	POSITION pos = m_mpComponents.GetStartPosition();
	while (pos)
	{
		CString refname;
		CSCOComponent *comp = NULL;
		m_mpComponents.GetNextAssoc(pos, refname, comp);

		comp->Write(file, true, convertToOutputUnits(1.));
	}

	// end COMPONENTS
	file.popHeader();
	file.writef(")\n");

	return true;
}

bool ScorpionWriter::Write(CString filename)
{
	if (m_pDoc == NULL)
		return false;

   //Create a log file
   if(!GetLogFile(filename, "Scorpion"))
   {
      ErrorMessage("Scorpion", "Can't create log file Scorpion.log !!");
   }

	m_pFile = m_pDoc->getFileList().GetOnlyShown(blockTypePcb);

	if (m_pFile == NULL)
		return false;

	gatherComponentInfo();

	CStdioFileWriteFormat file;
	CExtFileException e;
	if (!file.open(filename, &e))
	{
      // Can't open output file.
      CString tmp;
		tmp.Format("%s [%s]", e.getCause(), filename);
		ErrorMessage(tmp, "Scorpion Write");

		return false;
	}

	if (!writeComponentTypes(file))
	{
		file.close();
		return false;
	}

	if (!writePbaDescription(file))
	{
		file.close();
		return false;
	}

	file.close();

   // Close log file only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!m_localLogFilename.IsEmpty())
   {
      fclose(m_logFp);
   }

	return true;
}
