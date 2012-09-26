// $Header: /CAMCAD/5.0/Dca/DcaCollections.h 4     3/19/07 4:33a Kurt Van Ness $

#if !defined(__DcaCollections_h__)
#define __DcaCollections_h__

#pragma once

//=============================================================================

/*
History: RwLib.h $
 * 
 * *****************  Version 4  *****************
 * User: Kurtv        Date: 5/03/99    Time: 8:25a
 * Updated in $/LibKnvPP
 * Added declaration for InitHashTable().
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 4/22/98    Time: 11:52a
 * Updated in $/include
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 12/05/97   Time: 8:45a
 * Updated in $/include
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 9/08/97    Time: 11:50a
 * Created in $/include
 * Initial add
*/

//_____________________________________________________________________________
class CMapStringToWord : public CObject
{ 
   //DECLARE_DYNAMIC(CMapStringToWord);

private:
   CMapStringToPtr m_map;

public:
   CMapStringToWord(int blockSize = 10);
   ~CMapStringToWord();

   void SetAt(const char* key,WORD value);
   BOOL Lookup(const char* key,WORD& value) const;
   POSITION GetStartPosition() const;
   void GetNextAssoc(POSITION& pos,CString& key,WORD& value) const;
   void RemoveAll();
   void InitHashTable(int hashTableSize,BOOL allocateNow=TRUE) { m_map.InitHashTable(hashTableSize,allocateNow); }
   int GetCount() const { return (int) m_map.GetCount(); }
   BOOL RemoveKey(const char* key) { return m_map.RemoveKey(key); }
};

//_____________________________________________________________________________
class CMapStringToInt : public CObject
{ 
private:
   CMapStringToPtr m_map;

public:
   CMapStringToInt(int blockSize = 10);
   ~CMapStringToInt();

   void SetAt(const char* key,int value);
   BOOL Lookup(const char* key,int& value) const;
   POSITION GetStartPosition(void) const;
   void GetNextAssoc(POSITION& pos,CString& key,int& value) const;
   void RemoveAll();
   void InitHashTable(int hashTableSize,BOOL allocateNow=TRUE) { m_map.InitHashTable(hashTableSize,allocateNow); }
   int GetCount() const { return (int) m_map.GetCount(); }
   BOOL RemoveKey(const char* key) { return m_map.RemoveKey(key); }
};

#endif
