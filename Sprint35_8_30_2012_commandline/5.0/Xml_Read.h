/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/

#pragma once

#include "data.h"
#include "file.h"
#include "StdioZipFile.h"
#include "xmlrealpartcontent.h"
#include "DcaCamCadFileReader.h"

#define DisableOldXmlReader

class CAMCADDoc;
class CFileReadProgress;

//_____________________________________________________________________________
class CCamCadDcaXmlReader : public CDcaXmlReader
{
private:
   CCEtoODBDoc& m_camCadDoc;

public:
   CCamCadDcaXmlReader(CCEtoODBDoc& camCadDoc,CDcaOperationProgress* fileReadProgress=NULL);
   virtual ~CCamCadDcaXmlReader();

   virtual bool extendedEnt(FileStatusTag &status);
};

//_____________________________________________________________________________
class CCamCadDcaXmlZipReader : public CCamCadDcaXmlReader
{
private:
   CamCadFileTypeTag m_camCadFileType;
   CStdioRsiZipFile* m_zipFile;
   FileStatusTag m_openStatus;

public:
   CCamCadDcaXmlZipReader(CCEtoODBDoc& camCadDoc,CamCadFileTypeTag camCadFileType,CDcaOperationProgress* fileReadProgress=NULL);
   virtual ~CCamCadDcaXmlZipReader();

   virtual FileStatusTag open(const CString& filename);
   virtual FileStatusTag parse();
   virtual FileStatusTag GetLine();

   FileStatusTag getOpenStatus() { return m_openStatus; }
};

//_____________________________________________________________________________
class RealPartXMLReader : public CCamCadDcaXmlReader
{
private:
   CCEtoODBDoc& m_camCadDoc;

public:
   RealPartXMLReader(CCEtoODBDoc& camCadDoc);
   virtual ~RealPartXMLReader();

   virtual FileStatusTag open(const CString buffer);
   //void setDoc(CCEtoODBDoc *doc);
   virtual CDcaXmlContent& getDefinedContent();

protected:
   CString dataBuf;
//   XMLRealPartContent rpContent;
   CMemFile *memFile;
};

//_____________________________________________________________________________
class XMLZipReader : public CCamCadDcaXmlReader
{
private:
   CamCadFileTypeTag m_camCadFileType;
   CStdioRsiZipFile* m_zipFile;
   FileStatusTag m_openStatus;

public:
   XMLZipReader(CCEtoODBDoc& camCadDoc,CamCadFileTypeTag camCadFileType);
   virtual ~XMLZipReader();

   virtual FileStatusTag open(const CString& filename);
   virtual FileStatusTag parse();
   virtual FileStatusTag GetLine();
   //virtual CDcaXmlContent& getDefinedContent();

   FileStatusTag getOpenStatus() { return m_openStatus; }
};

//_____________________________________________________________________________
class CCamCadFileReader : public CObject
{
private:
   CamCadFileTypeTag m_camCadFileType;
   CString m_filePath;
   CString m_openFailureCause;

public:
   CCamCadFileReader();

   CamCadFileTypeTag getCamCadFileType() { return m_camCadFileType; }
   CString getOpenFailureCause() { return m_openFailureCause; }

   FileStatusTag open(const CString& filePath);
   FileStatusTag read(CCEtoODBDoc& camCadDoc);

private:
   bool readNonXmlFile();
};
