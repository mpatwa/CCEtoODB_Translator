// $Header: /CAMCAD/DcaLib/DcaXmlContent.h 4     6/30/07 3:02a Kurt Van Ness $

#if !defined(__DcaXmlContent_h__)
#define __DcaXmlContent_h__

#pragma once

#include "DcaCollections.h"

// From Lic.h
// flag so can OR a license to be available to more than one product (eg. CPRINT)
// any licenses that may get different prices should get their own license
#define  PRODUCT_GRAPHIC         0x00000001
#define  PRODUCT_PROFESSIONAL    0x00000002
#define  PRODUCT_PCB_TRANSLATOR  0x00000004
#define  PRODUCT_VISION          0x00000008

#define  PRODUCT_ALL             0xffffffff
#define  PRODUCT_NONE            0

enum DataTypeTag;

class CCamCadData;
class CAttributes;
class FileStruct;
class CDataList;
class DataStruct;
class CPolyList;
class CPntList;
class CNetList;
class CBusList;
class CBusStruct;
class CVariantList;
class CVariant;
class CDeviceTypeDirectory;
class CDRCList;
class DRCStruct;
class CCompPinList;
class CNamedView;
class CGTabTableList;

//_____________________________________________________________________________
class CMapWordToWord : public CMap<WORD, WORD, WORD, WORD>
{
};

//_____________________________________________________________________________
class CDcaXmlContent ///XMLContent
{
private:
   CCamCadData& m_camCadData;

public:
   CDcaXmlContent(CCamCadData& camCadData);
	~CDcaXmlContent();
   virtual BOOL startElement(CString localTagName, CMapStringToString* tAttributes);
   virtual void endElement(CString localTagName);
   static void SwapEscapeSequences(CString &strVal,  bool escToChar);

public:
   BOOL UserCancel;

   //virtual void SetDoc(CCEtoODBDoc* ccDoc);
   //virtual CCEtoODBDoc* GetDoc();

   CCamCadData& getCamCadData() { return m_camCadData; }

protected:
   //CCEtoODBDoc* doc;
   int netKeywordIndex;
   BOOL askLoadNonGrahpicData;
   CAttributes** m_currentAttribMap;
   CAttributes** lastAttribMap;

	FileStruct* m_currentFile;
   CDataList* m_currentDataList;
   BlockStruct* m_currentBlock;
   DataStruct* currentData;
   CPolyList* currentPolyList;
   CPntList* currentPntList;
   CNetList* currentNetList;
	CBusList* currentBusList;
	CBusStruct* currentBus;
	CVariantList* currentVariantList;
	CVariant* currentVariant;
   CDeviceTypeDirectory* currentTypeList;
   CDRCList* currentDRCList;
   DRCStruct* currentDRC;
   CCompPinList* currentCompPinList;
   CNamedView* currentNamedView;
   CGTabTableList* currentTableList;
   CGTabTable* currentTable;
   CGTabRow* currentTableRow;
	CMapStringToInt m_indexMap;
	CMapWordToWord m_remappingMap;
   CPoly3DList* currentPoly3DList;
   CPnt3DList* currentPnt3DList;

	CString currentMachineName;

   BOOL GetAttribute(CMapStringToString* attributes, CString attName, CString* attValue);

private:
   void AddAttrib(CMapStringToString* attributes);
   void AddKeyword(CMapStringToString* attributes);
   void AddLayer(CMapStringToString* attributes);
   void AddTable(CMapStringToString* attributes);
   void AddTableCol(CMapStringToString* attributes);
   void AddTableRow(CMapStringToString* attributes);
   void AddTableRowCell(CMapStringToString* attributes);
   void AddGeometry(CMapStringToString* attributes);
   void AddData(CMapStringToString* attributes,DataTypeTag dataType);
   void AddPoly(CMapStringToString* attributes);
   void AddPnt(CMapStringToString* attributes);
   void AddFile(CMapStringToString* attributes);
   void AddNet(CMapStringToString* attributes);
   void AddBus(CMapStringToString* attributes);
   void AddNetRef(CMapStringToString* attributes);
   void AddBusRef(CMapStringToString* attributes);
	void AddVariant(CMapStringToString* attributes);
	void AddVariantItem(CMapStringToString* attributes);
   void AddCompPin(CMapStringToString* attributes);
   void AddType(CMapStringToString* attributes);
   void AddDRC(CMapStringToString* attributes);
   void AddDRC_Measure(CMapStringToString* attributes);
   void AddDRC_Net(CMapStringToString* attributes);
   void AddDRC_Algorithm(CMapStringToString* attributes);
   void AddWidth(CMapStringToString* attributes);
   void AddNamedView(CMapStringToString* attributes);
   void AddNamedViewLayer(CMapStringToString* attributes);
   void AddSettings(CMapStringToString* attributes);
   void AddBackgroundBitmap(CMapStringToString* attributes, BOOL Top);
   void AddTopic(CMapStringToString *attributes);
   void AddIssue(CMapStringToString *attributes);
   void AddTopicNode(CMapStringToString *attributes, TopicNodeTypeTag nodetype);
   void AddPoly3D(CMapStringToString* attributes);
   void AddPnt3D(CMapStringToString* attributes);
};

#endif
