// $Header: /CAMCAD/DcaLib/DcaStdioZipFile.h 2     3/23/07 5:19a Kurt Van Ness $

#if !defined(__DcaStdioZipFile_h__)
#define __DcaStdioZipFile_h__

#pragma once

#include "DcaWriteFormat.h"

struct z_stream_s;

//_____________________________________________________________________________
enum FileStatusTag
{
   statusAlreadyOpen,
   statusBadData,
   statusCompressEngineFailure,
   statusCouldNotFindEndOfCentralDirectory,
   statusEof,
   statusFileOpenFailure,
   statusFileReadFailure,
   statusInvalidHeader,
   statusSucceeded,
   statusTooManyDisks,
   statusTooManyFiles,
   statusUnexpectedEof,
   statusUndefined,
   statusUserCancel,
   statusFileAttachFailure,
   statusFileDecryptionFailure,
   statusDecryptionAccessDenied
};

CString fileStatusTagToString(FileStatusTag fileStatusTag);

//_____________________________________________________________________________
class CZStream : public CObject
{
private:
   z_stream_s* m_zStream;

public:
   CZStream();
   ~CZStream();

public:
   int initDeflate(int level);
   int deflate(int flush);    
   int endDeflate();          

   int initInflate();         
   int inflate(int flush);    
   int endInflate();     

   z_stream_s* getStream() { return m_zStream; }
};

//_____________________________________________________________________________
enum UnixCompressEngineStatusTag
{
   statusSuccess,
   statusEndOfData,
   statusBufferError,
   statusDataError,
   statusCorruptedTableError,
   statusTokenBufAllocationError,
   statusInvalidCodeError,
   statusInvalidBitCountError,
   statusCompressEngineStatusUndefined
};

enum UnixCompressEngineStateTag
{
   stateUninitialized,
   stateClearTable,
   stateGetNextCode,
   stateGetMoreBytes,
   stateCheckInbufForCode,
   stateProcessCode,
   stateCheckOutbufForPrefixCode,
   stateBuildTokenString,
   stateCheckOutbufForSuffix,
   stateCheckOutbufForToken,
   stateUndefined,
};

class CUnixCompressEngine : public CObject
{
private:
   static const int m_tokenBufSize = 512;
   static const int m_maximumBits = 16;
   static const unsigned int m_clearCode = 256;
   static const unsigned int m_firstFreeCode = 257;

   unsigned char* m_nextIn;
   unsigned char* m_nextOut;
   int m_availableIn;
   int m_availableOut;

   unsigned int m_oldmaxcode;
   //unsigned int m_oldhashsize;
   int m_maxbits;
   int m_prevBits;
   int m_blockCompress;
   int m_offset;
   int m_size;
   int m_shift;
   unsigned int m_maxCode;
   unsigned int m_saveCode;
   unsigned int m_highCode;
   unsigned int m_nextFree;
   unsigned int m_prefixCode;
   unsigned int m_code;
   char m_suffixChar;
   int m_bits;
   bool m_clearTable;
   bool m_fullTable;
   bool m_codeTypePrefix;
   unsigned char m_inBuf[m_maximumBits];
   unsigned char* m_pInbuf;
   int m_inbufTransferCount;

   char* m_allocatedSuffixTable;
   unsigned short* m_allocatedPrefixTable;
   char* m_suffixTable;
   unsigned short* m_prefixTable;
   //unsigned short* m_ht;
   char* m_tokenBuf;
   int m_tokenCount;
   int m_maxTokenLen;

   int m_numBytesInput;
   int m_numBytesOutput;

   UnixCompressEngineStateTag m_compressEngineState;

public:
   CUnixCompressEngine();
   ~CUnixCompressEngine();

public:
   void setNextIn(unsigned char* nextIn)   { m_nextIn       = nextIn;       }
   void setNextOut(unsigned char* nextOut) { m_nextOut      = nextOut;      }
   void setAvailableIn(int availableIn)    { m_availableIn  = availableIn;  }
   void setAvailableOut(int availableOut)  { m_availableOut = availableOut; }

   int getAvailableIn()         { return m_availableIn; }
   int getAvailableOut()        { return m_availableOut; }
   unsigned char* getNextIn()   { return m_nextIn; }
   unsigned char* getNextOut()  { return m_nextOut; }

   int getNumBytesInput()       { return m_numBytesInput; }
   int getNumBytesOutput()      { return m_numBytesOutput; }

   UnixCompressEngineStatusTag decompress(bool flushFlag);

private:
   bool allocateTables(unsigned int maxcode,unsigned int hashsize);
};

//_____________________________________________________________________________
class CStdioUnixCompressedFileHeader : public CObject
{
private:
   bool m_validFlag;
   static char m_header[];
   static const int m_headerSize;

public:
   CStdioUnixCompressedFileHeader();

   FileStatusTag readHeader(CFile& file);
   bool writeHeader(CFile& file,const CString& fileName);
   bool getValidFlag() { return m_validFlag; }
   int getHeaderSize() { return m_headerSize; }
};

//_____________________________________________________________________________
// see http://www.pkware.com/products/enterprise/white_papers/appnote.html
class CStdioRsiZipFileHeader : public CObject
{
private:
   bool m_validFlag;
   static char m_header[];
   static const int m_headerSize;

public:
   CStdioRsiZipFileHeader();

   FileStatusTag readHeader(CFile& file);
   bool writeHeader(CFile& file,const CString& fileName);
   bool getValidFlag() { return m_validFlag; }
};

//_____________________________________________________________________________
class CStdioGzFileHeader : public CObject
{
private:
   bool m_validFlag;
   unsigned int m_compressedSize;

   static char m_header[];
   static const int m_headerSize;

public:
   CStdioGzFileHeader();

   FileStatusTag readHeader(CFile& file);
   bool writeHeader(CFile& file,const CString& fileName);
   bool getValidFlag() { return m_validFlag; }
   int getCompressedSize() { return m_compressedSize; }
};

//_____________________________________________________________________________
class CStdioPkZipFileHeader : public CObject
{
private:
   bool m_validFlag;
   bool m_validCcFlag;
   int m_headerSize;
   int m_endOfCentralDirectoryRecordSize;

   // local file header
   static const unsigned int m_localFileHeaderSignature = 0x04034b50;
   static const unsigned short m_versionNeededToExtract = 0x14;
   static const unsigned short m_versionMadeBy          = 0;
   static const unsigned short m_generalPurposeBitFlag  = 0x0000;
   static const unsigned short m_compressionMethod      =  8;
   unsigned short m_fileModificationTime;  // dos format 
   unsigned short m_fileModificationDate;  // dos format
   unsigned int m_crc;
   unsigned int m_compressedSize;
   unsigned int m_uncompressedSize;
   unsigned short m_fileNameLength;
   unsigned short m_extraFieldLength;
   unsigned short m_fileCommentLength;
   CString m_fileName;
   CString m_extraField;
   CString m_fileComment;

   // end of central directory record
   static const unsigned int m_endOfCentralDirectorySignature = 0x06054b50;
   unsigned short m_numberOfThisDisk;
   unsigned short m_numberOfTheDiskWithCentralDirectory;
   unsigned short m_numberOfCentralDirectoryEntriesOnThisDisk;
   unsigned short m_numberOfCentralDirectoryEntries;
   unsigned int m_sizeOfCentralDirectory;
   unsigned int m_centralDirectoryOffset;
   unsigned short m_zipFileCommentLength;
   CString m_zipFileComment;

   ULONGLONG m_updatePos;

   // central directory file header
   static const unsigned int m_centralFileHeaderSignature = 0x02014b50;

public:
   CStdioPkZipFileHeader();

   FileStatusTag readHeader(CFile& file);
   bool writeHeader(CFile& file,const CString& fileName);
   bool updateHeader(CFile& file);
   bool writeTrailer(CFile& file,const CString& fileComment,unsigned int crc,
      unsigned int compressedSize,unsigned int uncompressedSize);
   bool getValidFlag() { return m_validFlag; }
   bool getValidCcFlag() { return m_validCcFlag; }
   FileStatusTag readEndOfCentralDirectoryRecord(CFile& file);

   CString getFileName() { return m_fileName; }
   void setFileName(const CString& fileName) { m_fileName = fileName; }
   int getCompressedSize() { return m_compressedSize; }
};

//_____________________________________________________________________________
class CStdioCompressedFile : public CObject
{
protected:
   CString m_archivePath;
   CString m_fileName;
   CFile m_file;

public:
   virtual FileStatusTag openRead(const CString& archivePath) = 0;
   virtual bool openWrite(const CString& archivePath) = 0;
   virtual FileStatusTag readString(CString& string) = 0;
   virtual int getNumBytesInput() = 0;
   virtual int getNumBytesOutput() = 0;
   virtual int getCompressedSize() const = 0;

   virtual CString getArchivePath() { return m_archivePath; }
   virtual void setArchivePath(const CString& archivePath) { m_archivePath = archivePath; }
   virtual CString getFileName() { return m_fileName; }
   virtual void setFileName(const CString& fileName) { m_fileName = fileName; }

   virtual CFile& getFile() { return m_file; }
};

//_____________________________________________________________________________
class CStdioUnixCompressedFile : public CStdioCompressedFile
{
protected:
   int m_inBufSize;
   int m_outBufSize;

   bool m_readMode;
   bool m_writeMode;
   bool m_headerProcessed;
   bool m_eofFlag;
   bool m_eodFlag;
   char* m_inBuf;
   char* m_outBuf;
   CUnixCompressEngine m_compressEngine;
   CStdioUnixCompressedFileHeader m_header;
   unsigned int m_compressedSize;
   unsigned int m_totalCompressedBytesRead;
   unsigned int m_totalLinesRead;

protected:
   //z_stream_s* getStream() { return m_zStream.getStream(); }

public:
   CStdioUnixCompressedFile(int inBufSize=1024,int outBufSize=1024);
   virtual ~CStdioUnixCompressedFile();
   void setCompressedSize(int compressedSize) { m_compressedSize = compressedSize; }

public:
   virtual FileStatusTag openRead(const CString& filePath);
   virtual bool openWrite(const CString& filePath);
   virtual int getNumBytesInput();
   virtual int getNumBytesOutput();

   virtual FileStatusTag readHeader();
   virtual bool isHeaderValid();
   //virtual bool isValidCompressedCcFile() { return isHeaderValid(); }
   virtual FileStatusTag readString(CString& string);
   //virtual bool writeHeader();
   //virtual bool writeString(const CString& string);
   //virtual bool writeRaw(const char* buf,int len);
   //virtual void writeFile(const char* buf,int len);
   //virtual bool flush();
   virtual bool close();

   virtual int getCompressedSize() const;
};

//_____________________________________________________________________________
class CStdioRsiZipFile : public CStdioCompressedFile
{
protected:
   int m_inBufSize;
   int m_outBufSize;

   bool m_readMode;
   bool m_writeMode;
   bool m_headerProcessed;
   bool m_eofFlag;
   bool m_eodFlag;
   char* m_inBuf;
   char* m_outBuf;
   CZStream m_zStream;
   CStdioRsiZipFileHeader m_header;
   unsigned int m_compressedSize;
   unsigned int m_totalCompressedBytesRead;

protected:
   z_stream_s* getStream() { return m_zStream.getStream(); }

public:
   CStdioRsiZipFile(int inBufSize=1024,int outBufSize=1024);
   virtual ~CStdioRsiZipFile();
   void setCompressedSize(int compressedSize) { m_compressedSize = compressedSize; }

public:
   virtual FileStatusTag openRead(const CString& filePath);
   virtual bool openWrite(const CString& filePath);
   virtual int getNumBytesInput();
   virtual int getNumBytesOutput();

   virtual FileStatusTag readHeader();
   virtual bool isHeaderValid();
   virtual bool isValidCompressedCcFile() { return isHeaderValid(); }
   virtual FileStatusTag readString(CString& string);
   virtual bool writeHeader();
   virtual bool writeString(const CString& string);
   virtual bool writeRaw(const char* buf,int len);
   virtual void writeFile(const char* buf,int len);
   virtual bool flush();
   virtual bool close();
   virtual int getCompressedSize() const;
   virtual bool isCrRecordTerminator() const;
};

//_____________________________________________________________________________                     
class CStdioGzFile : public CStdioRsiZipFile
{
private:
   CStdioGzFileHeader m_header;
   unsigned int m_totalUncompressedBytesWritten;
   unsigned long m_crcTable[256];
   unsigned long m_crc;

public:
   CStdioGzFile(int inBufSize=1024,int outBufSize=1024);
   virtual ~CStdioGzFile();

public:
   virtual FileStatusTag readHeader();
   virtual bool isHeaderValid();
   virtual bool writeHeader();
   virtual bool writeRaw(const char* buf,int len);
   //virtual void writeFile(const char* buf,int len);
   virtual bool close();
   virtual bool isCrRecordTerminator() const;

private:
   void createCrcTable();
   void updateCrc(const char* p,int outputLen);
};

//_____________________________________________________________________________                     
class CStdioPkZipFile : public CStdioRsiZipFile
{
private:
   CStdioPkZipFileHeader m_header;
   unsigned int m_totalUncompressedBytesWritten;
   unsigned long m_crcTable[256];
   unsigned long m_crc;

public:
   CStdioPkZipFile(int inBufSize=1024,int outBufSize=1024);
   virtual ~CStdioPkZipFile();

public:
   virtual FileStatusTag readHeader();
   virtual bool isHeaderValid();
   virtual bool isValidCompressedCcFile() { return m_header.getValidCcFlag(); }
   virtual bool writeHeader();
   virtual bool writeRaw(const char* buf,int len);
   //virtual void writeFile(const char* buf,int len);
   virtual bool close();

private:
   void createCrcTable();
   void updateCrc(const char* p,int outputLen);
};

//_____________________________________________________________________________                     
class CStdioCompressedFileWriteFormat : public CWriteFormat
{
private:
   CString m_archivePath;
   CString m_fileName;
   
public:
   CStdioCompressedFileWriteFormat(int bufSize = 160);
   CStdioCompressedFileWriteFormat(const CString& archivePath,int bufSize = 160);
   CStdioCompressedFileWriteFormat(const CString& archivePath,const CString& fileName,int bufSize = 160);
   virtual ~CStdioCompressedFileWriteFormat();

   virtual bool open(const CString& archivePath,const CString& fileName=CString("")) = 0;
   virtual void close() = 0;
   virtual void write(const char* string) = 0;

   CString getArchivePath() { return m_archivePath; }
   void setArchivePath(const CString& archivePath) { m_archivePath = archivePath; }
   CString getFileName() { return m_fileName; }
   void setFileName(const CString& fileName) { m_fileName = fileName; }
};

//_____________________________________________________________________________                     
class CStdioRsiZipFileWriteFormat : public CStdioCompressedFileWriteFormat
{
private:
   CStdioRsiZipFile m_compressedFile;
   
public:
   CStdioRsiZipFileWriteFormat(int bufSize = 160);
   CStdioRsiZipFileWriteFormat(const CString& filePath,int bufSize = 160);
   virtual ~CStdioRsiZipFileWriteFormat();

   virtual bool open(const CString& archivePath,const CString& fileName=CString(""));
   virtual void close();
   virtual void write(const char* string);
};

//_____________________________________________________________________________                     
class CStdioGzFileWriteFormat : public CStdioCompressedFileWriteFormat
{
private:
   CStdioGzFile m_compressedFile;
   
public:
   CStdioGzFileWriteFormat(int bufSize = 160);
   CStdioGzFileWriteFormat(const CString& filePath,int bufSize = 160);
   virtual ~CStdioGzFileWriteFormat();

   virtual bool open(const CString& archivePath,const CString& fileName=CString(""));
   virtual void close();
   virtual void write(const char* string);
};

//_____________________________________________________________________________                     
class CStdioPkZipFileWriteFormat : public CStdioCompressedFileWriteFormat
{
private:
   CStdioPkZipFile m_compressedFile;
   
public:
   CStdioPkZipFileWriteFormat(int bufSize = 160);
   CStdioPkZipFileWriteFormat(const CString& archivePath,int bufSize = 160);
   CStdioPkZipFileWriteFormat(const CString& archivePath,const CString& fileName,int bufSize = 160);
   virtual ~CStdioPkZipFileWriteFormat();

   virtual bool open(const CString& archivePath,const CString& fileName=CString(""));
   virtual void close();
   virtual void write(const char* string);

   virtual void setArchivePath(const CString& archivePath);
   virtual void setFileName(const CString& fileName);
};

#endif
