// $Header: /CAMCAD/5.0/read_wrt/OdbPpIn.h 43    5/25/07 6:54p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#if ! defined (__OdbPpIn_h__)
#define __OdbPpIn_h__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AlienDatabase.h"
#include "RwLib.h"
#include "QfeLib.h"
#include "Units.h"
#include "Consolidator.h"

#define LineStatusUpdateInterval 200

CUnits& getOdbUnits();

//_____________________________________________________________________________
class COdbFeatureFile;
class COdbPpDatabase;
class COdbStep;
class COdbLayer;
class COdbFid;
class COdbSubnet;
class COdbComponentsFile;
class COdbComponent;
class COdbComponentPin;
class COdbPadstack;
class COdbPackageGeometry;
class COdbPadstackGeometry;
class COdbPackage;
class COdbNet;
class COdbPadstackTemplate;
class CQfePadTree;
class COdbFeature;
class CStdioUnixCompressedFile;

//_____________________________________________________________________________
CString properCase(CString string);
int getFileSize(const CString& path);

//_____________________________________________________________________________
#define tok(a) tok##a ,
#define tok2(a,b) tok_##b ,

enum OdbTokenTag
{
      // tok2("@",AtSign) yields <<tok_AtSign>>
      // tok(Angle)       yields <<tokAngle>>

   #include "OdbPpReaderTokens.h"

   tok_OdbTokenTagUndefined
};

#undef tok
#undef tok2

OdbTokenTag stringToOdbToken(CString tokenString);
OdbTokenTag paramToOdbToken(const CStringArray& params,int index);

//_____________________________________________________________________________
enum OdbPadUsageTag
{
   padUsageToeprint       = 0,
   padUsageVia            = 1,
   padUsageGlobalFiducial = 2,
   padUsageLocalFiducial  = 3,
   padUsageToolingHole    = 4,
   padUsageUndefined      = 5
};

CString odbPadUsageTagToString(OdbPadUsageTag tag);

//_____________________________________________________________________________
// ordered by decreasing severity
enum PadstackCompareStatusTag
{
   padstackCompareNotEquivalent ,
   padstackCompareMissingLayer ,
   padstackComparePadType ,
   padstackComparePolarity ,
   padstackCompareOffset ,
   padstackCompareRotation ,
   padstackCompareDcode ,
   padstackCompareEquivalent ,
   padstackCompareUndefined
};

CString padstackCompareStatusTagToString(PadstackCompareStatusTag tag);

class COdbBom;

//_____________________________________________________________________________
class CTimeInterval : public CObject
{
private:
   clock_t m_startTime;
   clock_t m_stopTime;

public:
   CTimeInterval();

   void setStartTime();
   void setStopTime();
   double getIntervalInSeconds();
   double getNonZeroIntervalInSeconds();
};

//_____________________________________________________________________________
class CUniqueErrorFilter : public CObject
{
private:
   CMapStringToInt m_errorMessages;

public:
   bool writef(CWriteFormat& log,int prefix,
      const CString& filePath,int lineNumber,const CString& recordString,
      const char* format,...);

   void empty() { m_errorMessages.RemoveAll(); }
};

//_____________________________________________________________________________
enum OdbArchiveFileStatusTag
{
   statusCopyOfArchiveFailed,
   statusGunzipFailure,
   statusMakeTempDirFailure,
   statusNoTempDir,
   statusNoMatrixDir,
   statusOdbArchiveSucceeded,
   statusTarFailure,
   statusTooManyDirs,
   statusUnrecognizedArchiveFormat,
   statusOdbArchiveUndefined
};

CString odbArchiveFileStatusTagToString(OdbArchiveFileStatusTag tagValue);

class COdbArchiveFile : public CObject
{
private:
   CFilePath m_archiveFilePath;
   CString m_tempDirPath;
   CString m_commandLine;
   CFilePath m_projectPath;
   bool m_hasTempFiles;

public:
   COdbArchiveFile(const CString& archiveFilePath);

   CString getTempDirPath();
   bool isPathArchive();
   OdbArchiveFileStatusTag extract();
   CString getProjectPath() { return m_projectPath.getPath(); }
   CString getProjectTempPath();
   CString getLogFilePath();
   CString getCommandLine() { return m_commandLine; }
   bool hasTempFiles() { return m_hasTempFiles; }
   bool removeTempFiles();
};

//_____________________________________________________________________________
class COdbFileReader : public CFileReader
{
private:
   static bool s_writeUncompressedFileFlag;

   CStdioFile* m_stdioFile;
   CStdioFile* m_uncompressedFile;
   CStdioCompressedFile* m_compressedFile;
   CString m_filePath;
   CString m_record;
   int m_fileSize;
   FileStatusTag m_openStatus;
   CTimeInterval m_readTime;

   char* m_start[MaxParserParams];
   char* m_end[MaxParserParams];
   char m_numBuf[MaxNumLength + 1];

public:
   COdbFileReader(int maxParams = MaxParserParams);
   ~COdbFileReader();

   void setRecord(const CString& record);
   bool open(const CString& filePath);
   FileStatusTag openFile(const CString& filePath);
   void close();
   bool readRecord();
   FileStatusTag readFileRecord(CWriteFormat* statusLog=NULL);
   void writeReadStatistics(CWriteFormat* log,int prefix);
   CString getRecord() { return m_record; }
   CString getFilePath() { return m_filePath; }
   FileStatusTag getOpenStatus() const { return m_openStatus; }
   void setInchesPerUnit(double inchesPerUnit) { m_inchesPerUnit = inchesPerUnit; }

   int getNumParams();
   int getNumRecordsRead() { return m_numRecordsRead; }
   CString getParam(int index);
   CString getParamToEndOfLine(int index);  // start at param index, return from there to end of line
   void getParam(CString& param,int index);
   void getParam(int& iParam,int index);
   void getParam(double& fParam,int index);
   void getParamInInches(double& fParam,int index) { getParam(fParam,index);  fParam *= m_inchesPerUnit; }
   int getFileLength();
   int getFilePosition();

   static void setWriteUncompressedFileFlag(bool flag) { s_writeUncompressedFileFlag = flag; }

private:
   void parse();
};

//_____________________________________________________________________________
class COdbMemFileReader : public CFileReader
{
private:
   HANDLE m_fileHandle;
   HANDLE m_mappingHandle;
   CString m_filePath;
   CString m_record;
   DWORD m_fileSize;
   const char* m_baseAddress;
   const char* m_currentAddress;
   const char* m_limitAddress;

   const char* m_start[MaxParserParams];
   const char* m_end[MaxParserParams];
   const char* m_recordStart;
   const char* m_recordEnd;
   char m_numBuf[MaxNumLength + 1];

public:
   COdbMemFileReader(int maxParams = MaxParserParams);
   ~COdbMemFileReader();

   bool open(const CString& filePath);
   void close();
   bool readRecord();
   CString getRecord();
   void setInchesPerUnit(double inchesPerUnit) { m_inchesPerUnit = inchesPerUnit; }

   int getNumParams();
   int getNumRecordsRead() { return m_numRecordsRead; }
   CString getParam(int index);
   void getParam(CString& param,int index);
   void getParam(int& iParam,int index);
   void getParam(double& fParam,int index);
   void getParamInInches(double& fParam,int index) { getParam(fParam,index);  fParam *= m_inchesPerUnit; }
   int getFileLength() { return (int)m_fileSize; }
   int getFilePosition() { return (m_currentAddress - m_baseAddress); }

private:
   void parse();
};

//_____________________________________________________________________________
class COdbMemFileReaderTester : public COdbMemFileReader
{
protected:
   COdbFileReader m_fileReader;

public:
   COdbMemFileReaderTester(int maxParams = MaxParserParams);

   bool open(const CString& filePath);
   void close();
   bool readRecord();

   bool testAssert(bool assertFlag);
};

//_____________________________________________________________________________
enum StandardSymbolTypeTag
{
   standardSymbolCircle,
   standardSymbolSquare,
   standardSymbolRectangle,
   standardSymbolRoundedRectangle,
   standardSymbolChamferedRectangle,
   standardSymbolOval,
   standardSymbolDiamond,
   standardSymbolOctagon,
   standardSymbolRoundDonut,
   standardSymbolSquareDonut,
   standardSymbolHorizontalHexagon,
   standardSymbolVerticalHexagon,
   standardSymbolButterfly,
   standardSymbolSquareButterfly,
   standardSymbolTriangle,
   standardSymbolHalfOval,
   standardSymbolRoundThermalRounded,
   standardSymbolRoundThermalSquare,
   standardSymbolSquareThermal,
   standardSymbolSquareThermalOpenCorners,
   standardSymbolSquareRoundThermal,
   standardSymbolRectangularThermal,
   standardSymbolRectangularThermalOpenCorners,
   standardSymbolEllipse,
   standardSymbolMoire,
   standardSymbolHole,
   standardSymbolNull,
   standardSymbolInvalid,
   standardSymbolUndefined
};

CString standardSymbolTypeTagToString(StandardSymbolTypeTag);

class COdbFeatureSymbol : public CObject
{
private:
   CString m_name;
   StandardSymbolTypeTag m_type;
   double m_dimension[6];
   double m_rotation;  // angle in degrees counterclockwise
   int m_symmetry;  // in degrees, from 0 to 360;
   double m_unitfactor; //in versin 7.0

   COdbFeatureFile* m_symbolFeatureFile;
   BlockStruct* m_apertureBlock;

public:
   COdbFeatureSymbol(CString name);
   ~COdbFeatureSymbol();

   CString getName()       { return m_name; }
   int getSymmetry()       { return m_symmetry; }
   bool isStandardSymbol() { return m_type != standardSymbolUndefined && 
                                    m_type != standardSymbolInvalid;      }
   bool hasData()          { return m_symbolFeatureFile != NULL; }

   bool readSymbolFeatureFile(COdbPpDatabase& odbPpDatabase,CWriteFormat& log);
   bool isEquivalent(const COdbFeatureSymbol& other) const;
   double getDimension(int index) { return m_dimension[index]; }
   double getDimensionInInches(int index) { return  m_unitfactor * m_dimension[index]/1000.; }
   double getDimensionInPageUnits(int index) { return getOdbUnits().convertFrom(pageUnitsInches, m_unitfactor * m_dimension[index]/1000.); } 
   CString getApertureNameDescriptor();
   ApertureShapeTag getApertureShape();

   void buildCamCadData(COdbPpDatabase& odbPpDatabase,BlockStruct* parentBlock);
   BlockStruct* getDefinedAperture(COdbPpDatabase& odbPpDatabase,CWriteFormat& log);
   int getWidthIndex(COdbPpDatabase& odbPpDatabase);

   void dump(CWriteFormat& writeFormat,int depth = -1);
   void setUnitFactor(double unitfactor){ m_unitfactor = unitfactor; }
private:
   void parseName();
};

//_____________________________________________________________________________
class COdbFeatureSymbolArray : public CTypedPtrArray<CObArray,COdbFeatureSymbol*>
{
public:
   COdbFeatureSymbol* getAt(int index);
   COdbFeatureSymbol* getAt(int index,CWriteFormat& log,
      const CString& line,const CString& filePath,int lineNumber);
};

//_____________________________________________________________________________
class COdbFeatureSymbolMap : public CTypedObMapWithArrayContainer<COdbFeatureSymbol>
{
public:
   COdbFeatureSymbol* getDefinedAt(const CString& name);

   void buildCamCadData(COdbPpDatabase& odbPpDatabase,CWriteFormat& log);
};

//_____________________________________________________________________________
class CAttributeMapEntry : public CObject
{
private:
   CString m_attribute;
   CString m_mapAttribute;

public:
   CAttributeMapEntry(const CString& attribute,const CString& mapAttribute);

   CString getAttributeName() { return m_attribute; }
   CString getMappedAttributeName() { return m_mapAttribute; }

   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
class CAttributeMapEntryArray : public CTypedObArrayWithMapContainer<CAttributeMapEntry>
{
public:
   CAttributeMapEntryArray();
   CAttributeMapEntryArray(CAttributeMapEntryArray& other);
   ~CAttributeMapEntryArray();
   CAttributeMapEntryArray& operator=(CAttributeMapEntryArray& other);

   CAttributeMapEntry* add(const CString& attribute,const CString& mapAttribute);
   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
enum AttributeDefTypeTag
{
   typeUndefined, 
   typeLayer    , typeInt    , typeDouble, typePInch, typePMil,typeRadio,typeSet,typeMenu,
   typeString   , typeBoolean
};

class CAttDef : public CObject
{
private:
   AttributeDefTypeTag m_type;
   CString m_name;

public:
   CAttDef(AttributeDefTypeTag type,const CString& name);
   AttributeDefTypeTag getType() { return m_type; }
   CString getName() { return m_name; }

};

//_____________________________________________________________________________
class CAttDefArray : public CTypedObArrayWithMapContainer<CAttDef>
{
public:
   CAttDefArray();
   CAttDefArray(CAttDefArray& other);
   ~CAttDefArray();
   CAttDefArray& operator=(CAttDefArray& other);

   CAttDef* add(AttributeDefTypeTag type,const CString& name);
};

//_____________________________________________________________________________
class COdbOptionTypeValue : public CObject
{
private:
   CString m_optionTypeName;
   CStringArray m_optionValues;

public:
   COdbOptionTypeValue(const CString& optionTypeName);
   COdbOptionTypeValue(const COdbOptionTypeValue& other);
   COdbOptionTypeValue& operator=(const COdbOptionTypeValue& other);

   CString getOptionTypeName() { return m_optionTypeName; }
   CString getOptionTypeKey() { CString optionTypeKey(m_optionTypeName);  return optionTypeKey.MakeLower(); }
   void addValue(int optionId,const CString& optionValue);
   CString getValue(int optionId);
};

//_____________________________________________________________________________
class COdbOptionTypeValues : public CObject
{
private:
   CTypedObMapWithArrayContainer<COdbOptionTypeValue> m_optionTypes;

public:
   COdbOptionTypeValues();
   COdbOptionTypeValues(const COdbOptionTypeValues& other);
   COdbOptionTypeValues& operator=(const COdbOptionTypeValues& other);
   void empty() { m_optionTypes.empty(); }

   void addValue(const CString& optionTypeName,int optionId,const CString& optionValue);
   CString getValue(const CString& optionTypeName,int optionId);
   bool contains(CString optionTypeName);
};

//_____________________________________________________________________________
class COdbFeatureAttribute : public CObject
{
private:
   CString m_param;
   CString m_name;
   CString m_stringValue;
   ValueTypeTag m_valueType;

public:
   COdbFeatureAttribute(const CString& name="",const CString& param="",
      ValueTypeTag valueType=valueTypeUndefined,const CString& stringValue="");
   COdbFeatureAttribute(const COdbFeatureAttribute& other);
   COdbFeatureAttribute& operator=(const COdbFeatureAttribute& other);

   CString getParam() const { return m_param; }
   CString getName() const { return m_name; }
   CString getStringValue() const { return m_stringValue; }
   CString& getStringValue() { return m_stringValue; }
   ValueTypeTag getValueType() const { return m_valueType; }

   void setParam(const CString& param) { m_param = param; }
   void setName(const CString& name) { m_name = name; }
   void setStringValue(const CString& stringValue) { m_stringValue = stringValue; }
   void setValueType(ValueTypeTag valueType) { m_valueType = valueType; }

   void addAttribute(CAttributes** attributeMap,
      CCamCadDatabase& camCadDatabase,CWriteFormat& log);
};

//_____________________________________________________________________________
class COdbFeatureAttributeArray : public CTypedObArrayContainer<COdbFeatureAttribute*>
{
public:
   COdbFeatureAttributeArray(int growBySize = 10) : 
      CTypedObArrayContainer<COdbFeatureAttribute*>(growBySize) {}
   COdbFeatureAttributeArray(const COdbFeatureAttributeArray& other);

   void placeAttributes(CAttributes** attributeMap,
      CCamCadDatabase& camCadDatabase,CWriteFormat& log);
   void addAttribute(const CString& name,ValueTypeTag valueType,const CString& stringValue);
   void takeAttributes(CStringArray* values,const CString& namePrefix);
   void mergeFrom(const COdbFeatureAttributeArray& other);
   void condense();
};

//_____________________________________________________________________________
class COdbFeatureAttributeArrayWithMap : public CTypedObArrayWithMapContainer<COdbFeatureAttribute>
{
public:
   void setAllUndefined();
};

//_____________________________________________________________________________
class COdbFeatureAttributes : public CObject
{
private:
   CAttributeMapEntryArray m_attribMapArray;
   CAttDefArray m_attDefArray;
   COdbOptionTypeValues m_optionTypeValues;
   CStringArray m_attributeNames;
   CStringArray m_attributeTextStrings;

   COdbFeatureAttributeArrayWithMap m_attributeCache;
   COdbLayer* m_layer;  // set by COdbFeatureFile objects

public:
   COdbFeatureAttributes();
   COdbFeatureAttributes(COdbFeatureAttributes& other);
   ~COdbFeatureAttributes();
   void empty();

   void initializeSettings(COdbFeatureAttributes& other);
   void initNamesAndStrings();

   void setLayer(COdbLayer* layer) { m_layer = layer; }
   int getAttributeNameCount() { return m_attributeNames.GetSize(); }
   int getAttributeTextStringCount() { return m_attributeTextStrings.GetSize(); }
   CAttDef* addAttDef(AttributeDefTypeTag type,const CString& name)
      { return m_attDefArray.add(type,name); }
   CAttributeMapEntry* addMappedAttribute(const CString& attribute,const CString& mapAttribute)
      { return m_attribMapArray.add(attribute,mapAttribute); }
   COdbOptionTypeValues& getOptionTypeValues() { return m_optionTypeValues; }

   void addAttributeName(const CString& attributeName) { m_attributeNames.Add(attributeName); }
   void addAttributeTextString(const CString& attributeTextString)  { m_attributeTextStrings.Add(attributeTextString); }

   void ResetAttributeNameList() { m_attributeNames.RemoveAll(); m_attributeCache.empty(); }

   bool addAttributes(COdbFeatureAttributeArray& featureAttributeArray,
      CFileReader& odbParser,int attributeParamIndex,
      const CString& filePath,const CString& recordString,int lineNumber,
      CWriteFormat& errorLog);

   bool addAttributes(COdbFeature& feature,
      CFileReader& odbParser,int attributeParamIndex,
      const CString& filePath,const CString& recordString,int lineNumber,
      CWriteFormat& errorLog);

   bool addAttribute(CAttributes** attributeMap,CCamCadDatabase& camCadDatabase,
      const CString& param,
      const CString& filePath,const CString& recordString,int lineNumber,
      CWriteFormat& errorLog);
   bool fillAttributes(COdbFeatureAttributeArrayWithMap& attributeArray,CCamCadDatabase& camCadDatabase,
      const CStringArray& params,int attributeParamIndex,
      const CString& filePath,const CString& recordString,int lineNumber,
      CWriteFormat& errorLog);
   bool getAttributes(COdbFeatureAttributeArray& attributeArray,CCamCadDatabase& camCadDatabase,
      const CStringArray& params,int attributeParamIndex,
      const CString& filePath,const CString& recordString,int lineNumber,
      CWriteFormat& errorLog);
   bool getAttributeData(COdbFeatureAttribute& featureAttribute,
      const CString& filePath,const CString& recordString,int lineNumber,
      CWriteFormat& errorLog);
   CString getMappedAttributeName(const CString& rawAttributeName);
   bool getMappedAttributeName(const CString& rawAttributeName, CString &mappedAttributeName);

   void dumpAttributeMap(CWriteFormat& log) { m_attribMapArray.dump(log); }
};

//_____________________________________________________________________________
enum OdbOptionTag
{
   optionComponentsSmdRule       ,
   optionDisplayLogFile          ,
   optionEnableDumpOutput        ,
   optionEnableDebugProperties   ,
   optionEnablePinNameRefdesEdit ,
   optionEnableNcPinQuest        ,
   optionEnablePadstackQuest     ,
   optionEnableViastackQuest     ,
   optionEnableOptionLogging     ,
   optionEnablePadstackReport    ,
   optionEnablePackagePinReport  ,
   optionEnableManufactureRead   ,
   optionEnableArtworkRead       ,
   optionKeepUncompressedFiles   ,
   optionWriteDebugCcFile        ,
   optionWriteHtmlLogFile        ,
   optionWriteUncompressedFiles  ,
   optionWriteTextLogFile        ,
   optionUncompressCommand       ,
   optionLogFileDirectoryPath    ,
   optionUserPathName            ,
   optionProject                 ,
   optionConvertFreePadsToVias   ,
   optionPadSearchRadius         ,
   optionUndefined
};

class COdbOption : public CObject
{
private:
   OdbOptionTag m_optionTag;
   CString m_name;
   CString m_stringValue;
   bool m_boolValue;
   int m_intValue;
   double m_dblValue;

public:
   COdbOption(OdbOptionTag optionTag,const CString& name,const CString& value);

   CString getName() { return m_name; }

   CString getStringValue() { return m_stringValue; }
   bool getBoolValue()      { return m_boolValue; }
   int getIntValue()        { return m_intValue; }
   double getDoubleValue()  { return m_dblValue; }

   void setValue(const CString& value);
   void setValue(bool value);
   void setValue(int value);
   void setValue(double value);
};

//_____________________________________________________________________________
#define QuserPathName            "userPathName"
#define Qproject                 "project"

class COdbOptions : public CObject
{
private:
   CTypedObArrayContainer<COdbOption*> m_options;
   COdbFeatureAttributes m_featureAttributes;

   // Consolidator has its own set of controls and parsing support, separate from
   // the other "local" options. This is the connector.
   CGeometryConsolidatorControl m_consolidatorControl;

public:
   COdbOptions();

   CGeometryConsolidatorControl &GetConsolidatorControl() { return m_consolidatorControl; }

   bool    getOptionFlag(OdbOptionTag optionTag);
   CString getOptionString(OdbOptionTag optionTag);
   int     getOptionInt(OdbOptionTag optionTag);
   double  getOptionDbl(OdbOptionTag optionTag);
   
   void add(OdbOptionTag optionTag,const CString& name,const CString& value);
   bool COdbOptions::loadOdbSettings(const CString& formatIn,CWriteFormat* log=NULL);
   bool loadOdbSettings(CWriteFormat* log=NULL);
   bool setOption(const CString& optionLine);
   void writeOptions(CWriteFormat& log);

   COdbFeatureAttributes& getFeatureAttributes() { return m_featureAttributes; }
   CString getMappedAttributeName(const CString& rawAttributeName)
      { return m_featureAttributes.getMappedAttributeName(rawAttributeName); }
};

//_____________________________________________________________________________
class COdbCompressableFileCache : public CObject
{
private:
   CStringList m_uncompressedFilePaths;

public:
   COdbCompressableFileCache();
   ~COdbCompressableFileCache();

   void cleanUp(CWriteFormat* log=NULL);

   // returns true if the uncompressed file path already exists or is created
   bool uncompress(const CString& uncompressedFilePath,CWriteFormat& errorLog);
};

//_____________________________________________________________________________
class COdbPolygonSegment : public CObject
{
protected:
   CPoint2d m_endPoint;

public:
   COdbPolygonSegment(double xEnd,double yEnd);

   void mirrorPolygonSegment();
   virtual void mirrorPolygon();
   virtual void buildCamCadData(COdbPpDatabase& odbPpDatabase,CPoly* poly,CPnt*& lastVertex, CPoint2d &lastOdbVertex);
};

//_____________________________________________________________________________
class COdbPolygonCurve : public COdbPolygonSegment
{
private:
   CPoint2d m_center;
   bool m_clockwise;

public:
   COdbPolygonCurve(double xEnd,double yEnd,double xCenter,double yCenter,bool clockwise);

   virtual void mirrorPolygon();
   virtual void buildCamCadData(COdbPpDatabase& odbPpDatabase,CPoly* poly,CPnt*& lastVertex, CPoint2d &lastOdbVertex);
};

//_____________________________________________________________________________
typedef CTypedObListContainer<COdbPolygonSegment*> COdbPolygonSegmentList;

//_____________________________________________________________________________
enum OutlineTypeTag
{
   outlineTypeCircle,
   outlineTypeSquare,
   outlineTypeRectangle,
   outlineTypeContour,
};

class COdbOutline : public CObject
{
private:
   GraphicClassTag m_graphicClass;

public:
   COdbOutline()  { m_graphicClass = graphicClassComponentOutline; }
   virtual ~COdbOutline();

   virtual OutlineTypeTag getOutlineType() = 0;
   virtual void mirrorOutline() = 0;
   virtual void buildCamCadData(COdbPpDatabase& odbPpDatabase,int layerIndex,BlockStruct* parentBlock) = 0;
   virtual void fillOutlinePoly(COdbPpDatabase& odbPpDatabase, CPoly &poly) = 0;

   GraphicClassTag getGraphicClass()         { return m_graphicClass; }
   void setGraphicClass(GraphicClassTag gc)  { m_graphicClass = gc; }

};

//_____________________________________________________________________________
class COdbCircleOutline : public COdbOutline
{
private:
   double m_xCenter;
   double m_yCenter;
   double m_radius;

public:
   COdbCircleOutline(double xCenter,double yCenter,double radius);

   virtual OutlineTypeTag getOutlineType() { return outlineTypeCircle; }
   virtual void mirrorOutline();
   virtual void buildCamCadData(COdbPpDatabase& odbPpDatabase,int layerIndex,BlockStruct* parentBlock);
   virtual void fillOutlinePoly(COdbPpDatabase& odbPpDatabase, CPoly &poly);
};

//_____________________________________________________________________________
class COdbSquareOutline : public COdbOutline
{
private:
   double m_xCenter;
   double m_yCenter;
   double m_halfSide;

public:
   COdbSquareOutline(double xCenter,double yCenter,double halfSide);

   virtual OutlineTypeTag getOutlineType() { return outlineTypeSquare; }
   virtual void mirrorOutline();
   virtual void buildCamCadData(COdbPpDatabase& odbPpDatabase,int layerIndex,BlockStruct* parentBlock);
   virtual void fillOutlinePoly(COdbPpDatabase& odbPpDatabase, CPoly &poly);
};

//_____________________________________________________________________________
class COdbRectangleOutline : public COdbOutline
{
private:
   double m_lowerLeftX;
   double m_lowerLeftY;
   double m_width;
   double m_height;

public:
   COdbRectangleOutline(double lowerLeftX,double lowerLeftY,double width,double height);

   virtual OutlineTypeTag getOutlineType() { return outlineTypeRectangle; }
   virtual void mirrorOutline();
   virtual void buildCamCadData(COdbPpDatabase& odbPpDatabase,int layerIndex,BlockStruct* parentBlock);
   virtual void fillOutlinePoly(COdbPpDatabase& odbPpDatabase, CPoly &poly);
};

//_____________________________________________________________________________
class COdbPolygon : public COdbOutline
{
private:
   CPoint2d m_startPoint;
   bool m_holeFlag;  // false - island

   COdbPolygonSegmentList m_polygonSegmentList;

public:
   COdbPolygon(double xStart,double yStart,bool holeFlag);

   virtual OutlineTypeTag getOutlineType() { return outlineTypeContour; }

   COdbPolygonSegment* addSegment(double x,double y);
   COdbPolygonCurve* addCurve(double xEnd,double yEnd,double xCenter,double yCenter,bool clockwise);
   virtual void mirrorOutline();
   virtual void buildCamCadData(COdbPpDatabase& odbPpDatabase,
      int layerIndex,BlockStruct* parentBlock);
   void buildCamCadData(COdbPpDatabase& odbPpDatabase,DataStruct* polyStruct,
      bool filledFlag,int widthIndex,bool positivePolarity,
      const CString& netName,CWriteFormat* log);
   virtual void fillOutlinePoly(COdbPpDatabase& odbPpDatabase, CPoly &poly);

};

//_____________________________________________________________________________
typedef CTypedObListContainer<COdbPolygon*> COdbPolygonList;

//_____________________________________________________________________________
class COdbFeatureBuildState : public CObject
{
private:
   COdbPpDatabase& m_odbPpDatabase;
   CCamCadDatabase* m_camCadDatabase;
   int m_layerIndex;
   BlockStruct* m_parentBlock;
   CWriteFormat& m_log;

   CString m_netName;
   DataStruct* m_polyStruct;
   CPoly* m_poly;
   CPnt* m_lastVertex;
   int m_widthIndex;
   GraphicClassTag m_surfaceGraphicClass;

public:
   COdbFeatureBuildState(COdbPpDatabase& odbPpDatabase,
      int layerIndex,BlockStruct* parentBlock,CWriteFormat& log);

   COdbPpDatabase& getOdbPpDatabase() { return m_odbPpDatabase; }
   CCamCadDatabase& getCamCadDatabase() { return *m_camCadDatabase; }
   int getLayerIndex() { return m_layerIndex; }
   GraphicClassTag getSurfaceGraphicClass() { return m_surfaceGraphicClass; }
   void setSurfaceGraphicClass(GraphicClassTag surfaceGraphicClass) { m_surfaceGraphicClass = surfaceGraphicClass; }
   BlockStruct* getParentBlock() { return m_parentBlock; }
   CWriteFormat& getLog() { return m_log; }

   void addLine(const CPoint2d& startPoint,const CPoint2d& endPoint,
      int widthIndex,const CString& netName);
   void addArc(const CPoint2d& startPoint,const CPoint2d& endPoint,
      const CPoint2d& centerPoint,bool clockwise,
      int widthIndex,const CString& netName);
   void flush();
};

//_____________________________________________________________________________
enum FeatureTypeTag 
{
   lineFeature,
   arcFeature,
   padFeature,
   textFeature,
   barcodeFeature,
   surfaceFeature,
   invalidFeature
};

class COdbFeature : public CObject
{
private:
   int m_featureId;
   COdbFeatureFile* m_featureFile;
   COdbFeatureAttributeArray m_attributes;
   int m_featureFileLineNumber;

   COdbSubnet* m_subnet;
   bool m_instantiatedFlag;

public:
   COdbFeature(int featureId, COdbFeatureFile* featureFile, int featureFileLineNumber);
   COdbFeature(const COdbFeature& other);
   virtual ~COdbFeature();

   COdbFeatureAttributeArray& getAttributes() { return m_attributes; }
   COdbSubnet* getSubnet()            { return m_subnet; }
   void setSubnet(COdbSubnet* subnet) { m_subnet = subnet; }
   COdbFeatureFile* getFeatureFile()  { return m_featureFile; }
   CString getNetName();

   virtual FeatureTypeTag getFeatureType() = 0;

   virtual void addAttribute(COdbFeatureAttribute* attribute);
   void takeAttributes(COdbFeatureAttributeArray& attributeArray);
   bool getInstantiatedFlag() { return m_instantiatedFlag; }
   void setInstantiatedFlag(bool flag = true);

   virtual void instantiateCamCadData(COdbFeatureBuildState& featureBuildState) = 0;
   virtual void addDebugProperties(CCamCadDatabase& camCadDatabase,
      CAttributes** attributeMap,CWriteFormat& errorLog);

   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
class COdbInvalidFeature : public COdbFeature
{
public:
   COdbInvalidFeature(int featureId,COdbFeatureFile* featureFile,int featureFileLineNumber);
   virtual FeatureTypeTag getFeatureType() { return invalidFeature; }
   virtual void instantiateCamCadData(COdbFeatureBuildState& featureBuildState) {}
};

//_____________________________________________________________________________
class COdbLine : public COdbFeature
{
private:
   CPoint2d m_start;
   CPoint2d m_end;
   COdbFeatureSymbol* m_featureSymbol;
   bool m_positivePolarity;
   int m_dcode;

public:
   COdbLine(int featureId,COdbFeatureFile* featureFile,int featureFileLineNumber,
      double xStart,double yStart,double xEnd,double yEnd,
      COdbFeatureSymbol* symbol,bool positivePolarity,int dcode);

public:
   virtual FeatureTypeTag getFeatureType() { return lineFeature; }
   virtual void instantiateCamCadData(COdbFeatureBuildState& featureBuildState);
};

//_____________________________________________________________________________
class COdbArc : public COdbFeature
{
private:
   CPoint2d m_start;
   CPoint2d m_end;
   CPoint2d m_center;
   COdbFeatureSymbol* m_featureSymbol;
   bool m_positivePolarity;
   int m_dcode;
   bool m_clockwise;

public:
   COdbArc(int featureId,COdbFeatureFile* featureFile,int featureFileLineNumber,
      double xStart,double yStart,double xEnd,double yEnd,
      double xCenter,double yCenter,COdbFeatureSymbol* symbol,bool positivePolarity,
      int dcode,bool clockwise);

public:
   virtual FeatureTypeTag getFeatureType() { return arcFeature; }
   virtual void instantiateCamCadData(COdbFeatureBuildState& featureBuildState);
};

//_____________________________________________________________________________
class COdbPad : public COdbFeature
{
protected:
   COdbFeatureSymbol* m_featureSymbol;

private:
   CPoint2d m_origin;
   bool m_positivePolarity;
   int m_dcode;

   double m_rotation; // angle in degrees counterclockwise
   bool m_mirrorFlag;
   OdbPadUsageTag m_padUsage;

public:
   COdbPad(int featureId,COdbFeatureFile* featureFile,int featureFileLineNumber,
      double x,double y,COdbFeatureSymbol* symbol,bool positivePolarity,
      int dcode,double rotation,bool mirrorFlag);
   COdbPad(const COdbPad& other);

public:
   virtual FeatureTypeTag getFeatureType() { return padFeature; }

   CPoint2d getOrigin()           { return m_origin; }
   double getX()                  { return m_origin.x;   }
   double getY()                  { return m_origin.y;   }
   void setOrigin(const CPoint2d& origin) { m_origin = origin; }
   COdbFeatureSymbol* getFeatureSymbol() { return m_featureSymbol; }
   OdbPadUsageTag getPadUsage() { return m_padUsage; }

   double getRotation() { return m_rotation; }
   bool getMirror() { return m_mirrorFlag; }

   void addAttribute(COdbFeatureAttribute* attribute);
   void transform(CTMatrix& matrix,double deltaAngle);
   PadstackCompareStatusTag compare(const COdbPad& other) const;
   bool isEquivalent(const COdbPad& other) const;
   void buildCamCadData(COdbPpDatabase& odbPpDatabase,int layerIndex,BlockStruct* parentBlock,
      CWriteFormat& log);

   virtual void instantiateCamCadData(COdbFeatureBuildState& featureBuildState);
   //virtual void AssertValid() const;

   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
class COdbPadGeometry : public COdbPad
{
private: 
   BlockStruct* m_block;

public:
   COdbPadGeometry(const COdbPad& pad);

   double getSizeA() { return ((m_featureSymbol != NULL) ? m_featureSymbol->getDimension(0) : 0.); }
   double getSizeB() { return ((m_featureSymbol != NULL) ? m_featureSymbol->getDimension(1) : 0.); }
   CString getApertureNameDescriptor() { return ((m_featureSymbol != NULL) ? m_featureSymbol->getApertureNameDescriptor() : "NULL"); }
   ApertureShapeTag getApertureShape() { return ((m_featureSymbol != NULL) ? m_featureSymbol->getApertureShape() : apertureRound); }
   BlockStruct* getBlock() { return m_block; }
   void setBlock(BlockStruct* block) { m_block = block; }
   void buildCamCadData(COdbPpDatabase& odbPpDatabase,const CString& padApertureName,
      CWriteFormat& log);

};

//_____________________________________________________________________________
class COdbText : public COdbFeature
{
private:
   CPoint2d m_origin;
   CString m_font;
   bool m_positivePolarity;
   double m_rotation;
   bool m_mirrorFlag;
   double m_xSize;
   double m_ySize;
   double m_widthFactor;
   CString m_text;
   bool m_currentVersion;  // false - previous version

public:
   COdbText(int featureId,COdbFeatureFile* featureFile,int featureFileLineNumber,
      double x,double y,const CString& font,bool positivePolarity,
      double rotation,bool mirrorFlag,double xSize,double ySize,double widthFactor,const CString& text,
      bool currentVersion);

public:
   virtual FeatureTypeTag getFeatureType() { return textFeature; }
   virtual void instantiateCamCadData(COdbFeatureBuildState& featureBuildState);
};

//_____________________________________________________________________________
class COdbBarcode : public COdbFeature
{
public:
   COdbBarcode(int featureId,COdbFeatureFile* featureFile,int featureFileLineNumber);

public:
   virtual FeatureTypeTag getFeatureType() { return barcodeFeature; }
   virtual void instantiateCamCadData(COdbFeatureBuildState& featureBuildState) {}
};

//_____________________________________________________________________________
class COdbSurface : public COdbFeature
{
private:
   bool m_positivePolarity;
   int m_dcode;
   COdbPolygonList m_polygonList;

public:
   COdbSurface(int featureId,COdbFeatureFile* featureFile,int featureFileLineNumber,
      bool positivePolarity,int dcode);

public:
   virtual FeatureTypeTag getFeatureType() { return surfaceFeature; }

   COdbPolygon* addPolygon(double xStart,double yStart,bool holeFlag);
   virtual void instantiateCamCadData(COdbFeatureBuildState& featureBuildState);
};

//_____________________________________________________________________________
class COdbFeatureArray : public CObject
{
private:
   CTypedPtrArray<CObArray,COdbFeature*> m_featureArray;
   bool m_isContainer;

public:
   COdbFeatureArray(int size=5000,bool isContainer=true);
   ~COdbFeatureArray();
   void empty();

   COdbFeature* getAt(int index);
   int getCount() { return m_featureArray.GetSize(); }

   COdbInvalidFeature* addInvalidFeature(COdbFeatureFile* featureFile,int lineNumber);
   COdbLine* addLine(COdbFeatureFile* featureFile,int lineNumber,
      double xStart,double yStart,double xEnd,double yEnd,
      COdbFeatureSymbol* symbol,bool positivePolarity,int dcode);
   COdbArc* addArc(COdbFeatureFile* featureFile,int lineNumber,
      double xStart,double yStart,double xEnd,double yEnd,
      double xCenter,double yCenter,COdbFeatureSymbol* symbol,bool positivePolarity,
      int dcode,bool clockwise);
   COdbPad* addPad(COdbFeatureFile* featureFile,int lineNumber,
      double x,double y,COdbFeatureSymbol* symbol,bool positivePolarity,
      int dcode,double rotation,bool mirrorFlag);
   COdbText* addText(COdbFeatureFile* featureFile,int lineNumber,
      double x,double y,const CString& font,bool positivePolarity,
      double rotation,bool mirrorFlag,double xSize,double ySize,double widthFactor,const CString& text,
      bool currentVersion);
   COdbBarcode* addBarcode(COdbFeatureFile* featureFile,int lineNumber);
   COdbSurface* addSurface(COdbFeatureFile* featureFile,int lineNumber,bool positivePolarity,int dcode);

   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
class COdbPadArray : public CTypedObArrayContainer<COdbPad*>
{
public:
   COdbPadArray(int growBySize,bool isContainer) : 
      CTypedObArrayContainer<COdbPad*>(growBySize,isContainer)
   {
   }

   COdbPad* getAt(int index);
};

//_____________________________________________________________________________
class COdbFeatureFile : public CObject
{
private:
   CFilePath m_filePath;
   COdbLayer* m_layer;
   CString m_name;  // layer or symbol name
   int m_id;

   COdbFeatureSymbolArray m_featureSymbolArray;  // noncontainer
   COdbFeatureAttributes m_featureAttributes;
   COdbFeatureArray m_featureArray;

   BlockStruct* m_block;  // only used for symbol feature files

public:
   COdbFeatureFile(const CString& name,int id = -1);
   COdbFeatureFile(COdbLayer& layer,int id = -1);

   CString getName() { return m_name; }
   int getId() { return m_id; }
   int getFeatureCount() { return m_featureArray.getCount(); }
   COdbLayer* getLayer() { return m_layer; }

   bool readLayerFeatures(COdbPpDatabase& odbPpDatabase,COdbStep& step,COdbLayer& layer,
      CWriteFormat& log);
   bool readSymbolFeatures(COdbPpDatabase& odbPpDatabase,
      CWriteFormat& log);

   bool readFeatures(COdbPpDatabase& odbPpDatabase,const CString& featureFilePath,
      CWriteFormat& log);

   COdbFeature* getFeatureAt(int index) { return m_featureArray.getAt(index); }
   BlockStruct* instantiateFeatures(COdbPpDatabase& odbPpDatabase,BlockStruct* parentBlock,CWriteFormat& log);
   void loadPads(CQfePadTree& padTree,CWriteFormat& log);
   void loadUninstantiatedPads(CQfePadTree& padTree,CWriteFormat& log);
   void convertFreePadsToVias(COdbPpDatabase& odbPpDatabase, COdbStep& step, CQfePadTree& padTree, CWriteFormat& log);
   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
class COdbFeatureFileArrayWithMap : public CTypedObArrayWithMapContainer<COdbFeatureFile>
{
public:
   bool readLayerFeatures(COdbPpDatabase& odbPpDatabase,COdbStep& step,COdbLayer& layer,
      CWriteFormat& log);
   void instantiateFeatures(COdbPpDatabase& odbPpDatabase,COdbStep& step,CWriteFormat& log);
   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
typedef CTypedPtrArray<CObArray,COdbFeatureFile*> COdbFeatureFileArray;

//_____________________________________________________________________________
class CQfePad : public CQfe
{
private:
   COdbPad& m_pad;
   COdbFeatureFile& m_featureFile;

public:
   CQfePad(COdbPad& pad,COdbFeatureFile& featureFile);
   ~CQfePad();

public:
   CPoint2d getOrigin() const { return m_pad.getOrigin(); }
   CExtent getExtent() const;
   COdbPad& getPad() { return m_pad; }
   COdbFeatureFile& getFeatureFile() { return m_featureFile; }

   virtual bool isExtentIntersecting(const CExtent& extent);
   virtual bool isPointWithinDistance(const CPoint2d& point,double distance);
   virtual bool isInViolation(CObject2d& otherObject);
   virtual int getObjectType() const;
   virtual CString getInfoString() const;

public:
   //COdbPadstackTemplate* getPadstack() { return m_padstack; }
};

//_____________________________________________________________________________
class CQfePadList : public CTypedPtrList<CQfeList,CQfePad*>
{
private:
   bool m_isContainer;

public:
   CQfePadList(bool isContainer=false,int nBlockSize=200);
   ~CQfePadList();
   void empty();
};

//_____________________________________________________________________________
class CQfePadTree : public CQfeExtentLimitedContainer
{
private:

public:
   CQfePadTree();
   ~CQfePadTree();

public:
   virtual int search(const CExtent& extent,CQfePadList& foundList)
      { return CQfeExtentLimitedContainer::search(extent,foundList); }

   virtual CQfePad* findFirst(const CExtent& extent)
      { return (CQfePad*)CQfeExtentLimitedContainer::findFirst(extent); }

   virtual CQfePad* findFirstViolation(const CExtent& extent,CQfe& qfe)
      { return (CQfePad*)CQfeExtentLimitedContainer::findFirstViolation(extent,qfe); }

   virtual CQfePad* findFirstEdgeToRectViolation(const CExtent& extent,int& entitiesChecked)
      { return (CQfePad*)CQfeExtentLimitedContainer::findFirstEdgeToRectViolation(extent,entitiesChecked); }

   virtual CQfePad* findFirstEdgeToPointViolation(const CExtent& extent,
      const CPoint2d& point,double distance,int& entitiesChecked)
      { return (CQfePad*)CQfeExtentLimitedContainer::findFirstEdgeToPointViolation(extent,point,
                                                        distance,entitiesChecked); }

   //virtual int findAllViolations(const CExtent& extent,CQfe& qfe,CQfeList& foundList);

   virtual void setAt(CQfePad* qfePad)
      { CQfeExtentLimitedContainer::setAt(qfePad); }
};

//_____________________________________________________________________________
class COdbStepper : public CObject
{
private:
   CString m_stepName;
   double m_x;
   double m_y;
   double m_dx;
   double m_dy;
   int m_nx;
   int m_ny;
   double m_angle;  // in degrees
   bool m_mirrorFlag;
   bool m_flipFlag;

public:
   COdbStepper(const CString& stepName,double x,double y,double dx,double dy,
      int nx,int ny,double angle,bool mirrorFlag, bool flipFlag);

   // Returns number of PCBs inserted for this step-repeat.
   int insertSubSteps(COdbPpDatabase& odbPpDatabase, COdbStep& step, int startPcbIndx, CWriteFormat& log);
};

//_____________________________________________________________________________
class COdbStepperArray : public CTypedObArrayContainer<COdbStepper*>
{
public:
   void insertSubSteps(COdbPpDatabase& odbPpDatabase,COdbStep& step,CWriteFormat& log);
};

//_____________________________________________________________________________
class COdbProperty : public CObject
{
private:
   CString m_name;
   CString m_value;
   CString m_floatingNumbers;

public:
   COdbProperty(const CString& name,const CString& value,const CString& floatingNumbers);

   CString getName() { return m_name; }
   CString getStringValue();
   ValueTypeTag getValueType() { return valueTypeString; }
};

//_____________________________________________________________________________
class COdbPropertyList : public CTypedObListContainer<COdbProperty*>
{
public:
   void placeAttributes(CAttributes** attributeMap,
      CCamCadDatabase& camCadDatabase,CWriteFormat& log);
};

//_____________________________________________________________________________
enum PackagePinTypeTag
{
   packagePinTypeThruHole,
   packagePinTypeBlind,
   packagePinTypeSurface,
   packagePinTypeUndefined
};

enum PackagePinElectricalTypeTag
{
   packagePinElectricalTypeElectrical,
   packagePinElectricalTypeMechanical,
   packagePinElectricalTypeUndefined
};

enum PackagePinMountTypeTag
{
   pinMountTypeSmt,
   pinMountTypeSmtPadSize,
   pinMountTypeThruHole,
   pinMountTypeThruHoleHoleSize,
   pinMountTypePressfit,
   pinMountTypeNonBoard,
   pinMountTypeHole,
   pinMountTypeUndefined
};

PackagePinTypeTag stringToPackagePinTypeTag(CString tagString);
PackagePinElectricalTypeTag stringToPackagePinElectricalTypeTag(CString tagString);
PackagePinMountTypeTag stringToPackagePinMountTypeTag(CString tagString);

class COdbPackagePin : public CObject
{
private:
   int m_id;
   CString m_pinName;
   PackagePinTypeTag m_pinType;
   CPoint2d m_origin;
   double m_fhs;
   PackagePinElectricalTypeTag m_pinElectricalType;
   PackagePinMountTypeTag m_pinMountType;
   CString m_vplForm;

   COdbOutline* m_outline;

public:
   COdbPackagePin(int id,const CString& pinName,PackagePinTypeTag pinType,double x,double y,double fhs,
      PackagePinElectricalTypeTag pinElectricalType,PackagePinMountTypeTag pinMountType);
   ~COdbPackagePin();

   CString getVplForm()       { return m_vplForm; }
   void setVplForm(CString f) { m_vplForm = f;    }

   CPoint2d getOrigin() { return m_origin; }

   CString getPinName() { return m_pinName; }

   COdbCircleOutline* addCircleOutline(double xCenter,double yCenter,double radius);
   COdbSquareOutline* addSquareOutline(double xCenter,double yCenter,double halfSide);
   COdbRectangleOutline* addRectangleOutline(double lowerLeftX,double lowerLeftY,
      double width,double height);
   COdbPolygon* addContourOutline(double xStart,double yStart,bool holeFlag);
   COdbOutline* getOutline() { return m_outline; }

   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
typedef CTypedObArrayContainer<COdbPackagePin*> COdbPackagePinArray;

//_____________________________________________________________________________
class COdbPackageGeometry : public CObject
{
private:
   CString m_name;
   COdbPackage& m_package;
   COdbComponent& m_component;
   BlockStruct* m_block;

public:
   COdbPackageGeometry(const CString& name,COdbPackage& package,COdbComponent& component);

   BlockStruct* getBlock() { return m_block; }
   void setBlock(BlockStruct* block) { m_block = block; }
   CString getName() { return m_name; }
   COdbPackage& getPackage() { return m_package; }
   COdbComponent& getComponent() { return m_component; }

   void buildCamCadData(COdbPpDatabase& odbPpDatabase,COdbStep& step,CWriteFormat& log);
};

//_____________________________________________________________________________
typedef CTypedObListContainer<COdbPackageGeometry*> COdbPackageGeometries;

//_____________________________________________________________________________
enum mountTypeTag { mtOther=0 , mtSmt=1 , mtThruHole=2 , mtUndefined = -1 };

class COdbPackage : public CObject
{
private:
   int m_id;
   CString m_originalName;
   CString m_validatedName;
   double m_pinPitch;
   double m_boundingBoxXmin;
   double m_boundingBoxYmin;
   double m_boundingBoxXmax;
   double m_boundingBoxYmax;

   COdbOutline* m_outline;
   COdbPropertyList m_propertyList;
 
   COdbPackagePinArray m_pins;
   COdbPackageGeometries m_packageGeometries;

   static int m_attachPadStackSuccessCount;
   static int m_attachPadStackFailureCount;

public:
   COdbPackage(int id, const CString& originalName, const CString& validatedName, double pinPitch,
      double boundingBoxXmin,double boundingBoxYmin,
      double boundingBoxXmax,double boundingBoxYmax);
   ~COdbPackage();

   CString getValidatedName() { return m_validatedName; }
   CString getOriginalName() { return m_originalName; }
   int getNumPins() { return m_pins.GetCount(); }

   COdbPackagePin* addPin(const CString& pinName,PackagePinTypeTag pinType,double x,double y,double fhs,
      PackagePinElectricalTypeTag pinElectricalType,PackagePinMountTypeTag pinMountType);
   COdbPackagePin* getPinAt(int pinIndex);
   void addProperty(COdbProperty* property);
   COdbCircleOutline* addCircleOutline(double xCenter,double yCenter,double radius);
   COdbSquareOutline* addSquareOutline(double xCenter,double yCenter,double halfSide);
   COdbRectangleOutline* addRectangleOutline(double lowerLeftX,double lowerLeftY,
      double width,double height);
   COdbPolygon* addContourOutline(double xStart,double yStart,bool holeFlag);
   COdbPackageGeometry& getPackageGeometryFor(COdbPpDatabase& odbPpDatabase,
      COdbStep& step,COdbComponent& component,CWriteFormat& log);
   COdbOutline* getOutline() { return m_outline; }
   void buildCamCadData(COdbPpDatabase& odbPpDatabase, BlockStruct* parentBlock, const bool mirrorPackage);
   void fillDftOutlinePoly(COdbPpDatabase& odbPpDatabase, CPoly &dftOutlinePoly);

   void dump(CWriteFormat& writeFormat,int depth = -1);
   static void report(CWriteFormat& writeFormat);

private:
};

//_____________________________________________________________________________
class COdbPackages : public CObject
{
private:
   int m_pkgIndexOffset;  // Difference between actual index in ODB data and index in this array
   CString m_fileSourceDescription;  // EDA/DATA or EDA/VPL_PKGS
   CTypedObArrayContainer<COdbPackage*> m_packages;

public:
   COdbPackages(CString desc)    { m_fileSourceDescription = desc; m_pkgIndexOffset = 0; }
   int getNumPackages() { return m_packages.GetSize(); }
   COdbPackage* getAtRawIndex(int index);  // Directly indexing the local storage, index starts at 0
   COdbPackage* getAtOdbIndex(int index);  // Indexed by ODB++ CAD reference, we split these to two lists, so need to apply offset for one of them.
   COdbPackage* addPackage(const CString& originalName, const CString& validatedName, double pinPitch,
      double boundingBoxXmin, double boundingBoxYmin,
      double boundingBoxXmax, double boundingBoxYmax);
   void dump(CWriteFormat& writeFormat,int depth = -1);

   int getPkgIndxOffset()              { return m_pkgIndexOffset; }
   void setPkgIndexOffset(int offset)  { m_pkgIndexOffset = offset; }
};

//_____________________________________________________________________________
enum FeatureIdTypeTag
{
   featureTypeCopper,
   featureTypeLaminate,
   featureTypeHole,
   featureTypeUndefined
};

FeatureIdTypeTag stringToFidTypeTag(CString tagString);

class COdbFeatureId : public CObject
{
private:
   FeatureIdTypeTag m_fidType;
   int m_layerNumber;  // 0 based index of layer names in LYR record - COdbStep::m_layerArray
   int m_featureNumber;
   int m_lineNumber;

   COdbFeature* m_feature;

public:
   COdbFeatureId(FeatureIdTypeTag fidType,int layerNumber,int featureNumber,int lineNumber);

   int getLayerNumber() { return m_layerNumber; }
   int getFeatureNumber() { return m_featureNumber; }
   int getLineNumber() { return m_lineNumber; }
   COdbFeature* getFeature() { return m_feature; }
   void setFeature(COdbFeature* feature) { m_feature = feature; }
   CString getDescriptor();

   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
typedef CTypedObListContainer<COdbFeatureId*> COdbFidList;

//_____________________________________________________________________________
enum SubnetTypeTag
{
   subnetToeprint,
   subnetVia,
   subnetTrace,
   subnetPlane,
   subnetUndefined
};

class COdbSubnet : public CObject
{
private:
   int m_id;
   COdbFidList m_fidList;

   COdbNet* m_net;

public:
   COdbSubnet(int id);
   virtual ~COdbSubnet();

   virtual SubnetTypeTag getSubnetType() = 0;

   COdbFeatureId* addFeatureId(FeatureIdTypeTag fidType,int layerNumber,int featureNumber,int lineNumber);
   bool linkFeatures(COdbFeatureFileArray& featureFileArray,CWriteFormat& log);
   COdbFidList& getFidList() { return m_fidList; }
   int getId() const { return m_id; }
   COdbNet* getNet() const { return m_net; }
   void setNet(COdbNet* net) { m_net = net; }
   CString getNetName();
   void placeAttributes(CAttributes** attributeMap,
      CCamCadDatabase& camCadDatabase,CWriteFormat& log);
   CString getDescriptor() const;

   virtual void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
class COdbSubnetArray : public CTypedObArrayContainer<COdbSubnet*>
{
private:
   CMapStringToInt SubnetIDMap;
public:
   void dump(CWriteFormat& writeFormat,int depth = -1);
   CMapStringToInt & getSubnetIDMap(){return SubnetIDMap;}
};

//_____________________________________________________________________________
class COdbToeprintSubnet : public COdbSubnet
{
private:
   bool m_topSideFlag;
   int m_componentId;  // zero based
   int m_pinId;        // zero based

   COdbComponentPin* m_odbComponentPin;

public:
   COdbToeprintSubnet(int id,bool topSideFlag,int componentId,int pinId);

   virtual SubnetTypeTag getSubnetType() { return subnetToeprint; }
   COdbComponentPin* getComponentPin() { return m_odbComponentPin; }

   bool linkComponentPin(COdbComponentsFile& topComponentsFile,
      COdbComponentsFile& bottomComponentsFile,CWriteFormat& log);
};

//_____________________________________________________________________________
typedef CTypedPtrArray<CObArray,COdbToeprintSubnet*> COdbToeprintSubnetArray;

//_____________________________________________________________________________
class COdbViaSubnet : public COdbSubnet
{
public:
   COdbViaSubnet(int id);

   virtual SubnetTypeTag getSubnetType() { return subnetVia; }
};

//_____________________________________________________________________________
class COdbTraceSubnet : public COdbSubnet
{
public:
   COdbTraceSubnet(int id);

   virtual SubnetTypeTag getSubnetType() { return subnetTrace; }
};

//_____________________________________________________________________________
enum PlaneFillTypeTag
{
   planeFillSolid,
   planeFillHatched,
   planeFillOutline,
   planeFillUndefined
};

enum CutoutTypeTag
{
   cutoutCircle,
   cutoutRectangle,
   cutoutOctagon,
   cutoutExact,
   cutoutUndefined
};

PlaneFillTypeTag stringToPlaneFillTypeTag(CString tagString);
CutoutTypeTag stringToCutoutTypeTag(CString tagString);

class COdbPlaneSubnet : public COdbSubnet
{
private:
   PlaneFillTypeTag m_fillType;
   CutoutTypeTag m_cutoutType;
   double m_fillSize;

public:
   COdbPlaneSubnet(int id,PlaneFillTypeTag fillType,CutoutTypeTag cutoutType,double fillSize);

   virtual SubnetTypeTag getSubnetType() { return subnetPlane; }
};

//_____________________________________________________________________________
class COdbLayerArray : public CTypedPtrArray<CObArray,COdbLayer*>
{
public:
   CString getDescriptor() const;
};

//_____________________________________________________________________________
typedef CTypedObListContainer<COdbPadstackTemplate*> COdbPadstackTemplateListContainer;

//_____________________________________________________________________________
class COdbNet : public CObject
{
private:
   int m_netId;
   CString m_netName;
   int m_subnetArrayGrowLimit;
   COdbSubnetArray m_subnetArray;
   COdbPadstackTemplateListContainer* m_padstackList;

public:
   COdbNet(int netId,const CString& netName);
   ~COdbNet();

   COdbSubnetArray& getSubnetArray() { return m_subnetArray; }
   int getNetId() { return m_netId; }
   CString getNetName() { return m_netName; }

   void addSubnet(COdbSubnet* subnet);
   COdbToeprintSubnet* addToeprintSubnet(bool topSideFlag,int componentId,int pinId);
   COdbViaSubnet* addViaSubnet();
   COdbTraceSubnet* addTraceSubnet();
   COdbPlaneSubnet* addPlaneSubnet(PlaneFillTypeTag planeFillType,CutoutTypeTag cutoutType,double fillSize);
   bool linkFeatures(COdbFeatureFileArray& featureFileArray,CWriteFormat& log,
      COperationProgress& operationProgress);
   bool linkComponentPins(COdbComponentsFile& topComponentsFile,
      COdbComponentsFile& bottomComponentsFile,CWriteFormat& log);
   bool buildCamCadData(COdbPpDatabase& odbPpDatabase,COdbStep& step,CWriteFormat& log);

   void addViaPadstack(COdbPadstackTemplate* padstack);
   bool instantiateVias(COdbPpDatabase& odbPpDatabase,COdbStep& step,
      COperationProgress& operationProgress,CWriteFormat& log);

   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
class COdbNetArray : public CTypedObArrayContainer<COdbNet*>
{
private:
   int m_subnetCount;

public:
   COdbNetArray();

   int getSubnetCount();
   COdbNet* addNet(const CString& netName);
   bool linkFeatures(COdbFeatureFileArrayWithMap& featureFileArray,COdbLayerArray& layerArray,
      CWriteFormat& log);
   bool linkComponentPins(COdbComponentsFile& topComponentsFile,
      COdbComponentsFile& bottomComponentsFile,CWriteFormat& log);
   bool instantiateVias(COdbPpDatabase& odbPpDatabase,COdbStep& step,
      CQfePadTree& padTree,CWriteFormat& log);
   bool buildCamCadData(COdbPpDatabase& odbPpDatabase,COdbStep& step,CWriteFormat& log);

   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
class COdbComponentPin : public CObject
{
private:
   COdbComponent& m_component;
   int m_pinId;
   int m_pinNumber;
   CPoint2d m_origin;  // board coordinates
   double m_rotation;  // angle in degrees counterclockwise
   bool m_mirrorFlag;
   int m_netNumber;
   int m_subnetNumber;
   CString m_pinName;

   //COdbToeprintSubnetArray m_subnetArray;
   COdbToeprintSubnet* m_subnet;

   CPoint2d m_padstackOffsetFromPackagePin;  
   COdbPadstackGeometry* m_padstackGeometry;

public:
   COdbComponentPin(COdbComponent& component,int pinId,int pinNumber,
      double x,double y,double rotation,bool mirrorFlag,
      int netNumber,int subnetNumber,const CString& pinName);

   int getPinId() { return m_pinId; }
   int getPinNumber() { return m_pinNumber; }
   CString getPinName() { return m_pinName; }
   CString getPinNumberString() { CString retval;  retval.Format("%d",m_pinNumber);  return retval; }
   CString getPinReference() { return (m_pinName == "" ? getPinNumberString() : m_pinName); }
   CPoint2d getOrigin() { return m_origin; }
   double getRotationDegrees()   { return m_rotation; }
   bool getMirrored()            { return m_mirrorFlag; }
   CPoint2d getPadstackOffsetFromPackagePin() { return m_padstackOffsetFromPackagePin; }
   //void addSubnet(COdbToeprintSubnet* subnet) { m_subnetArray.Add(subnet); }
   void addSubnet(COdbToeprintSubnet* subnet) { m_subnet = subnet; }
   COdbToeprintSubnet* getSubnet() { return m_subnet; }
   void setPadstackGeometry(COdbPadstackGeometry* padstackGeometry,CPoint2d stackOffset);
   COdbPadstackGeometry* getPadstackGeometry() { return m_padstackGeometry; }
   COdbComponent& getComponent() { return m_component; }
   bool padStacksMatch(const COdbComponentPin& other);
   COdbPad* getFirstSubnetPadForLayerNumber(int layerNumber);

   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
class COdbComponentPinArray : public CTypedObArrayContainer<COdbComponentPin*>
{
public:
   COdbComponentPin* getAt(int index);

   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
class COdbComponent : public CObject
{
private:
   int m_componentId;   // the same index starts with 0 for top and bottom
   int m_compsPackageId;  // Pkg ID from components file, essentially the CAD pkg ID.
   int m_comps3PackageId; // Pkg ID from the components3 file, may be a CAD pkg or VPL pkg.
   CPoint2d m_origin;   // location fromm components file
   double m_rotation;   // angle in degrees, counter-clockwise, from components file
   CPoint2d m_comps3Origin;  // location from components3 file (may be different than in components file when vpl pkgs are used)
   double m_comps3Rotation;  // from components3 file
   bool m_topFlag;
   bool m_mirrorFlag;
   CString m_refDes;
   CString m_partNumber;
   CString m_customerPartNumber;
   CString m_internalPartNumber;
   CString m_packageName;  // BOM info, not related to COdbPackage
   CStringArray* m_descriptions;
   CStringArray* m_vplVendorNames;
   CStringArray* m_vplManufacturerPartNumbers;
   CStringArray* m_vendorNames;
   CStringArray* m_manufacturerPartNumbers;

   COdbComponentPinArray m_pinArray;
   COdbPropertyList m_propertyList;
   COdbFeatureAttributeArray m_attributes;

   // links
   COdbPackage* m_package;

   DataStruct *m_compInsertData;  // Gets set when insert is created, is NULL up until then.

public:
   COdbComponent(int componentId,int compsPackageId,double x,double y,double rotation,bool topFlag,bool mirrorFlag,
      const CString& refDes,const CString& partNumber);
   ~COdbComponent();

   DataStruct *getCompInsertData()        { return m_compInsertData; }
   void setCompInsertData(DataStruct *d)  { m_compInsertData = d;    }

   CPoint2d getOrigin() { return m_origin; }
   double getRotation() { return m_rotation; }

   CPoint2d getComps3Origin() { return m_comps3Origin; }
   double getComps3Rotation() { return m_comps3Rotation; }
   void setComps3Origin(CPoint2d xy)     { m_comps3Origin = xy; }
   void setComps3Rotation(double rotDeg) { m_comps3Rotation = rotDeg; }

   CString getRefDes() { return m_refDes; }
   bool getMirrorFlag() { return m_mirrorFlag; }
   bool IsPlacedTop() { return m_topFlag; }
   
   int getComponentId() { return m_componentId; }

   int  getCompsPackageId()        { return m_compsPackageId; }
   int  getComps3PackageId()       { return m_comps3PackageId; }
   void setComps3PackageId(int id) { m_comps3PackageId = id; }

   int getNumPins() { return m_pinArray.GetSize(); }
   COdbComponentPin* getPin(int pinId) { return m_pinArray.getAt(pinId); }
   CString getPartNumber()          { return m_partNumber; }
   void setPartNumber(CString pn)   { m_partNumber = pn; }
   void setPackage(COdbPackage* package) { m_package = package; }
   COdbPackage* getPackage() { return m_package; }

   void addCustomerPartNumber(const CString& customerPartNumber) { m_customerPartNumber = customerPartNumber; }
   void addInternalPartNumber(const CString& internalPartNumber) { m_internalPartNumber = internalPartNumber; }
   void addPackageName(const CString& packageName) { m_packageName = packageName; }
   void addDescription(const CString& description);
   void addVplVendorName(const CString& vplVendorName);
   void addVplManufacturerPartNumber(const CString& vplManufacturerPartNumber);
   void addVendorName(const CString& vendorName);
   void addManufacturerPartNumber(const CString& manufacturerPartNumber);

   COdbFeatureAttributeArray& getAttributes() { return m_attributes; }
   COdbPropertyList& getProperties() { return m_propertyList; }
   void addPin(int pinNumber,double x,double y,double rotation,bool mirrorFlag,
      int netNumber,int subnetNumber,const CString& pinName);
   void addProperty(COdbProperty* property);
   bool padStacksMatch(const COdbComponent& other);
   bool inferComponentPinPositionFromFeatures(int pinId,CPoint2d& pinPosition);
   void generatePackagePinReport(COdbStep& step,CWriteFormat& report);
   CPoint2d getPadstackOriginRelativeToPackageForPin(int pinId);

   void placeAttributes(CAttributes** attributeMap,
      CCamCadDatabase& camCadDatabase, CWriteFormat& log);

   void placeAttributes(CAttributes** attributeMap,
      CCamCadDatabase& camCadDatabase,CWriteFormat& log,CStringArray* values,const CString& namePrefix);

   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
class COdbComponents : public CTypedObArrayWithMapContainer<COdbComponent>
{
public:
   COdbComponents();
   ~COdbComponents();

   COdbComponent* addComponent(int packageId,double x,double y,double rotation,bool topFlag,
      bool mirrorFlag,const CString& refDes,const CString& partNumber);

   void generatePackagePinReport(COdbStep& step,CWriteFormat& report);
   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
class COdbComponentsFile : public CObject
{
private:
   CFilePath m_filePath;

   COdbFeatureSymbolArray m_featureSymbolArray;  // noncontainer
   COdbFeatureAttributes m_featureAttributes;
   COdbComponents m_components;
   bool m_topFlag;

public:
   COdbComponentsFile(bool topFlag);

   int getSize()                                { return m_components.getSize(); }
   COdbComponent* getComponent(int componentId) { return m_components.getAt(componentId); }
   CString getName() { return m_filePath.getFileName(); }
   bool getTopFlag() { return m_topFlag; }

   bool readComponents(COdbPpDatabase& odbPpDatabase,COdbStep& step,COdbLayer& layer,
      bool topComponentLayerFlag,CWriteFormat& log);
   bool readComponents2(COdbPpDatabase& odbPpDatabase,COdbStep& step,COdbLayer& layer,
      bool topComponentLayerFlag,CWriteFormat& log);
   bool readComponents3(COdbPpDatabase& odbPpDatabase,COdbStep& step,COdbLayer& layer,
      bool topComponentLayerFlag,CWriteFormat& log);

   bool linkPackages(COdbPpDatabase& odbPpDatabase,COdbStep& step,CWriteFormat& log);
   bool instantiateAndAttachPadStacks(COdbPpDatabase& odbPpDatabase,
      COdbStep& step,CQfePadTree& padTree,CWriteFormat& log);
   bool createPcbComponentInserts(COdbPpDatabase& odbPpDatabase,
      COdbStep& step,CWriteFormat& log);
   int attachPadStacksToUnconnectedPins(COdbPpDatabase& odbPpDatabase,
      COdbStep& step,CQfePadTree& padTree,CWriteFormat& log);

   void DeterminePadSurfacesForPadstack(CQfePadList &foundList, COdbComponent *component, bool &addTopPads, bool &addBotPads);

   void generatePackagePinReport(COdbStep& step,CWriteFormat& report)
      { m_components.generatePackagePinReport(step,report); }
   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
class COdbBomCp : public CObject
{
private:
   CString m_cpn;     // customer part number
   CString m_ipn;     // internal part number
   CStringArray m_dscs; // Descriptions

public:
   COdbBomCp(CString cpn);

   CString GetCPN()  { return m_cpn; }

   CString GetIPN()           { return m_ipn; }
   void SetIPN(CString ipn)   { m_ipn = ipn; }

   void AddDsc(CString dsc)   { m_dscs.Add(dsc); }
   int GetDscCount()          { return m_dscs.GetCount(); }
   CString GetDsc(int indx)   { return (indx >= 0 && indx < m_dscs.GetCount()) ? m_dscs.GetAt(indx) : ""; }

};

//---------------------------------------------------------------

class COdbBomRdCpn : public CObject
{
private:
   CString m_refdes;
   CString m_cpn;
   bool m_nopop; // Opposite of loaded

public:
   COdbBomRdCpn(CString refdes)     { m_refdes = refdes; m_nopop = false; }

   void SetCPN(CString cpn)         { m_cpn = cpn; }
   void SetNoPop(bool flag)         { m_nopop = flag; }

   CString GetRefdes()              { return m_refdes; }
   CString GetCPN()                 { return m_cpn; }
   bool GetNoPop()                  { return m_nopop; }
};

//---------------------------------------------------------------

class COdbBom : public CObject
{
private:
   CTypedObArrayWithMapContainer<COdbBomCp>     m_bomCpMap;
   CTypedObArrayWithMapContainer<COdbBomRdCpn>  m_bomRdCpnMap;

   // String table of description aliases.
   CStringArray m_descriptionAliasTable;

   // Indices for specific fields within descriptions, i.e. mapping of DESC_ALIASES.
   int m_devicewTypeIndx;
   int m_valueIndx;
   int m_posTolIndx;
   int m_negTolIndx;
   int m_subclassIndx;
   int m_pinMappingsIndx;
   int m_ipnDescIndx;
   int m_cpnDescIndx;
   int m_plainDescIndx;

public:
   COdbBom();
   //~COdbBom();

   bool IsEmpty()    { return ((m_bomCpMap.getSize() < 1) && (m_bomRdCpnMap.getSize() < 1)); }

   void AddCp(COdbBomCp *bomCp) { if (bomCp != NULL) m_bomCpMap.add(bomCp->GetCPN(), bomCp); }
   COdbBomCp* GetCp(CString cpn) { COdbBomCp *bcp = NULL; m_bomCpMap.lookup(cpn, bcp); return bcp; }

   void AddRdCpn(COdbBomRdCpn *bomRdCpn) { if (bomRdCpn != NULL) m_bomRdCpnMap.add(bomRdCpn->GetRefdes(), bomRdCpn); }
   COdbBomRdCpn* GetRdCpn(CString refdes) { COdbBomRdCpn *bcp = NULL; m_bomRdCpnMap.lookup(refdes, bcp); return bcp; }

   void SetDescriptionIndex(CString description, int odbIndx);
   bool IsMappedDescriptionIndex(int indx);
   //int GetDeviceTypeDscIndx()       { return m_devicewTypeIndx; }
   //int GetValueDscIndx()            { return m_valueIndx;       }
   //int GetPosTolDscIndx()           { return m_posTolIndx;      }
   //int GetNegTolDscIndx()           { return m_negTolIndx;      }
   //int GetSubclassDscIndx()         { return m_subclassIndx;    }
   //int GetPinMappingDscIndx()       { return m_pinMappingsIndx; }
   //int GetIpnDescriptionDscIndx()   { return m_ipnDescIndx;     }
   //int GetCpnDescriptionDscIndx()   { return m_cpnDescIndx;     }

   CString GetDescriptionAlias(int indx) { return (indx >= 0 && indx < m_descriptionAliasTable.GetCount()) ? m_descriptionAliasTable.GetAt(indx) : ""; }

   CString GetDeviceType(COdbBomCp *cp) { return (cp != NULL) ? cp->GetDsc(m_devicewTypeIndx) : ""; }
   CString GetValue(COdbBomCp *cp)      { return (cp != NULL) ? cp->GetDsc(m_valueIndx) : "";       }
   CString GetPTol(COdbBomCp *cp)       { return (cp != NULL) ? cp->GetDsc(m_posTolIndx) : "";      }
   CString GetNTol(COdbBomCp *cp)       { return (cp != NULL) ? cp->GetDsc(m_negTolIndx) : "";      }
   CString GetSubclass(COdbBomCp *cp)   { return (cp != NULL) ? cp->GetDsc(m_subclassIndx) : "";    }
   CString GetPinMapping(COdbBomCp *cp) { return (cp != NULL) ? cp->GetDsc(m_pinMappingsIndx) : ""; }
   CString GetIpnDesc(COdbBomCp *cp)    { return (cp != NULL) ? cp->GetDsc(m_ipnDescIndx) : "";     }
   CString GetCpnDesc(COdbBomCp *cp)    { return (cp != NULL) ? cp->GetDsc(m_cpnDescIndx) : "";     }
   CString GetPlainDesc(COdbBomCp *cp)  { return (cp != NULL) ? cp->GetDsc(m_plainDescIndx) : "";   }

   CString GetValidatedDeviceType(COdbBomCp *cp);

   void ExtractPinMap(const CString& pinMapStr, CMapStringToString &pinRefToPinNameMap, CString &errMsg);
};

//_____________________________________________________________________________
class COdbStep : public CObject
{
private:
   int m_column;
   CString m_name;
   CPoint2d m_datumPoint;
   CPoint2d m_originPoint;
   CString m_affectingBOM;

   COdbPackages m_edaDataPackages;
   COdbPackages m_edaVplPackages;
   COdbStepperArray m_stepperArray;
   CStringArray m_netNamesArray;
   COdbFeatureAttributes m_netAttributes;

   // for BOM file
   COdbBom m_bom;

   // for "components" file
   COdbFeatureFileArrayWithMap m_featureFileArray;
   COdbComponentsFile m_topComponentsFile;
   COdbComponentsFile m_bottomComponentsFile;

   // non-container for layers contained in COdbPpDatabase::m_layerArray
   // indexed by order of layerName in LYR record of EDA data file
   COdbLayerArray m_layerArray;

   COdbNetArray m_netArray;

   // CamCad links
   FileStruct* m_file;
   BlockStruct* m_block;

   // cache
   int m_drillLayerIndex;

   // 
   CQfePadTree m_padTree;

public:
   COdbStep(const CString& name,int column);
   ~COdbStep();
   void empty();

   CString getName() { return m_name; }
   FileStruct* getFile() { return m_file; }
   int getFileNum() { return m_file->getFileNumber(); }
   BlockStruct* getBlock() { return m_block; }

   COdbBom &getBom() { return m_bom; }
   void ApplyBom(COdbPpDatabase& odbPpDatabase, CWriteFormat& log);
   bool HasPartNumbers(COdbPpDatabase& odbPpDatabase);
   void SetLoadedBasedOnPartnumber(COdbPpDatabase& odbPpDatabase);
   void SetLoaded(COdbPpDatabase& odbPpDatabase, bool loadedValue);

   void ApplyVplPkgs(COdbPpDatabase& odbPpDatabase, COdbComponentsFile& odbComponentsFile, CWriteFormat& log);
   void GenerateRealParts(COdbPpDatabase& odbPpDatabase, COdbPackages &odbPkgList, CWriteFormat& log);
   void CreateRealPartInsert(COdbPpDatabase& odbPpDatabase, COdbComponent* component, bool topFlag, CWriteFormat& log);

   CPoint2d getDatumPoint() const { return m_datumPoint; }
   void setDatumPoint(const CPoint2d& datumPoint) { m_datumPoint = datumPoint; }

   CPoint2d getOriginPoint() const { return m_originPoint; }
   void setOriginPoint(const CPoint2d& originPoint) { m_originPoint = originPoint; }

   CString getAffectingBomName()					{ return m_affectingBOM; }
   void setAffectingBomName(CString affectingBOM)	{ m_affectingBOM = affectingBOM; }

   COdbPackage* getEdaDataPackage(int packageId) { return m_edaDataPackages.getAtOdbIndex(packageId); }
   COdbPackage* getEdaVplPackage(int packageId)  { return m_edaVplPackages.getAtOdbIndex(packageId); }
   int getEdaDataPackageCount()     { return m_edaDataPackages.getNumPackages(); }
   int getEdaVplPackageCount()      { return m_edaVplPackages.getNumPackages(); }

   COdbLayer* getLayer(int layerNumber) { return m_layerArray.GetAt(layerNumber); }
   int getLayerNumberWithType(LayerTypeTag layerType);
   int getDrillLayerIndex();

   void readStep(COdbPpDatabase& odbPpDatabase,bool artworkModeFlag,CWriteFormat& log);
   bool buildCamCadBoard(COdbPpDatabase& odbPpDatabase,CWriteFormat& errorLog);
   bool readEdaData(COdbPpDatabase& odbPpDatabase,bool artworkModeFlag,CWriteFormat& log);
   bool readEdaVplPkgs(COdbPpDatabase& odbPpDatabase,bool artworkModeFlag,CWriteFormat& log);
   bool readBom(COdbPpDatabase& odbPpDatabase,bool artworkModeFlag,CWriteFormat& log);
   bool readLayers(COdbPpDatabase& odbPpDatabase,bool artworkModeFlag,CWriteFormat& log);
   bool readStepHeader(COdbPpDatabase& odbPpDatabase,COdbStep& step,CWriteFormat& log);
   void loadPadTree(CWriteFormat& log);
   int attachPadStacksToUnconnectedPins(COdbPpDatabase& odbPpDatabase,CWriteFormat& log);
   bool convertFreePadsToVias(COdbPpDatabase& odbPpDatabase,CWriteFormat& errorLog);

private:
   void addStepper(const CString& stepName,double x,double y,double dx,double dy,
      int nx,int ny,double angle,bool mirrorFlag, bool flipFlag);
   CString checkInvalidPackageName(const CString packageName, int nextPkgIndex, CWriteFormat& log);

public:
   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
//typedef CTypedObArrayContainer<COdbStep*> COdbStepArray;
class COdbStepArray
{
private:
   CTypedObArrayWithMapContainer<COdbStep> m_stepArray;

public:
   void empty();
   COdbStep* getAt(const CString& stepName) const;
   COdbStep* getAt(int index) const;
   int getSize() const;
   int AddStep(const CString& stepName,COdbStep* step);
};

//_____________________________________________________________________________
enum OdbLayerTypeTag 
{ 
   odbLayerTypeSignal, odbLayerTypePower, odbLayerTypeMixed, odbLayerTypeSolderMask, odbLayerTypeSolderPaste,
   odbLayerTypeSilkscreen, odbLayerTypeDrill, odbLayerTypeRout, odbLayerTypeDocument, odbLayerTypeComponent,
   odbLayerTypeProfile  ,
   odbLayerTypeUndefined
};

OdbLayerTypeTag stringToOdbLayerTypeTag(CString layerTypeString);

class COdbLayer : public CObject
{
private:
   int m_row;
   bool m_boardContext;
   OdbLayerTypeTag m_type;
   CString m_name;
   CString m_oldName;
   bool m_positivePolarity;
   CString m_startLayerName;
   CString m_endLayerName;

   // CamCad links
   COdbLayer* m_mirrorLayer;
   LayerStruct* m_layerStruct;

public:
   COdbLayer(const CString& name,int row,bool boardContext,OdbLayerTypeTag layerType,
      const CString& oldName,bool positivePolarity,
      const CString& startLayerName,const CString& endLayerName);

   int getRow() { return m_row; }
   CString getName() { return m_name; }
   OdbLayerTypeTag getType() { return m_type; }
   bool getBoardContext() { return m_boardContext; }
   bool getPositivePolarity() { return m_positivePolarity; }

   // CamCad links
   LayerStruct* getCCZLayerStruct() { return m_layerStruct; }
   int getCCZLayerIndex()           { return m_layerStruct != NULL ? m_layerStruct->getLayerIndex() : -1; }
   LayerTypeTag getCCZLayerType()   { return m_layerStruct != NULL ? m_layerStruct->getLayerType()  : layerTypeUnknown; }
   COdbLayer* getODBMirrorLayer() { return m_mirrorLayer; }
   void setODBMirrorLayer(COdbLayer* mirrorLayer);

   void unsetMirrorLayer();
   void setLayerStruct(LayerStruct* layerStruct) { m_layerStruct = layerStruct; }
};

//_____________________________________________________________________________
class COdbLayerArrayWithMap : CObject
{
private:
   CTypedObArrayWithMapContainer<COdbLayer> m_layers;

public:
   COdbLayerArrayWithMap() : m_layers(20) {}
   void empty() { m_layers.empty(); }

   COdbLayer* getAt(int index) { return m_layers.getAt(index); }
   COdbLayer* getAt(const CString& layerName) { return m_layers.getAt(properCase(layerName)); }
   int getSize() { return m_layers.getSize(); }
   void setAt(COdbLayer* layer);
};

//_____________________________________________________________________________
class COdbWheelDcode : public CObject
{
private:
   int m_dcode;
   CString m_symbolName;

public:
   COdbWheelDcode(int dcode,const CString& symbolName);
};

//_____________________________________________________________________________
typedef CTypedObArrayContainer<COdbWheelDcode*> COdbWheelDcodeArray;

//_____________________________________________________________________________
class COdbWheel : public CObject
{
private:
   CString m_name;
   COdbWheelDcodeArray m_wheelDcodes;

public:
   COdbWheel(const CString& name);

   bool readDcodes(const CString& dcodesFilePath,CWriteFormat& log);
};

//_____________________________________________________________________________
class COdbWheels : public CTypedObArrayWithMapContainer<COdbWheel>
{
public:
   COdbWheel* addWheel(const CString& wheelName);
};

//_____________________________________________________________________________
enum PadstackReportColumnTag
{
   reportColumnName = 0,
   reportColumnUseCount = 1,
   reportColumnMatchDescription = 2,
   reportColumnLayers = 3
};

class CPadstackReportColumns : public CObject
{
private:
   int m_numLayers;
   CWordArray m_columnWidths;

public:
   CPadstackReportColumns(int numLayers);

   void updateColumnWidth(PadstackReportColumnTag columnTag,int width);
   void updateColumnWidth(int layerIndex,int width);
   int getColumnWidth(PadstackReportColumnTag columnTag) 
      { return m_columnWidths.GetAt(columnTag); }
   int getColumnWidth(int layerIndex) 
      { return m_columnWidths.GetAt(layerIndex + reportColumnLayers); }
      int getNumLayers() { return m_numLayers; }
};

//_____________________________________________________________________________
class COdbPadstack : public CObject
{
protected:
   COdbPadArray m_padArray;
   int m_count;

public:
   COdbPadstack(int growBySize);
   void empty();

   int getSize() { return m_padArray.GetSize(); }
   COdbPad* getAt(int index) { return m_padArray.getAt(index); }
   COdbPad* removeAt(int index);
   double getDrillSize(int drillLayerIndex);
   int getCount();

   PadstackCompareStatusTag compare(const COdbPadstack& other) const;
   bool isEquivalent(const COdbPadstack& other) const;

   void dump(CWriteFormat& writeFormat,int depth = -1);

protected:
   void setAt(int index,COdbPad* pad);
};

//_____________________________________________________________________________
class COdbPadstackTemplate : public COdbPadstack
{
private:
   CPoint2d m_origin;
   COdbFeatureAttributeArray m_attributes;

public:
   COdbPadstackTemplate();

   COdbFeatureAttributeArray& getAttributes() { return m_attributes; }
   bool add(COdbPad* featurePad,int layerIndex,CTMatrix& matrix,double deltaAngle,CWriteFormat& log);
   CPoint2d getCenterPadOrigin();  // returns origin of pad closest to the centroid of all pads
   CPoint2d centerPads();  // returns offset subtracted from each pad (this call)
   CPoint2d getOrigin() { return m_origin; } // returns cumulative padoffsets.
   bool conditionallyTakePads(COdbPadstackTemplate& otherPadstack,CWriteFormat& log);
   bool addCoincidentFreePads(COdbPad* featurePad, CQfePadTree& padTree, CWriteFormat& log);
};

//_____________________________________________________________________________
class COdbPadstackGeometry : public COdbPadstack
{
private:
   BlockStruct* m_block;
   PadstackCompareStatusTag m_closestMatchStatus;
   CString m_closestMatchPadstackName;
   int m_useCount;

public:
   COdbPadstackGeometry(PadstackCompareStatusTag closestMatchStatus = padstackCompareUndefined,
      const CString& closestMatchPadstackName="");

   void setAt(int index,COdbPadGeometry* padGeometry)
      { COdbPadstack::setAt(index,padGeometry); }
   BlockStruct* getBlock() { return m_block; }
   void setBlock(BlockStruct* block) { m_block = block; }
   CString getName() { return (m_block != NULL ? m_block->getName() : ""); }
   PadstackCompareStatusTag getClosestMatchStatus() { return m_closestMatchStatus; }
   CString getClosestMatchPadstackName() { return m_closestMatchPadstackName; }
   int incrementUseCount() { return ++m_useCount; }
   CString getMatchDescription() 
      { return m_closestMatchPadstackName + ":" + 
               padstackCompareStatusTagToString(m_closestMatchStatus); }

   void updateReportWidths(CPadstackReportColumns& reportColumns);
   void writeReport(CWriteFormat& report,CPadstackReportColumns& reportColumns,const CString& delimiter);
};

//_____________________________________________________________________________
class COdbPadstackGeometries : public CObject
{
private:
   CTypedObArrayWithMapContainer<COdbPadstackGeometry> m_padstackGeometries;

public:
   int getCount() { return m_padstackGeometries.getSize(); }
   void empty() { m_padstackGeometries.empty(); }

   COdbPadstackGeometry* findEquivalent(const COdbPadstack& targetPadstack,
      PadstackCompareStatusTag& closestMatchStatus,CString& closestMatchPadstackName);
   void add(const CString& name,COdbPadstackGeometry* padstackGeometry) 
      { m_padstackGeometries.add(name,padstackGeometry); }
   void writeReport(CWriteFormat& report,COdbLayerArrayWithMap& layerArray);
};

//_____________________________________________________________________________
class CQfePadstackTemplate : public CQfe
{
private:
   COdbPadstackTemplate* m_padstack;

public:
   CQfePadstackTemplate(COdbPadstackTemplate* padstack);
   ~CQfePadstackTemplate();

public:
   CPoint2d getOrigin() const { return m_padstack->getOrigin(); }
   CExtent getExtent() const;
   virtual bool isExtentIntersecting(const CExtent& extent);
   virtual bool isPointWithinDistance(const CPoint2d& point,double distance);
   virtual bool isInViolation(CObject2d& otherObject);
   virtual int getObjectType() const;
   virtual CString getInfoString() const;

public:
   COdbPadstackTemplate* getPadstack() { return m_padstack; }

};

//_____________________________________________________________________________
class CQfePadstackTemplateList : public CTypedPtrList<CQfeList,CQfePadstackTemplate*>
{
private:
   bool m_isContainer;

public:
   CQfePadstackTemplateList(bool isContainer=false,int nBlockSize=200);
   ~CQfePadstackTemplateList();
   void empty();
};

//_____________________________________________________________________________
class CQfePadstackTemplates : public CQfeExtentLimitedContainer
{
private:

public:
   CQfePadstackTemplates();
   ~CQfePadstackTemplates();

public:
   virtual int search(const CExtent& extent,CQfePadstackTemplateList& foundList)
      { return CQfeExtentLimitedContainer::search(extent,foundList); }

   virtual CQfePadstackTemplate* findFirst(const CExtent& extent)
      { return (CQfePadstackTemplate*)CQfeExtentLimitedContainer::findFirst(extent); }

   virtual CQfePadstackTemplate* findFirstViolation(const CExtent& extent,CQfe& qfe)
      { return (CQfePadstackTemplate*)CQfeExtentLimitedContainer::findFirstViolation(extent,qfe); }

   virtual CQfePadstackTemplate* findFirstEdgeToRectViolation(const CExtent& extent,int& entitiesChecked)
      { return (CQfePadstackTemplate*)CQfeExtentLimitedContainer::findFirstEdgeToRectViolation(extent,entitiesChecked); }

   virtual CQfePadstackTemplate* findFirstEdgeToPointViolation(const CExtent& extent,
      const CPoint2d& point,double distance,int& entitiesChecked)
      { return (CQfePadstackTemplate*)CQfeExtentLimitedContainer::findFirstEdgeToPointViolation(extent,point,
                                                        distance,entitiesChecked); }

   //virtual int findAllViolations(const CExtent& extent,CQfe& qfe,CQfeList& foundList);

   virtual void setAt(CQfePadstackTemplate* qfePadstack)
      { CQfeExtentLimitedContainer::setAt(qfePadstack); }

public:
   //void load(CPadsDbFile& padsDbFile,CPinPads& pinPads);
   //void load(CLayerPinPads& layerPinPads,int layerNumber);
   //int search(const CLayerSpec& layerSpec,const CExtent& extent,CQfePinPadList& foundList)
   //{ return CQfeLayeredContainer::search(layerSpec,extent,foundList); }
};

//_____________________________________________________________________________
class COdbPpDatabase : public CAlienDatabase
{
private:
   CString m_uncompressCommand;
   PageUnitsTag m_pageUnits;
   double m_scaleFactor;

   COdbStepArray m_stepArray;
   COdbLayerArrayWithMap m_layerArray;  // layers defined in matrix file
   COdbFeatureSymbolMap m_symbolFeatures;
   COdbWheels m_wheels;
   COdbPadstackGeometries m_padstackGeometries;
   COdbPadstackGeometries m_viastackGeometries;
   COdbPadstackGeometry* m_defaultPadStackGeometry;
   CString m_boardOutlineLayerName;

public:
   COdbPpDatabase(CCamCadDatabase& camCadDatabase);
   void empty();

   COdbLayerArrayWithMap& getLayerArray() { return m_layerArray; }
   CString getUncompressCommand() { return m_uncompressCommand; }
   CString getBoardOutlineLayerName() { return m_boardOutlineLayerName; }

   virtual bool read(const CString& filePath,PageUnitsTag pageUnits,CWriteFormat& errorLog);
   bool readSymbolFeatures(CWriteFormat& log);
   bool readWheels(CWriteFormat& log);
   COdbFeatureSymbol* getFeatureSymbol(const CString& name) { return m_symbolFeatures.getDefinedAt(name); }
   COdbPadGeometry& getPadGeometryFor(COdbPad& pad);
   COdbPadstackGeometry& getPadstackGeometryFor(COdbPadstackTemplate& padStack,CWriteFormat& log);
   COdbPadstackGeometry& getViastackGeometryFor(COdbPadstackTemplate& padStack,CWriteFormat& log);
   COdbPadstackGeometry* getDefaultPadstack(CWriteFormat& log);
   COdbStep* getStep(const CString& stepName) { return m_stepArray.getAt(stepName); }
   void writePadstackReports(CWriteFormat& report);

private:
   bool readMatrixData(CWriteFormat& log);
   bool buildCamCadLayers();
   void buildCamCadApertures(CWriteFormat& log)
      { m_symbolFeatures.buildCamCadData(*this,log); }
   COdbPadstackGeometry& getPadstackGeometryFor(COdbPadstackGeometries& padstackGeometries,
      const CString& stackNamePrefix,COdbPadstackTemplate& padStack,CWriteFormat& log);
};

#endif
