// $Header: /CAMCAD/DcaLib/DcaWriteFormat.cpp 5     6/14/07 1:18p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaWriteFormat.h"
#include "DcaLib.h"
#include "DcaColors.h"

//=============================================================================

/*
History: WriteFormat.cpp $
 * 
 * *****************  Version 12  *****************
 * User: Kvanness     Date: 9/16/99    Time: 6:53a
 * Updated in $/LibKnvPP
 * Added support for newline control and buffer size access.
 * 
 * *****************  Version 11  *****************
 * User: Kurtv        Date: 6/12/99    Time: 10:13a
 * Updated in $/LibKnvPP
 * Added writef() overload with va_list parameter.
 * 
 * *****************  Version 10  *****************
 * User: Kurtv        Date: 10/08/98   Time: 4:27a
 * Updated in $/LibKnvPP
 * Added support for CWaitCursor
 * 
 * *****************  Version 9  *****************
 * User: Kurtv        Date: 4/22/98    Time: 9:43a
 * Updated in $/LibKnvPP
 * Added section code.
 * 
 * *****************  Version 8  *****************
 * User: Kurtv        Date: 11/14/97   Time: 8:13a
 * Updated in $/LibKnvPP
 * 
 * *****************  Version 7  *****************
 * User: Kurtv        Date: 9/22/97    Time: 7:40a
 * Updated in $/LibKnvPP
 * 
 * *****************  Version 6  *****************
 * User: Kurtv        Date: 9/15/97    Time: 1:52p
 * Updated in $/LibKnvPP
 * 
 * *****************  Version 5  *****************
 * User: Kurtv        Date: 9/08/97    Time: 11:56a
 * Updated in $/LibKnvPP
 * 
 * *****************  Version 4  *****************
 * User: Kurtv        Date: 8/02/97    Time: 11:57p
 * Updated in $/Libvlpro
 * Changes to support cad transfer
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 7/08/97    Time: 9:59a
 * Updated in $/Libvlpro
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 7/03/97    Time: 4:22a
 * Updated in $/Libvlpro
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 6/21/97    Time: 12:34a
 * Created in $/Libvlpro
 * Initial add
*/

//#include "StdAfx.h"
//#include "WriteFormat.h"
//
//IMPLEMENT_DYNAMIC(CWriteFormat,CObject);
//IMPLEMENT_DYNAMIC(CNullWriteFormat,CWriteFormat);
//   
//#ifdef USE_DEBUG_NEW
//#define new DEBUG_NEW
//#endif

//_____________________________________________________________________________
CFormatWaitCursor::CFormatWaitCursor(CCmdTarget& cmdTarget,CWriteFormat& writeFormat)
{
   m_writeFormat = &writeFormat;
   m_cmdTarget   = &cmdTarget;
   m_allocatedWaitCursor = NULL;

   if (m_writeFormat->registerWaitCursorCmdTarget(m_cmdTarget) == NULL)
   {
      m_allocatedWaitCursor = new CWaitCursor();
   }

   m_cmdTarget->RestoreWaitCursor();
}

CFormatWaitCursor::~CFormatWaitCursor()
{
   m_writeFormat->withdrawWaitCursorCmdTarget(m_cmdTarget);
   delete m_allocatedWaitCursor;
}

//_____________________________________________________________________________
CWriteFormat::CWriteFormat(int bufSize)
{
   // Case dts0100432780 has obsoleted need to directly manage buffer size.
   // The param is left in place for backward compatibility to all the usage, but
   // as time permits the usages may be adjusted to eliminate the unused
   // parameter altogether.
   // The bufsize parameter was used to allow sizing a fixed buffer previously
   // used by _vsnprintf. We are not longer using that, output size is fully
   // dynamic, the need to manage the buffer size by users of this utility is gone.

   m_priorityIndex = 0;
   m_priorityStack.SetSize(5,5);
   m_priorityStack.SetAtGrow(m_priorityIndex,0);

   m_priorityFenceIndex = 0;
   m_priorityFenceStack.SetSize(5,5);
   m_priorityFenceStack.SetAtGrow(m_priorityFenceIndex,0);

   m_nextHeaderIndex = 0;
   m_headerStack.SetSize(5,5);

   m_nextPrefixIndex = 0;
   m_prefixStack.SetSize(5,5);

   setSectionDefault(true);

   m_waitCursorCmdTarget = NULL;

   m_newLineMode   = false;
   m_endsInNewline = true;
}

CWriteFormat::~CWriteFormat()
{
}

void CWriteFormat::setSectionDefault(bool defaultEnabled)
{
   m_sectionIndex = 0;
   m_sectionStack.SetSize(0,5);
   m_markedSections.RemoveAll();
   m_sections.RemoveAll();

   m_sectionEnabled = defaultEnabled;
   m_defaultEnabled = defaultEnabled;
}

bool CWriteFormat::setNewLineMode(bool newLineMode)
{
   bool retval = m_newLineMode;
   m_newLineMode = newLineMode;
   m_endsInNewline = true;

   return retval;
}

int CWriteFormat::writef(const char* format,...)
{
   va_list args;
   va_start(args,format);

   int numWritten = 0;

   if (m_sectionEnabled && getPriority() >= getPriorityFence())
   {
      m_buf.FormatV(format, args);

      if (m_newLineMode)
      {
         numWritten = 0;
         int pos;

         for (bool loop = true;loop;)
         {
            CString buf;

            if (m_endsInNewline)
            {
               if (m_nextPrefixIndex > 0)
               {
                  buf = m_prefixStack.GetAt(m_nextPrefixIndex - 1);
               }

               for (int index = 0;index < m_nextHeaderIndex;index++)
               {
                  buf += m_headerStack.GetAt(index);
               }
            }

            pos = m_buf.Find('\n');
            m_endsInNewline = (pos >= 0);

            if (m_endsInNewline)
            {
               buf += m_buf.Left(pos + 1);
               m_buf = m_buf.Mid(pos + 1);

               loop = !m_buf.IsEmpty();
            }
            else
            {
               buf += m_buf;
               loop = false;
            }

            numWritten += buf.GetLength();
            write(buf);
         }
      }
      else if (m_nextHeaderIndex == 0 && m_nextPrefixIndex == 0)
      {
         numWritten = m_buf.GetLength();
         write(m_buf);
      }
      else
      {
         CString buf;

         if (m_nextPrefixIndex > 0)
         {
            buf = m_prefixStack.GetAt(m_nextPrefixIndex - 1);
         }

         for (int index = 0;index < m_nextHeaderIndex;index++)
         {
            buf += m_headerStack.GetAt(index);
         }

         buf += m_buf;

         numWritten = buf.GetLength();
         write(buf);
      }
   }

   restoreWaitCursor();

   return numWritten;
}

int CWriteFormat::writef(int prefixIndex,const char* format,...)
{
   va_list args;
   va_start(args,format);

   return writef(prefixIndex,format,args);
}

int CWriteFormat::writef(int prefixIndex,const char* format,va_list args)
{
   int numWritten = 0;

   if (m_sectionEnabled && getPriority() >= getPriorityFence())
   {
      m_buf.FormatV(format, args);

      if (m_nextHeaderIndex == 0 && m_nextPrefixIndex == 0 && m_prefixArray.GetSize() < 1)
      {
         numWritten = m_buf.GetLength();
         write(m_buf);
      }
      else
      {
         CString buf;

         if (prefixIndex >= 0 && prefixIndex < m_prefixArray.GetSize())
         {
            buf = m_prefixArray.GetAt(prefixIndex);
            int count = m_prefixCount.GetAt(prefixIndex) + 1;
            m_prefixCount.SetAt(prefixIndex,count);
         }

         if (m_nextPrefixIndex > 0)
         {
            buf += m_prefixStack.GetAt(m_nextPrefixIndex - 1);
         }

         for (int index = 0;index < m_nextHeaderIndex;index++)
         {
            buf += m_headerStack.GetAt(index);
         }

         buf += m_buf;

         numWritten = buf.GetLength();
         write(buf);
      }
   }

   restoreWaitCursor();

   return numWritten;
}

void CWriteFormat::flush()
{
}

int CWriteFormat::getPriority()
{
   return m_priorityStack.GetAt(m_priorityIndex);
}

int CWriteFormat::getPriorityFence()
{
   return m_priorityFenceStack.GetAt(m_priorityFenceIndex);
}

int CWriteFormat::setPriority(int priority)
{
   int retval = m_priorityStack.GetAt(m_priorityIndex);
   m_priorityStack.SetAtGrow(m_priorityIndex,priority);

   return retval;
}

int CWriteFormat::setPriorityFence(int priorityFence)
{
   int retval = m_priorityFenceStack.GetAt(m_priorityFenceIndex);
   m_priorityFenceStack.SetAtGrow(m_priorityFenceIndex,priorityFence);

   return retval;
}

int CWriteFormat::pushPriority(int priority)
{
   int retval = m_priorityStack.GetAt(m_priorityIndex);
   m_priorityIndex++;
   m_priorityStack.SetAtGrow(m_priorityIndex,priority);

   return retval;
}

int CWriteFormat::pushPriorityFence(int priorityFence)
{
   int retval = m_priorityFenceStack.GetAt(m_priorityFenceIndex);
   m_priorityFenceIndex++;
   m_priorityFenceStack.SetAtGrow(m_priorityFenceIndex,priorityFence);

   return retval;
}

int CWriteFormat::pushPriorityIncrement(int priorityIncrement)
{
   return pushPriority(getPriority() + priorityIncrement);
}

int CWriteFormat::pushPriorityFenceIncrement(int priorityFenceIncrement)
{
   return pushPriorityFence(getPriorityFence() + priorityFenceIncrement);
}

int CWriteFormat::popPriority()
{
   int retval = m_priorityStack.GetAt(m_priorityIndex);

   if (m_priorityIndex == 0)
   {
writef("popPriority() underflow.\n");
   }
   else
   {
      m_priorityIndex--;
   }

   return retval;
}

int CWriteFormat::popPriorityFence()
{
   int retval = m_priorityFenceStack.GetAt(m_priorityFenceIndex);

   if (m_priorityFenceIndex == 0)
   {
writef("popPriorityFence() underflow.\n");
   }
   else
   {
      m_priorityFenceIndex--;
   }

   return retval;
}

void CWriteFormat::setSection(const CString& sectionName)
{
   m_sections.SetAt(sectionName,NULL);
   calcSectionEnabled();
}

void CWriteFormat::clrSection(const CString& sectionName)
{
   m_sections.RemoveKey(sectionName);
   calcSectionEnabled();
}

void CWriteFormat::pushSection(const CString& sectionName)
{
   if (m_sectionIndex > 0)
   {
      m_sections.RemoveKey(m_sectionStack.GetAt(m_sectionIndex - 1));
   }

   m_sectionStack.SetAtGrow(m_sectionIndex++,sectionName);
   m_sections.SetAt(sectionName,NULL);
   calcSectionEnabled();
}

void CWriteFormat::popSection(const CString& sectionName)
{
   if (m_sectionIndex > 0)
   {
      m_sections.RemoveKey(m_sectionStack.GetAt(--m_sectionIndex));
      m_sectionStack.SetSize(m_sectionIndex);

      if (m_sectionIndex > 0)
      {
         m_sections.SetAt(m_sectionStack.GetAt(m_sectionIndex - 1),NULL);
      }

      calcSectionEnabled();
   }
   else
   {
writef("popSection() underflow.\n");
   }
}

void CWriteFormat::enableSection(const CString& sectionName)
{
   if (!m_defaultEnabled)
   {
      m_markedSections.SetAt(sectionName,NULL);
   }
   else
   {
      m_markedSections.RemoveKey(sectionName);
   }

   calcSectionEnabled();
}

void CWriteFormat::disableSection(const CString& sectionName)
{
   if (m_defaultEnabled)
   {
      m_markedSections.SetAt(sectionName,NULL);
   }
   else
   {
      m_markedSections.RemoveKey(sectionName);
   }

   calcSectionEnabled();
}

void CWriteFormat::calcSectionEnabled()
{
   m_sectionEnabled = m_defaultEnabled;
   CString sectionName;
   CObject* junk;

   for (POSITION pos = m_sections.GetStartPosition();pos != NULL;)
   {
      m_sections.GetNextAssoc(pos,sectionName,junk);

      if (m_markedSections.Lookup(sectionName,junk))
      {
         m_sectionEnabled = !m_defaultEnabled;
         break;
      }
   }
}

int CWriteFormat::pushHeader(const CString& header)
{
   m_headerStack.SetAtGrow(m_nextHeaderIndex,header);

   return ++m_nextHeaderIndex;
}

int CWriteFormat::popHeader()
{
   if (m_nextHeaderIndex == 0)
   {
writef("popHeader() underflow.\n");
   }
   else
   {
      m_nextHeaderIndex--;
   }

   return m_nextHeaderIndex;
}

int CWriteFormat::pushPrefix(const CString& prefix)
{
   m_prefixStack.SetAtGrow(m_nextPrefixIndex,prefix);

   return ++m_nextPrefixIndex;
}

int CWriteFormat::popPrefix()
{
   if (m_nextPrefixIndex == 0)
   {
writef("popPrefix() underflow.\n");
   }
   else
   {
      m_nextPrefixIndex--;
   }

   return m_nextPrefixIndex;
}

void CWriteFormat::setPrefix(int index,const CString& prefix)
{
   m_prefixArray.SetAtGrow(index,prefix);
   m_prefixCount.SetAtGrow(index,0);
}

CString CWriteFormat::getPrefix(int index)
{
   CString retval;

   if (index >= 0 && index < m_prefixArray.GetSize())
   {
      retval = m_prefixArray.GetAt(index);
   }

   return retval;
}

int CWriteFormat::getPrefixCount(int index)
{
   int retval = 0;

   if (index >= 0 && index < m_prefixCount.GetSize())
   {
      retval = m_prefixCount.GetAt(index);
   }

   return retval;
}

int CWriteFormat::setPrefixCount(int index,int value)
{
   int retval = getPrefixCount(index);

   m_prefixCount.SetAtGrow(index,value);

   return retval;
}

CString CWriteFormat::getFilePath()
{
   CString retval;

   return retval;
}

CCmdTarget* CWriteFormat::registerWaitCursorCmdTarget(CCmdTarget* cmdTarget)
{
   CCmdTarget* retval = m_waitCursorCmdTarget;

   if (m_waitCursorCmdTarget == NULL)
   {
      m_waitCursorCmdTarget = cmdTarget;
   }

   return retval;
}

void CWriteFormat::withdrawWaitCursorCmdTarget(CCmdTarget* cmdTarget)
{
   if (cmdTarget == m_waitCursorCmdTarget)
   {
      m_waitCursorCmdTarget = NULL;
   }
}

void CWriteFormat::restoreWaitCursor()
{
   if (m_waitCursorCmdTarget != NULL)
   {
      m_waitCursorCmdTarget->RestoreWaitCursor();
   }
}

//_____________________________________________________________________________
CNullWriteFormat::CNullWriteFormat()
{
}

void CNullWriteFormat::write(const char* string)
{
}

//=============================================================================
/*
History: WriteFormat.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Kvanness     Date: 2/06/00    Time: 11:58a
 * Updated in $/LibKnvPP
 * Added CExtFileException parameter to open()
 * 
 * *****************  Version 7  *****************
 * User: Kvanness     Date: 9/16/99    Time: 6:53a
 * Updated in $/LibKnvPP
 * Added support for bufSize control.
 * 
 * *****************  Version 6  *****************
 * User: Kurtv        Date: 5/03/99    Time: 8:29a
 * Updated in $/LibKnvPP
 * Added close()
 * 
 * *****************  Version 5  *****************
 * User: Kurtv        Date: 8/11/98    Time: 11:22a
 * Updated in $/LibKnvPP
 * changed CStdioFileWriteFormat::open() from BOOL to bool.
 * Used formatMessageBox() instead of AfxMessageBox().
 * 
 * *****************  Version 4  *****************
 * User: Kurtv        Date: 11/14/97   Time: 8:13a
 * Updated in $/LibKnvPP
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 9/08/97    Time: 11:56a
 * Updated in $/LibKnvPP
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 8/15/97    Time: 10:08a
 * Updated in $/CadTransfer
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 8/02/97    Time: 11:50p
 * Created in $/CadTransfer
 * Initial add
*/

//#include "StdAfx.h"
//#include "StdioFileWriteFormat.h"
//#include "LibKnvPP.h"
//#include "ExtFileException.h"
//
////IMPLEMENT_DYNCREATE(CTransferLogDocWriteFormat, CWriteFormat)
//
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

//_____________________________________________________________________________
CStdioFileWriteFormat::CStdioFileWriteFormat(int bufSize)
: CWriteFormat(bufSize)
{
   m_allocatedStdioFile = NULL;
   m_stdioFile = NULL;
}

CStdioFileWriteFormat::CStdioFileWriteFormat(CStdioFile* stdioFile,int bufSize) : CWriteFormat(bufSize)
{
   m_allocatedStdioFile = NULL;
   m_stdioFile = stdioFile;
}

CStdioFileWriteFormat::CStdioFileWriteFormat(const CString& filePath,int bufSize) : CWriteFormat(bufSize)
{
   m_allocatedStdioFile = NULL;
   m_stdioFile = NULL;
   open(filePath);
}

CStdioFileWriteFormat::~CStdioFileWriteFormat()
{
   close();
}

void CStdioFileWriteFormat::close()
{
   if (m_stdioFile != NULL)
   {
      m_stdioFile->Close();
   }

   delete m_allocatedStdioFile;
   m_allocatedStdioFile = NULL;
   m_stdioFile          = NULL;
   m_filePath.Empty();
}

void CStdioFileWriteFormat::flush()
{
   if (m_stdioFile != NULL)
   {
      m_stdioFile->Flush();
   }
}

bool CStdioFileWriteFormat::open(const CString& filePath,CExtFileException* exception)
{
   bool retval = false;

   m_filePath = filePath;
   delete m_allocatedStdioFile;

   if (m_filePath.IsEmpty())
   {
      m_allocatedStdioFile = NULL;
   }
   else
   {
      m_allocatedStdioFile = new CStdioFile();
      CExtFileException localException;
      CExtFileException* pLocalException = ((exception == NULL) ? &localException : exception);

      UINT mode = CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone;
      retval = (m_allocatedStdioFile->Open(filePath,mode,pLocalException) != 0);

      if (!retval)
      {
         CString reason;

         if (exception == NULL)
         {
            reason.Format(", '%s'",(const char*)pLocalException->getCause());
         }

         formatMessageBox(MB_OK,"Could not open the file '%s' for writing%s.",
            (const char*)filePath,
            (const char*)reason);

         delete m_allocatedStdioFile;
         m_allocatedStdioFile = NULL;
      }
   }

   m_stdioFile = m_allocatedStdioFile;

   return retval;
}

CString CStdioFileWriteFormat::getFilePath()
{
   return m_filePath;
}

void CStdioFileWriteFormat::write(const char* string)
{
   if (m_stdioFile != NULL)
   {
      m_stdioFile->WriteString(string);
   }
}

void CStdioFileWriteFormat::writefTimeStamp(const char* format,...)
{
   va_list args;
   va_start(args,format);

   CTime dateTime = CTime::GetCurrentTime();   

   writef("%s - ",dateTime.Format("%Y%m%d.%H%M%S"));
   writef(format,args);
}

//=============================================================================

//_____________________________________________________________________________
CStreamFileWriteFormat::CStreamFileWriteFormat(FILE* file,int bufSize) : CWriteFormat(bufSize)
{
   m_file = file;
}

CStreamFileWriteFormat::~CStreamFileWriteFormat()
{
}

void CStreamFileWriteFormat::write(const char* string)
{
   if (m_file!= NULL)
   {
      fputs(string,m_file);
   }
}

//=============================================================================

/*
History: WriteFormat.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 4/23/98    Time: 2:21p
 * Created in $/PadsDRC
 * Initial add.
*/

//#include "StdAfx.h"
//#include "TraceFormat.h"
//
//IMPLEMENT_DYNAMIC(CTraceFormat,CWriteFormat);
//   
//#ifdef USE_DEBUG_NEW
//#define new DEBUG_NEW
//#endif

//_____________________________________________________________________________
CTraceFormat::CTraceFormat(int bufSize) : CWriteFormat(bufSize)
{
   //disableSection("CEdgeStringCycle::removeSubsets");
   //disableSection("CEdgeStringCycle::referenceIfInterior");
   //disableSection("CGraph::getPlaneAreas");
   //disableSection("CGraph::getEdgeCycles");
   //disableSection("CPowerPlane::addEdgeString");
   //disableSection("CPowerPlane::calculateAreas");
   //disableSection("CPowerPlanes::addEdgeString");
   //disableSection("CPowerPlanes::addVoidEdgeStringCycle");
   //disableSection("CPowerPlanes::addBoardBoundary");
   //disableSection("CEdgeString::selfDisect");
   //disableSection("CEdgeString::isConnectivityValid");
}

void CTraceFormat::write(const char* string)
{
   TRACE(string);
}

void CTraceFormat::setFromFile(const CString& path)
{
   CStdioFile file;
   CString sectionName;

   if (file.Open(path,CFile::modeRead))
   {
      while (file.ReadString(sectionName))
      {
         enableSection(sectionName);
      }
   }
}

//=============================================================================

//_____________________________________________________________________________
CLogWriteFormat::CLogWriteFormat(int bufSize) : CStdioFileWriteFormat(bufSize)
{
   initPrefixes();
}

CLogWriteFormat::CLogWriteFormat(CStdioFile* stdioFile,int bufSize) : CStdioFileWriteFormat(stdioFile,bufSize)
{
   initPrefixes();
}

CLogWriteFormat::CLogWriteFormat(const CString& filePath,int bufSize) : CStdioFileWriteFormat(filePath,bufSize)
{
   initPrefixes();
}

CLogWriteFormat::~CLogWriteFormat()
{
}

void CLogWriteFormat::initPrefixes()
{
   setPrefix(PrefixError   ,"*** Error *** ");
   setPrefix(PrefixCaution ,"*** Caution *** ");
   setPrefix(PrefixWarning ,"*** Warning *** ");
   setPrefix(PrefixDisaster,"*** Disaster *** ");
   setPrefix(PrefixStatus  ,"Status: ");
   setPrefix(PrefixTask    ,"Task: ");
   setPrefix(PrefixSummary ,"Summary: ");
   setPrefix(PrefixSuccess ,"Success: ");
   setPrefix(PrefixFailure ,"Failure: ");
   setPrefix(PrefixSeverity1 ,getPrefix(PrefixWarning));
   setPrefix(PrefixSeverity2 ,getPrefix(PrefixError));
   setPrefix(PrefixSeverity3 ,getPrefix(PrefixDisaster));
}

//=============================================================================

/*
History: WriteFormat.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Kvanness     Date: 9/11/00    Time: 11:33p
 * Created in $/LibKnvPP
 * Initial add.
*/

//#include "StdAfx.h"
//#include "MultipleWriteFormat.h"
//
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

//_____________________________________________________________________________
CMultipleWriteFormat::CMultipleWriteFormat(int bufSize) : CWriteFormat(bufSize)
{
}

CMultipleWriteFormat::~CMultipleWriteFormat()
{
   empty();
}

void CMultipleWriteFormat::empty()
{
   m_writeFormatList.RemoveAll();
}

bool CMultipleWriteFormat::attach(CWriteFormat* writeFormat)
{
   bool retval = false;

   if (writeFormat != this && writeFormat != NULL)
   {
      POSITION pos = m_writeFormatList.Find(writeFormat);

      if (pos == NULL)
      {
         m_writeFormatList.AddTail(writeFormat);
         retval = true;
      }
   }

   return retval;
}

bool CMultipleWriteFormat::detach(CWriteFormat* writeFormat)
{
   bool retval = false;

   POSITION pos = m_writeFormatList.Find(writeFormat);

   if (pos != NULL)
   {
      m_writeFormatList.RemoveAt(pos);
      retval = true;
   }

   return retval;
}

void CMultipleWriteFormat::write(const char* string)
{
   CWriteFormat* writeFormat;

   for (POSITION pos = m_writeFormatList.GetHeadPosition();pos != NULL;)
   {
      writeFormat = m_writeFormatList.GetNext(pos);
      writeFormat->write(string);
   }
}

//=============================================================================

/*
History: WriteFormat.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Knv          Date: 6/14/01    Time: 3:51p
 * Created in $/knv/BibTool
 * Initial add.
*/

//#include "StdAfx.h"
//#include "HtmlFileWriteFormat.h"
//#include "LibKnvPP.h"
//#include "ExtFileException.h"

//IMPLEMENT_DYNCREATE(CHtmlFileWriteFormat, CStdioFileWriteFormat)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString convertToNonBreakingSpaces(const CString& textString)
{
   CString retval(textString);

   retval.Replace("  "," &nbsp;");

   return retval;
}

//_____________________________________________________________________________
CHtmlFileWriteFormat::CHtmlFileWriteFormat(int bufSize) : 
   CStdioFileWriteFormat(bufSize)
{
   initPrefixes();
   m_currentColor = colorBlack;
   m_formattedTextMode = true;
}

CHtmlFileWriteFormat::CHtmlFileWriteFormat(CStdioFile* stdioFile,int bufSize) : 
   CStdioFileWriteFormat(stdioFile,bufSize)
{
   initPrefixes();
   m_currentColor = colorBlack;
   m_formattedTextMode = true;
}

CHtmlFileWriteFormat::CHtmlFileWriteFormat(const CString& filePath,int bufSize) : 
   CStdioFileWriteFormat(filePath,bufSize)
{
   initPrefixes();
   m_currentColor = colorBlack;
   m_formattedTextMode = true;
}

CHtmlFileWriteFormat::~CHtmlFileWriteFormat()
{
   close();
}

void CHtmlFileWriteFormat::initPrefixes()
{
   setPrefix(PrefixError   ,"*** Error *** ");
   setPrefix(PrefixCaution ,"*** Caution *** ");
   setPrefix(PrefixWarning ,"*** Warning *** ");
   setPrefix(PrefixDisaster,"*** Disaster *** ");
   setPrefix(PrefixStatus  ,"Status: ");
   setPrefix(PrefixTask    ,"Task: ");
   setPrefix(PrefixSummary ,"Summary: ");
   setPrefix(PrefixSuccess ,"Success: ");
   setPrefix(PrefixFailure ,"Failure: ");
   setPrefix(PrefixSeverity1 ,getPrefix(PrefixWarning));
   setPrefix(PrefixSeverity2 ,getPrefix(PrefixError));
   setPrefix(PrefixSeverity3 ,getPrefix(PrefixDisaster));
}

CRecordParser& CHtmlFileWriteFormat::getRecordParser()
{
   return m_recordParser;
}

void CHtmlFileWriteFormat::close()
{
   if (m_formattedTextMode)
   {
      CStdioFileWriteFormat::write("</pre>\n");
   }

   CStdioFileWriteFormat::write(
"</font>\n"
"</body>\n"
"</html>\n"
);

   CStdioFileWriteFormat::close();
}

bool CHtmlFileWriteFormat::open(const CString& filePath,CExtFileException* exception)
{
   bool retval = CStdioFileWriteFormat::open(filePath,exception);

   if (retval)
   {
      CStdioFileWriteFormat::write(
"<html>\n"
"<body bgcolor=silver style=\"font-family: Courier; font-size: 8pt\">\n"
"<font color=\"Black\">\n"
);
   }

   if (m_formattedTextMode)
   {
      CStdioFileWriteFormat::write("<pre>\n");
   }

   return retval;
}

void CHtmlFileWriteFormat::write(const char* string)
{
   COLORREF color = m_recordParser.getRecordColor(string);

   if (color != m_currentColor)
   {
      CString colorString;
      colorString.Format("</font><font color=\"#%02x%02x%02x\">",
         GetRValue(color),GetGValue(color),GetBValue(color));
      CStdioFileWriteFormat::write(colorString);

      m_currentColor = color;
   }

   if (m_formattedTextMode)
   {
      CStdioFileWriteFormat::write(string);
   }
   else
   {
      CString line;
      line = convertToNonBreakingSpaces(string);
      line.Replace("\n","<br>\n");
      CStdioFileWriteFormat::write(line);
   }

// Flush();
}   

//_____________________________________________________________________________
CStdioFileWriteFormat* CDebugWriteFormat::s_writeFormat = NULL;
CNullWriteFormat* CDebugWriteFormat::s_nullWriteFormat = NULL;
CString CDebugWriteFormat::m_filePath;
bool CDebugWriteFormat::m_enabled = true;

CWriteFormat& CDebugWriteFormat::getWriteFormat()
{
   CWriteFormat* writeFormat;

   if (m_enabled)
   {
      if (s_writeFormat == NULL)
      {
         if (m_filePath.IsEmpty())
         {
            m_filePath = "c:\\CDebugWriteFormat.txt";
         }

         s_writeFormat = new CStdioFileWriteFormat(m_filePath,512);
         s_writeFormat->setNewLineMode(true);
      }

      writeFormat = s_writeFormat;
   }
   else
   {
      if (s_nullWriteFormat == NULL)
      {
         s_nullWriteFormat = new CNullWriteFormat();
      }

      writeFormat = s_nullWriteFormat;
   }

   return *writeFormat;
}

void CDebugWriteFormat::close()
{
   if (s_writeFormat != NULL)
   {
      s_writeFormat->flush();
      s_writeFormat->close();
   }

   delete s_writeFormat;
   s_writeFormat = NULL;

   delete s_nullWriteFormat;
   s_nullWriteFormat = NULL;
}

void CDebugWriteFormat::setFilePath(const CString& filePath)
{
   if (m_filePath.CompareNoCase(filePath) != 0)
   {
      close();
   }

   m_filePath = filePath;
}

bool CDebugWriteFormat::enable(bool enableFlag)
{
   bool retval = m_enabled;

   m_enabled = enableFlag;

   return retval;
}

//_____________________________________________________________________________
CStringWriteFormat::CStringWriteFormat(int bufSize) :
   CWriteFormat(bufSize)
{
}

void CStringWriteFormat::write(const char* string)
{
   m_buf += string;
}

//_____________________________________________________________________________
CMessageBoxWriteFormat::CMessageBoxWriteFormat(int bufSize) :
   CStringWriteFormat(bufSize)
{
}

CMessageBoxWriteFormat::~CMessageBoxWriteFormat()
{
   if (!m_buf.IsEmpty())
   {
      AfxMessageBox(m_buf);
   }
}

//=============================================================================

/*
History: RwLib.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 4/22/98    Time: 9:17a
 * Created in $/LibKnvPP
 * Initial add
*/

//#include "Stdafx.h"
//#include "ExtFileException.h"
//
////IMPLEMENT_DYNCREATE(CTransferLogDocWriteFormat, CWriteFormat)
//
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

//_____________________________________________________________________________
CString CExtFileException::getCause()
{  
   CString retval;

   switch(m_cause)
   {
      case CFileException::none: retval = "No error occurred.";  
         break;
#if _MSC_VER >= 1400
      case CFileException::genericException:
#else
      case CFileException::generic: 
#endif
         retval = "An unspecified error occurred.";  
         break;
      case CFileException::fileNotFound: retval = "The file could not be located.";  
         break;
      case CFileException::badPath: retval = "All or part of the path is invalid.";  
         break;
      case CFileException::tooManyOpenFiles: retval = "The permitted number of open files was exceeded.";  
         break;
      case CFileException::accessDenied: retval = "The file could not be accessed.";  
         break;
      case CFileException::invalidFile: retval = "There was an attempt to use an invalid file handle.";  
         break;
      case CFileException::removeCurrentDir: retval = "The current working directory cannot be removed.";  
         break;
      case CFileException::directoryFull: retval = "There are no more directory entries.";  
         break;
      case CFileException::badSeek: retval = "There was an error trying to set the file pointer.";  
         break;
      case CFileException::hardIO: retval = "There was a hardware error.";  
         break;
      case CFileException::sharingViolation: retval = "SHARE.EXE was not loaded, or a shared region was locked.";  
         break;
      case CFileException::lockViolation: retval = "There was an attempt to lock a region that was already locked.";  
         break;
      case CFileException::diskFull: retval = "The disk is full.";  
         break;
      case CFileException::endOfFile: retval = "The end of file was reached. ";  
         break;
      default: retval = "Unknown";  
         break;
   }

   return retval;
}

//=============================================================================

/*
History: RwLib.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Knv          Date: 9/15/00    Time: 3:29p
 * Created in $/knv/LibCad
 * Initial add to Auto-Graphics SourceSafe
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 5/04/99    Time: 6:06a
 * Updated in $/LibCad
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 5/03/99    Time: 7:02a
 * Created in $/LibCad
 * Initial add.
*/

//#include "StdAfx.h"
//#include "RecordParser.h"
//#include "SupString.h"
//#include "LibCad.h"
//#include "Colors.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char BASED_CODE THIS_FILE[] = __FILE__;
//#endif

IMPLEMENT_DYNCREATE(CRecordParser, CObject)
IMPLEMENT_DYNCREATE(CCadRecordParser, CObject)

//_______________________________________________________________________________
CRecordParser::CRecordParser()
{
   setDefaults();
}

CRecordParser::~CRecordParser()
{
}

void CRecordParser::setDefaults()
{
   setRecordPrefixAndColor(RecordTypeError    ,"*** Error"   ,colorRed);
   setRecordPrefixAndColor(RecordTypeWarning  ,"*** Warn"    ,colorDkCyan);
   setRecordPrefixAndColor(RecordTypeDisaster ,"*** Disaster",colorMagenta);
   setRecordPrefixAndColor(RecordTypeStatus   ,"Status:"     ,colorDkGreen);
   setRecordPrefixAndColor(RecordTypeTask     ,"Task:"       ,colorDkMagenta);
   setRecordPrefixAndColor(RecordTypeSummary  ,"Summary:"    ,colorBlue);
   setRecordPrefixAndColor(RecordTypeDebug    ,"Debug:"      ,colorDkGray);
   setRecordPrefixAndColor(RecordTypeColorTest,"Color Test:" ,colorBlack);
   setRecordPrefixAndColor(RecordTypeSuccess  ,"Success:"    ,colorGreen);
   setRecordPrefixAndColor(RecordTypeFailure  ,"Failure:"    ,colorMagenta);
}

void CRecordParser::setRecordPrefixAndColor(int recordType,const char* prefix,COLORREF color)
{
   setRecordPrefix(recordType,prefix);
   setRecordColor(recordType,color);
}

void CRecordParser::setRecordPrefix(int recordType,const char* prefix)
{
   m_recordPrefixes.SetAtGrow(recordType,prefix);
}

void CRecordParser::setRecordColor(int recordType,COLORREF color)
{
   m_recordColors.SetAtGrow(recordType,color);
}

int CRecordParser::getRecordType(const CString& record)
{
   int retval = -1;
   int length;

   for (int recordType = 0;recordType <= m_recordPrefixes.GetUpperBound();recordType++)
   {
      length = m_recordPrefixes.GetAt(recordType).GetLength();

      if (length != 0 && record.Left(length) == m_recordPrefixes.GetAt(recordType)   )
      {
         retval = recordType;
         break;
      }
   }

   return retval;
}

COLORREF CRecordParser::getRecordColor(int recordType,const CString& record)
{
   COLORREF retval = colorBlack;

   if (recordType >= 0 && recordType <= m_recordColors.GetUpperBound())
   {
      retval = m_recordColors.GetAt(recordType);

      if (recordType == RecordTypeColorTest)
      {
         int rVal,gVal,bVal;
         CSupString values;
         CStringArray params;

         values = record.Mid(11);
         values.Parse(params);

         rVal = atoi(params[0]);
         gVal = atoi(params[1]);
         bVal = atoi(params[2]);

         retval = RGB(rVal,gVal,bVal);
      }
   }

   return retval;
}

COLORREF CRecordParser::getRecordColor(const CString& record)
{
   int recordType = getRecordType(record);

   COLORREF retval = getRecordColor(recordType,record);

   return retval;
}

void CRecordParser::clear()
{
   m_recordColors.RemoveAll();
   m_recordPrefixes.RemoveAll();
}

//_______________________________________________________________________________
CCadRecordParser::CCadRecordParser()
{
   m_recordParser = &m_defaultRecordParser;
}

CCadRecordParser::~CCadRecordParser()
{
}

void CCadRecordParser::setRecordParser(CRecordParser* recordParser)
{
   m_recordParser = recordParser;

   if (m_recordParser == NULL)
   {
      m_recordParser = &m_defaultRecordParser;
   }
}


