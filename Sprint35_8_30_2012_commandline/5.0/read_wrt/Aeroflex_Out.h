
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#pragma once

#include "ccdoc.h"
#include "data.h"
#include "DFT.h"

enum EItemType
{
	typeUnknown = -1,
	typeVia = 0,
	typeTestpad = 1,
	typeComppin = 2,
};

enum EDFTSource
{
	sourceUnknown = -1,
	sourceAccess = 0,
	sourceTestAttr = 1,
};

enum EAeroflexUnits
{
	unitUnknown = -1,
	unitMils = 0,		// thousandths of an inch
	unitMicrons = 1,	// microns
};

enum EAllocate
{
	allocUnknown = -1,
	allocMasked = 0,
	allocGetat = 1,
	allocPrime = 2,
};

#define ALLOCATE_MASKED				"MASKED"
#define ALLOCATE_GETAT				"GETAT"
#define ALLOCATE_PRIME				"PRIME"

#define DEFAULT_SOURCE				sourceAccess
#define DEFAULT_AEROFLEX_UNITS	unitMils;

class CCluster;

/******************************************************************************
* CClusterItem
*/
class CClusterItem
{
public:
	CClusterItem(int entityNum, CString refName, CString pinName, double x, double y, double rotation,
					 int layer1, int layer2, EItemType type, EAllocate allocTop, EAllocate allocBottom, 
					 bool isLoaded, CCluster *cluster);
	~CClusterItem();

private:
	CString m_sRefname;
	CString m_sPinname;
	EAllocate m_eAllocTop;
	EAllocate m_eAllocBottom;
	int m_iEntityNum;
	double m_iX;
	double m_iY;
	double m_iRotation;
	int m_iLayer1;
	int m_iLayer2;
	EItemType m_eType;
	bool m_bIsLoaded;
	CCluster *m_pCluster;

	CString getAllocString(ETestSurface preferSurface);
	CString getLayerString();
	int writeVia(CFormatStdioFile &file, int indent, double unitFactor, ETestSurface preferSurface);
	int writeTestPad(CFormatStdioFile &file, int indent, double unitFactor, ETestSurface preferSurface);
	int writeComppin(CFormatStdioFile &file, int indent, double unitFactor, ETestSurface preferSurface);

public:
	void SetProperty(double x, double y, double rotation, int layer1, int layer2);
	void SetAllocTop(EAllocate allocate);
	void SetAllocBottom(EAllocate allocate);
	CCluster *GetCluster()	{ return m_pCluster; }

	int WriteItem(CFormatStdioFile &file, int indent, double unitFactor, ETestSurface preferSurface, bool exportUnloaded);
};

class CClusterItemMap : private CTypedPtrMap<CMapStringToPtr, CString, CClusterItem*>
{
public:
	CClusterItemMap();
	~CClusterItemMap();

	bool Is_Empty()	{ return (this->IsEmpty() ? true : false); }

	void DeleteAllItems();
	CClusterItem* AddItem(int entityNum, CString refName, CString pinName, double x, double y, double rotation, int layer1, int layer2,
			EItemType type, EAllocate allocTop, EAllocate allocBottom, bool isLoaded, CCluster *cluster);
	CClusterItem* FindItem(int entityNum);
	int WriteItems(CFormatStdioFile &file, int indent, double unitFactor, ETestSurface preferSurface, bool exportUnloaded);
};


/******************************************************************************
* CCluster
*/
class CCluster
{
public:
	CCluster(CCEtoODBDoc &doc, CString netName, int startLayerIndex, int endLayerIndex);
	~CCluster();
	
private:
	CCEtoODBDoc &m_pDoc;
	int m_iStartLayerIndex;
	int m_iEndLayerIndex;
	CString m_sNetName;
	CClusterItemMap m_viaMap;
	CClusterItemMap m_testpadMap;
	CClusterItemMap m_comppinMap;
	CPtrList m_trackList;

	int writeTracks(CFormatStdioFile &file, int indent, double unitFactor);

public:
	int GetBottomAeroflexLayer()	{ return 0; } // bottom is always zero
	int GetTopAeroflexLayer()		{ return (m_iEndLayerIndex - m_iStartLayerIndex); } // top is always nth layer
	int MapToAeroflexLayer(int camcadLayerIndex)		{ return (m_iEndLayerIndex - camcadLayerIndex); }
	
	bool IsEmpty()	{ return( m_comppinMap.Is_Empty() && m_testpadMap.Is_Empty() && m_viaMap.Is_Empty() ); }
	
	void AddViaItem(int entityNum, CString viaName, double x, double y, double rotation, int layer1, int layer2,
				EAllocate allocTop, EAllocate allocBottom, bool isLoaded);
	void AddTestpadItem(int entityNum, CString testPadName, double x, double y, double rotation, int layer,
				EAllocate allocTop, EAllocate allocBottom, bool isLoaded);
	void AddComppinItem(int entityNum, CString compName, CString pinName, double x, double y, double rotation, int layer,
				EAllocate allocTop, EAllocate allocBottom, bool isLoaded);
	void AddTrack(DataStruct *polyData);

	void SetItemAllocate(int entityNum, EAllocate allocate, EItemType type, bool isBottom);
	int WriteCluster(CFormatStdioFile &file, int indent, double unitFactor, bool writeTrack, ETestSurface preferSurface, bool exportUnloaded);
};

class CClusterMap : private CTypedPtrMap<CMapStringToPtr, CString, CCluster*>
{
public:
	CClusterMap(CCEtoODBDoc &doc);
	~CClusterMap();
	
private:
	CCEtoODBDoc &m_pDoc;

public:
	CCluster *AddCluster(CString netName, int startLayerIndex, int endLayerIndex);
	CCluster *FindCluster(CString netName);

	void DeleteAllClusters();
	int WriteClusters(CFormatStdioFile &file, int indent, double unitFactor, bool writeTrack, ETestSurface preferSurface, bool exportUnloaded);
};


/******************************************************************************
* CAeroflexWrite
*/
class CAeroflexWrite
{
public:
	CAeroflexWrite(CCEtoODBDoc &doc, double scale, FILE *logFp);
	~CAeroflexWrite();

private:
	CCEtoODBDoc &m_pDoc;
	double m_dUnitFactor;
	double m_dScale;
   FILE *m_logFp;
	int m_iStartLayerIndex;
	int m_iEndLayerIndex;
	bool m_bDisplayError;
	CMapStringToString m_textTypeMap;
	CMapStringToString m_shapeDefinitionMap;
	CClusterMap m_clusterMap;
	ETestSurface m_eTestSide;
	CMapStringToString m_toolHoleGeomDefinitionMap;

	// Variables from setting file
	bool m_bExportUnloaded;
	bool m_bWriteGraphics;
	bool m_bWriteTraces;
	EDFTSource m_eDFTSource;
	EAeroflexUnits m_eUnits;

	DbUnit getToolHoleDiameter(BlockStruct *toolHoleInstanceBlock);

	bool isThereTestProbe(FileStruct *pPcbFile);
	void getStackLayer();
	CString getTextTypeID(int width, int height);

	CString getShapeDefineID(BlockStruct *block, bool mirrored);
	CString findShapeDefineID(int blockNumber, bool mirrored);

	int gatherCluster(FileStruct *pPcbFile, bool useTestAttrib);
	void getAllocates(InsertTypeTag insertType, bool isBottom, Attrib *testAttrib, Attrib *technologyAttrib, EAllocate &allocTop, EAllocate & allocBottom);
	int checkDFTResult(FileStruct *pPcbFile);

	int loadSettings(CString fileName);
	int writeCB_Description(CFormatStdioFile &file, FileStruct *pPcbFile);
	int writeHeader(CFormatStdioFile &file, int indent, FileStruct *pPcbFile);
	int writeCB_NetAttributes(CNetList &netList);

	// These funtions write the ENVIRONMENT section
	int writeEnvironment(CFormatStdioFile &file, int indent, FileStruct *pPcbFile);
	int writeBoardShape(CFormatStdioFile &file, int indent, FileStruct *pPcbFile);
	int writeSHText(CFormatStdioFile &file, int indent,const TextStruct* text);
	int writeShapeDefine(CFormatStdioFile &file, int indent);
	int writeTextDefine(CFormatStdioFile &file, int indent);
	int writeToolHoleGeometries(CFormatStdioFile &file, int indent, FileStruct *pPcbFile);
	int writeToolHoleInstances(CFormatStdioFile &file, int indent, FileStruct *pPcbFile);

	// These functions write teh COMPONENTS section
	int writeComponents(CFormatStdioFile &file, int indent, FileStruct *pPcbFile);
	int writeDevice(CFormatStdioFile &file, int indent, FileStruct *pPcbFile);
	int writeType(DataStruct *data, int pinCount, CFormatStdioFile &file, int indent);

	// These functions write teh CONNECTIONS section
	int writeConnections(CFormatStdioFile &file, int indent, FileStruct *pPcbFile);

	int GetBottomAeroflexLayer()	{ return 0; } // bottom is always zero
	int GetTopAeroflexLayer()		{ return (m_iEndLayerIndex - m_iStartLayerIndex); } // top is always nth layer
	int MapToAeroflexLayer(int camcadLayerIndex)		{ return (m_iEndLayerIndex - camcadLayerIndex); }


public:
	int WriteFile(CString fileName);
   bool ShowLogFile()   { return m_bDisplayError; }
};
