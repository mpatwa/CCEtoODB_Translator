// $Header: /CAMCAD/4.6/DataDoctor.h 44    4/16/07 12:06p Rick Faltersack $

#if ! defined (__DataDoctor_h__)
#define __DataDoctor_h__

#pragma once

#include "CamCadDatabase.h"
#include "CompValues.h"
#include "DeviceType.h"
#include "RealPart.h"
#include "ODBC_Lib.h"
#include "GenerateCentroidAndOutline.h"
#include "Element.h"

class CDataDoctorComponent;

#define QFromLibraryExist	"EXIST"
#define QFromLibraryLoaded	"LOADED"

#define QPinMapUnmap			"UNMAP"
#define QPinMapMapped		"MAPPED"

#define QGeometry          "Geometry"
#define QPartNumber        "Part Number"

//_____________________________________________________________________________
// This is to support the control of the initially selected page when
// Data Doctor is opened. It is for the requirement to redirect the old
// Generate Component Centroids menu option to the Data Doctor with
// Centroids tab open. That is supposed to be short lived, and this can
// go after that. Or maybe it is handy enough to keep.
//
// The property sheet has three functions that map these to strings,
// map strings to these tage, and map the tags to the data doctor
// tab pages. Keep this all in sync!
//
enum DdPageTag
{
   DdUnknownPage,
   DdComponentsPage,
   DdPartsPage,
   DdCentroidsPage,
   DdPackagesPage,
   DdParallelAnalysisPage,
   DdLibraryPage,
   DdSubclassesPage,
};

//_____________________________________________________________________________
enum DataDoctorLibraryTag
{
	dataDoctorLibraryNo,
	dataDoctorLibraryExist,
	dataDoctorLibraryLoaded
};

//_____________________________________________________________________________
enum DataDoctorStatusTag
{
   dataDoctorStatusError,
   dataDoctorStatusOk,
   dataDoctorStatusIgnore,
   dataDoctorStatusUndefined
};

//_____________________________________________________________________________
enum DataDoctorPackageSourceTag
{
   dataDoctorPkgSrcPartnumber,
   dataDoctorPkgSrcGeometry
};

//_____________________________________________________________________________

class CDataDoctorLibraryAttrib : public CObject
{
private:
   CString m_name;
   int     m_keywordIndex;
   bool    m_active;

public:
   CDataDoctorLibraryAttrib(CString name)                                { m_name = name; m_keywordIndex = -1; m_active = false; }
   CDataDoctorLibraryAttrib(CString name, int keywordIndex, bool active) { m_name = name; m_keywordIndex = keywordIndex; m_active = active; }

   CString GetName()       { return m_name; }
   int GetKeywordIndex()   { return m_keywordIndex; }
   bool IsActive()         { return m_active; }

   void SetName(CString name)       { m_name = name; }
   void SetKeywordIndex(int indx)   { m_keywordIndex = indx; }
   void SetActive(bool active)      { m_active = active; }

};


class CDataDoctorLibraryAttribMap : public CMapSortedStringToOb<CDataDoctorLibraryAttrib>
{
private:
   CString m_settingsID;
   CStringArray m_prohibitedNames;

public:
   CDataDoctorLibraryAttribMap(CString settingsID);

   CString GetActiveAttribListStr(); // Mainly for saving user settings to registry, all active names in comma separated list

   void LoadSettings(bool reset = true);
   void SaveSettings();

   void LoadAttribNamesFromDB(bool reset = false);  // intended for use after LoadSettings, but could be stand alone

   bool IsProhibited(const CString &name);
};

//_____________________________________________________________________________

class CDataDoctorPin : public CObject
{
private:
   CompPinStruct& m_pin;
   NetStruct& m_net;

public:
   CDataDoctorPin(CompPinStruct& pin,NetStruct& net);

   CString getSortablePinRef() { return m_pin.getSortablePinRef(); }
   CompPinStruct& getCompPin() { return m_pin; }
   NetStruct& getNet() { return m_net; }
};

//_____________________________________________________________________________

class CDataDoctorPackageInfo
{
private:
   CString m_packageAlias;
   DataDoctorPackageSourceTag m_packageSource;
   CString m_outlineMethod;
   bool m_outlineFilled;
   double m_packageHeight;  // Always stored here in mm
   int m_dpmo; // Defects Per Million Opportunities
   CString m_Family;
   CString m_packageComment;
   CString m_partComment;

   CMapStringToString m_genericAttribs;

public:
   CDataDoctorPackageInfo()      { m_packageSource = dataDoctorPkgSrcGeometry; m_outlineFilled = false; m_packageHeight = 0.0; m_dpmo = 0;}

   CString GetPackageAlias()                          { return m_packageAlias; }
   CString GetPackageSource()                         { return m_packageSource == dataDoctorPkgSrcPartnumber ? QPartNumber : QGeometry; }
   CString GetOutlineMethod()                         { return m_outlineMethod; }
   OutlineAlgorithmTag GetOutlineMethodTag()          { return stringToOutlineAlgTag( m_outlineMethod ); }
   bool    GetOutlineFilled()                         { return m_outlineFilled; }
   double GetPackageHeight(PageUnitsTag toUnits)      { return m_packageHeight * getUnitsFactor(pageUnitsMilliMeters, toUnits); }
   CString GetPackageHeightStr(PageUnitsTag toUnits)  { double h = this->GetPackageHeight(toUnits); CString buf; if (h > 0.0) buf.Format("%.*f", GetDecimals(toUnits), h); return buf; } // empty string for h = 0 or less
   int GetDPMO()                                      { return m_dpmo; }
   CString GetDPMOStr()                               { CString buf; if (m_dpmo > 0) buf.Format("%d", m_dpmo); return buf; }
   CString GetFamily()                                { return m_Family; }
   CString GetPackageComments()                       { return m_packageComment; }
   CString GetPartComments()                          { return m_partComment; }

   CString GetGenericAttrib(CString name)             { CString buf; m_genericAttribs.Lookup(name, buf); return buf; }

   CDBGenericAttributeMap *AllocDBAttribMap();

   void SetPackageAlias(CString name)                             { m_packageAlias = name; }
   void SetPackageSource(CString name)                            { m_packageSource = (name.CompareNoCase(QPartNumber) == 0 ? dataDoctorPkgSrcPartnumber : dataDoctorPkgSrcGeometry); }
   void SetOutlineMethod(CString name)                            { m_outlineMethod = name; }
   void SetOutlineFilled(bool flag)                               { m_outlineFilled = flag; }
   void SetPackageHeight(double h, PageUnitsTag fromUnits)        { m_packageHeight = h * getUnitsFactor(fromUnits, pageUnitsMilliMeters); }
   void SetPackageHeight(CString h, PageUnitsTag fromUnits)       { SetPackageHeight(atof(h), fromUnits); }
   void SetDPMO(int dpmo)                                         { m_dpmo = dpmo; }
   void SetDPMO(CString dpmoStr)                                  { m_dpmo = atoi(dpmoStr); }
   void SetFamily(CString family)                                 { m_Family = family; }
   void SetPackageComments(CString comment)                       { m_packageComment = comment; }
   void SetPartComments(CString comment)                          { m_partComment = comment; }

   void SetGenericAttrib(CString name, CString value)             { m_genericAttribs.SetAt(name, value); }

   POSITION GetGenericAttribStartPosition()                                      { return m_genericAttribs.GetStartPosition(); }
   void GetNextGenericAttribAssoc(POSITION& pos, CString &name, CString &value)  { m_genericAttribs.GetNextAssoc(pos, name, value); }

   bool IsPackageSource(DataDoctorPackageSourceTag testSrc)       { return (m_packageSource == testSrc); }
};

//_____________________________________________________________________________

class CDataDoctorRealPart
{
private:
   CString m_name;
   CString m_prefix;
   BlockStruct *m_ccRealPartBlk;

   static int m_createdByKeywordIndex;
   static int m_familyKeywordIndex;

public:
   CDataDoctorRealPart(CString prefix);
   CDataDoctorRealPart(CString prefix, CString name);

   CString GetName()                { return m_name; }
   void SetName(CString nm)         { m_name = nm; }

   CString GetRPGeomName()          { return m_prefix + "_" + m_name;     } // CamCad Real Part geometry name
   CString GetRPPkgGeomName()       { return m_prefix + "_PKG_" + m_name; } // CamCad Real Part inserted Package geometry name

   BlockStruct *GetBlock()          { return m_ccRealPartBlk; }  // Overall RealPart block
   void SetBlock(BlockStruct *b)    { m_ccRealPartBlk = b; }

   BlockStruct *GetPkgBlock(CCamCadDatabase& camCadDatabase);  // Inserted Package block
   DataStruct *GetPkgInsertData(CCamCadDatabase& camCadDatabase);  // Inserted Package data

   bool GenerateSelf(CCamCadDatabase& camCadDatabase, CDataDoctorComponent& component, bool filled, bool overwrite = false);

   CDBOutlineVertexMap *AllocDBOutline(CCamCadDatabase& camCadDatabase, CString name, EDBNameType nametype);
   void                 AdoptDBOutline(CCamCadDatabase& camCadDatabase, CDBOutlineVertexMap *vertexMap, bool fillFlag, CDataDoctorComponent *sampleComp);

   bool CloneOutline(CCamCadDatabase& camCadDatabase, CDataDoctorComponent& component, bool fillFlag); // make outline in real part that matches component
   bool CloneOutline(CCamCadDatabase& camCadDatabase, CDataDoctorRealPart &otherRP, int fileNumber, bool fillFlag);
   bool CloneOutline(CCamCadDatabase& camCadDatabase, DataStruct *outlinePolylistInsertData, CPoint2d newOrigin, int fileNumber, bool fillFlag); // make outline in real part that matches polylist insert

   bool HasOutlineData(CCamCadDatabase& camCadDatabase)           { return GetOutlineData(camCadDatabase) != NULL; }
   DataStruct *GetOutlineData(CCamCadDatabase& camCadDatabase);
   CString GetExistingOutlineMethod(CCamCadDatabase& camCadDatabase);
   void RemoveOutlineData(CCamCadDatabase& camCadDatabase);

   void SetOutlineFill(CCamCadDatabase& camCadDatabase, bool flag); // Fill or clear filled in existing outline poly

   bool SaveToDB(CDBInterface &db, CCamCadDatabase &camCadDatabase, CString name, EDBNameType nametype);

   static int getCreatedByKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getFamilyKeywordIndex(CCamCadDatabase& camCadDatabase);

   static void DefineLayers(CCamCadDatabase& camCadDatabase);

};

//_____________________________________________________________________________

class CDataDoctorComponentList : public CTypedPtrList<CPtrList,CDataDoctorComponent*>
{
};

//_____________________________________________________________________________
class CDataDoctorComponent : public CObject
{
private:
   CString m_sortableRefDes;
   DataStruct& m_camcadComponentData;
   CTypedMapSortStringToObContainer<CDataDoctorPin> m_pins;
   CMapStringToString m_pinNameToPinRefMap;
	CMapStringToString m_pinRefToPinNameMap;
   DataDoctorStatusTag m_status;
   CDeviceType* m_deviceType;
   DeviceTypeTag m_deviceTypeTag;
   CString m_errors;
   CDataDoctorComponentList* m_mergedComponents;

   static int m_partNumberKeywordIndex;
   static int m_deviceTypeKeywordIndex;
   static int m_valueKeywordIndex;
   static int m_originalValueKeywordIndex;
   static int m_pToleranceKeywordIndex;
   static int m_nToleranceKeywordIndex;
   static int m_toleranceKeywordIndex;
   static int m_subclassKeywordIndex;
   static int m_loadedKeywordIndex;
   static int m_deviceToPackagePinmapKeywordIndex;
   static int m_mergedStatusKeywordIndex;
   static int m_mergedValueKeywordIndex;
   static int m_capacitiveOpensKeywordIndex;
	static int m_diodeOpensKeywordIndex;
	static int m_descriptionKeywordIndex;
   static int m_packageAliasKeywordIndex;
   static int m_packageSourceKeywordIndex;
   static int m_outlineMethodKeywordIndex;
   static int m_compHeightKeywordIndex;
   static int m_dpmoKeywordIndex;
   static int m_partCommentKeywordIndex;
   static int m_packageCommentKeywordIndex;

public:
   CDataDoctorComponent(CCamCadDatabase& camCadDatabase,DataStruct& component);
   virtual ~CDataDoctorComponent();
   void calculateStatus(CCamCadDatabase& camCadDatabase);

   static int getPartNumberKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getPackageAliasKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getDPMOKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getPackageSourceKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getOutlineMethodKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getDeviceTypeKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getValueKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getOriginalValueKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getPToleranceKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getNToleranceKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getToleranceKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getSubclassKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getLoadedKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getDeviceToPackagePinmapKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getMergedStatusKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getMergedValueKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getCapacitiveOpensKeywordIndex(CCamCadDatabase& camCadDatabase);
	static int getDiodeOpensKeywordIndex(CCamCadDatabase& camCadDatabase);
	static int getDescriptionKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getCompHeightKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getGenericAttribKeywordIndex(CCamCadDatabase& camCadDatabase, CString attribName);
   static int getPartCommentKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getPackageCommentKeywordIndex(CCamCadDatabase& camCadDatabase);
   static void resetKeywordIndices();

   DataStruct *GetOldStyleDFTOutlineData(CCamCadDatabase& camCadDatabase);
   CString GetExistingOutlineMethod(CCamCadDatabase& camCadDatabase);

   DataStruct *FindAssociatedRealPartInsertData(CCamCadDatabase& camCadDatabase);

   CPoint2d GetPinsCentroid(CCamCadDatabase& camCadDatabase); // Centroid of group of all pins

   DataDoctorStatusTag getStatus() { return m_status; }
   int getPinCount() const;
   int getNoConnectPinCount(CCamCadDatabase& camCadDatabase);
   CString getSortableRefDes() { return m_sortableRefDes; }
   CString getRefDes()        { return m_camcadComponentData.getInsert()->getRefname(); }
   PointStruct getOrigin()    { return m_camcadComponentData.getInsert()->getOrigin(); }
   double getAngleRadians()   { return m_camcadComponentData.getInsert()->getAngleRadians(); }
   bool isPlacedTop()         { return m_camcadComponentData.getInsert()->getPlacedTop(); }
   int getGeometryBlockNum()  { return m_camcadComponentData.getInsert()->getBlockNumber(); }
   DataStruct *getComponent() { return &m_camcadComponentData; }
   BlockStruct *getGeometryBlock(CCamCadDatabase& camCadDatabase); // inserted block
   CString getGeometryName(CCamCadDatabase& camCadDatabase); // aka inserted block's name
   CString getStatusString();
   CString getGenericAttrib(CCamCadDatabase& camCadDatabase, CString attribName);
   CString getPartNumber(CCamCadDatabase& camCadDatabase);
   CString getPackageAlias(CCamCadDatabase& camCadDatabase);
   CString getDPMO(CCamCadDatabase& camCadDatabase);
   CString getPackageSource(CCamCadDatabase& camCadDatabase);
   CString getOutlineMethod(CCamCadDatabase& camCadDatabase);
   CString getCompHeight(CCamCadDatabase& camCadDatabase);
   CString getDeviceType(CCamCadDatabase& camCadDatabase);
   CString getValue(CCamCadDatabase& camCadDatabase);
   CString getNormalizedValue(CCamCadDatabase& camCadDatabase);
   CString getDisplayValue(CCamCadDatabase& camCadDatabase);
   CString getPTolerance(CCamCadDatabase& camCadDatabase);
   CString getNTolerance(CCamCadDatabase& camCadDatabase);
   CString getTolerance(CCamCadDatabase& camCadDatabase);
   CString getNormalizedTolerance(const CString& unnormalizedTolerance);
   CString getNormalizedPTolerance(CCamCadDatabase& camCadDatabase);
   CString getNormalizedNTolerance(CCamCadDatabase& camCadDatabase);
   bool getLoaded(CCamCadDatabase& camCadDatabase);
   CString getDeviceToPackagePinmap(CCamCadDatabase& camCadDatabase,CDataDoctorPin& pin);
   CString getPinCountString();
	CString getPinRefsToPinNamesMapString(const CString delimiter, bool allPins = true);
	CString getPinNameOptionsString(const CString delimiter);
   CString getSubclass(CCamCadDatabase& camCadDatabase);
   CString getCapacitiveOpens(CCamCadDatabase& camCadDatabase);
   CString getDiodeOpens(CCamCadDatabase& camCadDatabase);
   CString getDefinedCapacitiveOpens(CCamCadDatabase& camCadDatabase);
   CString getDefinedDiodeOpens(CCamCadDatabase& camCadDatabase);
   CString getMergedStatus(CCamCadDatabase& camCadDatabase);
   CString getMergedValue(CCamCadDatabase& camCadDatabase);
	CString getDescription(CCamCadDatabase& camCadDatabase);
   CString getPartComments(CCamCadDatabase& camCadDatabase);
   CString getPackageComments(CCamCadDatabase& camCadDatabase);
   CString getErrors();
   DeviceTypeTag getDeviceTypeTag() const { return m_deviceTypeTag; }
   CDeviceType* getDeviceType() { return m_deviceType; }
   DeviceTypeTag getPassiveDeviceType();
   ValueUnitTag getUnits();

   void saveOriginalValue(CCamCadDatabase& camCadDatabase);

   void setAttribute(CCamCadDatabase& camCadDatabase,int keywordIndex,const CString& stringValue);
   void setGenericAttrib(CCamCadDatabase& camCadDatabase, const CString& attribName, const CString& stringValue);
   void setPartNumber(CCamCadDatabase& camCadDatabase,const CString& value);
   void setPackageAlias(CCamCadDatabase& camCadDatabase,const CString& value);
   void setDPMO(CCamCadDatabase& camCadDatabase,const CString& value);
   void setPackageSource(CCamCadDatabase& camCadDatabase,const CString& value);
   void setOutlineMethod(CCamCadDatabase& camCadDatabase,const CString& value);
   void setCompHeight(CCamCadDatabase& camCadDatabase,const CString& value);
   void setDeviceType(CCamCadDatabase& camCadDatabase,const CString& value);
   void setValue(CCamCadDatabase& camCadDatabase,const CString& value);
   void setOriginalValue(CCamCadDatabase& camCadDatabase,const CString& value);
   void setPTolerance(CCamCadDatabase& camCadDatabase,const CString& value);
   void setNTolerance(CCamCadDatabase& camCadDatabase,const CString& value);
	void setPinRefToPinNamesMapString(CCamCadDatabase& camCadDatabase,const CString& value, CString &errMsg);
   void setSubclass(CCamCadDatabase& camCadDatabase,const CString& value);
   void setLoaded(CCamCadDatabase& camCadDatabase,const CString& value);
   void setCapacitiveOpens(CCamCadDatabase& camCadDatabase,const CString& value);
   void setDiodeOpens(CCamCadDatabase& camCadDatabase,const CString& value);
   void setMergedStatus(CCamCadDatabase& camCadDatabase,const CString& value);
   void setMergedValue(CCamCadDatabase& camCadDatabase,double value);
	void setDescription(CCamCadDatabase& camCadDatabase,const CString& value);
   void setPartComments(CCamCadDatabase& camCadDatabase,const CString& stringValue);
   void setPackageComments(CCamCadDatabase& camCadDatabase,const CString& stringValue);
   void removeMergedAttributes(CCamCadDatabase& camCadDatabase);

   void addPin(CDataDoctorPin& pin) { m_pins.SetAt(pin.getSortablePinRef(),&pin);  clearPinNameMaps(); }
   POSITION getPinStartPosition() { return m_pins.GetStartPosition(); }
   void getPinNextAssoc(POSITION& pos,CString& key,CDataDoctorPin*& pin) 
      { m_pins.GetNextAssoc(pos,key,pin); }

   void clearPinNameMaps();
   void createPinMaps(CCamCadDatabase& camCadDatabase);
   bool hasPinFunctionName(CString pinName);
   int countPinsWithFunctionName(CString pinFunctionName);
	CString getPinMapStatus();
   bool IsNoConnect(CCamCadDatabase& camCadDatabase, CDataDoctorPin &pin);

   void removeMergedComponents();
   void addMergedComponent(CDataDoctorComponent& component);
   POSITION getHeadMergedComponentPosition() 
      { return ((m_mergedComponents != NULL) ? m_mergedComponents->GetHeadPosition() : NULL); }
   CDataDoctorComponent* getMextMergedComponent(POSITION& pos) 
      { return ((m_mergedComponents != NULL) ? m_mergedComponents->GetNext(pos) : NULL); }

	void resetPinRefToPinNameMapAttrib(CCamCadDatabase& camCadDatabase);
   bool IsDie(){return m_camcadComponentData.isInsertType(insertTypeDie);}

};

//_____________________________________________________________________________
class CDataDoctorComponents : public CTypedMapSortStringToObContainer<CDataDoctorComponent>
{
public:
   CDataDoctorComponents(bool isContainer=true,int size=128);
   void createPinMaps(CCamCadDatabase& camCadDatabase);
   int GetDieCount();
};

//_____________________________________________________________________________
class CDataDoctorPart : public CObject
{
private:
   CString m_partNumber;
   CDataDoctorComponents m_components;
   DataDoctorStatusTag m_status;
   CString m_errors;
	DataDoctorLibraryTag m_fromLibrary;
//	CDataDoctorPinMap m_pinMap;

   CDataDoctorPackageInfo m_pkgInfo;
   CDataDoctorRealPart m_realPart;
   bool m_isDiePart;

   DataDoctorStatusTag GetPackageAliasStatus(CCamCadDatabase& camCadDatabase, CString &msg);
   DataDoctorStatusTag GetDPMOStatus(CCamCadDatabase& camCadDatabase, CString &msg);
   DataDoctorStatusTag GetOutlineStatus(CCamCadDatabase& camCadDatabase, CString &msg);
   DataDoctorStatusTag GetGeometryStatus(CCamCadDatabase& camCadDatabase, CString &msg);
   DataDoctorStatusTag GetGenericAttribStatus(CCamCadDatabase& camCadDatabase, CString attribName, CString &msg);
   DataDoctorStatusTag GetGenericAttribListStatus(CCamCadDatabase& camCadDatabase, CString &msg);

public:
   CDataDoctorPart(const CString& partNumber, bool isDiePart);
   ~CDataDoctorPart();
   void calculateStatus(CCamCadDatabase& camCadDatabase);

   CDataDoctorComponent* getHeadComponent();

   int getGeometryCount(CCamCadDatabase& camCadDatabase);   // Count of different blocks inserted by this part number

   //CDataDoctorPackageInfo& GetPkgInfo()   { return m_pkgInfo; }
   CDataDoctorRealPart& GetRealPart()  { return m_realPart; }
   void CreateOutlineInserts(CCamCadDatabase& camCadDatabase, BlockStruct *pcbBlock);
   DataDoctorStatusTag GetPackageStatus(CCamCadDatabase& camCadDatabase, CString &msg);

   DataDoctorStatusTag getStatus() { return m_status; }
   CString getPartNumber();
   CString getStatusString();
   CString getDeviceType(CCamCadDatabase& camCadDatabase);
   CString getDisplayValue(CCamCadDatabase& camCadDatabase);
   CString getNormalizedPTolerance(CCamCadDatabase& camCadDatabase);
   CString getNormalizedNTolerance(CCamCadDatabase& camCadDatabase);
   int     getPinCount();
   CString getPinCountString();
	CString getPinRefsToPinNamesMapString(const CString delimiter, bool allPins = true);
	CString getPinNameOptionsString(const CString delimiter);
   CString getSubclass(CCamCadDatabase& camCadDatabase);
   CString getReferenceDesignators();
   CString getLoadedReferenceDesignators(CCamCadDatabase& camCadDatabase);
	CString getDescription(CCamCadDatabase& camCadDatabase);
   CString getGenericAttrib(CCamCadDatabase& camCadDatabase, CString& attribName);
   CString getPartComments(CCamCadDatabase& camCadDatabase);

   CString getErrors();
   bool isLoaded(CCamCadDatabase& camCadDatabase);
   DeviceTypeTag getDeviceTypeTag();
	DataDoctorLibraryTag getFromLibrary();
	CString getFromLibraryString();
	CString getPinMapStatus();

   CString GetDPMO()             { return m_pkgInfo.GetDPMOStr(); }
   CString GetPackageAlias()     { return m_pkgInfo.GetPackageAlias(); }
   CString GetPackageSource()    { return m_pkgInfo.GetPackageSource(); }
   CString GetOutlineMethod()    { return m_pkgInfo.GetOutlineMethod(); }
   OutlineAlgorithmTag GetOutlineMethodTag()       { return m_pkgInfo.GetOutlineMethodTag(); }
   CString GetPackageHeight(PageUnitsTag toUnits)  { return m_pkgInfo.GetPackageHeightStr(toUnits); }
   CString GetPackageGenericAttrib(CString name)   { return m_pkgInfo.GetGenericAttrib(name); }
   CString GetFamily()           { return m_pkgInfo.GetFamily(); }
   CString GetPackageComments()  { return m_pkgInfo.GetPackageComments(); }

   CDBGenericAttributeMap *AllocDBAttribMap()            { return m_pkgInfo.AllocDBAttribMap(); }
   void LoadDBAttribs(CCamCadDatabase& camCadDatabase, CDataDoctorLibraryAttribMap& configuredAttribMap, CDBGenericAttributeMap *attrDataMap, bool overwrite);

   void setPartNumber(CCamCadDatabase& camCadDatabase,const CString& value);
   void setValue(CCamCadDatabase& camCadDatabase,const CString& value);
   void setPTolerance(CCamCadDatabase& camCadDatabase,const CString& value);
   void setNTolerance(CCamCadDatabase& camCadDatabase,const CString& value);
	void setPinRefToPinNamesMapString(CCamCadDatabase& camCadDatabase,const CString& value, CString &errMsg);
   void setSubclass(CCamCadDatabase& camCadDatabase,const CString& value);
   void setDeviceType(CCamCadDatabase& camCadDatabase,const CString& value);
	void setDescription(CCamCadDatabase& camCadDatabase,const CString& value);
   void setPackageAlias(CCamCadDatabase& camCadDatabase,const CString& value);
   void setDPMO(CCamCadDatabase& camCadDatabase,const CString& value);
   void setPackageSource(CCamCadDatabase& camCadDatabase,const CString& value);
   void setPackageSource(CCamCadDatabase& camCadDatabase, EDBNameType value);
   void setOutlineMethod(CCamCadDatabase& camCadDatabase,const CString& value, bool filled, bool apply = true);
   void setPackageHeight(CCamCadDatabase& camCadDatabase, const CString& value);
   void setGenericAttrib(CCamCadDatabase& camCadDatabase, const CString& name, const CString& value);
	void resetFromLibrary();
   void setFamily(CCamCadDatabase& camCadDatabase,const CString& value);
   void addFamilyFromAttribute(CCamCadDatabase& camCadDatabase);
   void setPackageComments(CCamCadDatabase& camCadDatabase,const CString& value);
   void setPartComments(CCamCadDatabase& camCadDatabase,const CString& value);

   void resetPinRefToPinNameMapAttrib(CCamCadDatabase& camCadDatabase);

   void resetComponents()  { this->m_components.RemoveAll(); }
   void addComponent(CCamCadDatabase& camCadDatabase, CDataDoctorComponent& component, CDataDoctorLibraryAttribMap& libraryAttribMap);

   int adoptDBPartValues(CCamCadDatabase& camCadDatabase, CDataDoctorLibraryAttribMap& activeAttribMap, CDBPart *dbpart, bool overwriteFlag);

   // Package Tab info in DB
   bool LoadPackageInfoFromDB(CCamCadDatabase& camCadDatabase, bool outlineFillFlag, CDataDoctorLibraryAttribMap& configuredAttribMap);
   void SavePackageInfoToDB(CCamCadDatabase& camCadDatabase);
   bool IsDiePart() {return m_isDiePart;}
};

//_____________________________________________________________________________
class CDataDoctorParts : public CTypedMapSortStringToObContainer<CDataDoctorPart>
{
public:
   CDataDoctorParts(bool isContainer=true,int size=128);

   void sync(CCamCadDatabase& camCadDatabase,CDataDoctorComponents& components, CDataDoctorLibraryAttribMap& libraryAttribMap);

   // Part set operations
   void storePartDataToLibrary(CCamCadDatabase& camCadDatabase, bool storeAllPart);
   bool loadPartDataFromLibrary(CCamCadDatabase& camCadDatabase, CDataDoctorLibraryAttribMap& activeAttribMap, bool overwriteFlag, bool displayStatusMessage);
   
   // Single part operations
   bool storeOnePartToLibrary(CCamCadDatabase& camCadDatabase, CDataDoctorPart *part, CDBInterface& db);  // Use given CDBInterface
   bool storeOnePartToLibrary(CCamCadDatabase& camCadDatabase, CDataDoctorPart *part);                    // Open a CDBInterface
   bool loadOnePartFromLibrary(CCamCadDatabase& camCadDatabase, CDataDoctorLibraryAttribMap& activeAttribMap, CDataDoctorPart *part, bool overwriteFlag, bool displayStatusMessage, CDBInterface& db);
   bool loadOnePartFromLibrary(CCamCadDatabase& camCadDatabase, CDataDoctorLibraryAttribMap& activeAttribMap, CDataDoctorPart *part, bool overwriteFlag, bool displayStatusMessage);
   CDataDoctorPart *findPart(CString partnumber);

   CDataDoctorPart *FindPartUsingGeomBlk(int compGeomBlockNum);

   // Partnumber based Package Info in DB
   bool loadPackageDataFromLibrary(CCamCadDatabase& camCadDatabase, bool outlineFillMode, CDataDoctorLibraryAttribMap& configuredAttribMap, bool apply, bool displayMessage);
   void storePackageDataToLibrary(CCamCadDatabase& camCadDatabase, bool storeAll);
   int GetDiePartCount();

private:
   void resetComponents();
   void addComponent(CCamCadDatabase& camCadDatabase,CDataDoctorComponent& component, CDataDoctorLibraryAttribMap& libraryAttribMap);

};

//_____________________________________________________________________________

class CDataDoctorPackage : public CObject
{
private:
   CDataDoctorComponent *m_exampleDDComponent;
   BlockStruct *m_insertedGeomBlk;
   CCamCadDatabase *m_ccdb;

   CString m_geometryName;  // aka m_insertedGeomBlk->GetName(), just cached for speed

   CDataDoctorPackageInfo m_pkgInfo;
   CDataDoctorRealPart m_realPart;

   DataDoctorStatusTag GetPackageAliasStatus(CString &msg);
   DataDoctorStatusTag GetDPMOStatus(CString &msg);
   DataDoctorStatusTag GetOutlineStatus(CString &msg);
   DataDoctorStatusTag GetGenericAttribStatus(CString attribName, CString &msg);
   DataDoctorStatusTag GetGenericAttribListStatus(CString &msg);

public:
   CTypedPtrArray<CPtrArray, CDataDoctorComponent*> m_allInsertsOfThisPkg; //*rcf change this to be same kind of list as in DDPart, then consolidate status checker somehow, maybe to RealPart, maybe PkgInfo, maybe some of each

   CDataDoctorPackage(CCamCadDatabase& camCadDatabase, CDataDoctorComponent& ddcomponent, CDataDoctorLibraryAttribMap& libraryAttribMap, bool filled);

   CDataDoctorComponent *GetExampleComponent()  { return m_exampleDDComponent; }
   
   void CreateOutlineInserts(CCamCadDatabase& camCadDatabase, BlockStruct *pcbBlock);
   CDataDoctorRealPart& GetRealPart()  { return m_realPart; }

   void PurgeOldStyleDFTPackageOutline();

   CString GetGeometryName()              { return m_geometryName; }
   
   int GetGeomBlockNum()                  { return m_insertedGeomBlk != NULL ? m_insertedGeomBlk->getBlockNumber() : -1; }

   CString GetRefNameList();  // List of refnames for inserts of this geometry, regardless of Package Source setting

   CString GetDPMO()             { return m_pkgInfo.GetDPMOStr(); }
   CString GetPackageAlias()     { return m_pkgInfo.GetPackageAlias(); }
   CString GetOutlineMethod()    { return m_pkgInfo.GetOutlineMethod(); }
   OutlineAlgorithmTag GetOutlineMethodTag()         { return stringToOutlineAlgTag( GetOutlineMethod() ); }
   CString GetPackageHeight()                        { return m_pkgInfo.GetPackageHeightStr(m_ccdb->getPageUnits()); }
   CString GetPackageHeight(PageUnitsTag toUnits)    { return m_pkgInfo.GetPackageHeightStr(toUnits); }
   CString GetGenericAttrib(CString name)            { return m_pkgInfo.GetGenericAttrib(name); }
   CString GetFamily()                               { return m_pkgInfo.GetFamily(); }
   CString GetPackageComments()                      { return m_pkgInfo.GetPackageComments(); }

   CDBGenericAttributeMap *AllocDBAttribMap()            { return m_pkgInfo.AllocDBAttribMap(); }
   void LoadDBAttribs(CDataDoctorLibraryAttribMap& configuredAttribMap, CDBGenericAttributeMap *attrMap, bool apply);

   void SetPackageAlias(CString name, bool apply = true); // apply==true apply setting to cc data, false apply only to DDPackage
   void SetDPMO(CString dpmo, bool apply = true);
   bool SetOutlineMethod(CString name, bool filled, bool apply = true);
   void SetPackageHeight(CString height, bool apply = true);
   void SetPackageHeight(CString height, PageUnitsTag fromUnits, bool apply = true);
   void SetGenericAttrib(CString name, CString value, bool apply = true);
   void AddFamilyFromAttribute();
   void SetFamily(CString familyStr);
   void SetPackageComments(CString commentStr);

   CString GetExistingOutlineMethod();
   DataStruct *GetOldStyleDFTOutlineData();

   DataDoctorStatusTag GetStatus(CString &msg);

   void AddInsert(CDataDoctorComponent *ddcomp)    { m_allInsertsOfThisPkg.Add( ddcomp ); }

   void SaveToDB();
   bool LoadFromDB(bool outlineFillFlag, CDataDoctorLibraryAttribMap& configuredAttribMap);

};

class CDataDoctorPackages : public CTypedMapSortStringToObContainer<CDataDoctorPackage>
{
public:
   CDataDoctorPackages(bool isContainer=true, int size=128);

   void AddPkg( CCamCadDatabase& camCadDatabase, CDataDoctorComponent& ddcomponent, CDataDoctorLibraryAttribMap& libraryAttribMap, bool filled );
   CDataDoctorPackage *FindPkg( CString name );
   void GatherPackages(CCamCadDatabase& camCadDatabase, CDataDoctorComponents& ddcomponents, CDataDoctorLibraryAttribMap& libraryAttribMap, bool filled);
   void PurgeOldStyleDFTPackageOutlines();

   CDataDoctorPackage *FindPkgUsingGeomBlk(int compGeomBlockNum);

   // Part set operations
   void storePackageDataToLibrary(CCamCadDatabase& camCadDatabase, bool storeAllPart);
   bool loadPackageDataFromLibrary(CCamCadDatabase& camCadDatabase, bool outlineFillFlag, CDataDoctorLibraryAttribMap& configuredAttribMap, bool applyFlag, bool displayStatusMessage);
   
};



//_____________________________________________________________________________
class CDataDoctor
{
private:
   CCEtoODBDoc& m_camCadDoc;
   CCamCadDatabase m_camCadDatabase;
   BlockStruct* m_pcbBlock;
   CDataDoctorComponents m_components;
   CDataDoctorParts m_parts;
   CDataDoctorPackages m_packages;
   CSubclassList m_subclassList;

   CDataDoctorLibraryAttribMap m_partLibraryAttribMap;
   CDataDoctorLibraryAttribMap m_pkgLibraryAttribMap;

   // For parallel analysis
   CMapStringToString m_mergedNetMap;
   bool performParallelAnalysis(DeviceTypeTag targetDeviceType, bool ignoreUnloadedComponents);
   void identifyMergedNets(CMapStringToString &mergedNetMap, bool ignoreUnloadedComponents);

public:
   CDataDoctor(CCEtoODBDoc& camCadDoc);

   CCamCadDatabase& getCamCadDatabase() { return m_camCadDatabase; }
   BlockStruct* getPcbBlock() { return m_pcbBlock; }
   CDataDoctorComponents& getComponents() { return m_components; }
   CDataDoctorParts& getParts() { return m_parts; }
   CDataDoctorPackages& getPackages()  { return m_packages; }
   CSubclassList &getSubclassList()    { return m_subclassList; }

   CDataDoctorLibraryAttribMap& GetPartLibraryAttribMap()     { return m_partLibraryAttribMap; }
   CDataDoctorLibraryAttribMap& GetPackageLibraryAttribMap()  { return m_pkgLibraryAttribMap; }

   bool initializeData();
   bool doModal(DdPageTag initialPage = DdUnknownPage);
   bool performParallelAnalysis(bool analyzeResistors,bool analyzeCapacitors,bool analyzeInductors,
      bool ignoreUnloadedComponents,bool mergeValues);
   void removeParallelAnalysisResults();
   CString getReportFilePath();
   bool generateParallelAnalysisReport(CWriteFormat& report);
   bool displayParallelAnalysisReport();
   CString getNextNoConnectNetName();


   // This is temporary, for use during development to allow only access to some
   // new stuff only to certain users.
   static bool IsAnointedUser();
};

#endif
