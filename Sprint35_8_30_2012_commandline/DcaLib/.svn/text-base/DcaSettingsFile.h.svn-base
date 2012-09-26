// $Header: /CAMCAD/4.6/read_wrt/SettingsFile.h 4     9/19/06 7:45p Kurt Van Ness $

//   Router Solutions Inc.
//   Copyright © 2006. All Rights Reserved.

#if ! defined (__DcaSettingsFile_h__)
#define __DcaSettingsFile_h__

#include "DcaLib.h"

//_____________________________________________________________________________
class CSettingsFile
{
private:
   CString m_filePath;
   CStdioFile m_file;
   bool m_openFlag;

   CSupString m_line;
   CStringArray m_params;
   int m_numPar;

   bool m_filterEqualsSign;

public:
   CSettingsFile();
   CSettingsFile(const CString& filePath);
   virtual ~CSettingsFile();

   void setFilterEqualsSign(bool flag);

   bool open();
   bool open(const CString& filePath);
   void close();
   CString getFilePath() const;

   bool getNextCommandLine();
   bool getNextCommandLine(CString delimitList);
   bool getNextCommandLine(char delimiter);
   int getParamCount() const;
   CString getParam(int index) const;
   int getIntParam(int index) const;
   double getDoubleParam(int index) const;
   CString getCommand() const;
   bool isCommand(const char* command,int minimumParamCount);

   CString getCommandLineString() const;
   bool parseYesNoParam(int index,bool& flag) const;
   bool parseYesNoParam(int index,bool& flag,bool defaultValue) const;

   virtual bool commandsEqual(const CString& command0,const CString& command1);

   static CString normalizeParameterString(const CString& parametersString);
   static bool parameterStringsEqual(const CString& parameterString0,const CString& parameterString1);
};

#endif


