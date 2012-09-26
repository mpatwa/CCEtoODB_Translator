// $Header: /CAMCAD/DcaLib/DcaData.cpp 10    6/30/07 2:57a Kurt Van Ness $

#include "Stdafx.h"
#include "DcaData.h"
//#include "DcaBlock.h"
#include "DcaCamCadData.h"
#include "DcaBasesVector.h"
//#include "DcaPolyLib.h"
//#include "DcaArcPoint.h"
//#include "DcaMeasure.h"
#include "DcaWriteFormat.h"
//#include "DcaPolygon.h"
//#include "DcaFont.h"
#include "DcaLib.h"
#include "DcaEntityNumber.h"
#include "DcaDataType.h"
#include "DcaAttributes.h"
#include "DcaGraphicClass.h"
#include "DcaInsert.h"
#include "DcaPoly.h"
#include "DcaText.h"
#include "DcaBlob.h"
#include "DcaDataType.h"
#include "DcaDataListIterator.h"
#include "DcaTMatrix.h"
#include "DcaQfe.h"

#if defined(EnableDcaCamCadDocLegacyCode)
//_____________________________________________________________________________
ExtentRect::ExtentRect()
{  
   reset();
}

ExtentRect& ExtentRect::operator=(const CExtent& extent)
{
   left   = extent.getXmin();
   bottom = extent.getYmin();
   right  = extent.getXmax();
   top    = extent.getYmax();

   return *this;
}

void ExtentRect::reset()
{  
   left   =  DBL_MAX;
   bottom =  DBL_MAX;
   right  = -DBL_MAX;
   top    = -DBL_MAX;
}

bool ExtentRect::isCorrupt()
{  
   bool retval = false;
   
   if (!(_finite(left) && _finite(right) && _finite(bottom) && _finite(top)))
   {
      retval = true;
   }

   return retval;
}

bool ExtentRect::isValid()
{  
   bool retval = !isCorrupt();

   if (retval)
   {
      retval = (left <= right) && (bottom <= top);
   }

   return retval;
}
#endif

//_____________________________________________________________________________
//DataStruct::DataStruct(const DataStruct& other)
//: m_dataType(dataTypeUndefined)
//, m_entityNumber(-1)
//{
//   init();
//}

DataStruct::DataStruct(DataTypeTag dataType,int entityNumber)
: m_dataType(dataType)
, m_entityNumber(entityNumber)
{
   init();
}

DataStruct::DataStruct(const DataStruct& other)
{
   formatMessageBox("Error, DataStruct copy contructor called");
}

DataStruct::DataStruct(const DataStruct& other,int entityNumber,bool copyAttributesFlag)
: m_entityNumber(entityNumber)
{
   m_dataType        = other.m_dataType;
   m_selected        = other.m_selected;
   m_marked          = other.m_marked;
   m_negative        = other.m_negative;
   m_colorOverride   = other.m_colorOverride;
   m_overrideColor   = other.m_overrideColor;
   m_highlightColor  = other.m_highlightColor;
   m_graphicClass    = other.m_graphicClass;
   m_hide            = other.m_hide;
   m_layerIndex      = other.m_layerIndex;
   m_flagBits        = other.m_flagBits;
   m_physLyrStart    = other.m_physLyrStart;
   m_physLyrEnd      = other.m_physLyrEnd;

   m_polylist        = NULL;

   switch (m_dataType)
   {
   case dataTypeInsert:
      m_insert   = new InsertStruct(*(other.m_insert));
      break;
   case dataTypePoly:
      m_polylist = new CPolyList(*(other.m_polylist));
      break;
   case dataTypeText:
      m_text     = new TextStruct(*(other.m_text));
      break;
   case dataTypePoint:
      m_point    = new PointStruct(*(other.m_point));
      break;
   case dataTypeBlob:
      m_blob     = new BlobStruct(*(other.m_blob));
      break;
   case dataTypePoly3D:
      m_poly3dlist  = new CPoly3DList(*(other.m_poly3dlist));
      break;
   }

   if (other.m_attributes == NULL || !copyAttributesFlag)
   {
      m_attributes = NULL;
   }
   else
   {
      m_attributes = new CAttributes();
      CAttributeMapWrap attributeMap(m_attributes);
      attributeMap.copyFrom(other.m_attributes);
   }
}

void DataStruct::init()
{
   m_selected        = 0;
   m_marked          = 0;
   m_negative        = 0;
   m_colorOverride   = 0;
   m_overrideColor   = 0;
   m_highlightColor  = 0;
   m_graphicClass    = graphicClassNormal;
   m_hide            = 0;
   m_layerIndex      = 0;
   m_flagBits        = 0;

   m_polylist        = NULL;
   m_attributes      = NULL;

   m_physLyrStart = -1;
   m_physLyrEnd = -1;

   switch (m_dataType)
   {
   case dataTypeInsert:
      m_insert   = new InsertStruct();
      break;
   case dataTypePoly:
      m_polylist = new CPolyList();
      break;
   case dataTypeText:
      m_text     = new TextStruct();
      break;
   case dataTypePoint:
      m_point    = new PointStruct();
      break;
   case dataTypeBlob:
      m_blob     = new BlobStruct();
      break;
   case dataTypePoly3D:
      m_poly3dlist = new CPoly3DList();
      break;
   }
}

DataStruct& DataStruct::operator=(const DataStruct& other)
{
   if (&other != this)
   {
      deallocate();

      // keep existing value of entity
      m_dataType        = other.m_dataType;
      m_selected        = other.m_selected;
      m_marked          = other.m_marked;
      m_negative        = other.m_negative;
      m_colorOverride   = other.m_colorOverride;
      m_overrideColor   = other.m_overrideColor;
      m_highlightColor  = other.m_highlightColor;
      m_graphicClass    = other.m_graphicClass;
      m_hide            = other.m_hide;
      m_layerIndex      = other.m_layerIndex;
      m_flagBits        = other.m_flagBits;
      m_physLyrStart    = other.m_physLyrStart;
      m_physLyrEnd      = other.m_physLyrEnd;

      switch (m_dataType)
      {
      case dataTypeInsert:
         m_insert   = new InsertStruct(*(other.m_insert));
         break;
      case dataTypePoly:
         m_polylist = new CPolyList(*(other.m_polylist));
         break;
      case dataTypeText:
         m_text     = new TextStruct(*(other.m_text));
         break;
      case dataTypePoint:
         m_point    = new PointStruct(*(other.m_point));
         break;
      case dataTypeBlob:
         m_blob     = new BlobStruct(*(other.m_blob));
         break;
      case dataTypePoly3D:
         m_poly3dlist = new CPoly3DList(*(other.m_poly3dlist));
         break;
     }

      if (other.m_attributes != NULL)
      {
         m_attributes = new CAttributes();
         CAttributeMapWrap attributeMap(m_attributes);
         attributeMap.copyFrom(other.m_attributes);
      }
   }

   return *this;
}

DataStruct::~DataStruct()
{
#ifdef EnableDataStructTracker
   if (getDataStructTracker().contains(this))
   {
      formatMessageBox("%s.%d - delete of referenced DataStruct %p",__FILE__,__LINE__,this);
   }
#endif

   deallocate();
}

void DataStruct::deallocate()
{
   switch (m_dataType)
   {
   case dataTypeText:
      delete m_text;
      m_text = NULL;
      break;

   case dataTypeInsert:
      delete m_insert;
      m_insert = NULL;
      break;

   case dataTypePoly:
      delete m_polylist;
      m_polylist = NULL;
      break;

   case dataTypePoint:
      delete m_point;
      m_point = NULL;
      break;

   case dataTypeBlob:   
      delete m_blob;
      m_blob = NULL;
      break;

   case dataTypePoly3D:
      delete m_poly3dlist;
      m_poly3dlist = NULL;
      break;
   }

   delete m_attributes;
   m_attributes = NULL;
}

long DataStruct::getEntityNumber() const
{
   return m_entityNumber;
}

DataTypeTag DataStruct::getDataType() const
{
   return (DataTypeTag)m_dataType;
}

bool DataStruct::isSelected() const
{
   return (m_selected != 0);
}

void DataStruct::setSelected(bool flag)
{
   m_selected = flag;
}

void DataStruct::setSelected(BOOL flag)
{
   m_selected = flag;
}

bool DataStruct::isMarked() const
{
   return (m_marked != 0);
}

void DataStruct::setMarked(bool flag)
{
   m_marked = flag;
}

void DataStruct::setMarked(BOOL flag)
{
   m_marked = flag;
}

bool DataStruct::isNegative() const
{
   return (m_negative != 0);
}

void DataStruct::setNegative(bool flag)
{
   m_negative = flag;
}

void DataStruct::setNegative(BOOL flag)
{
   m_negative = flag;
}

bool DataStruct::isHidden() const
{
   return (m_hide != 0);
}

void DataStruct::setHidden(bool flag)
{
   m_hide = flag;
}

void DataStruct::setHidden(BOOL flag)
{
   m_hide = flag;
}

bool DataStruct::getColorOverride() const
{
   return (m_colorOverride != 0);
}

void DataStruct::setColorOverride(bool flag)
{
   m_colorOverride = flag;
}

void DataStruct::setColorOverride(BOOL flag)
{
   m_colorOverride = flag;
}

COLORREF DataStruct::getOverrideColor() const
{
   return m_overrideColor;
}

void DataStruct::setOverrideColor(COLORREF color)
{
   m_overrideColor = color;
}

bool DataStruct::hasHighlightColor() const
{
   return m_highlightColor > 0;
}

COLORREF DataStruct::getHighlightColor() const
{
   return m_highlightColor;
}

void DataStruct::setHighlightColor(COLORREF color)
{
   m_highlightColor = color;
}

GraphicClassTag DataStruct::getGraphicClass() const
{
   return m_graphicClass;
}

void DataStruct::setGraphicClass(GraphicClassTag graphicClass)
{
   m_graphicClass = graphicClass;
}

short DataStruct::getLayerIndex() const
{
   return m_layerIndex;
}

void DataStruct::setLayerIndex(short layerIndex)
{
   m_layerIndex = layerIndex;
}

DbFlag DataStruct::getFlags() const
{
   return m_flagBits;
}

void DataStruct::setFlags(DbFlag blockFlag)
{
   m_flagBits = blockFlag;
}

void DataStruct::setFlagBits(DbFlag mask)
{
   m_flagBits |= mask;
}

void DataStruct::clearFlagBits(DbFlag mask)
{
   m_flagBits &= ~mask;
}

CAttributes* DataStruct::getAttributes() const
{
   return m_attributes;
}

CAttributes*& DataStruct::getAttributesRef()
{
   return m_attributes;
}

CAttributes*& DataStruct::getAttributeMap()
{
   return m_attributes;
}

CAttributes*& DataStruct::getDefinedAttributeMap()
{
   return getDefinedAttributes();
}

CPolyList*& DataStruct::getPolyList()
{
   return m_polylist;
}

TextStruct*& DataStruct::getText()
{
   return m_text;
}

InsertStruct*& DataStruct::getInsert()
{
   return m_insert;
}

PointStruct*& DataStruct::getPoint()
{
   return m_point;
}

BlobStruct*& DataStruct::getBlob()
{
   return m_blob;
}

CPoly3DList*& DataStruct::getPoly3DList()
{
   return m_poly3dlist;
}

const CPolyList* DataStruct::getPolyList() const
{
   return m_polylist;
}

const TextStruct* DataStruct::getText() const
{
   return m_text;
}

const InsertStruct* DataStruct::getInsert() const
{
   return m_insert;
}

const PointStruct* DataStruct::getPoint() const
{
   return m_point;
}

const BlobStruct* DataStruct::getBlob() const
{
   return m_blob;
}

const CPoly3DList* DataStruct::getPoly3DList() const
{
   return m_poly3dlist;
}

void DataStruct::setGraphicClass(int graphicClass)          
{ 
   m_graphicClass = ((graphicClass >= graphicClassLowerBound && graphicClass <= graphicClassUpperBound) ? 
                        (GraphicClassTag)graphicClass : graphicClassUndefined); 
}

CAttributes*& DataStruct::getDefinedAttributes()
{
   if (m_attributes == NULL)
   {
      m_attributes = new CAttributes();
   }

   return m_attributes;
}

CAttributes& DataStruct::attributes()
{
   if (m_attributes == NULL)
   {
      m_attributes = new CAttributes();
   }

   return *m_attributes;
}

void DataStruct::transform(const CTMatrix& transformationMatrix,CCamCadData* camCadData,bool doAttributes)
{
   switch (m_dataType)
   {
   case dataTypeInsert:
      m_insert->transform(transformationMatrix);
      break;
   case dataTypePoly:
      m_polylist->transform(transformationMatrix,camCadData);
      break;
   case dataTypeText:
      m_text->transform(transformationMatrix);
      break;
   case dataTypePoint:
      m_point->transform(transformationMatrix);
      break;
   case dataTypeBlob:
      m_blob->transform(transformationMatrix);
      break;
    case dataTypePoly3D:
      m_poly3dlist->transform(transformationMatrix,camCadData);
      break;
  }

   if (m_attributes != NULL && doAttributes)
   {
      m_attributes->transform(transformationMatrix);
   }
}

bool DataStruct::isVisible(CCamCadData& camCadData,LayerStruct* parentsLayer,bool mirrorLayerFlag,bool filterLayersFlag)
{
   bool retval = true;

   LayerStruct* layer = camCadData.getLayer(getLayerIndex());

   if (layer == NULL || (layer->getFloating() && parentsLayer != NULL))
   {
      layer = parentsLayer;
   }

   bool layerVisibleByMirrorAttributeFlag = true;
   LayerStruct* pairedLayer = layer;

   if (layer != NULL)
   {
      if (mirrorLayerFlag)
      {
         pairedLayer = camCadData.getLayer(layer->getMirroredLayerIndex());

         if (pairedLayer->getNeverMirror())
         {
            layerVisibleByMirrorAttributeFlag = false;
         }
      }
      else  if (pairedLayer->getMirrorOnly())
      {
         layerVisibleByMirrorAttributeFlag = false;
      }

      if (filterLayersFlag)
      {
         retval = camCadData.getSelectLayerFilterStack().contains(mirrorLayerFlag ? layer->getMirroredLayerIndex() : layer->getLayerIndex());
      }
   }

   while (retval)
   {
      retval = false;

      if (getDataType() == dataTypeInsert)
      {
         if (! camCadData.isInsertTypeVisible(getInsert()->getInsertType()))
         {
            break;
         }

         BlockStruct* subBlock = camCadData.getBlock(getInsert()->getBlockNumber());

         if (subBlock == NULL)
         {
            break;
         }

         bool isTool     = ((subBlock->getFlags() & (BL_TOOL     | BL_BLOCK_TOOL    )) != 0);
         bool isAperture = ((subBlock->getFlags() & (BL_APERTURE | BL_BLOCK_APERTURE)) != 0);

         if (isTool || (isAperture && subBlock->getShape() != apertureComplex))
         {
            retval =  (layerVisibleByMirrorAttributeFlag && pairedLayer != NULL && pairedLayer->getVisible());
            break;
         }
         else if (isAperture && subBlock->getShape() == apertureComplex)
         {
            subBlock = camCadData.getBlock(subBlock->getComplexApertureSubBlockNumber());

            if (subBlock == NULL)
            {
               break;
            }
         }

         retval = false;

         for (POSITION pos = subBlock->getHeadDataPosition();pos != NULL && !retval;)
         {
            DataStruct* data = subBlock->getNextData(pos);
            bool insertLayerMirrored = getInsert()->getLayerMirrored();

            retval = data->isVisible(camCadData,layer,mirrorLayerFlag != insertLayerMirrored,filterLayersFlag);
         }
      }
      else
      {
         if (camCadData.isGraphicClassVisible(getGraphicClass()))
         {
            if (pairedLayer != NULL)
            {
               retval = layerVisibleByMirrorAttributeFlag && pairedLayer->getVisible();
            }
            else if (parentsLayer != NULL)
            {
               retval = parentsLayer->getVisible();
            }
         }
      }

      break;
   }

   return retval;
}

bool DataStruct::isSelectable(CCamCadData& camCadData,LayerStruct* parentsLayer,bool mirrorLayerFlag,bool filterLayersFlag)
{
   bool retval = isVisible(camCadData,parentsLayer,mirrorLayerFlag,filterLayersFlag);

   return retval;
}

bool DataStruct::setAttrib(CCamCadData& camCadData, int keywordIndex, ValueTypeTag valueType, void *value, AttributeUpdateMethodTag updateMethod, CAttribute** attribPtr)
{
   return (camCadData.setAttribute(attributes(), keywordIndex, valueType, value, updateMethod, attribPtr) ?  true : false); 
}

bool DataStruct::lookUpAttrib(WORD keyword, CAttribute *&attribute)
{
   if (m_attributes == NULL)
      return false;

   return m_attributes->Lookup(keyword, attribute)?true:false;
}

void DataStruct::removeAttrib(WORD keywordIndex)
{
   if (m_attributes != NULL)
   {
      m_attributes->deleteAttribute(keywordIndex);
   }
}

CString DataStruct::getProbeStyle(CCamCadData& camCadData)
{
   CString retval;

   if (getDataType() == dataTypeInsert && getInsert()->getInsertType() == insertTypeTestProbe)
   {
      int probeStyleKw = camCadData.getAttributeKeywordIndex(standardAttributeProbeStyle);
      CAttribute* attrib = NULL;

      if (m_attributes != NULL && m_attributes->Lookup(probeStyleKw, attrib))
      {
         retval = camCadData.getAttributeStringValue(*attrib);
      }

      if (retval.Trim().IsEmpty())
      {
         CString probeBlockName = camCadData.getBlock(m_insert->getBlockNumber())->getName();
         
         int indx = probeBlockName.Find("_Bot");

         if (indx < 0)
            indx = probeBlockName.Find("_Top");

         if (indx > -1)
            retval = probeBlockName.Left(indx);
         else
            retval = probeBlockName;
      }
   }

   return retval;
}

DbFlag DataStruct::getFlag() const
{
   return m_flagBits;
}

bool DataStruct::getMarked() const
{
   return (m_marked != 0);
}

bool DataStruct::getHidden() const
{
   return (m_hide != 0);
}

bool DataStruct::isInsert() const
{
   return (m_dataType == dataTypeInsert);
}

bool DataStruct::isInsertType(InsertTypeTag tag) const
{
   return (m_dataType == dataTypeInsert && m_insert != NULL && m_insert->getInsertType() == tag);
}

bool DataStruct::isValid() const
{
   bool retval = true;

   switch (m_dataType)
   {
   case dataTypeInsert:
      if (! m_insert->isValid())
      {
         retval = false;
      }
      break;
   case dataTypePoly:
      break;
   case dataTypeText:
      break;
   case dataTypePoint:
      break;
   case dataTypeBlob:
      break;
   case dataTypePoly3D:
      break;
   default:
      retval = false;
   }

   if (m_entityNumber < 0/* || m_entityNumber > CEntityNumber::getCurrent()*/)
   {
      retval = false;
   }

   return retval;
}

void DataStruct::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"DataStruct\n"
"{\n"
"   entity=%d\n"
"   type=%d (%s)\n"
"   graphic_class=%d (%s)\n"
"   layer=%d\n"
"   DbFlag=0x%04x\n",
m_entityNumber,
m_dataType,(const char*)dataStructTypeToString(m_dataType),
m_graphicClass,(const char*)graphicClassToString(m_graphicClass),
m_layerIndex,
m_flagBits);

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      switch(m_dataType)
      {
      case dataTypeDraw:
         break;
      case dataTypeInsert:
         m_insert->dump(writeFormat,depth);
         break;
      case dataTypePoly:
         m_polylist->dump(writeFormat,depth);
         break;
      case dataTypeText:
         m_text->dump(writeFormat,depth);
         break;
      case dataTypePoint:
         m_point->dump(writeFormat,depth);
         break;
      case dataTypeBlob:
         m_blob->dump(writeFormat,depth);
         break;
      case dataTypePoly3D:
         m_poly3dlist->dump(writeFormat,depth);
         break;
      }

      writeFormat.popHeader();
   }

   writeFormat.writef("}\n");
}

//_____________________________________________________________________________
CDataList::CDataList(bool isContainer)
: CTypedPtrListContainer<DataStruct*>(isContainer)
{
}

CDataList::CDataList(const CDataList& other)
: CTypedPtrListContainer<DataStruct*>(true)
{
   formatMessageBox("Error, CDataList copy constructor called.");
}

CDataList::CDataList(const CDataList& other,CCamCadData& camCadData)
: CTypedPtrListContainer<DataStruct*>(true)
{
   this->setEqual(other,camCadData);
}

CDataList::~CDataList()
{
   empty();
}

void CDataList::setEqual(const CDataList& other,CCamCadData& camCadData)
{
   if (&other != this)
   {
      empty();

      for (POSITION pos = other.GetHeadPosition();pos != NULL;)
      {
         DataStruct* otherData = other.GetNext(pos);

         if (otherData != NULL)
         {
            if (isContainer())
            {
               AddTail(camCadData.getNewDataStruct(*otherData));
            }
            else
            {
               AddTail(otherData);
            }
         }
      }
   }
}

//void CDataList::copyData(const CDataList& other,CCamCadData& camCadData)
//{
//   if (&other != this)
//   {
//      empty();
//
//      for (POSITION pos = other.GetHeadPosition();pos != NULL;)
//      {
//         DataStruct* otherData = other.GetNext(pos);
//
//         if (otherData != NULL)
//         {
//            if (isContainer())
//            {
//               AddTail(camCadData.getNewDataStruct(*otherData));
//            }
//            else
//            {
//               AddTail(otherData);
//            }
//         }
//      }
//   }
//}

//CDataList& CDataList::operator=(const CDataList& other)
//{
//   if (&other != this)
//   {
//      empty();
//
//      for (POSITION pos = other.GetHeadPosition();pos != NULL;)
//      {
//         DataStruct* otherData = other.GetNext(pos);
//
//         if (otherData != NULL)
//         {
//            if (isContainer())
//            {
//               AddTail(new DataStruct(*otherData));
//            }
//            else
//            {
//               AddTail(otherData);
//            }
//         }
//      }
//   }
//
//   return *this;
//}

void CDataList::rotateList(int rotationCount)
{
   if (GetCount() > 0)
   {
      while (rotationCount > 0)
      {
         AddTail(RemoveHead());
         rotationCount--;
      }

      while (rotationCount < 0)
      {
         AddHead(RemoveTail());
         rotationCount++;
      }
   }
}

int sortByEntityNumberCompare(const void* elem1,const void* elem2)
{
   const DataStruct *p1,*p2;
      
   p1 = *((DataStruct**)elem1);
   p2 = *((DataStruct**)elem2);
      
   int retval = p1->getEntityNumber() - p2->getEntityNumber();
      
   return retval;
}

void CDataList::sortByAscendingEntityNumber()
{
   CTypedPtrArray<CPtrArray,DataStruct*> sortingArray;
   sortingArray.SetSize(0,GetCount());

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = GetNext(pos);
      sortingArray.Add(data);
   }

   void** arrayData = sortingArray.GetData();
   qsort(arrayData,sortingArray.GetSize(),sizeof(DataStruct*),sortByEntityNumberCompare);

   RemoveAll();

   for (int index = 0;index < sortingArray.GetSize();index++)
   {
      DataStruct* data = sortingArray.GetAt(index);
      AddTail(data);
   }
}

void CDataList::takeData(CDataList& other)
{
   if (isContainer() == other.isContainer())
   {
      for (POSITION pos = other.GetHeadPosition();pos != NULL;)
      {
         POSITION oldPos = pos;
         DataStruct* otherData = other.GetNext(pos);

         if (otherData != NULL)
         {
            AddTail(otherData);
         }

         other.RemoveAt(oldPos);
      }
   }
}

DataStruct* CDataList::FindInsertData(CString refname, InsertTypeTag insertType)
{
   for (POSITION pos = this->GetHeadPosition();pos != NULL;)
   {
      DataStruct *data = this->GetNext(pos);

      if (data->getDataType() == dataTypeInsert && data->getInsert() != NULL)
      {
         if (data->getInsert()->getInsertType() == insertType && data->getInsert()->getRefname().CompareNoCase(refname) == 0)
         {
            return data;
         }
      }
   }

   return NULL;
}

CPoint2d CDataList::getPinCentroid() const
{
   CPoint2d centroid = getCentroid(insertTypePin);

   return centroid;
}

CPoint2d CDataList::getCentroid(InsertTypeTag insertType) const
{
   return getCentroid(CInsertTypeMask(insertType));
}

CPoint2d CDataList::getCentroid(const CInsertTypeMask& insertTypeMask) const
{
   CPoint2d centroid(0.,0.);
   int insertCount = 0;

   for (CDataListIterator insertIterator(*this,insertTypeMask);insertIterator.hasNext();)
   {
      DataStruct* data = insertIterator.getNext();

      insertCount++;
      centroid = centroid + data->getInsert()->getOrigin2d();
   }

   if (insertCount > 0)
   {
      centroid.x /= insertCount;
      centroid.y /= insertCount;
   }

   return centroid;
}

CExtent CDataList::getPinExtent() const
{
   CExtent extent = getExtent(insertTypePin);

   return extent;
}

CExtent CDataList::getExtent(InsertTypeTag insertType) const
{
   CExtent extent;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = GetNext(pos);

      if (data->getDataType() == dataTypeInsert)
      {
         InsertStruct* insert = data->getInsert();

         if (insertType == insertTypeUndefined || insert->getInsertType() == insertType)
         {
            extent.update(insert->getOrigin2d());
         }
      }
   }

   return extent;
}

CExtent CDataList::getExtent(CCamCadData* camCadData) const
{
   CExtent extent;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = GetNext(pos);

      if (data->getDataType() == dataTypeInsert)
      {
         InsertStruct* insert = data->getInsert();

         extent.update(insert->getOrigin2d());  // weak, just using insert point instead of extent of what is inserted
      }
      else if (data->getDataType() == dataTypePoly)
      {
         CPolyList* polyList = data->getPolyList();

         if (camCadData != NULL)
         {
            extent.update(polyList->getExtent(*camCadData));
         }
         else
         {
            extent.update(polyList->getExtent());
         }
      }
      else if (data->getDataType() == dataTypeText)
      {
         TextStruct *txt = data->getText();
         double penWidth = 0.0;
         double spaceRatio = 0.0;
         if (camCadData != NULL)
         {
            penWidth = camCadData->getWidth( txt->getPenWidthIndex() );
            spaceRatio = camCadData->getCamCadDataSettings().getTextSpacingRatio();
         }

         extent.update( txt->getExtent(spaceRatio, penWidth) );
      }
   }

   return extent;
}

CExtent CDataList::getExtent(const CTMatrix& matrix) const
{
   CExtent extent;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = GetNext(pos);

      if (data->getDataType() == dataTypeInsert)
      {
         InsertStruct* insert = data->getInsert();
         CPoint2d origin = insert->getOrigin2d();
         matrix.transform(origin);

         extent.update(origin);
      }
      else if (data->getDataType() == dataTypePoly)
      {
         CPolyList* polyList = data->getPolyList();

         extent.update(polyList->getExtent(matrix));
      }
   }

   return extent;
}

// x Axis is major axis
// y Axis is minor asis
CBasesVector CDataList::getPinMajorMinorAxes() const
{
   CBasesVector pinMajorMinorAxes;

   CExtent pinExtent = getPinExtent();
   CPoint2d extentOrigin = pinExtent.getCenter();
   CTMatrix matrix;
   matrix.translate(-extentOrigin);

   CTypedQfeTree<InsertStruct> pinInsertTree;
   CTypedQfeList<InsertStruct> pinList;

   for (CDataListIterator pinIterator(*this,insertTypePin);pinIterator.hasNext();)
   {
      DataStruct* pinData = pinIterator.getNext();
      InsertStruct* pin = pinData->getInsert();

      CPoint2d pinOrigin(pin->getOrigin2d());
      matrix.transform(pinOrigin);
      CTypedQfe<InsertStruct>* qfeInsert = new CTypedQfe<InsertStruct>(pinOrigin,pin);

      pinInsertTree.setAt(qfeInsert);
      pinList.AddTail(qfeInsert);
   }

   int pinCount = pinList.GetCount();
   int horizontalSymmetryCount = 0;
   int verticalSymmetryCount   = 0;
   int quadrant1Count          = 0;
   int quadrant2Count          = 0;
   int quadrant3Count          = 0;
   int quadrant4Count          = 0;

   if (pinCount > 1)
   {
      const double searchTolerance = .001;

      for (POSITION pinPos = pinList.GetHeadPosition();pinPos != NULL;)
      {
         CTypedQfe<InsertStruct>* qfeInsert = pinList.GetNext(pinPos);
         CPoint2d pinOrigin = qfeInsert->getOrigin();
         CTypedQfeList<InsertStruct> foundList;

         if (!fpeq(pinOrigin.x,0.))
         {
            CPoint2d symmetricPinOrigin(-pinOrigin.x,pinOrigin.y);
            CExtent searchExtent(symmetricPinOrigin,searchTolerance);

            pinInsertTree.search(searchExtent,foundList);

            if (foundList.GetCount() > 0)
            {
               horizontalSymmetryCount++;
            }
         }

         if (!fpeq(pinOrigin.y,0.))
         {
            CPoint2d symmetricPinOrigin(pinOrigin.x,-pinOrigin.y);
            CExtent searchExtent(symmetricPinOrigin,searchTolerance);

            pinInsertTree.search(searchExtent,foundList);

            if (foundList.GetCount() > 0)
            {
               verticalSymmetryCount++;
            }
         }

         if (pinOrigin.x > 0)
         {
            if (pinOrigin.y > 0)
            {
               quadrant1Count++;
            }
            else if (pinOrigin.y < 0)
            {
               quadrant4Count++;
            }
         }
         else if (pinOrigin.x < 0)
         {
            if (pinOrigin.y > 0)
            {
               quadrant2Count++;
            }
            else if (pinOrigin.y < 0)
            {
               quadrant3Count++;
            }
         }
      }

      if (horizontalSymmetryCount > verticalSymmetryCount)
      {
         pinMajorMinorAxes.initRotationMirror(90.,(quadrant1Count + quadrant4Count) > (quadrant2Count + quadrant3Count));
      }
      else if (verticalSymmetryCount > horizontalSymmetryCount)
      {
         pinMajorMinorAxes.initRotationMirror( 0.,(quadrant3Count + quadrant4Count) > (quadrant1Count + quadrant2Count));
      }
   }

   return pinMajorMinorAxes;
}

// only considers pin origin for symmetry
int CDataList::getPinSymmetryCount(double searchTolerance) const
{
   return getSymmetryCount(insertTypePin,symmetryCriteriaOrigin,searchTolerance);
}

int CDataList::getPinSymmetryCount(SymmetryCriteriaTag symmetryCriteria,double searchTolerance) const
{
   return getSymmetryCount(insertTypePin,symmetryCriteria,searchTolerance); // temp for now - knv
}

int CDataList::getSymmetryCount(InsertTypeTag insertType,SymmetryCriteriaTag symmetryCriteria,double searchTolerance) const
{
   return getSymmetryCount(CInsertTypeMask(insertTypeUnknown,insertTypePin),symmetryCriteria,searchTolerance);
}

int CDataList::getSymmetryCount(const CInsertTypeMask& insertTypeMask,SymmetryCriteriaTag symmetryCriteria,double searchTolerance) const
{
   int symmetryCount = 0;
   CPoint2d centroid = getCentroid(insertTypeMask);
   CTMatrix matrix;
   matrix.translate(-centroid);

   CTypedQfeTree<InsertStruct> insertTree;
   CTypedQfeList<InsertStruct> pinList;

   for (CDataListIterator insertIterator(*this,insertTypeMask);insertIterator.hasNext();)
   {
      DataStruct* pinData = insertIterator.getNext();
      InsertStruct* pin = pinData->getInsert();

      CPoint2d pinOrigin(pin->getOrigin2d());
      matrix.transform(pinOrigin);
      CTypedQfe<InsertStruct>* qfeInsert = new CTypedQfe<InsertStruct>(pinOrigin,pin);

      insertTree.setAt(qfeInsert);
      pinList.AddTail(qfeInsert);
   }

   int pinCount = pinList.GetCount();

   if (pinCount > 1)
   {
      CTypedQfe<InsertStruct>* pin1 = NULL;
      CPoint2d pin1Origin;
      double searchToleranceSqr = searchTolerance * searchTolerance;

      for (POSITION pin1pos = pinList.GetHeadPosition();pin1pos != NULL;)
      {
         pin1 = pinList.GetNext(pin1pos);
         pin1Origin = pin1->getOrigin();

         if (pin1Origin.getRadiusSqr() > searchToleranceSqr)
         {
            break;
         }

         pin1 = NULL;        
      }

      if (pin1 != NULL)
      {
         double minTheta = 4. * Pi;

         for (POSITION rotatedPinPos = pinList.GetHeadPosition();rotatedPinPos != NULL && symmetryCount == 0;)
         {
            CTypedQfe<InsertStruct>* otherPin = pinList.GetNext(rotatedPinPos);

            CPoint2d otherPinOrigin = otherPin->getOrigin();
            CTypedQfeList<InsertStruct> foundList;

            if (fpnear(pin1Origin.getRadius(),otherPinOrigin.getRadius(),searchTolerance))
            {
               double theta = normalizeRadians(otherPinOrigin.getTheta() - pin1Origin.getTheta());
               //double thetaDegrees = radiansToDegrees(theta);

               if (fpeq(theta,0.) || theta >= minTheta)
               {
                  continue;
               }

               if (symmetryCriteria == symmetryCriteriaPadstack)
               {
                  if (pin1->getObject()->getBlockNumber() != otherPin->getObject()->getBlockNumber())
                  {
                     continue;
                  }
               }

               CTMatrix rMatrix;
               rMatrix.rotateRadians(theta);

               for (POSITION pinPos = pinList.GetHeadPosition();;)
               {
                  if (pinPos == NULL)
                  {
                     minTheta = theta;
                     break;
                  }

                  CTypedQfe<InsertStruct>* rotatedPin = pinList.GetNext(pinPos);
                  CPoint2d rotatedPinOrigin = rotatedPin->getOrigin();
                  rMatrix.transform(rotatedPinOrigin);

                  CExtent searchExtent(rotatedPinOrigin,searchTolerance);

                  foundList.empty();
                  insertTree.search(searchExtent,foundList);
                  int foundCount = foundList.GetCount();

                  if (foundCount == 0)
                  {
                     break;
                  }
                  else if (foundCount == 1)
                  {
                     if (symmetryCriteria == symmetryCriteriaPadstack)
                     {
                        CTypedQfe<InsertStruct>* foundPin = foundList.GetHead();
                        CString foundPinNumber   = foundPin->getObject()->getRefname();
                        CString rotatedPinNumber = rotatedPin->getObject()->getRefname();

                        if (rotatedPin->getObject()->getBlockNumber() != foundPin->getObject()->getBlockNumber())
                        {
                           break;;
                        }
                     }
                  }
                  //else
                  //{
                  //   int foundCount = foundList.GetCount();
                  //   CTypedQfe<InsertStruct>* foundPin = foundList.GetHead();
                  //   CPoint2d foundPinOrigin = foundPin->getObject()->getOrigin2d();
                  //   CString foundPinNumber = foundPin->getObject()->getRefname();
                  //   CString rotatedPinNumber = rotatedPin->getObject()->getRefname();

                  //   int iii = 3;
                  //}
               }
            }
         }

         if (minTheta <= 2. * Pi)
         {
            symmetryCount = DcaRound(2.*Pi/minTheta);
         }
      }
   }

   return symmetryCount;
}

int CDataList::getInsertCount(const CInsertTypeMask& insertTypeMask) const
{
   int insertCount = 0;

   for (CDataListIterator insertIterator(*this,insertTypeMask);insertIterator.hasNext();)
   {
      DataStruct* data = insertIterator.getNext();
      insertCount++;
   }

   return insertCount;
}

DataStruct* CDataList::findEntity(int entityNumber)
{
   DataStruct* foundData = NULL;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = GetNext(pos);

      if (data->getEntityNumber() == entityNumber)
      {
         foundData = data;
         break;
      }
   }

   return foundData;
}

bool CDataList::isCompatibleWithRegularComplexAperture(CCamCadData& camCadData) const
{
   bool compatibleWithRegularComplexApertureFlag = true;

   for (POSITION pos = GetHeadPosition();pos != NULL && compatibleWithRegularComplexApertureFlag;)
   {
      compatibleWithRegularComplexApertureFlag = false;

      DataStruct* data = GetNext(pos);

      if (data->getDataType() == dataTypePoly)
      {
         LayerStruct* layer = camCadData.getLayer(data->getLayerIndex());

         if (layer != NULL && layer->isFloating())
         {
            compatibleWithRegularComplexApertureFlag = true;
         }
      }
   }

   return compatibleWithRegularComplexApertureFlag;
}

void CDataList::flattenInsert(CDataList& flatDataList,DataStruct& parentDataInsert,int parentLayer,CCamCadData& camCadData,
   const CTMatrix& parentMatrix,bool flattenRegularComplexAperturesFlag) const
{
   //CWriteFormat* writeFormat = getDebugWriteFormat();

   //if (writeFormat != NULL)
   //{
   //   writeFormat->writef("flattenInsert() -- Begin ----------------------------------\n");
   //   writeFormat->pushHeader("   ");
   //   parentDataInsert.dump(*writeFormat);
   //}

   InsertStruct* parentInsert = parentDataInsert.getInsert();
   BlockStruct* subBlock = camCadData.getBlockAt(parentInsert->getBlockNumber());

   CTMatrix matrix(parentMatrix);
   matrix.translateCtm(parentInsert->getOriginX(),parentInsert->getOriginY());
   matrix.rotateRadiansCtm(parentInsert->getAngle());
   matrix.scaleCtm(parentInsert->getGraphicMirrored() ? -1 : 1,1);

   // for debugging
   //CBasesVector basesVector;
   //basesVector.transform(matrix);
   //CPoint2d parentMatrixTranslation = basesVector.getOrigin();
   //double parentMatrixRotation      = basesVector.getRotation();
   //double parentMatrixScale         = basesVector.getScale();
   //bool parentMatrixMirror          = basesVector.getMirror();

   if (subBlock->isAperture())
   {
      bool keepUnflattenedApertureFlag = true;

      if (subBlock->isComplexAperture())
      {
         BlockStruct* complexApertureBlock = camCadData.getBlockAt(subBlock->getComplexApertureSubBlockNumber());

         if (flattenRegularComplexAperturesFlag || !complexApertureBlock->getDataList().isCompatibleWithRegularComplexAperture(camCadData))
         {
            keepUnflattenedApertureFlag = false;

            matrix = subBlock->getApertureTMatrix() * matrix;

            DataStruct* insertData = camCadData.getNewDataStruct(dataTypeInsert);
            insertData->getInsert()->setBlockNumber(complexApertureBlock->getBlockNumber());
            //insertData.setLayerIndex(parentDataInsert.getLayerIndex());

            flattenInsert(flatDataList,*insertData,parentLayer,camCadData,matrix,flattenRegularComplexAperturesFlag);

            delete insertData;
         }
      }
      
      if (keepUnflattenedApertureFlag)
      {
         DataStruct* newData = camCadData.getNewDataStruct(parentDataInsert);
         int apertureLayerIndex = camCadData.propagateLayer(parentLayer,newData->getLayerIndex());
         newData->setLayerIndex(apertureLayerIndex);

         CBasesVector basesVector;
         newData->getInsert()->setBasesVector(basesVector);
         newData->transform(matrix);

         flatDataList.AddTail(newData);
      }
   }
   else if (subBlock->isDrillHole())
   {
      DataStruct* newData = camCadData.getNewDataStruct(parentDataInsert);
      int drillLayerIndex = camCadData.propagateLayer(parentLayer,newData->getLayerIndex());
      newData->setLayerIndex(drillLayerIndex);

      CBasesVector basesVector;
      newData->getInsert()->setBasesVector(basesVector);
      newData->transform(matrix);

      flatDataList.AddTail(newData);
   }
   else
   {
      for (POSITION pos = subBlock->getDataList().GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = subBlock->getDataList().GetNext(pos);
         int dataLayer = camCadData.propagateLayer(parentLayer,data->getLayerIndex());
         //int dataLayer = (camCadData.IsFloatingLayer(data->getLayerIndex()) ? FloatingLayer : data->getLayerIndex());

         if (data->getDataType() == dataTypeInsert)
         {
            flattenInsert(flatDataList,*data,dataLayer,camCadData,matrix,flattenRegularComplexAperturesFlag);
         }
         else
         {
            DataStruct* newData = camCadData.getNewDataStruct(*data);
            newData->transform(matrix);
            newData->setLayerIndex(dataLayer);

            flatDataList.AddTail(newData);
         }
      }
   }

   //if (writeFormat != NULL)
   //{
   //   writeFormat->popHeader();
   //   writeFormat->writef("flattenInsert() -- End ----------------------------------\n");
   //   flushDebugWriteFormat();
   //}
}

CDataList* CDataList::getFlattenedDataList(CCamCadData& camCadData,int parentLayerIndex,bool flattenRegularComplexAperturesFlag) const
{
   CDataList& flatDataList = *(new CDataList(true));
   //CWriteFormat* writeFormat = getDebugWriteFormat();

   //if (writeFormat != NULL)
   //{
   //   writeFormat->writef("flattenPadstack() -- Begin ----------------------------------\n");
   //   writeFormat->pushHeader("   ");
   //   block.dump(*writeFormat);
   //}

   CTMatrix matrix;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = GetNext(pos);
      int dataLayerIndex = camCadData.propagateLayer(parentLayerIndex,data->getLayerIndex());

      if (data->getDataType() == dataTypeInsert)
      {
         flattenInsert(flatDataList,*data,dataLayerIndex,camCadData,matrix,flattenRegularComplexAperturesFlag);
      }
      else
      {
         DataStruct* newData = camCadData.getNewDataStruct(*data);
         flatDataList.AddTail(newData);
      }
   }

   //if (writeFormat != NULL)
   //{
   //   writeFormat->popHeader();
   //   writeFormat->writef("flattenPadstack() -- End ----------------------------------\n");
   //   flushDebugWriteFormat();
   //}
   return &flatDataList;
}

void CDataList::transform(const CTMatrix& transformationMatrix,CCamCadData* camCadData,bool doAttributes)
{
   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = GetNext(pos); 

      if (data != NULL)
      {
         data->transform(transformationMatrix,camCadData,doAttributes);
      }
   }
}

bool CDataList::isValid() const
{
   bool retval = true;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = GetNext(pos);

      if (data != NULL)
      {
         if (! data->isValid())
         {
            retval = false;
         }
      }
   }

   return retval;
}

//void CDataList::RemoveDataFromList(CCEtoODBDoc *doc, DataStruct *data, POSITION dataPos)
//{
//   RemoveOneEntityFromDataList(doc, this, data, dataPos);
//}
//
//void CDataList::RemoveDataByGraphicClass(CCEtoODBDoc *doc, GraphicClassTag graphicClass)
//{
//   for (CDataListIterator dataList(*this); dataList.hasNext();)
//   {
//      DataStruct* data = dataList.getNext();
//      if (data != NULL && data->getGraphicClass() == graphicClass)
//         RemoveOneEntityFromDataList(doc, this, data);
//   }
//}
//
//void CDataList::RemoveDataByInsertType(CCEtoODBDoc *doc, InsertTypeTag insertType)
//{
//   for (CDataListIterator dataList(*this, insertType); dataList.hasNext();)
//   {
//      DataStruct* data = dataList.getNext();
//      if (data != NULL)
//         RemoveOneEntityFromDataList(doc, this, data);
//   }
//}

void CDataList::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"CDataList\n"
"{\n"
"   Count=%d\n",
GetCount());

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      for (POSITION pos = GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = GetNext(pos);

         if (data != NULL)
         {
            data->dump(writeFormat,depth);
         }
      }

      writeFormat.popHeader();
   }

   writeFormat.writef("}\n");
}

//_____________________________________________________________________________
CMapEntityToDataStruct::CMapEntityToDataStruct(int blockSize,bool isContainer)
: CTypedMapIntToPtrContainer<DataStruct*>(blockSize,isContainer)
{
}

CMapEntityToDataStruct::CMapEntityToDataStruct(const CDataList& dataList)
: CTypedMapIntToPtrContainer<DataStruct*>(nextPrime2n(dataList.GetCount()),false)
{
   for (POSITION pos = dataList.GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = dataList.GetNext(pos);

      if (data != NULL)
      {
         SetAt(data->getEntityNumber(),data);
      }
   }
}

















