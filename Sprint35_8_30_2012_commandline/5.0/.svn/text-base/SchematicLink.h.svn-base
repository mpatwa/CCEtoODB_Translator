// $Header: /CAMCAD/4.6/SchematicLink.h 15    5/31/07 6:36p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// SchematicLink.h

#if !defined(__SchematicLink__h__)
#define __SchematicLink__h__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ccDoc.h"
#include "Data.h"
#include "File.h"
#include "afxcmn.h"
#include "listcontrol.h"

#define UNDEFINE_INDEX    -1

#define ERR_SETOFF            1
#define ERR_NOERROR           0
#define ERR_GENERAL           -1
#define ERR_BADTYPE           -2
#define ERR_NOTFOUND          -3
#define ERR_CANTCOMPARE       -4
#define ERR_NOTANET           -5
#define ERR_NODATA            -6

class CompPinItem;
class NetItem;
enum KillTrailingType
{
	killTrailingNone,
	killTrailingAlpha,
	killTrailingSuffix,
};

class SchSortItem
{
public:
   SchSortItem(const CString name, const CString item)
   {
      m_name = name;
      m_item = item;
   }
   ~SchSortItem()
   {
   }

private:
   CString m_name;
   CString m_item;

public:
   CString getName() const { return m_name; }
   CString getItem() const { return m_item; }
};

//---------------------------------------------------------------------------------------
// ItemProperty
//---------------------------------------------------------------------------------------
class ItemProperty
{
public:
   ItemProperty(const CString name);
   ~ItemProperty();

private:
   CString m_name;         // refdes of insert or netname
   CString m_compareName;  // value of attribute set by users to be use for comparision
   CString m_sortKey;      // sort key generated from m_filteredName
   bool m_useCrossReferenceMatch;

   //CString generateSortKey(CString name);

public:
   void setCompareName(const CString compareName);
   void setUseCrossReferenceMatch(const bool used) { m_useCrossReferenceMatch = used; }   

   CString getName() { return m_name; }
   CString getCompareName() { return m_compareName; }
   CString getSortKey() { return m_sortKey; }
   bool getUseCrossReferenceMatch() { return m_useCrossReferenceMatch; }
};

//---------------------------------------------------------------------------------------
// ItemData
//---------------------------------------------------------------------------------------
class ItemData
{
public:
   ItemData(DataStruct* dataStruct, const int sheetBlockNumber);
   ~ItemData();

private:
   DataStruct* m_dataStruct; 
   int m_sheetBlockNumber; // the block number that the dataStruct belongs to

public:
   int getSheetBlockNumber() { return m_sheetBlockNumber; }
   DataStruct* getDataStruct() { return m_dataStruct; }
};

//---------------------------------------------------------------------------------------
// ComponentItem
//---------------------------------------------------------------------------------------
class ComponentItem : public ItemProperty,  public ItemData
{
public:
   ComponentItem(const CString name, DataStruct* dataStruct, const int sheetBlockNumber);
   ~ComponentItem();

   void empty();

private:
   CTypedMapStringToPtrContainer<CompPinItem*> m_compPinItemMap;
   
   // This list is not a container, and must be a list because Pcb component can have a match
   // to a Schematic component this is seperated into several component
   CTypedPtrListContainer<ComponentItem*> m_matchedComponetItemList;   

public:
   void clearMatchedComponentItem();
   void addMatchedComponentItem(ComponentItem* matchedComponentItem);
   POSITION getMatchedComponentItemStartPosition();
   ComponentItem* getFirstMatchedComponentItem();
   ComponentItem* getNextMatchedComponentItem(POSITION& pos);
   int getMatchedCount();

   void addCompPinItem(CompPinItem* compPinItem);
   POSITION getCompPinItemStartPosition();
   CompPinItem* getNextCompPinItem(POSITION& pos);
   CompPinItem* findCompPinItem(const CString pinName);
};

class ComponentCollection : public CTypedMapStringToPtrContainer<ComponentItem*> //  CTypedMapSortStringToObContainer<ComponentItem>
{
public:
   ComponentCollection(const bool isContainer) : CTypedMapStringToPtrContainer<ComponentItem*>(nextPrime2n(20),isContainer)
   {
   }
   ~ComponentCollection()
   {
   };
};

//---------------------------------------------------------------------------------------
// CompPinItem
//---------------------------------------------------------------------------------------
class CompPinItem
{
public:
   CompPinItem(const CString compName, const CString pinName, ComponentItem* componentItem, NetItem* netItem);
   ~CompPinItem();

private:
   CString m_compName;
   CString m_pinName;
   CString m_compareCompName;
   CString m_comparePinName;
   ComponentItem* m_componentItem;
   NetItem* m_netItem;
   CompPinItem* m_matchedCompPinItem;

   void empty();

public:
   CString getPinName() const { return m_pinName; }
   CString getCompName() const { return m_compName; }
   CString getComparePinName() const { return m_comparePinName; }
   CString getCompareCompName() const { return m_compareCompName; }
   CString getCompareCompPinName() const { return m_compareCompName + "." + m_comparePinName; }
   void setCompareName(const CString compareCompName, const CString comparePinName);

   void setMatchedCompPinItem(CompPinItem* matchedCompPinItem);
   CompPinItem* getMatchedCompPinItem();

   NetItem* getNetItem();
};

class CompPinCollection : public CTypedMapStringToPtrContainer<CompPinItem*>
{
public:
   CompPinCollection(const bool isContainer) : CTypedMapStringToPtrContainer<CompPinItem*>(nextPrime2n(20), isContainer)
   {
   }
   ~CompPinCollection()
   {
   }
};

//---------------------------------------------------------------------------------------
// NetItem
//---------------------------------------------------------------------------------------
class NetItem : public ItemProperty
{
public:
   NetItem(const CString name);
   ~NetItem();
  
private:
   CTypedPtrListContainer<ItemData*> m_netDataList;         // is container
   CTypedPtrListContainer<CompPinItem*> m_compPinItemList;  // is not container
   CTypedMapStringToPtrContainer<NetItem*> m_matchedNetItemList;   // is not a container, and must be a list because Pcb net can have more than one Schematic match
   NetStruct* m_netStruct;
   int m_compPinMatchedCount;

   void empty();

public:
   void addNetData(DataStruct* dataStruct, const int sheetBlockNumber);

   void setNetStruct(NetStruct* netStruct);
   NetStruct* getNetStruct();

   void clearMatchedNetItem();
   void addMatchedNetItem(NetItem* matchedNetItem);
   POSITION getMatchNetItemStartPosition();
   NetItem* getNextMatchedNetItem(POSITION& pos);
   NetItem* getFirstMatachNetItem();
   int getMatchedCount();

   void addCompPinItem(CompPinItem* compPinItem);
   POSITION getCompPinItemStartPosition();
   CompPinItem* getNextCompPinItem(POSITION& pos);
   int getCompPinItemCount();

   POSITION getItemDataStartPosition();
   ItemData* getNextItemData(POSITION& pos);
   ItemData* getFirstItemData();
   int getItemDataCount();
};

class NetCollection : public CTypedMapStringToPtrContainer<NetItem*> // CTypedMapSortStringToObContainer<NetItem>
{
public:
   NetCollection(const bool isContainer) : CTypedMapStringToPtrContainer<NetItem*>(nextPrime2n(20),isContainer)
   {
   }
   ~NetCollection()
   {
   }
};

//---------------------------------------------------------------------------------------
// SchematicLinkCollection 
//---------------------------------------------------------------------------------------
class SchematicLinkCollection
{
public:
   SchematicLinkCollection(bool isSchematic);
   ~SchematicLinkCollection();

private:
   bool m_isSchematic;
   bool m_isCaseSensitive;
   ComponentCollection m_componentCollection;   // map of ComponentItem set by ComponentItem->m_name, which is insert->getRefdes()
   CompPinCollection m_compPinCollection;       // array of CompPinItem
   NetCollection m_netCollection;               // map of NetItem set by NetItem->m_name in lower case, which is NETNAME attribute
   CMapStringToString m_compNameMap;            // map of comp name from comppin (value of compDesignator attribute)

	// Need a map to ComponentItem in collection by ComponentItem->m_compareName

   void fileCollectionFromNetlist(CCEtoODBDoc& camcadDoc, FileStruct& fileStruct);
   void fillComponentAndNetCollection(CCEtoODBDoc& camcadDoc, FileStruct& fileStruct, const int netNameKw, const int componentCompareNameKw);
   void fillComponentAndNetCollection(CCEtoODBDoc& camcadDoc, BlockStruct& block, const int netNameKw, const int componentCompareNameKw);
   //void fillNetCollectionWithNetList(CCEtoODBDoc& camcadDoc, FileStruct& fileStruct);
   void addComponentItemWithData(CCEtoODBDoc& camcadDoc, DataStruct& data, const int sheetBlockNumber, const int compareNameKw);
   void addNetItemWithData(CCEtoODBDoc& camcadDoc, DataStruct& data, const int sheetBlockNumber, const int netNameKw);
   NetItem& addNetItem(const CString netName);
   CompPinItem* addCompPinItem(CCEtoODBDoc& camcadDoc, CompPinStruct& comppin, NetItem& netItem, const int compDesignatorKw, const int pinDesignatorKw);

   static int sortComponentItemByFilteredName(const void* elem1,const void* elem2);
   static int sortNetItemByFilteredName(const void* elem1,const void* elem2);
   static int sortByFilteredName(const bool isComponentSort, const void* elem1,const void* elem2);

public:
   void empty();
   void fillCollection(CCEtoODBDoc& camcadDoc, FileStruct& fileStruct);
   void sort();

   void clearComponenitItemMatch();
   void addComponentItem();
   POSITION getComponentStartPosition();
   ComponentItem* getNextComponent(POSITION& pos);
   ComponentItem* findComponentItem(const CString refname);

   void clearNetItemMatch();
   POSITION getNetItemStartPosition();
   NetItem* getNextNetItem(POSITION& pos);
   NetItem* findNetItem(const CString netName);
   NetCollection& getNetCollection();

   POSITION getCompPinItemStartPosition();
   CompPinItem* getNextCompPinItem(POSITION& pos);
   CompPinItem* findCompCompPinItemByCompareName(const CString compareCompPinName);
   int getCompPinItemCount();

   CMapStringToString& getCompNameMap() { return m_compNameMap; }
};

//---------------------------------------------------------------------------------------
// SchematicLink
//---------------------------------------------------------------------------------------
class SchematicLink
{
public:
   SchematicLink(CCEtoODBDoc& schematicDoc, CCEtoODBDoc& pcbDoc, FileStruct& schematicFileStruct, FileStruct& pcbFileStruct);
   ~SchematicLink();

private:
   CCEtoODBDoc& m_schDoc;
   CCEtoODBDoc& m_pcbDoc;
   FileStruct& m_schFileStruct;
   FileStruct& m_pcbFileStruct;
   SchematicLinkCollection m_schCollection;
   SchematicLinkCollection m_pcbCollection; 
   DataStruct* m_lastSchColorOverrideDataStruct;
   DataStruct* m_lastPcbColorOverrideDataStruct;
   bool m_isDataLoaded;

   int m_refdesMapKw;
   int m_netNameKw;
   int m_designatorKw;
   int m_compDesignatorKw;
   int m_pinDesignatorKw;


	// Component Comparision Settings
	//bool m_componentCompareNoCase;
	//KillTrailingType m_killTrailing;
	//CString m_trailingSurffix;

	// Net Comparision Settings
	//bool m_netCompareNoCase;
	//bool m_compareFollowByName;
	//NetCompareByType m_compareByType;

   void removeColorOverride();
   void emptyCollections();
   //CString killTrailing(const CString name);
   CString killTrailing(const KillTrailingType killTrailingType, const CString trailingSuffix, const CString name);
   void processCompPinItemMatch();
   void processNetItemMatchByName(const bool caseSensitive, NetCollection& schNetCollection, NetCollection& pcbNetCollection, CMapStringToString& nonMatchSchematicMap, CMapStringToString& nonMatchPcbMap, CMapStringToString& matchSchematicToPcbMap);
   void doFollowUpNetItemMatchByName(const bool caseSensitive, const bool followByNameMatch, CMapStringToString& nonMatchSchematicMap, CMapStringToString& nonMatchPcbMap, CMapStringToString& matchSchematicToPcbMap);
 
   void AnnotateNetnameUsingSchematicResult();

public:
   SchematicLinkCollection& getSchematicCollection() { return m_schCollection; }
   SchematicLinkCollection& getPcbCollection() { return m_pcbCollection; }
   CCEtoODBDoc& getSchematicCamcadDoc() { return m_schDoc; }
   CCEtoODBDoc& getPcbCamcadDoc() { return m_pcbDoc; }
   FileStruct& getSchematicFileStruct() { return m_schFileStruct; }
   FileStruct& getPcbFileStruct() { return m_pcbFileStruct; }

   bool loadData();
   bool processAutoCrosslink();

   void processComponentCrossReference(CMapStringToString& schematicToPcbMap);
   void processNetCrossReference(CMapStringToString& schematicToPcbMap);

   void clearComponentItemMatch();
   void processComponentItemMatch(const bool caseSensitive, const KillTrailingType killTrailingType, const CString trailingSuffix, CMapStringToString& nonMatchSchematicMap, CMapStringToString& nonMatchPcbMap, CMapStringToString& matchSchematicToPcbMap);

   void clearNetItemMatch();
   void processNetItemMatchByName(const bool caseSensitive, CMapStringToString& nonMatchSchematicMap, CMapStringToString& nonMatchPcbMap, CMapStringToString& matchSchematicToPcbMap);
   void processNetItemMatchByContent(const bool caseSensitive, const bool followByNameMatch, const double matchPercentage, CMapStringToString& nonMatchSchematicMap, CMapStringToString& nonMatchPcbMap, CMapStringToString& matchSchematicToPcbMap);
   void processNetItemMatchByContentPcbAsMaster(const bool caseSensitive, const bool followByNameMatch, const double matchPercentage, CMapStringToString& nonMatchSchematicMap, CMapStringToString& nonMatchPcbMap, CMapStringToString& matchSchematicToPcbMap);

   int getSchematicComponentMatch(const CString refname, CString& matchedName);
   int getSchematicNetMatch(DataStruct& dataStruct, CString& matchedName);

   int getPcbComponentMatch(const CString refname, CString& matchedName);
   int getPcbNetMatch(DataStruct& dataStruct, CString& matchedName);

   void zoomToComponent(CCEtoODBDoc& camcadDoc, FileStruct& fileStruct, DataStruct& dataStruct, const int sheetBlockNumber, const bool isComponentSchematic);
   void zoomToNet(CCEtoODBDoc& camcadDoc, FileStruct& fileStruct, NetItem& netItem, const int sheetBlockNumber, const bool isNetSchematic);
   void zoomToNet(CCEtoODBDoc& camcadDoc, FileStruct& fileStruct, DataStruct& dataStruct, const int sheetBlockNumber, const bool isNetSchematic);

	// Need functionto support API
	// - Load Compoent Cross Reference and process component comparision
	// - Load Net Cross Reference and process net comparision
	// - Cross probe schematic component and zoom to it, which mean ability to find Pcb given Schematic or vise visa
	// - Cross probe schematic net and zoom to it, which mean ability to find Pcb given Schematic or vise visa
};

//---------------------------------------------------------------------------------------
// SchematicLinkController
//---------------------------------------------------------------------------------------
class SchematicLinkController
{
public:
   SchematicLinkController();
   ~SchematicLinkController();

private:
   SchematicLink* m_schematicLink;
   bool m_schematicLinkInSession;
   bool m_componentCrossProbeOn;
   bool m_netCrossProbeOn;

   bool startSchematicLink();
   bool getCamcadDocuments(CCEtoODBDoc** schematicDoc, CCEtoODBDoc** pcbDoc);
   bool launchSchematicLinkDialog(SchematicLink& schematicLink); 

public:  
   bool isSchematicLinkInSession() const { return m_schematicLinkInSession; }
   void setComponentCrossProbeOn(const bool on) { m_componentCrossProbeOn = m_schematicLinkInSession && on; }
   bool getComponentCrossProbeOn() const { return m_componentCrossProbeOn; }
   void setNetCrossProbeOn(const bool on) { m_netCrossProbeOn = m_schematicLinkInSession && on; }
   bool getNetCrossProbeOn() const { return m_netCrossProbeOn; }

   void deleteSchematicLink();
   void stopSchematicLink();
   void toggerSchematicLink();
   bool requestDeleteOfSchematicLink();

   void clearCrossProbedColors();
   void removeHighlightedNet();
   void removeOverriddenComponent();

   int getMatchForSchematic(DataStruct& schDataStruct, CString& matchedName);
   int getMatchForPcb(DataStruct& pcbDataStruct, CString& matchedName);
};

//---------------------------------------------------------------------------------------
// AutoCrosslinkResultDlg dialog
//---------------------------------------------------------------------------------------
class AutoCrosslinkResultDlg : public CDialog
{
	DECLARE_DYNAMIC(AutoCrosslinkResultDlg)

public:
	AutoCrosslinkResultDlg(const int pcbCompMatchedCount, const int pcbCompConsideredCount, const int schCompMatchedCount, const int schCompConsideredCount,
                          const int pcbNetMatchedCount, const int pcbNetConsideredCount, const int schNetMatchedCount, const int schNetConsideredCount, 
                          const CString logFilePath); 
	virtual ~AutoCrosslinkResultDlg();
	enum { IDD = IDD_AutoCrosslinkResult };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

   virtual BOOL OnInitDialog();

private:
   int m_pcbCompMatchedCount;
   int m_pcbCompTotalCount;
   int m_schCompMatchedCount;
   int m_schCompTotalCount;
   int m_pcbNetMatchedCount;
   int m_pcbNetTotalCount;
   int m_schNetMatchedCount;
   int m_schNetTotalCount;
   CString m_logFilePath;
   CListControl m_resultsList;

   void displayResults();

public:
   afx_msg void OnBnClickedViewlog();
};

#endif // __SchematicLink__h__
