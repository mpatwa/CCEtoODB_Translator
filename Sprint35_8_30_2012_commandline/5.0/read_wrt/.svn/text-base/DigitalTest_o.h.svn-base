// $Header: /CAMCAD/5.0/read_wrt/DigitalTest_o.h 15    5/29/07 6:14p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/

#pragma once

#ifndef __DIGITALTEST_O__
#define __DIGITALTEST_O__

#include "typedcontainer.h"
#include "ccdoc.h"
#include "compvalues.h"
#include "dft.h"
#include "afxwin.h"
#include "PcbUtil.h"
#include "UserDefinedMessage.h"

class CPinList;
class CComponent;
class CComponentList;


class CSortKeyItem : public CObject
{
public:
	CSortKeyItem() {};

protected:
	CString m_sSortKey;

public:
	CString GetSortKey() const										{ return m_sSortKey; };
	void SetSortKey(CString sortKey)								{ m_sSortKey = sortKey; };
};

/******************************************************************************
* CDigitalTestSettings 
*/

class CDigitalTestSettings
{
private:

	CString m_digAttrName;
	CMapStringToPtr m_digAttrValues;
	int m_analogLow;
	int m_analogHigh;
	int m_digitalHigh;
   bool m_exportNoTest;
   CUserDefinedMessage m_userDefMsg;


public:
	CDigitalTestSettings();
	~CDigitalTestSettings();

	bool isDigital(CString val);

	int getAnalogLow()	{return m_analogLow;}
	int getAnalogHigh()	{return m_analogHigh;}
	int getDigitalHigh()	{return m_digitalHigh;}
   bool getExportNoTest() {return m_exportNoTest;}

	void setAnalogLow(int val)		{m_analogLow = val;}
	void setAnalogHigh(int val)	{m_analogHigh = val;}
	void setDigitalHigh(int val)	{m_digitalHigh = val;}
   void setExportNoTest(bool val) {m_exportNoTest = val;}

	CString getDigitalAttrName()	{return m_digAttrName;}

   CUserDefinedMessage& getUserDefMsg() {return m_userDefMsg;}

};


/******************************************************************************
* CPin 
*/
class CPin : public CSortKeyItem
{
public:
	CPin(DataStruct *data, CPinList &parentPinList, CComponent *parentComponent = NULL);
	CPin(const CPin &other);
	~CPin() {};

	void DumpToFile(CWriteFormat &file);

private:
	CComponent *m_pComponent;
	CPinList *m_pPinList;

	CString m_sName;
	int m_iNumber;
	CString m_sPinType;
	CString m_sNetName;

protected:
	const CComponent *GetParentComponent() const				{ return m_pComponent; };
	const CPinList *GetParentPinList() const					{ return m_pPinList; };

public:
	CString GetName() const											{ return m_sName; };
	int GetNumber() const											{ return m_iNumber; };
	CString GetNetName() const										{ return m_sNetName; };
	CString GetType() const											{ return m_sPinType; };
};
class CPinList : public CTypedPtrListContainer<CPin*>
{
public:
	CPinList(CCEtoODBDoc &doc, FileStruct &file, CComponent *parentComponent = NULL, CString(*keyGenFunc)(const CString) = NULL);

	void DumpToFile(CWriteFormat &file);

private:
	bool m_bSorted;
	CCEtoODBDoc &m_doc;
	FileStruct &m_file;
	CComponent *m_pComponent;
	CString(*m_pKeyGenFunction)(const CString);

protected:
	const CComponent *GetParentComponent() const									{ return m_pComponent; };

public:
	CCEtoODBDoc &GetDoc() const															{ return m_doc; };
	FileStruct &GetFile() const														{ return m_file; };

	bool IsSorted() const																{ return m_bSorted; };
	void SetKeyGenFunction(CString(*keyGenFunc)(const CString) )			{ m_pKeyGenFunction = keyGenFunc; m_bSorted = false; };
	void Sort(CString(*keyGenFunc)(const CString) = NULL);

	bool AddPin(DataStruct *data);
};



/******************************************************************************
* CDevice 
*/
class CDevice : public CObject
{
public:
	CDevice(DataStruct *data, CCEtoODBDoc &doc, CComponent *parentComponent = NULL);
	CDevice(const CDevice &other);
	~CDevice() {};

	void DumpToFile(CWriteFormat &file);

private:
	CComponent *m_pComponent;
	CString m_sType;
	ComponentValues m_cvValue;
	double m_dPlusTol;
	double m_dMinusTol;

protected:
	const CComponent *GetParentComponent() const				{ return m_pComponent; };

public:
	CString GetType() const											{ return m_sType; };
	CString GetValue(bool allowSpaceInValue = true);
	double GetPlusTolerance() const								{ return m_dPlusTol; };
	double GetMinusTolerance() const								{ return m_dMinusTol; };
};



/******************************************************************************
* CComponent 
*/
class CComponent : public CSortKeyItem
{
public:
	CComponent(DataStruct *data, CCEtoODBDoc &doc, FileStruct &file, CComponentList *parentComponentList = NULL);
	CComponent(const CComponent &other);
	~CComponent() {};

	void DumpToFile(CWriteFormat &file);

private:
	DataStruct *m_pComponentData;
	CComponentList *m_pComponentList;

	CString m_sRefName;
	CString m_sPartNumber;

protected:
	CDevice m_cDevice;
	CPinList m_lPins;

   const CComponentList *GetParentComponentList() const	{ return m_pComponentList; };

public:
	const DataStruct &GetParentData() const					{ return *m_pComponentData; };
	CString GetRefName() const										{ return m_sRefName; };
	CString GetDeviceName() const									{ return m_cDevice.GetType(); };
	CString GetPartNumber() const									{ return m_sPartNumber; };
	CString GetSortKey() const										{ return m_sSortKey; };
	int GetPinCount() const											{ return m_lPins.GetCount(); };

	void SortPins(CString(*keyGenFunc)(const CString) = NULL);
};
class CComponentList : public CTypedPtrListContainer<CComponent*>
{
public:
	CComponentList(CCEtoODBDoc &doc, FileStruct &file, CString(*keyGenFunc)(const CString) = NULL);

	void DumpToFile(CWriteFormat &file);

private:
	bool m_bSorted;
	CCEtoODBDoc &m_doc;
	FileStruct &m_file;
	CString(*m_pKeyGenFunction)(const CString);

protected:
	void SetUnsorted()																{ m_bSorted = false; };

public:
	CCEtoODBDoc &GetDoc() const														{ return m_doc; };
	FileStruct &GetFile() const													{ return m_file; };

	bool IsSorted() const															{ return m_bSorted; };
	void SetKeyGenFunction(CString(*keyGenFunc)(const CString))			{ m_pKeyGenFunction = keyGenFunc; m_bSorted = false; };
	void Sort(CString(*keyGenFunc)(const CString) = NULL);

	virtual bool AddComponent(DataStruct *data);
};



/******************************************************************************
* CDTComponent 
*/
class CDTComponent : public CComponent
{
public:
	CDTComponent(DataStruct *data, CCEtoODBDoc &doc, FileStruct &file, CComponentList *parentComponentList);

private:
	CString m_sSubClass;

public:
	CString GetSubClass() const				{ return m_sSubClass; };

	void SetSubClass(CString str)				{ m_sSubClass = str; };

	CString GetPartNumberString();
	CString GetComponentString();
};
class CDTComponentList : public CComponentList
{
public:
	CDTComponentList(CCEtoODBDoc &doc, FileStruct &file, CString(*keyGenFunc)(const CString));

	virtual bool AddComponent(DataStruct *data);
};



/******************************************************************************
* CMapDeviceToComponentList 
*/
class CMapDeviceToComponent : public CTypedMapStringToPtrContainer<CDTComponentList*>
{
public:
	CMapDeviceToComponent(CCEtoODBDoc &doc, FileStruct &file);

	void DumpToFile(CWriteFormat &file);

private:
	CCEtoODBDoc &m_doc;
	FileStruct &m_file;

public:
	void AddComponent(DataStruct *data);
};



/******************************************************************************
* CDTProbe 
*/
class CDTProbe : public CObject
{
public:
	CDTProbe(DataStruct *probe, CCEtoODBDoc &doc, FileStruct &file);
	~CDTProbe() {};

	void DumpToFile(CWriteFormat &file);

private:
	CCEtoODBDoc &m_doc;
	FileStruct &m_file;
	DataStruct *m_pProbe;
	CString m_sRefName;
	int m_iProbeNumber;
	bool m_bPlaced;
	double getProbeDiameter();

public:
	void setProbeNumber(int num)			{ m_iProbeNumber = num; }
	CString GetRefName() const				{ return m_sRefName; };
	int GetProbeNumber() const				{ return m_iProbeNumber; };
	bool IsPlaced() const					{ return m_bPlaced; };
	DataStruct *getProbeData()				{ return m_pProbe; }
	
	void updateInsertRefname(int probeNum);

};
class CDTProbeList : public CTypedPtrListContainer<CDTProbe*>
{
public:
	void DumpToFile(CWriteFormat &file);
};



/******************************************************************************
* CDTNet 
*/
class CDTNet : public CObject
{
public:
	CDTNet(NetStruct *net, CCEtoODBDoc &doc, FileStruct &file, CDigitalTestSettings &dtSettings);
	~CDTNet() {};

	void DumpToFile(CWriteFormat &file);

private:
	CCEtoODBDoc &m_doc;
	FileStruct &m_file;
	NetStruct *m_pNet;
	CString m_sName;
	eProbeableNetType m_eType;
	CDTProbeList m_lProbes;
	bool m_digitalNet;

public:
	CString GetName() const												{ return m_sName; }
	eProbeableNetType GetType() const								{ return m_eType; }
	NetStruct* GetNet() const								         { return m_pNet; }

	int GetCount_ProbeNumbers() const								{ return m_lProbes.GetCount(); };
	POSITION GetHeadPosition_ProbeNumbers() const				{ return m_lProbes.GetHeadPosition(); }
	POSITION GetTailPosition_ProbeNumbers() const				{ return m_lProbes.GetTailPosition(); }
   CDTProbe *GetHead_ProbeNumbers() const							{ return m_lProbes.GetHead(); }
	CDTProbe *GetTail_ProbeNumbers() const							{ return m_lProbes.GetTail(); }
	CDTProbe *GetNext_ProbeNumbers(POSITION &pos) const		{ return m_lProbes.GetNext(pos); }
	CDTProbe *GetPrev_ProbeNumbers(POSITION &pos) const		{ return m_lProbes.GetPrev(pos); }

	bool AddProbe(DataStruct *probe);

	DataStruct *findComponentData(CString refdes);
	bool isDigital()	{return m_digitalNet;};
};
class CDTNetList : public CTypedMapStringToPtrContainer<CDTNet*>
{
public:
	void DumpToFile(CWriteFormat &file);
};

/******************************************************************************
* CDigitalTestWriter 
*/
class CDigitalTestWriter : public CObject
{
public:
	CDigitalTestWriter(FileStruct &file, CCEtoODBDoc &doc);
	~CDigitalTestWriter();

private:
	CCEtoODBDoc &m_doc;
	FileStruct &m_file;
	CFormatStdioFile m_logFile;
	CMapDeviceToComponent m_mapDev2Comp;
	CDTNetList m_mapNetlist;

	CDigitalTestSettings m_dtSettings;

	int gatherComponents();
	int gatherProbes();
	int gatherNets();
	bool renumberProbes();
	bool probeIsInRange(int probeNum, bool digital);

	int writeBoard(CWriteFormat &file);
	int writePartNumbers(CWriteFormat &file);
	int writeComponents(CWriteFormat &file);
	int writePower(CWriteFormat &file);
	int writeMessage(CWriteFormat &file);
	int writeNetwork(CWriteFormat &file);

	int writeFile(CString filename);
	int dumpToFile(CString filename = "");

public:
	int WriteFiles(CString filename);
	CDigitalTestSettings *getSettings()	{return &m_dtSettings; }

	static CString SortKeyGenerator(const CString inStr);
	static CString GetDigitalTestType(const CString deviceType);
	static CString GetPartnumberHeader(const CString deviceType);
	static CString GetComponentHeader(const CString deviceType);

};

// CDigitalTestSettingsDlg dialog

class CDigitalTestSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CDigitalTestSettingsDlg)

public:
	//CDigitalTestSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	CDigitalTestSettingsDlg(CDigitalTestWriter &dtWriter, CWnd* pParent = NULL);
	virtual ~CDigitalTestSettingsDlg();

// Dialog Data
	enum { IDD = IDD_DIGITAL_TEST_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_analogLow;
	int m_analogHigh;
	int m_digitalHigh;
	afx_msg void OnBnClickedOk();
};

#endif // __DIGITALTEST_O__
