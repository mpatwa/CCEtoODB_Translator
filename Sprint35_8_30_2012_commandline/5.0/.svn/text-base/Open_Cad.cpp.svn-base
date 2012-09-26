// $Header: /CAMCAD/4.6/Open_Cad.cpp 51    3/29/07 12:28a Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "port.h"
#include "gauge.h"
#include "graph.h"
#include <cderr.h>
#include <direct.h>
#include "open_cad.h"
#include "lic.h"
#include "crypt.h"
#include "api.h"
#include "xml_read.h"
#include "Mentor.h"
#include "mainfrm.h"
#include "CadFormatRecognizer.h"

extern CString CAMCAD_File; // from CAMCAD.CPP
extern CView *activeView; // from CCVIEW.CPP
extern BOOL LoadingDataFile; // CCDOC.CPP
extern CProgressDlg *progress; // from PORT.CPP
extern CStringArray fileNames; // from PORT.CPP
extern int fileCount; // from PORT.CPP
extern int currentFile; // from PORT.CPP

HRESULT __CreateObject(LPOLESTR pszProgID, IDispatch FAR* FAR* ppdisp);
//void FillCurrentSettings(CCEtoODBDoc *doc);

//static int GetFileFormat(DISPID dispid, LPDISPATCH pdispApp, CString fullPath);
// int OpenCadFile_API(CString fullPath, int format);

/******************************************************************************
* CCEtoODBApp::OnOpenCadFile
*/
void CCEtoODBApp::OnOpenCADFile()
{
   CFileDialog FileDialog(TRUE, NULL, "*.*",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK)
      return;

   OpenCadFile(FileDialog.GetPathName());
}

/******************************************************************************
* CCEtoODBApp::OpenCadFile
*/ // Keep
int CCEtoODBApp::OpenCadFile(CString fullPath)
{
   if (!fileExists(fullPath))
   {
      ErrorMessage(fullPath, "File Not Found");
      return RC_FILE_NOT_FOUND;
   }

   /*if (!getSchematicLinkController().requestDeleteOfSchematicLink())
   {
      return RC_NOT_ALLOWED;
   }*/

   // Check for .cc or .ccz type file
   CCamCadFileReader camCadFileReader; // Miten -- Creating an object of the CCFileReader

   // use ecad parser to determine file type
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   _splitpath(fullPath, drive, dir, fname, ext);

   CString fileDirectory = drive;
   fileDirectory += dir;

   FileStatusTag openStatus = camCadFileReader.open(fullPath);

   if (openStatus == statusSucceeded)
   {
      CAMCAD_File = fullPath;
      LoadingDataFile = TRUE;
      OnFileNew();
      LoadingDataFile = FALSE;

      CCEtoODBView *view = (CCEtoODBView*)activeView;

      if (view == NULL)
         return RC_NO_ACTIVE_DOC;

      CCEtoODBDoc *doc = view->GetDocument();
      //doc->SetProjectPathByFirstFile(fileDirectory);
      doc->GenerateSmdComponentTechnologyAttribs(NULL,false);// Miten -- Dont know why do we need this but removing this changes one of the files in the odb output

      //AddToRecentFileList(CAMCAD_File);

      return RC_SUCCESS;
   }
   return RC_NO_ACTIVE_DOC;
}

