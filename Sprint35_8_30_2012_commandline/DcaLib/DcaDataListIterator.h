// $Header: /CAMCAD/DcaLib/DcaDataListIterator.h 3     3/09/07 5:15p Kurt Van Ness $

#if !defined(__DcaDataListIterator_h__)
#define __DcaDataListIterator_h__

#pragma once

#include "DcaInsertType.h"

class BlockStruct;
class CDataList;
class DataStruct;

enum DataTypeTag;
enum InsertTypeTag;

//_____________________________________________________________________________
class CDataListIterator
{
protected:
   const CDataList& m_dataList;
   DataTypeTag m_dataTypeFilter;
   InsertTypeTag m_insertTypeFilter;
   CInsertTypeMask m_insertTypeMask;
   POSITION m_pos;
   int m_index;
   bool m_filterFlag;
   bool m_insertFilterFlag;
   bool m_insertMaskFlag;

public:
   CDataListIterator(const BlockStruct& block);
   CDataListIterator(const BlockStruct& block,DataTypeTag dataTypeFilter);
   CDataListIterator(const BlockStruct& block,InsertTypeTag insertTypeFilter);
   CDataListIterator(const BlockStruct& block,CInsertTypeMask insertTypeMask);
   CDataListIterator(const CDataList& dataList);
   CDataListIterator(const CDataList& dataList,DataTypeTag dataTypeFilter);
   CDataListIterator(const CDataList& dataList,InsertTypeTag insertTypeFilter);
   CDataListIterator(const CDataList& dataList,CInsertTypeMask insertTypeMask);

   bool atEnd() const;
   bool hasNext() const;
   virtual DataStruct* getNext();
   int getIndex() const;

protected:
   virtual void init();
};

//_____________________________________________________________________________
class CModifiableDataListIterator : public CDataListIterator
{
private:
   CDataList& m_modifiableDataList;
   DataStruct* m_currentData;
   POSITION m_currentDataPos;

public:
   CModifiableDataListIterator(BlockStruct& block);
   CModifiableDataListIterator(BlockStruct& block,DataTypeTag dataTypeFilter);
   CModifiableDataListIterator(BlockStruct& block,InsertTypeTag insertTypeFilter);
   CModifiableDataListIterator(BlockStruct& block,CInsertTypeMask insertTypeMask);
   CModifiableDataListIterator(CDataList& dataList);
   CModifiableDataListIterator(CDataList& dataList,DataTypeTag dataTypeFilter);
   CModifiableDataListIterator(CDataList& dataList,InsertTypeTag insertTypeFilter);
   CModifiableDataListIterator(CDataList& dataList,CInsertTypeMask insertTypeMask);

public:
   virtual DataStruct* getNext();

public:
   void deleteCurrent();
   DataStruct* removeCurrent();
   DataStruct* getCurrent();

protected:
   virtual void init();
};

#endif
