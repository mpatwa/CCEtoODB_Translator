// $Header: /CAMCAD/DcaLib/DcaRegistryKey.h 1     3/23/07 5:19a Kurt Van Ness $

#if !defined(__DcaRegistryKey_h__)
#define __DcaRegistryKey_h__

#pragma once

//=============================================================================

/*
History: RwLib.h $
 * 
 * *****************  Version 2  *****************
 * User: Kvanness     Date: 3/18/01    Time: 5:03p
 * Updated in $/LibKnvPP
 * Added comments.
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 6/12/99    Time: 11:31a
 * Created in $/PadsDRC
 * Initial add.
*/

#include <atlbase.h>

//_____________________________________________________________________________
class CRegistryKey : public CRegKey
{
protected:
   CString m_keyName;

public:
   CRegistryKey();
   CRegistryKey(const CRegistryKey& other);
   ~CRegistryKey();
   CRegistryKey& operator=(const CRegistryKey& other);

   CString getKeyName() { return m_keyName; }

   bool isOpen();
   CRegistryKey openSubKey(const CString& keyName);

   // creates and opens the subkey, OK if the subkey already exists
   CRegistryKey createSubKey(const CString& keyName); 

   void closeKey();

   int getNumSubKeys();
   CRegistryKey openSubKeyAt(int index);

   int getNumValues();
   bool getIntValueAt(int index,CString& valueName,int& value);
   bool getStringValueAt(int index,CString& valueName,CString& value);

   bool getIntValue(const CString& valueName,DWORD& value);
   bool getStringValue(const CString& valueName,CString& value);

   bool getValue(const CString& valueName,bool& value);
   bool getValue(const CString& valueName,int& value);
   bool getValue(const CString& valueName,DWORD& value);
   bool getValue(const CString& valueName,double& value);
   bool getValue(const CString& valueName,CString& value);

   bool setValue(const CString& valueName,bool value);
   bool setValue(const CString& valueName,int value);
   bool setValue(const CString& valueName,DWORD value);
   bool setValue(const CString& valueName,double value);
   bool setValue(const CString& valueName,const char* value);
   bool setValue(const CString& valueName,const CString& value);

};

//_____________________________________________________________________________
class CAppRegistryKey : public CRegistryKey
{
public:
   CAppRegistryKey();
   ~CAppRegistryKey();
};

#endif
