
#include "StdAfx.h"
#include "DcaCamCadFileReader.h"
#include "DcaStdioZipFile.h"
#include "DcaLib.h"
#include "Dca.h"
#include "DcaCamCadData.h"
#include "DcaXmlContent.h"

#define VALUE_DELIMITERS "<>\""
#define DELIMITERS "<>=\"/ "

#define CALL(i) { FileStatusTag retVal = i; if (retVal != statusSucceeded) return retVal; }
//#define ImplementEsotericCcSections


//======================================================================================

///////////////////////////////////////////////////////////////////////////////
//  CFlexFile

#define USE_ONLY_ATTACHED_BUFFERS 1

CFlexFile::CFlexFile( eFileType ft )
    : m_CFileType       (ft) 
    , m_pStdioFile      (NULL)
#ifdef USE_PTR_2_STREAM
    , m_pMemFileStrm    (NULL)
#endif
    , m_openedFlags     (-1)
{
}

CFlexFile::~CFlexFile()
{
    if( m_pStdioFile )
        delete m_pStdioFile;
}

bool CFlexFile::AttachBuffer( const std::stringstream& fileString )
{
    m_CFileType = kCFileType_Memory;

#ifdef USE_PTR_2_STREAM
    try {
        m_pMemFileStrm = new std::stringstream;
        m_pMemFileStrm->str(pBuf);
    } catch (...) {
        return FALSE;
    }
    return !m_pMemFileStrm->fail();
#else
    m_MemFileStrm.str(fileString.str().c_str());
    return !m_MemFileStrm.fail();
#endif
}

BOOL CFlexFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pException)
{
    m_openedFileName = lpszFileName;
    m_openedFlags    = nOpenFlags;

    if( !lpszFileName )
        return FALSE;

    m_CFileType = kCFileType_Stdio;
    //if( m_CFileType == kCFileType_Stdio )
    //{
        if( !m_pStdioFile )
            m_pStdioFile = new CStdioFile();

        return m_pStdioFile->Open(lpszFileName, nOpenFlags, pException);
//    }
//    else if( m_CFileType == kCFileType_Memory )
//    {
//        ASSERT( !m_pStdioFile );
//
//#ifdef USE_ONLY_ATTACHED_BUFFERS
//        return FALSE;
//#else
//        // Temporary file load.  Need a file buffer setting API to replace this.
//        std::ifstream ifStream( lpszFileName );
//
//#ifdef USE_PTR_2_STREAM
//        try {
//            m_pMemFileStrm = new std::stringstream;
//
//            // Put the whole file into the stream
//            *m_pMemFileStrm << ifStream.rdbuf();
//        } catch (...) {
//            return FALSE;
//        }
//#else
//        m_MemFileStrm.str("");
//        // Put the whole file into the stream
//        m_MemFileStrm << ifStream.rdbuf();
//#endif
//#endif
//
//        return TRUE;
//    }
//
//    return FALSE;
}
// Keep
void CFlexFile::Close()
{
    m_openedFileName.Empty();
    m_openedFlags = -1;

    if( m_pStdioFile && m_CFileType == kCFileType_Stdio )
        m_pStdioFile->Close();

#ifdef USE_PTR_2_STREAM
    delete m_pMemFileStrm;
    m_pMemFileStrm = NULL;
#else
    m_MemFileStrm.str("");
#endif
}

void CFlexFile::SeekToBegin()
{
    if( m_pStdioFile && m_CFileType == kCFileType_Stdio )
    {
        //m_pStdioFile->SeekToBegin();  // Eliminate seeks
        m_pStdioFile->Close();
        BOOL bsts = Open(m_openedFileName.GetBuffer(0), m_openedFlags);
        return;
    }

    // Should be as simple as setting a pointer
#ifdef USE_PTR_2_STREAM
    if( m_pMemFileStrm )
        m_pMemFileStrm->seekg( 0, std::ios::beg );
#else
    m_MemFileStrm.seekg( std::ios::beg );
#endif
}

ULONGLONG CFlexFile::GetLength() const
{
    if( m_pStdioFile && m_CFileType == kCFileType_Stdio )
        return m_pStdioFile->GetLength();

    ULONGLONG size = 0;
#ifdef USE_PTR_2_STREAM
    if( m_pMemFileStrm )
        return m_pMemFileStrm->str().size();
#else
    size = m_MemFileStrm.str().size();
#endif

    return size;
}

ULONGLONG CFlexFile::GetPosition() const
{
    if( m_pStdioFile && m_CFileType == kCFileType_Stdio )
       return m_pStdioFile->GetPosition();

    ULONGLONG position = 0;
#ifdef USE_PTR_2_STREAM
    if( m_pMemFileStrm )
        return m_pMemFileStrm->str().size();
#else
    //*rcf Will not compile:  position = m_MemFileStrm.tellg();
    position = 1;
#endif

    return position;
}

UINT CFlexFile::Read(void* pBuf, UINT nCount)
{
    if( m_pStdioFile && m_CFileType == kCFileType_Stdio )
        return m_pStdioFile->Read( pBuf, nCount );

    if( !pBuf )
        return 0;

#ifdef USE_PTR_2_STREAM
    if( m_pMemFileStrm )
        m_pMemFileStrm->read( (char*)pBuf, nCount );
#else
    m_MemFileStrm.read( (char*)pBuf, nCount );
#endif

    return nCount;
}

BOOL CFlexFile::ReadString(CString& line)
{
    if( m_pStdioFile && m_CFileType == kCFileType_Stdio )
        return m_pStdioFile->ReadString(line);

    std::string sLine;
#ifdef USE_PTR_2_STREAM
    if( m_pMemFileStrm == NULL )
        return FALSE;

    getline( *m_pMemFileStrm, sLine );
    line = sLine.c_str();

    return !m_pMemFileStrm->fail();
#else
    getline( m_MemFileStrm, sLine );
    line = sLine.c_str();

    return !m_MemFileStrm.fail();
#endif
}

//======================================================================================


//_____________________________________________________________________________
CDcaXmlReader::CDcaXmlReader(CCamCadData& camCadData,CDcaOperationProgress* fileReadProgress)
: m_camCadData(camCadData)
, m_fileReadProgress(fileReadProgress)
, m_content(NULL)
{
   m_lineCnt = 0;
   m_charCnt = 0;
   //fileReadProgress = NULL;
   m_trackProgress  = false;
   m_file           = NULL;

   //content          = new CDcaXmlContent(camCadData);
}

CDcaXmlReader::~CDcaXmlReader()
{
   delete m_content;
   //delete fileReadProgress;
   delete m_file;
}

CDcaXmlContent& CDcaXmlReader::getDefinedContent()
{
   if (m_content == NULL)
   {
      m_content = new CDcaXmlContent(getCamCadData());
   }

   return *m_content;
}

void CDcaXmlReader::setFileReadProgress(CDcaOperationProgress* fileReadProgress)
{
   delete m_fileReadProgress;

   m_fileReadProgress = fileReadProgress;
}

//void CDcaXmlReader::setDoc(CCEtoODBDoc *doc)
//{
//   content = new XMLContent();
//   getDefinedContent().SetDoc(doc);
//}

void CDcaXmlReader::setTrackProgress(const CString& status)
{
   m_trackProgress          = true;
   m_fileReadProgressStatus = status;
}

FileStatusTag CDcaXmlReader::attachBuffer(const std::stringstream& fileString)
{
    FileStatusTag retval = statusUndefined;

    ERRMsg.Empty();

    if (m_file == NULL)
    {
        m_file = new CFlexFile();
    }

    bool bsts = m_file->AttachBuffer( fileString );

    return bsts ? checkFileHeader() : statusFileAttachFailure;
}

FileStatusTag CDcaXmlReader::open(const CString& filename)
{
   FileStatusTag retval = statusUndefined;

   ERRMsg.Empty();

   if (m_file == NULL)
   {
      m_file = new CFlexFile();
   }

    m_flags  = CFile::modeRead | CFile::shareDenyNone;
    m_flags |= CFile::osSequentialScan;   // no seeks
    m_flags |= CFile::typeText;//Binary;

   if (!m_file->Open(filename, m_flags))
   {
      retval = statusFileOpenFailure;
   }
   else
   {
      retval = checkFileHeader();
      
      if (retval == statusSucceeded)
         fileName = filename;
   }

   return retval;
}

FileStatusTag CDcaXmlReader::checkFileHeader()
{
   m_lastFilePos = 0;
   FileStatusTag retval = statusInvalidHeader;

   CString header("<?xml version ");
   CString headerBuf;
   int strLen = header.GetLength();

   if (m_file->Read(headerBuf.GetBufferSetLength(strLen), strLen))
   {
      headerBuf.GetBufferSetLength(strLen);
      int z = headerBuf.Find("<?xml");
      if (z > 0)
         headerBuf = headerBuf.Right(headerBuf.GetLength() - z );

      if (headerBuf.Left(5).CompareNoCase(header.Left(5)) == 0)
      {
         m_file->SeekToBegin();
         retval = statusSucceeded;
      }
      else
      {
         m_file->Close();
      }
   }

   return retval;
}
// Keep
FileStatusTag CDcaXmlReader::parse()
{
   FileStatusTag retval = statusFileOpenFailure;

   ERRMsg.Empty();

   if (m_file == NULL)
   {
      ErrorMessage(fileName, "File not opened", MB_ICONSTOP);
      return retval;
   }
   retval = XML();

   m_file->Close();

   if (retval != statusSucceeded)
   {
      if (retval != statusUserCancel)
         ErrorMessage(ERRMsg);
   }
   return retval;
}

FileStatusTag CDcaXmlReader::GetLine()
{
   //*rcf Leaving this messing while in transition, in case we find we need the olde style back.
   //*rcf Clean this up soon.

   //if (true)//m_file->m_CFileType == kCFileType_Memory)
   {
      // CCZ Data DLL Style

      int bsts = m_file->ReadString( line );

      if (bsts)
      {
         m_lineCnt++;

         // m_file->GetPosition() is not working for the memfile, so we keep our
         // own char count in order to simulate/estimate the file position. It is only
         // for the progress bar, so doesn't have to be super accurate.
         m_charCnt += line.GetLength();

         if (m_fileReadProgress != NULL && (m_lineCnt % 2000) == 0)
         {
            m_fileReadProgress->setLength((double)m_file->GetLength());
            double position = m_charCnt; //0.;
            //if (m_file != NULL)
            //{
            //   position = (double)m_file->GetPosition();
            //}
            m_fileReadProgress->updateProgress(position);
         }
      }

      return bsts ? statusSucceeded : statusEof;
   }
#ifdef OLDE_STYLE_CAMCAD_WAY
   else
   {
      // Traditional CAMCAD style
      int readResult = 1;
      CString buf;
      ULONGLONG filePos = m_file->GetPosition(); //m_file->GetPosition();

      if (filePos != m_lastFilePos)
      {
         //reset the buffers
         fileBuf.Empty();
      }

      // keep reading until we find a newline character ('\n')
      int newLinePos = fileBuf.Find("\n");
      while (newLinePos < 0 && readResult)
      {
         readResult = m_file->Read(buf.GetBufferSetLength(BUFSIZE), BUFSIZE);
         buf.GetBufferSetLength(readResult);


         // if there is nothing more to read and nothing more to parse, we hit the end
         if (!readResult && fileBuf.IsEmpty())
            return statusEof;

         fileBuf += buf;
         newLinePos = fileBuf.Find("\n");
      }
      line = (newLinePos > 0)?fileBuf.Left(newLinePos):fileBuf;
      fileBuf = (newLinePos > 0)?fileBuf.Mid(newLinePos+1):"";

      m_lineCnt++;
      m_charCnt += line.GetLength();

      if (m_fileReadProgress != NULL && (m_lineCnt % 2000) == 0)
      {
         m_fileReadProgress->setLength((double)m_file->GetLength());
         double position = 0.;
         if (m_file != NULL)
         {
            position = (double)m_file->GetPosition();
         }
         m_fileReadProgress->updateProgress(position);
      }

      line.Trim();
      m_lastFilePos = m_file->GetPosition();
   }
#endif
   return statusSucceeded;


}

FileStatusTag CDcaXmlReader::GetElement()
{
   FileStatusTag retval = statusUndefined;

   curElement = "";

   while (true)
   {
      if (line.IsEmpty())
      {
         retval = GetLine();

         if (retval != statusSucceeded)
         {
            break;
         }
      }

      line.TrimLeft();
      line.TrimRight(); 

      if (line.IsEmpty())
         continue;
      
      int z = line.Find("<?xml");
      if (z > 0 )
         line = line.Right(line.GetLength() - z);

      int index = line.Find('>', 0);

      if (index == -1)
      {
         curElement += line;
         curElement += ' ';
         line = "";
         continue;
      }
      else
      {
         curElement += line.Left(index+1);
         line = line.Mid(index+1);

         {
            if (curElement[0] == '<')  
            {      
               curElement.Replace('\t', ' ');
               originalElement = curElement;

               return statusSucceeded;
            }
            else
            {
               ERRMsg.Format("Expected '<' in \"%s\" [line %d]\n%s", curElement, m_lineCnt, originalElement);
               return statusBadData;
            }
         }
      }
   }

   if (!curElement.IsEmpty())
   {
      ERRMsg.Format("Unexpected End OF File - \"%s\" [line %d]\n%s", curElement, m_lineCnt, originalElement);
      retval = statusUnexpectedEof;
   }
   else
   {
      retval = statusSucceeded;
   }

   return retval;
}

FileStatusTag CDcaXmlReader::XML()
{
   CALL(DEFENT());

   CALL(ENTLIST());

   return statusSucceeded;
}

FileStatusTag CDcaXmlReader::DEFENT()
{
   CALL(GetElement());

   if (curElement.Left(2) != "<?")
   {
      if (m_file != NULL)
      {
         m_file->SeekToBegin();
      }

      return statusSucceeded;
   }

   if (curElement.Right(2) != "?>")
   {
      return statusBadData;
   }

   return statusSucceeded;
}

FileStatusTag CDcaXmlReader::ENTLIST()
{
   FileStatusTag retval;

   while ((retval = GetElement()) == statusSucceeded)
   {
      if (curElement.IsEmpty()) // EOF
      {
         return statusSucceeded;
      }

//    if (curElement.Left(2) == "</") // end TAG.  Pop out
      if (curElement.GetLength() >= 2 && curElement[0] == '<' && curElement[1] == '/') // end TAG.  Pop out
      {
         int i = curElement.Find('>');
         curTag = curElement.Mid(2, i-2);
         return statusSucceeded;
      }

      CALL(ENT());
   } 

   // GetElement() failed
   return retval;
}

bool CDcaXmlReader::extendedEnt(FileStatusTag &status)
{
   status = statusSucceeded; // Is not a failure

   return false; // But no extended entity was parsed, so return false
}

FileStatusTag CDcaXmlReader::ENT()
{
   FileStatusTag extendedEntityStatus = statusSucceeded;

   // If comment, then handle it here
   // Miten -- If the line has comment handle it is handled in this condition
   if (curElement.Left(4).Compare("<!--") == 0)
   {
      int index1 = curElement.Find(SOURCE_PRODUCT_GRAPHIC);

      if (index1 < 0)
         index1 = curElement.Find(SOURCE_PRODUCT_PROFESSIONAL);

      if (index1 < 0)
         index1 = curElement.Find(SOURCE_PRODUCT_PCB_TRANSLATOR);

      if (index1 < 0)
         index1 = curElement.Find(SOURCE_PRODUCT_VISION);

      if (index1 > -1)
      {
         int index2 = curElement.Find(" - ", index1);

         if (index2 > -1)
         {
            CString sourceProduct;
            int sourceVersionMajor = 0;
            int sourceVersionMinor = 0;
            int sourceVersionRevision = 0;

            CString tmpStr;
            curElement = curElement.Mid(index1, index2-index1).Trim();
            index1 = curElement.ReverseFind('.');

            if (index1 > -1)
            {
               tmpStr = curElement.Right(curElement.GetLength()-index1-1);
               sourceVersionRevision = atoi(tmpStr);
               curElement.Delete(index1, curElement.GetLength()-index1);
            }

            index1 = curElement.ReverseFind('.');

            if (index1 > -1)
            {
               tmpStr = curElement.Right(curElement.GetLength()-index1-1);
               sourceVersionMinor = atoi(tmpStr);
               curElement.Delete(index1, curElement.GetLength()-index1);
            }

            index1 = curElement.ReverseFind(' ');

            if (index1 > -1)
            {
               tmpStr = curElement.Right(curElement.GetLength()-index1-1);
               sourceVersionMajor = atoi(tmpStr);
               curElement.Delete(index1, curElement.GetLength()-index1);
            }
   
            sourceProduct = curElement.Trim();

            getCamCadData().setSourceProduct(sourceProduct);
            getCamCadData().setSourceVersionMajor(sourceVersionMajor);
            getCamCadData().setSourceVersionMinor(sourceVersionMinor);
            getCamCadData().setSourceVersionRevision(sourceVersionRevision);
         }
      }
      return statusSucceeded;
   }


   CALL(ENTBODY()); // [TAG][KWVALLIST]

   if (curElement.GetLength() >= 2 && curElement[0] == '/' && curElement[1] == '>') // single ent
   {
      BOOL ret = getDefinedContent().startElement(curTag, &kwvalMap); // Miten -- kwvalMap is a temp map which keeps a record of attributes for the current tag and then this temp map along with the tagname is given to the function to store it the actual data structure or map (m_map)
      getDefinedContent().endElement(curTag);

      kwvalMap.RemoveAll(); // Miten -- Once the attributes are added this temporary buffer is cleared

      if (!ret)
      {
         if (getDefinedContent().UserCancel)
            return statusUserCancel;
       
         ERRMsg.Format("User quit load process on current tag - \"%s\" [line %d]", curTag, m_lineCnt);
         return statusUnexpectedEof;
      }
   }
   else if (curTag == "CCNDB")
   {
      CMemFile ndbData(16384);
      CString tag("<CCNDB>\n");
      ndbData.Write(tag,tag.GetLength());

      while (true)
      {
         if (GetLine() != statusSucceeded)
         {
            ERRMsg.Format("Expected ending tag \"</CCNDB>\" [line %d]\n",m_lineCnt);
            break;
         }

         ndbData.Write(line,line.GetLength());
         ndbData.Write("\n",1);

         if (line.Trim() == "</CCNDB>")
         {
            ndbData.Write("\0",1);
            void* buf = ndbData.Detach();

//#if !defined(DisableDftModule)
//            getDefinedContent().getCamCadData().pCDFT->CacheDFTData((const char*)buf);
//#endif

            free(buf);
            line.Empty();

            break;
         }
      }
   }
   else if(extendedEnt(extendedEntityStatus))
   {
      return extendedEntityStatus;
   }
   else // has sub entlist
   { 
      CString lTag = curTag;

      BOOL ret = getDefinedContent().startElement(curTag, &kwvalMap); // Miten -- Value in the kwvalMap is passed to this function 

      kwvalMap.RemoveAll();

      if (!ret)
      {
         getDefinedContent().endElement(curTag);

         if (getDefinedContent().UserCancel)
            return statusUserCancel;

         ERRMsg.Format("User quit load process on current tag - \"%s\" [line %d]", curTag, m_lineCnt);
         return statusUnexpectedEof;
      }

      CALL(ENTLIST());

      // make sure the end tag is correct otherwise there is an error
      if (curTag != lTag)
      {
         ERRMsg.Format("Expected ending tag \"%s\" [line %d]\n%s", lTag, m_lineCnt, originalElement);
         return statusBadData;
      }

      getDefinedContent().endElement(lTag);
   }

   return statusSucceeded;
}

FileStatusTag CDcaXmlReader::ENTBODY()
{
   int i;

   // <
   curElement.TrimLeft(" <");

   // TAG
   if ((i = curElement.FindOneOf(DELIMITERS)) == -1)
   {
      ERRMsg.Format("Expected tag after '<' in \"%s\" [line %d]\n%s", curElement, m_lineCnt, originalElement);
      return statusBadData;
   }

   // Miten -- Get the current tag name
   curTag = curElement.Left(i);

// if (curElement.Mid(i, 2) == "/>")
   if (curElement.GetLength() >= 2 && curElement[i] == '/' && curElement[i+1] == '>')
      return statusSucceeded;

   curElement = curElement.Mid(i+1);
   curElement.TrimLeft();

   while (!curElement.IsEmpty() && curElement[0] != '>' && curElement[0] != '/')
   {
      // KEYWORD
      if ((i = curElement.FindOneOf(DELIMITERS)) == -1 || curElement[i] != '=')
      {
         ERRMsg.Format("Expected '=' [line %d]\n%s", m_lineCnt, originalElement);
         return statusBadData;
      }

      curKeyword = curElement.Left(i);
      curElement = curElement.Mid(i+1);
      curElement.TrimLeft();

      if (curElement[0] != '\"')
      {
         ERRMsg.Format("Expected \" after keyword \"%s\" [line %d]\n%s", curKeyword, m_lineCnt, originalElement);
         return statusBadData;
      }

      curElement = curElement.Mid(1);

      // VALUE
      if ((i = curElement.FindOneOf(VALUE_DELIMITERS)) == -1 || curElement[i] != '\"')
      {
         ERRMsg.Format("Expected \" [line %d]\n%s", m_lineCnt, originalElement);
         return statusBadData;
      }

      curValue = curElement.Left(i);
      curElement = curElement.Mid(i+1);
      curElement.TrimLeft();

      static CString temp;

      if (kwvalMap.Lookup(curKeyword, temp))
      {
         ERRMsg.Format("Duplicate keyword \"%s\" [line %d]\n%s", curKeyword, m_lineCnt, originalElement);
         return statusBadData;
      }

      curValue.Replace("&amp;", "&");
      curValue.Replace("&quot;", "\"");
      curValue.Replace("&apos;", "'");
      curValue.Replace("&lt;", "<");
      curValue.Replace("&gt;", ">");
      //curValue.Replace(char(4), char(32));
      int i;
      int startIndex = 0;
      int patternLength = 0;

      while ( (i=curValue.Find("&#",startIndex)) != -1)
      {
         CString rep;
         int t;
         const char* p = curValue;

         if (isdigit(p[i+2]))
         {
            patternLength = 3;

            if (isdigit(p[i+3]))  // double digit as "&#19;"
            {
               patternLength = 4;

               if (p[i+4] == ';')  // make sure the string ends in ";"
               {
                  patternLength = 5;

                  int val = p[i+2] - '0';
                  t = (10 * val) + p[i+3] - '0';            
                  rep = curValue.Mid(i, 5);
               }
            }
            else   // single digit as "&#5;"
            {
               if (p[i+3] == ';')   // make sure the string ends in ";"
               {
                  patternLength = 4;

                  t = p[i+2] - '0';
                  rep = curValue.Mid(i, 4);
               }
            }

            if (t < 32 && t != 9 && t != 10 && t != 13)
            {
               CString charStr = char(t);
               curValue.Replace (rep, charStr);

               startIndex += 1;
            }
            else
            {
               startIndex += patternLength;
            }
         }
         else
         {
            startIndex += 2;
         }
      }

      kwvalMap.SetAt(curKeyword, curValue);
   }

   return statusSucceeded;
}

//_____________________________________________________________________________
CDcaXmlZipReader::CDcaXmlZipReader(CCamCadData& camCadData,CamCadFileTypeTag camCadFileType,CDcaOperationProgress* fileReadProgress)
: CDcaXmlReader(camCadData)
{
   m_camCadFileType = camCadFileType;
   m_openStatus     = statusUndefined;

   if (camCadFileType == ccFileTypeXmlRsiZip)
   {
      m_zipFile = new CStdioRsiZipFile();
   }
   else
   {
      m_zipFile = new CStdioPkZipFile();
   }
}

CDcaXmlZipReader::~CDcaXmlZipReader()
{
   delete m_zipFile;
}

FileStatusTag CDcaXmlZipReader::open(const CString& filename)
{
   ERRMsg.Empty();

   m_openStatus = m_zipFile->openRead(filename);

   return m_openStatus;
}

FileStatusTag CDcaXmlZipReader::parse()
{
   FileStatusTag retval = statusFileOpenFailure;

   ERRMsg.Empty();

   //if (!getDefinedContent().GetDoc())
   //   return retval;

   //if (trackProgress)
   //{
   //   delete fileReadProgress;

   //   fileReadProgress = new CFileReadProgress(*m_zipFile);
   //   fileReadProgress->updateStatus(fileReadProgressStatus);
   //}

   retval = XML();

   m_zipFile->close();

   if (retval != statusSucceeded)
   {
      if (retval != statusUserCancel)
         ErrorMessage(ERRMsg);
   }

   return retval;
}

FileStatusTag CDcaXmlZipReader::GetLine()
{
   m_lineCnt++;

   //if (fileReadProgress != NULL && (lineCnt % 2000) == 0)
   //{
   //   fileReadProgress->updateProgress();
   //}

   return m_zipFile->readString(line);
}

//_____________________________________________________________________________



