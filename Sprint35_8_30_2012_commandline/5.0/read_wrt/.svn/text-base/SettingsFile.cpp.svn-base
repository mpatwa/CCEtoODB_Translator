// $Header: /CAMCAD/4.6/read_wrt/SettingsFile.cpp 6     9/19/06 7:45p Kurt Van Ness $

//   Router Solutions Inc.
//   Copyright © 2006. All Rights Reserved.

#include "StdAfx.h"
//#include "SettingsFile.h"
//
////_____________________________________________________________________________
//CSettingsFile::CSettingsFile()
//: m_openFlag(false)
//, m_numPar(0)
//{
//}
//
//CSettingsFile::CSettingsFile(const CString& filePath)
//: m_openFlag(false)
//, m_numPar(0)
//{
//   open(filePath);
//}
//
//CSettingsFile::~CSettingsFile()
//{
//   close();
//}
//
//bool CSettingsFile::open()
//{
//   return open(m_filePath);
//}
//
//bool CSettingsFile::open(const CString& filePath)
//{
//   close();
//
//   m_filePath = filePath;
//
//   m_openFlag = (m_file.Open(m_filePath,CFile::modeRead | CFile::shareDenyNone) != 0);
//
//   return m_openFlag;
//}
//
//void CSettingsFile::close()
//{
//   if (m_openFlag)
//   {
//      m_file.Close();
//   }
//
//   m_openFlag = false;
//}
//
//CString CSettingsFile::getFilePath() const
//{
//   return m_filePath;
//}
//
//bool CSettingsFile::getNextCommandLine()
//{
//   if (m_openFlag)
//   {
//      while (true)
//      {
//         if (m_file.ReadString(m_line))
//         {
//            m_line.Replace("\t"," ");
//            m_numPar = m_line.ParseQuote(m_params," ");
//
//            if (m_numPar > 0 && m_params[0].Left(1) == ".")
//            {
//               break;
//            }
//         }
//         else
//         {
//            m_line.Empty();
//            close();
//            break;
//         }
//      }
//   }
//
//   return m_openFlag;
//}
//
//int CSettingsFile::getParamCount() const
//{
//   return m_numPar;
//}
//
//CString CSettingsFile::getParam(int index) const
//{
//   CString param;
//
//   if (index >= 0 && index < m_params.GetCount())
//   {
//      param = m_params.GetAt(index);
//   }
//
//   return param;
//}
//
//int CSettingsFile::getIntParam(int index) const
//{
//   return atoi(getParam(index));
//}
//
//double CSettingsFile::getDoubleParam(int index) const
//{
//   return atof(getParam(index));
//}
//
//CString CSettingsFile::getCommand() const
//{
//   return getParam(0);
//}
//
//bool CSettingsFile::commandsEqual(CString command0,CString command1)
//{
//   command0.MakeLower();
//   command0.Replace("_","");
//
//   command1.MakeLower();
//   command1.Replace("_","");
//
//   bool retval = (command0.Compare(command1) == 0);
//
//   return retval;
//}
//
//bool CSettingsFile::isCommand(const char* command,int minimumParamCount)
//{
//   bool retval = false;
//
//   if (minimumParamCount < 1)
//   {
//      minimumParamCount = 1;
//   }
//
//   if (getParamCount() >= minimumParamCount)
//   {
//      retval = commandsEqual(getParam(0),command);
//   }
//
//   return retval;
//}
//
//CString CSettingsFile::getCommandLineString() const
//{
//   return m_line;
//}
//
//bool CSettingsFile::parseYesNoParam(int index,bool& flag) const
//{
//   CString value = getParam(index);
//
//   if (value.GetLength() > 0)
//   {
//      value = value.Left(1);
//
//      if (value.CompareNoCase("y") == 0 || value.CompareNoCase("t") == 0)
//      {
//         flag = true;
//      }
//      else if (value.CompareNoCase("n") == 0 || value.CompareNoCase("f") == 0)
//      {
//         flag = false;
//      }
//   }
//
//   return flag;
//}
//
//bool CSettingsFile::parseYesNoParam(int index,bool& flag,bool defaultValue) const
//{
//   flag = defaultValue;
//
//   return parseYesNoParam(index,flag);
//}

