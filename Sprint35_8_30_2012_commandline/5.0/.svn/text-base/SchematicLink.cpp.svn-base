// $Header: /CAMCAD/5.0/SchematicLink.cpp 20    6/17/07 8:54p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// SchematicLink.cpp

#include "stdafx.h"
#include "CCEtoODB.h"
#include "MainFrm.h"
#include "Sch_Lib.h"
#include "SchematicLink.h"
#include "SchematicLinkCompareDialogs.h"
#include ".\schematiclink.h"
#include "RwUiLib.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/* Define Section *********************************************************/
#define SCH_NUM_PATCH_LENGTH     8  // The length of number to patch up to

CTime startProcessTime;
CTime endProcessTime;

CString trimTrailing(const CString name)
{
   CString retval = name;

   CString trailingSuffixList = "-,_,&";
   CStringArray params;
   CSupString trialingSuffix(trailingSuffixList);
   trialingSuffix.Parse(params, ",");

   // Remove characters following a sperator such as (-,_,&)
   for (int i=0; i<params.GetCount(); i++)
   {
      int index = retval.Find(params.GetAt(i));
      if (index > 0)
      {
         retval = retval.Left(index);
      }
   }

   // Now remove characters that is not number from the end of the name
   int index = retval.GetLength();
   while (index>0 && !((int)retval[index-1]>=48 && (int)retval[index-1]<=57))
      index--;

   // If there is no characters left in the name, then return the original name
   if (index>0)
   {
      retval = retval.Left(index);
   }
   else
   {
      retval = name;
   }

   return retval;
}

CString generateSortKey(CString name)
{
   CString itemText = name;
   CString sortKey = "";
   CString numberString = "";
   int numberCount = 0;

   // (1) Creat the sort key
   for (int i=0; i<itemText.GetLength(); i++)
   {
      if (int(itemText[i]) >= 48 && int(itemText[i]) <= 57)
      {
         // The character is a number
         numberCount++;
         numberString.AppendChar(itemText[i]);
      }
      else 
      {
         // The character is NOT a number

         if (numberCount > 0)
         {
            // If previous characters were number, then patch the number is Zero up to the number in SCH_NUM_PATCH_LENGTH
            while (numberCount < SCH_NUM_PATCH_LENGTH)
            {
               numberString.Insert(0, "0");
               numberCount++;
            }

            // Append the number to the sortKey
            sortKey.Append(numberString);
         }

         // Append the character to the sortKey
         sortKey.AppendChar(itemText[i]);
         numberString = "";
         numberCount = 0;
      }
   }

   // Need to check of the end of the string is number
   if (numberCount > 0)
   {
      // If previous characters were number, then patch the number is Zero up to the number in SCH_NUM_PATCH_LENGTH
      while (numberCount < SCH_NUM_PATCH_LENGTH)
      {
         numberString.Insert(0, "0");
         numberCount++;
      }

      // Append the number to the sortKey
      sortKey.Append(numberString);
   }

   if (sortKey.IsEmpty())
      sortKey = name;

   return sortKey;

	//// create sort name
	//CString temp, buf;
	//bool alpha = true, lastAlpha = true, newSection = true;

	//name.MakeUpper();
	//for (int i=0; i<name.GetLength(); i++)
	//{
	//	if (isalpha(name[i]))
	//		alpha = true;
	//	else if (isdigit(name[i]))
	//		alpha = false;
	//	else
	//	{
	//		//if (lastAlpha)
	//		//{
	//		//	// add string
	//		//	temp = (CString)"AAAAAAAAAAAA" + temp;
	//		//	temp.Delete(0, temp.GetLength() - 12);
	//		//	buf += temp;
	//		//}
	//		//else
	//		//{
	//		//	// add number
	//		//	temp = (CString)"000000000000" + temp;
	//		//	temp.Delete(0, temp.GetLength() - 12);
	//		//	buf += temp;
	//		//}
	//		//temp.Empty();

	//		//buf += name[i];
	//		//lastAlpha = alpha;

	//		//newSection = true;
	//		continue;
	//	}

 //     if (i == 0)
 //     {
 //        lastAlpha = alpha;
 //        temp = name[i];
 //        continue;
 //     }

	//	//if (newSection)
	//	//{
	//	//	lastAlpha = alpha;
	//	//	temp = name[i];

	//	//	newSection = false;
	//	//	continue;
	//	//}

	//	if (lastAlpha != alpha)
	//	{
	//		if (lastAlpha)
	//		{
	//			// add string
	//			temp = temp + (CString)"AAAAAAAAAAAA";
	//			temp.Delete(0, temp.GetLength() - 12);
	//			buf += temp;
	//		}
	//		else
	//		{
	//			// add number
	//			temp = (CString)"000000000000" + temp;
	//			temp.Delete(0, temp.GetLength() - 12);
	//			buf += temp;
	//		}

	//		temp.Empty();
	//	}
	//	temp += name[i];
	//	lastAlpha = alpha;
	//}

	//if (lastAlpha)
	//{
	//	// add string
	//	temp = temp + (CString)"AAAAAAAAAAAA";
	//	temp.Delete(0, temp.GetLength() - 12);
	//	buf += temp;
	//}
	//else
	//{
	//	// add number
	//	temp = (CString)"000000000000" + temp;
	//	temp.Delete(0, temp.GetLength() - 12);
	//	buf += temp;
	//}

	//return buf;
}

//static int sortName(const void* elem1,const void* elem2)
//{
//   CString sortKey1 = *(CString*)elem1;
//   CString sortKey2 = *(CString*)elem2;
//
//   sortKey1 = generateSortKey(sortKey1);
//   sortKey2 = generateSortKey(sortKey2);
//
//   return sortKey1.Compare(sortKey2);
//}

static int sortName(const void* elem1,const void* elem2)
{
   SchSortItem* sortItem1 = (SchSortItem*)(((SElement*)elem1)->pObject->m_object);
   SchSortItem* sortItem2 = (SchSortItem*)(((SElement*)elem2)->pObject->m_object);

   CString sortKey1, sortKey2;

   sortKey1 = generateSortKey(sortItem1->getName());
   sortKey2 = generateSortKey(sortItem2->getName());

   return sortKey1.Compare(sortKey2);
}

void copyNameMaps(const CMapStringToString& fromMap, CMapStringToString& toMap)
{
   for (POSITION pos=fromMap.GetStartPosition(); pos != NULL;)
   {
      CString key, value;
      fromMap.GetNextAssoc(pos, key, value);
      toMap.SetAt(key, value);
   }
}

//---------------------------------------------------------------------------------------
// ItemProperty
//---------------------------------------------------------------------------------------
ItemProperty::ItemProperty(const CString name)
{
   m_name = name;
   m_useCrossReferenceMatch = false;

   setCompareName(name);
}

ItemProperty::~ItemProperty()
{
}

//CString ItemProperty::generateSortKey(CString name)
//{
//	// create sort name
//	CString temp, buf;
//	bool alpha = true, lastAlpha = true, newSection = true;
//
//	name.MakeUpper();
//	for (int i=0; i<name.GetLength(); i++)
//	{
//		if (isalpha(name[i]))
//			alpha = true;
//		else if (isdigit(name[i]))
//			alpha = false;
//		else
//		{
//			if (lastAlpha)
//			{
//				// add string
//				temp = (CString)"AAAAAAAAAAAA" + temp;
//				temp.Delete(0, temp.GetLength() - 12);
//				buf += temp;
//			}
//			else
//			{
//				// add number
//				temp = (CString)"000000000000" + temp;
//				temp.Delete(0, temp.GetLength() - 12);
//				buf += temp;
//			}
//			temp.Empty();
//
//			buf += name[i];
//			lastAlpha = alpha;
//
//			newSection = true;
//			continue;
//		}
//
//		if (newSection)
//		{
//			lastAlpha = alpha;
//			temp = name[i];
//
//			newSection = false;
//			continue;
//		}
//
//		if (lastAlpha != alpha)
//		{
//			if (lastAlpha)
//			{
//				// add string
//				temp = (CString)"AAAAAAAAAAAA" + temp;
//				temp.Delete(0, temp.GetLength() - 12);
//				buf += temp;
//			}
//			else
//			{
//				// add number
//				temp = (CString)"000000000000" + temp;
//				temp.Delete(0, temp.GetLength() - 12);
//				buf += temp;
//			}
//
//			temp.Empty();
//		}
//		temp += name[i];
//		lastAlpha = alpha;
//	}
//
//	if (lastAlpha)
//	{
//		// add string
//		temp = (CString)"AAAAAAAAAAAA" + temp;
//		temp.Delete(0, temp.GetLength() - 12);
//		buf += temp;
//	}
//	else
//	{
//		// add number
//		temp = (CString)"000000000000" + temp;
//		temp.Delete(0, temp.GetLength() - 12);
//		buf += temp;
//	}
//
//	return buf;
//}

void ItemProperty::setCompareName(const CString compareName)
{
   m_compareName = compareName;
   m_sortKey = generateSortKey(compareName);
}

//---------------------------------------------------------------------------------------
// ItemData
//---------------------------------------------------------------------------------------
ItemData::ItemData(DataStruct* dataStruct, const int sheetBlockNumber)
{
   m_dataStruct = dataStruct;
   m_sheetBlockNumber = sheetBlockNumber;
}

ItemData::~ItemData()
{
   m_dataStruct = NULL;
}

//---------------------------------------------------------------------------------------
// ComponentItem
//---------------------------------------------------------------------------------------
ComponentItem::ComponentItem(const CString name, DataStruct* dataStruct, const int sheetBlockNumber)
   : ItemProperty(name)
   , ItemData(dataStruct, sheetBlockNumber)
   , m_compPinItemMap(nextPrime2n(20), false)
   , m_matchedComponetItemList(false)
{
}

ComponentItem::~ComponentItem()
{
   m_compPinItemMap.empty();
   m_matchedComponetItemList.empty();
}

void ComponentItem::clearMatchedComponentItem()
{
   m_matchedComponetItemList.empty();
}

void ComponentItem::addMatchedComponentItem(ComponentItem* matchedComponentItem)
{
   m_matchedComponetItemList.AddTail(matchedComponentItem);
}

POSITION ComponentItem::getMatchedComponentItemStartPosition()
{
   return m_matchedComponetItemList.GetHeadPosition();
}

ComponentItem* ComponentItem::getFirstMatchedComponentItem()
{
   return m_matchedComponetItemList.GetHead();
}

ComponentItem* ComponentItem::getNextMatchedComponentItem(POSITION& pos)
{
   ComponentItem* componentItem = NULL;
   if (pos != NULL)
   {
      componentItem = m_matchedComponetItemList.GetNext(pos);
   }
   return componentItem;
}

int ComponentItem::getMatchedCount()
{
   return m_matchedComponetItemList.GetCount();
}

void ComponentItem::addCompPinItem(CompPinItem* compPinItem)
{
   if (compPinItem != NULL && findCompPinItem(compPinItem->getComparePinName()) == NULL)
   {
      m_compPinItemMap.setAt(compPinItem->getComparePinName(), compPinItem);
   }
}

POSITION ComponentItem::getCompPinItemStartPosition()
{
   return m_compPinItemMap.GetStartPosition();
}

CompPinItem* ComponentItem::getNextCompPinItem(POSITION& pos)
{
   CString key;
   CompPinItem* compPinItem =  NULL;
   if (pos != NULL)
   {
      m_compPinItemMap.GetNextAssoc(pos, key, compPinItem);
   }

   return compPinItem;
}

CompPinItem* ComponentItem::findCompPinItem(const CString pinName)
{
   CompPinItem* compPinItem = NULL;
   m_compPinItemMap.Lookup(pinName, compPinItem);
   return compPinItem;
}

//---------------------------------------------------------------------------------------
// CompPinItem
//---------------------------------------------------------------------------------------
CompPinItem::CompPinItem(const CString compName, const CString pinName, ComponentItem* componentItem, NetItem* netItem)
   : m_compName(compName)
   , m_pinName(pinName)
   , m_compareCompName(compName)
   , m_comparePinName(pinName)
{
   m_componentItem = componentItem;
   m_netItem = netItem;
   m_matchedCompPinItem = NULL;
}

CompPinItem::~CompPinItem()
{
   m_componentItem = NULL;
   m_netItem = NULL;
   m_matchedCompPinItem = NULL;
}

void CompPinItem::setCompareName(const CString compareCompName, const CString comparePinName)
{
   m_compareCompName = compareCompName;
   m_comparePinName = comparePinName;
}

void CompPinItem::setMatchedCompPinItem(CompPinItem* matchedCompPinItem)
{
   m_matchedCompPinItem = matchedCompPinItem;
}

CompPinItem* CompPinItem::getMatchedCompPinItem()
{
   return m_matchedCompPinItem;
}

NetItem* CompPinItem::getNetItem()
{
   return m_netItem;
}

//---------------------------------------------------------------------------------------
// NetItem
//---------------------------------------------------------------------------------------
NetItem::NetItem(const CString name)
   : ItemProperty(name)
   , m_netDataList(true)
   , m_compPinItemList(false)
   , m_matchedNetItemList(nextPrime2n(20), false)
   , m_compPinMatchedCount(0)
{
   empty();
   m_netStruct = NULL;
}

NetItem::~NetItem()
{
   empty();
   m_netStruct = NULL;
}

void NetItem::empty()
{
   m_netDataList.empty();
   m_compPinItemList.empty();
   m_matchedNetItemList.empty();
}

void NetItem::addNetData(DataStruct* dataStruct, const int sheetBlockNumber)
{
   ItemData* itemData = new ItemData(dataStruct, sheetBlockNumber);
   m_netDataList.AddTail(itemData);
}

void  NetItem::setNetStruct(NetStruct* netStruct)
{
   m_netStruct = netStruct;
}

NetStruct*  NetItem::getNetStruct()
{
   return m_netStruct;
}

void NetItem::clearMatchedNetItem()
{
   m_matchedNetItemList.empty();
   m_compPinMatchedCount = 0;
}

void NetItem::addMatchedNetItem(NetItem* matchedNetItem)
{
   NetItem* netItem = NULL;
   if (!m_matchedNetItemList.Lookup(matchedNetItem->getName(), netItem))
   {
      m_matchedNetItemList.setAt(matchedNetItem->getName(), matchedNetItem);
   }
}

POSITION NetItem::getMatchNetItemStartPosition()
{
   return m_matchedNetItemList.GetStartPosition();
}

NetItem* NetItem::getNextMatchedNetItem(POSITION& pos)
{
   CString key;
   NetItem* netItem = NULL;
   if (pos != NULL)
   {
      m_matchedNetItemList.GetNextAssoc(pos, key, netItem);
   }

   return netItem;
}

NetItem* NetItem::getFirstMatachNetItem()
{
   NetItem* netItem = NULL;
   POSITION pos = m_matchedNetItemList.GetStartPosition();
   if (pos != NULL)
   {
      CString key;
      m_matchedNetItemList.GetNextAssoc(pos, key, netItem);
   }

   return netItem;
}

int NetItem::getMatchedCount()
{
   return m_matchedNetItemList.GetCount();
}

void NetItem::addCompPinItem(CompPinItem* comppinItem)
{
   m_compPinItemList.AddTail(comppinItem);
}

POSITION NetItem::getCompPinItemStartPosition()
{
   return m_compPinItemList.GetHeadPosition();
}

CompPinItem* NetItem::getNextCompPinItem(POSITION& pos)
{
   CompPinItem* compPinItem =  NULL;
   if (pos != NULL)
   {
      compPinItem = m_compPinItemList.GetNext(pos);
   }

   return compPinItem;
}

int NetItem::getCompPinItemCount()
{
   return m_compPinItemList.GetCount();
}

POSITION NetItem::getItemDataStartPosition()
{
   return m_netDataList.GetHeadPosition();
}

ItemData* NetItem::getNextItemData(POSITION& pos)
{
   ItemData* itemData = NULL;
   if (pos != NULL)
   {
      itemData = m_netDataList.GetNext(pos);
   }
   return itemData;
}

ItemData* NetItem::getFirstItemData()
{
   ItemData* itemData = NULL;
   if (m_netDataList.GetCount() != 0)
      itemData = m_netDataList.GetHead();

   return itemData;
}

int NetItem::getItemDataCount()
{
   return m_netDataList.GetCount();
}

//---------------------------------------------------------------------------------------
// SchematicLinkCollection
//---------------------------------------------------------------------------------------
SchematicLinkCollection::SchematicLinkCollection(bool isSchematic)
   : m_isSchematic(isSchematic)
   , m_componentCollection(true)
   , m_compPinCollection(true)
   , m_netCollection(true)
{
   empty();
}

SchematicLinkCollection::~SchematicLinkCollection()
{
   empty();
}

void SchematicLinkCollection::empty()
{
   m_compPinCollection.empty();
   m_netCollection.empty();
   m_componentCollection.empty();
}

 void SchematicLinkCollection::fillCollection(CCEtoODBDoc& camcadDoc, FileStruct& fileStruct)
{
   fileCollectionFromNetlist(camcadDoc, fileStruct);  

   // Assume it is always DESIGNATOR
   int compareNameKw = camcadDoc.RegisterKeyWord(SCH_ATT_DESIGNATOR, valueTypeString);
   int netNameKw = camcadDoc.getStandardAttributeKeywordIndex(standardAttributeNetName);

   fillComponentAndNetCollection(camcadDoc, fileStruct, netNameKw, compareNameKw);
}

void SchematicLinkCollection::fileCollectionFromNetlist(CCEtoODBDoc& camcadDoc, FileStruct& fileStruct)
{
   int compDesignatorKw = camcadDoc.RegisterKeyWord(SCH_ATT_COMPDESIGNATOR, valueTypeString);
   int pinDesignatorKw = camcadDoc.RegisterKeyWord(SCH_ATT_PINDESIGNATOR, valueTypeString);

   // Add comppin from CAMCAD netlist to NetCollection
   for (POSITION pos=fileStruct.getHeadNetPosition(); pos != NULL;)
   {
      NetStruct* netStruct = fileStruct.getNextNet(pos);
      if (netStruct != NULL) // && !netStruct->getNetName().IsEmpty()) // && netStruct->getNetName().CompareNoCase("~Unused_Pins~") != 0)
      {
         NetItem& netItem = addNetItem(netStruct->getNetName());
         netItem.setNetStruct(netStruct);

         for (POSITION compPinPos=netStruct->getHeadCompPinPosition(); compPinPos != NULL;)
         {
            CompPinStruct* comppin = netStruct->getNextCompPin(compPinPos);

            // SCH_PORT_COMPPIN is "$$" and used to identify Global or Master port.  It is not port inside a symbol so ignore it
            if (comppin != NULL && comppin->getPinName() != SCH_PORT_COMPPIN)
            {
               CString compName = comppin->getRefDes();
               CString pinName = comppin->getPinName();

               // Add PCB comp and comppin
               if (!m_isSchematic)
               {
                  // Add compName to map so that comp can be add later in addComponentItemWithData()
                  if (!compName.IsEmpty())
                  {
                     CString key = compName; key.MakeLower();
                     CString value;
                     if (!m_compNameMap.Lookup(key, value))
                        m_compNameMap.SetAt(key, compName);
                  }

                  // Add CompPin
                  CompPinItem* compPinItem = new CompPinItem(compName, pinName, NULL, &netItem);
                  compPinItem->setCompareName(compName, pinName);

                  netItem.addCompPinItem(compPinItem);
               }

               // Add SCH comp and comppin
               else
               {

                  CString compareCompName;
                  CString comparePinName;

                  // Get compDesignator and pinDesignator from attributes
                  Attrib* compDesignatorAttrib = NULL;
                  Attrib* pinDesignatorAttrib = NULL;
                  if (comppin->lookUpAttrib(compDesignatorKw, compDesignatorAttrib))
                  {
                     compareCompName = compDesignatorAttrib->getStringValue();
                  }
                  if (comppin->lookUpAttrib(pinDesignatorKw, pinDesignatorAttrib))
                  {
                     comparePinName = pinDesignatorAttrib->getStringValue();
                  }

                  // Add compName to map so that comp can be add later in addComponentItemWithData()
                  if (!compareCompName.IsEmpty())
                  {
                     CString key = compareCompName; key.MakeLower();
                     CString value;
                     if (!m_compNameMap.Lookup(key, value))
                        m_compNameMap.SetAt(key, compareCompName);

                     //Add compPin
                     if (!comparePinName.IsEmpty())
                     {
                        // Only add comppin that has proper compname and pinname, for Schematic, those are in compareRefname and comparePinName.
                        // Sometime in Schematic, the file has items improperly identified as component and is added to the net with no
                        // component designator or pin designator.

                        CompPinItem* compPinItem = new CompPinItem(compName, pinName, NULL, &netItem);
                        compPinItem->setCompareName(compareCompName, comparePinName);

                        netItem.addCompPinItem(compPinItem);
                     }
                  }
               }

               //CompPinItem* compPinItem = addCompPinItem(camcadDoc, *comppin, netItem, compDesignatorKw, pinDesignatorKw);
               //if (compPinItem != NULL) 
               //{
               //   CString compName = compPinItem->getCompareCompName();
               //   compName.MakeLower();

               //   CString value;
               //   if (!m_compNameMap.Lookup(compName, value))
               //      m_compNameMap.SetAt(compName, compPinItem->getCompareCompName()); //compPinItem->getCompName());
               //}
            }
         }
      }
   }
}

void SchematicLinkCollection::fillComponentAndNetCollection(CCEtoODBDoc& camcadDoc, FileStruct& fileStruct, const int netNameKw, const int componentCompareNameKw)
{
   if (m_isSchematic)
   {
      for(int i=0; i<camcadDoc.getMaxBlockIndex(); i++)
      {
         BlockStruct* block = camcadDoc.getBlockAt(i);
         if (block != NULL && block->getBlockType() == blockTypeSheet)
         {
            fillComponentAndNetCollection(camcadDoc, *block, netNameKw, componentCompareNameKw);
         }
      }
   }
   else
   {
      fillComponentAndNetCollection(camcadDoc, *fileStruct.getBlock(), netNameKw, componentCompareNameKw);
   }                                              
}

void SchematicLinkCollection::fillComponentAndNetCollection(CCEtoODBDoc& camcadDoc, BlockStruct& block, const int netNameKw, const int componentCompareNameKw)
{  
   for (CDataListIterator dataList(block); dataList.hasNext();)
   {
      DataStruct* data = dataList.getNext();

      if (data->getDataType() == dataTypeInsert)
      {
         addComponentItemWithData(camcadDoc, *data, block.getBlockNumber(), componentCompareNameKw);
      }
      else if (data->getDataType() == dataTypePoly)
      {
         addNetItemWithData(camcadDoc, *data, block.getBlockNumber(), netNameKw);
      }
   }
}

//void SchematicLinkCollection::fillNetCollectionWithNetList(CCEtoODBDoc& camcadDoc, FileStruct& fileStruct)
//{
//   int compDesignatorKw = camcadDoc.RegisterKeyWord(SCH_ATT_COMPDESIGNATOR, valueTypeString);
//   int pinDesignatorKw = camcadDoc.RegisterKeyWord(SCH_ATT_PINDESIGNATOR, valueTypeString);
//
//   // Add comppin from CAMCAD netlist to NetCollection
//   for (POSITION pos=fileStruct.getHeadNetPosition(); pos != NULL;)
//   {
//      NetStruct* netStruct = fileStruct.getNextNet(pos);
//      if (netStruct != NULL && !netStruct->getNetName().IsEmpty())
//      {
//         NetItem& netItem = addNetItem(netStruct->getNetName());
//         netItem.setNetStruct(netStruct);
//         
//         for (POSITION compPinPos=netStruct->getHeadCompPinPosition(); compPinPos != NULL;)
//         {
//            CompPinStruct* comppin = netStruct->getNextCompPin(compPinPos);
//            if (comppin != NULL)
//            {   
//               addCompPinItem(camcadDoc, *comppin, netItem, compDesignatorKw, pinDesignatorKw);
//            }
//         }
//      }
//   }
//}
//
void SchematicLinkCollection::addComponentItemWithData(CCEtoODBDoc& camcadDoc, DataStruct& data, const int sheetBlockNumber, const int compareNameKw)
{
   InsertStruct* insert = data.getInsert();
   if (data.getDataType() == dataTypeInsert && insert != NULL)
   {
      if ((m_isSchematic && insert->getInsertType() == insertTypeSymbol) || 
         (!m_isSchematic && insert->getInsertType() == insertTypePcbComponent) || 
         (!m_isSchematic && insert->getInsertType() == insertTypeDie))
      {
         // Get the name of the component use for comparision, this can be different than 
         // the Designator(for Schematic) or the Refname(for PCB)
         CString componentName = insert->getRefname();
         CString compareName = componentName;
         Attrib* attrib = NULL;
         if (data.lookUpAttrib(compareNameKw, attrib))
         {
            CString compareNameValue = attrib->getStringValue();
            if (!compareNameValue.IsEmpty())
            {
               compareName = compareNameValue;
            }
         }
         //compareName = trimTrailing(compareName);

         CString lowerCaseName = compareName;
         lowerCaseName.MakeLower();

         ComponentItem* componentItem = findComponentItem(lowerCaseName);
         if (componentItem == NULL)
         {
            CString value;
            if (m_compNameMap.Lookup(lowerCaseName, value))
            {
               // Only add component that is already in the m_compName, which is created from netlist
               componentItem = new ComponentItem(componentName, &data, sheetBlockNumber);
               componentItem->setCompareName(compareName);

               m_componentCollection.SetAt(lowerCaseName, componentItem);
            }
         }
      }
   }
}

void SchematicLinkCollection::addNetItemWithData(CCEtoODBDoc& camcadDoc, DataStruct& data, const int sheetBlockNumber, const int netNameKw)
{
   if ((m_isSchematic && data.getGraphicClass() == graphicClassSignal) || !m_isSchematic && data.getGraphicClass() == graphicClassEtch)
   {
      Attrib* attrib = NULL;
      if (data.lookUpAttrib(netNameKw, attrib))
      {
         CString netName = attrib->getStringValue();

         if (!netName.IsEmpty())
         {
            NetItem* netItem = findNetItem(netName);
            if (netItem != NULL)
               netItem->addNetData(&data, sheetBlockNumber);

            //NetItem& netItem = addNetItem(netName);
            //netItem.addNetData(&data, sheetBlockNumber);
         }
      }
   }
}

NetItem& SchematicLinkCollection::addNetItem(const CString netName)
{
   //CString lowerCaseNetName = netName;
   //lowerCaseNetName.MakeLower();

   NetItem* netItem = findNetItem(netName);
   if (netItem == NULL)
   {
      netItem = new NetItem(netName);
      m_netCollection.SetAt(netName, netItem);
   }

   return *netItem;
}

CompPinItem* SchematicLinkCollection::addCompPinItem(CCEtoODBDoc& camcadDoc, CompPinStruct& comppin, NetItem& netItem, const int compDesignatorKw, const int pinDesignatorKw)
{
   CString compRefname = comppin.getRefDes();
   CString pinName = comppin.getPinName();
   CString compareRefname = compRefname;
   CString comparePinName = pinName;
   CompPinItem* compPinItem = NULL;

   if (m_isSchematic)
   {
      // Get compDesignator and pinDesignator from attributes
      Attrib* compDesignatorAttrib = NULL;
      Attrib* pinDesignatorAttrib = NULL;
      if (comppin.lookUpAttrib(compDesignatorKw, compDesignatorAttrib))
      {
         compareRefname = compDesignatorAttrib->getStringValue();
      }
      if (comppin.lookUpAttrib(pinDesignatorKw, pinDesignatorAttrib))
      {
         comparePinName = pinDesignatorAttrib->getStringValue();
      }
   }

   if (!compareRefname.IsEmpty() && !comparePinName.IsEmpty())
   {
      // Only add comppin that has proper compname and pinname, for Schematic, those are in compareRefname and comparePinName.
      // Sometime in Schematic, the file has items improperly identified as component and is added to the net with no
      // component designator or pin designator.

      compPinItem = new CompPinItem(compRefname, pinName, NULL, &netItem);
      compPinItem->setCompareName(compareRefname, comparePinName);

      CString compareCompPinName = compareRefname + "." + comparePinName;
      compareCompPinName.MakeLower();
      //m_compPinCollection.setAt(compareCompPinName, compPinItem);
      netItem.addCompPinItem(compPinItem);
   }

   return compPinItem;
}

void SchematicLinkCollection::sort()
{
   //m_componentCollection.setSortFunction(SchematicLinkCollection::sortComponentItemByFilteredName);
   //m_componentCollection.Sort();

   //m_netCollection.setSortFunction(SchematicLinkCollection::sortNetItemByFilteredName);
   //m_netCollection.Sort();
}

int SchematicLinkCollection::sortComponentItemByFilteredName(const void* elem1,const void* elem2)
{
   return sortByFilteredName(true, elem1, elem2);
}

int SchematicLinkCollection::sortNetItemByFilteredName(const void* elem1,const void* elem2)
{
   return sortByFilteredName(false, elem1, elem2);
}

int SchematicLinkCollection::sortByFilteredName(const bool isComponentSort, const void* elem1,const void* elem2)
{
   CString sortKey1, sortKey2;

   sortKey1 = ((ItemProperty*)elem1)->getSortKey();
   sortKey2 = ((ItemProperty*)elem2)->getSortKey();

   //sortKey1 =  ((ItemProperty*&)elem1)->getSortKey();
   //sortKey2 =  ((ItemProperty*)elem2)->getSortKey();


   //if (isComponentSort)
   //{
   //   sortKey1 =  ((const ComponentItem*&)elem1)->getSortKey();
   //   sortKey2 =  ((const ComponentItem*&)elem2)->getSortKey();
   //}
   //else
   //{
   //   sortKey1 =  (*(const NetItem**)elem1)->getSortKey();
   //   sortKey2 =  (*(const NetItem**)elem2)->getSortKey();
   //}

   return sortKey1.Compare(sortKey2);
}

void SchematicLinkCollection::clearComponenitItemMatch()
{
   for (POSITION pos=getComponentStartPosition(); pos != NULL;)
   {
      ComponentItem* componentItem = getNextComponent(pos);
      if (componentItem != NULL)
         componentItem->clearMatchedComponentItem();
   }
}

POSITION SchematicLinkCollection::getComponentStartPosition()
{ 
   return m_componentCollection.GetStartPosition(); 
}

ComponentItem* SchematicLinkCollection::getNextComponent(POSITION& pos) 
{  
   CString key;
   ComponentItem* componentItem = NULL;
   if (pos != NULL)
   {
      m_componentCollection.GetNextAssoc(pos, key, componentItem);
   }
   
   return componentItem;
}

ComponentItem* SchematicLinkCollection::findComponentItem(const CString refdes)
{
   ComponentItem* componentItem = NULL;
   CString lowerCaseRefdes = refdes;
   lowerCaseRefdes.MakeLower();
   m_componentCollection.Lookup(lowerCaseRefdes, componentItem);
   return componentItem;
}

void SchematicLinkCollection::clearNetItemMatch()
{
   for (POSITION pos=getNetItemStartPosition(); pos != NULL;)
   {
      NetItem* netItem = getNextNetItem(pos);
      if (netItem != NULL)
         netItem->clearMatchedNetItem();
   }
}

POSITION SchematicLinkCollection::getNetItemStartPosition()
{
   return m_netCollection.GetStartPosition(); 
}

NetItem* SchematicLinkCollection::getNextNetItem(POSITION& pos)
{
   CString key;
   NetItem* netItem = NULL;
   if (pos !=  NULL)
   {
      m_netCollection.GetNextAssoc(pos, key, netItem);
   }
   
   return netItem;
}

NetItem* SchematicLinkCollection::findNetItem(const CString netName)
{
   //CString lowerCaseNetName = netName;
   //lowerCaseNetName.MakeLower();

   NetItem* netItem = NULL;
   m_netCollection.Lookup(netName, netItem);
   return netItem;
}

NetCollection& SchematicLinkCollection::getNetCollection()
{
   return m_netCollection;
}

POSITION SchematicLinkCollection::getCompPinItemStartPosition()
{
   return m_compPinCollection.GetStartPosition();
}

CompPinItem* SchematicLinkCollection::getNextCompPinItem(POSITION& pos)
{
   CString key;
   CompPinItem* compPinItem = NULL;
   if (pos!= NULL)
   {
      m_compPinCollection.GetNextAssoc(pos, key, compPinItem);
   }

   return compPinItem;
}

CompPinItem* SchematicLinkCollection::findCompCompPinItemByCompareName(const CString compareCompPinName)
{
   CString lowerCaseComparePinName = compareCompPinName;
   lowerCaseComparePinName.MakeLower();

   CompPinItem* compPinItem = NULL;
   m_compPinCollection.Lookup(lowerCaseComparePinName, compPinItem);
   return compPinItem;
}

int SchematicLinkCollection::getCompPinItemCount()
{
   return m_compPinCollection.GetCount();
}

//---------------------------------------------------------------------------------------
// SchematicLink
//---------------------------------------------------------------------------------------
SchematicLink::SchematicLink(CCEtoODBDoc& schematicDoc, CCEtoODBDoc& pcbDoc, FileStruct& schematicFileStruct, FileStruct& pcbFileStruct)
   : m_schDoc(schematicDoc)
   , m_pcbDoc(pcbDoc)
   , m_schFileStruct(schematicFileStruct)
   , m_pcbFileStruct(pcbFileStruct)
   , m_schCollection(true)
   , m_pcbCollection(false)
{
   //emptyCollections();
   m_isDataLoaded = false;
   m_lastSchColorOverrideDataStruct = NULL;
   m_lastPcbColorOverrideDataStruct = NULL;

   m_netNameKw = -1;
   m_refdesMapKw = -1;
   m_designatorKw = -1;
   m_compDesignatorKw = -1;
   m_pinDesignatorKw = -1;
}

SchematicLink::~SchematicLink()
{
   emptyCollections();
   m_lastSchColorOverrideDataStruct = NULL;
   m_lastPcbColorOverrideDataStruct = NULL;
}

void SchematicLink::emptyCollections()
{
   m_schCollection.empty();
   m_pcbCollection.empty();
}

bool SchematicLink::loadData()
{
   if (!m_isDataLoaded)
   {
      emptyCollections();

      m_netNameKw = m_schDoc.getStandardAttributeKeywordIndex(standardAttributeNetName);
      m_refdesMapKw = m_schDoc.RegisterKeyWord(SCH_ATT_REFDES_MAP, valueTypeString);
      m_designatorKw = m_schDoc.RegisterKeyWord(SCH_ATT_DESIGNATOR, valueTypeString);

      m_compDesignatorKw = m_schDoc.RegisterKeyWord(SCH_ATT_COMPDESIGNATOR, valueTypeString);
      m_pinDesignatorKw = m_schDoc.RegisterKeyWord(SCH_ATT_PINDESIGNATOR, valueTypeString);

      Attrib* blockAttrib = NULL;
      BlockStruct* block = m_schFileStruct.getBlock();
      if (block != NULL && block->lookUpAttrib(m_refdesMapKw, blockAttrib))
      {
         CString refdesMapValue = blockAttrib->getStringValue();
         if (!refdesMapValue.IsEmpty())
         {
            m_designatorKw = m_schDoc.RegisterKeyWord(refdesMapValue, valueTypeString);
         }
      }

      m_schCollection.fillCollection(m_schDoc, m_schFileStruct);
      m_pcbCollection.fillCollection(m_pcbDoc, m_pcbFileStruct);
      m_isDataLoaded = true;
   }
 
   return m_isDataLoaded;
}

bool SchematicLink::processAutoCrosslink()
{
   startProcessTime = startProcessTime.GetCurrentTime();

   if (!m_isDataLoaded)
      return false;

   CMapStringToString pcbCompMatches;
   CMapStringToString schCompMatches;
   
   // Match comps
   for (POSITION schCompPos=m_schCollection.getCompNameMap().GetStartPosition(); schCompPos!=NULL;)
   {
      CString schKey, schCompName, pcbCompName;
      m_schCollection.getCompNameMap().GetNextAssoc(schCompPos, schKey, schCompName);

      // First check for exact comp name match
      if(m_pcbCollection.getCompNameMap().Lookup(schKey, pcbCompName))
      {  
         pcbCompMatches.SetAt(schKey, schCompName);
         schCompMatches.SetAt(schKey, pcbCompName);

         ComponentItem* pcbCompItem = m_pcbCollection.findComponentItem(schKey);
         ComponentItem* schCompItem = m_schCollection.findComponentItem(schKey);
         if (pcbCompItem != NULL && schCompItem != NULL)
         {
            pcbCompItem->addMatchedComponentItem(schCompItem);
            schCompItem->addMatchedComponentItem(pcbCompItem);
         }
      }
      // Since there is no match, now trim trailing alpha and suffix and try to match again
      else
      {
         CString trimedSchKey = trimTrailing(schKey);
         if(m_pcbCollection.getCompNameMap().Lookup(trimedSchKey, pcbCompName))
         {  
            pcbCompMatches.SetAt(trimedSchKey, schCompName);
            schCompMatches.SetAt(schKey, pcbCompName);

            ComponentItem* pcbCompItem = m_pcbCollection.findComponentItem(trimedSchKey);
            ComponentItem* schCompItem = m_schCollection.findComponentItem(schKey);
            if (pcbCompItem != NULL && schCompItem != NULL)
            {
               pcbCompItem->addMatchedComponentItem(schCompItem);
               schCompItem->addMatchedComponentItem(pcbCompItem);
            }
         }
      }
   }

   // Collect all comppins from nets
   CompPinCollection schNodes(false);
   int schConsideredNetCount = 0;
   for (POSITION netPos=m_schCollection.getNetItemStartPosition(); netPos!=NULL;)
   {
      NetItem* schNetItem = m_schCollection.getNextNetItem(netPos);
      if (schNetItem != NULL)
      {
         // For every net, collection comppins where the compNames are already matched
         bool considerNet = false;
         for (POSITION compPinPos=schNetItem->getCompPinItemStartPosition(); compPinPos!=NULL;)
         {            
            CompPinItem* schCompPinItem = schNetItem->getNextCompPinItem(compPinPos);
            if (schCompPinItem != NULL)
            {
               CString schCompName = schCompPinItem->getCompareCompName();
               CString schPinName = schCompPinItem->getComparePinName();
               schCompName.MakeLower();

               CString matchSchCompName;
               if (schCompMatches.Lookup(schCompName, matchSchCompName))
               {                  
                  CString compPinName = matchSchCompName + "." + schPinName;
                  compPinName.MakeLower();
                  CompPinItem* temp = NULL;
                  if (!schNodes.Lookup(compPinName, temp))
                     schNodes.setAt(compPinName, schCompPinItem);
                  considerNet = true;
               }
            }
         }

         if (considerNet)
            schConsideredNetCount++;
      }
   }

   // Match Net using PCB as master
   CTypedMapStringToPtrContainer<CStringArray*> partialMatchedPcbNets(true);
   CStringArray unmatchedPcbNets;
   CMapStringToString schNetMatches;
   CStringArray matchedSchNodes;
   int emptyNets = 0;
   for (POSITION netPos=m_pcbCollection.getNetItemStartPosition(); netPos!=NULL;)
   {
      NetItem* pcbNetItem = m_pcbCollection.getNextNetItem(netPos);
      if (pcbNetItem != NULL)
      {
         int node = 0;
         int matches = 0;
         bool needToDeleteUnmatchCompPins = true;
         CMapStringToString matchedSchNets;
         CStringArray matchedCompPins;
         CStringArray* unmatchedCompPins = new CStringArray();

         // For every comppin in PCB net, find a match comppin in SCH net
         for (POSITION compPinPos=pcbNetItem->getCompPinItemStartPosition(); compPinPos!=NULL;)
         {
            CompPinItem* pcbCompPinItem = pcbNetItem->getNextCompPinItem(compPinPos);
            if (pcbCompPinItem != NULL)
            {
               CString key = pcbCompPinItem->getCompareCompName();
               CString compName ;
               key.MakeLower();
               if (pcbCompMatches.Lookup(key, compName))
               {
                  node++;

                  CString compareCompPinName = pcbCompPinItem->getCompareCompPinName();
                  compareCompPinName.MakeLower();
                  CompPinItem* schCompPinItem = NULL;

                  // Comppin has a match
                  if (schNodes.Lookup(compareCompPinName, schCompPinItem) && schCompPinItem != NULL)
                  {
                     matches++;

                     schNodes.RemoveKey(compareCompPinName);
                     matchedCompPins.Add(compareCompPinName);

                     // Set the matched comppins of PCB and SCH to each other
                     pcbCompPinItem->setMatchedCompPinItem(schCompPinItem);
                     schCompPinItem->setMatchedCompPinItem(pcbCompPinItem);

                     CString schNetName;
                     if (schCompPinItem->getNetItem() != NULL && !matchedSchNets.Lookup(schCompPinItem->getNetItem()->getName(), schNetName))
                     {
                        matchedSchNets.SetAt(schCompPinItem->getNetItem()->getName(), schCompPinItem->getNetItem()->getName());

                        // Add the net names of PCB and SCH to each other so they each know
                        // what nets they are matched to
                        pcbNetItem->addMatchedNetItem(schCompPinItem->getNetItem());
                        schCompPinItem->getNetItem()->addMatchedNetItem(pcbNetItem);
                     }
                  }
                  // Comppin has no match
                  else
                  {
                     unmatchedCompPins->Add(pcbCompPinItem->getCompareCompPinName());
                  }
               }
            }
         }

         if (node == 0)
         {
            emptyNets++;
            continue;
         }

         for (POSITION pos=matchedSchNets.GetStartPosition(); pos != NULL;)
         {
            CString key, value;
            matchedSchNets.GetNextAssoc(pos, key, value);
            schNetMatches.SetAt(key, pcbNetItem->getName());
         }

         // Partial Match
         if (matches < node)
         {
			   // Increment 'matches' for other nodes of matched Nets that have not been matched
            for (POSITION netPos=matchedSchNets.GetStartPosition(); netPos!=NULL;)
            {
               CString key, schNetName;              
               matchedSchNets.GetNextAssoc(netPos, key, schNetName);
               NetItem* schNetItem = m_schCollection.findNetItem(schNetName);
               if (schNetItem != NULL)
               {
                  for (POSITION compPinPos=schNetItem->getCompPinItemStartPosition(); compPinPos!=NULL;)
                  {
                     CompPinItem* schCompPinItem = schNetItem->getNextCompPinItem(compPinPos);
                     if (schCompPinItem != NULL && schCompPinItem->getMatchedCompPinItem() == NULL)
                     {
                        CString schCompName = schCompPinItem->getCompareCompName();
                        CString schPinName = schCompPinItem->getComparePinName();
                        schCompName.MakeLower();

                        // Find the matched SCH compName
                        CString matchSchCompName;
                        if (schCompMatches.Lookup(schCompName, matchSchCompName))
                        {
                           // If comppin is in schNodes, then consider that comppin to be a partial match
                           CString compPinName = matchSchCompName + "." + schPinName;
                           compPinName.MakeLower();
                           CompPinItem* temp = NULL;
                           if (schNodes.Lookup(compPinName, temp))
                           {
                              matches++;

                              matchedSchNodes.Add(compPinName);
                              matchedCompPins.Add(compPinName);
                           }
                        }
                     }
                  }
               }
            }
         
			   //	Decrement 'nodes' if Component already has CompPins matched
            for (int i=0; i<unmatchedCompPins->GetCount(); i++)
            {
               CString unmatchedCompPinName = unmatchedCompPins->GetAt(i);
               CString unmatchedCompName = unmatchedCompPinName.Left(unmatchedCompPinName.Find("."));

               for (int k=0; k<matchedCompPins.GetCount(); k++)
               {
                  CString matchedCompPinName = matchedCompPins.GetAt(k);
                  CString matchedCompName = matchedCompPinName.Left(matchedCompPinName.Find("."));                  

                  if (unmatchedCompName.CompareNoCase(matchedCompName)== 0)
                  {
                     node--;
                     break;
                  }
               }
            }
         }

         if (node>0)
         {
            if (matches == 0)
            {
               unmatchedPcbNets.Add(pcbNetItem->getName());
            }
            else if (matches < node)
            {
               partialMatchedPcbNets.setAt(pcbNetItem->getName(), unmatchedCompPins);
               needToDeleteUnmatchCompPins = false;
            }
         }

         // unmatchedCompPins a point to an array that is creatd to hold partial unmatch comppins for
         // for PCB net and is saved in partialMatchedPcbNets map.  However, if there is no node in PCB net,
         // no match for PCB net, or no partial match, then unmatchedCompPins need to be delete and free.
         if (needToDeleteUnmatchCompPins)
         {
            delete unmatchedCompPins;
            unmatchedCompPins = NULL;
         }
         matchedCompPins.RemoveAll();
      }
   }

   // Clear schNodes that were inferred as matched
   for (int i= 0; i<matchedSchNodes.GetCount(); i++)
   {
      CompPinItem* temp = NULL;
      if (schNodes.Lookup(matchedSchNodes[i], temp))
         schNodes.RemoveKey(matchedSchNodes[i]);
   }

   endProcessTime = CTime::GetCurrentTime();
   CTimeSpan timeSpan = endProcessTime - startProcessTime;   

   // Log
   CFormatStdioFile file;
   CFileException err;
   CString logFileName = GetLogfilePath("AutoCrosslink.log");

   int pcbCompMatchedCount = 0, pcbCompTotalCount = m_pcbCollection.getCompNameMap().GetCount();
   int schCompMatchedCount = 0, schCompTotalCount = m_schCollection.getCompNameMap().GetCount();
   int pcbNetMatchedCount = 0, pcbNetTotalCount = m_pcbFileStruct.getNetCount();
   int schNetMatchedCount = 0, schNetTotalCount = m_schFileStruct.getNetCount();
   if (file.Open(logFileName, CFile::modeCreate|CFile::modeWrite, &err))
   {
      // Write header
      file.WriteString("AutoCrosslinking\n");
      file.WriteString("PCB File: \'%s\'\n", m_pcbDoc.docTitle); // m_pcbFileStruct.getName());
      file.WriteString("sch File: \'%s\'\n", m_schDoc.docTitle); // m_schFileStruct.getName());
      file.WriteString("Time Elapsed: %d\n\n", timeSpan.GetTotalSeconds());

      // Comp Matching
      file.WriteString("COMP MATCHING\n");
      pcbCompMatchedCount = pcbCompMatches.GetCount();
      double pcbCompsPercent = ((double)pcbCompMatchedCount / (double)pcbCompTotalCount) * 100;
      file.WriteString("%d of %d [%0.f%%] PCB Comps Matched\n", pcbCompMatchedCount, pcbCompTotalCount, floor(pcbCompsPercent));

      schCompMatchedCount = schCompMatches.GetCount();
      double schCompsPercent = ((double)schCompMatchedCount / (double)schCompTotalCount) * 100;
      file.WriteString("%d of %d [%0.f%%] Sch Comps Matched\n\n",schCompMatchedCount, schCompTotalCount, floor(schCompsPercent));

      // Net Matching
      file.WriteString("NET MATCHING\n");
      int pcbConsideredNetCount = pcbNetTotalCount - emptyNets;
      pcbNetMatchedCount = pcbConsideredNetCount - unmatchedPcbNets.GetCount();
      double pcbConsideredNetPercent = ((double)pcbConsideredNetCount / (double) pcbNetTotalCount) * 100;
      double pcbMatchedNetPercent = ((double)pcbNetMatchedCount / (double)pcbNetTotalCount) * 100;
      file.WriteString("%d of %d [%0.f%%] PCB Nets Considered\n", pcbConsideredNetCount, pcbNetTotalCount, floor(pcbConsideredNetPercent));
      file.WriteString("%d of %d [%0.f%%] PCB Nets Matched\n\n", pcbNetMatchedCount, pcbNetTotalCount, floor(pcbMatchedNetPercent));

      CMapStringToString unmatchSchNets;
      for (POSITION pos=schNodes.GetStartPosition(); pos != NULL;)
      {
         CString key;
         CompPinItem* compPinItem = NULL;
         schNodes.GetNextAssoc(pos, key, compPinItem);
         if (compPinItem != NULL && compPinItem->getNetItem() != NULL)
         {
            CString netName = compPinItem->getNetItem()->getName();
            CString temp;
            if (!unmatchSchNets.Lookup(compPinItem->getNetItem()->getName(), temp))
               unmatchSchNets.SetAt(netName, netName);
         }
      }

      // Use the above FOR loop to find unmatchSchNet and then calculate the schNetMatchedCount just like visECAD does
      schNetMatchedCount = schConsideredNetCount - unmatchSchNets.GetCount();
/*      for (POSITION pos=m_schCollection.getNetItemStartPosition(); pos!=NULL;)
      {
         NetItem* netItem = m_schCollection.getNextNetItem(pos);
         if (netItem != NULL && netItem->getMatchedCount() > 0)
            schNetMatchedCount++;
      }
 */    
      double schConsideredNetPercent = ((double)schConsideredNetCount / (double)schNetTotalCount) * 100;
      double schMatchedNetPercent = ((double)schNetMatchedCount / (double)schNetTotalCount) * 100;      
      file.WriteString("%d of %d [%0.f%%] Sch Nets Considered\n", schConsideredNetCount, schNetTotalCount, floor(schConsideredNetPercent));
      file.WriteString("%d of %d [%0.f%%] Sch Nets Matched\n\n", schNetMatchedCount, schNetTotalCount, floor(schMatchedNetPercent));

      // PCB Comps
      int unmatchedPcbCompCount = m_pcbCollection.getCompNameMap().GetCount() - pcbCompMatches.GetCount();
      double unmatchedPcbCompsPercent = ((double)unmatchedPcbCompCount / (double)m_pcbCollection.getCompNameMap().GetCount()) * 100;
      file.WriteString("\n-------------------------------------------\n");
      file.WriteString("Unmatched Pcb Comps = %d of %d [%.2f%%]\n", unmatchedPcbCompCount, m_pcbCollection.getCompNameMap().GetCount(), unmatchedPcbCompsPercent);
      CTypedMapSortStringToObContainer<SchSortItem> pcbUnmatchedCompNames;    
      pcbUnmatchedCompNames.setSortFunction(sortName);
      for (POSITION pos=m_pcbCollection.getCompNameMap().GetStartPosition(); pos!=NULL;)
      {
         CString key, compName;
         m_pcbCollection.getCompNameMap().GetNextAssoc(pos, key, compName);         
         if (!pcbCompMatches.Lookup(key, compName))
         {
            SchSortItem* sortItem = new SchSortItem(key, compName);
            pcbUnmatchedCompNames.SetAt(key, sortItem);
         }
      }
      pcbUnmatchedCompNames.Sort();
      CString* key = NULL; 
      SchSortItem* sortItem;
      pcbUnmatchedCompNames.GetFirstSorted(key, sortItem);
      while (sortItem != NULL)
      {
         file.WriteString("\t%s\n", sortItem->getItem());
 
         delete sortItem;
         sortItem = NULL;
 
         pcbUnmatchedCompNames.GetNextSorted(key, sortItem);
      }
      pcbUnmatchedCompNames.RemoveAll();

      // Sch Comps
      int unmatchedSchCompCount = m_schCollection.getCompNameMap().GetCount() - schCompMatches.GetCount();
      double unmatchedSchCompsPercent = ((double)unmatchedSchCompCount / (double)m_schCollection.getCompNameMap().GetCount()) * 100;
      file.WriteString("\n-------------------------------------------\n");
      file.WriteString("Unmatched Sch Comps = %d of %d [%.2f%%]\n", unmatchedSchCompCount, m_schCollection.getCompNameMap().GetCount(), unmatchedSchCompsPercent);
      CTypedMapSortStringToObContainer<SchSortItem> schUnmatchedCompNames;      
      schUnmatchedCompNames.setSortFunction(sortName);
      for (POSITION pos=m_schCollection.getCompNameMap().GetStartPosition(); pos!=NULL;)
      {
         CString key, compName;
         m_schCollection.getCompNameMap().GetNextAssoc(pos, key, compName);         
         if (!schCompMatches.Lookup(key, compName))
         {
            SchSortItem* sortItem = new SchSortItem(key, compName);
            schUnmatchedCompNames.SetAt(key, sortItem);
         }
      }
      schUnmatchedCompNames.Sort();
      schUnmatchedCompNames.GetFirstSorted(key, sortItem);
      while (sortItem != NULL)
      {
         file.WriteString("\t%s\n", sortItem->getItem());
 
         delete sortItem;
         sortItem = NULL;
 
         schUnmatchedCompNames.GetNextSorted(key, sortItem);
      }
      schUnmatchedCompNames.RemoveAll();

      // Matched Nets
      file.WriteString("\n-------------------------------------------\n");
      file.WriteString("Matched Nets = %d\n", schNetMatches.GetCount());
      CTypedMapSortStringToObContainer<SchSortItem> matchedNetPairs;      
      matchedNetPairs.setSortFunction(sortName);
      for (POSITION pos=schNetMatches.GetStartPosition(); pos!=NULL;)
      {
         CString key, value;
         schNetMatches.GetNextAssoc(pos, key, value);

         CString netPairs = value + "\t<-\t" + key;
         SchSortItem* sortItem = new SchSortItem(netPairs, netPairs);
         matchedNetPairs.SetAt(netPairs, sortItem);
      }
      //for (POSITION pos=m_schCollection.getNetItemStartPosition(); pos!=NULL;)
      //{
      //   NetItem* schNetItem = m_schCollection.getNextNetItem(pos);
      //   if (schNetItem != NULL && schNetItem->getFirstMatachNetItem() != NULL)
      //   {           
      //      CString netPairs = schNetItem->getFirstMatachNetItem()->getName() + "\t<-\t" + schNetItem->getName();
      //      SchSortItem* sortItem = new SchSortItem(netPairs, netPairs);
      //      matchedNetPairs.SetAt(netPairs, sortItem);
      //   }
      //}
      matchedNetPairs.Sort();
      matchedNetPairs.GetFirstSorted(key, sortItem);
      while (sortItem != NULL)
      {
         file.WriteString("\t%s\n", sortItem->getItem());
 
         delete sortItem;
         sortItem = NULL;
 
         matchedNetPairs.GetNextSorted(key, sortItem);
      }
      matchedNetPairs.RemoveAll();

      // Partial PCB Nets
      file.WriteString("\n-------------------------------------------\n");
      file.WriteString("Partial PCB Nets = %d  ( Format = Netname [unmatched pins] )\n", partialMatchedPcbNets.GetCount());
      CTypedMapSortStringToObContainer<SchSortItem> partialPcbNets;      
      partialPcbNets.setSortFunction(sortName);
      for (POSITION pos=partialMatchedPcbNets.GetStartPosition(); pos!=NULL;)
      {
         CString key;
         CStringArray* compPinArray;
         partialMatchedPcbNets.GetNextAssoc(pos, key, compPinArray);

         SchSortItem* sortItem = new SchSortItem(key, key);
         partialPcbNets.SetAt(key, sortItem);
      }
      partialPcbNets.Sort();
      partialPcbNets.GetFirstSorted(key, sortItem);
      while (sortItem != NULL)
      {
         CStringArray* compPinArray;
         if (partialMatchedPcbNets.Lookup(*key, compPinArray) && compPinArray != NULL)
         {
            CString compPins;
            for (int i=0; i<compPinArray->GetCount(); i++)
               compPins.AppendFormat(" %s", compPinArray->GetAt(i));

            file.WriteString("\t%s\t[%s ]\n", *key, compPins);
         }
 
         delete sortItem;
         sortItem = NULL;
 
         partialPcbNets.GetNextSorted(key, sortItem);
      }
      partialPcbNets.RemoveAll();
      //for (POSITION pos=partialMatchedPcbNets.GetStartPosition(); pos!=NULL;)
      //{
      //   CString key;
      //   CStringArray* compPinArray;
      //   partialMatchedPcbNets.GetNextAssoc(pos, key, compPinArray);

      //   CString compPins;
      //   if (compPinArray != NULL)
      //   {
      //      for (int i=0; i<compPinArray->GetCount(); i++)
      //         compPins.AppendFormat(" %s", compPinArray->GetAt(i));
      //   }

      //   file.WriteString("\t%s\t[%s ]\n", key, compPins);
      //}

      // Unmatched Sch Nodes
      file.WriteString("\n-------------------------------------------\n");
      file.WriteString("Unmatched Sch Nodes = %d\n", schNodes.GetCount());
      for (POSITION pos = schNodes.GetStartPosition(); pos != NULL;)
      {
         CString key;
         CompPinItem* schCompPinItem = NULL;
         schNodes.GetNextAssoc(pos, key, schCompPinItem);
         if (schCompPinItem != NULL && schCompPinItem->getMatchedCompPinItem() == NULL)
         {
            file.WriteString("\t%s", schCompPinItem->getCompareCompPinName());
            if (schCompPinItem->getNetItem() != NULL)
               file.WriteString(" of %s", schCompPinItem->getNetItem()->getName());
            file.WriteString("\n");
         }
      }

      file.Close();
   }   

   AutoCrosslinkResultDlg resultDlg(pcbCompMatchedCount, pcbCompTotalCount, schCompMatchedCount, schCompTotalCount,
                                    pcbNetMatchedCount, pcbNetTotalCount, schNetMatchedCount, schNetTotalCount, logFileName);
   if (resultDlg.DoModal() == IDOK)
      return true;
   else
      return false;
}

void SchematicLink::processComponentCrossReference(CMapStringToString& schematicToPcbMap)
{
/*  The function adhere by the following rules:
		1) The key and value of "schematicToPcbMap" is the "ComponentItem->m_compareName" of
			Schematic and Pcb respectively. 
		2) The key in the map of m_schCollection and m_pcbCollection are "ComponentItem->m_name"
		3) In the ComponentItem of m_pcbCollection, the "ComponentItem->m_compareName" and 
			"ComponentItem->m_name" is the same.  Therefore, we can do the lookup for m_pcbCollection
			using the value of "schematicToPcbMap"
*/
   if (!m_isDataLoaded)
      return;

	for (POSITION pos=m_schCollection.getComponentStartPosition(); pos != NULL;)
	{
		ComponentItem* schComponentItem = m_schCollection.getNextComponent(pos);
		if (schComponentItem != NULL)
		{
			CString pcbRefname;
			if (schematicToPcbMap.Lookup(schComponentItem->getCompareName(), pcbRefname))
			{
				ComponentItem* pcbComponentItem = m_pcbCollection.findComponentItem(pcbRefname);
				if (pcbComponentItem != NULL)
				{
					schComponentItem->addMatchedComponentItem(pcbComponentItem);
					pcbComponentItem->addMatchedComponentItem(schComponentItem);
				}
			}
		}
	}
}

void SchematicLink::processNetCrossReference(CMapStringToString& schematicToPcbMap)
{
}

void SchematicLink::clearComponentItemMatch()
{
   m_schCollection.clearComponenitItemMatch();
   m_pcbCollection.clearComponenitItemMatch();
}

void SchematicLink::processComponentItemMatch(const bool caseSensitive, const KillTrailingType killTrailingType, const CString trailingSuffix, CMapStringToString& nonMatchSchematicMap, CMapStringToString& nonMatchPcbMap, CMapStringToString& matchSchematicToPcbMap)
{
   if (!m_isDataLoaded)
      return;

   for (POSITION pos = m_schCollection.getComponentStartPosition(); pos != NULL;)
   {
      ComponentItem* schComponentItem = m_schCollection.getNextComponent(pos);
      if (schComponentItem != NULL && !schComponentItem->getCompareName().IsEmpty())
      {
         CString filteredSchematicName = killTrailing(killTrailingType, trailingSuffix, schComponentItem->getCompareName());

         bool isMatch = false;
         ComponentItem* pcbComponentItem = m_pcbCollection.findComponentItem(filteredSchematicName);
         if (pcbComponentItem != NULL)
         {
            if (caseSensitive)
            {
               isMatch = filteredSchematicName.Compare(pcbComponentItem->getCompareName()) == 0;
            }
            else
            {
               isMatch = true;
            }
         }

         if (isMatch)
         {
            schComponentItem->addMatchedComponentItem(pcbComponentItem);
            pcbComponentItem->addMatchedComponentItem(schComponentItem);

            matchSchematicToPcbMap.SetAt(filteredSchematicName, pcbComponentItem->getCompareName());
         }
         else
         {
            nonMatchSchematicMap.SetAt(filteredSchematicName, filteredSchematicName);
         }
      }
   }

   for (POSITION pos= m_pcbCollection.getComponentStartPosition(); pos != NULL;)
   {
      ComponentItem* pcbComponentItem = m_pcbCollection.getNextComponent(pos);
      if (pcbComponentItem != NULL && pcbComponentItem->getMatchedCount() == 0)
      {
         nonMatchPcbMap.SetAt(pcbComponentItem->getCompareName(), pcbComponentItem->getCompareName());
      }
   }
}

CString SchematicLink::killTrailing(const KillTrailingType killTrailingType, const CString trailingSuffix, const CString name)
{
   CString retval = name;

   if (killTrailingType == (int)killTrailingAlpha)
   {
      // Remove all trailing alpha, but if the trailing alpha is the first character
      // then we don't remove it.
      for (int i=name.GetLength(); i>0; i--)
      {
         if (!isalpha(name.GetAt(i-1)))
         {
            // Finally a character that is not alpha so any trailing alpha is already removed
            retval = name.Left(i);
            break;
         }
      }
   }
   else if (killTrailingType == (int)killTrailingSuffix)
   {
      // There is can be multiple trailing suffix delimited by comma so we have to find the first
      // trailling suffix in the name and remove it and everything after it.  But if the trailing suffix
      // is the first character then we don't remove it.

      CStringArray params;
      CSupString trialingSuffix(trailingSuffix);
      trialingSuffix.Parse(params, ",");

      for (int i=0; i<params.GetCount(); i++)
      {
         int index = retval.Find(params.GetAt(i));
         if (index > 0)
         {
            retval = retval.Left(index);
         }
      }
   }

   return retval;
}

void SchematicLink::clearNetItemMatch()
{
   m_schCollection.clearNetItemMatch();
   m_pcbCollection.clearNetItemMatch();
}

void SchematicLink::processNetItemMatchByName(const bool caseSensitive, CMapStringToString& nonMatchSchematicMap, CMapStringToString& nonMatchPcbMap, CMapStringToString& matchSchematicToPcbMap)
{
   if (!m_isDataLoaded)
      return;

   m_schCollection.clearNetItemMatch();
   m_pcbCollection.clearNetItemMatch();

   NetCollection& schNetCollection = m_schCollection.getNetCollection();
   NetCollection& pcbNetCollection = m_pcbCollection.getNetCollection();

   processNetItemMatchByName(caseSensitive, schNetCollection, pcbNetCollection, nonMatchSchematicMap, nonMatchPcbMap, matchSchematicToPcbMap);
}

void SchematicLink::processNetItemMatchByName(const bool caseSensitive, NetCollection& schNetCollection, NetCollection& pcbNetCollection, CMapStringToString& nonMatchSchematicMap, CMapStringToString& nonMatchPcbMap, CMapStringToString& matchSchematicToPcbMap)
{
   if (!m_isDataLoaded)
      return;

   for (POSITION pos=schNetCollection.GetStartPosition(); pos != NULL;)
   {
      CString key;
      NetItem* schNetItem = NULL;
      schNetCollection.GetNextAssoc(pos, key, schNetItem);
      if (schNetItem != NULL)
      {
         CString netName = schNetItem->getName();
         netName.MakeLower();

         bool isMatch = false;
         NetItem* pcbNetItem = NULL;
         if (pcbNetCollection.Lookup(netName, pcbNetItem) && pcbNetItem != NULL)
         {
            if (caseSensitive)
            {
               isMatch = schNetItem->getName().Compare(pcbNetItem->getName()) == 0;
            }
            else
            {
               isMatch = true;
            }
         }

         if (isMatch)
         {
            schNetItem->addMatchedNetItem(pcbNetItem);
            pcbNetItem->addMatchedNetItem(schNetItem);

            matchSchematicToPcbMap.SetAt(schNetItem->getName(), pcbNetItem->getName());
         }
         else
         {
            nonMatchSchematicMap.SetAt(schNetItem->getName(), schNetItem->getName());
         }
      }
   }

   for (POSITION pos=pcbNetCollection.GetStartPosition(); pos != NULL;)
   {
      CString key;
      NetItem* pcbNetItem = NULL;
      pcbNetCollection.GetNextAssoc(pos, key, pcbNetItem);

      if (pcbNetItem != NULL && pcbNetItem->getMatchedCount() == 0)
      {
         nonMatchPcbMap.SetAt(pcbNetItem->getName(), pcbNetItem->getName());
      }
   }
}

void SchematicLink::processNetItemMatchByContent(const bool caseSensitive, const bool followByNameMatch, const double matchPercentage, CMapStringToString& nonMatchSchematicMap, CMapStringToString& nonMatchPcbMap, CMapStringToString& matchSchematicToPcbMap)
{
   if (!m_isDataLoaded)
      return;

   m_schCollection.clearNetItemMatch();
   m_pcbCollection.clearNetItemMatch();

   processCompPinItemMatch();

   for (POSITION pos=m_schCollection.getNetItemStartPosition(); pos != NULL;)
   {
      NetItem* matchedPcbNetItem = NULL;
      NetItem* schNetItem = m_schCollection.getNextNetItem(pos);
      if (schNetItem != NULL)
      {
         int schCompPinMatchCount = 0;
         int pcbCompPinMatchCount = 0;

         for (POSITION compPinPos=schNetItem->getCompPinItemStartPosition(); compPinPos != NULL;)
         {
            CompPinItem* schCompPinItem = schNetItem->getNextCompPinItem(compPinPos);
            if (schCompPinItem != NULL)
            {
               CompPinItem* pcbCompPinItem = schCompPinItem->getMatchedCompPinItem();
               if (pcbCompPinItem != NULL )
               {  
                  NetItem* pcbNetItem = pcbCompPinItem->getNetItem();
                  if (pcbNetItem != NULL)
                  {
                     if (matchedPcbNetItem == NULL)
                     {
                        // Find first matching Pcb net

                        if (pcbNetItem->getMatchedCount() > 0)
                        {
                           // If Pcb net already has another matched Schematic net then stop checking
                           // because in Match By Content, only a one-to-one match is alllow between
                           // Schematic and Pcb nets.
                           break;
                        }
                        else
                        {
                           matchedPcbNetItem = pcbNetItem;
                        }
                     }
                     
                     if (pcbNetItem == matchedPcbNetItem)
                     {
                        // If the existing matched Pcb net is the same then increment comppin count
                        // otherewise ignore it
                        schCompPinMatchCount++;
                        pcbCompPinMatchCount++;
                     }
                  }
               }
            }
         }

         if (matchedPcbNetItem != NULL && schCompPinMatchCount > 0 && pcbCompPinMatchCount > 0)
         {
            double schMatchPercentage = schCompPinMatchCount/schNetItem->getCompPinItemCount() * 100;
            double pcbMatchPercentage = pcbCompPinMatchCount/matchedPcbNetItem->getCompPinItemCount() * 100;
       
            if (schMatchPercentage >= matchPercentage || pcbMatchPercentage >= matchPercentage)
            {
               // If either match comppin couunt of Schematic net or Pcb net is at least equal to the
               // matching percentage specified then the two net are consider matched
               schNetItem->addMatchedNetItem(matchedPcbNetItem);
               matchedPcbNetItem->addMatchedNetItem(schNetItem);

               matchSchematicToPcbMap.SetAt(schNetItem->getName(), matchedPcbNetItem->getName());
            }
         }

      }
   }

   doFollowUpNetItemMatchByName(caseSensitive, followByNameMatch, nonMatchSchematicMap, nonMatchPcbMap, matchSchematicToPcbMap);
}

void SchematicLink::processNetItemMatchByContentPcbAsMaster(const bool caseSensitive, const bool followByNameMatch, const double matchPercentage, CMapStringToString& nonMatchSchematicMap, CMapStringToString& nonMatchPcbMap, CMapStringToString& matchSchematicToPcbMap)
{
   if (!m_isDataLoaded)
      return;

   m_schCollection.clearNetItemMatch();
   m_pcbCollection.clearNetItemMatch();

   processCompPinItemMatch();

   int schCompPinMatchCount = 0;
   int pcbCompPinMatchCount = 0;

   for (POSITION pos=m_schCollection.getNetItemStartPosition(); pos != NULL;)
   {
      NetItem* matchedPcbNetItem = NULL;
      NetItem* schNetItem = m_schCollection.getNextNetItem(pos);
      if (schNetItem != NULL)
      {
         for (POSITION compPinPos=schNetItem->getCompPinItemStartPosition(); compPinPos != NULL;)
         {
            CompPinItem* schCompPinItem = schNetItem->getNextCompPinItem(compPinPos);
            if (schCompPinItem != NULL)
            {
               CompPinItem* pcbCompPinItem = schCompPinItem->getMatchedCompPinItem();
               if (pcbCompPinItem != NULL )
               {  
                  NetItem* pcbNetItem = pcbCompPinItem->getNetItem();
                  if (pcbNetItem != NULL)
                  {
                     if (matchedPcbNetItem == NULL)
                     {
                        // Find first matching Pcb net
                        matchedPcbNetItem = pcbNetItem;
                     }
                     
                     if (pcbNetItem == matchedPcbNetItem)
                     {
                        // If the existing matched Pcb net is the same then increment comppin count
                        // otherewise ignore it
                        schCompPinMatchCount++;
                        pcbCompPinMatchCount++;
                     }
                  }
               }
            }
         }

         if (matchedPcbNetItem != NULL && schCompPinMatchCount > 0 && pcbCompPinMatchCount > 0)
         {
            double schMatchPercentage = schCompPinMatchCount/schNetItem->getCompPinItemCount();
       
            if (schMatchPercentage >= matchPercentage)
            {
               // Schematic net MUST at least equal to the matching percentage specified in order
               // for it to consider a match to Pcb net
               schNetItem->addMatchedNetItem(matchedPcbNetItem);
               matchedPcbNetItem->addMatchedNetItem(schNetItem);

               matchSchematicToPcbMap.SetAt(schNetItem->getName(), matchedPcbNetItem->getName());
            }
         }

      }
   }

   doFollowUpNetItemMatchByName(caseSensitive, followByNameMatch, nonMatchSchematicMap, nonMatchPcbMap, matchSchematicToPcbMap);
}

void SchematicLink::processCompPinItemMatch()
{
   if (!m_isDataLoaded)
      return;

   for (POSITION pos=m_schCollection.getComponentStartPosition(); pos != NULL;)
   {
      ComponentItem* schComponentItem = m_schCollection.getNextComponent(pos);
      if (schComponentItem != NULL && schComponentItem->getMatchedCount() > 0)
      {
         ComponentItem* pcbComponentItem = schComponentItem->getFirstMatchedComponentItem();
         if (pcbComponentItem != NULL)
         {
            for (POSITION compPinPos=schComponentItem->getCompPinItemStartPosition(); compPinPos != NULL;)
            {
               CompPinItem* schCompPinItem = schComponentItem->getNextCompPinItem(compPinPos);
               if (schCompPinItem != NULL)
               {
                  CompPinItem* pcbCompPinItem = pcbComponentItem->findCompPinItem(schCompPinItem->getComparePinName());
                  if (pcbCompPinItem != NULL)
                  {
                     schCompPinItem->setMatchedCompPinItem(pcbCompPinItem);
                     pcbCompPinItem->setMatchedCompPinItem(schCompPinItem);
                  }
               }
            }
         }
      }
   }
}

void SchematicLink::doFollowUpNetItemMatchByName(const bool caseSensitive, const bool followByNameMatch, CMapStringToString& nonMatchSchematicMap, CMapStringToString& nonMatchPcbMap, CMapStringToString& matchSchematicToPcbMap)
{
   if (!m_isDataLoaded)
      return;

   NetCollection schNoMatchNetCollection(false);
   NetCollection pcbNoMatchNetCollection(false);

   for (POSITION pos=m_schCollection.getNetItemStartPosition(); pos != NULL;)
   {
      NetItem* schNetItem = m_schCollection.getNextNetItem(pos);
      if (schNetItem != NULL && schNetItem->getMatchedCount() == 0)
      {
         if (followByNameMatch)
         {
            CString netName = schNetItem->getName();
            netName.MakeLower();
            schNoMatchNetCollection.setAt(netName, schNetItem);
         }
         else
         {
            nonMatchSchematicMap.SetAt(schNetItem->getName(), schNetItem->getName());
         }
      }
   }

   for (POSITION pos=m_pcbCollection.getNetItemStartPosition(); pos != NULL;)
   {
      NetItem* pcbNetItem = m_pcbCollection.getNextNetItem(pos);
      if (pcbNetItem != NULL && pcbNetItem->getMatchedCount() == 0)
      {
         if (followByNameMatch)
         {
            CString netName = pcbNetItem->getName();
            netName.MakeLower();
            pcbNoMatchNetCollection.setAt(netName, pcbNetItem);
         }
         else
         {
            nonMatchPcbMap.SetAt(pcbNetItem->getName(), pcbNetItem->getName());
         }
      }
   }

   if (followByNameMatch)
   {
      processNetItemMatchByName(caseSensitive, schNoMatchNetCollection, pcbNoMatchNetCollection, nonMatchSchematicMap, nonMatchPcbMap, matchSchematicToPcbMap);
   }
}

int SchematicLink::getSchematicComponentMatch(const CString refname, CString& matchedName)
{
   int retval = ERR_NOTFOUND;

   if (!m_isDataLoaded)
      retval = ERR_NODATA;

   ComponentItem* schComponentItem = m_schCollection.findComponentItem(refname);

   //ComponentItem* schComponentItem = NULL;
   //for (POSITION pos=m_schCollection.getComponentStartPosition(); pos!=NULL;)
   //{
   //   ComponentItem* componentItem = m_schCollection.getNextComponent(pos);
   //   if (componentItem != NULL && componentItem->getName().Compare(refname) == 0)
   //   {
   //      schComponentItem = componentItem;
   //      break;
   //   }
   //}

   if (schComponentItem != NULL && schComponentItem->getMatchedCount() > 0)
   {
      ComponentItem* pcbComponentItem = schComponentItem->getFirstMatchedComponentItem();
      if (pcbComponentItem != NULL && pcbComponentItem->getDataStruct())
      {
         retval = ERR_NOERROR;
         matchedName = pcbComponentItem->getCompareName();

         zoomToComponent(m_pcbDoc, m_pcbFileStruct, *pcbComponentItem->getDataStruct(), pcbComponentItem->getSheetBlockNumber(), false);
      }
   }

   return retval;
}

int SchematicLink::getSchematicNetMatch(DataStruct& dataStruct, CString& matchedName)
{
   int retval = ERR_NOTFOUND;

   int netNameKw = m_schDoc.getStandardAttributeKeywordIndex(standardAttributeNetName);
   Attrib* attrib = NULL;
   if (dataStruct.lookUpAttrib(netNameKw, attrib))
   {
      CString netName = attrib->getStringValue();
      NetItem* schNetItem = m_schCollection.findNetItem(netName);
      if (schNetItem != NULL)
      {
         for (POSITION pos = schNetItem->getMatchNetItemStartPosition(); pos != NULL;)
         {
            NetItem* pcbNetItem = schNetItem->getNextMatchedNetItem(pos);
            if (pcbNetItem != NULL)
            {
               if (pcbNetItem->getItemDataCount() == 1)
               {
                  ItemData* itemData = pcbNetItem->getFirstItemData();
                  matchedName = pcbNetItem->getName();

                  if (itemData != NULL)
                  {
                     zoomToNet(m_pcbDoc, m_pcbFileStruct, *pcbNetItem, itemData->getSheetBlockNumber(), false);
                     //zoomToNet(m_pcbDoc, m_pcbFileStruct, *itemData->getDataStruct(), itemData->getSheetBlockNumber(), false);
                     break;
                  }
               }
               else
               {
                  matchedName = pcbNetItem->getName();

                  CSelectMatchedDlg selectMatchedDlg(*this, false, false);
                  selectMatchedDlg.setNetItem(pcbNetItem);

                  bool netSpanToOtherSheet = false;
                  int curSheetNumber = -1;
                  for (POSITION itemDataPos=pcbNetItem->getItemDataStartPosition(); itemDataPos != NULL;)
                  {
                     ItemData* itemData = pcbNetItem->getNextItemData(itemDataPos);
                     selectMatchedDlg.addMatchItemData(itemData);

                     if (curSheetNumber == -1)
                     {
                        curSheetNumber = itemData->getSheetBlockNumber();
                     }                        
                     else if (curSheetNumber != itemData->getSheetBlockNumber())
                     {
                        netSpanToOtherSheet = true;
                     }
                  }
   
                  if (netSpanToOtherSheet)
                  {
                     selectMatchedDlg.DoModal();
                  }
                  else
                  {
                     ItemData* itemData = pcbNetItem->getFirstItemData();
                     if (itemData)
                        zoomToNet(m_pcbDoc, m_pcbFileStruct, *pcbNetItem, itemData->getSheetBlockNumber(), false);
                  }
               }            

               // For now get return the first one found
               break;
            }
         }
      }
   }

   return retval;
}

int SchematicLink::getPcbComponentMatch(const CString refname, CString& matchedName)
{
   int retval = ERR_NOTFOUND;

   if (!m_isDataLoaded)
      retval = ERR_NODATA;

   ComponentItem* pcbComponentItem = m_pcbCollection.findComponentItem(refname);
   if (pcbComponentItem != NULL)
   {
      if (pcbComponentItem->getMatchedCount() == 1)
      {
         ComponentItem* schComponentItem = pcbComponentItem->getFirstMatchedComponentItem();
         if (schComponentItem != NULL && schComponentItem->getDataStruct() != NULL)
         {
            retval = ERR_NOERROR;
            matchedName = schComponentItem->getCompareName();

            zoomToComponent(m_schDoc, m_schFileStruct, *schComponentItem->getDataStruct(), schComponentItem->getSheetBlockNumber(), true);
         }
      }
      else if (pcbComponentItem->getMatchedCount() > 1)
      {
        CSelectMatchedDlg selectMatchedDlg(*this, true, true);

         for (POSITION pos=pcbComponentItem->getMatchedComponentItemStartPosition(); pos != NULL;)
         {
            ComponentItem* schComponentItem = pcbComponentItem->getNextMatchedComponentItem(pos);
            selectMatchedDlg.addMatchItemData(schComponentItem);  
         }

         selectMatchedDlg.DoModal();
      }
   }

   return retval;
}

int SchematicLink::getPcbNetMatch(DataStruct& dataStruct, CString& matchedName)
{
   int retval = ERR_NOTFOUND;

   int netNameKw = m_pcbDoc.getStandardAttributeKeywordIndex(standardAttributeNetName);
   Attrib* attrib = NULL;
   if (dataStruct.lookUpAttrib(netNameKw, attrib))
   {
      CString netName = attrib->getStringValue();
      NetItem* pcbNetItem = m_pcbCollection.findNetItem(netName);
      if (pcbNetItem != NULL)
      {
         for (POSITION pos = pcbNetItem->getMatchNetItemStartPosition(); pos != NULL;)
         {
            NetItem* schNetItem = pcbNetItem->getNextMatchedNetItem(pos);
            if (schNetItem != NULL)
            {
               // For now get return the first one found
               if (schNetItem->getItemDataCount() == 1)
               {
                  ItemData* itemData = schNetItem->getFirstItemData();
                  if (itemData != NULL)
                  {
                     zoomToNet(m_schDoc, m_schFileStruct, *schNetItem, itemData->getSheetBlockNumber(), true);
                     //zoomToNet(m_schDoc, m_schFileStruct, *itemData->getDataStruct(), itemData->getSheetBlockNumber(), true);
                  }
               }
               else if (schNetItem->getItemDataCount() > 1)
               {
                  matchedName = pcbNetItem->getName();

                  CSelectMatchedDlg selectMatchedDlg(*this, false, true);
                  selectMatchedDlg.setNetItem(schNetItem);

                  bool netSpanToOtherSheet = false;
                  int curSheetNumber = -1;
                  for (POSITION itemDataPos=schNetItem->getItemDataStartPosition(); itemDataPos != NULL;)
                  {
                     ItemData* itemData = schNetItem->getNextItemData(itemDataPos);
                     selectMatchedDlg.addMatchItemData(itemData);  
   
                     if (curSheetNumber == -1)
                     {
                        curSheetNumber = itemData->getSheetBlockNumber();
                     }                        
                     else if (curSheetNumber != itemData->getSheetBlockNumber())
                     {
                        netSpanToOtherSheet = true;
                     }
                  }
   
                  if (netSpanToOtherSheet)
                  {
                     selectMatchedDlg.DoModal();
                  }
                  else
                  {
                     ItemData* itemData = pcbNetItem->getFirstItemData();
                     zoomToNet(m_schDoc, m_schFileStruct, *schNetItem, itemData->getSheetBlockNumber(), false);
                  }
               }

               // For now get return the first one found
               break;
            }
         }
      }
   }

   return retval;
}

void SchematicLink::zoomToComponent(CCEtoODBDoc& camcadDoc, FileStruct& fileStruct, DataStruct& dataStruct, const int sheetBlockNumber, const bool isComponentSchematic)
{
   removeColorOverride();
   camcadDoc.UnselectAll(FALSE);

   if (dataStruct.getInsert() != NULL)
   {      
      BlockStruct* block = camcadDoc.getBlockAt(dataStruct.getInsert()->getBlockNumber());
      if (block != NULL)
      {
         BlockStruct* fileBlock = fileStruct.getBlock();   
         if (fileBlock == NULL || (sheetBlockNumber != fileBlock->getBlockNumber()))
         {
            fileStruct.setBlock(camcadDoc.getBlockAt(sheetBlockNumber));
            camcadDoc.OnFitPageToImage();
         }

         POSITION viewPos = camcadDoc.GetFirstViewPosition();
         CCEtoODBView* view = (CCEtoODBView*)camcadDoc.GetNextView(viewPos);
         if (view != NULL)
         {
            if (!block->extentIsValid())
               camcadDoc.CalcBlockExtents(block);

            CTMatrix matrix = dataStruct.getInsert()->getTMatrix();
            if (camcadDoc.getBottomView())
               matrix.scale(-1, 1);

            CExtent extent = block->getExtent();
            extent.transform(matrix);

	         double zoomMargin = isComponentSchematic?getApp().schSettings.SchMargin:getApp().schSettings.PcbMargin;
            zoomMargin /= 100;
	         double marginSizeX = (camcadDoc.getSettings().getXmax() - camcadDoc.getSettings().getXmin()) * zoomMargin;
	         double marginSizeY = (camcadDoc.getSettings().getYmax() - camcadDoc.getSettings().getYmin()) * zoomMargin;
	         double marginSize = max(marginSizeX, marginSizeY);

	         CDblRect rect;
            rect.xMax = extent.getXmax() + marginSize;
            rect.yMax = extent.getYmax() + marginSize;
            rect.xMin = extent.getXmin() - marginSize;
            rect.yMin = extent.getYmin() - marginSize;

	         view->ZoomBox(rect);
            dataStruct.setOverrideColor((isComponentSchematic?getApp().schSettings.SchHightlightColor:getApp().schSettings.PcbHightlightColor));
            dataStruct.setColorOverride(true);

            if (isComponentSchematic)
               m_lastSchColorOverrideDataStruct = &dataStruct;
            else
               m_lastPcbColorOverrideDataStruct = &dataStruct;
         }
      }
   }
}

void SchematicLink::zoomToNet(CCEtoODBDoc& camcadDoc, FileStruct& fileStruct, NetItem& netItem, const int sheetBlockNumber, const bool isNetSchematic)
{
   removeColorOverride();
   camcadDoc.UnselectAll(FALSE);

   BlockStruct* fileBlock = fileStruct.getBlock();   
   if (fileBlock == NULL || (sheetBlockNumber != fileBlock->getBlockNumber()))
   {
      fileStruct.setBlock(camcadDoc.getBlockAt(sheetBlockNumber));
      camcadDoc.OnFitPageToImage();
   }

   POSITION viewPos = camcadDoc.GetFirstViewPosition();
   CCEtoODBView* view = (CCEtoODBView*)camcadDoc.GetNextView(viewPos);
   if (view != NULL)
   {
      CDblRect rect;
      if (NetExtents(&camcadDoc, netItem.getNetStruct()->getNetName(), &rect))
      {
	      double zoomMargin = isNetSchematic?getApp().schSettings.SchMargin:getApp().schSettings.PcbMargin;
            zoomMargin /= 100;
	      double marginSizeX = (camcadDoc.getSettings().getXmax() - camcadDoc.getSettings().getXmin()) * zoomMargin;
	      double marginSizeY = (camcadDoc.getSettings().getYmax() - camcadDoc.getSettings().getYmin()) * zoomMargin;
	      double marginSize = max(marginSizeX, marginSizeY);

         rect.xMax += marginSize;
         rect.yMax += marginSize;
         rect.xMin -= marginSize;
         rect.yMin -= marginSize;

	      view->ZoomBox(rect);
         FlushQueue();

         if (netItem.getNetStruct() != NULL)
         {
            netItem.getNetStruct()->setHighlighted(true);
            int value = camcadDoc.RegisterValue(netItem.getNetStruct()->getNetName());
            camcadDoc.HighlightedNetsMap.SetAt(value, (isNetSchematic?getApp().schSettings.SchHightlightColor:getApp().schSettings.PcbHightlightColor));
            camcadDoc.HighlightByAttrib(FALSE, m_netNameKw, valueTypeString, value);
         }
      }
   }
}

void SchematicLink::zoomToNet(CCEtoODBDoc& camcadDoc, FileStruct& fileStruct, DataStruct& dataStruct, const int sheetBlockNumber, const bool isNetSchematic)
{
   removeColorOverride();
   camcadDoc.UnselectAll(FALSE);

   if (dataStruct.getPolyList() != NULL) 
   {
      BlockStruct* fileBlock = fileStruct.getBlock();   
      if (fileBlock == NULL || (sheetBlockNumber != fileBlock->getBlockNumber()))
      {
         fileStruct.setBlock(camcadDoc.getBlockAt(sheetBlockNumber));
         camcadDoc.OnFitPageToImage();
      }

      POSITION viewPos = camcadDoc.GetFirstViewPosition();
      CCEtoODBView* view = (CCEtoODBView*)camcadDoc.GetNextView(viewPos);
      if (view != NULL)
      {
         CExtent extent  = dataStruct.getPolyList()->getExtent();
         //extent.transform(matrix);

	      double zoomMargin = isNetSchematic?getApp().schSettings.SchMargin:getApp().schSettings.PcbMargin;
            zoomMargin /= 100;
	      double marginSizeX = (camcadDoc.getSettings().getXmax() - camcadDoc.getSettings().getXmin()) * zoomMargin;
	      double marginSizeY = (camcadDoc.getSettings().getYmax() - camcadDoc.getSettings().getYmin()) * zoomMargin;
	      double marginSize = max(marginSizeX, marginSizeY);

	      CDblRect rect;
         rect.xMax = extent.getXmax() + marginSize;
         rect.yMax = extent.getYmax() + marginSize;
         rect.xMin = extent.getXmin() - marginSize;
         rect.yMin = extent.getYmin() - marginSize;

	      view->ZoomBox(rect);
         dataStruct.setOverrideColor((isNetSchematic?getApp().schSettings.SchHightlightColor:getApp().schSettings.PcbHightlightColor));
         dataStruct.setColorOverride(true);

         if (isNetSchematic)
            m_lastSchColorOverrideDataStruct = &dataStruct;
         else
            m_lastPcbColorOverrideDataStruct = &dataStruct;


         //CDblRect rect;
         //camcadDoc.UnselectAll(FALSE);
         //if (!NetExtents(&camcadDoc, netItem.getName(), &rect))
         //{
	        // double zoomMargin = (isNetSchematic?getApp().schSettings.SchMargin:getApp().schSettings.PcbMargin) / 100;
	        // double marginSizeX = (camcadDoc.getSettings().getXmax() - camcadDoc.getSettings().getXmin()) * zoomMargin;
	        // double marginSizeY = (camcadDoc.getSettings().getYmax() - camcadDoc.getSettings().getYmin()) * zoomMargin;
	        // double marginSize = max(marginSizeX, marginSizeY);

	        // rect.xMin -= marginSize;
	        // rect.xMax += marginSize;		
	        // rect.yMin -= marginSize;
	        // rect.yMax += marginSize;     

         //   view->ZoomBox(rect);
         //   //FlushQueue();

         //   for (POSITION pos=netItem.getItemDataStartPosition(); pos != NULL;)
         //   {
         //      ItemData* itemData = netItem.getNextItemData(pos);
         //      if (itemData != NULL && itemData->getSheetBlockNumber() == selectedSheetBlockNumber && itemData->getDataStruct() != NULL)
         //      {
         //         itemData->getDataStruct()->setOverrideColor((isNetSchematic?getApp().schSettings.SchHightlightColor:getApp().schSettings.PcbHightlightColor));
         //         itemData->getDataStruct()->setColorOverride(true);
         //      }
         //   }
         //}
      }
   }
}

void SchematicLink::removeColorOverride()
{
   // Remove override color for Schematic component
   if (m_lastSchColorOverrideDataStruct != NULL)
   {
      BOOL needRedraw = m_lastSchColorOverrideDataStruct->getColorOverride();
      m_lastSchColorOverrideDataStruct->setColorOverride(false);

      if (needRedraw)
      {
         SelectStruct s;
         s.setData(m_lastSchColorOverrideDataStruct);
         s.setParentDataList(&m_schFileStruct.getBlock()->getDataList());
         s.scale = (DbUnit)m_schFileStruct.getScale();
         s.insert_x = (DbUnit)m_schFileStruct.getInsertX();
         s.insert_y = (DbUnit)m_schFileStruct.getInsertY();
         s.rotation = (DbUnit)m_schFileStruct.getRotation();
         s.mirror = m_schFileStruct.isMirrored();
         m_schDoc.DrawEntity(&s, 0, TRUE);
      }

      m_lastSchColorOverrideDataStruct = NULL;
   }

   // Remove override color for Pcb component
   if (m_lastPcbColorOverrideDataStruct != NULL)
   {
      BOOL needRedraw = m_lastPcbColorOverrideDataStruct->getColorOverride();
      m_lastPcbColorOverrideDataStruct->setColorOverride(false);

      if (needRedraw)
      {
         SelectStruct s;
         s.setData(m_lastPcbColorOverrideDataStruct);
         s.setParentDataList(&m_pcbFileStruct.getBlock()->getDataList());
         s.scale = (DbUnit)m_pcbFileStruct.getScale();
         s.insert_x = (DbUnit)m_pcbFileStruct.getInsertX();
         s.insert_y = (DbUnit)m_pcbFileStruct.getInsertY();
         s.rotation = (DbUnit)m_pcbFileStruct.getRotation();
         s.mirror = m_pcbFileStruct.isMirrored();
         m_pcbDoc.DrawEntity(&s, 0, TRUE);
      }

      m_lastPcbColorOverrideDataStruct = NULL;
   }

   // Remove override color for Schematic net
   if (!m_schDoc.HighlightedNetsMap.IsEmpty())
   {
      if (m_schDoc.HighlightedNetsMap.GetCount() > 1)
      {
         m_schDoc.HighlightedNetsMap.RemoveAll();
         m_schDoc.UpdateAllViews(NULL);
      }
      else
      {
         POSITION pos = m_schDoc.HighlightedNetsMap.GetStartPosition();
         if (pos)
         {
            int value;
            COLORREF key;
            m_schDoc.HighlightedNetsMap.GetNextAssoc(pos, value, key);
            m_schDoc.UnhighlightByAttrib(FALSE, m_netNameKw, valueTypeString, value);
         }
      }
   }

   // Remove override color for Pcb net
   if (!m_pcbDoc.HighlightedNetsMap.IsEmpty())
   {
      if ( m_pcbDoc.HighlightedNetsMap.GetCount() > 1)
      {
         m_pcbDoc.HighlightedNetsMap.RemoveAll();
         m_pcbDoc.UpdateAllViews(NULL);
      }
      else
      {
         POSITION pos =  m_pcbDoc.HighlightedNetsMap.GetStartPosition();
         if (pos)
         {
            int value;
            COLORREF key;
            m_pcbDoc.HighlightedNetsMap.GetNextAssoc(pos, value, key);
            m_pcbDoc.UnhighlightByAttrib(FALSE, m_netNameKw, valueTypeString, value);
         }
      }
   }

}

//---------------------------------------------------------------------------------------
// SchematicLinkController
//---------------------------------------------------------------------------------------
SchematicLinkController::SchematicLinkController()
{
   m_schematicLink = NULL;
   m_schematicLinkInSession = false;
   m_netCrossProbeOn = false;
   m_componentCrossProbeOn = false;
}

SchematicLinkController::~SchematicLinkController()
{
	deleteSchematicLink();
}

void SchematicLinkController::deleteSchematicLink()
{
   delete m_schematicLink;
   m_schematicLink = NULL;

   m_schematicLinkInSession = false;
   m_netCrossProbeOn = false;
   m_componentCrossProbeOn = false;
}

bool SchematicLinkController::startSchematicLink()
{
   bool retval = false;
   if (m_schematicLink != NULL)
   {
		//CString buf = "There were previous cross reference names found.  Do you want to continue?\n\n";
		//buf += "'Yes' - Use Schematic Link with the cross reference names found.\n";
		//buf += "'No' - Re-compare files and use new data for Schematic Link.\n";

		//retval = (AfxMessageBox(buf, MB_YESNO) == IDYES);

      delete m_schematicLink;
      m_schematicLink = NULL;
   }

   if (!retval)
	{

		CCEtoODBDoc* schematicDoc = NULL;
		CCEtoODBDoc* pcbDoc = NULL;
		if (getCamcadDocuments(&schematicDoc, &pcbDoc) && schematicDoc != NULL && pcbDoc != NULL)
		{
         FileStruct* schFile = schematicDoc->getFileList().GetOnlyShown(blockTypeSheet);
         FileStruct* pcbFile = pcbDoc->getFileList().GetOnlyShown(blockTypePcb);
         bool validDocuments = (schFile != NULL && pcbFile != NULL);

         if (validDocuments)
         {
            schematicDoc->schLinkPCBDoc = FALSE;
            pcbDoc->schLinkPCBDoc = TRUE;

			   m_schematicLink = new SchematicLink(*schematicDoc, *pcbDoc, *schFile, *pcbFile);
			   m_schematicLink->loadData();
            retval = m_schematicLink->processAutoCrosslink();
            //retval = true;

			   //if (launchSchematicLinkDialog(*schematicLink))
			   //{
				  // delete m_schematicLink;
				  // m_schematicLink = schematicLink;

      //         retval = true;
			   //}
			   //else
			   //{
				  // delete schematicLink;
				  // schematicLink = NULL;
			   //}
         }
		}
	}

   return retval;
}

bool SchematicLinkController::getCamcadDocuments(CCEtoODBDoc** schematicDoc, CCEtoODBDoc** pcbDoc)
{
   bool retval = false;
   int docCount = 0;
   CString errMsg = "Only 2 documents (a PCB and schematic) can and must be open when using schematic linking.";

   for (POSITION docTempPos = getApp().GetFirstDocTemplatePosition(); docTempPos != NULL;)
   {
		CMultiDocTemplate* pDocTemplate = (CMultiDocTemplate*)getApp().GetNextDocTemplate(docTempPos);
      if (pDocTemplate != NULL)
      {
         for (POSITION pos = pDocTemplate->GetFirstDocPosition(); pos != NULL;)
         {
			   CCEtoODBDoc* pDoc = (CCEtoODBDoc*)pDocTemplate->GetNextDoc(pos);
            if (pDoc != NULL)
            {  
               docCount++;
               if (docCount > 2)
               {
                  // More than two documents are available
                  ErrorMessage(errMsg, "Schematic Linking Error", MB_OK);
                  *schematicDoc = NULL;
                  *pcbDoc = NULL;
                  return false;
               }

               FileStruct* file = pDoc->getFileList().GetOnlyShown(blockTypeUndefined);
               if (file != NULL)
               {
                  if (file->getBlockType() == blockTypeSheet)
                     *schematicDoc = pDoc;
                  else if (file->getBlockType() == blockTypePcb)
                     *pcbDoc = pDoc;
               }
            }
         }
      }
   }

   if (docCount != 2 || *schematicDoc == NULL || *pcbDoc == NULL)
   {
      // More than two documents are available
      ErrorMessage(errMsg, "Schematic Linking Error", MB_OK);
      *schematicDoc = NULL;
      *pcbDoc = NULL;
   }
   else
   {
      POSITION viewPos = (*schematicDoc)->GetFirstViewPosition();
      if (viewPos)
      {
         CView *view = (*schematicDoc)->GetNextView(viewPos);
         view->GetParentFrame()->ActivateFrame();
      }
      ((CMDIFrameWnd*)getMainWnd())->MDITile(MDITILE_VERTICAL);

      retval = TRUE;
   }

   return retval;
}

bool SchematicLinkController::launchSchematicLinkDialog(SchematicLink& schematicLink)
{
   CSchematicLinkCompareRefdesDlg propertyPage1(schematicLink);
   CSchematicLinkCompareNetDlg propertyPage2(schematicLink);
  
   CPropertySheet dlgPropSheet("Schematic Link");
   dlgPropSheet.SetWizardMode();
   dlgPropSheet.AddPage(&propertyPage1);
   dlgPropSheet.AddPage(&propertyPage2);

   bool retval = (dlgPropSheet.DoModal() == ID_WIZFINISH);

   return retval;
}

void SchematicLinkController::stopSchematicLink()
{
	m_schematicLinkInSession = false;
   m_netCrossProbeOn = false;
   m_componentCrossProbeOn = false;

	CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
	CToolBarCtrl *c = &(frame->m_SchematicToolBar.GetToolBarCtrl());
	c->PressButton(ID_LINK_COMPS, 0);
	c->PressButton(ID_LINK_NETS, 0);
}

void SchematicLinkController::toggerSchematicLink()
{
   if (m_schematicLinkInSession)
   {
      stopSchematicLink();
   }
   else if (startSchematicLink())
   {
      m_schematicLinkInSession = true;
      m_netCrossProbeOn = true;
      m_componentCrossProbeOn = true;

      CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
		CToolBarCtrl *c = &(frame->m_SchematicToolBar.GetToolBarCtrl());
		c->PressButton(ID_LINK_COMPS, 1);
		c->PressButton(ID_LINK_NETS, 1);
   }
}

bool SchematicLinkController::requestDeleteOfSchematicLink()
{
   bool retval = true;

   if (m_schematicLinkInSession)
   {
      CString msg = (CString)"Schematic Link is currently running.\n\n" +
         "Continuing the requested action will render cross probing\n" +
         "unusable and will be disabled.  Re-scaning of the files will\n" +
         "be required to begin Schematic Link again.\n\n" + 
         "Do you want to continue?";
      int result = ErrorMessage(msg, "Shematic Link", MB_YESNO);
      
      if (result == IDYES)
      {
         deleteSchematicLink();   
      }
      else
      {
         retval = false;
      }
   }
 
   return retval;
}

void SchematicLinkController::clearCrossProbedColors() 
{
   removeHighlightedNet();
   removeOverriddenComponent();
}

void SchematicLinkController::removeHighlightedNet()
{
}

void SchematicLinkController::removeOverriddenComponent()
{
}

int SchematicLinkController::getMatchForSchematic(DataStruct& schDataStruct, CString& matchedName)
{ 
   int retval = ERR_SETOFF;

   if (m_schematicLink != NULL)
   {
      if (schDataStruct.getDataType() == dataTypeInsert && m_componentCrossProbeOn)
      {
         InsertStruct* insert = schDataStruct.getInsert();
         if (insert != NULL && insert->getInsertType() == insertTypeSymbol)
         {
            // Assume it is always DESIGNATOR
            int compareNameKw = m_schematicLink->getSchematicCamcadDoc().RegisterKeyWord(SCH_ATT_DESIGNATOR, valueTypeString);
            CString compareName = insert->getRefname();
            Attrib* attrib = NULL;
            if (schDataStruct.lookUpAttrib(compareNameKw, attrib))
            {
               CString compareNameValue = attrib->getStringValue();
               if (!compareNameValue.IsEmpty())
               {
                  compareName = compareNameValue;
               }
            }
            //compareName = trimTrailing(compareName);
            retval = m_schematicLink->getSchematicComponentMatch(compareName, matchedName);

            //CString refname = insert->getRefname();
            //retval = m_schematicLink->getSchematicComponentMatch(refname, matchedName);
         }
      }
      else if (schDataStruct.getDataType() == dataTypePoly && m_netCrossProbeOn)
      {
         if (schDataStruct.getGraphicClass() == graphicClassSignal)
         {
            retval = m_schematicLink->getSchematicNetMatch(schDataStruct, matchedName);
         }
      }
   }

   return retval; 
}

int SchematicLinkController::getMatchForPcb(DataStruct& pcbDataStruct, CString& matchedName) 
{ 
   int retval = ERR_SETOFF;
   if (m_schematicLink != NULL)
   {
      if (pcbDataStruct.getDataType() == dataTypeInsert && m_componentCrossProbeOn)
      {
         InsertStruct* insert = pcbDataStruct.getInsert();
         if (insert != NULL && (insert->getInsertType() == insertTypePcbComponent || insert->getInsertType() == insertTypeDie))
         {
            CString refname = insert->getRefname();
            retval = m_schematicLink->getPcbComponentMatch(refname, matchedName);
         }
      }
      else if (pcbDataStruct.getDataType() == dataTypePoly && m_netCrossProbeOn)
      {
         if (pcbDataStruct.getGraphicClass() == graphicClassEtch)
         {
            retval = m_schematicLink->getPcbNetMatch(pcbDataStruct, matchedName);
         }
      }
   }

   return retval; 
}

//---------------------------------------------------------------------------------------
// AutoCrosslinkResultDlg dialog
//---------------------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(AutoCrosslinkResultDlg, CDialog)
AutoCrosslinkResultDlg::AutoCrosslinkResultDlg(const int pcbCompMatchedCount, const int pcbCompTotalCount, const int schCompMatchedCount, const int schCompTotalCount,
   const int pcbNetMatchedCount, const int pcbNetTotalCount, const int schNetMatchedCount, const int schNetTotalCount, const CString logFilePath)
	: CDialog(AutoCrosslinkResultDlg::IDD, NULL)
   , m_pcbCompMatchedCount(pcbCompMatchedCount)
   , m_pcbCompTotalCount(pcbCompTotalCount)
   , m_schCompMatchedCount(schCompMatchedCount)
   , m_schCompTotalCount(schCompTotalCount)
   , m_pcbNetMatchedCount(pcbNetMatchedCount)
   , m_pcbNetTotalCount(pcbNetTotalCount)
   , m_schNetMatchedCount(schNetMatchedCount)
   , m_schNetTotalCount(schNetTotalCount)
   , m_logFilePath(logFilePath)
{
}

AutoCrosslinkResultDlg::~AutoCrosslinkResultDlg()
{
}

void AutoCrosslinkResultDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_ResultList, m_resultsList);
}

BEGIN_MESSAGE_MAP(AutoCrosslinkResultDlg, CDialog)
   ON_BN_CLICKED(IDC_ViewLog, OnBnClickedViewlog)
END_MESSAGE_MAP()

BOOL AutoCrosslinkResultDlg::OnInitDialog()
{
   CDialog::OnInitDialog();
   
   displayResults();

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void AutoCrosslinkResultDlg::displayResults()
{
   m_resultsList.DeleteAllItems();

   m_resultsList.InsertColumn(0,"",LVCFMT_LEFT,70);
   m_resultsList.InsertColumn(1,"",LVCFMT_RIGHT,50);
   m_resultsList.InsertColumn(2,"",LVCFMT_LEFT,65);
   m_resultsList.InsertColumn(3,"",LVCFMT_RIGHT,50);

   //int itemIndex = m_resultsList.InsertItem(LVIF_TEXT,0,strText,0,0,0,0);
   //m_resultsList.SetItemText(itemIndex,SurfaceColumn  ,surface);
   //m_resultsList.SetItemText(itemIndex,ViolationColumn,stencilViolation->getDescription());



//============
   CString strText;

   // Insert the item
   strText = "PCB Comps";
   int itemIndex = m_resultsList.InsertItem(LVIF_TEXT,0,strText,0,0,0,0);
   strText.Format("%d", m_pcbCompMatchedCount);
   m_resultsList.SetItemText(itemIndex, 1, strText);
   strText.Format("of  %d", m_pcbCompTotalCount);
   m_resultsList.SetItemText(itemIndex, 2, strText);
   strText.Format("%0.f%%", floor(((double)m_pcbCompMatchedCount/(double)m_pcbCompTotalCount)*100));
   m_resultsList.SetItemText(itemIndex, 3, strText);

   strText = "Sch Comps";
   itemIndex = m_resultsList.InsertItem(LVIF_TEXT|LVIF_STATE, 1, strText, 0, 0, 0, 0);
   strText.Format("%d", m_schCompMatchedCount);
   m_resultsList.SetItemText(itemIndex, 1, strText);
   strText.Format("of  %d", m_schCompTotalCount);
   m_resultsList.SetItemText(itemIndex, 2, strText);
   strText.Format("%0.f%%", floor(((double)m_schCompMatchedCount/(double)m_schCompTotalCount)*100));
   m_resultsList.SetItemText(itemIndex, 3, strText);

   strText = "PCB Nets";
   itemIndex = m_resultsList.InsertItem(LVIF_TEXT|LVIF_STATE, 2, strText, 0, 0, 0, 0);
   strText.Format("%d", m_pcbNetMatchedCount);
   m_resultsList.SetItemText(itemIndex, 1, strText);
   strText.Format("of  %d", m_pcbNetTotalCount);
   m_resultsList.SetItemText(itemIndex, 2, strText);
   strText.Format("%0.f%%", floor(((double)m_pcbNetMatchedCount/(double)m_pcbNetTotalCount)*100));
   m_resultsList.SetItemText(itemIndex, 3, strText);

   strText = "Sch Nets";
   itemIndex = m_resultsList.InsertItem(LVIF_TEXT|LVIF_STATE, 3, strText, 0, 0, 0, 0);
   strText.Format("%d", m_schNetMatchedCount);
   m_resultsList.SetItemText(itemIndex, 1, strText);
   strText.Format("of  %d", m_schNetTotalCount);
   m_resultsList.SetItemText(itemIndex, 2, strText);
   strText.Format("%0.f%%", floor(((double)m_schNetMatchedCount/(double)m_schNetTotalCount)*100));
   m_resultsList.SetItemText(itemIndex, 3, strText);
}

void AutoCrosslinkResultDlg::OnBnClickedViewlog()
{
   if (!m_logFilePath.IsEmpty())
   {
      if (fileExists(m_logFilePath))
      {
         ShellExecute(AfxGetApp()->GetMainWnd()->m_hWnd,"open",m_logFilePath,NULL,NULL,SW_SHOW);
      }     
   }
}
