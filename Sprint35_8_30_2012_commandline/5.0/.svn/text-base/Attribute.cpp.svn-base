// $Header: /CAMCAD/5.0/Attribute.cpp 20    6/15/07 5:33p Kurt Van Ness $

#include "StdAfx.h"

#ifdef UseOldCAttribute

#include "Attribute.h"
#include "CcDoc.h"
#include "WriteFormat.h"

//_____________________________________________________________________________
CAttribute::~CAttribute()
{
}

//_____________________________________________________________________________
int CAttributeList::m_refCount = 0;

CAttributeList::CAttributeList()
{
   m_refCount++;
}

CAttributeList::~CAttributeList()
{
   m_refCount--;
}

//_____________________________________________________________________________
CAttributeListMap::CAttributeListMap(int blockSize) :
   CTypedMapWordToObContainer<CAttributeList*>(blockSize)
{
}

//_____________________________________________________________________________
CAttributeMap::CAttributeMap(int blockSize) :
   m_map(blockSize)
{
}

CAttributeMap::~CAttributeMap()
{
}

INT_PTR CAttributeMap::GetCount() const
{ 
   return m_map.GetCount(); 
}

bool CAttributeMap::IsEmpty() const
{ 
   return m_map.IsEmpty() != 0; 
}

bool CAttributeMap::Lookup(WORD key,CAttributeList*& attributeList) const
{ 
   return (m_map.Lookup(key,attributeList) != 0);
}

bool CAttributeMap::Lookup(WORD key,CAttribute*& attribute) const
{ 
   bool retval = false;

   CAttributeList* attributeList;

   if (m_map.Lookup(key,attributeList))
   {
      if (attributeList->GetCount() > 0)
      {
         attribute = attributeList->GetHead();

         retval = (attribute != NULL);
      }
   }

   return retval;
}

void CAttributeMap::SetAt(WORD key,CAttribute* attribute)
{ 
   CAttributeList* attributeList;

   if (attribute == NULL)
   {
      if (m_map.Lookup(key,attributeList))
      {
         m_map.RemoveKey(key);
         delete attributeList;
      }
   }
   else
   {
      if (m_map.Lookup(key,attributeList))
      {
         attributeList->empty();
      }
      else
      {
         attributeList = new CAttributeList();
         m_map.SetAt(key,attributeList);
      }

      attributeList->AddTail(attribute);
   }
}

bool CAttributeMap::RemoveKey(WORD key)
{ 
   bool retval = false;

   CAttributeList* attributeList;

   if (m_map.Lookup(key,attributeList))
   {
      delete attributeList;
      m_map.RemoveKey(key);

      retval = true;
   }

   return retval;
}

void CAttributeMap::RemoveAll()
{ 
   m_map.empty(); 
}

POSITION CAttributeMap::GetStartPosition() const
{ 
   return m_map.GetStartPosition(); 
}

void CAttributeMap::GetNextAssoc(POSITION& rNextPosition,WORD& rKey,CAttribute*& attribute) const
{ 
   CAttributeList* attributeList;

   m_map.GetNextAssoc(rNextPosition,rKey,attributeList); 

   if (attributeList->GetCount() > 0)
   {
      attribute = attributeList->GetHead();
   }
   else
   {
      attribute = NULL;
   }
}

void CAttributeMap::GetNextAssoc(POSITION& rNextPosition,WORD& rKey,CAttributeList*& attributeList) const
{ 
   m_map.GetNextAssoc(rNextPosition,rKey,attributeList); 
}
#endif // #ifdef UseOldCAttribute





