// $Header: /CAMCAD/5.0/FlashRecognizer.h 51    6/21/07 8:26p Kurt Van Ness $

#if !defined(__FlashRecognizer_h__)
#define __FlashRecognizer_h__

#pragma once

#include "Polygon.h"
#include "StandardAperture.h"
#include "ProfileLib.h"
#include "Layer.h"
#include "DcaLine2d.h"

class CSegmentCluster;
class CFlashRecognizerParameters;
class CFlashRecognizer;
class CPoly;
class CCEtoODBDoc;
class CMessageFilter;

enum FlashRecognitionMethodTag
{
   flashRecognitionMethodSingleFlash,
   flashRecognitionMethodSingleSegment,
   flashRecognitionMethodOrthogonalRectangle,
   flashRecognitionMethodOrthogonalRectangleVariant,
   flashRecognitionMethodThreeEntityOblong,
   flashRecognitionMethodCircularBoundary,
   flashRecognitionMethodPolygonBoundary,
   flashRecognitionMethodPolygonMerged,
   flashRecognitionMethodRegionMerged,
   flashRecognitionMethodComplexCluster,
   flashRecognitionMethodUndefined
};

CString flashRecognitionMethodToFriendlyString(FlashRecognitionMethodTag recognitionMethod);

/////////////////////////////////////////////////////////////////////////////////////////////

//#include "DcaPoly.h"

//*rcf  THIS IS MESSY, change to regular include and put path in project
#include "GL.h"

class ScannerEngine
{
private:
   GeomHdr *m_geomhdr;
   GeomHdr *m_mergedGeomhdr;

   CCamCadData *m_camCadData;
   int m_camcadPolyListLayerIndex;

   void AddPoly(CPoly *poly);
   void AddPolyList(CPolyList *polylist);

   CPoly *GetNewPoly(const GeomHdr *gh);

public:
   ScannerEngine(CCamCadData *camCadData);
   ~ScannerEngine();
   
   void ClearGeom();  // Remove existing data, keep geomhdr
   void ScannerInit();

   void AddPolyListData(DataStruct *polylistData);
   DataStruct *GetNewPolyListData();
   DataStruct *GetOriginalPolyListData();

   void GetCounts(CPoly *poly, int &straights, int &arcs);

   void SetOrientation();
   void Merge();

   // SysGl uses integer coords.
   // CAMCAD uses doubles, and offers users up to 6 decimal places accuracy.
   double GetConversionFactor()     { return 1.0E6; }

};

/////////////////////////////////////////////////////////////////////////////////////////////

class CComplexAperturePolyConverter
{
   // Converts non-zero width polys used in complex apertures to zero-width outline polys.
   // Uses Scanner Engine to process the geometry.

private:
   CCamCadData &m_camcadData;

public:
   CComplexAperturePolyConverter(CCamCadData &m_camcadData);

   void ConvertGeometries();


};

/////////////////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
class CSegmentPolygon : public CPolygon
{
private:
   int m_id;

public:
   CSegmentPolygon(int id,PageUnitsTag pageUnits);

   int getId() const { return m_id; }
};

//_____________________________________________________________________________
class CSegmentPoly : public CPoly
{
private:
   int m_id;

public:
   CSegmentPoly(int id,CPoly& poly);

   int getId() const { return m_id; }
};

//_____________________________________________________________________________
class CSegment
{
private:
   static int m_nextId;

   CCamCadDatabase& m_camCadDatabase;
   int m_id;
   CLine2d m_line;
   double m_width;
   bool m_roundedFlag;
   DataStruct* m_parent;
   CSegmentPolygon* m_parentPolygon;
   CSegmentPoly* m_parentPoly;

   mutable CExtent* m_extent;
   mutable CExtent* m_pointExtent;

public:
   CSegment(const CSegment& other);
   CSegment(CCamCadDatabase& camCadDatabase,const CPoint2d& p0,const CPoint2d& p1,double width,bool roundedFlag=true);
   CSegment(CCamCadDatabase& camCadDatabase,const CLine2d& line,double width,DataStruct* parent,bool roundedFlag,
      CSegmentPolygon* parentPolygon,CSegmentPoly* parentPoly);
   ~CSegment();

   // accessors
   int getId() const { return m_id; }

   CCamCadDatabase& getCamCadDatabase() { return m_camCadDatabase; }
   PageUnitsTag getPageUnits() const;

   const CPoint2d& get0() const { return m_line.get0(); }
   void set0(const CPoint2d& point);

   const CPoint2d& get1() const { return m_line.get1(); }
   void set1(const CPoint2d& point);

   const CPoint2d& getEndPoint(int endIndex) const { return m_line.getEndPoint(endIndex); }
   void setEndPoint(int endIndex,const CPoint2d& point);

   const CLine2d& getLine() const { return m_line; }

   double getWidth() const { return m_width; }
   void setWidth(double width);

   bool getRounded() const { return m_roundedFlag; }
   void setRounded(bool roundedFlag);

   CPoint2d getOrigin() const;
   const CExtent& getExtent() const;
   const CExtent& getPointExtent() const;

   DataStruct* getParent() const { return m_parent; }

   CSegmentPolygon* getParentPolygon() const { return m_parentPolygon; }
   void setParentPolygon(CSegmentPolygon* parentPolygon) { m_parentPolygon = parentPolygon; }

   CSegmentPoly* getParentPoly() const { return m_parentPoly; }

   // operations
   double getEndpointDistanceSquared(const CSegment& other) const;
   bool isInside(const CPolygon& polygon) const;
   void transform(const CTMatrix& matrix);

   // exterior length
   double getExteriorLength() const; 
   double getLineLengthSquared() const { return m_line.getLengthSquared(); }

   bool intersects(const CSegment& other);
   void getLinePolygon(CPolygon& polygon) const;
   void getSquaredPolygon(CPolygon& polygon) const;
   void getRoundedPolygon(CPolygon& polygon) const;
   void getPolygon(CPolygon& polygon) const;

   void getPoly(CCamCadDatabase& camCadDatabase,CPoly& poly) const;
   void getPoly(CPoly& poly) const;

   CPoly* addPoly(CCamCadDatabase& camCadDatabase,DataStruct& polyData,int widthIndex) const;
   CPoly* makeNewPoly(int widthIndex) const;

   // diagnostics
   void dump(CWriteFormat& writeFormat,int depth = -1);

private:
   void clearState();
   void clearExtent();
   void clearPointExtent();
   double getSquareMajorOffset() const;

};

//_____________________________________________________________________________
class CSegmentList : public CTypedPtrListContainer<CSegment*> 
{
public:
   CSegmentList(bool isContainer=true) : CTypedPtrListContainer<CSegment*>(isContainer) {}
};

//_____________________________________________________________________________
class CSegmentArray : public CTypedPtrArray<CPtrArray,CSegment*> 
{
};

//_____________________________________________________________________________
class CSegmentQfe : public CTypedQfe<CSegment>
{
private:
   CSegmentCluster* m_cluster;

public:
   //CSegmentQfe(const CPoint2d& origin,CSegment* segment);
   CSegmentQfe(const CSegmentQfe& other);
   CSegmentQfe(const CSegmentQfe& other,const CTMatrix& matrix);
   CSegmentQfe(CSegment& segment);
   virtual ~CSegmentQfe();

   CSegment& getSegment() { return *getObject(); }
   const CSegment& getSegment() const { return *getObject(); }
   virtual CExtent getExtent() const { return getObject()->getExtent(); }

   CSegmentCluster* getCluster() { return m_cluster; }
   void setCluster(CSegmentCluster& cluster);
   void transform(const CTMatrix& matrix);

   static int sortByAscendingLength(const void* elem1,const void* elem2);
};

//_____________________________________________________________________________
class CSegmentQfeTree : public CDerivedQfeTree<CSegmentQfe>
{
public:
   CSegmentQfeTree();
};

//_____________________________________________________________________________
class CSegmentGraduatedQfeTree : public CDerivedGraduatedQfeTree<CSegmentQfe>
{
public:
   CSegmentGraduatedQfeTree(double granularity,double maxFeatureSize);
};

//_____________________________________________________________________________
class CSegmentQfeList : public CDerivedQfeList<CSegmentQfe>
{
public:
   CSegmentQfeList();
   CSegmentQfeList(const CSegmentQfeList& other,bool isContainer=true);
   CSegmentQfeList(const CSegmentQfeList& other,const CTMatrix& matrix);

   CPoint2d getCentroid() const;
   void transform(const CTMatrix& matrix);
   void sortByAscendingLength();
};

//_____________________________________________________________________________
class CGraphSegment
{
private:
   CSegment& m_segment;
   bool m_visited;
   bool m_evaluated;
   int m_headEndIndex;

public:
   CGraphSegment(CSegment& segment);

   CSegment& getSegment() { return m_segment; }

   bool getVisited() const { return m_visited; }
   void setVisited(bool flag) { m_visited = flag; }

   bool getEvaluated() const { return m_evaluated; }
   void setEvaluated(bool flag) { m_evaluated = flag; }

   int getHeadEndIndex() const { return m_headEndIndex; }
   void setHeadEndIndex(int index) { m_headEndIndex = index; }

   CPoint2d getHeadEndPoint() const { return m_segment.getEndPoint(    m_headEndIndex); }
   CPoint2d getTailEndPoint() const { return m_segment.getEndPoint(1 - m_headEndIndex); }
};

//_____________________________________________________________________________
class CGraphSegmentMap : public CTypedPtrMap<CMapPtrToPtr,CGraphSegment*,CGraphSegment*>
{
public:
   CGraphSegmentMap& operator=(const CGraphSegmentMap& other);
};

//_____________________________________________________________________________
class CGraphSegmentList : public CTypedPtrListContainer<CGraphSegment*>
{
public:
   CGraphSegmentList(bool isContainer = true) : CTypedPtrListContainer<CGraphSegment*>(isContainer) {}
   CPolygon* makePolygon(POSITION startPos,double& width,CGraphSegmentMap& segmentMap);
   CString getDiagram(POSITION pos=NULL);
};

//_____________________________________________________________________________
class CGraphSegmentQfe : public CTypedQfe<CGraphSegment>
{
private:
   int m_endIndex;
   CPoint2d m_origin;

public:
   CGraphSegmentQfe(CGraphSegment& graphSegment,int endIndex);

   // CQfe overrides
   virtual CPoint2d getOrigin() const { return m_origin; }
   virtual CExtent getExtent() const { return CExtent(m_origin,0.); }
   //virtual CString getInfoString() const;
};

//_____________________________________________________________________________
class CGraphSegmentQfeList : public CDerivedQfeList<CGraphSegmentQfe>
{
public:
   CString getDiagram(POSITION pos=NULL);
};

//_____________________________________________________________________________
class CGraphSegmentQfeTree : public CDerivedQfeTree<CGraphSegmentQfe>
{
public:
   CGraphSegmentQfeTree(bool isContainer=true) : CDerivedQfeTree<CGraphSegmentQfe>(isContainer){}

   bool containsDuplicateSegment(const CSegment& segment,double searchTolerance);
};

//_____________________________________________________________________________
class CSegmentCluster
{
private:
   static int m_nextClusterId;
   static int m_flashRecognitionMethodKeywordIndex;
   static int m_flashRecognitionTimeKeywordIndex;

   int m_clusterId;
   CSegmentQfeList m_list;
   bool m_isCleanAperture;
   DataStruct* m_apertureInsert;

public:
   CSegmentCluster();
   CSegmentCluster(const CSegmentCluster& other);
   CSegmentCluster(const CSegmentCluster& other,const CTMatrix& matrix);
   CSegmentCluster(CFlashRecognizer& flashRecognizer,DataStruct& apertureInsert,int layerIndex);
   ~CSegmentCluster();

   //CSegment& getSegment() { return m_segment; }
   int getId() const { return m_clusterId; }
   bool isCleanAperture() const { return m_isCleanAperture; }
   CSegmentQfeList& getList() { return m_list; }
   CExtent getExtent() const;
   CExtent getPointExtent() const;
   int getSegmentCount() const { return m_list.GetCount(); }
   bool merge(CSegmentQfe& segmentQfe);
   bool allSegmentsInsidePolygon(const CPolygon& polygon) const;
   bool getBoundaryPolygon(CCamCadDatabase& camCadDatabase,CPolygon& polygon,
      double searchTolerance,double& width,int maxIterationCount) const;
   DataStruct* makeApertureInsert(CCamCadDatabase& camCadDatabase,
      int layerIndex,int segregatedLayerIndex,CFlashRecognizer& flashRecognizer,
      bool simpleSearchFlag,CMessageFilter& messageFilter);
   DataStruct* getDefinedStandardApertureInsert(CCamCadDatabase& camCadDatabase,
      const CFlashRecognizerParameters& parameters,
      StandardApertureTypeTag apertureType,int layerIndex,const CPoint2d& origin,double angle,
      double dimension0=0.,double dimension1=0.,double dimension2=0.,
      double dimension3=0.,double dimension4=0.,double dimension5=0.);
   DataStruct* getComplexApertureInsert(CCamCadDatabase& camCadDatabase,
      const CFlashRecognizerParameters& parameters,
      int layerIndex,double width,double tolerance,const CPolygon& inputPolygon);
   DataStruct* getComplexApertureInsert(CCamCadDatabase& camCadDatabase,
      const CFlashRecognizerParameters& parameters,
      int layerIndex,double width,double tolerance,const CPoly& inputPoly);
   DataStruct* getDefinedRegionedComplexApertureInsert(CCamCadDatabase& camCadDatabase,
      CFlashRecognizer& flashRecognizer,
      int layerIndex,double width,double tolerance);
   DataStruct* getComplexClusterApertureInsert(CCamCadDatabase& camCadDatabase,
      const CFlashRecognizerParameters& parameters,
      int layerIndex,double width,double tolerance);
   DataStruct* getMergedClusterPolygonApertureInsert(CCamCadDatabase& camCadDatabase,
      const CFlashRecognizerParameters& parameters,
      int layerIndex,double width,double tolerance);
   void normalizeForSignature(CTMatrix& matrix);
   CString getSignature() const;
   bool normalizedVerticesEqual(const CSegmentCluster& other,double toleranceDistance) const;
   //void transform(const CTMatrix& matrix) { m_list.transform(matrix); }

   static int getFlashRecognitionMethodKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getFlashRecognitionTimeKeywordIndex(CCamCadDatabase& camCadDatabase);
   static void resetKeywordIndices();

   // diagnostics
   void dump(CWriteFormat& writeFormat,int depth = -1);

private:
   CPolygon* getBoundaryPolygon(CGraphSegmentQfeTree& tree,CGraphSegmentList& list,
      CGraphSegmentMap& segmentMap,
      CGraphSegment& segment,int endIndex,double searchTolerance,double& width,
      int& iterationCount,const int maxIterationCount) const;
};

//_____________________________________________________________________________
class CSegmentClusterMap
{
private:
   CTypedPtrMap<CMapPtrToPtr,CSegmentCluster*,CSegmentCluster*> m_map;

public:
   CSegmentClusterMap();
   ~CSegmentClusterMap();

   //bool lookup(CSegmentQfeList* qfeList);
   //bool setAt(CSegmentQfeList* qfeList);
   //bool deleteAt(CSegmentQfeList* qfeList);
   bool merge(CSegmentQfe& segmentQfe0,CSegmentQfe& segmentQfe1);
   bool add(CSegmentQfe& segmentQfe);
   void add(CSegmentCluster& segmentCluster);
   POSITION getStartPosition() { return m_map.GetStartPosition(); }
   void getNextAssoc(POSITION& pos,CSegmentCluster*& key) 
      { CSegmentCluster* value;  return m_map.GetNextAssoc(pos,key,value); }
   INT_PTR getCount() { return m_map.GetCount(); }
};

//_____________________________________________________________________________
class CClusterIndicator
{
private:
   //static CCamCadDatabase* m_camCadDatabase;
   //static BlockStruct* m_pcbBlock;
   //static LayerStruct* m_clusterDisplayLayer;
   //static LayerStruct* m_clusterIndicatorLayer;
   //static LayerStruct* m_clusterExtentLayer;

   CSegmentCluster& m_segmentCluster;
   CSegmentArray m_segmentArray0;
   CSegmentArray m_segmentArray1;
   CSegmentArray m_segmentArray2;
   CSegmentArray m_segmentArray3;
   CSegmentArray* m_segmentQuadrants[4];
   CPoint2d m_clusterOrigin;

public:
   CClusterIndicator(CSegmentCluster& segmentCluster);

   void addSegment(CSegment& segment);
   void addSegments();
   void instantiate(CCamCadDatabase& camCadDatabase,BlockStruct* pcbBlock,
      int segmentIdKeywordIndex,int polygonIdKeywordIndex,int clusterIdKeywordIndex,
      LayerStruct& clusterDisplayLayer,LayerStruct& clusterIndicatorLayer,LayerStruct& clusterExtentLayer);
   //void instantiate();
};

//_____________________________________________________________________________
class CFlashRecognizerParameters
{
private:
   CCamCadDatabase& m_camCadDatabase;

   double m_minimumFeatureSize;
   double m_maximumFeatureSize;
   double m_ignoreWidthLimit;
   double m_minimumWidthToSet;
   double m_minimumCornerRadius;
   double m_roundedCornerRadiusTolerance;
   double m_radialTolerance;
   double m_baseUnit;
   double m_apertureNormalizationAffinity;
   double m_clusterBoundaryAffinity;
   double m_clusterBoundaryFitnessFactor;
   double m_circumcircleTolerance;
   double m_rectangleCoordTolerance;
   int    m_minimumCircleVertexCount;
   int    m_maxBoundarySearchIterationCount;
   int    m_maxClusterSegments;

   bool   m_selectedFlag;
   bool   m_searchForBoundedClustersFlag;
   bool   m_mergeClustersIntoPolygonFlag;
   bool   m_mergeClustersIntoRegionFlag;
   bool   m_generateClusterDisplayFlag;
   bool   m_generateDiagnosticLayersFlag;
   bool   m_segregateClusterAperturesFlag;
   bool   m_convertComplexClusters;

public:
   CFlashRecognizerParameters(CCamCadDatabase& camCadDatabase);
   CFlashRecognizerParameters(const CFlashRecognizerParameters& other);
   CFlashRecognizerParameters& operator=(const CFlashRecognizerParameters& other);

   // accessors
   CCamCadDatabase& getCamCadDatabase() const { return m_camCadDatabase; }

   double getMinimumFeatureSize() const { return m_minimumFeatureSize; }
   void setMinimumFeatureSize(double featureSize) { m_minimumFeatureSize = featureSize; }

   double getMaximumFeatureSize() const { return m_maximumFeatureSize; }
   void setMaximumFeatureSize(double featureSize) { m_maximumFeatureSize = featureSize; }

   double getMinimumCornerRadius() const { return m_minimumCornerRadius; }
   void setMinimumCornerRadius(double radius) { m_minimumCornerRadius = radius; }

   double getRoundedCornerRadiusTolerance() const { return m_roundedCornerRadiusTolerance; }
   void setRoundedCornerRadiusTolerance(double radius) { m_roundedCornerRadiusTolerance = radius; }

   double getRadialTolerance() const { return m_radialTolerance; }
   void setRadialTolerance(double tolerance) { m_radialTolerance = tolerance; }

   double getIgnoreWidthLimit() const { return m_ignoreWidthLimit; }
   void setIgnoreWidthLimit(double width) { m_ignoreWidthLimit = width; }

   double getMinimumWidthToSet() const { return m_minimumWidthToSet; }
   void setMinimumWidthToSet(double width) { m_minimumWidthToSet = width; }

   double getBaseUnit() const { return m_baseUnit; }
   void setBaseUnit(double unit) { m_baseUnit = unit; }

   double getApertureNormalizationAffinity() const { return m_apertureNormalizationAffinity; }
   void setApertureNormalizationAffinity(double distance) { m_apertureNormalizationAffinity = distance; }

   double getClusterBoundaryAffinity() const { return m_clusterBoundaryAffinity; }
   void setClusterBoundaryAffinity(double tolerance) { m_clusterBoundaryAffinity = tolerance; }

   double getClusterBoundaryFitnessFactor() const { return m_clusterBoundaryFitnessFactor; }
   void setClusterBoundaryFitnessFactor(double fitnessFactor) { m_clusterBoundaryFitnessFactor= fitnessFactor; }

   double getCircumcircleTolerance() const { return m_circumcircleTolerance; }
   void setCircumcircleTolerance(double tolerance) { m_circumcircleTolerance = tolerance; }

   double getRectangleCoordTolerance() const { return m_rectangleCoordTolerance; }
   void setRectangleCoordTolerance(double tolerance) { m_rectangleCoordTolerance = tolerance; }

   int getMinimumCircleVertexCount() const { return m_minimumCircleVertexCount; }
   void setMinimumCircleVertexCount(int vertexCount) { m_minimumCircleVertexCount = vertexCount; }

   int getMaxBoundarySearchIterationCount() const { return m_maxBoundarySearchIterationCount; }
   void setMaxBoundarySearchIterationCount(int count) { m_maxBoundarySearchIterationCount = count; }

   int getMaxClusterSegments() const { return m_maxClusterSegments; }
   void setMaxClusterSegments(int maxClusterSegments) { m_maxClusterSegments = maxClusterSegments; }

   bool getSelectedFlag() const { return m_selectedFlag; }
   void setSelectedFlag(bool flag) { m_selectedFlag = flag; }

   bool getSearchForBoundedClusters() const { return m_searchForBoundedClustersFlag; }
   void setSearchForBoundedClusters(bool flag) { m_searchForBoundedClustersFlag = flag; }

   bool getMergeClustersIntoPolygon() const { return m_mergeClustersIntoPolygonFlag; }
   void setMergeClustersIntoPolygon(bool flag) { m_mergeClustersIntoPolygonFlag = flag;  }

   bool getMergeClustersIntoRegion() const { return m_mergeClustersIntoRegionFlag; }
   void setMergeClustersIntoRegion(bool flag) { m_mergeClustersIntoRegionFlag = flag; }

   bool getGenerateClusterDisplayFlag() const { return m_generateClusterDisplayFlag; }
   void setGenerateClusterDisplayFlag(bool flag) { m_generateClusterDisplayFlag = flag; }

   bool getGenerateDiagnosticLayersFlag() const { return m_generateDiagnosticLayersFlag; }
   void setGenerateDiagnosticLayersFlag(bool flag) { m_generateDiagnosticLayersFlag = flag; }

   bool getSegregateClusterAperturesFlag() const { return m_segregateClusterAperturesFlag; }
   void setSegregateClusterAperturesFlag(bool flag) { m_segregateClusterAperturesFlag = flag; }

   bool getConvertComplexClusters() const { return m_convertComplexClusters; }
   void setConvertComplexClusters(bool flag) { m_convertComplexClusters = flag; }

   void roundToBaseUnits(double& dimension) const;
   //bool isRecognizable(DataStruct& data);

   // operations
   void setToDefaults();
   void loadFromRegistry();
   void storeInRegistry();
};

//_____________________________________________________________________________
class CFlashRecognizer
{
private:
   CCamCadDatabase& m_camCadDatabase;
   int m_layerIndex;
   CFlashRecognizerParameters& m_parameters;
   BlockStruct* m_pcbBlock;
   //CTypedPtrArrayContainer<CPolygonQfeTree*> m_layerTrees;
   CSegmentQfeTree m_layerTree;
   CSegmentClusterMap m_clusterMap;
   CSegmentList m_segmentList;
   LayerStruct* m_complexLayer;
   CTypedPtrListContainer<CSegmentPolygon*> m_segmentPolygons;
   CTypedPtrListContainer<CSegmentPoly*> m_segmentPolys;
   CTypedMapStringToPtrContainer<LayerStruct*> m_diagnosticLayerMap;

public:
   CFlashRecognizer(CCamCadDatabase& camCadDatabase,CFlashRecognizerParameters& parameters,int layerIndex);

   CFlashRecognizerParameters& getParameters() { return m_parameters; }
   //CPolygonQfeTree& getLayerTree(int layerIndex);
   CCamCadDatabase& getCamCadDatabase() { return m_camCadDatabase; }
   CCamCadData& getCamCadData();
   LayerStruct& getDefinedComplexLayer();
   LayerStruct* getComplexLayer() { return m_complexLayer; }
   CString getUniqueLayerName(const CString& layerNamePrefix);
   bool scanData();
   bool scanLayer(bool processComplexLayerFlag);
   bool clusterSegments();
   void printTreeMetricsReport(CWriteFormat& writeFormat) { m_layerTree.printMetricsReport(writeFormat); }
   void generateClusterDisplay();
   void addDiagnosticAperture(CArray<int,int>& methodCountArray,DataStruct* apertureInsert);
   LayerStruct* convertClustersToApertures();
   void copyComplexApertureDataToComplexLayer(DataStruct& complexApertureInsert);
   CSegmentPolygon* addSegmentPolygon();
   CSegmentPoly* addSegmentPoly(CPoly& poly);
   //bool mergePolygonClusters(int layerIndex);
   LayerStruct* getDiagnosticLayer(const CString& baseLayerName);
};

//_____________________________________________________________________________
class CPerformanceReportEntry
{
private:
   DataStruct& m_aperture;
   FlashRecognitionMethodTag m_recognitionMethod;
   TimeIndex m_startTime;
   TimeIndex m_stopTime;

public:
   CPerformanceReportEntry(DataStruct& aperture,FlashRecognitionMethodTag recognitionMethod,TimeIndex startTime,TimeIndex stopTime);

   void writeReport(CWriteFormat& writeFormat,CCEtoODBDoc& camCadDoc);
   double getDurationInMilliseconds() const;
};

//_____________________________________________________________________________
class CPerformanceReporter
{
private:
   static CPerformanceReporter* m_reporter;

   CTypedPtrListContainer<CPerformanceReportEntry*> m_entries;

public:
   //CPerformanceReporter();

   void addEntry(DataStruct* aperture,FlashRecognitionMethodTag recognitionMethod,TimeIndex startTime,TimeIndex stopTime);
   void writeReport(CWriteFormat& writeFormat,CCEtoODBDoc& camCadDoc);
   void close();

   static CPerformanceReporter& getPerformanceReporter();


};

#endif
