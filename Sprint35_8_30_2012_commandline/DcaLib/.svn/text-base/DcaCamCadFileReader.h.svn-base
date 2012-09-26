
#if !defined(__DcaCamCadFileReader_h__)
#define __DcaCamCadFileReader_h__

#pragma once

#include "DcaCollections.h"

enum FileStatusTag;
enum DataTypeTag;

class CCamCadData;
class CDcaXmlContent;
class CDcaOperationProgress;
class CStdioRsiZipFile;

#define SUCCESS   0
#define FAILURE   1

#define BUFSIZE   500

//_____________________________________________________________________________
enum CamCadFileTypeTag
{
   ccFileTypeXml,
   ccFileTypeEncryptedXml,
   ccFileTypeXmlRsiZip,
   ccFileTypeXmlPkZip,
   ccFileTypeNonXml,
   ccFileTypeUndefined
};

//======================================================================================
//
// CFlexFile  -- Snatched from CCZ Data DLL as-is.
//

// Memory stream
#include <sstream>
#include <iostream>
#include <fstream>

//deadcode #define BUFSIZE 512

typedef enum enumFileType { 
    kCFileType_Stdio, 
    kCFileType_Memory
} eFileType;

class CFlexFile
{
public:
    CFlexFile( eFileType ft = kCFileType_Memory );
    ~CFlexFile();

    eFileType   m_CFileType;

    BOOL        Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pException = NULL);
    void        Close();
    // Read
    void        SeekToBegin();
    ULONGLONG   GetLength() const;
    ULONGLONG   GetPosition() const;
    BOOL        ReadString(CString& rString);
    UINT        Read(void* lpBuf, UINT nCount);
    // Mem
    bool        AttachBuffer( const std::stringstream& fileString );

//*rcf !!! protected:
    CString             m_openedFileName;
    int                 m_openedFlags;

    CStdioFile*         m_pStdioFile;
#ifdef USE_PTR_2_STREAM
    std::stringstream*  m_pMemFileStrm;
#else
    std::stringstream   m_MemFileStrm;
#endif
};

//======================================================================================

//_____________________________________________________________________________
class CDcaXmlReader ///XMLReader
{
private:
   CCamCadData& m_camCadData;

protected:
   CDcaXmlContent* m_content;  ///XMLContent* content;
   CString fileName, fileBuf;
   CFlexFile* m_file;
   UINT m_flags;
   ULONGLONG m_lastFilePos;
   long m_lineCnt;
   long m_charCnt;
   CString line, originalElement, curElement, curTok;

   CString curTag;
   CString curKeyword;
   CString curValue;

   CMapStringToString kwvalMap;
   bool m_trackProgress;
   CDcaOperationProgress* m_fileReadProgress;
   CString m_fileReadProgressStatus;

public:
   CString ERRMsg;

public:
   CDcaXmlReader(CCamCadData& camCadData,CDcaOperationProgress* fileReadProgress=NULL);
   virtual ~CDcaXmlReader();

   CCamCadData& getCamCadData() { return m_camCadData; }

   bool getTrackProgress() const { return m_trackProgress; }

   CDcaOperationProgress* getFileReadProgress() { return m_fileReadProgress; }
   void setFileReadProgress(CDcaOperationProgress* fileReadProgress);

   CString getFileReadProgressStatus() const { return m_fileReadProgressStatus; }

   virtual FileStatusTag attachBuffer(const std::stringstream& fileString);
   virtual FileStatusTag open(const CString& filename);
   virtual FileStatusTag parse();
   virtual FileStatusTag GetLine();
   virtual FileStatusTag GetElement();
   virtual FileStatusTag checkFileHeader();
   virtual void setTrackProgress(const CString& status);
   virtual CDcaXmlContent& getDefinedContent();
   virtual bool extendedEnt(FileStatusTag &status);

protected:
   virtual FileStatusTag XML();
   
   virtual FileStatusTag DEFENT();
   virtual FileStatusTag ENTLIST();
   virtual FileStatusTag ENT();
   
   virtual FileStatusTag ENTBODY();
};

//_____________________________________________________________________________
class CDcaXmlZipReader : public CDcaXmlReader
{
private:
   CamCadFileTypeTag m_camCadFileType;
   CStdioRsiZipFile* m_zipFile;
   FileStatusTag m_openStatus;

public:
   CDcaXmlZipReader(CCamCadData& camCadData,CamCadFileTypeTag camCadFileType,CDcaOperationProgress* fileReadProgress=NULL);
   virtual ~CDcaXmlZipReader();

   virtual FileStatusTag open(const CString& filename);
   virtual FileStatusTag parse();
   virtual FileStatusTag GetLine();

   FileStatusTag getOpenStatus() { return m_openStatus; }
};

//_____________________________________________________________________________


#endif
