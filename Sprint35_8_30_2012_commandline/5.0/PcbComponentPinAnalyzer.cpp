// $Header: /CAMCAD/5.0/PcbComponentPinAnalyzer.cpp 31    3/12/07 12:43p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2005. All Rights Reserved.
*/

#include "StdAfx.h"
#include "PcbComponentPinAnalyzer.h"

//_____________________________________________________________________________
CString bookReadingOrderTagToString(BookReadingOrderTag bookReadingOrderTag)
{
   CString retval;

   switch (bookReadingOrderTag)
   {
   case bookReadingTopBottomLeftRight:    retval = "Top Bottom Left Right";      break;
   case bookReadingTopBottomRightLeft:    retval = "Top Bottom Right Left";      break;
   case bookReadingBottomTopLeftRight:    retval = "Bottom Top Left Right";      break;
   case bookReadingBottomTopRightLeft:    retval = "Bottom Top Right Left";      break;
   case bookReadingLeftRightTopBottom:    retval = "Left Right Top Bottom";      break;
   case bookReadingLeftRightBottomTop:    retval = "Left Right Bottom Top";      break;
   case bookReadingRightLeftTopBottom:    retval = "Right Left Top Bottom";      break;
   case bookReadingRightLeftBottomTop:    retval = "Right Left Bottom Top";      break;
   default:                               retval = "Undefined";                  break;
   }

   return retval;
}
                                      
BookReadingOrderTag stringToBookReadingOrderTag(CString bookReadingOrderString)
{
   BookReadingOrderTag retval = bookReadingUndefined;

   if (bookReadingOrderTagToString(bookReadingTopBottomLeftRight).CompareNoCase(bookReadingOrderString) == 0)
   {
      retval = bookReadingTopBottomLeftRight;
   }
   else if (bookReadingOrderTagToString(bookReadingTopBottomRightLeft).CompareNoCase(bookReadingOrderString) == 0)
   {
      retval = bookReadingTopBottomRightLeft;
   }
   else if (bookReadingOrderTagToString(bookReadingBottomTopLeftRight).CompareNoCase(bookReadingOrderString) == 0)
   {
      retval = bookReadingBottomTopLeftRight;
   }
   else if (bookReadingOrderTagToString(bookReadingBottomTopRightLeft).CompareNoCase(bookReadingOrderString) == 0)
   {
      retval = bookReadingBottomTopRightLeft;
   }
   else if (bookReadingOrderTagToString(bookReadingLeftRightTopBottom).CompareNoCase(bookReadingOrderString) == 0)
   {
      retval = bookReadingLeftRightTopBottom;
   }
   else if (bookReadingOrderTagToString(bookReadingLeftRightBottomTop).CompareNoCase(bookReadingOrderString) == 0)
   {
      retval = bookReadingLeftRightBottomTop;
   }
   else if (bookReadingOrderTagToString(bookReadingRightLeftTopBottom).CompareNoCase(bookReadingOrderString) == 0)
   {
      retval = bookReadingRightLeftTopBottom;
   }
   else if (bookReadingOrderTagToString(bookReadingRightLeftBottomTop).CompareNoCase(bookReadingOrderString) == 0)
   {
      retval = bookReadingRightLeftBottomTop;
   }

   return retval;
}

//_____________________________________________________________________________
CString pinLabelingMethodTagToString(PinLabelingMethodTag pinNumberingTag)
{
   CString retval;

   switch (pinNumberingTag)
   {
   case pinLabelingMethodAlpha:             retval = "Alpha";             break;
   case pinLabelingMethodNumeric:           retval = "Numeric";           break;
   case pinLabelingMethodAlphaNumeric:      retval = "Alpha Numeric";     break;
   case pinLabelingMethodNumericAlpha:      retval = "Numeric Alpha";     break;
   default:                                 retval = "Undefined";         break;
   }

   return retval;
}

PinLabelingMethodTag stringToPinLabelingMethodTag(CString pinNumberingString)
{
   PinLabelingMethodTag retval = pinLabelingMethodUndefined;

   if (pinLabelingMethodTagToString(pinLabelingMethodAlpha).CompareNoCase(pinNumberingString) == 0)
   {
      retval = pinLabelingMethodAlpha;
   }
   else if (pinLabelingMethodTagToString(pinLabelingMethodNumeric).CompareNoCase(pinNumberingString) == 0)
   {
      retval = pinLabelingMethodNumeric;
   }
   else if (pinLabelingMethodTagToString(pinLabelingMethodAlphaNumeric).CompareNoCase(pinNumberingString) == 0)
   {
      retval = pinLabelingMethodAlphaNumeric;
   }
   else if (pinLabelingMethodTagToString(pinLabelingMethodNumericAlpha).CompareNoCase(pinNumberingString) == 0)
   {
      retval = pinLabelingMethodNumericAlpha;
   }

   return retval;
}

//_____________________________________________________________________________
CString pinOrderingMethodTagToString(PinOrderingMethodTag tagValue)
{
   CString retval;

   switch (tagValue)
   {
   case pinOrderingMethodManual:            retval = "Manual";             break;
   case pinOrderingMethodClockwise:         retval = "Clockwise";          break;
   case pinOrderingMethodCounterClockwise:  retval = "Counter Clockwise";  break;
   case pinOrderingMethodReadingOrder:      retval = "Reading Order";      break;
   default:                                 retval = "Undefined";          break;
   }

   return retval;
}

PinOrderingMethodTag stringToPinOrderingMethodTag(CString tagValue)
{
   PinOrderingMethodTag retval = pinOrderingMethodUndefined;

   if (pinOrderingMethodTagToString(pinOrderingMethodManual).CompareNoCase(tagValue) == 0)
   {
      retval = pinOrderingMethodManual;
   }
   else if (pinOrderingMethodTagToString(pinOrderingMethodClockwise).CompareNoCase(tagValue) == 0)
   {
      retval = pinOrderingMethodClockwise;
   }
   else if (pinOrderingMethodTagToString(pinOrderingMethodCounterClockwise).CompareNoCase(tagValue) == 0)
   {
      retval = pinOrderingMethodCounterClockwise;
   }
   else if (pinOrderingMethodTagToString(pinOrderingMethodReadingOrder).CompareNoCase(tagValue) == 0)
   {
      retval = pinOrderingMethodReadingOrder;
   }

   return retval;
}

PinOrderingMethodTag intToPinOrderMethodTag(int tagValue)
{
	if (tagValue >= pinOrderingMethodManual && tagValue <= pinOrderingMethodReadingOrder)
		return (PinOrderingMethodTag)tagValue;
	else
		return pinOrderingMethodUndefined;
}

//_____________________________________________________________________________
CString ipcStatusTagToString(IPCStatusTag ipcStatusTag)
{
   CString retval;

	switch (ipcStatusTag)
	{
	case IPCStatusNonStandardPart:		retval = "Non Standard Part";		break;
	case IPCStatusNonStandardIPC:			retval = "Non Standard IPC";		break;
	case IPCStatusStandardIPC:				retval = "Standard IPC";			break;
	default:										retval = "Undefined";				break;
	}

	return retval;
}

IPCStatusTag stringToIpcStatusTag(CString ipcStatusString)
{
	IPCStatusTag retval = IPCStatusUndefined;

	if (ipcStatusTagToString(IPCStatusNonStandardPart).CompareNoCase(ipcStatusString) == 0)
	{
		retval = IPCStatusNonStandardPart;
	}
	else if (ipcStatusTagToString(IPCStatusNonStandardIPC).CompareNoCase(ipcStatusString) == 0)
	{
		retval = IPCStatusNonStandardIPC;
	}
	else if (ipcStatusTagToString(IPCStatusStandardIPC).CompareNoCase(ipcStatusString) == 0)
	{
		retval = IPCStatusStandardIPC;
	}

	return retval;
}


//-----------------------------------------------------------------------------
// CPolarCoordinatePinArray
//-----------------------------------------------------------------------------
CPolarCoordinatePinArray::CPolarCoordinatePinArray(CCamCadDatabase& camCadDatabase, CDataList& dataList)
	: m_camCadDatabase(camCadDatabase)
	, m_dataList(dataList)
	, m_pinsArray()
{
	m_pinsArray.empty();
	m_tolerance = Units_Factor(pageUnitsMils, m_camCadDatabase.getCamCadDoc().getSettings().getPageUnits()) * .01;

	populatePolarCoordinateArray();
}

CPolarCoordinatePinArray::~CPolarCoordinatePinArray()
{
	m_pinsArray.empty();
}

void CPolarCoordinatePinArray::populatePolarCoordinateArray()
{
	CPoint2d offset = m_dataList.getExtent().getCenter();

	m_pinsArray.SetSize(0, m_dataList.GetCount());

	for (CDataListIterator dataList(m_dataList, insertTypePin); dataList.hasNext();)
	{
		DataStruct* data = dataList.getNext();
		if (data == NULL || data->getInsert() == NULL)
			continue;

		CPolarCoorinatePin* polarPin = new CPolarCoorinatePin(*data, data->getInsert()->getOrigin2d() - offset );

		int index = 0;
		for (index=0; index<m_pinsArray.GetCount(); index++)
		{
			CPolarCoorinatePin* curPolarPin = m_pinsArray.GetAt(index);
			if (curPolarPin == NULL)
				continue;

			if (polarPin->getTheta() < curPolarPin->getTheta() ||
				 (polarPin->getTheta() == curPolarPin->getTheta() && polarPin->getR() < curPolarPin->getR()))
			{
				m_pinsArray.InsertAt(index, polarPin);
				break;
			}
		}

		if (index == m_pinsArray.GetCount())
			m_pinsArray.SetAtGrow(m_pinsArray.GetCount(), polarPin);
	}
}

void CPolarCoordinatePinArray::analysizePinNumbering(DataStruct& pinOneData, bool clockwise)
{
	if (pinOneData.getDataType() != dataTypeInsert || pinOneData.getInsert()->getInsertType() != insertTypePin)
		return;

	// Find index of pin one
	int pinOneIndex = 0;
	for (int index=0; index<m_pinsArray.GetCount(); index++)
	{
		CPolarCoorinatePin* curPolarPin = m_pinsArray.GetAt(index);
		if (curPolarPin == NULL)
			continue;

		if (curPolarPin->getPinData().getInsert() == pinOneData.getInsert())
		{
			pinOneIndex = index;
			break;
		}
	}
	
	CString newPinName;
	int curIndex = pinOneIndex;
	int pinNum = 0;
	do
	{
		CPolarCoorinatePin* curPolarPin = m_pinsArray.GetAt(curIndex);
		if (curPolarPin == NULL)
			continue;

		newPinName.Format("%d", ++pinNum);
		curPolarPin->getPinData().getInsert()->setRefname(newPinName);
		//curPolarPin->getPinData().setAttrib(&m_camCadDatabase.getCamCadDoc(), m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributePinNumber),
		//	valueTypeInteger, (void*)&pinNum, SA_OVERWRITE, NULL);

		if (!clockwise)
		{
			curIndex++;

			// curIndex is at end of array, set it to the beginning
			if (curIndex == m_pinsArray.GetCount())
				curIndex = 0;
		}
		else
		{
			curIndex--;

			// curIndex is at the beginning of array, set it to the end
			if (curIndex < 0)
				curIndex = m_pinsArray.GetCount()-1;
		}

	} while (curIndex != pinOneIndex);
}

//_____________________________________________________________________________
COrderedDoubleArray::COrderedDoubleArray(double tolerance,int growBy)
: m_tolerance(tolerance)
, m_growBy(growBy)
{
   empty();
}

void COrderedDoubleArray::empty()
{
   m_array.SetSize(0,m_growBy);

   m_staleFlag = true;
   m_minInterval = 0.;
   m_maxInterval = 0.;
}

int COrderedDoubleArray::add(double value)
{
   int retval = -1;

   for (int index = 0;;index++)
   {
      if (index == m_array.GetSize())
      {
         m_array.SetAtGrow(index,value);
         retval = index;

         m_staleFlag = true;

         break;
      }

      double delta = value - m_array.GetAt(index);

      // x already existed
      if (fpnear(delta,0.,m_tolerance))
      {
         break;
      }

      if (value < m_array.GetAt(index))
      {
         m_array.InsertAt(index,value);
         retval = index;

         m_staleFlag = true;

         break;
      }
   }

   return retval;
}

double COrderedDoubleArray::getAt(int index) const
{
   return m_array.GetAt(index);
}

int COrderedDoubleArray::getSize() const
{
   return m_array.GetSize();
}

int COrderedDoubleArray::findIndex(double value)
{
   int retval = -1;

   int loIndex = 0;
   int hiIndex = m_array.GetSize() - 1;

   while (loIndex <= hiIndex)
   {
      int index = (loIndex + hiIndex) / 2;
      double arrayValue = m_array.GetAt(index);

      if (fpnear(value,arrayValue,m_tolerance))
      {
         retval = index;

         break;
      }

      if (value < arrayValue)
      {
         hiIndex = index - 1;
      }
      else
      {
         loIndex = index + 1;
      }
   }

   return retval;
}

void COrderedDoubleArray::analyze() const
{
   if (m_staleFlag)
   {
      if (getSize() < 2)
      {
         m_minInterval = 0.;
         m_maxInterval = 0.;
      }
      else
      {
         m_maxInterval = 0.;
         m_minInterval = m_array.GetAt(getSize() - 1) - m_array.GetAt(0);
         double previousValue;

         for (int index = 0;index < m_array.GetSize();index++)
         {
            double value = m_array.GetAt(index);

            if (index > 0)
            {
               double interval = value - previousValue;

               if (interval > m_maxInterval)
               {
                  m_maxInterval = interval;
               }

               if (interval < m_minInterval)
               {
                  m_minInterval = interval;
               }
            }

            previousValue = value;
         }
      }

      m_staleFlag = false;
   }
}

double COrderedDoubleArray::getMinInterval() const
{
   analyze();

   return m_minInterval;
}

double COrderedDoubleArray::getMaxInterval() const
{
   analyze();

   return m_maxInterval;
}

double COrderedDoubleArray::getInterval() const
{
   double interval = (getMinInterval() + getMaxInterval()) / 2.;

   return interval;
}

bool COrderedDoubleArray::hasRegularInterval() const
{
   bool retval = fpnear(getMinInterval(),getMaxInterval(),m_tolerance);

   return retval;
}

double COrderedDoubleArray::getRegularInterval(int offset) const
{
   double interval = 0.;
   int spanCount = getSize() - (2 * offset);

   if (spanCount > 1 && offset >= 0)
   {
      double maxInterval = 0.;
      double minInterval = m_array.GetAt(getSize() - offset - 1) - m_array.GetAt(offset);
      double previousValue;
      double firstValue;
      double value;

      for (int index = offset;index < m_array.GetSize() - offset;index++)
      {
         value = m_array.GetAt(index);

         if (index > offset)
         {
            double interval = value - previousValue;

            if (interval > maxInterval)
            {
               maxInterval = interval;
            }

            if (interval < minInterval)
            {
               minInterval = interval;
            }
         }
         else
         {
            firstValue = value;
         }

         previousValue = value;
      }

      if (fpnear(minInterval,maxInterval,m_tolerance))
      {
         interval = (value - firstValue) / (spanCount - 1);
      }
   }

   return interval;
}

//-----------------------------------------------------------------------------
// CPcbComponentPinAnalyzer
//-----------------------------------------------------------------------------
CPcbComponentPinAnalyzer::CPcbComponentPinAnalyzer(CCamCadDatabase& camCadDatabase, CDataList& dataList)
: m_camCadDatabase(camCadDatabase)
, m_componentGeometry(NULL)
, m_componentCentroid(NULL)
, m_dataList(dataList)
, m_pinsBucket(10,false)
, m_tolerance(m_camCadDatabase.convertToPageUnits(pageUnitsMils,1.5))
, m_xArray(m_tolerance,100)
, m_yArray(m_tolerance,100)
, m_pinDataTreeList(m_tolerance)
, m_footprintAnalyzedFlag(false)
, m_pcbComponentPackageType(pcbComponentPackageTypeUndefined)
, m_mainPins(false)
, m_thPins(false)
, m_smdPins(false)
, m_extraPins(false)
{
   initializeMainPins();
}

CPcbComponentPinAnalyzer::CPcbComponentPinAnalyzer(CCamCadDatabase& camCadDatabase, BlockStruct& componentGeometry)
: m_camCadDatabase(camCadDatabase)
, m_componentGeometry(&componentGeometry)
, m_componentCentroid(NULL)
, m_dataList(componentGeometry.getDataList())
, m_pinsBucket(10,false)
, m_tolerance(m_camCadDatabase.convertToPageUnits(pageUnitsMils,1.5))
, m_xArray(m_tolerance,100)
, m_yArray(m_tolerance,100)
, m_pinDataTreeList(m_tolerance)
, m_footprintAnalyzedFlag(false)
, m_pcbComponentPackageType(pcbComponentPackageTypeUndefined)
, m_mainPins(false)
, m_thPins(false)
, m_smdPins(false)
, m_extraPins(false)
{
   initializeMainPins();
}

CPcbComponentPinAnalyzer::~CPcbComponentPinAnalyzer()
{
}

bool CPcbComponentPinAnalyzer::isThruHolePin(DataStruct& pin)
{
   bool thFlag = false;

   if (pin.getDataType() == dataTypeInsert)
   {
      BlockStruct* padStackGeometry = m_camCadDatabase.getBlock(pin.getInsert()->getBlockNumber());

      for (CDataListIterator insertIterator(*padStackGeometry,dataTypeInsert);insertIterator.hasNext();)
      {
         DataStruct* pad = insertIterator.getNext();

         BlockStruct* padGeometry = m_camCadDatabase.getBlock(pad->getInsert()->getBlockNumber());

         if (padGeometry->isDrillHole())
         {
            thFlag = true;
            break;
         }
      }
   }

   return thFlag;
}

bool CPcbComponentPinAnalyzer::isThruHoleComponent()
{
   bool retval = (m_thPins.GetCount() > 0);

   return retval;
}

void CPcbComponentPinAnalyzer::initializeMainPins()
{
   m_componentCentroid = NULL;

   for (CDataListIterator dataList(m_dataList, dataTypeInsert); dataList.hasNext();)
   {
      DataStruct* pin = dataList.getNext();

      if (pin->getInsert()->getInsertType() == insertTypePin)
      {
         m_mainPins.AddTail(pin);

         if (isThruHolePin(*pin))
         {
            m_thPins.AddTail(pin);
         }
         else
         {
            m_smdPins.AddTail(pin);
         }
      }
      else if (pin->getInsert()->getInsertType() == insertTypeCentroid && m_componentCentroid == NULL)
      {
         m_componentCentroid = pin;
      }
   }

   initializeAnalysisData();
}

void CPcbComponentPinAnalyzer::initializeAnalysisData()
{
   populateXYArrays();
   populatePinsBucket();

   m_footprintAnalyzedFlag = false;
   m_bookReadingOrder      = bookReadingUndefined;
   m_pinNumberingMethod    = pinLabelingMethodUndefined;
   m_pinOrderingMethod     = pinOrderingMethodUndefined;
   m_ipcStatus             = IPCStatusUndefined;
   m_alphaSkipList.Empty();
}

int CPcbComponentPinAnalyzer::getXArrayIndex(double xCoordinate)
{
   return m_xArray.findIndex(xCoordinate);
}

int CPcbComponentPinAnalyzer::getYArrayIndex(double yCoordinate)
{
   return m_yArray.findIndex(yCoordinate);
}

int CPcbComponentPinAnalyzer::getPinsBucketIndex(int xIndex,int yIndex)
{
   int retval;

   if (xIndex > -1 && yIndex > -1)
      retval = xIndex + yIndex * m_xArray.getSize();
   else
      retval = -1;

   return retval;
}

DataStruct* CPcbComponentPinAnalyzer::getPinAtPinBucket(int xIndex,int yIndex)
{
   return m_pinsBucket.GetAt(getPinsBucketIndex(xIndex,yIndex));
}

int CPcbComponentPinAnalyzer::getPinsBucketIndex(CPoint2d origin)
{
   int xIndex = getXArrayIndex(origin.x);
   int yIndex = getYArrayIndex(origin.y);

   return getPinsBucketIndex(xIndex,yIndex);
}

void CPcbComponentPinAnalyzer::populateXYArrays()
{
   m_xArray.empty();
   m_yArray.empty();
   m_pinDataTreeList.empty();

   for (CDataListIterator dataList(m_mainPins, insertTypePin); dataList.hasNext();)
   {
      DataStruct* data = dataList.getNext();

      CPoint2d origin = data->getInsert()->getOrigin2d();

      m_xArray.add(origin.x);
      m_yArray.add(origin.y);
      m_pinDataTreeList.add(data);
   }
}

void CPcbComponentPinAnalyzer::populatePinsBucket()
{
   m_pinsBucket.empty();
   m_pinsBucket.SetSize(m_xArray.getSize() * m_yArray.getSize());
   m_pinCount = 0;

   for (CDataListIterator dataList(m_mainPins, insertTypePin); dataList.hasNext();)
   {
      DataStruct* data = dataList.getNext();

      int index = getPinsBucketIndex(data->getInsert()->getOrigin2d());

      if (index > -1)
      {
         m_pinsBucket.SetAtGrow(index,  data);
         m_pinCount++;
      }
   }
}

void CPcbComponentPinAnalyzer::populateAlphaOrNumericPinNumbering(
   MajorOrdinateTag majorOrdinate,SequenceOrderTag majorSequenceOrder,
   SequenceOrderTag minorSequenceOrder,LabelPrefixTag labelPrefix)
{
   bool xMajorFlag         = (majorOrdinate      == majorOrdinateX        );
   bool majorAscendingFlag = (majorSequenceOrder == sequenceOrderAscending);
   bool minorAscendingFlag = (minorSequenceOrder == sequenceOrderAscending);
   bool alphaPrefixFlag    = (labelPrefix        == labelPrefixAlpha      );

   int majorIncrement = (majorAscendingFlag ? 1 : -1);
   int minorIncrement = (minorAscendingFlag ? 1 : -1);

   COrderedDoubleArray& majorArray = (xMajorFlag ? m_xArray : m_yArray);
   COrderedDoubleArray& minorArray = (xMajorFlag ? m_yArray : m_xArray);

   int majorIndexStart = (majorAscendingFlag ? 0                      : majorArray.getSize() - 1);
   int majorIndexEnd   = (majorAscendingFlag ? majorArray.getSize() - 1 : 0                     );
   int minorIndexStart = (minorAscendingFlag ? 0                      : minorArray.getSize() - 1);
   int minorIndexEnd   = (minorAscendingFlag ? minorArray.getSize() - 1 : 0                     );

   int xIndex;
   int yIndex;

   int& majorIndex = (xMajorFlag ? xIndex : yIndex);
   int& minorIndex = (xMajorFlag ? yIndex : xIndex);
   CStringArray& prefixArray = (alphaPrefixFlag ? m_alphaLabelsArray   : m_numericLabelsArray);

   int prefixIndex = 0;
   CString label;

   for (majorIndex = majorIndexStart;;majorIndex += majorIncrement)
   {
      for (minorIndex = minorIndexStart;;minorIndex += minorIncrement)
      {
         int pinIndex = xIndex + yIndex * m_xArray.getSize();
         DataStruct* pinData = m_pinsBucket.GetAt(pinIndex);

         if (pinData != NULL)
         {
            label = prefixArray.GetAt(prefixIndex);
            prefixIndex++;
         }

         m_pinLabelsArray.SetAt(pinIndex, label);

         if (minorIndex == minorIndexEnd)
         {
            break;
         }
      }

      if (majorIndex == majorIndexEnd)
      {
         break;
      }
   }
}

void CPcbComponentPinAnalyzer::populateAlphaPinNumbering(BookReadingOrderTag readingOrder)
{
   int alphaIndex = 0;

   switch (readingOrder)
   {
   case bookReadingTopBottomLeftRight: 
      populateAlphaOrNumericPinNumbering(majorOrdinateX,sequenceOrderAscending,sequenceOrderDescending,labelPrefixAlpha);

      break;
   case bookReadingTopBottomRightLeft:    
      populateAlphaOrNumericPinNumbering(majorOrdinateX,sequenceOrderDescending,sequenceOrderDescending,labelPrefixAlpha);

      break;
   case bookReadingBottomTopLeftRight:    
      populateAlphaOrNumericPinNumbering(majorOrdinateX,sequenceOrderAscending,sequenceOrderAscending,labelPrefixAlpha);

      break;
   case bookReadingBottomTopRightLeft:    
      populateAlphaOrNumericPinNumbering(majorOrdinateX,sequenceOrderDescending,sequenceOrderAscending,labelPrefixAlpha);

      break;
   case bookReadingLeftRightTopBottom:    
      populateAlphaOrNumericPinNumbering(majorOrdinateY,sequenceOrderDescending,sequenceOrderAscending,labelPrefixAlpha);

      break;
   case bookReadingLeftRightBottomTop:    
      populateAlphaOrNumericPinNumbering(majorOrdinateY,sequenceOrderAscending,sequenceOrderAscending,labelPrefixAlpha);

      break;
   case bookReadingRightLeftTopBottom:    
      populateAlphaOrNumericPinNumbering(majorOrdinateY,sequenceOrderDescending,sequenceOrderDescending,labelPrefixAlpha);

      break;
   case bookReadingRightLeftBottomTop:    
      populateAlphaOrNumericPinNumbering(majorOrdinateY,sequenceOrderAscending,sequenceOrderDescending,labelPrefixAlpha);

      break;
   default:
      break;
   }
}

void CPcbComponentPinAnalyzer::populateNumericPinNumbering(BookReadingOrderTag readingOrder)
{
   int numericIndex = 0;

   switch (readingOrder)
   {
   case bookReadingTopBottomLeftRight: 
      populateAlphaOrNumericPinNumbering(majorOrdinateX,sequenceOrderAscending,sequenceOrderDescending,labelPrefixNumeric);

      break;
   case bookReadingTopBottomRightLeft:    
      populateAlphaOrNumericPinNumbering(majorOrdinateX,sequenceOrderDescending,sequenceOrderDescending,labelPrefixNumeric);

      break;
   case bookReadingBottomTopLeftRight:    
      populateAlphaOrNumericPinNumbering(majorOrdinateX,sequenceOrderAscending,sequenceOrderAscending,labelPrefixNumeric);

      break;
   case bookReadingBottomTopRightLeft:    
      populateAlphaOrNumericPinNumbering(majorOrdinateX,sequenceOrderDescending,sequenceOrderAscending,labelPrefixNumeric);

      break;
   case bookReadingLeftRightTopBottom:    
      populateAlphaOrNumericPinNumbering(majorOrdinateY,sequenceOrderDescending,sequenceOrderAscending,labelPrefixNumeric);

      break;
   case bookReadingLeftRightBottomTop:    
      populateAlphaOrNumericPinNumbering(majorOrdinateY,sequenceOrderAscending,sequenceOrderAscending,labelPrefixNumeric);

      break;
   case bookReadingRightLeftTopBottom:    
      populateAlphaOrNumericPinNumbering(majorOrdinateY,sequenceOrderDescending,sequenceOrderDescending,labelPrefixNumeric);

      break;
   case bookReadingRightLeftBottomTop:    
      populateAlphaOrNumericPinNumbering(majorOrdinateY,sequenceOrderAscending,sequenceOrderDescending,labelPrefixNumeric);

      break;
   default:
      break;
   }
}

void CPcbComponentPinAnalyzer::populateAlphaAndNumericPinNumbering(
   MajorOrdinateTag majorOrdinate,SequenceOrderTag majorSequenceOrder,
   SequenceOrderTag minorSequenceOrder,LabelPrefixTag labelPrefix)
{
   bool xMajorFlag         = (majorOrdinate      == majorOrdinateX        );
   bool majorAscendingFlag = (majorSequenceOrder == sequenceOrderAscending);
   bool minorAscendingFlag = (minorSequenceOrder == sequenceOrderAscending);
   bool alphaPrefixFlag    = (labelPrefix        == labelPrefixAlpha      );

   int majorIncrement = (majorAscendingFlag ? 1 : -1);
   int minorIncrement = (minorAscendingFlag ? 1 : -1);

   COrderedDoubleArray& majorArray = (xMajorFlag ? m_xArray : m_yArray);
   COrderedDoubleArray& minorArray = (xMajorFlag ? m_yArray : m_xArray);

   int majorIndexStart = (majorAscendingFlag ? 0                      : majorArray.getSize() - 1);
   int majorIndexEnd   = (majorAscendingFlag ? majorArray.getSize() - 1 : 0                     );
   int minorIndexStart = (minorAscendingFlag ? 0                      : minorArray.getSize() - 1);
   int minorIndexEnd   = (minorAscendingFlag ? minorArray.getSize() - 1 : 0                     );

   int xIndex;
   int yIndex;

   int& majorIndex = (xMajorFlag ? xIndex : yIndex);
   int& minorIndex = (xMajorFlag ? yIndex : xIndex);
   CStringArray& prefixArray = (alphaPrefixFlag ? m_alphaLabelsArray   : m_numericLabelsArray);
   CStringArray& suffixArray = (alphaPrefixFlag ? m_numericLabelsArray : m_alphaLabelsArray  );

   int prefixIndex = 0;
   int suffixIndex = 0;
   CString label;

   for (majorIndex = majorIndexStart;;majorIndex += majorIncrement)
   {
      suffixIndex = 0;

      for (minorIndex = minorIndexStart;;minorIndex += minorIncrement)
      {
         label.Format("%s%s", prefixArray.GetAt(prefixIndex),suffixArray.GetAt(suffixIndex));

         int pinLabelIndex = xIndex + yIndex * m_xArray.getSize();
         m_pinLabelsArray.SetAt(pinLabelIndex, label);

         suffixIndex++;

         if (minorIndex == minorIndexEnd)
         {
            break;
         }
      }

      prefixIndex++;

      if (majorIndex == majorIndexEnd)
      {
         break;
      }
   }
}

void CPcbComponentPinAnalyzer::populateAlphaNumericPinNumbering(BookReadingOrderTag readingOrder)
{
   m_bookReadingOrder   = readingOrder;
   m_pinNumberingMethod = pinLabelingMethodAlphaNumeric;

   switch (readingOrder)
   {
   case bookReadingTopBottomLeftRight: 
      populateAlphaAndNumericPinNumbering(majorOrdinateX,sequenceOrderAscending,sequenceOrderDescending,labelPrefixAlpha);

      break;
   case bookReadingTopBottomRightLeft:    
      populateAlphaAndNumericPinNumbering(majorOrdinateX,sequenceOrderDescending,sequenceOrderDescending,labelPrefixAlpha);

      break;
   case bookReadingBottomTopLeftRight:    
      populateAlphaAndNumericPinNumbering(majorOrdinateX,sequenceOrderAscending,sequenceOrderAscending,labelPrefixAlpha);

      break;
   case bookReadingBottomTopRightLeft:    
      populateAlphaAndNumericPinNumbering(majorOrdinateX,sequenceOrderDescending,sequenceOrderAscending,labelPrefixAlpha);

      break;
   case bookReadingLeftRightTopBottom:    
      populateAlphaAndNumericPinNumbering(majorOrdinateY,sequenceOrderDescending,sequenceOrderAscending,labelPrefixAlpha);

      break;
   case bookReadingLeftRightBottomTop:    
      populateAlphaAndNumericPinNumbering(majorOrdinateY,sequenceOrderAscending,sequenceOrderAscending,labelPrefixAlpha);

      break;
   case bookReadingRightLeftTopBottom:    
      populateAlphaAndNumericPinNumbering(majorOrdinateY,sequenceOrderDescending,sequenceOrderDescending,labelPrefixAlpha);

      break;
   case bookReadingRightLeftBottomTop:    
      populateAlphaAndNumericPinNumbering(majorOrdinateY,sequenceOrderAscending,sequenceOrderDescending,labelPrefixAlpha);

      break;
   default:
      break;
   }
}

void CPcbComponentPinAnalyzer::populateNumericAlphaPinNumbering(BookReadingOrderTag readingOrder)
{
   m_bookReadingOrder   = readingOrder;
   m_pinNumberingMethod = pinLabelingMethodNumericAlpha;

   switch (readingOrder)
   {
   case bookReadingTopBottomLeftRight: 
      populateAlphaAndNumericPinNumbering(majorOrdinateX,sequenceOrderAscending,sequenceOrderDescending,labelPrefixNumeric);

      break;
   case bookReadingTopBottomRightLeft:    
      populateAlphaAndNumericPinNumbering(majorOrdinateX,sequenceOrderDescending,sequenceOrderDescending,labelPrefixNumeric);

      break;
   case bookReadingBottomTopLeftRight:    
      populateAlphaAndNumericPinNumbering(majorOrdinateX,sequenceOrderAscending,sequenceOrderAscending,labelPrefixNumeric);

      break;
   case bookReadingBottomTopRightLeft:    
      populateAlphaAndNumericPinNumbering(majorOrdinateX,sequenceOrderDescending,sequenceOrderAscending,labelPrefixNumeric);

      break;
   case bookReadingLeftRightTopBottom:    
      populateAlphaAndNumericPinNumbering(majorOrdinateY,sequenceOrderDescending,sequenceOrderAscending,labelPrefixNumeric);

      break;
   case bookReadingLeftRightBottomTop:    
      populateAlphaAndNumericPinNumbering(majorOrdinateY,sequenceOrderAscending,sequenceOrderAscending,labelPrefixNumeric);

      break;
   case bookReadingRightLeftTopBottom:    
      populateAlphaAndNumericPinNumbering(majorOrdinateY,sequenceOrderDescending,sequenceOrderDescending,labelPrefixNumeric);

      break;
   case bookReadingRightLeftBottomTop:    
      populateAlphaAndNumericPinNumbering(majorOrdinateY,sequenceOrderAscending,sequenceOrderDescending,labelPrefixNumeric);

      break;
   default:
      break;
   }
}

void CPcbComponentPinAnalyzer::populateAlphaAndNumbericLabels(const CString& skipAlphas)
{
   m_alphaSkipList = skipAlphas;

   char validAlphaCharacters[26];
   int validAlphaCharacterCount = 0;
   CString label;

   while (true)
   {
      for (char c = 'A';c <= 'Z';c++)
      {
         if (m_alphaSkipList.Find(c,0) < 0)
         {
            validAlphaCharacters[validAlphaCharacterCount++] = c;
         }
      }

      if (validAlphaCharacterCount != 0)
      {
         break;
      }

      m_alphaSkipList.Empty();
   }

   for (int i=0; i<m_alphaLabelsArray.GetSize(); i++)
   {
      label.Empty();

      for (int index = i;;)
      {
         label = validAlphaCharacters[index % validAlphaCharacterCount] + label;

         index /= validAlphaCharacterCount;

         if (index == 0)
         {
            break;
         }
         else
         {
            index--;
         }
      }

      m_alphaLabelsArray.SetAt(i, label);
   }

   for (int i=0; i<m_numericLabelsArray.GetSize(); i++)
   {
      label.Format("%d", i+1);
      m_numericLabelsArray.SetAt(i, label);
   }
}

DataStruct* CPcbComponentPinAnalyzer::analysizePinNumbering(BookReadingOrderTag readingOrderTag, PinLabelingMethodTag pinNumberingTag,const CString& skipAlphas)
{
   DataStruct* firstPin = NULL;

   if (pinNumberingTag == pinLabelingMethodUndefined)
   {
      return firstPin;
   }
   else if (readingOrderTag == bookReadingUndefined)
   {
      return firstPin;
   }

   m_pinLabelsArray.RemoveAll();
   m_pinLabelsArray.SetSize(m_xArray.getSize() * m_yArray.getSize(), 0);
   m_alphaLabelsArray.RemoveAll();
   m_alphaLabelsArray.SetSize(m_xArray.getSize() * m_yArray.getSize(), 0);
   m_numericLabelsArray.RemoveAll();
   m_numericLabelsArray.SetSize(m_xArray.getSize() * m_yArray.getSize(), 0);

   populateAlphaAndNumbericLabels(skipAlphas);

   // Create pin numbering lables
   switch (pinNumberingTag)
   {
   case pinLabelingMethodAlpha:             
      populateAlphaPinNumbering(readingOrderTag);
      break;
   case pinLabelingMethodNumeric:
      populateNumericPinNumbering(readingOrderTag);
      break;
   case pinLabelingMethodAlphaNumeric:
      populateAlphaNumericPinNumbering(readingOrderTag);
      break;
   case pinLabelingMethodNumericAlpha:
      populateNumericAlphaPinNumbering(readingOrderTag);
      break;
   default:
      break;
   }

   // Update pin numbering to pin inserts and attributes
   int alphaLabelIndex = 0;
   int numericLabelIndex = 0;
   int pinLabelIndex = 0;

   for (int i=0; i<m_pinsBucket.GetCount(); i++)
   {
      DataStruct* pinData = m_pinsBucket.GetAt(i);
      if (pinData == NULL || pinData->getInsert() == NULL)
         continue;

      CString newPinName = m_pinLabelsArray.GetAt(i);
      pinData->getInsert()->setRefname(newPinName);

      if (firstPin == NULL || firstPin->getInsert()->getRefname().CompareNoCase(newPinName) > 0)
      {
         firstPin = pinData;
      }
   }

   return firstPin;
}

CString CPcbComponentPinAnalyzer::getGeometryDescriptor()
{
   analyzeFootprint();

   return m_geometryDescriptor;
}

PcbComponentPackageTypeTag CPcbComponentPinAnalyzer::getPcbComponentPackageType()
{
   analyzeFootprint();

   return m_pcbComponentPackageType;
}

void CPcbComponentPinAnalyzer::analyzeFootprint()
{
   if (m_footprintAnalyzedFlag)
   {
      return;
   }

   const double oneHundredMilsInPageUnits = m_camCadDatabase.convertToPageUnits(pageUnitsMils,100.);

   m_minSize = min(m_xArray.getSize(),m_yArray.getSize());
   m_maxSize = max(m_xArray.getSize(),m_yArray.getSize());

   bool smdFlag = (m_thPins.GetCount() <= m_smdPins.GetCount());

   if (m_xArray.hasRegularInterval() && m_yArray.hasRegularInterval())
   {
      m_pinPitch = min(m_xArray.getInterval(),m_yArray.getInterval());
      m_pinSpan  = max(m_xArray.getInterval(),m_yArray.getInterval());
      
      if (m_pinCount == 0)
      {  
         m_geometryDescriptor = "NoPins";
         m_pcbComponentPackageType = pcbComponentPackageTypeNoPins;
      }
      else if (m_pinCount == 1)
      {  
         if (smdFlag)
         {
            m_geometryDescriptor = "TestPoint";
            m_pcbComponentPackageType = pcbComponentPackageTypeTestPoint;
         }
         else
         {
            m_geometryDescriptor = "TestPin";
            m_pcbComponentPackageType = pcbComponentPackageTypeTestPin;
         }
      }
      else if (m_pinCount == 2)
      {  
         if (smdFlag)
         {
            m_geometryDescriptor = "Chip";
            m_pcbComponentPackageType = pcbComponentPackageTypeChip;
         }
         else
         {
            if (fpnear(m_pinSpan,oneHundredMilsInPageUnits,m_tolerance))
            {
               m_geometryDescriptor = "Jumper";
               m_pcbComponentPackageType = pcbComponentPackageTypeJumper;
            }
            else
            {
               m_geometryDescriptor = "Connector";
               m_pcbComponentPackageType = pcbComponentPackageTypeConnector;
            }
         }

      }
      else if (m_pinCount == 3)
      {  
         double minPitch = m_camCadDatabase.convertToPageUnits(pageUnitsMils, 20.);
         double maxPitch = m_camCadDatabase.convertToPageUnits(pageUnitsMils, 45.);
         double minSpan  = m_camCadDatabase.convertToPageUnits(pageUnitsMils, 60.);
         double maxSpan  = m_camCadDatabase.convertToPageUnits(pageUnitsMils,100.);

         if (m_minSize == 2 && m_maxSize == 3)
         {
            if (m_pinPitch >= minPitch && m_pinPitch <= maxPitch &&
                m_pinSpan  >= minSpan  && m_pinSpan  <= maxSpan     )
            {
               if (smdFlag)
               {
                  m_geometryDescriptor = "Sot23";
                  m_pcbComponentPackageType = pcbComponentPackageTypeSot23;
               }
            }
            else
            {
               DataStruct* middlePin = NULL;

               if (m_xArray.getSize() == 3)
               {
                  middlePin = getPinAtPinBucket(1,0);

                  if (middlePin == NULL)
                  {
                     middlePin = getPinAtPinBucket(1,1);
                  }
               }
               else
               {
                  middlePin = getPinAtPinBucket(0,1);

                  if (middlePin == NULL)
                  {
                     middlePin = getPinAtPinBucket(1,1);
                  }
               }

               if (middlePin != NULL)
               {
                  BlockStruct* apertureBlock = m_camCadDatabase.getBlock(middlePin->getInsert()->getBlockNumber());

                  if (apertureBlock->isSimpleAperture())
                  {
                     double minPinSize = m_camCadDatabase.convertToPageUnits(pageUnitsMils, 150.);

                     if (apertureBlock->getSizeA() >= minPinSize)
                     {
                        m_geometryDescriptor = "Crystal";
                        m_pcbComponentPackageType = pcbComponentPackageTypeCrystal;
                     }
                  }
               }
            }
         }
      }
      else if (m_minSize * m_maxSize == m_pinCount && m_minSize == 2)
      {
         if (smdFlag)
         {
            if (m_maxSize > 3)
            {
               double ratio = m_pinSpan / m_pinPitch;

               if (ratio > 20.)
               {
                  m_geometryDescriptor.Format("Tsop%d",m_pinCount);
                  m_pcbComponentPackageType = pcbComponentPackageTypeTsop;
               }
               else
               {
                  m_geometryDescriptor.Format("Soic%d",m_pinCount);
                  m_pcbComponentPackageType = pcbComponentPackageTypeSoic;
               }
            }
         }
         else
         {
            if (fpnear(m_pinSpan,oneHundredMilsInPageUnits,m_tolerance))
            {
               m_geometryDescriptor.Format("Header%dx%d",m_minSize,m_maxSize);
               m_pcbComponentPackageType = pcbComponentPackageTypeHeader;
            }
            else
            {
               m_geometryDescriptor.Format("Dip%d",m_pinCount);
               m_pcbComponentPackageType = pcbComponentPackageTypeDip;
            }
         }
      }
      else if (m_minSize == m_maxSize && m_minSize > 3)
      {
         if (m_minSize > 10 && fpeq(m_pinPitch,m_pinSpan))
         {
            if (smdFlag)
            {
               m_geometryDescriptor.Format("Bga%d(%dx%d)",m_pinCount,m_minSize,m_maxSize);
               m_pcbComponentPackageType = pcbComponentPackageTypeBga;
            }
            else
            {
               m_geometryDescriptor.Format("Pga%d(%dx%d)",m_pinCount,m_minSize,m_maxSize);
               m_pcbComponentPackageType = pcbComponentPackageTypePga;
            }  
         }
      }

      if (m_geometryDescriptor.IsEmpty())
      {
         m_geometryDescriptor.Format("%d_%s_%s",m_pinCount,fpfmt(m_pinPitch,3),fpfmt(m_pinSpan,3));
         m_pcbComponentPackageType = pcbComponentPackageTypeMisc;
      }
   }
   else if (m_minSize > 4 && (m_minSize - 2)*2 + (m_maxSize - 2)*2 == m_pinCount)
   {
      double xInterval = m_xArray.getRegularInterval(1);
      double yInterval = m_yArray.getRegularInterval(1);

      if (xInterval > 0. && fpnear(xInterval,yInterval,m_tolerance))
      {
         if (smdFlag)
         {
            if (m_minSize == m_maxSize)
            {
               m_geometryDescriptor.Format("Plcc%d",m_pinCount);
               m_pcbComponentPackageType = pcbComponentPackageTypePlcc;
            }
            else
            {
               m_geometryDescriptor.Format("Plcc%d(%dx%d)",m_pinCount,m_minSize - 2,m_maxSize - 2);
               m_pcbComponentPackageType = pcbComponentPackageTypePlcc;
            }
         }
      }
   }
   else if (m_extraPins.GetCount() == 0)
   {
      int pinCount = m_pinDataTreeList.GetCount();
      m_mainPins.empty();

      CPinChainGroup pinChainGroup(m_pinDataTreeList,m_camCadDatabase.getPageUnits(),m_tolerance);
      CPinChainList& pinChainList = pinChainGroup.getPinChainList();

      const int extraPinThreshHoldCount = 4;

      for (POSITION pos = pinChainList.getHeadPosition();pos != NULL;)
      {
         CPinChain* pinChain = pinChainList.getNext(pos);

         CDataList& classifiedPinsDataList = ((pinChain->getPinCount() > extraPinThreshHoldCount) ? m_mainPins : m_extraPins);
         CPinDataList& pinDataList = pinChain->getPinDataList();

         for (POSITION pinPos = pinDataList.GetHeadPosition();pinPos != NULL;)
         {
            CPinData* pinData = pinDataList.GetNext(pinPos);
            DataStruct* pad = pinData->getTopPad();
            classifiedPinsDataList.AddTail(pad);
         }
      }

      if (m_mainPins.GetCount() == 0)
      {
         m_mainPins.takeData(m_extraPins);
      }
      else if (m_extraPins.GetCount() > 0)
      {
         initializeAnalysisData();
         analyzeFootprint();
      }
   }

   m_footprintAnalyzedFlag = true;
}

void CPcbComponentPinAnalyzer::setInsertType(CDataList* pinList,InsertTypeTag insertType)
{
   if (pinList != NULL)
   {
      for (CDataListIterator insertIterator(*pinList,dataTypeInsert);insertIterator.hasNext();)
      {
         DataStruct* pin = insertIterator.getNext();
         pin->getInsert()->setInsertType(insertType);
      }
   }
}

void CPcbComponentPinAnalyzer::makeExtraPinsFiducials()
{
   int fiducialCount = 0;
   CString pinName;

   for (CDataListIterator insertIterator(m_extraPins,dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct* pin = insertIterator.getNext();
      pin->getInsert()->setInsertType(insertTypeFiducial);

      fiducialCount++;
      pinName.Format("FD%d",fiducialCount);

      pin->getInsert()->setRefname(pinName);
   }
}

bool CPcbComponentPinAnalyzer::getIndexPin(int& xIndex,int& yIndex)
{
   bool retval = false;
   xIndex = -1;
   yIndex = -1;

   switch (m_pcbComponentPackageType)
   {
   case pcbComponentPackageTypeBga:
   case pcbComponentPackageTypePga:
      break;
   case pcbComponentPackageTypeChip:
      break;
   case pcbComponentPackageTypeCrystal:
      break;
   case pcbComponentPackageTypeMisc:
      break;
   case pcbComponentPackageTypeNoPins:
      break;
   case pcbComponentPackageTypePlcc:
   case pcbComponentPackageTypeQfp:
      {
         DataStruct* bottomOfLeftPins  = getPinAtPinBucket(0                     ,1                     );
         DataStruct* topOfLeftPins     = getPinAtPinBucket(0                     ,m_yArray.getSize() - 2);
         DataStruct* bottomOfRightPins = getPinAtPinBucket(m_xArray.getSize() - 1,1                     );
         DataStruct* topOfRightPins    = getPinAtPinBucket(m_xArray.getSize() - 1,m_yArray.getSize() - 2);

         DataStruct* leftOfTopPins     = getPinAtPinBucket(1                     ,m_yArray.getSize() - 1);
         DataStruct* rightOfTopPins    = getPinAtPinBucket(m_xArray.getSize() - 2,m_yArray.getSize() - 1);
         DataStruct* leftOfBottomPins  = getPinAtPinBucket(1                     ,0                     );
         DataStruct* rightOfBottomPins = getPinAtPinBucket(m_xArray.getSize() - 2,0                     );

         if (bottomOfLeftPins  != NULL && bottomOfLeftPins != NULL &&
             bottomOfRightPins != NULL && topOfRightPins   != NULL     )
         {
            if (bottomOfLeftPins->getInsert()->getBlockNumber() == 
                topOfRightPins->getInsert()->getBlockNumber()     )
            {
               if (bottomOfLeftPins->getInsert()->getBlockNumber() != 
                   topOfLeftPins->getInsert()->getBlockNumber()     )
               {
                  retval = true;
                  xIndex = 0;
                  yIndex = m_yArray.getSize() - 2;
               }
               else if (bottomOfLeftPins->getInsert()->getBlockNumber() != 
                        bottomOfRightPins->getInsert()->getBlockNumber()    )
               {
                  retval = true;
                  xIndex = m_xArray.getSize() - 1;
                  yIndex = 1;
               }
            }

            if (!retval &&
                leftOfTopPins->getInsert()->getBlockNumber() == 
                rightOfBottomPins->getInsert()->getBlockNumber()     )
            {
               if (leftOfTopPins->getInsert()->getBlockNumber() != 
                   rightOfTopPins->getInsert()->getBlockNumber()     )
               {
                  retval = true;
                  xIndex = m_xArray.getSize() - 2;
                  yIndex = m_yArray.getSize() - 1;
               }
               else if (leftOfTopPins->getInsert()->getBlockNumber() != 
                        leftOfBottomPins->getInsert()->getBlockNumber()    )
               {
                  retval = true;
                  xIndex = 1;
                  yIndex = 0;
               }
            }
         }
      }

      break;
   case pcbComponentPackageTypeDip:
   case pcbComponentPackageTypeHeader:
   case pcbComponentPackageTypeSoic:
   case pcbComponentPackageTypeTsop:
      {
         DataStruct* topLeftPin     = getPinAtPinBucket(0                     ,m_yArray.getSize() - 1);
         DataStruct* bottomLeftPin  = getPinAtPinBucket(0                     ,0);
         DataStruct* topRightPin    = getPinAtPinBucket(m_xArray.getSize() - 1,m_yArray.getSize() - 1);
         DataStruct* bottomRightPin = getPinAtPinBucket(m_xArray.getSize() - 1,0);

         if (topLeftPin  != NULL && bottomLeftPin  != NULL &&
             topRightPin != NULL && bottomRightPin != NULL     )
         {
            bool verticalFlag = (m_xArray.getSize() == 2);

            if (verticalFlag)
            {
               if (bottomLeftPin->getInsert()->getBlockNumber() == 
                   bottomRightPin->getInsert()->getBlockNumber()     )
               {
                  if (bottomLeftPin->getInsert()->getBlockNumber() != 
                      topLeftPin->getInsert()->getBlockNumber()     )
                  {
                     retval = true;
                     xIndex = 0;
                     yIndex = m_yArray.getSize() - 1;
                  }
                  else if (bottomLeftPin->getInsert()->getBlockNumber() != 
                           bottomRightPin->getInsert()->getBlockNumber()     )
                  {
                     retval = true;
                     xIndex = m_xArray.getSize() - 1;
                     yIndex = 0;
                  }
               }
            }
            else
            {
               if (topLeftPin->getInsert()->getBlockNumber() == 
                   bottomRightPin->getInsert()->getBlockNumber()     )
               {
                  if (topLeftPin->getInsert()->getBlockNumber() != 
                      bottomLeftPin->getInsert()->getBlockNumber()     )
                  {
                     retval = true;
                     xIndex = 0;
                     yIndex = 0;
                  }
                  else if (topLeftPin->getInsert()->getBlockNumber() != 
                           topRightPin->getInsert()->getBlockNumber()     )
                  {
                     retval = true;
                     xIndex = m_xArray.getSize() - 1;
                     yIndex = m_yArray.getSize() - 1;
                  }
               }
            }
         }
      }

      break;
   case pcbComponentPackageTypeSot23:
      break;
   case pcbComponentPackageTypeTestPoint:
      break;
   case pcbComponentPackageTypeUndefined:
      break;
   }

   return retval;
}

bool CPcbComponentPinAnalyzer::getIndexPinAndRotation(DataStruct*& indexPin,int& rotationDegrees)
{
   bool retval = false;

   int xIndex,yIndex;

   switch (m_pcbComponentPackageType)
   {
   case pcbComponentPackageTypeBga:
   case pcbComponentPackageTypePga:
      break;
   case pcbComponentPackageTypeChip:
      break;
   case pcbComponentPackageTypeCrystal:
      break;
   case pcbComponentPackageTypeMisc:
      break;
   case pcbComponentPackageTypeNoPins:
      break;
   case pcbComponentPackageTypePlcc:
   case pcbComponentPackageTypeQfp:
      {
         DataStruct* bottomOfLeftPins  = getPinAtPinBucket(0                     ,1);
         DataStruct* topOfLeftPins     = getPinAtPinBucket(0                     ,m_yArray.getSize() - 2);
         DataStruct* bottomOfRightPins = getPinAtPinBucket(m_xArray.getSize() - 1,1);
         DataStruct* topOfRightPins    = getPinAtPinBucket(m_xArray.getSize() - 1,m_yArray.getSize() - 2);

         DataStruct* leftOfTopPins     = getPinAtPinBucket(1                     ,m_yArray.getSize() - 1);
         DataStruct* rightOfTopPins    = getPinAtPinBucket(m_xArray.getSize() - 2,m_yArray.getSize() - 1);
         DataStruct* leftOfBottomPins  = getPinAtPinBucket(1                     ,0);
         DataStruct* rightOfBottomPins = getPinAtPinBucket(m_xArray.getSize() - 2,0);

         if (bottomOfLeftPins  != NULL && topOfLeftPins	!= NULL &&
             bottomOfRightPins != NULL && topOfRightPins != NULL     )
         {
            if (bottomOfLeftPins->getInsert()->getBlockNumber() == 
                topOfRightPins->getInsert()->getBlockNumber()     )
            {
               if (bottomOfLeftPins->getInsert()->getBlockNumber() != 
                   topOfLeftPins->getInsert()->getBlockNumber()     )
               {  // topOfLeftPins is different
                  retval = true;
                  xIndex = 0;
                  yIndex = m_yArray.getSize() - 2;

                  indexPin = getPinAtPinBucket(xIndex,yIndex);
                  rotationDegrees = 0;
               }
               else if (bottomOfLeftPins->getInsert()->getBlockNumber() != 
                        bottomOfRightPins->getInsert()->getBlockNumber()    )
               {  // bottomOfRightPins is different
                  retval = true;
                  xIndex = m_xArray.getSize() - 1;
                  yIndex = 1;

                  indexPin = getPinAtPinBucket(xIndex,yIndex);
                  rotationDegrees = 180;
               }
            }

            if (!retval &&
                leftOfTopPins->getInsert()->getBlockNumber() == 
                rightOfBottomPins->getInsert()->getBlockNumber()     )
            {
               if (leftOfTopPins->getInsert()->getBlockNumber() != 
                   rightOfTopPins->getInsert()->getBlockNumber()     )
               {  // rightOfTopPins is different
                  retval = true;
                  xIndex = m_xArray.getSize() - 2;
                  yIndex = m_yArray.getSize() - 1;

                  indexPin = getPinAtPinBucket(xIndex,yIndex);
                  rotationDegrees = 270;
               }
               else if (leftOfTopPins->getInsert()->getBlockNumber() != 
                        leftOfBottomPins->getInsert()->getBlockNumber()    )
               {  // leftOfBottomPins is different
                  retval = true;
                  xIndex = 1;
                  yIndex = 0;

                  indexPin = getPinAtPinBucket(xIndex,yIndex);
                  rotationDegrees = 90;
               }
            }
         }
      }

      break;
   case pcbComponentPackageTypeDip:
   case pcbComponentPackageTypeHeader:
   case pcbComponentPackageTypeSoic:
   case pcbComponentPackageTypeTsop:
      {
         DataStruct* topLeftPin     = getPinAtPinBucket(0                     ,m_yArray.getSize() - 1);
         DataStruct* bottomLeftPin  = getPinAtPinBucket(0                     ,0);
         DataStruct* topRightPin    = getPinAtPinBucket(m_xArray.getSize() - 1,m_yArray.getSize() - 1);
         DataStruct* bottomRightPin = getPinAtPinBucket(m_xArray.getSize() - 1,0);

         if (topLeftPin  != NULL && bottomLeftPin  != NULL &&
             topRightPin != NULL && bottomRightPin != NULL     )
         {
            bool verticalFlag = (m_xArray.getSize() == 2);

            if (verticalFlag)
            {
               if (bottomLeftPin->getInsert()->getBlockNumber() == 
                   topRightPin->getInsert()->getBlockNumber()     )
               {
                  if (bottomLeftPin->getInsert()->getBlockNumber() != 
                      topLeftPin->getInsert()->getBlockNumber()     )
                  {  // topLeftPin is different
                     retval = true;
                     xIndex = 0;
                     yIndex = m_yArray.getSize() - 1;

                     indexPin = getPinAtPinBucket(xIndex,yIndex);
                     rotationDegrees = 0;
                  }
                  else if (bottomLeftPin->getInsert()->getBlockNumber() != 
                           bottomRightPin->getInsert()->getBlockNumber()     )
                  {  // bottomRightPin is different
                     retval = true;
                     xIndex = m_xArray.getSize() - 1;
                     yIndex = 0;

                     indexPin = getPinAtPinBucket(xIndex,yIndex);
                     rotationDegrees = 180;
                  }
               }
            }
            else
            {
               if (topLeftPin->getInsert()->getBlockNumber() == 
                   bottomRightPin->getInsert()->getBlockNumber()     )
               {
                  if (topLeftPin->getInsert()->getBlockNumber() != 
                      bottomLeftPin->getInsert()->getBlockNumber()     )
                  {  // bottomLeftPin is different
                     retval = true;
                     xIndex = 0;
                     yIndex = 0;

                     indexPin = getPinAtPinBucket(xIndex,yIndex);
                     rotationDegrees = 90;
                  }
                  else if (topLeftPin->getInsert()->getBlockNumber() != 
                           topRightPin->getInsert()->getBlockNumber()     )
                  {  // topRightPin is different
                     retval = true;
                     xIndex = m_xArray.getSize() - 1;
                     yIndex = m_yArray.getSize() - 1;

                     indexPin = getPinAtPinBucket(xIndex,yIndex);
                     rotationDegrees = 270;
                  }
               }
            }
         }
      }

      break;
   case pcbComponentPackageTypeSot23:
      break;
   case pcbComponentPackageTypeTestPoint:
      break;
   case pcbComponentPackageTypeUndefined:
      break;
   }

   return retval;
}

DataStruct* CPcbComponentPinAnalyzer::setPinNumberingToDefault()
{
   DataStruct* pin1 = NULL;

   CString pinName;
   int pinNumber = 1;

   for (POSITION pos = m_mainPins.GetHeadPosition();pos != NULL;pinNumber++)
   {
      DataStruct* pin = m_mainPins.GetNext(pos);
      pinName.Format("%d",pinNumber);

      pin->getInsert()->setRefname(pinName);

      if (pinNumber == 1)
      {
         pin1 = pin;
      }
   }

   return pin1;
}

int CPcbComponentPinAnalyzer::getIpcRotation(DataStruct* pin1)
{
   int ipcRotationDegrees = 0;

   if (pin1 != NULL)
   {
      CPoint2d pin1Origin = pin1->getInsert()->getOrigin2d();

      if (m_componentCentroid != NULL)
      {
         pin1Origin = pin1Origin - m_componentCentroid->getInsert()->getOrigin2d();
      }

      if (m_mainPins.GetCount() == 2)
      {
         // For two pin components, pin1 should be placed on the -x axis.

         if (fpnear(pin1Origin.x,0.,m_tolerance))
         {  // pin is on y axis
            ipcRotationDegrees = ((pin1Origin.y <= 0.) ? 90 : 270);
         }
         else if (fpnear(pin1Origin.y,0.,m_tolerance))
         {  // pin is on x axis
            ipcRotationDegrees = ((pin1Origin.x > 0.) ? 180 : 0);
         }
      }
      else if (m_mainPins.GetCount() > 2)
      {
         // For multi pin components, pin1 should be placed in quadrant 2, or on the +y axis.

         if (fpnear(pin1Origin.x,0.,m_tolerance))
         {  // pin is on y axis
            ipcRotationDegrees = ((pin1Origin.y >= 0.) ? 0 : 180);
         }
         else if (fpnear(pin1Origin.y,0.,m_tolerance))
         {  // pin is on x axis
            ipcRotationDegrees = ((pin1Origin.x >= 0.) ? 270 : 90);
         }
         else if (pin1Origin.x > 0. && pin1Origin.y > 0.)
         {
            ipcRotationDegrees = 270;
         }
         else if (pin1Origin.x < 0. && pin1Origin.y > 0.)
         {
            ipcRotationDegrees = 0;
         }
         else if (pin1Origin.x < 0. && pin1Origin.y < 0.)
         {
            ipcRotationDegrees = 90;
         }
         else if (pin1Origin.x > 0. && pin1Origin.y < 0.)
         {
            ipcRotationDegrees = 180;
         }
      }
   }

   if (ipcRotationDegrees != 0)
   {
      int iii = 3;
   }

   return ipcRotationDegrees;
}

bool CPcbComponentPinAnalyzer::updateGeometry(CBasesVector& componentBasesVector,CTMatrix& oldToNewGeometryMatrix)
{
   bool retval = (m_componentGeometry != NULL);

   if (retval)
   {
      analyzeFootprint();

      int rotationDegrees = 0;
      int xIndex;
      int yIndex;

      if (m_componentCentroid != NULL)
      {
         rotationDegrees = round(normalizeDegrees(-m_componentCentroid->getInsert()->getAngleDegrees(),.001));
         rotationDegrees = (rotationDegrees / 90) * 90;
      }

      OutlineIndicatorOrientationTag outlineIndicatorOrientation = m_componentGeometry->getComponentOutlineIndicatorOrientation(m_camCadDatabase.getCamCadData());
      DataStruct* pin1 = NULL;
      BookReadingOrderTag bookReadingOrder = bookReadingUndefined;

      switch (m_pcbComponentPackageType)
      {
      case pcbComponentPackageTypeBga:
      case pcbComponentPackageTypePga:
         {
            outlineIndicatorOrientation = outlineIndicatorOrientationTopLeft;
            bookReadingOrder            = bookReadingLeftRightTopBottom;

            int xr = m_xArray.getSize() - 1;
            int yt = m_yArray.getSize() - 1;

            bool tlFlag = (getPinAtPinBucket( 0,yt) != NULL);
            bool blFlag = (getPinAtPinBucket( 0,0 ) != NULL);
            bool brFlag = (getPinAtPinBucket(xr,0 ) != NULL);
            bool trFlag = (getPinAtPinBucket(xr,yt) != NULL);

            int flagSum = tlFlag + blFlag + brFlag + trFlag;

            if (flagSum == 1)
            {
               if      (tlFlag) rotationDegrees =   0;
               else if (trFlag) rotationDegrees =  90;
               else if (brFlag) rotationDegrees = 180;
               else if (blFlag) rotationDegrees = 170;
            }
            else if (flagSum == 3)
            {
               if      (!tlFlag) rotationDegrees =   0;
               else if (!trFlag) rotationDegrees =  90;
               else if (!brFlag) rotationDegrees = 180;
               else if (!blFlag) rotationDegrees = 170;
            }
         }

         break;
      case pcbComponentPackageTypeChip:
      case pcbComponentPackageTypeJumper:
			{
				if (m_xArray.getSize() != 2)
				{  // vertically oriented, pins on y axis
					xIndex = 0;

               if (rotationDegrees == 270)
               {  // pin1 is at top
					   yIndex = m_yArray.getSize() - 1;
               }
               else
               {  // pin1 is at bottom
                  rotationDegrees = 90;
					   yIndex = 0;
               }
				}
				else
				{  // horizontally oriented, pins on x axis
					yIndex = 0;

               if (rotationDegrees == 180)
               {  // pin1 is at right
					   xIndex = m_xArray.getSize() - 1;
               }
               else
               {  // pin1 is at left
                  rotationDegrees = 0;
					   xIndex = 0;
               }
				}

				pin1 = getPinAtPinBucket(xIndex,yIndex);
				outlineIndicatorOrientation = outlineIndicatorOrientationUndefined;
			}

         break;
      case pcbComponentPackageTypeNoPins:
         break;
      case pcbComponentPackageTypePlcc:
      case pcbComponentPackageTypeQfp:
         {
            if (m_xArray.getSize() < m_yArray.getSize())
            {
               //      o o o o            o o o o  
               //    x         o        o         o
               //    o         o        o         o
               //    o         o        o         o
               //    o         o   or   o         o 
               //    o         o        o         o
               //    o         o        o         x
               //      o o o o            o o o o  

               if (rotationDegrees != 0 && rotationDegrees != 180)
               {
                  rotationDegrees = 0;
               }
            }
            else if (m_xArray.getSize() > m_yArray.getSize())
            {
               //      o o o o o o            o o o o o x  
               //    o             o        o             o
               //    o             o        o             o
               //    o             o   or   o             o 
               //    o             o        o             o
               //      x o o o o o            o o o o o o  

               if (rotationDegrees != 90 && rotationDegrees != 270)
               {
                  rotationDegrees = 90;
               }
            }

            switch (rotationDegrees)
            {
            case 0:
               xIndex = 0;
               yIndex = m_yArray.getSize() - 2;

               break;
            case 90:
               xIndex = 1;
               yIndex = 0;

               break;
            case 180:
               xIndex = m_xArray.getSize() - 1;
               yIndex = 1;

               break;
            case 270:
               xIndex = m_xArray.getSize() - 2;
               yIndex = m_yArray.getSize() - 1;

               break;
            }

            outlineIndicatorOrientation = outlineIndicatorOrientationTopLeft;

            //if (m_xArray.getSize() == m_yArray.getSize())
            //{
            //   xIndex = 0;
            //   yIndex = m_yArray.getSize() - 2;
            //   rotationDegrees = (fpeq(rotationDegrees,180.) ? 180. : 0.);
            //}
            //else if (m_xArray.getSize() <= m_yArray.getSize())
            //{
            //   xIndex = 0;
            //   yIndex = m_yArray.getSize() - 2;
            //   rotationDegrees = (fpeq(rotationDegrees,180.) ? 180. : 0.);
            //}
            //else
            //{
            //   xIndex = 1;
            //   yIndex = 0;
            //   rotationDegrees = (fpeq(rotationDegrees,90.) ? 90. : 270.);
            //}

            pin1 = getPinAtPinBucket(xIndex,yIndex);
            getIndexPinAndRotation(pin1,rotationDegrees);

            makeExtraPinsFiducials();
         }

         break;
      case pcbComponentPackageTypeDip:
      case pcbComponentPackageTypeHeader:
      case pcbComponentPackageTypeSoic:
      case pcbComponentPackageTypeTsop:
         {
            bool verticalFlag = (m_xArray.getSize() == 2);
            outlineIndicatorOrientation = outlineIndicatorOrientationTop;

            if (verticalFlag)
            {
               //  |1  6|
               //  |2  5|
               //  |3  4|
               //yIndex = m_yArray.getSize() - 1;
               //rotationDegrees = (fpeq(rotationDegrees,180.) ? 180. : 0.);

               if (rotationDegrees != 180)
               {
                  rotationDegrees = 0;
                  xIndex = 0;
                  yIndex = m_yArray.getSize() - 1;
               }
               else
               {
                  xIndex = 1;
                  yIndex = 0;
               }
            }
            else
            {
               //  |6 5 4|
               //  |1 2 3|
               //yIndex = 0;
               //rotationDegrees = (fpeq(rotationDegrees,90.) ? 90. : 270.);

               if (rotationDegrees != 270)
               {
                  rotationDegrees = 90;
                  xIndex = 0;
                  yIndex = 0;
               }
               else
               {
                  xIndex = m_xArray.getSize() - 1;
                  yIndex = 1;
               }
            }

            pin1 = getPinAtPinBucket(xIndex,yIndex);
            getIndexPinAndRotation(pin1,rotationDegrees);

            makeExtraPinsFiducials();
         }

         break;
      case pcbComponentPackageTypeCrystal:
      case pcbComponentPackageTypeSot23:
         {
            //outlineIndicatorOrientation = outlineIndicatorOrientationTop;

            bool verticalFlag = (m_xArray.getSize() == 2);

            if (verticalFlag)
            {
               //  |1   |
               //  |   2|
               //  |3   |
               pin1 = getPinAtPinBucket(0,2);
               rotationDegrees = 0;

               if (pin1 == NULL)
               {
                  //  |   3|
                  //  |2   |
                  //  |   1|
                  pin1 = getPinAtPinBucket(1,0);
                  rotationDegrees = 180;
               }
            }
            else
            {
               //  |  2  |
               //  |1   3|
               pin1 = getPinAtPinBucket(0,0);
               rotationDegrees = 90;

               if (pin1 == NULL)
               {
                  //  |3   1|
                  //  |  2  |
                  pin1 = getPinAtPinBucket(2,1);
                  rotationDegrees = 270;
               }
            }
         }

         break;
      case pcbComponentPackageTypeTestPoint:
         break;
      case pcbComponentPackageTypeMisc:
      case pcbComponentPackageTypeUndefined:
         {
            m_mainPins.takeData(m_extraPins);
            DataStruct* firstPin = setPinNumberingToDefault();
            rotationDegrees = getIpcRotation(firstPin);
         }

         break;
      }

      m_ipcStatus = IPCStatusStandardIPC;

      if (pin1 != NULL)
      {
		   CPolarCoordinatePinArray polarPinArray(m_camCadDatabase,m_componentGeometry->getDataList());
		   polarPinArray.analysizePinNumbering(*pin1,false);
         m_pinNumberingMethod = pinLabelingMethodNumeric;
         m_pinOrderingMethod  = pinOrderingMethodCounterClockwise;
      }
      else if (bookReadingOrder != bookReadingUndefined)
      {
         pin1 = analysizePinNumbering(bookReadingOrder,pinLabelingMethodAlphaNumeric,"IOQS");
         m_pinOrderingMethod  = pinOrderingMethodReadingOrder;
      }
      //else
      //{
      //   m_ipcStatus = IPCStatusNonStandardPart;
      //   pin1 = setPinNumberingToDefault();
      //}

      //int ipcRotationDegrees = getIpcRotation(pin1);
      //rotationDegrees += ipcRotationDegrees;

      CExtent pinExtent = m_mainPins.getPinExtent();

      CTMatrix matrix;
      matrix.translate(-pinExtent.getCenter());
      matrix.rotateDegrees(-rotationDegrees);

      if (m_componentCentroid != NULL)
      {
         CPoint2d componentCentroidOrigin = m_componentCentroid->getInsert()->getOrigin2d();
         matrix.transform(componentCentroidOrigin);
         CPoint2d origin;

         if (! componentCentroidOrigin.fpeq(origin,m_tolerance))
         {
            matrix.translate(-componentCentroidOrigin);
         }
      }

      m_dataList.transform(matrix);
      oldToNewGeometryMatrix = matrix;

      matrix.invert();
      componentBasesVector.set();
      componentBasesVector.transform(matrix);

      m_componentGeometry->setComponentOutlineIndicatorOrientation(m_camCadDatabase.getCamCadData(),outlineIndicatorOrientation);
      m_componentGeometry->generateDefaultComponentOutline(m_camCadDatabase.getCamCadData());

      if (m_componentCentroid != NULL)
      {
         m_componentCentroid->getInsert()->setAngleRadians(0.);
      }
   }

   return retval;
}


