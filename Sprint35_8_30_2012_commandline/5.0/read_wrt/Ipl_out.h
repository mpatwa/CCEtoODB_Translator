// $Header: /CAMCAD/4.6/read_wrt/Ipl_out.h 50    5/15/07 1:15p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#if !defined(__Ipl_out_h__)
#define __Ipl_out_h__

#pragma once

#include "ccdoc.h"
#include "rwlib.h"
#include "find.h"
#include "compvalues.h"
#include "DeviceType.h"
#include "TypedContainer.h"
#include "Element.h"

#define CHNL_STARTNUMBER			0
#define CHNL_UNASSIGNED				9999
#define CHNL_UNASSIGNED_STR		"9999"

#define DEFAULT_MIN_LINE_LENGTH	10
#define DEFAULT_LINE_LENGTH		80
#define DEFAULT_MAX_LINE_LENGTH	10200

// A special internal testing code disguised as DeviceTypeTag, but
// discernable from real DeviceTypeTags
#define CAP_OPENS_TEST (DeviceTypeTag)(-(deviceTypeUndefined + 1))

enum EChannelSpacing
{
	ChannelSpacing16 = 1,
	ChannelSpacing32 = 2,
	ChannelSpacing1 = 3,
};

class CIplNetMap;

typedef long SORTTYPE;

//_____________________________________________________________________________
class CIplSortedArray : private CArray<SORTTYPE>
{
public:
	CIplSortedArray() : CArray<SORTTYPE>() {};
	~CIplSortedArray() {};

// Attributes
	INT_PTR GetSize() const { return CArray<SORTTYPE>::GetSize(); };
	INT_PTR GetCount() const { return CArray<SORTTYPE>::GetCount(); };
	BOOL IsEmpty() const { return CArray<SORTTYPE>::IsEmpty(); };
	INT_PTR GetUpperBound() const { return CArray<SORTTYPE>::GetUpperBound(); };

// Operations
	// Clean up
	void RemoveAll() { CArray<SORTTYPE>::RemoveAll(); };

	// Accessing elements
	SORTTYPE &GetAt(INT_PTR nIndex) { return CArray<SORTTYPE>::GetAt(nIndex); };

	// Potentially growing the array
	INT_PTR Add(SORTTYPE newElement);

	// overloaded operator helpers
	SORTTYPE &operator[](INT_PTR nIndex) { return CArray<SORTTYPE>::GetAt(nIndex); };

	// Operations that move elements around
	void RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1) { CArray<SORTTYPE>::RemoveAt(nIndex, nCount); };

	CString GetDelimitedString(CString delimiter, bool compress, bool avoidDuplicates = false);
	CString GetSmallestValueString();
	long GetSmallestValue();
};

/******************************************************************************
/  Channel Number Classes
/******************************************************************************
/******************************************************************************
* CChannel
*/
class CChannel : public CObject
{
public:
	CChannel(long chnlNum);
	~CChannel() { avoidList.RemoveAll(); };

private:
	long channelNumber;
	bool used;
	bool reserved;
	CList<CChannel*> avoidList;

public:
	long GetChannelNumber() { return channelNumber; };
	void AddToAvoid(CChannel *channel);

	bool IsUsed() { return used; };
	void SetUsed() { used = true; };
	void SetUnused() { used = false; avoidList.RemoveAll(); };

	bool IsReserved() { return reserved; };
	void SetReserved() { reserved = true; };
	void SetUnreserved() { reserved = false; };

	bool CanUse(CChannel *channel, EChannelSpacing channelSpacing);
   bool CanUse(CChannel *channel, EChannelSpacing channelSpacing, CString &errMsg);

	CString Dump(bool header = false);
};

/******************************************************************************
* CChannelNumberArray
*/
class CChannelNumberArray : public CArray<CChannel*>
{
public:
	CChannelNumberArray();
	~CChannelNumberArray();

private:
//	long curChannelNumber;
	CTypedMapStringToPtrContainer<CArray<int>*> preassignedChannels;
	EChannelSpacing channelSpacing;
	bool doRemapping;

public:
	long GetChannelSpacingNum();
	EChannelSpacing GetChannelSpacing();
	void SetChannelSpacing(EChannelSpacing chnlSpacing);

	CChannel *AddNewChannel(long newChannelNumber);

	CChannel *GetFirstAvailableChannel();
	CChannel *GetNextAvailableChannel(CChannel *channel = NULL);
	CChannel *GetHighestUsedChannel();
//	long GetCurNumber() { return curChannelNumber; };

	bool DoRemapping() { return doRemapping; };
	void SetRemappingFlag(bool flag) { doRemapping = flag; };

	void AddPreassignedChannel(CString netName, long channelNumber);
	CString GetPreassignedChannels();
	void SetPreassignedChannels(CCEtoODBDoc *doc, FileStruct *file, CIplNetMap *netMap, FILE *logFile);
   void SetPreassignedChannelsFromOutFileSettings(CIplNetMap *netMap, FILE *logFile);
   void SetPreassignedChannelsFromAttribs(CCEtoODBDoc *doc, FileStruct *file, CIplNetMap *netMap, FILE *logFile);
   bool HasPreassignedChannelAttribs(CCEtoODBDoc *doc, FileStruct *file);

	CString GetReservedChannels();
	void SetReservedChannels(CString reservedChannels);

	void Dump(CFormatStdioFile *file);
};

/******************************************************************************
/  Test Probe Classes
/******************************************************************************
/******************************************************************************
* CIplTestProbe
*/
class CIplTestProbe : public CObject
{
public:
	CIplTestProbe(CCEtoODBDoc *doc, DataStruct *testProbe);
   CIplTestProbe(CIplTestProbe *other);
	~CIplTestProbe() {};

private:
	CCEtoODBDoc* m_camCadDoc;
	DataStruct* m_testProbeData;
	DataStruct* m_testAccessPointData;

	// probedType determine which struct pointer is saved
   CEntity m_probedEntity;

	CString m_name;		// the name of the probe
	CString m_netName;	// the net that's being probed

	CChannel* m_channel;

public:
	CString GetName() { return m_name; };
   void SetRefname(CString refname) { m_name = refname; m_testProbeData->getInsert()->setRefname(refname); }
   void SetRefname(int channelNum)  { CString ref; ref.Format("%d", channelNum); SetRefname(ref); }
	CString GetNetName() { return m_netName; };
	long GetChannelNumber();
   int GetProbeBlockNumber();

   CString GetGridLocation();

	CChannel *GetChannel() { return m_channel; };
	void SetChannel(CChannel *newChannel);

	double GetXLocation() { return (m_testProbeData != NULL) ? m_testProbeData->getInsert()->getOrigin().x:0.0; };
	double GetYLocation() { return (m_testProbeData != NULL) ? m_testProbeData->getInsert()->getOrigin().y:0.0; };
	bool IsOnTop();
	bool IsPlaced();

   EEntityType GetProbedType() { return m_probedEntity.getEntityType(); };

   CString GetProbedItemRefName();
   CString GetProbedItemSortableRefName();
	InsertTypeTag GetProbedItemType();
	double GetProbedItemXLocation();
	double GetProbedItemYLocation();
	double GetProbedItemRotationDegrees();
	double GetProbedItemRotationRadians();
	CString GetProbedItemBlockName();
	bool IsProbedItemSMD();
	bool IsProbedItemOnTop();
   bool IsPowerInjection();
   bool IsProbedDiePin();

	// for EN_COMPPIN
	CString GetProbedItemPinName();
	int GetProbedItemPinNumber();

	

	void BackAssignChannelNumber();
	CString Dump(bool header = false);
};

/******************************************************************************
* CIplTestProbeList
*/
class CIplTestProbeList : public CTypedPtrList<CPtrList, CIplTestProbe*>
{
public:
	CIplTestProbeList() {};
	~CIplTestProbeList() {};

public:
	void BackAssignChannelNumbers();
	void Dump(CFormatStdioFile *file);
};

/******************************************************************************
/  Pin Classes
/******************************************************************************
/******************************************************************************
* CIplPin
*/
class CIplPin : public CObject
{
public:
	CIplPin(CCEtoODBDoc *doc, CString pinNm, DataStruct *data, int pinNum = -1, PinTypeTag type = pinTypeUnknown, CString netNm = "None");
	~CIplPin() {};

private:
	CCEtoODBDoc *pDoc;
	int pinNumber;
	CString pinName;
	CString netName;
	CString sortName;
	PinTypeTag pinType;
	DataStruct *pPinData;

	CString makeSortName(CString name);

public:
	CString GetName() { return pinName; };
	CString GetSortName() { return sortName; };

	int GetPinNumber() { return pinNumber; };
	void SetPinNumber(int pinNum) { pinNumber = pinNum; };
	CString GetNetName() { return netName; };
	void SetNetName(CString netNm) { netName = netNm; };
	PinTypeTag GetPinType() { return pinType; };
	void SetPinType(PinTypeTag type) { if (type >= pinTypeMin && type < pinTypeMax) pinType = type; };

	double GetXLocation() { return (pPinData != NULL)?pPinData->getInsert()->getOrigin().x:0.0; };
	double GetYLocation() { return (pPinData != NULL)?pPinData->getInsert()->getOrigin().y:0.0; };
	double GetRotation() { return (pPinData != NULL)?pPinData->getInsert()->getAngle():0.0; };
	bool IsSMD();

	CString Dump();
};

//******************************************************************************
//  Test Element Class
//******************************************************************************

class CIplTestElement
{
private:
   CString m_parentCompRefname;
   CSubclassElement *m_scElement;

public:
   CIplTestElement(CString parentCompRefname, CSubclassElement *scElement);
   CIplTestElement(CIplTestElement &other)   { this->m_parentCompRefname = other.m_parentCompRefname; this->m_scElement = other.m_scElement; }

   CString GetRefname();
   CString GetPinFunction(CString pinRefname)   { return m_scElement != NULL ? m_scElement->getPinFunction(pinRefname) : ""; }
   CString GetValue()                           { return m_scElement != NULL ? m_scElement->getValue() : ""; }
   CString GetPlusTol()                         { return m_scElement != NULL ? m_scElement->getPlusTol() : ""; }
   CString GetMinusTol()                        { return m_scElement != NULL ? m_scElement->getMinusTol() : ""; }
   DeviceTypeTag GetDeviceTypeTag(DeviceTypeTag defaultDevType);

   bool HasSubclassElement()                    { return m_scElement != NULL; }
   bool ContainsPin(CString pinRefname)         { return m_scElement != NULL ? m_scElement->containsPin(pinRefname) : false; }
};

/******************************************************************************
/  Component Classes
/******************************************************************************
////// 
PWR5    ID,DESC,PINS
////// 
PWR5_5  ID,DESC,VAL1,TOL1,CHNL#,GND_CHNL#
PWRA    ID,DESC,VAL1,TOL1,CHNL#,GND_CHNL#
PWRA_F  ID,DESC,VAL1,TOL1,CHNL#,GND_CHNL#
PWRB    ID,DESC,VAL1,TOL1,CHNL#,GND_CHNL#
PWRB_F  ID,DESC,VAL1,TOL1,CHNL#,GND_CHNL#
////// CIplComponent /////////////////////////////////////////////////
DCHG    ID,DESC
J       ID,DESC,PIN
D       ID,DESC,ANODE,CATHODE
QN      ID,DESC,BASE,EMIT,COLL		GNPN    ID,DESC,VAL1,VAL2,BASE,EMIT,COLL	// include Gain tests base on user options per Transistor
QP      ID,DESC,BASE,EMIT,COLL		GPNP    ID,DESC,VAL1,VAL2,BASE,EMIT,COLL	// include Gain tests base on user options per Transistor
////// CIplComponentName /////////////////////////////////////////////
IC      ID,DESC,NAME,PINLIST
VEC     ID,DESC,NAME,PINLIST
U       ID,DESC,NAME,PINLIST
ATMPL   ID,DESC,NAME,PINLIST
VCLUST  ID,DESC,NAME
DSCAN   ID,DESC,NAME,PINLIST
DFP     ID,DESC,NAME
////// CIplComponentNameInducer //////////////////////////////////////
WSCAN   ID,DESC,NAME,INDUCERNUM,PINLIST
FSCAN   ID,DESC,NAME,INDUCERNUM,PINLIST
FSPLUS  ID,DESC,NAME,INDUCERNUM,PINLIST
CSCAN   ID,DESC,NAME,INDUCERNUM,PINLIST

////// CIplComponentValTol ///////////////////////////////////////////
L       ID,DESC,VAL1,TOL1,PIN1,PIN2
R       ID,DESC,VAL1,TOL1,PIN1,PIN2
POT     ID,DESC,VAL1,TOL1,PIN1,PIN2,PIN3
REO     ID,DESC,VAL1,TOL1,PIN1,PIN2
RP_SI   ID,DESC,VAL1,TOL1,PINLIST
RP_SB   ID,DESC,VAL1,TOL1,PINLIST
RP_DI   ID,DESC,VAL1,TOL1,PINLIST
RP_DB   ID,DESC,VAL1,TOL1,PINLIST
Z       ID,DESC,VAL1,TOL1,ANODE,CATHODE
////// CIplComponentValTolTol ////////////////////////////////////////
C       ID,DESC,VAL1,TOL1,TOL2,PIN1,PIN2
////// CIplComponentValValTol ////////////////////////////////////////
RP_ST   ID,DESC,VAL1,VAL2,TOL1,PINLIST
RP_DT   ID,DESC,VAL1,VAL2,TOL1,PINLIST
*/


/******************************************************************************
* CIplComponent
*
* DCHG    ID,DESC
* J       ID,DESC,PIN
* D       ID,DESC,ANODE,CATHODE
* QN      ID,DESC,BASE,EMIT,COLL		GNPN    ID,DESC,VAL1,VAL2,BASE,EMIT,COLL	// include Gain tests base on user options per Transistor
* QP      ID,DESC,BASE,EMIT,COLL		GPNP    ID,DESC,VAL1,VAL2,BASE,EMIT,COLL	// include Gain tests base on user options per Transistor
*/
class CIplComponent : public CObject
{
public:
	CIplComponent(CCEtoODBDoc *doc, CIplTestElement *iplTestElement, CString compName, DataStruct *data, DeviceTypeTag devType, CString descAttrName, FILE *logFile = NULL);
	~CIplComponent();

private:
	CMapSortStringToOb pins;	// CIplPin
	CString m_descriptionAttribName;
   CIplTestElement *m_iplTestElement;
   BlockStruct *m_insertedCompGeom;

protected:
	CCEtoODBDoc *pDoc;
	DataStruct *pComponentData;
	DeviceTypeTag m_deviceType;		// device type through device typer
	CString componentID;			// ID
	CString description;			// DESC
	int m_sensorNumber;
	double m_compHeight;

	CString getPinList(CIplNetMap *netMap, FILE *logFile = NULL);

	void populateAvoidLists(CIplNetMap *netMap);
	bool setPinChannelNumber(CChannel *channel, CIplTestProbe *iplTestProbe, CIplNetMap *netMap, CChannelNumberArray *channelStats);

public:
   ////*rcf void SetTestElement(CIplTestElement *iplTestElement)  { m_iplTestElement = iplTestElement; }
   CString GetTestElementPinFunction(CString pinRefname) { return m_iplTestElement != NULL ? m_iplTestElement->GetPinFunction(pinRefname) : ""; }
   bool HasSubclassElement()                             { return m_iplTestElement != NULL ? m_iplTestElement->HasSubclassElement() : false; }
   bool SubclassElementContainsPin(CString pinRefname)   { return m_iplTestElement != NULL ? m_iplTestElement->ContainsPin(pinRefname) : false; }

   static int PinSortFunction(const void* elem1,const void* elem2);

	void AddPin(CString pinNm, int pinNum, PinTypeTag pinType, CString netNm);

	CString getDescriptionAttribName()	{ return m_descriptionAttribName; }

   BlockStruct *GetInsertedCompGeomBlock()   { return m_insertedCompGeom; }

	DeviceTypeTag GetType() { return m_deviceType; };
	CString GetName() { return componentID; };
   CString GetOriginalCczRefname()     { return (pComponentData != NULL && pComponentData->isInsert() && pComponentData->getInsert() != NULL) ? pComponentData->getInsert()->getRefname() : ""; }
   CString GetSortableRefname()  { return (pComponentData != NULL && pComponentData->isInsert() && pComponentData->getInsert() != NULL) ? pComponentData->getInsert()->getSortableRefDes() : ""; };
	CString GetDescription() { return description; }; // accessor for this class data
	CString LookupDescription(); // get description from CC data
	CString GetPartNumber();

	CString GetCapacitiveOpens();
	CString GetDiodeOpens();
	CString GetSubClass();
	CString GetSubClassAttribute();
	CString GetPlusTolerance(CString attribName);
   CString GetPlusTolerance() { return GetPlusTolerance(ATT_PLUSTOLERANCE); }
	CString GetValue();

   CString GetAttribute(CString attribName);

	int GetPinCount();
	CIplPin *GetFirstPin();
	CIplPin *GetNextPin();

	void ProcessProbeChannelNumbers(CChannelNumberArray *channelStats, CIplNetMap *netMap, FILE *logFile = NULL);

	CString GetBlockName();
	CString GetComponentDevice();
   CString GetGridLocation();
	double GetXLocation() { return (pComponentData != NULL)?pComponentData->getInsert()->getOrigin().x:0.0; };
	double GetYLocation() { return (pComponentData != NULL)?pComponentData->getInsert()->getOrigin().y:0.0; };
	double GetRotation() { return (pComponentData != NULL)?pComponentData->getInsert()->getAngle():0.0; };
	CPoint2d GetCentroidLocation();
	CExtent GetExtent();
	double GetCompHeight() { return m_compHeight; };
	bool IsOnTop();
	bool IsSMD();
	bool IsCapOpensQualified();

	void SetSensorNumber(int n) { m_sensorNumber = n; }
	int GetSensorNumber() { return m_sensorNumber; }

	virtual CString GetIplString(CIplNetMap *netMap, FILE *logFile = NULL);
	CString GetIplFSPlusString(CIplNetMap *netMap, FILE *logFile = NULL);
	CString GetIplCapScanString(CIplNetMap *netMap, FILE *logFile = NULL);
	bool GetGainTestString(CString &gainTestLine, int gainLevel, int collectorCurrent, CIplNetMap *netMap, FILE *logFile = NULL);

	void Dump(CFormatStdioFile *file);
};

/******************************************************************************
* CIplComponentName
* IC      ID,DESC,NAME,PINLIST
* VEC     ID,DESC,NAME,PINLIST
* U       ID,DESC,NAME,PINLIST
* ATMPL   ID,DESC,NAME,PINLIST
* VCLUST  ID,DESC,NAME
* DSCAN   ID,DESC,NAME,PINLIST
* DFP     ID,DESC,NAME
*/
class CIplComponentName : public CIplComponent
{
public:
	CIplComponentName(CCEtoODBDoc *doc, CIplTestElement *iplTestElement, CString compName, DataStruct *data, DeviceTypeTag devType, CString descAttrName, FILE *logFile = NULL);
	~CIplComponentName() {};

protected:
	CString componentName;		// NAME

public:
	virtual CString GetIplString(CIplNetMap *netMap, FILE *logFile = NULL);
};

/******************************************************************************
* CIplComponentNameInducer
* WSCAN   ID,DESC,NAME,INDUCERNUM,PINLIST
* FSCAN   ID,DESC,NAME,INDUCERNUM,PINLIST
* FSPLUS  ID,DESC,NAME,INDUCERNUM,PINLIST
* CSCAN   ID,DESC,NAME,INDUCERNUM,PINLIST
*/
class CIplComponentNameInducer : public CIplComponentName
{
public:
	CIplComponentNameInducer(CCEtoODBDoc *doc, CIplTestElement *iplTestElement, CString compName, DataStruct *data, DeviceTypeTag devType, CString descAttrName, FILE *logFile = NULL);
	~CIplComponentNameInducer() {};

protected:
	CString inducerName;		// INDUCERNUM

public:
	virtual CString GetIplString(CIplNetMap *netMap, FILE *logFile = NULL);
};

/******************************************************************************
* CIplComponentValTol
* L       ID,DESC,VAL1,TOL1,PIN1,PIN2
* R       ID,DESC,VAL1,TOL1,PIN1,PIN2
* POT     ID,DESC,VAL1,TOL1,PIN1,PIN2,PIN3
* REO     ID,DESC,VAL1,TOL1,PIN1,PIN2
* RP_SI   ID,DESC,VAL1,TOL1,PINLIST
* RP_SB   ID,DESC,VAL1,TOL1,PINLIST
* RP_DI   ID,DESC,VAL1,TOL1,PINLIST
* RP_DB   ID,DESC,VAL1,TOL1,PINLIST
* Z       ID,DESC,VAL1,TOL1,ANODE,CATHODE
*/
class CIplComponentValTol : public CIplComponent
{
public:
	CIplComponentValTol(CCEtoODBDoc *doc, CIplTestElement *iplTestElement, CString compName, DataStruct *data, DeviceTypeTag devType, CString descAttrName, FILE *logFile = NULL);
	~CIplComponentValTol() {};

protected:
	ComponentValues m_value;	// VAL1
	CString m_tol;				// TOL1

public:
	virtual CString GetIplString(CIplNetMap *netMap, FILE *logFile = NULL);
};

/******************************************************************************
* CIplComponentValTolTol
* C       ID,DESC,VAL1,TOL1,TOL2,PIN1,PIN2
*/
class CIplComponentValTolTol : public CIplComponentValTol
{
public:
	CIplComponentValTolTol(CCEtoODBDoc *doc, CIplTestElement *iplTestElement, CString compName, DataStruct *data, DeviceTypeTag devType, CString descAttrName, FILE *logFile = NULL);
	~CIplComponentValTolTol() {};

protected:
	CString m_tol2;		// TOL2

public:
	virtual CString GetIplString(CIplNetMap *netMap, FILE *logFile = NULL);
	bool GetDischargeString(CString &dischargeLine, ComponentValues dischargeMinValue, CIplNetMap *netMap, FILE *logFile = NULL);
};

/******************************************************************************
* CIplComponentValValTol
* RP_ST   ID,DESC,VAL1,VAL2,TOL1,PINLIST
* RP_DT   ID,DESC,VAL1,VAL2,TOL1,PINLIST
*/
class CIplComponentValValTol : public CIplComponentValTol
{
public:
	CIplComponentValValTol(CCEtoODBDoc *doc, CIplTestElement *iplTestElement, CString compName, DataStruct *data, DeviceTypeTag devType, CString descAttrName, FILE *logFile = NULL);
	~CIplComponentValValTol() {};

protected:
	ComponentValues val2;	// VAL2

public:
	virtual CString GetIplString(CIplNetMap *netMap, FILE *logFile = NULL);
};

/******************************************************************************
* CIplComponentMap
*/
class CIplComponentMap : public CMapSortedStringToOb<CIplComponent>
{
public:
   CIplComponentMap(CGTabTableList *tableList) : CMapSortedStringToOb<CIplComponent>() { pinNumberKeyword = 0; m_subclassList.Load(tableList); };
	~CIplComponentMap() {};

private:
	int pinNumberKeyword;

   // The map owns the subclass list because the CIplComponents will end up with pointers to elements
   // in this list. Since this Map class is a container, it owns the components.
   // The components then exist only as long as the map exists, and this subclass list has to match that.
   // When the map is deleted, so are the iplcomps, and so then too this subclass list.
   CSubclassList m_subclassList;

	DataStruct *findComponentData(CCEtoODBDoc *doc, CString compRefname, FileStruct *file, DeviceTypeTag &type, CString &subclass);

   CIplComponent *AddTestElement(CCEtoODBDoc *doc, CIplTestElement &iplTestElement, DataStruct *data, 
      DeviceTypeTag compDevTypeTag, CString descAttrName, FILE *logFile);

public:
	CIplComponent *Lookup(LPCTSTR key);
   CIplComponent *LookupElement(CString originalCompRefname, CString originalPinRefname, int nth); // returns nth (count starts at 1) IplComp that contains element for named pin, NULL if none
	void AddNetComponents(CCEtoODBDoc *doc, FileStruct *file, NetStruct *net, CString descAttrName, FILE *logFile = NULL);

	WORD SetPinNumberKeyword(WORD pinKW);
	void Dump(CFormatStdioFile *file);

   static int AscendingIplCompRefnameSortFunc(const void *a, const void *b);
};

/******************************************************************************
/  Net Classes
/******************************************************************************
/******************************************************************************
* CIplNet
*/
class CIplNet : public CObject
{
public:
	CIplNet(CCEtoODBDoc *doc, NetStruct *net, int netNum = 0, CString netStatus = "", CIplTestProbe *iplTP = NULL);
	~CIplNet() {};

private:
	CCEtoODBDoc *pDoc;
	NetStruct *pNet;

	int netNumber;
	CString netName;
	bool powerNet;
	bool groundNet;

	CTypedPtrList<CPtrList, CIplTestProbe*> iplTestProbes;	// these are deleted in the main list

public:
	CString GetName() { return netName; };
	int GetNumber() { return netNumber; };

	POSITION AddTestProbe(CIplTestProbe *iplTP);
	bool SetProbeChannelNumber(CChannel *channel, CIplTestProbe *iplTestProbe, EChannelSpacing channelSpacing);
   bool SetProbeChannelNumber(CChannel *channel, CIplTestProbe *iplTestProbe, EChannelSpacing channelSpacing, CString &errMsg);

	int GetTestProbeCount();
	CIplTestProbe *GetFirstTestProbe();
	CIplTestProbe *GetNextTestProbe(CIplTestProbe *&iplTP);

	bool IsPowerNet() { return powerNet; };
	bool IsGroundNet() { return groundNet; };

	NetStruct *GetNet() { return pNet; };

	void ProcessProbeChannelNumbers(CChannelNumberArray *channelStats);

	void Dump(CFormatStdioFile *file);
};

/******************************************************************************
* CIplNetMap
*/
class CIplNetMap : public CMapSortedStringToOb<CIplNet>
{
public:
	CIplNetMap() : CMapSortedStringToOb<CIplNet>() {};
	~CIplNetMap() {};

	CIplNet *Lookup(LPCTSTR key);
	void Dump(CFormatStdioFile *file);

   static int AscendingNetNameSortFunc(const void *a, const void *b);
};

/******************************************************************************
/  IPL Class
/******************************************************************************
/******************************************************************************
* CIplWriter
*/
class CIplWriter : public CObject
{
public:
	CIplWriter(CCEtoODBDoc &doc, bool useIplOutSettings, const char *filename = NULL, bool skipPowerInjectionProbesFlag = false);
	~CIplWriter();

protected:
	CIplWriter(CCEtoODBDoc &doc);

private:
	// IPL settings/rules only for IPL Writer Z1800
	int maxLineLength;
	ComponentValues dischargeMin;
	bool createGainTests;
	int gainLevel;
	int gainCollectorCurrent;
	CString m_descriptionAttribName;
	int m_decimalPlaces;
	double outputScale;

   int m_unplacedProbeStartChannel; // starting point for numbering unplaced probes

	int readSettingsFile(CString filename);

	CString splitLine(CString line);
	void DumpSettings(CFormatStdioFile *file);

	void writeGND(CFormatStdioFile *file);
	void writeSH(CFormatStdioFile *file);
	void writeAPC(CFormatStdioFile *file);
	void writeCONT(CFormatStdioFile *file);
	void writePWR5(CFormatStdioFile *file);
	void writeComponents(CFormatStdioFile *file);
	void writeOneComponent(CFormatStdioFile *file, CIplComponent *iplComp);
	void writeOneCapOpensTest(CFormatStdioFile *file, CIplComponent *iplComp);

protected:
	// IPL data structures
	CCEtoODBDoc *pDoc;
	FileStruct *pFile;
	CIplTestProbeList testProbes;
	CIplNetMap m_netMap;
	CIplComponentMap m_compMap;

	// Log file
	FILE *m_iplLogFp;
   CString m_localLogFilename;

	// IPL settings/rules
	CChannelNumberArray channelNumberStats;

	bool gatherData(bool skipPowerInjectionProbesFlag = false);
	void gatherAndAssignUnplacedProbes();
	BOOL IsProbeNameNumeric(CString name);

	int assignChannelNumbers();
	void assignCapOpensSensorNumbers();

	int writeIPL(CString filepath);
	int writeBoardOutline(CString filepath);
	int writeBoard(CString filepath);
	int writeIndXRef(CString filepath);
	int writeNails(CString filepath);
	int writeNameXRef(CString filepath);
	int writeNet2Nail(CString filepath);
	int writePins(CString filepath);
	int writeTstparts(CString filepath);
	int writeZ18_dis(CString filepath);

	CString getDescriptionAttribName()	{ return m_descriptionAttribName; }

	// IPL Writer is subclassed in other exporters which have
	// various needs for name cleaning. They override this otherwise
	// useless looking function.
	virtual CString cleanNetname(CString camcadName)	{ return camcadName; };
	virtual CString cleanSubclass(CString camcadName)	{ return camcadName; };
	virtual CString cleanRefdes(CString camcadName)		{ return camcadName; };

   static int AscendingChannelNumberSortFunc(const void *a, const void *b);

public:
	void setOutputScale(double scale);
	int WriteFiles(CString filepath);
	int writeNets(CString filepath);  // Used by TRI MDA ICT writer
	int writeParts(CString filepath); // Used by TRI MDA ICT writer
   int getDecimalPlaces()     { return m_decimalPlaces; }
};

/******************************************************************************
* CIplTestOrder
*/
class CIplTestOrder : public CList<DeviceTypeTag, DeviceTypeTag>
{
public:
	CIplTestOrder();
};

/******************************************************************************
* CIplSmartRefdes
*/
class CIplSmartRefdes
{
private:
   CString m_prefix;
   int m_number;

public:
	CIplSmartRefdes(const CString& wholeRefdes);
	CString getPrefix() { return m_prefix; }
	int getNumber() { return m_number; }
	bool operator<(CIplSmartRefdes &smrefdes);
};

/******************************************************************************
* CMapBlockToProbeVal
*/
class CMapBlockToProbeVal
{
private:
   CMapStringToInt            blockNameToProbe;
   CMap<int, int, int, int>   blocknNumToProbe;
   int                        probeIndex;

public:
   CMapBlockToProbeVal();
   ~CMapBlockToProbeVal();
   int LookupProbeNumber(BlockStruct *block);
   int GetProbeTableSize(){return blockNameToProbe.GetCount();}  
   void GetProbeTable(CStringArray &table);
};
#endif
