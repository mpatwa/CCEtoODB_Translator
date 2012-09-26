/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2011. All Rights Reserved.
*/

// HiokiWRITE.H

#if ! defined (__Hioki_Out_h__)
#define __Hioki_Out_h__

#pragma once

#include "DcaCamCadData.h"
#include "DcaCompPin.h"
#include "DcaData.h"
#include "DcaBlock.h"
#include "DcaPoint2d.h"
#include "Takaya_o.h"
#include "Element.h"

#define HIOKI_TEST_NONE       ' '
#define HIOKI_TEST_RESISTOR   'R'
#define HIOKI_TEST_INDUCTOR   'L'
#define HIOKI_TEST_CAPACITOR  'C'
#define HIOKI_TEST_ZENER      'Z'
#define HIOKI_TEST_TRANSISTOR 'Q'
#define HIOKI_TEST_DIODE      'D'
#define HIOKI_TEST_VOLTAGE    'V'
#define HIOKI_TEST_OPEN       'O'
#define HIOKI_TEST_SHORT      'S'
#define HIOKI_TEST_DISCHARGE  'd'
#define HIOKI_TEST_VISION     'v'
#define HIOKI_TEST_4LEADRES   'r'

static char GetHiokiTestCode(DeviceTypeTag devtype);

#define DEFAULT_GRID_LOCATION " "  // Blank
#define DEFAULT_COMP_VALUE    " "  // Blank


enum HiokiSurface
{
   surfaceTop     =  0,
   surfaceBottom  =  1,
   surfaceBoth    =  2,
   surfaceUnknown = -1,
};
HiokiSurface intToSurfaceTag(const int surface);

enum HiokiShortType
{
   shortTypePinToPin,
   shortTypeAnalysis,
   shortTypeUnknown,
};

HiokiShortType shortTypeStringToTag(const CString shortTypeString);

enum HiokiPinMapType
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

HiokiPinMapType pinMapTypeStringToTag(const CString pinMapTypeString);



//_____________________________________________________________________________
class HiokiSettings
{
public:
   HiokiSettings();
   ~HiokiSettings();

private:
   HiokiSurface m_testSurface;
   HiokiSurface m_preferenceSurface;
   HiokiShortType m_shortType;
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

   bool m_annotateOutput; // Turns on/off extra comments in output, is a debug mode but I didn't want debug as keyword in settings file.

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

   TakayaOriginMode m_originModeTag; // for DR 712571

public:
   void reset();

   void             setOriginMode(TakayaOriginMode tag)              { m_originModeTag = tag; }
   TakayaOriginMode getOriginMode()                                  { return m_originModeTag; }

   void setAnnotated(bool annotate)                               { m_annotateOutput = annotate; }
   bool getAnnotated()                                            { return m_annotateOutput; }

   void setTestSurface(const HiokiSurface surface)               { m_testSurface = surface; }
   HiokiSurface getTestSurface() const                           { return m_testSurface; }

   void setPreferenceSurface(const HiokiSurface surface)         { m_preferenceSurface = surface; }
   HiokiSurface getPreferenceSurface() const                     { return m_preferenceSurface; }

   void setShortType(const HiokiShortType shortType)                { m_shortType = shortType; }
   HiokiShortType getShortType() const                              { return m_shortType; }

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
class HiokiTestAccess
{
public:
   //HiokiTestAccess(const long entityNumber, const CString name, const CString netName, const CPoint2d xyLocation, const HiokiSurface surfaces);
   HiokiTestAccess(const long accessEntityNumber,const CString name, const CString netName,const CPoint2d xyLocation,
                       const HiokiSurface surface,const bool isOnGroundNet,const bool hasTakayaAccessAttrib,const int targetTypePriority,
                       const long targetEntityNumber);
   ~HiokiTestAccess();

private:
   long m_accessEntityNumber;  // entity number of self
   long m_targetEntityNumber;  // entity number of thing that the access is on
   CString m_name;
   CString m_netName;
   CString m_errorMessage;
   CPoint2d m_xyOrigin;
   HiokiSurface m_surface;
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
   HiokiSurface getSurface() const                { return m_surface; }
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
class HiokiTestAccessCollection
{
public:
   HiokiTestAccessCollection();
   ~HiokiTestAccessCollection();

private:
   CTypedArrayContainer<CPtrArray,HiokiTestAccess*> m_testAccessWithAttribute;
   CTypedArrayContainer<CPtrArray,HiokiTestAccess*> m_testAccessWithPriority;

   int m_curTestAccessWithAttributeIndex;
   int m_curTestAccessWithPriorityIndex;
   bool m_priorityArrayAlreadySorted;

   HiokiTestAccess* getTestAccess(FILE *logFp, const int maxHits, HiokiTestAccess* testAccessUsed);

   //void sortTestAccessWithPriority();
   //int priorityCompareFunction(const void *arg1, const void* arg2);

public:
   void empty();
   HiokiTestAccess* addTestAccess(const long accessEntityNumber, const CString name, const CString netName, const CPoint2d xyOrigin,
                       const HiokiSurface surface,const bool isOnGroundNet,const bool hasTakayaAccessAttrib,const int targetTypePriority,const long targetEntityNumber);

   HiokiTestAccess* getTestAccess(FILE *logFp, const int maxHits);
   HiokiTestAccess* getTestAccessOnTarget(const long targetEntityNumber);

   void getTwoTestAccess(FILE *logFp, const int maxHits, HiokiTestAccess** testAccess1, HiokiTestAccess** testAccess2);
   void writeLogFile(FILE *logFp);
};

//_____________________________________________________________________________
class HiokiProbeOutput
{
public:
   HiokiProbeOutput(const int probeNumber, const CString netName, const CPoint2d origin, const int testAccessEntityNumber);
   ~HiokiProbeOutput();

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
};

//_____________________________________________________________________________
class HiokiProbeOutputCollection
{
public:
   HiokiProbeOutputCollection();
   ~HiokiProbeOutputCollection();

private:
   CTypedPtrArrayContainer<HiokiProbeOutput*> m_probeOutputArray;
   CMapStringToString m_testAccessToProbeNumberMap;
   static int AscendingProbeNumberFunc(const void *a, const void *b);

public:
   CString getProbeNumberString(HiokiTestAccess* testAccess, bool autoGenerate = true);
   CString getProbeNumberString(HiokiTestAccess& testAccess, bool autoGenerate = true);
   int getCount();
   HiokiProbeOutput* getAt(const int index);
   void SortByProbeNumber();
};

//_____________________________________________________________________________
class HiokiNet
{
public:
   HiokiNet(NetStruct& netStruct);
   ~HiokiNet();

private:
   HiokiTestAccessCollection m_topTestAccessCollection;
   HiokiTestAccessCollection m_bottomTestAccessCollection;
   NetStruct& m_netStruct;
   CString m_name;
   TakayaNetType m_netType;
   bool m_isIcGroundNet;
   bool m_isMaxHitExceeded;
   HiokiSurface m_powerRailTestedSurface;

public:
   CString getName() const                         { return m_name; }
   NetStruct& getNetStruct()                       { return m_netStruct; }
   void setIcGroundNet(const bool isIcGroundNet)   { m_isIcGroundNet = isIcGroundNet; }
   
   void setNetType(const TakayaNetType netType)    { m_netType = netType; }
   TakayaNetType getNetType() const                { return m_netType; }

   void setPowerRailTestedSurface(const HiokiSurface surface)  { m_powerRailTestedSurface = surface; }
   HiokiSurface getPowerRailTestedSurface() const             { return m_powerRailTestedSurface; }

   HiokiTestAccess* addTestAccess(const long accessEntityNumber, const CString name, const CPoint2d xyOrigin,
                       const HiokiSurface surface,const bool hasTakayaAccessAttrib,const int targetTypePriority, const long targetEntityNumber);

   HiokiTestAccess* getTestAccess(FILE *logFp, const int maxHits, const HiokiSurface testSurface);
   HiokiTestAccess* getTestAccessOnTarget(const long targetEntityNumber, const HiokiSurface surface);

   void getTwoTestAccess(FILE *logFp, const int maxHits, const HiokiSurface testSurface, HiokiTestAccess** testAccess1, HiokiTestAccess** testAccess2);
   void writeLogFile(FILE *logFp);
};

//_____________________________________________________________________________
class HiokiNetMap
{
public:
   HiokiNetMap();
   ~HiokiNetMap();

private:
   CTypedMapStringToPtrContainer<HiokiNet*> m_netMap;
   CMapStringToString m_powerGroundNameMap;
   int m_powerNetCount;
   int m_groundNetCount;
   
   CString getPowerGroundName(HiokiNet& net);

public:
   void empty();
   HiokiNet* addNet(NetStruct& net);
   HiokiNet* findNet(const CString netName);
   void generatePowerRailShortTest(FILE *logFp, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface);
   void writeLogFile(FILE *logFp);
};

//_____________________________________________________________________________
class HiokiCompPin : public CObject
{
public:
   HiokiCompPin(CompPinStruct& compPinStruct, HiokiNet& net);
   ~HiokiCompPin();

private:
   //CPoint2d m_xyOrigin;
   //bool m_isMirrored;
   CompPinStruct& m_compPinStruct;
   HiokiNet& m_net;
   CString m_pinMap;
   bool m_alreadyTested;
   HiokiSurface m_testedSurface;
   HiokiTestAccess* m_topTestAccess;
   HiokiTestAccess* m_bottomTestAccess;
   
public:
   CString getPinName() const                { return m_compPinStruct.getPinName(); }
   CString getCompName() const               { return m_compPinStruct.getRefDes(); }   
   CString getCompPinName() const            { return m_compPinStruct.getPinRef(); }
   CString getNetName() const                { return m_net.getName(); }
   CPoint2d getInsertOrigin() const                { return m_compPinStruct.getOrigin(); }
   int   getEntityNumber() const             { return m_compPinStruct.getEntityNumber(); }

   void setPinMap(const CString pinMap)      { m_pinMap = pinMap; }
   CString getPinMap() const                 { return m_pinMap; }

   void setAlreadyTested(const bool tested)  { m_alreadyTested = tested; }
   bool getAlreadyTested() const             { return m_alreadyTested; }

   void setTestedSurface(const HiokiSurface testedSurface) { m_testedSurface = testedSurface; }
   HiokiSurface getTestedSurface() const { return m_testedSurface; }

   HiokiTestAccess* getTestAccess(FILE *logFp, const int maxHits, const HiokiSurface testSurface); // same net, any target
   HiokiTestAccess* getTestAccessOnSelf(const HiokiSurface testSurface); // only when target is self, otherwise NULL
};

//_____________________________________________________________________________
class HiokiCompPinMap
{
public:
   HiokiCompPinMap(bool isContainer);
   ~HiokiCompPinMap();

private:
   CTypedMapSortStringToObContainer<HiokiCompPin> m_compPinMap;
   bool m_alreadySorted;
      
   //CTypedMapStringToPtrContainer<HiokiCompPin*> m_compPinMap;

public:
   void empty();
   void addCompPin(HiokiCompPin* compPin);
   HiokiCompPin* addCompPin(CompPinStruct& compPinStruct, HiokiNet& net);
   HiokiCompPin* findCompPin(CString compPinName);
   POSITION getStartPosition() const;
   HiokiCompPin* getNext(POSITION& pos);

   void sort();
   HiokiCompPin* getFirstSorted();
   HiokiCompPin* getNextSorted();
};

//_____________________________________________________________________________
class HiokiComponent
{
public:
   HiokiComponent(FileStruct& file, InsertStruct& insert);
   ~HiokiComponent();

private:
   CString m_name;
   CString m_hiokiName;
   CString m_commentOperator; // If this set to "//", then the test will be commented out int the output
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
   HiokiSurface m_surface;
   bool m_isIcOpenTest;
   bool m_ipOpenTestDone;
   bool m_isPinShortTestDone;
   bool m_isLoaded;
   bool m_isSmd;
   int m_pinCount;
   double m_compHeight;
   bool m_writeSelectVisionTest;
   InsertStruct& m_insert;
   FileStruct& m_file;  // the file whose datalist has insert of this component
   HiokiCompPinMap m_compPinMap; //  not a container

   void initalize();
   void updateFromAttributes();

public:
   CString getName() const                      { return m_name; }
   CString getHiokiName() const                 { return m_hiokiName; }
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

   HiokiSurface getSurface() const          { return m_surface; }
   bool isIcOpenTest() const                    { return m_isIcOpenTest; }
   bool isLoaded() const                        { return m_isLoaded; }
   bool isSmd() const                           { return m_isSmd; }
   int getPinCount() const                      { return m_pinCount; }
   double getCompHeight() const                 { return m_compHeight; }
   InsertStruct& getInsert()                    { return m_insert; }
   FileStruct& getFile()                        { return m_file; }

   void addCompPin(HiokiCompPin* compPin);
   HiokiCompPin* findCompPin(const CString pinName);
   HiokiCompPin* getFirstSorted()           { return m_compPinMap.getFirstSorted(); }
   HiokiCompPin* getNextSorted()            { return m_compPinMap.getNextSorted(); }

   void setIcOpenTestDone(const bool done)      { m_ipOpenTestDone = done; }
   bool getIcOpenTestDone() const               { return m_ipOpenTestDone; }   

   void SetWriteSelectVisionTest(const bool write) { m_writeSelectVisionTest = write; }
   bool GetWriteSelectVisionTest() const { return m_writeSelectVisionTest; }

   void setPinShortTestDone(const bool done)    { m_isPinShortTestDone =  done; }
   bool getPinShortTestDone() const             { return m_isPinShortTestDone; }

   void setValue(const CString value)           { m_value = value; }
   CString getValue() const                     { return m_value; }
   CString getHiokiValue();

   void setComment(const CString comment)       { m_comment = comment; }
   CString getComment() const                   { return m_comment; }

   void setInsertOrigin(const CPoint2d origin)        { m_xyOrigin = origin; }
   CPoint2d getInsertOrigin() const                   { return m_xyOrigin; }


   void lookupAttributes(CCamCadData& camCadData, CAttributes* attributes, HiokiSettings &hiokiSettings);
   int generateTestPinCount(FILE *logFp, const int maxHits, HiokiSurface testSurface);
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
class HiokiIcDiodeCommandMap
{
public:
   HiokiIcDiodeCommandMap();
   ~HiokiIcDiodeCommandMap();

private:
   CTypedMapStringToPtrContainer<TakayaIcDiodeCommand*> m_icDiodeCommandMap;

public:
   void addCommand(const CString partNumber, const CString pinName, const bool swap);
   TakayaIcDiodeCommand* getGroundPin(const CString partNumber);

   void empty() { m_icDiodeCommandMap.empty(); }
};

//_____________________________________________________________________________
class HiokiAptTest
{
public:
	HiokiAptTest() { m_cElement = '\0';};
	~HiokiAptTest() {};

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
class HiokiAptModel 
{
public:
	HiokiAptModel(const CString name);
	~HiokiAptModel();

private:
	CString m_sName;
   CTypedPtrListContainer<HiokiAptTest*> m_aptTestList;

public:
   HiokiAptTest* createTest();
   bool generateAptModelTest(FILE *logFp, HiokiComponent& component, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface);
};

//_____________________________________________________________________________
class HiokiAptModelMap
{
public:
   HiokiAptModelMap();
	~HiokiAptModelMap();

private:
   CTypedMapStringToPtrContainer<HiokiAptModel*> m_aptModelMap;

public:
   void empty();
   HiokiAptModel* add(const CString aptModelName);
   HiokiAptModel* lookup(const CString aptModelName);
};

//_____________________________________________________________________________
class HiokiPinToPinShort : public CObject
{
public:
   HiokiPinToPinShort(const CString pinName1, const CString pinName2)
   {
      m_pinName1 = pinName1;
      m_pinName2 = pinName2;
   }
   ~HiokiPinToPinShort()
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
class HiokiPinToPinAnalyzer
{
public:
   HiokiPinToPinAnalyzer();
   ~HiokiPinToPinAnalyzer();

private:
   CTypedMapSortStringToObContainer<HiokiPinToPinShort> m_pinToPinMap;

public:
   void analysisPinToPinShort(BlockStruct& block, const double distance);
   HiokiPinToPinShort* getFirstSorted();
   HiokiPinToPinShort* getNextSorted();
};

//_____________________________________________________________________________
class HiokiPinToPinAnalyzerMap
{
public:
   HiokiPinToPinAnalyzerMap();
   ~HiokiPinToPinAnalyzerMap();

private:
	CTypedMapIntToPtrContainer<HiokiPinToPinAnalyzer*> m_pcbComponentPinToPinAnalyzerMap;

public:
   HiokiPinToPinAnalyzer* getPinToPinAnalyzer(BlockStruct& block, const double distance);
};

//_____________________________________________________________________________
class HiokiPackageOutlineExtentMap
{
public:
   HiokiPackageOutlineExtentMap();
   ~HiokiPackageOutlineExtentMap();

private:
   CTypedMapIntToPtrContainer<CExtent*> m_blockPackageExtentMap;

public:
   CExtent* getPackageOutlineExtent(CCamCadData& camCadData, BlockStruct& block, bool realPart);
};

//_____________________________________________________________________________
class HiokiComponentCollection
{
public:
   HiokiComponentCollection(CGTabTableList *tableList);
   ~HiokiComponentCollection();
    static int icdiode;
private:
   CTypedMapStringToPtrContainer<HiokiComponent*> m_componentMapByName;
   CTypedPtrListContainer<HiokiComponent*>m_resistorComponents;
   CTypedPtrListContainer<HiokiComponent*>m_capacitorComponents;
   CTypedPtrListContainer<HiokiComponent*>m_inductorComponents;
   CTypedPtrListContainer<HiokiComponent*>m_icComponents;
   CTypedPtrListContainer<HiokiComponent*>m_diodeComponents;
   CTypedPtrListContainer<HiokiComponent*>m_transitorComponents;
   CTypedPtrListContainer<HiokiComponent*>m_capacitorPolarizedComponents;
   CTypedPtrListContainer<HiokiComponent*>m_capacitorTantalumComponents;
   CTypedPtrListContainer<HiokiComponent*>m_otherComponents;
   CTypedPtrListContainer<HiokiComponent*>m_testPoints;  // Output to repair file only, no tests generated, not really a component

   ICOpenTestGeomMap m_icOpenTestGeomMap;
   HiokiPinToPinAnalyzerMap m_pinToPinAnalyzerMap;
   CMapStringToString m_pinToPinProbePairMap;
   HiokiPackageOutlineExtentMap m_packageOutlineExtentMap;

   CPoint2d m_topOutputOrigin;
   CPoint2d m_botOutputOrigin;

   int m_visionTestCount;
   CSubclassList m_subclassList;

   void applyTakayaOrigin(CPoint2d &cczInsertOrigin, bool topside);
   
   void generateResistorTest(FILE *logFp, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface);
   void generateCapacitorTest(FILE *logFp, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface);
   void generateDiodeTest(FILE *logFp, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface);
   void generateInductorTest(FILE *logFp, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface);
   void generateIcTest(CCamCadData& camcadData, FILE *logFp, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiNet& groundNet, HiokiIcDiodeCommandMap& icDiodeCommandMap, 
      const bool isIcDiodeTest, HiokiSettings& settings, const HiokiSurface testSurface);
   void generateTransistorTest(FILE *logFp, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface);
   void generateCapacitorPolarizedTest(CCamCadData& camcadData, FILE *logFp, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiNet* groundNet, HiokiSettings& settings, const HiokiSurface testSurface);
   void generateCapacitorTantalumTest(CCamCadData& camcadData, FILE *logFp, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiNet* groundNet, HiokiSettings& settings, const HiokiSurface testSurface);
   void generateIcOpenTest(FILE *logFp, CCamCadData& camcadData, HiokiNet& groundNet, HiokiComponent& component, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface);

   void generateUnsupportedDeviceTypeTest(FILE *logFp, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface);

   void generateGenericTest(FILE *logFp, HiokiComponent& component, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection,
      CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface);

   bool generateOutput(FILE *logFp, HiokiComponent& component, HiokiCompPin* compPin1, HiokiCompPin* compPin2,
      HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface);

   bool isValidDeviceTypeForMDBUse(const DeviceTypeTag deviceType);
   bool isValidTwoPinDeviceType(const DeviceTypeTag deviceType);
   bool isValidThreePinDeviceType(const DeviceTypeTag deviceType);
   bool isValidFourPinDeviceType(const DeviceTypeTag deviceType);
   void generateOutputUsingMDB(FILE *logFp, HiokiComponent& component, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
      HiokiSettings& settings, const HiokiSurface testSurface, CSubclass *sc);
   void generateTwoPinOutputUsingMDB(FILE *logFp, HiokiComponent& component,
     HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
     HiokiSettings& settings, const HiokiSurface testSurface, CSubclassElement *el);
   void generateThreePinOutputUsingMDB(FILE *logFp, HiokiComponent& component,
     HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
     HiokiSettings& settings, const HiokiSurface testSurface, CSubclassElement *el);
   void generateFourPinOutputUsingMDB(FILE *logFp, HiokiComponent& component,
     HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
     HiokiSettings& settings, const HiokiSurface testSurface, CSubclassElement *el);

   bool generateVisionOutput(FILE *logFp, HiokiComponent& component, 
      CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface);

   void generatePinToPinShortTest(CCamCadData& camcadData, FILE *logFp, CTypedPtrListContainer<HiokiComponent*>& components, HiokiProbeOutputCollection& probeOutputCollection,
      CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface); 

   void generateHighFlyZoneTest(FileStruct& file, CCamCadData& camcadData, FILE *logFp, CTypedPtrListContainer<HiokiComponent*>& components, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface);
   
   BlockStruct* GetRealPartPackageBlockForComponent(FileStruct& file, CCamCadData& camCadData,HiokiComponent* component);
   DataStruct *FindAccessDataForEntityNum(CCamCadData& camCadData,  BlockStruct *pcbFileBlock, long entityNumber);
   void getAccessMarkerXYOffset(CCamCadData& camCadData, BlockStruct *pcbFileBlock, int entityNumber, CPoint2d &pinOriginWithOffset);

public:
   void empty();
   void moveIcDeviceTypeToOtherComponent();

   void SetTakayaOrigin(CPoint2d topOrigin, CPoint2d botOrigin)   { m_topOutputOrigin = topOrigin; m_botOutputOrigin = botOrigin; }

   void convertToHiokiUnits(CPoint2d& location, double unitFactor, bool topside);

   HiokiComponent* addComponent(CCamCadData& camCadData, FileStruct& file, DataStruct& data, HiokiSettings& settings, CTMatrix transformMatrix);
   HiokiComponent* findComponent(const CString name);

   void generateComponentTest(CCamCadData& camcadData, FILE *logFp, HiokiAptModelMap& adptModelMap, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, 
      HiokiNet* groundNet, HiokiIcDiodeCommandMap& icDiodeCommandMap, HiokiSettings& settings, const HiokiSurface testSurface);

   void generateComponentHighFlyZoneTest(FileStruct& file, CCamCadData& camcadData, FILE *logFp, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, HiokiSettings& settings, const HiokiSurface testSurface);

   void writeLogFile(FILE *logFp);
};

//_____________________________________________________________________________
class HiokiPcbFile
{
public:
   HiokiPcbFile(FileStruct& fileStruct);
   ~HiokiPcbFile();

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


class HiokiCosRecord
{
   // Collects data for one COS file record and can write the record.
private:
   CString m_refdes;     // aka Part Name in Hioki
   CString m_HProbeNum;
   CString m_LProbeNum;
   char    m_testType;  // aka Device Name in Hioki
   HiokiComponent *m_hiokiComp;

public:
   //HiokiCosRecord();
   HiokiCosRecord(CString refDes, HiokiComponent *hiokiComp);
   HiokiCosRecord(CString refDes, HiokiComponent *hiokiComp, CString HProbeNum, CString LProbeNum);
   HiokiCosRecord(CString refDes, HiokiComponent *hiokiComp, char testType, CString HProbeNum, CString LProbeNum);

   void GetFormattedRecord(int stepNo, CString &output);
};

//_____________________________________________________________________________

class HiokiWriter
{
public:
   HiokiWriter();
  
   HiokiWriter(CCEtoODBDoc& m_camCadDoc, const CString pathName);
   ~HiokiWriter();
    
private:
   CCEtoODBDoc& m_camCadDoc;
   CCamCadData& m_camCadData;
   HiokiSettings m_settings;
   double m_unitFactor;
   bool m_hasGroundNet;

   CPoint2d m_topOutputOrigin;
   CPoint2d m_botOutputOrigin;
   
   CTypedListContainer<CPtrList,HiokiPcbFile*> m_pcbDesignList;
   HiokiAptModelMap m_aptModelMap;
   HiokiIcDiodeCommandMap m_icDiodeCommandMap;
   HiokiNetMap m_netMap;
   HiokiCompPinMap m_compPinMap;
   HiokiComponentCollection m_componentCollection;
   CStringArray m_topOutputArray;
   CStringArray m_bottomOutputArray;

   HiokiNet* m_groundNet;   
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

   bool getPcbDesignList();
   void loadSettings();
   bool loadHiokiData(FileStruct& pcbFile);

   void writeHiokiOutput(const CString fileName, const CString ptpFilename, CStringArray& outputArray, HiokiProbeOutputCollection& probeOutputCollection, const HiokiSurface testSurface);
   void writeHiokiProbes(CStdioFileWriteFormat* file, HiokiProbeOutputCollection& probeOutputCollection, const HiokiSurface testSurface);

   void generateProbablyShortTest(FileStruct& fileStruct, FILE *logFp, HiokiProbeOutputCollection& probeOutputCollection, CStringArray& outputArray, const HiokiSurface testSurface);
   CString getCompPinRefForProbableShort(NetStruct& netStruct);
   
public:
   void write();
   bool diode;
};

#endif
