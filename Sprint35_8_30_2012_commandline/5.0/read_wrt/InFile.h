// $Header: /CAMCAD/4.5/read_wrt/InFile.h 3     4/27/06 3:58p Kurt Van Ness $

//   Router Solutions Inc.
//   Copyright © 2006. All Rights Reserved.

#if ! defined (__InFile_h__)
#define __InFile_h__

#include "SettingsFile.h"

//_____________________________________________________________________________
class CInFile : public CSettingsFile
{
public:
   CInFile();
   CInFile(const CString& filePath);
   virtual ~CInFile();
};

/////////////////////////////////////////////////////////////////////////////
// CExtendFile
/////////////////////////////////////////////////////////////////////////////
class CExtendFile
{
#define MAX_BUFSIZE  100
private:
   FILE *m_filePoint;
   char m_prevstring[MAX_BUFSIZE];

public:
   CExtendFile(){m_filePoint = NULL;}
   ~CExtendFile(){Close();}

   bool Open(CString fileName, const char* mode);
   int fgetstream(char *buf,int size, FILE* fp);
   int ReadString(CString &line);
   bool FileExist(CString fileName);
   void Close();
};

#endif


