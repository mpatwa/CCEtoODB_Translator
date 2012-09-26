// $Header: /CAMCAD/5.0/read_wrt/CamCadDatabase.h 105   6/21/07 8:29p Kurt Van Ness $

//   Router Solutions Inc.
//   Copyright © 2002. All Rights Reserved.

#if ! defined (__CamCadDatabase_h__)
#define __CamCadDatabase_h__

#include <afxcmn.h>
#include <afxdlgs.h>
#include "CcDoc.h"
#include "Graph.h"
#include "RwLib.h"
#include "Drc.h"
#include "Units.h"
#include "Net_Util.h"
#include "Polygon.h"
#include "DcaCamCadData.h"

class CCamCadDatabase;
class COperationProgress;
class CSegmentCluster;

// from PcbUtil.h
NetStruct *add_net(FileStruct *fl,const char *net_name);
CompPinStruct *add_comppin(FileStruct *fl, NetStruct *n, const char *comp, const char *pin);

inline CAttributes** getAttributeMap(BlockStruct* block)           { return &(block->getAttributesRef());         }
inline CAttributes** getAttributeMap(DataStruct* data)             { return &(data->getAttributesRef());          }
inline CAttributes** getAttributeMap(TypeStruct* typeStruct)       { return &(typeStruct->getAttributesRef());    }
inline CAttributes** getAttributeMap(LayerStruct* layerStruct)     { return &(layerStruct->getAttributesRef());   }
inline CAttributes** getAttributeMap(NetStruct* netStruct)         { return &(netStruct->getAttributesRef());     }
inline CAttributes** getAttributeMap(CompPinStruct* compPinStruct) { return &(compPinStruct->getAttributesRef()); }
inline CAttributes** getAttributeMap(DRCStruct* drc)               { return &(drc->getAttributesRef());           }

enum DrcClassTag
{
   drcSimple         = 0,
   drcMeasure        = 1,
   drcNet            = 2,
   drcNoMarker       = 3,
   drcClassUndefined = 4
};

enum DrcAlgorithmTag
{
   drcGeneric                       =  0,
   drcProbablePinToPinShort         =  1,
   drcProbablePinToFeatureShort     =  2,
   drcProbableFeatureToFeatureShort =  3,
   drcNetWithoutProbe               =  4,
   drcNetWithoutAccess              =  5,
   drcComponentShadow               =  6,
   drcNetCollision                  =  7,
   drcNetAdjacency                  =  8,
   drcCompToCompSpacing             =  9,
   drcCompToBoardOutlineSpacing     = 10,
   drcAlgorithmUndefined            = 11
};

enum DrcFailureRangeTag
{
   drcCriticalFailure  = 0,
   drcMarginalFailure  = 1,
   drcUndefinedFailure = 2 
};

enum SurfaceTag
{
   topSurface       = 0x1,
   bottomSurface    = 0x2,
   undefinedSurface = 0x4
};

enum ComponentMountTypeTag
{
   thruHoleMountComponent  = 0x1,
   surfaceMountComponent   = 0x2,
   undefinedMountComponent = 0x4
};

enum DataTypeMaskTag
{
   dataTypeMaskNone   = 0x00,
   dataTypeMaskDraw   = 0x01,
   dataTypeMaskInsert = 0x02,
   dataTypeMaskPoly   = 0x04,
   dataTypeMaskText   = 0x08,
 //dataTypeMaskTool   = 0x10,
   dataTypeMaskPoint  = 0x20,
   dataTypeMaskBlob   = 0x40,
   dataTypeMaskAll    = 0x6f,
};

DataTypeMaskTag dataTypeToDataTypeMask(int dataType);

Attrib* getAttribute(CAttributes*& attributeMap,int keyWordIndex);

//_____________________________________________________________________________
class CCamCadBlockMap : public CTypedPtrMap<CMapStringToPtr,CString,BlockStruct*>
{
public:
   bool contains(const CString& blockName);
};

//_____________________________________________________________________________
class CCamCadBlockMapArray : public CObject
{
private:
   CTypedPtrArray<CObArray,CCamCadBlockMap*> m_mapArray;

public:
   CCamCadBlockMapArray();
   ~CCamCadBlockMapArray();
   void deleteAll();

   CCamCadBlockMap& getAt(int index);

};

//_____________________________________________________________________________
class CCamCadBlockIndex : public CObject
{
private:
   CCEtoODBDoc& m_camCadDoc;
   CCamCadBlockMapArray m_blockMapArray;
   int m_volatileChangeDetectedCnt;
   bool m_indexedFlag;

public:
   CCamCadBlockIndex(CCEtoODBDoc& camCadDoc);

public:
   BlockStruct* getDefinedBlock(const CString& name,BlockTypeTag blockType,int fileNum);
   BlockStruct* getBlock(const CString& name,int fileNum);
   BlockStruct* copyBlock(const CString& newName,BlockStruct* srcBlock,bool copyAttributesFlag=true);
   BlockStruct* copyBlock(const CString& newName,FileStruct& fileStruct,BlockStruct* srcBlock,bool copyAttributesFlag=true);
   bool addBlock(BlockStruct* block,int fileNum = -1);
   void invalidateIndex();
   void reindex();
   void verifyIndex() { if (!m_indexedFlag) reindex(); }
};

////_____________________________________________________________________________
//class CCamCadKeywordDirectory : public CObject
//{
//private:
//   CCamCadDatabase& m_camCadDatabase;
//   CKeyWordArray& m_keywordArray;
//   CMapStringToPtr& m_valueMap;
//
//   CMapStringToInt m_keywordMap;
//
//public:
//   CCamCadKeywordDirectory(CCamCadDatabase& camCadDatabase);
//   virtual ~CCamCadKeywordDirectory();
//
//public: // VT_STRING
//   void resync();
//   bool sync();
//   int getKeywordIndex(const CString& keyword);
//   int getDefinedKeywordIndex(const CString& keyword,ValueTypeTag,
//      CWriteFormat& errorLog);
//   bool addAttribute(CAttributes** attributeMap,const CString& keyword,
//      ValueTypeTag valueType,const CString& value,CWriteFormat& errorLog);
//};

//_____________________________________________________________________________
class CCamCadComponentDirectory : public CObject
{
private:
   CCamCadDatabase& m_camCadDatabase;
   CTypedPtrMap<CMapStringToOb,CString,DataStruct*> m_components;

public:
   CCamCadComponentDirectory(CCamCadDatabase& camCadDatabase);
   virtual ~CCamCadComponentDirectory();

public: 
   void resync();
   bool sync();
   bool load();

   DataStruct* getComponent(const CString& refdes);
};

//_____________________________________________________________________________
class CCamCadDeviceMap : public CObject
{
private:
   FileStruct* m_file;
   CTypedPtrArrayWithMap<TypeStruct>* m_typeStructs;

public:
   CCamCadDeviceMap(FileStruct* file);
   virtual ~CCamCadDeviceMap();

public: 
   void resync();
   bool sync();
   TypeStruct* getType(const CString& deviceName);
   TypeStruct* getDefinedType(CCamCadDatabase& camCadDatabase,const CString& deviceName);
};

//_____________________________________________________________________________
class CCamCadDeviceDirectory : public CObject
{
private:
   CTypedObArrayContainer<CCamCadDeviceMap*> m_deviceMapArray;

public:
   CCamCadDeviceDirectory();
   //virtual ~CCamCadKeywordDirectory();

public: 
   TypeStruct* getType(FileStruct* file,const CString& deviceName);
   TypeStruct* getDefinedType(FileStruct* file,CCamCadDatabase& camCadDatabase,
      const CString& deviceName);

private:
   CCamCadDeviceMap* getAt(FileStruct* file);
};

//_____________________________________________________________________________
class CCamCadNetMap : public CObject
{
private:
   FileStruct* m_file;
   CTypedPtrArrayWithMap<NetStruct>* m_netStructs;

public:
   CCamCadNetMap(FileStruct* file);
   virtual ~CCamCadNetMap();

public: 
   void resync();
   bool sync();

   NetStruct* getNet(const CString& netName);
   NetStruct* getDefinedNet(CCamCadDatabase& camCadDatabase,const CString& netName);
};

//_____________________________________________________________________________
class CCamCadNetDirectory : public CObject
{
private:
   CTypedObArrayContainer<CCamCadNetMap*> m_netMapArray;

public:
   CCamCadNetDirectory();
   //virtual ~CCamCadNetDirectory();

public: 
   NetStruct* getNet(FileStruct* file,const CString& netName);
   NetStruct* getDefinedNet(FileStruct* file,CCamCadDatabase& camCadDatabase,
      const CString& netName);

private:
   CCamCadNetMap* getAt(FileStruct* file);
};

//_____________________________________________________________________________
class CCamCadPin : public CObject
{
private:
   NetStruct& m_net;
   CompPinStruct& m_compPin;

public:
   CCamCadPin(NetStruct& net,CompPinStruct& compPin);

   NetStruct* getNet() { return &m_net; }
   CompPinStruct* getCompPin() { return &m_compPin; }
   CString getPinRef() const { return m_compPin.getPinRef(); }
};

//_____________________________________________________________________________
class CCamCadPinMap : public CObject
{
private:
   FileStruct* m_file;
   //CTypedPtrArrayWithMap<CompPinStruct>* m_compPinStructs;
   CTypedPtrArrayWithMap<CCamCadPin>* m_camCadPins;

public:
   CCamCadPinMap(FileStruct* file);
   virtual ~CCamCadPinMap();

public: 
   void resync();
   bool sync();

   CCamCadPin* getCamCadPin(const CString& pinRef);
   CCamCadPin* getCamCadPin(const CString& refDes,const CString& pinName);
   CCamCadPin* getDefinedCamCadPin(CCamCadDatabase& camCadDatabase,const CString& refDes,
      const CString& pinName,const CString& netName);
   CompPinStruct* getPin(const CString& refDes,const CString& pinName);
   CompPinStruct* getDefinedPin(CCamCadDatabase& camCadDatabase,const CString& refDes,
      const CString& pinName,const CString& netName);

   int getSize();
   CCamCadPin* getAt(int index);
};

//_____________________________________________________________________________
class CCamCadPinDirectory : public CObject
{
private:
   CTypedObArrayContainer<CCamCadPinMap*> m_pinMapArray;

public:
   CCamCadPinDirectory();
   //virtual ~CCamCadNetDirectory();
   void empty() { m_pinMapArray.empty(); }

public: 
   CCamCadPin* getCamCadPin(FileStruct* file,const CString& refDes,const CString& pinName);
   CCamCadPin* getDefinedCamCadPin(FileStruct* file,CCamCadDatabase& camCadDatabase,
      const CString& refDes,const CString& pinName,const CString& netName);
   CompPinStruct* getPin(FileStruct* file,const CString& refDes,const CString& pinName);
   CompPinStruct* getDefinedPin(FileStruct* file,CCamCadDatabase& camCadDatabase,
      const CString& refDes,const CString& pinName,const CString& netName);
   void discardCamCadPins(FileStruct& file);

private:
   CCamCadPinMap* getAt(FileStruct* file);
};

//_____________________________________________________________________________
class CComponentPin
{
private:
   CCamCadDatabase& m_camCadDatabase;

   int m_entityNumber;
   CString m_refDes;
   CString m_pinName;
   DataStruct* m_pinVessel;

public:
   CComponentPin(CCamCadDatabase& camCadDatabase,BlockStruct& pinsVesselGeometry,
      const CString& refDes,const CString& pinName,DataStruct* pinVessel=NULL);
   ~CComponentPin();

   CCamCadData& getCamCadData();

   CString getRefDes() const { return m_refDes; }
   CString getPinName() const { return m_pinName; }
   CString getPinRef() const;
   DataStruct& getPinVessel() { return *m_pinVessel; }

   CAttributes*& getDefinedAttributes() { return m_pinVessel->getDefinedAttributes(); }
   CAttributes& attributes() { return *(m_pinVessel->getDefinedAttributes()); }
};

//_____________________________________________________________________________
class CComponentPinMap : public CObject
{
private:
   CCamCadDatabase& m_camCadDatabase;
   FileStruct& m_file;
   DataStruct* m_componentPinsVessel;
   BlockStruct* m_componentPinsVesselGeometry;
   CTypedPtrArrayWithMap<CComponentPin>* m_componentPins;

public:
   CComponentPinMap(CCamCadDatabase& camCadDatabase,FileStruct& file);
   virtual ~CComponentPinMap();

   CCamCadData& getCamCadData();

public: 
   void resync();
   bool sync();

   CComponentPin* getDefinedComponentPin(const CString& refDes,const CString& pinName);

   int getSize();
   CComponentPin* getAt(int index);
};

//_____________________________________________________________________________
class CComponentPinDirectory
{
private:
   CTypedObArrayContainer<CComponentPinMap*> m_componentPinMapArray;

public:
   CComponentPinDirectory();
   virtual ~CComponentPinDirectory();
   void empty();

public: 
   CComponentPin* getDefinedComponentPin(CCamCadDatabase& camCadDatabase,FileStruct& file,
      const CString& refDes,const CString& pinName);
   void discardComponentPins(FileStruct& file);

private:
   CComponentPinMap* getAt(CCamCadDatabase& camCadDatabase,FileStruct& file);
};

//_____________________________________________________________________________
class CCamCadWidthDirectory : public CObject
{
private:
   CCEtoODBDoc& m_camCadDoc;
   int m_nextWidthIndex;
   CTypedMapIntToPtrContainer<void*> m_blockNumberMap;  // maps block numbers to width indexes

public:
   CCamCadWidthDirectory(CCEtoODBDoc& camCadDoc);
   virtual ~CCamCadWidthDirectory();

public: 
   void resync();
   bool sync();
   int getDefinedApertureIndex(const CString& apertureName,ApertureShapeTag apertureShape,
      double sizeA,double sizeB,double xOffset,double yOffset,double rotation);
   int getDefinedApertureIndex(int fileNumber,const CString& apertureName,ApertureShapeTag apertureShape,
      double sizeA,double sizeB,double xOffset,double yOffset,double rotation);
   int getDefinedWidthIndex(BlockStruct& block);

   int getWidthIndex(int blockNum);
   int getWidthIndex(BlockStruct* block);

   void dump();
};

//_____________________________________________________________________________
class CPolygonBlock : public CPolygon
{
private:
   BlockStruct& m_block;

public:
   CPolygonBlock(const CPolygon& polygon,BlockStruct& block);

   BlockStruct& getBlock() { return m_block; }
};

//_____________________________________________________________________________
class CPolygonBlockList : public CTypedPtrListContainer<CPolygonBlock*>
{
};

//_____________________________________________________________________________
class CCamCadPolygonApertureDirectory
{
private:
   CCEtoODBDoc& m_camCadDoc;
   bool m_synchronized;
   CTypedMapStringToPtrContainer<CPolygonBlockList*> m_polygonListMap;

public:
   CCamCadPolygonApertureDirectory(CCEtoODBDoc& camCadDoc);

   BlockStruct* lookup(const CPolygon& polygon,double tolerance);
   void setAt(const CPolygon& polygon,BlockStruct& block);
   void resync();
   bool sync();

   CString getPolygonAperturePrefix() const { return "PolygonAperture_"; }
};

//_____________________________________________________________________________
class CClusterBlock
{
private:
   BlockStruct& m_block;
   CSegmentCluster* m_segmentCluster;

public:
   CClusterBlock(const CSegmentCluster& segmentCluster,BlockStruct& block);
   ~CClusterBlock();

   BlockStruct& getBlock() { return m_block; }
   CSegmentCluster& getSegmentCluster() { return *m_segmentCluster; }
};

//_____________________________________________________________________________
class CClusterBlockList : public CTypedPtrListContainer<CClusterBlock*>
{
};

//_____________________________________________________________________________
class CCamCadClusterApertureDirectory
{
private:
   CCamCadDatabase& m_camCadDatabase;
   bool m_synchronized;
   CTypedMapStringToPtrContainer<CClusterBlockList*> m_clusterListMap;

public:
   CCamCadClusterApertureDirectory(CCamCadDatabase& camCadDatabase);

   CCEtoODBDoc& getCamCadDoc();

   BlockStruct* lookup(const CSegmentCluster& segmentCluster,double tolerance);
   void setAt(const CSegmentCluster& segmentCluster,BlockStruct& block);
   void resync();
   bool sync();

   CString getClusterAperturePrefix() const { return "ClusterAperture_"; }
};

//_____________________________________________________________________________
class CCamCadLayerMask
{
private:
   CCamCadDatabase& m_camCadDatabase;
   CByteArray m_layerMask;

public:
   CCamCadLayerMask(CCamCadDatabase& camCadDatabase);

   bool add(CamCadLayerTag layerTag);
   bool add(int layerIndex);
   void addAll();
   bool remove(CamCadLayerTag layerTag);
   bool remove(int layerIndex);
   bool addLayerType(LayerTypeTag layerTypeTag);
   bool removeLayerType(LayerTypeTag layerTypeTag);

   bool contains(CamCadLayerTag layerTag) const;
   bool contains(int layerIndex) const;
   bool isEmpty() const;
};

class CComponentPin; // temp

//_____________________________________________________________________________
class CCamCadDatabase : public CObject
{
private:
   CCEtoODBDoc& m_camCadDoc;
   CCamCadBlockIndex m_blockIndex;
   //CCamCadKeywordDirectory* m_keywordDirectory;
   CCamCadDeviceDirectory m_deviceDirectory;
   CCamCadWidthDirectory m_widthDirectory;
   CCamCadPinDirectory m_pinDirectory;
   CCamCadNetDirectory m_netDirectory;
   CCamCadPolygonApertureDirectory m_polygonApertureDirectory;
   CCamCadClusterApertureDirectory m_clusterApertureDirectory;
   CComponentPinDirectory* m_componentPinDirectory;
   //CTypedPtrArrayWithMap<TypeStruct> m_types;

   int m_camCadLayerIndexes[ccLayerUndefined];
   CString m_camCadLayerNames[ccLayerUndefined];
   //LayerStruct* m_camCadLayers[ccLayerUndefined];

public:
   CCamCadDatabase(CCEtoODBDoc& camCadDoc);
   virtual ~CCamCadDatabase();

private:
   void initializeCamCadLayers();
   void reindex();
   //CCamCadKeywordDirectory* getKeywordDirectory();

   // properties
public:
   CCEtoODBDoc& getCamCadDoc() { return m_camCadDoc; }
   CCamCadData& getCamCadData() { return m_camCadDoc.getCamCadData(); }

public:
   int allocateEntityNumber();

   // polygons
   DataStruct* newPolyStruct(int layer,DbFlag flg,BOOL negative, GraphicClassTag graphicClass);
   DataStruct* addPolyStruct(BlockStruct* parentBlock,
      int layer,DbFlag flg,BOOL negative, GraphicClassTag graphicClass);
   DataStruct* addPolyStruct(CDataList& parentDataList,
      int layer,DbFlag flg,BOOL negative, GraphicClassTag graphicClass);
   DataStruct* newPolyData(const CPolygon& polygon,int layer,DbFlag flg,BOOL negative, GraphicClassTag graphicClass);
   CPoly* addPoly(DataStruct* polyStruct,int widthIndex,bool Filled,bool VoidPoly,bool Closed);
   CPoly* addClosedPoly(DataStruct* polyStruct,int widthIndex);
   CPoly* addOpenPoly(DataStruct* polyStruct,int widthIndex);
   CPoly* addFilledPoly(DataStruct* polyStruct,int widthIndex);
   CPoly* addVoidPoly(DataStruct* polyStruct,int widthIndex);
   CPnt* addVertex(CPoly* poly,double x,double y,double bulge=0.);

   // layers
   CString getCamCadLayerName(CamCadLayerTag index);
   int getCamCadLayerIndex(CamCadLayerTag index);
   CamCadLayerTag getCamCadLayerTag(int layerIndex);
   CamCadLayerTag getOppositeCamCadLayerTag(CamCadLayerTag layerTag);
   int getCamCadLayerType(CamCadLayerTag layerTag);
   unsigned long getCamCadLayerAttributes(CamCadLayerTag layerTag);
   void initializeCamCadLayer(CamCadLayerTag layerTag);

   int deleteEntitiesOnLayer(CamCadLayerTag layerTag,unsigned int dataTypeMask=0xffff);
   int deleteEntitiesOnLayer(LayerStruct& layerStruct,unsigned int dataTypeMask=0xffff);
   int deleteEntitiesOnLayers(const CCamCadLayerMask& layerMask,unsigned int dataTypeMask=0xffff,COperationProgress* operationProgress=NULL);
   int deleteEntitiesOnLayers(const CCamCadLayerMask& layerMask,COperationProgress& operationProgress);
   int deleteLayers(const CCamCadLayerMask& layerMask,COperationProgress* operationProgress=NULL);
   int deleteLayer(LayerStruct& layerStruct);

   CString getPlacementDependantLayerNameSuffix();
   CString getPlacementDependantMirroredLayerNameSuffix();
   bool isLayerCompatible(LayerStruct* layer,CamCadLayerTag layerTag);
   void associateExistingLayers();
   void standardizeLayerNames();
   bool mirrorLayers(int layerIndex0,int layerIndex1);
   int getFloatingLayerIndex();
   //CString getLayerUnrelatedNameSuffix()                  {  return " (Layer Unrelated)"; }
   LayerStruct* getSingleVisibleLayer();

   int getDefinedLayerIndex(const char* layerName,bool floatingFlag = false,int layerType=LAYTYPE_UNKNOWN);
   LayerStruct* getDefinedLayer(const char* layerName,bool floatingFlag = false,int layerType=LAYTYPE_UNKNOWN);
   CString getNewLayerName(const CString& prefix,const CString& suffixFormat,bool skipFirstSuffixFlag);
   LayerStruct* getNewLayer(const CString& prefix,const CString& suffixFormat,bool skipFirstSuffixFlag,
      LayerTypeTag layerType,bool floatingFlag);

   // returns NULL if no layer is defined at layerIndex
   LayerStruct* getLayerAt(int layerIndex);

   // returns non NULL if layerTag != ccLayerUndefined
   LayerStruct* getLayer(CamCadLayerTag layerTag);

   // returns NULL if no layer named layerName is found
   LayerStruct* getLayer(const CString& layerName) const;
   int getLayerIndex(const CString& layerName);  // returns -1 if layer does not exist
   int getLayerIndex(CamCadLayerTag layerTag);

   int getLayerIndexIfDefined(CamCadLayerTag layerTag);
   bool isLayerDefined(CamCadLayerTag layerTag);
   int getNumLayers() const;

   // Files
   FileStruct* getFile(int fileNum);
   FileStruct* getSingleVisiblePcb();
   FileStruct* getNewSingleVisiblePcb(const CString& fileName,int cadSourceType);
   FileStruct* getSingleVisiblePanel();
   FileStruct* getFirstVisibleFile();
   FileStruct* getSingleVisibleFile();

   // units
   PageUnitsTag getPageUnits();
   double convertPageUnitsTo(PageUnitsTag units,double value);
   double convertToPageUnits(PageUnitsTag units,double value);

   // inserts
   DataStruct* referenceBlock(BlockStruct* parentBlock,
      const char* blockName,int insertType,const char* refName,int layerIndex,
      int fileNum,double x=0.,double y=0.,
      double angleRadians=0.,bool mirror=false,double scale=1.,bool global=false);

   DataStruct* referenceBlock(CDataList &parentDataList,BlockStruct* childBlock,int insertType,
      const char* refName,int layerIndex,double x=0.,double y=0.,
      double angleRadians=0.,bool mirror=false,double scale=1.);

   DataStruct* referenceBlock(BlockStruct* parentBlock,BlockStruct* childBlock,int insertType,
      const char* refName,int layerIndex,double x=0.,double y=0.,
      double angleRadians=0.,bool mirror=false,double scale=1.);

   // Creates an Insert DataStruct, but does not put it in any DataList
   DataStruct* insertBlock(BlockStruct* childBlock,int insertType,
      const char* refName,int layerIndex,double x=0.,double y=0.,
      double angleRadians=0.,bool mirror=false,double scale=1.);

   // blocks
   BlockStruct* getDefinedBlock(const CString& name,BlockTypeTag blockType,int fileNum = -1);
   BlockStruct* getBlock(const CString& name,int fileNum = -1);
   BlockStruct* getBlock(int blockIndex);
   BlockStruct* getNewBlock(const CString& prefix,const CString& suffixFormat,
      BlockTypeTag blockType,int fileNum = -1);
   BlockStruct* getNewBlock(const CString& prefixFormat,
      BlockTypeTag blockType,int fileNum = -1);
   CString getNewBlockName(const CString& prefix,const CString& suffixFormat,int fileNum = -1,bool skipFirstSuffixFlag=true);
   CString getNewBlockName(CString prefixFormat,int fileNum = -1);
   bool referenceDevice(const CString& deviceName,BlockStruct* geometry,FileStruct* file);
   BlockStruct* copyBlock(const CString& newName,BlockStruct* srcBlock,bool copyAttributesFlag=true);
   BlockStruct* copyBlock(const CString& newName,FileStruct& fileStruct,BlockStruct* srcBlock,bool copyAttributesFlag=true);
   int getNumBlocks();
   BlockStruct* getDefinedCentroidGeometry(int fileNum);
   void invalidateBlockIndex();

   // apertures
   int getDefinedApertureIndex(const CString& apertureName,ApertureShapeTag apertureShape,
      double sizeA,double sizeB,double xOffset,double yOffset,double rotation);
   int getDefinedApertureIndex(int fileNumber,const CString& apertureName,ApertureShapeTag apertureShape,
      double sizeA,double sizeB,double xOffset,double yOffset,double rotation);
   BlockStruct* getDefinedAperture(const CString& apertureName,ApertureShapeTag apertureShape,
      double sizeA=0.,double sizeB=0.,double xOffset=0.,double yOffset=0.,double rotation=0.);
   BlockStruct* getDefinedAperture(int fileNumber,const CString& apertureName,ApertureShapeTag apertureShape,
      double sizeA=0.,double sizeB=0.,double xOffset=0.,double yOffset=0.,double rotation=0.);
   BlockStruct* getDefinedPolygonAperture(CPolygon& polygon,double width,double tolerance,CTMatrix& matrix);
   BlockStruct* getDefinedPolyAperture(const CPoly& poly,double width,double tolerance,CTMatrix& matrix);
   BlockStruct* getDefinedClusterAperture(CSegmentCluster& segmentCluster,
      CPolyList& polyList,double width,double tolerance,CTMatrix& matrix);

   // widths and tools
   int getDefinedWidthIndex(double width);
   int getDefinedWidthIndex(BlockStruct& block);
   int getDefinedSquareWidthIndex(double width);
   int getWidthIndex(int blockNum);
   int getWidthIndex(BlockStruct* block);
   int getZeroWidthIndex();
   BlockStruct* getDefinedTool(double toolSize);

   // nets, pins, device types
   NetStruct* getDefinedNet(const CString& netName,FileStruct* fileStruct);
   NetStruct* getNet(const CString& netName,FileStruct* fileStruct);
   TypeStruct* getDefinedType(const CString& typeName,FileStruct *file);
   TypeStruct* getType(const CString& typeName,FileStruct *file);
   CompPinStruct* addCompPin(NetStruct* net,const CString& refDes,const CString& pinNumber);
   CompPinStruct* getDefinedCompPin(NetStruct* net,const CString& refDes,const CString& pinNumber);
   CCamCadPin* getDefinedCamCadPin(FileStruct* file,const CString& refDes,const CString& pinName,
      const CString& netName);
   CCamCadPin* getCamCadPin(FileStruct* file,const CString& refDes,const CString& pinName);
   CompPinStruct* getDefinedPin(FileStruct* file,const CString& refDes,const CString& pinName,
      const CString& netName);
   CompPinStruct* getPin(FileStruct* file,const CString& refDes,const CString& pinName);
   void deletePin(FileStruct* file,const CString& refDes,const CString& pinName);
   void deletePins(FileStruct* file,const CString& refDes);
   void discardCamCadPins(FileStruct& file);
   static CString getUnusedPinNetName();

   // components
   BlockStruct& generatePinnedComponent(BlockStruct& pinlessGeometry);
   void restructurePadstackGeometry(CDataList& padstackDataList,const CString& padstackName,const CDataList& sourceDataList,
      int fileNumber,bool flattenRegularComplexAperturesFlag=true);
   BlockStruct& restructureFiducialGeometry(const CString& geometryName,BlockStruct& fiducialGeometry,int insertLayerIndex);

   // component pins
   CComponentPin* getDefinedComponentPin(FileStruct& file,const CString& refDes,const CString& pinName);

   // text
   DataStruct* addText(BlockStruct* parentBlock,
      int layerIndex,const CString& text,double x,double y,double height,double charWidth,double angle=0.,DbFlag flag=0,
      bool proportional=false,bool mirror=false,int oblique=0,bool negative=false,int penWidthIndex=-1,bool specialChar=false);

   DataStruct* addText(CDataList &parentDataList,
      int layerIndex,const CString& text,double x,double y,double height,double charWidth,double angle=0.,DbFlag flag=0,
      bool proportional=false,bool mirror=false,int oblique=0,bool negative=false,int penWidthIndex=-1,bool specialChar=false);

   // keywords
   int getKeywordIndex(const char* keywordName);
   int registerKeyword(const char* keywordName,int valueType);
   int registerKeyWord(CString keyword,int section,ValueTypeTag valueType,
      CWriteFormat& errorLog);
   int registerHiddenKeyWord(CString keyword,int section,ValueTypeTag valueType,
      CWriteFormat& errorLog);
   int registerKeyWord(CString keyword,int section,ValueTypeTag valueType,bool hiddenFlag,
      CWriteFormat& errorLog);
   const KeyWordStruct* getKeywordAt(int keywordIndex);
   const KeyWordStruct* getKeyword(const CString& keyword);

   // attributes
   bool setAttrib(CAttributes** map,int keyword,ValueTypeTag valueType,
      void *value,AttributeUpdateMethodTag method,Attrib **attribPtr,CWriteFormat& errorLog);
   Attrib* addAttribute(CAttributes&  attributes,int keyWordIndex,const CString& attributeValue);
   Attrib* addAttribute(CAttributes&  attributes,int keyWordIndex,int attributeValue);
   Attrib* addAttribute(CAttributes&  attributes,int keyWordIndex,double attributeValue);
   Attrib* addAttribute(CAttributes&  attributes,int keyWordIndex,float attributeValue);
   Attrib* addAttribute(CAttributes** attributeMap,int keyWordIndex,const CString& attributeValue);
   void addAttribute(CAttributes** attributeMap,int keyWordIndex,const CString& attributeValue,CWriteFormat& errorLog);
   void addAttribute(CAttributes** attributeMap,int keyWordIndex,int attributeValue,CWriteFormat& errorLog);
   void addAttribute(CAttributes** attributeMap,int keyWordIndex,double attributeValue,CWriteFormat& errorLog);
   void addAttribute(CAttributes** attributeMap,int keyWordIndex,float attributeValue,CWriteFormat& errorLog);  //for "UNIT DOUBLE" type
   void addAttribute(CAttributes** attributeMap,int keyWordIndex,CWriteFormat& errorLog);
   void addAttribute(CAttributes** attributeMap,const CString& keyword,
      ValueTypeTag valueType,const CString& value,CWriteFormat& errorLog);
   void removeAttribute(CAttributes** attributeMap,int keyWordIndex);
   void removeAttribute(CAttributes** attributeMap,const CString& keyword);

   CString getAttributeStringValue(Attrib* attribute);
   bool getAttributeStringValue(CString& stringValue,CAttributes** attributes,int keywordIndex);
   bool getAttributeStringValue(CString& stringValue,CAttributes& attributes,int keywordIndex);

   // DRCs
   DRCStruct* addDrc(FileStruct& file,double x,double y,const CString& string,
      DrcClassTag drcClass,DrcFailureRangeTag failureRange,int algorithmIndex,
      DrcAlgorithmTag algorithmType,const CString& comment="");

   // Transmutation - reconstruct data from one CCamCadDatabase in another
   void transmute(CDataList& dataList,CCamCadDatabase& sourceCamCadDatabase);

   // Miscellaneous
   bool hasRecursiveReference();
   DataStruct* drawAxes(const CTMatrix& matrix,const CString& layerName);
};

#endif


