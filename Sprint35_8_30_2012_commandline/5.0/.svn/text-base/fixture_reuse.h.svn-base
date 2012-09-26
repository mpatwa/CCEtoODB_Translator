// $Header: /CAMCAD/4.6/fixture_reuse.h 12    5/15/07 1:16p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2005. All Rights Reserved.
*/

#pragma once

#ifndef __FIXTURE_REUSE__
#define __FIXTURE_REUSE__

#include "ccdoc.h"
#include "dft.h"
#include "units.h"
#include "Ck.h"
#include "afxwin.h"

/******************************************************************************
*/
class CCamcadNetnameList
{
private:
	CStringArray m_namelist;  // ordered list for searching
	CMapStringToString m_nameMap; // map adjusted name to camcad name

public:
	CCamcadNetnameList(FileStruct *file, CStdioFileWriteFormat *logFile = NULL);
	~CCamcadNetnameList();

	CString findBestMatch(CString foreignNetname);
};


/******************************************************************************
* CFixtureReuseDlg 
*/
class CFixtureReuseDlg : public CDialog
{
	DECLARE_DYNAMIC(CFixtureReuseDlg)

public:
	CFixtureReuseDlg(CCEtoODBDoc *doc, CWnd* pParent = NULL);   // standard constructor
	virtual ~CFixtureReuseDlg();

// Dialog Data
	enum { IDD = IDD_FIXTURE_REUSE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CCEtoODBDoc *m_doc;
	CString m_sTopFixturePath;
	CString m_sBotFixturePath;

public:
	CString GetTopFixturePath() const			{ return m_sTopFixturePath;	};
	CString GetBotFixturePath() const			{ return m_sBotFixturePath;	};
	afx_msg void OnBnClickedBrowseTop();
	afx_msg void OnBnClickedBrowseBottom();
protected:
	virtual void OnOK();

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedFixtureTypeCamcad();
	afx_msg void OnBnClickedFixtureTypeAgilent();
   afx_msg void OnBnClickedFixtureTypeFabmaster();
	afx_msg void OnBnClickedFixtureTypeTeradyneD2B();
	CStatic m_topFileLabel;
	CStatic m_botFileLabel;
	int m_fileTypeSel;
};



/******************************************************************************
* CFixtureProbeDrill 
*/
class CFixtureProbeDrill : public CObject
{
public:
	CFixtureProbeDrill()							{ m_sProbeRefname = "-1";		};
	~CFixtureProbeDrill() {};

protected:
	CPoint2d m_ptLocation;
	ETestSurface m_eSurface;
	CString m_sProbeRefname;
	CString m_sDrillType;
	CString m_sProbeSize;
	CString m_sCompPin;
	CString m_sNetName;
	CPPAccessibleLocation *m_pResultingAccLoc;

public:
	CPoint2d GetLocation() const							{ return m_ptLocation;				};
	ETestSurface GetSurface() const						{ return m_eSurface;					};
	CString GetProbeRefname() const						{ return m_sProbeRefname;	     	};
	CString GetDrillType() const							{ return m_sDrillType;				};
	CString GetProbeSize() const							{ return m_sProbeSize;				};
	CString GetCompPin() const								{ return m_sCompPin;					};
	CString GetNetName() const								{ return m_sNetName;					};
	CPPAccessibleLocation *GetAccLoc() const			{ return m_pResultingAccLoc;		};

	void SetLocation(CPoint2d pt) 						{ m_ptLocation = pt;					};
	void SetSurface(ETestSurface surface)				{ m_eSurface = surface;				};
   void SetProbeRefname(int number)						{ m_sProbeRefname.Format("%d", number); };
   void SetProbeRefname(CString refname)           { m_sProbeRefname = refname; }
	void SetDrillType(CString type) 						{ m_sDrillType = type;				};
	void SetProbeSize(CString size) 						{ m_sProbeSize = size;				};
	void SetCompPin(CString cp) 							{ m_sCompPin = cp;					};
	void SetNetName(CString net) 							{ m_sNetName = net;					};
	void SetAccLoc(CPPAccessibleLocation *accLoc)	{ m_pResultingAccLoc = accLoc;	};
};
class CFixtureProbeDrillList: public CTypedPtrListContainer<CFixtureProbeDrill*>
{
public:
	CFixtureProbeDrillList(bool isContainer=true) : CTypedPtrListContainer<CFixtureProbeDrill*>(isContainer) {};
};

/******************************************************************************
*/
class CFixtureToolHoleList: public CTypedPtrListContainer<CPoint2d*>
{
public:
	CFixtureToolHoleList(bool isContainer=true) : CTypedPtrListContainer<CPoint2d*>(isContainer) {};
	CExtent DetermineExtent();
};

/******************************************************************************
* CFixtureReuseProbe
*/
class CFixtureReuseProbe : public CPPProbe
{
public:
	CFixtureReuseProbe(CCEtoODBDoc *doc, ETestResourceType resType, CString netName);
	CFixtureReuseProbe(CFixtureReuseProbe &probe);
	~CFixtureReuseProbe();

	virtual void DumpToFile(CFormatStdioFile &file, int indent);

private:
	bool m_bPilotDrill;

public:
   CFixtureReuseProbe& operator=(const CFixtureReuseProbe &probe);

	bool IsPilotDrill() const												{ return m_bPilotDrill;							};
	void SetAsPilot(bool pilot)											{ m_bPilotDrill = pilot;						};
};



/******************************************************************************
* CFixtureReuseNet 
*/
class CFixtureReuseNet : public CPPNet
{
public:
	CFixtureReuseNet(CCEtoODBDoc *doc, CString netName = "");
	CFixtureReuseNet(CFixtureReuseNet &ppNet);
	~CFixtureReuseNet();

private:

public:
	bool IsNetProbed();
	int ReportAccessibleLocations(CStdioFileWriteFormat &logFile);
	CFixtureReuseProbe *AddTail_Pilots(CString netName);
};



/******************************************************************************
* CFixtureReuseSolution 
*/
class CFixtureReuseSolution : public CProbePlacementSolution
{
public:
	enum FixtureReuseSolutionReturnCodes
	{
		frsRetRetained = 0,
		frsRetChanged = -1,
		frsRetMoved = -2,
		frsRetUnplaced = -3,
		frsRetRemoved = -4,
		frsRetMissingTemplate = -5,
		frsRetInvalid = -6,
	};

public:
	CFixtureReuseSolution(CCEtoODBDoc *doc, CTestPlan *testPlan, FileStruct *file=NULL);
	~CFixtureReuseSolution();

private:
	CPPAccessibleLocation *findAccessLocation(double x, double y);                        // find on either surface
   CPPAccessibleLocation *findAccessLocation(double x, double y, ETestSurface surface);  // find on given surface only
	void addAllNets();
	CCamcadNetnameList m_ccnetnames;
	

public:
	bool IsNetProbed(NetStruct *net);
	bool GetAccessibleLocations(ETestSurface surface);
	int ReportAccessibleLocations(NetStruct *net, CStdioFileWriteFormat &logFile);
	FixtureReuseSolutionReturnCodes PlaceFixturePilot(double x, double y, CString drillType, CString probeSize, CString compPin, CString pilotNetname);
	FixtureReuseSolutionReturnCodes PlaceFixtureProbe(CFixtureProbeDrill *probeDrill, CPPAccessibleLocation *&accLoc);

	long PlaceProbesOnBoard();
};

/******************************************************************************
* CFixtureReuse 
*/
class CFixtureReuse : public CObject
{
public:
	enum EFixtureReuseReturnCodes
	{
		frRetOK = 0,
		frRetCantOpenFixture = -1,
		frRetNoDefaultFileStruct = -2,
		frRetNoAccess = -3,
		frRetAlignmentFailure = -4,
	};

	enum EFixtureFileSections
	{
		frSectionNone = 0,
		frSectionDillData = 1,
		frSectionRecepticleSize = 2,
		frSectionTestResourceWiring = 3,
		frSectionPowerInjectionWiring = 4,
		frSectionToolingHoles = 5,
		frSectionCapacitiveOpens = 6,
	};

	enum EFixtureFileType
	{
		frFileTypeUnknown,
		frCamcadFile,
		frAgilentFile,
      frFabmasterFile,
		frTeradyneD2BFile
	};


public:
	CFixtureReuse(CCEtoODBDoc &doc);
	~CFixtureReuse();

	bool DumpToFile(CString Filename);

private:
	CCEtoODBDoc &m_doc;
	FileStruct *m_pFile;
	CFixtureReuseSolution *m_pSolution;
	CFixtureProbeDrillList m_fixtureProbes;
	CUnits m_units;
	CString m_prefix;

	CFixtureToolHoleList m_toolHoles;

	CString m_sErrMsg;

   bool formatUsesSeparateTopAndBottomFiles(EFixtureFileType filetype);

	CFixtureProbeDrill *FindProbe(ETestSurface surface, CPoint2d &location, CStdioFileWriteFormat &logFile);

	EFixtureFileType determineFileType(CString fixtureFilePath);
	EFixtureReuseReturnCodes readCamcadFixtureFile(CString fixtureFilePath, CStdioFileWriteFormat &logFile, ETestSurface surface);
	EFixtureReuseReturnCodes readAgilentFixtureFile(CString fixtureFilePath, CStdioFileWriteFormat &logFile, ETestSurface surface);
   EFixtureReuseReturnCodes readFabmasterFixtureFile(CString fixtureFilePath, CStdioFileWriteFormat &logFile, ETestSurface surface);
	EFixtureReuseReturnCodes readTeradyneD2BFixtureFile(CString fixtureFilePath, CStdioFileWriteFormat &logFile, ETestSurface surface);
	
   bool SelectAlignmentCompPins(CompPinStruct **selectedcp1, CPoint2d *selectedprobe1Pt, CompPinStruct **selectedcp2, CPoint2d *selectedprobe2Pt, CMapWordToPtr& rejects);
	bool AttempAlignmentByCompPins(CString& alignStatMsg, CompPinStruct *selectedcp1, CPoint2d selectedprobe1Pt, CompPinStruct *selectedcp2, CPoint2d selectedprobe2Pt, CTMatrix& fixtureTransform, bool& fixtureFileMirrored);
   bool AlignFixtureByCompPins(CStdioFileWriteFormat &logFile, CString *msg);

	bool AlignFixtureByToolHoles(CStdioFileWriteFormat &logFile, CString *msg);
   void ReportToolHoleDifferences(CStdioFileWriteFormat &logFile);

public:
	CString StripPrefix(CString name);
	EFixtureReuseReturnCodes ApplyFixtureFiles(CString topFilePath, CString botFilePath, CString logFilePath);
	
	bool AlignFixture(CStdioFileWriteFormat &logFile, CString &errmsg);

	bool IsFixtureAlreadyAligned();
	bool AreToolHolesAligned(CFixtureToolHoleList &toolHoles);

	CString GetErrorMessage() const			{ return m_sErrMsg; };

   void MakeProbeRefnamesUnique();
};



#endif // __FIXTURE_REUSE__
