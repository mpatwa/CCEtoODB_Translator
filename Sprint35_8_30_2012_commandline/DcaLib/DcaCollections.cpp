// $Header: /CAMCAD/5.0/Dca/DcaCollections.cpp 4     3/19/07 4:33a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaCollections.h"

//=============================================================================

/*
History: RwLib.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 11/14/97   Time: 8:13a
 * Updated in $/libknvpp
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 9/06/97    Time: 10:32a
 * Created in $/libknvpp
 * initial add
*/

//#include "Stdafx.h"
//#include "MapStringToWord.h"
//
//IMPLEMENT_DYNAMIC(CMapStringToWord,CObject);
//
//#ifdef USE_DEBUG_NEW
//#define new DEBUG_NEW
//#endif
   
//_____________________________________________________________________________
CMapStringToWord::CMapStringToWord(int blockSize) : m_map(blockSize)
{
}

CMapStringToWord::~CMapStringToWord()
{
   RemoveAll();
}

void CMapStringToWord::SetAt(const char* key,WORD value)
{
m_map.SetAt(key,(void*)value); // Miten -- m_map this can be the possible data structure where all the values from the *.cc (XML file) are being stored
}

BOOL CMapStringToWord::Lookup(const char* key,WORD& value) const
{
   void* pValue;
   BOOL retval;

   if ((retval = m_map.Lookup(key,pValue)))
   {
      value = (WORD)pValue;
   }

   return retval;
}

POSITION CMapStringToWord::GetStartPosition() const
{
	return m_map.GetStartPosition();
}

void CMapStringToWord::GetNextAssoc(POSITION& pos,CString& key,WORD& value) const
{
   void* pValue;
   m_map.GetNextAssoc(pos,key,pValue);
   value = (WORD)pValue;
}

void CMapStringToWord::RemoveAll()
{
   m_map.RemoveAll();
}
   
//_____________________________________________________________________________
CMapStringToInt::CMapStringToInt(int blockSize) : m_map(blockSize)
{
}

CMapStringToInt::~CMapStringToInt()
{
   RemoveAll();
}

void CMapStringToInt::SetAt(const char* key,int value)
{
   m_map.SetAt(key,(void*)value);
}

BOOL CMapStringToInt::Lookup(const char* key,int& value) const
{
   void* pValue;
   BOOL retval;

   if ((retval = m_map.Lookup(key,pValue)))
   {
      value = (int)pValue;
   }

   return retval;
}

POSITION CMapStringToInt::GetStartPosition(void) const
{
	return m_map.GetStartPosition();
}

void CMapStringToInt::GetNextAssoc(POSITION& pos,CString& key,int& value) const
{
   void* pValue;
   m_map.GetNextAssoc(pos,key,pValue);
   value = (int)pValue;
}

void CMapStringToInt::RemoveAll()
{
   m_map.RemoveAll();
}
