// $Header: /CAMCAD/4.6/read_wrt/SettingsFile.h 4     9/19/06 7:45p Kurt Van Ness $

//   Router Solutions Inc.
//   Copyright © 2006. All Rights Reserved.

#if ! defined (__SettingsFile_h__)
#define __SettingsFile_h__

#include "DcaSettingsFile.h"

// moved to DcaSettingsFile.h

////_____________________________________________________________________________
//class CSettingsFile
//{
//private:
//   CString m_filePath;
//   CStdioFile m_file;
//   bool m_openFlag;
//
//   CSupString m_line;
//   CStringArray m_params;
//   int m_numPar;
//
//public:
//   CSettingsFile();
//   CSettingsFile(const CString& filePath);
//   virtual ~CSettingsFile();
//
//   bool open();
//   bool open(const CString& filePath);
//   void close();
//   CString getFilePath() const;
//
//   bool getNextCommandLine();
//   int getParamCount() const;
//   CString getParam(int index) const;
//   int getIntParam(int index) const;
//   double getDoubleParam(int index) const;
//   CString getCommand() const;
//   bool isCommand(const char* command,int minimumParamCount);
//
//   CString getCommandLineString() const;
//   bool parseYesNoParam(int index,bool& flag) const;
//   bool parseYesNoParam(int index,bool& flag,bool defaultValue) const;
//
//   virtual bool commandsEqual(CString command0,CString command1);
//};

#endif


