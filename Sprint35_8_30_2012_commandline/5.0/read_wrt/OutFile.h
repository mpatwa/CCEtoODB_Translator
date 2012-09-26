// $Header: /CAMCAD/4.5/read_wrt/OutFile.h 1     4/27/06 3:36p Kurt Van Ness $

//   Router Solutions Inc.
//   Copyright © 2006. All Rights Reserved.

#if ! defined (__OutFile_h__)
#define __OutFile_h__

#include "SettingsFile.h"

//_____________________________________________________________________________
class COutFile : public CSettingsFile
{
public:
   COutFile();
   COutFile(const CString& filePath);
   virtual ~COutFile();
};

#endif


