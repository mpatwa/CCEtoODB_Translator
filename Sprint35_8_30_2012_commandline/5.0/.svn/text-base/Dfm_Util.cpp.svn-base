// $Header: /CAMCAD/5.0/Dfm_Util.cpp 28    6/30/07 2:25a Kurt Van Ness $
 
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "crypt.h"                      
#include "lic.h"

#include "drc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char    *AlgTypes[];                  // from DRC.cpp

/******************************************************************************
* OnDfmLoadAttribs
*/
void CCEtoODBDoc::OnDfmLoadAttribs() 
{
#ifdef SHAREWARE
   return;
#endif

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDfm)) 
   {
      ErrorAccess("You do not have a License for DFM!");
      return;
   }*/

   CString path(getApp().getCamcadExeFolderPath());
   path += "dfm\\DFM_Attributes";

   int res = WinExec(path, SW_SHOW);
   switch (res)
   {
      case 0:
         MessageBox(NULL, "The system is out of memory or resources!", "DFM_Attributes", MB_OK | MB_ICONHAND);
      break;
      case ERROR_BAD_FORMAT:
         MessageBox(NULL, "The .EXE file is invalid (non-Win32 .EXE or error in .EXE image)!", "DFM_Attributes", MB_OK | MB_ICONHAND);
      break;
      case ERROR_FILE_NOT_FOUND:
      {
         CString  t;
         t.Format("The specified file [%s] was not found!\n", path);
         MessageBox(NULL, t, "DFM Attributes", MB_OK | MB_ICONHAND);
      }
      break;
      case ERROR_PATH_NOT_FOUND:
      {
         CString  t;
         t.Format("The specified path [%s] was not found!", path);
         MessageBox(NULL, t, "DFM Attributes", MB_OK | MB_ICONHAND);
      }
      break;
   }
}

/******************************************************************************
* dfm_report
*/
static int dfm_report(const char *fname, CCEtoODBDoc *doc, FileStruct *file, int output_units_accuracy)
{
   FILE *fReport = fopen(fname, "wt");
   if (fReport == NULL)
   {
      CString  tmp;

      tmp.Format("Unable to open file [%s] for writing.\nMake sure it is not Read-Only.", fname);
      ErrorMessage(tmp, "Unable to Save Report");
      return 0;
   }

   fprintf(fReport, "DFM Report\n\n");
   fprintf(fReport, "%s\n", getApp().getCompanyNameString());
   fprintf(fReport, "Created by %s\n", getApp().getCamCadTitle());
   CTime t = t.GetCurrentTime();
   fprintf(fReport, "%s\n",t.Format("Date & Time : %A, %B %d, %Y at %H:%M:%S"));
   fprintf(fReport, "\n");

   fprintf(fReport, "Project  : %s\n", doc->GetProjectPath());
   fprintf(fReport, "PCB File : %s\n", file->getName());
   fprintf(fReport, "Units    : %s\n", GetUnitName(doc->getSettings().getPageUnits()));
   fprintf(fReport, "\n");
   

   // loop throu all DRCs.
   POSITION drcPos;
   
   for (int i=0; i<MAX_DRC_ALG;i++)
   {
      int marginal = 0, critical = 0;

      fprintf(fReport, "DFM Test #%d: %s\n", i+1, AlgTypes[i]);

      drcPos = file->getDRCList().GetHeadPosition();
      while (drcPos)
      {
         DRCStruct *drc = file->getDRCList().GetNext(drcPos);
         if (drc->getAlgorithmIndex() == i)
         {
            if (drc->getFailureRange() == 0)
               critical++;
            else if (drc->getFailureRange() == 1)
               marginal++;
         }
      }

      fprintf(fReport, "\tMarginal   Violations : %d\n", marginal);
      fprintf(fReport, "\tCritical   Violations : %d\n", critical);

      fprintf(fReport, "\n");
   }  // run for all DRC tests


   fprintf(fReport, "\n");

   fprintf(fReport, "\nEnd\n");

   fclose(fReport);

   return 1;
}

/******************************************************************************
* OnDfmReporting
*/
void CCEtoODBDoc::OnDfmReporting() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDfm)) 
   {
      ErrorAccess("You do not have a License for DFM!");
      return;
   }*/

	CString dfmfilename = getApp().getUserPath() + "dfm.rep";
   int output_units_accuracy = GetDecimals(getSettings().getPageUnits()); 

   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = getFileList().GetNext(filePos);
      if (file->isShown() && file->getBlockType() == BLOCKTYPE_PCB)
      {
         dfm_report(dfmfilename, this, file, output_units_accuracy);
         Notepad(dfmfilename);
      }
   }
   return;
}

/******************************************************************************
* OnDfmRunner
*/
void CCEtoODBApp::OnDfmRunner() 
{
#ifdef SHAREWARE
   return;
#endif

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDfm)) 
   {
      ErrorAccess("You do not have a License for eSight!");
      return;
   }*/

   CString  path(this->getCamcadExeFolderPath());
   path += "eSight\\eSight_Runner.exe";
	//runTool("eSight Runner", "eSight_Runner", path, "");
}

/******************************************************************************
* OnDfmScripter
*/
void CCEtoODBApp::OnDfmScripter() 
{
#ifdef SHAREWARE
   return;
#endif

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDfm)) 
   {
      ErrorAccess("You do not have a License for eSight!");
      return;
   }*/

   CString  path(this->getCamcadExeFolderPath());
   path += "eSight\\eSight_Scripter.exe";
   //runTool("eSight Scripter", "eSight_Scripter", path, "");
}

