
#include "StdAfx.h"
#include "DcaCamCadData.h"
#include "DcaAttributes.h"
#include "DcaLib.h"
#include "DcaTMatrix.h"
#include "DcaDataType.h"
#include "DcaBlob.h"
#include "DcaInsert.h"
#include "DcaPoly.h"
#include "DcaText.h"
#include "DcaStandardAperture.h"
#include "DcaLayerType.h"
#include "DcaFont.h"
#include "DcaGeomLib.h"
#include "DcaPoint2.h"
#include "DcaDataListIterator.h"
#include "DcaWriteFormat.h"
#include "DcaGeomLib.h"


//_____________________________________________________________________________
CMapBlockNumberToBlock::CMapBlockNumberToBlock(int blockSize,bool isContainer)
: CTypedMapIntToPtrContainer<BlockStruct*>(blockSize,isContainer)
{
}

void CMapBlockNumberToBlock::setAt(BlockStruct* block)
{
   SetAt(block->getBlockNumber(),block);
}

//_____________________________________________________________________________
CAttributeValueDictionary::CAttributeValueDictionary()
{
   m_valueArray.SetSize(0,500);
   m_valueMap.InitHashTable(nextPrime2n(2000));

   add("");
}

CAttributeValueDictionary::~CAttributeValueDictionary()
{
}

const CStringArray& CAttributeValueDictionary::getValueArray() const
{
   return m_valueArray;
}

const CMapStringToPtr& CAttributeValueDictionary::getValueMap() const
{
   return m_valueMap;
}

int CAttributeValueDictionary::getSize() const
{
   int size = m_valueArray.GetSize();

   return size;
}

int CAttributeValueDictionary::getValueIndex(const CString& value) const
{
   int retval = -1;

   CString key(value);
   key.TrimLeft();
   key.TrimRight();

   if (!key.IsEmpty())
   {
      void* pIndex;

      if (m_valueMap.Lookup(key,pIndex))
      {
         retval = (int)pIndex;
      }
   }

   return retval;
}

int CAttributeValueDictionary::add(const CString& value)
{
   CString keyValue(value);
   keyValue.Trim();

   int index = getValueIndex(keyValue);

   //if (value.Compare("SMD") == 0)
   //{
   //   int iii = 3;
   //}

   //if (value.Compare("Placement") == 0)
   //{
   //   int iii = 3;
   //}

   if (index < 0)
   {
      index = m_valueArray.Add(keyValue);

      void* pIndex = (void*)index;

      m_valueMap.SetAt(keyValue,pIndex);
   }

   return index;
}

CString CAttributeValueDictionary::getAt(int index) const
{
   CString retval;

   if (index >= 0 && index < m_valueArray.GetSize())
   {
      retval = m_valueArray.GetAt(index);
   }

   return retval;
}

//_____________________________________________________________________________
CWidthTableArray::CWidthTableArray()
: m_blockNumberMap(nextPrime2n(400),false)
, m_zeroWidthIndex(-1)
, m_smallWidthIndex(-1)
{
   m_widthArray.SetSize(0,100);
}

void CWidthTableArray::empty()
{
   m_widthArray.RemoveAll();
   m_blockNumberMap.RemoveAll();

   m_zeroWidthIndex  = -1;
   m_smallWidthIndex = -1;
}

void CWidthTableArray::removeWidth(int widthIndex)
{
   if (widthIndex >= 0 && widthIndex < m_widthArray.GetSize())
   {
      BlockStruct* widthBlock = m_widthArray.GetAt(widthIndex);

      if (widthBlock != NULL)
      {
         int blockNumber = widthBlock->getBlockNumber();

         m_widthArray.SetAt(widthIndex,NULL);

         int referencedWidthIndex = lookupFromMap(blockNumber);

         if (referencedWidthIndex == widthIndex)
         {
            removeFromMap(blockNumber);
         }
      }
   }
}

void CWidthTableArray::removeFromMap(int blockNumber)
{
   m_blockNumberMap.RemoveKey(blockNumber);
}

void CWidthTableArray::addToMap(int blockNumber,int widthIndex)
{
   m_blockNumberMap.SetAt(blockNumber,(void*)widthIndex);
}

int CWidthTableArray::lookupFromMap(int blockNumber) const
{
   int retval = -1;
   void* widthIndex;

   if (m_blockNumberMap.Lookup(blockNumber,widthIndex))
   {
      retval = (int)widthIndex;
   }

   return retval;
}

void CWidthTableArray::SetAtGrow(int widthIndex, BlockStruct* block)
{
   removeWidth(widthIndex);

   // Must have a block for a width
   if (block != NULL)
   {
      m_widthArray.SetAtGrow(widthIndex,block);
      addToMap(block->getBlockNumber(),widthIndex);
   }
}

int CWidthTableArray::Add(BlockStruct* block)
{
   int widthIndex = m_widthArray.Add(block);
   m_blockNumberMap.SetAt(block->getBlockNumber(),(void*)widthIndex);

   return widthIndex;
}

int CWidthTableArray::GetSize() const
{
   return m_widthArray.GetSize();
}

int CWidthTableArray::GetCount() const
{
   return m_widthArray.GetCount();
}

BlockStruct* CWidthTableArray::GetAt(int index) const
{
   return m_widthArray.GetAt(index);
}

//BlockStruct*& CWidthTableArray::operator[](int index)
//{
//   return m_widthArray[index];
//}

BlockStruct* CWidthTableArray::operator[](int index) const
{
   return m_widthArray[index];
}

BlockStruct* CWidthTableArray::getAt(int widthIndex) const
{
   BlockStruct* block = NULL;

   if (widthIndex >= 0 && widthIndex < GetSize())
   {
      block = GetAt(widthIndex);
   }

   return block;
}

int CWidthTableArray::getWidthIndex(BlockStruct* widthBlock) const
{
   int foundWidthIndex = lookupFromMap(widthBlock->getBlockNumber());

   return foundWidthIndex;
}

void CWidthTableArray::SetAt(int widthIndex,BlockStruct* widthBlock)
{
   if (widthIndex >= 0 && widthIndex < m_widthArray.GetSize())
   {
      removeWidth(widthIndex);

      m_widthArray.SetAt(widthIndex,widthBlock);

      if (widthBlock != NULL)
      {
         addToMap(widthBlock->getBlockNumber(),widthIndex);
      }
   }
}

int CWidthTableArray::getDefinedWidthIndex(BlockStruct& block)
{
   int widthIndex = lookupFromMap(block.getBlockNumber());

   if (widthIndex < 0)
   {
      widthIndex = Add(&block);
   }

   return widthIndex;
}

int CWidthTableArray::getZeroWidthIndex() const
{
   return m_zeroWidthIndex;
}

void CWidthTableArray::setZeroWidthIndex(int widthIndex)
{
   if (widthIndex < 0 || widthIndex >= GetSize())
   {
      widthIndex = -1;
   }

   m_zeroWidthIndex = widthIndex;
}

int CWidthTableArray::getSmallWidthIndex() const
{
   return m_smallWidthIndex;
}

void CWidthTableArray::setSmallWidthIndex(int widthIndex)
{
   if (widthIndex < 0 || widthIndex >= GetSize())
   {
      widthIndex = -1;
   }

   m_smallWidthIndex = widthIndex;
}

//_____________________________________________________________________________
CCamCadData::CCamCadData()
: m_blockDirectory(*this)
, m_fileList(*this)
, m_sourceVersionMajor(-1)
, m_sourceVersionMinor(-1)
, m_sourceVersionRevision(-1)
{
   m_standardKeywordArray.SetSize(0,400);
}

CCamCadData::~CCamCadData()
{
   m_fileList.empty();
}

int CCamCadData::allocateEntityNumber()
{
   int entityNumber = m_entityNumberGenerator.allocate();

   //switch (entityNumber)
   //{
   //case 13721:
   //   { 
   //      int iii = 3; 
   //   }
   //   break;
   //case 2718:
   //   { 
   //      int iii = 3; 
   //   }
   //   break;
   //}

   return entityNumber;
}

int CCamCadData::allocateEntityNumber(int& entityNumber)
{
   if (entityNumber < 0)
   {
      entityNumber = m_entityNumberGenerator.allocate();
   }
   else
   {
      m_entityNumberGenerator.setAsAllocated(entityNumber);
   }

   //switch (entityNumber)
   //{
   //case 13721:
   //   { 
   //      int iii = 3; 
   //   }
   //   break;
   //case 2718:
   //   { 
   //      int iii = 3; 
   //   }
   //   break;
   //}

   return entityNumber;
}

int CCamCadData::getCurrentEntityNumber()
{
   int entityNumber = m_entityNumberGenerator.getCurrent();

   return entityNumber;
}

//int CCamCadData::setAsAllocated(int allocatedEntityNumber)
//{
//   int previousNextEntityNumber = m_entityNumberGenerator.setAsAllocated(allocatedEntityNumber);
//
//   return previousNextEntityNumber;
//}

PageUnitsTag CCamCadData::getPageUnits() const
{
   return m_camCadDataSettings.getPageUnits();
}

void CCamCadData::setPageUnits(PageUnitsTag pageUnits)
{
   m_camCadDataSettings.setPageUnits(pageUnits);
}

BlockStruct& CCamCadData::getDefinedWidthBlock(int widthIndex)
{
   if (widthIndex < 0 || widthIndex >= getWidthTable().GetSize())
   {
      widthIndex = getZeroWidthIndex();
   }

   BlockStruct* widthBlock = m_widthTable.getAt(widthIndex);

   return *widthBlock;
}

BlockStruct* CCamCadData::getWidthBlock(int widthIndex)
{
   BlockStruct* widthBlock = NULL;

   if (widthIndex >= 0 && widthIndex < getWidthTable().GetSize())
   {
      widthBlock = m_widthTable.getAt(widthIndex);
   }

   return widthBlock;
}

int CCamCadData::getMaxBlockIndex()
{
   return getBlockArray().getSize();
}

BlockStruct* CCamCadData::getBlockAt(int index)
{
   return getBlockArray().getBlockAt(index);
}

int CCamCadData::getZeroWidthIndex()
{
   int zeroWidthIndex = getWidthTable().getZeroWidthIndex();

   if (zeroWidthIndex < 0)
   {
      int widthFlags = BL_WIDTH | BL_APERTURE | BL_GLOBAL ;

      BlockStruct* block = m_blockDirectory.getAt(QZeroWidth,-1,blockTypeUnknown);

      if (block != NULL)
      {
         if ((block->getFlags() & widthFlags) != widthFlags ||
              block->getShape() != apertureRound ||
              block->getSizeA() != 0.)
         {
            block = NULL;
         }
      }

      if (block == NULL)
      {
         block = &(m_blockDirectory.getNewBlock(QZeroWidth,-1,blockTypeUnknown));

         block->setShape(apertureRound);
         block->setSizeA(0.);
         block->setFlags(widthFlags);
      }

      zeroWidthIndex = getWidthTable().Add(block);
      getWidthTable().setZeroWidthIndex(zeroWidthIndex);
   }

   return zeroWidthIndex;
}

int CCamCadData::getSmallWidthIndex()
{
   int smallWidthIndex = getWidthTable().getSmallWidthIndex();

   if (smallWidthIndex < 0)
   {
      int widthFlags = BL_WIDTH | BL_APERTURE | BL_GLOBAL | BL_SMALLWIDTH;

      BlockStruct* block = m_blockDirectory.getAt(QSmallWidth,-1,blockTypeUnknown);

      if (block != NULL)
      {
         if ((block->getFlags() & widthFlags) != widthFlags ||
              block->getShape() != apertureRound ||
              block->getSizeA() != getCamCadDataSettings().getSmallWidth())
         {
            block = NULL;
         }
      }

      if (block == NULL)
      {
         block = &(m_blockDirectory.getNewBlock(QSmallWidth,-1,blockTypeUnknown));

         block->setShape(apertureRound);
         block->setSizeA(getCamCadDataSettings().getSmallWidth());
         block->setFlags(widthFlags);
      }

      smallWidthIndex = getWidthTable().Add(block);
      getWidthTable().setSmallWidthIndex(smallWidthIndex);
   }

   return smallWidthIndex;
}

CAttribute* CCamCadData::constructAttribute()
{
   CAttribute* attribute = new CAttribute(*this);

   return attribute;
}

CAttribute* CCamCadData::constructAttribute(ValueTypeTag valueType)
{
   CAttribute* attribute = new CAttribute(*this,valueType);

   return attribute;
}

const KeyWordStruct* CCamCadData::getAttributeKeyword(int keywordIndex) const
{
   const KeyWordStruct* keyword = NULL;

   if (keywordIndex >= 0 && keywordIndex < getKeyWordArray().GetSize())
   {
      keyword = getKeyWordArray().GetAt(keywordIndex);
   }

   return keyword;
}

const KeyWordStruct* CCamCadData::getAttributeKeyword(StandardAttributeTag standardAttributeTag)
{
   const KeyWordStruct* keyword = NULL;

   if (standardAttributeTag < m_standardKeywordArray.GetSize())
   {
      keyword = m_standardKeywordArray.GetAt(standardAttributeTag);
   }

   if (keyword == NULL)
   {
      CString keywordName = StandardAttributeTagToName(standardAttributeTag);
      int keywordIndex = getKeyWordArray().Lookup(keywordName,keyWordSectionCC);

      if (keywordIndex < 0)
      {
         keywordIndex = getKeyWordArray().SetAtGrow(keywordName, attribGroupInternal, valueTypeString);
      }

      if (keywordIndex >= 0)
      {
         keyword = getKeyWordArray().GetAt(keywordIndex);
      }

      m_standardKeywordArray.SetAtGrow(standardAttributeTag,(KeyWordStruct*)keyword);
   }

   return keyword;
}

int CCamCadData::getAttributeKeywordIndex(StandardAttributeTag standardAttributeTag)
{
   int keywordIndex = -1;

   const KeyWordStruct* keyword = getAttributeKeyword(standardAttributeTag);

   if (keyword != NULL)
   {
      keywordIndex = keyword->getIndex();
   }

   return keywordIndex;
}

int CCamCadData::getAttributeKeywordIndex(const CString& attributeName) const
{
   int keywordIndex = getKeyWordArray().Lookup(attributeName,keyWordSectionCC);

   return keywordIndex;
}

int CCamCadData::getDefinedAttributeKeywordIndex(const CString& attributeName,ValueTypeTag valueType)
{
   int keywordIndex = getKeyWordArray().Lookup(attributeName,keyWordSectionCC);

   if (keywordIndex < 0)
   {
      keywordIndex = getKeyWordArray().getDefinedKeywordIndex(attributeName,valueType);
   }

   return keywordIndex;
}

bool CCamCadData::setAttribute(CAttributes& attributes,int keywordIndex,ValueTypeTag valueType,void* value,AttributeUpdateMethodTag updateMethod,CAttribute** ppAttribute)
{
   switch (valueType)
   {
   case valueTypeNone:
      attributes.setAttribute(*this,keywordIndex);

      break;
   case valueTypeString:
   case valueTypeEmailAddress:
   case valueTypeWebAddress:
      {
         CString stringValue = (char*)value;

         attributes.setAttribute(*this,keywordIndex,stringValue,updateMethod);
      }

      break;
   case valueTypeDouble:
   case valueTypeUnitDouble:
      {
         double doubleValue = *((double*)value);

         attributes.setAttribute(*this,keywordIndex,doubleValue,updateMethod);
      }

      break;
   case valueTypeInteger:
      {
         int intValue = *((int*)value);

         attributes.setAttribute(*this,keywordIndex,intValue,updateMethod);
      }

      break;
   }

   if (ppAttribute != NULL)
   {
      CAttribute* attribute;
      attributes.Lookup(keywordIndex,attribute);

      *ppAttribute = attribute;
   }

   return false;
}

CAttribute* CCamCadData::setAttribute(CAttributes& attributes,int keywordIndex,const CString& value)  // SA_OVERWRITE
{
   return attributes.setAttribute(*this,keywordIndex,value);
}

CAttribute*  CCamCadData::setAttribute(CAttributes& attributes,StandardAttributeTag attributeTag,const CString& value)  // SA_OVERWRITE
{
   return attributes.setAttribute(*this,attributeTag,value);
}

int CCamCadData::getDefinedAttributeStringValueIndex(const CString& stringValue)
{
   return m_attributeValueDictionary.add(stringValue);
}

bool CCamCadData::getAttributeStringValue(CString& attributeStringValue,CAttributes& attributes,StandardAttributeTag attributeTag) const
{
   return attributes.getAttribute(*this,attributeTag,attributeStringValue);
}

CString CCamCadData::getAttributeStringValue(const CAttribute& attribute) const
{
   CString stringValue;

   switch (attribute.getValueType())
   {
   case valueTypeString:
   case valueTypeEmailAddress:
   case valueTypeWebAddress:
      {
         int stringIndex = attribute.getStringValueIndex();

         stringValue = getAttributeValueDictionaryStringValue(stringIndex);
      }

      break;
   case valueTypeDouble:
   case valueTypeUnitDouble:
      stringValue.Format("%f",attribute.getDoubleValue());
      break;
   case valueTypeInteger:
      stringValue.Format("%d",attribute.getIntValue());
      break;
   }

   return stringValue;
}

double CCamCadData::getAttributeDoubleValue(const CAttribute& attribute) const
{
   double doubleValue = 0.;

   switch (attribute.getValueType())
   {
   case valueTypeString:
   case valueTypeEmailAddress:
   case valueTypeWebAddress:
      {
         int stringIndex = attribute.getStringValueIndex();

         CString stringValue = getAttributeValueDictionaryStringValue(stringIndex);
         doubleValue = atof(stringValue);
      }

      break;
   case valueTypeDouble:
   case valueTypeUnitDouble:
      doubleValue = attribute.getDoubleValue();
      break;
   case valueTypeInteger:
      doubleValue = attribute.getIntValue();
      break;
   }

   return doubleValue;
}

int CCamCadData::getAttributeIntegerValue(const CAttribute& attribute) const
{
   int integerValue = 0;

   switch (attribute.getValueType())
   {
   case valueTypeString:
   case valueTypeEmailAddress:
   case valueTypeWebAddress:
      {
         int stringIndex = attribute.getStringValueIndex();

         CString stringValue = getAttributeValueDictionaryStringValue(stringIndex);
         integerValue = DcaRound(atof(stringValue));
      }

      break;
   case valueTypeDouble:
   case valueTypeUnitDouble:
      integerValue = DcaRound(attribute.getDoubleValue());
      break;
   case valueTypeInteger:
      integerValue = attribute.getIntValue();
      break;
   }

   return integerValue;
}

CString CCamCadData::getAttributeValueDictionaryStringValue(int stringIndex) const
{
   return m_attributeValueDictionary.getAt(stringIndex);
}

int CCamCadData::addAttributeValueDictionaryStringValue(const CString& value)
{
   return m_attributeValueDictionary.add(value);
}

int CCamCadData::getAttributeValueDictionarySize(const CString& value) const
{
   return m_attributeValueDictionary.getSize();
}

int CCamCadData::getAttributeValueDictionaryValueIndex(const CString& value) const
{
   return m_attributeValueDictionary.getValueIndex(value);
}

CBlockDirectory& CCamCadData::getBlockDirectory()
{
   return m_blockDirectory;
}

CBlockArray& CCamCadData::getBlockArray()
{
   return m_blockDirectory.getBlockArray();
}

CFileList& CCamCadData::getFileList()
{
   return m_fileList;
}

CGTabTableList& CCamCadData::getTableList()
{
   return m_tableList;
}

CCamCadDataSettings& CCamCadData::getCamCadDataSettings()
{
   return m_camCadDataSettings;
}

const CCamCadDataSettings& CCamCadData::getCamCadDataSettings() const
{
   return m_camCadDataSettings;
}

CLayerArray& CCamCadData::getLayerArray()
{
   return m_layerArray;
}

const CLayerArray& CCamCadData::getLayerArray() const
{
   return m_layerArray;
}

CKeyWordArray& CCamCadData::getKeyWordArray()
{
   return m_keyWordArray;
}

const CKeyWordArray& CCamCadData::getKeyWordArray()        const
{
   return m_keyWordArray;
}

CTopicsList& CCamCadData::getTopicsList()
{ 
   return m_TopicList;
}

CNamedViewList& CCamCadData::getNamedViewList()
{
   return m_namedViewList;
}

BackgroundBitmapStruct& CCamCadData::getTopBackgroundBitMap()
{
   return m_topBackgroundBitMap;
}

BackgroundBitmapStruct& CCamCadData::getBottomBackgroundBitMap()
{
   return m_bottomBackgroundBitMap;
}

CString& CCamCadData::getRedLineString()
{
   return m_redLineString;
}

CString& CCamCadData::getLotsString()
{
   return m_lotsString;
}

CString& CCamCadData::getColorSetsString()
{
   return m_colorSetsString;
}

CWidthTableArray& CCamCadData::getWidthTable()
{
   return m_widthTable;
}

int CCamCadData::getNextWidthIndex()
{
   return getWidthTable().GetSize();
}

CStringArray& CCamCadData::getDfmAlgorithmNamesArray()
{
   return m_dfmAlgorithmNamesArray;
}

int CCamCadData::getDfmAlgorithmArraySize()
{
   return (int) m_dfmAlgorithmNamesArray.GetSize();
}

CAttributeValueDictionary& CCamCadData::getAttributeValueDictionary()
{
   return m_attributeValueDictionary;
}

const CAttributeValueDictionary& CCamCadData::getAttributeValueDictionary() const
{
   return m_attributeValueDictionary;
}

CString CCamCadData::getCcPath() const
{
   return m_ccPath;
}

void    CCamCadData::setCcPath(const CString& path)
{
   m_ccPath = path;
}

CString CCamCadData::getUserPath() const
{
   return m_userPath;
}

void    CCamCadData::setUserPath(const CString& path)
{
   m_userPath = path;
}

CString CCamCadData::getSourceProduct() const
{
   return m_sourceProduct;
}

void CCamCadData::setSourceProduct(const CString& sourceProduct)
{
   m_sourceProduct = sourceProduct;
}

int CCamCadData::getSourceVersionMajor() const
{
   return m_sourceVersionMajor;
}

void CCamCadData::setSourceVersionMajor(int sourceVersionMajor)
{
   m_sourceVersionMajor = sourceVersionMajor;
}

int CCamCadData::getSourceVersionMinor() const
{
   return m_sourceVersionMinor;
}

void CCamCadData::setSourceVersionMinor(int sourceVersionMinor)
{
   m_sourceVersionMinor = sourceVersionMinor;
}

int CCamCadData::getSourceVersionRevision() const
{
   return m_sourceVersionRevision;
}

void CCamCadData::setSourceVersionRevision(int sourceVersionRevision)
{
   m_sourceVersionRevision = sourceVersionRevision;
}

//===============================================================================================================================================
// to be deprecated =============================================================================================================================
//===============================================================================================================================================

/******************************************************************************
* ported from CCEtoODBDoc::SetVisAttrib() for dca
*
* SetVisAttrib()
*
*  Description: Sets values for keyword for a data (allocating AttribMap if necessary)
*
*  Parameter : data
*              keyword
*              void *value    --> int*, double*, char* (or NULL for valueTypeNone)
*              method --> SA_OVERWRITE, SA_APPEND, or SA_RETURN
*
*  Returns :   TRUE if keyword existed
*              FALSE if adding new keyword to map
*/
bool CCamCadData::setVisibleAttribute(CAttributes& attributes, int keywordIndex,
   ValueTypeTag valueType, void* value,
   double x, double y, double rotationRadians, double height, double width, bool proportionalSpacing, int penWidthIndex,
   bool visible, AttributeUpdateMethodTag updateMethod, DbFlag flags, short layerIndex, 
   bool mirrorDisabled, bool mirrorFlip,
   HorizontalPositionTag horizontalPosition, VerticalPositionTag verticalPosition,
   CAttribute **createdAttrib)
{
   CAttribute *attrib;
   bool result = setAttribute(attributes, keywordIndex, valueType, value, updateMethod, &attrib);

   attrib->setCoordinate(x,y);
   attrib->setRotationRadians(rotationRadians);
   attrib->setHeight(height);
   attrib->setWidth(width);
   attrib->setProportionalSpacing(proportionalSpacing);
   attrib->setPenWidthIndex(penWidthIndex);
   attrib->setMirrorDisabled(mirrorDisabled);
   attrib->setMirrorFlip(mirrorFlip);
   attrib->setVisible(visible);
   attrib->setFlags(flags);
   attrib->setLayerIndex(layerIndex);
   attrib->setInherited(false);
   attrib->setHorizontalPosition(horizontalPosition);
   attrib->setVerticalPosition(verticalPosition);

   if (createdAttrib != NULL)
      *createdAttrib = attrib;

   return result;
}


/* Add_Blockname *****************************************************************
*     - Adds a block to the blocklist
*     - Does not check for any conflicting blocks
*     - if (Aperture), tries to extract a dcode from the blockname
*     - returns block ptr
*/                                 
//BlockStruct* CCamCadData::Add_Blockname(const char *name, int filenum, DbFlag flg, BOOL Aperture, BlockTypeTag blockType)
//{
//   if (filenum == 0)
//   {
//      filenum = -1;
//   }
//
//   BlockStruct& block = getBlockArray().getNewBlock();
//   block.getAttributesRef() = NULL;
//
//   // init variables
//   block.setLibraryNumber(0);
//   block.setFileNumber(0);
//   block.setMarked(0);
//
//   if (blockType == blockTypeUndefined)
//      block.setBlockType(0);
//   else
//      block.setBlockType(blockType);
//
//   block.setFlags(0L);
//   block.resetExtent();
//   block.setShape(0);
//   block.setDcode(0);
//   block.setSizeA(0.);
//   block.setSizeB(0.);
//   block.setSizeC(0.);
//   block.setSizeD(0.);
//   block.setXoffset(0.);
//   block.setYoffset(0.);
//   block.setRotation(0.);
//   block.setSpokeCount(4);
//
//   block.setTcode(0);
//   block.setToolDisplay(false);
//   block.setToolBlockNumber(0);
//   block.setToolType(0);
//   block.setToolSize(0.0);
//   
//   block.setName(name);
//   block.setOriginalName(""); // initialized WS 30-dec-98
//   block.setFileNumber(filenum);
//   block.setFlags(flg);
//   block.setPackage(false);
//
//   // if aperture, set block.ApNum accordingly
//   if (Aperture)
//   {
//      if (strlen(name))
//      {
//         int i = strlen(name)-1;
//         int tmpcnt = 0;
//         char  tmp[80];
//
//         while (i && tmpcnt < 80 && isdigit(name[i]))  tmp[tmpcnt++] = name[i--];
//
//         tmp[tmpcnt] = '\0';
//         strrev(tmp);
//         block.setDcode(atoi(tmp));
//      }
//   }
//
//   return block;
//}

BlockStruct& CCamCadData::getDefinedBlock(int blockNumber)
{
   if (blockNumber < 0)
   {
      blockNumber = 0;
   }

   BlockStruct& block = getBlockDirectory().getDefinedBlock(blockNumber);

   return block;
}

BlockStruct& CCamCadData::getDefinedBlock(int blockNumber,const CString& name,int fileNumber,BlockTypeTag blockType)
{
   if (blockNumber < 0)
   {
      blockNumber = 0;
   }

   BlockStruct& block = getBlockDirectory().getDefinedBlock(blockNumber,name,fileNumber,blockType);

   return block;
}

int CCamCadData::getDefinedDfmAlgorithmNameIndex(const CString& algorithmName)
{
   int nameIndex;

   for (nameIndex = 0;;nameIndex++)
   {
      if (nameIndex >= m_dfmAlgorithmNamesArray.GetSize())
      {
         m_dfmAlgorithmNamesArray.Add(algorithmName);
         break;
      }

      CString existingName = m_dfmAlgorithmNamesArray.GetAt(nameIndex);

      if (existingName.Compare(algorithmName) == 0)
      {
         break;
      }
   }

   return nameIndex;
}

bool CCamCadData::isInsertTypeVisible(InsertTypeTag insertType)
{
   return getInsertTypeFilter().contains(insertType);
}

bool CCamCadData::isGraphicClassVisible(GraphicClassTag graphicClass)
{
   return getGraphicsClassFilter().contains(graphicClass);
}

CInsertTypeFilter& CCamCadData::getInsertTypeFilter()
{
   return m_insertTypeFilter;
}

CLayerFilterStack& CCamCadData::getSelectLayerFilterStack()
{
   return m_selectLayerFilterStack;
}

CGraphicsClassFilter& CCamCadData::getGraphicsClassFilter()
{
   return m_graphicsClassFilter;
}

LayerStruct& CCamCadData::getDefinedFloatingLayer()
{
   return m_layerArray.getDefinedFloatingLayer();
}

LayerStruct& CCamCadData::getDefinedLayer(int layerIndex)
{
   if (layerIndex < 0)
   {
      layerIndex = 0;
   }

   return m_layerArray.getDefinedLayerAt(layerIndex);
}

int CCamCadData::propagateLayer(int parentLayerIndex,int childLayerIndex)
{
   int retval = childLayerIndex;

/*
   Parent   Child  Result
  -1       -1       -1
  -1        f        f
  -1        Lc       Lc
   f       -1        f
   f        f        f
   f        Lc       Lc
   Lp      -1        Lp
   Lp       f        Lp
   Lp       Lc       Lc
*/

   if (childLayerIndex != -1)
   {
      if (getLayer(childLayerIndex)->isFloating())
      {
         childLayerIndex = FloatingLayer;
      }
   }

   if (parentLayerIndex != -1)
   {
      if (getLayer(parentLayerIndex)->isFloating())
      {
         parentLayerIndex = FloatingLayer;
      }
   }

   if (parentLayerIndex == -1)
   {
      if (childLayerIndex == -1)
      {
         retval = -1;
      }
      else if (childLayerIndex == FloatingLayer)
      {
         retval = FloatingLayer;
      }
      else
      {
         retval = childLayerIndex;
      }
   }
   else if (parentLayerIndex == FloatingLayer)
   {
      if (childLayerIndex == -1)
      {
         retval = FloatingLayer;
      }
      else if (childLayerIndex == FloatingLayer)
      {
         retval = FloatingLayer;
      }
      else
      {
         retval = childLayerIndex;
      }
   }
   else
   {
      if (childLayerIndex == -1)
      {
         retval = parentLayerIndex;
      }
      else if (childLayerIndex == FloatingLayer)
      {
         retval = parentLayerIndex;
      }
      else
      {
         retval = childLayerIndex;
      }
   }

   if (retval == FloatingLayer)
   {
      retval = getDefinedFloatingLayer().getLayerIndex();
   }

   return retval;
}

void CCamCadData::setMaxCoords()
{
}

void CCamCadData::loadBackgroundBitmap(BackgroundBitmapStruct& backgroundBitMap)
{
}

//===============================================================================================================================================================================
FileStruct* CCamCadData::getSingleVisiblePcb()
{
   FileStruct* pcbFileStruct = NULL;

   // make sure we are showing only one board   
   for (POSITION filePos = getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* fileStruct = getFileList().GetNext(filePos);

      if (fileStruct->isHidden() || !fileStruct->isShown())
         continue;

      if (fileStruct->getBlockType() == blockTypePcb)
      {
         if (pcbFileStruct == NULL)
         {
            pcbFileStruct = fileStruct;
         }
         else
         {
            pcbFileStruct = NULL;
            break;
         }
      }
   }

   return pcbFileStruct;
}

FileStruct* CCamCadData::getFile(int fileNumber)
{
   FileStruct* fileStruct = getFileList().FindByFileNumber(fileNumber);

   return fileStruct;
}

FileStruct* CCamCadData::getFirstVisibleFile()
{
   FileStruct* firstVisibleFileStruct = NULL;

   // make sure we are showing only one board   
   for (POSITION filePos = getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* fileStruct = getFileList().GetNext(filePos);

      if (fileStruct->isHidden() || !fileStruct->isShown())
      {
         continue;
      }

      firstVisibleFileStruct = fileStruct;

      break;
   }

   return firstVisibleFileStruct;
}

BlockStruct& CCamCadData::getDefinedBlock(const CString& name,BlockTypeTag blockType,int fileNum)
{
   return m_blockDirectory.getDefinedBlock(name,fileNum,blockType);
}

BlockStruct* CCamCadData::getNewBlock(const CString& prefix,const CString& suffixFormat,BlockTypeTag blockType,int fileNum)
{
   CString name = getNewBlockName(prefix,suffixFormat,fileNum);
   BlockStruct& block = getDefinedBlock(name,blockType,fileNum);

   return &block;
}

BlockStruct& CCamCadData::getNewBlock(const CString& name,int fileNumber,BlockTypeTag blockType)
{
   return m_blockDirectory.getNewBlock(name,fileNumber,blockType);
}

BlockStruct* CCamCadData::getNewBlock(const CString& prefixFormat,BlockTypeTag blockType,int fileNum)
{
   CString name = getNewBlockName(prefixFormat,fileNum);
   BlockStruct& block = getDefinedBlock(name,blockType,fileNum);

   return &block;
}

BlockStruct* CCamCadData::getBlock(const CString& name,int fileNumber)
{
   return m_blockDirectory.getAt(name,fileNumber);
}

CString CCamCadData::getNewBlockName(const CString& prefix,const CString& suffixFormat,int fileNum,bool skipFirstSuffixFlag)
{
   CString name,suffix;

   BlockStruct* block;

   for (int ind=(skipFirstSuffixFlag ? -1 : 0);;ind++)
   {
      if (ind >= 0)
      {
         suffix.Format(suffixFormat,ind);
      }

      name  = prefix + suffix;

      if (name.GetLength() > 0)
      {
         block = getBlock(name,fileNum);

         if (block == NULL)
         {
            break;
         }
      }
   }

   return name;
}

CString CCamCadData::getNewBlockName(CString prefixFormat,int fileNum)
{
   CString name,previousName;
   BlockStruct* block;

   for (int ind=0;;ind++)
   {
      name.Format(prefixFormat,ind);
      block = getBlock(name,fileNum);

      if (block == NULL)
      {
         break;
      }

      if (name == previousName)
      {
         prefixFormat = "%d";
      }

      previousName = name;
   }

   return name;
}

//void CCamCadData::calculateBlockExtents(BlockStruct& block)
//{
//}

BlockStruct* CCamCadData::getBlock(int blockNumber) const
{
   BlockStruct* block = m_blockDirectory.getAt(blockNumber);

   return block;
}

#ifdef ImplementNew_CCamCadData_blockExtents
CExtent CCamCadData::blockExtents(CDataList& DataList,double insert_x,double insert_y,double rotation,int mirror,double scale,
   int insertLayer,bool checkOnlyVisibleEntitiesFlag,CMessageFilter* messageFilter)
{
	bool debugFlag = false;
   static int callLevel = 0;  // for debugging

   callLevel++;

   CExtent extent;
   BlockStruct* subblock;
   //ExtentRect extents;
   int layer;
   int extentCount = 0;

#ifdef UseOldTransform
   Mat2x2 m;
   RotMat2(&m, rotation);
#endif

   CTMatrix matrix;

   matrix.scale(((mirror & MIRROR_FLIP) != 0) ? -scale : scale,scale);
   matrix.rotateRadians(rotation);
   matrix.translate(insert_x,insert_y);

   double spaceRatio = getCamCadDataSettings().getTextSpacingRatio();

   POSITION dataPos = DataList.GetHeadPosition();

   for (int index=0;dataPos != NULL;index++)
   {
      DataStruct* data = DataList.GetNext(dataPos);

      if (data->isHidden())
         continue;

      if (data->getDataType() == dataTypeBlob)
      {
#ifdef UseOldTransform
         Point2 p;
         p.x = data->getBlob()->pnt.x * scale;

         if (mirror & MIRROR_FLIP)
            p.x = -p.x;

         p.y = data->getBlob()->pnt.y * scale;
         TransPoint2(&p, 1, &m, insert_x, insert_y);

         extent.update(p.x,p.y);
         extent.update(p.x + data->getBlob()->width * scale,p.y + data->getBlob()->height * scale);
#else
         CPoint2d origin = data->getBlob()->getOrigin();
         origin.transform(matrix);

         extent.update(origin);
         extent.update(origin.x + data->getBlob()->width * scale,origin.y + data->getBlob()->height * scale);
#endif
         //extent.assertFloat();

         extentCount++;
      }
      else
      {
         if (data->getDataType() == dataTypeInsert)
         {
            if (checkOnlyVisibleEntitiesFlag && !getInsertTypeFilter().contains(data->getInsert()->getInsertType()))
               continue;
         }
         else 
         {  
            if (checkOnlyVisibleEntitiesFlag && !getGraphicsClassFilter().contains((data->getGraphicClass()>0?data->getGraphicClass():0)))
               continue;
         }

         // figure LAYER
         if (isFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
         {
            layer = insertLayer;
         }
         else
         {
            layer = data->getLayerIndex();
         }

         // figure TYPE
         if (data->getDataType() == dataTypeInsert)
         {
            subblock = getBlockDirectory().getAt(data->getInsert()->getBlockNumber());
            // this only can happen if a insert refers to a non existing geometry
            if (subblock == NULL)
               continue;
         }

         // Check Layer Visible
         if (data->getDataType() != dataTypeInsert || 
               ((subblock->getFlags() & BL_TOOL     || subblock->getFlags() & BL_BLOCK_TOOL    ) && !subblock->getToolDisplay()) || 
               ((subblock->getFlags() & BL_APERTURE || subblock->getFlags() & BL_BLOCK_APERTURE) &&  subblock->getShape() != apertureComplex))
         {
            // only check layer for non insert entities
            if (checkOnlyVisibleEntitiesFlag && !isFloatingLayer(layer) && !isLayerPairVisible(layer)) 
               continue;
         }
         else // block
         { 
            // adjust for LAYER = -1 
            // only for Block Apertures and Block Tools when insertLayer = -1
            if (layer == -1 &&
               ((subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE) ||
                (subblock->getFlags() & BL_TOOL    ) || (subblock->getFlags() & BL_BLOCK_TOOL    )) )
            {
               if (!subblock->getDataList().IsEmpty() )
               {
                  layer = subblock->getDataList().GetHead()->getLayerIndex();

                  if (checkOnlyVisibleEntitiesFlag && !isLayerPairVisible(layer))
                     continue;
               }
            }
         }

         switch (data->getDataType())
         {
         case dataTypePoint:
            {
#ifdef UseOldTransform
               Point2 p;
               p.x = data->getPoint()->x * scale;

               if (mirror & MIRROR_FLIP)
                  p.x = -p.x;

               p.y = data->getPoint()->y * scale;
               TransPoint2(&p, 1, &m, insert_x, insert_y);

               extent.update(p.x,p.y);
               //extent.assertFloat();
#else
               CPoint2d point = data->getPoint()->getPoint2d();
               point.transform(matrix);

               extent.update(point);
#endif

               extentCount++;
            }

            break;

         case dataTypePoly:
            if (data->getPolyList()->hasPoints())
            {
#ifdef UseOldTransform
               CExtent polyExtent = PolyExtents(this,data->getPolyList(),scale,mirror & MIRROR_FLIP,insert_x,insert_y,&m,TRUE);
#else
               CPolyList polyList(*(data->getPolyList()));
               polyList.transform(matrix,this);

               CExtent polyExtent = polyList.getExtent(*this);
#endif

               if (polyExtent.isValid())
               {
                  extent.update(polyExtent);
                  //extent.assertFloat();

                  extentCount++;
               }
            }

            break;

         case dataTypeText:
            {
					double penWidth = getWidthTable()[data->getText()->getPenWidthIndex()]->getSizeA();
               CExtent textExtent = data->getText()->getExtent(spaceRatio,matrix,penWidth);
               extent.update(textExtent);
               extentCount++;
            }

            //if (TextExtents(this, data, &extents, scale, mirror & MIRROR_FLIP, insert_x, insert_y, rotation))
            //{
            //   extent.update(extents.left, extents.bottom);
            //   extent.update(extents.right, extents.top);
            //   //extent.assertFloat();

            //   extentCount++;
            //}

            break;  

         case dataTypeInsert:
            {
               // calculate point

					// Removed as per Case #845...  Centroids should be included in extents calculations.
               //if (data->getInsert()->getInsertType() == insertTypeCentroid)
					//	continue;

#ifdef UseOldTransform
               Point2 point2;
               point2.x = data->getInsert()->getOriginX() * scale;

               if (mirror & MIRROR_FLIP)
                  point2.x = -point2.x;

               point2.y = data->getInsert()->getOriginY() * scale;
               TransPoint2(&point2, 1, &m, insert_x, insert_y);
#else
               CPoint2d point2 = data->getInsert()->getOrigin2d();
               point2.transform(matrix);
#endif

               BOOL 
                  DrawAp = FALSE, 
                  DrawSubblock = FALSE,
                  IsTool = (subblock->getFlags() & BL_TOOL) || (subblock->getFlags() & BL_BLOCK_TOOL),
                  IsAp   = (subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE);
               double 
                  _x     = point2.x,
                  _y     = point2.y,
                  _rot   = rotation,
                  _scale = scale;
               int 
                  _mirror = mirror;
               int 
                  _layer = layer;

               // TOOL
               if (IsTool)
               {
                  if (subblock->getToolDisplay()) // use another block's graphics to display this tool
                  {
                     subblock = getBlockDirectory().getAt(subblock->getToolBlockNumber());

                     if ((subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE))
                        DrawAp = TRUE;
                     else
                        DrawSubblock = TRUE;
                  }
                  else // draw generic tool
                  {
                     extent.update(_x - subblock->getToolSize() / 2,_y - subblock->getToolSize() / 2);
                     extent.update(_x + subblock->getToolSize() / 2,_y + subblock->getToolSize() / 2);
                     //extent.assertFloat();

                     extentCount++;
                  }
               } // end TOOL

               // APERTURE
               if (IsAp || DrawAp)
               {
                  // COMPLEX APERTURE
                  if (subblock->getShape() == apertureComplex)
                  {
                     // subblock is complex aperture insert (only looking at block's values ex. sizeA & rotation)

                     if (mirror & MIRROR_FLIP)
                        _rot = rotation - subblock->getRotation() - data->getInsert()->getAngle();
                     else
                        _rot = rotation + subblock->getRotation() + data->getInsert()->getAngle();
      
                     // now change subblock to aperture definition block (looking at block definition ex. polys & circles)
                     subblock = getBlockDirectory().getAt((int)(subblock->getSizeA()));
                     DrawSubblock = TRUE;
                  }     
                  else // NORMAL APERTURE
                  {
                     if (subblock->getShape() == apertureUndefined)
                     {
                        extent.update(data->getInsert()->getOriginX(), data->getInsert()->getOriginY());
                        //extent.assertFloat();

                        extentCount++;
                     }
                     else
                     {
                        //CPolyList* polylist = ApertureToPoly(subblock, data->getInsert(), mirror);
                        CPolyList* polylist = subblock->getAperturePolys(*this);
                        CTMatrix insertMatrix = data->getInsert()->getTMatrix();
                        polylist->transform(insertMatrix,this);

                        if (polylist != NULL && polylist->hasPoints())
                        {
#ifdef UseOldTransform
                           extents.left  = extents.bottom =  FLT_MAX;
                           extents.right = extents.top    = -FLT_MAX;

                           if (PolyExtents(this, polylist, &extents, scale, mirror & MIRROR_FLIP, insert_x, insert_y, &m, TRUE))
                           {
                              extent.update(extents.left ,extents.bottom);
                              extent.update(extents.right,extents.top   );
                              //extent.assertFloat();

                              extentCount++;
                           }
#else
                           polylist->transform(matrix,this);

                           CExtent polyExtent = polylist->getExtent(*this);
                           extent.update(polyExtent);
                           extentCount++;
#endif

                           delete polylist;
                        }
                     }
                  } // end NORMAL APERTURE
               } // end APERTURE
      
               // GRAPHIC BLOCK
               if (!IsAp && !IsTool) 
               {
                  DrawSubblock = TRUE;

                  if (mirror & MIRROR_FLIP)
                     _rot = rotation - data->getInsert()->getAngle();
                  else
                     _rot = rotation + data->getInsert()->getAngle();

                  _mirror = mirror ^ data->getInsert()->getMirrorFlags();
                  _scale  =  scale * data->getInsert()->getScale();
               }

               // Draw Block
               if (DrawSubblock)
               {
                  if (subblock->getDataList().IsEmpty())
                  {     
                     extent.update(_x,_y);
                     //extent.assertFloat();

                     extentCount++;
                  }
                  else
                  {
                     double xmin, xmax, ymin, ymax;
                     xmin = ymin =  FLT_MAX;
                     xmax = ymax = -FLT_MAX;

                     //if (extentCount < 1)
                     //{
                     //   int iii = 3;
                     //}

                     // use extents if at a 90 degree angle
                     if (abs(DcaRound(radiansToDegrees(_rot)) % 90) < 2)
                     {
                        validateBlockExtents(*subblock,checkOnlyVisibleEntitiesFlag);
      
                        if (subblock->extentIsValid())
                        {
                           if (UseInsertExtents(&xmin, &xmax, &ymin, &ymax, subblock, _x, _y, _rot, _mirror, _scale))
                           {
                              extent.update(xmin,ymin);
                              extent.update(xmax,ymax);
                              //extent.assertFloat();

                              extentCount++;
                           }
                        }
                     }
                     else
                     {
                        if (block_extents(this, &xmin, &xmax, &ymin, &ymax, &(subblock->getDataList()), _x, _y, _rot, _mirror, _scale, layer,checkOnlyVisibleEntitiesFlag))
                        {
                           extent.update(xmin,ymin);
                           extent.update(xmax,ymax);
                           //extent.assertFloat();

                           extentCount++;
                        }
                     }
                  }
               } // end Draw Block

               // check visible attribs
               if (getShowAttribs() && CFontList::fontIsLoaded() && data->getAttributesRef() && !data->getAttributesRef()->IsEmpty())
               {
                  CExtent attribExtents = VisibleAttribsExtents(this,data->getAttributesRef(),&point2,_scale,_rot,_mirror);

                  if (attribExtents.isValid())
                  {
                     extent.update(attribExtents);
                     //extent.assertFloat();

                     extentCount++;
                  }
               }
            }

            break;
         }
      }

		if (debugFlag && messageFilter != NULL)
		{
			messageFilter->formatMessage("EntityNumber = %d\txMin = %0.3f\txMax = %0.3f\tyMin = %0.3f\tyMax = %0.3f\n",
					data->getEntityNumber(), extent.getXmin(), extent.getXmax(), extent.getYmin(), extent.getYmax());
		}
   }

   callLevel--;

   //if (extentCount > 0)
   //{
   //   if (! extent.isFloat())
   //   {
   //      int iii = 3;
   //   }
   //}

   return extent;
}
#else
CExtent CCamCadData::blockExtents(CDataList& DataList,double insert_x,double insert_y,double rotation,int mirror,double scale,
   int insertLayer,bool checkOnlyVisibleEntitiesFlag,CMessageFilter* messageFilter)
{
   return blockExtents(&DataList,insert_x,insert_y,rotation,mirror,scale,insertLayer,checkOnlyVisibleEntitiesFlag,messageFilter);
}
#endif

bool CCamCadData::validateBlockExtents(BlockStruct& block,bool checkOnlyVisibleEntitiesFlag)
{
   bool calculatedFlag = ! block.extentIsValid();

   if (calculatedFlag)
   {
      block.calculateBlockExtents(*this,checkOnlyVisibleEntitiesFlag);
   }

   return calculatedFlag;
}
   

bool CCamCadData::isFloatingLayer(int layerIndex) const
{
   bool retval = true;

   if (layerIndex  >= 0 && layerIndex < getLayerArray().GetSize())
   {
      LayerStruct* layer = getLayerArray().GetAt(layerIndex);

      if (layer != NULL)
      {
         retval = layer->isFloating();
      }
   }

   return retval;
}

//BOOL CCEtoODBDoc::get_layer_visible_extents(int layernum)
bool CCamCadData::isLayerPairVisible(int layerIndex) const
{
   bool retval = false;

   if (layerIndex  >= 0 && layerIndex < getLayerArray().GetSize())
   {
      LayerStruct* layer = getLayerArray().GetAt(layerIndex);

      if (layer != NULL)
      {
         retval = layer->isVisible();

         if (!retval)
         {
            LayerStruct* mirroredLayer = getLayerArray().GetAt(layer->getMirroredLayerIndex());

            // If mirrored layer exists then use its visibility. If it does not exist then we
            // have some bad data, a mirror layer setting pointing to a non-existent layer.
            // Clear the bad setting by setting mirror to self. Sort of an odd thing to have a
            // "is visible" checker do, but this gets used during extent check right after reading
            // a ccz file, and before we get to the "verifyAndRepair" function, so we can't wait 
            // for verifyAndRepair to do it. So we just do it.
            if (mirroredLayer != NULL)
               retval = mirroredLayer->isVisible();
            else
               layer->setMirroredLayerIndex(layer->getLayerIndex());
         }
      }
   }

   return retval;
}

bool CCamCadData::isLayerVisible(int layerIndex,int mirrorFlags) const
{
   bool retval = false;

   if (layerIndex  >= 0 && layerIndex < getLayerArray().GetSize())
   {
      LayerStruct* layer = getLayerArray().GetAt(layerIndex);

      if (layer != NULL)
      {
         retval = layer->getVisible();

         if (mirrorFlags > -1)
         {
            if ((mirrorFlags & MIRROR_LAYERS) != 0)
            {
               LayerStruct& mirroredLayer = layer->getMirroredLayer();

               retval = mirroredLayer.getVisible();

               if (!editingGeometry() && mirroredLayer.getNeverMirror())
               {
                  retval = false;
               }
            }
            else
            {
               if (!editingGeometry() && layer->getMirrorOnly())
               {
                  retval = false;
               }
            }
         }
      }
   }

   return retval;
}

DataStruct* CCamCadData::getNewDataStruct(DataTypeTag dataType,int entityNumber)
{
   allocateEntityNumber(entityNumber);

   DataStruct* data = new DataStruct(dataType,entityNumber);

   return data;
}

DataStruct* CCamCadData::getNewDataStruct(DataStruct& other,bool copyAttributesFlag,int entityNumber)
{
   allocateEntityNumber(entityNumber);

   DataStruct* data = new DataStruct(other,entityNumber,copyAttributesFlag);

   return data;
}

DataStruct* CCamCadData::insertBlock(BlockStruct& childBlock,InsertTypeTag insertType,
   const CString& refname,int layerIndex,double x,double y,double angleRadians,bool mirror,double scale)
{
   DataStruct* data = getNewDataStruct(dataTypeInsert);
   InsertStruct* insert = data->getInsert();

   // If layerIndex is -1 the block elements are placed on the layers which they are
   // defined on. If layerIndex > -1, all entities on floating layers are put on that layer.
   data->setLayerIndex(layerIndex);

   insert->getShadingRef().On = FALSE;
   insert->setOriginX(x);
   insert->setOriginY(y);

   insert->setAngle(angleRadians);

   insert->setMirrorFlags(mirror ? MIRROR_ALL : 0); // normal mirroring does both. If you want different for an insert, change ->mirror after call to Graph_Block_Ref().  see DBUTIL.H
   insert->setPlacedBottom(mirror); // normal mirroring does both. If you want different for an insert, change ->mirror after call to Graph_Block_Ref().  see DBUTIL.H
   insert->setScale(scale);
   insert->setBlockNumber(childBlock.getBlockNumber()); 
   insert->setInsertType(insertType);
   insert->setRefname(refname);

   // inherit block's attributes

   if (childBlock.getAttributesRef() != NULL)
   {
      WORD keyword;
      Attrib* b_ptr;
      Attrib* i_ptr;
      CAttributeMapWrap dataAttribMap(data->getAttributesRef());

      for (POSITION pos = childBlock.getAttributesRef()->GetStartPosition();pos != NULL;)
      {
         childBlock.getAttributesRef()->GetNextAssoc(pos, keyword,b_ptr);

         i_ptr = b_ptr->allocateCopy();
         i_ptr->setInherited(true); // set the inherited flag

         dataAttribMap.addInstance(keyword,i_ptr);
      }
   }

   return data;
}

// use CPolyList::getExtent()
//CExtent CCamCadData::getPolyExtents(CPolyList& polylist,const CTMatrix& matrix,bool useWidth) const
//{
//}

DataStruct* CCamCadData::newPolyStruct(int layerIndex,GraphicClassTag graphicClass,DbFlag flags,bool negative)
{
   if (layerIndex < 0)
   {
      layerIndex = 0;
   }

   DataStruct* data = new DataStruct(dataTypePoly,allocateEntityNumber());

   data->setLayerIndex(layerIndex);
   data->setFlags(flags);
   data->setNegative(negative);
   data->setGraphicClass(graphicClass);

   return data;
}

DataStruct* CCamCadData::addPolyStruct(BlockStruct& parentBlock,int layerIndex,GraphicClassTag graphicClass,DbFlag flags,bool negative)
{
   DataStruct* data = newPolyStruct(layerIndex,graphicClass,flags,negative);

   parentBlock.getDataList().AddTail(data);

   return data;
}

DataStruct* CCamCadData::addPolyStruct(CDataList& parentDataList,int layerIndex,GraphicClassTag graphicClass,DbFlag flags,bool negative)
{
   DataStruct* data = newPolyStruct(layerIndex,graphicClass,flags,negative);

   parentDataList.AddTail(data);

   return data;
}

CPoly* CCamCadData::addPoly(DataStruct& polyStruct,int widthIndex,bool Filled,bool VoidPoly,bool Closed)
{
   if (widthIndex < 0)
   {
      widthIndex = 0;
   }

   CPoly* poly = new CPoly;

   poly->setWidthIndex(widthIndex);
   poly->setFilled(Filled);
   poly->setVoid(VoidPoly);
   poly->setClosed(Closed || Filled);

   polyStruct.getPolyList()->AddTail(poly);

   return poly;
}

CPoly* CCamCadData::addClosedPoly(DataStruct& polyStruct,int widthIndex)
{
   return addPoly(polyStruct,widthIndex,false,false,true);
}

CPoly* CCamCadData::addOpenPoly(  DataStruct& polyStruct,int widthIndex)
{
   return addPoly(polyStruct,widthIndex,false,false,false);
}

CPoly* CCamCadData::addFilledPoly(DataStruct& polyStruct,int widthIndex)
{
   return addPoly(polyStruct,widthIndex,true,false,true);
}

CPnt* CCamCadData::addVertex(CPoly& poly,double x,double y,double bulge)
{
   // 572 is a bulge for 89.9 * 4 = 359.6
   // after this number, the bulge does into infinity.
   // infinity 1*e16 is for 90 degree * 4 = a full circle
   if (fabs(bulge) > 572)
   {
      bulge = 0.;  
      ASSERT(false);
   }

   CPnt* pnt = new CPnt(x,y,bulge);

   poly.getPntList().AddTail(pnt);

   return pnt;
}

int CCamCadData::getDefinedWidthIndex(double width)
{
   int widthIndex;

   if (width < 0.)
   {
      width = 0.;
   }

   BlockStruct* widthBlock = NULL;

   for (widthIndex = 0;widthIndex < m_widthTable.GetSize();widthIndex++)
   {
      widthBlock = m_widthTable.getAt(widthIndex);

      if (widthBlock != NULL)
      {
         if (widthBlock->getShape() == apertureRound &&
             fpeq(widthBlock->getSizeA(),width,SMALLNUMBER))
         {
            break;
         }
      }

      widthBlock = NULL;
   }

   if (widthBlock == NULL)
   {
      int exponent = CStandardApertureShape::getDefaultDescriptorDimensionFactorExponent(getPageUnits());
      CStandardApertureCircle roundAperture(getPageUnits(),exponent);
      roundAperture.setDiameter(width);
      CString apertureName = "AP_" + roundAperture.getDescriptor();

      widthBlock = getBlockDirectory().getAt(apertureName,-1,blockTypeUnknown);

      if (widthBlock != NULL)
      {
         if (! widthBlock->isAperture())
         {
            widthBlock = NULL;
         }
         else if (widthBlock->getShape() != apertureRound ||
                  !fpeq(widthBlock->getSizeA(),width,SMALLNUMBER))
         {
            widthBlock = NULL;
         }
      }

      if (widthBlock == NULL)
      {
          widthBlock = &(getBlockDirectory().getNewBlock(apertureName,-1,blockTypeUnknown));
          widthBlock->setFlags(BL_WIDTH | BL_APERTURE | BL_BLOCK_APERTURE);
          widthBlock->setSizeA(width);
          widthBlock->setShape(apertureRound);
      }

      widthIndex = m_widthTable.Add(widthBlock);
   }

   return widthIndex;
}

int CCamCadData::getDefinedApertureIndex(const CString& apertureName,ApertureShapeTag apertureShape,
   double sizeA,double sizeB,double xOffset,double yOffset,double rotationRadians,int fileNumber,DbFlag flags,int dCode)
{
   int apertureIndex;

   BlockStruct& apertureBlock = getDefinedAperture(apertureName,apertureShape,
                                    sizeA,sizeB,xOffset,yOffset,rotationRadians,fileNumber,flags,dCode,&apertureIndex);

   return apertureIndex;
}

BlockStruct& CCamCadData::getDefinedAperture(const CString& apertureName,ApertureShapeTag apertureShape,
   double sizeA,double sizeB,double xOffset,double yOffset,double rotationRadians,int fileNumber,DbFlag flags,int dCode,int* definedApertureIndex)
{
   CBlockList* apertureBlockList = m_blockDirectory.getListAt(apertureName,fileNumber);
   BlockStruct* apertureBlock = NULL;
   int apertureIndex = -1;

   if (apertureBlockList != NULL)
   {
      for (POSITION pos = apertureBlockList->getHeadPosition();pos != NULL;)
      {
         apertureBlock = apertureBlockList->getNext(pos);

         if (apertureBlock != NULL && apertureBlock->isAperture())
         {
            if (apertureBlock->getShape() == apertureShape &&
               fpeq(apertureBlock->getSizeA(),sizeA) && 
               fpeq(apertureBlock->getSizeB(),sizeB) && 
               fpeq(apertureBlock->getXoffset(),xOffset) && 
               fpeq(apertureBlock->getYoffset(),yOffset) && 
               fpeq(apertureBlock->getRotationRadians(),rotationRadians) )
            {
               apertureIndex = m_widthTable.getWidthIndex(apertureBlock);
               break;
            }
         }

         apertureBlock = NULL;
      }
   }

   if (apertureIndex < 0)
   {
      if (apertureBlock == NULL)
      {
         apertureBlock = &(getBlockDirectory().getNewBlock(apertureName,fileNumber,blockTypeUnknown));

         apertureBlock->setShape(apertureShape);
         apertureBlock->setSizeA(sizeA);
         apertureBlock->setSizeB(sizeB);
         apertureBlock->setXoffset(xOffset);
         apertureBlock->setYoffset(yOffset);
         apertureBlock->setRotationRadians(rotationRadians);
         apertureBlock->setDcode(dCode);
         apertureBlock->setFlags(BL_WIDTH | BL_APERTURE | BL_BLOCK_APERTURE);
         apertureBlock->setSpokeCount(0);
      }

      apertureIndex = m_widthTable.Add(apertureBlock);
   }

   if (definedApertureIndex != NULL)
   {
      *definedApertureIndex = apertureIndex;
   }

   return *apertureBlock;
}

double CCamCadData::getWidth(int widthIndex) const
{
   double width = 0.;
   BlockStruct* widthBlock = m_widthTable.getAt(widthIndex);

   if (widthBlock != NULL)
   {
      width = widthBlock->getSizeA();
   }

   return width;
}

double CCamCadData::convertToPageUnits(PageUnitsTag units,double value)
{
   return value * getUnitsFactor(units,getPageUnits());
}

//DataStruct* CCamCadData::addDataStruct(BlockStruct& block,DataTypeTag dataType,int layerIndex,GraphicClassTag graphicClass,DbFlag flags = 0,bool negative = false)
//{
//}

LayerStruct* CCamCadData::getLayer(int layerIndex) const
{
   LayerStruct* foundLayer = NULL;

   if (layerIndex >= 0 && layerIndex < m_layerArray.GetSize())
   {
      foundLayer = getLayerArray().GetAt(layerIndex);
   }

   return foundLayer;
}

int CCamCadData::getLayerIndex(CamCadLayerTag layerTag)
{
   return getDefinedLayer(layerTag).getLayerIndex();
}

LayerStruct& CCamCadData::getDefinedLayer(CamCadLayerTag layerTag)
{
   return m_layerArray.getDefinedLayer(layerTag);
}

LayerStruct& CCamCadData::getDefinedLayer(const CString& layerName,bool floatingFlag)
{
   return getDefinedLayer(layerName,floatingFlag,layerTypeUnknown);
}

LayerStruct& CCamCadData::getDefinedLayer(const CString& layerName,bool floatingFlag,LayerTypeTag layerType)
{
   LayerStruct* layer = m_layerArray.getLayer(layerName);

   if (layer == NULL)
   {
      layer = &(m_layerArray.getDefinedLayer(layerName));

      layer->setFloating(floatingFlag);
      layer->setLayerType(layerType);
   }

   return *layer;
}

int CCamCadData::getDefinedLayerIndex(const CString& layerName,bool floatingFlag)
{
   return getDefinedLayerIndex(layerName,floatingFlag,layerTypeUnknown);
}

int CCamCadData::getDefinedLayerIndex(const CString& layerName,bool floatingFlag,LayerTypeTag layerType)
{
   int layerIndex = getDefinedLayer(layerName,floatingFlag,layerType).getLayerIndex();

   return layerIndex;
}

int CCamCadData::removeDataFromDataListByGraphicClass(BlockStruct& block,GraphicClassTag graphicClass)
{
   int removedCount = 0;

   CDataList& dataList = block.getDataList();
   CDataList discardedDataList(true);

   for (POSITION pos = dataList.GetHeadPosition();pos != NULL;)
   {
      POSITION oldPos = pos;
      DataStruct* data = dataList.GetNext(pos);

      if (data->getGraphicClass() == graphicClass)
      {
         dataList.RemoveAt(oldPos);
         discardedDataList.AddTail(data);

         removedCount++;
      }
   }

   discardData(discardedDataList);

   return removedCount;
}

void CCamCadData::discardData(CDataList& dataList) const
{
   dataList.empty();
}

bool CCamCadData::showAttributes() const
{
   return false;
}

bool CCamCadData::editingGeometry() const
{
   return false;
}


DataStruct* CCamCadData::addText(BlockStruct& parentBlock,
   int layerIndex,const CString& text,double x,double y,double height,double charWidth,double angle,DbFlag flag,
   bool proportional,bool mirror,int oblique,bool negative,int penWidthIndex,bool specialChar)
{
   return addText(parentBlock.getDataList(), layerIndex, text, x, y, height, charWidth, angle, flag, proportional, mirror, oblique, negative, penWidthIndex, specialChar);
}

DataStruct* CCamCadData::addText(CDataList& parentDataList,
   int layerIndex,const CString& text,double x,double y,double height,double charWidth,double angle,DbFlag flag,
   bool proportional,bool mirror,int oblique,bool negative,int penWidthIndex,bool specialChar)
{
   DataStruct* textData = NULL;

   if (layerIndex < 0)
   {
      layerIndex = 0;
   }

   if (!text.IsEmpty())
   {
      CString textString;

      if (!specialChar)
      {
         for (const char* p = text;*p != '\0';p++)
         {
            textString += *p;

            if (*p == '%') textString += *p;
         }
      }
      else
      {
         textString = text;
      }

      textData = new DataStruct(dataTypeText,allocateEntityNumber());
      TextStruct* textStruct = textData->getText();

      parentDataList.AddTail(textData);

      textData->setLayerIndex(layerIndex);
      textData->setFlags(flag);
      textData->setNegative(negative);

      // if no widhtindex supplied (-1) take 1/10 of the height as the penwidth
      if (penWidthIndex < 0)
      {
         double width = .1 * height;
         penWidthIndex = getDefinedWidthIndex(width);
      }

      textStruct->setPenWidthIndex(penWidthIndex);

      textStruct->setText(textString);
      textStruct->setPnt(x,y);
      textStruct->setHeight(height);
      textStruct->setWidth(charWidth);    // character width
      textStruct->setProportionalSpacing(proportional);
      textStruct->setRotation(angle);        // angle in Radians
      textStruct->setMirrored(mirror);
      textStruct->setOblique(oblique);
      textStruct->setFontNumber(0);
      textStruct->setMirrorDisabled(false);
      //textStruct->setVerticalPosition(intToVerticalPositionTag(lineAlignment));
      //textStruct->setHorizontalPosition(intToHorizontalPositionTag(textAlignment));
   }

   return textData;
}

int CCamCadData::purgeUnusedBlocks(CWriteFormat* reportWriteFormat)
{
   int purgedCount = 0;

   CMapBlockNumberToBlock map(nextPrime2n(16000),false); // map of used block numbers

   BlockStruct* mappedBlock;

   CWaitCursor wait;
   CString intStr;
   int i = 0;

   // mark width blocks as used
   for (i=0;i < getNextWidthIndex();i++)
   {
      BlockStruct* block = getWidthTable()[i];

      if (block == NULL)
         continue;

      mapUsedBlocks(map,*block);
   }

   // mark file blocks as used and recurse into them   
   for (POSITION pos = getFileList().GetHeadPosition();pos != NULL;)
   {
      FileStruct *file = getFileList().GetNext(pos);

      mapUsedBlocks(map,*(file->getBlock()));

      if (file->getOriginalBlock() != NULL)
      {
         mapUsedBlocks(map,*(file->getOriginalBlock()));
      }
   }

   // purge unused blocks
   for (i = 0; i < getMaxBlockIndex(); i++)
   {
      BlockStruct* block = getBlockAt(i);

      if (block == NULL) continue;

      if (!map.Lookup(block->getBlockNumber(),mappedBlock))
      {
         if (reportWriteFormat != NULL)
         {
            reportWriteFormat->writef("Unused Geometry purged : [%s] [%d]\n",block->getName(),block->getFileNumber());
         }

         purgedCount++;
         //getBlockArray().deleteBlock(block);
         delete block;
      }
   }

   // remove types that point to a freed block or don't point to any block
   for (POSITION filePos = getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* file = getFileList().GetNext(filePos);
      TypeStruct* type;
      
      for (POSITION typePos = file->getTypeList().GetHeadPosition();typePos != NULL;)
      {
         POSITION oldTypePos = typePos;

         type = file->getTypeList().GetNext(typePos);

         if (type->getBlockNumber() < 0 || getBlockAt(type->getBlockNumber()) == NULL)
         {
            if (reportWriteFormat != NULL)
            {
               reportWriteFormat->writef("Device type with no block reference removed : [%s] [%d]\n",type->getName(),type->getBlockNumber());
            }

            file->getTypeList().deleteAt(oldTypePos);
            //delete type;
         }
      }
   }

   if (reportWriteFormat != NULL)
   {
      reportWriteFormat->writef("\nTotal of %d Geometries purged.\n\n", purgedCount);
   }

   return purgedCount;
}

int CCamCadData::purgeUnusedWidths(CWriteFormat* reportWriteFormat)
{
   int purgedCount = 0;

   CMapBlockNumberToBlock map(nextPrime2n(1000),false); // map of used widths

   BlockStruct* mappedBlock;
   CWaitCursor wait;
   CString intStr;

   for (int blockIndex = 0;blockIndex < this->getMaxBlockIndex();blockIndex++)
   {
      BlockStruct* block = getBlockAt(blockIndex);

      if (block == NULL)
         continue;

      mapUsedWidths(map,*block);
   }

   BlockStruct* width0 = ((getWidthTable().GetSize() > 0) ? getWidthTable().GetAt(0) : NULL);

   // purge unused blocks (never purge index 0)
   // Never purge small width or zero width when their index is set in the width table,
   // regardless of whether they are used or not.
   int smallWidthIndex = getWidthTable().getSmallWidthIndex();
   int zeroWidthIndex = getWidthTable().getZeroWidthIndex();
   for (int widthIndex = 1;widthIndex < getWidthTable().GetSize();widthIndex++)
   {
      if (widthIndex == smallWidthIndex)
         continue;

      if (widthIndex == zeroWidthIndex)
         continue;

      BlockStruct* width = getWidthTable().GetAt(widthIndex);

      if (width == NULL)
         continue;

      if (!map.Lookup(width->getBlockNumber(),mappedBlock))
      {
         if (width != width0)
         {
            if (reportWriteFormat != NULL)
            {
               reportWriteFormat->writef("Unused Width purged : [%s] [%d]\n", width->getName(), width->getFileNumber());
            }

            purgedCount++;

            getWidthTable().SetAt(widthIndex,width0);
         }
      }
   }

   if (reportWriteFormat != NULL)
   {
      reportWriteFormat->writef("\nTotal of %d Widths purged.\n\n", purgedCount);
   }

   return purgedCount;
}

int CCamCadData::purgeUnusedWidthsAndBlocks(CWriteFormat* reportWriteFormat)
{
   int purgedCount = 0;

   for (int pass = 1;pass < 5;pass++)
   {
      if (reportWriteFormat != NULL)
      {
         reportWriteFormat->writef("\nPurging unused widths and blocks pass %d\n\n",pass);
      }

      int purgedWidthCount = purgeUnusedWidths(reportWriteFormat);
      int purgedBlockCount = purgeUnusedBlocks(reportWriteFormat);

      purgedCount += purgedWidthCount + purgedBlockCount;

      if (purgedWidthCount == 0 && purgedBlockCount == 0)
      {
         break;
      }
   }

   return purgedCount;
}
                                  
void CCamCadData::mapUsedBlocks(CMapBlockNumberToBlock& map,BlockStruct& block)
{  
   BlockStruct* mappedBlock;

   if (!map.Lookup(block.getBlockNumber(),mappedBlock))
   {
      map.setAt(&block);
      BlockStruct* subBlock;

      if (block.isAperture())
      {
         if (block.getShape() == apertureComplex)
         {
            int subBlockNumber = block.getComplexApertureSubBlockNumber();

            subBlock = getBlockAt(subBlockNumber);

            if (subBlock != NULL)
            {
               mapUsedBlocks(map,*subBlock);
            }
         }
      }

      for (CDataListIterator insertIterator(block,dataTypeInsert);insertIterator.hasNext();)
      {
         DataStruct* data = insertIterator.getNext();
         int blockNumber = data->getInsert()->getBlockNumber();

         subBlock = getBlockAt(blockNumber);

         if (subBlock != NULL)
         {
            mapUsedBlocks(map,*subBlock);
         }
      }
   }
}
                                  
void CCamCadData::mapUsedWidths(CMapBlockNumberToBlock& map,BlockStruct& block)
{  
   for (POSITION pos = block.getHeadDataPosition();pos != NULL;)
   {
      DataStruct* data = block.getNextData(pos);

      if (data->getDataType() == dataTypePoly)
      {
         CPolyList* polyList = data->getPolyList();

         for (POSITION polyPos = polyList->GetHeadPosition();polyPos != NULL;)
         {
            CPoly* poly = polyList->GetNext(polyPos);

            BlockStruct& width = getDefinedWidthBlock(poly->getWidthIndex());
            map.setAt(&width);
         }
      }
      else if (data->getDataType() == dataTypeText)
      {
         BlockStruct* width = getWidthTable().GetAt(data->getText()->getPenWidthIndex());
         map.setAt(width);
      }
      else if (data->getDataType() == dataTypeInsert)
      {
         BlockStruct* subBlock = getBlockAt(data->getInsert()->getBlockNumber());

         if (subBlock->isAperture() || subBlock->isTool())
         {
            map.setAt(subBlock);
         }
      }
   }
}

int CCamCadData::generatePinLocations(CWriteFormat* reportWriteFormat, CDcaOperationProgress* progress) 
{
   int errorsWrittenCount = 0;
            
   CWaitCursor wait;

   generatePadstackAccessFlag(false,NULL);
   
   for (POSITION pos = getFileList().GetHeadPosition();pos != NULL;)
   {
      FileStruct* file = getFileList().GetNext(pos);

      if (file->getBlockType() == blockTypePcb)
      {
         if (reportWriteFormat != NULL)
         {
            reportWriteFormat->writef("Processing pin locations for file %d.\n",file->getFileNumber());
         }

         errorsWrittenCount += generatePinLocation(file->getFileNumber(),true, reportWriteFormat, progress);  // do it always

         // here report which pins did not make it.
         if (reportWriteFormat != NULL)
         {
            for (POSITION netPos = file->getNetList().GetHeadPosition();netPos != NULL;)
            {
               NetStruct* net = file->getNetList().GetNext(netPos);

               for (POSITION compPinPos = net->getHeadCompPinPosition();compPinPos != NULL;)
               {
                  CompPinStruct* compPin = net->getNextCompPin(compPinPos);

                  if (!compPin->getPinCoordinatesComplete())
                  {
                     errorsWrittenCount++;

                     reportWriteFormat->writef("Error: Component [%s] Pin [%s] in Net [%s] could not be found in Design.\n",
                        compPin->getRefDes(),compPin->getPinName(),net->getNetName());
                  }
               }
            }
         }
      }
   }

   return errorsWrittenCount;
}

int CCamCadData::generatePinLocation(int fileNumber,bool recalculateFlag,CWriteFormat* reportWriteFormat,CDcaOperationProgress* progress)
{
   int errorsWrittenCount = 0;

   FileStruct* file = getFile(fileNumber);

   if (file != NULL && file->getBlock() != NULL)
   {
      CWaitCursor wait;      

      if (progress != NULL)
      {
         progress->setLength(file->getBlock()->getDataList().GetCount());
         progress->updateProgress(0.);
      }

      for (POSITION compDataPos = file->getBlock()->getDataList().GetHeadPosition();compDataPos != NULL;)
      {
         if (progress != NULL)
         {
            progress->incrementProgress();
         }

         DataStruct* compData = file->getBlock()->getDataList().GetNext(compDataPos);

         if (compData->getDataType() != dataTypeInsert)
            continue;

         if (compData->getInsert()->getInsertType() != insertTypePcbComponent && compData->getInsert()->getInsertType() != insertTypeTestPoint
            && !compData->isInsertType(insertTypeDie))
            continue;

         CString compName = compData->getInsert()->getRefname();

         BlockStruct* compGeom = getBlock(compData->getInsert()->getBlockNumber());

         if (compGeom == NULL)
         {
            if (reportWriteFormat != NULL)
            {
               reportWriteFormat->writef("Bad Geometry Reference : Comp[%s] to Geometry #%d\n",
                  compName.GetBuffer(0),compData->getInsert()->getBlockNumber());

               errorsWrittenCount++;
            }

            continue;
         }

         double insert_x = compData->getInsert()->getOriginX();
         double insert_y = compData->getInsert()->getOriginY();
         double insert_rot = compData->getInsert()->getAngle();
         bool insert_mirror       = ((compData->getInsert()->getMirrorFlags() & MIRROR_FLIP  ) != 0);  // flips koos
         bool insert_mirror_layer = ((compData->getInsert()->getMirrorFlags() & MIRROR_LAYERS) != 0);  // mirrors layers
         double scale = 1;
      
         // has no reference designator
         if (compName.IsEmpty())
            continue;

         // here loop thru block (component definition) and find PINPOS     
         for (POSITION pinDataPos = compGeom->getDataList().GetHeadPosition();pinDataPos != NULL;)
         {
            DataStruct* pinData = compGeom->getDataList().GetNext(pinDataPos);

            if (pinData->getDataType() != dataTypeInsert)
               continue;

            if (pinData->getInsert()->getInsertType() != insertTypePin && !pinData->isInsertType(insertTypeDiePin))
               continue;

            CString pinName = pinData->getInsert()->getRefname();

            if (pinName.IsEmpty())
               continue;

            //CompPinStruct *cp = check_add_comppin(compName, pinName, file);
            CompPinStruct* cp = &(file->getNetList().getDefinedCompPin(compName,pinName));

            if (!cp->getPinCoordinatesComplete() || recalculateFlag)
            {
               Point2 point2;
               Mat2x2 m;
               RotMat2(&m, insert_rot);

               point2.x = pinData->getInsert()->getOriginX() * scale;
               if (insert_mirror)
                  point2.x = -point2.x;
               point2.y = pinData->getInsert()->getOriginY() * scale;
               TransPoint2(&point2, 1, &m, insert_x, insert_y);
                                         
               cp->setPinCoordinatesComplete(true);
               cp->setOrigin(point2.x,point2.y);
               cp->setRotationRadians( insert_rot + pinData->getInsert()->getAngle());   // pin rotation plus comp rotation.
               cp->setMirror( compData->getInsert()->getMirrorFlags() ^ pinData->getInsert()->getMirrorFlags());
            }

            if (cp->getPadstackBlockNumber() < 0 || recalculateFlag)
            {
               cp->setPadstackBlockNumber( pinData->getInsert()->getBlockNumber());
            }

            if (!cp->getVisible() || recalculateFlag)
            {
               BlockStruct* pinGeom = getBlock(pinData->getInsert()->getBlockNumber());

               if (pinGeom == NULL)
               {
                  if (reportWriteFormat != NULL)
                  {
                     reportWriteFormat->writef("Bad Geometry Reference : Pin[%s] to Geometry #%d\n",
                        pinName,pinData->getInsert()->getBlockNumber());

                     errorsWrittenCount++;
                  }

                  continue;
               }

               bool accessTop        = ((pinGeom->getFlags() & BL_ACCESS_TOP        ) != 0);
               bool accessBottom     = ((pinGeom->getFlags() & BL_ACCESS_BOTTOM     ) != 0);
               bool accessTopOnly    = ((pinGeom->getFlags() & BL_ACCESS_TOP_ONLY   ) != 0);
               bool accessBottomOnly = ((pinGeom->getFlags() & BL_ACCESS_BOTTOM_ONLY) != 0);

               bool isVisibleTop,isVisibleBottom;

               bool pinLayerMirrored = pinData->getInsert()->getLayerMirrored();
               LayerStruct* pinLayer = getUltimateLayer(*pinData);
               bool layerHasMirror = pinLayer != NULL ? (pinLayer->getMirroredLayerIndex() != pinLayer->getLayerIndex()) : false;

               if (!insert_mirror_layer || !layerHasMirror) // component insert is not mirrored
               {
                  if (pinLayerMirrored && layerHasMirror)
                  {
                     isVisibleTop    = accessBottom;
                     isVisibleBottom = accessTop || accessBottomOnly;
                  }
                  else
                  {
                     isVisibleTop    = accessTop || accessTopOnly;
                     isVisibleBottom = accessBottom;
                  }
               }
               else  // component insert is mirrored
               {
                  if (pinLayerMirrored && layerHasMirror)
                  {
                     isVisibleTop    = accessTop || accessTopOnly;
                     isVisibleBottom = accessBottom;
                  }
                  else
                  {
                     isVisibleTop    = accessBottom;
                     isVisibleBottom = accessTop || accessBottomOnly;
                  }
               }

               if ((pinGeom->getFlags() & BL_ACCESS_NONE) != 0)
               {
                  cp->setVisible(visibleNone);
               }
               else if (isVisibleTop && isVisibleBottom)
               {
                  cp->setVisible(visibleBoth);
               }
               else if (isVisibleTop)
               {
                  cp->setVisible(visibleTop);
               }
               else if (isVisibleBottom)
               {
                  cp->setVisible(visibleBottom);
               }
               else
               {
                  cp->setVisible(visibleBoth);
               }
            }

            cp->attributes().CopyAll(pinData->attributes());
         }
      }
   }

   return errorsWrittenCount;
}

LayerStruct* CCamCadData::getUltimateLayer(DataStruct& insertData)
{
   // We're expecting the insertData at first call to be a padstack.
   // We're looking for the layer used by a pad, any pad. 
   // There could be more than one pad and one layer involved, but we aren't being that thorough.
   // For our purposes here we want a sample, the first one found will do.

   LayerStruct* layer = getLayer(insertData.getLayerIndex());

   if (layer == NULL)
   {
      int blockNumber = insertData.getInsert()->getBlockNumber();
      BlockStruct* block = getBlock(blockNumber);

      if (block != NULL)
      {
         for (CDataListIterator insertIterator(*block,dataTypeInsert);insertIterator.hasNext();)
         {
            DataStruct* data = insertIterator.getNext();

            layer = getUltimateLayer(*data);

            if (layer != NULL)
            {
               break;
            }
         }
      }
   }

   return layer;
}

// this function updates the netlist with the NC non connect net.
// it loop through all component (INSERTPCBCOMPONENT) and inserts
// all INSERTPIN from 
void CCamCadData::generateNoConnectNets(int fileNumber,CDcaOperationProgress* progress)
{
   FileStruct* fileStruct = getFile(fileNumber);

   if (fileStruct != NULL)
   {
      // loop thru the PCB file
      DataStruct* data;
      const CDataList& fileDataList = fileStruct->getBlock()->getDataList();
      CNetList& netList = fileStruct->getNetList();
      NetStruct& unusedNet = netList.getUnusedPinsNet();

      if (progress != NULL)
      {
         progress->setLength(fileDataList.GetCount());
      }
      
      for (POSITION pos = fileDataList.GetHeadPosition();pos != NULL;)
      {
         data = fileDataList.GetNext(pos);

         if (progress != NULL)
         {
            progress->incrementProgress();
         }
         
         if (data->getDataType() == dataTypeInsert)
         {
            if (data->getInsert()->getInsertType() == insertTypePcbComponent ||
                data->getInsert()->getInsertType() == insertTypeTestPoint )
            {
               BlockStruct* block = getBlock(data->getInsert()->getBlockNumber());
               CString refDes = data->getInsert()->getRefname();

               // here loop thru block and find INSERTPIN
               for (CDataListIterator pinIterator(*block,insertTypePin);pinIterator.hasNext();)
               {
                  DataStruct* pinData = pinIterator.getNext();

                  CString pinName = pinData->getInsert()->getRefname();
                  //CString pinRef  = CompPinStruct::getPinRef(refDes,pinName);

                  CompPinStruct* compPin = netList.getCompPin(refDes,pinName);

                  if (compPin == NULL)
                  {
                     unusedNet.addCompPin(refDes,pinName);
                  }
               }

               //DataStruct *bp;
               //POSITION bpos = block->getDataList().GetHeadPosition();
               //while (bpos != NULL)
               //{
               //   bp = block->getDataList().GetNext(bpos);
      
               //   if (bp->getDataType() != T_INSERT)        continue;

               //   switch (bp->getDataType())
               //   {
               //      case T_INSERT:
               //      {
               //         if (bp->getInsert()->getInsertType() == INSERTTYPE_PIN)
               //         {
               //            // here is a pin
               //            CString pinname = bp->getInsert()->getRefname();
               //            check_add_comppin(compname,pinname,f);
               //         }
               //      }
               //   }
               //} // end loop thru block
            }
         }
      } // for
   }
}

void CCamCadData::generatePadstackAccessFlag(bool alwaysFlag,CDcaOperationProgress* progress)
{
   if (progress != NULL)
   {
      progress->updateStatus("Calculating padstack access attributes",getMaxBlockIndex());
   }

   for (int blockIndex =0 ;blockIndex < getMaxBlockIndex();blockIndex++)
   {
      BlockStruct* block = getBlock(blockIndex);

      if (progress != NULL)
      {
         progress->incrementProgress();
      }

      if (block == NULL)
         continue;

      if (block->getBlockType() != blockTypePadstack)
         continue;

      if (!alwaysFlag)
      {
         int accessMask = (BL_ACCESS_TOP | BL_ACCESS_BOTTOM | BL_ACCESS_NONE | BL_ACCESS_TOP_ONLY | BL_ACCESS_BOTTOM_ONLY);

         if ((block->getFlags() & accessMask) != 0)
         {
            continue;
         }
      }

      block->setFlags(0);

      int flag = getPadstackAccessLayer(block->getDataList());

      if (flag == padStackAccessNone)
         block->setFlagBits(BL_ACCESS_NONE);

      if ((flag & padStackAccessTop) != 0)
         block->setFlagBits(BL_ACCESS_TOP);

      if ((flag & padStackAccessBottom) != 0)
         block->setFlagBits(BL_ACCESS_BOTTOM);

      if ((flag & padStackAccessTopOnly) != 0)
         block->setFlagBits(BL_ACCESS_TOP_ONLY);

      if ((flag & padStackAccessBottomOnly) != 0)
         block->setFlagBits(BL_ACCESS_BOTTOM_ONLY);
   }
}

int CCamCadData::getPadstackAccessLayer(CDataList& dataList)
{
#ifdef EnableRecursiveReferenceChecking
   static CDataList dataStack(false);
#endif

   int layerFlag = 0;   

   for (POSITION dataPos = dataList.GetHeadPosition();dataPos != NULL;)
   {
      DataStruct* data = dataList.GetNext(dataPos);

#ifdef EnableRecursiveReferenceChecking
      // check for recursive referencing
      if (dataStack.Find(data) != NULL)
      {
         int blockNumber = -1;
         CString blockName;

         if (data->getDataType() == dataTypeInsert)
         {
            blockNumber = data->getInsert()->getBlockNumber();

            BlockStruct* block = getBlock(blockNumber);

            blockName = block->getName();
         }

         getApp().getMessageFilterTypeMessage().formatMessageBox("Recursive block reference found in '%s', blockNumber=%d, blockName='%s'",__FUNCTION__,blockNumber,blockName);

         continue;
      }

      dataStack.AddTail(data);
#endif
      
      if (data->getDataType() == dataTypeInsert)
      {
         switch (data->getInsert()->getInsertType())
         {
         case insertTypeThermalPad:
         case insertTypeClearancePad:
         case insertTypeObstacle:
         case insertTypeDrillSymbol:
            continue;
         }
      }

      LayerStruct* layer = getLayer(data->getLayerIndex());

      if (layer != NULL && !layer->isFloating())
      {
         switch (layer->getLayerType())
         {
         case layerTypePadTop:
         case layerTypeSignalTop:
            layerFlag |= ((layer->getNeverMirror()) ? padStackAccessTopOnly : padStackAccessTop);

            break;
         case layerTypePadBottom:
         case layerTypeSignalBottom:
            layerFlag |= ((layer->getMirrorOnly()) ? padStackAccessBottomOnly : padStackAccessBottom);

            break;
         case layerTypePadAll:
         case layerTypeSignalAll:
         case layerTypePadOuter:
         case layerTypeSignalOuter:
            layerFlag |= (padStackAccessTop | padStackAccessBottom);

            break;
         case layerTypePadInner:
         case layerTypeSignalInner:
            layerFlag |= padStackAccessInner;

            break;
         case layerTypeMaskTop:
            layerFlag |= padStackAccessMaskTop;

            break;
         case layerTypeMaskBottom:
            layerFlag |= padStackAccessMaskBottom;

            break;
         case layerTypeMaskAll:
            layerFlag |= (padStackAccessMaskTop | padStackAccessMaskBottom);

            break;
         }
      }

      if (data->getDataType() == dataTypeInsert)
      {
         BlockStruct* block = getBlock(data->getInsert()->getBlockNumber());

         if ((block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE) && block->getShape() == apertureComplex)
         {
            block = getBlock(block->getComplexApertureSubBlockNumber());
         }

         layerFlag |= getPadstackAccessLayer(block->getDataList());
      }

#ifdef EnableRecursiveReferenceChecking
      dataStack.RemoveTail();
#endif
   }

   return layerFlag;
}

void CCamCadData::refreshTypetoCompAttributes()
{
   for (POSITION filePos = getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* file = getFileList().GetNext(filePos);

      if (file->isShown() && file->getBlockType() == blockTypePcb)  
      {
         // loop through datalist to find inserts
         for (CDataListIterator insertIterator(*(file->getBlock()),dataTypeInsert);insertIterator.hasNext();)
         {
            DataStruct* data = insertIterator.getNext();

            if (data->getAttributesRef() == NULL)
               continue;

            CString typeName;

            if (data->attributes().getAttribute(standardAttributeTypeListLink,typeName))
            {
               if (! typeName.IsEmpty())
               {
                  TypeStruct& type = file->getDeviceTypeDirectory().getDefinedType(typeName);

                  if (type.getAttributesRef() != NULL)
                  {
                     data->attributes().CopyAll(type.attributes());
                  }
               }
            }

            //Attrib *a =  is_attvalue(doc, data->getAttributesRef(), ATT_TYPELISTLINK, 1);

            //if (a == NULL)
            //   continue;

            //CString t = get_attvalue_string(doc, a);

            //if (strlen(t) == 0)
            //   continue;

            //TypeStruct *type = AddType(file, t);
      
            //if (type->getAttributesRef())
            //{
            //   // loop through block's attributes
            //   for (attribPos = type->getAttributesRef()->GetStartPosition();attribPos != NULL;)
            //   {
            //      type->getAttributesRef()->GetNextAssoc(attribPos, keyword, b_ptr);

            //      CString val = get_attvalue_string(doc, b_ptr);
            //      doc->SetUnknownAttrib(&data->getAttributesRef(), doc->getKeyWordArray()[keyword]->cc, val, mode, NULL);
            //   }
            //}
         }
      }
   }

   return;
}

void CCamCadData::normalizeBottomBuiltGeometries()
{
   FileStruct* pcbFile = getFileList().GetFirstShown(blockTypePcb);

   if (pcbFile == NULL)
   {
      ErrorMessage("No visible PCB file detected.\n\nNormalize Bottom Built Geometries requires a visible PCB file.", "Normalize Bottom Built Geometries");
      return;
   }

   getBlockArray().GenerateDesignSurfaceInfo(*this,true);

   // Collect all bottom built geometry block numbers into a map and all inserts of those geometries into a list.
   // Note that this is not really "all" bottom built geometries, it is bottom built geometries 
   // that are inserted in the PCB. The significance is that this may not necessarily compose
   // a complete list of blocks that need modification. I.e. some inserts may in turn insert
   // blocks that also need some treatment, and these will not be in this list.
   CTypedPtrListContainer<DataStruct*> bottomBuiltDataList(false);
   CMapBlockNumberToBlock bottomBuiltGeometryMap(nextPrime2n(20),false);

   for (CDataListIterator dataListIterator(*pcbFile->getBlock(), dataTypeInsert); dataListIterator.hasNext();)
   {
      DataStruct* data = dataListIterator.getNext();
      InsertStruct* insert = data->getInsert();
      BlockStruct* dataBlock = getBlock(insert->getBlockNumber());

      if (dataBlock != NULL && !dataBlock->IsDesignedOnTop(*this))
      {
         bottomBuiltDataList.AddTail(data);
         bottomBuiltGeometryMap.SetAt(dataBlock->getBlockNumber(), NULL);
      }
   }

   // Loop through the bottom built geometries map to create top built geometries
   for (POSITION geomPos = bottomBuiltGeometryMap.GetStartPosition(); geomPos != NULL;)
   {
      int bottomBuiltBlockNum = -1;
      BlockStruct* topBuiltBlock = NULL;

      bottomBuiltGeometryMap.GetNextAssoc(geomPos, bottomBuiltBlockNum, topBuiltBlock);

      if (bottomBuiltBlockNum < 0)
         continue;
   
      // If the blockNum has a block then it means that the built definition is already creeated
      if (topBuiltBlock != NULL)
         continue;

      int mirCount = 0;
      //topBuiltBlock = CreateTopDefinedGeomtry(bottomBuiltBlockNum, bottomBuiltGeometryMap);
      topBuiltBlock = createTopDefinedGeometry(bottomBuiltBlockNum, bottomBuiltGeometryMap,mirCount);
      bottomBuiltGeometryMap.SetAt(bottomBuiltBlockNum, topBuiltBlock);
   }

   // Loop through list of inserts of bottom built geometry and replace them with the top built geometry
   for (POSITION dataPos = bottomBuiltDataList.GetHeadPosition(); dataPos != NULL;)
   {
      DataStruct* data = bottomBuiltDataList.GetNext(dataPos);

      if (data != NULL && data->getInsert() != NULL)
      {
         InsertStruct* insert = data->getInsert();

         BlockStruct* topBuiltBlock = NULL;
         bottomBuiltGeometryMap.Lookup(insert->getBlockNumber(), topBuiltBlock);

         if (topBuiltBlock != NULL)
         {
            insert->setBlockNumber(topBuiltBlock->getBlockNumber());

            if (insert->getGraphicMirrored())
            {
               insert->setMirrorFlags(0);
               insert->setPlacedBottom(false);
            }
            else
            {
               insert->setMirrorFlags(MIRROR_ALL);
               insert->setPlacedBottom(true);
            }
         }
      }
   }

   bottomBuiltDataList.RemoveAll();
   bottomBuiltGeometryMap.RemoveAll();
}

BlockStruct* CCamCadData::createTopDefinedGeometry(int bottomBuiltBlockNum, CMapBlockNumberToBlock& bottomBuiltGeometryMap,int& mirCount)
{
   BlockStruct* bottomBuiltBlock = this->getBlockAt(bottomBuiltBlockNum);

   if (bottomBuiltBlock == NULL)
      return NULL;

   // Do not change apertures and tools, same for top or bottom
   if (bottomBuiltBlock->isAperture() || bottomBuiltBlock->isTool())
      return NULL;

   // Found top built so return it
   BlockStruct* topBuiltBlock = NULL;

   if (bottomBuiltGeometryMap.Lookup(bottomBuiltBlockNum, topBuiltBlock) && topBuiltBlock !=  NULL)
      return topBuiltBlock;

   // Create name for top built geometry
   int count = 1;
   CString topBuiltGeomName = (CString)bottomBuiltBlock->getName() + "_Top";

   while (true)
   {
      //topBuiltBlock = Graph_Block_Exists(this, topBuiltGeomName, bottomBuiltBlock->getFileNumber());
      topBuiltBlock = getBlock(topBuiltGeomName,bottomBuiltBlock->getFileNumber());

      // If no block with the "TOP" version of the geometry then exit loop to create one
      if (topBuiltBlock == NULL) 
         break;
      
      // If there is a geometry with the "TOP" version name, and its original name match the bottom built geometry
      // name, then it is the "TOP" version return it
      if (topBuiltBlock->getOriginalName().CompareNoCase(bottomBuiltBlock->getName()) == 0)
         return topBuiltBlock;

      // Name already used, but geometry was not derived the corresponding bottom built geometry
      topBuiltGeomName.Format("%s_TOP_%d", bottomBuiltBlock->getName(), ++count);
   }

   //topBuiltBlock = Graph_Block_On(GBO_APPEND, topBuiltGeomName, bottomBuiltBlock->getFileNumber(), 0, bottomBuiltBlock->getBlockType());
   topBuiltBlock = &(getNewBlock(topBuiltGeomName,bottomBuiltBlock->getFileNumber(),bottomBuiltBlock->getBlockType()));
   //topBuiltBlock->setBlockType(bottomBuiltBlock->getBlockType());
   topBuiltBlock->setOriginalName(bottomBuiltBlock->getName());

   //Graph_Block_Copy(bottomBuiltBlock, 0.0, 0.0, 0.0, 0, 1, -1, TRUE); 
   //Graph_Block_Off();
   topBuiltBlock->attributes().CopyAll(bottomBuiltBlock->attributes());
   CDataList dataCopyList(bottomBuiltBlock->getDataList(),*this);
   topBuiltBlock->getDataList().takeData(dataCopyList);

   for (CDataListIterator dataIterator(*topBuiltBlock);dataIterator.hasNext();)
   {
      DataStruct* data = dataIterator.getNext();

      switch (data->getDataType())
      {
      case dataTypeInsert:
         {
            InsertStruct* insert = data->getInsert();

            if (insert != NULL)
            {
               BlockStruct* subBlock = this->getBlockAt(insert->getBlockNumber());
               
               if (insert->getGraphicMirrored())
               {
                  mirCount++;
               }

               // Case 1817
               // Recursively treat all things involved in item
               BlockStruct* topBuiltSubBlock = createTopDefinedGeometry(subBlock->getBlockNumber(),bottomBuiltGeometryMap,mirCount);

               if (topBuiltSubBlock != NULL)
               {
                  bottomBuiltGeometryMap.SetAt(subBlock->getBlockNumber(), topBuiltSubBlock);
                  insert->setBlockNumber(topBuiltSubBlock->getBlockNumber());
               }

               // Case 1817
               // Using recursion, alteration of item starts at bottom of hierarchy, instead
               // of top of hierarchy as original code did. 
               // Treat layer number if not a floating layer.
               // Change insert/mirror only the same number of times that mirror
               // occurs in original item hierarchy. Simply mirroring every level in
               // hiearchy is not correct. I.e. Bottom-Bottom is not the "mirror" of Top-Top.
               if (!this->isFloatingLayer(data->getLayerIndex()))
               {
                  LayerStruct* layer = this->getLayer(data->getLayerIndex());

                  if (layer != NULL && layer->getMirroredLayerIndex() > 0)
                  {
                     data->setLayerIndex(layer->getMirroredLayerIndex());
                  }
               }
               else if (mirCount > 0)
               {
                  mirCount--;

                  if (insert->getGraphicMirrored())
                  {
                     insert->setMirrorFlags(0);
                     insert->setPlacedBottom(false);
                  }
                  else
                  {
                     insert->setMirrorFlags(MIRROR_ALL);
                     insert->setPlacedBottom(true);
                  }

                  insert->setOriginX(-insert->getOriginX());
                  insert->setAngle(-insert->getAngle());
               }
               // else do nothing, is "neutral" item in hierarchy

            }
         }

         break;
      case dataTypePoly:
         {
            // Change the polylist X coordinate and layer
            //MirrorPolyOnX(data->getPolyList());
            CTMatrix mirrorMatrix;
            mirrorMatrix.mirrorAboutYAxis();

            data->getPolyList()->transform(mirrorMatrix);

            if (!this->isFloatingLayer(data->getLayerIndex()) && data->getLayerIndex() != -1)
            {
               LayerStruct* layer = this->getLayer(data->getLayerIndex());

               data->setLayerIndex(layer->getMirroredLayerIndex());
            }
         }

         break;
      case dataTypeText:
         {
            // Change the text X coordinate and layer
            data->getText()->setMirrored(false);
            data->getText()->setPntX(-data->getText()->getPnt().x);

            if (!this->isFloatingLayer(data->getLayerIndex()) && data->getLayerIndex() != -1)
            {
               LayerStruct* layer = this->getLayer(data->getLayerIndex());

               data->setLayerIndex(layer->getMirroredLayerIndex());
            }
         }

         break;
      default:
         break;
      }
   }

   topBuiltBlock->GenerateDesignSurfaceAttribute(*this, true);

   return topBuiltBlock;
}

BlockStruct& CCamCadData::getDefinedGluePointGeometry()
{
   const char* gluePointGeometryName = "$gluepoint";

   BlockStruct* gluePointGeometry = getBlockDirectory().getAt(gluePointGeometryName,-1,blockTypeGluePoint);

   if (gluePointGeometry == NULL)
   {
      gluePointGeometry = &(getNewBlock(gluePointGeometryName,-1,blockTypeGluePoint));
      gluePointGeometry->setFlagBits(BL_SPECIAL);

      LayerStruct& gluePointLayerTop = getLayerArray().getDefinedLayer(ccLayerGluePointTop);

      CPoly* poly;
      double unitLength = getUnitsFactor(pageUnitsMils,getPageUnits());
      double radius     = 50. * unitLength;

      DataStruct* data = getNewDataStruct(dataTypePoly);
      data->setLayerIndex(gluePointLayerTop.getLayerIndex());

      gluePointGeometry->getDataList().AddTail(data);

      // circle
      poly = addFilledPoly(*data,getZeroWidthIndex());
      addVertex(*poly,0., radius,1.);
      addVertex(*poly,0.,-radius,1.);
      addVertex(*poly,0.,     0.,1.);

      // line
      poly = addOpenPoly(*data,getZeroWidthIndex());
      addVertex(*poly,    0.,0.);
      addVertex(*poly,radius,0.);

      // arrow head
      poly = addClosedPoly(*data,getZeroWidthIndex());
      addVertex(*poly, 50. * unitLength, 25. * unitLength);
      addVertex(*poly, 50. * unitLength,-25. * unitLength);
      addVertex(*poly,100. * unitLength,  0.             );
      addVertex(*poly, 50. * unitLength, 25. * unitLength);
   }

   return *gluePointGeometry;
}

int CCamCadData::getDefinedToolIndex(double size,const CString& toolName,bool plated)
{
   int toolWidthIndex = -1;
   int tcode  = 0;
   int toolType = 0;
   int geometryNumber = 0;
   bool display = false;

   DbFlag flags = BL_TOOL | BL_GLOBAL; // tools are global

   for (int widthIndex = 0; widthIndex < m_widthTable.GetSize();widthIndex++)
   {
      BlockStruct* block = m_widthTable.getAt(widthIndex);

      if ((block != NULL) && ((block->getFlags() & BL_TOOL) != 0) && 
          fpeq(block->getToolSize(),size) && block->getToolHolePlated() == plated)
      {
         toolWidthIndex = widthIndex;

         break;
      }
   }

   if (toolWidthIndex < 0)
   {
      const int fileNumber = -1;
      BlockStruct* block;

      if (toolName.IsEmpty())
      {
         block = getNewBlock("DRILL_%d",blockTypeDrillHole,fileNumber);
      }
      else
      {
         block = &(getNewBlock(toolName,fileNumber,blockTypeDrillHole));
      }

      block->setTcode(tcode);
      block->setToolSize(size);
      block->setToolType(toolType);
      block->setToolBlockNumber(geometryNumber);
      block->setToolDisplay(display);
      block->setFlags(flags);
      block->setToolHolePlated(plated);

      // we need to make sure that we also can NC-ROUTE (Draw) with a tool
      // must be in the width index. The Tool List works of this 
      toolWidthIndex = m_widthTable.Add(block);
   }

   return toolWidthIndex;
}

BlockStruct& CCamCadData::getDefinedComplexAperture(const CString& apertureName,const CString& subBlockName,
   double xOffset,double yOffset,double rotationRadians,int fileNumber,BlockTypeTag blockType,int dCode)
{
   BlockStruct* widthBlock = NULL;
   BlockStruct* subBlock = getBlockDirectory().getAt(subBlockName,fileNumber);

   if (subBlock == NULL)
   {
      subBlock = &(getBlockDirectory().getDefinedBlock(subBlockName,fileNumber,blockTypeUndefined));
   }

   int subBlockNumber = subBlock->getBlockNumber();

   DbFlag flags = BL_APERTURE | ((fileNumber == -1) ? BL_GLOBAL : 0);

   for (int widthIndex = 0;;widthIndex++)
   {
      if (widthIndex >= getWidthTable().GetSize())
      {
         widthBlock = NULL;
         break;
      }

      widthBlock = getWidthTable().getAt(widthIndex);

      if (widthBlock != NULL)
      {
         if (widthBlock->getName().Compare(apertureName) == 0 && widthBlock->getFileNumber() == fileNumber)
         {
            if (blockType == blockTypeUndefined || widthBlock->getBlockType() == blockType)
            {
               widthBlock->setShape(apertureComplex);
               widthBlock->setComplexApertureSubBlockNumber(subBlockNumber);
               widthBlock->setDcode(dCode);
               widthBlock->setFlags(flags);

               break;
            }
         }
      }
   }

   if (widthBlock == NULL)
   {
      widthBlock = &(getBlockDirectory().getNewBlock(apertureName,fileNumber,blockType));

      widthBlock->setShape(apertureComplex);
      widthBlock->setComplexApertureSubBlockNumber(subBlockNumber);
      widthBlock->setDcode(dCode);
      widthBlock->setFlags(flags);
      widthBlock->setXoffset(xOffset);
      widthBlock->setYoffset(yOffset);
   }

   return *widthBlock;
}

DataStruct* CCamCadData::drawAxes(const CTMatrix& matrix,const CString& layerName)
{
   DataStruct* polyStruct = NULL;

   BlockStruct* pcbBlock = getFirstVisibleFile()->getBlock();

   if (pcbBlock != NULL)
   {
      double u = .002;
      int layerIndex = this->getDefinedLayerIndex(layerName);
      DataStruct* polyStruct = addPolyStruct(*pcbBlock,layerIndex,graphicClassNormal);
      int widthIndex = getZeroWidthIndex();

      double diagramData[][4] =
      {
         // insert point
         {   0. ,   0.,  -2.,  -2. },

         // x axis
         {  -2. ,   0.,  22.,   0. },
         {   2. ,   0.,   2.,   1. }, 
         {   4. ,   0.,   4.,   1. }, 
         {   6. ,   0.,   6.,   1. }, 
         {   8. ,   0.,   8.,   1. }, 
         {  10. ,   0.,  10.,   2. }, 
         {  12. ,   0.,  12.,   1. }, 
         {  14. ,   0.,  14.,   1. }, 
         {  16. ,   0.,  16.,   1. }, 
         {  18. ,   0.,  18.,   1. }, 
         {  20. ,   0.,  20.,   2. }, 

         {  20. ,   2.,  15.,   7. }, 

         {  23. ,   0.,  25.,   2. }, 
         {  23. ,   2.,  25.,   0. }, 

         // y axis
         {    0., -2. ,  0.,   22. },
         {    0.,  2. ,  1.,    2. }, 
         {    0.,  4. ,  1.,    4. }, 
         {    0.,  6. ,  1.,    6. }, 
         {    0.,  8. ,  1.,    8. }, 
         {    0., 10. ,  2.,   10. }, 
         {    0., 12. ,  1.,   12. }, 
         {    0., 14. ,  1.,   14. }, 
         {    0., 16. ,  1.,   16. }, 
         {    0., 18. ,  1.,   18. }, 
         {    0., 20. ,  2.,   20. }, 

         {   -1., 23. ,  1.,   25. }, 
         {   -1., 25. ,  0.,   24. }, 
      };

      const int numSegments = sizeof(diagramData) / (4*sizeof(double));

      for (int pointIndex = 0;pointIndex < numSegments;pointIndex++)
      {
         CPoint2d p0(diagramData[pointIndex][0] * u,diagramData[pointIndex][1] * u);
         CPoint2d p1(diagramData[pointIndex][2] * u,diagramData[pointIndex][3] * u);

         matrix.transform(p0);
         matrix.transform(p1);

         CPoly* poly = addOpenPoly(*polyStruct,widthIndex);
         addVertex(*poly,p0.x,p0.y);
         addVertex(*poly,p1.x,p1.y);
      }
   }

   return polyStruct;
}

//=====================================================================================================================================================
// Deprecated, must be rewritten
#if defined(EnableDcaCamCadDocLegacyCode)

//_____________________________________________________________________________
//class ExtentRect 
//{
//public:
//   double left, top, right, bottom;
//
//   ExtentRect();
//   ExtentRect& operator=(const CExtent& extent);
//   void reset();
//
//   bool isCorrupt();
//   bool isValid();
//};

//class Mat2x2 
//{
//public:
//   double m2[ 2 ][ 2 ];
//};

void RotMat2( Mat2x2 *m, double az ); 
void TransPoint2( Point2 *v, int n, const Mat2x2 *m, double tx, double ty ); 

CExtent UseInsertExtents(BlockStruct* block, double insert_x, double insert_y, double rotation, int mirror, double scale)
{
   CExtent extent;

   if (block->getExtent().isSensible(1e10))
   {
      Mat2x2 m;
      RotMat2(&m, rotation);
      Point2 p;

      { // (xmin, ymin)
         p.x = block->getXmin() * scale;
         p.y = block->getYmin() * scale;

         if (mirror & MIRROR_FLIP)  p.x = -p.x;

         TransPoint2(&p, 1, &m, insert_x, insert_y);

         extent.update(p.x,p.y);
      }

      { // (xmin, ymax)
         p.x = block->getXmin() * scale;
         p.y = block->getYmax() * scale;

         if (mirror & MIRROR_FLIP)  p.x = -p.x;

         TransPoint2(&p, 1, &m, insert_x, insert_y);

         extent.update(p.x,p.y);
      }

      { // (xmax, ymax)
         p.x = block->getXmax() * scale;
         p.y = block->getYmax() * scale;

         if (mirror & MIRROR_FLIP)  p.x = -p.x;

         TransPoint2(&p, 1, &m, insert_x, insert_y);

         extent.update(p.x,p.y);
      }

      { // (xmax, ymin)
         p.x = block->getXmax() * scale;
         p.y = block->getYmin() * scale;

         if (mirror & MIRROR_FLIP)  p.x = -p.x;

         TransPoint2(&p, 1, &m, insert_x, insert_y);

         extent.update(p.x,p.y);
      }
   }

   return extent;
}

bool UseInsertExtents(double* lxmin, double* lxmax, double* lymin, double* lymax,BlockStruct* block, 
      double insert_x, double insert_y, double rotation, int mirror, double scale)
{
   CExtent extent = UseInsertExtents(block, insert_x, insert_y, rotation, mirror, scale);

   if (extent.isValid())
   {
      *lxmin = extent.getXmin();
      *lymin = extent.getYmin();
      *lxmax = extent.getXmax();
      *lymax = extent.getYmax();
   }

   return extent.isValid();
}

bool PolyPntExtents(CPntList *pntList,CExtent& extent, double scale, int mirror,
      double insert_x, double insert_y, Mat2x2 *m, double width);
bool PolyPntExtents(CPntList *pntList, ExtentRect *extents, double scale, int mirror,
      double insert_x, double insert_y, Mat2x2 *m, double width);


//_____________________________________________________________________________
CExtent CCamCadData::blockExtents(CDataList* DataList,     
      double blockInsertX, double blockInsertY, double blockInsertRotation, int mirror,double scale,int insertLayer,bool checkOnlyVisibleEntitiesFlag,CMessageFilter* messageFilter)
{
	bool debugFlag = false;
   static int callLevel = 0;  // for debugging

   callLevel++;

   CExtent extent;
   BlockStruct *subblock;
   ExtentRect extents;
   int layer;
   int extentCount = 0;

   Mat2x2 incomingRotMat;
   RotMat2(&incomingRotMat, blockInsertRotation);

   CTMatrix matrix;
   matrix.scale(((mirror & MIRROR_FLIP) != 0) ? -scale : scale,scale);
   matrix.rotateRadians(blockInsertRotation);
   matrix.translate(blockInsertX, blockInsertY);

   double spaceRatio = this->getCamCadDataSettings().getTextSpacingRatio();

   POSITION dataPos = DataList->GetHeadPosition();

   for (int index=0;dataPos != NULL;index++)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->isHidden())
         continue;

      if (data->getDataType() == dataTypeBlob)
      {
         Point2 p;
         p.x = data->getBlob()->pnt.x * scale;

         if (mirror & MIRROR_FLIP)
            p.x = -p.x;

         p.y = data->getBlob()->pnt.y * scale;
         TransPoint2(&p, 1, &incomingRotMat, blockInsertX, blockInsertY);

         extent.update(p.x,p.y);
         extent.update(p.x + data->getBlob()->width * scale,p.y + data->getBlob()->height * scale);
         //extent.assertFloat();

         extentCount++;
      }
      else
      {
         if (data->getDataType() == dataTypeInsert)
         {
            
            if (checkOnlyVisibleEntitiesFlag && !getInsertTypeFilter().contains(data->getInsert()->getInsertType()))
               continue;
         }
         else 
         {  
            if (checkOnlyVisibleEntitiesFlag && !getGraphicsClassFilter().contains((data->getGraphicClass()>0?data->getGraphicClass():0)))
               continue;
         }

         // figure LAYER
         if (isFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = data->getLayerIndex();

         // figure TYPE
         if (data->getDataType() == dataTypeInsert)
         {
            subblock = getBlockAt(data->getInsert()->getBlockNumber());
            // this only can happen if a insert refers to a non existing geometry
            if (subblock == NULL)
               continue;
         }

         // Check Layer Visible
         if (data->getDataType() != dataTypeInsert || 
               ((subblock->getFlags() & BL_TOOL     || subblock->getFlags() & BL_BLOCK_TOOL    ) && !subblock->getToolDisplay()) || 
               ((subblock->getFlags() & BL_APERTURE || subblock->getFlags() & BL_BLOCK_APERTURE) &&  subblock->getShape() != apertureComplex))
         {
            // only check layer for non insert entities
            if (checkOnlyVisibleEntitiesFlag && !isFloatingLayer(layer) && !isLayerPairVisible(layer)) 
               continue;
         }
         else // block
         { 
            // adjust for LAYER = -1 
            // only for Block Apertures and Block Tools when insertLayer = -1
            if (layer == -1 &&
               ((subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE) ||
                (subblock->getFlags() & BL_TOOL    ) || (subblock->getFlags() & BL_BLOCK_TOOL    )) )
            {
               if (!subblock->getDataList().IsEmpty() )
               {
                  layer = subblock->getDataList().GetHead()->getLayerIndex();

                  if (checkOnlyVisibleEntitiesFlag && !isLayerPairVisible(layer))
                     continue;
               }
            }
         }

         switch (data->getDataType())
         {
         case dataTypePoint:
            {
               Point2 p;
               p.x = data->getPoint()->x * scale;

               if (mirror & MIRROR_FLIP)
                  p.x = -p.x;

               p.y = data->getPoint()->y * scale;
               TransPoint2(&p, 1, &incomingRotMat, blockInsertX, blockInsertY);

               extent.update(p.x,p.y);
               extentCount++;
            }

            break;

         case dataTypePoly:
            if (data->getPolyList()->hasPoints())
            {
               CExtent polyExtent = PolyExtents(data->getPolyList(),scale,mirror & MIRROR_FLIP,blockInsertX,blockInsertY,&incomingRotMat,TRUE);

               if (polyExtent.isValid())
               {
                  extent.update(polyExtent);
                  extentCount++;
               }
            }

            break;

         case dataTypeText:
            {
               double penWidth = getWidth(data->getText()->getPenWidthIndex());
               CExtent textExtent = data->getText()->getExtent(spaceRatio,matrix,penWidth);
               extent.update(textExtent);
               extentCount++;
            }
            break;  

         case dataTypeInsert:
            {
               // calculate point

					// Removed as per Case #845...  Centroids should be included in extents calculations.
               //if (data->getInsert()->getInsertType() == insertTypeCentroid)
					//	continue;

               Point2 point2;
               point2.x = data->getInsert()->getOriginX() * scale;

               if (mirror & MIRROR_FLIP)
                  point2.x = -point2.x;

               point2.y = data->getInsert()->getOriginY() * scale;
               TransPoint2(&point2, 1, &incomingRotMat, blockInsertX, blockInsertY);

               BOOL 
                  DrawAp = FALSE, 
                  DrawSubblock = FALSE,
                  IsTool = (subblock->getFlags() & BL_TOOL) || (subblock->getFlags() & BL_BLOCK_TOOL),
                  IsAp   = (subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE);
               double 
                  _x     = point2.x,
                  _y     = point2.y,
                  _rot   = blockInsertRotation,
                  _scale = scale;
               int 
                  _mirror = mirror;
               int 
                  _layer = layer;

               // TOOL
               if (IsTool)
               {
                  if (subblock->getToolDisplay()) // use another block's graphics to display this tool
                  {
                     subblock = getBlockAt(subblock->getToolBlockNumber());

                     if ((subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE))
                        DrawAp = TRUE;
                     else
                        DrawSubblock = TRUE;
                  }
                  else // draw generic tool
                  {
                     extent.update( _x - subblock->getToolSize() / 2, _y - subblock->getToolSize() / 2);
                     extent.update( _x + subblock->getToolSize() / 2, _y + subblock->getToolSize() / 2);
                     extentCount++;
                  }
               } // end TOOL

               // APERTURE
               if (IsAp || DrawAp)
               {
                  // COMPLEX APERTURE
                  if (subblock->getShape() == apertureComplex)
                  {
                     // subblock is complex aperture insert (only looking at block's values ex. sizeA & rotation)

                     if (mirror & MIRROR_FLIP)
                        _rot = blockInsertRotation - subblock->getRotation() - data->getInsert()->getAngle();
                     else
                        _rot = blockInsertRotation + subblock->getRotation() + data->getInsert()->getAngle();
      
                     // now change subblock to aperture definition block (looking at block definition ex. polys & circles)
                     subblock = getBlockAt((int)(subblock->getSizeA()));
                     DrawSubblock = TRUE;
                  }     
                  else // NORMAL APERTURE
                  {
                     if (subblock->getShape() == apertureUndefined)
                     {
                        extent.update(data->getInsert()->getOriginX(), data->getInsert()->getOriginY());
                        //extent.assertFloat();

                        extentCount++;
                     }
                     else
                     {
                        //CPolyList *polylist = ApertureToPoly(subblock, data->getInsert(), mirror);
                        // getAperturePolys() will apply subBlock->getRotation() so make sure it does not
                        // get re-applied again later.
                        CPolyList* polylist = subblock->getAperturePolys(*this);

                        // Do not use subblock->getRotation() here, that rotation is already considered and
                        // applied in the subblock->getAperturePoly(). Putting it here applies it twice. 
                        double _rotrot = 0.;
                        if (mirror & MIRROR_FLIP)
                         _rotrot = blockInsertRotation - data->getInsert()->getAngle();
                        else
                           _rotrot = blockInsertRotation + data->getInsert()->getAngle();
                        Mat2x2 mmm;
                        RotMat2(&mmm, _rotrot);
                        
                        if (polylist != NULL && polylist->hasPoints())
                        {
                           extents.left  = extents.bottom =  FLT_MAX;
                           extents.right = extents.top    = -FLT_MAX;

                           if (PolyExtents(polylist, &extents, scale, mirror & MIRROR_FLIP, _x, _y, &mmm, TRUE))
                           {
                              extent.update(extents.left ,extents.bottom);
                              extent.update(extents.right,extents.top   );
                              extentCount++;
                           }

                           delete polylist;
                        }
                     }
                  } // end NORMAL APERTURE
               } // end APERTURE
      
               // GRAPHIC BLOCK
               if (!IsAp && !IsTool) 
               {
                  DrawSubblock = TRUE;

                  if (mirror & MIRROR_FLIP)
                     _rot = blockInsertRotation - data->getInsert()->getAngle();
                  else
                     _rot = blockInsertRotation + data->getInsert()->getAngle();

                  _mirror = mirror ^ data->getInsert()->getMirrorFlags();
                  _scale  =  scale * data->getInsert()->getScale();
               }

               // Draw Block
               if (DrawSubblock)
               {
                  if (subblock->getDataList().IsEmpty())
                  {     
                     extent.update(_x,_y);
                     extentCount++;
                  }
                  else
                  {
                     double xmin, xmax, ymin, ymax;
                     xmin = ymin =  FLT_MAX;
                     xmax = ymax = -FLT_MAX;

                     // use extents if at a 90 degree angle
                     if (abs(DcaRound(radiansToDegrees(_rot)) % 90) < 2)
                     {
                        validateBlockExtents(subblock,checkOnlyVisibleEntitiesFlag);
      
                        if (subblock->extentIsValid())
                        {
                           if (UseInsertExtents(&xmin, &xmax, &ymin, &ymax, subblock, _x, _y, _rot, _mirror, _scale))
                           {
                              extent.update(xmin,ymin);
                              extent.update(xmax,ymax);
                              extentCount++;
                           }
                        }
                     }
                     else
                     {
                        if (block_extents(&xmin, &xmax, &ymin, &ymax, &(subblock->getDataList()), _x, _y, _rot, _mirror, _scale, layer,checkOnlyVisibleEntitiesFlag))
                        {
                           extent.update(xmin,ymin);
                           extent.update(xmax,ymax);
                           extentCount++;
                        }
                     }
                  }
               } // end Draw Block

               // check visible attribs
               if (showAttributes() && CFontList::fontIsLoaded() && data->getAttributesRef() && !data->getAttributesRef()->IsEmpty())
               {
                  CExtent attribExtents = VisibleAttribsExtents(data->getAttributesRef(),&point2,_scale,_rot,_mirror);

                  if (attribExtents.isValid())
                  {
                     extent.update(attribExtents);
                     extentCount++;
                  }
               }
            }

            break;
         }
      }

		if (debugFlag && messageFilter != NULL)
		{
			messageFilter->formatMessage("EntityNumber = %d\txMin = %0.3f\txMax = %0.3f\tyMin = %0.3f\tyMax = %0.3f\n",
					data->getEntityNumber(), extent.getXmin(), extent.getXmax(), extent.getYmin(), extent.getYmax());
		}
   }

   callLevel--;

   return extent;
}

CExtent CCamCadData::PolyExtents(CPolyList *polylist, double scale, int mirror,
   double insert_x, double insert_y, Mat2x2 *m, int use_width)
{
   bool retval = false;
   CPoly *poly;
   double width;
   CExtent extent;

	if (polylist == NULL)
		return extent;

   for (POSITION polyPos = polylist->GetHeadPosition();polyPos != NULL;)
   {
      poly = polylist->GetNext(polyPos);
      int widthIndex = poly->getWidthIndex();

      if (use_width && poly->getWidthIndex() >= 0 && getWidthBlock(widthIndex) != NULL)
      {
         width = (getWidth(widthIndex) / 2.) * scale;
      }
      else
      {
         width = 0;
      }

      if (PolyPntExtents(&poly->getPntList(), extent, scale, mirror, insert_x, insert_y, m, width))
      {
         retval = true;
      }
   }

   return extent;
}  // END PolyExtents

bool CCamCadData::PolyExtents(CPolyList *polylist, ExtentRect *extents, double scale, int mirror,
      double insert_x, double insert_y, Mat2x2 *m, int use_width)
{
   bool retval = false;

   CPoly *poly;
   double width;

   extents->left  = extents->bottom =  FLT_MAX;
   extents->right = extents->top    = -FLT_MAX;
   
   for (POSITION polyPos = polylist->GetHeadPosition();polyPos != NULL;)
   {
      poly = polylist->GetNext(polyPos);
      int widthIndex = poly->getWidthIndex();

      if (use_width && widthIndex >= 0 && getWidthBlock(widthIndex) != NULL)
      {
         width = (getWidth(widthIndex) / 2.) * scale;
      }
      else
      {
         width = 0;
      }

      if (PolyPntExtents(&poly->getPntList(), extents, scale, mirror, insert_x, insert_y, m, width))
      {
         retval = true;
      }
   }

   return retval;
}

bool CCamCadData::validateBlockExtents(BlockStruct* block,bool checkOnlyVisibleEntitiesFlag)
{
   bool calculatedFlag = (block != NULL && ! block->extentIsValid());

   if (calculatedFlag)
   {
      block->calculateBlockExtents(*this,checkOnlyVisibleEntitiesFlag);
   }

   return calculatedFlag;
}

bool CCamCadData::block_extents(double *lxmin, double *lxmax, double *lymin, double *lymax, CDataList *DataList,     
      double insert_x, double insert_y, double rotation, int mirror, double scale, int insertLayer, BOOL onlyVisible)
{
   CExtent extent = blockExtents(DataList,insert_x,insert_y,rotation,mirror,scale,insertLayer,onlyVisible != 0);

   bool retval = extent.isValid();

   if (retval)
   {
      *lxmin = extent.getXmin();
      *lxmax = extent.getXmax();
      *lymin = extent.getYmin();
      *lymax = extent.getYmax();
   }

   return retval;
}

CExtent CCamCadData::VisibleAttribsExtents(CAttributes* map,Point2* pnt, 
   double block_scale, double block_rot, int block_mir)
{
   CExtent extent;
   
   for (POSITION attribPos = map->GetStartPosition();attribPos != NULL;)
   {
      CAttribute* attrib;
      WORD keyword;

      map->GetNextAssoc(attribPos, keyword, attrib);

      //if (!attrib->isVisible() || attrib->getValueType() != VT_STRING || !doc->get_layer_visible(attrib->getLayerIndex(), block_mir))
      if (!attrib->isVisible() || attrib->getValueType() != valueTypeString || !isLayerVisible(attrib->getLayerIndex(), block_mir))
         continue;

      CExtent attribExtent = 
         VisibleAttribExtents(attrib,pnt,block_scale,block_rot,block_mir);

      if (attribExtent.isValid())
      {
         extent.update(attribExtent);
      }
   }

   return extent;
}

CExtent CCamCadData::VisibleAttribExtents(CAttribute* attrib,Point2* pnt, double block_scale, double block_rot, int block_mir)
{
   CExtent extent;

   bool validCoordsFlag = (_finite(attrib->getX()) && _finite(attrib->getY()));

   if (validCoordsFlag)
   {
      Mat2x2 m;
      RotMat2(&m, block_rot);
      Point2 p;
      p.x = attrib->getX() * block_scale;

      if (block_mir & MIRROR_FLIP)
         p.x = -p.x;

      p.y = attrib->getY() * block_scale;
      TransPoint2(&p, 1, &m, pnt->x, pnt->y);

      double text_rot;

      if (block_mir & MIRROR_FLIP)
         text_rot = block_rot - attrib->getRotationRadians();
      else
         text_rot = block_rot + attrib->getRotationRadians();

      //extent = TextBox(doc,attrib->getStringValue(), 
      //      &p, attrib->getWidth() * block_scale, attrib->getHeight() * block_scale, text_rot, block_mir, FALSE);

      TextStruct textStruct;

      textStruct.setText(getAttributeStringValue(*attrib));
      textStruct.setOrigin(p.x,p.y);
      textStruct.setWidth(attrib->getWidth() * block_scale);
      textStruct.setHeight(attrib->getHeight() * block_scale);
      textStruct.setRotation(text_rot);
      textStruct.setMirrored(block_mir != 0);
      textStruct.setProportionalSpacing(false);

      CExtent extent = textStruct.getExtent(getCamCadDataSettings().getTextSpacingRatio());


   }

   return extent;
}

#endif //#if defined(EnableDcaCamCadDocLegacyCode)








