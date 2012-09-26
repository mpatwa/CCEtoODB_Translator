// $Header: /CAMCAD/DcaLib/DcaSortedMap.cpp 1     3/09/07 5:10p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaSortedMap.h"

//=============================================================================

/*
History: RwLib.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Kvanness     Date: 2/06/00    Time: 11:50a
 * Updated in $/LibKnvPP
 * Added empty(), swapSeniority(), and next()
 * 
 * *****************  Version 7  *****************
 * User: Kurtv        Date: 11/19/98   Time: 10:27a
 * Updated in $/LibKnvPP
 * Change BOOL to bool, where appropriate.
 * 
 * *****************  Version 6  *****************
 * User: Kurtv        Date: 11/14/97   Time: 8:13a
 * Updated in $/LibKnvPP
 * 
 * *****************  Version 5  *****************
 * User: Kurtv        Date: 7/19/97    Time: 8:24p
 * Updated in $/libknvpp
 * Added isContainer logic and index parameters to rewind() and next()
 * 
 * *****************  Version 4  *****************
 * User: Kurtv        Date: 3/05/97    Time: 3:38a
 * Updated in $/LibKnvPP
 * Renamed files, (lengthened and fixed case)
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 3/20/96    Time: 9:00a
 * Updated in $/libknvpp
 * Fixed bug in SetAt() that reallocated a CPObject even if the key
 * already existed.
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 12/27/95   Time: 8:36p
 * Updated in $/libknvpp
*/


//#include "Stdafx.h"
//#include "MapSortStringToOb.h"
//
//IMPLEMENT_DYNAMIC(CPObject,CObject);
//IMPLEMENT_DYNAMIC(CMapSortStringToOb,CMapStringToOb);
//   
//#ifdef USE_DEBUG_NEW
//#define new DEBUG_NEW
//#endif
   
/////////////////////////////////////////////////////////////////////////////////////
int mapSortStringToObKeyCmp(const void* elem1,const void* elem2)
{
   const SElement *p1,*p2;
      
   p1 = (SElement*)elem1;
   p2 = (SElement*)elem2;
      
   int retval = p1->pString->Compare(*(p2->pString));
      
   return retval;
}

int mapSortStringToObSeniorityCmp(const void* elem1,const void* elem2)
{
   const SElement *p1,*p2;
      
   p1 = (SElement*)elem1;
   p2 = (SElement*)elem2;
      
   int retval = p1->pObject->m_seniority - p2->pObject->m_seniority;
      
   return retval;
}

/////////////////////////////////////////////////////////////////////////////////////
CPObject::CPObject(int seniority,CObject* object)
{
   m_seniority = seniority;
   m_object = object;
}

CPObject::~CPObject()
{
   //delete m_object;
}

/////////////////////////////////////////////////////////////////////////////////////
CMapSortStringToOb::CMapSortStringToOb(int nBlockSize,bool isContainer) :
   CMapStringToOb(nBlockSize)
{
   m_isContainer = isContainer;

   m_isSorted = false;
   m_hElement = NULL;
   m_sortPos = 0;
   m_index = 0;
   m_sortSize = 0;
   m_nextSeniority = 0;
   m_sortFunction = mapSortStringToObKeyCmp;
}

CMapSortStringToOb::~CMapSortStringToOb()
{
   empty();
}

void CMapSortStringToOb::empty()
{
   if (m_isContainer)
   {
      DeleteAll();
   }
   else
   {
      RemoveAll();
   }
}

bool CMapSortStringToOb::swapSeniority(const char* key0,const char* key1)
{
   bool retval = false;
   CPObject *p0,*p1;

   if (CMapStringToOb::Lookup(key0,(CObject*&)p0) &&
       CMapStringToOb::Lookup(key1,(CObject*&)p1))
   {
      if (p0 != NULL && p1 != NULL)
      {
         int temp = p0->m_seniority;
         p0->m_seniority = p1->m_seniority;
         p1->m_seniority = temp;

         m_isSorted = false;
         retval = true;
      }
   }

   return retval;
}

void CMapSortStringToOb::SetAt(const char* key, CObject* newValue)
{
   CPObject* pObject;

   m_isSorted = false;

   if (CMapStringToOb::Lookup(key,(CObject*&)pObject))
   {
      if (m_isContainer)
      {
         delete pObject->m_object;
      }

      pObject->m_object = newValue;
   }
   else
   {
      pObject = new CPObject(m_nextSeniority,newValue);
      m_nextSeniority++;

      CMapStringToOb::SetAt(key,(CObject*)pObject);
   }
}

BOOL CMapSortStringToOb::RemoveKey(const char* key)
{
   BOOL retval = FALSE;
   m_isSorted = false;
   CPObject* pObject;
   
   if (CMapStringToOb::Lookup(key,(CObject*&)pObject))
   {
      delete pObject;
      retval =  CMapStringToOb::RemoveKey(key); 
   }
   
   return retval;
}

void CMapSortStringToOb::RemoveAll()
{
   m_isSorted = false;
   
   POSITION pos;
   CString key;
   CPObject* pObject;
         
   for (pos = CMapStringToOb::GetStartPosition();pos != NULL;)
   {
      CMapStringToOb::GetNextAssoc(pos,key,(CObject*&)pObject);
      delete pObject;
   }
   
   m_nextSeniority = 0;
   
   CMapStringToOb::RemoveAll();
   destroyElements();
}  

void CMapSortStringToOb::DeleteAll()
{
   m_isSorted = false;
   
   POSITION pos;
   CString key;
   CPObject* pObject;
         
   for (pos = CMapStringToOb::GetStartPosition();pos != NULL;)
   {
      CMapStringToOb::GetNextAssoc(pos,key,(CObject*&)pObject);
      delete pObject->m_object;
      delete pObject;
   }
   
   m_nextSeniority = 0;
   
   CMapStringToOb::RemoveAll();
   destroyElements();
}  

void CMapSortStringToOb::GetNextAssoc(POSITION& rNextPosition, CString& rKey, CObject*& rValue) const
{
   CPObject* pObject;
   
   CMapStringToOb::GetNextAssoc(rNextPosition,rKey,(CObject*&)pObject);
   rValue = pObject->m_object;
}

BOOL CMapSortStringToOb::Lookup(const char* key, CObject*& rValue) const
{
   BOOL retval;
   CPObject* pObject;
   
   if (retval = CMapStringToOb::Lookup(key,(CObject*&)pObject))
   {
      rValue = pObject->m_object;
   }
   
   return retval;   
}

void CMapSortStringToOb::GetFirstSorted(CString*& rKey, CObject*& rValue)
{
   if (!IsEmpty())
   {
      Sort();
      m_sortPos = 0;
   
      SElement* retElement;
   
      retElement = &(m_hElement[0]);
      rKey = retElement->pString;
      rValue = retElement->pObject->m_object;
   }
   else
   {
      rKey   = NULL;
      rValue = NULL;
   }
}

void CMapSortStringToOb::GetNextSorted(CString*& rKey, CObject*& rValue)
{
   if (!m_isSorted)
   {
      rKey   = NULL;
      rValue = NULL;
   }
   else
   {
      if (++m_sortPos >= GetCount())
      {
         rKey   = NULL;
         rValue = NULL;
         m_sortPos = GetCount();
      }
      else
      {
         SElement* retElement;
         
         retElement = &(m_hElement[m_sortPos]);
         rKey = retElement->pString;
         rValue = retElement->pObject->m_object;
      }
   }
}

void CMapSortStringToOb::GetSorted(CString*& rKey, CObject*& rValue)
{
   if (!m_isSorted)
   {
      rKey   = NULL;
      rValue = NULL;
   }
   else
   {
      if (m_sortPos >= GetCount() || IsEmpty())
      {
         rKey   = NULL;
         rValue = NULL;
      }
      else
      {
         SElement* retElement;
         
         retElement = &(m_hElement[m_sortPos]);
         rKey = retElement->pString;
         rValue = retElement->pObject->m_object;
      }
   }
}

void CMapSortStringToOb::rewind(int& index)
{
   Sort(); 
   index = 0;
}

bool CMapSortStringToOb::next(CObject*& object,int& index)
{
   bool retval = false;
   
   if (!m_isSorted)
   {
      retval = false;
   }
   else
   {
      if (index >= GetCount())
      {
         retval = false;
         index = GetCount();
      }
      else
      {
         SElement* retElement;
         
         retElement = &(m_hElement[index]);
         object = retElement->pObject->m_object;
         index++;
         retval = true;
      }
   }
   
   return retval;
}

bool CMapSortStringToOb::next(CObject*& object,CString& key,int& index)
{
   bool retval = false;
   
   if (!m_isSorted)
   {
      retval = false;
   }
   else
   {
      if (index >= GetCount())
      {
         retval = false;
         index = GetCount();
      }
      else
      {
         SElement* retElement;
         
         retElement = &(m_hElement[index]);
         key    = *(retElement->pString);
         object = retElement->pObject->m_object;
         index++;
         retval = true;
      }
   }
   
   return retval;
}

// sort by key (new function)
void CMapSortStringToOb::Sort()
{
   if (!m_isSorted)
   {
      destroyElements();
      m_sortSize = GetCount();
      m_hElement = new SElement [m_sortSize];      
         
      POSITION pos;
      CString key;
      CPObject* pObject;
      int i;
         
      for (pos = CMapStringToOb::GetStartPosition(),i = 0;pos != NULL && i < m_sortSize;i++)
      {
         CMapStringToOb::GetNextAssoc(pos,key,(CObject*&)pObject);
         m_hElement[i].pString = new CString(key); 
         m_hElement[i].pObject = pObject; 
      }

      qsort((void*)m_hElement,(size_t)m_sortSize,sizeof(SElement),
             m_sortFunction);
   }
   
   m_isSorted = true;
}

void CMapSortStringToOb::setSortFunction(int (*sortFunction)(const void* elem1,const void* elem2))
{
   m_isSorted = false;

   if (sortFunction == NULL)
   {
      m_sortFunction = mapSortStringToObKeyCmp;
   }
   else
   { 
      m_sortFunction = sortFunction;
   }
}

void CMapSortStringToOb::setSortBySeniority(bool seniority)
{
   if (seniority)
   {
      setSortFunction(mapSortStringToObSeniorityCmp);
   }
   else
   {
      setSortFunction(mapSortStringToObKeyCmp);
   }
} 

void CMapSortStringToOb::destroyElements()
{
   if (m_hElement != NULL)
   {
      int i;
   
      for (i = 0;i < m_sortSize;i++)
      {
         delete m_hElement[i].pString;
         //delete m_hElement[i].pObject;
      }
      
      delete m_hElement;
   }
   
   m_sortSize = 0;
   m_hElement = NULL;
}

#ifdef _DEBUG
void CMapSortStringToOb::Dump(CDumpContext& dc) const
{
   CMapStringToOb::Dump(dc);
   
   dc << "CMapSortStringToOb " <<
   "\n";
}

#endif
