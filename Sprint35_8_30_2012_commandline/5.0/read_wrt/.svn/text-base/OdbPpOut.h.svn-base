
#include <vector>
#include <map>
#include <set>
using std::map;
using std::vector;
using std::multimap;
class EdaDataNet;
class EdaDataPkg;
using std::set;

//--------------------------------------------------------------------------

// We just got rid of the notion of an orphan layer, it has not yet been reviewed.
// Stuff related is left lying about, in case we want it back.
// After the notion is validated we can clean out the dead orphan layer stuff.

// The name of the default ODB layer, the layer to use when CCZ layer
// in hand does not map to any recognized layer.
#define ORPHAN_LAYER_NAME "misc"

//--------------------------------------------------------------------------

typedef struct
{
   bool m_isString;
   int m_attributeNum;
   int m_attributeStringNum;
   CString m_value;
}ODBAttribute;

//--------------------------------------------------------------------------

enum FeatureUseType
{
   Copper,
   Laminate,
   Hole
};

//--------------------------------------------------------------------------

enum OdbPadUsageTag
{
   padUsageToeprint       = 0,
   padUsageVia            = 1,
   padUsageGlobalFiducial = 2,
   padUsageLocalFiducial  = 3,
   padUsageToolingHole    = 4,
   padUsageUndefined      = 5
};

//--------------------------------------------------------------------------

typedef struct
{
   FeatureUseType m_useType;
   int m_layerNum;
   int m_featureNum;
}FeatureIDRecord;

//--------------------------------------------------------------------------

typedef struct
{
   bool m_isTop;
   int m_compNum;
   int m_pinNum;
   vector<FeatureIDRecord* > m_featureIDList;
}SNTToePrintRecord;

//--------------------------------------------------------------------------

typedef struct
{
   vector<FeatureIDRecord* > m_featureIDList;
}SNTViaRecord;

//--------------------------------------------------------------------------

typedef struct
{
   vector<FeatureIDRecord* > m_featureIDList;
}SNTPadRecord;

//--------------------------------------------------------------------------

//This structure is keep just the attribute Name/Value and if the 
typedef struct
{
   CString m_attributeName;
   bool m_isString;
   CString m_value;
}CAMCADAttribute;

//--------------------------------------------------------------------------

class ODBComponent;

class BomPartEntry
{
   // A part number entry, shared by all comp inserts for given partnumber.

private:
   ODBComponent* m_comp;

   CString m_partNumber;  // CCZ PARTNUMBER attrib value
   CString m_description; // CCZ Description attrib value
   CString m_descriptionL;// CCZ "Description (Legacy)" attrib value, a vPlan thing, not sure we really want this.
   CString m_devicetype;  // CCZ DEVICETYPE attrib
   CString m_value;       // CCZ VALUE attrib
   CString m_plustol;     // CCZ PLUSTOLERANCE aka +TOL attrib
   CString m_minustol;    // CCZ MINUSTOLERANCE aka -TOL attrib
   CString m_subclass;    // CCZ SUBCLASS attrib
   CString m_loaded;      // CCZ LOADED attrib
   CString m_pinmap;      // CCZ collected from individual comppin pinmap settings.

   // This is for user designated BOM DSC values. These are in the array in
   // the same order as the .BOM_ATTRIB attribute name settings are in the
   // name settings array, which is the same order as in the odb.out file.
   // As of this writing (Feb 2012) the user can have three of these attribs,
   // these plus the standard ones above are the 10 limit.
   CStringArray   m_userAttribVals;

   CStringArray m_refDesList; // All refdes's using this part

public:
   BomPartEntry(ODBComponent* comp, FileStruct *file, CVariantItem *variantItem);

   void AddRefDes(CString refdes)   { m_refDesList.Add(refdes); }
   CString GetPartNumber()          { return m_partNumber; }
   CString GetDescription()         { return m_description; }
   CString GetDescriptionLegacy()   { return m_descriptionL; }  // Do we really want to do this one?
   CString GetDeviceType()          { return m_devicetype; }
   CString GetValue()               { return m_value; }
   CString GetPlusTol()             { return m_plustol; }
   CString GetMinusTol()            { return m_minustol; }
   CString GetSubclass()            { return m_subclass; }
   CString GetLoaded()              { return m_loaded; }
   CString GetPinMap()              { return m_pinmap; }
   int GetCount()                   { return m_refDesList.GetSize(); }
   CString GetRefDes(int indx)      { return (indx >= 0 && indx < m_refDesList.GetSize()) ? m_refDesList.GetAt(indx) : "errorOutOfRange"; }

   int  GetUserAttribCount()           { return m_userAttribVals.GetCount(); }
   void AddUserAttribVal(CString val)  { m_userAttribVals.Add(val);          }
   CString GetUserAttribValAt(int i)   { return (i >= 0 && i < m_userAttribVals.GetCount()) ? m_userAttribVals.GetAt(i) : ""; }
};

//--------------------------------------------------------------------------

class BomData
{
private:
   CString m_bomName;
   map<CString, int> m_bomPartIndexMap;  // map of PN to its index in m_bomPartList vector
   vector<BomPartEntry* > m_bomPartList;

public:
   BomData(CString bomName);
   ~BomData();
   
   BomPartEntry* AddBomPartEntry(ODBComponent* comp, FileStruct *file, CVariantItem *variantItem = NULL);
   int FindBomPartIndex(CString partnum);

   void WriteBomPartEntries(FILE *fp);  // per-partnumber output
   void WriteBomCompEntries(FILE *fp);  // per-refdes output

   CString GetBomName()    { return m_bomName; }
   bool IsUsed()           { return m_bomPartList.size() > 0; }

   static CString GetValidOdbBomName(CString bomName);
};

//--------------------------------------------------------------------------

class CInsertDataStructOb : public CObject
{
private:
   DataStruct *m_data; // An insert's datastruct
   CString m_sortableRefdes;

public:
   CInsertDataStructOb(DataStruct *data) { m_data = data; }
   DataStruct *GetData()      { return m_data; }

   CString GetSortableRefdes();

	int CompareNoCase(CInsertDataStructOb &other);
   int CompareNoCase(CInsertDataStructOb *other) { return other != NULL ? CompareNoCase(*other) : -1; }
};

//--------------------------------------------------------------------------

class CSortedInsertDataStructObMap : public CMapSortedStringToOb<CInsertDataStructOb>
{
private:
public:
   void AddInsertData(DataStruct *data);
   static int AscendingRefnameSortFunc(const void *a, const void *b);
};

//-----------------------------------------------------------------------------------

class EdaData
{
   CString m_outputFileName;
   CString m_header;
   vector<CString> m_layersList;
   map<CString, int> m_netNameIndexMap;         // map of NetName and its index in edaDataNetList vector
   vector<EdaDataNet* > m_edaDataNetList;
   vector<EdaDataPkg* > m_edaDataPkgList;
   map<CString, int> m_pkgNameIndexMap;         // map of PkgName and its index in edaDataPkgList vector
   vector<CString> m_netAttributeNames;
   map<CString, int> m_netAttributeNameIndexMap; // map of netAttributeName and its index in m_netAttributeNames vector
   vector<CString> m_netAttributeStringValues;
   map<CString, int> m_netAttributeStringValueIndexMap; // map of netAttributeStringValue and its index in m_netAttributeStringValues
   void OutputHeader(FILE*);
   void OutputLayerList(FILE*);
   void OutputNetAttributeNames(FILE*);
   void OutputNetAttributeStringValues(FILE*);
   void OutputNetRecord(FILE*);
   void OutputPkgSection(FILE*);
   void OutputFeatureID(FILE* file, FeatureIDRecord* featureIDRecord);
public:
   ~EdaData();
   int AddNetAttributeName(CString AttributeName);          //returns the index of vector where it is added
   int FindNetAttributeNameNumber(const CString & attributeName); //returns -1 if not found
   int AddNetAttributeStringValue(CString AttributeValue); //returns the index of vector where it is added   
   int FindNetAttributeStringValueNumber(const CString & value); //returns -1 if not found
   EdaDataNet* AddEdaNet(NetStruct* net, CString netName, int& index);
   int FindEdaNetNumber(CString cczNetName);                  //returns -1 if not found, name is from ccz data (not odb validated name)
   EdaDataPkg* AddEdaPkg(BlockStruct *compGeomBlock, bool isBottomBuilt, double scale, int& index);
   int FindEdaPkgNumber(BlockStruct *compGeomBlock);       //returns -1 if not found
   void WriteOutputFile(CString stepPath);

   bool EdaData::getCopperPad(CCamCadData& camCadData,
      BlockStruct& padstackGeometry ,bool placedTopFlag, bool topFlag, int insertlayer, DataStruct*& copperPad) const;
   bool EdaData::getCopperPad(CCamCadData& camCadData,
      /*DataStruct& component,*/ BlockStruct& padstackGeometry, bool topFlag, int insertlayer, DataStruct*& copperPad) const;
};

//--------------------------------------------------------------------------

class EdaDataNet
{
private:
   CString m_cczNetName; // Original name from ccz data
   CString m_odbNetName; // Name validated for ODB.

   vector<SNTToePrintRecord *> m_toePrintRecordList;
   vector<SNTViaRecord *> m_viaRecordList;
   vector<FeatureIDRecord* > m_traceRecordList;
   NetStruct* m_camcadNetPtr;
   multimap<int, ODBAttribute* > m_attributeList; // map of attributeNumber and ODBAttribute record

public:
   EdaDataNet(NetStruct* net, CString netName);
   ~EdaDataNet();
   void AddSNTToePrintRecord(SNTToePrintRecord*);
   void AddSNTViaRecord(SNTViaRecord*);
   void AddSNTTraceRecord(FeatureIDRecord*);
   void OutputToFile(FILE*);
   CString GetCczNetName() { return m_cczNetName; }
   CString GetOdbNetName() { return m_odbNetName; }
   vector<SNTToePrintRecord *> GetToePrintRecordList() { return m_toePrintRecordList; }
   vector<SNTViaRecord *> GetViaRecordList() { return m_viaRecordList; }
   vector<FeatureIDRecord* > GetTraceRecordList() { return m_traceRecordList; }
   multimap<int, ODBAttribute* > GetAttributeList() { return m_attributeList; }
   NetStruct* GetCamcadNetStruct() { return m_camcadNetPtr;}
   void AddODBAttribute(ODBAttribute*);
};

//--------------------------------------------------------------------------

//This class is for keeping the component geometry information.
class EdaDataPkg
{
private:
   BlockStruct *m_block;
   vector<ODBAttribute* > m_attributeList;
   double m_scaleFactor;
   CString m_validatedName;
   bool m_isBottomBuilt;

public:
   EdaDataPkg(BlockStruct *blockPtr, bool isBottomBuilt, double scaleFactor);
   ~EdaDataPkg();
   BlockStruct* getCamcadBlockStruct();
   vector<ODBAttribute* > GetAttributeList();
   void AddODBAttribute(ODBAttribute*);
   double GetScaleFactor();
   bool GetIsBottomBuilt();
   CString GetName();   // Returns m_block's name adjusted to legal ODB++ symbol name
   static CString GetValidatedName(BlockStruct *geomBlock);
};

//--------------------------------------------------------------------------

enum FeatureType
{
   FEATURE_LINE,
   FEATURE_PAD,
   FEATURE_ARC,
   FEATURE_TEXT,
   FEATURE_SURFACE
};

//--------------------------------------------------------------------------

//Feature record, Type LINE
typedef struct
{
   CPoint2d m_startPoint;                  // start point
   CPoint2d m_endPoint;                    // end point
   int m_symNum;                         // serial number of the symbol in the feature symbol names section
   char m_polarity;                      // P for positive, N for negative
   int m_dcode;                          // gerber dcode number (0 if not defined)
   vector<ODBAttribute* > m_attributeList;  // Feature attribute list
}LineRecord;

//--------------------------------------------------------------------------

//Feature record, type PAD
typedef struct
{
   CPoint2d m_centerPoint;                // center point
   int m_symNum;                        // serial number of the symbol in the feature symbol names section
   char m_polarity;                     // P for positive, N for negative
   int m_dcode;                         // gerber dcode number (0 if not defined)
   double m_rotation;
   bool m_mirror;
   OdbPadUsageTag m_padUsageType;        // .pad_usage
   vector<ODBAttribute* > m_attributeList; // Feature attribute list
}PadRecord;

//--------------------------------------------------------------------------

//Feature record, type ARC
typedef struct
{
   CPoint2d m_startPoint;                  // start point
   CPoint2d m_endPoint;                    // end point
   CPoint2d m_centerPoint;                 // center point
   int m_symNum;                         // a serial number of the symbol in the feature symbol names section
   char m_polarity;                      // P for positive, N for negative
   int m_dcode;                          // gerber dcode number (0 if not defined)
   char m_clockWise;                     // Y for clockwise, N for counter clockwise
   vector<ODBAttribute* > m_attributeList;  // Feature attribute list
}ArcRecord;

//--------------------------------------------------------------------------

//Feature record, type TEXT
typedef struct
{
   CPoint2d m_textLocation;               // text location (bottom left of first character for 0 orientation)
   double m_rotation;                     // text orientation.
   double m_xsize;                        // Character size
   double m_ysize;                        // Character size
   double m_penWidth;   // For: m_widthFactor;   //width of character segment (in units of 12 mils) i.e. 1 = 12 mils, 0.5 = 6 mils
   CString m_text;                        // text string
   char m_polarity;                       // P for positive, N for negative
   bool m_mirror;
   //int version; We will always ouput "1", "0" for previous version, "1" for current version
   vector<ODBAttribute* > m_attributeList;   // Feature attribute list
}TextRecord;

//--------------------------------------------------------------------------

//Feature record, type SURFACE
typedef struct
{
   char m_polarity;                     // P for positive, N for negative
   int m_dcode;                         // gerber dcode number (0 if not defined)
   CPolyList* m_polyList;               // We will directly write the output.
   vector<ODBAttribute* > m_attributeList; // Feature attribute list
}SurfaceRecord;

//--------------------------------------------------------------------------

class FeatureRecord
{
   FeatureType m_type;             // To capture the record type.
   void* m_featureRecordPtr;
public :
   FeatureRecord(FeatureType type);
   ~FeatureRecord();
   void* GetFeatureTypePtr() { return m_featureRecordPtr; }
   FeatureType GetFeatureType() { return m_type; }
};

//--------------------------------------------------------------------------

class OdbFeatureFile
{
   vector<CString> m_symbolTable;
   map<CString, int> m_symNameIndexMap; // map of symbol Name and its index in m_symbolTable vector
   vector<CString> m_featureAttributeNames;
   map<CString, int> m_featureAttributeNameIndexMap; // map of featureAttributeName and its index in m_featureAttributeNames
   vector<CString> m_featureAttributeStringValues;
   map<CString, int> m_featureAttributeStringValueIndexMap; // map of featureAttributeStringValue and its index in m_featureAttributeStringValues vector
   vector<FeatureRecord* > m_featureList;
public:
   ~OdbFeatureFile();
   int AddSymbol(CString symbolName);
   int FindSymbolNumber(CString symbolName);
   int AddFeatureAttributeName(CString attributeName);
   int FindFeatureAttributeNameNumber(CString attributeName);
   int AddFeatureAttributeStringValue(CString attributeStringValue);
   int FindFeatureAttributeStringValueNumber(CString attributeStringValue);
   int AddFeatureRecord(FeatureRecord *featureRecord);
   void OutputToFile(FILE* file, COperationProgress* progress = NULL);
   void AddFeatureAttributeNames();
};

//--------------------------------------------------------------------------

enum ODBLayerType
{
   SIGNAL,
   POWER_GROUND,
   MIXED,
   SOLDER_MASK,
   SOLDER_PASTE,
   SILK_SCREEN,
   DRILL,
   ROUT,
   DOCUMENT,
   COMPONENT,
   MASK
};

//--------------------------------------------------------------------------

class OdbLayerStruct
{
public:
   CString name;
   CString context;
   CString type;
   CString polarity;
   CString startName;
   CString endName;
   CString addType;

   LayerTypeTag cczLayerType;
   int electricalLayerNumber;

   OdbLayerStruct()  {}
   OdbLayerStruct(CString nm, CString ctx, CString typ, CString pol, CString strtNm, CString endNm, CString addTyp, LayerTypeTag lt, int elecNum)
   { name = nm; context = ctx; type = typ; polarity = pol; startName = strtNm; endName = endNm; addType = addTyp; 
     cczLayerType = lt; electricalLayerNumber = elecNum; }
};

//--------------------------------------------------------------------------

typedef struct
{
   CString name;
   double X;
   double Y;
   bool flip;
   double angle;
   bool mirror;
}StepRepeatInfo;

//--------------------------------------------------------------------------

class ODBCompPin
{
public:
   double m_X;
   double m_Y;
   double m_rotation;
   CString m_mirror;
   int m_netNumber;
   int m_subNetNumber;
   CString m_pinName;
   //not used   CString m_mappedPinFunction;

   ODBCompPin(CString pinName, double x, double y, double rotation, CString mirror, int netNumber, int subNetNumber)
   {
      m_pinName = pinName;
      m_X = x;
      m_Y = y;
      m_rotation = rotation;
      m_mirror = mirror;
      m_netNumber = netNumber;
      m_subNetNumber = subNetNumber;
   }
};

//--------------------------------------------------------------------------

class ODBComponent
{
   DataStruct *m_data;
   int m_pkgNum;
   CString m_refName;
   double m_boardScaleFactor;
   CString m_partnumber;  // CCZ PARTNUMBER attrib
   bool m_loaded;         // CCZ LOADED attrib flag

   map<CString, int> m_pinNameIndexMap;      // map of pinName and its index in m_CompPinList vector
   vector<ODBCompPin* > m_CompPinList;

public:
   ODBComponent(DataStruct *data, int pkgNum, double scale, CString refName);
   ~ODBComponent();
   int AddCompPin(ODBCompPin*);
   DataStruct* GetCompDataStruct()  { return m_data; }
   int GetCompDataEntityNumber()    { return m_data != NULL ? m_data->getEntityNumber() : -1; }
   int GetPkgNum() { return m_pkgNum; }
   CString GetRefName() { return m_refName; }
   double GetBoardScaleFactor() { return m_boardScaleFactor; }
   vector<ODBCompPin* > GetCompPinList() { return m_CompPinList; }
   CString GetPartNumber()    { return m_partnumber; }
   bool IsLoaded()            { return m_loaded; }
};

//--------------------------------------------------------------------------

class ComponentLayerFeatureFile
{
   vector<ODBComponent*> m_componentList;
   map<CString, int> m_compNameIndexMap; // map of compName and its index in m_CompPinList vector
public:
   ~ComponentLayerFeatureFile();
   ODBComponent* AddODBComponent(DataStruct *data,CString refName, int pkgNum, double scale, int& index);
   int FindComponentNumber(CString compName);         // returns -1 if not found
   ODBComponent *FindComponent(CString compName);     // returns NULL if not found
   void OutputToFile(FILE* file, COperationProgress* progress = NULL);
};

//--------------------------------------------------------------------------

class OdbStep
{
private:
   CString m_stepPath; // Path + step name
   CString m_stepName; // name of the step
   map<CString,OdbFeatureFile* > m_layerFeatureFileList;
   bool m_panelStep;
   void WriteCadnetNetlistFile();
   FileStruct* m_file;   // THE CAMCAD DATA
   EdaData* m_edaData;
   BomData* m_bomData; // Primary BOM Data
   vector<StepRepeatInfo*> m_stepRepeatInfo;
   ComponentLayerFeatureFile* m_componentTopLayerFeatureFile;
   ComponentLayerFeatureFile* m_componentBottomLayerFeatureFile;
   double m_scaleFactor;

   CTypedPtrListContainer<BomData*> m_bomVariantList;  // BOMs from CCZ Variants.
   CString GetCurrentVariantName();  // Get current variant setting from ccz file being exported.

   bool IsUsefulPadstackData(DataStruct *data);

   CString GetOdbSizeStr(double cczPageUnitsVal);
   CString GetOdbStandardSymbolName(BlockStruct *apertureBlock);
   bool IsOdbStandardSymbolName(CString name);

   CStringArray m_odbShapeNames; // For validation check, user shape names can't look like ODB standard shape names.

   bool IsOdbComponentType(DataStruct *compInsertData);  // Returns true if data is insert type that is to be processed as component for ODB.
   ODBComponent *GetOdbComponent(CString refName);

   void WriteOneCadNetlistPoint(FILE *fp, int netSerialNum, CPoint2d insertLoc, CString side, BlockStruct *insertedBlock, InsertTypeTag insertType);
   void WriteCadNetlistComppins(FILE *fp, int netSerialNum, NetStruct *net);
   void WriteCadNetlistVias(FILE *fp, int netSerialNum, NetStruct *net);

   void ResolveOdbNet(CompPinStruct *comppin, CString &odbNetName, NetStruct **net);
   void ProcessPinPadstack(DataStruct *componentData, InsertStruct *pinInsert, BlockStruct *pinBlock, CTMatrix pinMatrix, const CTMatrix& boardMatrix,
      SNTToePrintRecord* sntToePrintRecord,
      double pinInsertAngleDegrees, double componentInsertAngleDegrees, double compScaleFactor, EdaDataNet* edaDataNetPtr);

   ODBComponent* ProcessBlockTypePcbComponent(bool isActualComponent, CString refName, DataStruct *componentData, const CTMatrix& componentMatrix, const CTMatrix& boardMatrix, double boardScale);
   void ProcessInsertTypePcbComponent(DataStruct *np, const CTMatrix&);
   void ProcessRandomInsertTypeAbusingPcbComponentGeometry(DataStruct *np, const CTMatrix&);

public:
   OdbStep(FileStruct* file, CString stepPath, CString stepName, bool panelStep,double scaleFactor);
   ~OdbStep();
   void WriteProfileFile();
   int CreateStepDirStruct();
   CString GetStepPath();
   CString GetStepName();
   FileStruct* GetStepFileStruct();

   CString GetOdbSymbolName(BlockStruct *symBlock, double scale);

   CStringArray &GetOdbStandardShapeNames();

   void InitializeStepLayerFeatureFileList();
   OdbFeatureFile* GetLayerFeatureFilePtr(CString odbLayerName);
   ComponentLayerFeatureFile* GetComponentTopLayerFeatureFile();
   ComponentLayerFeatureFile* GetComponentBottomLayerFeatureFile();

   void AddStepRepeatInfo(StepRepeatInfo* );

   void Process();
   void ProcessInsertTypePcb(DataStruct *np, const CTMatrix&);
   void ProcessInsertTypeVia(DataStruct *np, const CTMatrix&);
   
   void ProcessOtherPads(DataStruct *data, const CTMatrix& boardMatrix);
   void ProcessOtherPadstack(DataStruct *parentData, DataStruct *padstackData, const CTMatrix& boardMatrix);
   void ProcessOnePad(DataStruct *parentData, DataStruct *padData, const CTMatrix& boardMatrix);
   void ProcessText(DataStruct* data, const CTMatrix& m);
   void ProcessPoly(DataStruct* data, const CTMatrix& m, bool parentLayerMirrored);
   void ProcessPoly(CPolyList* polyList,const CTMatrix &m, CString odbLayerName, EdaDataNet* dataNetPtr = NULL, bool isNegative = false);
   void ProcessOtherInsertTypes(DataStruct *parentData, const CTMatrix& boardMatrix, bool propagatedLayerMirrored = false, bool keepSilkscreenOnly = false);
   bool ProcessVariants();  // Returns true if processing variants made one or more variant BOMs.
   bool ProcessVariant(CVariant *variant);



   void WriteStepRepeatInfo();
   void WriteCadNetlistInfo();
   void WriteBOMFile(BomData *bomData);
   void WriteVariantBOMFiles();
   void WriteComponentLayerFeatureFile(bool top);
   void WriteLayerFeatureFile();
};

//--------------------------------------------------------------------------

class OdbSymbol
{
private:
   CString m_symbolName;
   BlockStruct *m_geomBlock;
   double m_scale;

public:
   OdbSymbol(CString symName, BlockStruct *cczGeomBlock, double scale)
   {
      m_symbolName = symName;
      m_geomBlock = cczGeomBlock;
      m_scale = scale;
   }

   BlockStruct *GetGeomBlock()   { return m_geomBlock; }
   double GetScale()             { return m_scale; }

   CString GetOdbSymbolName(BlockStruct *cczBlock, double scale); // returns name of block if non-blank, otherwise made up name based on block number
};

typedef vector<OdbLayerStruct*> OdbLayerList;

//--------------------------------------------------------------------------

class OdbJob
{   
   CString m_jobPath;
   vector<CString> m_matrixStepArray;
   map<CString,OdbSymbol*> m_SymbolList;
   vector<OdbStep* > m_stepList;

   OdbLayerList m_matrixLayerArray;

   void CreateCczToOdbLayerMap();
   void ApplyCczLayerNames();
   void FixFromToLayerNames();
   void FillOdbLayerListCLASSIC();  // Original style, fixed set of conventional ODB layer names.
   void FillOdbLayerListFROMDATA(); // Builds ODB layer names based on current CCZ data.
   
   OdbLayerStruct *GetOdbLayerByName(CString odbLayerName);
   OdbLayerStruct *GetOdbLayerByElectricalStackup(int electStackNum);

   CString GetOdbLayerType(LayerTypeTag cczlayerType); // Returns name of ODB type if there is one, empty string if there isn't.
   

public:
   OdbJob(CString jobPath);
   ~OdbJob();

   OdbLayerStruct *GetOdbLayerByType(LayerTypeTag cczLayerType);

   CMapWordToPtr m_layerMap; // Maps ccz layer index to ODBLayer ptr. //*RCF bug MOVE THIS TO PRIVATE AND MAKE ACCESSOR
   
   CString GetJobPath();
   void WriteMatrixFile();
   void WriteSymbolFiles(); // We will create symbol files only for pads.
   int CreateJobDirStruct();

   void FillOdbLayerList();  // Loads either CLASSIC or FROMDATA
   OdbLayerList& GetOdbLayerList();

   void CopyStandardFontsFile();
   void AddStep(OdbStep*);
   bool IsPCBAlreadyProcessed(FileStruct* file);
   void AddSymbol(CString, OdbSymbol*);
   bool IsSymbolPresent(CString symName);
};

//--------------------------------------------------------------------------

class OdbOutSettingsFile
{
   bool m_compressOutput;
   bool m_useOutlineAutoJoin;
   CStringArray m_bomAttrNames;
   bool m_useCadLayerNames;

public:
   OdbOutSettingsFile();
   void LoadSettingsFile(CString settingsFilePath);

   bool GetUseCadLayerNames()       { return m_useCadLayerNames; }
   void SetUseCadLayerNames(bool v) { m_useCadLayerNames = v; }

   bool GetCompressOutput() { return m_compressOutput; }
   void SetCompressOutput(bool compressOutput) { m_compressOutput = compressOutput; }

   bool GetUseOutlineAutoJoin() { return m_useOutlineAutoJoin; }
   void SetUseOutlineAutoJoin(bool autoJoin) { m_useOutlineAutoJoin = autoJoin; }

   int GetBomAttribNameCount()            { return m_bomAttrNames.GetCount();              }
   void AddBomAttribName(CString name)    { if (!name.IsEmpty()) m_bomAttrNames.Add(name); }
   CString GetBomAttribNameAt(int indx)   { return (indx >= 0 && indx < m_bomAttrNames.GetCount()) ? m_bomAttrNames.GetAt(indx) : ""; }

   void Validate(); // Puts messages to log for problems.
};


//--------------------------------------------------------------------------
// This should move to some general utility place some time.
// This is a rework of the "join" feature.
//

enum JoinResult
{
   joinResultNothing = 0,
   joinResultJoined  = 1,
   joinResultClosed  = 2
};

class PolylistCompressor
{
private:
   CPolyList &m_inputPolylist;  // Keep this read only, it is owned outside of this class.
   double m_variance; // aka tolerance, for comparing locations
   CCEtoODBDoc &m_doc;

   bool PntsOverlap(CPnt *p1, CPnt *p2, double variance);
   int CheckForBadVariance(CPoly *poly1, CPoly* poly2, double variance);
   JoinResult TryToConnect(bool connectToHead, CPoly *basePoly, CPolyList &candidatePolylist, double variance);

   void ConnectPolys(CPoly *polyToKeep, bool connectToKeeperHead, CPoly *polyToConsume, bool connectConsumedHead);
   bool TryToClosePoly(CPoly *poly);
   JoinResult ProcessPolylist(CPolyList &polyList);

public:
   PolylistCompressor(CCEtoODBDoc &doc, CPolyList &inputPolylist);

   bool JoinPolys(CPolyList &resultPolylist);

};
