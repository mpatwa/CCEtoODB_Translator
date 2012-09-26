// $Header: /CAMCAD/5.0/read_wrt/ScorpionOut.h 8     2/26/07 7:19p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#pragma once

#ifndef __SCORPIONOUT__
#define __SCORPIONOUT__

#include "General.h"
#include "point2d.h"
#include "compvalues.h"
#include "file.h"
#include "data.h"
#include "typedcontainer.h" 
#include "writeformat.h"
#include "RwLib.h"
#include "DcaFormatStdioFile.h"

class CSCOPin : public CObject
{
public:
	CSCOPin(DataStruct &data, CCEtoODBDoc &doc);
	CSCOPin(const CSCOPin &other);
	~CSCOPin();

private:
	DataStruct *m_pData;
	int m_iNumber;
	CString m_sName;
	CString m_sType;
	CPoint2d m_ptLocation;
	CString m_sSortKey;

	CString generateSortKey(CString name);

public:
	int GetNumber() const				{ return m_iNumber;		};
	CString GetName() const				{ return m_sName;			};
	CString GetType() const				{ return m_sType;			};
	CString GetTypeScorpion(CString deviceType) const;
	CPoint2d GetLocation() const		{ return m_ptLocation;	};
	CString GetSortKey() const			{ return m_sSortKey;		};

	void SetType(CString type)			{ m_sType = type;			};

	void Write(CWriteFormat &file, CString deviceType);
};

class CSCOPinMap : public CTypedMapSortStringToObContainer<CSCOPin>
{
};

class CSCOComponent : public CObject
{
public:
	CSCOComponent(DataStruct &data, CCEtoODBDoc &doc);
	CSCOComponent(const CSCOComponent &other);
	~CSCOComponent();

private:
	DataStruct *m_pData;
	CString m_sRefDes;
	CString m_sPartNumber;
	CPoint2d m_ptLocation;
	CString m_sDeviceType;
	CString m_sSubClass;
	ComponentValues m_cvValue;
	double m_dTolerance;
	bool m_bOnTop;
	CSCOPinMap m_mpPins;

	void writeStock(CWriteFormat &file);
	void writeLocation(CWriteFormat &file, double unitConversion);
	void writeValue(CWriteFormat &file);
	void writeTolerance(CWriteFormat &file);
	void writeComment(CWriteFormat &file);
	void writeFamily(CWriteFormat &file);
	void writePinTotal(CWriteFormat &file);
	void writeType(CWriteFormat &file);
	void writePinList(CWriteFormat &file);

public:
	DataStruct *GetData() const		{ return m_pData;	};
	CString GetRefDes() const			{ return m_sRefDes;		};
	CString GetPartNumber() const		{ return m_sPartNumber;	};
	CPoint2d GetLocation() const		{ return	m_ptLocation;	};
	CString GetType() const				{ return	m_sDeviceType;	};
	CString GetTypeScorpion() const;
	ComponentValues GetValue() const	{ return m_cvValue;		};
	CString GetValueEng() const;
	double GetTolerance() const		{ return m_dTolerance;	};
	bool IsOnTop() const					{ return	m_bOnTop;		};

	bool LookUpPin(CString pinName, CSCOPin *&pin) const;

	void Write(CWriteFormat &file, bool useLocations = false, double unitConversion = 1.);
};
class CSCOComponentMap : public CTypedMapStringToPtrContainer<CSCOComponent*>
{
};

class ScorpionWriter
{
public:
	ScorpionWriter(CCEtoODBDoc *ccDoc);
	~ScorpionWriter();

private:
	CCEtoODBDoc *m_pDoc;
	FileStruct *m_pFile;
	PageUnitsTag m_pgOutputPageUnit;
	CSCOComponentMap m_mpComponents;

   FILE *m_logFp;
   CString m_localLogFilename;

	double convertToOutputUnits(double value) const;
	bool gatherComponentInfo();
	bool writeComponentTypes(CWriteFormat &file) const;
	
	bool writePbaDescription(CWriteFormat &file) const;
	bool writeEnvironment(CWriteFormat &file) const;
	bool writeCoordinateSystem(CWriteFormat &file) const		{ return true;	};
	
	bool writeBoardStructure(CWriteFormat &file) const;
	bool writeBoardPanels(CWriteFormat &file) const;

	bool writeSurface(CWriteFormat &file) const					{ return true;	};

	bool writeFiducials(CWriteFormat &file) const;
	bool writeFiducialPanels(CWriteFormat &file) const;
	bool writeFiducialPanel(CWriteFormat &file) const;
	bool writeFiducialList(CWriteFormat &file) const;
	bool writeFiducial(DataStruct &data, CWriteFormat &file) const;

	bool writeConnections(CWriteFormat &file) const;
	bool writeNode(NetStruct &net, CWriteFormat &file) const;
	bool writePinList(NetStruct &net, CWriteFormat &file) const;

	bool writeFixture(CWriteFormat &file) const;
	bool writeFixturePanels(CWriteFormat &file) const;
	bool writeFixturePanel(CWriteFormat &file) const;
	bool writeNails(CWriteFormat &file) const;
	bool writeNail(DataStruct &probe, CWriteFormat &file) const;

	bool writeComponents(CWriteFormat &file) const;

   bool GetLogFile(CString outputfolderpath, CString logname);

public:
	bool Write(CString filename);
};

#endif // __SCORPIONOUT__
