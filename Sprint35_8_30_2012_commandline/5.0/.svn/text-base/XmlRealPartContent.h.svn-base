
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/

#pragma once

#include "data.h"
#include "file.h"
#include "realpart.h"
#include "DcaXmlContent.h"
class CCEtoODBDoc;

typedef CList<CAttributes**, CAttributes**> CAttributeMapList;
typedef CList<CDataList*, CDataList*> CDataListMapList;

template<class TYPE, class ARG_TYPE = const TYPE&>
class CStack
{
public:
   CStack();
   ~CStack() {};

   void Push(ARG_TYPE attribMap);
   TYPE Pop();

   int StackHeight();

private:
   CList<TYPE, ARG_TYPE> stack;
};

//_____________________________________________________________________________
class XMLRealPartContent : public CDcaXmlContent
{
private:
   CCEtoODBDoc& m_camCadDoc;

	// License flags
	bool pkgLic;
	bool devLic;

	CStack<CAttributes**, CAttributes**> attribStack;
   CStack<CDataList*, CDataList*> dataListStack;

   CMapWordToLibrary *libMap;
   CMapStringToBlock packageMap;
   CMapStringToBlock deviceMap;
   CMapStringToBlock partMap;
   CMapStringToBlock pinMap;

   CMapStringToData lpDataMap;

   FileStruct *currentVisBoard;
   int footLayerTop, footLayerBot;
   int pkgBodyTop, pkgBodyBot;
   int pkgPinTop, pkgPinBot;

public:
   XMLRealPartContent(CCEtoODBDoc& camCadDoc);

   CCEtoODBDoc& getCamCadDoc() { return m_camCadDoc; }
   CCamCadData& getCamCadData() { return m_camCadDoc.getCamCadData(); }

   virtual BOOL startElement(CString localTagName, CMapStringToString *tAttributes);
   virtual void endElement(CString localTagName);

protected:
   //virtual void SetDoc(CCEtoODBDoc *ccDoc);

private:
   CString buildDefinitionName(CString name, SLibrary *lib);

   void initRealPartData(CMapStringToString *attributes);
   void addLibrary(CMapStringToString *attributes);

   void addPackageDefinition(CMapStringToString *attributes);
   void addPackage(CMapStringToString *attributes);

   void addPoly(CMapStringToString *attributes);
   void addPolyPoint(CMapStringToString *attributes);

   void addDeviceDefinition(CMapStringToString *attributes);
   void addDevice(CMapStringToString *attributes);

   void addPartDefinition(CMapStringToString *attributes);
   void addPart(CMapStringToString *attributes);

   void addPinDefinition(CMapStringToString *attributes);
   void addPin(CMapStringToString *attributes);

   void addAttrib(CMapStringToString *attributes);

	void inheritAttributes(DataStruct *data, BlockStruct *block);
};