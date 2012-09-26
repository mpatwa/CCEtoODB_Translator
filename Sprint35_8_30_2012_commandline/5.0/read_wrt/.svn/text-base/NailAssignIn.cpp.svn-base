// $Header: /CAMCAD/4.6/read_wrt/NailAssignIn.cpp 11    12/02/06 9:18p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "stdafx.h"
#include "nailassignin.h"
#include "net_util.h" 
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// E.g. 4.6 becomes 406, 4.10 becomes 410
#define CamCadMajorMinorVersion ((CamCadMajorVersion *100) + CamCadMinorVersion)


void ReadNailAssignement(const char *fileName, CCEtoODBDoc *Doc, FormatStruct *format)
{
	if (Doc == NULL)
		return;

	FileStruct *file = Doc->getFileList().GetOnlyShown(blockTypePcb);

	if (file == NULL)
	{
		ErrorMessage("Error: No file is currently visible");
		return;
	}

	bool haveProbe = false;
	POSITION pos = file->getBlock()->getHeadDataInsertPosition();
	while (pos && !haveProbe)
	{
		DataStruct *data = file->getBlock()->getNextDataInsert(pos);
		InsertStruct *insert = data->getInsert();

		haveProbe = insert->getInsertType() == insertTypeTestProbe;
	}

	if (!haveProbe)
	{
		ErrorMessage("Error: Currently visible file has not probes.");
		return;
	}

	CNARReader reader(Doc, file);
	if (!reader.ReadFile(fileName))
		ErrorMessage("Error: Error reading file");
}



/******************************************************************************
* CNARProbeItem
*/
CNARProbeItem::CNARProbeItem(DataStruct *probeData, CCEtoODBDoc *doc)
{
	if (probeData == NULL)
		return;

	if (doc == NULL)
		return;

	InsertStruct *insert = probeData->getInsert();
	if (insert->getInsertType() != insertTypeTestProbe)
		return;

	WORD cktNetNameKW = doc->RegisterKeyWord(ATT_CKT_NETNAME, 0, valueTypeString);
	WORD netNameKW = doc->RegisterKeyWord(ATT_NETNAME, 0, valueTypeString);
	WORD testResKW = doc->RegisterKeyWord(ATT_TESTRESOURCE, 0, valueTypeString);
	WORD placedKW = doc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, valueTypeString);

	// create the doc
	m_pDoc = doc;

	// set the data pointer
	m_pProbe = probeData;

	// get the probe name
	m_sOrigName = insert->getRefname();
	m_sNewName.Empty();

	// get the net of the probe
	Attrib *attrib = NULL;

	if (probeData->lookUpAttrib(netNameKW, attrib))
		m_sNetName = get_attvalue_string(m_pDoc, attrib);

	NetStruct *net = FindNet(doc->getFileList().GetOnlyShown(blockTypePcb), m_sNetName);

	if (net != NULL && net->lookUpAttrib(cktNetNameKW, attrib))
		m_sNetName = get_attvalue_string(m_pDoc, attrib);

	// get the resource type of the probe
	CString resType;

	if (probeData->lookUpAttrib(testResKW, attrib))
		resType = get_attvalue_string(m_pDoc, attrib);

	m_eResourceType = (resType == "Test")?testResourceTypeTest:testResourceTypePowerInjection;

	// determine if the probe is placed
	CString placedStr;

	if (probeData->lookUpAttrib(placedKW, attrib))
		placedStr = get_attvalue_string(m_pDoc, attrib);

   m_bPlaced = (placedStr.CompareNoCase("Placed") == 0);
}

CNARProbeItem::~CNARProbeItem()
{
}

void CNARProbeItem::SetName(CString name)
{
	m_sNewName = name;
	m_pProbe->getInsert()->setRefname(STRDUP(m_sNewName));

	WORD refdesKey = m_pDoc->RegisterKeyWord(ATT_REFNAME, 0, valueTypeString);
	Attrib* attrib = NULL;
	if (m_pProbe->getAttributes() && m_pProbe->getAttributes()->Lookup(refdesKey, attrib))
	{
		attrib->setStringValueIndex(m_pDoc->RegisterValue(name));
	}
}

void CNARProbeItem::SetTINID(CString conn)
{
	WORD tinKW = m_pDoc->RegisterKeyWord(ATT_TESTERINTERFACE, 0, valueTypeString);

   m_pProbe->setAttrib(m_pDoc->getCamCadData(), tinKW, valueTypeString, (VOID*)conn.GetBuffer(0), attributeUpdateOverwrite, NULL);
}



/******************************************************************************
* CNARNetMap
*/
void CNARNetMap::SetAt(CString key, CString probeName)
{
	CStringList* probeList;
	if (CTypedMapStringToPtrContainer<CStringList*>::Lookup(key, probeList))
	{
		// add the probe name to the list if it doesn't already exist
		bool found = false;
		POSITION pos = probeList->GetHeadPosition();
		while (pos && !found)
		{
			CString str = probeList->GetNext(pos);

			found = str == probeName;
		}

		if (!found)
			probeList->AddTail(probeName);
	}
	else
	{
		probeList = new CStringList();
		probeList->AddTail(probeName);
		CTypedMapStringToPtrContainer<CStringList*>::SetAt(key, probeList);
	}
}



/******************************************************************************
* CNARProbeMap
*/
void CNARProbeMap::SetAt(DataStruct *newData, CCEtoODBDoc *doc)
{
	CNARProbeItem *probeItem = NULL;

	// if it's already included, move on
	if (CTypedMapStringToPtrContainer<CNARProbeItem*>::Lookup(newData->getInsert()->getRefname(), probeItem))
		return;

	probeItem = new CNARProbeItem(newData, doc);
	CTypedMapStringToPtrContainer<CNARProbeItem*>::SetAt(probeItem->GetOriginalName(), probeItem);
	m_netMap.SetAt(probeItem->GetNetName(), probeItem->GetOriginalName());
}

bool CNARProbeMap::LookupNet(CString netName, CStringList*& probes)
{
	if (!m_netMap.Lookup(netName, probes))
      return false;

	return true;
}



/******************************************************************************
* CNARReader
*/
CNARReader::CNARReader(CCEtoODBDoc *doc, FileStruct *file)
{
	m_pDoc = doc;
	m_pFile = file;
}

CNARReader::~CNARReader()
{
}

void CNARReader::loadProbes()
{
	POSITION pos = m_pFile->getBlock()->getHeadDataInsertPosition();
	while (pos)
	{
		DataStruct *data = m_pFile->getBlock()->getNextDataInsert(pos);
		InsertStruct *insert = data->getInsert();

		if (insert->getInsertType() != insertTypeTestProbe)
			continue;

		m_probeMap.SetAt(data, m_pDoc);
	}
}

void CNARReader::identifyFileType()
{
	CString buf, line;

	for (int i=0; i<6; i++)
	{
		m_readFile.ReadString(line);
		buf += line + " ";
	}

	if (buf.Find("nas.exe") > 0)
	{
		m_eNarType = narFileTypeNasExe;
		m_logFile.WriteString("Reading nas.exe file...\n");
	}
	else if (buf.Find("GR Navigate GenFixReports") > 0)
	{
		m_eNarType = narFileTypeGrNav;
		m_logFile.WriteString("Reading GR Navigate GenFixReports file...\n");
	}

	m_readFile.SeekToBegin();
}

bool CNARReader::isValidProbeName(CString name)
{
	name.Trim();

	if (name[0] != 'F')
		return false;

	bool areDigitsOnly = true;
	for (int i=1; i<name.GetLength() && areDigitsOnly; i++)
		areDigitsOnly = isdigit(name[i])?true:false;

	return areDigitsOnly;
}

int CNARReader::getTokenCount(CString str)
{
	int tokPos = 0;
	str.Tokenize(" \t\n", tokPos);

	int tokCount = 0;
	while (tokPos > 0)
	{
		tokCount++;
		str.Tokenize(" \t\n", tokPos);
	}

	return tokCount;
}

CString CNARReader::skipComment(CString line)
{
	int endComment = line.Find("*/");
	while (endComment < 0)
	{
		if (!m_readFile.ReadString(line))
			return "";
		endComment = line.Find("*/");
	}

	return line.Mid(endComment+2);
}

bool CNARReader::getNextValidLine(CString &line, bool start)
{
	while (m_readFile.ReadString(line))
	{
		// skip this line if the first 30 (start=false) or else 15 characters are white spaces
		CString buf = line.Mid(0, start?15:30);
		if (buf.Trim().IsEmpty())
			continue;

		line.Trim();

		// skip the comment if there is one found
		if (line.Find("/*") == 0)
			line = skipComment(line);

		if (line.IsEmpty())
			continue;

		int tokPos = 0;
		CString token;

		if (line[0] == '-')
			continue;

		token = line.Tokenize(" \t\n", tokPos);
		if (token.IsEmpty() || token == "ASSIGNED" || token == "NAIL" || token == "Page:")
			continue;

		return true;
	}

	return false;
}

bool CNARReader::getNextProbeLine(CString &line)
{
	line.Empty();

	if (m_eNarType == narFileTypeNasExe)
		line = getNextProbeLineNas();
	else if (m_eNarType == narFileTypeGrNav)
		line = getNextProbeLineGrNav();

	return !line.IsEmpty();
}

CString CNARReader::getNextProbeLineNas()
{
	CString retLine, line;

	retLine.Empty();
	while (retLine.IsEmpty() && getNextValidLine(line, true))
	{
		int tokPos = 0;
		CString token = line.Tokenize(" \t\n", tokPos);
		if (token == "#")
			token = line.Tokenize(" \t\n", tokPos);
		if (token[0] == '#')
			token = token.Mid(1);

		if (!isValidProbeName(token))
		{
			m_logFile.WriteString("'%s' on line %d is an invalid probe name\n", token, m_readFile.GetLinePosition());
			continue;
		}

		retLine += line + " ";

		if (getTokenCount(line) <= 4)
		{
			if (!getNextValidLine(line, false))
				return "";
			line.Trim();
			retLine += line;
		}
	}

	return retLine;
}

CString CNARReader::getNextProbeLineGrNav()
{
	CString retLine, line;

	retLine.Empty();
	while (retLine.IsEmpty() && getNextValidLine(line, true))
	{
		int tokPos = 0;
		CString token = line.Tokenize(" \t\n", tokPos);

		if (!isValidProbeName(token))
		{
			m_logFile.WriteString("'%s' on line %d is an invalid probe name\n", token, m_readFile.GetLinePosition());
			continue;
		}

		retLine += line + " ";

		if (getTokenCount(line) < 4)  // case dts0100458404 changed <= to strictly <
		{
			if (!getNextValidLine(line, false))
				return "";
			line.Trim();
			retLine += line;
		}
	}

	return retLine;
}

void CNARReader::processProbeLine(CString line)
{
	if (m_eNarType == narFileTypeNasExe)
		processProbeLineNas(line);
	else if (m_eNarType == narFileTypeGrNav)
		processProbeLineGrNav(line);
}

void CNARReader::processProbeLineNas(CString line)
{
	CString newProbeName, netName, curProbeName, connString;
	int tokPos = 0;

	// parse out each field
	newProbeName = line.Tokenize(" \t\n", tokPos);
	// skip # as a token (special processing
	if (newProbeName == "#")
		newProbeName = line.Tokenize(" \t\n", tokPos);
	if (newProbeName[0] == '#')
		newProbeName = newProbeName.Mid(1);
	netName = line.Tokenize(" \t\n", tokPos);
	curProbeName = line.Tokenize(" \t\n", tokPos);
	connString = line.Tokenize(" \t\n", tokPos);

	// make sure the probe names from the file is valid
	if (!isValidProbeName(newProbeName))
	{
		m_logFile.WriteString("Invalid probe name [%s] on line %d\n", newProbeName, m_readFile.GetLinePosition());
		return;
	}
	if (!isValidProbeName(curProbeName))
	{
		m_logFile.WriteString("Invalid probe name [%s] on line %d\n", curProbeName, m_readFile.GetLinePosition());
		return;
	}

	CStringList *retList = NULL;
	if (!m_probeMap.LookupNet(netName, retList))
	{
		m_logFile.WriteString("Net name [%s] not found with an existing probe \n", netName);
		return;
	}

	// parse out the probe number (remove the 'F')
	CString newProbeNumber = newProbeName.Mid(1);

	// find a probe on this net (looked up in m_probeMap) that hasn't been renamed yet
	POSITION pos = retList->GetHeadPosition();
	while (pos)
	{
		CString retItem = retList->GetNext(pos);

		CNARProbeItem *pItem = NULL;
		if (!m_probeMap.Lookup(retItem, pItem))
			continue;

		if (!pItem->GetNewName().IsEmpty())
			continue;

		pItem->SetName(newProbeNumber);
		pItem->SetTINID(connString);
		break;
	}

	// parse out the probe number (remove the 'F')
	//CNARProbeItem *pItem = NULL;
	//CString newProbeNumber = newProbeName.Mid(1), curProbeNumber = curProbeName.Mid(1);
	//if (!m_probeMap.Lookup(curProbeNumber, pItem))
	//{
	//	m_logFile.WriteString("Probe %s not found in file\n", curProbeNumber);
	//	return;
	//}

	//// make sure the net is the same
	//if (netName != pItem->GetNetName())
	//{
	//	m_logFile.WriteString("Unmatched net for probe %s\n", curProbeNumber);
	//	return;
	//}

	//pItem->SetName(newProbeNumber);
	//pItem->SetTINID(connString);
}

void CNARReader::processProbeLineGrNav(CString line)
{
	CString newProbeName, netName, connString;
	int tokPos = 0;

	// parse out each field
	newProbeName = line.Tokenize(" \t\n", tokPos);
	netName = line.Tokenize(" \t\n", tokPos);
	connString = line.Tokenize(" \t\n", tokPos);

	// make sure the probe names from the file is valid
	if (!isValidProbeName(newProbeName))
	{
		m_logFile.WriteString("Invalid probe name [%s] on line %d\n", newProbeName, m_readFile.GetLinePosition());
		return;
	}

	CStringList *retList = NULL;
	if (!m_probeMap.LookupNet(netName, retList))
	{
		m_logFile.WriteString("Net name [%s] not found with an existing probe \n", netName);
		return;
	}

	// parse out the probe number (remove the 'F')
	CString newProbeNumber = newProbeName.Mid(1);

	// find a probe on this net (looked up in m_probeMap) that hasn't been renamed yet
	POSITION pos = retList->GetHeadPosition();
	while (pos)
	{
		CString retItem = retList->GetNext(pos);

		CNARProbeItem *pItem = NULL;
		if (!m_probeMap.Lookup(retItem, pItem))
			continue;

		if (!pItem->GetNewName().IsEmpty())
			continue;

		pItem->SetName(newProbeNumber);
		pItem->SetTINID(connString);
		break;
	}
}

int CNARReader::removeLeftoverProbes()
{
	int delCount = 0;
	POSITION pos = m_probeMap.GetStartPosition();
	while (pos)
	{
		CString key;
		CNARProbeItem *pItem = NULL;
		m_probeMap.GetNextAssoc(pos, key, pItem);

		if (pItem->GetNewName().IsEmpty() && pItem->GetResourceType() == testResourceTypeTest)
		{
			m_logFile.WriteString("Removed probe %s [%s] on Net %s\n", pItem->GetOriginalName(), pItem->IsPlaced()?"Placed":"Unplaced", pItem->GetNetName());
			delCount++;

			// remove probe
			m_probeMap.RemoveKey(key);
			//m_pFile->getBlock()->RemoveDataFromList(m_pDoc, pItem->GetProbeData());
         m_pDoc->removeDataFromDataList(*(m_pFile->getBlock()),pItem->GetProbeData());

			delete pItem;
		}
	}

	return delCount;
}

bool CNARReader::ReadFile(CString filename)
{
	if (m_pDoc == NULL || m_pFile == NULL)
		return false;

	//char drive[_MAX_DRIVE], dir[_MAX_DIR], file[_MAX_FNAME], ext[_MAX_EXT];
	//_splitpath(filename, drive, dir, file, ext);

	CString logFile = GetLogfilePath("nar.log");
	if (m_logFile.Open(logFile, CFile::modeWrite|CFile::modeCreate))
	{
		m_logFile.WriteString("Filename: %s\n", logFile);
		CTime t = t.GetCurrentTime();
		m_logFile.WriteString("%s\n\n", t.Format("Created date :%A, %B %d, %Y at %H:%M:%S"));
	}

	if (!m_readFile.Open(filename, CFile::modeRead))
	{
		m_logFile.WriteString("Error opening %s\n", filename);
		m_logFile.Close();
		return false;
	}

	loadProbes();
	m_logFile.WriteString("Found %d probes in the file\n\n.", m_probeMap.GetCount());

	identifyFileType();

	int probeCount = 0;
	CString probeLine;
	while (getNextProbeLine(probeLine))
	{
		processProbeLine(probeLine);
		probeCount++;
	}
	m_logFile.WriteString("Total probes read: %d\n\n", probeCount);

	int delCount = removeLeftoverProbes();
	m_logFile.WriteString("Total probes removed: %d\n", delCount);

	m_readFile.Close();

	CTime t = t.GetCurrentTime();
   m_logFile.WriteString("\n%s\n", t.Format("Done :%A, %B %d, %Y at %H:%M:%S"));
   m_logFile.WriteString("End Logfile\n");
   m_logFile.Close();

	//if (errorCount)
	//	Logreader(logFile);

	return true;
}
