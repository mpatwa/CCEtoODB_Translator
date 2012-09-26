// $Header: /CAMCAD/5.0/Dca/DcaMessageFilter.cpp 4     3/22/07 12:56a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaMessageFilter.h"
#include "DcaLib.h"

//_____________________________________________________________________________
bool CMessageFilter::m_useDialogsFlag = false;

CMessageFilter::CMessageFilter(MessageFilterTypeTag messageFilterType)
: m_modifiedFlag(true)
, m_messageFilterType(messageFilterType)
, m_fileFormat(messageFilterFileFormatEmbellished)
, m_fileAppend(false)
{
   m_messageFormatMap.InitHashTable(nextPrime2n(100));
   m_messageMap.InitHashTable(nextPrime2n(500));
}

CMessageFilter::CMessageFilter(MessageFilterTypeTag messageFilterType,const CString& filePath,bool append, MessageFilterFileFormatTag fileFormat)
: m_modifiedFlag(true)
, m_messageFilterType(messageFilterType)
, m_fileFormat(fileFormat)
, m_filePath(filePath)
, m_fileAppend(append)
{
   m_messageFormatMap.InitHashTable(nextPrime2n(100));
   m_messageMap.InitHashTable(nextPrime2n(500));
}

CMessageFilter::~CMessageFilter()
{
   if (m_modifiedFlag)
   {
      flush();
   }
}

void CMessageFilter::empty()
{
   m_messageFormatMap.RemoveAll();
   m_messageMap.RemoveAll();
   m_messageFormatList.RemoveAll();
   m_messageList.RemoveAll();
}

bool CMessageFilter::getUseDialogsFlag()
{
   return m_useDialogsFlag;
}

void CMessageFilter::setUseDialogsFlag(bool flag)
{
   m_useDialogsFlag = flag;
}

int CMessageFilter::getMessageFormatCount() const
{
   return m_messageFormatList.GetCount();
}

int CMessageFilter::getMessageCount() const
{
   return m_messageList.GetCount();
}

void CMessageFilter::flush()
{
   if (! m_filePath.IsEmpty())
   {
      CStdioFile file;

      if (file.Open(m_filePath,CFile::modeWrite | CFile::modeCreate | (m_fileAppend ? CFile::modeNoTruncate : 0)))
      {
         if (m_fileAppend)
            file.SeekToEnd();

         CStringList& list    = ((m_messageFilterType == messageFilterTypeFormat) ? m_messageFormatList : m_messageList);
         CMapStringToInt& map = ((m_messageFilterType == messageFilterTypeFormat) ? m_messageFormatMap  : m_messageMap );

         for (POSITION pos = list.GetHeadPosition();pos != NULL;)
         {
            CString message = list.GetNext(pos);
            int count = 0;

            map.Lookup(message,count);

            if (message.GetLength() > 0 && message.Right(1) == "\n")
            {
               message.GetBufferSetLength(message.GetLength() - 1);
            }

            CString line;
            if (m_fileFormat == messageFilterFileFormatEmbellished) // original style
               line.Format("%4d: \"%s\"\n",count,message);
            else
               line.Format("%s\n", message); // plain, only other choice at the moment

            file.WriteString(line);
         }

         file.Close();
      }
   }

   m_modifiedFlag = false;
}

bool CMessageFilter::containsMessageFormat(const CString& format)
{
   int count;
   bool retval =  (m_messageFormatMap.Lookup(format,count) != 0);

   return retval;
}

bool CMessageFilter::addMessageFormat(const CString& format)
{
   bool retval = false;
   int count = 1;

   if (m_messageFormatMap.Lookup(format,count))
   {
      retval = true;
      count++;
   }
   else
   {
      m_messageFormatList.AddTail(format);
   }

   m_messageFormatMap.SetAt(format,count);
   m_modifiedFlag = true;

   return retval;
}

bool CMessageFilter::containsMessage(const CString& message)
{
   int count;
   bool retval =  (m_messageMap.Lookup(message,count) != 0);

   return retval;
}

bool CMessageFilter::addMessage(const CString& message)
{
   bool retval = false;
   int count = 1;

   if (m_messageMap.Lookup(message,count))
   {
      retval = true;
      count++;
   }
   else
   {
      m_messageList.AddTail(message);
   }

   m_messageMap.SetAt(message,count);
   m_modifiedFlag = true;

   return retval;
}

bool CMessageFilter::addMessage(const CString& format,const CString& message)
{
   bool retvalMessage       = addMessage(message);
   bool retvalMessageFormat = addMessageFormat(format);

   return ((m_messageFilterType == messageFilterTypeFormat) ? retvalMessageFormat : retvalMessage);
}

bool CMessageFilter::formatMessage(const char* format,...)
{
   va_list args;
   va_start(args,format);

   CString message;
   message.FormatV(format,args);

   bool retval = addMessage(format,message);

   return retval;
}

int CMessageFilter::formatMessageBox(const char* format,...)
{
   va_list args;
   va_start(args,format);

   int retval = 0;

   CString message;
   message.FormatV(format,args);

   if (!addMessage(format,message))
   {
      retval = AfxMessageBox(message);  //*rcf this is a bug, does not silence !!!
   }

   return retval;
}

int CMessageFilter::formatMessageBox(UINT flags,const char* format,...)
{
   va_list args;
   va_start(args,format);

   int retval = 0;

   CString message;
   message.FormatV(format,args);

   if (!addMessage(format,message))
   {
      retval = AfxMessageBox(message,flags);  //*rcf this is a bug, does not silence !!!
   }

   return retval;
}

int CMessageFilter::formatMessageBoxApp(const char* format,...)
{
   va_list args;
   va_start(args,format);

   int retval = 0;

   CString message;
   message.FormatV(format,args);

   if (!addMessage(format,message))
   {
      if (getUseDialogsFlag()) //*rcf this is a bug, no calls ever set useDialogs flag
      {
         retval = AfxMessageBox(message);
      }
   }

   return retval;
}

int CMessageFilter::formatMessageBoxApp(UINT flags,const char* format,...)
{
   va_list args;
   va_start(args,format);

   int retval = 0;

   CString message;
   message.FormatV(format,args);

   if (!addMessage(format,message))
   {
      if (getUseDialogsFlag())  //*rcf this is a bug, no calls ever set useDialogs flag
      {
         retval = AfxMessageBox(message,flags);
      }
   }

   return retval;
}