// $Header: /CAMCAD/5.0/read_wrt/RwLib.cpp 64    5/22/07 1:45a Rick Faltersack $

#include "StdAfx.h"
#include "CCEtoODB.h"
#include "RwLib.h"
#include "Gauge.h"
#include "StdioZipFile.h"
#include "CompValues.h"
#include "Fpeq.h"
#include "WriteFormat.h"
#include "DbUtil.h"
#include "extents.h"

#include <io.h>
#include <sys/stat.h>
#include <direct.h>

//=============================================================================

void displayProgress(CWriteFormat& log,const char* format,...)
{
   extern CProgressDlg* progress;

   va_list args;
   va_start(args,format);

   CString messageBuf;
   messageBuf.FormatV(format,args);
   va_end(args);

   log.writef(PrefixStatus,"%s %s\n",
      CTime::GetCurrentTime().Format("%Y%m%d.%H%M%S"),
      (const char*)messageBuf);

   if (progress != NULL)
      progress->SetStatus(messageBuf);
}

void hideProgress()
{
   extern CProgressDlg* progress;

   if (progress != NULL)
   {
      progress->GetDlgItem(CG_IDC_PROGDLG_PROGRESS)->ShowWindow(SW_HIDE);
      progress->GetDlgItem(CG_IDC_PROGDLG_PERCENT)->ShowWindow(SW_HIDE);
   }
}

//=============================================================================

/*
History: RwLib.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 3/05/97    Time: 3:38a
 * Updated in $/LibKnvPP
 * Renamed files, (lengthened and fixed case)
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 12/27/95   Time: 8:36p
 * Updated in $/libknvpp
*/

//#include "Stdafx.h"
//#include "LibKnvPP.h"

///////////////////////////////////////////////////////////////////////////////
int trchar(CString& string,char fromChar,char toChar)
{
   char* p;
   int len = string.GetLength();
   int count = 0,index;
   
   for (p = string.GetBuffer(0),index = 0;*p != '\0' && index < len;p++,index++)
   { 
      if (*p == fromChar)
      {
         *p = toChar;
         count++;
      }
   }
   
   string.ReleaseBuffer();
   
   return count;
}

//=============================================================================

/*
History: RwLib.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Kurtv        Date: 11/14/97   Time: 8:13a
 * Updated in $/libknvpp
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 3/05/97    Time: 3:38a
 * Updated in $/LibKnvPP
 * Renamed files, (lengthened and fixed case)
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 12/27/95   Time: 8:36p
 * Updated in $/libknvpp
*/

//#include "Stdafx.h"
//#include "LibKnvPP.h"

///////////////////////////////////////////////////////////////////////////////
// Keep
CString dirname(CString path)
{
   trchar(path,'/','\\');
   int index = path.ReverseFind('\\');
   
   if (index < 0) path.Empty();
   else if (index == 0)
   {
      path.GetBufferSetLength(1);
   }
   else
   {
      path.GetBufferSetLength(index);
   }
   
   return path; 
}

//=============================================================================

/*
History: RwLib.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Kvanness     Date: 2/06/00    Time: 11:52a
 * Updated in $/LibKnvPP
 * Added comments
 * 
 * *****************  Version 7  *****************
 * User: Kurtv        Date: 4/22/98    Time: 9:24a
 * Updated in $/LibKnvPP
 * Changed logic to prevent infinite loop when dirname(x) returns x.
 * 
 * *****************  Version 6  *****************
 * User: Kurtv        Date: 11/14/97   Time: 8:13a
 * Updated in $/LibKnvPP
 * 
 * *****************  Version 5  *****************
 * User: Kurtv        Date: 8/13/97    Time: 1:03p
 * Updated in $/libknvpp
 * 
 * *****************  Version 4  *****************
 * User: Kurtv        Date: 3/05/97    Time: 3:38a
 * Updated in $/LibKnvPP
 * Renamed files, (lengthened and fixed case)
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 9/29/96    Time: 1:43a
 * Updated in $/libknvpp
 * Added additional error checking code to handle win32 compatibility
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 12/27/95   Time: 8:36p
 * Updated in $/libknvpp
*/

//#include "Stdafx.h"
//#include "LibKnvPP.h"
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <direct.h>
//#include <errno.h>

//_____________________________________________________________________________
// returns -1 if an error occured, otherwise returns 0
// path is a directory, constucts entire path
int mkdirtree(const CString& dirPath)
{
   extern int errno;
   struct _stat status;
   int retval = -1,localErrno;
   
   retval = _mkdir(dirPath);
   localErrno = errno;
      
   if (retval == -1)
   {
      if (localErrno == ENOENT)
      {
         CString parentDir = dirname(dirPath);

         if (parentDir != dirPath)
         {
            if ((retval = mkdirtree(parentDir)) == 0)
            {
               retval = _mkdir(dirPath);
               localErrno = errno;
            }
         }
      }
      else if (localErrno == EACCES)
      {
         // on NOVELL networks, EACCES is returned instead of ENOENT
         // when path is missing parent directories
         retval = _stat(dirPath,&status);
         localErrno = errno;

         if (retval == -1)
         {
            if (localErrno == ENOENT)
            {
               CString parentDir = dirname(dirPath);

               if (parentDir != dirPath)
               {
                  if ((retval = mkdirtree(parentDir)) == 0)
                  {
                     retval = _mkdir(dirPath);
                     localErrno = errno;
                  }
               }
            }
         }
      }
      else if (localErrno == EEXIST)
      {
         // on NT networks, EEXIST is returned instead of ENOENT
         // when path is missing parent directories
         retval = _stat(dirPath,&status);
         localErrno = errno;

         if (retval == -1)
         {
            if (localErrno == ENOENT)
            {
               CString parentDir = dirname(dirPath);

               if (parentDir != dirPath)
               {
                  if ((retval = mkdirtree(parentDir)) == 0)
                  {
                     retval = _mkdir(dirPath);
                     localErrno = errno;
                  }
               }
            }
         }
      }
   }
   
   return retval;
}

//=============================================================================
/*
History: RwLib.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Kvanness     Date: 2/06/00    Time: 11:51a
 * Updated in $/LibKnvPP
 * Added comments
 * 
 * *****************  Version 5  *****************
 * User: Kurtv        Date: 11/14/97   Time: 8:13a
 * Updated in $/LibKnvPP
 * 
 * *****************  Version 4  *****************
 * User: Kurtv        Date: 8/13/97    Time: 1:03p
 * Updated in $/libknvpp
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 3/05/97    Time: 3:38a
 * Updated in $/LibKnvPP
 * Renamed files, (lengthened and fixed case)
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 12/27/95   Time: 8:36p
 * Updated in $/libknvpp
*/

//#include "Stdafx.h"
//#include "LibKnvPP.h"

//_____________________________________________________________________________
// returns -1 if an error occured, otherwise returns 0
// path is a file, constructs parent directory
int mkdirpath(const CString& filePath)
{
   int retval = mkdirtree(dirname(filePath));
   
   return retval;
}

//=============================================================================

/*
History: RwLib.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Kvanness     Date: 2/06/00    Time: 11:45a
 * Created in $/libknvpp
 * Initial add
*/

//#include "Stdafx.h"
//#include "LibKnvPP.h"
//#include "FilePath.h"

//_____________________________________________________________________________
bool shellPathDelete(const CString& fullPath)
{
   CString path(fullPath);
   int len = path.GetLength();
   char* pathBuf = path.GetBufferSetLength(len + 2);
   pathBuf[len + 1] = '\0';

   SHFILEOPSTRUCT fileOp;
   fileOp.hwnd = NULL;
   fileOp.wFunc = FO_DELETE;
   fileOp.pFrom = pathBuf;
   fileOp.pTo   = NULL;
   fileOp.fFlags = FOF_NOCONFIRMATION;

   bool success = (SHFileOperation(&fileOp) == 0);

   return success;
}

//_____________________________________________________________________________
// returns true if the directory tree is successfully deleted
bool removeDirectoryTreeAbsolute(const CString& fullPath)
{
   bool retval = true;

   CFileFind fileFind;
   CString filePath;

#ifdef xxxSaferDebug
   if (formatMessageBox(MB_YESNO | MB_ICONQUESTION,
      "OK to delete the directory '%s' and its contents?",(const char*)fullPath) != IDYES)
   {
      return false;
   }
#endif

   for (BOOL moreFiles = fileFind.FindFile(fullPath + "/*.*");moreFiles;)
   {
      moreFiles = fileFind.FindNextFile();
      filePath  = fileFind.GetFilePath();

      if (fileFind.IsDirectory() && !fileFind.IsDots())
      {
         retval = removeDirectoryTreeAbsolute(fileFind.GetFilePath()) && retval;
      }
      else if (!fileFind.IsDirectory())
      {
         //retval = shellPathDelete(filePath) && retval;
         retval = (DeleteFile(filePath) != 0) && retval;
      }
   }

   fileFind.Close();

   retval = (RemoveDirectory(fullPath) != 0) && retval;
   
   return retval;
}

CString RecursiveFindFilePath(CString parentPath, CString fileName)
{
   // Search for file starting at parentPath.
   // If not found in folder parentPath, recursively search all folders in parentPath.
   // Return path of file if found, otherwise return empty string.

   if (parentPath.Right(1) != "\\")
      parentPath += "\\";

   CString filepath( parentPath + fileName );

   CFileFind finder;
   if (finder.FindFile(filepath))
   {
      finder.FindNextFile();
      return finder.GetFilePath();
   }
   else
   {
      CString allfiles( parentPath + "*.*" );

      BOOL moreFiles = finder.FindFile(allfiles);
      while (moreFiles)
      {
         moreFiles = finder.FindNextFile();

         CString temp(finder.GetFileName());
         if (finder.IsDirectory() && !finder.IsDots())   // not interested in . or ..  (would cause infinite recursion, continually looking through self)
         {
            CString candidate = RecursiveFindFilePath( finder.GetFilePath(), fileName );
            if (!candidate.IsEmpty())
               return candidate;
         }    
      }
   }

   return ""; // Not found
}

//_____________________________________________________________________________
// returns true if the directory tree is successfully deleted
bool removeDirectoryTree(const CString& path)
{
   bool retval = true;

   CFilePath filePath(path);

   if (!filePath.isAbsolutePath())
   {
      const int pathSize = 300;

      CString currentDirectory;
      retval = (GetCurrentDirectory(pathSize,currentDirectory.GetBuffer(pathSize)) > 0);
      currentDirectory.ReleaseBuffer();

      filePath.pushRoot(currentDirectory);
   }

   if (retval)
   {
      retval = removeDirectoryTreeAbsolute(filePath);
   }
   
   return retval;
}

//_____________________________________________________________________________
bool CAppRegistrySetting::Set(CString value)
{
   // okay if value is empty
   
   if (!m_category.IsEmpty() && !m_name.IsEmpty())
   {
      CAppRegistryKey appKey;
      CRegistryKey settings = appKey.createSubKey("Settings");

      if (settings.isOpen())
      {
         CRegistryKey registryKey = settings.createSubKey(m_category);

         if (registryKey.isOpen())
         { 
            if (registryKey.setValue(m_name, value))
            {
               return true;
            }
         }
      }
   }

   return false;
}

bool CAppRegistrySetting::Set(bool value)
{
   // okay if value is empty
   
   if (!m_category.IsEmpty() && !m_name.IsEmpty())
   {
      CAppRegistryKey appKey;
      CRegistryKey settings = appKey.createSubKey("Settings");

      if (settings.isOpen())
      {
         CRegistryKey registryKey = settings.createSubKey(m_category);

         if (registryKey.isOpen())
         { 
            if (registryKey.setValue(m_name, value))
            {
               return true;
            }
         }
      }
   }

   return false;
}

CString CAppRegistrySetting::Get()
{
   CString value;

   if (!m_category.IsEmpty() && !m_name.IsEmpty())
   {
      CAppRegistryKey appKey;
      CRegistryKey settings = appKey.createSubKey("Settings");

      if (settings.isOpen())
      {
         CRegistryKey registryKey = settings.createSubKey(m_category);

         if (registryKey.isOpen())
         { 
            if (registryKey.getValue(m_name, value))
            {
               // Ok, got it.
            }
         }
      }
   }

   return value;
}

bool CAppRegistrySetting::Get(bool& value)
{
   bool retval = false;
   if (!m_category.IsEmpty() && !m_name.IsEmpty())
   {
      CAppRegistryKey appKey;
      CRegistryKey settings = appKey.createSubKey("Settings");

      if (settings.isOpen())
      {
         CRegistryKey registryKey = settings.createSubKey(m_category);

         if (registryKey.isOpen())
         { 
            if (registryKey.getValue(m_name, value))
            {
               // Ok, got it.
               retval = true;
            }
         }
      }
   }

   return retval;
}


//=============================================================================

// During conversion of registry settings key from Router Solutions to Mentor Graphics
// some special lookups were required for implementing backward compatibility.
// Rather then leaving them spread around the various places that did these lookups
// they have been grouped together in this class. Someday we might not care about this
// backward compatibility anymore, so we can just delete this class and then delete
// anything that referenced it and that is that.


CString CBackwardCompatRegistry::Asymtek_GetDirectoryFromRegistry()
{
   // Lookup value in style that was local to old Asymtek code

   HRESULT res;
   HKEY settingsKey;
	char dirPath[_MAX_PATH];
	dirPath[0] = '\0';

   res = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Router Solutions\\CAMCAD Professional\\Settings", 0, KEY_READ, &settingsKey);
   if (res == ERROR_SUCCESS)
	{
		DWORD bufLen = _MAX_PATH;
		DWORD lType;
	   res = RegQueryValueEx(settingsKey, "Asymtek Header Directory", NULL, &lType, (LPBYTE)dirPath, &bufLen);
		RegCloseKey(settingsKey);
	}

	return dirPath;
}

CString CBackwardCompatRegistry::DFT_GetTestplanDir()
{
   // Lookup value in style that was local to old Access Analysis and Probe Placement code

   HRESULT res;
   HKEY settingsKey;
   char dirPath[_MAX_PATH];
   dirPath[0] = '\0';

   res = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Router Solutions\\CAMCAD Professional\\Settings", 0, KEY_READ, &settingsKey);
   if (res == ERROR_SUCCESS)
   {
      DWORD bufLen = _MAX_PATH;
      DWORD lType;
      res = RegQueryValueEx(settingsKey, "DFT Testplan Directory", NULL, &lType, (LPBYTE)dirPath, &bufLen);
      RegCloseKey(settingsKey);
   }

   return dirPath;
}


#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 255  //  MSDN says it can be up to 16383

void CBackwardCompatRegistry::PropagateRegistry(HKEY rootHkey, CString sourceKeyname)
{
   HKEY sourceHkey;

   if( RegOpenKeyEx( rootHkey, sourceKeyname, 0, KEY_READ, &sourceHkey) == ERROR_SUCCESS )
   {
      TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
      DWORD    cbName;                   // size of name string 
      TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
      DWORD    cchClassName = MAX_PATH;  // size of class string 
      DWORD    cSubKeys=0;               // number of subkeys 
      DWORD    cbMaxSubKey;              // longest subkey size 
      DWORD    cchMaxClass;              // longest class string 
      DWORD    cValues;              // number of values for key 
      DWORD    cchMaxValue;          // longest value name 
      DWORD    cbMaxValueData;       // longest value data 
      DWORD    cbSecurityDescriptor; // size of security descriptor 
      FILETIME ftLastWriteTime;      // last write time 

      DWORD i, retCode; 

      TCHAR  valueName[MAX_VALUE_NAME]; 
      DWORD cchValue = MAX_VALUE_NAME; 

      // Get the class name and the value count. 
      retCode = RegQueryInfoKey(
         sourceHkey,              // key handle 
         achClass,                // buffer for class name 
         &cchClassName,           // size of class string 
         NULL,                    // reserved 
         &cSubKeys,               // number of subkeys 
         &cbMaxSubKey,            // longest subkey size 
         &cchMaxClass,            // longest class string 
         &cValues,                // number of values for this key 
         &cchMaxValue,            // longest value name 
         &cbMaxValueData,         // longest value data 
         &cbSecurityDescriptor,   // security descriptor 
         &ftLastWriteTime);       // last write time 


      // Enumerate the subkeys

      if (cSubKeys)
      {
         for (i=0; i<cSubKeys; i++) 
         { 
            cbName = MAX_KEY_LENGTH;
            retCode = RegEnumKeyEx(sourceHkey, i, achKey, &cbName, NULL, NULL, NULL, &ftLastWriteTime); 
            if (retCode == ERROR_SUCCESS) 
            {
               CString subkeyname = sourceKeyname + "\\" + achKey;
               PropagateRegistry(rootHkey, subkeyname);
            }
         }
      } 

      // Enumerate the key values. 

      if (cValues) 
      {
         for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
         { 
            cchValue = MAX_VALUE_NAME; 
            valueName[0] = '\0'; 
            retCode = RegEnumValue(sourceHkey, i, valueName, &cchValue, NULL, NULL, NULL, NULL);

            if (retCode == ERROR_SUCCESS ) 
            { 
               char rsi_value[256];
               rsi_value[0] = '\0';
               DWORD rsi_bufLen = 256, rsi_type;
               HRESULT rsi_res = RegQueryValueEx(sourceHkey, valueName, NULL, &rsi_type, (LPBYTE)rsi_value, &rsi_bufLen);
               if (rsi_res == ERROR_SUCCESS)
               {
                  // If target does not already have this setting then propagate it
                  HKEY mgc_SettingsKey;
                  CString targetKeyname = sourceKeyname;
                  targetKeyname.Replace("Router Solutions", REGISTRY_COMPANY);
                  HRESULT mgc_res = RegOpenKeyEx(rootHkey, targetKeyname, 0, KEY_ALL_ACCESS, &mgc_SettingsKey);
                  if(mgc_res != ERROR_SUCCESS )
                  {
                     // Apparantly key does not exist, create it.
                     mgc_res = RegCreateKeyEx(rootHkey, targetKeyname, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &mgc_SettingsKey, NULL);
                  }
                  if (mgc_res == ERROR_SUCCESS)
                  {
                     char mgc_value[256];
                     mgc_value[0] = '\0';
                     DWORD mgc_bufLen = 256;
                     DWORD mgc_type;
                     mgc_res = RegQueryValueEx(mgc_SettingsKey, valueName, NULL, &mgc_type, (LPBYTE)mgc_value, &mgc_bufLen);
                     if (mgc_res != ERROR_SUCCESS)
                     {
                        mgc_res = RegSetValueEx(mgc_SettingsKey, valueName, NULL, rsi_type, (LPBYTE)rsi_value, rsi_bufLen);
                        if (mgc_res != ERROR_SUCCESS)
                        {
                           // Uh oh ... now what ...
                           int jj = 0;
                        }

                        RegCloseKey(mgc_SettingsKey);
                     }
                  }
               }
            } 
         }
      }

      RegCloseKey(sourceHkey);
   }
}



//=============================================================================

//_____________________________________________________________________________
extern CProgressDlg* progress;

COperationProgress::COperationProgress(double operationLength)
{
   m_position           = 0.;
   m_nextUpdatePosition = 0.;
   m_length             = operationLength;
   m_allocatedDialog    = NULL;

   init();
}

COperationProgress::~COperationProgress()
{
   extern CProgressDlg* progress;

   if (progress != NULL)
   {
      progress->GetDlgItem(CG_IDC_PROGDLG_PROGRESS)->ShowWindow(SW_HIDE);
      progress->GetDlgItem(CG_IDC_PROGDLG_PERCENT)->ShowWindow(SW_HIDE);
   }

   if (m_allocatedDialog != NULL)
   {
      if (progress == m_allocatedDialog)
      {
         progress = NULL;
      }

      m_allocatedDialog->DestroyWindow();
      delete m_allocatedDialog;      
   }
}

void COperationProgress::init()
{ 
   if (progress != NULL && !(::IsWindow(*progress)))
   {
      delete progress;
      progress = NULL;
   }

   if (progress == NULL)
   {
      if (m_allocatedDialog == NULL)
      {
         m_allocatedDialog = new CProgressDlg("", true);
         m_allocatedDialog->Create();
      }

      progress = m_allocatedDialog;
   }

   int showflag = getApp().SilentRunning ? SW_HIDE : SW_SHOW;
   progress->ShowWindow(showflag);
   progress->GetDlgItem(CG_IDC_PROGDLG_PROGRESS)->ShowWindow(showflag);
   progress->GetDlgItem(CG_IDC_PROGDLG_PERCENT)->ShowWindow(showflag);

   progress->SetRange(0,1000);
}

void COperationProgress::setLength(double length)
{
   m_length = length;
   m_nextUpdatePosition = 0.;
}

double COperationProgress::updateProgress(double position)
{
   extern CProgressDlg* progress;

   m_position = position;

   int pos = 0;

   if (m_length > 0.)
   {
      pos = (int)(1000. * (m_position / m_length));
   }

   if (progress != NULL)
      progress->SetPos(pos);

   m_nextUpdatePosition = ((pos + 9) / 10) * (m_length / 100.);

   return m_position;
}

double COperationProgress::incrementProgress(double increment)
{
   m_position += increment;

   if (m_position >= m_nextUpdatePosition)
   {
      updateProgress(m_position);
   }

   return m_position;
}

void COperationProgress::updateStatus(const CString& status)
{
   extern CProgressDlg* progress;
   if (progress != NULL)
      progress->SetStatus(status);
}

void COperationProgress::updateStatus(const CString& status,double length,double position)
{
   updateStatus(status);

   m_length             = length;
   m_position           = position;
   m_nextUpdatePosition = position;
}

//_____________________________________________________________________________
CFileReadProgress::CFileReadProgress(CFile& file)
{
   m_CFile        = &file;
   m_FILE         = NULL;
   m_fileReader   = NULL;
   m_stdioCompressedFile = NULL;

   setLength((double)m_CFile->GetLength());
}

CFileReadProgress::CFileReadProgress(FILE* file)
{
   m_CFile        = NULL;
   m_FILE         = file;
   m_fileReader   = NULL;
   m_stdioCompressedFile = NULL;

   setLength((double)_filelength(_fileno(m_FILE)));
}

CFileReadProgress::CFileReadProgress(CFileReader& fileReader)
{
   m_CFile        = NULL;
   m_FILE         = NULL;
   m_fileReader   = &fileReader;
   m_stdioCompressedFile = NULL;

   setLength((double)m_fileReader->getFileLength());
}

CFileReadProgress::CFileReadProgress(CStdioCompressedFile& stdioCompressedFile)
{
   m_CFile        = NULL;
   m_FILE         = NULL;
   m_fileReader   = NULL;
   m_stdioCompressedFile = &stdioCompressedFile;

   setLength((double)m_stdioCompressedFile->getFile().GetLength());
}

double CFileReadProgress::updateProgress()
{
   double position = 0.;

   if (m_CFile != NULL)
   {
      position = (double)m_CFile->GetPosition();
   }
   else if (m_FILE != NULL)
   {
      position = (double)ftell(m_FILE);
   }
   else if (m_fileReader != NULL)
   {
      position = (double)m_fileReader->getFilePosition();
   }
   else if (m_stdioCompressedFile != NULL)
   {
      position = (double)m_stdioCompressedFile->getNumBytesInput();
   }

   COperationProgress::updateProgress(position);

   return position;
}

//_____________________________________________________________________________
CMemoryStatus::CMemoryStatus(const CString& description)
{
   m_memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
   m_description           = description;
}

bool CMemoryStatus::sampleMemoryStatus()
{
   memset(&m_memoryStatus,0,sizeof(MEMORYSTATUSEX));
   m_memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);

   bool retval = false;
   
   // needed to remove to enable CamCad to launch on Windows NT
   //if (COsVersion::isWindowsXp() || COsVersion::isWindows2000() || COsVersion::isWindows2003())
   //{
   //   // GlobalMemoryStatusEx() returns a nonzero value for success
   //   retval = (GlobalMemoryStatusEx(&m_memoryStatus) != 0);
   //}

   return retval;
}

CString CMemoryStatus::int64DisplayString(unsigned __int64 value)
{
   const int k = 1024;
   double friendlyValue = (double)value;
   int exponent = 0;

   while (friendlyValue > k)
   {
      friendlyValue /= k;
      exponent += 3;
   }

   CString suffix = metricPrefixTagToAbbreviatedString((MetricPrefixTag)exponent);
   CString retval;

   retval.Format("%8.3f%-2.2s (0x%016I64x)",friendlyValue,(const char*)(suffix + "B"),value);

   return retval;
}

void CMemoryStatus::writeStatusReport(CWriteFormat& writeFormat)
{
   writeFormat.writef("dwMemoryLoad            = %d\n"        ,m_memoryStatus.dwMemoryLoad);
   writeFormat.writef("ullTotalPhys            = %s\n",int64DisplayString(m_memoryStatus.ullTotalPhys));
   writeFormat.writef("ullAvailPhys            = %s\n",int64DisplayString(m_memoryStatus.ullAvailPhys));
   writeFormat.writef("ullTotalPageFile        = %s\n",int64DisplayString(m_memoryStatus.ullTotalPageFile));
   writeFormat.writef("ullAvailPageFile        = %s\n",int64DisplayString(m_memoryStatus.ullAvailPageFile));
   writeFormat.writef("ullTotalVirtual         = %s\n",int64DisplayString(m_memoryStatus.ullTotalVirtual));
   writeFormat.writef("ullAvailVirtual         = %s\n",int64DisplayString(m_memoryStatus.ullAvailVirtual));
   writeFormat.writef("ullAvailExtendedVirtual = %s\n",int64DisplayString(m_memoryStatus.ullAvailExtendedVirtual));

   writeStatusReportLegend(writeFormat);
}

void CMemoryStatus::writeComparitiveStatusReport(CWriteFormat& writeFormat,const CMemoryStatus& other)
{
   int fieldWidth = int64DisplayString(0).GetLength();

   CString blanks("                                ");

   CString description0 = blanks + getDescription();
   CString description1 = blanks + other.getDescription();
   description0 = description0.Right(fieldWidth);
   description1 = description1.Right(fieldWidth);


   writeFormat.writef("                          %s     %s\n",
      description0,description1);

   writeFormat.writef("dwMemoryLoad            = %*d     %*d\n"            ,
      fieldWidth,m_memoryStatus.dwMemoryLoad,fieldWidth,other.m_memoryStatus.dwMemoryLoad);

   writeFormat.writef("ullTotalPhys            = %s     %s\n",
      int64DisplayString(m_memoryStatus.ullTotalPhys),
      int64DisplayString(other.m_memoryStatus.ullTotalPhys));

   writeFormat.writef("ullAvailPhys            = %s     %s\n",
      int64DisplayString(m_memoryStatus.ullAvailPhys),
      int64DisplayString(other.m_memoryStatus.ullAvailPhys));

   writeFormat.writef("ullTotalPageFile        = %s     %s\n",
      int64DisplayString(m_memoryStatus.ullTotalPageFile),
      int64DisplayString(other.m_memoryStatus.ullTotalPageFile));

   writeFormat.writef("ullAvailPageFile        = %s     %s\n",
      int64DisplayString(m_memoryStatus.ullAvailPageFile),
      int64DisplayString(other.m_memoryStatus.ullAvailPageFile));
   writeFormat.writef("ullTotalVirtual         = %s     %s\n",
      int64DisplayString(m_memoryStatus.ullTotalVirtual),
      int64DisplayString(other.m_memoryStatus.ullTotalVirtual));

   writeFormat.writef("ullAvailVirtual         = %s     %s\n",
      int64DisplayString(m_memoryStatus.ullAvailVirtual),
      int64DisplayString(other.m_memoryStatus.ullAvailVirtual));

   writeFormat.writef("ullAvailExtendedVirtual = %s     %s\n",
      int64DisplayString(m_memoryStatus.ullAvailExtendedVirtual),
      int64DisplayString(other.m_memoryStatus.ullAvailExtendedVirtual));

   writeStatusReportLegend(writeFormat);
}

void CMemoryStatus::writeStatusReportLegend(CWriteFormat& writeFormat)
{
   writeFormat.writef("\ndwMemoryLoad\n");
   writeFormat.writef("Number between 0 and 100 that gives a general idea of current memory utilization, in which 0 indicates no memory use and 100 indicates full memory use.\n\n");

   writeFormat.writef("ullTotalPhys\n");
   writeFormat.writef("Total size of physical memory, in bytes.\n\n");

   writeFormat.writef("ullAvailPhys\n");
   writeFormat.writef("Size of physical memory available, in bytes.\n\n");

   writeFormat.writef("ullTotalPageFile\n");
   writeFormat.writef("Size of the committed memory limit, in bytes. This is physical memory plus the size of the page file, minus a small overhead.\n\n");

   writeFormat.writef("ullAvailPageFile\n");
   writeFormat.writef("Size of available memory to commit, in bytes. The limit is ullTotalPageFile.\n\n");

   writeFormat.writef("ullTotalVirtual\n");
   writeFormat.writef("Total size of the user mode portion of the virtual address space of the calling process, in bytes.\n\n");

   writeFormat.writef("ullAvailVirtual\n");
   writeFormat.writef("Size of unreserved and uncommitted memory in the user mode portion of the virtual address space of the calling process, in bytes.\n\n");

   writeFormat.writef("ullAvailExtendedVirtual\n");
   writeFormat.writef("Size of unreserved and uncommitted memory in the extended portion of the virtual address space of the calling process, in bytes.\n\n");
}



/****************************************************************************/
/*
      Like fgets only better, no pain in the neck arbitrary fixed size buffers.
      Reads into a CString.
*/

bool fgetcs(CString &string, FILE *ifp)
{
   string.Empty();

   char c;
   while ( (c = fgetc(ifp)) != EOF )
   {
      string += c;

      if (c == '\n')
         return true;
   }
 
   // Even apparantly blank lines in the file will have a newline, so we should
   // never get a truly empty string unless we hit EOF.

   return !string.IsEmpty();
}


/****************************************************************************/
/*
      A step toward standardizing exporter log file headers.
*/
void WriteStandardExportLogHeader(FILE *logFp, CString exportFormatName)
{
   CTime time = CTime::GetCurrentTime();

   fprintf(logFp, "%s Format Export Log\n", exportFormatName);
   //fprintf(logFp, "Created by %s - %s\n", getApp().getCamCadTitle(), getApp().getCopyrightString());
   fprintf(logFp, "%s\n", time.Format("%A, %B %d, %Y - %X%p"));
   fprintf(logFp, "\n");
}

/****************************************************************************/
