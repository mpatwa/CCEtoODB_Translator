// $Header: /CAMCAD/DcaLib/DcaMessageFilter.h 3     3/09/07 5:15p Kurt Van Ness $

#if !defined(__DcaMessageFilter_h__)
#define __DcaMessageFilter_h__

#pragma once

#include "DcaCollections.h"

//_____________________________________________________________________________
enum MessageFilterTypeTag
{
   messageFilterTypeFormat,
   messageFilterTypeMessage,
   messageFilterTypeUndefined
};

// For writing to file
enum MessageFilterFileFormatTag
{
   messageFilterFileFormatPlain,
   messageFilterFileFormatEmbellished,  // original style
};

//_____________________________________________________________________________
class CMessageFilter
{
private:
   static bool m_useDialogsFlag;

   MessageFilterTypeTag m_messageFilterType;
   MessageFilterFileFormatTag m_fileFormat;
   CString m_filePath;
   bool m_fileAppend;
   bool m_modifiedFlag;

   CMapStringToInt m_messageFormatMap;
   CMapStringToInt m_messageMap;
   CStringList m_messageFormatList;
   CStringList m_messageList;

public:
   CMessageFilter(MessageFilterTypeTag messageFilterType);
   CMessageFilter(MessageFilterTypeTag messageFilterType,const CString& filePath, bool append = false, MessageFilterFileFormatTag fileFormat = messageFilterFileFormatEmbellished);
   ~CMessageFilter();
   void empty();
   void flush();

   bool containsMessageFormat(const CString& format);
   bool containsMessage(const CString& message);

   bool formatMessage(const char* format,...);

   int formatMessageBox(const char* format,...);
   int formatMessageBox(UINT flags,const char* format,...);
   int formatMessageBoxApp(const char* format,...);
   int formatMessageBoxApp(UINT flags,const char* format,...);

   int getMessageFormatCount() const;
   int getMessageCount() const;

   static bool getUseDialogsFlag();
   static void setUseDialogsFlag(bool flag);

private:
   bool addMessageFormat(const CString& format);
   bool addMessage(const CString& message);
   bool addMessage(const CString& format,const CString& message);
};

#endif
