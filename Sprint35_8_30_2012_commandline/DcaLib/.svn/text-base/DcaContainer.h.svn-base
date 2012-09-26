// $Header: /CAMCAD/DcaLib/DcaContainer.h 4     6/30/07 2:57a Kurt Van Ness $

#if !defined(__DcaContainer_h__)
#define __DcaContainer_h__

#pragma once

// todo - knv - replace with new implementations of common containers

/*
CTypedObArrayContainer
CTypedPtrArrayContainer
CTypedArrayContainer

CTypedObListContainer
CTypedPtrListContainer
CTypedListContainer

CTypedMapStringToObContainer
CTypedMapStringToPtrContainer
CTypedMapWordToObContainer
CTypedMapWordToPtrContainer
CTypedMapPtrToPtrContainer
CTypedMapDwordToPtrContainer
CTypedMapIntToPtrContainer
CTypedMapContainer

CTypedObArrayWithMap
CTypedPtrArrayWithMap
CTypedObArrayWithMapContainer
CTypedPtrArrayWithMapContainer
CTypedArrayWithMap
*/

#include "DcaLib.h"
#include <afxtempl.h>

//=============================================================================
template<class ARRAY_BASE_CLASS,class MAP_BASE_CLASS,class VALUE>
class CTypedArrayWithMap : public CObject
{
private:
   bool m_isContainer;
   int m_size;

   CTypedPtrArray<ARRAY_BASE_CLASS,VALUE*> m_array;
   mutable CTypedPtrMap<MAP_BASE_CLASS,CString,VALUE*>* m_map;

public:
   CTypedArrayWithMap(int size=100,bool isContainer=false);
   virtual ~CTypedArrayWithMap();
   virtual void empty();

   virtual int add(const CString& key,VALUE* value);
   virtual void setAt(int index,const CString& key,VALUE* value);
   virtual VALUE* getAt(int index) const;
   virtual VALUE* getAt(const CString& key) const;
   virtual bool lookup(const CString& key,VALUE*& value) const;
   int getSize() const { return m_array.GetSize(); }
   bool isContainer() const { return m_isContainer; }

private:
   CTypedPtrMap<MAP_BASE_CLASS,CString,VALUE*>& getMap() const;
   void setSize();
   void initHashTableSize();
   void rehash();
};

//_____________________________________________________________________________
template<class VALUE>
class CTypedObArrayWithMap : public CTypedArrayWithMap<CObArray,CMapStringToOb,VALUE>
{
public:
   CTypedObArrayWithMap(int size=100,bool isContainer=false) : 
      CTypedArrayWithMap<CObArray,CMapStringToOb,VALUE>(size,isContainer)
      {
      }
};

//_____________________________________________________________________________
template<class VALUE>
class CTypedPtrArrayWithMap : public CTypedArrayWithMap<CPtrArray,CMapStringToPtr,VALUE>
{
public:
   CTypedPtrArrayWithMap(int size=100,bool isContainer=false) : 
      CTypedArrayWithMap<CPtrArray,CMapStringToPtr,VALUE>(size,isContainer)
      {
      }
};

//_____________________________________________________________________________
template<class VALUE>
class CTypedObArrayWithMapContainer : public CTypedArrayWithMap<CObArray,CMapStringToOb,VALUE>
{
public:
   CTypedObArrayWithMapContainer(int size=100,bool isContainer=true) : 
      CTypedArrayWithMap<CObArray,CMapStringToOb,VALUE>(size,isContainer)
      {
      }
};

//_____________________________________________________________________________
template<class VALUE>
class CTypedPtrArrayWithMapContainer : public CTypedArrayWithMap<CPtrArray,CMapStringToPtr,VALUE>
{
public:
   CTypedPtrArrayWithMapContainer(int size=100,bool isContainer=true) : 
      CTypedArrayWithMap<CPtrArray,CMapStringToPtr,VALUE>(size,isContainer)
      {
      }
};

//_____________________________________________________________________________
template<class ARRAY_BASE_CLASS,class MAP_BASE_CLASS,class VALUE>
CTypedArrayWithMap<ARRAY_BASE_CLASS,MAP_BASE_CLASS,VALUE>::CTypedArrayWithMap(int size,bool isContainer)
{
   m_isContainer = isContainer;
   m_size        = size;
   m_map         = NULL;

   if (m_size < 10) m_size = 10;

   setSize();
}

template<class ARRAY_BASE_CLASS,class MAP_BASE_CLASS,class VALUE>
CTypedArrayWithMap<ARRAY_BASE_CLASS,MAP_BASE_CLASS,VALUE>::~CTypedArrayWithMap()
{
   m_size = 10;

   empty();

   delete m_map;
}

template<class ARRAY_BASE_CLASS,class MAP_BASE_CLASS,class VALUE>
void CTypedArrayWithMap<ARRAY_BASE_CLASS,MAP_BASE_CLASS,VALUE>::empty()
{
   if (m_isContainer)
   {
      for (int ind=0;ind < m_array.GetSize();ind++)
      {
         VALUE* obj = m_array.GetAt(ind);
         delete obj;
      }
   }

   m_array.RemoveAll();

   if (m_map != NULL)
   {
      m_map->RemoveAll();
      delete m_map;
      m_map = NULL;
   }

   setSize();
}

template<class ARRAY_BASE_CLASS,class MAP_BASE_CLASS,class VALUE>
CTypedPtrMap<MAP_BASE_CLASS,CString,VALUE*>& CTypedArrayWithMap<ARRAY_BASE_CLASS,MAP_BASE_CLASS,VALUE>::getMap() const
{
   if (m_map == NULL)
   {
      m_map = new CTypedPtrMap<MAP_BASE_CLASS,CString,VALUE*>();
   }

   return *m_map;
}

template<class ARRAY_BASE_CLASS,class MAP_BASE_CLASS,class VALUE>
void CTypedArrayWithMap<ARRAY_BASE_CLASS,MAP_BASE_CLASS,VALUE>::setSize()
{
   m_array.SetSize(0,m_size);
   initHashTableSize();
}

template<class ARRAY_BASE_CLASS,class MAP_BASE_CLASS,class VALUE>
void CTypedArrayWithMap<ARRAY_BASE_CLASS,MAP_BASE_CLASS,VALUE>::initHashTableSize()
{
   getMap().InitHashTable((int)(nextPrime2n((int)(max(m_size,m_array.GetSize())/.8))),true);
}

template<class ARRAY_BASE_CLASS,class MAP_BASE_CLASS,class VALUE>
void CTypedArrayWithMap<ARRAY_BASE_CLASS,MAP_BASE_CLASS,VALUE>::rehash()
{
   if (m_map != NULL)
   {
      CTypedPtrMap<MAP_BASE_CLASS,CString,VALUE*>* oldMap = m_map;
      m_map = new CTypedPtrMap<MAP_BASE_CLASS,CString,VALUE*>();

      initHashTableSize();
      CString key;
      VALUE* obj;

      for (POSITION pos = oldMap->GetStartPosition();pos != NULL;)
      {
         oldMap->GetNextAssoc(pos,key,obj);
         m_map->SetAt(key,obj);
      }

      oldMap->RemoveAll();
      delete oldMap;
   }
}

template<class ARRAY_BASE_CLASS,class MAP_BASE_CLASS,class VALUE>
int CTypedArrayWithMap<ARRAY_BASE_CLASS,MAP_BASE_CLASS,VALUE>::add(const CString& key,VALUE* value)
{
   int retval = (int)m_array.Add(value);
   getMap().SetAt(key,value);

   if (m_array.GetSize() >= (INT_PTR)(getMap().GetHashTableSize()))
   {
      rehash();
   }

   return retval;
}

template<class ARRAY_BASE_CLASS,class MAP_BASE_CLASS,class VALUE>
void CTypedArrayWithMap<ARRAY_BASE_CLASS,MAP_BASE_CLASS,VALUE>::setAt(int index,const CString& key,VALUE* value)
{
   if (index < m_array.GetSize())
   {
      VALUE* obj = m_array.GetAt(index);

      if (obj != NULL)
      {
         CString existingKey;
         VALUE* existingObj;

         for (POSITION pos = getMap().GetStartPosition();pos != NULL;)
         {
            getMap().GetNextAssoc(pos,existingKey,existingObj);

            if (existingObj == obj)
            {
               getMap().RemoveKey(existingKey);
            }
         }
      }

      if (m_isContainer)
      {
         delete obj;
      }
   }

   m_array.SetAtGrow(index,value);
   getMap().SetAt(key,value);

   if (m_array.GetSize() >= (INT_PTR)(getMap().GetHashTableSize()))
   {
      rehash();
   }
}

template<class ARRAY_BASE_CLASS,class MAP_BASE_CLASS,class VALUE>
bool CTypedArrayWithMap<ARRAY_BASE_CLASS,MAP_BASE_CLASS,VALUE>::lookup(const CString& key,VALUE*& value) const
{
   bool retval = (getMap().Lookup(key,value) != 0);
   //bool retval = true; UINT INT_PTR

   return retval;
}

template<class ARRAY_BASE_CLASS,class MAP_BASE_CLASS,class VALUE>
VALUE* CTypedArrayWithMap<ARRAY_BASE_CLASS,MAP_BASE_CLASS,VALUE>::getAt(int index) const
{
   VALUE* obj = NULL;

   if (index >= 0 && index < m_array.GetSize())
   {
      obj = m_array.GetAt(index);
   }

   return obj;
}

template<class ARRAY_BASE_CLASS,class MAP_BASE_CLASS,class VALUE>
VALUE* CTypedArrayWithMap<ARRAY_BASE_CLASS,MAP_BASE_CLASS,VALUE>::getAt(const CString& key) const
{
   VALUE* obj = NULL;

   if (! getMap().Lookup(key,obj))
   {
      obj = NULL;
   }

   return obj;
}


//=============================================================================
template<class MAP_BASE_CLASS,class ARRAY_BASE_CLASS,class VALUE>
class CTypedMapWithArrayContainer : public CObject
{
private:
   bool m_isContainer;
   int m_hashTableSize;

   mutable CTypedPtrMap<MAP_BASE_CLASS,CString,VALUE*>* m_map;
   CTypedPtrArray<ARRAY_BASE_CLASS,VALUE*> m_array;

public:
   CTypedMapWithArrayContainer(int hashTableSize=101,bool isContainer=true);
   virtual ~CTypedMapWithArrayContainer();
   virtual void empty();

   virtual int setAt(const CString& key,VALUE* value);
   virtual VALUE* getAt(int index) const;
   virtual VALUE* getAt(const CString& key) const;
   virtual bool lookup(const CString& key,VALUE*& value) const;
   int getCount() const { return getMap().GetCount(); }
   int getHashTableSize() const { return getMap().GetHashTableSize(); }

private:
   CTypedPtrMap<MAP_BASE_CLASS,CString,VALUE*>& getMap() const;
   void rehash();
};

//_____________________________________________________________________________
template<class VALUE>
class CTypedObMapWithArrayContainer : public CTypedMapWithArrayContainer<CMapStringToOb,CObArray,VALUE>
{
public:
   CTypedObMapWithArrayContainer(int hashTableSize=101,bool isContainer=true) : 
      CTypedMapWithArrayContainer<CMapStringToOb,CObArray,VALUE>(hashTableSize,isContainer)
      {
      }
};

//_____________________________________________________________________________
template<class VALUE>
class CTypedPtrMapWithArrayContainer : public CTypedMapWithArrayContainer<CMapStringToPtr,CPtrArray,VALUE>
{
public:
   CTypedPtrMapWithArrayContainer(int hashTableSize=101,bool isContainer=true) : 
      CTypedMapWithArrayContainer<CMapStringToPtr,CPtrArray,VALUE>(hashTableSize,isContainer)
      {
      }
};

//_____________________________________________________________________________
template<class MAP_BASE_CLASS,class ARRAY_BASE_CLASS,class VALUE>
CTypedMapWithArrayContainer<MAP_BASE_CLASS,ARRAY_BASE_CLASS,VALUE>::CTypedMapWithArrayContainer(int hashTableSize,bool isContainer)
{
   m_isContainer   = isContainer;
   m_hashTableSize = hashTableSize;
   m_map           = NULL;

   if (m_hashTableSize < 17) m_hashTableSize = 17;

   m_array.SetSize(0,m_hashTableSize);
}

template<class MAP_BASE_CLASS,class ARRAY_BASE_CLASS,class VALUE>
CTypedMapWithArrayContainer<MAP_BASE_CLASS,ARRAY_BASE_CLASS,VALUE>::~CTypedMapWithArrayContainer()
{
   empty();
}

template<class MAP_BASE_CLASS,class ARRAY_BASE_CLASS,class VALUE>
void CTypedMapWithArrayContainer<MAP_BASE_CLASS,ARRAY_BASE_CLASS,VALUE>::empty()
{
   if (m_isContainer)
   {
      for (int ind=0;ind < m_array.GetSize();ind++)
      {
         VALUE* obj = m_array.GetAt(ind);
         delete obj;
      }
   }

   m_array.SetSize(0,m_hashTableSize);

   if (m_map != NULL)
   {
      m_map->RemoveAll();
      delete m_map;
      m_map = NULL;
   }
}

template<class MAP_BASE_CLASS,class ARRAY_BASE_CLASS,class VALUE>
CTypedPtrMap<MAP_BASE_CLASS,CString,VALUE*>& CTypedMapWithArrayContainer<MAP_BASE_CLASS,ARRAY_BASE_CLASS,VALUE>::getMap() const
{
   if (m_map == NULL)
   {
      m_map = new CTypedPtrMap<MAP_BASE_CLASS,CString,VALUE*>();
      m_map->InitHashTable(m_hashTableSize);
   }

   return *m_map;
}

template<class MAP_BASE_CLASS,class ARRAY_BASE_CLASS,class VALUE>
void CTypedMapWithArrayContainer<MAP_BASE_CLASS,ARRAY_BASE_CLASS,VALUE>::rehash()
{
   if (m_map != NULL)
   {
      int newHashTableSize = nextPrime2n(m_map->GetCount() - 1);

      if (newHashTableSize < 17) newHashTableSize = 17;

      CTypedPtrMap<MAP_BASE_CLASS,CString,VALUE*>* newMap = new CTypedPtrMap<MAP_BASE_CLASS,CString,VALUE*>();
      newMap->InitHashTable(newHashTableSize);

      CString key;
      VALUE* obj;

      for (POSITION pos = m_map->GetStartPosition();pos != NULL;)
      {
         m_map->GetNextAssoc(pos,key,obj);
         newMap->SetAt(key,obj);
      }

      delete m_map;
      m_map = newMap;

      if (m_array.GetSize() < newHashTableSize)
      {
         m_array.SetSize(m_array.GetSize(),newHashTableSize - m_array.GetSize());
      }
   }
}

template<class MAP_BASE_CLASS,class ARRAY_BASE_CLASS,class VALUE>
int CTypedMapWithArrayContainer<MAP_BASE_CLASS,ARRAY_BASE_CLASS,VALUE>::setAt(const CString& key,VALUE* value)
{
   int retval = -1;
   VALUE* oldValue = getAt(key);

   if (oldValue != NULL)
   {
      if (m_isContainer)
      {
         delete oldValue;
      }
   }
   else
   {
      retval = m_array.Add(value);
   }

   getMap().SetAt(key,value);

   if (m_array.GetSize() >= (INT_PTR)(getMap().GetHashTableSize()))
   {
      rehash();
   }

   return retval;
}

template<class MAP_BASE_CLASS,class ARRAY_BASE_CLASS,class VALUE>
bool CTypedMapWithArrayContainer<MAP_BASE_CLASS,ARRAY_BASE_CLASS,VALUE>::lookup(const CString& key,VALUE*& value) const
{
   bool retval = (getMap().Lookup(key,value) != 0);

   return retval;
}

template<class MAP_BASE_CLASS,class ARRAY_BASE_CLASS,class VALUE>
VALUE* CTypedMapWithArrayContainer<MAP_BASE_CLASS,ARRAY_BASE_CLASS,VALUE>::getAt(int index) const
{
   VALUE* obj = NULL;

   if (index >= 0 && index < m_array.GetSize())
   {
      obj = m_array.GetAt(index);
   }

   return obj;
}

template<class MAP_BASE_CLASS,class ARRAY_BASE_CLASS,class VALUE>
VALUE* CTypedMapWithArrayContainer<MAP_BASE_CLASS,ARRAY_BASE_CLASS,VALUE>::getAt(const CString& key) const
{
   VALUE* obj = NULL;

   if (! getMap().Lookup(key,obj))
   {
      obj = NULL;
   }

   return obj;
}

//=============================================================================
template<class ARRAY_BASE_CLASS,class VALUE>
class CTypedArrayContainer : public CTypedPtrArray<ARRAY_BASE_CLASS,VALUE>
{
private:
   bool m_isContainer;
   int m_growBySize;
   bool m_isSorted;
   int (*m_sortFunction)(const void* elem1,const void* elem2);

public:
   CTypedArrayContainer(int growBySize=10,bool isContainer=true);
   virtual ~CTypedArrayContainer();
   virtual void empty();

   virtual bool isContainer() { return m_isContainer; }
   virtual void setGrowBySize(int growBySize) { m_growBySize = growBySize;  SetSize(GetSize(),m_growBySize); }
   virtual int getGrowBySize() const { return m_growBySize; }

   virtual void setAt(INT_PTR index,VALUE element);
   virtual void setAtGrow(INT_PTR index,VALUE element);

   virtual void sort();
   virtual void setSortFunction(int (*sortFunction)(const void* elem1,const void* elem2));
};

//_____________________________________________________________________________
template<class VALUE>
class CTypedObArrayContainer : public CTypedArrayContainer<CObArray,VALUE>
{
public:
   CTypedObArrayContainer(int growBySize=10,bool isContainer=true) : 
      CTypedArrayContainer<CObArray,VALUE>(growBySize,isContainer)
      {
      }
};

//_____________________________________________________________________________
template<class VALUE>
class CTypedPtrArrayContainer : public CTypedArrayContainer<CPtrArray,VALUE>
{
public:
   CTypedPtrArrayContainer(int growBySize=10,bool isContainer=true) : 
      CTypedArrayContainer<CPtrArray,VALUE>(growBySize,isContainer)
      {
      }
};

//_____________________________________________________________________________
template<class ARRAY_BASE_CLASS,class VALUE>
CTypedArrayContainer<ARRAY_BASE_CLASS,VALUE>::CTypedArrayContainer(int growBySize,bool isContainer) :
m_growBySize(growBySize), m_isContainer(isContainer), m_isSorted(false), m_sortFunction(NULL)
{
}

template<class ARRAY_BASE_CLASS,class VALUE>
CTypedArrayContainer<ARRAY_BASE_CLASS,VALUE>::~CTypedArrayContainer()
{
   empty();
}

template<class ARRAY_BASE_CLASS,class VALUE>
void CTypedArrayContainer<ARRAY_BASE_CLASS,VALUE>::empty()
{
   if (m_isContainer)
   {
      for (int ind=0;ind < GetSize();ind++)
      {
         VALUE obj = GetAt(ind);
         delete obj;
      }
   }

   RemoveAll();
   m_isSorted = false;
}

template<class ARRAY_BASE_CLASS,class VALUE>
void CTypedArrayContainer<ARRAY_BASE_CLASS,VALUE>::setAt(INT_PTR index,VALUE element)
{
   if (m_isContainer && index < GetSize())
   {
      delete GetAt(index);
   }

   SetAt(index,element);
   m_isSorted = false;
}

template<class ARRAY_BASE_CLASS,class VALUE>
void CTypedArrayContainer<ARRAY_BASE_CLASS,VALUE>::setAtGrow(INT_PTR index,VALUE element)
{
   if (m_isContainer && index < GetSize())
   {
      delete GetAt(index);
   }

   SetAtGrow(index,element);
}

template<class ARRAY_BASE_CLASS,class VALUE>
void CTypedArrayContainer<ARRAY_BASE_CLASS,VALUE>::sort()
{
   if (!m_isSorted && m_sortFunction != NULL)
   {
      int elementCount = (int)GetSize();

      if (elementCount > 1)
      {
         VALUE* array = new VALUE[elementCount];

         for (int ind=0;ind < elementCount;ind++)
         {
            VALUE obj = GetAt(ind);
            array[ind] = obj;
         }

         qsort((void*)array,(size_t)elementCount,sizeof(VALUE),m_sortFunction);

         for (int ind=0;ind < elementCount;ind++)
         {
            VALUE obj = array[ind];
            SetAt(ind,obj);
         }

         delete [] array;
      }

      m_isSorted = true;
   }
}

template<class ARRAY_BASE_CLASS,class VALUE>
void CTypedArrayContainer<ARRAY_BASE_CLASS,VALUE>::setSortFunction(int (*sortFunction)(const void* elem1,const void* elem2))
{
   m_sortFunction = sortFunction;
   m_isSorted = false;
}

//=============================================================================
template<class LIST_BASE_CLASS,class VALUE>
class CTypedListContainer : public CTypedPtrList<LIST_BASE_CLASS,VALUE>
{
private:
   bool m_isContainer;

public:
   CTypedListContainer(bool isContainer=true);
   virtual ~CTypedListContainer();
   virtual void empty();

   virtual bool isContainer() { return m_isContainer; }
};

//_____________________________________________________________________________
template<class VALUE>
class CTypedObListContainer : public CTypedListContainer<CObList,VALUE>
{
public:
   CTypedObListContainer(bool isContainer=true) : 
      CTypedListContainer<CObList,VALUE>(isContainer)
      {
      }
};

//_____________________________________________________________________________
template<class VALUE>
class CTypedPtrListContainer : public CTypedListContainer<CPtrList,VALUE>
{
public:
   CTypedPtrListContainer(bool isContainer=true) : 
      CTypedListContainer<CPtrList,VALUE>(isContainer)
      {
      }
};

//_____________________________________________________________________________
template<class LIST_BASE_CLASS,class VALUE>
CTypedListContainer<LIST_BASE_CLASS,VALUE>::CTypedListContainer(bool isContainer) :
m_isContainer(isContainer)
{
}

template<class LIST_BASE_CLASS,class VALUE>
CTypedListContainer<LIST_BASE_CLASS,VALUE>::~CTypedListContainer()
{
   empty();
}

template<class LIST_BASE_CLASS,class VALUE>
void CTypedListContainer<LIST_BASE_CLASS,VALUE>::empty()
{
   if (m_isContainer)
   {
      for (POSITION pos = GetHeadPosition();pos != NULL;)
      {
         VALUE obj = GetNext(pos);
         delete obj;
      }
   }

   RemoveAll();
}

//=============================================================================
template<class MAP_BASE_CLASS,class KEY,class VALUE>
class CTypedMapContainer : public CTypedPtrMap<MAP_BASE_CLASS,KEY,VALUE>
{
private:
   bool m_isContainer;

public:
   CTypedMapContainer(int blockSize=10,bool isContainer=true);
   virtual ~CTypedMapContainer();
   virtual void empty();

   virtual bool isContainer() { return m_isContainer; }

   virtual void setAt(KEY key,VALUE value)
   {
      if (m_isContainer)
      {
         VALUE oldValue;

         if (Lookup(key,oldValue))
         {
            delete oldValue;
         }
      }

      SetAt(key,value);
   }
};

//_____________________________________________________________________________
template<class VALUE>
class CTypedMapStringToObContainer : public CTypedMapContainer<CMapStringToOb,CString,VALUE>
{
public:
   CTypedMapStringToObContainer(int blockSize=10,bool isContainer=true) : 
      CTypedMapContainer<CMapStringToOb,CString,VALUE>(blockSize,isContainer)
      {
      }
};

//_____________________________________________________________________________
template<class VALUE>
class CTypedMapStringToPtrContainer : public CTypedMapContainer<CMapStringToPtr,CString,VALUE>
{
public:
   CTypedMapStringToPtrContainer(int blockSize=10,bool isContainer=true) : 
      CTypedMapContainer<CMapStringToPtr,CString,VALUE>(blockSize,isContainer)
      {
      }
};

//_____________________________________________________________________________
template<class VALUE>
class CTypedMapWordToObContainer : public CTypedMapContainer<CMapWordToOb,WORD,VALUE>
{
public:
   CTypedMapWordToObContainer(int blockSize=10,bool isContainer=true) : 
      CTypedMapContainer<CMapWordToOb,WORD,VALUE>(blockSize,isContainer)
      {
      }
};

//_____________________________________________________________________________
template<class VALUE>
class CTypedMapWordToPtrContainer : public CTypedMapContainer<CMapWordToPtr,WORD,VALUE>
{
public:
   CTypedMapWordToPtrContainer(int blockSize=10,bool isContainer=true) : 
      CTypedMapContainer<CMapWordToPtr,WORD,VALUE>(blockSize,isContainer)
      {
      }
};

//_____________________________________________________________________________
template<class KEY,class VALUE>
class CTypedMapPtrToPtrContainer
{
private:
   CTypedMapContainer<CMapPtrToPtr,void*,VALUE> m_map;

public:
   CTypedMapPtrToPtrContainer(int blockSize=10,bool isContainer=true) : 
      m_map(blockSize,isContainer)
      {
      }

      virtual void empty() { m_map.empty(); }

      // Attributes
      // number of elements
      INT_PTR GetCount() const { return m_map.GetCount(); }
      INT_PTR GetSize()  const { return m_map.GetSize(); }
      BOOL IsEmpty()     const { return m_map.IsEmpty(); }

      // Lookup
      BOOL Lookup(KEY key, VALUE& rValue) const
      { return m_map.Lookup((void*)key,rValue); }

      // Operations
      // Lookup and add if not there
      VALUE& operator[](KEY key)
      { return m_map[(void*)key]; }

      // add a new (key, value) pair
      void SetAt(KEY key,VALUE newValue)
      { m_map.SetAt((void*)key,newValue); }

      // removing existing (key, ?) pair
      bool RemoveKey(KEY key)
      { return (m_map.RemoveKey((void*)key) != 0); }

      void RemoveAll()
      { m_map.RemoveAll(); }

      // iterating all (key, value) pairs
      POSITION GetStartPosition() const
      { return m_map.GetStartPosition(); }

      void GetNextAssoc(POSITION& rNextPosition,KEY& rKey,VALUE& rValue) const
      { m_map.GetNextAssoc(rNextPosition,(void*&)rKey,rValue); }

      // advanced features for derived classes
      UINT GetHashTableSize() const
      { return m_map.GetHashTableSize(); }

      void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE)
      { m_map.InitHashTable(hashSize,bAllocNow); }

      // Overridables: special non-virtual (see map implementation for details)
      // Routine used to user-provided hash keys
      UINT HashKey(KEY key) const
      { return m_map.HashKey((void*)key); }
};

//_____________________________________________________________________________
template<class VALUE>
class CTypedMapDwordToPtrContainer : public CTypedMapPtrToPtrContainer<DWORD,VALUE>
{
public:
   CTypedMapDwordToPtrContainer(int blockSize=10,bool isContainer=true) : 
      CTypedMapPtrToPtrContainer<DWORD,VALUE>(blockSize,isContainer)
      {
      }
};

//_____________________________________________________________________________
template<class VALUE>
class CTypedMapIntToPtrContainer : public CTypedMapPtrToPtrContainer<int,VALUE>
{
public:
   CTypedMapIntToPtrContainer(int blockSize=10,bool isContainer=true) : 
      CTypedMapPtrToPtrContainer<int,VALUE>(blockSize,isContainer)
      {
      }
};

//_____________________________________________________________________________
template<class KEY>
class CTypedMapPtrToIntContainer
{
private:
   CTypedMapContainer<CMapPtrToPtr,KEY,void*> m_map;

public:
   CTypedMapPtrToIntContainer(int blockSize=10,bool isContainer=true) : 
      m_map(blockSize,isContainer)
      {
      }

      virtual void empty() { m_map.empty(); }

      // Attributes
      // number of elements
      INT_PTR GetCount() const { return m_map.GetCount(); }
      INT_PTR GetSize()  const { return m_map.GetSize(); }
      BOOL IsEmpty()     const { return m_map.IsEmpty(); }

      // Lookup
      BOOL Lookup(KEY key, int& rValue) const
      { return m_map.Lookup(key,(void*&)rValue); }

      // Operations
      // Lookup and add if not there
      int& operator[](KEY key)
      { return m_map[key]; }

      // add a new (key, value) pair
      void SetAt(KEY key,int newValue)
      { m_map.SetAt(key,(void*)newValue); }

      // removing existing (key, ?) pair
      bool RemoveKey(KEY key)
      { return (m_map.RemoveKey(key) != 0); }

      void RemoveAll()
      { m_map.RemoveAll(); }

      // iterating all (key, value) pairs
      POSITION GetStartPosition() const
      { return m_map.GetStartPosition(); }

      void GetNextAssoc(POSITION& rNextPosition,KEY& rKey,int& rValue) const
      { m_map.GetNextAssoc(rNextPosition,rKey,(void*&)rValue); }

      // advanced features for derived classes
      UINT GetHashTableSize() const
      { return m_map.GetHashTableSize(); }

      void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE)
      { m_map.InitHashTable(hashSize,bAllocNow); }

      // Overridables: special non-virtual (see map implementation for details)
      // Routine used to user-provided hash keys
      UINT HashKey(KEY key) const
      { return m_map.HashKey(key); }
};

//_____________________________________________________________________________
template<class MAP_BASE_CLASS,class KEY,class VALUE>
CTypedMapContainer<MAP_BASE_CLASS,KEY,VALUE>::CTypedMapContainer(int blockSize,bool isContainer) :
CTypedPtrMap<MAP_BASE_CLASS,KEY,VALUE>(blockSize),
m_isContainer(isContainer)
{
}

template<class MAP_BASE_CLASS,class KEY,class VALUE>
CTypedMapContainer<MAP_BASE_CLASS,KEY,VALUE>::~CTypedMapContainer()
{
   empty();
}

template<class MAP_BASE_CLASS,class KEY,class VALUE>
void CTypedMapContainer<MAP_BASE_CLASS,KEY,VALUE>::empty()
{
   if (m_isContainer)
   {
      VALUE obj;
      KEY key;

      for (POSITION pos = GetStartPosition();pos != NULL;)
      {
         GetNextAssoc(pos,key,obj);
         delete obj;
      }
   }

   RemoveAll();
}

#endif
