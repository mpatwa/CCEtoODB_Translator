// $Header: /CAMCAD/4.6/Debug.h 10    10/12/06 9:21p Kurt Van Ness $

#if !defined(__Debug_H__)
#define __Debug_H__ 

#pragma once

#include "TypedContainer.h"

//#define EnableDataStructTracker
//#define EnableSelectStructTracker

class CCEtoODBDoc;
class CPolygon;
class CPoly;
class CPoint2d;
class CExtent;
class DataStruct;
class CExtendedRgn;
class CCamCadDatabase;
class CBasesVector;
class CCamCadData;

class CDebug
{
private:
   static int m_nextDebugPolygonId;
   static int m_nextDebugPolyId;
   static int m_nextDebugPointId;
   static int m_nextDebugExtentId;
   static int m_nextDebugRegionId;
   static bool m_addEnable;

public:
   static DataStruct* addDebugPolygon(CCEtoODBDoc& camCadDoc,CPolygon& polygon,const char* format,...);
   static DataStruct* addDebugPoly(CCEtoODBDoc& camCadDoc,CPoly& poly,const char* format,...);
   static DataStruct* addDebugPoint(CCEtoODBDoc& camCadDoc,const CPoint2d& point,const char* format,...);
   static DataStruct* addDebugExtent(CCEtoODBDoc& camCadDoc,const CExtent& extent,double rotationDegrees,const char* format,...);
   static DataStruct* addDebugRegion(CCEtoODBDoc& camCadDoc,const CExtendedRgn& region,const char* format,...);
   static DataStruct* addDebugAxis(CCamCadDatabase& camCadDatabase,const CBasesVector& basesVector,double unitSize,const char* format,...);
   static DataStruct* addDebugAxis(CCamCadData& camCadData,const CBasesVector& basesVector,double unitSize,const char* format,...);

   static DataStruct* addDebugPolygon(CPolygon& polygon,const char* format,...);
   static DataStruct* addDebugPoly(CPoly& poly,const char* format,...);
   static DataStruct* addDebugPoint(const CPoint2d& point,const char* format,...);
   static DataStruct* addDebugExtent(const CExtent& extent,double rotationDegrees,const char* format,...);
   static DataStruct* addDebugRegion(const CExtendedRgn& region,const char* format,...);

   static int getNextDebugPolygonId() { return m_nextDebugPolygonId; }
   static int getNextDebugPolyId()    { return m_nextDebugPolyId;    }
   static int getNextDebugPointId()   { return m_nextDebugPointId;   }
   static int getNextDebugExtentId()  { return m_nextDebugExtentId;  }
   static int getNextDebugRegionId()  { return m_nextDebugRegionId;  }

   static void setAddEnable(bool flag) { m_addEnable = flag; }

   static void writefTimeStampFile(const char* filePath,const char* format,...);
   static void writefTimeStamp(const char* format,...);

private:
   static void addIdAttribute(CCEtoODBDoc& camCadDoc,DataStruct& data,int& id);
};

//_____________________________________________________________________________
template<class TYPE>
class CMemoryTracker
{
private:
   //static CMemoryTracker<TYPE>* m_memoryTracker;

   CTypedMapPtrToIntContainer<TYPE*> m_memoryMap;

public:
   CMemoryTracker(int blockSize=1000);

   void empty();
   virtual int incrementReference(TYPE* data);
   int decrementReference(TYPE* data);
   bool contains(TYPE* data);
   bool isValid();

   //static CMemoryTracker<TYPE>& getMemoryTracker();
   //static void releaseMemoryTracker();
};

//_____________________________________________________________________________
//template<class TYPE>
//CMemoryTracker<TYPE>* CDataStructTracker<TYPE>::m_dataStructTracker = NULL;
//
//template<class TYPE>
//CMemoryTracker<TYPE>& CMemoryTracker<TYPE>::getDataStructTracker()
//{
//   if (m_dataStructTracker<TYPE> == NULL)
//   {
//      m_dataStructTracker<TYPE> = new CMemoryTracker<TYPE>();
//   }
//
//   return *m_dataStructTracker<TYPE>;
//}
//
//void CMemoryTracker<TYPE>::releaseDataStructTracker()
//{
//   delete m_dataStructTracker<TYPE>;
//   m_dataStructTracker<TYPE> = NULL;
//}

template<class TYPE>
CMemoryTracker<TYPE>::CMemoryTracker(int blockSize)
: m_memoryMap(blockSize,false)
{
   m_memoryMap.InitHashTable(blockSize);
}

template<class TYPE>
void CMemoryTracker<TYPE>::empty()
{
   m_memoryMap.empty();
}

template<class TYPE>
int CMemoryTracker<TYPE>::incrementReference(TYPE* element)
{
   int referenceCount = 0;

   if (element != NULL)
   {
      m_memoryMap.Lookup(element,referenceCount);

      referenceCount++;

      m_memoryMap.SetAt(element,referenceCount);
   }

   return referenceCount;
}

template<class TYPE>
int CMemoryTracker<TYPE>::decrementReference(TYPE* element)
{
   int referenceCount = 0;

   if (element != NULL)
   {
      m_memoryMap.Lookup(element,referenceCount);

      referenceCount--;

      if (referenceCount > 0)
      {
         m_memoryMap.SetAt(element,referenceCount);
      }
      else
      {
         m_memoryMap.RemoveKey(element);
      }
   }

   return referenceCount;
}

template<class TYPE>
bool CMemoryTracker<TYPE>::contains(TYPE* element)
{
   int referenceCount = 0;

   bool retval = (m_memoryMap.Lookup(element,referenceCount) != 0);

   return retval;
}

template<class TYPE>
bool CMemoryTracker<TYPE>::isValid()
{
   return true;
}

class SelectStruct;

CMemoryTracker<SelectStruct>& getSelectStructTracker();
void releaseSelectStructTracker();
CMemoryTracker<DataStruct>& getDataStructTracker();
void releaseDataStructTracker();

////_____________________________________________________________________________
//class CDataStructTracker
//{
//private:
//   static CDataStructTracker* m_dataStructTracker;
//
//   CTypedMapPtrToIntContainer<DataStruct*> m_dataStructMap;
//
//public:
//   CDataStructTracker();
//
//   void empty();
//   int incrementReference(DataStruct* data);
//   int decrementReference(DataStruct* data);
//   bool contains(DataStruct* data);
//   bool isValid();
//
//   static CDataStructTracker& getDataStructTracker();
//   static void releaseDataStructTracker();
//};

#endif
