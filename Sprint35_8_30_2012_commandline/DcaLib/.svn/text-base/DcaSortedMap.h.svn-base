// $Header: /CAMCAD/DcaLib/DcaSortedMap.h 1     3/09/07 5:10p Kurt Van Ness $

#if !defined(__DcaSortedMap_h__)
#define __DcaSortedMap_h__

#pragma once


//=============================================================================

/*
History: RwLib.h $
 * 
 * *****************  Version 8  *****************
 * User: Kvanness     Date: 2/06/00    Time: 11:50a
 * Updated in $/LibKnvPP
 * Added empty(), swapSeniority(), and next()
 * 
 * *****************  Version 7  *****************
 * User: Kurtv        Date: 11/19/98   Time: 10:24a
 * Updated in $/include
 * Change BOOL to bool, where appropriate.
 * 
 * *****************  Version 6  *****************
 * User: Kurtv        Date: 4/22/98    Time: 11:52a
 * Updated in $/include
 * 
 * *****************  Version 5  *****************
 * User: Kurtv        Date: 7/19/97    Time: 8:27p
 * Updated in $/include
 * Added isContainer logic and index parameters to rewind() and next()
 * 
 * *****************  Version 4  *****************
 * User: Kurtv        Date: 3/05/97    Time: 4:11a
 * Updated in $/include
 * Added variable sort function feature
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 2/27/97    Time: 5:27p
 * Updated in $/include
 * Changes for longer file names
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 1/25/96    Time: 9:11p
 * Updated in $/Include
*/

#if !defined(__MapSortStringToOb_H__)
#define __MapSortStringToOb_H__

#include <afxwin.h>

/*
#ifndef __AFX_H__
#include "afx.h"
#endif
*/

//_____________________________________________________________________________
class CPObject : public CObject
{
   //DECLARE_DYNAMIC(CPObject);

public:
   CObject* m_object;
   int m_seniority;
   
public:
   CPObject(int seniority,CObject* object);
   ~CPObject();
   
};

//_____________________________________________________________________________
typedef struct sElement
{
   CString* pString;
   CPObject* pObject;
} SElement;

//_____________________________________________________________________________
class CMapSortStringToOb : public CMapStringToOb 
{
   //DECLARE_DYNAMIC(CMapSortStringToOb);

//   DECLARE_SERIAL(CMapSortStringToOb)
private:
   bool m_isContainer;
   bool m_isSorted;
   int m_nextSeniority;
   int m_sortPos;
   int m_index;
   int m_sortSize;
   SElement* m_hElement;
   int (*m_sortFunction)(const void* elem1,const void* elem2);

protected:
/*
   // Association
   struct CAssoc
   {
      CAssoc* pNext;
      UINT nHashValue;  // needed for efficient iteration
      CString key;
      CObject* value;
   };
*/

public:

// Construction
   CMapSortStringToOb(int nBlockSize=10,bool isContainer=true);
   virtual ~CMapSortStringToOb();
   virtual void empty();
   
// Attributes
   // number of elements
   virtual int GetCount() const
      { return CMapStringToOb::GetCount(); }

   virtual BOOL IsEmpty() const
      { return CMapStringToOb::IsEmpty(); }

   virtual bool IsSorted() const   // new function
      { return m_isSorted; }

   // Lookup
   virtual BOOL Lookup(const char* key, CObject*& rValue) const;

// Operations
   // Lookup and add if not there
   //??CObject*& operator[](const char* key);

   // add a new (key, value) pair
   virtual void SetAt(const char* key, CObject* newValue);

   // removing existing (key, ?) pair
   virtual BOOL RemoveKey(const char* key);

   virtual void RemoveAll();
   virtual void DeleteAll();

   // iterating all (key, value) pairs
   virtual POSITION GetStartPosition() const
      { return CMapStringToOb::GetStartPosition(); }

   virtual void GetNextAssoc(POSITION& rNextPosition, CString& rKey, CObject*& rValue) const;
      
   // new functionality
   virtual void rewind() { rewind(m_index); }
   virtual bool next(CObject*& object) { return next(object,m_index); }

   virtual void Sort();
   virtual void GetFirstSorted(CString*& rKey, CObject*& rValue);
   virtual void GetNextSorted(CString*& rKey, CObject*& rValue);
   virtual void GetSorted(CString*& rKey, CObject*& rValue);
   virtual void rewind(int& index);
   virtual bool next(CObject*& object,int& index);
   virtual bool next(CObject*& object,CString& key,int& index);
   virtual bool swapSeniority(const char* key0,const char* key1);
   virtual void setSortBySeniority(bool seniority=true); 
   virtual void setSortFunction(int (*sortFunction)(const void* elem1,const void* elem2));

   // advanced features for derived classes
   //UINT GetHashTableSize() const;
   //void InitHashTable(UINT hashSize);

// Overridables: special non-virtual (see map implementation for details)
   // Routine used to user-provided hash keys
   //UINT HashKey(const char* key) const;

// Implementation
private:
   void destroyElements();
/*
   CAssoc** m_pHashTable;
   UINT m_nHashTableSize;
   int m_nCount;
   CAssoc* m_pFreeList;
   struct CPlex* m_pBlocks;
   int m_nBlockSize;

   CAssoc* NewAssoc();
   void FreeAssoc(CAssoc*);
   CAssoc* GetAssocAt(const char*, UINT&) const;
*/

public:

//   void Serialize(CArchive&);
#ifdef _DEBUG
   void Dump(CDumpContext& dc) const;
   void AssertValid() const
      { CMapStringToOb::AssertValid(); }  
   
#endif
};

//_____________________________________________________________________________
template<class VALUE>
class CTypedMapSortStringToObContainer : public CMapSortStringToOb
{
public:
   CTypedMapSortStringToObContainer(int size=101,bool isContainer=true) :
      CMapSortStringToOb(size,isContainer){}
   virtual ~CTypedMapSortStringToObContainer(){}
   //virtual void empty() { CMapSortStringToOb::empty(); }

   //virtual int GetCount()  const { return CMapSortStringToOb::GetCount(); }
   //virtual BOOL IsEmpty()  const { return CMapSortStringToOb::IsEmpty();  }
   //virtual bool IsSorted() const { return CMapSortStringToOb::IsSorted(); }
   //virtual BOOL RemoveKey(const char* key) { return CMapSortStringToOb::RemoveKey(key); }
   //virtual void RemoveAll() { CMapSortStringToOb::RemoveAll(); }
   //virtual void DeleteAll() { CMapSortStringToOb::DeleteAll(); }
   //virtual POSITION GetStartPosition() const { return CMapSortStringToOb::GetStartPosition(); }
   //virtual void rewind() { CMapSortStringToOb::rewind(); }
   //virtual void Sort() { CMapSortStringToOb::Sort(); }
   //virtual void rewind(int& index) { CMapSortStringToOb::rewind(index); }
   //virtual bool swapSeniority(const char* key0,const char* key1) { return CMapSortStringToOb::swapSeniority(key0,key1); }
   //virtual void setSortBySeniority(bool seniority=true) { CMapSortStringToOb::setSortBySeniority(seniority); }
   //virtual void setSortFunction(int (*sortFunction)(const void* elem1,const void* elem2)) { CMapSortStringToOb::setSortFunction(sortFunction); }

   virtual BOOL Lookup(const char* key,VALUE*& rValue) const 
      { return CMapSortStringToOb::Lookup(key,(CObject*&)rValue); }

   virtual void SetAt(const char* key,VALUE* newValue)
      { CMapSortStringToOb::SetAt(key,(CObject*)newValue); }

   virtual void GetNextAssoc(POSITION& rNextPosition, CString& rKey, VALUE*& rValue) const
      { CMapSortStringToOb::GetNextAssoc(rNextPosition,rKey,(CObject*&)rValue); }

   virtual bool next(VALUE*& object)
      { return CMapSortStringToOb::next((CObject*&)object); }

   virtual void GetFirstSorted(CString*& rKey,VALUE*& rValue)
      { CMapSortStringToOb::GetFirstSorted(rKey,(CObject*&)rValue); }

   virtual void GetNextSorted(CString*& rKey,VALUE*& rValue)
      { CMapSortStringToOb::GetNextSorted(rKey,(CObject*&)rValue); }

   virtual void GetSorted(CString*& rKey,VALUE*& rValue)
      { CMapSortStringToOb::GetSorted(rKey,(CObject*&)rValue); }

   virtual bool next(VALUE*& object,int& index)
      { return CMapSortStringToOb::next((CObject*&)object,index); }

   virtual bool next(VALUE*& object,CString& key,int& index)
      { return CMapSortStringToOb::next((CObject*&)object,key,index); }

};

#endif  

//=============================================================================

/*
History: RwLib.h $
 * 
 * *****************  Version 2  *****************
 * User: Kvanness     Date: 2/06/00    Time: 11:48a
 * Updated in $/LibKnvPP
 * Added next() with key parameter
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 5/03/99    Time: 6:50a
 * Created in $/libknvpp
 * Initial add.
*/

#if !defined(__MapSortedStringToOb_H__)
#define __MapSortedStringToOb_H__

//#include "MapSortStringToOb.h"

//_____________________________________________________________________________
template<class VALUE>
class CMapSortedStringToOb : public CMapSortStringToOb
{
public:

// Construction
   CMapSortedStringToOb(int nBlockSize=10,bool isContainer=true) : CMapSortStringToOb(nBlockSize,isContainer) { };
   
// Attributes
   // number of elements
   virtual int GetCount() const
      { return CMapSortStringToOb::GetCount(); }

   virtual BOOL IsEmpty() const
      { return CMapSortStringToOb::IsEmpty(); }

   virtual bool IsSorted() const   
      { return CMapSortStringToOb::IsSorted(); }

// Operations
   // Lookup
   virtual BOOL Lookup(const char* key, VALUE*& rValue) const
      { return CMapSortStringToOb::Lookup(key,(CObject*&)rValue); }

   // add a new (key, value) pair
   virtual void SetAt(const char* key, VALUE* newValue)
      { CMapSortStringToOb::SetAt(key,newValue); }
   //virtual VALUE*& operator [](const char* key);

   // removing existing (key, ?) pair
   virtual BOOL RemoveKey(const char* key)
      { return CMapSortStringToOb::RemoveKey(key); }

   virtual void RemoveAll()
      { CMapSortStringToOb::RemoveAll(); }

   virtual void DeleteAll()
      { CMapSortStringToOb::DeleteAll(); }


   // iterating all (key, value) pairs
   virtual POSITION GetStartPosition() const
      { return CMapSortStringToOb::GetStartPosition(); }

   virtual void GetNextAssoc(POSITION& rNextPosition, CString& rKey, VALUE*& rValue) const
      { CMapSortStringToOb::GetNextAssoc(rNextPosition,rKey,(CObject*&)rValue); }
      
   // new functionality
   virtual void rewind() { CMapSortStringToOb::rewind(); }
   virtual bool next(VALUE*& object) { return CMapSortStringToOb::next((CObject*&)object); }

   void Sort() { CMapSortStringToOb::Sort(); }

   virtual void GetFirstSorted(CString*& rKey, VALUE*& rValue)
      { CMapSortStringToOb::GetFirstSorted(rKey,(CObject*&)rValue); }

   virtual void GetNextSorted(CString*& rKey, VALUE*& rValue)
      { CMapSortStringToOb::GetNextSorted(rKey,(CObject*&)rValue); }

   virtual void GetSorted(CString*& rKey, VALUE*& rValue)
      { CMapSortStringToOb::GetSorted(rKey,(CObject*&)rValue); }

   virtual void rewind(int& index)
      { CMapSortStringToOb::rewind(index); }

   virtual bool next(VALUE*& object,int& index)
      { return CMapSortStringToOb::next((CObject*&)object,index); }

   virtual bool next(VALUE*& object,CString& key,int& index)
      { return CMapSortStringToOb::next((CObject*&)object,key,index); }

   void setSortBySeniority(bool seniority=true)
      { CMapSortStringToOb::setSortBySeniority(seniority); }

   void setSortFunction(int (*sortFunction)(const void* elem1,const void* elem2))
      { CMapSortStringToOb::setSortFunction(sortFunction); }

#ifdef _DEBUG
public:
   void Dump(CDumpContext& dc) const
      { CMapStringToOb::Dump(dc); }  

   void AssertValid() const
      { CMapStringToOb::AssertValid(); }  
   
#endif
};

#endif  

#endif
