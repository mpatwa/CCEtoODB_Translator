

#if !defined(__DcaWriteFormat_h__)
#define __DcaWriteFormat_h__

#pragma once

//=============================================================================

#include <afxtempl.h>

class CWriteFormat;

//_____________________________________________________________________________
class CFormatWaitCursor : public CObject
{
private:
   CWriteFormat* m_writeFormat;
   CCmdTarget* m_cmdTarget;
   CWaitCursor* m_allocatedWaitCursor;

public:
   CFormatWaitCursor(CCmdTarget& cmdTarget,CWriteFormat& writeFormat);
   ~CFormatWaitCursor();
};

//_____________________________________________________________________________
class CWriteFormat : public CObject
{ 
   friend CFormatWaitCursor;

private:
   CString m_buf;

   int m_priorityIndex;
   CArray<int,int> m_priorityStack;

   int m_priorityFenceIndex;
   CArray<int,int> m_priorityFenceStack;

   int m_nextHeaderIndex;
   CStringArray m_headerStack;

   int m_nextPrefixIndex;
   CStringArray m_prefixStack;

   CStringArray m_prefixArray;
   CArray<int,int> m_prefixCount;

   bool m_defaultEnabled;
   bool m_sectionEnabled;
   CMapStringToOb m_markedSections;
   CMapStringToOb m_sections;
   int m_sectionIndex;
   CStringArray m_sectionStack;

   CCmdTarget* m_waitCursorCmdTarget;

   bool m_newLineMode;
   bool m_endsInNewline;

private:
   CCmdTarget* registerWaitCursorCmdTarget(CCmdTarget* cmdTarget);
   void withdrawWaitCursorCmdTarget(CCmdTarget* cmdTarget);

public:
   CWriteFormat(int bufSize=160);
   virtual ~CWriteFormat();

   int getPriority();
   int getPriorityFence();
   int setPriority(int priority);   
   int setPriorityFence(int priorityFence);   
   int pushPriority(int priority);   
   int pushPriorityFence(int priorityFence);   
   int pushPriorityIncrement(int priorityIncrement);   
   int pushPriorityFenceIncrement(int priorityFenceIncrement);   
   int popPriority();   
   int popPriorityFence(); 

   void setSectionDefault(bool defaultEnable);
   void setSection(const CString& sectionName);
   void clrSection(const CString& sectionName);
   void enableSection(const CString& sectionName);
   void disableSection(const CString& sectionName);
   void pushSection(const CString& sectionName);
   void popSection(const CString& sectionName);
   void calcSectionEnabled();

   int pushHeader(const CString& header);
   int popHeader(); 
   int pushPrefix(const CString& prefix);
   int popPrefix(); 
   void setPrefix(int index,const CString& prefix);
   CString getPrefix(int index);
   int getPrefixCount(int index);
   int setPrefixCount(int index,int value);
   bool setNewLineMode(bool newLineMode);

   void restoreWaitCursor();
    
   int writef(const char* format,...);
   int writef(int prefixIndex,const char* format,va_list args);
   int writef(int prefixIndex,const char* format,...);

   virtual CString getFilePath();
   virtual void write(const char* string) = 0;
   virtual void flush();
};

//_____________________________________________________________________________
class CNullWriteFormat : public CWriteFormat
{ 

public:
   CNullWriteFormat();

   virtual void write(const char* string);
};

//=============================================================================

class CExtFileException : public CFileException
{
public:
   CString getCause();
};

//=============================================================================
                  
class CStdioFileWriteFormat : public CWriteFormat
{
private:
   CStdioFile* m_allocatedStdioFile;
   CStdioFile* m_stdioFile;
   CString m_filePath;
   
public:
   CStdioFileWriteFormat(int bufSize = 160);
   CStdioFileWriteFormat(CStdioFile* stdioFile,int bufSize = 160);
   CStdioFileWriteFormat(const CString& filePath,int bufSize = 160);
   virtual ~CStdioFileWriteFormat();

   virtual bool open(const CString& filePath,CExtFileException* exception=NULL);
   virtual void close();
   virtual void flush();

   virtual CString getFilePath();
   virtual void write(const char* string);
   virtual void writefTimeStamp(const char* format,...);
};

//_____________________________________________________________________________                     
class CStreamFileWriteFormat : public CWriteFormat
{
private:
   FILE* m_file;
   
public:
   CStreamFileWriteFormat(FILE* file,int bufSize = 160);
   virtual ~CStreamFileWriteFormat();

   //virtual void close();

   virtual void write(const char* string);
};

//=============================================================================

class CTraceFormat : public CWriteFormat
{ 
   //DECLARE_DYNAMIC(CTraceFormat);

public:
   CTraceFormat(int bufSize = 160);

   virtual void write(const char* string);
   void setFromFile(const CString& path);
};

//=============================================================================
                     
#define PrefixUndefined (-1)
#define PrefixError     (0)
#define PrefixWarning   (1)
#define PrefixDisaster  (2)
#define PrefixStatus    (3)
#define PrefixTask      (4)
#define PrefixSummary   (5)
#define PrefixSeverity1 (6)
#define PrefixSeverity2 (7)
#define PrefixSeverity3 (8)
#define PrefixSeverityAssignOriginal  (9)
#define PrefixSeverityAssignNew      (10)
#define PrefixSuccess  (11)
#define PrefixFailure  (12)
#define PrefixCaution  (13)
#define PrefixLastUsed (13)

//_____________________________________________________________________________
class CLogWriteFormat : public CStdioFileWriteFormat
{
public:
   CLogWriteFormat(int bufSize = 160);
   CLogWriteFormat(CStdioFile* stdioFile,int bufSize = 160);
   CLogWriteFormat(const CString& filePath,int bufSize = 160);
   virtual ~CLogWriteFormat();

private:
   void initPrefixes();
   
};

//_____________________________________________________________________________
class CMultipleWriteFormat : public CWriteFormat
{
private:
   CTypedPtrList<CObList,CWriteFormat*> m_writeFormatList;
   
public:
   CMultipleWriteFormat(int bufSize = 160);
   ~CMultipleWriteFormat();
   void empty();

   bool attach(CWriteFormat* writeFormat);
   bool detach(CWriteFormat* writeFormat);

   virtual void write(const char* string);
};

//=============================================================================

class CRecordParser : public CObject
{
   DECLARE_DYNCREATE(CRecordParser)

protected:
   CDWordArray m_recordColors;
   CStringArray m_recordPrefixes;

public:
   CRecordParser();
   virtual ~CRecordParser();

   void setDefaults();

   virtual int getRecordType(const CString& record);
   virtual COLORREF getRecordColor(int recordType,const CString& record);
   virtual COLORREF getRecordColor(const CString& record);
   virtual void clear();
   virtual void setRecordPrefixAndColor(int recordType,const char* prefix,COLORREF color);
   virtual void setRecordPrefix(int recordType,const char* prefix);
   virtual void setRecordColor(int recordType,COLORREF color);
};

//_______________________________________________________________________________
class CCadRecordParser : public CObject
{
   DECLARE_DYNCREATE(CCadRecordParser)

protected:
   CRecordParser m_defaultRecordParser;
   CRecordParser* m_recordParser;

public:
   CCadRecordParser();
   virtual ~CCadRecordParser();

   CRecordParser& getRecordParser() { return *m_recordParser; }
   void setRecordParser(CRecordParser* recordParser);
};

//=============================================================================
                    
class CHtmlFileWriteFormat : public CStdioFileWriteFormat
{
private:
   bool m_formattedTextMode;
   CRecordParser m_recordParser;
   COLORREF m_currentColor;
   
public:
   CHtmlFileWriteFormat(int bufSize = 160);
   CHtmlFileWriteFormat(CStdioFile* stdioFile,int bufSize = 160);
   CHtmlFileWriteFormat(const CString& filePath,int bufSize = 160);
   virtual ~CHtmlFileWriteFormat();

   virtual bool open(const CString& filePath,CExtFileException* exception=NULL);
   virtual void close();

   virtual void write(const char* string);
   virtual void initPrefixes();

   virtual CRecordParser& getRecordParser();
};

//_____________________________________________________________________________
class CDebugWriteFormat
{
private:
   static bool m_enabled;
   static CStdioFileWriteFormat* s_writeFormat;
   static CString m_filePath;
   static CNullWriteFormat* s_nullWriteFormat;

public:
   static CWriteFormat& getWriteFormat();
   static void close();
   static void setFilePath(const CString& filePath);
   static bool enable(bool enableFlag);
};

//_____________________________________________________________________________
class CStringWriteFormat : public CWriteFormat
{
protected:
   CString m_buf;

public:
   CStringWriteFormat(int bufSize = 300);

   const CString& getString() const { return m_buf; }

   virtual void write(const char* string);
};

//_____________________________________________________________________________
class CMessageBoxWriteFormat : public CStringWriteFormat
{
public:
   CMessageBoxWriteFormat(int bufSize = 300);
   virtual ~CMessageBoxWriteFormat();
};

#endif
