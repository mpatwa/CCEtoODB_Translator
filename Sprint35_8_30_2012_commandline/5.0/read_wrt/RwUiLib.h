// $Header: /CAMCAD/5.0/read_wrt/RwUiLib.h 4     12/10/06 4:04p Kurt Van Ness $

#if !defined(__RwUiLib_h__)
#define __RwUiLib_h__

#pragma once

#include "RwLib.h"

//CString getLogFilePath(const char* fileName);
bool extractArchive(const CString& filePathString,CString& errorMessage);
int formatMessageBoxApp(const char* format,...);
int formatMessageBoxApp(UINT flags,const char* format,...);
CString GetTempDirectory();
CString GetLogfileDirectory();
CString GetLogfilePath(CString logFileName);
CString GetTempFilePath(CString fileName);
#endif

