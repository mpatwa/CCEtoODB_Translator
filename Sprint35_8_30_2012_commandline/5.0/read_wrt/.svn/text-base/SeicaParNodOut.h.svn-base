// $Header: /CAMCAD/4.5/read_wrt/SeicaParNodOut.h 11    2/24/06 3:15p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/

#if ! defined (__SeicaParNodOut_h__)
#define __SeicaParNodOut_h__

#pragma once

#include "TypedContainer.h"
#include "CamCadDatabase.h"

CString getSeicaUnits(ValueUnitTag valueUnit);
CString getSeicaMacro(DeviceTypeTag deviceType);

//-----------------------------------------------------------------------------
// CSeicaSettings
//-----------------------------------------------------------------------------
class CSeicaSettings
{
public:
   CSeicaSettings();
   ~CSeicaSettings();

   void reset();
   void loadSettingFile();

private:
   CString m_deviceTypeKeyword;

public:
   CString getDeviceTypeKeyword() const { return m_deviceTypeKeyword; }
};

//_____________________________________________________________________________
class CSeicaParWriter
{
private:
   CCamCadDatabase& m_camCadDatabase;
   CSeicaSettings& m_seicaSettings;

public:
   CSeicaParWriter(CCamCadDatabase& camCadDatabase, CSeicaSettings& seicaSettings);

   bool write(const CString& outputFilePath);
};

//_____________________________________________________________________________
class CTestEntityMap
{
private:
   CCamCadDatabase& m_camCadDatabase;
   bool m_initializedFlag;
	int m_hasProbesOnSideFlag;		// bit flag:  0 = none, 1 = top, 2 = bottom
   CTypedMapIntToPtrContainer<DataStruct*> m_entityToLinkedAccessPointMap;
   CTypedMapIntToPtrContainer<DataStruct*> m_entityToLinkedTestProbeMap;

public:
   CTestEntityMap(CCamCadDatabase& camCadDatabase);

	bool hasProbesOnTop();
	bool hasProbesOnBottom();
   DataStruct* getAccessPoint(int entityNumber);
   DataStruct* getTestProbe(int entityNumber);

private:
   void init();

};

//_____________________________________________________________________________
class CSeicaNode : public CObject
{
private:
   CString m_signal;
   CString m_refDes;
   CString m_pin;
   CPoint2d m_origin;     // Part / pin origin
   CPoint2d m_testorigin; // Probe or access pt origin
   CString m_pad;
   CString m_position;
   CString m_technology;
   CString m_testability;
   CString m_channel;

   CString m_sortablePinRef;

public:
   CSeicaNode(const CString& signal,const CString& refDes,const CString& pin,
		CPoint2d origin, CPoint2d testorigin,
      const CString& pad,const CString& position,const CString& technology,
      const CString& testability,const CString& channel);

   CString getSignal()         const { return m_signal; }
   CString getRefDes()         const { return m_refDes; }
   CString getPin()            const { return m_pin; }
   CString getPad()            const { return m_pad; }
   CString getPosition()       const { return m_position; }
   CString getTechnology()     const { return m_technology; }
   CString getTestability()    const { return m_testability; }
   CString getChannel()        const { return m_channel; }
   CPoint2d getOrigin()        const { return m_origin; }
   CPoint2d getTestOrigin()    const { return m_testorigin; }
   CString getSortablePinRef() const { return m_sortablePinRef; }
   CString getPinRef()         const { return m_refDes + "." + m_pin; }
   int getChannelNumber()      const { return atoi(m_channel); }
};

//_____________________________________________________________________________
class CSeicaNodes : public CMapSortedStringToOb<CSeicaNode>
{
private:
   CCamCadDatabase& m_camCadDatabase;
   CTestEntityMap m_testEntityMap;
	CMapStringToString m_unplacedProbesChannelNumber;

public:
   CSeicaNodes(CCamCadDatabase& camCadDatabase);

   void scanData();
	static int sortBySignalAndPinRef(const void* elem1,const void* elem2);
	CString getUnplacedProbe(CString netname);

private:
   CString getSurfacePadDescriptor(DataStruct* surfacePad,BlockStruct& padstackGeometry);
   bool getCopperPad(CCamCadDatabase& camCadDatabase,DataStruct*& copperPad,
      DataStruct& component,BlockStruct& padstackGeometry,bool topFlag, int insertlayer = -1) const;
   bool getCopperPad(CCamCadDatabase& camCadDatabase,DataStruct*& copperPad,
      BlockStruct& padstackGeometry,bool placedTopFlag,bool topFlag, int insertlayer = -1) const;
   void getSeicaPinMapping(CMapStringToString& seicaPinMap,DataStruct& component,DeviceTypeTag deviceType);
	void getSeicaPinMapping(FileStruct* pcbFile,CMapStringToString& seicaPinMap,DataStruct& component,DeviceTypeTag deviceType);
	void getTestValues(CString& testabilityValue,CString& channelValue, CPoint2d& testOrigin,
      int entityNumber,bool primaryAccesSideTopFlag);
};

//_____________________________________________________________________________
class CSeicaNodWriter
{
private:
   CCamCadDatabase& m_camCadDatabase;
   CSeicaSettings& m_seicaSettings;
   CTypedPtrMap<CMapStringToPtr,CString,DataStruct*> m_componentMap;
   CMapStringToString m_pinNumberMap;

public:
   CSeicaNodWriter(CCamCadDatabase& camCadDatabase, CSeicaSettings& seicaSettings);
   ~CSeicaNodWriter();

   bool write(const CString& outputFilePath);
   bool write2(const CString& outputFilePath);

private:
   DataStruct* getComponent(const CString& refDes);
   void initMaps();
   void getSeicaPinMapping(CStringArray& seicaPinMapping,DataStruct& component,DeviceTypeTag deviceType);
   bool getCopperPad(CCamCadDatabase& camCadDatabase,
      DataStruct*& copperPad,DataStruct& component,BlockStruct& padstackGeometry,bool topFlag) const;
};


#endif
