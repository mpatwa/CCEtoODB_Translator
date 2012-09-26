
#if !defined(__CamCadData_h__)
#define __CamCadData_h__

#pragma once

#include "DcaContainer.h"
//#include "Block.h"
//#include "File.h"
//#include "Settings.h"
//#include "NameView.h"

#include "DcaDblRect.h"
#include "DcaBlock.h"
#include "DcaFile.h"
#include "DcaLayer.h"
#include "DcaCamCadDataSettings.h"
#include "DcaKeyword.h"
#include "DcaNamedView.h"
#include "DcaGraphicClass.h"
#include "DcaEntityNumber.h"
#include "DcaTopics.h"
#include "DcaFont.h"
#include "DcaTable.h"

#define QZeroWidth  "Zero Width"
#define QSmallWidth "Small Width"

#define SOURCE_PRODUCT_GRAPHIC            "CAMCAD Graphic"
#define SOURCE_PRODUCT_PROFESSIONAL       "CAMCAD Professional"
#define SOURCE_PRODUCT_PCB_TRANSLATOR     "CAMCAD PCB Translator"
#define SOURCE_PRODUCT_VISION             "CAMCAD Vision"

class CPoly;
class Point2;
class Mat2x2;     // temporary, for deprecated functions
class ExtentRect; // temporary, for deprecated functions

//class CExtent;

enum CamCadFileTypeTag;
enum FileStatusTag;
enum HorizontalPositionTag;
enum VerticalPositionTag;
enum LayerTypeTag;
enum CamCadLayerTag;

enum PadStackAccessFlag 
{
   padStackAccessNone       = 0x00,
   padStackAccessTop        = 0x01,
   padStackAccessBottom     = 0x02,
   padStackAccessMaskTop    = 0x04,
   padStackAccessMaskBottom = 0x08,
   padStackAccessTopOnly    = 0x10,
   padStackAccessBottomOnly = 0x20,
   padStackAccessInner      = 0x40
};

//_____________________________________________________________________________
//class CDcaErrorHandler
//{
//private:
//
//public:
//   void errorMessage(const CString& message,const CString& caption,
//};

//_____________________________________________________________________________
struct HighlightedPinStruct
{
   CString comp, pin;
   COLORREF color;
};

//_____________________________________________________________________________
class CMapBlockNumberToBlock : public CTypedMapIntToPtrContainer<BlockStruct*>
{
public:
   CMapBlockNumberToBlock(int blockSize=10,bool isContainer=true);

   void setAt(BlockStruct* block);
};

//_____________________________________________________________________________
class CAttributeValueDictionary
{
private:
   CStringArray m_valueArray;
   CMapStringToPtr m_valueMap;

public:
   CAttributeValueDictionary();
   ~CAttributeValueDictionary();

   const CStringArray& getValueArray() const;
   const CMapStringToPtr& getValueMap() const;

   int getSize() const;
   int getValueIndex(const CString& value) const;
   int add(const CString& value);
   CString getAt(int index) const;
};

//_____________________________________________________________________________
class CWidthTableArray // : public CTypedPtrArray<CPtrArray, BlockStruct*>
{
private:
   CTypedPtrArray<CPtrArray, BlockStruct*> m_widthArray;
   CTypedMapIntToPtrContainer<void*> m_blockNumberMap;  // maps block numbers to width indexes

   int m_zeroWidthIndex;
   int m_smallWidthIndex;

public:
   CWidthTableArray();

   void empty();

   void SetAtGrow(int index,BlockStruct* block);
   int Add(BlockStruct* block);

   int GetSize() const;
   int GetCount() const;

   BlockStruct* GetAt(int widthIndex) const;
   BlockStruct* operator [](int widthIndex) const;

   BlockStruct* getAt(int widthIndex) const;
   int getWidthIndex(BlockStruct* widthBlock) const;

   void SetAt(int widthIndex,BlockStruct* widthBlock);

   int getDefinedWidthIndex(BlockStruct& block);

   int getZeroWidthIndex() const;
   void setZeroWidthIndex(int widthIndex);

   int getSmallWidthIndex() const;
   void setSmallWidthIndex(int widthIndex);

private:
   void removeWidth(int widthIndex);
   void removeFromMap(int blockNumber);
   void addToMap(int blockNumber,int widthIndex);
   int lookupFromMap(int blockNumber) const;
};

//_____________________________________________________________________________
//class CCamCadWidthDirectory : public CObject
class CWidthDirectory
{
private:
   CCamCadData& m_camCadData;
   CWidthTableArray m_widthArray;
   //int m_nextWidthIndex;
   CTypedMapIntToPtrContainer<void*> m_blockNumberMap;  // maps block numbers to width indexes

public:
   CWidthDirectory(CCamCadData& m_camCadData);
   virtual ~CWidthDirectory();

public: 
   int getDefinedApertureIndex(const CString& apertureName,ApertureShapeTag apertureShape,
      double sizeA,double sizeB,double xOffset,double yOffset,double rotation);
   int getDefinedWidthIndex(BlockStruct& block);

   int getWidthIndex(int blockNum);
   int getWidthIndex(BlockStruct* block);

   void dump();
};

//_____________________________________________________________________________
class BackgroundBitmapStruct
{
public:
   CDblRect placementRect;
   CBitmap bitmap;
   CString filename;
   BOOL show;

public:
   CString getFileName() const { return filename; }
   BOOL getShow()        const { return show;     }

   double getXmin() const { return placementRect.xMin; }
   double getXmax() const { return placementRect.xMax; }
   double getYmin() const { return placementRect.yMin; }
   double getYmax() const { return placementRect.yMax; }
}; 

//_____________________________________________________________________________
class CCamCadData
{
private:
   //CBlockArray         m_blockArray;
   CEntityNumberGenerator m_entityNumberGenerator;
   CBlockDirectory        m_blockDirectory;
   CFileList              m_fileList;  
   CLayerArray            m_layerArray; 
   CCamCadDataSettings    m_camCadDataSettings;
   CGTabTableList         m_tableList;
   CWidthTableArray       m_widthTable;
   CKeyWordArray          m_keyWordArray;
   CStringArray           m_dfmAlgorithmNamesArray;
   CNamedViewList         m_namedViewList;
   BackgroundBitmapStruct m_topBackgroundBitMap;
   BackgroundBitmapStruct m_bottomBackgroundBitMap;
   CTopicsList             m_TopicList; 
   CString m_redLineString; 
   CString m_lotsString; 
   CString m_colorSetsString;
   CString m_ccPath;
   CString m_userPath;

   //CStringArray m_attrbuteStringValueArray;  // corresponds to CCEtoODBDoc::ValueArray
   CAttributeValueDictionary m_attributeValueDictionary;  // corresponds to CCEtoODBDoc::ValueArray
   CTypedPtrArray<CPtrArray,KeyWordStruct*> m_standardKeywordArray;  // indexed by StandardAttributeTag

   //int m_nextWidthIndex;
   //int m_zeroWidthIndex;
   //int m_dfmAlgorithmArraySize;

   CLayerFilterStack m_selectLayerFilterStack;
   CInsertTypeFilter m_insertTypeFilter;
   CGraphicsClassFilter m_graphicsClassFilter;

	CString m_sourceProduct;
   int m_sourceVersionMajor;
   int m_sourceVersionMinor;
   int m_sourceVersionRevision;

public:
   CCamCadData();
   ~CCamCadData();

public:
   CBlockDirectory&       getBlockDirectory() ;
   CBlockArray& getBlockArray();
   CFileList& getFileList();
   CCamCadDataSettings& getCamCadDataSettings();
   const CCamCadDataSettings& getCamCadDataSettings() const;
   CGTabTableList& getTableList();

   CLayerArray& getLayerArray();
   const CLayerArray& getLayerArray() const;

   CKeyWordArray& getKeyWordArray();
   const CKeyWordArray& getKeyWordArray() const;

   CTopicsList& getTopicsList();

   CNamedViewList& getNamedViewList();
   BackgroundBitmapStruct& getTopBackgroundBitMap();
   BackgroundBitmapStruct& getBottomBackgroundBitMap();
   CString& getRedLineString();
   CString& getLotsString();
   CString& getColorSetsString();
   CWidthTableArray& getWidthTable();
   int getNextWidthIndex();
   CStringArray& getDfmAlgorithmNamesArray();
   int getDfmAlgorithmArraySize();

   CAttributeValueDictionary& getAttributeValueDictionary();
   const CAttributeValueDictionary& getAttributeValueDictionary() const;

   CString getCcPath() const;
   void    setCcPath(const CString& path);

   CString getUserPath() const;
   void    setUserPath(const CString& path);

   CString getSourceProduct() const;
   void setSourceProduct(const CString& sourceProduct);

   int getSourceVersionMajor() const;
   void setSourceVersionMajor(int sourceVersionMajor);

   int getSourceVersionMinor() const;
   void setSourceVersionMinor(int sourceVersionMinor);

   int getSourceVersionRevision() const;
   void setSourceVersionRevision(int sourceVersionRevision);

public:
   // units
   PageUnitsTag getPageUnits() const;
   void setPageUnits(PageUnitsTag pageUnits);

   double convertPageUnitsTo(PageUnitsTag units,double value);
   double convertToPageUnits(PageUnitsTag units,double value);

   // Entity Numbers
   int allocateEntityNumber();
   int allocateEntityNumber(int& entityNumber);
   int getCurrentEntityNumber();
   //int setAsAllocated(int allocatedEntityNumber);

   // Files
   FileStruct* getFile(int fileNum);
   FileStruct* getSingleVisiblePcb();
   FileStruct* getNewSingleVisiblePcb(const CString& fileName,int cadSourceType);
   FileStruct* getSingleVisiblePanel();
   FileStruct* getFirstVisibleFile();
   FileStruct* getSingleVisibleFile();

   // blocks
   BlockStruct& getDefinedWidthBlock(int widthIndex);
   BlockStruct& getDefinedBlock(int blockNumber);
   BlockStruct& getDefinedBlock(int blockNumber,const CString& name,int fileNumber,BlockTypeTag blockType);
   BlockStruct& getDefinedBlock(const CString& name,BlockTypeTag blockType,int fileNum);
   BlockStruct* getWidthBlock(int widthIndex);
   int getMaxBlockIndex();
   BlockStruct* getBlockAt(int index);
   BlockStruct* getNewBlock(const CString& prefix,const CString& suffixFormat,BlockTypeTag blockType,int fileNum = -1);
   BlockStruct* getNewBlock(const CString& prefixFormat,BlockTypeTag blockType,int fileNum = -1);
   BlockStruct& getNewBlock(const CString& name,int fileNumber,BlockTypeTag blockType);
   BlockStruct* getBlock(const CString& name,int fileNum = -1);
   BlockStruct* getBlock(int blockNumber) const;
   CString getNewBlockName(const CString& prefix,const CString& suffixFormat,int fileNum = -1,bool skipFirstSuffixFlag=true);
   CString getNewBlockName(CString prefixFormat,int fileNum = -1);

   void mapUsedBlocks(CMapBlockNumberToBlock& map,BlockStruct& block);
   void mapUsedWidths(CMapBlockNumberToBlock& map,BlockStruct& block);

   int purgeUnusedBlocks(CWriteFormat* writeFormat=NULL);
   int purgeUnusedWidths(CWriteFormat* writeFormat=NULL);
   int purgeUnusedWidthsAndBlocks(CWriteFormat* reportWriteFormat=NULL);

   // apertures
   int getDefinedApertureIndex(const CString& apertureName,ApertureShapeTag apertureShape,
      double sizeA=0.,double sizeB=0.,double xOffset=0.,double yOffset=0.,double rotationRadians=0.,int fileNumber= -1,DbFlag flags=0,int dCode=0);
   BlockStruct& getDefinedAperture(const CString& apertureName,ApertureShapeTag apertureShape,
      double sizeA=0.,double sizeB=0.,double xOffset=0.,double yOffset=0.,double rotationRadians=0.,int fileNumber= -1,DbFlag flags=0,int dCode=0,
      int* definedApertureIndex=NULL);
   BlockStruct& getDefinedComplexAperture(const CString& apertureName,const CString& subBlockName,
      double xOffset=0.,double yOffset=0.,double rotationRadians=0.,int fileNumber= -1,BlockTypeTag blockType = blockTypeUndefined,int dCode=0);

   // data lists
   int removeDataFromDataListByGraphicClass(BlockStruct& block,GraphicClassTag graphicClass);
   int removeDataFromDataListByGraphicClass(CDataList& dataList,GraphicClassTag graphicClass);

   // data
   //DataStruct* addDataStruct(BlockStruct& block,DataTypeTag dataType,int layerIndex,GraphicClassTag graphicClass,DbFlag flags = 0,bool negative = false);
   DataStruct* getNewDataStruct(DataTypeTag dataType,int entityNumber = -1);
   DataStruct* getNewDataStruct(DataStruct& other,bool copyAttributesFlag=true,int entityNumber = -1);
   DataStruct* insertBlock(BlockStruct& childBlock,InsertTypeTag insertType,const CString& refName,int layerIndex,
      double x=0.,double y=0.,double angleRadians=0.,bool mirror=false,double scale=1.);

   // polys
   DataStruct* newPolyStruct(int layerIndex,GraphicClassTag graphicClass,DbFlag flags=0,bool negative=false);
   DataStruct* addPolyStruct(BlockStruct& parentBlock,int layerIndex,GraphicClassTag graphicClass,DbFlag flags=0,bool negative=false);
   DataStruct* addPolyStruct(CDataList& parentDataList,int layerIndex,GraphicClassTag graphicClass,DbFlag flags=0,bool negative=false);
   //DataStruct* newPolyData(const CPolygon& polygon,int layerIndex,GraphicClassTag graphicClass,DbFlag flags=0,bool negative=false);
   CPoly* addPoly(      DataStruct& polyStruct,int widthIndex,bool Filled,bool VoidPoly,bool Closed);
   CPoly* addClosedPoly(DataStruct& polyStruct,int widthIndex);
   CPoly* addOpenPoly(  DataStruct& polyStruct,int widthIndex);
   CPoly* addFilledPoly(DataStruct& polyStruct,int widthIndex);
   CPoly* addVoidPoly(  DataStruct& polyStruct,int widthIndex);

   // vertices
   CPnt* addVertex(CPoly& poly,double x,double y,double bulge=0.);

   // polygon

   // text
   DataStruct* addText(BlockStruct& parentBlock,
      int layerIndex,const CString& text,double x,double y,double height,double charWidth,double angle=0.,DbFlag flag=0,
      bool proportional=false,bool mirror=false,int oblique=0,bool negative=false,int penWidthIndex=-1,bool specialChar=false);

   DataStruct* addText(CDataList& parentDataList,
      int layerIndex,const CString& text,double x,double y,double height,double charWidth,double angle=0.,DbFlag flag=0,
      bool proportional=false,bool mirror=false,int oblique=0,bool negative=false,int penWidthIndex=-1,bool specialChar=false);


   // attributes
   CAttribute* constructAttribute();
   CAttribute* constructAttribute(ValueTypeTag valueType);

   bool setAttribute(CAttributes& attributes,int keyword,ValueTypeTag valueType,void* value,AttributeUpdateMethodTag updateMethod,CAttribute** attribute);
   CAttribute* setAttribute(CAttributes& attributes,int keyword,const CString& value);  // SA_OVERWRITE
   CAttribute* setAttribute(CAttributes& attributes,StandardAttributeTag attributeTag,const CString& value);  // SA_OVERWRITE

   bool getAttributeStringValue(CString& attributeStringValue,CAttributes& attributes,StandardAttributeTag attributeTag) const;
   CString getAttributeStringValue(const CAttribute& attribute) const;
   double getAttributeDoubleValue(const CAttribute& attribute) const;
   int getAttributeIntegerValue(const CAttribute& attribute) const;

   int getAttributeKeywordIndex(StandardAttributeTag attributeTag);
   int getAttributeKeywordIndex(const CString& attributeName) const;
   int getDefinedAttributeKeywordIndex(const CString& attributeName,ValueTypeTag valueType);
   const KeyWordStruct* getAttributeKeyword(StandardAttributeTag attributeTag);
   const KeyWordStruct* getAttributeKeyword(int keywordIndex) const;
   int getDefinedAttributeStringValueIndex(const CString& stringValue);

   CString getAttributeValueDictionaryStringValue(int stringIndex) const;
   int addAttributeValueDictionaryStringValue(const CString& value);
   int getAttributeValueDictionarySize(const CString& value) const;
   int getAttributeValueDictionaryValueIndex(const CString& value) const;

   // layers
   LayerStruct* getLayer(int layerIndex) const;
   LayerStruct& getDefinedLayer(CamCadLayerTag layerTag);
   LayerStruct& getDefinedLayer(int layerIndex);
   LayerStruct& getDefinedLayer(const CString& layerName,bool floatingFlag=false);
   LayerStruct& getDefinedLayer(const CString& layerName,bool floatingFlag,LayerTypeTag layerType);
   int getLayerIndex(CamCadLayerTag layerTag);
   int propagateLayer(int parentLayerIndex,int childLayerIndex);
   LayerStruct& getDefinedFloatingLayer();
   int getDefinedLayerIndex(const CString& layerName,bool floatingFlag = false);
   int getDefinedLayerIndex(const CString& layerName,bool floatingFlag,LayerTypeTag layerType);
   bool isFloatingLayer(int layerIndex) const;
   bool isLayerPairVisible(int layerIndex) const;
   bool isLayerVisible(int layerIndex,int mirrorFlags) const;

   // extents
   //void calculateBlockExtents(BlockStruct& block);
   CExtent blockExtents(CDataList& DataList,double insert_x,double insert_y,double rotation,int mirror,double scale,
      int insertLayer,bool checkOnlyVisibleEntitiesFlag,CMessageFilter* messageFilter);
   //CExtent getPolyExtents(CPolyList& polylist,const CTMatrix& matrix,bool useWidth) const;  // use CPolyList::getExtent()
   bool validateBlockExtents(BlockStruct& block,bool checkOnlyVisibleEntitiesFlag);

   // widths
   int getZeroWidthIndex();
   int getSmallWidthIndex();
   int getDefinedWidthIndex(double width);
   double getWidth(int widthIndex) const;
   int getDefinedToolIndex(double size,const CString& toolName="",bool plated=true);

   // select
   bool isInsertTypeVisible(InsertTypeTag insertType);
   bool isGraphicClassVisible(GraphicClassTag graphicClass);

   // miscellaneous
   CInsertTypeFilter& getInsertTypeFilter();
   CLayerFilterStack& getSelectLayerFilterStack();
   CGraphicsClassFilter& getGraphicsClassFilter();
   int getDefinedDfmAlgorithmNameIndex(const CString& algorithmName);
   DataStruct* drawAxes(const CTMatrix& matrix,const CString& layerName);

   // PCB Utilities
   void generateNoConnectNets(int fileNumber,CDcaOperationProgress* progress);
   void generatePadstackAccessFlag(bool alwaysFlag,CDcaOperationProgress* progress);
   int getPadstackAccessLayer(CDataList& dataList);
   void refreshTypetoCompAttributes();
   void normalizeBottomBuiltGeometries();
   BlockStruct* createTopDefinedGeometry(int bottomBuiltBlockNum,CMapBlockNumberToBlock& bottomBuiltGeometryMap,int& mirCount);
   BlockStruct& getDefinedGluePointGeometry();

   // pins
   int generatePinLocations(CWriteFormat* reportWriteFormat,CDcaOperationProgress* progress);
   int generatePinLocation(int fileNumber,bool recalculateFlag,CWriteFormat* reportWriteFormat,CDcaOperationProgress* progress);
   LayerStruct* getUltimateLayer(DataStruct& insertData);

   // interface
   virtual void setMaxCoords();
   virtual void loadBackgroundBitmap(BackgroundBitmapStruct& backgroundBitMap);
   virtual void discardData(CDataList& dataList) const;
   virtual bool showAttributes() const;
   virtual bool editingGeometry() const;

   // to be deprecated =============================================================================================================================
   //BlockStruct* Add_Blockname(const char *name, int filenum, DbFlag flg, BOOL Aperture, BlockTypeTag blockType = blockTypeUndefined);

   bool setVisibleAttribute(CAttributes& attributes, int keywordIndex,
      ValueTypeTag valueType, void* value,
      double x, double y, double rotationRadians, double height, double width, bool proportionalSpacing, int penWidthIndex,
      bool visible, AttributeUpdateMethodTag updateMethod, DbFlag flags, short layerIndex, 
      bool mirrorDisabled, bool mirrorFlip,
      HorizontalPositionTag horizontalPosition, VerticalPositionTag verticalPosition,
      CAttribute **createdAttrib = NULL);

#if defined(EnableDcaCamCadDocLegacyCode)
private:
   // Deprecated, must be rewritten
   CExtent blockExtents(CDataList* DataList,     
      double insert_x,double insert_y,double rotation,int mirror,double scale,int insertLayer,bool checkOnlyVisibleEntitiesFlag,CMessageFilter* messageFilter=NULL);
   CExtent CCamCadData::PolyExtents(CPolyList *polylist, double scale, int mirror,
      double insert_x, double insert_y, Mat2x2 *m, int use_width);
   bool CCamCadData::PolyExtents(CPolyList *polylist, ExtentRect *extents, double scale, int mirror,
      double insert_x, double insert_y, Mat2x2 *m, int use_width);
   bool validateBlockExtents(BlockStruct* block,bool checkOnlyVisibleEntitiesFlag);
   bool block_extents(double *lxmin, double *lxmax, double *lymin, double *lymax, CDataList *DataList,     
      double insert_x, double insert_y, double rotation, int mirror, double scale, int insertLayer, BOOL onlyVisible);
   CExtent VisibleAttribsExtents(CAttributes* map,Point2* pnt, 
      double block_scale, double block_rot, int block_mir);
   CExtent VisibleAttribExtents(CAttribute* attrib,Point2* pnt, double block_scale, double block_rot, int block_mir);
#endif


};

#endif
