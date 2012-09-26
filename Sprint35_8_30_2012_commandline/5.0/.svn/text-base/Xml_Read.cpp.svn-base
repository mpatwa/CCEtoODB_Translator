
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/

///////////////////////////////////////////////////////////////////////////////////
//
// XML rules for a recursive decent parser
// ----------------------------------------
//
// [XML] = [DEFENT][ENTLIST] | [ENTLIST]
//
// [DEFENT] = <?xml version=[VALUE] encoding=[VALUE]?>[COMMENTLIST]
// [ENTLIST] = [ENT][ENTLIST] | [COMMENT][ENTLIST] | empty
// [ENT] = <[ENTBODY][ENT_]
// [ENT_] = /> | >[ENTLIST]</[TAG]>
//
// [COMMENT] = <!--[STR]-->
//
// [ENTBODY] = [TAG][KWVALLIST]
// [KWVALLIST] = [KWVAL][KWVALLIST] | empty
// [KWVAL] = [KEYWORD]=[VALUE]
//
// [TAG] = [SSTR]
// [KEYWORD] = [SSTR]
// [VALUE]  = "[STR]"
//
// [SSTR] = SINGLE STRING
// [STR] = STRING W/ SPACES
//
///////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CcDoc.h"
#include "xml_read.h"
#include "RwLib.h"
#include "DftFacade.h"
#include "CCEtoODB.h"
#include "Encryption/Encryption.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define VALUE_DELIMITERS "<>\""
#define DELIMITERS "<>=\"/ "

static FileStatusTag retVal;
#define CALL(i) { retVal = i; if (retVal != statusSucceeded) return retVal; }

//_____________________________________________________________________________
/******************************************************************************
* RealPartXMLReader
*/
RealPartXMLReader::RealPartXMLReader(CCEtoODBDoc& camCadDoc)
: CCamCadDcaXmlReader(camCadDoc)
, m_camCadDoc(camCadDoc)
{
   memFile = NULL;
}

/******************************************************************************
* ~RealPartXMLReader
*/
RealPartXMLReader::~RealPartXMLReader()
{
   delete memFile;
}

CDcaXmlContent& RealPartXMLReader::getDefinedContent()
{
   if (m_content == NULL)
   {
      m_content = new XMLRealPartContent(m_camCadDoc);
   }

   return *m_content;
}

/******************************************************************************
* setDoc
*/
//void RealPartXMLReader::setDoc(CCEtoODBDoc *doc)
//{
//   content = (XMLContent*)(new XMLRealPartContent());
//   content->SetDoc(doc);
//}

/******************************************************************************
* open
*/
FileStatusTag RealPartXMLReader::open(const CString buffer)
{
   FileStatusTag retval = statusUndefined;

   ERRMsg.Empty();

   dataBuf = buffer;
   if (m_file == NULL)
   {
      //*rcf No data to test this code conversion. It compiles, unknown if it works.
      // This is part of conversion of XML reader to support CCE.
      m_file = new CFlexFile; // was: new CMemFile((BYTE*)(LPCSTR)dataBuf, buffer.GetAllocLength());
      std::stringstream strstrm;
      strstrm << buffer;
      m_file->AttachBuffer( strstrm );
   }

   if (m_file)
   {
      retval = statusInvalidHeader;

      CString header("<realpart");
      CString headerBuf;
      int strLen = header.GetLength();

      if (m_file->Read(headerBuf.GetBufferSetLength(strLen), strLen))
      {
         headerBuf.GetBufferSetLength(header.GetLength());

         if (headerBuf.CompareNoCase(header) == 0)
         {
            m_file->SeekToBegin();

            retval = statusSucceeded;
         }
      }
   }

   return retval;
}


//_____________________________________________________________________________
/******************************************************************************
* XMLZipReaderReader::XMLZipReader
*/
XMLZipReader::XMLZipReader(CCEtoODBDoc& camCadDoc,CamCadFileTypeTag camCadFileType)
: CCamCadDcaXmlReader(camCadDoc)
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

/******************************************************************************
* XMLZipReader::~XMLZipReader
*/
XMLZipReader::~XMLZipReader()
{
   delete m_zipFile;
}

/******************************************************************************
* XMLZipReader::open
*/
FileStatusTag XMLZipReader::open(const CString& filename)
{
   ERRMsg.Empty();

   m_openStatus = m_zipFile->openRead(filename);

   return m_openStatus;
}

/******************************************************************************
* XMLZipReader::parse
*/
FileStatusTag XMLZipReader::parse()
{
   FileStatusTag retval = statusFileOpenFailure;

   ERRMsg.Empty();

   if (getTrackProgress())
   {
      CFileReadProgress* fileReadProgress = new CFileReadProgress(*m_zipFile);
      fileReadProgress->updateStatus(getFileReadProgressStatus());

      setFileReadProgress(fileReadProgress);
   }

   retval = XML();

   m_zipFile->close();

   if (retval != statusSucceeded)
   {
      if (retval != statusUserCancel)
         ErrorMessage(ERRMsg);
   }

   return retval;
}

/******************************************************************************
* XMLZipReader::GetLine
*/
FileStatusTag XMLZipReader::GetLine()
{
   m_lineCnt++;

   if (getFileReadProgress() != NULL && (m_lineCnt % 2000) == 0)
   {
      getFileReadProgress()->updateProgress();
   }

   return m_zipFile->readString(line);
}

//_____________________________________________________________________________
CCamCadDcaXmlReader::CCamCadDcaXmlReader(CCEtoODBDoc& camCadDoc,CDcaOperationProgress* fileReadProgress)
: CDcaXmlReader(camCadDoc.getCamCadData(),fileReadProgress)
, m_camCadDoc(camCadDoc)
{
}

CCamCadDcaXmlReader::~CCamCadDcaXmlReader()
{
}

bool CCamCadDcaXmlReader::extendedEnt(FileStatusTag &status)
{
   bool retval = true;
   status = statusSucceeded; // optimist

   if (curTag == "DFTSolutions")
   {
      CMemFile ndbData(16384);
      //CString tag("<DFTSolutions>\n");
      //ndbData.Write(tag,tag.GetLength());  // BUG This is where the probeNumberOffset is getting lost.
      ndbData.Write(originalElement,originalElement.GetLength());  // This fixes aove bug.

      while (true)
      {
         if (GetLine() != statusSucceeded)
         {
            ERRMsg.Format("Expected ending tag \"</DFTSolutions>\" [line %d]\n",m_lineCnt);
            break;
         }

         ndbData.Write(line,line.GetLength());
         ndbData.Write("\n",1);

         if (line.Trim() == "</DFTSolutions>")
         {
            ndbData.Write("\0",1);
            void* buf = ndbData.Detach();
            bool dftSolnStat = true;

				FileStruct *file = getDefinedContent().getCamCadData().getFileList().GetLastAdded();

            if (file != NULL)
            {
               // true return means success, though does not mean there necessarily was a DFTSoluton.
               // false return means failed, as in XML parsing failed, so abort further read of CCZ.

               /*if (!m_camCadDoc.LoadDFTSolutionXML((const char*)buf, *file))
               {
                  status = statusBadData;
                  ERRMsg.Format("Error reading \"<DFTSolutions>\" section.\n");
               }*/
            }

            free(buf);
            line.Empty();

            break;
         }
      }
   }
   else if (curTag == "MultipleMachines")
   {
      CMemFile ndbData(16384);
      CString tag("<MultipleMachines>\n");
      ndbData.Write(tag,tag.GetLength());

      while (true)
      {
         if (GetLine() != statusSucceeded)
         {
            ERRMsg.Format("Expected ending tag \"</MultipleMachines>\" [line %d]\n",m_lineCnt);
            break;
         }

         ndbData.Write(line,line.GetLength());
         ndbData.Write("\n",1);

         if (line.Trim() == "</MultipleMachines>")
         {
            ndbData.Write("\0",1);
            void* buf = ndbData.Detach();

				FileStruct *file = getDefinedContent().getCamCadData().getFileList().GetLastAdded();

            if (file != NULL)
            {
					//m_camCadDoc.LoadMultipleMachineXML((const char*)buf, *file);
            }

            free(buf);
            line.Empty();

            break;
         }
      }
   }	
	else if (curTag == "Lots")
	{
		// the whole section needs to be read in a variable and saved for writing
	   CMemFile RedLineData(163840);
      CString tag("<Lots>\n");
      RedLineData.Write(tag,tag.GetLength());

      while (true)
      {
         if (GetLine() != statusSucceeded)
         {
            ERRMsg.Format("Expected ending tag \"</Lots>\" [line %d]\n",m_lineCnt);
            break;
         }

         RedLineData.Write(line,line.GetLength());
         RedLineData.Write("\n",1);
			
         if (line.Trim() == "</Lots>")
         {
            RedLineData.Write("\0",1);
            void* buf = RedLineData.Detach();

			   m_camCadDoc.AddLotsXMLData((const char*)buf);
   				
			   free(buf);
			   line.Empty();

            break;
         }
      }
	}
	else if (curTag == "ColorSets")
	{
		// the whole section needs to be read in a variable and saved for writing
		CMemFile ColorSets(163840);
      CString tag("<ColorSets>\n");
      ColorSets.Write(tag,tag.GetLength());

      while (true)
      {
         if (GetLine() != statusSucceeded)
         {
            ERRMsg.Format("Expected ending tag \"</ColorSets>\" [line %d]\n",m_lineCnt);
            break;
         }

         ColorSets.Write(line,line.GetLength());
         ColorSets.Write("\n",1);
			
         if (line.Trim() == "</ColorSets>")
         {
            ColorSets.Write("\0",1);
            void* buf = ColorSets.Detach();

				m_camCadDoc.AddColorSetsXMLData((const char*)buf);
				
				free(buf);
            line.Empty();

            break;
         }
      }
	}
   else
   {
      retval = false;
   }

   return retval;
}

//_____________________________________________________________________________
CCamCadDcaXmlZipReader::CCamCadDcaXmlZipReader(CCEtoODBDoc& camCadDoc,CamCadFileTypeTag camCadFileType,CDcaOperationProgress* fileReadProgress)
: CCamCadDcaXmlReader(camCadDoc,fileReadProgress)
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

CCamCadDcaXmlZipReader::~CCamCadDcaXmlZipReader()
{
   delete m_zipFile;
}

FileStatusTag CCamCadDcaXmlZipReader::open(const CString& filename)
{
   ERRMsg.Empty();

   m_openStatus = m_zipFile->openRead(filename);

   return m_openStatus;
}

FileStatusTag CCamCadDcaXmlZipReader::parse()
{
   FileStatusTag retval = statusFileOpenFailure;

   ERRMsg.Empty();

   if (m_trackProgress)
   {
      m_fileReadProgress = new CFileReadProgress(*m_zipFile);
      m_fileReadProgress->updateStatus(m_fileReadProgressStatus);
   }

   retval = XML();

   m_zipFile->close();

   if (retval != statusSucceeded)
   {
      if (retval != statusUserCancel)
         ErrorMessage(ERRMsg);
   }

   return retval;
}

FileStatusTag CCamCadDcaXmlZipReader::GetLine()
{
   m_lineCnt++;

   if (m_fileReadProgress != NULL && (m_lineCnt % 2000) == 0)
   {
      m_fileReadProgress->updateProgress();
   }

   return m_zipFile->readString(line);
}

//_____________________________________________________________________________
CCamCadFileReader::CCamCadFileReader()
{
   m_camCadFileType = ccFileTypeUndefined;
}

FileStatusTag CCamCadFileReader::open(const CString& filePath)
{
   FileStatusTag retval = statusFileOpenFailure;
   m_filePath = filePath;

   CFile file;
   CExtFileException fileException;

   if (file.Open(m_filePath,CFile::modeRead,&fileException) == 0)
   {
      fileException.getCause();
      CString caption = "Error reading '" + filePath + "'";
	  getApp().LogMessage(caption + "\n");

      //MessageBox(NULL,caption,m_openFailureCaues,MB_ICONEXCLAMATION | MB_OK);
   }
   else
   {
      retval = statusSucceeded;
      CString buf;

      // Grab buffer of 100 chars from start of file, should be enough to ID the content.
      int numRead = file.Read(buf.GetBufferSetLength(100),100);
      buf.ReleaseBuffer(numRead);
		
		int z = buf.Find("<?xml");
		if (z > 0)
			buf = buf.Right(buf.GetLength() - z);

      CString xmlHeader("<?xml ");
      CString xmlRsiZipHeader("ccz\x01");
      CString xmlEncryptHeader("CCZEncrypt");
      CString xmlPkZipHeader("\x50\x4b\x03\x04");
      CString nonXmlHeader("* CAMCAD Data File");

      if (numRead >= xmlHeader.GetLength() && 
          buf.Left(xmlHeader.GetLength()).CompareNoCase(xmlHeader) == 0)
      {
         // Appears to be normal XML but content may actually be encrypted.
         // Check for normal content or encrypted content.

         // Get tokens with XML start tag as the delimiter
         char * pBuffer = (char *)(LPCTSTR)buf;

         // Gets <?xml
         CString nexttoken = CString(strtok (pBuffer, "<"));

         // Get to the root XML node
         nexttoken = CString(strtok (NULL, "<"));

         // Skip any XML comments in between xml decl and root node
         CString xmlComment("!--");
         while( nexttoken.GetLength() > xmlComment.GetLength() &&
            nexttoken.Left(xmlComment.GetLength()).CompareNoCase(xmlComment) == 0 )
            nexttoken = CString(strtok (NULL, "<"));

         // Check if the the root node is CCZEncrypt
         if( numRead >= xmlEncryptHeader.GetLength() && 
            nexttoken.Left(xmlEncryptHeader.GetLength()).CompareNoCase(xmlEncryptHeader) == 0)
         {
            m_camCadFileType = ccFileTypeEncryptedXml;  // Is encrypted content.
            if (!getApp().GetDecryptionAllowed())
               retval = statusDecryptionAccessDenied;
         }
         else
         {
            m_camCadFileType = ccFileTypeXml;  // Is normal content.
         }
      }
      else if (numRead >= xmlRsiZipHeader.GetLength() && 
               buf.Left(xmlRsiZipHeader.GetLength()).CompareNoCase(xmlRsiZipHeader) == 0)
      {
         m_camCadFileType = ccFileTypeXmlRsiZip;
      }
      else if (numRead >= xmlPkZipHeader.GetLength() && 
               buf.Left(xmlPkZipHeader.GetLength()).CompareNoCase(xmlPkZipHeader) == 0)
      {
         m_camCadFileType = ccFileTypeXmlPkZip;
      }
      else if (numRead >= nonXmlHeader.GetLength() && 
               buf.Left(nonXmlHeader.GetLength()).CompareNoCase(nonXmlHeader) == 0)
      {
         m_camCadFileType = ccFileTypeNonXml;
      }
      else 
      {
         m_camCadFileType = ccFileTypeUndefined;
         m_filePath.Empty();

         retval = ((numRead < 1) ? statusFileReadFailure : statusInvalidHeader);
      }

      file.Close();
   }

   return retval;
}
// Miten -- passing the pointer of CCEtoODBDoc which contains all the file related details including the file name
FileStatusTag CCamCadFileReader::read(CCEtoODBDoc& camCadDoc)
{
   FileStatusTag retval = statusUndefined;


   switch (m_camCadFileType)
   {
   case ccFileTypeXml:
      {
         /*bool dontShowProgressBar = ((CCEtoODBApp*)AfxGetApp())->UsingAutomation &&
                                !AfxGetMainWnd()->IsWindowVisible();
         COperationProgress *progress = NULL;
         if(!dontShowProgressBar)
            progress = new COperationProgress;*/

         CCamCadDcaXmlReader reader(camCadDoc);//, progress);
         retval = reader.open(m_filePath);
         
         if (retval == statusSucceeded)
         {
            //reader.setTrackProgress("Reading CAMCAD Data File");
            retval = reader.parse();
         }

         //if(NULL != progress)
            //delete progress;
      }
      break;

   case ccFileTypeEncryptedXml:
      {
         // Decrypt if such has already been enabled. If not enabled, silently do nothing.
         if (getApp().GetDecryptionAllowed())
         {
            CDcaXmlReader reader(camCadDoc.getCamCadData());

            std::stringstream strstrm;

            //Decrypt stream
            CEncryption crypto;

            bool bsts = crypto.DecryptData(m_filePath, strstrm);

            if( !bsts )
               return statusFileDecryptionFailure;

            if( (retval = reader.attachBuffer( strstrm ) ) != statusSucceeded)
			{
				getApp().LogMessage("File decryption failure\n");
				return retval;
			}

			if (strstrm != "")
				getApp().LogMessage("File decrypted successfully\n");

            // Free up stringstream
            strstrm.str("");

			// Miten -- At this point file reading part is over and now the parsing of file will take place
			// Miten -- i.e. constructing the internal data structure will start     
            retval = reader.parse(); // Miten -- parse the entire file after decrypting it
         }
      }
      break;

   case ccFileTypeXmlRsiZip:
   case ccFileTypeXmlPkZip:
      {
         //COperationProgress progress;
         CCamCadDcaXmlZipReader reader(camCadDoc, m_camCadFileType);

         retval = reader.open(m_filePath);

         if (retval == statusSucceeded)
         {
            //reader.setTrackProgress((m_camCadFileType == ccFileTypeXmlRsiZip) ? 
              // "Reading Compressed CAMCAD Data File" : "Reading PKZIP Compressed CAMCAD Data File");
			 getApp().LogMessage("File unzipped successfully\n");
            retval = reader.parse();
         }
      }
      break;

   /*case ccFileTypeNonXml:
      retval = camCadDoc.loadNonXmlCcFile(m_filePath);
      break;*/

   }

   if (retval == statusSucceeded)
   {
      camCadDoc.verifyAndRepairData();
   }

   return retval;
}



