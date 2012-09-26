// $Header: /CAMCAD/4.6/read_wrt/ckt_out.h 24    5/01/07 4:47p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// CKT_OUT.H

#pragma once

#include "ccdoc.h"
#include "dft.h"
#include "compvalues.h"

class CUserDefinedMessage;

enum ECKTBoardItemType
{
	cktBoardItemTypeUnknown = 0,
	cktBoardItemTypeComponent = 1,
	cktBoardItemTypeVia = 2,
};

/******************************************************************************
* CKTInfoNetNameItem 
*/
class CKTInfoNetNameItem : public CObject
{
public:
	CKTInfoNetNameItem() {};
	CKTInfoNetNameItem(const CKTInfoNetNameItem &info);
	CKTInfoNetNameItem(CCEtoODBDoc* doc, DataStruct* data, CString info, CString netName);
	~CKTInfoNetNameItem() {};

	CKTInfoNetNameItem &operator=(const CKTInfoNetNameItem &other);
	
private:
	CString m_sInfo;
	CString m_sCamcadNetName; // original net name from camcad
   mutable CString m_sCktNetName; // use this for all ckt output, gets special treatment if net type is power or ground, otherwise same as camcad net name
	CString m_sCktValidNetName;  // name check already applied
	DataStruct*   m_pData;
	BOOL       m_bTool;
	CCEtoODBDoc* m_pDoc;

public:
	CString GetInfo() const				   { return m_sInfo;				};
   void SetInfo(CString info)          { m_sInfo = info;          };
	CString GetCamcadNetName() const	   { return m_sCamcadNetName;	};
   CString GetCKTNetName() const;
	CString GetCKTValidNetName() const	{ return m_sCktValidNetName;	};
	DataStruct*  GetData()      	      { return m_pData;  			};
	BOOL    GetTool()   const           { return  m_bTool;         };
	BOOL    SetTool();
   CString GetSortKey();
	
};
typedef CList<CKTInfoNetNameItem> CKTInfoNetNameList;


/******************************************************************************
* CKTBoardItem
*/
class CKTBoardItem : public CObject
{
public:
	CKTBoardItem() { m_eItemType = cktBoardItemTypeUnknown; m_boardItemLogFp = NULL; };
	CKTBoardItem(const CKTBoardItem &item);
	CKTBoardItem(DataStruct *data, CTMatrix mat, CCEtoODBDoc &doc, FILE *logFp, CString msgAttrName, CString refName = "");
	~CKTBoardItem() {};

	CKTBoardItem &operator=(const CKTBoardItem &other);

private:
	CString m_sRefName;
	CString m_sSortKey;
	CString m_sCKTDeviceID; // A CKT-specific name mapped from CAMCAD device type
	CString m_sDeviceType;  // CAMCAD device type from attrib
	CString m_sValue;
	ComponentValues m_cvValue;
	CString m_sPlusTolerance;
	CString m_sMinusTolerance;
	CString m_sMessage;
	BOOL    m_bCapOpen;
	ECKTBoardItemType m_eItemType;
	CString    m_sPlacement;
	DataStruct* m_data;
	FILE *m_boardItemLogFp;
	
	
	CString generateSortKey(CString refName) const;
	CString getCKTDeviceID(CString deviceName, CString subClass, int pinCount) const;
	CString generateMessage(CCEtoODBDoc *doc, DataStruct *data, CTMatrix mat, CString msgAttribName, CUserDefinedMessage *usrDefinedMsg) const;

public:
	CKTInfoNetNameList m_pinList;
	int unit;
	CString GetSortKey() const									{ return m_sSortKey;			};
	ECKTBoardItemType GetItemType() const					{ return m_eItemType;		};
	CString GetCircuitString() const;
	CString GetValueString(CCEtoODBDoc* pDoc) const;
	CString GetRefName() const                         { return m_sRefName;       };
	//CKTInfoNetNameList GetPinList()                    { return m_pinList;       };
	
	BOOL    GetCapOpen() const                         { return m_bCapOpen;       };
	
	CString GetPlacement() const                       { return m_sPlacement;  };
	
	DataStruct*       GetData()                        { return m_data; }

   bool IsPinMappingSupportedByDevType(CCEtoODBDoc &doc);
   CString GetCKTMappedPinName(CCEtoODBDoc &doc, CompPinStruct *cp);

   void InsertPinItem(CKTInfoNetNameItem &item);

};


class CKTBoardItemList
{
public:
	CKTBoardItemList() {};
	~CKTBoardItemList() {};

private:
	CList<CKTBoardItem> m_boardList;

public:
	// remove all elements
	void RemoveAll();

	// iteration
	POSITION GetHeadPosition() const;
	POSITION GetTailPosition() const;
	CKTBoardItem& GetNext(POSITION& rPosition); // return *Position++
	const CKTBoardItem& GetNext(POSITION& rPosition) const; // return *Position++
	CKTBoardItem& GetPrev(POSITION& rPosition); // return *Position--
	const CKTBoardItem& GetPrev(POSITION& rPosition) const; // return *Position--

	// getting/modifying an element at a given position
	CKTBoardItem& GetAt(POSITION position);
	const CKTBoardItem& GetAt(POSITION position) const;
	void RemoveAt(POSITION position);

	void Insert(CKTBoardItem item);
};


/******************************************************************************
* CKTProbe
*/
class CKTProbe : public CKTInfoNetNameItem
{
public:
	CKTProbe() : CKTInfoNetNameItem() {};
	CKTProbe(CCEtoODBDoc* doc, DataStruct* data, CString info, CString netName);
	~CKTProbe() {};

private:
	bool m_bPlaced;
	ETestResourceType m_eResourceType;
   CEntity m_accessEntity;
   CEntity m_probeEntity;
   bool  m_IsProbedDiePin;

public:
	CString GetProbeString() const;
	bool IsPlaced() const										{	return m_bPlaced;				};
	ETestResourceType GetResourceType() const				{	return m_eResourceType;		};
   CEntity GetAccessEntity()                          {  return m_accessEntity;     }
   CEntity GetProbeEntity()                           {  return m_probeEntity;      }
   EEntityType GetEntityType()                        {  return m_probeEntity.getEntityType();}
   bool IsProbedDiePin() const                        {  return m_IsProbedDiePin;}

   void SetProbedDiePin(CCamCadData &cancadData, DataStruct* data);
};
typedef CList<CKTProbe> CKTProbeList;


/******************************************************************************
* CKTWriter
*/
class CKTWriter
{
public:
	CKTWriter(CCEtoODBDoc &doc, FormatStruct *format, int page_units, FILE *logFp);
	~CKTWriter();

private:
	CCEtoODBDoc &m_pDoc;
	FILE *m_cktLogFp;

	FormatStruct *m_format;
	int m_pageUnits;

	CKTBoardItemList m_boardItems;
	CKTProbeList m_probes;
	int LoadSettings(CString fileName);
	int OutputNAVFormat(CString fileName);
	int OutputCKTFormat(CString fileName);

   CString m_messageAttribName; // Case 2079, attrib name to use for MSG (message) field
   CString m_usersAdaptorName; // DR 653414, from settings file .ADAPTOR_NAME

	long m_lViaIndex;
	long m_lBondPadIndex;

	int getAllData();
   bool IsSingleDiePinNet(NetStruct *net);
   bool IsDiePins(CompPinStruct *cp);
   bool IsDiePins(FileStruct *file, CompPinStruct *cp);

	int writeCircuitSection(CFormatStdioFile &file);
	int writeValueSection(CFormatStdioFile &file);
	int writeAdaptorSection(CFormatStdioFile &file);
	int writeCKTFile(CFormatStdioFile &file);
	int writeDPIFile(CFormatStdioFile &file);
	int writeFDSFile(CFormatStdioFile &file);
	int writeFDSAP03File(CFormatStdioFile &file);
	int writeFDSAP04File(CFormatStdioFile &file);
	int writeFDSAP05File(CFormatStdioFile &file);
	int writeFDSAS03File(CFormatStdioFile &file);
	int writeFDSAS10File(CFormatStdioFile &file);
	int writeNDBFile(CFormatStdioFile &file);
	CString writeFDSBoardOutline(CFormatStdioFile &file);
public:
	int WriteFile(CString fileName);
};