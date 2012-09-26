// $Header: /CAMCAD/4.6/Port.cpp 52    4/02/07 5:35p Lynn Phung $
 
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "filetype.h"
#include "dirdlg.h"
#include "port.h"
#include "gauge.h"
#include "cedit.h"
#include "graph.h"
#include "pan.h"
#include "api.h"
#include "crypt.h"
#include "lyr_lyr.h"
#include "CCEtoODB.h"
#include "AgilentSjPlxIn.h"
#include "CAMCADNavigator.h"
#include "MainFrm.h"
#include <direct.h>
#include <cderr.h>
#include <float.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CCEtoODBView *apiView; // from API.CPP

CProgressDlg* progress = NULL;               
CStringArray fileNames;
int fileCount; 
int currentFile;

void ParseImportFormatString(CString formatString, FormatStruct *format);
void CloseDRCList();


/******************************************************************************
* Export
*/ // Keep
short API::Export(short fileType, LPCTSTR filename) 
{
   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   int retval = 0;//getApp().getCamcadLicense().isFormatLicensedForExport(fileType);
   if (retval != RC_SUCCESS)
   {
      return retval;
   }

   /*if (!strlen(filename))
      doc->FileExport(fileType);*/
   else
   {
      // Get Format
      FormatStruct format;
      format.FileType = fileType;
      InitExportFormat(&format, &doc->getSettings());

      if (!GetExportFormat(&format, &doc->getSettings(), doc))
         return RC_GENERAL_ERROR;

      // Call Writers
      doc->ExportFile(filename, &format);// Miten -- file exporting function is called here
   }

   return RC_SUCCESS;
}


/******************************************************************************
* ExportFormat
*/
short API::ExportFormat(short fileType, LPCTSTR filename, LPCTSTR formatString)
{
   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   /*int retval = getApp().getCamcadLicense().isFormatLicensedForExport(fileType);
   if (retval != RC_SUCCESS)
   {
      return retval;
   }*/

   /*if (!strlen(filename))
      doc->FileExport(fileType);*/
   /*else
   {
      // Get Format
      FormatStruct format;
      format.FileType = fileType;
      InitExportFormat(&format, &doc->getSettings());

      /*if (!doc->GetExportFormatByString(filename, formatString, &format))
         return RC_GENERAL_ERROR;

      // Call Writers
      doc->ExportFile(filename, &format);
   }*/

   return RC_SUCCESS;
}


/******************************************************************************
* OnFileExport
*/
void CCEtoODBDoc::OnFileExport() 
{
#ifndef SHAREWARE

   if (GeometryEditing)
      OnDoneEditing();

   // Get FileType
   int fileType;      

   if (FileExportType(&fileType).DoModal() != IDOK) 
      return;

   /*if (getApp().getCamcadLicense().isFormatLicensedForExport(fileType) == 0)
      FileExport(fileType);*/

#endif
}

/******************************************************************************
* ExportDXF
*/
short API::ExportDXF(LPCTSTR filename, double realWidth, BOOL explode, BOOL exportHidden, BOOL addLineEnds) 
{
   /*int retval = getApp().getCamcadLicense().isFormatLicensedForExport(fileTypeDxf);
   if (retval != RC_SUCCESS)
   {
      return retval;
   }*/

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   FormatStruct format;
   format.FileType = fileTypeDxf;
   InitExportFormat(&format, &doc->getSettings());

   format.AddLineEnds = addLineEnds;
   format.RealWidth = realWidth;
   format.Scale = 1.0;
   format.DXF.Units = doc->getSettings().getPageUnits();
   format.hollow = TRUE;
   format.exporthiddenentities = exportHidden;

   if (explode)
      format.explodeStyle = EXPLODE_ALL; 
   else
      format.explodeStyle = EXPLODE_ROOT;

   // layer to layer map
   Layer_LayerMap lyrmap;
   lyrmap.doc = doc;
   lyrmap.keyword = "DXF";

   // Call Writers
   doc->ExportFile(filename, &format);

   return RC_SUCCESS;
}

/******************************************************************************
* ExportGENCAD
*/
short API::ExportGENCAD(LPCTSTR filename)
{
   /*int retval = getApp().getCamcadLicense().isFormatLicensedForExport(fileTypeGencad);
   if (retval != RC_SUCCESS)
   {
      return retval;
   }*/

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   FormatStruct format;
   format.FileType = Type_GENCAD;
   InitExportFormat(&format, &doc->getSettings());

   format.Version = 1;
   format.CreatePanelFile = 1;

   // Call Writers
   doc->ExportFile(filename, &format, TRUE);

   return RC_SUCCESS;
}
