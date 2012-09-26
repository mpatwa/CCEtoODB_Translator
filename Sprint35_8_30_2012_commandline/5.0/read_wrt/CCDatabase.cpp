
//   Router Solutions Inc.
//   Copyright © 2002. All Rights Reserved.

#include "StdAfx.h"
#include "CamCadDatabase.h"
#include "Graph.h"
#include "RwLib.h"
#include "WriteFormat.h"
#include "PcbUtil.h"
#include "Attribute.h"
#include <math.h>
#include "EntityNumber.h"
#include "Polygon.h"
#include "FlashRecognizer.h"

BOOL InfiniteRecursion(CCEtoODBDoc *doc);

Attrib* getAttribute(CAttributes*& attributeMap,int keyWordIndex)
{
   Attrib* attrib = NULL;

   if (attributeMap != NULL)
   {
      attributeMap->Lookup(keyWordIndex,attrib);
   }

   return attrib;
}

DataTypeMaskTag dataTypeToDataTypeMask(int dataType)
{
   DataTypeMaskTag retval = dataTypeMaskNone;

   switch (dataType)
   {
   case T_DRAW:    retval = dataTypeMaskDraw;    break;
   case T_INSERT:  retval = dataTypeMaskInsert;  break;
   case T_POLY:    retval = dataTypeMaskPoly;    break;
   case T_TEXT:    retval = dataTypeMaskText;    break;
 //case T_TOOL:    retval = dataTypeMaskTool;    break;
   case T_POINT:   retval = dataTypeMaskPoint;   break;
   case T_BLOB:    retval = dataTypeMaskBlob;    break;
   }

   return retval;
}

//_____________________________________________________________________________
bool CCamCadBlockMap::contains(const CString& blockName)
{
   bool retval = false;
   BlockStruct* block;

   if (Lookup(blockName,block))
   {
      retval = true;
   }

   return retval;
}

//_____________________________________________________________________________
CCamCadBlockMapArray::CCamCadBlockMapArray()
{
}

CCamCadBlockMapArray::~CCamCadBlockMapArray()
{
   deleteAll();
}

void CCamCadBlockMapArray::deleteAll()
{
   for (int ind = 0;ind < m_mapArray.GetSize();ind++)
   {
      CCamCadBlockMap* blockMap = m_mapArray.GetAt(ind);
      delete blockMap;
   }

   m_mapArray.RemoveAll();
}

CCamCadBlockMap& CCamCadBlockMapArray::getAt(int index)
{
   index++;  // array base is -1

   ASSERT(index >= 0);

   CCamCadBlockMap* mapArray = NULL;

   if (index < m_mapArray.GetSize())
   {
      mapArray = m_mapArray.GetAt(index);
   }

   if (mapArray == NULL)
   {
      mapArray = new CCamCadBlockMap();
      mapArray->InitHashTable(nextPrime2n(8000),false);
      m_mapArray.SetAtGrow(index,mapArray);
   }

   return *mapArray;
}

//_____________________________________________________________________________
CCamCadBlockIndex::CCamCadBlockIndex(CCEtoODBDoc& camCadDoc) :
   m_camCadDoc(camCadDoc)
{
   m_volatileChangeDetectedCnt = 0;
   m_indexedFlag = false;
}

BlockStruct* CCamCadBlockIndex::getDefinedBlock(const CString& name,BlockTypeTag blockType,int fileNum)
{
   verifyIndex();

   CCamCadBlockMap& mapArray = m_blockMapArray.getAt(fileNum);
   BlockStruct* block=NULL;

   if (mapArray.Lookup(name,block))
   {
      if (block->getName() != name || block->getFileNumber() != fileNum)
      {
         m_volatileChangeDetectedCnt++;
         mapArray.RemoveKey(name);
         block = NULL;
      }
   }

   if (block == NULL)
   {
      block = Graph_Block_Exists(&m_camCadDoc,name,fileNum);

      if (block == NULL)
      {
         block = m_camCadDoc.Add_Blockname(name,fileNum,0,0);
         block->setBlockType(blockType);
      }

      mapArray.SetAt(name,block);
   }

   return block;
}

bool CCamCadBlockIndex::addBlock(BlockStruct* newBlock,int fileNum)
{
   bool retval = false;

   verifyIndex();

   CCamCadBlockMap& mapArray = m_blockMapArray.getAt(fileNum);
   BlockStruct* block = NULL;

   if (! mapArray.Lookup(newBlock->getName(),block))
   {
      mapArray.SetAt(newBlock->getName(),newBlock);

      retval = true;
   }

   return retval;
}

BlockStruct* CCamCadBlockIndex::getBlock(const CString& name,int fileNum)
{
   verifyIndex();

   CCamCadBlockMap& mapArray = m_blockMapArray.getAt(fileNum);
   BlockStruct* block=NULL;

   if (mapArray.Lookup(name,block))
   {
      if (block->getName() != name || block->getFileNumber() != fileNum)
      {
         m_volatileChangeDetectedCnt++;
         mapArray.RemoveKey(name);
         block = Graph_Block_Exists(&m_camCadDoc,name,fileNum);

         if (block != NULL)
         {
            mapArray.SetAt(name,block);
         }
      }
   }

   return block;
}

BlockStruct* CCamCadBlockIndex::copyBlock(const CString& newName,BlockStruct* srcBlock,bool copyAttributesFlag)
{
   BlockStruct* block = getDefinedBlock(newName,srcBlock->getBlockType(),srcBlock->getFileNumber());

   block->setProperties(*srcBlock,copyAttributesFlag);
   block->setName(newName);

   return block;
}

BlockStruct* CCamCadBlockIndex::copyBlock(const CString& newName,FileStruct& fileStruct,BlockStruct* srcBlock,bool copyAttributesFlag)
{
   BlockStruct* block = getDefinedBlock(newName,srcBlock->getBlockType(),fileStruct.getFileNumber());

   block->setProperties(*srcBlock,copyAttributesFlag);
   block->setName(newName);

   return block;
}

void CCamCadBlockIndex::reindex()
{
   m_blockMapArray.deleteAll();

   for (int blockIndex = 0;blockIndex < m_camCadDoc.getMaxBlockIndex();blockIndex++)
   {
      BlockStruct* block = m_camCadDoc.getBlockAt(blockIndex);

      if (block != NULL)
      {
         CCamCadBlockMap& blockMap = m_blockMapArray.getAt(block->getFileNumber());
         blockMap.SetAt(block->getName(),block);
      }
   }

   m_indexedFlag = true;
}

void CCamCadBlockIndex::invalidateIndex()
{
   m_blockMapArray.deleteAll();

   m_indexedFlag = false;
}

//_____________________________________________________________________________
//CCamCadKeywordDirectory::CCamCadKeywordDirectory(CCamCadDatabase& camCadDatabase) :
//   m_camCadDatabase(camCadDatabase),
//   m_keywordArray(camCadDatabase.getCamCadDoc().getKeyWordArray()),
//   m_valueMap(camCadDatabase.getCamCadDoc().ValueDictionary)
//{
//}
//
//CCamCadKeywordDirectory::~CCamCadKeywordDirectory()
//{
//}
//
//void CCamCadKeywordDirectory::resync()
//{
//   // resize the value map if needed
//   if (m_valueMap.GetSize() > .8 * m_valueMap.GetHashTableSize())
//   {
//      int newSize = 2 * m_valueMap.GetSize();
//
//      if (newSize < 100) newSize = 100;
//
//      newSize = nextPrime2n(newSize);
//
//      CString keyword;
//      void* value;
//      CMapStringToPtr tempValueMap;
//      tempValueMap.InitHashTable(newSize,true);
//
//      for (POSITION pos=m_valueMap.GetStartPosition();pos != NULL;)
//      {
//         m_valueMap.GetNextAssoc(pos,keyword,value);
//         tempValueMap.SetAt(keyword,value);
//      }
//
//      m_valueMap.RemoveAll();
//      m_valueMap.InitHashTable(newSize);
//
//      for (POSITION pos=tempValueMap.GetStartPosition();pos != NULL;)
//      {
//         tempValueMap.GetNextAssoc(pos,keyword,value);
//         m_valueMap.SetAt(keyword,value);
//      }
//   }
//
//   int newKeywordMapSize = max(m_keywordArray.GetSize(),m_keywordMap.GetCount());
//   newKeywordMapSize = nextPrime2n(newKeywordMapSize - 1);
//
//   m_keywordMap.RemoveAll();
//   m_keywordMap.InitHashTable(newKeywordMapSize);
//
//   for (int ind=0;ind < m_keywordArray.GetSize();ind++)
//   {
//      const KeyWordStruct* keywordEntry = m_keywordArray[ind];
//
//      m_keywordMap.SetAt(keywordEntry->getInKeyword(),ind);
//   }
//}
//
//// returns true if keywords were in sync
//bool CCamCadKeywordDirectory::sync()
//{
//   bool retval = (m_keywordMap.GetCount() == m_keywordArray.GetSize());
//
//   if (! retval)
//   {
//      resync();
//   }
//
//   return retval;
//}
//
//int CCamCadKeywordDirectory::getKeywordIndex(const CString& keyword)
//{
//   int retval = -1;
//
//   if (! m_keywordMap.Lookup(keyword,retval))
//   {
//      retval = -1;
//   }
//
//   return retval;
//}
//
//int CCamCadKeywordDirectory::getDefinedKeywordIndex(const CString& keyword,ValueTypeTag valueType,CWriteFormat& errorLog)
//{
//   int retval = getKeywordIndex(keyword);
//
//   if (retval < 0)
//   {
//      retval = m_camCadDatabase.registerKeyWord(keyword,1,valueType,errorLog);
//      m_keywordMap.SetAt(keyword,retval);
//   }
//
//   return retval;
//}
//
//bool CCamCadKeywordDirectory::addAttribute(CAttributes** attributeMap,const CString& keyword,
//   ValueTypeTag valueType,const CString& value,CWriteFormat& errorLog)
//{
//   int keywordIndex = getDefinedKeywordIndex(keyword,valueType,errorLog);
//
//   void* valuePointer;
//   int intValue;
//   double doubleValue;
//   char* endPtr;
//
//   switch (valueType)
//   {
//   case valueTypeString:
//   case valueTypeEmailAddress:
//   case valueTypeWebAddress:
//      valuePointer = (void*)((const char*)value);
//      break;
//   case valueTypeDouble:
//   case valueTypeUnitDouble:
//      doubleValue = strtod((const char*)value,&endPtr);
//
//      if (*endPtr != '\0')
//      {
//errorLog.writef(PrefixError,
//"could not convert string to double, '%s'.\n",
//(const char*)value);
//      }
//
//      valuePointer = &doubleValue;
//      break;
//   case valueTypeInteger:
//      intValue = strtol((const char*)value,&endPtr,10);
//
//      if (*endPtr != '\0')
//      {
//errorLog.writef(PrefixError,
//"could not convert string to int, '%s'.\n",
//(const char*)value);
//      }
//
//      valuePointer = &intValue;
//      break;
//   case valueTypeNone:
//      valuePointer = NULL;
//      break;
//   }
//
//   bool retval = (m_camCadDatabase.setAttrib(attributeMap,keywordIndex,valueType,valuePointer,attributeUpdateAppend,NULL,errorLog) != 0);
//
//   return retval;
//}

//_____________________________________________________________________________
CCamCadDeviceMap::CCamCadDeviceMap(FileStruct* file)
{
   m_file = file;
   m_typeStructs = NULL;
}

CCamCadDeviceMap::~CCamCadDeviceMap()
{
   delete m_typeStructs;
}

void CCamCadDeviceMap::resync()
{
   delete m_typeStructs;

   int size = max(100,(int)(1.3 * m_file->getTypeList().GetCount()));

   m_typeStructs = new CTypedPtrArrayWithMap<TypeStruct>(size,false);

   for (POSITION pos = m_file->getTypeList().GetHeadPosition();pos != NULL;)
   {
      TypeStruct* typeStruct = m_file->getTypeList().GetNext(pos);
      m_typeStructs->add(typeStruct->getName(),typeStruct);
   }
}

// returns true if the arrays are in sync
bool CCamCadDeviceMap::sync()
{
   bool retval = (m_typeStructs != NULL && 
                  m_file->getTypeList().GetCount() == m_typeStructs->getSize());

   if (! retval)
   {
      resync();
   }

   return retval;
}

TypeStruct* CCamCadDeviceMap::getType(const CString& deviceName)
{
   TypeStruct* typeStruct = NULL;

   sync();

   if (! m_typeStructs->lookup(deviceName,typeStruct))
   {
      typeStruct = NULL;
   }

   return typeStruct;
}

TypeStruct* CCamCadDeviceMap::getDefinedType(CCamCadDatabase& camCadDatabase,
   const CString& deviceName)
{
   TypeStruct* typeStruct = getType(deviceName);

   if (typeStruct == NULL)
   {
      typeStruct = m_file->getTypeList().addType(deviceName);

      //typeStruct            = new TypeStruct;
      //typeStruct->setEntityNumber(camCadDatabase.allocateEntityNumber());
      //typeStruct->getAttributesRef() = NULL;
      //typeStruct->setName(deviceName);
      typeStruct->setBlockNumber( -1);   // no name assigned

      m_typeStructs->add(deviceName,typeStruct);
      //m_file->getTypeList().AddTail(typeStruct);
   }

   return typeStruct;
}

//_____________________________________________________________________________
CCamCadDeviceDirectory::CCamCadDeviceDirectory()
{
}

TypeStruct* CCamCadDeviceDirectory::getType(FileStruct* file,const CString& deviceName)
{
   CCamCadDeviceMap* deviceMap = getAt(file);
   TypeStruct* typeStruct = deviceMap->getType(deviceName);

   return typeStruct;
}

TypeStruct* CCamCadDeviceDirectory::getDefinedType(FileStruct* file,
   CCamCadDatabase& camCadDatabase,const CString& deviceName)
{
   CCamCadDeviceMap* deviceMap = getAt(file);
   TypeStruct* typeStruct = deviceMap->getDefinedType(camCadDatabase,deviceName);

   return typeStruct;
}

CCamCadDeviceMap* CCamCadDeviceDirectory::getAt(FileStruct* file)
{
   CCamCadDeviceMap* deviceMap = NULL;

   if (file->getFileNumber() < m_deviceMapArray.GetSize())
   {
      deviceMap = m_deviceMapArray.GetAt(file->getFileNumber());
   }

   if (deviceMap == NULL)
   {
      deviceMap = new CCamCadDeviceMap(file);
      m_deviceMapArray.SetAtGrow(file->getFileNumber(),deviceMap);
   }

   return deviceMap;
}

//_____________________________________________________________________________
CCamCadNetMap::CCamCadNetMap(FileStruct* file)
{
   m_file       = file;
   m_netStructs = NULL;
}

CCamCadNetMap::~CCamCadNetMap()
{
   delete m_netStructs;
}

void CCamCadNetMap::resync()
{
   delete m_netStructs;

   int size = max(100,(int)(1.3 * m_file->getNetList().GetCount()));

   m_netStructs = new CTypedPtrArrayWithMap<NetStruct>(size,false);

   for (POSITION pos = m_file->getNetList().GetHeadPosition();pos != NULL;)
   {
      NetStruct* netStruct = m_file->getNetList().GetNext(pos);
      m_netStructs->add(netStruct->getNetName(),netStruct);
   }
}

// returns true if the arrays are in sync
bool CCamCadNetMap::sync()
{
   bool retval = (m_netStructs != NULL && 
                  m_file->getNetList().GetCount() == m_netStructs->getSize());

   if (! retval)
   {
      resync();
   }

   return retval;
}

NetStruct* CCamCadNetMap::getNet(const CString& netName)
{
   NetStruct* netStruct = NULL;

   sync();

   if (! m_netStructs->lookup(netName,netStruct))
   {
      netStruct = NULL;
   }

   return netStruct;
}

NetStruct* CCamCadNetMap::getDefinedNet(CCamCadDatabase& camCadDatabase,
   const CString& netName)
{
   NetStruct* netStruct = getNet(netName);

   if (netStruct == NULL)
   {
      netStruct = m_file->getNetList().addNet(netName);

      //netStruct            = new NetStruct;
      //netStruct->setEntityNumber(camCadDatabase.allocateEntityNumber());
      //netStruct->getAttributesRef() = NULL;
      //netStruct->setNetName(netName);
      //netStruct->setFlags(0);
      //netStruct->setHighlighted(false);

      m_netStructs->add(netName,netStruct);
      //m_file->getNetList().AddTail(netStruct);
   }

   return netStruct;
}

//_____________________________________________________________________________
CCamCadNetDirectory::CCamCadNetDirectory()
{
}

NetStruct* CCamCadNetDirectory::getNet(FileStruct* file,const CString& netName)
{
   CCamCadNetMap* netMap = getAt(file);
   NetStruct* netStruct = netMap->getNet(netName);

   return netStruct;
}

NetStruct* CCamCadNetDirectory::getDefinedNet(FileStruct* file,
   CCamCadDatabase& camCadDatabase,const CString& netName)
{
   CCamCadNetMap* netMap = getAt(file);
   NetStruct* netStruct = netMap->getDefinedNet(camCadDatabase,netName);

   return netStruct;
}

CCamCadNetMap* CCamCadNetDirectory::getAt(FileStruct* file)
{
   CCamCadNetMap* netMap = NULL;

   if (file->getFileNumber() < m_netMapArray.GetSize())
   {
      netMap = m_netMapArray.GetAt(file->getFileNumber());
   }

   if (netMap == NULL)
   {
      netMap = new CCamCadNetMap(file);
      m_netMapArray.SetAtGrow(file->getFileNumber(),netMap);
   }

   return netMap;
}

//_____________________________________________________________________________
CCamCadPin::CCamCadPin(NetStruct& net,CompPinStruct& compPin) :
   m_net(net),
   m_compPin(compPin)
{
}

//_____________________________________________________________________________
CCamCadPinMap::CCamCadPinMap(FileStruct* file)
{
   m_file           = file;
   //m_compPinStructs = NULL;
   m_camCadPins     = NULL;
}

CCamCadPinMap::~CCamCadPinMap()
{
   //delete m_compPinStructs;
   delete m_camCadPins;
}

void CCamCadPinMap::resync()
{
   //delete m_compPinStructs;
   delete m_camCadPins;
   int numPins = 0;

   for (POSITION pos = m_file->getNetList().GetHeadPosition();pos != NULL;)
   {
      NetStruct* netStruct = m_file->getNetList().GetNext(pos);
      numPins += netStruct->getCompPinCount();
   }

   int size = max(100,(int)(1.3 * numPins));

   m_camCadPins = new CTypedPtrArrayWithMap<CCamCadPin>(size,true);

   for (POSITION pos = m_file->getNetList().GetHeadPosition();pos != NULL;)
   {
      NetStruct* netStruct = m_file->getNetList().GetNext(pos);

      for (POSITION pinPos = netStruct->getHeadCompPinPosition();pinPos != NULL;)
      {
         CompPinStruct* compPinStruct = netStruct->getNextCompPin(pinPos);

         CCamCadPin* camCadPin = new CCamCadPin(*netStruct,*compPinStruct);
         m_camCadPins->add(camCadPin->getPinRef(),camCadPin);
      }
   }
}

// returns true if the arrays are in sync
bool CCamCadPinMap::sync()
{
   //bool retval = (m_compPinStructs != NULL && m_compPinStructs->getSize() > 0);
   bool retval = (m_camCadPins != NULL && m_camCadPins->getSize() > 0);

   //bool retval = (m_compPinStructs != NULL && 
   //               m_net->getCompPinCount() == m_compPinStructs->getSize());

   if (! retval)
   {
      resync();
   }

   return retval;
}

CCamCadPin* CCamCadPinMap::getCamCadPin(const CString& refDes,const CString& pinName)
{
   CCamCadPin* camCadPin = NULL;

   sync();

   if (!m_camCadPins->lookup(CompPinStruct::getPinRef(refDes,pinName),camCadPin))
   {
      camCadPin = NULL;
   }

   return camCadPin;
}

CCamCadPin* CCamCadPinMap::getCamCadPin(const CString& pinRef)
{
   CSupString pinName(pinRef);
   CString refDes = pinName.Scan(".");

   return getCamCadPin(refDes,pinName);
}

CompPinStruct* CCamCadPinMap::getPin(const CString& refDes,const CString& pinName)
{
   CompPinStruct* compPinStruct = NULL;
   CCamCadPin* camCadPin = getCamCadPin(refDes,pinName);

   if (camCadPin != NULL)
   {
      compPinStruct = camCadPin->getCompPin();
   }

   return compPinStruct;
}

CCamCadPin* CCamCadPinMap::getDefinedCamCadPin(CCamCadDatabase& camCadDatabase,
   const CString& refDes,const CString& pinName,const CString& netName)
{
   CCamCadPin* camCadPin = getCamCadPin(refDes,pinName);

   if (camCadPin == NULL)
   {     
      //CompPinStruct* compPinStruct = new CompPinStruct();

      //compPinStruct->setEntityNumber(camCadDatabase.allocateEntityNumber());
      //compPinStruct->getAttributesRef() = NULL;
      //compPinStruct->setOrigin(0.,0.);
      //compPinStruct->setRotationRadians(0.);  
      //compPinStruct->setMirror(0); 
      //compPinStruct->setPinCoordinatesComplete(false);   
      //compPinStruct->setVisible(VISIBLE_BOTH);
      //compPinStruct->setRefDes(refDes);
      //compPinStruct->setPinName(pinName);
      //
      //compPinStruct->setPadstackBlockNumber(-1);

      NetStruct* net = camCadDatabase.getDefinedNet(netName,m_file);

      CompPinStruct* compPinStruct = net->addCompPin(refDes,pinName);
      compPinStruct->setVisible(VISIBLE_BOTH);
      compPinStruct->setPadstackBlockNumber(-1);

      //net->getCompPinList().AddTail(compPinStruct);

      camCadPin = new CCamCadPin(*net,*compPinStruct);
      m_camCadPins->add(camCadPin->getPinRef(),camCadPin);
   }

   return camCadPin;
}

CompPinStruct* CCamCadPinMap::getDefinedPin(CCamCadDatabase& camCadDatabase,
   const CString& refDes,const CString& pinName,const CString& netName)
{
   CCamCadPin* camCadPin = getDefinedCamCadPin(camCadDatabase,refDes,pinName,netName);

   CompPinStruct* compPinStruct = camCadPin->getCompPin();

   return compPinStruct;
}

int CCamCadPinMap::getSize()
{
   sync();

   int size = m_camCadPins->getSize();

   return size;
}

CCamCadPin* CCamCadPinMap::getAt(int index)
{
   sync();

   CCamCadPin* pin = m_camCadPins->getAt(index);

   return pin;
}

//_____________________________________________________________________________
CCamCadPinDirectory::CCamCadPinDirectory()
{
}

CCamCadPin* CCamCadPinDirectory::getCamCadPin(FileStruct* file,
   const CString& refDes,const CString& pinName)
{
   CCamCadPinMap* pinMap = getAt(file);
   CCamCadPin* camCadPin = pinMap->getCamCadPin(refDes,pinName);

   return camCadPin;
}

CCamCadPin* CCamCadPinDirectory::getDefinedCamCadPin(FileStruct* file,CCamCadDatabase& camCadDatabase,
   const CString& refDes,const CString& pinName,const CString& netName)
{
   CCamCadPinMap* pinMap = getAt(file);
   CCamCadPin* camCadPin = pinMap->getDefinedCamCadPin(camCadDatabase,refDes,pinName,netName);

   return camCadPin;
}

CompPinStruct* CCamCadPinDirectory::getPin(FileStruct* file,
   const CString& refDes,const CString& pinName)
{
   CCamCadPinMap* pinMap = getAt(file);
   CompPinStruct* compPinStruct = pinMap->getPin(refDes,pinName);

   return compPinStruct;
}

CompPinStruct* CCamCadPinDirectory::getDefinedPin(FileStruct* file,CCamCadDatabase& camCadDatabase,
   const CString& refDes,const CString& pinName,const CString& netName)
{
   CCamCadPinMap* pinMap = getAt(file);
   CompPinStruct* compPinStruct = pinMap->getDefinedPin(camCadDatabase,refDes,pinName,netName);

   return compPinStruct;
}

CCamCadPinMap* CCamCadPinDirectory::getAt(FileStruct* file)
{
   CCamCadPinMap* pinMap = NULL;

   if (file->getFileNumber() < m_pinMapArray.GetSize())
   {
      pinMap = m_pinMapArray.GetAt(file->getFileNumber());
   }

   if (pinMap == NULL)
   {
      pinMap = new CCamCadPinMap(file);
      m_pinMapArray.SetAtGrow(file->getFileNumber(),pinMap);
   }

   return pinMap;
}

void CCamCadPinDirectory::discardCamCadPins(FileStruct& file)
{
   CCamCadPinMap* pinMap = NULL;

   if (file.getFileNumber() < m_pinMapArray.GetSize())
   {
      pinMap = m_pinMapArray.GetAt(file.getFileNumber());
   }

   if (pinMap != NULL)
   {
      delete pinMap;
      m_pinMapArray.SetAt(file.getFileNumber(),NULL);
   }
}

//_____________________________________________________________________________
CCamCadWidthDirectory::CCamCadWidthDirectory(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
, m_blockNumberMap(10,false)
{
   m_nextWidthIndex = -1;
}

CCamCadWidthDirectory::~CCamCadWidthDirectory()
{
}

void CCamCadWidthDirectory::resync()
{
   int newMapSize = max(100,2 * m_camCadDoc.getNextWidthIndex());
   newMapSize = nextPrime2n(newMapSize - 1);

   m_blockNumberMap.RemoveAll();
   m_blockNumberMap.InitHashTable(newMapSize);
   void* widthIndex;

   for (int ind=0;ind < m_camCadDoc.getNextWidthIndex();ind++)
   {
      BlockStruct* block = m_camCadDoc.getWidthTable().GetAt(ind);

      if (block != NULL)
      {
         int blockNumber = block->getBlockNumber();

         if (! m_blockNumberMap.Lookup(blockNumber,widthIndex))
         {
            m_blockNumberMap.SetAt(blockNumber,(void*)ind);
         }
      }
   }

   m_nextWidthIndex = m_camCadDoc.getNextWidthIndex();
}

// returns true if widths were in sync
bool CCamCadWidthDirectory::sync()
{
   bool retval = (m_nextWidthIndex == m_camCadDoc.getNextWidthIndex());

   if (! retval)
   {
      resync();
   }

   return retval;
}

int CCamCadWidthDirectory::getDefinedApertureIndex(const CString& apertureName,ApertureShapeTag apertureShape,
   double sizeA,double sizeB,double xOffset,double yOffset,double rotation)
{
   return getDefinedApertureIndex(-1,apertureName,apertureShape,sizeA,sizeB,xOffset,yOffset,rotation);
}

int CCamCadWidthDirectory::getDefinedApertureIndex(int fileNumber,const CString& apertureName,ApertureShapeTag apertureShape,
   double sizeA,double sizeB,double xOffset,double yOffset,double rotation)
{
   int dcode = 0;
   DbFlag flag = 0;
   bool overwriteValues = false;
   int err;

   sync();

   int apertureIndex = m_camCadDoc.Graph_Aperture(fileNumber,apertureName,apertureShape,sizeA,sizeB,
      xOffset,yOffset,rotation,dcode,flag,overwriteValues,&err);

   if (apertureIndex == m_nextWidthIndex)
   {
      BlockStruct* block = m_camCadDoc.getWidthTable().GetAt(apertureIndex);

      if (block != NULL)
      {
         m_blockNumberMap.SetAt(block->getBlockNumber(),(void*)apertureIndex);
      }

      m_nextWidthIndex++;
   }

   return apertureIndex;
}

int CCamCadWidthDirectory::getWidthIndex(int blockNum)
{
   int retval = -1;
   void* widthIndex;

   sync();

   if (m_blockNumberMap.Lookup(blockNum,widthIndex))
   {
      retval = (int)widthIndex;
   }

   return retval;
}

int CCamCadWidthDirectory::getWidthIndex(BlockStruct* block)
{
   int retval = -1;

   if (block != NULL)
   {
      retval = getWidthIndex(block->getBlockNumber());
   }

   return retval;
}

int CCamCadWidthDirectory::getDefinedWidthIndex(BlockStruct& block)
{
   sync();

   int apertureIndex = getWidthIndex(block.getBlockNumber());

   if (apertureIndex < 0)
   {
      apertureIndex = m_nextWidthIndex++;

      //apertureIndex = m_camCadDoc.getNextWidthIndex()++;
      //m_camCadDoc.getWidthTable().SetAtGrow(apertureIndex,&block);
      apertureIndex = m_camCadDoc.getWidthTable().Add(&block);

      m_blockNumberMap.SetAt(block.getBlockNumber(),(void*)apertureIndex);
   }

   return apertureIndex;
}

void CCamCadWidthDirectory::dump()
{
   int blockNumber;
   void* widthIndex;
   int index = 0;

   for (POSITION pos = m_blockNumberMap.GetStartPosition();pos != NULL;index++)
   {
      m_blockNumberMap.GetNextAssoc(pos,blockNumber,widthIndex);
   }
}

//_____________________________________________________________________________
CPolygonBlock::CPolygonBlock(const CPolygon& polygon,BlockStruct& block)
: CPolygon(polygon)
, m_block(block)
{
   //*((CPolygon*)this) = polygon;
}

//_____________________________________________________________________________
CCamCadPolygonApertureDirectory::CCamCadPolygonApertureDirectory(CCEtoODBDoc& camCadDoc) :
   m_camCadDoc(camCadDoc)
{
   m_synchronized = false;
}

BlockStruct* CCamCadPolygonApertureDirectory::lookup(const CPolygon& polygon,double tolerance)
{
   sync();
   BlockStruct* apertureBlock = NULL;

   CPolygonBlockList* polygonBlockList;
   CString polygonSignature = polygon.getSignature();

   if (m_polygonListMap.Lookup(polygonSignature,polygonBlockList))
   {
      for (POSITION pos = polygonBlockList->GetHeadPosition();pos != NULL;)
      {
         CPolygonBlock* polygonBlock = polygonBlockList->GetNext(pos);

         if (polygon.normalizedVerticesEqual(*polygonBlock,tolerance))
         {
            apertureBlock = &(polygonBlock->getBlock());
         }
      }
   }

   return apertureBlock;
}

void CCamCadPolygonApertureDirectory::setAt(const CPolygon& polygon,BlockStruct& apertureBlock)
{
   sync();
   CPolygonBlockList* polygonBlockList = NULL;
   CString polygonSignature = polygon.getSignature();

   m_polygonListMap.Lookup(polygonSignature,polygonBlockList);

   if (polygonBlockList == NULL)
   {
      polygonBlockList = new CPolygonBlockList();
      m_polygonListMap.SetAt(polygonSignature,polygonBlockList);
   }

   CPolygonBlock* polygonBlock = new CPolygonBlock(polygon,apertureBlock);

   polygonBlockList->AddTail(polygonBlock);
}

void CCamCadPolygonApertureDirectory::resync()
{
   m_polygonListMap.empty();
   m_synchronized = true;

   CString polygonAperturePrefix = getPolygonAperturePrefix();

   for (int blockIndex = 0;blockIndex < m_camCadDoc.getMaxBlockIndex();blockIndex++)
   {
      BlockStruct* aperture = m_camCadDoc.getBlockAt(blockIndex);

      if (aperture != NULL && aperture->isAperture() &&
          aperture->getName().Left(polygonAperturePrefix.GetLength()) == polygonAperturePrefix)
      {
         int apertureGeometrySubblockNumber = aperture->getComplexApertureSubBlockNumber();
         BlockStruct* apertureGeometry = m_camCadDoc.getBlockAt(apertureGeometrySubblockNumber);

         if (apertureGeometry != NULL && apertureGeometry->getDataCount() == 1)
         {
            DataStruct* data = apertureGeometry->getDataList().GetHead();

            if (data->getDataType() == dataTypePoly)
            {
               CPolyList* polyList = data->getPolyList();

               if (polyList->GetCount() == 1)
               {
                  CPoly* poly = polyList->GetHead();
               
                  CTMatrix matrix;
                  CPolygon polygon(m_camCadDoc.getPageUnits());
                  polygon.setVertices(*poly);
                  polygon.normalizeForSignature(matrix);
                  setAt(polygon,*aperture);
               }
            }
         }
      }
   }
}

// returns true if polygon apertures are in sync
bool CCamCadPolygonApertureDirectory::sync()
{
   bool retval = m_synchronized;

   if (! retval)
   {
      resync();
   }

   return retval;
}

//_____________________________________________________________________________
CClusterBlock::CClusterBlock(const CSegmentCluster& segmentCluster,BlockStruct& block) :
   m_block(block)
{
   m_segmentCluster = new CSegmentCluster(segmentCluster);
}

CClusterBlock::~CClusterBlock()
{
   delete m_segmentCluster;
}

//_____________________________________________________________________________
CCamCadClusterApertureDirectory::CCamCadClusterApertureDirectory(CCamCadDatabase& camCadDatabase)
: m_camCadDatabase(camCadDatabase)
{
   m_synchronized = false;
}

CCEtoODBDoc& CCamCadClusterApertureDirectory::getCamCadDoc()
{
   return m_camCadDatabase.getCamCadDoc();
}

BlockStruct* CCamCadClusterApertureDirectory::lookup(const CSegmentCluster& segmentCluster,double tolerance)
{
   sync();
   BlockStruct* apertureBlock = NULL;

   CClusterBlockList* clusterBlockList;
   CString clusterSignature = segmentCluster.getSignature();

   if (m_clusterListMap.Lookup(clusterSignature,clusterBlockList))
   {
      for (POSITION pos = clusterBlockList->GetHeadPosition();pos != NULL;)
      {
         CClusterBlock* clusterBlock = clusterBlockList->GetNext(pos);

         if (segmentCluster.normalizedVerticesEqual(clusterBlock->getSegmentCluster(),tolerance))
         {
            apertureBlock = &(clusterBlock->getBlock());
         }
      }
   }

   return apertureBlock;
}

void CCamCadClusterApertureDirectory::setAt(const CSegmentCluster& segmentCluster,BlockStruct& apertureBlock)
{
   sync();
   CClusterBlockList* clusterBlockList = NULL;
   CString clusterSignature = segmentCluster.getSignature();

   m_clusterListMap.Lookup(clusterSignature,clusterBlockList);

   if (clusterBlockList == NULL)
   {
      clusterBlockList = new CClusterBlockList();
      m_clusterListMap.SetAt(clusterSignature,clusterBlockList);
   }

   CClusterBlock* clusterBlock = new CClusterBlock(segmentCluster,apertureBlock);

   clusterBlockList->AddTail(clusterBlock);
}

void CCamCadClusterApertureDirectory::resync()
{
   m_clusterListMap.empty();
   m_synchronized = true;

   CString clusterAperturePrefix = getClusterAperturePrefix();

   for (int blockIndex = 0;blockIndex < getCamCadDoc().getMaxBlockIndex();blockIndex++)
   {
      BlockStruct* aperture = getCamCadDoc().getBlockAt(blockIndex);

      if (aperture != NULL && aperture->isAperture() &&
          aperture->getName().Left(clusterAperturePrefix.GetLength()) == clusterAperturePrefix)
      {
         int apertureGeometrySubblockNumber = aperture->getComplexApertureSubBlockNumber();
         BlockStruct* apertureGeometry = getCamCadDoc().getBlockAt(apertureGeometrySubblockNumber);
         CSegmentCluster* segmentCluster = NULL;

         if (apertureGeometry != NULL && apertureGeometry->getDataCount() == 1)
         {
            for (POSITION dataPos = apertureGeometry->getHeadDataPosition();dataPos != NULL;)
            {
               DataStruct* data = apertureGeometry->getNextData(dataPos);

               if (data->getDataType() == dataTypePoly)
               {
                  CPolyList* polyList = data->getPolyList();

                  for (POSITION polyPos = polyList->GetHeadPosition();polyPos != NULL;)
                  {
                     CPoly* poly = polyList->GetNext(polyPos);

                     if (poly->getPntList().GetCount() == 2)
                     {
                        POSITION polyPos = poly->getPntList().GetHeadPosition();
                        CPnt* pnt0 = poly->getPntList().GetNext(polyPos);
                        CPnt* pnt1 = poly->getPntList().GetNext(polyPos);
                        BlockStruct* widthBlock = getCamCadDoc().getWidthBlock(poly->getWidthIndex());
                        double width = getCamCadDoc().getWidth(poly->getWidthIndex());
                        bool roundedFlag = (widthBlock->getShape() == apertureRound);

                        CSegment* segment = new CSegment(m_camCadDatabase,CPoint2d(pnt0->x,pnt0->y),CPoint2d(pnt1->x,pnt1->y),width,roundedFlag);
                        CSegmentQfe* segmentQfe = new CSegmentQfe(*segment);

                        if (segmentCluster == NULL)
                        {
                           segmentCluster = new CSegmentCluster();
                        }

                        segmentCluster->merge(*segmentQfe);
                     }
                  }
               }
            }
         }

         if (segmentCluster != NULL)
         {                     
            CTMatrix matrix;
            segmentCluster->normalizeForSignature(matrix);
            setAt(*segmentCluster,*aperture);
         }
      }
   }
}

// returns true if cluster apertures are in sync
bool CCamCadClusterApertureDirectory::sync()
{
   bool retval = m_synchronized;

   if (! retval)
   {
      resync();
   }

   return retval;
}

//_____________________________________________________________________________
CCamCadLayerMask::CCamCadLayerMask(CCamCadDatabase& camCadDatabase) :
   m_camCadDatabase(camCadDatabase)
{
   m_layerMask.SetSize(0,50);
}

bool CCamCadLayerMask::add(CamCadLayerTag layerTag)
{
   return add(m_camCadDatabase.getLayerIndexIfDefined(layerTag));
}

bool CCamCadLayerMask::add(int layerIndex)
{
   bool retval = (layerIndex >= 0);

   if (retval)
   {
      m_layerMask.SetAtGrow(layerIndex,true);
   }

   return retval;
}

void CCamCadLayerMask::addAll()
{
   int layerCount = m_camCadDatabase.getCamCadDoc().getLayerCount();

   for (int layerIndex = 0;layerIndex < layerCount;layerIndex++)
   {
      m_layerMask.SetAtGrow(layerIndex,true);
   }
}

bool CCamCadLayerMask::remove(CamCadLayerTag layerTag)
{
   return remove(m_camCadDatabase.getLayerIndexIfDefined(layerTag));
}

bool CCamCadLayerMask::remove(int layerIndex)
{
   bool retval = (layerIndex >= 0);

   if (retval)
   {
      m_layerMask.SetAtGrow(layerIndex,false);
   }

   return retval;
}

bool CCamCadLayerMask::addLayerType(LayerTypeTag layerTypeTag)
{
   bool retval = false;
   CLayerArray& layerArray = m_camCadDatabase.getCamCadDoc().getLayerArray();

   for (int layerIndex = 0;layerIndex < layerArray.GetSize();layerIndex++)
   {
      LayerStruct* layer = layerArray.GetAt(layerIndex);

      if (layer != NULL && layer->getLayerType() == layerTypeTag)
      {
         m_layerMask.SetAtGrow(layerIndex,true);
         retval = true;
      }
   }

   return retval;
}

bool CCamCadLayerMask::removeLayerType(LayerTypeTag layerTypeTag)
{
   bool retval = false;
   CLayerArray& layerArray = m_camCadDatabase.getCamCadDoc().getLayerArray();

   for (int layerIndex = 0;layerIndex < layerArray.GetSize();layerIndex++)
   {
      LayerStruct* layer = layerArray.GetAt(layerIndex);

      if (layer->getLayerType() == layerTypeTag)
      {
         m_layerMask.SetAtGrow(layerIndex,false);
         retval = true;
      }
   }

   return retval;
}

bool CCamCadLayerMask::contains(CamCadLayerTag layerTag) const
{
   return contains(m_camCadDatabase.getLayerIndexIfDefined(layerTag));
}

bool CCamCadLayerMask::contains(int layerIndex) const
{
   bool retval = (layerIndex >= 0 && layerIndex < m_layerMask.GetSize());

   if (retval)
   {
      retval = (m_layerMask.GetAt(layerIndex) != 0);
   }

   return retval;
}

bool CCamCadLayerMask::isEmpty() const
{
   bool retval = true;

   for (int layerIndex = 0;layerIndex < m_layerMask.GetSize();layerIndex++)
   {
      if (m_layerMask.GetAt(layerIndex) != 0)
      {
         retval = false;
         break;
      }
   }

   return retval;
}

//_____________________________________________________________________________
CCamCadDatabase::CCamCadDatabase(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
, m_widthDirectory(camCadDoc)
, m_blockIndex(camCadDoc)
, m_polygonApertureDirectory(camCadDoc)
, m_clusterApertureDirectory(*this)
{
   initializeCamCadLayers();
   //m_keywordDirectory      = NULL;
   m_componentPinDirectory = NULL;
   reindex();
}

CCamCadDatabase::~CCamCadDatabase()
{
   //delete m_keywordDirectory;
   delete m_componentPinDirectory;
}

void CCamCadDatabase::reindex()
{
   m_blockIndex.reindex();
}

void CCamCadDatabase::initializeCamCadLayers()
{
   m_camCadLayerNames[ccLayerBoardOutline           ] = "Board Outline";
   m_camCadLayerNames[ccLayerFloat                  ] = "Floating Layer";
   m_camCadLayerNames[ccLayerDrillHoles             ] = "Drill Holes";
   m_camCadLayerNames[ccLayerDummyPads              ] = "Dummy Pads";  
   m_camCadLayerNames[ccLayerPadTop                 ] = "Pad Layer Top";
   m_camCadLayerNames[ccLayerPadBottom              ] = "Pad Layer Bottom";
   m_camCadLayerNames[ccLayerPadTopPd               ] = CString("Pad Layer Top"   ) + getPlacementDependantLayerNameSuffix();
   m_camCadLayerNames[ccLayerPadBottomPd            ] = CString("Pad Layer Bottom") + getPlacementDependantLayerNameSuffix();
   m_camCadLayerNames[ccLayerPadTopPdM              ] = CString("Pad Layer Top"   ) + getPlacementDependantMirroredLayerNameSuffix();
   m_camCadLayerNames[ccLayerPadBottomPdM           ] = CString("Pad Layer Bottom") + getPlacementDependantMirroredLayerNameSuffix();
   m_camCadLayerNames[ccLayerPadInner               ] = "Pad Layer Inner";
   m_camCadLayerNames[ccLayerPadAll                 ] = "Pad Layer All";
   m_camCadLayerNames[ccLayerPadFlash               ] = "Pad Flash";
   m_camCadLayerNames[ccLayerPadClearance           ] = "Pad Clearance";
   m_camCadLayerNames[ccLayerMaskTop                ] = "Solder Mask Top";
   m_camCadLayerNames[ccLayerMaskBottom             ] = "Solder Mask Bottom";
   m_camCadLayerNames[ccLayerMaskTopPd              ] = CString("Solder Mask Top"   ) + getPlacementDependantLayerNameSuffix();
   m_camCadLayerNames[ccLayerMaskBottomPd           ] = CString("Solder Mask Bottom") + getPlacementDependantLayerNameSuffix();
   m_camCadLayerNames[ccLayerMaskTopPdM             ] = CString("Solder Mask Top"   ) + getPlacementDependantMirroredLayerNameSuffix();
   m_camCadLayerNames[ccLayerMaskBottomPdM          ] = CString("Solder Mask Bottom") + getPlacementDependantMirroredLayerNameSuffix();

   m_camCadLayerNames[ccLayerSilkTop                ] = "Silkscreen Top";
   m_camCadLayerNames[ccLayerSilkBottom             ] = "Silkscreen Bottom";
   m_camCadLayerNames[ccLayerSilk                   ] = "Silkscreen";
   m_camCadLayerNames[ccLayerAssemblyTop            ] = "Assembly Top";
   m_camCadLayerNames[ccLayerAssemblyBottom         ] = "Assembly Bottom";
   m_camCadLayerNames[ccLayerAssembly               ] = "Assembly";
   m_camCadLayerNames[ccLayerPinAssemblyTop         ] = "Pin Assembly Top";
   m_camCadLayerNames[ccLayerPinAssemblyBottom      ] = "Pin Assembly Bottom";
   m_camCadLayerNames[ccLayerPinAssembly            ] = "Pin Assembly";
   m_camCadLayerNames[ccLayerPlaceTop               ] = "Place Top";
   m_camCadLayerNames[ccLayerPlaceBottom            ] = "Place Bottom";
   m_camCadLayerNames[ccLayerPlace                  ] = "Place";
   m_camCadLayerNames[ccLayerMountTop               ] = "Mount Top";
   m_camCadLayerNames[ccLayerMountBottom            ] = "Mount Bottom";
   m_camCadLayerNames[ccLayerMount                  ] = "Mount";
   m_camCadLayerNames[ccLayerLargeViaKeepOutTop     ] = "Large Via Keepout Top";
   m_camCadLayerNames[ccLayerLargeViaKeepOutBottom  ] = "Large Via Keepout Bottom";
   m_camCadLayerNames[ccLayerLargeViaKeepOut        ] = "Large Via Keepout";
   m_camCadLayerNames[ccLayerResistTop              ] = "Resist Top";
   m_camCadLayerNames[ccLayerResistBottom           ] = "Resist Bottom";
   m_camCadLayerNames[ccLayerResist                 ] = "Resist";
   m_camCadLayerNames[ccLayerResistInhibitTop       ] = "Resist Inhibit Top";
   m_camCadLayerNames[ccLayerResistInhibitBottom    ] = "Resist Inhibit Bottom";
   m_camCadLayerNames[ccLayerResistInhibit          ] = "Resist Inhibit";
   m_camCadLayerNames[ccLayerPasteTop               ] = "Solder Paste Top";
   m_camCadLayerNames[ccLayerPasteBottom            ] = "Solder Paste Bottom";
   m_camCadLayerNames[ccLayerStencilTop             ] = "Stencil Top";
   m_camCadLayerNames[ccLayerStencilBottom          ] = "Stencil Bottom";
   m_camCadLayerNames[ccLayerCentroidTop            ] = "Centroid Top";
   m_camCadLayerNames[ccLayerCentroidBottom         ] = "Centroid Bottom";
   m_camCadLayerNames[ccLayerEducatorBoardOutline   ] = "Educator Board Outline";
   m_camCadLayerNames[ccLayerEducatorCentroidTop    ] = "Educator Centroid Top";
   m_camCadLayerNames[ccLayerEducatorCentroidBottom ] = "Educator Centroid Bottom";
   m_camCadLayerNames[ccLayerEducatorMatchedTop     ] = "Educator Matched Top";
   m_camCadLayerNames[ccLayerEducatorMatchedBottom  ] = "Educator Matched Bottom";
   m_camCadLayerNames[ccLayerEducatorUnmatchedTop   ] = "Educator Unmatched Top";
   m_camCadLayerNames[ccLayerEducatorUnmatchedBottom] = "Educator Unmatched Bottom";
   m_camCadLayerNames[ccLayerEducatorSilkTop        ] = "Educator Silkscreen Top";
   m_camCadLayerNames[ccLayerEducatorSilkBottom     ] = "Educator Silkscreen Bottom";

   m_camCadLayerNames[ccLayerViaKeepOutN            ] = "Via Keepout";
   m_camCadLayerNames[ccLayerRouteKeepInN           ] = "Route Keepin";
   m_camCadLayerNames[ccLayerLineInhibitN           ] = "Line Inhibit";
   m_camCadLayerNames[ccLayerCopperInhibitN         ] = "Copper Inhibit";
   m_camCadLayerNames[ccLayerNegativeCopperN        ] = "Negative Copper";
   m_camCadLayerNames[ccLayerNegativeCopperInhibitN ] = "Negative Copper Inhibit";

   for (int ind = 0;ind < ccLayerUndefined;ind++)
   {
      m_camCadLayerIndexes[ind] = -1;
   }
}

CString CCamCadDatabase::getCamCadLayerName(CamCadLayerTag index)
{
   return m_camCadLayerNames[index];
}

int CCamCadDatabase::getCamCadLayerIndex(CamCadLayerTag index)
{
   return m_camCadLayerIndexes[index];
}

CamCadLayerTag CCamCadDatabase::getCamCadLayerTag(int layerIndex)
{
   int tagIndex;

   for (tagIndex = 0;tagIndex < ccLayerUndefined;tagIndex++)
   {
      if (m_camCadLayerIndexes[tagIndex] == layerIndex)
      {
         break;
      }
   }

   return (CamCadLayerTag)tagIndex;
}

CamCadLayerTag CCamCadDatabase::getOppositeCamCadLayerTag(CamCadLayerTag layerTag)
{
   CamCadLayerTag oppositeLayerTag = ccLayerUndefined;

   switch (layerTag)
   {
   case ccLayerMaskTop:                  oppositeLayerTag = ccLayerMaskBottom;               break;
   case ccLayerMaskBottom:               oppositeLayerTag = ccLayerMaskTop;                  break;
   case ccLayerPadTop:                   oppositeLayerTag = ccLayerPadBottom;                break;
   case ccLayerPadBottom:                oppositeLayerTag = ccLayerPadTop;                   break;
   case ccLayerSilkTop:                  oppositeLayerTag = ccLayerSilkBottom;               break;
   case ccLayerSilkBottom:               oppositeLayerTag = ccLayerSilkTop;                  break;
   case ccLayerPadTopPd:                 oppositeLayerTag = ccLayerPadBottomPd;              break;
   case ccLayerPadBottomPd:              oppositeLayerTag = ccLayerPadTopPd;                 break;
   case ccLayerPadTopPdM:                oppositeLayerTag = ccLayerPadBottomPdM;             break;
   case ccLayerPadBottomPdM:             oppositeLayerTag = ccLayerPadTopPdM;                break;
   case ccLayerMaskTopPd:                oppositeLayerTag = ccLayerMaskBottomPd;             break;
   case ccLayerMaskBottomPd:             oppositeLayerTag = ccLayerMaskTopPd;                break;
   case ccLayerMaskTopPdM:               oppositeLayerTag = ccLayerMaskBottomPdM;            break;
   case ccLayerMaskBottomPdM:            oppositeLayerTag = ccLayerMaskTopPdM;               break;
   case ccLayerAssemblyTop:              oppositeLayerTag = ccLayerAssemblyBottom;           break;
   case ccLayerAssemblyBottom:           oppositeLayerTag = ccLayerAssemblyTop;              break;
   case ccLayerPinAssemblyTop:           oppositeLayerTag = ccLayerPinAssemblyBottom;        break;
   case ccLayerPinAssemblyBottom:        oppositeLayerTag = ccLayerPinAssemblyTop;           break;
   case ccLayerPlaceTop:                 oppositeLayerTag = ccLayerPlaceBottom;              break;
   case ccLayerPlaceBottom:              oppositeLayerTag = ccLayerPlaceTop;                 break;
   case ccLayerMountTop:                 oppositeLayerTag = ccLayerMountBottom;              break;
   case ccLayerMountBottom:              oppositeLayerTag = ccLayerMountTop;                 break;
   case ccLayerLargeViaKeepOutTop:       oppositeLayerTag = ccLayerLargeViaKeepOutBottom;    break;
   case ccLayerLargeViaKeepOutBottom:    oppositeLayerTag = ccLayerLargeViaKeepOutTop;       break;
   case ccLayerResistTop:                oppositeLayerTag = ccLayerResistBottom;             break;
   case ccLayerResistBottom:             oppositeLayerTag = ccLayerResistTop;                break;
   case ccLayerPasteTop:                 oppositeLayerTag = ccLayerPasteBottom;              break;
   case ccLayerPasteBottom:              oppositeLayerTag = ccLayerPasteTop;                 break;
   case ccLayerStencilTop:               oppositeLayerTag = ccLayerStencilBottom;            break;
   case ccLayerStencilBottom:            oppositeLayerTag = ccLayerStencilTop;               break;
   case ccLayerCentroidTop:              oppositeLayerTag = ccLayerCentroidBottom;           break;
   case ccLayerCentroidBottom:           oppositeLayerTag = ccLayerCentroidTop;              break;
   case ccLayerEducatorCentroidTop:      oppositeLayerTag = ccLayerEducatorCentroidBottom;   break;
   case ccLayerEducatorCentroidBottom:   oppositeLayerTag = ccLayerEducatorCentroidTop;      break;
   case ccLayerEducatorMatchedTop:       oppositeLayerTag = ccLayerEducatorMatchedBottom;    break;
   case ccLayerEducatorMatchedBottom:    oppositeLayerTag = ccLayerEducatorMatchedTop;       break;
   case ccLayerEducatorUnmatchedTop:     oppositeLayerTag = ccLayerEducatorUnmatchedBottom;  break;
   case ccLayerEducatorUnmatchedBottom:  oppositeLayerTag = ccLayerEducatorUnmatchedTop;     break;
   case ccLayerEducatorSilkTop:          oppositeLayerTag = ccLayerEducatorSilkBottom;       break;
   case ccLayerEducatorSilkBottom:       oppositeLayerTag = ccLayerEducatorSilkTop;          break;
   case ccLayerResistInhibitTop:         oppositeLayerTag = ccLayerResistInhibitBottom;      break;
   case ccLayerResistInhibitBottom:      oppositeLayerTag = ccLayerResistInhibitTop;         break;
   }

   return oppositeLayerTag;
}

int CCamCadDatabase::getCamCadLayerType(CamCadLayerTag layerTag)
{
   int layerType = LAYTYPE_UNKNOWN;

   switch (layerTag)
   {
   case ccLayerBoardOutline:             layerType = LAYTYPE_BOARD_OUTLINE;          break;
   case ccLayerFloat:                    layerType = LAYTYPE_UNKNOWN;                break;
   case ccLayerDrillHoles:               layerType = LAYTYPE_DRILL;                  break;
   case ccLayerDummyPads:                layerType = LAYTYPE_UNKNOWN;                break;     
   case ccLayerPadTop:                   layerType = LAYTYPE_PAD_TOP;                break;
   case ccLayerPadBottom:                layerType = LAYTYPE_PAD_BOTTOM;             break;
   case ccLayerPadTopPd:                 layerType = LAYTYPE_PAD_TOP;                break;
   case ccLayerPadBottomPd:              layerType = LAYTYPE_PAD_BOTTOM;             break;
   case ccLayerPadTopPdM:                layerType = LAYTYPE_PAD_TOP;                break;
   case ccLayerPadBottomPdM:             layerType = LAYTYPE_PAD_BOTTOM;             break;
   case ccLayerPadInner:                 layerType = LAYTYPE_PAD_INNER;              break;
   case ccLayerPadAll:                   layerType = LAYTYPE_PAD_ALL;                break;
   case ccLayerPadFlash:                 layerType = LAYTYPE_UNKNOWN;                break;
   case ccLayerPadClearance:             layerType = LAYTYPE_PLANECLEARANCE;         break;

   case ccLayerMaskTop:                  layerType = LAYTYPE_MASK_TOP;               break;
   case ccLayerMaskBottom:               layerType = LAYTYPE_MASK_BOTTOM;            break;
   case ccLayerMaskTopPd:                layerType = LAYTYPE_MASK_TOP;               break;
   case ccLayerMaskBottomPd:             layerType = LAYTYPE_MASK_BOTTOM;            break;
   case ccLayerMaskTopPdM:               layerType = LAYTYPE_MASK_TOP;               break;
   case ccLayerMaskBottomPdM:            layerType = LAYTYPE_MASK_BOTTOM;            break;
   case ccLayerSilkTop:                  layerType = LAYTYPE_SILK_TOP;               break;
   case ccLayerSilkBottom:               layerType = LAYTYPE_SILK_BOTTOM;            break;
   case ccLayerSilk:                     layerType = LAYTYPE_UNKNOWN;                break;
   case ccLayerAssemblyTop:              layerType = LAYTYPE_COMPONENT_DFT_TOP;      break;
   case ccLayerAssemblyBottom:           layerType = LAYTYPE_COMPONENT_DFT_BOTTOM;   break;
   case ccLayerAssembly:                 layerType = LAYTYPE_UNKNOWN;                break;
   case ccLayerPinAssemblyTop:           layerType = LAYTYPE_TOP;                    break;
   case ccLayerPinAssemblyBottom:        layerType = LAYTYPE_BOTTOM;                 break;
   case ccLayerPinAssembly:              layerType = LAYTYPE_ALL;                    break;
   case ccLayerPlaceTop:                 layerType = LAYTYPE_TOP;                    break;
   case ccLayerPlaceBottom:              layerType = LAYTYPE_BOTTOM;                 break;
   case ccLayerPlace:                    layerType = LAYTYPE_UNKNOWN;                break;
   case ccLayerMountTop:                 layerType = LAYTYPE_TOP;                    break;
   case ccLayerMountBottom:              layerType = LAYTYPE_BOTTOM;                 break;
   case ccLayerMount:                    layerType = LAYTYPE_UNKNOWN;                break;
   case ccLayerLargeViaKeepOutTop:       layerType = LAYTYPE_TOP;                    break;
   case ccLayerLargeViaKeepOutBottom:    layerType = LAYTYPE_BOTTOM;                 break;
   case ccLayerLargeViaKeepOut:          layerType = LAYTYPE_UNKNOWN;                break;
   case ccLayerResistTop:                layerType = LAYTYPE_MASK_TOP;               break;
   case ccLayerResistBottom:             layerType = LAYTYPE_MASK_BOTTOM;            break;
   case ccLayerPasteTop:                 layerType = LAYTYPE_PASTE_TOP;              break;
   case ccLayerPasteBottom:              layerType = LAYTYPE_PASTE_BOTTOM;           break;
   case ccLayerStencilTop:               layerType = LAYTYPE_STENCIL_TOP;            break;
   case ccLayerStencilBottom:            layerType = LAYTYPE_STENCIL_BOTTOM;         break;
   case ccLayerCentroidTop:              layerType = LAYTYPE_CENTROID_TOP;           break;
   case ccLayerCentroidBottom:           layerType = LAYTYPE_CENTROID_BOTTOM;        break;

   case ccLayerEducatorBoardOutline:     layerType = LAYTYPE_BOARD_OUTLINE;          break;
   case ccLayerEducatorCentroidTop:      layerType = LAYTYPE_CENTROID_TOP;           break;
   case ccLayerEducatorCentroidBottom:   layerType = LAYTYPE_CENTROID_BOTTOM;        break;
   case ccLayerEducatorMatchedTop:       layerType = LAYTYPE_TOP;                    break;
   case ccLayerEducatorMatchedBottom:    layerType = LAYTYPE_BOTTOM;                 break;
   case ccLayerEducatorUnmatchedTop:     layerType = LAYTYPE_TOP;                    break;
   case ccLayerEducatorUnmatchedBottom:  layerType = LAYTYPE_BOTTOM;                 break;
   case ccLayerEducatorSilkTop:          layerType = LAYTYPE_SILK_TOP;               break;
   case ccLayerEducatorSilkBottom:       layerType = LAYTYPE_SILK_BOTTOM;            break;

   case ccLayerResist:                   layerType = LAYTYPE_UNKNOWN;                break;
   case ccLayerResistInhibitTop:         layerType = LAYTYPE_MASK_TOP;               break;
   case ccLayerResistInhibitBottom:      layerType = LAYTYPE_MASK_BOTTOM;            break;
   case ccLayerResistInhibit:            layerType = LAYTYPE_UNKNOWN;                break;
   }

   return layerType;
}

unsigned long CCamCadDatabase::getCamCadLayerAttributes(CamCadLayerTag layerTag)
{
   unsigned long attributes = 0;

   switch (layerTag)
   {
   case ccLayerPadTopPd:
   case ccLayerPadBottomPd:
   case ccLayerMaskTopPd:
   case ccLayerMaskBottomPd:
      attributes = LY_NEVERMIRROR;
      break;
   case ccLayerPadTopPdM:
   case ccLayerPadBottomPdM:
   case ccLayerMaskTopPdM:
   case ccLayerMaskBottomPdM:
      attributes = LY_MIRRORONLY;
      break;
   }

   return attributes;
}

bool CCamCadDatabase::mirrorLayers(int layerIndex0,int layerIndex1)
{
   bool retval = false;

   LayerStruct* layer0 = getLayerAt(layerIndex0);
   LayerStruct* layer1 = getLayerAt(layerIndex1);

   if (layer0 != NULL && layer1 != NULL)
   {
      if (layer0->getMirroredLayerIndex() == layer1->getLayerIndex() &&
          layer1->getMirroredLayerIndex() == layer0->getLayerIndex()     )
      {
         retval = true;
      }
      else
      {
         LayerStruct* mirroredLayer0 = getLayerAt(layer0->getMirroredLayerIndex());
         LayerStruct* mirroredLayer1 = getLayerAt(layer1->getMirroredLayerIndex());

         if (mirroredLayer0 != NULL && mirroredLayer1 != NULL &&
             mirroredLayer0->getMirroredLayerIndex() == layer0->getLayerIndex() &&
             mirroredLayer1->getMirroredLayerIndex() == layer1->getLayerIndex()    )
         {
            mirroredLayer0->setMirroredLayerIndex(mirroredLayer0->getLayerIndex());
            mirroredLayer1->setMirroredLayerIndex(mirroredLayer1->getLayerIndex());

            layer0->setMirroredLayerIndex(layer1->getLayerIndex());
            layer1->setMirroredLayerIndex(layer0->getLayerIndex());

            retval = true;
         }
      }
   }

   return retval;
}

int CCamCadDatabase::getFloatingLayerIndex()
{
   return getLayerIndex(ccLayerFloat);
}

void CCamCadDatabase::initializeCamCadLayer(CamCadLayerTag layerTag)
{
   if (layerTag != ccLayerUndefined)
   {
      int layerType = getCamCadLayerType(layerTag);
      bool floatFlag = (layerTag == ccLayerFloat);

      m_camCadLayerIndexes[layerTag] = getDefinedLayerIndex(m_camCadLayerNames[layerTag],floatFlag,layerType);

      CamCadLayerTag oppositeLayerTag = getOppositeCamCadLayerTag(layerTag);

      if (oppositeLayerTag != ccLayerUndefined)
      {
         int oppositeLayerType = getCamCadLayerType(oppositeLayerTag);
         m_camCadLayerIndexes[oppositeLayerTag] = getDefinedLayerIndex(m_camCadLayerNames[oppositeLayerTag],false,oppositeLayerType);

         switch (layerTag)
         {
         case ccLayerMaskTop:          
         case ccLayerMaskBottom:       
         case ccLayerPadTop:           
         case ccLayerPadBottom:        
         case ccLayerSilkTop:          
         case ccLayerSilkBottom:       
         case ccLayerAssemblyTop:   
         case ccLayerAssemblyBottom:
         case ccLayerPinAssemblyTop:   
         case ccLayerPinAssemblyBottom:
         case ccLayerEducatorCentroidTop:   
         case ccLayerEducatorCentroidBottom:
         //case ccLayerStencilTop:     // mirroring the stencil layers breaks the stencil generator - 20060425.1851 - knv
         //case ccLayerStencilBottom:
            m_camCadDoc.Graph_Level_Mirror(m_camCadLayerNames[layerTag],m_camCadLayerNames[oppositeLayerTag],NULL);
            break;
         }

         unsigned long attributes = getCamCadLayerAttributes(layerTag);

         if (attributes != 0)
         {
            getLayerAt(m_camCadLayerIndexes[layerTag        ])->setFlagBits(attributes);
            getLayerAt(m_camCadLayerIndexes[oppositeLayerTag])->setFlagBits(attributes);
         }
      }
   }
}

LayerStruct* CCamCadDatabase::getSingleVisibleLayer()
{
   LayerStruct* singleVisibleLayer = NULL;

   for (int layerIndex = 0;layerIndex < getCamCadDoc().getLayerArray().GetSize();layerIndex++)
   {
      LayerStruct* layer = getCamCadDoc().getLayerArray().GetAt(layerIndex);

      if (layer != NULL && layer->isVisible())
      {
         if (singleVisibleLayer != NULL)
         {
            singleVisibleLayer = NULL;
            break;
         }

         singleVisibleLayer = layer;
      }
   }

   return singleVisibleLayer;
}

CString CCamCadDatabase::getPlacementDependantLayerNameSuffix()
{
   return " (Placement Dependant)";
}

CString CCamCadDatabase::getPlacementDependantMirroredLayerNameSuffix()
{
   return " (Placement Dependant) mirrored";
}

bool CCamCadDatabase::isLayerCompatible(LayerStruct* layer,CamCadLayerTag layerTag)
{
   bool retval = false;

   if (layer != NULL && layer->getLayerType() == getCamCadLayerType(layerTag))
   {
      unsigned long attributes = getCamCadLayerAttributes(layerTag);

      if (layer->getLayerFlags() == attributes)
      {
         if ((layer->isFloating() != 0) == (layerTag == ccLayerFloat))
         {
            retval = true;
         }
      }
   }

   return retval;
}

void CCamCadDatabase::associateExistingLayers()
{
   CLayerArray& layerArray = m_camCadDoc.getLayerArray();

   for (int layerIndex = 0;layerIndex < layerArray.GetSize();layerIndex++)
   {
      LayerStruct* layer = layerArray.GetAt(layerIndex);

      if (layer != NULL && layer->getLayerIndex() == layerIndex)
      {
         if (getCamCadLayerTag(layerIndex) == ccLayerUndefined)
         {
            CamCadLayerTag associatedLayerTag = ccLayerUndefined;
            CamCadLayerTag oppositeLayerTag   = ccLayerUndefined;
            int oppositeLayerIndex = -1;

            for (int tagIndex = 0;tagIndex < ccLayerUndefined && associatedLayerTag == ccLayerUndefined;tagIndex++)
            {
               if (m_camCadLayerIndexes[tagIndex] < 0)
               {
                  CamCadLayerTag layerTag = (CamCadLayerTag)tagIndex;
                  oppositeLayerTag        = ccLayerUndefined;

                  if (isLayerCompatible(layer,layerTag))
                  {
                     oppositeLayerIndex = layer->getMirroredLayerIndex();

                     if (oppositeLayerIndex != layerIndex)
                     {
                        if (getCamCadLayerTag(oppositeLayerIndex) == ccLayerUndefined)
                        {
                           oppositeLayerTag = getOppositeCamCadLayerTag(layerTag);
                           LayerStruct* oppositeLayer = layerArray.GetAt(oppositeLayerIndex);

                           if (isLayerCompatible(oppositeLayer,oppositeLayerTag))
                           {
                              associatedLayerTag = layerTag;
                           }
                        }
                     }
                     else
                     {
                        associatedLayerTag = layerTag;
                     }
                  }
               }
            }

            if (associatedLayerTag != ccLayerUndefined)
            {
               m_camCadLayerIndexes[associatedLayerTag] = layerIndex;

               if (oppositeLayerTag != ccLayerUndefined)
               {
                  m_camCadLayerIndexes[oppositeLayerTag] = oppositeLayerIndex;
               }
            }
         }
      }
   }
}

void CCamCadDatabase::standardizeLayerNames()
{
   CLayerArray& layerArray = m_camCadDoc.getLayerArray();

   for (int tagIndex = 0;tagIndex < ccLayerUndefined;tagIndex++)
   {
      //CamCadLayerTag layerTag = (CamCadLayerTag)tagIndex;
      int layerIndex = m_camCadLayerIndexes[tagIndex];

      if (layerIndex >= 0)
      {
         LayerStruct* layer = layerArray.GetAt(layerIndex);

         layer->setName( m_camCadLayerNames[tagIndex]);
      }
   }
}

int CCamCadDatabase::deleteEntitiesOnLayer(CamCadLayerTag layerTag,unsigned int dataTypeMask)
{
   int deletedDataCount = -1;

   if (isLayerDefined(layerTag))
   {
      deletedDataCount = deleteEntitiesOnLayer(*(getLayer(layerTag)),dataTypeMask);
   }

   return deletedDataCount;
}

int CCamCadDatabase::deleteEntitiesOnLayer(LayerStruct& layerStruct,unsigned int dataTypeMask)
{
   int deletedDataCount = 0;

   for (int blockIndex=0;blockIndex < m_camCadDoc.getMaxBlockIndex();blockIndex++)
   {
      BlockStruct* block = m_camCadDoc.getBlockAt(blockIndex);

      if (block != NULL)
      {         
         for (POSITION dataPos = block->getDataList().GetHeadPosition();dataPos != NULL;)
         {
            POSITION oldDataPos = dataPos;
            DataStruct* data = block->getDataList().GetNext(dataPos);

            if (data->getLayerIndex() == layerStruct.getLayerIndex())
            {
               if ((dataTypeToDataTypeMask(data->getDataType()) & dataTypeMask) != 0)
               {
                  RemoveOneEntityFromDataList(&m_camCadDoc,&(block->getDataList()),data,oldDataPos);
                  deletedDataCount++;
               }
            }
            else if (data->getAttributesRef() != NULL)
            {               
               for (POSITION pos = data->getAttributesRef()->GetStartPosition();pos != NULL;)
               {
                  Attrib* attrib;
                  WORD keyword;
                  data->getAttributesRef()->GetNextAssoc(pos,keyword,attrib);

                  if (attrib->getLayerIndex() == layerStruct.getLayerIndex())
                  {
                     attrib->setVisible(false);
                     attrib->setLayerIndex(-1);
                  }
               }
            }
         }
      }
   }

   return deletedDataCount;
}

int CCamCadDatabase::deleteEntitiesOnLayers(const CCamCadLayerMask& layerMask,unsigned int dataTypeMask,
                                            COperationProgress* operationProgress)
{
   int deletedDataCount = 0;

   if (!layerMask.isEmpty())
   {
      if (operationProgress != NULL)
      {
         operationProgress->setLength(m_camCadDoc.getMaxBlockIndex());
      }

      for (int blockIndex=0;blockIndex < m_camCadDoc.getMaxBlockIndex();blockIndex++)
      {
         if (operationProgress != NULL)
         {
            operationProgress->incrementProgress();
         }

         BlockStruct* block = m_camCadDoc.getBlockAt(blockIndex);

         if (block != NULL)
         {         
            for (POSITION dataPos = block->getDataList().GetHeadPosition();dataPos != NULL;)
            {
               POSITION oldDataPos = dataPos;
               DataStruct* data = block->getDataList().GetNext(dataPos);

               if (layerMask.contains(data->getLayerIndex()))
               {
                  if ((dataTypeToDataTypeMask(data->getDataType()) & dataTypeMask) != 0)
                  {
                     RemoveOneEntityFromDataList(&m_camCadDoc,&(block->getDataList()),data,oldDataPos);
                     deletedDataCount++;
                  }
               }
               else if (data->getAttributesRef() != NULL)
               {               
                  for (POSITION pos = data->getAttributesRef()->GetStartPosition();pos != NULL;)
                  {
                     Attrib* attrib;
                     WORD keyword;
                     data->getAttributesRef()->GetNextAssoc(pos,keyword,attrib);

                     if (layerMask.contains(attrib->getLayerIndex()))
                     {
                        attrib->setVisible(false);
                        attrib->setLayerIndex(-1);
                     }
                  }
               }
            }
         }
      }
   }

   return deletedDataCount;
}

int CCamCadDatabase::deleteEntitiesOnLayers(const CCamCadLayerMask& layerMask,COperationProgress& operationProgress)
{
   return deleteEntitiesOnLayers(layerMask,0xffff,&operationProgress);
}

int CCamCadDatabase::deleteLayers(const CCamCadLayerMask& layerMask,COperationProgress* operationProgress)
{
   int deletedDataCount = deleteEntitiesOnLayers(layerMask,0xffff,operationProgress);

   for (int layerIndex = 0;layerIndex < m_camCadDoc.getLayerCount();layerIndex++)
   {
      if (layerMask.contains(layerIndex))
      {
         LayerStruct* layerStruct = m_camCadDoc.getLayerAt(layerIndex);
         m_camCadDoc.RemoveLayer(layerStruct);
      }
   }

   return deletedDataCount;
}

int CCamCadDatabase::deleteLayer(LayerStruct& layerStruct)
{
   int deletedDataCount = deleteEntitiesOnLayer(layerStruct);

   m_camCadDoc.RemoveLayer(&layerStruct);

   return deletedDataCount;
}

LayerStruct* CCamCadDatabase::getLayerAt(int layerIndex)
{
   return m_camCadDoc.getLayer(layerIndex);
}

LayerStruct* CCamCadDatabase::getLayer(CamCadLayerTag layerTag)
{
   if (m_camCadLayerIndexes[layerTag] < 0)
   {
      initializeCamCadLayer(layerTag);
   }

   LayerStruct* layerStruct = getLayerAt(m_camCadLayerIndexes[layerTag]);

   return layerStruct;
}

LayerStruct* CCamCadDatabase::getLayer(const CString& layerName) const
{
   return m_camCadDoc.getLayer(layerName);
}

int CCamCadDatabase::getLayerIndex(const CString& layerName)
{
   LayerStruct *lp = m_camCadDoc.getLayer(layerName);
   if (lp != NULL)
      return lp->getLayerIndex();

   return -1;
}

int CCamCadDatabase::getLayerIndex(CamCadLayerTag layerTag)
{
   return getLayer(layerTag)->getLayerIndex();
}

int CCamCadDatabase::getLayerIndexIfDefined(CamCadLayerTag layerTag)
{
   return m_camCadLayerIndexes[layerTag];
}

int CCamCadDatabase::getNumLayers() const
{
   return m_camCadDoc.getLayerArray().GetSize();
}

bool CCamCadDatabase::isLayerDefined(CamCadLayerTag layerTag)
{
   bool retval = (m_camCadLayerIndexes[layerTag] >= 0);

   return retval;
}

int CCamCadDatabase::allocateEntityNumber()
{
   return getCamCadData().allocateEntityNumber();
}

//
//void CCamCadDatabase::startFile(const CString& fileName,int sourceType)
//{
// m_board = Graph_File_Start(fileName,sourceType);
// m_board->blocktype = m_board->block_ptr->blocktype = BLOCKTYPE_PCB;
//}
//
//void CCamCadDatabase::prepareAddEntityToBoard()
//{
// m_camCadDoc.PrepareAddEntity(m_board);
//}
//
//void CCamCadDatabase::prepareAddEntityToPanel()
//{
// m_camCadDoc.PrepareAddEntity(m_panel);
//}
//
//CCamCadLayer& CCamCadDatabase::getDefinedLayer(const CString& layerName)
//{
// // bad coding
//   CCamCadLayer* layer = (CCamCadLayer*)m_camCadDoc.Add_Layer(layerName);
//
// return *layer;
//}

PageUnitsTag CCamCadDatabase::getPageUnits()
{
   return intToPageUnitsTag(m_camCadDoc.getSettings().getPageUnits());
}

double CCamCadDatabase::convertPageUnitsTo(PageUnitsTag units,double value)
{
   double retval = value * Units_Factor(m_camCadDoc.getSettings().getPageUnits(),units);

   return retval;
}

double CCamCadDatabase::convertToPageUnits(PageUnitsTag units,double value)
{
   double retval = value * Units_Factor(units,m_camCadDoc.getSettings().getPageUnits());

   return retval;
}

DataStruct* CCamCadDatabase::newPolyStruct(int layer,DbFlag flg,BOOL negative, GraphicClassTag graphicClass)
{
   ASSERT(layer >= 0);

   DataStruct *data = getCamCadData().getNewDataStruct(dataTypePoly);

   data->getAttributesRef() = NULL;
   //data->setEntityNumber(CEntityNumber::allocate());

   //data->setDataType(dataTypePoly);
   data->setLayerIndex(layer);
   data->setFlags(flg);
   data->setNegative(negative);

   data->setGraphicClass(graphicClass);

   data->setSelected(false);
   data->setMarked(false);
   data->setColorOverride(false);
   data->setOverrideColor(0);
   data->setHidden(false);

   data->getPolyList() = new CPolyList;

   return data;
}

DataStruct* CCamCadDatabase::newPolyData(const CPolygon& polygon,int layer,DbFlag flg,BOOL negative, GraphicClassTag graphicClass)
{
   DataStruct* data = newPolyStruct(layer,flg,negative,graphicClass);
   int widthIndex = getDefinedWidthIndex(0.);
   CPoly* poly = addClosedPoly(data,widthIndex);

   for (POSITION pos = polygon.getHeadPointPosition();pos != NULL;)
   {
      CPoint2d point = polygon.getNextPoint(pos);
      addVertex(poly,point.x,point.y);
   }

   poly->close();

   return data;
}

DataStruct* CCamCadDatabase::addPolyStruct(BlockStruct* parentBlock,
      int layer,DbFlag flg,BOOL negative, GraphicClassTag graphicClass)
{
   DataStruct *data = newPolyStruct(layer,flg,negative, graphicClass);

   if (parentBlock != NULL)
   {
      parentBlock->getDataList().AddTail(data);
   }

   return data;
}

DataStruct* CCamCadDatabase::addPolyStruct(CDataList& parentDataList,
      int layer,DbFlag flg,BOOL negative, GraphicClassTag graphicClass)
{
   DataStruct *data = newPolyStruct(layer,flg,negative,graphicClass);

   parentDataList.AddTail(data);

   return data;
}

CPoly* CCamCadDatabase::addPoly(DataStruct* polyStruct,int widthIndex,bool Filled,bool VoidPoly,bool Closed)
{
   // BEWARE! it is okay if polyStruct is NULL, the CPoly will still be allocated.
   // HOWEVER it will not have an owner, so code calling is responsible for freeing the CPoly memory.
   ASSERT(widthIndex >= 0);

   CPoly *poly = new CPoly;
   poly->setWidthIndex(widthIndex);
   poly->setFilled(Filled);
   poly->setVoid(VoidPoly);
   poly->setClosed(Closed || Filled);
   poly->setThermalLine(false);
   poly->setFloodBoundary(false);
   poly->setHidden(false);
   poly->setHatchLine(false);

   if (polyStruct != NULL)
      polyStruct->getPolyList()->AddTail(poly);

   return poly;
}

CPoly* CCamCadDatabase::addClosedPoly(DataStruct* polyStruct,int widthIndex)
{
   return addPoly(polyStruct,widthIndex,false,false,true);
}

CPoly* CCamCadDatabase::addOpenPoly(DataStruct* polyStruct,int widthIndex)
{
   return addPoly(polyStruct,widthIndex,false,false,false);
}

CPoly* CCamCadDatabase::addFilledPoly(DataStruct* polyStruct,int widthIndex)
{
   return addPoly(polyStruct,widthIndex,true,false,true);
}

CPoly* CCamCadDatabase::addVoidPoly(DataStruct* polyStruct,int widthIndex)
{
   return addPoly(polyStruct,widthIndex,true,true,true);
}

CPnt* CCamCadDatabase::addVertex(CPoly* poly,double x, double y, double bulge)
{
   // 572 is a bulge for 89.9 * 4 = 359.6
   // after this number, the bulge goes to infinity.
   // infinity 1*e16 is for 90 degree * 4 = a full circle
   if (fabs(bulge) > 572)
   {
      bulge = 0;  
      ASSERT(false);
   }

   CPnt *pnt = new CPnt;
   pnt->x = (DbUnit)x;
   pnt->y = (DbUnit)y;
   pnt->bulge = (DbUnit)bulge;

   //currentPoly->list.AddTail(pnt);
   poly->getPntList().AddTail(pnt);

   return pnt;
}

DataStruct* CCamCadDatabase::addText(BlockStruct* parentBlock,
   int layerIndex,const CString& text,double x,double y,double height,double charWidth,double angle,DbFlag flag,
   bool proportional,bool mirror,int oblique,bool negative,int penWidthIndex,bool specialChar)
{
   ASSERT(parentBlock != NULL);

   CDataList* oldDataList = GetCurrentDataList();

   SetCurrentDataList(&(parentBlock->getDataList()));

   DataStruct* dataBlock = Graph_Text(layerIndex,text,x,y,height,charWidth,angle,flag,proportional,mirror,
                                      oblique,negative,penWidthIndex,specialChar);

   SetCurrentDataList(oldDataList);

   return dataBlock;
}


DataStruct* CCamCadDatabase::addText(CDataList &parentDataList,
   int layerIndex,const CString& text,double x,double y,double height,double charWidth,double angle,DbFlag flag,
   bool proportional,bool mirror,int oblique,bool negative,int penWidthIndex,bool specialChar)
{
   CDataList* oldDataList = GetCurrentDataList();

   SetCurrentDataList( &parentDataList );

   DataStruct* dataBlock = Graph_Text(layerIndex,text,x,y,height,charWidth,angle,flag,proportional,mirror,
                                      oblique,negative,penWidthIndex,specialChar);

   SetCurrentDataList(oldDataList);

   return dataBlock;
}

FileStruct* CCamCadDatabase::getFile(int fileNum)
{
   return m_camCadDoc.Find_File(fileNum);
}

FileStruct* CCamCadDatabase::getSingleVisiblePcb()
{
   return getCamCadData().getSingleVisiblePcb();
}

FileStruct* CCamCadDatabase::getNewSingleVisiblePcb(const CString& fileName,int cadSourceType)
{
   FileStruct* pcbFileStruct = NULL;

   // make sure we are showing only one board   
   for (POSITION filePos = m_camCadDoc.getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* fileStruct = m_camCadDoc.getFileList().GetNext(filePos);

      if (fileStruct != NULL)
      {
         fileStruct->setShow(false);
      }
   }

   pcbFileStruct = m_camCadDoc.Add_File(fileName,cadSourceType);
   pcbFileStruct->setShow(true);
   pcbFileStruct->setBlockType(blockTypePcb);

   return pcbFileStruct;
}

FileStruct* CCamCadDatabase::getSingleVisiblePanel()
{
   FileStruct* panelFileStruct = NULL;

   // make sure we are showing only one board   
   for (POSITION filePos = m_camCadDoc.getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* fileStruct = m_camCadDoc.getFileList().GetNext(filePos);

      if (fileStruct->isHidden() || !fileStruct->isShown())
         continue;

      if (fileStruct->getBlockType() == blockTypePanel)
      {
         if (panelFileStruct == NULL)
         {
            panelFileStruct = fileStruct;
         }
         else
         {
            panelFileStruct = NULL;
            break;
         }
      }
   }

   return panelFileStruct;
}

FileStruct* CCamCadDatabase::getFirstVisibleFile()
{
   FileStruct* firstVisibleFileStruct = NULL;

   // make sure we are showing only one board   
   for (POSITION filePos = m_camCadDoc.getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* fileStruct = m_camCadDoc.getFileList().GetNext(filePos);

      if (fileStruct->isHidden() || !fileStruct->isShown())
      {
         continue;
      }

      firstVisibleFileStruct = fileStruct;

      break;
   }

   return firstVisibleFileStruct;
}

FileStruct* CCamCadDatabase::getSingleVisibleFile()
{
   FileStruct* visibleFileStruct = NULL;

   for (POSITION filePos = m_camCadDoc.getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* fileStruct = m_camCadDoc.getFileList().GetNext(filePos);

      if (fileStruct->isHidden() || !fileStruct->isShown())
      {
         continue;
      }

      if (visibleFileStruct != NULL)
      {
         visibleFileStruct = NULL;
         break;
      }

      visibleFileStruct = fileStruct;
   }

   return visibleFileStruct;
}

DataStruct* CCamCadDatabase::referenceBlock(BlockStruct* parentBlock,
   const char* blockName,int insertType,const char* refName,int layerIndex,int fileNum,double x,double y,
   double angleRadians,bool mirror,double scale,bool global)
{
   ASSERT(parentBlock != NULL);

   CDataList* oldDataList = GetCurrentDataList();

   SetCurrentDataList(&(parentBlock->getDataList()));

   DataStruct* dataBlock = m_camCadDoc.Graph_Block_Reference(blockName,refName,fileNum,x,y,angleRadians,
      (mirror ? MIRROR_ALL : 0),scale,layerIndex,global);

   dataBlock->getInsert()->setInsertType(insertType);

   SetCurrentDataList(oldDataList);

   return dataBlock;
}

DataStruct* CCamCadDatabase::referenceBlock(BlockStruct* parentBlock, BlockStruct* childBlock, int insertType,
   const char* refname, int layerIndex, double x, double y, double angle, bool mirror, double scale)
{
   ASSERT(parentBlock != NULL);
   ASSERT(childBlock != NULL);

   return referenceBlock(parentBlock->getDataList(), childBlock, insertType, refname, layerIndex, x, y, angle, mirror, scale);
}

DataStruct* CCamCadDatabase::referenceBlock(CDataList &parentDataList,BlockStruct* childBlock,int insertType,
   const char* refname,int layerIndex,double x,double y,double angle,bool mirror,double scale)
{
   ASSERT(childBlock != NULL);

   DataStruct* data = insertBlock(childBlock,insertType,refname,layerIndex,x,y,angle,mirror,scale);

   parentDataList.AddTail(data);

   return data;
}

DataStruct* CCamCadDatabase::insertBlock(BlockStruct* childBlock,int insertType,
   const char* refname,int layerIndex,double x,double y,double angle,bool mirror,double scale)
{
   ASSERT(childBlock != NULL);

   DataStruct* data = getCamCadData().getNewDataStruct(dataTypeInsert);

   data->getAttributesRef() = NULL;
   //data->setEntityNumber(CEntityNumber::allocate());

   //data->setDataType(dataTypeInsert);
   data->setSelected(false);
   data->setMarked(false);
   data->setNegative(false);
   data->setGraphicClass(graphicClassNormal);
   data->setFlags(0);
   data->setColorOverride(false);
   data->setOverrideColor(0);
   data->setHidden(false);

   // If layerIndex is -1 the block elements are placed on the layers which they are
   // defined on. If layerIndex > -1, all entities on floating layers are put on that layer.
   data->setLayerIndex(layerIndex);

   data->getInsert() = new InsertStruct;
   data->getInsert()->getShadingRef().On = FALSE;
   data->getInsert()->setOriginX(x);
   data->getInsert()->setOriginY(y);

   data->getInsert()->setAngle(angle);

   data->getInsert()->setMirrorFlags(mirror ? MIRROR_ALL : 0); // normal mirroring does both. If you want different for an insert, change ->mirror after call to Graph_Block_Ref().  see DBUTIL.H
   data->getInsert()->setPlacedBottom(mirror); // normal mirroring does both. If you want different for an insert, change ->mirror after call to Graph_Block_Ref().  see DBUTIL.H
   data->getInsert()->setScale(scale);
   //data->getInsert()->getBlockNumber() = doc->Get_Block_Num(block_name, filenum, global); 
   data->getInsert()->setBlockNumber(childBlock->getBlockNumber()); 
   //data->getInsert()->setInsertType(insertTypeUnknown);
   data->getInsert()->setInsertType(insertType);

   if (refname)
      data->getInsert()->setRefname(STRDUP(refname));
   else 
      data->getInsert()->setRefname(NULL);

   // inherit block's attributes
   //BlockStruct *b = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
   BlockStruct *b = childBlock;

   if (b->getAttributesRef() != NULL)
   {
      WORD keyword;
      Attrib* b_ptr;
      Attrib* i_ptr;
      CAttributeMapWrap dataAttribMap(data->getAttributesRef());

      for (POSITION pos = b->getAttributesRef()->GetStartPosition();pos != NULL;)
      {
         b->getAttributesRef()->GetNextAssoc(pos, keyword,b_ptr);

         i_ptr = b_ptr->allocateCopy();
         i_ptr->setInherited(true); // set the inherited flag

         dataAttribMap.addInstance(keyword,i_ptr);
      }
   }

   return data;
}

BlockStruct* CCamCadDatabase::getDefinedBlock(const CString& name,BlockTypeTag blockType,int fileNum)
{
   return &(getCamCadData().getDefinedBlock(name,blockType,fileNum));
}

BlockStruct* CCamCadDatabase::getBlock(const CString& name,int fileNum)
{
   return getCamCadData().getBlock(name,fileNum);
}

BlockStruct* CCamCadDatabase::getBlock(int blockIndex)
{
   BlockStruct* block = NULL;

   if (blockIndex >= 0 && blockIndex < m_camCadDoc.getMaxBlockIndex())
   {
      block = m_camCadDoc.getBlockAt(blockIndex);
   }

   return block;
}

CString CCamCadDatabase::getNewBlockName(const CString& prefix,const CString& suffixFormat,
   int fileNum,bool skipFirstSuffixFlag)
{
   return getCamCadData().getNewBlockName(prefix,suffixFormat,fileNum,skipFirstSuffixFlag);
}

CString CCamCadDatabase::getNewBlockName(CString prefixFormat,int fileNum)
{
   return getCamCadData().getNewBlockName(prefixFormat,fileNum);
}

BlockStruct* CCamCadDatabase::getNewBlock(const CString& prefix,const CString& suffixFormat,
   BlockTypeTag blockType,int fileNum)
{
   return getCamCadData().getNewBlock(prefix,suffixFormat,blockType,fileNum);
}

BlockStruct* CCamCadDatabase::getNewBlock(const CString& prefixFormat,
   BlockTypeTag blockType,int fileNum)
{
   CString name = getNewBlockName(prefixFormat,fileNum);
   BlockStruct* block = getDefinedBlock(name,blockType,fileNum);

   return block;
}

int CCamCadDatabase::getDefinedWidthIndex(double width)
{
   return m_camCadDoc.getDefinedWidthIndex(width);
}

int CCamCadDatabase::getDefinedSquareWidthIndex(double width)
{
   int err;

   return m_camCadDoc.Graph_Aperture("",T_SQUARE,width,0.,0.,0.,0.,0,BL_WIDTH,0,&err);
}

int CCamCadDatabase::getWidthIndex(int blockNum)
{
   return m_widthDirectory.getWidthIndex(blockNum);
}

int CCamCadDatabase::getWidthIndex(BlockStruct* block)
{
   return m_widthDirectory.getWidthIndex(block);
}

int CCamCadDatabase::getDefinedWidthIndex(BlockStruct& block)
{
   return m_widthDirectory.getDefinedWidthIndex(block);
}

BlockStruct* CCamCadDatabase::getDefinedTool(double toolSize)
{
   return Graph_FindTool(toolSize,0,true,true);
}

int CCamCadDatabase::getZeroWidthIndex() 
{ 
   int zeroWidthIndex = getDefinedWidthIndex(0.); 

   return zeroWidthIndex;
}

TypeStruct* CCamCadDatabase::getType(const CString& name,FileStruct *file)
{
   TypeStruct* typeStruct = m_deviceDirectory.getType(file,name);

   return typeStruct;
}

TypeStruct* CCamCadDatabase::getDefinedType(const CString& name,FileStruct *file)
{
   TypeStruct* typeStruct = m_deviceDirectory.getDefinedType(file,*this,name);

   return typeStruct;
}

NetStruct* CCamCadDatabase::getDefinedNet(const CString& netName,FileStruct* fileStruct) 
{
   return m_netDirectory.getDefinedNet(fileStruct,*this,netName);
}

NetStruct* CCamCadDatabase::getNet(const CString& netName,FileStruct* fileStruct) 
{
   return m_netDirectory.getNet(fileStruct,netName);
}

CompPinStruct* CCamCadDatabase::addCompPin(NetStruct* net,const CString& refDes,const CString& pinNumber)
{ 
   m_pinDirectory.empty();

   return add_comppin(0,net,refDes,pinNumber); 
}

CompPinStruct* CCamCadDatabase::getDefinedCompPin(NetStruct* net,const CString& refDes,const CString& pinNumber)
{ 
   CompPinStruct* compPinStruct = (net)?net->findCompPin(refDes, pinNumber, true):NULL;
   if(!compPinStruct)
      compPinStruct = addCompPin(net, refDes,pinNumber); 

   return compPinStruct;
}

CCamCadPin* CCamCadDatabase::getDefinedCamCadPin(FileStruct* file,const CString& refDes,const CString& pinName,
   const CString& netName)
{
   return m_pinDirectory.getDefinedCamCadPin(file,*this,refDes,pinName,netName);
}

CCamCadPin* CCamCadDatabase::getCamCadPin(FileStruct* file,const CString& refDes,const CString& pinName)
{
   return m_pinDirectory.getCamCadPin(file,refDes,pinName);
}

CompPinStruct* CCamCadDatabase::getDefinedPin(FileStruct* file,const CString& refDes,const CString& pinName,
   const CString& netName)
{
   return m_pinDirectory.getDefinedPin(file,*this,refDes,pinName,netName);
}

CompPinStruct* CCamCadDatabase::getPin(FileStruct* file,const CString& refDes,const CString& pinName)
{
   return m_pinDirectory.getPin(file,refDes,pinName);
}

void CCamCadDatabase::deletePin(FileStruct* file,const CString& refDes,const CString& pinName)
{ 
   m_pinDirectory.empty();

   RemoveCompPin(file,refDes,pinName);
}

void CCamCadDatabase::deletePins(FileStruct* file,const CString& refDes)
{ 
   m_pinDirectory.empty();

   RemoveCompPins(file,refDes);
}

void CCamCadDatabase::discardCamCadPins(FileStruct& file)
{
   m_pinDirectory.discardCamCadPins(file);
}

CString CCamCadDatabase::getUnusedPinNetName()
{
   return NET_UNUSED_PINS;
}

BlockStruct& CCamCadDatabase::generatePinnedComponent(BlockStruct& pinlessGeometry)
{
   BlockStruct* pinnedGeometry = &pinlessGeometry;

   if (pinlessGeometry.getPinCount() == 0)
   {
      CString baseGeometryName = pinlessGeometry.getName();

      if (baseGeometryName.IsEmpty())
      {
         baseGeometryName.Format("%d",pinlessGeometry.getBlockNumber());
      }

      pinnedGeometry                = getNewBlock(baseGeometryName + "_pin",blockTypePcbComponent,pinlessGeometry.getFileNumber());
      BlockStruct* padStackGeometry = getNewBlock("PS_" + baseGeometryName ,blockTypePadstack    ,pinlessGeometry.getFileNumber());

      referenceBlock(pinnedGeometry,padStackGeometry,insertTypePin,"1",-1);

      CDataList dataList(pinlessGeometry.getDataList(),getCamCadData());

      padStackGeometry->getDataList().takeData(dataList);
   }

   return *pinnedGeometry;
}

void CCamCadDatabase::restructurePadstackGeometry(CDataList& padstackDataList,const CString& padstackName,const CDataList& sourceDataList,
   int fileNumber,bool flattenRegularComplexAperturesFlag)
{
   int floatingLayerIndex = getCamCadDoc().getDefinedFloatingLayer()->getLayerIndex();
   CTypedPtrArrayContainer<CDataList*> layeredDataLists;

   // the new DataStructs will be contained in the CDataList elements in the layeredDataLists variable
   CDataList* flatDataList = sourceDataList.getFlattenedDataList(getCamCadData(),-1,flattenRegularComplexAperturesFlag);

   CTMatrix matrix;
   DataStruct* data;

   for (POSITION pos = flatDataList->GetHeadPosition();pos != NULL;)
   {
      POSITION oldPos = pos;
      data = flatDataList->GetNext(pos);
      flatDataList->RemoveAt(oldPos);

      int layerIndex = data->getLayerIndex() + 2;

      if (layerIndex < 0) layerIndex = 0;

      CDataList* dataListForLayer = NULL;

      if (layerIndex < layeredDataLists.GetSize())
      {
         dataListForLayer = layeredDataLists.GetAt(layerIndex);
      }

      if (dataListForLayer == NULL)
      {
         dataListForLayer = new CDataList(true);
         layeredDataLists.setAtGrow(layerIndex,dataListForLayer);
      }

      dataListForLayer->AddTail(data);
   }

   for (int ind = 0;ind < layeredDataLists.GetSize();ind++)
   {
      CDataList* dataListForLayer = layeredDataLists.GetAt(ind);

      if (dataListForLayer != NULL)
      {
         bool singleApertureFlag = false;
         int layerNumber = ind - 2;

         //if (writeFormat != NULL)
         //{
         //   writeFormat->writef("restructurePadStack() -- DataList for layerNumber=%d ----------\n",layerNumber);
         //   dataListForLayer->dump(*writeFormat);
         //}

         int apertureIndex;

         if (dataListForLayer->GetCount() == 1)
         {
            data = dataListForLayer->GetHead();

            if (data->getDataType() == dataTypeInsert)
            {
               InsertStruct* apertureInsert = data->getInsert();
               BlockStruct* apertureBlock = getBlock(apertureInsert->getBlockNumber());

               bool drillFlag = (apertureBlock->getBlockType() == blockTypeTooling   ||
                                 apertureBlock->getBlockType() == blockTypeDrillHole   );

               if (apertureBlock->isAperture() || drillFlag)
               {
                  singleApertureFlag = true;

                  DataStruct* padInsertData = insertBlock(apertureBlock,insertTypeUnknown,"",layerNumber,
                                                apertureInsert->getOriginX(),apertureInsert->getOriginY(),apertureInsert->getAngleRadians(),
                                                apertureInsert->getGraphicMirrored(),apertureInsert->getScale());

                  padstackDataList.AddTail(padInsertData);
               }
            }
         }

         if (!singleApertureFlag)
         {
            CString apertureName;
            apertureName.Format("AP_%s_%d",padstackName,layerNumber);

            BlockStruct* complexApertureBlock = getNewBlock(apertureName,blockTypePadshape);

            complexApertureBlock->getDataList().takeData(*dataListForLayer);

            for (CDataListIterator dataIterator(*complexApertureBlock);dataIterator.hasNext();)
            {
               DataStruct* data = dataIterator.getNext();
               data->setLayerIndex(floatingLayerIndex);
            }

            int err;
            apertureIndex = getCamCadDoc().Graph_Aperture(apertureName,apertureComplex,complexApertureBlock->getBlockNumber(),0.,0.,0.,0.,0,0,0,&err);
            BlockStruct* aperture = getCamCadDoc().getWidthBlock(apertureIndex);

            DataStruct* padstackData = insertBlock(aperture,insertTypeUnknown,"",layerNumber,0.,0.,0.,false,1.);

            padstackDataList.AddTail(padstackData);
         }
      }
   }
}

BlockStruct& CCamCadDatabase::restructureFiducialGeometry(const CString& fiducialGeometryName,BlockStruct& fiducialGeometry,int insertLayerIndex)
{
   BlockStruct* restructuredFiducialGeometry = &fiducialGeometry;
   int fileNumber = fiducialGeometry.getFileNumber();

   if (fiducialGeometry.getName().CompareNoCase(fiducialGeometryName) != 0)
   {
      restructuredFiducialGeometry = getBlock(fiducialGeometryName,fileNumber);

      if (restructuredFiducialGeometry == NULL)
      {
         CString fiducialPadstackName("PS_" + fiducialGeometryName);
         BlockStruct* padstackGeometry = getBlock(fiducialPadstackName);

         if (padstackGeometry == NULL)
         {
            padstackGeometry = getDefinedBlock(fiducialPadstackName,blockTypePadstack,fileNumber);
            restructurePadstackGeometry(padstackGeometry->getDataList(),fiducialGeometryName,fiducialGeometry.getDataList(),fiducialGeometry.getFileNumber());

            //int floatingLayerIndex = getCamCadDoc().getDefinedFloatingLayer()->getLayerIndex();
            //CTypedPtrArrayContainer<CDataList*> layeredDataLists;

            //// the new DataStructs will be contained in the CDataList elements in the layeredDataLists variable
            //CDataList* flatDataList = fiducialGeometry.getDataList().getFlattenedDataList(getCamCadDoc(),-1);

            //CTMatrix matrix;
            //DataStruct* data;

            //for (POSITION pos = flatDataList->GetHeadPosition();pos != NULL;)
            //{
            //   POSITION oldPos = pos;
            //   data = flatDataList->GetNext(pos);
            //   flatDataList->RemoveAt(oldPos);

            //   int layerIndex = data->getLayerIndex() + 2;

            //   if (layerIndex < 0) layerIndex = 0;

            //   CDataList* dataListForLayer = NULL;

            //   if (layerIndex < layeredDataLists.GetSize())
            //   {
            //      dataListForLayer = layeredDataLists.GetAt(layerIndex);
            //   }

            //   if (dataListForLayer == NULL)
            //   {
            //      dataListForLayer = new CDataList(true);
            //      layeredDataLists.setAtGrow(layerIndex,dataListForLayer);
            //   }

            //   dataListForLayer->AddTail(data);
            //}

            //for (int ind = 0;ind < layeredDataLists.GetSize();ind++)
            //{
            //   CDataList* dataListForLayer = layeredDataLists.GetAt(ind);

            //   if (dataListForLayer != NULL)
            //   {
            //      bool singleApertureFlag = false;
            //      int layerNumber = ind - 2;

            //      //if (writeFormat != NULL)
            //      //{
            //      //   writeFormat->writef("restructurePadStack() -- DataList for layerNumber=%d ----------\n",layerNumber);
            //      //   dataListForLayer->dump(*writeFormat);
            //      //}

            //      int apertureIndex;

            //      if (dataListForLayer->GetCount() == 1)
            //      {
            //         data = dataListForLayer->GetHead();

            //         if (data->getDataType() == dataTypeInsert)
            //         {
            //            InsertStruct* apertureInsert = data->getInsert();
            //            BlockStruct* apertureBlock = getBlock(apertureInsert->getBlockNumber());

            //            bool drillFlag = (apertureBlock->getBlockType() == blockTypeTooling   ||
            //                              apertureBlock->getBlockType() == blockTypeDrillHole   );

            //            if (apertureBlock->isAperture() || drillFlag)
            //            {
            //               singleApertureFlag = true;

            //               referenceBlock(padstackGeometry,apertureBlock,insertTypeUnknown,"",layerNumber,
            //                  apertureInsert->getOriginX(),apertureInsert->getOriginY(),apertureInsert->getAngleRadians(),
            //                  apertureInsert->getGraphicMirrored(),apertureInsert->getScale());
            //            }
            //         }
            //      }

            //      if (!singleApertureFlag)
            //      {
            //         CString apertureName;
            //         apertureName.Format("AP_%s_%d",fiducialGeometryName,layerNumber);

            //         BlockStruct* complexApertureBlock = getNewBlock(apertureName,blockTypePadshape);

            //         complexApertureBlock->getDataList().takeData(*dataListForLayer);

            //         for (CDataListIterator dataIterator(*complexApertureBlock);dataIterator.hasNext();)
            //         {
            //            DataStruct* data = dataIterator.getNext();
            //            data->setLayerIndex(floatingLayerIndex);
            //         }

            //         int err;
            //         apertureIndex = getCamCadDoc().Graph_Aperture(apertureName,apertureComplex,complexApertureBlock->getBlockNumber(),0.,0.,0.,0.,0,0,0,&err);
            //         BlockStruct* aperture = getCamCadDoc().getWidthBlock(apertureIndex);

            //         referenceBlock(padstackGeometry,aperture,insertTypeUnknown,"",layerNumber,
            //            0.,0.,0.,false,1.);
            //      }
            //   }
            //}
         }

         restructuredFiducialGeometry = 
            getNewBlock(fiducialGeometryName,blockTypeFiducial,fiducialGeometry.getFileNumber());

         referenceBlock(restructuredFiducialGeometry,padstackGeometry,insertTypePin,"1",-1,
            0.,0.,0.,false,1.);
      }
   }

   return *restructuredFiducialGeometry;
}

CComponentPin* CCamCadDatabase::getDefinedComponentPin(FileStruct& file,const CString& refDes,const CString& pinName)
{
   if (m_componentPinDirectory == NULL)
   {
      m_componentPinDirectory = new CComponentPinDirectory();
   }

   CComponentPin* componentPin = m_componentPinDirectory->getDefinedComponentPin(*this,file,refDes,pinName);

   return componentPin;
}

int CCamCadDatabase::getDefinedLayerIndex(const char* layerName,bool floatingFlag,int layerType)
{
   if (strlen(layerName) < 1 || layerName[0] == ' ')
   {
      AfxDebugBreak();
   }

   LayerStruct* layer = m_camCadDoc.Add_Layer(layerName);

   if (floatingFlag)
   {
      layer->setFloating(true);
   }
      
   if (layer->getLayerType() == LAYTYPE_UNKNOWN)
   {
      layer->setLayerType(layerType);
   }

   return layer->getLayerIndex();
}

LayerStruct* CCamCadDatabase::getDefinedLayer(const char* layerName,bool floatingFlag,int layerType)
{
   if (strlen(layerName) < 1 || layerName[0] == ' ')
   {
      AfxDebugBreak();
   }

   LayerStruct* layer = &(m_camCadDoc.getLayerArray().getDefinedLayer(layerName));

   if (floatingFlag)
   {
      layer->setFloating(true);
   }

   if (layerType != LAYTYPE_UNKNOWN && layer->getLayerType() == LAYTYPE_UNKNOWN)
   {
      layer->setLayerType(layerType);
   }

   return layer;
}

CString CCamCadDatabase::getNewLayerName(const CString& prefix,const CString& suffixFormat,bool skipFirstSuffixFlag)
{
   CString name,suffix;

   LayerStruct* layer;

   for (int ind=(skipFirstSuffixFlag ? -1 : 0);;ind++)
   {
      if (ind >= 0)
      {
         suffix.Format(suffixFormat,ind);
      }

      name  = prefix + suffix;

      if (name.GetLength() > 0)
      {
         layer = getLayer(name);

         if (layer == NULL)
         {
            break;
         }
      }
   }

   return name;
}

LayerStruct* CCamCadDatabase::getNewLayer(const CString& prefix,const CString& suffixFormat,bool skipFirstSuffixFlag,
   LayerTypeTag layerType,bool floatingFlag)
{
   CString name = getNewLayerName(prefix,suffixFormat,skipFirstSuffixFlag);
   LayerStruct* layer = getDefinedLayer(name,floatingFlag,layerType);

   return layer;
}

BlockStruct* CCamCadDatabase::copyBlock(const CString& newName,BlockStruct* srcBlock,bool copyAttributesFlag) 
{
   return m_blockIndex.copyBlock(newName,srcBlock,copyAttributesFlag);
}

BlockStruct* CCamCadDatabase::copyBlock(const CString& newName,FileStruct& fileStruct,BlockStruct* srcBlock,bool copyAttributesFlag) 
{
   return m_blockIndex.copyBlock(newName,fileStruct,srcBlock,copyAttributesFlag);
}

int CCamCadDatabase::getNumBlocks()
{
   return m_camCadDoc.getMaxBlockIndex();
}

void CCamCadDatabase::invalidateBlockIndex()
{
   m_blockIndex.invalidateIndex();
}

BlockStruct* CCamCadDatabase::getDefinedCentroidGeometry(int fileNum)
{
   CString centroidGeometryName("$centroid$");
   BlockStruct* centroidGeometry = getBlock(centroidGeometryName);

   if (centroidGeometry == NULL)
   {
      centroidGeometry = getDefinedBlock(centroidGeometryName,blockTypeCentroid,fileNum);
      LayerStruct* floatingLayer = getLayer(ccLayerFloat);

      DataStruct* polyStruct = addPolyStruct(centroidGeometry,floatingLayer->getLayerIndex(),0,false, graphicClassNormal);
      int widthIndex = getDefinedWidthIndex(0.);
      double radius = convertToPageUnits(pageUnitsInches,.005);

      CPoly* poly = addClosedPoly(polyStruct,widthIndex);

      addVertex(poly,-radius,0.,1.);
      addVertex(poly, radius,0.,1.);
      addVertex(poly,-radius,0.);

      poly = addOpenPoly(polyStruct,widthIndex);

      addVertex(poly,0.    ,radius);
      addVertex(poly,0.    ,0.);
      addVertex(poly,radius,0.);

      poly = addClosedPoly(polyStruct,widthIndex);

      addVertex(poly,   radius, .5*radius);
      addVertex(poly,   radius,-.5*radius);
      addVertex(poly,2.*radius,0.);
      addVertex(poly,   radius, .5*radius);
   }

   return centroidGeometry;
}

// returns true if an alternate type was not created
bool CCamCadDatabase::referenceDevice(const CString& deviceName,BlockStruct* geometry,FileStruct* file)
{
   bool retval = true;

   TypeStruct* deviceType = getDefinedType(deviceName,file);
   
   if (deviceType->getBlockNumber() == -1)
   {
      deviceType->setBlockNumber( geometry->getBlockNumber());
   }
   else if (deviceType->getBlockNumber() != geometry->getBlockNumber())
   {
      retval = false;

      for (int ind = 0;;ind++)
      {
         CString indString;

         if (ind > 0)
         {
            indString.Format("_%d",ind);
         }

         CString newDeviceName = deviceName + "_" + geometry->getName() + indString;

         TypeStruct* newDeviceType = getType(newDeviceName,file);

         if (newDeviceType != NULL)
         {
            if (newDeviceType->getBlockNumber() != geometry->getBlockNumber())
            {
               continue;
            }
         }
         else
         {
            newDeviceType = getDefinedType(newDeviceName,file);
            newDeviceType->setBlockNumber( geometry->getBlockNumber());
            getCamCadDoc().CopyAttribs(&(newDeviceType->getAttributesRef()),deviceType->getAttributesRef());

            break;
         }
      }
   }

   return retval;
}

int CCamCadDatabase::getDefinedApertureIndex(const CString& apertureName,ApertureShapeTag apertureShape,
   double sizeA,double sizeB,double xOffset,double yOffset,double rotation)
{
   return getDefinedApertureIndex(-1,apertureName,apertureShape,sizeA,sizeB,xOffset,yOffset,rotation);
}

int CCamCadDatabase::getDefinedApertureIndex(int fileNumber,const CString& apertureName,ApertureShapeTag apertureShape,
   double sizeA,double sizeB,double xOffset,double yOffset,double rotation)
{
   BlockStruct* apertureBlock = getBlock(apertureName);

   int apertureIndex = -1;

   if (apertureBlock != NULL && apertureBlock->isAperture())
   {
      if (apertureBlock->getShape() == apertureShape &&
          apertureBlock->getFileNumber() == fileNumber && 
          fpeq(apertureBlock->getSizeA(),sizeA) && 
          fpeq(apertureBlock->getSizeB(),sizeB) && 
          fpeq(apertureBlock->getXoffset(),xOffset) && 
          fpeq(apertureBlock->getYoffset(),yOffset) && 
          fpeq(apertureBlock->getRotationRadians(),rotation) )
      {
         apertureIndex = m_widthDirectory.getWidthIndex(apertureBlock);
      }
   }

   if (apertureIndex < 0)
   {
      apertureIndex = m_widthDirectory.getDefinedApertureIndex(fileNumber,apertureName,apertureShape,
                           sizeA,sizeB,xOffset,yOffset,rotation);

      BlockStruct* block = m_camCadDoc.getWidthTable().GetAt(apertureIndex);
      m_blockIndex.addBlock(block);
   }

   return apertureIndex;
}

BlockStruct* CCamCadDatabase::getDefinedAperture(const CString& apertureName,ApertureShapeTag apertureShape,
   double sizeA,double sizeB,double xOffset,double yOffset,double rotation)
{
   return getDefinedAperture(-1,apertureName,apertureShape,sizeA,sizeB,xOffset,yOffset,rotation);
}

BlockStruct* CCamCadDatabase::getDefinedAperture(int fileNumber,const CString& apertureName,ApertureShapeTag apertureShape,
   double sizeA,double sizeB,double xOffset,double yOffset,double rotation)
{
   int apertureIndex = getDefinedApertureIndex(fileNumber,apertureName,apertureShape,sizeA,sizeB,xOffset,yOffset,rotation);

   BlockStruct* aperture = getCamCadDoc().getWidthTable().GetAt(apertureIndex);

   ASSERT(aperture != NULL);

   return aperture;
}

BlockStruct* CCamCadDatabase::getDefinedPolygonAperture(CPolygon& polygon,double width,double tolerance,CTMatrix& matrix)
{
   BlockStruct* aperture = NULL;

   if (polygon.isValid())
   {
      polygon.normalizeForSignature(matrix);

      aperture = m_polygonApertureDirectory.lookup(polygon,tolerance);

      if (aperture == NULL)
      {
         CString polygonAperturePrefix = m_polygonApertureDirectory.getPolygonAperturePrefix();
         CString apertureNameFormat(polygonAperturePrefix + "%04d");
         CString apertureGeometryNameFormat("AP_" + apertureNameFormat);

         BlockStruct* complexApertureGeometry = getNewBlock(apertureGeometryNameFormat,blockTypePadshape);
         aperture = getNewBlock(apertureNameFormat,blockTypeUnknown);
         aperture->setComplexApertureSubBlockNumber(complexApertureGeometry->getBlockNumber());
         aperture->setShape(apertureComplex);
         aperture->setFlags(BL_APERTURE | BL_GLOBAL);

         bool debugFlag = true;

         if (debugFlag)
         {
            int signatureKeywordIndex = this->registerKeyword("Signature",valueTypeString);
            CString signature = polygon.getSignature();
            addAttribute(getAttributeMap(complexApertureGeometry),signatureKeywordIndex,signature);
         }

         int widthIndex = getDefinedWidthIndex(width);
         DataStruct* polyData = addPolyStruct(complexApertureGeometry,getFloatingLayerIndex(),0,false, graphicClassNormal);
         CPoly* poly = addFilledPoly(polyData,widthIndex);

         for (POSITION pos = polygon.getHeadPointPosition();pos != NULL;)
         {
            CPoint2d point = polygon.getNextPoint(pos);
            addVertex(poly,point.x,point.y);
         }

         poly->close();

         m_polygonApertureDirectory.setAt(polygon,*aperture);  

         // Add to width table
         //m_camCadDoc.getWidthTable().SetAtGrow(++m_camCadDoc.getNextWidthIndex(),aperture);
         int apertureWidthIndex = m_camCadDoc.getWidthTable().Add(aperture);
      }
   }

   return aperture;
}

BlockStruct* CCamCadDatabase::getDefinedPolyAperture(const CPoly& inputPoly,double width,double tolerance,CTMatrix& matrix)
{
   BlockStruct* aperture = NULL;
   CPoly poly(inputPoly);
   poly.simplifyBulges(1);
   CPolygon polygon(poly,getPageUnits());

   //if (true)
   //{
   //   static int debugCount = 0;
   //   FileStruct* pcbFile   = getSingleVisibleFile();

   //   if (pcbFile != NULL)
   //   {
   //      BlockStruct* pcbBlock = pcbFile->getBlock();

   //      DataStruct* polyData = addPolyStruct(pcbBlock,getLayer(ccLayerDummyPads)->getLayerIndex(),0,false,graphicClassNormal);
   //      polyData->getPolyList()->AddTail(new CPoly(poly));
   //   }
   //}

   //if (polygon.isValid())
   {
      polygon.normalizeForSignature(matrix);

      aperture = m_polygonApertureDirectory.lookup(polygon,tolerance);

      if (aperture == NULL)
      {
         CString polygonAperturePrefix = m_polygonApertureDirectory.getPolygonAperturePrefix();
         CString apertureNameFormat(polygonAperturePrefix + "%04d");
         CString apertureGeometryNameFormat("AP_" + apertureNameFormat);

         BlockStruct* complexApertureGeometry = getNewBlock(apertureGeometryNameFormat,blockTypePadshape);
         aperture = getNewBlock(apertureNameFormat,blockTypeUnknown);
         aperture->setComplexApertureSubBlockNumber(complexApertureGeometry->getBlockNumber());
         aperture->setShape(apertureComplex);
         aperture->setFlags(BL_APERTURE | BL_GLOBAL);

         bool debugFlag = true;

         if (debugFlag)
         {
            int signatureKeywordIndex = this->registerKeyword("Signature",valueTypeString);
            CString signature = polygon.getSignature();
            addAttribute(getAttributeMap(complexApertureGeometry),signatureKeywordIndex,signature);
         }

         DataStruct* polyData = addPolyStruct(complexApertureGeometry,getFloatingLayerIndex(),0,false, graphicClassNormal);
         CPoly* aperturePoly = new CPoly(inputPoly);

         CTMatrix iMatrix(matrix);
         iMatrix.invert();
         aperturePoly->transform(iMatrix);

         aperturePoly->setWidthIndex(getDefinedWidthIndex(width));
         polyData->getPolyList()->AddTail(aperturePoly);

         m_polygonApertureDirectory.setAt(polygon,*aperture);  

         // Add to width table
         //m_camCadDoc.getWidthTable().SetAtGrow(++m_camCadDoc.getNextWidthIndex(),aperture);
         int widthIndex = m_camCadDoc.getWidthTable().Add(aperture);
      }
   }

   return aperture;
}

BlockStruct* CCamCadDatabase::getDefinedClusterAperture(CSegmentCluster& segmentCluster,
   CPolyList& polyList,double width,double tolerance,CTMatrix& matrix)
{
   BlockStruct* aperture = NULL;
   segmentCluster.normalizeForSignature(matrix);

   aperture = m_clusterApertureDirectory.lookup(segmentCluster,tolerance);

   if (aperture == NULL)
   {
      CString clusterAperturePrefix = m_clusterApertureDirectory.getClusterAperturePrefix();
      CString apertureNameFormat(clusterAperturePrefix + "%04d");
      CString apertureGeometryNameFormat("AP_" + apertureNameFormat);

      BlockStruct* complexApertureGeometry = getNewBlock(apertureGeometryNameFormat,blockTypePadshape);
      aperture = getNewBlock(apertureNameFormat,blockTypeUnknown);
      aperture->setComplexApertureSubBlockNumber(complexApertureGeometry->getBlockNumber());
      aperture->setShape(apertureComplex);
      aperture->setFlags(BL_APERTURE | BL_GLOBAL);

      //bool debugFlag = true;

      //if (debugFlag)
      //{
      //   int signatureKeywordIndex = this->registerKeyword("Signature",valueTypeString);
      //   CString signature = polygon.getSignature();
      //   addAttribute(getAttributeMap(complexApertureGeometry),signatureKeywordIndex,signature);
      //}

      //for (POSITION pos = segmentCluster.getList().GetHeadPosition();pos != NULL;)
      //{
      //   CSegmentQfe* segmentQfe = segmentCluster.getList().GetNext(pos);
      //   CSegment& segment = segmentQfe->getSegment();
      //   int widthIndex = (segment.getRounded() ? getDefinedWidthIndex(segment.getWidth()) :
      //                                            getDefinedSquareWidthIndex(segment.getWidth()));

      //   DataStruct* polyData = addPolyStruct(complexApertureGeometry,getFloatingLayerIndex(),0,false,graphicClassNormal);
      //   CPoly* poly = addOpenPoly(polyData,widthIndex);
      //   addVertex(poly,segment.get0().x,segment.get0().y);
      //   addVertex(poly,segment.get1().x,segment.get1().y);
      //}

      DataStruct* polyData = addPolyStruct(complexApertureGeometry,getFloatingLayerIndex(),0,false, graphicClassNormal);
      polyData->getPolyList()->takeData(polyList);
      CTMatrix iMatrix(matrix);
      iMatrix.invert();
      polyData->getPolyList()->transform(iMatrix);

      m_clusterApertureDirectory.setAt(segmentCluster,*aperture);

      // Add to width table
      //m_camCadDoc.getWidthTable().SetAtGrow(++m_camCadDoc.getNextWidthIndex(),aperture);
      int widthIndex = m_camCadDoc.getWidthTable().Add(aperture);
   }

   return aperture;
}

extern BOOL LoadingDataFile;

/* RegisterKeyWord()
*
*  Description: Gets index to this KeyWord in the KeyWordArray.
*     If this KeyWord was not in the array, it adds it.
*
*  Parameter : str -> KeyWord
*              section -> 0=cc, 1=in, 2=out
*
*  Returns : index to KeyWord in KeyWordArray
*/
int CCamCadDatabase::registerKeyWord(CString keyword,int section,
   ValueTypeTag valueType,CWriteFormat& errorLog)
{
   return registerKeyWord(keyword,section,valueType,false,errorLog);
}

int CCamCadDatabase::registerHiddenKeyWord(CString keyword,int section,
   ValueTypeTag valueType,CWriteFormat& errorLog)
{
   return registerKeyWord(keyword,section,valueType,true,errorLog);
}

int CCamCadDatabase::getKeywordIndex(const char* keywordName)
{
   return getCamCadDoc().IsKeyWord(keywordName,1);
}

int CCamCadDatabase::registerKeyword(const char* keywordName,int valueType)
{
   return getCamCadDoc().RegisterKeyWord(keywordName,1,valueType);
}

int CCamCadDatabase::registerKeyWord(CString keyword,int section,
   ValueTypeTag valueType,bool hiddenFlag,CWriteFormat& errorLog)
{
   int   index;

   // no left or right blanks
   keyword.Trim();

   if (keyword.GetLength() == 0)
   {
      return -1;
   }

   if ((index = getCamCadDoc().IsKeyWord(keyword,section)) > -1)
   {
      ValueTypeTag oldValueType = intToValueTypeTag(getCamCadDoc().getKeyWordArray()[index]->getValueType());

      if (oldValueType != valueType)
      {
         if (LoadingDataFile)
         {
            getCamCadDoc().getKeyWordArray().SetValueType(index, valueType);
         }
         else
         {
errorLog.writef(PrefixError,
"Value type mismatch for keyword '%s', incoming value type of '%s' does not equal existing value type of '%s'\n",
(const char*)keyword,
(const char*)valueTypeTagToString(valueType),
(const char*)valueTypeTagToString(oldValueType));

            // need to make a new keyword, different from the internal one.
         }
      }

      return index;
   }

   // add to array
   return getCamCadDoc().getKeyWordArray().SetAtGrow(keyword, attribGroupUser, valueType, true, hiddenFlag);
}

/******************************************************************************
* SetAttrib()
*
*  Description: Sets values for keyword for a data (allocating AttribMap if necessary)
*
*  Parameter : data
*              keyword
*              void *value    --> int*, double*, char* (or NULL for VT_NONE)
*              method --> SA_OVERWRITE, SA_APPEND, or SA_RETURN
*
*  Returns :   TRUE if keyword existed
*              FALSE if adding new keyword to map
*/
bool CCamCadDatabase::setAttrib(CAttributes** map,int keyword,ValueTypeTag valueType,
   void *value,AttributeUpdateMethodTag method,Attrib **attribPtr,CWriteFormat& errorLog)
{
   // NOTE: in "old" CC files, Keyword Array is filled at the end, so can't index into KeyWordArray yet
   if (keyword == -1)
   {
      errorLog.writef(PrefixError,"Wrong Attribute Keyword Register");  

      return false;
   }

   if (!LoadingDataFile)
   {
      ValueTypeTag documentValueType = intToValueTypeTag(getCamCadDoc().getKeyWordArray()[keyword]->getValueType());

      if (valueType != documentValueType)
      {
errorLog.writef(PrefixError,
"CCamCadDatabase::setAttrib() - Attribute Value Type [%s] - Keyword Value Type [%s]\n",
(const char*)valueTypeTagToString(valueType),
(const char*)valueTypeTagToString(documentValueType));

         return false;
      }
   }

   if (*map == NULL)
   {
      *map = new CAttributes();
   }

   bool result;
   Attrib* attrib;

   // is keyword in map 
   if ((*map)->Lookup(keyword, attrib))
   {
      if (attribPtr != NULL)
      {
         *attribPtr = attrib;
      }

      result = true;

      if (method == attributeUpdateFenced)
      {
         return true;
      }
   }
   else // doesn't exist, add keyword to map
   {
      attrib = getCamCadData().constructAttribute();  

      if (attribPtr != NULL)
      {
         *attribPtr = attrib;
      }

      (*map)->SetAt(keyword,attrib);

      result = false;
   }

   switch (valueType)
   {
   case valueTypeInteger:
      attrib->setIntValue(*((int*)value));
   break;
   case valueTypeDouble:
   case valueTypeUnitDouble:
      attrib->setDoubleValue(*((double*)value));
   break;
   case valueTypeNone:
      attrib->setDoubleValue(0.0);
   break;
   case valueTypeString:
   case valueTypeEmailAddress:
   case valueTypeWebAddress:
      // already had a string value for this keyword
      if (result && method == attributeUpdateAppend && attrib->getStringValueIndex() != -1)
      {
         //CString temp = attrib->getStringValue();
         CString temp = attrib->getStringValue();

         // same attribute with same keyword with same value already exist - do not double up.
         // result == TRUE means the keyword already exist.
         if (value && temp.CompareNoCase((char *)value) != 0)
         {
            temp += "\n";
            temp += (char*)value;
            attrib->setStringValueIndex(getCamCadDoc().RegisterValue(temp));
         }
         else
         {
            int i = 0; // here double
         }
      }
      else // adding a new keyword and value
      {
         if ((char*)value != NULL)
         {
            attrib->setStringValueIndex(getCamCadDoc().RegisterValue((char*)value));
         }
         else
         {
            attrib->setStringValueIndex(getCamCadDoc().RegisterValue((char*)""));
         }
      }

      break;
   }

   attrib->setValueType(valueType);
   attrib->setCoordinate(0.,0.);
   attrib->setRotationRadians(0);
   attrib->setHeight(0);
   attrib->setWidth(0);
   attrib->setProportionalSpacing(false);
   attrib->setPenWidthIndex(0);
   attrib->setMirrorDisabled(false);
   attrib->setVisible(false);
   attrib->setFlags(0);
   attrib->setLayerIndex(0);
   attrib->setInherited(false);
   attrib->setHorizontalPosition(horizontalPositionLeft);
   attrib->setVerticalPosition(verticalPositionBaseline);

   return result;
}

// keyword index based access
void CCamCadDatabase::addAttribute(CAttributes** attributeMap,int keyWordIndex,
   const CString& attributeValue,CWriteFormat& errorLog)
{
   setAttrib(attributeMap,keyWordIndex,
      valueTypeString,(void *)((const char*)attributeValue),attributeUpdateAppend,NULL,errorLog);
}

void CCamCadDatabase::addAttribute(CAttributes** attributeMap,int keyWordIndex,
   int attributeValue,CWriteFormat& errorLog)
{
   setAttrib(attributeMap,keyWordIndex,
      valueTypeInteger,(void *)(&attributeValue),attributeUpdateAppend,NULL,errorLog);
}

void CCamCadDatabase::addAttribute(CAttributes** attributeMap,int keyWordIndex,
   double attributeValue,CWriteFormat& errorLog)
{
   setAttrib(attributeMap,keyWordIndex,
      valueTypeDouble,(void *)(&attributeValue),attributeUpdateAppend,NULL,errorLog);
}

void CCamCadDatabase::addAttribute(CAttributes** attributeMap,int keyWordIndex,
   float attributeValue,CWriteFormat& errorLog)
{
   double doubleAttributeValue = (double)attributeValue;

   setAttrib(attributeMap,keyWordIndex,
      valueTypeUnitDouble,(void *)(&doubleAttributeValue),attributeUpdateAppend,NULL,errorLog);
}

void CCamCadDatabase::addAttribute(CAttributes** attributeMap,int keyWordIndex,
   CWriteFormat& errorLog)
{
   setAttrib(attributeMap,keyWordIndex,
      valueTypeNone,(void *)(NULL),attributeUpdateAppend,NULL,errorLog);
}

Attrib* CCamCadDatabase::addAttribute(CAttributes& attributeMap,int keyWordIndex,
   const CString& attributeValue)
{
   CAttributes* attributes = &attributeMap;

   return addAttribute(&attributes,keyWordIndex,attributeValue);
}

Attrib* CCamCadDatabase::addAttribute(CAttributes** attributeMap,int keyWordIndex,
   const CString& attributeValue)
{
   Attrib* attribute = NULL;

   const KeyWordStruct* keyWordStruct = getKeywordAt(keyWordIndex);

	if (keyWordStruct != NULL)
	{
		void* value;
		double fValue;
		int iValue;

		switch (keyWordStruct->getValueType())
		{
		case valueTypeString:
		case valueTypeEmailAddress:
		case valueTypeWebAddress:
			value = (void*)(const char*)attributeValue;
			break;
		case valueTypeDouble:
		case valueTypeUnitDouble:
			fValue = atof(attributeValue);
			value = &fValue;
			break;
		case valueTypeInteger:
			iValue = atoi(attributeValue);
			value = &iValue;
			break;
		}

		getCamCadDoc().SetAttrib(attributeMap,keyWordIndex,keyWordStruct->getValueType(),value,attributeUpdateOverwrite,&attribute);
	}

   return attribute;
}

Attrib* CCamCadDatabase::addAttribute(CAttributes& attributes,int keyWordIndex,int attributeValue)
{
   Attrib* attribute = NULL;

   const KeyWordStruct* keyWordStruct = getKeywordAt(keyWordIndex);

	if (keyWordStruct != NULL)
	{
		void* value;
      double fValue;
      CString stringValue;

		switch (keyWordStruct->getValueType())
		{
		case valueTypeString:
		case valueTypeEmailAddress:
		case valueTypeWebAddress:
         stringValue.Format("%d",attributeValue);
			value = (void*)(const char*)stringValue;
			break;
		case valueTypeDouble:
		case valueTypeUnitDouble:
			fValue = attributeValue;
			value = &fValue;
			break;
		case valueTypeInteger:
			value = &attributeValue;
			break;
		}

		getCamCadDoc().SetAttrib(attributes,keyWordIndex,keyWordStruct->getValueType(),value,attributeUpdateOverwrite,&attribute);
	}

   return attribute;
}

Attrib* CCamCadDatabase::addAttribute(CAttributes& attributes,int keyWordIndex,double attributeValue)
{
   Attrib* attribute = NULL;

   const KeyWordStruct* keyWordStruct = getKeywordAt(keyWordIndex);

	if (keyWordStruct != NULL)
	{
		void* value;
      int iValue;
      CString stringValue;

		switch (keyWordStruct->getValueType())
		{
		case valueTypeString:
		case valueTypeEmailAddress:
		case valueTypeWebAddress:
         stringValue.Format("%f",attributeValue);
			value = (void*)(const char*)stringValue;
			break;
		case valueTypeDouble:
		case valueTypeUnitDouble:
			value = &attributeValue;
			break;
		case valueTypeInteger:
         iValue = round(attributeValue);
			value = &iValue;
			break;
		}

		getCamCadDoc().SetAttrib(attributes,keyWordIndex,keyWordStruct->getValueType(),value,attributeUpdateOverwrite,&attribute);
	}

   return attribute;
}

//CCamCadKeywordDirectory* CCamCadDatabase::getKeywordDirectory()
//{
//   if (m_keywordDirectory == NULL)
//   {
//      m_keywordDirectory = new CCamCadKeywordDirectory(*this);
//      m_keywordDirectory->resync();
//   }
//
//   return m_keywordDirectory;
//}

// keyword string based access
void CCamCadDatabase::addAttribute(CAttributes** attributeMap,const CString& keyword,
   ValueTypeTag valueType,const CString& value,CWriteFormat& errorLog)
{
   //getKeywordDirectory()->addAttribute(attributeMap,keyword,valueType,value,errorLog);

   if (attributeMap != NULL)
   {
      if (*attributeMap == NULL)
      {
         *attributeMap = new CAttributes();
      }

      int keywordIndex = getCamCadData().getKeyWordArray().getDefinedKeywordIndex(keyword,valueType);

      CAttributes& attributes = **attributeMap;

      CAttribute* attribute;

      if (!attributes.Lookup(keywordIndex,attribute))
      {
         attribute = new CAttribute(getCamCadData(),valueType);
         attributes.SetAt(keywordIndex,attribute);
      }

      attribute->setValueFromString(value);
   }
}

void CCamCadDatabase::removeAttribute(CAttributes** attributeMap,int keyWordIndex)
{
   if (attributeMap != NULL && *attributeMap != NULL)
   {
      (*attributeMap)->deleteAttribute(keyWordIndex);

      //void* voidPointer;

      //if ((*attributeMap)->Lookup(keyWordIndex,voidPointer))
      //{
      //   (*attributeMap)->RemoveKey(keyWordIndex);
      //   delete ((Attrib*)voidPointer);
      //}
   }
}

void CCamCadDatabase::removeAttribute(CAttributes** attributeMap,const CString& keyword)
{
   if (attributeMap != NULL && *attributeMap != NULL)
   {
      //int keyWordIndex = getKeywordDirectory()->getKeywordIndex(keyword);
      int keywordIndex = getCamCadData().getKeyWordArray().getKeywordIndex(keyword);

      (*attributeMap)->deleteAttribute(keywordIndex);
   }
}

CString CCamCadDatabase::getAttributeStringValue(Attrib* attribute)
{
   return m_camCadDoc.getAttributeStringValue(attribute);
}

bool CCamCadDatabase::getAttributeStringValue(CString& stringValue,CAttributes** attributes,int keywordIndex)
{
   return m_camCadDoc.getAttributeStringValue(stringValue,attributes,keywordIndex);
}

bool CCamCadDatabase::getAttributeStringValue(CString& stringValue,CAttributes& attributes,int keywordIndex)
{
   return m_camCadDoc.getAttributeStringValue(stringValue,attributes,keywordIndex);
}

const KeyWordStruct* CCamCadDatabase::getKeywordAt(int keywordIndex)
{
   const KeyWordStruct* keywordStruct = NULL;

   if (keywordIndex >= 0 && keywordIndex < m_camCadDoc.getKeyWordArray().GetSize())
   {
      keywordStruct = m_camCadDoc.getKeyWordArray()[keywordIndex];
   }

   return keywordStruct;
}

const KeyWordStruct* CCamCadDatabase::getKeyword(const CString& keyword)
{
   const KeyWordStruct* keywordStruct = getCamCadData().getKeyWordArray().getKeywordAt(keyword);

   return keywordStruct;
}

DRCStruct* CCamCadDatabase::addDrc(FileStruct& file,double x,double y,const CString& string,
   DrcClassTag drcClass,DrcFailureRangeTag failureRange,int algorithmIndex,
   DrcAlgorithmTag algorithmType,const CString& comment)
{
   DRCStruct* drc = AddDRC(&m_camCadDoc,&file,x,y,string,drcClass,failureRange,algorithmIndex,algorithmType);
   drc->setComment(comment);

   return drc;
}

DataStruct*  CCamCadDatabase::drawAxes(const CTMatrix& matrix,const CString& layerName)
{
   double u = .002;

   BlockStruct* pcbBlock = getFirstVisibleFile()->getBlock();
   int layerIndex = this->getDefinedLayerIndex(layerName);
   DataStruct* polyStruct = addPolyStruct(pcbBlock,layerIndex,0,false, graphicClassNormal);
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

      CPoly* poly = addOpenPoly(polyStruct,widthIndex);
      addVertex(poly,p0.x,p0.y);
      addVertex(poly,p1.x,p1.y);
   }

   return polyStruct;
}

bool CCamCadDatabase::hasRecursiveReference()
{
   bool retval = (InfiniteRecursion(&m_camCadDoc) != 0);

   return retval;
}

/*
void CCamCadDatabase::transmute(CDataStruct& data,CCamCadDatabase& sourceCamCadDatabase)
{
   CAttributes* attributes = data->getAttributeMap();

   if (attributes != NULL)
   {
      attributes->empty();
   }

   if (data->getDataType() == dataTypeInsert)
   {
      BlockStruct* subBlock = sourceCamCadDatabase.getBlock(data->getInsert()->getBlockNumber());
      BlockStruct* transmutedSubBlock = getBlock(subBlock->getName());

      if (transmutedSubBlock == NULL)
      {
         transmutedSubBlock = copyBlock(subBlock->getName(),subBlock,false);
         transmute(transmutedSubBlock->getDataList(),sourceCamCadDatabase);
      }

      data->getInsert()->setBlockNumber(transmutedSubBlock->getBlockNumber());

      while (transmutedSubBlock->isAperture() && transmutedSubBlock->getShape() == apertureComplex)
      {
         BlockStruct* apertureBlock = sourceCamCadDatabase.getBlock(subBlock->getComplexApertureSubBlockNumber());
         BlockStruct* transmutedApertureBlock = getBlock(apertureBlock->getName());

         if (transmutedApertureBlock == NULL)
         {
            transmutedApertureBlock = copyBlock(apertureBlock->getName(),apertureBlock,false);
            transmute(transmutedApertureBlock->getDataList(),sourceCamCadDatabase);
         }

         transmutedSubBlock->setComplexApertureSubBlockNumber(transmutedApertureBlock->getBlockNumber());
         transmutedSubBlock = transmutedApertureBlock;
      }
   }
   else if (data->getDataType() == dataTypePoly)
   {
      CPolyList* polyList = data->getPolyList();

      for (POSITION pos = polyList->GetHeadPosition();pos != NULL;)
      {
         CPoly* poly = polyList->GetNext(pos);

         int widthIndex = poly->getWidthIndex();
         double width = sourceCamCadDatabase.getCamCadDoc().getWidth(widthIndex);
         int transmutedWidthIndex = getDefinedWidthIndex(width);
         poly->setWidthIndex(transmutedWidthIndex);
      }

   }
   else if (data->getDataType() == dataTypeText)
   {
      // pen width index

   }
}
*/

void CCamCadDatabase::transmute(CDataList& dataList,CCamCadDatabase& sourceCamCadDatabase)
{
   for (POSITION pos = dataList.GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = dataList.GetNext(pos);
      CAttributes* attributes = data->getAttributeMap();

      if (attributes != NULL)
      {
         attributes->empty();
      }

      if (data->getDataType() == dataTypeInsert)
      {
         BlockStruct* subBlock = sourceCamCadDatabase.getBlock(data->getInsert()->getBlockNumber());
         BlockStruct* transmutedSubBlock = getBlock(subBlock->getName());

         if (transmutedSubBlock == NULL)
         {
            transmutedSubBlock = copyBlock(subBlock->getName(),subBlock,false);
            transmute(transmutedSubBlock->getDataList(),sourceCamCadDatabase);
         }

         data->getInsert()->setBlockNumber(transmutedSubBlock->getBlockNumber());

         while (transmutedSubBlock->isAperture() && transmutedSubBlock->getShape() == apertureComplex)
         {
            BlockStruct* apertureBlock = sourceCamCadDatabase.getBlock(subBlock->getComplexApertureSubBlockNumber());
            BlockStruct* transmutedApertureBlock = getBlock(apertureBlock->getName());

            if (transmutedApertureBlock == NULL)
            {
               transmutedApertureBlock = copyBlock(apertureBlock->getName(),apertureBlock,false);
               transmute(transmutedApertureBlock->getDataList(),sourceCamCadDatabase);
            }

            transmutedSubBlock->setComplexApertureSubBlockNumber(transmutedApertureBlock->getBlockNumber());
            transmutedSubBlock = transmutedApertureBlock;
         }
      }
      else if (data->getDataType() == dataTypePoly)
      {
         CPolyList* polyList = data->getPolyList();

         for (POSITION pos = polyList->GetHeadPosition();pos != NULL;)
         {
            CPoly* poly = polyList->GetNext(pos);

            int widthIndex = poly->getWidthIndex();
            double width = sourceCamCadDatabase.getCamCadDoc().getWidth(widthIndex);
            int transmutedWidthIndex = getDefinedWidthIndex(width);
            poly->setWidthIndex(transmutedWidthIndex);
         }

      }
      else if (data->getDataType() == dataTypeText)
      {
         // pen width index

      }
   }
}




