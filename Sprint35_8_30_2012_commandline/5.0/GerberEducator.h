// $Header: /CAMCAD/5.0/GerberEducator.h 85    3/12/07 12:40p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#if ! defined (__GerberEducator_h__)
#define __GerberEducator_h__

#pragma once

#include "CcDoc.h"
#include "afxwin.h"
#include "Qfe.h"
#include "CamCadDatabase.h"
#include "TMState.h"
#include "RedBlackTree.h"
#include "afxcmn.h"
#include "PolarCoordinate.h"
#include "QfeTypes.h"

#define QEducationMethodShape      "Shape"
#define QEducationMethodRefDes     "RefDes"
#define QEducationMethodPartNumber "PartNumber"
#define QEducationMethodPinPattern "PinPattern"
#define QEducationMethodAutomatic  "Automatic"

#define QRefDesSuffixFormat "%05d"

#define QPreGerberEducator "pre Gerber Educator"
#define QGerberEducator    "Gerber Educator"

class CGerberEducator;
class CGerberEducatorBaseDialog;
class CGerberEducatorCentroid;
class CPinChain;
class CPinDataTreeList;
class CPinData;
class CGerberEducatorComponentGeometryDirectory;
class CGerberEducatorUi;

enum GerberEducationMethodTag
{
   gerberEducationMethodByShape,
   gerberEducationMethodByRefDes,
   gerberEducationMethodByPartNumber,
   gerberEducationMethodByPinPattern,
   gerberEducationMethodAutomatic,
   gerberEducationMethodUndefined
};

CString gerberEducationMethodTagToString(GerberEducationMethodTag tagValue);

enum GerberEducatorPackageIdentifierTag
{
   gerberEducatorPackageIdentifierShape,
   gerberEducatorPackageIdentifierPartNumber,
   gerberEducatorPackageIdentitfierUndefined
};

enum GerberEducatorDataSourceTag
{
   gerberEducatorDataSourceGerber,
   gerberEducatorDataSourceCentroid,
   gerberEducatorDataSourceDrill,
   gerberEducatorDataSourceUndefined,
   gerberEducatorDataSourceLowerBound = gerberEducatorDataSourceGerber,
   gerberEducatorDataSourceUpperBound = gerberEducatorDataSourceUndefined
};

GerberEducatorDataSourceTag stringToGerberEducatorDataSource(const CString& tagValue);
CString gerberEducatorDataSourceToString(GerberEducatorDataSourceTag tagValue);

enum GerberEducatorDataSourceTypeTag
{
   gerberEducatorDataSourceTypePadTop,
   gerberEducatorDataSourceTypePadBottom,
   gerberEducatorDataSourceTypeSilkscreenTop,
   gerberEducatorDataSourceTypeSilkscreenBottom,
   gerberEducatorDataSourceTypeTop,
   gerberEducatorDataSourceTypeBottom,
   gerberEducatorDataSourceTypeBoardOutline,
   gerberEducatorDataSourceTypeThru,
   gerberEducatorDataSourceTypeBlind,
   gerberEducatorDataSourceTypeBuried,
   gerberEducatorDataSourceTypeUndefined,
   gerberEducatorDataSourceTypeLowerBound = gerberEducatorDataSourceTypePadTop,
   gerberEducatorDataSourceTypeUpperBound = gerberEducatorDataSourceTypeUndefined
};

GerberEducatorDataSourceTypeTag stringToGerberEducatorDataSourceType(const CString& tagValue);
CString gerberEducatorDataSourceTypeToString(GerberEducatorDataSourceTypeTag tagValue);

//_____________________________________________________________________________
class CPinTreeNodeList : public CTypedRedBlackTreeNodeList<CPolarCoordinate,DataStruct>
{
};

//_____________________________________________________________________________
class CPinTreeByPolarCoordinate : public CTypedRedBlackTreeContainer<CPolarCoordinate,DataStruct>
{
private:
   double m_searchTolerance;

public:
   CPinTreeByPolarCoordinate(double searchTolerance);

   double getSearchTolerance() { return m_searchTolerance; }
   void setSearchTolerance(double searchTolerance) { m_searchTolerance = searchTolerance; }

   void test(CWriteFormat* writeFormat);
   bool findMeshSolution(CPinTreeByPolarCoordinate& subTree,double& deltaAngle);
};

//_____________________________________________________________________________
class CQfePhysicalPin : public CTypedQfe<int>
{
private:
   CString m_pinRef;
   bool m_topPadFlag;
   bool m_bottomPadFlag;
   bool m_viaFlag;

public:
   CQfePhysicalPin(const CPoint2d& origin,const CString& pinRef,bool topPadFlag,bool bottomPadFlag,bool viaFlag);
   ~CQfePhysicalPin();

public:
   //// CQfe overrides
   //virtual CPoint2d getOrigin() const { return m_origin; }
   //virtual CExtent getExtent() const;
   //virtual CString getInfoString() const;

   //// CObject2d overrides
   //virtual bool isExtentIntersecting(const CExtent& extent);
   //virtual bool isPointWithinDistance(const CPoint2d& point,double distance);
   //virtual bool isInViolation(CObject2d& otherObject);
   virtual int getObjectType() const { return 0; }

   // contents
   CString getPinRef() const { return m_pinRef; }
   bool isVisible(bool topFlag);
   bool isVia() const { return m_viaFlag; }
};

//_____________________________________________________________________________
class CQfePhysicalPinList : public CDerivedQfeList<CQfePhysicalPin>
{
};

//_____________________________________________________________________________
class CQfePhysicalPinTree : public CDerivedQfeTree<CQfePhysicalPin>
{
};

//_____________________________________________________________________________
#define PinsMatch   1
#define PinsClash   2

class CGerberEducatorPinClaim
{
private:
   CGerberEducatorCentroid* m_centroid;
   int m_componentCount;
   int m_pinCount;
   double m_distanceToCentroid;
   bool m_centroidOriginFlag;
   bool m_xAxisSymmetryFlag;
   bool m_yAxisSymmetryFlag;
   bool m_radialSymmetryFlag;

public:
   CGerberEducatorPinClaim(CGerberEducatorCentroid* m_centroid=NULL);
   CGerberEducatorPinClaim& operator=(const CGerberEducatorPinClaim& other);

   void setComponentCount(int count) { m_componentCount = count; }
   void setPinCount(int count) { m_pinCount = count; }
   void setDistanceToCentroid(double distance) { m_distanceToCentroid = distance; }
   void setCentroidOriginFlag() { m_centroidOriginFlag = true; }
   void setXAxisSymmetryFlag() { m_xAxisSymmetryFlag = true; }
   void setYAxisSymmetryFlag() { m_yAxisSymmetryFlag = true; }
   void setRadialSymmetryFlag() { m_radialSymmetryFlag = true; }

   bool getCentroidOriginFlag() const    { return m_centroidOriginFlag; }
   bool getRadialSymmetryFlag() const    { return m_radialSymmetryFlag; }
   bool getLateralSymmetryFlag() const   { return m_xAxisSymmetryFlag || m_yAxisSymmetryFlag; }
   bool getBilateralSymmetryFlag() const { return m_xAxisSymmetryFlag && m_yAxisSymmetryFlag; }
   double getDistanceToCentroid() const  { return m_distanceToCentroid; }
   int getPinCount() const               { return m_pinCount; }
   CGerberEducatorCentroid* getCentroid() const { return m_centroid; }

   int compare(const CGerberEducatorPinClaim& other) const;

public:
   void dump(CWriteFormat& writeFormat,CGerberEducator& gerberEducator);
};

//_____________________________________________________________________________
class CQfeApertureInsert : public CQfe
{
private:
   DataStruct* m_apertureInsert;
   //CPinTreeByPolarCoordinate* m_pinTree;
   CGerberEducatorPinClaim m_pinClaim;

public:
   CQfeApertureInsert(DataStruct* apertureInsert);
   ~CQfeApertureInsert();

public:
   // CQfe overrides
   virtual CPoint2d getOrigin() const { return m_apertureInsert->getInsert()->getOrigin2d(); }
   virtual CExtent getExtent() const;
   virtual CString getInfoString() const;

   // CObject2d overrides
   virtual bool isExtentIntersecting(const CExtent& extent);
   virtual bool isPointWithinDistance(const CPoint2d& point,double distance);
   virtual bool isInViolation(CObject2d& otherObject);
   virtual int getObjectType() const { return 0; }
   virtual bool isValid() const;

   // contents
   DataStruct* getAperture() { return m_apertureInsert;}
   CGerberEducatorPinClaim& getPinClaim() { return m_pinClaim; }
   void updatePinClaim(CGerberEducatorPinClaim& pinClaim);
   //CPinTreeByPolarCoordinate* getPinTree(CGerberEducator& gerberEducator,double searchTolerance);
};

//_____________________________________________________________________________
class CQfeApertureInsertTree : public CDerivedQfeTree<CQfeApertureInsert>
{
public:
   CQfeApertureInsertTree(bool isContainer = true) : CDerivedQfeTree<CQfeApertureInsert>(isContainer){}
};

//_____________________________________________________________________________
class CQfeApertureInsertList : public CDerivedQfeList<CQfeApertureInsert>
{
};

////_____________________________________________________________________________
//class CQfeApertureInsertList : public CTypedPtrList<CQfeList,CQfeApertureInsert*>
//{
//private:
//   bool m_isContainer;
//
//public:
//   CQfeApertureInsertList(bool isContainer=false,int nBlockSize=200);
//   ~CQfeApertureInsertList();
//   void empty();
//};
//
////_____________________________________________________________________________
//class CQfeApertureInsertTree : public CQfeExtentLimitedContainer
//{
//private:
//
//public:
//   CQfeApertureInsertTree(bool isContainer=true);
//   ~CQfeApertureInsertTree();
//
//public:
//   virtual int search(const CExtent& extent,CQfeApertureInsertList& foundList)
//      { return CQfeExtentLimitedContainer::search(extent,foundList); }
//
//   virtual CQfeApertureInsert* findFirst(const CExtent& extent)
//      { return (CQfeApertureInsert*)CQfeExtentLimitedContainer::findFirst(extent); }
//
//   virtual CQfeApertureInsert* findFirstViolation(const CExtent& extent,CQfe& qfe)
//      { return (CQfeApertureInsert*)CQfeExtentLimitedContainer::findFirstViolation(extent,qfe); }
//
//   virtual CQfeApertureInsert* findFirstEdgeToRectViolation(const CExtent& extent,int& entitiesChecked)
//      { return (CQfeApertureInsert*)CQfeExtentLimitedContainer::findFirstEdgeToRectViolation(extent,entitiesChecked); }
//
//   virtual CQfeApertureInsert* findFirstEdgeToPointViolation(const CExtent& extent,
//      const CPoint2d& point,double distance,int& entitiesChecked)
//      { return (CQfeApertureInsert*)CQfeExtentLimitedContainer::findFirstEdgeToPointViolation(extent,point,
//                                                        distance,entitiesChecked); }
//
//   //virtual int findAllViolations(const CExtent& extent,CQfe& qfe,CQfeList& foundList);
//
//   virtual void setAt(CQfeApertureInsert* qfe)
//      { CQfeExtentLimitedContainer::setAt(qfe); }
//};

//_____________________________________________________________________________
class CGerberEducatorRefDes
{
private:
   CString m_refDes;
   CString m_prefix;
   CString m_refDesSuffixFormat;
   int m_suffix;  // if -1, then suffix is empty

public:
   CGerberEducatorRefDes();
   CGerberEducatorRefDes(const CString& refDes);

   CString getRefDes() const { return m_refDes; }
   CString getPrefix() const { return m_prefix; }
   int getSuffix() const { return m_suffix; }
   void setRefDes(const CString& refDes);
   void setPrefix(const CString& prefix);
   void setSuffix(int suffix);
   void setSuffix(int suffix,const CString& refDesSuffixFormat);

private:
   void build();
};

//_____________________________________________________________________________
class CGerberEducatorRefDesSuffixMap
{
private:
   mutable CGerberEducatorRefDes m_refDes;
   mutable int m_maxSuffix;
   static WORD m_value;

   CMapPtrToWord m_suffixMap;

public:
   CGerberEducatorRefDesSuffixMap(const CString& refDesPrefix);

   void define(int refDesSuffix);
   CString defineCeiling(const CString& refDesSuffixFormat="%d");
   CString defineNew(int refDesSuffix,const CString& refDesSuffixFormat="%d");
   CString queryCeiling() const;
   CString queryNextUndefined(int refDesSuffix) const;
   CString queryPrevUndefined(int refDesSuffix) const;
   bool undefine(int refDesSuffix);
   bool isDefined(int refDesSuffix) const;

private:
   void calcMaxSuffix() const;
};

//_____________________________________________________________________________
class CGerberEducatorRefDesMap 
{
private:
   CTypedMapStringToPtrContainer<CGerberEducatorRefDesSuffixMap*> m_prefixMap;

public:
   CGerberEducatorRefDesMap();

   void define(const CString& refDes);
   CString defineCeiling(const CString& refDesPrefix,const CString& refDesSuffixFormat="%d");
   CString defineNew(const CString& refDes,const CString& refDesSuffixFormat="%d");
   CString queryCeiling(const CString& refDesPrefix) const;
   CString queryNextUndefined(const CString& refDes) const;
   CString queryPrevUndefined(const CString& refDes) const;
   bool undefine(const CString& refDes);
   bool isDefined(const CString& refDes) const;

private:
   CGerberEducatorRefDesSuffixMap& getDefinedSuffixMap(const CString& refDesPrefix);
   CGerberEducatorRefDesSuffixMap* getSuffixMap(const CString& refDesPrefix) const;
};

//_____________________________________________________________________________
class CGerberEducatorComponents
{
private:
   CTypedPtrMap<CMapStringToPtr,CString,DataStruct*> m_components;
   CGerberEducatorRefDesMap m_refDesMap;

public:
   CGerberEducatorComponents();

   bool Lookup(const CString& refDes,DataStruct*& component);
   void SetAt(const CString& refDes,DataStruct* component);
   bool RemoveKey(const CString& refDes);
   CGerberEducatorRefDesMap& getRefDesMap() { return m_refDesMap; }
};

////_____________________________________________________________________________
//class CGerberEducatorPin : public CQfeApertureInsert
//{
//private:
//   CGerberEducatorPinClaim m_pinClaim;
//
//public:
//   CGerberEducatorPin(DataStruct* m_apertureInsert);
//   ~CGerberEducatorPin();
//
//   CGerberEducatorPinClaim getPinClaim() { return m_pinClaim; }
//
//   void updatePinClaim(CGerberEducatorPinClaim& pinClaim);
//};
//
////_____________________________________________________________________________
//class CGerberEducatorPinList
//{
//private:
//   CTypedPtrListContainer<CGerberEducatorPin*> m_pinList;
//
//public:
//   CGerberEducatorPinList(){};
//   void add(CGerberEducatorPin* pin) { m_pinList.AddTail(pin); }
//   int getCount() { return m_pinList.GetCount(); }
//   POSITION getHeadPosition() { return m_pinList.GetHeadPosition(); }
//   CGerberEducatorPin* getNext(POSITION& pos) { return m_pinList.GetNext(pos); }
//};

//_____________________________________________________________________________
class CGerberEducatorCentroid
{
private:
   DataStruct& m_centroid;
   CTMatrix m_pcbToLocal;
   CTMatrix m_localToPcb;
   CQfeApertureInsertList m_pinList;

   static double m_searchTolerance;

public:
   CGerberEducatorCentroid(DataStruct& centroid,bool mirrorFlag);

   DataStruct& getCentroid() { return m_centroid; }
   CQfeApertureInsertList& getPinList() { return m_pinList; }
   CString getRefDes(CGerberEducator& gerberEducator);
   CString getShape(CGerberEducator& gerberEducator);
   CString getPartNumber(CGerberEducator& gerberEducator);

   CPoint2d transformPcbToLocal(const CPoint2d& point);
   CPoint2d transformLocalToPcb(const CPoint2d& point);
   void addPin(CQfeApertureInsert* pinApertureInsert);
   void evaluatePinClaims(CGerberEducator& gerberEducator,int componentCount);
   void updatePinMask(CByteArray& pinMask);

   CExtent getPinExtent(CPoint2d pinOrigin);
   static void setSearchTolerance(double searchTolerance) { m_searchTolerance = searchTolerance; }
};

//_____________________________________________________________________________
class CGerberEducatorCentroidList : public CTypedPtrListContainer<CGerberEducatorCentroid*>
{
};

//_____________________________________________________________________________
class CGerberEducatorCentroidListMap : public CTypedMapStringToPtrContainer<CGerberEducatorCentroidList*>
{
};

////_____________________________________________________________________________
//class CGerberEducatorCentroidListList
//{
//private:
//   CTypedPtrListContainer<CGerberEducatorCentroidList*> m_list;
//
//public:
//   CGerberEducatorCentroidListList(bool isContainer=true) :
//      m_list(isContainer){}
//
//	POSITION AddTail(CGerberEducatorCentroidList* centroidList)
//		{ return _CTypedPtrList<CPtrList,CGerberEducatorCentroidList*>::AddTail((void*)newElement); }
//
//};

//_____________________________________________________________________________
class CGerberEducatorCentroidListArray : public CTypedPtrArrayContainer<CGerberEducatorCentroidList*>
{
public:
   CGerberEducatorCentroidListArray(bool isContainer) : 
      CTypedPtrArrayContainer<CGerberEducatorCentroidList*>(10,isContainer) {}
};

////_____________________________________________________________________________
//class CGerberEducatorCentroidListListArray : public CTypedPtrArrayContainer<CGerberEducatorCentroidListList*>
//{
//};

//_____________________________________________________________________________
class CGerberEducatorCentroidListArrayArray : public CTypedPtrArrayContainer<CGerberEducatorCentroidListArray*>
{
};

//_____________________________________________________________________________
class CGerberEducatorComponentGeometryList
{
private:
   CTypedPtrListContainer<BlockStruct*> m_geometryList;
   CGerberEducatorComponentGeometryDirectory& m_parent;
   int m_pinCount;

public:
   CGerberEducatorComponentGeometryList(CGerberEducatorComponentGeometryDirectory& parent,int pinCount);

   CGerberEducator& getGerberEducator();

   void add(BlockStruct* geometry);
   POSITION getHeadPosition() const { return m_geometryList.GetHeadPosition(); }
   BlockStruct* getNext(POSITION& pos) const { return m_geometryList.GetNext(pos); }

   BlockStruct* findMatchingComponentGeometry(CPinDataTreeList& pinList,DataStruct* centroidData,
      CBasesVector& insertBasesVector,bool placedTopFlag,bool forceThFlag);
};

//_____________________________________________________________________________
class CGerberEducatorComponentGeometryDirectory
{
private:
   CTypedMapIntToPtrContainer<CGerberEducatorComponentGeometryList*> m_geometryListMap;
   CGerberEducator& m_gerberEducator;

public:
   CGerberEducatorComponentGeometryDirectory(CGerberEducator& gerberEducator);

   CGerberEducator& getGerberEducator() { return m_gerberEducator; }

   void add(BlockStruct* geometry);
   BlockStruct* findMatchingComponentGeometry(CPinDataTreeList& pinList,DataStruct* centroidData,
      CBasesVector& insertBasesVector,bool placedTopFlag,bool forceThFlag);

private:
   CGerberEducatorComponentGeometryList& getGeometryList(int pinCount);
};

//_____________________________________________________________________________
enum EducatorFunctionalityTag
{
   educatorFunctionalityBasic,
   educatorFunctionalityLegacy,
   educatorFunctionalityCreateGeometry,
   educatorFunctionalityUndefined
};

//_____________________________________________________________________________
class CGerberEducatorProgressSummary
{
private:
   CGerberEducator& m_gerberEducator;

   int m_topMatchedPadCount;
   int m_bottomMatchedPadCount;
   int m_topUnmatchedPadCount;
   int m_bottomUnmatchedPadCount;

   int m_topMatchedCentroidCount;
   int m_bottomMatchedCentroidCount;
   int m_topUnmatchedCentroidCount;
   int m_bottomUnmatchedCentroidCount;

   int m_topComponentCount;
   int m_bottomComponentCount;

   CMapStringToInt m_componentGeometryCountMap;

public:
   CGerberEducatorProgressSummary(CGerberEducator& gerberEducator);

public:
   void initialize();
   void incrementPadCount(bool topFlag,bool matchedFlag);
   void incrementCentroidCount(bool topFlag,bool matchedFlag);

   void addComponent(DataStruct& component);
   void removeComponent(DataStruct& component);

   void matchPad(bool topFlag);
   void unmatchPad(bool topFlag);

   void matchCentroid(bool topFlag);
   void unmatchCentroid(bool topFlag);

   int getPadCount(bool topFlag,bool matchedFlag) const;
   int getCentroidCount(bool topFlag,bool matchedFlag) const;
   int getComponentCount(bool topFlag) const;
   int getComponentGeometryCount() const;

};

//_____________________________________________________________________________
class CGerberEducatorBase
{
protected:
   CGerberEducatorBaseDialog* m_dialog;
   CGerberEducatorUi* m_gerberEducatorUi;

public:
   CGerberEducatorBase();
   virtual ~CGerberEducatorBase();

   virtual EducatorFunctionalityTag getEducatorFunctionality() = 0;
   virtual CGerberEducatorBaseDialog& getDialog() = 0;

   virtual CGerberEducatorUi* getGerberEducatorUi();
   virtual void setGerberEducatorUi(CGerberEducatorUi* gerberEducatorUi);
};

//_____________________________________________________________________________
class CGerberEducator : public CGerberEducatorBase
{
protected:
   CCEtoODBDoc& m_camCadDoc;
   //EducatorFunctionalityTag m_educatorFunctionality;
   CCamCadDatabase m_camCadDatabase;
   FileStruct* m_pcbFile;
   CDataList m_redrawList;
   CDataList m_eraseList;

   //int m_topPadInputLayerIndex;
   //int m_bottomPadInputLayerIndex;
   //int m_topSilkScreenInputLayerIndex;
   //int m_bottomSilkScreenInputLayerIndex;
   //int m_topCentroidInputLayerIndex;
   //int m_bottomCentroidInputLayerIndex;

   CLayerFilter m_topPadLayers;
   CLayerFilter m_bottomPadLayers;
   CLayerFilter m_topSilkScreenLayers;
   CLayerFilter m_bottomSilkScreenLayers;
   CLayerFilter m_topCentroidLayers;
   CLayerFilter m_bottomCentroidLayers;

   bool m_initialized;

   CQfePhysicalPinTree m_physicalPinTree;
   CQfeApertureInsertTree m_apertureInsertTree;
   CDataStructQfeTree m_topCentroidTree;
   CDataStructQfeTree m_bottomCentroidTree;
   CDataList m_centroidList;
   CTypedPtrMap<CMapStringToPtr,CString,DataStruct*> m_centroidMap;
   //CTypedPtrMap<CMapStringToPtr,CString,DataStruct*> m_components;
   CGerberEducatorComponents m_components;
   CGerberEducatorComponentGeometryDirectory m_componentGeometryDirectory;
   CGerberEducatorProgressSummary m_gerberEducatorProgressSummary;

   int m_topUnmatchedLayerIndex;
   int m_bottomUnmatchedLayerIndex;
   int m_topMatchedLayerIndex;
   int m_bottomMatchedLayerIndex;

   int m_refDesKeywordIndex;
   int m_shapeKeywordIndex;
   int m_machineFeederKeywordIndex;
   int m_machineKeywordIndex;
   int m_feederKeywordIndex;
   int m_partNumberKeywordIndex;
   int m_educationMethodKeywordIndex;

   static double m_searchTolerance;
   static double m_padShapeTolerance;
   static double m_pinPitchTolerance;
   static double m_coordinateTolerance;
   static CInsertTypeMask m_pinInsertTypeMask;

public:
   CGerberEducator(CCEtoODBDoc& camCadDoc);
   virtual ~CGerberEducator();

   CCEtoODBDoc& getCamCadDoc() { return m_camCadDoc; }
   CCamCadDatabase& getCamCadDatabase() { return m_camCadDatabase; }
   CCamCadData& getCamCadData() { return m_camCadDoc.getCamCadData(); }
   CGerberEducatorRefDesMap& getRefDesMap() { return m_components.getRefDesMap(); }
   CGerberEducatorProgressSummary& getGerberEducatorProgressSummary() { return m_gerberEducatorProgressSummary; }

   void initialize();
   bool isInitialized() const;
   void deleteEducatorData(bool deleteLayersFlag=false);
   void showDialog();
   void update();
   void saveNamedView();
   void defineLayerDataSourceAttribute(LayerStruct& layer);

   int getTopUnmatchedLayerIndex()      { return m_topUnmatchedLayerIndex; }
   int getBottomUnmatchedLayerIndex()   { return m_bottomUnmatchedLayerIndex; }
   int getTopMatchedLayerIndex()        { return m_topMatchedLayerIndex; }
   int getBottomMatchedLayerIndex()     { return m_bottomMatchedLayerIndex; }
   int getRefDesKeywordIndex()          { return m_refDesKeywordIndex; }
   int getShapeKeywordIndex()           { return m_shapeKeywordIndex; }
   int getMachineFeederKeywordIndex()   { return m_machineFeederKeywordIndex; }
   int getMachineKeywordIndex()         { return m_machineKeywordIndex; }
   int getFeederKeywordIndex()          { return m_feederKeywordIndex; }
   int getPartNumberKeywordIndex()      { return m_partNumberKeywordIndex; }
   int getEducationMethodKeywordIndex() { return m_educationMethodKeywordIndex; }

   DataStruct* getComponent(const CString& refDes);
   CDataStructQfeTree& getCentroidTree(bool topFlag) { return (topFlag ? m_topCentroidTree : m_bottomCentroidTree); }

   bool isOnUnmatchedLayer(int layerIndex) 
   { 
      return (layerIndex == getTopUnmatchedLayerIndex()    ||
              layerIndex == getBottomUnmatchedLayerIndex()    ); 
   }

   bool isOnTopMatchTypeLayer(int layerIndex) 
   { 
      return (layerIndex == getTopMatchedLayerIndex()    ||
              layerIndex == getTopUnmatchedLayerIndex()    ); 
   }

   int getLinkedLayerIndex(int layerIndex);
   int getOppositeLayerIndex(int layerIndex);
   int getUnmatchedLayerIndexForPadLayer(int padLayerIndex,bool componentPlacedTopFlag);
   int getPadLayerIndexForUnmatchedLayer(int unmatchedLayerIndex,bool componentPlacedTopFlag);

   CPinTreeByPolarCoordinate* calcPinTree(CQfeApertureInsert& pin1Aperture,double searchTolerance);

   void createComponentGeometry(BlockStruct& constructedComponentGeometry,
      CBasesVector& componentBasesVector,CPinDataTreeList& pinList,
      DataStruct* centroidData,bool topFlag,bool forceThFlag);

   void erase(DataStruct& data);

   void addComponentByCentroid(DataStruct& centroid,CPinDataTreeList& pinList,bool forceThFlag,GerberEducationMethodTag educationMethod);
   DataStruct* addComponent(
      const CString& refDes,const CString& shape,const CString& partNumber,double fhs,
      const bool topFlag,bool forceThFlag,
      CPinDataTreeList& pinList,DataStruct* centroidData,GerberEducationMethodTag educationMethod);
   void addComponentAtCentroid(const CString& refDes,const CString& shape,
      const CString& partNumber,bool topFlag,bool forceThFlag,double fhs,double rotationRadians,bool preferredCcwOrthoRotationFlag,
      CPinDataTreeList& pinList);

   void addSinglePinComponents(
      CString refDes,CString shape,CString partNumber,double fhs,
      BlockStruct& componentGeometry,bool matchCentroidFlag,double preferredRotation,bool preferredCcwOrthoRotationFlag,
      bool orthogonalFlag,bool forceThFlag,GerberEducationMethodTag educationMethod);
   void addComponents(
      CString refDes,CString shape,CString partNumber,double fhs,
      DataStruct& component,bool matchCentroidFlag,bool orthogonalFlag,bool forceThFlag,GerberEducationMethodTag educationMethod);
   void addComponentsByCentroid(DataStruct& centroid,CDataList& selectedCentroidList,bool forceThFlag,GerberEducationMethodTag educationMethod);
   void deleteComponent(DataStruct* component,POSITION componentPos);
   void deleteComponents(bool byRefDes,bool byShape,bool byPartNumber,bool byGeometry);

	//int setIpcStandardOrientation(BlockStruct *block);   
	//InsertStruct *getFirstPinInsert(BlockStruct *block);  // Support for setIpcStandardOrientation()
	
	void evaluatePinClaims(CGerberEducatorCentroidList& centroidList,GerberEducatorPackageIdentifierTag packageIdentifierTag);
   void addAutomatically(CGerberEducatorCentroidList& centroidList);
   void addAutomatically(GerberEducatorPackageIdentifierTag packageIdentifier);
   //BlockStruct* findMatchingComponentGeometry(BlockStruct& geometryTemplate,const CString& geometryBaseName);
   BlockStruct* getPadStackGeometry(CPinData& pinData,bool topFlag,bool forceThFlag,double& rotationRadians,double& fhs);
   void addByShape(DataStruct& centroid,GerberEducationMethodTag=gerberEducationMethodByShape);
   void addByPartNumber(DataStruct& centroid);
   void addByPinPattern(const CString& refDes,const CString& shape,const CString& partNumber,
      bool topFlag,bool forceThFlag,double fhs,double rotationRadians,bool preferredCcwOrthoRotationFlag,CPinDataTreeList& pinList,
      DataStruct* centroidData=NULL,bool othogonalFlag=false);
   CString getRefDes(DataStruct& centroid);
   CString getShape(DataStruct& centroid);
   CString getMachineFeeder(DataStruct& centroid);
   CString getMachine(DataStruct& centroid);
   CString getFeeder(DataStruct& centroid);
   CString getPartNumber(DataStruct& centroid);
   CString getPackageIdentifier(DataStruct& data,GerberEducatorPackageIdentifierTag packageIdentifierTag);
   CString getEducationMethod(DataStruct& centroid);
   bool onTopLayer(DataStruct& centroid);
   DataStruct* getSingleComponentCentroid(BlockStruct& componentGeometry);
   bool findGeometryPattern(CPinDataTreeList& pinList,BlockStruct& componentGeometry,
      DataStruct** centroidData,CTMatrix& componentGeometryToPcbMatrix,bool topFlag);
   int augmentPins(CPinDataTreeList& selectedPins);

   bool aperturesEqual(CCamCadDatabase& camCadDatabase,DataStruct* aperture0,DataStruct* aperture1,double deltaAngle=0.);
   bool padShapesEqual(BlockStruct* padGeometry0,BlockStruct* padGeometry1);
   bool padStackPadsEqual(BlockStruct& padStack0,BlockStruct& padStack1);
   bool padStacksEqual(BlockStruct& padStack0,BlockStruct& padStack1,double deltaAngleRadians);
   DataStruct* getPadstackSurfacePad(DataStruct& pin,bool topFlag);
   void getPadstackSurfacePads(DataStruct& pin,DataStruct*& topPad,DataStruct*& bottomPad);

   void addToRedrawList(DataStruct* data);
   void emptyRedrawList();
   void drawRedrawList(bool empty=true);

   void addToEraseList(DataStruct* data);
   void emptyEraseList();
   void drawEraseList(DataStruct* data=NULL,bool empty=true);

private:
   void addCompPins(DataStruct& component);
   void setTopMatchedLayerIndex(int layerIndex)      { m_topMatchedLayerIndex = layerIndex; }
   void setBottomMatchedLayerIndex(int layerIndex)   { m_bottomMatchedLayerIndex = layerIndex; }
   void setTopUnmatchedLayerIndex(int layerIndex)    { m_topUnmatchedLayerIndex = layerIndex; }
   void setBottomUnmatchedLayerIndex(int layerIndex) { m_bottomUnmatchedLayerIndex = layerIndex; }


public:
   static double getSearchTolerance()   { return m_searchTolerance; }
   static double getPadShapeTolerance() { return m_padShapeTolerance; }
   static double getPinPitchTolerance() { return m_pinPitchTolerance; }
   static double getCoordinateTolerance() { return m_coordinateTolerance; }
   static const CInsertTypeMask& getPinInsertTypeMask() { return m_pinInsertTypeMask; }

public:
   virtual EducatorFunctionalityTag getEducatorFunctionality();
   virtual CGerberEducatorBaseDialog& getDialog();
   virtual bool getEnableCentroidEditingFlag();
   virtual CString getToolName();
   virtual CString getEducationMethodAttributeName();
   virtual CString getEducationSurfaceAttributeName();
   virtual LayerStruct* getLayer(CamCadLayerTag layerTag);
   virtual bool isTopGerberLayer(LayerStruct& layer);
   virtual bool isBottomGerberLayer(LayerStruct& layer);
   virtual bool isTopSilkScreenLayer(LayerStruct& layer);
   virtual bool isBottomSilkScreenLayer(LayerStruct& layer);
   virtual bool isTopCentroidLayer(LayerStruct& layer);
   virtual bool isBottomCentroidLayer(LayerStruct& layer);
   virtual bool scanForGerberEducatorInputLayers(bool displayMessagesFlag=true);
};

//_____________________________________________________________________________
class CGerberCentroidMergeTool : public CGerberEducator
{
public:
   CGerberCentroidMergeTool(CCEtoODBDoc& camCadDoc);
   virtual ~CGerberCentroidMergeTool();

public:
   virtual EducatorFunctionalityTag getEducatorFunctionality();
   virtual bool getEnableCentroidEditingFlag();
   virtual CString getToolName();
   virtual CString getEducationMethodAttributeName();
   virtual CString getEducationSurfaceAttributeName();
   virtual LayerStruct* getLayer(CamCadLayerTag layerTag);
};

//_____________________________________________________________________________
class CGerberEducatorGeometryCreator : public CGerberEducator
{
public:
   CGerberEducatorGeometryCreator(CCEtoODBDoc& camCadDoc);
   CGerberEducatorGeometryCreator(CGerberEducatorUi& gerberEducatorUi);
   virtual ~CGerberEducatorGeometryCreator();

public:
   virtual EducatorFunctionalityTag getEducatorFunctionality();
   virtual CGerberEducatorBaseDialog& getDialog();
   virtual bool getEnableCentroidEditingFlag();
   virtual CString getToolName();
   virtual bool isTopGerberLayer(LayerStruct& layer);
   virtual bool isBottomGerberLayer(LayerStruct& layer);
   virtual bool isTopSilkScreenLayer(LayerStruct& layer);
   virtual bool isBottomSilkScreenLayer(LayerStruct& layer);
   virtual bool isTopCentroidLayer(LayerStruct& layer);
   virtual bool isBottomCentroidLayer(LayerStruct& layer);

private:
   void getDataSourceAndDataType(LayerStruct& layer,CString& dataSource,CString& dataType);
};

#endif
