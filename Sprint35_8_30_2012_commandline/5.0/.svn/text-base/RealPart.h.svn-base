// $Header: /CAMCAD/4.6/RealPart.h 15    12/06/06 10:58a Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2003. All Rights Reserved.
*/

#pragma once

#define MEMBUF          65536
#define INDENT          3
#define PRECISION       4

// RealPart database guid attribute key
#define REALPART_GUID   "RealPart_GUID"

// constant defines for real part enum values (AC030702)
#define RP_POLY_BODY         1
#define RP_POLY_PIN          2
#define RP_POLY_SILKSCREEN   3
#define RP_POLY_PAD          4
#define RP_POLY_PKG_FOOT     5
#define RP_POLY_PIN_1_DES    6

// RealPartData key names
#define RP_RPXML        "REALPARTXML"
#define RP_FILE         "FILE"
#define RP_LIB_LIST     "LIBRARY_LIST"
#define RP_LIB          "LIBRARY"
#define RP_PKGDEF_LIST  "PACKAGEDEFINITION_LIST"
#define RP_PKGDEF       "PACKAGEDEFINITION"
#define RP_PKG				"PACKAGE"
#define RP_PINDEF_LIST	"PINDEFINITION_LIST"
#define RP_PINDEF			"PINDEFINITION"
#define RP_PIN_LIST		"PIN_LIST"
#define RP_PIN				"PIN"
#define RP_DEVDEF_LIST  "DEVICEDEFINITION_LIST"
#define RP_DEVDEF       "DEVICEDEFINITION"
#define RP_DEV          "DEVICE"
#define RP_PART_DATA    "PART_DATA"
#define RP_PARTDEF_LIST "PARTDEFINITION_LIST"
#define RP_PARTDEF      "PARTDEFINITION"
#define RP_PART_LIST    "PART_LIST"
#define RP_PART         "PART"
#define RP_LP_DATA      "LANDPATTERN_DATA"
#define RP_LPDEF_LIST   "LANDPATTERNDEFINITION_LIST"
#define RP_LPDEF        "LANDPATTERNDEFINITION"
#define RP_LP_LIST      "LANDPATTERN_LIST"
#define RP_LP           "LANDPATTERN"
#define RP_POLY         "POLY"
#define RP_POLYPOINT    "POLYPOINT"
#define RP_ATTRIB_LIST	"ATTRIBUTE_LIST"
#define RP_ATTRIB			"ATTRIBUTE"

// RealPart package layer
#define RP_LAYER_PKG_FOOT_TOP			"Package Foot Top"
#define RP_LAYER_PKG_FOOT_BOTTOM		"Package Foot Bottom"
#define RP_LAYER_PKG_BODY_TOP			"Package Body Top"
#define RP_LAYER_PKG_BODY_BOTTOM				"Package Body Bottom"
#define RP_LAYER_PKG_PIN_TOP			"Package Pin Top"
#define RP_LAYER_PKG_PIN_BOTTOM		"Package Pin Bottom"



bool HasRealPart(FileStruct &file);


class CMapStringToBlock : public CTypedPtrMap<CMapStringToPtr, CString, BlockStruct*>
{
public:
   // Look up
   BlockStruct *Lookup(LPCTSTR key);
};

//class CMapStringToBlock : public CMapStringToPtr
//{
//public:
// CMapStringToBlock() : CMapStringToPtr() {};
// ~CMapStringToBlock() {};
//
// // Look up
// BlockStruct *Lookup(LPCTSTR key);
// BOOL Lookup(LPCTSTR key, BlockStruct *&block) const;
//
// // add a new (key, value) pair
// void SetAt(LPCTSTR key, BlockStruct *block);
//
// // iterating all (key, value) pairs
// void GetNextAssoc(POSITION &rNextPosition, CString &rKey, BlockStruct *&block) const;
//};

class CMapStringToData : public CTypedPtrMap<CMapStringToPtr, CString, DataStruct*>
{
public:
   // Look up
   DataStruct *Lookup(LPCTSTR key);
};

//class CMapStringToData : public CMapStringToPtr
//{
//public:
// CMapStringToData() : CMapStringToPtr() {};
// ~CMapStringToData() {};
//
// // Look up
// DataStruct *Lookup(LPCTSTR key);
// BOOL Lookup(LPCTSTR key, DataStruct *&data) const;
//
// // add a new (key, value) pair
// void SetAt(LPCTSTR key, DataStruct *data);
//
// // iterating all (key, value) pairs
// void GetNextAssoc(POSITION &rNextPosition, CString &rKey, DataStruct *&data) const;
//};

struct SLibrary
{
   int number;
   CString GUID;
   FileStruct *file;
};

class CMapStringToLibrary : public CTypedPtrMap<CMapStringToPtr, CString, SLibrary*>
{
public:
   CMapStringToLibrary(CCEtoODBDoc *doc);
   ~CMapStringToLibrary();

   // Look up
   SLibrary *Lookup(LPCTSTR key);
};

//class CMapStringToLibrary : public CMapStringToPtr
//{
//public:
// CMapStringToLibrary(CCEtoODBDoc *doc);
// ~CMapStringToLibrary();
//
// // Look up
// SLibrary *Lookup(LPCTSTR key);
// BOOL Lookup(LPCTSTR key, SLibrary *&library) const;
//
// // add a new (key, value) pair
// void SetAt(LPCTSTR key, SLibrary *library);
//
// // iterating all (key, value) pairs
// void GetNextAssoc(POSITION &rNextPosition, CString &rKey, SLibrary *&library) const;
//};

class CMapWordToLibrary : public CMapWordToPtr
{
public:
   CMapWordToLibrary(CCEtoODBDoc *doc);
   ~CMapWordToLibrary();

   // Look up
   SLibrary *Lookup(WORD key);
   BOOL Lookup(WORD key, SLibrary *&library) const;

   // add a new (key, value) pair
   void SetAt(WORD key, SLibrary *library);

   // iterating all (key, value) pairs
   void GetNextAssoc(POSITION &rNextPosition, WORD &rKey, SLibrary *&library) const;
};


class CRealPartWrite
{
public:
   CRealPartWrite(CCEtoODBDoc *doc);
   ~CRealPartWrite();

   int GatherRPData();
   ULONGLONG GetLength();
   BYTE *Detach();

private:
   CCEtoODBDoc *doc;
   CMemFile *mFile;
   int indent;

   ///////////////////////////////////////////////////////////////////////////////
   // member variables for gather layout data
   CMapWordToLibrary *libMap;

   CMapStringToBlock lpGeoms;
   CMapStringToBlock prtGeoms;
   CMapStringToBlock pkgGeoms;
   CMapStringToBlock devGeoms;

   CMapStringToData lpDatas;
// CMapStringToData pkgDatas;
   CMapStringToData prtDatas;

   ///////////////////////////////////////////////////////////////////////////////
   // helper functions
   int increaseIndent();
   int decreaseIndent();
   int getBottomStackNum();
   CPolyList *getRPPad(DataStruct *padstack);

   ///////////////////////////////////////////////////////////////////////////////
   // methods to gather information from the layout design
   void getAllData(BlockStruct *fileBlock);

   void addLP(CString key, DataStruct *data);
   void addPRT(CString key, DataStruct *data);
   void addPKG(CString key, DataStruct *data);

   void addLP(CString key, BlockStruct *block);
   void addPRT(CString key, BlockStruct *block);
   void addPKG(CString key, BlockStruct *block);

   ///////////////////////////////////////////////////////////////////////////////
   // writing methods
   void writeLibraryList();
   void writeLibrary(SLibrary *library);

   void writeFile(FileStruct *file, POSITION filePos);

   void writePartData();
   void writePackageDefinitionList();
   void writePackageDefinition(BlockStruct *block);
   void writePackage(DataStruct *data);

   void writeDeviceDefinitionList();
   void writeDeviceDefinition(BlockStruct *block);
   void writeDevice(DataStruct *data);

   void writePartDefinitionList();
   void writePartDefinition(BlockStruct *block);
   void writePartList();
   void writePart(DataStruct *data);

   void writeLandPatternData();
   void writeLandPatternDefinitionList();
   void writeLandPatternDefinition(BlockStruct *block);
   void writeLandPatternList();
   void writeLandPattern(DataStruct *data);

   void writeAttributes(CAttributes* attribMap);
};



/******************************************************************************
* CComponentRealpartLink 
*/
class CComponentRealpartLink : public CObject
{
public:
	CComponentRealpartLink(CCEtoODBDoc &doc);
	~CComponentRealpartLink();

private:
	CCEtoODBDoc &m_doc;
	WORD m_loadedKW;
	DataStruct *m_pComponentData;
	DataStruct *m_pRealpartData;

public:
	bool IsComponentLoaded() const;
	bool HasMatch() const;

	bool AddComponent(DataStruct *data);
	bool AddRealpart(DataStruct *data);

	bool ShowRealpart();
	bool HideRealpart();

   DataStruct *getRealPartData(){return m_pRealpartData;}
};

class CComponentRealpartLinkMap : public CTypedMapStringToPtrContainer<CComponentRealpartLink*>
{
private:
   CCEtoODBDoc& m_camCadDoc;
   FileStruct& m_fileStruct;

public:
	CComponentRealpartLinkMap(CCEtoODBDoc &doc, FileStruct &file);

   void HideRealpartsForUnloadedComponents();
};