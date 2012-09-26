// $Header: /CAMCAD/DcaLib/DcaApertureShape.cpp 3     3/09/07 5:15p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// TAKAYAFPTWRITE.H

#if ! defined (__TakayaFPTWrite_h__)
#define __TakayaFPTWriter_h__

#pragma once

#include "DcaCamCadData.h"
#include "DcaCompPin.h"
#include "DcaData.h"
#include "DcaBlock.h"
#include "DcaPoint2d.h"
#include "Takaya_o.h"
#include "Element.h"


enum TakayaFPTSurface
{
   surfaceTop = 0,
   surfaceBottom = 1,
   surfaceBoth = 2,
   surfaceUnknown = -1,
};
TakayaFPTSurface intToSurfaceTag(const int surface);

enum TakayaShortType
{
   shortTypePinToPin,
   shortTypeAnalysis,
   shortTypeUnknown,
};
TakayaShortType shortTypeStringToTag(const CString shortTypeString);

enum TakayaPinMapType
{
   pinMapTypeAnode,
   pinMapTypeCathode,
   pinMapTypeBase,
   pinMapTypeCollector,
   pinMapTypeEmitter,
   pinMapTypeGate,
   pinMapTypeSource,
   pinMapTypeDrain,
   pinMapTypeUnknown,
};

enum TakayaNetType
{
   netTypeGround,
   netTypePower,
   netTypeUnknown,
};

TakayaPinMapType pinMapTypeStringToTag(const CString pinMapTypeString);



//_____________________________________________________________________________
class TakayaFPTSettings
{
public:
   TakayaFPTSettings();
   ~TakayaFPTSettings();

private:
   TakayaFPTSurface m_testSurface;
   TakayaFPTSurface m_preferenceSurface;
   TakayaShortType m_shortType;
   bool m_topPopulated;
   bool m_bottomPopulated;
   bool m_polarizedCapVisionTest;
   bool m_tantalumCapVisionTest;
   bool m_allComponentVisionTest;
   bool m_selectVisionTest;
   int m_maxVisionTestCount;
   int m_maxHitCount;
   double m_highFlyZone;
   CString m_PPCommandAttribName;
   double m_kelvinThreshold; // DR 779515 Kelvin threshold value in ohms

   bool m_testLibraryMode; // EXPORE_MORE
   CString m_testLibraryFileName;   // APT_MODEL_FILE_NAME
   CString m_takayaLibraryDirectory;

	CMapStringToString m_pinToPinDeviceTypeMap;
   double m_pinToPinDistance;

   bool m_exportIcCapacitor;
   bool m_exportIcDiode;
   bool m_isIcOpenDescriptionAttrib;
   CString m_icOpenDescription;
   CString m_icDiodePrefix;
   CString m_icCapacitorPrefix;
   CString m_commentAttribute;

   double m_unitFactor;
   double m_capOpensSensorDiameterMM;

   bool m_allowIcDiodeWithIcOpen;

   bool m_exportAllAccessPoints; // DR 640105, Add support to export access poinots that do not have probes.

   TakayaOriginMode m_originModeTag; // for DR 712571

public:
   void reset();

   void             setOriginMode(TakayaOriginMode tag)              { m_originModeTag = tag; }
   TakayaOriginMode getOriginMode()                                  { return m_originModeTag; }

   void setTestSurface(const TakayaFPTSurface surface)               { m_testSurface = surface; }
   TakayaFPTSurface getTestSurface() const                           { return m_testSurface; }

   void setPreferenceSurface(const TakayaFPTSurface surface)         { m_preferenceSurface = surface; }
   TakayaFPTSurface getPreferenceSurface() const                     { return m_preferenceSurface; }

   void setShortType(const TakayaShortType shortType)                { m_shortType = shortType; }
   TakayaShortType getShortType() const                              { return m_shortType; }

   void setTopPopulated(const bool enable)                           { m_topPopulated = enable; }
   bool getTopPopulated() const                                      { return m_topPopulated; }

   void setBottomPopulated(const bool enable)                        { m_bottomPopulated = enable; }
   bool getBottomPopulated() const                                   { return m_bottomPopulated; }

   void setPolarizedCapVisionTest(const bool enable)                 { m_polarizedCapVisionTest = enable; }
   bool getPolarizedCapVisionTest() const                            { return m_polarizedCapVisionTest; }

   void setTantalumCapVisionTest(const bool enable)                  { m_tantalumCapVisionTest = enable; }
   bool getTantalumCapVisionTest() const                             { return m_tantalumCapVisionTest; }

   void setAllComponentVisionTest(const bool enable)                 { m_allComponentVisionTest = enable; }
   bool getAllComponentVisionTest() const                            { return m_allComponentVisionTest; }

   void setSelectVisionTest(const bool enable)                       { m_selectVisionTest = enable; }
   bool getSelectVisionTest() const                                  { return m_selectVisionTest; }

   void setMaxVisionTestCount(const int count)                       { m_maxVisionTestCount = count; }
   int getMaxVisionTestCount() const                                 { return m_maxVisionTestCount; }

   void setMaxHitCount(const int count)                              { m_maxHitCount = count; }
   int getMaxHitCount() const                                        { return m_maxHitCount; }

   void setHighFlyZone(const double height)                             { m_highFlyZone = height; }
   double getHighFlyZone() const                                     { return m_highFlyZone; }

   void setTestLibraryMode(const bool enable)                        { m_testLibraryMode = enable; }
   bool getTestLibraryMode() const                                   { return m_testLibraryMode; }

   void setTestLibraryFileName(const CString fileName)               { m_testLibraryFileName = fileName; }
   CString getTestLibraryFileName() const                            { return m_testLibraryFileName; }
   CString getTestLibraryFullPathName() const;

   void setTakayaLibraryDirectory(const CString directory)           { m_takayaLibraryDirectory = directory; }
   CString getTakayLibraryDirectory() const                          { return m_takayaLibraryDirectory; }

   void setPPCommandAttribName(const CString name)                   { m_PPCommandAttribName = name; }
   CString getPPCommandAttribName() const                            { return m_PPCommandAttribName; }

   void setKelvinThreshold(const double val)                         { m_kelvinThreshold = val; }
   double getKelvinThreshold() const                                 { return m_kelvinThreshold; }

   void setExportAllAccessPoints(const bool enable)                  { m_exportAllAccessPoints = enable; }
   bool getExportAllAccessPoints() const                             { return m_exportAllAccessPoints; }

   void setExportIcCapacitor(const bool enable)                      { m_exportIcCapacitor = enable; }
   bool getExportIcCapacitor() const                                 { return m_exportIcCapacitor; }

   void setExportIcDiode(const bool enable)                          { m_exportIcDiode = enable; }
   bool getExportIcDiode() const                                     { return m_exportIcDiode; }

   void addPinToPinDevice(const CString deviceType);
   bool lookupPinToPinDevice(const DeviceTypeTag deviceType);

   void setPinToPinDistance(const double distance)                   { m_pinToPinDistance = distance; }
   double getPinToPinDistance() const                                { return m_pinToPinDistance; }

   void setIsIcOpenDescriptionAttrib(const bool isAttrib)            { m_isIcOpenDescriptionAttrib = isAttrib; }
   bool isIcOpenDescriptionAttrib() const                            { return m_isIcOpenDescriptionAttrib; }

   void setIcOpenDescription(const CString description)              { m_icOpenDescription = description; }
   CString getIcOpenDescription() const                              { return m_icOpenDescription; }

   void setIcDiodePrefix(const CString prefix)                       { m_icDiodePrefix = prefix; }
   CString getIcDiodePrefix() const                                  { return m_icDiodePrefix; }
   
   void setIcCapacitorPrefix(const CString prefix)                   { m_icCapacitorPrefix = prefix; }
   CString getIcCapacitorPrefix() const                              { return m_icCapacitorPrefix; }

   void setCommentAttribute(const CString attribute)                 { m_commentAttribute = attribute; }
   CString getCommentAttribute() const                               { return m_commentAttribute; }

   void setUnitFactor(const double unitFactor)                       { m_unitFactor = unitFactor; }
   double getUnitFactor() const                                      { return m_unitFactor; }

   void setCapOpensSensorDiameter(const double capOpensSensorDiameter)         { if (capOpensSensorDiameter > 0.0) m_capOpensSensorDiameterMM = capOpensSensorDiameter; } // Input REQUIRED to be in millimeters
   double getCapOpensSensorDiameter(PageUnitsTag desiredUnits) const           { return (m_capOpensSensorDiameterMM * Units_Factor(pageUnitsMilliMeters, desiredUnits)); } // Returns value converted from mm to page units specified in parameter

   void setAllowIcDiodeWithIcOpen(const bool enable)                 { m_allowIcDiodeWithIcOpen = enable; }
   bool getAllowIcDiodeWithIcOpen() const                            { return m_allowIcDiodeWithIcOpen; }
   
};

//_____________________________________________________________________________
class TakayaFPTTestAccess
{
public:
   //TakayaFPTTestAccess(const long entityNumber, const CString name, const CString netName, const CPoint2d xyLocation, const TakayaFPTSurface surfaces);
   TakayaFPTTestAccess(const long accessEntityNumber,const CString name, const CString netName,const CPoint2d xyLocation,
                       const TakayaFPTSurface surface,const bool isOnGroundNet,const bool hasTakayaAccessAttrib,const int targetTypePriority,
                       const long targetEntityNumber);
   ~TakayaFPTTestAccess();

private:
   long m_accessEntityNumber;  // entity number of self
   long m_targetEntityNumber;  // entity number of thing that the access is on
   CString m_name;
   CString m_netName;
   CString m_errorMessage;
   CPoint2d m_xyOrigin;
   TakayaFPTSurface m_surface;
   bool m_isOnIcGroundNet;
   bool m_hasTakayaAccessAttrib;
   bool m_usedForIcCapacitorTest;
   bool m_usedForIcDiodeTest;
   int m_targetTypePriority;
   int m_usedCount;

public:
   CString getName() const                            { return m_name; }
   CString getNetName()const                          { return m_netName; }
   CString getErrorMessage() const                    { return m_errorMessage; }
   CPoint2d getInsertOrigin() const                         { return m_xyOrigin; }
   TakayaFPTSurface getSurface() const                { return m_surface; }
   bool getIsOnIcGroundNet() const                        { return m_isOnIcGroundNet; }
   bool getTakayaAccessAttrib() const                 { return m_hasTakayaAccessAttrib; }
   int getTargetTypePriority()const                   { return m_targetTypePriority; }
   void increaseUsedCount()                           { m_usedCount++; }
   int getUsedCount() const                           { return m_usedCount; }

   void setUsedForIcCapacitorTest(const bool used)    { m_usedForIcCapacitorTest = used; }
   bool getUsedForIcCapacitorTest() const             { return m_usedForIcCapacitorTest; }

   void setUsedForIcDiodeTest(const bool used)        { m_usedForIcDiodeTest = used; }
   bool getUsedForIcDiodeTest() const                 { return m_usedForIcDiodeTest; }
   int getAccessEntityNumber() const                  { return m_accessEntityNumber; }
   int getTargetEntityNumber() const                  { return m_targetEntityNumber; }
};

//_____________________________________________________________________________
class TakayaFPTTestAccessCollection
{
public:
   TakayaFPTTestAccessCollection();
   ~TakayaFPTTestAccessCollection();

private:
   CTypedArrayContainer<CPtrArray,TakayaFPTTestAccess*> m_testAccessWithAttribute;
   CTypedArrayContainer<CPtrArray,TakayaFPTTestAccess*> m_testAccessWithPriority;

   int m_curTestAccessWithAttributeIndex;
   int m_curTestAccessWithPriorityIndex;
   bool m_priorityArrayAlreadySorted;

   TakayaFPTTestAccess* getTestAccess(FILE *logFp, const int maxHits, TakayaFPTTestAccess* testAccessUsed);

   //void sortTestAccessWithPriority();
   //int priorityCompareFunction(const void *arg1, const void* arg2);

   // This supports a weakling iterator. Iteration spans both lists, first
   // m_testAccessWithAttribute and then m_testAccessWithAttribute;
   int m_iteratorIndex;


public:
   TakayaFPTTestAccess* GetFirstTestAccess();  // These two use m_iteratorIndex
   TakayaFPTTestAccess* GetNextTestAccess();   // These two use m_iteratorIndex

   void empty();
   TakayaFPTTestAccess* addTestAccess(const long accessEntityNumber, const CString name, const CString netName, const CPoint2d xyOrigin,
                       const TakayaFPTSurface surface,const bool isOnGroundNet,const bool hasTakayaAccessAttrib,const int targetTypePriority,const long targetEntityNumber);

   TakayaFPTTestAccess* getTestAccess(FILE *logFp, const int maxHits);
   TakayaFPTTestAccess* getTestAccessOnTarget(const long targetEntityNumber);

   void getTwoTestAccess(FILE *logFp, const int maxHits, TakayaFPTTestAccess** testAccess1, TakayaFPTTestAccess** testAccess2);
   void writeLogFile(FILE *logFp);
};

//_____________________________________________________________________________
class TakayaFPTProbeOutput
{
public:
   TakayaFPTProbeOutput(const int probeNumber, const CString netName, const CPoint2d origin, const int testAccessEntityNumber);
   ~TakayaFPTProbeOutput();

private:
   int m_testAccessEntityNumber;
   int m_probeNumber;
   CString m_netName;
   CPoint2d m_xyOrigin;

public:
   int getProbeNumber() const             { return m_probeNumber; }
   CString getNetName() const             { return m_netName; }
   CPoint2d getInsertOrigin() const       { return m_xyOrigin; }
   int getTestAccessEntityNumber() const  { return m_testAccessEntityNumber;}
   DataStruct *getTestAccessData(CCamCadData &camCadData, int accessEntityNumber);
};

//_____________________________________________________________________________
class TakayaFPTProbeOutputCollection
{
public:
   TakayaFPTProbeOutputCollection();
   ~TakayaFPTProbeOutputCollection();

private:
   CTypedPtrArrayContainer<TakayaFPTProbeOutput*> m_probeOutputArray;
   CMapStringToString m_testAccessToProbeNumberMap;
   static int AscendingProbeNumberFunc(const void *a, const void *b);

public:
   CString getProbeNumberString(TakayaFPTTestAccess* testAccess, bool autoGenerate = true);
   CString getProbeNumberString(TakayaFPTTestAccess& testAccess, bool autoGenerate = true);
   int getCount();
   TakayaFPTProbeOutput* getAt(const int index);
   void SortByProbeNumber();
   void RemoveAll();
};

//_____________________________________________________________________________
class TakayaFPTNet
{
public:
   TakayaFPTNet(NetStruct& netStruct);
   ~TakayaFPTNet();

private:
   TakayaFPTTestAccessCollection m_topTestAccessCollection;
   TakayaFPTTestAccessCollection m_bottomTestAccessCollection;
   NetStruct& m_netStruct;
   CString m_name;
   TakayaNetType m_netType;
   bool m_isIcGroundNet;
   bool m_isMaxHitExceeded;
   TakayaFPTSurface m_powerRailTestedSurface;

public:
   CString getName() const                         { return m_name; }
   NetStruct& getNetStruct()                       { return m_netStruct; }
   void setIcGroundNet(const bool isIcGroundNet)   { m_isIcGroundNet = isIcGroundNet; }
   
   void setNetType(const TakayaNetType netType)    { m_netType = netType; }
   TakayaNetType getNetType() const                { return m_netType; }

   void setPowerRailTestedSurface(const TakayaFPTSurface surface)  { m_powerRailTestedSurface = surface; }
   TakayaFPTSurface getPowerRailTestedSurface() const             { return m_powerRailTestedSurface; }

   TakayaFPTTestAccess* addTestAccess(const long accessEntityNumber, const CString name, const CPoint2d xyOrigin,
                       const TakayaFPTSurface surface,const bool hasTakayaAccessAttrib,const int targetTypePriority, const long targetEntityNumber);

   TakayaFPTTestAccess* getTestAccess(FILE *logFp, const int maxHits, const TakayaFPTSurface testSurface);
   TakayaFPTTestAccess* getTestAccessOnTarget(const long targetEntityNumber, const TakayaFPTSurface surface);

   void getTwoTestAccess(FILE *logFp, const int maxHits, const TakayaFPTSurface testSurface, TakayaFPTTestAccess** testAccess1, TakayaFPTTestAccess** testAccess2);
   void writeLogFile(FILE *logFp);

   TakayaFPTTestAccessCollection &getTestAccessCollection(const TakayaFPTSurface testSurface) { return (testSurface == testSurfaceTop) ? (m_topTestAccessCollection) : (m_bottomTestAccessCollection); }
};

//_____________________________________________________________________________
class TakayaFPTNetMap
{
public:
   TakayaFPTNetMap();
   ~TakayaFPTNetMap();

private:
   CTypedMapStringToPtrContainer<TakayaFPTNet*> m_netNameMap;
   CMapStringToString m_powerGroundNameMap;
   int m_powerNetCount;
   int m_groundNetCount;
   
   CString getPowerGroundName(TakayaFPTNet& net);

public:
   void empty();
   TakayaFPTNet* addNet(NetStruct& net);
   TakayaFPTNet* findNet(const CString netName);
   void generatePowerRailShortTest(FILE *logFp, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface);
   void writeLogFile(FILE *logFp);

   POSITION GetStartPosition()                                             { return m_netNameMap.GetStartPosition(); }
   void GetNextAssoc(POSITION &pos, CString &key, TakayaFPTNet*& fptNet)   { m_netNameMap.GetNextAssoc(pos, key, fptNet); }
};

//_____________________________________________________________________________
class TakayaFPTCompPin : public CObject
{
public:
   TakayaFPTCompPin(CompPinStruct& compPinStruct, TakayaFPTNet& net);
   ~TakayaFPTCompPin();

private:
   //CPoint2d m_xyOrigin;
   //bool m_isMirrored;
   CompPinStruct& m_compPinStruct;
   TakayaFPTNet& m_net;
   CString m_pinMap;
   bool m_alreadyTested;
   TakayaFPTSurface m_testedSurface;
   TakayaFPTTestAccess* m_topTestAccess;
   TakayaFPTTestAccess* m_bottomTestAccess;
   
public:
   CString getPinName() const                { return m_compPinStruct.getPinName(); }
   CString getCompName() const               { return m_compPinStruct.getRefDes(); }   
   CString getCompPinName() const            { return m_compPinStruct.getPinRef(); }
   CString getNetName() const                { return m_net.getName(); }
   TakayaFPTNet& getNet()                    { return m_net; }
   CPoint2d getInsertOrigin() const          { return m_compPinStruct.getOrigin(); }
   int   getEntityNumber() const             { return m_compPinStruct.getEntityNumber(); }

   void setPinMap(const CString pinMap)      { m_pinMap = pinMap; }
   CString getPinMap() const                 { return m_pinMap; }

   void setAlreadyTested(const bool tested)  { m_alreadyTested = tested; }
   bool getAlreadyTested() const             { return m_alreadyTested; }

   void setTestedSurface(const TakayaFPTSurface testedSurface) { m_testedSurface = testedSurface; }
   TakayaFPTSurface getTestedSurface() const { return m_testedSurface; }

   TakayaFPTTestAccess* getTestAccess(FILE *logFp, const int maxHits, const TakayaFPTSurface testSurface); // same net, any target
   TakayaFPTTestAccess* getTestAccessOnSelf(const TakayaFPTSurface testSurface); // only when target is self, otherwise NULL
};

//_____________________________________________________________________________
class TakayaFPTCompPinMap
{
public:
   TakayaFPTCompPinMap(bool isContainer);
   ~TakayaFPTCompPinMap();

private:
   CTypedMapSortStringToObContainer<TakayaFPTCompPin> m_compPinMap;
   bool m_alreadySorted;
      
   //CTypedMapStringToPtrContainer<TakayaFPTCompPin*> m_compPinMap;

public:
   void empty();
   void addCompPin(TakayaFPTCompPin* compPin);
   TakayaFPTCompPin* addCompPin(CompPinStruct& compPinStruct, TakayaFPTNet& net);
   TakayaFPTCompPin* findCompPin(CString compPinName);
   POSITION getStartPosition() const;
   TakayaFPTCompPin* getNext(POSITION& pos);

   void sort();
   TakayaFPTCompPin* getFirstSorted();
   TakayaFPTCompPin* getNextSorted();
};

//_____________________________________________________________________________
class TakayaFPTComponent
{
public:
   TakayaFPTComponent(FileStruct& file, InsertStruct& insert);
   ~TakayaFPTComponent();

private:
   CString m_name;
   CString m_takayaName;
   CString m_commentOperator; // If this set to "//", then the test will be commented out in the output.
   CString m_value;
   CString m_comment;
   CString m_element;
   CString m_gridLocation;
   CString m_subClass;
   CString m_positiveTol;
   CString m_negativeTol;
   CString m_partNumber;
   CString m_mergedStatus;    
   CString m_mergedValue;     
   CString m_aptModelName;
   CString m_testStrategy;
   CString m_icOpenDescription;
   CString m_ppCommand;
   DeviceTypeTag m_deviceType;
   CPoint2d m_xyOrigin;
   TakayaFPTSurface m_surface;
   bool m_isIcOpenTest;
   bool m_ipOpenTestDone;
   bool m_isPinShortTestDone;
   bool m_isLoaded;
   bool m_isSmd;
   int m_pinCount;
   double m_compHeightPageUnits;  // COMPHEIGHT attrib val in ccz page units
   double m_compHeightMM;         // COMPHEIGHT attrib val in millimeters
   bool m_writeSelectVisionTest;
   InsertStruct& m_insert;
   FileStruct& m_file;  // the file whose datalist has insert of this component
   TakayaFPTCompPinMap m_compPinMap; //  not a container

   void initalize();
   void updateFromAttributes();

public:
   CString getName() const                      { return m_name; }
   CString getTakayaName() const                { return m_takayaName; }
   CString getCommentOperator() const           { return m_commentOperator; }
   CString getElement() const                   { return m_element; }
   CString getLocation() const                  { return m_gridLocation; }
   CString getSubClass() const                  { return m_subClass; }
   CString getPositiveTol() const               { return m_positiveTol; }
   CString getNegativeTol() const               { return m_negativeTol; }
   CString getPartNumber() const                { return m_partNumber; }
   CString getMergedStatus() const              { return m_mergedStatus; }
   CString getMergedValue() const               { return m_mergedValue; }
   CString getAptModelName() const              { return m_aptModelName; }
   CString getTestStrategy() const              { return m_testStrategy; }
   CString getIcOpenDescription() const         { return m_icOpenDescription; }
   CString getPPCommand() const                 { return m_ppCommand; }
   DeviceTypeTag getDeviceType() const          { return m_deviceType; }
   void unsetDeviceType()                       { m_deviceType = deviceTypeUnknown; }

   TakayaFPTSurface getSurface() const          { return m_surface; }
   bool isIcOpenTest() const                    { return m_isIcOpenTest; }
   bool isLoaded() const                        { return m_isLoaded; }
   bool isSmd() const                           { return m_isSmd; }
   int getPinCount() const                      { return m_pinCount; }
   double getCompHeight() const                 { return m_compHeightPageUnits; }
   double getCompHeightMM() const               { return m_compHeightMM; }
   InsertStruct& getInsert()                    { return m_insert; }
   FileStruct& getFile()                        { return m_file; }

   void addCompPin(TakayaFPTCompPin* compPin);
   TakayaFPTCompPin* findCompPin(const CString pinName);
   TakayaFPTCompPin* getFirstSorted()           { return m_compPinMap.getFirstSorted(); }
   TakayaFPTCompPin* getNextSorted()            { return m_compPinMap.getNextSorted(); }

   void setIcOpenTestDone(const bool done)      { m_ipOpenTestDone = done; }
   bool getIcOpenTestDone() const               { return m_ipOpenTestDone; }   

   void SetWriteSelectVisionTest(const bool write) { m_writeSelectVisionTest = write; }
   bool GetWriteSelectVisionTest() const { return m_writeSelectVisionTest; }

   void setPinShortTestDone(const bool done)    { m_isPinShortTestDone =  done; }
   bool getPinShortTestDone() const             { return m_isPinShortTestDone; }

   void setValue(const CString value)           { m_value = value; }
   CString getValue() const                     { return m_value; }

   void setComment(const CString comment)       { m_comment = comment; }
   CString getComment() const                   { return m_comment; }

   void setInsertOrigin(const CPoint2d origin)        { m_xyOrigin = origin; }
   CPoint2d getInsertOrigin() const                   { return m_xyOrigin; }


   void updateProperty(CCEtoODBDoc& doc, CCamCadData& camCadData, CAttributes* attributes, TakayaFPTSettings &takayaSettings);
   int generateTestPinCount(FILE *logFp, const int maxHits, TakayaFPTSurface testSurface);
};

//_____________________________________________________________________________
class TakayaIcDiodeCommand
{
public:
   TakayaIcDiodeCommand(const CString partNumber, const CString pinName, const bool swap)
   {
      m_partNumber = partNumber;
      m_pinName = pinName;
      m_swap = swap;
   }
   ~TakayaIcDiodeCommand(){};

private:
   CString m_partNumber;
   CString m_pinName;
   bool m_swap;

public:
   CString getPartNumber() const { return m_partNumber; }
   CString getPinName() const { return m_pinName; }
   bool isSwap() const { return m_swap; }
};

//_____________________________________________________________________________
class TakayaIcDiodeCommandMap
{
public:
   TakayaIcDiodeCommandMap();
   ~TakayaIcDiodeCommandMap();

private:
   CTypedMapStringToPtrContainer<TakayaIcDiodeCommand*> m_icDiodeCommandMap;

public:
   void addCommand(const CString partNumber, const CString pinName, const bool swap);
   TakayaIcDiodeCommand* getGroundPin(const CString partNumber);

   void empty() { m_icDiodeCommandMap.empty(); }
};

//_____________________________________________________________________________
class TakayaFPTAptTest
{
public:
	TakayaFPTAptTest() { m_cElement = '\0';};
	~TakayaFPTAptTest() {};

private:
   int m_sDeviceType;
	CString m_sRefdes;
	CString m_sValue;
	CString m_sComment;
	CString m_sLocation;
	CString m_sOption;
	CString m_cElement;
	CString m_sPinNum1;
	CString m_sPinNum2;
   CString m_sPinNum3;
   CString m_sPinNum4;

public:
   void SetDeviceType(const int deviceType)  { m_sDeviceType = deviceType; }
	void SetRefdes(const CString refdes)		{ m_sRefdes = refdes;		}
	void SetValue(const CString value)			{ m_sValue = value;			}
	void SetCommment(const CString comment)	{ m_sComment = comment;		}
	void SetLocation(const CString location)	{ m_sLocation = location;	}
	void SetOption(const CString option)		{ m_sOption = option;		}
	void SetElement(const CString element)			{ m_cElement = element;		}
	void SetPinNum1(const CString pinNum1)		{ m_sPinNum1 = pinNum1;		}
	void SetPinNum2(const CString pinNum2)		{ m_sPinNum2 = pinNum2;		}
	void SetPinNum3(const CString pinNum3)		{ m_sPinNum3 = pinNum3;		}
	void SetPinNum4(const CString pinNum4)		{ m_sPinNum4 = pinNum4;		}

   int GetDeviceType() const                 { return m_sDeviceType;    }
	CString GetRefdes() const						{ return m_sRefdes;			}
	CString GetValue() const						{ return m_sValue;			}
	CString GetComment() const						{ return m_sComment;			}
	CString GetLocation() const					{ return m_sLocation;		}
	CString GetOption() const						{ return m_sOption;			}
	CString GetElement() const		   			{ return m_cElement;			}
	CString GetPinNum1() const						{ return m_sPinNum1;			}
	CString GetPinNum2() const						{ return m_sPinNum2;			}
	CString GetPinNum3() const						{ return m_sPinNum3;			}
	CString GetPinNum4() const						{ return m_sPinNum4;			}
};

//_____________________________________________________________________________
class TakayaFPTAptModel 
{
public:
	TakayaFPTAptModel(const CString name);
	~TakayaFPTAptModel();

private:
	CString m_sName;
   CTypedPtrListContainer<TakayaFPTAptTest*> m_aptTestList;

public:
   TakayaFPTAptTest* createTest();
   bool generateAptModelTest(FILE *logFp, TakayaFPTComponent& component, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface);
};

//_____________________________________________________________________________
class TakayaFPTAptModelMap
{
public:
   TakayaFPTAptModelMap();
	~TakayaFPTAptModelMap();

private:
   CTypedMapStringToPtrContainer<TakayaFPTAptModel*> m_aptModelMap;

public:
   void empty();
   TakayaFPTAptModel* add(const CString aptModelName);
   TakayaFPTAptModel* lookup(const CString aptModelName);
};

//_____________________________________________________________________________
class TakayaFPTPinToPinShort : public CObject
{
public:
   TakayaFPTPinToPinShort(const CString pinName1, const CString pinName2)
   {
      m_pinName1 = pinName1;
      m_pinName2 = pinName2;
   }
   ~TakayaFPTPinToPinShort()
   {
   }

private:
   CString m_pinName1;
   CString m_pinName2;

public:
   CString getPinName1() const { return m_pinName1; }
   CString getPinName2() const { return m_pinName2; }
};

//_____________________________________________________________________________
class TakayaFPTPinToPinAnalyzer
{
public:
   TakayaFPTPinToPinAnalyzer();
   ~TakayaFPTPinToPinAnalyzer();

private:
   CTypedMapSortStringToObContainer<TakayaFPTPinToPinShort> m_pinToPinMap;

public:
   void analysisPinToPinShort(BlockStruct& block, const double distance);
   TakayaFPTPinToPinShort* getFirstSorted();
   TakayaFPTPinToPinShort* getNextSorted();
};

//_____________________________________________________________________________
class TakayaFPTPinToPinAnalyzerMap
{
public:
   TakayaFPTPinToPinAnalyzerMap();
   ~TakayaFPTPinToPinAnalyzerMap();

private:
	CTypedMapIntToPtrContainer<TakayaFPTPinToPinAnalyzer*> m_pcbComponentPinToPinAnalyzerMap;

public:
   TakayaFPTPinToPinAnalyzer* getPinToPinAnalyzer(BlockStruct& block, const double distance);
};

//_____________________________________________________________________________
class TakayaFPTPackageOutlineExtentMap
{
public:
   TakayaFPTPackageOutlineExtentMap();
   ~TakayaFPTPackageOutlineExtentMap();

private:
   CTypedMapIntToPtrContainer<CExtent*> m_blockPackageExtentMap;
   CExtent* getPackageOutlineExtent(CCamCadData& camCadData, BlockStruct& block, bool realPart);
   BlockStruct* GetRealPartPackageBlockForComponent(CCamCadData& camCadData, TakayaFPTComponent* component);

public:
   CExtent* getPackageOutlineExtent(CCamCadData& camCadData, TakayaFPTComponent* component);

};

//_____________________________________________________________________________
class TakayaFPTComponentCollection
{
public:
   TakayaFPTComponentCollection(CGTabTableList *tableList);
   ~TakayaFPTComponentCollection();
    static int icdiode;
private:
   CTypedMapStringToPtrContainer<TakayaFPTComponent*> m_componentMapByName;
   CTypedPtrListContainer<TakayaFPTComponent*>m_resistorComponents;
   CTypedPtrListContainer<TakayaFPTComponent*>m_capacitorComponents;
   CTypedPtrListContainer<TakayaFPTComponent*>m_inductorComponents;
   CTypedPtrListContainer<TakayaFPTComponent*>m_icComponents;
   CTypedPtrListContainer<TakayaFPTComponent*>m_diodeComponents;
   CTypedPtrListContainer<TakayaFPTComponent*>m_transitorComponents;
   CTypedPtrListContainer<TakayaFPTComponent*>m_capacitorPolarizedComponents;
   CTypedPtrListContainer<TakayaFPTComponent*>m_capacitorTantalumComponents;
   CTypedPtrListContainer<TakayaFPTComponent*>m_otherComponents;
   CTypedPtrListContainer<TakayaFPTComponent*>m_testPoints;  // Output to repair file only, no tests generated, not really a component

   ICOpenTestGeomMap m_icOpenTestGeomMap;
   TakayaFPTPinToPinAnalyzerMap m_pinToPinAnalyzerMap;
   CMapStringToString m_pinToPinProbePairMap;
   TakayaFPTPackageOutlineExtentMap m_packageOutlineExtentMap;

   CPoint2d m_topOutputOrigin;
   CPoint2d m_botOutputOrigin;

   int m_visionTestCount;
   CSubclassList m_subclassList;

   void applyTakayaOrigin(CPoint2d &cczInsertOrigin, bool topside);
   
   void generateResistorTest(FILE *logFp, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface);
   void generateCapacitorTest(FILE *logFp, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface);
   void generateDiodeTest(FILE *logFp, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface);
   void generateInductorTest(FILE *logFp, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface);
   void generateIcTest(CCamCadData& camcadData, FILE *logFp, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTNet& groundNet, TakayaIcDiodeCommandMap& icDiodeCommandMap, 
      const bool isIcDiodeTest, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface);
   void generateTransistorTest(FILE *logFp, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface);
   void generateCapacitorPolarizedTest(CCamCadData& camcadData, FILE *logFp, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTNet* groundNet, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface);
   void generateCapacitorTantalumTest(CCamCadData& camcadData, FILE *logFp, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTNet* groundNet, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface);
   void generateIcOpenTest(FILE *logFp, CCamCadData& camcadData, TakayaFPTNet& groundNet, TakayaFPTComponent& component, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface);

   void generateUnsupportedDeviceTypeTest(FILE *logFp, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface);

   bool generateKelvinTest(FILE *logFp, TakayaFPTComponent& component, TakayaFPTProbeOutputCollection& probeOutputCollection,
      CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface);

   void generateGenericTest(FILE *logFp, TakayaFPTComponent& component, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection,
      CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface);

   bool generateOutput(FILE *logFp, TakayaFPTComponent& component, TakayaFPTCompPin* compPin1, TakayaFPTCompPin* compPin2,
      TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface);

   bool isValidDeviceTypeForMDBUse(const DeviceTypeTag deviceType);
   bool isValidTwoPinDeviceType(const DeviceTypeTag deviceType);
   bool isValidThreePinDeviceType(const DeviceTypeTag deviceType);
   bool isValidFourPinDeviceType(const DeviceTypeTag deviceType);
   void generateOutputUsingMDB(FILE *logFp, TakayaFPTComponent& component, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
      TakayaFPTSettings& settings, const TakayaFPTSurface testSurface, CSubclass *sc);
   void generateTwoPinOutputUsingMDB(FILE *logFp, TakayaFPTComponent& component,
     TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
     TakayaFPTSettings& settings, const TakayaFPTSurface testSurface, CSubclassElement *el);
   void generateThreePinOutputUsingMDB(FILE *logFp, TakayaFPTComponent& component,
     TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
     TakayaFPTSettings& settings, const TakayaFPTSurface testSurface, CSubclassElement *el);
   void generateFourPinOutputUsingMDB(FILE *logFp, TakayaFPTComponent& component,
     TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
     TakayaFPTSettings& settings, const TakayaFPTSurface testSurface, CSubclassElement *el);

   bool generateVisionOutput(FILE *logFp, TakayaFPTComponent& component, 
      CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface);

   void generatePinToPinShortTest(CCamCadData& camcadData, FILE *logFp, CTypedPtrListContainer<TakayaFPTComponent*>& components, TakayaFPTProbeOutputCollection& probeOutputCollection,
      CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface); 

   void generateHighFlyZoneTest(FileStruct& file, CCamCadData& camcadData, FILE *logFp, CTypedPtrListContainer<TakayaFPTComponent*>& components, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface);
   
   void writeRepairFileSection(FileStruct& file, CCamCadData& camcadData, CStdioFileWriteFormat& repairFile, const CTMatrix pcbMatrix, 
      CTypedPtrListContainer<TakayaFPTComponent*>& components, 
      TakayaFPTProbeOutputCollection& topProbeOutputCollection, TakayaFPTProbeOutputCollection& bottomProbeOutputCollection, 
      TakayaFPTSettings& settings, const double pageUnitFactor, const double takayaUnitFactor);

   void writeRepairFileVias(FileStruct& file, CCamCadData& camcadData, CStdioFileWriteFormat& repairFile, const CTMatrix pcbMatrix,
      TakayaFPTNetMap& netMap, TakayaFPTProbeOutputCollection& topProbeOutputCollection, TakayaFPTProbeOutputCollection& bottomProbeOutputCollection, 
      TakayaFPTSettings& settings, const double pageUnitFactor, const double takayaUnitFactor);

   void writeRepairFileCompRecord(CStdioFileWriteFormat& repairFile, TakayaFPTComponent *takayaComponent, TakayaFPTCompPin *takayaCompPin, CPoint2d centroid, CPoint2d pinOrigin, CPoint2d pinOriginWithOffset, CExtent *packageExtent, CString topProbeName, CPoint2d topProbeOrigin, CString botProbeName, CPoint2d botProbeOrigin);
   
   void writeRepairFileRecord(CStdioFileWriteFormat& repairFile,
      CString compName, CString pinName, CString netName, TakayaFPTSurface surface,
      CString partnumber, bool isLoaded, double compHeight, CString ppCommand,
      CPoint2d centroid, double rotationDegrees, CString smdThru, CString value,
      CExtent *packageExtent, CPoint2d pinOrigin,  CPoint2d pinOriginWithOffset,
      CString topProbeName, CPoint2d topProbeOrigin, CString botProbeName, CPoint2d botProbeOrigin);

   void writeRepairFileHeader(CStdioFileWriteFormat& repairFile);

   BlockStruct* GetRealPartPackageBlockForComponent(CCamCadData& camCadData,TakayaFPTComponent* component);
   DataStruct *FindAccessDataForEntityNum(CCamCadData& camCadData,  BlockStruct *pcbFileBlock, long entityNumber);
   void getAccessMarkerXYOffset(CCamCadData& camCadData, BlockStruct *pcbFileBlock, int entityNumber, CPoint2d &pinOriginWithOffset);

public:
   void empty();
   void moveIcDeviceTypeToOtherComponent();

   void SetTakayaOrigin(CPoint2d topOrigin, CPoint2d botOrigin)   { m_topOutputOrigin = topOrigin; m_botOutputOrigin = botOrigin; }

   void convertToTakayaUnits(CPoint2d& location, double unitFactor, bool topside);

   TakayaFPTComponent* addComponent(CCEtoODBDoc& doc, CCamCadData& camCadData, FileStruct& file, DataStruct& data, TakayaFPTSettings& settings, CTMatrix transformMatrix);
   TakayaFPTComponent* findComponent(const CString name);

   void generateComponentTest(CCamCadData& camcadData, FILE *logFp, TakayaFPTAptModelMap& adptModelMap, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
      TakayaFPTNet* groundNet, TakayaIcDiodeCommandMap& icDiodeCommandMap, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface);

   void generateComponentHighFlyZoneTest(FileStruct& file, CCamCadData& camcadData, FILE *logFp, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, TakayaFPTSettings& settings, const TakayaFPTSurface testSurface);

   void generateComponentRepairFile(FileStruct& file, CCamCadData& camcadData, CStdioFileWriteFormat& repairFile, const CTMatrix pcbMatrix, 
      TakayaFPTNetMap& netMap, TakayaFPTProbeOutputCollection& topProbeOutputCollection, TakayaFPTProbeOutputCollection& bottomProbeOutputCollection, 
      TakayaFPTSettings& settings, const double pageUnitFactor, const double takayaUnitFactor);

   void writeLogFile(FILE *logFp);
};

//_____________________________________________________________________________
class TakayaFPTPcbFile
{
public:
   TakayaFPTPcbFile(FileStruct& fileStruct);
   ~TakayaFPTPcbFile();

private:
   FileStruct& m_fileStruct;
   CString m_name;
   int m_blockNumber;
   CTMatrix m_matrix;
   CPoint2d m_xyOrigin;
   double m_rotationRadian;   
   bool m_isMirrored;

public:
   FileStruct& getFileStruct() const            { return m_fileStruct; }

   void setName(const CString name)             { m_name = name; }
   CString getName() const                      { return m_name; }
   
   void setBlockNumber(const int number)        { m_blockNumber = number; }
   int getBlockNubmer() const                   { return m_blockNumber; }

   void setMatrix(const CTMatrix matrix)        { m_matrix = matrix; }
   CTMatrix getMatrix() const                   { return m_matrix; }

   void setInsertOrigin(const CPoint2d origin)  { m_xyOrigin = origin; }
   CPoint2d getInsertOrigin() const             { return m_xyOrigin; }

   void setRotationRadian(const double radian)  { m_rotationRadian = radian; }
   double getRotationRadian() const             { return m_rotationRadian; }

   void setMirrored(const bool isMirrored)      { m_isMirrored = isMirrored; }
   bool getMirrored() const                     { return m_isMirrored; }
};

//_____________________________________________________________________________
class TakayaFPTWrite
{
public:
   TakayaFPTWrite();
  
   TakayaFPTWrite(CCEtoODBDoc& m_camCadDoc, const CString pathName);
   ~TakayaFPTWrite();
    
private:
   CCEtoODBDoc& m_camCadDoc;
   CCamCadData& m_camCadData;
   TakayaFPTSettings m_settings;
   double m_unitFactor;
   bool m_hasGroundNet;

   CPoint2d m_topOutputOrigin;
   CPoint2d m_botOutputOrigin;
   
   CTypedListContainer<CPtrList,TakayaFPTPcbFile*> m_pcbDesignList;
   TakayaFPTAptModelMap m_aptModelMap;
   TakayaIcDiodeCommandMap m_icDiodeCommandMap;
   TakayaFPTNetMap m_netMap;
   TakayaFPTCompPinMap m_compPinMap;
   TakayaFPTComponentCollection m_componentCollection;
   CStringArray m_topOutputArray;
   CStringArray m_bottomOutputArray;

   TakayaFPTNet* m_groundNet;   
   CProgressDlg* m_progressDlg;

   FILE* m_logFp;
   CString m_localLogFilename;

   CStdioFileWriteFormat* m_topOutputFile;
   CStdioFileWriteFormat* m_bottomOutputFile;
   CMapStringToString m_probablyShortNetPairMap;

   CString m_pathName;
	CString m_topBrdRefPntCommand; 
	CString m_topAuxRefPntCommand;
	CString m_botBrdRefPntCommand;
	CString m_botAuxRefPntCommand;

   void empty();
   CProgressDlg& getProgressDlg();

   FILE* getLogFile();
   void closeLogFile();

   CStdioFileWriteFormat& getTopFile();
   void closeTopFile();

   CStdioFileWriteFormat& getBottomFile();
   void closeBottomFile();

   bool getPcbDesignList();
   void loadSettings();
   void loadModelTest();
   bool loadTakayaData(FileStruct& pcbFile);

   void writeTakayaOutput(const CString fileName, CStringArray& outputArray, TakayaFPTProbeOutputCollection& probeOutputCollection, const TakayaFPTSurface testSurface);
   void generateProbablyShortTest(FileStruct& fileStruct, FILE *logFp, TakayaFPTProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, const TakayaFPTSurface testSurface);
   CString getCompPinRefForProbableShort(NetStruct& netStruct);
   void updateProbeInCCZ(TakayaFPTProbeOutput* probeOutput);
   void writeProbeAccessPoints(CStdioFileWriteFormat* file, TakayaFPTProbeOutputCollection& probeOutputCollection, const TakayaFPTSurface testSurface);
   int createProbesForUnusedAccessMarks(const TakayaFPTSurface testSurface);
   
public:
   void write();
   bool diode;

};

#endif