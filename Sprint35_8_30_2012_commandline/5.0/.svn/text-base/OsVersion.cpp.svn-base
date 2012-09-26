// $Header: /CAMCAD/4.5/OsVersion.cpp 3     10/18/05 8:13p Kurt Van Ness $

/*
History: OsVersion.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 4/04/97    Time: 1:14p
 * Updated in $/SchPrint
 * Lengthened file names and fixed case
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 4/04/97    Time: 1:03a
 * Updated in $/Schprint
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 2/13/97    Time: 1:00a
 * Created in $/Schprint
 * Initial add.
*/

#include "Stdafx.h"
#include "OsVersion.h"

/*
typedef struct _OSVERSIONINFO{  
    DWORD dwOSVersionInfoSize; 
    DWORD dwMajorVersion; 
    DWORD dwMinorVersion; 
    DWORD dwBuildNumber; 
    DWORD dwPlatformId; 
    TCHAR szCSDVersion[ 128 ]; 
} OSVERSIONINFO; 

VER_PLATFORM_WIN32s            	Win32s on Windows 3.1. 
VER_PLATFORM_WIN32_WINDOWS	      Win32 on Windows 95.
VER_PLATFORM_WIN32_NT	         Win32 on Windows NT.

*/

bool COsVersion::m_initialized = false;
OSVERSIONINFO COsVersion::m_versionInfo;

void COsVersion::initialize()
{
   if (!m_initialized)
   {
      m_initialized = true;

      m_versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
      GetVersionEx(&m_versionInfo);
   }
}

bool COsVersion::isWin32s()
{
   initialize();

   return m_versionInfo.dwPlatformId == VER_PLATFORM_WIN32s;
}

bool COsVersion::isWindows95()
{
   initialize();

   bool retval = ((m_versionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
                  (m_versionInfo.dwMajorVersion == 4) &&
                  (m_versionInfo.dwMinorVersion == 0));

   return retval;
}

bool COsVersion::isWindows98()
{
   initialize();

   bool retval = ((m_versionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
                  (m_versionInfo.dwMajorVersion == 4) &&
                  (m_versionInfo.dwMinorVersion == 10));

   return retval;
}

bool COsVersion::isWindowsMe()
{
   initialize();

   bool retval = ((m_versionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
                  (m_versionInfo.dwMajorVersion == 4) &&
                  (m_versionInfo.dwMinorVersion == 90));

   return retval;
}

bool COsVersion::isWindowsNt351()
{
   initialize();

   bool retval = ((m_versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
                  (m_versionInfo.dwMajorVersion == 3) &&
                  (m_versionInfo.dwMinorVersion == 51));

   return retval;
}

bool COsVersion::isWindowsNt4()
{
   initialize();

   bool retval = ((m_versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
                  (m_versionInfo.dwMajorVersion == 4) &&
                  (m_versionInfo.dwMinorVersion == 0));

   return retval;
}

bool COsVersion::isWindows2000()
{
   initialize();

   bool retval = ((m_versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
                  (m_versionInfo.dwMajorVersion == 5) &&
                  (m_versionInfo.dwMinorVersion == 0));

   return retval;
}

bool COsVersion::isWindowsXp()
{
   initialize();

   bool retval = ((m_versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
                  (m_versionInfo.dwMajorVersion == 5) &&
                  (m_versionInfo.dwMinorVersion == 1));

   return retval;
}

bool COsVersion::isWindows2003()
{
   initialize();

   bool retval = ((m_versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
                  (m_versionInfo.dwMajorVersion == 5) &&
                  (m_versionInfo.dwMinorVersion == 2));

   return retval;
}


