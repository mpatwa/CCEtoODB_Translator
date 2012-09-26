// $Header: /CAMCAD/4.6/Project.cpp 20    1/15/07 5:08p Lynn Phung $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-98. All Rights Reserved.

   CCP - project reader, writer !

   CC - see datafile.cpp

*/

#include "stdafx.h"
#include "ccdoc.h"
#include "CCEtoODB.h"
#include "gauge.h"
#include "cedit.h"
#include "ccview.h"
#include "crypt.h"
#include "port.h"

extern CProgressDlg *progress; // from PORT.CPP
extern CView *activeView; // from CCVIEW.CPP
extern CString CAMCAD_File; // from CAMCAD.CPP
extern CStringArray fileNames; // from PORT.CPP
extern int fileCount; // from PORT.CPP
extern int currentFile; // from PORT.CPP

BOOL LoadingProjectFile = FALSE;

void ParseImportFormatString(CString formatString, FormatStruct *format);

/******************************************************************************
* OnSaveProjectFile
*/
void CCEtoODBDoc::OnSaveProjectFile() 
{
   CString FileName;
   CFileDialog FileDialog(FALSE, "CCP", "*.CCP",
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "CAMCAD Project File (*.CCP)|*.CCP|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   FileName = FileDialog.GetPathName();

   FILE *stream;
   if ((stream = fopen(FileName, "wt")) == NULL)
   {
      MessageBox(NULL, FileName, "Unable To Open File", MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   fputs("* CAMCAD Project File\n\n\nSETTINGS\n\n", stream);

   fprintf(stream, "\tPageUnits %s\n", GetUnitName(getSettings().getPageUnits()));
   int decimals = GetDecimals(getSettings().getPageUnits());
   fprintf(stream, "\tPageSize Left=%.*lf Right=%.*lf Bottom=%.*lf Top=%.*lf\n", 
         decimals, getSettings().getXmin(), decimals, getSettings().getXmax(), decimals, getSettings().getYmin(), decimals, getSettings().getYmax());

   fputs("\n\nFILES\n\n", stream);
   fputs(projectFileString, stream);

   fputs("\nPLACEMENT\n\n", stream);

   fclose(stream);
}

/* GetNextLine *
*     - Gets next non-empty line from file 
*     - returns FALSE on end of file
*/
BOOL GetNextLine(CStdioFile *file, CString *line)
{
   if (!file->ReadString(*line))
      return FALSE;

   // skip empty lines
   line->TrimLeft();
   line->TrimRight();
   while (line->IsEmpty())
   {
      if (!file->ReadString(*line))
         return FALSE;
      line->TrimLeft();
      line->TrimRight();
   }
   return TRUE;
}

/* ParseFileStrings *
*     - reads fileType String
*     - reads filenames string
*     - parses filenames, filling fileNames array
*     - reads formatString
*     - returns FALSE on end of file
*/
BOOL ParseFileStrings(CStdioFile *file, CString *fileTypeString, CString *formatString)
{
   CString line;

   /*** Get FileType String ***/
   if (!GetNextLine(file, &line)) return FALSE;
   if (line == "PLACEMENT") return FALSE;
   *fileTypeString = line;


   /*** Get FileNames ***/
   if (!GetNextLine(file, &line)) return FALSE;
   if (line == "PLACEMENT") return FALSE;

   // parse filenames and fill fileNames Array
   fileCount = 0;
   fileNames.RemoveAll();
   char *tok = strtok(line.GetBuffer(0), "|");
   while (tok != NULL)
   {
      CString filename = tok;
      filename.TrimLeft();
      filename.TrimRight();
      if (!filename.IsEmpty())
         fileNames.SetAtGrow(fileCount++, filename);
      tok = strtok(NULL, "|");
   }


   /*** Get FormatString ***/
   if (!GetNextLine(file, &line)) return FALSE;
   if (line == "PLACEMENT") return FALSE;
   *formatString = line;


   return TRUE;
}

/* ParseSettings *
*/
void ParseSettings(CStdioFile *file, CCEtoODBDoc *doc)
{
   CString line;
   while (GetNextLine(file, &line))
   {
      if (line == "SETTINGS")
         continue;

      if (line == "FILES")
         return;
   
      if (line.Left(8) == "PageSize")
      {
         double left, top, right, bottom;
         left = bottom = -1;
         right = top = 1;
         if (sscanf(line, "PageSize Left=%lf Right=%lf Bottom=%lf Top=%lf", &left, &right, &bottom, &top) == 4)
         {
            doc->getSettings().setXmax(right);
            doc->getSettings().setXmin(left);
            doc->getSettings().setYmax(top);
            doc->getSettings().setYmin(bottom);

            doc->SetMaxCoords();
            POSITION viewPos = doc->GetFirstViewPosition();
            while (viewPos)
            {
               CCEtoODBView *view = (CCEtoODBView *)doc->GetNextView(viewPos);

               view->ScaleNum = 1;
               view->ScaleDenom = 1.0;
               view->SetScroll();
            }
         }
      }

      else if (line.Left(9) == "PageUnits")
      {
         CString units = line.GetBuffer(0) + 9;
         units.TrimLeft();
         units.TrimRight();

         if (units == "INCHES")
            doc->getSettings().setPageUnits(pageUnitsInches);
         else if (units == "MILS")
            doc->getSettings().setPageUnits(pageUnitsMils);
         else if (units == "MM")
            doc->getSettings().setPageUnits(pageUnitsMilliMeters);
         else if (units == "HPPLOTTER")
            doc->getSettings().setPageUnits(pageUnitsHpPlotter);
         else if (units == "TNMETER")
            doc->getSettings().setPageUnits(pageUnitsCentiMicroMeters);
      }
   }
}

/******************************************************************************
* LoadProjectFile
*/
void CCEtoODBDoc::LoadProjectFile() 
{
   CStdioFile file;

   // Open File
   if (!file.Open(CAMCAD_File, file.modeRead | file.typeText))
   {
      ErrorMessage(CAMCAD_File, "Unable to Open File!", MB_ICONEXCLAMATION | MB_OK);
      return;
   }


   // Check File Format
   CString line;
   GetNextLine(&file, &line);
   if (line.Compare("* CAMCAD Project File"))
   {
      ErrorMessage("This is Not a CAMCAD Project File", CAMCAD_File, MB_ICONEXCLAMATION | MB_OK);
      return;
   }


   // Parse Settings
   ParseSettings(&file, this);


   CWaitCursor hourglass;
   progress = new CProgressDlg("Importing...", FALSE);
   progress->Create();
   StoreDocForImporting();


   // Parse Files
   CString fileTypeString, formatString;
   while (ParseFileStrings(&file, &fileTypeString, &formatString))
   {
      // Translate fileTypeString
      FileTypeTag fileType = stringToFileTypeTag(fileTypeString);

      if (fileType == fileTypeUnknown)
         continue;

      /*if (getApp().getCamcadLicense().isFormatLicensedForImport(fileType) != 0)
      {
         continue;
      }*/

      // Init FormatStruct
      FormatStruct format;
      format.FileType = fileType;
      //InitImportFormat(&format, &getSettings());

      // Translate formatString
      ParseImportFormatString(formatString, &format);

      // call readers
      for (currentFile = 0; currentFile<fileCount; currentFile++)
      {
         progress->SetPos(0);
         progress->SetStatus(fileNames[currentFile]);
         RestoreWaitCursor();
         /*if (ImportFile(fileNames[currentFile], &format, FALSE) < 1)
            break;*/
      }


		// Only call GenerateSmdComponentTechnologyAttribs() to generate the TECHNOLOGY and SMD attribute
		// if the read is not ODB++ because ODB++ reader already called this function internally
		if (format.FileType != Type_ODB_PP)
         this->GenerateSmdComponentTechnologyAttribs(NULL,false);
   }

   verifyAndRepairData();

   progress->DestroyWindow();
   delete progress;
   progress = NULL;

   FlushQueue();
   //FillCurrentSettings(this); // update Files and Layers in Current Settings


   // Parse Placement
   
   POSITION pos = getFileList().GetHeadPosition();
   while (pos != NULL)
      getFileList().GetNext(pos)->setNotPlacedYet(false);

}

/******************************************************************************
* WriteImportFormatString
*     - Writes formatString according to format and fileType
*/
void WriteImportFormatString(FormatStruct *format, CString *formatString)
{
	CString tmpFormatString;

   switch (format->FileType)
	{
	case fileTypeGerber:
		{
			tmpFormatString.AppendFormat("Type=%d;", format->GR.type);
			tmpFormatString.AppendFormat("Zero=%d;", format->GR.zero);
			tmpFormatString.AppendFormat("Units=%d;", format->GR.units);
			tmpFormatString.AppendFormat("Digits=%d;", format->GR.digits);
			tmpFormatString.AppendFormat("Decimals=%d", format->GR.decimal);
		}
		break;

	default:
		{
			tmpFormatString = "\t(Format )\n";
		}
		break;
	}

   *formatString = tmpFormatString;
}

/******************************************************************************
* ParseImportFormatString
*     - fills FormatStruct by parsing formatString 
*/
void ParseImportFormatString(CString formatString, FormatStruct *format)
{
   char *temp = STRDUP(formatString);
   char *tok;

   tok = strtok(temp, "; \t\n");
   while (tok)
   {
      switch (format->FileType)
      {
      case Type_Gerber:
         {
            int i;

            if (sscanf(tok, "Type=%d", &i))
               format->GR.type = i;

            else if (sscanf(tok, "Zero=%d", &i))
               format->GR.zero = i;

            else if (sscanf(tok, "Units=%d", &i))
               format->GR.units = i;

            else if (sscanf(tok, "Digits=%d", &i))
               format->GR.digits = i;

            else if (sscanf(tok, "Decimals=%d", &i))
               format->GR.decimal = i;
         }
         break;
      }

      tok = strtok(NULL, "; \t\n");
   }
}

// end PROJECT.CPP

