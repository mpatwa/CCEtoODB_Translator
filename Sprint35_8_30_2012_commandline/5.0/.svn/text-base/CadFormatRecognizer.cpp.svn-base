

#include "StdAfx.h"
#include "CadFormatRecognizer.h"
#include "EnumIterator.h"
#include "DbUtil.h"
#include "Mentor.h"

//_________________________________________________________________________________________________
CCadFormatAcceptor::CCadFormatAcceptor(CCadFormatRecognizer& cadFormatRecognizer)
: m_cadFormatRecognizer(cadFormatRecognizer)
{
}

CCadFormatAcceptor::~CCadFormatAcceptor()
{
}

CCadFormatRecognizer& CCadFormatAcceptor::getCadFormatRecognizer()
{
   return m_cadFormatRecognizer;
}

FileTypeTag CCadFormatAcceptor::getFileType() const
{
   return fileTypeUnknown;
}


FileTypeTag CCadFormatAcceptor::getSpecificFileType() const
{
   return fileTypeUnknown;
}

bool CCadFormatAcceptor::accepts(const CString& filePath)
{
   bool retval = false;

   CStdioFile file;
   CFileException fileException;

   if (file.Open(filePath,CFile::modeRead | CFile::shareDenyNone,&fileException))
   {
      retval = accepts(filePath,&file);
   }

   return retval;
}

bool CCadFormatAcceptor::accepts(const CString& filePath,CStdioFile* file)
{
   bool retval = false;

   return retval;
}

int CCadFormatAcceptor::getAssociateFileList(const CString& filePath, CStringArray &filenameList)
{
   // Just put self in list
   filenameList.RemoveAll();
   filenameList.Add(filePath);
   return 1;
}
//_________________________________________________________________________________________________
CCadFormatRecognizer::CCadFormatRecognizer()
: m_directoryFlag(false)
{
}

CCadFormatRecognizer::~CCadFormatRecognizer()
{
}

bool CCadFormatRecognizer::getDirectoryFlag() const
{
   return m_directoryFlag;
}

void CCadFormatRecognizer::setDirectoryFlag(bool flag)
{
   m_directoryFlag = flag;
}

const CStringArray& CCadFormatRecognizer::getFilePaths() const
{
   return m_filePaths;
}

CStringArray& CCadFormatRecognizer::getFilePaths()
{
   return m_filePaths;
}

FileTypeTag CCadFormatRecognizer::determineFileType(const CString& filePath)
{
   FileTypeTag determinedFileType = fileTypeUnknown;

   CStdioFile file;
   CStdioFile* pFile = NULL;
   CFileException fileException;

   if (file.Open(filePath,CFile::modeRead | CFile::shareDenyNone,&fileException))
   {
      pFile = &file;
   }

   for (EnumIterator(FileTypeTag,fileTypeIterator);fileTypeIterator.hasNext();)
   {
      FileTypeTag fileType = fileTypeIterator.getNext();

      CCadFormatAcceptor* cadFormatAcceptor = getCadFormatAcceptor(fileType);

      if (cadFormatAcceptor != NULL)
      {
         if (cadFormatAcceptor->accepts(filePath,pFile))
         {
            determinedFileType = fileType;

            break;
         }
      }
   }

   return determinedFileType;
}

CCadFormatAcceptor* CCadFormatRecognizer::getCadFormatAcceptor(FileTypeTag fileType)
{
   CCadFormatAcceptor* cadFormatAcceptor = NULL;

   if (fileType >= 0 && fileType < m_acceptors.GetSize())
   {
     cadFormatAcceptor = m_acceptors.GetAt(fileType);
   }

   if (cadFormatAcceptor == NULL)
   {
      switch (fileType)
      {
      case fileTypeCr5000Layout:         cadFormatAcceptor = new CCadFormatAcceptorCr5000Layout(*this);  break;
      case fileTypeUnidat:               cadFormatAcceptor = new CCadFormatAcceptorUnidat(*this);        break;
      case fileTypeMentorLayout:         cadFormatAcceptor = new CCadFormatAcceptorMentorLayout(*this, fileTypeMentorLayout);  break;   
      case fileTypeMentorNeutralLayout:  cadFormatAcceptor = new CCadFormatAcceptorMentorLayout(*this, fileTypeMentorNeutralLayout);  break;
      case fileTypeAeroflexNail:         cadFormatAcceptor = new CCadFormatAcceptorAeroflexNail(*this, fileTypeAeroflexNail); break;
      case fileTypeAeroflexNailWire:     cadFormatAcceptor = new CCadFormatAcceptorAeroflexNail(*this, fileTypeAeroflexNailWire); break;
      case fileTypeSchematicNetlist:     cadFormatAcceptor = new CCadFormatAcceptorSchematicNetlist(*this, fileTypeSchematicNetlist); break;
      }

      if (cadFormatAcceptor != NULL)
      {
         m_acceptors.SetAtGrow(fileType,cadFormatAcceptor);
      }
   }

   return cadFormatAcceptor;
}

int CCadFormatRecognizer::getAssociateFileList(FileTypeTag fileType, CString& filePath, CStringArray &filenameList)
{
   int filecnt = 1;
   CCadFormatAcceptor* cadFormatAcceptor = NULL;
   if (fileType >= 0 && fileType < m_acceptors.GetSize())
     cadFormatAcceptor = m_acceptors.GetAt(fileType);

   if(cadFormatAcceptor)
   {
      filecnt = cadFormatAcceptor->getAssociateFileList(filePath, filenameList);
   }

   return filecnt;
}

//_________________________________________________________________________________________________
CCadFormatAcceptorCr5000Layout::CCadFormatAcceptorCr5000Layout(CCadFormatRecognizer& cadFormatRecognizer)
: CCadFormatAcceptor(cadFormatRecognizer)
{
}

CCadFormatAcceptorCr5000Layout::~CCadFormatAcceptorCr5000Layout()
{
}

FileTypeTag CCadFormatAcceptorCr5000Layout::getFileType() const
{
   return fileTypeCr5000Layout;
}

bool CCadFormatAcceptorCr5000Layout::accepts(const CString& filePath,CStdioFile* file)
{
   bool retval = false;

   if (file != NULL)
   {
      CString line;
      file->SeekToBegin();

      while (file->ReadString(line))
      {
         line.Trim();

         if ((line.Compare("(pcf") == 0) || (line.Compare("(pnf") == 0))
         {
            retval = true;

            break;
         }
      }
   }

   return retval;
}

//_________________________________________________________________________________________________
#define QUnidatString1 "UNIDAT_VERSION="

CCadFormatAcceptorUnidat::CCadFormatAcceptorUnidat(CCadFormatRecognizer& cadFormatRecognizer)
: CCadFormatAcceptor(cadFormatRecognizer)
{
}

CCadFormatAcceptorUnidat::~CCadFormatAcceptorUnidat()
{
}

FileTypeTag CCadFormatAcceptorUnidat::getFileType() const
{
   return fileTypeUnidat;
}

bool CCadFormatAcceptorUnidat::accepts(const CString& filePath,CStdioFile* file)
{
   bool retval = false;

   if (file != NULL)
   {
      CString line;
      file->SeekToBegin();

      for (int lineNumber = 1;lineNumber < 20 && file->ReadString(line);lineNumber++)
      {
         line.Trim();

         if (line.Left(strlen(QUnidatString1)).Compare(QUnidatString1) == 0)
         {
            retval = true;

            break;
         }
      }
   }

   return retval;
}

//_________________________________________________________________________________________________
CCadFormatAcceptorMentorLayout::CCadFormatAcceptorMentorLayout(CCadFormatRecognizer& cadFormatRecognizer, FileTypeTag fileType)
: CCadFormatAcceptor(cadFormatRecognizer)
,m_FileType(fileType)
{

    //26 = Mentor Neutral File Read
   m_formatArray[MENFORMAT_NEUTRAL].Add("###Board ");
   m_formatArray[MENFORMAT_NEUTRAL].Add("B_UNITS");

   //12 = Mentor Board Station v8 Read, .prt or geom_ascii
   m_formatArray[MENFORMAT_GEOM].Add("LOCK_WINDOW");
   m_formatArray[MENFORMAT_GEOM].Add("PAGE");

   //12 = Mentor Board Station v8 Read, .prt or geom_ascii
   m_formatArray[MENFORMAT_GEOM1].Add("$$LOCK_WINDOWS");
   m_formatArray[MENFORMAT_GEOM1].Add("$$PAGE");

   //12 = Mentor Board Station v8 Read, .net or net.net  
   m_formatArray[MENFORMAT_NETLIST].Add("NET '");
   m_formatArray[MENFORMAT_NETLIST].Add("NET '");

   //12 = Mentor Board Station v8 Read, .wir or trace.trace
   m_formatArray[MENFORMAT_WIRE].Add("ST ");
   m_formatArray[MENFORMAT_WIRE].Add("XRF ");
   
   //12 = Mentor Board Station v8 Read, .cmp or comp.comp    
   m_formatArray[MENFORMAT_COMP].Add("#REFERENCE PART_NUMBER SYMBOL GEOMETRY BOARD_LOCATION PROPERTIES");

   //12 = Mentor Board Station v8 Read, tech.tech
   m_formatArray[MENFORMAT_TECHNOLOGY].Add("$set_transcript_mode");
   m_formatArray[MENFORMAT_TECHNOLOGY].Add("$$define_technology");

   //12 = Mentor Board Station v8 Read, layer.layer
   m_formatArray[MENFORMAT_LAYER].Add("#name stacking #");
   m_formatArray[MENFORMAT_LAYER].Add("BREAKOUT");

   //12 = Mentor Board Station v8 Read, testpoints.testpoints
   m_formatArray[MENFORMAT_TESTPOINTS].Add("#Fixture Name");
   m_formatArray[MENFORMAT_TESTPOINTS].Add("#X-Loc Y-Loc");

   //12 = Mentor Board Station v8 Read, pins.pins
   m_formatArray[MENFORMAT_PINS].Add("# PIN  Reference-Pin");
   m_formatArray[MENFORMAT_PINS].Add("PIN ");

}

CCadFormatAcceptorMentorLayout::~CCadFormatAcceptorMentorLayout()
{
}

int CCadFormatAcceptorMentorLayout::NormalizedFind(CString source, CString dest)
{
   char *regexp = " ";
   char *replace = "";

   source.Replace(regexp, replace);
   source.MakeUpper();

   dest.Replace(regexp, replace);
   dest.MakeUpper();

   return source.Find(dest);
}

FileTypeTag CCadFormatAcceptorMentorLayout::getFileType() const
{
   return m_FileType;
}

bool CCadFormatAcceptorMentorLayout::accepts(const CString& filePath,CStdioFile* file)
{
   int firstline_limit = MAXLINE_LIMIT;
   int secondline_limit = MAXLINE_LIMIT;

   if (file != NULL)
   {
      CString line1;
      file->SeekToBegin();

      int chkstart = (m_FileType == fileTypeMentorLayout)?0:MENFORMAT_NEUTRAL;
      int chkend = (m_FileType == fileTypeMentorLayout)?MENFORMAT_NEUTRAL:MENFORMAT_NEUTRAL + 1;

      while (file->ReadString(line1) && firstline_limit)
      {

         ULONGLONG filepos = file->GetPosition() - 5;
         for(int fileIdx = chkstart; fileIdx < chkend; fileIdx++)
         {
            int formatcnt = m_formatArray[fileIdx].GetCount();
            CString formatstr = (formatcnt)?m_formatArray[fileIdx].GetAt(0):"";

            int hitcnt = 0;
            if(!formatstr.IsEmpty() && !NormalizedFind(line1,formatstr))
            {
               //found first matching line
               if((++hitcnt) == formatcnt)
                  return true;

               secondline_limit = MAXLINE_LIMIT;
               file->Seek(filepos,CFile::begin);

               //search for next matching
               while ((--secondline_limit))
               {
                  CString line2;
                  //end of line
                  if(!file->ReadString(line2))
                     return false;                     
                  
                  for(int formatIdx = 1, hitcnt = 1; formatIdx < formatcnt; formatIdx++)
                  {
                     formatstr = m_formatArray[fileIdx].GetAt(formatIdx);
                     
                     //found matching line
                     if(!NormalizedFind(line2,formatstr))
                     {
                        if((++hitcnt) == formatcnt)
                           return true;
                        break;
                     }/* if*/

                  }/*for*/

               }/*while*/

            }/* if*/
         }/*for*/

         firstline_limit--; 
      }/*while*/
   }/* if*/

   return false;
}



int CCadFormatAcceptorMentorLayout::getAssociateFileList(const CString& fullPath, CStringArray &fileNames)
{
   CStringArray FileNameList;  
   FileNameList.SetSize(MEN_TOTAL);
   
   CTime tmpModifiedTime = NULL;
   CTime fileTime[MEN_TOTAL];
   memset(fileTime,0,sizeof(fileTime));

   fileNames.RemoveAll();
   int fileCount = 0;

   CFilePath ftfFilePath(fullPath);
   CString fileDirectory = ftfFilePath.getDirectoryPath() + "\\";

   CFileFind finder;
   BOOL bWorking = finder.FindFile(fileDirectory + "*.*");
   CString tmpFullPath = "";

   // Here is what this WHILE loop is doing
   // 1) If the selected file is a Mentor Boardstation file, then search the directory
   //    for all the other five files with the latest date
   // 2) If the selected file is a Mentor Nuetral file, then search the directory 
   //    for the WIRE file with the latest date
   // 2) If the selected file is a WIRE file, then search the directory first for
   //    Mentor Boardstation file.  If no Mentor Boardstation file then search for Mentor Neutral file

   while (bWorking)
   {
      if (tmpFullPath.IsEmpty())
      {
         // First file is the selected file
         tmpFullPath = fullPath;
      }
      else
      {
         bWorking = finder.FindNextFile();
         

         CString nextFileName = finder.GetFileName();             
         BOOL dirFile = finder.IsDirectory();
         if (dirFile)
            continue;

         if (nextFileName[0] == '.')
            continue;

         tmpFullPath = fileDirectory + nextFileName;
         if (!tmpFullPath.CompareNoCase(fullPath))
            continue;   // skip if file is the same as selected file
      }
     
      // check format
      int format = getCadFormatRecognizer().determineFileType(tmpFullPath);
      if (format != Type_Mentor_Layout && format != Type_Mentor_Neutral_Layout)
         continue;

      int mentorFileType = 0;

      FILE *fp;
      if ((fp = fopen(tmpFullPath, "rt")) != NULL)
      {
         finder.GetLastWriteTime(tmpModifiedTime);

         if (m_FileType == Type_Mentor_Layout)
            mentorFileType = tst_mentorbrdfiletype(fp);

         if (m_FileType == Type_Mentor_Neutral_Layout && !FileNameList.GetAt(MEN_NEUT).IsEmpty() && mentorFileType != MEN_WIRE)
         {
            // case 1820.  the file must be closed before continuing. 02/07/06 MAN                 
            fclose(fp);
            continue;
         }
         
         if (mentorFileType && FileNameList.GetAt(mentorFileType).Compare(fullPath) &&
            (fileTime[mentorFileType] == NULL || tmpModifiedTime > fileTime[mentorFileType]))
         {
            fileTime[mentorFileType] = tmpModifiedTime;
            FileNameList.SetAt(mentorFileType,tmpFullPath);
         }         
         else if (format == Type_Mentor_Neutral_Layout && FileNameList.GetAt(MEN_NEUT).Compare(fullPath) &&
                     (fileTime[MEN_NEUT] == NULL || tmpModifiedTime > fileTime[MEN_NEUT]))
         {
            fileTime[MEN_NEUT] = tmpModifiedTime;
            FileNameList.SetAt(MEN_NEUT,tmpFullPath); 
         }

      }
      fclose(fp);
   }
   
   if (m_FileType == Type_Mentor_Layout && !FileNameList.GetAt(MEN_GEOM).IsEmpty())
   {
      int sortFile[] = {MEN_LAYER,MEN_GEOM,MEN_TECHNOLOGY,MEN_COMP,MEN_NETLIST,MEN_WIRE,MEN_PACKAGE,MEN_PINS,MEN_TESTPOINT};
      for(int fidx = 0; fidx < sizeof(sortFile)/sizeof(int*); fidx++)
      {
         if(!FileNameList.GetAt(sortFile[fidx]).IsEmpty())
            fileNames.SetAtGrow(fileCount++, FileNameList.GetAt(sortFile[fidx]));
      }    
   }
   else if (!FileNameList.GetAt(MEN_NEUT).IsEmpty())
   {
      fileNames.SetAtGrow(fileCount++, FileNameList.GetAt(MEN_NEUT));

      if (!FileNameList.GetAt(MEN_WIRE).IsEmpty())
         fileNames.SetAtGrow(fileCount++, FileNameList.GetAt(MEN_WIRE));

   }
          
   return fileCount;
}


//_________________________________________________________________________________________________

CCadFormatAcceptorAeroflexNail::CCadFormatAcceptorAeroflexNail(CCadFormatRecognizer& cadFormatRecognizer, FileTypeTag fileType)
: CCadFormatAcceptor(cadFormatRecognizer)
,m_FileType(fileType)
{
    //AeroflexNail 
   m_formatArray[0].Add("/CVT_VERSION2.0");
   m_formatArray[0].Add("/Filename...");

   //AeroflexNailWir
   m_formatArray[1].Add("|ADAPTORWIRINGSCHEDULE");
   m_formatArray[1].Add("|AdaptorName:");

}

CCadFormatAcceptorAeroflexNail::~CCadFormatAcceptorAeroflexNail()
{
}

FileTypeTag CCadFormatAcceptorAeroflexNail::getFileType() const
{
   return m_FileType;
}

int CCadFormatAcceptorAeroflexNail::getFileCommandIndex(FileTypeTag fileType)
{
   int filecmdIndx = 0;
   switch(fileType)
   {
   case fileTypeAeroflexNail:
      filecmdIndx = 0;
      break;
   case fileTypeAeroflexNailWire:
      filecmdIndx = 1;
      break;
   }
   return filecmdIndx;
}

bool CCadFormatAcceptorAeroflexNail::accepts(const CString& filePath,CStdioFile* file)
{
   if(!file)
      return false;

   bool reval = false;
   CString line= "";
   int matchedLine = 0;
   int linecnt = 30;       // only test max line

   file->SeekToBegin();
   while(file->ReadString(line))
   {
      line.MakeUpper().Trim();
      line.Replace(" ","");

      //test each Aeroflex command
      int cmdIdx = getFileCommandIndex(m_FileType);
      int subcmdIdx = matchedLine;
      int comdLen = m_formatArray[cmdIdx].GetCount();

      for(; subcmdIdx < comdLen; subcmdIdx++)
      {
         CString cmdLines = m_formatArray[cmdIdx].GetAt(subcmdIdx);
         if(line.Find(cmdLines.MakeUpper()) > -1)
         {
            matchedLine++;
            break;
         }
      }//for
      
      //If all commads matched, find the filetype
      if(matchedLine == comdLen)
      {
         reval = true;
         break;
      }      
           
      linecnt--;
      if(!linecnt)
         break;
   }//while

   return reval;
}


//*******************************************************************************************

CCadFormatAcceptorSchematicNetlist::CCadFormatAcceptorSchematicNetlist(CCadFormatRecognizer& cadFormatRecognizer, FileTypeTag fileType)
: CCadFormatAcceptor(cadFormatRecognizer)
,m_FileType(fileType)
,m_specificFileType(fileTypeUnknown)
{
}

CCadFormatAcceptorSchematicNetlist::~CCadFormatAcceptorSchematicNetlist()
{
}

FileTypeTag CCadFormatAcceptorSchematicNetlist::getFileType() const
{
   return m_FileType;
}

FileTypeTag CCadFormatAcceptorSchematicNetlist::getSpecificFileType() const
{
   return m_specificFileType;
}

bool CCadFormatAcceptorSchematicNetlist::accepts(const CString& filePath, CStdioFile* file)
{
   if(!file)
      return false;

   int lineCheckLimiter = 30;  // Max number of lines to check in file (to keep from fully reading immense file that is not matching)

   // Collect the lines into a string array
   CString line;
   CFileBufferStringArray fileBuffer;
   file->SeekToBegin();
   while( (lineCheckLimiter-- > 0) && file->ReadString(line))
   {
      line.MakeUpper().Trim();
      fileBuffer.Add(line);
   }


   // File buffer was stored up-cased so we can use direct compare instead of up-casing all the time.
   // Use upper case form of record content for testing.

   m_specificFileType = fileTypeUnknown;

   if (fileBuffer.Contains("%NET") && fileBuffer.Contains("%PAGE"))
   {
      m_specificFileType = fileTypeVbKeyinNetlist;
   }
   else if (fileBuffer.Contains(".NET") && fileBuffer.Contains(".COMPPIN"))
   {
      m_specificFileType = fileTypeCamcadNetlist;
   }
   else if (fileBuffer.Contains("NET '"))
   {
      m_specificFileType = fileTypeBoardstationNetlist;
   }
   else if (fileBuffer.ContainsLeftJustified("DW ")  // start of data section
      && ((fileBuffer.ContainsLeftJustified("AS ") && fileBuffer.ContainsLeftJustified("AP "))   // good data
      || fileBuffer.ContainsLeftJustified("EW")) )   // end of data section (whether good data found or not)
   {
      m_specificFileType = fileTypeViewlogicNetlist;
   }
   else if (fileBuffer.ContainsLeftJustified("!PADS-POWERPCB-") && fileBuffer.Contains("NETLIST FILE"))
      /// Checking the following stuff too would be nice, but there is a *PARTS* section at top
      /// of file, so depending on part count, the rest of this stuff may be way down there in
      /// the file. So we'll go with the lightweight comment line check above.
      ///fileBuffer.Contains("*PCB*") && 
      ///fileBuffer.Contains("*NET*") && 
      ///fileBuffer.Contains("*SIGNAL*"))
   {
      m_specificFileType = fileTypePadsPowerPcbNetlist;
   }

   return (m_specificFileType != fileTypeUnknown);
}

int CCadFormatAcceptorSchematicNetlist::getAssociateFileList(const CString& fullPath, CStringArray &fileNames)
{
   // Only specific formats are being allowed the multi-file-set treatment.
   // At the moment, only Viewlogic.

   fileNames.RemoveAll();

   if (this->m_specificFileType != fileTypeViewlogicNetlist)
   {
      // Just put the selected file in list, and leave.
      fileNames.Add(fullPath);
   }
   else
   {
      // Multi-file-set treatment

      // Member var will get clobbered by file tests, so save it for reference and to reset later
      FileTypeTag savedSpecificFileType = this->m_specificFileType;

      CFilePath srcFilePath(fullPath);
      CString fileDirectory = srcFilePath.getDirectoryPath() + "\\";

      CFileFind finder;
      BOOL bWorking = finder.FindFile(fileDirectory + "*.*");
      CString tmpFullPath = "";

      while (bWorking)
      {
         if (tmpFullPath.IsEmpty())
         {
            // First file is the selected file
            tmpFullPath = fullPath;
         }
         else
         {
            bWorking = finder.FindNextFile();


            CString nextFileName = finder.GetFileName();             
            BOOL dirFile = finder.IsDirectory();
            if (dirFile)
               continue;

            if (nextFileName[0] == '.')
               continue;

            tmpFullPath = fileDirectory + nextFileName;
            if (!tmpFullPath.CompareNoCase(fullPath))
               continue;   // skip if file is the same as selected file since it is already in the list
         }

         // check format
         CCadFormatAcceptor *schematicAcceptor = getCadFormatRecognizer().getCadFormatAcceptor(fileTypeSchematicNetlist);
         if (schematicAcceptor != NULL && schematicAcceptor->accepts(tmpFullPath) &&
            schematicAcceptor->getSpecificFileType() == savedSpecificFileType)
         {
            fileNames.Add(tmpFullPath);
         }

      }
   }
   
   return fileNames.GetCount();
}


//*******************************************************************************************


bool CFileBufferStringArray::Contains(CString findme)
{
   for (int i = 0; i < this->GetCount(); i++)
   {
      if (this->GetAt(i).Find(findme) > -1)
         return true;
   }

   return false;
}

bool CFileBufferStringArray::ContainsNoCase(CString findme)
{
   findme.MakeUpper();

   for (int i = 0; i < this->GetCount(); i++)
   {
      CString line( this->GetAt(i) );
      line.MakeUpper();

      if (line.Find(findme) > -1)
         return true;
   }

   return false;
}


bool CFileBufferStringArray::ContainsLeftJustified(CString findme)
{
   for (int i = 0; i < this->GetCount(); i++)
   {
      if (this->GetAt(i).Find(findme) == 0)
         return true;
   }

   return false;
}

bool CFileBufferStringArray::ContainsNoCaseLeftJustified(CString findme)
{
   findme.MakeUpper();

   for (int i = 0; i < this->GetCount(); i++)
   {
      CString line( this->GetAt(i) );
      line.MakeUpper();

      if (line.Find(findme) == 0)
         return true;
   }

   return false;
}

//---------------------------------------------------------------