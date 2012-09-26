// $Header: /CAMCAD/DcaLib/DcaDataListIterator.cpp 4     3/09/07 5:15p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaDataListIterator.h"
#include "DcaBlock.h"
#include "DcaDataType.h"
#include "DcaInsert.h"

//_____________________________________________________________________________
CDataListIterator::CDataListIterator(const BlockStruct& block)
: m_dataList(block.getDataList())
, m_filterFlag(false)
, m_insertFilterFlag(false)
, m_insertMaskFlag(false)
, m_dataTypeFilter(dataTypeUndefined)
, m_insertTypeFilter(insertTypeUndefined)
{
   init();
}

CDataListIterator::CDataListIterator(const CDataList& dataList)
: m_dataList(dataList)
, m_filterFlag(false)
, m_insertFilterFlag(false)
, m_insertMaskFlag(false)
, m_dataTypeFilter(dataTypeUndefined)
, m_insertTypeFilter(insertTypeUndefined)
{
   init();
}

CDataListIterator::CDataListIterator(const BlockStruct& block,DataTypeTag dataTypeFilter)
: m_dataList(block.getDataList())
, m_filterFlag(true)
, m_insertFilterFlag(false)
, m_insertMaskFlag(false)
, m_dataTypeFilter(dataTypeFilter)
, m_insertTypeFilter(insertTypeUndefined)
{
   init();
}

CDataListIterator::CDataListIterator(const CDataList& dataList,DataTypeTag dataTypeFilter)
: m_dataList(dataList)
, m_filterFlag(true)
, m_insertFilterFlag(false)
, m_insertMaskFlag(false)
, m_dataTypeFilter(dataTypeFilter)
, m_insertTypeFilter(insertTypeUndefined)
{
   init();
}

CDataListIterator::CDataListIterator(const BlockStruct& block,InsertTypeTag insertTypeFilter)
: m_dataList(block.getDataList())
, m_filterFlag(true)
, m_insertFilterFlag(true)
, m_insertMaskFlag(false)
, m_dataTypeFilter(dataTypeInsert)
, m_insertTypeFilter(insertTypeFilter)
{
   init();
}

CDataListIterator::CDataListIterator(const CDataList& dataList,InsertTypeTag insertTypeFilter)
: m_dataList(dataList)
, m_filterFlag(true)
, m_insertFilterFlag(true)
, m_insertMaskFlag(false)
, m_dataTypeFilter(dataTypeInsert)
, m_insertTypeFilter(insertTypeFilter)
{
   init();
}

CDataListIterator::CDataListIterator(const BlockStruct& block,CInsertTypeMask insertTypeMask)
: m_dataList(block.getDataList())
, m_filterFlag(true)
, m_insertFilterFlag(false)
, m_insertMaskFlag(true)
, m_dataTypeFilter(dataTypeInsert)
, m_insertTypeFilter(insertTypeUndefined)
, m_insertTypeMask(insertTypeMask)
{
   init();
}

CDataListIterator::CDataListIterator(const CDataList& dataList,CInsertTypeMask insertTypeMask)
: m_dataList(dataList)
, m_filterFlag(true)
, m_insertFilterFlag(false)
, m_insertMaskFlag(true)
, m_dataTypeFilter(dataTypeInsert)
, m_insertTypeFilter(insertTypeUndefined)
, m_insertTypeMask(insertTypeMask)
{
   init();
}

void CDataListIterator::init()
{
   m_index            = 0;

   if (m_filterFlag)
   {
      for (m_pos = m_dataList.GetHeadPosition();m_pos != NULL;)
      {
         DataStruct* data = m_dataList.GetAt(m_pos);

         if (data->getDataType() == m_dataTypeFilter)
         {
            if (m_insertFilterFlag)
            {
               if (data->getInsert()->getInsertType() == m_insertTypeFilter)
               {
                  break;  // found insert type match
               }
            }
            else if (m_insertMaskFlag)
            {
               if (m_insertTypeMask.contains(data->getInsert()->getInsertType()))
               {
                  break;  // found insert type mask match
               }
            }
            else
            {
               break;  // found data type match
            }
         }

         // no match found
         m_dataList.GetNext(m_pos);
         m_index++;
      }
   }
   else
   {
      m_pos = m_dataList.GetHeadPosition();
   }
}

int CDataListIterator::getIndex() const
{
   return m_index;
}

bool CDataListIterator::atEnd() const
{
   bool retval = (m_pos == NULL);

   return retval;
}

bool CDataListIterator::hasNext() const
{
   bool retval = (m_pos != NULL);

   return retval;
}

DataStruct* CDataListIterator::getNext()
{
   DataStruct* data = m_dataList.GetNext(m_pos);
   m_index++;

   if (m_filterFlag)
   {
      while (m_pos != NULL)
      {
         DataStruct* nextdata = m_dataList.GetAt(m_pos);

         if (nextdata->getDataType() == m_dataTypeFilter)
         {
            if (m_insertFilterFlag)
            {
               if (nextdata->getInsert()->getInsertType() == m_insertTypeFilter)
               {
                  break;  // found insert type match
               }
            }
            else if (m_insertMaskFlag)
            {
               if (m_insertTypeMask.contains(nextdata->getInsert()->getInsertType()))
               {
                  break;  // found insert type mask match
               }
            }
            else
            {
               break;  // found data type match
            }
         }

         // no match found
         m_dataList.GetNext(m_pos);
         m_index++;
      }

      // for debugging
      //if (m_insertFilterFlag && data->getInsert()->getInsertType() != m_insertTypeFilter)
      //{
      //   int iii = 3;
      //}
   }

   return data;
}

//_________________________________________________________________________________________________
CModifiableDataListIterator::CModifiableDataListIterator(BlockStruct& block)
: CDataListIterator(block)
, m_modifiableDataList(block.getDataList())
{
}

CModifiableDataListIterator::CModifiableDataListIterator(BlockStruct& block,DataTypeTag dataTypeFilter)
: CDataListIterator(block,dataTypeFilter)
, m_modifiableDataList(block.getDataList())
{
}

CModifiableDataListIterator::CModifiableDataListIterator(BlockStruct& block,InsertTypeTag insertTypeFilter)
: CDataListIterator(block,insertTypeFilter)
, m_modifiableDataList(block.getDataList())
{
}

CModifiableDataListIterator::CModifiableDataListIterator(BlockStruct& block,CInsertTypeMask insertTypeMask)
: CDataListIterator(block,insertTypeMask)
, m_modifiableDataList(block.getDataList())
{
}

CModifiableDataListIterator::CModifiableDataListIterator(CDataList& dataList)
: CDataListIterator(dataList)
, m_modifiableDataList(dataList)
{
}

CModifiableDataListIterator::CModifiableDataListIterator(CDataList& dataList,DataTypeTag dataTypeFilter)
: CDataListIterator(dataList,dataTypeFilter)
, m_modifiableDataList(dataList)
{
}

CModifiableDataListIterator::CModifiableDataListIterator(CDataList& dataList,InsertTypeTag insertTypeFilter)
: CDataListIterator(dataList,insertTypeFilter)
, m_modifiableDataList(dataList)
{
}

CModifiableDataListIterator::CModifiableDataListIterator(CDataList& dataList,CInsertTypeMask insertTypeMask)
: CDataListIterator(dataList,insertTypeMask)
, m_modifiableDataList(dataList)
{
}

void CModifiableDataListIterator::init()
{
   m_currentData    = NULL;
   m_currentDataPos = NULL;

   CDataListIterator::init();
}

DataStruct* CModifiableDataListIterator::getNext()
{
   m_currentDataPos = m_pos;
   m_currentData    = m_dataList.GetNext(m_pos);
   m_index++;

   if (m_filterFlag)
   {
      while (m_pos != NULL)
      {
         DataStruct* nextdata = m_dataList.GetAt(m_pos);

         if (nextdata->getDataType() == m_dataTypeFilter)
         {
            if (m_insertFilterFlag)
            {
               if (nextdata->getInsert()->getInsertType() == m_insertTypeFilter)
               {
                  break;  // found insert type match
               }
            }
            else if (m_insertMaskFlag)
            {
               if (m_insertTypeMask.contains(nextdata->getInsert()->getInsertType()))
               {
                  break;  // found insert type mask match
               }
            }
            else
            {
               break;  // found data type match
            }
         }

         // no match found
         m_dataList.GetNext(m_pos);
         m_index++;
      }

      // for debugging
      //if (m_insertFilterFlag && data->getInsert()->getInsertType() != m_insertTypeFilter)
      //{
      //   int iii = 3;
      //}
   }

   return m_currentData;
}

void CModifiableDataListIterator::deleteCurrent()
{
   if (m_currentData != NULL && m_currentDataPos != NULL)
   {
      m_modifiableDataList.RemoveAt(m_currentDataPos);

      delete m_currentData;

      m_currentData = NULL;
   }
}

DataStruct* CModifiableDataListIterator::removeCurrent()
{
   DataStruct* currentData = NULL;

   if (m_currentData != NULL && m_currentDataPos != NULL)
   {
      m_modifiableDataList.RemoveAt(m_currentDataPos);

      currentData = m_currentData;

      m_currentData = NULL;
   }

   return currentData;
}

DataStruct* CModifiableDataListIterator::getCurrent()
{
   return m_currentData;
}



