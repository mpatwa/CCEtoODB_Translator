// $Header: /CAMCAD/4.5/OsVersion.h 3     10/18/05 8:13p Kurt Van Ness $

/*
History: OsVersion.h $
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 4/24/98    Time: 12:08p
 * Updated in $/SchPrint
 * Added "#pragma once" directive.
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 4/04/97    Time: 1:14p
 * Updated in $/Schprint
 * Lengthened file names and fixed case
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 4/04/97    Time: 12:16p
 * Created in $/Schprint
 * Initial add
*/

#if !defined(__OsVersion_H__)
#define __OsVersion_H__

#pragma once

class COsVersion
{
private:
   static bool m_initialized;
   static OSVERSIONINFO m_versionInfo;

public:
   static void initialize();
   static bool isWin32s();
   static bool isWindows95();
   static bool isWindows98();
   static bool isWindowsMe();
   static bool isWindowsNt351();
   static bool isWindowsNt4();
   static bool isWindows2000();
   static bool isWindowsXp();
   static bool isWindows2003();
};

#endif

