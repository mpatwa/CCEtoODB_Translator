// $Header: /CAMCAD/4.5/read_wrt/InFile.cpp 3     4/27/06 3:58p Kurt Van Ness $

//   Router Solutions Inc.
//   Copyright © 2006. All Rights Reserved.

#include "StdAfx.h"
#include "InFile.h"

//_____________________________________________________________________________
CInFile::CInFile()
{
}

CInFile::CInFile(const CString& filePath)
: CSettingsFile(filePath)
{
}

CInFile::~CInFile()
{
}


/////////////////////////////////////////////////////////////////////////////
// CExtendFile
/////////////////////////////////////////////////////////////////////////////
bool CExtendFile::Open(CString fileName, const char* mode)
{
   m_filePoint = fopen(fileName.GetBuffer(),mode);
   fileName.ReleaseBuffer();   
   m_prevstring[0] = 0x00;

   return (m_filePoint)?true:false;
}

int CExtendFile::fgetstream(char *buf,int size, FILE* fp)
{
   int nextpos = 0;
   buf[0] = 0x00;

   //if previous string exists, copy to buffer
   if(m_prevstring[0]) 
   {
      nextpos = strlen(m_prevstring);
      strncpy(buf,m_prevstring,nextpos);
   }

   fgets(&buf[nextpos],size - nextpos,fp);
   
   //compatible with other OS
   int endpos = (strchr(buf,'\r') - buf);
   int len = strlen(buf);

   if(endpos >= 0 && endpos < len - 1)
   {
      nextpos = endpos + 1;
      strncpy(m_prevstring,&buf[nextpos],len - nextpos);
      m_prevstring[len - nextpos] = 0x00;
   }
   else
   {
      m_prevstring[0] = 0x00;
      nextpos = len;
   }

   buf[nextpos] = 0x00;
   return strlen(buf);
}

int CExtendFile::ReadString(CString &line)
{
   int count = 0;
   line.Empty();

   if(m_filePoint)
   {
      char buf[MAX_BUFSIZE];
      while(fgetstream(buf,MAX_BUFSIZE,m_filePoint))
      {
         int len = strlen(buf);
         count += len; 

         if(len > 0 && (buf[len-1] == '\r' || buf[len-1] == '\n'))
         {
            buf[len-1] = '\0';
            line.Append(buf);
            break;
         }

         line.Append(buf);
      }      
   }

   return count;
}

bool CExtendFile::FileExist(CString fileName)
{
   return fileExists(fileName);
}

void CExtendFile::Close()
{
   if(m_filePoint)
      fclose(m_filePoint);
}