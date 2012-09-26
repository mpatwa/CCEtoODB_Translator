// $Header: /CAMCAD/4.5/PcbComponentPinAnalyzer.h 20    12/08/05 12:52a Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2005. All Rights Reserved.
*/

#if ! defined (__PcbComponentPinAnalyzer_h__)
#define __PcbComponentPinAnalyzer_h__

#pragma once

#include "CamCadDatabase.h"
#include "PinChain.h"
#include "PolarCoordinate.h"

enum BookReadingOrderTag
{
   bookReadingTopBottomLeftRight,
   bookReadingTopBottomRightLeft,
   bookReadingBottomTopLeftRight,
   bookReadingBottomTopRightLeft,
   bookReadingLeftRightTopBottom,
   bookReadingLeftRightBottomTop,
   bookReadingRightLeftTopBottom,
   bookReadingRightLeftBottomTop,
   bookReadingUndefined
};

CString bookReadingOrderTagToString(BookReadingOrderTag bookReadingOrderTag);
BookReadingOrderTag stringToBookReadingOrderTag(CString bookReadingOrderString);

enum PinLabelingMethodTag
{
   pinLabelingMethodAlpha,
   pinLabelingMethodNumeric,
   pinLabelingMethodAlphaNumeric,
   pinLabelingMethodNumericAlpha,
   pinLabelingMethodUndefined
};

CString pinLabelingMethodTagToString(PinLabelingMethodTag pinNumberingTag);
PinLabelingMethodTag stringToPinLabelingMethodTag(CString pinNumberingString);

enum PinOrderingMethodTag
{
   pinOrderingMethodUndefined  = -1,
   pinOrderingMethodManual = 0,
   pinOrderingMethodClockwise = 1,
   pinOrderingMethodCounterClockwise = 2,
   pinOrderingMethodReadingOrder = 3,
};

CString pinOrderingMethodTagToString(PinOrderingMethodTag tagValue);
PinOrderingMethodTag stringToPinOrderingMethodTag(CString tagValue);
PinOrderingMethodTag intToPinOrderMethodTag(int tagValue); 

enum IPCStatusTag
{
	IPCStatusUndefined,
	IPCStatusNonStandardPart,
	IPCStatusNonStandardIPC,
	IPCStatusStandardIPC,
};

CString ipcStatusTagToString(IPCStatusTag ipcStatusTag);
IPCStatusTag stringToIpcStatusTag(CString ipcStatusString);

enum PcbComponentPackageTypeTag
{
   pcbComponentPackageTypeBga,
   pcbComponentPackageTypeChip,
   pcbComponentPackageTypeConnector,//
   pcbComponentPackageTypeCrystal,
   pcbComponentPackageTypeDip,//
   pcbComponentPackageTypeHeader,//
   pcbComponentPackageTypeJumper,//
   pcbComponentPackageTypeMisc,
   pcbComponentPackageTypeNoPins,
   pcbComponentPackageTypePga,//
   pcbComponentPackageTypePlcc,
   pcbComponentPackageTypeQfp,
   pcbComponentPackageTypeSoic,
   pcbComponentPackageTypeSot23,
   pcbComponentPackageTypeTestPin,//
   pcbComponentPackageTypeTestPoint,
   pcbComponentPackageTypeTsop,
   pcbComponentPackageTypeUndefined
};

enum MajorOrdinateTag
{
   majorOrdinateX,
   majorOrdinateY
};

enum SequenceOrderTag
{
   sequenceOrderAscending,
   sequenceOrderDescending
};

enum LabelPrefixTag
{
   labelPrefixAlpha,
   labelPrefixNumeric
};

//-----------------------------------------------------------------------------
// CPolarCoorinatePin
//-----------------------------------------------------------------------------
class CPolarCoorinatePin : public CPolarCoordinate
{
public:
	CPolarCoorinatePin(DataStruct& data, const CPoint2d& rectangularCoordinate )
		: CPolarCoordinate(rectangularCoordinate)
		, m_pinData(data) {};
	~CPolarCoorinatePin() {};

private:
	DataStruct& m_pinData;

public:
	DataStruct& getPinData()			{ return m_pinData; }
};


//-----------------------------------------------------------------------------
// CPolarCoordinatePinArray
//-----------------------------------------------------------------------------
class CPolarCoordinatePinArray
{
public:
	CPolarCoordinatePinArray(CCamCadDatabase& camCadDatabase, CDataList& dataList);
	~CPolarCoordinatePinArray();

private:
   CCamCadDatabase& m_camCadDatabase;
	CDataList& m_dataList;
	CTypedPtrArrayContainer<CPolarCoorinatePin*> m_pinsArray;
	double m_tolerance;

	void populatePolarCoordinateArray();

public:
	void analysizePinNumbering(DataStruct& pinOneData, bool clockwise);
};

//_____________________________________________________________________________
class COrderedDoubleArray
{
private:
   CArray<double> m_array;
   double m_tolerance;
   int m_growBy;

   mutable bool m_staleFlag;
   mutable double m_minInterval;
   mutable double m_maxInterval;

public:
   COrderedDoubleArray(double tolerance,int growBy = 1);
   void empty();

   int add(double value);
   double getAt(int index) const;
   int getSize() const;
   int findIndex(double value);

   double getMinInterval() const;
   double getMaxInterval() const;
   double getInterval() const;
   bool hasRegularInterval() const;
   double getRegularInterval(int offset) const;

private:
   void analyze() const;
};

//-----------------------------------------------------------------------------
// CPcbComponentPinAnalyzer
//-----------------------------------------------------------------------------
class CPcbComponentPinAnalyzer
{
protected:
   CCamCadDatabase& m_camCadDatabase;
   CDataList& m_dataList;

   double m_tolerance; // must be declared before other members that reference it during 
                       // initialization in constructors

   BlockStruct* m_componentGeometry;
   DataStruct* m_componentCentroid;
   CTypedPtrArrayContainer<DataStruct*> m_pinsBucket;
   CPinDataTreeList m_pinDataTreeList;

   COrderedDoubleArray m_xArray;
   COrderedDoubleArray m_yArray;
   CStringArray m_alphaLabelsArray;
   CStringArray m_numericLabelsArray;
   CStringArray m_pinLabelsArray;

   int m_pinCount;
   bool m_footprintAnalyzedFlag;
   PcbComponentPackageTypeTag m_pcbComponentPackageType;
   CString m_geometryDescriptor;
   BookReadingOrderTag m_bookReadingOrder;
   PinLabelingMethodTag m_pinNumberingMethod;
   PinOrderingMethodTag m_pinOrderingMethod;
   IPCStatusTag m_ipcStatus;
   CString m_alphaSkipList;

   int m_minSize;
   int m_maxSize;
   double m_pinPitch;
   double m_pinSpan;
   CDataList m_mainPins;
   CDataList m_thPins;
   CDataList m_smdPins;
   CDataList m_extraPins;

public:
   CPcbComponentPinAnalyzer(CCamCadDatabase& camCadDatabase, CDataList& dataList);
   CPcbComponentPinAnalyzer(CCamCadDatabase& camCadDatabase, BlockStruct& componentGeometry);
   ~CPcbComponentPinAnalyzer();

   void initializeMainPins();
   void initializeAnalysisData();
   bool isThruHolePin(DataStruct& pin);
   bool isThruHoleComponent();

   DataStruct* analysizePinNumbering(BookReadingOrderTag readingOrderTag, PinLabelingMethodTag pinNumberingTag,const CString& skipAlphas);
   CString getGeometryDescriptor();
   PcbComponentPackageTypeTag getPcbComponentPackageType();
   bool updateGeometry(CBasesVector& componentBasesVector,CTMatrix& oldToNewGeometryMatrix);

   BookReadingOrderTag getBookReadingOrder() const { return m_bookReadingOrder; }
   PinLabelingMethodTag getPinNumberingMethod() const { return m_pinNumberingMethod; }
   PinOrderingMethodTag getPinOrderingMethod() const { return m_pinOrderingMethod; }
   IPCStatusTag getIpcStatus() const { return m_ipcStatus; }
   CString getAlphaSkipList() const { return m_alphaSkipList; }
   BlockStruct* getComponentGeometry() { return m_componentGeometry; }

protected:
   void populateXYArrays();
   void populatePinsBucket();
   void populateAlphaAndNumbericLabels(const CString& skipAlphas);

   void populateAlphaPinNumbering(BookReadingOrderTag readingOrder);
   void populateNumericPinNumbering(BookReadingOrderTag readingOrder);
   void populateAlphaOrNumericPinNumbering(MajorOrdinateTag majorOrdinate,SequenceOrderTag majorSequenceOrder,SequenceOrderTag minorSequenceOrder,LabelPrefixTag labelPrefix);
   void populateAlphaAndNumericPinNumbering(MajorOrdinateTag majorOrdinate,SequenceOrderTag majorSequenceOrder,SequenceOrderTag minorSequenceOrder,LabelPrefixTag labelPrefix);
   void populateAlphaNumericPinNumbering(BookReadingOrderTag readingOrder);
   void populateNumericAlphaPinNumbering(BookReadingOrderTag readingOrder);

   int getXArrayIndex(double xCoordinate);
   int getYArrayIndex(double yCoordinate);
   int getPinsBucketIndex(int xIndex,int yIndex);
   int getPinsBucketIndex(CPoint2d origin);
   DataStruct* getPinAtPinBucket(int xIndex,int yIndex);
   bool getIndexPin(int& xIndex,int& yIndex);
   bool getIndexPinAndRotation(DataStruct*& indexPin,int& rotationDegrees);

   void analyzeFootprint();
   void setInsertType(CDataList* pinList,InsertTypeTag insertType);
   void makeExtraPinsFiducials();
   DataStruct* setPinNumberingToDefault();
   int getIpcRotation(DataStruct* pin1);
};

#endif
