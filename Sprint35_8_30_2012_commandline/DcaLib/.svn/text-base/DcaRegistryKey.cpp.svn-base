// $Header: /CAMCAD/DcaLib/DcaRegistryKey.cpp 1     3/23/07 5:19a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaRegistryKey.h"

//=============================================================================

/*
History: RwLib.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Kvanness     Date: 3/18/01    Time: 5:02p
 * Updated in $/LibKnvPP
 * Fixed bug in CRegistryKey::getStringValue()
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 6/12/99    Time: 11:31a
 * Created in $/PadsDRC
 * Initial add.
*/

//#include "StdAfx.h"
//#include "RegistryKey.h"
//
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

//_____________________________________________________________________________
CRegistryKey::CRegistryKey()
{
}

CRegistryKey::CRegistryKey(const CRegistryKey& other)
{
   *this = other;
}

CRegistryKey::~CRegistryKey()
{
}

CRegistryKey& CRegistryKey::operator=(const CRegistryKey& other)
{
   if (&other != this)
   {
      Close();

      if (other.m_hKey != NULL)
      {
         m_keyName = other.m_keyName;

         VERIFY(Open(other,NULL) == ERROR_SUCCESS);  // open another HKEY to the same registry key
      }
   }

   return *this;
}

bool CRegistryKey::isOpen()
{
   return (m_hKey != NULL);
}

CRegistryKey CRegistryKey::openSubKey(const CString& keyName)
{
   CRegistryKey subKey;

   LONG status = subKey.Open(*this,keyName);

   if (status != ERROR_SUCCESS)
   {
      subKey.Close();
   }
   else
   {
      subKey.m_keyName = keyName;
   }

   return subKey;
}

// creates and opens the subkey, OK if subkey already exists
CRegistryKey CRegistryKey::createSubKey(const CString& keyName)
{
   CRegistryKey subKey;

   LONG status = subKey.Create(*this,keyName);

   if (status != ERROR_SUCCESS)
   {
      subKey.Close();
   }
   else
   {
      subKey.m_keyName = keyName;
   }

   return subKey;
}

void CRegistryKey::closeKey()
{
   m_keyName.Empty();
   LONG status = Close();
}

int CRegistryKey::getNumSubKeys()
{
   DWORD numSubKeys;
   LONG status = RegQueryInfoKey(*this,NULL,NULL,NULL,&numSubKeys,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
   ASSERT(status == ERROR_SUCCESS);

   return numSubKeys;
}

CRegistryKey CRegistryKey::openSubKeyAt(int index)
{
   CString keyName;
   const DWORD maxNameLen = 128;
   DWORD len = maxNameLen;
   FILETIME fileTime;

   LONG status = RegEnumKeyEx(*this,index,keyName.GetBuffer(len + 1),&len,NULL,NULL,NULL,&fileTime);
   ASSERT(status == ERROR_SUCCESS);
   ASSERT(len < maxNameLen);

   keyName.ReleaseBuffer();

   return openSubKey(keyName);
}

int CRegistryKey::getNumValues()
{
   DWORD numValues;
   LONG status = RegQueryInfoKey(*this,NULL,NULL,NULL,NULL,NULL,NULL,&numValues,NULL,NULL,NULL,NULL);
   ASSERT(status == ERROR_SUCCESS);

   return numValues;
}

bool CRegistryKey::getIntValueAt(int index,CString& valueName,int& value)
{
   bool retval = false;

   const DWORD maxBufLen = 128;
   DWORD nameLen = maxBufLen;
   DWORD type;

   LONG status = RegEnumValue(*this,index,valueName.GetBuffer(nameLen + 1),&nameLen,NULL,&type,NULL,NULL);
   ASSERT(status == ERROR_SUCCESS);
   ASSERT(nameLen <  maxBufLen);

   valueName.ReleaseBuffer();

   if (type == REG_DWORD)
   {
      DWORD dw;

      status = QueryDWORDValue(valueName,dw);
      ASSERT(status == ERROR_SUCCESS);

      value  = dw;
      retval = true;
   }

   return retval;
}

bool CRegistryKey::getStringValueAt(int index,CString& valueName,CString& value)
{
   bool retval = false;

   const DWORD maxBufLen = 128;
   DWORD nameLen = maxBufLen;
   DWORD type;
   DWORD dataLen;

   LONG status = RegEnumValue(*this,index,valueName.GetBuffer(nameLen + 1),&nameLen,NULL,&type,NULL,&dataLen);
   ASSERT(status == ERROR_SUCCESS);
   ASSERT(nameLen <  maxBufLen);

   valueName.ReleaseBuffer();

   if (type == REG_SZ)
   {
      DWORD valueLen = maxBufLen;

      status = QueryStringValue(valueName,value.GetBuffer(valueLen + 1),&valueLen);
      ASSERT(status == ERROR_SUCCESS);
      ASSERT(valueLen <  maxBufLen);

      value.ReleaseBuffer();
      retval = true;
   }

   return retval;
}

bool CRegistryKey::getIntValue(const CString& valueName,DWORD& value)
{
   DWORD dw;

   LONG status = QueryDWORDValue(valueName,dw);
   bool retval = (status == ERROR_SUCCESS);
   value  = dw;

   return retval;
}

bool CRegistryKey::getStringValue(const CString& valueName,CString& value)
{
   const DWORD maxBufLen = 128;
   DWORD valueLen = maxBufLen;

   LONG status = QueryStringValue(valueName,value.GetBuffer(valueLen + 1),&valueLen);
   bool retval = (status == ERROR_SUCCESS);
   ASSERT(valueLen <  maxBufLen || !retval);

   value.ReleaseBuffer();

   return retval;
}

bool CRegistryKey::getValue(const CString& valueName,DWORD& value)
{
   DWORD dw;

   LONG status = QueryDWORDValue(valueName,dw);
   bool retval = (status == ERROR_SUCCESS);

   if (retval)
   {
      value  = dw;
   }

   return retval;
}

bool CRegistryKey::getValue(const CString& valueName,int& value)
{
   DWORD dw;

   LONG status = QueryDWORDValue(valueName,dw);
   bool retval = (status == ERROR_SUCCESS);

   if (retval)
   {
      value  = dw;
   }

   return retval;
}

bool CRegistryKey::getValue(const CString& valueName,bool& value)
{
   DWORD dw;

   LONG status = QueryDWORDValue(valueName,dw);
   bool retval = (status == ERROR_SUCCESS);

   if (retval)
   {
      value  = (dw != 0);
   }

   return retval;
}

bool CRegistryKey::getValue(const CString& valueName,CString& value)
{
   DWORD maxBufLen = 128;
   DWORD valueLen = maxBufLen;

   // Standard buf works most of the time
   LONG status = QueryStringValue(valueName,value.GetBuffer(valueLen + 1),&valueLen);

   // But somtimes standard buf is too small, try larger if necessary.
   while (status == ERROR_MORE_DATA && maxBufLen < 1000000)
   {
      maxBufLen *= 2;
      valueLen = maxBufLen;
      value.ReleaseBuffer();
      status = QueryStringValue(valueName,value.GetBuffer(valueLen + 1),&valueLen);
   }

   bool retval = (status == ERROR_SUCCESS);
   ASSERT(valueLen <  maxBufLen || !retval);

   value.ReleaseBuffer();

   return retval;
}

bool CRegistryKey::getValue(const CString& valueName,double& value)
{
   const DWORD maxBufLen = 128;
   DWORD valueLen = maxBufLen;
   CString stringValue;

   LONG status = QueryStringValue(valueName,stringValue.GetBuffer(valueLen + 1),&valueLen);
   bool retval = (status == ERROR_SUCCESS);
   ASSERT(valueLen <  maxBufLen || !retval);

   stringValue.ReleaseBuffer();

   if (retval)
   {
      value = atof(stringValue);
   }

   return retval;
}

bool CRegistryKey::setValue(const CString& valueName,DWORD value)
{
   LONG status = SetDWORDValue(valueName,value);
   bool retval = (status == ERROR_SUCCESS);

   return retval;
}

bool CRegistryKey::setValue(const CString& valueName,int value)
{
   LONG status = SetDWORDValue(valueName,value);
   bool retval = (status == ERROR_SUCCESS);

   return retval;
}

bool CRegistryKey::setValue(const CString& valueName,bool value)
{
   LONG status = SetDWORDValue(valueName,value);
   bool retval = (status == ERROR_SUCCESS);

   return retval;
}

bool CRegistryKey::setValue(const CString& valueName,const CString& value)
{
   LONG status = SetStringValue(valueName,value);
   bool retval = (status == ERROR_SUCCESS);

   return retval;
}

bool CRegistryKey::setValue(const CString& valueName,const char* value)
{
   LONG status = SetStringValue(valueName,value);
   bool retval = (status == ERROR_SUCCESS);

   return retval;
}

bool CRegistryKey::setValue(const CString& valueName,double value)
{
   CString stringValue;
   stringValue.Format("%f",value);

   LONG status = SetStringValue(valueName,stringValue);
   bool retval = (status == ERROR_SUCCESS);

   return retval;
}

//_____________________________________________________________________________
CAppRegistryKey::CAppRegistryKey()
{
   HKEY hKey = AfxGetApp()->GetAppRegistryKey();
   ASSERT(hKey != NULL);

   m_keyName = AfxGetApp()->m_pszProfileName;
   Attach(hKey);
}

CAppRegistryKey::~CAppRegistryKey()
{
}
