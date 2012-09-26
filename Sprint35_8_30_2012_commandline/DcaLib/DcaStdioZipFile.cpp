// $Header: /CAMCAD/DcaLib/DcaStdioZipFile.cpp 2     6/04/07 5:21p Lynn Phung $

#include "StdAfx.h"
#include "DcaStdioZipFile.h"
#include "ZLib.h"

CString fileStatusTagToString(FileStatusTag fileStatusTag)
{
   CString retval("Undefined");

   switch (fileStatusTag)
   {
   case statusAlreadyOpen:                        retval = "statusAlreadyOpen";                        break;
   case statusBadData:                            retval = "statusBadData";                            break;
   case statusCompressEngineFailure:              retval = "statusCompressEngineFailure";              break;
   case statusCouldNotFindEndOfCentralDirectory:  retval = "statusCouldNotFindEndOfCentralDirectory";  break;
   case statusEof:                                retval = "statusEof";                                break;
   case statusFileOpenFailure:                    retval = "statusFileOpenFailure";                    break;
   case statusFileReadFailure:                    retval = "statusFileReadFailure";                    break;
   case statusInvalidHeader:                      retval = "statusInvalidHeader";                      break;
   case statusSucceeded:                          retval = "statusSucceeded";                          break;
   case statusTooManyDisks:                       retval = "statusTooManyDisks";                       break;
   case statusTooManyFiles:                       retval = "statusTooManyFiles";                       break;
   case statusUnexpectedEof:                      retval = "statusUnexpectedEof";                      break;
   case statusUndefined:                          retval = "statusUndefined";                          break;
   case statusUserCancel:                         retval = "statusUserCancel";                         break;
   case statusFileAttachFailure:                  retval = "statusFileAttachFailure";                  break;
   case statusFileDecryptionFailure:              retval = "statusFileDecryptionFailure";              break;
   case statusDecryptionAccessDenied:             retval = "statusDecryptionAccessDenied";             break;
   }

   return retval;
}

//_____________________________________________________________________________
CZStream::CZStream()
{
   m_zStream = new z_stream_s;

   m_zStream->zalloc = NULL;
   m_zStream->zfree  = NULL;
   m_zStream->opaque = NULL;
}

CZStream::~CZStream()
{
   delete m_zStream;
}

int CZStream::initDeflate(int level) 
{ 
   //return ::deflateInit_(m_zStream,level,ZLIB_VERSION,sizeof(z_stream)); 

   level = 6;
   int method = 8;
   int windowBits = -15;
   int memLevel = 8;
   int strategy = 0;
   return ::deflateInit2_(m_zStream,level,method,windowBits,memLevel,strategy,ZLIB_VERSION,sizeof(z_stream));  
}

int CZStream::deflate(int flush)     
{ 
   return ::deflate(m_zStream,flush);  
}

int CZStream::endDeflate()           
{ 
   return ::deflateEnd(m_zStream);  
}

int CZStream::initInflate()          
{ 
   int windowBits = -15;
   return ::inflateInit2_(m_zStream,windowBits,ZLIB_VERSION,sizeof(z_stream));  
}

int CZStream::inflate(int flush)     
{ 
   return ::inflate(m_zStream,flush);  
}

int CZStream::endInflate()           
{ 
   return ::inflateEnd(m_zStream);  
}

//_____________________________________________________________________________
CUnixCompressEngine::CUnixCompressEngine()
{
   m_allocatedSuffixTable = NULL;
   m_allocatedPrefixTable = NULL;
   m_suffixTable          = NULL;
   m_prefixTable          = NULL;

   //m_ht       = NULL;
   m_tokenBuf = NULL;

   m_oldmaxcode  = 0;
   //m_oldhashsize = 0;
   m_maxbits     = 13;
   m_blockCompress = 0;
   m_maxCode     = 0;
   m_offset      = 0;
   m_bits        = 0;

   m_compressEngineState = stateUninitialized;
}

CUnixCompressEngine::~CUnixCompressEngine()
{
   delete[] m_allocatedSuffixTable;
   delete[] m_allocatedPrefixTable;
   //delete[] m_ht;

   if (m_tokenBuf != NULL)
   {
      free(m_tokenBuf);
   }
}

//int CUnixCompressEngine::allocateArray(unsigned int maxcode,unsigned int hashsize)
//{
//}

bool CUnixCompressEngine::allocateTables(unsigned int maxcode,unsigned int hashsize)
{
   bool retval = true;

   //if (hashsize > m_oldhashsize) 
   //{
   //   delete[] m_ht;
   //   m_ht = NULL;

   //   m_oldhashsize = 0;
   //}

   if (maxcode > m_oldmaxcode) 
   {
      delete[] m_allocatedPrefixTable;
      delete[] m_allocatedSuffixTable;

      int offset = 256;
      m_allocatedSuffixTable  = new char[maxcode + 1 - offset];
      m_suffixTable = m_allocatedSuffixTable - offset;

      m_allocatedPrefixTable  = new unsigned short[maxcode + 1 - offset];
      m_prefixTable = m_allocatedPrefixTable - offset;

      m_oldmaxcode = maxcode;
   }

   //if (hashsize > m_oldhashsize) 
   //{
   //   delete[] m_ht;
   //   m_ht  = new unsigned short[hashsize];

   //   m_oldhashsize = hashsize;
   //}

   return retval;
}

UnixCompressEngineStatusTag CUnixCompressEngine::decompress(bool flushFlag)
{
   UnixCompressEngineStatusTag retval = statusCompressEngineStatusUndefined;

   bool inputProgress  = false;
   bool outputProgress = false; 
   UnixCompressEngineStateTag savedState,previousState = stateUndefined;

   for (bool loopFlag = true;loopFlag;)
   {
      savedState = m_compressEngineState;

      switch (m_compressEngineState)
      {
      case stateUninitialized:
         if (m_availableIn == 0)
         {
            loopFlag = false;
            break;
         }

         m_numBytesInput  = 0;
         m_numBytesOutput = 0;

         m_maxbits = (unsigned int)(*m_nextIn);
         m_availableIn -= 1;
         m_nextIn      += 1;
         m_numBytesInput++;
         inputProgress = true;

         m_blockCompress = (m_maxbits & 0x80);
         m_maxbits &= 0x1f;

         if (m_maxbits > m_maximumBits)
         {
            retval = statusInvalidBitCountError;
            loopFlag = false;
            break;
         }

         if (m_tokenBuf != NULL)
         {
            free(m_tokenBuf);
         }

         m_tokenBuf = (char*)malloc(m_tokenBufSize);

         m_maxCode = ~(~(unsigned int)0 << m_maxbits);

         allocateTables(m_maxCode,0);

         m_clearTable = true;
         m_saveCode = m_clearCode;
         m_offset = 0;
         m_prevBits = 0;
         m_maxTokenLen = 512;

         m_compressEngineState = stateClearTable;

         break;
      case stateClearTable:
         m_code = m_saveCode;

         if (m_code == m_clearCode && m_clearTable)
         {
            m_bits = 9;
            m_highCode = ~(~(unsigned int)0 << m_bits);
            m_fullTable = false;
            m_clearTable = (m_blockCompress != 0);
            m_nextFree = (m_blockCompress == 0 ? 256 : m_firstFreeCode);

            m_codeTypePrefix = true;
            m_compressEngineState = stateGetNextCode;
         }
         else
         {
            m_compressEngineState = stateBuildTokenString;
         }

         break;
      case stateGetNextCode:
         //If the next entry is a different bit-size than the preceeding one
         //then we must adjust the size and scrap the old buffer.
         if (m_prevBits != m_bits)
         {
            m_prevBits = m_bits;
            m_size     = 0;
         }

         m_shift = m_offset;

         //m_compressEngineState = stateCheckInbufForCode;

         //break;
         //If we can't read another code from the buffer, then refill it.
         if (m_size - m_shift < m_bits) 
         {
            m_inbufTransferCount = m_bits;
            m_pInbuf             = m_inBuf;

            m_compressEngineState = stateGetMoreBytes;
         }
         else
         {
            m_compressEngineState = stateProcessCode;
         }

         break;
      case stateGetMoreBytes:
         {
            int transferCount = ((m_inbufTransferCount <= m_availableIn) ? 
                                     m_inbufTransferCount : m_availableIn);

            memmove(m_pInbuf,m_nextIn,transferCount);
            m_availableIn        -= transferCount;
            m_nextIn             += transferCount;
            m_numBytesInput      += transferCount;
            inputProgress = (inputProgress || (transferCount > 0));

            int size = m_bits;

            if (m_inbufTransferCount != transferCount)
            {
               m_inbufTransferCount -= transferCount;
               m_pInbuf             += transferCount;

               if (flushFlag)
               {
                  size = m_pInbuf - m_inBuf;

                  if (size == 0)
                  {
                     loopFlag = false;
                     break;
                  }
               }
               else
               {
                  loopFlag = false;
                  break;
               }
            }

            m_size = size << 3;

            if (m_size <= 0)
            {
               retval = statusDataError;
               loopFlag = false;
               break;
            }

            m_offset = 0;
            m_shift  = 0;

            m_compressEngineState = stateProcessCode;
         }

         break;
      case stateProcessCode:
         {
            /* Get to the first byte. */
            unsigned char* bp = m_inBuf + (m_shift >> 3);

            /* Get first part (low order bits) */
            m_code = (*bp++ >> (m_shift &= 7));

            /* high order bits. */
            m_code |= *bp++ << (m_shift = 8 - m_shift);

            if ((m_shift += 8) < m_bits) 
            {
               m_code |= *bp << m_shift;
            }

            if (m_codeTypePrefix)
            {
               m_prefixCode = m_code & m_highCode;
            }
            else
            {
               m_saveCode = m_code & m_highCode;
            }

            m_offset += m_bits;

            m_compressEngineState = (m_codeTypePrefix ? stateCheckOutbufForPrefixCode : stateClearTable);
         }

         break;
      case stateCheckOutbufForPrefixCode:
         if (m_availableOut == 0)
         {
            loopFlag = false;
            break;
         }

         m_suffixChar = (char)m_prefixCode;
         *m_nextOut = m_prefixCode;
         m_nextOut++;
         m_availableOut--;
         m_numBytesOutput++;
         outputProgress = true;

         m_codeTypePrefix = false;
         m_compressEngineState = stateGetNextCode;

         break;
      case stateBuildTokenString:
         m_tokenCount = 0;

         if (m_code >= m_nextFree && !m_fullTable) 
         {
            if (m_code != m_nextFree)
            {
               // Non-existant code 
               retval = statusDataError;
               loopFlag = false;
               break;
            }

            // Special case for sequence KwKwK (see text of article)
            m_code = m_prefixCode;
            m_tokenBuf[m_tokenCount++] = m_suffixChar;
         }

         // Build the token string in reverse order by chasing down through
         // successive prefix tokens of the current token.  Then output it.
         while (m_code >= 256) 
         {
            // These are checks to ease paranoia. Prefix codes must decrease
            // monotonically, otherwise we must have corrupt tables.  We can
            // also check that we haven't overrun the token buffer.
            if (m_code <= m_prefixTable[m_code])
            {
               retval = statusCorruptedTableError;
               loopFlag = false;
               break;
            }

            if (m_tokenCount >= m_maxTokenLen) 
            {
               m_maxTokenLen *= 2;   /* double the size of the token buffer */

               m_tokenBuf = (char*)realloc(m_tokenBuf,m_maxTokenLen);

               if (m_tokenBuf == NULL) 
               {
                  retval = statusTokenBufAllocationError;
                  loopFlag = false;
                  break;
               }
            }

            m_tokenBuf[m_tokenCount++] = m_suffixTable[m_code];
            m_code = (unsigned int)m_prefixTable[m_code];
         }

         m_suffixChar = (char)m_code;

         m_compressEngineState = stateCheckOutbufForSuffix;

         break;
      case stateCheckOutbufForSuffix:
         if (m_availableOut == 0)
         {
            loopFlag = false;
            break;
         }

         *m_nextOut = m_suffixChar;
         m_nextOut++;
         m_availableOut--;
         m_numBytesOutput++;
         outputProgress = true;

         m_compressEngineState = stateCheckOutbufForToken;

         break;
      case stateCheckOutbufForToken:
         while (m_tokenCount > 0)
         {
            if (m_availableOut == 0)
            {
               loopFlag = false;
               break;
            }

            m_tokenCount--;

            *m_nextOut = m_tokenBuf[m_tokenCount];
            m_nextOut++;
            m_availableOut--;
            m_numBytesOutput++;
            outputProgress = true;
         }

         if (!loopFlag)
         {
            break;
         }

         // If table isn't full, add new token code to the table with
         // codeprefix and codesuffix, and remember current code.
         if (!m_fullTable) 
         {
            m_code = m_nextFree;

            if (m_code < 256 || m_code > m_maxCode)
            {
               retval = statusInvalidCodeError;
               loopFlag = false;
               break;
            }

            m_prefixTable[m_code] = m_prefixCode;
            m_suffixTable[m_code] = m_suffixChar;
            m_prefixCode  = m_saveCode;

            if (m_code++ == m_highCode) 
            {
               if (m_highCode >= m_maxCode) 
               {
                  m_fullTable = true;
                  --m_code;
               }
               else 
               {
                  ++m_bits;
                  m_highCode += m_code;           /* nextfree == highcode + 1 */
               }
            }

            m_nextFree = m_code;
         }

         m_codeTypePrefix = false;
         m_compressEngineState = stateGetNextCode;

         break;
      }

      previousState = savedState;
   }

   if (retval == statusCompressEngineStatusUndefined)
   {
      if (!inputProgress && !outputProgress)
      {
         if (m_availableIn == 0)
         {
            retval = statusEndOfData;
         }
         else
         {
            retval = statusBufferError;
         }
      }
      else
      {
         retval = statusSuccess;
      }
   }

   return retval;
}

//_____________________________________________________________________________
char CStdioUnixCompressedFileHeader::m_header[] = {'\x1f', '\x9d' };
const int CStdioUnixCompressedFileHeader::m_headerSize = sizeof(CStdioUnixCompressedFileHeader::m_header);

CStdioUnixCompressedFileHeader::CStdioUnixCompressedFileHeader()
{
   m_validFlag   = false;
}

FileStatusTag CStdioUnixCompressedFileHeader::readHeader(CFile& file)
{
   FileStatusTag retval = statusFileReadFailure;

   char headerBuf[m_headerSize];

   if (file.Read(headerBuf,m_headerSize) == m_headerSize)
   {
      m_validFlag = (memcmp(headerBuf,m_header,m_headerSize) == 0);
      retval = statusSucceeded;
   }

   return retval;
}

bool CStdioUnixCompressedFileHeader::writeHeader(CFile& file,const CString& fileName)
{
   bool retval = true;

   try
   {
      file.Write(m_header,m_headerSize);
   }
   catch (CException* e)
   {
      CString errorMessage;
      e->GetErrorMessage(errorMessage.GetBufferSetLength(128),128);
      retval = false;
   }

   return retval;
}

//_____________________________________________________________________________
char CStdioRsiZipFileHeader::m_header[] = {'c', 'c', 'z', '\01' };
const int CStdioRsiZipFileHeader::m_headerSize = sizeof(CStdioRsiZipFileHeader::m_header);

CStdioRsiZipFileHeader::CStdioRsiZipFileHeader()
{
   m_validFlag   = false;
}

FileStatusTag CStdioRsiZipFileHeader::readHeader(CFile& file)
{
   FileStatusTag retval = statusFileReadFailure;

   char headerBuf[m_headerSize];

   if (file.Read(headerBuf,m_headerSize) == m_headerSize)
   {
      m_validFlag = (memcmp(headerBuf,m_header,m_headerSize) == 0);
      retval = statusSucceeded;
   }

   return retval;
}

bool CStdioRsiZipFileHeader::writeHeader(CFile& file,const CString& fileName)
{
   bool retval = true;

   try
   {
      file.Write(m_header,m_headerSize);
   }
   catch (CException* e)
   {
      CString errorMessage;
      e->GetErrorMessage(errorMessage.GetBufferSetLength(128),128);
      retval = false;
   }

   return retval;
}

//_____________________________________________________________________________
//   +---+---+---+---+---+---+---+---+---+---+
//   |ID1|ID2|CM |FLG|     MTIME     |XFL|OS | (more-->)
//   +---+---+---+---+---+---+---+---+---+---+
#define FTEXT    0x01
#define FHCRC    0x02
#define FEXTRA   0x04
#define FNAME    0x08
#define FCOMMENT 0x10

char CStdioGzFileHeader::m_header[] = 
{'\x1f', '\x8b', '\x08', '\x08', '\x00', '\x00', '\x00', '\x00', '\x00', '\x0b' };
const int CStdioGzFileHeader::m_headerSize = sizeof(CStdioGzFileHeader::m_header);

CStdioGzFileHeader::CStdioGzFileHeader()
{
   m_validFlag = false;
}

FileStatusTag CStdioGzFileHeader::readHeader(CFile& file)
{
   m_compressedSize = (int)file.GetLength();

   FileStatusTag retval = statusFileReadFailure;
   m_validFlag = false;

   char headerBuf[m_headerSize];
   const int readHeaderCompareSize = 3;

   while (file.Read(headerBuf,m_headerSize) == m_headerSize)
   {
      //m_validFlag = (memcmp(headerBuf,m_header,readHeaderCompareSize) == 0);
      //retval = statusSucceeded;

      if (memcmp(headerBuf,m_header,readHeaderCompareSize) == 0)
      {
         m_compressedSize -= m_headerSize;

         unsigned char flags = headerBuf[3];

         if ((flags & FEXTRA) != 0)
         {
            char xlenBuf[2];

            if (file.Read(xlenBuf,2) != 2)
            {
               break;
            }

            m_compressedSize -= 2;

            int xlen = 256*xlenBuf[1] + xlenBuf[2];
            CString extraFieldBuf;         
            
            if (file.Read(extraFieldBuf.GetBufferSetLength(xlen),xlen) != xlen)
            {
               break;
            }

            m_compressedSize -= xlen;
         }

         if ((flags & FNAME) != 0)
         {
            char byte;
            int len;

            while ((len = file.Read(&byte,1)) == 1)
            {
               m_compressedSize--;

               if (byte == '\0')
               {
                  break;
               }
            }

            if (len != 1)
            {
               break;
            }
         }

         if ((flags & FCOMMENT) != 0)
         {
            char byte;
            int len;

            while ((len = file.Read(&byte,1)) == 1)
            {
               m_compressedSize--;

               if (byte == '\0')
               {
                  break;
               }
            }

            if (len != 1)
            {
               break;
            }
         }

         if ((flags & FHCRC) != 0)
         {
            char crcBuf[2];

            if (file.Read(crcBuf,2) != 2)
            {
               break;
            }

            m_compressedSize -= 2;
         }

         // 4 byte crc and 4 byte file size module 2^32
         //m_compressedSize -= 8;

         retval = statusSucceeded;
         m_validFlag = true;
      }

      break;
   }

   return retval;
}

bool CStdioGzFileHeader::writeHeader(CFile& file,const CString& fileName)
{
   bool retval = true;

   try
   {
      file.Write(m_header,m_headerSize);
      file.Write((const char*)fileName,fileName.GetLength());
      file.Write("\0",1);
   }
   catch (CException* e)
   {
      CString errorMessage;
      e->GetErrorMessage(errorMessage.GetBufferSetLength(128),128);
      retval = false;
   }

   return retval;
}

//_____________________________________________________________________________
CStdioPkZipFileHeader::CStdioPkZipFileHeader()
{
   m_validFlag   = false;
   m_validCcFlag = false;

   m_headerSize = 0;

// m_localFileHeaderSignature = 0x04034b50;
   m_headerSize              += sizeof(m_localFileHeaderSignature);

// m_versionNeededToExtract   =     10;
   m_headerSize              += sizeof(m_versionNeededToExtract);

// m_generalPurposeBitFlag    = 0x0000;
   m_headerSize              += sizeof(m_generalPurposeBitFlag);

// m_compressionMethod        =      8;
   m_headerSize              += sizeof(m_compressionMethod);

   m_fileModificationTime     =      0;
   m_headerSize              += sizeof(m_fileModificationTime);

   m_fileModificationDate     =      0;
   m_headerSize              += sizeof(m_fileModificationDate);

   m_crc                      =      0;
   m_headerSize              += sizeof(m_crc);

   m_compressedSize           =      0;
   m_headerSize              += sizeof(m_compressedSize);

   m_uncompressedSize         =      0;
   m_headerSize              += sizeof(m_uncompressedSize);

   m_fileNameLength           =      0;
   m_headerSize              += sizeof(m_fileNameLength);

   m_extraFieldLength         =      0;
   m_headerSize              += sizeof(m_extraFieldLength);

   // for updating header
   m_updatePos = 0;

   // for locating end of central directory record
   m_endOfCentralDirectoryRecordSize  = 0;
   m_endOfCentralDirectoryRecordSize += sizeof(m_endOfCentralDirectorySignature);
   m_endOfCentralDirectoryRecordSize += sizeof(m_numberOfThisDisk);
   m_endOfCentralDirectoryRecordSize += sizeof(m_numberOfTheDiskWithCentralDirectory);
   m_endOfCentralDirectoryRecordSize += sizeof(m_numberOfCentralDirectoryEntriesOnThisDisk);
   m_endOfCentralDirectoryRecordSize += sizeof(m_numberOfCentralDirectoryEntries);
   m_endOfCentralDirectoryRecordSize += sizeof(m_sizeOfCentralDirectory);
   m_endOfCentralDirectoryRecordSize += sizeof(m_centralDirectoryOffset);
   m_endOfCentralDirectoryRecordSize += sizeof(m_zipFileCommentLength);
}

FileStatusTag CStdioPkZipFileHeader::readHeader(CFile& file)
{
   FileStatusTag retval = statusUndefined;
   m_validFlag = false;

   char* headerBuf = new char[m_headerSize];

   if (file.Read(headerBuf,m_headerSize) == m_headerSize)
   {
      m_validFlag = true;
      int index = 0;

      m_validFlag = (*(unsigned int*)(headerBuf + index) == m_localFileHeaderSignature) && m_validFlag;
      index      += sizeof(m_localFileHeaderSignature);

      m_validFlag = (*(unsigned short*)(headerBuf + index) == m_versionNeededToExtract) && m_validFlag;
      index      += sizeof(m_versionNeededToExtract);

      m_validFlag = (*(unsigned short*)(headerBuf + index) == m_generalPurposeBitFlag) && m_validFlag;
      index      += sizeof(m_generalPurposeBitFlag);

      m_validFlag = (*(unsigned short*)(headerBuf + index) == m_compressionMethod) && m_validFlag;
      index      += sizeof(m_compressionMethod);

      if (m_validFlag)
      {
         m_fileModificationTime = *(unsigned short*)(headerBuf + index);
         index                 += sizeof(m_fileModificationTime);

         m_fileModificationDate = *(unsigned short*)(headerBuf + index);
         index                 += sizeof(m_fileModificationDate);

         m_crc                  = *(unsigned int*)(headerBuf + index);
         index                 += sizeof(m_crc);

         m_compressedSize       = *(unsigned int*)(headerBuf + index);
         index                 += sizeof(m_compressedSize);

         m_uncompressedSize     = *(unsigned int*)(headerBuf + index);
         index                 += sizeof(m_uncompressedSize);

         m_fileNameLength       = *(unsigned short*)(headerBuf + index);
         index                 += sizeof(m_fileNameLength);

         m_extraFieldLength     = *(unsigned short*)(headerBuf + index);
         index                 += sizeof(m_extraFieldLength);

         if (file.Read(m_fileName.GetBufferSetLength(m_fileNameLength),m_fileNameLength) != m_fileNameLength)
         {
            m_validFlag = false;
            retval = statusFileReadFailure;
         }
         else if (file.Read(m_extraField.GetBufferSetLength(m_extraFieldLength),m_extraFieldLength) != m_extraFieldLength)
         {
            m_validFlag = false;
            retval = statusFileReadFailure;
         }
         else
         {
            m_fileName.ReleaseBuffer(m_fileNameLength);
            m_extraField.ReleaseBuffer(m_extraFieldLength);
         }
      }
   }

   delete headerBuf;

   if (retval == statusUndefined)
   {
      retval = (m_validFlag ? statusSucceeded : statusInvalidHeader);
   }

   return retval;
}

FileStatusTag CStdioPkZipFileHeader::readEndOfCentralDirectoryRecord(CFile& file)
{
   FileStatusTag retval = statusUndefined;

   m_validCcFlag = false;

   int minFileSize    = m_headerSize + m_endOfCentralDirectoryRecordSize;
   ULONGLONG fileSize = file.GetLength();

   if (fileSize > minFileSize)
   {
      ULONGLONG currentFilePosition;
      currentFilePosition = file.GetPosition();

      const int maxZipFileCommentSize = 256;
      int dirBufSize = m_endOfCentralDirectoryRecordSize + maxZipFileCommentSize;

      if (dirBufSize > fileSize)
      {
         dirBufSize = (int)fileSize;
      }

      CString dirBuffer;
      char* dirBuf = new char[dirBufSize];
      file.Seek(-dirBufSize,CFile::end);
      int numBytesRead = file.Read(dirBuf,dirBufSize);

      int signatureSize = sizeof(m_endOfCentralDirectorySignature);
      const char* sigBuf = ( char*)(&m_endOfCentralDirectorySignature);

      int index = -1;
      int pos   =  0;
      int state =  0;

      while (pos <= numBytesRead - m_endOfCentralDirectoryRecordSize)
      {
         if (dirBuf[pos + state] == sigBuf[state])
         {
            state++;

            if (state == signatureSize)
            {
               index = pos;
               break;
            }
         }
         else
         {
            pos++;
            state = 0;
         }
      }

      if (index >= 0)
      {
         m_validCcFlag = true;

         m_validCcFlag = (*(unsigned int*)(dirBuf + index) == m_endOfCentralDirectorySignature) && m_validCcFlag;
         index      += sizeof(m_endOfCentralDirectorySignature);

         m_numberOfThisDisk = *(unsigned short*)(dirBuf + index);
         index      += sizeof(m_numberOfThisDisk);

         m_numberOfTheDiskWithCentralDirectory = *(unsigned short*)(dirBuf + index);
         index      += sizeof(m_numberOfTheDiskWithCentralDirectory);

         m_numberOfCentralDirectoryEntriesOnThisDisk = *(unsigned short*)(dirBuf + index);
         index      += sizeof(m_numberOfCentralDirectoryEntriesOnThisDisk);

         m_numberOfCentralDirectoryEntries = *(unsigned short*)(dirBuf + index);
         index      += sizeof(m_numberOfCentralDirectoryEntries);

         if ((m_numberOfThisDisk                          != 0) ||
             (m_numberOfTheDiskWithCentralDirectory       != 0))
         {
            retval = statusTooManyDisks;
            m_validCcFlag = false;
         }
         else if ((m_numberOfCentralDirectoryEntriesOnThisDisk != 1) ||
                  (m_numberOfCentralDirectoryEntries           != 1))
         {
            retval = statusTooManyFiles;
            m_validCcFlag = false;
         }
         else
         {
            m_sizeOfCentralDirectory = *(unsigned int*)(dirBuf + index);
            index                   += sizeof(m_sizeOfCentralDirectory);

            m_centralDirectoryOffset = *(unsigned int*)(dirBuf + index);
            index                   += sizeof(m_centralDirectoryOffset);

            m_zipFileCommentLength   = *(unsigned short*)(dirBuf + index);
            index                   += sizeof(m_zipFileCommentLength);

            if (file.Read(m_zipFileComment.GetBufferSetLength(m_zipFileCommentLength),m_zipFileCommentLength) != 
                  m_zipFileCommentLength)
            {
               retval = statusFileReadFailure;
               m_validCcFlag = false;
            }
            else
            {
               m_fileName.ReleaseBuffer(m_zipFileCommentLength);
            }
         }
      }
      else
      {
         retval = statusCouldNotFindEndOfCentralDirectory;
      }

      file.Seek(currentFilePosition,CFile::begin);

      delete dirBuf;
   }

   if (retval == statusUndefined)
   {
      retval = (m_validCcFlag ? statusSucceeded : statusInvalidHeader);
   }

   return retval;
}

bool CStdioPkZipFileHeader::writeHeader(CFile& file,const CString& fileName)
{
   bool retval = true;

   try
   {
      m_fileName = fileName;

      m_fileNameLength    = m_fileName.GetLength();
      m_extraFieldLength  = m_extraField.GetLength();

      file.Write(&m_localFileHeaderSignature,sizeof(m_localFileHeaderSignature));
      file.Write(&m_versionNeededToExtract  ,sizeof(m_versionNeededToExtract));
      file.Write(&m_generalPurposeBitFlag   ,sizeof(m_generalPurposeBitFlag));
      file.Write(&m_compressionMethod       ,sizeof(m_compressionMethod));

      m_updatePos = file.GetPosition();

      file.Write(&m_fileModificationTime    ,sizeof(m_fileModificationTime));
      file.Write(&m_fileModificationDate    ,sizeof(m_fileModificationDate));
      file.Write(&m_crc                     ,sizeof(m_crc));
      file.Write(&m_compressedSize          ,sizeof(m_compressedSize));
      file.Write(&m_uncompressedSize        ,sizeof(m_uncompressedSize));
      file.Write(&m_fileNameLength          ,sizeof(m_fileNameLength));
      file.Write(&m_extraFieldLength        ,sizeof(m_extraFieldLength));

      file.Write((const char*)m_fileName  ,m_fileNameLength);
      file.Write((const char*)m_extraField,m_extraFieldLength);
   }
   catch (CException* e)
   {
      CString errorMessage;
      e->GetErrorMessage(errorMessage.GetBufferSetLength(128),128);
      retval = false;
   }

   return retval;
}

bool CStdioPkZipFileHeader::updateHeader(CFile& file)
{
   bool retval = true;

   try
   {
      //FILETIME fileTime;
      //GetSystemTimeAsFileTime(&fileTime);
      //FileTimeToDosDateTime(&fileTime,&m_fileModificationDate,&m_fileModificationTime);

      //m_crc              = crc;
      //m_compressedSize   = compressedSize;
      //m_uncompressedSize = uncompressedSize;

      file.Seek(m_updatePos,CFile::begin);

      file.Write(&m_fileModificationTime    ,sizeof(m_fileModificationTime));
      file.Write(&m_fileModificationDate    ,sizeof(m_fileModificationDate));
      file.Write(&m_crc                     ,sizeof(m_crc));
      file.Write(&m_compressedSize          ,sizeof(m_compressedSize));
      file.Write(&m_uncompressedSize        ,sizeof(m_uncompressedSize));
   }
   catch (CException* e)
   {
      CString errorMessage;
      e->GetErrorMessage(errorMessage.GetBufferSetLength(128),128);
      retval = false;
   }

   return retval;
}

bool CStdioPkZipFileHeader::writeTrailer(CFile& file,const CString& fileComment,
   unsigned int crc,unsigned int compressedSize,unsigned int uncompressedSize)
{
   bool retval = true;

   try
   {
      m_fileComment = fileComment;

      m_fileNameLength    = m_fileName.GetLength();
      m_extraFieldLength  = m_extraField.GetLength();
      m_fileCommentLength = m_fileComment.GetLength();

      FILETIME fileTime;
      GetSystemTimeAsFileTime(&fileTime);
      FileTimeToDosDateTime(&fileTime,&m_fileModificationDate,&m_fileModificationTime);

      m_crc              = crc;
      m_compressedSize   = compressedSize;
      m_uncompressedSize = uncompressedSize;

      unsigned int centralDirectoryOffset = (unsigned int)file.GetPosition();

      file.Write(&m_centralFileHeaderSignature,sizeof(m_centralFileHeaderSignature));
      file.Write(&m_versionMadeBy             ,sizeof(m_versionMadeBy)); 
      file.Write(&m_versionNeededToExtract    ,sizeof(m_versionNeededToExtract));
      file.Write(&m_generalPurposeBitFlag     ,sizeof(m_generalPurposeBitFlag));
      file.Write(&m_compressionMethod         ,sizeof(m_compressionMethod));
      file.Write(&m_fileModificationTime      ,sizeof(m_fileModificationTime));
      file.Write(&m_fileModificationDate      ,sizeof(m_fileModificationDate));
      file.Write(&m_crc                       ,sizeof(m_crc));
      file.Write(&m_compressedSize            ,sizeof(m_compressedSize));
      file.Write(&m_uncompressedSize          ,sizeof(m_uncompressedSize));
      file.Write(&m_fileNameLength            ,sizeof(m_fileNameLength));
      file.Write(&m_extraFieldLength          ,sizeof(m_extraFieldLength));
      file.Write(&m_fileCommentLength         ,sizeof(m_fileCommentLength));

      unsigned short discNumberStart = 0;
      file.Write(&discNumberStart             ,sizeof(discNumberStart));

      unsigned short internalFileAttributes = 0;
      file.Write(&internalFileAttributes      ,sizeof(internalFileAttributes));

      unsigned int externalFileAttributes = 0;
      file.Write(&externalFileAttributes      ,sizeof(externalFileAttributes));

      unsigned int relativeOffsetOfLocalHeader = 0;
      file.Write(&relativeOffsetOfLocalHeader ,sizeof(relativeOffsetOfLocalHeader));

      file.Write((const char*)m_fileName   ,m_fileNameLength);
      file.Write((const char*)m_extraField ,m_extraFieldLength);
      file.Write((const char*)m_fileComment,m_fileComment.GetLength());

      unsigned int endOfCentralDirectoryOffset = (unsigned int)file.GetPosition();

      unsigned int endOfCentralDirectorySignature = 0x06054b50;
      unsigned short numberOfThisDisk                          = 0;
      unsigned short numberOfTheDiskWithCentralDirectory       = 0;
      unsigned short numberOfCentralDirectoryEntriesOnThisDisk = 1;
      unsigned short numberOfCentralDirectoryEntries           = 1;
      unsigned int sizeOfCentralDirectory = endOfCentralDirectoryOffset - centralDirectoryOffset;
      unsigned short zipFileCommentLength                      = 0;

      file.Write(&endOfCentralDirectorySignature           ,sizeof(endOfCentralDirectorySignature));
      file.Write(&numberOfThisDisk                         ,sizeof(numberOfThisDisk));
      file.Write(&numberOfTheDiskWithCentralDirectory      ,sizeof(numberOfTheDiskWithCentralDirectory));
      file.Write(&numberOfCentralDirectoryEntriesOnThisDisk,sizeof(numberOfCentralDirectoryEntriesOnThisDisk));
      file.Write(&numberOfCentralDirectoryEntries          ,sizeof(numberOfCentralDirectoryEntries));
      file.Write(&sizeOfCentralDirectory                   ,sizeof(sizeOfCentralDirectory));
      file.Write(&centralDirectoryOffset                   ,sizeof(centralDirectoryOffset));
      file.Write(&zipFileCommentLength                     ,sizeof(zipFileCommentLength));
   }
   catch (CException* e)
   {
      CString errorMessage;
      e->GetErrorMessage(errorMessage.GetBufferSetLength(128),128);
      retval = false;
   }

   return retval;
}

//_____________________________________________________________________________
CStdioUnixCompressedFile::CStdioUnixCompressedFile(int inBufSize,int outBufSize)
{
   m_inBufSize  = inBufSize;
   m_outBufSize = outBufSize;

   m_inBuf      = new char[m_inBufSize  + 1];
   m_outBuf     = new char[m_outBufSize + 1];

   m_readMode   = false;
   m_writeMode  = false;
   m_eofFlag    = false;
   m_headerProcessed = false;
}

CStdioUnixCompressedFile::~CStdioUnixCompressedFile()
{
   delete[] m_inBuf;
   delete[] m_outBuf;
}

int CStdioUnixCompressedFile::getCompressedSize() const
{
   return m_compressedSize;
}

int CStdioUnixCompressedFile::getNumBytesInput() 
{ 
   return m_compressEngine.getNumBytesInput(); 
}

int CStdioUnixCompressedFile::getNumBytesOutput() 
{ 
   return m_compressEngine.getNumBytesOutput(); 
}

FileStatusTag CStdioUnixCompressedFile::readHeader() 
{ 
   return m_header.readHeader(m_file); 
}

bool CStdioUnixCompressedFile::isHeaderValid() 
{ 
   return m_header.getValidFlag(); 
}

FileStatusTag CStdioUnixCompressedFile::openRead(const CString& archivePath)
{
   FileStatusTag retval = statusAlreadyOpen;

   if (!m_readMode && !m_writeMode)
   {
      m_totalCompressedBytesRead = 0;
      m_totalLinesRead = 0;
      m_compressedSize = 0;
      m_eodFlag        = false;

      if (m_file.Open(archivePath,CFile::modeRead))
      {
         //getStream()->next_in   = (unsigned char*)m_inBuf;
         //getStream()->avail_in  = 0;
         //getStream()->next_out  = (unsigned char*)m_outBuf;
         //getStream()->avail_out = m_outBufSize;
         m_compressEngine.setNextIn((unsigned char*)m_inBuf);
         m_compressEngine.setAvailableIn(0);
         m_compressEngine.setNextOut((unsigned char*)m_outBuf);
         m_compressEngine.setAvailableOut(m_outBufSize);

         CFileStatus fileStatus;
         m_file.GetStatus(fileStatus);
         m_compressedSize = (int)fileStatus.m_size;

         retval = readHeader();

         if (retval == statusSucceeded)
         {
            if (isHeaderValid())
            {
               m_archivePath = archivePath;
               retval     = statusSucceeded;
               m_readMode = true;
               m_totalCompressedBytesRead += m_header.getHeaderSize();
            }
            else
            {
               retval = statusInvalidHeader;
            }
         }

         if (retval != statusSucceeded)
         {
            m_file.Close();
         }
      }
      else
      {
         retval = statusFileOpenFailure;
      }
   }

   return retval;
}

FileStatusTag CStdioUnixCompressedFile::readString(CString& string)
{
   FileStatusTag retval = statusEof;
   string.GetBufferSetLength(0);

   // fails if ascii nulls are present in the uncompressed data

   while (true)
   {
      int outputCharCount = m_outBufSize - m_compressEngine.getAvailableOut();

      if (outputCharCount > 0)
      {
         *(m_compressEngine.getNextOut()) = '\0';
         char* p = strchr(m_outBuf,'\n');

         if (p != NULL)
         {
            m_totalLinesRead++;

            if (p > m_outBuf && p[-1] == '\r')
            {
               p[-1] = '\0';
            }
            else
            {
               *p = '\0';
            }

            string += m_outBuf;

            int numLeft = (char*)m_compressEngine.getNextOut() - p - 1;

            memmove(m_outBuf,p + 1,numLeft);
            m_compressEngine.setNextOut((unsigned char*)m_outBuf + numLeft);
            m_compressEngine.setAvailableOut(m_outBufSize - numLeft);

            retval = statusSucceeded;
            break;
         }
         else 
         {
            if (m_outBuf[outputCharCount - 1] == '\r')
            {
               m_outBuf[outputCharCount - 1] = '\0';
            }

            string += m_outBuf;
            m_compressEngine.setNextOut((unsigned char*)m_outBuf);
            m_compressEngine.setAvailableOut(m_outBufSize);
         }
      }

      if (m_compressEngine.getAvailableIn() == 0 && !m_eofFlag)
      {
         int numCompressedBytesRemaining = m_compressedSize - m_totalCompressedBytesRead;
         int numBytesToRead = ((numCompressedBytesRemaining < m_inBufSize) ? numCompressedBytesRemaining : m_inBufSize);
         int numRead = m_file.Read(m_inBuf,numBytesToRead);
         m_totalCompressedBytesRead += numRead;

         if (numRead < m_inBufSize)
         {
            m_eofFlag = true;
         }

         m_compressEngine.setNextIn((unsigned char*)m_inBuf);
         m_compressEngine.setAvailableIn(numRead);
      }

      if (m_eodFlag && outputCharCount == 0)
      {
         break;
      }

      UnixCompressEngineStatusTag status = m_compressEngine.decompress(m_eofFlag);

      if (status == statusEndOfData)
      {
         m_eodFlag = true;
      }
      else if (status != statusSuccess)
      {
         retval = statusCompressEngineFailure;
         break;
      }

      //// convert null characters to linefeeds
      //char* q   = m_outBuf;
      //char* eob = m_outBuf + (m_outBufSize - m_compressEngine.getAvailableOut());

      //while (q < eob)
      //{
      //   q = (char*)memchr(q,0,eob - q);

      //   if (q == NULL)
      //   {
      //      break;
      //   }

      //   *(q++) = '\n';
      //}
   }

   return retval;
}

bool CStdioUnixCompressedFile::openWrite(const CString& archivePath)
{
   bool retval = false;

   return retval;
}

bool CStdioUnixCompressedFile::close()
{
   bool retval = true;

   if (m_writeMode)
   {
   }
   else if (m_readMode)
   {
      m_file.Close();
   }

   m_readMode = m_writeMode = false;

   return retval;
}

//_____________________________________________________________________________
CStdioRsiZipFile::CStdioRsiZipFile(int inBufSize,int outBufSize)
{
   m_inBufSize  = inBufSize;
   m_outBufSize = outBufSize;

   m_inBuf      = new char[m_inBufSize  + 1];
   m_outBuf     = new char[m_outBufSize + 1];

   m_readMode   = false;
   m_writeMode  = false;
   m_eofFlag    = false;
   m_headerProcessed = false;
}

CStdioRsiZipFile::~CStdioRsiZipFile()
{
   delete[] m_inBuf;
   delete[] m_outBuf;
}

bool CStdioRsiZipFile::isCrRecordTerminator() const
{
   return false;
}

int CStdioRsiZipFile::getCompressedSize() const
{
   return m_compressedSize;
}

int CStdioRsiZipFile::getNumBytesInput() 
{ 
   return getStream()->total_in; 
}

int CStdioRsiZipFile::getNumBytesOutput() 
{ 
   return getStream()->total_out; 
}

FileStatusTag CStdioRsiZipFile::readHeader() 
{ 
   return m_header.readHeader(m_file); 
}

bool CStdioRsiZipFile::isHeaderValid() 
{ 
   return m_header.getValidFlag(); 
}

FileStatusTag CStdioRsiZipFile::openRead(const CString& archivePath)
{
   FileStatusTag retval = statusAlreadyOpen;

   if (!m_readMode && !m_writeMode)
   {
      m_totalCompressedBytesRead = 0;
      m_compressedSize = 0;
      m_eodFlag        = false;

      if (m_file.Open(archivePath,CFile::modeRead))
      {
         getStream()->next_in   = (unsigned char*)m_inBuf;
         getStream()->avail_in  = 0;
         getStream()->next_out  = (unsigned char*)m_outBuf;
         getStream()->avail_out = m_outBufSize;

         retval = readHeader();

         if (retval == statusSucceeded)
         {
            if (isHeaderValid())
            {
               int status = m_zStream.initInflate();

               if (status == Z_OK)
               {
                  m_archivePath = archivePath;
                  retval     = statusSucceeded;
                  m_readMode = true;
               }
               else
               {
                  retval = statusCompressEngineFailure;
               }
            }
            else
            {
               retval = statusInvalidHeader;
            }
         }

         if (retval != statusSucceeded)
         {
            m_file.Close();
         }
      }
      else
      {
         retval = statusFileOpenFailure;
      }
   }

   return retval;
}

FileStatusTag CStdioRsiZipFile::readString(CString& string)
{
   FileStatusTag retval = statusEof;
   string.GetBufferSetLength(0);

   // fails if ascii nulls are present in the uncompressed data

   while (true)
   {
      int outputCharCount = m_outBufSize - getStream()->avail_out;

      if (outputCharCount > 0)
      {
         *(getStream()->next_out) = '\0';
         char* p = strstr(m_outBuf,"\n");
         int terminatorLen = 1;

         if (p != NULL)
         {
            if (p > m_outBuf && p[-1] == '\r')
            {
               p[-1] = '\0';
            }
            else
            {
               p[0] = '\0';
            }

            string += m_outBuf;
            //string += "\n";

            int numLeft = (char*)getStream()->next_out - p - terminatorLen;

            memmove(m_outBuf,p + terminatorLen,numLeft);
            getStream()->next_out  = (unsigned char*)m_outBuf + numLeft;
            getStream()->avail_out = m_outBufSize - numLeft;

            retval = statusSucceeded;
            break;
         }
         else 
         {
            string += m_outBuf;
            getStream()->next_out  = (unsigned char*)m_outBuf;
            getStream()->avail_out = m_outBufSize;
         }
      }

      if (getStream()->avail_in == 0 && !m_eofFlag)
      {
         int numCompressedBytesRemaining = m_compressedSize - m_totalCompressedBytesRead;
         int numBytesToRead = ((numCompressedBytesRemaining < m_inBufSize) ? numCompressedBytesRemaining : m_inBufSize);
         int numRead = m_file.Read(m_inBuf,numBytesToRead);
         m_totalCompressedBytesRead += numRead;

         if (numRead < m_inBufSize)
         {
            m_eofFlag = true;
         }

         getStream()->next_in  = (unsigned char*)m_inBuf;
         getStream()->avail_in = numRead;
      }

      if (m_eodFlag && outputCharCount == 0)
      {
         break;
      }

      int status = m_zStream.inflate(Z_SYNC_FLUSH);

      if (status == Z_STREAM_END)
      {
         m_eodFlag = true;
      }
      else if (status != Z_OK)
      {
         retval = statusCompressEngineFailure;
         break;
      }
   }

   return retval;
}

bool CStdioRsiZipFile::writeHeader()
{
   return m_header.writeHeader(m_file,getFileName());
}

bool CStdioRsiZipFile::openWrite(const CString& archivePath)
{
   bool retval = false;

   if (!m_readMode && !m_writeMode)
   {
      UINT mode = CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone;

      if (m_file.Open(archivePath,mode))
      {
         m_archivePath = archivePath;

         if (writeHeader())
         {
            getStream()->next_in   = (unsigned char*)m_inBuf;
            getStream()->avail_in  = 0;
            getStream()->next_out  = (unsigned char*)m_outBuf;
            getStream()->avail_out = m_outBufSize;

            int status = m_zStream.initDeflate(Z_DEFAULT_COMPRESSION);

            if (status == Z_OK)
            {
               retval      = true;
               m_writeMode = true;
            }
         }
         else
         {
            m_file.Close();
            m_archivePath.Empty();
         }
      }
   }

   return retval;
}

void CStdioRsiZipFile::writeFile(const char* p,int outputLen)
{
   m_file.Write(p,outputLen);
}

bool CStdioRsiZipFile::writeString(const CString& string)
{
   CString outputString(string);
   outputString.Replace("\n","\r\n");

   int outputLen = outputString.GetLength();
   const char* p = (const char*)outputString;

   bool retval = writeRaw(p,outputLen);

   return retval;
}

bool CStdioRsiZipFile::writeRaw(const char* p,int outputLen)
{
   bool retval = false;

   while (true)
   {
      int lenRemaining = m_inBufSize - getStream()->avail_in;

      if (outputLen <= lenRemaining)
      {
         memcpy(m_inBuf + getStream()->avail_in,p,outputLen);
         getStream()->avail_in += outputLen;

         retval = true;
         break;
      }
      else
      {
         memcpy(m_inBuf + getStream()->avail_in,p,lenRemaining);
         getStream()->avail_in += lenRemaining;

         p         += lenRemaining;
         outputLen -= lenRemaining;

         for (bool completeFlag = false;!completeFlag;)
         {
            int status = m_zStream.deflate(Z_NO_FLUSH);

            if (status != Z_OK)
            {
               if (status != Z_BUF_ERROR)
               {
                  return false;
               }

               break;
            }

            completeFlag = ((getStream()->avail_out > 0) && (getStream()->avail_in == 0));

            int writeLen = m_outBufSize - getStream()->avail_out;

            if (writeLen > 0)
            {
               writeFile(m_outBuf,writeLen);
               getStream()->next_out  = (unsigned char*)m_outBuf;
               getStream()->avail_out = m_outBufSize;
            }
         }

         getStream()->next_in  = (unsigned char*)m_inBuf;
         getStream()->avail_in = 0;
      }
   }

   return retval;
}

bool CStdioRsiZipFile::flush()
{
   bool retval = m_writeMode;
   bool eod = false;

   while (retval)
   {
      int writeLen = m_outBufSize - getStream()->avail_out;

      if (writeLen > 0)
      {
         writeFile(m_outBuf,writeLen);
         getStream()->next_out  = (unsigned char*)m_outBuf;
         getStream()->avail_out = m_outBufSize;
      }

      if (eod)
      {
         break;
      }

      if ((getStream()->avail_in > 0) || (getStream()->avail_out > 0))
      {
         int status = m_zStream.deflate(Z_FINISH);

         if (status == Z_STREAM_END)
         {
            eod = true;
         }
         else if (status != Z_OK)
         {
            return false;
         }
      }
      else
      {
         break;
      }
   }

   return retval;
}

bool CStdioRsiZipFile::close()
{
   bool retval = false;

   if (m_writeMode)
   {
      retval = flush();
      m_zStream.endDeflate();
      m_file.Close();
   }
   else if (m_readMode)
   {
      int status = m_zStream.endInflate();
      retval = (status == Z_OK);
      m_file.Close();
   }

   m_readMode = m_writeMode = false;

   return retval;
}

//_____________________________________________________________________________
CStdioGzFile::CStdioGzFile(int inBufSize,int outBufSize) :
   CStdioRsiZipFile(inBufSize,outBufSize)
{
   m_totalUncompressedBytesWritten = 0;
   m_crc               = 0;

   createCrcTable();
}

CStdioGzFile::~CStdioGzFile()
{
}

void CStdioGzFile::createCrcTable() 
{ 
  unsigned long c;
  int n, k;

  for (n = 0; n < 256; n++)
  {
    c = (unsigned long) n;

    for (k = 0; k < 8; k++) 
    {
      if (c & 1) 
      {
        c = 0xedb88320L ^ (c >> 1);
      } 
      else 
      {
        c = c >> 1;
      }
    }

    m_crcTable[n] = c;
  }
}

bool CStdioGzFile::isCrRecordTerminator() const
{
   return true;
}

FileStatusTag CStdioGzFile::readHeader() 
{ 
   FileStatusTag retval = m_header.readHeader(m_file); 

   if (retval == statusSucceeded)
   {
      setCompressedSize(m_header.getCompressedSize());
   }

   return retval;
}

bool CStdioGzFile::isHeaderValid() 
{ 
   return m_header.getValidFlag(); 
}

bool CStdioGzFile::writeHeader()
{
   return m_header.writeHeader(m_file,getFileName());
}

void CStdioGzFile::updateCrc(const char* p,int outputLen)
{
   unsigned long c = m_crc ^ 0xffffffffL;

   for (int ind = 0;ind < outputLen;ind++)
   {
      c = m_crcTable[(c ^ (unsigned char)(p[ind])) & 0xff] ^ (c >> 8);
   }

   m_crc = c ^ 0xffffffffL;
}

bool CStdioGzFile::writeRaw(const char* p,int outputLen)
{
   m_totalUncompressedBytesWritten += outputLen;
   updateCrc(p,outputLen);

   bool retval = CStdioRsiZipFile::writeRaw(p,outputLen);

   return retval;
}

//void CStdioGzFile::writeFile(const char* p,int outputLen)
//{
//   m_totalBytesWritten += outputLen;
//   updateCrc(p,outputLen);
//   m_file.Write(p,outputLen);
//}

bool CStdioGzFile::close()
{
   bool retval = false;

   if (m_writeMode)
   {
      retval = flush();
      m_zStream.endDeflate();

      //unsigned long crc               = _byteswap_ulong(m_crc);
      //unsigned long totalBytesWritten = _byteswap_ulong(m_totalUncompressedBytesWritten);
      //m_file.Write((void*)&crc              ,sizeof(crc              ));
      //m_file.Write((void*)&totalBytesWritten,sizeof(totalBytesWritten));
      m_file.Write((void*)&m_crc                          ,sizeof(m_crc                          ));
      m_file.Write((void*)&m_totalUncompressedBytesWritten,sizeof(m_totalUncompressedBytesWritten));

      m_file.Close();
   }
   else if (m_readMode)
   {
      int status = m_zStream.endInflate();
      retval = (status == Z_OK);
      m_file.Close();
   }

   m_readMode = m_writeMode = false;

   return retval;
}

//_____________________________________________________________________________
CStdioPkZipFile::CStdioPkZipFile(int inBufSize,int outBufSize) :
   CStdioRsiZipFile(inBufSize,outBufSize)
{
   m_totalUncompressedBytesWritten = 0;
   m_crc                           = 0;

   createCrcTable();
}

CStdioPkZipFile::~CStdioPkZipFile()
{
}

void CStdioPkZipFile::createCrcTable() 
{ 
  unsigned long c;
  int n, k;

  for (n = 0; n < 256; n++)
  {
    c = (unsigned long) n;

    for (k = 0; k < 8; k++) 
    {
      if (c & 1) 
      {
        c = 0xedb88320L ^ (c >> 1);
      } 
      else 
      {
        c = c >> 1;
      }
    }

    m_crcTable[n] = c;
  }
}

FileStatusTag CStdioPkZipFile::readHeader() 
{ 
   FileStatusTag retval = m_header.readHeader(m_file); 

   if (retval == statusSucceeded)
   {
      setCompressedSize(m_header.getCompressedSize());

      retval = m_header.readEndOfCentralDirectoryRecord(m_file);
   }

   return retval;
}

bool CStdioPkZipFile::isHeaderValid() 
{ 
   return m_header.getValidFlag(); 
}

bool CStdioPkZipFile::writeHeader()
{
   return m_header.writeHeader(m_file,getFileName());
}

void CStdioPkZipFile::updateCrc(const char* p,int outputLen)
{
   unsigned long c = m_crc ^ 0xffffffffL;

   for (int ind = 0;ind < outputLen;ind++)
   {
      c = m_crcTable[(c ^ (unsigned char)(p[ind])) & 0xff] ^ (c >> 8);
   }

   m_crc = c ^ 0xffffffffL;
}

bool CStdioPkZipFile::writeRaw(const char* p,int outputLen)
{
   m_totalUncompressedBytesWritten += outputLen;
   updateCrc(p,outputLen);

   bool retval = CStdioRsiZipFile::writeRaw(p,outputLen);

   return retval;
}

//void CStdioGzFile::writeFile(const char* p,int outputLen)
//{
//   m_totalBytesWritten += outputLen;
//   updateCrc(p,outputLen);
//   m_file.Write(p,outputLen);
//}

bool CStdioPkZipFile::close()
{
   bool retval = false;

   if (m_writeMode)
   {
      retval = flush();
      m_zStream.endDeflate();

      //unsigned long crc               = _byteswap_ulong(m_crc);
      //unsigned long totalBytesWritten = _byteswap_ulong(m_totalUncompressedBytesWritten);
      //m_file.Write((void*)&crc              ,sizeof(crc              ));
      //m_file.Write((void*)&totalBytesWritten,sizeof(totalBytesWritten));

      unsigned int compressedSize   = getNumBytesOutput();
      unsigned int uncompressedSize = getNumBytesInput();

      m_header.writeTrailer(m_file,"Generated by CamCad - Mentor Graphics",
         m_crc,compressedSize,uncompressedSize);

      m_header.updateHeader(m_file);

      m_file.Close();
   }
   else if (m_readMode)
   {
      int status = m_zStream.endInflate();
      retval = (status == Z_OK);
      m_file.Close();
   }

   m_readMode = m_writeMode = false;

   return retval;
}

//_____________________________________________________________________________
CStdioCompressedFileWriteFormat::CStdioCompressedFileWriteFormat(int bufSize) : 
   CWriteFormat(bufSize)
{
}

CStdioCompressedFileWriteFormat::CStdioCompressedFileWriteFormat(const CString& archivePath,int bufSize) : 
   CWriteFormat(bufSize)
{
   m_archivePath = archivePath;
}

CStdioCompressedFileWriteFormat::CStdioCompressedFileWriteFormat(const CString& archivePath,const CString& fileName,int bufSize) : 
   CWriteFormat(bufSize)
{
   m_archivePath = archivePath;
   m_fileName    = fileName;
}

CStdioCompressedFileWriteFormat::~CStdioCompressedFileWriteFormat()
{
}

//_____________________________________________________________________________
CStdioRsiZipFileWriteFormat::CStdioRsiZipFileWriteFormat(int bufSize) : 
   CStdioCompressedFileWriteFormat(bufSize)
{
}

CStdioRsiZipFileWriteFormat::CStdioRsiZipFileWriteFormat(const CString& archivePath,int bufSize) : 
   CStdioCompressedFileWriteFormat(archivePath,bufSize)
{
   open(archivePath);
}

CStdioRsiZipFileWriteFormat::~CStdioRsiZipFileWriteFormat()
{
   close();
}

void CStdioRsiZipFileWriteFormat::close()
{
   setArchivePath("");
   m_compressedFile.close();
}

bool CStdioRsiZipFileWriteFormat::open(const CString& archivePath,const CString& fileName)
{
   setArchivePath(archivePath);
   setFileName(fileName);

   bool retval = m_compressedFile.openWrite(archivePath);

   return retval;
}

void CStdioRsiZipFileWriteFormat::write(const char* string)
{
   m_compressedFile.writeString(string);
}

//_____________________________________________________________________________
CStdioGzFileWriteFormat::CStdioGzFileWriteFormat(int bufSize) : 
   CStdioCompressedFileWriteFormat(bufSize)
{
}

CStdioGzFileWriteFormat::CStdioGzFileWriteFormat(const CString& archivePath,int bufSize) : 
   CStdioCompressedFileWriteFormat(archivePath,bufSize)
{
   open(archivePath);
}

CStdioGzFileWriteFormat::~CStdioGzFileWriteFormat()
{
   close();
}

void CStdioGzFileWriteFormat::close()
{
   setArchivePath("");
   m_compressedFile.close();
}

bool CStdioGzFileWriteFormat::open(const CString& archivePath,const CString& fileName)
{
   setArchivePath(archivePath);
   setFileName(fileName);

   bool retval = m_compressedFile.openWrite(archivePath);

   return retval;
}

void CStdioGzFileWriteFormat::write(const char* string)
{
   m_compressedFile.writeString(string);
}

//_____________________________________________________________________________
CStdioPkZipFileWriteFormat::CStdioPkZipFileWriteFormat(int bufSize) : 
   CStdioCompressedFileWriteFormat(bufSize)
{
}

CStdioPkZipFileWriteFormat::CStdioPkZipFileWriteFormat(const CString& archivePath,int bufSize) : 
   CStdioCompressedFileWriteFormat(archivePath,bufSize)
{
   open(archivePath);
}

CStdioPkZipFileWriteFormat::CStdioPkZipFileWriteFormat(const CString& archivePath,const CString& fileName,int bufSize) : 
   CStdioCompressedFileWriteFormat(archivePath,fileName,bufSize)
{
   open(archivePath,fileName);
}

CStdioPkZipFileWriteFormat::~CStdioPkZipFileWriteFormat()
{
   close();
}

void CStdioPkZipFileWriteFormat::setArchivePath(const CString& archivePath)
{
   m_compressedFile.setArchivePath(archivePath);
   CStdioCompressedFileWriteFormat::setArchivePath(archivePath);
}

void CStdioPkZipFileWriteFormat::setFileName(const CString& fileName)
{
   m_compressedFile.setFileName(fileName);
   CStdioCompressedFileWriteFormat::setFileName(fileName);
}

void CStdioPkZipFileWriteFormat::close()
{
   setArchivePath("");
   m_compressedFile.close();
}

bool CStdioPkZipFileWriteFormat::open(const CString& archivePath,const CString& fileName)
{
   setArchivePath(archivePath);
   setFileName(fileName);

   bool retval = m_compressedFile.openWrite(archivePath);

   return retval;
}

void CStdioPkZipFileWriteFormat::write(const char* string)
{
   m_compressedFile.writeString(string);
}






