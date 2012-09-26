// $Header: /CAMCAD/4.6/Port_Lib.cpp 142   5/01/07 3:40p Rick Faltersack $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "gauge.h"
#include "filetype.h"
#include <cderr.h>
#include <float.h>
#include "format.h"
#include "formathp.h"
#include "mentor.h"
#include "cedit.h"
#include "graph.h"
#include "ccview.h"
#include "port.h"
#include <direct.h>
#include "lyr_lyr.h"
#include "pcbutil.h"
#include "lyr_file.h"
#include "lyr_pen.h"
#include "hp5dx.h"
#include "pan.h"
#include "dirdlg.h"
#include "lic.h"
#include "DFT.h"
#include "CadFormatRecognizer.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// E.g. 4.6 becomes 406, 4.10 becomes 410
#define CamCadMajorMinorVersion ((CamCadMajorVersion *100) + CamCadMinorVersion)

FileStruct *currentPCBFile = NULL;

extern CFileLayerList *FileLayerList; // Lyr_File.cpp
extern CProgressDlg *progress; // from PORT.CPP
extern CStringArray fileNames; // from PORT.CPP
extern int fileCount; // from PORT.CPP
extern int currentFile; // from PORT.CPP

void CloseDRCList();

static char logFile[_MAX_PATH];
static char macroFile[_MAX_PATH];
static char apPrefix[MAXPREFIX];

static CString includeFile, ruleFile, boardDefaultFile, nameCheckFile, HP3070LogFile, HP_Directory, HP_Panelname;

// Miten -- function to look out for while exporting and file
// Keep
/* ExportFile *****************************************************************/
void CCEtoODBDoc::ExportFile(CString fileName, FormatStruct *format, BOOL run_silent)
{
#ifndef SHAREWARE // do not do anything in ExportFile

   switch (format->FileType)
   {
   case fileTypeOdbPlusPlus: // Miten -- case to to call ODB write function
      ODB_WriteFiles(fileName, this, format, getSettings().getPageUnits());
      break;
   }
#endif
}

/******************************************************************************
* InitExportFormat
*
*  DESCRIPTION -
*     - fills FormatStruct with defaults
*/
void InitExportFormat(FormatStruct *format, SettingsStruct *settings)
{
   format->PortFileUnits = settings->getPageUnits(); // default to pageUnits
   format->RealWidth = 0.01;// * PageUnit_2_OutputFactor(UNIT_INCHES, Format.PortFileUnits);
   format->Scale = 1.0;
   format->filename[0] = '\0';
}

/******************************************************************************
* GetExportFormat
*
*  DESCRIPTION -
*     - Executes appropriate format dialog based on selected File Type
*     - Fills FormatStruct
*/
BOOL GetExportFormat(FormatStruct *format, SettingsStruct *settings, CCEtoODBDoc *doc)
{
   bool retval = true;
   return retval;
}