
#if !defined(__CcDoc_h__)
#define __CcDoc_h__

#pragma once

#include "file.h"
#include "block.h"
#include "keyword.h"
#include "lyrgroup.h"
#include "select.h"
#include "resource.h"
#include "settings.h"
#include "undo.h"
#include "net_hilt.h"
#include "nameview.h"
#include "dblrect.h"
#include "attrib.h"
#include "afxtempl.h"
#include "Units.h"
#include "GeneralDialog.h"
#include "DcaCamCadData.h"


//#define EnableGeometryTreeImplementation

#include "GeometryTree.h"

//#include "DFTFacade.h"

// CAMCAD Doc version /////////////////////////////////////////////////////////////////////////////////
//#define DOC_VERSION "6.1"   // Change Edif delimiter from char(4) to char(32) [CAMCAD version = 4.2.18]
//#define DOC_VERSION "6.2"   // Added new structure to components for Real Part (RestructureCompData) [CAMCAD version = 4.3.0bl]
//#define DOC_VERSION "6.3"   // Added new insert types, graphic classes, layer types, and block types (RealPart types)
//#define DOC_VERSION "6.4"   // Added new DFT section [CAMCAD version = 4.4.0br]
//#define DOC_VERSION "7.0"   // Changed Exposed metal diameter attribute from a double to a unit-double
                              // Needed to make a major version change b/c previous versions of CAMCAD on check the major (changed to now check both)
#define DOC_VERSION "7.1"  // Added section <PPSolution></PPSolution> and <MultipleMachines></MultipleMachines> to CC file

class CStencilStepperDialog;
class CCEtoODBDoc;

//_____________________________________________________________________________
typedef CTypedPtrList<CPtrList, HighlightedPinStruct*> CHighlightedPinList;


//_____________________________________________________________________________
struct LayerTypeStruct
{
   COLORREF color;
   BOOL show;
};

//_____________________________________________________________________________
#define MAX_STANDARD_COLORSETS  3  // Normal, Top, Bottom
#define MAX_CUSTOM_COLORSETS   12  // Custom1, Custom2, Custom3, etc
#define MAX_LAYERSETS   (MAX_STANDARD_COLORSETS + MAX_CUSTOM_COLORSETS)
#define ALL_INT64       0xffffffffffffffff 

class CDFTFacade;

#if defined(EnableGerberEducator)
class CGerberEducator;
class CGerberCentroidMergeTool;
enum EducatorFunctionalityTag;
#endif

enum EFeatureType
{
   featureTestVia = -1,
   featureUnknown = 0,
   featureVia     = INSERTTYPE_VIA,
   featureTestPad = INSERTTYPE_TESTPAD,
   featureFreePad = INSERTTYPE_FREEPAD,
};

enum DrawStateTag
{
   drawStateNormal               =  0,
   drawStateSelected             =  1,
   drawStateMarked               =  2,
   drawStateHighlighted          =  3,
   drawStateErase                = -1,
   drawStateR2Not                = -2,
   drawStateAccordingToDataState = -3,
};

////////////////////////////////////////////////////////////////////////////////
// CFeatureTypeItem
////////////////////////////////////////////////////////////////////////////////
class CFeatureTypeItem : public CSelItem
{
public:
   CFeatureTypeItem(CString name = "", EFeatureType featureType = featureUnknown, BOOL selected = FALSE):CSelItem(name, selected)
   {
      m_eFeatureType = featureType;
   };
   ~CFeatureTypeItem(){};

private:
   EFeatureType m_eFeatureType;

public:
   EFeatureType GetFeatureType()                   { return m_eFeatureType;         };
   void SetName(CString name)                      { m_name = name;                 };
   void SetFeatureType(EFeatureType featureType)   { m_eFeatureType = featureType;  };
};

typedef CTypedPtrList<CPtrList, CFeatureTypeItem*> CFeatureTypeItemList;

//------------------------------------------------------------------------------
// CProjectPath
class CProjectPath
{
public:
   CProjectPath();
   ~CProjectPath();

private:
   CString m_pathByFirstFile;
   CString m_pathByUserSelection;
   int m_projectPathOption;   // 0 = Set project path manually
                              // 1 = Set project path by the directory of the first file loaded into CAMCAD

   //CString m_projectPathByFirstFile;
   //CString m_projectPathByUser;
   //bool m_projectPathAlreadySetByFirstFile;

public:
   void SetDefault();

   //CString GetPathByFirstFile() const;
   //CString GetPathByUserSelection() const;
   //int GetOption() const;

   //void SetPathByFirstFile(const CString path);
   //void SetPathByUserSelection(const CString path);
   //void SetOption(const int option);
};

//_____________________________________________________________________________
class CCamCadDocData : public CCamCadData
{
private:
   CCEtoODBDoc& m_camCadDoc;

public:
   CCamCadDocData(CCEtoODBDoc& camCadDoc);

   CCEtoODBDoc& getCamCadDoc() { return m_camCadDoc; }

   virtual void setMaxCoords();
   virtual void loadBackgroundBitmap(BackgroundBitmapStruct& backgroundBitMap);
};

//_____________________________________________________________________________
class CCEtoODBDoc : public CDocument
{
private:
   static int m_nextId;
   int m_id;

   // In order to separate the CAMCAD data from the document, 
   // the following members need to be moved from CCEtoODBDoc to CCamCadData. 
private:
   CCamCadDocData m_camCadData;
   SettingsStruct m_settings;
   CAuxiliaryFiles m_auxiliaryFiles;

   bool m_isTxpTestSessionDoc;  // true when doc is loaded (from ODB++ or CCZ) by vPlan Test Sesssion startup, otherwise false.

public:
   CCamCadData&       getCamCadData()                   { return m_camCadData; }
   const CCamCadData& getCamCadData() const             { return m_camCadData; }

   bool IsTxpTestSessionDoc()                            { return m_isTxpTestSessionDoc; }
   void IsTxpTestSessionDoc(bool flag)                   { m_isTxpTestSessionDoc = flag; }

   CBlockArray&       getBlockArray()                    { return m_camCadData.getBlockArray(); }
   CFileList&         getFileList()                      { return m_camCadData.getFileList(); }
   SettingsStruct&    getSettings()                      { return m_settings; }
   CLayerArray&       getLayerArray()                    { return m_camCadData.getLayerArray(); }
   const CLayerArray& getLayerArray() const              { return m_camCadData.getLayerArray(); }
   CKeyWordArray&     getKeyWordArray()                  { return m_camCadData.getKeyWordArray(); }
   CNamedViewList&    getNamedViewList()                 { return m_camCadData.getNamedViewList(); }
   BackgroundBitmapStruct& getTopBackgroundBitMap()      { return m_camCadData.getTopBackgroundBitMap(); }
   BackgroundBitmapStruct& getBottomBackgroundBitMap()   { return m_camCadData.getBottomBackgroundBitMap(); }
   CString&           getRedLineString()                 { return m_camCadData.getRedLineString(); }
   CString&           getLotsString()                    { return m_camCadData.getLotsString(); }
   CString&           getColorSetsString()               { return m_camCadData.getColorSetsString(); }
   CWidthTableArray&  getWidthTable()                    { return m_camCadData.getWidthTable(); }
   int                getNextWidthIndex()                { return m_camCadData.getNextWidthIndex(); }
   CStringArray&      getDfmAlgorithmNamesArray()        { return m_camCadData.getDfmAlgorithmNamesArray(); }
   int                getDfmAlgorithmArraySize()         { return m_camCadData.getDfmAlgorithmArraySize(); }

   CString            getCcPath() const                  { return m_camCadData.getCcPath(); }
   void               setCcPath(const CString& ccPath)   { m_camCadData.setCcPath(ccPath); }

public:
   CCEtoODBDoc();

protected:
   DECLARE_DYNCREATE(CCEtoODBDoc)

// Attributes
public:
   CString CCFileName;

#if defined (EnableGeometryTreeImplementation)
   CGeometryTreeFilesArray* m_qfeFilesArray;
#endif

   CDFTFacade *pCDFT;

private:
   CProjectPath m_projectPath;
   bool m_bLocked;
   bool fileLoaded;

   CString m_sUserNameLock;
   CUnits m_units;

   CStencilStepperDialog* m_stencilStepperDialog;

#if defined(EnableGerberEducator)
   CGerberEducator* m_gerberEducator;
#endif

public:
   bool isFileLoaded()              { return fileLoaded; };
   void setFileLoaded(bool flag)    { fileLoaded = flag; };

   BlockStruct* getBlockAt(int index)                 { return getBlockArray().getBlockAt(index);  }
   BlockStruct* getDefinedBlockAt(int index);
   bool swapBlocks(BlockStruct* block0,BlockStruct* block1);
   int getMaxBlockIndex()                             { return getBlockArray().getSize();          }
   void resetBlockExtents();
   void GenerateBlockDesignSurfaceInfo(bool regenerate);

   void AddRedlineXMLData(CString xmlString);
   void AddLotsXMLData(CString xmlString);
   CString GetRedlineXMLData();
   CString GetLotsXMLData();
   void AddColorSetsXMLData(CString xmlString);
   CString GetColorSetsXMLData();

#if defined(EnableGerberEducator)
   CGerberEducator& getDefinedGerberEducator(EducatorFunctionalityTag educatorFunctionality);
   CGerberEducator* getGerberEducator() { return m_gerberEducator; }
   void terminateGerberEducatorSession();
#endif

   void updateGerberEducator();
   void updateGerberEducator(int originalSelectionListModificationCount);

   int getMaxLayerIndex() const { return getLayerArray().GetSize(); }

public:
   int colorCount;
   int ta_options_loaded;  // test analysis access options  
   CLayerGroupList LayerGroupList;

   CTypedPtrArray<CPtrArray,KeyWordStruct*> m_standardKeywordArray;  // indexed by StandardAttributeTag


   // List of Top Level Entities to Group Edit
   CSelectList SelectList;

private:
   // Array of Levels of Hierarchy of Selected Entity
   CSelectStack m_selectStack;

public:
   // HighlightedNets
   CMap <int, int, COLORREF, COLORREF> HighlightedNetsMap;
   CHighlightedPinList HighlightedPinsList;

public:
   // UNDO
   UndoItemList *undo[MAX_UNDO_LEVEL];
   int undo_level;
   int undo_min;
   int redo_max;

   // Schematic Link
   BOOL schLinkPCBDoc;

   // Color Sets
   LayerTypeStruct LayerTypeArray[MAX_LAYERSETS][MAX_LAYTYPE]; // Normal, Top, Bottom, Cust 1, 2, 3, 4, 5
   CStringArray CustomLayersetNames;
   int CustomColorsetButtonIDs[MAX_CUSTOM_COLORSETS];
   //BOOL& Bottom;
   int ColorSet;
   int PanelBoardView;

   CString projectFileString;
   bool m_firstTimeDraw;
   //BOOL showClasses[MAX_CLASSTYPE];
   //BOOL showInsertTypes[MAX_INSERTTYPE];
private:
   bool m_showFills;
   bool m_showApFills;
   bool m_showAttribs;

public:
   BOOL showPinnrsTop;
   BOOL showPinnrsBottom;
   BOOL showPinnetsTop;
   BOOL showPinnetsBottom;
   BOOL showTAOffsetsTop;
   BOOL showTAOffsetsBottom;
   BOOL showSeqNums;
   BOOL showSeqArrows;
   BOOL showDirection;

   DataStruct *seqInsertBehind;
   CDataList *seqDataList;

   BlockStruct *currentMarker; // current DRC Geometry

   // max_Coord are not to exceed MAX_COORD because of MAX_INT
   // these depend on the user defined range of units (Settings._max)
   // the larger of the two is equal to MAX_COORD
   // the smaller is proprtionally smaller than MAX_COORD
   int minXCoord, maxXCoord, minYCoord, maxYCoord;
   BOOL NegativeView;
   BOOL DrawingImported;
   BOOL TitleFull;
   BOOL FoundPolyEntity;
   int Filter, SearchType;
   BOOL GeometryEditing;
   BOOL DontAskRegenPinLocs;
   CString docTitle;

   BOOL bgBmpOnTop;

// Operations - private
private:
   void mapUsedBlocks(CMapBlockNumberToBlock& map,BlockStruct& block);
   void mapUsedWidths(CMapBlockNumberToBlock& map,BlockStruct& block);

   void ApplyCczInSettings();

// Operations - public
public:
   bool getBottomView() const;
   void setBottomView(bool bottomViewFlag);

   CCEtoODBDoc* StoreDocForImporting(); // Call before importing to remember Lists
   void PrepareAddEntity(FileStruct *file);
   void SetMaxCoords();
   BOOL LoadCczDataFile();
   //FileStatusTag loadNonXmlCcFile(const CString& CCFileName);
   void LoadProjectFile();
   void DeleteOccurancesOfKeyword(WORD keyword);
   BOOL AreThereOccurancesOfKeyword(WORD keyword);
   void MergeOccurancesOfKeyword(WORD from, WORD to, int method);
   void MoveOrigin(double x, double y);
   void zoomToComponent(const CString& refDes);

   // blocks
   BlockStruct *Add_Blockname(const char *name, int filenum, DbFlag flg, BOOL Aperture, BlockTypeTag blockType = blockTypeUndefined);
   void RemoveBlock(BlockStruct *block);
   void FreeBlock(BlockStruct *block)  { delete block; }
   int Get_Block_Num(const char *blockname, int filenum, int global, BlockTypeTag blockType = blockTypeUndefined);
   BlockStruct *Find_Block_by_Name(CString blockname, int filenum, BlockTypeTag blockType = blockTypeUndefined);
   BlockStruct *Find_Block_by_Num(int block_num);
   BOOL is_file_inserted(int blocknum);

   void purgeUnusedBlocks(bool displayReport);
   int purgeUnusedBlocks(CWriteFormat* writeFormat=NULL);
   void purgeUnusedWidths(bool displayReport);
   int purgeUnusedWidths(CWriteFormat* writeFormat=NULL);
   void purgeUnusedWidthsAndBlocks(bool displayReport);

   DataStruct* Graph_Block_Reference(const char *block_name, const char *refname, int filenum, double x, double y,
         double angle, int mirror, double scale, int layer, int global,BlockTypeTag blockType = blockTypeUndefined);

   DataStruct* Graph_Block_Reference(int block_number, const char *refname, double x, double y,
         double angle, int mirror, double scale, int layer);

   DataStruct& SetCentroid(BlockStruct& block, double x, double y, double angleDegrees);
   DataStruct& SetCentroid(BlockStruct& block, double x, double y, double angleDegrees, CString methodAttrVal);
   bool ValidateTestpoint(BlockStruct& block);

   // Data and DataLists
   bool removeDataFromDataList(CDataList& dataList,DataStruct* data,POSITION dataPos=NULL);
   bool removeDataFromDataList(BlockStruct& block,DataStruct* data,POSITION dataPos=NULL);
   int removeDataFromDataListByGraphicClass(CDataList& dataList,GraphicClassTag graphicClass);
   int removeDataFromDataListByGraphicClass(BlockStruct& block ,GraphicClassTag graphicClass);
   int removeDataFromDataListByInsertType(CDataList& dataList,InsertTypeTag insertType);
   int removeDataFromDataListByInsertType(BlockStruct& block ,InsertTypeTag insertType);

   bool validateAndRepair(DataStruct& data);

   // widths and apertures
   double getWidth(int widthIndex);
   int getDefinedWidthIndex(double width);
   int getDefinedSquareWidthIndex(double width);
   BlockStruct* getWidthBlock(int widthIndex);
   BlockStruct* getDefinedWidthBlock(int widthIndex);
   int getSmallWidthIndex();
   int getZeroWidthIndex();

   //-------------------------------------------------------------
   // Okay - Assumes all is global, ie. filenumber = -1
   int Graph_Complex(const char *name, int dcode, const char *subBlockName,
         double xoffset, double yoffset, double rotation, BlockTypeTag blockType = blockTypeUndefined);

   // Better - Be explicit with file number, still prone to mismatch with inserted block
   int Graph_Complex(int filenum, const char *name, int dcode, const char *subBlockName,
         double xoffset, double yoffset, double rotation, BlockTypeTag blockType = blockTypeUndefined);

   // Best - Auto syncs complex ap's and inserted block's file number
   int Graph_Complex(const char *name, int dcode, BlockStruct *subBlock,
         double xoffset, double yoffset, double rotation);
   //-------------------------------------------------------------

   int Graph_Aperture(const char *name, int shape, double sizeA, double sizeB,
         double xoffset, double yoffset, double rotation,
         int dcode, DbFlag flag, int overWriteValues, int *overWriteError);

   int Graph_Aperture(int filenum, const char *name, int shape, double sizeA, double sizeB,
         double xoffset, double yoffset, double rotation,
         int dcode, DbFlag flag, int overWriteValues, int *overWriteError);

   // layers
   void RemoveLayer(LayerStruct *layer);
   //void FreeLayer(LayerStruct*& layer);
   LayerStruct *FindLayer(int layernum) const { return getLayer(layernum); }
   LayerStruct* getLayer(int layerIndex) const;
   LayerStruct *FindLayer_by_Name(const char *layername) const { return getLayer(layername); }
   LayerStruct* getLayer(const CString& layerName) const;
   LayerStruct* getLayerNoCase(const CString& layerName) const;
   LayerStruct& getDefinedLayer(const CString& layerName);
   LayerStruct& getDefinedLayerNoCase(const CString& layerName);
   LayerStruct *Add_Layer(CString name);
   BOOL IsFloatingLayer(int layernum);
   BOOL IsNegativeLayer(int layernum);
   int getMaxElectricalLayerNumber() const;
   unsigned long get_layer_color(int layernum, int mirror);
   bool get_layer_visible(int layernum, int mirror);
   bool isLayerVisible(int layerIndex,bool mirrorLayersFlag);
   BOOL get_layer_editable(int layernum, int mirror);
   BOOL get_layer_visible_extents(int layernum);
   int get_layer_mirror(int layernum, int mirror);
   int getMirroredLayerIndex(int layerIndex,bool mirrorLayerFlag);
   void UseColorSet(int set, BOOL newValueForBottom);
   LayerStruct* getLayerAt(int layerIndex) { return FindLayer(layerIndex); }
   LayerStruct* getDefinedLayerAt(int layerIndex);
   LayerStruct* AddNewLayer(CString name);
   int getLayerCount() const { return getLayerArray().GetSize(); }
   LayerStruct* getDefinedFloatingLayer();
   void MarkUnusedLayers();
   void RemoveUnusedLayers(bool purge_electricalstackup=false);
   int Graph_Level(const char *layerName, const char *prefix, BOOL floating);
   void Graph_Level_Mirror(const char* layerName1,const char* layerName2,const char* prefix=NULL);
   CLayerFilterStack& getSelectLayerFilterStack() { return getCamCadData().getSelectLayerFilterStack(); }
   int propagateLayer(int parentLayerIndex,int childLayerIndex);

private:

   // Polys
   void convertPolyToTerior(bool exteriorFlag);

public:
   // files
   bool HasDuplicateFileNumbers(); // True if FileList has separate files with same file number
   FileStruct* Add_File(CString name, int source_cad);
   FileStruct* Find_File(int num);
   FileStruct* Find_File_by_BlockPtr(BlockStruct *block);
   FileStruct* Find_File_by_BlockGeomNum(int geomnum);
   FileStruct* FileFileByName(CString fileName);
   void SetShowAllFile(bool showFlag);
	void ValidateProbes(FileStruct& fileStruct);
	void HideRealpartsForUnloadedComponents(FileStruct& fileStruct);
	void applyProbeStyles(FileStruct& fileStruct,CProbeStyleList& probeStyles);
   //bool LoadDFTSolutionXML(CString xmlDataString, FileStruct& fileStruct, bool setCurrentSolution = true);
   //bool LoadDFTSolutionFile(CString xmlFileName, FileStruct& fileStruct, bool setCurrentSolution = true);
   //void LoadMultipleMachineXML(CString xmlString, FileStruct& fileStruct);

   // settings
   void Scale(double factor, int decimals);
   PageUnitsTag getPageUnits() { return getSettings().getPageUnits(); }
   const CUnits& getUnits() { m_units.setPageUnits(getPageUnits());  return m_units; }
   double convertPageUnitsTo(PageUnitsTag units,double value);
   double convertToPageUnits(PageUnitsTag units,double value);
   void loadDefaultSettings();
   void saveDefaultSettings();

   bool getShowFills() const;
   void setShowFills(bool flag);

   bool getShowApFills() const;
   void setShowApFills(bool flag);

   bool getShowAttribs() const;
   void setShowAttribs(bool flag);

   // nets
   void ColorNet(NetStruct *net, COLORREF color);

   // extents
   void get_extents(double *lxmin, double *lxmax, double *lymin, double *lymax);
   CExtent getExtent();
   void CalcBlockExtents(BlockStruct* block);
   bool validateBlockExtents(BlockStruct* block,bool checkOnlyVisibleEntitiesFlag=true);
   void MarkRect(double xmin, double xmax, double ymin, double ymax, BOOL CompletelyEnclosedIn);
   CExtent blockExtents(CDataList* DataList,     
         double insert_x,double insert_y,double rotation,int mirror,double scale,int insertLayer,bool checkOnlyVisibleEntitiesFlag,CMessageFilter* messageFilter=NULL);

   // select
   CSelectList& getSelectList() { return SelectList; }
   bool Search(double X, double Y, double Range,bool MarkLast,bool SearchAgain);
   
   int SearchBlock(CSelectStructListContainer& selectStructList,CDataList *DataList, POSITION pos, const bool levelZeroFlag, const int filenum, 
         const double insert_x, const double insert_y, const double scale, const double rotation, const int mirror, const int insertLayer,
         bool filterChild);
   int SearchBlockLevel0(CDataList *DataList, POSITION pos, const int filenum, 
         const double insert_x, const double insert_y, const double scale, const double rotation, const int mirror, const int insertLayer,
         bool fileterChild);
   
   bool SearchTextData(CSelectStructListContainer& selectStructList,CDataList *DataList, DataStruct *data, int filenum, int layer, 
         double insert_x, double insert_y, double scale, double rotation, int mirror);
   bool SearchText(const char *text, int penWidthIndex, double insert_x, double insert_y, double scale, double rotation, int mirror,
         double text_x, double text_y, int text_mirror, double text_rotation, short text_fontnum, double width, double height, BOOL proportional,
         int textAlignment, int lineAlignment);
   void DrawEntity(SelectStruct& selectStruct,DrawStateTag drawState);
   void DrawEntityIgnoreClassFilter(SelectStruct& selectStruct,DrawStateTag drawState);
   void DrawEntity(SelectStruct *s, int state, BOOL Override);
   void DrawSubSelected();
   void HighlightPolyItem(SelectStruct *s, CPoly *poly, CPnt *pnt, BOOL Erase);
   void HighlightByAttrib(BOOL AllValues, WORD Keyword, int ValueType, double Val);
   void UnhighlightByAttrib(BOOL AllValues, WORD Keyword, int ValueType, double Val);

   CInsertTypeFilter& getInsertTypeFilter() { return getCamCadData().getInsertTypeFilter(); }
   CGraphicsClassFilter& getGraphicsClassFilter() { return getCamCadData().getGraphicsClassFilter(); }
   bool isInsertTypeVisible(InsertTypeTag insertType);
   bool isGraphicClassVisible(GraphicClassTag graphicClass);
   void setInsertTypeVisible(InsertTypeTag insertType,bool visibleFlag);
   void setGraphicClassVisible(GraphicClassTag graphicClass,bool visibleFlag);
   void findNextEntityAtThisLevel();

   SelectStruct *InsertInSelectList(DataStruct *p, int filenum, double insert_x, double insert_y, 
         double scale, double rotation, int mirror, CDataList *DataList, BOOL Mark, BOOL ClearSelectedHiearchy,
         bool addToSelectStack=true);
   SelectStruct* InsertInSelectList(SelectStruct& selectStruct,bool mark,bool clearSelectedHierarchy,bool addToSelectStack=true);
   void SelectEntity(BOOL SearchAgain);
   void ClearSubSelectArray(int upThroughLevel);
   void UnselectAll(BOOL Redraw);
   void unselectAll(bool redrawFlag,bool checkVisibleEntitiesOnlyFlag);
   //void clearSelected(bool redrawFlag,bool checkVisibleEntitiesOnlyFlag);
   SelectStruct *MakeThisEntitySelected(DataStruct *p, int filenum, double insert_x, double insert_y, double scale, 
         double rotation, int mirror, int layer, CDataList *DataList);  
   void NavNetSelectionChanged(NetStruct *net);
   void NavInstSelectionChanged(DataStruct* data);

   bool somethingIsSelected() const;
   bool nothingIsSelected() const;
   //int getSelectStackTopIndex() const;
   //void setSelectStackTopIndex(int index);
   //int getSelectStackLevelIndex() const;
   //void setSelectStackLevelIndex(int index);
   //SelectStruct* getSelectionAtSelectStackLevel();
   //SelectStruct* getSelectionAt(int index);
   CSelectStack& getSelectStack();
   const CSelectStack& getSelectStack() const;
   //CTMatrix getTMatrixAtSelectStackLevel();
   //CTMatrix getTMatrixForSelectionAt(int index);
   //SelectStruct* pushSelectionOntoSelectStack(CDataList* dataList,DataStruct* data,const int filenum,
   //                 const double insert_x,const double insert_y,const double scale,const double rotation,const int mirror);

   // undo
   void ClearNextUndoLevel();
   void PrepareThisUndoLevel();
   void ClearUndo();

   // copper
   void Flood_Fill(CPolyList *PolyList, double clearance);

   // attributes
   int IsKeyWord(const char *str, int section);  // return index or -1 for not found
   int RegisterKeyWord(const char* attributeName,ValueTypeTag valueType,bool hiddenFlag = false);
   int RegisterKeyWord(const char *str, int section, int valueType,bool hiddenFlag = false);
   int IsValue(const char *STR);
   int RegisterValue(const char *str);

   BOOL SetAttrib(CAttributes** map, int keyword, int valueType, void *value, int method=SA_OVERWRITE, Attrib **attribPtr=NULL);
   bool SetAttrib(CAttributes& attributes, int keyword, int valueType, void *value, int method=SA_OVERWRITE, Attrib **attribPtr=NULL);
   BOOL SetAttrib(CAttributes** map, int keyword,int value, int method=SA_OVERWRITE, Attrib **attribPtr=NULL);
   BOOL SetAttrib(CAttributes** map, int keyword,float value, int method=SA_OVERWRITE, Attrib **attribPtr=NULL);
   BOOL SetAttrib(CAttributes** map, int keyword,double value, int method=SA_OVERWRITE, Attrib **attribPtr=NULL);
   BOOL SetAttrib(CAttributes** map, int keyword,const char* value, int method=SA_OVERWRITE, Attrib **attribPtr=NULL);
   BOOL SetUnknownAttrib(CAttributes** map, const char *keyword, const char *value, int method, Attrib **attribPtr);
   BOOL SetVisAttrib(CAttributes** map, int keyword, int valueType, void *value, 
         double x, double y, double rotation, double height, double width, BOOL proportional, int penWidthIndex, 
         BOOL visible, int method, DbFlag flag, short layer, BOOL never_mirror, int textAlignment, int lineAlignment,
         BOOL mirror_flip = FALSE);
   BOOL SetUnknownVisAttrib(CAttributes** map, const char *keyword, const char *value,
      double x, double y, double rotation, double height, double width, BOOL proportional, int penWidthIndex, 
      BOOL visible, int method, DbFlag flag, short layer, BOOL never_mirror, int textAlignment, int lineAlignment,
      BOOL mirror_flip = FALSE);

   CString getAttributeStringValue(Attrib* attribute);
   bool getAttributeStringValue(CString& stringValue,CAttributes** attributes,int keywordIndex);
   bool getAttributeStringValue(CString& stringValue,CAttributes& attributes,int keywordIndex);

   void CopyAttribs(CAttributes** to, CAttributes* from);
   void MergeAttribs(CAttributes** to, CAttributes* from, int Method);
   void RegisterInternalKeywords();
   const KeyWordStruct* getStandardAttributeKeyword(StandardAttributeTag standardAttributeTag);
   const KeyWordStruct* getKeyword(int keywordIndex);
   int getStandardAttributeKeywordIndex(StandardAttributeTag);

   // named views
   void getLayerViewData(CNamedView& namedView);
   void setLayerViewData(CNamedView& namedView);

   // DFT Tool Stacks
private:
   BlockStruct *CreateSTDTool(CString toolStackGeom, int &type);

   // Gerber Educator
public:
   void gerberEducatorCreateGeometries();

   // Centroids
   BlockStruct *GetCentroidGeometry();

   // ported from FileStruct to support dca
      // DFT Solution
      //CDFTSolutionList& getDFTSolutions(const FileStruct& file);

      // temporary, to be removed after 4.6 Access Analysis code is released to 4.5 - knv 20060406
      //CDFTSolution* AddNewDFTSolution(FileStruct& file,CString name,int pageUnits) { return AddNewDFTSolution(name,intToPageUnitsTag(pageUnits)); }

      /*CDFTSolution* AddNewDFTSolution(const FileStruct& file, const CString& name, const bool isFlipped, PageUnitsTag pageUnits);
	   CDFTSolution* FindDFTSolution(const FileStruct& file, const CString& name, bool isFlipped);
      CDFTSolution* GetCurrentDFTSolution(const FileStruct& file);
      int SetCurrentDFTSolution(const FileStruct& file,POSITION pos);
	   int SetCurrentDFTSolution(const FileStruct& file,const CString& name);
	   int SetCurrentDFTSolution(const FileStruct& file,CDFTSolution* dftSolution);*/

	   // Multiple Machine
	   //CMachineList* CreatMachineList(const FileStruct& file);
      //CMachineList* getMachineList(const FileStruct& file);
	   /*int GetMachineCount(const FileStruct& file);
	   POSITION GetMachineHeadPosition(const FileStruct& file);
	   POSITION GetMachineTailPosition(const FileStruct& file);
	   CMachine* GetMachineHead(const FileStruct& file);
	   CMachine* GetMachineTail(const FileStruct& file);
	   CMachine* GetMachineNext(const FileStruct& file,POSITION &pos);
	   CMachine* GetMachinePrev(const FileStruct& file,POSITION &pos);
	   CMachine* FindMachine(const FileStruct& file,const CString& name);
	   CMachine* GetCurrentMachine(const FileStruct& file);
	   int SetCurrentMachine(const FileStruct& file,POSITION pos);
	   int SetCurrentMachine(const FileStruct& file,const CString& name);
	   int SetCurrentMachine(const FileStruct& file,CMachine* machine);
      void DeleteMachineOrigin(FileStruct *pcbFile, CString MachineName);*/


	   void ValidateProbes(CCEtoODBDoc *doc);
      POSITION FindInsertDataAt(FileStruct *pcbFile, int blockNumber);
   // above ported from FileStruct to support dca

public:
   CString GetProjectPath(CString defaultPath = "") const;

   void SetProjectPathByFirstFile(const CString path);
   void SetProjectPathByUserSelection(const CString path);
   void SetProjectPathOption(const int option);

   DataStruct *InsertToolStack(CString pcb, CString toolStackID, CString toolStackGeom, double x, double y);
   DataStruct *InsertToolStack(int pcb, CString toolStackID, CString toolStackGeom, double x, double y);
   void DeleteToolStack(CString pcb, CString toolStackID);
   void DeleteToolStack(int pcb, CString toolStackID);
   void DeleteAllToolStacks(CString pcb);
   void DeleteAllToolStacks(int pcb);

   void loadBackgroundBitmap(BackgroundBitmapStruct& backgroundBitMap);

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CCEtoODBDoc)
   public:
   virtual void OnCloseDocument();
   virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
   virtual BOOL SaveModified();
   //}}AFX_VIRTUAL

// Implementation
public:
   POSITION GetDocumentPosition();
   virtual ~CCEtoODBDoc();
   virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

   bool IsLocked() const                        { return m_bLocked; };
   CString GetUserLock() const                  {  return m_sUserNameLock; };
   bool Lock(CString userName);
   bool Unlock(CString userName);

   //void WriteXML(FILE *stream);
   //void WriteXML(CWriteFormat& writeFormat,bool showProgressFlag=true);

   //void FileExport(int fileType);
   //void DoFileImport(FileTypeTag fileType);
   //int ImportFile(const char *fileName, FormatStruct *format, BOOL AllowDialogs);

   //bool GetExportFormatByString(CString fileName, CString formatString, FormatStruct *format);
   void ExportFile(CString fileName, FormatStruct *format, BOOL run_silent = FALSE);

   //bool SaveDatafile(const CString& filePath);
   //bool SaveDataFileAs(const char *FileName);
   //bool SaveCompressedDataFileAs(const CString& FileName,CamCadFileTypeTag camCadFileType);

   //bool SaveDatafile();
   //bool SaveDataFileAs();

   bool HasData();
   void FreeDoc();
   void SortEntities();
   void ConvertEtchToSignals();

   LayerGroupStruct *FindLayerGroup(const char *name);
   LayerGroupStruct *AddLayerGroup(const char *name);
   void LayerGroup_Show(LayerGroupStruct *group, BOOL On);
   void LayerGroup_Color(LayerGroupStruct *group, COLORREF color);
   //bool calcExtents(ExtentRect& extents,FileStruct*& pcbFile);
   CExtent calcFileOutlineExtents(FileStruct* pcbFile);
   void generatePinLocations(bool displayLogFileFlag=true,COperationProgress* progress=NULL);
   void GenerateSmdComponentTechnologyAttribs(COperationProgress* progress=NULL, bool visibleFilesOnly = false);

   // verify data
public:
   bool verifyAndRepairData();
   void FitPageKeepingZoom(BOOL newValueForBottom);
   //void unmarkAndUnselectEntity() ;
   //void toggleMarkSelection(bool previouslySelectedFlag);

private:
   int m_NBBG_MirCnt;  // used by CreateTopDefinedGeometry et al.
   bool IsFeatureTypeInArray(CFeatureTypeItemList *featureTypeList, DataStruct *data);
   void ConvertFeatureToPCBComponent(CFeatureTypeItemList *featureTypeList);
   BlockStruct* CreateTopDefinedGeomtry(int bottomBuildBlockNum, CMapBlockNumberToBlock& bottomBuildGeometryMap);
   BlockStruct *CreateReversePadstack(BlockStruct* padstack, CString newPadstackName);
   void MirrorPolyOnX(CPolyList* polylist);

public:
   //{{AFX_MSG(CCEtoODBDoc)
   afx_msg void OnNormalizeValues();
   afx_msg void OnPcbCAMCADNavigator();
   afx_msg void FilterInsertType();
   afx_msg void FiltergraphicClass();
   //afx_msg void OnFileImport();
   afx_msg void OnFileExport();
   afx_msg void OnApertureList();
   afx_msg void OnEditTables();
   afx_msg void OnFileList();
   afx_msg void OnGeometryList();
   afx_msg void OnLayerList();
   afx_msg void OnRedraw();
   afx_msg void OnChangeSettings();
   afx_msg void OnGeometryEnhancer();
   afx_msg void OnSettingsSave();
   afx_msg void OnSettingsLoadLocal();
   afx_msg void SearchAgain();
   //afx_msg void MarkEntity();
   afx_msg void OnEditEntity();
   afx_msg void OnAddFile();
   afx_msg void OnAddLayer();
   afx_msg void OnNegativeView();
   //afx_msg void OnDeleteSelected();
   //afx_msg void OnClearSelected();
   //afx_msg void OnSelectAllWithSameGeometry();
   //afx_msg void OnSelectAllFlashesWithSameDCode();
   //afx_msg void OnSelectAllDrawsWithSameDCode();
   afx_msg void OnShowGrid();
   afx_msg void OnSnapToGrid();
   afx_msg void OnShowOrigin();
   afx_msg void OnUndo();
   afx_msg void OnRedo();
   afx_msg void DumpUndo();
   //afx_msg void ReverseMarked();
   afx_msg void OnExplodeBlocks();
   afx_msg void OnExplodePolylines();
   afx_msg void OnCopperPour();
   afx_msg void OnAttribLists();
   afx_msg void OnKeywordMap();
   afx_msg void OnShowFills();
   afx_msg void OnShowAllkeepin();
   afx_msg void OnShowAllkeepout();
   afx_msg void OnShowAnalysis();
   afx_msg void OnShowAntiEtch();
   afx_msg void OnShowBoardGeom();
   afx_msg void OnShowBoardoutline();
   afx_msg void OnShowCompoutline();
   afx_msg void OnShowPackageOutline();
   afx_msg void OnShowDrawing();
   afx_msg void OnShowDrc();
   afx_msg void OnShowEtch();
   afx_msg void OnShowManufacturing();
   afx_msg void OnShowNormal();
   afx_msg void OnShowPaneloutline();
   afx_msg void OnShowPlacekeepin();
   afx_msg void OnShowPlacekeepout();
   afx_msg void OnShowRoutkeepin();
   afx_msg void OnShowRoutkeepout();
   afx_msg void OnShowViakeepin();
   afx_msg void OnShowViakeepout();
   afx_msg void OnShowUnroute();
   afx_msg void OnShowPackageBody();
   afx_msg void OnShowPackagePin();
   afx_msg void OnShowSignal();
   afx_msg void OnShowBus();

   afx_msg void OnShowPanelBorder();
   afx_msg void OnShowManufacturingOutl();
   afx_msg void OnShowContour();
   afx_msg void OnShowPlaneKeepout();
   afx_msg void OnShowPlaneNoConnect();
   afx_msg void OnShowBoardKeepout();
   afx_msg void OnShowPadKeepout();
   afx_msg void OnShowEPBody();
   afx_msg void OnShowEPMask();
   afx_msg void OnShowEPOverGlaze();
   afx_msg void OnShowEPDielectric();
   afx_msg void OnShowEPConductive();
   afx_msg void OnShowBusPath();
   afx_msg void OnShowRegion();
   afx_msg void OnShowCavity();

   afx_msg void OnShowTraceFanout();
   afx_msg void OnShowAll();
   afx_msg void OnShowNone();
   afx_msg void OnShowAttribs();
   afx_msg void OnHiglightSelectedNet();
   afx_msg void OnToolsToollist();
   afx_msg void OnToolList();
   afx_msg void OnShowMilling();
   afx_msg void OnRefreshInheritance();
   afx_msg void OnSelectNets();
   afx_msg void OnRestructureFiles();
   afx_msg void OnItUnknown();
   afx_msg void OnItVia();
   afx_msg void OnItPin();
   afx_msg void OnItPcbcomp();
   afx_msg void OnItMechcomp();
   afx_msg void OnItGenericcomp();
   afx_msg void OnItPcb();
   afx_msg void OnItFiducial();
   afx_msg void OnItTooling();
   afx_msg void OnItTestpoint();
   afx_msg void OnItTestprobe();
   afx_msg void OnItFreepad();
   afx_msg void OnItAll();
   afx_msg void OnItNone();
   //afx_msg void OnUnlinkPolys();
   afx_msg void OnEditSelectedBlock();
   afx_msg void OnDoneEditing();
   afx_msg void OnSelectAndEditBlock();
   afx_msg void OnDeleteTraces();
   //afx_msg void OnCleanSelectedPolys();
   afx_msg void OnDeleteDocGraphic();
   afx_msg void OnFillSolid();
   afx_msg void OnFillHorizontal();
   afx_msg void OnFillVertical();
   afx_msg void OnFillCross();
   afx_msg void OnFillDiagonal1();
   afx_msg void OnFillDiagonal2();
   afx_msg void OnFillDiagonalCross();
   afx_msg void OnExplodeAll();
   afx_msg void OnOGP_LoadDefaults();
   afx_msg void OnOGP_InteractiveLights();
   afx_msg void OnOGP_FieldOfView();
   afx_msg void OnOGP_InteractiveEdge();
   afx_msg void OnOGP_InteractiveCentroid();
   afx_msg void OnOGP_InteractiveTol();
   afx_msg void OnShowPinnrTop();
   afx_msg void OnShowPinnrBottom();
   afx_msg void OnShowPinnetsTop();
   afx_msg void OnShowPinnetsBottom();
   afx_msg void OnCompPinEditor();
   afx_msg void OnGeneratePinloc();
   afx_msg void OnEasyOutline_Debug();
   afx_msg void OnMergeFiles();
   afx_msg void OnAssignAttrib();
   afx_msg void OnStepInto();
   afx_msg void OnStepOutOf();
   afx_msg void OnPanel();
   afx_msg void OnPanelizer();
   afx_msg void OnShowSeqNums();
   afx_msg void OnShowSeqArrows();
   afx_msg void OnSeqHead();
   afx_msg void OnSeqTail();
   afx_msg void OnSeqAssignHead();
   afx_msg void OnSeqBehindHead();
   afx_msg void OnGenerateSmdComponentVisibleFilesOnly();
   afx_msg void OnGenerateComponentCentroid();
   afx_msg void OnShowApFills();
   afx_msg void OnOGP_ReverseDirection();
   afx_msg void OnOGP_ShowDirection();
   afx_msg void OnTestabilityAnalyzer();
   afx_msg void OnCompPinAttribs();
   afx_msg void OnLayerStack();
   afx_msg void OnReportsSpreadsheet();
   afx_msg void OnReportsEntityNumbers();
   afx_msg void OnCreateLayerGroups();
   afx_msg void OnEditLayerGroups();
   afx_msg void OnShowBoardcutout();
   afx_msg void OnShowPanelcutout();
   afx_msg void OnLayertypes();
   afx_msg void OnColorsetsNormal();
   afx_msg void OnColorsetsTop();
   afx_msg void OnColorsetsTop(BOOL newValueForBottom);
   afx_msg void OnColorsetsBottom();
   afx_msg void OnColorsetsBottom(BOOL newValueForBottom);

   afx_msg void OnColorsetsCustomN(int setNum);
   afx_msg void OnColorsetsCustom1();
   afx_msg void OnColorsetsCustom2();
   afx_msg void OnColorsetsCustom3();
   afx_msg void OnColorsetsCustom4();
   afx_msg void OnColorsetsCustom5();
   afx_msg void OnColorsetsCustom6();
   afx_msg void OnColorsetsCustom7();
   afx_msg void OnColorsetsCustom8();
   afx_msg void OnColorsetsCustom9();
   afx_msg void OnColorsetsCustom10();
   afx_msg void OnColorsetsCustom11();
   afx_msg void OnColorsetsCustom12();
   afx_msg void OnTopview();

   afx_msg void OnBottomview();
   afx_msg void OnPanelView();
   afx_msg void OnBoardView();
   afx_msg void OnGerberinCreatepadstacks();
   afx_msg void OnDeriveNetlist();
   afx_msg void OnJoinAutojoin();
   afx_msg void OnTestpointAssignment();
   afx_msg void OnAssignDftAttributes();
   afx_msg void OnAutomatedAttribAssignment();
   afx_msg void OnTestAttr2Dft();
   afx_msg void OnItGate();
   afx_msg void OnItPortinstance();
   afx_msg void OnItDrillhole();
   afx_msg void OnItMechanicalpin();
   afx_msg void OnItDrillsymbol();
   afx_msg void OnItCentroid();
   afx_msg void OnItClearpad();
   afx_msg void OnItReliefpad();
   afx_msg void OnItObstacle();
   afx_msg void OnItDrcmarker();
   afx_msg void OnItTestpad();
   afx_msg void OnItSchemJunction();
   afx_msg void OnItGluepoint();
   afx_msg void OnItTestAccess();
   afx_msg void OnItRejectMarker();
   afx_msg void OnItXOut();
   afx_msg void OnItHierarchicalSymbol();
   afx_msg void OnItSheetConnector();
   afx_msg void OnItTiedot();
   afx_msg void OnItRipper();
   afx_msg void OnItGround();
   afx_msg void OnItTerminator();
   afx_msg void OnItAperture();
   afx_msg void OnItRealPart();
   afx_msg void OnItPad();
   afx_msg void OnItPackage();
   afx_msg void OnItPackagePin();
   afx_msg void OnItStencilHole();
   afx_msg void OnItComplexDrillHole();
   afx_msg void OnItCompositeComp();
   afx_msg void OnItRouteTarget();
   afx_msg void OnItDie();
   afx_msg void OnItDiePin();
   afx_msg void OnItBondPad();
   afx_msg void OnItBondWire();
   //afx_msg void OnChangeLayer();
   afx_msg void OnOriginalColors();
   afx_msg void OnRegen();
   afx_msg void OnSaveProjectFile();
   afx_msg void OnWorldView();
   afx_msg void OnFilter();
   afx_msg void OnFilterEndpoint();
   afx_msg void OnFilterEntity();
   afx_msg void OnFilterInserts();
   afx_msg void OnFilterLines();
   afx_msg void OnFilterCenter();
   afx_msg void OnFilterPoints();
   afx_msg void OnLayerManipulation();
   afx_msg void OnFitPageToImage();
   afx_msg void OnPurgeUnusedBlocks();
   afx_msg void OnPurgeUnusedWidths();
   afx_msg void OnPurgeUnusedWidthsAndGeometries();
   afx_msg void OnRotateFiles();
   afx_msg void OnOGP_SetPointType();
   afx_msg void OnMakeManufacturingRefdes();
   afx_msg void OnSetBgBmpCoords();
   afx_msg void OnOpenBgBmpFile();
   afx_msg void OnBgBmpShow();
   afx_msg void OnAutoBoardOrigin_BL();
   afx_msg void OnAutoBoardOrigin_BR();
   afx_msg void OnAutoBoardOrigin_TL();
   afx_msg void OnAutoBoardOrigin_TR();
   afx_msg void OnDrcList();
   afx_msg void OnTestabilityReport();
   afx_msg void OnFiltertypesAll();
   afx_msg void OnFiltertypesPins();
   afx_msg void OnArrangeToolbars();
   afx_msg void OnImportGeometries();
   afx_msg void OnReplaceGeometry();
   afx_msg void OnTypePinEditor();
   afx_msg void OnDebug();
   afx_msg void OnColorPaletteDialog();
   afx_msg void OnSelectStackDialog();
   afx_msg void OnGeneratePadstack();
   afx_msg void OnLocalLibrary();
   afx_msg void OnTestProbePlacement();
   afx_msg void OnGenerateBomGraphic();
   afx_msg void OnHideRemoveAll();
   afx_msg void OnPurgeUnusedLayers();
   afx_msg void OnResolveBreakoutPad();
   afx_msg void OnColorsetsCustom();
   afx_msg void OnRemoveNetnames();
   afx_msg void OnVerifyDesignData();
   afx_msg void OnDataDoctor();
   afx_msg void OnListNamedViews();
   afx_msg void OnNetlistRepair();
   afx_msg void OnNetlistLoad();
   afx_msg void OnNetlistSave();
   afx_msg void OnNetlistRemove();
   afx_msg void OnSchematicNetlistLoad();
   afx_msg void OnSchematicNetlistFromLayout();
   afx_msg void OnOutlineSelected();
   afx_msg void OnConvertPolyToZeroWidth();
   afx_msg void OnConvertThickPolyToInteriorPoly();
   afx_msg void OnDftTestprobeSequence();
   afx_msg void OnPolyShrink();
   afx_msg void OnSelfIntersect();
   afx_msg void OnSaveDatafile();
   //afx_msg void OnSaveCompressedDatafile();
   afx_msg void OnSaveDatafileAs();
   afx_msg void OnAddTA_Defaults();
   afx_msg void OnAddTA_PlaceAuto();
   afx_msg void OnAddTA_PlaceBoth();
   afx_msg void OnAddTA_PlaceBottom();
   afx_msg void OnAddTA_PlaceTop();
   afx_msg void OnTaShowTestOffsetsBottom();
   afx_msg void OnTaShowTestOffsetsTop();
   afx_msg void OnFixFreeVoids();
   afx_msg void OnBomClearColor();
   afx_msg void OnBomColorComp();
   afx_msg void OnBomSetComponentsUnloaded();
   afx_msg void OnBomSetLoaded();
   afx_msg void OnTestabilityOutlinegenerator();

   afx_msg void OnDftValidatePcb();
   afx_msg void OnDftTestPlan();
   afx_msg void OnDftExportNailAssignments();
   afx_msg void OnDftImportTesterProgram();
   afx_msg void OnDftFixtureInserts();
   afx_msg void OnDftExportDrills();
   afx_msg void OnDftExportWiringList();
   afx_msg void OnDftOptions();

   afx_msg void OnShowComponentboundary();
   afx_msg void OnShowPlacementregion();
   afx_msg void OnShowTestpointkeepin();
   afx_msg void OnShowTestpointkeepout();
   afx_msg void OnPolyRemoveHatch();
   afx_msg void OnGerberCADNetlistCompare();
// afx_msg void OnTaInheritFromFeature();
   afx_msg void OnOptimizePadstack();
   afx_msg void OnNetlistExplodeNcpins();
   afx_msg void OnNetlistMergeNcpins();
   afx_msg void OnConvertPadstacksToPinsVias();
   afx_msg void OnDfmReporting();
   afx_msg void OnSmPrep();
   afx_msg void OnSmProcess();
   afx_msg void OnSmProcessNegative();
   afx_msg void OnCheckValueAttribs();
   afx_msg void OnSetOriginalColors();
   afx_msg void OnDfmLoadAttribs();
   afx_msg void OnLayerStackup();
   afx_msg void OnHideTopLevel();
   afx_msg void OnPanInsert();
   afx_msg void OnConvertSelectedToFlash();
   afx_msg void OnConvertPolysToFlashes();
   afx_msg void OnManufacturingGrid();
   afx_msg void OnColorByAttribute();
   //afx_msg void OnLayerOffsetAuto();
   afx_msg void OnCleanGerber();
   afx_msg void OnConvertFlashesToPolys();
   afx_msg void OnGeneratePinNumbers();
   afx_msg void OnGeneratePinPitch();
   afx_msg void OnGeneratePin1Marker();
   afx_msg void OnAssignGrClass();
   //}}AFX_MSG

   // Stencil Stepper command handlers
   afx_msg void OnStencilStepper();
   afx_msg void OnTerminateStencilStepper();

   // Stencil Generator command handlers
   afx_msg void OnConnectToStencilSession();
   afx_msg void OnTerminateStencilSession();
   afx_msg void OnBrowseStencilErrors();
   afx_msg void OnTerminateBrowseStencilErrors();
   afx_msg void OnStencilPanZoom();
   afx_msg void OnStencilHidePropertySheet();
   afx_msg void OnModifiedGenerationRule();
   afx_msg void OnValidationRulesInSync();
   afx_msg void OnValidationRulesOutOfSync();
   afx_msg void OnModifiedValidationRules();
   afx_msg void OnModifiedGenerationParameters();
   afx_msg void OnStencilRendered();
   afx_msg void OnValidationPerformed();
   afx_msg void OnContinueStencilSession();

   // ------------------------------------------------
   afx_msg void OnUpdateDoneEditing(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowPinnrTop(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowPinnrBottom(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowPinnetsTop(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowPinnetsBottom(CCmdUI* pCmdUI);
   afx_msg void OnUpdateTopView(CCmdUI* pCmdUI);
   afx_msg void OnUpdateBottomView(CCmdUI* pCmdUI);
   afx_msg void OnUpdatePanelView(CCmdUI* pCmdUI);
   afx_msg void OnUpdateBoardView(CCmdUI* pCmdUI);
   afx_msg void OnUpdateNegativeView(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowGrid(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowOrigin(CCmdUI* pCmdUI);
   afx_msg void OnUpdateSnapToGrid(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowSequenceNumbers(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowSequenceArrows(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowTestOffsetsTop(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowTestOffsetsBottom(CCmdUI* pCmdUI);

   afx_msg void OnUpdateFillSolid(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFillHorizontal(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFillVertical(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFillCross(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFillDiagonal1(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFillDiagonal2(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFillDiagonalCross(CCmdUI* pCmdUI);

   afx_msg void OnUpdateFilterEndpoint(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFilter(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFilterEntity(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFilterInserts(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFilterLines(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFilterCenter(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFilterPoints(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFiltertypesAll(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFiltertypesPins(CCmdUI* pCmdUI);

   // Graphic Classes
   afx_msg void OnUpdateShowFills(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowApFills(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowAttribs(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowAllkeepin(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowAllkeepout(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowAnalysis(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowAntiEtch(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowBoardCutout(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowBoardGeom(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowBoardoutline(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowComponentBoundary(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowCompoutline(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowDrawing(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowPackageOutline(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowDrc(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowEtch(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowManufacturing(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowMilling(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowNormal(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowPanelCutout(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowPaneloutline(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowPlacekeepin(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowPlacekeepout(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowPlacementRegion(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowRoutkeepin(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowRoutkeepout(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowViakeepin(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowViakeepout(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowUnroute(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowPackageBody(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowPackagePin(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowSignal(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowBus(CCmdUI* pCmdUI);

   afx_msg void OnUpdateShowTestpointKeepin(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowTestpointKeepout(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowTraceFanout(CCmdUI* pCmdUI);

   afx_msg void OnUpdateShowPanelBorder(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowManufacturingOutl(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowContour(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowPlaneKeepout(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowPlaneNoConnect(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowBoardKeepout(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowPadKeepout(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowEPBody(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowEPMask(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowEPOverGlaze(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowEPDielectric(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowEPConductive(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowBusPath(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowRegion(CCmdUI* pCmdUI);
   afx_msg void OnUpdateShowCavity(CCmdUI* pCmdUI);


   
   // Insert Types
   afx_msg void OnUpdateItUnknown(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItVia(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItPin(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItPcbcomp(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItMechcomp(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItGenericcomp(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItPcb(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItFiducial(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItTooling(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItTestpoint(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItTestprobe(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItFreepad(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItAll(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItNone(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItGate(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItPortinstance(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItDrillhole(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItMechanicalpin(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItDrillsymbol(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItCentroid(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItClearpad(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItReliefpad(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItObstacle(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItDrcmarker(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItTestAccess(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItTestpad(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItSchemJunction(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItGluepoint(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItRejectMarker(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItXOut(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItHierarchicalSymbol(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItSheetConnector(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItTiedot(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItRipper(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItGround(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItTerminator(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItAperture(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItRealPart(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItPad(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItPackage(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItPackagePin(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItStencilHole(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItComplexDrillHole(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItCompositeComp(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItRouteTarget(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItDie(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItDiePin(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItBondPad(CCmdUI* pCmdUI);
   afx_msg void OnUpdateItBondWire(CCmdUI* pCmdUI);

   afx_msg void OnUpdateSaveDataFile(CCmdUI* pCmdUI);
   afx_msg void OnUpdateSaveCompressedDataFile(CCmdUI* pCmdUI);
   afx_msg void OnUpdateSaveDataFileAs(CCmdUI* pCmdUI);

   afx_msg void OnNoOperation();
   afx_msg void OnUpdateNoOperation(CCmdUI* pCmdUI);

   DECLARE_MESSAGE_MAP()

   // Generated OLE dispatch map functions
   //{{AFX_DISPATCH(CCEtoODBDoc)
   //}}AFX_DISPATCH
   DECLARE_DISPATCH_MAP()
   DECLARE_INTERFACE_MAP()
   afx_msg void OnCheckBrokenInserts();
   afx_msg void OnRemoveBrokenInserts();
   afx_msg void OnAttribVisibility();
   afx_msg void OnGerberInheritecaddata();
   afx_msg void OnDfmAnalysisGerberThermalDfm();
   afx_msg void OnUpdateDfmAnalysisGerberThermalDfm(CCmdUI *pCmdUI);
   afx_msg void OnSchematicChooserefdes();
   afx_msg void OnPcbAssigndevicetypes();
   afx_msg void OnToggleGeometryTree();
   afx_msg void OnUpdateToggleGeometryTree(CCmdUI *pCmdUI);
   afx_msg void OnPcbGeneratedevicetype();

   // Gerber Educator command handlers
#if defined(EnableGerberEducator)
   afx_msg void OnGerberEducator();
   afx_msg void OnUpdateGerberEducator(CCmdUI *pCmdUI);
   afx_msg void OnGerberCentroidMergeTool();
   afx_msg void OnUpdateGerberCentroidMergeTool(CCmdUI *pCmdUI);   
   //afx_msg void OnGerberEducatorCreateGeometries();
   afx_msg void OnUpdateGerberEducatorCreateGeometries(CCmdUI *pCmdUI);

   afx_msg void OnTerminateGerberEducatorSession();

   afx_msg void OnGerberEducatorGerberImport();
   afx_msg void OnGerberEducatorGenericCentroidImport();

   afx_msg void OnGerberEducatorConnectToSession();
   afx_msg void OnGerberEducatorSetupDataType();
   afx_msg void OnGerberEducatorCreateGeometries();
   afx_msg void OnGerberEducatorEditGeometries();
   afx_msg void OnGerberEducatorEditComponents();
   afx_msg void OnGerberEducatorCleanUpData();
   afx_msg void OnGerberEducatorColorsDialog();
   afx_msg void OnGerberEducatorTerminateSession();
#endif

   //afx_msg void OnDftanalysisKelvinanalysis();
   afx_msg void OnUpdateDftanalysisKelvinanalysis(CCmdUI *pCmdUI);
   afx_msg void OnPcbConvertviatopcbcomponent();
   afx_msg void OnProbableShortAnalysis();
   afx_msg void OnDftanalysisAccessanalysis();
   afx_msg void OnDftanalysisProbeplacement();
   afx_msg void OnConvertDrawsToApertures();
   afx_msg void OnUpdateConvertDrawsToApertures(CCmdUI *pCmdUI);
   afx_msg void OnVariantNavigator();
   afx_msg void OnMultipleMachineManagement();
   afx_msg void OnLineConfiguration();
   afx_msg void OnDftanalysisProbetoprobeviolationcheck();
   afx_msg void OnUpdateDftanalysisProbetoprobeviolationcheck(CCmdUI *pCmdUI);
   afx_msg void OnDftanalysisProbestylematrix();
   afx_msg void OnDftDeleteAccessMarkersTop();
   afx_msg void OnDftDeleteAccessMarkersBottom();
   afx_msg void OnDftDeleteAccessMarkersBoth();
   afx_msg void OnDftDeleteTestProbesTop();
   afx_msg void OnDftDeleteTestProbesBottom();
   afx_msg void OnDftDeleteTestProbesBoth();

public:
   afx_msg void OnConvertComplexApertureNonZeroWidthPolys();
   afx_msg void OnDftanalysisFixturereuse();
   afx_msg void OnToolsNormalizeBottomBuildGeometries();

   afx_msg void OnConsolidateGeomPinsOnly();
   afx_msg void OnConsolidateGeomStandard();
   afx_msg void OnConsolidateGeomCustom();
};

CCEtoODBDoc* getActiveDocument();

// end CCDOC.H

#endif
