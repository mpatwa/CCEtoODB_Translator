// $Header: /CAMCAD/5.0/read_wrt/RwUiLib.cpp 5     3/12/07 12:54p Kurt Van Ness $

#include "StdAfx.h"
#include "RwUiLib.h"
#include "CcView.h"
#include "CCEtoODB.h"
#include <io.h>
   

//_____________________________________________________________________________
bool extractArchive(const CString& filePathString,CString& errorMessage)
{
   bool retval = true;

   CFilePath filePath(filePathString);
   filePath.setDelimiterBackSlash();

   CFilePath unixDirPath(filePathString);
   unixDirPath.setDelimiterSlash();
   unixDirPath.popLeaf();

   CString  commandLine;

   if (filePath.getExtension().CompareNoCase("zip") == 0)
   {
      commandLine.Format("gunzip -f \"%s\"",(const char*)filePath.getPath());

      if (ExecAndWait(commandLine, SW_HIDE, NULL))
      {
         errorMessage.Format("%s\nThere was an error using gunzip.exe.",commandLine);
         retval = false;
      }
   }
   else if (filePath.getExtension().CompareNoCase("tar") == 0)
   {
      commandLine.Format("tar -x -f \"%s\" -C \"%s\"",
         (const char*)filePath.getPath(),(const char*)unixDirPath.getPath());

      if (ExecAndWait(commandLine, SW_HIDE, NULL))
      {
         errorMessage.Format("%s\nThere was an error using tar.exe.",commandLine);
         retval = false;
      }
   }
   else if (filePath.getExtension().CompareNoCase("tgz") == 0)
   {
      // this unzips from tgz to tar
      commandLine.Format("gunzip -f \"%s\"",(const char*)filePath.getPath());

      if (ExecAndWait(commandLine, SW_HIDE, NULL))
      {
         errorMessage.Format("%s\nThere was an error using gunzip.exe.",commandLine);
         retval = false;
      }
      else
      {
         CFilePath tarPath(filePath);
         tarPath.setExtension("tar");

         commandLine.Format("tar -xf \"%s\" -C \"%s\"", 
            (const char*)tarPath.getPath(),(const char*)unixDirPath.getPath());

         if (ExecAndWait(commandLine, SW_HIDE, NULL))
         {
            errorMessage.Format("%s\nThere was an error using tar.exe.",commandLine);
            retval = false;
         }
      }
   }
   else
   {
      errorMessage.Format("The file extension [%s] is unknown to the supported extract functions, can not Extract Archive!",
         (const char*)filePath.getExtension());
      retval = false;
   }

   return retval;
}

//_____________________________________________________________________________
int formatMessageBoxApp(const char* format,...)
{
   va_list args;
   va_start(args,format);

   CString buf;
   buf.FormatV(format,args);

   return ErrorMessage(buf);
}
//_____________________________________________________________________________
int formatMessageBoxApp(UINT flags, const char* format,...)
{
   va_list args;
   va_start(args,format);

   CString buf;
   buf.FormatV(format, args);

   return ErrorMessage(buf, "", flags);
}
//_____________________________________________________________________________
//Utility Function
BOOL ConvertToForwardSlash(CString& filePath)
{
    filePath.Replace('\\','/');

    return TRUE;    
}

// This one is the standard logfile path, the one that most things should use.
// Keep
CString GetLogfilePath(CString logFileName)
{
   CString logFileDirectory = GetLogfileDirectory();
   ConvertToForwardSlash(logFileDirectory);
   if(logFileDirectory.Right(1) != '/')
      logFileDirectory.Append("/");
   CString logfilePath = logFileDirectory + logFileName;
   return logfilePath;
}

// This one is restricted to the TEMP dir, should only be used in certain circumstances.
CString GetTempFilePath(CString fileName)
{
 
   CString dir = GetTempDirectory();
   ConvertToForwardSlash(dir);
   if(dir.Right(1) != '/')
      dir.Append("/");
   CString filePath = dir + fileName;
   return filePath;
}

// Keep
CString GetLogfileDirectory()
{
   CString logfileDirectory;

   // The old conventional standard log file location is where the active CCZ file is.
   if(NULL != getActiveView() && NULL != getActiveView()->GetDocument())
   {
      CString logfileDirectory = getActiveView()->GetDocument()->GetProjectPath();
      if(!logfileDirectory.IsEmpty())
      {
         if(-1 != _access(logfileDirectory,02))
            return logfileDirectory;
      }
   }

   // Must not be an active CCZ file, or maybe can't access there, so try using TEMP folder.
   logfileDirectory = GetTempDirectory();
   if(!logfileDirectory.IsEmpty())
   {
      if(-1 != _access(logfileDirectory,02))
            return logfileDirectory;
   }

   return "";  // Standard paths didn't work.
}
// Keep
CString GetTempDirectory()
{
   CString tempDir = getenv("TEMP");
   if(!tempDir.IsEmpty())
   {
      if(-1 != _access(tempDir,02))
            return tempDir;
   }

   tempDir = getenv("TMP");
   if(!tempDir.IsEmpty())
   {
      if(-1 != _access(tempDir,02))
         return tempDir;
   }

   return "";  // No temp dir
}
