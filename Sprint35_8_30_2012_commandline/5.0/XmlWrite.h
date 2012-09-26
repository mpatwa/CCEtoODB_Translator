// $Header: /CAMCAD/5.0/XmlWrite.h 1     6/30/07 1:45a Kurt Van Ness $

#if !defined(__XmlWrite_h__)
#define __XmlWrite_h__

#pragma once

#include "DcaCamCadFileWriter.h"

class CCEtoODBDoc;

//_____________________________________________________________________________
class CCamCadDcaCamCadFileWriter : public CDcaCamCadFileWriter
{
private:
   CCEtoODBDoc& m_camCadDoc;

public:
   CCamCadDcaCamCadFileWriter(CCEtoODBDoc& camCadDoc);
   virtual ~CCamCadDcaCamCadFileWriter();

   CCEtoODBDoc& getCamCadDoc() { return m_camCadDoc; }

private:
   virtual void writeExtendedXmlAttributes(CWriteFormat& writeFormat,const FileStruct&        fileStruct,CCamCadFileWriteProgress& progress);
   virtual void writeExtendedXml          (CWriteFormat& writeFormat,const FileStruct&        fileStruct,CCamCadFileWriteProgress& progress);
};

#endif
