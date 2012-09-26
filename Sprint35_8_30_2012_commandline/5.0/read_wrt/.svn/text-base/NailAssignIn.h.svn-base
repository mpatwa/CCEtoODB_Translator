// $Header: /CAMCAD/4.5/read_wrt/NailAssignIn.h 5     10/18/05 8:13p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#pragma once

#ifndef __NAILASSIGNIN__
#define __NAILASSIGNIN__

#include "data.h"
#include "dft.h"

enum ENARFileType
{
	narFileTypeNasExe = 0,
	narFileTypeGrNav = 1,
};

/******************************************************************************
* CNARProbeItem 
*/
class CNARProbeItem : public CObject
{
public:
	CNARProbeItem(DataStruct *probeData, CCEtoODBDoc *doc);
	~CNARProbeItem();

private:
	CCEtoODBDoc *m_pDoc;
	CString m_sOrigName;
	CString m_sNewName;
	CString m_sNetName;
	ETestResourceType m_eResourceType;
	bool m_bPlaced;
	DataStruct *m_pProbe;

public:
	const CString GetOriginalName() const					{ return m_sOrigName;		};
	const CString GetNewName() const							{ return m_sNewName;			};
	const CString GetNetName() const							{ return m_sNetName;			};
	DataStruct *GetProbeData()									{ return m_pProbe;			};
	ETestResourceType GetResourceType() const				{ return m_eResourceType;	};
	bool IsPlaced() const										{ return m_bPlaced;			};

	void SetName(CString name);
	void SetTINID(CString conn);
};



/******************************************************************************
* CNARNetMap 
*/
class CNARNetMap : public CTypedMapStringToPtrContainer<CStringList*>
{
public:
	void SetAt(CString key, CString probeName);
};



/******************************************************************************
* CNARProbeMap 
*/
class CNARProbeMap : public CTypedMapStringToPtrContainer<CNARProbeItem*>
{
public:
	CNARProbeMap() : CTypedMapStringToPtrContainer<CNARProbeItem*>() {};
	~CNARProbeMap() {};

private:
	CNARNetMap m_netMap;

public:
	void SetAt(DataStruct *newData, CCEtoODBDoc *doc);
	bool LookupNet(CString netName, CStringList*& probes);
};



/******************************************************************************
* CNARReader 
*/
class CNARReader : public CObject
{
public:
	CNARReader(CCEtoODBDoc *doc, FileStruct *file);
	~CNARReader();
	
private:
	CCEtoODBDoc *m_pDoc;
	FileStruct *m_pFile;
	CNARProbeMap m_probeMap;
	ENARFileType m_eNarType;
	int m_iLineCnt;

	CFormatStdioFile m_readFile;
	CFormatStdioFile m_logFile;

	void loadProbes();
	void identifyFileType();

	bool isValidProbeName(CString name);
	int getTokenCount(CString str);

	CString skipComment(CString line);
	bool getNextValidLine(CString &line, bool start);
	bool getNextProbeLine(CString &line);
	CString getNextProbeLineNas();
	CString getNextProbeLineGrNav();

	void processProbeLine(CString line);
	void processProbeLineNas(CString line);
	void processProbeLineGrNav(CString line);

	int removeLeftoverProbes();

public:
	bool ReadFile(CString filename);
};


#endif // __NAILASSIGNIN__
