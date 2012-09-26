
/*****************************************************************************/
/* Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/

// Settings.OLDSAVE switches between OLD and New style save
//

#include "stdafx.h"
#include <fcntl.h>
#include <math.h>
#include "CCdoc.h"
#include "attrib.h"                                  
#include "graph.h"
#include "lic.h"
#include "pcbutil.h"
#include "pcbutil2.h"
#include "ijl.h"
#include "xml_read.h"
#include "drc.h"
#include "bgrndbmp.h"
#include "RwLib.h"
#include "RwUiLib.h"
#include "StdioZipFile.h"
#include "EntityNumber.h"
#include "CCEtoODB.h"
#include "DcaCamCadFileWriter.h"
#include "XmlWrite.h"
#include "Consolidator.h"


extern CString       CAMCAD_File; // from CAMCAD.CPP
extern int           NextFileNum; // from GRAPH.CPP
extern CPolyList     *currentPolyList; // from GRAPH.CPP

static long stream_linecnt;

void WriteXML(FILE *stream, CCEtoODBDoc *doc);
void WriteXML(CWriteFormat& writeFormat, CCEtoODBDoc *doc);
void WriteAttribs(CCEtoODBDoc *doc, FILE *stream, CAttributes* map);
void SwapSpecialCharacters(CString *string);
bool fileExists(const CString& path);

/* Defines *****************************************************************/

#define  FNULL       fnull
#define  MAX_LINE    5000  /* Max line length. Must be really long, because linked layer list can be a lot. */
#define  MAX_TOKEN   MAX_LINE

#define  QApplicationName "CAMCAD.EXE"

static   long     ifp_line = 0;                    /* Current line number. */
static   char     token[MAX_TOKEN];                /* Current token.       */
static   FILE     *ferr;
static   int      display_error = 0;
static   FILE     *stream;
static   int      token_name;

/****************************************************************************/
/****************************************************************************/
/*
*/
static   void  convert_special_char(char *t)
{
   CString  n, m;
   unsigned int      i;
   int      esc = 0;

   n = "";
   m = "";

   for (i=0;i<strlen(t);i++)
   {
      if (t[i] == '%')
      {
         if (esc)
         {
            char  tmp[255], *lp;
            strcpy(tmp,m);
            if (lp = strtok(tmp," \t"))
            {
               n += (char)atoi(lp);
               while (lp = strtok(NULL," \t"))
               {
                  n += (char)atoi(lp);
               }
            }
            m = "";
            esc = FALSE;
         }
         else
            esc = TRUE;
      }
      else
      {
         if (esc)
         {
            m += t[i];
         }
         else
         {
            n += t[i];
         }
      }
   }

   strcpy(t, n);
   return;
}

//static void SaveAttribs(FILE *stream, CAttributes* map, CCEtoODBDoc *doc);
//static void LoadAttrib(CCEtoODBDoc *doc, CAttributes** map, FILE *stream);

/*****************************************************************************/
/* 
   do not allow blanks in filename -> convert into ?

   '?' is not allowed in a filename -> therefore use it

*/
static char *no_blank_filename(const char *n)
{
   static   char  name[_MAX_PATH];
   unsigned int   i;
   strcpy(name, n);

   for (i=0;i<strlen(name);i++)
   {
      if (name[i] == ' ')  name[i] = '?';
   }

   return name;
}

// backwards reading a filename
static char *cnv_no_blank_filename(char *name)
{
   unsigned int   i;
   
   for (i=0;i<strlen(name);i++)
   {
      if (name[i] == '?')  name[i] = ' ';
   }

   return name;
}


/*****************************************************************************/
/* 
*/
static char *write_text(const char *s)
{
   static   CString  tmp;
   unsigned int i;

   tmp = "";

   for (i=0;i<strlen(s);i++)
   {
      if (!isprint(s[i]))
      {
         // continue if a character is not printable - or maybe we should do the hex presentation like in EDIF ???
      }
      else
      if (s[i] == '\n')
         tmp += "\\n";
      else
         tmp += s[i];

   }

   return tmp.GetBuffer(0);
}

/******************************************************************************
* CCEtoODBDoc::OnSaveDatafile()
*/
/*void CCEtoODBDoc::OnSaveDatafile() 
{
   SaveDatafile();
}
*/
/******************************************************************************
* CCEtoODBDoc::OnSaveCompressedDatafile
*/
/*void CCEtoODBDoc::OnSaveCompressedDatafile() 
{
   CFilePath ccFilePath(CCFileName);
   ccFilePath.setExtension("ccz");
   ccFilePath.setDelimiterBackSlash();
   CCFileName = ccFilePath.getPath();

   //SaveDatafile(CCFileName);
}*/

/******************************************************************************
* CCEtoODBDoc::OnSaveDatafileAs()
*/
/*void CCEtoODBDoc::OnSaveDatafileAs() 
{
	//SaveDataFileAs();
}*/

/******************************************************************************
* CCEtoODBDoc::SaveDatafile
*/
/*bool CCEtoODBDoc::SaveDatafile()
{
   // By phone call 16 May 07 Mark wants the standard data file changed from
   // cc to ccz.

   // vPlan aka TXP accomodations here.
   // WI 15666, more or less.
   // Doing Save will always save to the vPlan project location, if the loaded CCZ
   // Doc is the one made from starting the the Test Session from vPlan. For all other
   // CCZ Docs the Save is (supposed to be) unchanged.
   CCFileName.Trim();
   CCEtoODBDoc *activeDoc = getActiveDocument();
   if (activeDoc != NULL)
   {
      if (activeDoc->IsTxpTestSessionDoc())
      {
         CCFileName = getApp().m_txpJob.GetResolvedDestinationFilePath();
      }
   }

   CFilePath ccFilePath(CCFileName);
   ccFilePath.setExtension("ccz");
   ccFilePath.setDelimiterBackSlash();
   CCFileName = ccFilePath.getPath();

   return SaveDatafile(CCFileName);
}
*/
/******************************************************************************
* CCEtoODBDoc::SaveDatafile
*/
/*bool CCEtoODBDoc::SaveDatafile(const CString& filePathString) 
{
   CFilePath filePath(filePathString);

   if (filePath.getBaseFileName().IsEmpty() || filePath.getBaseFileName().Left(1) == ".")
   {
      //return SaveDataFileAs();
   }
   else
   {
      // Always write txp test session doc without permission query. WI 15666.
      // All others need overwrite permission.
      bool writeFileFlag = this->IsTxpTestSessionDoc() || !fileExists(filePath.getPath());

      if (!writeFileFlag)
      {
         switch (ErrorMessage("Do you want to overwrite?", filePath.getPath(), MB_YESNOCANCEL))
         {
         case IDYES:
            writeFileFlag = true;
            break;

         case IDNO:
            SaveDataFileAs();
            break;

         case IDCANCEL:
            break;
         }
      }

      if (writeFileFlag)
      {
         CCamCadFileReader reader;
         bool compressFlag = false;

         FileStatusTag openStatus = reader.open(filePath.getPath());
         CamCadFileTypeTag camCadFileType = reader.getCamCadFileType();

         if (openStatus == statusSucceeded)
         {
            compressFlag = ((camCadFileType == ccFileTypeXmlRsiZip) || (camCadFileType == ccFileTypeXmlPkZip));
         }
         else
         {
            compressFlag = (filePath.getExtension().CompareNoCase("ccz") == 0);
         }

         if (compressFlag)
         {
            return SaveCompressedDataFileAs(filePath.getPath(),ccFileTypeXmlPkZip);
         }
         /*else
         {
            //return SaveDataFileAs(filePath.getPath());
         }
      }
		else
			return false;
   }
}
*/
/*bool CCEtoODBDoc::SaveDataFileAs()
{
   int filterIndex = 2;
   CString defaultExtension("ccz");
   CString initialFileName("*.ccz");
   CFilePath ccFilePath(CCFileName);

   if (ccFilePath.getExtension().CompareNoCase("cc") == 0 || 
       ccFilePath.getBaseFileName().CompareNoCase(".cc") == 0)
   {
      filterIndex      = 0;
      defaultExtension = "cc";
      initialFileName  = "*.cc";
   }
   else if (ccFilePath.getExtension().CompareNoCase("ccz") == 0 || 
       ccFilePath.getBaseFileName().CompareNoCase(".ccz") == 0)
   {
      filterIndex      = 2;
      defaultExtension = "ccz";
      initialFileName  = "*.ccz";
   }

   if (ccFilePath.getBaseFileName().IsEmpty() || ccFilePath.getBaseFileName().Left(1) == ".")
   {
      CCFileName.Empty();
   }

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
#ifdef SHAREWARE
      ErrorMessage("To purchase a copy of CAMCAD please contact your reseller or email Rsi_sales@mentor.com", "CAMCAD Shareware does not support File Save!");
      CString file = getApp().getCamcadExeFolderPath();
      file += "order.txt";
      Notepad(file);
#else
      ErrorAccess("You do not have a License to Save a Data File!");
#endif
      return false;
   }

   CFileDialog FileDialog(FALSE,defaultExtension,initialFileName,
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "CAMCAD Data File (*.cc)|*.cc|PKZIP CAMCAD Data File (*.ccz)|*.ccz|All Files (*.*)|*.*||", NULL);

   FileDialog.m_ofn.nFilterIndex = filterIndex;

   if (FileDialog.DoModal() != IDOK) return false;

   CCFileName = FileDialog.GetPathName();

   filterIndex = FileDialog.m_ofn.nFilterIndex;

   if (filterIndex == 2)
   {
      return SaveCompressedDataFileAs(CCFileName,ccFileTypeXmlPkZip);
   }
   else
   {
      //return SaveDataFileAs(CCFileName);
   }
}
*/
/******************************************************************************
* CCEtoODBDoc::SaveCompressedDataFileAs()
*/
/*bool CCEtoODBDoc::SaveCompressedDataFileAs(const CString& archivePathStr,CamCadFileTypeTag camCadFileType)
{
   if (GeometryEditing)
      OnDoneEditing();

   CWaitCursor wait;

   CStdioCompressedFileWriteFormat* writeFormat = NULL;
   CFilePath filePath(archivePathStr);
   filePath.setExtension("cc");

   if (camCadFileType == ccFileTypeXmlRsiZip)
   {
      writeFormat = new CStdioRsiZipFileWriteFormat(8192);
   }
   else
   {
      writeFormat = new CStdioPkZipFileWriteFormat(8192);
   }

   // New as of TXP integration... Previously the whole folder path that is to
   // contain the file needed to exist before attempting to open the file for
   // writing. As of now, we will create the folder path on demand if it does
   // not already exist (assuming the file system allows us to).
   // We don't report errors based on the dir creation, we'll let the
   // open fail and let it get reported that way.
   CString pathFolders( filePath.getDirectoryPath() );
   int res = mkdirtree(pathFolders);

   if (!writeFormat->open(archivePathStr, filePath.getFileName()))
   {
      ErrorMessage(archivePathStr, "Unable To Open File",MB_ICONEXCLAMATION | MB_OK);

		delete writeFormat;
		return false;
   }
   else
   {
      COperationProgress *progress = NULL;
      if (!getApp().SilentRunning)
         progress = new COperationProgress;

      CCamCadDcaCamCadFileWriter camCadFileWriter(*this);

      camCadFileWriter.write(*writeFormat,QApplicationName,getApp().getCamCadTitle(), progress);

      // vPlan aka TXP accomodations here.
      // WI 15666, more or less.
      // Doing Save will always save to the vPlan project location, if the loaded CCZ
      // Doc is the one made from starting the the Test Session from vPlan. So if we
      // just SaveAs'd the TXP job doc, leave the title bar showing the TXP doc, do
      // not update it to the SavedAs doc. For all others we update title bar to
      // SavedAs doc, as usual.
      CString titleBarName(archivePathStr);
      CCEtoODBDoc *activeDoc = getActiveDocument();
      if (activeDoc != NULL)
      {
         if (activeDoc->IsTxpTestSessionDoc())
         {
            titleBarName = getApp().m_txpJob.GetResolvedDestinationFilePath();
         }
      }

      SetTitle(titleBarName);
		docTitle = titleBarName;

      if (progress != NULL)
         delete progress;
   }

   getApp().AddToRecentFileList(archivePathStr);
   delete writeFormat;
	return true;
}
*/
/******************************************************************************
* CCEtoODBDoc::SaveDataFileAs()
*/
/*bool CCEtoODBDoc::SaveDataFileAs(const char *FileName)
{
   if (GeometryEditing)
      OnDoneEditing();

   CWaitCursor wait;

   if ((stream = fopen(FileName, "wt")) == NULL)
   {
      ErrorMessage(FileName, "Unable To Open File", MB_ICONEXCLAMATION | MB_OK);
      return false;
   }


   CStreamFileWriteFormat writeFormat(stream, 8192);

   COperationProgress *progress = NULL;
   CCamCadDcaCamCadFileWriter camCadFileWriter(*this);

   if (!getApp().SilentRunning)
      progress = new COperationProgress;

   camCadFileWriter.write(writeFormat,QApplicationName,getApp().getCamCadTitle(), progress);

   if (progress != NULL)
      delete progress;


   // check for out of space or other write errors.
   if (ferror(stream))
   {
      CString  tmp;
      tmp.Format("Error saving Datafile [%s] -> CC file not valid!", FileName);
      ErrorMessage(tmp, "Disk Write Error");
      fclose(stream);
      UNLINK(FileName); // delete it just in case.
   }
   else
   {
      fclose(stream);

      // vPlan aka TXP accomodations here.
      // WI 15666, more or less.
      // Doing Save will always save to the vPlan project location, if the loaded CCZ
      // Doc is the one made from starting the the Test Session from vPlan. So if we
      // just SaveAs'd the TXP job doc, leave the title bar showing the TXP doc, do
      // not update it to the SavedAs doc. For all others we update title bar to
      // SavedAs doc, as usual.
      CString titleBarName(FileName);
      CCEtoODBDoc *activeDoc = getActiveDocument();
      if (activeDoc != NULL)
      {
         if (activeDoc->IsTxpTestSessionDoc())
         {
            titleBarName = getApp().m_txpJob.GetResolvedDestinationFilePath();
         }
      }

      SetTitle(titleBarName);
      docTitle = titleBarName;
   }

   getApp().AddToRecentFileList(FileName);
   return true;
} // end SaveDataFile()
*/
/***************************************************************************
* 
*/
static int emptyline(const char *l)
{
   CString  tmp;
   tmp = l;
   tmp.TrimLeft();
   tmp.TrimRight();
   return (strlen(tmp) == 0);
}

/***************************************************************************
* 
   Simulates FGETS but allows non printable Control Z and counts line number
*/
static char *FGETS(char *string, int len, FILE *file)
{
   int   c, lastchar = 0;
   int   cnt = 0;

   while (TRUE)
   {
      c = fgetc(file);
      if (c == EOF)  break;
      if (lastchar == 0x0D && c == 0x0A)  break;
      if (lastchar == 0x0A && c == 0x0D)  break;
      if (cnt < len)
      {
         string[cnt] = c;
         cnt++;
      }
      else
      {
         CString  tmp;
         tmp.Format("Read Error near line %d\n", stream_linecnt);
         ErrorMessage(tmp, "CC File Read Error!");
         break;
      }
      lastchar = c;
   }
   stream_linecnt++;
   string[cnt] = '\0';

   // here convert linefeed cr to \n
   if ((lastchar == 0x0D && c == 0x0A) || (lastchar == 0x0A && c == 0x0D))
   {
      string[cnt-1] = '\n';
   }

   if(cnt == 0)   return NULL;

   return string;
}

/***************************************************************************
* LoadDataFile
*/ //Keep
BOOL CCEtoODBDoc::LoadCczDataFile() 
{
   // This is The Real Load CCZ function. 
	// Miten -- Actual File reader. Called by CWinApp::OnFileNew() function
   BOOL retval = false;

   CCFileName = CAMCAD_File;

   CString CCPath = CCFileName;
   CCPath = CCPath.Left(CCPath.ReverseFind('\\')+1);
   setCcPath(CCPath);

   //CWaitCursor wait;
   DontAskRegenPinLocs = FALSE;  // reset
   StoreDocForImporting();

   getWidthTable().empty();

   // Open file and check File Format
   CCamCadFileReader camCadFileReader;
   // Miten == Dont know why are we checking it previously even though we are going to check it here
   FileStatusTag openStatus = camCadFileReader.open(CCFileName);

   if (openStatus == statusFileOpenFailure)
   {
      CString cause = camCadFileReader.getOpenFailureCause();
      CString caption = "Error opening '" + CCFileName + "'";
      ErrorMessage(caption ,cause, MB_ICONEXCLAMATION | MB_OK);
   }
   else if (openStatus == statusFileReadFailure)
   {
      ErrorMessage(CCFileName, "Unable to read file.", MB_ICONEXCLAMATION | MB_OK);
   }
   else if (openStatus == statusInvalidHeader)
   {
      ErrorMessage(CCFileName, "File format not recognized.", MB_ICONEXCLAMATION | MB_OK);
   }
   else if (openStatus == statusDecryptionAccessDenied)
   {
      ErrorMessage("CCE decryption is not enabled.", "Access Denied", MB_ICONEXCLAMATION | MB_OK);
   }
   else if (openStatus == statusSucceeded)
   {
      int original_page_units = getSettings().getPageUnits();
		FileStatusTag readStatus;

      TRY
		{
			readStatus = camCadFileReader.read(*this);
		}
		CATCH (CException, pE)
		{
			ErrorMessage("Error occurred while loading CC file!", "CC File Load");
			FreeDoc();
		   return retval;
		}
		END_CATCH

      bool succeeded = (readStatus == statusSucceeded || readStatus == statusEof);

      retval = succeeded;

      switch (camCadFileReader.getCamCadFileType())
      {
      case ccFileTypeXml:
      case ccFileTypeXmlRsiZip:
      case ccFileTypeXmlPkZip:
      case ccFileTypeEncryptedXml:
         {
            if (!succeeded && readStatus != statusUserCancel)
            {
               formatMessageBoxApp(MB_ICONEXCLAMATION | MB_OK,
                  "Error reading file, '%s'",(const char*)fileStatusTagToString(readStatus));
            }

            //                               old settings         new settings
            double unitfactor = Units_Factor(original_page_units, getSettings().getPageUnits());
            scale_default_units(&getSettings(), unitfactor);
            SortEntities();
				ConvertEtchToSignals();  // Actually applies to Schematic CCZ only.
            ApplyCczInSettings();
         }

         break;

      case ccFileTypeNonXml:
         break;
      }
   }
   else if (openStatus != statusUserCancel)
   {
      formatMessageBoxApp(MB_ICONEXCLAMATION | MB_OK,
         "Error opening file, '%s'",(const char*)fileStatusTagToString(openStatus));
   }

	if (retval)
		OnRegen();

   return retval;
}
// Keep
void CCEtoODBDoc::ApplyCczInSettings()
{
   // Read and Apply the settings file. 
   // Someday maybe separate read and apply, maybe make a settings class to store
   // the settings.
   // For now, there are so few commands and parsing is so simple, and applying the
   // settings is so simple, just do it all here.

   CString settingsFilename( getApp().getImportSettingsFilePath("ccz.in") );

   // Open the ccz.in settings file. This settings file is completely optional.
   // If not present do not issue error or warning, just silently go away.
   FILE  *fp;
   if ((fp = fopen(settingsFilename, "rt")) == NULL)
   {
      // No settings file found. Not an error, not even a warning.
      return;
   }
   else
   {
      // The settings fromm ccz.in get saved into these.
      CTypeChanger MakeInto_CommandProcessor;
      CGeometryConsolidatorControl ConsolidatorControl;

      // Do the parsing.
      char *lp;
      char  line[255];
      while (fgets(line, 255, fp))
      {
         CString w = line;
         if ((lp = get_string(line, " \t\n")) == NULL)
            continue;

         if (lp[0] == '.')
         {
            if (ConsolidatorControl.Parse(line))
            {
               // Nothing more to do, Parse() already did it if it was a Consolidator command.
            }
            else if (MakeInto_CommandProcessor.ParseCommand(line))
            {
               // Nothing more to do, ParseCommand() already did it if it was a MakeInto command.
            }
            else
            {
               // Not recognized command, ignore it.
            }
         }
      }

      // Apply the settings.
      // Do MakeInto first, it might change nature of some objects. Could lead to improper consolidation
      // if not done first.
      MakeInto_CommandProcessor.Apply(this);

      // Consolidate
      if (ConsolidatorControl.GetIsEnabled())
      {
         CGeometryConsolidator squisher(this, "" /*no report*/);
         squisher.Go( &ConsolidatorControl );
      }

   }
}