// $Header: /CAMCAD/4.5/read_wrt/fixture_out.h 24    12/22/05 10:48a Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#if !defined(__FixtureOut_h__)
#define __FixtureOut_h__

#pragma once

#include "find.h"
#include "units.h"
#include "dft.h"
#include "probestyles.h"

/******************************************************************************
* CFixtureProbe
*/
class CFixtureProbe : public CObject
{
public:
	CFixtureProbe(CCEtoODBDoc *doc, FileStruct *file, DataStruct *testProbe, CTMatrix mat, int boardNumber = 0);
	~CFixtureProbe() {};

protected:
	CCEtoODBDoc *m_pDoc;
	DataStruct *m_pTestProbeData;
	DataStruct *m_pTestAccessPointData;

	// probedType determine which struct pointer is saved
	//EEntityType m_eProbedType;
	//VOID *m_pProbedItem;
   CEntity m_probedEntity;

	int m_iBoardNumber;
	bool m_bPlaced;
	CString m_sName;		// the name of the probe, aka Refname aka probe number
	CString m_sNetName;	// the net that's being probed
	CString m_sTemplateName; // name of probe template as seen in Probe Placement "probe" tab, from block inserted by probe
	CPoint2d m_ptLocation;
	ETestResourceType m_eResourceType;
	CString m_sTesterInterface;
	int m_iSize;
	double m_dDrillSize;
	double m_dExposedMetalDiameter;
	CString m_sTechnology;
	CString m_sProbeStyle;
	CString m_sProbeMessage;

public:
	bool IsPlaced() const								{ return m_bPlaced; };
	CString GetName() const								{ return m_sName; };
	CString GetNetName() const							{ return m_sNetName; };
	CString GetTemplateName() const					{ return m_sTemplateName; };
	int GetBoardNumber()	const							{ return m_iBoardNumber; }

	CPoint2d GetLocation() const						{ return m_ptLocation; };
	bool IsOnTop() const;

   EEntityType GetProbedType() const				{ return  m_probedEntity.getEntityType(); };
   DataStruct *GetProbedComponent()					{ return (m_probedEntity.getEntityType() == entityTypeData   ) ? m_probedEntity.getData()    : NULL; }
	CompPinStruct *GetProbedCompPin()				{ return (m_probedEntity.getEntityType() == entityTypeCompPin) ? m_probedEntity.getCompPin() : NULL; }
	
	// for EN_COMPPIN
	CString GetCompName() const;
	CString GetPinName() const;

	// for EN_DATA and EN_COMPPIN
	CString GetRefName() const;

	ETestResourceType GetResourceType() const		{ return m_eResourceType; };
	CString GetResourceTypeString() const;

	CString GetTesterInterface() const				{ return m_sTesterInterface; };

	int GetSize() const									{ return m_iSize; };
	double GetDrillSize() const						{ return m_dDrillSize; };
	double GetExposedMetalDiameter() const			{ return m_dExposedMetalDiameter; };
	CString GetTechnology() const						{ return m_sTechnology; };
	CString GetProbeStyle() const						{ return m_sProbeStyle; };
	CString GetProbeMessage() const				   { return m_sProbeMessage; };

	void SetName(CString name)							{ m_sName = name; };
};

//_______________________________________________________________________________________
class CProbe : public CFixtureProbe
{
public:
	CProbe(CCEtoODBDoc *doc, FileStruct *file, DataStruct *testProbe, CTMatrix mat);

protected:
	CString m_sTargetType;
	double m_dFeatureDrillSize;

public:
	DataStruct *GetProbeData()				{ return m_pTestProbeData; };
	CString GetTargetType() const			{ return m_sTargetType; };
	double GetFeatureDrillSize() const	{ return m_dFeatureDrillSize; };

};

/******************************************************************************
* CFixtureProbeList 
*/
class CFixtureProbeList : public CTypedPtrListContainer<CFixtureProbe*>
{
};



/******************************************************************************
* CFixtureAccLoc 
*/
class CFixtureAccLoc : public CPPAccessibleLocation
{
public:
	CFixtureAccLoc(CCEtoODBDoc *doc, DataStruct *testAccessData) : CPPAccessibleLocation(doc, testAccessData)	{ m_bForFixtureFile = true;		};
	~CFixtureAccLoc() {};

	void DumpToFile(CFormatStdioFile &file, int indent);

private:
	bool m_bForFixtureFile;

public:
	bool IsForFixtureFile() const				{ return m_bForFixtureFile;		};

	void SetForFixtureFlag(bool flag)		{ m_bForFixtureFile = flag;		};
};



/******************************************************************************
* CFixtureNet 
*/
class CFixtureNet : public CPPNet
{
public:
	CFixtureNet(CCEtoODBDoc *doc, CString netName) : CPPNet(doc, netName) {};
	~CFixtureNet() {};

public:
	CPPAccessibleLocation *AddTail_AccessibleLocations(DataStruct *testAccessData);
};



/******************************************************************************
* CPilotDrillSolution 
*/
class CPilotDrillSolution : protected CProbePlacementSolution
{
public:
	CPilotDrillSolution(CCEtoODBDoc *doc, CTestPlan *testPlan, FileStruct *file=NULL) : CProbePlacementSolution(doc, testPlan, file) {};
	~CPilotDrillSolution() {};

	void DumpToFile(CFormatStdioFile &file, int indent);

private:
	void addAllNets();

	void gatherAndMatchProbes();
	void createAndPlaceProbes();
	int maximizeFixtureProbeSizes();
	int removeFixtureTemplatesFromProbes();
	int fixViolatingFixtureProbeTemplates();

public:
	int RunAnalysis();
	void WriteDrills(CFormatStdioFile &file, PageUnitsTag outputPageUnit, int outputPrecision, bool top, CTMatrix mat=CTMatrix());
};
class CPilotDrillSolutionMap : public CTypedMapStringToPtrContainer<CPilotDrillSolution*>
{
public:
	CPilotDrillSolutionMap() {};
	CPilotDrillSolutionMap(CCEtoODBDoc *doc, FileStruct *file);
};



/******************************************************************************
* CFixtureFileWriter 
*/
class CFixtureFileWriter : public CObject
{
public:
	CFixtureFileWriter(FileStruct *file, CCEtoODBDoc *doc);
	~CFixtureFileWriter();

private:
	CCEtoODBDoc *m_pDoc;
	FileStruct *m_pFile;
	PageUnitsTag m_eOutputPageUnit;
	int m_iOutputPrecision;
	FILE* m_logFile;
	CFixtureProbeList m_probeList;
	CPilotDrillSolutionMap m_PilotDrillSolutions;
	int probeNumberOffset;

	int loadSettings(CString filename);
	int gatherProbes();
	double getDrillHoleSize(DataStruct *data);
	int findHighestProbeNumber();
	void applyProbeNumberOffset(int probeNumberOffset);

	int writeDrillData(CFormatStdioFile &file, bool top);
	int writeRecepticleSize(CFormatStdioFile &file, bool top);
	int writeTestResource(CFormatStdioFile &file, bool top);
	int writePowerInjection(CFormatStdioFile &file, bool top);
	int writeToolingHoles(CFormatStdioFile &file, bool top);
	int writeCapcitiveOpenComps(CFormatStdioFile &file, bool top);

	int writeFile(CString filename, bool top);

	int dumpToFile(CString filename);

public:
	int WriteFiles(CString filepath);
};

#endif#pragma once


// CFixtureOutProbeOffsetDlg dialog

class CFixtureOutProbeOffsetDlg : public CDialog
{
	DECLARE_DYNAMIC(CFixtureOutProbeOffsetDlg)

public:
	CFixtureOutProbeOffsetDlg(int defaultOffset, CWnd* pParent = NULL);   // standard constructor
	virtual ~CFixtureOutProbeOffsetDlg();
	int getProbeNumberOffset()	{ return m_probeNumberOffset; }

// Dialog Data
	enum { IDD = IDD_FIXTURE_OUT_PROBE_OFFSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	int m_probeNumberOffset;  // the user changes this
	int m_minimumOffset;      // the user does not change this
};
